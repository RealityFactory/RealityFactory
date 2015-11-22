/*
	CTeleporter.h:		Teleporter class handler

	(c) 1999 Edward A. Averill, III
	All Rights Reserved

	This file contains the class declaration for teleporter handling.
*/

#include "RabidFramework.h"

#ifndef __RGF_CTELEPORTER_H_
#define __RGF_CTELEPORTER_H_

class CTeleporter : public CRGFComponent
{
public:
  CTeleporter();
  ~CTeleporter();
  bool HandleCollision(geWorld_Model *pModel, geActor *theActor);
	void Tick(geFloat dwTicks);			// Do any animation
	int SaveTo(FILE *SaveFD);
															// Save teleporters & targets to file
	int RestoreFrom(FILE *RestoreFD);
															// Restore teleporters & targets from file
  int BindToPath(char *szName);		// Bind TeleportTarget to motion path
	int LocateEntity(char *szName, void **pEntityData);
	int ReSynchronize();
private:
  int m_TeleporterCount;			// Count of teleporters
};

#endif

// --------------------------------------------------------------------------
