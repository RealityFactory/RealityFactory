// qxTerrainPoly.h
//
// Original code: GenScape (c) 1999, Lucas Ackerman
//
// Author: Jeff Thelen
//
// Copyright 2000 Quixotic, Inc.  All Rights Reserved.
//////////////////////////////////////////////////////////////////////


#ifndef qxTerrainPoly_H
#define qxTerrainPoly_H


class qxTerrainVert;
class qxTerrainMapBase;
class qxBinTriTree;
class qxMergeQueue;
class qxSplitQueue;
class qxSplitQueueNode;
class qxMergeQueueNode;

class qxTerrainPoly
{

public:

	qxTerrainPoly();
	qxTerrainPoly(	qxTerrainMapBase* pParent,
					qxTerrainVert* lv, 
					qxTerrainVert* rv, 
					qxTerrainVert* tv,
					qxBinTriTree* pTree,
					int nTreeID
					);

	~qxTerrainPoly();

	void ClearChildTree();
	void Render();
	void RenderWireframe();

	// properly sets world-space Position vect based on vert locations
	void UpdatePosition();	

	// recalc wedge bounding box, for view-frustrum culling
	void UpdateWedgeBounds();	
	// recursively sets all view-frustrum flags
	void UpdateViewFlags(); 

	// calculates tri priority based on distance from camera & other factors
	// if less than nNearestIsHighest, we force the highest priority
	int CalcPriority(int nNearestIsHighest = 0);	



private:
	// test if the wedge bounding box is inside of the halfspace defined by this point 
	// & normal.
	// uses standard point-normal definition of a plane, dot-product inside-outside test
	// returns 0 if tri is inside the halfspace, 1 if not, and 2 if completely outside.
	bool TestHalfspace(	geVec3d *Point, geVec3d *Normal, 
						int Flag, bool& bAllInFlag);

	// these recurse over the tree and accordingly set VF_ flags 
	// (but not VF_In_ flags!) to a tri and all is children
	void SetViewFlagsAllOut();	
	void SetViewFlagsAllIn();

	geVec3d Position;	// 3d point, center of polygon.  used for priority calc.

	geVec3d LU, LD, RU, RD, TU, TD;		// bounding wedge corners.  left/right/top tri corners, and up/down.

	// // == True if poly is rendered, else == False if has subpolys rendered instead
	bool				m_bActive;	
					
	qxTerrainPoly*		m_pNext;	// for linked lists
	int					m_nTreeID;	// for variance table lookup
	qxBinTriTree*		m_pOwnerTree;


	// right-isosceles triangle, hypoteneuse down, verts are left, right, top.
/*

				* top vert
			  /	| \
		Ln	/	|   \  Rn
		  /	 Lc	| Rc  \
		/		|		\
left  *-------------------*  right  
				Bn
*/

	qxTerrainVert* m_pLeftVert;	// lv
	qxTerrainVert* m_pRightVert;	// rv
	qxTerrainVert* m_pTopVert;		// tv

	/* Sub_Vert -child vertex- will be the "Top_Vert" of it's child tri's.
	CODE THIS, since no tri manages its own verts (each vert is managed by 
	the next-higher structure,
	whether its the parent landscape (for the top-level polys), or it's parent.
	*/
	qxTerrainVert* m_pSubVert;		

	qxTerrainPoly* m_pParent;	

	qxTerrainPoly* m_pLeftChild;	
	qxTerrainPoly* m_pRightChild;	

	qxTerrainPoly* m_pLeftNeighbor;	
	qxTerrainPoly* m_pRightNeighbor;	
	qxTerrainPoly* m_pBottomNeighbor;	

	qxTerrainMapBase* m_pTerrainMap;

	// split/merge queue nodes this tri is in
	qxSplitQueueNode*	m_pSplitQueueNode;
	qxMergeQueueNode*	m_pMergeQueueNode;


	// halfspace label bit flags
	#define VF_IN_FAR		(1<<0)
	#define VF_IN_NEAR		(1<<1)
	#define VF_IN_TOP		(1<<2)
	#define VF_IN_BOTTOM	(1<<3)
	#define VF_IN_LEFT		(1<<4)
	#define VF_IN_RIGHT		(1<<5)

	
	// view frustrum half space flags
	int VF_Halfspace_Flags;

	// these 3 could use the last 2 bits in the first byte flag^^^,
	// but its probably simpler just to keep em seperate
	// overall view frustrum label
	#define VF_OUT			(1<<0)
	#define VF_ALL_IN		(1<<1)
	#define VF_DONT_KNOW	(1<<2)

	int		VF_Overall;


	
	friend class	qxTerrainTile;
	friend class	qxBinTriTree;
	friend class	qxPolyPool;
	friend class	qxMergeQueue;
	friend class	qxSplitQueue;
	friend class	qxTerrainMapBase;

};



#endif

