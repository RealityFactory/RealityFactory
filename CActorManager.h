/************************************************************************************//**
 * @file CActorManager.h
 * @brief This file contains the class declaration of the Actor Manager system
 * for Reality Factory.
 *
 * It's the Actor Manager's job to load, remove, translate, rotate, and scale
 * all the actors in the system. The Actor Manager maintains a database of
 * "loaded actors" from which instances can be created.
 ****************************************************************************************/

#ifndef __CACTOR_MANAGER__
#define __CACTOR_MANAGER__

class CPersistentAttributes;			// Forward ref.

// Setup defines
#define	ACTOR_LIST_SIZE		1024		///< Max # of actors per level
#define	PASSENGER_LIST_SIZE	64			///< Max # of passengers in a vehicle
#define ATTACHACTORS		16			///< Max # of attached actors
#define ATTACHBLENDACTORS	16			///< Max # of attached blending actors

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
	void*	CollideObject;
	float	Percent;
	geVec3d	Normal;
	CollideObjectInformation *PrevCollideObject;
	CollideObjectInformation *NextCollideObject;

} CollideObjectInformation;


// ANIMATION CHANNELS
enum
{
	ANIMATION_CHANNEL_ROOT = 0,		///< the default animation channel (lower-body)
	ANIMATION_CHANNEL_UPPERBODY,	///< torso/upper body
	ANIMATION_CHANNEL_HEAD,			///< head
	ANIMATION_CHANNEL_USER001,		///< custom user channel1
	ANIMATION_CHANNEL_USER002,		///< custom user channel2
	ANIMATION_CHANNELS
};


struct Attachment
{
	char MasterBone[64];
	char SlaveBone[64];
	geActor *AttachedActor;
	geVec3d Offset;
	bool CollFlag;
	int RenderFlag;
};

/************************************************************************************//**
 * @brief This struct holds information for each instance of an actor
 ****************************************************************************************/
struct ActorInstanceList
{
	geActor *Actor;						///< The actor itself
	geActor_Def *theDef;				///< Pointer to loaded actor def
	geActor_Def *theLODDef[3];			///< Actor definition
	geActor *LODActor[3];				///< Level of detail actors
	geBitmap *Bitmap;
	float LODdistance[5];
	int LODLevel;
	geVec3d localTranslation;			///< Actor translation
	geVec3d OldTranslation;				///< Actors previous position
	geVec3d localRotation;				///< Actor rotation
	geVec3d BaseRotation;				///< Actor baseline rotation

	// default channel
	geFloat AnimationTime;				///< Time in current animation
	geFloat PrevAnimationTime;
	geMotion *pMotion;
	geMotion *pBMotion;

#if 0
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
#endif
	bool AllowTilt;						///< allow actor to tilt up/down
	geFloat TiltX;
	bool BoxChange;						///< allow BB to change
	bool NoCollision;					///< allow no collision detection
	geFloat ActorAnimationSpeed;		///< Speed of actor animation
	geVec3d Scale;						///< Actors current XYZ scale
	geFloat Health;						///< Actors health, 0 = destroyed
	geVec3d Gravity;					///< Gravity acting on actor
	geFloat GravityTime;				///< Time gravity has been effective (for falling)
	geFloat StepHeight;					///< Max. Y delta for moving up during motion
	bool ForceEnabled;
	geVec3d ForceVector[4];				///< Current force vector acting on actor
	geFloat ForceLevel[4];				///< Current level force is acting at
	geFloat ForceDecay[4];				///< Decay speed of force, in units/second
	geVec3d PositionHistory[128];		///< Actors position history
	float ShadowSize;					///< Size of bitmap shadow
	geFloat ShadowAlpha;				///< Alpha of shadow
	geBitmap *ShadowBitmap;				///< Bitmap to use for shadow
	bool ProjectedShadows;				///< flag to enable projected shadows for this actor
	bool Moving;
	int CurrentZone;
	int OldZone;
	int GravityCoeff;
	Liquid *LQ;
	char szEntityName[128];

	bool Attached;
	geVec3d masterRotation;
	Attachment AttachedActors[ATTACHACTORS];

