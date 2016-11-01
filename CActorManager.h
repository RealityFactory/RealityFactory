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

#include <hash_map>

class CPersistentAttributes;			// Forward ref.

// Setup defines
#define	PASSENGER_LIST_SIZE	64			///< Max # of passengers in a vehicle
#define ATTACH_ACTORS		16			///< Max # of attached actors
#define ATTACH_BLEND_ACTORS	16			///< Max # of attached blending actors

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
	geActor*	AttachedActor;
	geVec3d		Offset;
	bool		CollFlag;
	int			RenderFlag;
	char		MasterBone[256];
	char		SlaveBone[256];
};


struct WindowAttachment
{
	CEGUI::Window*	Window;
	float OffsetX, OffsetY;
};

/************************************************************************************//**
 * @brief This struct holds information for each instance of an actor
 ****************************************************************************************/
struct ActorInstance
{
	geActor*		Actor;				///< The actor itself
	geActor_Def*	theDef;				///< Pointer to loaded actor def
	geActor_Def*	theLODDef[3];		///< Actor definition
	geActor*		LODActor[3];		///< Level of detail actors
	geBitmap*		Bitmap;
	float			LODdistance[5];
	int				LODLevel;
	geVec3d			localTranslation;	///< Actor translation
	geVec3d			OldTranslation;		///< Actors previous position
	geVec3d			localRotation;		///< Actor rotation
	geVec3d			BaseRotation;		///< Actor baseline rotation

	bool			AllowTilt;				///< allow actor to tilt up/down
	float			TiltX;
	bool			BoxChange;				///< allow BB to change
	bool			NoCollision;			///< allow no collision detection
	float			ActorAnimationSpeed;	///< Speed of actor animation
	geVec3d			Scale;					///< Actors current XYZ scale
	float			Health;					///< Actors health, 0 = destroyed
	geVec3d			Gravity;				///< Gravity acting on actor
	float			GravityTime;			///< Time gravity has been effective (for falling)
	float			StepHeight;				///< Max. Y delta for moving up during motion
	bool			ForceEnabled;
	geVec3d			ForceVector[4];			///< Current force vector acting on actor
	float			ForceLevel[4];			///< Current level force is acting at
	float			ForceDecay[4];			///< Decay speed of force, in units/second
	float			ShadowSize;				///< Size of bitmap shadow
	float			ShadowAlpha;			///< Alpha of shadow
	geBitmap*		ShadowBitmap;			///< Bitmap to use for shadow
	bool			ProjectedShadows;		///< flag to enable projected shadows for this actor
	bool			Moving;
	int				CurrentZone;
	int				OldZone;
	int				GravityCoeff;
	Liquid*			LQ;
	std::string		szEntityName;

	bool			Attached;
	geVec3d			MasterRotation;
	std::vector<Attachment>	AttachedActors;

	bool			AttachedB;
	std::vector<geActor*>	AttachedBlendActors;

	int				RenderFlag;
	geWorld_Model*	PassengerModel;
	float			StartTime;
	GE_Collision	GravityCollision;
	bool			HideRadar;
	std::string		Group;
	GE_RGBA			FillColor;
	GE_RGBA			AmbientColor;
	geBoolean		AmbientLightFromFloor;

	std::string		MotionName;				///< Name of current motion
	std::string		NextMotionName;			///< Name of next motion in queue
	std::string		DefaultMotionName;		///< Default motion for this actor
	std::string		BlendMotionName;
	std::string		BlendNextMotionName;
	float			BlendAmount;
	float			BlendNextAmount;
	bool			BlendFlag;
	bool			BlendNextFlag;
	bool			HoldAtEnd;
	bool			TransitionFlag;

	float			AnimationTime;			///< Time in current animation
	float			PrevAnimationTime;
	geMotion*		pMotion;
	geMotion*		pBMotion;

#if 0
	// support for multiple animation channels
	// each channel provides full support for blending, etc

