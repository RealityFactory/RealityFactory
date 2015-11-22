/*
CTeleporter.cpp:		Teleporter class handler

  (c) 1999 Edward A. Averill, III
  
	This file contains the class implementation for teleporter handling.
*/

#include "RabidFramework.h"

extern geSound_Def *SPool_Sound(char *SName);

//	CTeleporter
//
//	Constructor.  The way we're doing teleporters is that a teleporter
//	..manifests in the game as a ball of fog surrounding a teleport
//	..trigger.  Once the trigger is hit, the teleporter activates and
//	..the player is "instantly" teleported to the destination.

CTeleporter::CTeleporter()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	m_TeleporterCount = 0;					// No teleports
	
	//	Ok, check to see if there are teleporters in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "Teleporter");
	
	if(!pSet) 
		return;									// No teleporters
	
	//	Ok, we have teleporters somewhere.  Dig through 'em all.
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		Teleporter *pTeleporter = (Teleporter*)geEntity_GetUserData(pEntity);
		if((pTeleporter->szEntityName == NULL) || (strlen(pTeleporter->szEntityName) <= 0))
		{
			char szName[128];
			geEntity_GetName(pEntity, szName, 128);
			pTeleporter->szEntityName = szName;
		}
		m_TeleporterCount++;								// Kick teleporter count
		// Ok, put this entity into the Global Entity Registry
		CCD->EntityRegistry()->AddEntity(pTeleporter->szEntityName, "Teleporter");
		// Reset all the data for each teleporter
		pTeleporter->bActive = GE_TRUE;				// Teleporter ready
		pTeleporter->theSound = NULL;					// No sound right now
		pTeleporter->SoundHandle = -1;
		pTeleporter->theFog = NULL;						// No fog sphere...yet
		pTeleporter->bForward = true;					// Animation direction
		pTeleporter->active = false;
		if(!EffectC_IsStringNull(pTeleporter->szSoundFile))
		{
			pTeleporter->theSound=SPool_Sound(pTeleporter->szSoundFile);
			
			if(!pTeleporter->theSound)
			{
				char szError[256];
				sprintf(szError,"Can't open audio file '%s'\n", pTeleporter->szSoundFile);
				CCD->ReportError(szError, false);
			}
		}
		// Ok, now we add a FOG SPHERE around our teleport trigger.  This makes for
		// ..a neat, simple, and cheap effect - enter the fog, hit the trigger, and
		// .. *POOF* you're teleported.
		if(pTeleporter->bUseFogEffect == GE_TRUE)
		{
			pTeleporter->fDensity = kFogDensity;		// Basic teleport field density
			pTeleporter->cColor.a = 255.0f;
			pTeleporter->fDelta = pTeleporter->fogVariance / (geFloat)pTeleporter->fogSpeed;
		}
	}
	
	//	Ok, if we have teleport targets, clear out the pathfollower data
	
	pSet = geWorld_GetEntitySet(CCD->World(), "TeleportTarget");
	
	if(pSet != NULL)
	{
		for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
		{
			TeleportTarget *pTarget = (TeleportTarget*)geEntity_GetUserData(pEntity);
			pTarget->bFollower = GE_FALSE;
			CCD->EntityRegistry()->AddEntity(pTarget->Name, "TeleportTarget");
		}
	}
	
	//	Ok, we've counted the teleporters and reset 'em all to their default
	//	..values.  Leave!
	
	return;
}

//	~CTeleporter
//
//	Destructor, clean up after ourselves.

CTeleporter::~CTeleporter()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	if(m_TeleporterCount == 0)
		return;						// Don't waste CPU cycles
	
	//	Ok, check to see if there are teleporters in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "Teleporter");
	
	if(!pSet) 
		return;									// No teleporters
	
	//	Ok, we have teleporters somewhere.  Dig through 'em all and release
	//	..the audio (if any).
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		Teleporter *pTeleport = (Teleporter*)geEntity_GetUserData(pEntity);
		if(pTeleport->theSound != NULL)
			geSound_FreeSoundDef(CCD->Engine()->AudioSystem(), pTeleport->theSound);
		pTeleport->theSound = NULL;							// No sound, right now...
		if(pTeleport->theFog != NULL)
			geWorld_RemoveFog(CCD->World(), pTeleport->theFog);
		pTeleport->theFog = NULL;								// Fog cleared, heh heh
	}
	
	//	Teleporters cleaned up.  Bail this mess.
	
	return;
}

