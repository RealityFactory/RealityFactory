/****************************************************************************************/
/*																						*/
/*	ActorManager.h:		Actor Manager													*/
/*																						*/
/*	This file contains the class declaration of the Actor Manager						*/
/*	system for the Reality factory.														*/
/*																						*/
/*	It's the Actor Manager's job to load, remove, translate,							*/
/*	rotate, and scale all the actors in the system.  The Actor Manager					*/
/*	maintains a database of "loaded actors" from which instances can					*/
/*	be created.																			*/
/*																						*/
/*	Edit History:																		*/
/*	=============																		*/
/*	07/15/2004 Wendell Buckner															*/
/*	BUG FIX - Bone Collisions fail because we expect to hit the bone immediately after	*/
/*	hitting the overall bounding box. So tag the actor as being hit at the bounding box */
/*	level and after that check ONLY the bone bounding boxes until the whatever hit the	*/
/*	overall bounding box no longer exists.												*/
/*																						*/
/*	begin change gekido - 02.24.2004													*/
/*	pretty major changes to the animation system, adding animation 'channels'			*/
/*	so the user can trigger up to 5 different channels of animation at once.			*/
/*																						*/
/****************************************************************************************/


#ifndef __CACTOR_MANAGER__
#define __CACTOR_MANAGER__

class CPersistentAttributes;			// Forward ref.

//	Setup defines

#define	ACTOR_LIST_SIZE		1024		// Max # of actors per level
#define	PASSENGER_LIST_SIZE	64			// Max # of passengers in a vehicle
#define ATTACHACTORS		16

/* 07/15/2004 Wendell Buckner
    BUG FIX - Bone Collisions fail because we expect to hit the bone immediately after hitting the
	overall bounding box. So tag the actor as being hit at the bounding box level and after that check ONLY
	the bone bounding boxes until the whatever hit the overall bounding box no longer exists. */
typedef enum CollideObjectLevels
{
	COLActorBBox,
	COLBoneBBox,
	COLTriangleBBox,
	COLMaxBBox,

} CollideObjectLevels;

typedef struct CollideObjectInformation
{
	void    *CollideObject;
	geFloat Percent;
	geVec3d Normal;
    CollideObjectInformation *PrevCollideObject;
    CollideObjectInformation *NextCollideObject;

} CollideObjectInformation;

// begin change gekido
// ANIMATION CHANNELS
#define ANIMATION_CHANNEL_ROOT 0		// the default animation channel (lower-body)
#define ANIMATION_CHANNEL_UPPERBODY 1	// torso/upper body
#define ANIMATION_CHANNEL_HEAD 2		// head
#define ANIMATION_CHANNEL_USER001 3		// custom user channel1
#define ANIMATION_CHANNEL_USER002 4		// custom user channel2

struct Attachment
{
	char MasterBone[64];
	geActor *AttachedActor;
	geVec3d Offset;
	bool CollFlag;
	int RenderFlag;
};

/****************************************************************************************/
/*	This struct holds information for each instance of an actor							*/
/****************************************************************************************/
struct ActorInstanceList
{
	geActor *Actor;						// The actor itself
	geActor_Def *theDef;				// Pointer to loaded actor def
	geActor_Def *theLODDef[3];			// Actor definition
	geActor *LODActor[3];
	geBitmap *Bitmap;
	float LODdistance[5];
	int LODLevel;
	geVec3d localTranslation;			// Actor translation
	geVec3d OldTranslation;				// Actors previous position
	geVec3d localRotation;				// Actor rotation
	geVec3d BaseRotation;				// Actor baseline rotation


