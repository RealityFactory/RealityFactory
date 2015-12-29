// qxSun.cpp: implementation of the qxSun class.
//
//
// Author: Jeff Thelen
//
// Copyright 2000 Quixotic, Inc.  All Rights Reserved.
//////////////////////////////////////////////////////////////////////

#include "..\\RabidFramework.h"
#include "qxSun.h"
#include "qxColor.h"
#include "QxUser.h" //genesis
#include "qxSkyDome.h"

extern geBitmap *TPool_Bitmap(const char *DefaultBmp, const char *DefaultAlpha,
							  const char *BName, const char *AName);

//////////////////////////////////////////////////////////////////////
// qxSun
//////////////////////////////////////////////////////////////////////

qxSun::qxSun()
: qxEffectParticleChamber("qxSun", g_VoidNull)
, m_fPercentToZenith(-1.0f)
, m_bVisible(true)
, m_fAlpha(1.0f)
{
	//may slow system down
	m_nRenderStyle =
		GE_RENDER_DO_NOT_OCCLUDE_OTHERS|GE_RENDER_NO_FOG
		;
}


qxSun::~qxSun()
{
}


bool qxSun::Init()
{
	// The particle chamber handles only the flares for the sun

	geVec3d_Set(&m_vSunOrigin, 0, 0, (CCD->TerrainMgr()->GetLandscapeSize()*.5f) );
	m_fScale = CCD->TerrainMgr()->GetScaleXZ()*CCD->TerrainMgr()->GetScaleSun();

	Origin = m_vSunOrigin;

	ParticlesPerSec		= 4;
	ParticlesMax		= 4;

	AnglesRandom		= true;

	UnitLifeMax			= 4.0f;
	UnitLifeMin			= 1.00f;
	VarianceSource		= 0;
	VarianceDest		= 0;
	BmpName				= "terrain\\flare5.bmp";
	BmpAlphaName		= "terrain\\a_flare5.bmp";

	SetVisibilityFlags( EFFECTC_CLIP_SEMICIRCLE );

	SizeStart			= .5f   *	m_fScale;
	SizeEnd				= 1.00f *	m_fScale;
	SizeVariation		= .3f	*	m_fScale;

	AlphaStart			= 0.0f;
	AlphaEnd			= 64.0f;
	AlphaFadeInFadeOut	= true;

	qxColor start(255,255,255);
	ColorStart			= start.rgba;
	qxColor end(255,255,255);
	ColorEnd			= end.rgba;

	DistanceMax			= 0;// Always visible.
	DistanceMin			= 0;
	SpeedMax			= 4  *	m_fScale;
	SpeedMin			= 1  *	m_fScale;

	RandomGravity		= true;
	geVec3d_Set(&RandomGravityMin, -.5, -.5, -.5);
	geVec3d_Set(&RandomGravityMax, .5,.5, .5);

	if( !qxEffectParticleChamber::Init() )
		return false;

	if( !InitFlares() )
		return false;

	return true;
}


bool qxSun::InitFlares()
{
	m_ColorSun = CCD->TerrainMgr()->GetSuncolor();

	int i = 0;

	// Huge flare
	m_SunFlares[i].m_pBmp = TPool_Bitmap("terrain\\white256.bmp","terrain\\a_sunflare.bmp", NULL, NULL);
	m_SunFlares[i].m_fLengthFactor	= 1.0f;
	m_SunFlares[i].m_fScale			= 2.0f*m_fScale;
	m_SunFlares[i++].m_fAlpha		= 100.0f;

	// The Sun image
	m_SunFlares[i].m_pBmp = TPool_Bitmap("terrain\\flare6.bmp", "terrain\\a_flare6.bmp", NULL, NULL);
	m_SunFlares[i].m_fLengthFactor	= 1.0f;
	m_SunFlares[i].m_fScale			= 1.0f*m_fScale;
	m_SunFlares[i++].m_fAlpha		= 110.0f;

	//thin ring
	m_SunFlares[i].m_pBmp = TPool_Bitmap("terrain\\white128.bmp", "terrain\\a_flare3.bmp", NULL, NULL);
	m_SunFlares[i].m_fLengthFactor	= .60f;
	m_SunFlares[i].m_fScale			= .60f*m_fScale;
	m_SunFlares[i++].m_fAlpha		= 32.0f;

	//thick ring
	m_SunFlares[i].m_pBmp = TPool_Bitmap("terrain\\white128.bmp", "terrain\\a_flare4.bmp", NULL, NULL);
	m_SunFlares[i].m_fLengthFactor	= 0.40f;
	m_SunFlares[i].m_fScale			= .60f*m_fScale;
	m_SunFlares[i++].m_fAlpha		= 24.0f;

	//big thick fuzz
	m_SunFlares[i].m_pBmp = TPool_Bitmap("terrain\\white128.bmp", "terrain\\a_flare2.bmp", NULL, NULL);
	m_SunFlares[i].m_fLengthFactor	= -0.02f;
	m_SunFlares[i].m_fScale			= 0.50f*m_fScale;
	m_SunFlares[i++].m_fAlpha		= 24.0f;

	for( i = 0; i < NUM_FLARES; i++)
	{
		GE_LVertex v;
		memset(&v, 0, sizeof(GE_LVertex));
		v.r = m_ColorSun.r;
		v.g = m_ColorSun.g;
		v.b = m_ColorSun.b;

		m_SunFlares[i].m_pPoly =
		geWorld_AddPoly(CCD->Engine()->World(),
							&v,
							1,
							m_SunFlares[i].m_pBmp,
							GE_TEXTURED_POINT,
							GE_RENDER_DO_NOT_OCCLUDE_OTHERS | GE_RENDER_NO_FOG | GE_RENDER_NO_CLIP,
							m_SunFlares[i].m_fScale );
	}

	return true;
}


