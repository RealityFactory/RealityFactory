/************************************************************************************//**
 * @file CAudioManager.h
 * @brief Audio Manager
 *
 * This file contains the class declaration of the Audio Manager system for
 * Reality Factory.
 * The Audio Manager handles the playback of ALL audio effects in
 * Reality Factory - this means MIDI, CD Audio, WAVE effects, MP3
 * and streaming audio tracks. The Audio Manager is an encapsulation
 * of the FMOD Audio System.
 * @remarks unimplemented/unused
 ****************************************************************************************/

#ifndef __CAUDIO_MANAGER__
#define __CAUDIO_MANAGER__

//	This struct holds information for each instance of an audio effect
struct AudioInstance
{
	geVec3d SpatialPosition;	// Transform of audio source
	geFloat Volume;				// Volume of instance
	geFloat Pan;				// Pan of instance, if not 3D audio;
	bool	bUpdated;			// Need to update due to state change
	bool	bRemovedWhenDone;	// TRUE if entity removed when playback completes
	int		theHandle;			// Handle for this entity
};


struct AudioInstanceList
{
	char szAudioName[512];		// Filename for audio entity
	int AudioType;				// Type of audio entity this is
	AudioInstance *IList;		// List of instances, this audio
};


class CAudioManager : public CRGFComponent
{
public:
	CAudioManager();									// Default constructor
	~CAudioManager();									// Default destructor

	int Add(char *szFilename, int nType, int *nHandle);	// Add audio
	int Remove(int nHandle);							// Remove instance
	int RemoveOnCompletion(int nHandle);				// Set audio removal on completion

	int Start(int nHandle);								// Start audio playing
	int Stop(int nHandle);								// Stop audio
	int Pause(int nHandle);								// Pause audio
	int Rewind(int nHandle);							// Rewind audio

	bool IsPlaying(int nHandle);						// Is audio playing
	int SetLooping(int nHandle, bool bLoops);			// Set looping/not looping
	int Status(int nHandle);							// Return audio playback status
	int MoveEmitter(int nHandle, geVec3d Position);		// Change spatial position

	int SetVolume(int nHandle, geFloat fVolume);		// Set volume
	geFloat GetVolume(int nHandle);						// Get volume

	int SetPan(int nHandle, geFloat fPan);				// Set pan position
	geFloat GetPan(int nHandle);						// Get pan position

	void Tick(geFloat dwTicks);							// Process passage of time

private:

	int FindFreeChannel();					// Find a free playback channel
	int GetPlaybackChannel(int nHandle);	// audio handle -> playback channel
	int ChannelFromHandle(int nHandle);		// Handle lookup

private:
	//	Private member variables
	AudioInstanceList *MainList[512];		// Database of managed audio
	int PlayingChannels[32][2];				// 32 concurrent playing audio entities
	int m_InstanceCount;					// Global instance counter
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
