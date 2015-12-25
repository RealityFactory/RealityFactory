/************************************************************************************//**
 * @file C3DAudioSource.cpp
 * @brief 3D Audio Source class implementation
 *
 * This file contains the class implementation for 3D audio source handling.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

// Include the One True Header
#include "RabidFramework.h"
#include "C3DAudioSource.h"

extern geSound_Def *SPool_Sound(const char *SName);

/* ------------------------------------------------------------------------------------ */
// Constructor
//
// Load up all 3D audio sources, load and initialize sounds and set the
// ..entities to default values.
/* ------------------------------------------------------------------------------------ */
C3DAudioSource::C3DAudioSource() :
	m_EntityCount(0)
{
	// Ok, check to see if there are 3D audio sources in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "AudioSource3D");

	if(!pSet)
		return;		// No 3D audio sources

	geEntity *pEntity;

	// Ok, we have 3D audio sources somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		AudioSource3D *pSource = static_cast<AudioSource3D*>(geEntity_GetUserData(pEntity));

		if(EffectC_IsStringNull(pSource->szEntityName))
		{
			char szName[128];
			geEntity_GetName(pEntity, szName, 128);
			pSource->szEntityName = szName;
		}

		++m_EntityCount;							// Kick source count
		// Ok, put this entity into the Global Entity Registry
		CCD->EntityRegistry()->AddEntity(pSource->szEntityName, "AudioSource3D");
		pSource->OriginOffset = pSource->origin;

		if(pSource->Model)
		{
			geVec3d ModelOrigin;
			geWorld_GetModelRotationalCenter(CCD->World(), pSource->Model, &ModelOrigin);
			geVec3d_Subtract(&pSource->origin, &ModelOrigin, &pSource->OriginOffset);
		}

		// Reset all the data for each 3D audio source
		pSource->effect = -1;
		pSource->active = GE_FALSE;

		if(!EffectC_IsStringNull(pSource->szSoundFile))
		{
			SPool_Sound(pSource->szSoundFile);

			// set the entity active if there's no trigger
			if(EffectC_IsStringNull(pSource->TriggerName))
			{
				pSource->effect = Create(pSource->origin, pSource->szSoundFile,
											pSource->fRadius, pSource->bLoopSound);
				pSource->active = GE_TRUE;
			}
		}

		pSource->ProgrammedTrigger = GE_FALSE;
	}
	// Audio loaded and running, let's go!
}


/* ------------------------------------------------------------------------------------ */
// Create
/* ------------------------------------------------------------------------------------ */
int C3DAudioSource::Create(const geVec3d &Origin, const char *SoundFile, float radius, geBoolean Looping)
{
	int effect = -1;
	Snd Sound;

	memset(&Sound, 0, sizeof(Sound));
	geVec3d_Copy(&Origin, &(Sound.Pos));
	Sound.Min = radius;

	Sound.Loop = Looping;

	Sound.SoundDef = SPool_Sound(SoundFile);

	if(!Sound.SoundDef)
	{
		char szError[256];
		sprintf(szError, "[WARNING] File %s - Line %d: Failed to open audio file '%s'\n",
				__FILE__, __LINE__, SoundFile);
		CCD->ReportError(szError, false);
	}
	else
	{
		effect = CCD->EffectManager()->Item_Add(EFF_SND, static_cast<void*>(&Sound));
	}

	return effect;
}


/* ------------------------------------------------------------------------------------ */
// Destructor
//
// Clean up all audio we have loaded.
/* ------------------------------------------------------------------------------------ */
C3DAudioSource::~C3DAudioSource()
{
}


