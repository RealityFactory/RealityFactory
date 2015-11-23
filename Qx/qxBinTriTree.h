// qxBinTriTree.h
//
// Original code: GenScape (c) 1999, Lucas Ackerman
//
// Author: Jeff Thelen
//
// Copyright 2000 Quixotic, Inc.  All Rights Reserved.
//////////////////////////////////////////////////////////////////////


#ifndef qxBinTriTree_H
#define qxBinTriTree_H


class qxTerrainVert;
class qxTerrainMapBase;
class qxTerrainPoly;


class qxBinTriTree
{
public:

	qxBinTriTree(){};
	
	// left, right, top verts of the base tri.
	qxBinTriTree(	qxTerrainMapBase* newOwner, 
					qxTerrainVert* lv, 
					qxTerrainVert* rv, 
					qxTerrainVert* tv);
	
	~qxBinTriTree();


	uint8 * m_pVarianceTable;//[256]; // 256 == max depth = 7
	bool *	m_pFinishedTable;//[256];	// which v's are in the table.  0 == missing, 1 == entered

	void		CalcVarianceTable();			// fills variance table
	int			LookupVariance(int id);		// retrieves variance from table

	void		Update();

private:
	// for calculating a triangle's variance, will add it to 
	// vtable if it doesnt exist (as determined by finished_table)
	// take's id# and heightmap x,z location of each vert (left, right, top) 
	// of the tri.
	int	CalcVariance(	int id, int lx, int lz, int rx, int rz, 
						int tx, int tz);

	qxTerrainMapBase*	m_pOwner;
	qxTerrainPoly*		m_pTerrainPolyRoot;

	friend class qxTerrainTile;

};


#endif