	// default channel
	geFloat AnimationTime;				// Time in current animation
	geFloat PrevAnimationTime;
	geMotion *pMotion;
	geMotion *pBMotion;

// changed QD 12/15/05 - currently not used...
/*
// begin change gekido 02.22.2004
// adding support for multiple animation 'channels' which
// allow designers to create multi-threaded animations blended from sub-motions

	// channel 2
	geFloat AnimationTime1;				// Time in current animation
	geFloat PrevAnimationTime1;
	geMotion *pMotion1;
	geMotion *pBMotion1;

	// channel 3
	geFloat AnimationTime2;				// Time in current animation
	geFloat PrevAnimationTime2;
	geMotion *pMotion2;
	geMotion *pBMotion2;

	// channel 4
	geFloat AnimationTime3;				// Time in current animation
	geFloat PrevAnimationTime3;
	geMotion *pMotion3;
	geMotion *pBMotion3;

	// channel 5
	geFloat AnimationTime4;				// Time in current animation
	geFloat PrevAnimationTime4;
	geMotion *pMotion4;
	geMotion *pBMotion4;
// end channel animation info
// end change gekido 02.22.2004
*/
	bool AllowTilt;						// allow actor to tilt up/down
	geFloat TiltX;
	bool BoxChange;						// allow BB to change
	bool NoCollision;					// allow no collision detection
	geFloat ActorAnimationSpeed;		// Speed of actor animation
// changed Nout/QD 12/15/05
//	geFloat Scale;						// Actors current scale
	geVec3d Scale;						// Actors current XYZ scale
// end change
	geFloat Health;						// Actors health, 0 = destroyed
	geFloat Gravity;					// Gravity acting on actor
	geFloat GravityTime;				// Time gravity has been effective (for falling)
	geFloat StepHeight;					// Max. Y delta for moving up during motion
	geVec3d ForceVector[4];				// Current force vector acting on actor
	geFloat ForceLevel[4];				// Current level force is acting at
	geFloat ForceDecay[4];				// Decay speed of force, in units/second
	geVec3d PositionHistory[128];		// Actors position history
// changed RF063
	float ShadowSize;
// changed QD 06/26/04
	geFloat ShadowAlpha;
	geBitmap *ShadowBitmap;
// end change
// changed gekido Oct07.2004
// flag to enable projected shadows for this actor
	bool ProjectedShadows;
	bool Moving;
	int CurrentZone;
	int OldZone;
	int GravityCoeff;
	Liquid *LQ;
	char szEntityName[128];
	bool Attached;
	geVec3d masterRotation;
	Attachment AttachedActors[ATTACHACTORS];
	int RenderFlag;
// end change RF063
// changed RF064
    geWorld_Model *PassengerModel;
	float StartTime;
	GE_Collision GravityCollision;
	bool HideRadar;
	char Group[64];
	GE_RGBA FillColor;
	GE_RGBA AmbientColor;
// end change RF064

// changed QD 07/21/04
	geBoolean AmbientLightFromFloor;
// end change

