// qxTerrainMapBase.cpp: implementation of the qxTerrain class.
//
// Original code: GenScape (c) 1999, Lucas Ackerman
//
// Author: Jeff Thelen
//
// Copyright 2000 Quixotic, Inc.  All Rights Reserved.
//////////////////////////////////////////////////////////////////////

#include "..\\RabidFramework.h"
#include "qxTerrainMapBase.h"
#include "qxTerrainVert.h"
#include "qxTerrainTile.h"
#include "qxSun.h"
#include "qxSkyDome.h"
#include "QxUser.h"	// this is for the geTClip_ interface

extern geBitmap *TPool_Bitmap(const char *DefaultBmp, const char *DefaultAlpha,
							  const char *BName, const char *AName);

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define NEAR_PLANE .1f

qxTerrainMapBase::qxTerrainMapBase( qxTerrainDefinition& TerrainDef )
:m_nHeightMapWidth( 0 )
,m_nHeightMapLength( 0 )
,m_nHeightMapSize(0)
,m_nTileSize( 0 )
,m_nMapOffsetIndexX ( TerrainDef.MapOffsetIndexX )
,m_nMapOffsetIndexZ( TerrainDef.MapOffsetIndexZ )
,m_strTextureFile( TerrainDef.m_strBmp )
,m_strAlphaFile( TerrainDef.m_strAlpha )
,m_nDesiredPolyCount( 1500 )
,m_pTexture(0)
,m_ppBaseTiles(0)
,m_bUpdate(true)
,m_pVertList(0)
,m_fNearestDistanceToCamera(1000.0f)
,m_fNormalDistanceToCamera(500.0f)
,m_fScale(1.0f)
,m_pSplitQueue(0)
,m_pMergeQueue(0)
,m_pSplitList(0)
,m_nNearestIsHighest(0)
,m_nRenderFlags( GE_RENDER_CLAMP_UV | GE_RENDER_DO_NOT_OCCLUDE_OTHERS) // | GE_RENDER_NO_FOG )
,m_SunLight(false)
,m_bRenderBackFace(false)
{
	m_nDesiredPolyCount =	CCD->TerrainMgr()->GetDesiredTriangles();
	m_nDistanceDetail	=	CCD->TerrainMgr()->GetDistanceDetail();
	m_pPolyPool			= 	CCD->TerrainMgr()->GetPolyPoolQX();
	m_pVertPool			=	CCD->TerrainMgr()->GetVertPoolQX();
	m_nLandscapeSize	= 	CCD->TerrainMgr()->GetLandscapeSize();
	m_nFarPlane			=	TerrainDef.m_nFarPlane;
	m_nMapOffsetX		=	m_nMapOffsetIndexX * m_nLandscapeSize;
	m_nMapOffsetZ		=	m_nMapOffsetIndexZ * m_nLandscapeSize;

	m_pSplitQueue		=	new qxSplitQueue;
	m_pMergeQueue		=	new qxMergeQueue;

	SetCurrentVertColor(CCD->TerrainMgr()->GetAmbientLightColor() );
	SetNormalDistanceToCamera(CCD->TerrainMgr()->GetNormalDistanceToCamera() );
}


qxTerrainMapBase::~qxTerrainMapBase()
{
	delete m_pSplitQueue;
	delete m_pMergeQueue;

	// delete base grid verts
	qxTerrainVert * pVert = m_pVertList;
	qxTerrainVert * pVert2;
	while(pVert != NULL)
	{
		pVert2 = pVert->m_pNext;
		delete pVert;
		pVert = pVert2;
	}


	// delete tiles
	for(int i = 0; i < m_nTilesCountTotal; i++)
	{
		if( m_ppBaseTiles[i] )
			delete (qxTerrainTile*)(m_ppBaseTiles[i]);
	}

	// delete tile ptr array
	delete [] m_ppBaseTiles;
}


bool qxTerrainMapBase::Init()
{
	return true;
}


bool qxTerrainMapBase::LoadTexture( LPCSTR strTextureFile, LPCSTR strAlphaFile)
{
	char TextureFile[64], AlphaFile[64];
	strcpy(TextureFile, strTextureFile);
	strcpy(AlphaFile, strAlphaFile);

	m_strTextureFile = strTextureFile;
	m_strAlphaFile = strAlphaFile;

	m_pTexture = TPool_Bitmap(TextureFile, AlphaFile, NULL, NULL);

	if( !m_pTexture )
		return false;

	QXASSERT( geBitmap_Width( m_pTexture ) == geBitmap_Height( m_pTexture ) );

	return true;
}


int qxTerrainMapBase::SetDesiredPolyCount(int newCount)
{
	m_nDesiredPolyCount = newCount;

	if(m_nDesiredPolyCount < m_nMinPolyCount)
		m_nDesiredPolyCount = m_nMinPolyCount;

	m_bUpdate = true;

	return m_nDesiredPolyCount;
}


void qxTerrainMapBase::LightVertex(qxTerrainVert* pVert, GE_RGBA* pColor)
{
	pVert->CurrentVert.r = pColor->r;
	pVert->CurrentVert.g = pColor->g;
	pVert->CurrentVert.b = pColor->b;
	pVert->CurrentVert.a = pColor->a;
}


void qxTerrainMapBase::LightAllVerts()
{
	qxTerrainVert* pVert = m_pVertList;

	while( pVert != NULL)
	{
		LightVertex(pVert);	// set per-vertex lighting
		pVert = pVert->m_pNext;
	}
}


void qxTerrainMapBase::OffsetAllVerts()
{
	geVec3d vCenter;
	CCD->CameraManager()->GetPosition(&vCenter);

	qxTerrainVert* pVert = m_pVertList;

	while( pVert != NULL)
	{
		pVert->CurrentVert.X -= (((float)CCD->TerrainMgr()->GetLandscapeSize()/2.0f));// + vCenter.X);
		pVert->CurrentVert.Z -= (((float)CCD->TerrainMgr()->GetLandscapeSize()/2.0f));// + vCenter.Z);
		pVert = pVert->m_pNext;
	}

	// update all base tiles
	for(int i = 0; i < m_nTilesCountTotal; i++)
	{
		m_ppBaseTiles[i]->Update();
	}

	m_bUpdate = true;
}


