// qxTerrainMap.cpp: implementation of the qxTerrainMap class.
//
// Original code: GenScape (c) 1999, Lucas Ackerman
//
// Author: Jeff Thelen
//
// Copyright 2000 Quixotic, Inc.  All Rights Reserved.
//////////////////////////////////////////////////////////////////////

#include "..\\RabidFramework.h"
#include "qxTerrainMgr.h"
#include "qxTerrainMapBase.h"
#include "qxTerrainMap.h"
#include "qxTerrainVert.h"
#include "qxTerrainTile.h"
#include "QxUser.h"


extern geBitmap *TPool_Bitmap(const char *DefaultBmp, const char *DefaultAlpha,
							  const char *BName, const char *AName);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


qxTerrainMap::qxTerrainMap(	qxTerrainDefinition& TerrainDef )

:	qxTerrainMapBase(	TerrainDef )

,	m_pHeightMapData(0)
,	m_pTileTextures(0)
,	m_strHeightMap( TerrainDef.m_strHeightmap )
,	m_eHeightFieldSourceType( TerrainDef.m_eHeightFieldSourceType )
,	m_nFractalMapSize( 64 )
,	m_nFractalScale ( 256 )
,	m_nFractalSeed ( 0 )
,	m_fFractalHeight(.80f)
,	m_nFractalDefaultHeight(10)
,	m_nTEXTURESIZE(64)
{

}

qxTerrainMap::~qxTerrainMap()
{
	if( m_pHeightMapData )
	{
		delete [] m_pHeightMapData;
	}

	//
	// Delete the textures
	//
	if( m_pTileTextures )
	{
		for(int i = 0; i < m_nTilesCountTotal; i++)
		{
			if( m_pTileTextures[i] )
			{
				geWorld_RemoveBitmap(CCD->Engine()->World(), m_pTileTextures[i]);
				geBitmap_Destroy(&m_pTileTextures[i]);
			}
		}

		delete [] m_pTileTextures;
	}

}




bool qxTerrainMap::Init()
{
	if( !qxTerrainMapBase::Init() )
		return false;

	//
	// Load Main Texture
	//
	QXASSERT( m_strTextureFile.empty()==false );

	char TextureFile[64];
	strcpy(TextureFile, m_strTextureFile.c_str());

	m_pTexture = CreateFromFileName(TextureFile);
	geBitmap_SetPreferredFormat(m_pTexture,GE_PIXELFORMAT_24BIT_RGB);

	if(!m_pTexture)
	{
		return false;
	}


	if( !LoadHeightMap() )
		return false;

	m_nTilesCountX		= geBitmap_Width(m_pTexture) / m_nTEXTURESIZE;
	m_nTilesCountZ		= geBitmap_Height(m_pTexture) / m_nTEXTURESIZE;
	m_nTilesCountTotal	= m_nTilesCountX * m_nTilesCountZ;
	m_nTileSize			= m_nLandscapeSize / m_nTilesCountX;

	QXASSERT(m_nTileSize == m_nLandscapeSize / m_nTilesCountZ);


	int MaxSplitDepth = (int)(2.0 * log10((double)((m_nHeightMapWidth-1) / m_nTilesCountX)) / (log10(2.0)));
	m_nMaxVarianceLookupId = (1<<MaxSplitDepth) - 1;	//(2 ^ Max_Split_Depth) - 1;

	// hmap-coord tile size
	m_nHeightElementsPerTile = (m_nHeightMapWidth-1) / m_nTilesCountX;
	// both dimensions should work out to be the same, else wev got a problem!
	QXASSERT(m_nHeightElementsPerTile == (m_nHeightMapLength - 1) / m_nTilesCountZ);


	m_nPolyCount	= m_nTilesCountTotal * 2;
	m_nMinPolyCount = m_nPolyCount;

	if(m_nDesiredPolyCount < m_nMinPolyCount)
		m_nDesiredPolyCount = m_nMinPolyCount;


	if(!InitBaseVerts())
		return false;

	if( !BuildTextures() )
		return false;


	geBitmap_Destroy(&m_pTexture);
	m_pTexture = 0;

	return true;
}

