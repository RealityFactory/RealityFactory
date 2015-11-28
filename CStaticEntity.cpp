/****************************************************************************************/
/*																						*/
/*	CStaticEntity.cpp:		Static Entity Handling Class								*/
/*																						*/
/*	(c) 2001 Ralph Deane																*/
/*																						*/
/*	This file contains the class implementation for the Static Entity					*/
/*	handling class.  Static entities are Genesis3D actors that do not					*/
/*	have any animation (this means health packs, weapons, props, things					*/
/*	that aren't world models but aren't at the level of non-player characters).			*/
/*																						*/
/****************************************************************************************/

//	Include the One True Header
#include "RabidFramework.h"

extern geSound_Def *SPool_Sound(char *SName);
// changed QD 06/26/04
extern geBitmap *TPool_Bitmap(char *DefaultBmp, char *DefaultAlpha, char *BName, char *AName);
// end change

/* ------------------------------------------------------------------------------------ */
//	Constructor
//
//	Go through all the entity proxies in the level and load up the
//	..various actors to be used.
/* ------------------------------------------------------------------------------------ */
CStaticEntity::CStaticEntity()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	m_StaticEntityCount = 0;

	// Ok, see if we have any static entity proxies we need to set up.
	pSet = geWorld_GetEntitySet(CCD->World(), "StaticEntityProxy");

	if(!pSet)
		return;													// Don't waste CPU time.

	// Ok, we have static entities somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		StaticEntityProxy *pProxy = (StaticEntityProxy*)geEntity_GetUserData(pEntity);
		pProxy->bFollower = GE_FALSE;

		if(EffectC_IsStringNull(pProxy->szEntityName))
		{
			char szName[128];
			geEntity_GetName(pEntity, szName, 128);
			pProxy->szEntityName = szName;
		}

		m_StaticEntityCount++;

		// Ok, put this entity into the Global Entity Registry
		CCD->EntityRegistry()->AddEntity(pProxy->szEntityName, "StaticEntityProxy");
		pProxy->ActorRotation.X = GE_PIOVER180*pProxy->ActorRotation.X;
		pProxy->ActorRotation.Y = GE_PIOVER180*pProxy->ActorRotation.Y;
		pProxy->ActorRotation.Z = GE_PIOVER180*pProxy->ActorRotation.Z;

		pProxy->Actor = CCD->ActorManager()->SpawnActor(pProxy->szActorFile,
			pProxy->origin, pProxy->ActorRotation, pProxy->szDefaultAction,
			pProxy->szDefaultAction, NULL);				// Spawn the actor off

		if(!pProxy->Actor)
		{
			char szError[256];
			sprintf(szError, "*WARNING* File %s - Line %d: %s : Missing Actor '%s'",
					__FILE__, __LINE__, pProxy->szEntityName, pProxy->szActorFile);
			CCD->ReportError(szError, false);
			// changed QD 07/15/06
			/*
			CCD->ShutdownLevel();
			delete CCD;
			CCD = NULL;
			MessageBox(NULL, szError, "StaticEntity", MB_OK);
			exit(-333);
			*/
			continue;
			// end change
		}

// changed Nout 12/15/05 - StaticEntity can be selected via ActorManager->GetEntityByName
		CCD->ActorManager()->SetEntityName(pProxy->Actor, pProxy->szEntityName);
// end change
		CCD->ActorManager()->SetScale(pProxy->Actor, pProxy->ScaleFactor);
		CCD->ActorManager()->SetType(pProxy->Actor, ENTITY_PROP);
		CCD->ActorManager()->SetAutoStepUp(pProxy->Actor, true);

		if(pProxy->StepHeight != 0.0f)
			CCD->ActorManager()->SetStepHeight(pProxy->Actor, pProxy->StepHeight);

		CCD->ActorManager()->SetAlpha(pProxy->Actor, (geFloat)pProxy->InitialAlpha);	// Set alpha
		CCD->ActorManager()->SetColDetLevel(pProxy->Actor, RGF_COLLISIONLEVEL_2);

		if(pProxy->IsAVehicle)
			CCD->ActorManager()->SetType(pProxy->Actor, ENTITY_VEHICLE);	// Make a vehicle

		if(pProxy->SubjectToGravity)
			CCD->ActorManager()->SetGravity(pProxy->Actor, CCD->Player()->GetGravity());
