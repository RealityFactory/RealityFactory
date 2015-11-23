// qxSkyDome.h: 
//
// Original code: GenScape (c) 1999, Lucas Ackerman
//
// Author: Jeff Thelen
//
// Copyright 2000 Quixotic, Inc.  All Rights Reserved.
//////////////////////////////////////////////////////////////////////

#ifndef qxSkyDome_H
#define qxSkyDome_H


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\\RabidFramework.h"
#include "qxTerrainDefinitionFile.h"
#include "qxTerrainMap.h"

class	qxTerrainMgr;
class	qxTerrainTile;
class	qxTerrainVert;
class   qxTerrainMap;


class qxSkyDome : public qxTerrainMap
{

public:

	qxSkyDome(	qxTerrainDefinition& TerrainDef, 
				int WidthLength, 
				float fMaxHeight,
				float fScaleXZ = 1.0f);

	~qxSkyDome();

	bool		Init();
	bool		Render();
	bool		RenderWireframe();
	int			Frame();
	void		Draw();

	void		LightVertexWireFrame(qxTerrainVert* pVert);
	void		LightVertex(qxTerrainVert* pVert);

	void		ToggleMoveWithCamera() { m_bMoveWithCamera = !m_bMoveWithCamera; }

	void		ScaleXZ();


	float		GetDistanceFromSunFactor() { return m_fDistanceFromSunFactor; }
	float		GetMaxSkyHeight() { return m_fMaxHeight; }
	
	qxColor*	GetCurrentSunColor()	{ return &m_CurrentSunColor; }
	GE_RGBA*	GetCurrentSkyColor()	{ return &m_CurrentSkyColor.rgba; }
	float		GetSunIntensity()		{ return m_fSunIntensity; }
	qxColor		GetTwilightColor()		{ return m_TwilightColor; }


	void		SetMinBlueSkyColor(float f) { m_fMinBlueSkyColor = f; }
	float		GetMinBlueSkyColor()		{ return m_fMinBlueSkyColor; }
	float		GetScaleXZ() { return m_fScaleXZ; }

protected:


	geBitmap*	GetTexture();
	geBitmap*	GetTexture(int i) ;
	void		CenterOnCamera();
	void		MoveWithCamera();
	void		ShadeTexture() {};
	bool		LoadHeightMap();
	float		m_fScale;
	float		m_fMaxHeight;
	bool		m_bMoveWithCamera;
	
	
	
	// Scale the dome in the X Z directions
	float		m_fScaleXZ;


	void		AdjustSkyColor();
	qxColor		m_CurrentSkyColor;
	
	// Factor for determining color depending on triangle's distance from sun
	float		m_fDistanceFromSunFactor;

	float		m_fColorUpdateTime;

	// Timer flag so we don't update every triangle every frame if not necessary
	bool		m_bTimeToUpdateSkyColor;

	qxColor		m_TwilightColor;
	qxColor		m_CurrentSunColor;
	float		m_fMinBlueSkyColor;
	float		m_fSunIntensity;
	float		fTimer;

};





#endif