// qxTerrainMapBase.h:
//
// Original code: GenScape (c) 1999, Lucas Ackerman
//
// Author: Jeff Thelen
//
// Copyright 2000 Quixotic, Inc.  All Rights Reserved.
//////////////////////////////////////////////////////////////////////

#ifndef qxTerrainMapBase_H
#define qxTerrainMapBase_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "qxTerrainVert.h"
#include "qxTerrainUtils.h"
#include "qxTerrainDefinitionFile.h"


class qxTerrainMgr;
class qxTerrainTile;
class qxTerrainVert;
class qxTerrainMapBase;
class qxVertPool;
class qxPolyPool;
class qxTerrainPoly;
class qxSplitQueue;
class qxMergeQueue;


class qxTerrainMapBase
{
public:

	qxTerrainMapBase() {}
	explicit qxTerrainMapBase( qxTerrainDefinition& TerrainDef );
	virtual ~qxTerrainMapBase();

	virtual	bool	Render();
	virtual bool	RenderWireframe();
	virtual void	Draw();
	virtual int		Frame();
	virtual bool	InitBaseVerts();
	virtual void	LightVertex(qxTerrainVert* pVert);
	virtual void	LightVertexSunLight(qxTerrainVert* pVert);
	virtual void	LightVertexWireFrame(qxTerrainVert* pVert);
	virtual void	LightVertex(qxTerrainVert* pVert, GE_RGBA* pColor);
	virtual void	LightAllVerts(GE_RGBA* pColor);
	virtual void	LightAllVerts(float alpha);
	virtual void	LightAllVerts();
	virtual void	OffsetAllVerts();

	virtual bool Init();


	virtual		geBitmap*	GetTexture()		{ return m_pTexture; }
	virtual		geBitmap*	GetTexture(int)		{ return m_pTexture; }

	bool		GetRenderBackFace()				{ return m_bRenderBackFace; }

	int			GetMapOffsetIndexZ()			{ return	m_nMapOffsetIndexZ; }
	int			GetMapOffsetIndexX()			{ return	m_nMapOffsetIndexX; }
	int			GetMapOffsetZ()					{ return	m_nMapOffsetZ; }
	int			GetMapOffsetX()					{ return 	m_nMapOffsetX; }
	int			SetDesiredPolyCount(int newCount);


	void PutPoly(qxTerrainPoly** p) { m_pPolyPool->PutPoly(p); }
	void PutVert(qxTerrainVert** p) { m_pVertPool->PutVert(p); }

	void SetCurrentVertColor(GE_RGBA* pRGBA) { m_CurrentVertColor = *pRGBA; }

	void SetCurrentVertColor(float r, float g, float b, float a)
	{
		m_CurrentVertColor.r = r; m_CurrentVertColor.g = g;
		m_CurrentVertColor.b = b; m_CurrentVertColor.a = a;
	}

	void GetCurrentVertColor(float& r, float& g, float&b, float& a)
	{
		r = m_CurrentVertColor.r; g = m_CurrentVertColor.g;
		b = m_CurrentVertColor.b; a = m_CurrentVertColor.a;
	}

	int			GetMaxVarianceLookupId() { return m_nMaxVarianceLookupId; }

	virtual int16	GetElementHeight( int x, int z ) = 0;
	virtual bool	SetCurrentVertHeight( qxTerrainVert* pVert);
	virtual int16	GetElementHeightUnscaled(int x, int z){ return GetElementHeight(x, z); }


	int			SetDistanceDetail(int newDetail);

	int			GetPolyCount() { return m_nPolyCount; }


	void		SetNormalDistanceToCamera(float f) { m_fNormalDistanceToCamera = f;}

	virtual void	RebuildAllTextures() {}
	virtual void	ShadeTexture(){}

	void			Dump();

	void			SetRenderFlags(int n) { m_nRenderFlags = n; }
	void			SetSunLight(bool flag) { m_SunLight = flag; }

protected:

	int					m_nRenderFlags;

	// Heighmap length and width
	int					m_nHeightMapWidth;
	int					m_nHeightMapLength;
	int					m_nHeightMapSize;

	// Individual tile length and width
	int					m_nTileSize;

	// Number of tiles in width and length
	int					m_nTilesCountX;
	int					m_nTilesCountZ;
	int					m_nTilesCountTotal;

