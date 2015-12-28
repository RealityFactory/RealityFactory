/************************************************************************************//**
 * @file CutScene.cpp
 * @brief CutScene handler
 *
 * This file contains the class implementation for the CutScene entity for
 * RGF-based games.
 * @author Daniel Queteschiner
 *//*
 * Copyright (c) 2002 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#include "RabidFramework.h"
#include "CutScene.h"

/* ------------------------------------------------------------------------------------ */
// Constructor
/* ------------------------------------------------------------------------------------ */
CCutScene::CCutScene()
{
	// Ok, check to see if there are Messages in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "CutScene");

	if(!pSet)
		return;

	geEntity *pEntity;

	// Ok, we have Messages somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		CutScene *pCutScene = static_cast<CutScene*>(geEntity_GetUserData(pEntity));

		if(!EffectC_IsStringNull(pCutScene->szCutScene))
			continue;

		if(EffectC_IsStringNull(pCutScene->szEntityName))
		{
			char szName[128];
			geEntity_GetName(pEntity, szName, 128);
			pCutScene->szEntityName = szName;
		}

		// Ok, put this entity into the Global Entity Registry
		CCD->EntityRegistry()->AddEntity(pCutScene->szEntityName, "CutScene");

		pCutScene->played = GE_FALSE;
		pCutScene->active = GE_FALSE;
		pCutScene->triggeron = GE_FALSE;
	}
}

/* ------------------------------------------------------------------------------------ */
// Destructor
/* ------------------------------------------------------------------------------------ */
CCutScene::~CCutScene()
{
}

/* ------------------------------------------------------------------------------------ */
// Tick
/* ------------------------------------------------------------------------------------ */
void CCutScene::Tick(geFloat /*dwTicks*/)
{
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "CutScene");

	if(!pSet)
		return;		// Not on this level.

	geEntity *pEntity;

	// Ok, we have CutScenes somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		CutScene *pCutScene = static_cast<CutScene*>(geEntity_GetUserData(pEntity));

		if(pCutScene->played)
			continue;

		if(!EffectC_IsStringNull(pCutScene->TriggerName))
		{
			if(GetTriggerState(pCutScene->TriggerName))
			{
				if(pCutScene->active == GE_FALSE)
				{
					pCutScene->active = GE_TRUE;
				}
				// prevent looping of the file when trigger is on
				else
					pCutScene->triggeron = GE_TRUE;
			}
			else
			{
				if(pCutScene->active == GE_TRUE)
				{
					pCutScene->active = GE_FALSE;
					pCutScene->triggeron = GE_FALSE;
				}
			}

			if(pCutScene->triggeron == GE_TRUE)
				continue;
		}
		else
		{
			if(pCutScene->active == GE_FALSE)
			{
				pCutScene->active = GE_TRUE;
				pCutScene->played = GE_TRUE;
			}
		}

		if(pCutScene->active == GE_TRUE)
		{
			CCD->Player()->DisableFog();		// Turn off fogging for cut scene
			CCD->Player()->DisableClipPlane();	// Turn off the clipping plane as well

			// Play the cut scene
			CCD->Play(pCutScene->szCutScene, pCutScene->XPos, pCutScene->YPos, pCutScene->Center);

			CCD->Player()->ShowFog();			// Show fog, if enabled
			CCD->Player()->ActivateClipPlane();	//  Activate clipping plane, if enabled

			CCD->Engine()->ResetSystem();
		}
	}

	return;
}


/* ----------------------------------- END OF FILE ------------------------------------ */
