/************************************************************************************//**
 * @file CMovingPlatforms.cpp
 * @brief Moving Platform Handler
 *
 * This file contains the class implementation for the CMovingPlatforms class that
 * encapsulates moving platform handling in the RGF.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

//	You only need the one, master include file.
#include "RabidFramework.h"

extern geSound_Def *SPool_Sound(const char *SName);
extern "C" void	DrawBoundBox(geWorld *World, const geVec3d *Pos, const geVec3d *Min, const geVec3d *Max);

/* ------------------------------------------------------------------------------------ */
//	CMovingPlatforms
//
//	Default constructor, clear all user data to defaults and load audio.
/* ------------------------------------------------------------------------------------ */
CMovingPlatforms::CMovingPlatforms()
{
	geEntity *pEntity;
// MOD010122 - Added next four lines of data declarations.
	geFloat  TList[TIME_LIST_MAX], tStart, tEnd;
	int      i;
	geMotion *pMotion;
	const char *Eventstring;

	m_PlatformCount = 0;					// No doors

	// Ok, check to see if there are platforms in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "MovingPlatform");

	if(!pSet)
		return;									// No platforms, how odd...

	// Ok, we have platforms somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		MovingPlatform *pPlatform = static_cast<MovingPlatform*>(geEntity_GetUserData(pEntity));

		if(EffectC_IsStringNull(pPlatform->szEntityName))
		{
			char szName[128];
			geEntity_GetName(pEntity, szName, 128);
			pPlatform->szEntityName = szName;
		}

		if(!pPlatform->Model)
		{
			char szError[256];
			sprintf(szError,"[WARNING] File %s - Line %d: %s: Missing Model\n",
					__FILE__, __LINE__, pPlatform->szEntityName);
			CCD->ReportError(szError, false);
			// changed QD 07/15/06
			/*
			CCD->ShutdownLevel();
			delete CCD;
			CCD = NULL;
			MessageBox(NULL, szError,"Missing Platform Model", MB_OK);
			exit(-333);
			*/
			continue;
		}

		m_PlatformCount++;							// Kick door count

		// Ok, put this entity into the Global Entity Registry
		CCD->EntityRegistry()->AddEntity(pPlatform->szEntityName, "MovingPlatform");

		// Now add the platform into the Model Manager, which will do all the
		// ..hard work for us!
		CCD->ModelManager()->AddModel(pPlatform->Model, ENTITY_PLATFORM);
		CCD->ModelManager()->SetLooping(pPlatform->Model, pPlatform->bLooping);
		CCD->ModelManager()->SetReverse(pPlatform->Model, pPlatform->bReverse);
		CCD->ModelManager()->SetAllowInside(pPlatform->Model, pPlatform->bAllowInside);
// Start Aug2003DCS
		CCD->ModelManager()->SetModelOrigin(pPlatform->Model, &(pPlatform->origin));
		CCD->ModelManager()->SetTargetTime(pPlatform->Model, -1.0f);
// End Aug2003DCS
// Start Added By Pickles
		CCD->ModelManager()->SetRideable(pPlatform->Model, pPlatform->Rideable);
// End Added by Pickles
// MOD010122 - Start of new data initializations
		CCD->ModelManager()->SetOneShot(pPlatform->Model, pPlatform->bOneShot);
		CCD->ModelManager()->SetRunTimed(pPlatform->Model, pPlatform->bRunTimed);
		CCD->ModelManager()->SetTimeEachTrig(pPlatform->Model, pPlatform->TimeEachTrig);

		if(pPlatform->bRunFromList)
		{
			CCD->ModelManager()->SetRunFromList(pPlatform->Model, GE_TRUE);

			for(i=0; i<TIME_LIST_MAX; i++)
			{
				TList[i] = -1.0f;
			}

			sscanf(pPlatform->TimeList, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f",
				&TList[0],	&TList[1],	&TList[2],	&TList[3],	&TList[4],
				&TList[5],	&TList[6],	&TList[7],	&TList[8],	&TList[9],
				&TList[10], &TList[11], &TList[12], &TList[13], &TList[14],
				&TList[15], &TList[16], &TList[17], &TList[18], &TList[19]);

			CCD->ModelManager()->SetTimeList(pPlatform->Model, TList);
		}

		if(pPlatform->bRunToNextEvent)
		{
			pMotion = geWorld_ModelGetMotion(pPlatform->Model);

			if(pMotion)					// Have motion data?
			{
				CCD->ModelManager()->SetRunFromList(pPlatform->Model, GE_TRUE);

				for(i=0; i<TIME_LIST_MAX; i++)
				{
					TList[i] = -1.0f;
				}

				geMotion_GetTimeExtents(pMotion, &tStart, &tEnd);
				geMotion_SetupEventIterator(pMotion, tStart, tEnd);
				i = 0;

				while (geMotion_GetNextEvent(pMotion, &tStart, &Eventstring) &&
					(i < TIME_LIST_MAX))
				{
					char Arg[6][128];
					memset(Arg, 0, sizeof(Arg));
					sscanf(Eventstring, "%s %s %s %s %s %s",
						&Arg[0], &Arg[1], &Arg[2], &Arg[3], &Arg[4], &Arg[5]);

					if(Arg[0][0] == 'S')
					{
						if(!EffectC_IsStringNull(Arg[1]))
							SPool_Sound(Arg[1]);
					}

					TList[i] = tStart;
					i++;
				}

				CCD->ModelManager()->SetTimeList(pPlatform->Model, TList);
			}
		}
// MOD010122 - End of new data initializations
		CCD->ModelManager()->SetAnimationSpeed(pPlatform->Model, pPlatform->AnimationSpeed);

		// Reset all the animation data for each and every door
		pPlatform->bInAnimation		= GE_FALSE;
		pPlatform->bTrigger			= GE_FALSE;		// Not triggered
		pPlatform->bInCollision		= GE_FALSE;		// No collisions
		pPlatform->bActive			= GE_TRUE;		// Platform is good to go
		// EFFECT
		pPlatform->CallBack			= GE_FALSE;
		pPlatform->LastIncrement	= 0;			// No last time count
		pPlatform->bActorOnMe		= GE_FALSE;		// No actor on platform
		pPlatform->theSound			= NULL;			// No sound, right now...
		pPlatform->SoundHandle		= -1;			// No sound playing
		pPlatform->bForward			= GE_TRUE;		// Animation runs forward

		geExtBox theBBox;
		geWorld_ModelGetBBox(CCD->World(), pPlatform->Model, &theBBox.Min, &theBBox.Max);
		theBBox.Min.X += 126.0f;
		theBBox.Min.Y += 126.0f;
		theBBox.Min.Z += 126.0f;
		theBBox.Max.X -= 126.0f;
		theBBox.Max.Y -= 126.0f;
		theBBox.Max.Z -= 126.0f;

		CCD->ModelManager()->SetBoundingBox(pPlatform->Model, theBBox);

		if(!EffectC_IsStringNull(pPlatform->szSoundFile))
		{
			pPlatform->theSound = SPool_Sound(pPlatform->szSoundFile);

			if(!pPlatform->theSound)
			{
				char szError[256];
				sprintf(szError, "[WARNING] File %s - Line %d: %s: Failed to open audio file '%s'\n",
						__FILE__, __LINE__, pPlatform->szEntityName, pPlatform->szSoundFile);
				CCD->ReportError(szError, false);
			}
		}

		// Ok, if the platform is autostart, set it to running
		if(pPlatform->bAutoStart == GE_TRUE)
		{
			CCD->ModelManager()->Start(pPlatform->Model);	// Start me up!
			pPlatform->bTrigger = GE_TRUE;					// Trigger right off
		}
	}