void qxSun::Draw()
{
}


int qxSun::Frame()
{
	geVec3d Translation = m_vSunOrigin;

	geXForm3d_Rotate(CCD->TerrainMgr()->GetEarthRotation(),
						&m_vSunOrigin, &Translation);

	//
	// Store the percent to the Zenith
	//
	geVec3d qSun = Translation;
	geVec3d_Normalize(&qSun);
	m_fPercentToZenith = qSun.Y;

	geVec3d thePosition;
	CCD->CameraManager()->GetPosition(&thePosition);
	const geVec3d* pCam = &thePosition;

	geVec3d dif;
	geVec3d_Add(pCam, &Translation, &dif);

	geVec3d vOldOrigin = Origin;
	SetOriginAndDest(&dif, NULL);

	geVec3d OriginDif;
	geVec3d_Subtract(&Origin, &vOldOrigin, &OriginDif);

	TranslateAllParticles(&OriginDif);


	if( m_bVisible && m_fPercentToZenith > -.05 )//hack
	{
		geVec3d ViewVect;
		GetViewVector(&ViewVect);
		geVec3d ViewVectNormalized = ViewVect;
		geVec3d_Normalize(&ViewVectNormalized);

		geVec3d_Scale(&ViewVect, 200.0f*m_fScale, &ViewVect);

		geVec3d CenterPos;
		geVec3d thePosition;
		CCD->CameraManager()->GetPosition(&thePosition);
		geVec3d_Add(&thePosition, &ViewVect, &CenterPos);

		geVec3d vLightVect;
		geVec3d_Subtract( &Origin, &CenterPos, &vLightVect );

		float length = geVec3d_Normalize(&vLightVect);

		float dot = geVec3d_DotProduct(&ViewVectNormalized, &vLightVect);

		if( dot > 0.0f)
		{
			geVec3d Pos;
			geVec3d Scale;

			for( int i = 0; i < NUM_FLARES; i++)
			{
				geVec3d_Scale(&vLightVect, m_SunFlares[i].m_fLengthFactor*length, &Scale);
				geVec3d_Add( &CenterPos, &Scale, &Pos);

				GE_LVertex v;
				gePoly_GetLVertex(m_SunFlares[i].m_pPoly, 0, &v);
				v.X = Pos.X;
				v.Y = Pos.Y;
				v.Z = Pos.Z;
				v.a = (m_SunFlares[i].m_fAlpha * dot) * m_fAlpha;

				gePoly_SetLVertex(m_SunFlares[i].m_pPoly, 0, &v);
				m_SunFlares[i].m_pPoly->Scale = m_SunFlares[i].m_fScale * dot;
			}
		}
	}//visible
	else
	{
		for( int i = 0; i < NUM_FLARES; i++)
		{
			GE_LVertex v;
			gePoly_GetLVertex(m_SunFlares[i].m_pPoly, 0, &v);
			v.a = 0.0f;
			gePoly_SetLVertex(m_SunFlares[i].m_pPoly, 0, &v);
		}
	}

	return 1;//qxEffectParticleChamber::Frame();
}


//
// qxSunFlare
//
qxSunFlare::qxSunFlare()
: m_pBmp(0)
, m_pPoly(0)
, m_fScale(1.0f)
, m_fAlpha(255.0f)
, m_fLengthFactor(0.0f)
{
}

qxSunFlare::~qxSunFlare()
{
	if(m_pPoly)
	{
		geWorld_RemovePoly(CCD->Engine()->World(), m_pPoly);
		m_pPoly = 0;
	}
}

