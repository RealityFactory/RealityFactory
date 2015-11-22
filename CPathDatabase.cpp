/*
	CPathDatabase.cpp:		Path and pathpoint in-memory database

	(c) 1999 Edward A. Averill, III

	This file contains the implementation of the CPathDatabase in-
memory path and pathpoint database.  This database is used to drive
PathFollower entities and to provide a database interface to the
eventual path-following AI code.
*/

//	Include the One True Header

#include "RabidFramework.h"

//	Default constructor
//
//	Scan through the current level, creating a doubly-linked-list
//	..in-memory database.

CPathDatabase::CPathDatabase()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	PathPoint *OldPoint = NULL;

  m_nPathCount = 0;												// No paths yet
	memset(&m_DB, 0, 1000 * sizeof(PathDatabaseRecord*));
	memset(&m_Handles, 0, 500 * sizeof(PathDatabaseRecord*));

//	Ok, check to see if there are pathpoints in this world

	pSet = geWorld_GetEntitySet(CCD->World(), "PathPoint");

	if(!pSet) 
		return;									// No pathpoints

//	Ok, scan through the pathpoints in the level.  As we hit any
//	..start nodes, create a new path in the database and then chase
//	..the name links to replicate the entire path in our database.

//	And yes, while this does indeed duplicate the data that's in the
//	..level file, the geEntity interface is not a neatly linked,
//	..searchable table so we're kind of stuck with this.

	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	    pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
		{
		PathPoint *pPoint = (PathPoint*)geEntity_GetUserData(pEntity);
		if(pPoint->PointType != 0)
		  continue;							// Not a head-of-list point
		// Ok, this is the first point on a path.  Let's create a new
		// ..path in our database, then chase the chain and add all
		// ..the waypoints to the new path.
	  PathDatabaseRecord *pPathHead = CreateNewPath(pPoint->PointName, 
			pPoint->origin, pPoint->PointType, pPoint->Range);
	  while((pPoint->NextPointName != NULL) && (strlen(pPoint->NextPointName) != 0))
		  {
			OldPoint = pPoint;
			pPoint = FindPathPoint(pPoint->NextPointName);
			if(pPoint == NULL)
			  {
				char szBug[256];
				sprintf(szBug,"Can't locate linked pathpoint '%s'", OldPoint->NextPointName);
				CCD->ReportError(szBug, false);
			  break;							// Can't find next point?
				}
			AddToPath(pPathHead, pPoint->PointName, pPoint->origin,
					pPoint->PointType, pPoint->Range);			// Add to end of path
			}
		}

  return;
}

//	Default destructor
//
//	Go through and release the current databases memory back to Windoze.

CPathDatabase::~CPathDatabase()
{
  PathDatabaseRecord *pTemp = NULL, *pSaved = NULL;

  for(int nTemp = 0; nTemp < 1000; nTemp++)
	  {
	  if(m_DB[nTemp] == NULL)
		  continue;								// Nothing to delete here
		pTemp = m_DB[nTemp];			// Start here
		while(pTemp != NULL)
		  {
			pSaved = pTemp->pNext;	// Save pointer to next entry
			delete pTemp;						// Gun it!
			pTemp = pSaved;					// To next, if there is one
			}
		m_DB[nTemp] = NULL;				// This path cleaned up
		}

  return;
}

//	Locate
//
//	Scan through all path points in the level, looking for the
//	..specified point.  Return all the info on it when we
//	..find it.

int CPathDatabase::Locate(char *szPointName, int *nType, geVec3d *Position, 
			geFloat *fRange)
{
  PathPoint *pPoint = FindPathPoint(szPointName);

  if(pPoint == NULL)
	  return RGF_FAILURE;					// No such point

//	Point we have, stuff the values and return with 'em.

  *nType = pPoint->PointType;
	*Position = pPoint->origin;
	*fRange = pPoint->Range;

  return RGF_SUCCESS;
}

//	OpenPath
//
//	Scan through the database of paths, looking for one with an initial
//	..point name of 'szPointName'.  Return the position of the point,
//	..as well as a 'handle' to a control block that'll be used to move
//	..around in the specified path.

int CPathDatabase::OpenPath(char *szPointName, geVec3d *Position)
{
  int nTemp, nHandle;

  for(nTemp = 0; nTemp < 1000; nTemp++)
	  {
	  if(m_DB[nTemp] == NULL)
		  continue;									// Empty, don't bother
		if(strcmp(m_DB[nTemp]->szName, szPointName) != 0)
		  continue;									// Wrong name, don't bother
		//	Ok, right name!  Now let's see if we have any free handles...
		for(nHandle = 0; nHandle < 500; nHandle++)
		  if(m_Handles[nHandle] == NULL) break;
		if(nHandle >= 500)
		  break;										// Bail the loop, return an error
		// Ok, we've got a point and we've got a handle.  Do the deed!
		m_Handles[nHandle] = m_DB[nTemp];			// Current record = head of list
		*Position = m_Handles[nHandle]->Position;	// Return origin
		return nHandle;							// Handle back to caller
		}

//	No such path, sorry..

  return -1;
}

//	NextPoint
//
//	Passed the handle of an open path, return the position of the next
//	..point in the list, if there is one.

int CPathDatabase::NextPoint(int nPathHandle, geVec3d *Position)
{
  if((nPathHandle < 0) || (nPathHandle > 500))
	  return RGF_FAILURE;				// Bad handle!

	if(m_Handles[nPathHandle] == NULL)
	  return RGF_FAILURE;				// Still a bad handle

  if(m_Handles[nPathHandle]->pNext == NULL)
	  return RGF_NO_NEXT;				// No next record

//	Ok, all be hunky-dory.  Advance to the next point in the path.

  m_Handles[nPathHandle] =
	  m_Handles[nPathHandle]->pNext;

	*Position = m_Handles[nPathHandle]->Position;

  return RGF_SUCCESS;
}

