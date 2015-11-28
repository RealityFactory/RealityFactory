/****************************************************************************************/
/*																						*/
/*	oggaudio.h:			OggAudio class													*/
/*																						*/
/*	Programming of this player was done by Torok Edwin and adapted to RF from his code	*/
/*																						*/
/****************************************************************************************/

#ifndef _OGGAUDIOH_
#define _OGGAUDIOH_

//	* Local defines, constants
#include "vorbis/vorbisfile.h"

//	* Oggaudio class

class OggAudio
{
public:
	OggAudio(LPDIRECTSOUND lpDS);	// Default constructor
	~OggAudio();					// Default destructor
	int Load(char *szFileName);		// Add playback stream
	int Play(bool bLooping);		// Start streaming
	int Stop();						// Stop streaming
	int Pause();					// Pause streaming
	int Delete();					// Delete stream
	int Rewind();					// Rewind stream to beginning
	bool IsPlaying();				// Is stream playing?
	int SetVolume(int nVolume);		// Set volume on audio

private:
//	Private member functions
	DWORD GetMaxWriteSize();		// Get max. buffer write size
	int PumpWave(int nSize);		// Pump wave data to stream
	// Static timer callback
	static void CALLBACK TimerFunction(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2);

private:
	//	Private member variables
	LPDIRECTSOUND m_pDS;			// DirectSound bject
	OggVorbis_File ovf;				// Handle to Ogg file
	FILE*	vf;//the file
	bool  Pumping;
	int m_nDataPosition;			// Start of Ogg data in file
	bool m_fActive;					// Playback active flag
	LPDIRECTSOUNDBUFFER m_pStream;	// Sound buffers
	int m_nTimerID;					// Timer ID, this instance
	bool m_fEOF;					// At end of WAVE file
	int OggPcmPos;
	bool m_bLoops;					// Loop this WAVE or not?
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
