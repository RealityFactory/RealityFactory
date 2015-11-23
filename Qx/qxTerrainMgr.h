// qxTerrainMgr.h: 
//
// Author: Jeff Thelen
//
// Copyright 2000 Quixotic, Inc.  All Rights Reserved.
//////////////////////////////////////////////////////////////////////

#ifndef QX_TERRAIN_MGR_H
#define QX_TERRAIN_MGR_H


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define PRIORITIES 512

#include "..\\RabidFramework.h"
#include "qxColor.h"
#include "..\\Simkin\\skScriptedExecutable.h"

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

class TerrainObject : public skScriptedExecutable
{
public:
	TerrainObject(char *fileName);
	~TerrainObject();
	bool getValue (const skString& fieldName, const skString& attribute, skRValue& value);
    bool setValue (const skString& fieldName, const skString& attribute, const skRValue& value);
    bool method (const skString& methodName, skRValueArray& arguments,skRValue& returnValue);

	char	Order[64];
	float	ElapseTime;
};

class qxTerrainMgr
{
public:

	qxTerrainMgr();
	~qxTerrainMgr();

	bool Init();
	bool Frame();
	void Draw();
	bool Load();
	bool Render();

	void PolyCountRaise();
	void PolyCountLower();
	void DistanceDetailLower();
	void DistanceDetailRaise();

	CIniFile *GetIniFile()
	{ return &AttrFile; }


	geVec3d* GetLightSource()		{ return &m_vLightSource; }
	float GetLightAmbient()			{ return m_fLightAmbient; }
	float GetLightIntensity()		{ return m_fLightIntensity; }

	int GetFarPlane()				{ return m_nFarPlane; }
	int GetHeightFieldSourceType()	{ return m_nHeightFieldSourceType; }
	qxPolyPool*	GetPolyPoolQX()		{ return m_pqxPolyPool; }
	qxVertPool* GetVertPoolQX()		{ return m_pqxVertPool; }

	int GetDesiredTriangles()		{ return m_nDesiredTriangles; }
	int GetLandscapeSize()			{ return m_nLandscapeSize; }
	float GetScaleY()				{ return m_fScaleY; }
	float GetScaleXZ()				{ return m_fScaleXZ; }
	int GetOffsetY()				{ return m_OffsetY; }
	int GetShadingMethod()			{ return m_nShadingMethod; }
	int GetDistanceDetail()			{ return m_nDistanceDetail; }
	gePixelFormat GetPixelFormatFinal() { return m_PixelFormatFinal; }
	float GetScaleSun()				{ return m_SunScale; }

	GE_RGBA Getrgba()  { return rgba; }
	float GetMinBlueSkyColor()  { return m_fMinBlueSkyColor; };
	float GetDistanceFromSunFactor()  { return m_fDistanceFromSunFactor; };
	GE_RGBA Getcolor()  { return color; };
	float GetColorUpdateTime()  { return m_fColorUpdateTime; };
	bool GetTextureFlow()  { return TextureFlow; };

	void SetNormalDistanceToCamera( float f );
	float GetNormalDistanceToCamera() { return m_fNormalDistanceToCamera; }

	void ToggleRenderWireframe();
	void RebuildAllTextures();
	GE_RGBA* GetAmbientLightColor() { return &m_AmbientLightColor; }

	qxTerrainMapBase* GetEastNeighbor(qxTerrainMapBase* p);
	qxTerrainMapBase* GetWestNeighbor(qxTerrainMapBase* p);
	qxTerrainMapBase* GetNorthNeighbor(qxTerrainMapBase* p);
	qxTerrainMapBase* GetSouthNeighbor(qxTerrainMapBase* p);
	qxTerrainMapBase* GetSouthWestNeighbor(qxTerrainMapBase* p);
	qxTerrainMapBase* GetSouthEastNeighbor(qxTerrainMapBase* p);
	qxTerrainMapBase* GetNorthWestNeighbor(qxTerrainMapBase* p);
	qxTerrainMapBase* GetNorthEastNeighbor(qxTerrainMapBase* p);

	
	qxTerrainMapBase* GetMap( int OffsetX, int OffsetZ );



	const	geXForm3d* GetEarthRotation(){ return &m_matEarthRotation; }

	float	GetTwilightDistanceFromHorizon() { return m_fTwilightDistanceFromHorizon; }
	float	GetSunPercentToZenith();
	float	GetTwilightPercent() { return m_fTwilightPercent; }

	qxSun*	GetSun(){ return m_pSun; }

	void	SetWorldBounds( int X, int Z ) { m_nWorldBoundsX = X, m_nWorldBoundsZ = Z; }
	bool	GetRenderWireframe() { return m_bRenderWireframe; }

	qxSkyDome*	GetSkyDome() { return m_pSkyDome; }

	geBitmap *GetTexture() { return PolyTex; }
	void SetTexture(geBitmap *Bitmap) { PolyTex = Bitmap; }
	
private:

	bool Initialized;
	// the furthest boundaries of the world
	int m_nWorldBoundsX;
	int m_nWorldBoundsZ;

	GE_RGBA		m_AmbientLightColor;

	qxPolyPool*	m_pqxPolyPool;
	qxVertPool*	m_pqxVertPool;

	int			m_nDistanceDetail;
	bool		m_bUpdateLandscape;
	bool		m_bRenderWireframe;
	bool		m_bRenderLandscape;
	int			m_nHeightFieldSourceType;

	float		m_fScaleY;
	float		m_fScaleXZ;
	int			m_nFarPlane;
	int			m_OffsetY;
	int			m_nDesiredTriangles;
	int			m_nShadingMethod;
	float		m_SunScale;
	float		m_TimeScale;

	// The threshold for determining percentage of triangles to put onscreen per TerrainPatch
	float		m_fNormalDistanceToCamera;
	int			m_nPolyIncrement;
	int			m_WorldToTilesWidth;
	int			m_WorldToTilesLength;
	int			m_nLandscapeSize;
	gePixelFormat m_PixelFormatFinal;

	geBitmap *PolyTex;


	geVec3d m_vLightSource;
	float m_fLightAmbient;
	float m_fLightIntensity;

	float fPlayerLatitude;
	float m_fPlayerLatitudeRotation;
	int Hour;
	int Day;
	int Month;
	float TerrainTime;
	TerrainObject *Object;


	void		DynamicLoadCheck();
	void		Shutdown();
	void		ShadeAllTextures();
	void		RenderAll(void);
	void		RenderAllWireframe(void);
	void CalculatePlayerLatitude();


	//
	// Terrain definition
	// 

	bool LoadTerrainDefinitionFile(char* pStr);
	qxTerrainDefinitionFile* m_pTerrainDefinitionFile;


	CTypedPtrArray<CPtrArray, qxTerrainMapBase*>m_pMaps;
	bool LoadMap( int OffsetX, int OffsetZ );

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
	char m_strBmp[64];
	int SkyMaxHeight;
	float fScaleXZ;
	float fSkyLengthHeight;
	GE_RGBA rgba;
	float m_fMinBlueSkyColor;
	float m_fDistanceFromSunFactor;
	GE_RGBA color;
	float m_fColorUpdateTime;
	bool TextureFlow;

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
