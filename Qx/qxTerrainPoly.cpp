// qxBinTriTree.cpp: implementation of the qxBinTriTree class.
//
// Original code: GenScape (c) 1999, Lucas Ackerman
//
// Author: Jeff Thelen
//
// Copyright 2000 Quixotic, Inc.  All Rights Reserved.
//////////////////////////////////////////////////////////////////////

#include "..\\RabidFramework.h"
#include "qxTerrainMapBase.h"
#include "qxTerrainPoly.h"
#include "qxTerrainVert.h"
#include "qxBinTriTree.h"
#include "QxUser.h"


qxTerrainPoly::qxTerrainPoly()
:m_bActive( true )
,m_pSubVert(0)
,m_pTerrainMap(0)
,m_pParent(0)
,m_pOwnerTree(0)
,m_pLeftChild(0)
,m_pRightChild(0)
,m_pLeftNeighbor(0)
,m_pRightNeighbor(0)
,m_pBottomNeighbor(0)
,m_pSplitQueueNode(0)
,m_pMergeQueueNode(0)
,VF_Overall(VF_OUT)
,m_pLeftVert(0)
,m_pRightVert(0)
,m_pTopVert(0)
,m_nTreeID(0)
,m_pNext(0)
{
	// NOTE: left, right, and top verts must be assigned manually if
	// this default constructor is called
}

qxTerrainPoly::qxTerrainPoly(qxTerrainMapBase* pTerrainMap,
							qxTerrainVert *lv,
							qxTerrainVert *rv,
							qxTerrainVert *tv,
							qxBinTriTree* pTree,
							int nTreeID
							)
:m_bActive( true )
,m_pSubVert(0)
,m_pTerrainMap(pTerrainMap)
,m_pOwnerTree(pTree)
,m_pLeftChild(0)
,m_pRightChild(0)
,m_pLeftNeighbor(0)
,m_pRightNeighbor(0)
,m_pBottomNeighbor(0)
,m_pSplitQueueNode(0)
,m_pMergeQueueNode(0)
,VF_Overall(VF_OUT)
,m_pLeftVert(lv)
,m_pRightVert(rv)
,m_pTopVert(tv)
,m_nTreeID(nTreeID)
,m_pNext(0)
,m_pParent(0)
{
}


qxTerrainPoly::~qxTerrainPoly()
{
	if(m_bActive)	// no sub polys
		return;

	if(m_pLeftChild)	// should be NULL if poly is !m_bActive
		delete m_pLeftChild;
	if(m_pRightChild)
		delete m_pRightChild;
	if(m_pSubVert)
		delete m_pSubVert;
}


void qxTerrainPoly::ClearChildTree(void)
{
	if(!m_bActive)
	{
		m_bActive = true;

		// cant just pool these, cause the whole TREE must be pooled.
		// try recursive call, then pool.  linkage should work out

		m_pLeftChild->ClearChildTree();
		m_pRightChild->ClearChildTree();

		m_pTerrainMap->PutPoly(&(m_pLeftChild));
		m_pTerrainMap->PutPoly(&(m_pRightChild));
		m_pTerrainMap->PutVert(&(m_pSubVert));
	}

}


