/************************************************************************************//**
 * @file CTeleporter.cpp
 * @brief Teleporter class handler
 *
 * This file contains the class implementation for teleporter handling.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#include "RabidFramework.h"
#include "CLevel.h"
#include "CPathFollower.h"
#include "CTeleporter.h"

extern geSound_Def *SPool_Sound(const char *SName);

/* ------------------------------------------------------------------------------------ */
// CTeleporter
//
// Constructor.  The way we're doing teleporters is that a teleporter
// ..manifests in the game as a ball of fog surrounding a teleport
// ..trigger.  Once the trigger is hit, the teleporter activates and
// ..the player is "instantly" teleported to the destination.
/* ------------------------------------------------------------------------------------ */
CTeleporter::CTeleporter() :
	m_EntityCount(0)					// No teleports
{
	// Ok, check to see if there are teleporters in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "Teleporter");

	if(!pSet)
		return;									// No teleporters

	geEntity *pEntity;

	// Ok, we have teleporters somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Teleporter *pTeleporter = static_cast<Teleporter*>(geEntity_GetUserData(pEntity));

		if((pTeleporter->szEntityName == NULL) || (strlen(pTeleporter->szEntityName) <= 0))
		{
			char szName[128];
			geEntity_GetName(pEntity, szName, 128);
			pTeleporter->szEntityName = szName;
		}

		++m_EntityCount;								// Kick teleporter count

		// Ok, put this entity into the Global Entity Registry
		CCD->EntityRegistry()->AddEntity(pTeleporter->szEntityName, "Teleporter");

		// Reset all the data for each teleporter
		pTeleporter->bActive		= GE_TRUE;				// Teleporter ready
		pTeleporter->theSound		= NULL;					// No sound right now
		pTeleporter->SoundHandle	= -1;
		pTeleporter->theFog			= NULL;					// No fog sphere...yet
		pTeleporter->bForward		= GE_TRUE;				// Animation direction
		pTeleporter->active			= GE_FALSE;
		pTeleporter->cFadeColor.a	= 0.0f;
		pTeleporter->CallBack		= GE_FALSE;
		pTeleporter->CallBackCount	= 0;
		pTeleporter->fOldTime		= 0.0f;
		pTeleporter->bDoFade		= GE_FALSE;
		pTeleporter->bFadeOut		= GE_TRUE;

		if(!EffectC_IsStringNull(pTeleporter->szSoundFile))
		{
			pTeleporter->theSound = SPool_Sound(pTeleporter->szSoundFile);

			if(!pTeleporter->theSound)
			{
				CCD->Log()->Warning("File %s - Line %d: %s: Failed to open audio file '%s'",
									__FILE__, __LINE__,
									pTeleporter->szEntityName, pTeleporter->szSoundFile);
			}
		}

		// Ok, now we add a FOG SPHERE around our teleport trigger.  This makes for
		// ..a neat, simple, and cheap effect - enter the fog, hit the trigger, and
		// .. *POOF* you're teleported.
		if(pTeleporter->bUseFogEffect == GE_TRUE)
		{
			pTeleporter->fDensity = kFogDensity;		// Basic teleport field density
			pTeleporter->cColor.a = 255.0f;
			pTeleporter->fDelta = pTeleporter->fogVariance / static_cast<geFloat>(pTeleporter->fogSpeed);
		}
	}

	// Ok, if we have teleport targets, clear out the pathfollower data
	pSet = geWorld_GetEntitySet(CCD->World(), "TeleportTarget");

	if(pSet != NULL)
	{
		for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
			pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
		{
			TeleportTarget *pTarget = static_cast<TeleportTarget*>(geEntity_GetUserData(pEntity));
			pTarget->bFollower = GE_FALSE;
			CCD->EntityRegistry()->AddEntity(pTarget->Name, "TeleportTarget");
		}
	}
}

/* ------------------------------------------------------------------------------------ */
// ~CTeleporter
//
// Destructor, clean up after ourselves.
/* ------------------------------------------------------------------------------------ */
CTeleporter::~CTeleporter()
{
	if(m_EntityCount == 0)
		return;						// Don't waste CPU cycles

	// Ok, check to see if there are teleporters in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "Teleporter");

	if(!pSet)
		return;											// No teleporters

	geEntity *pEntity;

	// Ok, we have teleporters somewhere.  Dig through 'em all and release
	// ..the audio (if any).
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Teleporter *pTeleport = static_cast<Teleporter*>(geEntity_GetUserData(pEntity));

		if(pTeleport->theSound != NULL)
			geSound_FreeSoundDef(CCD->Engine()->AudioSystem(), pTeleport->theSound);

		pTeleport->theSound = NULL;						// No sound, right now...

		if(pTeleport->theFog != NULL)
			geWorld_RemoveFog(CCD->World(), pTeleport->theFog);

		pTeleport->theFog = NULL;						// Fog cleared, heh heh
	}
}

