/************************************************************************************//**
 * @file CAttribute.cpp
 * @brief Attribute class implementation
 *
 * This file contains the class implementation for Attribute handling.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#include "RabidFramework.h"
#include "CLevel.h"
#include "CInventory.h"
#include "CAttribute.h"

extern geSound_Def *SPool_Sound(const char *SName);
extern "C" void	DrawBoundBox(geWorld *World, const geVec3d *Pos, const geVec3d *Min, const geVec3d *Max);
extern geBitmap *TPool_Bitmap(const char *DefaultBmp, const char *DefaultAlpha,
							  const char *BName, const char *AName);

/* ------------------------------------------------------------------------------------ */
// Constructor
/* ------------------------------------------------------------------------------------ */
CAttribute::CAttribute() :
	m_DynamicAttributes(1000),
	m_DynamicAttribute1(NULL)
{
	// Ok, check to see if there are Attributes in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "Attribute");
	geEntity *pEntity;

	if(pSet)
	{
		// Ok, we have Attributes somewhere.  Dig through 'em all.
		for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
			pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
		{
			Attribute *pSource = static_cast<Attribute*>(geEntity_GetUserData(pEntity));

			if(EffectC_IsStringNull(pSource->szEntityName))
			{
				char szName[128];
				geEntity_GetName(pEntity, szName, 128);
				pSource->szEntityName = szName;
			}

			// convert Degrees to Radians
			pSource->ActorRotation.X = GE_PIOVER180 * pSource->ActorRotation.X;
			pSource->ActorRotation.Y = GE_PIOVER180 * pSource->ActorRotation.Y;
			pSource->ActorRotation.Z = GE_PIOVER180 * pSource->ActorRotation.Z;

			pSource->Actor = CCD->ActorManager()->SpawnActor(pSource->szActorName,
								pSource->origin, pSource->ActorRotation, "", "", NULL);

			if(!pSource->Actor)
			{
				CCD->Log()->Warning("File %s - Line %d: %s : Missing Actor '%s'",
									__FILE__, __LINE__,
									pSource->szEntityName, pSource->szActorName);

				pSource->alive = GE_FALSE;
				pSource->ReSpawn = GE_FALSE;
				continue;
			}

			CCD->ActorManager()->RemoveActor(pSource->Actor);
			geActor_Destroy(&pSource->Actor);
			pSource->Actor = NULL;
			InitAttribute(pSource);
		}
	}

	// ModifyAttribute
	pSet = geWorld_GetEntitySet(CCD->World(), "ModifyAttribute");

	if(pSet)
	{
		// Ok, we have Attributes somewhere.  Dig through 'em all.
		for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
			pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
		{
			ModifyAttribute *pSource = static_cast<ModifyAttribute*>(geEntity_GetUserData(pEntity));
			pSource->active = GE_FALSE;
		}
	}
}


bool CAttribute::InitAttribute(Attribute *pAttribute)
{
	// Ok, put this entity into the Global Entity Registry
	CCD->EntityRegistry()->AddEntity(pAttribute->szEntityName, "Attribute");
	pAttribute->OriginOffset = pAttribute->origin;

	if(pAttribute->Model)
	{
		geVec3d ModelOrigin;
		geWorld_GetModelRotationalCenter(CCD->World(), pAttribute->Model, &ModelOrigin);
		geVec3d_Subtract(&pAttribute->origin, &ModelOrigin, &pAttribute->OriginOffset);
	}

	pAttribute->active = GE_FALSE;
	pAttribute->bState = GE_FALSE;
	pAttribute->CallBack = GE_FALSE;
	pAttribute->alive = GE_TRUE;

	pAttribute->Actor = NULL;

	if(!EffectC_IsStringNull(pAttribute->szSoundFile))
	{
		SPool_Sound(pAttribute->szSoundFile);
	}

	if(!EffectC_IsStringNull(pAttribute->szReSpawnSound))
	{
		SPool_Sound(pAttribute->szReSpawnSound);
	}

	if(!EffectC_IsStringNull(pAttribute->ShadowBitmap))
	{
		pAttribute->Bitmap = TPool_Bitmap(pAttribute->ShadowBitmap, pAttribute->ShadowAlphamap, NULL, NULL);
	}

	return true;
}


