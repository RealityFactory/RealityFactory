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

/**
 * @brief Class to handle the playback of animated GIF files
 */
class CAnimGif : public CRGFComponent
{
public:
	CAnimGif(const char *szFile, int fileformat);
	~CAnimGif();

	int Play(int XPos, int YPos, bool Center);

	geBitmap *NextFrame(bool repeat);

	bool DisplayNextFrameTexture(const char *szTextureName, bool FFrame);

private:
	bool GetImage(bool repeat);

	bool TakeIt(void);

	void Output(BYTE bit);

	BYTE GetByte(void);

	WORD GetCode(void);

private:
	geVFile			*m_MainFS;
	BYTE			m_buffer[1024];
	bool			m_Active;
	bool			m_Texture;
	bool			m_FirstFrame;
	int				m_nWidth;
	int				m_nHeight;
	int				m_BackgroundColor;
	int				m_GlobalColorSize;
	BYTE			*m_GlobalColorTable;
	int				m_GifSize;
	BYTE			*m_pcGif;
	BYTE			*m_pcGifTrack;
	int				m_TotalReadByte;
	int				m_DisposalMethod;
	int				m_TransparentIndex;
	BOOL			m_bTransparentIndex;
	int				m_DelayTime;
	int				m_iWidth, m_iHeight;
	int				m_iTop, m_iLeft;
	int				m_iWidth1;
	BYTE			m_cCurentByte, m_cPackedField;
	UINT			m_uBitSize, m_uPrimaryBitSize;
	UINT			m_uRemain, m_uReadByte, m_uBlockSize;
	int				m_x, m_y;
	int				m_iPass, m_iRow;
	int				m_iMaxByte;
	geBitmap_Palette *m_Palette;
	BYTE			*m_pcBitmap;
	geBitmap		*m_Bmp;
	geBitmap		*m_LockedBMP;
	geBitmap_Info	m_Info;
	float			m_VAnimTime;
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
