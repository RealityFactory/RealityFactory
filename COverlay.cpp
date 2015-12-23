/************************************************************************************//**
 * @file COverlay.cpp
 * @brief Overlay class implementation
 *
 * This file contains the class implementation for the COverlay class.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

// Include the One True Header
#include "RabidFramework.h"

extern geBitmap *TPool_Bitmap(const char *DefaultBmp, const char *DefaultAlpha,
							  const char *BName, const char *AName);

/* ------------------------------------------------------------------------------------ */
//	Constructor
/* ------------------------------------------------------------------------------------ */
COverlay::COverlay()
{
	// Ok, see if we have any Overlay entities at all
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "Overlay");

	if(!pSet)
		return;

	geEntity *pEntity;

	// Look through all of our Overlays
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Overlay *pItem = static_cast<Overlay*>(geEntity_GetUserData(pEntity));

		if(EffectC_IsStringNull(pItem->szEntityName))
		{
			char szName[128];
			geEntity_GetName(pEntity, szName, 128);
			pItem->szEntityName = szName;
		}

		// Ok, put this entity into the Global Entity Registry
		CCD->EntityRegistry()->AddEntity(pItem->szEntityName, "Overlay");

		if(pItem->Model)
			CCD->ModelManager()->AddModel(pItem->Model, ENTITY_OVERLAY);

		pItem->FBitmap = NULL;
		pItem->Texture = NULL;

		if(!pItem->Animated)
		{
			pItem->Texture = TPool_Bitmap(pItem->Bitmap, pItem->Alphamap, NULL, NULL);
		}
		else
		{
			pItem->FBitmap = static_cast<geBitmap**>(geRam_AllocateClear(sizeof(*(pItem->FBitmap))*pItem->BitmapCount));

			for(int i=0; i<pItem->BitmapCount; ++i)
			{
				char BmpName[256];

				// build bmp and alpha names
				sprintf(BmpName, "%s%d%s", pItem->Bitmap, i, ".bmp");

				if(!EffectC_IsStringNull(pItem->Alphamap))
				{
					char AlphaName[256];
					sprintf( AlphaName, "%s%d%s", pItem->Alphamap, i, ".bmp" );
					pItem->FBitmap[i] = TPool_Bitmap(BmpName, AlphaName, NULL, NULL);
				}
				else
				{
					pItem->FBitmap[i] = TPool_Bitmap(BmpName, BmpName, NULL, NULL);
				}
			}

			pItem->CurTex	= 0;
			pItem->Time		= 0.0f;
			pItem->CycleDir = 1;
			pItem->Texture	= pItem->FBitmap[pItem->CurTex];
			pItem->Alpha	= pItem->Transparency;
		}
	}
}

/* ------------------------------------------------------------------------------------ */
//	Destructor
/* ------------------------------------------------------------------------------------ */
COverlay::~COverlay()
{
	// Ok, see if we have any Overlay entities at all
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "Overlay");

	if(!pSet)
		return;

	geEntity *pEntity;

	// Look through all of our Overlays
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Overlay *pItem = static_cast<Overlay*>(geEntity_GetUserData(pEntity));

		if(pItem->FBitmap)
			geRam_Free(pItem->FBitmap);
	}
}

/* ------------------------------------------------------------------------------------ */
//	IsOverlay
/* ------------------------------------------------------------------------------------ */
Overlay *COverlay::IsOverlay(const geWorld_Model *theModel)
{
	// Ok, see if we have any Overlay entities at all
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "Overlay");

	if(!pSet)
		return NULL;

	geEntity *pEntity;

	// Look through all of our Overlays
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Overlay *pItem = static_cast<Overlay*>(geEntity_GetUserData(pEntity));

		if(!EffectC_IsStringNull(pItem->TriggerName))
		{
			if(!GetTriggerState(pItem->TriggerName))
				continue;
		}

		if(pItem->Model)
		{
			if(pItem->Model == theModel)
			{
				return pItem;
			}
		}
	}

	return NULL;
}