// changed QD 07/24/04
		CCD->ActorManager()->SetActorDynamicLighting(pProxy->Actor, pProxy->FillColor, pProxy->AmbientColor, pProxy->AmbientLightFromFloor);
// end change

		CCD->ActorManager()->SetShadow(pProxy->Actor, pProxy->ShadowSize);

// changed QD 06/26/04
		if(pProxy->ShadowAlpha > 0.0f)
			CCD->ActorManager()->SetShadowAlpha(pProxy->Actor, pProxy->ShadowAlpha);

		if(!EffectC_IsStringNull(pProxy->ShadowBitmap))
			CCD->ActorManager()->SetShadowBitmap(pProxy->Actor, TPool_Bitmap(pProxy->ShadowBitmap, pProxy->ShadowAlphamap, NULL, NULL));
// end change

// begin change gekido
		CCD->ActorManager()->SetProjectedShadows(pProxy->Actor, pProxy->UseProjectedShadows);
// end change gekido

// changed QD Shadows
		CCD->ActorManager()->SetStencilShadows(pProxy->Actor, pProxy->UseStencilShadows);
// end change

		if(pProxy->EnvironmentMapping)
			SetEnvironmentMapping(pProxy->Actor, true, pProxy->AllMaterial, pProxy->PercentMapping, pProxy->PercentMaterial);

// changed RF064
		CCD->ActorManager()->SetHideRadar(pProxy->Actor, pProxy->HideFromRadar);

		if(!EffectC_IsStringNull(pProxy->ChangeMaterial))
			CCD->ActorManager()->ChangeMaterial(pProxy->Actor, pProxy->ChangeMaterial);

		if(pProxy->BoxSize.X != 0.0f && pProxy->BoxSize.Y != 0.0f && pProxy->BoxSize.Z != 0.0f)
		{
			CCD->ActorManager()->SetBoxChange(pProxy->Actor, false);
			CCD->ActorManager()->SetBBox(pProxy->Actor, pProxy->BoxSize.X, -(pProxy->BoxSize.Y*2.0f), pProxy->BoxSize.Z);
		}
// end change RF064

// changed QD 07/21/04
		if(pProxy->NoCollision)
			CCD->ActorManager()->SetNoCollide(pProxy->Actor);
// end change

		pProxy->bInitialized	= GE_FALSE;		// Pathfollowing not initialized
		pProxy->IsHit			= GE_FALSE;
		pProxy->index			= -1;
		pProxy->alive			= GE_TRUE;
		pProxy->bState			= GE_TRUE;
		pProxy->CallBack		= GE_FALSE;
		pProxy->Time			= pProxy->DamageDelay;
		pProxy->DoingDamage		= GE_FALSE;
// changed QD 05/06/2004
		pProxy->dying			= GE_FALSE;

		if(pProxy->FadeOutTime <= 0.0f)
			pProxy->FadeOut = GE_FALSE;
		else
			pProxy->AlphaRate = (pProxy->InitialAlpha)/pProxy->FadeOutTime;
// end change

		if(pProxy->AttributeAmt != -1)
		{
			CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(pProxy->Actor);

			if(EffectC_IsStringNull(pProxy->DamageAttribute))
			{
				theInv->AddAndSet("health",(int)pProxy->AttributeAmt);
				theInv->SetValueLimits("health", 0, (int)pProxy->AttributeAmt);
			}
			else
			{
				theInv->AddAndSet(pProxy->DamageAttribute,(int)pProxy->AttributeAmt);
				theInv->SetValueLimits(pProxy->DamageAttribute, 0, (int)pProxy->AttributeAmt);
			}
		}

		pProxy->bFollower = GE_FALSE;

		if(!EffectC_IsStringNull(pProxy->szSoundFile))
		{
			SPool_Sound(pProxy->szSoundFile);
		}
	}

	return;
}

