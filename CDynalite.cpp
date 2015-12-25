/************************************************************************************//**
 * @file CDynalite.cpp
 * @brief Dynamic Light handler
 *
 * This file contains the class implementation for the CDynalite class that
 * encapsulates all dynamic point light effects for RGF-based games.
 * @author Ralph Deane
 *//*
 * Copyright (c) 1999 Ralph Deane; All rights reserved.
 ****************************************************************************************/

//	Include the One True Header
#include "RabidFramework.h"
#include "CDynalite.h"

/* ------------------------------------------------------------------------------------ */
//	Constructor
/* ------------------------------------------------------------------------------------ */
CDynalite::CDynalite()
{
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "DynamicLight");

	if(!pSet)
		return;						// None there.

	geEntity *pEntity;

	// Ok, we have dynamic lights somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		DynamicLight *pLight = static_cast<DynamicLight*>(geEntity_GetUserData(pEntity));
		pLight->RadiusSpeed *= 1000.0f;			// From SECONDS to MILLISECONDS

		if(EffectC_IsStringNull(pLight->szEntityName))
		{
			char szName[128];
			geEntity_GetName(pEntity, szName, 128);
			pLight->szEntityName = szName;
		}

		// Ok, put this entity into the Global Entity Registry
		CCD->EntityRegistry()->AddEntity(pLight->szEntityName, "DynamicLight");
		// Added next if block to calculate the Origin offset.  We pass it to
		// SetOrigin later in the tick function so that the light will stay in
		// the same position relative to the model.  The offset is just the
		// difference of the Dynamic Light's origin vector and the Model's
		// origin vector.
		pLight->OriginOffset = pLight->origin;

		if(pLight->Model)
		{
			geVec3d ModelOrigin;
			geWorld_GetModelRotationalCenter(CCD->World(), pLight->Model, &ModelOrigin);
			geVec3d_Subtract(&(pLight->origin), &ModelOrigin, &(pLight->OriginOffset));
		}

		pLight->active = GE_FALSE;
		pLight->NumFunctionValues = strlen(pLight->RadiusFunction);

		if(pLight->NumFunctionValues == 0)
		{
			CCD->ReportError("[WARNING] DynamicLight has no RadiusFunction string\n", false);
			continue;
		}

		pLight->IntervalWidth = pLight->RadiusSpeed / static_cast<geFloat>(pLight->NumFunctionValues);
	}

	return;
}

/* ------------------------------------------------------------------------------------ */
//	Destructor
/* ------------------------------------------------------------------------------------ */
CDynalite::~CDynalite()
{
	return;
}

/* ------------------------------------------------------------------------------------ */
//	Tick
/* ------------------------------------------------------------------------------------ */
geBoolean CDynalite::Tick(geFloat dwTicks)
{
	if(CCD->World() == NULL)
		return GE_TRUE;

	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "DynamicLight");

	if(!pSet)
		return GE_TRUE;

	geEntity		*pEntity;

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		DynamicLight *Light = static_cast<DynamicLight*>(geEntity_GetUserData(pEntity));

		if(!EffectC_IsStringNull(Light->TriggerName))
		{
			if(GetTriggerState(Light->TriggerName))
			{
				if(Light->active == GE_FALSE)
				{
					Light->DynLight = geWorld_AddLight(CCD->World());
					Light->active = GE_TRUE;
				}
			}
			else
			{
				if(Light->active == GE_TRUE)
					geWorld_RemoveLight(CCD->World(), Light->DynLight );

				Light->active = GE_FALSE;
			}
		}
		else
		{
			if(Light->active == GE_FALSE)
			{
				Light->DynLight = geWorld_AddLight(CCD->World());
				Light->active = GE_TRUE;
			}
		}

		if(Light->active == GE_TRUE)
		{
			// Added next line of code to pass the OriginOffset to SetOrigin
			// so that the light will stay in the same position relative to the model.
			// Also we now call SetOriginOffset instead of SetOffset in the line after.
			int32 Leaf;
			Light->origin = Light->OriginOffset;
			SetOriginOffset(Light->EntityName, Light->BoneName, Light->Model, &(Light->origin));
			geWorld_GetLeaf(CCD->World(), &(Light->origin), &Leaf);

			geFloat Percentage = Light->LastTime / Light->RadiusSpeed;

			int Index = static_cast<int>(Percentage * Light->NumFunctionValues);

			if(Light->InterpolateValues && Index < Light->NumFunctionValues - 1)
			{
				geFloat	Remainder;
				geFloat	InterpolationPercentage;
				int		DeltaValue;
				geFloat	Value;

				Remainder = fmod(Light->LastTime, Light->IntervalWidth);
				InterpolationPercentage = Remainder / Light->IntervalWidth;
				DeltaValue = Light->RadiusFunction[Index + 1] - Light->RadiusFunction[Index];
				Value = Light->RadiusFunction[Index] + DeltaValue * InterpolationPercentage;
				Percentage = (Value - static_cast<geFloat>('a')) / (static_cast<geFloat>('z' - 'a'));
			}
			else
				Percentage = (static_cast<geFloat>(Light->RadiusFunction[Index] - 'a')) / (static_cast<geFloat>('z' - 'a'));

			geFloat Radius = Percentage * (Light->MaxRadius - Light->MinRadius) + Light->MinRadius;
			geVec3d Pos = Light->origin;

			geWorld_SetLightAttributes(CCD->World(),
									   Light->DynLight,
									   &Pos,
									   &(Light->Color),
									   Radius,
									   GE_TRUE);

			Light->LastTime = fmod(Light->LastTime + dwTicks, Light->RadiusSpeed);

			if(EffectC_IsPointVisible(CCD->World(),
					CCD->CameraManager()->Camera(),
					&Pos,
					Leaf,
					EFFECTC_CLIP_LEAF ) == GE_FALSE)
			{
				geWorld_RemoveLight(CCD->World(), Light->DynLight);
				Light->active = GE_FALSE;
			}
		}
	}

	return GE_TRUE;
}

//	******************** CRGF Overrides ********************

/* ------------------------------------------------------------------------------------ */
//	LocateEntity
//
//	Given a name, locate the desired item in the currently loaded level
//	..and return it's user data.
/* ------------------------------------------------------------------------------------ */
int CDynalite::LocateEntity(const char *szName, void **pEntityData)
{
	// Ok, check to see if there are dynamic lights in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "DynamicLight");

	if(!pSet)
		return RGF_NOT_FOUND;							// No dynamic lights

	geEntity *pEntity;

	// Ok, we have dynamic lights.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		DynamicLight *pTheEntity = static_cast<DynamicLight*>(geEntity_GetUserData(pEntity));

		if(!strcmp(pTheEntity->szEntityName, szName))
		{
			*pEntityData = static_cast<void*>(pTheEntity);
			return RGF_SUCCESS;
		}
	}

	return RGF_NOT_FOUND;								// Sorry, no such entity here
}

/* ------------------------------------------------------------------------------------ */
//	ReSynchronize
//
//	Correct internal timing to match current time, to make up for time lost
//	..when outside the game loop (typically in "menu mode").
/* ------------------------------------------------------------------------------------ */
int CDynalite::ReSynchronize()
{
	return RGF_SUCCESS;
}

/* ----------------------------------- END OF FILE ------------------------------------ */
