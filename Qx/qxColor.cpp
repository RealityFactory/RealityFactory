////////////////////////////////////////////////////////////////////////////
//
// This file contains code CONFIDENTIAL to Chimerix Entertainment, Inc.
// and Quixotic, Inc.  Use or disclosure without permission is prohibited.
//
// Copyright 2000 Chimerix Entertainment, Inc.  All Rights Reserved.
// Copyright 2000 Quixotic, Inc.  All Rights Reserved.
//
//////////////////////////////////////////////////////////////////////////////
//
//        FILE: qxColor.cpp
//
// ENVIRONMENT: Vortex VR Engine
//
//      AUTHOR: Tim Bomgardner
//
//    TAB SIZE: 4
//
// DESCRIPTION: See Color.h
//
///////////////////////////////////////////////////////////////////////////
// Date      By   Description
// --------  ---  ---------------------------------------------------------
// 01/09/99  TAB  Initial Creation
// 02/10/99  TAB  Scale RGB correctly when saturation is 0
//
///////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <math.h>
#include "genesis.h"
#include "qxColor.h"

#ifndef QXASSERT
#include <assert.h>
#define QXASSERT assert
#endif

const float qxColor::UNDEFINED = -1.0;
///////////////////////////////////////////////////////////////////////////
//      METHOD: constructor
//              ===========
// DESCRIPTION: Construct the qxColor object.
///////////////////////////////////////////////////////////////////////////




qxColor::qxColor(InitType initType, float rh, float gs, float bv,
	float a)
{
	switch (initType)
	{
		case RGB:
			SetRGBA(rh, gs, bv, a);
			break;

		case HSV:
			SetHSVA(rh, gs, bv, a);
			break;

		default:
			QXASSERT(0);
			break;
	}
}

///////////////////////////////////////////////////////////////////////////
//      METHOD: ConvertHSVtoRGB
//              ===============
// DESCRIPTION: Convert the HSV values to RGB values.
///////////////////////////////////////////////////////////////////////////
void qxColor::ConvertHSVtoRGB()
{
	if (m_saturation == 0)
	{
		//
		// Hue is irrelevant when saturation is zero
		//
		rgba.r = m_value * 255.0f;
		rgba.g = m_value * 255.0f;
		rgba.b = m_value * 255.0f;
	}
	else
	{
		float h = (m_hue >= 360.0)? 0 : m_hue;  // 360 == 0
		h /= 60.0;  // hue is now 0-5
		int i = (int)floor(h);  // largest int <= h
		float f = h - i;  // fractional part of hue

		switch (i)
		{
			case 0:
				rgba.r   = m_value * 255.0f;
				rgba.g = t(f);
				rgba.b  = p(f);
				break;

			case 1:
				rgba.r   = q(f);
				rgba.g = m_value * 255.0f;
				rgba.b  = p(f);
				break;

			case 2:
				rgba.r   = p(f);
				rgba.g = m_value * 255.0f;
				rgba.b  = t(f);
				break;

			case 3:
				rgba.r   = p(f);
				rgba.g = q(f);
				rgba.b  = m_value * 255.0f;
				break;

			case 4:
				rgba.r   = t(f);
				rgba.g = p(f);
				rgba.b  = m_value * 255.0f;
				break;

			case 5:
				rgba.r   = m_value * 255.0f;
				rgba.g = p(f);
				rgba.b  = q(f);
				break;
		}
	}
}

///////////////////////////////////////////////////////////////////////////
//      METHOD: ConvertRGBtoHSV
//              ===============
// DESCRIPTION: Convert the RGB values to HSV values.
///////////////////////////////////////////////////////////////////////////
void qxColor::ConvertRGBtoHSV()
{
	float r = rgba.r / 255.0f;
	float g = rgba.g / 255.0f;
	float b = rgba.b / 255.0f;

	//
	// Compute value
	//
	float max = __max(__max(r, g), b);
	float min = __min(__min(r, g), b);
	m_value = max;

	//
	// Compute saturation
	//
	if (max == 0.0)
	{
		m_saturation = 0.0;
	}
	else
	{
		m_saturation = (max - min) / max;
	}

	//
	// Compute hue
	//
	if (m_saturation == 0.0)
	{
		m_hue = UNDEFINED;
	}
	else
	{
		float delta = max - min;
		if (r == max)
			m_hue = (g - b) / delta;
		else if (g == max)
			m_hue = 2 + (b - r) / delta;
		else if (b == max)
			m_hue = 4 + (r - g) / delta;
		m_hue *= 60.0;
		if (m_hue < 0.0) m_hue += 360.0f;
	}
}

///////////////////////////////////////////////////////////////////////////
void qxColor::SetH(float h)
{
	QXASSERT((h >= 0.0 && h <= 360.0) || h == UNDEFINED);
	m_hue = h;
	ConvertHSVtoRGB();
}

///////////////////////////////////////////////////////////////////////////
void qxColor::SetS(float s)
{
	QXASSERT(s >= 0.0 && s <= 1.0);
	m_saturation = s;
	ConvertHSVtoRGB();
}

///////////////////////////////////////////////////////////////////////////
void qxColor::SetV(float v)
{
	QXASSERT(v >= 0.0 && v <= 1.0);
	m_value = v;
	ConvertHSVtoRGB();
}



///////////////////////////////////////////////////////////////////////////
void qxColor::SetRGBA(float red, float green, float blue, float alpha)
{

	rgba.r = red;
	rgba.g = green;
	rgba.b = blue;
	rgba.a = alpha;
	ConvertRGBtoHSV();
}

///////////////////////////////////////////////////////////////////////////
void qxColor::SetHSVA(float h, float s, float v, float alpha)
{
	QXASSERT((h >= 0.0 && h <= 360.0) || h == UNDEFINED);
	QXASSERT(s >= 0.0 && s <= 1.0);
	QXASSERT(v >= 0.0 && v <= 1.0);
	QXASSERT((s == 0.0) || (s > 0.0 && h != UNDEFINED));
	QXASSERT(alpha >= 0.0 && alpha <= 255.0);
	m_hue = h;
	m_saturation = s;
	m_value = v;
	rgba.a = alpha;
	ConvertHSVtoRGB();
}