// recursive tree render
void qxTerrainPoly::Render( )
{
	// if this poly is outside the view frustrum, dont render it
	if(VF_Overall == VF_OUT)
	{
		return;
	}

	if(m_bActive)
	{

	/*	TEST: 2d backface removal method.
	might as well use 2d, because its faster and verts are already in screenspace anyway
	this actually calculates the z-component of the 3d cross-product of 2 edges of a triangle.

	2d points: (x1,y1), (x2,y2), (x3,y3) = the first three points of the polygon.
	left, right, top.

	float fBackface = (x1-x2)*(y3-y2)-(y1-y2)*(x3-x2);
	If fBackface is positive the polygon is visible.

	p1 = top, p2 = left, p3 = right.  so, counterclockwise vert order:
	*/
	// this works niiiiicely.  much faster.  2 multiplications & 5 subtractions.

		float fBackface = ((m_pTopVert->ProjectedVert.X - m_pLeftVert->ProjectedVert.X)
			* (m_pRightVert->ProjectedVert.Y - m_pLeftVert->ProjectedVert.Y))
			- ((m_pTopVert->ProjectedVert.Y - m_pLeftVert->ProjectedVert.Y)
			* (m_pRightVert->ProjectedVert.X - m_pLeftVert->ProjectedVert.X));


		if( !m_pTerrainMap->GetRenderBackFace() )
		{
			if( fBackface <= 0)	// is < faster or slower than <= ???, or is there a quicker negative test?
				return;
		}
		else if(fBackface > 0)
			return;

		GE_LVertex verts[3];
		memset((void*) &verts[0], 0, sizeof(GE_LVertex));
		memset((void*) &verts[1], 0, sizeof(GE_LVertex));
		memset((void*) &verts[2], 0, sizeof(GE_LVertex));

		memcpy((void*) &verts[0].X, (void*) &m_pLeftVert->ProjectedVert.X, sizeof(float) * 3);
		memcpy((void*) &verts[1].X, (void*) &m_pRightVert->ProjectedVert.X, sizeof(float) * 3);
		memcpy((void*) &verts[2].X, (void*) &m_pTopVert->ProjectedVert.X, sizeof(float) * 3);

		memcpy((void*) &verts[0].u, (void*) &m_pLeftVert->CurrentVert.u, sizeof(float) * 6);
		memcpy((void*) &verts[1].u, (void*) &m_pRightVert->CurrentVert.u, sizeof(float) * 6);
		memcpy((void*) &verts[2].u, (void*) &m_pTopVert->CurrentVert.u, sizeof(float) * 6);

		// if the triangle is completely onscreen, then
		//its not necessary to do complete clipping on it
		if(VF_Overall == VF_ALL_IN)
		{
			geTClip_UnclippedTriangle(verts);
			return;
		}

/*
		verts[0].r = 255.0f; verts[0].b = 255.0f; verts[0].g = 255.0f;
		verts[1].r = 255.0f; verts[1].b = 255.0f; verts[1].g = 255.0f;
		verts[2].r = 255.0f; verts[2].b = 255.0f; verts[2].g = 255.0f;
		verts[0].a = 255.0f; verts[1].a = 255.0f; verts[2].a = 255.0f;
*/
		geTClip_UnclippedTriangle(verts);

		//geEngine_RenderPoly(CCD->Engine()->Engine(), (GE_TLVertex *)verts, 3, CCD->TerrainMgr()->GetTexture(),
			//CCD->TerrainMgr()->GetRender());

		//geTClip_Triangle(verts);
		return;
	}

	// light the sub vertex, T&P sub vert, render children

	m_pTerrainMap->LightVertex(m_pSubVert);

	geCamera_TransformAndProject(CCD->CameraManager()->Camera(),
								(geVec3d *)&(m_pSubVert->CurrentVert),
								(geVec3d *)&(m_pSubVert->ProjectedVert));

	m_pLeftChild->Render();
	m_pRightChild->Render();
}


