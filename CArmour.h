/************************************************************************************//**
 * @file CArmour.h
 * @brief CArmour class
 * @author Ralph Deane
 *//*
 * Copyright (c) Ralph Deane; All rights reserved
 ****************************************************************************************/

#ifndef __RGF_CARMOUR_H_
#define __RGF_CARMOUR_H_

#define MAXARMOUR 20
#define MAXDAMAGEBY 10

/**
 * @brief Armour type definition
 */
typedef struct ArmourType
{
	char	*Name;
	char	*Attribute;
	char	*DamageBy[MAXDAMAGEBY];
	int		Protection[MAXDAMAGEBY];
	int		DamageTo[MAXDAMAGEBY];

} ArmourType;

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

	void DisableHud(const char *Attr);
	int AdjustDamage(int Amount, const char *name, const char *Attr);

private:
	ArmourType List[MAXARMOUR];
	int ListPtr;
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