	bool AttachedB;
	geActor *AttachedBlendActors[ATTACHBLENDACTORS];

	int RenderFlag;
	geWorld_Model *PassengerModel;
	float StartTime;
	GE_Collision GravityCollision;
	bool HideRadar;
	char Group[64];
	GE_RGBA FillColor;
	GE_RGBA AmbientColor;

	geBoolean AmbientLightFromFloor;

	// default animation channel (channel 1)
	char szMotionName[128];				///< Name of current motion
	char szNextMotionName[128];			///< Name of next motion in queue
	char szDefaultMotionName[128];		///< Default motion for this actor
	char szBlendMotionName[128];
	float BlendAmount;
	bool BlendFlag;
	char szBlendNextMotionName[128];
	float BlendNextAmount;
	bool BlendNextFlag;
	bool HoldAtEnd;
	bool TransitionFlag;

#if 0
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
#endif

	int CollDetLevel;
	bool slide;
	bool fAutoStepUp;							///< Actor auto step up flag
	geFloat MaxStepHeight;						///< Actors max step height, in world units
	geActor *Vehicle;							///< If riding on anything
	geActor *Passengers[PASSENGER_LIST_SIZE];	///< If vehicle, riders
	int ActorType;								///< Actor type
	bool NeedsNewBB;							///< TRUE means needs new EXTBOX
	CPersistentAttributes *Inventory;			///< Actor's inventory
	struct ActorInstanceList *pNext;			///< Next actor instance in list

/* 07/15/2004 Wendell Buckner
	BUG FIX - Bone Collisions fail because we expect to hit the bone immediately after hitting the
	overall bounding box. So tag the actor as being hit at the bounding box level and after that check ONLY
	the bone bounding boxes until the whatever hit the overall bounding box no longer exists. */
	CollideObjectInformation* CollideObjects[COLMaxBBox];
	char LastBoneHit[256];						///< Name of last bone collided, if any
};

/************************************************************************************//**
 * @brief This struct is the main database element for the actor list
 ****************************************************************************************/
struct LoadedActorList
{
	geActor_Def			*theActorDef[4];	///< Actor definition
	geActor				*Actor[4];
	geBitmap			*Bitmap;
	geVec3d				BaseRotation;		///< Baseline rotation
	geFloat				BaseScale;			///< Baseline scale
	geFloat				BaseHealth;			///< Baseline health
	char				*szFilename;		///< Filename actor was from
	char				*szDefaultMotion;	///< Default motion for actors
	int					ActorType;			///< Type of actor this is
	int					InstanceCount;		///< # of actors in instance list
	ActorInstanceList	*IList;				///< List of instance for this actor
};


/************************************************************************************//**
 *
 * @brief CActorManager Class
 *
 ****************************************************************************************/
class CActorManager : public CRGFComponent
{
public:
	CActorManager();					///< Default constructor
	~CActorManager();					///< Default destructor

	// -----------------------------------------------------------------------------------
	// COLLISION
	// -----------------------------------------------------------------------------------

	int SetCollide(const geActor *theActor);
	int SetNoCollide(const geActor *theActor);
	int SetColDetLevel(const geActor *theActor, int ColDetLevel);
	int GetColDetLevel(const geActor *theActor, int *ColDetLevel);

	// Set/Get bounding box
	void SetBBox(geActor *theActor, float SizeX, float SizeY, float SizeZ);
	void SetBoundingBox(const geActor *theActor, const char *Animation);
	int GetBoundingBox(const geActor *theActor, geExtBox *theBox);
	int SetBoxChange(const geActor *theActor, bool Flag); // allow bbox to change?

	// Actor-actor collision check
	geBoolean DoesBoxHitActor(const geVec3d &thePoint, const geExtBox &theBox, geActor **theActor);
	geBoolean DoesBoxHitActor(const geVec3d &thePoint, const geExtBox &theBox, geActor **theActor,
							  const geActor *ActorToExclude);


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

