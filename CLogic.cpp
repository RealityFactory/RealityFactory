/*
	CLogic.cpp:		Logic Gate class implementation

	(c) 2001 Ralph Deane
	All Rights Reserved

	This file contains the class implementation for Logic Gate
handling.
*/

//	Include the One True Header

#include "RabidFramework.h"

//	Constructor
//
//	Load up all logic gates and set the
//	..entities to default values.

CLogic::CLogic()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

  	Count = 0;

//	Ok, check to see if there are logic gates in this world

	pSet = geWorld_GetEntitySet(CCD->World(), "LogicGate");

	if(!pSet)
		return;									// No 3D audio sources

//	Ok, we have logic gates somewhere.  Dig through 'em all.

	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	    pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		LogicGate *pSource = (LogicGate*)geEntity_GetUserData(pEntity);
		if(EffectC_IsStringNull(pSource->szEntityName))
		{
			char szName[128];
			geEntity_GetName(pEntity, szName, 128);
			pSource->szEntityName = szName;
		}
		Count++;								// Kick source count
		// Ok, put this entity into the Global Entity Registry
		CCD->EntityRegistry()->AddEntity(pSource->szEntityName, "LogicGate");
		// Reset all the data for each logic gate

		pSource->active=false;
		pSource->bState=false;
		pSource->OldState=false;
		pSource->inDelay=false;
		pSource->time=0.0f;

	  }

  return;
}


//	Destructor
//
//	Clean up.

CLogic::~CLogic()
{
}


//	Tick
//

void CLogic::Tick(float dwTicks)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	geBoolean state, state2;

	SetState();

	if(Count == 0)
	  return;						// Don't waste CPU cycles

//	Ok, check to see if there are logic gates in this world

	pSet = geWorld_GetEntitySet(CCD->World(), "LogicGate");

	if(!pSet)
		return;									// No sources

