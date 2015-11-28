/*
CAutoDoors.cpp:		Automatic door handler

  (c) 2001 Ralph Deane
  
	This file contains the class implementation for the CAutoDoors
	class that encapsulates automatic-door handling in the RGF.
*/

//	You only need the one, master include file.

#include "RabidFramework.h"

extern geSound_Def *SPool_Sound(char *SName);

//	CAutoDoors
//
//	Default constructor.  Go through and set the user data for each door
//	..to their default values.

CAutoDoors::CAutoDoors()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	geFloat  TList[TIME_LIST_MAX], tStart, tEnd;
	int      i;
	geMotion *pMotion;
	const char     *Eventstring;
	
	m_DoorCount = 0;					// No doors
	
	//	Ok, check to see if there are automatic doors in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "Door");
	
	if(!pSet) 
		return;									// No doors, how odd...
	
	//	Ok, we have doors somewhere.  Dig through 'em all.
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		Door *pDoor = (Door*)geEntity_GetUserData(pEntity);
		
		if(EffectC_IsStringNull(pDoor->szEntityName))
		{
			char szName[128];
			geEntity_GetName(pEntity, szName, 128);
			pDoor->szEntityName = szName;
		}
		m_DoorCount++;							// Kick door count
		// Ok, put this entity into the Global Entity Registry
		CCD->EntityRegistry()->AddEntity(pDoor->szEntityName, "Door");
		// Now add the platform into the Model Manager, which will do all the
		// ..hard work for us!
		if(!pDoor->Model)
		{
			char szError[256];
			sprintf(szError,"Doors : Missing Model '%s'", pDoor->szEntityName);
			CCD->ReportError(szError, false);
			CCD->ShutdownLevel();
			delete CCD;
			CCD = NULL;
			MessageBox(NULL, szError,"Missing Door Model", MB_OK);
			exit(-333);
		}
		CCD->ModelManager()->AddModel(pDoor->Model, ENTITY_DOOR);
		CCD->ModelManager()->SetLooping(pDoor->Model, false);
		CCD->ModelManager()->SetReverseOnCollide(pDoor->Model, pDoor->bReverse);
		CCD->ModelManager()->SetRotating(pDoor->Model, pDoor->bRotating);
		CCD->ModelManager()->SetAnimationSpeed(pDoor->Model, pDoor->AnimationSpeed);
		CCD->ModelManager()->SetOneShot(pDoor->Model, pDoor->bOneShot);
		CCD->ModelManager()->SetRunTimed(pDoor->Model, pDoor->bRunTimed);
		CCD->ModelManager()->SetTimeEachTrig(pDoor->Model, pDoor->TimeEachTrig);
		if (pDoor->bRunFromList)
		{
			CCD->ModelManager()->SetRunFromList(pDoor->Model, GE_TRUE);
			for (i = 0; i < TIME_LIST_MAX; i++)
			{
				TList[i] = -1.0f;
			}
			sscanf(pDoor->TimeList,
				"%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f",
				&TList[0], &TList[1], &TList[2], &TList[3], &TList[4], 
				&TList[5], &TList[6], &TList[7], &TList[8], &TList[9],
				&TList[10], &TList[11], &TList[12], &TList[13], &TList[14], 
				&TList[15], &TList[16], &TList[17], &TList[18], &TList[19]);
			CCD->ModelManager()->SetTimeList(pDoor->Model, TList);
		}
		if (pDoor->bRunToNextEvent)
		{
			pMotion = geWorld_ModelGetMotion(pDoor->Model);
			if(pMotion)					// Have motion data?
			{
				CCD->ModelManager()->SetRunFromList(pDoor->Model, GE_TRUE);
				for (i = 0; i < TIME_LIST_MAX; i++)
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
                    sscanf(Eventstring, 
                          "%s %s %s %s %s %s",
                           &Arg[0], &Arg[1], &Arg[2], &Arg[3], &Arg[4], &Arg[5]);
                    if(Arg[0][0]=='S')
                    {
						if(!EffectC_IsStringNull(Arg[1]))
                            SPool_Sound(Arg[1]);
					}
					TList[i] = tStart;
					i++;
				}
				CCD->ModelManager()->SetTimeList(pDoor->Model, TList);
			}
		}
		// Reset all the animation data for each and every door
		pDoor->bInAnimation= GE_FALSE;
		pDoor->AnimationTime = 0;						// No time in animation
		pDoor->bTrigger = GE_FALSE;					// Not triggered
		pDoor->CallBack = GE_FALSE;
		pDoor->bInCollision = GE_FALSE;			// No collisions
		pDoor->bActive = GE_TRUE;						// Door is good to go
		pDoor->LastIncrement = 0;						// No last time count
		pDoor->theSound = NULL;							// No sound, right now...
		pDoor->SoundHandle = -1;
		geExtBox theBBox;
		geWorld_ModelGetBBox(CCD->World(), pDoor->Model,
			&theBBox.Min, &theBBox.Max);
		theBBox.Min.X += 126.0f;
		theBBox.Min.Y += 126.0f;
		theBBox.Min.Z += 126.0f;
		theBBox.Max.X -= 126.0f;
		theBBox.Max.Y -= 126.0f;
		theBBox.Max.Z -= 126.0f;
		CCD->ModelManager()->SetBoundingBox(pDoor->Model, theBBox);
		if(!EffectC_IsStringNull(pDoor->szSoundFile))
		{
			pDoor->theSound=SPool_Sound(pDoor->szSoundFile);
			
			if(!pDoor->theSound)
			{
				char szError[256];
				sprintf(szError,"Can't open audio file '%s'\n", pDoor->szSoundFile);
				CCD->ReportError(szError, false);
			}
		}
	}
	
	//	Ok, we've counted the doors and reset 'em all to their default
	//	..values.  Leave!
	
	return;
}

