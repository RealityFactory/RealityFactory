/************************************************************************************//**
 * @file CRain.h
 * @brief Rain handler
 *
 * This file contains the class declaration for the CRain class that
 * encapsulates rain based special effects for RGF-based games.
 * @author Ralph Deane
 *//*
 * Copyright (c) 1999 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_CRAIN_H_
#define __RGF_CRAIN_H_

/**
 * @brief Class declaration for CRain
 */
class CRain : public CRGFComponent
{
public:
	CRain();
	~CRain();

	int Create(Rain *R);
	void Tick(geFloat dwTicks);
	int LocateEntity(const char *szName, void **pEntityData);
	int ReSynchronize();
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
