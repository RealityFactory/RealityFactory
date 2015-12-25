/************************************************************************************//**
 * @file StreamingAudio.h
 * @brief Streaming audio encapsulation class
 *
 * This file contains the class definitions for audio streaming functionality,
 * based on the SoundServices functionality.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All Rights Reserved
 ****************************************************************************************/

#ifndef __RGF_STREAMINGAUDIO_H_
#define __RGF_STREAMINGAUDIO_H_

//	* Local defines, constants

class OggAudio;
class CMp3Manager;
const int kBufferSize = 48000*4;

/**
 * @brief Streaming audio class
 */
class StreamingAudio
{
public:
	explicit StreamingAudio(LPDIRECTSOUND lpDS);	///< Constructor
	~StreamingAudio();						///< Default destructor

	int Create(char *szFileName);			///< Add playback stream
	int Play(bool bLooping);				///< Start streaming
	int Stop();								///< Stop streaming
	int Pause();							///< Pause streaming
	int Delete();							///< Delete stream
	int Rewind();							///< Rewind stream to beginning
	bool IsPlaying();						///< Is stream playing?
	int SetVolume(LONG nVolume);			///< Set volume on audio

private:
	//	Private member functions
	DWORD GetMaxWriteSize();				///< Get max. buffer write size
	int PumpWave(int nSize);				///< Pump wave data to stream

	// Static timer callback
	static void CALLBACK TimerFunction(UINT uID, UINT uMsg, DWORD dwUser,
										DWORD dw1, DWORD dw2);

	int WaveOpen(char *szFileName, HMMIO *pFileID,
					WAVEFORMATEX **ppwfxInfo,	MMCKINFO *pckInRIFF);

	int WaveRead(HMMIO hmmioIn, int nSizeToRead, BYTE *pDestination,
					MMCKINFO *pckIn, UINT *nBytesRead);

private:
	//	Private member variables
	LPDIRECTSOUND	m_pDS;					///< DirectSound object
	HMMIO			m_hWaveFile;			///< Handle to WAVE file
	int				m_nDataPosition;		///< Start of WAVE data in file
	int				m_nCurrentPosition;		///< Current read position
	int				m_nDataSize;			///< Wave data size, in bytes
	int				m_nDataLeft;			///< # of bytes of data left
	bool			m_fActive;				///< Playback active flag
	LPDIRECTSOUNDBUFFER m_pStream;			///< Sound buffers
	int				m_nTimerID;				///< Timer ID, this instance
	WAVEFORMATEX	*m_pWaveFormat;			///< Wave format block
	MMCKINFO		m_rRiffData;			///< Wave RIFF block
	bool			m_fEOF;					///< At end of WAVE file
	int				m_nOffset;
	bool			m_bLoops;				///< Loop this WAVE or not?
	bool			mp3;
	bool			ogg;
	CMp3Manager		*Mpeg3;
	OggAudio		*Ogg;
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
