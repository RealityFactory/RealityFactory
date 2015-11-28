/****************************************************************************************/
/*																						*/
/*	CMorphingFields.cpp:		Morphing Fields class handler							*/
/*																						*/
/*	(c) 2001 Ralph Deane																*/
/*	All Rights Reserved																	*/
/*																						*/
/*	This file contains the class implementation for morphing fields						*/
/*	handling.																			*/
/*																						*/
/****************************************************************************************/

#include "RabidFramework.h"

extern geSound_Def *SPool_Sound(char *SName);

/* ------------------------------------------------------------------------------------ */
//	Constructor
//
//	Set all morphing fields to default values, load sounds, set up fog zones.
//	If there is audio, start it looping at zero volume.
/* ------------------------------------------------------------------------------------ */
CMorphingFields::CMorphingFields()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	m_MorphingFieldCount = 0;

	// Ok, check to see if there are morphing fields in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "MorphingField");

	if(!pSet)
		return;									// No morphing fields

	// Ok, we have morphing fields somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		MorphingField *pField = (MorphingField*)geEntity_GetUserData(pEntity);
		pField->bFollower = GE_FALSE;

		if(EffectC_IsStringNull(pField->szEntityName))
		{
			char szName[128];
			geEntity_GetName(pEntity, szName, 128);
			pField->szEntityName = szName;
		}

		m_MorphingFieldCount++;								// Kick field count

		// Ok, put this entity into the Global Entity Registry
		CCD->EntityRegistry()->AddEntity(pField->szEntityName, "MorphingField");

		// Reset all the data for each morphing field
		pField->theSound	= NULL;							// No sound right now
		pField->theFog		= NULL;							// No fog sphere...yet
		pField->bForward	= GE_TRUE;							// Animation direction
		pField->fRadius		= pField->fogRadiusStart;		// Start fog radius
		pField->tElapsed	= (geFloat)pField->fogSpeed;
		pField->clrCurrent	= pField->clrStart;				// Current color
		pField->SoundHandle = -1;							// No sound yet
		pField->active		= GE_FALSE;
		pField->theSound	= NULL;

		if(!EffectC_IsStringNull(pField->szSoundFile))
		{
			pField->theSound=SPool_Sound(pField->szSoundFile);

			if(!pField->theSound)
			{
				char szError[256];
				sprintf(szError, "*WARNING* File %s - Line %d: %s: Failed to open audio file '%s'\n",
						__FILE__, __LINE__, pField->szEntityName, pField->szSoundFile);
				CCD->ReportError(szError, false);
			}
		}

		// Ok, now we add a FOG SPHERE as our FIELD EFFECT.
		pField->fDensity = kFogDensity;					// Basic morphing field density
		pField->clrStart.a = pField->clrEnd.a = 255.0f;

		// Ok, now we precompute deltas for the various attributes of our
		// ..morphing field.
		pField->fDepthDelta = pField->fogVariance / (geFloat)pField->fogSpeed;
		pField->fRadiusDelta = (pField->fogRadiusEnd - pField->fogRadiusStart) /
			(geFloat)pField->fogSpeed;
		pField->cDelta.r = (pField->clrEnd.r - pField->clrStart.r) / (geFloat)pField->fogSpeed;
		pField->cDelta.g = (pField->clrEnd.g - pField->clrStart.g) / (geFloat)pField->fogSpeed;
		pField->cDelta.b = (pField->clrEnd.b - pField->clrStart.b) / (geFloat)pField->fogSpeed;
		pField->OriginOffset = pField->origin;

		if(pField->Model)
		{
			geVec3d ModelOrigin;
			geWorld_GetModelRotationalCenter(CCD->World(), pField->Model, &ModelOrigin);
			geVec3d_Subtract(&pField->origin, &ModelOrigin, &pField->OriginOffset);
  		}
	}

	return;
}