//	Ok, we have logic gates somewhere.  Dig through 'em all.

	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	    pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		LogicGate *pSource = (LogicGate*)geEntity_GetUserData(pEntity);
		CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
		switch(pSource->Type)
		{
			case 0: // AND
				state = GetLTriggerState(pSource->Trigger1Name);
				state2 = GetLTriggerState(pSource->Trigger2Name);
				pSource->bState = (state && state2);
				break;
			case 1: // DELAY
				state = GetLTriggerState(pSource->Trigger1Name);
				if(pSource->inDelay)
				{
					pSource->time+=dwTicks;
					if(pSource->time>=(pSource->Delay*1000.0f))
					{
						pSource->bState = pSource->OldState;
						pSource->inDelay = false;
					}
				}
				else
				{
					if(pSource->OldState!=state)
					{
						pSource->OldState = state;
						pSource->time = 0.0f;
						pSource->inDelay = true;
					}
				}
				break;
			case 2: // NOT
				state = GetLTriggerState(pSource->Trigger1Name);
				pSource->bState = (!state);
				break;
			case 3: // OR
				state = GetLTriggerState(pSource->Trigger1Name);
				state2 = GetLTriggerState(pSource->Trigger2Name);
				pSource->bState = (state || state2);
				break;
			case 4: // XOR
				state = GetLTriggerState(pSource->Trigger1Name);
				state2 = GetLTriggerState(pSource->Trigger2Name);
				pSource->bState = (state ^ state2);
				break;
			case 5: // Flip-Flop
				state = GetLTriggerState(pSource->Trigger1Name);
				if(pSource->OldState==false && state)
				{
					if(pSource->bState)
						pSource->bState=false;
					else
						pSource->bState=true;
				}
				pSource->OldState = state;
				break;
			case 6: // CALLBACK
				pSource->bState = GetCallBackState(pSource->Trigger1Name);
				break;
			case 7: // Hold
				state = GetLTriggerState(pSource->Trigger1Name);
				if(pSource->inDelay)
				{
					pSource->time+=dwTicks;
					if(pSource->time>=(pSource->Delay*1000.0f))
					{
						pSource->bState = false;
						pSource->inDelay = false;
					}
				}
				else
				{
					if(state)
					{
						pSource->bState = state;
						pSource->time = 0.0f;
						pSource->inDelay = true;
					}
				}
				break;
			case 8: // has attribute
				pSource->bState = false;
				if(theInv->Has(pSource->Trigger1Name))
				{
// changed RF063
					if(theInv->Value(pSource->Trigger1Name)>pSource->Amount)
						pSource->bState = true;
				}
				break;
			case 9: // Stay on
				state = GetLTriggerState(pSource->Trigger1Name);
				if(pSource->bState || state)
					pSource->bState = true;
				break;
// changed RF063
			case 10: // Use attribute
				pSource->bState = false;
// changed RF064
				if(CCD->Player()->GetUseAttribute(pSource->Trigger1Name))
				{
					pSource->bState = true;
				}
// end change RF064
				break;
			case 11: // Clear Use Attribute
				state = GetLTriggerState(pSource->Trigger1Name);
				if(state)
				{
					if(!pSource->OldState)
					{
// changed RF064
						if(CCD->Player()->GetUseAttribute(pSource->Trigger2Name))
						{
							CCD->HUD()->ActivateElement(pSource->Trigger2Name, false);
							CCD->Player()->DelUseAttribute(pSource->Trigger2Name);
						}
// end change RF064
						pSource->OldState = true;
					}
				}
				else
					pSource->OldState = false;
				break;
// changed RF064
			case 12: // Contents 
				pSource->bState = false;
				if(!EffectC_IsStringNull(pSource->Trigger1Name))
				{
					geActor	*Actor = GetEntityActor(pSource->Trigger2Name);
					if(Actor)
					{
						int Zone;
						CCD->ActorManager()->GetActorZone(Actor, &Zone);
						Zone ^= 1;
						switch(Zone)
						{
						case kWaterZone:
							if(!stricmp(pSource->Trigger1Name, "Water"))
								pSource->bState = true;
							break;
						case kLavaZone:
							if(!stricmp(pSource->Trigger1Name, "Lava"))
								pSource->bState = true;
							break;
						case kToxicGasZone:
							if(!stricmp(pSource->Trigger1Name, "ToxicGas"))
								pSource->bState = true;
							break;
						case kNoGravityZone:
							if(!stricmp(pSource->Trigger1Name, "ZeroG"))
								pSource->bState = true;
							break;
						case kFrozenZone:
							if(!stricmp(pSource->Trigger1Name, "Frozen"))
								pSource->bState = true;
							break;
						case kSludgeZone:
							if(!stricmp(pSource->Trigger1Name, "Sludge"))
								pSource->bState = true;
							break;
						case kSlowMotionZone:
							if(!stricmp(pSource->Trigger1Name, "SlowMotion"))
								pSource->bState = true;
							break;
						case kFastMotionZone:
							if(!stricmp(pSource->Trigger1Name, "FastMotion"))
								pSource->bState = true;
							break;
						case kClimbLaddersZone:
							if(!stricmp(pSource->Trigger1Name, "Ladder"))
								pSource->bState = true;
							break;
						case kImpassibleZone:
							if(!stricmp(pSource->Trigger1Name, "Impenetrable"))
								pSource->bState = true;
							break;
						case kUnclimbableZone:
							if(!stricmp(pSource->Trigger1Name, "Unclimbable"))
								pSource->bState = true;
							break;
						} 
					}
				}
				break;
			case 13: // Stop oxygen restore
				state = GetLTriggerState(pSource->Trigger1Name);
				if(state)
					CCD->Player()->SetRestoreOxy(false);
				break;
// end change RF064
		}

	}

	LState *pool, *temp;
	pool = Bottom;
	while	(pool!= NULL)
	{
		temp = pool->next;
		free(pool->Name);
		geRam_Free(pool);
		pool = temp;
	}
	return;
}


//	SaveTo
//
//	Save the current state of every  logic gate in the current world
//	..off to an open file.

int CLogic::SaveTo(FILE *SaveFD, bool type)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

//	Ok, check to see if there are  triggers in this world

	pSet = geWorld_GetEntitySet(CCD->World(), "LogicGate");

	if(!pSet)
		return RGF_SUCCESS;									// No gates, whatever...

