/*
	CLevelController.h:		Level Controller

	(c) 2002 David C. Swope

	This file contains the class declaration for the CLevelController
   class that encapsulates level control logic handling in the RGF.
*/

#include "RabidFramework.h"

#ifndef __RGF_CLEVELCONTROLLER_H__
#define __RGF_CLEVELCONTROLLER_H__

#include "Simkin\\skScriptedExecutable.h"

#define DEBUGLINES	8

class ControllerObject : public skScriptedExecutable
{
public:
	ControllerObject(char *fileName);
	~ControllerObject();
	bool getValue (const skString& fieldName, const skString& attribute, skRValue& value);
   bool setValue (const skString& fieldName, const skString& attribute, const skRValue& value);
   bool method (const skString& methodName, skRValueArray& arguments,skRValue& returnValue);

	char	Order[64];
   char  ThinkOrder[64];
	float	ElapseTime;
   float ThinkTime;
	char	szName[64];
	bool	console;
	char	*ConsoleHeader;
	char	*ConsoleError;
	char	*ConsoleDebug[DEBUGLINES];
};

class CLevelController : public CRGFComponent
{
public:
   CLevelController();	                  // Constructor
  ~CLevelController();	                  // Destructor
	void Tick(geFloat dwTicks);			   // Increment animation time
   int SaveTo(FILE *SaveFD);				   // Save LevelController status to a file
	int RestoreFrom(FILE *RestoreFD);	   // Restore LevelController status from a file
private:
   int ConsoleBlock;
};

#endif
