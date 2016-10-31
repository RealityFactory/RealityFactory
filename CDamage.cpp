/************************************************************************************//**
 * @file CDamage.cpp
 * @brief Damage class implementation
 *
 * This file contains the class implementation for Damage handling.
 * @author Ralph Deane
 *//*
 ****************************************************************************************/

//	Include the One True Header
#include "RabidFramework.h"
#include "CLevel.h"
#include "CInventory.h"
#include "CArmour.h"
#include "CDamage.h"

extern geSound_Def *SPool_Sound(const char *SName);

/* ------------------------------------------------------------------------------------ */
// Constructor
//
// Load up all logic gates and set the	entities to default values.
/* ------------------------------------------------------------------------------------ */
CDamage::CDamage()
{
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "DestroyableModel");

	if(!pSet)
		return;

	geEntity *pEntity;

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		DestroyableModel *pDestroy = static_cast<DestroyableModel*>(geEntity_GetUserData(pEntity));

		if(EffectC_IsStringNull(pDestroy->szEntityName))
		{
			char szName[128];
			geEntity_GetName(pEntity, szName, 128);
			pDestroy->szEntityName = szName;
		}

		// Ok, put this entity into the Global Entity Registry
		CCD->EntityRegistry()->AddEntity(pDestroy->szEntityName, "DestroyableModel");

		if(!pDestroy->Model)
		{
			CCD->Log()->Warning("File %s - Line %d: DestroyableModel: '%s' Missing Model",
								__FILE__, __LINE__, pDestroy->szEntityName);
			continue;
		}

		pDestroy->bState = GE_TRUE;
		pDestroy->CallBack = GE_FALSE;
		pDestroy->active = GE_FALSE;
		pDestroy->Animating = GE_FALSE;
		CCD->ModelManager()->AddModel(pDestroy->Model, ENTITY_GENERIC);

		if(pDestroy->Model1)
			CCD->ModelManager()->AddModel(pDestroy->Model1, ENTITY_GENERIC);

		if(pDestroy->Model2)
			CCD->ModelManager()->AddModel(pDestroy->Model2, ENTITY_GENERIC);

		if(pDestroy->Model3)
			CCD->ModelManager()->AddModel(pDestroy->Model3, ENTITY_GENERIC);

		if(pDestroy->Model4)
			CCD->ModelManager()->AddModel(pDestroy->Model4, ENTITY_GENERIC);

		pDestroy->OriginalAmt = pDestroy->AttributeAmt;

		if(!EffectC_IsStringNull(pDestroy->szSoundFile))
			SPool_Sound(pDestroy->szSoundFile);
	}
}


/* ------------------------------------------------------------------------------------ */
// Destructor
//
// Clean up.
/* ------------------------------------------------------------------------------------ */
CDamage::~CDamage()
{
}