//	Ok, we have logic gates somewhere.  Dig through 'em all.

	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	    pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity))
		{
		LogicGate *pSource = (LogicGate*)geEntity_GetUserData(pEntity);
		WRITEDATA(&pSource->active, sizeof(geBoolean), 1, SaveFD);
		WRITEDATA(&pSource->bState, sizeof(geBoolean), 1, SaveFD);
		WRITEDATA(&pSource->OldState, sizeof(geBoolean), 1, SaveFD);
		WRITEDATA(&pSource->inDelay, sizeof(geBoolean), 1, SaveFD);
		WRITEDATA(&pSource->time, sizeof(float), 1, SaveFD);
	  }

  return RGF_SUCCESS;
}

//	RestoreFrom
//
//	Restore the state of every  logic gate in the current world from an
//	..open file.

int CLogic::RestoreFrom(FILE *RestoreFD, bool type)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

//	Ok, check to see if there are  triggers in this world

	pSet = geWorld_GetEntitySet(CCD->World(), "LogicGate");

	if(!pSet)
		return RGF_SUCCESS;									// No gates, whatever...

  for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
    pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity))
	  {
		LogicGate *pSource = (LogicGate*)geEntity_GetUserData(pEntity);
		READDATA(&pSource->active, sizeof(geBoolean), 1, RestoreFD);
		READDATA(&pSource->bState, sizeof(geBoolean), 1, RestoreFD);
		READDATA(&pSource->OldState, sizeof(geBoolean), 1, RestoreFD);
		READDATA(&pSource->inDelay, sizeof(geBoolean), 1, RestoreFD);
		READDATA(&pSource->time, sizeof(float), 1, RestoreFD);
    }

  return RGF_SUCCESS;
}

//	******************** CRGF Overrides ********************

//	LocateEntity
//
//	Given a name, locate the desired item in the currently loaded level
//	..and return it's user data.

int CLogic::LocateEntity(char *szName, void **pEntityData)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

//	Ok, check to see if there are logic gates in this world

	pSet = geWorld_GetEntitySet(CCD->World(), "LogicGate");

	if(!pSet)
		return RGF_NOT_FOUND;									// No 3D audio sources

//	Ok, we have logic gates somewhere.  Dig through 'em all.

	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	    pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity))
		{
		LogicGate *pSource = (LogicGate*)geEntity_GetUserData(pEntity);

		if(strcmp(pSource->szEntityName, szName) == 0)
		  {
			*pEntityData = (void *)pSource;
			return RGF_SUCCESS;
			}
		}

  return RGF_NOT_FOUND;								// Sorry, no such entity here
}

//	ReSynchronize
//
//	Correct internal timing to match current time, to make up for time lost
//	..when outside the game loop (typically in "menu mode").

int CLogic::ReSynchronize()
{
	return RGF_SUCCESS;
}

void CLogic::SetState()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	LState *pool;

	Bottom = (LState *)NULL;

	if(Count == 0)
	  return;						// Don't waste CPU cycles

//	Ok, check to see if there are logic gates in this world

	pSet = geWorld_GetEntitySet(CCD->World(), "LogicGate");

	if(!pSet)
		return;									// No 3D audio sources

//	Ok, we have logic gates somewhere.  Dig through 'em all.

	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	    pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		LogicGate *pSource = (LogicGate*)geEntity_GetUserData(pEntity);
		if(EffectC_IsStringNull(pSource->szEntityName))
		{
			char szName[128];
			geEntity_GetName(pEntity, szName, 128);
			pSource->szEntityName = szName;
		}
		pool = GE_RAM_ALLOCATE_STRUCT(LState);
		memset(pool, 0xcc, sizeof(LState));
		pool->next = Bottom;
		Bottom = pool;
		if(pool->next)
			pool->next->prev = pool;
		pool->Name=strdup(pSource->szEntityName);
		pool->state=pSource->bState;
	}

  return;
}

bool CLogic::GetLTriggerState(char *Name)
{
	LState *pool;

	char *EntityType = CCD->EntityRegistry()->GetEntityType(Name);
	if(EntityType)
	{
		if(!stricmp(EntityType, "LogicGate"))
		{
			pool=Bottom;
			while ( pool != NULL )
			{
				if(!stricmp(Name, pool->Name))
					return pool->state;
				pool = pool->next;
			}
			char szError[256];
			sprintf(szError,"Invalid Trigger Name '%s'\n", Name);
			CCD->ReportError(szError, false);
			return false;
		}
	}
	return GetTriggerState(Name);
}
