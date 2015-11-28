/************************************************************************************//**
 * @file CEffects.h
 * @brief Predefined effects handler
 *
 * This file contains the class declarations for the predefined effects for
 * RGF-based games.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_PREEFFMANAGER_H_
#define __RGF_PREEFFMANAGER_H_

#define MAXEXPITEM 200

typedef struct ExpItem
{
	geBoolean	Active;
	char		Name[64];
	int			Type;
	void		*Data;

} ExpItem;

/**
 * @brief CPreEffect handles predefined effects
 */
class CPreEffect : public CRGFComponent
{
public:
	CPreEffect();
	~CPreEffect();

	int AddEffect(int k, const geVec3d &Position, const geVec3d &Offset);
	bool EffectActive(int k)	{ return Effects[k].Active; }
	char *EffectName(int k)		{ return Effects[k].Name;	}
	int EffectType(int k)		{ return Effects[k].Type;	}

private:
	ExpItem Effects[MAXEXPITEM];
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