// Start Aug2003DCS
	// Run through the platforms again to see if any are attached to another model
	pSet = geWorld_GetEntitySet(CCD->World(), "MovingPlatform");

	if(!pSet)
		return;									// No platforms, how odd...

	// Ok, we have platforms somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		MovingPlatform *pPlatform = static_cast<MovingPlatform*>(geEntity_GetUserData(pEntity));

		// changed QD 07/15/06
		if(!pPlatform->Model)
			continue;
		// end change

		if(pPlatform->ParentModel)
		{
			CCD->ModelManager()->SetParentModel(pPlatform->Model, pPlatform->ParentModel);
			pPlatform->OriginOffset = pPlatform->origin;
			geVec3d ModOrg;
			geWorld_GetModelRotationalCenter(CCD->World(), pPlatform->Model, &ModOrg);

			geVec3d ModelOrigin;
			geWorld_GetModelRotationalCenter(CCD->World(), pPlatform->ParentModel, &ModelOrigin);
			geVec3d_Subtract(&ModOrg, &ModelOrigin, &pPlatform->OriginOffset);
			CCD->ModelManager()->SetModelOriginOffset(pPlatform->Model, &pPlatform->OriginOffset);
		}
	}
// End Aug2003DCS

// Start Aug2003DCS
	// Run through the doors again to see if any are attached to another model
	// We have to do this here so that possible parent (moving platform) models will have been created
	pSet = geWorld_GetEntitySet(CCD->World(), "Door");

	if(!pSet)
		return;									// No platforms, how odd...

	// Ok, we have platforms somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Door *pDoor = static_cast<Door*>(geEntity_GetUserData(pEntity));

		// changed QD 07/15/06
		if(!pDoor->Model)
			continue;
		// end change

		if(pDoor->ParentModel)
		{
			CCD->ModelManager()->SetParentModel(pDoor->Model, pDoor->ParentModel);
			pDoor->OriginOffset = pDoor->origin;
			geVec3d ModOrg;
			geWorld_GetModelRotationalCenter(CCD->World(), pDoor->Model, &ModOrg);

			geVec3d ModelOrigin;
			geWorld_GetModelRotationalCenter(CCD->World(), pDoor->ParentModel, &ModelOrigin);
			geVec3d_Subtract(&ModOrg, &ModelOrigin, &pDoor->OriginOffset);
			CCD->ModelManager()->SetModelOriginOffset(pDoor->Model, &pDoor->OriginOffset);
		}
	}
