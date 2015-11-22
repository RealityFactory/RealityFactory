/*
	CAutoDoors.h:		Automatic door handler

	(c) 1999 Edward A. Averill, III

	This file contains the class declaration for the CAutoDoors
class that encapsulates automatic-door handling in the RGF.
*/

#include "RabidFramework.h"

#ifndef __RGF_CAUTODOORS_H__
#define __RGF_CAUTODOORS_H__


class CAutoDoors : public CRGFComponent
{
public:
  CAutoDoors();				// Constructor
	~CAutoDoors();																// Destructor
  void Render(geXForm3d ViewPoint, DWORD  dwTime);			// Render all doors
  bool HandleCollision(geWorld_Model *pModel,	bool bTriggerCall);
  void TriggerNextDoor(geWorld_Model *pModel,	bool bTriggerCall);
													// Follow door trigger chain
	bool IsADoor(geWorld_Model *theModel);				// Is this model a door?
  void Tick(geFloat dwTicks);					// Increment animation time
  int SaveTo(FILE *SaveFD);				// Save all doors to a file
	int RestoreFrom(FILE *RestoreFD);			// Restore all doors from a file
	int LocateEntity(char *szName, void **pEntityData);
	int ReSynchronize();
private:
	int PlaySound(geSound_Def *theSound, geVec3d Origin, bool SoundLoop);
  int m_DoorCount;					// Count of doors in world
};

#endif
