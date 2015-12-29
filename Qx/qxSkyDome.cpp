/****************************************************************************************/
/*	qxSkyDome.cpp: implementation of the qxSkyDome class.								*/
/*																						*/
/*	Original code: GenScape (c) 1999, Lucas Ackerman									*/
/*																						*/
/*	Author: Jeff Thelen																	*/
/*																						*/
/*	Copyright 2000 Quixotic, Inc.  All Rights Reserved.									*/
/*																						*/
/*	Edit History:																		*/
/*	=============																		*/
/*	02/01/07 QD:	- added skydome Y offset											*/
/*																						*/
/****************************************************************************************/

#include "..\\RabidFramework.h"
#include "qxTerrainMgr.h"
#include "qxTerrainMapBase.h"
#include "qxTerrainMap.h"
#include "qxColor.h"
#include "qxSkyDome.h"
#include "qxTerrainVert.h"
#include "qxTerrainTile.h"
#include "QxUser.h"
#include "qxEffectTextureFlow.h"
#include "qxSun.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

qxSkyDome::qxSkyDome(	qxTerrainDefinition& TerrainDef, int HeightMapWidthLength,
						float fMaxHeight,
						float fScaleXZ)

:	qxTerrainMap(TerrainDef)
,	m_fScale(1.0)
,	m_bMoveWithCamera(false)
,	m_fScaleXZ(fScaleXZ)
,	m_bTimeToUpdateSkyColor(true)
,	m_fDistanceFromSunFactor(1.0)
,	m_fMinBlueSkyColor(16.0f)
{

	m_fMaxHeight = fMaxHeight * CCD->TerrainMgr()->GetScaleXZ();

	m_nHeightMapWidth	=  HeightMapWidthLength;
	m_nHeightMapLength	= HeightMapWidthLength;
	m_bRenderBackFace	= true;
	fTimer = 0.0f;
	pTextureFlow = 0;
}

qxSkyDome::~qxSkyDome()
{
	delete pTextureFlow;
}


bool qxSkyDome::Init()
{
	m_nTEXTURESIZE = 64;
	if( !qxTerrainMap::Init() )
		return false;

	// we want to see all the polys please
	m_fNormalDistanceToCamera = 999999.0f;


	//
	// GE_RENDER_DEPTH_SORT_BF (no z write) allows celestial objects to shine
	// through the atmosphere. However, the impact in addition to
	// this artifact is unknown.
	//
	m_nRenderFlags	=
		GE_RENDER_CLAMP_UV
		| GE_RENDER_DO_NOT_OCCLUDE_OTHERS // No z writing will be performed
		| GE_RENDER_DEPTH_SORT_BF;

	geEntity_EntitySet* lEntitySet;
	geEntity* lEntity;

	lEntitySet = geWorld_GetEntitySet(CCD->World(), "EnvironmentSetup");

	if(lEntitySet != NULL)
	{
		lEntity = geEntity_EntitySetGetNextEntity(lEntitySet, NULL);
		if(lEntity)
		{
			EnvironmentSetup *theState = (EnvironmentSetup*)geEntity_GetUserData(lEntity);
			if(theState->EnableDistanceFog == GE_TRUE)
				m_nRenderFlags |= GE_RENDER_NO_FOG ;
		}
	}

	CenterOnCamera();

	if(CCD->TerrainMgr()->GetTextureFlow())
	{
/*		pTextureFlow = new qxEffectTextureFlow("test", g_VoidNull);
		Type = AttrFile->GetValue(KeyName, "skytextureflowbmp");
		if(Type=="")
			pTextureFlow->BmpName = strdup("terrain\\clouds_128.bmp");
		else
			pTextureFlow->BmpName = strdup(Type);

		pTextureFlow->OffsetX = (float)AttrFile->GetValueF(KeyName, "skytextureflowoffsetx");
		pTextureFlow->OffsetY = (float)AttrFile->GetValueF(KeyName, "skytextureflowoffsety");

		pTextureFlow->Init(); */
	}

	GE_RGBA rgba = CCD->TerrainMgr()->Getrgba();
	SetCurrentVertColor(&rgba);

	m_fMinBlueSkyColor = CCD->TerrainMgr()->GetMinBlueSkyColor();
	m_fDistanceFromSunFactor = CCD->TerrainMgr()->GetDistanceFromSunFactor();
	m_fDistanceFromSunFactor *= CCD->TerrainMgr()->GetScaleXZ();
	GE_RGBA color = CCD->TerrainMgr()->Getcolor();;

	m_TwilightColor.SetRGBA(color.r, color.g, color.b);

	ScaleXZ();

	m_fColorUpdateTime = CCD->TerrainMgr()->GetColorUpdateTime();

	return true;
}