// End Aug2003DCS

	// Ok, we've counted the platform and reset 'em all to their default values.
}

/* ------------------------------------------------------------------------------------ */
//	~CMovingPlatforms
//
//	Default destructor, clean up anything we allocated on construction
/* ------------------------------------------------------------------------------------ */
CMovingPlatforms::~CMovingPlatforms()
{
}

/* ------------------------------------------------------------------------------------ */
//	PlaySound
/* ------------------------------------------------------------------------------------ */
// changed QD 12/15/05 - changed 2nd argument from geVec3d to const geVec3d&
int CMovingPlatforms::PlaySound(geSound_Def *theSound, const geVec3d &Origin, bool SoundLoop)
{
	if(!theSound)
		return -1;

	Snd Sound;

	memset(&Sound, 0, sizeof(Sound));
	geVec3d_Copy(&Origin, &(Sound.Pos));
	Sound.Min = CCD->GetAudibleRadius();
// changed RF064
	Sound.Loop = SoundLoop;
// end change RF064
	Sound.SoundDef = theSound;
	int index = CCD->EffectManager()->Item_Add(EFF_SND, static_cast<void*>(&Sound));

	if(SoundLoop)
		return index;

	return -1;
}

/* ------------------------------------------------------------------------------------ */
//	HandleCollision
//
//	Handle a collision with a platform.
/* ------------------------------------------------------------------------------------ */
// changed RF063
bool CMovingPlatforms::HandleCollision(const geWorld_Model *pModel, bool bTriggerCall,
									   bool UseKey, const geActor *theActor)
{
	geEntity *pEntity;

	if(m_PlatformCount == 0)
		return false;									// None here, ignore call.

	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "MovingPlatform");

	if(!pSet)
		return false;

	// Once more we scan the platform list.  Does this get old, or what?
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		// Get the platform data so we can compare models
		MovingPlatform *pPlatform = static_cast<MovingPlatform*>(geEntity_GetUserData(pEntity));

		// changed QD 07/15/06
		if(!pPlatform->Model)
			continue;
		// end change

		if(pPlatform->Model == pModel)
		{
// changed RF063
			if(pPlatform->PlayerOnly && theActor!=CCD->Player()->GetActor())
				return false;

			if(UseKey && !pPlatform->UseKey)
				return false;
// end change RF063

			if((!pPlatform->bShoot) && bTriggerCall)
				return false;

			if(pPlatform->bShoot && !bTriggerCall)
				return false;

			// If this platform doesn't activate on collision, and this call isn't
			// ..from a platform trigger, ignore it.
// changed RF063
			if(pPlatform->bNoCollide && !UseKey)
				return true;	// Fake a no-hit situation
// end change RF063

			bool state = true;

			if(!EffectC_IsStringNull(pPlatform->TriggerName))
				state = GetTriggerState(pPlatform->TriggerName);

			if(!state)
			{
				pPlatform->CallBack = GE_TRUE;
				pPlatform->CallBackCount = 2;
				return true;
			}

			// MOD010122 - Next section of code has many changes, you just can't call Start anymore!
			//            You might have to call ReStart.
			// If the platform isn't animating and it's available, start it!
			// Ok, if the entity isn't already activated AND the entity is
			// ..available for activation AND it's not already triggered,
			// ..ACTIVATE IT!
			if((!pPlatform->bInAnimation) && (pPlatform->bActive == GE_TRUE) &&
				(!pPlatform->bTrigger) && state)
			{
				pPlatform->bTrigger = GE_TRUE;			// It's this one, trigger the animation
// changed RF064
				pPlatform->bInAnimation = GE_TRUE;
// end change RF064

				if(pPlatform->bRunWhileTrig || pPlatform->bRunFromList ||
					pPlatform->bRunTimed    || pPlatform->bRunToNextEvent)
				{
					if (CCD->ModelManager()->HasMoved(pPlatform->Model))
						CCD->ModelManager()->ReStart(pPlatform->Model);
					else
						CCD->ModelManager()->Start(pPlatform->Model);
				}
				else
					CCD->ModelManager()->Start(pPlatform->Model);

				// MOD010122 - End of changed section.
				pPlatform->SoundHandle = PlaySound(pPlatform->theSound, pPlatform->origin, pPlatform->bAudioLoops);
			}

			// Now check to see if there's a linked door that we also need to
			// ..trigger.
			if(pPlatform->NextToTrigger != NULL)
				TriggerNextPlatform(pPlatform->NextToTrigger, bTriggerCall);

			return true;									// Hmmph, we hit a platform!
		}
	}

	return false;							// We hit no known platforms
}