	geBoolean DoesRayHitActor(const geVec3d &OldPosition, const geVec3d &NewPosition,
								geActor **theActor,	const geActor *ActorToExclude,
								geFloat *Percent, geVec3d *Normal);

	geBoolean DoesRayHitActor(const geVec3d &OldPosition, const geVec3d &NewPosition,
								geActor **theActor, const geActor *ActorToExclude,
								geFloat *Percent, geVec3d *Normal,
								void *CollisionObject);

    geBoolean DidRayHitActor(const geVec3d &OldPosition, const geVec3d &NewPosition,
								geActor **theActor, const geActor *ActorToExclude,
								geFloat *Percent, geVec3d *Normal,
								void *CollisionObj);

    geBoolean SetLastBoneHit(const geActor *theActor, const char *LastBoneHit);
    char *GetLastBoneHit(const geActor *theActor);

	// -----------------------------------------------------------------------------------
	// LOAD
	// -----------------------------------------------------------------------------------

	/**
	 * @brief Load an actor from a file
	 */
	geActor *LoadActor(const char *szFilename, geActor *OldActor);

	/**
	 * @brief Spawn an actor w/ parms
	 */
	geActor *SpawnActor(const char *szFilename, const geVec3d &Position, const geVec3d &Rotation,
						const char *DefaultMotion, const char *CurrentMotion, geActor *OldActor);

	// -----------------------------------------------------------------------------------
	// REMOVE
	// -----------------------------------------------------------------------------------

	/**
	 * @brief Remove an actor
	 */
	int RemoveActor(const geActor *theActor);
	int RemoveActorCheck(const geActor *theActor);

	// -----------------------------------------------------------------------------------
	// ACTORTYPE
	// -----------------------------------------------------------------------------------

	/**
	 * @brief Set actor type
	 */
	int SetType(const geActor *theActor, int nType);

	/**
	 * @brief Get actor type
	 */
	int GetType(const geActor *theActor, int *nType);

	bool IsActor(const geActor *theActor);

	// -----------------------------------------------------------------------------------
	// POSITION/ROTATION
	// -----------------------------------------------------------------------------------

	// position/rotate an actor
	int Position(const geActor *theActor, const geVec3d &Position);
	int Rotate(const geActor *theActor, const geVec3d &Rotation);

	// Get position/rotation
	int GetPosition(const geActor *theActor, geVec3d *thePosition);
	int GetRotation(const geActor *theActor, geVec3d *theRotation);
	int GetRotate(const geActor *theActor, geVec3d *theRotation);

	// Set/Get Aligning Rotation
	int SetAligningRotation(const geActor *theActor, const geVec3d &Rotation);
	int GetAligningRotation(const geActor *theActor, geVec3d *Rotation);

	int AddTranslation(const geActor *theActor, const geVec3d &Amount);		///< Add translation to actor
	int AddRotation(const geActor *theActor, const geVec3d &Amount);		///< Add rotation to actor
	int RotateToFacePoint(const geActor *theActor, const geVec3d &Position);///< Rotate to face point

	// Turn actor LEFT/RIGHT
	int TurnLeft(const geActor *theActor, geFloat theAmount);
	int TurnRight(const geActor *theActor, geFloat theAmount);

	// Get Bone Position/Rotation
	int GetBonePosition(const geActor *theActor, const char *szBone, geVec3d *thePosition);
	int GetBoneRotation(const geActor *theActor, const char *szBone, geVec3d *theRotation);

	// -----------------------------------------------------------------------------------
	// FALLING
	// -----------------------------------------------------------------------------------

	geBoolean Falling(const geActor *theActor);						///< Is actor falling?
	int ReallyFall(const geActor *theActor);
	int CheckReallyFall(const geActor *theActor, const geVec3d &StartPos);

	// -----------------------------------------------------------------------------------
	// ORIENTATION
	// -----------------------------------------------------------------------------------

	// Get UP/IN/LEFT vector
	int UpVector(const geActor *theActor, geVec3d *UpVector);		///< Get UP vector
	int InVector(const geActor *theActor, geVec3d *InVector);		///< Get IN vector
	int LeftVector(const geActor *theActor, geVec3d *LeftVector);	///< Get LEFT vector