	int					m_nHeightElementsPerTile;

	// Index of this patch's position in the world (x, z)
	int					m_nMapOffsetIndexX;
	int					m_nMapOffsetIndexZ;
	// World coordinate offsets
	int					m_nMapOffsetZ;
	int					m_nMapOffsetX;

	// The actual world size of this terrain
	int					m_nLandscapeSize;


	int					m_nPolyCount;
	int					m_nDesiredPolyCount;
	int					m_nMinPolyCount;
	std::string			m_strTextureFile;
	std::string			m_strAlphaFile;

	bool				m_bUpdate;

	bool				m_bRenderBackFace;

	bool				m_SunLight;

	GE_RGBA				m_CurrentVertColor;

	//
	// these are points which lie on the view frustrum halfspace planes
	//
	geVec3d FrustrumLeft;
	geVec3d FrustrumRight;
	geVec3d FrustrumTop;
	geVec3d FrustrumBottom;
	geVec3d FrustrumNear;
	geVec3d FrustrumFar;

	//
	// normal for each halfspace plane
	//
	geVec3d NormalLeft;
	geVec3d NormalRight;
	geVec3d NormalTop;
	geVec3d NormalBottom;
	geVec3d NormalNear;
	geVec3d NormalFar;


	geBitmap*		m_pTexture;

	qxVertPool*		m_pVertPool;
	qxPolyPool*		m_pPolyPool;

	/////////////////////////////////////////////////////////////////////////////////

	qxTerrainTile**		m_ppBaseTiles;		// pointer to array of base tiles
	qxTerrainVert*		m_pVertList;		// base array of active verts.  rest are Sub_Vert's in TerrainPoly's

	//
	// Level of Detail
	// Dynamic recalculation of polygons depending on distance form camera
	//

	// The nearest poly distance to camera
	float				m_fNearestDistanceToCamera;
	// this is the normal distance (100% polys)
	float				m_fNormalDistanceToCamera;

	// For Water maps, we force the nearest polys to split
	int					m_nNearestIsHighest;


	float				m_fScale;

	//
	// max id# in the variance lookup tables
	//
	int					m_nMaxVarianceLookupId;


	int					m_nDistanceDetail;
	int					m_nFarPlane;


	qxSplitQueue*		m_pSplitQueue;
	qxMergeQueue*		m_pMergeQueue;

	// after a Split(qxTerrainPoly), this will point to a list of split qxTerrainPoly's.
	qxTerrainPoly*		m_pSplitList;


	void			UpdateViewFrustrum();
	qxTerrainVert*	AllocateVert();
	qxTerrainPoly*	AllocatePoly();
	bool			LoadTexture(const char* strTextureFile, const char* strAlphaFile);
	void			ResetBaseTiles();

	void			GetSurfaceNormal(int X, int Z, geVec3d* Normal);

	void Split( qxTerrainPoly* pPoly );
	void Split2( qxTerrainPoly* pPoly, bool bCopy );
	void Merge( qxTerrainPoly* pPoly );


	//
	// Friends
	//
	friend class qxTerrainPoly;
	friend class qxTerrainMgr;
};


inline void qxTerrainMapBase::LightAllVerts(float alpha)
{
	qxTerrainVert* pVert = m_pVertList;

	while( pVert != NULL)
	{

		pVert->CurrentVert.a = alpha;
		pVert = pVert->m_pNext;
	}
}


inline void qxTerrainMapBase::LightAllVerts(GE_RGBA* pColor)
{
	qxTerrainVert* pVert = m_pVertList;

	if( pColor )
	{
		while( pVert != NULL)
		{
			pVert->CurrentVert.r = pColor->r;
			pVert->CurrentVert.g = pColor->g;
			pVert->CurrentVert.b = pColor->b;
			pVert->CurrentVert.a = pColor->a;
			pVert = pVert->m_pNext;
		}
	}
	else
	{
		while( pVert != NULL)
		{
			pVert->CurrentVert.r = m_CurrentVertColor.r;
			pVert->CurrentVert.g = m_CurrentVertColor.g;
			pVert->CurrentVert.b = m_CurrentVertColor.b;
			pVert->CurrentVert.a = m_CurrentVertColor.a;
			pVert = pVert->m_pNext;
		}
	}
}


#endif