/************************************************************************************//**
 * @file CViewSwitch.cpp
 * @brief ViewSwitch class implementation
 *
 * This file contains the class implementation for ViewSwitch handling.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2000 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#include "RabidFramework.h"
#include "CViewSwitch.h"

/* ------------------------------------------------------------------------------------ */
// Constructor
//
// Load up all ViewSwitchs and set the entity values.
/* ------------------------------------------------------------------------------------ */
CViewSwitch::CViewSwitch()
{
	ViewActive = false;
	pViewSwitch = NULL;
}

/* ------------------------------------------------------------------------------------ */
// Destructor
//
// Clean up.
/* ------------------------------------------------------------------------------------ */
CViewSwitch::~CViewSwitch()
{
}

/* ------------------------------------------------------------------------------------ */
// Tick
/* ------------------------------------------------------------------------------------ */
void CViewSwitch::Tick()
{
	if(ViewActive)
	{
		if(!GetTriggerState(pViewSwitch->TriggerName))
		{
			if(pViewSwitch->Restore == GE_TRUE)
			{
				CCD->CameraManager()->RestoreFromS();
				CCD->Player()->SwitchCamera(OldView);
			}

			ViewActive = false;
			pViewSwitch = NULL;
		}

		return;
	}

	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "ViewSwitch");

	if(!pSet)
		return;

	geEntity *pEntity;

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		ViewSwitch *pSource = static_cast<ViewSwitch*>(geEntity_GetUserData(pEntity));

		if(!EffectC_IsStringNull(pSource->TriggerName))
		{
			if(GetTriggerState(pSource->TriggerName))
			{
				ViewActive = true;
				pViewSwitch = pSource;

				if(pViewSwitch->Restore == GE_TRUE)
				{
					OldView = CCD->Player()->GetViewPoint();
					CCD->CameraManager()->SaveToS();
				}

				CCD->Player()->SwitchCamera(pSource->LevelView);
				return;
			}
		}
	}
}

/* ----------------------------------- END OF FILE ------------------------------------ */
