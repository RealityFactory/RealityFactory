/************************************************************************************//**
 * @file CCorona.cpp
 * @brief This file contains the class implementation for Corona handling.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

//	Include the One True Header
#include "RabidFramework.h"
#include "CCorona.h"

extern geBitmap *TPool_Bitmap(const char *DefaultBmp, const char *DefaultAlpha,
							  const char *BName, const char *AName);

/* ------------------------------------------------------------------------------------ */
//	Constructor
/* ------------------------------------------------------------------------------------ */
CCorona::CCorona() :
	m_EntityCount(0)
{
	// Ok, check to see if there are TEMPLATE in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "Corona");

	if(!pSet)
		return;

	m_Bitmap = TPool_Bitmap("corona.bmp", "a_corona.bmp", NULL, NULL);

	if(!m_Bitmap)
		return;

	geEntity *pEntity;

	// Ok, we have Coronas somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Corona *pSource = static_cast<Corona*>(geEntity_GetUserData(pEntity));

		if(EffectC_IsStringNull(pSource->szEntityName))
		{
			char szName[128];
			geEntity_GetName(pEntity, szName, 128);
			pSource->szEntityName = szName;
		}

		++m_EntityCount;					// Kick Corona count

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
		pSource->effect = -1;
		pSource->active = GE_FALSE;
	}

	// let's go!
}

/* ------------------------------------------------------------------------------------ */
//	Create
/* ------------------------------------------------------------------------------------ */
int CCorona::Create(const geVec3d &Origin, Corona *pCorona)
{
	EffCorona C;

	memset(&C, 0, sizeof(C));
	C.Texture = m_Bitmap;
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

	int effect = CCD->EffectManager()->Item_Add(EFF_CORONA, static_cast<void*>(&C));

	return effect;
}


/* ------------------------------------------------------------------------------------ */
//	Destructor
//
//	Clean up all audio we have loaded.
/* ------------------------------------------------------------------------------------ */
CCorona::~CCorona()
{
}

/* ------------------------------------------------------------------------------------ */
//	Tick
/* ------------------------------------------------------------------------------------ */
void CCorona::Tick(geFloat /*dwTicks*/)
{
	if(m_EntityCount == 0)
		return;						// Don't waste CPU cycles

	// Ok, check to see if there are Coronas in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "Corona");

	if(!pSet)
		return;									// No sources

	geEntity *pEntity;

	// Ok, we have Coronas somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Corona *pSource = static_cast<Corona*>(geEntity_GetUserData(pEntity));

		if(!EffectC_IsStringNull(pSource->TriggerName))
		{
			if(GetTriggerState(pSource->TriggerName))
			{
				if(pSource->active == GE_FALSE)
				{
					pSource->effect = Create(pSource->origin, pSource);
					pSource->active = GE_TRUE;
				}
			}
			else
			{
				if(pSource->effect != -1)
				{
					CCD->EffectManager()->Item_Delete(EFF_CORONA, pSource->effect);
					pSource->effect = -1;
				}

				pSource->active = GE_FALSE;
			}
		}
		else
		{
			if(pSource->active == GE_FALSE)
			{
				pSource->effect = Create(pSource->origin, pSource);
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
				CCD->EffectManager()->Item_Modify(EFF_CORONA, pSource->effect, static_cast<void*>(&C), CORONA_POS);
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
int CCorona::LocateEntity(const char *szName, void **pEntityData)
{
	// Ok, check to see if there are Corona in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "Corona");

	if(!pSet)
		return RGF_NOT_FOUND;									// No 3D audio sources

	geEntity *pEntity;

	// Ok, we have Corona somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Corona *pSource = static_cast<Corona*>(geEntity_GetUserData(pEntity));

		if(!strcmp(pSource->szEntityName, szName))
		{
			*pEntityData = static_cast<void*>(pSource);
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