void CAttribute::AddAttributeEntity(Attribute *pAttribute)
{
	if(EffectC_IsStringNull(pAttribute->szEntityName))
	{
		char DefaultName[128];
		sprintf(DefaultName, "Attribute%04d", m_DynamicAttributes);
		pAttribute->szEntityName = DefaultName;
		++m_DynamicAttributes;
	}

	pAttribute->ActorRotation.X = GE_PIOVER180 * pAttribute->ActorRotation.X;
	pAttribute->ActorRotation.Y = GE_PIOVER180 * pAttribute->ActorRotation.Y;
	pAttribute->ActorRotation.Z = GE_PIOVER180 * pAttribute->ActorRotation.Z;

	if(InitAttribute(pAttribute))
	{
		if(m_DynamicAttribute1 == NULL)
		{
			m_DynamicAttribute1 = geWorld_AddEntity(CCD->World(), "Attribute",
													pAttribute->szEntityName,
													static_cast<void*>(pAttribute));
		}
		else
		{
			geWorld_AddEntity(	CCD->World(), "Attribute",
								pAttribute->szEntityName,
								static_cast<void*>(pAttribute));
		}
	}
}


/* ------------------------------------------------------------------------------------ */
// Destructor
/* ------------------------------------------------------------------------------------ */
CAttribute::~CAttribute()
{
	// Ok, check to see if there are Attributes in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "Attribute");

	if(pSet)
	{
		geEntity *pEntity;

		// Ok, we have Attributes somewhere.  Dig through 'em all.
		for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
			pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity))
		{
			Attribute *pSource = static_cast<Attribute*>(geEntity_GetUserData(pEntity));

			if(pSource->Actor)
			{
				if(pSource->active == GE_TRUE)
					CCD->ActorManager()->RemoveActorCheck(pSource->Actor);

				geActor_Destroy(&pSource->Actor);
				pSource->Actor = NULL;
			}
		}
	}
}


