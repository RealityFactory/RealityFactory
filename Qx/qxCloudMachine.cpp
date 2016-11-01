// qxCloudMachine.cpp: implementation of the qxCloudMachine class.
//
//
// Author: Jeff Thelen
//
// Copyright 2000 Quixotic, Inc.  All Rights Reserved.
//////////////////////////////////////////////////////////////////////

#include "..\\RabidFramework.h"
#include "qxTerrainMgr.h"
#include "qxCloudMachine.h"
#include "qxColor.h"
#include "qxParticle.h"
#include "QxUser.h"	//Genesis
#include "qxSkyDome.h"
#include "qxSun.h"


//////////////////////////////////////////////////////////////////////
// qxCloudMachine
//////////////////////////////////////////////////////////////////////
const float TIME_TO_MOVE_ORIGIN = 1.0f;
const float TIME_TO_KILL_PARTICLES = 15.00f;

qxCloudMachine::qxCloudMachine()
:qxEffectParticleChamber("qxCloudMachine", g_VoidNull)
,m_nHeightMin( 2000 )
,m_nHeightMax( 5000 )
,m_fTimeToKillParticles( TIME_TO_KILL_PARTICLES )
,m_fTimeToMoveOrigin( TIME_TO_MOVE_ORIGIN )
,m_eWindDir( DIRECTION_S )
{
	//prevents flicker, but may slow system down
	m_nRenderStyle = GE_RENDER_DO_NOT_OCCLUDE_OTHERS | GE_RENDER_DEPTH_SORT_BF | GE_RENDER_NO_CLIP | GE_RENDER_NO_FOG;
	m_nHeightMin		= (int)CCD->TerrainManager()->GetSkyDome()->GetMaxSkyHeight();
	m_nHeightMax		= (int)CCD->TerrainManager()->GetSkyDome()->GetMaxSkyHeight()*2;
}


qxCloudMachine::~qxCloudMachine()
{

}


bool qxCloudMachine::Init()
{
	geVec3d_Clear(&Origin);
	Origin.Y = Frand((float)m_nHeightMin,(float)m_nHeightMax);
	float Size = (float)CCD->TerrainManager()->GetLandscapeSize()/2.0f;
	Origin.X = Frand(-Size, Size);
	Origin.Z = Frand(-Size, Size);

	ParticlesPerSec		= 1;
	ParticlesMax		= 40;
	AnglesRandom		= false;

	UnitLifeMax			= 30.0f;
	UnitLifeMin			= 15.00f;
	VarianceSource		= 0;
	VarianceDest		= 0;
	BmpName				= "terrain\\white256.bmp";
	BmpAlphaName		= "terrain\\a_clouds4.bmp";

	SetVisibilityFlags( 0 );

	SizeStart			= 10;
	SizeEnd				= 20;
	SizeVariation		= 5;

	AlphaStart			= 0.0f;
	AlphaEnd			= 255.0f;
	AlphaFadeInFadeOut	= true;

	qxColor start;
	start.SetRGB(255);

	ColorStart			= start.rgba;
	qxColor end;
	end.SetRGB(220);

	ColorEnd			= end.rgba;

	DistanceMax			= 0;// Always visible.
	DistanceMin			= 0;
	SpeedMax			= 300;
	SpeedMin			= 100;

	geVec3d_Set(&QuadRotateDegreesMin,0, 0 , -1);
	geVec3d_Set(&QuadRotateDegreesMax,0, 0 , 1 );

	// This is a quad that lays flat
	// Slight tilt on the Z axis for variety
	geVec3d_Set(&QuadRandInitialRotateDegreesMax, 90, 359 , 0);
	geVec3d_Set(&QuadRandInitialRotateDegreesMin, 90, 0 , 0);
	Quad				= true;

	if(!qxEffectParticleChamber::Init())
		return false;

	SetAngles();

	return true;
}


void qxCloudMachine::Draw()
{

}


