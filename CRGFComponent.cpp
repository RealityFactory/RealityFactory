/************************************************************************************//**
 * @file CRGFComponent.cpp
 * @brief RGF component base class
 *
 * This file contains error-reporting implementations of the common base class
 * member functions. Typically RGF component writers will over-ride most of
 * these functions.
 * Why isn't this class a pure virtual class? Because I'd rather not force
 * component implementors to write shell functions for unused common interfaces!
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All Rights Reserved.
 ****************************************************************************************/

#include <windows.h> // OutputDebugString
#include <RGFStatus.h>
#include <CRGFComponent.h>

/* ------------------------------------------------------------------------------------ */
//	Default constructor
//
//	Just flags the component as initialized.
/* ------------------------------------------------------------------------------------ */
CRGFComponent::CRGFComponent() :
	m_Initialized(true)
{
}

/* ------------------------------------------------------------------------------------ */
//	Default destructor
//
//	Just flag the component as uninitialized
/* ------------------------------------------------------------------------------------ */
CRGFComponent::~CRGFComponent()
{
}

/* ------------------------------------------------------------------------------------ */
//	Ok, the following implementations are STUB ONLY.  The component
//	..author is expected to over-ride those member functions that
//	..make sense (and will actually be called!) in their components.
/* ------------------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------------------ */
//	Execute
//
//	Passed in a command code and a pointer to any command arguments,
//	..perform the desired function.  This will typically be used to
//	..implement a scripting interface or to expose component functionality
//	..to other components.
/* ------------------------------------------------------------------------------------ */
int CRGFComponent::Execute(char *szName, int nCommand, void *pArgs, int *pArgTypes)
{
	OutputDebugString("Unimplemented Execute called\n");

	return RGF_UNIMPLEMENTED;
}

/* ------------------------------------------------------------------------------------ */
//	LocateEntity
//
//	Passed in a name, the approriate entity is located in the currently
//	..loaded world by name.  If found,the pointer is filled with the
//	..address of the entity-specific user data.  This is typically
//	..used internally to the component, although it could be called
//	..to determine if a name is of this entities type.
/* ------------------------------------------------------------------------------------ */
int CRGFComponent::LocateEntity(const char *szName, void **pEntityData)
{
	OutputDebugString("Unimplemented LocateEntity called\n");

	return RGF_UNIMPLEMENTED;
}

/* ------------------------------------------------------------------------------------ */
//	SetComponentTime
//
//	Passed in the time in timeGetTime() format, set the internal clock
//	..of this component to this time.  Typically, this will be used
//	..to force synchronization with some global time source, say for
//	..multiplayer support.
/* ------------------------------------------------------------------------------------ */
int CRGFComponent::SetComponentTime(DWORD dwTime)
{
	OutputDebugString("Unimplemented SetComponentTime called\n");

	return RGF_UNIMPLEMENTED;
}

/* ------------------------------------------------------------------------------------ */
//	ClearTimeCounter
//
//	Clear out any internal timers or counters in this component.  This
//	..will typically be used to force a reset of the components internal
//	..time.  Again, this is mainly for future multiplayer support.
/* ------------------------------------------------------------------------------------ */
int CRGFComponent::ClearTimeCounter()
{
	OutputDebugString("Unimplemented ClearTimeCounter called\n");

	return RGF_UNIMPLEMENTED;
}

/* ------------------------------------------------------------------------------------ */
//	ReSynchronize
//
//	Reset the 'last time tick' to the current time tick in time-driven
//	..components.  This causes the animation cycles of components that
//	..have been suspended (ie. when the game loop is temporarily exited
//	..to handle menu processing) to resynchronize such that the animations
//	..proceed correctly.
/* ------------------------------------------------------------------------------------ */
int CRGFComponent::ReSynchronize()
{
	OutputDebugString("Unimplemented ReSynchronize called\n");

	return RGF_UNIMPLEMENTED;
}

/* ------------------------------------------------------------------------------------ */
//	DispatchEvent
//
//	Dispatch an event to a specific named entity of this type.  This will
//	..typically be used to pass collision data, environmental changes, etc.
//	..to entities throughout the RealityFactory system.
/* ------------------------------------------------------------------------------------ */
int CRGFComponent::DispatchEvent(char *szName, int EventID,
								 void *pArgs, int *pArgTypes)
{
	OutputDebugString("Unimplemented DispatchEvent called\n");

	return RGF_UNIMPLEMENTED;
}


/* ----------------------------------- END OF FILE ------------------------------------ */
