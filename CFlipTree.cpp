/************************************************************************************//**
 * @file CFlipTree.cpp
 * @brief CFlipTree class implementation
 * @author Dan Valeo
 * @date Aug 2004
 *//*
 * Copyright (c) 2004 Dan Valeo; All rights reserved.
 ****************************************************************************************/

#include "RabidFramework.h"
#include "CFlipTree.h"

extern geBitmap *TPool_Bitmap(const char *DefaultBmp, const char *DefaultAlpha,
							  const char *BName, const char *AName);

/* ------------------------------------------------------------------------------------ */
//	Constructor
/* ------------------------------------------------------------------------------------ */
CFlipTree::CFlipTree()
{
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "FlipTree");

	if(!pSet)
		return;

	geEntity *pEntity;

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		FlipTree *S = static_cast<FlipTree*>(geEntity_GetUserData(pEntity));

		if(EffectC_IsStringNull(S->szEntityName))
		{
			// Get the Entity Name
			char szName[128];
			geEntity_GetName(pEntity, szName, 128);
			S->szEntityName = szName;
		}

		//Add it to Entity Registry
		CCD->EntityRegistry()->AddEntity(S->szEntityName, "FlipTree");

		S->Bitmap = NULL;

		if(!EffectC_IsStringNull(S->BitmapName))
		{
			if(!EffectC_IsStringNull(S->AlphamapName))
				S->Bitmap = TPool_Bitmap(S->BitmapName, S->AlphamapName, NULL, NULL);
			else
				S->Bitmap = TPool_Bitmap(S->BitmapName , S->BitmapName , NULL, NULL);
		}

		S->Shadow = NULL;

		//shadow image
		if(!EffectC_IsStringNull(S->ShadowMapName))
		{
			if(!EffectC_IsStringNull(S->ShadowAlphaName))
				S->Shadow = TPool_Bitmap(S->ShadowMapName, S->ShadowAlphaName, NULL, NULL);
			else
				S->Shadow = TPool_Bitmap(S->ShadowMapName, S->ShadowMapName, NULL, NULL);
		}

		S->fActor = NULL;

		//load the actor
		if(!EffectC_IsStringNull(S->ActorName))
		{
			S->fActor = CCD->ActorManager()->LoadActor(S->ActorName, NULL);

			if(S->fActor)
			{
				int RenderFlag = 0;
				CCD->ActorManager()->SetActorFlags(S->fActor, RenderFlag);
				CCD->ActorManager()->Position(S->fActor, S->origin);
				CCD->ActorManager()->Rotate(S->fActor, S->Rotation);
				CCD->ActorManager()->SetScale(S->fActor, S->Scale);
				CCD->ActorManager()->SetHideRadar(S->fActor, true);
				CCD->ActorManager()->SetBoxChange(S->fActor, false);
				CCD->ActorManager()->SetBBox(S->fActor, S->BoxSize.X, S->BoxSize.Y, S->BoxSize.Z);
				CCD->ActorManager()->SetShadow(S->fActor, S->ShadowSize);
				CCD->ActorManager()->SetShadowAlpha(S->fActor, 255.0f);
				CCD->ActorManager()->SetShadowBitmap(S->fActor, S->Shadow);
				CCD->ActorManager()->SetActorDynamicLighting(S->fActor, S->FillColor, S->AmbientColor, S->AmbientFromFloor);
			}
		}
	}
}

/* ------------------------------------------------------------------------------------ */
//	Destructor
/* ------------------------------------------------------------------------------------ */
CFlipTree::~CFlipTree()
{
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "FlipTree");

	if(!pSet)
		return;

	geEntity *pEntity;

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		FlipTree *S = static_cast<FlipTree*>(geEntity_GetUserData(pEntity));

		if(S->fActor)
		{
			CCD->ActorManager()->RemoveActorCheck(S->fActor);
			geActor_Destroy(&(S->fActor));
			S->fActor = NULL;
		}
	}
}

/* ------------------------------------------------------------------------------------ */
//	Tick
/* ------------------------------------------------------------------------------------ */
void CFlipTree::Tick(geFloat /*dwTicks*/)
{
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "FlipTree");

	if(!pSet)
		return;

	geEntity *pEntity;
	geFloat range_squared;
	geVec3d Pos1;
	GE_LVertex	Vertex;

	Vertex.a = 255.0f;
	Vertex.u = 0;
	Vertex.v = 0;

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		FlipTree *S = static_cast<FlipTree*>(geEntity_GetUserData(pEntity));

		if(!S->fActor)
			continue;

		// Speed Enhancment
		if(!geWorld_IsActorPotentiallyVisible(CCD->World() , S->fActor, CCD->CameraManager()->Camera()))
			continue;

		// get position of camera
		CCD->CameraManager()->GetPosition(&Pos1);

		//check distance
		range_squared = geVec3d_DistanceBetweenSquared(&S->origin, &Pos1);

		// render poly outside of range or actor inside range.
		if(range_squared > (S->Range * S->Range) && S->Bitmap)
		{
			//draw a poly
			CCD->ActorManager()->SetActorFlags(S->fActor, 0);

			if(range_squared < (S->ClipRange * S->ClipRange))
			{
				// set poly color
				Vertex.r = S->BitmapColor.r;
				Vertex.g = S->BitmapColor.g;
				Vertex.b = S->BitmapColor.b;
				Vertex.X = S->origin.X;
				Vertex.Y = S->origin.Y + ((geBitmap_Height(S->Bitmap)*0.5f)*S->Scale);
				Vertex.Z = S->origin.Z;

				geWorld_AddPolyOnce(CCD->World(), &Vertex, 1,
					S->Bitmap, GE_TEXTURED_POINT,
					GE_RENDER_DEPTH_SORT_BF | GE_RENDER_DO_NOT_OCCLUDE_OTHERS,
					S->Scale);
			}
		}
		else
		{
			//render the actor
			CCD->ActorManager()->SetActorFlags(S->fActor, GE_ACTOR_RENDER_NORMAL | GE_ACTOR_COLLIDE);
		}
	}
}


/* ----------------------------------- END OF FILE ------------------------------------ */