void qxTerrainMapBase::LightVertexSunLight(qxTerrainVert* pVert)
{
	qxSkyDome *SkyDome = CCD->TerrainMgr()->GetSkyDome();
	static qxSun* pSun = CCD->TerrainMgr()->GetSun();

	if(!SkyDome)
		return;

	if(!pSun)
	{
		qxColor SkyColor(m_CurrentVertColor);
		pVert->CurrentVert.r = SkyColor.rgba.r;
		pVert->CurrentVert.g = SkyColor.rgba.g;
		pVert->CurrentVert.b = SkyColor.rgba.b;
		pVert->CurrentVert.a = SkyColor.rgba.a;
		return;
	}

	float fTwilightPercent = CCD->TerrainMgr()->GetTwilightPercent();
	float fSunPercentToZenith = CCD->TerrainMgr()->GetSunPercentToZenith();
	float fTwilightDist = CCD->TerrainMgr()->GetTwilightDistanceFromHorizon();

	qxColor m_CurrentSunColor;

	float m_fSunIntensity;

	if( fTwilightPercent == 1.0 )
	{
		m_CurrentSunColor = qxColorWhite;
		m_fSunIntensity = 1.0f;
	}
	else
	{
		m_CurrentSunColor.SetRGBA(0, 0, 14.0, 255.0);

		// Percentage of twilight color dominance
		// twilight -> horizon = .80
		// horizon  -> twilight = .20
		float fPreHorizonPercent = (fSunPercentToZenith+fTwilightDist)/ fTwilightDist;
		fPreHorizonPercent = fPreHorizonPercent*.8f;
		fPreHorizonPercent = GE_CLAMP(fPreHorizonPercent, 0.0f, .8f);

		// horizon to twilight
		float fPostHorizonPercent = fSunPercentToZenith/ fTwilightDist;
		fPostHorizonPercent = fPostHorizonPercent * .2f;
		fPostHorizonPercent = GE_CLAMP(fPostHorizonPercent, 0.0f, .2f);

		m_fSunIntensity = fPreHorizonPercent;

		if( fTwilightPercent > .50f )
		{
			m_fSunIntensity += fPostHorizonPercent;
		}

		if(m_fSunIntensity < 0.2f)
			m_fSunIntensity = 0.2f;

		// Move toward white as dominance wanes
		float s = m_CurrentSunColor.GetS();
		s *= 1.0f-m_fSunIntensity;
		m_CurrentSunColor.SetS(s);
	}

	float fDist = geVec3d_DistanceBetween(	&pSun->Origin,
		((geVec3d*)&(pVert->CurrentVert.X)));

	if( fDist != 0.0f )
	{
		float fDistClamped = (SkyDome->GetDistanceFromSunFactor()) / fDist;
		fDist = fDistClamped;
		fDistClamped = GE_CLAMP(fDistClamped, 0.0f, 1.0f);

		qxColor SkyColor(m_CurrentVertColor);

		// Increase the saturation farther away

		float s = SkyColor.GetS();
		float delta = 1.0f-s;
		s += delta * (1.0f-m_fSunIntensity);
		SkyColor.SetS(s);

		s = SkyColor.GetS();
		delta = 1.0f-s;
		s += delta * (1.0f-m_fSunIntensity);
		SkyColor.SetS(s);

		if( fTwilightPercent == 1.0 )
		{
			pVert->CurrentVert.r = SkyColor.rgba.r;
			pVert->CurrentVert.g = SkyColor.rgba.g;
			pVert->CurrentVert.b = SkyColor.rgba.b;
			pVert->CurrentVert.a = SkyColor.rgba.a;
		}

		// Add in the sunlight glare
		// Weakening influence the closer to the end of twilight
		else
		{

			pVert->CurrentVert.r = SkyColor.rgba.r*m_fSunIntensity;
			pVert->CurrentVert.g = SkyColor.rgba.g*m_fSunIntensity;
			pVert->CurrentVert.b = SkyColor.rgba.b * m_fSunIntensity;
			pVert->CurrentVert.a = SkyColor.rgba.a;

			float fIntensity = ( fDist*( 1.0f-m_fSunIntensity) );

			pVert->CurrentVert.r = GE_CLAMP8(pVert->CurrentVert.r +
				(m_fSunIntensity*
				(m_CurrentSunColor.rgba.r * fIntensity )
				) );

			pVert->CurrentVert.g = GE_CLAMP8( pVert->CurrentVert.g +
				(m_fSunIntensity*
				(m_CurrentSunColor.rgba.g * fIntensity )
				) );

			pVert->CurrentVert.b = GE_CLAMP8( pVert->CurrentVert.b +
				(m_fSunIntensity*
				(m_CurrentSunColor.rgba.b * fIntensity )
				));
		}
	}
}


void qxTerrainMapBase::LightVertex(qxTerrainVert* pVert)
{
	if(!m_SunLight)
	{
		pVert->CurrentVert.r = m_CurrentVertColor.r;
		pVert->CurrentVert.g = m_CurrentVertColor.g;
		pVert->CurrentVert.b = m_CurrentVertColor.b;
		pVert->CurrentVert.a = m_CurrentVertColor.a;
	}
	else
		LightVertexSunLight(pVert);
}


void qxTerrainMapBase::LightVertexWireFrame(qxTerrainVert* Vert)
{
	Vert->CurrentVert.r = 255.0f;
	Vert->CurrentVert.g = 0;
	Vert->CurrentVert.b = 0;
	Vert->CurrentVert.a = 255.0f;
}


int qxTerrainMapBase::SetDistanceDetail(int newDetail)
{
	m_nDistanceDetail = newDetail;

	if(m_nDistanceDetail > MAX_DISTANCE_DETAIL)
		m_nDistanceDetail = MAX_DISTANCE_DETAIL;

	m_bUpdate = true;

	return m_nDistanceDetail;
}


void qxTerrainMapBase::Draw()
{
}


