/************************************************************************************//**
 * @file CAreaChecker.h
 * @author Dan Valeo
 *//*
 * Copyright (c) 2004 Dan Valeo; All rights reserved
 ****************************************************************************************/

#ifndef __RGF_CAREACHECK_H_
#define __RGF_CAREACHECK_H_

#pragma warning( disable : 4068 )

#include "genesis.h"

class CAreaChecker : public CRGFComponent
{
public:
	CAreaChecker();
	~CAreaChecker();

	void Tick(float dwTicks);

private:
	bool IsCloseEnough(const geVec3d &Pos1, const geVec3d &pPos2, float diameter);
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