/* ------------------------------------------------------------------------------------ */
//	Destructor
//
//	Clean up all audio and fog on our way out.
/* ------------------------------------------------------------------------------------ */
CMorphingFields::~CMorphingFields()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	if(m_MorphingFieldCount == 0)
		return;						// Don't waste CPU cycles

	//	Ok, check to see if there are morphing fields in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "MorphingField");

	if(!pSet)
		return;									// No fields

	//	Ok, we have morphing fields somewhere.  Dig through 'em all and release
	//	..the audio (if any).
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		MorphingField *pField = (MorphingField*)geEntity_GetUserData(pEntity);

		if(pField->theFog != NULL)
			geWorld_RemoveFog(CCD->World(), pField->theFog);

		pField->theFog = NULL;								// Fog cleared, heh heh
	}

	//	Morphing fields cleaned up.  Bail this mess.
	return;
}

/* ------------------------------------------------------------------------------------ */
//	Tick
//
//	Perform animation work on this morphing field.  In this case, that means
//	..updating any looping audio effects for the players current position,
//	..morphing fog color, morphing fog radius, and morphing fog density.
//	..It seems like a lot of work, but the effect is _cool_!
/* ------------------------------------------------------------------------------------ */
void CMorphingFields::Tick(geFloat dwTicks)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	if(m_MorphingFieldCount == 0)
		return;						// Don't waste CPU cycles

	// Ok, check to see if there are morphing fields in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "MorphingField");

	if(!pSet)
		return;									// No fields

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		MorphingField *pField = (MorphingField*)geEntity_GetUserData(pEntity);

		if(!EffectC_IsStringNull(pField->TriggerName))
		{
			if(GetTriggerState(pField->TriggerName))
			{
				if(pField->active == GE_FALSE)
				{
					if(pField->theSound && pField->SoundHandle==-1)
					{
						Snd Sound;
						memset(&Sound, 0, sizeof(Sound));
						geVec3d_Copy(&(pField->origin), &(Sound.Pos));
						Sound.Min = CCD->GetAudibleRadius();
						Sound.Loop = GE_TRUE;
						Sound.SoundDef = pField->theSound;
						pField->SoundHandle = CCD->EffectManager()->Item_Add(EFF_SND, (void*)&Sound);
					}

					pField->active = GE_TRUE;
				}
			}
			else
			{
				if(pField->active == GE_TRUE)
				{
					if(pField->theFog != NULL)
						geWorld_RemoveFog(CCD->World(), pField->theFog);

					pField->theFog = NULL;

					if(pField->SoundHandle != -1)
					{
						CCD->EffectManager()->Item_Delete(EFF_SND, pField->SoundHandle);
						pField->SoundHandle = -1;
					}

					pField->active = GE_FALSE;
				}
			}
		}
		else
		{
			if(pField->active == GE_FALSE)
			{
				if(pField->theSound && pField->SoundHandle == -1)
				{
					Snd Sound;
					memset( &Sound, 0, sizeof( Sound ) );
					geVec3d_Copy( &(pField->origin), &( Sound.Pos ) );
					Sound.Min = CCD->GetAudibleRadius();
					Sound.Loop = GE_TRUE;
					Sound.SoundDef = pField->theSound;
					pField->SoundHandle = CCD->EffectManager()->Item_Add(EFF_SND, (void*)&Sound);
				}

				pField->active = GE_TRUE;
			}
		}

		if(pField->active == GE_TRUE)
		{
			if(pField->bFollower == GE_TRUE)
			{
				// Seek to next position, if in motion
				CCD->PathFollower()->GetNextPosition(pField->szEntityName, &pField->origin,	false);
			}
			else
			{
				pField->origin = pField->OriginOffset;
				SetOriginOffset(pField->EntityName, pField->BoneName, pField->Model, &(pField->origin));
			}

			int32 Leaf;
			geWorld_GetLeaf(CCD->World(), &(pField->origin), &Leaf);

			if(EffectC_IsPointVisible(CCD->World(),
					CCD->CameraManager()->Camera(),
					&(pField->origin),
					Leaf,
					EFFECTC_CLIP_LEAF) == GE_FALSE)
			{
				if(pField->theFog != NULL)
					geWorld_RemoveFog(CCD->World(), pField->theFog);

				pField->theFog = NULL;
				pField->active = GE_FALSE;
			}
			else
			{
				if(pField->theFog == NULL)
				{
					pField->theFog = geWorld_AddFog(CCD->World());

					if(pField->theFog != NULL)
						geFog_SetAttributes(pField->theFog, &pField->origin, &pField->clrStart, 0.0f,
											pField->fDensity, pField->fogRadiusStart);
				}
			}

			pField->tElapsed -= dwTicks;			// Decrement cycle time

			if(pField->theFog != NULL)
			{
				// Morph the fog from start to end and back.
				pField->fDensity += pField->fDepthDelta * dwTicks;	// Animate density

				if((pField->fDensity > (pField->fogVariance + kFogDensity)) ||
					(pField->fDensity < (kFogDensity-pField->fogVariance)))
					pField->fDepthDelta = -(pField->fDepthDelta);		// Flip sign

				pField->fRadius += pField->fRadiusDelta;

				if((pField->fRadius > pField->fogRadiusEnd) ||
					(pField->fRadius < pField->fogRadiusStart))
					pField->fRadiusDelta = -(pField->fRadiusDelta);	// Flip sign

				switch(pField->bForward)
				{
				case GE_TRUE:
					pField->clrCurrent.r += (pField->cDelta.r * dwTicks);
					pField->clrCurrent.g += (pField->cDelta.g * dwTicks);
					pField->clrCurrent.b += (pField->cDelta.b * dwTicks);

					// Ok, clamp out-of-range values
					if(pField->clrCurrent.r > 255.0) pField->clrCurrent.r = 255.0;
					else if(pField->clrCurrent.r < 0.0) pField->clrCurrent.r = 0.0;

					if(pField->clrCurrent.g > 255.0) pField->clrCurrent.g = 255.0;
					else if(pField->clrCurrent.g < 0.0) pField->clrCurrent.g = 0.0;

					if(pField->clrCurrent.b > 255.0) pField->clrCurrent.b = 255.0;
					else if(pField->clrCurrent.b < 0.0) pField->clrCurrent.b = 0.0;

					geFog_SetAttributes(pField->theFog, &pField->origin, &pField->clrCurrent, 0.0f,
										pField->fDensity, pField->fRadius);			// Animate it all

					if(pField->tElapsed <= 0)
					{
						pField->bForward = GE_FALSE;								// Switch direction
						pField->tElapsed = (geFloat)pField->fogSpeed;
						pField->clrCurrent = pField->clrEnd;
					}
					break;
				case GE_FALSE:
					pField->clrCurrent.r -= (pField->cDelta.r * dwTicks);
					pField->clrCurrent.g -= (pField->cDelta.g * dwTicks);
					pField->clrCurrent.b -= (pField->cDelta.b * dwTicks);

					// Ok, clamp out-of-range values
					if(pField->clrCurrent.r > 255.0) pField->clrCurrent.r = 255.0;
					else if(pField->clrCurrent.r < 0.0) pField->clrCurrent.r = 0.0;

					if(pField->clrCurrent.g > 255.0) pField->clrCurrent.g = 255.0;
					else if(pField->clrCurrent.g < 0.0) pField->clrCurrent.g = 0.0;

					if(pField->clrCurrent.b > 255.0) pField->clrCurrent.b = 255.0;
					else if(pField->clrCurrent.b < 0.0) pField->clrCurrent.b = 0.0;

					geFog_SetAttributes(pField->theFog, &pField->origin, &pField->clrCurrent, 0.0f,
										pField->fDensity, pField->fRadius);			// Animate it all

					if(pField->tElapsed <= 0)
					{
						pField->bForward = GE_TRUE;									// Switch direction
						pField->tElapsed = (geFloat)pField->fogSpeed;
						pField->clrCurrent = pField->clrStart;
					}

					break;
				}
			}
		}
	}

	return;
}

