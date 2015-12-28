// qxTerrainTile.cpp: implementation of the qxTerrain class.
//
// Original code: GenScape (c) 1999, Lucas Ackerman
//
// Author: Jeff Thelen
//
// Copyright 2000 Quixotic, Inc.  All Rights Reserved.
//////////////////////////////////////////////////////////////////////

#include "..\\RabidFramework.h"
#include "qxTerrainMapBase.h"
#include "qxBinTriTree.h"
#include "qxTerrainPoly.h"
#include "qxTerrainTile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

qxTerrainTile::qxTerrainTile(	qxTerrainMapBase* pOwner,
								qxTerrainVert* tl,
								qxTerrainVert* tr,
								qxTerrainVert* bl,
								qxTerrainVert* br)
:m_pOwner(pOwner)
,TL(tl)
,TR(tr)
,BL(bl)
,BR(br)
,m_pBinTreeRT(0)
,m_pBinTreeLB(0)
{
	// l, r, t, tri verts
	// make the diamond
	m_pBinTreeRT = new qxBinTriTree(m_pOwner, TL, BR, TR);
	m_pBinTreeLB = new qxBinTriTree(m_pOwner, BR, TL, BL);

	m_pBinTreeRT->m_pTerrainPolyRoot->m_pBottomNeighbor = m_pBinTreeLB->m_pTerrainPolyRoot;
	m_pBinTreeLB->m_pTerrainPolyRoot->m_pBottomNeighbor = m_pBinTreeRT->m_pTerrainPolyRoot;

	return;
}


qxTerrainTile::~qxTerrainTile()
{
	delete m_pBinTreeRT;
	delete m_pBinTreeLB;
}