/* ------------------------------------------------------------------------------------ */
// HandleCollision
//
// Handle a collision with a teleport trigger.
/* ------------------------------------------------------------------------------------ */
bool CTeleporter::HandleCollision(const geWorld_Model *pModel, geActor *theActor)
{
	if(m_EntityCount == 0)
		return false;									// None here, ignore call.

	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "Teleporter");

	if(!pSet)
		return false;

	geEntity *pEntity;

	// Once more we scan the teleporter list.  Does this get old, or what?
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		// Get the teleporter data so we can compare models
		Teleporter *pTeleport = static_cast<Teleporter*>(geEntity_GetUserData(pEntity));

		if(pTeleport->Model == pModel)
		{
			if(pTeleport->PlayerOnly && theActor != CCD->Player()->GetActor())
				return false;

			if(pTeleport->bActive == GE_FALSE || pTeleport->active == GE_FALSE)
				return true;								// Hit, yes, something happened, no

			// Ok, now we search the list of teleport targets, looking for one
			// ..with the name we're shooting for.  Once we do, we'll relocate
			// ..our unsuspecting player to that location.
			geEntity_EntitySet *pSet2 = geWorld_GetEntitySet(CCD->World(), "TeleportTarget");

			if(pSet2 == NULL)
			{
				CCD->Log()->Debug("Teleporters with no targets");
				return true;
			}

			geEntity *pEntity2;

			// Fine, there ARE targets.  Scan until we find the one we want, then
			// ..move the sucker...erm, PLAYER...there.
			for(pEntity2=geEntity_EntitySetGetNextEntity(pSet2, NULL); pEntity2;
				pEntity2=geEntity_EntitySetGetNextEntity(pSet2, pEntity2))
			{
				TeleportTarget *pTarget = static_cast<TeleportTarget*>(geEntity_GetUserData(pEntity2));

				if(!strcmp(pTeleport->Target, pTarget->Name))
				{
					// It's this one, do the teleport!
					if(pTeleport->bOneShot == GE_TRUE)
						pTeleport->bActive = GE_FALSE;			// Teleporter shut down

					pTeleport->CallBack = GE_TRUE;
					pTeleport->CallBackCount = 2;

					if(pTeleport->theSound && pTeleport->SoundHandle == -1)
					{
						Snd Sound;
						memset(&Sound, 0, sizeof(Sound));
						geVec3d_Copy(&(pTeleport->origin), &(Sound.Pos));
						Sound.Min = CCD->Level()->GetAudibleRadius();
						Sound.Loop = GE_FALSE;
						Sound.SoundDef = pTeleport->theSound;
						CCD->EffectManager()->Item_Add(EFF_SND, static_cast<void*>(&Sound));
					}

					// Do a "teleporter" effect.  Note that this causes all
					// ..kinds of weirdness with timings if you're supposed to
					// ..appear in the air at the target end, I'm not sure WHY this
					// ..is but there it is.  However, it is a neat effect...
					if((pTeleport->bUseTeleportEffect == GE_TRUE) &&
						(pTeleport->bUseFogEffect == GE_TRUE) &&
						pTeleport->theFog)
					{
						for(int nTemp=1000; nTemp<6000; nTemp+=100)
						{
							CCD->Engine()->BeginFrame();
							geFog_SetAttributes(pTeleport->theFog,
												&pTeleport->origin,
												&pTeleport->cColor,
												0.0f, static_cast<geFloat>(nTemp+500),
												pTeleport->fogRadius+static_cast<geFloat>(nTemp/400));
							CCD->Engine()->RenderWorld();
							CCD->Engine()->EndFrame();

							CCD->ResetClock();
						}
					}

					// move to new position
					if(pTeleport->fFadeTime > 0.0f && !pTeleport->bDoFade)
					{
						pTeleport->bDoFade = GE_TRUE;
						pTeleport->bFadeOut = GE_TRUE;
						pTeleport->fOldTime = CCD->FreeRunningCounter_F();
						pTeleport->theActor = theActor;
					}
					else
					{
						if(pTarget->UseOffset)
						{
							geVec3d TargetPos;

							CCD->ActorManager()->GetPosition(theActor, &TargetPos);
							geVec3d_Subtract(&(pTeleport->origin), &TargetPos, &TargetPos);
							geVec3d_Subtract(&(pTarget->origin), &TargetPos, &TargetPos);
							CCD->ActorManager()->Position(theActor, TargetPos);
						}
						else
						{
							CCD->ActorManager()->Position(theActor, pTarget->origin);
						}

						// rotate to match target angle
						if(pTarget->UseAngle)
						{
							geVec3d RealAngle;
							geVec3d_Scale(&(pTarget->Angle), GE_PIOVER180, &RealAngle);
							RealAngle.Y -= GE_PIOVER2;

							CCD->ActorManager()->Rotate(theActor, RealAngle);

							if(CCD->Player()->GetActor() == theActor && CCD->Player()->GetViewPoint() == FIRSTPERSON)
							{
								CCD->CameraManager()->SetRotation(RealAngle);
							}
						}
					}

					return true;
				}
			}

			return true;					// Player is _TELEPORTED_
		}
	}

	return false;							// We hit no known teleporters
}

