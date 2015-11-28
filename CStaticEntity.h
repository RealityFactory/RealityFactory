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

/**
 * @brief CStaticEntity handles StaticEntityProxy entities
 *
 * Static entities are Genesis3D actors that do not	have any animation (this
 * means health packs, weapons, props, things that aren't world models but are
 * not at the level of non-player characters.
 */
class CStaticEntity : public CRGFComponent
{
public:
	CStaticEntity();									///< Default constructor
	~CStaticEntity();									///< Default destructor

	void Render(geXForm3d ViewPoint, DWORD  dwTime);	///< Render all static entities
	int HandleCollision(const geActor *pActor, const geActor *theActor, bool Gravity, bool UseKey);
	void ClearHit();
	void Tick(geFloat dwTicks);
														///< Handle time-based action, if any
	int BindToPath(const char *szName);						///< Bind entity to motion path
	int SaveTo(FILE *SaveFD, bool type);				///< Save states to a supplied file
	int RestoreFrom(FILE *RestoreFD, bool type);		///< Restore states from a supplied file
	int LocateEntity(const char *szName, void **pEntityData);
	int GetEntity(const geActor *Actor, void **pEntityData);
	int ReSynchronize();

private:
	int m_StaticEntityCount;							///< Count of static entities
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
