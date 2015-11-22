/*
GameEntityDataTypes.h:	Rabid Games Game Framework Entity Data Types

  (c) 1999 Edward A. Averill, III
  
	This file contains all of the game entity data type declarations,
	suitable for use by GEdit and the game itself.
*/

//	Disable unknown pragma warnings (GEdit #pragmas used here)

#pragma warning( disable : 4068 )

#ifndef __RGF_GAMEENTITYDATATYPES_H_
#define __RGF_GAMEENTITYDATATYPES_H_

#include "genesis.h"
 
//	Typedefs

// Sun
//
// Entity that gives special kind of, sun-like, lighting.
// courtesy of Lambert
//
#pragma GE_Type("icons\\wm_sun.bmp")
typedef struct   Sun
{
#pragma		GE_Published
   	geVec3d		origin;
	char *		DaviName;
	GE_RGBA		Color;
	int		Light;
	int		FallOffType ;
	geFloat		FallOffRadiusInTexels ;
	geFloat		FallOffRadiusTopIntensity ;
	geFloat		FallOffSecretNumber ;
#pragma		GE_Origin(origin)
#pragma		GE_DefaultValue(DaviName, "Sun")
#pragma		GE_DefaultValue(Color, "255 255 255")
#pragma		GE_DefaultValue(Light, "150")
#pragma		GE_DefaultValue( FallOffType, "0" )
#pragma		GE_DefaultValue( FallOffRadiusInTexels, "0" )
#pragma		GE_DefaultValue( FallOffRadiusTopIntensity, "0" )
#pragma		GE_DefaultValue( FallOffSecretNumber, "1" )
#pragma		GE_Documentation(origin, "Location to put the Sun.")
#pragma		GE_Documentation(DaviName, " Name that can contain rgb or visible for special lighting.")
#pragma		GE_Documentation(Color, "Color of the light.")
#pragma		GE_Documentation(Light, "Intensity of the light.")
#pragma		GE_Documentation( FallOffType, "0=No Fall Off, 1=Hard Radius, 2=Gradual falloff, 3=Alt Gradual falloff" )
#pragma		GE_Documentation( FallOffRadiusInTexels, "Radius of the light (NB: Only FallOfType <> 0)" )
#pragma		GE_Documentation( FallOffRadiusTopIntensity, "Inside this radius the light is 100 percent (no falloff)" )
#pragma		GE_Documentation( FallOffSecretNumber, "Default 1; influences falloff calculation." )
} Sun ;

//
// Another sun entity with a different type of light
// courtesy of Wismerhill

#pragma GE_Type("icons\\wm_sunlight.bmp")
typedef struct tagSunLight
{
#pragma GE_Published
	geVec3d	origin;
	GE_RGBA	color;
	int	light;
	geVec3d	angles;
	int style;
#pragma 	GE_Origin(origin)
#pragma 	GE_DefaultValue(light, "150")
#pragma		GE_DefaultValue(color, "255 255 255")
#pragma 	GE_Angles(angles)
#pragma 	GE_DefaultValue(angles, "0 0 0")
#pragma		GE_Documentation(color, "Color of the light.")
#pragma		GE_Documentation(light, "Intensity of the light.")
#pragma		GE_Documentation(angles, "Direction of the light.")
} SunLight ;

// FogLight
#pragma GE_Type("FogLight.ico")
typedef struct FogLight
{
#pragma GE_Published
	geVec3d		origin;
	GE_RGBA		Color;
	geFloat		Brightness;
	geFloat		Radius;
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(Color,"100 100 100")
#pragma GE_DefaultValue(Radius,"100")
#pragma GE_DefaultValue(Brightness,"1500")
#pragma GE_Documentation(Color,"RGB color of the fog")
#pragma GE_Documentation(Radius,"Radius of the fog")
#pragma GE_Documentation(Brightness,"Intensity of the fog")
} FogLight;

// Basic automatic door (opens on collide, closes <n> seconds later)

#pragma GE_Type("icons\\wm_door.bmp") 
typedef struct _Door
{
#pragma	GE_Private					// Not seen by GEdit
	geBoolean bInAnimation;		// Is door animating?
	geBoolean bTrigger;				// Has the door been triggered?
	geFloat	AnimationTime;				// Ticks in animation
	geBoolean bInCollision;		// Door is colliding flag
	geBoolean bActive;				// Door can be used
	geFloat LastIncrement;				// Last tick increment
	geFloat tDoor;							// Door animation position time
	geSound_Def *theSound;		// Sound def pointer
	int SoundHandle;
	geBoolean CallBack;
	int CallBackCount;
#pragma GE_Published
	geWorld_Model	*Model;			// Door model to animate
	geWorld_Model *NextToTrigger;	// Next door to trigger, if any
	geVec3d origin;						// Location of same
	char *szSoundFile;				// Name of sound to play on open
	geBoolean bOneShot;				// TRUE if door opens ONLY ONCE
	geBoolean bNoCollide;			// TRUE if door won't open on collision
	char *szEntityName;				// Name of this entity
	geBoolean bAudioLoops;		// Audio effect loops while animation plays
	geFloat AnimationSpeed;		// Speed of the animation, in %
	char *TriggerName;
	geBoolean bShoot;
	geBoolean bReverse;
	geBoolean bRotating;
	geBoolean     bRunWhileTrig;    // Animation only runs while trigger is on
    geBoolean     bRunTimed;        // Animation runs specified time increment each
                                    //           time trigger goes from off to on
    geFloat       TimeEachTrig;     // Time animation will run each trigger, only valid
                                    //           if bRunTimed is true
    geBoolean     bRunToNextEvent;  // Animation runs until the next event time is reached
                                    //           each time triggered
    geBoolean     bRunFromList;     // Animation runs to next time in provided list each
                                    //           time triggered
    char          *TimeList;        // List of times to run animation to each time triggered
                                    //           in the form (1.0 3.5 ...) only valid if 
									//           bRunFromList is true
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(szSoundFile, "")
#pragma GE_DefaultValue(bOneShot, "False")
#pragma GE_DefaultValue(bNoCollide, "False")
#pragma GE_DefaultValue(szEntityName, "")
#pragma GE_DefaultValue(bAudioLoops, "False")
#pragma GE_DefaultValue(AnimationSpeed, "1.0")
#pragma GE_DefaultValue(TriggerName, "")
#pragma GE_DefaultValue(bShoot, "False")
#pragma GE_DefaultValue(bReverse, "False")
#pragma GE_DefaultValue(bRotating, "False")
#pragma GE_DefaultValue(bRunWhileTrig, "False")
#pragma GE_DefaultValue(bRunTimed, "False")
#pragma GE_DefaultValue(TimeEachTrig, "0")
#pragma GE_DefaultValue(bRunToNextEvent, "False") 
#pragma GE_DefaultValue(bRunFromList, "False")
#pragma GE_DefaultValue(TimeList, "") 
#pragma GE_Documentation(Model, "Door model to animate")
#pragma GE_Documentation(NextToTrigger, "Next door to trigger, if any")
#pragma GE_Documentation(szSoundFile, "Sound file to play when door opens")
#pragma GE_Documentation(bOneShot, "TRUE if door operates only once")
#pragma GE_Documentation(bNoCollide, "TRUE if door won't open on collision")
#pragma GE_Documentation(szEntityName, "Name of entity (used in scripting and triggers)")
#pragma GE_Documentation(bAudioLoops, "TRUE if audio loops while animation plays")
#pragma GE_Documentation(AnimationSpeed,"Speed of animation, 1.0 = 100% original")
#pragma GE_Documentation(TriggerName, "Name of trigger entity to use")
#pragma GE_Documentation(bShoot, "TRUE if shooting causes collision")
#pragma GE_Documentation(bReverse, "TRUE if collision causes door to reverse direction")
#pragma GE_Documentation(bRotating, "TRUE if door is a rotating (swing) model")
} Door;

//	The infamous and never-to-be-forgotten Moving Platform
//

#pragma GE_Type("icons\\wm_platform.bmp")
typedef struct _MovingPlatform
{
#pragma GE_Private
	geBoolean bInAnimation;		// Is the platform animating?
	geBoolean bTrigger;				// Has the platform been triggered?
	geBoolean bActorOnMe;			// Is there an ACTOR sitting on me?
	geBoolean bInCollision;		// Platform is colliding flag
	geBoolean bActive;				// Platform can be used
	geFloat LastIncrement;				// Last tick increment
	geBoolean bForward;				// Animation running forward?
	geFloat tPlatform;					// Platform animation position time
	geSound_Def *theSound;		// Sound def pointer
	int SoundHandle;			// For later modification of sound
	geBoolean CallBack;
	int CallBackCount;
#pragma GE_Published
	geWorld_Model *Model;			// Platform model
	geWorld_Model *NextToTrigger;	// Next door to trigger, if any
	geVec3d origin;						// Location of model
	geFloat AnimationSpeed;		// Speed of the animation, in %
	char *szSoundFile;				// Name of sound to play on trigger
	geBoolean bOneShot;				// TRUE if platform runs ONLY ONCE
	geBoolean bNoCollide;			// TRUE if platform won't start on collision
	char *szEntityName;				// Name of this moving platform
	geBoolean bAudioLoops;		// Audio effect loops while animation plays
	geBoolean bAutoStart;			// Animation starts when level loads
	geBoolean bLooping;				// Once started, animation runs forever
	char *TriggerName;
	geBoolean bShoot;
	geBoolean bReverse;
	//MOD010122 - Added next six variables
    geBoolean     bRunWhileTrig;    // Animation only runs while trigger is on
    geBoolean     bRunTimed;        // Animation runs specified time increment each
                                    //           time trigger goes from off to on
    geFloat       TimeEachTrig;     // Time animation will run each trigger, only valid
                                    //           if bRunTimed is true
    geBoolean     bRunToNextEvent;  // Animation runs until the next event time is reached
                                    //           each time triggered
    geBoolean     bRunFromList;     // Animation runs to next time in provided list each
                                    //           time triggered
    char          *TimeList;        // List of times to run animation to each time triggered
                                    //           in the form (1.0 3.5 ...) only valid if 
									//           bRunFromList is true

#pragma GE_Origin(origin)
#pragma GE_DefaultValue(szSoundFile, "")
#pragma GE_DefaultValue(bOneShot, "False")
#pragma GE_DefaultValue(szEntityName, "")
#pragma GE_DefaultValue(bNoCollide, "False")
#pragma GE_DefaultValue(bAudioLoops, "False")
#pragma GE_DefaultValue(bAutoStart, "False")
#pragma GE_DefaultValue(bLooping, "False")
#pragma GE_DefaultValue(AnimationSpeed, "1.0")
#pragma GE_DefaultValue(TriggerName, "")
#pragma GE_DefaultValue(bShoot, "False")
#pragma GE_DefaultValue(bReverse, "True")
#pragma GE_DefaultValue(bRunWhileTrig, "False")
#pragma GE_DefaultValue(bRunTimed, "False")
#pragma GE_DefaultValue(TimeEachTrig, "0")
#pragma GE_DefaultValue(bRunToNextEvent, "False") 
#pragma GE_DefaultValue(bRunFromList, "False")
#pragma GE_DefaultValue(TimeList, "") 
#pragma GE_Documentation(Model,"Platform to use")
#pragma GE_Documentation(NextToTrigger, "Next platform to trigger, if any")
#pragma GE_Documentation(szSoundFile, "Sound file to play when platform triggers")
#pragma GE_Documentation(bOneShot, "TRUE if platform operates only once")
#pragma GE_Documentation(bNoCollide, "TRUE if platform won't start on collision")
#pragma GE_Documentation(szEntityName, "Name of entity (used in scripting and triggers)")
#pragma GE_Documentation(bAudioLoops, "TRUE if audio loops while animation plays")
#pragma GE_Documentation(bAutoStart, "Platform starts on level load")
#pragma GE_Documentation(bLooping, "Animation loops forever once started")
#pragma GE_Documentation(AnimationSpeed,"Speed of animation, 1.0 = 100% original")
#pragma GE_Documentation(TriggerName, "Name of trigger entity to use")
#pragma GE_Documentation(bShoot, "TRUE if shooting causes collision")
#pragma GE_Documentation(bReverse, "TRUE if animation reverses at end")
} MovingPlatform;

//	Teleport entities (teleporter and teleport target)

#pragma GE_Type("icons\\sf_teleport.bmp")
typedef struct _Teleporter
{
#pragma GE_Private
	geBoolean bActive;				  // This teleporter ready to run?
	geSound_Def *theSound;			// Handle of audio effect for teleport
	int SoundHandle;				// For later modification of sound
	geFog *theFog;							// Teleporters sphere of fog
	geBoolean bForward;					// Fog animation direction
	geFloat fDelta;								// Fog depth variance delta
	geFloat fDensity;							// Current fog density
	geBoolean active;
#pragma GE_Published
	geWorld_Model *Model;					// Teleporter model
	geVec3d origin;								// Location of entity
	char *Target;									// Name of teleport target
	geBoolean bOneShot;						// Teleporter only runs ONCE
	char *szSoundFile;						// Audio to play when teleport activates
	GE_RGBA cColor;								// Fog color
	int fogSpeed;									// Speed at which fog density changes, in msec.
	geFloat fogVariance;					// Depth of density variance
	geFloat fogRadius;						// Radius of fog around entity
	char *szEntityName;						// Name of entity
	geBoolean bUseFogEffect;			// TRUE if fog effect is active
	geBoolean bUseTeleportEffect;	// TRUE if teleport effect is run on collision
	char *TriggerName;	// Name of trigger entity
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(bOneShot, "False")
#pragma GE_DefaultValue(Target, "")
#pragma GE_DefaultValue(szSoundFile, "")
#pragma GE_DefaultValue(cColor,"255 0 0")
#pragma GE_DefaultValue(fogSpeed,"1000")
#pragma GE_DefaultValue(fogVariance, "500.0")
#pragma GE_DefaultValue(fogRadius, "40.0")
#pragma GE_DefaultValue(szEntityName, "")
#pragma GE_DefaultValue(bUseFogEffect, "True")
#pragma GE_DefaultValue(bUseTeleportEffect, "True")
#pragma GE_DefaultValue(TriggerName, "")
#pragma GE_Documentation(Model,"Teleporter model to use")
#pragma GE_Documentation(Target, "Name of teleport target to go to")
#pragma GE_Documentation(bOneShot,"True if teleport only works once")
#pragma GE_Documentation(cColor,"Color for teleport field")
#pragma GE_Documentation(fogSpeed,"Speed of density variance, in milliseconds")
#pragma GE_Documentation(fogVariance,"Depth of fog variance")
#pragma GE_Documentation(fogRadius,"Radius of fog around entity")
#pragma GE_Documentation(szSoundFile, "Audio effect to play when teleport activated")
#pragma GE_Documentation(szEntityName, "Name of entity (used in scripting and triggers)")
#pragma GE_Documentation(bUseFogEffect, "TRUE if fog is active for teleport entity")
#pragma GE_Documentation(bUseTeleportEffect, "TRUE if, on collision, fog densifies prior to player teleport")
#pragma GE_Documentation(TriggerName, "Name of trigger entity to use")
} Teleporter;

#pragma GE_Type("icons\\sf_teletargt.bmp")
typedef struct _TeleportTarget
{
#pragma GE_Private
	geBoolean bFollower;				// Bound to path follower
#pragma GE_Published
	char *Name;								  // Name of this teleport target
	geVec3d origin;						  // Location of this teleport target
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(Name, "")
#pragma GE_Documentation(Name,"Name of this teleport target")
} TeleportTarget;

