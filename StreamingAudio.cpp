/************************************************************************************//**
 * @file StreamingAudio.cpp
 * @brief Streaming audio encapsulation class
 *
 * This file contains the class implementation of audio streaming services,
 * based on the SoundServices functionality.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All Rights Reserved
 ****************************************************************************************/

#include "RabidFramework.h"
#include "OggAudio.h"
#include "CMp3.h"


/* ------------------------------------------------------------------------------------ */
// StreamingAudio
//
// Default constructor, takes a DirectSound pointer
/* ------------------------------------------------------------------------------------ */
StreamingAudio::StreamingAudio(LPDIRECTSOUND lpDS) :
	m_pDS(lpDS),			// Save DSound pointer
	m_hWaveFile(NULL),		// Handle to WAVE file
	m_nDataPosition(0),		// Start of WAVE data in file
	m_nCurrentPosition(0),	// Current read position
	m_nDataSize(0),			// Wave data size, in bytes
	m_fActive(false),		// Playback active flag
	m_pStream(NULL),		// Sound buffer
	m_nTimerID(0),			// Pumping timer ID
	m_pWaveFormat(NULL),	// Wave file format
	m_fEOF(false),			// Not at end of file
	m_nOffset(0),
	m_EndPos(0),
	m_PlayPos(0),
	m_bLoops(false),		// Assume no looping
	m_EndEvent(NULL),
	m_bMp3(false),
	m_bOgg(false),
	m_pMp3(NULL),
	m_pOgg(NULL),
	m_Volume(1.0f),			// [0,1]
	m_Pan(0.0f),			// [0,1]
	m_Frequency(1.0f),
	m_BaseFrequency(1)
{
}

/* ------------------------------------------------------------------------------------ */
// ~StreamingAudio
//
// Default destructor
/* ------------------------------------------------------------------------------------ */
StreamingAudio::~StreamingAudio()
{
	if(m_bOgg)
	{
		if(m_pOgg)
		{
			m_pOgg->Stop();
			delete m_pOgg;
		}
		return;
	}

	timeKillEvent(m_nTimerID);			// Stop the timer

	if(m_bMp3)
	{
		if(m_pMp3)
		{
			m_pMp3->StopMp3();
			delete m_pMp3;
		}
		return;
	}

	m_fActive = false;					// Inactive now

	if(m_hWaveFile != NULL)
	{
		mmioClose(m_hWaveFile, 0);
		m_hWaveFile = NULL;
		delete m_pWaveFormat;
	}

	if(m_pStream != NULL)
	{
		m_pStream->Release();			// Drop the DSound buffer
	}
}

