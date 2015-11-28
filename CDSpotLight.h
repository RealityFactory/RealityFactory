/************************************************************************************//**
 * @file CDSpotLight.h
 * @brief Dynamic Spotlight handler
 *
 * This file contains the class declaration for the CDSpotLight class that
 * encapsulates all dynamic spot light effects for RGF-based games.
 * @author Daniel Queteschiner
 *//*
 * Copyright (c) 2003 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_CDSPOTLIGHT_H_
#define __RGF_CDSPOTLIGHT_H_

/**
 * @brief CDSpotLight handles DSpotLight entities
 */
class CDSpotLight : public CRGFComponent
{
public:
	CDSpotLight();
	virtual ~CDSpotLight();

	geBoolean Tick(geFloat dwTicks);
	int LocateEntity(const char *szName, void **pEntityData);
	int ReSynchronize();
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
