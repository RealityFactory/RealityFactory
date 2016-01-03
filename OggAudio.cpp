/************************************************************************************//**
 * @file OggAudio.cpp
 * @brief OggAudio class
 * @author Torok Edwin
 ****************************************************************************************/

#include <dsound.h>
#include <genesis.h>
#include "RGFStatus.h"
#include "RGFConstants.h"
#include "CLog.h"
#include "OggAudio.h"

/* ------------------------------------------------------------------------------------ */
// OggAudio
//
// Default constructor, takes a DirectSound pointer
/* ------------------------------------------------------------------------------------ */
OggAudio::OggAudio(LPDIRECTSOUND lpDS) :
	m_pDS(lpDS),		// Save DSound pointer
	m_Pumping(false),
	m_nDataPosition(0),	// Start of WAVE data in file
	m_fActive(false),	// Playback active flag
	m_pStream(NULL),	// Sound buffer
	m_nTimerID(0),		// Pumping timer ID
	m_fEOF(false),		// Not at end of file
	m_OggPcmPos(0),
	m_EndPos(0),
	m_PlayPos(0),
	m_bLoops(false),	// Assume no looping
	m_EndEvent(NULL),
	m_BaseFrequency(1)
{
}

/* ------------------------------------------------------------------------------------ */
// ~OggAudio
//
// Default destructor
/* ------------------------------------------------------------------------------------ */
OggAudio::~OggAudio()
{
	timeKillEvent(m_nTimerID);		// Stop the timer
	CloseHandle(m_EndEvent);

	ov_clear(&m_ovf);

	if(m_pStream != NULL)
	{
		m_pStream->Release();		// Drop the DSound buffer
	}
}

/* ------------------------------------------------------------------------------------ */
// Load
//
// Creates a new audio stream and sets up a timer for it.
/* ------------------------------------------------------------------------------------ */
int OggAudio::Load(const char *szFileName)
{
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
	if(ov_fopen(const_cast<char*>(szFileName), &m_ovf) < 0)
	{
		sxLog::GetSingletonPtr()->Warning("[OggAudio::Load]: ov_open failed");
		return RGF_FAILURE;
	}

	// Fetch DirectSound interface we want
	LPDIRECTSOUND pDSIF;
	int nError = m_pDS->QueryInterface(IID_IDirectSound, reinterpret_cast<LPVOID*>(&pDSIF));

	// Create a DSound buffer to stream into
	DSBUFFERDESC theDesc;
	vorbis_info *vi = ov_info(&m_ovf, -1);
	m_EndPos = static_cast<DWORD>((ov_pcm_total(&m_ovf, -1) * 2) % kBufferSize);
	m_EndEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	DSBPOSITIONNOTIFY notify;
	notify.dwOffset = m_EndPos;
	notify.hEventNotify = m_EndEvent;

	memset(&theDesc, 0, sizeof(DSBUFFERDESC));

	theDesc.dwSize = sizeof(DSBUFFERDESC);
	theDesc.dwBufferBytes = kBufferSize;
	theDesc.lpwfxFormat = new WAVEFORMATEX;
	theDesc.lpwfxFormat->wFormatTag = WAVE_FORMAT_PCM;
	theDesc.lpwfxFormat->nChannels = vi->channels;
	theDesc.lpwfxFormat->nSamplesPerSec = vi->rate;
	theDesc.lpwfxFormat->nBlockAlign = vi->channels * 2;
	theDesc.lpwfxFormat->wBitsPerSample = 16;
	theDesc.lpwfxFormat->cbSize = 0;
	theDesc.lpwfxFormat->nAvgBytesPerSec = theDesc.lpwfxFormat->nSamplesPerSec * theDesc.lpwfxFormat->nBlockAlign;
	theDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN | DSBCAPS_CTRLFREQUENCY | DSBCAPS_GETCURRENTPOSITION2 |
						DSBCAPS_CTRLPOSITIONNOTIFY;

	nError = pDSIF->CreateSoundBuffer(&theDesc, &m_pStream, NULL);
	m_pStream->GetFrequency(&m_BaseFrequency);
	delete (theDesc.lpwfxFormat);

	pDSIF->Release();						// Done w/ this.

	if(nError != 0)							// Error!  Sic out.
	{
		sxLog::GetSingletonPtr()->Warning("[OggAudio::Load]: Error while creating soundbuffer");
		ov_clear(&m_ovf);
		return RGF_FAILURE;
	}

	LPDIRECTSOUNDNOTIFY lpDsNotify;
	HRESULT hr;

	if(SUCCEEDED(hr = m_pStream->QueryInterface(IID_IDirectSoundNotify, (LPVOID*)&lpDsNotify)))
	{
		hr = lpDsNotify->SetNotificationPositions(1, &notify);
		lpDsNotify->Release();
	}

	m_OggPcmPos = 0;
	m_PlayPos = 0;

	m_fActive = true;						// Set as active

	PumpWave(kBufferSize);					// Initial buffer load

	// Ok, file loaded and ready to rock, start a timer for this stream.
	m_Pumping = false;

	MMRESULT nTimer = timeSetEvent(125, 5, &TimerFunction, reinterpret_cast<DWORD_PTR>(this),
									TIME_PERIODIC | TIME_CALLBACK_FUNCTION);

	if(nTimer == 0)
	{
		sxLog::GetSingletonPtr()->Warning("[OggAudio::Load]: Timer startup failure");
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
		sxLog::GetSingletonPtr()->Warning("[OggAudio::Play]: no stream");
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
		sxLog::GetSingletonPtr()->Warning("[OggAudio::Stop]: no stream");
		return RGF_FAILURE;							// No stream
	}

	m_fActive = false;

	m_pStream->Stop();								// Stop playback
	m_OggPcmPos = 0;
	m_PlayPos = 0;

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
		sxLog::GetSingletonPtr()->Warning("[OggAudio::Pause]: no stream");
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
		sxLog::GetSingletonPtr()->Warning("[OggAudio::Delete]: no stream");
		return RGF_FAILURE;							// No stream
	}

	Stop();											// Stop playback

	timeKillEvent(m_nTimerID);						// Kill timer

	ov_clear(&m_ovf);

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
		sxLog::GetSingletonPtr()->Warning("[OggAudio::Rewind]: no stream");
		return RGF_FAILURE;							// No stream
	}

	// Check to be sure it's active
	if(m_fActive == false)
	{
		sxLog::GetSingletonPtr()->Warning("[OggAudio::Rewind]: attempting to rewind an inactive stream");
		return RGF_FAILURE;							// Not an active stream
	}

	// Ok, position to the start of the WAVE DATA
	ov_pcm_seek(&m_ovf, 0);

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

	return true;									// Is playing
}

