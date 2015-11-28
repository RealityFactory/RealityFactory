/************************************************************************************//**
 * @file CFloating.h
 * @brief Floating Particle handler
 *
 * This file contains the class declaration for the CFloat class that
 * encapsulates floating particle based special effects for RGF-based games.
 * @author Ralph Deane
 *//*
 * Copyright (c) 1999 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_CFLOAT_H_
#define __RGF_CFLOAT_H_

/**
 * @brief Class declaration for CFloat
 */
class CFloat : public CRGFComponent
{
public:
	CFloat();
	~CFloat();

	void Tick(geFloat dwTicks);
	int LocateEntity(const char *szName, void **pEntityData);
	int ReSynchronize();
private:
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
