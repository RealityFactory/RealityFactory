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

extern geSound_Def *SPool_Sound(const char *SName);

/* ------------------------------------------------------------------------------------ */
//	Constructor
//
//	Load up all logic gates and set the	entities to default values.
/* ------------------------------------------------------------------------------------ */
CDamage::CDamage()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	pSet = geWorld_GetEntitySet(CCD->World(), "DestroyableModel");

	if(!pSet)
		return;

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		DestroyableModel *pDestroy = (DestroyableModel*)geEntity_GetUserData(pEntity);

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
			char szError[256];
			sprintf(szError,"[WARNING] File %s - Line %d: DestroyableModel: '%s' Missing Model\n",
					__FILE__, __LINE__, pDestroy->szEntityName);
			CCD->ReportError(szError, false);
			continue;
			// changed QD 07/15/06
			/*
			CCD->ShutdownLevel();
			delete CCD;
			CCD = NULL;
			MessageBox(NULL, szError, "Missing Destroyable Model", MB_OK);
			exit(-333);
			*/
			// end change
		}

		pDestroy->bState = GE_TRUE;
		pDestroy->CallBack = GE_FALSE;
		pDestroy->active = GE_FALSE;
// changed RF064
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
// end change RF064

		pDestroy->OriginalAmt = pDestroy->AttributeAmt;

		if(!EffectC_IsStringNull(pDestroy->szSoundFile))
			SPool_Sound(pDestroy->szSoundFile);
	}
}

/* ------------------------------------------------------------------------------------ */
//	Destructor
//
//	Clean up.
/* ------------------------------------------------------------------------------------ */
CDamage::~CDamage()
{
}

/* ------------------------------------------------------------------------------------ */
//	Tick
/* ------------------------------------------------------------------------------------ */
void CDamage::Tick(geFloat dwTicks)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	pSet = geWorld_GetEntitySet(CCD->World(), "DestroyableModel");

	if(!pSet)
		return;

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		DestroyableModel *pDestroy = (DestroyableModel*)geEntity_GetUserData(pEntity);

		// changed QD 07/15/06
		if(!pDestroy->Model)
			continue;
		// end change

		if(pDestroy->CallBack == GE_TRUE)
		{
			pDestroy->CallBackCount -= 1;

			if(pDestroy->CallBackCount == 0)
				pDestroy->CallBack = GE_FALSE;
		}

