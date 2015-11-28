/************************************************************************************//**
 * @file CRGFComponent.h
 * @brief RGF component base class
 *
 * This file contains the interface declaration for the Rabid Game Framework
 * (RGF) common base class.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All Rights Reserved.
 ****************************************************************************************/

#ifndef __RGF_CRGFCOMPONENT_H_
#define __RGF_CRGFCOMPONENT_H_

class CRGFComponent
{
public:
	CRGFComponent();									// Default constructor
	~CRGFComponent();									// Default destructor

	// Execute command
	int Execute(char *szName, int nCommand, void *pArgs, int *pArgTypes);
	int LocateEntity(char *szName, void **pEntityData);	// Locate an entity
	int SetComponentTime(DWORD dwTime);					// Set components internal time
	int ClearTimeCounter();								// Clear components time counter
	int ReSynchronize();								// Force component clock resynchronization
	// Dispatch event to entity
	int DispatchEvent(char *szName, int EventID, void *pArgs, int *pArgTypes);

	inline bool IsValid()		{ return m_Initialized;	}
	inline void MarkValid()		{ m_Initialized = true;	}
	inline void MarkInvalid()	{ m_Initialized = false;}
private:
  bool m_Initialized;					// Component is initialized
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