/* ------------------------------------------------------------------------------------ */
// SetVolume
//
// Set the playback volume of the audio stream.
/* ------------------------------------------------------------------------------------ */
int OggAudio::SetVolume(long volume)
{
	// Sanity check parameter
	if(m_pStream == NULL)
	{
		sxLog::GetSingletonPtr()->Warning("[OggAudio::SetVolume]: no stream");
		return RGF_FAILURE;
	}

	if(volume > DSBVOLUME_MAX || volume < DSBVOLUME_MIN)
		return RGF_FAILURE;

	m_pStream->SetVolume(volume);					// Set it!

	return RGF_SUCCESS;
}


int OggAudio::SetPan(long pan)
{
	// Sanity check parameter
	if(m_pStream == NULL)
	{
		sxLog::GetSingletonPtr()->Warning("[OggAudio::SetPan]: no stream");
		return RGF_FAILURE;
	}

	if(pan > DSBPAN_RIGHT || pan < DSBPAN_LEFT)
		return RGF_FAILURE;

	m_pStream->SetPan(pan);							// Set it!

	return RGF_SUCCESS;
}


int OggAudio::SetFrequency(unsigned long frequency)
{
	// Sanity check parameter
	if(m_pStream == NULL)
	{
		sxLog::GetSingletonPtr()->Warning("[OggAudio::SetFrequency]: no stream");
		return RGF_FAILURE;							// Bad parameter
	}

	m_pStream->SetFrequency(frequency);				// Set it!

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

	// Ok, we need to set up our "silence" value and adjust it for
	// ..8bit samples if needed.
	int nSilence = 0x0;

	// Ok, try to lock <n>K of the buffer.  If it fails, just bail this function.
	hr = m_pStream->Lock(m_OggPcmPos, nSize, &lpbuf1, &dwsize1, &lpbuf2, &dwsize2, 0);

	// If the buffer was lost, restore and retry lock.
	if(hr == DSERR_BUFFERLOST)
	{
		m_pStream->Restore();
		hr = m_pStream->Lock(m_OggPcmPos, nSize, &lpbuf1, &dwsize1, &lpbuf2, &dwsize2, 0);
	}

	if(hr != DS_OK)
	{
		sxLog::GetSingletonPtr()->Warning("[OggAudio:Pumpwave]: error locking stream");
		return RGF_FAILURE;								// bail out
	}

	if(lpbuf1 != NULL)
		memset(lpbuf1, nSilence, dwsize1);				// Clear to silence

	if(lpbuf2 != NULL)
		memset(lpbuf2, nSilence, dwsize2);				// Here, also

	m_OggPcmPos = (m_OggPcmPos + dwsize1 + dwsize2) % kBufferSize;

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
	long Oggread;
	char* lpb = static_cast<char*>(lpbuf1);
	DWORD nBytesRead = 0;
	int cs;

	while(nBytesRead < dwsize1 && !m_fEOF)
	{
		Oggread = ov_read(&m_ovf, lpb, dwsize1 - nBytesRead, 0, 2, 1, &cs);

		if(Oggread > 0)
		{
			nBytesRead += Oggread;
			lpb += Oggread;
		}

		if(Oggread == 0)
			m_fEOF = true;
		else
			m_fEOF = false;
	}

	if((lpbuf2 != NULL) && (!m_fEOF))
	{
		nBytesRead = 0;
		lpb = static_cast<char*>(lpbuf2);

		while(nBytesRead < dwsize2 && !m_fEOF)
		{
			Oggread = ov_read(&m_ovf, lpb, dwsize2-nBytesRead, 0, 2, 1, &cs);

			if(Oggread > 0)
			{
				nBytesRead += Oggread;
				lpb += Oggread;
			}

			if(Oggread == 0)
				m_fEOF = true;
			else
				m_fEOF = false;
		}
	}

	// Unlock buffer, we're done with it for now.
	m_pStream->Unlock(lpbuf1, dwsize1, lpbuf2, dwsize2);

	// Ok, if we're at the end of file AND we're flagged to loop, rewind
	// ..to the beginning of the buffer so we start from the top next
	// ..time through.
	if(m_fEOF)
	{
		if(m_bLoops)
		{
			Rewind();					// Hope the sound designer looped the OGG right!
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
DWORD OggAudio::GetMaxWriteSize()
{
	DWORD dwWriteCursor, dwPlayCursor, dwMaxSize;

	// Get current play position
	if(m_pStream->GetCurrentPosition(&dwPlayCursor, &dwWriteCursor) == DS_OK)
	{
		if(m_OggPcmPos <= dwPlayCursor)
		{
			// Our write position trails play cursor
			dwMaxSize = dwPlayCursor - m_OggPcmPos;
		}
		else // (m_cbBufOffset > dwPlayCursor)
		{
			// Play cursor has wrapped
			dwMaxSize = kBufferSize - m_OggPcmPos + dwPlayCursor;
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
void CALLBACK OggAudio::TimerFunction(UINT /*uID*/, UINT /*uMsg*/,
									  DWORD dwUser, DWORD /*dw1*/, DWORD /*dw2*/)
{
	OggAudio *thePointer = reinterpret_cast<OggAudio*>(dwUser);

	if(!thePointer->m_Pumping)
	{
		thePointer->m_Pumping = true;
		DWORD nSize = thePointer->GetMaxWriteSize();

		if(nSize != 0)
			thePointer->PumpWave(nSize);					// Pump stream

		thePointer->m_Pumping = false;
	}
}


/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
size_t VFS_Read  (void *ptr, size_t size, size_t nmemb, void *datasource);
int    VFS_Seek  (void *datasource, ogg_int64_t offset, int whence);
int    VFS_Close (void *datasource);
long   VFS_Tell  (void *datasource);

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
size_t VFS_Read(void *ptr, size_t size, size_t nmemb, void *datasource)
{
	geVFile *file = static_cast<geVFile*>(datasource);
	size_t count = static_cast<int>(size * nmemb);

	geVFile_Read(file, ptr, static_cast<int>(count));

	return count;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
int VFS_Seek(void *datasource, ogg_int64_t offset, int whence)
{
	geVFile *file = static_cast<geVFile*>(datasource);
	geVFile_Whence vfs_whence;

	switch(whence)
	{
	case SEEK_SET:
		vfs_whence = GE_VFILE_SEEKSET;
		break;
	case SEEK_CUR:
		vfs_whence = GE_VFILE_SEEKCUR;
		break;
	default:
		vfs_whence = GE_VFILE_SEEKEND;
		break;
	}

	if(geVFile_Seek(file, static_cast<int>(offset), vfs_whence) == GE_TRUE)
		return 0;

	return -1;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
int VFS_Close (void *datasource)
{
	geVFile *file = static_cast<geVFile*>(datasource);

	if(geVFile_Close(file) == GE_TRUE)
		return 0;

	return EOF;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
long VFS_Tell(void *datasource)
{
	long position;
	geVFile *file = static_cast<geVFile*>(datasource);
	geVFile_Tell(file, &position);
	return position;
}

/* ----------------------------------- END OF FILE ------------------------------------ */