//	PreviousPoint
//
//	Passed the handle of an open path, return the position of the
//	..previous point in the list, if there is one.

int CPathDatabase::PreviousPoint(int nPathHandle, geVec3d *Position)
{
  if((nPathHandle < 0) || (nPathHandle > 500))
	  return RGF_FAILURE;				// Bad handle!

	if(m_Handles[nPathHandle] == NULL)
	  return RGF_FAILURE;				// Still a bad handle

  if(m_Handles[nPathHandle]->pPrevious == NULL)
	  return RGF_NO_PREVIOUS;				// No previous record

//	Ok, all be hunky-dory.  Move to the previous point in the path.

  m_Handles[nPathHandle] =
	  m_Handles[nPathHandle]->pPrevious;

	*Position = m_Handles[nPathHandle]->Position;

  return RGF_SUCCESS;
}

//	Rewind
//
//	Reset the pointer for this handle to the first point in the path,
//	..and return that points position.

int CPathDatabase::Rewind(int nPathHandle, geVec3d *Position)
{
  if((nPathHandle < 0) || (nPathHandle > 500))
	  return RGF_FAILURE;				// Bad handle!

	if(m_Handles[nPathHandle] == NULL)
	  return RGF_FAILURE;				// Still a bad handle

//	Ok, rewind!

  while(m_Handles[nPathHandle]->pPrevious != NULL)
	  m_Handles[nPathHandle] = m_Handles[nPathHandle]->pPrevious;

  *Position = m_Handles[nPathHandle]->Position;

  return RGF_SUCCESS;
}

//	GetPointRange
//
//	Passed the handle of an open path, return the range value associated
//	..with that point, if any.

int CPathDatabase::GetPointRange(int nPathHandle, geFloat *fRange)
{
  if((nPathHandle < 0) || (nPathHandle > 500))
	  return RGF_FAILURE;				// Bad handle!

	if(m_Handles[nPathHandle] == NULL)
	  return RGF_FAILURE;				// Still a bad handle

  *fRange = m_Handles[nPathHandle]->Range;

  return RGF_SUCCESS;
}

//	ClosePath
//
//	Return the handle indicated to the pool of free path handles.

int CPathDatabase::ClosePath(int nPathHandle)
{
  if((nPathHandle < 0) || (nPathHandle > 500))
	  return RGF_FAILURE;				// Bad handle!

	if(m_Handles[nPathHandle] == NULL)
	  return RGF_FAILURE;				// Still a bad handle

  m_Handles[nPathHandle] = NULL;			// *FOOP* it's free

  return RGF_SUCCESS;
}

//	********** PRIVATE MEMBER FUNCTIONS ************

//	CreateNewPath
//
//	Allocate a new path in the in-memory path database, using the
//	..passed-in data to set up the first point in the path.

PathDatabaseRecord *CPathDatabase::CreateNewPath(char *szPointName, 
	geVec3d Position, int nType, geFloat Range)
{
//	Ok, look for a free entry in our path table

  for(int nTemp = 0; nTemp < 1000; nTemp++)
	  {
		if(m_DB[nTemp] == NULL)
		  {
			// This slot is free!  Create a new path.
			m_DB[nTemp] = new PathDatabaseRecord;
			m_DB[nTemp]->nType = nType;
			m_DB[nTemp]->Position = Position;
			m_DB[nTemp]->Range = Range;
			strcpy(m_DB[nTemp]->szName, szPointName);
			m_DB[nTemp]->pPrevious = m_DB[nTemp]->pNext = NULL;
			return m_DB[nTemp];					// Back to caller
			}
		}

//	No free entries?  That's a problem, ugh...

  return NULL;
}

//	AddToPath
//
//	Passed the address of the head of the list to add the point to,
//	..append this record to the end, patching the previously-last
//	..record to have the new record as it's 'next', and the new
//	..record to have the previous record as it's 'previous'.

int CPathDatabase::AddToPath(PathDatabaseRecord *pHead, char *szPointName, 
	geVec3d Position,	int nType, geFloat fRange)
{
  while(pHead->pNext != NULL)
	  pHead = pHead->pNext;					// Seek to end of list

//	Ok, we're at the last entry in the list.  Make a new record
//	..and append it.

  PathDatabaseRecord *pNew = new PathDatabaseRecord;
	pNew->pPrevious = pHead;				// Point to previous
	pNew->pNext = NULL;							// No next, is now end of list
	pHead->pNext = pNew;						// Point to new record

//	Ok, stuff the data in

  pNew->nType = nType;
	pNew->Position = Position;
	pNew->Range = fRange;
	strcpy(pNew->szName, szPointName);

//	Done, time to leave.

  return RGF_SUCCESS;
}

//	FindPathPoint
//
//	Search the currently loaded level for a specific point and
//	..return it.

PathPoint *CPathDatabase::FindPathPoint(char *szPointName)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

//	Ok, check to see if there are pathpoints in this world

	pSet = geWorld_GetEntitySet(CCD->World(), "PathPoint");

	if(!pSet) 
		return NULL;									// No pathpoints

//	Grovel through the pathpoints looking for the one we want...

	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	    pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
		{
		PathPoint *pPoint = (PathPoint*)geEntity_GetUserData(pEntity);
		if(!strcmp(szPointName, pPoint->PointName))
		  return pPoint;			// Found it!
		}

  return NULL;					// Point not found!
}
