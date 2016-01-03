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


class OggAudio;
class CMp3Manager;

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
	int SetVolume(long volume);				///< Set volume on audio
	int SetPan(long pan);
	int SetFrequency(unsigned long frequency);
	int Modify3D(geVec3d *soundPos, float minRadius);

private:
	// Private member functions
	DWORD GetMaxWriteSize();				///< Get max. buffer write size

	int PumpWave(int nSize);				///< Pump wave data to stream

	// Static timer callback
	static void CALLBACK TimerFunction(UINT uID, UINT uMsg, DWORD dwUser,
										DWORD dw1, DWORD dw2);

	int WaveOpen(char *szFileName, HMMIO *pFileID,
					WAVEFORMATEX **ppwfxInfo, MMCKINFO *pckInRIFF);

	int WaveRead(HMMIO hmmioIn, int nSizeToRead, BYTE *pDestination,
					MMCKINFO *pckIn, UINT *nBytesRead);

private:
	// Private member variables
	LPDIRECTSOUND	m_pDS;					///< DirectSound object
	HMMIO			m_hWaveFile;			///< Handle to WAVE file
	int				m_nDataPosition;		///< Start of WAVE data in file
	int				m_nCurrentPosition;		///< Current read position
	int				m_nDataSize;			///< Wave data size, in bytes
	bool			m_fActive;				///< Playback active flag
	LPDIRECTSOUNDBUFFER m_pStream;			///< Sound buffers
	int				m_nTimerID;				///< Timer ID, this instance
	WAVEFORMATEX	*m_pWaveFormat;			///< Wave format block
	MMCKINFO		m_rRiffData;			///< Wave RIFF block
	bool			m_fEOF;					///< At end of WAVE file
	int				m_nOffset;				///< WriteCursor position
	DWORD			m_EndPos;				///< PlayCursor position when end of audio file playback
	DWORD			m_PlayPos;				///< PlayCursor position when audio file fully read into buffer
	bool			m_bLoops;				///< Loop this WAVE or not?
	HANDLE			m_EndEvent;
	bool			m_bMp3;
	bool			m_bOgg;
	CMp3Manager		*m_pMp3;
	OggAudio		*m_pOgg;
	float			m_Volume;
	float			m_Pan;
	float			m_Frequency;
	DWORD			m_BaseFrequency;
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
