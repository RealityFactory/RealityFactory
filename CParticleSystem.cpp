/************************************************************************************//**
 * @file CParticleSystem.cpp
 * @brief Particle Systems Handler
 *
 * This file contains the class implementation and data types for the RGF particle
 * system handler classes.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#include "RabidFramework.h"
#include "CParticleSystem.h"

extern geSound_Def *SPool_Sound(const char *SName);
extern geBitmap *TPool_Bitmap(const char *DefaultBmp, const char *DefaultAlpha,
							  const char *BName, const char *AName);

/* ------------------------------------------------------------------------------------ */
// Constructor
//
// Initialize the list of active particle systems to empty, then go through the
// loaded world and set up all the static particle	systems defined therein.
/* ------------------------------------------------------------------------------------ */
CParticleSystem::CParticleSystem()
{
	// Clear out our containers
	memset(&theList, 0, sizeof(ParticleSystem*) * 50);
	memset(&bmpList, 0, sizeof(geBitmap*) * 50);

	// Ok, see if we have any particle system proxies we need to set up.
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "ParticleSystemProxy");

	if(!pSet)
		return;													// Don't waste CPU time.

	geEntity *pEntity;
	int pmHandle, psHandle;

	// Ok, we have particle systems somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		ParticleSystemProxy *pProxy = static_cast<ParticleSystemProxy*>(geEntity_GetUserData(pEntity));

		if(EffectC_IsStringNull(pProxy->szEntityName))
		{
			char szName[128];
			geEntity_GetName(pEntity, szName, 128);
			pProxy->szEntityName = szName;
		}

		// Ok, put this entity into the Global Entity Registry
		CCD->EntityRegistry()->AddEntity(pProxy->szEntityName, "ParticleSystemProxy");

		SetOriginOffset(pProxy->EntityName, pProxy->BoneName, pProxy->Model, &(pProxy->origin));
		pProxy->OriginOffset = pProxy->origin;

		// Ok, let's set up the system.
		pProxy->clrColor.a = 255.0f;		// Maximum alpha to start

		if(LoadParticleMap(&pmHandle, pProxy->szTexture, pProxy->szAlpha) != RGF_SUCCESS)
		{
			char szBug[256];
			sprintf(szBug, "[WARNING] File %s - Line %d: %s: Failed to load particle map '%s'\n",
					__FILE__, __LINE__, pProxy->szEntityName, pProxy->szTexture);
			CCD->ReportError(szBug, false);
			return;												// Pretty serious problem.
		}

		switch(pProxy->nStyle)
		{
		case kPSystem_Shockwave:
		case kPSystem_Fountain:
		case kPSystem_Rain:
		case kPSystem_Sphere:
		case kPSystem_Column:
		case kPSystem_ExplosiveArray:
		case kPSystem_SpiralArm:
		case kPSystem_Trail:
		case kPSystem_Guardian:
		case kPSystem_ImplodeSphere:
		case kPSystem_ImplodeShockwave:
		case kPSystem_ImplodeSpiralArm:
			{
				if(Create(&psHandle, pProxy->nStyle, pmHandle, pProxy->origin,
					pProxy->Scale, pProxy->szEntityName) != RGF_SUCCESS)
				{
					CCD->ReportError("[WARNING] Failed to create particle system from proxy", false);
					return;							// Way bad serious error
				}

				// Set the particle color, unhide it, and start it up.
				SetColor(psHandle, pProxy->clrColor, true);
				SetSystemLife(psHandle, -1);		// Lives until level closes

				// Check for system default over-rides in the proxy
				if(pProxy->BirthRate != 0)
					SetBirthRate(psHandle, pProxy->BirthRate);

				if(pProxy->MaxParticleCount != 0)
					SetMaxSize(psHandle, pProxy->MaxParticleCount);

				if(pProxy->ParticleLifespan != 0)
					SetParticleLife(psHandle, pProxy->ParticleLifespan);

				Hide(psHandle, true);				// hide it
				Pause(psHandle, true);				// Stop it
				pProxy->psHandle = psHandle;		// Save handle off
				pProxy->pmHandle = pmHandle;		// Save off texture handle, too.
				break;
			}
		default:
			CCD->ReportError("[WARNING] Illegal particle system style", false);
			return;											// Another serious error.
		}

		pProxy->active = GE_FALSE;
		pProxy->effect = -1;

		if(!EffectC_IsStringNull(pProxy->szSoundFile))
		{
			SPool_Sound(pProxy->szSoundFile);
		}

		if(EffectC_IsStringNull(pProxy->TriggerName))
		{
			pProxy->active = GE_TRUE;
			Hide(psHandle, false);	// unhide it
			Pause(psHandle, false);	// Start it
		}
	}
}


