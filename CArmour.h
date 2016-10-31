/************************************************************************************//**
 * @file CArmour.h
 * @brief CArmour class
 * @author Ralph Deane
 *//*
 * Copyright (c) Ralph Deane; All rights reserved
 ****************************************************************************************/

#ifndef __RGF_CARMOUR_H_
#define __RGF_CARMOUR_H_

#include <hash_map>

#define MAXDAMAGEBY 10

/**
 * @brief Armour type definition
 */
typedef struct ArmourType ArmourType;

/**
 * @brief CArmour handles armour of player
 *
 * Adjust damage done to player and armour HUD elements
 */
class CArmour : public CRGFComponent
{
public:
	CArmour();
	~CArmour();

	bool IsArmour(const std::string &attr) const;

	void DisableHud(const std::string& attr) const;

	/**
	 * @brief Reduce damage according to armour
	 * @param amount original amount of damage
	 * @param name name of damage dealer
	 * @param attr attribute to be damaged
	 * @returns adjusted amount of damage
	 */
	int AdjustDamage(int amount, const std::string& name, const std::string& attr) const;

private:
	stdext::hash_map<std::string, ArmourType*> m_Armours;
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