/* ------------------------------------------------------------------------------------ */
// Tick
//
// Increment animation times on teleporters.  This is used to vary
// ..the color of each teleporter from one point to another.  The
// ..time variance is set in the world editor.
/* ------------------------------------------------------------------------------------ */
void CTeleporter::Tick(geFloat dwTicks)
{
	if(m_EntityCount == 0)
		return;						// Don't waste CPU cycles

	// First, we're going to scrounge through all the TeleportTargets
	// ..and see if any of them are bound to a motion path.
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "TeleportTarget");

	geEntity *pEntity;

	if(pSet != NULL)
	{
		for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
			pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
		{
			TeleportTarget *pTarget = static_cast<TeleportTarget*>(geEntity_GetUserData(pEntity));

			if(pTarget->bFollower)
				CCD->Level()->PathFollower()->GetNextPosition(pTarget->Name, &pTarget->origin,	false);
			// Seek to next position, if in motion
		}
	}

	// Ok, check to see if there are teleporters in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "Teleporter");

	if(!pSet)
		return;									// No teleporters

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Teleporter *pTeleport = static_cast<Teleporter*>(geEntity_GetUserData(pEntity));

		if(!EffectC_IsStringNull(pTeleport->TriggerName))
		{
			if(GetTriggerState(pTeleport->TriggerName))
			{
				if(pTeleport->active == GE_FALSE)
					pTeleport->active = GE_TRUE;
			}
			else
			{
				if(pTeleport->active == GE_TRUE)
				{
					if(pTeleport->theFog != NULL)
						geWorld_RemoveFog(CCD->World(), pTeleport->theFog);

					pTeleport->theFog = NULL;
					pTeleport->active = GE_FALSE;
				}
			}
		}
		else
		{
			if(pTeleport->active == GE_FALSE)
				pTeleport->active = GE_TRUE;
		}

		if(pTeleport->active == GE_TRUE)
		{
			int32 Leaf;
			geWorld_GetLeaf(CCD->World(), &(pTeleport->origin), &Leaf);

			if(EffectC_IsPointVisible(CCD->World(),
				CCD->CameraManager()->Camera(),
				&(pTeleport->origin),
				Leaf,
				EFFECTC_CLIP_LEAF) == GE_FALSE)
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
						geFog_SetAttributes(pTeleport->theFog,
											&pTeleport->origin,
											&pTeleport->cColor,
											0.0f,
											pTeleport->fDensity,
											pTeleport->fogRadius);
				}
			}

			if((pTeleport->theFog != NULL) && (pTeleport->bActive))
			{
				// Teleport has fog and is active, animate.
				if(pTeleport->bForward)
				{
					pTeleport->fDensity += (pTeleport->fDelta * dwTicks);

					if(pTeleport->fDensity > (kFogDensity+pTeleport->fogVariance))
						pTeleport->bForward = GE_FALSE;			// Start decrementing next time
				}
				else
				{
					pTeleport->fDensity -= (pTeleport->fDelta * dwTicks);

					if(pTeleport->fDensity < (kFogDensity-pTeleport->fogVariance))
						pTeleport->bForward = GE_TRUE;			// Start incrementing next time
				}

				geFog_SetAttributes(pTeleport->theFog,
									&pTeleport->origin,
									&pTeleport->cColor,
									0.0f,
									pTeleport->fDensity,
									pTeleport->fogRadius);		// Animate the density!
			}
		}

		if(pTeleport->CallBack == GE_TRUE)
		{
			pTeleport->CallBackCount -= 1;

			if(pTeleport->CallBackCount == 0)
			{
				pTeleport->CallBack = GE_FALSE;
			}
		}
	}
}


