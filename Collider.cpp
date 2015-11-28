/************************************************************************************//**
 * @file Collider.cpp
 * @brief Collision and Contents Checking Class
 *
 * This file contains the class implementation for the Collider collision and
 * contents checking and handling class. This is an utility class to be used
 * by other RGF classes.
 *//*
 * Thanks to Ng Khoon Siah for the detailed actor collision code and the high
 * level of checking it performs.
 ****************************************************************************************/

//	You only need the one, master include file.
#include "RabidFramework.h"

static int kCollideFlags;

extern "C" void	DrawBoundBox(geWorld *World, const geVec3d *Pos, const geVec3d *Min, const geVec3d *Max);

/* ------------------------------------------------------------------------------------ */
//	CBExclusion
/* ------------------------------------------------------------------------------------ */
geBoolean CBExclusion(geWorld_Model *Model, geActor *Actor, void *Context)
{
	if(Actor == NULL)
		return GE_FALSE;				// NULL actor?

	geActor *thePtr = (geActor*)Context;

	if(Actor == thePtr)
		return GE_FALSE;

	if(CCD->ActorManager()->IsActor(Actor))
		return GE_TRUE;

	return GE_FALSE;
}

/* ------------------------------------------------------------------------------------ */
//	Default constructor
/* ------------------------------------------------------------------------------------ */
Collider::Collider()
{
	m_IgnoreContents = false;					// Do contents checking
	LastCollisionReason = RGF_NO_COLLISION;		// Nothing happened yet
	m_CheckLevel = RGF_COLLISIONLEVEL_1;		// Full collision checking

	kCollideFlags = GE_CONTENTS_SOLID_CLIP | GE_CONTENTS_WINDOW;

	return;
}

/* ------------------------------------------------------------------------------------ */
//	Default destructor
/* ------------------------------------------------------------------------------------ */
Collider::~Collider()
{
	return;
}

/* ------------------------------------------------------------------------------------ */
//	GetContentsOf
//
//	Get the contents of an ExtBox zone
/* ------------------------------------------------------------------------------------ */
int Collider::GetContentsOf(const geVec3d &Position, geExtBox *theBox,
							GE_Contents *theContents)
{
	memset(theContents, 0, sizeof(GE_Contents));

	int Result = GE_FALSE;

	Result = geWorld_GetContents(CCD->World(),
		&Position, &(theBox->Min), &(theBox->Max), GE_COLLIDE_MODELS,
		0xffffffff, NULL, NULL, theContents);

	if(Result == GE_TRUE)
		return RGF_SUCCESS;								// SOMETHING was there...
	else
	{
		// Have to check for world models, feh
		geExtBox pBox = *theBox;
		geWorld_Model *pModel;

		if(CCD->ModelManager()->ContentModel(Position, pBox, &pModel) == GE_TRUE)
		{
			theContents->Model = pModel;
			return RGF_SUCCESS;
		}

		return RGF_FAILURE;								// No contents?
	}
}

/* ------------------------------------------------------------------------------------ */
//	GetContentsOf
//
//	Get the contents of an ExtBox zone, with an optional actor to exclude
/* ------------------------------------------------------------------------------------ */
int Collider::GetContentsOf(const geVec3d &Position, geExtBox *theBox,
							geActor *theActor, GE_Contents *theContents)
{
	memset(theContents, 0, sizeof(GE_Contents));

	int Result = geWorld_GetContents(CCD->World(),
		&Position, &(theBox->Min), &(theBox->Max), GE_COLLIDE_ALL,
		0xffffffff, CBExclusion, theActor, theContents);

	if((Result == GE_FALSE) && (geWorld_GetContents(CCD->World(),
		&Position, &(theBox->Min), &(theBox->Max), GE_COLLIDE_MODELS,
		0xffffffff, NULL, NULL, theContents) == GE_TRUE))
	{
		if((theContents->Contents & (GE_CONTENTS_EMPTY | GE_CONTENTS_SOLID)) == GE_CONTENTS_EMPTY)
			Result = GE_FALSE;
		else
			Result = GE_TRUE;
	}

	if(Result == GE_TRUE)
		return RGF_SUCCESS;							// SOMETHING was there...
	else
	{
		// Have to check for world models, feh
		geExtBox pBox = *theBox;
		geWorld_Model *pModel;
		geActor *pActor;

		if(CCD->ModelManager()->DoesBoxHitModel(Position, pBox, &pModel) == GE_TRUE)
		{
			theContents->Model = pModel;
			return RGF_SUCCESS;
		}

		// Now do the same for managed actors
		if(CCD->ActorManager()->DoesBoxHitActor(Position, pBox, &pActor) == GE_TRUE)
		{
			theContents->Actor = pActor;
			return RGF_SUCCESS;						// Ugh, we hit an actor instead
		}

		return RGF_FAILURE;							// No contents?
	}
}

/* ------------------------------------------------------------------------------------ */
//	CheckActorCollision performs a detailed collision check on the
//	..actor at its current position to see if its in collision with
//	..anything.
/* ------------------------------------------------------------------------------------ */
bool Collider::CheckActorCollision(geActor *theActor)
{
	geBoolean Result = CheckIntersection(theActor, CCD->World());

	if(Result == GE_TRUE)
		return true;							// We're intersecting something
	else
		return false;							// We're clear
}

/* ------------------------------------------------------------------------------------ */
//	IgnoreContents
//
//	Set or unset the "ignore contents" flag.  This forces the collider
//	..to STRICTLY use non-contents collision checking, which is not
//	..always accurate!
/* ------------------------------------------------------------------------------------ */
void Collider::IgnoreContents(bool bFlag)
{
	m_IgnoreContents = bFlag;
}

/* ------------------------------------------------------------------------------------ */
//	CheckLevel
//
//	Set the collision checking level. RGF_COLLISIONLEVEL_1 is
//	..strictly axially-aligned bounding boxes, RGF_COLLISIONLEVEL_2
//	..performs per-bone axially-aligned bounding box checks.
/* ------------------------------------------------------------------------------------ */
void Collider::CheckLevel(int nLevel)
{
	m_CheckLevel = nLevel;
}

/* ------------------------------------------------------------------------------------ */
//	CheckActorCollision performs a detailed collision check on the
//	..actor at a target position to see if it will collide with
//	..anything.
/* ------------------------------------------------------------------------------------ */
bool Collider::CheckActorCollision(geVec3d *Position, geActor *theActor)
{
	//	First, check to see if the bounding box will hit, and if not, we
	//	..need not go to a Stage II collision test

	geExtBox theBox;
	geActor_GetExtBox(theActor, &theBox);

	//	Into model space
	geExtBox tmpBox = theBox;

	tmpBox.Min.X -= Position->X;
	tmpBox.Min.Y -= Position->Y;
	tmpBox.Min.Z -= Position->Z;
	tmpBox.Max.X -= Position->X;
	tmpBox.Max.Y -= Position->Y;
	tmpBox.Max.Z -= Position->Z;

	if(CCD->MenuManager()->GetBoundBox() && theActor==CCD->Player()->GetActor())
		DrawBoundBox(CCD->World(), Position, &tmpBox.Min, &tmpBox.Max);

	if(CanOccupyPosition(Position, &theBox) == GE_TRUE)
		return false;							// Nothing to hit, AABB style

	//	Hmm, we hit with the actor-global bounding box, try checking
	//	..with bone-based AABBs
	geBoolean Result = CheckIntersection(Position, theActor, CCD->World());

	if(Result == GE_TRUE)
		return true;							// We're intersecting something
	else
		return false;							// We're clear
}

/* ------------------------------------------------------------------------------------ */
//	CheckActorCollision performs a detailed collision check on the
//	..actor as it moves to see if it will end up in collision with
//	..anything.
/* ------------------------------------------------------------------------------------ */
bool Collider::CheckActorCollision(const geVec3d &Start, const geVec3d &End, geActor *theActor)
{
	//	First, check to see if the bounding box will hit, and if not, we
	//	..need not go to a Stage II collision test
	geExtBox theBox;
	geActor_GetExtBox(theActor, &theBox);

	geExtBox tBox = theBox;
	tBox.Min.X -= Start.X;
	tBox.Min.Y -= Start.Y;
	tBox.Min.Z -= Start.Z;
	tBox.Max.X -= Start.X;
	tBox.Max.Y -= Start.Y;
	tBox.Max.Z -= Start.Z;

	if(CCD->MenuManager()->GetBoundBox() && theActor == CCD->Player()->GetActor())
		DrawBoundBox(CCD->World(), &End, &tBox.Min, &tBox.Max);

	if(CheckForCollision(&tBox.Min, &tBox.Max, Start, End) == false)
		return false;												// Cleared first hurdle

	//	Ok, sometimes we don't care about anything BUT a bounding-box check,
	//	..so at this point we'll see if that's all we want
	if(m_CheckLevel <= RGF_COLLISIONLEVEL_1)
		return true;							// AABB impact is good enough

	if(!m_IgnoreContents)
	{
		if(CheckIntersection(&End, theActor, CCD->World()))
			return true;
	}

	//	AABB + contents isn't good enough, try an AABB check PER BONE
	if(CheckSubCollision(&Start, &End, theActor, CCD->World()) == GE_TRUE)
		return true;
	else
		return false;							// We'll clear everything
}

/* ------------------------------------------------------------------------------------ */
//	CheckActorCollision performs a detailed collision check on the
//	..actor as it moves to see if it will end up in collision with
//	..anything.  This returns a collision structure.
/* ------------------------------------------------------------------------------------ */
bool Collider::CheckActorCollision(const geVec3d &Start, const geVec3d &End, geActor *theActor,
								   GE_Collision *Collision)
{
	//	First, check to see if the bounding box will hit, and if not, we
	//	..need not go to a Stage II collision test
	geExtBox theBox;

	geActor_GetExtBox(theActor, &theBox);

	theBox.Min.X -= Start.X;
	theBox.Min.Y -= Start.Y;
	theBox.Min.Z -= Start.Z;
	theBox.Max.X -= Start.X;
	theBox.Max.Y -= Start.Y;
	theBox.Max.Z -= Start.Z;

	if(CCD->MenuManager()->GetBoundBox() && theActor==CCD->Player()->GetActor())
		DrawBoundBox(CCD->World(), &End, &theBox.Min, &theBox.Max);

	if((CheckForCollision(&theBox.Min, &theBox.Max, Start, End, Collision, theActor)) == false)
	{
		if(!(m_CheckLevel <= RGF_COLLISIONLEVEL_1))
		{
			if(CheckIntersection(theActor, CCD->World()))
			{
				return true;
			}
		}

		return false;	// Cleared first hurdle
	}

	//	Ok, sometimes we don't care about anything BUT a bounding-box check,
	//	..so at this point we'll see if that's all we want
	if(m_CheckLevel <= RGF_COLLISIONLEVEL_1)
		return true;							// AABB impact is good enough

	if(!m_IgnoreContents)
	{
		if(CheckIntersection(&Start, theActor, CCD->World()))
			return true;

		if(CheckIntersection(&End, theActor, CCD->World()))
			return true;
	}

	//	AABB + contents isn't good enough, try an AABB check PER BONE
	if(CheckSubCollision(&Start, &End, theActor, CCD->World()) == GE_TRUE)
		return true;
	else
		return false;							// We'll clear everything
}

