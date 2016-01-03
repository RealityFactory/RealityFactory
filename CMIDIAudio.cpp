/************************************************************************************//**
 * @file CMIDIAudio.cpp
 * @brief MIDI Audio Playback Class
 *
 * This file contains the class implementation for the CMIDIAudio MIDI file
 * playback class.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#include "RabidFramework.h"
#include "CFileManager.h"
#include "CMIDIAudio.h"

/* ------------------------------------------------------------------------------------ */
// Constructor
//
// Open up the MIDI sequencer via MCI
/* ------------------------------------------------------------------------------------ */
CMIDIAudio::CMIDIAudio() :
	m_bLooping(false),		// No play, no loop
	m_bActive(true),		// Assume we're up running
	m_mciDeviceID(-1),		// Device ID from open
	m_ListCount(0),			// No list of files to play
	m_szList(NULL),			// None, really!
	m_Position(0),			// No playlist position
	m_saveloop(false)
{
	m_savefile[0]	= 0;	// No file yet
	m_MIDIFile[0]	= 0;	// No file yet

	// All done.  The way the MCI sequencer works, there's no mechanism for loading
	// ..in a new MIDI file without "opening" it anew, so we'll just open/close as
	// ..part of the start/stop cycle.
}

/* ------------------------------------------------------------------------------------ */
// Destructor
//
// Shut down the sequencer and clean up.
/* ------------------------------------------------------------------------------------ */
CMIDIAudio::~CMIDIAudio()
{
	MCI_GENERIC_PARMS mciClose;
	MCIERROR theError;

	if(!IsPlaying())
		return;								// Not playing means no shutdown needed

	mciClose.dwCallback = NULL;				// Please, no notifications

	Stop();									// Stop playback

	theError = mciSendCommand(m_mciDeviceID, MCI_CLOSE, MCI_WAIT, (DWORD)&mciClose);	// Shut it down!

	if(theError != 0)
		CCD->Log()->Warning("CMIDIAudio: Failed to close MCI MIDI SEQUENCER device");
}

