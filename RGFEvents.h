/****************************************************************************************/
/*																						*/
/*	RGFEvents.h:	RGF Component Event and Command Codes								*/
/*																						*/
/*	(c) 2001 Ralph Deane																*/
/*																						*/
/*	This file contains all the RGF EVENT and COMMAND codes that are						*/
/*	used to pass messages between RGF components.										*/
/*																						*/
/****************************************************************************************/


#ifndef __RGF_EVENTS_H_
#define __RGF_EVENTS_H_

//	EVENTS
//
//	Events are sent to components to inform them that Something Happened.

typedef enum
{
	EV_COLLIDE_MODEL = 0,			// Model collided with receiver
									// Argument: pointer to model that caused collision
	EV_COLLIDE_ACTOR,				// Actor collided with receiver
									// Argument: pointer to actor that caused collision
	EV_COLLIDE_MESH,				// Some geometry collided with receiver
									// Argument: pointer to mesh that caused collision
	EV_COLLIDE_PARTICLE,			// Particle collided with receiver
									// Argument: pointer to particle system that generated particle
	EV_COLLIDE_ACTOR_MODEL,			// Actor collided with model
									// Argument: pointer to actor that hit model
									// Argument: pointer to model that was hit
	EV_COLLIDE_ACTOR_ACTOR,			// Two actors have collided
									// Argument: pointer to first actor in collision
									// Argument: pointer to second actor in collision
	EV_LEVEL_CHANGE					// Level change is about to occur
									// Global notification, no arguments
} RGFEventCodes;

typedef enum
{
	CMD_HEALTH_MOD = 0,				// Update receivers health level
									// Argument: FLOAT amount to modify health by
	CMD_TRANSLATE,					// Move a specific instance of the receiver
									// Argument: STRING name of entity to move
									// Argument: GEVEC3D place to move entity to
	CMD_ADD,						// Add an instance of the receiver
									// Argument: Number of entities to add
	CMD_REMOVE						// Remove an instance of the receiver
									// Argument: STRING name of entity to remove
} RGFCommandCodes;

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
