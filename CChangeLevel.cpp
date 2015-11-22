/*
	CChangeLevel.h:		Changelevel class handler

	(c) 2001 Ralph Deane
	All Rights Reserved

	This file contains the class declaration for ChangeLevel
handling.
*/

//	Include the One True Header

#include "RabidFramework.h"

CChangeLevel::CChangeLevel()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	//	Ok, see if we have any changelevel entities at all
	
	pSet = geWorld_GetEntitySet(CCD->World(), "ChangeLevel");
	
	if(!pSet) 
		return;	
	
	//	Look through all of our changelevels
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		ChangeLevel *pItem = (ChangeLevel*)geEntity_GetUserData(pEntity);
		if(EffectC_IsStringNull(pItem->szEntityName))
		{
			char szName[128];
			geEntity_GetName(pEntity, szName, 128);
			pItem->szEntityName = szName;
		}
		// Ok, put this entity into the Global Entity Registry
		CCD->EntityRegistry()->AddEntity(pItem->szEntityName, "ChangeLevel");
		pItem->CallBack = GE_FALSE;
	}
}

CChangeLevel::~CChangeLevel()
{
}

void CChangeLevel::Tick(float dwTicks)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	//	Ok, see if we have any changelevel entities at all
	
	pSet = geWorld_GetEntitySet(CCD->World(), "ChangeLevel");
	
	if(!pSet) 
		return;	
	
	//	Look through all of our changelevels to see if the model
	//	..we hit is one of 'em.
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		ChangeLevel *pItem = (ChangeLevel*)geEntity_GetUserData(pEntity);
// change RF063
		if(!EffectC_IsStringNull(pItem->TriggerChange))
		{
			if(GetTriggerState(pItem->TriggerChange))
			{
				CCD->SetChangeLevelData(pItem);
				return;
			}
		}
// end change RF063
		if(pItem->CallBack==GE_TRUE)
		{
			pItem->CallBackCount-=1;
			if(pItem->CallBackCount==0)
				pItem->CallBack=GE_FALSE;
		}
	}
}

bool CChangeLevel::CheckChangeLevel(geWorld_Model *theModel, bool UseKey)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	//	Ok, see if we have any changelevel entities at all
	
	pSet = geWorld_GetEntitySet(CCD->World(), "ChangeLevel");
	
	if(!pSet) 
		return false;				// This model is no exit
	
	//	Look through all of our changelevels to see if the model
	//	..we hit is one of 'em.
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		ChangeLevel *pItem = (ChangeLevel*)geEntity_GetUserData(pEntity);
		if(pItem->Model == theModel)
		{
			if(UseKey && !pItem->UseKey)
				return false;

			if(!EffectC_IsStringNull(pItem->Trigger))
			{
				if(!GetTriggerState(pItem->Trigger))
				{
					if(!pItem->CallBack)
					{
						pItem->CallBack = GE_TRUE;
						pItem->CallBackCount = 2;
					}
					return false;
				}
			}
			// Hmm, we DID hit one.  Save offf the new level filename and the
			// ..splash screen name for later, so that the framework can get
			// ..to this information.
			CCD->SetChangeLevelData(pItem);
			return true;
		}
	}
	
	return false;									// Nope, not a changelevel model
}

//	******************** CRGF Overrides ********************

//	LocateEntity
//
//	Given a name, locate the desired item in the currently loaded level
//	..and return it's user data.

int CChangeLevel::LocateEntity(char *szName, void **pEntityData)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	//	Ok, check to see if there are ChangeLevels in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "ChangeLevel");
	
	if(!pSet) 
		return RGF_NOT_FOUND;	
	
	//	Ok, we have ChangeLevels.  Dig through 'em all.
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		ChangeLevel *pTheEntity = (ChangeLevel*)geEntity_GetUserData(pEntity);
		if(strcmp(pTheEntity->szEntityName, szName) == 0)
		{
			*pEntityData = (void *)pTheEntity;
			return RGF_SUCCESS;
		}
	}
	
	return RGF_NOT_FOUND;								// Sorry, no such entity here
}