//	Ye olde player start position

#pragma GE_Type("icons\\pstart.bmp")
typedef struct _PlayerStart
{
#pragma GE_Published
	geVec3d     origin;
	geVec3d     Angle;
	char *szMIDIFile;						// MIDI file to play, if any
	char *szCDTrack;						// CD TRACK to play, if any
	char *szStreamingAudio;			// Streaming audio to play, if any
	geBoolean bSoundtrackLoops;	// Soundtrack for level loops
	char *szEntityName;
#pragma GE_Origin(origin)
#pragma GE_Angles(Angle)
#pragma GE_DefaultValue(szMIDIFile, "")
#pragma GE_DefaultValue(szCDTrack, "")
#pragma GE_DefaultValue(szStreamingAudio, "")
#pragma GE_DefaultValue(szEntityName, "")
#pragma GE_DefaultValue(bSoundtrackLoops, "False")
#pragma GE_Documentation(szMIDIFile, "MIDI file to play for level")
#pragma GE_Documentation(szCDTrack, "CD Track to play for level")
#pragma GE_Documentation(szStreamingAudio, "Streaming audio to play, if any")
#pragma GE_Documentation(bSoundtrackLoops, "TRUE if soundtrack for level loops")
#pragma GE_Documentation(szEntityName, "Name of Start Point, blank for default")
} PlayerStart;

//	MorphingField, color/size/density morphing fog field

#pragma GE_Type("icons\\sf_morphf.bmp")
typedef struct _MorphingField
{
#pragma GE_Private
	geFog *theFog;							// MorphingField sphere of fog
	geBoolean bForward;					// Fog animation direction
	geFloat fDepthDelta;					// Fog depth variance delta
	geFloat fDensity;							// Current fog density
	GE_RGBA cDelta;							// Color morphing delta
	geFloat fRadiusDelta;					// Fog radius delta
	geFloat fRadius;							// Current fog radius
	geSound_Def *theSound;			// Handle of audio effect for MorphingField
	int SoundHandle;				// For later modification of sound
	geFloat tElapsed;								// Time elapsed in single animation cycle
	GE_RGBA clrCurrent;					// Current fog color
	geBoolean bFollower;				// TRUE if entity is bound to a motion path
	geBoolean active;		  // is activated
	geVec3d  OriginOffset;      //Translation from model center
#pragma GE_Published
	geVec3d origin;							// Location of entity in world
	char *szSoundFile;					// Looping audio to play for MorphingField
	GE_RGBA clrStart;						// Fog color, start of morph
	GE_RGBA clrEnd;							// Fog color, end of morph
	int fogSpeed;								// Speed at which fog density changes, in msec.
	geFloat fogVariance;					// Depth of density variance
	geFloat fogRadiusStart;				// Radius of fog around entity, start of morph
	geFloat fogRadiusEnd;					// Radius of fog around entity, end of morph
	char *szEntityName;					// Name of this entity
	char *EntityName;	// Nmae of entity to attach to
	geWorld_Model *Model;	// Name of model to attach to
	char *BoneName;		// Name of actor bone to attach to
	char *TriggerName;	// Name of trigger entity
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(szSoundFile, "")
#pragma GE_DefaultValue(clrStart,"100 100 100")
#pragma GE_DefaultValue(clrEnd, "100 100 100")
#pragma GE_DefaultValue(fogSpeed,"1000")
#pragma GE_DefaultValue(fogVariance, "500.0")
#pragma GE_DefaultValue(fogRadiusStart, "40.0")
#pragma GE_DefaultValue(fogRadiusEnd, "80.0")
#pragma GE_DefaultValue(szEntityName, "")
#pragma GE_DefaultValue(EntityName, "")
#pragma GE_DefaultValue(BoneName, "")
#pragma GE_DefaultValue(TriggerName, "")
#pragma GE_Documentation(clrStart,"Start color for field")
#pragma GE_Documentation(clrEnd, "End color for field")
#pragma GE_Documentation(szSoundFile, "Audio loop to play for field sound effect")
#pragma GE_Documentation(fogSpeed,"Speed of morphing variance, in milliseconds")
#pragma GE_Documentation(fogVariance,"Depth of field density variance")
#pragma GE_Documentation(fogRadiusStart,"Start radius of field around entity")
#pragma GE_Documentation(fogRadiusEnd, "End radius of field around entity")
#pragma GE_Documentation(szEntityName, "Name of entity (used in scripting and triggers)")
#pragma GE_Documentation(EntityName, "Name of entity to attach to")
#pragma GE_Documentation(Model, "Name of model to attach to")
#pragma GE_Documentation(BoneName, "Name of actor bone to attach to")
#pragma GE_Documentation(TriggerName, "Name of trigger entity to use")
} MorphingField;

//	3D Audio source entity

#pragma GE_Type("icons\\a_3d.bmp")
typedef struct _AudioSource3D
{
#pragma GE_Private
	int *effect;		  // pointer to array of effect indexs
	geBoolean active;		  // is activated
	geVec3d  OriginOffset;      //Translation from model center
#pragma GE_Published
	geVec3d origin;		// Location of entity in world
	char *szSoundFile;	// Audio that is the sound
	geFloat fRadius;		// Maximum audible radius for sound 
	char *szEntityName;	// Name of this entity
	char *EntityName;	// Nmae of entity to attach to
	geWorld_Model *Model;	// Name of model to attach to
	char *BoneName;		// Name of actor bone to attach to
	char *TriggerName;	// Name of trigger entity
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(szSoundFile, "")
#pragma GE_DefaultValue(fRadius, "200.0")
#pragma GE_DefaultValue(szEntityName, "")
#pragma GE_DefaultValue(EntityName, "")
#pragma GE_DefaultValue(BoneName, "")
#pragma GE_DefaultValue(TriggerName, "")
#pragma GE_Documentation(szSoundFile, "Audio loop to play")
#pragma GE_Documentation(fRadius, "Maximum audible radius for sound")
#pragma GE_Documentation(szEntityName, "Name of entity (used in scripting and triggers)")
#pragma GE_Documentation(EntityName, "Name of entity to attach to")
#pragma GE_Documentation(Model, "Name of model to attach to")
#pragma GE_Documentation(BoneName, "Name of actor bone to attach to")
#pragma GE_Documentation(TriggerName, "Name of trigger entity to use")
} AudioSource3D;

//	Static Entity proxy
//
//	eaa3 09/11/2000 Note that the Static Entity Proxy is used for a LOT of
//	..different things at this point - moving props, static props, and now
//	.."attribute" modifiers.  If the "attribute name" is present, then when
//	..the actor is collided with the collision sound is played, the actor is
//	..removed from the game, and the colliding actor has its attribute list
//	..updated appropriately.  Also, note that a prop now has "health" and
//	..when the health drops to zero the actor is removed from the game, at
//	..least until "RespawnTime" has elapsed (a RespawnTime of zero means there
//	..is no respawn for this static entity).

#pragma GE_Type("icons\\p_static.bmp")
typedef struct _StaticEntityProxy
{
#pragma GE_Private
	geSound_Def *theSound;			// Handle of audio effect
	geSound *SoundHandle;				// For later modification of sound
	geActor *Actor;							// Actor for this entity
	geBoolean bFollower;				// TRUE if entity is bound to a motion path
	geBoolean bInitialized;			// Path following initialized flag
	geBoolean IsHit;
	geBoolean alive;
	geBoolean CallBack;
	geFloat Time;
	geBoolean DoingDamage;
	int CallBackCount;
	geBoolean bState;
#pragma GE_Published
	geVec3d origin;
	char *szSoundFile;					// Name of audio effect to play on collision
	geBoolean SoundLoop;
	geFloat fRadius;						// Audio audible radius
	char *szActorFile;
	char *szEntityName;
	char *szDefaultAction;			// Default animation to play
	char *szImpactAction;				// Animation to play when collided with
	geBoolean bCanPush;					// Static entity can be pushed around
	int InitialAlpha;						// Transparency of actor
	geBoolean SubjectToGravity;	// TRUE if entity subject to gravity
	geFloat ScaleFactor;				// Factor to scale model by
	geBoolean FaceDestination;	// TRUE if entity rotated to face destination
	geVec3d ActorRotation;			// Rotation needed to properly orient actor
	geBoolean IsAVehicle;				// TRUE if actor can be "ridden"
	geFloat AttributeAmt;							// Props health points, or -1 for indestructible
	geBoolean DoDamage;
	geFloat Damage;
	geFloat DamageDelay;
	char *DamageTo;
	char *DamageAttribute;
	geFloat StepHeight;
	geBoolean DeathDissappear;
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(szSoundFile, "")
#pragma GE_DefaultValue(SoundLoop, "False")
#pragma GE_DefaultValue(fRadius, "200.0")
#pragma GE_DefaultValue(szEntityName, "")
#pragma GE_DefaultValue(szActorFile, "")
#pragma GE_DefaultValue(szDefaultAction, "")
#pragma GE_DefaultValue(szImpactAction, "")
#pragma GE_DefaultValue(bCanPush, "False")
#pragma GE_DefaultValue(InitialAlpha, "255")
#pragma GE_DefaultValue(SubjectToGravity, "True")
#pragma GE_DefaultValue(ScaleFactor, "1.0")
#pragma GE_DefaultValue(FaceDestination, "False")
#pragma GE_DefaultValue(ActorRotation, "0 180 0.0")
#pragma GE_DefaultValue(IsAVehicle, "False")
#pragma GE_DefaultValue(DoDamage, "False")
#pragma GE_DefaultValue(Damage, "0")
#pragma GE_DefaultValue(DamageDelay, "5")
#pragma GE_DefaultValue(DamageTo, "")
#pragma GE_DefaultValue(AttributeAmt, "-1")
#pragma GE_DefaultValue(DamageAttribute, "")
#pragma GE_DefaultValue(StepHeight, "0")
#pragma GE_DefaultValue(DeathDissappear, "True")
#pragma GE_Documentation(szSoundFile, "Sound to play on collision/activation")
#pragma GE_Documentation(SoundLoop, "TRUE if sound loops")
#pragma GE_Documentation(fRadius, "Audible range for sound")
#pragma GE_Documentation(szActorFile, "Actor file to load for this proxy")
#pragma GE_Documentation(szEntityName, "Name of entity (used in scripting and triggers)")
#pragma GE_Documentation(szDefaultAction, "Default animation, if any, for this prop")
#pragma GE_Documentation(szImpactAction, "Animation, if any, to play when collided with")
#pragma GE_Documentation(bCanPush, "TRUE if the player can push actor around")
#pragma GE_Documentation(InitialAlpha,"Transparency of actor, from 0-255 with 0=transparent")
#pragma GE_Documentation(SubjectToGravity, "TRUE if actor will be subject to gravity")
#pragma GE_Documentation(ScaleFactor, "Number to scale actor by")
#pragma GE_Documentation(FaceDestination, "TRUE if actor rotated to face destination")
#pragma GE_Documentation(ActorRotation, "Rotation needed to properly orient actor in game")
#pragma GE_Documentation(IsAVehicle, "TRUE if actor can be ridden")
#pragma GE_Documentation(DoDamage, "TRUE if actor does damage on collision")
#pragma GE_Documentation(Damage, "Amount of damage done")
#pragma GE_Documentation(DamageDelay, "Number of seconds until damage occurs again")
#pragma GE_Documentation(DamageTo, "Name of attribute actor causes damage to")
#pragma GE_Documentation(AttributeAmt,"Prop health points, or -1 if indestructible")
#pragma GE_Documentation(DamageAttribute, "Name of attribute that takes damage")
#pragma GE_Documentation(StepHeight, "Height that actor can step up when moving")
#pragma GE_Documentation(DeathDissappear, "TRUE if actor is removed when dead")
} StaticEntityProxy;

//	Particle System proxy

#pragma GE_Type("icons\\sf_partsys.bmp")
typedef struct _ParticleSystemProxy
{
#pragma GE_Private
	int psHandle;			// Particle system handle
	int pmHandle;			// Particle map handle
	int *effect;		  	// pointer to array of effect indexs
	geBoolean active;
	geVec3d  OriginOffset;      //Translation from model center
#pragma GE_Published
	geVec3d origin;
	int nStyle;			// Particle system style
	char *szSoundFile;		// Audio effect for particle system
	geFloat fRadius;			// Audio audible radius
	char *szTexture;		// Texture for particles
	GE_RGBA clrColor;		// Color for particles
	int BirthRate;			// Over-ride default system birth rate
	int MaxParticleCount;		// Over-ride default max particle count
	int ParticleLifespan;		// Over-ride default particle lifespan
	char *szEntityName;		// Name for this particle system
	char *EntityName;		// Nmae of entity to attach to
	geWorld_Model *Model;		// Name of model to attach to
	char *BoneName;			// Name of actor bone to attach to
	char *TriggerName;		// Name of trigger entity
	geFloat Scale;
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(nStyle, "2")
#pragma GE_DefaultValue(szSoundFile, "")
#pragma GE_DefaultValue(fRadius, "200.0")
#pragma GE_DefaultValue(szTexture, "")
#pragma GE_DefaultValue(clrColor, "255 255 255")
#pragma GE_DefaultValue(BirthRate, "0")
#pragma GE_DefaultValue(MaxParticleCount, "0")
#pragma GE_DefaultValue(ParticleLifespan, "0")
#pragma GE_DefaultValue(szEntityName, "")
#pragma GE_DefaultValue(EntityName, "")
#pragma GE_DefaultValue(BoneName, "")
#pragma GE_DefaultValue(TriggerName, "")
#pragma GE_DefaultValue(Scale, "1")
#pragma GE_Documentation(nStyle, "Particle system style")
#pragma GE_Documentation(szSoundFile, "Audio loop to play")
#pragma GE_Documentation(fRadius, "Audible range for sound")
#pragma GE_Documentation(szTexture, "Texture file to use for particles")
#pragma GE_Documentation(clrColor, "Color for particles")
#pragma GE_Documentation(BirthRate, "Particle birth rate to use instead of system default")
#pragma GE_Documentation(MaxParticleCount, "Maximum particle count to use instead of system default")
#pragma GE_Documentation(ParticleLifespan, "Particle life span to use instead of system default")
#pragma GE_Documentation(szEntityName, "Name of entity (used in scripting and triggers)")
#pragma GE_Documentation(EntityName, "Name of entity to attach to")
#pragma GE_Documentation(Model, "Name of model to attach to")
#pragma GE_Documentation(BoneName, "Name of actor bone to attach to")
#pragma GE_Documentation(TriggerName, "Name of trigger entity to use")
#pragma GE_Documentation(Scale, "Scale of particles")
} ParticleSystemProxy;

//	ChangeLevel
//
//	Entity that causes a model to become a level-change trigger.

