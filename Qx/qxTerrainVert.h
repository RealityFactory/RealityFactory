// qxTerrainVert.h
//
// Original code: GenScape (c) 1999, Lucas Ackerman
//
// Author: Jeff Thelen
//
// Copyright 2000 Quixotic, Inc.  All Rights Reserved.
//////////////////////////////////////////////////////////////////////

#ifndef qxTerrainVert_H
#define qxTerrainVert_H


class qxTerrainVert
{
public:
	qxTerrainVert();
	~qxTerrainVert() {}

	GE_LVertex		CurrentVert;		// this vert will be the one rendered
	geVec3d			ProjectedVert;		// T&P'd camera-space location
	qxTerrainVert*	m_pNext;		// for linked list
	int hX;
	int hZ;						// (width, height) vertex location in Heightmap (x,y)

private:

};


#endif