	// -----------------------------------------------------------------------------------
	// STEP UP
	// -----------------------------------------------------------------------------------

	int SetAutoStepUp(const geActor *theActor, bool fEnable);		///< Set actor auto step up
	int SetStepHeight(const geActor *theActor, geFloat MaxStep);	///< Set actor step-up height
	int GetStepHeight(const geActor *theActor, geFloat *MaxStep);

	// -----------------------------------------------------------------------------------
	// MOVING
	// -----------------------------------------------------------------------------------

	void SetMoving(const geActor *theActor);
	bool GetMoving(const geActor *theActor);

	int MoveForward(const geActor *theActor, geFloat Speed);		///< Move actor forward
	int MoveBackward(const geActor *theActor, geFloat Speed);		///< Move actor backward
	int MoveLeft(const geActor *theActor, geFloat Speed);			///< Move actor left
	int MoveRight(const geActor *theActor, geFloat Speed);			///< Move actor right
	// Move actor to nowhere (to a galaxy far, far away...)
	int MoveAway(const geActor *theActor);

	// -----------------------------------------------------------------------------------
	// ANIMATION METHODS
	// -----------------------------------------------------------------------------------

	int SetAnimationSpeed(const geActor *theActor, geFloat Speed);		// Set animation speed

	char *GetMotion(const geActor *theActor);
	int SetMotion(const geActor *theActor, const char *MotionName, int Channel = ANIMATION_CHANNEL_ROOT);		///< Set actor motion
	int SetNextMotion(const geActor *theActor, const char *MotionName, int Channel = ANIMATION_CHANNEL_ROOT);	///< Prepare next motion
	int SetDefaultMotion(const geActor *theActor, const char *MotionName, int Channel = ANIMATION_CHANNEL_ROOT);///< Set default motion
	int ClearMotionToDefault(const geActor *theActor, int Channel = ANIMATION_CHANNEL_ROOT);				///< Force motion to default motion

	/**
	 * @brief creates a new pose from 2 animations, keeps existing animation timing (if mid-animation)
	 */
	int SetBlendMot(const geActor *theActor, const char *name1, const char *name2,
					float Amount, int Channel = ANIMATION_CHANNEL_ROOT);
	/**
	 * @brief creates a new pose from 2 animations, CLEARS animation timing
	 */
	int SetBlendMotion(const geActor *theActor, const char *name1, const char *name2,
							float Amount, int Channel = ANIMATION_CHANNEL_ROOT);
	int SetBlendNextMotion(const geActor *theActor, const char *name1, const char *name2,
							float Amount, int Channel = ANIMATION_CHANNEL_ROOT);

	void SetHoldAtEnd(const geActor *theActor, bool State, int Channel = ANIMATION_CHANNEL_ROOT);
	bool EndAnimation(const geActor *theActor, int Channel = ANIMATION_CHANNEL_ROOT);

	// start multiplayer
	void SetAnimationTime(const geActor *theActor, float time, int Channel = ANIMATION_CHANNEL_ROOT);
	// end multiplayer
	float GetAnimationTime(const geActor *theActor, int Channel = ANIMATION_CHANNEL_ROOT);

	geMotion *GetpMotion(const geActor *theActor, int Channel = ANIMATION_CHANNEL_ROOT);
	geMotion *GetpBMotion(const geActor *theActor, int Channel = ANIMATION_CHANNEL_ROOT);

	float GetBlendAmount(const geActor *theActor, int Channel = ANIMATION_CHANNEL_ROOT);
	int CheckAnimation(const geActor *theActor, const char *Animation, int Channel = ANIMATION_CHANNEL_ROOT);

	int SetTransitionMotion(const geActor *theActor, const char *name1, float Amount,
							const char *name2, int Channel = ANIMATION_CHANNEL_ROOT);
	bool CheckTransitionMotion(const geActor *theActor, const char *name1,
								int Channel = ANIMATION_CHANNEL_ROOT);
	float GetStartTime(const geActor *theActor, int Channel = ANIMATION_CHANNEL_ROOT);
	bool GetTransitionFlag(const geActor *theActor, int Channel = ANIMATION_CHANNEL_ROOT);

