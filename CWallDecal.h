/************************************************************************************//**
 * @file CWallDecal.h
 * @brief WallDecal class handler
 *
 * This file contains the class declaration for WallDecal handling.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2000 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_CWALLDECAL_H_
#define __RGF_CWALLDECAL_H_

/**
 * @brief CWallDecal handles WallDecal entities
 */
class CWallDecal : public CRGFComponent
{
public:
	CWallDecal();
	~CWallDecal();

	void AddDecal(WallDecal *pSource);

	void Tick(geFloat dwTicks);

	geFloat DotProductAngle(geVec3d *V1, geVec3d *V2);

	void AddDecal(WallDecal *pSource, const geVec3d *InVec, const geVec3d *RightVec);

	int  SetProgrammedTrigger(const char *szName, geBoolean Flag);

	int  SetCurrentBitmap(const char *szName, int CurrentBitmap);
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */


