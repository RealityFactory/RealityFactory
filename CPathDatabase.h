/************************************************************************************//**
 * @file CPathDatabase.h
 * @brief Path and pathpoint in-memory database
 *
 * This file contains the declaration of the CPathDatabase in-memory path and
 * pathpoint database.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_CPATHDATABASE_H_
#define __RGF_CPATHDATABASE_H_

#define MAX_PATHS		1000
#define MAX_OPEN_PATHS	500
/**
 * @brief PathDatabaseRecord
 */
struct PathDatabaseRecord
{
	char				szName[128];	///< Point name
	geVec3d				Position;		///< Position
	int					nType;			///< Point type
	geFloat				Range;			///< If ranged, the range
	PathDatabaseRecord	*pPrevious;		///< Previous point in path, if any
	PathDatabaseRecord	*pNext;			///< Next point in path, if any
};

/**
 * @brief CPathDatabase in-memory path and pathpoint database
 *
 * This database is used to drive PathFollower entities and to provide a
 * database interface to the eventual path-following AI code.
 */
class CPathDatabase
{
public:
	CPathDatabase();					///< Default constructor
	~CPathDatabase();					///< Default destructor

	int Locate(const char *szPointName, int *nType, geVec3d *Position, geFloat *fRange);///< Find a point in the database
	int OpenPath(const char *szPointName, geVec3d *Position);							///< Find first point in the database,
																						///< ..return handle to path
	int NextPoint(int nPathHandle, geVec3d *Position);									///< Find next point in the path
	int PreviousPoint(int nPathHandle, geVec3d *Position);								///< Find previous point in the path
	int Rewind(int nPathHandle, geVec3d *Position);										///< Return to first point in list
	int GetPointRange(int nPathHandle, geFloat *Range);									///< If current point ranged, get range
	int ClosePath(int nPathHandle);														///< Release path handle

private:
	PathDatabaseRecord *CreateNewPath(const char *szPointName, const geVec3d &Position,
									int nType, geFloat Range);
	int AddToPath(PathDatabaseRecord *pHead, const char *szPointName, const geVec3d &Position,
									int nType, geFloat fRange);
	PathPoint *FindPathPoint(const char *szPointName);

private:
	int m_nPathCount;								///< # of paths in database
	PathDatabaseRecord *m_DB[MAX_PATHS];			///< Up to 1000 paths
	PathDatabaseRecord *m_Handles[MAX_OPEN_PATHS];	///< Up to 500 open paths
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