int qxTerrainMap::Frame()
{
	return qxTerrainMapBase::Frame();
}

bool qxTerrainMap::Render()
{
	return qxTerrainMapBase::Render();
}

bool qxTerrainMap::RenderWireframe()
{
	return qxTerrainMapBase::RenderWireframe();
}



bool qxTerrainMap::BuildTextures( )
{
	QXASSERT( m_pTexture );

	//
	// Delete the current textures, if any
	//
	if( m_pTileTextures )
	{
		for(int i = 0; i < m_nTilesCountTotal; i++)
		{
			if( m_pTileTextures[i] )
			{
				geWorld_RemoveBitmap(CCD->Engine()->World(), m_pTileTextures[i]);
				geBitmap_Destroy(&m_pTileTextures[i]);
			}
		}

		delete [] m_pTileTextures;
	}


	//
	// Load textures
	//


	geBitmap* Lock;

	m_pTileTextures = new geBitmap*[ m_nTilesCountTotal ];
	if( !m_pTileTextures )
	{
		return false;
	}


	// D3D driver doesn't use more than 4 or 5.  might as well save on tex mem.
	int mips = 5;


	geBitmap_Info Info;

	if(!geBitmap_GetInfo(m_pTexture, &Info, 0) )
	{
		return false;
	}

	gePixelFormat format = Info.Format;	//GE_PIXELFORMAT_8BIT;	// 8 bit palletized
	gePixelFormat newformat = GE_PIXELFORMAT_24BIT_RGB;//GE_PIXELFORMAT_8BIT;//GE_PIXELFORMAT_8BIT_GRAY;

	QXASSERT( Info.Width == Info.Height ); // for now...

	// get proper tile texture size:
	// bitmap is 1024 x 1024 / 4 = 256 each
	// 256 / 4 = 64
	m_nTileTextureSize = Info.Width / m_nTilesCountX;

	while(m_nTileTextureSize > m_nTEXTURESIZE)
	{
		m_nTileTextureSize = m_nTileTextureSize / 2;
	}


	QXASSERT(m_nTileTextureSize <= m_nTEXTURESIZE);
	QXASSERT(m_nTileTextureSize > 0);

	mips = mips - (( m_nTEXTURESIZE / m_nTileTextureSize ) - 1);
	//mips = mips - (( 256 / m_nTileTextureSize ) - 1);
	if(mips < 0 )
		mips = 0;


	int k;
	geBoolean success;

	for(int i = 0; i < m_nTilesCountZ; i++)	//y
	{
		for(int j = 0; j < m_nTilesCountX; j++)	//x
		{
			k = (i * m_nTilesCountZ + j);	//index

			// create tile texture
			m_pTileTextures[k] = geBitmap_Create(m_nTileTextureSize, m_nTileTextureSize, mips, format);
			QXASSERT(m_pTileTextures[k]);

			// lock
			success = geBitmap_LockForWriteFormat(m_pTileTextures[k], &Lock, 0, 0, format);
			if(!success)
			{
				geBitmap_SetFormat(m_pTileTextures[k],format,GE_TRUE,0,NULL);
				success = geBitmap_LockForWriteFormat(m_pTileTextures[k],&Lock,0,0, format);
			}
			QXASSERT(success);

			// copy from base texture
			success = geBitmap_Blit(m_pTexture,
									(j * m_nTileTextureSize),
									(i * m_nTileTextureSize),
									Lock, 0, 0, m_nTileTextureSize, m_nTileTextureSize);
			QXASSERT(success);

			// unlock
			success = geBitmap_UnLock(Lock);
			QXASSERT(success);

			//change to dest format
			success = geBitmap_SetFormat(	m_pTileTextures[k],
											newformat, GE_FALSE, 0, (geBitmap_Palette *)NULL);
			QXASSERT(success);

			// build mips
			for(int q = 1; q < mips; q++)	// update mips 1-7 from mip 0 (the full-sized texture)
			{
				success = geBitmap_UpdateMips(m_pTileTextures[k], 0, q);
				QXASSERT(success);
			}

			// add to world
			success = geWorld_AddBitmap(CCD->Engine()->World(), m_pTileTextures[k]);
			QXASSERT(success);

		}
	}

	ShadeTexture();

	return true;
}


