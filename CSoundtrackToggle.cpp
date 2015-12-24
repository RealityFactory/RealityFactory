/************************************************************************************//**
 * @file CSoundtrackToggle.cpp
 * @brief Soundtrack Toggle Entity Class
 *
 * This file contains the class implementation for the Soundtrack Toggle Entity
 * handling class.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

// Include the One True Header
#include "RabidFramework.h"

/* ------------------------------------------------------------------------------------ */
//	Constructor
/* ------------------------------------------------------------------------------------ */
CSoundtrackToggle::CSoundtrackToggle() :
	m_EntityCount(0)
{
	m_Streams = NULL;
	m_dsPtr = (LPDIRECTSOUND)geSound_GetDSound();;
	theMIDIPlayer = new CMIDIAudio();
	m_Streams = new StreamingAudio(m_dsPtr);

	// Ok, see if we have any soundtrack toggles we need to set up.
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "SoundtrackToggle");

	if(!pSet)
		return;										// Don't waste CPU time.

	geEntity *pEntity;

	// Ok, we have soundtrack toggles somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		SoundtrackToggle *pToggle = static_cast<SoundtrackToggle*>(geEntity_GetUserData(pEntity));
		++m_EntityCount;

		pToggle->bActive = GE_TRUE;					// Toggle is active
		pToggle->LastTimeToggled = 0.0f;			// Ready right away
		pToggle->alive = GE_FALSE;
	}
}

/* ------------------------------------------------------------------------------------ */
//	Destructor
//
//	Go through all toggles and unload all the audio associated with 'em.
/* ------------------------------------------------------------------------------------ */
CSoundtrackToggle::~CSoundtrackToggle()
{

	if(theMIDIPlayer)
	{
		delete theMIDIPlayer;
		theMIDIPlayer = NULL;
	}

	if(m_Streams)
	{
		delete m_Streams;
		m_Streams = NULL;
	}

// changed QD 12/15/05 - code does nothing
/*
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	pSet = geWorld_GetEntitySet(CCD->World(), "SoundtrackToggle");

	if(!pSet)
		return;									// All gone?  How odd.

	// Ok, we have toggles somewhere.  Dig through 'em all and release the audio (if any)
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		SoundtrackToggle *pToggle = (SoundtrackToggle*)geEntity_GetUserData(pEntity);
	}

	//	All cleaned up, bail this mess.
	return;
*/
// end change
}

/* ------------------------------------------------------------------------------------ */
//	Tick
//
//	Check to see if the player is within the trigger range for any
//	..of the soundtrack toggles we manage
/* ------------------------------------------------------------------------------------ */
void CSoundtrackToggle::Tick(geFloat dwTicks)
{
	if(m_EntityCount == 0)
		return;							// No toggles in world, bail early

	// Ok, see if we have any soundtrack toggles
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "SoundtrackToggle");

	if(!pSet)
		return;												// Don't waste CPU time.

	geVec3d PlayerPos = CCD->Player()->Position();					// Get player position

	geEntity *pEntity;

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		SoundtrackToggle *pToggle = static_cast<SoundtrackToggle*>(geEntity_GetUserData(pEntity));

		if(geVec3d_DistanceBetweenSquared(&(pToggle->origin), &PlayerPos) > pToggle->Range*pToggle->Range)
			continue;														// Too far away

		// We need to check to see if we're getting collided with "too often",
		// ..that is, if the player is standing in the trigger or going back
		// ..and forth through it quickly.  While handling this can lead to
		// ..some possible anomolies (the right soundtrack might not always
		// ..play if the player is doing something perverse) you'll always end
		// ..up with one or the other playing.
		if(CCD->FreeRunningCounter() < static_cast<unsigned long>(pToggle->LastTimeToggled + (pToggle->SleepTime*1000)))
			continue;									// No more often than every <n> seconds.

		if(!pToggle->bActive)
			continue;									// This one isn't active

		pToggle->LastTimeToggled = CCD->FreeRunningCounter_F();	// Update toggle time

		pToggle->alive = GE_TRUE;

		if((pToggle->CDTrackOne != 0) && (CCD->CDPlayer()))
		{
			CCD->CDPlayer()->Stop();
			CCD->CDPlayer()->Play(pToggle->CDTrackOne, pToggle->bCDLoops);
		}

		if(!EffectC_IsStringNull(pToggle->szMIDIFileOne) && theMIDIPlayer)
		{
			theMIDIPlayer->Stop();
			theMIDIPlayer->Play(pToggle->szMIDIFileOne, pToggle->bMIDILoops);
		}

		if(!EffectC_IsStringNull(pToggle->szStreamFileOne) && m_Streams)
		{
// changed QD 09/02/03
// build filename
			char music[256];
			strcpy(music, CCD->GetDirectory(kAudioStreamFile));
			strcat(music, "\\");
			strcat(music, pToggle->szStreamFileOne);

			m_Streams->Stop();
//			m_Streams->Create(pToggle->szStreamFileOne);
			m_Streams->Create(music);
// end change
			m_Streams->Play(pToggle->bStreamLoops);
		}

		if(pToggle->bOneShot)
			pToggle->bActive = GE_FALSE;					// Deactivate after use
	}

	return;								// All done.
}

/* ------------------------------------------------------------------------------------ */
//	ReSynchronize
//
//	Correct internal timing to match current time, to make up for time lost
//	..when outside the game loop (typically in "menu mode").
/* ------------------------------------------------------------------------------------ */
int CSoundtrackToggle::ReSynchronize()
{
	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	Stop all streaming audio tracks
/* ------------------------------------------------------------------------------------ */
void CSoundtrackToggle::StopStreaming()
{
	// Ok, see if we have any soundtrack toggles
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "SoundtrackToggle");

	if(!pSet)
		return;													// Don't waste CPU time.

	geEntity *pEntity;

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	    pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		SoundtrackToggle *pToggle = static_cast<SoundtrackToggle*>(geEntity_GetUserData(pEntity));

		if((pToggle->szStreamFileOne != NULL) && (strlen(pToggle->szStreamFileOne) > 0))
			  CCD->AudioStreams()->Stop(pToggle->szStreamFileOne);
	}
}

/* ------------------------------------------------------------------------------------ */
//	Set the volume for all the streams
/* ------------------------------------------------------------------------------------ */
void CSoundtrackToggle::SetVolume(LONG nVolume)
{
	m_Streams->SetVolume(nVolume);
}


/* ----------------------------------- END OF FILE ------------------------------------ */
