/*
	CPathFollower.h:		Entity motion binding component

	(c) 1999 Edward A. Averill, III

	This file contains the declaration for the CPathFollower component.
This component causes moveable entities to travel at a specified speed
between points on a path.  Not all entities are moveable!
*/

#ifndef __RGF_CPATHFOLLOWER_H_
#define __RGF_CPATHFOLLOWER_H_

class CPathFollower : public CRGFComponent
{
public:
  CPathFollower();			// Default constructor
	~CPathFollower();							// Default destructor
  int Tick(geFloat dwTicks);										// Check range-based motion triggers
	bool HandleCollision(geWorld_Model *Model);
  int GetNextPosition(char *szEntityName, geVec3d *NextPosition,
		bool YLocked);									// Find next position for entity
	int GetTarget(char *szEntityName, geVec3d *Target);
																		// Get target point for entity
  int GetPathOrigin(char *szEntityName, geVec3d *PathOrigin);
																		// Get path origin for entity
	geFloat GetSpeed(char *szEntityName);		// Get speed of entity
  int GetDirectionVector(geVec3d LookFrom, geVec3d LookAt,
				geVec3d *theDirectionVector);	// Get normalized direction vector
  int GetRotationToFacePoint(geVec3d LookFrom, geVec3d LookAt,
				geVec3d *LookRotation);	// Get rotation needed to face a point
	int ReSynchronize();
private:
  int TypeNameToIndex(char *szName);
  int m_PathFollowerCount;					// # of path followers
};

#endif