	// default animation channel (channel 1)
	char szMotionName[128];				// Name of current motion
	char szNextMotionName[128];			// Name of next motion in queue
	char szDefaultMotionName[128];		// Default motion for this actor
	char szBlendMotionName[128];
	float BlendAmount;
	bool BlendFlag;
	char szBlendNextMotionName[128];
	float BlendNextAmount;
	bool BlendNextFlag;
	bool HoldAtEnd;
	bool TransitionFlag;
// changed QD 12/15/05 - currently not used...
/*
// begin change gekido 02.22.2004
// support for multiple animation channels
// each channel provides full support for blending, etc

	// channel 2 (upper body)
	char szMotionName1[128];			// Name of current motion
	char szNextMotionName1[128];		// Name of next motion in queue
	char szDefaultMotionName1[128];		// Default motion for this actor
	char szBlendMotionName1[128];
	float BlendAmount1;
	bool BlendFlag1;
	char szBlendNextMotionName1[128];
	float BlendNextAmount1;
	bool BlendNextFlag1;
	bool HoldAtEnd1;
	bool TransitionFlag1;

	// channel 3 (head)
	char szMotionName2[128];			// Name of current motion
	char szNextMotionName2[128];		// Name of next motion in queue
	char szDefaultMotionName2[128];		// Default motion for this actor
	char szBlendMotionName2[128];
	float BlendAmount2;
	bool BlendFlag2;
	char szBlendNextMotionName2[128];
	float BlendNextAmount2;
	bool BlendNextFlag2;
	bool HoldAtEnd2;
	bool TransitionFlag2;

	// channel 4 (user custom 1)
	char szMotionName3[128];			// Name of current motion
	char szNextMotionName3[128];		// Name of next motion in queue
	char szDefaultMotionName3[128];		// Default motion for this actor
	char szBlendMotionName3[128];
	float BlendAmount3;
	bool BlendFlag3;
	char szBlendNextMotionName3[128];
	float BlendNextAmount3;
	bool BlendNextFlag3;
	bool HoldAtEnd3;
	bool TransitionFlag3;

	// channel 5 (user custom 2)
	char szMotionName4[128];			// Name of current motion
	char szNextMotionName4[128];		// Name of next motion in queue
	char szDefaultMotionName4[128];		// Default motion for this actor
	char szBlendMotionName4[128];
	float BlendAmount4;
	bool BlendFlag4;
	char szBlendNextMotionName4[128];
	float BlendNextAmount4;
	bool BlendNextFlag4;
	bool HoldAtEnd4;
	bool TransitionFlag4;
// end change gekido 02.22.2004
*/
	int CollDetLevel;
	bool slide;
	bool fAutoStepUp;							// Actor auto step up flag
	geFloat MaxStepHeight;						// Actors max step height, in world units
	geActor *Vehicle;							// If riding on anything
	geActor *Passengers[PASSENGER_LIST_SIZE];	// If vehicle, riders
	int ActorType;								// Actor type
	bool NeedsNewBB;							// TRUE means needs new EXTBOX
	CPersistentAttributes *Inventory;			// Actors inventory
	struct ActorInstanceList *pNext;			// Next in list

/* 07/15/2004 Wendell Buckner
    BUG FIX - Bone Collisions fail because we expect to hit the bone immediately after hitting the
	overall bounding box. So tag the actor as being hit at the bounding box level and after that check ONLY
	the bone bounding boxes until the whatever hit the overall bounding box no longer exists. */
	CollideObjectInformation *CollideObjects[COLMaxBBox];
	char LastBoneHit[256];					// Name of last bone collided, if any
};

/****************************************************************************************/
/*	This struct is the main database element for the actor list							*/
/****************************************************************************************/
struct LoadedActorList
{
	geActor_Def			*theActorDef[4];	// Actor definition
	geActor				*Actor[4];
	geBitmap			*Bitmap;
	geVec3d				BaseRotation;		// Baseline rotation
	geFloat				BaseScale;			// Baseline scale
	geFloat				BaseHealth;			// Baseline health
	char				*szFilename;		// Filename actor was from
	char				*szDefaultMotion;	// Default motion for actors
	int					ActorType;			// Type of actor this is
	int					InstanceCount;		// # of actors in instance list
	ActorInstanceList	*IList;				// List of instance for this actor
};


/****************************************************************************************/
/*																						*/
/*	CActorManager Class																	*/
/*																						*/
/****************************************************************************************/

class CActorManager : public CRGFComponent
{
public:
	CActorManager();					// Default constructor
	~CActorManager();					// Default destructor

	// -----------------------------------------------------------------------------------
	// COLLISION
	// -----------------------------------------------------------------------------------

	// changed RF064
	int SetCollide(geActor *theActor);
	// end change RF064
	int SetNoCollide(geActor *theActor);
	int SetColDetLevel(geActor *theActor, int ColDetLevel);
	int GetColDetLevel(geActor *theActor, int *ColDetLevel);

	// Set/Get bounding box
	void SetBBox(geActor *theActor, float SizeX, float SizeY, float SizeZ);
	void SetBoundingBox(geActor *theActor, char *Animation);
	int GetBoundingBox(geActor *theActor, geExtBox *theBox);
	int SetBoxChange(geActor *theActor, bool Flag); // allow bbox to change?

	// Actor-actor collision check
	geBoolean DoesBoxHitActor(const geVec3d &thePoint, const geExtBox &theBox, geActor **theActor);
	geBoolean DoesBoxHitActor(const geVec3d &thePoint, const geExtBox &theBox, geActor **theActor,
							  geActor *ActorToExclude);