/* ------------------------------------------------------------------------------------ */
// Tick
/* ------------------------------------------------------------------------------------ */
void CAttribute::Tick(geFloat dwTicks)
{
	// Ok, check to see if there are Attributes in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "Attribute");
	geEntity *pEntity;

	if(pSet)
	{
		// Ok, we have Attributes somewhere.  Dig through 'em all.
		for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
			pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
		{
			Attribute *pSource = static_cast<Attribute*>(geEntity_GetUserData(pEntity));

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
							Sound.Min = CCD->Level()->GetAudibleRadius();
							Sound.Loop = GE_FALSE;
							Sound.SoundDef = SPool_Sound(pSource->szReSpawnSound);

							if(Sound.SoundDef != NULL)
								CCD->EffectManager()->Item_Add(EFF_SND, static_cast<void*>(&Sound));
						}
					}
				}

				continue;
			}

			pSource->origin = pSource->OriginOffset;

			if(SetOriginOffset(pSource->EntityName, pSource->BoneName, pSource->Model, &(pSource->origin)))
				CCD->ActorManager()->Position(pSource->Actor, pSource->origin);

			if(!EffectC_IsStringNull(pSource->TriggerName))
			{
				if(GetTriggerState(pSource->TriggerName))
				{
					if(pSource->active == GE_FALSE)
					{
						pSource->Actor = CCD->ActorManager()->SpawnActor(	pSource->szActorName,
																			pSource->origin,
																			pSource->ActorRotation,
																			"", "", NULL);

						CCD->ActorManager()->SetType(pSource->Actor, ENTITY_ATTRIBUTE_MOD);

						CCD->ActorManager()->SetEntityName(pSource->Actor, pSource->szEntityName);

						CCD->ActorManager()->SetScale(pSource->Actor, pSource->Scale);
						CCD->ActorManager()->Position(pSource->Actor, pSource->origin);

						if(pSource->Gravity)
							CCD->ActorManager()->SetGravity(pSource->Actor, CCD->Level()->GetGravity());

						CCD->ActorManager()->SetActorDynamicLighting(	pSource->Actor,
																		pSource->FillColor,
																		pSource->AmbientColor,
																		pSource->AmbientLightFromFloor);

						CCD->ActorManager()->SetShadow(pSource->Actor, pSource->ShadowSize);

						if(pSource->ShadowAlpha > 0.0f)
							CCD->ActorManager()->SetShadowAlpha(pSource->Actor, pSource->ShadowAlpha);

						if(!EffectC_IsStringNull(pSource->ShadowBitmap))
							CCD->ActorManager()->SetShadowBitmap(pSource->Actor, pSource->Bitmap);

						CCD->ActorManager()->SetProjectedShadows(pSource->Actor, pSource->UseProjectedShadows);
						CCD->ActorManager()->SetStencilShadows(pSource->Actor, pSource->UseStencilShadows);

						CCD->ActorManager()->SetHideRadar(pSource->Actor, pSource->HideFromRadar);

						if(!EffectC_IsStringNull(pSource->ChangeMaterial))
							CCD->ActorManager()->ChangeMaterial(pSource->Actor, pSource->ChangeMaterial);

						if(pSource->EnvironmentMapping)
							SetEnvironmentMapping(	pSource->Actor,
													true,
													pSource->AllMaterial,
													pSource->PercentMapping,
													pSource->PercentMaterial);

						pSource->active = GE_TRUE;
						pSource->bState = GE_TRUE;
					}
				}
				else
				{
					if(pSource->active == GE_TRUE)
					{
						CCD->ActorManager()->RemoveActorCheck(pSource->Actor);
						geActor_Destroy(&pSource->Actor);
						pSource->Actor = NULL;
						pSource->active = GE_FALSE;
						pSource->bState = GE_FALSE;
					}
				}
			}
			else
			{
				if(pSource->active == GE_FALSE)
				{
					pSource->Actor = CCD->ActorManager()->SpawnActor(	pSource->szActorName,
																		pSource->origin,
																		pSource->ActorRotation,
																		"", "", NULL);

					CCD->ActorManager()->SetType(pSource->Actor, ENTITY_ATTRIBUTE_MOD);

					CCD->ActorManager()->SetEntityName(pSource->Actor, pSource->szEntityName);

					CCD->ActorManager()->SetScale(pSource->Actor, pSource->Scale);
					CCD->ActorManager()->Position(pSource->Actor, pSource->origin);

					if(pSource->Gravity)
						CCD->ActorManager()->SetGravity(pSource->Actor, CCD->Level()->GetGravity());

					CCD->ActorManager()->SetActorDynamicLighting(	pSource->Actor,
																	pSource->FillColor,
																	pSource->AmbientColor,
																	pSource->AmbientLightFromFloor);

					CCD->ActorManager()->SetShadow(pSource->Actor, pSource->ShadowSize);

					if(pSource->ShadowAlpha > 0.0f)
						CCD->ActorManager()->SetShadowAlpha(pSource->Actor, pSource->ShadowAlpha);

					if(!EffectC_IsStringNull(pSource->ShadowBitmap))
						CCD->ActorManager()->SetShadowBitmap(pSource->Actor, pSource->Bitmap);

					CCD->ActorManager()->SetProjectedShadows(pSource->Actor, pSource->UseProjectedShadows);
					CCD->ActorManager()->SetStencilShadows(pSource->Actor, pSource->UseStencilShadows);

					CCD->ActorManager()->SetHideRadar(pSource->Actor, pSource->HideFromRadar);

					if(!EffectC_IsStringNull(pSource->ChangeMaterial))
						CCD->ActorManager()->ChangeMaterial(pSource->Actor, pSource->ChangeMaterial);

					if(pSource->EnvironmentMapping)
						SetEnvironmentMapping(	pSource->Actor,
												true,
												pSource->AllMaterial,
												pSource->PercentMapping,
												pSource->PercentMaterial);

					pSource->active = GE_TRUE;
					pSource->bState = GE_TRUE;
				}
			}
		}
	}

	// ModifyAttribute
	pSet = geWorld_GetEntitySet(CCD->World(), "ModifyAttribute");

	if(pSet)
	{
		// Ok, we have Attributes somewhere.  Dig through 'em all.
		for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
			pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
		{
			ModifyAttribute *pSource = static_cast<ModifyAttribute*>(geEntity_GetUserData(pEntity));

			if(!EffectC_IsStringNull(pSource->TriggerName))
			{
				if(GetTriggerState(pSource->TriggerName))
				{
					if(pSource->active == GE_FALSE)
					{
						CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());

						if(theInv->Has(pSource->Attribute))
						{
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

							sxInventory::GetSingletonPtr()->UpdateItem(pSource->Attribute, true);

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
}


/* ------------------------------------------------------------------------------------ */
// HandleCollision
/* ------------------------------------------------------------------------------------ */
bool CAttribute::HandleCollision(geActor *theTarget, geActor *pActor, bool UseKey)
{
	// Ok, check to see if there are Attributes in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "Attribute");

	if(!pSet)
		return false;

	geEntity *pEntity;

	// Ok, we have Attributes somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Attribute *pSource = static_cast<Attribute*>(geEntity_GetUserData(pEntity));

		if(pSource->Actor != pActor)
			continue;		// Not this one, keep looking

		if(pSource->PlayerOnly && theTarget != CCD->Player()->GetActor())
			return false;

		if(pSource->active == GE_FALSE)
			continue;

		if(pSource->UseKey && !UseKey)
			continue;

		if(UseKey && !pSource->UseKey)
			return false;

		bool flag = false;

		CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(theTarget);

		if(!EffectC_IsStringNull(pSource->AttributeName))
		{
			if(theInv->Has(pSource->AttributeName))
			{
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
					if(theInv->Value(pSource->AttributeName) < theInv->High(pSource->AttributeName) ||
						pSource->AttributeAmount < 0)
					{
						if(pSource->SetMaximumAmount)
							theInv->Set(pSource->AttributeName, theInv->High(pSource->AttributeName));
						else
							theInv->Modify(pSource->AttributeName, pSource->AttributeAmount);

						flag = true;
					}
				}

				sxInventory::GetSingletonPtr()->UpdateItem(pSource->AttributeName, true);
			}
		}

		if(!flag)
		{
			if(!EffectC_IsStringNull(pSource->AttributeAltName))
			{
				if(theInv->Has(pSource->AttributeAltName))
				{
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
						if(theInv->Value(pSource->AttributeAltName) < theInv->High(pSource->AttributeAltName) ||
							pSource->AttributeAltAmount < 0)
						{
							if(pSource->SetMaximumAmount)
								theInv->Set(pSource->AttributeAltName, theInv->High(pSource->AttributeAltName));
							else
								theInv->Modify(pSource->AttributeAltName, pSource->AttributeAltAmount);

							flag = true;
						}
					}

					sxInventory::GetSingletonPtr()->UpdateItem(pSource->AttributeAltName, true);
				}
			}
		}

		if(flag)
		{
			CCD->ActorManager()->RemoveActorCheck(pSource->Actor);
			geActor_Destroy(&pSource->Actor);
			pSource->Actor = NULL;

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
				Sound.Min = CCD->Level()->GetAudibleRadius();
				Sound.Loop = GE_FALSE;
				Sound.SoundDef = SPool_Sound(pSource->szSoundFile);

				if(Sound.SoundDef != NULL)
					CCD->EffectManager()->Item_Add(EFF_SND, static_cast<void*>(&Sound));
			}

			return true;
		}
	}

	return false;
}