/* ------------------------------------------------------------------------------------ */
//	CheckForCollision performs a two-point collision check to see if
//	..something can move from Point A to Point B.  Not all that accurate
//	..as only general axially-aligned bounding boxes are being used.
/* ------------------------------------------------------------------------------------ */
bool Collider::CheckForCollision(geVec3d *Min, geVec3d *Max,
								 const geVec3d &OldPosition, const geVec3d &NewPosition)
{
	GE_Collision Collision;
	GE_Contents Contents;
	int Result, Result2;

	Result = geWorld_Collision(CCD->World(), Min, Max, &OldPosition, &NewPosition, kCollideFlags,
		GE_COLLIDE_ACTORS, 0xffffffff, NULL, NULL, &Collision);

	if((Result == GE_FALSE) && (geWorld_Collision(CCD->World(), Min,
		Max, &OldPosition, &NewPosition, kCollideFlags,
		GE_COLLIDE_MODELS, 0x0, NULL, NULL, &Collision) == GE_TRUE))
		Result = GE_TRUE;

// changed QD 01/2004
	if((Result == GE_FALSE) && CCD->Meshes()->CollisionCheck(Min, Max, OldPosition, NewPosition, NULL))
		Result = GE_TRUE;
// end change

	//	Only do a GetContents if we're not doing a ray collision AND there
	//	..wasn't a geWorld collision detected.
	if((Min != NULL) && (Max != NULL) && (Result != GE_TRUE) && (!m_IgnoreContents))
	{
		geExtBox pBox;
		pBox.Min = *Min;
		pBox.Max = *Max;
		Result2 = GetContentsOf(NewPosition, &pBox, &Contents);

		if((Contents.Actor == NULL) && (Contents.Model == NULL) && (Contents.Mesh == NULL))
			Result2 = 0;								// No real contents here
	}
	else
		Result2 = 0;									// No contents checking

	//	Set collision reason
	LastCollisionReason = RGF_NO_COLLISION;				// Assume no hit

	if(Result == 1)
		LastCollisionReason |= RGF_COLLIDE_AABB;		// Bounding-box hit

	if(Result2 == 1)
	{
		if(Contents.Actor != NULL)
			LastCollisionReason |= RGF_COLLIDE_ACTOR;	// Hit an actor

		if(Contents.Model != NULL)
			LastCollisionReason |= RGF_COLLIDE_MODEL;	// Hit a model

		if(Contents.Mesh != NULL)
			LastCollisionReason |= RGF_COLLIDE_MESH;	// Hit a mesh
	}

	if((Result == 1) || (Result2 == 1))
		return true;									// Collision confirmed
	else
		return false;									// Naah, we didn't hit anything
}

/* ------------------------------------------------------------------------------------ */
//	CheckForCollision
//
//	Combines two Genesis3D collision detection functions in an
//	..attempt to make REALLY SURE that there's either a collision
//	..or not!  This version does returns collision details.
/* ------------------------------------------------------------------------------------ */
bool Collider::CheckForCollision(geVec3d *Min, geVec3d *Max,
								 const geVec3d &OldPosition, const geVec3d &NewPosition,
								 GE_Collision *Collision)
{
	GE_Contents Contents;
	int Result, Result2;

	Result = geWorld_Collision(CCD->World(), Min, Max, &OldPosition, &NewPosition, kCollideFlags,
		GE_COLLIDE_ACTORS, 0xffffffff, NULL, NULL, Collision);

	if((Result == GE_FALSE) && (geWorld_Collision(CCD->World(), Min,
		Max, &OldPosition, &NewPosition, kCollideFlags,
		GE_COLLIDE_MODELS, 0x0, NULL, NULL, Collision) == GE_TRUE))
		Result = GE_TRUE;

// changed QD 01/2004
	if((Result == GE_FALSE) && CCD->Meshes()->CollisionCheck(Min, Max, OldPosition, NewPosition, Collision))
		Result = GE_TRUE;
// end change

	if((Min != NULL) && (Max != NULL) && (Result != GE_TRUE) && (!m_IgnoreContents))
	{
		Result2 = geWorld_GetContents(CCD->World(),
			&NewPosition, Min, Max, GE_COLLIDE_ALL, 0xffffffff,
			NULL, NULL, &Contents);

		if((Contents.Actor == NULL) && (Contents.Model == NULL) && (Contents.Mesh == NULL))
			Result2 = 0;								// No real contents here
		else if((Contents.Contents & (GE_CONTENTS_EMPTY | GE_CONTENTS_SOLID)) == GE_CONTENTS_EMPTY)
			Result2 = 0;								// It's EMPTY, ignore it
		else
		{
			// Fill collision struct with information
			Collision->Mesh = Contents.Mesh;
			Collision->Model = Contents.Model;
			Collision->Actor = Contents.Actor;
			Collision->Impact = OldPosition;
		}
	}
	else
		Result2 = 0;									// No contents checking

	//	Set collision reason
	LastCollisionReason = RGF_NO_COLLISION;				// Assume no hit

	if(Result == 1)
		LastCollisionReason |= RGF_COLLIDE_AABB;		// Bounding-box hit

	if(Result2 == 1)
	{
		if(Contents.Actor != NULL)
			LastCollisionReason |= RGF_COLLIDE_ACTOR;	// Hit an actor
		if(Contents.Model != NULL)
			LastCollisionReason |= RGF_COLLIDE_MODEL;	// Hit a model
		if(Contents.Mesh != NULL)
			LastCollisionReason |= RGF_COLLIDE_MESH;	// Hit a mesh
	}

	if((Result == 1) || (Result2 == 1))
		return true;									// Collision confirmed
	else
		return false;									// Naah, we didn't hit anything
}

/* ------------------------------------------------------------------------------------ */
//	CheckForCollision
//
//	Combines two Genesis3D collision detection functions in an
//	..attempt to make REALLY SURE that there's either a collision
//	..or not!  This version does returns contents details.
/* ------------------------------------------------------------------------------------ */
bool Collider::CheckForCollision(geVec3d *Min, geVec3d *Max,
								 const geVec3d &OldPosition, const geVec3d &NewPosition,
								 GE_Contents *Contents)
{
	GE_Collision Collision;
	int Result, Result2;

	memset(Contents, 0, sizeof(GE_Contents));

	Result = geWorld_Collision(CCD->World(), Min,
		Max, &OldPosition, &NewPosition, kCollideFlags,
		GE_COLLIDE_ACTORS, 0xffffffff, NULL, NULL, &Collision);

	if((Result == GE_FALSE) && (geWorld_Collision(CCD->World(), Min,
		Max, &OldPosition, &NewPosition, kCollideFlags,
		GE_COLLIDE_MODELS, 0x0, NULL, NULL, &Collision) == GE_TRUE))
		Result = GE_TRUE;

	if((Min != NULL) && (Max != NULL) && (Result != GE_TRUE) && (!m_IgnoreContents))
	{
		Result2 = geWorld_GetContents(CCD->World(),
			&NewPosition, Min, Max, GE_COLLIDE_ALL, 0xffffffff,
			NULL, NULL, Contents);

		if((Contents->Actor == NULL) && (Contents->Model == NULL) && (Contents->Mesh == NULL))
			Result2 = 0;								// No real contents here
		else if((Contents->Contents & (GE_CONTENTS_EMPTY | GE_CONTENTS_SOLID)) == GE_CONTENTS_EMPTY)
			Result2 = 0;								// Ignore empty models
	}
	else
		Result2 = 0;									// No contents checking

	//	Set collision reason
	LastCollisionReason = RGF_NO_COLLISION;				// Assume no hit

	if(Result == 1)
		LastCollisionReason |= RGF_COLLIDE_AABB;		// Bounding-box hit

	if(Result2 == 1)
	{
		if(Contents->Actor != NULL)
			LastCollisionReason |= RGF_COLLIDE_ACTOR;	// Hit an actor

		if(Contents->Model != NULL)
			LastCollisionReason |= RGF_COLLIDE_MODEL;	// Hit a model

		if(Contents->Mesh != NULL)
			LastCollisionReason |= RGF_COLLIDE_MESH;	// Hit a mesh
	}

	if((Result == 1) || (Result2 == 1))
		return true;									// Collision confirmed
	else
		return false;									// Naah, we didn't hit anything
}

/* ------------------------------------------------------------------------------------ */
//	CheckForCollision
//
//	Combines two Genesis3D collision detection functions in an
//	..attempt to make REALLY SURE that there's either a collision
//	..or not!  This version returns BOTH contents and collision data.
/* ------------------------------------------------------------------------------------ */
bool Collider::CheckForCollision(geVec3d *Min, geVec3d *Max,
								 const geVec3d &OldPosition, const geVec3d &NewPosition,
								 GE_Collision *Collision, GE_Contents *Contents)
{
	int Result, Result2;

	memset(Contents, 0, sizeof(GE_Contents));
	memset(Collision, 0, sizeof(GE_Collision));

	Result = geWorld_Collision(CCD->World(), Min,
		Max, &OldPosition, &NewPosition, kCollideFlags,
		GE_COLLIDE_ACTORS, 0xffffffff, NULL, NULL, Collision);

	if((Result == GE_FALSE) && (geWorld_Collision(CCD->World(), Min,
		Max, &OldPosition, &NewPosition, kCollideFlags,
		GE_COLLIDE_MODELS, 0x0, NULL, NULL, Collision) == GE_TRUE))
		Result = GE_TRUE;

	if((Min != NULL) && (Max != NULL) && (Result != 1) && (!m_IgnoreContents))
	{
		Result2 = geWorld_GetContents(CCD->World(),
			&NewPosition, Min, Max, GE_COLLIDE_ALL, 0xffffffff,
			NULL, NULL, Contents);

		if((Contents->Actor == NULL) && (Contents->Model == NULL) && (Contents->Mesh == NULL))
			Result2 = 0;								// No real contents here
		else if((Contents->Contents & (GE_CONTENTS_EMPTY | GE_CONTENTS_SOLID)) == GE_CONTENTS_EMPTY)
			Result2 = 0;								// Ignore empty models
		else
		{
			// Fill collision struct with information
			Collision->Mesh = Contents->Mesh;
			Collision->Model = Contents->Model;
			Collision->Actor = Contents->Actor;
			Collision->Impact = OldPosition;
		}
	}
	else
		Result2 = 0;									// No contents checking

	//	Set collision reason
	LastCollisionReason = RGF_NO_COLLISION;				// Assume no hit

	if(Result == 1)
		LastCollisionReason |= RGF_COLLIDE_AABB;		// Bounding-box hit

	if(Result2 == 1)
	{
		if(Contents->Actor != NULL)
			LastCollisionReason |= RGF_COLLIDE_ACTOR;	// Hit an actor

		if(Contents->Model != NULL)
			LastCollisionReason |= RGF_COLLIDE_MODEL;	// Hit a model

		if(Contents->Mesh != NULL)
			LastCollisionReason |= RGF_COLLIDE_MESH;	// Hit a mesh
	}

	if((Result == 1) || (Result2 == 1))
		return true;									// Collision confirmed
	else
		return false;									// Naah, we didn't hit anything
}