#pragma GE_Type("icons\\wm_level.bmp")
typedef struct _ChangeLevel
{
#pragma GE_Private
	geBoolean CallBack;
	int CallBackCount;
#pragma GE_Published
	geVec3d origin;
	char *szEntityName;
	geWorld_Model *Model;				// Model to use as trigger
	char *szSplashFile;					// Bitmap to use as level-change splash screen
	char *szAudioFile;					// Sound to play with splash, if any
	char *szNewLevel;						// Filename for new level
	char *szCutScene;						// Cut scene to play before level change, if any
	char *szMessage;
	int Font;
	geBoolean UseOffset;
	geBoolean UseAngle;
	char *StartPointName;
	geBoolean UseEffect;
	GE_RGBA EffectColor;
	char *Trigger;
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(szEntityName, "")
#pragma GE_DefaultValue(szSplashFile,"")
#pragma GE_DefaultValue(szAudioFile, "")
#pragma GE_DefaultValue(szNewLevel, "")
#pragma GE_DefaultValue(szCutScene, "")
#pragma GE_DefaultValue(szMessage, "")
#pragma GE_DefaultValue(Trigger, "")
#pragma GE_DefaultValue(Font, "3")
#pragma GE_DefaultValue(UseOffset, "False")
#pragma GE_DefaultValue(UseAngle, "False")
#pragma GE_DefaultValue(StartPointName, "")
#pragma GE_DefaultValue(UseEffect, "False")
#pragma GE_DefaultValue(EffectColor, "255 255 255")
#pragma GE_Documentation(szEntityName, "Name of entity (used in scripting and triggers)")
#pragma GE_Documentation(Model,"Model to use as level-change trigger")
#pragma GE_Documentation(szSplashFile,"Level change bitmap splash screen")
#pragma GE_Documentation(szAudioFile,"Sound to play with splash screen")
#pragma GE_Documentation(szNewLevel, "Name of level file to load")
#pragma GE_Documentation(szCutScene, "Inter-level cut scene to play, if any")
#pragma GE_Documentation(szMessage, "Message to display instead of Splash Screen")
#pragma GE_Documentation(Font, "Size of Message font 0 to 6")
#pragma GE_Documentation(Trigger, "Name of trigger entity to use")
#pragma GE_Documentation(StartPointName, "Entity Name of PlayerStart to use")
#pragma GE_Documentation(UseOffset, "if True then Offset Player at Start Position")
#pragma GE_Documentation(UseAngle, "if True then use PlayerStart Angle at Start Position")
#pragma GE_Documentation(UseEffect, "if True then use Fog Effect at level change")
#pragma GE_Documentation(EffectColor, "Color of Effect Fog")
} ChangeLevel;

//	SoundtrackToggle
//
//	Entity that causes passing withing range to trigger
//	..a change in the playing soundtrack.

#pragma GE_Type("icons\\a_3d.bmp")
typedef struct _SoundtrackToggle
{
#pragma GE_Private
	geBoolean bActive;					// Trigger ready for use
	geBoolean bWhichOne;				// Which selection to play
	geFloat LastTimeToggled;				// Last time the soundtrack was toggled
	geSound_Def *theSound;			// Handle of audio effect
	geSound *SoundHandle;				// For later modification of sound
#pragma GE_Published
	geVec3d origin;
	char *szMIDIFileOne;				// First MIDI file to play, if any
	char *szMIDIFileTwo;				// Second MIDI file to play, if any
	int CDTrackOne;							// First CD track to play, if any
	int CDTrackTwo;							// Second CD track to play, if any
	char *szStreamFileOne;			// First streaming sound to play, if any
	char *szStreamFileTwo;			// Second streaming sound to play, if any
	char *szSoundFile;					// Audio effect to play on trigger, if any
	geBoolean bOneShot;					// Soundtrack toggles only once
	geBoolean bCDLoops;					// TRUE if the CD soundtrack loops
	geBoolean bMIDILoops;				// TRUE if the MIDI soundtrack loops
	geBoolean bStreamLoops;			// TRUE if the streaming audio loops
	int SleepTime;							// Delay before next toggle is possible, in seconds
	geFloat Range;								// Range of trigger entity
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(szMIDIFileOne, "")
#pragma GE_DefaultValue(szMIDIFileTwo, "")
#pragma GE_DefaultValue(CDTrackOne, "0")
#pragma GE_DefaultValue(CDTrackTwo, "0")
#pragma GE_DefaultValue(szStreamFileOne, "")
#pragma GE_DefaultValue(szStreamFileTwo, "")
#pragma GE_DefaultValue(szSoundFile, "")
#pragma GE_DefaultValue(bOneShot, "False")
#pragma GE_DefaultValue(bCDLoops, "True")
#pragma GE_DefaultValue(bMIDILoops, "True")
#pragma GE_DefaultValue(bStreamLoops, "True")
#pragma GE_DefaultValue(SleepTime, "30")
#pragma GE_DefaultValue(Range, "20.0")
#pragma GE_Documentation(szMIDIFileOne, "MIDI soundtrack file #1")
#pragma GE_Documentation(szMIDIFileTwo, "MIDI soundtrack file #2")
#pragma GE_Documentation(CDTrackOne, "CD Audio track #1")
#pragma GE_Documentation(CDTrackTwo, "CD Audio track #2")
#pragma GE_Documentation(szStreamFileOne, "Streaming audio file #1")
#pragma GE_Documentation(szStreamFileTwo, "Streaming audio file #2")
#pragma GE_Documentation(szSoundFile, "Optional audio effect to play on toggle")
#pragma GE_Documentation(bOneShot, "TRUE if toggle only works once")
#pragma GE_Documentation(bCDLoops, "TRUE if the CD soundtrack loops")
#pragma GE_Documentation(bMIDILoops, "TRUE if the MIDI soundtrack loops")
#pragma GE_Documentation(bStreamLoops, "TRUE if the streaming audio loops")
#pragma GE_Documentation(SleepTime, "Delay before this toggle will trigger again")
#pragma GE_Documentation(Range, "Range in world units of the trigger")
} SoundtrackToggle;

//	StreamingAudioProxy
//
//	Entity that causes passing withing range to start streaming
//	..a WAVE file from disk.

#pragma GE_Type("icons\\a_stream.bmp")
typedef struct _StreamingAudioProxy
{
#pragma GE_Private
	geBoolean bActive;					// Trigger ready for use
	int LastTimeTriggered;			// Last time the stream was triggered
#pragma GE_Published
	geVec3d origin;
	char *szStreamFile;					// First streaming sound to play, if any
	geBoolean bOneShot;					// Soundtrack toggles only once
	geBoolean bLoops;						// TRUE if the soundtrack loops
	int SleepTime;							// Delay before next toggle is possible, in seconds
	geFloat Range;								// Range of trigger entity
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(szStreamFile, "")
#pragma GE_DefaultValue(bOneShot, "False")
#pragma GE_DefaultValue(bLoops, "True")
#pragma GE_DefaultValue(SleepTime, "30")
#pragma GE_DefaultValue(Range, "20.0")
#pragma GE_Documentation(szStreamFile, "Streaming audio file to play")
#pragma GE_Documentation(bOneShot, "TRUE if toggle only works once")
#pragma GE_Documentation(bLoops, "TRUE if the played soundtrack loops")
#pragma GE_Documentation(SleepTime, "Delay before this toggle will trigger again")
#pragma GE_Documentation(Range, "Range in world units of the trigger")
} StreamingAudioProxy;

//  VideoTextureReplacer
//
//	Tells the VideoTexture component which textures in this level
//	..will be replaced with what AVI files.

#pragma GE_Type("icons\\sf_video.bmp")
typedef struct _VideoTextureReplacer
{
#pragma GE_Private
	geBoolean Playing;					// Video playing flag
#pragma GE_Published
	geVec3d origin;							// Entity origin
	char *szTextureName;				// Name of texture to replace
	char *szVideoName;					// Video to use for texture
	geFloat Radius;								// Player enters this radius, video plays
	geBoolean OnlyPlayInRadius;	// TRUE if the video plays only while the
	// ..player is inside the hit radius
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(szTextureName, "")
#pragma GE_DefaultValue(szVideoName, "")
#pragma GE_DefaultValue(Radius, "0.0")
#pragma GE_DefaultValue(OnlyPlayInRadius,"False")
#pragma GE_Documentation(szTextureName, "Name of texture to replace")
#pragma GE_Documentation(szVideoName, "Video to play as texture replacement")
#pragma GE_Documentation(Radius, "Range of trigger zone to start video, or 0.0 if always on")
#pragma GE_Documentation(OnlyPlayInRadius, "TRUE if video only plays while player in range")
} VideoTextureReplacer;

//	EnvironmentSetup
//
//	Allows the level designer to set various player parameters
//	..when the level is loaded.  If a parameter is ZERO, then
//	..the existing (default) value is used for that element.

#pragma GE_Type("icons\\e_setup.bmp")
typedef struct _EnvironmentSetup
{
#pragma GE_Published
	geVec3d origin;							// Required origin
	geFloat Gravity;							// Player gravity
	geFloat Speed;								// Player speed
	geFloat JumpSpeed;						// Player jump speed
	geFloat StepHeight;						// Player stair step-up height
	geBoolean RealJumping;
	geBoolean EnableDistanceFog;	// Enable engine distance fog
	GE_RGBA DistanceFogColor;		// Color of distance fog
	geFloat FogStartDistLow;		// Start of fog from camera, in world units
	geFloat FogStartDistHigh;
	geFloat TotalFogDistLow;	// Distance at which fog is total
	geFloat TotalFogDistHigh;
	geBoolean UseFarClipPlane;	// Set or don't use clip plane
	geFloat FarClipPlaneDistLow;	// The clip plane distance to use
	geFloat FarClipPlaneDistHigh;
	geFloat MaxLookUpAngle;
	geFloat MaxLookDwnAngle;
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(Gravity, "0")
#pragma GE_DefaultValue(Speed, "0")
#pragma GE_DefaultValue(JumpSpeed, "0")
#pragma GE_DefaultValue(StepHeight, "0")
#pragma GE_DefaultValue(MaxLookUpAngle, "0")
#pragma GE_DefaultValue(MaxLookDwnAngle, "0")
#pragma GE_DefaultValue(EnableDistanceFog, "False")
#pragma GE_DefaultValue(RealJumping, "False")
#pragma GE_DefaultValue(DistanceFogColor, "255 255 255")
#pragma GE_DefaultValue(FogStartDistLow, "50.0")
#pragma GE_DefaultValue(FogStartDistHigh, "50.0")
#pragma GE_DefaultValue(TotalFogDistLow, "200.0")
#pragma GE_DefaultValue(TotalFogDistHigh, "200.0")
#pragma GE_DefaultValue(UseFarClipPlane, "False")
#pragma GE_DefaultValue(FarClipPlaneDistLow, "500.0")
#pragma GE_DefaultValue(FarClipPlaneDistHigh, "500.0")
#pragma GE_Documentation(Gravity, "Downward force of gravity")
#pragma GE_Documentation(RealJumping, "if true use realistic jumping")
#pragma GE_Documentation(Speed, "Player speed of motion")
#pragma GE_Documentation(JumpSpeed, "Player upward jump speed")
#pragma GE_Documentation(MaxLookUpAngle, "Maximum angle (in degrees) player can look up")
#pragma GE_Documentation(MaxLookDwnAngle, "Maximum angle (in degrees) player can look down")
#pragma GE_Documentation(StepHeight, "Highest step a player automatically climbs")
#pragma GE_Documentation(EnableDistanceFog,"TRUE to enable distance fogging")
#pragma GE_Documentation(DistanceFogColor,"Color for distance fog")
#pragma GE_Documentation(FogStartDistLow,"Min start distance from camera for fog")
#pragma GE_Documentation(FogStartDistHigh,"Max start distance from camera for fog")
#pragma GE_Documentation(TotalFogDistLow,"Min distance from camera where fog is solid")
#pragma GE_Documentation(TotalFogDistHigh,"Max distance from camera where fog is solid")
#pragma GE_Documentation(UseFarClipPlane, "TRUE if far clipping plane is to be used")
#pragma GE_Documentation(FarClipPlaneDistLow, "Min distance past which nothing will be rendered")
#pragma GE_Documentation(FarClipPlaneDistHigh, "Max distance past which nothing will be rendered")
} EnvironmentSetup;

//	Corona
//
//	Put a corona around a light or other object.  This entity is
//	..courtesy of Ralph Deane, although I reformatted the code to
//	..match my own sick sense of style!

#pragma GE_Type("icons\\sf_corona.bmp")
typedef struct _Corona
{
#pragma	GE_Private
	int *effect;		  // pointer to array of effect indexs
	geBoolean active;		  // is activated
	geVec3d  OriginOffset;      //Translation from model center
#pragma GE_Published
	geVec3d origin;
	char 	*szEntityName;
	geFloat FadeTime;
	geFloat 	MinRadius;
	geFloat	MaxRadius;
	geFloat	MaxVisibleDistance;
	geFloat	MaxRadiusDistance;
	geFloat	MinRadiusDistance;
	GE_RGBA Color;
	char 	*EntityName;	// Name of entity to attach to
	geWorld_Model *Model;	// Name of model to attach to
	char 	*BoneName;	// Name of actor bone to attach to
	char 	*TriggerName;	// Name of trigger entity
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(szEntityName, "")
#pragma GE_DefaultValue(FadeTime, "0")
#pragma GE_DefaultValue(MinRadius, "1")
#pragma GE_DefaultValue(MaxRadius, "20")
#pragma GE_DefaultValue(MaxVisibleDistance, "2000")
#pragma GE_DefaultValue(MaxRadiusDistance, "1000")
#pragma GE_DefaultValue(MinRadiusDistance, "100")
#pragma GE_DefaultValue(Color, "255.0 255.0 255.0")
#pragma GE_DefaultValue(EntityName, "")
#pragma GE_DefaultValue(BoneName, "")
#pragma GE_DefaultValue(TriggerName, "")
#pragma GE_Documentation(szEntityName, "Name of this entity, if any")
#pragma GE_Documentation(FadeTime, "How long the fade takes to drop to zero visibility (seconds)")
#pragma GE_Documentation(MinRadius, "Minimum radius corona will ever drop to (texels)")
#pragma GE_Documentation(MaxRadius, "Maximum radius corona will ever increase to (texels)")
#pragma GE_Documentation(MaxVisibleDistance, "Maximum distance corona is visible at (texels)")
#pragma GE_Documentation(MaxRadiusDistance, "Above this distance, corona is capped at MaxRadius (texels)")
#pragma GE_Documentation(MinRadiusDistance, "Below this distance, corona is capped at MinRadius (texels)")
#pragma GE_Documentation(EntityName, "Name of entity to attach to")
#pragma GE_Documentation(Model, "Name of model to attach to")
#pragma GE_Documentation(BoneName, "Name of actor bone to attach to")
#pragma GE_Documentation(TriggerName, "Name of trigger entity to use")
}	Corona;

//	DynamicLight
//
//	A light that dynamically illuminates the level area.  This entity is
//	..courtesy of Ralph Deane, although I reformatted the code to
//	..match my own sick sense of style!

