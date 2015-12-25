/************************************************************************************//**
 * @file CEntityRegistry.h
 * @brief Entity name and type registry
 *
 * This file contains the class declaration for the Entity Registry.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved
 ****************************************************************************************/

#ifndef __RGF_CENTITYREGISTRY_H_
#define __RGF_CENTITYREGISTRY_H_

class CPersistentAttributes;

/**
 * @brief Entity name and type registry
 *
 * The Entity Registry is used to maintain a list of entity names and types,
 * as well as a system for retrieving and setting information common across
 * all RGF entities (like origin and name).
 */
class CEntityRegistry
{
public:
	CEntityRegistry();							///< Default constructor
	~CEntityRegistry();							///< Default destructor

	int AddEntity(const char *szName, const char *szType);	///< Add entity to registry
	int DeleteEntity(const char *szName);					///< Delete entity from registry
	char *GetEntityType(const char *szName);				///< Get type for entity
	void Dump();											///< Debug dumper

private:

	CPersistentAttributes *m_pList;				///< Registry list
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
