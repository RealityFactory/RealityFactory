/************************************************************************************//**
 * @file CAttribute.cpp
 * @brief Attribute class implementation
 *
 * This file contains the class implementation for Attribute handling.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

//	Include the One True Header
#include "RabidFramework.h"

extern geSound_Def *SPool_Sound(char *SName);
extern "C" void	DrawBoundBox(geWorld *World, const geVec3d *Pos, const geVec3d *Min, const geVec3d *Max);
// changed QD 06/26/04
extern geBitmap *TPool_Bitmap(char *DefaultBmp, char *DefaultAlpha, char *BName, char *AName);
// end change

/* ------------------------------------------------------------------------------------ */
//	Constructor
/* ------------------------------------------------------------------------------------ */
CAttribute::CAttribute()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	//	Ok, check to see if there are Attributes in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "Attribute");

	if(pSet)
	{
		//	Ok, we have Attributes somewhere.  Dig through 'em all.
		for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
			pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
		{
			Attribute *pSource = (Attribute*)geEntity_GetUserData(pEntity);

			if(EffectC_IsStringNull(pSource->szEntityName))
			{
				char szName[128];
				geEntity_GetName(pEntity, szName, 128);
				pSource->szEntityName = szName;
			}

			//	Ok, put this entity into the Global Entity Registry
			CCD->EntityRegistry()->AddEntity(pSource->szEntityName, "Attribute");
			pSource->OriginOffset = pSource->origin;

			if(pSource->Model)
			{
				geVec3d ModelOrigin;
	    		geWorld_GetModelRotationalCenter(CCD->World(), pSource->Model, &ModelOrigin);
				geVec3d_Subtract(&pSource->origin, &ModelOrigin, &pSource->OriginOffset);
  			}

			pSource->active = GE_FALSE;
			pSource->bState = GE_FALSE;
			pSource->CallBack = GE_FALSE;
			pSource->alive = GE_TRUE;
			pSource->ActorRotation.X = GE_PIOVER180*pSource->ActorRotation.X;
			pSource->ActorRotation.Y = GE_PIOVER180*pSource->ActorRotation.Y;
			pSource->ActorRotation.Z = GE_PIOVER180*pSource->ActorRotation.Z;
			pSource->Actor = CCD->ActorManager()->SpawnActor(pSource->szActorName,
								pSource->origin, pSource->ActorRotation, "", "", NULL);

			if(!pSource->Actor)
			{
				char szError[256];
				sprintf(szError,"*WARNING* File %s - Line %d: %s : Missing Actor '%s'\n",
						__FILE__, __LINE__, pSource->szEntityName, pSource->szActorName);
				CCD->ReportError(szError, false);
				// changed QD 07/15/06 - make missing actor not a fatal error
				pSource->alive = GE_FALSE;
				pSource->ReSpawn = GE_FALSE;
				continue;
				/*
				CCD->ShutdownLevel();
				delete CCD;
				CCD = NULL;
				MessageBox(NULL, szError,"Attribute", MB_OK);
				exit(-333);
				*/
				// end change
			}

			CCD->ActorManager()->RemoveActor(pSource->Actor);
			geActor_Destroy(&pSource->Actor);
// start multiplayer
			pSource->Actor = NULL;
// end multiplayer

			if(!EffectC_IsStringNull(pSource->szSoundFile))
			{
				SPool_Sound(pSource->szSoundFile);
			}

			if(!EffectC_IsStringNull(pSource->szReSpawnSound))
			{
				SPool_Sound(pSource->szReSpawnSound);
			}

// changed QD 06/26/04
			if(!EffectC_IsStringNull(pSource->ShadowBitmap))
			{
				pSource->Bitmap = TPool_Bitmap(pSource->ShadowBitmap, pSource->ShadowAlphamap, NULL, NULL);
			}
// end change
		}
	}

