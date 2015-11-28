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

/**
 * @brief Class declaration for CFoliage
 */
class CFoliage : public CRGFComponent
{
public:
	CFoliage();
	~CFoliage();

	void Tick(geFloat dwTicks);
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
