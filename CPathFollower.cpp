/************************************************************************************//**
 * @file CPathFollower.cpp
 * @brief Entity motion binding component
 *
 * This file contains the implementation for the CPathFollower component.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

// Include the One True Header
#include "RabidFramework.h"

/* ------------------------------------------------------------------------------------ */
//	Constructor
//
//	Initialize all path followers and set the path origin and next target points.
/* ------------------------------------------------------------------------------------ */
CPathFollower::CPathFollower()
{
	m_PathFollowerCount = 0;

	// Ok, check to see if there are path followers in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "PathFollower");

	if(!pSet)
		return;										// No path followers

	geEntity *pEntity;

	// Ok, dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		PathFollower *pFollower = static_cast<PathFollower*>(geEntity_GetUserData(pEntity));

		pFollower->bMoving		= GE_FALSE;			// Not moving
		pFollower->bReady		= GE_TRUE;			// Ready for action!
		pFollower->bTriggered	= GE_FALSE;			// Not triggered
		pFollower->TimeInMotion = 0.0f;				// Not in motion yet
		pFollower->bForward		= GE_TRUE;			// Will move FORWARD

		// Ok, we need to find the origin of the first pathpoint so we know
		// ..where to initially move our bound entity to...
		char *szType = CCD->EntityRegistry()->GetEntityType(pFollower->EntityName);

		if(szType == NULL)
		{
			// We'll consider this a modest error
			char szBug[132];
			sprintf(szBug, "[WARNING] File %s - Line %d: Missing bound entity '%s' in registry\n",
					__FILE__, __LINE__, pFollower->EntityName);
			CCD->ReportError(szBug, false);
			continue;					// Maybe the next one will be OK
		}

		// Ok, get the pathpoint name and retrieve the location of the
		// ..path head.  This will be where we initially move the entity
		// ..to.  Also, at this time, get the location of the next point
		// ..in the path (if any) and store that as our target point
		CCD->PathDatabase()->Locate(pFollower->PathFirstNode,
									&pFollower->CurrentNodeType,
									&pFollower->PathOrigin,
									&pFollower->PointRange);

		pFollower->CurrentPosition = pFollower->PathOrigin;

		if(pFollower->CurrentNodeType != RGF_POINT_TYPE_RANGED)
		{
			// Not a ranged point, get the location of our first target
			// ..node.
			pFollower->PathHandle = CCD->PathDatabase()->OpenPath(pFollower->PathFirstNode,
																	&pFollower->PathOrigin);
			CCD->PathDatabase()->NextPoint(pFollower->PathHandle, &pFollower->CurrentTarget);
		}
		else
		{
			// For a ranged node, compute a random target to move towards inside
			// ..the range sphere.
			srand(CCD->FreeRunningCounter());
			geVec3d RandomPosition;

			RandomPosition.X = static_cast<geFloat>(rand() % static_cast<int>(pFollower->PointRange));
			if(rand()&1)
				RandomPosition.X = -(RandomPosition.X);

			RandomPosition.Y = static_cast<geFloat>(rand() % static_cast<int>(pFollower->PointRange));
			if(rand()&1)
				RandomPosition.Y = -(RandomPosition.Y);

			RandomPosition.Z = static_cast<geFloat>(rand() % static_cast<int>(pFollower->PointRange));
			if(rand()&1)
				RandomPosition.Z = -(RandomPosition.Z);

			pFollower->CurrentTarget = RandomPosition;
		}

		int nTypeID = TypeNameToIndex(szType);			// Turn string to index #

		// At this point, we need to inform the appropriate component that one
		// ..of their entities will be bound to a motion path.  The number of
		// ..bindable entities is relatively small, so we'll just do the old
		// ..'switch and call' method.

		switch(nTypeID)
		{
		// Effect
		case 0:		// MorphingField
			if(CCD->MorphingFields()->BindToPath(pFollower->EntityName) != RGF_SUCCESS)
			{
				char szBug[128];
				sprintf(szBug, "[WARNING] Failed to bind MorphingField entity '%s' to path '%s'\n",
						pFollower->EntityName, pFollower->PathFirstNode);
				CCD->ReportError(szBug, false);
			}
			break;
		case 1:		// StaticEntityProxy
			if(CCD->Props()->BindToPath(pFollower->EntityName) != RGF_SUCCESS)
			{
				char szBug[128];
				sprintf(szBug, "[WARNING] Failed to bind StaticEntityProxy entity '%s' to path '%s'\n",
						pFollower->EntityName, pFollower->PathFirstNode);
				CCD->ReportError(szBug, false);
			}
			break;
		case 2:		// TeleportTarget
			if(CCD->Teleporters()->BindToPath(pFollower->EntityName) != RGF_SUCCESS)
			{
				char szBug[128];
				sprintf(szBug, "[WARNING] Failed to bind TeleportTarget entity '%s' to path '%s'\n",
						pFollower->EntityName, pFollower->PathFirstNode);
				CCD->ReportError(szBug, false);
			}
			break;
		default:
			{
				char szBug[128];
				sprintf(szBug, "[WARNING] Attempt to bind nonbindable entity '%s' type '%s' to path\n",
						pFollower->EntityName, szType);
				CCD->ReportError(szBug, false);
			}
			break;
		}

		// If there are no triggers for the motion, auto-start it when we load.
		if((pFollower->Trigger == NULL) && (pFollower->RangeTrigger == 0.0f))
		{
			pFollower->bTriggered = GE_TRUE;		// Auto-start motion on load
			pFollower->LastMotionTick = CCD->FreeRunningCounter_F();
			pFollower->bMoving = GE_TRUE;
		}

		++m_PathFollowerCount;
	}

	return;
}