	std::string		MotionName[ANIMATION_CHANNELS];			///< Name of current motion
	std::string		NextMotionName[ANIMATION_CHANNELS];		///< Name of next motion in queue
	std::string		DefaultMotionName[ANIMATION_CHANNELS];	///< Default motion for this actor
	std::string		BlendMotionName[ANIMATION_CHANNELS];
	std::string		BlendNextMotionName[ANIMATION_CHANNELS];
	float			BlendAmount[ANIMATION_CHANNELS];
	float			BlendNextAmount[ANIMATION_CHANNELS];
	bool			BlendFlag[ANIMATION_CHANNELS];
	bool			BlendNextFlag[ANIMATION_CHANNELS];
	bool			HoldAtEnd[ANIMATION_CHANNELS];
	bool			TransitionFlag[ANIMATION_CHANNELS];

	float			AnimationTime[ANIMATION_CHANNELS];		///< Time in current animation
	float			PrevAnimationTime[ANIMATION_CHANNELS];
	geMotion*		pMotion[ANIMATION_CHANNELS];
	geMotion*		pBMotion[ANIMATION_CHANNELS];
#endif

	int				CollDetLevel;
	bool			slide;
	bool			AutoStepUp;						///< Actor auto step up flag
	float			MaxStepHeight;					///< Actors max step height, in world units
	geActor*		Vehicle;						///< If riding on anything
	geActor*		Passengers[PASSENGER_LIST_SIZE];///< If vehicle, riders
	int				ActorType;						///< Actor type
	bool			NeedsNewBB;						///< TRUE means needs new EXTBOX
	CPersistentAttributes* Inventory;				///< Actor's inventory
	struct ActorInstance* pNext;					///< Next actor instance in list

/* 07/15/2004 Wendell Buckner
	BUG FIX - Bone Collisions fail because we expect to hit the bone immediately after hitting the
	overall bounding box. So tag the actor as being hit at the bounding box level and after that check ONLY
	the bone bounding boxes until the whatever hit the overall bounding box no longer exists. */
	CollideObjectInformation* CollideObjects[COLMaxBBox];
	std::string		LastBoneHit;					///< Name of last bone collided, if any
};

/************************************************************************************//**
 * @brief This struct is the main database element for the actor list
 ****************************************************************************************/
struct LoadedActorList
{
	geActor_Def*		ActorDef[4];		///< Actor definition
	geActor*			Actor[4];
	geBitmap*			Bitmap;
	ActorInstance*		IList;				///< List of instance for this actor
};

