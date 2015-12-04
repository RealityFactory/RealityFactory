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

/**
 * @brief CRGFComponent common base class
 *
 * All RGF components will inherit from this common base class. Note that
 * implementations that report errors are provided for all base class member
 * functions, this is to ease development of new RGF components.
 */
class CRGFComponent
{
public:
	CRGFComponent();									///< Default constructor
	~CRGFComponent();									///< Default destructor

	int Execute(char *szName, int nCommand, void *pArgs, int *pArgTypes); ///< Execute command

	/**
	 * @brief Given a name, locate the desired entity in the currently loaded
	 * level and return its user data.
	 */
	int LocateEntity(const char *szName, void **pEntityData);

	/**
	 * @brief Correct internal timing to match current time, to make up for time
	 * lost when outside the game loop (typically in "menu mode").
	 */
	int ReSynchronize();

	int SetComponentTime(DWORD dwTime);					///< Set components internal time

	int ClearTimeCounter();								///< Clear components time counter

	int DispatchEvent(char *szName, int EventID, void *pArgs, int *pArgTypes);	///< Dispatch event to entity

	inline bool IsValid()		{ return m_Initialized;	}

	inline void MarkValid()		{ m_Initialized = true;	}

	inline void MarkInvalid()	{ m_Initialized = false;}

private:
	bool m_Initialized;					///< Component is initialized
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
