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
		{ 
			
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
}

CActMaterial::~CActMaterial()
{
	
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
			
			CCD->ActorManager()->ChangeMaterial(pMaterial->Actor, pMaterial->ChangeMaterial);
			if(pMaterial->ChangeLighting)
				CCD->ActorManager()->SetActorDynamicLighting(pMaterial->Actor, pMaterial->FillColor, pMaterial->AmbientColor);
		}
	}
	return; 
} 