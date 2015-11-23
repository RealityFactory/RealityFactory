// qxMoon_H.h: 
//
// Author: Jeff Thelen
//
// Copyright 2000 Quixotic, Inc.  All Rights Reserved.
//////////////////////////////////////////////////////////////////////

#ifndef qxMoon_H
#define qxMoon_H


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000




enum eMOON_PHASE
{
	MOON_PHASE_NEW,
	MOON_PHASE_QUARTER_WAX,
	MOON_PHASE_FULL,
	MOON_PHASE_QUARTER_WANE,
	MOON_PHASE_OLD
};

#include "..\\RabidFramework.h"


class qxMoon 
{
public:

	qxMoon();

	~qxMoon();

	bool	Init();
	
	int		Frame();

	//
	// Attributes
	//

	GE_RGBA		m_ColorMoon;
	geVec3d		m_vMoonOrigin;

protected:


	geBitmap*	m_pBmp;
	gePoly*		m_pPoly;
	float		m_fScale;
	float		m_fAlpha;
	eMOON_PHASE	m_eMoonPhase;

	
};




#endif