/* ------------------------------------------------------------------------------------ */
//	Destructor
//
//	Go through all proxies and unload all the actors associated with 'em.
/* ------------------------------------------------------------------------------------ */
CStaticEntity::~CStaticEntity()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	if(m_StaticEntityCount == 0)
		return;									// Don't waste time here

	pSet = geWorld_GetEntitySet(CCD->World(), "StaticEntityProxy");

	if(!pSet)
		return;									// All gone?  How odd.

	// Ok, we have static entities s somewhere.  Dig through 'em all and release
	// ..the audio (if any) and the actors we loaded.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		StaticEntityProxy *pProxy = (StaticEntityProxy*)geEntity_GetUserData(pEntity);

		if(pProxy->Actor)
		{
			if(pProxy->alive == GE_TRUE || !pProxy->DeathDissappear)
			{
				CCD->ActorManager()->RemoveActor(pProxy->Actor);
			}

			geActor_Destroy(&pProxy->Actor);
		}
	}

	//	All cleaned up, bail this mess.
	return;
}

extern "C" void	DrawBoundBox(geWorld *World, const geVec3d *Pos, const geVec3d *Min, const geVec3d *Max);

/* ------------------------------------------------------------------------------------ */
//	Render
//
//	Render all of the static entities in the level.
/* ------------------------------------------------------------------------------------ */
void CStaticEntity::Render(geXForm3d ViewPoint, DWORD dwTime)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	if(m_StaticEntityCount == 0)
		return;									// Don't waste time here

	pSet = geWorld_GetEntitySet(CCD->World(), "StaticEntityProxy");

	if(!pSet)
		return;									// All gone?  How odd.

	// Ok, we have static entities s somewhere.  Dig through 'em all and release
	// ..the audio (if any) and the actors we loaded.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		StaticEntityProxy *pProxy = (StaticEntityProxy*)geEntity_GetUserData(pEntity);

		// changed QD 07/15/06
		if(!pProxy->Actor)
			continue;
		// end change

		if(CCD->MenuManager()->GetSEBoundBox() && pProxy->alive)
		{
			geExtBox ExtBox;
			//geActor_GetDynamicExtBox(pProxy->Actor, &ExtBox);
			geActor_GetExtBox(pProxy->Actor, &ExtBox);
			ExtBox.Min.X -= pProxy->origin.X;
			ExtBox.Min.Y -= pProxy->origin.Y;
			ExtBox.Min.Z -= pProxy->origin.Z;
			ExtBox.Max.X -= pProxy->origin.X;
			ExtBox.Max.Y -= pProxy->origin.Y;
			ExtBox.Max.Z -= pProxy->origin.Z;
			DrawBoundBox(CCD->World(), &pProxy->origin, &ExtBox.Min, &ExtBox.Max);
		}
	}

	return;
}

/* ------------------------------------------------------------------------------------ */
//	HandleCollision
//
//	Process a collision between the player and an actor.  We will check
//	..all known proxies and, if the actor associated with one is the
//	..actor we hit, will act (heh) on it.  For now, that means we play
//	..the "collision" audio, though eventually this will trigger script
//	..execution.
/* ------------------------------------------------------------------------------------ */
// changed RF063
int CStaticEntity::HandleCollision(geActor *pActor, geActor *theActor, bool Gravity, bool UseKey)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	if(m_StaticEntityCount == 0)
		return RGF_FAILURE;							// No props in world, bail early

	// Ok, see if we have any static entity proxies
	pSet = geWorld_GetEntitySet(CCD->World(), "StaticEntityProxy");

	if(!pSet)
		return RGF_FAILURE;													// Don't waste CPU time.

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		StaticEntityProxy *pProxy = (StaticEntityProxy*)geEntity_GetUserData(pEntity);

		// changed QD 07/15/06
		if(!pProxy->Actor)
			continue;
		// end change

		if(pProxy->Actor != pActor)
		{
			pProxy->IsHit = GE_FALSE;
			continue;	  // Not this one, keep looking
		}

// changed RF063
		if(UseKey && !pProxy->UseKey)
			return RGF_FAILURE;

		if(UseKey && !(pProxy->GetDamaged && pProxy->alive && pProxy->DoDamage && !pProxy->DoingDamage))
			return RGF_FAILURE;
// end change RF063

		if(!pProxy->alive)
		{
			pProxy->IsHit = GE_TRUE;
			return RGF_SUCCESS;
		}

		if(pProxy->Actor == pActor && pProxy->alive && pProxy->DoDamage && !pProxy->DoingDamage)
		{
			if(pProxy->GetDamaged)
			{
				if(pProxy->AttributeAmt != -1)
				{
// changed RF063
					if(EffectC_IsStringNull(pProxy->DamageAttribute))
						CCD->Damage()->DamageActor(pProxy->Actor, pProxy->Damage, "health", pProxy->Damage, "health", "Actor");
					else
						CCD->Damage()->DamageActor(pProxy->Actor, pProxy->Damage, pProxy->DamageAttribute, pProxy->Damage, pProxy->DamageAttribute, "Actor");
// end change RF063
					pProxy->Time = 0.0f;
					pProxy->DoingDamage = GE_TRUE;
				}
			}
			else
			{
				if(theActor)
				{
// changed RF063
					if(EffectC_IsStringNull(pProxy->DamageTo))
						CCD->Damage()->DamageActor(theActor, pProxy->Damage, "health", pProxy->DamageAlt, pProxy->DamageToAlt, "SEP");
					else
						CCD->Damage()->DamageActor(theActor, pProxy->Damage, pProxy->DamageTo, pProxy->DamageAlt, pProxy->DamageToAlt, "SEP");
// end change RF063
					pProxy->Time = 0.0f;
					pProxy->DoingDamage = GE_TRUE;
				}
			}
		}

		if(!EffectC_IsStringNull(pProxy->szSoundFile))
		{
			if(pProxy->index != -1)
			{
				if(!CCD->EffectManager()->Item_Alive(pProxy->index))
					pProxy->index = -1;
			}
			// eaa3 03/28/2000 Check to see if the sound is already playing,
			// ..and if so, DON'T REPLAY IT!!!
			if(pProxy->IsHit == GE_FALSE)
			{
				if(pProxy->index == -1)
				{
					Snd Sound;
					pProxy->IsHit = GE_TRUE;

					memset(&Sound, 0, sizeof(Sound));
					geVec3d_Copy(&(pProxy->origin), &(Sound.Pos));
					Sound.Min = CCD->GetAudibleRadius();
					Sound.Loop = GE_FALSE;
					Sound.SoundDef = SPool_Sound(pProxy->szSoundFile);
					pProxy->index = CCD->EffectManager()->Item_Add(EFF_SND, (void*)&Sound);
				}
			}
			// End of sound checking
		}

		// If we have a collision animation, switch to that
		if(!EffectC_IsStringNull(pProxy->szImpactAction))
		{
// changed RF064
			char *Motion = CCD->ActorManager()->GetMotion(pProxy->Actor);

			if(strcmp(Motion, pProxy->szImpactAction))
			{
				CCD->ActorManager()->SetMotion(pProxy->Actor, pProxy->szImpactAction);
				CCD->ActorManager()->SetNextMotion(pProxy->Actor, pProxy->szDefaultAction);
			}
// end change RF064
		}

		// Ok, if this is a MOVEABLE STATIC ACTOR, let's see if it can be
		// ..moved in the direction the player avatar is looking
		if(pProxy->bCanPush && !Gravity && (theActor == CCD->Player()->GetActor()))
		{
			geVec3d In, SavedPosition;

			int direct = CCD->Player()->LastDirection();

			if(direct == RGF_K_FORWARD || direct == RGF_K_BACKWARD)
				CCD->Player()->GetIn(&In);
			else
				CCD->Player()->GetLeft(&In);

			CCD->ActorManager()->GetPosition(pProxy->Actor, &(pProxy->origin));
			SavedPosition = pProxy->origin;					// Back this up..
			geFloat speed = CCD->Player()->LastMovementSpeed();

			if(direct == RGF_K_BACKWARD || direct == RGF_K_RIGHT)
				speed = -speed;

			geVec3d_AddScaled(&(pProxy->origin), &In, speed, &(pProxy->origin)); // Move the item
			bool result = CCD->ActorManager()->ValidateMove(SavedPosition, pProxy->origin, pProxy->Actor, false);

			if(result)
				return RGF_RECHECK;
		}

		return RGF_SUCCESS;													// Hit, and dealt with
	}

	return RGF_FAILURE;														// Nothing we own collided with
}