int	qxTerrainMapBase::Frame()
{
	// update pCameraPosition
	// required for qxTerrainPoly::CalcPriority() s

	bool bViewChanged = CCD->CameraManager()->GetViewChanged();

	if( !( m_bUpdate || bViewChanged) )
		return 1;

	m_fNearestDistanceToCamera = 1000.0f;


	//
	// if we want to re-triangulate from scratch
	// instead of split/merge progressively
	//
	if( m_bUpdate )
	{
		m_bUpdate = false;

		//
		// reset mesh to base triangulation
		//

		// Clear Split Queue (Qs)
		m_pSplitQueue->Clear();

		// Clear Merge Queue (Qm)
		m_pMergeQueue->Clear();

		// fill Split & Merge Queues, and recompute all queue node priorities
		// do this stuff
		// force all bintritree's into the base-triangulation with some recursive fn
		// then add all base tri's to Qs

		// fix up all base grid neighbor pointers,
		// after trees are cleared, and put tri's back in Qs
		ResetBaseTiles();

		UpdateViewFrustrum();

		m_pSplitQueue->UpdatePriorities( m_nNearestIsHighest );
	}

	else	// else, update all view flags, update all priorities.
	{
		UpdateViewFrustrum();
		m_pSplitQueue->UpdatePriorities( m_nNearestIsHighest );
		m_pMergeQueue->UpdatePriorities( m_nNearestIsHighest );
	}

	// Optimization: Adjust desired poly count for distance from camera JT
	int nDesiredPolyCount = m_nDesiredPolyCount;

	if(	m_fNearestDistanceToCamera > m_fNormalDistanceToCamera )
	{
		float fPercent = m_fNormalDistanceToCamera / m_fNearestDistanceToCamera;
		nDesiredPolyCount = (int)( fPercent*((float)m_nDesiredPolyCount));

		if(nDesiredPolyCount < m_nMinPolyCount )
			nDesiredPolyCount = m_nMinPolyCount;
	}


	qxTerrainPoly* pTerrainPoly;

	int MergePriority = 0;
	int SplitPriority = 0;

	qxSplitQueueNode* pSplitQueueNode = m_pSplitQueue->GetHighestPriorityNode();
	qxMergeQueueNode* pMergeQueueNode = m_pMergeQueue->GetLowestPriorityNode();


	if(pMergeQueueNode == NULL)
		MergePriority = 0;
	else
		MergePriority = pMergeQueueNode->m_nPriority;

	if(pSplitQueueNode == NULL)
	{
		QXASSERT(false);	// there must ALWAYS be tri's in the split queue
	}
	else
	{
		SplitPriority = pSplitQueueNode->m_nPriority;
	}

	// loop until mesh is optimal
	// max split priority is > min merge priority, and mesh is at desired density
	int i = 0;

	while(	(  (SplitPriority > MergePriority )
			|| (m_nPolyCount < nDesiredPolyCount) )
			&& i < 1000 )
	{	// we must keep updating the mesh

		// put a max limit on updates per frame.
		// lest we hit a degenerate case where it loops forever - this CAN happen.
		i++;

		/////////////////////////////////////////////////////////////////
		// too many polys - mesh is too dense, or has reached max poly count
		/////////////////////////////////////////////////////////////////
		if(m_nPolyCount > nDesiredPolyCount)
		{
			// find lowest priority node (can be diamond or a single edge tri) in Qm

			// there MUST be nodes to merge, otherwise we have a BUG
			QXASSERT(pMergeQueueNode != NULL);

			pTerrainPoly = pMergeQueueNode->m_pPoly1;

			// merge it
			Merge( pTerrainPoly );

			if(pTerrainPoly->m_pBottomNeighbor != NULL)
				Merge(pTerrainPoly->m_pBottomNeighbor); //MQN->Poly2; // if it exists

			// update queues:

			// remove diamond from MergeQueue
			// remove this node - gets both polys
			m_pMergeQueue->RemoveNode(pMergeQueueNode);

			// add newly mergable diamonds to MergeQueue

			// need parent pointer, check if all children of each parent
			// and parent->BN of the tri's
			// (pTerrainPoly, and pTerrainPoly->m_pBottomNeighbor, if it exists)
			// are active, if so, put em in MergeQueue

			// each parent of pTerrainPoly and pTerrainPoly->BN needs
			// to be tested with its children and BN to see if its a mergable diamond

			// pTerrainPoly is only a child of a mergable diamond if it's
			// got a parent (ie, isn't the root tri)
			if(pTerrainPoly->m_pParent != NULL)
			{

				// first check pTerrainPoly's parent & parent's bn
				// if the pTerrainPoly's parent is only half of a diamond
				if(pTerrainPoly->m_pParent->m_pBottomNeighbor == NULL)
				{
					if(		(pTerrainPoly->m_pParent->m_pLeftChild->m_bActive)
						&&	(pTerrainPoly->m_pParent->m_pRightChild->m_bActive) )
						m_pMergeQueue->InsertDiamond(pTerrainPoly->m_pParent);
				}
				// else, make sure the whole diamond is mergable

				else	// pTerrainPoly->m_pParent->m_pBottomNeighbor != NULL
				{
							// tri's parent and parent's BN must be each other's BN
					if(		(pTerrainPoly->m_pParent->m_pBottomNeighbor->m_pBottomNeighbor == pTerrainPoly->m_pParent)
						&&	(pTerrainPoly->m_pParent->m_pLeftChild->m_bActive)
						&&	(pTerrainPoly->m_pParent->m_pRightChild->m_bActive)
						&&	(pTerrainPoly->m_pParent->m_pBottomNeighbor->m_pLeftChild->m_bActive)
						&&	(pTerrainPoly->m_pParent->m_pBottomNeighbor->m_pRightChild->m_bActive) )

							m_pMergeQueue->InsertDiamond(pTerrainPoly->m_pParent);
				}


				// then check same for pTerrainPoly->bn
				// BN->BN is always == pTerrainPoly, cause BN was just merged
				if(pTerrainPoly->m_pBottomNeighbor != NULL)
				{
					pTerrainPoly = pTerrainPoly->m_pBottomNeighbor;

					// rest is same as above for this bn tri (which was just merged).
					if(pTerrainPoly->m_pParent->m_pBottomNeighbor == NULL)
					{
						if(		(pTerrainPoly->m_pParent->m_pLeftChild->m_bActive)
							&&	(pTerrainPoly->m_pParent->m_pRightChild->m_bActive) )
							m_pMergeQueue->InsertDiamond(pTerrainPoly->m_pParent);
					}
					// else, make sure the whole diamond is mergable
					else	// pTerrainPoly->m_pParent->m_pBottomNeighbor != NULL
					{
						// tri's parent and parent's BN must be each other's BN
						if(		(pTerrainPoly->m_pParent->m_pBottomNeighbor->m_pBottomNeighbor == pTerrainPoly->m_pParent)
							&&	(pTerrainPoly->m_pParent->m_pLeftChild->m_bActive )
							&&	(pTerrainPoly->m_pParent->m_pRightChild->m_bActive )
							&&	(pTerrainPoly->m_pParent->m_pBottomNeighbor->m_pLeftChild->m_bActive)
							&&	(pTerrainPoly->m_pParent->m_pBottomNeighbor->m_pRightChild->m_bActive) )
							m_pMergeQueue->InsertDiamond(pTerrainPoly->m_pParent);
					}
				}//if(pTerrainPoly->m_pBottomNeighbor != NULL)
			}//	if(pTerrainPoly->Parent != NULL)
		}//if(m_nPolyCount > nDesiredPolyCount)

		/////////////////////////////////////////////////////////////////
		// need more triangles, split some
		/////////////////////////////////////////////////////////////////
		else
		{
			// find highest priority tri in Qs
			QXASSERT(pSplitQueueNode != NULL);
			pTerrainPoly = pSplitQueueNode->m_pPoly;
			QXASSERT(pTerrainPoly->m_bActive);

			// don't split 0-priority pTerrainPoly's!
			// if we hit 0 here, then we're at the max triangulation
			// possible for the current view
			if(pSplitQueueNode->m_nPriority == 0)
				break;

			// split the tri
			// m_pSplitList keeps track of newly split tris.  see Split2().
			// there should be NOTHING in the split list before a split
			QXASSERT(m_pSplitList == NULL);
			Split( pTerrainPoly );
			// now Split_List is a list of all tris that were force split
			QXASSERT(m_pSplitList != NULL);

			//
			// update queues
			//
			while( m_pSplitList )	// check each tri in the list
			{
				if(m_pSplitList->m_pParent != NULL)
				{
					// if pTerrainPoly's parent is in merge queue, remove it
					if(m_pSplitList->m_pParent->m_pMergeQueueNode != NULL)
					{
						m_pMergeQueue->RemoveNode(m_pSplitList->m_pParent->m_pMergeQueueNode);
					}
				}

				// remove pTerrainPoly from split queue
				m_pSplitQueue->RemoveNode(m_pSplitList->m_pSplitQueueNode);

				m_pSplitList = m_pSplitList->m_pNext;
			}

			QXASSERT(m_pSplitList == NULL);

			// add newly mergable diamonds to Qm
			m_pMergeQueue->InsertDiamond(pTerrainPoly);
			// note: any that were force-split as a result of Split(tri), aren't mergable

		} // split some

		// get newest high & low nodes
		pSplitQueueNode = m_pSplitQueue->GetHighestPriorityNode();
		SplitPriority = pSplitQueueNode->m_nPriority;

		pMergeQueueNode = m_pMergeQueue->GetLowestPriorityNode();
		if(pMergeQueueNode == NULL)
			MergePriority = 0;
		else
			MergePriority = pMergeQueueNode->m_nPriority;

	} // while


	return 1;
}


