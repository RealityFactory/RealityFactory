// qxStarField.cpp: implementation of the qxStarField class.
//
//
// Author: Jeff Thelen
//
// Copyright 2000 Quixotic, Inc.  All Rights Reserved.
//////////////////////////////////////////////////////////////////////

#include "..\\RabidFramework.h"
#include "qxStars.h"
#include "qxColor.h"
#include "QxUser.h" //genesis

extern geBitmap *TPool_Bitmap(char *DefaultBmp, char *DefaultAlpha, char *BName, char *AName);

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// qxStarField
//////////////////////////////////////////////////////////////////////

qxStarField::qxStarField()
:
m_pStarSparkle(0)
{
	geXForm3d_SetIdentity(&m_matLocal);
}

qxStarField::~qxStarField()
{

}


bool qxStarField::Init()
{
	if( !InitStarsMajor())
		return false;

	return true;
}

bool qxStarField::InitStarsMajor()
{

	float Scale = (CCD->TerrainMgr()->GetLandscapeSize())/1000.0f;

	// Random stars. We should in the end use actual star coordinates for 
	// reality's sake
	geBitmap* pBmp1 = TPool_Bitmap("terrain\\star1.bmp", "terrain\\a_star2.bmp", NULL, NULL);
	geBitmap* pBmp2 = TPool_Bitmap("terrain\\star2.bmp", "terrain\\a_star2.bmp", NULL, NULL);
	geBitmap* pBmp3 = TPool_Bitmap("terrain\\white64.bmp", "terrain\\a_starfield64.bmp", NULL, NULL);
	geBitmap* pBmp4 = TPool_Bitmap("terrain\\white128.bmp", "terrain\\a_starfield128.bmp", NULL, NULL);

	geBitmap* pCurBmp = pBmp1;

	int CurBmp = 0;

	for(int i = 0; i < NUM_STARS; i++)
	{
		geVec3d RotateRads;
		RotateRads.X = Frand(	RADIANS(-180), RADIANS(180) );
		RotateRads.Y = Frand(	RADIANS(-180), RADIANS(180) );
		RotateRads.Z = Frand(	RADIANS(-180), RADIANS(180) );
		
		
		geXForm3d matRotate;
		geXForm3d_SetIdentity(&matRotate);
		
		
		geXForm3d_RotateX( &matRotate, RotateRads.X );
		geXForm3d_RotateY(&matRotate, RotateRads.Y );
		geXForm3d_RotateZ(&matRotate, RotateRads.Z );
		
		geVec3d In, Pos;
		geXForm3d_GetIn(&matRotate, &In);	
		
		
		geVec3d_Scale(&In, 2000*Scale, &In);
		
		geVec3d_Clear(&Pos);

	
		geVec3d_Add( &Pos, &In, &m_Stars[i].m_vOrigin);

			
		m_Stars[i].m_fScale		= Frand(.25f*Scale, 1.00f*Scale);
		m_Stars[i].m_Color.a	= Frand(64, 255);
		
		switch( CurBmp )
		{
		case 0: CurBmp=1; pCurBmp = pBmp1; break;
		case 1: CurBmp=2; pCurBmp = pBmp2; break;
		case 2: CurBmp=3; pCurBmp = pBmp3; break;
		case 3: CurBmp=0; pCurBmp = pBmp4; break;
		
		}
	

		
		
		GetRandomStarColor( &m_Stars[i].m_Color );
		
		GE_LVertex v;
		memset(&v, 0, sizeof(v));
		v.a =	m_Stars[i].m_Color.a;
		v.r =	m_Stars[i].m_Color.r;
		v.g =	m_Stars[i].m_Color.g;
		v.b =	m_Stars[i].m_Color.b;
		geVec3d_Copy(&m_Stars[i].m_vOrigin, (geVec3d*) &v.X);

		m_Stars[i].m_pPoly = 
			geWorld_AddPoly(CCD->Engine()->World(),
			&v,
			1,
			pCurBmp,
			GE_TEXTURED_POINT,
			GE_RENDER_DO_NOT_OCCLUDE_OTHERS|GE_RENDER_NO_FOG|GE_RENDER_NO_CLIP,
			m_Stars[i].m_fScale	 );
	}

	return true;
}