void qxTerrainMap::ShadeTexture()
{

	int method = CCD->TerrainMgr()->GetShadingMethod();

	if( method == 1 )
		ShadeLambert();

	else if ( method == 2 )
		ShadeGouraud();

}

bool qxTerrainMap::LoadHeightMap()
{

	geBitmap	*pHeightBMP = 0;

	if( m_pHeightMapData )
	{
		delete [] m_pHeightMapData;
		m_pHeightMapData = 0;
	}

	//
	// HEIGHTFIELD_FRACTAL
	//
	if( m_eHeightFieldSourceType == HEIGHTFIELD_FRACTAL )
	{
		m_nHeightMapWidth	= m_nFractalMapSize+1;
		m_nHeightMapLength	= m_nFractalMapSize+1;
	}

	else if( m_eHeightFieldSourceType == HEIGHTFIELD_GREY_BMP )
	{
		char HeightMap[64];
		strcpy(HeightMap, m_strHeightMap.c_str());

		pHeightBMP = CreateFromFileName(HeightMap);

		if(!pHeightBMP)
		{
			return false;
		}

		geBitmap_SetPreferredFormat(pHeightBMP, GE_PIXELFORMAT_8BIT_GRAY);

		m_nHeightMapWidth	= geBitmap_Width(pHeightBMP);
		m_nHeightMapLength	= geBitmap_Height(pHeightBMP);
	}

	QXASSERT(m_nHeightMapWidth == m_nHeightMapLength); // for now...

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



	if( m_eHeightFieldSourceType == HEIGHTFIELD_FRACTAL )
	{
		if(!FractalGenerate( ) )
		{
			QXASSERT("FractalGenerate");
			return false;
		}

	}
	else if( m_eHeightFieldSourceType == HEIGHTFIELD_GREY_BMP )
	{
		geBitmap * pLock;
		geBitmap_Info Info;

		if( !geBitmap_LockForRead(pHeightBMP,&pLock,0,0,GE_PIXELFORMAT_8BIT_GRAY, GE_FALSE, 0) )
		{
			return false;
		}


		if( !geBitmap_GetInfo(pLock, &Info, NULL) )
		{
			return false;
		}

		uint8 *pBits = (uint8 *) geBitmap_GetBits(pLock);
		QXASSERT( pBits );

		for(int z=0; z < Info.Height; z++)
		{
			for(int x=0; x < Info.Width; x++)
			{
				SetElementHeight(x, z, (*(pBits++)));
			}

			// note that pBits is a byte pointer, and Stride is in pixels :
			pBits += Info.Stride -  Info.Width;
		}

		geBitmap_UnLock(pLock);

	} // HEIGHTFIELD_GREY_BMP


	if( CCD->TerrainMgr()->GetScaleY() != 1.0f)
		Scale( CCD->TerrainMgr()->GetScaleY() );

	if(pHeightBMP)
		geBitmap_Destroy(&pHeightBMP);

	return true;
}


void qxTerrainMap::Scale( float fScale )
{

	for( int x = 0; x < m_nHeightMapWidth; x++)
	{
		for( int z = 0; z < m_nHeightMapLength; z++)
		{
			SetElementHeight(x, z, GetElementHeight(x, z) * (int)fScale);
		}
	}
}



