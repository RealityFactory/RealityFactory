/*
	CEntityRegistry.h:		Entity name and type registry

	(c) 2001 Ralph Deane

	This file contains the class declaration for the Entity Registry.
The Entity Registry is used to maintain a list of entity names and
types, as well as a system for retrieving and setting information
common across all RGF entities (like origin and name).
*/

#ifndef __RGF_CENTITYREGISTRY_H_
#define __RGF_CENTITYREGISTRY_H_

class CEntityRegistry
{
public:
  CEntityRegistry();		// Default constructor
	~CEntityRegistry();														// Default destructor
	int AddEntity(char *szName, char *szType);		// Add entity to registry
	int DeleteEntity(char *szName);								// Delete entity from registry
  char *GetEntityType(char *szName);						// Get type for entity
  void Dump();					// Debug dumper
private:
  CPersistentAttributes *m_pList;								// Registry list
};

#endif
