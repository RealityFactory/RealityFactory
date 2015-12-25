/************************************************************************************//**
 * @file Chaos.cpp
 * @brief Chaos handler
 *
 * This file contains the class implementation for the Chaos class that
 * encapsulates chaos based special effects for RGF-based games.
 * @author Ralph Deane
 *//*
 * Copyright (c) 1999 Ralph Deane; All rights reserved.
 ****************************************************************************************/

// Include the One True Header
#include "RabidFramework.h"
#include "Chaos.h"

/* ------------------------------------------------------------------------------------ */
//	Constructor
/* ------------------------------------------------------------------------------------ */
Chaos::Chaos()
{
	geBitmap_Info		AttachInfo;
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "EChaos");

	if(!pSet)
		return;	// Not on this level.

	geEntity			*pEntity;

	// Ok, we have procedurals somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		EChaos *pTex = static_cast<EChaos*>(geEntity_GetUserData(pEntity));

			pTex->SegmentSize = 1;

// changed QD 08/30/02
			pTex->Actor = NULL;
			pTex->CAttachBmp = NULL;

			if(!EffectC_IsStringNull(pTex->EntityName))
			{
				pTex->Actor = GetEntityActor(pTex->EntityName);

				if(!pTex->Actor)
				{
					// changed QD 07/15/06
					char szName[128];
					char szError[256];
					geEntity_GetName(pEntity, szName, 128);

					sprintf(szError, "[WARNING] File %s - Line %d: %s: Missing actor '%s'\n",
							__FILE__, __LINE__, szName, pTex->EntityName);
					CCD->ReportError(szError, false);
					/*
					CCD->ShutdownLevel();
					delete CCD;
					CCD = NULL;
					MessageBox(NULL, szError,"EChaos", MB_OK);
					exit(-333);
					*/
					continue;
					// end change
				}
			}

			// get the output bitmap as an actor bitmap...
			if(pTex->Actor != NULL)
			{
				// locals
				geActor_Def *ActorDef;
				geBody *Body;
				int MaterialCount;
				int i;
				const char *MaterialName;
				float R, G, B;
				int Length;

				// get actor material count
				ActorDef = geActor_GetActorDef(pTex->Actor);

				if(ActorDef == NULL)
					continue;

				Body = geActor_GetBody(ActorDef);

				if(Body == NULL)
                       continue;

				MaterialCount = geActor_GetMaterialCount(pTex->Actor);

				// get bitmap pointer
				Length = strlen(pTex->AttachBmp);

				for(i=0; i<MaterialCount; i++)
				{
					if(geBody_GetMaterial(Body, i, &MaterialName, &(pTex->CAttachBmp), &R, &G, &B) == GE_FALSE)
						continue;

					if(strnicmp(pTex->AttachBmp, MaterialName, Length) == 0)
						break;
				}

				if(i == MaterialCount)
				{
					char szName[128];
					char szError[256];
					geEntity_GetName(pEntity, szName, 128);
					sprintf(szError, "[WARNING] File %s - Line %d: %s: Missing ActorMaterial %s\n",
						__FILE__, __LINE__, szName, pTex->AttachBmp);
					CCD->ReportError(szError, false);
					pTex->CAttachBmp = NULL;
					continue;
					/*
					CCD->ShutdownLevel();
					delete CCD;
					MessageBox(NULL, szError,"EChaos", MB_OK);
					exit(-333); //or continue
					*/
				}
			}
			else	// ...or a world bitmap
			{
				pTex->CAttachBmp = geWorld_GetBitmapByName(CCD->World(), pTex->AttachBmp);

				if(pTex->CAttachBmp == NULL)
				{
					char szName[128];
					char szError[256];
					geEntity_GetName(pEntity, szName, 128);
					sprintf(szError, "[WARNING] File %s - Line %d: %s: Missing Texture %s in level\n",
						__FILE__, __LINE__, szName, pTex->AttachBmp);
					CCD->ReportError(szError, false);
					continue;
					/*
					CCD->ShutdownLevel();
					delete CCD;
					MessageBox(NULL, szError,"EChaos", MB_OK);
					exit(-333); //or continue
					*/
               }
			}
// end change QD 08/30/02

			//if(pTex->CAttachBmp == NULL)
			//	continue;

			geBitmap_SetFormatMin(pTex->CAttachBmp, CHAOS_FORMAT);
			geBitmap_ClearMips(pTex->CAttachBmp);

			if(geBitmap_GetInfo(pTex->CAttachBmp, &AttachInfo, NULL) == GE_FALSE)
				continue;

			pTex->SegmentCount = AttachInfo.Width / pTex->SegmentSize;

			// fail if the sway amount is bigger than the texture size
			if((pTex->MaxXSway >= AttachInfo.Width) || (pTex->MaxYSway >= AttachInfo.Height))
				continue;

			pTex->OriginalBmp = geBitmap_Create(AttachInfo.Width, AttachInfo.Height, 1, CHAOS_FORMAT);

			if(pTex->OriginalBmp == NULL)
				continue;

			geBitmap_ClearMips( pTex->OriginalBmp );
			geBitmap_BlitBitmap( pTex->CAttachBmp, pTex->OriginalBmp );
			pTex->WorkBmp = geBitmap_Create( AttachInfo.Width, AttachInfo.Height, 1, CHAOS_FORMAT );

			if(pTex->WorkBmp != NULL)
				geBitmap_ClearMips(pTex->WorkBmp );
	}

	// all done
	return;
}

