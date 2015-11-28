#include "RabidFramework.h"
/************************************************************************************//**
 * @file CAVIPlayer.h
 * @brief Play AVI file into Genesis3D engine
 *
 * This file contains the class declaration for the CAVIPlayer class.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_CAVIPLAYER_H__
#define __RGF_CAVIPLAYER_H__

//	Ok, we define the max # of audio and video streams we
//	..can handle in a single AVI file.  More than this, and
//	..you have a problem.

#define	MAX_AUDIO_STREAMS		8
#define MAX_VIDEO_STREAMS		8

class CAVIPlayer : public CRGFComponent
{
public:
	CAVIPlayer();
	~CAVIPlayer();

	// Version 053
	int Play(char *szFile, int XPos, int YPos, bool Center);	// Play an AVI
	int Open(char *szFile);										// Open an AVI
	int DisplayFrameAt(int XPos, int YPos, DWORD dwTime);		// Display one frame from a specific time
	int DisplayFrame(int XPos, int YPos, int FrameID);			// Display one specific frame
	int Close();												// Close the open AVI file

	int GetAudioStreamCount();									// Get the # of audio streams in the AVI
	int GetVideoStreamCount();									// Get the # of video streams in the AVI

	int DisplayFrameTexture(int nFrame, char *szTextureName);			// Display AVI frame as texture
	int DisplayNextFrameTexture(char *szTextureName, bool bFirstFrame);	// Play an AVI as a texture map

private:
	void Init();								// Initialize buffers
	void Release();								// Release buffers

	void FindStreams();

	bool DetermineAudioFormats();
	bool DetermineVideoFormats();

	int ExtractAudioStream(int nStreamNum, int nBytes, LPBYTE pBuffer);

	bool StartVideoRetrieve(int nStreamNum);
	bool EndVideoRetrieve(int nStreamNum);

	void GetVideoFrameAtTime(int nStreamNum, LONG lTimeInMilliSec,
						LPBITMAPINFOHEADER *ppbi);
	void GetVideoFrame(int nStreamNum, LONG lFrame,
						LPBITMAPINFOHEADER *ppbi);

	LPWAVEFORMATEX GetAudioFormat(int nStreamNum);
	LPBITMAPINFO GetVideoFormat(int nStreamNum);

	int CreateStreamingAudioBuffer(int nAudioStreamID);
	int DestroyStreamingAudioBuffer();
	void PumpBuffer(int nAudioStreamID, bool ForceLoad);
	int GetMaxWriteSize();

private:
	bool m_bVideoPrimed;						// Open video is in retrieval
	PAVIFILE m_pAviFile;						// Open AVI file

	// Audio and video stream pointer arrays
	PAVISTREAM m_pAudioStreams[MAX_AUDIO_STREAMS];
	PAVISTREAM m_pVideoStreams[MAX_VIDEO_STREAMS];

	int m_nNumAudioStreams;						// # of of audio streams
	int m_nNumVideoStreams;						// # of video streams (doh)

	// Array of stream audio formats
	LPWAVEFORMATEX m_pAudioFormats[MAX_AUDIO_STREAMS];
	// Audio stream data buffers
	LPBYTE m_pAudioData[MAX_AUDIO_STREAMS];
	// Array of stream video formats
	LPBITMAPINFO m_pVideoFormats[MAX_VIDEO_STREAMS];
	// Video stream data buffers
	PGETFRAME m_pVideoPGF[MAX_VIDEO_STREAMS];
	// Array of end times for video streams
	LONG m_lVideoEndTime[MAX_VIDEO_STREAMS];
	// AUDIO streaming member variables
	LPDIRECTSOUND m_pDS;						// DirectSound object
	LPDIRECTSOUNDBUFFER m_pStream;				// Sound buffers
	int m_nOffset;
	bool m_AudioEOF;
	int m_nLastFramePlayed;
	int m_nBufSize;								// Audio buffer size
	BITMAPINFOHEADER *m_LastFrameBitmap;
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