void qxSkyDome::ScaleXZ()
{
	if( m_fScaleXZ != 1.0f)
	{
		qxTerrainVert* pVert = m_pVertList;

		while ( pVert )
		{
			pVert->CurrentVert.X *= m_fScaleXZ;
			pVert->CurrentVert.Z *= m_fScaleXZ;
			pVert = pVert->m_pNext;
		}
	}
}

int qxSkyDome::Frame()
{
	if(m_bMoveWithCamera)
	{
		MoveWithCamera();
	}

	// if we move our heads, update the color of the sky ( new triangles will need color)
	m_bTimeToUpdateSkyColor = m_bUpdate | CCD->CameraManager()->GetViewChanged();

	fTimer += CCD->LastElapsedTime_F()*0.001f;

	//geEngine_Printf(CCD->Engine()->Engine(), 0,10,"Time = %f",fTimer);

	//if( fTimer > m_fColorUpdateTime)
	{
		fTimer = 0.0f;
		m_bTimeToUpdateSkyColor = true;
	}

	if( pTextureFlow )
		pTextureFlow->Frame();

	AdjustSkyColor();

	return qxTerrainMap::Frame();
}


bool qxSkyDome::Render()
{
	return qxTerrainMap::Render();
}


bool qxSkyDome::RenderWireframe()
{
	return qxTerrainMap::RenderWireframe();
}


geBitmap* qxSkyDome::GetTexture()
{
	if( pTextureFlow )
		return pTextureFlow->GetBitmap();

	return qxTerrainMapBase::GetTexture();
}


geBitmap* qxSkyDome::GetTexture(int i)
{
	if( pTextureFlow )
		return pTextureFlow->GetBitmap();

	return qxTerrainMap::GetTexture(i);
}


bool qxSkyDome::LoadHeightMap()
{
	{
		delete [] m_pHeightMapData;
		m_pHeightMapData = 0;
	}


	//Probably not divisible by 32. If not, add another column and row.
	bool bIsDivisibleBy32 = !(m_nHeightMapWidth%32);

	if(bIsDivisibleBy32)
	{
		m_nHeightMapWidth++;
		m_nHeightMapLength++;
	}

	m_nHeightMapSize = m_nHeightMapWidth* m_nHeightMapLength;

	m_pHeightMapData = new int16[m_nHeightMapSize];

	if( !m_pHeightMapData )
	{
		return false;
	}

	//Zero out.
	memset(m_pHeightMapData, 0, (sizeof(int16))*(m_nHeightMapSize));


	float fPlanetSize = (float)CCD->TerrainMgr()->GetLandscapeSize() * CCD->TerrainMgr()->GetScaleXZ();// * 8.0f;
	float planesize = 2.0f * (float)sqrt( (	m_fMaxHeight*m_fMaxHeight) -
											fPlanetSize);

	float	fDelta = planesize/(float)(m_nHeightMapWidth-1);


	// NOTE
	// Since the data stored int16, we get data range errors if height turns
	// out to be < -32000 or > 32000

	for(int z = 0; z < m_nHeightMapLength ; z++)	// x loop
	{
		for(int x = 0; x < m_nHeightMapWidth; x++)	// z loop
		{
			float xdist = (-0.5f * planesize) + ((float)x*fDelta);
			float zdist = (-0.5f * planesize) + ((float)z*fDelta);

			float xheight = (xdist*xdist) / m_fMaxHeight;
			float zheight = (zdist*zdist) / m_fMaxHeight;
			float height = xheight + zheight;

			height = (m_fMaxHeight)-(height);//*fHeightFact);
			if( ( x == 0 || z == 0
				|| z == m_nHeightMapLength-1
				|| x == m_nHeightMapWidth-1 ) && height > 0.0f)
				height = 0.0f;

			height += (float)CCD->TerrainMgr()->GetSkyDomeOffsetY();
			SetElementHeight( x, z, (int)height);
		}
	}

	if( m_fScale != 1.0f)
		Scale( m_fScale );

	return true;
}