/* ------------------------------------------------------------------------------------ */
//	Tick
/* ------------------------------------------------------------------------------------ */
void COverlay::Tick(geFloat dwTicks)
{
	// Ok, see if we have any Overlay entities at all
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "Overlay");

	if(!pSet)
		return;

	geEntity *pEntity;

	// Look through all of our Overlays
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Overlay *pItem = static_cast<Overlay*>(geEntity_GetUserData(pEntity));

		if(!EffectC_IsStringNull(pItem->TriggerName))
		{
			if(!GetTriggerState(pItem->TriggerName))
			{
				pItem->Alpha=pItem->Transparency; // reset alpha when trigger is off
				continue;
			}
		}

		if(pItem->Animated)
		{
			pItem->Time += dwTicks;

			if(pItem->AlphaRate != 0.0f)
			{
				pItem->Alpha += (pItem->AlphaRate*(dwTicks*0.001f));

				if(pItem->Alpha < 0)
					pItem->Alpha = 0;
				else if(pItem->Alpha > 255.0f)
					pItem->Alpha = 255.0f;
			}

			if(pItem->Time >= (1000.0f/pItem->Speed))
			{
				pItem->Time = 0.0f;
				switch(pItem->Style)
				{
					case 0:
						break;
					case 2:
						pItem->CurTex += pItem->CycleDir;
						if(pItem->CurTex >= pItem->BitmapCount || pItem->CurTex < 0)
						{
							pItem->CycleDir = -pItem->CycleDir;
							pItem->CurTex += pItem->CycleDir;
						}
						break;
					case 3:
						pItem->CurTex = (rand() % pItem->BitmapCount);
						break;
					case 4:
						pItem->CurTex += 1;
						if(pItem->CurTex >= pItem->BitmapCount)
							pItem->CurTex = pItem->BitmapCount - 1;
						break;
					default:
						pItem->CurTex += 1;
						if(pItem->CurTex >= pItem->BitmapCount)
							pItem->CurTex = 0;
						break;
				}
			}

			pItem->Texture = pItem->FBitmap[pItem->CurTex];
		}
	}
}

/* ------------------------------------------------------------------------------------ */
//	Render
/* ------------------------------------------------------------------------------------ */
void COverlay::Render()
{
	// Ok, see if we have any Overlay entities at all
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "Overlay");

	if(!pSet)
		return;

	geEntity *pEntity;

	// Look through all of our Overlays
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Overlay *pItem = static_cast<Overlay*>(geEntity_GetUserData(pEntity));

		if(!EffectC_IsStringNull(pItem->TriggerName))
		{
			if(!GetTriggerState(pItem->TriggerName))
				continue;
		}

		if(!pItem->Model)
		{
			geVec3d Direction, Pos;
			GE_LVertex	Vertex;
			geXForm3d theViewPoint = CCD->CameraManager()->ViewPoint();
			geXForm3d_GetIn(&theViewPoint, &Direction);

			geVec3d_AddScaled(&(theViewPoint.Translation), &Direction, CCD->CameraManager()->GetOverlayDist(), &Pos);

			Vertex.r = pItem->TintColor.r;
			Vertex.g = pItem->TintColor.g;
			Vertex.b = pItem->TintColor.b;
			Vertex.a = pItem->Alpha;

			Vertex.u = 0.0f;
			Vertex.v = 0.0f;

			Vertex.X = Pos.X;
			Vertex.Y = Pos.Y;
			Vertex.Z = Pos.Z;

			geWorld_AddPolyOnce(CCD->World(), &Vertex, 1, pItem->Texture,
				GE_TEXTURED_POINT, GE_RENDER_DO_NOT_OCCLUDE_SELF, 2.0f);
		}
	}

	return;
}


/* ----------------------------------- END OF FILE ------------------------------------ */