	/* 07/15/2004 Wendell Buckner
    BUG FIX - Bone Collisions fail because we expect to hit the bone immediately after hitting the
	overall bounding box. So tag the actor as being hit at the bounding box level and after that check ONLY
	the bone bounding boxes until the whatever hit the overall bounding box no longer exists. */
	CollideObjectInformation *AddCollideObject(CollideObjectLevels CollideObjectLevel,
												ActorInstanceList *ActorInstance,
												CollideObjectInformation *CollideObject);

	geBoolean RemoveCollideObject(CollideObjectLevels CollideObjectLevel,
									ActorInstanceList *ActorInstance,
									void *theCollideObject);

	CollideObjectInformation *GetCollideObject(CollideObjectLevels CollideObjectLevel,
												ActorInstanceList *ActorInstance,
												void *theCollideObject);

	// changed RF064
	geBoolean DoesRayHitActor(const geVec3d &OldPosition, const geVec3d &NewPosition,
								geActor **theActor,	geActor *ActorToExclude,
								geFloat *Percent, geVec3d *Normal);
	// end change RF064

	geBoolean DoesRayHitActor(const geVec3d &OldPosition, const geVec3d &NewPosition,
								geActor **theActor, geActor *ActorToExclude,
								geFloat *Percent, geVec3d *Normal,
								void *CollisionObject);

    geBoolean DidRayHitActor(const geVec3d &OldPosition, const geVec3d &NewPosition,
								geActor **theActor, geActor *ActorToExclude,
								geFloat *Percent, geVec3d *Normal,
								void *CollisionObj);

    geBoolean SetLastBoneHit(geActor *theActor, const char *LastBoneHit);
    char *GetLastBoneHit(geActor *theActor);

	// -----------------------------------------------------------------------------------
	// LOAD
	// -----------------------------------------------------------------------------------

	// Load an actor from a file
	geActor *LoadActor(char *szFilename, geActor *OldActor);
	// Spawn an actor w/ parms
	geActor *SpawnActor(char *szFilename, const geVec3d &Position, const geVec3d &Rotation,
						char *DefaultMotion, char *CurrentMotion, geActor *OldActor);

	// -----------------------------------------------------------------------------------
	// REMOVE
	// -----------------------------------------------------------------------------------

	// Remove an actor
	int RemoveActor(geActor *theActor);
	int RemoveActorCheck(geActor *theActor);

	// -----------------------------------------------------------------------------------
	// ACTORTYPE
	// -----------------------------------------------------------------------------------

	// Set/Get actor type
	int SetType(geActor *theActor, int nType);
	int GetType(geActor *theActor, int *nType);

	bool IsActor(geActor *theActor);

	// -----------------------------------------------------------------------------------
	// POSITION/ROTATION
	// -----------------------------------------------------------------------------------

	// position/rotate an actor
	int Position(geActor *theActor, const geVec3d &Position);
	int Rotate(geActor *theActor, const geVec3d &Rotation);

	// Get position/rotation
	int GetPosition(geActor *theActor, geVec3d *thePosition);
	int GetRotation(geActor *theActor, geVec3d *theRotation);
	int GetRotate(geActor *theActor, geVec3d *theRotation);

	// Set/Get Aligning Rotation
	int SetAligningRotation(geActor *theActor, const geVec3d &Rotation);
	int GetAligningRotation(geActor *theActor, geVec3d *Rotation);

	int AddTranslation(geActor *theActor, const geVec3d &Amount);		// Add translation to actor
	int AddRotation(geActor *theActor, const geVec3d &Amount);			// Add rotation to actor
	int RotateToFacePoint(geActor *theActor, const geVec3d &Position);	// Rotate to face point

	// Turn actor LEFT/RIGHT
	int TurnLeft(geActor *theActor, geFloat theAmount);
	int TurnRight(geActor *theActor, geFloat theAmount);

	// Get Bone Position/Rotation
	int GetBonePosition(geActor *theActor, char *szBone, geVec3d *thePosition);
	int GetBoneRotation(geActor *theActor, char *szBone, geVec3d *theRotation);

