/************************************************************************************//**
 * @file CEntityRegistry.cpp
 * @brief Entity name and type registry
 *
 * This file contains the class implementation for the Entity Registry.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved
 ****************************************************************************************/

#include "RabidFramework.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#include "RGFStatus.h"
#include "CEntityRegistry.h"
#include "CPersistentAttributes.h"

/* ------------------------------------------------------------------------------------ */
//	Constructor
//
//	Just initialize a clean persistent attributes object
/* ------------------------------------------------------------------------------------ */
CEntityRegistry::CEntityRegistry()
{
	m_pList = new CPersistentAttributes;

	return;
}

/* ------------------------------------------------------------------------------------ */
//	Destructor
//
//	Just delete the persistent attributes list
/* ------------------------------------------------------------------------------------ */
CEntityRegistry::~CEntityRegistry()
{
	m_pList->Clear();
	delete m_pList;
	m_pList = NULL;

	return;
}

/* ------------------------------------------------------------------------------------ */
//	AddEntity
//
//	Add the entity named 'szName' of type 'szType' to the entity
//	..list.  Note that multiple entities with the same name end
//	..up creating multiple instances and are NOT uniquely tagged.
/* ------------------------------------------------------------------------------------ */
int CEntityRegistry::AddEntity(const char *szName, const char *szType)
{
	//if(m_pList->Has(szName))
	//{
		//m_pList->Add(szName);
	//}
	//else
	{
		m_pList->Add(szName);
		m_pList->AllocateUserData(szName, strlen(szType)+1);
		char *szTemp = reinterpret_cast<char*>(m_pList->UserData(szName));
		strcpy(szTemp, szType);						// Save item type off
	}

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	DeleteEntity
//
//	Remove an entity instance of name 'szName'.  If the last instance
//	..is removed, the record is deleted.
/* ------------------------------------------------------------------------------------ */
int CEntityRegistry::DeleteEntity(const char *szName)
{
	m_pList->Remove(szName);				// Drop the instance

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	GetEntityType
//
//	Given an entity name, return the type associated with it.
/* ------------------------------------------------------------------------------------ */
char *CEntityRegistry::GetEntityType(const char *szName)
{
	if(!m_pList->Has(szName))
		return NULL;							// No such entity?

	return reinterpret_cast<char*>(m_pList->UserData(szName));
}

/* ------------------------------------------------------------------------------------ */
//	Dump
//
//	Dump the entire entity registry to the debug file and
//	..debug window.  Not typically used, but good for debugging...
/* ------------------------------------------------------------------------------------ */
void CEntityRegistry::Dump()
{
	if(m_pList != NULL)
		m_pList->Dump();

	return;
}

/* ----------------------------------- END OF FILE ------------------------------------ */
