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

	void Tick(geFloat dwTicks);

	/**
	 * @brief Given a name, locate the desired entity in the currently loaded
	 * level and return its user data.
	 */
	int LocateEntity(const char *szName, void **pEntityData);

	/**
	 * @brief Correct internal timing to match current time, to make up for time
	 * lost when outside the game loop (typically in "menu mode").
	 */
	int ReSynchronize();

private:
	int CreateSpray(Rain *R);
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