bool qxTerrainMapBase::Render()
{
	geRect Rect;
	geCamera_GetClippingRect(CCD->CameraManager()->Camera(), &Rect);

	geTClip_Push();

	geTClip_SetupEdges(CCD->Engine()->Engine(),
						(float)Rect.Left,
						(float)Rect.Right,
						(float)Rect.Top,
						(float)Rect.Bottom,
						1.0f);

	//
	// transform & project all base verts
	//
	LightAllVerts();

	qxTerrainVert* pVert = m_pVertList;

	while( pVert != NULL)
	{
		geCamera_TransformAndProject(CCD->CameraManager()->Camera(),
									(geVec3d *)&(pVert->CurrentVert),
									(geVec3d *) &(pVert->ProjectedVert));

		pVert = pVert->m_pNext;
	}


	geTClip_SetRenderFlags( m_nRenderFlags );

	CCD->TerrainMgr()->SetRender( m_nRenderFlags );

	//
	// render all polys
	//
	for(int i = 0; i < m_nTilesCountTotal; i++)
	{
		//
		// if either base-tri in the tile is !VF_Out,
		// then switch textures & render tri's.
		// else, skip em, so we dont do _SetTexture()
		// when nothing with the texture is rendered
		//

		if( 	!( (m_ppBaseTiles[i])->IsLBRootOut() ) ||
				!( (m_ppBaseTiles[i])->IsRTRootOut() ) )
		{

			CCD->TerrainMgr()->SetTexture(GetTexture(i));

			geTClip_SetTexture( GetTexture(i) );

			m_ppBaseTiles[i]->LBRender();
			m_ppBaseTiles[i]->RTRender();
		}
	}


	geTClip_Pop();

	return true;
}


bool qxTerrainMapBase::RenderWireframe()
{
	qxTerrainVert* pVert = m_pVertList;

	geRect Rect;
	geCamera_GetClippingRect(CCD->CameraManager()->Camera(), &Rect);

	geTClip_Push();

	geTClip_SetupEdges(CCD->Engine()->Engine(),
						(float)Rect.Left,
						(float)Rect.Right,
						(float)Rect.Top,
						(float)Rect.Bottom,
						1.0);


	while(pVert != NULL)
	{
		LightVertexWireFrame(pVert);
		geCamera_TransformAndProject(CCD->CameraManager()->Camera(),
										(geVec3d *)&(pVert->CurrentVert),
										(geVec3d *)&(pVert->ProjectedVert));
		pVert = pVert->m_pNext;
	}



	geTClip_SetRenderFlags(m_nRenderFlags);
	CCD->TerrainMgr()->SetRender( m_nRenderFlags );

	// render all polys
	for(int i = 0; i < m_nTilesCountTotal; i++)
	{

		if( 	!( (m_ppBaseTiles[i])->IsLBRootOut() )  ||
				!( (m_ppBaseTiles[i])->IsRTRootOut() )  )
		{
			geTClip_SetTexture( NULL );
			(m_ppBaseTiles[i])->LBRenderWireFrame();
			(m_ppBaseTiles[i])->RTRenderWireFrame();
		}
	}

	geTClip_Pop();

	return true;
}