//	~CAutoDoors
//
//	Default destructor.  Clean up the audio allocated by all doors on the
//	..way out.

CAutoDoors::~CAutoDoors()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	if(m_DoorCount == 0)
		return;											// No need to waste time here.
	
	//	Ok, check to see if there are automatic doors in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "Door");
	
	if(!pSet) 
		return;									// No doors, how odd...
	
	//	Ok, we have doors somewhere.  Dig through 'em all and release
	//	..the audio (if any).
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		
	}
	
	//	Doors cleaned up.  Bail this mess.
	
	return;
}

//	HandleCollision
//
//	Handle a collision with an automatic door.
// changed RF063
bool CAutoDoors::HandleCollision(geWorld_Model *pModel,	bool bTriggerCall, bool UseKey, geActor *theActor)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	if(m_DoorCount == 0)
		return false;									// None here, ignore call.
	
	pSet = geWorld_GetEntitySet(CCD->World(), "Door");
	if(!pSet)
	{
		CCD->ReportError("CAutoDoors: handlecollision: no doors", false);
		return false;
	}
	
	//	Once more we scan the door list.  Does this get old, or what?
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		// Get the door data so we can compare models
		Door *pDoor= (Door*)geEntity_GetUserData(pEntity);
		if(pDoor->Model == pModel)
		{
// changed RF063
			if(pDoor->PlayerOnly && theActor!=CCD->Player()->GetActor())
				return false;
			if(UseKey && !pDoor->UseKey)
				return false;
// end change RF063
			if((!pDoor->bShoot) && bTriggerCall)
				return false;
			if(pDoor->bShoot && !bTriggerCall)
				return false;
			// Models match, we hit this one.  If the entity doesn't activate
			// ..on collide AND this isn't a call from a trigger, don't
			// ..activate the entity.
// changed RF063
			if(pDoor->bNoCollide && !UseKey)
				return true;			// Fake a no-hit situation
// end change RF063
			bool state = true;
			if(!EffectC_IsStringNull(pDoor->TriggerName))
				state = GetTriggerState(pDoor->TriggerName);
			if(!state)
			{
				pDoor->CallBack = GE_TRUE;
				pDoor->CallBackCount = 2;
				return true;
			}
			
			// Ok, if the entity isn't already activated AND the entity is
			// ..available for activation AND it's not already triggered,
			// ..ACTIVATE IT!
			if((!pDoor->bInAnimation) && (pDoor->bActive == GE_TRUE) && 
				(!pDoor->bTrigger) && state)
			{
				pDoor->bTrigger= true;			// It's this one, trigger the animation
				if ( pDoor->bRunWhileTrig || pDoor->bRunFromList ||
					pDoor->bRunTimed     || pDoor->bRunToNextEvent)
				{
					if (CCD->ModelManager()->HasMoved(pDoor->Model))
						CCD->ModelManager()->ReStart(pDoor->Model);
					else
						CCD->ModelManager()->Start(pDoor->Model);
				}
				else
					CCD->ModelManager()->Start(pDoor->Model);
				pDoor->bInAnimation = GE_TRUE;
				pDoor->SoundHandle = PlaySound(pDoor->theSound, pDoor->origin, pDoor->bAudioLoops);
			}
				// Now check to see if there's a linked door that we also need to
				// ..trigger.
			if(pDoor->NextToTrigger != NULL)
				TriggerNextDoor(pDoor->NextToTrigger, bTriggerCall);
			return true;									// Hmmph, we hit an automatic door.
		}
	}
	
	return false;							// We hit no known doors
}


