/*
	CAIController.cpp:		AI controller class

	(c) 1999 Edward A. Averill, III

	This file contains the class implementation for the RGF
Artificial Intelligence (AI) controller.
*/

#include "RabidFramework.h"				// The One True Include File

//	Constructor
//
//	Create an AI controller based on the desired AI name.

CAIController::CAIController(char *szName)
{
  return;
}

//	Destructor
//
//	Clean up AI state

CAIController::~CAIController()
{
  return;
}

//	Tick
//
//	Handle the passage of time.

int CAIController::Tick(geFloat TicksPassed)
{
  return RGF_UNIMPLEMENTED;
}

//	SaveTo
//
//	Save the AI state to a file

int CAIController::SaveTo(FILE *fd)
{
  return RGF_UNIMPLEMENTED;
}

//	RestoreFrom
//
//	Restore the AI state from a file

int CAIController::RestoreFrom(FILE *fd)
{
  return RGF_UNIMPLEMENTED;
}

