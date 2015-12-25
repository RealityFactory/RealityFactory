/************************************************************************************//**
 * @file CFloating.cpp
 * @brief Floating Particle handler
 *
 * This file contains the class implementation for the CFloat class that
 * encapsulates floating particle based special effects for RGF-based games.
 * @author Ralph Deane
 *//*
 * Copyright (c) 1999 Ralph Deane; All rights reserved.
 ****************************************************************************************/

//	Include the One True Header
#include "RabidFramework.h"
#include "CFloating.h"

extern geBitmap *TPool_Bitmap(const char *DefaultBmp, const char *DefaultAlpha,
							  const char *BName, const char *AName);

/* ------------------------------------------------------------------------------------ */
//	CFloat Constructor
/* ------------------------------------------------------------------------------------ */
CFloat::CFloat()
{
	// test for any floating particle entities
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "FloatingParticles");

	// there is none
	if(!pSet)
		return;

	geEntity *pEntity;

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		// get data
		FloatingParticles *S = static_cast<FloatingParticles*>(geEntity_GetUserData(pEntity));

		if(EffectC_IsStringNull(S->szEntityName))
		{
			char szName[128];
			geEntity_GetName(pEntity, szName, 128);
			S->szEntityName = szName;
		}

		// Ok, put this entity into the Global Entity Registry
		CCD->EntityRegistry()->AddEntity(S->szEntityName, "FloatingParticles");
		S->OriginOffset = S->origin;

		if(S->Model)
		{
			geVec3d ModelOrigin;
			geWorld_GetModelRotationalCenter(CCD->World(), S->Model, &ModelOrigin);
			geVec3d_Subtract(&S->origin, &ModelOrigin, &S->OriginOffset);
  		}

	    // get total number of sprites to use
		S->EffectCount = S->ParticleCount;

	    // create dynamic array to hold index numbers
		S->EffectList = (int*)malloc(sizeof(int) * S->EffectCount);
		// create dynamic xform array
		S->Xf = (geXForm3d*)calloc(S->ParticleCount * sizeof(*(S->Xf)) ,1);
		// create dynamic speed array
		S->Speed = (float*)calloc(S->ParticleCount * sizeof(*(S->Speed)) ,1);

		// get the bitmap used as the sprite
		// g_bubble/A__bubble are default bitmap names
		S->Bitmap = TPool_Bitmap("g_bubble.bmp", "a_bubble.bmp", S->BmpName, S->AlphaName);

		if(S->Color.r < 0.0f)
			S->Color.r = 0.0f;
		if(S->Color.g < 0.0f)
			S->Color.g = 0.0f;
		if(S->Color.b < 0.0f)
			S->Color.b = 0.0f;

		// clear out sprite data
		Sprite Spr;
		memset(&Spr, 0, sizeof(Spr));

		Spr.Texture = &(S->Bitmap);
		Spr.Color.r = S->Color.r;
		Spr.Color.g = S->Color.g;
		Spr.Color.b = S->Color.b;
		Spr.Color.a = 255.0f;
		Spr.Pause	= GE_TRUE;
		// only one sprite so don't cycle at all
		Spr.TotalTextures = 1;
		Spr.Style = SPRITE_CYCLE_NONE;
		// set sprite scale
		Spr.Scale = S->Scale;

		S->active = GE_FALSE;

		// set sprite position
		S->origin = S->OriginOffset;
		SetOriginOffset(S->EntityName, S->BoneName, S->Model, &(S->origin));
		geVec3d_Copy(&(S->origin), &(S->BasePos));

		// do for required number of sprites
		for(int i=0; i<S->EffectCount; ++i)
		{
			// set random direction
			// changed QD 12/15/05
			//geXForm3d_SetIdentity(&(S->Xf[i]));
			//geXForm3d_RotateX(&(S->Xf[i]), EffectC_Frand(-S->XSlant, S->XSlant));
			geXForm3d_SetXRotation(&(S->Xf[i]), EffectC_Frand(-S->XSlant, S->XSlant));
			// end change
			geXForm3d_RotateZ(&(S->Xf[i]), EffectC_Frand(-S->ZSlant, S->ZSlant));

			// pick random start spot
			geVec3d_Copy(&(S->BasePos), &(S->Xf[i].Translation));
			S->Xf[i].Translation.X += EffectC_Frand(-(S->Radius*0.5f), S->Radius*0.5f);
			S->Xf[i].Translation.Z += EffectC_Frand(-(S->Radius*0.5f), S->Radius*0.5f);
			geVec3d_Copy(&(S->Xf[i].Translation), &(Spr.Pos));

			// set speed
			S->Speed[i] = EffectC_Frand(S->MinSpeed, S->MaxSpeed);

			// add effect to manager
			S->EffectList[i] = CCD->EffectManager()->Item_Add(EFF_SPRITE, static_cast<void*>(&Spr));
		}
	}

	return;
}