#pragma GE_Type("icons\\sf_dynlight.bmp")
typedef struct _DynamicLight
{
#pragma	GE_Private
	geFloat	LastTime;
	geFloat	IntervalWidth;
	int		NumFunctionValues;
	geLight *DynLight;
	geBoolean active;		  // is activated
	//MOD010124 - Added next private variable
	geVec3d  OriginOffset;      //Translation from model center
#pragma GE_Published
	geVec3d	origin;
	char	*szEntityName;
	int		MinRadius;
	int		MaxRadius;
	geBoolean	InterpolateValues;
	char	*RadiusFunction;
	geFloat	RadiusSpeed;
	GE_RGBA	Color;
	char 	*EntityName;	// Name of entity to attach to
	geWorld_Model *Model;	// Name of model to attach to
	char 	*BoneName;	// Name of actor bone to attach to
	char 	*TriggerName;	// Name of trigger entity
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(szEntityName, "")
#pragma GE_DefaultValue(MinRadius, "50")
#pragma GE_DefaultValue(MaxRadius, "300")
#pragma GE_DefaultValue(Color, "255.0 255.0 255.0")
#pragma GE_DefaultValue(RadiusFunction, "aza")
#pragma GE_DefaultValue(RadiusSpeed, "2.0")
#pragma GE_DefaultValue(InterpolateValues, "True")
#pragma GE_DefaultValue(EntityName, "")
#pragma GE_DefaultValue(BoneName, "")
#pragma GE_DefaultValue(TriggerName, "")
#pragma GE_Documentation(szEntityName, "Name of this entity, if any")
#pragma GE_Documentation(MinRadius, "Minimum radius of the light (texels)")
#pragma GE_Documentation(MaxRadius, "Maximum radius of the light (texels)")
#pragma GE_Documentation(InterpolateValues, "True or False.  Whether to interpolate between the RadiusFunction values")
#pragma GE_Documentation(RadiusFunction, "a-z, repeated (E.g. aabdfzddsfdz)  Specify light values over time.  a = Minimum z = maximum")
#pragma GE_Documentation(RadiusSpeed, "How fast to run through RadiusFunction values (seconds)")
#pragma GE_Documentation(EntityName, "Name of entity to attach to")
#pragma GE_Documentation(Model, "Name of model to attach to")
#pragma GE_Documentation(BoneName, "Name of actor bone to attach to")
#pragma GE_Documentation(TriggerName, "Name of trigger entity to use")
#pragma GE_Documentation(Color, "Color of the light")
}	DynamicLight;

//	ElectricBoltTerminus
//
//	Defines the target for an electrical bolt effect.  This entity is
//	..courtesy of Ralph Deane, although I reformatted the code to
//	..match my own sick sense of style!

#pragma GE_Type("icons\\sf_elecbterm.bmp")
typedef struct _ElectricBoltTerminus
{
#pragma GE_Private
	geVec3d  OriginOffset;      //Translation from model center
#pragma GE_Published
	geVec3d	origin;
	char	*szEntityName;
	char 	*EntityName;	// Name of entity to attach to
	geWorld_Model *Model;	// Name of model to attach to
	char 	*BoneName;	// Name of actor bone to attach to
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(szEntityName, "")
#pragma GE_DefaultValue(EntityName, "")
#pragma GE_DefaultValue(BoneName, "")
#pragma GE_Documentation(szEntityName, "Name of this entity, if any")
#pragma GE_Documentation(EntityName, "Name of entity to attach to")
#pragma GE_Documentation(Model, "Name of model to attach to")
#pragma GE_Documentation(BoneName, "Name of actor bone to attach to")
}	ElectricBoltTerminus;

//	ElectricBolt
//
//	Emitter for electrical bolt effects.  This entity is
//	..courtesy of Ralph Deane, although I reformatted the code to
//	..match my own sick sense of style!

typedef struct Electric_BoltEffect Electric_BoltEffect;

#pragma GE_Type("icons\\sf_electricb.bmp")
typedef struct _ElectricBolt
{
#pragma	GE_Private
	geFloat	LastTime;
	geFloat	LastBoltTime;
	Electric_BoltEffect	*Bolt;
	geBoolean active;		  // is activated
	int *effect;		  // pointer to array of effect indexs
	geBitmap	*Bitmap;
	geVec3d  OriginOffset;      //Translation from model center
#pragma GE_Published
	geVec3d	origin;
	char	*szEntityName;
	char	*SoundFile;
	geFloat fRadius;		// Maximum audible radius for sound
	char	*BmpName;
	char	*AlphaName;
	int		Width;
	int		NumPoints;
	geBoolean	Intermittent;
	geFloat	MinFrequency;
	geFloat	MaxFrequency;
	geFloat	Wildness;
	ElectricBoltTerminus *Terminus;
	int	DominantColor;
	GE_RGBA	Color;
	char 	*EntityName;	// Name of entity to attach to
	geWorld_Model *Model;	// Name of model to attach to
	char 	*BoneName;	// Name of actor bone to attach to
	char 	*TriggerName;	// Name of trigger entity
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(szEntityName, "")
#pragma GE_DefaultValue(SoundFile, "")
#pragma GE_DefaultValue(fRadius, "200.0")
#pragma GE_DefaultValue(BmpName, "")
#pragma GE_DefaultValue(AlphaName, "")
#pragma GE_DefaultValue(Width, "8")
#pragma GE_DefaultValue(NumPoints, "64")
#pragma GE_DefaultValue(Intermittent, "True")
#pragma GE_DefaultValue(MinFrequency, "4.0")
#pragma GE_DefaultValue(MaxFrequency, "1.0")
#pragma GE_DefaultValue(Wildness, "0.5")
#pragma GE_DefaultValue(DominantColor, "2")
#pragma GE_DefaultValue(Color, "160.0 160.0 255.0")
#pragma GE_DefaultValue(EntityName, "")
#pragma GE_DefaultValue(BoneName, "")
#pragma GE_DefaultValue(TriggerName, "")
#pragma GE_Documentation(szEntityName, "Name of this entity, if any")
#pragma GE_Documentation(SoundFile, "Name of sound of bolt")
#pragma GE_Documentation(fRadius, "Audible range for sound")
#pragma GE_Documentation( BmpName, "Name of bitmap file to use" )
#pragma GE_Documentation( AlphaName, "Name of alpha bitmap file to use" )
#pragma GE_Documentation(Width, "Width in texels of the bolt")
#pragma GE_Documentation(NumPoints, "Power of 2.  Number of control points.  Stick to 32, 64, 128.")
#pragma GE_Documentation(Intermittent, "Whether the bolt is continuous (False), or random (True)")
#pragma GE_Documentation(MinFrequency, "If the bolt is intermittent, the min. time in msec. between zaps")
#pragma GE_Documentation(MaxFrequency, "If the bolt is intermittent, the max. time in msec. between zaps")
#pragma GE_Documentation(Wildness, "Degree of 'freneticity' of the bolt (0 to 1)")
#pragma GE_Documentation(Terminus, "Where the other end of the bolt is")
#pragma GE_Documentation(DominantColor, "Specifies the dominant color of the bolt. 0 = Red, 1 = Green, 2 = Blue")
#pragma GE_Documentation(Color, "Base color of the bolt.  The two non-dominant color values MUST be the same!")
#pragma GE_Documentation(EntityName, "Name of entity to attach to")
#pragma GE_Documentation(Model, "Name of model to attach to")
#pragma GE_Documentation(BoneName, "Name of actor bone to attach to")
#pragma GE_Documentation(TriggerName, "Name of trigger entity to use")
}	ElectricBolt;

//	TextureProc
//
//	Procedural texture entity, contributed by Ralph Deane.  Ralph
//	..is a MAJOR contributor to RGF - thanks, Ralph!

typedef struct Procedural	Procedural;

#pragma GE_Type("icons\\sf_proctex.bmp")
typedef struct _TextureProc
{
#pragma GE_Private
	Procedural *Proc;
#pragma GE_Published
	geVec3d origin;			// Entity origin
	char *szTextureName;		// Name of texture to replace
	char *szProcName;		// Proc to use for texture
	char *Parameter;
	geBoolean DistanceFlag;
	geFloat Distance;
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(szTextureName, "")
#pragma GE_DefaultValue(szProcName, "")
#pragma GE_DefaultValue(Parameter, "")
#pragma GE_DefaultValue(DistanceFlag, "False")
#pragma GE_DefaultValue(Distance, "400")
#pragma GE_Documentation(szTextureName, "Name of texture to replace")
#pragma GE_Documentation(szProcName, "Procedural to replace texture")
#pragma GE_Documentation(Parameter, "Parameter string for procedural")
#pragma GE_Documentation(DistanceFlag, "True if texture stops rendering past defined distance")
#pragma GE_Documentation(Distance, "Distance after which texture stops rendering")
} TextureProc;

//	PathPoint
//
//	Defines a single point of a path to be followed by other
//	..entities.

#pragma GE_Type("icons\\ai_pathpoint.bmp")
typedef struct _PathPoint
{
#pragma	GE_Private
#pragma GE_Published
	geVec3d	origin;
	int PointType;						// Type of this point
	char *PointName;					// Name of this point
	char *NextPointName;			// Name of next point in list, if any
	geFloat Range;							// For ranging, ranging distance
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(PointType, "0")
#pragma GE_DefaultValue(PointName, "")
#pragma GE_DefaultValue(NextPointName, "")
#pragma GE_DefaultValue(Range, "0.0")
#pragma GE_Documentation(PointType, "0=Start, 1=Waypoint, 2=Ranging")
#pragma GE_Documentation(PointName, "Name of this pathpoint")
#pragma GE_Documentation(NextPointName, "Name of next point in path, if any")
#pragma GE_Documentation(Range,"If ranging point, max. ranging distance")
} PathPoint;

//	PathFollower
//
//	Binds an entity to a path, that is, the entity named in the
//	..PathFollower will be moved back and forth on the path
//	..whose first point is named in the entity.  Really, it
//	..all makes more sense in the real documentation..

#pragma GE_Type("icons\\ai_pathfollow.bmp")
typedef struct _PathFollower
{
#pragma GE_Private
	geFloat TimeInMotion;					// Used to compute speed between points
	geFloat LastMotionTick;				// Last time this entity was processed
	geBoolean bReady;					// Follower ready for use
	geBoolean bMoving;				// Follower is doing motion
	geBoolean bTriggered;			// Motion has been triggered
	geBoolean bForward;				// PAth motion direction flag
	geVec3d PathOrigin;				// Position of head of path
	int CurrentNodeType;			// Type of current node
	geVec3d CurrentPosition;	// Entities current position
	geFloat PointRange;					// If at ranged point, the range
	geVec3d CurrentTarget;		// Entities current target position
	int PathHandle;						// Handle if PATH and not RANGED POINT ONLY
#pragma GE_Published
	geVec3d origin;
	char *PathFirstNode;			// Name of first node in path to follow
	char *EntityName;					// Name of entity to move along path
	geFloat Speed;							// Speed along path, in world units/second
	geWorld_Model *Trigger;		// Motion trigger, if any
	geFloat RangeTrigger;				// Range trigger for motion, if any
	geBoolean MotionLoops;		// TRUE if motion will loop (bidirectional)
	int RandomRollTime;				// For RANGED point, # of milliseconds between new point generation
	geBoolean CollisionTest;	// TRUE if entity motion needs to be collision tested
	geBoolean Reverse;
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(PathFirstNode, "")
#pragma GE_DefaultValue(EntityName, "")
#pragma GE_DefaultValue(Speed, "30.0")
#pragma GE_DefaultValue(RangeTrigger, "0.0")
#pragma GE_DefaultValue(MotionLoops, "False")
#pragma GE_DefaultValue(RandomRollTime, "5000")
#pragma GE_DefaultValue(CollisionTest, "False")
#pragma GE_DefaultValue(Reverse, "True")
#pragma GE_Documentation(PathFirstNode, "Name of first node of path to follow")
#pragma GE_Documentation(EntityName, "Name of entity to move on path")
#pragma GE_Documentation(Speed, "Speed of motion in world units/second")
#pragma GE_Documentation(Trigger, "Model to use as motion start trigger, if model-collision triggered")
#pragma GE_Documentation(RangeTrigger, "Radius of trigger, if range triggered")
#pragma GE_Documentation(MotionLoops, "TRUE if motion loops (bidirectional)")
#pragma GE_Documentation(RandomRollTime,"Milliseconds between random point generation for ranged points")
#pragma GE_Documentation(CollisionTest, "TRUE if basic motion collision testing is used")
#pragma GE_Documentation(Reverse, "TRUE if entity turns around at end")
} PathFollower;

//	PlayerSetup
//
//	This entity contains pointers to various information files that
//	..define game-wide player states: player attributes, HUD layout,
//	..etc,

#pragma GE_Type("icons\\psetup.bmp")
typedef struct _PlayerSetup
{
#pragma GE_Private
#pragma GE_Published
	geVec3d origin;
	char *HUDInfoFile;								// File containing HUD description
	char *AttributeInfoFile;					// File containing attribute declarations
	char *OpeningCutScene;						// AVI for opening cut scene, if any
	geFloat PlayerScaleFactor;				// Player scaling factor
	geFloat ActorScaleFactor;
	geVec3d ActorRotation;						// Actor rotation to set default orientation
	geBoolean HeadBobbing;					// eaa3 12/18/2000 Head bobbing enable
	geFloat HeadBobSpeed;					// eaa3 12/18/2000 Speed of head bob
	geFloat HeadBobLimit;					// eaa3 12/18/2000 Max. amount of head bob
	int LevelViewPoint;
	geBoolean LevelViewAllowed;
	geBoolean Level1stAllowed;
	geBoolean Level3rdAllowed;
	geBoolean LevelIsoAllowed;
	geFloat Level1stHeight;
	geFloat Level3rdHeight;
	geFloat Level3rdAngle;
	geFloat Level3rdDistance;
	geFloat LevelIsoHeight;
	geFloat LevelIsoAngleX;
	geFloat LevelIsoAngleY;
	geFloat LevelIsoDistance;
	geBoolean UseDeathFog;
	GE_RGBA DeathFogColor;
	char *DeathMessage;
	int DeathFontSize;
	char *Deathlevel;
	geFloat LiteRadiusMin;
	geFloat LiteRadiusMax;
	GE_RGBA LiteColorMin;
	GE_RGBA LiteColorMax;
	geFloat LiteIntensity;
	geFloat StaminaDelay;
	geBoolean Allow3rdLook;
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(HUDInfoFile, "hud.ini")
#pragma GE_DefaultValue(AttributeInfoFile, "player.ini")
#pragma GE_DefaultValue(OpeningCutScene, "")
#pragma GE_DefaultValue(PlayerScaleFactor, "1.0")
#pragma GE_DefaultValue(ActorScaleFactor, "1.0")
#pragma GE_DefaultValue(ActorRotation, "0 180 0")
#pragma GE_DefaultValue(HeadBobbing, "False")
#pragma GE_DefaultValue(HeadBobSpeed, "1.0")
#pragma GE_DefaultValue(HeadBobLimit, "8.0")
#pragma GE_DefaultValue(LevelViewPoint, "0")
#pragma GE_DefaultValue(Allow3rdLook, "True")
#pragma GE_DefaultValue(LevelViewAllowed, "False")
#pragma GE_DefaultValue(Level1stAllowed, "False")
#pragma GE_DefaultValue(Level3rdAllowed, "False")
#pragma GE_DefaultValue(LevelIsoAllowed, "False")
#pragma GE_DefaultValue(Level1stHeight, "0.0")
#pragma GE_DefaultValue(Level3rdHeight, "0.0")
#pragma GE_DefaultValue(Level3rdAngle, "0.0")
#pragma GE_DefaultValue(Level3rdDistance, "0.0")
#pragma GE_DefaultValue(LevelIsoHeight, "0.0")
#pragma GE_DefaultValue(LevelIsoAngleX, "0.0")
#pragma GE_DefaultValue(LevelIsoAngleY, "0.0")
#pragma GE_DefaultValue(LevelIsoDistance, "0.0")
#pragma GE_DefaultValue(UseDeathFog, "False")
#pragma GE_DefaultValue(DeathFogColor, "255 0 0")
#pragma GE_DefaultValue(DeathMessage, "")
#pragma GE_DefaultValue(DeathFontSize, "0")
#pragma GE_DefaultValue(Deathlevel, "")
#pragma GE_DefaultValue(LiteRadiusMin, "150.0" )
#pragma GE_DefaultValue(LiteRadiusMax, "300.0" )
#pragma GE_DefaultValue(LiteColorMin, "255.0 255.0 255.0" )
#pragma GE_DefaultValue(LiteColorMax, "255.0 255.0 255.0" )
#pragma GE_DefaultValue(LiteIntensity, "1.0" )
#pragma GE_DefaultValue(StaminaDelay, "1" )
#pragma GE_Documentation(HUDInfoFile, "HUD configuration file")
#pragma GE_Documentation(AttributeInfoFile, "Player attribute configuration file")
#pragma GE_Documentation(OpeningCutScene, "Cut scene to play on game start-up")
#pragma GE_Documentation(PlayerScaleFactor, "Value to scale player actor by")
#pragma GE_Documentation(ActorScaleFactor, "Value to scale actor by") 
#pragma GE_Documentation(ActorRotation, "Rotation needed to properly orient player in game")
#pragma GE_Documentation(PlayerScaleFactor, "Value to scale player by") 	// update #2
#pragma GE_Documentation(ActorScaleFactor, "Value to scale actor by") 
#pragma GE_Documentation(HeadBobbing, "TRUE if players head bobs when moving")
#pragma GE_Documentation(HeadBobSpeed,"Speed of head bob in increments per move")
#pragma GE_Documentation(HeadBobLimit, "Maximum amount of head bob, in total units")
#pragma GE_Documentation(LevelViewPoint,"Initial Player viewpoint: 0=1st person, 1=3rd person w/panning, 2=isometric")
#pragma GE_Documentation(LevelViewAllowed, "TRUE if viewpoint changing allowed")
#pragma GE_Documentation(Allow3rdLook, "TRUE if 3rd person look up/down allowed")
#pragma GE_Documentation(Level1stAllowed, "TRUE if 1st person allowed")
#pragma GE_Documentation(Level3rdAllowed, "TRUE if 3rd person allowed")
#pragma GE_Documentation(LevelIsoAllowed, "TRUE if isometric allowed")
#pragma GE_Documentation(Level1stHeight, "Height of 1st person camera")
#pragma GE_Documentation(Level3rdHeight, "Height of 3rd person camera")
#pragma GE_Documentation(Level3rdAngle, "Angle of 3rd person camera")
#pragma GE_Documentation(Level3rdDistance, "Distance of 3rd person camera behind player")
#pragma GE_Documentation(LevelIsoHeight, "Height of isometric camera")
#pragma GE_Documentation(LevelIsoAngleX, "Angle up of isometric camera")
#pragma GE_Documentation(LevelIsoAngleY, "Angle around of isometric camera")
#pragma GE_Documentation(LevelIsoDistance, "Distance of isometric camera behind player")
#pragma GE_Documentation(UseDeathFog, "If true use fog swirl at death")
#pragma GE_Documentation(DeathFogColor, "Color of fog used at death")
#pragma GE_Documentation(DeathMessage, "Message to display at death")
#pragma GE_Documentation(DeathFontSize, "Font size of death message")
#pragma GE_Documentation(Deathlevel, "Level to change to at death")
#pragma GE_Documentation(LiteRadiusMin, "Min light radius")
#pragma GE_Documentation(LiteRadiusMax, "Max light radius")
#pragma GE_Documentation(LiteColorMin, "Min light color values")
#pragma GE_Documentation(LiteColorMax, "Max light color values")
#pragma GE_Documentation(LiteIntensity, "Max light intensity")
#pragma GE_Documentation(StaminaDelay, "Number of seconds before Stamina increases 1 point")

} PlayerSetup;

