/************************************************************************************//**
 * @file CTeleporter.h
 * @brief Teleporter class handler
 *
 * This file contains the class declaration for teleporter handling.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_CTELEPORTER_H_
#define __RGF_CTELEPORTER_H_

/**
 * @brief CTeleporter handles Teleporter and TeleportTarget entities
 */
class CTeleporter : public CRGFComponent
{
public:
	CTeleporter();
	~CTeleporter();

	bool HandleCollision(const geWorld_Model *pModel, geActor *theActor); ///< Handle collision with teleporters
	void Tick(geFloat dwTicks);						///< Update teleporters & target, do teleporter effect
	int SaveTo(FILE *SaveFD, bool type);			///< Save teleporters & targets to file
	int RestoreFrom(FILE *RestoreFD, bool type);	///< Restore teleporters & targets from file
	int BindToPath(const char *szName);				///< Bind TeleportTarget to motion path
	int LocateEntity(const char *szName, void **pEntityData);
	int ReSynchronize();
	void DoFade(void);

private:
	int m_TeleporterCount;							///< Count of teleporters
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