/* ------------------------------------------------------------------------------------ */
//	CheckForCollision
//
//	Combines two Genesis3D collision detection functions in an
//	..attempt to make REALLY SURE that there's either a collision
//	..or not!  This version does returns collision details, and
//	..also takes an ACTOR to exclude from contents checking.
/* ------------------------------------------------------------------------------------ */
bool Collider::CheckForCollision(geVec3d *Min, geVec3d *Max,
								 const geVec3d &OldPosition, const geVec3d &NewPosition,
								 GE_Collision *Collision, geActor *Actor)
{
	GE_Contents Contents;
	int Result, Result2;

	memset(Collision, 0, sizeof(GE_Collision));

	Result = geWorld_Collision(CCD->World(), Min,
		Max, &OldPosition, &NewPosition, kCollideFlags,
		GE_COLLIDE_ACTORS, 0xffffffff, CBExclusion, Actor, Collision);

// changed QD 01/2004
	if((Result == GE_FALSE) && CCD->Meshes()->CollisionCheck(Min, Max, OldPosition, NewPosition, Collision))
	{
		Result = GE_TRUE;
		Collision->Mesh = (geMesh*)CCD->Meshes();		// just give it a pointer
		Collision->Model = NULL;
		Collision->Actor = NULL;
	}
// end change

	if((Result == GE_FALSE) && (geWorld_Collision(CCD->World(), Min,
		Max, &OldPosition, &NewPosition, kCollideFlags,
		GE_COLLIDE_MODELS, 0x0, NULL, NULL, Collision) == GE_TRUE))
	{
		Collision->Actor = NULL;
		//geEngine_Printf(CCD->Engine()->Engine(), 0,30,"Coll Model = %x",Collision->Model);

		if(!(CCD->Doors()->IsADoor(Collision->Model) && CCD->ModelManager()->IsRunning(Collision->Model)))
		{
			Result = GE_TRUE;
			Collision->Impact = OldPosition;
		}
		else
			Collision->Model = NULL;
	}
	else
	{
		Collision->Model = NULL;

		if(Result == GE_TRUE)
			Collision->Impact = OldPosition;
	}

	if((Min != NULL) && (Max != NULL) && (Result != GE_TRUE) && (!m_IgnoreContents))
	{
		geExtBox theBBox;
		theBBox.Min = *Min;
		theBBox.Max = *Max;
		Result2 = CanOccupyPosition(&NewPosition, &theBBox, Actor, &Contents);

		if(!Result2)
		{
			// Fill collision struct with information
			Collision->Mesh = Contents.Mesh;
			Collision->Model = Contents.Model;
			Collision->Actor = Contents.Actor;
			Collision->Impact = OldPosition;
			Result2 = GE_TRUE;
		}
		else
			Result2 = GE_FALSE;
	}
	else
		Result2 = GE_FALSE;								// No contents checking

	//	Set collision reason
	LastCollisionReason = RGF_NO_COLLISION;				// Assume no hit

	if(Result == GE_FALSE)
		LastCollisionReason |= RGF_COLLIDE_AABB;		// Bounding-box hit

// Start Nov2003DCS
	if(Result2 == GE_TRUE)
// End Nov2003DCS
	{
		if(Contents.Actor != NULL)
			LastCollisionReason |= RGF_COLLIDE_ACTOR;	// Hit an actor

		if(Contents.Model != NULL)
			LastCollisionReason |= RGF_COLLIDE_MODEL;	// Hit a model

		if(Contents.Mesh != NULL)
			LastCollisionReason |= RGF_COLLIDE_MESH;	// Hit a mesh
	}

	if((Result == GE_TRUE) || (Result2 == GE_TRUE))
	{
		return true;									// Collision confirmed
	}
	else
	{
		Collision->Impact = NewPosition;
		return false;									// Naah, we didn't hit anything
	}
}

/* ------------------------------------------------------------------------------------ */
//	CheckForWCollision
/* ------------------------------------------------------------------------------------ */
bool Collider::CheckForWCollision(geVec3d *Min, geVec3d *Max,
								 const geVec3d &OldPosition, const geVec3d &NewPosition,
								 GE_Collision *Collision, geActor *Actor)
{
	GE_Contents Contents;
	int Result, Result2;

	memset(Collision, 0, sizeof(GE_Collision));

	Result = geWorld_Collision(CCD->World(), Min,
		Max, &OldPosition, &NewPosition, kCollideFlags,
		GE_COLLIDE_ACTORS, 0xffffffff, CBExclusion, Actor, Collision);

	if((Result == GE_FALSE) && (geWorld_Collision(CCD->World(), Min,
		Max, &OldPosition, &NewPosition, kCollideFlags,
		GE_COLLIDE_MODELS, 0x0, NULL, NULL, Collision) == GE_TRUE))
	{
		Collision->Actor = NULL;

		if(!(CCD->Doors()->IsADoor(Collision->Model) && CCD->ModelManager()->IsRunning(Collision->Model)))
		{
			Result = GE_TRUE;
		}
		else
			Collision->Model = NULL;
	}
	else
	{
		Collision->Model = NULL;
	}

	if((Min != NULL) && (Max != NULL) && (Result != GE_TRUE) && (!m_IgnoreContents))
	{
		geExtBox theBBox;
		theBBox.Min = *Min;
		theBBox.Max = *Max;
		Result2 = CanOccupyPosition(&NewPosition, &theBBox, Actor, &Contents);

		if(!Result2)
		{
			// Fill collision struct with information
			Collision->Mesh = Contents.Mesh;
			Collision->Model = Contents.Model;
			Collision->Actor = Contents.Actor;
			Collision->Impact = OldPosition;
			Result2 = GE_TRUE;
		}
		else
			Result2 = GE_FALSE;
	}
	else
		Result2 = GE_FALSE;								// No contents checking

	//	Set collision reason
	LastCollisionReason = RGF_NO_COLLISION;				// Assume no hit

	if(Result == GE_FALSE)
		LastCollisionReason |= RGF_COLLIDE_AABB;		// Bounding-box hit

	if(Result2 == GE_FALSE)
	{
		if(Contents.Actor != NULL)
			LastCollisionReason |= RGF_COLLIDE_ACTOR;	// Hit an actor

		if(Contents.Model != NULL)
			LastCollisionReason |= RGF_COLLIDE_MODEL;	// Hit a model

		if(Contents.Mesh != NULL)
			LastCollisionReason |= RGF_COLLIDE_MESH;	// Hit a mesh
	}

	if((Result == GE_TRUE) || (Result2 == GE_TRUE))
	{
		return true;									// Collision confirmed
	}
	else
	{
		Collision->Impact = NewPosition;
		return false;									// Naah, we didn't hit anything
	}
}

/* ------------------------------------------------------------------------------------ */
//	CheckModelMotion checks to see if an animated world model can move
//	..to transform ModelPosition, given a potential obstructing object
//	..of size 'ObstructionSize' at position 'ObstructionPosition'.
/* ------------------------------------------------------------------------------------ */
bool Collider::CheckModelMotion(geWorld_Model *theModel, geXForm3d *ModelPosition,
								geExtBox *ObstructionSize, geVec3d *ObstructionPosition,
								geVec3d *OutputObstructionPosition)
{
	// Here's where the MAGICK is.  TestModelMove() won't return GE_TRUE,
	// ..indicating a collision, *UNLESS* there is nowhere to move the bounding
	// ..box passed in such that no collision occurs.  Otherwise, it returns
	// ..GE_FALSE *BUT* THE OUT POSITION WILL CONTAIN WHERE THE OBJECT UNDER
	// ..TEST HAS TO GO TO BE VALID.  What does this _really_ mean?  That
	// ..you'd damn well better set the object under test's (plyPos) position
	// ..to the Out position or you'll be screwed, the platform will pass through
	// ..your object if it's not going to crush it, and you'll tear your
	// ..hair out for weeks wondering why it doesn't work right.

	int nResult = geWorld_TestModelMove(CCD->World(), theModel, ModelPosition,
		&ObstructionSize->Min, &ObstructionSize->Max, ObstructionPosition,
		OutputObstructionPosition);

	if(nResult == GE_TRUE)
		return true;							// Fine, model can move
	else
		return false;							// Not fine, model CAN'T move
}

/* ------------------------------------------------------------------------------------ */
//	CheckModelMotion checks to see if an animated world model can move
//	..to transform ModelPosition, given a potential obstructing object
//	..of size 'ObstructionMin' & 'ObstructionMax' at position 'ObstructionPosition'.
//	..This override takes two points as min/max instead of an ExtBox.
/* ------------------------------------------------------------------------------------ */
bool Collider::CheckModelMotion(geWorld_Model *theModel, geXForm3d *ModelPosition,
								geVec3d *ObstructionMin, geVec3d *ObstructionMax,
								geVec3d *ObstructionPosition,
								geVec3d *OutputObstructionPosition)
{
	// Here's where the MAGICK is.  TestModelMove() won't return GE_TRUE,
	// ..indicating a collision, *UNLESS* there is nowhere to move the bounding
	// ..box passed in such that no collision occurs.  Otherwise, it returns
	// ..GE_FALSE *BUT* THE OUT POSITION WILL CONTAIN WHERE THE OBJECT UNDER
	// ..TEST HAS TO GO TO BE VALID.  What does this _really_ mean?  That
	// ..you'd damn well better set the object under test's (plyPos) position
	// ..to the Out position or you'll be screwed, the platform will pass through
	// ..your object if it's not going to crush it, and you'll tear your
	// ..hair out for weeks wondering why it doesn't work right.

	int nResult = geWorld_TestModelMove(CCD->World(), theModel, ModelPosition,
		ObstructionMin, ObstructionMax, ObstructionPosition,
		OutputObstructionPosition);

	if(nResult == GE_TRUE)
		return true;							// Fine, model can move
	else
		return false;							// Not fine, model CAN'T move
}

/* ------------------------------------------------------------------------------------ */
//	CheckModelMotionActor checks to see if an animated world model can move
//	..to transform ModelPosition, given a potential obstructing actor
//	..at position 'ActorPosition'.
/* ------------------------------------------------------------------------------------ */
bool Collider::CheckModelMotionActor(geWorld_Model *theModel, geXForm3d *ModelPosition,
									 geActor *theActor, geVec3d *ActorPosition,
									 geVec3d *OutputActorPosition)
{
	geExtBox ActorBox;

	CCD->ActorManager()->GetBoundingBox(theActor, &ActorBox);

	// Here's where the MAGICK is.  TestModelMove() won't return GE_TRUE,
	// ..indicating a collision, *UNLESS* there is nowhere to move the bounding
	// ..box passed in such that no collision occurs.  Otherwise, it returns
	// ..GE_FALSE *BUT* THE OUT POSITION WILL CONTAIN WHERE THE OBJECT UNDER
	// ..TEST HAS TO GO TO BE VALID.  What does this _really_ mean?  That
	// ..you'd damn well better set the object under test's (plyPos) position
	// ..to the Out position or you'll be screwed, the platform will pass through
	// ..your object if it's not going to crush it, and you'll tear your
	// ..hair out for weeks wondering why it doesn't work right.

	int nResult = geWorld_TestModelMove(CCD->World(), theModel, ModelPosition,
		&ActorBox.Min, &ActorBox.Max, ActorPosition,
		OutputActorPosition);

	if(nResult == GE_TRUE)
		return true;							// Fine, model can move
	else
		return false;							// Not fine, model CAN'T move
}

/* ------------------------------------------------------------------------------------ */
//	CanOccupyPosition checks to see if a specific zone in the level is
//	..occupiable (??) by something of the specified size 'theBox'.
/* ------------------------------------------------------------------------------------ */
bool Collider::CanOccupyPosition(const geVec3d *thePoint, geExtBox *theBox)
{
	GE_Contents Contents;

	memset(&Contents, 0, sizeof(GE_Contents));

	int Result = geWorld_GetContents(CCD->World(),
		thePoint, &theBox->Min, &theBox->Max, GE_COLLIDE_ALL,
		0xffffffff, NULL, NULL, &Contents);

	if((Result == GE_FALSE) && (geWorld_GetContents(CCD->World(),
		thePoint, &theBox->Min, &theBox->Max, GE_COLLIDE_MODELS,
		0xffffffff, NULL, NULL, &Contents) == GE_TRUE))
	{
		if((Contents.Contents & (GE_CONTENTS_EMPTY | GE_CONTENTS_SOLID)) == GE_CONTENTS_EMPTY)
			Result = GE_FALSE;
		else if((Contents.Contents & GE_CONTENTS_HINT))
		{
			Result = GE_FALSE;
		}
// changed RF064
/*		else if((Contents.Contents & GE_CONTENTS_AREA))
		{
			Result = GE_FALSE;
		} */
// end change RF064
		else
		{
			Result = GE_TRUE;

			if(CCD->Doors()->IsADoor(Contents.Model))
				Result = GE_FALSE;
		}
	}

	if(Result == GE_FALSE)
	{
		// First, check to see if we hit a managed model
		geWorld_Model *pModel;
		geActor *pActor;
		geVec3d pTemp = *thePoint;
		geExtBox pBox = *theBox;
		pBox.Min.X += pTemp.X;
		pBox.Min.Y += pTemp.Y;
		pBox.Min.Z += pTemp.Z;
		pBox.Max.X += pTemp.X;
		pBox.Max.Y += pTemp.Y;
		pBox.Max.Z += pTemp.Z;

		// Gotta check to see if we're hitting a world model
		if(CCD->ModelManager()->DoesBoxHitModel(pTemp, pBox, &pModel) == GE_TRUE)
			return false;					// Sorry, you can't be here...

		// Now do the same for managed actors
		if(CCD->ActorManager()->DoesBoxHitActor(pTemp, pBox, &pActor) == GE_TRUE)
			return false;					// Ugh, we hit an actor instead

		return true;						// No contents, so it's OK to be there.
	}

	//	Well, we have contents.  Now we don't care if the contents happen
	//	..to be something we're aware of, so let's check mainly for
	//	..ACTORS and MODELS.
	if((Contents.Mesh != NULL) || (Contents.Model != NULL) || (Contents.Actor != NULL))
		return false;						// Something there we don't like.

	//	Finally, check to see if the contents is marked IMPENETRABLE in the Type.
	if((Contents.Contents & 0xffff0000) == Impenetrable)
		return false;						// Can't exist in this one..

	//	All else being equal, I guess it's OK to be here...
	return true;
}

