/************************************************************************************//**
 * @file CAudioStream.cpp
 * @brief Streaming audio handler class
 *
 * This file contains the class implementation for the CAudioStream class that
 * handles streaming large WAVE files from mass storage into the Genesis3D
 * sound system.
 * @note This class depends on the 11/08/1999 Genesis3D engine mod to the file
 * Sound.c that exposes a function call to get the DirectSound object that the
 * Genesis3D engine is using. If you don't have this mod, this won't operate.
 * @author Ralph Deane
 *//*
 * Copyright(c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

//	Include the One True Header File
#include "RabidFramework.h"

/* ------------------------------------------------------------------------------------ */
//	Constructor
//
//	Clear up internal tables, and go through and locate/prepare all
//	..StreamingAudioProxy entities to prepare for possible playback.
/* ------------------------------------------------------------------------------------ */
CAudioStream::CAudioStream()
{
	geEntity *pEntity;

	m_nStreamerCount = 0;
	m_LoopingProxy = -1;							// No looping proxy yet

	for(int nTemp=0; nTemp<32; nTemp++)
    {
		m_FileList[nTemp] = NULL;
		m_Streams[nTemp] = NULL;
	}

	// Ok, get the DirectSound object from the Genesis3D audio system so
	// ..we can fiddle around behind its back...
	m_dsPtr = static_cast<LPDIRECTSOUND>(geSound_GetDSound());

	// Now scan for StreamingAudioProxy entities
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "StreamingAudioProxy");

	if(!pSet)
		return;										// Don't waste CPU time.

	//	Ok, we have soundtrack toggles somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	    pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		m_nStreamerCount++;
		StreamingAudioProxy *pProxy = static_cast<StreamingAudioProxy*>(geEntity_GetUserData(pEntity));
		pProxy->bActive = true;						// Trigger is active
		pProxy->LastTimeTriggered = 0;				// Ready immediately
	}

	return;
}

/* ------------------------------------------------------------------------------------ */
//	Destructor
//
//	Shut down all playing streams, clean up objects.
/* ------------------------------------------------------------------------------------ */
CAudioStream::~CAudioStream()
{
	for(int nTemp=0; nTemp<MAX_AUDIOSTREAMS; ++nTemp)
	{
		if(m_FileList[nTemp] != NULL)
			delete m_FileList[nTemp];

		m_FileList[nTemp] = NULL;

		if(m_Streams[nTemp] != NULL)
			delete m_Streams[nTemp];

		m_Streams[nTemp] = NULL;
	}

	return;
}

