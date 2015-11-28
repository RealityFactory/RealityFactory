/************************************************************************************//**
 * @file CFirePoint.h
 * @brief FirePoint handler
 *
 * This file contains the class declaration for the CFirePoint class that
 * encapsulates weapon firing for RGF-based games.
 * @author Ralph Deane
 *//*
 * Copyright (c) 1999 Ralph Deane; All rights reserved
 ****************************************************************************************/

#ifndef __RGF_CFIREPOINT_H_
#define __RGF_CFIREPOINT_H_

/**
 * @brief class declaration for CFirePoint
 */
class CFirePoint : public CRGFComponent
{
public:
	CFirePoint();
	~CFirePoint();

	void Tick(geFloat dwTicks);
	int LocateEntity(const char *szName, void **pEntityData);
	int ReSynchronize();
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