void qxTerrainMapBase::UpdateViewFrustrum()
{
	geVec3d thePosition;
	CCD->CameraManager()->GetPosition(&thePosition);
	const geVec3d* pCameraPosition = &thePosition;

	geRect Rect;				// screenspace clipping rect
	geVec3d TL, TR, BL, BR;		// top/bottom, left/right screen corner vects, in world space
	geVec3d TLF, TRF, BLF, BRF;	// same, but projected to far clipping plane

	geVec3d Left, Right;	// left, right, vects on the plane.  x product = plane normal

	geCamera_GetClippingRect(CCD->CameraManager()->Camera(), &Rect);

	// project verts from corners of viewing rect,
	// use these to get a point & normal (plane) for each halfspace

	// top left
	// get TL direction vect
	geCamera_ScreenPointToWorld(CCD->CameraManager()->Camera(), Rect.Left, Rect.Top, &TL);
	// get actual world-space TLF pos
	geVec3d_AddScaled(pCameraPosition, &TL, (float)m_nFarPlane, &TLF);
	// get world-space TL near pos
	geVec3d_AddScaled(pCameraPosition, &TL, NEAR_PLANE, &TL);

	// top right
	geCamera_ScreenPointToWorld(CCD->CameraManager()->Camera(), Rect.Right, Rect.Top, &TR);
	geVec3d_AddScaled(pCameraPosition, &TR, (float)m_nFarPlane, &TRF);
	geVec3d_AddScaled(pCameraPosition, &TR, NEAR_PLANE, &TR);

	// bottom left
	geCamera_ScreenPointToWorld(CCD->CameraManager()->Camera(), Rect.Left, Rect.Bottom, &BL);
	geVec3d_AddScaled(pCameraPosition, &BL, (float)m_nFarPlane, &BLF);
	geVec3d_AddScaled(pCameraPosition, &BL, NEAR_PLANE, &BL);

	// bottom right
	geCamera_ScreenPointToWorld(CCD->CameraManager()->Camera(), Rect.Right, Rect.Bottom, &BR);
	geVec3d_AddScaled(pCameraPosition, &BR, (float)m_nFarPlane, &BRF);
	geVec3d_AddScaled(pCameraPosition, &BR, NEAR_PLANE, &BR);

	// 6 points on the 6 halfspace planes

	FrustrumLeft = TL;
	FrustrumRight = TR;
	FrustrumTop = TR;
	FrustrumBottom = BR;
	FrustrumNear = TL;
	FrustrumFar = TLF;

	// set plane normals

	// bottom normal
	geVec3d_Subtract(&BL, &BLF, &Left);
	geVec3d_Subtract(&BL, &BRF, &Right);
	geVec3d_CrossProduct(&Right, &Left, &NormalBottom);
	geVec3d_Normalize(&NormalBottom);


	// top normal
	geVec3d_Subtract(&TL, &TLF, &Right);
	geVec3d_Subtract(&TL, &TRF, &Left);
	geVec3d_CrossProduct(&Right, &Left, &NormalTop);
	geVec3d_Normalize(&NormalTop);


	// left normal
	geVec3d_Subtract(&TL, &TLF, &Left);
	geVec3d_Subtract(&TL, &BLF, &Right);
	geVec3d_CrossProduct(&Right, &Left, &NormalLeft);
	geVec3d_Normalize(&NormalLeft);


	// right normal
	geVec3d_Subtract(&BR, &BRF, &Left);
	geVec3d_Subtract(&BR, &TRF, &Right);
	geVec3d_CrossProduct(&Right, &Left, &NormalRight);
	geVec3d_Normalize(&NormalRight);


	// near normal
	geVec3d_Subtract(&TL, &BL, &Left);
	geVec3d_Subtract(&TL, &BR, &Right);
	geVec3d_CrossProduct(&Right, &Left, &NormalNear);
	geVec3d_Normalize(&NormalNear);

	// far normal
	geVec3d_Subtract(&TLF, &BRF, &Left);
	geVec3d_Subtract(&TLF, &BLF, &Right);
	geVec3d_CrossProduct(&Right, &Left, &NormalFar);
	geVec3d_Normalize(&NormalFar);


	// now, update view frustrum info for the whole landscape
	for(int i = 0; i < m_nTilesCountTotal; i++)
	{
		(m_ppBaseTiles[i])->UpdateViewFlagsLB();
		(m_ppBaseTiles[i])->UpdateViewFlagsRT();
	}
}


inline qxTerrainVert* qxTerrainMapBase::AllocateVert()
{
	// first try to get a vert from the pool
	qxTerrainVert* Vert = m_pVertPool->ExtractVert();
	// otherwise, make a new vert
	if(Vert == NULL)
		Vert = new qxTerrainVert();
	return Vert;
}


inline qxTerrainPoly* qxTerrainMapBase::AllocatePoly()
{
	// first try to get a poly from the pool
	qxTerrainPoly* Poly = m_pPolyPool->ExtractPoly();
	// otherwise, make a new poly
	if(Poly == NULL)
		Poly = new qxTerrainPoly();
	return Poly;
}


void qxTerrainMapBase::ResetBaseTiles()
{
	int i;
	m_nPolyCount = m_nMinPolyCount;

	for(i = 0; i < m_nTilesCountTotal; i++)
	{
		// clear grid-tile sub trees (also sets Root_pPoly->m_bActive = GE_TRUE)
		m_ppBaseTiles[i]->Reset();

		// insert root tris back into split queue
		m_pSplitQueue->InsertTerrainPoly( m_ppBaseTiles[i]->LBGetTerrainPoly() );
		m_pSplitQueue->InsertTerrainPoly( m_ppBaseTiles[i]->RTGetTerrainPoly() );
	}

	// relink grid-tiles
	for(i = 0; i < m_nTilesCountX; i++)	// horiz
	{
		for(int j = 0; j < m_nTilesCountZ; j++)	// vert
		{
			// link left and link top
			if(i > 0)
				m_ppBaseTiles[j * m_nTilesCountX + i]->LinkLeft(m_ppBaseTiles[j * m_nTilesCountX + (i-1)]);
			if(j > 0)
				m_ppBaseTiles[j * m_nTilesCountX + i]->LinkTop(m_ppBaseTiles[(j-1) * m_nTilesCountX + i]);
		}
	}
}


void qxTerrainMapBase::Split(qxTerrainPoly* pPoly)
{
	// these QXASSERTs make SURE that we are splitting
	// a tri that isnt already split
	QXASSERT( pPoly->m_bActive );
	QXASSERT( !pPoly->m_pLeftChild);
	QXASSERT( !pPoly->m_pRightChild);

	if (pPoly->m_pBottomNeighbor)
	{

		if (pPoly->m_pBottomNeighbor->m_pBottomNeighbor != pPoly)
		{
			Split(pPoly->m_pBottomNeighbor);
		}

		// no probs here
		QXASSERT(pPoly->m_pBottomNeighbor != NULL);
		QXASSERT(pPoly->m_pBottomNeighbor->m_pBottomNeighbor == pPoly);

		Split2(pPoly, false);

		// use 3 instead of 2.  does less work, uses ->BN info already calc'd in split2
		Split2(pPoly->m_pBottomNeighbor, true);

		pPoly->m_pLeftChild->m_pRightNeighbor = pPoly->m_pBottomNeighbor->m_pRightChild;
		pPoly->m_pRightChild->m_pLeftNeighbor = pPoly->m_pBottomNeighbor->m_pLeftChild;
		pPoly->m_pBottomNeighbor->m_pLeftChild->m_pRightNeighbor = pPoly->m_pRightChild;
		pPoly->m_pBottomNeighbor->m_pRightChild->m_pLeftNeighbor = pPoly->m_pLeftChild;
	}
	else
	{
		Split2(pPoly, false);
		pPoly->m_pLeftChild->m_pRightNeighbor = 0;
		pPoly->m_pRightChild->m_pLeftNeighbor = 0;
	}
}


