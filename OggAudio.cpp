/************************************************************************************//**
 * @file OggAudio.cpp
 * @brief OggAudio class
 * @author Torok Edwin
 ****************************************************************************************/

#include <dsound.h>
#include <genesis.h>
#include "RGFStatus.h"
#include "RGFConstants.h"
#include "RabidFramework.h"
#include "OggAudio.h"

/* ------------------------------------------------------------------------------------ */
// OggAudio
//
// Default constructor, takes a DirectSound pointer
/* ------------------------------------------------------------------------------------ */
OggAudio::OggAudio(LPDIRECTSOUND lpDS)
{
	m_nDataPosition = 0;			// Start of WAVE data in file
	m_fActive		= false;		// Playback active flag
	m_pStream		= NULL;			// Sound buffer
	m_nTimerID		= 0;			// Pumping timer ID
	vf				= NULL;
	OggPcmPos		= 0;
	m_pDS			= lpDS;			// Save DSound pointer
	m_fEOF			= false;		// Not at end of file
	m_bLoops		= false;		// Assume no looping
	return;
}

/* ------------------------------------------------------------------------------------ */
// ~OggAudio
//
// Default destructor
/* ------------------------------------------------------------------------------------ */
OggAudio::~OggAudio()
{
	m_fActive = false;				// Inactive now

	timeKillEvent(m_nTimerID);		// Stop the timer

	if(vf != NULL)
	{
		ov_clear(&ovf);
		fclose(vf);
		vf = NULL;
	}

	if(m_pStream != NULL)
	{
		m_pStream->Release();		// Drop the DSound buffer
		m_pStream = NULL;
	}

	return;
}