//	EnvAudioSetup
//
//	This entity handles defining the Foley played when an actor (player
//	..avatar, AI-driven actor) moves in certain contents.

#pragma GE_Type("icons\\e_audio.bmp")
typedef struct _EnvAudioSetup
{
#pragma GE_Published
	geVec3d origin;									// Entity origin
	char *DefaultMotionSound;				// Default motion sound .WAV file
	char *SoundWhenInWater;					// Motion sound when in water
	char *SoundWhenInLava;					// Motion sound when in lava
	char *SoundWhenInToxicGas;			// Motion sound when in toxic gas
	char *SoundWhenInZeroG;					// Motion sound when in zero-G
	char *SoundWhenInFrozen;				// Motion sound when on frozen surface
	char *SoundWhenInSludge;				// Motion sound when in viscous sludge
	char *SoundWhenInSlowMotion;		// Motion sound when in slo-mo area
	char *SoundWhenInFastMotion;		// Motion sound when in fast-mo area
	char *SoundWhenUsingLadders;
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(DefaultMotionSound, "")
#pragma GE_DefaultValue(SoundWhenInWater, "")
#pragma GE_DefaultValue(SoundWhenInLava, "")
#pragma GE_DefaultValue(SoundWhenInToxicGas, "")
#pragma GE_DefaultValue(SoundWhenInZeroG, "")
#pragma GE_DefaultValue(SoundWhenInFrozen, "")
#pragma GE_DefaultValue(SoundWhenInSludge, "")
#pragma GE_DefaultValue(SoundWhenInSlowMotion, "")
#pragma GE_DefaultValue(SoundWhenInFastMotion, "")
#pragma GE_DefaultValue(SoundWhenUsingLadders, "")
#pragma GE_Documentation(DefaultMotionSound, "Default motion sound")
#pragma GE_Documentation(SoundWhenInWater, "Sound to use when moving in water")
#pragma GE_Documentation(SoundWhenInLava, "Sound to use when moving in lava")
#pragma GE_Documentation(SoundWhenInToxicGas, "Sound to use when moving in toxic gas")
#pragma GE_Documentation(SoundWhenInZeroG, "Sound to use when moving in zero-G")
#pragma GE_Documentation(SoundWhenInFrozen, "Sound to use when moving in frozen area")
#pragma GE_Documentation(SoundWhenInSludge, "Sound to use when moving in sludge")
#pragma GE_Documentation(SoundWhenInSlowMotion, "Sound to use when moving in slow motion area")
#pragma GE_Documentation(SoundWhenInFastMotion, "Sound to use when moving in fast motion area")
#pragma GE_Documentation(SoundWhenUsingLadders, "Sound to use when climbing ladders")
} EnvAudioSetup;

//	Ralph Deane's Floating Particles Entity

#pragma GE_Type("icons\\wm_floatpart.bmp")
typedef struct FloatingParticles
{
#pragma	GE_Private
	int 	*EffectList;
	int	EffectCount;
	geXForm3d	*Xf;
	geFloat		*Speed;
	geVec3d		BasePos;
	geBitmap	*Bitmap;
	geBoolean active;
	geVec3d  OriginOffset;      //Translation from model center
#pragma GE_Published
	geVec3d	origin;
	char	*szEntityName;
	GE_RGBA	Color;
	geFloat	Scale;
	int		ParticleCount;
	geFloat	Radius;
	geFloat	Height;
	geFloat	XSlant;
	geFloat	ZSlant;
	geFloat	MinSpeed;
	geFloat	MaxSpeed;
	char	*BmpName;
	char	*AlphaName;
	char 	*EntityName;	// Name of entity to attach to
	geWorld_Model *Model;	// Name of model to attach to
	char 	*BoneName;	// Name of actor bone to attach to
	char 	*TriggerName;	// Name of trigger entity
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(szEntityName, "")
#pragma GE_DefaultValue( Color, "255.0 255.0 255.0" )
#pragma GE_DefaultValue( Scale, "1.0" )
#pragma GE_DefaultValue( ParticleCount, "10" )
#pragma GE_DefaultValue( Radius, "100" )
#pragma GE_DefaultValue( Height, "100" )
#pragma GE_DefaultValue( XSlant, "0.65" )
#pragma GE_DefaultValue( ZSlant, "0.65" )
#pragma GE_DefaultValue( MinSpeed, "60.0" )
#pragma GE_DefaultValue( MaxSpeed, "150.0" )
#pragma GE_DefaultValue(EntityName, "")
#pragma GE_DefaultValue(BoneName, "")
#pragma GE_DefaultValue(TriggerName, "")
#pragma GE_Documentation(szEntityName, "Name of this entity, if any")
#pragma GE_Documentation( Color, "Texture color" )
#pragma GE_Documentation( Scale, "What scale to use for the bitmap" )
#pragma GE_Documentation( ParticleCount, "How many particles to use" )
#pragma GE_Documentation( Radius, "Radius of particle cylinder" )
#pragma GE_Documentation( Height, "Max height each particle will go from base" )
#pragma GE_Documentation( XSlant, "Upwards slant on X axis" )
#pragma GE_Documentation( ZSlant, "Upwards slant on X axis" )
#pragma GE_Documentation( MinSpeed, "Min speed of each particle" )
#pragma GE_Documentation( MaxSpeed, "Max speed of each particle" )
#pragma GE_Documentation( BmpName, "Name of bitmap file to use" )
#pragma GE_Documentation( AlphaName, "Name of alpha bitmap file to use" )
#pragma GE_Documentation(EntityName, "Name of entity to attach to")
#pragma GE_Documentation(Model, "Name of model to attach to")
#pragma GE_Documentation(BoneName, "Name of actor bone to attach to")
#pragma GE_Documentation(TriggerName, "Name of trigger entity to use")
} FloatingParticles;

//	Ralph Deane's Chaos Procedural

#pragma GE_Type("icons\\sf_chaos.bmp")
typedef struct EChaos
{
#pragma	GE_Private
	geBitmap	*CAttachBmp;
	geBitmap	*OriginalBmp;
	geBitmap	*WorkBmp;
	geFloat		XOffset;
	geFloat		YOffset;
	int		SegmentSize;
	int		SegmentCount;
#pragma GE_Published
	geVec3d		origin;
	int		MaxXSway;
	int		MaxYSway;
	geFloat		XStep;
	geFloat		YStep;
	char		*AttachBmp;
#pragma GE_Origin(origin)			
#pragma GE_DefaultValue( MaxXSway, "10" )
#pragma GE_DefaultValue( MaxYSway, "10" )
#pragma GE_DefaultValue( XStep, "1.0" )
#pragma GE_DefaultValue( YStep, "1.0" )
#pragma GE_Documentation( MaxXSway, "Total horizontal texture pixel sway" )
#pragma GE_Documentation( MaxYSway, "Total vertical texture pixel sway" )
#pragma GE_Documentation( XStep, "Horizontal scroll speed" )
#pragma GE_Documentation( YStep, "Vertical scroll speed" )
#pragma GE_Documentation( AttachBmp, "Name of the texture to attach this effect to" )
} EChaos;

//	Ralph Deane's Rain Effect

#pragma GE_Type("icons\\sf_rain.bmp")
typedef struct Rain
{
#pragma GE_Private
	int 	*EffectList;
	int		EffectCount;
	geBoolean active;
	geVec3d  OriginOffset;      //Translation from model center
#pragma GE_Published
	geVec3d		origin;
	char	*szEntityName;
	geVec3d		Gravity;
	geFloat		Radius;
	geFloat		Severity;
	geFloat		DropLife;
	GE_RGBA		ColorMin;
	GE_RGBA		ColorMax;
	char		*BmpName;
	char		*AlphaName;
	char 	*EntityName;	// Name of entity to attach to
	geWorld_Model *Model;	// Name of model to attach to
	char 	*BoneName;	// Name of actor bone to attach to
	char 	*TriggerName;	// Name of trigger entity
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(szEntityName, "")
#pragma GE_DefaultValue( Gravity, "0.0 -60.0 0.0" )
#pragma GE_DefaultValue( Radius, "100.0" )
#pragma GE_DefaultValue( Severity, "0.5" )
#pragma GE_DefaultValue( DropLife, "1.0" )
#pragma GE_DefaultValue( ColorMin, "255.0 255.0 255.0" )
#pragma GE_DefaultValue( ColorMax, "255.0 255.0 255.0" )
#pragma GE_DefaultValue(EntityName, "")
#pragma GE_DefaultValue(BoneName, "")
#pragma GE_DefaultValue(TriggerName, "")
#pragma GE_Documentation(szEntityName, "Name of this entity, if any")
#pragma GE_Documentation( Gravity, "The velocity applied to each drop each second (world units)" )
#pragma GE_Documentation( Radius, "The redius of the rain coverage aren (world units)" )
#pragma GE_Documentation( Severity, "How severe the rain is, 0.0 being tame, 1.0 being insanity" )
#pragma GE_Documentation( DropLife, "How long a drop lasts (seconds)" )
#pragma GE_Documentation( ColorMin, "Minimum RGB for each drop" )
#pragma GE_Documentation( ColorMax, "Maximum RGB for each drop" )
#pragma GE_Documentation( BmpName, "Name of bitmap file to use" )
#pragma GE_Documentation( AlphaName, "Name of alpha bitmap file to use" )
#pragma GE_Documentation(EntityName, "Name of entity to attach to")
#pragma GE_Documentation(Model, "Name of model to attach to")
#pragma GE_Documentation(BoneName, "Name of actor bone to attach to")
#pragma GE_Documentation(TriggerName, "Name of trigger entity to use")
} Rain;

//	Ralph Deane's Spout Entity

#pragma GE_Type("icons\\sf_spout.bmp")
typedef struct Spout
{
#pragma	GE_Private
	int		EffectList;
	int		EffectCount;
	geFloat		PauseTime;
	geBoolean 	PauseState;
	geBoolean active;
	geVec3d  OriginOffset;      //Translation from model center
#pragma GE_Published
	geVec3d		origin;
	char	*szEntityName;
	geVec3d		Angles;
	geFloat		ParticleCreateRate;
	geFloat		MinScale;
	geFloat		MaxScale;
	geFloat		MinSpeed;
	geFloat		MaxSpeed;
	geFloat		MinUnitLife;
	geFloat		MaxUnitLife;
	int		SourceVariance;
	int		DestVariance;
	GE_RGBA		ColorMin;
	GE_RGBA		ColorMax;
	geVec3d		Gravity;
	char		*BmpName;
	char		*AlphaName;
	geFloat		MinPauseTime;
	geFloat		MaxPauseTime;
	geFloat		TotalLife;
	geBoolean	Bounce;
	char 	*EntityName;	// Name of entity to attach to
	geWorld_Model *Model;	// Name of model to attach to
	char 	*BoneName;	// Name of actor bone to attach to
	char 	*TriggerName;	// Name of trigger entity
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(szEntityName, "")
#pragma GE_Angles(Angles)			
#pragma GE_DefaultValue( ParticleCreateRate, "0.1" )
#pragma GE_DefaultValue( MinScale, "1.0" )
#pragma GE_DefaultValue( MaxScale, "1.0" )
#pragma GE_DefaultValue( MinSpeed, "10.0" )
#pragma GE_DefaultValue( MaxSpeed, "30.0" )
#pragma GE_DefaultValue( MinUnitLife, "3.0" )
#pragma GE_DefaultValue( MaxUnitLife, "6.0" )
#pragma GE_DefaultValue( SourceVariance, "0" )
#pragma GE_DefaultValue( DestVariance, "1" )
#pragma GE_DefaultValue( ColorMin, "255.0 255.0 255.0" )
#pragma GE_DefaultValue( ColorMax, "255.0 255.0 255.0" )
#pragma GE_DefaultValue( Gravity, "0.0 0.0 0.0" )
#pragma GE_DefaultValue( TotalLife, "0.0" )
#pragma GE_DefaultValue( MinPauseTime, "0.0" )
#pragma GE_DefaultValue( MaxPauseTime, "0.0" )
#pragma GE_DefaultValue(EntityName, "")
#pragma GE_DefaultValue(BoneName, "")
#pragma GE_DefaultValue(TriggerName, "")
#pragma GE_DefaultValue(Bounce, "False")
#pragma GE_Documentation(szEntityName, "Name of this entity, if any")
#pragma GE_Documentation( Angles, "Direction in which particles will shoot" )
#pragma GE_Documentation( ParticleCreateRate, "Every how many seconds to add a new particle" )
#pragma GE_Documentation( MinScale, "Min scale of the textures" )
#pragma GE_Documentation( MaxScale, "Max scale of the textures" )
#pragma GE_Documentation( MinSpeed, "Min speed of the textures" )
#pragma GE_Documentation( MaxSpeed, "Max speed of the textures" )
#pragma GE_Documentation( MinUnitLife, "Min life of each texture" )
#pragma GE_Documentation( MaxUnitLife, "Max life of each texture" )
#pragma GE_Documentation( SourceVariance, "How much to vary spray source point" )
#pragma GE_Documentation( DestVariance, "How much to vary spray dest point" )
#pragma GE_Documentation( ColorMin, "Minimum RGB values for each particle" )
#pragma GE_Documentation( ColorMax, "Maximum RGB values for each particle" )
#pragma GE_Documentation( Gravity, "Gravity vector to apply to each particle" )
#pragma GE_Documentation( BmpName, "Name of bitmap file to use" )
#pragma GE_Documentation( AlphaName, "Name of alpha bitmap file to use" )
#pragma GE_Documentation( TotalLife, "How many seconds this spout lasts. Set to 0 for continuous." )
#pragma GE_Documentation( MinPauseTime, "Low range of randomly chosen pause time (seconds)" )
#pragma GE_Documentation( MaxPauseTime, "High range of randomly chosen pause time (seconds)" )
#pragma GE_Documentation(EntityName, "Name of entity to attach to")
#pragma GE_Documentation(Bounce, "True if particles bounce when they hit something")
#pragma GE_Documentation(Model, "Name of model to attach to")
#pragma GE_Documentation(BoneName, "Name of actor bone to attach to")
#pragma GE_Documentation(TriggerName, "Name of trigger entity to use")
} Spout;

