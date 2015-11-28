/************************************************************************************//**
 * @file CShake.h
 * @brief Screen Shake handler
 *
 * This file contains the class declarations for the Screen Shake enitity
 * for RGF-based games.
 * @author Ralph Deane
 *//*
 * Copyright (c) 1999 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_SHAKE_H_
#define __RGF_SHAKE_H_

/**
 * @brief CShake class handles Screenshake entities
 */
class CShake : public CRGFComponent
{
public:
	CShake();
	~CShake();

	void Tick(geFloat dwTicks);
private:
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
