/************************************************************************************//**
 * @file RGFTypes.h
 * @brief RGF Types, Defines, etc.
 *
 * This file contains various types, defines, etc. used througout an RGF-based program.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All Rights Reserved.
 ****************************************************************************************/

#ifndef __RGF_TYPES_H_
#define __RGF_TYPES_H_

/**
 * @brief RGF Entity Types
 */
typedef enum
{
	ENTITY_DOOR = 0,			///< DOOR entity
	ENTITY_WEAPON,				///< Weapon entity
	ENTITY_PROJECTILE,			///< Projectile Entity
	ENTITY_VIEWEAPON,			///< 1st person view weapon
	ENTITY_TRIGGER,				///< TRIGGER entity
	ENTITY_PLATFORM,			///< PLATFORM entity
	ENTITY_ACTOR,				///< ACTOR entity
	ENTITY_PROP,				///< PROP entity
	ENTITY_NPC,					///< NPC entity
	ENTITY_VEHICLE,				///< Vehicle entity
	ENTITY_ATTRIBUTE_MOD,		///< Attribute modifier (powerup/ammo/health)
	ENTITY_LIQUID,
// changed RF064
	ENTITY_OVERLAY,
// end change RF064
	ENTITY_GENERIC				///< Generic "thing"
} RGFEntityTypes;

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