void qxTerrainPoly::RenderWireframe(void)
{
	// if this poly is outside the view frustrum, dont render it
	if(VF_Overall == VF_OUT)
		return;

	if(m_bActive)
	{
		// the ++'s on vert coords here just offset one of the vertices, so we can see the line drawn.

		GE_LVertex verts[3];

		// left-right vert side
		memcpy((void*) &verts[0].X, (void*) &m_pLeftVert->ProjectedVert.X, sizeof(float) * 3);
		memcpy((void*) &verts[1].X, (void*) &m_pLeftVert->ProjectedVert.X, sizeof(float) * 3);
		memcpy((void*) &verts[2].X, (void*) &m_pRightVert->ProjectedVert.X, sizeof(float) * 3);

		memcpy((void*) &verts[0].u, (void*) &m_pLeftVert->CurrentVert.u, sizeof(float) * 6);
		memcpy((void*) &verts[1].u, (void*) &m_pLeftVert->CurrentVert.u, sizeof(float) * 6);
		memcpy((void*) &verts[2].u, (void*) &m_pRightVert->CurrentVert.u, sizeof(float) * 6);

		verts[0].X ++;
		verts[0].Y ++;
		verts[0].Z ++;

		//geTClip_UnclippedTriangle(verts);
		geTClip_Triangle(verts);

	// right-top vert side
		memcpy((void*) &verts[0].X, (void*) &m_pRightVert->ProjectedVert.X, sizeof(float) * 3);
		memcpy((void*) &verts[1].X, (void*) &m_pTopVert->ProjectedVert.X, sizeof(float) * 3);
		memcpy((void*) &verts[2].X, (void*) &m_pRightVert->ProjectedVert.X, sizeof(float) * 3);

		memcpy((void*) &verts[0].u, (void*) &m_pLeftVert->CurrentVert.u, sizeof(float) * 6);
		memcpy((void*) &verts[1].u, (void*) &m_pLeftVert->CurrentVert.u, sizeof(float) * 6);
		memcpy((void*) &verts[2].u, (void*) &m_pRightVert->CurrentVert.u, sizeof(float) * 6);

		verts[0].X ++;
		verts[0].Y ++;
		verts[0].Z ++;

		//geTClip_UnclippedTriangle(verts);
		geTClip_Triangle(verts);

	// top-left vert side
		memcpy((void*) &verts[0].X, (void*) &m_pTopVert->ProjectedVert.X, sizeof(float) * 3);
		memcpy((void*) &verts[1].X, (void*) &m_pLeftVert->ProjectedVert.X, sizeof(float) * 3);
		memcpy((void*) &verts[2].X, (void*) &m_pTopVert->ProjectedVert.X, sizeof(float) * 3);

		memcpy((void*) &verts[0].u, (void*) &m_pLeftVert->CurrentVert.u, sizeof(float) * 6);
		memcpy((void*) &verts[1].u, (void*) &m_pLeftVert->CurrentVert.u, sizeof(float) * 6);
		memcpy((void*) &verts[2].u, (void*) &m_pRightVert->CurrentVert.u, sizeof(float) * 6);

		verts[0].X ++;
		verts[0].Y ++;
		verts[0].Z ++;

		//geTClip_UnclippedTriangle(verts);
		geTClip_Triangle(verts);

		return;
	}

	//else, render sub-polys instead
	m_pTerrainMap->LightVertexWireFrame(m_pSubVert);


	// T&P sub vert, render children
	geCamera_TransformAndProject(CCD->CameraManager()->Camera(),
								(geVec3d *)&(m_pSubVert->CurrentVert),
								&(m_pSubVert->ProjectedVert));

	m_pLeftChild->RenderWireframe();
	m_pRightChild->RenderWireframe();
}


void qxTerrainPoly::UpdatePosition()
{
	Position.X = (m_pLeftVert->CurrentVert.X + m_pRightVert->CurrentVert.X + m_pTopVert->CurrentVert.X) * ONE_OVER_THREE;//OneOverThree;
	Position.Y = (m_pLeftVert->CurrentVert.Y + m_pRightVert->CurrentVert.Y + m_pTopVert->CurrentVert.Y) * ONE_OVER_THREE;//OneOverThree;
	Position.Z = (m_pLeftVert->CurrentVert.Z + m_pRightVert->CurrentVert.Z + m_pTopVert->CurrentVert.Z) * ONE_OVER_THREE;//OneOverThree;
}


void qxTerrainPoly::UpdateWedgeBounds()
{
	int variance;

/*	if the variance for this tri isn't stored, then make as small
	an acceptable box as possible
	NOTE: try experimenting with diff forced variance values here,
	so stuff close to camera won't be clipped out prematurely.  100 seems ok.
	very small values (like 1), should also work.  TEST this.
*/

	if(m_nTreeID > m_pTerrainMap->m_nMaxVarianceLookupId)
	{
		variance = 1;
	}
	else
	{
		// else, lookup the variance for a proper box
		variance = m_pOwnerTree->LookupVariance( m_nTreeID );
	}

	// create bounding wedge for this triangle.  contains all possible children as well (cause Y is +-variance)
	LU.X = m_pLeftVert->CurrentVert.X;
	LU.Z = m_pLeftVert->CurrentVert.Z;
	LU.Y = m_pLeftVert->CurrentVert.Y + variance;

	LD.X = m_pLeftVert->CurrentVert.X;
	LD.Z = m_pLeftVert->CurrentVert.Z;
	LD.Y = m_pLeftVert->CurrentVert.Y - variance;

	RU.X = m_pRightVert->CurrentVert.X;
	RU.Z = m_pRightVert->CurrentVert.Z;
	RU.Y = m_pRightVert->CurrentVert.Y + variance;

	RD.X = m_pRightVert->CurrentVert.X;
	RD.Z = m_pRightVert->CurrentVert.Z;
	RD.Y = m_pRightVert->CurrentVert.Y - variance;

	TU.X = m_pTopVert->CurrentVert.X;
	TU.Z = m_pTopVert->CurrentVert.Z;
	TU.Y = m_pTopVert->CurrentVert.Y + variance;

	TD.X = m_pTopVert->CurrentVert.X;
	TD.Z = m_pTopVert->CurrentVert.Z;
	TD.Y = m_pTopVert->CurrentVert.Y - variance;
}