/* ------------------------------------------------------------------------------------ */
//	CanOccupyPosition checks to see if a specific zone in the level is
//	..occupiable (??) by something of the specified size 'theBox'.
//	..This version takes an ACTOR as input, as well...
/* ------------------------------------------------------------------------------------ */
bool Collider::CanOccupyPosition(const geVec3d *thePoint, geExtBox *theBox,
								 geActor *Actor)
{
	GE_Contents Contents;

	memset(&Contents, 0, sizeof(GE_Contents));


	int Result = geWorld_GetContents(CCD->World(),
		thePoint, &theBox->Min, &theBox->Max, GE_COLLIDE_ACTORS,
		0xffffffff, CBExclusion, Actor, &Contents);

// changed QD 01/2004
	geVec3d temp;
	geVec3d_Add(thePoint, &theBox->Max, &temp);

	if((Result == GE_FALSE) &&CCD->Meshes()->CollisionCheck(&theBox->Min, &theBox->Max, *thePoint, temp, NULL))
	{
		Contents.Mesh=(geMesh*)CCD->Meshes();
		Result=GE_TRUE;
	}
// end change

	if((Result == GE_FALSE) && (geWorld_GetContents(CCD->World(),
		thePoint, &theBox->Min, &theBox->Max, GE_COLLIDE_MODELS,
		0xffffffff, NULL, NULL, &Contents) == GE_TRUE))
	{
		Contents.Actor = NULL;

		if((Contents.Contents & (GE_CONTENTS_EMPTY | GE_CONTENTS_SOLID)) == GE_CONTENTS_EMPTY)
			Result = GE_FALSE;
		else if((Contents.Contents & GE_CONTENTS_HINT))
		{
			Result = GE_FALSE;
		}
// changed RF064
/*		else if((Contents.Contents & GE_CONTENTS_AREA))
		{
			Result = GE_FALSE;
		} */
// end change RF064
		else
		{
			Result = GE_TRUE;

			if(CCD->Doors()->IsADoor(Contents.Model))
				Result = GE_FALSE;
		}
	}


	if(Result == GE_FALSE)
	{
		geWorld_Model *pModel;
		geActor *pActor;
		geVec3d pTemp = *thePoint;
		geExtBox pBox = *theBox;
		pBox.Min.X += pTemp.X;
		pBox.Min.Y += pTemp.Y;
		pBox.Min.Z += pTemp.Z;
		pBox.Max.X += pTemp.X;
		pBox.Max.Y += pTemp.Y;
		pBox.Max.Z += pTemp.Z;

		// Gotta check to see if we're hitting a world model
		if(CCD->ModelManager()->DoesBoxHitModel(pTemp, pBox, &pModel) == GE_TRUE)
			return false;					// Sorry, you can't be here...

		// Now do the same for managed actors
		if(CCD->ActorManager()->DoesBoxHitActor(pTemp, pBox, &pActor, Actor) == GE_TRUE)
			return false;					// Ugh, we hit an actor instead

		return true;						// No contents, so it's OK to be there.
	}

	//	Well, we have contents.  Now we don't care if the contents happen
	//	..to be something we're aware of, so let's check mainly for
	//	..ACTORS and MODELS.
	if((Contents.Mesh != NULL) || (Contents.Model != NULL) || (Contents.Actor != NULL))
		return false;						// Something there we don't like.

	//	Finally, check to see if the contents is marked IMPENETRABLE in the Type.
	if((Contents.Contents & 0xffff0000) == Impenetrable)
		return false;						// Can't exist in this one..

	if(Result != GE_FALSE)
		return false;						// We hit SOMETHING, so it's not OK...

	//	All else being equal, I guess it's OK to be here...
	return true;
}

/* ------------------------------------------------------------------------------------ */
//	CanOccupyPosition checks to see if a specific zone in the level is
//	..occupiable (??) by something of the specified size 'theBox'.
//	..This version takes an ACTOR as input, along with a GE_Contents
//	..struct for information return.
/* ------------------------------------------------------------------------------------ */
bool Collider::CanOccupyPosition(const geVec3d *thePoint, geExtBox *theBox,
								 geActor *Actor, GE_Contents *Contents)
{
	memset(Contents, 0, sizeof(GE_Contents));

	int Result = geWorld_GetContents(CCD->World(),
		thePoint, &theBox->Min, &theBox->Max, GE_COLLIDE_ACTORS,
		0xffffffff, CBExclusion, Actor, Contents);

	int Result1 = geWorld_GetContents(CCD->World(),
		thePoint, &theBox->Min, &theBox->Max, GE_COLLIDE_MODELS,
		0xffffffff, NULL, NULL, Contents);

	if((Result == GE_FALSE) && (Result1 == GE_TRUE))
	{
		Contents->Actor = NULL;
		if((Contents->Contents & (GE_CONTENTS_EMPTY | GE_CONTENTS_SOLID)) == GE_CONTENTS_EMPTY)
			Result = GE_FALSE;
		else if((Contents->Contents & GE_CONTENTS_HINT))
		{
			Result = GE_FALSE;
		}
// changed RF064
/*		else if((Contents->Contents & GE_CONTENTS_AREA))
		{
			Result = GE_FALSE;
		} */
// end change RF064
		else
		{
			Result = GE_TRUE;
			if(CCD->Doors()->IsADoor(Contents->Model))
			//if((CCD->Doors()->IsADoor(Contents->Model) && CCD->ModelManager()->IsRunning(Contents->Model)))
				Result = GE_FALSE;
		}
	}
	else
		Contents->Model = NULL;

	if(Result == GE_FALSE)
	{
		geWorld_Model *pModel;
		geActor *pActor;
		geVec3d pTemp = *thePoint;
		geExtBox pBox = *theBox;
		pBox.Min.X += pTemp.X;
		pBox.Min.Y += pTemp.Y;
		pBox.Min.Z += pTemp.Z;
		pBox.Max.X += pTemp.X;
		pBox.Max.Y += pTemp.Y;
		pBox.Max.Z += pTemp.Z;
		Contents->Model = NULL;
		Contents->Actor = NULL;
		// Gotta check to see if we're hitting a world model
		if(CCD->ModelManager()->DoesBoxHitModel(pTemp, pBox, &pModel) == GE_TRUE)
		{
			Contents->Actor = NULL;
			Contents->Model = pModel;		// Model we hit
			return false;					// Sorry, you can't be here...
		}
		// Now do the same for managed actors
		if(CCD->ActorManager()->DoesBoxHitActor(pTemp, pBox, &pActor, Actor) == GE_TRUE)
		{
			Contents->Model = NULL;
			Contents->Actor = pActor;		// Actor we hit
			return false;					// Ugh, we hit an actor instead
		}
		return true;						// No contents, so it's OK to be there.
	}

	//	Well, we have contents.  Now we don't care if the contents happen
	//	..to be something we're aware of, so let's check mainly for
	//	..ACTORS and MODELS.

	if((Contents->Mesh != NULL) || (Contents->Model != NULL) || (Contents->Actor != NULL))
		return false;						// Something there we don't like.

	//	Finally, check to see if the contents is marked IMPENETRABLE in the Type.

	if((Contents->Contents & 0xffff0000) == Impenetrable)
		return false;						// Can't exist in this one..

	if(Result != GE_FALSE)
		return false;						// We hit SOMETHING, so it's not OK...

	//	All else being equal, I guess it's OK to be here...

	return true;
}

//	--------- PRIVATE METHODS TO SUPPORT COLLISION CHECKING ---------

/* ------------------------------------------------------------------------------------ */
//	CheckIntersection
//
//	Given an already translated actor, check to see if any of the
//	..bone axially-aligned bounding boxes intersect anything that
//	..they shouldn't.
/* ------------------------------------------------------------------------------------ */
geBoolean Collider::CheckIntersection(geActor *Actor, geWorld *World)
{
	int TotalBoneCount = 0;
	geExtBox theBoneBox;
	geBoolean Collided = GE_FALSE;
	GE_Contents Contents;

	TotalBoneCount = geBody_GetBoneCount(geActor_GetBody(geActor_GetActorDef(Actor)));

	if(TotalBoneCount <= 0)
		OutputDebugString("Zero bone count!\n");

	Collided = GE_FALSE;

	for(int BoneIndex=0; BoneIndex<TotalBoneCount; BoneIndex++)
	{
		// The bone bounding box comes back in worldspace coordinates...
		if(geActor_GetBoneExtBoxByIndex(Actor, BoneIndex, &theBoneBox) != GE_TRUE)
			continue;								// Nothing here, skip it

		geXForm3d theTransform;
		geActor_GetBoneTransformByIndex(Actor, BoneIndex, &theTransform);

		// Ok, convert from worldspace to modelspace for the bounding box
		theBoneBox.Min.X -= theTransform.Translation.X;
		theBoneBox.Min.Y -= theTransform.Translation.Y;
		theBoneBox.Min.Z -= theTransform.Translation.Z;
		theBoneBox.Max.X -= theTransform.Translation.X;
		theBoneBox.Max.Y -= theTransform.Translation.Y;
		theBoneBox.Max.Z -= theTransform.Translation.Z;

		// Inflate the bounding box slightly
		theBoneBox.Min.X -= 2.0f;
		theBoneBox.Min.Y -= 2.0f;
		theBoneBox.Min.Z -= 2.0f;
		theBoneBox.Max.X += 2.0f;
		theBoneBox.Max.Y += 2.0f;
		theBoneBox.Max.Z += 2.0f;

		geVec3d theFoo;
		theFoo.X = theTransform.Translation.X;
		theFoo.Y = theTransform.Translation.Y;
		theFoo.Z = theTransform.Translation.Z;

		if(CCD->MenuManager()->GetBoundBox())
			DrawBoundBox(World, &theFoo, &theBoneBox.Min, &theBoneBox.Max);

		memset(&Contents, 0, sizeof(GE_Contents));
		geWorld_GetContents(World, &theFoo, &theBoneBox.Min, &theBoneBox.Max,
			GE_VISIBLE_CONTENTS | GE_COLLIDE_ACTORS | GE_COLLIDE_MODELS, 0xffffffff, NULL, NULL, &Contents);

		if(Contents.Actor != NULL)
			Collided = GE_TRUE;
		if(Contents.Mesh != NULL)
			Collided = GE_TRUE;
		if(Contents.Model != NULL)
			Collided = GE_TRUE;
		if((Contents.Contents & 0xffff0000) == Impenetrable)
			Collided = GE_TRUE;
		if(((Contents.Contents & (GE_CONTENTS_EMPTY | GE_CONTENTS_SOLID)) == GE_CONTENTS_EMPTY) &&
			(Contents.Actor == NULL))
			Collided = GE_FALSE;

		// Ok, if we hit ANYTHING, we can take the early exit.
		if(Collided == GE_TRUE)
			break;
	}

	return Collided;
}