/* ------------------------------------------------------------------------------------ */
// Tick
/* ------------------------------------------------------------------------------------ */
void CDamage::Tick(float /*timeElapsed*/)
{
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "DestroyableModel");

	if(!pSet)
		return;

	geEntity *pEntity;

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		DestroyableModel *pDestroy = static_cast<DestroyableModel*>(geEntity_GetUserData(pEntity));

		if(!pDestroy->Model)
			continue;

		if(pDestroy->CallBack == GE_TRUE)
		{
			pDestroy->CallBackCount -= 1;

			if(pDestroy->CallBackCount == 0)
				pDestroy->CallBack = GE_FALSE;
		}

		if(pDestroy->Animating)
		{
			bool running = false;

			if(!CCD->ModelManager()->IsRunning(pDestroy->Model))
			{
				geXForm3d xForm;
				geWorld_GetModelXForm(CCD->World(), pDestroy->Model, &xForm);
				xForm.Translation.X = 10000.0f;
				xForm.Translation.Y = 10000.0f;
				xForm.Translation.Z = 10000.0f;
				geWorld_SetModelXForm(CCD->World(), pDestroy->Model, &xForm);

				if(!pDestroy->NoRemove)
					CCD->ModelManager()->SetPosition(pDestroy->Model, xForm.Translation);
			}
			else
			{
				running = true;
			}

			if(pDestroy->Model1)
			{
				if(!CCD->ModelManager()->IsRunning(pDestroy->Model1))
				{
					geXForm3d xForm;
					geWorld_GetModelXForm(CCD->World(), pDestroy->Model1, &xForm);
					xForm.Translation.X = 10000.0f;
					xForm.Translation.Y = 10000.0f;
					xForm.Translation.Z = 10000.0f;
					geWorld_SetModelXForm(CCD->World(), pDestroy->Model1, &xForm);

					if(!pDestroy->NoRemove)
						CCD->ModelManager()->SetPosition(pDestroy->Model1, xForm.Translation);
				}
				else
				{
					running = true;
				}
			}

			if(pDestroy->Model2)
			{
				if(!CCD->ModelManager()->IsRunning(pDestroy->Model2))
				{
					geXForm3d xForm;
					geWorld_GetModelXForm(CCD->World(), pDestroy->Model2, &xForm);
					xForm.Translation.X = 10000.0f;
					xForm.Translation.Y = 10000.0f;
					xForm.Translation.Z = 10000.0f;
					geWorld_SetModelXForm(CCD->World(), pDestroy->Model2, &xForm);

					if(!pDestroy->NoRemove)
						CCD->ModelManager()->SetPosition(pDestroy->Model2, xForm.Translation);
				}
				else
				{
					running = true;
				}
			}

			if(pDestroy->Model3)
			{
				if(!CCD->ModelManager()->IsRunning(pDestroy->Model3))
				{
					geXForm3d xForm;
					geWorld_GetModelXForm(CCD->World(), pDestroy->Model3, &xForm);
					xForm.Translation.X = 10000.0f;
					xForm.Translation.Y = 10000.0f;
					xForm.Translation.Z = 10000.0f;
					geWorld_SetModelXForm(CCD->World(), pDestroy->Model3, &xForm);

					if(!pDestroy->NoRemove)
						CCD->ModelManager()->SetPosition(pDestroy->Model3, xForm.Translation);
				}
				else
				{
					running = true;
				}
			}

			if(pDestroy->Model4)
			{
				if(!CCD->ModelManager()->IsRunning(pDestroy->Model4))
				{
					geXForm3d xForm;
					geWorld_GetModelXForm(CCD->World(), pDestroy->Model4, &xForm);
					xForm.Translation.X = 10000.0f;
					xForm.Translation.Y = 10000.0f;
					xForm.Translation.Z = 10000.0f;
					geWorld_SetModelXForm(CCD->World(), pDestroy->Model4, &xForm);

					if(!pDestroy->NoRemove)
						CCD->ModelManager()->SetPosition(pDestroy->Model4, xForm.Translation);
				}
				else
				{
					running = true;
				}
			}

			if(!running)
			{
				pDestroy->bState = GE_FALSE;
				pDestroy->Animating = GE_FALSE;
				pDestroy->CallBack = GE_TRUE;
				pDestroy->CallBackCount = 2;
			}

			continue;
		}

		if(!EffectC_IsStringNull(pDestroy->TriggerName))
		{
			if(GetTriggerState(pDestroy->TriggerName))
				pDestroy->active = GE_TRUE;
			else
				pDestroy->active = GE_FALSE;
		}
		else
		{
			pDestroy->active = GE_TRUE;
		}

		if(pDestroy->active == GE_TRUE && pDestroy->bState)
		{
			if(pDestroy->AttributeAmt <= 0.0f)
			{
				if(pDestroy->Animate)
				{
					pDestroy->Animating = GE_TRUE;
					CCD->ModelManager()->Start(pDestroy->Model);

					if(pDestroy->Model1)
						CCD->ModelManager()->Start(pDestroy->Model1);

					if(pDestroy->Model2)
						CCD->ModelManager()->Start(pDestroy->Model2);

					if(pDestroy->Model3)
						CCD->ModelManager()->Start(pDestroy->Model3);

					if(pDestroy->Model4)
						CCD->ModelManager()->Start(pDestroy->Model4);
				}
				else
				{
					pDestroy->bState = GE_FALSE;
					pDestroy->CallBack = GE_TRUE;
					pDestroy->CallBackCount = 2;

					geXForm3d xForm;
					geWorld_GetModelXForm(CCD->World(), pDestroy->Model, &xForm);
					xForm.Translation.X = 10000.0f;
					xForm.Translation.Y = 10000.0f;
					xForm.Translation.Z = 10000.0f;
					geWorld_SetModelXForm(CCD->World(), pDestroy->Model, &xForm);
					CCD->ModelManager()->SetPosition(pDestroy->Model, xForm.Translation);

					if(pDestroy->Model1)
					{
						geXForm3d xForm;
						geWorld_GetModelXForm(CCD->World(), pDestroy->Model1, &xForm);
						xForm.Translation.X = 10000.0f;
						xForm.Translation.Y = 10000.0f;
						xForm.Translation.Z = 10000.0f;
						geWorld_SetModelXForm(CCD->World(), pDestroy->Model1, &xForm);

						if(!pDestroy->NoRemove)
							CCD->ModelManager()->SetPosition(pDestroy->Model1, xForm.Translation);
					}

					if(pDestroy->Model2)
					{
						geXForm3d xForm;
						geWorld_GetModelXForm(CCD->World(), pDestroy->Model2, &xForm);
						xForm.Translation.X = 10000.0f;
						xForm.Translation.Y = 10000.0f;
						xForm.Translation.Z = 10000.0f;
						geWorld_SetModelXForm(CCD->World(), pDestroy->Model2, &xForm);

						if(!pDestroy->NoRemove)
							CCD->ModelManager()->SetPosition(pDestroy->Model2, xForm.Translation);
					}

					if(pDestroy->Model3)
					{
						geXForm3d xForm;
						geWorld_GetModelXForm(CCD->World(), pDestroy->Model3, &xForm);
						xForm.Translation.X = 10000.0f;
						xForm.Translation.Y = 10000.0f;
						xForm.Translation.Z = 10000.0f;
						geWorld_SetModelXForm(CCD->World(), pDestroy->Model3, &xForm);

						if(!pDestroy->NoRemove)
							CCD->ModelManager()->SetPosition(pDestroy->Model3, xForm.Translation);
					}

					if(pDestroy->Model4)
					{
						geXForm3d xForm;
						geWorld_GetModelXForm(CCD->World(), pDestroy->Model4, &xForm);
						xForm.Translation.X = 10000.0f;
						xForm.Translation.Y = 10000.0f;
						xForm.Translation.Z = 10000.0f;
						geWorld_SetModelXForm(CCD->World(), pDestroy->Model4, &xForm);

						if(!pDestroy->NoRemove)
							CCD->ModelManager()->SetPosition(pDestroy->Model4, xForm.Translation);
					}
				}

				if(!EffectC_IsStringNull(pDestroy->szSoundFile))
				{
					Snd sound;
					memset(&sound, 0, sizeof(sound));
					geWorld_GetModelRotationalCenter(CCD->World(), pDestroy->Model, &sound.Pos);
					sound.Min = CCD->Level()->GetAudibleRadius();
					sound.Loop = GE_FALSE;
					sound.SoundDef = SPool_Sound(pDestroy->szSoundFile);
					CCD->EffectManager()->Item_Add(EFF_SND, static_cast<void*>(&sound));
				}
			}
		}
	}
}


