/************************************************************************************//**
 * @file CAutoDoors.h
 * @brief Automatic door handler
 *
 * This file contains the class declaration for the CAutoDoors class that
 * encapsulates automatic-door handling in the RGF.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved
 ****************************************************************************************/

#ifndef __RGF_CAUTODOORS_H__
#define __RGF_CAUTODOORS_H__

/**
 * @brief CAutoDoors handles all Door entities
 */
class CAutoDoors : public CRGFComponent
{
public:
	CAutoDoors();										///< Constructor
	~CAutoDoors();										///< Destructor

	void Render(geXForm3d ViewPoint, DWORD  dwTime);	///< Render all doors

	/**
	 * @brief Handle collisions with doors
	 */
	bool HandleCollision(geWorld_Model *pModel,	bool bTriggerCall, bool UseKey, geActor *theActor);

	/**
	 * @brief Follow door trigger chain
	 */
	void TriggerNextDoor(geWorld_Model *pModel,	bool bTriggerCall);

	bool IsADoor(geWorld_Model *theModel) const;		///< Is this model a door?

	void Tick(geFloat dwTicks);							///< Increment animation time

	int SaveTo(FILE *SaveFD, bool type);				///< Save all doors to a file

	int RestoreFrom(FILE *RestoreFD, bool type);		///< Restore all doors from a file

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
	int m_DoorCount;					///< Count of doors in world
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
