/************************************************************************************//**
 * @file oggaudio.h
 * @brief OggAudio class
 * @author Torok Edwin
 ****************************************************************************************/

#ifndef _OGGAUDIOH_
#define _OGGAUDIOH_

//	* Local defines, constants
#include "vorbis\\vorbisfile.h"

const int kBufferSize = 48000*4;

/**
 * @brief OggAudio class
 */
class OggAudio
{
public:
	explicit OggAudio(LPDIRECTSOUND lpDS);	///< Constructor
	~OggAudio();						///< Default destructor

	int Load(const char *szFileName);	///< Add playback stream
	int Play(bool bLooping);			///< Start streaming
	int Stop();							///< Stop streaming
	int Pause();						///< Pause streaming
	int Delete();						///< Delete stream
	int Rewind();						///< Rewind stream to beginning
	bool IsPlaying();					///< Is stream playing?
	int SetVolume(long volume);			///< Set volume on audio
	int SetPan(long pan);
	int SetFrequency(unsigned long frequency);
	long GetBaseFrequency() const { return m_BaseFrequency; }

private:
	// Private member functions

	/**
	 * @brief Get max. buffer write size
	 */
	DWORD GetMaxWriteSize();

	/**
	 * @brief Pump wave data to stream
	 */
	int PumpWave(int nSize);

	/**
	 * @brief Static timer callback
	 */
	static void CALLBACK TimerFunction(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2);

private:
	// Private member variables
	LPDIRECTSOUND m_pDS;				///< DirectSound bject
	OggVorbis_File m_ovf;				///< Handle to Ogg file
	bool m_Pumping;
	int m_nDataPosition;				///< Start of Ogg data in file
	bool m_fActive;						///< Playback active flag
	LPDIRECTSOUNDBUFFER m_pStream;		///< Sound buffers
	int m_nTimerID;						///< Timer ID, this instance
	bool m_fEOF;						///< At end of Ogg file
	DWORD m_OggPcmPos;
	DWORD m_EndPos;
	DWORD m_PlayPos;
	bool m_bLoops;						///< Loop this Ogg or not?
	HANDLE m_EndEvent;
	DWORD m_BaseFrequency;
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
