/************************************************************************************//**
 * @file CFirePoint.h
 * @brief FirePoint handler
 *
 * This file contains the class declaration for the CFirePoint class that
 * encapsulates weapon firing for RGF-based games.
 * @author Ralph Deane
 *//*
 * Copyright (c) 1999 Ralph Deane; All rights reserved
 ****************************************************************************************/

#ifndef __RGF_CFIREPOINT_H_
#define __RGF_CFIREPOINT_H_

#pragma warning( disable : 4068 )

#include "genesis.h"

// class declaration for CRain
class CFirePoint : public CRGFComponent
{
public:
	CFirePoint();
	~CFirePoint();

	void Tick(float dwTicks);
	int LocateEntity(char *szName, void **pEntityData);
	int ReSynchronize();
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