/* ------------------------------------------------------------------------------------ */
//	CheckIntersection
//
//	Given an actor and a target location, check to see if any of the
//	..bone axially-aligned bounding boxes will intersect anything that
//	..they shouldn't.
/* ------------------------------------------------------------------------------------ */
geBoolean Collider::CheckIntersection(const geVec3d *Position, geActor *Actor, geWorld *World)
{
	int TotalBoneCount = 0;
	geExtBox theBoneBox;
	geBoolean Collided = GE_FALSE;
	GE_Contents Contents;

	TotalBoneCount = geBody_GetBoneCount(geActor_GetBody(geActor_GetActorDef(Actor)));

	if(TotalBoneCount <= 0)
		OutputDebugString("Zero bone count!\n");

	Collided = GE_FALSE;

	for(int BoneIndex=0; BoneIndex<TotalBoneCount; BoneIndex++)
	{
		// The bone bounding box comes back in worldspace coordinates...
		if(geActor_GetBoneExtBoxByIndex(Actor, BoneIndex, &theBoneBox) != GE_TRUE)
			continue;								// Nothing here, skip it

		geXForm3d theTransform;
		geActor_GetBoneTransformByIndex(Actor, BoneIndex, &theTransform);

		// Ok, convert from worldspace to modelspace for the bounding box
		theBoneBox.Min.X -= theTransform.Translation.X;
		theBoneBox.Min.Y -= theTransform.Translation.Y;
		theBoneBox.Min.Z -= theTransform.Translation.Z;
		theBoneBox.Max.X -= theTransform.Translation.X;
		theBoneBox.Max.Y -= theTransform.Translation.Y;
		theBoneBox.Max.Z -= theTransform.Translation.Z;

		// Inflate the bounding box slightly
		theBoneBox.Min.X -= 2.0f;
		theBoneBox.Min.Y -= 2.0f;
		theBoneBox.Min.Z -= 2.0f;
		theBoneBox.Max.X += 2.0f;
		theBoneBox.Max.Y += 2.0f;
		theBoneBox.Max.Z += 2.0f;

		memset(&Contents, 0, sizeof(GE_Contents));
		int Result = CanOccupyPosition(&(theTransform.Translation), &theBoneBox, Actor, &Contents);

		if(Result == GE_FALSE)
			Collided = GE_TRUE;

		// Ok, if we hit ANYTHING, we can take the early exit.
		if(Collided == GE_TRUE)
			break;
	}

	return Collided;
}

/* ------------------------------------------------------------------------------------ */
//	CheckSubCollision
//
//	Given an actor, a start, and a target location, check to see if any of the
//	..bone axially-aligned bounding boxes will intersect anything that
//	..they shouldn't.
/* ------------------------------------------------------------------------------------ */
geBoolean Collider::CheckSubCollision(const geVec3d *Start, const geVec3d *End, geActor *Actor,
									  geWorld *World)
{
	int TotalBoneCount = 0;
	geExtBox theBoneBox;
	geBoolean Collided = GE_FALSE;
	GE_Collision Collision;

	TotalBoneCount = geActor_GetBoneCount(Actor);

	if(TotalBoneCount <= 0)
		OutputDebugString("Zero bone count!\n");

	Collided = GE_FALSE;

	//	Make local copies of the start and end positions
	geVec3d xStart, xEnd, lStart, lEnd;
	lStart = *Start;
	lEnd =  *End;

	for(int BoneIndex=0; BoneIndex<TotalBoneCount; BoneIndex++)
	{
		// The bone bounding box comes back in worldspace coordinates...
		if(geActor_GetBoneExtBoxByIndex(Actor, BoneIndex, &theBoneBox) != GE_TRUE)
			continue;								// Nothing here, skip it

		geXForm3d theTransform;
		geActor_GetBoneTransformByIndex(Actor, BoneIndex, &theTransform);

		// Ok, convert from worldspace to modelspace for the bounding box
		theBoneBox.Min.X -= theTransform.Translation.X;
		theBoneBox.Min.Y -= theTransform.Translation.Y;
		theBoneBox.Min.Z -= theTransform.Translation.Z;
		theBoneBox.Max.X -= theTransform.Translation.X;
		theBoneBox.Max.Y -= theTransform.Translation.Y;
		theBoneBox.Max.Z -= theTransform.Translation.Z;

		// Inflate the bounding box slightly
		theBoneBox.Min.X -= 2.0f;
		theBoneBox.Min.Y -= 2.0f;
		theBoneBox.Min.Z -= 2.0f;
		theBoneBox.Max.X += 2.0f;
		theBoneBox.Max.Y += 2.0f;
		theBoneBox.Max.Z += 2.0f;

		// Compute the delta between our start position and the current
		// ..bone translation.  We're assuming (heh) that the start is
		// ..the CURRENT POSITION of the root bone of the actor.
		xStart = lStart;
		xEnd = lEnd;
		geVec3d gTemp = lStart;
		gTemp.X -= theTransform.Translation.X;
		gTemp.Y -= theTransform.Translation.Y;
		gTemp.Z -= theTransform.Translation.Z;

		xStart.X -= gTemp.X; xStart.Y -= gTemp.Y; xStart.Z -= gTemp.Z;
		xEnd.X   -= gTemp.X; xEnd.Y   -= gTemp.Y; xEnd.Z   -= gTemp.Z;

		memset(&Collision, 0, sizeof(GE_Collision));

		Collided = geWorld_Collision(World, &theBoneBox.Min,
			&theBoneBox.Max, &xStart, &xEnd, kCollideFlags,
			GE_COLLIDE_ACTORS, 0xffffffff, CBExclusion, Actor, &Collision);

		if(Collided == GE_TRUE)
			break;					// Take an early dump...

		if(geWorld_Collision(World, &theBoneBox.Min,
			&theBoneBox.Max, &xStart, &xEnd, kCollideFlags,
			GE_COLLIDE_MODELS, 0x0, NULL, NULL, &Collision) == GE_TRUE)
			Collided = GE_TRUE;

		if(Collided == GE_TRUE)
			break;					// hit a model, let's bail
	}

	return Collided;
}

/* ------------------------------------------------------------------------------------ */
//	CheckSubCollision
//
//	Given an actor, a start, and a target location, check to see if any of the
//	..bone axially-aligned bounding boxes will intersect anything that
//	..they shouldn't.
/* ------------------------------------------------------------------------------------ */
geBoolean Collider::CheckSubCollision(const geVec3d *Start, const geVec3d *End, geActor *Actor,
									  geWorld *World, GE_Collision *Collision)
{
	int TotalBoneCount = 0;
	geExtBox theBoneBox;
	geBoolean Collided = GE_FALSE;

	TotalBoneCount = geBody_GetBoneCount(geActor_GetBody(geActor_GetActorDef(Actor)));

	if(TotalBoneCount <= 0)
		OutputDebugString("Zero bone count!\n");

	Collided = GE_FALSE;

	//	Make local copies of the start and end positions
	geVec3d xStart, xEnd, lStart, lEnd;
	memcpy(&lStart, Start, sizeof(geVec3d));
	memcpy(&lEnd, End, sizeof(geVec3d));

	for(int BoneIndex=0; BoneIndex<TotalBoneCount; BoneIndex++)
	{
		// The bone bounding box comes back in worldspace coordinates...
		if(geActor_GetBoneExtBoxByIndex(Actor, BoneIndex, &theBoneBox) != GE_TRUE)
			continue;								// Nothing here, skip it

		geXForm3d theTransform;
		geActor_GetBoneTransformByIndex(Actor, BoneIndex, &theTransform);

		// Ok, convert from worldspace to modelspace for the bounding box
		theBoneBox.Min.X -= theTransform.Translation.X;
		theBoneBox.Min.Y -= theTransform.Translation.Y;
		theBoneBox.Min.Z -= theTransform.Translation.Z;
		theBoneBox.Max.X -= theTransform.Translation.X;
		theBoneBox.Max.Y -= theTransform.Translation.Y;
		theBoneBox.Max.Z -= theTransform.Translation.Z;

		// Inflate the bounding box slightly
		theBoneBox.Min.X -= 2.0f;
		theBoneBox.Min.Y -= 2.0f;
		theBoneBox.Min.Z -= 2.0f;
		theBoneBox.Max.X += 2.0f;
		theBoneBox.Max.Y += 2.0f;
		theBoneBox.Max.Z += 2.0f;

		// Compute the delta between our start position and the current
		// ..bone translation.  We're assuming (heh) that the start is
		// ..the CURRENT POSITION of the root bone of the actor.
		xStart = lStart;
		xEnd = lEnd;
		geVec3d gTemp;
		memcpy(&gTemp, Start, sizeof(geVec3d));
		gTemp.X -= theTransform.Translation.X;
		gTemp.Y -= theTransform.Translation.Y;
		gTemp.Z -= theTransform.Translation.Z;

		xStart.X -= gTemp.X; xStart.Y -= gTemp.Y; xStart.Z -= gTemp.Z;
		xEnd.X   -= gTemp.X; xEnd.Y   -= gTemp.Y; xEnd.Z   -= gTemp.Z;

		memset(Collision, 0, sizeof(GE_Collision));

		Collided = geWorld_Collision(World, &theBoneBox.Min,
			&theBoneBox.Max, &xStart, &xEnd, kCollideFlags,
			GE_COLLIDE_ACTORS, 0xffffffff, CBExclusion, Actor, Collision);

		if((Collision->Actor != NULL) || (Collision->Model != NULL) ||
			(Collision->Mesh != NULL))
			Collided = GE_TRUE;

		if(geWorld_Collision(World, &theBoneBox.Min,
			&theBoneBox.Max, &xStart, &xEnd, kCollideFlags,
			GE_COLLIDE_MODELS, 0x0, NULL, NULL, Collision) == GE_TRUE)
		{
			Collided = GE_TRUE;
			Collision->Actor = NULL;
		}

		// Ok, if we hit ANYTHING, we can take the early exit.
		if(Collided == GE_TRUE)
			break;
	}

	return Collided;
}

/* ------------------------------------------------------------------------------------ */
//	CheckBoneCollision
//
//	Given two actors, one that moved and one that is static, go through
//	..all the bones in both actors to determine which bone on the moving
//	..actor hit which bone on the stationary actor, if any.
/* ------------------------------------------------------------------------------------ */
geBoolean Collider::CheckSubCollision(geWorld *World, geActor *Actor, geActor *StaticActor,
									  int *MovingBoneHit, int *StaticBoneHit)
{
	int TotalBoneCount = 0;
	int TotalStaticBoneCount = 0;
	geExtBox theBoneBox, theStaticBoneBox, Temp;

	TotalBoneCount = geActor_GetBoneCount(Actor);

	if(TotalBoneCount <= 0)
		OutputDebugString("Zero bone count!\n");

	TotalStaticBoneCount = geActor_GetBoneCount(StaticActor);

	if(TotalStaticBoneCount <= 0)
		OutputDebugString("Zero static bones!\n");

	//	Ok, for EACH static bone in the non-moving actor check the
	//	..motion of EACH bone in the moving actor and see if it hit.
	for(int nStatic=0; nStatic<TotalStaticBoneCount; nStatic++)
	{
		// The bone bounding box comes back in worldspace coordinates...
		if(geActor_GetBoneExtBoxByIndex(StaticActor, nStatic, &theStaticBoneBox) != GE_TRUE)
			continue;								// Nothing here, skip it

		// We have the worldspace extbox for the static bone.  Go through
		// ..each and every bone in the moving actor and check to see if
		// ..its extbox overlaps ours.
		for(int BoneIndex=0; BoneIndex<TotalBoneCount; BoneIndex++)
		{
			// The bone bounding box comes back in worldspace coordinates...
			if(geActor_GetBoneExtBoxByIndex(Actor, BoneIndex, &theBoneBox) != GE_TRUE)
				continue;								// Nothing here, skip it

			if(geExtBox_Intersection(&theBoneBox, &theStaticBoneBox, &Temp) == GE_TRUE)
			{
				// They hit!  Return the colliding bone indices
				*StaticBoneHit = nStatic;
				*MovingBoneHit = BoneIndex;
				return GE_TRUE;					// We hit!
			}
		}
	}

	return GE_FALSE;							// No bones intersected
}

#define		VectorToSUB(a, b) ( *(((geFloat*)&a) + b) )

/* ------------------------------------------------------------------------------------ */
//	MoveBox
//	Creates a box around the entire move
/* ------------------------------------------------------------------------------------ */
void Collider::GetMoveBox(const geVec3d *Mins, const geVec3d *Maxs,
						  const geVec3d *Front, const geVec3d *Back,
						  geVec3d *OMins, geVec3d *OMaxs)
{
	int32		i;

	for (i=0 ; i<3 ; i++)
	{
		if (VectorToSUB(*Back, i) > VectorToSUB(*Front, i))
		{
			VectorToSUB(*OMins, i) = VectorToSUB(*Mins, i) - 1.0f;
			VectorToSUB(*OMaxs, i) = VectorToSUB(*Back, i) + VectorToSUB(*Maxs, i) + 1.0f - VectorToSUB(*Front, i);
		}
		else
		{
			VectorToSUB(*OMins, i) = VectorToSUB(*Back, i) + VectorToSUB(*Mins, i) - 1.0f - VectorToSUB(*Front, i);
			VectorToSUB(*OMaxs, i) = VectorToSUB(*Maxs, i) + 1.0f;
		}
	}
}

