#include "RabidFramework.h"
/************************************************************************************//**
 * @file CMorphingFields.h
 * @brief Morphing Fields class handler
 *
 * This file contains the class declaration for morphing fields handling.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_CMORPHINGFIELDS_H_
#define __RGF_CMORPHINGFIELDS_H_

class CMorphingFields : public CRGFComponent
{
public:
	CMorphingFields();
	~CMorphingFields();

	void Tick(geFloat dwTicks);				// Do any animation
	int BindToPath(char *szName);			// Bind entity to motion path
	int SaveTo(FILE *SaveFD);
											// Save state to a file
	int RestoreFrom(FILE *RestoreFD);		// Restore state from a file
	int LocateEntity(char *szName, void **pEntityData);
	int ReSynchronize();

private:
	int m_MorphingFieldCount;				// Count of morphing fields
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