int CAutoDoors::PlaySound(geSound_Def *theSound, geVec3d Origin, bool SoundLoop)
{
	if(!theSound)
		return -1;

	Snd Sound;

	memset( &Sound, 0, sizeof( Sound ) );
    geVec3d_Copy( &(Origin), &( Sound.Pos ) );
    Sound.Min=CCD->GetAudibleRadius();
	Sound.Loop=SoundLoop;
	Sound.SoundDef = theSound;
	int index = CCD->EffectManager()->Item_Add(EFF_SND, (void *)&Sound);
	if(SoundLoop)
		return index;
	return -1;
}


//	TriggerNextDoor
//
//	Trigger doors that are linked.  Note that this call IS RECURSIVE
//	..and will let you trigger a whole sequence of doors from one
//	..collision.  Also, if you want fixed double-doors the links CAN
//	..point at each other as a recursive call is NOT made if the door
//	..being triggered here is already triggered.

void CAutoDoors::TriggerNextDoor(geWorld_Model *pModel,	bool bTriggerCall)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	if(m_DoorCount == 0)
		return;									// None here, ignore call.
	
	pSet = geWorld_GetEntitySet(CCD->World(), "Door");
	if(!pSet)
	{
		CCD->ReportError("CAutoDoors: TriggerNextDoor: no doors", false);
		return;
	}
	
	//	Once more we scan the door list.  Does this get old, or what?
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		// Get the door data so we can compare models
		Door *pDoor= (Door*)geEntity_GetUserData(pEntity);
		if(pDoor->Model == pModel)
		{
			if((!pDoor->bShoot) && (bTriggerCall == TRUE))
				return;
			// Models match, we hit this one.  If the entity doesn't activate
			// ..on collide AND this isn't a call from a trigger, don't
			// ..activate the entity.
			if((pDoor->bNoCollide))
				return;								// Fake a no-hit situation
			bool state = true;
			if(!EffectC_IsStringNull(pDoor->TriggerName))
				state = GetTriggerState(pDoor->TriggerName);
			
			// Ok, if the entity isn't already activated AND the entity is
			// ..available for activation AND it's not already triggered,
			// ..ACTIVATE IT!
			if((!pDoor->bInAnimation) && (pDoor->bActive == GE_TRUE) && 
				(!pDoor->bTrigger) && state)
			{
				pDoor->bTrigger= true;			// It's this one, trigger the animation
				//MOD010122 - Changed this section too.  You just can't call Start anymore!
				//            You might have to call ReStart.
				if ( pDoor->bRunWhileTrig || pDoor->bRunFromList ||
					pDoor->bRunTimed     || pDoor->bRunToNextEvent)
				{
					if (CCD->ModelManager()->HasMoved(pDoor->Model))
						CCD->ModelManager()->ReStart(pDoor->Model);
					else
						CCD->ModelManager()->Start(pDoor->Model);
				}
				else
					CCD->ModelManager()->Start(pDoor->Model);
				pDoor->bInAnimation = GE_TRUE;
				pDoor->SoundHandle = PlaySound(pDoor->theSound, pDoor->origin, pDoor->bAudioLoops);
				// Now check to see if there's a linked door that we also need to
				// ..trigger.
				if(pDoor->NextToTrigger != NULL)
					TriggerNextDoor(pDoor->NextToTrigger, bTriggerCall);
			}
			return;									// Hmmph, we hit an automatic door.
		}
	}
	
	return;							// We hit no known doors
}

