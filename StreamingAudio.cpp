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

// As always, we include the One True Header File
#include "RabidFramework.h"

//	************************ BEGIN IMPLEMENTATION ***************************

/* ------------------------------------------------------------------------------------ */
//	StreamingAudio
//
//	Default constructor, takes a DirectSound pointer
/* ------------------------------------------------------------------------------------ */
StreamingAudio::StreamingAudio(LPDIRECTSOUND lpDS)
{
	m_nDataPosition		= 0;		// Start of WAVE data in file
	m_nCurrentPosition	= 0;		// Current read position
	m_nDataSize			= 0;		// Wave data size, in bytes
	m_fActive			= false;	// Playback active flag
	m_pStream			= NULL;		// Sound buffer
	m_nTimerID			= 0;		// Pumping timer ID
	m_hWaveFile			= NULL;		// Handle to WAVE file
	m_pWaveFormat		= NULL;		// Wave file format
	m_pDS				= lpDS;		// Save DSound pointer
	m_fEOF				= false;	// Not at end of file
	m_bLoops			= false;	// Assume no looping
	Mpeg3				= NULL;
	Ogg					= NULL;
	mp3					= false;
	ogg					= false;

	return;
}

/* ------------------------------------------------------------------------------------ */
//	~StreamingAudio
//
//	Default destructor
/* ------------------------------------------------------------------------------------ */
StreamingAudio::~StreamingAudio()
{
	if(ogg)
	{
		if(Ogg)
		{
			Ogg->Stop();
			delete Ogg;
			Ogg = NULL;
		}

		return;
	}

	timeKillEvent(m_nTimerID);			// Stop the timer

	if(mp3)
	{
		if(Mpeg3)
		{
			Mpeg3->StopMp3();
			delete Mpeg3;
			Mpeg3 = NULL;
		}
		return;
	}

	m_fActive = false;					// Inactive now

	if(m_hWaveFile != NULL)
	{
		mmioClose(m_hWaveFile, 0);
		m_hWaveFile = NULL;
		delete m_pWaveFormat;
		m_pWaveFormat = NULL;
	}

	if(m_pStream != NULL)
	{
		m_pStream->Release();			// Drop the DSound buffer
		m_pStream = NULL;
	}

	return;
}