/* ------------------------------------------------------------------------------------ */
//	Destructor
/* ------------------------------------------------------------------------------------ */
Chaos::~Chaos()
{
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "EChaos");

	if(!pSet)
		return;	// Not on this level.

	geEntity *pEntity;

	// Ok, we have procedurals somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		EChaos *pTex = static_cast<EChaos*>(geEntity_GetUserData(pEntity));

		if((pTex->OriginalBmp != NULL) && (pTex->CAttachBmp != NULL))
			geBitmap_BlitBitmap(pTex->OriginalBmp, pTex->CAttachBmp );

		if(pTex->OriginalBmp != NULL)
			geBitmap_Destroy(&(pTex->OriginalBmp));

		if(pTex->WorkBmp != NULL)
			geBitmap_Destroy(&(pTex->WorkBmp));
	}
}

/* ------------------------------------------------------------------------------------ */
//	Tick
/* ------------------------------------------------------------------------------------ */
void Chaos::Tick(geFloat dwTicks)
{
	geEntity_EntitySet	*pSet = geWorld_GetEntitySet(CCD->World(), "EChaos");

	if(!pSet)
		return;	// Not on this level.

	geBitmap_Info AttachInfo, OriginalInfo;
	int	Row, Col;
	int	XPos, YPos;
	geFloat	CosStep;
	geFloat	CurYOffset;
	geFloat	CurXOffset;
	geEntity *pEntity;

	dwTicks *= 0.001f;

	// Ok, we have procedurals somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		EChaos *pTex = static_cast<EChaos*>(geEntity_GetUserData(pEntity));

		if(!pTex->CAttachBmp)
			continue;

// changed QD 08/30/02
		// do nothing further if the texture is not visible
		if(pTex->Actor && !CCD->ActorManager()->IsActor(pTex->Actor))
			continue;

		if(pTex->Actor == NULL)
		{
			if(geWorld_BitmapIsVisible(CCD->World(), pTex->CAttachBmp) == GE_FALSE)
				continue;
		}
		else
		{
			if(geWorld_IsActorPotentiallyVisible(CCD->World(), pTex->Actor, CCD->CameraManager()->Camera()) == GE_FALSE)
				continue;
		}
// end change QD 08/30/02


		if(geBitmap_GetInfo(pTex->CAttachBmp, &AttachInfo, NULL) == GE_FALSE)
			continue;

		if(geBitmap_GetInfo(pTex->OriginalBmp, &OriginalInfo, NULL) == GE_FALSE)
			continue;


		// compute vertical offset
		pTex->YOffset += pTex->YStep * dwTicks;

		if(pTex->YOffset > GE_2PI)
			pTex->YOffset = 0.0f;

		CosStep = GE_2PI / static_cast<geFloat>(pTex->SegmentCount);
		CurYOffset = pTex->YOffset;

		// adjust vertically
		for(Col=0; Col<pTex->SegmentCount; ++Col)
		{
			// adjust offset
			CurYOffset += CosStep;

			if(CurYOffset > GE_PI)
			{
				CosStep = -CosStep;
				// changed QD 12/15/05
				CurYOffset = GE_2PI - CurYOffset;
			}
			else if(CurYOffset < 0.0)
			{
				CosStep = -CosStep;
				CurYOffset = -CurYOffset;
			}

			// compute positions
			XPos = Col * pTex->SegmentSize;
			YPos = static_cast<int>((cos(CurYOffset) + 1.0f) * pTex->MaxYSway * 0.5f);

			// adjust bitmap
			geBitmap_Blit(pTex->OriginalBmp, XPos, 0, pTex->WorkBmp, XPos, YPos, pTex->SegmentSize, AttachInfo.Height - YPos );
			geBitmap_Blit(pTex->OriginalBmp, XPos, AttachInfo.Height - YPos, pTex->WorkBmp, XPos, 0, pTex->SegmentSize, YPos );
		}

		// compute horizontal offset
		pTex->XOffset += pTex->XStep * dwTicks;

		if(pTex->XOffset > GE_2PI)
			pTex->XOffset = 0.0f;

		CosStep = GE_2PI/static_cast<geFloat>(pTex->SegmentCount);
		CurXOffset = pTex->XOffset;

		// adjust horizontally
		for(Row=0; Row<pTex->SegmentCount; ++Row)
		{
			// adjust offset
			CurXOffset += CosStep;

			if(CurXOffset > GE_PI)
			{
				CosStep = -CosStep;
				// changed QD 12/15/05
				CurXOffset = GE_2PI - CurXOffset;
			}
			else if(CurXOffset < 0.0f)
			{
				CosStep = -CosStep;
				CurXOffset = -CurXOffset;
			}

			// compute positions
			XPos = static_cast<int>((cos(CurXOffset) + 1.0f) * pTex->MaxXSway * 0.5f);
			YPos = Row * pTex->SegmentSize;

			// adjust bitmap
			geBitmap_Blit(pTex->WorkBmp, 0, YPos, pTex->CAttachBmp, XPos, YPos, AttachInfo.Width - XPos, pTex->SegmentSize);
			geBitmap_Blit(pTex->WorkBmp, AttachInfo.Width - XPos, YPos, pTex->CAttachBmp, 0, YPos, XPos, pTex->SegmentSize);
		}

/* 04/23/2004 Wendell Buckner
	 EMBM CHAOS - force an update to the real bumpmap texture attached to this bitmap */
		geBitmap_UpdateBumpMapAlt(pTex->CAttachBmp);
	}

	// all done
	return;
}

/* ----------------------------------- END OF FILE ------------------------------------ */