// changed RF063
	// ModifyAttribute
	pSet = geWorld_GetEntitySet(CCD->World(), "ModifyAttribute");

	if(pSet)
	{
		//	Ok, we have Attributes somewhere.  Dig through 'em all.
		for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
			pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity))
		{
			ModifyAttribute *pSource = (ModifyAttribute*)geEntity_GetUserData(pEntity);
			pSource->active = GE_FALSE;
		}
	}
// end change RF063
}

/* ------------------------------------------------------------------------------------ */
//	Destructor
/* ------------------------------------------------------------------------------------ */
CAttribute::~CAttribute()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	//	Ok, check to see if there are Attributes in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "Attribute");

	if(pSet)
	{
		//	Ok, we have Attributes somewhere.  Dig through 'em all.
		for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
			pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity))
		{
			Attribute *pSource = (Attribute*)geEntity_GetUserData(pEntity);

			if(pSource->Actor)
			{
				if(pSource->active == GE_TRUE)
					CCD->ActorManager()->RemoveActor(pSource->Actor);

				geActor_Destroy(&pSource->Actor);
			}
		}
	}
}

/* ------------------------------------------------------------------------------------ */
// Tick
/* ------------------------------------------------------------------------------------ */
void CAttribute::Tick(float dwTicks)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	//	Ok, check to see if there are Attributes in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "Attribute");

	if(pSet)
	{
		//	Ok, we have Attributes somewhere.  Dig through 'em all.
		for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
			pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
		{
			Attribute *pSource = (Attribute*)geEntity_GetUserData(pEntity);

			if(pSource->CallBack == GE_TRUE)
			{
				pSource->CallBackCount -= 1;

				if(pSource->CallBackCount == 0)
					pSource->CallBack = GE_FALSE;
			}

			if(!pSource->alive)
			{
				if(pSource->ReSpawn)
				{
					pSource->Tick += dwTicks;

					if(pSource->Tick >= (pSource->Delay*1000.0f))
					{
						pSource->alive = GE_TRUE;

						if(!EffectC_IsStringNull(pSource->szReSpawnSound))
						{
							Snd	Sound;

							memset(&Sound, 0, sizeof(Sound));
							geVec3d_Copy(&(pSource->origin), &(Sound.Pos));
							Sound.Min = CCD->GetAudibleRadius();
							Sound.Loop = GE_FALSE;
							Sound.SoundDef = SPool_Sound(pSource->szReSpawnSound);

							if(Sound.SoundDef != NULL)
								CCD->EffectManager()->Item_Add(EFF_SND, (void*)&Sound);
						}
					}
				}

				continue;
			}

			pSource->origin = pSource->OriginOffset;

// changed QD 08/14/03
// fixed: attribute following another entity
			if(SetOriginOffset(pSource->EntityName, pSource->BoneName, pSource->Model, &(pSource->origin)))
				CCD->ActorManager()->Position(pSource->Actor, pSource->origin);
// end change 08/14/03

			if(!EffectC_IsStringNull(pSource->TriggerName))
			{
				if(GetTriggerState(pSource->TriggerName))
				{
					if(pSource->active == GE_FALSE)
					{
						pSource->Actor = CCD->ActorManager()->SpawnActor(pSource->szActorName,
								pSource->origin, pSource->ActorRotation, "", "", NULL);//pSource->Actor);
						CCD->ActorManager()->SetType(pSource->Actor, ENTITY_ATTRIBUTE_MOD);
// changed Nout 12/15/05
						CCD->ActorManager()->SetEntityName(pSource->Actor, pSource->szEntityName);
// end change
						CCD->ActorManager()->SetScale(pSource->Actor, pSource->Scale);
						CCD->ActorManager()->Position(pSource->Actor, pSource->origin);

						if(pSource->Gravity)
							CCD->ActorManager()->SetGravity(pSource->Actor, CCD->Player()->GetGravity());
// changed QD 07/21/04
//						CCD->ActorManager()->SetActorDynamicLighting(pSource->Actor, pSource->FillColor, pSource->AmbientColor);
						CCD->ActorManager()->SetActorDynamicLighting(pSource->Actor, pSource->FillColor,
							pSource->AmbientColor, pSource->AmbientLightFromFloor);
// end change
						CCD->ActorManager()->SetShadow(pSource->Actor, pSource->ShadowSize);
// changed QD 06/26/04
						if(pSource->ShadowAlpha > 0.0f)
							CCD->ActorManager()->SetShadowAlpha(pSource->Actor, pSource->ShadowAlpha);

						if(!EffectC_IsStringNull(pSource->ShadowBitmap))
							CCD->ActorManager()->SetShadowBitmap(pSource->Actor, pSource->Bitmap);
// end change
// changed QD Shadows
						CCD->ActorManager()->SetProjectedShadows(pSource->Actor, pSource->UseProjectedShadows);
						CCD->ActorManager()->SetStencilShadows(pSource->Actor, pSource->UseStencilShadows);
// end change
// changed RF064
						CCD->ActorManager()->SetHideRadar(pSource->Actor, pSource->HideFromRadar);

						if(!EffectC_IsStringNull(pSource->ChangeMaterial))
							CCD->ActorManager()->ChangeMaterial(pSource->Actor, pSource->ChangeMaterial);

						if(pSource->EnvironmentMapping)
							SetEnvironmentMapping(pSource->Actor, true, pSource->AllMaterial, pSource->PercentMapping, pSource->PercentMaterial);
// end change RF064
						pSource->active = GE_TRUE;
						pSource->bState = GE_TRUE;
					}
				}
				else
				{
					if(pSource->active == GE_TRUE)
					{
						CCD->ActorManager()->RemoveActor(pSource->Actor);
						geActor_Destroy(&pSource->Actor);
						pSource->active = GE_FALSE;
						pSource->bState = GE_FALSE;
					}
				}
			}
			else
			{
				if(pSource->active == GE_FALSE)
				{
					pSource->Actor = CCD->ActorManager()->SpawnActor(pSource->szActorName,
							pSource->origin, pSource->ActorRotation, "", "", NULL);//pSource->Actor);
					CCD->ActorManager()->SetType(pSource->Actor, ENTITY_ATTRIBUTE_MOD);
// changed Nout 12/15/05
					CCD->ActorManager()->SetEntityName(pSource->Actor, pSource->szEntityName);
// end change
					CCD->ActorManager()->SetScale(pSource->Actor, pSource->Scale);
					CCD->ActorManager()->Position(pSource->Actor, pSource->origin);

					if(pSource->Gravity)
						CCD->ActorManager()->SetGravity(pSource->Actor, CCD->Player()->GetGravity());
// changed QD 07/21/04
					// CCD->ActorManager()->SetActorDynamicLighting(pSource->Actor, pSource->FillColor, pSource->AmbientColor);
					CCD->ActorManager()->SetActorDynamicLighting(pSource->Actor, pSource->FillColor,
							pSource->AmbientColor, pSource->AmbientLightFromFloor);
// end change
					CCD->ActorManager()->SetShadow(pSource->Actor, pSource->ShadowSize);
// changed QD 06/26/04
					if(pSource->ShadowAlpha > 0.0f)
						CCD->ActorManager()->SetShadowAlpha(pSource->Actor, pSource->ShadowAlpha);

					if(!EffectC_IsStringNull(pSource->ShadowBitmap))
						CCD->ActorManager()->SetShadowBitmap(pSource->Actor, pSource->Bitmap);
// end change
// changed QD Shadows
					CCD->ActorManager()->SetProjectedShadows(pSource->Actor, pSource->UseProjectedShadows);
					CCD->ActorManager()->SetStencilShadows(pSource->Actor, pSource->UseStencilShadows);
// end change
// changed RF064
					CCD->ActorManager()->SetHideRadar(pSource->Actor, pSource->HideFromRadar);

					if(!EffectC_IsStringNull(pSource->ChangeMaterial))
						CCD->ActorManager()->ChangeMaterial(pSource->Actor, pSource->ChangeMaterial);

					if(pSource->EnvironmentMapping)
						SetEnvironmentMapping(pSource->Actor, true, pSource->AllMaterial, pSource->PercentMapping, pSource->PercentMaterial);
// end change RF064
					pSource->active = GE_TRUE;
					pSource->bState = GE_TRUE;
				}
			}
		}
	}