/* ------------------------------------------------------------------------------------ */
//	Create
//
//	Creates a new audio stream and sets up a timer for it.
/* ------------------------------------------------------------------------------------ */
int StreamingAudio::Create(char *szFileName)
{
	int nError = 0;

	if(CCD->GetLogging())
	{
		char szDebug[512];
		sprintf(szDebug, "[INFO] Loaded %s", szFileName);
		CCD->ReportError(szDebug, false);
	}

	// Sanity check parameters
	if(szFileName == NULL)
		return RGF_FAILURE;										// Wrong.

// changed RF064
	int len = strlen(szFileName)-4;
	if(stricmp((szFileName+len),".mp3") == 0)
	{
		if(Mpeg3 != NULL)
			return RGF_FAILURE;
		Mpeg3 = new CMp3Manager;
		Mpeg3->OpenMediaFile(szFileName);
		mp3 = true;
		m_fActive = true;

		// start a timer for this stream.

		MMRESULT nTimer = timeSetEvent(125, 5, &TimerFunction, reinterpret_cast<DWORD>(this),
										TIME_PERIODIC | TIME_CALLBACK_FUNCTION);

		if(nTimer == NULL)
		{
			CCD->ReportError("[WARNING] StreamingAudio: Timer callback set-up failed", false);
			return RGF_FAILURE;									// Timer startup failed.
		}

		m_nTimerID = nTimer;					// Save timer ID
		return RGF_SUCCESS;
	}

	mp3 = false;

	if(stricmp((szFileName+len), ".ogg") == 0)
	{
		if(Ogg != NULL)
			return RGF_FAILURE;

		Ogg = new OggAudio(m_pDS);
		Ogg->Load(szFileName);
		ogg = true;
		m_fActive = true;
		return RGF_SUCCESS;
	}

	ogg = false;

	// start a timer for this stream.
	MMRESULT nTimer = timeSetEvent(125, 5, &TimerFunction, reinterpret_cast<DWORD>(this),
						TIME_PERIODIC | TIME_CALLBACK_FUNCTION);

	if(nTimer == NULL)
	{
		CCD->ReportError("[WARNING] StreamingAudio: Timer callback set-up failed", false);
		return RGF_FAILURE;									// Timer startup failed.
	}

	m_nTimerID = nTimer;									// Save timer ID
// end change RF064

	//	Check for stream availability
	if(m_pStream != NULL)
		return RGF_FAILURE;									// Already loaded!

	// Open up the WAVE file.
	nError = WaveOpen(szFileName, &m_hWaveFile, &m_pWaveFormat,	&m_rRiffData);

	if(nError != RGF_SUCCESS)
	{
		char szBug[128];
		sprintf(szBug, "[WARNING] File %s - Line %d: Failed to open wave file '%s'\n(Note: streamig audio can't be placed in VFS)",
				__FILE__, __LINE__, szFileName);
		CCD->ReportError(szBug, false);
		return RGF_FAILURE;									// Problem here, too!
	}

	// Get current position in file, which will be the start of the
	// ..WAVE data.
	m_nDataPosition = mmioSeek(m_hWaveFile, 0, SEEK_CUR);
	m_nDataSize = m_rRiffData.cksize;						// Save data size

	// Fetch DirectSound interface we want
	LPDIRECTSOUND pDSIF;
	nError = m_pDS->QueryInterface(IID_IDirectSound, reinterpret_cast<LPVOID*>(&pDSIF));

	// Create a DSound buffer to stream into
	DSBUFFERDESC theDesc;

	memset(&theDesc, 0, sizeof(DSBUFFERDESC));
	theDesc.dwSize = sizeof(DSBUFFERDESC);
	theDesc.dwBufferBytes = kBufferSize;
	theDesc.lpwfxFormat = m_pWaveFormat;
	theDesc.dwFlags = DSBCAPS_CTRLVOLUME;

	nError = pDSIF->CreateSoundBuffer(&theDesc, &m_pStream, NULL);

	pDSIF->Release();									// Done w/ this.

	if(nError != 0)										// Error!  Sick out.
	{
		char szBug[128];
		sprintf(szBug, "StreamingAudio: Failed to create buffer for '%s'\n", szFileName);
		OutputDebugString(szBug);
		mmioClose(m_hWaveFile, 0);
		m_hWaveFile = NULL;
		delete m_pWaveFormat;
		m_pWaveFormat = NULL;
		return RGF_FAILURE;
	}

	m_nOffset = 0;								// Start at top of buffer
// changed RF064
	//m_fActive = true;							// Set as active
// end change RF064
	PumpWave(kBufferSize);				// Initial buffer load

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	Play
//
//	Start this stream buffer playing, with or without looping.
/* ------------------------------------------------------------------------------------ */
int StreamingAudio::Play(bool bLooping)
{
	if(mp3)
	{
		if(Mpeg3 == NULL)
			return RGF_FAILURE;
		m_fActive = true;
		m_bLoops = bLooping;
		Mpeg3->PlayMp3((LONG)(CCD->MenuManager()->GetmVolLevel()*1000.0f), bLooping);
		return RGF_SUCCESS;
	}

	LONG ScaledVolume;
	LONG nVolume = (LONG)(CCD->MenuManager()->GetmVolLevel()*10000.0f);

	if(nVolume <= 1)
		ScaledVolume = -10000;
	else if(nVolume > 10000)
		ScaledVolume = 0;
	else
		//ScaledVolume = (LONG)(log10(nVolume)/4*10000 - 10000);
		ScaledVolume = (LONG)(log10((double)nVolume)*2500 - 10000);
	nVolume = ScaledVolume;

	if(ogg)
	{
		if(Ogg == NULL)
			return RGF_FAILURE;
		m_fActive = true;
		m_bLoops = bLooping;
		Ogg->Play(bLooping);
		SetVolume(nVolume);
		return RGF_SUCCESS;
	}

	// Check for stream availability
	if(m_pStream == NULL)
		return RGF_FAILURE;							// No stream

// changed RF064
	if(m_fActive == true)
		return RGF_SUCCESS;
// end change RF064

	m_fActive = true;
	m_bLoops = bLooping;

	m_pStream->Play(0, 0, DSBPLAY_LOOPING);			// Start playback
	SetVolume(nVolume);

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	Stop
//
//	Stop this stream from playing back.
/* ------------------------------------------------------------------------------------ */
int StreamingAudio::Stop()
{
	if(mp3)
	{
// changed RF064
		m_fActive = false;

		if(Mpeg3 == NULL)
			return RGF_FAILURE;

		Mpeg3->StopMp3();
		return RGF_SUCCESS;
// end change RF064
	}

	if(ogg)
	{
		m_fActive = false;

		if(Ogg == NULL)
			return RGF_FAILURE;

		Ogg->Stop();
		return RGF_SUCCESS;
	}

	//Check for stream availability
	if(m_pStream == NULL)
		return RGF_FAILURE;							// No stream

	m_fActive = false;

	m_pStream->Stop();								// Stop playback

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	Pause
//
//	Pause/unpause this stream's playback.
/* ------------------------------------------------------------------------------------ */
int StreamingAudio::Pause()
{
	if(mp3)
	{
		if(Mpeg3 == NULL)
			return RGF_FAILURE;

		if(m_fActive == false)
		{
			m_fActive = true;
			Mpeg3->PlayMp3((LONG)(CCD->MenuManager()->GetmVolLevel()*1000.0f), m_bLoops);
		}
		else
		{
			m_fActive = false;
			Mpeg3->StopMp3();
		}

		return RGF_SUCCESS;
	}

	if(ogg)
	{
		if(Ogg == NULL)
			return RGF_FAILURE;

		Ogg->Pause();
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
//	Delete
//
//	Delete this stream - stop it and kill the timer.
/* ------------------------------------------------------------------------------------ */
int StreamingAudio::Delete()
{
	if(ogg)
	{
		if(Ogg == NULL)
			return RGF_FAILURE;

		Ogg->Stop();
		delete Ogg;
		Ogg = NULL;
		return RGF_SUCCESS;
	}

	timeKillEvent(m_nTimerID);							// Kill timer

	if(mp3)
	{
		if(Mpeg3 == NULL)
			return RGF_FAILURE;

		Mpeg3->StopMp3();
		delete Mpeg3;
		Mpeg3 = NULL;
		return RGF_SUCCESS;
	}

	// Check for stream availability
	if(m_pStream == NULL)
		return RGF_FAILURE;								// No stream

	Stop();												// Stop playback

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	Rewind
//
//	Reposition stream playback to the start of the wave data
/* ------------------------------------------------------------------------------------ */
int StreamingAudio::Rewind()
{
	if(mp3)
	{
		if(Mpeg3 == NULL)
			return RGF_FAILURE;

		return RGF_SUCCESS;
	}

	if(ogg)
	{
		if(Ogg == NULL)
			return RGF_FAILURE;

		Ogg->Rewind();
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
//	IsPlaying
//
//	Returns TRUE if the stream is actively playing, FALSE otherwise
//	..If the stream is at EOF that qualifies as "not playing".
/* ------------------------------------------------------------------------------------ */
bool StreamingAudio::IsPlaying()
{
	if(mp3)
	{
		return false;
	}

	if(ogg)
	{
		if(!Ogg)
			return false;

		return Ogg->IsPlaying();
	}

	// Check for stream availability
	if(m_pStream == NULL)
		return false;									// No stream

	if((m_fEOF == true) || (m_fActive == false))
		return false;									// Not playing

	return true;										// Is playing
}

/* ------------------------------------------------------------------------------------ */
//	SetVolume
//
//	Set the playback volume of the audio stream.
/* ------------------------------------------------------------------------------------ */
int StreamingAudio::SetVolume(LONG nVolume)
{
	if(mp3)
	{
		if(Mpeg3 == NULL)
			return RGF_FAILURE;

		Mpeg3->SetVolume((nVolume+10000)/10);
		return RGF_SUCCESS;
	}

	LONG ScaledVolume;
	nVolume += 10000;;

	if(nVolume <= 1)
		ScaledVolume = -10000;
	else if(nVolume > 10000)
		ScaledVolume = 0;
	else
		//ScaledVolume = (LONG)(log10(nVolume)/4*10000 - 10000);
		ScaledVolume = (LONG)(log10((double)nVolume)*2500 - 10000);


	nVolume = ScaledVolume;

	if(ogg)
	{
		if(Ogg == NULL)
			return RGF_FAILURE;

		Ogg->SetVolume(nVolume);
		return RGF_SUCCESS;
	}

	//	Sanity check parameter
	if((m_pStream == NULL) || (nVolume > 0))
		return RGF_FAILURE;						// Bad parameter

	m_pStream->SetVolume(nVolume);				// Set it!

	return RGF_SUCCESS;
}

//	******************* PRIVATE MEMBER FUNCTIONS **********************

/* ------------------------------------------------------------------------------------ */
//	PumpWave
//
//	Fill the streaming audio buffer from the wave file, making sure that
//	..there's always something (up to and including silence) in the
//	..buffer for playback.
/* ------------------------------------------------------------------------------------ */
int StreamingAudio::PumpWave(int nSize)
{
	HRESULT hr;
	void *lpbuf1 = NULL, *lpbuf2 = NULL;
	DWORD dwsize1 = 0, dwsize2 = 0;
	UINT nBytesRead = 0;

	//	Ok, we need to set up our "silence" value and adjust it for
	//	..8bit samples if needed.

	int nSilence = 0x0;

	if(m_pWaveFormat->wBitsPerSample == 8)
		nSilence = 0x80;									// In case wave is 8-bit

	//	Ok, try to lock <n>K of the buffer.  If it fails, just bail this
	//	..function.
	hr = m_pStream->Lock(m_nOffset, nSize, &lpbuf1, &dwsize1, &lpbuf2, &dwsize2, 0);

	if(hr != DS_OK)
	{
		CCD->ReportError("[WARNING] StreamingAudio::PumpWave: Failed to lock", false);
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

	//	Fine, read data into the circular buffer directly from the
	//	..wave file if there's anything there.
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

	//	Ok, if we're at the end of file AND we're flagged to loop, rewind
	//	..to the beginning of the buffer so we start from the top next
	//	..time through.
	if(m_fEOF && m_bLoops)
		Rewind();						// Hope the sound designer looped the WAVE right!

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	GetMaxWriteSize
//
//	Get the maximum number of bytes we can write into the current buffer.
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
//	TimerFunction
//
//	This function takes care of periodically pumping audio into
//	..the playback buffer for streaming waves.  Based on the
//	..user information passed in (set up when the wave file was
//	..started streaming) it calls into the instance that is needed.
//	..Yeah, this is a little wacky, but it let's me stream the wave
//	..out without having to continually poll and stuff buffers.
/* ------------------------------------------------------------------------------------ */
void CALLBACK StreamingAudio::TimerFunction(UINT uID, UINT uMsg,
											DWORD dwUser, DWORD dw1, DWORD dw2)
{
	StreamingAudio *thePointer = (StreamingAudio*)dwUser;

	if(thePointer->mp3)
	{
		if(thePointer->Mpeg3 == NULL)
			return;

		if(thePointer->m_fActive && thePointer->m_bLoops)
			thePointer->Mpeg3->Refresh();

		return;
	}

	DWORD nSize = thePointer->GetMaxWriteSize();

	if(nSize != 0)
		thePointer->PumpWave(nSize);					// Pump stream

	return;
}

/* ------------------------------------------------------------------------------------ */
//	WaveOpen
//
//	Open up a wave file, validate it, and return a handle to the open
//	..file and a WAVEFORMATEX structure containing the wave info.
/* ------------------------------------------------------------------------------------ */
int StreamingAudio::WaveOpen(char *szFileName, HMMIO *pFileID,
							 WAVEFORMATEX **ppwfxInfo,	MMCKINFO *pckInRIFF)
{
	HMMIO hmmioIn = NULL;
	MMCKINFO ckIn;					// chunk info. for general use.
	PCMWAVEFORMAT pcmWaveFormat;	// Temp PCM structure to load in.
	WORD cbExtraAlloc = 0;			// Extra bytes for waveformatex
	int nError = 0;					// Return value.

	//	Set up for the job
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

	//	Expect the 'fmt' chunk to be at least as large as <PCMWAVEFORMAT>;
	//	if there are extra parameters at the end, we'll ignore them.
    if(ckIn.cksize < (long) sizeof(PCMWAVEFORMAT))
	{
		mmioClose(hmmioIn, 0);				// Clean up
		return -4;
	}

	// Read the 'fmt ' chunk into <pcmWaveFormat>
	if(mmioRead(hmmioIn, (HPSTR) &pcmWaveFormat, (long) sizeof(pcmWaveFormat)) != (long) sizeof(pcmWaveFormat))
	{
		mmioClose(hmmioIn, 0);				// Clean up
		return -5;
	}

	// Ok, allocate the waveformatex, but if its not pcm
	// format, read the next word, and thats how many extra
	// bytes to allocate.
	if(pcmWaveFormat.wf.wFormatTag == WAVE_FORMAT_PCM)
		cbExtraAlloc = 0;
	else
	{
		// Read in length of extra bytes.
		if (mmioRead(hmmioIn, (LPSTR) &cbExtraAlloc, (long) sizeof(cbExtraAlloc)) != (long) sizeof(cbExtraAlloc))
		{
			mmioClose(hmmioIn, 0);				// Clean up
			return -6;
		}
	}

	// Ok, now allocate that waveformatex structure.
	if((*ppwfxInfo = (WAVEFORMATEX*)malloc(sizeof(WAVEFORMATEX)+cbExtraAlloc)) == NULL)
	{
		mmioClose(hmmioIn, 0);				// Clean up
		return -7;
	}

	//	Copy the bytes from the pcm structure to the waveformatex structure
	memcpy(*ppwfxInfo, &pcmWaveFormat, sizeof(pcmWaveFormat));
	(*ppwfxInfo)->cbSize = cbExtraAlloc;

	// Now, read those extra bytes into the structure, if cbExtraAlloc != 0.
	if(cbExtraAlloc != 0)
	{
		if(mmioRead(hmmioIn, (LPSTR) (((BYTE*)&((*ppwfxInfo)->cbSize))+sizeof(cbExtraAlloc)),
			(long) (cbExtraAlloc)) != (long) (cbExtraAlloc))
		{
			mmioClose(hmmioIn, 0);				// Clean up
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
//	This will read wave data from the wave file.  Makre sure we're descended into
//	the data chunk, else this will fail bigtime!
//	hmmioIn         - Handle to mmio.
//	cbRead          - # of bytes to read.
//	pbDest          - Destination buffer to put bytes.
//	cbActualRead- # of bytes actually read.
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
