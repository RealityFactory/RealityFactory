// qxTerrainMap.h:
//
// Original code: GenScape (c) 1999, Lucas Ackerman
//
// Author: Jeff Thelen
//
// Copyright 2000 Quixotic, Inc.  All Rights Reserved.
//////////////////////////////////////////////////////////////////////

#ifndef qxTerrainMap_H
#define qxTerrainMap_H


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\\RabidFramework.h"
#include "qxTerrainDefinitionFile.h"
#include "qxTerrainMapBase.h"

class	qxTerrainMgr;
class	qxTerrainTile;
class	qxTerrainVert;


class qxTerrainMap : public qxTerrainMapBase
{
public:

	qxTerrainMap(	qxTerrainDefinition& TerrainDef );

	virtual			~qxTerrainMap();

	virtual bool	Init();
	virtual bool	Render();
	virtual bool	RenderWireframe();
	virtual int		Frame();

	geBitmap*	GetTexture(int i) {  return m_pTileTextures[i]; }

	inline int16	GetElementHeight( int x, int Z );
	int16			GetElementHeightUnscaled(int x, int z );
	inline void		SetElementHeight(int X, int Z, int16 Y);

	virtual	void	ShadeTexture();


	void		SetFractal(	int	nFractalMapSize,
							int	nFractalScale,
							int	nFractalSeed,
							float fFractalHeight,
							int	nFractalDefaultHeight)
	{
		m_eHeightFieldSourceType	= HEIGHTFIELD_FRACTAL;
		m_nFractalMapSize			= nFractalMapSize;
		m_nFractalScale				= nFractalScale;
		m_nFractalSeed				= nFractalSeed;
		m_fFractalHeight			= fFractalHeight;
		m_nFractalDefaultHeight		= nFractalDefaultHeight;
	}

protected:

	void			ShadeLambert();
	void			ShadeGouraud();

	std::string		m_strHeightMap;
	virtual	bool	LoadHeightMap();
	bool			BuildTextures();

	// data array, holds heightmap data.
	int16*		m_pHeightMapData;	// = (Length * Width) x2 bytes per height element.

	void		Scale( float fScale );

	int			m_nTileTextureSize;
	int			m_nTEXTURESIZE;


	geBitmap** m_pTileTextures;

	//
	// Fractal functions
	//
	int		FractalAvgSquareVals ( int i, int j, int Stride, int nMesh );
	int		FractalAvgDiamondVals (int i, int j, int Stride, int nLines, int nMesh );
	bool	FractalGenerate( );

	int		m_nFractalMapSize;
	int		m_nFractalScale;
	int		m_nFractalSeed;
	float	m_fFractalHeight;
	int		m_nFractalDefaultHeight;

	eHeightFieldSourceType	m_eHeightFieldSourceType;
};


inline void qxTerrainMap::SetElementHeight(int X, int Z, int16 Y)
{
	QXASSERT(X < m_nHeightMapWidth && X >= 0);
	QXASSERT(Z < m_nHeightMapLength && Z >= 0);
	QXASSERT(m_pHeightMapData);

	m_pHeightMapData[(m_nHeightMapLength * Z) + X] =  Y;
}


inline int16 qxTerrainMap::GetElementHeight( int x, int z )
{
	QXASSERT(x < m_nHeightMapWidth && x >= 0);
	QXASSERT(z < m_nHeightMapLength && z >= 0);
	QXASSERT(m_pHeightMapData);

	return m_pHeightMapData[(m_nHeightMapLength * z) + x]+CCD->TerrainMgr()->GetOffsetY();
}


#endif