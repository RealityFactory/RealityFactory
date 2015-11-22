/*
	CLiquid.cpp:		Liquid class implementation

	(c) 2001 Ralph Deane
	All Rights Reserved

	This file contains the class implementation for CLiquid
handling.
*/

//	Include the One True Header

#include "RabidFramework.h"

extern geBitmap *TPool_Bitmap(char *DefaultBmp, char *DefaultAlpha, char *BName, char *AName);
extern geSound_Def *SPool_Sound(char *SName);

CLiquid::CLiquid()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	//	Ok, see if we have any Liquid entities at all
	
	pSet = geWorld_GetEntitySet(CCD->World(), "Liquid");
	
	if(!pSet) 
		return;	

	//	Look through all of our Liquids
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		Liquid *pItem = (Liquid*)geEntity_GetUserData(pEntity);
		if(EffectC_IsStringNull(pItem->szEntityName))
		{
			char szName[128];
			geEntity_GetName(pEntity, szName, 128);
			pItem->szEntityName = szName;
		}
		// Ok, put this entity into the Global Entity Registry
		CCD->EntityRegistry()->AddEntity(pItem->szEntityName, "Liquid");
		CCD->ModelManager()->AddModel(pItem->Model, ENTITY_LIQUID);
		pItem->Texture = TPool_Bitmap(pItem->Bitmap, pItem->Alphamap, "water.Bmp", "a_water.Bmp");
		if(!EffectC_IsStringNull(pItem->SurfaceSound))
			SPool_Sound(pItem->SurfaceSound);
		if(!EffectC_IsStringNull(pItem->SwimSound))
			SPool_Sound(pItem->SwimSound);
		if(!EffectC_IsStringNull(pItem->InLiquidSound))
			SPool_Sound(pItem->InLiquidSound);
	}
}

CLiquid::~CLiquid()
{
}

Liquid *CLiquid::IsLiquid(geWorld_Model *theModel)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	//	Ok, see if we have any Liquid entities at all
	
	pSet = geWorld_GetEntitySet(CCD->World(), "Liquid");
	
	if(!pSet) 
		return NULL;	
	
	//	Look through all of our Liquids
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		Liquid *pItem = (Liquid*)geEntity_GetUserData(pEntity);

		if(pItem->Model == theModel)
		{
			return pItem;
		}
	}
	return NULL;
}