void qxTerrainMap::ShadeLambert()
{
	uint16 x, y, u, v, i, j;
	geBitmap *bmp, *Lock;
	geBitmap_Info Info;
	geBoolean success;
	gePixelFormat format = GE_PIXELFORMAT_24BIT_RGB;
	float shade;	// textel color intensity, 0 to 1.
	float shade_mult;
	uint8 *bits, *bptr, *bptr2;
	int R, G, B, A;
	uint8 bpp;

	geVec3d* pLightSource	= CCD->TerrainMgr()->GetLightSource();
	float fAmbient			= CCD->TerrainMgr()->GetLightAmbient();
	float fIntensity		= CCD->TerrainMgr()->GetLightIntensity();


	geVec3d Normal;

	uint8 TextelsPerElem	= m_nTileTextureSize / m_nHeightElementsPerTile;	// textels per height element

	// 8-bit per-hmap-element shade values.
	// normalized to be in 0 - 255 range.  0 = black, 255 = fullbright
	uint8 *slope_shade_map = (uint8*)malloc(sizeof(uint8) * m_nHeightMapSize);

	memset((void*)slope_shade_map, 128, m_nHeightMapSize);

	uint32 line_offset = 0;

	//
	// create slope_shade_map.  each height element gets a shade based on its surface normal
	//
	for(y = 0; y < m_nHeightMapLength; y++)
	{
		for(x = 0; x < m_nHeightMapWidth; x++)
		{
			GetSurfaceNormal(x, y, &Normal);

			shade = geVec3d_DotProduct(&Normal, pLightSource);
			shade = shade * (1 - fAmbient);

			// non-negative shade values only!
			if(shade < 0)
				shade = 0;

			slope_shade_map[x + line_offset] = (unsigned char)shade * 255;

		}

		line_offset += m_nHeightMapWidth;		// y * m_nHeightMapWidth
	}


	// for each tile texture, re-create from base texture and shading map
	for(i = 0; i < m_nTilesCountZ; i++) // y
	{
		for(j = 0; j < m_nTilesCountX; j++) // x
		{

			// get bmp
			bmp = GetTexture(i * m_nTilesCountX + j);

			geBitmap_GetInfo(bmp, &Info, (geBitmap_Info*) NULL);

			char szBug[256];
			sprintf(szBug, "Texture# %d Tex %d", i * m_nTilesCountX + j, Info.Format);
			CCD->ReportError(szBug, false);

			// lock for write
			success = geBitmap_LockForWriteFormat(bmp, &Lock, 0, 0, format);
			if(!success)
			{
				geBitmap_SetFormat(bmp,format,GE_TRUE,0,NULL);
				success = geBitmap_LockForWriteFormat(bmp,&Lock,0,0, format);
			}
			QXASSERT(success);

			success = geBitmap_GetInfo(Lock, &Info, (geBitmap_Info*) NULL);
			QXASSERT(success);

			sprintf(szBug, "Bpp %d Format %d", gePixelFormat_BytesPerPel(Info.Format), Info.Format);
			CCD->ReportError(szBug, false);

			bpp = gePixelFormat_BytesPerPel(Info.Format);
			bits = (uint8 *)geBitmap_GetBits(Lock);

			// for each height element under this texture tile
			for(y = 0; y < m_nHeightElementsPerTile; y ++)		//y
			{
				for(x = 0; x < m_nHeightElementsPerTile; x ++)		//x
				{
					shade = (float)slope_shade_map[i * m_nHeightMapWidth * m_nHeightElementsPerTile + j * m_nHeightElementsPerTile + y * m_nHeightMapWidth + x] * ONE_OVER_255;
					shade += slope_shade_map[i * m_nHeightMapWidth * m_nHeightElementsPerTile + j * m_nHeightElementsPerTile + (y+1) * m_nHeightMapWidth + x] * ONE_OVER_255;
					shade += slope_shade_map[i * m_nHeightMapWidth * m_nHeightElementsPerTile + j * m_nHeightElementsPerTile + y * m_nHeightMapWidth + (x+1)] * ONE_OVER_255;
					shade += slope_shade_map[i * m_nHeightMapWidth * m_nHeightElementsPerTile + j * m_nHeightElementsPerTile + (y+1) * m_nHeightMapWidth + (x+1)] * ONE_OVER_255;
					shade = shade * (float)0.25;	// = shade / 4;

					shade_mult = (shade + fAmbient) * fIntensity;

					// for each textel on this height element
					for(v = 0; v < TextelsPerElem; v ++)		//y
					{
						for(u = 0; u < TextelsPerElem; u ++)		//x
						{
							bptr = bits + bpp * ((TextelsPerElem * (y * TextelsPerElem * m_nHeightElementsPerTile + x)) + (v * Info.Stride + u));
							bptr2 = bptr;

							gePixelFormat_GetColor(format, &bptr, &R, &G, &B, &A);

							R = R * (int)shade_mult;
							G = G * (int)shade_mult;
							B = B * (int)shade_mult;

							gePixelFormat_PutColor(format, &bptr2, R, G, B, A);
						}
					}
				}
			}
			// unlock
			success = geBitmap_UnLock(Lock);
			QXASSERT(success);

			// refresh mips
			geBitmap_RefreshMips(bmp);
			//QXASSERT(success);

			gePixelFormat newformat = CCD->TerrainMgr()->GetPixelFormatFinal();
			success = geBitmap_SetFormat(bmp, newformat, GE_FALSE, 0, (geBitmap_Palette *)NULL);
			QXASSERT(success);
		}
	}

	free(slope_shade_map);
}



