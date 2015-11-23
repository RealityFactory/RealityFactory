// CMorph.cpp: Implementierung der Klasse CMorph.
//  taken from the GDemo Source modified for RF
//  original Author: Peter Siamidis
//  Description:
//		Morph one texture into another.
//                                                                                      
//  The contents of this file are subject to the Genesis3D Public License               
//  Version 1.01 (the "License"); you may not use this file except in                   
//  compliance with the License. You may obtain a copy of the License at                
//  http://www.genesis3d.com                                                            
//                                                                                      
//  Software distributed under the License is distributed on an "AS IS"                 
//  basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See                
//  the License for the specific language governing rights and limitations              
//  under the License.                                                                  
//                                                                                      
//  Copyright (C) 1996-1999 Eclipse Entertainment, L.L.C. All Rights Reserved           
//                                                                                      
//////////////////////////////////////////////////////////////////////

//	Include the One True Header
#include "RabidFramework.h"

extern geBitmap *TPool_Bitmap(char *DefaultBmp, char *DefaultAlpha, char *BName, char *AName);

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CMorph::CMorph()
{
	geBitmap_Info	StartInfo, EndInfo, MorphInfo;
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	pSet = geWorld_GetEntitySet(CCD->World(), "EM_Morph");

	if(!pSet) 
	 return;	// Not on this level.

//	Ok, we have Morph textures somewhere.  Dig through 'em all.

	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
      pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		EM_Morph *pMorph = (EM_Morph*)geEntity_GetUserData(pEntity);
	
		if(EffectC_IsStringNull(pMorph->szEntityName))
		{
			char szName[128];
			geEntity_GetName(pEntity, szName, 128);
			pMorph->szEntityName = szName;
		}
		// Ok, put this entity into the Global Entity Registry
		CCD->EntityRegistry()->AddEntity(pMorph->szEntityName, "EM_Morph");
	
		//make sure that there are at least 2 morph stages
		if(pMorph->MorphStages < 2)
			pMorph->MorphStages = 2;

		pMorph->active = false;
		pMorph->CurStage = 0;
		pMorph->CurDelayToNextStage = pMorph->DelayToNextStage;
		pMorph->StageDir = 1;
		pMorph->First = true;

		if(EffectC_IsStringNull(pMorph->StartBmp))
			pMorph->CStartBmp = NULL;
		else
			pMorph->CStartBmp = TPool_Bitmap(pMorph->StartBmp, pMorph->StartBmp, NULL, NULL);
		if(pMorph->CStartBmp == NULL)
		{
			char szError[256];
			sprintf(szError,"Could not create StartBmp ");
			CCD->ReportError(szError, false);
			CCD->ShutdownLevel();
			delete CCD;
			MessageBox(NULL, szError,"EM_Morph", MB_OK);
			exit(-333);
			//or just continue;
		}
		if (geBitmap_GetInfo(pMorph->CStartBmp, &StartInfo, NULL) == GE_FALSE)
			continue;
		if(geBitmap_SetFormatMin(pMorph->CStartBmp, GE_PIXELFORMAT_24BIT_RGB ) == GE_FALSE)
			continue;
		if(EffectC_IsStringNull(pMorph->EndBmp))
			pMorph->CEndBmp = NULL;
		else
			pMorph->CEndBmp = TPool_Bitmap(pMorph->EndBmp, pMorph->EndBmp, NULL, NULL);
		if(pMorph->CEndBmp == NULL)
		{
			char szError[256];
			sprintf(szError,"Could not create EndBmp");
			CCD->ReportError(szError, false);
			CCD->ShutdownLevel();
			delete CCD;
			MessageBox(NULL, szError,"EM_Morph", MB_OK);
			exit(-333);
			//or just continue;
		}	
		if (geBitmap_GetInfo(pMorph->CEndBmp, &EndInfo, NULL) == GE_FALSE)
			continue;
		if(geBitmap_SetFormatMin(pMorph->CEndBmp, GE_PIXELFORMAT_24BIT_RGB ) == GE_FALSE)
			continue;
		if ((StartInfo.Width != EndInfo.Width) || (StartInfo.Height != EndInfo.Height))
			continue;

		pMorph->Actor = NULL;
		if(!EffectC_IsStringNull(pMorph->EntityName))
		{
			pMorph->Actor = GetEntityActor(pMorph->EntityName);
			if(pMorph->Actor == NULL)
			{
				char szError[256];
				sprintf(szError,"Error can't find actor");
				CCD->ReportError(szError, false);
				CCD->ShutdownLevel();
				delete CCD;
				MessageBox(NULL, szError,"EChaos", MB_OK);
				exit(-333);// or continue
			}
		}

		// get the output bitmap as an actor bitmap...
		if (pMorph->Actor != NULL)
		{

			// locals
			geActor_Def	*ActorDef;
			geBody		*Body;
			int			MaterialCount;
			int			i;
			const char		*MaterialName;
			float		R, G, B;
			int			Length;


			// get actor material count
			ActorDef = geActor_GetActorDef(pMorph->Actor);
			if (ActorDef == NULL)
				continue;
			Body = geActor_GetBody(ActorDef);
			if (Body == NULL)
				continue;
			MaterialCount = geActor_GetMaterialCount(pMorph->Actor);

			// get bitmap pointer
			Length = strlen(pMorph->BitmapToAttachTo);
			for(i = 0;i < MaterialCount;i++)
			{
				
				if(geBody_GetMaterial(Body, i, &MaterialName, &(pMorph->CMorphBmp), &R, &G, &B) == GE_FALSE)
				{
					continue;
				}
				if(strnicmp(pMorph->BitmapToAttachTo, MaterialName, Length) == 0)
				{
					break;
				}
			}
			if(i == MaterialCount)
			{
				char szError[256];
				sprintf(szError,"Error can't find ActorMaterial");
				CCD->ReportError(szError, false);
				CCD->ShutdownLevel();
				delete CCD;
				MessageBox(NULL, szError,"EM_Morph", MB_OK);
				exit(-333);
			}
		}

		// ...or a world bitmap
		else
		{
			pMorph->CMorphBmp = geWorld_GetBitmapByName(CCD->World(), pMorph->BitmapToAttachTo);
			if(pMorph->CMorphBmp == NULL)
			{
				char szError[256];
				sprintf(szError,"Error can't find bitmap in level");
				CCD->ReportError(szError, false);
				CCD->ShutdownLevel();
				delete CCD;
				MessageBox(NULL, szError,"EM_Morph", MB_OK);
				exit(-333);
			}	
		}

		// prepare the output bitmap
		if(geBitmap_GetInfo(pMorph->CMorphBmp, &MorphInfo, NULL) == GE_FALSE)
			continue;
		if((StartInfo.Width != MorphInfo.Width) || (StartInfo.Height != MorphInfo.Height))
			continue;
		if(geBitmap_SetFormatMin(pMorph->CMorphBmp, GE_PIXELFORMAT_24BIT_RGB) != GE_TRUE)
			continue;
		
		geBitmap_ClearMips(pMorph->CMorphBmp);
	}

	// all done
	return;
}