	// -----------------------------------------------------------------------------------
	// FALLING
	// -----------------------------------------------------------------------------------

	geBoolean Falling(geActor *theActor);						// Is actor falling?
	int ReallyFall(geActor *theActor);
	int CheckReallyFall(geActor *theActor, const geVec3d &StartPos);

	// -----------------------------------------------------------------------------------
	// ORIENTATION
	// -----------------------------------------------------------------------------------

	// Get UP/IN/LEFT vector
	int UpVector(geActor *theActor, geVec3d *UpVector);			// Get UP vector
	int InVector(geActor *theActor, geVec3d *InVector);			// Get IN vector
	int LeftVector(geActor *theActor, geVec3d *LeftVector);		// Get LEFT vector

	// -----------------------------------------------------------------------------------
	// STEP UP
	// -----------------------------------------------------------------------------------

	int SetAutoStepUp(geActor *theActor, bool fEnable);			// Set actor auto step up
	int SetStepHeight(geActor *theActor, geFloat fMaxStep);		// Set actor step-up height
	int GetStepHeight(geActor *theActor, geFloat *fMaxStep);

	// -----------------------------------------------------------------------------------
	// MOVING
	// -----------------------------------------------------------------------------------

	void SetMoving(geActor *theActor);
	bool GetMoving(geActor *theActor);

	int MoveForward(geActor *theActor, geFloat fSpeed);			// Move actor forward
	int MoveBackward(geActor *theActor, geFloat fSpeed);		// Move actor backward
	int MoveLeft(geActor *theActor, geFloat fSpeed);			// Move actor left
	int MoveRight(geActor *theActor, geFloat fSpeed);			// Move actor right
	// Move actor to nowhere (to a galaxy far, far away...)
	int MoveAway(geActor *theActor);

	// -----------------------------------------------------------------------------------
	// ANIMATION METHODS
	// -----------------------------------------------------------------------------------

	int SetAnimationSpeed(geActor *theActor, geFloat fSpeed);		// Set animation speed

	// begin change gekido 02.22.2004
	// support for animation channels
	// also reorganized the animation functions so they are in one place
	char *GetMotion(geActor *theActor);
	int SetMotion(geActor *theActor, char *MotionName, int Channel = ANIMATION_CHANNEL_ROOT);		// Set actor motion
	int SetNextMotion(geActor *theActor, char *MotionName, int Channel = ANIMATION_CHANNEL_ROOT);	// Prepare next motion
	int SetDefaultMotion(geActor *theActor, char *MotionName, int Channel = ANIMATION_CHANNEL_ROOT);// Set default motion
	int ClearMotionToDefault(geActor *theActor, int Channel = ANIMATION_CHANNEL_ROOT);				// Force motion to default motion

	// creates a new pose from 2 animations, keeps existing animation timing (if mid-animation)
	int SetBlendMot(geActor *theActor, char *name1, char *name2, float Amount, int Channel = ANIMATION_CHANNEL_ROOT);
	// creates a new pose from 2 animations, CLEARS animation timing
	int SetBlendMotion(geActor *theActor, char *name1, char *name2, float Amount, int Channel = ANIMATION_CHANNEL_ROOT);
	int SetBlendNextMotion(geActor *theActor, char *name1, char *name2, float Amount, int Channel = ANIMATION_CHANNEL_ROOT);

	void SetHoldAtEnd(geActor *theActor, bool State, int Channel = ANIMATION_CHANNEL_ROOT);
	bool EndAnimation(geActor *theActor, int Channel = ANIMATION_CHANNEL_ROOT);

	// start multiplayer
	void SetAnimationTime(geActor *theActor, float time, int Channel = ANIMATION_CHANNEL_ROOT);
	// end multiplayer
	float GetAnimationTime(geActor *theActor, int Channel = ANIMATION_CHANNEL_ROOT);

	geMotion *GetpMotion(geActor *theActor, int Channel = ANIMATION_CHANNEL_ROOT);
	geMotion *GetpBMotion(geActor *theActor, int Channel = ANIMATION_CHANNEL_ROOT);