/* ------------------------------------------------------------------------------------ */
//	clear IsHit flag on all entities
/* ------------------------------------------------------------------------------------ */
void CStaticEntity::ClearHit()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	if(m_StaticEntityCount == 0)
		return;							// No props in world, bail early

	// Ok, see if we have any static entity proxies
	pSet = geWorld_GetEntitySet(CCD->World(), "StaticEntityProxy");

	if(!pSet)
		return;													// Don't waste CPU time.

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		StaticEntityProxy *pProxy = (StaticEntityProxy*)geEntity_GetUserData(pEntity);
		pProxy->IsHit = GE_FALSE;
	}
}

/* ------------------------------------------------------------------------------------ */
//	Tick
//
//	Handle any time-based processing necessary.  In this case, that means
//	..to update the sound effects from any playing audio effects to
//	..reflect the player avatar position.
/* ------------------------------------------------------------------------------------ */
void CStaticEntity::Tick(geFloat dwTicks)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	if(m_StaticEntityCount == 0)
		return;							// No props in world, bail early

	// Ok, see if we have any static entity proxies
	pSet = geWorld_GetEntitySet(CCD->World(), "StaticEntityProxy");

	if(!pSet)
		return;													// Don't waste CPU time.

	// Ok, we have static entities somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		StaticEntityProxy *pProxy = (StaticEntityProxy*)geEntity_GetUserData(pEntity);

		// changed QD 07/15/06
		if(!pProxy->Actor)
			continue;
		// end change

// changed QD 05/06/2004
		if(pProxy->dying && pProxy->DeathDissappear)
		{
			if(EffectC_IsStringNull(pProxy->szDeathAction) || CCD->ActorManager()->EndAnimation(pProxy->Actor))
			{
				if (pProxy->FadeOut && pProxy->InitialAlpha > 0.0f)
				{
					CCD->ActorManager()->SetNoCollide(pProxy->Actor);
					pProxy->InitialAlpha -= (pProxy->AlphaRate*(dwTicks*0.001f));
					CCD->ActorManager()->SetAlpha(pProxy->Actor, pProxy->InitialAlpha);
				}
				else
				{
					CCD->ActorManager()->RemoveActorCheck(pProxy->Actor);
					pProxy->dying = GE_FALSE;
				}
			}

			continue;
		}
// end change

		if(pProxy->DoingDamage)
		{
			pProxy->Time += (dwTicks*0.001f);

			if(pProxy->Time>pProxy->DamageDelay)
				pProxy->DoingDamage = GE_FALSE;
		}

		if(pProxy->CallBack == GE_TRUE)
		{
			pProxy->CallBackCount -= 1;
			if(pProxy->CallBackCount == 0)
				pProxy->CallBack = GE_FALSE;
		}
		if(pProxy->AttributeAmt != -1 && pProxy->alive)
		{
			CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(pProxy->Actor);
			int health = theInv->Value("health");

			if(!EffectC_IsStringNull(pProxy->DamageAttribute))
				health = theInv->Value(pProxy->DamageAttribute);

			if(health <= 0)
			{
				pProxy->alive = GE_FALSE;
				pProxy->bState = GE_FALSE;
				pProxy->CallBack = GE_TRUE;
				pProxy->CallBackCount = 2;

// changed QD 05/06/2004
				pProxy->dying = GE_TRUE;

				if(!EffectC_IsStringNull(pProxy->szDeathAction))
				{
					CCD->ActorManager()->SetMotion(pProxy->Actor, pProxy->szDeathAction);
					CCD->ActorManager()->SetHoldAtEnd(pProxy->Actor, true);
				}
				else if(pProxy->DeathDissappear && (pProxy->FadeOut == GE_FALSE))
				{
					CCD->ActorManager()->RemoveActorCheck(pProxy->Actor);
					pProxy->dying = GE_FALSE;
				}
// end change
			}
		}

		// Now handle pathfollowing
		if(pProxy->bFollower == GE_TRUE && pProxy->alive)
		{
			// Ok, we need to see if we've already positioned this fellow to
			// ..its starting point - if not, on the first tick we translate
			// ..it to the head of the path it's on
			if(pProxy->bInitialized == GE_FALSE)
			{
				CCD->PathFollower()->GetPathOrigin(pProxy->szEntityName, &pProxy->origin);
				CCD->ActorManager()->Position(pProxy->Actor, pProxy->origin);
				pProxy->bInitialized = GE_TRUE;
				return;
			}

			// If the actor needs to face his destination, let's add that to the
			// ..actors default rotation.  Notice that, with this flag, we're also
			// ..going to do motion different - we'll take the facing direction as
			// ..our target vector and do an AddScaled() to the old origin to move
			// ..in the right direction without point interpolation.  This is very
			// ..useful for moving "pseudo-NPC" entities around...
			CCD->PathFollower()->GetNextPosition(pProxy->szEntityName, &(pProxy->origin), false);	 // Seek to next position,
			geVec3d pPosition;
			CCD->ActorManager()->GetPosition(pProxy->Actor, &pPosition);

			if(pProxy->SubjectToGravity)
				pProxy->origin.Y = pPosition.Y;

			if(pProxy->FaceDestination == GE_TRUE)
			{
				geVec3d pTarget;
				CCD->PathFollower()->GetTarget(pProxy->szEntityName, &pTarget);				// Seek to next position

				if(pProxy->SubjectToGravity)
					pTarget.Y = pPosition.Y;

				CCD->ActorManager()->RotateToFacePoint(pProxy->Actor, pTarget);
			}

			CCD->ActorManager()->ValidateMove(pPosition, pProxy->origin, pProxy->Actor, true);
		}
	}

	return;
}

