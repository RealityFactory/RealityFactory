/****************************************************************************************/
/*	qxTerrainMgr.h:																		*/
/*																						*/
/*	Author: Jeff Thelen																	*/
/*																						*/
/*	Copyright 2000 Quixotic, Inc.  All Rights Reserved.									*/
/*																						*/
/*	Edit History:																		*/
/*	=============																		*/
/*	02/01/07 QD:	- added skxdome Y offset											*/
/*																						*/
/****************************************************************************************/

#ifndef QX_TERRAIN_MGR_H
#define QX_TERRAIN_MGR_H


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define PRIORITIES 512

#include "qxColor.h"
#include "QxUser.h"
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
    bool method (const skString& methodName, skRValueArray& arguments,skRValue& returnValue,skExecutableContext &ctxt);//change simkin

	char	Order[64];
	float	ElapseTime;
	char	szName[64];
};


class qxTerrainMgr
{
public:

	qxTerrainMgr();
	~qxTerrainMgr();

	bool Init();
	bool Frame();
	void Draw();
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
	int GetLandscapeSize()			{ return m_nLandscapeSize * (int)m_fScaleXZ; }
	float GetScaleY()				{ return m_fScaleY; }
	float GetScaleXZ()				{ return m_fScaleXZ; }
	int GetOffsetY()				{ return m_OffsetY; }
	int GetSkyDomeOffsetY()			{ return m_SkyDomeOffsetY; }
	int GetShadingMethod()			{ return m_nShadingMethod; }
	int GetDistanceDetail()			{ return m_nDistanceDetail; }
	gePixelFormat GetPixelFormatFinal() { return m_PixelFormatFinal; }
	float GetScaleSun()				{ return m_SunScale; }
	bool GetAllowSun()				{ return AllowSun; }
	bool GetAllowCloud()			{ return AllowCloud; }
	void SetAllowSun(bool flag)		{ AllowSun = flag; }
	void SetAllowCloud(bool flag)	{ AllowCloud = flag; }
	bool GetAllowSkyFog()			{ return skyfog; }
	void SetAllowSkyFog(bool flag)			{ skyfog = flag; }

	void SetSunScale(float value)			{ m_SunScale = value; }
	void SetScaleY(float value)				{ m_fScaleY = value; }
	void SetScaleXZ(float value)			{ m_fScaleXZ = value; }
	void SetOffsetY(int value)				{ m_OffsetY = value; }
	void SetSkyDomeOffsetY(int value)		{ m_SkyDomeOffsetY = value; }
	void SetDesiredTriangles(int value)		{ m_nDesiredTriangles = value; }
	void SetLandscapeSize(int value)		{ m_nLandscapeSize = value; }
	void SetAmbientLightColor(float r, float g, float b, float a)
	{ m_AmbientLightColor.r = r;
	  m_AmbientLightColor.g = g;
	  m_AmbientLightColor.b = b;
	  m_AmbientLightColor.a = a;}
	void SetTwilightDistanceFromHorizon(float value) { m_fTwilightDistanceFromHorizon = value; }
	void SetTimeScale(float value)		{m_TimeScale = value; }
	void SetSky(int height, float res)
	{ SkyMaxHeight = height; fSkyLengthHeight = res; }
	void SetSkyTexture(char *name)
	{ strcpy(m_strBmp, name); }
	void SetSkyColor(float r, float g, float b, float a)
	{ rgba.r = r;
	  rgba.g = g;
	  rgba.b = b;
	  rgba.a = a;}
	void SetTwilightColor(float r, float g, float b, float a)
	{ color.r = r;
	  color.g = g;
	  color.b = b;
	  color.a = a;}
	void SetSunColor(float r, float g, float b, float a)
	{ suncolor.r = r;
	  suncolor.g = g;
	  suncolor.b = b;
	  suncolor.a = a;}
	void SetDistanceFromSunFactor(float value)	{m_fDistanceFromSunFactor = value; }
	void SetLocation(float lat, int mon, int day, int hour)
	{ fPlayerLatitude = lat;
	  Month = mon;
	  Day = day;
	  Hour = hour; }
	void SetTerrain(char *height, char *tex)
	{ strcpy(heightmap, height); strcpy(texmap, tex); }
	void SetMoonColor(float r, float g, float b, float a)
	{ mooncolor.r = r;
	  mooncolor.g = g;
	  mooncolor.b = b;
	  mooncolor.a = a;}
	void SetMoonPhase(int value)	{MoonPhase = value; }
	int GetMoonPhase()  { return MoonPhase; }

	GE_RGBA Getrgba()					{ return rgba; }
	float GetMinBlueSkyColor()  { return m_fMinBlueSkyColor; }
	float GetDistanceFromSunFactor()  { return m_fDistanceFromSunFactor; }
	GE_RGBA Getcolor()  { return color; }
	GE_RGBA GetSuncolor()  { return suncolor; }
	GE_RGBA GetMooncolor()  { return mooncolor; }
	float GetColorUpdateTime()  { return m_fColorUpdateTime; }
	bool GetTextureFlow()  { return TextureFlow; }
	float GetElapsedTime() { return ElapsedTime; }
	float GetTerrainTime() { return TerrainTime; }
	eDirection GetWindDir() { return WindDir; }
	void SetWindDir(eDirection Dir) { WindDir = Dir; }

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
	void	SetRenderWireframe(bool flag) { m_bRenderWireframe = flag; }
	void	SetRenderLandscape(bool flag) { m_bRenderLandscape = flag; }

	qxSkyDome*	GetSkyDome() { return m_pSkyDome; }
	void ToggleSkyFollow();

	geBitmap *GetTexture() { return PolyTex; }
	void SetTexture(geBitmap *Bitmap) { PolyTex = Bitmap; }
	void SetRender(int value) { nRenderflag = value; }
	int GetRender() { return nRenderflag; }

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
	int			m_SkyDomeOffsetY;
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

	char texmap[128];
	char heightmap[128];
	int nRenderflag;
	bool AllowSun;
	bool AllowCloud;


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
	float ElapsedTime;


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


	std::vector<qxTerrainMapBase*> m_pMaps;
	bool LoadMap( int OffsetX, int OffsetZ );

	//
	// Clouds
	//

	qxCloudMachine*	m_pCloudMachine;
	eDirection	WindDir;

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
	bool skyfog;
	GE_RGBA rgba;
	float m_fMinBlueSkyColor;
	float m_fDistanceFromSunFactor;
	GE_RGBA color;
	GE_RGBA suncolor;
	GE_RGBA mooncolor;
	int MoonPhase;
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

#endif
