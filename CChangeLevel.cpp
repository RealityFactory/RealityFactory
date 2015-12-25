/************************************************************************************//**
 * @file CChangeLevel.cpp
 * @brief Changelevel class handler
 *
 * This file contains the class implementation for ChangeLevel handling.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

//	Include the One True Header
#include "RabidFramework.h"
#include "CChangeLevel.h"

/* ------------------------------------------------------------------------------------ */
//	Constructor
/* ------------------------------------------------------------------------------------ */
CChangeLevel::CChangeLevel()
{
	//	Ok, see if we have any changelevel entities at all
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "ChangeLevel");

	if(!pSet)
		return;

	geEntity *pEntity;

	//	Look through all of our changelevels
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		ChangeLevel *pItem = static_cast<ChangeLevel*>(geEntity_GetUserData(pEntity));

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

/* ------------------------------------------------------------------------------------ */
//	Destructor
/* ------------------------------------------------------------------------------------ */
CChangeLevel::~CChangeLevel()
{
}

/* ------------------------------------------------------------------------------------ */
//	Tick
/* ------------------------------------------------------------------------------------ */
void CChangeLevel::Tick(geFloat /*dwTicks*/)
{
	//	Ok, see if we have any changelevel entities at all
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "ChangeLevel");

	if(!pSet)
		return;

	geEntity *pEntity;

	//	Look through all of our changelevels to see if the model
	//	..we hit is one of 'em.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		ChangeLevel *pItem = static_cast<ChangeLevel*>(geEntity_GetUserData(pEntity));

		if(!EffectC_IsStringNull(pItem->TriggerChange))
		{
			if(GetTriggerState(pItem->TriggerChange))
			{
				CCD->SetChangeLevelData(pItem);
				CCD->SetChangeLevel(true);
				CCD->SetKeyPaused(false);
				return;
			}
		}

		if(pItem->CallBack == GE_TRUE)
		{
			pItem->CallBackCount -= 1;
			if(pItem->CallBackCount == 0)
				pItem->CallBack = GE_FALSE;
		}
	}
}

/* ------------------------------------------------------------------------------------ */
//	CheckChangeLevel
/* ------------------------------------------------------------------------------------ */
bool CChangeLevel::CheckChangeLevel(geWorld_Model *theModel, bool UseKey)
{
	//	Ok, see if we have any changelevel entities at all
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "ChangeLevel");

	if(!pSet)
		return false;				// This model is no exit

	geEntity *pEntity;

	//	Look through all of our changelevels to see if the model
	//	..we hit is one of 'em.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		ChangeLevel *pItem = static_cast<ChangeLevel*>(geEntity_GetUserData(pEntity));

		if(pItem->Model == theModel)
		{
			if(UseKey && !pItem->UseKey)
				continue;

			if(!EffectC_IsStringNull(pItem->Trigger))
			{
				if(!GetTriggerState(pItem->Trigger))
				{
					if(!pItem->CallBack)
					{
						pItem->CallBack = GE_TRUE;
						pItem->CallBackCount = 2;
					}

					continue;
				}
				else
				{
					CCD->SetChangeLevelData(pItem);
					return true;
				}
			}
			else
			{
				// Hmm, we DID hit one.  Save offf the new level filename and the
				// ..splash screen name for later, so that the framework can get
				// ..to this information.
				CCD->SetChangeLevelData(pItem);
				return true;
			}
		}
	}

	return false;									// Nope, not a changelevel model
}

//	******************** CRGF Overrides ********************

/* ------------------------------------------------------------------------------------ */
//	LocateEntity
//
//	Given a name, locate the desired item in the currently loaded level
//	..and return it's user data.
/* ------------------------------------------------------------------------------------ */
int CChangeLevel::LocateEntity(const char *szName, void **pEntityData)
{
	//	Ok, check to see if there are ChangeLevels in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "ChangeLevel");

	if(!pSet)
		return RGF_NOT_FOUND;

	geEntity *pEntity;

	//	Ok, we have ChangeLevels.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		ChangeLevel *pTheEntity = static_cast<ChangeLevel*>(geEntity_GetUserData(pEntity));

		if(!strcmp(pTheEntity->szEntityName, szName))
		{
			*pEntityData = static_cast<void*>(pTheEntity);
			return RGF_SUCCESS;
		}
	}

	return RGF_NOT_FOUND;								// Sorry, no such entity here
}

/* ------------------------------------------------------------------------------------ */
//	SaveTo
//
//	Save the current state of every door in the current world
//	..off to an open file.
/* ------------------------------------------------------------------------------------ */
int CChangeLevel::SaveTo(FILE *SaveFD, bool type)
{
	//	Ok, check to see if there are ChangeLevel in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "ChangeLevel");

	if(!pSet)
		return RGF_SUCCESS;									// No doors, whatever...

	geEntity *pEntity;

	//	Ok, we have doors somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		ChangeLevel *pItem = static_cast<ChangeLevel*>(geEntity_GetUserData(pEntity));

		WRITEDATA(type, &pItem->CallBack,		sizeof(geBoolean),	1, SaveFD);
		WRITEDATA(type, &pItem->CallBackCount,	sizeof(int),		1, SaveFD);
	}

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	RestoreFrom
//
//	Restore the state of every ChangeLevel in the current world from an
//	..open file.
/* ------------------------------------------------------------------------------------ */
int CChangeLevel::RestoreFrom(FILE *RestoreFD, bool type)
{
	//	Ok, check to see if there are ChangeLevel in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "ChangeLevel");

	if(!pSet)
		return RGF_SUCCESS;									// No doors, whatever...

	geEntity *pEntity;

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		ChangeLevel *pItem = static_cast<ChangeLevel*>(geEntity_GetUserData(pEntity));

		READDATA(type, &pItem->CallBack,		sizeof(geBoolean),	1, RestoreFD);
		READDATA(type, &pItem->CallBackCount,	sizeof(int),		1, RestoreFD);
	}

	return RGF_SUCCESS;
}

/* ----------------------------------- END OF FILE ------------------------------------ */
