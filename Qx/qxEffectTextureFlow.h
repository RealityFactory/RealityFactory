// EffectTextureFlow.h: interface for the qxEffectTextureFlow class.
//
// Author: Jeff Thelen
//
// Copyright 1999 Quixotic, Inc.  All Rights Reserved.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EFFECTTEXTUREFLOW_H__C1C2F0C1_685A_11D3_BE84_100B45C100D5__INCLUDED_)
#define AFX_EFFECTTEXTUREFLOW_H__C1C2F0C1_685A_11D3_BE84_100B45C100D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\\RabidFramework.h"
#include "qxEffectBase.h"

typedef struct ColorRGBA
{
	int R[256];
	int G[256];
	int B[256];
	int	A[256];

} ColorRGBA;


class qxEffectTextureFlow : public qxEffectBase  
{

public:
	
	qxEffectTextureFlow(char* strName, void*& qxFromEditor);
	~qxEffectTextureFlow();
	virtual bool Init();
	virtual int Frame();
	static const char* GetQXTypeName() { return "qxEffectTextureFlow"; }

	
protected:

	geBitmap*		m_pOriginalBmpCopy;	// copy of the original bitmap
	float			m_fCurX;			// current offsets
	float			m_fCurY;			// current offsets
	float			m_fXOffset;			// offset amounts
	float			m_fYOffset;			// offset amounts
	bool			m_bMipsChecked;		// whether or not preprocessing of mips has occured
	gePixelFormat	m_PixelFormat;		// our prefered pixel format
	uint8			m_Table[256][256];	// color lookup table
	
	uint8 FindBestColorMatch( ColorRGBA	*ColorTable,	// color table to use
							  int			R,				// desired red
							  int			G,				// desired green
							  int			B,				// desired blue
							  int			A );			// desired alpha

	bool CreateColorTable();
	

public:
	char		*BmpName;
	char		*AlphaName;
	float		OffsetX;
	float		OffsetY;
};

#endif // !defined(AFX_EFFECTTEXTUREFLOW_H__C1C2F0C1_685A_11D3_BE84_100B45C100D5__INCLUDED_)
