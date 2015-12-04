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

	/**
	 * @brief Check range-based motion triggers
	 */
	int Tick(geFloat dwTicks);

	bool HandleCollision(const geWorld_Model *Model);

	/**
	 * @brief Find next position for entity
	 */
	int GetNextPosition(const char *szEntityName, geVec3d *NextPosition, bool YLocked);

	/**
	 * @brief Get target point for entity
	 */
	int GetTarget(const char *szEntityName, geVec3d *Target);

	/**
	 * @brief Get path origin for entity
	 */
	int GetPathOrigin(const char *szEntityName, geVec3d *PathOrigin);

	/**
	 * @brief Get speed of entity
	 */
	geFloat GetSpeed(const char *szEntityName);

	/**
	 * @brief Get normalized direction vector
	 */
	int GetDirectionVector(const geVec3d &LookFrom, const geVec3d &LookAt,
							geVec3d *theDirectionVector);

	/**
	 * @brief Get rotation needed to face a point
	 */
	int GetRotationToFacePoint(const geVec3d &LookFrom, const geVec3d &LookAt,
							geVec3d *LookRotation);

	/**
	 * @brief Correct internal timing to match current time, to make up for time
	 * lost when outside the game loop (typically in "menu mode").
	 */
	int ReSynchronize();

private:
	int TypeNameToIndex(const char *szName);
	int m_PathFollowerCount;					///< # of path followers
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