//	Ralph Deane's Flame Entity

#pragma GE_Type("icons\\sf_flame.bmp")
typedef struct Flame
{
#pragma	GE_Private
	int		EffectList0;
	int		EffectList1;
	geBoolean active;
	geVec3d  OriginOffset;      //Translation from model center
#pragma GE_Published
	geVec3d		origin;
	char	*szEntityName;
	geVec3d		Angles;
	geFloat		Scale;
	geFloat		ParticleCreateRate;
	geFloat		MinScale;
	geFloat		MaxScale;
	geFloat		MinSpeed;
	geFloat		MaxSpeed;
	geFloat		MinUnitLife;
	geFloat		MaxUnitLife;
	int		SourceVariance;
	int		DestVariance;
	GE_RGBA		ColorMin;
	GE_RGBA		ColorMax;
	geVec3d		Gravity;
	char		*BmpName;
	char		*AlphaName;
	geFloat		TotalLife;
	
	geFloat		GRadiusMin;
	geFloat		GRadiusMax;
	GE_RGBA		GColorMin;
	GE_RGBA		GColorMax;
	geFloat		GIntensity;
	geBoolean	GCastShadows;
	char 	*EntityName;	// Name of entity to attach to
	geWorld_Model *Model;	// Name of model to attach to
	char 	*BoneName;	// Name of actor bone to attach to
	char 	*TriggerName;	// Name of trigger entity
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(szEntityName, "")
#pragma GE_Angles(Angles)
#pragma GE_DefaultValue( Angles, "-90.0.0 0.0" )
#pragma GE_DefaultValue( Scale, "1.0" )
#pragma GE_DefaultValue( ParticleCreateRate, "0.05" )
#pragma GE_DefaultValue( MinScale, "0.5" )
#pragma GE_DefaultValue( MaxScale, "0.5" )
#pragma GE_DefaultValue( MinSpeed, "10.0" )
#pragma GE_DefaultValue( MaxSpeed, "30.0" )
#pragma GE_DefaultValue( MinUnitLife, "0.5" )
#pragma GE_DefaultValue( MaxUnitLife, "1.0" )
#pragma GE_DefaultValue( SourceVariance, "3" )
#pragma GE_DefaultValue( DestVariance, "20" )
#pragma GE_DefaultValue( ColorMin, "255.0 255.0 255.0" )
#pragma GE_DefaultValue( ColorMax, "255.0 255.0 255.0" )
#pragma GE_DefaultValue( Gravity, "0.0.0 0.0" )
#pragma GE_DefaultValue( TotalLife, "0.0" )
#pragma GE_DefaultValue(EntityName, "")
#pragma GE_DefaultValue(BoneName, "")
#pragma GE_DefaultValue(TriggerName, "")
#pragma GE_Documentation(szEntityName, "Name of this entity, if any")
#pragma GE_Documentation( Angles, "Direction in which particles will shoot" )
#pragma GE_Documentation( Scale, "Scale of the flame" )
#pragma GE_Documentation( BoneName, "Which bone on the actor to hook to" )
#pragma GE_Documentation( ParticleCreateRate, "Every how many seconds to add a new particle" )
#pragma GE_Documentation( MinScale, "Min scale of the textures" )
#pragma GE_Documentation( MaxScale, "Max scale of the textures" )
#pragma GE_Documentation( MinSpeed, "Min speed of the textures" )
#pragma GE_Documentation( MaxSpeed, "Max speed of the textures" )
#pragma GE_Documentation( MinUnitLife, "Min life of each texture" )
#pragma GE_Documentation( MaxUnitLife, "Max life of each texture" )
#pragma GE_Documentation( SourceVariance, "How much to vary spray source point" )
#pragma GE_Documentation( DestVariance, "How much to vary spray dest point" )
#pragma GE_Documentation( ColorMin, "Minimum RGB values for each particle" )
#pragma GE_Documentation( ColorMax, "Maximum RGB values for each particle" )
#pragma GE_Documentation( Gravity, "Gravity vector to apply to each particle" )
#pragma GE_Documentation( BmpName, "Name of bitmap file to use" )
#pragma GE_Documentation( AlphaName, "Name of alpha bitmap file to use" )
#pragma GE_Documentation( TotalLife, "How many seconds this spout lasts. Set to 0 for continuous." )
	
#pragma GE_DefaultValue( GRadiusMin, "150.0" )
#pragma GE_DefaultValue( GRadiusMax, "300.0" )
#pragma GE_DefaultValue( GColorMin, "255.0 255.0 255.0" )
#pragma GE_DefaultValue( GColorMax, "255.0 255.0 255.0" )
#pragma GE_DefaultValue( GIntensity, "1.0" )
#pragma GE_DefaultValue( GCastShadows, "True" )
	
#pragma GE_Documentation(GRadiusMin, "Min light radius")
#pragma GE_Documentation(GRadiusMax, "Max light radius")
#pragma GE_Documentation(GColorMin, "Min color values")
#pragma GE_Documentation(GColorMax, "Max color values")
#pragma GE_Documentation(GIntensity, "Max light intensity")
#pragma GE_Documentation(GCastShadows, "Whether to cast shadows")
#pragma GE_Documentation(EntityName, "Name of entity to attach to")
#pragma GE_Documentation(Model, "Name of model to attach to")
#pragma GE_Documentation(BoneName, "Name of actor bone to attach to")
#pragma GE_Documentation(TriggerName, "Name of trigger entity to use")
} Flame;

//	NonPlayerCharacter
//
//	This entity defines the load point for a non-player character, one
//	..which is driven by the RealityFactory AI subsystem.

#pragma GE_Type("icons\\ai_nonplayer.bmp")
typedef struct _NonPlayerCharacter
{
#pragma GE_Private
	geBoolean alive;
	geBoolean active;
	geActor *Actor;				// Actor for this entity
	geFloat Tick;
	geBoolean bState;
	geBoolean CallBack;
	void *DBot;
	geVec3d Location;
#pragma GE_Published
	geVec3d origin;
	char *szEntityName;	// Name of this entity
	char *AnimIdle;
	char *AnimWalk;
	char *AnimWalkAttack;
	char *AnimAttack;
	char *AnimJump;
	char *AnimDie;
	char *TriggerName;
	char *ActivateTriggerName;
	char *szActorName;
	geVec3d ActorRotation;	// Rotation needed to properly orient actor
	geFloat Speed;
	geFloat Delay;
	geBoolean ReSpawn;
	int MaxNumber;
	geBoolean Gravity;
	geFloat Scale;
	int AttributeAmt;
	char *Attribute;
	geFloat DamageAmt;
	char *DamageAttribute;
	int Aggresiveness;
	geFloat DyingTime;
	geBoolean StopToAttack;
	geFloat AttackDelay;
	geBoolean Melee;
	char *Projectile;
	char *FireBone;
	geVec3d FireOffset;
	char *AttackSound;
	char *DieSound;
	int AimingSkill;
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(szEntityName, "")
#pragma GE_DefaultValue(szActorName, "")
#pragma GE_DefaultValue(AnimIdle, "Idle")
#pragma GE_DefaultValue(AnimWalkAttack, "Walk")
#pragma GE_DefaultValue(AnimAttack, "Shoot")
#pragma GE_DefaultValue(AnimWalk, "Walk")
#pragma GE_DefaultValue(AnimJump, "Jump")
#pragma GE_DefaultValue(AnimDie, "Die")
#pragma GE_DefaultValue(ActorRotation, "0 180 0.0")
#pragma GE_DefaultValue(Speed, "10.0")
#pragma GE_DefaultValue(DyingTime, "5.0")
#pragma GE_DefaultValue(AttackDelay, "2.0")
#pragma GE_DefaultValue(TriggerName, "")
#pragma GE_DefaultValue(ActivateTriggerName, "")
#pragma GE_DefaultValue(StopToAttack, "False")
#pragma GE_DefaultValue(Melee, "False")
#pragma GE_DefaultValue(Projectile, "")
#pragma GE_DefaultValue(FireBone, "")
#pragma GE_DefaultValue(FireOffset, "0 0 0")
#pragma GE_DefaultValue(Delay, "0.0")
#pragma GE_DefaultValue(ReSpawn, "False")
#pragma GE_DefaultValue(MaxNumber, "1")
#pragma GE_DefaultValue(Scale, "1.0")
#pragma GE_DefaultValue(Gravity, "True")
#pragma GE_DefaultValue(AttributeAmt, "100")
#pragma GE_DefaultValue(Attribute, "health")
#pragma GE_DefaultValue(DamageAmt, "10")
#pragma GE_DefaultValue(DamageAttribute, "health")
#pragma GE_DefaultValue(Aggresiveness, "5")
#pragma GE_DefaultValue(AttackSound, "")
#pragma GE_DefaultValue(DieSound, "")
#pragma GE_DefaultValue(AimingSkill, "5")
#pragma GE_Documentation(szEntityName, "Name of entity (used in scripting and triggers)")
#pragma GE_Documentation(TriggerName, "Name of trigger entity used to spawn NPC")
#pragma GE_Documentation(ActivateTriggerName, "Name of trigger entity used to activate NPC")
#pragma GE_Documentation(szActorName, "Name of actor to use")
#pragma GE_Documentation(ActorRotation, "Rotation needed to properly orient actor in game")
#pragma GE_Documentation(Speed, "Speed of actor")
#pragma GE_Documentation(AttackDelay, "Number of seconds before attacking again")
#pragma GE_Documentation(DyingTime, "Number of seconds before corpse dissappears")
#pragma GE_Documentation(Delay, "Number of seconds before respawning")
#pragma GE_Documentation(ReSpawn, "Is NPC respawnable?")
#pragma GE_Documentation(MaxNumber, "Maximum number of times to respawn, -1 is unlimited")
#pragma GE_Documentation(Scale, "Scale of actor")
#pragma GE_Documentation(StopToAttack, "True if stops motion to attack")
#pragma GE_Documentation(Gravity, "True if subject to gravity")
#pragma GE_Documentation(AnimIdle, "Idle animation name")
#pragma GE_Documentation(AnimWalk, "Walk animation name")
#pragma GE_Documentation(AnimWalkAttack, "Walking attack animation name")
#pragma GE_Documentation(AnimAttack, "Stopped attack animation name")
#pragma GE_Documentation(AnimJump, "Jump animation name")
#pragma GE_Documentation(AnimDie, "Dying animation name")
#pragma GE_Documentation(AttributeAmt, "Amount of Attribute NPC has")
#pragma GE_Documentation(Attribute, "Attribute that takes damage")
#pragma GE_Documentation(DamageAmt, "Amount of damage inflicted per melee attack")
#pragma GE_Documentation(DamageAttribute, "Attribute that gets damage")
#pragma GE_Documentation(Aggresiveness, "Agressiveness of NPC from 1 to 10")
#pragma GE_Documentation(Melee, "True if attack type is melee")
#pragma GE_Documentation(Projectile, "Name of projectile used")
#pragma GE_Documentation(FireBone, "Name of bone to launch projectile from")
#pragma GE_Documentation(FireOffset, "Offset from FireBone to launch point")
#pragma GE_Documentation(AttackSound, "Sound to play when attacking")
#pragma GE_Documentation(DieSound, "Sound to play when dying")
#pragma GE_Documentation(AimingSkill, "How accurate the aiming is, from 1 to 10")
} NonPlayerCharacter;

//	ReactiveAI
//
//	This entity defines the parameters for the RealityFactory
//	.."Reactive AI" system.  This allows the designer to bind a specific
//	..reactive AI response (along with animations for the various behaviors)
//	..to NPC Actors in the level.
//
//	..Note that the various strings for animation sequences may contain
//	..multiple, comma-seperated names - if there is more than one animation
//	..sequence defined in a type, RealityFactory randomly picks one of them
//	..to play for that state.  This provides a lot of "non-repeating" motion
//	..even for simple, reactive AI driven NPCs.  For example, multiple idle
//	..animations may be defined as:
//
//		IdleAnimation1,IdleAnimation2,IdleAnimation3
//
//	..This causes RealityFactory to switch randomly between each of these
//	..three animations while the NPC is in "idle" state.
//
//	Reactive AI is basically a state machine - the AI switches between
//	..states based on what the environment and circumstances dictate,
//	..rather than on any goal the AI may have (thus "reactive" AI).  The
//	..AI states are:
//
//	IDLE			NPC is idle, not at alert
//	ALERT			NPC is idle but alert
//	SEARCH		NPC is seeking an enemy
//	PATROL		NPC is patrolling along a path
//	ATTACK		NPC is attacking an enemy
//	RETREAT		NPC is retreating from an enemy
//  THREATEN  NPC is acting in a threatening manner, but not attacking
//
//	There are also "transient" states, states that are temporary and
//	..are a response to some transient outside event.  Unless otherwise
//	..stated, these states persist for the duration of a single full
//	..animation cycle, then the previous state is resumed:
//
//	DEATH			NPC is dying, NPC removed from game at end of animation
//	L_INJURY	NPC has been lightly injured
//	M_INJURY	NPC has taken moderate injury
//	H_INJURY	NPC has taken heavy injury
//
//	For NPCs on patrol paths, they can be LOCKED to the path - that is,
//	..they will not leave a set "patrol radius" of there current path
//	..point to pursue an enemy.
//
//	State transitions are performed by the RealityFactory Reactive AI
//	..class handler.  Some parameters for controlling the speed and direction
//	..of state changes are provided as part of the entity, but the entire
//	..concept is that most of the "hard work" of getting the NPCs to behave
//	..in a more or less "realistic" fashion is hidden in the AI engine.
//	..Of course, the reactive AI isn't as flexible as goal-driven AI, but
//	..constructing a behavior script for a true goal-oriented NPC is a
//	..great deal of work and should only be required for "name" entities
//	..and not the greater population of any game.
//
//	It's also important to note that the AI is as independent of the actor
//	..itself as possible.  In theory, you _should_ be able to bind the same
//	..AI to different actors, as long as the animation cycle names are the
//	..same between all bound actors.  This means that you can tune the AI's
//	..behavior once, and then use it across several different entities.
//
//	On reaction codes: there are three main "classes" that are reacted to:
//	..the player avatar, other NPCs not of the same AI, and NPCs driven
//	..by the same AI.  Each of these can be set seperately.  Reaction codes
//	..are:
//
//	R_IGNORE		NPC ignores class
//	R_NEUTRAL		NPC is neutral towards
//	R_FRIENDLY	NPC is friendly and helpful towards class
//	R_HOSTILE		NPC is hostile towards class
//	R_RANDOM		NPC reacts randomly towards class
//	R_ANTIPATHY	NPC has antipathy, but won't attack unless provoked
//	R_AFFINITY	NPC is non-threatening but inclined to help if needed
/*
#pragma GE_Type("icons\\ai_react.bmp")
typedef struct _ReactiveAI
{
#pragma GE_Private
#pragma GE_Published
	geVec3d Origin;
	char *IdleAnimations;							// Name of idle animation(s)
	char *DeathAnimations;						// Name of death animation(s)
	char *AttackAnimations;						// Name of attack animation(s)
	char *RetreatAnimations;					// Name of retreat animation(s)
	char *SearchingAnimations;				// Name of search animation(s)
	char *AlertAnimations;						// Name of idle-alert animation(s)
	char *PatrolAnimations;						// Name of path-patrol animation(s)
	char *ThreatenAnimations;					// Name of threatening animation(s)
	char *LightInjuryAnimations;			// Name of "took light injury" animations(s)
	char *MediumInjuryAnimations;			// Name of "took medium injury" animation(s)
	char *HeavyInjuryAnimations;			// Name of "took heavy injury" animation(s)
	char *PatrolPath;									// Path NPC will patrol, if any
	geBoolean LockedToPatrolPath;			// TRUE if NPC is locked to patrol path
	geFloat PatrolRadius;							// Max. deviation from path for pursuit
	int PlayerReaction;								// Player reaction code
	int OthersReactionCode;						// Non-player reaction code
	int SelfReactionCode;							// Reaction to others of same AI code
#pragma GE_Origin(Origin)
} ReactiveAI;
*/
//	Trigger - sets state to true when hit