/* ------------------------------------------------------------------------------------ */
// Tick
/* ------------------------------------------------------------------------------------ */
void C3DAudioSource::Tick(geFloat /*dwTicks*/)
{
	if(m_EntityCount == 0)
		return;		// Don't waste CPU cycles

	// Ok, check to see if there are 3D audio sources in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "AudioSource3D");

	if(!pSet)
		return;		// No sources

	geEntity *pEntity;

	// Ok, we have 3D audio sources somewhere. Dig through 'em all.
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		AudioSource3D *pSource = static_cast<AudioSource3D*>(geEntity_GetUserData(pEntity));

		if(!EffectC_IsStringNull(pSource->TriggerName))
		{
			if( ((strcmp(pSource->TriggerName, "*Programmed*") == 0) && pSource->ProgrammedTrigger) ||
				((strcmp(pSource->TriggerName, "*Programmed*") != 0) && GetTriggerState(pSource->TriggerName)))
			{
				if(pSource->active == GE_FALSE)
				{
					pSource->effect = Create(pSource->origin, pSource->szSoundFile, pSource->fRadius, pSource->bLoopSound);
					pSource->active = GE_TRUE;

					if(!pSource->bLoopSound)
						pSource->ProgrammedTrigger = GE_FALSE;
				}
			}
			else
			{
				if(pSource->bLoopSound)
				{
					if(pSource->effect != -1)
					{
						CCD->EffectManager()->Item_Delete(EFF_SND, pSource->effect);
						pSource->effect = -1;
					}

					pSource->active = GE_FALSE;
				}
				else
				{
					if((pSource->effect != -1) && !CCD->EffectManager()->Item_Alive(pSource->effect))
					{
						CCD->EffectManager()->Item_Delete(EFF_SND, pSource->effect);
						pSource->effect = -1;
					}

					pSource->active = GE_FALSE;
				}
			}
		}

		if(pSource->active == GE_TRUE)
		{
			pSource->origin = pSource->OriginOffset;

			if(SetOriginOffset(pSource->EntityName, pSource->BoneName, pSource->Model, &(pSource->origin)))
			{
				if(pSource->effect != -1)
				{
					Snd Sound;
					geVec3d_Copy(&(pSource->origin), &(Sound.Pos));
					CCD->EffectManager()->Item_Modify(EFF_SND, pSource->effect, static_cast<void*>(&Sound), SND_POS);
				}
			}
		}
	}

	return;
}


/* ------------------------------------------------------------------------------------ */
// SetProgrammedTrigger
//
// Given a name, locate the desired item in the currently loaded level
// ..and set its ProgrammedTrigger boolean.
/* ------------------------------------------------------------------------------------ */
int C3DAudioSource::SetProgrammedTrigger(const char *szName, geBoolean Flag)
{
	// Ok, check to see if there are 3D Audio Sources in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "AudioSource3D");

	if(!pSet)
		return RGF_NOT_FOUND;						// No 3D audio sources

	geEntity *pEntity;

	// Ok, we have 3D audio sources.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet,NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet,pEntity))
	{
		AudioSource3D *pSource = static_cast<AudioSource3D*>(geEntity_GetUserData(pEntity));

		if(!strcmp(pSource->szEntityName, szName))
		{
			pSource->ProgrammedTrigger = Flag;
			return RGF_SUCCESS;
		}
	}

	return RGF_NOT_FOUND;							// Sorry, no such entity here
}


/* ------------------------------------------------------------------------------------ */
// IsPlaying
//
// Given a name, locate the desired sound in the currently loaded level
// ..and return true if it is playing, false if not.
/* ------------------------------------------------------------------------------------ */
geBoolean C3DAudioSource::IsPlaying(const char *szName)
{
	// Ok, check to see if there are 3D Audio Sources in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "AudioSource3D");

	if(!pSet)
		return GE_FALSE;							// No 3D audio sources

	geEntity *pEntity;

	// Ok, we have 3D audio sources.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet,NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet,pEntity))
	{
		AudioSource3D *pSource = static_cast<AudioSource3D*>(geEntity_GetUserData(pEntity));

		if(!strcmp(pSource->szEntityName, szName))
		{
			if(pSource->effect != -1)
				return GE_TRUE;
			else
				return GE_FALSE;
		}
	}

	return GE_FALSE;								// Sorry, no such entity here
}


// ******************** CRGF Overrides ********************

/* ------------------------------------------------------------------------------------ */
// LocateEntity
//
// Given a name, locate the desired item in the currently loaded level
// ..and return its user data.
/* ------------------------------------------------------------------------------------ */
int C3DAudioSource::LocateEntity(const char *szName, void **pEntityData)
{
	// Ok, check to see if there are 3D audio sources in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "AudioSource3D");

	if(!pSet)
		return RGF_NOT_FOUND;						// No 3D audio sources

	geEntity *pEntity;

	// Ok, we have 3D audio sources somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		AudioSource3D *pSource = static_cast<AudioSource3D*>(geEntity_GetUserData(pEntity));

		if(!strcmp(pSource->szEntityName, szName))
		{
			*pEntityData = static_cast<void*>(pSource);
			return RGF_SUCCESS;
		}
	}

	return RGF_NOT_FOUND;							// Sorry, no such entity here
}


/* ------------------------------------------------------------------------------------ */
// ReSynchronize
//
// Correct internal timing to match current time, to make up for time lost
// ..when outside the game loop (typically in "menu mode").
/* ------------------------------------------------------------------------------------ */
int C3DAudioSource::ReSynchronize()
{
	return RGF_SUCCESS;
}


/* ----------------------------------- END OF FILE ------------------------------------ */