// changed RF063
	// ModifyAttribute
	pSet = geWorld_GetEntitySet(CCD->World(), "ModifyAttribute");

	if(pSet)
	{
		//	Ok, we have Attributes somewhere.  Dig through 'em all.
		for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
			pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
		{
			ModifyAttribute *pSource = (ModifyAttribute*)geEntity_GetUserData(pEntity);

			if(!EffectC_IsStringNull(pSource->TriggerName))
			{
				if(GetTriggerState(pSource->TriggerName))
				{
					if(pSource->active == GE_FALSE)
					{
						CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());

						if(theInv->Has(pSource->Attribute))
						{
							// changed QD 12/15/05
							if(pSource->PowerUp)
							{
								int NewHighLimit = theInv->High(pSource->Attribute) + pSource->Amount;

								theInv->SetHighLimit(pSource->Attribute, NewHighLimit);

								if(pSource->SetMaximumAmount)
									theInv->Set(pSource->Attribute, NewHighLimit);
							}
							else
							{
								if(pSource->SetMaximumAmount)
									theInv->Set(pSource->Attribute, theInv->High(pSource->Attribute));
								else
									theInv->Modify(pSource->Attribute, pSource->Amount);
							}
							// end change
							pSource->active = GE_TRUE;

						}
					}
				}
				else
				{
					pSource->active = GE_FALSE;
				}
			}
		}
	}