// changed RF064
		if(pDestroy->Animating)
		{
			bool running = false;

			if(!CCD->ModelManager()->IsRunning(pDestroy->Model))
			{
				geXForm3d Xf1;
				geWorld_GetModelXForm(CCD->World(), pDestroy->Model, &Xf1);
				Xf1.Translation.X = 10000.0f;
				Xf1.Translation.Y = 10000.0f;
				Xf1.Translation.Z = 10000.0f;
				geWorld_SetModelXForm(CCD->World(), pDestroy->Model, &Xf1);

				if(!pDestroy->NoRemove)
					CCD->ModelManager()->SetPosition(pDestroy->Model, Xf1.Translation);
			}
			else
				running = true;

			if(pDestroy->Model1)
			{
				if(!CCD->ModelManager()->IsRunning(pDestroy->Model1))
				{
					geXForm3d Xf1;
					geWorld_GetModelXForm(CCD->World(), pDestroy->Model1, &Xf1);
					Xf1.Translation.X = 10000.0f;
					Xf1.Translation.Y = 10000.0f;
					Xf1.Translation.Z = 10000.0f;
					geWorld_SetModelXForm(CCD->World(), pDestroy->Model1, &Xf1);

					if(!pDestroy->NoRemove)
						CCD->ModelManager()->SetPosition(pDestroy->Model1, Xf1.Translation);
				}
				else
					running = true;
			}

			if(pDestroy->Model2)
			{
				if(!CCD->ModelManager()->IsRunning(pDestroy->Model2))
				{
					geXForm3d Xf1;
					geWorld_GetModelXForm(CCD->World(), pDestroy->Model2, &Xf1);
					Xf1.Translation.X = 10000.0f;
					Xf1.Translation.Y = 10000.0f;
					Xf1.Translation.Z = 10000.0f;
					geWorld_SetModelXForm(CCD->World(), pDestroy->Model2, &Xf1);

					if(!pDestroy->NoRemove)
						CCD->ModelManager()->SetPosition(pDestroy->Model2, Xf1.Translation);
				}
				else
					running = true;
			}

			if(pDestroy->Model3)
			{
				if(!CCD->ModelManager()->IsRunning(pDestroy->Model3))
				{
					geXForm3d Xf1;
					geWorld_GetModelXForm(CCD->World(), pDestroy->Model3, &Xf1);
					Xf1.Translation.X = 10000.0f;
					Xf1.Translation.Y = 10000.0f;
					Xf1.Translation.Z = 10000.0f;
					geWorld_SetModelXForm(CCD->World(), pDestroy->Model3, &Xf1);

					if(!pDestroy->NoRemove)
						CCD->ModelManager()->SetPosition(pDestroy->Model3, Xf1.Translation);
				}
				else
					running = true;
			}

			if(pDestroy->Model4)
			{
				if(!CCD->ModelManager()->IsRunning(pDestroy->Model4))
				{
					geXForm3d Xf1;
					geWorld_GetModelXForm(CCD->World(), pDestroy->Model4, &Xf1);
					Xf1.Translation.X = 10000.0f;
					Xf1.Translation.Y = 10000.0f;
					Xf1.Translation.Z = 10000.0f;
					geWorld_SetModelXForm(CCD->World(), pDestroy->Model4, &Xf1);

					if(!pDestroy->NoRemove)
						CCD->ModelManager()->SetPosition(pDestroy->Model4, Xf1.Translation);
				}
				else
					running = true;
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
			pDestroy->active = GE_TRUE;

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

					geXForm3d Xf1;
					geWorld_GetModelXForm(CCD->World(), pDestroy->Model, &Xf1);
					Xf1.Translation.X = 10000.0f;
					Xf1.Translation.Y = 10000.0f;
					Xf1.Translation.Z = 10000.0f;
					geWorld_SetModelXForm(CCD->World(), pDestroy->Model, &Xf1);
					CCD->ModelManager()->SetPosition(pDestroy->Model, Xf1.Translation);

					if(pDestroy->Model1)
					{
						geXForm3d Xf1;
						geWorld_GetModelXForm(CCD->World(), pDestroy->Model1, &Xf1);
						Xf1.Translation.X = 10000.0f;
						Xf1.Translation.Y = 10000.0f;
						Xf1.Translation.Z = 10000.0f;
						geWorld_SetModelXForm(CCD->World(), pDestroy->Model1, &Xf1);

						if(!pDestroy->NoRemove)
							CCD->ModelManager()->SetPosition(pDestroy->Model1, Xf1.Translation);
					}

					if(pDestroy->Model2)
					{
						geXForm3d Xf1;
						geWorld_GetModelXForm(CCD->World(), pDestroy->Model2, &Xf1);
						Xf1.Translation.X = 10000.0f;
						Xf1.Translation.Y = 10000.0f;
						Xf1.Translation.Z = 10000.0f;
						geWorld_SetModelXForm(CCD->World(), pDestroy->Model2, &Xf1);

						if(!pDestroy->NoRemove)
							CCD->ModelManager()->SetPosition(pDestroy->Model2, Xf1.Translation);
					}

					if(pDestroy->Model3)
					{
						geXForm3d Xf1;
						geWorld_GetModelXForm(CCD->World(), pDestroy->Model3, &Xf1);
						Xf1.Translation.X = 10000.0f;
						Xf1.Translation.Y = 10000.0f;
						Xf1.Translation.Z = 10000.0f;
						geWorld_SetModelXForm(CCD->World(), pDestroy->Model3, &Xf1);

						if(!pDestroy->NoRemove)
							CCD->ModelManager()->SetPosition(pDestroy->Model3, Xf1.Translation);
					}

					if(pDestroy->Model4)
					{
						geXForm3d Xf1;
						geWorld_GetModelXForm(CCD->World(), pDestroy->Model4, &Xf1);
						Xf1.Translation.X = 10000.0f;
						Xf1.Translation.Y = 10000.0f;
						Xf1.Translation.Z = 10000.0f;
						geWorld_SetModelXForm(CCD->World(), pDestroy->Model4, &Xf1);

						if(!pDestroy->NoRemove)
							CCD->ModelManager()->SetPosition(pDestroy->Model4, Xf1.Translation);
					}
				}

				if(!EffectC_IsStringNull(pDestroy->szSoundFile))
				{
					Snd Sound;
					memset(&Sound, 0, sizeof(Sound));
					geWorld_GetModelRotationalCenter(CCD->World(), pDestroy->Model, &(Sound.Pos));
					Sound.Min = CCD->GetAudibleRadius();
					Sound.Loop = GE_FALSE;
					Sound.SoundDef = SPool_Sound(pDestroy->szSoundFile);
					CCD->EffectManager()->Item_Add(EFF_SND, (void*)&Sound);
				}
			}
