/************************************************************************************//**
 * @file CLiquid.cpp
 * @brief Liquid class implementation
 *
 * This file contains the class implementation for the CLiquid and COverlay class.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

// Include the One True Header
#include "RabidFramework.h"

extern geSound_Def *SPool_Sound(const char *SName);
extern geBitmap *TPool_Bitmap(const char *DefaultBmp, const char *DefaultAlpha,
							  const char *BName, const char *AName);

/* ------------------------------------------------------------------------------------ */
//	Constructor
/* ------------------------------------------------------------------------------------ */
CLiquid::CLiquid()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	// Ok, see if we have any Liquid entities at all
	pSet = geWorld_GetEntitySet(CCD->World(), "Liquid");

	if(!pSet)
		return;

	// Look through all of our Liquids
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Liquid *pItem = static_cast<Liquid*>(geEntity_GetUserData(pEntity));

		if(EffectC_IsStringNull(pItem->szEntityName))
		{
			char szName[128];
			geEntity_GetName(pEntity, szName, 128);
			pItem->szEntityName = szName;
		}

		// Ok, put this entity into the Global Entity Registry
		CCD->EntityRegistry()->AddEntity(pItem->szEntityName, "Liquid");
		CCD->ModelManager()->AddModel(pItem->Model, ENTITY_LIQUID);
		pItem->Texture = TPool_Bitmap(pItem->Bitmap, pItem->Alphamap, NULL, NULL);

		if(!EffectC_IsStringNull(pItem->SurfaceSound))
			SPool_Sound(pItem->SurfaceSound);

		if(!EffectC_IsStringNull(pItem->SwimSound))
			SPool_Sound(pItem->SwimSound);

		if(!EffectC_IsStringNull(pItem->InLiquidSound))
			SPool_Sound(pItem->InLiquidSound);
	}
}

/* ------------------------------------------------------------------------------------ */
//	Destructor
/* ------------------------------------------------------------------------------------ */
CLiquid::~CLiquid()
{
}

/* ------------------------------------------------------------------------------------ */
//	IsLiquid
/* ------------------------------------------------------------------------------------ */
Liquid *CLiquid::IsLiquid(const geWorld_Model *theModel)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	// Ok, see if we have any Liquid entities at all
	pSet = geWorld_GetEntitySet(CCD->World(), "Liquid");

	if(!pSet)
		return NULL;

	// Look through all of our Liquids
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Liquid *pItem = static_cast<Liquid*>(geEntity_GetUserData(pEntity));

		if(pItem->Model == theModel)
		{
			return pItem;
		}
	}

	return NULL;
}


/* ----------------------------------- END OF FILE ------------------------------------ */
