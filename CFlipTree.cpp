// Pickles Aug 04

#include "RabidFramework.h"
#include <Ram.h>

extern geBitmap *TPool_Bitmap(char *DefaultBmp, char *DefaultAlpha, char *BName, char *AName);

float FastDistance(geVec3d Pos1,geVec3d Pos2)
{
	geVec3d B;
	geVec3d_Subtract(&Pos1,&Pos2,&B);
	return geVec3d_DotProduct(&B,&B);

}

CFlipTree::CFlipTree()
{
	geEntity_EntitySet *	Set;
	geEntity *		Entity;

	Set = geWorld_GetEntitySet(CCD->World(), "FlipTree");
	if (Set == NULL)
	    return;

	Entity = geEntity_EntitySetGetNextEntity(Set, NULL);

	while(Entity)
	{
		FlipTree *	S;

		S = (FlipTree *)geEntity_GetUserData(Entity);
		if(EffectC_IsStringNull(S->szEntityName))
		{
			// Get the Entity Name
			char szName[128];
			geEntity_GetName(Entity, szName, 128);
			S->szEntityName = szName;
		}

		//Add it to Entity Registry
		CCD->EntityRegistry()->AddEntity(S->szEntityName, "FlipTree");

		if(!EffectC_IsStringNull(S->BitmapName))
		{
			S->Bitmap = (geBitmap *)geRam_AllocateClear(sizeof(S->Bitmap));

			if(!EffectC_IsStringNull(S->AlphamapName))
				S->Bitmap = TPool_Bitmap(S->BitmapName, S->AlphamapName, NULL, NULL);
			else
				S->Bitmap = TPool_Bitmap(S->BitmapName , S->BitmapName , NULL, NULL);
		}

		//shadow image
		if(!EffectC_IsStringNull(S->ShadowMapName))
		{
			S->Shadow = (geBitmap *)geRam_AllocateClear(sizeof(S->Shadow));
			if(!EffectC_IsStringNull(S->ShadowAlphaName))
				S->Shadow = TPool_Bitmap(S->ShadowMapName , S->ShadowAlphaName , NULL, NULL);
			else
				S->Shadow = TPool_Bitmap(S->ShadowMapName, S->ShadowMapName , NULL, NULL);
		}

		//load the actor
		if(!EffectC_IsStringNull(S->ActorName))
		{
			int RenderFlag = 0;

			S->fActor = CCD->ActorManager()->LoadActor(S->ActorName,NULL);
			if(S->fActor)
			{
				CCD->ActorManager()->SetActorFlags(S->fActor,RenderFlag);
				CCD->ActorManager()->Position(S->fActor,S->origin);
				CCD->ActorManager()->Rotate(S->fActor,S->Rotation);
				CCD->ActorManager()->SetScale(S->fActor,S->Scale);
				CCD->ActorManager()->SetHideRadar(S->fActor,true);
				CCD->ActorManager()->SetBoxChange(S->fActor, false);
				CCD->ActorManager()->SetBBox(S->fActor,S->BoxSize.X,S->BoxSize.Y,S->BoxSize.Z);
				CCD->ActorManager()->SetShadow(S->fActor,S->ShadowSize);
				CCD->ActorManager()->SetShadowAlpha(S->fActor, 255.0f);
				CCD->ActorManager()->SetShadowBitmap(S->fActor,S->Shadow);
				CCD->ActorManager()->SetActorDynamicLighting(S->fActor, S->FillColor, S->AmbientColor, S->AmbientFromFloor);
			}
		}
		Entity = geEntity_EntitySetGetNextEntity(Set, Entity);
	}
	return;
}


void CFlipTree::Tick(float dwTicksIn)
{
	geEntity_EntitySet *	Set;
	geEntity *		Entity;
	geFloat rng;
	geVec3d Pos1;
	GE_LVertex	Vertex;
	int RenderFlag;


	Set = geWorld_GetEntitySet(CCD->World(), "FlipTree");
	if (Set == NULL)
	    return;

	Entity = geEntity_EntitySetGetNextEntity(Set, NULL);

	Vertex.a = 255.0f;
	Vertex.u = 0;
	Vertex.v = 0;

	while(Entity)
	{
	    FlipTree *	S;

		S = (FlipTree *)geEntity_GetUserData(Entity);

		// Speed Enhancment
		if(!geWorld_IsActorPotentiallyVisible(CCD->World() , S->fActor, CCD->CameraManager()->Camera()))
			continue;

		// get position of camera
		CCD->CameraManager()->GetPosition(&Pos1);

		//check distance
		//rng = geVec3d_DistanceBetween(&S->origin,&Pos1);
		rng = FastDistance(S->origin,Pos1);

		// render poly outside of range or actor inside range.
		if(rng > (S->Range * S->Range))
		{
			//draw a poly
			RenderFlag = 0;//not rendered
			CCD->ActorManager()->SetActorFlags(S->fActor,RenderFlag);
			if(rng < (S->ClipRange * S->ClipRange))
			{
				// set poly color
				Vertex.r = S->BitmapColor.r;
				Vertex.g = S->BitmapColor.g;
				Vertex.b = S->BitmapColor.b;
				Vertex.X = S->origin.X;
				Vertex.Y = S->origin.Y + ((geBitmap_Height(S->Bitmap)/2.f)*S->Scale);
				Vertex.Z = S->origin.Z;
				geWorld_AddPolyOnce(CCD->World(), &Vertex, 1,
					S->Bitmap, GE_TEXTURED_POINT,
					GE_RENDER_DEPTH_SORT_BF | GE_RENDER_DO_NOT_OCCLUDE_OTHERS,
					S->Scale );//replace 1 with scale
			}
		}
		else
		{
			//render the actor
			RenderFlag = GE_ACTOR_RENDER_NORMAL | GE_ACTOR_COLLIDE;
			if(S->fActor)
				CCD->ActorManager()->SetActorFlags(S->fActor,RenderFlag);
		}

		Entity = geEntity_EntitySetGetNextEntity(Set, Entity);
	}


}


CFlipTree::~CFlipTree()
{

	geEntity_EntitySet *	Set;
	geEntity *		Entity;

	Set = geWorld_GetEntitySet(CCD->World(), "FlipTree");
	if (Set == NULL)
	    return;

	Entity = geEntity_EntitySetGetNextEntity(Set, NULL);

	while(Entity)
	{
	    FlipTree *	S;

		S = (FlipTree *)geEntity_GetUserData(Entity);
		if(S->Bitmap)
			geRam_Free(S->Bitmap);

		Entity = geEntity_EntitySetGetNextEntity(Set, Entity);
	}

	return;
}

