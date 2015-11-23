/*

    CActMaterial.cpp               CActMaterial handler

	(c) 2002 Ralph Deane

	This file contains the class implementation for the 
	CActMaterial enitity  for RGF-based games.

	Programming provided by QuestOfDreams
*/

//        Include the One True Header

#include "RabidFramework.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CActMaterial::CActMaterial()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	//           Ok, check to see if there are MaterialChanges in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "ActMaterial");
	
	Bottom = Top = NULL;

	if(!pSet) 
		return; 
	
	//          Ok, we have Messages somewhere.  Dig through 'em all.
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		ActMaterial *pMaterial = (ActMaterial*)geEntity_GetUserData(pEntity);
		
		if(EffectC_IsStringNull(pMaterial->szEntityName))
		{
			char szName[128];
			geEntity_GetName(pEntity, szName, 128);
			pMaterial->szEntityName = szName;
		}
		// Ok, put this entity into the Global Entity Registry
		CCD->EntityRegistry()->AddEntity(pMaterial->szEntityName, "ActMaterial");
		
		pMaterial->active = false;
	
		pMaterial->Actor = NULL;
		if(!EffectC_IsStringNull(pMaterial->EntityName))
			pMaterial->Actor = GetEntityActor(pMaterial->EntityName);
			
		if(pMaterial->Actor == NULL)
		{
			char szError[256];
			sprintf(szError,"ActMaterial : Error can't find actor");
			CCD->ReportError(szError, false);
			CCD->ShutdownLevel();
			delete CCD;
			MessageBox(NULL, szError,"ActMaterial", MB_OK);
			exit(-333);
		}
	}
}

CActMaterial::~CActMaterial()
{
	if(Bottom)
	{
		MaterialList *pool, *temp;
		
		pool = Bottom;
		while	(pool!= NULL)
		{
			temp = pool->next;
			delete pool;
			pool = temp;
		}
		Bottom = NULL;
	}
}

void CActMaterial::Tick(float dwTicks)
{
	
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	pSet = geWorld_GetEntitySet(CCD->World(), "ActMaterial");
	
	if(!pSet) 
		return;         // Not on this level.
	
	//          Ok, we have ActorMaterial changes somewhere.  Dig through 'em all.
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		ActMaterial *pMaterial = (ActMaterial*)geEntity_GetUserData(pEntity);
		
		if(EffectC_IsStringNull(pMaterial->ChangeMaterial))
			continue;

		if(!EffectC_IsStringNull(pMaterial->TriggerName))
		{
			if(GetTriggerState(pMaterial->TriggerName))
			{
				if(pMaterial->active == false)
				{
					pMaterial->active = true;
				}
			}
			else
			{
				if(pMaterial->active == GE_TRUE)
				{
					pMaterial->active = false;
				}
			}
		}
		else
		{
			if(pMaterial->active == GE_FALSE)
			{
				pMaterial->active = true;
			}
		}
		
		if(pMaterial->active == GE_TRUE)
		{
			MaterialList *pool;
			pool = new MaterialList;
			pool->next = NULL;
			pool->prev = Top;
			if(!Bottom)
				Bottom = pool;
			Top = pool;
			if(pool->prev)
				pool->prev->next = pool;
			strcpy(pool->Entity, pMaterial->EntityName);
			strcpy(pool->Material, pMaterial->ChangeMaterial);
			pool->ChangeLighting = pMaterial->ChangeLighting;
			CCD->ActorManager()->ChangeMaterial(pMaterial->Actor, pMaterial->ChangeMaterial);
			if(pMaterial->ChangeLighting)
			{
				pool->FillColor = pMaterial->FillColor;
				pool->AmbientColor = pMaterial->AmbientColor;
				CCD->ActorManager()->SetActorDynamicLighting(pMaterial->Actor, pMaterial->FillColor, pMaterial->AmbientColor);
			}
		}
	}
	return; 
} 


//	SaveTo
//
//	Save the current state of every ActMaterial in the current world
//	..off to an open file.

int CActMaterial::SaveTo(FILE *SaveFD, bool type)
{
	bool Any = false;
	if(Bottom)
		Any = true;
	WRITEDATA(&Any, sizeof(bool), 1, SaveFD);
	if(Any)
	{
		MaterialList *pool, *temp;
		int count = 0;
		
		pool = Bottom;
		while	(pool!= NULL)
		{
			temp = pool->next;
			count += 1;
			pool = temp;
		}
		WRITEDATA(&count, sizeof(int), 1, SaveFD);
		pool = Bottom;
		while	(pool!= NULL)
		{
			temp = pool->next;
			WRITEDATA(pool->Entity, sizeof(char), 64, SaveFD);
			WRITEDATA(pool->Material, sizeof(char), 64, SaveFD);
			WRITEDATA(&pool->ChangeLighting, sizeof(bool), 1, SaveFD);
			WRITEDATA(&pool->AmbientColor, sizeof(GE_RGBA), 1, SaveFD);
			WRITEDATA(&pool->FillColor, sizeof(GE_RGBA), 1, SaveFD);
			pool = temp;
		}
	}

	return RGF_SUCCESS;
}

//	RestoreFrom
//
//	Restore the state of every ActMaterial in the current world from an
//	..open file.

int CActMaterial::RestoreFrom(FILE *RestoreFD, bool type)
{
	bool Any;
	int count;

	Bottom = Top = NULL;

	READDATA(&Any, sizeof(bool), 1, RestoreFD);
	if(Any)
	{
		READDATA(&count, sizeof(int), 1, RestoreFD);
		for(int i=0;i<count;i++)
		{
			MaterialList *pool;
			pool = new MaterialList;
			pool->next = NULL;
			pool->prev = Top;
			if(!Bottom)
				Bottom = pool;
			Top = pool;
			if(pool->prev)
				pool->prev->next = pool;
			READDATA(pool->Entity, sizeof(char), 64, RestoreFD);
			READDATA(pool->Material, sizeof(char), 64, RestoreFD);
			READDATA(&pool->ChangeLighting, sizeof(bool), 1, RestoreFD);
			READDATA(&pool->AmbientColor, sizeof(GE_RGBA), 1, RestoreFD);
			READDATA(&pool->FillColor, sizeof(GE_RGBA), 1, RestoreFD);
			geActor *Actor = GetEntityActor(pool->Entity);
			CCD->ActorManager()->ChangeMaterial(Actor, pool->Material);
			if(pool->ChangeLighting)
				CCD->ActorManager()->SetActorDynamicLighting(Actor, pool->FillColor, pool->AmbientColor);
		}
	}

	return RGF_SUCCESS;
}