/* ------------------------------------------------------------------------------------ */
//	Destructor
//
//	Doesn't do anything.
/* ------------------------------------------------------------------------------------ */
CPathFollower::~CPathFollower()
{
}

/* ------------------------------------------------------------------------------------ */
//	Tick
//
//	Check to see if the player avatar is within trigger range for any
//	..range-triggered entity motion paths.
/* ------------------------------------------------------------------------------------ */
int CPathFollower::Tick(geFloat dwTicks)
{
	// Ok, check to see if there are path followers in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "PathFollower");

	if(!pSet)
		return RGF_FAILURE;											// No path followers

	geEntity *pEntity;
	geVec3d PlayerPos = CCD->Player()->Position();

	// Ok, dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		PathFollower *pFollower = static_cast<PathFollower*>(geEntity_GetUserData(pEntity));

		if(pFollower->bReady == GE_FALSE)
			continue;												// Disabled entity

		if(pFollower->bMoving == GE_TRUE)
			continue;												// Already moving, don't re-trigger

		if(pFollower->RangeTrigger == 0.0f)
			continue;												// Not range trigger, keep moving

		// changed QD 12/15/05
		//if(geVec3d_DistanceBetween(&(pFollower->origin), &PlayerPos) > pFollower->RangeTrigger)
		//	continue;	// Too far away
		geVec3d temp;
		geVec3d_Subtract(&(pFollower->origin), &PlayerPos, &temp);

		if(geVec3d_DotProduct(&temp, &temp) > pFollower->RangeTrigger*pFollower->RangeTrigger)
			continue;
		// end change

		// In range, trigger it!
		pFollower->bTriggered = GE_TRUE;							// Triggered...
		pFollower->bMoving = GE_TRUE;								// We're moving!
		pFollower->TimeInMotion = 0.0f;								// Clear time in motion
		pFollower->LastMotionTick = CCD->FreeRunningCounter_F();	// Prime the time
	}

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	HandleCollision
//
//	Check to see if the model collided with is a motion trigger, and if so,
//	..trigger it and tell the caller.
/* ------------------------------------------------------------------------------------ */
bool CPathFollower::HandleCollision(const geWorld_Model *Model)
{
	// Ok, check to see if there are path followers in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "PathFollower");

	if(!pSet)
		return RGF_FAILURE;											// No path followers

	geEntity *pEntity;

	// Ok, dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		PathFollower *pFollower = static_cast<PathFollower*>(geEntity_GetUserData(pEntity));

		if(pFollower->Trigger != Model)
			continue;												// Keep looking

		if(pFollower->bReady == GE_FALSE)
			return true;											// Disabled, but dealt with

		if(pFollower->bTriggered == GE_TRUE)
			return true;											// Already hit, but dealt with

		if(pFollower->bMoving == GE_TRUE)
			return true;											// Moving, but dealt with

		// Hit and untriggered, trigger it!
		pFollower->bTriggered = GE_TRUE;							// Triggered...
		pFollower->bMoving = GE_TRUE;								// We're moving!
		pFollower->TimeInMotion = 0.0f;								// Clear time in motion
		pFollower->LastMotionTick = CCD->FreeRunningCounter_F();	// Prime the time
		return true;												// Hit, and dealt with!
	}

	return false;													// Not something we dealt with
}

