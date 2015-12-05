/************************************************************************************//**
 * @file CShake.cpp
 * @brief Screen Shake handler
 *
 * This file contains the class implementation for the Screen Shake enitity
 * for RGF-based games.
 * @author Ralph Deane
 *//*
 * Copyright (c) 1999 Ralph Deane; All rights reserved.
 ****************************************************************************************/

// Include the One True Header
#include "RabidFramework.h"

/* ------------------------------------------------------------------------------------ */
//	Constructor
/* ------------------------------------------------------------------------------------ */
CShake::CShake()
{
	geEntity *pEntity;

	// Ok, see if we have any ScreenShake entities at all
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "ScreenShake");

	if(!pSet)
		return;

	// Look through all of our ScreenShakes
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		ScreenShake *pSource = static_cast<ScreenShake*>(geEntity_GetUserData(pEntity));

		pSource->OriginOffset = pSource->origin;

		if(pSource->Model)
		{
			geVec3d ModelOrigin;
	    	geWorld_GetModelRotationalCenter(CCD->World(), pSource->Model, &ModelOrigin);
			geVec3d_Subtract(&(pSource->origin), &ModelOrigin, &(pSource->OriginOffset));
  		}

		pSource->active = GE_FALSE;
	}
}

/* ------------------------------------------------------------------------------------ */
//	Destructor
/* ------------------------------------------------------------------------------------ */
CShake::~CShake()
{
}

/* ------------------------------------------------------------------------------------ */
//	Tick
/* ------------------------------------------------------------------------------------ */
void CShake::Tick(geFloat dwTicks)
{
	geEntity *pEntity;

	// Ok, check to see if there are ScreenShakes in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "ScreenShake");

	if(!pSet)
		return;

	// Ok, we have ScreenShakes somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		ScreenShake *pSource = static_cast<ScreenShake*>(geEntity_GetUserData(pEntity));

		pSource->origin = pSource->OriginOffset;
		SetOriginOffset(pSource->EntityName, pSource->BoneName, pSource->Model, &(pSource->origin));

		if(!EffectC_IsStringNull(pSource->TriggerName))
		{
			if(GetTriggerState(pSource->TriggerName))
			{
				if(pSource->active == GE_FALSE)
				{
					if(pSource->ShakeAmt > 0.0f)
						CCD->CameraManager()->SetShake(pSource->ShakeAmt, pSource->ShakeDecay, pSource->origin);

					pSource->active = GE_TRUE;
				}
			}
			else
			{
				if(pSource->active == GE_TRUE)
				{
					pSource->active = GE_FALSE;
				}
			}
		}
		else
		{
			if(pSource->active == GE_FALSE)
			{
				pSource->active = GE_TRUE;

				if(pSource->ShakeAmt > 0.0f)
					CCD->CameraManager()->SetShake(pSource->ShakeAmt, pSource->ShakeDecay, pSource->origin);
			}
		}
	}
}

/* ----------------------------------- END OF FILE ------------------------------------ */