inline void qxSkyDome::CenterOnCamera()
{
	geVec3d vCenter;
	CCD->CameraManager()->GetPosition(&vCenter);

	qxTerrainVert* pVert = m_pVertList;

	while ( pVert )
	{
		pVert->CurrentVert.X -= vCenter.X;
		pVert->CurrentVert.Z -= vCenter.Z;
		pVert = pVert->m_pNext;
	}

	// update all base tiles
	for(int i = 0; i < m_nTilesCountTotal; i++)
	{
		m_ppBaseTiles[i]->Update();
	}

	m_bUpdate = true;
}


inline void qxSkyDome::MoveWithCamera()
{
	if( !CCD->CameraManager()->GetPositionMoved() )
		return;

	const geVec3d* pCamMove = CCD->CameraManager()->GetMoveDif();
	qxTerrainVert* pVert = m_pVertList;

	while ( pVert )
	{
		pVert->CurrentVert.X -= pCamMove->X;
		pVert->CurrentVert.Z -= pCamMove->Z;
		pVert = pVert->m_pNext;
	}

	// update all base tiles
	for(int i = 0; i < m_nTilesCountTotal; i++)
	{
		m_ppBaseTiles[i]->Update();
	}

	m_bUpdate = true;
}


void qxSkyDome::Draw()
{
}


void qxSkyDome::AdjustSkyColor()
{
	m_CurrentSkyColor.SetRGBA( m_CurrentVertColor );

	// Find the sun or moon
	// and brighten the nearer triangles
	static qxSun* pSun = CCD->TerrainMgr()->GetSun();

	m_CurrentSunColor = qxColorWhite;
	m_fSunIntensity = 1.0f;

	if(pSun)
	{
		// -1.0 midnight, 1.0 noon
		float fSunPercentToZenith = CCD->TerrainMgr()->GetSunPercentToZenith();
		// distance above and below horizon 
		float fTwilightDist = CCD->TerrainMgr()->GetTwilightDistanceFromHorizon();
		// Twilight distance is 0.0 to 1.0 ( from approx -.2 to .2 above horizon)
		float fTwilightPercent = CCD->TerrainMgr()->GetTwilightPercent() ;

		//
		// SUN COLOR
		//
		if( fTwilightPercent == 1.0 )
		{
			m_CurrentSunColor = qxColorWhite;
			m_fSunIntensity = 1.0f;
		}
		else if( fTwilightPercent == 0.0 )
		{
			m_CurrentSunColor = qxColorBlack;
			m_fSunIntensity = 0.0f;
		}
		else
		{
			m_CurrentSunColor = m_TwilightColor;

			// Percentage of twilight color dominance
			// twilight -> horizon = .80
			// horizon  -> twilight = .20
			float fPreHorizonPercent = (fSunPercentToZenith+fTwilightDist)/ fTwilightDist;
			fPreHorizonPercent = fPreHorizonPercent*.8f;
			fPreHorizonPercent = GE_CLAMP(fPreHorizonPercent, 0.0f, .8f);

			// horizon to twilight
			float fPostHorizonPercent = fSunPercentToZenith/ fTwilightDist;
			fPostHorizonPercent = fPostHorizonPercent * .2f;
			fPostHorizonPercent = GE_CLAMP(fPostHorizonPercent, 0.0f, .2f);

			m_fSunIntensity = fPreHorizonPercent;

			if( fTwilightPercent > .50f )
			{
				m_fSunIntensity += fPostHorizonPercent;
			}

			// Move toward white as dominance wanes
			float s = m_CurrentSunColor.GetS();
			s *= 1.0f-m_fSunIntensity;
			m_CurrentSunColor.SetS(s);
		}	// SUN COLOR
	}

	// Darken color (toward black) as percent wanes
	float v = m_CurrentSkyColor.GetV();
	v *= m_fSunIntensity;
	m_CurrentSkyColor.SetV(v);

	// White->Max Blue as percent increases
	float s = m_CurrentSkyColor.GetS();
	float delta = 1.0f-s;
	s += delta * (1.0f-m_fSunIntensity);
	m_CurrentSkyColor.SetS(s);
}