/* ------------------------------------------------------------------------------------ */
//	GetNextPosition
//
//	Given the name of an entity, return it's next position.  This
//	..is the primary motion service!  Based on the current point type
//	..and the kind of motion, return the geVec3d position that the
//	..named entity should be moved to.  A VERY BASIC ray collision test
//	..is performed to determine if the entity can move, so it is
//	..possible for there to be (some) interpenetration, although it
//	..is expected that entities which require "real" collision detection
//	..will do a full-scale collision test using the returned new position.
//	..for ranged motion, a random direction is picked and the entity is
//	..moved in that direction up to the limit of the range.  For RANGED
//	..points, if the parameter YLocked == true then there is no motion
//	..permitted in the Y range (this prevents having props locked to a
//	..motion path from flying in the air!).
/* ------------------------------------------------------------------------------------ */
int CPathFollower::GetNextPosition(const char *szEntityName, geVec3d *NextPosition,
								   bool YLocked)
{
	// Ok, check to see if there are path followers in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "PathFollower");

	if(!pSet)
		return RGF_FAILURE;									// No path followers

	geEntity *pEntity;
	int nResult;
	geVec3d thePos, temp, DirectionVector;

	// Ok, dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		PathFollower *pFollower = static_cast<PathFollower*>(geEntity_GetUserData(pEntity));

		if(strcmp(pFollower->EntityName, szEntityName) != 0)
			continue;										// Keep looking

		// Ok, it's this entity.  Let's set the return position to the
		// ..current position in case we aren't moving
		*NextPosition = pFollower->CurrentPosition;

		// Compute the # of milliseconds since we last moved, then use that
		// ..to compute the distance we've travelled in that period
		geFloat nMotionTime = (CCD->FreeRunningCounter_F() - pFollower->LastMotionTick);

		if(nMotionTime > 100.0f)
			nMotionTime = 100.0f;							// In case of stalls elsewhere

		geFloat fDistance = nMotionTime * (pFollower->Speed * 0.001f); // / 1000.0f);
		pFollower->TimeInMotion += nMotionTime;
		pFollower->LastMotionTick = CCD->FreeRunningCounter_F();

		if(pFollower->bMoving == GE_FALSE)
			return RGF_SUCCESS;								// Current position is good

		if(pFollower->bReady == GE_FALSE)
			return RGF_SUCCESS;								// Disabled, use current position

		switch(pFollower->CurrentNodeType)
		{
		// For both start and waypoints, we're going to compute the new
		// position the same way (since they're both target-based motion).
		case RGF_POINT_TYPE_START:
		case RGF_POINT_TYPE_WAY:
			// Ok, we're moving.  What we need to do is perform a new position
			// ..computation based on the entities speed on the path and the
			// ..vector towards the current target, if not a ranged point.  First,
			// ..compute the direction vector towards the current target
			geVec3d_Subtract(&pFollower->CurrentTarget, &pFollower->CurrentPosition, &DirectionVector);
			geVec3d_Normalize(&DirectionVector);
			geVec3d_Scale(&DirectionVector, fDistance, &DirectionVector);
			// Fine, DirectionVector now contains a displacement from the current
			// ..position of the bound entity towards the target node.
			if(YLocked)
				DirectionVector.Y = 0.0f;			// Stomp on Y vector value
			temp.X = pFollower->CurrentPosition.X + DirectionVector.X;
			temp.Y = pFollower->CurrentPosition.Y + DirectionVector.Y;
			temp.Z = pFollower->CurrentPosition.Z + DirectionVector.Z;
			break;
		case RGF_POINT_TYPE_RANGED:
			// Ranged points are somewhat different.  We're basically constraining
			// ..random motion to a sphere (if not YLocked) of space in which the
			// ..bound entity randomly moves about.  What we'll do is randomly
			// ..pick a target point in the range sphere every <n> seconds and
			// ..have the entity move towards that point.
			if(pFollower->TimeInMotion > pFollower->RandomRollTime)
			{
				// Compute a new random target in the range sphere.
				srand(CCD->FreeRunningCounter());
				geVec3d RandomPosition;
				RandomPosition.X = (geFloat)(rand() % (int)pFollower->PointRange);
				if(rand()%2 == 1)
					RandomPosition.X = -(RandomPosition.X);

				RandomPosition.Y = (geFloat)(rand() % (int)pFollower->PointRange);
				if(rand()%2 == 1)
					RandomPosition.Y = -(RandomPosition.Y);

				RandomPosition.Z = (geFloat)(rand() % (int)pFollower->PointRange);
				if(rand()%2 == 1)
					RandomPosition.Z = -(RandomPosition.Z);

				RandomPosition.X += pFollower->PathOrigin.X;
				RandomPosition.Y += pFollower->PathOrigin.Y;
				RandomPosition.Z += pFollower->PathOrigin.Z;

				pFollower->CurrentTarget = RandomPosition;
				pFollower->TimeInMotion = 0.0f;				// Clear re-seek time
			}
			// From here on in, the code is identical to the target-seeking
			// ..computations performed for way an start points.
			geVec3d_Subtract(&pFollower->CurrentTarget, &pFollower->CurrentPosition, &DirectionVector);
			geVec3d_Normalize(&DirectionVector);
			geVec3d_Scale(&DirectionVector, fDistance, &DirectionVector);
			// Fine, DirectionVector now contains a displacement from the current
			// ..position of the bound entity towards the target node.
			if(YLocked)
				DirectionVector.Y = 0.0f;			// Stomp on Y vector value
			temp.X = pFollower->CurrentPosition.X + DirectionVector.X;
			temp.Y = pFollower->CurrentPosition.Y + DirectionVector.Y;
			temp.Z = pFollower->CurrentPosition.Z + DirectionVector.Z;
			break;
		}

		// Ok, we have a new point to move to!  Let's do a basic ray
		// ..collision test so we can first-level cull any motion
		if(pFollower->CollisionTest == GE_TRUE)
		{
			nResult = CCD->Collision()->CheckForCollision(NULL, NULL,
									pFollower->CurrentPosition,	temp);

			if(nResult)
				return RGF_SUCCESS;						// Don't move, we collided.
		}

		// Fine, no collisions.  Let's check to see if (a) we're not on a
		// ..ranged point and (b) if we're within 1 world unit of the target
		// ..point.  If we pass those tests, we will reset the target to the
		// ..next target point.
		*NextPosition = temp;						// Copy out for use
		pFollower->CurrentPosition = *NextPosition;

		if(pFollower->CurrentNodeType == RGF_POINT_TYPE_RANGED)
			return RGF_SUCCESS;						// Done all we need to do, bail this.

		if(geVec3d_DistanceBetweenSquared(&pFollower->CurrentPosition, &pFollower->CurrentTarget) <= pFollower->Speed*pFollower->Speed)
		{
			// Move to the NEXT path point, based on our direction
			if(pFollower->bForward == GE_TRUE)
			{
				// Get next pathpoint
				nResult = CCD->PathDatabase()->NextPoint(pFollower->PathHandle, &thePos);

				if(nResult == RGF_NO_NEXT)
				{
					// No next point is END OF PATH.  Fine, if looping motion, let's
					// ..switch directions - otherwise shut down the motion.
					if(pFollower->MotionLoops)
					{
						if(pFollower->Reverse)
							pFollower->bForward = GE_FALSE;			// Switch direction
						else
						{
							CCD->PathDatabase()->Rewind(pFollower->PathHandle, &thePos);
							pFollower->CurrentTarget = thePos;
						}
					}
					else
					{
						pFollower->bReady = GE_FALSE;				// End of motion
						pFollower->bMoving = GE_FALSE;				// Don't move any more!
					}
				}
				else
				{
					// All be cool, set our target tot he new point
					pFollower->CurrentTarget = thePos;
				}
			}
			else
			{
				// Get previous pathpoint
				nResult = CCD->PathDatabase()->PreviousPoint(pFollower->PathHandle, &thePos);

				if(nResult == RGF_NO_PREVIOUS)
				{
					// No previous point is START OF PATH.  Fine, if looping motion, let's
					// ..switch directions - otherwise shut down the motion.
					if(pFollower->MotionLoops)
						pFollower->bForward = GE_TRUE;				// Switch direction
					else
					{
						pFollower->bReady = GE_FALSE;				// End of motion
						pFollower->bMoving = GE_FALSE;				// Don't move any more!
					}
				}
				else
				{
					// All be cool, set our target tot he new point
					pFollower->CurrentTarget = thePos;
				}
			}
		}

		// All done, new point back to caller
		return RGF_SUCCESS;
	}

	// Entity not bound, return failure.
	return RGF_FAILURE;
}