void CTeleporter::DoFade(void)
{
	if(m_EntityCount == 0)
		return;						// Don't waste CPU cycles

	// Ok, check to see if there are teleporters in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "Teleporter");

	if(!pSet)
		return;									// No teleporters

	geEntity *pEntity;

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Teleporter *pTeleport = static_cast<Teleporter*>(geEntity_GetUserData(pEntity));

		if(pTeleport->bDoFade)
		{
			GE_Rect Rect;
			Rect.Left = Rect.Top = 0;
			Rect.Right = CCD->Engine()->Width() - 1;
			Rect.Bottom = CCD->Engine()->Height() - 1;

			// fade out
			if(pTeleport->bFadeOut == GE_TRUE)
			{
				float ElapsedTime = (CCD->FreeRunningCounter_F() - pTeleport->fOldTime);

				if(ElapsedTime > (pTeleport->fFadeTime/255.0f))
				{
					pTeleport->cFadeColor.a += (255.0f*ElapsedTime)/pTeleport->fFadeTime;

					if(pTeleport->cFadeColor.a >= 255.0f)
					{
						pTeleport->cFadeColor.a = 255.0f;
						pTeleport->bFadeOut = GE_FALSE;
					}

					pTeleport->fOldTime = CCD->FreeRunningCounter_F();
				}

				geEngine_FillRect(CCD->Engine()->Engine(), &Rect, &(pTeleport->cFadeColor));
			}
			else // fade in
			{
				float ElapsedTime = (CCD->FreeRunningCounter_F() - pTeleport->fOldTime);

				if(ElapsedTime > (pTeleport->fFadeTime/255.0f))
				{
					pTeleport->cFadeColor.a -= (255.0f*ElapsedTime)/pTeleport->fFadeTime;

					if(pTeleport->cFadeColor.a <= 0.0f)
					{
						pTeleport->cFadeColor.a = 0.0f;
						pTeleport->bDoFade = GE_FALSE;
					}

					pTeleport->fOldTime = CCD->FreeRunningCounter_F();
				}

				geEngine_FillRect(CCD->Engine()->Engine(), &Rect, &(pTeleport->cFadeColor));
			}

			if(pTeleport->cFadeColor.a >= 255.0f)
			{
				geEntity_EntitySet *pSet2 = geWorld_GetEntitySet(CCD->World(), "TeleportTarget");

				if(!pSet2)
					continue;

				geEntity *pEntity2;

				for(pEntity2=geEntity_EntitySetGetNextEntity(pSet2, NULL); pEntity2;
					pEntity2=geEntity_EntitySetGetNextEntity(pSet2, pEntity2))
				{
					TeleportTarget *pTarget = static_cast<TeleportTarget*>(geEntity_GetUserData(pEntity2));

					if(!strcmp(pTeleport->Target, pTarget->Name))
					{
						// move to new position
						if(pTarget->UseOffset)
						{
							geVec3d TargetPos;

							CCD->ActorManager()->GetPosition(pTeleport->theActor, &TargetPos);
							geVec3d_Subtract(&(pTeleport->origin), &TargetPos, &TargetPos);
							geVec3d_Subtract(&(pTarget->origin), &TargetPos, &TargetPos);
							CCD->ActorManager()->Position(pTeleport->theActor, TargetPos);
						}
						else
						{
							CCD->ActorManager()->Position(pTeleport->theActor, pTarget->origin);
						}

						// rotate to match target angle
						if(pTarget->UseAngle)
						{
							geVec3d RealAngle;
							geVec3d_Scale(&(pTarget->Angle), GE_PIOVER180, &RealAngle);
							RealAngle.Y -= GE_PIOVER2;

							CCD->ActorManager()->Rotate(pTeleport->theActor, RealAngle);

							if(CCD->Player()->GetActor() == pTeleport->theActor &&
								CCD->Player()->GetViewPoint() == FIRSTPERSON)
							{
								CCD->CameraManager()->SetRotation(RealAngle);
							}
						}
					}
				}
			}
		}
	}
}

