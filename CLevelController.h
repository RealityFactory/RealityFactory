/************************************************************************************//**
 * @file CLevelController.h
 * @brief Level Controller
 *
 * This file contains the class declaration for the CLevelController class that
 * encapsulates level control logic handling in the RGF.
 * @author David C. Swope
 *//*
 * Copyright (c) 2002 David C. Swope; All rights reserved.
 ****************************************************************************************/

#include "RabidFramework.h"

#ifndef __RGF_CLEVELCONTROLLER_H__
#define __RGF_CLEVELCONTROLLER_H__

#include "Simkin\\skScriptedExecutable.h"

#define DEBUGLINES	8

/**
 * @brief ControllerObject class for LevelController scripts
 */
class ControllerObject : public skScriptedExecutable
{
	friend class CLevelController;
public:
	ControllerObject(char *fileName);
	~ControllerObject();

	bool getValue(const skString &fieldName, const skString &attribute, skRValue &value);
	bool setValue(const skString &fieldName, const skString &attribute, const skRValue &value);
	bool method (const skString &methodName, skRValueArray &arguments, skRValue &returnValue, skExecutableContext &ctxt);

private:
	char	Order[64];
	char	ThinkOrder[64];
	float	ElapseTime;
	float	ThinkTime;
	char	szName[64];
	bool	console;
	char	*ConsoleHeader;
	char	*ConsoleError;
	char	*ConsoleDebug[DEBUGLINES];
};

/**
 * @brief CLevelController handles LEvelController entities
 */
class CLevelController : public CRGFComponent
{
public:
	CLevelController();					///< Constructor
	~CLevelController();				///< Destructor

	void Tick(geFloat dwTicks);			///< Increment animation time
	int SaveTo(FILE *SaveFD);			///< Save LevelController status to a file
	int RestoreFrom(FILE *RestoreFD);	///< Restore LevelController status from a file
private:
	int ConsoleBlock;
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