/* ------------------------------------------------------------------------------------ */
//	GetTarget
//
//	Given the name of an entity, return the absolute position of it's
//	..current target.  This is used primarily to help set the
//	..facing direction of entities moving through the world.
/* ------------------------------------------------------------------------------------ */
int CPathFollower::GetTarget(const char *szEntityName, geVec3d *Target)
{
	// Ok, check to see if there are path followers in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "PathFollower");

	if(!pSet)
		return RGF_FAILURE;									// No path followers

	geEntity *pEntity;

	// Ok, dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		PathFollower *pFollower = static_cast<PathFollower*>(geEntity_GetUserData(pEntity));

		if(strcmp(pFollower->EntityName, szEntityName) != 0)
			continue;										// Keep looking

		// Ok, it's this entity.  Let's set the return position to the
		// ..current position in case we aren't moving
		*Target = pFollower->CurrentPosition;

		if(pFollower->bMoving == GE_FALSE)
			return RGF_SUCCESS;								// Current position is good

		if(pFollower->bReady == GE_FALSE)
			return RGF_SUCCESS;								// Disabled, use current position

		*Target = pFollower->CurrentTarget;

		// All done, new point back to caller
		return RGF_SUCCESS;
	}

	// Entity not bound, return failure.
	return RGF_FAILURE;
}