/* ------------------------------------------------------------------------------------ */
// Play
//
// Play back a General MIDI file, using the default MIDI device as
// ..opened in the constructor.  Note that ONLY General MIDI files
// ..are assumed, and that it's assumed that the user has a General
// ..MIDI-compatible sound card installed.
/* ------------------------------------------------------------------------------------ */
int CMIDIAudio::Play(const std::string& szFile, bool bLoop)
{
	std::string szTemp = CFileManager::GetSingletonPtr()->GetDirectory(kMIDIFile);
	szTemp += "\\" + szFile;

	// Ok, we're opening the MIDI file as part of the sequencer open.  This
	// ..is the simplest way to do it.  The output device that gets opened
	// ..is the default sequencer as set in the Multimedia control panel.

	MCI_OPEN_PARMS mciOpen;

	mciOpen.lpstrDeviceType = "sequencer";

	// Ok, we're opening the MIDI file as part of the sequencer open.  This
	// ..is the simplest way to do it.  The output device that gets opened
	// ..is the default sequencer as set in the Multimedia control panel.

	mciOpen.lpstrElementName = szTemp.c_str();

	if(mciSendCommand(NULL, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT, (DWORD)(LPVOID) &mciOpen))
	{
		// Blew the file open, return an ERROR.
		CCD->Log()->Warning("File %s - Line %d: Failed to open sequence file '%s'",
							__FILE__, __LINE__, szFile.c_str());
		return RGF_FAILURE;
	}

	m_mciDeviceID = mciOpen.wDeviceID;			// Save device ID for later work.

	// Ok, the file is opened and the sequencer set up.  Let's play the file!

	MCI_PLAY_PARMS mciPlay;
	memset(&mciPlay, 0, sizeof(MCI_PLAY_PARMS));

	if(mciSendCommand(m_mciDeviceID, MCI_PLAY, MCI_NOTIFY, (DWORD)(LPVOID)&mciPlay))
	{
		// Can't play it?  Damn!
		CCD->Log()->Warning("File %s - Line %d: Failed to play sequence file '%s'",
							__FILE__, __LINE__, szFile.c_str());
		MCI_GENERIC_PARMS mciClose;
		mciClose.dwCallback = NULL;							// Please, no notifications
		mciSendCommand(m_mciDeviceID, MCI_CLOSE, MCI_WAIT, (DWORD)&mciClose);
		m_mciDeviceID = (-1);
		return RGF_FAILURE;
	}

	m_saveloop = m_bLooping = bLoop;						// Set loopyness

	strcpy(m_MIDIFile, szFile.c_str());						// Save filename

	strcpy(m_savefile, szFile.c_str());

	m_bActive = true;

	CCD->Log()->Debug("Loaded " + szFile);

	// File playing, let's BAIL THIS MESS!
	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// Restore
/* ------------------------------------------------------------------------------------ */
void CMIDIAudio::Restore()
{
	if(m_savefile[0] != 0)
		Play(m_savefile, m_saveloop);
}

/* ------------------------------------------------------------------------------------ */
// PlayList
//
// Play a list of MIDI files.
/* ------------------------------------------------------------------------------------ */
int CMIDIAudio::PlayList(char **szList, int nCount)
{
	m_MIDIFile[0] = 0;				// No single file playback here

	// Allocate space for the array of pointers
	m_szList = new char*[nCount];

	if(m_szList == NULL)
	{
		// Serious bad news, memory won't allocate
		CCD->Log()->Warning("CMIDIAudio: playlist alloc fail #1");
		return RGF_FAILURE;
	}

	for(int nTemp=0; nTemp<nCount; ++nTemp)
	{
		m_szList[nTemp] = new char[strlen(szList[nTemp])+2];

		if(m_szList[nTemp] == NULL)
		{
			// Worse and worse!
			CCD->Log()->Warning("CMIDIAudio: playlist alloc fail #2");
			return RGF_FAILURE;
		}

		strcpy(m_szList[nTemp], szList[nTemp]);
	}

	// Playlist allocated, let's get going...
	m_ListCount = nCount;					// # of tunes to play
	m_Position = 0;							// Which tune we're on

	Play(szList[0], false);					// Start me up!

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// Stop
//
// Stop the currently playing MIDI file.
/* ------------------------------------------------------------------------------------ */
int CMIDIAudio::Stop()
{
	MCI_GENERIC_PARMS mciClose;
	MCIERROR theError;

	m_MIDIFile[0] = 0;
	m_bLooping = false;								// Whatever else, looping stops!

	m_Position = 0;									// No song playing...
	m_ListCount = 0;								// ..and I mean it!

	if(!IsPlaying())
		return RGF_SUCCESS;							// Not playing, no need to stop

	mciClose.dwCallback = NULL;						// Please, no notifications
	theError = mciSendCommand(m_mciDeviceID, MCI_CLOSE, MCI_WAIT, (DWORD)&mciClose);	// Shut it down!

	if(theError != 0)
		CCD->Log()->Warning("CMIDIAudio: Failed to stop MCI MIDI SEQUENCER device");

	m_mciDeviceID = (-1);

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// IsPlaying
//
// Return TRUE if there's a MIDI file running.
/* ------------------------------------------------------------------------------------ */
bool CMIDIAudio::IsPlaying()
{
	MCI_STATUS_PARMS mciState;
	MCIERROR theError;

	if(m_mciDeviceID < 0)
		return false;										// No MIDI device open.

	mciState.dwItem = MCI_STATUS_MODE;

	theError = mciSendCommand(m_mciDeviceID, MCI_STATUS,
		MCI_WAIT | MCI_STATUS_ITEM, (DWORD)&mciState);

	if(theError != 0)
		return false;										// Likely, device not open.

	if(mciState.dwReturn == MCI_MODE_PLAY)
		return true;										// MIDI is playing

	return false;											// MIDI not playing
}

/* ------------------------------------------------------------------------------------ */
// Check
//
// This function takes care of checking the MIDI sequencer periodically
// ..to see if the file is still playing.  I'd use notifications but I've
// ..found those to be somewhat unreliable, THIS I know works.
/* ------------------------------------------------------------------------------------ */
void CMIDIAudio::Check()
{
	if(m_mciDeviceID < 0)
		return;								// No MIDI file open, ignore call

	if(IsPlaying())
		return;								// Playing, don't disturb it.

	// Check for a looping MIDI, if so, reposition and restart.
	if(m_bLooping)
	{
		MCI_SEEK_PARMS mciSeek;
		MCI_PLAY_PARMS mciPlay;
		memset(&mciSeek, 0, sizeof(mciSeek));
		mciSeek.dwTo = 1;

		if(mciSendCommand(m_mciDeviceID, MCI_SEEK, MCI_WAIT | MCI_SEEK_TO_START,
			(DWORD)(LPMCI_SEEK_PARMS)&mciSeek))
		{
			CCD->Log()->Warning("CMIDIAudio: Failed to loop to start of MIDI File");
			return;
		}

		memset(&mciPlay, 0, sizeof(MCI_PLAY_PARMS));

		if(mciSendCommand(m_mciDeviceID, MCI_PLAY, MCI_NOTIFY, (DWORD)(LPVOID)&mciPlay))
		{
			MCI_GENERIC_PARMS mciClose;
			// Can't restart it?  Damn!
			CCD->Log()->Warning("CMIDIAudio: Failed to restart looping MIDI file");
			mciClose.dwCallback = NULL;							// Please, no notifications
			mciSendCommand(m_mciDeviceID, MCI_CLOSE, MCI_WAIT, (DWORD)&mciClose);
			m_bLooping = false;
			return;
		}

		return;									// MIDI looped, or we blew up somewhere
	}

	// If we have a MIDI playlist, we need to close the device and start
	// ..the next one up!

	Stop();										// Close off the device

	if(++m_Position > m_ListCount)
	{
		// Playlist complete, free memory and do clean-up
		m_Position = 0;

		for(int nTemp=0; nTemp<m_ListCount; ++nTemp)
			delete m_szList[nTemp];

		delete m_szList;
		m_ListCount = 0;
		return;
	}

	// Otherwise, play the next song in the ol' jukebox....
	Play(m_szList[m_Position], false);			// Start the next song.
}

/* ------------------------------------------------------------------------------------ */
// SaveTo
//
// Save the current MIDI playback state to the supplied file
/* ------------------------------------------------------------------------------------ */
int CMIDIAudio::SaveTo(FILE *SaveFD)
{
	fwrite(&m_bLooping,		sizeof(bool),	1, SaveFD);
	fwrite(&m_bActive,		sizeof(bool),	1, SaveFD);
	fwrite(&m_ListCount,	sizeof(int),	1, SaveFD);

	for(int nTemp = 0; nTemp < m_ListCount; ++nTemp)
	{
		int len = strlen(m_szList[nTemp])+1;
		fwrite(&len, sizeof(int), 1, SaveFD);
		fwrite(m_szList[nTemp], 1, len, SaveFD);
	}

	fwrite(&m_Position, sizeof(int), 1, SaveFD);
	fwrite(&m_MIDIFile, 1, 256, SaveFD);

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// RestoreFrom
//
// Restore the MIDI playback state from the supplied file
/* ------------------------------------------------------------------------------------ */
int CMIDIAudio::RestoreFrom(FILE *RestoreFD)
{
	int nTemp;
	Stop();						// Shut it all down.

	for(nTemp=0; nTemp<m_ListCount; ++nTemp)
	{
		delete m_szList[nTemp];
		m_szList[nTemp] = NULL;
	}

	delete m_szList;
	m_szList = NULL;

	fread(&m_bLooping,	sizeof(bool),	1, RestoreFD);
	fread(&m_bActive,	sizeof(bool),	1, RestoreFD);
	fread(&m_ListCount, sizeof(int),	1, RestoreFD);

	if(m_ListCount != 0)
		m_szList = new char*[m_ListCount];
	else
		m_szList = NULL;

	for(nTemp=0; nTemp<m_ListCount; ++nTemp)
	{
		int len;
		fread(&len, sizeof(int), 1, RestoreFD);
		m_szList[nTemp] = new char[len];
		fread(m_szList[nTemp], 1, len, RestoreFD);
	}

	fread(&m_Position, sizeof(int), 1, RestoreFD);
	fread(&m_MIDIFile, 1, 256, RestoreFD);

	// Ok, data in, start up MIDI again if it was playing
	if(m_bActive)
	{
		if(m_MIDIFile[0] != 0)
			Play(m_MIDIFile, m_bLooping);
		else if(m_ListCount != 0)
			Play(m_szList[m_Position], m_bLooping);
	}

	return RGF_SUCCESS;
}

/* ----------------------------------- END OF FILE ------------------------------------ */