	// -----------------------------------------------------------------------------------
	// LIGHTING
	// -----------------------------------------------------------------------------------

	// Actor lighting control, Oh Joy.
	int SetActorDynamicLighting(const geActor *theActor, const GE_RGBA &FillColor,
								const GE_RGBA &AmbientColor, geBoolean AmbientLightFromFloor);
	int GetActorDynamicLighting(const geActor *theActor, GE_RGBA *FillColor,
								GE_RGBA *AmbientColor, geBoolean *AmbientLightFromFloor);
	int ResetActorDynamicLighting(const geActor *theActor);

	// -----------------------------------------------------------------------------------
	// FORCE
	// -----------------------------------------------------------------------------------

	// It's possible to add a FORCE to an actor.  This FORCE (seperate from gravity)
	// ..effects an actors translation over time.
	int SetForce(const geActor *theActor, int nForceNumber, const geVec3d &fVector,
					geFloat InitialValue, geFloat Decay);
	int GetForce(const geActor *theActor, int nForceNumber, geVec3d *ForceVector,
					geFloat *ForceLevel, geFloat *ForceDecay);

	int RemoveForce(const geActor *theActor, int nForceNumber);
	geBoolean ForceActive(const geActor *theActor, int nForceNumber);
	void SetForceEnabled(const geActor *theActor, bool enable);

	// -----------------------------------------------------------------------------------
	// ZONE
	// -----------------------------------------------------------------------------------

	// Functions for probing the actors environment
	int GetActorZone(const geActor *theActor, int *ZoneType);				///< Get actor's zone
	int GetActorOldZone(const geActor *theActor, int *ZoneType);
	Liquid *GetLiquid(const geActor *theActor);

	// -----------------------------------------------------------------------------------
	// ENTITYNAME
	// -----------------------------------------------------------------------------------

	int SetEntityName(const geActor *theActor, const char *name);
	const char *GetEntityName(const geActor *theActor);
	geActor *GetByEntityName(const char *name);

	// -----------------------------------------------------------------------------------
	// SCALE
	// -----------------------------------------------------------------------------------

	// Important note: to set the BASELINE for scale, etc. for all actors
	// ..of a specific type use nHandle = (-1).
	int SetScale(const geActor *theActor, geFloat Scale);			///< Scale actor
	int GetScale(const geActor *theActor, geFloat *Scale);			///< Get actor scale (max of x,y,z)
	int SetScaleX(const geActor *theActor, geFloat Scale);
	int SetScaleY(const geActor *theActor, geFloat Scale);
	int SetScaleZ(const geActor *theActor, geFloat Scale);
	int SetScaleXYZ(const geActor *theActor, const geVec3d &Scale);

	int GetScaleX(const geActor *theActor, geFloat *Scale);
	int GetScaleY(const geActor *theActor, geFloat *Scale);
	int GetScaleZ(const geActor *theActor, geFloat *Scale);
	int GetScaleXYZ(const geActor *theActor, geVec3d *Scale);

	// -----------------------------------------------------------------------------------
	// ALPHA
	// -----------------------------------------------------------------------------------

	int SetAlpha(const geActor *theActor, geFloat Alpha);		///< Set actor alpha
	int GetAlpha(const geActor *theActor, geFloat *Alpha);		///< Get actor alpha

	// -----------------------------------------------------------------------------------
	// HEALTH
	// -----------------------------------------------------------------------------------

	int SetHealth(const geActor *theActor, geFloat Health);		///< Set actor's health
	int ModifyHealth(const geActor *theActor, geFloat Amount);	///< Modify actor's health
	int GetHealth(const geActor *theActor, geFloat *Health);	///< Get actor's health

	// -----------------------------------------------------------------------------------
	// GRAVITY
	// -----------------------------------------------------------------------------------

	int SetGravity(const geActor *theActor, geVec3d Gravity);	///< Set gravity for actor
	int GetGravity(const geActor *theActor, geVec3d *Gravity);	///< Get gravity
	int SetGravityTime(const geActor *theActor, geFloat Gravitytime);
	int GetGravityCollision(const geActor *theActor, GE_Collision *Collision);

