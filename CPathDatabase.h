/*
	CPathDatabase.h:		Path and pathpoint in-memory database

	(c) 2001 Ralph Deane

	This file contains the declaration of the CPathDatabase in-
memory path and pathpoint database.  This database is used to drive
PathFollower entities and to provide a database interface to the
eventual path-following AI code.
*/

#ifndef __RGF_CPATHDATABASE_H_
#define __RGF_CPATHDATABASE_H_

struct PathDatabaseRecord
{
  char szName[128];												// Point name
	geVec3d Position;												// Position
	int nType;															// Point type
	geFloat Range;														// If ranged, the range
	PathDatabaseRecord *pPrevious;					// Previous point in path, if any
	PathDatabaseRecord *pNext;							// Next point in path, if any
};

class CPathDatabase
{
public:
  CPathDatabase();			// Default constructor
	~CPathDatabase();															// Default destructor
	int Locate(char *szPointName, int *nType, 
		geVec3d *Position, geFloat *fRange);					// Find a point in the database
	int OpenPath(char *szPointName, geVec3d *Position);
																					// Find first point in the database,
																					// ..return handle to path
	int NextPoint(int nPathHandle, geVec3d *Position);
																					// Find next point in the path
	int PreviousPoint(int nPathHandle, geVec3d *Position);
																					// Find previous point in the path
	int Rewind(int nPathHandle, geVec3d *Position);
																					// Return to first point in list
	int GetPointRange(int nPathHandle, geFloat *fRange);
																					// If current point ranged, get range
  int ClosePath(int nPathHandle);					// Release path handle
private:
  PathDatabaseRecord *CreateNewPath(char *szPointName, geVec3d Position, 
									int nType, geFloat Range);
	int AddToPath(PathDatabaseRecord *pHead, char *szPointName, geVec3d Position,
									int nType, geFloat fRange);
  PathPoint *FindPathPoint(char *szPointName);
  int m_nPathCount;												// # of paths in database
	PathDatabaseRecord *m_DB[1000];					// Up to 1000 paths
	PathDatabaseRecord *m_Handles[500];			// Up to 500 open paths
};

#endif
