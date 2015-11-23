// qxMoon.cpp: implementation of the qxMoon class.
//
//
// Author: Jeff Thelen
//
// Copyright 2000 Quixotic, Inc.  All Rights Reserved.
//////////////////////////////////////////////////////////////////////

#include "..\\RabidFramework.h"
#include "qxMoon.h"
#include "qxColor.h"
#include "QxUser.h" //genesis

extern geBitmap *TPool_Bitmap(char *DefaultBmp, char *DefaultAlpha, char *BName, char *AName);

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// qxMoon
//////////////////////////////////////////////////////////////////////

qxMoon::qxMoon()
:m_pBmp(0)
,m_pPoly(0)
,m_eMoonPhase(MOON_PHASE_FULL)
{

}

qxMoon::~qxMoon()
{
	if(m_pPoly)
	{
		geWorld_RemovePoly(CCD->Engine()->World(), m_pPoly);
		m_pPoly = 0;
	}

}


bool qxMoon::Init()
{
	qxColor c;//(0,128,255);
	m_ColorMoon = c.rgba;

	// This places the moon opposite the sun (at zenith at midnight)
	geVec3d_Set(&m_vMoonOrigin, 0.0f, 0.0f, (float)-(CCD->TerrainMgr()->GetLandscapeSize()) );

	float Scale = (CCD->TerrainMgr()->GetLandscapeSize())/1000.0f;

	m_eMoonPhase = MOON_PHASE_QUARTER_WAX;//temp
	switch (m_eMoonPhase)
	{
		case MOON_PHASE_FULL:
		{
			
			m_pBmp = TPool_Bitmap("terrain\\moonFull.Bmp", "terrain\\a_moonFull.Bmp", NULL, NULL);
			m_fScale			= .5f*Scale;
			m_fAlpha			= 255.0f;
			break;
			
		}
	
		case MOON_PHASE_QUARTER_WAX:
		{
			
			m_pBmp = TPool_Bitmap("terrain\\moonFull.Bmp", "terrain\\a_moonq1.Bmp", NULL, NULL);
			m_fScale			= .5f*Scale;
			m_fAlpha			= 255.0f;
			break;
			
		}
		default: 
			QXASSERT(0);
			break;
	}

	GE_LVertex v;
	memset(&v, 0, sizeof(GE_LVertex));
	v.r = m_ColorMoon.r;
	v.g = m_ColorMoon.g;
	v.b = m_ColorMoon.b;

	m_pPoly = geWorld_AddPoly(CCD->Engine()->World(),
								&v,
								1,
								m_pBmp,
								GE_TEXTURED_POINT,
								GE_RENDER_DO_NOT_OCCLUDE_OTHERS|GE_RENDER_NO_FOG|GE_RENDER_NO_CLIP,
								m_fScale );


	return true;
}


int qxMoon::Frame()
{
	geVec3d thePosition;
	CCD->CameraManager()->GetPosition(&thePosition);
	const geVec3d* pCam = &thePosition;
	

	GE_LVertex v;
	gePoly_GetLVertex( m_pPoly, 0, &v );
	geVec3d Translation;
	
	geXForm3d_Rotate(CCD->TerrainMgr()->GetEarthRotation(),
						&m_vMoonOrigin, &Translation);
	
	geVec3d_Add(pCam, &Translation, (geVec3d*)&v.X);

	v.a = m_fAlpha;

	gePoly_SetLVertex( m_pPoly, 0, &v );


	return 1;
}

