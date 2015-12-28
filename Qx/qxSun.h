// qxSun.h:
//
// Author: Jeff Thelen
//
// Copyright 2000 Quixotic, Inc.  All Rights Reserved.
//////////////////////////////////////////////////////////////////////

#ifndef qxSun_H
#define qxSun_H


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define NUM_FLARES 5

#include "qxEffectParticleChamber.h"

class qxSunFlare
{
public:
	qxSunFlare();
	~qxSunFlare();

	geBitmap*	m_pBmp;
	gePoly*		m_pPoly;
	float		m_fScale;
	float		m_fAlpha;
	float		m_fLengthFactor;
};


class qxSun : public qxEffectParticleChamber
{
public:

	qxSun();
	~qxSun();

	bool	Init();

	int		Frame();

	void	Draw();

	//
	// Attributes
	//

	geVec3d		m_vSunOrigin;

	float		GetPercentToZenith()	{ return m_fPercentToZenith; }
	void		SetVisible(bool b)		{ m_bVisible = b; }
	void		SetAlpha(float f)		{ m_fAlpha = f; }

protected:

	GE_RGBA		m_ColorSun;

	bool		InitFlares();
	qxSunFlare	m_SunFlares[NUM_FLARES];
	float		m_fScale;
	float		m_fPercentToZenith;

	bool		m_bVisible;

	// Global alpha. Good for dimming sun during cloudy/foggy days.
	float		m_fAlpha;
};


#endif