void qxTerrainMap::ShadeGouraud()
{

	uint16 x, y, u, v, i, j;
	geBitmap *bmp, *Lock;
	geBitmap_Info Info;
	geBoolean success;

	geVec3d* pLightSource	= CCD->TerrainMgr()->GetLightSource();
	float fAmbient			= CCD->TerrainMgr()->GetLightAmbient();
	float fIntensity		= CCD->TerrainMgr()->GetLightIntensity();

	gePixelFormat format = GE_PIXELFORMAT_24BIT_RGB;
	float shade;	// textel color intensity, 0 to 1.

	int TERRAIN_FINALFORMAT = CCD->TerrainMgr()->GetPixelFormatFinal();

	uint8 *bits, *bptr, *bptr2;
	int R, G, B, A;
	uint8 bpp;

	float TL, TR, BL, BR;	// shades of 4 hmap-elements in a square shape

	geVec3d Normal;			// surface normal


	uint8 TextelPerElem = m_nTileTextureSize / m_nHeightElementsPerTile;	// textels per height element

	uint8 * slope_shade_map;

	// 8-bit per-hmap-element shade values.
	// normalized to be in 0 - 255 range.  0 = black, 255 = fullbright
	slope_shade_map = (uint8*) malloc(sizeof(uint8) * m_nHeightMapLength * m_nHeightMapWidth);

	memset((void*)slope_shade_map, 128, m_nHeightMapLength * m_nHeightMapWidth);

	float* left_edge_list = (float*) malloc(sizeof(float) * TextelPerElem);
	float* right_edge_list = (float*) malloc(sizeof(float) * TextelPerElem);

	uint32 line_offset = 0;

	// create slope_shade_map.
	// each height element gets a shade based on its surface normal
	for(y = 0; y < m_nHeightMapLength; y++)	// y loop
	{
		for(x = 0; x < m_nHeightMapWidth; x++)	// x loop
		{
			GetSurfaceNormal(x, y, &Normal);

			shade = geVec3d_DotProduct(&Normal, pLightSource );
			shade = shade * (1 - fAmbient);

			// non-negative shade values only!
			if(shade < 0)
				shade = 0;

			slope_shade_map[x + line_offset] = (unsigned char)shade * 255;

		}

		line_offset += m_nHeightMapWidth;		// y * m_nHeightMapWidth
	}


	// for each tile texture, re-create from base texture and shading map
	for(i = 0; i < m_nTilesCountZ; i++) // y
	{
		for(j = 0; j < m_nTilesCountX; j++) // x
		{
			// get bmp
			bmp = GetTexture(i * m_nTilesCountX + j);

			// lock for write
			success = geBitmap_LockForWriteFormat(bmp, &Lock, 0, 0, format);
			if(!success)
			{
				geBitmap_SetFormat(bmp,format,GE_TRUE,0,NULL);
				geBitmap_LockForWriteFormat(bmp,&Lock,0,0, format);
			}
			QXASSERT(success);

			success = geBitmap_GetInfo(Lock, &Info, (geBitmap_Info*) NULL);
			QXASSERT(success);

			bpp = gePixelFormat_BytesPerPel(Info.Format);
			bits = (uint8 *)geBitmap_GetBits(Lock);

			// for each height element under this texture
			for(y = 0; y < m_nHeightElementsPerTile; y ++)		//y
			{
				for(x = 0; x < m_nHeightElementsPerTile; x ++)		//x
				{

					TL = (float)slope_shade_map[i * m_nHeightMapWidth * m_nHeightElementsPerTile + j * m_nHeightElementsPerTile + y * m_nHeightMapWidth + x] * ONE_OVER_255;
					BL = (float)slope_shade_map[i * m_nHeightMapWidth * m_nHeightElementsPerTile + j * m_nHeightElementsPerTile + (y+1) * m_nHeightMapWidth + x] * ONE_OVER_255;
					TR = (float)slope_shade_map[i * m_nHeightMapWidth * m_nHeightElementsPerTile + j * m_nHeightElementsPerTile + y * m_nHeightMapWidth + (x+1)] * ONE_OVER_255;
					BR = (float)slope_shade_map[i * m_nHeightMapWidth * m_nHeightElementsPerTile + j * m_nHeightElementsPerTile + (y+1) * m_nHeightMapWidth + (x+1)] * ONE_OVER_255;


					// set up gourad shading edge lists
					float gradient;
					gradient = (BL - TL) / (float)TextelPerElem;
					shade = TL;
					for(v = 0; v < TextelPerElem; v ++)	// left edge list, shade from TL to BL.
					{
						left_edge_list[v] = shade;
						shade += gradient;
					}

					gradient = (BR - TR) / (float)TextelPerElem;
					shade = TR;
					for(v = 0; v < TextelPerElem; v ++)	// right edge list, shade from TR to BR.
					{
						right_edge_list[v] = shade;
						shade += gradient;
					}

					// for each textel on this height element
					for(v = 0; v < TextelPerElem; v ++)		//y
					{

						gradient = (right_edge_list[v] - left_edge_list[v]) / (float)TextelPerElem;
						shade = left_edge_list[v];

						for(u = 0; u < TextelPerElem; u ++)		//x
						{
							// this is soooo egregious.  lol.  move this all crap outside of the inner loop.
							bptr = bits + bpp * ((TextelPerElem * (y * TextelPerElem * m_nHeightElementsPerTile + x)) + (v * Info.Stride + u));
							bptr2 = bptr;

							gePixelFormat_GetColor(format, &bptr, &R, &G, &B, &A);

							R = R * (int)((shade + fAmbient) * fIntensity);
							G = G * (int)((shade + fAmbient) * fIntensity);
							B = B * (int)((shade + fAmbient) * fIntensity);

							gePixelFormat_PutColor(format, &bptr2, R, G, B, A);

							shade += gradient;
						}
					}
				}
			}
			// unlock
			success = geBitmap_UnLock(Lock);
			QXASSERT(success);

			// refresh mips
			success = geBitmap_RefreshMips(bmp);
			QXASSERT(success);

			//	TEST - final format used doesnt appear to affect framerate.
			// BUT, without does slow down the 12mb v2, cause of texture mem issues.
			// so might as well use 8-bit for Glide.  eh hurts tex quality tho.

			gePixelFormat newformat = 	(gePixelFormat)TERRAIN_FINALFORMAT;
			success = geBitmap_SetFormat(bmp, newformat, GE_FALSE, 0, (geBitmap_Palette *)NULL);
			QXASSERT(success);
		}
	}


	free(slope_shade_map);
	free(left_edge_list);
	free(right_edge_list);


}

