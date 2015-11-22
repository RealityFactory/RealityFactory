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

//	Ok, see if we have any static entity proxies we need to set up.

	pSet = geWorld_GetEntitySet(CCD->World(), "SoundtrackToggle");

	if(!pSet) 
		return;													// Don't waste CPU time.

//	Ok, we have soundtrack toggles somewhere.  Dig through 'em all.

	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	    pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
		{
		SoundtrackToggle *pToggle = (SoundtrackToggle*)geEntity_GetUserData(pEntity);
		m_SoundtrackToggleCount++;
		pToggle->theSound = NULL;						// No sound, right now...
		pToggle->SoundHandle = NULL;				// No sound playing
		pToggle->bActive = true;						// Toggle is active
		pToggle->bWhichOne = true;					// Start with first soundtrack
		pToggle->LastTimeToggled = 0;				// Ready right away
		if((pToggle->szSoundFile != NULL) && (strlen(pToggle->szSoundFile) > 0))
		  {
			// Load the sound to be played
			geVFile *SoundFile;
			CCD->OpenRFFile(&SoundFile, kAudioFile, pToggle->szSoundFile,
								GE_VFILE_OPEN_READONLY);
	    if(!SoundFile)
			  {
				char szError[256];
				sprintf(szError,"Can't open audio file '%s'", pToggle->szSoundFile);
				CCD->ReportError(szError, false);
				}
			else
			  {
	      pToggle->theSound = geSound_LoadSoundDef(CCD->Engine()->AudioSystem(), SoundFile);
	      geVFile_Close(SoundFile);
				}
		  }
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

  if(m_SoundtrackToggleCount == 0)
	  return;									// Don't waste time here

	pSet = geWorld_GetEntitySet(CCD->World(), "SoundtrackToggle");

	if(!pSet) 
		return;									// All gone?  How odd.

//	Ok, we have toggles somewhere.  Dig through 'em all and release
//	..the audio (if any) and the actors we loaded.

	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	    pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
		{
		SoundtrackToggle *pToggle = (SoundtrackToggle*)geEntity_GetUserData(pEntity);
		if(pToggle->theSound != NULL)
		  geSound_FreeSoundDef(CCD->Engine()->AudioSystem(), pToggle->theSound);
		pToggle->theSound = NULL;							// No sound, right now...
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
		if(CCD->CDPlayer())
		  CCD->CDPlayer()->Stop();						// Kill CD soundtrack, if any
		if(CCD->MIDIPlayer())
		  CCD->MIDIPlayer()->Stop();					// Kill MIDI soundtrack, if any
		StopStreaming();
//		CCD->AudioStreams()->StopAll();
		pToggle->LastTimeToggled = CCD->FreeRunningCounter_F();	// Update toggle time
    if(pToggle->theSound != NULL)
	    {
		  // We have some sound, play the sucker.
		  geFloat Volume, Pan, Frequency;
		  geXForm3d xfmPlayer = CCD->Player()->ViewPoint();
		  geSound3D_GetConfig(CCD->World(),	&xfmPlayer,	&pToggle->origin,
				kAudibleRadius, 2.0f,	&Volume, &Pan,	&Frequency);
		  pToggle->SoundHandle = geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), 
				pToggle->theSound, Volume, Pan, Frequency, GE_FALSE);
		  }
		// Ok, now we check to see what we're supposed to play next.
		if(pToggle->bWhichOne)
		  {
			if((pToggle->CDTrackOne != 0) && (CCD->CDPlayer() != NULL))
			  CCD->CDPlayer()->Play(pToggle->CDTrackOne, pToggle->bCDLoops);
			if((pToggle->szMIDIFileOne != NULL) && (strlen(pToggle->szMIDIFileOne) > 0))
			  CCD->MIDIPlayer()->Play(pToggle->szMIDIFileOne, pToggle->bMIDILoops);
			if((pToggle->szStreamFileOne != NULL) && (strlen(pToggle->szStreamFileOne) > 0))
			  CCD->AudioStreams()->Play(pToggle->szStreamFileOne, pToggle->bStreamLoops, true);
			}
		else
		  {
			if((pToggle->CDTrackTwo != 0) && (CCD->CDPlayer() != NULL))
			  CCD->CDPlayer()->Play(pToggle->CDTrackTwo, pToggle->bCDLoops);
			if((pToggle->szMIDIFileTwo != NULL) && (strlen(pToggle->szMIDIFileTwo) > 0))
			  CCD->MIDIPlayer()->Play(pToggle->szMIDIFileTwo, pToggle->bMIDILoops);
			if((pToggle->szStreamFileTwo != NULL) && (strlen(pToggle->szStreamFileTwo) > 0))
			  CCD->AudioStreams()->Play(pToggle->szStreamFileOne, pToggle->bStreamLoops, true);
			}
		pToggle->bWhichOne = !pToggle->bWhichOne;		// Switch the toggle
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
		if((pToggle->szStreamFileTwo != NULL) && (strlen(pToggle->szStreamFileTwo) > 0))
			  CCD->AudioStreams()->Stop(pToggle->szStreamFileTwo); 
	}
}
