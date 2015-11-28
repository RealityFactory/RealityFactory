/*
	CSoundtrackToggle.cpp:		Soundtrack Toggle Entity Class

	(c) 2001 Ralph Deane

	This file contains the class implementation for the Soundtrack
Toggle handling class.
*/

//	Include the One True Header

#include "RabidFramework.h"

//	Constructor
//
//	Go through all the entity proxies in the level and load up the
//	..various actors to be used.

CSoundtrackToggle::CSoundtrackToggle()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	m_SoundtrackToggleCount = 0;
	theMIDIPlayer = NULL;
	m_Streams = NULL;
	m_dsPtr = (LPDIRECTSOUND)geSound_GetDSound();;
	theMIDIPlayer = new CMIDIAudio();
	m_Streams = new StreamingAudio(m_dsPtr);

	//	Ok, see if we have any soundtrack toggles we need to set up.
	
	pSet = geWorld_GetEntitySet(CCD->World(), "SoundtrackToggle");
	
	if(!pSet) 
		return;													// Don't waste CPU time.
	
	//	Ok, we have soundtrack toggles somewhere.  Dig through 'em all.
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		SoundtrackToggle *pToggle = (SoundtrackToggle*)geEntity_GetUserData(pEntity);
		m_SoundtrackToggleCount++;
		
		pToggle->bActive = true;						// Toggle is active
		pToggle->LastTimeToggled = 0;				// Ready right away
		pToggle->alive = false;
	}
	
	return;
}

//	Destructor
//
//	Go through all toggles and unload all the audio associated with 'em.

CSoundtrackToggle::~CSoundtrackToggle()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

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

	pSet = geWorld_GetEntitySet(CCD->World(), "SoundtrackToggle");
	
	if(!pSet) 
		return;									// All gone?  How odd.
	
	//	Ok, we have toggles somewhere.  Dig through 'em all and release
	//	..the audio (if any) and the actors we loaded.
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		SoundtrackToggle *pToggle = (SoundtrackToggle*)geEntity_GetUserData(pEntity);
		
	}
	
	//	All cleaned up, bail this mess.
	
	return;
}

//	Tick
//
//	Check to see if the player is within the trigger range for any
//	..of the soundtrack toggles we manage

void CSoundtrackToggle::Tick(geFloat dwTicks)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	geVec3d PlayerPos;
	
	if(m_SoundtrackToggleCount == 0)
		return;							// No toggles in world, bail early
	
	//	Ok, see if we have any soundtrack toggles
	
	pSet = geWorld_GetEntitySet(CCD->World(), "SoundtrackToggle");
	
	if(!pSet) 
		return;													// Don't waste CPU time.
	
	PlayerPos = CCD->Player()->Position();					// Get player position
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		SoundtrackToggle *pToggle = (SoundtrackToggle*)geEntity_GetUserData(pEntity);
		if(geVec3d_DistanceBetween(&pToggle->origin, &PlayerPos) > pToggle->Range)
			continue;														// Too far away
		// We need to check to see if we're getting collided with "too often",
		// ..that is, if the player is standing in the trigger or going back
		// ..and forth through it quickly.  While handling this can lead to
		// ..some possible anomolies (the right soundtrack might not always
		// ..play if the player is doing something perverse) you'll always end
		// ..up with one or the other playing.
		if(CCD->FreeRunningCounter() < (unsigned long)(pToggle->LastTimeToggled + (pToggle->SleepTime*1000)))
			continue;									// No more often than every <n> seconds.
		if(!pToggle->bActive)
			continue;									// This one isn't active

		pToggle->LastTimeToggled = CCD->FreeRunningCounter_F();	// Update toggle time
		
		pToggle->alive = true;

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
			m_Streams->Stop();
			m_Streams->Create(pToggle->szStreamFileOne);
			m_Streams->Play(pToggle->bStreamLoops);
		}
		
		if(pToggle->bOneShot)
			pToggle->bActive = false;						// Deactivate after use
	}
	
	return;								// All done.
}


//	ReSynchronize
//
//	Correct internal timing to match current time, to make up for time lost
//	..when outside the game loop (typically in "menu mode").

int CSoundtrackToggle::ReSynchronize()
{

	return RGF_SUCCESS;
}

// Stop all streaming audio tracks
//
void CSoundtrackToggle::StopStreaming()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	
//	Ok, see if we have any soundtrack toggles

	pSet = geWorld_GetEntitySet(CCD->World(), "SoundtrackToggle");

	if(!pSet) 
		return;													// Don't waste CPU time.

	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	    pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		SoundtrackToggle *pToggle = (SoundtrackToggle*)geEntity_GetUserData(pEntity);
		if((pToggle->szStreamFileOne != NULL) && (strlen(pToggle->szStreamFileOne) > 0))
			  CCD->AudioStreams()->Stop(pToggle->szStreamFileOne);
	}
}

// Set the volume for all the streams

void CSoundtrackToggle::SetVolume(LONG nVolume)
{
	m_Streams->SetVolume(nVolume);
}