/* ------------------------------------------------------------------------------------ */
//	CheckBoneCollision
/* ------------------------------------------------------------------------------------ */
geBoolean Collider::CheckBoneCollision(geWorld *World, geActor *StaticActor,
									   const geVec3d *Position, geVec3d *Min, geVec3d *Max)
{
	int TotalStaticBoneCount = 0;
	geExtBox theBox, theStaticBoneBox, Temp;

	DrawBoundBox(CCD->World(), Position, Min, Max);

	theBox.Min.X = Min->X+Position->X;
	theBox.Min.Y = Min->Y+Position->Y;
	theBox.Min.Z = Min->Z+Position->Z;
	theBox.Max.X = Max->X+Position->X;
	theBox.Max.Y = Max->Y+Position->Y;
	theBox.Max.Z = Max->Z+Position->Z;

	TotalStaticBoneCount = geActor_GetBoneCount(StaticActor);

	if(TotalStaticBoneCount <= 0)
		OutputDebugString("Zero static bones!\n");

	//	Ok, for EACH static bone in the non-moving actor check the
	//	..motion of EACH bone in the moving actor and see if it hit.
	for(int nStatic=0; nStatic<TotalStaticBoneCount; nStatic++)
	{
		// The bone bounding box comes back in worldspace coordinates...
		if(geActor_GetBoneExtBoxByIndex(StaticActor, nStatic, &theStaticBoneBox) != GE_TRUE)
			continue;								// Nothing here, skip it

		geXForm3d theTransform;
		geActor_GetBoneTransformByIndex(StaticActor, nStatic, &theTransform);

		// Ok, convert from worldspace to modelspace for the bounding box
		theStaticBoneBox.Min.X -= theTransform.Translation.X;
		theStaticBoneBox.Min.Y -= theTransform.Translation.Y;
		theStaticBoneBox.Min.Z -= theTransform.Translation.Z;
		theStaticBoneBox.Max.X -= theTransform.Translation.X;
		theStaticBoneBox.Max.Y -= theTransform.Translation.Y;
		theStaticBoneBox.Max.Z -= theTransform.Translation.Z;

		DrawBoundBox(CCD->World(), &theTransform.Translation, &theStaticBoneBox.Min, &theStaticBoneBox.Max);

		theStaticBoneBox.Min.X += theTransform.Translation.X;
		theStaticBoneBox.Min.Y += theTransform.Translation.Y;
		theStaticBoneBox.Min.Z += theTransform.Translation.Z;
		theStaticBoneBox.Max.X += theTransform.Translation.X;
		theStaticBoneBox.Max.Y += theTransform.Translation.Y;
		theStaticBoneBox.Max.Z += theTransform.Translation.Z;

		if(geExtBox_Intersection(&theBox, &theStaticBoneBox, &Temp) == GE_TRUE)
		{
			return GE_TRUE;
		}
		//if(theBox.Min.X>theStaticBoneBox.Max.X || theBox.Max.X<theStaticBoneBox.Min.X) continue;
		//if(theBox.Min.Y>theStaticBoneBox.Max.Y || theBox.Max.Y<theStaticBoneBox.Min.Y) continue;
		//if(theBox.Min.Z>theStaticBoneBox.Max.Z || theBox.Max.Z<theStaticBoneBox.Min.Z) continue;
		//return GE_TRUE;
	}

	return GE_FALSE;							// No bones intersected
}

/* ------------------------------------------------------------------------------------ */
//	Probe
//
//	Given a transform, probe forward/backward from it some distance and
//	..return collision information.  Useful for pre-checking AI motion.
/* ------------------------------------------------------------------------------------ */
geBoolean Collider::Probe(const geXForm3d &theXForm, float fDistance, GE_Collision *theCollision)
{
	geVec3d Start, End;
	geVec3d InVector;

	Start = theXForm.Translation;							// Start point

	geXForm3d_GetIn(&theXForm, &InVector);		// In & out

	//	Compute the endpoint
	geVec3d_AddScaled(&Start, &InVector, fDistance, &End);

	//	Now do a ray collision test to "probe" the space
	geBoolean ItHit = geWorld_Collision(CCD->World(), NULL, NULL,
		&Start, &End, kCollideFlags, GE_COLLIDE_ALL,
		0xffffffff, NULL, NULL, theCollision);

// changed QD 01/2004
	if(ItHit == GE_FALSE)
		ItHit = CCD->Meshes()->CollisionCheck(NULL, NULL, Start, End, theCollision);
// end change

	return ItHit;					// Return collision status
}

/* ------------------------------------------------------------------------------------ */
//	Probe
//
//	Given a transform, probe forward/backward from it some distance and
//	..return collision information.  Useful for pre-checking AI motion.
//	This flavor takes an Actor pointer to exclude from the checking.
/* ------------------------------------------------------------------------------------ */
geBoolean Collider::Probe(const geXForm3d &theXForm, float fDistance, GE_Collision *theCollision,
						  geActor *theActor)
{
	geVec3d Start, End;
	geVec3d InVector;

	Start = theXForm.Translation;				// Start point

	geXForm3d_GetIn(&theXForm, &InVector);		// In & out

	//	Compute the endpoint
	geVec3d_AddScaled(&Start, &InVector, fDistance, &End);

	//	Now do a ray collision test to "probe" the space
	geBoolean ItHit = geWorld_Collision(CCD->World(), NULL, NULL,
		&Start, &End, kCollideFlags, GE_COLLIDE_ALL,
		0xffffffff, CBExclusion, theActor, theCollision);

// changed QD 01/2004
	if(ItHit == GE_FALSE)
		ItHit=CCD->Meshes()->CollisionCheck(NULL, NULL, Start, End, theCollision);
// end change

	return ItHit;					// Return collision status
}

/* ------------------------------------------------------------------------------------ */
//	Trace
/* ------------------------------------------------------------------------------------ */
TraceData Collider::Trace(geVec3d *Start, geVec3d *End, geVec3d *Min, geVec3d *Max,
							geActor *Actor, GE_Collision *Collision)
{
	TraceData Data;
	geExtBox theBox;

	Data.fraction = 1.0f;
	Data.endpos = *End;
	Data.allsolid = false;
	Data.startsolid = false;

	if(!Min || !Max)
	{
		theBox.Min.X = theBox.Min.Y = theBox.Min.Z = -0.1f;
		theBox.Max.X = theBox.Max.Y = theBox.Max.Z = 0.1f;
	}
	else
	{
		theBox.Min = *Min;
		theBox.Max = *Max;
	}

	if(!CanOccupyPosition(Start, &theBox, Actor))
	{
		Data.fraction = 0.0f;
		Data.startsolid = true;

		if(!CanOccupyPosition(End, &theBox, Actor))
			Data.allsolid = true;

		return Data;
	}

	bool ItHit = geWorld_Collision(CCD->World(), Min, Max,
		Start, End, kCollideFlags,
		//GE_COLLIDE_ALL, 0xffffffff, CBExclusion, Actor, Collision);
		GE_COLLIDE_MODELS, 0x0, NULL, NULL, Collision);

	if(ItHit)
	{
		Data.endpos = Collision->Impact;
		float maxdist = geVec3d_DistanceBetween(Start, End);
		float actdist = geVec3d_DistanceBetween(Start, &Collision->Impact);
		Data.fraction = actdist/maxdist;
	}

	return Data;
}

/* ------------------------------------------------------------------------------------ */
//	ProcessCollision
//
//	If we have a collision, we need to dispatch that fact to
//	..the handler for that entity type and let it figure out
//	..which entity just got smacked.
/* ------------------------------------------------------------------------------------ */
// Weapon
int Collider::ProcessCollision(const GE_Collision &theCollision, geActor *theActor, bool Gravity)
{
	bool bShoot = false;
	int aType = ENTITY_GENERIC;

	if(!theActor)
		bShoot = true;
	else
	{
		CCD->ActorManager()->GetType(theActor, &aType);

		if(aType == ENTITY_PROJECTILE)
			bShoot = true;
	}

	if(theCollision.Model != NULL)
	{
		// We hit a world model.  Check each kind of world model that
		// ..we know about.
		if(!bShoot)
		{
			if(theActor == CCD->Player()->GetActor())
			{
				if(CCD->Changelevel()->CheckChangeLevel(theCollision.Model, false))// && !bShoot)
				{
					CCD->SetChangeLevel(true);			// We hit a change level
					return kCollideWorldModel;
				}
			}
		}

		// ..all the other possibilities now.
		if(CCD->Doors()->HandleCollision(theCollision.Model, bShoot, false, theActor))
		{
			CCD->ModelManager()->HandleCollision(theCollision.Model, theActor);
			return kCollideDoor; // Hit, and processed
		}

		if(CCD->Platforms()->HandleCollision(theCollision.Model, bShoot, false, theActor))
		{
			CCD->ModelManager()->HandleCollision(theCollision.Model, theActor);
			return kCollidePlatform;						// Hit, and processed
		}

		int Result = CCD->Triggers()->HandleCollision(theCollision.Model, bShoot, false, theActor);

		CCD->ModelManager()->HandleCollision(theCollision.Model, theActor);

		if(Result == RGF_SUCCESS)
			return kCollideWorldModel;

		if(Result == RGF_EMPTY)
			return kNoCollision;

		if(CCD->PathFollower()->HandleCollision(theCollision.Model))
			return kCollideWorldModel;	// Hit, and processed

		// *NOTE* Add new world-model collision handlers here.
		if(!bShoot || (bShoot && aType == ENTITY_PROJECTILE))
		{
			if(CCD->Teleporters()->HandleCollision(theCollision.Model, theActor))
				return kCollideTrigger;	// Hit, and processed

			int Result1 = CCD->ModelManager()->HandleCollision(theCollision.Model, theActor);

			if(Result1 == RGF_SUCCESS)
				return kCollideWorldModel;					// Hit, and processed

			if(Result1 == RGF_EMPTY)
				return kNoCollision;
		}

		return kCollideNoMove;								// Model hit, but not dealt with
	}

	//	Ok, check to see if we hit an actor!
	if(theCollision.Actor != NULL)
	{
		// Hit an actor!  Act on it..heh heh...
		int nType;
		int result;

		if(CCD->ActorManager()->GetType(theCollision.Actor, &nType)==RGF_NOT_FOUND)
			return kNoCollision;

		switch(nType)
		{
		case ENTITY_GENERIC:
			return kCollideActor;
			break;
		case ENTITY_ACTOR: // Player
			return kCollideActor;
			break;
		case ENTITY_NPC:  // NPC
			result = CCD->Pawns()->HandleCollision(theCollision.Actor, theActor, Gravity);
			if(result==RGF_SUCCESS)
				return kCollideNPC;
			if(result==RGF_RECHECK)
				return kNoCollision;
			break;
		case ENTITY_VEHICLE: // Vehicle
			result = CCD->Pawns()->HandleCollision(theCollision.Actor, theActor, Gravity);
			if(result==RGF_SUCCESS)
				return kCollideVehicle;
			if(result==RGF_RECHECK)
				return kNoCollision;
			break;
		case ENTITY_PROP: // StaticEntityProxy
			result = CCD->Props()->HandleCollision(theCollision.Actor, theActor, Gravity, false);
			if(result==RGF_SUCCESS)
				return kCollideActor;
			if(result==RGF_RECHECK)
				return kNoCollision;
			break;
		case ENTITY_VIEWEAPON: // 1st person weapon
			return kNoCollision;
			break;
		case ENTITY_PROJECTILE: // Projectile
			return kCollideActor;
			break;
		case ENTITY_ATTRIBUTE_MOD: // Attribute
			if(!bShoot)
// changed RF063
// changed QD 08/13/03
				CCD->Attributes()->HandleCollision(theActor, theCollision.Actor, false);
			return kCollideActor;//kNoCollision;
// end change 08/13/03
			break;
		default:
			return kNoCollision;
			break;
		}
	}
// changed QD 01/2004
	if(theCollision.Mesh != NULL)
		return kCollideMesh;
// end change

	return kNoCollision;
}