/* ------------------------------------------------------------------------------------ */
// SaveTo
//
// Save the current state off to an open file.
/* ------------------------------------------------------------------------------------ */
int CDamage::SaveTo(FILE* saveFD, bool type)
{
	// Ok, check to see if there are DestroyableModel in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "DestroyableModel");

	if(!pSet)
		return RGF_SUCCESS;

	geEntity *pEntity;

	// Ok, we have DestroyableModel somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		DestroyableModel *pDestroy = static_cast<DestroyableModel*>(geEntity_GetUserData(pEntity));

		WRITEDATA(type, &pDestroy->AttributeAmt, sizeof(geFloat), 1, saveFD);
	}

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// RestoreFrom
//
// Restore the state from an open file.
/* ------------------------------------------------------------------------------------ */
int CDamage::RestoreFrom(FILE* restoreFD, bool type)
{
	// Ok, check to see if there are DestroyableModel in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "DestroyableModel");

	if(!pSet)
		return RGF_SUCCESS;									// No doors, whatever...

	geEntity *pEntity;

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		DestroyableModel *pDestroy = static_cast<DestroyableModel*>(geEntity_GetUserData(pEntity));

		READDATA(type, &pDestroy->AttributeAmt, sizeof(geFloat), 1, restoreFD);
	}

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// DamageActor
/* ------------------------------------------------------------------------------------ */
void CDamage::DamageActor(const geActor *actor,
						  float amount, const std::string& attr,
						  float altAmount, const std::string& altAttr, const std::string& name)
{
	CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(actor);

	if(theInv)
	{
		int actualAmount;

		if(!attr.empty())
		{
			if(theInv->Has(attr) && (theInv->Value(attr) > theInv->Low(attr)))
			{
				actualAmount = static_cast<int>(amount);

				if(actor == CCD->Player()->GetActor())
				{
					actualAmount = CCD->Armours()->AdjustDamage(actualAmount, name, attr);
				}

				theInv->Modify(attr, -actualAmount);

				if(actor == CCD->Player()->GetActor())
				{
					sxInventory::GetSingletonPtr()->UpdateItem(attr, true);
				}

				return;
			}
		}

		if(!altAttr.empty())
		{
			if(theInv->Has(altAttr) && (theInv->Value(altAttr)>theInv->Low(altAttr)))
			{
				actualAmount = static_cast<int>(altAmount);

				if(actor == CCD->Player()->GetActor())
				{
					actualAmount = CCD->Armours()->AdjustDamage(actualAmount, name, altAttr);
				}

				theInv->Modify(altAttr, -actualAmount);

				if(actor == CCD->Player()->GetActor())
				{
					sxInventory::GetSingletonPtr()->UpdateItem(altAttr, true);
				}
			}
		}
	}
}


