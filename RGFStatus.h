/************************************************************************************//**
 * @file RGFStatus.h
 * @brief Reality Factory defines file
 *
 * This file contains all the various status, error, key, and result code
 * defines used throughout the RGF system.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_RGFSTATUS_H__
#define __RGF_RGFSTATUS_H__

//	Various status codes

#define	RGF_SUCCESS				1		// Successful result
#define RGF_FAILURE				0		// Failure
#define RGF_UNIMPLEMENTED		-1		// Not implemented
#define RGF_FATAL_ABORT			-2		// FATAL, caller should ABORT program
#define RGF_NO_INPUT			0		// No user input to process
#define RGF_HIT_CHANGELEVEL		-3		// Player avatar hit a change-level entity
#define RGF_NOT_FOUND			-4		// Something not located
#define RGF_NO_NEXT				-5		// No next something
#define RGF_NO_PREVIOUS			-6		// No previous something
#define RGF_NO_FREE_SPACE		-7		// No free space for action
#define RGF_EMPTY				-8
#define RGF_RECHECK				-9

//	RGF collision codes

#define RGF_NO_COLLISION		0x0000	// No collision
#define RGF_COLLIDE_AABB		0x0001	// Bounding box collision
#define RGF_COLLIDE_ACTOR		0x0002	// Actor collision
#define RGF_COLLIDE_MODEL		0x0004	// Model collision
#define RGF_COLLIDE_MESH		0x0008	// Mesh collision
#define RGF_COLLIDE_CONTENTS	0x000e	// Collide w/ contents

#define RGF_COLLISIONLEVEL_1	0x0001	// AABB, total actor only
#define RGF_COLLISIONLEVEL_2	0x0002	// AABB, at the bone level

//	RGF action codes

typedef enum
{
	RGF_K_NOACTION = 0,
	RGF_K_FORWARD,
	RGF_K_BACKWARD,
	RGF_K_TURN_LEFT,
	RGF_K_TURN_RIGHT,
	RGF_K_LEFT,
	RGF_K_RIGHT,
	RGF_K_JUMP,
	RGF_K_CROUCH,
	RGF_K_LOOKUP,
	RGF_K_LOOKDOWN,
	RGF_K_LOOKSTRAIGHT,
	RGF_K_FIRE,
	RGF_K_ALTFIRE,
	RGF_K_WEAPON_0,
	RGF_K_WEAPON_1,
	RGF_K_WEAPON_2,
	RGF_K_WEAPON_3,
	RGF_K_WEAPON_4,
	RGF_K_WEAPON_5,
	RGF_K_WEAPON_6,
	RGF_K_WEAPON_7,
	RGF_K_WEAPON_8,
	RGF_K_WEAPON_9,
	RGF_K_RUN,
	RGF_K_CAMERA,
	RGF_K_ZOOM_IN,
	RGF_K_ZOOM_OUT,
	RGF_K_CAMERA_RESET,
	RGF_K_EXIT,
	RGF_K_QUICKSAVE,
	RGF_K_QUICKLOAD,
	RGF_K_FIRST_PERSON_VIEW,
	RGF_K_THIRD_PERSON_VIEW,
	RGF_K_ISO_VIEW,
	RGF_K_SKIP,
	RGF_K_HELP,
	RGF_K_HUD,
	RGF_K_LOOKMODE,
	RGF_K_SCRNSHOT,
	RGF_K_ZOOM_WEAPON,
	RGF_K_HOLSTER_WEAPON,
	RGF_K_LIGHT,
	RGF_K_USE,
	RGF_K_INVENTORY,
	RGF_K_CONSOLE,
	RGF_K_DROP,
	RGF_K_RELOAD,
	RGF_K_POWERUP,
	RGF_K_POWERDWN,
	RGF_K_MAXACTION
} ACTIONCODES;

typedef enum
{
	KEY_ESC =0,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
	KEY_0,
	KEY_MINUS,
	KEY_BACK,
	KEY_TAB,
	KEY_Q,
	KEY_W,
	KEY_E,
	KEY_R,
	KEY_T,
	KEY_Y,
	KEY_U,
	KEY_I,
	KEY_O,
	KEY_P,
	KEY_RETURN,
	KEY_CONTROL,
	KEY_A,
	KEY_S,
	KEY_D,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_TILDE,
	KEY_SHIFT,
	KEY_Z,
	KEY_X,
	KEY_C,
	KEY_V,
	KEY_B,
	KEY_N,
	KEY_M,
	KEY_PERIOD,
	KEY_SLASH,
	KEY_SPACE,
	KEY_ALT,
	KEY_PLUS,
	KEY_INSERT,
	KEY_HOME,
	KEY_PAGEUP,
	KEY_DELETE,
	KEY_END,
	KEY_PAGEDOWN,
	KEY_UP,
	KEY_DOWN,
	KEY_LEFT,
	KEY_RIGHT,
	KEY_F1,
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	KEY_F6,
	KEY_F7,
	KEY_F8,
	KEY_F9,
	KEY_F10,
	KEY_F11,
	KEY_F12,
	KEY_SYSRQ,
	KEY_LBUTTON,
	KEY_RBUTTON,
	KEY_MBUTTON, // update #2
	KEY_LBRACKET,
	KEY_RBRACKET,
	KEY_EQUAL,
	KEY_BACKSLASH,
	KEY_SEMICOLON,
	KEY_COMMA,
	KEY_APOSTROPHE,
	KEY_NUMPAD0,
	KEY_NUMPAD1,
	KEY_NUMPAD2,
	KEY_NUMPAD3,
	KEY_NUMPAD4,
	KEY_NUMPAD5,
	KEY_NUMPAD6,
	KEY_NUMPAD7,
	KEY_NUMPAD8,
	KEY_NUMPAD9,
	KEY_DECIMAL,
	KEY_MAXIMUM
} KEYCODES;

//	RGF model types

#define RGF_NOT_A_MODEL			0x0000		// Model unknown to us
#define RGF_MODEL_DOOR			0x0001		// Model is a door
#define RGF_MODEL_PLATFORM		0x0002		// Model is a platform
#define RGF_MODEL_DOORTRIGGER	0x0003		// Model is a door trigger
#define RGF_MODEL_TELEPORTER	0x0004		// Model is a teleporter
#define RGF_MODEL_TPTARGET		0x0005		// Model is a teleport target
#define RGF_MODEL_PTTRIGGER		0x0006		// Model is a platform trigger
#define RGF_MODEL_ACT_STATIC	0x0007		// Model is a static actor
#define RGF_MODEL_ACT_MOVING	0x0008		// Model is a moving actor
#define RGF_MODEL_NPC			0x0009		// Model is a non-player character

//	RGF attribute compare values

#define	RGF_ATTR_EQUAL			0x0001		// Equal to
#define	RGF_ATTR_LESS			0x0002		// Less than
#define RGF_ATTR_GREATER		0x0003		// Greater than
#define	RGF_ATTR_NOTEQUAL		0x0004		// Not equal
#define RGF_ATTR_LESSEQ			0x0005		// Less than or equal to
#define	RGF_ATTR_GREATEREQ		0x0006		// Greater than or equal to

//	RGF pathpoint types

#define RGF_POINT_TYPE_START	0x0000		// Start path point
#define RGF_POINT_TYPE_WAY		0x0001		// Waypoint
#define RGF_POINT_TYPE_RANGED	0x0002		// Ranged point

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