void qxSkyDome::LightVertexWireFrame(qxTerrainVert* pVert)
{
	pVert->CurrentVert.r = 0;
	pVert->CurrentVert.g = 0;
	pVert->CurrentVert.b = 255;
	pVert->CurrentVert.a = 255;
}


void qxSkyDome::LightVertex(qxTerrainVert* pVert)
{
	if( !m_bTimeToUpdateSkyColor )
		return;

	// Find the sun or moon
	// and brighten the triangles with alpha
	static qxSun* pSun = CCD->TerrainMgr()->GetSun();

	if(!pSun)
	{
		qxColor SkyColor( m_CurrentSkyColor );
		pVert->CurrentVert.r = SkyColor.rgba.r;
		pVert->CurrentVert.g = SkyColor.rgba.g;
		pVert->CurrentVert.b = SkyColor.rgba.b;
		pVert->CurrentVert.a = SkyColor.rgba.a;
		return;
	}

	float fTwilightPercent = CCD->TerrainMgr()->GetTwilightPercent();
	float fDist = geVec3d_DistanceBetween(	&pSun->Origin,
		((geVec3d*)&(pVert->CurrentVert.X)));

	if( fDist != 0.0f )
	{
		float fDistClamped = m_fDistanceFromSunFactor / fDist;
		fDist = fDistClamped;
		fDistClamped = GE_CLAMP(fDistClamped, 0.0f, 1.0f);

		qxColor SkyColor( m_CurrentSkyColor );

		// Increase the saturation farther away (move to deeper blue)
		float s = SkyColor.GetS();
		float delta = 1.0f-s;
		s += delta * (1.0f-fDistClamped);
		SkyColor.SetS(s);

		float v = SkyColor.GetV();
		v = v * (fDistClamped*2);
		v = GE_CLAMP(v, 0.0f, 1.0f);
		SkyColor.SetV(v);


		if( fTwilightPercent == 1.0 )
		{
			pVert->CurrentVert.r = SkyColor.rgba.r;
			pVert->CurrentVert.g = SkyColor.rgba.g;
			pVert->CurrentVert.b = GE_CLAMP(SkyColor.rgba.b, m_fMinBlueSkyColor, 255);
			pVert->CurrentVert.a = SkyColor.rgba.a;
		}
		// Add in the sunlight glare
		// Weakening influence the closer to the end of twilight
		else
		{
			pVert->CurrentVert.r = SkyColor.rgba.r*m_fSunIntensity;
			pVert->CurrentVert.g = SkyColor.rgba.g*m_fSunIntensity;
			pVert->CurrentVert.b = SkyColor.rgba.b * m_fSunIntensity;
			pVert->CurrentVert.a = SkyColor.rgba.a;

			float fIntensity = ( fDist*( 1.0f-m_fSunIntensity ) );

			pVert->CurrentVert.r = GE_CLAMP8(pVert->CurrentVert.r +
				(m_fSunIntensity*
				(m_CurrentSunColor.rgba.r * fIntensity )
				) );

			pVert->CurrentVert.g = GE_CLAMP8( pVert->CurrentVert.g +
				(m_fSunIntensity*
				(m_CurrentSunColor.rgba.g * fIntensity )
				) );

			pVert->CurrentVert.b = GE_CLAMP( pVert->CurrentVert.b +
				(m_fSunIntensity*
				(m_CurrentSunColor.rgba.b * fIntensity ) ),
				m_fMinBlueSkyColor, 255
				);
		}
	}
}


