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

/**
 * actors riding on a moving platform
 */
struct ActorsOnPlatforms
{
	geActor *theActor;			///< Actor that's on the platform
	geWorld_Model *thePlatform;	///< Platform model actor is on!
};

/**
 * @brief CMovingPlatforms handles MovingPlatform entities
 */
class CMovingPlatforms : public CRGFComponent
{
public:
	CMovingPlatforms();								///< Constructor
	~CMovingPlatforms();							///< Destructor

	bool HandleCollision(const geWorld_Model *pModel, bool bTriggerCall, bool UseKey, const geActor *theActor);

	void TriggerNextPlatform(const geWorld_Model *pModel, bool bTriggerCall);

	bool IsAPlatform(const geWorld_Model *theModel);	///< Is this model a platform?

	void Tick(geFloat dwTicks);						///< Increment animation time

	int SaveTo(FILE *SaveFD, bool type);			///< Save all platforms to a file

	int RestoreFrom(FILE *RestoreFD, bool type);	///< Restore all platforms from a file

	/**
	 * @brief Given a name, locate the desired entity in the currently loaded
	 * level and return its user data.
	 */
	int LocateEntity(const char *szName, void **pEntityData);

	/**
	 * @brief Correct internal timing to match current time, to make up for time
	 * lost when outside the game loop (typically in "menu mode").
	 */
	int ReSynchronize();

private:
	int PlaySound(geSound_Def *theSound, const geVec3d &Origin, bool SoundLoop);

private:
	int m_EntityCount;					///< Count of moving platforms in world
	ActorsOnPlatforms AP[512];			///< No more than 512 actors on platforms at once
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