void qxTerrainPoly::UpdateViewFlags(void)
{
	// new set of flags.  after calc'ing current VF pos,
	// compare these vs. whats already set.
	// only recurse to children if theyr not already set correctly.

	bool	New_All_In = true;

	// if this is a base-grid tri, then calc new flags from scratch
	if(m_pParent==NULL)
	{
		VF_Halfspace_Flags = 0;

		// test order should go from most-likely to be rejected, to least likely.
		// start with near, then l/r or t/b (not sure, test this for speed), then far.

		// test Near
		if(!TestHalfspace(&m_pTerrainMap->FrustrumNear, &m_pTerrainMap->NormalNear,
								VF_IN_NEAR, New_All_In))
		{
			return; // wedge was entirely outside the halfspace
		}

		//
		// test Left
		//
		if(!TestHalfspace(&m_pTerrainMap->FrustrumLeft,
								&m_pTerrainMap->NormalLeft,
								VF_IN_LEFT, New_All_In))
			return;

		//
		// test Right
		//

		if(!TestHalfspace(&m_pTerrainMap->FrustrumRight, &m_pTerrainMap->NormalRight,
			VF_IN_RIGHT, New_All_In ))
			return;

		//
		// test Top
		//
		if(! TestHalfspace(&m_pTerrainMap->FrustrumTop, &m_pTerrainMap->NormalTop,
							VF_IN_TOP, New_All_In))
			return;

		//
		// test Bottom
		//
		if( !TestHalfspace(&m_pTerrainMap->FrustrumBottom, &m_pTerrainMap->NormalBottom,
							VF_IN_BOTTOM, New_All_In))
			return;

		//
		// test Far
		//
		if(!TestHalfspace(&m_pTerrainMap->FrustrumFar, &m_pTerrainMap->NormalFar,
								VF_IN_FAR, New_All_In ))
			return;

		//
		// all halfspace tests are done, tri isnt all-out
		// so if all in, set flags, else, update children
		//
		if( New_All_In )
		{
			if((VF_Overall & VF_ALL_IN) == GE_TRUE)	// NOTE: does this ever happen?
			{
				return;		// tree is already properly set
			}

			// else
			VF_Overall = VF_ALL_IN;

			// set children
			if(m_bActive == GE_FALSE)
			{
				m_pLeftChild->SetViewFlagsAllIn();
				m_pRightChild->SetViewFlagsAllIn();
			}
			return;
		}

		//
		// tri isn't all in or all out, so update children
		//
		VF_Overall = VF_DONT_KNOW;

		if(!m_bActive)
		{
			m_pLeftChild->UpdateViewFlags();
			m_pRightChild->UpdateViewFlags();
		}

		return;
	} //if Parent == NULL


	//
	// else, tri is somewhere in the tree, so inherit parent's
	// _In flags, reevaluate not-In portions, etc.
	//
	else
	{

		// inherit parents flags.
		VF_Halfspace_Flags = m_pParent->VF_Halfspace_Flags;

		// all true VF_In_ flags are valid, because this tri is within
		// the parents bounding wedge
		// any false VF_In_ flags should be rechecked,
		// and then the overall In, Out or Dont Know updated and children updated

		// test Near
		if(! (VF_Halfspace_Flags & VF_IN_NEAR))
			 if( !TestHalfspace(&m_pTerrainMap->FrustrumNear, &m_pTerrainMap->NormalNear,
								VF_IN_NEAR, New_All_In) )
			return;


		//
		// test Left
		//
		if(! (VF_Halfspace_Flags & VF_IN_LEFT))
			if( !TestHalfspace(&m_pTerrainMap->FrustrumLeft, &m_pTerrainMap->NormalLeft,
								VF_IN_LEFT, New_All_In) )
				return;

		//
		// test Right
		//
		if(! (VF_Halfspace_Flags & VF_IN_RIGHT))
			if(!TestHalfspace(&m_pTerrainMap->FrustrumRight, &m_pTerrainMap->NormalRight,
									VF_IN_RIGHT, New_All_In) )
				return;

		//
		// test Top
		//
		if(! (VF_Halfspace_Flags & VF_IN_TOP))
			if( !TestHalfspace(&m_pTerrainMap->FrustrumTop, &m_pTerrainMap->NormalTop,
									VF_IN_TOP, New_All_In) )
				return;

		//
		// test Bottom
		//
		if(! (VF_Halfspace_Flags & VF_IN_BOTTOM))
			if( !TestHalfspace(&m_pTerrainMap->FrustrumBottom, &m_pTerrainMap->NormalBottom,
					VF_IN_BOTTOM, New_All_In) )
				return;

		//
		// test Far
		//
		if(! (VF_Halfspace_Flags & VF_IN_FAR))
			if( !TestHalfspace(&m_pTerrainMap->FrustrumFar, &m_pTerrainMap->NormalFar,
					VF_IN_FAR, New_All_In) )
					return;

		// all halfspace tests are done, tri isnt all-out,
		// so if all in, set flags, else, update children
		if( New_All_In )
		{
			if(VF_Overall & VF_ALL_IN)
				return;		// tree is already properly set

			// else
			VF_Overall = VF_ALL_IN;

			// set children
			if(m_bActive == GE_FALSE)
			{
				m_pLeftChild->SetViewFlagsAllIn();
				m_pRightChild->SetViewFlagsAllIn();
			}
			return;
		}

		// tri isn't all in or all out, so update children
		VF_Overall = VF_DONT_KNOW;

		if(m_bActive == GE_FALSE)
		{
			m_pLeftChild->UpdateViewFlags();
			m_pRightChild->UpdateViewFlags();
		}

		return;
	}

	QXASSERT(false);	// if it hasn't returned by here, there's a BUG!
}