void qxTerrainMapBase::Split2(qxTerrainPoly* pPoly, bool bCopy)
{
	QXASSERT(pPoly->m_bActive);

	m_nPolyCount++;

	//update the split tri
	pPoly->m_bActive = false;

	// TEST: add tri to list of split tris, to be later purged from queue
	pPoly->m_pNext = m_pSplitList;
	m_pSplitList = pPoly;


	// create sub vert
	pPoly->m_pSubVert = AllocateVert();

	if( bCopy )
	{
		// copy vert from bottom neighbor, instead of calc'ing all the stuff
		*(pPoly->m_pSubVert) = *(pPoly->m_pBottomNeighbor->m_pSubVert);

		// texture coords can't be copied, because ->BN may reside on another texture
		pPoly->m_pSubVert->CurrentVert.u = (pPoly->m_pLeftVert->CurrentVert.u + pPoly->m_pRightVert->CurrentVert.u) / 2;
		pPoly->m_pSubVert->CurrentVert.v = (pPoly->m_pLeftVert->CurrentVert.v + pPoly->m_pRightVert->CurrentVert.v) / 2;

	}
	else
	{
		pPoly->m_pSubVert->CurrentVert.X =
			(pPoly->m_pLeftVert->CurrentVert.X + pPoly->m_pRightVert->CurrentVert.X) / 2;
		pPoly->m_pSubVert->CurrentVert.Z =
			(pPoly->m_pLeftVert->CurrentVert.Z + pPoly->m_pRightVert->CurrentVert.Z) / 2;

		// texture coords
		pPoly->m_pSubVert->CurrentVert.u =
			(pPoly->m_pLeftVert->CurrentVert.u + pPoly->m_pRightVert->CurrentVert.u) / 2;
		pPoly->m_pSubVert->CurrentVert.v =
			(pPoly->m_pLeftVert->CurrentVert.v + pPoly->m_pRightVert->CurrentVert.v) / 2;

		// heightmap coords
		pPoly->m_pSubVert->hX = (pPoly->m_pLeftVert->hX + pPoly->m_pRightVert->hX) / 2;
		pPoly->m_pSubVert->hZ = (pPoly->m_pLeftVert->hZ + pPoly->m_pRightVert->hZ) / 2;

		//pPoly->m_pSubVert->CurrentVert.Y =
			//GetElementHeight(pPoly->m_pSubVert->hX, pPoly->m_pSubVert->hZ);
		if(!SetCurrentVertHeight(pPoly->m_pSubVert))
			pPoly->m_pSubVert->CurrentVert.Y = (pPoly->m_pLeftVert->CurrentVert.Y
								+ pPoly->m_pRightVert->CurrentVert.Y) / 2;

		//JJT
		//pPoly->Owner_Landscape->CalculateVertexLighting( pPoly->m_pSubVert );

	}// no copy


	// alloc child polys
	pPoly->m_pLeftChild = AllocatePoly();
	pPoly->m_pRightChild = AllocatePoly();

	pPoly->m_pLeftChild->m_bActive = true;
	pPoly->m_pRightChild->m_bActive = true;

	// assign child poly tree id's, for variance-table lookup
	pPoly->m_pLeftChild->m_nTreeID	= pPoly->m_nTreeID * 2;
	pPoly->m_pRightChild->m_nTreeID = pPoly->m_nTreeID * 2 + 1;

	// TEST: parent ptrs needed for adding polys to merge queue
	pPoly->m_pLeftChild->m_pParent = pPoly;
	pPoly->m_pRightChild->m_pParent = pPoly;

	pPoly->m_pLeftChild->m_pTerrainMap = this;
	pPoly->m_pRightChild->m_pTerrainMap = this;

	// for variance lookup, needs to know owner tree
	pPoly->m_pLeftChild->m_pOwnerTree = pPoly->m_pOwnerTree;
	pPoly->m_pRightChild->m_pOwnerTree = pPoly->m_pOwnerTree;

	// assign verts
	pPoly->m_pLeftChild->m_pTopVert = pPoly->m_pSubVert;
	pPoly->m_pLeftChild->m_pLeftVert = pPoly->m_pTopVert;
	pPoly->m_pLeftChild->m_pRightVert = pPoly->m_pLeftVert;

	pPoly->m_pRightChild->m_pTopVert = pPoly->m_pSubVert;
	pPoly->m_pRightChild->m_pRightVert = pPoly->m_pTopVert;
	pPoly->m_pRightChild->m_pLeftVert = pPoly->m_pRightVert;

	// calc child poly positions.
	pPoly->m_pLeftChild->UpdatePosition();
	pPoly->m_pRightChild->UpdatePosition();
	// update wedge bounding box
	pPoly->m_pLeftChild->UpdateWedgeBounds();
	pPoly->m_pRightChild->UpdateWedgeBounds();
	// update view flags
	pPoly->m_pLeftChild->UpdateViewFlags();
	pPoly->m_pRightChild->UpdateViewFlags();


	// setup neighbor poly ptrs
	pPoly->m_pLeftChild->m_pLeftNeighbor = pPoly->m_pRightChild;
	pPoly->m_pRightChild->m_pRightNeighbor = pPoly->m_pLeftChild;

	pPoly->m_pLeftChild->m_pBottomNeighbor = pPoly->m_pLeftNeighbor;

	if(pPoly->m_pLeftNeighbor != NULL)
	{
		if(pPoly->m_pLeftNeighbor->m_pBottomNeighbor == pPoly)
		{
			pPoly->m_pLeftNeighbor->m_pBottomNeighbor = pPoly->m_pLeftChild;
		}
		else
		{
			if(pPoly->m_pLeftNeighbor->m_pLeftNeighbor == pPoly)
			{
				pPoly->m_pLeftNeighbor->m_pLeftNeighbor = pPoly->m_pLeftChild;
			}
			else
			{
				pPoly->m_pLeftNeighbor->m_pRightNeighbor = pPoly->m_pLeftChild;
			}
		}
	}

	pPoly->m_pRightChild->m_pBottomNeighbor = pPoly->m_pRightNeighbor;

	if(pPoly->m_pRightNeighbor != NULL)
	{
		if(pPoly->m_pRightNeighbor->m_pBottomNeighbor == pPoly)
		{
			pPoly->m_pRightNeighbor->m_pBottomNeighbor = pPoly->m_pRightChild;
		}
		else
		{
			if(pPoly->m_pRightNeighbor->m_pRightNeighbor == pPoly)
			{
				pPoly->m_pRightNeighbor->m_pRightNeighbor = pPoly->m_pRightChild;
			}
			else
			{
				pPoly->m_pRightNeighbor->m_pLeftNeighbor = pPoly->m_pRightChild;
			}
		}
	}


	// put children in split queue
	m_pSplitQueue->InsertTerrainPoly(pPoly->m_pLeftChild);
	m_pSplitQueue->InsertTerrainPoly(pPoly->m_pRightChild);
}


bool qxTerrainMapBase::SetCurrentVertHeight( qxTerrainVert* pVert)
{
	pVert->CurrentVert.Y = GetElementHeight( pVert->hX, pVert->hZ );
	return true;
}


