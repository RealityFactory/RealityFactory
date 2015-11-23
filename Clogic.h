/*
	CLogic.h:		Logic Gate class handler

	(c) 2001 Ralph Deane
	All Rights Reserved

	This file contains the class declaration for logic gate
handling.
*/

#ifndef __RGF_CLOGIC_H_
#define __RGF_CLOGIC_H_

#include <Ram.h>

typedef struct	LState
{
  LState    *next;
  LState    *prev;
  char     *Name;
  bool		state;
} LState;

class CLogic : public CRGFComponent
{
public:
  CLogic();
  ~CLogic();
  void Tick(float dwTicks);
  int SaveTo(FILE *SaveFD, bool type);
  int RestoreFrom(FILE *RestoreFD, bool type);
  int LocateEntity(char *szName, void **pEntityData);
  int ReSynchronize();
private:
	void SetState();
	bool GetLTriggerState(char *Name);
  int Count;		// Count of 3D audio sources
  LState *Bottom;
};

#endif


