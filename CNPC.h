/*
CNPC.h:		Non-player character encapsulation class

  This file contains the class declaration for non-player
  characters in an RGF-based game.
*/

#ifndef __RGF_CNPC_H__
#define __RGF_CNPC_H__

typedef geBoolean (*CONTROLp)(void *Data, float dwTicks);

typedef struct Bot_Var
{
	geActor		*Actor;
	char		Attribute[128];
	geVec3d		Velocity;
	geVec3d		LastGoodPos;
	geBoolean	Stopped;
	geBoolean	LastStopped;
	char		Animation[128];
	char		LastAnimation[128];
	char		AnimIdle[128];
	char		AnimWalk[128];
	char		AnimAttack[128];
	char		AnimWalkAttack[128];
	char		AnimJump[128];
	char		AnimDie[128];
	float		RunSpeed;		// Scale value for running speed
	float		JumpSpeed;
	CONTROLp    Action;			// Function that's called every thing
    TrackData   TrackInfo;		// Current track info - track/point/direction
    TrackData   TrackInfoPrev;	// Last point track info
    int32       Dir;			// TOWARD/AWAY from player - Should prob be MODE
	geVec3d		TgtPos;			// Current point you are moving towards
    geActor		*TgtPlayer;		// The player you are aware of at the moment
	geVec3d     MoveVec;		// Current Movement Vector
	float		TimeOut;		// Limit certain actions to a maximum time
	float		ModeTimeOut;	// Limit modes to a maximum time
	int32		ShootCount;		// Current number of shots
	float       TimeSeen;		// Time the player has been in view
	float       TimeNotSeen;    // Time the player has been out of view
	float       ModeTimeSeen;	// For Mode Routines only - Get reset on mode changes
	float       ModeTimeNotSeen;// For Mode Routines only - Get reset on mode changes
	float		ActiveTime;     // For Actor Bots only - limits movement when not in view
    int32       Mode;           // Current Mode of Bot
	int32		Bumps;			// Number of bumps in a row
	int32		LedgeBumps;		// Number of Ledge bumps in a row
	geVec3d		GoalPos;	    // Current goal - could be the player or any other point.
	int8		BotType;        // Multi-player or Actor
	geBoolean	PastFirstTrackPoint; //Flag - sort of self documenting :Q
	int32		WeaponDiff;
	int32		HealthDiff;
    Stack       TrackStack;     // Stack of Tracks
	int32		HealthCheck;	// health check
	float		PickTimeout;
	float		TimeSinceTrack;
	geBoolean	FaceTgtPlayerOnRetreat;
	int32		BotTgtPicked;
	float		NextWeaponTime;
	int			Rank;
	bool		Active;
	char		Trigger[128];
	bool		Dying;
	float		DyingTime;
	bool		StopToAttack;
	bool		Attacking;
	bool		Melee;
	char		Projectile[128];
	char		FireBone[128];
	geVec3d		FireOffset;
	float		MeleeDistance;
	float		AttackDelay;
	char		DamageAttribute[128];
	float		DamageAmt;
	char		AttackSound[128];
	char		DieSound[128];
	float		AimDelay;
	float		AimSkill;
	geVec3d		AimPos;
	CPersistentAttributes *theInv;
	bool		Wander;
	float		AlphaStep;
} Bot_Var;

class CNPC : public CRGFComponent
{
public:
	CNPC();										// Constructor
	~CNPC();									// Destructor
	void Tick(float dwTicks);					// Increment animation time
	void Spawn(NonPlayerCharacter *pSource);
	void Debug();
	int LocateEntity(char *szName, void **pEntityData);
	int SaveTo(FILE *SaveFD, bool type);
	int RestoreFrom(FILE *RestoreFD, bool type);
private:

};

#endif