void qxTerrainMapBase::Merge(qxTerrainPoly* pPoly)
{
	QXASSERT(!pPoly->m_bActive);

	pPoly->m_bActive = GE_TRUE;
	m_nPolyCount--;

	// relink tri to children's neighbors
	pPoly->m_pLeftNeighbor = pPoly->m_pLeftChild->m_pBottomNeighbor;
	pPoly->m_pRightNeighbor = pPoly->m_pRightChild->m_pBottomNeighbor;

	// relink children's neighbors to parent
	// link left child's BN to parent
	if(pPoly->m_pLeftChild->m_pBottomNeighbor)	// pPoly->m_pLeftNeighbor
	{
		if(pPoly->m_pLeftChild->m_pBottomNeighbor->m_pBottomNeighbor == pPoly->m_pLeftChild)
		{
			pPoly->m_pLeftChild->m_pBottomNeighbor->m_pBottomNeighbor = pPoly;
		}
		else
		{
			if(pPoly->m_pLeftChild->m_pBottomNeighbor->m_pRightNeighbor == pPoly->m_pLeftChild)
			{
				pPoly->m_pLeftChild->m_pBottomNeighbor->m_pRightNeighbor = pPoly;
			}
			else
			{
				pPoly->m_pLeftChild->m_pBottomNeighbor->m_pLeftNeighbor = pPoly;
			}
		}
	}
	// link right child's BN to parent
	if(pPoly->m_pRightChild->m_pBottomNeighbor)	// pPoly->m_pRightNeighbor
	{
		if(pPoly->m_pRightChild->m_pBottomNeighbor->m_pBottomNeighbor == pPoly->m_pRightChild)
		{
			pPoly->m_pRightChild->m_pBottomNeighbor->m_pBottomNeighbor = pPoly;
		}
		else
		{
			if(pPoly->m_pRightChild->m_pBottomNeighbor->m_pRightNeighbor == pPoly->m_pRightChild)
			{
				pPoly->m_pRightChild->m_pBottomNeighbor->m_pRightNeighbor = pPoly;
			}
			else
			{
				pPoly->m_pRightChild->m_pBottomNeighbor->m_pLeftNeighbor = pPoly;
			}
		}
	}

	m_pSplitQueue->RemoveNode(pPoly->m_pRightChild->m_pSplitQueueNode);
	m_pSplitQueue->RemoveNode(pPoly->m_pLeftChild->m_pSplitQueueNode);


	m_pSplitQueue->InsertTerrainPoly(pPoly);

	m_pPolyPool->PutPoly( &(pPoly->m_pLeftChild) );
	m_pPolyPool->PutPoly( &(pPoly->m_pRightChild) );
	m_pVertPool->PutVert( &(pPoly->m_pSubVert) );

	return;
}


