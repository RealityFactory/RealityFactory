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

// Overlay

COverlay::COverlay()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	//	Ok, see if we have any Overlay entities at all
	
	pSet = geWorld_GetEntitySet(CCD->World(), "Overlay");
	
	if(!pSet) 
		return;	

	//	Look through all of our Overlays
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		Overlay *pItem = (Overlay*)geEntity_GetUserData(pEntity);
		if(EffectC_IsStringNull(pItem->szEntityName))
		{
			char szName[128];
			geEntity_GetName(pEntity, szName, 128);
			pItem->szEntityName = szName;
		}
		// Ok, put this entity into the Global Entity Registry
		CCD->EntityRegistry()->AddEntity(pItem->szEntityName, "Overlay");
		CCD->ModelManager()->AddModel(pItem->Model, ENTITY_OVERLAY);
		pItem->FBitmap = NULL;
		pItem->Texture = NULL;
		if(!pItem->Animated)
			pItem->Texture = TPool_Bitmap(pItem->Bitmap, pItem->Alphamap, "water.Bmp", "a_water.Bmp");
		else
		{
			pItem->FBitmap = (geBitmap **)geRam_AllocateClear( sizeof( *(pItem->FBitmap ) ) * pItem->BitmapCount );
			for (int i = 0; i < pItem->BitmapCount; i++ )
			{
				char BmpName[256];
				char AlphaName[256];
				// build bmp and alpha names
				sprintf( BmpName, "%s%d%s", pItem->Bitmap, i, ".bmp" );
				if(!EffectC_IsStringNull(pItem->Alphamap))
				{
					sprintf( AlphaName, "%s%d%s", pItem->Alphamap, i, ".bmp" );
					pItem->FBitmap[i] = TPool_Bitmap(BmpName, AlphaName, NULL, NULL);
				}
				else
				{
					pItem->FBitmap[i] = TPool_Bitmap(BmpName, BmpName, NULL, NULL);
				}
			}
			pItem->CurTex = 0;
			pItem->Time = 0.0f;
			pItem->CycleDir = 1;
			pItem->Texture = pItem->FBitmap[pItem->CurTex];
		}
	}
}

COverlay::~COverlay()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	//	Ok, see if we have any Overlay entities at all
	
	pSet = geWorld_GetEntitySet(CCD->World(), "Overlay");
	
	if(!pSet) 
		return;	
	
	//	Look through all of our Overlays
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		Overlay *pItem = (Overlay*)geEntity_GetUserData(pEntity);
		if(pItem->FBitmap)
			geRam_Free(pItem->FBitmap);
	}
}

Overlay *COverlay::IsOverlay(geWorld_Model *theModel)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	//	Ok, see if we have any Overlay entities at all
	
	pSet = geWorld_GetEntitySet(CCD->World(), "Overlay");
	
	if(!pSet) 
		return NULL;	
	
	//	Look through all of our Overlays
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		Overlay *pItem = (Overlay*)geEntity_GetUserData(pEntity);

		if(pItem->Model == theModel)
		{
			return pItem;
		}
	}
	return NULL;
}

void COverlay::Tick(float dwTicks)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	//	Ok, see if we have any Overlay entities at all
	
	pSet = geWorld_GetEntitySet(CCD->World(), "Overlay");
	
	if(!pSet) 
		return;	
	
	//	Look through all of our Overlays
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		Overlay *pItem = (Overlay*)geEntity_GetUserData(pEntity);
		if(pItem->Animated)
		{
			pItem->Time += dwTicks;
			if(pItem->Time>=(1000.0f*(1.0f/pItem->Speed)))
			{
				pItem->Time = 0.0f;
				switch(pItem->Style)
				{
					case 0:
						break;
					case 2:
						pItem->CurTex += pItem->CycleDir;
						if(pItem->CurTex>=pItem->BitmapCount || pItem->CurTex<0)
						{
							pItem->CycleDir = -pItem->CycleDir;
							pItem->CurTex += pItem->CycleDir;
						}
						break;
					case 3:
						pItem->CurTex = ( rand() % pItem->BitmapCount );
						break;
					default:
						pItem->CurTex +=1;
						if(pItem->CurTex>=pItem->BitmapCount)
							pItem->CurTex = 0;
						break;
				}
			}
			pItem->Texture = pItem->FBitmap[pItem->CurTex];
		}
	}
}