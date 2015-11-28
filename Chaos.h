/************************************************************************************//**
 * @file Chaos.h
 * @brief Chaos handler
 *
 * This file contains the class declaration for the Chaos class that
 * encapsulates chaos based special effects for RGF-based games.
 * @author Ralph Deane
 *//*
 * Copyright (c) 1999 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_CHAOS_H_
#define __RGF_CHAOS_H_

#define CHAOS_FORMAT	GE_PIXELFORMAT_32BIT_ARGB

/**
 * @brief Class declaration for Chaos
 */
class Chaos : public CRGFComponent
{
public:
	Chaos();
	~Chaos();

	void Tick(geFloat dwTick);

private:
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