#pragma GE_Type("icons\\wm_trigger.bmp")
typedef struct _Trigger
{
#pragma GE_Private
	geBoolean bInAnimation;		// Is trigger animating?
	geBoolean bTrigger;		// Has the trigger been activated?
	geFloat	AnimationTime;		// Ticks in animation
	geBoolean bInCollision;		// Trigger is colliding flag
	geBoolean bActive;		// Trigger can be used
	geFloat LastIncrement;		// Last tick increment
	geFloat tDoor;			// Trigger model animation position time
	geSound_Def *theSound;		// Sound def pointer
	int SoundHandle;		// For later modification of sound
	geBoolean bState;		// Trigger state
	geFloat time;				// on/off timer
	geBoolean isHit;
	geBoolean CallBack;
	int CallBackCount;
#pragma GE_Published
	geWorld_Model *Model;		// Trigger model
	geVec3d origin;			// Location of trigger
	geBoolean bOneShot;		// Trigger only works ONCE
	geBoolean bShoot;		// Trigger only works if shot
	geFloat TimeOn;			// seconds trigger is on
	geFloat TimeOff;			// seconds trigger is off
	char *szSoundFile;		// Name of sound to play on activation
	geFloat AnimationSpeed;		// Speed of the animation, in %
	char *szEntityName;		// Name of entity
	geBoolean bAudioLoops;		// Audio effect loops while animation plays
	char *TriggerName;
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(bOneShot, "False")
#pragma GE_DefaultValue(bShoot, "False")
#pragma GE_DefaultValue(TimeOn, "1")
#pragma GE_DefaultValue(TimeOff, "1")
#pragma GE_DefaultValue(szSoundFile, "")
#pragma GE_DefaultValue(szEntityName, "")
#pragma GE_DefaultValue(bAudioLoops, "False")
#pragma GE_DefaultValue(TriggerName, "")
#pragma GE_DefaultValue(AnimationSpeed, "1.0")
#pragma GE_Documentation(Model, "Trigger to use")
#pragma GE_Documentation(szSoundFile, "Sound file to play when trigger hit")
#pragma GE_Documentation(bOneShot, "1 if trigger only works once")
#pragma GE_Documentation(bShoot, "1 if trigger only works if shot")
#pragma GE_Documentation(TimeOn, "seconds that trigger is on")
#pragma GE_Documentation(TimeOff, "seconds that trigger is off")
#pragma GE_Documentation(szEntityName, "Name of entity (used in scripting and triggers)")
#pragma GE_Documentation(bAudioLoops, "TRUE if audio loops while animation plays")
#pragma GE_Documentation(TriggerName, "Name of trigger entity to use")
#pragma GE_Documentation(AnimationSpeed,"Speed of animation, 1.0 = 100% original")
} Trigger;

//	Logic entity

#pragma GE_Type("icons\\logicgate.bmp")
typedef struct _LogicGate
{
#pragma GE_Private
	geBoolean bState;
	geFloat time;
	geBoolean active;		  // is activated
	geBoolean OldState;
	geBoolean inDelay;
#pragma GE_Published
	geVec3d origin;		// Location of entity in world
	char *szEntityName;	// Name of this entity
	char *Trigger1Name;	// Name of trigger 1 entity
	char *Trigger2Name;	// Name of trigger 2 entity
	int Type;
	geFloat Delay;
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(szEntityName, "")
#pragma GE_DefaultValue(Trigger1Name, "")
#pragma GE_DefaultValue(Trigger2Name, "")
#pragma GE_DefaultValue(Type, "0")
#pragma GE_DefaultValue(Delay, "0")
#pragma GE_Documentation(szEntityName, "Name of entity (used in scripting and triggers)")
#pragma GE_Documentation(Trigger1Name, "Name of trigger 1 entity to use")
#pragma GE_Documentation(Trigger2Name, "Name of trigger 2 entity to use")
#pragma GE_Documentation(Type, "0-AND 1-DELAY 2-NOT 3-OR 4-XOR 5-FLIPFLOP 6-CALLBACK")
#pragma GE_Documentation(Delay, "Number of seconds to delay change")
} LogicGate;

// Message entity

#pragma GE_Type("icons\\message.bmp")
typedef struct _Message
{
#pragma GE_Private
	geBoolean active;
	geFloat Ticks;
#pragma GE_Published
	geVec3d origin;
	char *szEntityName;	// Name of this entity
	char *Text;
	geFloat TimeOn;
	char *TriggerName;
	int FontSize;
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(szEntityName, "")
#pragma GE_DefaultValue(Text, "")
#pragma GE_DefaultValue(TimeOn, "5.0")
#pragma GE_DefaultValue(TriggerName, "")
#pragma GE_DefaultValue(FontSize, "0")
#pragma GE_Documentation(szEntityName, "Name of entity (used in scripting and triggers)")
#pragma GE_Documentation(Text, "Text of message")
#pragma GE_Documentation(TimeOn, "Number of seconds to display message")
#pragma GE_Documentation(TriggerName, "Name of trigger entity to use")
#pragma GE_Documentation(FontSize, "Size of Font - range 0 to 29")

} Message;


//
// DecalDefine
//

#pragma GE_Type("icons\\decdefin.bmp")
typedef struct _DecalDefine
{
#pragma GE_Private
	geBitmap	*Bitmap;
#pragma GE_Published
	geVec3d	origin;
	int 	Type;
	char 	*BmpName;
	char 	*AlphaName;
	geFloat Width;
	geFloat Height;
	geFloat Alpha;
	geFloat LifeTime;
	GE_RGBA	Color;
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(Type, "0")
#pragma GE_DefaultValue(BmpName, "")
#pragma GE_DefaultValue(AlphaName, "")
#pragma GE_DefaultValue(Width, "10.0")
#pragma GE_DefaultValue(Height, "10.0")
#pragma GE_DefaultValue(Alpha, "255")
#pragma GE_DefaultValue(LifeTime, "60.0")
#pragma GE_DefaultValue(Color, "255.0 255.0 255.0")
#pragma GE_Documentation(Type, "Number of decal")
#pragma GE_Documentation(BmpName, "Name of bitmap")
#pragma GE_Documentation(AlphaName, "Name of alpha bitmap")
#pragma GE_Documentation(Width, "Width of Decal")
#pragma GE_Documentation(Height, "Height of Decal")
#pragma GE_Documentation(Alpha, "Transparency level - 0 to 255")
#pragma GE_Documentation(LifeTime, "Seconds that decal exists")
#pragma GE_Documentation(Color, "Color of decal")
} DecalDefine;

//
// WallDecal
//

#pragma GE_Type("icons\\walldecal.bmp")
typedef struct _WallDecal
{
#pragma GE_Private
	geBitmap *Bitmap;
	geBoolean active;
	gePoly *Poly;
#pragma GE_Published
	geVec3d	origin;
	geVec3d	Angle;
	char 	*BmpName;
	char 	*AlphaName;
	geFloat Width;
	geFloat Height;
	geFloat Alpha;
	GE_RGBA	Color;
	char *TriggerName;
#pragma GE_Origin(origin)
#pragma GE_Angles(Angle)
#pragma GE_DefaultValue(BmpName, "")
#pragma GE_DefaultValue(AlphaName, "")
#pragma GE_DefaultValue(Width, "10.0")
#pragma GE_DefaultValue(Height, "10.0")
#pragma GE_DefaultValue(Alpha, "255")
#pragma GE_DefaultValue(Color, "255.0 255.0 255.0")
#pragma GE_DefaultValue(TriggerName, "")
#pragma GE_Documentation(BmpName, "Name of bitmap")
#pragma GE_Documentation(AlphaName, "Name of alpha bitmap")
#pragma GE_Documentation(Width, "Width of Decal")
#pragma GE_Documentation(Height, "Height of Decal")
#pragma GE_Documentation(Alpha, "Transparency level - 0 to 255")
#pragma GE_Documentation(Color, "Color of decal")
#pragma GE_Documentation(TriggerName, "Name of trigger entity to use")
} WallDecal;


#pragma GE_Type("icons\\firepoint.bmp")
typedef struct _FirePoint
{
#pragma GE_Private
	geBoolean active;
	geFloat Tick;
	geFloat FireRate;
	geVec3d  OriginOffset;      //Translation from model center
	geVec3d  RealAngle;
#pragma GE_Published
	geVec3d	origin;
	geVec3d	Angle;
	char *szEntityName;		// Name of entity
	char 	*EntityName;	// Name of entity to attach to
	geWorld_Model *Model;	// Name of model to attach to
	char 	*BoneName;	// Name of actor bone to attach to
	char 	*TriggerName;	// Name of trigger entity
	char	*Projectile;
	geFloat MinFireRate;
	geFloat MaxFireRate;
	char *szSoundFile;
	char *Attribute;
#pragma GE_Origin(origin)
#pragma GE_Angles(Angle)
#pragma GE_DefaultValue(szEntityName, "")
#pragma GE_DefaultValue(Projectile, "bullet")
#pragma GE_DefaultValue(MinFireRate, "1.0")
#pragma GE_DefaultValue(MaxFireRate, "1.0")
#pragma GE_DefaultValue(EntityName, "")
#pragma GE_DefaultValue(BoneName, "")
#pragma GE_DefaultValue(TriggerName, "")
#pragma GE_DefaultValue(szSoundFile, "")
#pragma GE_DefaultValue(Attribute, "health")
#pragma GE_Documentation(szEntityName, "Name of entity (used in scripting and triggers)")
#pragma GE_Documentation(Projectile, "Name of projectile to fire")
#pragma GE_Documentation(MinFireRate, "Minimum number of seconds until weapon can fire again")
#pragma GE_Documentation(MaxFireRate, "Maximum number of seconds until weapon can fire again")
#pragma GE_Documentation(EntityName, "Name of entity to attach to")
#pragma GE_Documentation(Model, "Name of model to attach to")
#pragma GE_Documentation(BoneName, "Name of actor bone to attach to")
#pragma GE_Documentation(TriggerName, "Name of trigger entity to use")
#pragma GE_Documentation(szSoundFile, "Name of firing sound")
#pragma GE_Documentation(Attribute, "Attribute that projectile damages")
} FirePoint;

/*
#pragma GE_Type("icons\\turret.bmp")
typedef struct _Turret
{
#pragma GE_Private
	geBoolean active;
	geFloat HTime;
	geFloat VTime;
	geVec3d  OriginOffset;      //Translation from model center
#pragma GE_Published
	geVec3d	Origin;
	char *szEntityName;		// Name of entity
	char 	*TriggerName;	// Name of trigger entity
	char 	*FirePointName;	// Name of entity to attach to
	geWorld_Model *Model;	// Name of model to attach to
	geFloat HorzRotation;
	geFloat VertRotation;
	geFloat MaxSensorDist;
#pragma GE_Origin(Origin)
#pragma GE_DefaultValue(szEntityName, "")
#pragma GE_DefaultValue(FirePointName, "")
#pragma GE_DefaultValue(TriggerName, "")
#pragma GE_DefaultValue(HorzRotation, "10")
#pragma GE_DefaultValue(VertRotation, "10")
#pragma GE_DefaultValue(MaxSensorDist, "400")
#pragma GE_Documentation(szEntityName, "Name of entity (used in scripting and triggers)")
#pragma GE_Documentation(FirePointName, "Name of FirePoint entity")
#pragma GE_Documentation(Model, "Name of model to attach to")
#pragma GE_Documentation(TriggerName, "Name of trigger entity to use")
#pragma GE_Documentation(HorzRotation, "Degrees/sec that turret rotates horizontally")
#pragma GE_Documentation(VertRotation, "Degrees/sec that turret rotates vertically")
#pragma GE_Documentation(MaxSensorDist, "Maximum distance that turret will detect player")
} Turret;
*/

#pragma GE_Type("icons\\model.ico")
typedef struct _DestroyableModel
{
#pragma GE_Private
	geBoolean active;
	geBoolean bState;
	geBoolean CallBack;
	int CallBackCount;
	geFloat OriginalAmt;
#pragma GE_Published
	geVec3d	origin;
	char *szEntityName;		// Name of entity
	char 	*TriggerName;	// Name of trigger entity
	geWorld_Model *Model;	// Name of model to attach to
	geFloat AttributeAmt;	// Props health points
	char *Attribute;
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(szEntityName, "")
#pragma GE_DefaultValue(TriggerName, "")
#pragma GE_DefaultValue(AttributeAmt, "100")
#pragma GE_DefaultValue(Attribute, "health")
#pragma GE_Documentation(szEntityName, "Name of entity (used in scripting and triggers)")
#pragma GE_Documentation(Model, "Name of model to attach to")
#pragma GE_Documentation(TriggerName, "Name of trigger entity to use")
} DestroyableModel;

//	Model State Modifier
//
//	This entity allows the designer to set "state variables" for individual
//	..models in each level.  These states change how actors interact with the
//	..models, etc. - note that you have to make your world geometry element into
//	..a "model" in the World Editor in order to apply this entity to it!

