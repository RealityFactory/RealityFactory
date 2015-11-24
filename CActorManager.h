/*
ActorManager.h:		Actor Manager

  This file contains the class declaration of the Actor Manager
  system for the Reality factory.
  
	It's the Actor Manager's job to load, remove, translate,
	rotate, and scale all the actors in the system.  The Actor Manager
	maintains a database of "loaded actors" from which instances can
	be created.
*/

#ifndef __CACTOR_MANAGER__
#define __CACTOR_MANAGER__

class CPersistentAttributes;			// Forward ref.

//	Setup defines

#define	ACTOR_LIST_SIZE		1024		// Max # of actors per level
#define	PASSENGER_LIST_SIZE	64		// Max # of passengers in a vehicle

//	This struct holds information for each instance of an actor

struct ActorInstanceList
{
	geActor *Actor;								// The actor itself
	geActor_Def *theDef;					// Pointer to loaded actor def
	geVec3d localTranslation;			// Actor translation
	geVec3d OldTranslation;				// Actors previous position
	geVec3d localRotation;				// Actor rotation
	geVec3d BaseRotation;					// Actor baseline rotation
	geFloat AnimationTime;				// Time in current animation
	geFloat PrevAnimationTime;
	geMotion *pMotion;
	geMotion *pBMotion;
	bool AllowTilt;					// allow actor to tilt up/down
	geFloat TiltX;
	bool BoxChange;					// allow BB to change
	bool NoCollision;				// allow no collision detection
	geFloat ActorAnimationSpeed;	// Speed of actor animation
	geFloat Scale;								// Actors current scale
	geFloat Health;								// Actors health, 0 = destroyed
	geFloat Gravity;							// Gravity acting on actor
	geFloat GravityTime;					// Time gravity has been effective (for falling)
	geFloat StepHeight;						// Max. Y delta for moving up during motion
	geVec3d ForceVector[4];				// Current force vector acting on actor
	geFloat ForceLevel[4];				// Current level force is acting at
	geFloat ForceDecay[4];				// Decay speed of force, in units/second
	geVec3d PositionHistory[128];	// Actors position history
// changed RF063
	float ShadowSize;
	bool Moving;
	int CurrentZone;
	int OldZone;
	int GravityCoeff;
	Liquid *LQ;
	char szEntityName[128];
	bool Attached;
	geActor *AttachedActor;
// end change RF063
// changed RF064
    geWorld_Model *PassengerModel;
	bool TransitionFlag;
	float StartTime;
	GE_Collision GravityCollision;
	bool HideRadar;
	char Group[64];
	GE_RGBA FillColor;
	GE_RGBA AmbientColor;
// end change RF064
	char szMotionName[128];				// Name of current motion
	char szNextMotionName[128];		// Name of next motion in queue
	char szDefaultMotionName[128];	// Default motion for this actor
	char szBlendMotionName[128];
	float BlendAmount;
	bool BlendFlag;
	char szBlendNextMotionName[128];
	float BlendNextAmount;
	bool BlendNextFlag;
	int CollDetLevel;
	bool HoldAtEnd;
	bool slide;
	bool fAutoStepUp;							// Actor auto step up flag
	geFloat MaxStepHeight;				// Actors max step height, in world units
	geActor *Vehicle;							// If riding on anything
	geActor *Passengers[PASSENGER_LIST_SIZE];			// If vehicle, riders
	int ActorType;								// Actor type
	bool NeedsNewBB;							// TRUE means needs new EXTBOX
	CPersistentAttributes *Inventory;		// Actors inventory
	struct ActorInstanceList *pNext;	// Next in list
};

//	This struct is the main database element for the actor list

struct LoadedActorList
{
	geActor_Def *theActorDef;			// Actor definition
	geActor *Actor;
	geVec3d BaseRotation;					// Baseline rotation
	geFloat BaseScale;						// Baseline scale
	geFloat BaseHealth;						// Baseline health
	char *szFilename;							// Filename actor was from
	char *szDefaultMotion;				// Default motion for actors
	int ActorType;								// Type of actor this is
	int InstanceCount;						// # of actors in instance list
	ActorInstanceList *IList;			// List of instance for this actor
};

