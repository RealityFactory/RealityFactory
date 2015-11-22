/*
		RGFTypes.h:		RGF Types, Defines, etc.

		Authored	07/11/2000		Edward A. Averill, III

		This file contains various types, defines, etc. used througout
an RGF-based program.
*/

#ifndef __RGF_TYPES_H_
#define __RGF_TYPES_H_

typedef enum
{
  ENTITY_DOOR = 0,						// DOOR entity
// Weapon
	ENTITY_WEAPON,							// Weapon entity
	ENTITY_PROJECTILE,						// Projectile Entity
	ENTITY_VIEWEAPON,						// 1st person view weapon
	ENTITY_TRIGGER,							// TRIGGER entity
	ENTITY_PLATFORM,						// PLATFORM entity
	ENTITY_ACTOR,								// ACTOR entity
	ENTITY_PROP,								// PROP entity
	ENTITY_NPC,									// NPC entity
	ENTITY_VEHICLE,							// Vehicle entity
	ENTITY_ATTRIBUTE_MOD,				// Attribute modifier (powerup/ammo/health)
	ENTITY_GENERIC							// Generic "thing"
} RGFEntityTypes;

#endif