	// -----------------------------------------------------------------------------------
	// SPATIAL RELATION
	// -----------------------------------------------------------------------------------

	/**
	 * @brief Is actor in front of me?
	 */
	bool IsInFrontOf(const geActor *theActor, const geActor *theOtherActor);
	/**
	 * @brief Get distance between 2 actors
	 */
	geFloat DistanceFrom(const geActor *theActor, const geActor *theOtherActor);
	/**
	 * @brief Get distance between a point and an actor
	 */
	geFloat DistanceFrom(const geVec3d &Point, const geActor *theActor);
	/**
	 * @ brief Fill a list with pointers to all actors within a specific range of a particular point
	 */
	int GetActorsInRange(const geVec3d &Point, geFloat Range, int nListSize, geActor **ActorList);

	// -----------------------------------------------------------------------------------
	// VEHICLE/PASSENGER
	// -----------------------------------------------------------------------------------

	// Actor on model stuff
	void SetVehicle(const geActor *theActor, geActor *theVehicle);
	geActor *GetVehicle(const geActor *theActor);
	int SetPassenger(geActor *thePassenger, geActor *theVehicle);
	int RemovePassenger(const geActor *thePassenger);

	// -----------------------------------------------------------------------------------
	// INVENTORY
	// -----------------------------------------------------------------------------------

	/**
	 * @brief Inventory list access
	 */
	CPersistentAttributes *Inventory(const geActor *theActor);

	// -----------------------------------------------------------------------------------
	// TICK
	// -----------------------------------------------------------------------------------

	// Dispatch time to all actors
	void Tick(geFloat dwTicks);			///< Process passage of time

	// -----------------------------------------------------------------------------------
	// TILT
	// -----------------------------------------------------------------------------------

	int SetTilt(const geActor *theActor, bool Flag);
	int TiltUp(const geActor *theActor, geFloat theAmount);
	int TiltDown(const geActor *theActor, geFloat theAmount);
	float GetTiltX(const geActor *theActor);

	// -----------------------------------------------------------------------------------
	// RADAR
	// -----------------------------------------------------------------------------------

	int SetHideRadar(const geActor *theActor, bool flag);
	int GetHideRadar(const geActor *theActor, bool *flag);

	// -----------------------------------------------------------------------------------
	// GROUP
	// -----------------------------------------------------------------------------------

	int SetGroup(const geActor *theActor, const char *name);
	const char *GetGroup(const geActor *theActor);

	// -----------------------------------------------------------------------------------
	// SHADOWS
	// -----------------------------------------------------------------------------------

	// BITMAP SHADOW
	/**
	 * @brief Set the size of the actor's shadow bitmap;
	 */
	int SetShadow(const geActor *theActor, geFloat Size);
	/**
	 * @brief Set the alpha of the actor's shadow bitmap
	 */
	int SetShadowAlpha(const geActor *theActor, geFloat Alpha);
	int SetShadowBitmap(const geActor *theActor, geBitmap *Bitmap);
	// PROJECTED SHADOW
	/**
	 * @brief Enable/Disable projected shadows for a particular actor
	 */
	int SetProjectedShadows(const geActor *theActor, bool flag);
	// STENCIL SHADOW
	/**
	 * @brief Enable/Disable stencil shadows for a particular actor
	 */
	geBoolean SetStencilShadows(const geActor *theActor, geBoolean flag);

	// -----------------------------------------------------------------------------------
	// ATTACH/DETACH
	// -----------------------------------------------------------------------------------

	/**
	 * @brief Attach a slave actor to a master actor
	 *
	 * The slave actor is attached from the bone SlaveBoneName to the bone
	 * MasterBoneName of the master actor. The offset AttachOffset is applied
	 * to the attachment point and is calculated in the direction the master
	 * actor is facing. The rotation Angle is applied to the base rotation of
	 * the slave actor. The slave actor is initially aligned to face the same
	 * direction as the master actor, before the rotations are applied. The
	 * slave actor will rotate to follow the master actor's rotation. The slave
	 * actor will move with the master actor to maintain its attachment.
	 */
	void ActorAttach(geActor *Slave, const char *SlaveBoneName,
					 const geActor *Master, const char *MasterBoneName,
					 const geVec3d &AttachOffset, const geVec3d &Angle);