/* ------------------------------------------------------------------------------------ */
// SaveTo
//
// Save Attributes to a supplied file
/* ------------------------------------------------------------------------------------ */
int CAttribute::SaveTo(FILE *SaveFD, bool type)
{
	// Ok, check to see if there are Attributes in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "Attribute");

	if(!pSet)
		return RGF_SUCCESS;									// No fields

	geEntity *pEntity;

	// Ok, we have Attributes somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity && pEntity!=m_DynamicAttribute1;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Attribute *pSource = static_cast<Attribute*>(geEntity_GetUserData(pEntity));

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
// RestoreFrom
//
// Restore Attributes from a supplied file
/* ------------------------------------------------------------------------------------ */
int CAttribute::RestoreFrom(FILE *RestoreFD, bool type)
{
	// Ok, check to see if there are Attributes in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "Attribute");

	if(!pSet)
		return RGF_SUCCESS;

	geEntity *pEntity;

	// Ok, we have Attributes somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Attribute *pSource = static_cast<Attribute*>(geEntity_GetUserData(pEntity));

		READDATA(type, &(pSource->alive),			sizeof(geBoolean),	1, RestoreFD);
		READDATA(type, &(pSource->active),			sizeof(geBoolean),	1, RestoreFD);
		READDATA(type, &(pSource->Tick),			sizeof(geFloat),	1, RestoreFD);
		READDATA(type, &(pSource->CallBack),		sizeof(geBoolean),	1, RestoreFD);
		READDATA(type, &(pSource->CallBackCount),	sizeof(int),		1, RestoreFD);
		READDATA(type, &(pSource->bState),			sizeof(geBoolean),	1, RestoreFD);

		if(pSource->active == GE_TRUE)
		{
			pSource->Actor = CCD->ActorManager()->SpawnActor(	pSource->szActorName,
																pSource->origin,
																pSource->ActorRotation,
																"", "", NULL);

			CCD->ActorManager()->SetType(pSource->Actor, ENTITY_ATTRIBUTE_MOD);

			CCD->ActorManager()->SetEntityName(pSource->Actor, pSource->szEntityName);

			CCD->ActorManager()->SetScale(pSource->Actor, pSource->Scale);
			CCD->ActorManager()->Position(pSource->Actor, pSource->origin);

			if(pSource->Gravity)
				CCD->ActorManager()->SetGravity(pSource->Actor, CCD->Level()->GetGravity());

			CCD->ActorManager()->SetActorDynamicLighting(	pSource->Actor,
															pSource->FillColor,
															pSource->AmbientColor,
															pSource->AmbientLightFromFloor);

			CCD->ActorManager()->SetShadow(pSource->Actor, pSource->ShadowSize);

			if(pSource->ShadowAlpha > 0.0f)
				CCD->ActorManager()->SetShadowAlpha(pSource->Actor, pSource->ShadowAlpha);

			if(!EffectC_IsStringNull(pSource->ShadowBitmap))
				CCD->ActorManager()->SetShadowBitmap(pSource->Actor, pSource->Bitmap);

			CCD->ActorManager()->SetHideRadar(pSource->Actor, pSource->HideFromRadar);

			if(!EffectC_IsStringNull(pSource->ChangeMaterial))
				CCD->ActorManager()->ChangeMaterial(pSource->Actor, pSource->ChangeMaterial);
		}
	}

	return RGF_SUCCESS;
}