/* ------------------------------------------------------------------------------------ */
//	TriggerNextPlatform
//
//	Trigger platforms that are linked.  Note that this call IS RECURSIVE
//	..and will let you trigger a whole sequence of platforms from one
//	..collision.  Also, if you want fixed dual platforms the links CAN
//	..point at each other as a recursive call is NOT made if the platform
//	..being triggered here is already triggered.
/* ------------------------------------------------------------------------------------ */
void CMovingPlatforms::TriggerNextPlatform(const geWorld_Model *pModel, bool bTriggerCall)
{
	geEntity *pEntity;

	if(m_PlatformCount == 0)
		return;									// None here, ignore call.

	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "MovingPlatform");

	if(!pSet)
	{
		CCD->ReportError("CMovingPlatforms: TriggerNextPlatform: no platforms", false);
		return;
	}

	// Once more we scan the platform list.  Does this get old, or what?
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		// Get the platform data so we can compare models
		MovingPlatform *pPlatform = static_cast<MovingPlatform*>(geEntity_GetUserData(pEntity));

		// changed QD 07/15/06
		if(!pPlatform->Model)
			continue;
		// end change

		if(pPlatform->Model == pModel)
		{
			if((!pPlatform->bShoot) && bTriggerCall)
				return;

			// Models match, we hit this one.  If the entity doesn't activate
			// ..on collide AND this isn't a call from a trigger, don't
			// ..activate the entity.

			bool state = true;

			if(!EffectC_IsStringNull(pPlatform->TriggerName))
				state = GetTriggerState(pPlatform->TriggerName);

			// Ok, if the entity isn't already activated AND the entity is
			// ..available for activation AND it's not already triggered,
			// ..ACTIVATE IT!
			if((!pPlatform->bInAnimation) && (pPlatform->bActive == GE_TRUE) &&
				(!pPlatform->bTrigger) && state)
			{
				pPlatform->bTrigger = GE_TRUE;			// It's this one, trigger the animation
// changed RF064
				pPlatform->bInAnimation = GE_TRUE;
// end change RF064
// MOD010122 - Changed this section too.  You just can't call Start anymore!
				//            You might have to call ReStart.
				if(pPlatform->bRunWhileTrig || pPlatform->bRunFromList ||
					pPlatform->bRunTimed    || pPlatform->bRunToNextEvent)
				{
					if(CCD->ModelManager()->HasMoved(pPlatform->Model))
						CCD->ModelManager()->ReStart(pPlatform->Model);
					else
						CCD->ModelManager()->Start(pPlatform->Model);
				}
				else
					CCD->ModelManager()->Start(pPlatform->Model);
// MOD010122 - End of changed section

				pPlatform->SoundHandle = PlaySound(pPlatform->theSound, pPlatform->origin, pPlatform->bAudioLoops);
				// Now check to see if there's a linked plaform that we also need to
				// ..trigger.
				if(pPlatform->NextToTrigger != NULL)
					TriggerNextPlatform(pPlatform->NextToTrigger, bTriggerCall);
			}
			return;							// Hmmph, we hit a platform
		}
	}

	return;									// We hit no known platforms
}

