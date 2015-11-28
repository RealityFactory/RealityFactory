/************************************************************************************//**
 * @file CDecal.h
 * @brief Decal class handler
 *
 * This file contains the class declaration for Decal handling.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_CDECAL_H_
#define __RGF_CDECAL_H_

/**
 * @brief Decal definition
 */
typedef struct	Decal
{
	Decal			*next;
	Decal    		*prev;
	float			TimeToLive;
	geBitmap		*Bitmap;
	GE_LVertex		vertex[4];
	long			Leaf;
	float			Width, Height;
	geVec3d			direction;
	geVec3d			OriginOffset;
	geWorld_Model	*Model;

} Decal;

/**
 * @brief CDecal handles DecalDefine entities
 */
class CDecal : public CRGFComponent
{
public:
	CDecal();
	~CDecal();

	void Tick(geFloat dwTicks);
	void AddDecal(int type, geVec3d *impact, geVec3d *normal, geWorld_Model *pModel);

private:

	Decal *Bottom;	///< Linked list of decals
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */

