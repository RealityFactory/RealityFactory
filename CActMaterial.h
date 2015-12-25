/************************************************************************************//**
 * @file CActMaterial.h
 * @brief CActMaterial handler
 *
 * This file contains the class implementation for the
 * CActMaterial entity for RGF-based games.
 * @author Ralph Deane
 * @author Daniel Queteschiner
 *//*
 * Copyright (c) 2002 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_ACTMATERIAL_H_
#define __RGF_ACTMATERIAL_H_

/**
 * @brief CActMaterial handler
 *
 * ActMaterial entities change materials of actors
 */
class CActMaterial : public CRGFComponent
{
public:
	/**
	 * @brief Default constructor
	 */
	CActMaterial();

	/**
	 * @brief Destructor
	 */
	~CActMaterial();

	/**
	 * @brief Update ActMaterial entities
	 */
	void Tick(geFloat dwTicks);

	/**
	 * @brief Save state of ActMaterial entities to a file
	 */
	int SaveTo(FILE *SaveFD, bool type);

	/**
	 * @brief Restore state of ActMaterial entities from a file
	 */
	int RestoreFrom(FILE *RestoreFD, bool type);
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