/* ------------------------------------------------------------------------------------ */
// Load
//
// Creates a new audio stream and sets up a timer for it.
/* ------------------------------------------------------------------------------------ */
int OggAudio::Load(const char *szFileName)
{
	int nError = 0;

	// Sanity check parameters
	if(szFileName == NULL)
	{
		return RGF_FAILURE;			// Wrong.
	}

	// Check for stream availability
	if(m_pStream != NULL)
	{
		return RGF_FAILURE;		// Already loaded
	}

	// Open up the OGG file.
	vf = fopen(szFileName, "rb");

	if(vf)
	{
		if(ov_open(vf, &ovf, NULL, 0) < 0)
		{
			CCD->ReportError("[WARNING] [OggAudio::Load]: ov_open failed", false);
			return RGF_FAILURE;
		}
	}
	else
	{
		CCD->ReportError("[WARNING] [OggAudio::Load]:unable to find file", false);
		return RGF_FAILURE;
	}

	// Fetch DirectSound interface we want
	LPDIRECTSOUND pDSIF;
	nError = m_pDS->QueryInterface(IID_IDirectSound, reinterpret_cast<LPVOID*>(&pDSIF));

	// Create a DSound buffer to stream into
	DSBUFFERDESC theDesc;
	vorbis_info *vi = ov_info(&ovf,-1);

	memset(&theDesc, 0, sizeof(DSBUFFERDESC));

	theDesc.dwSize = sizeof(DSBUFFERDESC);
	theDesc.dwBufferBytes = kBufferSize;
	theDesc.lpwfxFormat =(LPWAVEFORMATEX) malloc(sizeof(WAVEFORMATEX));
	theDesc.lpwfxFormat->wFormatTag = WAVE_FORMAT_PCM;
	theDesc.lpwfxFormat->nChannels = vi->channels;
	theDesc.lpwfxFormat->nSamplesPerSec = vi->rate;
	theDesc.lpwfxFormat->nBlockAlign = vi->channels * 2;
	theDesc.lpwfxFormat->wBitsPerSample = 16;
	theDesc.lpwfxFormat->cbSize = 0;
	theDesc.lpwfxFormat->nAvgBytesPerSec = theDesc.lpwfxFormat->nSamplesPerSec * theDesc.lpwfxFormat->nBlockAlign;

	nError = pDSIF->CreateSoundBuffer(&theDesc, &m_pStream, NULL);

	free(theDesc.lpwfxFormat);

	pDSIF->Release();						// Done w/ this.

	if(nError != 0)							// Error!  Sic out.
	{
		CCD->ReportError("[WARNING] [OggAudio::Load]: Error while creating soundbuffer", false);
		ov_clear(&ovf);
		fclose(vf);
		vf = NULL;
		return RGF_FAILURE;
	}

	OggPcmPos = 0;

	m_fActive = true;						// Set as active

	PumpWave(kBufferSize);					// Initial buffer load

	// Ok, file loaded and ready to rock, start a timer for this stream.
	Pumping = false;
	MMRESULT nTimer = timeSetEvent(125, 5, &TimerFunction, (DWORD)this,
									TIME_PERIODIC | TIME_CALLBACK_FUNCTION);

	if(nTimer == 0)
	{
		CCD->ReportError("[WARNING] [OggAudio::Load]: Timer startup failure", false);
		return RGF_FAILURE;					// Timer startup failed.
	}

	m_nTimerID = nTimer;					// Save timer ID
	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// Play
//
// Start this stream buffer playing, with or without looping.
/* ------------------------------------------------------------------------------------ */
int OggAudio::Play(bool bLooping)
{
	// Check for stream availability
	if(m_pStream == NULL)
	{
		CCD->ReportError("[WARNING] [OggAudio::Play]: no stream", false);
		return RGF_FAILURE;						// no stream
	}

	m_fActive = true;
	m_bLoops = bLooping;

	m_pStream->Play(0, 0, DSBPLAY_LOOPING);

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// Stop
//
// Stop this stream from playing back.
/* ------------------------------------------------------------------------------------ */
int OggAudio::Stop()
{
	// Check for stream availability
	if(m_pStream == NULL)
	{
		CCD->ReportError("[WARNING] [OggAudio::Stop]: no stream", false);
		return RGF_FAILURE;							// No stream
	}

	m_fActive = false;

	m_pStream->Stop();								// Stop playback
	OggPcmPos = 0;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// Pause
//
// Pause/unpause this stream's playback.
/* ------------------------------------------------------------------------------------ */
int OggAudio::Pause()
{
	// Check for stream availability
	if(m_pStream == NULL)
	{
		CCD->ReportError("[WARNING] [OggAudio::Pause]: no stream", false);
		return RGF_FAILURE;							// No stream
	}

	if(m_fActive == false)
		m_fActive = true;
	else
		m_fActive = false;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// Delete
//
// Delete this stream - stop it and kill the timer, close file;
/* ------------------------------------------------------------------------------------ */
int OggAudio::Delete()
{
	// Check for stream availability
	if(m_pStream == NULL)
	{
		CCD->ReportError("[WARNING] [OggAudio::Delete]: no stream", false);
		return RGF_FAILURE;							// No stream
	}

	Stop();											// Stop playback

	timeKillEvent(m_nTimerID);						// Kill timer

	ov_clear(&ovf);
	fclose(vf);

	m_pStream->Release();
	m_pStream = NULL;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// Rewind
//
// Reposition stream playback to the start of the wave data
/* ------------------------------------------------------------------------------------ */
int OggAudio::Rewind()
{
	// Check for stream availability
	if(m_pStream == NULL)
	{
		CCD->ReportError("[WARNING] [OggAudio::Rewind]: no stream", false);
		return RGF_FAILURE;							// No stream
	}

	// Check to be sure it's active
	if(m_fActive == false)
	{
		CCD->ReportError("[WARNING] [OggAudio::Rewind]: attempting to rewind an inactive stream", false);
		return RGF_FAILURE;							// Not an active stream
	}

	// Ok, position to the start of the WAVE DATA
	ov_pcm_seek(&ovf, 0);
	m_fEOF = false;									// Not EOF any longer

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// IsPlaying
//
// Returns TRUE if the stream is actively playing, FALSE otherwise
/* ------------------------------------------------------------------------------------ */
bool OggAudio::IsPlaying()
{
	// Check for stream availability
	if(m_pStream == NULL)
	{
		return false;								// No stream
	}

	if((m_fEOF == true) || (m_fActive == false))
	{
		return false;										// Not playing
	}

	return true;									// Is playing
}

/* ------------------------------------------------------------------------------------ */
// SetVolume
//
// Set the playback volume of the audio stream.
/* ------------------------------------------------------------------------------------ */
int OggAudio::SetVolume(int nVolume)
{
	// Sanity check parameter
	if(m_pStream == NULL)
	{
		CCD->ReportError("[WARNING] [OggAudio::SetVolume]: no stream", false);
		return RGF_FAILURE;							// Bad parameter
	}

	m_pStream->SetVolume(nVolume);				// Set it!
	return RGF_SUCCESS;
}

// ******************* PRIVATE MEMBER FUNCTIONS **********************

/* ------------------------------------------------------------------------------------ */
// PumpWave
//
// Fill the streaming audio buffer from the ogg file, making sure that
// ..there's always something (up to and including silence) in the
// ..buffer for playback.
/* ------------------------------------------------------------------------------------ */
int OggAudio::PumpWave(int nSize)
{
	HRESULT hr;
	void *lpbuf1 = NULL, *lpbuf2 = NULL;
	DWORD dwsize1 = 0, dwsize2 = 0;
	UINT nBytesRead = 0;

	// Ok, we need to set up our "silence" value and adjust it for
	// ..8bit samples if needed.
	int nSilence = 0x0;

	// Ok, try to lock <n>K of the buffer.  If it fails, just bail this function.
	hr = m_pStream->Lock(OggPcmPos, nSize, &lpbuf1, &dwsize1, &lpbuf2, &dwsize2, 0);

	if(hr != DS_OK)
	{
		OutputDebugString("[OggAudio:Pumpwave]:error locking stream");
		return RGF_FAILURE;								// bail out
	}

	if(lpbuf1 != NULL)
		memset(lpbuf1, nSilence, dwsize1);				// Clear to silence

	if(lpbuf2 != NULL)
		memset(lpbuf2, nSilence, dwsize2);				// Here, also

	OggPcmPos = (OggPcmPos + dwsize1 + dwsize2) % kBufferSize;

	if(m_fEOF == true)
	{
		m_pStream->Unlock(lpbuf1, dwsize1, lpbuf2, dwsize2);
		return RGF_FAILURE;								// End of file, pump silence
	}

	if(m_fActive == false)
	{
		m_pStream->Unlock(lpbuf1, dwsize1, lpbuf2, dwsize2);
		return RGF_SUCCESS;								// Inactive, pump silence
	}

	// Fine, read data into the circular buffer directly from the
	// ..wave file if there's anything there.
	int Oggread;
	char* lpb = (char*)lpbuf1;
	nBytesRead = 0;
	int cs;

	while(nBytesRead<dwsize1 && !m_fEOF)
	{
		Oggread = ov_read(&ovf, (char*)lpb, dwsize1-nBytesRead, 0, 2, 1, &cs);

		if(Oggread > 0)
		{
			nBytesRead += Oggread;
			lpb += Oggread;
		}

		m_fEOF = feof(vf);
	}

	if((lpbuf2 != NULL) && (!m_fEOF))
	{
		nBytesRead = 0;
		lpb = static_cast<char*>(lpbuf2);

		while(nBytesRead < dwsize2 && !m_fEOF)
		{
			Oggread = ov_read(&ovf, (char*)lpb, dwsize2-nBytesRead, 0, 2, 1, &cs);

			if(Oggread > 0)
			{
				nBytesRead += Oggread;
				lpb += Oggread;
			}

			m_fEOF = feof(vf);
		}
	}

	// Unlock buffer, we're done with it for now.
	m_pStream->Unlock(lpbuf1, dwsize1, lpbuf2, dwsize2);

	// Ok, if we're at the end of file AND we're flagged to loop, rewind
	// ..to the beginning of the buffer so we start from the top next
	// ..time through.
	if(m_fEOF && m_bLoops)
		Rewind();					// Hope the sound designer looped the WAVE right!

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// GetMaxWriteSize
//
// Get the maximum number of bytes we can write into the current buffer.
/* ------------------------------------------------------------------------------------ */
DWORD OggAudio::GetMaxWriteSize()
{
	DWORD dwWriteCursor, dwPlayCursor, dwMaxSize;

	// Get current play position
	if(m_pStream->GetCurrentPosition(&dwPlayCursor, &dwWriteCursor) == DS_OK)
	{
		if((UINT)OggPcmPos <= dwPlayCursor)
		{
			// Our write position trails play cursor
			dwMaxSize = dwPlayCursor - OggPcmPos;
		}
		else // (m_cbBufOffset > dwPlayCursor)
		{
			// Play cursor has wrapped
			dwMaxSize = kBufferSize - OggPcmPos + dwPlayCursor;
		}
	}
	else
		dwMaxSize = 0;

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
void CALLBACK OggAudio::TimerFunction(UINT /*uID*/, UINT /*uMsg*/,
									  DWORD dwUser, DWORD /*dw1*/, DWORD /*dw2*/)
{
	OggAudio *thePointer = reinterpret_cast<OggAudio*>(dwUser);

	if(!thePointer->Pumping)
	{
		thePointer->Pumping = true;
		DWORD nSize = thePointer->GetMaxWriteSize();

		if(nSize != 0)
			thePointer->PumpWave(nSize);					// Pump stream

		thePointer->Pumping = false;
	}

	return;
}


/* ----------------------------------- END OF FILE ------------------------------------ */