//	Tick
//
//	Increment animation times for all _animating_ doors that aren't
//	..in a collision state.  This allows time to "stop" for those
//	..doors that are colliding, preventing odd skips and jumps in
//	..the entities animation.

void CAutoDoors::Tick(float dwTicks)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	if(m_DoorCount == 0)
		return;											// No need to waste time here.
	
	//	Ok, check to see if there are automatic doors in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "Door");
	
	if(!pSet) 
		return;									// No doors, how odd...
	
	//	Ok, we have doors somewhere.  Dig through 'em all.
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		Door *pDoor= (Door*)geEntity_GetUserData(pEntity);
		if(!pDoor->Model)
			continue;
		if(!EffectC_IsStringNull(pDoor->TriggerName))
		{
			if(GetTriggerState(pDoor->TriggerName))
			{
				if((!pDoor->bInAnimation) && (pDoor->bActive == GE_TRUE) && 
					(!pDoor->bTrigger) && (pDoor->bNoCollide))
				{
					pDoor->bTrigger= true;			// It's this one, trigger the animation
					if ( pDoor->bRunWhileTrig || pDoor->bRunFromList ||
						pDoor->bRunTimed     || pDoor->bRunToNextEvent)
					{
						if (CCD->ModelManager()->HasMoved(pDoor->Model))
							CCD->ModelManager()->ReStart(pDoor->Model);
						else
							CCD->ModelManager()->Start(pDoor->Model);
					}
					else
						CCD->ModelManager()->Start(pDoor->Model);
					pDoor->bInAnimation = GE_TRUE;
					pDoor->SoundHandle = PlaySound(pDoor->theSound, pDoor->origin, pDoor->bAudioLoops);
					// Now check to see if there's a linked door that we also need to
					// ..trigger.
					if(pDoor->NextToTrigger != NULL)
						TriggerNextDoor(pDoor->NextToTrigger, false);
				}
			}
			else
			{
				if(pDoor->bInAnimation && (pDoor->bActive == GE_TRUE) && 
					pDoor->bTrigger && pDoor->bNoCollide && pDoor->bRunWhileTrig)
				{   //If bRunWhileTrig is true then un-trigger the animation
					pDoor->bTrigger= false;
					CCD->ModelManager()->Stop(pDoor->Model);
					if(pDoor->SoundHandle != -1)
					{
						CCD->EffectManager()->Item_Delete(EFF_SND, pDoor->SoundHandle);
						pDoor->SoundHandle = -1;
					}
				}
			}
		}
		if(pDoor->CallBack==GE_TRUE)
		{
			pDoor->CallBackCount-=1;
			if(pDoor->CallBackCount==0)
				pDoor->CallBack=GE_FALSE;
		}
		if((pDoor->bInAnimation == GE_TRUE) &&
			(CCD->ModelManager()->IsRunning(pDoor->Model) == false))
		{
			// Animation has stopped/not running, handle it.
			CCD->ModelManager()->Stop(pDoor->Model);
			if((pDoor->bOneShot != GE_TRUE) && (pDoor->bActive == GE_TRUE))
			{
				// Ok, not one-shot, reset the door
				pDoor->bInAnimation = GE_FALSE;
				pDoor->bTrigger = false;
			}
			if(pDoor->SoundHandle != -1)
			{
				CCD->EffectManager()->Item_Delete(EFF_SND, pDoor->SoundHandle);
				pDoor->SoundHandle = -1;
			}
		}
	}
	
	return;
}

//	IsADoor
//
//	Return TRUE if the passed-in model is a door, FALSE otherwise