/* ------------------------------------------------------------------------------------ */
//	Debug Routines
/* ------------------------------------------------------------------------------------ */
bool Collider::CheckActorCollisionD(const geVec3d &Start, const geVec3d &End,
									geActor *theActor, GE_Collision *Collision)
{
	//	First, check to see if the bounding box will hit, and if not, we
	//	..need not go to a Stage II collision test

	geExtBox theBox;

	geActor_GetExtBox(theActor, &theBox);

	theBox.Min.X -= Start.X;
	theBox.Min.Y -= Start.Y;
	theBox.Min.Z -= Start.Z;
	theBox.Max.X -= Start.X;
	theBox.Max.Y -= Start.Y;
	theBox.Max.Z -= Start.Z;

	if(CCD->MenuManager()->GetBoundBox())
		DrawBoundBox(CCD->World(), &End, &theBox.Min, &theBox.Max);

	Flag2 = 0;
	Flag3 = 0;

	if((CheckForCollisionD(&theBox.Min, &theBox.Max, Start, End, Collision, theActor)) == false)
	{
		if(!(m_CheckLevel <= RGF_COLLISIONLEVEL_1))
		{
			if(CheckIntersection(theActor, CCD->World()))
			{
				return true;
			}
		}

		return false;	// Cleared first hurdle
	}
/*
	//	Ok, sometimes we don't care about anything BUT a bounding-box check,
	//	..so at this point we'll see if that's all we want
	if(m_CheckLevel <= RGF_COLLISIONLEVEL_1)
		return true;							// AABB impact is good enough

	if(!m_IgnoreContents)
	{
		if(CheckIntersection(&Start, theActor, CCD->World()))
			return true;
		if(CheckIntersection(&End, theActor, CCD->World()))
			return true;
	}
*/
	//	AABB + contents isn't good enough, try an AABB check PER BONE

	if(CheckSubCollision(&Start, &End, theActor, CCD->World()) == GE_TRUE)
		return true;
	else
		return false;							// We'll clear everything
}

/* ------------------------------------------------------------------------------------ */
//	CheckForCollisionD
/* ------------------------------------------------------------------------------------ */
bool Collider::CheckForCollisionD(geVec3d *Min, geVec3d *Max,
								  const geVec3d &OldPosition, const geVec3d &NewPosition,
								  GE_Collision *Collision, geActor *Actor)
{
	GE_Contents Contents;
	int Result, Result2;
	geVec3d OMins, OMaxs;

	GetMoveBox(Min, Max, &OldPosition, &NewPosition, &OMins, &OMaxs);

	memset(Collision, 0, sizeof(GE_Collision));

	Result = geWorld_Collision(CCD->World(), Min,
		Max, &OldPosition, &NewPosition, kCollideFlags,
		GE_COLLIDE_ACTORS, 0xffffffff, CBExclusion, Actor, Collision);

	Collision->Model = NULL;
	int ColDetLevel;

	if(Result && CCD->ActorManager()->GetColDetLevel(Collision->Actor,&ColDetLevel)==RGF_SUCCESS)
	{
		if(ColDetLevel>RGF_COLLISIONLEVEL_1)
		{
			if(CheckBoneCollision(CCD->World(), Collision->Actor, &OldPosition, &OMins, &OMaxs)==GE_FALSE)
				Result = GE_FALSE;
		}
	}

	if(Result)
		return true;

	if((Result == GE_FALSE) && (geWorld_Collision(CCD->World(), Min,
		Max, &OldPosition, &NewPosition, kCollideFlags,
		GE_COLLIDE_MODELS, 0x0, NULL, NULL, Collision) == GE_TRUE))
	{
		Collision->Actor = NULL;
		Result = GE_TRUE;
	}

	if((Min != NULL) && (Max != NULL) && (Result != GE_TRUE) && (!m_IgnoreContents))
	{
		geExtBox theBBox;
		theBBox.Min = OMins;
		theBBox.Max = OMaxs;
		Result2 = CanOccupyPositionD(&OldPosition, &theBBox, Actor, &Contents);

		if(!Result2)
		{
			// Fill collision struct with information
			Collision->Mesh = Contents.Mesh;
			Collision->Model = Contents.Model;
			Collision->Actor = Contents.Actor;
			Collision->Impact = OldPosition;
			Result2 = GE_TRUE;
		}
		else
		{
			theBBox.Min = *Min;
			theBBox.Max = *Max;
			Result2 = CanOccupyPositionD(&NewPosition, &theBBox, Actor, &Contents);

			if(!Result2)
			{
				// Fill collision struct with information
				Collision->Mesh = Contents.Mesh;
				Collision->Model = Contents.Model;
				Collision->Actor = Contents.Actor;
				Collision->Impact = OldPosition;
				Result2 = GE_TRUE;
			}
			else
				Result2 = GE_FALSE;
		}
	}
	else
		Result2 = GE_FALSE;							// No contents checking

	//	Set collision reason
	LastCollisionReason = RGF_NO_COLLISION;				// Assume no hit

	if(Result == GE_TRUE)
		LastCollisionReason |= RGF_COLLIDE_AABB;		// Bounding-box hit

	if(Result2 == GE_TRUE)
	{
		if(Contents.Actor != NULL)
			LastCollisionReason |= RGF_COLLIDE_ACTOR;	// Hit an actor

		if(Contents.Model != NULL)
			LastCollisionReason |= RGF_COLLIDE_MODEL;	// Hit a model

		if(Contents.Mesh != NULL)
			LastCollisionReason |= RGF_COLLIDE_MESH;	// Hit a mesh
	}

	if((Result == GE_TRUE) || (Result2 == GE_TRUE))
	{
		return true;							// Collision confirmed
	}
	else
		return false;							// Naah, we didn't hit anything
}

/* ------------------------------------------------------------------------------------ */
//	CanOccupyPositionD
/* ------------------------------------------------------------------------------------ */
bool Collider::CanOccupyPositionD(const geVec3d *thePoint, geExtBox *theBox,
								 geActor *Actor, GE_Contents *Contents)
{
	memset(Contents, 0, sizeof(GE_Contents));

	int Result = geWorld_GetContents(CCD->World(),
		thePoint, &theBox->Min, &theBox->Max, GE_COLLIDE_ACTORS,
		0xffffffff, CBExclusion, Actor, Contents);

	Contents->Model = NULL;
	int ColDetLevel;

	if(Result && CCD->ActorManager()->GetColDetLevel(Contents->Actor,&ColDetLevel)==RGF_SUCCESS)
	{
		if(ColDetLevel>RGF_COLLISIONLEVEL_1)
		{
			if(CheckBoneCollision(CCD->World(), Contents->Actor, thePoint, &theBox->Min, &theBox->Max)==GE_FALSE)
				Result = GE_FALSE;
		}
	}

	if(Result)
		return false;

	int Result1 = geWorld_GetContents(CCD->World(),
		thePoint, &theBox->Min, &theBox->Max, GE_COLLIDE_MODELS,
		0xffffffff, NULL, NULL, Contents);

	Flag2 = Contents->Contents & GE_CONTENTS_HINT;

	if((Result == GE_FALSE) && (Result1 == GE_TRUE))
	{
		Contents->Actor = NULL;

		if((Contents->Contents & (GE_CONTENTS_EMPTY | GE_CONTENTS_SOLID)) == GE_CONTENTS_EMPTY)
			Result = GE_FALSE;
		else if((Contents->Contents & GE_CONTENTS_HINT))
		{
			Result = GE_FALSE;
		}
// changed RF064
/*		else if((Contents->Contents & GE_CONTENTS_AREA))
		{
			Result = GE_FALSE;
		} */
// end change RF064
		else
		{
			Result = GE_TRUE;

			if(CCD->Doors()->IsADoor(Contents->Model))
				Result = GE_FALSE;
		}
	}

	if(Result == GE_FALSE)
	{
		geWorld_Model *pModel;
		geActor *pActor;
		geVec3d pTemp = *thePoint;
		geExtBox pBox = *theBox;
		pBox.Min.X += pTemp.X;
		pBox.Min.Y += pTemp.Y;
		pBox.Min.Z += pTemp.Z;
		pBox.Max.X += pTemp.X;
		pBox.Max.Y += pTemp.Y;
		pBox.Max.Z += pTemp.Z;
		Contents->Model = NULL;
		Contents->Actor = NULL;

		// Gotta check to see if we're hitting a world model
		if(CCD->ModelManager()->DoesBoxHitModel(pTemp, pBox, &pModel) == GE_TRUE)
		{
			Contents->Model = pModel;		// Model we hit
			return false;					// Sorry, you can't be here...
		}

		// Now do the same for managed actors
		if(CCD->ActorManager()->DoesBoxHitActor(pTemp, pBox, &pActor, Actor) == GE_TRUE)
		{
			int ColDetLevel;

			if(CCD->ActorManager()->GetColDetLevel(pActor,&ColDetLevel)==RGF_SUCCESS)
			{
				if(ColDetLevel>RGF_COLLISIONLEVEL_1)
				{
					bool Result = CheckBoneCollision(CCD->World(), pActor, thePoint, &theBox->Min, &theBox->Max);

					if(!Result)
						return true;
				}
			}

			Contents->Actor = pActor;		// Actor we hit
			return false;					// Ugh, we hit an actor instead
		}

		return true;						// No contents, so it's OK to be there.
	}

	//	Well, we have contents.  Now we don't care if the contents happen
	//	..to be something we're aware of, so let's check mainly for
	//	..ACTORS and MODELS.
	if((Contents->Mesh != NULL) || (Contents->Model != NULL) || (Contents->Actor != NULL))
		return false;						// Something there we don't like.

	//	Finally, check to see if the contents is marked IMPENETRABLE in the Type.
	if((Contents->Contents & 0xffff0000) == Impenetrable)
		return false;						// Can't exist in this one..

	if(Result != GE_FALSE)
		return false;						// We hit SOMETHING, so it's not OK...

	//	All else being equal, I guess it's OK to be here...
	return true;
}

/* ------------------------------------------------------------------------------------ */
//	Debug
/* ------------------------------------------------------------------------------------ */
void Collider::Debug()
{

}

