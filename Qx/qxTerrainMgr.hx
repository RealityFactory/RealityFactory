// qxTerrainMgr.h: 
//
// Author: Jeff Thelen
//
// Copyright 2000 Quixotic, Inc.  All Rights Reserved.
//////////////////////////////////////////////////////////////////////

#ifndef QX_TERRAIN_MGR_H
#define QX_TERRAIN_MGR_H

#include "..\\RabidFramework.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define PRIORITIES 512

#define MAX_DISTANCE_DETAIL 16

//#include "qxWaveEffect.h"
#include "qxColor.h"

class qxStarField;
class qxPolyPool;
class qxVertPool;
class qxEffectTextureFlow;
class qxTerrainMapBase;
class qxCloudMachine;
class qxSun;
class qxMoon;
class qxTerrainDefinitionFile;
class qxSkyDome;
class qxColor;


class qxTerrainMgr
{
public:

	qxTerrainMgr();
	~qxTerrainMgr();

	bool Init();
	bool Frame();
	void Draw();
	bool Render();
	int GetLandscapeSize()			{ return m_nLandscapeSize; }
	qxSun*	GetSun(){ return m_pSun; }
	qxSkyDome*	GetSkyDome() { return m_pSkyDome; }
	const	geXForm3d* GetEarthRotation(){ return &m_matEarthRotation; }
	float GetScaleXZ()	{ return m_fScaleXZ; }
	float	GetTwilightDistanceFromHorizon() { return m_fTwilightDistanceFromHorizon; }
	float	GetSunPercentToZenith();
	float	GetTwilightPercent() { return m_fTwilightPercent; }
	CIniFile *GetIniFile()
	{ return &AttrFile; }

	void ToggleRenderWireframe();

private:

	bool		m_bRenderWireframe;
	float		m_fScaleXZ;
	int			m_nLandscapeSize;

	void		Shutdown();

	//
	// Clouds
	//

	qxCloudMachine*	m_pCloudMachine;

	//
	// Sun, Moon, Stars
	//
	qxSun*			m_pSun;
	qxMoon*			m_pMoon;
	qxStarField*	m_pStarField;
	qxSkyDome*		m_pSkyDome;
	float			m_fTwilightDistanceFromHorizon;
	void UpdateTwilightPercent();

	//
	// Earth's rotation matrix
	//
	geXForm3d	m_matEarthRotation;
	void		SetEarthAxis();

	void		UpdateFog();
	qxColor		m_ColorFog;
	
	float		m_fTwilightPercent;
	CIniFile AttrFile;
};

#endif // !defined(AFX_WORLD_H__5F7A1C41_0BE9_11D3_BE84_50514EC10001__INCLUDED_)
