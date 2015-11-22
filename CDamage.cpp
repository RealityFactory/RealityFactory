/*
	CDamage.cpp:		Damage class implementation

	(c) 2001 Ralph Deane
	All Rights Reserved

	This file contains the class implementation for Damage
handling.
*/

//	Include the One True Header

#include "RabidFramework.h"

extern geSound_Def *SPool_Sound(char *SName);

//	Constructor
//
//	Load up all logic gates and set the
//	..entities to default values.

CDamage::CDamage()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	pSet = geWorld_GetEntitySet(CCD->World(), "DestroyableModel");
	
	if(!pSet) 
		return;	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
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
			sprintf(szError,"DestroyableModel : Missing Model '%s'", pDestroy->szEntityName);
			CCD->ReportError(szError, false);
			CCD->ShutdownLevel();
			delete CCD;
			MessageBox(NULL, szError,"Missing Destroyable Model", MB_OK);
			exit(-333);
		}
		pDestroy->bState = true;
		pDestroy->CallBack = false;
		pDestroy->active = false;
		pDestroy->OriginalAmt = pDestroy->AttributeAmt;
		if(!EffectC_IsStringNull(pDestroy->szSoundFile))
			SPool_Sound(pDestroy->szSoundFile);
	}
}

//	Destructor
//
//	Clean up.

CDamage::~CDamage()
{
}

void CDamage::Tick(float dwTicks)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	pSet = geWorld_GetEntitySet(CCD->World(), "DestroyableModel");
	
	if(!pSet) 
		return;	
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		DestroyableModel *pDestroy= (DestroyableModel*)geEntity_GetUserData(pEntity);
		if(pDestroy->CallBack==GE_TRUE)
		{
			pDestroy->CallBackCount-=1;
			if(pDestroy->CallBackCount==0)
				pDestroy->CallBack=GE_FALSE;
		}
		if(!EffectC_IsStringNull(pDestroy->TriggerName))
		{
			if(GetTriggerState(pDestroy->TriggerName))
				pDestroy->active = true;
			else
				pDestroy->active = false;
		}
		else
			pDestroy->active = true;
		if(pDestroy->active == GE_TRUE && pDestroy->bState)
		{
			if(pDestroy->AttributeAmt<=0.0f)
			{
				pDestroy->bState = false;
				pDestroy->CallBack = GE_TRUE;
				pDestroy->CallBackCount = 2;
				geXForm3d Xf1;
				geWorld_GetModelXForm(CCD->World(), pDestroy->Model, &Xf1);
				Xf1.Translation.X=10000.0f;
				Xf1.Translation.Y=10000.0f;
				Xf1.Translation.Z=10000.0f;
				geWorld_SetModelXForm(CCD->World(), pDestroy->Model, &Xf1);
				if(CCD->ModelManager()->IsModel(pDestroy->Model))
					CCD->ModelManager()->RemoveModel(pDestroy->Model);
				if(!EffectC_IsStringNull(pDestroy->szSoundFile))
				{
					Snd Sound;
					memset( &Sound, 0, sizeof( Sound ) );
					geWorld_GetModelRotationalCenter(CCD->World(), pDestroy->Model, 
						&Sound.Pos);
					Sound.Min=kAudibleRadius;
					Sound.Loop=false;
					Sound.SoundDef = SPool_Sound(pDestroy->szSoundFile);
					CCD->EffectManager()->Item_Add(EFF_SND, (void *)&Sound);
				}
			}
		}
	}
}

// changed RF063
//	SaveTo
//
//	Save the current state 
//	..off to an open file.

int CDamage::SaveTo(FILE *SaveFD)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	//	Ok, check to see if there are DestroyableModel in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "DestroyableModel");
	
	if(!pSet) 
		return RGF_SUCCESS;
	
	//	Ok, we have DestroyableModel somewhere.  Dig through 'em all.
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		DestroyableModel *pDestroy = (DestroyableModel*)geEntity_GetUserData(pEntity);
		fwrite(&pDestroy->AttributeAmt, sizeof(geFloat), 1, SaveFD);
	}
	
	return RGF_SUCCESS;
}

//	RestoreFrom
//
//	Restore the state from an
//	..open file.

int CDamage::RestoreFrom(FILE *RestoreFD)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	//	Ok, check to see if there are DestroyableModel in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "DestroyableModel");
	
	if(!pSet) 
		return RGF_SUCCESS;									// No doors, whatever...
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
    pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		DestroyableModel *pDestroy = (DestroyableModel*)geEntity_GetUserData(pEntity);
		fread(&pDestroy->AttributeAmt, sizeof(geFloat), 1, RestoreFD);
    }
	
	return RGF_SUCCESS;
}