// not sure if we have to remove something special here
// please take a look at this
CMorph::~CMorph()
{
	return;
}

void CMorph::Tick(float dwTicks)
{
	
	// locals
	geBitmap		*StartLocked = NULL;
	geBitmap		*EndLocked = NULL;
	geBitmap		*MorphLocked = NULL;
	geBitmap_Info	StartInfo, EndInfo, MorphInfo;
	uint8			*Start, *End, *Morph;
	uint8			*CurStart, *CurEnd, *CurMorph;
	int				Row, Col;
	geBoolean		Result;
	float			EndFrac;


	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	pSet = geWorld_GetEntitySet(CCD->World(), "EM_Morph");

	if(!pSet) 
	 return;	// Not on this level.

//	Ok, we have Morph textures somewhere.  Dig through 'em all.

	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
      pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		EM_Morph *pMorph = (EM_Morph*)geEntity_GetUserData(pEntity);
	

		if(!EffectC_IsStringNull(pMorph->TriggerName))
		{
			if(GetTriggerState(pMorph->TriggerName))
				pMorph->active = true;
			else
				pMorph->active = false;
		}
		else
			pMorph->active = true;

		if(pMorph->active == GE_TRUE)
		{		
			if(pMorph->Actor && !CCD->ActorManager()->IsActor(pMorph->Actor))
				continue;

			if(!pMorph->First)
			{
				// do nothing if next stage is not ready
				pMorph->CurDelayToNextStage -= dwTicks*0.001f;
				if(pMorph->CurDelayToNextStage > 0.0f)
					continue;
				
				// adjust stage number
				pMorph->CurDelayToNextStage = pMorph->DelayToNextStage;
				pMorph->CurStage += pMorph->StageDir;
				if((pMorph->CurStage >= pMorph->MorphStages) || (pMorph->CurStage < 0))
				{
					if(pMorph->Loop == GE_FALSE )
						continue;
					pMorph->StageDir = -(pMorph->StageDir);
					pMorph->CurStage += pMorph->StageDir;
				}

				// do nothing further if the texture is not visible
				if(pMorph->Actor == NULL)
				{
					if(geWorld_BitmapIsVisible(CCD->World(), pMorph->CMorphBmp) == GE_FALSE)
						continue;
				}
				else
				{	
					if(geWorld_IsActorPotentiallyVisible(CCD->World(), pMorph->Actor, CCD->CameraManager()->Camera()) == GE_FALSE)
						continue;
				}
			}
			pMorph->First = false;

			// setup frac
			EndFrac = (float)pMorph->CurStage / ((float)pMorph->MorphStages - 1.0f);

			// lock start bitmap for reading
			if(geBitmap_LockForRead(pMorph->CStartBmp, &StartLocked, 0, 0, GE_PIXELFORMAT_24BIT_RGB, GE_FALSE, 255) == GE_FALSE)
			{
				goto ALLDONE;
			}
			if(geBitmap_GetInfo(StartLocked, &StartInfo, NULL) == GE_FALSE)
			{
				goto ALLDONE;
			}
			Start = (unsigned char*)geBitmap_GetBits( StartLocked );
			if(Start == NULL)
			{
				goto ALLDONE;
			}

			// lock end bitmap for reading
			if(geBitmap_LockForRead(pMorph->CEndBmp, &EndLocked, 0, 0, GE_PIXELFORMAT_24BIT_RGB, GE_FALSE, 255) == GE_FALSE)
			{
				goto ALLDONE;
			}
			if(geBitmap_GetInfo(EndLocked, &EndInfo, NULL) == GE_FALSE)
			{
				goto ALLDONE;
			}
			End = (unsigned char*)geBitmap_GetBits(EndLocked);
			if (End == NULL)
			{
				goto ALLDONE;
			}

			// lock the destination bitmap for writing
			if(geBitmap_LockForWriteFormat(pMorph->CMorphBmp, &MorphLocked, 0, 0, GE_PIXELFORMAT_24BIT_RGB) == GE_FALSE)
			{
				goto ALLDONE;
			}
			if(geBitmap_GetInfo(MorphLocked, &MorphInfo, NULL) == GE_FALSE)
			{
				goto ALLDONE;
			}
			Morph = (unsigned char*)geBitmap_GetBits(MorphLocked);
			if (Morph == NULL)
			{
				goto ALLDONE;
			} 


			// setup morph bitmap
			assert( StartInfo.Height >= MorphInfo.Height );
			assert( StartInfo.Width  >= MorphInfo.Width  );
			assert( EndInfo.Height   >= MorphInfo.Height );
			assert( EndInfo.Width    >= MorphInfo.Width  );
			for(Row = 0;Row < MorphInfo.Height;Row++)
			{

				// setup pointers
				CurMorph = Morph + ( MorphInfo.Stride * Row * 3 );
				CurStart = Start + ( StartInfo.Stride * Row * 3 );
				CurEnd = End + ( EndInfo.Stride * Row * 3 );

				// copy data
				for ( Col = 0; Col < MorphInfo.Width; Col++ )
				{

					// adjust pixel
					CurMorph[0] = CurStart[0] + (char)( (float)( CurEnd[0] - CurStart[0] ) * EndFrac );
					CurMorph[1] = CurStart[1] + (char)( (float)( CurEnd[1] - CurStart[1] ) * EndFrac );
					CurMorph[2] = CurStart[2] + (char)( (float)( CurEnd[2] - CurStart[2] ) * EndFrac );

					// adjust pointers
					CurMorph += 3;
					CurStart += 3;
					CurEnd += 3;
				}
			}

			// unlock all bitmaps
			ALLDONE:
			if ( StartLocked != NULL )
			{
				Result = geBitmap_UnLock( StartLocked );
				assert( Result == GE_TRUE );
			}
			if ( EndLocked != NULL )
			{
				Result = geBitmap_UnLock( EndLocked );
				assert( Result == GE_TRUE );
			}
			if ( MorphLocked != NULL )
			{
				Result = geBitmap_UnLock( MorphLocked );
				assert( Result == GE_TRUE );
			}
		}

	}
	// all done
	return;
}