bool CAutoDoors::IsADoor(geWorld_Model *theModel)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	if(m_DoorCount == 0)
		return false;											// No need to waste time here.
	
	//	Ok, check to see if there are automatic doors in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "Door");
	
	if(!pSet) 
		return false;									// No doors, how odd...
	
	//	Ok, we have doors somewhere.  Dig through 'em all.
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		Door *pDoor= (Door*)geEntity_GetUserData(pEntity);
		if(pDoor->Model == theModel)
			return true;							// Model IS a door
	}
	
	return false;							// Nope, it's not a door.
}

//	SaveTo
//
//	Save the current state of every door in the current world
//	..off to an open file.

int CAutoDoors::SaveTo(FILE *SaveFD, bool type)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	//	Ok, check to see if there are automatic doors in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "Door");
	
	if(!pSet) 
		return RGF_SUCCESS;									// No doors, whatever...
	
	//	Ok, we have doors somewhere.  Dig through 'em all.
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		Door *pDoor = (Door*)geEntity_GetUserData(pEntity);
		WRITEDATA(&pDoor->bInAnimation, sizeof(geBoolean), 1, SaveFD);
		WRITEDATA(&pDoor->bTrigger, sizeof(geBoolean), 1, SaveFD);
		WRITEDATA(&pDoor->AnimationTime, sizeof(int), 1, SaveFD);
		WRITEDATA(&pDoor->bInCollision, sizeof(geBoolean), 1, SaveFD);
		WRITEDATA(&pDoor->bActive, sizeof(geBoolean), 1, SaveFD);
		WRITEDATA(&pDoor->LastIncrement, sizeof(int), 1, SaveFD);
		WRITEDATA(&pDoor->tDoor, sizeof(geFloat), 1, SaveFD);
		WRITEDATA(&pDoor->CallBack, sizeof(geBoolean), 1, SaveFD);
		WRITEDATA(&pDoor->CallBackCount, sizeof(int), 1, SaveFD);
	}
	
	return RGF_SUCCESS;
}

//	RestoreFrom
//
//	Restore the state of every door in the current world from an
//	..open file.

int CAutoDoors::RestoreFrom(FILE *RestoreFD, bool type)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	//	Ok, check to see if there are automatic doors in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "Door");
	
	if(!pSet) 
		return RGF_SUCCESS;									// No doors, whatever...
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
    pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		Door *pDoor = (Door*)geEntity_GetUserData(pEntity);
		READDATA(&pDoor->bInAnimation, sizeof(geBoolean), 1, RestoreFD);
		READDATA(&pDoor->bTrigger, sizeof(geBoolean), 1, RestoreFD);
		READDATA(&pDoor->AnimationTime, sizeof(int), 1, RestoreFD);
		READDATA(&pDoor->bInCollision, sizeof(geBoolean), 1, RestoreFD);
		READDATA(&pDoor->bActive, sizeof(geBoolean), 1, RestoreFD);
		READDATA(&pDoor->LastIncrement, sizeof(int), 1, RestoreFD);
		READDATA(&pDoor->tDoor, sizeof(geFloat), 1, RestoreFD);
		READDATA(&pDoor->CallBack, sizeof(geBoolean), 1, RestoreFD);
		READDATA(&pDoor->CallBackCount, sizeof(int), 1, RestoreFD);
		if(pDoor->bInAnimation)
			geWorld_OpenModel(CCD->World(), pDoor->Model, GE_TRUE);
    }
	
	return RGF_SUCCESS;
}

//	******************** CRGF Overrides ********************

//	LocateEntity
//
//	Given a name, locate the desired item in the currently loaded level
//	..and return it's user data.

int CAutoDoors::LocateEntity(char *szName, void **pEntityData)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	//	Ok, check to see if there are doors in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "Door");
	
	if(!pSet) 
		return RGF_NOT_FOUND;									// No doors
	
	//	Ok, we have doors.  Dig through 'em all.
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		Door *pTheEntity = (Door*)geEntity_GetUserData(pEntity);
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

int CAutoDoors::ReSynchronize()
{
	return RGF_SUCCESS;
}
