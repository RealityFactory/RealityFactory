/*
		StreamingAudio.h:	Streaming audio encapsulation class

		Authored		10/07/1998	Edward A. Averill, III
		Updated			10/07/1998	Edward A. Averill, III

		Ported over to Rabid Game Framework 11/08/1999

		(c) 1998 Edward A. Averill, III

* PURPOSE *

		This file contains the class definitions for audio streaming
functionality, based on the SoundServices functionality.

* MODIFICATION HISTORY *

	11/08/1999	  eaa3	Ported class to RGF
	10/07/1998		eaa3	First cut.
*/

//	* Local defines, constants

const int kBufferSize = 48000;

//	* Streaming audio class

class StreamingAudio
{
public:
  StreamingAudio(LPDIRECTSOUND lpDS);				// Default constructor
	~StreamingAudio();												// Default destructor
	int Create(char *szFileName);							// Add playback stream
	int Play(bool bLooping);									// Start streaming
	int Stop();																// Stop streaming
	int Pause();															// Pause streaming
	int Delete();															// Delete stream
	int Rewind();															// Rewind stream to beginning
	bool IsPlaying();													// Is stream playing?
	int SetVolume(LONG nVolume);							// Set volume on audio
private:
//	Private member functions
  DWORD GetMaxWriteSize();									// Get max. buffer write size
  int PumpWave(int nSize);									// Pump wave data to stream
  static void CALLBACK TimerFunction(UINT uID, UINT uMsg, DWORD dwUser,
	                 DWORD dw1, DWORD dw2);		// Static timer callback
  int WaveOpen(char *szFileName, HMMIO *pFileID,
			WAVEFORMATEX **ppwfxInfo,	MMCKINFO *pckInRIFF);
  int WaveRead(HMMIO hmmioIn, int nSizeToRead, BYTE *pDestination,
		MMCKINFO *pckIn, UINT *nBytesRead);
//	Private member variables
  LPDIRECTSOUND m_pDS;											// DirectSound object
  HMMIO m_hWaveFile;												// Handle to WAVE file
  int m_nDataPosition;											// Start of WAVE data in file
	int m_nCurrentPosition;										// Current read position
	int m_nDataSize;													// Wave data size, in bytes
	int m_nDataLeft;													// # of bytes of data left
	bool m_fActive;														// Playback active flag
	LPDIRECTSOUNDBUFFER m_pStream;						// Sound buffers
	int m_nTimerID;														// Timer ID, this instance
	WAVEFORMATEX *m_pWaveFormat;							// Wave format block
	MMCKINFO m_rRiffData;											// Wave RIFF block
	bool m_fEOF;															// At end of WAVE file
	int m_nOffset;
	bool m_bLoops;														// Loop this WAVE or not?
};

//	=========================================================================