/* ------------------------------------------------------------------------------------ */
// DamageActorInRange
/* ------------------------------------------------------------------------------------ */
void CDamage::DamageActorInRange(geVec3d point, float range,
								 float amount, const std::string& attr,
								 float altAmount, const std::string& altAttr, const std::string& name)
{
	geActor *actorsInRange[512];

	int actorCount = CCD->ActorManager()->GetActorsInRange(point, range, 512, actorsInRange);

	for(int nTemp=0; nTemp<actorCount; ++nTemp)
	{
		DamageActor(actorsInRange[nTemp], amount, attr, altAmount, altAttr, name);
	}
}


/* ------------------------------------------------------------------------------------ */
// DamageModel
/* ------------------------------------------------------------------------------------ */
void CDamage::DamageModel(const geWorld_Model* model,
						  float amount, const std::string& attr,
						  float altAmount, const std::string& altAttr)
{
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "DestroyableModel");

	if(!pSet)
		return;

	geEntity *pEntity;

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		DestroyableModel *pDestroy= static_cast<DestroyableModel*>(geEntity_GetUserData(pEntity));

		if(!pDestroy->Model)
			continue;

		if(	model != pDestroy->Model  &&
			model != pDestroy->Model1 &&
			model != pDestroy->Model2 &&
			model != pDestroy->Model3 &&
			model != pDestroy->Model4)
			continue;

		if(EffectC_IsStringNull(pDestroy->Attribute))
		{
			if(attr == "health" && pDestroy->AttributeAmt > 0)
			{
				pDestroy->AttributeAmt -= amount;
			}
			else if(altAttr == "health")
			{
				pDestroy->AttributeAmt -= altAmount;
			}
		}
		else
		{
			if(attr == pDestroy->Attribute && pDestroy->AttributeAmt > 0)
			{
				pDestroy->AttributeAmt -= amount;
			}
			else if(altAttr == pDestroy->Attribute)
			{
				pDestroy->AttributeAmt -= altAmount;
			}
		}
	}
}


