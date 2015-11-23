// qxTerrainTile.h
//
// Original code: GenScape (c) 1999, Lucas Ackerman
//
// Author: Jeff Thelen
//
// Copyright 2000 Quixotic, Inc.  All Rights Reserved.
//////////////////////////////////////////////////////////////////////


#ifndef qxTerrainTile_H
#define qxTerrainTile_H

class qxTerrainPoly;
class qxTerrainVert;
class qxTerrainMapBase;

#include "qxBinTriTree.h"
#include "qxTerrainPoly.h"


// a qxTerrainTile is one tile in the grid of qxBinTriTree's in a qxLandscape
/*

	UL			UR
	*-----------*
	| \	   		|
	|	\ 	T_R |
	|	  \		|
	| L_B	\	|
	|  		  \	|
	*-----------*
	BL			BR

*/

class qxTerrainTile	//== 2 bintritree's
{

public:
	qxTerrainTile(){};
	qxTerrainTile(qxTerrainMapBase* Owner, qxTerrainVert* tl, qxTerrainVert* tr, qxTerrainVert* bl, qxTerrainVert* br);
	~qxTerrainTile();



	// return's the respective vertex
	qxTerrainVert* GetTL(void)	{ return TL;	}
	qxTerrainVert* GetTR(void)	{ return TR; }
	qxTerrainVert* GetBL(void)	{ return BL;	}
	qxTerrainVert* GetBR(void)	{ return BR; }
	

	// These are only called by the TerrainMap. Here for convenience.
	bool IsLBRootOut()	{ return (m_pBinTreeLB->m_pTerrainPolyRoot->VF_Overall == VF_OUT ); }
	bool IsRTRootOut()	{ return (m_pBinTreeRT->m_pTerrainPolyRoot->VF_Overall == VF_OUT ); }
	
	void LBRender()		{ m_pBinTreeLB->m_pTerrainPolyRoot->Render(); }
	void RTRender()		{ m_pBinTreeRT->m_pTerrainPolyRoot->Render(); }

	void LBRenderWireFrame() { m_pBinTreeLB->m_pTerrainPolyRoot->RenderWireframe(); }
	void RTRenderWireFrame() { m_pBinTreeRT->m_pTerrainPolyRoot->RenderWireframe(); }

	void UpdateViewFlagsLB() { m_pBinTreeLB->m_pTerrainPolyRoot->UpdateViewFlags(); }
	void UpdateViewFlagsRT() { m_pBinTreeRT->m_pTerrainPolyRoot->UpdateViewFlags(); }

	qxTerrainPoly* LBGetTerrainPoly() { return m_pBinTreeLB->m_pTerrainPolyRoot; }
	qxTerrainPoly* RTGetTerrainPoly() { return m_pBinTreeRT->m_pTerrainPolyRoot; }

	void Reset() 
	{ 
		m_pBinTreeLB->m_pTerrainPolyRoot->ClearChildTree(); 
		m_pBinTreeRT->m_pTerrainPolyRoot->ClearChildTree(); 

		m_pBinTreeLB->m_pTerrainPolyRoot->m_pBottomNeighbor = m_pBinTreeRT->m_pTerrainPolyRoot;
		m_pBinTreeRT->m_pTerrainPolyRoot->m_pBottomNeighbor = m_pBinTreeLB->m_pTerrainPolyRoot;
	}

	void Update()
	{
		m_pBinTreeRT->Update();
		m_pBinTreeLB->Update();
	}

	/*	these are for cross-linking the BinTriTree neighbor tri's 
	at the highest level
	so the entire mesh of tiles will tesselate together.
	each Link_ fn links both m_pRootTri's to each other, 
	so calling _Top and _Bottom 
	on the same 2 tri's is not necessary.
	*/
	void LinkTop(qxTerrainTile *tile)
	{
		m_pBinTreeRT->m_pTerrainPolyRoot->m_pLeftNeighbor = tile->m_pBinTreeLB->m_pTerrainPolyRoot;
		tile->m_pBinTreeLB->m_pTerrainPolyRoot->m_pLeftNeighbor = m_pBinTreeRT->m_pTerrainPolyRoot;
	}
	
	void LinkBottom(qxTerrainTile *tile)
	{
		m_pBinTreeLB->m_pTerrainPolyRoot->m_pLeftNeighbor = tile->m_pBinTreeRT->m_pTerrainPolyRoot;
		tile->m_pBinTreeRT->m_pTerrainPolyRoot->m_pLeftNeighbor = m_pBinTreeLB->m_pTerrainPolyRoot;
	}
	
	void LinkLeft(qxTerrainTile *tile)
	{
		m_pBinTreeLB->m_pTerrainPolyRoot->m_pRightNeighbor = tile->m_pBinTreeRT->m_pTerrainPolyRoot;
		tile->m_pBinTreeRT->m_pTerrainPolyRoot->m_pRightNeighbor = m_pBinTreeLB->m_pTerrainPolyRoot;
	}
	
	void LinkRight(qxTerrainTile *tile)
	{
		m_pBinTreeRT->m_pTerrainPolyRoot->m_pRightNeighbor = tile->m_pBinTreeLB->m_pTerrainPolyRoot;
		tile->m_pBinTreeLB->m_pTerrainPolyRoot->m_pRightNeighbor = m_pBinTreeRT->m_pTerrainPolyRoot;
	}



	
private:

	qxTerrainMapBase*	m_pOwner;
	qxBinTriTree*		m_pBinTreeRT;	
	qxBinTriTree*		m_pBinTreeLB;

	qxTerrainVert *TL, *TR, *BL, *BR; // 4 corner vert ptrs.  top/bottom left/right


};



#endif
