/*

    CCutScene.cpp               CutScene handler

	(c) 2002 Ralph Deane

	This file contains the class implementation for the 
	CutScene enitity  for RGF-based games.

	Programming provided by QuestOfDreams
*/

//           Include the One True Header

#include "RabidFramework.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CCutScene::CCutScene()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	//      Ok, check to see if there are Messages in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "CutScene");
	
	if(!pSet) 
		return; 
	
	//      Ok, we have Messages somewhere.  Dig through 'em all.
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity; pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		CutScene *pCutScene = (CutScene*)geEntity_GetUserData(pEntity);
		if(!EffectC_IsStringNull(pCutScene->szCutScene))
			continue;
		if(EffectC_IsStringNull(pCutScene->szEntityName))
		{
			char szName[128];
			geEntity_GetName(pEntity, szName, 128);
			pCutScene->szEntityName = szName;
		}
		//      Ok, put this entity into the Global Entity Registry
		CCD->EntityRegistry()->AddEntity(pCutScene->szEntityName, "CutScene");
		
		pCutScene->played = false;
		pCutScene->active = false;
		pCutScene->triggeron= false;
	}
}


CCutScene::~CCutScene()
{
	
}

void CCutScene::Tick(float dwTicks)
{
	
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	pSet = geWorld_GetEntitySet(CCD->World(), "CutScene");
	
	if(!pSet) 
		return;   // Not on this level.
	
	//      Ok, we have CutScenes somewhere.  Dig through 'em all.
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity; pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		CutScene *pCutScene = (CutScene*)geEntity_GetUserData(pEntity);
		
		if(pCutScene->played)
			continue;
		
		if(!EffectC_IsStringNull(pCutScene->TriggerName))
		{
			if(GetTriggerState(pCutScene->TriggerName))
			{
				if(pCutScene->active == GE_FALSE)
				{
					pCutScene->active = true;
				}
				//        prevent looping of the file when trigger is on
				else
					pCutScene->triggeron = true;
			}
			else
			{
				if(pCutScene->active == GE_TRUE)
				{
					pCutScene->active = false;
					pCutScene->triggeron = false;
				}
			}
			
			if(pCutScene->triggeron == GE_TRUE)
				continue;
		}
		else
		{
			if(pCutScene->active == GE_FALSE)
			{
				pCutScene->active = true;
				pCutScene->played = true;
			}
		}
		
		if(pCutScene->active == GE_TRUE)
		{
			CCD->Player()->DisableFog();        // Turn off fogging for cut scene
			CCD->Player()->DisableClipPlane();        // Turn off the clipping plane as well
			
			//        Play the cut scene
			CCD->Play(pCutScene->szCutScene, pCutScene->XPos, pCutScene->YPos, pCutScene->Center);
			
			CCD->Player()->ShowFog();        //  Show fog, if enabled
			CCD->Player()->ActivateClipPlane();        //  Activate clipping plane, if enabled
			
			CCD->ResetClock();
			CCD->Engine()->BeginFrame();
			CCD->Engine()->EndFrame();
			CCD->ResetClock();
		}
	}
	return;
} 