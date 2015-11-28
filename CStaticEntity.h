/************************************************************************************//**
 * @file CStaticEntity.h
 * @brief Static Entity Handling Class
 *
 * This file contains the class declaration for the Static Entity handling class.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_CSTATICENTITY_H_
#define __RGF_CSTATICENTITY_H_

#include "ExtBox.h"

class CStaticEntity : public CRGFComponent
{
public:
	CStaticEntity();									// Default constructor
	~CStaticEntity();									// Default destructor

	void Render(geXForm3d ViewPoint, DWORD  dwTime);	// Render all static entities
	int HandleCollision(geActor *pActor, geActor *theActor, bool Gravity, bool UseKey);
	void ClearHit();
	void Tick(geFloat dwTicks);
														// Handle time-based action, if any
	int BindToPath(char *szName);						// Bind entity to motion path
	int SaveTo(FILE *SaveFD, bool type);				// Save states to a supplied file
	int RestoreFrom(FILE *RestoreFD, bool type);		// Restore states from a supplied file
	int LocateEntity(char *szName, void **pEntityData);
	int GetEntity(geActor *Actor, void **pEntityData);
	int ReSynchronize();

private:
	int m_StaticEntityCount;							// Count of static entities
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