/* ------------------------------------------------------------------------------------ */
// SaveTo
//
// Save all teleporters and teleport targets to the supplied file
/* ------------------------------------------------------------------------------------ */
int CTeleporter::SaveTo(FILE *SaveFD, bool type)
{
	if(m_EntityCount == 0)
		return RGF_SUCCESS;						// Don't waste CPU cycles

	// Ok, check to see if there are teleporters in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "Teleporter");

	if(!pSet)
		return RGF_SUCCESS;									// No teleporters

	geEntity *pEntity;

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Teleporter *pTeleport = static_cast<Teleporter*>(geEntity_GetUserData(pEntity));

		WRITEDATA(type, &(pTeleport->bActive),		sizeof(geBoolean),	1, SaveFD);
		WRITEDATA(type, &(pTeleport->bForward),		sizeof(geBoolean),	1, SaveFD);
		WRITEDATA(type, &(pTeleport->fDelta),		sizeof(geFloat),	1, SaveFD);
		WRITEDATA(type, &(pTeleport->fDensity),		sizeof(geFloat),	1, SaveFD);
		WRITEDATA(type, &(pTeleport->origin),		sizeof(geVec3d),	1, SaveFD);
		WRITEDATA(type, &(pTeleport->active),		sizeof(geBoolean),	1, SaveFD);
		WRITEDATA(type, &(pTeleport->CallBack),		sizeof(geBoolean),	1, SaveFD);
		WRITEDATA(type, &(pTeleport->CallBackCount),sizeof(int),		1, SaveFD);
	}

	pSet = geWorld_GetEntitySet(CCD->World(), "TeleportTarget");

	if(!pSet)
		return RGF_SUCCESS;									// No targets

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		TeleportTarget *pTarget = static_cast<TeleportTarget*>(geEntity_GetUserData(pEntity));

		WRITEDATA(type, &(pTarget->origin), sizeof(geVec3d), 1, SaveFD);
	}

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// RestoreFrom
//
// Restore all teleporters and teleport targets from the supplied file
/* ------------------------------------------------------------------------------------ */
int CTeleporter::RestoreFrom(FILE *RestoreFD, bool type)
{
	if(m_EntityCount == 0)
		return RGF_SUCCESS;						// Don't waste CPU cycles

	// Ok, check to see if there are teleporters in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "Teleporter");

	if(!pSet)
		return RGF_SUCCESS;									// No teleporters

	geEntity *pEntity;

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Teleporter *pTeleport = static_cast<Teleporter*>(geEntity_GetUserData(pEntity));

		READDATA(type, &(pTeleport->bActive),		sizeof(geBoolean),	1, RestoreFD);
		READDATA(type, &(pTeleport->bForward),		sizeof(geBoolean),	1, RestoreFD);
		READDATA(type, &(pTeleport->fDelta),		sizeof(geFloat),	1, RestoreFD);
		READDATA(type, &(pTeleport->fDensity),		sizeof(geFloat),	1, RestoreFD);
		READDATA(type, &(pTeleport->origin),		sizeof(geVec3d),	1, RestoreFD);
		READDATA(type, &(pTeleport->active),		sizeof(geBoolean),	1, RestoreFD);
		READDATA(type, &(pTeleport->CallBack),		sizeof(geBoolean),	1, RestoreFD);
		READDATA(type, &(pTeleport->CallBackCount),	sizeof(int),		1, RestoreFD);
	}

	pSet = geWorld_GetEntitySet(CCD->World(), "TeleportTarget");

	if(!pSet)
		return RGF_SUCCESS;									// No targets

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		TeleportTarget *pTarget = static_cast<TeleportTarget*>(geEntity_GetUserData(pEntity));

		READDATA(type, &(pTarget->origin), sizeof(geVec3d), 1, RestoreFD);
	}

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// BindToPath
//
// Given the name of an entity, bind that entity to a motion path.
/* ------------------------------------------------------------------------------------ */
int CTeleporter::BindToPath(const char *szName)
{
	// Ok, check to see if there are teleport targets in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "TeleportTarget");

	if(!pSet)
		return RGF_FAILURE;									// No targets

	geEntity *pEntity;

	// Ok, we have teleport targets somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		TeleportTarget *pTarget = static_cast<TeleportTarget*>(geEntity_GetUserData(pEntity));

		if(!strcmp(pTarget->Name, szName))
		{
			pTarget->bFollower = GE_TRUE;
			return RGF_SUCCESS;					// Flagged and good to go
		}
	}

	return RGF_FAILURE;							// No such entity, sorry
}

// ******************** CRGF Overrides ********************

/* ------------------------------------------------------------------------------------ */
// LocateEntity
//
// Given a name, locate the desired item in the currently loaded level
// ..and return its user data.
/* ------------------------------------------------------------------------------------ */
int CTeleporter::LocateEntity(const char *szName, void **pEntityData)
{
	// Ok, check to see if there are teleporters in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "Teleporter");

	if(!pSet)
		return RGF_NOT_FOUND;									// No teleporters

	geEntity *pEntity;

	// Ok, we have teleporters.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Teleporter *pTheEntity = static_cast<Teleporter*>(geEntity_GetUserData(pEntity));

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
int CTeleporter::ReSynchronize()
{
	return RGF_SUCCESS;
}

/* ----------------------------------- END OF FILE ------------------------------------ */