/* ------------------------------------------------------------------------------------ */
// Create
//
// Creates a new audio stream and sets up a timer for it.
/* ------------------------------------------------------------------------------------ */
int StreamingAudio::Create(char *szFileName)
{
	int nError = 0;

	CCD->Log()->Debug("Loaded %s", szFileName);

	// Sanity check parameters
	if(szFileName == NULL)
		return RGF_FAILURE;										// Wrong.

	int len = strlen(szFileName) - 4;

	if(stricmp((szFileName+len), ".mp3") == 0)
	{
		if(m_pMp3 != NULL)
			return RGF_FAILURE;

		m_pMp3 = new CMp3Manager;
		m_pMp3->OpenMediaFile(szFileName);
		m_bMp3 = true;
		m_fActive = true;

		// start a timer for this stream.
		MMRESULT nTimer = timeSetEvent(125, 5, &TimerFunction, reinterpret_cast<DWORD>(this),
										TIME_PERIODIC | TIME_CALLBACK_FUNCTION);

		if(nTimer == NULL)
		{
			CCD->Log()->Warning("StreamingAudio: Timer callback set-up failed");
			return RGF_FAILURE;									// Timer startup failed.
		}

		m_nTimerID = nTimer;					// Save timer ID

		return RGF_SUCCESS;
	}

	m_bMp3 = false;

	if(stricmp((szFileName+len), ".ogg") == 0)
	{
		if(m_pOgg != NULL)
			return RGF_FAILURE;

		m_pOgg = new OggAudio(m_pDS);
		m_pOgg->Load(szFileName);
		m_bOgg = true;
		m_fActive = true;
		m_BaseFrequency = m_pOgg->GetBaseFrequency();

		return RGF_SUCCESS;
	}

	m_bOgg = false;

	// start a timer for this stream.
	MMRESULT nTimer = timeSetEvent(125, 5, &TimerFunction, reinterpret_cast<DWORD>(this),
						TIME_PERIODIC | TIME_CALLBACK_FUNCTION);

	if(nTimer == NULL)
	{
		CCD->Log()->Warning("StreamingAudio: Timer callback set-up failed");
		return RGF_FAILURE;									// Timer startup failed.
	}

	m_nTimerID = nTimer;									// Save timer ID

	// Check for stream availability
	if(m_pStream != NULL)
		return RGF_FAILURE;									// Already loaded!

	// Open up the WAVE file.
	nError = WaveOpen(szFileName, &m_hWaveFile, &m_pWaveFormat,	&m_rRiffData);

	if(nError != RGF_SUCCESS)
	{
		CCD->Log()->Warning("File %s - Line %d: Failed to open wave file '%s'\n(Note: streamig audio can't be placed in VFS)",
							__FILE__, __LINE__, szFileName);
		return RGF_FAILURE;									// Problem here, too!
	}

	// Get current position in file, which will be the start of the WAVE data.
	m_nDataPosition = mmioSeek(m_hWaveFile, 0, SEEK_CUR);
	m_nDataSize = m_rRiffData.cksize;						// Save data size
	m_EndPos = m_nDataSize % kBufferSize;
	m_EndEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	DSBPOSITIONNOTIFY notify;
	notify.dwOffset = m_EndPos;
	notify.hEventNotify = m_EndEvent;

	// Fetch DirectSound interface we want
	LPDIRECTSOUND pDSIF;
	nError = m_pDS->QueryInterface(IID_IDirectSound, reinterpret_cast<LPVOID*>(&pDSIF));

	// Create a DSound buffer to stream into
	DSBUFFERDESC theDesc;

	memset(&theDesc, 0, sizeof(DSBUFFERDESC));
	theDesc.dwSize = sizeof(DSBUFFERDESC);
	theDesc.dwBufferBytes = kBufferSize;
	theDesc.lpwfxFormat = m_pWaveFormat;
	theDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN | DSBCAPS_CTRLFREQUENCY | DSBCAPS_GETCURRENTPOSITION2 |
						DSBCAPS_CTRLPOSITIONNOTIFY;

	nError = pDSIF->CreateSoundBuffer(&theDesc, &m_pStream, NULL);
	m_pStream->GetFrequency(&m_BaseFrequency);
	pDSIF->Release();									// Done w/ this.

	if(nError != 0)										// Error!  Sick out.
	{
		char szBug[128];
		sprintf(szBug, "StreamingAudio: Failed to create buffer for '%s'\n", szFileName);
		OutputDebugString(szBug);

		mmioClose(m_hWaveFile, 0);
		m_hWaveFile = NULL;
		SAFE_DELETE(m_pWaveFormat);

		return RGF_FAILURE;
	}

	LPDIRECTSOUNDNOTIFY lpDsNotify;
	HRESULT hr;

	if(SUCCEEDED(hr = m_pStream->QueryInterface(IID_IDirectSoundNotify, (LPVOID*)&lpDsNotify)))
	{
		hr = lpDsNotify->SetNotificationPositions(1, &notify);
		lpDsNotify->Release();
	}

	m_nOffset = 0;						// Start at top of buffer
	m_PlayPos = 0;
	PumpWave(kBufferSize);				// Initial buffer load

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// Play
//
// Start this stream buffer playing, with or without looping.
/* ------------------------------------------------------------------------------------ */
int StreamingAudio::Play(bool bLooping)
{
	if(m_bMp3)
	{
		if(m_pMp3 == NULL)
			return RGF_FAILURE;

		m_fActive = true;
		m_bLoops = bLooping;
		m_Volume = CCD->MenuManager()->GetmVolLevel();
		m_pMp3->PlayMp3(static_cast<long>(CCD->MenuManager()->GetmVolLevel()*1000.0f), bLooping);

		return RGF_SUCCESS;
	}

	m_Volume = CCD->MenuManager()->GetmVolLevel();
	long volume = static_cast<long>(CCD->MenuManager()->GetmVolLevel() * 10000.0f);

	if(volume <= 1)
		volume = -10000;
	else if(volume > 10000)
		volume = 0;
	else
		volume = static_cast<long>(log10(static_cast<float>(volume)) * 2500.f - 10000.f);

	if(m_bOgg)
	{
		if(m_pOgg == NULL)
			return RGF_FAILURE;

		m_fActive = true;
		m_bLoops = bLooping;
		m_pOgg->Play(bLooping);
		m_pOgg->SetVolume(volume);

		return RGF_SUCCESS;
	}

	// Check for stream availability
	if(m_pStream == NULL)
		return RGF_FAILURE;							// No stream

	if(m_fActive == true)
		return RGF_SUCCESS;

	m_fActive = true;
	m_bLoops = bLooping;

	m_pStream->Play(0, 0, DSBPLAY_LOOPING);			// Start playback
	m_pStream->SetVolume(volume);

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// Stop
//
// Stop this stream from playing back.
/* ------------------------------------------------------------------------------------ */
int StreamingAudio::Stop()
{
	if(m_bMp3)
	{
		m_fActive = false;

		if(m_pMp3 == NULL)
			return RGF_FAILURE;

		m_pMp3->StopMp3();
		return RGF_SUCCESS;
	}

	if(m_bOgg)
	{
		m_fActive = false;

		if(m_pOgg == NULL)
			return RGF_FAILURE;

		m_pOgg->Stop();
		return RGF_SUCCESS;
	}

	// Check for stream availability
	if(m_pStream == NULL)
		return RGF_FAILURE;							// No stream

	m_fActive = false;

	m_pStream->Stop();								// Stop playback

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// Pause
//
// Pause/unpause this stream's playback.
/* ------------------------------------------------------------------------------------ */
int StreamingAudio::Pause()
{
	if(m_bMp3)
	{
		if(m_pMp3 == NULL)
			return RGF_FAILURE;

		if(m_fActive == false)
		{
			m_fActive = true;
			m_pMp3->PlayMp3(static_cast<LONG>(CCD->MenuManager()->GetmVolLevel()*1000.0f), m_bLoops);
		}
		else
		{
			m_fActive = false;
			m_pMp3->StopMp3();
		}

		return RGF_SUCCESS;
	}

	if(m_bOgg)
	{
		if(m_pOgg == NULL)
			return RGF_FAILURE;

		m_pOgg->Pause();
		return RGF_SUCCESS;
	}

	// Check for stream availability
	if(m_pStream == NULL)
		return RGF_FAILURE;								// No stream

	if(m_fActive == false)
		m_fActive = true;
	else
		m_fActive = false;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// Delete
//
// Delete this stream - stop it and kill the timer.
/* ------------------------------------------------------------------------------------ */
int StreamingAudio::Delete()
{
	if(m_bOgg)
	{
		if(m_pOgg == NULL)
			return RGF_FAILURE;

		m_pOgg->Stop();
		SAFE_DELETE(m_pOgg);

		return RGF_SUCCESS;
	}

	timeKillEvent(m_nTimerID);							// Kill timer

	if(m_bMp3)
	{
		if(m_pMp3 == NULL)
			return RGF_FAILURE;

		m_pMp3->StopMp3();
		SAFE_DELETE(m_pMp3);

		return RGF_SUCCESS;
	}

	// Check for stream availability
	if(m_pStream == NULL)
		return RGF_FAILURE;								// No stream

	Stop();												// Stop playback

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// Rewind
//
// Reposition stream playback to the start of the wave data
/* ------------------------------------------------------------------------------------ */
int StreamingAudio::Rewind()
{
	if(m_bMp3)
	{
		if(m_pMp3 == NULL)
			return RGF_FAILURE;

		m_pMp3->Rewind();
		return RGF_SUCCESS;
	}

	if(m_bOgg)
	{
		if(m_pOgg == NULL)
			return RGF_FAILURE;

		m_pOgg->Rewind();
		return RGF_SUCCESS;
	}

	// Check for stream availability
	if(m_pStream == NULL)
		return RGF_FAILURE;								// No stream

	// Check to be sure it's active
	if(m_fActive == false)
		return RGF_FAILURE;								// Not an active stream

	// Ok, position to the start of the WAVE DATA
	mmioSeek(m_hWaveFile, m_nDataPosition, SEEK_SET);

	m_rRiffData.cksize = m_nDataSize;					// Restore data size

	m_fEOF = false;										// Not EOF any longer

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// IsPlaying
//
// Returns TRUE if the stream is actively playing, FALSE otherwise
// ..If the stream is at EOF that qualifies as "not playing".
/* ------------------------------------------------------------------------------------ */
bool StreamingAudio::IsPlaying()
{
	if(m_bMp3)
	{
		if(!m_pMp3)
			return false;

		return m_pMp3->IsPlaying();
	}

	if(m_bOgg)
	{
		if(!m_pOgg)
			return false;

		return m_pOgg->IsPlaying();
	}

	// Check for stream availability
	if(m_pStream == NULL)
		return false;									// No stream

	if(m_fActive == false)
		return false;

	if(m_fEOF == true)
	{
		if(WaitForSingleObject(m_EndEvent,0) == WAIT_OBJECT_0)
		{
			Stop();
			return false;							// Not playing
		}
	}

	return true;										// Is playing
}

/* ------------------------------------------------------------------------------------ */
// SetVolume
//
// Set the playback volume of the audio stream.
/* ------------------------------------------------------------------------------------ */
int StreamingAudio::SetVolume(long volume)
{
	if(m_bMp3)
	{
		if(m_pMp3 == NULL)
			return RGF_FAILURE;

		//m_pMp3->SetVolume((volume + 10000) / 10);
		// [0; 1000]
		volume = static_cast<long>(pow(10.f, (volume + 10000.f) / 2500.f - 1.f));

		m_pMp3->SetVolume(volume);

		return RGF_SUCCESS;
	}

	if(m_bOgg)
	{
		if(m_pOgg == NULL)
			return RGF_FAILURE;

		m_pOgg->SetVolume(volume);
		return RGF_SUCCESS;
	}

	{
		if(m_pStream == NULL)
			return RGF_FAILURE;

		// Sanity check parameter
		if(volume > DSBVOLUME_MAX || volume < DSBVOLUME_MIN)
			return RGF_FAILURE;						// Bad parameter

		m_pStream->SetVolume(volume);				// Set it!
	}

	return RGF_SUCCESS;
}


int StreamingAudio::SetPan(long pan)
{
	if(m_bMp3)
	{
		if(m_pMp3 == NULL)
			return RGF_FAILURE;

		// no way to set pan for mp3
		return RGF_SUCCESS;
	}

	if(m_bOgg)
	{
		if(m_pOgg == NULL)
			return RGF_FAILURE;

		m_pOgg->SetPan(pan);
		return RGF_SUCCESS;
	}

	{
		if(m_pStream == NULL)
			return RGF_FAILURE;

		if(pan > DSBPAN_RIGHT || pan < DSBPAN_LEFT)
			return RGF_FAILURE;

		m_pStream->SetPan(pan);
	}

	return RGF_SUCCESS;
}


int StreamingAudio::SetFrequency(unsigned long frequency)
{
	if(m_bMp3)
	{
		if(m_pMp3 == NULL)
			return RGF_FAILURE;

		// no way to set frequency for mp3
		return RGF_SUCCESS;
	}

	if(m_bOgg)
	{
		if(m_pOgg == NULL)
			return RGF_FAILURE;

		m_pOgg->SetFrequency(frequency);
		return RGF_SUCCESS;
	}

	{
		if(m_pStream == NULL)
			return RGF_FAILURE;

		m_pStream->SetFrequency(frequency);
	}

	return RGF_SUCCESS;
}


int StreamingAudio::Modify3D(geVec3d *soundPos, float minRadius)
{
	if(!soundPos)
	{
		return RGF_FAILURE;
	}

	if(m_bMp3)
	{
		return RGF_SUCCESS;
	}

	geXForm3d camera;
	float volDelta, panDelta;
	float volume, pan, frequency;

	// get the camera xform
	camera = CCD->CameraManager()->ViewPoint();

	// get 3d sound values
	geSound3D_GetConfig(CCD->World(),
						&camera,
						soundPos,
						minRadius,
						0.0f,
						&volume,
						&pan,
						&frequency);

	// return true or false depending on whether or not its worth modifying the sound
	volDelta = m_Volume - volume;

	if(volDelta < 0.0f)
	{
		volDelta = -volDelta;
	}

	panDelta = m_Pan - pan;

	if(panDelta < 0.0f)
	{
		panDelta = -panDelta;
	}

	if((volDelta < 0.03f) && (panDelta < 0.02f))
	{
		return RGF_SUCCESS;
	}

	long lVolume = static_cast<long>((1.0f - volume) * DSBVOLUME_MIN);
	if(SetVolume(lVolume) != RGF_SUCCESS)
		return RGF_FAILURE;
	m_Volume = volume;

	long lPan = static_cast<long>(pan * DSBPAN_RIGHT);
	if(SetPan(lPan) != RGF_SUCCESS)
		return RGF_FAILURE;
	m_Pan = pan;

	// TODO
	unsigned long ulFreq = 1;//static_cast<unsigned long>(channel->BaseFreq * frequency);

	//if(ulFreq < 0)
	//	ulFreq = 0;

	if(SetFrequency(ulFreq) != RGF_SUCCESS)
		return RGF_FAILURE;
	m_Frequency = frequency;

	return RGF_SUCCESS;
}

// ******************* PRIVATE MEMBER FUNCTIONS **********************

/* ------------------------------------------------------------------------------------ */
// PumpWave
//
// Fill the streaming audio buffer from the wave file, making sure that
// ..there's always something (up to and including silence) in the
// ..buffer for playback.
/* ------------------------------------------------------------------------------------ */
int StreamingAudio::PumpWave(int nSize)
{
	HRESULT hr;
	void *lpbuf1 = NULL, *lpbuf2 = NULL;
	DWORD dwsize1 = 0, dwsize2 = 0;
	UINT nBytesRead = 0;

	// Ok, we need to set up our "silence" value and adjust it for
	// ..8bit samples if needed.

	int nSilence = 0x0;

	if(m_pWaveFormat->wBitsPerSample == 8)
		nSilence = 0x80;									// In case wave is 8-bit

	// Ok, try to lock <n>K of the buffer.  If it fails, just bail this
	// ..function.
	hr = m_pStream->Lock(m_nOffset, nSize, &lpbuf1, &dwsize1, &lpbuf2, &dwsize2, 0);

	if(hr != DS_OK)
	{
		CCD->Log()->Warning("StreamingAudio::PumpWave: Failed to lock");
		return RGF_SUCCESS;									// Fake it, bail out
	}

	if(lpbuf1 != NULL)
		memset(lpbuf1, nSilence, dwsize1);					// Clear to silence

	if(lpbuf2 != NULL)
		memset(lpbuf2, nSilence, dwsize2);					// Here, also

	m_nOffset = (m_nOffset + dwsize1 + dwsize2) % kBufferSize;

	if(m_fEOF == true)
	{
		m_pStream->Unlock(lpbuf1, dwsize1, lpbuf2, dwsize2);
		return RGF_SUCCESS;									// End of file, pump silence
	}

	if(m_fActive == false)
	{
		m_pStream->Unlock(lpbuf1, dwsize1, lpbuf2, dwsize2);
		return RGF_SUCCESS;									// Inactive, pump silence
	}

	// Fine, read data into the circular buffer directly from the
	// ..wave file if there's anything there.

	// Fill block #1
	hr = WaveRead(m_hWaveFile, dwsize1, static_cast<BYTE*>(lpbuf1), &m_rRiffData, &nBytesRead);

	if(nBytesRead != dwsize1)								// End of wave file
	{
		m_fEOF = true;
		// m_pStream->Unlock(lpbuf1, dwsize1, lpbuf2, dwsize2);
		// return RGF_SUCCESS;
	}

	if((lpbuf2 != NULL) && (!m_fEOF))
	{
		// Fill block #2
		hr = WaveRead(m_hWaveFile, dwsize2, static_cast<BYTE*>(lpbuf2), &m_rRiffData, &nBytesRead);

		if(nBytesRead != dwsize2)							// End of wave file
			m_fEOF = true;
	}

	// Unlock buffer, we're done with it for now.
	m_pStream->Unlock(lpbuf1, dwsize1, lpbuf2, dwsize2);

	// Ok, if we're at the end of file AND we're flagged to loop, rewind to the
	// ..beginning of the buffer so we start from the top next time through.
	if(m_fEOF)
	{
		if(m_bLoops)
		{
			Rewind();		// Hope the sound designer looped the WAVE right!
		}
		else
		{
			m_pStream->GetCurrentPosition(&m_PlayPos, NULL);
			ResetEvent(m_EndEvent);		// Next time event is set, we're done playing
		}
	}


	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// GetMaxWriteSize
//
// Get the maximum number of bytes we can write into the current buffer.
/* ------------------------------------------------------------------------------------ */
DWORD StreamingAudio::GetMaxWriteSize()
{
	DWORD dwWriteCursor, dwPlayCursor, dwMaxSize;

	// Get current play position
	if(m_pStream->GetCurrentPosition(&dwPlayCursor, &dwWriteCursor) == DS_OK)
	{
		if(static_cast<DWORD>(m_nOffset) <= dwPlayCursor)
		{
			// Our write position trails play cursor
			dwMaxSize = dwPlayCursor - m_nOffset;
		}
		else // (m_cbBufOffset > dwPlayCursor)
		{
			// Play cursor has wrapped
			dwMaxSize = kBufferSize - m_nOffset + dwPlayCursor;
		}
	}
	else
	{
		dwMaxSize = 0;
	}

	return (dwMaxSize & 0xfffffffe);
}

/* ------------------------------------------------------------------------------------ */
// TimerFunction
//
// This function takes care of periodically pumping audio into
// ..the playback buffer for streaming waves.  Based on the
// ..user information passed in (set up when the wave file was
// ..started streaming) it calls into the instance that is needed.
// ..Yeah, this is a little wacky, but it let's me stream the wave
// ..out without having to continually poll and stuff buffers.
/* ------------------------------------------------------------------------------------ */
void CALLBACK StreamingAudio::TimerFunction(UINT /*uID*/, UINT /*uMsg*/,
											DWORD dwUser, DWORD /*dw1*/, DWORD /*dw2*/)
{
	StreamingAudio *thePointer = reinterpret_cast<StreamingAudio*>(dwUser);

	if(thePointer->m_bMp3)
	{
		if(thePointer->m_pMp3 == NULL)
			return;

		if(thePointer->m_fActive && thePointer->m_bLoops)
			thePointer->m_pMp3->Refresh();

		return;
	}

	DWORD nSize = thePointer->GetMaxWriteSize();

	if(nSize != 0)
		thePointer->PumpWave(nSize);					// Pump stream
}

/* ------------------------------------------------------------------------------------ */
// WaveOpen
//
// Open up a wave file, validate it, and return a handle to the open
// ..file and a WAVEFORMATEX structure containing the wave info.
/* ------------------------------------------------------------------------------------ */
int StreamingAudio::WaveOpen(char *szFileName, HMMIO *pFileID,
							 WAVEFORMATEX **ppwfxInfo,	MMCKINFO *pckInRIFF)
{
	HMMIO hmmioIn = NULL;
	MMCKINFO ckIn;					// chunk info. for general use.
	PCMWAVEFORMAT pcmWaveFormat;	// Temp PCM structure to load in.
	WORD cbExtraAlloc = 0;			// Extra bytes for waveformatex

	// Set up for the job
	*ppwfxInfo = NULL;

	if((hmmioIn = mmioOpen(szFileName, NULL, MMIO_ALLOCBUF | MMIO_READ)) == NULL)
		return -1;

	if(mmioDescend(hmmioIn, pckInRIFF, NULL, 0) != 0)
	{
		mmioClose(hmmioIn, 0);					// Clean up
		return -1;
	}

	// Check to make sure we're really opening a WAVE file
	if((pckInRIFF->ckid != FOURCC_RIFF) || (pckInRIFF->fccType != mmioFOURCC('W', 'A', 'V', 'E')))
	{
		mmioClose(hmmioIn, 0);				// Clean up
		return -2;
	}

	// Search the input file for for the 'fmt ' chunk.
	ckIn.ckid = mmioFOURCC('f', 'm', 't', ' ');

	if(mmioDescend(hmmioIn, &ckIn, pckInRIFF, MMIO_FINDCHUNK) != 0)
	{
		mmioClose(hmmioIn, 0);				// Clean up
		return -3;
	}

	// Expect the 'fmt' chunk to be at least as large as <PCMWAVEFORMAT>;
	// if there are extra parameters at the end, we'll ignore them.
	if(ckIn.cksize < static_cast<DWORD>(sizeof(PCMWAVEFORMAT)))
	{
		mmioClose(hmmioIn, 0);				// Clean up
		return -4;
	}

	// Read the 'fmt ' chunk into <pcmWaveFormat>
	if(mmioRead(hmmioIn, (HPSTR) &pcmWaveFormat, static_cast<LONG>(sizeof(pcmWaveFormat))) != static_cast<LONG>(sizeof(pcmWaveFormat)))
	{
		mmioClose(hmmioIn, 0);				// Clean up
		return -5;
	}

	// Ok, allocate the waveformatex, but if its not pcm
	// format, read the next word, and thats how many extra
	// bytes to allocate.
	if(pcmWaveFormat.wf.wFormatTag == WAVE_FORMAT_PCM)
	{
		cbExtraAlloc = 0;
	}
	else
	{
		// Read in length of extra bytes.
		if(mmioRead(hmmioIn, reinterpret_cast<LPSTR>(&cbExtraAlloc), static_cast<LONG>(sizeof(cbExtraAlloc)))
			!= static_cast<LONG>(sizeof(cbExtraAlloc)))
		{
			mmioClose(hmmioIn, 0);			// Clean up
			return -6;
		}
	}

	// Ok, now allocate that waveformatex structure.
	if((*ppwfxInfo = static_cast<WAVEFORMATEX*>(malloc(sizeof(WAVEFORMATEX) + cbExtraAlloc))) == NULL)
	{
		mmioClose(hmmioIn, 0);				// Clean up
		return -7;
	}

	// Copy the bytes from the pcm structure to the waveformatex structure
	memcpy(*ppwfxInfo, &pcmWaveFormat, sizeof(pcmWaveFormat));
	(*ppwfxInfo)->cbSize = cbExtraAlloc;

	// Now, read those extra bytes into the structure, if cbExtraAlloc != 0.
	if(cbExtraAlloc != 0)
	{
		if(mmioRead(hmmioIn, (LPSTR)(((BYTE*)&((*ppwfxInfo)->cbSize)) + sizeof(cbExtraAlloc)), static_cast<LONG>(cbExtraAlloc))
			!= static_cast<LONG>(cbExtraAlloc))
		{
			mmioClose(hmmioIn, 0);			// Clean up
			return -8;
		}
	}

	// Ascend the input file out of the 'fmt ' chunk
	if(mmioAscend(hmmioIn, &ckIn, 0) != 0)
	{
		mmioClose(hmmioIn, 0);				// Clean up
		return -9;
	}

	// Search the input file for for the 'data' chunk.
	ckIn.ckid = mmioFOURCC('d', 'a', 't', 'a');

	if(mmioDescend(hmmioIn, &ckIn, pckInRIFF, MMIO_FINDCHUNK) != 0)
	{
		mmioClose(hmmioIn, 0);				// Clean up
		return -10;
	}

	memcpy(pckInRIFF, &ckIn, sizeof(MMCKINFO));			// Set up for later use.

	// Ok, return the handle to the file for later use...
	*pFileID = hmmioIn;

	return RGF_SUCCESS;									// Ok, it worked!
}

/* ------------------------------------------------------------------------------------ */
// This will read wave data from the wave file.  Makre sure we're descended into
// the data chunk, else this will fail bigtime!
// hmmioIn         - Handle to mmio.
// cbRead          - # of bytes to read.
// pbDest          - Destination buffer to put bytes.
// cbActualRead- # of bytes actually read.
/* ------------------------------------------------------------------------------------ */
int StreamingAudio::WaveRead(HMMIO hmmioIn, int nSizeToRead, BYTE *pDestination,
								MMCKINFO *pckIn, UINT *nBytesRead)
{
	if(pckIn->cksize <= 0)
		return -1;							// No data bytes left!

	if(static_cast<DWORD>(nSizeToRead) > pckIn->cksize)
		nSizeToRead = pckIn->cksize;		// Only read what's left

	LONG nRead = mmioRead(hmmioIn, reinterpret_cast<HPSTR>(pDestination), static_cast<LONG>(nSizeToRead));
	pckIn->cksize -= nRead;					// Decrement count

	*nBytesRead = nRead;					// Save byte count.

	if(nRead != nSizeToRead)
		return -1;							// End of file

	return 0;								// Worked fine.
}


/* ----------------------------------- END OF FILE ------------------------------------ */