/* ------------------------------------------------------------------------------------ */
//	Play
//
//	Play a streaming WAVE file, with looping if desired.  This will
//	..allocate buffers and start a fill-timer thread running for the
//	.. stream to make sure the playback buffer is always filled.  If
//	..desired, when the playback hits end-of-file we'll rewind it and
//	..keep it looping.  Note that there is a slight delay between when
//	..this call is made and when the playback starts.
/* ------------------------------------------------------------------------------------ */
int CAudioStream::Play(const char *szFilename, bool fLoopIt, bool bProxy)
{
	if((szFilename == NULL) || (strlen(szFilename) <= 0))
		return RGF_FAILURE;

// FIX #1
	char szTemp[256];
	strcpy(szTemp, CCD->GetDirectory(kAudioStreamFile));
	strcat(szTemp, "\\");
	strcat(szTemp, szFilename);

	// If this is a request from the proxy trigger to stream a wave,
	// ..and we're already streaming a looping wave started by a
	// ..proxy, kill the pre-existing one before we start the new
	// ..one.  Why?  Well, you really only want one soundtrack
	// ..looping at one time, and this prevents accidents where
	// ..another soundtrack is started ALONG WITH the earlier one.

	if(m_LoopingProxy > 0)
    {
		m_Streams[m_LoopingProxy]->Stop();		// Stop it.

		delete m_Streams[m_LoopingProxy];		// Kill it
		m_Streams[m_LoopingProxy] = NULL;		// Crush it

		delete m_FileList[m_LoopingProxy];		// Smear it
		m_FileList[m_LoopingProxy] = NULL;		// Be mean to it!

		m_LoopingProxy = -1;					// ..and make it go away.
	}

	//	Ok, locate a free spot in the list
	int nSlot = FindFreeSlot();

	if(nSlot < 0)
		return RGF_FAILURE;							// No free slots, too many streams

// FIX #1
	m_FileList[nSlot] = new char[strlen(szTemp) + 2];
// FIX #1

	strcpy(m_FileList[nSlot], szTemp);		// Save name off

	m_Streams[nSlot] = new StreamingAudio(m_dsPtr);

	if(m_Streams[nSlot] == NULL)
	{
		delete m_FileList[nSlot];
		m_FileList[nSlot] = NULL;

		return RGF_FAILURE;
	}

	//	Ok, we've got the slot, filled it, constructed a StreamingAudio
	//	..object to handle the streaming WAVE, let's PLAY IT.
// FIX #1
	if(m_Streams[nSlot]->Create(szTemp) != RGF_SUCCESS)
	{
		char szBug[256];

		sprintf(szBug, "[WARNING] File %s - Line %d: Failed to play '%s'\n", __FILE__, __LINE__, szTemp);
		CCD->ReportError(szBug, false);

		delete m_FileList[nSlot];
		m_FileList[nSlot] = NULL;

		delete m_Streams[nSlot];
		m_Streams[nSlot] = NULL;

		return RGF_FAILURE;
	}

	m_Streams[nSlot]->Play(fLoopIt);	// Start me up!

	// If this is LOOPING and a PROXY TRIGGER, save the slot ID for later
	if(fLoopIt && bProxy)
		m_LoopingProxy = nSlot;					// For later harm.

	// At this point the thing should be playing, so we're outta here.
	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	IsPlaying
//
//	Check to see if a specific streaming WAVE file is playing.
/* ------------------------------------------------------------------------------------ */
bool CAudioStream::IsPlaying(const char *szFilename)
{
	if((szFilename == NULL) || (strlen(szFilename) <= 0))
		return false;							// You're kidding, right?

// FIX #1
	char szTemp[256];
	strcpy(szTemp, CCD->GetDirectory(kAudioStreamFile));
	strcat(szTemp, "\\");
	strcat(szTemp, szFilename);
// FIX #1

	int nSlot = FindInList(szTemp);

	if(nSlot < 0)
		return false;							// No such, not playing

	return m_Streams[nSlot]->IsPlaying();
}

/* ------------------------------------------------------------------------------------ */
//	Pause
//
//	Pause the streams playback.  Playing will pause at the next read of
//	..the stream file, so there will be some delay before playback is
//	..actually suspended.
/* ------------------------------------------------------------------------------------ */
int CAudioStream::Pause(const char *szFilename)
{
	if((szFilename == NULL) || (strlen(szFilename) <= 0))
		return false;							// You're kidding, right?

// FIX #1
	char szTemp[256];
	strcpy(szTemp, CCD->GetDirectory(kAudioStreamFile));
	strcat(szTemp, "\\");
	strcat(szTemp, szFilename);
// FIX #1

	int nSlot = FindInList(szTemp);

	if(nSlot < 0)
		return false;							// No such, not playing

	return m_Streams[nSlot]->Pause();
}

/* ------------------------------------------------------------------------------------ */
//	Stop
//
//	Stop playback and remove the file from the streaming list.
//	..Note that there is a slight delay between this call and
//	..when the file actually stops streaming.
/* ------------------------------------------------------------------------------------ */
int CAudioStream::Stop(const char *szFilename)
{
	if((szFilename == NULL) || (strlen(szFilename) <= 0))
		return false;							// You're kidding, right?

// FIX #1
	char szTemp[256];
	strcpy(szTemp, CCD->GetDirectory(kAudioStreamFile));
	strcat(szTemp, "\\");
	strcat(szTemp, szFilename);
// FIX #1

	int nSlot = FindInList(szTemp);

	if(nSlot < 0)
	  return false;							// No such, not playing

	// Ok, stop playback, kill the stream, and clean up the table
	int nError = m_Streams[nSlot]->Stop();

	delete m_Streams[nSlot];
	m_Streams[nSlot] = NULL;
	delete m_FileList[nSlot];
	m_FileList[nSlot] = NULL;

	return nError;
}

/* ------------------------------------------------------------------------------------ */
//	Tick
//
//	Perform time-based work.  In this case, we watch to see if the
//	..player has come close enough to a StreamingAudioProxy entity
//	..to trigger it.  We also sweep through the stream list, deleting
//	..any files that are no longer playing.
/* ------------------------------------------------------------------------------------ */
void CAudioStream::Tick(geFloat dwTicks)
{
	geEntity *pEntity;
	geVec3d PlayerPos;

	if(m_nStreamerCount == 0)
		return;										// No streamers in world, bail early

	// Now scan for StreamingAudioProxy entities
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "StreamingAudioProxy");

	if(!pSet)
		return;										// Don't waste CPU time.

	// Clean up any non-playing streams
	Sweep();

	// Ok, go through and see if we need to trigger playback.
	PlayerPos = CCD->Player()->Position();			// Get player position

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	    pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		StreamingAudioProxy *pProxy = static_cast<StreamingAudioProxy*>(geEntity_GetUserData(pEntity));

		if(pProxy->bActive == false)
			continue;								// Not active, ignore it

		// changed QD 12/15/05
		//if(geVec3d_DistanceBetween(&pProxy->origin, &PlayerPos) > pProxy->Range)
		//	continue;	// Too far away
		geVec3d temp;
		geVec3d_Subtract(&pProxy->origin, &PlayerPos, &temp);

		if(geVec3d_DotProduct(&temp, &temp) > pProxy->Range*pProxy->Range)
			continue;	// Too far away
		// end change

		// Check to see if we've waited long enough before triggering this
		// ..from the last time.
		if(CCD->FreeRunningCounter() < static_cast<DWORD>(pProxy->LastTimeTriggered + (pProxy->SleepTime*1000)))
			continue;								// No more often than every <n> seconds.

		// Ok, we're close enough and it's time.  Trigger it!
		Play(pProxy->szStreamFile, pProxy->bLoops, true);

		// If this is a one-shot, kill any hope of reactivation.
		if(pProxy->bOneShot == GE_TRUE)
			pProxy->bActive = false;				// Turn it off!
	}

	return;
}

