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

class CChangeLevel : public CRGFComponent
{
public:
	CChangeLevel();
	~CChangeLevel();

	void Tick(float dwTicks);
	bool CheckChangeLevel(geWorld_Model *theModel, bool UseKey);

	int LocateEntity(char *szName, void **pEntityData);

	int SaveTo(FILE *SaveFD, bool type);
	int RestoreFrom(FILE *RestoreFD, bool type);
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
