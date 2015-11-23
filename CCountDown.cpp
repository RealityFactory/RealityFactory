
#include "RabidFramework.h"

extern geSound_Def *SPool_Sound(char *SName);

CCountDown::CCountDown()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	//	Ok, see if we have any CountDownTimer entities at all
	
	pSet = geWorld_GetEntitySet(CCD->World(), "CountDownTimer");
	
	if(!pSet) 
		return;	
	
	//	Look through all of our CountDownTimers
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		CountDownTimer *pItem = (CountDownTimer*)geEntity_GetUserData(pEntity);
		if(EffectC_IsStringNull(pItem->szEntityName))
		{
			char szName[128];
			geEntity_GetName(pEntity, szName, 128);
			pItem->szEntityName = szName;
		}
		// Ok, put this entity into the Global Entity Registry
		CCD->EntityRegistry()->AddEntity(pItem->szEntityName, "CountDownTimer");
		pItem->active = GE_FALSE;
		pItem->alive = GE_FALSE;
		pItem->Time = 0.0f;
		pItem->OldState = GE_FALSE;
		pItem->bState = GE_FALSE;
		if(!EffectC_IsStringNull(pItem->Attribute))
		{
			CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
			if(theInv->Has(pItem->Attribute))
			{
				pItem->alive = GE_TRUE;
				theInv->SetValueLimits(pItem->Attribute, 0, (int)pItem->TimeAmt);
				theInv->Set(pItem->Attribute, 0);
			}
		}
		if(!EffectC_IsStringNull(pItem->SoundFile))
			SPool_Sound(pItem->SoundFile);
		pItem->index = -1;
	}
}

CCountDown::~CCountDown()
{
}

void CCountDown::Tick(float dwTicks)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	//	Ok, see if we have any CountDownTimer entities at all
	
	pSet = geWorld_GetEntitySet(CCD->World(), "CountDownTimer");
	
	if(!pSet) 
		return;	
	
	//	Look through all of our CountDownTimers

	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		CountDownTimer *pItem = (CountDownTimer*)geEntity_GetUserData(pEntity);
		CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
		if(!EffectC_IsStringNull(pItem->StopTrigger))
		{
			if(GetTriggerState(pItem->StopTrigger))
			{
				pItem->active = GE_FALSE;
				pItem->bState = GE_FALSE;
				if(pItem->index!=-1)
				{
					CCD->EffectManager()->Item_Delete(EFF_SND, pItem->index);
					pItem->index = -1;
				}
				continue;
			}
		}
		if(!pItem->active)
		{
			if(!EffectC_IsStringNull(pItem->TriggerName))
			{
				if(GetTriggerState(pItem->TriggerName))
				{
					if(!pItem->OldState)
					{
						pItem->OldState = GE_TRUE;
						pItem->Time = pItem->TimeAmt;
						pItem->active = GE_TRUE;
						pItem->bState = GE_FALSE;
						if(pItem->alive)
							theInv->Set(pItem->Attribute, (int)pItem->Time);
						if(!EffectC_IsStringNull(pItem->SoundFile))
						{
							Snd Sound;
							memset( &Sound, 0, sizeof( Sound ) );
							CCD->ActorManager()->GetPosition(CCD->Player()->GetActor(), &Sound.Pos);
							Sound.Min=CCD->GetAudibleRadius();
							Sound.Loop=true;
							Sound.SoundDef = SPool_Sound(pItem->SoundFile);
							pItem->index = CCD->EffectManager()->Item_Add(EFF_SND, (void *)&Sound);
						}
					}
				}
				else
					pItem->OldState = GE_FALSE;
			}
		}
		else
		{
			pItem->Time -= (dwTicks*0.001f);
			if(pItem->Time<=0.0f)
			{
				pItem->active = GE_FALSE;
				pItem->bState = GE_TRUE;
				if(pItem->index!=-1)
				{
					CCD->EffectManager()->Item_Delete(EFF_SND, pItem->index);
					pItem->index = -1;
				}
			}
			if(pItem->alive)
				theInv->Set(pItem->Attribute, (int)pItem->Time);
		}
	}
}

//	******************** CRGF Overrides ********************

//	LocateEntity
//
//	Given a name, locate the desired item in the currently loaded level
//	..and return it's user data.

int CCountDown::LocateEntity(char *szName, void **pEntityData)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	//	Ok, check to see if there are CountDownTimers in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "CountDownTimer");
	
	if(!pSet) 
		return RGF_NOT_FOUND;
	
	//	Ok, we have CountDownTimers.  Dig through 'em all.
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		CountDownTimer *pTheEntity = (CountDownTimer*)geEntity_GetUserData(pEntity);
		if(strcmp(pTheEntity->szEntityName, szName) == 0)
		{
			*pEntityData = (void *)pTheEntity;
			return RGF_SUCCESS;
		}
	}
	
	return RGF_NOT_FOUND;								// Sorry, no such entity here
}

//	SaveTo
//
//	Save the current state of every  CountDownTimer in the current world
//	..off to an open file.

int CCountDown::SaveTo(FILE *SaveFD, bool type)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

//	Ok, check to see if there are  CountDownTimers in this world

	pSet = geWorld_GetEntitySet(CCD->World(), "CountDownTimer");

	if(!pSet)
		return RGF_SUCCESS;

//	Ok, we have CountDownTimers somewhere.  Dig through 'em all.

	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	    pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity))
		{
		CountDownTimer *pSource = (CountDownTimer*)geEntity_GetUserData(pEntity);
		WRITEDATA(&pSource->active, sizeof(geBoolean), 1, SaveFD);
		WRITEDATA(&pSource->bState, sizeof(geBoolean), 1, SaveFD);
		WRITEDATA(&pSource->OldState, sizeof(geBoolean), 1, SaveFD);
		WRITEDATA(&pSource->Time, sizeof(float), 1, SaveFD);
	  }

  return RGF_SUCCESS;
}

//	RestoreFrom
//
//	Restore the state of every CountDownTimer in the current world from an
//	..open file.

int CCountDown::RestoreFrom(FILE *RestoreFD, bool type)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

//	Ok, check to see if there are CountDownTimers in this world

	pSet = geWorld_GetEntitySet(CCD->World(), "CountDownTimer");

	if(!pSet)
		return RGF_SUCCESS;

  for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
    pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity))
	  {
		CountDownTimer *pSource = (CountDownTimer*)geEntity_GetUserData(pEntity);
		READDATA(&pSource->active, sizeof(geBoolean), 1, RestoreFD);
		READDATA(&pSource->bState, sizeof(geBoolean), 1, RestoreFD);
		READDATA(&pSource->OldState, sizeof(geBoolean), 1, RestoreFD);
		READDATA(&pSource->Time, sizeof(float), 1, RestoreFD);
    }

  return RGF_SUCCESS;
}