// end change RF063
}

/* ------------------------------------------------------------------------------------ */
//	HandleCollision
/* ------------------------------------------------------------------------------------ */
// changed QD 08/13/03 added UseKey
bool CAttribute::HandleCollision(geActor *theTarget, geActor *pActor, bool UseKey)
// end change 08/13/03
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	//	Ok, check to see if there are Attributes in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "Attribute");

	if(!pSet)
		return false;

	//	Ok, we have Attributes somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Attribute *pSource = (Attribute*)geEntity_GetUserData(pEntity);

		if(pSource->Actor != pActor)
			continue;	  // Not this one, keep looking

// changed RF063
		if(pSource->PlayerOnly && theTarget!=CCD->Player()->GetActor())
			return false;
// end change RF063

		if(pSource->active == GE_FALSE)
			continue;

// changed RF063
// changed QD 08/13/03
		if(pSource->UseKey && !UseKey)
			continue;

		if(UseKey && !pSource->UseKey)
			return false;
// end change 08/13/03

		bool flag = false;

		CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(theTarget);

		if(!EffectC_IsStringNull(pSource->AttributeName))
		{
			if(theInv->Has(pSource->AttributeName))
			{
				// changed QD 12/15/05
				if(pSource->PowerUp)
				{
					int NewHighLimit = theInv->High(pSource->AttributeName) + pSource->AttributeAmount;

					theInv->SetHighLimit(pSource->AttributeName, NewHighLimit);

					if(pSource->SetMaximumAmount)
						theInv->Set(pSource->AttributeName, NewHighLimit);

					flag = true;
				}
				else
				{
					if(theInv->Value(pSource->AttributeName) < theInv->High(pSource->AttributeName)
						|| pSource->AttributeAmount < 0)
					{
						if(pSource->SetMaximumAmount)
							theInv->Set(pSource->AttributeName, theInv->High(pSource->AttributeName));
						else
							theInv->Modify(pSource->AttributeName, pSource->AttributeAmount);

						flag = true;
					}

				}
				// end change
			}
		}

		if(!flag)
		{
			if(!EffectC_IsStringNull(pSource->AttributeAltName))
			{
				if(theInv->Has(pSource->AttributeAltName))
				{
					// changed QD 12/15/05
					if(pSource->PowerUp)
					{
						int NewHighLimit = theInv->High(pSource->AttributeAltName) + pSource->AttributeAltAmount;

						theInv->SetHighLimit(pSource->AttributeAltName, NewHighLimit);

						if(pSource->SetMaximumAmount)
							theInv->Set(pSource->AttributeAltName, NewHighLimit);

						flag = true;
					}
					else
					{
						if(theInv->Value(pSource->AttributeAltName) < theInv->High(pSource->AttributeAltName)
							|| pSource->AttributeAltAmount < 0)
						{
							if(pSource->SetMaximumAmount)
								theInv->Set(pSource->AttributeAltName, theInv->High(pSource->AttributeAltName));
							else
								theInv->Modify(pSource->AttributeAltName, pSource->AttributeAltAmount);

							flag = true;
						}
					}
					// end change
				}
			}
		}

		if(flag)
		{
			CCD->ActorManager()->RemoveActor(pSource->Actor);
			geActor_Destroy(&pSource->Actor);

			pSource->active = GE_FALSE;
			pSource->bState = GE_FALSE;
			pSource->alive = GE_FALSE;
			pSource->Tick = 0.0f;
			pSource->CallBack = GE_TRUE;
			pSource->CallBackCount = 2;

			if(!EffectC_IsStringNull(pSource->szSoundFile))
			{
				Snd Sound;
				memset(&Sound, 0, sizeof(Sound));
				geVec3d_Copy(&(pSource->origin), &(Sound.Pos));
				Sound.Min = CCD->GetAudibleRadius();
				Sound.Loop = GE_FALSE;
				Sound.SoundDef = SPool_Sound(pSource->szSoundFile);

				if(Sound.SoundDef != NULL)
					CCD->EffectManager()->Item_Add(EFF_SND, (void*)&Sound);
			}

			return true;
		}
