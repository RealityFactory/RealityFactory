/************************************************************************************//**
 * @file CLiquid.h
 * @brief Overlay
 *
 * This file contains the class declaration for the COverlay class.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_COVERLAY_H__
#define __RGF_COVERLAY_H__

/**
 * @brief COverlay class declaration
 */
class COverlay : public CRGFComponent
{
public:
	COverlay();
	~COverlay();

	void Tick(geFloat dwTicks);

	Overlay *IsOverlay(const geWorld_Model *theModel);

	void Render();
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