	float GetBlendAmount(geActor *theActor, int Channel = ANIMATION_CHANNEL_ROOT);
	int CheckAnimation(geActor *theActor, char *Animation, int Channel = ANIMATION_CHANNEL_ROOT);

	int SetTransitionMotion(geActor *theActor, char *name1, float Amount, char *name2, int Channel = ANIMATION_CHANNEL_ROOT);
	bool CheckTransitionMotion(geActor *theActor, char *name1, int Channel = ANIMATION_CHANNEL_ROOT);
	float GetStartTime(geActor *theActor, int Channel = ANIMATION_CHANNEL_ROOT);
	bool GetTransitionFlag(geActor *theActor, int Channel = ANIMATION_CHANNEL_ROOT);
	// end change gekido

	// -----------------------------------------------------------------------------------
	// LIGHTING
	// -----------------------------------------------------------------------------------

	//	Actor lighting control, Oh Joy.
	// changed QD 07/21/04
	int SetActorDynamicLighting(geActor *theActor, const GE_RGBA &FillColor,
								const GE_RGBA &AmbientColor, geBoolean AmbientLightFromFloor);
	// end change
	// changed QD 12/15/05
	int GetActorDynamicLighting(geActor *theActor, GE_RGBA *FillColor, GE_RGBA *AmbientColor,
								geBoolean *AmbientLightFromFloor);
	// end change
	int ResetActorDynamicLighting(geActor *theActor);

	// -----------------------------------------------------------------------------------
	// FORCE
	// -----------------------------------------------------------------------------------

	// It's possible to add a FORCE to an actor.  This FORCE (seperate from gravity)
	// ..effects an actors translation over time.
	int SetForce(geActor *theActor, int nForceNumber, const geVec3d &fVector,
					geFloat InitialValue, geFloat Decay);
	// changed RF063
	int GetForce(geActor *theActor, int nForceNumber, geVec3d *ForceVector,
					geFloat *ForceLevel, geFloat *ForceDecay);

	int RemoveForce(geActor *theActor, int nForceNumber);
	geBoolean ForceActive(geActor *theActor, int nForceNumber);
	// end change RF063

	// -----------------------------------------------------------------------------------
	// ZONE
	// -----------------------------------------------------------------------------------

	// changed RF063
	//	Functions for probing the actors environment
	int GetActorZone(geActor *theActor, int *ZoneType);				// Get actors zone
	int GetActorOldZone(geActor *theActor, int *ZoneType);
	Liquid *GetLiquid(geActor *theActor);
	// end change RF063

	// -----------------------------------------------------------------------------------
	// ENTITYNAME
	// -----------------------------------------------------------------------------------

	// changed RF063
	int SetEntityName(geActor *theActor, char *name);
	// Added By Pickles to RF07D
	char *GetEntityName(geActor *theActor);
	// End Added By Pickles to RF07D
	geActor *GetByEntityName(char *name);
	// end change RF063

	// -----------------------------------------------------------------------------------
	// SCALE
	// -----------------------------------------------------------------------------------

	// Important note: to set the BASELINE for scale, etc. for all actors
	// ..of a specific type use nHandle = (-1).
	int SetScale(geActor *theActor, geFloat fScale);			// Scale actor
	int GetScale(geActor *theActor, geFloat *fScale);			// Get actor scale (max of x,y,z)
	// changed Nout/QD 12/15/05
	int SetScaleX(geActor *theActor, geFloat fScale);
	int SetScaleY(geActor *theActor, geFloat fScale);
	int SetScaleZ(geActor *theActor, geFloat fScale);
	int SetScaleXYZ(geActor *theActor, const geVec3d &Scale);

	int GetScaleX(geActor *theActor, geFloat *fScale);
	int GetScaleY(geActor *theActor, geFloat *fScale);
	int GetScaleZ(geActor *theActor, geFloat *fScale);
	int GetScaleXYZ(geActor *theActor, geVec3d *Scale);
	// end change

