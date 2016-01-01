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
CViewSwitch::CViewSwitch() :
	m_OldView(0),
	m_ViewActive(false),
	m_pViewSwitch(NULL)
{
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
	if(m_ViewActive)
	{
		if(!GetTriggerState(m_pViewSwitch->TriggerName))
		{
			if(m_pViewSwitch->Restore == GE_TRUE)
			{
				// FIRST restore point of view, THEN restore saved values
				CCD->Player()->SwitchCamera(m_OldView);
				CCD->CameraManager()->RestoreFromS();
			}

			m_ViewActive = false;
			m_pViewSwitch = NULL;
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
				m_ViewActive = true;
				m_pViewSwitch = pSource;

				if(m_pViewSwitch->Restore == GE_TRUE)
				{
					m_OldView = CCD->Player()->GetViewPoint();
					CCD->CameraManager()->SaveToS();
				}

				CCD->Player()->SwitchCamera(pSource->LevelView);
				return;
			}
		}
	}
}

/* ----------------------------------- END OF FILE ------------------------------------ */