// end change RF063
	}

	return false;
}

// changed RF063
/* ------------------------------------------------------------------------------------ */
//	SaveTo
//
//	Save Attributes to a supplied file
/* ------------------------------------------------------------------------------------ */
int CAttribute::SaveTo(FILE *SaveFD, bool type)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	//	Ok, check to see if there are Attributes in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "Attribute");

	if(!pSet)
		return RGF_SUCCESS;									// No fields

	//	Ok, we have Attributes somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Attribute *pSource = (Attribute*)geEntity_GetUserData(pEntity);

		WRITEDATA(type, &(pSource->alive),			sizeof(geBoolean),	1, SaveFD);
		WRITEDATA(type, &(pSource->active),			sizeof(geBoolean),	1, SaveFD);
		WRITEDATA(type, &(pSource->Tick),			sizeof(geFloat),	1, SaveFD);
		WRITEDATA(type, &(pSource->CallBack),		sizeof(geBoolean),	1, SaveFD);
		WRITEDATA(type, &(pSource->CallBackCount),	sizeof(int),		1, SaveFD);
		WRITEDATA(type, &(pSource->bState),			sizeof(geBoolean),	1, SaveFD);
	}

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	RestoreFrom
//
//	Restore Attributes from a supplied file
/* ------------------------------------------------------------------------------------ */
int CAttribute::RestoreFrom(FILE *RestoreFD, bool type)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	//	Ok, check to see if there are Attributes in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "Attribute");

	if(!pSet)
		return RGF_SUCCESS;

	//	Ok, we have Attributes somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Attribute *pSource = (Attribute*)geEntity_GetUserData(pEntity);

		READDATA(type, &(pSource->alive),		sizeof(geBoolean),	1, RestoreFD);
		READDATA(type, &(pSource->active),		sizeof(geBoolean),	1, RestoreFD);
		READDATA(type, &(pSource->Tick),		sizeof(geFloat),	1, RestoreFD);
		READDATA(type, &(pSource->CallBack),	sizeof(geBoolean),	1, RestoreFD);
		READDATA(type, &(pSource->CallBackCount), sizeof(int),		1, RestoreFD);
		READDATA(type, &(pSource->bState),		sizeof(geBoolean),	1, RestoreFD);