class CIniFile;

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

	int SetCollide(const geActor* actor);
	int SetNoCollide(const geActor* actor);
	int SetColDetLevel(const geActor* actor, int colDetLevel);
	int GetColDetLevel(const geActor* actor, int *colDetLevel);

	// Set/Get bounding box
	void SetBBox(geActor* actor, float sizeX, float sizeY, float sizeZ);
	void SetBoundingBox(const geActor* actor, const std::string& animation);
	int GetBoundingBox(const geActor* actor, geExtBox* box);
	int SetBoxChange(const geActor* actor, bool flag); // allow bbox to change?

	// Actor-actor collision check
	geBoolean DoesBoxHitActor(const geVec3d& point, const geExtBox& box, geActor** actor);
	geBoolean DoesBoxHitActor(const geVec3d& point, const geExtBox& box, geActor** actor,
							  const geActor* actorToExclude);


	/* 07/15/2004 Wendell Buckner
	BUG FIX - Bone Collisions fail because we expect to hit the bone immediately after hitting the
	overall bounding box. So tag the actor as being hit at the bounding box level and after that check ONLY
	the bone bounding boxes until the whatever hit the overall bounding box no longer exists. */
	CollideObjectInformation* AddCollideObject(CollideObjectLevels collideObjectLevel,
												ActorInstance* actorInstance,
												CollideObjectInformation* collideObject);

	geBoolean RemoveCollideObject(CollideObjectLevels collideObjectLevel,
									ActorInstance* actorInstance,
									void* collideObject);

	CollideObjectInformation* GetCollideObject(CollideObjectLevels collideObjectLevel,
												ActorInstance* actorInstance,
												void* collideObject);

	geBoolean DoesRayHitActor(const geVec3d& oldPosition, const geVec3d& newPosition,
								geActor** actor, const geActor* actorToExclude,
								float* percent, geVec3d* normal);

	geBoolean DoesRayHitActor(const geVec3d& oldPosition, const geVec3d& newPosition,
								geActor** actor, const geActor* actorToExclude,
								float* percent, geVec3d* normal,
								void* collisionObject);

	geBoolean DidRayHitActor(const geVec3d& oldPosition, const geVec3d& newPosition,
								geActor** actor, const geActor* actorToExclude,
								float* percent, geVec3d* normal,
								void* collisionObj);

	geBoolean SetLastBoneHit(const geActor* actor, const std::string& lastBoneHit);
	std::string GetLastBoneHit(const geActor* actor);

	// -----------------------------------------------------------------------------------
	// LOAD
	// -----------------------------------------------------------------------------------

	/**
	 * @brief Load an actor from a file
	 */
	geActor* LoadActor(const std::string& filename, geActor* oldActor);

	/**
	 * @brief Spawn an actor w/ parms
	 */
	geActor* SpawnActor(const std::string& filename, const geVec3d& position, const geVec3d& rotation,
						const std::string& defaultMotion, const std::string& currentMotion, geActor* oldActor);

	// -----------------------------------------------------------------------------------
	// REMOVE
	// -----------------------------------------------------------------------------------

	/**
	 * @brief Remove an actor
	 */
	int RemoveActor(const geActor* actor);
	int RemoveActorCheck(const geActor* actor);

	// -----------------------------------------------------------------------------------
	// ACTORTYPE
	// -----------------------------------------------------------------------------------

	/**
	 * @brief Set actor type
	 */
	int SetType(const geActor* actor, int type);

	/**
	 * @brief Get actor type
	 */
	int GetType(const geActor* actor, int* type);

	bool IsActor(const geActor* actor);

	// -----------------------------------------------------------------------------------
	// POSITION/ROTATION
	// -----------------------------------------------------------------------------------

	/**
	 * @brief Position actor
	 */
	int Position(const geActor* actor, const geVec3d& position);

	/**
	 * @brief Rotate actor
	 */
	int Rotate(const geActor* actor, const geVec3d& rotation);

	// Get position/rotation
	int GetPosition(const geActor* actor, geVec3d* position);
	int GetRotation(const geActor* actor, geVec3d* rotation);
	int GetRotate(const geActor* actor, geVec3d* rotation);

	// Set/Get Aligning Rotation
	int SetAligningRotation(const geActor* actor, const geVec3d& rotation);
	int GetAligningRotation(const geActor* actor, geVec3d* rotation);

	int AddTranslation(const geActor* actor, const geVec3d& amount);		///< Add translation to actor
	int AddRotation(const geActor* actor, const geVec3d& amount);			///< Add rotation to actor
	int RotateToFacePoint(const geActor* actor, const geVec3d& position);	///< Rotate to face point

	// Turn actor LEFT/RIGHT
	int TurnLeft(const geActor* actor, float amount);
	int TurnRight(const geActor* actor, float amount);

	// Get Bone Position/Rotation
	int GetBonePosition(const geActor* actor, const std::string& bone, geVec3d* position);
	int GetBoneRotation(const geActor* actor, const std::string& bone, geVec3d* rotation);

	// -----------------------------------------------------------------------------------
	// FALLING
	// -----------------------------------------------------------------------------------

	geBoolean Falling(const geActor* actor);						///< Is actor falling?
	int ReallyFall(const geActor* actor);
	int CheckReallyFall(const geActor* actor, const geVec3d& startPos);

	// -----------------------------------------------------------------------------------
	// ORIENTATION
	// -----------------------------------------------------------------------------------

	// Get UP/IN/LEFT vector
	int UpVector(const geActor* actor, geVec3d* upVector);		///< Get UP vector
	int InVector(const geActor* actor, geVec3d* inVector);		///< Get IN vector
	int LeftVector(const geActor* actor, geVec3d* leftVector);	///< Get LEFT vector

	// -----------------------------------------------------------------------------------
	// STEP UP
	// -----------------------------------------------------------------------------------

	int SetAutoStepUp(const geActor* actor, bool enable);		///< Set actor auto step up
	int SetStepHeight(const geActor* actor, float maxStep);		///< Set actor step-up height
	int GetStepHeight(const geActor* actor, float* maxStep);	///< Get actor step-up height

	// -----------------------------------------------------------------------------------
	// MOVING
	// -----------------------------------------------------------------------------------

	void SetMoving(const geActor* actor);
	bool GetMoving(const geActor* actor);

	int MoveForward(const geActor* actor, float speed);			///< Move actor forward
	int MoveBackward(const geActor* actor, float speed);		///< Move actor backward
	int MoveLeft(const geActor* actor, float speed);			///< Move actor left
	int MoveRight(const geActor* actor, float speed);			///< Move actor right
	// Move actor to nowhere (to a galaxy far, far away...)
	int MoveAway(const geActor* actor);

	// -----------------------------------------------------------------------------------
	// ANIMATION METHODS
	// -----------------------------------------------------------------------------------

	int SetAnimationSpeed(const geActor* actor, float speed);		///< Set animation speed

	std::string GetMotion(const geActor* actor);

	// TODO: support for animation channels
	/**
	 * @brief Set actor motion
	 */
	int SetMotion(const geActor* actor, const std::string& motionName, int channel = ANIMATION_CHANNEL_ROOT);

	/**
	 * @brief Prepare next motion
	 */
	int SetNextMotion(const geActor* actor, const std::string& motionName, int channel = ANIMATION_CHANNEL_ROOT);

	/**
	 * @brief Set default motion
	 */
	int SetDefaultMotion(const geActor* actor, const std::string& motionName, int channel = ANIMATION_CHANNEL_ROOT);

	/**
	 * @brief Force motion to default motion
	 */
	int ClearMotionToDefault(const geActor* actor, int channel = ANIMATION_CHANNEL_ROOT);

	/**
	 * @brief creates a new pose from 2 animations, keeps existing animation timing (if mid-animation)
	 */
	int SetBlendMot(const geActor* actor, const std::string& name1, const std::string& name2,
					float amount, int channel = ANIMATION_CHANNEL_ROOT);

	/**
	 * @brief creates a new pose from 2 animations, CLEARS animation timing
	 */
	int SetBlendMotion(const geActor* actor, const std::string& name1, const std::string& name2,
							float amount, int channel = ANIMATION_CHANNEL_ROOT);
	int SetBlendNextMotion(const geActor* actor, const std::string& name1, const std::string& name2,
							float amount, int channel = ANIMATION_CHANNEL_ROOT);

	void SetHoldAtEnd(const geActor* actor, bool state, int channel = ANIMATION_CHANNEL_ROOT);
	bool EndAnimation(const geActor* actor, int channel = ANIMATION_CHANNEL_ROOT);

	// start multiplayer
	void SetAnimationTime(const geActor* actor, float time, int channel = ANIMATION_CHANNEL_ROOT);
	// end multiplayer
	float GetAnimationTime(const geActor* actor, int channel = ANIMATION_CHANNEL_ROOT);

	geMotion* GetpMotion(const geActor* actor, int channel = ANIMATION_CHANNEL_ROOT);
	geMotion* GetpBMotion(const geActor* actor, int channel = ANIMATION_CHANNEL_ROOT);

	float GetBlendAmount(const geActor* actor, int channel = ANIMATION_CHANNEL_ROOT);
	int CheckAnimation(const geActor* actor, const std::string& animation, int channel = ANIMATION_CHANNEL_ROOT);

	int SetTransitionMotion(const geActor* actor, const std::string& name1, float amount,
							const std::string& name2, int channel = ANIMATION_CHANNEL_ROOT);
	bool CheckTransitionMotion(const geActor* actor, const std::string& name1,
								int channel = ANIMATION_CHANNEL_ROOT);
	float GetStartTime(const geActor* actor, int channel = ANIMATION_CHANNEL_ROOT);
	bool GetTransitionFlag(const geActor* actor, int channel = ANIMATION_CHANNEL_ROOT);

	// -----------------------------------------------------------------------------------
	// LIGHTING
	// -----------------------------------------------------------------------------------

	// Actor lighting control, Oh Joy.
	int SetActorDynamicLighting(const geActor* actor, const GE_RGBA& fillColor,
								const GE_RGBA& ambientColor, geBoolean ambientLightFromFloor);
	int GetActorDynamicLighting(const geActor* actor, GE_RGBA* fillColor,
								GE_RGBA* ambientColor, geBoolean* ambientLightFromFloor);
	int ResetActorDynamicLighting(const geActor* actor);

	// -----------------------------------------------------------------------------------
	// FORCE
	// -----------------------------------------------------------------------------------

	// It's possible to add a FORCE to an actor.  This FORCE (seperate from gravity)
	// ..effects an actors translation over time.
	int SetForce(const geActor* actor, int forceNumber, const geVec3d& forceVector,
					float forceLevel, float forceDecay);

	int GetForce(const geActor* actor, int forceNumber, geVec3d* forceVector,
					float* forceLevel, float* forceDecay);

	int RemoveForce(const geActor* actor, int forceNumber);
	geBoolean ForceActive(const geActor* actor, int forceNumber);

	void SetForceEnabled(const geActor* actor, bool enable);

	// -----------------------------------------------------------------------------------
	// ZONE
	// -----------------------------------------------------------------------------------

	// Functions for probing the actors environment
	int GetActorZone(const geActor* actor, int* zoneType);				///< Get actor's zone
	int GetActorOldZone(const geActor* actor, int* zoneType);
	Liquid* GetLiquid(const geActor* actor);

	// -----------------------------------------------------------------------------------
	// ENTITYNAME
	// -----------------------------------------------------------------------------------

	int SetEntityName(const geActor* actor, const std::string& name);
	std::string GetEntityName(const geActor* actor);
	geActor* GetByEntityName(const char* name);

	// -----------------------------------------------------------------------------------
	// SCALE
	// -----------------------------------------------------------------------------------

	// Important note: to set the BASELINE for scale, etc. for all actors
	// ..of a specific type use nHandle = (-1).
	int SetScale(const geActor* actor, float scale);			///< Scale actor
	int GetScale(const geActor* actor, float *scale);			///< Get actor scale (max of x,y,z)

	int SetScaleX(const geActor* actor, float scale);
	int SetScaleY(const geActor* actor, float scale);
	int SetScaleZ(const geActor* actor, float scale);
	int SetScaleXYZ(const geActor* actor, const geVec3d& scale);

	int GetScaleX(const geActor* actor, float* scale);
	int GetScaleY(const geActor* actor, float* scale);
	int GetScaleZ(const geActor* actor, float* scale);
	int GetScaleXYZ(const geActor* actor, geVec3d* scale);

	// -----------------------------------------------------------------------------------
	// ALPHA
	// -----------------------------------------------------------------------------------

	int SetAlpha(const geActor* actor, float alpha);		///< Set actor alpha
	int GetAlpha(const geActor* actor, float* alpha);		///< Get actor alpha

	// -----------------------------------------------------------------------------------
	// HEALTH
	// -----------------------------------------------------------------------------------

	int SetHealth(const geActor* actor, float health);		///< Set actor's health
	int ModifyHealth(const geActor* actor, float amount);	///< Modify actor's health
	int GetHealth(const geActor* actor, float* health);		///< Get actor's health

	// -----------------------------------------------------------------------------------
	// GRAVITY
	// -----------------------------------------------------------------------------------

	int SetGravity(const geActor* actor, geVec3d gravity);	///< Set gravity for actor
	int GetGravity(const geActor* actor, geVec3d* gravity);	///< Get gravity

	int SetGravityTime(const geActor* actor, float gravityTime);
	int GetGravityCollision(const geActor* actor, GE_Collision* collision);

	// -----------------------------------------------------------------------------------
	// SPATIAL RELATION
	// -----------------------------------------------------------------------------------

	/**
	 * @brief Is actor in front of me?
	 */
	bool IsInFrontOf(const geActor* actor, const geActor* otherActor);

	/**
	 * @brief Get distance between 2 actors
	 */
	float DistanceFrom(const geActor* actor, const geActor* otherActor);

	/**
	 * @brief Get distance between a point and an actor
	 */
	float DistanceFrom(const geVec3d& point, const geActor* actor);

	/**
	 * @ brief Fill a list with pointers to all actors within a specific range of a particular point
	 */
	int GetActorsInRange(const geVec3d& point, float range, int listSize, geActor** actorList);

	// -----------------------------------------------------------------------------------
	// VEHICLE/PASSENGER
	// -----------------------------------------------------------------------------------

	// Actor on model stuff
	void SetVehicle(const geActor* actor, geActor* vehicle);
	geActor* GetVehicle(const geActor* actor);
	int SetPassenger(geActor* passenger, geActor* vehicle);
	int RemovePassenger(const geActor* passenger);

	// -----------------------------------------------------------------------------------
	// INVENTORY
	// -----------------------------------------------------------------------------------

	/**
	 * @brief Inventory list access
	 */
	CPersistentAttributes* Inventory(const geActor* actor);

	// -----------------------------------------------------------------------------------
	// UPDATE
	// -----------------------------------------------------------------------------------

	// Dispatch time to all actors
	void Update(float timeElapsed);			///< Process passage of time

	// -----------------------------------------------------------------------------------
	// TILT
	// -----------------------------------------------------------------------------------

	int SetTilt(const geActor* actor, bool flag);
	int TiltUp(const geActor* actor, float amount);
	int TiltDown(const geActor* actor, float amount);
	float GetTiltX(const geActor* actor);

	// -----------------------------------------------------------------------------------
	// RADAR
	// -----------------------------------------------------------------------------------

	int SetHideRadar(const geActor* actor, bool flag);
	int GetHideRadar(const geActor* actor, bool* flag);

	// -----------------------------------------------------------------------------------
	// GROUP
	// -----------------------------------------------------------------------------------

	int SetGroup(const geActor* actor, const std::string& name);
	std::string GetGroup(const geActor* actor);

	// -----------------------------------------------------------------------------------
	// SHADOWS
	// -----------------------------------------------------------------------------------

	// BITMAP SHADOW
	/**
	 * @brief Set the size of the actor's shadow bitmap;
	 */
	int SetShadow(const geActor* actor, float size);

	/**
	 * @brief Set the alpha of the actor's shadow bitmap
	 */
	int SetShadowAlpha(const geActor* actor, float alpha);

	/**
	 * @brief Set the bitmap used for the actor's shadow
	 */
	int SetShadowBitmap(const geActor* actor, geBitmap* bitmap);

	// PROJECTED SHADOW
	/**
	 * @brief Enable/Disable projected shadows for a particular actor
	 */
	int SetProjectedShadows(const geActor* actor, bool flag);

	// STENCIL SHADOW
	/**
	 * @brief Enable/Disable stencil shadows for a particular actor
	 */
	geBoolean SetStencilShadows(const geActor* actor, geBoolean flag);

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
	void ActorAttach(geActor* slave, const char* slaveBoneName,
					 const geActor* master, const char* masterBoneName,
					 const geVec3d& attachOffset, const geVec3d& angle);

	/**
	 * @brief Detach a slave actor from its master actor
	 */
	void DetachFromActor(const geActor* master, const geActor* slave);

	/**
	 * @brief Attach a slave actor to a master actor, using the master's animation
	 */
	void ActorAttachBlend(geActor* slave, const geActor* master);

	/**
	 * @brief Detach a blending slave actor from its master actor
	 */
	void DetachBlendFromActor(const geActor* master, const geActor* slave);

