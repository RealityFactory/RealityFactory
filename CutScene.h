/************************************************************************************//**
 * @file CutScene.h
 * @brief CCutScene class
 *
 * This file contains the class declaration for the CutScene entity for
 * RGF-based games.
 * @author Daniel Queteschiner
 *//*
 * Copyright (c) 2002 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_CUTSCENE_H_
#define __RGF_CUTSCENE_H_

/**
 * @brief CCutScene handles CutScene entities
 */
class CCutScene : public CRGFComponent
{
public:
	CCutScene();
	~CCutScene();

	void Tick(geFloat dwTicks);
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