#pragma GE_Type("icons\\wm_modattrib.bmp")
typedef struct _ModelStateModifier
{
#pragma GE_Private
	geFloat DTime;
	geFloat FTime;
	geBoolean DoForce;
	geBoolean DoDamage;
	geSound_Def *theFSound;			// Handle of audio effect
	geSound *FSoundHandle;	
	geSound_Def *theDSound;			// Handle of audio effect
	geSound *DSoundHandle;
#pragma GE_Published
	geVec3d origin;							// Origin of entity
	geWorld_Model *theModel;		// Model to assign modifiers to
	geBoolean ForceImparter;		// TRUE if model imparts force to player
	int ForceVector;						// Force vector ID (0=down,1=up,2=left,3=right, 4=forward, 5=backward)
	geFloat ForceAmount;				// Amount of force to impact
	geFloat ForceDecay;					// Amount to decay force imparted
	char *ForceAudioEffect;			// Audio effect to play when force is applied
	geFloat ForceTimeDelay;					// Delay (in sec.) before force or damage engages
	geFloat DamageTimeDelay;
	geBoolean DamageGenerator;	// TRUE if contact causes damage
	geFloat DamageAmount;				// Amount of damage per collision
	char *DamageAudioEffect;		// Audio effect to play when damage is applied
	char *DamageAttribute;
	char *DamageAltAttribute;
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(ForceImparter, "False")
#pragma GE_DefaultValue(DamageGenerator, "False")
#pragma GE_DefaultValue(ForceVector, "1")
#pragma GE_DefaultValue(ForceAmount, "100.0")
#pragma GE_DefaultValue(ForceDecay, "20.0")
#pragma GE_DefaultValue(ForceAudioEffect, "")
#pragma GE_DefaultValue(ForceTimeDelay, "1")
#pragma GE_DefaultValue(DamageTimeDelay, "1")
#pragma GE_DefaultValue(DamageAmount, "1.0")
#pragma GE_DefaultValue(DamageAudioEffect, "")
#pragma GE_DefaultValue(DamageAttribute, "")
#pragma GE_DefaultValue(DamageAltAttribute, "")
#pragma GE_Documentation(theModel,"Model to assign modifier to")
#pragma GE_Documentation(ForceImparter, "True if model imparts a force to an actor")
#pragma GE_Documentation(ForceAmount, "Initial amount of force applied")
#pragma GE_Documentation(ForceDecay,"Amount of force decay (larger is faster)")
#pragma GE_Documentation(ForceAudioEffect,"Audio effect to play when force applied")
#pragma GE_Documentation(ForceTimeDelay,"Delay in sec. before force activates again")
#pragma GE_Documentation(DamageTimeDelay,"Delay in sec. before damage occurs again")
#pragma GE_Documentation(DamageGenerator,"True if contact with the model causes damage")
#pragma GE_Documentation(DamageAmount,"Amount of damage caused by contact with model")
#pragma GE_Documentation(DamageAudioEffect,"Audio effect to play when damage applied")
#pragma GE_Documentation(DamageAttribute,"Attribute to damage")
#pragma GE_Documentation(DamageAltAttribute,"Alternate Attribute to damage when Attribute is 0")
} ModelStateModifier;

//	Model Attributes
//
//	Due to some sickness down inside G3D, brush contents are NOT returned when the
//	..brush is NOT a solid model.  To get around this little problem (EVIL HACK)
//	..I've added the ModelAttributes entity.
// Version 053
#pragma GE_Type("icons\\wm_modstat.bmp")
typedef struct _ModelAttributes
{
#pragma GE_Published
	geVec3d origin;							// Origin of entity
	geWorld_Model *theModel;		// Model to assign modifiers to
	geBoolean Water;						// Model is a "water" surface
	geBoolean Lava;							// Model is a "lava" surface
	geBoolean Frozen;						// Model is a "frozen" surface
	geBoolean Sludge;						// Model is a "sludge" surface
	geBoolean Climbable;				// Model is climbable no matter what the surface
	geBoolean Ladder;						// Model is a "ladder"
	geBoolean Impassible;
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(Water,"False")
#pragma GE_DefaultValue(Lava,"False")
#pragma GE_DefaultValue(Frozen,"False")
#pragma GE_DefaultValue(Sludge,"False")
#pragma GE_DefaultValue(Climbable, "True")
#pragma GE_DefaultValue(Ladder, "False")
#pragma GE_DefaultValue(Impassible, "False")
	
#pragma GE_Documentation(Water,"TRUE if surface is watery")
#pragma GE_Documentation(Lava, "TRUE if surface is lava")
#pragma GE_Documentation(Frozen,"TRUE if surface is frozen")
#pragma GE_Documentation(Sludge,"TRUE if surface is sludge")
#pragma GE_Documentation(Climbable,"TRUE if surface can be climbed on")
#pragma GE_Documentation(Ladder, "TRUE if model can be used as a ladder")
#pragma GE_Documentation(Impassible,"TRUE if surface is impassible")
} ModelAttributes;

//
// Attribute entity
//

#pragma GE_Type("icons\\attribute.bmp")
typedef struct _Attribute
{
#pragma GE_Private
	geBoolean active;
	geBoolean alive;
	geFloat Tick;
	geActor *Actor;				// Actor for this entity
	geBoolean CallBack;
	int CallBackCount;
	geVec3d  OriginOffset;      //Translation from model center
	geBoolean bState;
#pragma GE_Published
	geVec3d origin;
	char *szEntityName;	// Name of this entity
	char 	*EntityName;	// Name of entity to attach to
	geWorld_Model *Model;	// Name of model to attach to
	char 	*BoneName;	// Name of actor bone to attach to
	char *TriggerName;
	char *szSoundFile;	// Audio that is the sound
	char *szReSpawnSound;
	char *szActorName;
	geVec3d ActorRotation;	// Rotation needed to properly orient actor
	geFloat Delay;
	geBoolean ReSpawn;
	geBoolean Gravity;
	geFloat Scale;
	char *AttributeName;				// Name of attribute to update on collision, if any
	int AttributeAmount;				// Amount to modify attribute by
	char *AttributeAltName;
	int AttributeAltAmount;
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(szEntityName, "")
#pragma GE_DefaultValue(EntityName, "")
#pragma GE_DefaultValue(BoneName, "")
#pragma GE_DefaultValue(TriggerName, "")
#pragma GE_DefaultValue(szSoundFile, "")
#pragma GE_DefaultValue(szReSpawnSound, "")
#pragma GE_DefaultValue(szActorName, "skull.act")
#pragma GE_DefaultValue(ActorRotation, "-90 180 0.0")
#pragma GE_DefaultValue(Delay, "0.0")
#pragma GE_DefaultValue(ReSpawn, "False")
#pragma GE_DefaultValue(Gravity, "False")
#pragma GE_DefaultValue(Scale, "1.0")
#pragma GE_DefaultValue(AttributeName, "health")
#pragma GE_DefaultValue(AttributeAmount, "1")
#pragma GE_DefaultValue(AttributeAltName, "")
#pragma GE_DefaultValue(AttributeAltAmount, "0")
#pragma GE_Documentation(szEntityName, "Name of entity (used in scripting and triggers)")
#pragma GE_Documentation(EntityName, "Name of entity to attach to")
#pragma GE_Documentation(Model, "Name of model to attach to")
#pragma GE_Documentation(BoneName, "Name of actor bone to attach to")
#pragma GE_Documentation(TriggerName, "Name of trigger entity to use")
#pragma GE_Documentation(szSoundFile, "Audio loop to play on pickup")
#pragma GE_Documentation(szReSpawnSound, "Audio loop to play on respawn")
#pragma GE_Documentation(szActorName, "Name of actor to use")
#pragma GE_Documentation(ActorRotation, "Rotation needed to properly orient actor in game")
#pragma GE_Documentation(Delay, "Number of seconds before respawning")
#pragma GE_Documentation(ReSpawn, "Is attribute respawnable?")
#pragma GE_Documentation(Gravity, "Is attribute actor affected by gravity?")
#pragma GE_Documentation(Scale, "Scale of actor")
#pragma GE_Documentation(AttributeName,"Name of actor attribute to modify, if any")
#pragma GE_Documentation(AttributeAmount,"Amount to modify actor attribute by")
#pragma GE_Documentation(AttributeAltName,"Name of alternate actor attribute to modify, if any")
#pragma GE_Documentation(AttributeAltAmount,"Amount to modify alternate actor attribute by")
} Attribute;

// FlipBook

#pragma GE_Type("icons\\flipbook.bmp")
typedef struct FlipBook
{
#pragma	GE_Private
	int EffectList;
	geBitmap	**Bitmap;
	geBitmap	*WorldBitmap;
	geBoolean active;
	geVec3d  OriginOffset;      //Translation from model center
	geFloat  Time;
	int CurTex;
	int CycleDir;
#pragma GE_Published
	geVec3d	origin;
	char	*szEntityName;
	GE_RGBA	Color;
	geFloat	Scale;
	int		BitmapCount;
	int		Style;
	geFloat	Speed;
	geFloat	ScaleRate;	// how much to subtract from scale each second
	geFloat	RotationRate;	// how much to add to art rotation each second
	geFloat	Alpha;
	geFloat	AlphaRate;	// how much to subtract from alpha each second
	geFloat	Rotation;	// art rotation amount
	char	*BmpNameBase;
	char	*AlphaNameBase;
	char	*TextureName;
	char 	*EntityName;	// Name of entity to attach to
	geWorld_Model *Model;	// Name of model to attach to
	char 	*BoneName;	// Name of actor bone to attach to
	char 	*TriggerName;	// Name of trigger entity
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(szEntityName, "")
#pragma GE_DefaultValue( Color, "255.0 255.0 255.0" )
#pragma GE_DefaultValue( Scale, "1.0" )
#pragma GE_DefaultValue( ScaleRate, "0.0" )
#pragma GE_DefaultValue( Rotation, "0.0" )
#pragma GE_DefaultValue( RotationRate, "0.0" )
#pragma GE_DefaultValue( Alpha, "255.0" )
#pragma GE_DefaultValue( AlphaRate, "0.0" )
#pragma GE_DefaultValue( BitmapCount, "1" )
#pragma GE_DefaultValue( Style, "0" )
#pragma GE_DefaultValue( Speed, "10.0" )
#pragma GE_DefaultValue(BmpNameBase, "")
#pragma GE_DefaultValue(AlphaNameBase, "")
#pragma GE_DefaultValue(TextureName, "")
#pragma GE_DefaultValue(EntityName, "")
#pragma GE_DefaultValue(BoneName, "")
#pragma GE_DefaultValue(TriggerName, "")
#pragma GE_Documentation(szEntityName, "Name of this entity, if any")
#pragma GE_Documentation( Color, "Texture color" )
#pragma GE_Documentation( Scale, "What scale to use for the bitmap" )
#pragma GE_Documentation( ScaleRate, "Amount to decrease Scale per second" )
#pragma GE_Documentation( Rotation, "Inital amount to rotate bitmap in degrees" )
#pragma GE_Documentation( RotationRate, "Amount to rotate bitmap in degrees per second" )
#pragma GE_Documentation( AlphaRate, "Amount to decrease alpha per second" )
#pragma GE_Documentation( Alpha, "Inital bitmap alpha" )
#pragma GE_Documentation( TextureName, "Name of texture to replace if used as Procedural" )
#pragma GE_Documentation( BitmapCount, "How many bitmaps in animation" )
#pragma GE_Documentation( Style, "Animation style 0 to 4" )
#pragma GE_Documentation( Speed, "# of bitmaps per second" )
#pragma GE_Documentation( BmpNameBase, "Base name of bitmap files without extension" )
#pragma GE_Documentation( AlphaNameBase, "Base name of alpha bitmap files without extension" )
#pragma GE_Documentation(EntityName, "Name of entity to attach to")
#pragma GE_Documentation(Model, "Name of model to attach to")
#pragma GE_Documentation(BoneName, "Name of actor bone to attach to")
#pragma GE_Documentation(TriggerName, "Name of trigger entity to use")
} FlipBook;


// NPC Pathpoint

#pragma GE_Type("model.ico")
typedef struct NPCPathPoint 
{
#pragma GE_Published
	geVec3d		origin;
	int         PathType;
	int			ActionType;
	geFloat		Time;
	geFloat		Dist;
	geFloat		VelocityScale;
	NPCPathPoint	*Next;
	geWorld_Model *MoveWithModel;
	int         Direction;
	int			ShootTimes;
	NPCPathPoint	*WatchPoint;
#pragma GE_Private
	geVec3d     Pos;
	geVec3d  OriginOffset;
	
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(PathType, "-1")
#pragma GE_DefaultValue(ActionType, "-1")
#pragma GE_DefaultValue(Direction, "1")
	
#pragma GE_Documentation(Next, "Link to next point")
#pragma GE_Documentation(PathType, "Path Type")
#pragma GE_Documentation(ActionType, "ActionType")
#pragma GE_Documentation(Time, "ActionType Modifier - Only applies to certain actions.  If left 0 a default value will be used.")
#pragma GE_Documentation(Dist, "ActionType Modifier - Only applies to certain actions.  If left 0 a default value will be used.")
#pragma GE_Documentation(VelocityScale, "ActionType Modifier - Only applies to certain actions.  If left 0 a default value will be used.")
#pragma GE_Documentation(MoveWithModel,"Link to model. Entity will move with this model.")
#pragma GE_Documentation(Direction, "Direction from which action is triggered: 1=Forward, -1=Reverse, 0=Both")
} NPCPathPoint;

//
// Explosion entity
//

#pragma GE_Type("icons\\model.ico")
typedef struct _Explosion
{
#pragma GE_Private
	geBoolean active;
	geVec3d  OriginOffset;      //Translation from model center
#pragma GE_Published
	geVec3d origin;
	char 	*EntityName;	// Name of entity to attach to
	geWorld_Model *Model;	// Name of model to attach to
	char 	*BoneName;	// Name of actor bone to attach to
	char *TriggerName;
	char *ExplosionName;
	char *DamageAttribute;
	geFloat DamageAmt;
	geFloat Radius;
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(ExplosionName, "")
#pragma GE_DefaultValue(EntityName, "")
#pragma GE_DefaultValue(BoneName, "")
#pragma GE_DefaultValue(TriggerName, "")
#pragma GE_DefaultValue(DamageAttribute, "")
#pragma GE_DefaultValue(DamageAmt, "0")
#pragma GE_DefaultValue(Radius, "0")
#pragma GE_Documentation(ExplosionName, "Name of explosion to use")
#pragma GE_Documentation(EntityName, "Name of entity to attach to")
#pragma GE_Documentation(Model, "Name of model to attach to")
#pragma GE_Documentation(BoneName, "Name of actor bone to attach to")
#pragma GE_Documentation(TriggerName, "Name of trigger entity to use")
#pragma GE_Documentation(DamageAttribute, "Name of attribute to damage")
#pragma GE_Documentation(DamageAmt, "Amount of damage done")
#pragma GE_Documentation(Radius, "Radius of explosion damage")
} Explosion;

//	Possible brush contents

#pragma GE_BrushContents
typedef enum
{
	Water = 0x00010000,
		Lava = 0x00020000,
		ToxicGas = 0x00040000,
		ZeroG = 0x00080000,
		Frozen = 0x00100000,
		Sludge = 0x00200000,
		SlowMotion = 0x00400000,
		FastMotion = 0x00800000,
		Ladder = 0x01000000,
		Impenetrable = 0x10000000,
		Unclimbable = 0x02000000,
		UserSurface1 = 0x04000000,
		UserSurface2 = 0x08000000
		/*
		type13 = 0x10000000,
		type14 = 0x20000000,
		type15 = 0x40000000,
		type16 = 0x80000000
		*/
} UserContentsEnum;


#endif

#pragma warning( default : 4068 )
