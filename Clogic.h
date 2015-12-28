/************************************************************************************//**
 * @file Clogic.h
 * @brief Logic Gate class handler
 *
 * This file contains the class declaration for logic gate handling.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_CLOGIC_H_
#define __RGF_CLOGIC_H_

typedef struct LState
{
	LState	*next;
	LState	*prev;
	char	*Name;
	bool	state;

} LState;

/**
 * @brief CLogic handles LogicGate entities
 */
class CLogic : public CRGFComponent
{
public:
	CLogic();
	~CLogic();

	void Tick(geFloat dwTicks);

	int SaveTo(FILE *SaveFD, bool type);

	int RestoreFrom(FILE *RestoreFD, bool type);

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
	void SetState();

	bool GetLTriggerState(const char *Name);

private:
	int		m_EntityCount;		// Count of LogicGate entities
	LState	*Bottom;
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