/* ------------------------------------------------------------------------------------ */
// DamageModelInRange
/* ------------------------------------------------------------------------------------ */
void CDamage::DamageModelInRange(geVec3d point, float range,
								 float amount, const std::string& attr,
								 float altAmount, const std::string& altAttr)
{
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "DestroyableModel");

	if(!pSet)
		return;

	float range2 = range * range;
	geEntity *pEntity;

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		DestroyableModel *pDestroy= static_cast<DestroyableModel*>(geEntity_GetUserData(pEntity));

		if(!pDestroy->Model)
			continue;

		if(pDestroy->active && pDestroy->bState)
		{
			geVec3d position;
			geWorld_GetModelRotationalCenter(CCD->World(), pDestroy->Model, &position);

			geVec3d temp;
			geVec3d_Subtract(&point, &position, &temp);

			float distance2;
			float minDistance2 = geVec3d_DotProduct(&temp, &temp);

			if(pDestroy->Model1)
			{
				geWorld_GetModelRotationalCenter(CCD->World(), pDestroy->Model1, &position);
				geVec3d_Subtract(&point, &position, &temp);
				distance2 = geVec3d_DotProduct(&temp, &temp);

				if(distance2 < minDistance2)
					minDistance2 = distance2;
			}

			if(pDestroy->Model2)
			{
				geWorld_GetModelRotationalCenter(CCD->World(), pDestroy->Model2, &position);
				geVec3d_Subtract(&point, &position, &temp);
				distance2 = geVec3d_DotProduct(&temp, &temp);

				if(distance2 < minDistance2)
					minDistance2 = distance2;
			}

			if(pDestroy->Model3)
			{
				geWorld_GetModelRotationalCenter(CCD->World(), pDestroy->Model3, &position);
				geVec3d_Subtract(&point, &position, &temp);
				distance2 = geVec3d_DotProduct(&temp, &temp);

				if(distance2 < minDistance2)
					minDistance2 = distance2;
			}

			if(pDestroy->Model4)
			{
				geWorld_GetModelRotationalCenter(CCD->World(), pDestroy->Model4, &position);
				geVec3d_Subtract(&point, &position, &temp);
				distance2 = geVec3d_DotProduct(&temp, &temp);

				if(distance2 < minDistance2)
					minDistance2 = distance2;
			}

			if(minDistance2 <= range2)
			{
				if(EffectC_IsStringNull(pDestroy->Attribute))
				{
					if(attr == "health" && pDestroy->AttributeAmt > 0)
					{
						pDestroy->AttributeAmt -= amount;
					}
					else if(altAttr == "health")
					{
						pDestroy->AttributeAmt -= altAmount;
					}
				}
				else
				{
					if(attr == pDestroy->Attribute && pDestroy->AttributeAmt > 0)
					{
						pDestroy->AttributeAmt -= amount;
					}
					else if(altAttr == pDestroy->Attribute)
					{
						pDestroy->AttributeAmt -= altAmount;
					}
				}
			}
		}
	}
}


/* ------------------------------------------------------------------------------------ */
// IsDestroyable
/* ------------------------------------------------------------------------------------ */
bool CDamage::IsDestroyable(geWorld_Model* model, int* percentage)
{
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "DestroyableModel");

	if(!pSet)
		return false;

	geEntity *pEntity;

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		DestroyableModel *pDestroy= static_cast<DestroyableModel*>(geEntity_GetUserData(pEntity));

		if(!pDestroy->Model)
			continue;

		if(	model != pDestroy->Model  &&
			model != pDestroy->Model1 &&
			model != pDestroy->Model2 &&
			model != pDestroy->Model3 &&
			model != pDestroy->Model4)
			continue;

		*percentage = (static_cast<int>(pDestroy->AttributeAmt) * 100)/static_cast<int>(pDestroy->OriginalAmt);

		return true;
	}

	return false;
}


// ******************** CRGF Overrides ********************

/* ------------------------------------------------------------------------------------ */
// LocateEntity
//
// Given a name, locate the desired item in the currently loaded level
// ..and return it's user data.
/* ------------------------------------------------------------------------------------ */
int CDamage::LocateEntity(const char *szName, void **pEntityData)
{
	// Ok, check to see if there are DestroyableModel in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "DestroyableModel");

	if(!pSet)
		return RGF_NOT_FOUND;

	geEntity *pEntity;

	// Ok, we have static entity proxies.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		DestroyableModel *pTheEntity = static_cast<DestroyableModel*>(geEntity_GetUserData(pEntity));

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
int CDamage::ReSynchronize()
{
	return RGF_SUCCESS;
}

/* ----------------------------------- END OF FILE ------------------------------------ */
