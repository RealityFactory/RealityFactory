/************************************************************************************//**
 * @file CChangeLevel.h
 * @brief Changelevel class handler
 *
 * This file contains the class declaration for ChangeLevel handling.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_CCHANGELEVEL_H_
#define __RGF_CCHANGELEVEL_H_

/**
 * @brief CChangeLevel manages all ChangeLevel entities
 */
class CChangeLevel : public CRGFComponent
{
public:
	CChangeLevel();
	~CChangeLevel();

	void Tick(geFloat dwTicks);

	bool CheckChangeLevel(geWorld_Model *theModel, bool UseKey);

	/**
	 * @brief Given a name, locate the desired entity in the currently loaded
	 * level and return its user data.
	 */
	int LocateEntity(const char *szName, void **pEntityData);

	int SaveTo(FILE *SaveFD, bool type);

	int RestoreFrom(FILE *RestoreFD, bool type);
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