bool qxTerrainMapBase::InitBaseVerts()
{
	m_nMapOffsetX -= (CCD->TerrainMgr()->GetLandscapeSize()/2);
	m_nMapOffsetZ -= (CCD->TerrainMgr()->GetLandscapeSize()/2);

	// array of pointers to base tiles
	m_ppBaseTiles = new qxTerrainTile*[m_nTilesCountTotal];
	memset(m_ppBaseTiles, 0, sizeof(qxTerrainTile*)*m_nTilesCountTotal);

	qxTerrainVert *TL, *TR, *BL, *BR;
	TL = TR = BL = BR = (qxTerrainVert *)NULL;
	qxTerrainVert* Vert;

	int i, j;

	// first tile, at 0,0
	for( i = 0; i < 2; i++)
	{
		for( j = 0; j < 2; j++)
		{
			Vert = new qxTerrainVert();
			// hmap coords
			Vert->hX = i * m_nHeightElementsPerTile;
			Vert->hZ = j * m_nHeightElementsPerTile;

			// world coords
			Vert->CurrentVert.X = (float)(i * m_nTileSize)	+ m_nMapOffsetX;
			Vert->CurrentVert.Z = (float)(j * m_nTileSize)	+ m_nMapOffsetZ;
			Vert->CurrentVert.Y = GetElementHeight(Vert->hX, Vert->hZ);

			Vert->CurrentVert.u = (float)i;	// 0 to 1
			Vert->CurrentVert.v = (float)j;

			// shuffle verts around each time.  after the loop, these will be ordered correctly
			TL = BL;
			BL = TR;
			TR = BR;
			BR = Vert;
		}
	}

	// upper-left corner tile
	m_ppBaseTiles[0] = new qxTerrainTile(this, TL, TR, BL, BR);

	//add these 4 verts to the landscape base-grid vert list
	m_pVertList = TL;
	TL->m_pNext = TR;
	TR->m_pNext = BL;
	BL->m_pNext = BR;
	BR->m_pNext = 0;

	// top row of tiles
	j = 0;
	for(i = 1; i < m_nTilesCountX; i++)
	{
		// leftside verts
		// left verts are inherited from bottom of right tile
		// start with all same coords as rightside vert of left-neighbor tile
		Vert = (m_ppBaseTiles[j * m_nTilesCountZ + (i-1)])->GetTR();
		TL = new qxTerrainVert();
		*TL = *Vert;
		TL->CurrentVert.u = 0;
		TL->CurrentVert.v = 0;

		TL->m_pNext = m_pVertList;
		m_pVertList = TL;

		Vert = (m_ppBaseTiles[j * m_nTilesCountZ + (i-1)])->GetBR();
		BL = new qxTerrainVert();
		*BL = *Vert;
		BL->CurrentVert.u = 0;
		BL->CurrentVert.v = 1;

		BL->m_pNext = m_pVertList;
		m_pVertList = BL;

		// rightside verts
		Vert = new qxTerrainVert();

		Vert->hX = (i + 1) * m_nHeightElementsPerTile;
		Vert->hZ = 0;

		Vert->CurrentVert.X = (float)((i + 1) * m_nTileSize)+ m_nMapOffsetX;
		Vert->CurrentVert.Z = (float)0 + m_nMapOffsetZ;
		Vert->CurrentVert.Y = GetElementHeight(Vert->hX, Vert->hZ);

		Vert->CurrentVert.u = 1;	// 0 to 1
		Vert->CurrentVert.v = 0;

		TR = Vert;

		TR->m_pNext = m_pVertList;
		m_pVertList = TR;


		Vert = new qxTerrainVert();

		Vert->hX = (i + 1) * m_nHeightElementsPerTile;
		Vert->hZ = 1 * m_nHeightElementsPerTile;

		Vert->CurrentVert.X = (float)((i + 1) * m_nTileSize) + m_nMapOffsetX;
		Vert->CurrentVert.Z = (float)(1 * m_nTileSize) + m_nMapOffsetZ;
		Vert->CurrentVert.Y = GetElementHeight(Vert->hX, Vert->hZ);

		Vert->CurrentVert.u = 1;	// 0 to 1
		Vert->CurrentVert.v = 1;

		BR = Vert;

		BR->m_pNext = m_pVertList;
		m_pVertList = BR;

		m_ppBaseTiles[i] = new qxTerrainTile(this, TL, TR, BL, BR);

		// link left
		(m_ppBaseTiles[i])->LinkLeft(m_ppBaseTiles[i - 1]);
	}


	// left column of tiles
	i = 0;
	for(j = 1; j < m_nTilesCountZ; j++)
	{

		// top verts are inherited from bottom of top tile
		Vert = (m_ppBaseTiles[(j-1) * m_nTilesCountZ + i])->GetBL();
		TL = new qxTerrainVert();
		*TL = *Vert;
		TL->CurrentVert.u = 0;
		TL->CurrentVert.v = 0;

		TL->m_pNext = m_pVertList;
		m_pVertList = TL;

		Vert = (m_ppBaseTiles[(j-1) * m_nTilesCountZ + i])->GetBR();
		TR = new qxTerrainVert();
		*TR = *Vert;
		TR->CurrentVert.u = 1;
		TR->CurrentVert.v = 0;

		TR->m_pNext = m_pVertList;
		m_pVertList = TR;

		// bottomside verts
		Vert = new qxTerrainVert();

		Vert->hX = 0;
		Vert->hZ = (j+1) * m_nHeightElementsPerTile;

		Vert->CurrentVert.X = (float)0 + m_nMapOffsetX;
		Vert->CurrentVert.Z = (float)((j+1) * m_nTileSize) + m_nMapOffsetZ;
		Vert->CurrentVert.Y = GetElementHeight(Vert->hX, Vert->hZ);

		Vert->CurrentVert.u = 0;	// 0 to 1
		Vert->CurrentVert.v = 1;

		BL = Vert;

		BL->m_pNext = m_pVertList;
		m_pVertList = BL;


		Vert = new qxTerrainVert();

		Vert->hX = (i + 1) * m_nHeightElementsPerTile;
		Vert->hZ = (j+1) * m_nHeightElementsPerTile;

		Vert->CurrentVert.X = (float)((i + 1) * m_nTileSize) + m_nMapOffsetX;
		Vert->CurrentVert.Z = (float)((j+1) * m_nTileSize) + m_nMapOffsetZ;
		Vert->CurrentVert.Y = GetElementHeight(Vert->hX, Vert->hZ);

		Vert->CurrentVert.u = 1;	// 0 to 1
		Vert->CurrentVert.v = 1;

		BR = Vert;

		BR->m_pNext = m_pVertList;
		m_pVertList = BR;


		m_ppBaseTiles[j * m_nTilesCountX + i] = new qxTerrainTile(this, TL, TR, BL, BR);


		// link top
		(m_ppBaseTiles[j * m_nTilesCountX + i])->LinkTop(m_ppBaseTiles[(j-1) * m_nTilesCountX + i]);
	}

	// rest of grid of tiles
	for(i = 1; i < m_nTilesCountX; i++)
	{
		for(j = 1; j < m_nTilesCountZ; j++)
		{

		//get top-left from top-neighbors bottom-left
		Vert = (m_ppBaseTiles[(j-1) * m_nTilesCountZ + i])->GetBL();
		TL = new qxTerrainVert();
		*TL = *Vert;
		TL->CurrentVert.u = 0.0;
		TL->CurrentVert.v = 0.0;

		TL->m_pNext = m_pVertList;
		m_pVertList = TL;

		//get top-right from top-neighbors bottom-right
		Vert = (m_ppBaseTiles[(j-1) * m_nTilesCountZ + i])->GetBR();
		TR = new qxTerrainVert();
		*TR = *Vert;
		TR->CurrentVert.u = 1.0;
		TR->CurrentVert.v = 0.0;

		TR->m_pNext = m_pVertList;
		m_pVertList = TR;

		// get bottom-left, from left-neighbors bottom-right
		Vert = (m_ppBaseTiles[j * m_nTilesCountZ + (i-1)])->GetBR();
		BL = new qxTerrainVert();
		*BL = *Vert;
		BL->CurrentVert.u = 0.0;
		BL->CurrentVert.v = 1.0;

		BL->m_pNext = m_pVertList;
		m_pVertList = BL;

		// create new BR
		Vert = new qxTerrainVert();

		Vert->hX = (i + 1) * m_nHeightElementsPerTile;
		Vert->hZ = (j+1) * m_nHeightElementsPerTile;

		Vert->CurrentVert.X = (float)((i + 1) * m_nTileSize)+ m_nMapOffsetX;
		Vert->CurrentVert.Z = (float)((j+1) * m_nTileSize) + m_nMapOffsetZ;
		Vert->CurrentVert.Y = (float)GetElementHeight(Vert->hX, Vert->hZ);

		Vert->CurrentVert.u = 1.0;	// 0 to 1
		Vert->CurrentVert.v = 1.0;

		BR = Vert;

		BR->m_pNext = m_pVertList;
		m_pVertList = BR;

		m_ppBaseTiles[j * m_nTilesCountX + i] = new qxTerrainTile(this, TL, TR, BL, BR);

		// link left and link top
		(m_ppBaseTiles[j * m_nTilesCountX + i])->LinkTop(m_ppBaseTiles[(j-1) * m_nTilesCountX + i]);
		(m_ppBaseTiles[j * m_nTilesCountX + i])->LinkLeft(m_ppBaseTiles[j * m_nTilesCountX + (i-1)]);

		}
	}

	return true;
}


////////////////////////////////////////////////////////////////////////////////
//
// GetSurfaceNormal()
//
// calc the surface normal of a heightfield element.
// this DOES take into account the worldspace mapping of the heightfield elements,
// so if you stretch a heightfield far horizontally,
// the normals will reflect this properly.
//
// X,Z are HEIGHTMAP coords, not world coords.
//
////////////////////////////////////////////////////////////////////////////////

void qxTerrainMapBase::GetSurfaceNormal(int X, int Z, geVec3d* Normal)
{
	QXASSERT(Normal != NULL);
	QXASSERT(X < m_nHeightMapWidth);
	QXASSERT(Z < m_nHeightMapLength);

	// heights of the elements immediately surrounding the one at X,Z.
	float north, south, left, right;

	// 2 * world-coord space between height samples
	int space_between_height_samples_X2 = 2 * m_nTileSize / m_nHeightElementsPerTile;

	// heights of elements just outside the heightmap are taken from nearest inside hmap.

	if(Z == 0)
		north = GetElementHeight(X, Z);
	else
		north = GetElementHeight(X, Z - 1);

	if(Z == m_nHeightMapLength - 1)
		south = GetElementHeight(X, Z);
	else
		south = GetElementHeight(X, Z + 1);

	if(X == 0)
		left = GetElementHeight(X, Z);
	else
		left = GetElementHeight(X - 1, Z);

	if(X == m_nHeightMapWidth - 1)
		right = GetElementHeight(X, Z);
	else
		right = GetElementHeight(X + 1, Z);


	Normal->X = 0 - (right - left)/(float)space_between_height_samples_X2;
	Normal->Z = 0 - (south - north)/(float)space_between_height_samples_X2;
	Normal->Y = 1;

	geVec3d_Normalize(Normal);
}


void qxTerrainMapBase::Dump()
{
}