// tests wedge bounds, not the rendered triangle
bool qxTerrainPoly::TestHalfspace(geVec3d *Point, geVec3d *Normal,
								  int Flag, bool& bAllInFlag)
{
	// bounding box is defined as follows: geVec3d LU, LD, RU, RD, TU, TD;
	geVec3d V;
	geFloat Result;

	bool AllOut = true;		// true if all pts were outside
	bool AnyIn	= false;	// true if any pts were inside
	bool AllIn	= true;		// true if all pts were inside

	// trying this order: LU, RD, TU, LD, RU, TD.  criss-cross pattern.

	// V = LU - Point;
	geVec3d_Subtract(&LU, Point, &V);
	Result = geVec3d_DotProduct(&V, Normal);
	if(Result > 0)	// if this point was inside
	{
		AnyIn = true;
		AllOut = false;
	}
	else
		AllIn = false;

	geVec3d_Subtract(&RD, Point, &V);
	Result = geVec3d_DotProduct(&V, Normal);
	if(Result > 0)
	{
		AnyIn = true;
		AllOut = false;
	}
	else
		AllIn = false;

	if(!AllIn && !AllOut)
		goto goto_AnyIn;

	geVec3d_Subtract(&TU, Point, &V);
	Result = geVec3d_DotProduct(&V, Normal);
	if(Result > 0)
	{
		AnyIn = true;
		AllOut = false;
	}
	else
		AllIn = false;

	if(!AllIn && !AllOut)
		goto goto_AnyIn;

	geVec3d_Subtract(&LD, Point, &V);
	Result = geVec3d_DotProduct(&V, Normal);
	if(Result > 0)
	{
		AnyIn = true;
		AllOut = false;
	}
	else
		AllIn = false;

	if(!AllIn && !AllOut)
		goto goto_AnyIn;

	geVec3d_Subtract(&RU, Point, &V);
	Result = geVec3d_DotProduct(&V, Normal);
	if(Result > 0)
	{
		AnyIn = true;
		AllOut = false;
	}
	else
		AllIn = false;

	if(!AllIn && !AllOut)
		goto goto_AnyIn;

	geVec3d_Subtract(&TD, Point, &V);
	Result = geVec3d_DotProduct(&V, Normal);
	if(Result > 0)
	{
		AnyIn = true;
		AllOut = false;
	}
	else
		AllIn = false;


	if(AllOut == GE_TRUE)
	{
		QXASSERT(AnyIn == false);
		QXASSERT(AllIn == false);

		if(VF_Overall & VF_OUT)
		{
			return false;	// tree is already set with proper flags
		}

		VF_Overall = VF_OUT;

		if(!m_bActive)	// if !m_bActive, set all children's flags to AllOut
		{
			m_pLeftChild->SetViewFlagsAllOut();
			m_pRightChild->SetViewFlagsAllOut();
		}

		// else, no children, exit
		return false;
	}

	if(AllIn == GE_TRUE)
	{
		QXASSERT(AllOut == false);
		QXASSERT(AnyIn == true);

		VF_Halfspace_Flags |= Flag;

		return true;
	}

// not AllIn or AllOut, return 1 (partly inside)
goto_AnyIn:

		// result == 1, wedge was partially inside this halfspace
		bAllInFlag = GE_FALSE;

		return true;
}