/* ------------------------------------------------------------------------------------ */
//	CFloat Destructor
/* ------------------------------------------------------------------------------------ */
CFloat::~CFloat()
{
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "FloatingParticles");

	if(!pSet)
		return;

	geEntity *pEntity;

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		FloatingParticles *R = static_cast<FloatingParticles*>(geEntity_GetUserData(pEntity));

		// free any dynamic arrays
		if(R->EffectList != NULL)	// changed QD 12/15/05
			free(R->EffectList);

		if(R->Xf != NULL)
			free(R->Xf);

		if(R->Speed != NULL)
			free(R->Speed);
	}

	return;
}

/* ------------------------------------------------------------------------------------ */
//	Tick
/* ------------------------------------------------------------------------------------ */
void CFloat::Tick(geFloat dwTicks)
{
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "FloatingParticles");

	if(!pSet)
		return;

	// get amount of time since last call
	dwTicks *= 0.001f;

	geEntity *pEntity;

	// wade thru all entities
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		FloatingParticles *S;

		geVec3d Movement;
		geVec3d  Pos;
		geVec3d  Offset;
		Sprite  Spr;

		// get data
		S = static_cast<FloatingParticles*>(geEntity_GetUserData(pEntity));

		if(!EffectC_IsStringNull(S->TriggerName))
			S->active = GetTriggerState(S->TriggerName);
		else
			S->active = GE_TRUE;

		S->origin = S->OriginOffset;
		SetOriginOffset(S->EntityName, S->BoneName, S->Model, &(S->origin));
		geVec3d_Subtract( &(S->origin), &( S->BasePos ), &Movement );
		geVec3d_Copy( &(S->origin), &( S->BasePos ) );

		// modify all sprites
		for(int i=0; i<S->EffectCount; ++i)
		{
			if(S->active == GE_TRUE)
			{
				// adjust this particles orientation
				geVec3d_Copy(&(S->Xf[i].Translation), &Pos);
				geVec3d_Set(&(S->Xf[i].Translation), 0.0f, 0.0f, 0.0f);
				geXForm3d_RotateY(&(S->Xf[i]), dwTicks*5.0f);
				geVec3d_Copy(&Pos, &(S->Xf[i].Translation));

				// adjust particle position
				geXForm3d_GetUp(&(S->Xf[i]), &Offset);
				geVec3d_AddScaled(&(S->Xf[i].Translation), &Offset, dwTicks * S->Speed[i], &(S->Xf[i].Translation));
				geVec3d_Add(&(S->Xf[i].Translation), &Movement, &(S->Xf[i].Translation));
				geVec3d_Copy(&(S->Xf[i].Translation), &(Spr.Pos));

				// reset particle if it has hit its height limit
// changed RF064
				if(fabs(S->Xf[i].Translation.Y - S->BasePos.Y) > S->Height)
// end change RF064
				{
  					// set random direction
					// changed QD 12/15/05
					//geXForm3d_SetIdentity(&(S->Xf[i]));
					//geXForm3d_RotateX(&(S->Xf[i]), EffectC_Frand(-S->XSlant, S->XSlant));
					geXForm3d_SetXRotation(&(S->Xf[i]), EffectC_Frand(-S->XSlant, S->XSlant));
					// end change
					geXForm3d_RotateZ(&(S->Xf[i]), EffectC_Frand(-S->ZSlant, S->ZSlant));

					// pick random start spot
					geVec3d_Copy(&(S->BasePos), &(S->Xf[i].Translation));
					S->Xf[i].Translation.X += EffectC_Frand(-(S->Radius*0.5f), S->Radius*0.5f);
					S->Xf[i].Translation.Z += EffectC_Frand(-(S->Radius*0.5f), S->Radius*0.5f);
					geVec3d_Copy(&(S->Xf[i].Translation), &(Spr.Pos));

					// set speed
					S->Speed[i] = EffectC_Frand(S->MinSpeed, S->MaxSpeed);
				}

				// modify sprite in effect manager
				Spr.Pause = GE_FALSE;
				CCD->EffectManager()->Item_Modify(EFF_SPRITE, S->EffectList[i], static_cast<void*>(&Spr), SPRITE_POS | SPRITE_PAUSE);
			}
			else
			{
				Spr.Pause = GE_TRUE;
				// modify sprite in effect manager
				CCD->EffectManager()->Item_Modify(EFF_SPRITE, S->EffectList[i], static_cast<void*>(&Spr), SPRITE_PAUSE);
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
int CFloat::LocateEntity(const char *szName, void **pEntityData)
{
	// Ok, check to see if there are Floating Particles in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "FloatingParticles");

	if(!pSet)
		return RGF_NOT_FOUND;

	geEntity *pEntity;

	// Ok, we have Floating Particles somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	    pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		FloatingParticles *pSource = static_cast<FloatingParticles*>(geEntity_GetUserData(pEntity));

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
int CFloat::ReSynchronize()
{
	return RGF_SUCCESS;
}


/* ----------------------------------- END OF FILE ------------------------------------ */