//
// Fractal functions
//

bool qxTerrainMap::FractalGenerate()
{
	int	x, z;
	int	oddline;
	float ratio, scale;

	// initialize random number generator
	if( !m_nFractalSeed )
		srand((unsigned) time(NULL));
	else
		srand (m_nFractalSeed);

	/*
	Set up our roughness constants.
	Random numbers are always generated in the range 0.0 to 1.0.
	'scale' is multiplied by the randum number.
	'ratio' is multiplied by 'scale' after each iteration
	to effectively reduce the randum number range.
	*/

	ratio = (float) pow (2.,(double)-m_fFractalHeight);
	scale = m_nFractalScale * ratio;

	/* Seed the first four values. For example, in a 4x4 array, we
	would initialize the data points indicated by '*':

	*   .   .   .   *

	.   .   .   .   .

	.   .   .   .   .

	.   .   .   .   .

	*   .   .   .   *

	In terms of the "diamond-square" algorithm, this gives us
	"squares".

	*/

	int nLines	= m_nHeightMapWidth-1;
	int nMesh	= m_nHeightMapWidth;

	int Stride	= nLines / 2;

	SetElementHeight( 0, 0, m_nFractalDefaultHeight );
	SetElementHeight( nLines, 0 , m_nFractalDefaultHeight );
	SetElementHeight( 0, nLines,  m_nFractalDefaultHeight );
	SetElementHeight( nLines, nLines,  m_nFractalDefaultHeight );

	//
	// Make the edges match the neighboring map, if any
	//
	qxTerrainMapBase* pEast		= CCD->TerrainMgr()->GetEastNeighbor(this);
	qxTerrainMapBase* pWest		= CCD->TerrainMgr()->GetWestNeighbor(this);
	qxTerrainMapBase* pNorth	= CCD->TerrainMgr()->GetNorthNeighbor(this);
	qxTerrainMapBase* pSouth	= CCD->TerrainMgr()->GetSouthNeighbor(this);

	// 0, 0
	// Western neighbor's eastern edge
	if( pWest )
	{
		int h = pWest->GetElementHeightUnscaled( 0, nLines );
		SetElementHeight(0,0, h);
	}

	// 0, Max
	// Southern neighbor's northern edge
	if( pSouth )
	{
		int h = pSouth->GetElementHeightUnscaled( nLines, nLines );
		SetElementHeight(0,nLines, h);

	}

	// Max, 0
	// Northern neighbor's southern edge
	if( pNorth )
	{
		int h = pNorth->GetElementHeightUnscaled( 0, 0 );
		SetElementHeight(nLines, 0, h);
	}

	// Max, Max
	// Eastern neighbor's Western edge
	if( pEast )
	{
		int h = pEast->GetElementHeightUnscaled( nLines, 0 );
		SetElementHeight(nLines,nLines, h);
	}

	/*
	Now we add ever-increasing detail based on the "diamond" seeded
	values. We loop over Stride, which gets cut in half at the
	bottom of the loop. Since it's an int, eventually division by 2
	will produce a zero result, terminating the loop.
	*/

	while (Stride)
	{
	/* Take the existing "square" data and produce "diamond"
	data. On the first pass through with a 4x4 matrix, the
	existing data is shown as "X"s, and we need to generate the
	"*" now:

	X   .   .   .   X

	.   .   .   .   .

	.   .   *   .   .

	.   .   .   .   .

	X   .   .   .   X

	It doesn't look like diamonds. What it actually is, for the
	first pass, is the corners of four diamonds meeting at the
	center of the array.

	*/

		for (x=Stride; x<nLines; x+=Stride)
		{
			for (z=Stride; z<nLines; z+=Stride)
			{
				m_pHeightMapData[(z * nMesh) + x] =
					(int)(scale * fractRand(0.50f) +
					FractalAvgSquareVals (x, z, Stride, nMesh));
				z += Stride;
			}
			x += Stride;
		}

		/* Take the existing "diamond" data and make it into
		"squares". Back to our 4X4 example: The first time we
		encounter this code, the existing values are represented by
		"X"s, and the values we want to generate here are "*"s:

		X   .   *   .   X

		.   .   .   .   .

		*   .   X   .   *

		.   .   .   .   .

		X   .   *   .   X

		x and z represent our (x,y) position in the array. The
		first value we want to generate is at (x=2,z=0), and we use
		"oddline" and "Stride" to increment z to the desired value.
		*/
		oddline = 0;
		for (x=0; x<=nLines; x+=Stride)
		{
			oddline = (oddline == 0);

			for (z=0; z<=nLines; z+=Stride)
			{
				if ((oddline) && !z)
					z+=Stride;

				// Eastern edge
				if( z == nLines )
				{
					// Eastern neighbor's western edge
					if( pEast )
						m_pHeightMapData[(z * nMesh) + x] =
							pEast->GetElementHeightUnscaled( x, 0 );

					else
						m_pHeightMapData[(z * nMesh) + x] =
							m_nFractalDefaultHeight;//scale * fractRand(0.50f) + m_nFractalDefaultHeight;
				}
				// Western edge
				else if( z == 0 )
				{
					// Western neighbor's eastern edge
					if( pWest )
						m_pHeightMapData[(z * nMesh) + x] =
							pWest->GetElementHeightUnscaled( x, nLines );

					else
						m_pHeightMapData[(z * nMesh) + x] =
							m_nFractalDefaultHeight;//scale * fractRand(0.50f) + m_nFractalDefaultHeight;

				}

				// Southern edge
				else if(  x == 0  )
				{
					// Southern neighbor's northern edge
					if( pSouth )
						m_pHeightMapData[(z * nMesh) + x] =
							pSouth->GetElementHeightUnscaled( nLines, z);

					else
						m_pHeightMapData[(z * nMesh) + x] =
						m_nFractalDefaultHeight;//scale * fractRand(0.50f) + m_nFractalDefaultHeight;


				}

				// Northern edge
				else if( x == nLines )
				{
					// Northern neighbor's southern edge
					if( pNorth )
						m_pHeightMapData[(z * nMesh) + x] =
							pNorth->GetElementHeightUnscaled(0, z);
					else
						m_pHeightMapData[(z * nMesh) + x] =
							m_nFractalDefaultHeight;//scale * fractRand(0.50f) + m_nFractalDefaultHeight;


				}


				// x and z are setup. Call avgDiamondVals with the
				// current position. It will return the average of the
				// surrounding diamond data points.

				else
					m_pHeightMapData[(z * nMesh) + x] =
					(int)(scale * fractRand(0.50f) +
					FractalAvgDiamondVals (x, z, Stride, nLines, nMesh));



				z+=Stride;
			}
		}


		// reduce random number range.
		scale *= ratio;
		Stride >>= 1;
	}




	return true;
}