class CActorManager : public CRGFComponent
{
public:
	CActorManager();					// Default constructor
	~CActorManager();					// Default destructor
	geActor *LoadActor(char *szFilename, geActor *OldActor);	// Load an actor from a file
	geActor *SpawnActor(char *szFilename, geVec3d Position, geVec3d Rotation,
		char *DefaultMotion, char *CurrentMotion, geActor *OldActor);	// Spawn an actor w/ parms
	int RemoveActor(geActor *theActor);		// Remove an actor
	int RemoveActorCheck(geActor *theActor);
	int SetAligningRotation(geActor *theActor, geVec3d Rotation);
	int GetAligningRotation(geActor *theActor, geVec3d *Rotation);
	int SetType(geActor *theActor, int nType);					// Set actor type
	int GetType(geActor *theActor, int *nType);					// Get actor type
	int Rotate(geActor *theActor, geVec3d Rotation);		// Rotate an actor
	int Position(geActor *theActor, geVec3d Position);	// Position an actor
	int MoveAway(geActor *theActor);						// Move actor to nowhere
	bool IsActor(geActor *theActor);
	int GetPosition(geActor *theActor, geVec3d *thePosition);	// Get position
	int GetRotation(geActor *theActor, geVec3d *theRotation);	// Get rotation
	int GetRotate(geActor *theActor, geVec3d *theRotation);
	int TurnLeft(geActor *theActor, geFloat theAmount);	// Turn actor LEFT
	int TurnRight(geActor *theActor, geFloat theAmount);	// Turn actor RIGHT
	int ReallyFall(geActor *theActor);
	int CheckReallyFall(geActor *theActor, geVec3d StartPos);
	int UpVector(geActor *theActor, geVec3d *UpVector);		// Get UP vector
	int InVector(geActor *theActor, geVec3d *InVector);		// Get IN vector
	int LeftVector(geActor *theActor, geVec3d *LeftVector);	// Get LEFT vector
	int GetBonePosition(geActor *theActor, char *szBone, geVec3d *thePosition);
	int GetBoneRotation(geActor *theActor, char *szBone, geVec3d *theRotation);
	int AddTranslation(geActor *theActor, geVec3d Amount);	// Add translation to actor
	int AddRotation(geActor *theActor, geVec3d Amount);			// Add rotation to actor
	int RotateToFacePoint(geActor *theActor, geVec3d Position);	// Rotate to face point
	int SetAutoStepUp(geActor *theActor, bool fEnable);	// Set actor auto step up
	int SetStepHeight(geActor *theActor, geFloat fMaxStep);	// Set actor step-up height
	int MoveForward(geActor *theActor, geFloat fSpeed);		// Move actor forward
	int MoveBackward(geActor *theActor, geFloat fSpeed);	// Move actor backward
	int MoveLeft(geActor *theActor, geFloat fSpeed);			// Move actor left
	int MoveRight(geActor *theActor, geFloat fSpeed);			// Move actor right
	int SetAnimationSpeed(geActor *theActor, geFloat fSpeed);	// Set animation speed
	int SetMotion(geActor *theActor, char *MotionName);	// Set actor motion
	int SetNextMotion(geActor *theActor, char *MotionName);	// Prepare next motion
	int SetDefaultMotion(geActor *theActor, char *MotionName);	// Set default motion
	int ClearMotionToDefault(geActor *theActor);		// Force motion to default motion
	//	Actor lighting control, Oh Joy.
	int SetActorDynamicLighting(geActor *theActor, GE_RGBA FillColor, GE_RGBA AmbientColor);
	int ResetActorDynamicLighting(geActor *theActor);
	//	It's possible to add a FORCE to an actor.  This FORCE (seperate from gravity)
	//	..effects an actors translation over time.
	int SetForce(geActor *theActor, int nForceNumber, geVec3d fVector, geFloat InitialValue, geFloat Decay);
// changed RF063
	int GetForce(geActor *theActor, int nForceNumber, geVec3d *ForceVector, geFloat *ForceLevel, geFloat *ForceDecay);
	int SetBlendMot(geActor *theActor, char *name1, char *name2, float Amount);
	void SetMoving(geActor *theActor);
	bool GetMoving(geActor *theActor);
	int GetStepHeight(geActor *theActor, geFloat *fMaxStep);
	int GetActorOldZone(geActor *theActor, int *ZoneType);
	Liquid *GetLiquid(geActor *theActor);
	geActor *GetByEntityName(char *name);
	int SetEntityName(geActor *theActor, char *name);
// end change RF063
	int RemoveForce(geActor *theActor, int nForceNumber);
	geBoolean ForceActive(geActor *theActor, int nForceNumber);
	int SetColDetLevel(geActor *theActor, int ColDetLevel);
	int GetColDetLevel(geActor *theActor, int *ColDetLevel);
	int SetBlendMotion(geActor *theActor, char *name1, char *name2, float Amount);
	int SetBlendNextMotion(geActor *theActor, char *name1, char *name2, float Amount);
	//	Important note: to set the BASELINE for scale, etc.for all actors
	//	..of a specific type use nHandle = (-1).
	int SetScale(geActor *theActor, geFloat fScale);				// Scale actor
	int GetScale(geActor *theActor, geFloat *fScale);		// Get actor scale
	int SetNoCollide(geActor *theActor);
	int SetAlpha(geActor *theActor, geFloat fAlpha);		// Set actor alpha
	int GetAlpha(geActor *theActor, geFloat *fAlpha);		// Get actor alpha
	int GetBoundingBox(geActor *theActor, geExtBox *theBox);	// Get current BBox
	void SetBBox(geActor *theActor, float SizeX, float SizeY, float SizeZ);
	geBoolean DoesBoxHitActor(geVec3d thePoint, geExtBox theBox, 
		geActor **theActor);					// Actor-actor collision check
	geBoolean DoesBoxHitActor(geVec3d thePoint, geExtBox theBox, 
		geActor **theActor, geActor *ActorToExclude);
	int SetHealth(geActor *theActor, geFloat fHealth);	// Set actors health
	int ModifyHealth(geActor *theActor, geFloat fAmount);	// Modify actors health
	int GetHealth(geActor *theActor, geFloat *fHealth);	// Get actors health
	int SetGravity(geActor *theActor, geFloat fGravity);	// Set gravity for actor
	int GetGravity(geActor *theActor, geFloat *fGravity);	// Get gravity
	//	Get actor spatial relation data
	bool IsInFrontOf(geActor *theActor, geActor *theOtherActor);	// Is actor in front of me?
	geFloat DistanceFrom(geActor *theActor, geActor *theOtherActor);	// How far away is the other actor?
	geFloat DistanceFrom(geVec3d Point, geActor *theActor);		// How far is actor from point?
	//	Fill a list with pointers to all actors within a specific range of
	//	..a particular point
	int GetActorsInRange(geVec3d Point, geFloat Range, int nListSize, geActor **ActorList);
	//	Functions for probing the actors environment
	int GetActorZone(geActor *theActor, int *ZoneType);		// Get actors zone
	char *GetMotion(geActor *theActor);
	geBoolean Falling(geActor *theActor);									// Is actor falling?
	//	Actor on model stuff
	void SetVehicle(geActor *theActor, geActor *theVehicle);
	geActor *GetVehicle(geActor *theActor);
	int SetPassenger(geActor *thePassenger, geActor *theVehicle);
	int RemovePassenger(geActor *thePassenger);
	//	Inventory list access
	CPersistentAttributes *Inventory(geActor *theActor);
	//	Dispatch time to all actors
	void Tick(geFloat dwTicks);			// Process passage of time
	int SetTilt(geActor *theActor, bool Flag);
	int TiltUp(geActor *theActor, geFloat theAmount);
	int TiltDown(geActor *theActor, geFloat theAmount);
	float GetTiltX(geActor *theActor);
	int SetSlide(geActor *theActor, bool Flag);
	int CheckAnimation(geActor *theActor, char *Animation);
	int SetBoxChange(geActor *theActor, bool Flag);
	void SetBoundingBox(geActor *theActor, char *Animation);
	int GetAnimationHeight(geActor *theActor, char *Animation, float *Height);
	int SetAnimationHeight(geActor *theActor, char *Animation, bool Camera);
	geBoolean ValidateMove(geVec3d StartPos, geVec3d EndPos, geActor *theActor, bool slide);
	int CountActors();
	void SetHoldAtEnd(geActor *theActor, bool State);
	bool EndAnimation(geActor *theActor);
	int SetShadow(geActor *theActor, geFloat fSize);
	float GetAnimationTime(geActor *theActor);
	geMotion *GetpMotion(geActor *theActor);
	geMotion *GetpBMotion(geActor *theActor);
	float GetBlendAmount(geActor *theActor);
// start multiplayer
	void SetAnimationTime(geActor *theActor, float time);
// end multiplayer
// changed RF064
	int ChangeMaterial(geActor *theActor, char *Change);
	int SetTransitionMotion(geActor *theActor, char *name1, float Amount, char *name2);
	bool CheckTransitionMotion(geActor *theActor, char *name1);
	float GetStartTime(geActor *theActor);
	bool GetTransitionFlag(geActor *theActor);
	int GetGravityCollision(geActor *theActor, GE_Collision *Collision);
	int SetHideRadar(geActor *theActor, bool flag);
	int GetHideRadar(geActor *theActor, bool *flag);
	char *GetGroup(geActor *theActor);
	int SetGroup(geActor *theActor, char *name);
	void SetShadowAlpha(geFloat Alpha)
	{ ShadowAlpha = Alpha; }
	void SetShadowBitmap(geBitmap *Bitmap)
	{ ShadowBitmap = Bitmap; }
	int SetCollide(geActor *theActor);
	geBoolean DoesRayHitActor(geVec3d OldPosition, geVec3d NewPosition, 
		geActor **theActor, geActor *ActorToExclude, geFloat *Percent, geVec3d *Normal);
	int SetGravityTime(geActor *theActor, geFloat fGravitytime);
	void ActorAttach(geActor* Slave,  char *SlaveBoneName, geActor* Master,
		char * MasterBoneName, geXForm3d* Attachment); 
// end change RF064
private:
	//	Private member functions
	void RemoveAllActors(LoadedActorList *theEntry);
	geActor *AddNewInstance(LoadedActorList *theEntry, geActor *OldActor);
	ActorInstanceList *LocateInstance(geActor *theActor, LoadedActorList *theEntry);
	ActorInstanceList *LocateInstance(geActor *theActor);
	int RemoveInstance(geActor *theActor);			// Delete instance of actor
	void TimeAdvanceAllInstances(LoadedActorList *theEntry,	geFloat dwTicks);
	void UpdateActorState(ActorInstanceList *theEntry);
	void AdvanceInstanceTime(ActorInstanceList *theEntry,	geFloat dwTicks);
	void ProcessGravity(ActorInstanceList *theEntry, geFloat dwTicks);
	void ProcessForce(ActorInstanceList *theEntry, geFloat dwTicks);
	int Move(ActorInstanceList *pEntry, int nHow, geFloat fSpeed);			// Velocity-based motion for actor
	bool CheckForStepUp(ActorInstanceList *pEntry, geVec3d NewPosition);
	int GetCurrentZone(ActorInstanceList *pEntry);	// Get brush zone types
	geBoolean ValidateMotion(geVec3d StartPos, geVec3d EndPos,
		ActorInstanceList *pEntry, bool fStepUpOK, bool slide);
	// Validate and handle motion
	void MoveAllVehicles(LoadedActorList *theEntry, float dwTicks);		// Move vehicle actors
	int TranslateAllPassengers(ActorInstanceList *pEntry);
	
	//	Private member variables
	LoadedActorList *MainList[ACTOR_LIST_SIZE];	// Database of loaded actors
	int m_GlobalInstanceCount;			// Level instance counter
// changed RF064
	geFloat ShadowAlpha;
	CIniFile AttrFile;
	bool ValidAttr;
	geBitmap *ShadowBitmap;
// end change RF064
};

#endif
