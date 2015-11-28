/************************************************************************************//**
 * @file CDynalite.h
 * @brief Dynamic Light handler
 *
 * This file contains the class declarations for the CDynalite class that
 * encapsulates all dynamic point light effects for RGF-based games.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_CDYNALITE_H_
#define __RGF_CDYNALITE_H_

/**
 * @brief CDynalite handles DynamicLight entities
 */
class CDynalite : public CRGFComponent
{
public:
	CDynalite();	///< Default constructor
	~CDynalite();

	geBoolean Tick(geFloat dwTicks);
	int LocateEntity(const char *szName, void **pEntityData);
	int ReSynchronize();
private:

};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