// end change RF064
		}
	}
}

// changed RF063
/* ------------------------------------------------------------------------------------ */
//	SaveTo
//
//	Save the current state off to an open file.
/* ------------------------------------------------------------------------------------ */
int CDamage::SaveTo(FILE *SaveFD, bool type)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	//	Ok, check to see if there are DestroyableModel in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "DestroyableModel");

	if(!pSet)
		return RGF_SUCCESS;

	//	Ok, we have DestroyableModel somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		DestroyableModel *pDestroy = (DestroyableModel*)geEntity_GetUserData(pEntity);

		WRITEDATA(type, &pDestroy->AttributeAmt, sizeof(geFloat), 1, SaveFD);
	}

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	RestoreFrom
//
//	Restore the state from an open file.
/* ------------------------------------------------------------------------------------ */
int CDamage::RestoreFrom(FILE *RestoreFD, bool type)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	//	Ok, check to see if there are DestroyableModel in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "DestroyableModel");

	if(!pSet)
		return RGF_SUCCESS;									// No doors, whatever...

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	    pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		DestroyableModel *pDestroy = (DestroyableModel*)geEntity_GetUserData(pEntity);

		READDATA(type, &pDestroy->AttributeAmt, sizeof(geFloat), 1, RestoreFD);
    }

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	DamageActor
/* ------------------------------------------------------------------------------------ */
void CDamage::DamageActor(const geActor *Actor,
						  float amount, const char *Attr,
						  float Altamount, const char *AltAttr, const char *name)
{
	int ActualAmount;

	CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(Actor);

	if(theInv)
	{
		if(!EffectC_IsStringNull(Attr))
		{
			if(theInv->Has(Attr) && (theInv->Value(Attr) > theInv->Low(Attr)))
			{
				ActualAmount = (int)amount;

// changed QD 12/15/05 - bug fix
				if(Actor == CCD->Player()->GetActor())
				{
					ActualAmount = CCD->Armours()->AdjustDamage(ActualAmount, name, Attr);
				}

				theInv->Modify(Attr, -ActualAmount);
				return;
			}
		}

		if(!EffectC_IsStringNull(AltAttr))
		{
			if(theInv->Has(AltAttr) && (theInv->Value(AltAttr)>theInv->Low(AltAttr)))
			{
				ActualAmount = (int)Altamount;

// changed QD 12/15/05 - bug fix
				if(Actor == CCD->Player()->GetActor())
				{
					ActualAmount = CCD->Armours()->AdjustDamage(ActualAmount, name, AltAttr);
				}

				theInv->Modify(AltAttr, -ActualAmount);
			}
		}
	}
	return;
}