	// -----------------------------------------------------------------------------------
	// ALPHA
	// -----------------------------------------------------------------------------------

	int SetAlpha(geActor *theActor, geFloat fAlpha);			// Set actor alpha
	int GetAlpha(geActor *theActor, geFloat *fAlpha);			// Get actor alpha

	// -----------------------------------------------------------------------------------
	// HEALTH
	// -----------------------------------------------------------------------------------

	int SetHealth(geActor *theActor, geFloat fHealth);			// Set actors health
	int ModifyHealth(geActor *theActor, geFloat fAmount);		// Modify actors health
	int GetHealth(geActor *theActor, geFloat *fHealth);			// Get actors health

	// -----------------------------------------------------------------------------------
	// GRAVITY
	// -----------------------------------------------------------------------------------

	int SetGravity(geActor *theActor, geFloat fGravity);		// Set gravity for actor
	int GetGravity(geActor *theActor, geFloat *fGravity);		// Get gravity
	// changed RF064
	int SetGravityTime(geActor *theActor, geFloat fGravitytime);
	int GetGravityCollision(geActor *theActor, GE_Collision *Collision);
	// end change RF064

	// -----------------------------------------------------------------------------------
	// SPATIAL RELATION
	// -----------------------------------------------------------------------------------

	// Is actor in front of me?
	bool IsInFrontOf(geActor *theActor, geActor *theOtherActor);
	// How far away is the other actor?
	geFloat DistanceFrom(geActor *theActor, geActor *theOtherActor);
	// How far is actor from point?
	geFloat DistanceFrom(const geVec3d &Point, geActor *theActor);
	// Fill a list with pointers to all actors within a specific range of a particular point
	int GetActorsInRange(const geVec3d &Point, geFloat Range, int nListSize, geActor **ActorList);

	// -----------------------------------------------------------------------------------
	// VEHICLE/PASSENGER
	// -----------------------------------------------------------------------------------

	// Actor on model stuff
	void SetVehicle(geActor *theActor, geActor *theVehicle);
	geActor *GetVehicle(geActor *theActor);
	int SetPassenger(geActor *thePassenger, geActor *theVehicle);
	int RemovePassenger(geActor *thePassenger);

	// -----------------------------------------------------------------------------------
	// INVENTORY
	// -----------------------------------------------------------------------------------

	// Inventory list access
	CPersistentAttributes *Inventory(geActor *theActor);

	// -----------------------------------------------------------------------------------
	// TICK
	// -----------------------------------------------------------------------------------

	// Dispatch time to all actors
	void Tick(geFloat dwTicks);			// Process passage of time

	// -----------------------------------------------------------------------------------
	// TILT
	// -----------------------------------------------------------------------------------

	int SetTilt(geActor *theActor, bool Flag);
	int TiltUp(geActor *theActor, geFloat theAmount);
	int TiltDown(geActor *theActor, geFloat theAmount);
	float GetTiltX(geActor *theActor);

	// -----------------------------------------------------------------------------------
	// RADAR
	// -----------------------------------------------------------------------------------

	// changed RF064
	int SetHideRadar(geActor *theActor, bool flag);
	int GetHideRadar(geActor *theActor, bool *flag);
	// end change RF064

	// -----------------------------------------------------------------------------------
	// GROUP
	// -----------------------------------------------------------------------------------

	// changed RF064
	int SetGroup(geActor *theActor, char *name);
	char *GetGroup(geActor *theActor);
	// end change RF064

	// -----------------------------------------------------------------------------------
	// SHADOWS
	// -----------------------------------------------------------------------------------

	// changed RF064
	// changed QD 06/26/04
	// BITMAP SHADOW
	int SetShadow(geActor *theActor, geFloat fSize);
	// void SetShadowAlpha(geFloat Alpha) { ShadowAlpha = Alpha; }
	// void SetShadowBitmap(geBitmap *Bitmap){ ShadowBitmap = Bitmap; }
	int SetShadowAlpha(geActor *theActor, geFloat Alpha);
	int SetShadowBitmap(geActor *theActor, geBitmap *Bitmap);
	// begin change gekido - Projected Shadows
	// PROJECTED SHADOW
	int SetProjectedShadows(geActor *theActor, bool flag);
	// end change gekido
	// end change
	// changed QD Shadows
	// STENCIL SHADOW
	geBoolean SetStencilShadows(geActor *theActor, geBoolean flag);
	// end change
	// end change RF064