//	HandleCollision
//
//	Handle a collision with a teleport trigger.

bool CTeleporter::HandleCollision(geWorld_Model *pModel, geActor *theActor)
{
	geEntity_EntitySet *pSet, *pSet2;
	geEntity *pEntity, *pEntity2;
	
	if(m_TeleporterCount == 0)
		return false;									// None here, ignore call.
	
	pSet = geWorld_GetEntitySet(CCD->World(), "Teleporter");
	if(!pSet)
	{
		CCD->ReportError("CTeleporter: handlecollision: no teleporters",
			false);
		return false;
	}
	
	//	Once more we scan the teleporter list.  Does this get old, or what?
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		// Get the teleporter data so we can compare models
		Teleporter *pTeleport = (Teleporter*)geEntity_GetUserData(pEntity);
		if(pTeleport->Model == pModel)
		{
			if(pTeleport->bActive == GE_FALSE || pTeleport->active == false)
				return true;								// Hit, yes, something happened, no
			// Ok, now we search the list of teleport targets, looking for one
			// ..with the name we're shooting for.  Once we do, we'll relocate
			// ..our unsuspecting player to that location.
			pSet2 = geWorld_GetEntitySet(CCD->World(), "TeleportTarget");
			if(pSet2 == NULL)
			{
				CCD->ReportError("Teleporters with no targets", false);
				return true;
			}
			// Fine, there ARE targets.  Scan until we find the one we want, then
			// ..move the sucker...erm, PLAYER...there.
			for(pEntity2 = geEntity_EntitySetGetNextEntity(pSet2, NULL); pEntity2;
			pEntity2 = geEntity_EntitySetGetNextEntity(pSet2, pEntity2)) 
			{
				TeleportTarget *pTarget = (TeleportTarget*)geEntity_GetUserData(pEntity2);
				
				if(strcmp(pTeleport->Target, pTarget->Name) == 0)
				{
					// It's this one, do the teleport!
					if(pTeleport->bOneShot == GE_TRUE)
						pTeleport->bActive = GE_FALSE;			// Teleporter shut down
					if(pTeleport->theSound && pTeleport->SoundHandle==-1)
					{
						Snd Sound;
						memset( &Sound, 0, sizeof( Sound ) );
						geVec3d_Copy( &(pTeleport->origin), &( Sound.Pos ) );
						Sound.Min=kAudibleRadius;
						Sound.Loop=false;
						Sound.SoundDef = pTeleport->theSound;
						CCD->EffectManager()->Item_Add(EFF_SND, (void *)&Sound);
					}
					// Do a "teleporter" effect.  Note that this causes all
					// ..kinds of weirdness with timings if you're supposed to
					// ..appear in the air at the target end, I'm not sure WHY this
					// ..is but there it is.  However, it is a neat effect...
					if((pTeleport->bUseTeleportEffect == GE_TRUE) &&
						(pTeleport->bUseFogEffect == GE_TRUE) && pTeleport->theFog)
					{
						for(int nTemp = 1000; nTemp < 6000; nTemp += 100)
						{
							CCD->Engine()->BeginFrame();
							geFog_SetAttributes(pTeleport->theFog, &pTeleport->origin, &pTeleport->cColor,
								0.0f, (geFloat)nTemp+500, pTeleport->fogRadius+(geFloat)(nTemp/400));
							CCD->Engine()->RenderWorld();
							CCD->Engine()->EndFrame();
						}
					}
					// Ok, let's MOVE THE ACTOR
					CCD->ActorManager()->Position(theActor, pTarget->origin);
					return true;
				}
			}
			return true;									// Player is _TELEPORTED_
		}
	}
	
	return false;							// We hit no known teleporters
}