/* ------------------------------------------------------------------------------------ */
//	GetPathOrigin
//
//	Return the FIRST point on the path associated with this entity.
/* ------------------------------------------------------------------------------------ */
int CPathFollower::GetPathOrigin(const char *szEntityName, geVec3d *PathOrigin)
{
	// Ok, check to see if there are path followers in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "PathFollower");

	if(!pSet)
		return RGF_FAILURE;									// No path followers

	geEntity *pEntity;

	// Ok, dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		PathFollower *pFollower = static_cast<PathFollower*>(geEntity_GetUserData(pEntity));

		if(strcmp(pFollower->EntityName, szEntityName) != 0)
			continue;										// Keep looking

		// Ok, it's this entity.  Let's set the return position to the
		// ..current position in case we aren't moving
		*PathOrigin = pFollower->PathOrigin;

		// All done, new point back to caller
		return RGF_SUCCESS;
	}

	// Entity not bound, return failure.
	return RGF_FAILURE;
}

/* ------------------------------------------------------------------------------------ */
//	GetSpeed
//
//	Get the motion speed of an entity on a path.
/* ------------------------------------------------------------------------------------ */
geFloat CPathFollower::GetSpeed(const char *szEntityName)
{

	// Ok, check to see if there are path followers in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "PathFollower");

	if(!pSet)
		return RGF_FAILURE;									// No path followers

	geEntity *pEntity;

	// Ok, dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		PathFollower *pFollower = static_cast<PathFollower*>(geEntity_GetUserData(pEntity));

		if(strcmp(pFollower->EntityName, szEntityName) != 0)
			continue;										// Keep looking

		return pFollower->Speed;							// Return speed to caller
	}

	// Entity not bound, return nothing
	return 0.0f;
}