int qxCloudMachine::Frame()
{
	SetAngles();

	qxEffectParticleChamber::Frame();

	float fTime = CCD->LastElapsedTime_F()*0.001f;

	// Kill invisible particles every now and then
	m_fTimeToKillParticles-= fTime;

	if(	m_fTimeToKillParticles < 0.0f )
	{
		m_fTimeToKillParticles = TIME_TO_KILL_PARTICLES;
		KillInvisibleParticles( CCD->Engine()->GetFogEnd()*2.0f );
	}

	UpdateColors();

	// Shift the origin around every now and then
	m_fTimeToMoveOrigin -= fTime;

	if(	m_fTimeToMoveOrigin < 0.0f )
	{
		m_fTimeToMoveOrigin = TIME_TO_MOVE_ORIGIN;


		//Get the far limits of the fog.
		//float fStart = CCD->Engine()->GetFogEnd()*2.0f;
		float fStart = (float)CCD->TerrainManager()->GetLandscapeSize()/2.0f;

		geVec3d thePosition;
		CCD->CameraManager()->GetPosition(&thePosition);
		const geVec3d* pCam = &thePosition;

		geVec3d v;

		v.X	= Frand( -fStart, fStart );
		v.X	+= 	pCam->X;
		v.Z	= Frand( -fStart, fStart );
		v.Z	+= pCam->Z;

		v.Y	= Frand((float)m_nHeightMin,(float)m_nHeightMax);

		SetOriginAndDest(&v, NULL);
	}

	return 1;
}


void qxCloudMachine::DoCloudless()
{
	ParticlesPerSec		= 4.0f;
	ParticlesMax		= 4;

	UnitLifeMax			= 480.0f;
	UnitLifeMin			= 240.00f;

	BmpName				= "terrain\\white256.bmp";
	BmpAlphaName		= "terrain\\a_clouds4.bmp";

	SizeStart			= 4;
	SizeEnd				= 10;
	SizeVariation		= 2;

	AlphaStart			= 64.0f;
	AlphaEnd			= 255.0f;

	qxColor start;
	start.SetRGB(255);
	ColorStart			= start.rgba;

	qxColor end;
	end.SetRGB(220);
	ColorEnd			= end.rgba;

	SpeedMax			= 30;
	SpeedMin			= 2;

	m_nHeightMin		= (int)CCD->TerrainManager()->GetSkyDome()->GetMaxSkyHeight();
	m_nHeightMax		= (int)CCD->TerrainManager()->GetSkyDome()->GetMaxSkyHeight()*2;

	qxEffectParticleChamber::ReInit();

	SetAngles();
}


void qxCloudMachine::SetNightBlueColor()
{
	qxColor start;
	start.SetRGBA(90,128,255);
	ColorStart			= start.rgba;

	qxColor end;
	end.SetRGBA(90,128,160);
	ColorEnd			= end.rgba;

	qxEffectParticleChamber::ReInit();
}


void qxCloudMachine::DoWhiteFluffy()
{
	ParticlesPerSec		= 2;
	ParticlesMax		= 10;

	UnitLifeMax			= 240.0f;
	UnitLifeMin			= 60.00f;

	BmpName				= "terrain\\clouds_256.bmp";
	BmpAlphaName		= "terrain\\a_clouds4.bmp";

	SizeStart			= 2;
	SizeEnd				= 10;
	SizeVariation		= 1;

	AlphaStart			= 0.0f;
	AlphaEnd			= 255.0f;

	qxColor start;
	start.SetRGB(255);
	ColorStart			= start.rgba;

	qxColor end;
	end.SetRGB(220);
	ColorEnd			= end.rgba;

	SpeedMax			= 10;
	SpeedMin			= 2;

	m_nHeightMin		= (int)CCD->TerrainManager()->GetSkyDome()->GetMaxSkyHeight();
	m_nHeightMax		= (int)CCD->TerrainManager()->GetSkyDome()->GetMaxSkyHeight()*2;

	qxEffectParticleChamber::ReInit();

	SetAngles();
}


