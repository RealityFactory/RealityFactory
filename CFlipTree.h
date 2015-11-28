/************************************************************************************//**
 * @file CFlipTree.h
 * @brief CFlipTree class declaration
 * @author Dan Valeo
 * @date Aug 2004
 *//*
 * Copyright (c) 2004 Dan Valeo; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_CFLIPTREE_H_
#define __RGF_CFLIPTREE_H_

/**
 * @brief Class declaration for CFlipBook
 */
class CFlipTree : public CRGFComponent
{
public:
	CFlipTree();
	~CFlipTree();

	void Tick(geFloat dwTicks);
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