/* ------------------------------------------------------------------------------------ */
//	SetVolume
//
//	Set the volume for all the streams
/* ------------------------------------------------------------------------------------ */
void CAudioStream::SetVolume(LONG nVolume)
{
	for(int nTemp=0; nTemp<MAX_AUDIOSTREAMS; ++nTemp)
	{
		// If there's something in a slot, we have to check it.
		if(m_Streams[nTemp] != NULL)
		{
			// Likely suspect, check it out!
			if(m_Streams[nTemp]->IsPlaying() == false)
			{
				m_Streams[nTemp]->SetVolume(nVolume);
			}
		}
	}

	return;
}

/* ------------------------------------------------------------------------------------ */
//	StopAll
//
//	Stop all the streams
/* ------------------------------------------------------------------------------------ */
void CAudioStream::StopAll()
{
	for(int nTemp=0; nTemp<MAX_AUDIOSTREAMS; ++nTemp)
	{
		if(m_FileList[nTemp] != NULL)
			delete m_FileList[nTemp];

		m_FileList[nTemp] = NULL;

		if(m_Streams[nTemp] != NULL)
			delete m_Streams[nTemp];

		m_Streams[nTemp] = NULL;
	}

	return;
}

/* ------------------------------------------------------------------------------------ */
//	PauseAll
//
//	Pause all the streams
/* ------------------------------------------------------------------------------------ */
void CAudioStream::PauseAll()
{
	for(int nTemp=0; nTemp<MAX_AUDIOSTREAMS; ++nTemp)
	{
		// If there's something in a slot, we have to check it.
		if(m_Streams[nTemp] != NULL)
		{
			m_Streams[nTemp]->Pause();
		}
	}

	return;
}

//	**************** PRIVATE MEMBER FUNCTIONS **************

/* ------------------------------------------------------------------------------------ */
//	FindFreeSlot
//
//	Locate a free slot in the array for us to use, return -1 if none.
/* ------------------------------------------------------------------------------------ */
int CAudioStream::FindFreeSlot()
{
	for(int nTemp=0; nTemp<MAX_AUDIOSTREAMS; ++nTemp)
	{
		if(m_FileList[nTemp] == NULL)
			return nTemp;			// Free found, bail this!
	}

	return -1;						// No free slots, argh!
}

/* ------------------------------------------------------------------------------------ */
//	FindInList
//
//	Look through the list of streams we're playing and see if the
//	..named file is one of 'em.
/* ------------------------------------------------------------------------------------ */
int CAudioStream::FindInList(const char *szFile)
{
	for(int nTemp=0; nTemp<MAX_AUDIOSTREAMS; ++nTemp)
	{
		if(m_FileList[nTemp] != NULL)
		{
			if(!strcmp(m_FileList[nTemp], szFile))
				return nTemp;							// It's here, it's here!
		}
	}

	//	Nope, can't find it.
	return -1;
}

/* ------------------------------------------------------------------------------------ */
//	Sweep
//
//	Go through all the playing streams and check to be sure they're
//	..playing.  Those that are NOT playing are deleted and removed
//	..from the list.
/* ------------------------------------------------------------------------------------ */
void CAudioStream::Sweep()
{
	for(int nTemp=0; nTemp<MAX_AUDIOSTREAMS; ++nTemp)
	{
		// If there's something in a slot, we have to check it.
		if(m_Streams[nTemp] != NULL)
		{
			// Likely suspect, check it out!
			if(m_Streams[nTemp]->IsPlaying() == false)
			{
				// Stopped playing?  Die, then!
				m_Streams[nTemp]->Stop();

				delete m_Streams[nTemp];
				m_Streams[nTemp] = NULL;

				delete m_FileList[nTemp];
				m_FileList[nTemp] = NULL;
			}
		}
	}

	return;
}

/* ------------------------------------------------------------------------------------ */
//	ReSynchronize
//
//	Correct internal timing to match current time, to make up for time lost
//	..when outside the game loop (typically in "menu mode").
/* ------------------------------------------------------------------------------------ */
int CAudioStream::ReSynchronize()
{
	return RGF_SUCCESS;
}

/* ----------------------------------- END OF FILE ------------------------------------ */