/* ------------------------------------------------------------------------------------ */
//	DamageActorInRange
/* ------------------------------------------------------------------------------------ */
void CDamage::DamageActorInRange(geVec3d Point, geFloat Range,
								 float amount, const char *Attr,
								 float Altamount, const char *AltAttr, const char *name)
{
	geActor *ActorsInRange[512];

	int nActorCount = CCD->ActorManager()->GetActorsInRange(Point, Range, 512, &ActorsInRange[0]);

// changed QD 12/15/05 - if test not needed, for loop wouldn't start
	// if(nActorCount != 0)
	{
		for(int nTemp=0; nTemp<nActorCount; nTemp++)
		{
			DamageActor(ActorsInRange[nTemp], amount, Attr, Altamount, AltAttr, name);
		}
	}
}

/* ------------------------------------------------------------------------------------ */
//	DamageModel
/* ------------------------------------------------------------------------------------ */
void CDamage::DamageModel(const geWorld_Model *Model,
						  float amount, const char *Attr,
						  float Altamount, const char *AltAttr)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	pSet = geWorld_GetEntitySet(CCD->World(), "DestroyableModel");

	if(!pSet)
		return;

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		DestroyableModel *pDestroy= (DestroyableModel*)geEntity_GetUserData(pEntity);

		// changed QD 07/15/06
		if(!pDestroy->Model)
			continue;
		// end change

// changed RF064
		if(Model != pDestroy->Model && Model != pDestroy->Model1 && Model != pDestroy->Model2
			&& Model != pDestroy->Model3 && Model != pDestroy->Model4)
			continue;
// end change RF064

		if(EffectC_IsStringNull(pDestroy->Attribute))
		{
			if(!stricmp(Attr, "health") && pDestroy->AttributeAmt > 0)
			{
				pDestroy->AttributeAmt -= amount;
			}
			else if(!stricmp(AltAttr, "health"))
				pDestroy->AttributeAmt -= Altamount;
		}
		else
		{
			if(!stricmp(Attr, pDestroy->Attribute) && pDestroy->AttributeAmt > 0)
			{
				pDestroy->AttributeAmt -= amount;
			}
			else if(!stricmp(AltAttr, pDestroy->Attribute))
				pDestroy->AttributeAmt -= Altamount;
		}
	}
}

/* ------------------------------------------------------------------------------------ */
//	DamageModelInRange
/* ------------------------------------------------------------------------------------ */
void CDamage::DamageModelInRange(geVec3d Point, geFloat Range,
								 float amount, const char *Attr,
								 float Altamount, const char *AltAttr)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	pSet = geWorld_GetEntitySet(CCD->World(), "DestroyableModel");

	if(!pSet)
		return;