/*
	07/15/2004 Wendell Buckner
    BUG FIX - Bone Collisions fail because we expect to hit the bone immediately after
	hitting the	overall bounding box. So tag the actor as being hit at the bounding box
	level and after that check ONLY	the bone bounding boxes until the whatever hit the
	overall bounding box no longer exists.
*/
/* ------------------------------------------------------------------------------------ */
//	CheckForBoneCollision
/* ------------------------------------------------------------------------------------ */
bool Collider::CheckForBoneCollision(geVec3d *Min, geVec3d *Max,
								 const geVec3d &OldPosition, const geVec3d &NewPosition,
								 GE_Collision *Collision, geActor *Actor,
								 char *BoneHit, bool BoneLevel,
								 void *CollisionObject )
{
	GE_Contents Contents;
	int Result, Result1;

	memset(Collision, 0, sizeof(GE_Collision));

	geActor *pActor;
	geFloat T;
	geVec3d Normal;

	geBoolean RayHitActor = GE_FALSE;

	RayHitActor = CCD->ActorManager()->DoesRayHitActor(OldPosition, NewPosition, &pActor, Actor, &T, &Normal, CollisionObject );

	if(!RayHitActor && BoneLevel)
		RayHitActor = CCD->ActorManager()->DidRayHitActor(OldPosition, NewPosition, &pActor, Actor, &T, &Normal, CollisionObject );

	if(RayHitActor)
	{
		Collision->Actor = pActor;
		Collision->Model = NULL;
		Collision->Plane.Normal = Normal;

		geVec3d Vec;
		geVec3d_Subtract(&NewPosition, &OldPosition, &Vec);

//	changed QD 12/15/05
		// float len = geVec3d_Length(&Vec);
		// geVec3d_Normalize(&Vec);
		float len = geVec3d_Normalize(&Vec);
// end change

		geVec3d_AddScaled(&OldPosition, &Vec, len*T, &(Collision->Impact));

		if(!BoneLevel)
			return true;

		int TotalStaticBoneCount = geActor_GetBoneCount(Collision->Actor);
		geExtBox theStaticBoneBox;

		for(int nStatic=0; nStatic<TotalStaticBoneCount; nStatic++)
		{
			// The bone bounding box comes back in worldspace coordinates...
			if(geActor_GetBoneExtBoxByIndex(Collision->Actor, nStatic, &theStaticBoneBox) != GE_TRUE)
				continue;								// Nothing here, skip it

			if(geExtBox_RayCollision(&theStaticBoneBox, &OldPosition, &NewPosition, &T, &Normal) == GE_TRUE)
			{
				geXForm3d Attachment;
				int ParentBoneIndex;
				const char *BoneName;

				geBody_GetBone(geActor_GetBody(geActor_GetActorDef(Collision->Actor)),
					nStatic, &BoneName, &Attachment, &ParentBoneIndex);

// changed QD 12/15/05
/*				if(BoneName != NULL)
					strcpy(BoneHit, BoneName);

				if(BoneName)
					CCD->ActorManager()->SetLastBoneHit(pActor, BoneName);
*/
				if(BoneName)
				{
					strcpy(BoneHit, BoneName);
					CCD->ActorManager()->SetLastBoneHit(pActor, BoneName);
				}
// end change
				return true;
			}
		}
	}

	memset(Collision, 0, sizeof(GE_Collision));

	Result = GE_FALSE;

	if(geWorld_Collision(CCD->World(), NULL,
		NULL, &OldPosition, &NewPosition, kCollideFlags,
		GE_COLLIDE_MODELS, 0x0, NULL, NULL, Collision) == GE_TRUE)
	{
		Collision->Actor = NULL;
		if(!(CCD->Doors()->IsADoor(Collision->Model) && CCD->ModelManager()->IsRunning(Collision->Model)))
			return true;
		else
			Collision->Model = NULL;
	}

	if((Min != NULL) && (Max != NULL) && (Result != GE_TRUE))
	{
		Result1 = geWorld_GetContents(CCD->World(),
			&NewPosition, Min, Max, GE_COLLIDE_MODELS,
			0xffffffff, NULL, NULL, &Contents);

		if(Result1 == GE_TRUE)
		{
			Contents.Actor = NULL;
			if((Contents.Contents & (GE_CONTENTS_EMPTY | GE_CONTENTS_SOLID)) == GE_CONTENTS_EMPTY)
				Result = GE_FALSE;
			else if((Contents.Contents & GE_CONTENTS_HINT))
			{
				Result = GE_FALSE;
			}
// changed RF064
/*			else if((Contents.Contents & GE_CONTENTS_AREA))
			{
				Result = GE_FALSE;
			} */
// end change RF064
			else
			{
				Result = GE_TRUE;

				if(CCD->Doors()->IsADoor(Contents.Model))
					Result = GE_FALSE;
			}
		}

		if(Result == GE_FALSE)
		{
			geWorld_Model *pModel;
			geExtBox pBox;
			pBox.Min.X = Min->X + NewPosition.X;
			pBox.Min.Y = Min->Y + NewPosition.Y;
			pBox.Min.Z = Min->Z + NewPosition.Z;
			pBox.Max.X = Max->X + NewPosition.X;
			pBox.Max.Y = Max->Y + NewPosition.Y;
			pBox.Max.Z = Max->Z + NewPosition.Z;

			Contents.Model = NULL;
			Contents.Actor = NULL;

			// Gotta check to see if we're hitting a world model
			if(CCD->ModelManager()->DoesBoxHitModel(NewPosition, pBox, &pModel) == GE_TRUE)
			{
				Contents.Model = pModel;		// Model we hit
				Result = GE_TRUE;					// Sorry, you can't be here...
			}
		}

		if(Result == GE_TRUE)
		{
			// Fill collision struct with information
			Collision->Mesh = Contents.Mesh;
			Collision->Model = Contents.Model;
			Collision->Actor = NULL;
			Collision->Impact = OldPosition;
			return true;
		}
	}

	return false;
}

/* ------------------------------------------------------------------------------------ */
//	CheckForBoneCollision
/* ------------------------------------------------------------------------------------ */
bool Collider::CheckForBoneCollision(geVec3d *Min, geVec3d *Max,
								 const geVec3d &OldPosition, const geVec3d &NewPosition,
								 GE_Collision *Collision, geActor *Actor,
								 char *BoneHit, bool BoneLevel)
{
	GE_Contents Contents;
	int Result, Result1;

	memset(Collision, 0, sizeof(GE_Collision));

// changed QD 01/2004
	if(CCD->Meshes()->CollisionCheck(NULL,NULL,/*Min, Max,*/ OldPosition, NewPosition, Collision))
	{
		Collision->Mesh = (geMesh*)CCD->Meshes();
		Collision->Actor = NULL;
		Collision->Model = NULL;
		return true;
	}
// end change

	geActor *pActor;
	geFloat T;
	geVec3d Normal;

	if(CCD->ActorManager()->DoesRayHitActor(OldPosition, NewPosition,
		&pActor, Actor, &T, &Normal) == GE_TRUE)
	{
		Collision->Actor = pActor;
		Collision->Model = NULL;
		Collision->Plane.Normal = Normal;

		geVec3d Vec;
		geVec3d_Subtract(&NewPosition, &OldPosition, &Vec);

// changed QD 01/2004
		// float len = geVec3d_Length(&Vec);
		// geVec3d_Normalize(&Vec);
		float len = geVec3d_Normalize(&Vec);
// end change

		geVec3d_AddScaled(&OldPosition, &Vec, len*T, &Collision->Impact);

		if(!BoneLevel)
			return true;

		int TotalStaticBoneCount = geActor_GetBoneCount(Collision->Actor);
		geExtBox theStaticBoneBox;

		for(int nStatic=0; nStatic<TotalStaticBoneCount; nStatic++)
		{
			// The bone bounding box comes back in worldspace coordinates...
			if(geActor_GetBoneExtBoxByIndex(Collision->Actor, nStatic, &theStaticBoneBox) != GE_TRUE)
				continue;								// Nothing here, skip it

			if(geExtBox_RayCollision(&theStaticBoneBox, &OldPosition, &NewPosition, &T, &Normal)==GE_TRUE)
			{
				geXForm3d Attachment;
				int ParentBoneIndex;
				const char *BoneName;

				geBody_GetBone(geActor_GetBody(geActor_GetActorDef(Collision->Actor)),
					nStatic, &BoneName, &Attachment, &ParentBoneIndex);

				if(BoneName != NULL)
					strcpy(BoneHit, BoneName);

				return true;
			}
		}
	}

	memset(Collision, 0, sizeof(GE_Collision));

	Result = GE_FALSE;

	if(geWorld_Collision(CCD->World(), NULL,
		NULL, &OldPosition, &NewPosition, kCollideFlags,
		GE_COLLIDE_MODELS, 0x0, NULL, NULL, Collision) == GE_TRUE)
	{
		Collision->Actor = NULL;

		if(!(CCD->Doors()->IsADoor(Collision->Model) && CCD->ModelManager()->IsRunning(Collision->Model)))
			return true;
		else
			Collision->Model = NULL;
	}



	if((Min != NULL) && (Max != NULL) && (Result != GE_TRUE))
	{
		Result1 = geWorld_GetContents(CCD->World(),
			&NewPosition, Min, Max, GE_COLLIDE_MODELS,
			0xffffffff, NULL, NULL, &Contents);

		if(Result1 == GE_TRUE)
		{
			Contents.Actor = NULL;

			if((Contents.Contents & (GE_CONTENTS_EMPTY | GE_CONTENTS_SOLID)) == GE_CONTENTS_EMPTY)
				Result = GE_FALSE;
			else if((Contents.Contents & GE_CONTENTS_HINT))
			{
				Result = GE_FALSE;
			}
// changed RF064
/*			else if((Contents.Contents & GE_CONTENTS_AREA))
			{
				Result = GE_FALSE;
			} */
// end change RF064
			else
			{
				Result = GE_TRUE;

				if(CCD->Doors()->IsADoor(Contents.Model))
					Result = GE_FALSE;
			}
		}
		if(Result == GE_FALSE)
		{
			geWorld_Model *pModel;
			geExtBox pBox;

			pBox.Min.X = Min->X + NewPosition.X;
			pBox.Min.Y = Min->Y + NewPosition.Y;
			pBox.Min.Z = Min->Z + NewPosition.Z;
			pBox.Max.X = Max->X + NewPosition.X;
			pBox.Max.Y = Max->Y + NewPosition.Y;
			pBox.Max.Z = Max->Z + NewPosition.Z;

			Contents.Model = NULL;
			Contents.Actor = NULL;

			// Gotta check to see if we're hitting a world model
			if(CCD->ModelManager()->DoesBoxHitModel(NewPosition, pBox, &pModel) == GE_TRUE)
			{
				Contents.Model = pModel;		// Model we hit
				Result = GE_TRUE;					// Sorry, you can't be here...
			}
		}

		if(Result == GE_TRUE)
		{
			// Fill collision struct with information
			Collision->Mesh = Contents.Mesh;
			Collision->Model = Contents.Model;
			Collision->Actor = NULL;
			Collision->Impact = OldPosition;
			return true;
		}
	}

	return false;
}

/* ------------------------------------------------------------------------------------ */
//	CheckSolid
/* ------------------------------------------------------------------------------------ */
bool Collider::CheckSolid(geVec3d *thePoint, geExtBox *theBox, geActor *Actor)
{
	GE_Contents Contents;
	geBoolean Result = GE_FALSE;

	memset(&Contents, 0, sizeof(GE_Contents));

	if(geWorld_GetContents(CCD->World(),
		thePoint, &theBox->Min, &theBox->Max, GE_COLLIDE_MODELS,
		0xffffffff, NULL, NULL, &Contents) == GE_TRUE)
	{
		Contents.Actor = NULL;

		if((Contents.Contents & (GE_CONTENTS_EMPTY | GE_CONTENTS_SOLID)) == GE_CONTENTS_EMPTY)
			Result = GE_FALSE;
		else if((Contents.Contents & GE_CONTENTS_HINT))
		{
			Result = GE_FALSE;
		}
		else
		{
			Result = GE_TRUE;

			if(CCD->Doors()->IsADoor(Contents.Model))
				Result = GE_FALSE;
		}
	}

	if(Result == GE_FALSE)
	{
		geWorld_Model *pModel;
		geVec3d pTemp = *thePoint;
		geExtBox pBox = *theBox;

		pBox.Min.X += pTemp.X;
		pBox.Min.Y += pTemp.Y;
		pBox.Min.Z += pTemp.Z;
		pBox.Max.X += pTemp.X;
		pBox.Max.Y += pTemp.Y;
		pBox.Max.Z += pTemp.Z;

		// Gotta check to see if we're hitting a world model
		if(CCD->ModelManager()->DoesBoxHitModel(pTemp, pBox, &pModel) == GE_TRUE)
			return false;					// Sorry, you can't be here...

		return true;						// No contents, so it's OK to be there.
	}

	//	Well, we have contents.  Now we don't care if the contents happen
	//	..to be something we're aware of, so let's check mainly for
	//	..ACTORS and MODELS.
	if((Contents.Mesh != NULL) || (Contents.Model != NULL))
		return false;						// Something there we don't like.

	//	Finally, check to see if the contents is marked IMPENETRABLE in the Type.
	if((Contents.Contents & 0xffff0000) == Impenetrable)
		return false;						// Can't exist in this one..

	if(Result != GE_FALSE)
		return false;						// We hit SOMETHING, so it's not OK...

	//	All else being equal, I guess it's OK to be here...
	return true;
}

/* ----------------------------------- END OF FILE ------------------------------------ */
