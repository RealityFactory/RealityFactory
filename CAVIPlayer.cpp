/************************************************************************************//**
 * @file CAVIPlayer.cpp
 * @brief Play AVI file into Genesis3D engine
 *
 * This file contains the class implementation for the CAVIPlayer class.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

//#define MONDO_DEBUG
#include "RabidFramework.h"

/* ------------------------------------------------------------------------------------ */
// CAVIPlayer
//
// Constructor
/* ------------------------------------------------------------------------------------ */
CAVIPlayer::CAVIPlayer() :
	m_nOffset(0),
	m_AudioEOF(false),
	m_nLastFramePlayed(0),
	m_nBufSize(0),
	m_LastFrameBitmap(NULL)
{
	Init();
}


/* ------------------------------------------------------------------------------------ */
// ~CAVIPlayer
//
// Destructor
/* ------------------------------------------------------------------------------------ */
CAVIPlayer::~CAVIPlayer()
{
	Release();
}


/* ------------------------------------------------------------------------------------ */
// Play
//
// Play an AVI file at a specific position in the game window.
// ..Note that this always plays VIDEO STREAM 0 and AUDIO STREAM
// ..0, no attempt is made to handle multiple streams in the
// ..same file.
/* ------------------------------------------------------------------------------------ */
int CAVIPlayer::Play(const char *szFile, int XPos, int YPos, bool Center)
{
	// Open the file up.
	if(Open(szFile) != RGF_SUCCESS)
		return RGF_FAILURE;						// AVI didn't open.

	if(!StartVideoRetrieve(0))					// Start bringing it in
	{
		return RGF_FAILURE;
	}

	// Fine, the file opened, get the bitmap info for it.

	LPBITMAPINFO pVideoFormat = GetVideoFormat(0);	// Video format
	if(!pVideoFormat)
	{
		return RGF_FAILURE;
	}

	// Ok, while we have frames, let's load 'em in, copy them into
	// ..the engine, and blast up the bitmap.  Note that NOTHING
	// ..ELSE CAN WILL BE HAPPENING WHILE THE VIDEO IS PLAYING.
	int nWidth = pVideoFormat->bmiHeader.biWidth;
	int nHeight = pVideoFormat->bmiHeader.biHeight;

	if(Center)
	{
		XPos = (CCD->Engine()->Width() - nWidth) / 2;
		YPos = (CCD->Engine()->Height() - nHeight) / 2;
	}

	gePixelFormat nFormat = GE_PIXELFORMAT_NO_DATA;
	int nAlignValue = 0;

	// Here's how it shakes down: 16bit color is always RGB/555,
	// ..24bit color is always BGR, and 32bit color is always
	// ..BGR with the high byte unused (worthless).  I'm NOT supporting
	// ..8bit color from video files, if you want it, put it in.
	switch(pVideoFormat->bmiHeader.biBitCount)
	{
	case 8:
		CCD->ReportError("[WARNING] 8-bit video unsupported", false);
		EndVideoRetrieve(0);
		Close();
		return RGF_FAILURE;
	case 16:
		nFormat = GE_PIXELFORMAT_16BIT_555_RGB;
		nAlignValue = (nWidth * 2) + ((nWidth * 2) % 4);
		break;
	case 24:
		nFormat = GE_PIXELFORMAT_24BIT_BGR;
		nAlignValue = (nWidth * 3) + ((nWidth * 3) % 4);
		break;
	case 32:
		nFormat = GE_PIXELFORMAT_32BIT_XBGR;
		nAlignValue = (nWidth * 4);
		break;
	}

	geBitmap *theBmp = NULL;
	geBitmap_Info Info;

	theBmp = geBitmap_Create(nWidth, nHeight, 1, nFormat);
	if(!theBmp)
	{
		EndVideoRetrieve(0);
		Close();
		return RGF_FAILURE;
	}
	if(geBitmap_SetPreferredFormat(theBmp, nFormat) == GE_FALSE)
	{
	}
	if(geEngine_AddBitmap(CCD->Engine()->Engine(), theBmp) == GE_FALSE)
	{
		EndVideoRetrieve(0);
		geBitmap_Destroy(&theBmp);
		Close();
		return RGF_FAILURE;
	}
	geBitmap_GetInfo(theBmp, &Info, NULL);
	if(geBitmap_ClearMips(theBmp) == GE_FALSE)
	{
		EndVideoRetrieve(0);
		geEngine_RemoveBitmap(CCD->Engine()->Engine(), theBmp);
		geBitmap_Destroy(&theBmp);
		Close();
		return RGF_FAILURE;
	}

	// **NOTE**	11/23/1999
	// All this is to force Genesis3D to get the bitmap loaded into
	// ..where it needs to be so our first frame doesn't take a
	// ..bazillion milliseconds.  For some reason, the first
	// ..BeginFrame/DrawBitmap/EndFrame sequence takes nearly
	// ..720milliseconds (at least on my machine), causing the
	// ..first frame of the video to push all remaining frames
	// ..out of synch with the audio.  So, we lock and unlock
	// ..and do the whole fake-render-cycle so that we can avoid
	// ..having whatever setup is happening down inside Genesis3D
	// ..screw us up whilst playing video.

	geBitmap *LockedBMP = NULL;

	geBitmap_LockForWriteFormat(theBmp, &LockedBMP, 0, 0, nFormat);

	// The following block of code comes to us courtesy of
	// ..Ralph Deane, who discovered that the lock sometimes
	// ..failed and how to fix it!

	if(LockedBMP == NULL)
	{
		geBitmap_SetFormat(theBmp, nFormat, GE_TRUE, 0, NULL);
		geBitmap_LockForWriteFormat(theBmp, &LockedBMP, 0, 0, nFormat);
		if(LockedBMP == NULL)
		{
			EndVideoRetrieve(0);
			geEngine_RemoveBitmap(CCD->Engine()->Engine(), theBmp);
			geBitmap_Destroy(&theBmp);
			Close();
			return RGF_FAILURE;
		}
	}

	LPBITMAPINFOHEADER pBmp;				// Will hold decompressed frame
	unsigned char *wptr ,*pptr;

	GetVideoFrameAtTime(0, 0, &pBmp);
	wptr = static_cast<unsigned char*>(geBitmap_GetBits(LockedBMP));
	pptr = reinterpret_cast<unsigned char*>(pBmp) + pBmp->biSize;

	// The following weirdness is required because the DIB
	// ..coming in from the AVI file is INVERTED, so we have
	// ..to copy it to the target bitmap from the bottom
	// ..up to make it display correctly.  Thanks to Ernest
	// ..Kleynhans for catching this little mistake 11/22/99.

	pptr += (nWidth * ((nHeight - 1) * (pBmp->biBitCount >> 3)));

	// Ok, we've got a _somewhat_ optimized version of the bitmap
	// ..copy, it's far faster than memcpy() but it's not as
	// ..optimized as it could be.  However, for now, it does seem
	// ..to be Good Enough.

	int nTemp, nTemp2;
	int y;

	switch(nFormat)
	{
	case GE_PIXELFORMAT_16BIT_555_RGB:
		nTemp2 = Info.Stride * 2;

		for(y=0; y<Info.Height; ++y)
		{
			__asm
			{
				mov esi, pptr
				mov edi, wptr
				mov ecx, nWidth
				rep movsw
			}

			wptr += nTemp2;
			pptr -= nAlignValue;
		}
		break;
	case GE_PIXELFORMAT_24BIT_BGR:
		nTemp = nWidth * 3;
		nTemp2 = Info.Stride * 3;

		for(y=0; y<Info.Height; ++y)
		{
			__asm
			{
				mov esi, pptr
				mov edi, wptr
				mov ecx, nTemp
				rep movs
			}

			wptr += nTemp2;
			pptr -= nAlignValue;
		}
		break;
	case GE_PIXELFORMAT_32BIT_XBGR:
		nTemp2 = Info.Stride * 4;

		for(y=0; y<Info.Height; ++y)
		{
			__asm
			{
				mov esi, pptr
				mov edi, wptr
				mov ecx, nWidth
				rep movsd
			}

			wptr += nTemp2;
			pptr -= nAlignValue;
		}
		break;
	}

	geBitmap_UnLock(LockedBMP);

	if(CCD->GetHasFocus())
	{
		geEngine_BeginFrame(CCD->Engine()->Engine(), CCD->CameraManager()->Camera(), GE_TRUE);
		geEngine_DrawBitmap(CCD->Engine()->Engine(), theBmp, NULL, XPos, YPos);
		geEngine_EndFrame(CCD->Engine()->Engine());
	}

	// End of the force-the-bitmap-to-be-ready code.  Blech.

	bool bAudioStreamPlaying = false;

	if(GetAudioStreamCount() != 0)
	{
		if(CreateStreamingAudioBuffer(0) == RGF_SUCCESS)	// We're gonna play audio
			bAudioStreamPlaying = true;
	}

	DWORD ElapsedTime, OldTime, FrameTime = 0;
	int nFrameTotal = 0;

	OldTime = CCD->FreeRunningCounter();				// Prime the time.

	FrameTime = 0;

	// **IMPORTANT NOTE** It APPEARS that, if I try to blit to the
	// ..screen with bitmaps TOO FAST, the sucker locks up.  Regardless,
	// ..since I'm not running any video faster than 30fps, I've set
	// ..the rendering loop to not try to get new frames from the AVI
	// ..file any faster than that.  The system will RENDER full-speed,
	// ..but the AVI file API won't be hit any more often than once
	// ..every 30msec.

	for(;;)
	{
		MSG msg;

		// If Winblows has something to say, take it in and pass it on in the
		// ..off-chance someone cares.
		while(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			GetMessage(&msg, NULL, 0, 0 );
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if(!CCD->GetHasFocus())
			continue;

		ElapsedTime = CCD->FreeRunningCounter() - OldTime;

		if(ElapsedTime > 30)
		{
			OldTime = CCD->FreeRunningCounter();					// Prepare for next loop
			++nFrameTotal;

			if(bAudioStreamPlaying)
				PumpBuffer(0, false);				// Pump audio if we have any

			pBmp = NULL;
			// A note: sometimes Things Happen in Windows to cause a
			// .."hiccup", that is, the whole system freezes EXCEPT
			// ..for the timer.  If that happens, the following IF
			// ..will help us recover such that the audio and video
			// ..don't drift too far out of synch.

			//if(ElapsedTime > 60)
			//ElapsedTime = 30;			// Force video to recover

			FrameTime += ElapsedTime;			// Compute new frame time
			GetVideoFrameAtTime(0, FrameTime, &pBmp);

			if(!pBmp)
				break;											// Video over, exit loop

			// Lock the created bitmap for write!
			geBitmap_LockForWriteFormat(theBmp, &LockedBMP, 0, 0, nFormat);

			// The lock sometimes fails, here's how to fix it!
			if(LockedBMP == NULL)
			{
				geBitmap_SetFormat(theBmp, nFormat, GE_TRUE, 0, NULL);
				geBitmap_LockForWriteFormat(theBmp,&LockedBMP,0,0, nFormat);

				if(LockedBMP == NULL)
				{
					if(bAudioStreamPlaying)
						DestroyStreamingAudioBuffer();

					bAudioStreamPlaying = false;
					EndVideoRetrieve(0);
					Close();

					return RGF_FAILURE;
				}
			}

			wptr = static_cast<unsigned char*>(geBitmap_GetBits(LockedBMP));
			pptr = reinterpret_cast<unsigned char*>(pBmp) + pBmp->biSize;

			// The following weirdness is required because the DIB
			// ..coming in from the AVI file is INVERTED, so we have
			// ..to copy it to the target bitmap from the bottom
			// ..up to make it display correctly.  Thanks to Ernest
			// ..Kleynhans for catching this little mistake 11/22/99.

			pptr += (nWidth * ((nHeight - 1) * (pBmp->biBitCount >> 3)));

			// Ok, we've got a _somewhat_ optimized version of the bitmap
			// ..copy, it's far faster than memcpy() but it's not as
			// ..optimized as it could be.  However, for now, it does seem
			// ..to be Good Enough.

			switch(nFormat)
			{
			case GE_PIXELFORMAT_16BIT_555_RGB:
				nTemp2 = Info.Stride * 2;

				for(y=0; y<Info.Height; ++y)
				{
					__asm
					{
						mov esi, pptr
						mov edi, wptr
						mov ecx, nWidth
						rep movsw
					}

					wptr += nTemp2;
					pptr -= nAlignValue;
				}
				break;
			case GE_PIXELFORMAT_24BIT_BGR:
				nTemp = nWidth * 3;
				nTemp2 = Info.Stride * 3;

				for(y=0; y<Info.Height; ++y)
				{
					__asm
					{
						mov esi, pptr
						mov edi, wptr
						mov ecx, nTemp
						rep movs
					}

					wptr += nTemp2;
					pptr -= nAlignValue;
				}
				break;
			case GE_PIXELFORMAT_32BIT_XBGR:
				nTemp2 = Info.Stride * 4;

				for(y=0; y<Info.Height; ++y)
				{
					__asm
					{
						mov esi, pptr
						mov edi, wptr
						mov ecx, nWidth
						rep movsd
					}

					wptr += nTemp2;
					pptr -= nAlignValue;
				}
				break;
			}

			geBitmap_UnLock(LockedBMP);
			geEngine_BeginFrame(CCD->Engine()->Engine(), CCD->CameraManager()->Camera(), GE_TRUE);
			geEngine_DrawBitmap(CCD->Engine()->Engine(), theBmp, NULL, XPos, YPos);
			geEngine_EndFrame(CCD->Engine()->Engine());
		}

		// Check for SPACE to see if player has seen enough
		if((GetAsyncKeyState(VK_SPACE) & 0x8000) != 0)
		{
			while((GetAsyncKeyState(VK_SPACE) & 0x8000) != 0)
			{
			}

			if(bAudioStreamPlaying)
				DestroyStreamingAudioBuffer();

			bAudioStreamPlaying = false;
			geEngine_BeginFrame(CCD->Engine()->Engine(), CCD->CameraManager()->Camera(), GE_TRUE);
			geEngine_EndFrame(CCD->Engine()->Engine());
			break;													// Lemme out!
		}
	}

	EndVideoRetrieve(0);								// All done.

	geEngine_RemoveBitmap(CCD->Engine()->Engine(), theBmp);

	geBitmap_Destroy(&theBmp);

	// Ok, if we have an audio stream playing it's quite possible for
	// ..it to need to drain out even AFTER the video is done!  To
	// ..make this happen, we'll keep pumping audio until it's done
	// ..and then wait the max. time (1 second) for the buffer to clear.

	if(bAudioStreamPlaying)
	{
		for(int nTemp=0; nTemp<10; ++nTemp)
		{
			PumpBuffer(0, false);				// Make sure nothing but silence
			Sleep(50);							// Let audio buffer play out
		}

		DestroyStreamingAudioBuffer();			// Kill any audio streaming
	}

	// Close the file down.
	Close();

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// Open
//
// Open an AVI file for playback, but don't start playing it.
/* ------------------------------------------------------------------------------------ */
int CAVIPlayer::Open(const char *szFile)
{
	if((szFile == NULL) || (strlen(szFile) <= 0))
		return RGF_FAILURE;					// Wrong!

	char szTemp[256];

	strncpy(szTemp, CCD->GetDirectory(kVideoFile), 255);
	szTemp[255] = 0;
	strncat(szTemp, "\\", 255-strlen(szTemp));
	strncat(szTemp, szFile, 255-strlen(szTemp));

	if(AVIFileOpen(&m_pAviFile, szTemp, OF_READ, NULL))
	{
		char szBug[256];
		sprintf(szBug, "[WARNING] File %s - Line %d: Failed to open AVI file %s",
				__FILE__, __LINE__, szTemp);
		CCD->ReportError(szBug, false);

		return RGF_FAILURE;					// Wrong again!
	}

	// File open, now scan it to built the data structures containing
	// ..audio and video stream information.
	FindStreams();

	// Ok, check to see if the audio and video formats are proper
	if(!DetermineAudioFormats() || !DetermineVideoFormats())
	{
		char szBug[256];
		sprintf(szBug, "[WARNING] File %s - Line %d: AVI file %s has invalid/indeterminate formats",
				__FILE__, __LINE__, szFile);
		CCD->ReportError(szBug, false);

		Release();
		return RGF_FAILURE;
	}

	m_nLastFramePlayed = -1;					// No last frame played
	m_LastFrameBitmap = NULL;					// No last bitmap frame

	// The file is open, we've loaded all the info we need about the
	// ..file, and we're ready to start streaming from it.
	if(CCD->GetLogging())
	{
		char szDebug[512];
		sprintf(szDebug, "[INFO] Loaded %s", szFile);
		CCD->ReportError(szDebug, false);
	}

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// DisplayFrameAt
//
// Display a single frame from the previously opened AVI file
// ..at a specific location in the game window.  The frame is
// ..determined by the time in milliseconds passed in.
/* ------------------------------------------------------------------------------------ */
int CAVIPlayer::DisplayFrameAt(int XPos, int YPos, DWORD dwTime)
{
	if(!CCD->GetHasFocus())
		return RGF_SUCCESS;

	if(!StartVideoRetrieve(0))
		return RGF_FAILURE;

	// Get the bitmap info for the file
	LPBITMAPINFOHEADER pBmp;				// Will hold decompressed frame

	LPBITMAPINFO pVideoFormat = GetVideoFormat(0);	// Video format
	if(!pVideoFormat)
		return RGF_FAILURE;

	// Let's grab a frame and blit it, shall we?

	int nWidth = pVideoFormat->bmiHeader.biWidth;
	int nHeight = pVideoFormat->bmiHeader.biHeight;
	int nAlignValue = 0;
	gePixelFormat nFormat = GE_PIXELFORMAT_NO_DATA;

	// Here's how it shakes down: 16bit color is always RGB/555,
	// ..24bit color is always BGR, and 32bit color is always
	// ..BGR with the high byte unused (worthless).  I'm NOT supporting
	// ..8bit color from video files, if you want it, put it in.

	switch(pVideoFormat->bmiHeader.biBitCount)
	{
	case 8:
		CCD->ReportError("[WARNING] 8-bit video unsupported", false);
		EndVideoRetrieve(0);
		Close();
		return RGF_FAILURE;
	case 16:
		nFormat = GE_PIXELFORMAT_16BIT_555_RGB;
		nAlignValue = (nWidth * 2) + ((nWidth * 2) % 4);
		break;
	case 24:
		nFormat = GE_PIXELFORMAT_24BIT_BGR;
		nAlignValue = (nWidth * 3) + ((nWidth * 3) % 4);
		break;
	case 32:
		nFormat = GE_PIXELFORMAT_32BIT_XBGR;
		nAlignValue = nWidth * 4;
		break;
	}

	geBitmap *theBmp, *LockedBMP;
	geBitmap_Info Info;
	unsigned char *wptr ,*pptr;

	theBmp = geBitmap_Create(nWidth, nHeight, 1, nFormat);
	geBitmap_SetPreferredFormat(theBmp, nFormat);
	geEngine_AddBitmap(CCD->Engine()->Engine(), theBmp);
	geBitmap_GetInfo(theBmp, &Info, NULL);
	geBitmap_ClearMips(theBmp);

	pBmp = NULL;
	GetVideoFrameAtTime(0, dwTime, &pBmp);

	int nStatus = RGF_FAILURE;			// Assume failure

	if(pBmp)
	{
		// Lock the created bitmap for write!
		geBitmap_LockForWriteFormat(theBmp, &LockedBMP, 0, 0, nFormat);

		// The following block of code comes to us courtesy of
		// ..Ralph Deane, who discovered that the lock sometimes
		// ..failed and how to fix it!
		if(LockedBMP == NULL)
		{
			geBitmap_SetFormat(theBmp, nFormat, GE_TRUE, 0, NULL);
			geBitmap_LockForWriteFormat(theBmp, &LockedBMP, 0, 0, nFormat);

			if(LockedBMP == NULL)
				return RGF_FAILURE;
		}

		wptr = static_cast<unsigned char*>(geBitmap_GetBits(LockedBMP));
		pptr = reinterpret_cast<unsigned char*>(pBmp) + pBmp->biSize;
		// The following weirdness is required because the DIB
		// ..coming in from the AVI file is INVERTED, so we have
		// ..to copy it to the target bitmap from the bottom
		// ..up to make it display correctly.  Thanks to Ernest
		// ..Kleynhans for catching this little mistake 11/22/99.

		pptr += (nWidth * ((nHeight - 1) * (pBmp->biBitCount >> 3)));

		// Ok, we've got a _somewhat_ optimized version of the bitmap
		// ..copy, it's far faster than memcpy() but it's not as
		// ..optimized as it could be.  However, for now, it does seem
		// ..to be Good Enough.

		int y;
		int nTemp, nTemp2;

		switch(nFormat)
		{
		case GE_PIXELFORMAT_16BIT_555_RGB:
			nTemp2 = Info.Stride * 2;

			for(y=0; y<Info.Height; ++y)
			{
				__asm
				{
					mov esi, pptr
					mov edi, wptr
					mov ecx, nWidth
					rep movsw
				}

				wptr += nTemp2;
				pptr -= nAlignValue;
			}
			break;
		case GE_PIXELFORMAT_24BIT_BGR:
			nTemp = nWidth * 3;
			nTemp2 = Info.Stride * 3;

			for(y=0; y<Info.Height; ++y)
			{
				__asm
				{
					mov esi, pptr
					mov edi, wptr
					mov ecx, nTemp
					rep movs
				}

				wptr += nTemp2;
				pptr -= nAlignValue;
			}
			break;
		case GE_PIXELFORMAT_32BIT_XBGR:
			nTemp2 = Info.Stride * 4;

			for(y=0; y<Info.Height; ++y)
			{
				__asm
				{
					mov esi, pptr
					mov edi, wptr
					mov ecx, nWidth
					rep movsd
				}

				wptr += nTemp2;
				pptr -= nAlignValue;
			}
			break;
		}

		geBitmap_UnLock(LockedBMP);
		geEngine_BeginFrame(CCD->Engine()->Engine(), CCD->CameraManager()->Camera(), GE_TRUE);
		geEngine_DrawBitmap(CCD->Engine()->Engine(), theBmp, NULL, XPos, YPos);
		geEngine_EndFrame(CCD->Engine()->Engine());
		nStatus = RGF_SUCCESS;
	}

	geEngine_RemoveBitmap(CCD->Engine()->Engine(), theBmp);
	geBitmap_Destroy(&theBmp);

	return nStatus;
}


/* ------------------------------------------------------------------------------------ */
// DisplayFrame
//
// Display a single, specific frame from the previously opened
// ..AVI file at a specific location in the game window.
/* ------------------------------------------------------------------------------------ */
int CAVIPlayer::DisplayFrame(int XPos, int YPos, int FrameID)
{
	if(!CCD->GetHasFocus())
		return RGF_SUCCESS;

	if(!StartVideoRetrieve(0))
		return RGF_FAILURE;

	// Get the bitmap info for the file
	LPBITMAPINFOHEADER pBmp;				// Will hold decompressed frame

	LPBITMAPINFO pVideoFormat = GetVideoFormat(0);	// Video format
	if(!pVideoFormat)
		return RGF_FAILURE;

	// Let's grab a frame and blit it, shall we?
	int nWidth = pVideoFormat->bmiHeader.biWidth;
	int nHeight = pVideoFormat->bmiHeader.biHeight;
	int nAlignValue = 0;
	gePixelFormat nFormat = GE_PIXELFORMAT_NO_DATA;

	// Here's how it shakes down: 16bit color is always RGB/555,
	// ..24bit color is always BGR, and 32bit color is always
	// ..BGR with the high byte unused (worthless).  I'm NOT supporting
	// ..8bit color from video files, if you want it, put it in.

	switch(pVideoFormat->bmiHeader.biBitCount)
	{
	case 8:
		CCD->ReportError("[WARNING] 8-bit video unsupported", false);
		EndVideoRetrieve(0);
		Close();
		return RGF_FAILURE;
	case 16:
		nFormat = GE_PIXELFORMAT_16BIT_555_RGB;
		nAlignValue = (nWidth * 2) + ((nWidth * 2) % 4);
		break;
	case 24:
		nFormat = GE_PIXELFORMAT_24BIT_BGR;
		nAlignValue = (nWidth * 3) + ((nWidth * 3) % 4);
		break;
	case 32:
		nFormat = GE_PIXELFORMAT_32BIT_XBGR;
		nAlignValue = nWidth * 4;
		break;
	}

	geBitmap *theBmp, *LockedBMP;
	geBitmap_Info Info;
	unsigned char *wptr ,*pptr;

	theBmp = geBitmap_Create(nWidth, nHeight, 1, nFormat);
	geBitmap_SetPreferredFormat(theBmp, nFormat);
	geEngine_AddBitmap(CCD->Engine()->Engine(), theBmp);
	geBitmap_GetInfo(theBmp, &Info, NULL);
	geBitmap_ClearMips(theBmp);

	pBmp = NULL;
	GetVideoFrame(0, FrameID, &pBmp);

	int nStatus = RGF_FAILURE;			// Assume failure

	if(pBmp)
	{
		// Lock the created bitmap for write!
		geBitmap_LockForWriteFormat(theBmp,&LockedBMP, 0, 0, nFormat);

		// The following block of code comes to us courtesy of
		// ..Ralph Deane, who discovered that the lock sometimes
		// ..failed and how to fix it!
		if(LockedBMP == NULL)
		{
			geBitmap_SetFormat(theBmp, nFormat, GE_TRUE, 0, NULL);
			geBitmap_LockForWriteFormat(theBmp, &LockedBMP, 0, 0, nFormat);

			if(LockedBMP == NULL)
				return RGF_FAILURE;
		}

		wptr = static_cast<unsigned char*>(geBitmap_GetBits(LockedBMP));
		pptr = reinterpret_cast<unsigned char*>(pBmp) + pBmp->biSize;

		// The following weirdness is required because the DIB
		// ..coming in from the AVI file is INVERTED, so we have
		// ..to copy it to the target bitmap from the bottom
		// ..up to make it display correctly.  Thanks to Ernest
		// ..Kleynhans for catching this little mistake 11/22/99.

		pptr += (nWidth * ((nHeight - 1) * (pBmp->biBitCount >> 3)));

		// Ok, we've got a _somewhat_ optimized version of the bitmap
		// ..copy, it's far faster than memcpy() but it's not as
		// ..optimized as it could be.  However, for now, it does seem
		// ..to be Good Enough.

		int y;
		int nTemp, nTemp2;

		switch(nFormat)
		{
		case GE_PIXELFORMAT_16BIT_555_RGB:
			nTemp2 = Info.Stride * 2;

			for(y=0; y<Info.Height; ++y)
			{
				__asm
				{
					mov esi, pptr
					mov edi, wptr
					mov ecx, nWidth
					rep movsw
				}

				wptr += nTemp2;
				pptr -= nAlignValue;
			}
			break;
		case GE_PIXELFORMAT_24BIT_BGR:
			nTemp = nWidth * 3;
			nTemp2 = Info.Stride * 3;

			for(y=0; y<Info.Height; ++y)
			{
				__asm
				{
					mov esi, pptr
					mov edi, wptr
					mov ecx, nTemp
					rep movs
				}

				wptr += nTemp2;
				pptr -= nAlignValue;
			}
			break;
		case GE_PIXELFORMAT_32BIT_XBGR:
			nTemp2 = Info.Stride * 4;

			for(y=0; y<Info.Height; ++y)
			{
				__asm
				{
					mov esi, pptr
					mov edi, wptr
					mov ecx, nWidth
					rep movsd
				}

				wptr += nTemp2;
				pptr -= nAlignValue;
			}
			break;
		}

		geBitmap_UnLock(LockedBMP);
		geEngine_BeginFrame(CCD->Engine()->Engine(), CCD->CameraManager()->Camera(), GE_TRUE);
		geEngine_DrawBitmap(CCD->Engine()->Engine(), theBmp, NULL, XPos, YPos);
		geEngine_EndFrame(CCD->Engine()->Engine());
		nStatus = RGF_SUCCESS;
	}

	geEngine_RemoveBitmap(CCD->Engine()->Engine(), theBmp);
	geBitmap_Destroy(&theBmp);

	return nStatus;
}


/* ------------------------------------------------------------------------------------ */
// Close
//
// Close the currently open AVI file, if there is one.
/* ------------------------------------------------------------------------------------ */
int CAVIPlayer::Close()
{
	EndVideoRetrieve(0);
	Release();

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// DisplayFrameTexture
//
// Passed the name of a texture and a frame number, this
// ..function will replace the current texture contents with the
// ..contents of the desired frame of the AVI file.  Note that this
// ..affects every polygon surfaced with this bitmap!!!
/* ------------------------------------------------------------------------------------ */
int CAVIPlayer::DisplayFrameTexture(int nFrame, const char *szTextureName)
{
	geBitmap *theBitmap;

	theBitmap = geWorld_GetBitmapByName(CCD->World(), szTextureName);

	if(theBitmap == NULL)
		return RGF_FAILURE;					// Doesn't exist?

	// Ok, we've got the bitmap, see if it's visible...
	if(geWorld_BitmapIsVisible(CCD->World(), theBitmap) != GE_TRUE)
		return RGF_SUCCESS;					// Lie, it's not visible anyway

	// Ok, we have a texture, we know it's visible, copy the desired
	// ..frame into the bitmap!
	geBitmap_ClearMips(theBitmap);

	if(!StartVideoRetrieve(0))
		return RGF_FAILURE;

	// Get the bitmap info for the file
	LPBITMAPINFOHEADER pBmp;				// Will hold decompressed frame

	LPBITMAPINFO pVideoFormat = GetVideoFormat(0);	// Video format
	if(!pVideoFormat)
		return RGF_FAILURE;

	// Let's grab a frame and blit it, shall we?
	int nWidth = pVideoFormat->bmiHeader.biWidth;
	int nHeight = pVideoFormat->bmiHeader.biHeight;
	int nAlignValue = 0;
	gePixelFormat nFormat = GE_PIXELFORMAT_NO_DATA;

	// Here's how it shakes down: 16bit color is always RGB/555,
	// ..24bit color is always BGR, and 32bit color is always
	// ..BGR with the high byte unused (worthless).  I'm NOT supporting
	// ..8bit color from video files, if you want it, put it in.

	switch(pVideoFormat->bmiHeader.biBitCount)
	{
	case 8:
		CCD->ReportError("[WARNING] 8-bit video unsupported", false);
		EndVideoRetrieve(0);
		Close();
		return RGF_FAILURE;
	case 16:
		nFormat = GE_PIXELFORMAT_16BIT_555_RGB;
		nAlignValue = (nWidth * 2) + ((nWidth * 2) % 4);
		break;
	case 24:
		nFormat = GE_PIXELFORMAT_24BIT_BGR;
		nAlignValue = (nWidth * 3) + ((nWidth * 3) % 4);
		break;
	case 32:
		nFormat = GE_PIXELFORMAT_32BIT_XBGR;
		nAlignValue = nWidth * 4;
		break;
	}

	geBitmap *LockedBMP;
	geBitmap_Info Info;
	unsigned char *wptr ,*pptr;

	geBitmap_GetInfo(theBitmap,&Info,NULL);
	geBitmap_ClearMips(theBitmap);

	pBmp = NULL;
	GetVideoFrame(0, nFrame, &pBmp);

	int nStatus = RGF_FAILURE;			// Assume failure

	if(pBmp)
	{
		// Lock the created bitmap for write!
		geBitmap_LockForWriteFormat(theBitmap,&LockedBMP,0,0, nFormat);
		// The following block of code comes to us courtesy of
		// ..Ralph Deane, who discovered that the lock sometimes
		// ..failed and how to fix it!
		if(LockedBMP == NULL)
		{
			geBitmap_SetFormat(theBitmap, nFormat, GE_TRUE, 0, NULL);
			geBitmap_LockForWriteFormat(theBitmap, &LockedBMP, 0, 0, nFormat);

			if(LockedBMP == NULL)
				return RGF_FAILURE;
		}

		wptr = static_cast<unsigned char*>(geBitmap_GetBits(LockedBMP));
		pptr = reinterpret_cast<unsigned char*>(pBmp) + pBmp->biSize;

		// The following weirdness is required because the DIB
		// ..coming in from the AVI file is INVERTED, so we have
		// ..to copy it to the target bitmap from the bottom
		// ..up to make it display correctly.  Thanks to Ernest
		// ..Kleynhans for catching this little mistake 11/22/99.

		pptr += (nWidth * ((nHeight - 1) * (pBmp->biBitCount >> 3)));

		// Ok, we've got a _somewhat_ optimized version of the bitmap
		// ..copy, it's far faster than memcpy() but it's not as
		// ..optimized as it could be.  However, for now, it does seem
		// ..to be Good Enough.

		int y;
		int nTemp, nTemp2;

		switch(nFormat)
		{
		case GE_PIXELFORMAT_16BIT_555_RGB:
			nTemp2 = Info.Stride * 2;

			for(y=0; y<Info.Height; ++y)
			{
				__asm
				{
					mov esi, pptr
					mov edi, wptr
					mov ecx, nWidth
					rep movsw
				}

				wptr += nTemp2;
				pptr -= nAlignValue;
			}
			break;
		case GE_PIXELFORMAT_24BIT_BGR:
			nTemp = nWidth * 3;
			nTemp2 = Info.Stride * 3;

			for(y=0; y<Info.Height; ++y)
			{
				__asm
				{
					mov esi, pptr
					mov edi, wptr
					mov ecx, nTemp
					rep movs
				}

				wptr += nTemp2;
				pptr -= nAlignValue;
			}
			break;
		case GE_PIXELFORMAT_32BIT_XBGR:
			nTemp2 = Info.Stride * 4;

			for(y=0; y<Info.Height; ++y)
			{
				__asm
				{
					mov esi, pptr
					mov edi, wptr
					mov ecx, nWidth
					rep movsd
				}

				wptr += nTemp2;
				pptr -= nAlignValue;
			}
			break;
		}

		geBitmap_UnLock(LockedBMP);
		nStatus = RGF_SUCCESS;
	}

	// Ok, we (hopefully!) did it!  Let's bail out.
	return nStatus;
}


/* ------------------------------------------------------------------------------------ */
// DisplayNextFrameTexture
//
// Passed the name of a texture and a first/next flag, this
// ..function will replace the current texture contents with the
// ..contents of the appropriate frame of the AVI file.  Note that this
// ..affects every polygon surfaced with this bitmap!!!  Note that
// ..there are a LOT of static in this function, this is in order
// ..to minimize the impact of repeated calls into this function
// ..after the "first frame" call.  It makes a SIGNIFICANT performance
// ..improvement to do this.
/* ------------------------------------------------------------------------------------ */
int CAVIPlayer::DisplayNextFrameTexture(const char *szTextureName, bool bFirstFrame)
{
	static int OldTime = 0;
	static int FrameTime = 0;
	static geBitmap *theBitmap;
	static geBitmap_Info Info;
	static LPBITMAPINFO pVideoFormat;
	static int nWidth, nHeight;
	static gePixelFormat nFormat;
	static int nAlignValue;
	int nStatus = RGF_FAILURE;			// Assume failure
	LPBITMAPINFOHEADER pBmp;			// Will hold decompressed frame
	geBitmap *LockedBMP;
	unsigned char *wptr ,*pptr;

	// If this is the first frame, clear out all the timing variables
	if(bFirstFrame)
	{
		OldTime = CCD->FreeRunningCounter();		// Prime the time
		FrameTime = 0;								// No time passed yet

		if(!StartVideoRetrieve(0))					// Prime the video pump
			return RGF_FAILURE;

		// Locate the bitmap to be replaced in the world.
		theBitmap = geWorld_GetBitmapByName(CCD->World(), szTextureName);

		if(theBitmap == NULL)
			return RGF_FAILURE;					// Doesn't exist?

		geBitmap_ClearMips(theBitmap);
		geBitmap_GetInfo(theBitmap,&Info,NULL);
		geBitmap_ClearMips(theBitmap);
		pVideoFormat = GetVideoFormat(0);	// Video format
		if(!pVideoFormat)
			return RGF_FAILURE;

		nWidth = pVideoFormat->bmiHeader.biWidth;
		nHeight = pVideoFormat->bmiHeader.biHeight;

		// Here's how it shakes down: 16bit color is always RGB/555,
		// ..24bit color is always BGR, and 32bit color is always
		// ..BGR with the high byte unused (worthless).  I'm NOT supporting
		// ..8bit color from video files, if you want it, put it in.

		switch(pVideoFormat->bmiHeader.biBitCount)
		{
		case 8:
			CCD->ReportError("[WARNING] 8-bit video unsupported", false);
			EndVideoRetrieve(0);
			Close();
			return RGF_FAILURE;
		case 16:
			nFormat = GE_PIXELFORMAT_16BIT_555_RGB;
			nAlignValue = (nWidth * 2) + ((nWidth * 2) % 4);
			break;
		case 24:
			nFormat = GE_PIXELFORMAT_24BIT_BGR;
			nAlignValue = (nWidth * 3) + ((nWidth * 3) % 4);
			break;
		case 32:
			nFormat = GE_PIXELFORMAT_32BIT_XBGR;
			nAlignValue = nWidth * 4;
			break;
		}
	}

	// Compute the time between passes so we can pick the right frame
	int ElapsedTime = (CCD->FreeRunningCounter() - OldTime);

	// We don't want this routine entered more often than once
	// ..every 40msec (which is more than a 20fps. rendering rate!)
	// ..or the whole system crawls like a swatted fly.

	if(ElapsedTime < 40)
		return RGF_SUCCESS;

	//if(ElapsedTime > 60)
		//ElapsedTime = 60;					// Correct for stalls somewhere else

	FrameTime += ElapsedTime;
	OldTime = CCD->FreeRunningCounter();

	// Ok, grab out the appropriate video frame for processing

	pBmp = NULL;
	GetVideoFrameAtTime(0, FrameTime, &pBmp);

	if(pBmp)
	{
		// Lock the created bitmap for write!
		LockedBMP = NULL;
		geBitmap_LockForWriteFormat(theBitmap,&LockedBMP,0,0, nFormat);
		// The following block of code comes to us courtesy of
		// ..Ralph Deane, who discovered that the lock sometimes
		// ..failed and how to fix it!
		if(LockedBMP == NULL)
		{
			geBitmap_SetFormat(theBitmap,nFormat,GE_TRUE,0,NULL);
			geBitmap_LockForWriteFormat(theBitmap,&LockedBMP,0,0, nFormat);

			if(LockedBMP == NULL)
				return RGF_FAILURE;
		}

		wptr = static_cast<unsigned char*>(geBitmap_GetBits(LockedBMP));
		pptr = reinterpret_cast<unsigned char*>(pBmp) + pBmp->biSize;

		// The following weirdness is required because the DIB
		// ..coming in from the AVI file is INVERTED, so we have
		// ..to copy it to the target bitmap from the bottom
		// ..up to make it display correctly.  Thanks to Ernest
		// ..Kleynhans for catching this little mistake 11/22/99.

		pptr += (nWidth * ((nHeight - 1) * (pBmp->biBitCount >> 3)));

		// Ok, we've got a _somewhat_ optimized version of the bitmap
		// ..copy, it's far faster than memcpy() but it's not as
		// ..optimized as it could be.  However, for now, it does seem
		// ..to be Good Enough.

		int y, nTemp, nTemp2;

		switch(nFormat)
		{
		case GE_PIXELFORMAT_16BIT_555_RGB:
			nTemp2 = Info.Stride * 2;

			for(y=0; y<Info.Height; ++y)
			{
				__asm
				{
					mov esi, pptr
					mov edi, wptr
					mov ecx, nWidth
					rep movsw
				}

				wptr += nTemp2;
				pptr -= nAlignValue;
			}
			break;
		case GE_PIXELFORMAT_24BIT_BGR:
			nTemp = nWidth * 3;
			nTemp2 = Info.Stride * 3;

			for(y=0; y<Info.Height; ++y)
			{
				__asm
				{
					mov esi, pptr
					mov edi, wptr
					mov ecx, nTemp
					rep movs
				}

				wptr += nTemp2;
				pptr -= nAlignValue;
			}
			break;
		case GE_PIXELFORMAT_32BIT_XBGR:
			nTemp2 = Info.Stride * 4;

			for(y=0; y<Info.Height; ++y)
			{
				__asm
				{
					mov esi, pptr
					mov edi, wptr
					mov ecx, nWidth
					rep movsd
				}

				wptr += nTemp2;
				pptr -= nAlignValue;
			}
			break;
		}

		geBitmap_UnLock(LockedBMP);
		nStatus = RGF_SUCCESS;
	}
	else
	{
		// End of video, let's "rewind" it automatically
		OldTime = CCD->FreeRunningCounter();			// Prime the time
		FrameTime = 0;									// No time passed yet
	}

	// Ok, we (hopefully!) did it!  Let's bail out.
	return nStatus;
}

// ******************** PRIVATE MEMBER FUNCTIONS ***********************

/* ------------------------------------------------------------------------------------ */
// Init
//
// Clean up the objects buffers, etc.
/* ------------------------------------------------------------------------------------ */
void CAVIPlayer::Init()
{
	m_pAviFile = NULL;
	m_nNumAudioStreams = 0;
	m_nNumVideoStreams = 0;

	int n;

	for(n=0; n<MAX_AUDIO_STREAMS; ++n)
	{
		m_pAudioStreams[n] = NULL;
		m_pAudioFormats[n] = NULL;
		m_pAudioData[n] = NULL;
	}

	for(n=0; n<MAX_VIDEO_STREAMS; ++n)
	{
		m_pVideoStreams[n] = NULL;
		m_pVideoFormats[n] = NULL;
		m_pVideoPGF[n] = NULL;
		m_lVideoEndTime[n] = 0;
	}

	m_bVideoPrimed = false;

	return;
}


/* ------------------------------------------------------------------------------------ */
// Release
//
// Let go of the piles of stuff we allocated to track this
// ..AVI file.
/* ------------------------------------------------------------------------------------ */
void CAVIPlayer::Release()
{
	EndVideoRetrieve(0);

	int n;
	for(n=0; n<m_nNumAudioStreams; ++n)
	{
		if(m_pAudioStreams[n])
			AVIStreamRelease(m_pAudioStreams[n]);

		delete [] ((LPBYTE)m_pAudioFormats[n]);

		delete [] m_pAudioData[n];
	}

	for(n=0; n<m_nNumVideoStreams; ++n)
	{
		if(m_pVideoStreams[n])
			AVIStreamRelease(m_pVideoStreams[n]);

		delete [] ((LPBYTE)m_pVideoFormats[n]);
	}

	if(m_pAviFile)
		AVIFileRelease(m_pAviFile);

	Init();
}


/* ------------------------------------------------------------------------------------ */
// FindStreams
//
// Locate the various audio and video streams in an AVI file.
// ..Loads the internal structures of the current instance with
// ..the located data.
/* ------------------------------------------------------------------------------------ */
void CAVIPlayer::FindStreams()
{
	do
	{
		if(AVIFileGetStream(m_pAviFile, &m_pAudioStreams[m_nNumAudioStreams],
			streamtypeAUDIO, m_nNumAudioStreams))
			break;
	}while(++m_nNumAudioStreams < MAX_AUDIO_STREAMS);

	do
	{
		if(AVIFileGetStream(m_pAviFile, &m_pVideoStreams[m_nNumVideoStreams],
			streamtypeVIDEO, m_nNumVideoStreams))
			break;
	}while(++m_nNumVideoStreams < MAX_VIDEO_STREAMS);
}


/* ------------------------------------------------------------------------------------ */
// DetermineAudioFormats
//
// Figure out what audio formats the various audio streams are.
// ..Load the instances internal variables with the info.
/* ------------------------------------------------------------------------------------ */
bool CAVIPlayer::DetermineAudioFormats()
{
	for(int n=0; n<m_nNumAudioStreams; ++n)
	{
		PAVISTREAM pStream = m_pAudioStreams[n];
		LONG lSize;

		if(AVIStreamReadFormat(pStream, AVIStreamStart(pStream), NULL, &lSize))
			return false;

		LPBYTE pChunk = new BYTE[lSize];

		if(!pChunk)
			return false;

		if(AVIStreamReadFormat(pStream, AVIStreamStart(pStream), pChunk, &lSize))
			return false;

		m_pAudioFormats[n] = reinterpret_cast<LPWAVEFORMATEX>(pChunk);
	}

	return true;
}


/* ------------------------------------------------------------------------------------ */
// DetermineVideoFormats
//
// Figure out what vodei formats the various audio streams are.
// ..Load the instances internal variables with the info.
/* ------------------------------------------------------------------------------------ */
bool CAVIPlayer::DetermineVideoFormats()
{
	for(int n=0; n<m_nNumVideoStreams; ++n)
	{
		PAVISTREAM pStream = m_pVideoStreams[n];
		LONG lSize;

		if(AVIStreamReadFormat(pStream, AVIStreamStart(pStream), NULL, &lSize))
			return false;

		LPBYTE pChunk = new BYTE[lSize];

		if(!pChunk)
			return false;

		if(AVIStreamReadFormat(pStream, AVIStreamStart(pStream), pChunk, &lSize))
			return false;

		m_pVideoFormats[n] = (LPBITMAPINFO)pChunk;
	}

	return true;
}


/* ------------------------------------------------------------------------------------ */
// ExtractAudioStream
//
// Read the next buffer of audio data from a specified audio stream
// ..in the currently open AVI file.
/* ------------------------------------------------------------------------------------ */
int CAVIPlayer::ExtractAudioStream(int nStreamNum, int nSamples, LPBYTE pBuffer)
{
	long nReadIn = 0;
	static int nSamplePos = 0;
	long nSamplesIn = 0;

	if(nStreamNum >= m_nNumAudioStreams)
	{
		CCD->ReportError("[WARNING] ExtractAudioStream: bad stream ID", false);
		return 0;
	}

	int nBufSize = nSamples * GetAudioFormat(nStreamNum)->nBlockAlign;

	PAVISTREAM pStream = m_pAudioStreams[nStreamNum];

	if(AVIStreamRead(pStream, nSamplePos, nSamples, pBuffer, nBufSize, &nReadIn, &nSamplesIn))
	{
		CCD->ReportError("[WARNING] ExtractAudioStream: Error reading AVI stream\n", false);
		return 0;
	}

#ifdef MONDO_DEBUG
	char szCrapola[256];
	sprintf(szCrapola, "nReadIn %ld nSamplesIn %ld wanted %d start %d\n", nReadIn,
		nSamplesIn, nSamples, nSamplePos);
	OutputDebugString(szCrapola);
#endif

	nSamplePos += nSamplesIn;

	return static_cast<int>(nReadIn);
}


/* ------------------------------------------------------------------------------------ */
// StartVideoRetrieve
//
// Initialize the desired stream such that it's ready to start pulling
// ..bitmaps out of for display.
/* ------------------------------------------------------------------------------------ */
bool CAVIPlayer::StartVideoRetrieve(int nStreamNum)
{
	if(nStreamNum >= m_nNumVideoStreams)
		return false;

	if(m_bVideoPrimed)
		return true;					// Already primed

	PAVISTREAM pStream = m_pVideoStreams[nStreamNum];

	PGETFRAME &pgf = m_pVideoPGF[nStreamNum];

	pgf = AVIStreamGetFrameOpen(pStream, NULL);

	if(!pgf)
	{
		return false;
	}

	m_lVideoEndTime[nStreamNum] = AVIStreamEndTime(pStream);

	m_bVideoPrimed = true;

	return true;
}


/* ------------------------------------------------------------------------------------ */
// EndVideoRetrieve
//
// Close down the video stream we had previously prepared.  This
// ..cleans up the AVI subsystem for this stream.
/* ------------------------------------------------------------------------------------ */
bool CAVIPlayer::EndVideoRetrieve(int nStreamNum)
{
	if(!m_bVideoPrimed)
		return true;					// Not primed

	PGETFRAME &pgf = m_pVideoPGF[nStreamNum];

	if(AVIStreamGetFrameClose(pgf))
	{
		return false;
	}

	pgf = NULL;

	m_bVideoPrimed = false;

	return true;
}


/* ------------------------------------------------------------------------------------ */
// GetVideoFrameAtTime
//
// Read a specific video frame, computed from the time in milliseconds,
// ..into a bitmap buffer suitable for display.
/* ------------------------------------------------------------------------------------ */
void CAVIPlayer::GetVideoFrameAtTime(int nStreamNum, LONG lTimeInMilliSec, LPBITMAPINFOHEADER *ppbi)
{
	if(nStreamNum >= m_nNumVideoStreams)
	{
		*ppbi = NULL;
		return;
	}

	PAVISTREAM pStream = m_pVideoStreams[nStreamNum];
	PGETFRAME &pgf = m_pVideoPGF[nStreamNum];

	LONG lFrame;

	if(lTimeInMilliSec <= m_lVideoEndTime[nStreamNum])
	{
		lFrame = AVIStreamTimeToSample(pStream, lTimeInMilliSec) + 1;
	}
	else
	{
		*ppbi = NULL; // video is done, no more frames
		return;
	}

	if(lFrame <= 0)
		lFrame = 1;								// Avoid the deadly "frame 0"

	// Ok, if this is a new frame, load the desired one from the open
	// ..video file.
	if(m_nLastFramePlayed != lFrame)
	{
		m_LastFrameBitmap = (LPBITMAPINFOHEADER)AVIStreamGetFrame(pgf, lFrame);
		m_nLastFramePlayed = lFrame;
	}

	*ppbi = m_LastFrameBitmap;		// Used new or cached
}


/* ------------------------------------------------------------------------------------ */
// GetVideoFrame
//
// Retrieves a specific frame number from the stream desired into a
// ..bitmap suitable for display.
/* ------------------------------------------------------------------------------------ */
void CAVIPlayer::GetVideoFrame(int nStreamNum, LONG lFrame, LPBITMAPINFOHEADER *ppbi)
{
	PGETFRAME &pgf = m_pVideoPGF[nStreamNum];

	if(lFrame <= 0)
		lFrame = 1;					// Avoid the deadly "frame 0"

	*ppbi = (LPBITMAPINFOHEADER)AVIStreamGetFrame(pgf, lFrame);
}


/* ------------------------------------------------------------------------------------ */
// GetAudioFormat
//
// Determine the audio format of a specific audio stream in the
// ..currently open AVI file.
/* ------------------------------------------------------------------------------------ */
LPWAVEFORMATEX CAVIPlayer::GetAudioFormat(int nStreamNum)
{
	if(nStreamNum >= m_nNumAudioStreams)
		return NULL;

	return m_pAudioFormats[nStreamNum];
}


/* ------------------------------------------------------------------------------------ */
// GetVideoFormat
//
// Determine the video format of a specific video stream in the
// ..currently open AVI file.
/* ------------------------------------------------------------------------------------ */
LPBITMAPINFO CAVIPlayer::GetVideoFormat(int nStreamNum)
{
	if(nStreamNum >= m_nNumVideoStreams)
		return NULL;

	return m_pVideoFormats[nStreamNum];
}


/* ------------------------------------------------------------------------------------ */
// CreateStreamingAudioBuffer
//
// Create a streaming audio buffer for audio playback from
// ..a video file.  This buffer will need to be periodically
// ..fed by calls to the PumpBuffer() routine.
/* ------------------------------------------------------------------------------------ */
int CAVIPlayer::CreateStreamingAudioBuffer(int nAudioStreamID)
{
	m_pDS = (LPDIRECTSOUND)geSound_GetDSound();

	// Fetch DirectSound interface we want
	LPDIRECTSOUND pDSIF;

	m_pDS->QueryInterface(IID_IDirectSound, reinterpret_cast<LPVOID*>(&pDSIF));

	// Create a DSound buffer to stream into
	DSBUFFERDESC theDesc;

	memset(&theDesc, 0, sizeof (DSBUFFERDESC));
	theDesc.dwSize = sizeof (DSBUFFERDESC);
	theDesc.lpwfxFormat = GetAudioFormat(nAudioStreamID);
	theDesc.dwBufferBytes = 22000 * theDesc.lpwfxFormat->nBlockAlign;

	m_nBufSize = theDesc.dwBufferBytes;

	int nError = pDSIF->CreateSoundBuffer(&theDesc, &m_pStream, NULL);

	pDSIF->Release();									// Done w/ this.

	if(nError != 0)										// Error!  Sick out.
	{
		char szBug[128];
		sprintf(szBug, "[WARNING] File %s - Line %d: Failed to create buffer for streamID %d\n",
				__FILE__, __LINE__, nAudioStreamID);
		CCD->ReportError(szBug, false);
		return RGF_FAILURE;
	}

	// Lock the ENTIRE buffer and fill it with SILENCE

	void *lpbuf1 = NULL, *lpbuf2 = NULL;
	long dwsize1 = 0, dwsize2 = 0;

	// Ok, we need to set up our "silence" value and adjust it for
	// ..8bit samples if needed.
	int nSilence = 0x0;

	if(GetAudioFormat(nAudioStreamID)->wBitsPerSample == 8)
		nSilence = 0x80;										// In case wave is 8-bit

	// Ok, try to lock <n>K of the buffer.  If it fails, just bail this
	// ..function.
	HRESULT hr = m_pStream->Lock(0, m_nBufSize,
								&lpbuf1, reinterpret_cast<DWORD*>(&dwsize1),
								&lpbuf2, reinterpret_cast<DWORD*>(&dwsize2),
								DSBLOCK_ENTIREBUFFER);

	if(hr != DS_OK)
	{
#ifdef MONDO_DEBUG
		OutputDebugString("CreateStreamingAudioBuffer: buflock failed!\n");
#endif
		return RGF_FAILURE;
	}

	if(lpbuf1 != NULL)
		memset(lpbuf1, nSilence, dwsize1);				// Clear to silence

	if(lpbuf2 != NULL)
		memset(lpbuf2, nSilence, dwsize2);				// Here, also

	m_pStream->Unlock(lpbuf1, dwsize1, lpbuf2, dwsize2);

	m_nOffset = 0;
	m_AudioEOF = false;

	PumpBuffer(nAudioStreamID, true);							// Pump it!

	// Ok, let's do the FIRST pump on it and start it playing!
	m_pStream->Play(0, 0, DSBPLAY_LOOPING);	// Start playback

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// DestroyStreamingAudioBuffer
//
// Close off and clean up the audio playback buffer.
/* ------------------------------------------------------------------------------------ */
int CAVIPlayer::DestroyStreamingAudioBuffer()
{
	m_pStream->Stop();								// Stop playback

	m_pStream->Release();

	m_pStream = NULL;									// Zapped.

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// PumpBuffer
//
// Fills the streaming audio buffer from an audio stream in
// ..an open AVI file.  At end of file, the buffer plays
// ..silence.
/* ------------------------------------------------------------------------------------ */
void CAVIPlayer::PumpBuffer(int nAudioStreamID, bool ForceLoad)
{
	HRESULT hr;
	void *lpbuf1 = NULL, *lpbuf2 = NULL;
	long dwsize1 = 0, dwsize2 = 0, nSize;
	int nBytesRead = 0;

	// Ok, we need to set up our "silence" value and adjust it for
	// ..8bit samples if needed.
	int nSilence = 0x0;

	if(GetAudioFormat(nAudioStreamID)->wBitsPerSample == 8)
		nSilence = 0x80;										// In case wave is 8-bit

	int nBlockSize = GetAudioFormat(nAudioStreamID)->nBlockAlign;

	// If this is a FORCE LOAD, we want to fill the WHOLE BUFFER with
	// ..audio, so lock it and LOAD!
	if(ForceLoad)
	{
		hr = m_pStream->Lock(0, m_nBufSize,
								&lpbuf1, reinterpret_cast<DWORD*>(&dwsize1),
								&lpbuf2, reinterpret_cast<DWORD*>(&dwsize2),
								0);
		m_nOffset = (m_nOffset + dwsize1 + dwsize2) % m_nBufSize;
		nBytesRead = ExtractAudioStream(nAudioStreamID, dwsize1 / nBlockSize, (LPBYTE)lpbuf1);

		if(nBytesRead != dwsize1)
			m_AudioEOF = true;

		if(!m_AudioEOF && (lpbuf2 != NULL))
		{
			nBytesRead = ExtractAudioStream(nAudioStreamID, dwsize2 / nBlockSize, (LPBYTE)lpbuf2);
			if(nBytesRead != dwsize1)
				m_AudioEOF = true;
		}

		m_pStream->Unlock(lpbuf1, dwsize1, lpbuf2, dwsize2);

		return;
	}

	if(GetMaxWriteSize() < nBlockSize * 512)
		return;

	nSize = GetMaxWriteSize() / nBlockSize;	// Samples in whole buffer

	// Ok, try to lock <n>K of the buffer.  If it fails, just bail this
	// ..function.
	hr = m_pStream->Lock(m_nOffset, nSize * nBlockSize,
							&lpbuf1, reinterpret_cast<DWORD*>(&dwsize1),
							&lpbuf2, reinterpret_cast<DWORD*>(&dwsize2),
							0);

	if(hr != DS_OK)
	{
		CCD->ReportError("[WARNING] PumpWave: Failed to lock", false);
#ifdef MONDO_DEBUG
		char szFrack[256];
		sprintf(szFrack,"locktry for %ld audiobuf bytes failed\n", nSize);
		OutputDebugString(szFrack);
#endif
		return;														// Fake it, bail out
	}

	if(lpbuf1 != NULL)
	{
		memset(lpbuf1, nSilence, dwsize1);				// Clear to silence
#ifdef MONDO_DEBUG
		OutputDebugString("Clear buf1\n");
#endif
	}

	if(lpbuf2 != NULL)
	{
		memset(lpbuf2, nSilence, dwsize2);				// Here, also
#ifdef MONDO_DEBUG
		OutputDebugString("Clear buf2\n");
#endif
	}

	m_nOffset = (m_nOffset + dwsize1 + dwsize2) % m_nBufSize;

#ifdef MONDO_DEBUG
	char szGloop[256];
	sprintf(szGloop, "offset %d dwsize1 %ld ptr %p dwsize2 %ld ptr %p\n", m_nOffset,
		dwsize1, lpbuf1, dwsize2, lpbuf2);
	OutputDebugString(szGloop);
#endif

	if(m_AudioEOF == true)
	{
		m_pStream->Unlock(lpbuf1, dwsize1, lpbuf2, dwsize2);
#ifdef MONDO_DEBUG
		OutputDebugString("End of audio stream unlock, no read performed\n");
#endif
		return;														// End of file, pump silence
	}

	// Fine, read data into the circular buffer directly from the
	// ..video file if there's anything there.
	nBytesRead = ExtractAudioStream(nAudioStreamID, dwsize1 / nBlockSize, (LPBYTE)lpbuf1);

	if(nBytesRead != dwsize1)									// End of audio in the video
	{
		m_AudioEOF = true;
#ifdef MONDO_DEBUG
		char szScum[256];
		sprintf(szScum, "EOFAudio wanted %ld read %d\n", dwsize1, nBytesRead);
		OutputDebugString(szScum);
#endif
		m_nOffset = 0;
	}

	if((lpbuf2 != NULL) && (!m_AudioEOF))
	{
		nBytesRead = ExtractAudioStream(nAudioStreamID, dwsize2 / nBlockSize, (LPBYTE)lpbuf2);

		if(nBytesRead != dwsize2)									// End of wave file
		{
			m_AudioEOF = true;
#ifdef MONDO_DEBUG
			OutputDebugString("Second test: end of audio stream\n");
#endif
			m_nOffset = 0;
		}

#ifdef MONDO_DEBUG
		OutputDebugString("Second audio buffer loaded\n");
#endif
	}

	// Unlock buffer, we're done with it for now.
	m_pStream->Unlock(lpbuf1, dwsize1, lpbuf2, dwsize2);

	return;
}


/* ------------------------------------------------------------------------------------ */
// GetMaxWriteSize
//
// Get the maximum number of bytes we can write into the current buffer.
/* ------------------------------------------------------------------------------------ */
int CAVIPlayer::GetMaxWriteSize()
{
	DWORD dwWriteCursor, dwPlayCursor, dwMaxSize;

	// Get current play position
	if(m_pStream->GetCurrentPosition(&dwPlayCursor, &dwWriteCursor) == DS_OK)
	{
		if((DWORD)m_nOffset <= dwPlayCursor)
		{
			// Our write position trails play cursor
			dwMaxSize = dwPlayCursor - m_nOffset;
		}
		else // (m_cbBufOffset > dwPlayCursor)
		{
			// Play cursor has wrapped
			dwMaxSize = m_nBufSize - m_nOffset + dwPlayCursor;
		}

#ifdef MONDO_DEBUG
		char szFudge[256];
		sprintf(szFudge,"playcursor %x writecursor %x max %d offset %d\n",
			dwPlayCursor, dwWriteCursor, dwMaxSize, m_nOffset);
		OutputDebugString(szFudge);
#endif
	}
	else
	{
		dwMaxSize = 0;
#ifdef MONDO_DEBUG
		OutputDebugString("Ouch!\n");
#endif
	}

	return (dwMaxSize & 0xfffffffe);
}

/* ----------------------------------- END OF FILE ------------------------------------ */