//	Tick
//
//	Increment animation times on teleporters.  This is used to vary
//	..the color of each teleporter from one point to another.  The
//	..time variance is set in the world editor.

void CTeleporter::Tick(geFloat dwTicks)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	if(m_TeleporterCount == 0)
		return;						// Don't waste CPU cycles
	
	//	First, we're going to scrounge through all the TeleportTargets
	//	..and see if any of them are bound to a motion path.
	
	pSet = geWorld_GetEntitySet(CCD->World(), "TeleportTarget");
	
	if(pSet != NULL)
	{
		for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
		{
			TeleportTarget *pTarget = (TeleportTarget*)geEntity_GetUserData(pEntity);
			if(pTarget->bFollower)
				CCD->PathFollower()->GetNextPosition(pTarget->Name, &pTarget->origin,	false);
			// Seek to next position, if in motion
		}
	}
	
	//	Ok, check to see if there are teleporters in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "Teleporter");
	
	if(!pSet) 
		return;									// No teleporters
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		Teleporter *pTeleport = (Teleporter*)geEntity_GetUserData(pEntity);
		if(!EffectC_IsStringNull(pTeleport->TriggerName))
		{
			if(GetTriggerState(pTeleport->TriggerName))
			{
				if(pTeleport->active==false)
					pTeleport->active=true;
			}
			else
			{
				if(pTeleport->active==GE_TRUE)
				{
					if(pTeleport->theFog != NULL)
						geWorld_RemoveFog(CCD->World(), pTeleport->theFog);
					pTeleport->theFog = NULL;
					pTeleport->active=false;
				}
			}
		}
		else
		{
			if(pTeleport->active==false)
				pTeleport->active=true;
		}
		if(pTeleport->active==GE_TRUE)
		{
			int32 Leaf;
			geWorld_GetLeaf(CCD->World(), &(pTeleport->origin), &(Leaf) );
			if ( EffectC_IsPointVisible(CCD->World(),
				CCD->CameraManager()->Camera(),
				&(pTeleport->origin),
				Leaf,
				EFFECTC_CLIP_LEAF ) == GE_FALSE )
			{
				if(pTeleport->theFog != NULL)
					geWorld_RemoveFog(CCD->World(), pTeleport->theFog);
				pTeleport->theFog = NULL;
			}
			else
			{
				if(pTeleport->theFog == NULL)
				{
					pTeleport->theFog = geWorld_AddFog(CCD->World());
					if(pTeleport->theFog != NULL)
						geFog_SetAttributes(pTeleport->theFog, &pTeleport->origin, &pTeleport->cColor, 0.0f,
						pTeleport->fDensity, pTeleport->fogRadius);	
				}
			}
			if((pTeleport->theFog != NULL) && (pTeleport->bActive))
			{
				// Teleport has fog and is active, animate.
				if(pTeleport->bForward)
				{
					pTeleport->fDensity += (pTeleport->fDelta * dwTicks);
					if(pTeleport->fDensity > (kFogDensity+pTeleport->fogVariance))
						pTeleport->bForward = false;			// Start decrementing next time
				}
				else
				{
					pTeleport->fDensity -= (pTeleport->fDelta * dwTicks);
					if(pTeleport->fDensity < (kFogDensity-pTeleport->fogVariance))
						pTeleport->bForward = true;			// Start incrementing next time
				}
				geFog_SetAttributes(pTeleport->theFog, &pTeleport->origin, &pTeleport->cColor, 0.0f,
					pTeleport->fDensity, pTeleport->fogRadius);			// Animate the density!
			}
		}
	}
	
	return;
}

//	SaveTo
//
//	Save all teleporters and teleport targets to the supplied file

