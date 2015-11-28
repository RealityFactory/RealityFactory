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

/**
 * @brief This class implements Liquid
 */
class CLiquid : public CRGFComponent
{
public:
	CLiquid();
	~CLiquid();

	Liquid *IsLiquid(const geWorld_Model *theModel);

private:

};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