/* ------------------------------------------------------------------------------------ */
//	IsAPlatform
//
//	Return TRUE if the passed-in model is a platform, FALSE otherwise
/* ------------------------------------------------------------------------------------ */
bool CMovingPlatforms::IsAPlatform(const geWorld_Model *theModel)
{
	geEntity *pEntity;

	if(m_PlatformCount == 0)
		return false;						// Don't waste time here

	// Ok, check to see if there are platforms in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "MovingPlatform");

	if(!pSet)
		return false;						// No platforms, how odd...

	// Ok, we have platforms somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		MovingPlatform *pPlatform = static_cast<MovingPlatform*>(geEntity_GetUserData(pEntity));

		// changed QD 07/15/06
		if(!pPlatform->Model)
			continue;
		// end change

		if(pPlatform->Model == theModel)
			return true;					// Model IS a platform
	}

	return false;							// Nope, it's not a platform
}

/* ------------------------------------------------------------------------------------ */
//	Tick
//
//	Increment animation times for all _animating_ platforms that aren't
//	..in a collision state.
/* ------------------------------------------------------------------------------------ */
void CMovingPlatforms::Tick(geFloat dwTicks)
{
	geEntity *pEntity;

	// Ok, check to see if there are platforms in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "MovingPlatform");

	if(!pSet)
		return;									// No platforms, how odd...

	// Ok, we have platforms somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		MovingPlatform *pPlatform = static_cast<MovingPlatform*>(geEntity_GetUserData(pEntity));

		if(!pPlatform->Model)
			continue;

		if(!EffectC_IsStringNull(pPlatform->TriggerName))
		{
			if(GetTriggerState(pPlatform->TriggerName))
			{
				if((!pPlatform->bInAnimation) && (pPlatform->bActive == GE_TRUE) &&
					(!pPlatform->bTrigger) && (pPlatform->bNoCollide))
				{
					pPlatform->bTrigger = GE_TRUE;			// It's this one, trigger the animation
					// MOD010122 - Changed this section too.  You just can't call Start anymore!
					//            You might have to call ReStart.

					if(pPlatform->bRunWhileTrig || pPlatform->bRunFromList ||
						pPlatform->bRunTimed    || pPlatform->bRunToNextEvent)
					{
						if(CCD->ModelManager()->HasMoved(pPlatform->Model))
							CCD->ModelManager()->ReStart(pPlatform->Model);
						else
							CCD->ModelManager()->Start(pPlatform->Model);
					}
					else
						CCD->ModelManager()->Start(pPlatform->Model);

					pPlatform->bInAnimation = GE_TRUE;

					// MOD010122 - End of changed section
					pPlatform->SoundHandle = PlaySound(pPlatform->theSound, pPlatform->origin, pPlatform->bAudioLoops);

					// Now check to see if there's a linked door that we also need to trigger.
					if(pPlatform->NextToTrigger != NULL)
						TriggerNextPlatform(pPlatform->NextToTrigger, false);
				}
// changed RF064
				else
				{
					//If bRunWhileTrig is true then un-trigger the animation
					pPlatform->bTrigger = GE_FALSE;
					CCD->ModelManager()->Stop(pPlatform->Model);

					if(pPlatform->SoundHandle != -1)
					{
						CCD->EffectManager()->Item_Delete(EFF_SND, pPlatform->SoundHandle);
						pPlatform->SoundHandle = -1;
					}
				}
// end change RF064
			}
			//MOD010122 - The next section of code was added to handle the case when the
			//            trigger goes off and we are animating and we are in RunWhileTrig mode.
			else
			{
				if(pPlatform->bInAnimation && (pPlatform->bActive == GE_TRUE) &&
					pPlatform->bTrigger && pPlatform->bNoCollide && pPlatform->bRunWhileTrig)
				{
					//If bRunWhileTrig is true then un-trigger the animation
					pPlatform->bTrigger = GE_FALSE;
					CCD->ModelManager()->Stop(pPlatform->Model);

					if(pPlatform->SoundHandle != -1)
					{
						CCD->EffectManager()->Item_Delete(EFF_SND, pPlatform->SoundHandle);
						pPlatform->SoundHandle = -1;
					}
				}
			}
		}

		if(pPlatform->CallBack == GE_TRUE)
		{
			pPlatform->CallBackCount -= 1;

			if(pPlatform->CallBackCount == 0)
				pPlatform->CallBack = GE_FALSE;
		}

		if(CCD->ModelManager()->IsRunning(pPlatform->Model))
		{
			if(pPlatform->SoundHandle != -1)
			{
				geVec3d thePosition;
				CCD->ModelManager()->GetPosition(pPlatform->Model, &thePosition);
				Snd Sound;
				geVec3d_Copy(&thePosition, &(Sound.Pos));
				CCD->EffectManager()->Item_Modify(EFF_SND, pPlatform->SoundHandle, static_cast<void*>(&Sound), SND_POS);
			}
		}

		if((pPlatform->bInAnimation == GE_TRUE) &&
			(CCD->ModelManager()->IsRunning(pPlatform->Model) == false))
		{
			// Animation has stopped/not running, handle it.
			if((pPlatform->bOneShot != GE_TRUE) && (pPlatform->bActive == GE_TRUE))
			{
				// Ok, not one-shot, reset the door
				pPlatform->bInAnimation = GE_FALSE;
				pPlatform->bTrigger = GE_FALSE;
			}

			if(pPlatform->SoundHandle != -1)
			{
				CCD->EffectManager()->Item_Delete(EFF_SND, pPlatform->SoundHandle);
				pPlatform->SoundHandle = -1;
			}
		}

// Start Aug2003DCS
		//Handle the case where the model is animating on command from a script
		if(CCD->ModelManager()->IsRunning(pPlatform->Model)
			&& (pPlatform->bInAnimation == GE_FALSE)
			&& (pPlatform->SoundHandle == -1))
		{
			pPlatform->SoundHandle = PlaySound(pPlatform->theSound, pPlatform->origin, pPlatform->bAudioLoops);
		}

		if(!CCD->ModelManager()->IsRunning(pPlatform->Model) && (pPlatform->SoundHandle != -1))
		{
			CCD->EffectManager()->Item_Delete(EFF_SND, pPlatform->SoundHandle);
			pPlatform->SoundHandle = -1;
		}
// End Aug2003DCS
	}

	return;
}