void CDamage::DamageActor(geActor *Actor, float amount, char *Attr, float Altamount, char *AltAttr)
{
	CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(Actor);
	if(theInv)
	{
		if(!EffectC_IsStringNull(Attr))
		{
			if(theInv->Has(Attr) && (theInv->Value(Attr)>theInv->Low(Attr)))
			{
				theInv->Modify(Attr,(int)-amount);
				return;
			}
		}
		if(!EffectC_IsStringNull(AltAttr))
		{
			if(theInv->Has(AltAttr) && (theInv->Value(AltAttr)>theInv->Low(AltAttr)))
				theInv->Modify(AltAttr,(int)-Altamount);
		}
	}
	return;
}

void CDamage::DamageActorInRange(geVec3d Point, geFloat Range, float amount, char *Attr, float Altamount, char *AltAttr)
{
	geActor *ActorsInRange[512];

	int nActorCount = CCD->ActorManager()->GetActorsInRange(Point, 
		Range, 512, &ActorsInRange[0]);
	
	if(nActorCount != 0)
	{
		for(int nTemp = 0; nTemp < nActorCount; nTemp++)
		{
			DamageActor(ActorsInRange[nTemp], amount, Attr, Altamount, AltAttr);
		}
	}
}

void CDamage::DamageModel(geWorld_Model *Model, float amount, char *Attr, float Altamount, char *AltAttr)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	pSet = geWorld_GetEntitySet(CCD->World(), "DestroyableModel");
	
	if(!pSet) 
		return;	
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		DestroyableModel *pDestroy= (DestroyableModel*)geEntity_GetUserData(pEntity);

		if(Model != pDestroy->Model)
			continue;
		if(EffectC_IsStringNull(pDestroy->Attribute))
		{
			if(!stricmp(Attr, "health") && pDestroy->AttributeAmt>0)
			{
				pDestroy->AttributeAmt-=amount;
			}
			else if(!stricmp(AltAttr, "health"))
				pDestroy->AttributeAmt-=Altamount;
		}
		else
		{
			if(!stricmp(Attr, pDestroy->Attribute) && pDestroy->AttributeAmt>0)
			{
				pDestroy->AttributeAmt-=amount;
			}
			else if(!stricmp(AltAttr, pDestroy->Attribute))
				pDestroy->AttributeAmt-=Altamount;
		}
	}
}

void CDamage::DamageModelInRange(geVec3d Point, geFloat Range, float amount, char *Attr, float Altamount, char *AltAttr)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	pSet = geWorld_GetEntitySet(CCD->World(), "DestroyableModel");
	
	if(!pSet) 
		return;	
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		DestroyableModel *pDestroy= (DestroyableModel*)geEntity_GetUserData(pEntity);
		
		if(pDestroy->active == GE_TRUE && pDestroy->bState)
		{
			geVec3d thePosition;
			geWorld_GetModelRotationalCenter(CCD->World(), pDestroy->Model, 
					&thePosition);

			if((float)fabs(geVec3d_DistanceBetween(&Point, &thePosition))<Range)
			{
				if(EffectC_IsStringNull(pDestroy->Attribute))
				{
					if(!stricmp(Attr, "health") && pDestroy->AttributeAmt>0)
					{
						pDestroy->AttributeAmt-=amount;
					}
					else if(!stricmp(AltAttr, "health"))
						pDestroy->AttributeAmt-=Altamount;
				}
				else
				{
					if(!stricmp(Attr, pDestroy->Attribute) && pDestroy->AttributeAmt>0)
					{
						pDestroy->AttributeAmt-=amount;
					}
					else if(!stricmp(AltAttr, pDestroy->Attribute))
						pDestroy->AttributeAmt-=Altamount;
				}
			}
		}
	}
}
// end change RF063

bool CDamage::IsDestroyable(geWorld_Model *Model, int *Percentage)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	pSet = geWorld_GetEntitySet(CCD->World(), "DestroyableModel");
	
	if(!pSet) 
		return false;	
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		DestroyableModel *pDestroy= (DestroyableModel*)geEntity_GetUserData(pEntity);

		if(Model != pDestroy->Model)
			continue;
		*Percentage = ((int)pDestroy->AttributeAmt * 100)/(int)pDestroy->OriginalAmt;
		return true;
	}
	return false;
}

//	******************** CRGF Overrides ********************

//	LocateEntity
//
//	Given a name, locate the desired item in the currently loaded level
//	..and return it's user data.

int CDamage::LocateEntity(char *szName, void **pEntityData)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	//	Ok, check to see if there are DestroyableModel in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "DestroyableModel");
	
	if(!pSet) 
		return RGF_NOT_FOUND;
	
	//	Ok, we have static entity proxies.  Dig through 'em all.
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		DestroyableModel *pTheEntity = (DestroyableModel*)geEntity_GetUserData(pEntity);
		if(strcmp(pTheEntity->szEntityName, szName) == 0)
		{
			*pEntityData = (void *)pTheEntity;
			return RGF_SUCCESS;
		}
	}
	
	return RGF_NOT_FOUND;								// Sorry, no such entity here
}

//	ReSynchronize
//
//	Correct internal timing to match current time, to make up for time lost
//	..when outside the game loop (typically in "menu mode").

int CDamage::ReSynchronize()
{
	
	return RGF_SUCCESS;
}