/* ------------------------------------------------------------------------------------ */
//	SaveTo
//
//	Save morphingfield states to a supplied file
/* ------------------------------------------------------------------------------------ */
int CMorphingFields::SaveTo(FILE *SaveFD)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	if(m_MorphingFieldCount == 0)
		return RGF_SUCCESS;						// Don't waste CPU cycles

	// Ok, check to see if there are morphing fields in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "MorphingField");

	if(!pSet)
		return RGF_SUCCESS;									// No fields

	// Ok, we have morphing fields somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		MorphingField *pField = (MorphingField*)geEntity_GetUserData(pEntity);

		fwrite(&pField->bForward,	sizeof(geBoolean),	1, SaveFD);
		fwrite(&pField->fRadius,	sizeof(geFloat),	1, SaveFD);
		fwrite(&pField->tElapsed,	sizeof(int),		1, SaveFD);
		fwrite(&pField->clrCurrent, sizeof(GE_RGBA),	1, SaveFD);
		fwrite(&pField->origin,		sizeof(geVec3d),	1, SaveFD);
	}

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	RestoreFrom
//
//	Restore morphingfield states from a supplied file
/* ------------------------------------------------------------------------------------ */
int CMorphingFields::RestoreFrom(FILE *RestoreFD)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	if(m_MorphingFieldCount == 0)
		return RGF_SUCCESS;						// Don't waste CPU cycles

	// Ok, check to see if there are morphing fields in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "MorphingField");

	if(!pSet)
		return RGF_SUCCESS;									// No fields

	// Ok, we have morphing fields somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		MorphingField *pField = (MorphingField*)geEntity_GetUserData(pEntity);

		fread(&pField->bForward,	sizeof(geBoolean),	1, RestoreFD);
		fread(&pField->fRadius,		sizeof(geFloat),	1, RestoreFD);
		fread(&pField->tElapsed,	sizeof(int),		1, RestoreFD);
		fread(&pField->clrCurrent,	sizeof(GE_RGBA),	1, RestoreFD);
		fread(&pField->origin,		sizeof(geVec3d),	1, RestoreFD);
	}

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	BindToPath
//
//	Given the name of an entity, bind that entity to a motion path.
/* ------------------------------------------------------------------------------------ */
int CMorphingFields::BindToPath(char *szName)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	// Ok, check to see if there are morphing fields in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "MorphingField");

	if(!pSet)
		return RGF_FAILURE;									// No fields

	// Ok, we have morphing fields somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		MorphingField *pSource = (MorphingField*)geEntity_GetUserData(pEntity);

		if(!strcmp(pSource->szEntityName, szName))
		{
			pSource->bFollower = GE_TRUE;
			return RGF_SUCCESS;					// Flagged and good to go
		}
	}

	return RGF_FAILURE;							// No such entity, sorry
}

//	******************** CRGF Overrides ********************

/* ------------------------------------------------------------------------------------ */
//	LocateEntity
//
//	Given a name, locate the desired item in the currently loaded level
//	..and return it's user data.
/* ------------------------------------------------------------------------------------ */
int CMorphingFields::LocateEntity(char *szName, void **pEntityData)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	// Ok, check to see if there are morphing fields in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "MorphingField");

	if(!pSet)
		return RGF_NOT_FOUND;									// No morphing fields

	// Ok, we have morphing fields.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		MorphingField *pTheEntity = (MorphingField*)geEntity_GetUserData(pEntity);

		if(!strcmp(pTheEntity->szEntityName, szName))
		{
			*pEntityData = (void*)pTheEntity;
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
int CMorphingFields::ReSynchronize()
{
	return RGF_SUCCESS;
}


/* ----------------------------------- END OF FILE ------------------------------------ */
