/****************************************************************************************/
/*																						*/
/*	CLogic.cpp:		Logic Gate class implementation										*/
/*																						*/
/*	(c) 2001 Ralph Deane																*/
/*	All Rights Reserved																	*/
/*																						*/
/*	This file contains the class implementation for Logic Gate							*/
/*	handling.																			*/
/*																						*/
/****************************************************************************************/

//	Include the One True Header
#include "RabidFramework.h"

/* ------------------------------------------------------------------------------------ */
//	Constructor
//
//	Load up all logic gates and set the
//	..entities to default values.
/* ------------------------------------------------------------------------------------ */
CLogic::CLogic()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

  	Count = 0;

	// Ok, check to see if there are logic gates in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "LogicGate");

	if(!pSet)
		return;									// No 3D audio sources

	// Ok, we have logic gates somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	    pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
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

		pSource->active		= GE_FALSE;
		pSource->bState		= GE_FALSE;
		pSource->OldState	= GE_FALSE;
		pSource->inDelay	= GE_FALSE;
		pSource->time		= 0.0f;
	}

	return;
}

/* ------------------------------------------------------------------------------------ */
//	Destructor
//
//	Clean up.
/* ------------------------------------------------------------------------------------ */
CLogic::~CLogic()
{
}

/* ------------------------------------------------------------------------------------ */
//	Tick
/* ------------------------------------------------------------------------------------ */
void CLogic::Tick(float dwTicks)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	geBoolean state, state2;

	SetState();

	if(Count == 0)
		return;						// Don't waste CPU cycles

	// Ok, check to see if there are logic gates in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "LogicGate");

	if(!pSet)
		return;									// No sources

	// Ok, we have logic gates somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	    pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
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
				pSource->time += dwTicks;

				if(pSource->time>=(pSource->Delay*1000.0f))
				{
					pSource->bState = pSource->OldState;
					pSource->inDelay = GE_FALSE;
				}
			}
			else
			{
				if(pSource->OldState != state)
				{
					pSource->OldState = state;
					pSource->time = 0.0f;
					pSource->inDelay = GE_TRUE;
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
			if(pSource->OldState == GE_FALSE && state)
			{
				if(pSource->bState)
					pSource->bState = GE_FALSE;
				else
					pSource->bState = GE_TRUE;
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
				pSource->time += dwTicks;

				if(pSource->time>=(pSource->Delay*1000.0f))
				{
					pSource->bState = GE_FALSE;
					pSource->inDelay = GE_FALSE;
				}
			}
			else
			{
				if(state)
				{
					pSource->bState = state;
					pSource->time = 0.0f;
					pSource->inDelay = GE_TRUE;
				}
			}
			break;
		case 8: // has attribute
			pSource->bState = GE_FALSE;
			if(theInv->Has(pSource->Trigger1Name))
			{
// changed RF063
				if(theInv->Value(pSource->Trigger1Name)>pSource->Amount)
					pSource->bState = GE_TRUE;
			}
			break;
		case 9: // Stay on
			state = GetLTriggerState(pSource->Trigger1Name);
			if(pSource->bState || state)
				pSource->bState = GE_TRUE;
			break;
// changed RF063
		case 10: // Use attribute
			pSource->bState = GE_FALSE;
// changed RF064
			if(CCD->Player()->GetUseAttribute(pSource->Trigger1Name))
			{
				pSource->bState = GE_TRUE;
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
					pSource->OldState = GE_TRUE;
				}
			}
			else
				pSource->OldState = GE_FALSE;
			break;
// changed RF064
		case 12: // Contents
			pSource->bState = GE_FALSE;
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
							pSource->bState = GE_TRUE;
						break;
					case kLavaZone:
						if(!stricmp(pSource->Trigger1Name, "Lava"))
							pSource->bState = GE_TRUE;
						break;
					case kToxicGasZone:
						if(!stricmp(pSource->Trigger1Name, "ToxicGas"))
							pSource->bState = GE_TRUE;
						break;
					case kNoGravityZone:
						if(!stricmp(pSource->Trigger1Name, "ZeroG"))
							pSource->bState = GE_TRUE;
						break;
					case kFrozenZone:
						if(!stricmp(pSource->Trigger1Name, "Frozen"))
							pSource->bState = GE_TRUE;
						break;
					case kSludgeZone:
						if(!stricmp(pSource->Trigger1Name, "Sludge"))
							pSource->bState = GE_TRUE;
						break;
					case kSlowMotionZone:
						if(!stricmp(pSource->Trigger1Name, "SlowMotion"))
							pSource->bState = GE_TRUE;
						break;
					case kFastMotionZone:
						if(!stricmp(pSource->Trigger1Name, "FastMotion"))
							pSource->bState = GE_TRUE;
						break;
					case kClimbLaddersZone:
						if(!stricmp(pSource->Trigger1Name, "Ladder"))
							pSource->bState = GE_TRUE;
						break;
					case kImpassibleZone:
						if(!stricmp(pSource->Trigger1Name, "Impenetrable"))
							pSource->bState = GE_TRUE;
						break;
					case kUnclimbableZone:
						if(!stricmp(pSource->Trigger1Name, "Unclimbable"))
							pSource->bState = GE_TRUE;
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
		case 14: // Difficulty level
			pSource->bState = GE_FALSE;
			if(!stricmp(pSource->Trigger1Name, "Easy") && (CCD->GetDifficultLevel() == 1))
				pSource->bState = GE_TRUE;
			else if(!stricmp(pSource->Trigger1Name, "Normal") && (CCD->GetDifficultLevel() == 2))
				pSource->bState = GE_TRUE;
			else if(!stricmp(pSource->Trigger1Name, "Hard") && (CCD->GetDifficultLevel() == 3))
				pSource->bState = GE_TRUE;
			break;
		case 15: // active weapon
			pSource->bState = GE_FALSE;
			if(!stricmp(pSource->Trigger1Name, "All"))
			{
				if(!(CCD->Weapons()->GetCurrent() == -1 || CCD->Weapons()->GetCurrent() == 11))
					pSource->bState = GE_TRUE;
			}
			else
			{
				if(CCD->Weapons()->GetCurrent() == atoi(pSource->Trigger1Name))
					pSource->bState = GE_TRUE;
			}
			break;
		case 16: // Character name
			pSource->bState = GE_FALSE;
			if(CCD->MenuManager()->GetUseSelect())
			{
				if(!stricmp(pSource->Trigger1Name, CCD->MenuManager()->GetCurrentName()))
					pSource->bState = GE_TRUE;
			}
			break;
// end change RF064
		}
	}

	LState *pool, *temp;
	pool = Bottom;

	while(pool != NULL)
	{
		temp = pool->next;
		free(pool->Name);
		geRam_Free(pool);
		pool = temp;
	}

	return;
}

