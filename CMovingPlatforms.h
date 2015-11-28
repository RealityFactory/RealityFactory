#include "RabidFramework.h"
/************************************************************************************//**
 * @file CMovingPlatforms.h
 * @brief Moving Platform Handler
 *
 * This file contains the class declaration for the CMovingPlatforms class that
 * encapsulates moving platform handling in the RGF.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_CMOVINGPLATFORMS_H__
#define __RGF_CMOVINGPLATFORMS_H__

struct ActorsOnPlatforms
{
	geActor *theActor;			// Actor that's on the platform
	geWorld_Model *thePlatform;	// Platform model actor is on!
};


class CMovingPlatforms : public CRGFComponent
{
public:
	CMovingPlatforms();								// Constructor
	~CMovingPlatforms();							// Destructor

	// changed RF063
	bool HandleCollision(geWorld_Model *pModel,	bool bTriggerCall, bool UseKey, geActor *theActor);
	void TriggerNextPlatform(geWorld_Model *pModel, bool bTriggerCall);
	bool IsAPlatform(geWorld_Model *theModel);		// Is this model a platform?
	void Tick(geFloat dwTicks);						// Increment animation time

	int SaveTo(FILE *SaveFD, bool type);			// Save all platforms to a file
	int RestoreFrom(FILE *RestoreFD, bool type);	// Restore all platforms from a file

	int LocateEntity(char *szName, void **pEntityData);
	int ReSynchronize();

private:
	// changed QD 12/15/05 - changed 2nd argument from geVec3d to const geVec3d&
	int PlaySound(geSound_Def *theSound, const geVec3d &Origin, bool SoundLoop);
	int m_PlatformCount;					// Count of moving platforms in world
	ActorsOnPlatforms AP[512];				// No more than 512 actors on platforms at once
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
