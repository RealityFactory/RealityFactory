/************************************************************************************//**
 * @file CCDAudio.cpp
 * @brief CD Audio Playback Class
 *
 * This file contains the class implementation for the CCDAudio CD Audio playback class.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#include "RabidFramework.h"
#include "CCDAudio.h"

/* ------------------------------------------------------------------------------------ */
// MciWndProc
// Constructor
//
// Set up default values, connect to MCI, open the CD player
/* ------------------------------------------------------------------------------------ */
CCDAudio::CCDAudio() :
	m_PlayCount(0),						// No active tracks
	m_bLooping(false),					// Nothing to play, no looping
	m_bActive(false),					// Device currently unavailable
	m_mciDeviceID(-1),
	m_nCurrentTrack(0),					// No current track
	m_nPlayIndex(0),					// No track list active
	m_saveloop(false),
	m_savetrack(0),
	m_cdon(false)
{
	memset(m_PlayList, 0, sizeof(int)*99);	// Nothing playing

	// Ok, let's open up a channel to MCI so we can communicate with the CD
	// ..Audio device.
	MCI_OPEN_PARMS mciOpen;

	mciOpen.lpstrDeviceType = "cdaudio";	// We want CD Audio
	MCIERROR theError = mciSendCommand(NULL, MCI_OPEN, MCI_OPEN_TYPE, (DWORD)&mciOpen);

	if(theError != 0)
	{
		CCD->ReportError("[WARNING] CCDAudio: Failed to open MCI CD AUDIO device", false);
		return;
	}
	else
	{
		m_bActive = true;						// Ready to rock!
		m_cdon = false;
		m_mciDeviceID = mciOpen.wDeviceID;			// Save this for later operations
	}

	// Set the format for the CD audio device to TRACKS/MINUTES/SECONDS
	MCI_SET_PARMS mciSet;

	mciSet.dwCallback = NULL;			// Just in case...
	mciSet.dwAudio = 0;						// Not setting audio output channels
	mciSet.dwTimeFormat = MCI_FORMAT_TMSF;		// Tracks/minutes/seconds/frames

	theError = mciSendCommand(m_mciDeviceID, MCI_SET,
				MCI_SET_TIME_FORMAT, (DWORD)&mciSet);
}

/* ------------------------------------------------------------------------------------ */
//	Destructor
//
//	Stop the CD, shut down MCI, clean up.
/* ------------------------------------------------------------------------------------ */
CCDAudio::~CCDAudio()
{
	MCI_GENERIC_PARMS mciClose;

	mciClose.dwCallback = NULL;							// Please, no notifications

	Stop();												// Stop playback

	MCIERROR theError = mciSendCommand(m_mciDeviceID, MCI_CLOSE,
										MCI_WAIT, (DWORD)&mciClose);	// Shut it down!

	if(theError != 0)
	{
		CCD->ReportError("[WARNING] CCDAudio: Failed to close MCI CD AUDIO device", false);
	}

	m_bActive = false;
	m_mciDeviceID = 0;
}

/* ------------------------------------------------------------------------------------ */
//	Play
//
//	Play a single CD Audio track, with looping if desired.
/* ------------------------------------------------------------------------------------ */
int CCDAudio::Play(int nTrack, bool bLoop)
{
	if(nTrack == 0)
		return RGF_SUCCESS;

	if(m_cdon)
	{
		m_saveloop = m_bLooping = bLoop;			// If we're looping
		m_savetrack = m_nCurrentTrack = nTrack;		// Our current track
	}
	else
	{
		m_saveloop = bLoop;
		m_savetrack = m_nCurrentTrack = nTrack;		// Our current track
		return RGF_SUCCESS;
	}

	MCI_PLAY_PARMS mciPlay;

	mciPlay.dwFrom = 0L;
	mciPlay.dwTo = 0L;
	mciPlay.dwFrom = MCI_MAKE_TMSF(nTrack, 0, 0, 0);				// Start track
	mciPlay.dwTo = MCI_MAKE_TMSF(nTrack+1, 0, 0, 0);				// End track

	MCIERROR theError = mciSendCommand(m_mciDeviceID, MCI_PLAY, MCI_NOTIFY, (DWORD)&mciPlay);

	if(theError != 0)
	{
		char szError[80];
		sprintf(szError, "[WARNING] File %s - Line %d: CCDAudio: Failed to play from %d to %d\n",
				__FILE__, __LINE__, nTrack, nTrack+1);
		CCD->ReportError(szError, false);
		return RGF_FAILURE;
	}

	//	Track is playing, let's bail out.
	return RGF_SUCCESS;												// Track playing
}

/* ------------------------------------------------------------------------------------ */
//	IsPlaying
//
//	Check to see if the CD Player is playing.
/* ------------------------------------------------------------------------------------ */
bool CCDAudio::IsPlaying()
{
	MCI_STATUS_PARMS mciState;

	mciState.dwItem = MCI_STATUS_MODE;

	MCIERROR theError = mciSendCommand(m_mciDeviceID, MCI_STATUS,
				MCI_WAIT | MCI_STATUS_ITEM, (DWORD)&mciState);

	if(mciState.dwReturn == MCI_MODE_PLAY)
		return true;												// CD is playing

	return false;													// CD not playing
}