void qxCloudMachine::DoGreyOvercast()
{
	ParticlesPerSec		= 1;
	ParticlesMax		= 20;

	UnitLifeMax			= 120.0f;
	UnitLifeMin			= 30.00f;

	BmpName				= "terrain\\white256.bmp";
	BmpAlphaName		= "terrain\\a_clouds4.bmp";

	SizeStart			= 40;
	SizeEnd				= 100;
	SizeVariation		= 5;

	AlphaStart			= 0.0f;
	AlphaEnd			= 200.0f;

	qxColor start;
	start.SetRGB(120);
	ColorStart			= start.rgba;

	qxColor end;
	end.SetRGB(64);
	ColorEnd			= end.rgba;

	SpeedMax			= 50;
	SpeedMin			= 20;

	m_nHeightMin		= (int)CCD->TerrainManager()->GetSkyDome()->GetMaxSkyHeight();
	m_nHeightMax		= (int)CCD->TerrainManager()->GetSkyDome()->GetMaxSkyHeight()*2;

	qxEffectParticleChamber::ReInit();

	SetAngles();
}


//
// Helper to kill distant invisible particles
//
void qxCloudMachine::KillInvisibleParticles( float fDistThreshold )
{
	m_nParticlesAlive = 0;

	// update all particles that we own
	for ( int i=0; i < ParticlesMax; i++ )
	{
		if ( m_pParticles[i]->m_fAge < 0.0f )
			continue;

		geVec3d p = *((geVec3d*)&(m_pParticles[i]->m_vVertex.X));

		//Ignore the Y
		geVec3d thePosition;
		CCD->CameraManager()->GetPosition(&thePosition);
		p.Y = thePosition.Y;

		float fDistSq = geVec3d_DistanceBetweenSquared(&p, &thePosition);

		if ( fDistSq >  fDistThreshold*fDistThreshold )
		{
			if(m_pParticles[i]->m_vVertex.a <= AlphaStart)
			{
				m_pParticles[i]->Die();
				continue;
			}
		}

		m_nParticlesAlive++;
	}
}


void qxCloudMachine::SetAngles()
{
	m_eWindDir = CCD->TerrainManager()->GetWindDir();

	switch(m_eWindDir)
	{
	case DIRECTION_N:	qxEffectParticleChamber::SetAngles(&(ANGLE_N)); break;
	case DIRECTION_NE:	qxEffectParticleChamber::SetAngles(&(ANGLE_NE)); break;
	case DIRECTION_E:	qxEffectParticleChamber::SetAngles(&(ANGLE_E)); break;
	case DIRECTION_SE:	qxEffectParticleChamber::SetAngles(&(ANGLE_SE)); break;
	case DIRECTION_S:	qxEffectParticleChamber::SetAngles(&(ANGLE_S)); break;
	case DIRECTION_SW:	qxEffectParticleChamber::SetAngles(&(ANGLE_SW)); break;
	case DIRECTION_W:	qxEffectParticleChamber::SetAngles(&(ANGLE_W)); break;
	case DIRECTION_NW:	qxEffectParticleChamber::SetAngles(&(ANGLE_NW)); break;
	default: QXASSERT(0); break;
	}
}


void qxCloudMachine::UpdateColors()
{
	// Find the sun or moon
	// and brighten the triangles with alpha
	static const qxSun* pSun = CCD->TerrainManager()->GetSun();

	if(!pSun)
		return;

	qxColor SunColor( *(CCD->TerrainManager()->GetSkyDome()->GetCurrentSunColor()) );

	for ( int i=0; i < ParticlesMax; i++ )
	{
		if ( m_pParticles[i]->m_fAge < 0.0f )
			continue;

		GE_LVertex* pVect = &m_pParticles[i]->m_vVertex;

		float	fSunIntensity = CCD->TerrainManager()->GetSkyDome()->GetSunIntensity();

		pVect->r = SunColor.rgba.r * fSunIntensity ;
		pVect->g = SunColor.rgba.g * fSunIntensity;
		pVect->b = SunColor.rgba.b * fSunIntensity;
		m_pParticles[i]->UpdatePoly();
	}
}
