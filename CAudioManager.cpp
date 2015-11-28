/****************************************************************************************/
/*																						*/
/*	CAudioManager.cpp:		Audio Manager												*/
/*																						*/
/*	This file contains the class implementation of the Audio Manager					*/
/*	system for Reality Factory.															*/
/*																						*/
/*	The Audio Manager handles the playback of ALL audio effects in						*/
/*	Reality Factory - this means MIDI, CD Audio, WAVE effects,							*/
/*	MP3, and streaming audio tracks.  The Audio Manager is an encapsulation				*/
/*	of the FMOD Audio System.															*/
/*																						*/
/****************************************************************************************/

#include "RabidFramework.h"				// The One True Header

/* ------------------------------------------------------------------------------------ */
//	Default constructor
/* ------------------------------------------------------------------------------------ */
CAudioManager::CAudioManager()
{
	for(int nTemp=0; nTemp<512; nTemp++)
		MainList[nTemp] = NULL;

	m_InstanceCount = 0;

	for(nTemp=0; nTemp<32; nTemp++)
		PlayingChannels[nTemp][0] = -1;				// Free channel

	return;
}

/* ------------------------------------------------------------------------------------ */
//	Default Destructor
/* ------------------------------------------------------------------------------------ */
CAudioManager::~CAudioManager()
{
  return;
}

/* ------------------------------------------------------------------------------------ */
//	Add
//
//	Add a new audio entity to the pool.  If the entity is already loaded,
//	..this creates a new instance rather than reloading it.
/* ------------------------------------------------------------------------------------ */
int CAudioManager::Add(char *szFilename, int nType, int *nHandle)
{
	*nHandle = (-1);

	return RGF_UNIMPLEMENTED;
}

/* ------------------------------------------------------------------------------------ */
//	Start
//
//	Start playback of the desired audio entity.
/* ------------------------------------------------------------------------------------ */
int CAudioManager::Start(int nHandle)
{
	return RGF_UNIMPLEMENTED;
}

/* ------------------------------------------------------------------------------------ */
//	Stop
//
//	Stop playback of the indicated audio entity.
/* ------------------------------------------------------------------------------------ */
int CAudioManager::Stop(int nHandle)
{
	return RGF_UNIMPLEMENTED;
}

/* ------------------------------------------------------------------------------------ */
//	Pause
//
//	Toggle the paused state of the desired entity.  If it's playing, stop it,
//	..if not, start it.
/* ------------------------------------------------------------------------------------ */
int CAudioManager::Pause(int nHandle)
{
	return RGF_UNIMPLEMENTED;
}

/* ------------------------------------------------------------------------------------ */
//	IsPlaying
//
//	Returns true if audio entity is playing, false otherwise
/* ------------------------------------------------------------------------------------ */
bool CAudioManager::IsPlaying(int nHandle)
{
	return false;
}

/* ------------------------------------------------------------------------------------ */
//	Remove
//
//	Remove the indicated audio entity from memory.
/* ------------------------------------------------------------------------------------ */
int CAudioManager::Remove(int nHandle)
{
	return RGF_UNIMPLEMENTED;
}

/* ------------------------------------------------------------------------------------ */
//	Rewind
//
//	Rewind the indicated audio entity.
/* ------------------------------------------------------------------------------------ */
int CAudioManager::Rewind(int nHandle)
{
	return RGF_UNIMPLEMENTED;
}

/* ------------------------------------------------------------------------------------ */
//	SetLooping
//
//	Set the looping flag for the indicated audio entity.
/* ------------------------------------------------------------------------------------ */
int CAudioManager::SetLooping(int nHandle, bool bLoops)
{
	return RGF_UNIMPLEMENTED;
}

/* ------------------------------------------------------------------------------------ */
//	RemoveOnCompletion
//
//	Set the flag that informs the audio manager to remove the specified
//	..audio effect when it completes playback.
/* ------------------------------------------------------------------------------------ */
int CAudioManager::RemoveOnCompletion(int nHandle)
{
	return RGF_UNIMPLEMENTED;
}

