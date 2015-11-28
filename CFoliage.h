/************************************************************************************//**
 * @file CFoliage.h
 * @brief Foliage class declaration
 * @author Dan Valeo
 * @date Jul 2004
 *//*
 * Copyright (c) 2004 Dan Valeo; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_CFOLIAGE_H_
#define __RGF_CFOLIAGE_H_

#pragma warning( disable : 4068 )

#include "genesis.h"

// class declaration for CFlipBook
class CFoliage : public CRGFComponent
{
public:
	CFoliage();
	~CFoliage();

	void Tick(float dwTicks);
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
