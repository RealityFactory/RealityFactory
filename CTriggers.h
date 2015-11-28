#include "RabidFramework.h"
/************************************************************************************//**
 * @file CTriggers.h
 * @brief Trigger handler
 *
 * This file contains the class declaration for the CTriggers class that
 * encapsulates trigger handling in the RGF.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_CTRIGGERS_H__
#define __RGF_CTRIGGERS_H__

#include <Ram.h>

typedef struct	TState
{
	TState	*next;
	TState	*prev;
	char	*Name;
	bool	state;

} TState;


class CTriggers : public CRGFComponent
{
public:
	CTriggers();				// Constructor
	~CTriggers();				// Destructor

	bool HandleTriggerEvent(char *TName);			// MOD010122 - This line added
	// changed RF063
	int HandleCollision(geWorld_Model *pModel, bool HitType, bool UseKey, geActor *theActor);
	bool IsATrigger(geWorld_Model *theModel);		// Is this model a trigger?
	void Tick(float dwTicks);						// Increment animation time
	int SaveTo(FILE *SaveFD, bool type);			// Save triggers to file
	int RestoreFrom(FILE *RestoreFD, bool type);	// Restore triggers from file
	int LocateEntity(char *szName, void **pEntityData);
	int ReSynchronize();

private:
	// changed QD 12/15/05 - changed 2nd argument from geVec3d to const geVec3d&
	int PlaySound(geSound_Def *theSound, const geVec3d &Origin, bool SoundLoop);
	void SetState();
	bool GetTTriggerState(char *Name);
	void FreeState();

private:
	int m_TriggerCount;			// Count of triggers in world
	TState *Bottom;
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