/* ------------------------------------------------------------------------------------ */
//	Status
//
//	Return the current playback status of the desired entity.
/* ------------------------------------------------------------------------------------ */
int CAudioManager::Status(int nHandle)
{
	return RGF_UNIMPLEMENTED;
}

/* ------------------------------------------------------------------------------------ */
//	MoveEmitter
//
//	Move the spatial position of the audio emitter.
/* ------------------------------------------------------------------------------------ */
int CAudioManager::MoveEmitter(int nHandle, geVec3d Position)
{
	return RGF_UNIMPLEMENTED;
}

/* ------------------------------------------------------------------------------------ */
//	SetVolume
//
//	Adjust the volume of an audio entity
/* ------------------------------------------------------------------------------------ */
int CAudioManager::SetVolume(int nHandle, geFloat fVolume)
{
	return RGF_UNIMPLEMENTED;
}

/* ------------------------------------------------------------------------------------ */
//	GetVolume
//
//	Get the current volume of an audio entity
/* ------------------------------------------------------------------------------------ */
geFloat CAudioManager::GetVolume(int nHandle)
{
	return 0.0f;
}

/* ------------------------------------------------------------------------------------ */
//	SetPan
//
//	Set the pan position for an audio entity
/* ------------------------------------------------------------------------------------ */
int CAudioManager::SetPan(int nHandle, geFloat fPan)
{
	return RGF_UNIMPLEMENTED;
}

/* ------------------------------------------------------------------------------------ */
//	GetPan
//
//	Get the pan position for an audio entity
/* ------------------------------------------------------------------------------------ */
geFloat CAudioManager::GetPan(int nHandle)
{
	return 0.0f;
}

/* ------------------------------------------------------------------------------------ */
//	Tick
//
//	Inform all audio entities of the passage of time.  Audio entities
//	..bound to a physical location are updated here to adjust for
//	..changes in audio location.
/* ------------------------------------------------------------------------------------ */
void CAudioManager::Tick(geFloat dwTicks)
{
	return;
}

//	*** PRIVATE MEMBER FUNCTIONS ***

/* ------------------------------------------------------------------------------------ */
//	FindFreeChannel
//
//	Search the hardware playback channel list and return the index # of a
//	..free playback slot.
/* ------------------------------------------------------------------------------------ */
int CAudioManager::FindFreeChannel()
{
	for(int nTemp=0; nTemp<32; nTemp++)
	{
		if(PlayingChannels[nTemp][0] < 0)
			return nTemp;				// A free channel!
	}

	return -1;							// No free playback channels
}

/* ------------------------------------------------------------------------------------ */
//	GetPlaybackChannel
//
//	Return the hardware playback channel an audio entity is using
/* ------------------------------------------------------------------------------------ */
int CAudioManager::GetPlaybackChannel(int nHandle)
{
	for(int nTemp=0; nTemp<512; nTemp++)
	{
		if(MainList[nTemp] == NULL)
			continue;						// Ignore the empty slots

		AudioInstance *pTemp = MainList[nTemp]->IList;

		while(pTemp != NULL)
		{
			if(pTemp->theHandle == nHandle)
				return ChannelFromHandle(nHandle);		// Found it!
		}
	}

	return -1;							// Handle is not actively playing
}

/* ------------------------------------------------------------------------------------ */
//	ChannelFromHandle
//
//	Given a playback handle, see if it's in the hardware playback list.
/* ------------------------------------------------------------------------------------ */
int CAudioManager::ChannelFromHandle(int nHandle)
{
	for(int nTemp=0; nTemp<32; nTemp++)
	{
		if(PlayingChannels[nTemp][1] == nHandle)
			return nTemp;				// Handle is playing on this channel
	}

	return -1;							// Handle is NOT playing
}

/* ----------------------------------- END OF FILE ------------------------------------ */
