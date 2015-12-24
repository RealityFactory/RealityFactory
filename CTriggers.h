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

/**
 * @brief Trigger state list element
 */
typedef struct	TState
{
	TState	*next;
	TState	*prev;
	char	*Name;
	bool	state;

} TState;

/**
 * @brief CTriggers handles Trigger entities
 */
class CTriggers : public CRGFComponent
{
public:
	CTriggers();				///< Constructor
	~CTriggers();				///< Destructor

	bool HandleTriggerEvent(const char *TName);

	int HandleCollision(const geWorld_Model *pModel, bool HitType, bool UseKey, const geActor *theActor);

	bool IsATrigger(const geWorld_Model *theModel);	///< Is this model a trigger?

	void Tick(geFloat dwTicks);						///< Increment animation time

	int SaveTo(FILE *SaveFD, bool type);			///< Save triggers to file

	int RestoreFrom(FILE *RestoreFD, bool type);	///< Restore triggers from file

	/**
	 * @brief Given a name, locate the desired entity in the currently loaded
	 * level and return its user data.
	 */
	int LocateEntity(const char *szName, void **pEntityData);

	/**
	 * @brief Correct internal timing to match current time, to make up for time
	 * lost when outside the game loop (typically in "menu mode").
	 */
	int ReSynchronize();

private:
	int PlaySound(geSound_Def *theSound, const geVec3d &Origin, bool SoundLoop);

	void SetState();

	bool GetTTriggerState(const char *Name);

	void FreeState();

private:
	int m_EntityCount;			// Count of triggers in world
	TState *Bottom;
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