inline void qxTerrainPoly::SetViewFlagsAllOut(void)
{
	VF_Halfspace_Flags = 0;
	VF_Overall = VF_OUT;

	if(m_bActive == GE_FALSE)
	{
		m_pLeftChild->SetViewFlagsAllOut();
		m_pRightChild->SetViewFlagsAllOut();
	}
}


void qxTerrainPoly::SetViewFlagsAllIn(void)
{
	VF_Halfspace_Flags = VF_IN_NEAR | VF_IN_FAR | VF_IN_LEFT | VF_IN_RIGHT | VF_IN_TOP | VF_IN_BOTTOM;
	VF_Overall = VF_ALL_IN;

	if(m_bActive == GE_FALSE)
	{
		m_pLeftChild->SetViewFlagsAllIn();
		m_pRightChild->SetViewFlagsAllIn();
	}
}


int qxTerrainPoly::CalcPriority(int nNearestIsHighest)
{
	int priority;

	// if tri is outside view frustrum, priority = 0
	if(VF_Overall & VF_OUT)
		return 0;

	// if this tri is at the bottom of the tree, and thus unsplittable, pri=0
	if(m_nTreeID > m_pTerrainMap->GetMaxVarianceLookupId())
	{
		return 0;
	}

	if(! (VF_Halfspace_Flags & VF_IN_NEAR))
	{
		return PRIORITIES - 1;	//256;
	}

	const geXForm3d* pCXF = geCamera_GetCameraSpaceXForm(CCD->CameraManager()->Camera());

	float distance =
						(Position.X * pCXF->CX)
					+	(Position.Y * pCXF->CY)
					+	(Position.Z * pCXF->CZ)
					+	pCXF->Translation.Z;

	int int_dist = (int)(-distance);

	if( m_pTerrainMap->m_fNearestDistanceToCamera > -distance)
	{
		m_pTerrainMap->m_fNearestDistanceToCamera = -distance;
	}

	if( int_dist < nNearestIsHighest)
	{
		return PRIORITIES -1;
	}

	priority = m_pOwnerTree->LookupVariance(m_nTreeID);

	// int_dist is shifted-right Distance_Detail bits,
	// and this is subtracted from priority.
	priority = 256 + priority;//125 + priority;

	if(priority > (int_dist >> m_pTerrainMap->m_nDistanceDetail))
		priority = priority - (int_dist >> m_pTerrainMap->m_nDistanceDetail);
	else
		priority = 0;

	// else the index into the priority table will be out of bounds
	QXASSERT(priority < PRIORITIES);


	// onscreen tri's get a priority of at least 1, so theyr higher than offscreen tris
	if(priority == 0)
		priority++;

	return priority;
}


