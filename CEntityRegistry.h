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

class CEntityRegistry
{
public:
	CEntityRegistry();							// Default constructor
	~CEntityRegistry();							// Default destructor

	int AddEntity(char *szName, char *szType);	// Add entity to registry
	int DeleteEntity(char *szName);				// Delete entity from registry
	char *GetEntityType(char *szName);			// Get type for entity
	void Dump();								// Debug dumper

private:

	CPersistentAttributes *m_pList;				// Registry list
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
