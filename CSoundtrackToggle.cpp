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
#include "CCDAudio.h"
#include "CMIDIAudio.h"
#include "CSoundtrackToggle.h"

/* ------------------------------------------------------------------------------------ */
// Constructor
/* ------------------------------------------------------------------------------------ */
CSoundtrackToggle::CSoundtrackToggle() :
	m_EntityCount(0)
{
	m_dsPtr = (LPDIRECTSOUND)geSound_GetDSound();
	m_MIDIPlayer = new CMIDIAudio();
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
// Destructor
//
// Go through all toggles and unload all the audio associated with 'em.
/* ------------------------------------------------------------------------------------ */
CSoundtrackToggle::~CSoundtrackToggle()
{
	delete m_MIDIPlayer;
	delete m_Streams;
}

/* ------------------------------------------------------------------------------------ */
// Tick
//
// Check to see if the player is within the trigger range for any
// ..of the soundtrack toggles we manage
/* ------------------------------------------------------------------------------------ */
void CSoundtrackToggle::Tick(geFloat /*dwTicks*/)
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

		if(!EffectC_IsStringNull(pToggle->szMIDIFileOne) && m_MIDIPlayer)
		{
			m_MIDIPlayer->Stop();
			m_MIDIPlayer->Play(pToggle->szMIDIFileOne, pToggle->bMIDILoops);
		}

		if(!EffectC_IsStringNull(pToggle->szStreamFileOne) && m_Streams)
		{
			// build filename
			char music[256];
			strcpy(music, CCD->GetDirectory(kAudioStreamFile));
			strcat(music, "\\");
			strcat(music, pToggle->szStreamFileOne);

			m_Streams->Stop();
			m_Streams->Create(music);

			m_Streams->Play(pToggle->bStreamLoops);
		}

		if(pToggle->bOneShot)
			pToggle->bActive = GE_FALSE;					// Deactivate after use
	}
}

/* ------------------------------------------------------------------------------------ */
// ReSynchronize
//
// Correct internal timing to match current time, to make up for time lost
// ..when outside the game loop (typically in "menu mode").
/* ------------------------------------------------------------------------------------ */
int CSoundtrackToggle::ReSynchronize()
{
	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// Stop all streaming audio tracks
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
// Set the volume for all the streams
/* ------------------------------------------------------------------------------------ */
void CSoundtrackToggle::SetVolume(LONG nVolume)
{
	m_Streams->SetVolume(nVolume);
}


/* ----------------------------------- END OF FILE ------------------------------------ */
