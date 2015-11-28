/************************************************************************************//**
 * @file CCorona.cpp
 * @brief This file contains the class implementation for Corona handling.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

//	Include the One True Header
#include "RabidFramework.h"

extern geBitmap *TPool_Bitmap(char *DefaultBmp, char *DefaultAlpha, char *BName, char *AName);

/* ------------------------------------------------------------------------------------ */
//	Constructor
/* ------------------------------------------------------------------------------------ */
CCorona::CCorona()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

  	Count = 0;

	// Ok, check to see if there are TEMPLATE in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "Corona");

	if(!pSet)
		return;

  	CoronaBitmap = TPool_Bitmap("Corona.Bmp", "Corona_a.Bmp", NULL, NULL);

  	if(!CoronaBitmap)
    		return;

	// Ok, we have Coronas somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	    pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Corona *pSource = (Corona*)geEntity_GetUserData(pEntity);

		if(EffectC_IsStringNull(pSource->szEntityName))
		{
			char szName[128];
			geEntity_GetName(pEntity, szName, 128);
			pSource->szEntityName = szName;
		}

		Count++;						// Kick source count

		// Ok, put this entity into the Global Entity Registry
		CCD->EntityRegistry()->AddEntity(pSource->szEntityName, "Corona");
		pSource->OriginOffset = pSource->origin;

		if(pSource->Model)
		{
			geVec3d ModelOrigin;
	    	geWorld_GetModelRotationalCenter(CCD->World(), pSource->Model, &ModelOrigin);
			geVec3d_Subtract(&pSource->origin, &ModelOrigin, &pSource->OriginOffset);
  		}

		// Reset all the data for each Corona
		pSource->effect = (int *)malloc(sizeof(int) * 1);
		pSource->effect[0] = -1;
		pSource->active = GE_FALSE;
	}

	//  let's go!
	return;
}

/* ------------------------------------------------------------------------------------ */
//	Create
/* ------------------------------------------------------------------------------------ */
int CCorona::Create(const geVec3d &Origin, Corona *pCorona)
{
	int effect = -1;
	EffCorona C;

	memset( &C, 0, sizeof(C) );
	C.Texture = CoronaBitmap;
	C.FadeTime = pCorona->FadeTime;
	C.MinRadius = pCorona->MinRadius;
	C.MaxRadius = pCorona->MaxRadius;
	C.MaxVisibleDistance = pCorona->MaxVisibleDistance;
	C.MinRadiusDistance = pCorona->MinRadiusDistance;
	C.MaxRadiusDistance = pCorona->MaxRadiusDistance;
	C.Vertex.X = Origin.X;
	C.Vertex.Y = Origin.Y;
	C.Vertex.Z = Origin.Z;
	C.Vertex.r = pCorona->Color.r;
	C.Vertex.g = pCorona->Color.g;
	C.Vertex.b = pCorona->Color.b;

	effect = CCD->EffectManager()->Item_Add(EFF_CORONA, (void*)&C);

	return effect;
}


/* ------------------------------------------------------------------------------------ */
//	Destructor
//
//	Clean up all audio we have loaded.
/* ------------------------------------------------------------------------------------ */
CCorona::~CCorona()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

  	if(Count == 0)
		return;						// Don't waste CPU cycles

	// Ok, check to see if there are Coronas in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "Corona");

	if(!pSet)
		return;				// No sources

	// Ok, we have Coronas somewhere.  Dig through 'em all and release
	// everything
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	    pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Corona *pSource = (Corona*)geEntity_GetUserData(pEntity);
		free(pSource->effect);
	}

	// Bail this mess.
	return;
}

/* ------------------------------------------------------------------------------------ */
//	Tick
/* ------------------------------------------------------------------------------------ */
void CCorona::Tick(float dwTicks)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	if(Count == 0)
		return;						// Don't waste CPU cycles

	// Ok, check to see if there are Coronas in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "Corona");

	if(!pSet)
		return;									// No sources

	// Ok, we have Coronas somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	    pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Corona *pSource = (Corona*)geEntity_GetUserData(pEntity);

		if(!EffectC_IsStringNull(pSource->TriggerName))
		{
			if(GetTriggerState(pSource->TriggerName))
			{
				if(pSource->active == GE_FALSE)
				{
					pSource->effect[0] = Create(pSource->origin, pSource);
					pSource->active = GE_TRUE;
				}
			}
			else
			{
				if(pSource->effect[0] != -1)
				{
    				CCD->EffectManager()->Item_Delete(EFF_CORONA, pSource->effect[0]);
					pSource->effect[0] = -1;
				}

				pSource->active = GE_FALSE;
			}
		}
		else
		{
			if(pSource->active == GE_FALSE)
			{
				pSource->effect[0] = Create(pSource->origin, pSource);
				pSource->active = GE_TRUE;
			}
		}

		if(pSource->active == GE_TRUE)
		{
			pSource->origin = pSource->OriginOffset;

			if(SetOriginOffset(pSource->EntityName, pSource->BoneName, pSource->Model, &(pSource->origin)))
			{
				EffCorona C;
				C.Vertex.X = pSource->origin.X;
				C.Vertex.Y = pSource->origin.Y;
				C.Vertex.Z = pSource->origin.Z;
      			CCD->EffectManager()->Item_Modify(EFF_CORONA, pSource->effect[0], (void*)&C, CORONA_POS);
			}
		}
	}

	return;
}

//	******************** CRGF Overrides ********************

/* ------------------------------------------------------------------------------------ */
//	LocateEntity
//
//	Given a name, locate the desired item in the currently loaded level
//	..and return it's user data.
/* ------------------------------------------------------------------------------------ */
int CCorona::LocateEntity(char *szName, void **pEntityData)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	// Ok, check to see if there are Corona in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "Corona");

	if(!pSet)
		return RGF_NOT_FOUND;									// No 3D audio sources

	// Ok, we have Corona somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	    pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Corona *pSource = (Corona*)geEntity_GetUserData(pEntity);

		if(!strcmp(pSource->szEntityName, szName))
		{
			*pEntityData = (void*)pSource;
			return RGF_SUCCESS;
		}
	}

	return RGF_NOT_FOUND;		// Sorry, no such entity here
}

/* ------------------------------------------------------------------------------------ */
//	ReSynchronize
//
//	Correct internal timing to match current time, to make up for time lost
//	..when outside the game loop (typically in "menu mode").
/* ------------------------------------------------------------------------------------ */
int CCorona::ReSynchronize()
{
	return RGF_SUCCESS;
}


/* ----------------------------------- END OF FILE ------------------------------------ */