/* ------------------------------------------------------------------------------------ */
//	SaveTo
//
//	Save the current state of every  logic gate in the current world
//	..off to an open file.
/* ------------------------------------------------------------------------------------ */
int CLogic::SaveTo(FILE *SaveFD, bool type)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	// Ok, check to see if there are  triggers in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "LogicGate");

	if(!pSet)
		return RGF_SUCCESS;									// No gates, whatever...

	// Ok, we have logic gates somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	    pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		LogicGate *pSource = (LogicGate*)geEntity_GetUserData(pEntity);

		WRITEDATA(&pSource->active,		sizeof(geBoolean),	1, SaveFD);
		WRITEDATA(&pSource->bState,		sizeof(geBoolean),	1, SaveFD);
		WRITEDATA(&pSource->OldState,	sizeof(geBoolean),	1, SaveFD);
		WRITEDATA(&pSource->inDelay,	sizeof(geBoolean),	1, SaveFD);
		WRITEDATA(&pSource->time,		sizeof(float),		1, SaveFD);
	}

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	RestoreFrom
//
//	Restore the state of every  logic gate in the current world from an	open file.
/* ------------------------------------------------------------------------------------ */
int CLogic::RestoreFrom(FILE *RestoreFD, bool type)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	// Ok, check to see if there are  triggers in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "LogicGate");

	if(!pSet)
		return RGF_SUCCESS;					// No gates, whatever...

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		LogicGate *pSource = (LogicGate*)geEntity_GetUserData(pEntity);

		READDATA(&pSource->active,		sizeof(geBoolean),	1, RestoreFD);
		READDATA(&pSource->bState,		sizeof(geBoolean),	1, RestoreFD);
		READDATA(&pSource->OldState,	sizeof(geBoolean),	1, RestoreFD);
		READDATA(&pSource->inDelay,		sizeof(geBoolean),	1, RestoreFD);
		READDATA(&pSource->time,		sizeof(float),		1, RestoreFD);
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
int CLogic::LocateEntity(char *szName, void **pEntityData)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	// Ok, check to see if there are logic gates in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "LogicGate");

	if(!pSet)
		return RGF_NOT_FOUND;			// No logic gates

	// Ok, we have logic gates somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		LogicGate *pSource = (LogicGate*)geEntity_GetUserData(pEntity);

		if(!strcmp(pSource->szEntityName, szName))
		{
			*pEntityData = (void *)pSource;
			return RGF_SUCCESS;
		}
	}

	return RGF_NOT_FOUND;				// Sorry, no such entity here
}

/* ------------------------------------------------------------------------------------ */
//	ReSynchronize
//
//	Correct internal timing to match current time, to make up for time lost
//	..when outside the game loop (typically in "menu mode").
/* ------------------------------------------------------------------------------------ */
int CLogic::ReSynchronize()
{
	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	SetState
/* ------------------------------------------------------------------------------------ */
void CLogic::SetState()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	LState *pool;

	Bottom = (LState *)NULL;

	if(Count == 0)
		return;						// Don't waste CPU cycles

	// Ok, check to see if there are logic gates in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "LogicGate");

	if(!pSet)
		return;									// No 3D audio sources

	// Ok, we have logic gates somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
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

		pool->Name = strdup(pSource->szEntityName);
		pool->state = pSource->bState;
	}

	return;
}

/* ------------------------------------------------------------------------------------ */
//	GetLTriggerState
/* ------------------------------------------------------------------------------------ */
bool CLogic::GetLTriggerState(char *Name)
{
	LState *pool;

	char *EntityType = CCD->EntityRegistry()->GetEntityType(Name);

	if(EntityType)
	{
		if(!stricmp(EntityType, "LogicGate"))
		{
			pool=Bottom;

			while(pool != NULL)
			{
				if(!stricmp(Name, pool->Name))
					return pool->state;

				pool = pool->next;
			}

			char szError[256];
			sprintf(szError, "*WARNING* File %s - Line %d: Invalid Trigger Name '%s'\n",
					__FILE__, __LINE__, Name);
			CCD->ReportError(szError, false);
			return false;
		}
	}

	return GetTriggerState(Name);
}

/* ----------------------------------- END OF FILE ------------------------------------ */
