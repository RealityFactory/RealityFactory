/*
CEntityRegistry.cpp:		Entity name and type registry

  (c) 2001 Ralph Deane
  
	This file contains the class implementation of the Entity Registry.
	The Entity Registry is used to maintain a list of entity names and
	types, as well as a system for retrieving and setting information
	common across all RGF entities (like origin and name).
*/

//	Include the One True Header

#include "RabidFramework.h"

//	Constructor
//
//	Just initialize a clean persistent attributes object

CEntityRegistry::CEntityRegistry()
{
	m_pList = new CPersistentAttributes;
	
	return;
}

//	Destructor
//
//	Just delete the persistent attributes list

CEntityRegistry::~CEntityRegistry()
{
	m_pList->Clear();
	delete m_pList;
	m_pList = NULL;
	
	return;
}

//	AddEntity
//
//	Add the entity named 'szName' of type 'szType' to the entity
//	..list.  Note that multiple entities with the same name end
//	..up creating multiple instances and are NOT uniquely tagged.

int CEntityRegistry::AddEntity(char *szName, char *szType)
{
	if(m_pList->Has(szName))
	{
		m_pList->Add(szName);
		char szBug[128];
		sprintf(szBug,"WARNING: duplicate entityname '%s' type '%s'",
			szName, szType);
		CCD->ReportError(szBug, false);
	}
	else
	{
		m_pList->Add(szName);
		m_pList->AllocateUserData(szName, strlen(szType)+1);
		char *szTemp = (char *)m_pList->UserData(szName);
		strcpy(szTemp, szType);						// Save item type off
	}
	
	return RGF_SUCCESS;
}

//	DeleteEntity
//
//	Remove an entity instance of name 'szName'.  If the last instance
//	..is removed, the record is deleted.

int CEntityRegistry::DeleteEntity(char *szName)
{
	m_pList->Remove(szName);				// Drop the instance
	
	return RGF_SUCCESS;
}

//	GetEntityType
//
//	Given an entity name, return the type associated with it.

char *CEntityRegistry::GetEntityType(char *szName)
{
	if(!m_pList->Has(szName))
		return NULL;							// No such entity?
	
	return (char *)m_pList->UserData(szName);
}

//	Dump
//
//	Dump the entire entity registry to the debug file and
//	..debug window.  Not typically used, but good for debugging...

void CEntityRegistry::Dump()
{
	if(m_pList != NULL)
		m_pList->Dump();
	
	return;
}