int qxStarField::Frame()
{


	// -1.0 midnight, 1.0 noon
	float fSunPercentToZenith = CCD->TerrainMgr()->GetSunPercentToZenith();

	float fSunupAlphaAdjust = 1.0f;

	// See how far above horizon the sun is. If (arbitrarily)
	// above 0, then no stars to be seen
	// We should just remove the stars, instead of setting their alpha here
	if( fSunPercentToZenith >= 0.0f )
	{
		fSunupAlphaAdjust = 0.0f;
	}
	// Somewhere in twilight
	else if ( fSunPercentToZenith >= - CCD->TerrainMgr()->GetTwilightDistanceFromHorizon() )
	{
		fSunupAlphaAdjust = -fSunPercentToZenith / CCD->TerrainMgr()->GetTwilightDistanceFromHorizon();
	}

	geVec3d thePosition;
	CCD->CameraManager()->GetPosition(&thePosition);
	const geVec3d* pCam = &thePosition;

	for(int i = 0; i < NUM_STARS; i++)
	{
		geVec3d Translation = m_Stars[i].m_vOrigin;
		
		GE_LVertex v;
		gePoly_GetLVertex( m_Stars[i].m_pPoly, 0, &v );
		
		geXForm3d_Rotate(CCD->TerrainMgr()->GetEarthRotation(),
			&m_Stars[i].m_vOrigin, &Translation);
		
		geVec3d_Add(pCam, &Translation, (geVec3d*)&v.X);

		v.a = fSunupAlphaAdjust * m_Stars[i].m_Color.a;

		gePoly_SetLVertex( m_Stars[i].m_pPoly, 0, &v );
	}
	
		
	if( fSunupAlphaAdjust < 1.0f )
		return 1;

	// sparkle for a random star
	if( !m_pStarSparkle )
	{
		int idx = rand()%NUM_STARS;
		m_pStarSparkle = &m_Stars[idx];
		m_fStarSparkleAlpha = 0.0f;
		m_fStarSparkleDir = 1500.0f;
		m_fStarSparkleTime = Frand(.025f, .75f);

		GE_LVertex v;
		gePoly_GetLVertex( m_pStarSparkle->m_pPoly, 0, &v );
		v.a = m_fStarSparkleAlpha;
		gePoly_SetLVertex( m_pStarSparkle->m_pPoly, 0, &v );
	}
	else
	{
				
		float fTime  = CCD->LastElapsedTime_F()*0.001f;
		m_fStarSparkleTime -= fTime;

		GE_LVertex v;
		gePoly_GetLVertex( m_pStarSparkle->m_pPoly, 0, &v );
		
		m_fStarSparkleAlpha += m_fStarSparkleDir * fTime;

		// end of sparkle time
		if( m_fStarSparkleTime <= 0.0)
		{
			v.a = m_pStarSparkle->m_Color.a;
			gePoly_SetLVertex( m_pStarSparkle->m_pPoly, 0, &v );
			m_pStarSparkle = 0;
		}
		else
		{
			if( m_fStarSparkleAlpha > 255.0)
			{
				m_fStarSparkleAlpha = 255.0;
				m_fStarSparkleDir = -m_fStarSparkleDir;
			}
			else if( m_fStarSparkleAlpha < m_pStarSparkle->m_Color.a*.5f)
			{
				m_fStarSparkleAlpha = m_pStarSparkle->m_Color.a*.5f;
				m_fStarSparkleDir = -m_fStarSparkleDir;
			}
			
			v.a = m_fStarSparkleAlpha;
			
			gePoly_SetLVertex( m_pStarSparkle->m_pPoly, 0, &v );
		}
	}

	return 1;

}


void qxStarField::GetRandomStarColor(GE_RGBA* p)
{

	int Color = rand()%16;

	switch( Color )
	{
		//YELLOW-ORANGE-RED
	case 0:
		{
			p->r = 255;
			p->b = 0;
			p->g = Frand(0, 255);
			break;
		}
		//BLUE
	case 1:
		{
			p->r = 0;
			p->b = 255;
			p->g = Frand(0, 255);
			break;
		}
		// VIOLET-WHITE
	case 2:
		{
			p->r = 255;
			p->b = 255;
			p->g = Frand(128, 255);
			break;
		}

	// BLUE-WHITE
	case 3:
		{
			p->r = Frand(0, 255);
			p->b = 255;
			p->g = 255;
			break;
		}
	// WHITE
	default:
		{
			p->r = 255;
			p->b = 255;
			p->g = 255;
			break;
		}
	}
}


//
// qxStar
//

qxStar::qxStar()
: m_pPoly(0)
, m_fScale(1.0f)
{
	ColorInit(&m_Color);
	geVec3d_Clear(&m_vOrigin);

}

qxStar::~qxStar()
{

	if(m_pPoly)
	{
		geWorld_RemovePoly(CCD->Engine()->World(), m_pPoly);
		m_pPoly = 0;
	}

}
