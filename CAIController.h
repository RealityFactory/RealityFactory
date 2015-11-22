/*
	CAIController.h:		AI controller class

	(c) 1999 Edward A. Averill, III

	This file contains the class declaration for the RGF
Artificial Intelligence (AI) controller.
*/

#ifndef __RGF_CAICONTROLLER_H__
#define __RGF_CAICONTROLLER_H__

class CAIController : public CRGFComponent
{
public:
  CAIController(char *szName);	// Constructor
  ~CAIController();				// Destructor
  int Tick(geFloat TicksPassed);	// Handle the passage of time
  int SaveTo(FILE *fd);			// Save AI state to a file
  int RestoreFrom(FILE *fd);	// Restore AI state from a file
private:
  geBoolean Active;					// Entity is active
  int CurrentState;					// AIs current state index
  int PreviousState;				// AIs previous state index
  geBoolean ReturnToPreviousState;	// TRUE if AI should return to previous state
  char *CurrentAnimationName;		// Name of current animation
};

#endif