//	****************** PRIVATE MEMBER FUNCTIONS ************

/* ------------------------------------------------------------------------------------ */
//	TypeNameToIndex
//
//	Given a component type name, turn it into a fixed type index.
/* ------------------------------------------------------------------------------------ */
int CPathFollower::TypeNameToIndex(const char *szName)
{
	// Effect
	static char *theNames[] = {"MorphingField", "StaticEntityProxy", "TeleportTarget", NULL};

	for(int nTemp=0; theNames[nTemp]!=NULL; nTemp++)
	{
		if(!strcmp(szName, theNames[nTemp]))
			return nTemp;					// Here's your index
	}

	return NULL;
}

/* ------------------------------------------------------------------------------------ */
//	GetDirectionVector
//
//	Passed in two points, return a normalized direction vector
//	..from the 'LookFrom' point to the 'LookAt' point.
/* ------------------------------------------------------------------------------------ */
int CPathFollower::GetDirectionVector(const geVec3d &LookFrom, const geVec3d &LookAt,
									  geVec3d *theDirectionVector)
{
	geVec3d_Subtract(&LookAt, &LookFrom, theDirectionVector);

	geVec3d_Normalize(theDirectionVector);

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	GetRotationToFacePoint
//
//	Passed in two points, a 'LookFrom' point and a 'LookAt' point,
//	..return the transform required to face 'LookFrom' at 'LookAt'.
//
//	Shamelessly stolen from Seven's ProjectZ code!
/* ------------------------------------------------------------------------------------ */
int CPathFollower::GetRotationToFacePoint(const geVec3d &LookFrom, const geVec3d &LookAt,
										  geVec3d *LookRotation)
{
	geVec3d_Subtract(&LookAt, &LookFrom, LookRotation);

	geFloat l = geVec3d_Length(LookRotation);

	// protect from Div by Zero
	if(l > 0.0f)
	{
		geFloat x = LookRotation->X;

		LookRotation->X = GE_PIOVER2 - acos(LookRotation->Y / l);
		LookRotation->Y = atan2(x, LookRotation->Z) + GE_PI;
		LookRotation->Z = 0.0f;	// roll is zero - always!!?
	}

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	ReSynchronize
//
//	Correct internal timing to match current time, to make up for time lost
//	..when outside the game loop (typically in "menu mode").
/* ------------------------------------------------------------------------------------ */
int CPathFollower::ReSynchronize()
{
	return RGF_SUCCESS;
}


/* ----------------------------------- END OF FILE ------------------------------------ */
