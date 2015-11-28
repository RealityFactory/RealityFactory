#include "RabidFramework.h"
/************************************************************************************//**
 * @file CLiquid.h
 * @brief Liquid
 *
 * This file contains the class declaration for the CLiquid class.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_CLIQUID_H__
#define __RGF_CLIQUID_H__

class CLiquid : public CRGFComponent
{
public:
	CLiquid();
	~CLiquid();

	Liquid *IsLiquid(geWorld_Model *theModel);

private:

};


class COverlay : public CRGFComponent
{
public:
	COverlay();
	~COverlay();

	void Tick(float dwTicks);
	Overlay *IsOverlay(geWorld_Model *theModel);
	void Render();

private:

};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
