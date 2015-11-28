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
	// changed RF063
	bool HandleCollision(geWorld_Model *pModel,	bool bTriggerCall, bool UseKey, geActor *theActor);
	void TriggerNextDoor(geWorld_Model *pModel,	bool bTriggerCall);
														///< Follow door trigger chain
	bool IsADoor(geWorld_Model *theModel);				///< Is this model a door?
	void Tick(geFloat dwTicks);							///< Increment animation time
	int SaveTo(FILE *SaveFD, bool type);				///< Save all doors to a file
	int RestoreFrom(FILE *RestoreFD, bool type);		///< Restore all doors from a file
	int LocateEntity(const char *szName, void **pEntityData);
	int ReSynchronize();

private:
	int PlaySound(geSound_Def *theSound, const geVec3d &Origin, bool SoundLoop);
	int m_DoorCount;					///< Count of doors in world
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