private:
	/**
	 * @brief Detach a slave actor from its master when the master actor gets removed
	 */
	void ActorDetach(const geActor* slave);

public:

	// -----------------------------------------------------------------------------------
	// LOD
	// -----------------------------------------------------------------------------------

	/**
	 * @brief Set level of detail distances of a particular actor
	 */
	int SetLODdistance(const geActor* actor, float LOD1, float LOD2, float LOD3,
						float LOD4, float LOD5);
	/**
	 * @brief Get the current level of detail of a particular actor
	 */
	int GetLODLevel(const geActor* actor, int* level);

	// -----------------------------------------------------------------------------------
	// MISC METHODS
	// -----------------------------------------------------------------------------------

	int SetSlide(const geActor* actor, bool flag);
	int GetAnimationHeight(const geActor* actor, const std::string& animation, float* height);
	int SetAnimationHeight(const geActor* actor, const std::string& animation, bool camera);
	geBoolean ValidateMove(const geVec3d& startPos, const geVec3d& endPos,
							const geActor* actor, bool slide);
	int CountActors() const;
	int ChangeMaterial(const geActor* actor, const std::string& change);
	int SetRoot(const geActor* actor, const std::string& boneName);
	int SetActorFlags(const geActor* actor, int flag);

private:
	// -----------------------------------------------------------------------------------
	// Private member functions
	// -----------------------------------------------------------------------------------

	geActor* AddNewInstance(LoadedActorList* entry, geActor* oldActor);
	int  RemoveInstance(const geActor* actor);			///< Delete instance of actor
	void RemoveAllActors(LoadedActorList* entry);

	inline ActorInstance* LocateInstance(const geActor* actor);

	void AdvanceInstanceTime(ActorInstance* entry, float timeElapsed);
	void TimeAdvanceAllInstances(LoadedActorList* entry, float timeElapsed);
	void UpdateActorState(ActorInstance* entry);

	void ProcessGravity(ActorInstance* entry, float timeElapsed);
	void ProcessForce(ActorInstance* entry, float timeElapsed);

	int Move(ActorInstance* entry, int how, float speed);			///< Velocity-based motion for actor
	bool CheckForStepUp(ActorInstance* entry, geVec3d newPosition);
	int GetCurrentZone(ActorInstance* entry);						///< Get brush zone types
	geBoolean ValidateMotion(geVec3d startPos, geVec3d endPos,
		ActorInstance* entry, bool stepUpOK, bool slide);			///< Validate and handle motion

	void MoveAllVehicles(LoadedActorList* entry, float timeElapsed);///< Move vehicle actors
	int TranslateAllPassengers(ActorInstance* entry);

private:
	// -----------------------------------------------------------------------------------
	// Private member variables
	// -----------------------------------------------------------------------------------

	stdext::hash_map<std::string, LoadedActorList*>		m_LoadedActorFiles;
	stdext::hash_map<const geActor*, ActorInstance*>	m_ActorLookUp;

	int m_InstanceCount;							///< Level instance counter
	CIniFile* m_MaterialIniFile;
	bool m_ValidIniFile;
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