	// -----------------------------------------------------------------------------------
	// ATTACH/DETACH
	// -----------------------------------------------------------------------------------

	// changed RF064
	void ActorAttach(geActor *Slave,  char *SlaveBoneName,
					 geActor *Master, char *MasterBoneName,
					 const geVec3d &AttachOffset, const geVec3d &Angle);
	void ActorDetach(geActor *Slave);
	void DetachFromActor(geActor *Master, geActor *Slave);
	// end change RF064

	// -----------------------------------------------------------------------------------
	// LOD
	// -----------------------------------------------------------------------------------

	// changed RF064
	int SetLODdistance(geActor *theActor,float LOD1,float LOD2,float LOD3,float LOD4,float LOD5);
	int GetLODLevel(geActor *theActor, int *Level);
	// end change RF064

	// -----------------------------------------------------------------------------------
	// MISC METHODS
	// -----------------------------------------------------------------------------------

	int SetSlide(geActor *theActor, bool Flag);
	int GetAnimationHeight(geActor *theActor, char *Animation, float *Height);
	int SetAnimationHeight(geActor *theActor, char *Animation, bool Camera);
	geBoolean ValidateMove(const geVec3d &StartPos, const geVec3d &EndPos,
							geActor *theActor, bool slide);
	int CountActors();
	// changed RF064
	int ChangeMaterial(geActor *theActor, char *Change);
	int SetRoot(geActor *theActor, char *BoneName);
	int SetActorFlags(geActor *theActor, int Flag);
	//  end change RF064


private:
	// -----------------------------------------------------------------------------------
	//	Private member functions
	// -----------------------------------------------------------------------------------

	geActor *AddNewInstance(LoadedActorList *theEntry, geActor *OldActor);
	int  RemoveInstance(geActor *theActor);			// Delete instance of actor
	void RemoveAllActors(LoadedActorList *theEntry);

	ActorInstanceList *LocateInstance(geActor *theActor, LoadedActorList *theEntry);
	ActorInstanceList *LocateInstance(geActor *theActor);

	void AdvanceInstanceTime(ActorInstanceList *theEntry,	geFloat dwTicks);
	void TimeAdvanceAllInstances(LoadedActorList *theEntry,	geFloat dwTicks);
	void UpdateActorState(ActorInstanceList *theEntry);

	void ProcessGravity(ActorInstanceList *theEntry, geFloat dwTicks);
	void ProcessForce(ActorInstanceList *theEntry, geFloat dwTicks);
	int Move(ActorInstanceList *pEntry, int nHow, geFloat fSpeed);			// Velocity-based motion for actor
	bool CheckForStepUp(ActorInstanceList *pEntry, geVec3d NewPosition);
	int GetCurrentZone(ActorInstanceList *pEntry);	// Get brush zone types
	geBoolean ValidateMotion(geVec3d StartPos, geVec3d EndPos,
		ActorInstanceList *pEntry, bool fStepUpOK, bool slide);			// Validate and handle motion

	void MoveAllVehicles(LoadedActorList *theEntry, float dwTicks);		// Move vehicle actors
	int TranslateAllPassengers(ActorInstanceList *pEntry);

private:
	// -----------------------------------------------------------------------------------
	//	Private member variables
	// -----------------------------------------------------------------------------------

	LoadedActorList *MainList[ACTOR_LIST_SIZE];	// Database of loaded actors
	int m_GlobalInstanceCount;			// Level instance counter
	// changed RF064
	// changed QD 06/26/04 - each actor can have its own shadow bitmap now
	// geFloat ShadowAlpha;
	// geBitmap *ShadowBitmap;
	// end change
	CIniFile AttrFile;
	bool ValidAttr;
	// end change RF064
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
