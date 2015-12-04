/************************************************************************************//**
 * @file CProcedural.h
 * @brief Procedural handler
 *
 * This file contains the class declaration for the CProcedural class that
 * encapsulates Procedural handling in the RGF.
 * @author  Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_CProcedural_H__
#define __RGF_CProcedural_H__

/**
 * @brief CProcedural class
 */
class CProcedural : public CRGFComponent
{
public:
	CProcedural();				///< Constructor
	~CProcedural();				///< Destructor

	/**
	 * @brief Update procedural textures
	 */
	void Tick(geFloat dwTicks);

	/**
	 * @brief Correct internal timing to match current time, to make up for time
	 * lost when outside the game loop (typically in "menu mode").
	 */
	int ReSynchronize();
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
