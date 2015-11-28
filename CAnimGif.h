#include "RabidFramework.h"
/************************************************************************************//**
 * @file CAnimGif.h
 * @brief Play AnimGif file into Genesis3D engine
 *
 * This file contains the class declaration for the CAnimGif class. This class
 * will take an AnimGif file and play it back into the Genesis3D engine window
 * at a specific position.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_CANIMGIF_H__
#define __RGF_CANIMGIF_H__

typedef struct
{
	BYTE bit;
	WORD previouscode;
	WORD nextcode;

} GIFTABLE;


class CAnimGif : public CRGFComponent
{
public:
	CAnimGif(char *szFile, int fileformat);
	~CAnimGif();

	int Play(int XPos, int YPos, bool Center);
	geBitmap *NextFrame(bool repeat);
	bool DisplayNextFrameTexture(char *szTextureName, bool FFrame);

private:

	bool GetImage(bool repeat);
	bool TakeIt(void);
	void Output(BYTE bit);
	BYTE GetByte(void);
	WORD GetCode(void);

private:

	geVFile			*MainFS;
	BYTE			buffer[1024];
	bool			Active;
	bool			Texture;
	bool			FirstFrame;
	int				nWidth;
	int				nHeight;
	int				BackgroundColor;
	int				GlobalColorSize;
	BYTE			*GlobalColorTable;
	int				GifSize;
	BYTE			*pcGif;
	BYTE			*pcGifTrack;
	int				TotalReadByte;
	int				DisposalMethod;
	int				TransparentIndex;
	BOOL			bTransparentIndex;
	int				DelayTime;
	int				iWidth, iHeight;
	int				iTop, iLeft;
	int				iWidth1;
	BYTE			cCurentByte,cPackedField;
	UINT			uBitSize, uPrimaryBitSize;
	int				iFinishCode, iResetCode;
	UINT			uRemain, uReadByte, uBlockSize;
	int				x, y;
	int				iPass, iRow;
	int				iMaxByte;
	geBitmap_Palette *Palette;
	BYTE			*pcBitmap;
	geBitmap		*theBmp;
	geBitmap		*LockedBMP;
	geBitmap_Info	Info;
	float			VAnimTime;

};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
