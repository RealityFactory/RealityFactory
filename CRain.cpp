/************************************************************************************//**
 * @file CRain.cpp
 * @brief Rain handler
 *
 * This file contains the class implementation for the CRain class that
 * encapsulates rain based special effects for RGF-based games.
 * @author Ralph Deane
 *//*
 * Copyright (c) 1999 Ralph Deane; All rights reserved.
 ****************************************************************************************/

// Include the One True Header
#include "RabidFramework.h"

#define RAINM_RADIUSTOEFFECTRATIO	50

extern geBitmap *TPool_Bitmap(const char *DefaultBmp, const char *DefaultAlpha,
							  const char *BName, const char *AName);

/* ------------------------------------------------------------------------------------ */
//	CRain Constructor
/* ------------------------------------------------------------------------------------ */
CRain::CRain()
{
	geEntity *pEntity;

	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "Rain");

	if(!pSet)
		return;

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Rain *R = static_cast<Rain*>(geEntity_GetUserData(pEntity));

		if(EffectC_IsStringNull(R->szEntityName))
		{
			char szName[128];
			geEntity_GetName(pEntity, szName, 128);
			R->szEntityName = szName;
		}

		// Ok, put this entity into the Global Entity Registry
		CCD->EntityRegistry()->AddEntity(R->szEntityName, "Rain");
		R->OriginOffset = R->origin;

		if(R->Model)
		{
			geVec3d ModelOrigin;
			geWorld_GetModelRotationalCenter(CCD->World(), R->Model, &ModelOrigin);
			geVec3d_Subtract(&(R->origin), &ModelOrigin, &(R->OriginOffset));
		}

		// calculate number of Sprays needed
		R->EffectCount = static_cast<int>(R->Radius) / RAINM_RADIUSTOEFFECTRATIO;

		// create dynamic array to hold effect indexes
		R->EffectList = (int*)malloc(sizeof(int)*R->EffectCount);

		// make sure data is valid
		if(R->Severity < 0.0f)
			R->Severity = 0.0f;
		else if(R->Severity > 1.0f)
			R->Severity = 1.0f;

		if(R->DropLife <= 0.0f)
			R->DropLife = 0.1f;

		R->active = GE_FALSE;

		TPool_Bitmap("rain.bmp", "a_rain.bmp", R->BmpName, R->AlphaName);
	}
}

/* ------------------------------------------------------------------------------------ */
//	Create
/* ------------------------------------------------------------------------------------ */
int CRain::Create(Rain *R)
{
	int effect = -1;
    Spray Sp;

    // clear out spray data
    memset(&Sp, 0, sizeof(Sp));

    // get bitmap for use as texture
    // rain/a_rain are default bitmap names
    Sp.Texture = TPool_Bitmap("rain.bmp", "a_rain.bmp", R->BmpName, R->AlphaName);

    // setup spray data
    Sp.MinScale		= 0.5f;
    Sp.MaxScale		= 1.5f;
	Sp.ShowAlways	= GE_TRUE;
    Sp.Rate			= (1.1f - R->Severity)*0.1f;
    geVec3d_Copy(&(R->Gravity), &(Sp.Gravity));

	// set source position
	geVec3d_Copy(&(R->origin), &(Sp.Source));
	Sp.SourceVariance	= static_cast<int>(R->Radius*0.5f);
	Sp.MinUnitLife		= R->DropLife;
	Sp.MaxUnitLife		= R->DropLife;

	// set destination position
	geVec3d_AddScaled(&(Sp.Source), &(Sp.Gravity), Sp.MinUnitLife, &(Sp.Dest));
	Sp.DestVariance = static_cast<int>(R->Radius*0.5f);

	memcpy(&(Sp.ColorMin), &(R->ColorMin), sizeof(Sp.ColorMin));
    memcpy(&(Sp.ColorMax), &(R->ColorMax), sizeof(Sp.ColorMax));

	Sp.ColorMin.a = Sp.ColorMax.a = 255.0f;

	Sp.UseWind = R->UseWind;

	effect = CCD->EffectManager()->Item_Add(EFF_SPRAY, static_cast<void*>(&Sp));
	return effect;
}

/* ------------------------------------------------------------------------------------ */
//	CRain  Destructor
/* ------------------------------------------------------------------------------------ */
CRain::~CRain()
{
	geEntity *pEntity;

	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "Rain");

	if(!pSet)
		return;

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Rain *R = static_cast<Rain*>(geEntity_GetUserData(pEntity));

		// free dynamic array
		if(R->EffectList)	// changed QD 07/15/06
			free(R->EffectList);
	}
}

/* ------------------------------------------------------------------------------------ */
//	Tick
/* ------------------------------------------------------------------------------------ */
void CRain::Tick(geFloat dwTicks)
{
	geEntity *pEntity;
	int i;
	Spray Sp;

	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "Rain");

	if(!pSet)
		return;

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Rain *R = static_cast<Rain*>(geEntity_GetUserData(pEntity));

		if(!EffectC_IsStringNull(R->TriggerName))
		{
			if(GetTriggerState(R->TriggerName))
			{
				if(R->active == GE_FALSE)
				{
					for(i=0; i<R->EffectCount; i++)
						R->EffectList[i] = Create(R);

					R->active = GE_TRUE;
				}
			}
			else
			{
				if(R->active == GE_TRUE)
				{
					for(i=0; i<R->EffectCount; i++)
						CCD->EffectManager()->Item_Delete(EFF_SPRAY, R->EffectList[i]);

					R->active = GE_FALSE;
				}
			}
		}
		else
		{
			if(R->active == GE_FALSE)
			{
				for(i=0; i<R->EffectCount; i++)
					R->EffectList[i] = Create(R);

				R->active = GE_TRUE;
			}
		}

		if(R->active == GE_TRUE)
		{
			R->origin = R->OriginOffset;

			if(SetOriginOffset(R->EntityName, R->BoneName, R->Model, &(R->origin)))
			{
	  			geVec3d_Copy(&(R->origin), &(Sp.Source));
				geVec3d_AddScaled(&(Sp.Source), &(Sp.Gravity), Sp.MinUnitLife, &(Sp.Dest));

				// adjust position
				for(int i=0; i<R->EffectCount; i++)
					CCD->EffectManager()->Item_Modify(EFF_SPRAY, R->EffectList[i], static_cast<void*>(&Sp), SPRAY_SOURCE | SPRAY_ACTUALDEST);
			}
		}
	}
}


//	******************** CRGF Overrides ********************

/* ------------------------------------------------------------------------------------ */
//	LocateEntity
//
//	Given a name, locate the desired item in the currently loaded level
//	..and return it's user data.
/* ------------------------------------------------------------------------------------ */
int CRain::LocateEntity(const char *szName, void **pEntityData)
{
	geEntity *pEntity;

	// Ok, check to see if there are Rain in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "Rain");

	if(!pSet)
		return RGF_NOT_FOUND;

	// Ok, we have Rain somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Rain *pSource = static_cast<Rain*>(geEntity_GetUserData(pEntity));

		if(!strcmp(pSource->szEntityName, szName))
		{
			*pEntityData = static_cast<void*>(pSource);
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
int CRain::ReSynchronize()
{
	return RGF_SUCCESS;
}


/* ----------------------------------- END OF FILE ------------------------------------ */