/* ------------------------------------------------------------------------------------ */
//	SaveTo
//
//	Save static entities states to a supplied file
/* ------------------------------------------------------------------------------------ */
int CStaticEntity::SaveTo(FILE *SaveFD, bool type)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	if(m_StaticEntityCount == 0)
		return RGF_SUCCESS;							// No props in world, bail early

	// Ok, see if we have any static entity proxies
	pSet = geWorld_GetEntitySet(CCD->World(), "StaticEntityProxy");

	if(!pSet)
		return RGF_SUCCESS;

	// Ok, we have static entities somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
// changed RF063
		StaticEntityProxy *pProxy = (StaticEntityProxy*)geEntity_GetUserData(pEntity);

		// changed QD 07/15/06
		if(!pProxy->Actor)
			continue;
		// end change

		if(pProxy->alive || (!pProxy->alive && !pProxy->DeathDissappear))
		{
			CCD->ActorManager()->GetPosition(pProxy->Actor, &pProxy->origin);
		}

		WRITEDATA(&pProxy->origin,			sizeof(geVec3d),	1, SaveFD);
		WRITEDATA(&pProxy->CallBack,		sizeof(geBoolean),	1, SaveFD);
		WRITEDATA(&pProxy->CallBackCount,	sizeof(int),		1, SaveFD);
		WRITEDATA(&pProxy->bState,			sizeof(geBoolean),	1, SaveFD);
		WRITEDATA(&pProxy->DoingDamage,		sizeof(geBoolean),	1, SaveFD);
		WRITEDATA(&pProxy->Time,			sizeof(geFloat),	1, SaveFD);
		WRITEDATA(&pProxy->alive,			sizeof(geBoolean),	1, SaveFD);
		WRITEDATA(&pProxy->IsHit,			sizeof(geBoolean),	1, SaveFD);
		WRITEDATA(&pProxy->bInitialized,	sizeof(geBoolean),	1, SaveFD);

		if(pProxy->alive)
		{
			CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(pProxy->Actor);
			int health = theInv->Value("health");

			if(!EffectC_IsStringNull(pProxy->DamageAttribute))
				health = theInv->Value(pProxy->DamageAttribute);

			WRITEDATA(&health, sizeof(int), 1, SaveFD);
		}
// end change RF063
	}

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	RestoreFrom
//
//	Restore static entities states from a supplied file
/* ------------------------------------------------------------------------------------ */
int CStaticEntity::RestoreFrom(FILE *RestoreFD, bool type)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	if(m_StaticEntityCount == 0)
		return RGF_SUCCESS;							// No props in world, bail early

	// Ok, see if we have any static entity proxies
	pSet = geWorld_GetEntitySet(CCD->World(), "StaticEntityProxy");

	if(!pSet)
		return RGF_SUCCESS;

	// Ok, we have static entities somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
