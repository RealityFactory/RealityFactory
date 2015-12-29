// qxBinTriTree.cpp: implementation of the qxBinTriTree class.
//
// Original code: GenScape (c) 1999, Lucas Ackerman
//
// Author: Jeff Thelen
//
// Copyright 2000 Quixotic, Inc.  All Rights Reserved.
//////////////////////////////////////////////////////////////////////

#include "..\\RabidFramework.h"
#include "qxBinTriTree.h"
#include "qxTerrainVert.h"
#include "qxTerrainTile.h"
#include "qxTerrainMapBase.h"
#include "qxTerrainPoly.h"

#ifndef QXASSERT
#include <assert.h>
#define QXASSERT assert
#endif

qxBinTriTree::qxBinTriTree(qxTerrainMapBase* newOwner,
						   qxTerrainVert* lv,
						   qxTerrainVert* rv,
						   qxTerrainVert* tv)
: m_pOwner(newOwner)

{

	m_pVarianceTable = new uint8[ m_pOwner->GetMaxVarianceLookupId() + 1];
	m_pFinishedTable = (bool*) NULL;

	m_pTerrainPolyRoot = new qxTerrainPoly(m_pOwner, lv, rv, tv, this, 1);
	m_pTerrainPolyRoot->UpdatePosition();
	m_pTerrainPolyRoot->m_bActive = true;

	CalcVarianceTable();

	// NOTE: UpdateWedgeBounds() MUST happen AFTER the var table is calc'd,
	//	cause variance is needed to calc the wedge bounding box
	m_pTerrainPolyRoot->UpdateWedgeBounds();

	return;
}

qxBinTriTree::~qxBinTriTree()
{
	delete m_pTerrainPolyRoot;
	delete [] m_pVarianceTable;
}

//
// We only call this when shifting water maps for dynamic mapping
//
void qxBinTriTree::Update()
{

	m_pTerrainPolyRoot->UpdatePosition();
	m_pTerrainPolyRoot->UpdateWedgeBounds();

}

void qxBinTriTree::CalcVarianceTable(void)
{
	int Max = m_pOwner->GetMaxVarianceLookupId();
	m_pFinishedTable = new bool[ Max + 1 ];

	// first, clear existing table
	for(int i = 0; i < Max; i++)
		m_pFinishedTable[i] = 0;		// no elements filled yet

/*	one CalcVar on the root tri requires that all the other tri's in the tree
	also be calc'd because its a recursive process,
	where a parent's V must be >= all its children because it includes them
	(accounts for possible visual error of all children)
*/
	CalcVariance(m_pTerrainPolyRoot->m_nTreeID,
				 m_pTerrainPolyRoot->m_pLeftVert->hX,
				 m_pTerrainPolyRoot->m_pLeftVert->hZ,
				 m_pTerrainPolyRoot->m_pRightVert->hX,
				 m_pTerrainPolyRoot->m_pRightVert->hZ,
				 m_pTerrainPolyRoot->m_pTopVert->hX,
				 m_pTerrainPolyRoot->m_pTopVert->hZ);

	delete m_pFinishedTable;
}

int qxBinTriTree::CalcVariance( int id,
								 int lx,
								 int lz,
								 int rx,
								 int rz,
								 int tx,
								 int tz)
{

	// if tri already has variance calc'd, then get it from the table.
	if(m_pFinishedTable[ id-1 ] == 1)
		return LookupVariance(id);

	// else, calc it and add to the table.

	// the map height at the middle of the hypotenuse
	float RealHeight = m_pOwner->GetElementHeight( (lx+rx)/2, (lz+rz)/2 );

	// the average of the real heights at the two ends of the hypot
	float AvgHeight = (float)(	m_pOwner->GetElementHeight(lx,lz) + m_pOwner->GetElementHeight(rx,rz) )	 / 2;


	int v = (int)fabs( RealHeight - AvgHeight );

	// calcVar for children
	// 2^7 for max_depth=8, next depth == 8.  128 is the min id#
	// for a node at the deepest level (7).

	if(id < ((m_pOwner->GetMaxVarianceLookupId() + 1)/2))
	{
		// left child
		v = max( v, CalcVariance((id * 2), tx, tz, lx, lz, (lx+rx)/2, (lz+rz)/2) );
		// right child
		v = max( v, CalcVariance((id * 2 + 1), rx, rz, tx, tz, (lx+rx)/2, (lz+rz)/2) );
	}

	if(v > 255)
		v = 255;

	m_pVarianceTable[id - 1] = v;	// store the variance
	m_pFinishedTable[id - 1] = 1;	// this element is entered

	return v;
}

int qxBinTriTree::LookupVariance(int id)
{
	QXASSERT(id != 0);
	QXASSERT(id <= m_pOwner->GetMaxVarianceLookupId() );
	return m_pVarianceTable[id - 1];
}

