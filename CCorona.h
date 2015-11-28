/************************************************************************************//**
 * @file CCorona.h
 * @brief This file contains the class declaration for Corona handling.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_CCORONA_H_
#define __RGF_CCORONA_H_

/**
 * @brief CCorona handles Corona entities
 */
class CCorona : public CRGFComponent
{
public:
	CCorona();
	~CCorona();

	void Tick(geFloat dwTicks);
	int LocateEntity(const char *szName, void **pEntityData);
	int ReSynchronize();

private:
	/**
	 * @brief Create corona effect
	 */
	int Create(const geVec3d &Origin, Corona *pCorona);

private:
	int Count;
	geBitmap *CoronaBitmap;
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
