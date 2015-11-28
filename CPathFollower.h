/************************************************************************************//**
 * @file CPathFollower.h
 * @brief Entity motion binding component
 *
 * This file contains the declaration for the CPathFollower component.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_CPATHFOLLOWER_H_
#define __RGF_CPATHFOLLOWER_H_

/**
 * @brief CPathFollower handles PathFollower entities
 *
 * This component causes moveable entities to travel at a specified speed
 * between points on a path. Not all entities are moveable!
 */
class CPathFollower : public CRGFComponent
{
public:
	CPathFollower();							///< Default constructor
	~CPathFollower();							///< Default destructor

	int Tick(geFloat dwTicks);					///< Check range-based motion triggers
	bool HandleCollision(const geWorld_Model *Model);
	// Find next position for entity
	int GetNextPosition(const char *szEntityName, geVec3d *NextPosition, bool YLocked);
	int GetTarget(const char *szEntityName, geVec3d *Target);			///< Get target point for entity
	int GetPathOrigin(const char *szEntityName, geVec3d *PathOrigin);	///< Get path origin for entity
	geFloat GetSpeed(const char *szEntityName);							///< Get speed of entity
	int GetDirectionVector(const geVec3d &LookFrom, const geVec3d &LookAt,
							geVec3d *theDirectionVector);				///< Get normalized direction vector
	int GetRotationToFacePoint(const geVec3d &LookFrom, const geVec3d &LookAt,
							geVec3d *LookRotation);						///< Get rotation needed to face a point
	int ReSynchronize();

private:
	int TypeNameToIndex(const char *szName);
	int m_PathFollowerCount;					///< # of path followers
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
