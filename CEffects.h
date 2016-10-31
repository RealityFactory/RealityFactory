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

#include <hash_map>

class EffectDefinition;

/**
 * @brief CPreEffect handles predefined effects
 */
class CPreEffect : public CRGFComponent
{
public:
	CPreEffect();
	~CPreEffect();

	bool EffectExists(const std::string& effectName);

	int EffectType(const std::string& effectName);

	int AddEffect(const std::string& effectName, const geVec3d& position, const geVec3d& offset);

private:
	stdext::hash_map<std::string, EffectDefinition*> m_Effects;
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