	/**
	 * @brief Detach a slave actor from its master actor
	 */
	void DetachFromActor(const geActor *Master, const geActor *Slave);

	/**
	 * @brief Attach a slave actor to a master actor, using the master's animation
	 */
	void ActorAttachBlend(geActor *Slave, const geActor *Master);

	/**
	 * @brief Detach a blending slave actor from its master actor
	 */
	void DetachBlendFromActor(const geActor *Master, const geActor *Slave);

private:
	/**
	 * @brief Detach a slave actor from its master when the master actor gets removed
	 */
	void ActorDetach(const geActor *Slave);

public:

	// -----------------------------------------------------------------------------------
	// LOD
	// -----------------------------------------------------------------------------------

	/**
	 * @brief Set level of detail distances of a particular actor
	 */
	int SetLODdistance(const geActor *theActor, float LOD1, float LOD2, float LOD3,
						float LOD4, float LOD5);
	/**
	 * @brief Get the current level of detail of a particular actor
	 */
	int GetLODLevel(const geActor *theActor, int *Level);

	// -----------------------------------------------------------------------------------
	// MISC METHODS
	// -----------------------------------------------------------------------------------

	int SetSlide(const geActor *theActor, bool Flag);
	int GetAnimationHeight(const geActor *theActor, const char *Animation, float *Height);
	int SetAnimationHeight(const geActor *theActor, const char *Animation, bool Camera);
	geBoolean ValidateMove(const geVec3d &StartPos, const geVec3d &EndPos,
							const geActor *theActor, bool slide);
	int CountActors();
	int ChangeMaterial(const geActor *theActor, const char *Change);
	int SetRoot(const geActor *theActor, const char *BoneName);
	int SetActorFlags(const geActor *theActor, int Flag);


private:
	// -----------------------------------------------------------------------------------
	// Private member functions
	// -----------------------------------------------------------------------------------

	geActor *AddNewInstance(LoadedActorList *theEntry, geActor *OldActor);
	int  RemoveInstance(const geActor *theActor);		///< Delete instance of actor
	void RemoveAllActors(LoadedActorList *theEntry);

	ActorInstanceList *LocateInstance(const geActor *theActor, const LoadedActorList *theEntry);
	ActorInstanceList *LocateInstance(const geActor *theActor);

	void AdvanceInstanceTime(ActorInstanceList *theEntry,	geFloat dwTicks);
	void TimeAdvanceAllInstances(LoadedActorList *theEntry,	geFloat dwTicks);
	void UpdateActorState(ActorInstanceList *theEntry);

	void ProcessGravity(ActorInstanceList *theEntry, geFloat dwTicks);
	void ProcessForce(ActorInstanceList *theEntry, geFloat dwTicks);
	int Move(ActorInstanceList *pEntry, int nHow, geFloat fSpeed);		///< Velocity-based motion for actor
	bool CheckForStepUp(ActorInstanceList *pEntry, geVec3d NewPosition);
	int GetCurrentZone(ActorInstanceList *pEntry);	// Get brush zone types
	geBoolean ValidateMotion(geVec3d StartPos, geVec3d EndPos,
		ActorInstanceList *pEntry, bool fStepUpOK, bool slide);			///< Validate and handle motion

	void MoveAllVehicles(LoadedActorList *theEntry, float dwTicks);		///< Move vehicle actors
	int TranslateAllPassengers(ActorInstanceList *pEntry);

private:
	// -----------------------------------------------------------------------------------
	// Private member variables
	// -----------------------------------------------------------------------------------

	LoadedActorList *MainList[ACTOR_LIST_SIZE];	///< Database of loaded actors
	int m_GlobalInstanceCount;					///< Level instance counter
	CIniFile AttrFile;
	bool ValidAttr;
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