// ******************** CRGF Overrides ********************

/* ------------------------------------------------------------------------------------ */
// LocateEntity
//
// Given a name, locate the desired item in the currently loaded level
// ..and return it's user data.
/* ------------------------------------------------------------------------------------ */
int CAttribute::LocateEntity(const char *szName, void **pEntityData)
{
	// Ok, check to see if there are Attributes in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "Attribute");

	if(!pSet)
		return RGF_NOT_FOUND;							// No Attribute entities

	geEntity *pEntity;

	// Ok, we have Attribute entities.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Attribute *pTheEntity = static_cast<Attribute*>(geEntity_GetUserData(pEntity));

		if(!strcmp(pTheEntity->szEntityName, szName))
		{
			*pEntityData = static_cast<void*>(pTheEntity);
			return RGF_SUCCESS;
		}
	}

	return RGF_NOT_FOUND;								// Sorry, no such entity here
}


/* ------------------------------------------------------------------------------------ */
// ReSynchronize
//
// Correct internal timing to match current time, to make up for time lost
// ..when outside the game loop (typically in "menu mode").
/* ------------------------------------------------------------------------------------ */
int CAttribute::ReSynchronize()
{
	return RGF_SUCCESS;
}

/* ----------------------------------- END OF FILE ------------------------------------ */
