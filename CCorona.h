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

	/**
	 * @brief Given a name, locate the desired entity in the currently loaded
	 * level and return its user data.
	 */
	int LocateEntity(const char *szName, void **pEntityData);

	/**
	 * @brief Correct internal timing to match current time, to make up for time
	 * lost when outside the game loop (typically in "menu mode").
	 */
	int ReSynchronize();

private:
	/**
	 * @brief Create corona effect
	 */
	int Create(const geVec3d &Origin, Corona *pCorona);

private:
	int m_EntityCount;
	geBitmap *m_Bitmap;
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