/*
 * avgDiamondVals - Given the x,z location as the center of a diamond,
 * average the data values at the four corners of the diamond and
 * return it. "Stride" represents the distance from the diamond center
 * to a diamond corner.
 */
int qxTerrainMap::FractalAvgDiamondVals (int x, int z, int Stride, int nLines, int nMesh )
{
    /* In this diagram, our input Stride is 1, the x,z location is
       indicated by "X", and the four value we want to average are
       "*"s:
           .   *   .

           *   X   *

           .   *   .
       */

	return ((int) ((float)(m_pHeightMapData[((z-Stride)*nMesh) + x] +
			 m_pHeightMapData[((z+Stride)*nMesh) + x] +
			 m_pHeightMapData[(z*nMesh) + x-Stride] +
			 m_pHeightMapData[(z*nMesh) + x+Stride]) * .25f));
}


/*
 * avgSquareVals - Given the x,z location as the center of a square,
 * average the data values at the four corners of the square and return
 * it. "Stride" represents half the length of one side of the square.
 *
 * Called by fill2DFractArray.
 */
int qxTerrainMap::FractalAvgSquareVals ( int x, int z, int Stride, int nMesh )
{
    /* In this diagram, our input Stride is 1, the x,z location is
       indicated by "*", and the four value we want to average are
       "X"s:
           X   .   X

           .   *   .

           X   .   X
       */
    return ((int)((float)(m_pHeightMapData[((z-Stride)*nMesh) + x-Stride] +
		     m_pHeightMapData[((z-Stride)*nMesh) + x+Stride] +
		     m_pHeightMapData[((z+Stride)*nMesh) + x-Stride] +
		     m_pHeightMapData[((z+Stride)*nMesh) + x+Stride]) * .25f));
}


int16 qxTerrainMap::GetElementHeightUnscaled( int x, int z )
{

	QXASSERT(x < m_nHeightMapWidth && x >= 0);
	QXASSERT(z < m_nHeightMapLength && z >= 0);
	QXASSERT(m_pHeightMapData);

	return (int)((float)m_pHeightMapData[(m_nHeightMapLength * z) + x]
			/ CCD->TerrainMgr()->GetScaleY());
}
