//////////////////////////////////////////////////////////////////////////////
//
// This file contains code CONFIDENTIAL to Chimerix Entertainment, Inc.
// and Quixotic, Inc.  Use or disclosure without permission is prohibited.
//
// Copyright 2000 Chimerix Entertainment, Inc.  All Rights Reserved.
// Copyright 2000 Quixotic, Inc.  All Rights Reserved.
//
//////////////////////////////////////////////////////////////////////////////
//
//        FILE: qxColor.h
//
// ENVIRONMENT: Vortex VR Engine
//
//      AUTHOR: Tim Bomgardner
//
//    TAB SIZE: 4
//
// DESCRIPTION: Manage a color in either HSV or RGB format.
//
//////////////////////////////////////////////////////////////////////////////
// Date      By   Description
// --------  ---  ------------------------------------------------------------
// 01/09/00  TAB  Creation
//
//////////////////////////////////////////////////////////////////////////////

#ifndef qxColor_h
#define qxColor_h

class qxColor
{
public:

	static const float UNDEFINED;
	enum InitType
	{
		RGB,
		HSV
	};

	qxColor::qxColor( qxColor& qx )
	{
		rgba.r = qx.rgba.r;
		rgba.g = qx.rgba.g;
		rgba.b = qx.rgba.b;
		rgba.a = qx.rgba.a;
		m_hue	= qx.m_hue;
		m_saturation = qx.m_saturation;
		m_value	= qx.m_value;
	}

	qxColor::qxColor( GE_RGBA* p )
	{
		rgba.r = p->r;
		rgba.g=p->g;
		rgba.b=p->b;
		rgba.a=p->a;
		ConvertRGBtoHSV();

	}

	qxColor::qxColor( GE_RGBA& p )
	{
		rgba.r	=	p.r;
		rgba.g	=	p.g;
		rgba.b	=	p.b;
		rgba.a	=	p.a;
		ConvertRGBtoHSV();
	}

	qxColor()
	{
		rgba.r	=	255.0f;
		rgba.g	=	255.0f;
		rgba.b	=	255.0f;
		rgba.a	=	255.0f;
		ConvertRGBtoHSV();
	}
	qxColor(float re, float gr, float bl, float al=255.0)
	{
		rgba.r	=	re;
		rgba.g	=	gr;
		rgba.b	=	bl;
		rgba.a	=	al;
		ConvertRGBtoHSV();
	}

	qxColor(InitType initType, float rh, float gs, float bv,
		float alpha = 255.0);
	~qxColor() { }


	inline float GetH() const { return m_hue; }
	inline float GetS() const { return m_saturation; }
	inline float GetV() const { return m_value; }


	void SetH(float h);
	void SetS(float s);
	void SetV(float v);

	void SetRGBA(GE_RGBA& c) { SetRGBA(c.r, c.g, c.b, c.a); }
	void SetRGBA(float r, float g, float b, float a = 255.0);
	void SetHSVA(float h, float s, float v, float a = 255.0);

	void ConvertHSVtoRGB();
	void ConvertRGBtoHSV();


	const qxColor& operator=(const qxColor& qx)
	{
		rgba.r = qx.rgba.r;
		rgba.g = qx.rgba.g;
		rgba.b = qx.rgba.b;
		rgba.a = qx.rgba.a;
		m_hue	= qx.m_hue;
		m_saturation = qx.m_saturation;
		m_value	= qx.m_value;
		return *this;
	};

	operator GE_RGBA*() const
	{
		return (GE_RGBA*) this;
	};

	GE_RGBA rgba;

	void White() { SetRGBA( 255, 255, 255 ); }
	void Black() { SetRGBA( 0, 0, 0 ); }
	void SetRGB( float f ) { SetRGBA( f, f, f ); }

protected:

	inline float p(float /*f*/) const
		{ return (float)(m_value * (1.0 - m_saturation) * 255.0); }
	inline float q(float f) const
		{ return (float)(m_value * (1.0 - (m_saturation * f)) * 255.0); }
	inline float t(float f) const
		{ return (float)(m_value * (1.0 - (m_saturation * (1.0 - f))) * 255.0); }

private:


	float m_hue;
	float m_saturation;
	float m_value;

};


// Some common color defines
static GE_RGBA COLOR_WHITE ={ 255, 255, 255, 255 };
static GE_RGBA COLOR_BLACK ={ 0,0,0, 255 };
static qxColor COLOR_NIGHT_BLUE(0,0,64,255);
static qxColor COLOR_DAY_GRAY(200, 200, 200,255);
static qxColor COLOR_DAY_LIGHT_BLUE(150, 200, 255, 255);
static qxColor qxColorWhite( COLOR_WHITE );
static qxColor qxColorBlack( COLOR_BLACK );


#endif