// changed RF063
		StaticEntityProxy *pProxy = (StaticEntityProxy*)geEntity_GetUserData(pEntity);

		// changed QD 07/15/06
		if(!pProxy->Actor)
			continue;
		// end change

		READDATA(&pProxy->origin,			sizeof(geVec3d),	1, RestoreFD);
		READDATA(&pProxy->CallBack,			sizeof(geBoolean),	1, RestoreFD);
		READDATA(&pProxy->CallBackCount,	sizeof(int),		1, RestoreFD);
		READDATA(&pProxy->bState,			sizeof(geBoolean),	1, RestoreFD);
		READDATA(&pProxy->DoingDamage,		sizeof(geBoolean),	1, RestoreFD);
		READDATA(&pProxy->Time,				sizeof(geFloat),	1, RestoreFD);
		READDATA(&pProxy->alive,			sizeof(geBoolean),	1, RestoreFD);
		READDATA(&pProxy->IsHit,			sizeof(geBoolean),	1, RestoreFD);
		READDATA(&pProxy->bInitialized,		sizeof(geBoolean),	1, RestoreFD);

		if(pProxy->alive || (!pProxy->alive && !pProxy->DeathDissappear))
		{
			int health;
			READDATA(&health, sizeof(int), 1, RestoreFD);
			CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(pProxy->Actor);

			if(!EffectC_IsStringNull(pProxy->DamageAttribute))
				theInv->Set(pProxy->DamageAttribute, health);
			else
				theInv->Set("health", health);

			CCD->ActorManager()->Position(pProxy->Actor, pProxy->origin);
		}
		else
			CCD->ActorManager()->RemoveActor(pProxy->Actor);
// end change RF063
	}

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	BindToPath
//
//	Given the name of an entity, bind that entity to a motion path.
/* ------------------------------------------------------------------------------------ */
int CStaticEntity::BindToPath(char *szName)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	// Ok, check to see if there are static entities in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "StaticEntityProxy");

	if(!pSet)
		return RGF_FAILURE;						// No props

	// Ok, we have static entities somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		StaticEntityProxy *pSource = (StaticEntityProxy*)geEntity_GetUserData(pEntity);

		if(!strcmp(pSource->szEntityName, szName))
		{
			pSource->bFollower = GE_TRUE;
			return RGF_SUCCESS;					// Flagged and good to go
		}
	}

	return RGF_FAILURE;							// No such entity, sorry
}

/* ------------------------------------------------------------------------------------ */
//	GetEntity
/* ------------------------------------------------------------------------------------ */
int CStaticEntity::GetEntity(geActor *Actor, void **pEntityData)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	// Ok, check to see if there are static entity proxies in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "StaticEntityProxy");

	if(!pSet)
		return RGF_NOT_FOUND;									// No static entity proxies

	// Ok, we have static entity proxies.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		StaticEntityProxy *pTheEntity = (StaticEntityProxy*)geEntity_GetUserData(pEntity);

		// changed QD 07/15/06
		if(!pTheEntity->Actor)
			continue;
		// end change

		if(pTheEntity->Actor == Actor)
		{
			*pEntityData = (void*)pTheEntity;
			return RGF_SUCCESS;
		}
	}

	return RGF_NOT_FOUND;								// Sorry, no such entity here
}


//	******************* PRIVATE MEMBER FUCNTIONS ***************


//	******************** CRGF Overrides ********************

/* ------------------------------------------------------------------------------------ */
//	LocateEntity
//
//	Given a name, locate the desired item in the currently loaded level
//	..and return it's user data.
/* ------------------------------------------------------------------------------------ */
int CStaticEntity::LocateEntity(char *szName, void **pEntityData)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	// Ok, check to see if there are static entity proxies in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "StaticEntityProxy");

	if(!pSet)
		return RGF_NOT_FOUND;									// No static entity proxies

	// Ok, we have static entity proxies.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		StaticEntityProxy *pTheEntity = (StaticEntityProxy*)geEntity_GetUserData(pEntity);

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
int CStaticEntity::ReSynchronize()
{
	return RGF_SUCCESS;
}

/* ----------------------------------- END OF FILE ------------------------------------ */