/* ------------------------------------------------------------------------------------ */
// CreateSound
/* ------------------------------------------------------------------------------------ */
int CParticleSystem::CreateSound(const geVec3d &Origin, const char *SoundFile, float radius)
{
	int effect = -1;
	Snd Sound;

	memset(&Sound, 0, sizeof(Sound));
	geVec3d_Copy(&Origin, &(Sound.Pos));
	Sound.Min = radius;
	Sound.Loop = GE_TRUE;
	Sound.SoundDef = SPool_Sound(SoundFile);

	if(!(Sound.SoundDef))
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
// Shut down and clean up all active particle systems.
/* ------------------------------------------------------------------------------------ */
CParticleSystem::~CParticleSystem()
{
	for(int nTemp=0; nTemp<50; ++nTemp)
	{
		if(theList[nTemp] != NULL)
		{
			Clear(nTemp);
			delete theList[nTemp];
			theList[nTemp] = NULL;
		}
	}
}


/* ------------------------------------------------------------------------------------ */
// LoadParticleMap
//
// This function loads a texture map to be used as a particle texture.
// ..Note that, if the ALPHA map is not specified, the SAME FILE is
// ..used both for alpha and color.
/* ------------------------------------------------------------------------------------ */
int CParticleSystem::LoadParticleMap(int *pmHandle, const char *szPrimary, const char *szAlpha)
{
	int nHandle = (-1), nTemp;

	// Sanity check
	if(pmHandle == NULL)
		return RGF_FAILURE;		// Bad programmer!

	if(EffectC_IsStringNull(szPrimary) == GE_TRUE)
		return RGF_FAILURE;		//if you want to have a default bmp file you can do it here

	// Ok, first find a free handle
	for(nTemp=0; nTemp<50; ++nTemp)
	{
		if(bmpList[nTemp] == NULL)
		{
			nHandle = nTemp;
			break;
		}
	}

	if(nHandle < 0)
		return RGF_FAILURE;							// Didn't work!

	// We have a handle, let's load the files.
	if(EffectC_IsStringNull(szAlpha) == GE_TRUE)
	{
		bmpList[nHandle] = TPool_Bitmap(szPrimary, szPrimary, NULL, NULL);
	}
	else
	{
		bmpList[nHandle] = TPool_Bitmap(szPrimary, szAlpha, NULL, NULL);
	}

	if(bmpList[nHandle] == NULL)
	{
		char szBug[200];
		sprintf(szBug, "[WARNING] File %s - Line %d: Failed to open particlemap file '%s'",
				__FILE__, __LINE__, szPrimary);
		CCD->ReportError(szBug, false);
		return RGF_FAILURE;
	}

	*pmHandle = nHandle;								// Handle back to caller

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// Create
//
// Create a brand-new particle system.  This particle system becomes active
// immediately on return from this routine and will be eligible for display in the
// next Render() call. A handle to the particle system is returned if it created
// successfully.  Note that all systems are created NOT RUNNING and HIDDEN.
/* ------------------------------------------------------------------------------------ */
int CParticleSystem::Create(int *psHandle, int nSystemType, int nTexHandle,
							const geVec3d &emitterPos, float Scale, const char *szName)
{
	int nHandle = -1;

	*psHandle = -1;						// Assume failure

	// Various defaults: white, no rotation.
	GE_RGBA clrWhite = {255.0f, 255.0f, 255.0f, 255.0f};

	// Ok, which one of our menu of particle system types would the
	// ..caller like today?
	switch(nSystemType)
	{
	case kPSystem_Shockwave:
		{
			nHandle = Allocate();
			if(nHandle < 0)
			{
				CCD->ReportError("[WARNING] PSAlloc failed, SHOCKWAVE", false);
				return RGF_FAILURE;
			}
			// Set particle system defaults
			theList[nHandle]->Lifetime			= 5000.0f;			// Seconds of life, per particle
			theList[nHandle]->SystemLife		= 5000.0f;			// Whole system life
			theList[nHandle]->Scale				= 0.1f*Scale;		// Small particles
			theList[nHandle]->bRunning			= false;			// Create paused
			theList[nHandle]->bVisible			= false;			// Create invisible
			theList[nHandle]->Texture			= bmpList[nTexHandle];
			theList[nHandle]->Emitter			= emitterPos;		// Emitter position
			theList[nHandle]->nType				= nSystemType;		// Save system type off
			theList[nHandle]->Color				= clrWhite;			// Default to white particles
			theList[nHandle]->InitialVelocity	= 0.9f;				// Initial particle velocity
			theList[nHandle]->Gravity			= -0.01f;			// Miniscule gravity effect
			theList[nHandle]->Maximum			= 120;				// Max 120 particles in shockwave
			theList[nHandle]->SpawnSpeed		= 0;				// No spawning for shockwave
			theList[nHandle]->EffectRadius		= 0.0f;				// Radius unused
			theList[nHandle]->bForever			= false;			// Not an infinite system
			strcpy(theList[nHandle]->szName, szName);				// Particle system name
			// Ok, system defaults set.  Load the particle list with the
			// ..initial particles to be used for the shockwave
			SetupShockwave(nHandle);
			break;
		}
	case kPSystem_Fountain:
		{
			nHandle = Allocate();
			if(nHandle < 0)
			{
				CCD->ReportError("[WARNING] PSAlloc failed, FOUNTAIN", false);
				return RGF_FAILURE;
			}
			// Set particle system defaults
			theList[nHandle]->Lifetime			= 5000.0f;			// Seconds of life, per particle
			theList[nHandle]->SystemLife		= 22000.0f;			// Whole system life
			theList[nHandle]->Scale				= 0.05f*Scale;		// Small particles
			theList[nHandle]->bRunning			= false;			// Create paused
			theList[nHandle]->bVisible			= false;			// Create invisible
			theList[nHandle]->Texture			= bmpList[nTexHandle];
			theList[nHandle]->Emitter			= emitterPos;		// Emitter position
			theList[nHandle]->nType				= nSystemType;		// Save system type off
			theList[nHandle]->Color				= clrWhite;			// Default to white particles
			theList[nHandle]->InitialVelocity	= 0.8f;				// Initial particle velocity
			theList[nHandle]->Gravity			= -0.2f;			// Miniscule gravity effect
			theList[nHandle]->Maximum			= 250;				// Max 250 particles in fountain
			theList[nHandle]->SpawnSpeed		= 50;				// Add up to 50 particles/second
			theList[nHandle]->EffectRadius		= 0.0f;				// Radius unused
			theList[nHandle]->bForever			= false;			// Not an infinite system
			strcpy(theList[nHandle]->szName, szName);				// Particle system name
			// Ok, system defaults set.  Load the particle list with the
			// ..initial particles to be used for the fountain
			SetupFountain(nHandle);
			break;
		}
	case kPSystem_Rain:
		{
			nHandle = Allocate();
			if(nHandle < 0)
			{
				CCD->ReportError("[WARNING] PSAlloc failed, RAIN", false);
				return RGF_FAILURE;
			}
			// Set particle system defaults
			theList[nHandle]->Lifetime			= 5000.0f;			// Seconds of life, per particle
			theList[nHandle]->SystemLife		= 30000.0f;			// Whole system life
			theList[nHandle]->Scale				= 0.03f*Scale;		// Small particles
			theList[nHandle]->bRunning			= false;			// Create paused
			theList[nHandle]->bVisible			= false;			// Create invisible
			theList[nHandle]->Texture			= bmpList[nTexHandle];
			theList[nHandle]->Emitter			= emitterPos;		// Emitter position
			theList[nHandle]->nType				= nSystemType;		// Save system type off
			theList[nHandle]->Color				= clrWhite;			// Default to white particles
			theList[nHandle]->InitialVelocity	= 0.0f;				// Initial particle velocity
			theList[nHandle]->Gravity			= -0.8f;			// Miniscule gravity effect
			theList[nHandle]->Maximum			= 200;				// Max 180 particles in fountain
			theList[nHandle]->SpawnSpeed		= 40;				// Add up to 40 particles/second
			theList[nHandle]->EffectRadius		= 12.0f;			// Max # of units from emitter
			theList[nHandle]->bForever			= false;			// Not an infinite system
			strcpy(theList[nHandle]->szName, szName);				// Particle system name
			// Ok, system defaults set.  Load the particle list with the
			// ..initial particles to be used for the rain system
			SetupRain(nHandle);
			break;
		}
	case kPSystem_Sphere:
		{
			nHandle = Allocate();
			if(nHandle < 0)
			{
				CCD->ReportError("[WARNING] PSAlloc failed, SPHERE", false);
				return RGF_FAILURE;
			}
			// Set particle system defaults
			theList[nHandle]->Lifetime			= 2000.0f;			// Seconds of life, per particle
			theList[nHandle]->SystemLife		= 20000.0f;			// Whole system life
			theList[nHandle]->Scale				= 0.03f*Scale;		// Small particles
			theList[nHandle]->bRunning			= false;			// Create paused
			theList[nHandle]->bVisible			= false;			// Create invisible
			theList[nHandle]->Texture			= bmpList[nTexHandle];
			theList[nHandle]->Emitter			= emitterPos;		// Emitter position
			theList[nHandle]->nType				= nSystemType;		// Save system type off
			theList[nHandle]->Color				= clrWhite;			// Default to white particles
			theList[nHandle]->InitialVelocity	= 0.8f;				// Initial particle velocity
			theList[nHandle]->Gravity			= 0.0f;				// No gravity effect
			theList[nHandle]->Maximum			= 280;				// Max 280 particles in sphere
			theList[nHandle]->SpawnSpeed		= 60;				// Add up to 60 particles/second
			theList[nHandle]->EffectRadius		= 0.0f;				// Effect radius not used
			theList[nHandle]->bForever			= false;			// Not an infinite system
			strcpy(theList[nHandle]->szName, szName);				// Particle system name
			// Ok, system defaults set.  Load the particle list with the
			// ..initial particles to be used for the sphere system
			SetupSphere(nHandle);
			break;
		}
	case kPSystem_Column:
		{
			// A COLUMN is just a dense, fast UPWARDS RAIN
			nHandle = Allocate();
			if(nHandle < 0)
			{
				CCD->ReportError("[WARNING] PSAlloc failed, COLUMN", false);
				return RGF_FAILURE;
			}
			// Set particle system defaults
			theList[nHandle]->Lifetime			= 3500.0f;			// Seconds of life, per particle
			theList[nHandle]->SystemLife		= 16000.0f;			// Whole system life
			theList[nHandle]->Scale				= 0.07f*Scale;		// Medium particles
			theList[nHandle]->bRunning			= false;			// Create paused
			theList[nHandle]->bVisible			= false;			// Create invisible
			theList[nHandle]->Texture			= bmpList[nTexHandle];
			theList[nHandle]->Emitter			= emitterPos;		// Emitter position
			theList[nHandle]->nType				= nSystemType;		// Save system type off
			theList[nHandle]->Color				= clrWhite;			// Default to white particles
			theList[nHandle]->InitialVelocity	= 0.0f;				// Initial particle velocity
			theList[nHandle]->Gravity			= 1.8f;				// Fast gravity effect
			theList[nHandle]->Maximum			= 600;				// Max 600 particles in fountain
			theList[nHandle]->SpawnSpeed		= 80;				// Add up to 80 particles/second
			theList[nHandle]->EffectRadius		= 8.0f;				// Max # of units from emitter
			theList[nHandle]->bForever			= false;			// Not an infinite system
			strcpy(theList[nHandle]->szName, szName);				// Particle system name
			// Ok, system defaults set.  Load the particle list with the
			// ..initial particles to be used for the column system
			SetupRain(nHandle);
			break;
		}
	case kPSystem_ExplosiveArray:
		{
			// Note that an EXPLOSIVE ARRAY is just a fast UPWARDS RAIN.
			nHandle = Allocate();
			if(nHandle < 0)
			{
				CCD->ReportError("[WARNING] PSAlloc failed, EXPLOSIVE ARRAY", false);
				return RGF_FAILURE;
			}
			// Set particle system defaults
			theList[nHandle]->Lifetime			= 3000.0f;			// Seconds of life, per particle
			theList[nHandle]->SystemLife		= 8000.0f;			// Whole system life
			theList[nHandle]->Scale				= 0.05f*Scale;		// Small particles
			theList[nHandle]->bRunning			= false;			// Create paused
			theList[nHandle]->bVisible			= false;			// Create invisible
			theList[nHandle]->Texture			= bmpList[nTexHandle];
			theList[nHandle]->Emitter			= emitterPos;		// Emitter position
			theList[nHandle]->nType				= nSystemType;		// Save system type off
			theList[nHandle]->Color				= clrWhite;			// Default to white particles
			theList[nHandle]->InitialVelocity	= 0.0f;				// Initial particle velocity
			theList[nHandle]->Gravity			= 1.8f;				// Fast gravity effect
			theList[nHandle]->Maximum			= 500;				// Max 500 particles in fountain
			theList[nHandle]->SpawnSpeed		= 120;				// Add up to 120 particles/second
			theList[nHandle]->EffectRadius		= 18.0f;			// Max # of units from emitter
			theList[nHandle]->bForever			= false;			// Not an infinite system
			strcpy(theList[nHandle]->szName, szName);				// Particle system name
			// Ok, system defaults set.  Load the particle list with the
			// ..initial particles to be used for the explosive array system
			SetupRain(nHandle);
			break;
		}
	case kPSystem_SpiralArm:
		{
			nHandle = Allocate();
			if(nHandle < 0)
			{
				CCD->ReportError("[WARNING] PSAlloc failed: SpiralArm", false);
				return RGF_FAILURE;
			}
			// Set particle system defaults
			theList[nHandle]->Lifetime			= 4000.0f;			// Seconds of life, per particle
			theList[nHandle]->SystemLife		= 12000.0f;			// Whole system life
			theList[nHandle]->Scale				= 0.1f*Scale;		// Small particles
			theList[nHandle]->bRunning			= false;			// Create paused
			theList[nHandle]->bVisible			= false;			// Create invisible
			theList[nHandle]->Texture			= bmpList[nTexHandle];
			theList[nHandle]->Emitter			= emitterPos;		// Emitter position
			theList[nHandle]->nType				= nSystemType;		// Save system type off
			theList[nHandle]->Color				= clrWhite;			// Default to white particles
			theList[nHandle]->InitialVelocity	= 0.9f;				// Initial particle velocity
			theList[nHandle]->Gravity			= -0.05f;			// Miniscule gravity effect
			theList[nHandle]->Maximum			= 120;				// Max 120 particles in spiral arms
			theList[nHandle]->SpawnSpeed		= 80;				// 80 per sec for spiral arms
			theList[nHandle]->EffectRadius		= 0.0f;				// Radius unused
			theList[nHandle]->bForever			= false;			// Not an infinite system
			strcpy(theList[nHandle]->szName, szName);				// Particle system name
			// Ok, system defaults set.  Load the particle list with the
			// ..initial particles to be used for the spiral arms
			SetupSpiralArm(nHandle);
			break;
		}
	case kPSystem_Trail:
		{
			nHandle = Allocate();
			if(nHandle < 0)
			{
				CCD->ReportError("[WARNING] PSAlloc failed: SpiralArm", false);
				return RGF_FAILURE;
			}
			// Set particle system defaults
			theList[nHandle]->Lifetime			= 6000.0f;			// Seconds of life, per particle
			theList[nHandle]->SystemLife		= 12000.0f;			// Whole system life
			theList[nHandle]->Scale				= 0.06f*Scale;		// Small particles
			theList[nHandle]->bRunning			= false;			// Create paused
			theList[nHandle]->bVisible			= false;			// Create invisible
			theList[nHandle]->Texture			= bmpList[nTexHandle];
			theList[nHandle]->Emitter			= emitterPos;		// Emitter position
			theList[nHandle]->nType				= nSystemType;		// Save system type off
			theList[nHandle]->Color				= clrWhite;			// Default to white particles
			theList[nHandle]->InitialVelocity	= 0.2f;				// Initial particle velocity
			theList[nHandle]->Gravity			= -0.02f;			// Miniscule gravity effect
			theList[nHandle]->Maximum			= 220;				// Max 120 particles in spiral arms
			theList[nHandle]->SpawnSpeed		= 60;				// 80 per sec for spiral arms
			theList[nHandle]->EffectRadius		= 1.5f;				// Radius unused
			theList[nHandle]->bForever			= false;			// Not an infinite system
			strcpy(theList[nHandle]->szName, szName);				// Particle system name
			// Ok, system defaults set.  Load the particle list with the
			// ..initial particles to be used for the trail
			SetupTrail(nHandle);
			break;
		}
	case kPSystem_Guardian:
		{
			nHandle = Allocate();
			if(nHandle < 0)
			{
				CCD->ReportError("[WARNING] PSAlloc failed, GUARDIAN EFFECT", false);
				return RGF_FAILURE;
			}
			// Set particle system defaults
			theList[nHandle]->Lifetime			= 6000.0f;			// Seconds of life, per particle
			theList[nHandle]->SystemLife		= 20000.0f;			// Whole system life
			theList[nHandle]->Scale				= 0.04f*Scale;		// Small particles
			theList[nHandle]->bRunning			= false;			// Create paused
			theList[nHandle]->bVisible			= false;			// Create invisible
			theList[nHandle]->Texture			= bmpList[nTexHandle];
			theList[nHandle]->Emitter			= emitterPos;		// Emitter position
			theList[nHandle]->nType				= nSystemType;		// Save system type off
			theList[nHandle]->Color				= clrWhite;			// Default to white particles
			theList[nHandle]->InitialVelocity	= 0.9f;				// Initial particle velocity
			theList[nHandle]->Gravity			= 0.0f;				// No gravity effect
			theList[nHandle]->Maximum			= 320;				// Max 320 particles in effect
			theList[nHandle]->SpawnSpeed		= 70;				// Add up to 60 particles/second
			theList[nHandle]->EffectRadius		= 0.0f;				// Effect radius not used
			theList[nHandle]->bForever			= false;			// Not an infinite system
			strcpy(theList[nHandle]->szName, szName);				// Particle system name
			// Ok, system defaults set.  Load the particle list with the
			// ..initial particles to be used for the guardian effect system
			SetupGuardian(nHandle);
			break;
		}
	case kPSystem_ImplodeSphere:
		{
			nHandle = Allocate();
			if(nHandle < 0)
			{
				CCD->ReportError("[WARNING] PSAlloc failed, IMPLODESPHERE", false);
				return RGF_FAILURE;
			}
			// Set particle system defaults
			theList[nHandle]->Lifetime			= 2000.0f;			// Seconds of life, per particle
			theList[nHandle]->SystemLife		= 20000.0f;			// Whole system life
			theList[nHandle]->Scale				= 0.03f*Scale;		// Small particles
			theList[nHandle]->bRunning			= false;			// Create paused
			theList[nHandle]->bVisible			= false;			// Create invisible
			theList[nHandle]->Texture			= bmpList[nTexHandle];
			theList[nHandle]->Emitter			= emitterPos;		// Emitter position
			theList[nHandle]->nType				= nSystemType;		// Save system type off
			theList[nHandle]->Color				= clrWhite;			// Default to white particles
			theList[nHandle]->InitialVelocity	= 0.8f;				// Initial particle velocity
			theList[nHandle]->Gravity			= 0.0f;				// No gravity effect
			theList[nHandle]->Maximum			= 280;				// Max 280 particles in sphere
			theList[nHandle]->SpawnSpeed		= 60;				// Add up to 60 particles/second
			theList[nHandle]->EffectRadius		= 0.0f;				// Effect radius not used
			theList[nHandle]->bForever			= false;			// Not an infinite system
			strcpy(theList[nHandle]->szName, szName);				// Particle system name
			// Ok, system defaults set.  Load the particle list with the
			// ..initial particles to be used for the imploding sphere system
			SetupImplodeSphere(nHandle);
			break;
		}
	case kPSystem_ImplodeShockwave:
		{
			nHandle = Allocate();
			if(nHandle < 0)
			{
				CCD->ReportError("[WARNING] PSAlloc failed, IMPLODE_SHOCKWAVE", false);
				return RGF_FAILURE;
			}
			// Set particle system defaults
			theList[nHandle]->Lifetime			= 5000.0f;			// Seconds of life, per particle
			theList[nHandle]->SystemLife		= 5000.0f;			// Whole system life
			theList[nHandle]->Scale				= 0.1f*Scale;		// Small particles
			theList[nHandle]->bRunning			= false;			// Create paused
			theList[nHandle]->bVisible			= false;			// Create invisible
			theList[nHandle]->Texture			= bmpList[nTexHandle];
			theList[nHandle]->Emitter			= emitterPos;		// Emitter position
			theList[nHandle]->nType				= nSystemType;		// Save system type off
			theList[nHandle]->Color				= clrWhite;			// Default to white particles
			theList[nHandle]->InitialVelocity	= 0.9f;				// Initial particle velocity
			theList[nHandle]->Gravity			= -0.01f;			// Miniscule gravity effect
			theList[nHandle]->Maximum			= 120;				// Max 120 particles in shockwave
			theList[nHandle]->SpawnSpeed		= 0;				// No spawning for shockwave
			theList[nHandle]->EffectRadius		= 0.0f;				// Radius unused
			theList[nHandle]->bForever			= false;			// Not an infinite system
			strcpy(theList[nHandle]->szName, szName);				// Particle system name
			// Ok, system defaults set.  Load the particle list with the
			// ..initial particles to be used for the shockwave
			SetupImplodeShockwave(nHandle);
			break;
		}
	case kPSystem_ImplodeSpiralArm:
		{
			nHandle = Allocate();
			if(nHandle < 0)
			{
				CCD->ReportError("[WARNING] PSAlloc failed: SpiralArm", false);
				return RGF_FAILURE;
			}
			// Set particle system defaults
			theList[nHandle]->Lifetime			= 4000.0f;			// Seconds of life, per particle
			theList[nHandle]->SystemLife		= 12000.0f;			// Whole system life
			theList[nHandle]->Scale				= 0.1f*Scale;		// Small particles
			theList[nHandle]->bRunning			= false;			// Create paused
			theList[nHandle]->bVisible			= false;			// Create invisible
			theList[nHandle]->Texture			= bmpList[nTexHandle];
			theList[nHandle]->Emitter			= emitterPos;		// Emitter position
			theList[nHandle]->nType				= nSystemType;		// Save system type off
			theList[nHandle]->Color				= clrWhite;			// Default to white particles
			theList[nHandle]->InitialVelocity	= 0.9f;				// Initial particle velocity
			theList[nHandle]->Gravity			= -(0.05f);			// Miniscule gravity effect
			theList[nHandle]->Maximum			= 120;				// Max 120 particles in spiral arms
			theList[nHandle]->SpawnSpeed		= 80;				// 80 per sec for spiral arms
			theList[nHandle]->EffectRadius		= 0.0f;				// Radius unused
			theList[nHandle]->bForever			= false;			// Not an infinite system
			strcpy(theList[nHandle]->szName, szName);				// Particle system name
			// Ok, system defaults set.  Load the particle list with the
			// ..initial particles to be used for the spiral arms
			SetupImplodeSpiralArm(nHandle);
			break;
		}
	default:
		return RGF_FAILURE;						// Hmm, try that again!
	}

	*psHandle = nHandle;						// Back to caller

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// Pause
//
// Pause and unpause a particle systems execution.  If there are particles in the
// system, they will STILL RENDER, but time will not advance for the system.
/* ------------------------------------------------------------------------------------ */
int CParticleSystem::Pause(int psHandle, bool bPause)
{
	if(theList[psHandle] == NULL)
		return RGF_FAILURE;						// Bad parm, dude!

	theList[psHandle]->bRunning = !bPause;		// Set state

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// Hide
//
// Hide and unhide a particle systems execution.  If there are particles in the
// system, they will STILL HAVE TIME ADVANCE, but they will NOT RENDER.
/* ------------------------------------------------------------------------------------ */
int CParticleSystem::Hide(int psHandle, bool bShowIt)
{
	if(theList[psHandle] == NULL)
		return RGF_FAILURE;						// Bad parm, dude!

	theList[psHandle]->bVisible = !bShowIt;		// Set state

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// Rewind
//
// Clear out a particle systems particles and start it all over again.
// ..The system is reloaded, and set to NOT RUNNING and HIDDEN.
/* ------------------------------------------------------------------------------------ */
int CParticleSystem::Rewind(int psHandle)
{
	if(theList[psHandle] == NULL)
		return RGF_FAILURE;							// Not in operation

	theList[psHandle]->bRunning = false;			// Shut it off
	theList[psHandle]->bVisible = false;			// Hide it for now

	Clear(psHandle);								// Clean it up

	switch(theList[psHandle]->nType)
	{
	case kPSystem_Shockwave:
		// Reset particle system defaults
		theList[psHandle]->Lifetime = 5000.0f;		// Seconds of life, per particle
		theList[psHandle]->SystemLife = 5000.0f;	// Whole system life
		// Reload the particles
		SetupShockwave(psHandle);
		break;
	case kPSystem_Fountain:
		// Set particle system defaults
		theList[psHandle]->Lifetime = 5000.0f;		// Seconds of life, per particle
		theList[psHandle]->SystemLife = 22000.0f;	// Whole system life
		// Reload the particles
		SetupFountain(psHandle);
		break;
	case kPSystem_Rain:
		// Set particle system defaults
		theList[psHandle]->Lifetime = 5000.0f;		// Seconds of life, per particle
		theList[psHandle]->SystemLife = 30000.0f;	// Whole system life
		SetupRain(psHandle);
		break;
	case kPSystem_Sphere:
		// Set particle system defaults
		theList[psHandle]->Lifetime = 5000.0f;		// Seconds of life, per particle
		theList[psHandle]->SystemLife = 30000.0f;	// Whole system life
		SetupSphere(psHandle);
		break;
	case kPSystem_Column:
		// Set particle system defaults
		theList[psHandle]->Lifetime = 6000.0f;		// Seconds of life, per particle
		theList[psHandle]->SystemLife = 16000.0f;	// Whole system life
		SetupRain(psHandle);
		break;
	case kPSystem_ExplosiveArray:
		// Set particle system defaults
		theList[psHandle]->Lifetime = 4000.0f;		// Seconds of life, per particle
		theList[psHandle]->SystemLife = 8000.0f;	// Whole system life
		SetupRain(psHandle);
		break;
	case kPSystem_SpiralArm:
		// Reset particle system defaults
		theList[psHandle]->Lifetime = 4000.0f;		// Seconds of life, per particle
		theList[psHandle]->SystemLife = 12000.0f;	// Whole system life
		// Reload the particles
		SetupSpiralArm(psHandle);
		break;
	case kPSystem_Trail:
		// Set particle system defaults
		theList[psHandle]->Lifetime = 6000.0f;		// Seconds of life, per particle
		theList[psHandle]->SystemLife = 12000.0f;	// Whole system life
		SetupTrail(psHandle);
		break;
	case kPSystem_Guardian:
		// Set particle system defaults
		theList[psHandle]->Lifetime = 6000.0f;		// Seconds of life, per particle
		theList[psHandle]->SystemLife = 20000.0f;	// Whole system life
		SetupGuardian(psHandle);
		break;
	case kPSystem_ImplodeSphere:
		// Set particle system defaults
		theList[psHandle]->Lifetime = 5000.0f;		// Seconds of life, per particle
		theList[psHandle]->SystemLife = 30000.0f;	// Whole system life
		SetupImplodeSphere(psHandle);
		break;
	case kPSystem_ImplodeShockwave:
		// Reset particle system defaults
		theList[psHandle]->Lifetime = 5000.0f;		// Seconds of life, per particle
		theList[psHandle]->SystemLife = 5000.0f;	// Whole system life
		// Reload the particles
		SetupImplodeShockwave(psHandle);
		break;
	case kPSystem_ImplodeSpiralArm:
		// Reset particle system defaults
		theList[psHandle]->Lifetime = 4000.0f;		// Seconds of life, per particle
		theList[psHandle]->SystemLife = 12000.0f;	// Whole system life
		// Reload the particles
		SetupImplodeSpiralArm(psHandle);
		break;
	default:
		return RGF_FAILURE;							// Hmm, try that again!
	}

	return RGF_SUCCESS;								// System ready to run again
}


/* ------------------------------------------------------------------------------------ */
// Destroy
//
// Kill a particle system, shutting it down and freeing it up.
/* ------------------------------------------------------------------------------------ */
int CParticleSystem::Destroy(int psHandle)
{
	if(theList[psHandle] == NULL)
		return RGF_FAILURE;						// No such

	Remove(psHandle);							// Gun the sucker!

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// SetColor
//
// Change the base particle color for the indicated particle system.
// ..If bChangeExisting is true, _ALL_ particles in the system will change color.
/* ------------------------------------------------------------------------------------ */
int CParticleSystem::SetColor(int psHandle, const GE_RGBA &clrColor, bool bChangeExisting)
{
	if(theList[psHandle] == NULL)
		return RGF_FAILURE;						// Try again, luser!

	theList[psHandle]->Color = clrColor;		// Color now set.

	if(bChangeExisting)
	{
		// Go through the particle list and change their current color,
		// ..so that on the next render pass all particles will change!
		theParticle *pPtr = theList[psHandle]->Particles;

		while(pPtr != NULL)
		{
			pPtr->Vertex.r = clrColor.r;
			pPtr->Vertex.g = clrColor.g;
			pPtr->Vertex.b = clrColor.b;
			pPtr->Vertex.a = clrColor.a;
			pPtr = pPtr->Next;
		}
	}

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// SetEmitter
//
// Change the point of emission for new particles.  This is useful
// ..for particles that are used as projectile trails, spell effect trails, etc.
/* ------------------------------------------------------------------------------------ */
int CParticleSystem::SetEmitter(int psHandle, const geVec3d &emitterPos)
{
	if(theList[psHandle] == NULL)
		return RGF_FAILURE;						// Try again, luser!

	theList[psHandle]->Emitter = emitterPos;

	return RGF_SUCCESS;							// New particles come from new place!
}


/* ------------------------------------------------------------------------------------ */
// SetTexture
//
// Change the texture used for the indicated particle system.  If
// ..bChangeExisting is true, _ALL_ particles in the system will change texture.
/* ------------------------------------------------------------------------------------ */
int CParticleSystem::SetTexture(int psHandle, int pmHandle,	bool bChangeExisting)
{
	if(theList[psHandle] == NULL)
		return RGF_FAILURE;						// Try again, luser!

	theList[psHandle]->Texture = bmpList[pmHandle];

	if(bChangeExisting)
	{
		// Go through the particle list and change their current texture,
		// ..so that on the next render pass all particles will change!
		theParticle *pPtr = theList[psHandle]->Particles;

		while(pPtr != NULL)
		{
			pPtr->Texture = bmpList[pmHandle];
			pPtr = pPtr->Next;
		}
	}

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// SetVelocity
//
// Change the velocity for all new particles in the system indicated.
// ..If bChangeExisting is true, change the velocity for all current
// ..particles as well.
/* ------------------------------------------------------------------------------------ */
int CParticleSystem::SetVelocity(int psHandle, float fVelocity,	bool bChangeExisting)
{
	if(theList[psHandle] == NULL)
		return RGF_FAILURE;						// Try again, luser!

	theList[psHandle]->InitialVelocity = fVelocity;

	if(bChangeExisting)
	{
		// Go through the particle list and change their current velocity,
		// ..so that on the next render pass all particles will change!
		theParticle *pPtr = theList[psHandle]->Particles;

		while(pPtr != NULL)
		{
			pPtr->Velocity = fVelocity;
			pPtr = pPtr->Next;
		}
	}

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// SetParticleLife
//
// Set the lifetime (in milliseconds) for all new particles emitted
// ..by the indicated particle system.  Existing particles are NOT	effected.
/* ------------------------------------------------------------------------------------ */
int CParticleSystem::SetParticleLife(int psHandle, int nLifeInMsec)
{
	if(theList[psHandle] == NULL)
		return RGF_FAILURE;						// Try again, luser!

	theList[psHandle]->Lifetime = static_cast<float>(nLifeInMsec);

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// SetEffectRadius
//
// Set the maximum radius of an area-based effect.  Not used in
// ..all systems, mainly Rain and Explosive Array.
/* ------------------------------------------------------------------------------------ */
int CParticleSystem::SetEffectRadius(int psHandle, float fRadius)
{
	if(theList[psHandle] == NULL)
		return RGF_FAILURE;							// Rotten code!

	theList[psHandle]->EffectRadius = fRadius;

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// SetSystemLife
//
// Set the lifetime of the particle system, in milliseconds.  If the
// ..system is running the lifetime will be reset to this value.  Note
// ..that passing in a NEGATIVE NUMBER makes this system ETERNAL, that
// ..is, it runs until stopped by a function call.
/* ------------------------------------------------------------------------------------ */
int CParticleSystem::SetSystemLife(int psHandle, int nLife)
{
	if(theList[psHandle] == NULL)
		return RGF_FAILURE;							// Rotten code!

	theList[psHandle]->SystemLife = static_cast<float>(nLife);

	if(nLife < 0)
		theList[psHandle]->bForever = true;
	else
		theList[psHandle]->bForever = false;

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// SetBirthRate
//
// Set the particle birth rate per second for the desired system.  A
// ..birth rate of zero means no new particles get added EVER.
/* ------------------------------------------------------------------------------------ */
int CParticleSystem::SetBirthRate(int psHandle, int nBirthRate)
{
	if(theList[psHandle] == NULL)
		return RGF_FAILURE;							// Rotten code!

	theList[psHandle]->SpawnSpeed = nBirthRate;

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// SetMaxSize
//
// Sets the maximum number of particles that can exist concurrently
// ..in the desired particle system.
/* ------------------------------------------------------------------------------------ */
int CParticleSystem::SetMaxSize(int psHandle, int nMaxParticles)
{
	if(theList[psHandle] == NULL)
		return RGF_FAILURE;							// Rotten code!

	theList[psHandle]->Maximum = nMaxParticles;

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// Tick
//
// Increment time for all the currently running particle systems.
/* ------------------------------------------------------------------------------------ */
void CParticleSystem::Tick(geFloat dwTicks)
{
	for(int nTemp=0; nTemp<50; ++nTemp)
	{
		if(theList[nTemp] != NULL)
			Sweep(nTemp, dwTicks);					// Update running particle systems
	}

	// Ok, go through all the proxies and update the 3D audio positioning
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "ParticleSystemProxy");

	if(!pSet)
		return;

	geEntity *pEntity;

	// Ok, we have particle system proxies somewhere.  Dig through 'em all and
	// ..update the audio (if any).
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		ParticleSystemProxy *pProxy = static_cast<ParticleSystemProxy*>(geEntity_GetUserData(pEntity));

		if(!EffectC_IsStringNull(pProxy->TriggerName))
		{
			if(GetTriggerState(pProxy->TriggerName))
			{
				if(pProxy->active == GE_FALSE)
				{
					Hide(pProxy->psHandle, false);
					Pause(pProxy->psHandle, false);

					if(!EffectC_IsStringNull(pProxy->szSoundFile))
						pProxy->effect = CreateSound(pProxy->origin, pProxy->szSoundFile, pProxy->fRadius);

					pProxy->active = GE_TRUE;
				}
			}
			else
			{
				if(pProxy->effect != -1)
					CCD->EffectManager()->Item_Delete(EFF_SND, pProxy->effect);

				Hide(pProxy->psHandle, true);
				Pause(pProxy->psHandle, true);
				Clear(pProxy->psHandle);
				pProxy->active = GE_FALSE;
			}
		}
		else
		{
			if(!EffectC_IsStringNull(pProxy->szSoundFile))
				pProxy->effect = CreateSound(pProxy->origin, pProxy->szSoundFile, pProxy->fRadius);
		}

		if(pProxy->active == GE_TRUE)
		{
			pProxy->origin = pProxy->OriginOffset;
			SetOriginOffset(pProxy->EntityName, pProxy->BoneName, pProxy->Model, &(pProxy->origin));

			if(pProxy->effect != -1)
			{
				Snd Sound;
				geVec3d_Copy(&(pProxy->origin), &(Sound.Pos));
				CCD->EffectManager()->Item_Modify(EFF_SND, pProxy->effect, static_cast<void*>(&Sound), SND_POS);
			}

			SetEmitter(pProxy->psHandle, pProxy->origin);
		}
	}
}


/* ------------------------------------------------------------------------------------ */
// Render
//
// Render all active and visible particle systems.
/* ------------------------------------------------------------------------------------ */
int CParticleSystem::Render()
{
	for(int nTemp=0; nTemp<50; ++nTemp)
	{
		if(theList[nTemp] != NULL)
		{
			// Particle system in slot, render it!
			RenderSystem(nTemp, CCD->Player()->Position());
		}
	}

	return RGF_SUCCESS;
}


// ************* PRIVATE MEMBER FUNCTIONS ****************

/* ------------------------------------------------------------------------------------ */
// Allocate
//
// Locate a free entry in the particle system table and allocate it,
// ..setting it to an EMPTY system.  If no free slots are available,
// ..return an invalid handle.
/* ------------------------------------------------------------------------------------ */
int CParticleSystem::Allocate()
{
	for(int nTemp=0; nTemp<50; ++nTemp)
	{
		if(theList[nTemp] == NULL)
		{
			// Free slot!  Fill 'er up...
			theList[nTemp] = new ParticleSystem;
			memset(theList[nTemp], 0, sizeof(ParticleSystem));
			return nTemp;
		}
	}

	// No free entries, error back to caller.
	return -1;
}


/* ------------------------------------------------------------------------------------ */
// Remove
//
// Remove a particle system from the particle system table.
/* ------------------------------------------------------------------------------------ */
int CParticleSystem::Remove(int nHandle)
{
	Clear(nHandle);

	// Particles zapped, gun the control structure
	delete theList[nHandle];
	theList[nHandle] = NULL;

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// Clear
//
// Clear out all particles associated with a particle system
/* ------------------------------------------------------------------------------------ */
int CParticleSystem::Clear(int nHandle)
{
	theParticle *myParticle = theList[nHandle]->Particles;
	theParticle *pTemp = myParticle;

	// If there are particles in the list, zap 'em.
	while(myParticle != NULL)
	{
		pTemp = myParticle->Next;
		delete myParticle;
		myParticle = pTemp;
	}

	theList[nHandle]->Particles = NULL;			// No more particles!
	theList[nHandle]->Count = 0;				// Say so, then.

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// Spawn
//
// Add a new particle into the system indicated by the handle.  Return
// ..a pointer to the new particle entity.
/* ------------------------------------------------------------------------------------ */
theParticle *CParticleSystem::Spawn(int nHandle, const GE_RGBA &theColor,
									const geVec3d &Pos,	const geVec3d &Rotation,
									float fVelocity)
{
	theParticle *theNewParticle = new theParticle;

	if(theNewParticle == NULL)
	{
		CCD->ReportError("[WARNING] Particle Spawn: out of memory", false);
		return NULL;								// Uh-oh, major problem.
	}

	// Ok, new particle around, let's clean it up and insert it.
	memset(theNewParticle, 0, sizeof(theParticle));

	theNewParticle->LifeTime = theList[nHandle]->Lifetime;
	theNewParticle->Velocity = fVelocity;			// Initial velocity
	theNewParticle->Rotation = Rotation;			// Set rotation
	theNewParticle->Texture = theList[nHandle]->Texture;

	// Set the particle color
	theNewParticle->Vertex.r = theColor.r;
	theNewParticle->Vertex.g = theColor.g;
	theNewParticle->Vertex.b = theColor.b;
	theNewParticle->Vertex.a = 255.0f;

	// Default UV data
	theNewParticle->Vertex.u = 0.0f;
	theNewParticle->Vertex.v = 0.0f;

	// Set the particle start position
	theNewParticle->Vertex.X = Pos.X;
	theNewParticle->Vertex.Y = Pos.Y;
	theNewParticle->Vertex.Z = Pos.Z;

	// Patch empty particle into the particle list
	if(theList[nHandle]->Particles != NULL)
	{
		theNewParticle->Next = theList[nHandle]->Particles;
		theNewParticle->Prev = NULL;
		theList[nHandle]->Particles->Prev = theNewParticle;
		theList[nHandle]->Particles = theNewParticle;
	}
	else
	{
		// Head of list.
		theList[nHandle]->Particles = theNewParticle;
		theNewParticle->Next = theNewParticle->Prev = NULL;
	}

	return theNewParticle;		// New particle back to caller
}


/* ------------------------------------------------------------------------------------ */
// ReverseSpawn
//
// Add a new particle into the system indicated by the handle.  Return
// ..a pointer to the new particle entity.  Note that this version of
// ..Spawn() creates a particle at the furthest point that it would
// ..travel in it's lifetime, with a REVERSE velocity such that it
// ..travels from the outer limit towards the origin.
/* ------------------------------------------------------------------------------------ */
theParticle *CParticleSystem::ReverseSpawn(int nHandle, const GE_RGBA &theColor,
										   const geVec3d &Pos, const geVec3d &Rotation,
										   float fVelocity)
{
	theParticle *theNewParticle = new theParticle;
	geVec3d NewPosition, In;
	geXForm3d Xform;

	if(theNewParticle == NULL)
	{
		CCD->ReportError("[WARNING] Reverse Particle Spawn: out of memory", false);
		return NULL;								// Uh-oh, major problem.
	}

	// Ok, new particle around, let's clean it up and insert it.
	memset(theNewParticle, 0, sizeof(theParticle));

	theNewParticle->LifeTime = theList[nHandle]->Lifetime;
	theNewParticle->Velocity = -fVelocity;	// Initial velocity
	theNewParticle->Rotation = Rotation;
	theNewParticle->Texture = theList[nHandle]->Texture;

	// Set the particle color
	theNewParticle->Vertex.r = theColor.r;
	theNewParticle->Vertex.g = theColor.g;
	theNewParticle->Vertex.b = theColor.b;
	theNewParticle->Vertex.a = 255.0f;

	// Default UV data
	theNewParticle->Vertex.u = 0.0f;
	theNewParticle->Vertex.v = 0.0f;

	// Ok, move it out to it's "real" start position
	geXForm3d_SetZRotation(&Xform, Rotation.Z);
	geXForm3d_RotateX(&Xform, Rotation.X);
	geXForm3d_RotateY(&Xform, Rotation.Y);

	// Get particle forward vector
	geXForm3d_GetIn(&Xform, &In);
	// Move particle based on the direction it's "facing"
	geVec3d_AddScaled(&Pos, &In, fVelocity*theNewParticle->LifeTime/30.f, &NewPosition);

	theNewParticle->Vertex.X = NewPosition.X;
	theNewParticle->Vertex.Y = NewPosition.Y;
	theNewParticle->Vertex.Z = NewPosition.Z;

	// Patch empty particle into the particle list
	if(theList[nHandle]->Particles != NULL)
	{
		theNewParticle->Next = theList[nHandle]->Particles;
		theNewParticle->Prev = NULL;
		theList[nHandle]->Particles->Prev = theNewParticle;
		theList[nHandle]->Particles = theNewParticle;
	}
	else
	{
		// Head of list.
		theList[nHandle]->Particles = theNewParticle;
		theNewParticle->Next = theNewParticle->Prev = NULL;
	}

	return theNewParticle;		// New particle back to caller
}


/* ------------------------------------------------------------------------------------ */
// Sweep
//
// Pass through all particles in a particle system, updating each
// ..particles lifespan, velocity, etc. and removing any particles
// ..that expire.
/* ------------------------------------------------------------------------------------ */
void CParticleSystem::Sweep(int nHandle, float dwMsec)
{
	theParticle *myParticle = theList[nHandle]->Particles;
	geXForm3d Xform;
	geVec3d In, OldPosition, NewPosition;
	bool bListHead;

	if(theList[nHandle]->bRunning == false)
		return;								// Time doesn't pass for paused system.

	theList[nHandle]->SystemLife -= dwMsec;		// System running down?

	// If out of time, and doesn't live forever, KILL THE MUTHA DEAD!
	if((theList[nHandle]->SystemLife <= 0.0f) &&
	   (theList[nHandle]->bForever == false))
	{
		Clear(nHandle);										// System out of time
		theList[nHandle]->SystemLife = 0.0f;				// Clamp to zero
		theList[nHandle]->bRunning = false;					// Stop time for empty system
		theList[nHandle]->bVisible = false;					// And hide it
		return;
	}

	// Process time for each particle in the system.
	while(myParticle != NULL)
	{
		myParticle->LifeTime -= dwMsec;						// Particles die eventually

		if(myParticle->LifeTime <= 0.0f)
		{
			// Kill off this particle, died of old age
			if(myParticle->Prev != NULL)
				(myParticle->Prev)->Next = myParticle->Next;// Drop from list

			if(myParticle->Next != NULL)
				(myParticle->Next)->Prev = myParticle->Prev;// Patch back-links

			theParticle *pTemp = myParticle->Next;

			if(myParticle->Prev == NULL)
				bListHead = true;							// Deleting list head!
			else
				bListHead = false;

			delete myParticle;								// _ZAP_

			myParticle = pTemp;								// Skip to next one

			if(bListHead)
				theList[nHandle]->Particles = pTemp;

			theList[nHandle]->Count--;						// One less particle around

			continue;
		}

		// Particle still lives, apply gravity, etc. if needed
		OldPosition.X = myParticle->Vertex.X;
		OldPosition.Y = myParticle->Vertex.Y;
		OldPosition.Z = myParticle->Vertex.Z;

		geXForm3d_SetZRotation(&Xform, myParticle->Rotation.Z);
		geXForm3d_RotateX(&Xform, myParticle->Rotation.X);
		geXForm3d_RotateY(&Xform, myParticle->Rotation.Y);

		// Get particle forward vector
		geXForm3d_GetIn(&Xform, &In);
		// Move particle based on the direction it's "facing"
		geVec3d_AddScaled(&OldPosition, &In, myParticle->Velocity, &NewPosition);

		myParticle->Vertex.X = NewPosition.X;
		myParticle->Vertex.Y = NewPosition.Y;
		myParticle->Vertex.Z = NewPosition.Z;
		myParticle->Pos = NewPosition;						// Save off position!!

		// Apply gravity, if there IS any
		ApplyGravity(nHandle, myParticle);

		// Decay the alpha of the particle based on it's life
		myParticle->Vertex.a -= theList[nHandle]->AlphaDecay * dwMsec;
		myParticle = myParticle->Next;						// Next victim!
	}

	// Ok, check to see if we need to fill in for dead particles.
	if(theList[nHandle]->SpawnSpeed != 0)
	{
		// Ok, we're allowed to spawn additional particles to fill in from
		// ..those that have Dropped Down Dead.
		float fTemp = static_cast<float>(theList[nHandle]->SpawnSpeed) * 0.001f * dwMsec;
		int nNewCount = static_cast<int>(fTemp);

		if(nNewCount == 0)
			nNewCount = 1;									// Check for at least 1 new birth

		if(nNewCount > (theList[nHandle]->Maximum - theList[nHandle]->Count))
			nNewCount = (theList[nHandle]->Maximum - theList[nHandle]->Count);

		switch(theList[nHandle]->nType)
		{
		case kPSystem_Shockwave:
			if(theList[nHandle]->Count==0)
				for(nNewCount=theList[nHandle]->Maximum; nNewCount>0; --nNewCount)
					AddShockwaveParticle(nHandle);
			break;
		case kPSystem_Fountain:
			for(; nNewCount>0; --nNewCount)
				AddFountainParticle(nHandle);
			break;
		case kPSystem_Rain:
			for(; nNewCount>0; --nNewCount)
				AddRainParticle(nHandle);
			break;
		case kPSystem_Sphere:
			for(; nNewCount>0; --nNewCount)
				AddSphereParticle(nHandle);
			break;
		case kPSystem_Column:
			for(; nNewCount>0; --nNewCount)
				AddRainParticle(nHandle);
			break;
		case kPSystem_ExplosiveArray:
			for(; nNewCount>0; --nNewCount)
				AddRainParticle(nHandle);
			break;
		case kPSystem_SpiralArm:
			for(; nNewCount>0; --nNewCount)
				AddSpiralArmParticle(nHandle);
			break;
		case kPSystem_Trail:
			for(; nNewCount>0; --nNewCount)
				AddTrailParticle(nHandle);
			break;
		case kPSystem_Guardian:
			for(; nNewCount>0; --nNewCount)
				AddGuardianParticle(nHandle);
			break;
		case kPSystem_ImplodeSphere:
			for(; nNewCount>0; --nNewCount)
				AddImplodeSphereParticle(nHandle);
			break;
		case kPSystem_ImplodeShockwave:
			if(theList[nHandle]->Count==0)
				for(nNewCount=theList[nHandle]->Maximum; nNewCount>0; --nNewCount)
					AddImplodeShockwaveParticle(nHandle);
			break;
		case kPSystem_ImplodeSpiralArm:
			for(; nNewCount>0; --nNewCount)
				AddImplodeSpiralArmParticle(nHandle);
			break;
		}
	}
}


/* ------------------------------------------------------------------------------------ */
// ApplyGravity
//
// Apply the effect of gravity, if there is gravity, to a particle.
/* ------------------------------------------------------------------------------------ */
void CParticleSystem::ApplyGravity(int nHandle, theParticle *myParticle)
{
	if(theList[nHandle] == NULL)
		return;								// Nothing to do.

	if(myParticle == NULL)
		return;								// Nothing to do it to.

	if(theList[nHandle]->Gravity == 0.0f)
		return;								// Nothing to do it with.

	geVec3d OldPosition, NewPosition; //, In, Up;
	geVec3d Up = {0.0f, 1.0f, 0.0f};

	OldPosition.X = myParticle->Vertex.X;
	OldPosition.Y = myParticle->Vertex.Y;
	OldPosition.Z = myParticle->Vertex.Z;

	// Gravity always pulls towards negative Y in this G3D 1.0 implementation
	geVec3d_AddScaled(&OldPosition, &Up, theList[nHandle]->Gravity, &NewPosition);

	myParticle->Vertex.X = NewPosition.X;
	myParticle->Vertex.Y = NewPosition.Y;
	myParticle->Vertex.Z = NewPosition.Z;
}


/* ------------------------------------------------------------------------------------ */
// RenderSystem
//
// Render the particle system in the appropriate slot.  Only render the
// ..system if the viewpoint of the player is close enough to be able to
// ..see the system, which means within about 600.0f units.
/* ------------------------------------------------------------------------------------ */
void CParticleSystem::RenderSystem(int nHandle, const geVec3d & /*PlayerPos*/)
{
	theParticle *myParticle = theList[nHandle]->Particles;

	if(myParticle == NULL)
		return;									// No particles, don't bother

	// We're in range and have particles.  Let's render 'em.
	while(myParticle != NULL)
	{
		// If the particle system isn't invisible, add the polygons for this
		// ..frame.
		if(theList[nHandle]->bVisible)
		{
			long nLeafID;
			geWorld_GetLeaf(CCD->World(), &(myParticle->Pos), &nLeafID);

			// If the BSP leaf the particle is in might be visible,
			// ..go ahead and add it.
			if(geWorld_MightSeeLeaf(CCD->World(), nLeafID) == GE_TRUE)
			{
				geWorld_AddPolyOnce(CCD->World(), &myParticle->Vertex, 1,
					theList[nHandle]->Texture, GE_TEXTURED_POINT,
					GE_RENDER_DEPTH_SORT_BF | GE_RENDER_DO_NOT_OCCLUDE_OTHERS,
					theList[nHandle]->Scale);
			}
		}

		myParticle = myParticle->Next;
	}
}


// ***************************************************************************************
// **				IMPLEMENTATION OF SYSTEM-TYPE SPECIFIC ROUTINES						**
// ***************************************************************************************

/* ------------------------------------------------------------------------------------ */
// SetupShockwave
//
// Load the particle list for the specified system indicated by
// ..'nHandle' with shockwave particles.
/* ------------------------------------------------------------------------------------ */
void CParticleSystem::SetupShockwave(int nHandle)
{
	// all particles at once for a shockwave
	for(int nFoo=0; nFoo<theList[nHandle]->Maximum; ++nFoo)
		AddShockwaveParticle(nHandle);

	// Particle alpha decay speed
	theList[nHandle]->AlphaDecay = 255.0f / theList[nHandle]->Lifetime;
}


/* ------------------------------------------------------------------------------------ */
// AddShockwaveParticle
//
// Add a new shockwave particle to the desired particle system.
/* ------------------------------------------------------------------------------------ */
void CParticleSystem::AddShockwaveParticle(int nHandle)
{
	static geVec3d vRot = {0.0f, 0.0f, 0.0f};
	static bool fToggle = false;

	// X-Axis rotation 0=straight up, -90=into screen, +90=towards user
	// ..Note that vRot.X starts at 89.55 to correct for G3D orientations

	// Half move positive, half move negative, makes an expanding ring
	if(fToggle)
	{
		Spawn(nHandle, theList[nHandle]->Color, theList[nHandle]->Emitter,
				vRot, theList[nHandle]->InitialVelocity);
	}
	else
	{
		Spawn(nHandle, theList[nHandle]->Color, theList[nHandle]->Emitter,
				vRot, -(theList[nHandle]->InitialVelocity));
	}

	vRot.Y += GE_2PI/theList[nHandle]->Maximum;	// Increment to fan across horizontal plane

	if(vRot.Y > GE_2PI)
		vRot.Y -= GE_2PI;			// Wrap around

	fToggle = !fToggle;				// Switch to opposite side next call

	theList[nHandle]->Count++;		// One more particle in the party.
}


/* ------------------------------------------------------------------------------------ */
// SetupSpiralArm
//
// Load the particle list for the specified system indicated by
// ..'nHandle' with spiral arm particles.
/* ------------------------------------------------------------------------------------ */
void CParticleSystem::SetupSpiralArm(int nHandle)
{
	// We start with less than the maximum particles and let
	// ..the fill-in process add to it, creating a nifty spiral
	// ..arm effect.

	for(int nFoo=0; nFoo<10; ++nFoo)
		AddSpiralArmParticle(nHandle);

	// Particle alpha decay speed
	theList[nHandle]->AlphaDecay = 255.0f / static_cast<float>(theList[nHandle]->Lifetime);
}


/* ------------------------------------------------------------------------------------ */
// AddSpiralArmParticle
//
// Add a new spiral arm particle to the desired particle system.
/* ------------------------------------------------------------------------------------ */
void CParticleSystem::AddSpiralArmParticle(int nHandle)
{
	static geVec3d vRot = {0.0f, 0.0f, 0.0f};
	static bool fToggle = false;

	// X-Axis rotation 0=straight up, -90=into screen, +90=towards user
	// ..Note that vRot.X starts at 89.55 to correct for G3D orientations

	// Half move positive, half move negative, makes twin opposite arms

	if(fToggle)
	{
		Spawn(nHandle, theList[nHandle]->Color, theList[nHandle]->Emitter,
				vRot, theList[nHandle]->InitialVelocity);
	}
	else
	{
		Spawn(nHandle, theList[nHandle]->Color, theList[nHandle]->Emitter,
				vRot, -(theList[nHandle]->InitialVelocity));
	}

	vRot.Y += 0.05f;				// Increment to fan across horizontal plane

	if(vRot.Y > 3.145f)
		vRot.Y = 0.0f;				// Wrap around

	fToggle = !fToggle;				// Switch to opposite side next call

	theList[nHandle]->Count++;		// One more particle in the party.
}


/* ------------------------------------------------------------------------------------ */
// SetupFountain
//
// Load the particle list for the specified system indicated by
// ..'nHandle' with fountain particles.
/* ------------------------------------------------------------------------------------ */
void CParticleSystem::SetupFountain(int nHandle)
{
	// Start off slow, let spawning fill the fountain
	for(int nFoo=0; nFoo<8; ++nFoo)
		AddFountainParticle(nHandle);

	// Particle alpha decay speed
	theList[nHandle]->AlphaDecay = 255.0f / theList[nHandle]->Lifetime;
}


/* ------------------------------------------------------------------------------------ */
// AddFountainParticle
//
// Add a new fountain particle to the desired particle system.
/* ------------------------------------------------------------------------------------ */
void CParticleSystem::AddFountainParticle(int nHandle)
{
	static geVec3d vRot = {1.0f, 0.0f, 0.0f};

	// rotations are in RADIANS, and there are 57.295 degress per radian.
	// A complete circle is, more or less, 6.28 radians.

	Spawn(nHandle, theList[nHandle]->Color, theList[nHandle]->Emitter,
			vRot, theList[nHandle]->InitialVelocity);

	vRot.X += 0.015f;				// Increment to fan across horizontal plane
	vRot.Y += 0.75f;

	if(vRot.X > 1.3f)
		vRot.X = 1.0f;				// Wrap back to start

	if(vRot.Y > 12.28f)
		vRot.Y = 0.0f;				// Wrap around. more or less

	theList[nHandle]->Count++;		// One more particle in the party.
}


/* ------------------------------------------------------------------------------------ */
// SetupRain
//
// Set up initial particles for "rain" particle effect.
/* ------------------------------------------------------------------------------------ */
void CParticleSystem::SetupRain(int nHandle)
{
	// Start off slow, let spawning make more rain
	for(int nFoo=0; nFoo<30; ++nFoo)
		AddRainParticle(nHandle);

	// Particle alpha decay speed
	theList[nHandle]->AlphaDecay = 255.0f / theList[nHandle]->Lifetime;
}


/* ------------------------------------------------------------------------------------ */
// AddRainParticle
//
// Add a new rain particle to the particle system list
/* ------------------------------------------------------------------------------------ */
void CParticleSystem::AddRainParticle(int nHandle)
{
	static geVec3d vRot = {5.5f, 0.0f, 0.0f};
	geVec3d thePos;
	int nX, nZ;

	// Perturb the particle on X and Z axes to move it away from the emitter
	thePos = theList[nHandle]->Emitter;

	srand(CCD->FreeRunningCounter());			// Spin random number generator
	nX = rand() % static_cast<int>(theList[nHandle]->EffectRadius);
	srand(CCD->FreeRunningCounter());			// Spin random number generator
	nZ = rand() % static_cast<int>(theList[nHandle]->EffectRadius);

	if(((rand()%33) & 0x01) != 0)
		thePos.X += static_cast<float>(nX);
	else
		thePos.X -= static_cast<float>(nX);

	if(((rand()%33) & 0x01) != 0)
		thePos.Z += static_cast<float>(nZ);
	else
		thePos.Z -= static_cast<float>(nZ);

	Spawn(nHandle, theList[nHandle]->Color, thePos,
			vRot, theList[nHandle]->InitialVelocity);

	theList[nHandle]->Count++;					// One more particle in the party.
}


/* ------------------------------------------------------------------------------------ */
// SetupSphere
//
// Load the particle list for the specified system indicated by
// ..'nHandle' with sphere particles.
/* ------------------------------------------------------------------------------------ */
void CParticleSystem::SetupSphere(int nHandle)
{
	// Start off with a good sphere
	for(int nFoo=0; nFoo<60; ++nFoo)
		AddSphereParticle(nHandle);

	// Particle alpha decay speed
	theList[nHandle]->AlphaDecay = 255.0f / theList[nHandle]->Lifetime;
}


/* ------------------------------------------------------------------------------------ */
// AddSphereParticle
//
// Add a new sphere particle to the desired particle system.
/* ------------------------------------------------------------------------------------ */
void CParticleSystem::AddSphereParticle(int nHandle)
{
	static geVec3d vRot = {0.0f, 0.0f, -1.0f};
	static bool fToggle = false;

	// X-Axis rotation 0=straight up, -90=into screen, +90=towards user
	// ..Note that vRot.X starts at 89.55 to correct for G3D orientations

	// Half move positive, half move negative, makes twin opposite domes
	if(fToggle)
	{
		Spawn(nHandle, theList[nHandle]->Color, theList[nHandle]->Emitter,
				vRot, theList[nHandle]->InitialVelocity);
	}
	else
	{
		Spawn(nHandle, theList[nHandle]->Color, theList[nHandle]->Emitter,
				vRot, -(theList[nHandle]->InitialVelocity));
	}

	vRot.Y += 0.05f;				// Increment to fan across horizontal plane

	if(vRot.Y > 12.24f)
		vRot.Y = 0.0f;				// Wrap around

	vRot.X += 0.7f;

	if(vRot.X > 12.24f)
		vRot.X = 0.0f;				// Wrap around

	fToggle = !fToggle;				// Switch to opposite side next call

	theList[nHandle]->Count++;		// One more particle in the party.
}


/* ------------------------------------------------------------------------------------ */
// SetupTrail
//
// Load initial trail particles.
/* ------------------------------------------------------------------------------------ */
void CParticleSystem::SetupTrail(int nHandle)
{
	for(int nTemp=0; nTemp<8; ++nTemp)
		AddTrailParticle(nHandle);

	// Particle alpha decay speed
	theList[nHandle]->AlphaDecay = 255.0f / theList[nHandle]->Lifetime;
}


/* ------------------------------------------------------------------------------------ */
// AddTrailParticle
//
// Add a particle as part of a particle trail.  Trail particles have
// ..a very slight gravity effect, a very low velocity, and a more or
// ..less random rotation to emulate drifting particles left by, say,
// ..a rocket or other projectile.
/* ------------------------------------------------------------------------------------ */
void CParticleSystem::AddTrailParticle(int nHandle)
{
	static geVec3d vRot = {0.0f, 0.0f, 0.0f};
	int nX, nY, nZ;

	// Perturb the particle rotation on all axes to fake Brownian motion
	srand(CCD->FreeRunningCounter());			// Spin random number generator
	nX = rand() % static_cast<int>(theList[nHandle]->EffectRadius);

	srand(CCD->FreeRunningCounter());			// Spin random number generator
	nY = rand() % static_cast<int>(theList[nHandle]->EffectRadius);

	srand(CCD->FreeRunningCounter());			// Spin random number generator
	nZ = rand() % static_cast<int>(theList[nHandle]->EffectRadius);

	if(((rand()%33) & 0x01) != 0)
		vRot.X += static_cast<float>(nX);
	else
		vRot.X -= static_cast<float>(nX);

	if(((rand()%33) & 0x01) != 0)
		vRot.Y += static_cast<float>(nY);
	else
		vRot.Y -= static_cast<float>(nY);

	if(((rand()%33) & 0x01) != 0)
		vRot.Z += static_cast<float>(nZ);
	else
		vRot.Z -= static_cast<float>(nZ);

	Spawn(nHandle, theList[nHandle]->Color, theList[nHandle]->Emitter,
			vRot, theList[nHandle]->InitialVelocity);

	theList[nHandle]->Count++;					// One more particle in the party.
}


/* ------------------------------------------------------------------------------------ */
// SetupGuardian
//
// Load the particle list for the specified system indicated by
// ..'nHandle' with 'guardian effect' particles.
/* ------------------------------------------------------------------------------------ */
void CParticleSystem::SetupGuardian(int nHandle)
{
	// Start off with a dense guardian
	for(int nFoo=0; nFoo<80; ++nFoo)
		AddGuardianParticle(nHandle);

	// Particle alpha decay speed
	theList[nHandle]->AlphaDecay = 255.0f / theList[nHandle]->Lifetime;
}


/* ------------------------------------------------------------------------------------ */
// AddGuardianParticle
//
// Add a new 'guardian effect' particle to the desired particle system.
/* ------------------------------------------------------------------------------------ */
void CParticleSystem::AddGuardianParticle(int nHandle)
{
	static geVec3d vRot = {0.0f, 0.0f, -1.0f};
	static bool fToggle = false;

	// X-Axis rotation 0=straight up, -90=into screen, +90=towards user
	// ..Note that vRot.X starts at 89.55 to correct for G3D orientations

	// Half move positive, half move negative, makes twin opposite domes
	if(fToggle)
	{
		Spawn(nHandle, theList[nHandle]->Color, theList[nHandle]->Emitter,
				vRot, theList[nHandle]->InitialVelocity);
	}
	else
	{
		Spawn(nHandle, theList[nHandle]->Color, theList[nHandle]->Emitter,
				vRot, -(theList[nHandle]->InitialVelocity));
	}

	vRot.Y += 0.05f;				// Increment to fan across horizontal plane

	if(vRot.Y > 6.24f)
		vRot.Y = 0.0f;				// Wrap around

	vRot.X += 0.05f;

	if(vRot.X > 6.24f)
		vRot.X = 0.0f;				// Wrap around

	fToggle = !fToggle;				// Switch to opposite side next call

	theList[nHandle]->Count++;		// One more particle in the party.
}


/* ------------------------------------------------------------------------------------ */
// SetupImplodeSphere
//
// Load the particle list for the specified system indicated by
// ..'nHandle' with imploding sphere particles.
/* ------------------------------------------------------------------------------------ */
void CParticleSystem::SetupImplodeSphere(int nHandle)
{
	// Start off with a good sphere
	for(int nFoo=0; nFoo<60; ++nFoo)
		AddImplodeSphereParticle(nHandle);

	// Particle alpha decay speed
	theList[nHandle]->AlphaDecay = 255.0f / theList[nHandle]->Lifetime;
}

/* ------------------------------------------------------------------------------------ */
// AddImplodeSphereParticle
//
// Add a new imploding sphere particle to the desired particle system.
/* ------------------------------------------------------------------------------------ */
void CParticleSystem::AddImplodeSphereParticle(int nHandle)
{
	static geVec3d vRot = {0.0f, 0.0f, -1.0f};
	static bool fToggle = false;

	// X-Axis rotation 0=straight up, -90=into screen, +90=towards user
	// ..Note that vRot.X starts at 89.55 to correct for G3D orientations

	// Half move positive, half move negative, makes twin opposite domes
	if(fToggle)
	{
		ReverseSpawn(nHandle, theList[nHandle]->Color, theList[nHandle]->Emitter,
						vRot, theList[nHandle]->InitialVelocity);
	}
	else
	{
		ReverseSpawn(nHandle, theList[nHandle]->Color, theList[nHandle]->Emitter,
						vRot, -(theList[nHandle]->InitialVelocity));
	}

	vRot.Y += 0.05f;				// Increment to fan across horizontal plane

	if(vRot.Y > 12.24f)
		vRot.Y = 0.0f;				// Wrap around

	vRot.X += 0.7f;

	if(vRot.X > 12.24f)
		vRot.X = 0.0f;				// Wrap around

	fToggle = !fToggle;				// Switch to opposite side next call

	theList[nHandle]->Count++;		// One more particle in the party.
}


/* ------------------------------------------------------------------------------------ */
// SetupImplodeShockwave
//
// Load the particle list for the specified system indicated by
// ..'nHandle' with imploding shockwave particles.
/* ------------------------------------------------------------------------------------ */
void CParticleSystem::SetupImplodeShockwave(int nHandle)
{
	// all particles at once for a shockwave
	for(int nFoo=0; nFoo<theList[nHandle]->Maximum; ++nFoo)
		AddImplodeShockwaveParticle(nHandle);

	// Particle alpha decay speed
	theList[nHandle]->AlphaDecay = 255.0f / theList[nHandle]->Lifetime;
}


/* ------------------------------------------------------------------------------------ */
// AddImplodeShockwaveParticle
//
// Add a new imploding shockwave particle to the desired particle system.
/* ------------------------------------------------------------------------------------ */
void CParticleSystem::AddImplodeShockwaveParticle(int nHandle)
{
	static geVec3d vRot = {0.0f, 0.0f, 0.0f};
	static bool fToggle = false;

	// X-Axis rotation 0=straight up, -90=into screen, +90=towards user
	// ..Note that vRot.X starts at 89.55 to correct for G3D orientations

	// Half move positive, half move negative, makes an expanding ring
	if(fToggle)
	{
		ReverseSpawn(nHandle, theList[nHandle]->Color, theList[nHandle]->Emitter,
						vRot, theList[nHandle]->InitialVelocity);
	}
	else
	{
		ReverseSpawn(nHandle, theList[nHandle]->Color, theList[nHandle]->Emitter,
						vRot, -(theList[nHandle]->InitialVelocity));
	}

	vRot.Y += GE_2PI/theList[nHandle]->Maximum;	// Increment to fan across horizontal plane

	if(vRot.Y > GE_2PI)
		vRot.Y -= GE_2PI;			// Wrap around

	fToggle = !fToggle;				// Switch to opposite side next call

	theList[nHandle]->Count++;		// One more particle in the party.
}


/* ------------------------------------------------------------------------------------ */
// SetupImplodeSpiralArm
//
// Load the particle list for the specified system indicated by
// ..'nHandle' with imploding spiral arm particles.
/* ------------------------------------------------------------------------------------ */
void CParticleSystem::SetupImplodeSpiralArm(int nHandle)
{
	// We start with less than the maximum particles and let
	// ..the fill-in process add to it, creating a nifty spiral
	// ..arm effect.
	for(int nFoo=0; nFoo<10; ++nFoo)
		AddImplodeSpiralArmParticle(nHandle);

	// Particle alpha decay speed
	theList[nHandle]->AlphaDecay = 255.0f / theList[nHandle]->Lifetime;
}


/* ------------------------------------------------------------------------------------ */
// AddImplodeSpiralArmParticle
//
// Add a new imploding spiral arm particle to the desired particle system.
/* ------------------------------------------------------------------------------------ */
void CParticleSystem::AddImplodeSpiralArmParticle(int nHandle)
{
	static geVec3d vRot = {0.0f, 0.0f, 0.0f};
	static bool fToggle = false;

	// X-Axis rotation 0=straight up, -90=into screen, +90=towards user
	// ..Note that vRot.X starts at 89.55 to correct for G3D orientations

	// Half move positive, half move negative, makes twin opposite arms
	if(fToggle)
	{
		ReverseSpawn(nHandle, theList[nHandle]->Color, theList[nHandle]->Emitter,
						vRot, theList[nHandle]->InitialVelocity);
	}
	else
	{
		ReverseSpawn(nHandle, theList[nHandle]->Color, theList[nHandle]->Emitter,
						vRot, -(theList[nHandle]->InitialVelocity));
	}

	vRot.Y += 0.05f;				// Increment to fan across horizontal plane

	if(vRot.Y > 3.145f)
		vRot.Y = 0.0f;				// Wrap around

	fToggle = !fToggle;				// Switch to opposite side next call

	theList[nHandle]->Count++;		// One more particle in the party.
}


// ******************** CRGF Overrides ********************

/* ------------------------------------------------------------------------------------ */
// LocateEntity
//
// Given a name, locate the desired item in the currently loaded level
// ..and return it's user data.
/* ------------------------------------------------------------------------------------ */
int CParticleSystem::LocateEntity(const char *szName, void **pEntityData)
{
	// Ok, check to see if there are particle system proxies in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "ParticleSystemProxy");

	if(!pSet)
		return RGF_NOT_FOUND;									// No particle system proxies

	geEntity *pEntity;

	// Ok, we have particle system proxies.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		ParticleSystemProxy *pTheEntity = static_cast<ParticleSystemProxy*>(geEntity_GetUserData(pEntity));

		if(!strcmp(pTheEntity->szEntityName, szName))
		{
			*pEntityData = static_cast<void*>(pTheEntity);
			return RGF_SUCCESS;
		}
	}

	return RGF_NOT_FOUND;								// Sorry, no such entity here
}


/* ------------------------------------------------------------------------------------ */
// ReSynchronize
//
// Correct internal timing to match current time, to make up for time lost
// ..when outside the game loop (typically in "menu mode").
/* ------------------------------------------------------------------------------------ */
int CParticleSystem::ReSynchronize()
{
	return RGF_SUCCESS;
}

/* ----------------------------------- END OF FILE ------------------------------------ */
