/****************************************************************************************/
/*																						*/
/*	CViewSwitch.cpp:		ViewSwitch class implementation								*/
/*																						*/
/*	(c) 2000 Ralph Deane																*/
/*	All Rights Reserved																	*/
/*																						*/
/*	This file contains the class implementation for ViewSwitch handling.				*/
/*																						*/
/****************************************************************************************/

//	Include the One True Header
#include "RabidFramework.h"

/* ------------------------------------------------------------------------------------ */
//	Constructor
//
//	Load up all ViewSwitchs and set the entity values.
/* ------------------------------------------------------------------------------------ */
CViewSwitch::CViewSwitch()
{
	ViewActive = false;
	pViewSwitch = NULL;

// changed QD 12/15/05 - code does nothing
/*	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	// Ok, check to see if there are FixedCameras in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "ViewSwitch");

	if(!pSet)
		return;

	// Ok, we have ViewSwitchs somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	    pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		ViewSwitch *pSource = (ViewSwitch*)geEntity_GetUserData(pEntity);
	}

	return;
*/
// end change
}

/* ------------------------------------------------------------------------------------ */
//	Destructor
//
//	Clean up.
/* ------------------------------------------------------------------------------------ */
CViewSwitch::~CViewSwitch()
{

}

/* ------------------------------------------------------------------------------------ */
// Tick
/* ------------------------------------------------------------------------------------ */
void CViewSwitch::Tick()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

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

	pSet = geWorld_GetEntitySet(CCD->World(), "ViewSwitch");

	if(!pSet)
		return;

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		ViewSwitch *pSource = (ViewSwitch*)geEntity_GetUserData(pEntity);

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