/* ------------------------------------------------------------------------------------ */
//	Stop
//
//	Stop the current playback, whether a single track or a list.  If
//	..a list is playing, _ALL_ playback is still stopped.
/* ------------------------------------------------------------------------------------ */
int CCDAudio::Stop()
{
	m_bLooping = false;									// Prevent restarting

	if(!IsPlaying())
		return RGF_SUCCESS;								// Not playing, don't waste time!

	MCI_GENERIC_PARMS mciStop;

	mciStop.dwCallback = NULL;							// Please, no notifications

	// Halt playback
	MCIERROR theError = mciSendCommand(m_mciDeviceID, MCI_STOP, MCI_WAIT, (DWORD)&mciStop);

	if(theError != 0)
	{
		CCD->ReportError("[WARNING] CCDAudio: Failed to stop MCI CD AUDIO device", false);
		return RGF_FAILURE;
	}

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	PlaySequence
//
//	Play a sequence of CD Audio tracks.  Up to 99 tracks can be sequenced.
//	..This is useful if you have a series of audio tracks you want played
//	..for a single level, to keep the soundtrack from getting boring.
/* ------------------------------------------------------------------------------------ */
int CCDAudio::PlaySequence(int nCount, int *nTrackList)
{
	if((nCount <= 0) || (nCount > 99))
		return RGF_FAILURE;								// Bad parameters!

	memcpy(m_PlayList, nTrackList, sizeof(int)*nCount);	// Copy list

	m_nPlayIndex = 0;									// Moved to next already
	m_PlayCount = nCount;								// # of tracks to play
	Play(m_PlayList[0], false);							// Play the first track

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	Check
//
//	Since it appears that not all MCI CD Audio devices provide proper
//	..notification when a track is completed, we need to poll the
//	..device to see if it's done the last play request.  This function
//	..does that.
/* ------------------------------------------------------------------------------------ */
void CCDAudio::Check()
{
	static ULONG lastTime = CCD->FreeRunningCounter();

	if(m_nCurrentTrack == 0)
		return;											// No track playing

	if(m_bActive == false)
		return;											// No CD/CD open failed

	if((CCD->FreeRunningCounter() - lastTime) < 250)
		return;											// Don't check more often than every 250msec.

	lastTime = CCD->FreeRunningCounter();				// Update time period counter

	if(IsPlaying())
		return;											// Playing, don't bother it

	if(m_bLooping)
	{
		Play(m_nCurrentTrack, true);					// Play same old track again
		return;
	}

	m_nCurrentTrack = 0;								// Assume no more playback

	// Ok, we're not looping.  If we have a playlist, move to the next
	// ..track in the list (if there is one).
	if(m_PlayCount != 0)
	{
		++m_nPlayIndex;

		if(m_nPlayIndex > m_PlayCount)
		{
			m_nPlayIndex = 0;
			return;										// Playback of the list is complete
		}

		Play(m_PlayList[m_nPlayIndex], false);			// Play next track in list
	}
}

/* ------------------------------------------------------------------------------------ */
//	SaveTo
//
//	Save the current CD Player state to the supplied file
/* ------------------------------------------------------------------------------------ */
int CCDAudio::SaveTo(FILE *SaveFD)
{
	fwrite(&m_PlayCount,		sizeof(int),	1,	SaveFD);
	fwrite(&m_nPlayIndex,		sizeof(int),	1,	SaveFD);
	fwrite(&m_bLooping,			sizeof(bool),	1,	SaveFD);
	fwrite(&m_bActive,			sizeof(bool),	1,	SaveFD);
	fwrite(&m_nCurrentTrack,	sizeof(int),	1,	SaveFD);
	fwrite(&m_PlayList,			sizeof(int),	99, SaveFD);

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	RestoreFrom
//
//	Restore the CD Player state from the supplied file
/* ------------------------------------------------------------------------------------ */
int CCDAudio::RestoreFrom(FILE *RestoreFD)
{
	Stop();					// Shut it down for now...

	fread(&m_PlayCount,		sizeof(int),	1,	RestoreFD);
	fread(&m_nPlayIndex,	sizeof(int),	1,	RestoreFD);
	fread(&m_bLooping,		sizeof(bool),	1,	RestoreFD);
	fread(&m_bActive,		sizeof(bool),	1,	RestoreFD);
	fread(&m_nCurrentTrack, sizeof(int),	1,	RestoreFD);
	fread(&m_PlayList,		sizeof(int),	99, RestoreFD);

	//	Ok, if we were playing anything, restart it.

	if(m_PlayCount != 0)
		Play(m_PlayList[m_nPlayIndex], false);
	else if(m_nCurrentTrack != 0)
		Play(m_nCurrentTrack, m_bLooping);

	return RGF_SUCCESS;
}

/* ----------------------------------- END OF FILE ------------------------------------ */