int CTeleporter::SaveTo(FILE *SaveFD)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	if(m_TeleporterCount == 0)
		return RGF_SUCCESS;						// Don't waste CPU cycles
	
	//	Ok, check to see if there are teleporters in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "Teleporter");
	
	if(!pSet) 
		return RGF_SUCCESS;									// No teleporters
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		Teleporter *pTeleport = (Teleporter*)geEntity_GetUserData(pEntity);
		fwrite(&pTeleport->bActive, sizeof(geBoolean), 1, SaveFD);
		fwrite(&pTeleport->bForward, sizeof(geBoolean), 1, SaveFD);
		fwrite(&pTeleport->fDelta, sizeof(geFloat), 1, SaveFD);
		fwrite(&pTeleport->fDensity, sizeof(geFloat), 1, SaveFD);
		fwrite(&pTeleport->origin, sizeof(geVec3d), 1, SaveFD);
	}
	
	pSet = geWorld_GetEntitySet(CCD->World(), "TeleportTarget");
	
	if(!pSet) 
		return RGF_SUCCESS;									// No targets
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		TeleportTarget *pTarget = (TeleportTarget*)geEntity_GetUserData(pEntity);
		fwrite(&pTarget->origin, sizeof(geVec3d), 1, SaveFD);
	}
	
	return RGF_SUCCESS;
}

//	RestoreFrom
//
//	Restore all teleporters and teleport targets from the supplied file

int CTeleporter::RestoreFrom(FILE *RestoreFD)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	if(m_TeleporterCount == 0)
		return RGF_SUCCESS;						// Don't waste CPU cycles
	
	//	Ok, check to see if there are teleporters in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "Teleporter");
	
	if(!pSet) 
		return RGF_SUCCESS;									// No teleporters
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		Teleporter *pTeleport = (Teleporter*)geEntity_GetUserData(pEntity);
		fread(&pTeleport->bActive, sizeof(geBoolean), 1, RestoreFD);
		fread(&pTeleport->bForward, sizeof(geBoolean), 1, RestoreFD);
		fread(&pTeleport->fDelta, sizeof(geFloat), 1, RestoreFD);
		fread(&pTeleport->fDensity, sizeof(geFloat), 1, RestoreFD);
		fread(&pTeleport->origin, sizeof(geVec3d), 1, RestoreFD);
	}
	
	pSet = geWorld_GetEntitySet(CCD->World(), "TeleportTarget");
	
	if(!pSet) 
		return RGF_SUCCESS;									// No targets
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		TeleportTarget *pTarget = (TeleportTarget*)geEntity_GetUserData(pEntity);
		fread(&pTarget->origin, sizeof(geVec3d), 1, RestoreFD);
	}
	
	return RGF_SUCCESS;
}

//	BindToPath
//
//	Given the name of an entity, bind that entity to a motion path.

int CTeleporter::BindToPath(char *szName)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	//	Ok, check to see if there are teleport targets in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "TeleportTarget");
	
	if(!pSet) 
		return RGF_FAILURE;									// No targets
	
	//	Ok, we have teleport targets somewhere.  Dig through 'em all.
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		TeleportTarget *pTarget = (TeleportTarget*)geEntity_GetUserData(pEntity);
		if(strcmp(pTarget->Name, szName) == 0)
		{
			pTarget->bFollower = GE_TRUE;
			return RGF_SUCCESS;					// Flagged and good to go
		}
	}
	
	return RGF_FAILURE;							// No such entity, sorry
}

//	******************** CRGF Overrides ********************

//	LocateEntity
//
//	Given a name, locate the desired item in the currently loaded level
//	..and return it's user data.

int CTeleporter::LocateEntity(char *szName, void **pEntityData)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	//	Ok, check to see if there are teleporters in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "Teleporter");
	
	if(!pSet) 
		return RGF_NOT_FOUND;									// No teleporters
	
	//	Ok, we have teleporters.  Dig through 'em all.
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		Teleporter *pTheEntity = (Teleporter*)geEntity_GetUserData(pEntity);
		if(strcmp(pTheEntity->szEntityName, szName) == 0)
		{
			*pEntityData = (void *)pTheEntity;
			return RGF_SUCCESS;
		}
	}
	
	return RGF_NOT_FOUND;								// Sorry, no such entity here
}

//	ReSynchronize
//
//	Correct internal timing to match current time, to make up for time lost
//	..when outside the game loop (typically in "menu mode").

int CTeleporter::ReSynchronize()
{
	
	return RGF_SUCCESS;
}