// changed QD 12/15/05 - use squared distance
	geFloat Range2 = Range*Range;

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		DestroyableModel *pDestroy= (DestroyableModel*)geEntity_GetUserData(pEntity);

		// changed QD 07/15/06
		if(!pDestroy->Model)
			continue;
		// end change

		if(pDestroy->active && pDestroy->bState)
		{
			geVec3d thePosition;

			//float Distance, MinDistance;
			float Distance2, MinDistance2;
			geVec3d Temp;

			geWorld_GetModelRotationalCenter(CCD->World(), pDestroy->Model, &thePosition);

			// MinDistance = (float)fabs(geVec3d_DistanceBetween(&Point, &thePosition));
			geVec3d_Subtract(&Point, &thePosition, &Temp);
			MinDistance2 = geVec3d_DotProduct(&Temp, &Temp);

			if(pDestroy->Model1)
			{
				geWorld_GetModelRotationalCenter(CCD->World(), pDestroy->Model1, &thePosition);
				// Distance = (float)fabs(geVec3d_DistanceBetween(&Point, &thePosition));
				geVec3d_Subtract(&Point, &thePosition, &Temp);
				Distance2 = geVec3d_DotProduct(&Temp, &Temp);

				// if(Distance < MinDistance)
				//	MinDistance = Distance;
				if(Distance2 < MinDistance2)
					MinDistance2 = Distance2;
			}

			if(pDestroy->Model2)
			{
				geWorld_GetModelRotationalCenter(CCD->World(), pDestroy->Model2, &thePosition);
				// Distance = (float)fabs(geVec3d_DistanceBetween(&Point, &thePosition));
				geVec3d_Subtract(&Point, &thePosition, &Temp);
				Distance2 = geVec3d_DotProduct(&Temp, &Temp);

				// if(Distance < MinDistance)
				//	MinDistance = Distance;
				if(Distance2 < MinDistance2)
					MinDistance2 = Distance2;
			}

			if(pDestroy->Model3)
			{
				geWorld_GetModelRotationalCenter(CCD->World(), pDestroy->Model3, &thePosition);
				// Distance = (float)fabs(geVec3d_DistanceBetween(&Point, &thePosition));
				geVec3d_Subtract(&Point, &thePosition, &Temp);
				Distance2 = geVec3d_DotProduct(&Temp, &Temp);

				// if(Distance < MinDistance)
				//	MinDistance = Distance;
				if(Distance2 < MinDistance2)
					MinDistance2 = Distance2;
			}

			if(pDestroy->Model4)
			{
				geWorld_GetModelRotationalCenter(CCD->World(), pDestroy->Model4, &thePosition);
				// Distance = (float)fabs(geVec3d_DistanceBetween(&Point, &thePosition));
				geVec3d_Subtract(&Point, &thePosition, &Temp);
				Distance2 = geVec3d_DotProduct(&Temp, &Temp);

				// if(Distance < MinDistance)
				//	MinDistance = Distance;
				if(Distance2 < MinDistance2)
					MinDistance2 = Distance2;
			}

			//if(MinDistance <= Range)
			if(MinDistance2 <= Range2)
// end change
			{
				if(EffectC_IsStringNull(pDestroy->Attribute))
				{
					if(!stricmp(Attr, "health") && pDestroy->AttributeAmt > 0)
					{
						pDestroy->AttributeAmt -= amount;
					}
					else if(!stricmp(AltAttr, "health"))
						pDestroy->AttributeAmt -= Altamount;
				}
				else
				{
					if(!stricmp(Attr, pDestroy->Attribute) && pDestroy->AttributeAmt > 0)
					{
						pDestroy->AttributeAmt -= amount;
					}
					else if(!stricmp(AltAttr, pDestroy->Attribute))
						pDestroy->AttributeAmt -= Altamount;
				}
			}
		}
	}
}
// end change RF063

/* ------------------------------------------------------------------------------------ */
//	IsDestroyable
/* ------------------------------------------------------------------------------------ */
bool CDamage::IsDestroyable(geWorld_Model *Model, int *Percentage)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	pSet = geWorld_GetEntitySet(CCD->World(), "DestroyableModel");

	if(!pSet)
		return false;

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		DestroyableModel *pDestroy= (DestroyableModel*)geEntity_GetUserData(pEntity);

		// changed QD 07/15/06
		if(!pDestroy->Model)
			continue;
		// end change

// changed RF064
		if(Model != pDestroy->Model && Model != pDestroy->Model1 && Model != pDestroy->Model2
			&& Model != pDestroy->Model3 && Model != pDestroy->Model4)
			continue;
// end change RF064

		*Percentage = ((int)pDestroy->AttributeAmt * 100)/(int)pDestroy->OriginalAmt;

		return true;
	}

	return false;
}

//	******************** CRGF Overrides ********************

/* ------------------------------------------------------------------------------------ */
//	LocateEntity
//
//	Given a name, locate the desired item in the currently loaded level
//	..and return it's user data.
/* ------------------------------------------------------------------------------------ */
int CDamage::LocateEntity(const char *szName, void **pEntityData)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	//	Ok, check to see if there are DestroyableModel in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "DestroyableModel");

	if(!pSet)
		return RGF_NOT_FOUND;

	//	Ok, we have static entity proxies.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		DestroyableModel *pTheEntity = (DestroyableModel*)geEntity_GetUserData(pEntity);

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
int CDamage::ReSynchronize()
{
	return RGF_SUCCESS;
}

/* ----------------------------------- END OF FILE ------------------------------------ */
