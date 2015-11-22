/*
	CRGFComponent.h:		RGF component base class

	(c) 2001 Ralph Deane

	This file contains the interface declaration for the Rabid Game
Framework (RGF) common base class.  All RGF components will inherit
from this common base class.  Note that implementations that report
errors are provided for all base class member functions, this is
to ease development of new RGF components.
*/

#ifndef __RGF_CRGFCOMPONENT_H_
#define __RGF_CRGFCOMPONENT_H_

class CRGFComponent
{
public:
  CRGFComponent();						// Default constructor
	~CRGFComponent();						// Default destructor
	int Execute(char *szName, int nCommand, void *pArgs, 
			int *pArgTypes);		// Execute command
	int LocateEntity(char *szName, void **pEntityData);	// Locate an entity
	int SetComponentTime(DWORD dwTime);				// Set components internal time
	int ClearTimeCounter();			// Clear components time counter
	int ReSynchronize();				// Force component clock resynchronization
	int DispatchEvent(char *szName, int EventID, void *pArgs,
			int *pArgTypes);		// Dispatch event to entity
	inline bool IsValid() { return m_Initialized;}
	inline void MarkValid() { m_Initialized = true;}
	inline void MarkInvalid() { m_Initialized = false;}
private:
  bool m_Initialized;					// Component is initialized
};

#endif
