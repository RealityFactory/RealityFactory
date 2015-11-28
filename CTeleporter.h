/****************************************************************************************/
/*																						*/
/*	CTeleporter.h:		Teleporter class handler										*/
/*																						*/
/*	(c) 2001 Ralph Deane																*/
/*	All Rights Reserved																	*/
/*																						*/
/*	This file contains the class declaration for teleporter handling.					*/
/*																						*/
/****************************************************************************************/


#include "RabidFramework.h"

#ifndef __RGF_CTELEPORTER_H_
#define __RGF_CTELEPORTER_H_

class CTeleporter : public CRGFComponent
{
public:
	CTeleporter();
	~CTeleporter();

	bool HandleCollision(geWorld_Model *pModel, geActor *theActor);
	void Tick(geFloat dwTicks);						// Do any animation
	int SaveTo(FILE *SaveFD, bool type);			// Save teleporters & targets to file
	int RestoreFrom(FILE *RestoreFD, bool type);	// Restore teleporters & targets from file
	int BindToPath(char *szName);					// Bind TeleportTarget to motion path
	int LocateEntity(char *szName, void **pEntityData);
	int ReSynchronize();
	void DoFade(void);

private:
	int m_TeleporterCount;							// Count of teleporters
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