// changed RF064
		if(pSource->active == GE_TRUE)
		{
			pSource->Actor = CCD->ActorManager()->SpawnActor(pSource->szActorName,
				pSource->origin, pSource->ActorRotation, "", "", NULL);//pSource->Actor);
			CCD->ActorManager()->SetType(pSource->Actor, ENTITY_ATTRIBUTE_MOD);
// changed Nout 12/15/05
			CCD->ActorManager()->SetEntityName(pSource->Actor, pSource->szEntityName);
// end change
			CCD->ActorManager()->SetScale(pSource->Actor, pSource->Scale);
			CCD->ActorManager()->Position(pSource->Actor, pSource->origin);

			if(pSource->Gravity)
				CCD->ActorManager()->SetGravity(pSource->Actor, CCD->Player()->GetGravity());
// changed QD 07/21/04
			// CCD->ActorManager()->SetActorDynamicLighting(pSource->Actor, pSource->FillColor, pSource->AmbientColor);
			CCD->ActorManager()->SetActorDynamicLighting(pSource->Actor, pSource->FillColor,
							pSource->AmbientColor, pSource->AmbientLightFromFloor);
// end change

			CCD->ActorManager()->SetShadow(pSource->Actor, pSource->ShadowSize);

// changed QD 06/26/04
			if(pSource->ShadowAlpha > 0.0f)
				CCD->ActorManager()->SetShadowAlpha(pSource->Actor, pSource->ShadowAlpha);

			if(!EffectC_IsStringNull(pSource->ShadowBitmap))
				CCD->ActorManager()->SetShadowBitmap(pSource->Actor, pSource->Bitmap);
// end change

			CCD->ActorManager()->SetHideRadar(pSource->Actor, pSource->HideFromRadar);

			if(!EffectC_IsStringNull(pSource->ChangeMaterial))
				CCD->ActorManager()->ChangeMaterial(pSource->Actor, pSource->ChangeMaterial);
		}
// end change RF064
	}

	return RGF_SUCCESS;
}
// end change RF063

//	******************** CRGF Overrides ********************

/* ------------------------------------------------------------------------------------ */
//	LocateEntity
//
//	Given a name, locate the desired item in the currently loaded level
//	..and return it's user data.
/* ------------------------------------------------------------------------------------ */
int CAttribute::LocateEntity(char *szName, void **pEntityData)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	//	Ok, check to see if there are Attributes in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "Attribute");

	if(!pSet)
		return RGF_NOT_FOUND;							// No static entity proxies

	//	Ok, we have static entity proxies.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Attribute *pTheEntity = (Attribute*)geEntity_GetUserData(pEntity);

		if(!strcmp(pTheEntity->szEntityName, szName))
		{
			*pEntityData = (void*)pTheEntity;
			return RGF_SUCCESS;
		}
	}

	return RGF_NOT_FOUND;								// Sorry, no such entity here
}

/* ------------------------------------------------------------------------------------ */
//	ReSynchronize
//
//	Correct internal timing to match current time, to make up for time lost
//	..when outside the game loop (typically in "menu mode").
/* ------------------------------------------------------------------------------------ */
int CAttribute::ReSynchronize()
{
	return RGF_SUCCESS;
}

/* ----------------------------------- END OF FILE ------------------------------------ */