/* ------------------------------------------------------------------------------------ */
//	SaveTo
//
//	Save the current state of every platform in the current world
//	..off to an open file.
/* ------------------------------------------------------------------------------------ */
int CMovingPlatforms::SaveTo(FILE *SaveFD, bool type)
{
	geEntity *pEntity;

	// Ok, check to see if there are platforms in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "MovingPlatform");

	if(!pSet)
		return RGF_SUCCESS;									// No platforms, whatever...

	// Ok, we have platforms somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		MovingPlatform *pPlatform = static_cast<MovingPlatform*>(geEntity_GetUserData(pEntity));

		WRITEDATA(type, &pPlatform->bInAnimation,	sizeof(geBoolean),	1, SaveFD);
		WRITEDATA(type, &pPlatform->bTrigger,		sizeof(geBoolean),	1, SaveFD);
		WRITEDATA(type, &pPlatform->bActorOnMe,		sizeof(geBoolean),	1, SaveFD);
		WRITEDATA(type, &pPlatform->bInCollision,	sizeof(geBoolean),	1, SaveFD);
		WRITEDATA(type, &pPlatform->bActive,		sizeof(geBoolean),	1, SaveFD);
		WRITEDATA(type, &pPlatform->LastIncrement,	sizeof(int),		1, SaveFD);
		WRITEDATA(type, &pPlatform->bForward,		sizeof(geBoolean),	1, SaveFD);
		WRITEDATA(type, &pPlatform->tPlatform,		sizeof(geFloat),	1, SaveFD);
		WRITEDATA(type, &pPlatform->CallBack,		sizeof(geBoolean),	1, SaveFD);
		WRITEDATA(type, &pPlatform->CallBackCount,	sizeof(int),		1, SaveFD);
	}

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	RestoreFrom
//
//	Restore the state of every platform in the current world from an open file.
/* ------------------------------------------------------------------------------------ */
int CMovingPlatforms::RestoreFrom(FILE *RestoreFD, bool type)
{
	geEntity *pEntity;

	// Ok, check to see if there are platforms in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "MovingPlatform");

	if(!pSet)
		return RGF_SUCCESS;									// No platforms, whatever...

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		MovingPlatform *pPlatform = static_cast<MovingPlatform*>(geEntity_GetUserData(pEntity));

		READDATA(type, &pPlatform->bInAnimation,	sizeof(geBoolean),	1, RestoreFD);
		READDATA(type, &pPlatform->bTrigger,		sizeof(geBoolean),	1, RestoreFD);
		READDATA(type, &pPlatform->bActorOnMe,		sizeof(geBoolean),	1, RestoreFD);
		READDATA(type, &pPlatform->bInCollision,	sizeof(geBoolean),	1, RestoreFD);
		READDATA(type, &pPlatform->bActive,			sizeof(geBoolean),	1, RestoreFD);
		READDATA(type, &pPlatform->LastIncrement,	sizeof(int),		1, RestoreFD);
		READDATA(type, &pPlatform->bForward,		sizeof(geBoolean),	1, RestoreFD);
		READDATA(type, &pPlatform->tPlatform,		sizeof(geFloat),	1, RestoreFD);
		READDATA(type, &pPlatform->CallBack,		sizeof(geBoolean),	1, RestoreFD);
		READDATA(type, &pPlatform->CallBackCount,	sizeof(int),		1, RestoreFD);

		if(pPlatform->bInAnimation)
			geWorld_OpenModel(CCD->World(), pPlatform->Model, GE_TRUE);
	}

	return RGF_SUCCESS;
}

//	******************** CRGF Overrides ********************

/* ------------------------------------------------------------------------------------ */
//	LocateEntity
//
//	Given a name, locate the desired item in the currently loaded level
//	..and return it's user data.
/* ------------------------------------------------------------------------------------ */
int CMovingPlatforms::LocateEntity(const char *szName, void **pEntityData)
{
	geEntity *pEntity;

	// Ok, check to see if there are moving platforms in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "MovingPlatform");

	if(!pSet)
		return RGF_NOT_FOUND;									// No moving platforms

	// Ok, we have moving platforms.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		MovingPlatform *pTheEntity = static_cast<MovingPlatform*>(geEntity_GetUserData(pEntity));

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
int CMovingPlatforms::ReSynchronize()
{
	return RGF_SUCCESS;
}

/* ----------------------------------- END OF FILE ------------------------------------ */

