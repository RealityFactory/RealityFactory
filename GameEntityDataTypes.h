/****************************************************************************************/
/// @file GameEntityDataTypes.h
/// @brief Reality Factory Entity Data Types
///
/// This file contains all of the game entity data type declarations,
/// suitable for use by GEdit and the game itself.
/// @author Ralph Deane
//  Copyright (c) 2001 Ralph Deane; All Rights Reserved.
/****************************************************************************************/

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
#pragma GE_Type("..\\icons\\wm_sun.bmp")
typedef struct   Sun
{
#pragma	GE_Published
   	geVec3d		origin;
	char *		DaviName;
	GE_RGBA		Color;
	int			Light;
	int			FallOffType ;
	geFloat		FallOffRadiusInTexels ;
	geFloat		FallOffRadiusTopIntensity ;
	geFloat		FallOffSecretNumber ;
#pragma	GE_Origin(origin)
#pragma	GE_DefaultValue(DaviName, "Sun")
#pragma	GE_DefaultValue(Color, "255 255 255")
#pragma	GE_DefaultValue(Light, "150")
#pragma	GE_DefaultValue( FallOffType, "0" )
#pragma	GE_DefaultValue( FallOffRadiusInTexels, "0" )
#pragma	GE_DefaultValue( FallOffRadiusTopIntensity, "0" )
#pragma	GE_DefaultValue( FallOffSecretNumber, "1" )
#pragma	GE_Documentation(origin, "Location to put the Sun.")
#pragma	GE_Documentation(DaviName, " Name that can contain rgb or visible for special lighting.")
#pragma	GE_Documentation(Color, "Color of the light.")
#pragma	GE_Documentation(Light, "Intensity of the light.")
#pragma	GE_Documentation( FallOffType, "0=No Fall Off, 1=Hard Radius, 2=Gradual falloff, 3=Alt Gradual falloff" )
#pragma	GE_Documentation( FallOffRadiusInTexels, "Radius of the light (NB: Only FallOfType <> 0)" )
#pragma	GE_Documentation( FallOffRadiusTopIntensity, "Inside this radius the light is 100 percent (no falloff)" )
#pragma	GE_Documentation( FallOffSecretNumber, "Default 1; influences falloff calculation." )
} Sun;


//Start PIckles Aug 04
#pragma GE_Type("..\\icons\\Foglight.ico")
typedef struct _FlipTree
{
#pragma GE_Private
	geBitmap	*Bitmap;
	geBitmap	*Shadow;
	geActor		*fActor;
#pragma GE_Published
	geVec3d		origin;
	geVec3d		Rotation;
	geVec3d		BoxSize;
	GE_RGBA		BitmapColor;
	GE_RGBA		FillColor;
	GE_RGBA		AmbientColor;
	geBoolean	AmbientFromFloor;
	char		*szEntityName;
	char		*BitmapName;
	char		*AlphamapName;
	char		*ShadowMapName;
	char		*ShadowAlphaName;
	char		*ActorName;
	int			ClipRange;
	int			Range;
	geFloat		Scale;
	geFloat		ShadowSize;
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(Rotation,"0 0 0")
#pragma GE_DefaultValue(szEntityName,"")
#pragma GE_DefaultValue(BitmapName,"")
#pragma GE_DefaultValue(AlphamapName,"")
#pragma GE_DefaultValue(BitmapColor, "128 128 128")
#pragma GE_DefaultValue(FillColor, "128 128 128")
#pragma GE_DefaultValue(AmbientColor, "255 255 255")
#pragma GE_DefaultValue(AmbientFromFloor, "False")
#pragma GE_DefaultValue(ShadowMapName,"")
#pragma GE_DefaultValue(ShadowAlphaName,"")
#pragma GE_DefaultValue(ActorName,"")
#pragma GE_DefaultValue(ClipRange,"1024")
#pragma GE_DefaultValue(BoxSize,"0 0 0")
#pragma GE_DefaultValue(Range,"512")
#pragma GE_DefaultValue(Scale,"1.0")
#pragma GE_DefaultValue(ShadowSize,"0.0")
#pragma GE_Documentation(BitmapName,"Bitmap image.")
#pragma GE_Documentation(Rotation,"Actor Rotation")
#pragma GE_Documentation(BoxSize,"Bounding Box Dimensions")
#pragma GE_Documentation(ClipRange,"Clipping Distance")
#pragma GE_Documentation(ActorName,"Actor File Name")
#pragma GE_Documentation(ShadowSize,"Actor and Bitmap Scale")
#pragma GE_Documentation(Scale,"Actor and Bitmap Scale")
#pragma GE_Documentation(BitmapColor,"Image Fill Color")
#pragma GE_Documentation(FillColor,"Actor Fill Color")
#pragma GE_Documentation(AmbientFromFloor,"View Ambient Light From Floor")
#pragma GE_Documentation(AmbientColor,"Ambient Light Color")
#pragma GE_Documentation(AlphamapName,"Alpha for Image")
#pragma GE_Documentation(ShadowMapName,"Shadow Image Name")
#pragma GE_Documentation(ShadowAlphaName,"Alpha for Shadow Image")
#pragma GE_Documentation(Range,"Range at Which Actor is Rendered")
#pragma GE_Documentation(szEntityName,"Name of entity.")
} FlipTree;
//end added pickles

// Start Pickles oct 04
#pragma GE_Type("..\\icons\\Foglight.ico")
typedef struct _AreaSwitch
{
#pragma GE_Published
	geVec3d		origin;
	geFloat		Diameter;
	char		*Trigger;
	geBoolean	InState;
	char		*Entity;
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(Diameter,"128")
#pragma GE_DefaultValue(Trigger,"")
#pragma GE_DefaultValue(InState,"true")
#pragma GE_DefaultValue(Entity,"")
#pragma GE_Documentation(Diameter,"Area Diameter")
#pragma GE_Documentation(Trigger,"Name of Trigger To Control")
#pragma GE_Documentation(InState,"State of Trigger While Inside Area")
#pragma GE_Documentation(Entity,"Name of Entity to Detect. Blank is Player")
}AreaSwitch;
//end Pickles oct 04

//
// Another sun entity with a different type of light
// courtesy of Wismerhill

#pragma GE_Type("..\\icons\\wm_sunlight.bmp")
typedef struct tagSunLight
{
#pragma GE_Published
	geVec3d	origin;
	GE_RGBA	color;
	int		light;
	geVec3d	angles;
	int		style;
#pragma	GE_Origin(origin)
#pragma	GE_DefaultValue(light, "150")
#pragma	GE_DefaultValue(color, "255 255 255")
#pragma	GE_Angles(angles)
#pragma	GE_DefaultValue(angles, "0 0 0")
#pragma	GE_Documentation(color, "Color of the light.")
#pragma	GE_Documentation(light, "Intensity of the light.")
#pragma	GE_Documentation(angles, "Direction of the light.")
} SunLight;

// FogLight
#pragma GE_Type("..\\icons\\FogLight.ico")
typedef struct FogLight
{
#pragma GE_Published
	geVec3d	origin;
	GE_RGBA	Color;
	geFloat	Brightness;
	geFloat	Radius;
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(Color,"100 100 100")
#pragma GE_DefaultValue(Radius,"100")
#pragma GE_DefaultValue(Brightness,"1500")
#pragma GE_Documentation(Color,"RGB color of the fog")
#pragma GE_Documentation(Radius,"Radius of the fog")
#pragma GE_Documentation(Brightness,"Intensity of the fog")
} FogLight;

// Basic automatic door

#pragma GE_Type("..\\icons\\wm_door.bmp")
typedef struct _Door
{
#pragma	GE_Private						// Not seen by GEdit
	geBoolean		bInAnimation;		// Is door animating?
	geBoolean		bTrigger;			// Has the door been triggered?
	geFloat			AnimationTime;		// Ticks in animation
	geBoolean		bInCollision;		// Door is colliding flag
	geBoolean		bActive;			// Door can be used
	geFloat			LastIncrement;		// Last tick increment
	geFloat			tDoor;				// Door animation position time
	geSound_Def		*theSound;			// Sound def pointer
	int				SoundHandle;
	geBoolean		CallBack;
	int				CallBackCount;
// Start Aug2003DCS
	geVec3d			OriginOffset;		// Translation from model center
// End Aug2003DCS
#pragma GE_Published
	geWorld_Model	*Model;				// Door model to animate
	geWorld_Model	*NextToTrigger;		// Next door to trigger, if any
	geVec3d			origin;
// Start Aug2003DCS
   geWorld_Model	*ParentModel;		// Name of model this model is attached to
// End Aug2003DCS// Location of same
	char			*szSoundFile;		// Name of sound to play on open
	geBoolean		bOneShot;			// TRUE if door opens ONLY ONCE
	geBoolean		bNoCollide;			// TRUE if door won't open on collision
	char			*szEntityName;		// Name of this entity
	geBoolean		bAudioLoops;		// Audio effect loops while animation plays
	geFloat			AnimationSpeed;		// Speed of the animation, in %
	char			*TriggerName;
	geBoolean		bShoot;
	geBoolean		bReverse;
	geBoolean		bRotating;
// changed RF063
	geBoolean		UseKey;
	geBoolean		PlayerOnly;
// end change RF063
	geBoolean		bRunWhileTrig;		// Animation only runs while trigger is on
    geBoolean		bRunTimed;			// Animation runs specified time increment each
										//           time trigger goes from off to on
    geFloat			TimeEachTrig;		// Time animation will run each trigger, only valid
										//           if bRunTimed is true
    geBoolean		bRunToNextEvent;	// Animation runs until the next event time is reached
										//           each time triggered
    geBoolean		bRunFromList;		// Animation runs to next time in provided list each
										//           time triggered
    char			*TimeList;			// List of times to run animation to each time triggered
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
// changed RF063
#pragma GE_DefaultValue(UseKey, "False")
#pragma GE_DefaultValue(PlayerOnly, "False")
// end change RF063
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
// changed RF063
#pragma GE_Documentation(UseKey, "if true then can be activated by Use Key")
#pragma GE_Documentation(PlayerOnly, "if true then can be activated only by player")
// end change RF063
// Start Aug2003DCS
#pragma GE_Documentation(ParentModel, "Name of model this door model is attached to")
// End Aug2003DCS
} Door;

//	The infamous and never-to-be-forgotten Moving Platform
//

#pragma GE_Type("..\\icons\\wm_platform.bmp")
typedef struct _MovingPlatform
{
#pragma GE_Private
	geBoolean		bInAnimation;		// Is the platform animating?
	geBoolean		bTrigger;			// Has the platform been triggered?
	geBoolean		bActorOnMe;			// Is there an ACTOR sitting on me?
	geBoolean		bInCollision;		// Platform is colliding flag
	geBoolean		bActive;			// Platform can be used
	geFloat			LastIncrement;		// Last tick increment
	geBoolean		bForward;			// Animation running forward?
	geFloat			tPlatform;			// Platform animation position time
	geSound_Def		*theSound;			// Sound def pointer
	int				SoundHandle;		// For later modification of sound
	geBoolean		CallBack;
	int				CallBackCount;
// Start Aug2003DCS
	geVec3d			OriginOffset;		// Translation from model center
// End Aug2003DCS
#pragma GE_Published
	geWorld_Model	*Model;				// Platform model
	geWorld_Model	*NextToTrigger;		// Next door to trigger, if any
	geVec3d			origin;				// Location of model
// Start Aug2003DCS
	geWorld_Model	*ParentModel;		// Name of model this model is attached to
// End Aug2003DCS
// Added Pickles RF071A
	geBoolean		Rideable;
// End Added PIckles RF071A
   	geFloat			AnimationSpeed;		// Speed of the animation, in %
	char			*szSoundFile;		// Name of sound to play on trigger
	geBoolean		bOneShot;			// TRUE if platform runs ONLY ONCE
	geBoolean		bNoCollide;			// TRUE if platform won't start on collision
	char			*szEntityName;		// Name of this moving platform
	geBoolean		bAudioLoops;		// Audio effect loops while animation plays
	geBoolean		bAutoStart;			// Animation starts when level loads
	geBoolean		bLooping;			// Once started, animation runs forever
	char			*TriggerName;
	geBoolean		bShoot;
	geBoolean		bReverse;
	geBoolean		bAllowInside;
// changed RF063
	geBoolean		UseKey;
	geBoolean		PlayerOnly;
// end change RF063

    geBoolean		bRunWhileTrig;		// Animation only runs while trigger is on
    geBoolean		bRunTimed;			// Animation runs specified time increment each
										//           time trigger goes from off to on
    geFloat			TimeEachTrig;		// Time animation will run each trigger, only valid
										//           if bRunTimed is true
    geBoolean		bRunToNextEvent;	// Animation runs until the next event time is reached
										//           each time triggered
    geBoolean		bRunFromList;		// Animation runs to next time in provided list each
										//           time triggered
    char			*TimeList;			// List of times to run animation to each time triggered
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
#pragma GE_DefaultValue(bAllowInside, "False")
// changed RF063
#pragma GE_DefaultValue(UseKey, "False")
#pragma GE_DefaultValue(PlayerOnly, "False")
// end change RF063
#pragma GE_DefaultValue(bRunTimed, "False")
#pragma GE_DefaultValue(TimeEachTrig, "0")
#pragma GE_DefaultValue(bRunToNextEvent, "False")
#pragma GE_DefaultValue(bRunFromList, "False")
#pragma GE_DefaultValue(TimeList, "")
//Added Pickles RF071A
#pragma GE_DefaultValue(Rideable, "True")
//End Added PIckles RF071A
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
#pragma GE_Documentation(bAllowInside, "TRUE if you can enter inside of model")
// changed RF063
#pragma GE_Documentation(UseKey, "if true then can be activated by Use Key")
#pragma GE_Documentation(PlayerOnly, "if true then can be activated only by player")
// end change RF063
// Start Aug2003DCS
#pragma GE_Documentation(ParentModel, "Name of model this model is attached to")
// End Aug2003DCS
} MovingPlatform;


// Start Aug2003DCS
//	The Level Controller
//
#pragma GE_Type("..\\icons\\model.ico")
typedef struct _LevelController
{
#pragma GE_Private
	void		*Data;
#pragma GE_Published
	geVec3d		origin;
	char		*szEntityName;				// Name of entity
	char		*ScriptName;
	char		*StartOrder;
	char		*InitOrder;
	geBoolean	Enable;
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(szEntityName, "")
#pragma GE_DefaultValue(ScriptName, "")
#pragma GE_DefaultValue(StartOrder, "")
#pragma GE_DefaultValue(InitOrder, "")
#pragma GE_DefaultValue(Enable, "True")

#pragma GE_Documentation(szEntityName, "Name of entity (used in scripting and triggers)")
#pragma GE_Documentation(ScriptName, "Name of Action Script file to use")
#pragma GE_Documentation(StartOrder, "Name of first runtime order")
#pragma GE_Documentation(InitOrder, "Name of initial order to run")
#pragma GE_Documentation(Enable, "if True then enable LevelController")
} LevelController;
// End Aug2003DCS


//	Teleport entities (teleporter and teleport target)

#pragma GE_Type("..\\icons\\sf_teleport.bmp")
typedef struct _Teleporter
{
#pragma GE_Private
	geBoolean		bActive;			// This teleporter ready to run?
	geSound_Def		*theSound;			// Handle of audio effect for teleport
	int				SoundHandle;		// For later modification of sound
	geFog			*theFog;			// Teleporters sphere of fog
	geBoolean		bForward;			// Fog animation direction
	geFloat			fDelta;				// Fog depth variance delta
	geFloat			fDensity;			// Current fog density
	geBoolean		active;
// changed QD 12/15/05
	geBoolean		CallBack;
	int				CallBackCount;
	geFloat			fOldTime;
	geBoolean		bDoFade;
	geBoolean		bFadeOut;
	geActor			*theActor;
// end change
#pragma GE_Published
	geWorld_Model	*Model;				// Teleporter model
	geVec3d			origin;				// Location of entity
	char			*Target;			// Name of teleport target
	geBoolean		bOneShot;			// Teleporter only runs ONCE
	char			*szSoundFile;		// Audio to play when teleport activates
	GE_RGBA			cColor;				// Fog color
	int				fogSpeed;			// Speed at which fog density changes, in msec.
	geFloat			fogVariance;		// Depth of density variance
	geFloat			fogRadius;			// Radius of fog around entity
	char			*szEntityName;		// Name of entity
	geBoolean		bUseFogEffect;		// TRUE if fog effect is active
	geBoolean		bUseTeleportEffect;	// TRUE if teleport effect is run on collision
	char			*TriggerName;		// Name of trigger entity
// changed RF063
	geBoolean		PlayerOnly;
// end change
// changed Nout 12/15/05
	geFloat			fFadeTime;			// Time the blending fades in out out
	GE_RGBA			cFadeColor;			// blend color
// end change
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
// changed RF063
#pragma GE_DefaultValue(PlayerOnly, "False")
// end change
//Added ny Nout May 2005
#pragma GE_DefaultValue(fFadeTime, "0.0")
#pragma GE_DefaultValue(cFadeColor, "255 255 255")
//End added by Nout
#pragma GE_Documentation(Model,"Teleporter model to use")
#pragma GE_Documentation(Target, "Name of teleport target to go to")
#pragma GE_Documentation(bOneShot, "True if teleport only works once")
#pragma GE_Documentation(cColor, "Color for teleport field")
#pragma GE_Documentation(fogSpeed, "Speed of density variance, in milliseconds")
#pragma GE_Documentation(fogVariance, "Depth of fog variance")
#pragma GE_Documentation(fogRadius, "Radius of fog around entity")
#pragma GE_Documentation(szSoundFile, "Audio effect to play when teleport activated")
#pragma GE_Documentation(szEntityName, "Name of entity (used in scripting and triggers)")
#pragma GE_Documentation(bUseFogEffect, "TRUE if fog is active for teleport entity")
#pragma GE_Documentation(bUseTeleportEffect, "TRUE if, on collision, fog densifies prior to player teleport")
#pragma GE_Documentation(TriggerName, "Name of trigger entity to use")
// changed RF063
#pragma GE_Documentation(PlayerOnly, "True if only player can activate")
// end change
// changed Nout 12/15/05
#pragma GE_Documentation(fFadeTime, "Fade in/out time in milliseconds")
#pragma GE_Documentation(cFadeColor, "Color used for fading")
// end change
} Teleporter;

#pragma GE_Type("..\\icons\\sf_teletargt.bmp")
typedef struct _TeleportTarget
{
#pragma GE_Private
	geBoolean	bFollower;			// Bound to path follower
#pragma GE_Published
	char		*Name;				// Name of this teleport target
	geVec3d		origin;				// Location of this teleport target
// changed Nout/QD 12/15/05
	geVec3d     Angle;
	geBoolean	UseAngle;			// Rotate player to match the entity's angle
// end change
// changed QD
	geBoolean	UseOffset;
// end change QD
#pragma GE_Origin(origin)
// changed Nout/QD 12/15/05
#pragma GE_Angles(Angle)
#pragma GE_DefaultValue(Name, "")
#pragma GE_DefaultValue(UseAngle, "False")
// end change
// changed QD
#pragma GE_DefaultValue(UseOffset, "False")
// end change QD
// changed Nout/QD 12/15/05
#pragma GE_Documentation(Name, "Name of this teleport target")
#pragma GE_Documentation(UseAngle, "if True then use TeleportTarget Angle at Target Position")
#pragma GE_Documentation(UseOffset, "if True then Offset Player at TeleportTarget")
// end change
} TeleportTarget;


//	Ye olde player start position

#pragma GE_Type("..\\icons\\pstart.bmp")
typedef struct _PlayerStart
{
#pragma GE_Published
	geVec3d     origin;
	geVec3d     Angle;
	char		*szMIDIFile;		// MIDI file to play, if any
	char		*szCDTrack;			// CD TRACK to play, if any
	char		*szStreamingAudio;	// Streaming audio to play, if any
	geBoolean	bSoundtrackLoops;	// Soundtrack for level loops
	char		*szEntityName;
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

#pragma GE_Type("..\\icons\\sf_morphf.bmp")
typedef struct _MorphingField
{
#pragma GE_Private
	geFog			*theFog;		// MorphingField sphere of fog
	geBoolean		bForward;		// Fog animation direction
	geFloat			fDepthDelta;	// Fog depth variance delta
	geFloat			fDensity;		// Current fog density
	GE_RGBA			cDelta;			// Color morphing delta
	geFloat			fRadiusDelta;	// Fog radius delta
	geFloat			fRadius;		// Current fog radius
	geSound_Def		*theSound;		// Handle of audio effect for MorphingField
	int				SoundHandle;	// For later modification of sound
	geFloat			tElapsed;		// Time elapsed in single animation cycle
	GE_RGBA			clrCurrent;		// Current fog color
	geBoolean		bFollower;		// TRUE if entity is bound to a motion path
	geBoolean		active;			// is activated
	geVec3d			OriginOffset;	// Translation from model center
#pragma GE_Published
	geVec3d			origin;			// Location of entity in world
	char			*szSoundFile;	// Looping audio to play for MorphingField
	GE_RGBA			clrStart;		// Fog color, start of morph
	GE_RGBA			clrEnd;			// Fog color, end of morph
	int				fogSpeed;		// Speed at which fog density changes, in msec.
	geFloat			fogVariance;	// Depth of density variance
	geFloat			fogRadiusStart;	// Radius of fog around entity, start of morph
	geFloat			fogRadiusEnd;	// Radius of fog around entity, end of morph
	char			*szEntityName;	// Name of this entity
	char			*EntityName;	// Nmae of entity to attach to
	geWorld_Model	*Model;			// Name of model to attach to
	char			*BoneName;		// Name of actor bone to attach to
	char			*TriggerName;	// Name of trigger entity
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

#pragma GE_Type("..\\icons\\a_3d.bmp")
typedef struct _AudioSource3D
{
#pragma GE_Private
	int				*effect;		// pointer to array of effect indexs
	geBoolean		active;			// is activated
	geVec3d			OriginOffset;	// Translation from model center
// Start Aug2003DCS
	geBoolean		ProgrammedTrigger;
// End Aug2003DCS
#pragma GE_Published
	geVec3d			origin;			// Location of entity in world
	char			*szSoundFile;	// Audio that is the sound
	geFloat			fRadius;		// Maximum audible radius for sound
	char			*szEntityName;	// Name of this entity
	char			*EntityName;	// Nmae of entity to attach to
	geWorld_Model	*Model;			// Name of model to attach to
	char			*BoneName;		// Name of actor bone to attach to
	char			*TriggerName;	// Name of trigger entity
// Start Aug2003DCS
	geBoolean		bLoopSound;
// End Aug2003DCS
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(szSoundFile, "")
#pragma GE_DefaultValue(fRadius, "200.0")
#pragma GE_DefaultValue(szEntityName, "")
#pragma GE_DefaultValue(EntityName, "")
#pragma GE_DefaultValue(BoneName, "")
#pragma GE_DefaultValue(TriggerName, "")
//Start Aug2003DCS
#pragma GE_DefaultValue(bLoopSound, "True")
//End Aug2003DCS
#pragma GE_Documentation(szSoundFile, "Audio loop to play")
#pragma GE_Documentation(fRadius, "Maximum audible radius for sound")
#pragma GE_Documentation(szEntityName, "Name of entity (used in scripting and triggers)")
#pragma GE_Documentation(EntityName, "Name of entity to attach to")
#pragma GE_Documentation(Model, "Name of model to attach to")
#pragma GE_Documentation(BoneName, "Name of actor bone to attach to")
#pragma GE_Documentation(TriggerName, "Name of trigger entity to use")
//Start Aug2003DCS
#pragma GE_Documentation(bLoopSound, "True-looping  False-one shot")
//End Aug2003DCS
} AudioSource3D;

//	Static Entity proxy
//

#pragma GE_Type("..\\icons\\p_static.bmp")
typedef struct _StaticEntityProxy
{
#pragma GE_Private
	int			index;
	geActor		*Actor;				// Actor for this entity
	geBoolean	bFollower;			// TRUE if entity is bound to a motion path
	geBoolean	bInitialized;		// Path following initialized flag
	geBoolean	IsHit;
	geBoolean	alive;
	geBoolean	CallBack;
	geFloat		Time;
	geBoolean	DoingDamage;
	int			CallBackCount;
	geBoolean	bState;
// changed QD 05/06/2004
	geFloat		AlphaRate;
	geBoolean	dying;
// end change
#pragma GE_Published
	geVec3d		origin;
	char		*szSoundFile;		// Name of audio effect to play on collision
	geBoolean	SoundLoop;
	geFloat		fRadius;			// Audio audible radius
	char		*szActorFile;
	char		*szEntityName;
	char		*szDefaultAction;	// Default animation to play
	char		*szImpactAction;	// Animation to play when collided with
// changed QD 05/06/2004
	char		*szDeathAction;
	geBoolean	FadeOut;
	geFloat		FadeOutTime;
	geFloat		InitialAlpha;		// Transparency of actor
// end change
	geBoolean	bCanPush;			// Static entity can be pushed around
	geBoolean	SubjectToGravity;	// TRUE if entity subject to gravity
	geFloat		ScaleFactor;		// Factor to scale model by
	geBoolean	FaceDestination;	// TRUE if entity rotated to face destination
	geVec3d		ActorRotation;		// Rotation needed to properly orient actor
	geBoolean	IsAVehicle;			// TRUE if actor can be "ridden"
	geFloat		AttributeAmt;		// Props health points, or -1 for indestructible
	geBoolean	DoDamage;
	geBoolean	GetDamaged;
	geFloat		Damage;
// changed RF063
	geFloat		DamageAlt;
	geFloat		DamageDelay;
	char		*DamageTo;
// changed RF063
	char		*DamageToAlt;
	char		*DamageAttribute;
	geFloat		StepHeight;
	geBoolean	DeathDissappear;
	GE_RGBA		FillColor;
	GE_RGBA		AmbientColor;
// changed QD 07/21/04
	geBoolean	AmbientLightFromFloor;
	geBoolean	NoCollision;
// end change
	geFloat		ShadowSize;
// changed QD 06/26/04
	geFloat		ShadowAlpha;
	char		*ShadowBitmap;
	char		*ShadowAlphamap;
// end change
// begin change gekido - Projected Shadows
	geBoolean	UseProjectedShadows;
// end change gekido
// changed QD Shadows
	geBoolean	UseStencilShadows;
// end change
// changed RF063
	geBoolean	UseKey;
// end change RF063
// change RF064
	geVec3d		BoxSize;
	geBoolean	HideFromRadar;
	char		*ChangeMaterial;
	geBoolean	EnvironmentMapping;
	geBoolean	AllMaterial;
	geFloat		PercentMapping;
	geFloat		PercentMaterial;
// end change RF064
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(szSoundFile, "")
#pragma GE_DefaultValue(SoundLoop, "False")
#pragma GE_DefaultValue(fRadius, "200.0")
#pragma GE_DefaultValue(szEntityName, "")
#pragma GE_DefaultValue(szActorFile, "")
#pragma GE_DefaultValue(szDefaultAction, "")
#pragma GE_DefaultValue(szImpactAction, "")
// changed QD 05/06/2004
#pragma GE_DefaultValue(szDeathAction, "")
#pragma GE_DefaultValue(FadeOut, "False")
#pragma GE_DefaultValue(FadeOutTime, "5")
// end change
#pragma GE_DefaultValue(bCanPush, "False")
#pragma GE_DefaultValue(InitialAlpha, "255.0")
#pragma GE_DefaultValue(SubjectToGravity, "True")
#pragma GE_DefaultValue(ScaleFactor, "1.0")
#pragma GE_DefaultValue(FaceDestination, "False")
#pragma GE_DefaultValue(ActorRotation, "0 180 0.0")
#pragma GE_DefaultValue(IsAVehicle, "False")
#pragma GE_DefaultValue(DoDamage, "False")
#pragma GE_DefaultValue(GetDamaged, "False")
#pragma GE_DefaultValue(Damage, "0")
// changed RF063
#pragma GE_DefaultValue(DamageAlt, "0")
#pragma GE_DefaultValue(DamageDelay, "5")
#pragma GE_DefaultValue(DamageTo, "")
// changed RF063
#pragma GE_DefaultValue(DamageToAlt, "")
#pragma GE_DefaultValue(AttributeAmt, "-1")
#pragma GE_DefaultValue(DamageAttribute, "")
#pragma GE_DefaultValue(StepHeight, "0")
#pragma GE_DefaultValue(DeathDissappear, "True")
#pragma GE_DefaultValue(FillColor, "0 0 0")
#pragma GE_DefaultValue(AmbientColor, "0 0 0")
// changed QD 07/21/04
#pragma GE_DefaultValue(AmbientLightFromFloor, "True")
#pragma GE_DefaultValue(NoCollision, "False")
// end change
#pragma GE_DefaultValue(ShadowSize, "0")
// changed QD 06/26/04
#pragma GE_DefaultValue(ShadowAlpha, "128.0")
#pragma GE_DefaultValue(ShadowBitmap, "")
#pragma GE_DefaultValue(ShadowAlphamap, "")
// end change
// begin change gekido - Projected Shadows
#pragma GE_DefaultValue(UseProjectedShadows, "False")
// end change gekido
// changed QD Shadows
#pragma GE_DefaultValue(UseStencilShadows, "False")
// end change
#pragma GE_DefaultValue(UseKey, "False")
// change RF064
#pragma GE_DefaultValue(BoxSize, "0 0 0")
#pragma GE_DefaultValue(HideFromRadar, "False")
#pragma GE_DefaultValue(ChangeMaterial, "")
#pragma GE_DefaultValue(EnvironmentMapping, "False")
#pragma GE_DefaultValue(AllMaterial, "False")
#pragma GE_DefaultValue(PercentMapping, "100")
#pragma GE_DefaultValue(PercentMaterial, "100")
// end change RF064
#pragma GE_Documentation(szSoundFile, "Sound to play on collision/activation")
#pragma GE_Documentation(SoundLoop, "TRUE if sound loops")
#pragma GE_Documentation(fRadius, "Audible range for sound")
#pragma GE_Documentation(szActorFile, "Actor file to load for this proxy")
#pragma GE_Documentation(szEntityName, "Name of entity (used in scripting and triggers)")
#pragma GE_Documentation(szDefaultAction, "Default animation, if any, for this prop")
#pragma GE_Documentation(szImpactAction, "Animation, if any, to play when collided with")
// changed QD 05/06/2004
#pragma GE_Documentation(szDeathAction, "Animation, if any, to play when entity is dying")
#pragma GE_Documentation(FadeOut, "If DeathDissappear is True, fade out actor when dying")
#pragma GE_Documentation(FadeOutTime, "Number of seconds until actor is totally transparent")
// end change
#pragma GE_Documentation(bCanPush, "TRUE if the player can push actor around")
#pragma GE_Documentation(InitialAlpha,"Transparency of actor, from 0-255 with 0=transparent")
#pragma GE_Documentation(SubjectToGravity, "TRUE if actor will be subject to gravity")
#pragma GE_Documentation(ScaleFactor, "Number to scale actor by")
#pragma GE_Documentation(FaceDestination, "TRUE if actor rotated to face destination")
#pragma GE_Documentation(ActorRotation, "Rotation needed to properly orient actor in game")
#pragma GE_Documentation(IsAVehicle, "TRUE if actor can be ridden")
#pragma GE_Documentation(DoDamage, "TRUE if actor does damage on collision")
#pragma GE_Documentation(GetDamaged, "TRUE if actor gets damaged on collision")
#pragma GE_Documentation(Damage, "Amount of damage done")
// changed RF063
#pragma GE_Documentation(DamageAlt, "Amount of alternate damage done")
#pragma GE_Documentation(DamageDelay, "Number of seconds until damage occurs again")
#pragma GE_Documentation(DamageTo, "Name of attribute actor causes damage to")
// changed RF063
#pragma GE_Documentation(DamageToAlt, "Name of alternate attribute actor causes damage to")
#pragma GE_Documentation(AttributeAmt,"Prop health points, or -1 if indestructible")
#pragma GE_Documentation(DamageAttribute, "Name of attribute that takes damage")
#pragma GE_Documentation(StepHeight, "Height that actor can step up when moving")
#pragma GE_Documentation(DeathDissappear, "TRUE if actor is removed when dead")
#pragma GE_Documentation(FillColor, "Color of actor fill lighting")
#pragma GE_Documentation(AmbientColor, "Color of actor ambient lighting")
// changed QD 07/21/04
#pragma GE_Documentation(AmbientLightFromFloor, "Calculate ambient lighting instead of using AmbientColor")
#pragma GE_Documentation(NoCollision, "no collision with other actors")
// end change
#pragma GE_Documentation(ShadowSize, "Radius of actor shadow, 0 is none")
// changed QD 06/26/04
#pragma GE_Documentation(ShadowAlpha, "Transparency of actor shadow (0 to 255)")
#pragma GE_Documentation(ShadowBitmap, "name of bitmap to use for shadow")
#pragma GE_Documentation(ShadowAlphamap, "name of alpha bitmap to use for shadow")
// end change
// begin change gekido - Projected Shadows
#pragma GE_Documentation(UseProjectedShadows, "whether to use the projected shadow system for this actor")
// end change gekido
// changed QD Shadows
#pragma GE_Documentation(UseStencilShadows, "whether to use stencil shadows for this actor")
// end change
#pragma GE_Documentation(UseKey, "if true the can be activated by Use key")
// change RF064
#pragma GE_Documentation(BoxSize, "if not all 0 then use as bounding box size")
#pragma GE_Documentation(HideFromRadar, "if true then does not show on radar")
#pragma GE_Documentation(ChangeMaterial, "name of section containing material change info")
#pragma GE_Documentation(EnvironmentMapping, "if true then use environmental mapping")
#pragma GE_Documentation(AllMaterial, "if true apply to all material")
#pragma GE_Documentation(PercentMapping, "percentage of mapping to apply")
#pragma GE_Documentation(PercentMaterial, "percentage of material to use")
// end change RF064
} StaticEntityProxy;


// Start Pickles Jul 04
// Foliage
#pragma GE_Type("..\\icons\\Foglight.ico")
typedef struct _Foliage
{
#pragma GE_Private
	geBitmap	*Bitmap;
#pragma GE_Published
	geVec3d		origin;
	int			Density;
	int			Diameter;
	char		*szEntityName;
	char		*BitmapName;
	char		*AlphamapName;
	int			Seed;
	geFloat		sMax;
	geFloat		sMin;
	geBoolean	UseAlpha;
	int			Range;
	GE_RGBA		Color;
	char		*Texture;
// changed Nout 12/15/05
	geBoolean	Hanging;
// end change
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(Density, "100")
#pragma GE_DefaultValue(Diameter, "256")
#pragma GE_DefaultValue(BitmapName, "")
#pragma GE_DefaultValue(AlphamapName, "")
#pragma GE_DefaultValue(Seed, "12345")
#pragma GE_DefaultValue(Range, "512")
#pragma GE_DefaultValue(sMax, "1.0")
#pragma GE_DefaultValue(sMin, "0.25")
#pragma GE_DefaultValue(Color, "255 255 255")
#pragma GE_DefaultValue(Texture, "")
#pragma GE_DefaultValue(UseAlpha, "true")
#pragma GE_DefaultValue(szEntityName, "")
// changed Nout 12/15/05
#pragma GE_DefaultValue(Hanging, "False")
// end change
#pragma GE_Documentation(Density, "Number of plants rendered within diameter")
#pragma GE_Documentation(Diameter, "Area Diameter")
#pragma GE_Documentation(BitmapName, "Bitmap image.")
#pragma GE_Documentation(sMax, "Maximum scale")
#pragma GE_Documentation(sMin, "Minimum scale")
#pragma GE_Documentation(AlphamapName, "Alpha for image")
#pragma GE_Documentation(Seed, "Random Seed Value")
#pragma GE_Documentation(Range, "Minimum rendering distance.")
#pragma GE_Documentation(Texture, "Name of texture to only apply the particle to")
#pragma GE_Documentation(UseAlpha, "Use Fading Effect")
#pragma GE_Documentation(Color, "Ambient Color")
#pragma GE_Documentation(szEntityName, "Name of entity")
} Foliage;
// End Pickles Jul 04


// changed QD 01/2004
#pragma GE_Type("icons\\p_static.bmp")
typedef struct _StaticMesh
{
#pragma GE_Private
	int				ListIndex;
	int				RenderFlags;
	geVec3d			Rotation;
	geVec3d			OriginOffset;
	GE_RGBA			*ColorLOD0;
	GE_RGBA			*ColorLOD1;
	GE_RGBA			*ColorLOD2;
	GE_RGBA			*ColorLOD3;
#pragma GE_Published
	geVec3d			origin;
	char			*szActorFile;
	char			*szEntityName;
	geBoolean		UseFillColor;
	GE_RGBA			FillColor;
	geBoolean		Backfaced;
	geBoolean		ZSort;
	int				VisCheckLevel;
	int				ColCheckLevel;
	geFloat			Scale;
	geVec3d			ActorRotation;
	geFloat			Alpha;
	char			*EntityName;	// Name of entity to attach to
	char			*BoneName;
	geWorld_Model	*Model;			// Name of model to attach to
	geBoolean		CompSunLight;
// changed QD 09/28/2004
	geBoolean		CompLight;
	geBoolean		CompLightSmooth;
	GE_RGBA			AmbientColor;
// end change
	geBoolean		StaticShadowCaster;
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(EntityName, "")
#pragma GE_DefaultValue(szEntityName, "")
#pragma GE_DefaultValue(szActorFile, "")
#pragma GE_DefaultValue(FillColor, "128 128 128")
#pragma GE_DefaultValue(Backfaced, "False")
#pragma GE_DefaultValue(ZSort, "False")
#pragma GE_DefaultValue(UseFillColor, "False")
#pragma GE_DefaultValue(VisCheckLevel, "1")
#pragma GE_DefaultValue(Scale, "1.0")
#pragma GE_DefaultValue(Alpha, "255.0")
#pragma GE_DefaultValue(ActorRotation, "0 0 0")
#pragma GE_DefaultValue(CompSunLight, "False")
// changed QD 09/28/2004
#pragma GE_DefaultValue(CompLight, "False")
#pragma GE_DefaultValue(CompLightSmooth, "True")
#pragma GE_DefaultValue(AmbientColor, "0 0 0")
// end change
#pragma GE_DefaultValue(StaticShadowCaster, "False")
#pragma GE_Documentation(szEntityName, "Name of entity (used in scripting and triggers)")
#pragma GE_Documentation(EntityName, "Name of entity to attach to")
#pragma GE_Documentation(Model, "Name of model to attach to")
#pragma GE_Documentation(CompSunLight, "Compute Lighting from a SunLight entity")
// changed QD 09/28/2004
#pragma GE_Documentation(CompLight, "compute lighting from light and spotlight entities")
#pragma GE_Documentation(CompLightSmooth, "smooth the lighting")
#pragma GE_Documentation(AmbientColor, "Color of ambient lighting")
// end change
#pragma GE_Documentation(VisCheckLevel, "0 no visibility test, 1 AABB, 2 per face")
#pragma GE_Documentation(ColCheckLevel, "0 no test,1 AABB-AABB,2 AABB-AABB/ray + face-OBB/ray,3 face-OBB/ray,4 face-ray")
#pragma GE_Documentation(StaticShadowCaster, "entity casts shadow")
#pragma GE_Documentation(Backfaced, "render backfaces")
#pragma GE_Documentation(ZSort, "z-sort faces (for correct transparency)")
} StaticMesh;
// end change


//	Particle System proxy

#pragma GE_Type("..\\icons\\sf_partsys.bmp")
typedef struct _ParticleSystemProxy
{
#pragma GE_Private
	int				psHandle;			// Particle system handle
	int				pmHandle;			// Particle map handle
	int				*effect;		  	// pointer to array of effect indexs
	geBoolean		active;
	geVec3d			OriginOffset;		//Translation from model center
#pragma GE_Published
	geVec3d			origin;
	int				nStyle;				// Particle system style
	char			*szSoundFile;		// Audio effect for particle system
	geFloat			fRadius;			// Audio audible radius
	char			*szTexture;			// Texture for particles
// changed RF064
	char			*szAlpha;			// Alphamap for particles
// end change RF064
	GE_RGBA			clrColor;			// Color for particles
	int				BirthRate;			// Over-ride default system birth rate
	int				MaxParticleCount;	// Over-ride default max particle count
	int				ParticleLifespan;	// Over-ride default particle lifespan
	char			*szEntityName;		// Name for this particle system
	char			*EntityName;		// Nmae of entity to attach to
	geWorld_Model	*Model;				// Name of model to attach to
	char			*BoneName;			// Name of actor bone to attach to
	char			*TriggerName;		// Name of trigger entity
	geFloat			Scale;
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(nStyle, "2")
#pragma GE_DefaultValue(szSoundFile, "")
#pragma GE_DefaultValue(fRadius, "200.0")
#pragma GE_DefaultValue(szTexture, "")
// changed RF064
#pragma GE_DefaultValue(szAlpha, "")
// end change RF064
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
// changed RF064
#pragma GE_Documentation(szAlpha, "Alphamap to use for particles")
// end change RF064
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

#pragma GE_Type("..\\icons\\wm_level.bmp")
typedef struct _ChangeLevel
{
#pragma GE_Private
	geBoolean		CallBack;
	int				CallBackCount;
#pragma GE_Published
	geVec3d			origin;
	char			*szEntityName;
	geWorld_Model	*Model;				// Model to use as trigger
	char			*szSplashFile;		// Bitmap to use as level-change splash screen
	char			*szAudioFile;		// Sound to play with splash, if any
	char			*szNewLevel;		// Filename for new level
	char			*szCutScene;		// Cut scene to play before level change, if any
	char			*szMessage;
	int				Font;
	geBoolean		UseOffset;
	geBoolean		UseAngle;
	char			*StartPointName;
	geBoolean		UseEffect;
	GE_RGBA			EffectColor;
	char			*Trigger;
	geBoolean		KeepAttributes;
// changed RF063
	geBoolean		UseKey;
	char			*TriggerChange;
	geBoolean		SplashHold;
// end change RF063
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
#pragma GE_DefaultValue(KeepAttributes, "True")
// changed RF063
#pragma GE_DefaultValue(UseKey, "False")
#pragma GE_DefaultValue(TriggerChange, "")
#pragma GE_DefaultValue(SplashHold, "False")
// end change RF063
#pragma GE_DefaultValue(EffectColor, "255 255 255")
#pragma GE_Documentation(szEntityName, "Name of entity (used in scripting and triggers)")
#pragma GE_Documentation(Model,"Model to use as level-change trigger")
#pragma GE_Documentation(szSplashFile,"Level change bitmap splash screen")
#pragma GE_Documentation(szAudioFile,"Sound to play with splash screen")
#pragma GE_Documentation(szNewLevel, "Name of level file to load")
#pragma GE_Documentation(szCutScene, "Inter-level cut scene to play, if any")
#pragma GE_Documentation(szMessage, "Message to display instead of Splash Screen")
#pragma GE_Documentation(Font, "Size of Message font")
#pragma GE_Documentation(Trigger, "Name of trigger entity to use")
#pragma GE_Documentation(StartPointName, "Entity Name of PlayerStart to use")
#pragma GE_Documentation(UseOffset, "if True then Offset Player at Start Position")
#pragma GE_Documentation(UseAngle, "if True then use PlayerStart Angle at Start Position")
#pragma GE_Documentation(UseEffect, "if True then use Fog Effect at level change")
#pragma GE_Documentation(EffectColor, "Color of Effect Fog")
#pragma GE_Documentation(KeepAttributes, "if False then lose attributes at level change")
// changed RF063
#pragma GE_Documentation(UseKey, "if true then can be activated by the Use Key")
#pragma GE_Documentation(TriggerChange, "Name of trigger entity that will force a Level Change")
#pragma GE_Documentation(SplashHold, "if true then display Splash screen until SPACE is pressed")
// end change RF063
} ChangeLevel;

//	SoundtrackToggle
//
//	Entity that causes passing withing range to trigger
//	..a change in the playing soundtrack.

#pragma GE_Type("..\\icons\\a_3d.bmp")
typedef struct _SoundtrackToggle
{
#pragma GE_Private
	geBoolean	alive;
	geBoolean	bActive;			// Trigger ready for use
	geFloat		LastTimeToggled;	// Last time the soundtrack was toggled
#pragma GE_Published
	geVec3d		origin;
	char		*szMIDIFileOne;		// First MIDI file to play, if any
	int			CDTrackOne;			// First CD track to play, if any
	char		*szStreamFileOne;	// First streaming sound to play, if any
	geBoolean	bOneShot;			// Soundtrack toggles only once
	geBoolean	bCDLoops;			// TRUE if the CD soundtrack loops
	geBoolean	bMIDILoops;			// TRUE if the MIDI soundtrack loops
	geBoolean	bStreamLoops;		// TRUE if the streaming audio loops
	int			SleepTime;			// Delay before next toggle is possible, in seconds
	geFloat		Range;				// Range of trigger entity
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(szMIDIFileOne, "")
#pragma GE_DefaultValue(CDTrackOne, "0")
#pragma GE_DefaultValue(szStreamFileOne, "")
#pragma GE_DefaultValue(bOneShot, "False")
#pragma GE_DefaultValue(bCDLoops, "True")
#pragma GE_DefaultValue(bMIDILoops, "True")
#pragma GE_DefaultValue(bStreamLoops, "True")
#pragma GE_DefaultValue(SleepTime, "30")
#pragma GE_DefaultValue(Range, "20.0")
#pragma GE_Documentation(szMIDIFileOne, "MIDI soundtrack file #1")
#pragma GE_Documentation(CDTrackOne, "CD Audio track #1")
#pragma GE_Documentation(szStreamFileOne, "Streaming audio file #1")
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

#pragma GE_Type("..\\icons\\a_stream.bmp")
typedef struct _StreamingAudioProxy
{
#pragma GE_Private
	geBoolean	bActive;			// Trigger ready for use
	int			LastTimeTriggered;	// Last time the stream was triggered
#pragma GE_Published
	geVec3d		origin;
	char		*szStreamFile;		// First streaming sound to play, if any
	geBoolean	bOneShot;			// Soundtrack toggles only once
	geBoolean	bLoops;				// TRUE if the soundtrack loops
	int			SleepTime;			// Delay before next toggle is possible, in seconds
	geFloat		Range;				// Range of trigger entity
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

#pragma GE_Type("..\\icons\\sf_video.bmp")
typedef struct _VideoTextureReplacer
{
#pragma GE_Private
	geBoolean	Playing;			// Video playing flag
// changed RF063
	geBoolean	Gif;
#pragma GE_Published
	geVec3d		origin;				// Entity origin
	char		*szTextureName;		// Name of texture to replace
	char		*szVideoName;		// Video to use for texture
	geFloat		Radius;				// Player enters this radius, video plays
	geBoolean	OnlyPlayInRadius;	// TRUE if the video plays only while the
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

#pragma GE_Type("..\\icons\\e_setup.bmp")
typedef struct _EnvironmentSetup
{
#pragma GE_Published
	geVec3d		origin;					// Required origin
	geFloat		Gravity;				// Player gravity
	geFloat		Speed;					// Player speed
	geFloat		JumpSpeed;				// Player jump speed
	geFloat		StepHeight;				// Player stair step-up height
	geBoolean	RealJumping;
	geBoolean	EnableDistanceFog;		// Enable engine distance fog
	GE_RGBA		DistanceFogColor;		// Color of distance fog
	geFloat		FogStartDistLow;		// Start of fog from camera, in world units
	geFloat		FogStartDistHigh;
	geFloat		TotalFogDistLow;		// Distance at which fog is total
	geFloat		TotalFogDistHigh;
	geBoolean	UseFarClipPlane;		// Set or don't use clip plane
	geFloat		FarClipPlaneDistLow;	// The clip plane distance to use
	geFloat		FarClipPlaneDistHigh;
	geFloat		MinShakeDist;
// changed RF064
// changed QD 06/26/04 each actor has its own shadowbitmap now
//	geFloat ShadowAlpha;
//	char *ShadowBitmap;
//	char *ShadowAlphamap;
// end change
	geFloat		AudibleRadius;
	geFloat		SlideSlope;
	geFloat		SlideSpeed;
	geFloat		LODdistance1;
	geFloat		LODdistance2;
	geFloat		LODdistance3;
	geFloat		LODdistance4;
	geFloat		LODdistance5;
	geBoolean	LODAnimation;
// end change RF064
// changed QD Shadows
	GE_RGBA		SShadowsColor;
	geFloat		SShadowsAlpha;
	int			SShadowsMaxLightToUse;
// end change
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(Gravity, "0")
#pragma GE_DefaultValue(Speed, "0")
#pragma GE_DefaultValue(JumpSpeed, "0")
#pragma GE_DefaultValue(StepHeight, "0")
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
#pragma GE_DefaultValue(MinShakeDist, "50.0")
// changed RF064
// changed QD 06/26/04
//#pragma GE_DefaultValue(ShadowAlpha, "0.0")
//#pragma GE_DefaultValue(ShadowBitmap, "")
//#pragma GE_DefaultValue(ShadowAlphamap, "")
// end change QD
#pragma GE_DefaultValue(AudibleRadius, "500.0")
#pragma GE_DefaultValue(SlideSlope, "0.8")
#pragma GE_DefaultValue(SlideSpeed, "40")
#pragma GE_DefaultValue(LODdistance1, "0.0")
#pragma GE_DefaultValue(LODdistance2, "0.0")
#pragma GE_DefaultValue(LODdistance3, "0.0")
#pragma GE_DefaultValue(LODdistance4, "0.0")
#pragma GE_DefaultValue(LODdistance5, "0.0")
#pragma GE_DefaultValue(LODAnimation, "False")
// end change RF064
// changed QD Shadows
#pragma GE_DefaultValue(SShadowsColor, "0 0 0")
#pragma GE_DefaultValue(SShadowsAlpha, "96.0")
#pragma GE_DefaultValue(SShadowsMaxLightToUse, "0")
// end change
#pragma GE_Documentation(Gravity, "Downward force of gravity")
#pragma GE_Documentation(RealJumping, "if true use realistic jumping")
#pragma GE_Documentation(Speed, "Player speed of motion")
#pragma GE_Documentation(JumpSpeed, "Player upward jump speed")
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
#pragma GE_Documentation(MinShakeDist, "Distance inside which explosion shake is at maximum")
// changed RF064
// changed QD 06/26/04
//#pragma GE_Documentation(ShadowAlpha, "Transparency of actor shadows (0 to 255)")
//#pragma GE_Documentation(ShadowBitmap, "name of bitmap to use for shadows")
//#pragma GE_Documentation(ShadowAlphamap, "name of alpha bitmap to use for shadows")
// end change QD
#pragma GE_Documentation(AudibleRadius, "Maximum distance sound is audible")
#pragma GE_Documentation(SlideSlope, "Min Slope required to start sliding (1 - flat, 0 - verticle")
#pragma GE_Documentation(SlideSpeed, "Speed factor for slope sliding")
#pragma GE_Documentation(LODAnimation, "if True then LOD actors have their own animations")
// end change RF064
// changed QD Shadows
#pragma GE_Documentation(SShadowsColor, "color of stencil shadows")
#pragma GE_Documentation(SShadowsAlpha, "alpha of stencil shadows")
#pragma GE_Documentation(SShadowsMaxLightToUse, "max number of lights which cast stencil shadows")
// end change
} EnvironmentSetup;


//	Corona
//
//	Put a corona around a light or other object.

#pragma GE_Type("..\\icons\\sf_corona.bmp")
typedef struct _Corona
{
#pragma	GE_Private
	int				*effect;		// pointer to array of effect indexs
	geBoolean		active;			// is activated
	geVec3d			OriginOffset;	// Translation from model center
#pragma GE_Published
	geVec3d			origin;
	char 			*szEntityName;
	geFloat			FadeTime;
	geFloat 		MinRadius;
	geFloat			MaxRadius;
	geFloat			MaxVisibleDistance;
	geFloat			MaxRadiusDistance;
	geFloat			MinRadiusDistance;
	GE_RGBA			Color;
	char 			*EntityName;	// Name of entity to attach to
	geWorld_Model	*Model;			// Name of model to attach to
	char 			*BoneName;		// Name of actor bone to attach to
	char 			*TriggerName;	// Name of trigger entity
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
//	A light that dynamically illuminates the level area.

#pragma GE_Type("..\\icons\\sf_dynlight.bmp")
typedef struct _DynamicLight
{
#pragma	GE_Private
	geFloat			LastTime;
	geFloat			IntervalWidth;
	int				NumFunctionValues;
	geLight			*DynLight;
	geBoolean		active;			// is activated
// MOD010124 - Added next private variable
	geVec3d			OriginOffset;	// Translation from model center
#pragma GE_Published
	geVec3d			origin;
	char			*szEntityName;
	int				MinRadius;
	int				MaxRadius;
	geBoolean		InterpolateValues;
	char			*RadiusFunction;
	geFloat			RadiusSpeed;
	GE_RGBA			Color;
	char 			*EntityName;	// Name of entity to attach to
	geWorld_Model	*Model;			// Name of model to attach to
	char 			*BoneName;		// Name of actor bone to attach to
	char 			*TriggerName;	// Name of trigger entity
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
//	Defines the target for an electrical bolt effect.

#pragma GE_Type("..\\icons\\sf_elecbterm.bmp")
typedef struct _ElectricBoltTerminus
{
#pragma GE_Private
	geVec3d			OriginOffset;	// Translation from model center
#pragma GE_Published
	geVec3d			origin;
	char			*szEntityName;
	char 			*EntityName;	// Name of entity to attach to
	geWorld_Model	*Model;			// Name of model to attach to
	char 			*BoneName;		// Name of actor bone to attach to
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
//	Emitter for electrical bolt effects.

typedef struct Electric_BoltEffect Electric_BoltEffect;

#pragma GE_Type("..\\icons\\sf_electricb.bmp")
typedef struct _ElectricBolt
{
#pragma	GE_Private
	geFloat			LastTime;
	geFloat			LastBoltTime;
	Electric_BoltEffect	*Bolt;
	geBoolean		active;				// is activated
	int				*effect;			// pointer to array of effect indexs
	geBitmap		*Bitmap;
	geVec3d			OriginOffset;		// Translation from model center
	geBoolean		bState;
	geFloat			DTime;
	geBoolean		DoingDamage;
#pragma GE_Published
	geVec3d			origin;
	char			*szEntityName;
	char			*SoundFile;
	geFloat			fRadius;			// Maximum audible radius for sound
	char			*BmpName;
	char			*AlphaName;
	int				Width;
	int				NumPoints;
	geBoolean		Intermittent;
	geFloat			MinFrequency;
	geFloat			MaxFrequency;
	geFloat			Wildness;
	ElectricBoltTerminus *Terminus;
	int				DominantColor;
	GE_RGBA			Color;
	char 			*EntityName;		// Name of entity to attach to
	geWorld_Model	*Model;				// Name of model to attach to
	char 			*BoneName;			// Name of actor bone to attach to
	char 			*TriggerName;		// Name of trigger entity
	geBoolean		DoDamage;
	char			*DamageAttribute;
	geFloat			DamageAmt;
// changed RF063
	char			*DamageAltAttribute;
	geFloat			DamageAltAmt;
// end change RF063
	geFloat			DamageTime;
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
#pragma GE_DefaultValue(DoDamage, "False")
#pragma GE_DefaultValue(DamageAttribute, "health")
#pragma GE_DefaultValue(DamageAmt, "1")
// changed RF063
#pragma GE_DefaultValue(DamageAltAttribute, "")
#pragma GE_DefaultValue(DamageAltAmt, "1")
// end change RF063
#pragma GE_DefaultValue(DamageTime, "1")
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
#pragma GE_Documentation(DoDamage, "true if bolt does damage")
#pragma GE_Documentation(DamageAttribute, "Name of attribute damaged")
#pragma GE_Documentation(DamageAmt, "Amount of damage done")
// changed RF063
#pragma GE_Documentation(DamageAltAttribute, "Name of alternate attribute damaged")
#pragma GE_Documentation(DamageAltAmt, "Amount of alternate damage done")
// end change Rf063
#pragma GE_Documentation(DamageTime, "Number of seconds before damage is done again")
}	ElectricBolt;


//	TextureProc
//
//	Procedural texture entity,

typedef struct Procedural	Procedural;

#pragma GE_Type("..\\icons\\sf_proctex.bmp")
typedef struct _TextureProc
{
#pragma GE_Private
	Procedural	*Proc;
#pragma GE_Published
	geVec3d		origin;			// Entity origin
	char		*szTextureName;	// Name of texture to replace
	char		*szProcName;	// Proc to use for texture
	char		*Parameter;
// changed Nout 12/15/05
	char		*TriggerName;	// Name of trigger entity
// end change
	geBoolean	DistanceFlag;
	geFloat		Distance;
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(szTextureName, "")
#pragma GE_DefaultValue(szProcName, "")
#pragma GE_DefaultValue(Parameter, "")
// changed Nout 12/15/05
#pragma GE_DefaultValue(TriggerName, "")
// end change
#pragma GE_DefaultValue(DistanceFlag, "False")
#pragma GE_DefaultValue(Distance, "400")
#pragma GE_Documentation(szTextureName, "Name of texture to replace")
#pragma GE_Documentation(szProcName, "Procedural to replace texture")
#pragma GE_Documentation(Parameter, "Parameter string for procedural")
// changed Nout 12/15/05
#pragma GE_Documentation(TriggerName, "Name of trigger entity to use")
// end change
#pragma GE_Documentation(DistanceFlag, "True if texture stops rendering past defined distance")
#pragma GE_Documentation(Distance, "Distance after which texture stops rendering")
} TextureProc;


//	PathPoint
//
//	Defines a single point of a path to be followed by other
//	..entities.

#pragma GE_Type("..\\icons\\ai_pathpoint.bmp")
typedef struct _PathPoint
{
#pragma	GE_Private
#pragma GE_Published
	geVec3d	origin;
	int		PointType;			// Type of this point
	char	*PointName;			// Name of this point
	char	*NextPointName;		// Name of next point in list, if any
	geFloat Range;				// For ranging, ranging distance
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
//	..whose first point is named in the entity.

#pragma GE_Type("..\\icons\\ai_pathfollow.bmp")
typedef struct _PathFollower
{
#pragma GE_Private
	geFloat			TimeInMotion;		// Used to compute speed between points
	geFloat			LastMotionTick;		// Last time this entity was processed
	geBoolean		bReady;				// Follower ready for use
	geBoolean		bMoving;			// Follower is doing motion
	geBoolean		bTriggered;			// Motion has been triggered
	geBoolean		bForward;			// PAth motion direction flag
	geVec3d			PathOrigin;			// Position of head of path
	int				CurrentNodeType;	// Type of current node
	geVec3d			CurrentPosition;	// Entities current position
	geFloat			PointRange;			// If at ranged point, the range
	geVec3d			CurrentTarget;		// Entities current target position
	int				PathHandle;			// Handle if PATH and not RANGED POINT ONLY
#pragma GE_Published
	geVec3d			origin;
	char			*PathFirstNode;		// Name of first node in path to follow
	char			*EntityName;		// Name of entity to move along path
	geFloat			Speed;				// Speed along path, in world units/second
	geWorld_Model	*Trigger;			// Motion trigger, if any
	geFloat			RangeTrigger;		// Range trigger for motion, if any
	geBoolean		MotionLoops;		// TRUE if motion will loop (bidirectional)
	int				RandomRollTime;		// For RANGED point, # of milliseconds between new point generation
	geBoolean		CollisionTest;		// TRUE if entity motion needs to be collision tested
	geBoolean		Reverse;
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

#pragma GE_Type("..\\icons\\psetup.bmp")
typedef struct _PlayerSetup
{
#pragma GE_Private
#pragma GE_Published
	geVec3d		origin;
// changed RF063
	char		*ActorName;
// end change RF063
	char		*HUDInfoFile;			// File containing HUD description
	char		*AttributeInfoFile;		// File containing attribute declarations
	char		*OpeningCutScene;		// AVI for opening cut scene, if any
	geFloat		PlayerScaleFactor;		// Player scaling factor
	geFloat		ActorScaleFactor;
	geVec3d		ActorRotation;			// Actor rotation to set default orientation
	geBoolean	HeadBobbing;			// eaa3 12/18/2000 Head bobbing enable
	geFloat		HeadBobSpeed;			// eaa3 12/18/2000 Speed of head bob
	geFloat		HeadBobLimit;			// eaa3 12/18/2000 Max. amount of head bob
	int			LevelViewPoint;
// changed RF064
	geBoolean	LockView;
	geBoolean	MovieMode;
// end change RF064
	geBoolean	UseDeathFog;
	GE_RGBA		DeathFogColor;
	char		*DeathMessage;
	int			DeathFontSize;
	char		*Deathlevel;
	geFloat		ShadowSize;
// changed QD 06/26/04
	geFloat		ShadowAlpha;
	char		*ShadowBitmap;
	char		*ShadowAlphamap;
// end change
	geBoolean	UseProjectedShadows;	// change gekido - Projected Shadows
// changed QD Shadows
	geBoolean	UseStencilShadows;
// end change
	geBoolean	KeepAttrAtDeath;
#pragma GE_Origin(origin)
// changed RF063
#pragma GE_DefaultValue(ActorName, "")
// end change RF063
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
// changed RF064
#pragma GE_DefaultValue(LockView, "False")
#pragma GE_DefaultValue(MovieMode, "False")
// end change RF064
#pragma GE_DefaultValue(UseDeathFog, "False")
#pragma GE_DefaultValue(DeathFogColor, "255 0 0")
#pragma GE_DefaultValue(DeathMessage, "")
#pragma GE_DefaultValue(DeathFontSize, "0")
#pragma GE_DefaultValue(Deathlevel, "")
#pragma GE_DefaultValue(ShadowSize, "0")
// changed QD 06/26/04
#pragma GE_DefaultValue(ShadowAlpha, "0.0")
#pragma GE_DefaultValue(ShadowBitmap, "")
#pragma GE_DefaultValue(ShadowAlphamap, "")
// end change
#pragma GE_DefaultValue(UseProjectedShadows, "False") // change gekido - Projected Shadows
// changed QD Shadows
#pragma GE_DefaultValue(UseStencilShadows, "False")
// end change
#pragma GE_DefaultValue(KeepAttrAtDeath, "False")
// changed RF063
#pragma GE_Documentation(ActorName, "Name of actor to use as player in this level")
// end change RF063
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
// changed RF064
#pragma GE_Documentation(LockView, "TRUE if viewpoint is unchangable in level")
#pragma GE_Documentation(MovieMode, "TRUE if player input is disabled and ESC sets KeyMonitor to on")
// end change RF064
#pragma GE_Documentation(UseDeathFog, "If true use fog swirl at death")
#pragma GE_Documentation(DeathFogColor, "Color of fog used at death")
#pragma GE_Documentation(DeathMessage, "Message to display at death")
#pragma GE_Documentation(DeathFontSize, "Font size of death message")
#pragma GE_Documentation(Deathlevel, "Level to change to at death")
#pragma GE_Documentation(ShadowSize, "Size of actor shadow, 0 is none")
// changed QD 06/26/04
#pragma GE_Documentation(ShadowAlpha, "Transparency of actor shadow (0 to 255)")
#pragma GE_Documentation(ShadowBitmap, "name of bitmap to use for shadow")
#pragma GE_Documentation(ShadowAlphamap, "name of alpha bitmap to use for shadow")
// end change
#pragma GE_Documentation(UseProjectedShadows, "use projected shadows for the player")  // change gekido - Projected Shadows
// changed QD Shadows
#pragma GE_Documentation(UseStencilShadows, "use stencil shadows for the player")
// end change
#pragma GE_Documentation(KeepAttrAtDeath, "if TRUE then retain all current attributes after dying")

} PlayerSetup;


//	Floating Particles Entity

#pragma GE_Type("..\\icons\\wm_floatpart.bmp")
typedef struct FloatingParticles
{
#pragma	GE_Private
	int 			*EffectList;
	int				EffectCount;
	geXForm3d		*Xf;
	geFloat			*Speed;
	geVec3d			BasePos;
	geBitmap		*Bitmap;
	geBoolean		active;
	geVec3d			OriginOffset;	// Translation from model center
#pragma GE_Published
	geVec3d			origin;
	char			*szEntityName;
	GE_RGBA			Color;
	geFloat			Scale;
	int				ParticleCount;
	geFloat			Radius;
	geFloat			Height;
	geFloat			XSlant;
	geFloat			ZSlant;
	geFloat			MinSpeed;
	geFloat			MaxSpeed;
	char			*BmpName;
	char			*AlphaName;
	char 			*EntityName;	// Name of entity to attach to
	geWorld_Model	*Model;			// Name of model to attach to
	char 			*BoneName;		// Name of actor bone to attach to
	char 			*TriggerName;	// Name of trigger entity
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
#pragma GE_Documentation( ZSlant, "Upwards slant on Z axis" )
#pragma GE_Documentation( MinSpeed, "Min speed of each particle" )
#pragma GE_Documentation( MaxSpeed, "Max speed of each particle" )
#pragma GE_Documentation( BmpName, "Name of bitmap file to use" )
#pragma GE_Documentation( AlphaName, "Name of alpha bitmap file to use" )
#pragma GE_Documentation(EntityName, "Name of entity to attach to")
#pragma GE_Documentation(Model, "Name of model to attach to")
#pragma GE_Documentation(BoneName, "Name of actor bone to attach to")
#pragma GE_Documentation(TriggerName, "Name of trigger entity to use")
} FloatingParticles;

//	Chaos Procedural

#pragma GE_Type("..\\icons\\sf_chaos.bmp")
typedef struct EChaos
{
#pragma	GE_Private
	geBitmap	*CAttachBmp;
	geBitmap	*OriginalBmp;
	geBitmap	*WorkBmp;
	geFloat		XOffset;
	geFloat		YOffset;
	int			SegmentSize;
	int			SegmentCount;
// changed RF064
	geActor		*Actor;
// end change RF064
#pragma GE_Published
	geVec3d		origin;
	int			MaxXSway;
	int			MaxYSway;
	geFloat		XStep;
	geFloat		YStep;
	char		*AttachBmp;
// changed RF064
	char		*EntityName;
// end change RF064
#pragma GE_Origin(origin)
#pragma GE_DefaultValue( MaxXSway, "10" )
#pragma GE_DefaultValue( MaxYSway, "10" )
#pragma GE_DefaultValue( XStep, "1.0" )
#pragma GE_DefaultValue( YStep, "1.0" )
// changed RF064
#pragma GE_DefaultValue(EntityName, "" )
// end change RF064
#pragma GE_Documentation( MaxXSway, "Total horizontal texture pixel sway" )
#pragma GE_Documentation( MaxYSway, "Total vertical texture pixel sway" )
#pragma GE_Documentation( XStep, "Horizontal scroll speed" )
#pragma GE_Documentation( YStep, "Vertical scroll speed" )
#pragma GE_Documentation( AttachBmp, "Name of the texture to attach this effect to" )
// changed RF064
#pragma GE_Documentation(EntityName, "Name of entity whose actor texture is to be changed" )
// end change RF064
} EChaos;

//	Rain Effect

#pragma GE_Type("..\\icons\\sf_rain.bmp")
typedef struct Rain
{
#pragma GE_Private
	int 			*EffectList;
	int				EffectCount;
	geBoolean		active;
	geVec3d			OriginOffset;	// Translation from model center
#pragma GE_Published
	geVec3d			origin;
	char			*szEntityName;
	geVec3d			Gravity;
	geFloat			Radius;
	geFloat			Severity;
	geFloat			DropLife;
	GE_RGBA			ColorMin;
	GE_RGBA			ColorMax;
	char			*BmpName;
	char			*AlphaName;
	char 			*EntityName;	// Name of entity to attach to
	geWorld_Model	*Model;			// Name of model to attach to
	char			*BoneName;		// Name of actor bone to attach to
	char 			*TriggerName;	// Name of trigger entity
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


//	Spout Entity

#pragma GE_Type("..\\icons\\sf_spout.bmp")
typedef struct Spout
{
#pragma	GE_Private
	int				EffectList;
	int				EffectCount;
	geFloat			PauseTime;
	geBoolean 		PauseState;
	geBoolean		active;
	geVec3d			OriginOffset;	// Translation from model center
#pragma GE_Published
	geVec3d			origin;
	char			*szEntityName;
	geVec3d			Angles;
	geFloat			ParticleCreateRate;
	geFloat			MinScale;
	geFloat			MaxScale;
	geFloat			MinSpeed;
	geFloat			MaxSpeed;
	geFloat			MinUnitLife;
	geFloat			MaxUnitLife;
	int				SourceVariance;
	int				DestVariance;
	GE_RGBA			ColorMin;
	GE_RGBA			ColorMax;
	geVec3d			Gravity;
	char			*BmpName;
	char			*AlphaName;
	geFloat			MinPauseTime;
	geFloat			MaxPauseTime;
	geFloat			TotalLife;
	geBoolean		Bounce;
	char 			*EntityName;	// Name of entity to attach to
	geWorld_Model	*Model;			// Name of model to attach to
	char 			*BoneName;		// Name of actor bone to attach to
	char 			*TriggerName;	// Name of trigger entity
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
#pragma GE_Documentation(Angles, "Direction in which particles will shoot" )
#pragma GE_Documentation(ParticleCreateRate, "Every how many seconds to add a new particle" )
#pragma GE_Documentation(MinScale, "Min scale of the textures" )
#pragma GE_Documentation(MaxScale, "Max scale of the textures" )
#pragma GE_Documentation(MinSpeed, "Min speed of the textures" )
#pragma GE_Documentation(MaxSpeed, "Max speed of the textures" )
#pragma GE_Documentation(MinUnitLife, "Min life of each texture" )
#pragma GE_Documentation(MaxUnitLife, "Max life of each texture" )
#pragma GE_Documentation(SourceVariance, "How much to vary spray source point" )
#pragma GE_Documentation(DestVariance, "How much to vary spray dest point" )
#pragma GE_Documentation(ColorMin, "Minimum RGB values for each particle" )
#pragma GE_Documentation(ColorMax, "Maximum RGB values for each particle" )
#pragma GE_Documentation(Gravity, "Gravity vector to apply to each particle" )
#pragma GE_Documentation(BmpName, "Name of bitmap file to use" )
#pragma GE_Documentation(AlphaName, "Name of alpha bitmap file to use" )
#pragma GE_Documentation(TotalLife, "How many seconds this spout lasts. Set to 0 for continuous." )
#pragma GE_Documentation(MinPauseTime, "Low range of randomly chosen pause time (seconds)" )
#pragma GE_Documentation(MaxPauseTime, "High range of randomly chosen pause time (seconds)" )
#pragma GE_Documentation(EntityName, "Name of entity to attach to")
#pragma GE_Documentation(Bounce, "True if particles bounce when they hit something")
#pragma GE_Documentation(Model, "Name of model to attach to")
#pragma GE_Documentation(BoneName, "Name of actor bone to attach to")
#pragma GE_Documentation(TriggerName, "Name of trigger entity to use")
} Spout;


//	Flame Entity

#pragma GE_Type("..\\icons\\sf_flame.bmp")
typedef struct Flame
{
#pragma	GE_Private
	int				EffectList0;
	int				EffectList1;
	geBoolean		active;
	geVec3d			OriginOffset;	// Translation from model center
#pragma GE_Published
	geVec3d			origin;
	char			*szEntityName;
	geVec3d			Angles;
	geFloat			Scale;
	geFloat			ParticleCreateRate;
	geFloat			MinScale;
	geFloat			MaxScale;
	geFloat			MinSpeed;
	geFloat			MaxSpeed;
	geFloat			MinUnitLife;
	geFloat			MaxUnitLife;
	int				SourceVariance;
	int				DestVariance;
	GE_RGBA			ColorMin;
	GE_RGBA			ColorMax;
	geVec3d			Gravity;
	char			*BmpName;
	char			*AlphaName;
	geFloat			TotalLife;

	geFloat			GRadiusMin;
	geFloat			GRadiusMax;
	GE_RGBA			GColorMin;
	GE_RGBA			GColorMax;
	geFloat			GIntensity;
	geBoolean		GCastShadows;
	char 			*EntityName;	// Name of entity to attach to
	geWorld_Model	*Model;			// Name of model to attach to
	char 			*BoneName;		// Name of actor bone to attach to
	char 			*TriggerName;	// Name of trigger entity
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

//	Trigger - sets state to true when hit

#pragma GE_Type("..\\icons\\wm_trigger.bmp")
typedef struct _Trigger
{
#pragma GE_Private
	geBoolean		bInAnimation;	// Is trigger animating?
	geBoolean		bTrigger;		// Has the trigger been activated?
	geFloat			AnimationTime;	// Ticks in animation
	geBoolean		bInCollision;	// Trigger is colliding flag
	geBoolean		bActive;		// Trigger can be used
	geFloat			LastIncrement;	// Last tick increment
	geFloat			tDoor;			// Trigger model animation position time
	geSound_Def		*theSound;		// Sound def pointer
	int				SoundHandle;	// For later modification of sound
	geBoolean		bState;			// Trigger state
	geFloat			time;			// on/off timer
	geBoolean		isHit;
	geBoolean		CallBack;
	int				CallBackCount;
#pragma GE_Published
	geWorld_Model	*Model;			// Trigger model
	geVec3d			origin;			// Location of trigger
	geBoolean		bOneShot;		// Trigger only works ONCE
	geBoolean		bShoot;			// Trigger only works if shot
	geFloat			TimeOn;			// seconds trigger is on
	geFloat			TimeOff;		// seconds trigger is off
	char			*szSoundFile;	// Name of sound to play on activation
	geFloat			AnimationSpeed;	// Speed of the animation, in %
	char			*szEntityName;	// Name of entity
	geBoolean		bAudioLoops;	// Audio effect loops while animation plays
	char			*TriggerName;
// changed RF063
	geBoolean		UseKey;
	geBoolean		PlayerOnly;
// end change RF063
// changed RF064
	geBoolean		bNoCollide;
// end change RF064
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
// changed RF063
#pragma GE_DefaultValue(UseKey, "False")
#pragma GE_DefaultValue(PlayerOnly, "False")
// end change RF063
// changed RF064
#pragma GE_DefaultValue(bNoCollide, "False")
// end change RF064
#pragma GE_Documentation(Model, "Trigger to use")
#pragma GE_Documentation(szSoundFile, "Sound file to play when trigger hit")
#pragma GE_Documentation(bOneShot, "true if trigger only works once")
#pragma GE_Documentation(bShoot, "true if trigger only works if shot")
#pragma GE_Documentation(TimeOn, "seconds that trigger is on")
#pragma GE_Documentation(TimeOff, "seconds that trigger is off")
#pragma GE_Documentation(szEntityName, "Name of entity (used in scripting and triggers)")
#pragma GE_Documentation(bAudioLoops, "TRUE if audio loops while animation plays")
#pragma GE_Documentation(TriggerName, "Name of trigger entity to use")
#pragma GE_Documentation(AnimationSpeed,"Speed of animation, 1.0 = 100% original")
// changed RF063
#pragma GE_Documentation(UseKey, "if true then can be activated by Use Key")
#pragma GE_Documentation(PlayerOnly, "if true then can be activated only by player")
// end change RF063
// changed RF064
#pragma GE_Documentation(bNoCollide, "if true then collision will not activate trigger")
// end change RF064
} Trigger;

//	Logic entity

#pragma GE_Type("..\\icons\\logicgate.bmp")
typedef struct _LogicGate
{
#pragma GE_Private
	geBoolean	bState;
	geFloat		time;
	geBoolean	active;			// is activated
	geBoolean	OldState;
	geBoolean	inDelay;
#pragma GE_Published
	geVec3d		origin;			// Location of entity in world
	char		*szEntityName;	// Name of this entity
	char		*Trigger1Name;	// Name of trigger 1 entity
	char		*Trigger2Name;	// Name of trigger 2 entity
	int			Type;
	geFloat		Delay;
// changed RF063
	int			Amount;
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(szEntityName, "")
#pragma GE_DefaultValue(Trigger1Name, "")
#pragma GE_DefaultValue(Trigger2Name, "")
#pragma GE_DefaultValue(Type, "0")
#pragma GE_DefaultValue(Delay, "0")
// changed RF063
#pragma GE_DefaultValue(Amount, "0")
#pragma GE_Documentation(szEntityName, "Name of entity (used in scripting and triggers)")
#pragma GE_Documentation(Trigger1Name, "Name of trigger 1 entity to use")
#pragma GE_Documentation(Trigger2Name, "Name of trigger 2 entity to use")
#pragma GE_Documentation(Type, "0-AND 1-DELAY 2-NOT 3-OR 4-XOR 5-FLIPFLOP 6-CALLBACK")
#pragma GE_Documentation(Delay, "Number of seconds to delay change")
// changed RF063
#pragma GE_Documentation(Amount, "Minimum amount of attribute")
} LogicGate;

// Message entity

#pragma GE_Type("..\\icons\\message.bmp")
typedef struct _Message
{
#pragma GE_Private
	void	*Data;
#pragma GE_Published
	geVec3d origin;
	char	*szEntityName;	// Name of this entity
	char	*DisplayType;
	char	*TriggerName;
	char	*TextName;
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(szEntityName, "")
#pragma GE_DefaultValue(DisplayType, "Static")
#pragma GE_DefaultValue(TriggerName, "")
#pragma GE_DefaultValue(TextName, "")
#pragma GE_Documentation(szEntityName, "Name of entity (used in scripting and triggers)")
#pragma GE_Documentation(DisplayType, "Name of message type (can only be Static currently)")
#pragma GE_Documentation(TriggerName, "Name of trigger entity to use")
#pragma GE_Documentation(TextName, "Name of text section to display")

} Message;


//
// DecalDefine
//

#pragma GE_Type("..\\icons\\decdefin.bmp")
typedef struct _DecalDefine
{
#pragma GE_Private
	geBitmap	*Bitmap;
#pragma GE_Published
	geVec3d		origin;
	int 		Type;
	char 		*BmpName;
	char 		*AlphaName;
	geFloat		Width;
	geFloat		Height;
	geFloat		Alpha;
	geFloat		LifeTime;
	GE_RGBA		Color;
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

#pragma GE_Type("..\\icons\\walldecal.bmp")
typedef struct _WallDecal
{
#pragma GE_Private
// changed RF064
	void			*vertex;
	geBitmap		**FBitmap;
	geFloat			Time;
	int				CurTex;
	int				CycleDir;
	geVec3d			OriginOffset;
// end change RF064
	geBitmap		*Bitmap;
	geBoolean		active;
	gePoly			*Poly;
// Start Aug2003DCS
	geBoolean		ProgrammedTrigger;
	geBoolean		BmpChanged;
	geVec3d			OriginalAngle;
	geVec3d			LastAngle;
// End Aug2003DCS
#pragma GE_Published
	geVec3d			origin;
// Start Aug2003DCS
	char			*szEntityName;	// Name of this entity
// End Aug2003DCS
	geVec3d			Angle;
	char 			*BmpName;
	char 			*AlphaName;
	geFloat			Width;
	geFloat			Height;
	geFloat			Alpha;
	GE_RGBA			Color;
	char			*TriggerName;
// changed RF064
	geBoolean		Animated;
	int				BitmapCount;
	int				Style;
	geFloat			Speed;
	char 			*EntityName;	// Name of entity to attach to
	geWorld_Model	*Model;			// Name of model to attach to
	char 			*BoneName;		// Name of actor bone to attach to
// end change RF064
#pragma GE_Origin(origin)
#pragma GE_Angles(Angle)
#pragma GE_DefaultValue(BmpName, "")
#pragma GE_DefaultValue(AlphaName, "")
#pragma GE_DefaultValue(Width, "10.0")
#pragma GE_DefaultValue(Height, "10.0")
#pragma GE_DefaultValue(Alpha, "255")
#pragma GE_DefaultValue(Color, "255.0 255.0 255.0")
#pragma GE_DefaultValue(TriggerName, "")
// changed RF064
#pragma GE_DefaultValue(Animated, "False")
#pragma GE_DefaultValue(BitmapCount, "1")
#pragma GE_DefaultValue(Style, "0")
#pragma GE_DefaultValue(Speed, "10")
#pragma GE_DefaultValue(EntityName, "")
#pragma GE_DefaultValue(BoneName, "")
// end change RF064
#pragma GE_Documentation(BmpName, "Name of bitmap")
#pragma GE_Documentation(AlphaName, "Name of alpha bitmap")
#pragma GE_Documentation(Width, "Width of Decal")
#pragma GE_Documentation(Height, "Height of Decal")
#pragma GE_Documentation(Alpha, "Transparency level - 0 to 255")
#pragma GE_Documentation(Color, "Color of decal")
#pragma GE_Documentation(TriggerName, "Name of trigger entity to use")
// changed RF064
#pragma GE_Documentation(Animated, "true if decal is animated")
#pragma GE_Documentation(BitmapCount, "number of bitmaps in sequence")
#pragma GE_Documentation(Style, "Animation style 0 to 3")
#pragma GE_Documentation(Speed, "# of bitmaps per second")
#pragma GE_Documentation(EntityName, "Name of entity to attach to")
#pragma GE_Documentation(Model, "Name of model to attach to")
#pragma GE_Documentation(BoneName, "Name of actor bone to attach to")
// end change RF064
} WallDecal;

// FirePoint

#pragma GE_Type("..\\icons\\firepoint.bmp")
typedef struct _FirePoint
{
#pragma GE_Private
	geBoolean		active;
	geFloat			Tick;
	geFloat			FireRate;
	geVec3d			OriginOffset;	// Translation from model center
	geVec3d			RealAngle;
#pragma GE_Published
	geVec3d			origin;
	geVec3d			Angle;
	char			*szEntityName;	// Name of entity
	char 			*EntityName;	// Name of entity to attach to
	geWorld_Model	*Model;			// Name of model to attach to
	char 			*BoneName;		// Name of actor bone to attach to
	char 			*TriggerName;	// Name of trigger entity
	char			*Projectile;
	geFloat			MinFireRate;
	geFloat			MaxFireRate;
	char			*szSoundFile;
	char			*Attribute;
// changed RF063
	char			*AltAttribute;
// end change RF063
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
// changed RF063
#pragma GE_DefaultValue(AltAttribute, "")
// end change RF063
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
// changed RF063
#pragma GE_Documentation(AltAttribute, "Alternate Attribute that projectile damages")
// end change RF063
} FirePoint;

// DestroyableModel

#pragma GE_Type("..\\icons\\model.ico")
typedef struct _DestroyableModel
{
#pragma GE_Private
	geBoolean		active;
	geBoolean		bState;
	geBoolean		CallBack;
	int				CallBackCount;
	geFloat			OriginalAmt;
// changed RF064
	geBoolean		Animating;
// end change RF064
#pragma GE_Published
	geVec3d			origin;
	char			*szEntityName;	// Name of entity
	char 			*TriggerName;	// Name of trigger entity
	geWorld_Model	*Model;			// Name of model to attach to
	geFloat			AttributeAmt;	// Props health points
	char			*Attribute;
	char			*szSoundFile;
// changed RF064
	geBoolean		Animate;
	geWorld_Model	*Model1;
	geWorld_Model	*Model2;
	geWorld_Model	*Model3;
	geWorld_Model	*Model4;
	geBoolean		NoRemove;
// end change RF064
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(szEntityName, "")
#pragma GE_DefaultValue(TriggerName, "")
#pragma GE_DefaultValue(AttributeAmt, "100")
#pragma GE_DefaultValue(Attribute, "health")
#pragma GE_DefaultValue(szSoundFile, "")
// changed RF064
#pragma GE_DefaultValue(Animate, "False")
#pragma GE_DefaultValue(NoRemove, "False")
// end change RF064
#pragma GE_Documentation(szEntityName, "Name of entity (used in scripting and triggers)")
#pragma GE_Documentation(Model, "Name of model to attach to")
#pragma GE_Documentation(TriggerName, "Name of trigger entity to use")
#pragma GE_Documentation(szSoundFile, "Name of sound to play when model is destroyed")
// changed RF064
#pragma GE_Documentation(Animate, "Animate model when destroyed")
#pragma GE_Documentation(Model1, "Name of model to attach to")
#pragma GE_Documentation(Model2, "Name of model to attach to")
#pragma GE_Documentation(Model3, "Name of model to attach to")
#pragma GE_Documentation(Model4, "Name of model to attach to")
#pragma GE_Documentation(NoRemove, "if true then don't remove model(s) when dead")
// end change RF064
} DestroyableModel;

//	Model State Modifier
//
//	This entity allows the designer to set "state variables" for individual
//	..models in each level.  These states change how actors interact with the
//	..models, etc. - note that you have to make your world geometry element into
//	..a "model" in the World Editor in order to apply this entity to it!

#pragma GE_Type("..\\icons\\wm_modattrib.bmp")
typedef struct _ModelStateModifier
{
#pragma GE_Private
	geFloat			DTime;
	geFloat			FTime;
	geBoolean		DoForce;
	geBoolean		DoDamage;
	geSound_Def		*theFSound;			// Handle of audio effect
	geSound			*FSoundHandle;
	geSound_Def		*theDSound;			// Handle of audio effect
	geSound			*DSoundHandle;
#pragma GE_Published
	geVec3d			origin;				// Origin of entity
	geWorld_Model	*theModel;			// Model to assign modifiers to
	geBoolean		ForceImparter;		// TRUE if model imparts force to player
// changed RF064
	geVec3d			ForceVector;		// Force vector ID (0=down,1=up,2=left,3=right, 4=forward, 5=backward)
// end change RF064
	geFloat			ForceAmount;		// Amount of force to impact
	geFloat			ForceDecay;			// Amount to decay force imparted
	char			*ForceAudioEffect;	// Audio effect to play when force is applied
	geFloat			ForceTimeDelay;		// Delay (in sec.) before force or damage engages
	geFloat			DamageTimeDelay;
	geBoolean		DamageGenerator;	// TRUE if contact causes damage
	geFloat			DamageAmount;		// Amount of damage per collision
// changed RF063
	geFloat			DamageAltAmount;
	char			*DamageAudioEffect;	// Audio effect to play when damage is applied
	char			*DamageAttribute;
	char			*DamageAltAttribute;
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(ForceImparter, "False")
#pragma GE_DefaultValue(DamageGenerator, "False")
// changed RF064
#pragma GE_DefaultValue(ForceVector, "0 1 0")
// end change RF064
#pragma GE_DefaultValue(ForceAmount, "100.0")
#pragma GE_DefaultValue(ForceDecay, "20.0")
#pragma GE_DefaultValue(ForceAudioEffect, "")
#pragma GE_DefaultValue(ForceTimeDelay, "1")
#pragma GE_DefaultValue(DamageTimeDelay, "1")
#pragma GE_DefaultValue(DamageAmount, "1.0")
// changed RF063
#pragma GE_DefaultValue(DamageAltAmount, "1.0")
#pragma GE_DefaultValue(DamageAudioEffect, "")
#pragma GE_DefaultValue(DamageAttribute, "")
#pragma GE_DefaultValue(DamageAltAttribute, "")
#pragma GE_Documentation(theModel,"Model to assign modifier to")
#pragma GE_Documentation(ForceImparter, "True if model imparts a force to an actor")
#pragma GE_Documentation(ForceAmount, "Initial amount of force applied")
// changed RF064
#pragma GE_Documentation(ForceVector, "Direction of force")
// end change RF064
#pragma GE_Documentation(ForceDecay,"Amount of force decay (larger is faster)")
#pragma GE_Documentation(ForceAudioEffect,"Audio effect to play when force applied")
#pragma GE_Documentation(ForceTimeDelay,"Delay in sec. before force activates again")
#pragma GE_Documentation(DamageTimeDelay,"Delay in sec. before damage occurs again")
#pragma GE_Documentation(DamageGenerator,"True if contact with the model causes damage")
#pragma GE_Documentation(DamageAmount,"Amount of damage caused by contact with model")
// changed RF063
#pragma GE_Documentation(DamageAltAmount,"Amount of alternate damage caused by contact with model")
#pragma GE_Documentation(DamageAudioEffect,"Audio effect to play when damage applied")
#pragma GE_Documentation(DamageAttribute,"Attribute to damage")
#pragma GE_Documentation(DamageAltAttribute,"Alternate Attribute to damage when Attribute is 0")
} ModelStateModifier;


//	Model Attributes
//
//	Due to some sickness down inside G3D, brush contents are NOT returned when the
//	..brush is NOT a solid model.  To get around this little problem (EVIL HACK)
//	..I've added the ModelAttributes entity.

#pragma GE_Type("..\\icons\\wm_modstat.bmp")
typedef struct _ModelAttributes
{
#pragma GE_Published
	geVec3d			origin;			// Origin of entity
	geWorld_Model	*theModel;		// Model to assign modifiers to
	geBoolean		Water;			// Model is a "water" surface
	geBoolean		Lava;			// Model is a "lava" surface
	geBoolean		Frozen;			// Model is a "frozen" surface
	geBoolean		Sludge;			// Model is a "sludge" surface
// changed RF063
	geBoolean		UnClimbable;	// Model is unclimbable
	geBoolean		Ladder;			// Model is a "ladder"
	geBoolean		Impassible;
	geBoolean		ZeroG;
	geBoolean		SlowMotion;
	geBoolean		FastMotion;
	geBoolean		ToxicGas;
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(Water,"False")
#pragma GE_DefaultValue(Lava,"False")
#pragma GE_DefaultValue(Frozen,"False")
#pragma GE_DefaultValue(Sludge,"False")
// changed RF063
#pragma GE_DefaultValue(UnClimbable, "False")
#pragma GE_DefaultValue(Ladder, "False")
#pragma GE_DefaultValue(Impassible, "False")
#pragma GE_DefaultValue(ZeroG,"False")
#pragma GE_DefaultValue(SlowMotion, "False")
#pragma GE_DefaultValue(FastMotion, "False")
#pragma GE_DefaultValue(ToxicGas, "False")

#pragma GE_Documentation(Water,"TRUE if surface is watery")
#pragma GE_Documentation(Lava, "TRUE if surface is lava")
#pragma GE_Documentation(Frozen,"TRUE if surface is frozen")
#pragma GE_Documentation(Sludge,"TRUE if surface is sludge")
// changed RF063
#pragma GE_Documentation(UnClimbable,"TRUE if surface can not be climbed on")
#pragma GE_Documentation(Ladder, "TRUE if model can be used as a ladder")
#pragma GE_Documentation(Impassible,"TRUE if surface is impassible")
#pragma GE_Documentation(ZeroG,"TRUE if model is zero gravity")
#pragma GE_Documentation(SlowMotion,"TRUE if surface causes slow motion")
#pragma GE_Documentation(FastMotion, "TRUE if surface causes fast motion")
#pragma GE_Documentation(ToxicGas,"TRUE if model is toxic gas")
} ModelAttributes;

//
// Attribute entity
//

#pragma GE_Type("..\\icons\\attribute.bmp")
typedef struct _Attribute
{
#pragma GE_Private
	geBoolean		active;
	geBoolean		alive;
	geFloat			Tick;
	geActor			*Actor;				// Actor for this entity
	geBoolean		CallBack;
	int				CallBackCount;
	geVec3d			OriginOffset;		// Translation from model center
	geBoolean		bState;
// changed QD 06/26/04
	geBitmap		*Bitmap;
// end change
#pragma GE_Published
	geVec3d			origin;
	char			*szEntityName;		// Name of this entity
	char 			*EntityName;		// Name of entity to attach to
	geWorld_Model	*Model;				// Name of model to attach to
	char 			*BoneName;			// Name of actor bone to attach to
	char			*TriggerName;
	char			*szSoundFile;		// Audio that is the sound
	char			*szReSpawnSound;
	char			*szActorName;
	geVec3d			ActorRotation;		// Rotation needed to properly orient actor
	geFloat			Delay;
	geBoolean		ReSpawn;
	geBoolean		Gravity;
	geFloat			Scale;
	char			*AttributeName;		// Name of attribute to update on collision, if any
	int				AttributeAmount;	// Amount to modify attribute by
	char			*AttributeAltName;
	int				AttributeAltAmount;
	GE_RGBA			FillColor;
	GE_RGBA			AmbientColor;
// changed QD 07/21/04
	geBoolean		AmbientLightFromFloor;
// end change
	geFloat			ShadowSize;
// changed QD 06/26/04
	geFloat			ShadowAlpha;
	char			*ShadowBitmap;
	char			*ShadowAlphamap;
// end change
// changed QD Shadows
	geBoolean		UseProjectedShadows;
	geBoolean		UseStencilShadows;
// end change
// changed RF063
	geBoolean		PlayerOnly;
// changed RF064
	geBoolean		HideFromRadar;
	char			*ChangeMaterial;
	geBoolean		EnvironmentMapping;
	geBoolean		AllMaterial;
	geFloat			PercentMapping;
	geFloat			PercentMaterial;
// end change RF064
// changed QD 08/13/03
	geBoolean		UseKey;
// end change 08/13/03
// changed QD 12/15/05
	geBoolean		PowerUp;
	geBoolean		SetMaximumAmount;
// end change
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(szEntityName, "")
#pragma GE_DefaultValue(EntityName, "")
#pragma GE_DefaultValue(BoneName, "")
#pragma GE_DefaultValue(TriggerName, "")
#pragma GE_DefaultValue(szSoundFile, "")
#pragma GE_DefaultValue(szReSpawnSound, "")
#pragma GE_DefaultValue(szActorName, "skull.act")
#pragma GE_DefaultValue(ActorRotation, "-90 180 0")
#pragma GE_DefaultValue(Delay, "0.0")
#pragma GE_DefaultValue(ReSpawn, "False")
#pragma GE_DefaultValue(Gravity, "False")
#pragma GE_DefaultValue(Scale, "1.0")
#pragma GE_DefaultValue(AttributeName, "health")
#pragma GE_DefaultValue(AttributeAmount, "1")
#pragma GE_DefaultValue(AttributeAltName, "")
#pragma GE_DefaultValue(AttributeAltAmount, "0")
#pragma GE_DefaultValue(FillColor, "0 0 0")
#pragma GE_DefaultValue(AmbientColor, "0 0 0")
// changed QD 07/21/04
#pragma GE_DefaultValue(AmbientLightFromFloor, "True")
// end change
#pragma GE_DefaultValue(ShadowSize, "0")
// changed QD 06/26/04
#pragma GE_DefaultValue(ShadowAlpha, "128.0")
#pragma GE_DefaultValue(ShadowBitmap, "")
#pragma GE_DefaultValue(ShadowAlphamap, "")
// end change
// changed QD Shadows
#pragma GE_DefaultValue(UseProjectedShadows, "False")
#pragma GE_DefaultValue(UseStencilShadows, "False")
// end change
// changed RF063
#pragma GE_DefaultValue(PlayerOnly, "False")
// changed RF064
#pragma GE_DefaultValue(HideFromRadar, "False")
#pragma GE_DefaultValue(ChangeMaterial, "")
#pragma GE_DefaultValue(EnvironmentMapping, "False")
#pragma GE_DefaultValue(AllMaterial, "False")
#pragma GE_DefaultValue(PercentMapping, "100")
#pragma GE_DefaultValue(PercentMaterial, "100")
// end change RF064
// changed QD 08/13/03
#pragma GE_DefaultValue(UseKey, "False")
// end change 08/13/03
// changed QD 12/15/05
#pragma GE_DefaultValue(PowerUp, "False")
#pragma GE_DefaultValue(SetMaximumAmount, "False")
// end change
#pragma GE_Documentation(szEntityName, "Name of entity (used in scripting and triggers)")
#pragma GE_Documentation(EntityName, "Name of entity to attach to")
#pragma GE_Documentation(Model, "Name of model to attach to")
#pragma GE_Documentation(BoneName, "Name of actor bone to attach to")
#pragma GE_Documentation(TriggerName, "Name of trigger entity to use")
#pragma GE_Documentation(szSoundFile, "Audio loop to play on pickup")
#pragma GE_Documentation(szReSpawnSound, "Audio loop to play on respawn")
#pragma GE_Documentation(szActorName, "Name of actor to use")
#pragma GE_Documentation(FillColor, "Color of actor fill lighting")
#pragma GE_Documentation(AmbientColor, "Color of actor ambient lighting")
// changed QD 07/21/04
#pragma GE_Documentation(AmbientLightFromFloor, "Calculate ambient lighting instead of using AmbientColor")
// end change
#pragma GE_Documentation(ActorRotation, "Rotation needed to properly orient actor in game")
#pragma GE_Documentation(Delay, "Number of seconds before respawning")
#pragma GE_Documentation(ReSpawn, "Is attribute respawnable?")
#pragma GE_Documentation(Gravity, "Is attribute actor affected by gravity?")
#pragma GE_Documentation(Scale, "Scale of actor")
#pragma GE_Documentation(AttributeName,"Name of actor attribute to modify, if any")
#pragma GE_Documentation(AttributeAmount,"Amount to modify actor attribute by")
#pragma GE_Documentation(AttributeAltName,"Name of alternate actor attribute to modify, if any")
#pragma GE_Documentation(AttributeAltAmount,"Amount to modify alternate actor attribute by")
#pragma GE_Documentation(ShadowSize, "Radius of actor shadow, 0 is none")
// changed QD 06/26/04
#pragma GE_Documentation(ShadowAlpha, "Transparency of actor shadow (0 to 255)")
#pragma GE_Documentation(ShadowBitmap, "name of bitmap to use for shadow")
#pragma GE_Documentation(ShadowAlphamap, "name of alpha bitmap to use for shadow")
// end change
// changed QD Shadows
#pragma GE_Documentation(UseProjectedShadows, "use projected shadows")  //Projected Shadows
#pragma GE_Documentation(UseStencilShadows, "use stencil shadowsr")
// end change
// changed RF063
#pragma GE_Documentation(PlayerOnly, "True if only player can collect")
// changed RF064
#pragma GE_Documentation(HideFromRadar, "if true then does not show on radar")
#pragma GE_Documentation(ChangeMaterial, "name of section that contains material change info")
#pragma GE_Documentation(EnvironmentMapping, "if true then use environmental mapping")
#pragma GE_Documentation(AllMaterial, "if true apply to all material")
#pragma GE_Documentation(PercentMapping, "percentage of mapping to apply")
#pragma GE_Documentation(PercentMaterial, "percentage of material to use")
// end change RF064
// changed QD 12/15/05
#pragma GE_Documentation(PowerUp, "Increase upper limit of AttributeName by AttributeAmount")
#pragma GE_Documentation(SetMaximumAmount, "Set amount of AttributeName or AttributeAltName to maximum")
// end change
} Attribute;

// FlipBook

#pragma GE_Type("..\\icons\\flipbook.bmp")
typedef struct FlipBook
{
#pragma	GE_Private
	int			EffectList;
	geBitmap	**Bitmap;
	geBitmap	*WorldBitmap;
	geBoolean	active;
	geVec3d		OriginOffset;		// Translation from model center
	geFloat		Time;
	int			CurTex;
	int			CycleDir;
	geBoolean	changed;
#pragma GE_Published
	geVec3d			origin;
	char			*szEntityName;
	GE_RGBA			Color;
	geFloat			Scale;
	int				BitmapCount;
	int				Style;
	geFloat			Speed;
	geFloat			ScaleRate;		// how much to subtract from scale each second
	geFloat			RotationRate;	// how much to add to art rotation each second
	geFloat			Alpha;
	geFloat			AlphaRate;		// how much to subtract from alpha each second
	geFloat			Rotation;		// art rotation amount
	char			*BmpNameBase;
	char			*AlphaNameBase;
	char			*TextureName;
	char 			*EntityName;	// Name of entity to attach to
	geWorld_Model	*Model;			// Name of model to attach to
	char 			*BoneName;		// Name of actor bone to attach to
	char 			*TriggerName;	// Name of trigger entity
	geBoolean		AtLevelStart;
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
#pragma GE_DefaultValue(AtLevelStart, "False")
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
#pragma GE_Documentation(AtLevelStart, "change texture only once at level start")
} FlipBook;


//
// Explosion entity
//

#pragma GE_Type("..\\icons\\model.ico")
typedef struct _Explosion
{
#pragma GE_Private
	geBoolean		active;
	geVec3d			OriginOffset;	// Translation from model center
#pragma GE_Published
	geVec3d			origin;
	char 			*EntityName;	// Name of entity to attach to
	geWorld_Model	*Model;			// Name of model to attach to
	char 			*BoneName;		// Name of actor bone to attach to
	char			*TriggerName;
	char			*ExplosionName;
	char			*DamageAttribute;
	geFloat			DamageAmt;
// changed RF063
	char			*DamageAltAttribute;
	geFloat			DamageAltAmt;
// end change RF063
	geFloat			Radius;
	geFloat			ShakeAmt;
	geFloat			ShakeDecay;
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(ExplosionName, "")
#pragma GE_DefaultValue(EntityName, "")
#pragma GE_DefaultValue(BoneName, "")
#pragma GE_DefaultValue(TriggerName, "")
#pragma GE_DefaultValue(DamageAttribute, "")
#pragma GE_DefaultValue(DamageAmt, "0")
// changed RF063
#pragma GE_DefaultValue(DamageAltAttribute, "")
#pragma GE_DefaultValue(DamageAltAmt, "0")
// end change RF063
#pragma GE_DefaultValue(Radius, "0")
#pragma GE_DefaultValue(ShakeAmt, "0")
#pragma GE_DefaultValue(ShakeDecay, "0")
#pragma GE_Documentation(ExplosionName, "Name of explosion to use")
#pragma GE_Documentation(EntityName, "Name of entity to attach to")
#pragma GE_Documentation(Model, "Name of model to attach to")
#pragma GE_Documentation(BoneName, "Name of actor bone to attach to")
#pragma GE_Documentation(TriggerName, "Name of trigger entity to use")
#pragma GE_Documentation(DamageAttribute, "Name of attribute to damage")
#pragma GE_Documentation(DamageAmt, "Amount of damage done")
// changed RF063
#pragma GE_Documentation(DamageAltAttribute, "Name of alternate attribute to damage")
#pragma GE_Documentation(DamageAltAmt, "Amount of alternate damage done")
// end change RF063
#pragma GE_Documentation(Radius, "Radius of explosion damage")
#pragma GE_Documentation(ShakeAmt, "Amount of screen shake caused by explosion")
#pragma GE_Documentation(ShakeDecay, "Speed of decay of shaking, higher is faster")
} Explosion;


//
// Shake entity
//

#pragma GE_Type("..\\icons\\screenshake.bmp")
typedef struct _ScreenShake
{
#pragma GE_Private
	geBoolean		active;
	geVec3d			OriginOffset;	// Translation from model center
#pragma GE_Published
	geVec3d			origin;
	char 			*EntityName;	// Name of entity to attach to
	geWorld_Model	*Model;			// Name of model to attach to
	char			*BoneName;		// Name of actor bone to attach to
	char			*TriggerName;
	geFloat			ShakeAmt;
	geFloat			ShakeDecay;
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(EntityName, "")
#pragma GE_DefaultValue(BoneName, "")
#pragma GE_DefaultValue(TriggerName, "")
#pragma GE_DefaultValue(ShakeAmt, "0")
#pragma GE_DefaultValue(ShakeDecay, "0")
#pragma GE_Documentation(EntityName, "Name of entity to attach to")
#pragma GE_Documentation(Model, "Name of model to attach to")
#pragma GE_Documentation(BoneName, "Name of actor bone to attach to")
#pragma GE_Documentation(TriggerName, "Name of trigger entity to use")
#pragma GE_Documentation(ShakeAmt, "Amount of screen shake caused")
#pragma GE_Documentation(ShakeDecay, "Speed of decay of shaking, higher is faster")
} ScreenShake;

//	Possible brush contents

#pragma GE_BrushContents
typedef enum
{
	Water			= 0x00010000,
	Lava			= 0x00020000,
	ToxicGas		= 0x00040000,
	ZeroG			= 0x00080000,
	Frozen			= 0x00100000,
	Sludge			= 0x00200000,
	SlowMotion		= 0x00400000,
	FastMotion		= 0x00800000,
	Ladder			= 0x01000000,
	Unclimbable		= 0x02000000,
	UserSurface1	= 0x04000000,
	UserSurface2	= 0x08000000,
	Impenetrable	= 0x10000000
//	typea13			= 0x10000000,
//	typea14			= 0x20000000,
//	typea15			= 0x40000000,
//	typea16			= 0x80000000
} UserContentsEnum;

//
// FixedCamera
//

#pragma GE_Type("..\\icons\\fixedcamera.bmp")
typedef struct _FixedCamera
{
#pragma GE_Private
	geVec3d			Rotation;
	geVec3d			OriginOffset;	// Translation from model center
	geVec3d			RealAngle;
#pragma GE_Published
	geVec3d			origin;
	geVec3d			Angle;
	geFloat			FieldofView;
// changed QD 12/15/05
	geFloat			FOVCheckRange;
// end change
	char			*TriggerName;
	char			*ForceTrigger;
// changed RF064
	geBoolean		UseFirst;
	char 			*EntityName;	// Name of entity to attach to
	geWorld_Model	*Model;			// Name of model to attach to
	char 			*BoneName;		// Name of actor bone to attach to
	geBoolean		AngleRotation;
// end change RF064
#pragma GE_Origin(origin)
#pragma GE_Angles(Angle)
#pragma GE_DefaultValue(FieldofView, "2")
// changed QD 12/15/05
#pragma GE_DefaultValue(FOVCheckRange, "100")
// end change
#pragma GE_DefaultValue(TriggerName, "")
#pragma GE_DefaultValue(ForceTrigger, "")
// changed RF064
#pragma GE_DefaultValue(UseFirst, "False")
#pragma GE_DefaultValue(EntityName, "")
#pragma GE_DefaultValue(BoneName, "")
#pragma GE_DefaultValue(AngleRotation, "False")
// end change RF064
#pragma GE_Documentation(FieldofView, "Camera field of view")
#pragma GE_Documentation(FOVCheckRange, "Percentage of FOV to use for Player visibility checking (0 to 100)")
#pragma GE_Documentation(TriggerName, "Name of trigger entity")
#pragma GE_Documentation(ForceTrigger, "Name of trigger entity that forces camera to activate")
// changed RF064
#pragma GE_Documentation(UseFirst, "if true then use this as the first camera in the level")
#pragma GE_Documentation(EntityName, "Name of entity to attach to")
#pragma GE_Documentation(Model, "Name of model to attach to")
#pragma GE_Documentation(BoneName, "Name of actor bone to attach to")
#pragma GE_Documentation(AngleRotation, "if False then use rotation from actor")
// end change RF064
} FixedCamera;

//
// View Switch
//

#pragma GE_Type("..\\icons\\viewswitch.bmp")
typedef struct _ViewSwitch
{
#pragma GE_Private

#pragma GE_Published
	geVec3d		origin;
	int			LevelView;
	char		*TriggerName;
	geBoolean	Restore;
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(LevelView, "0")
#pragma GE_DefaultValue(TriggerName, "")
#pragma GE_DefaultValue(Restore, "False")
#pragma GE_Documentation(LevelView, "View type to switch to")
#pragma GE_Documentation(TriggerName, "Name of trigger entity")
#pragma GE_Documentation(Restore, "Restore old view after trigger goes off")
} ViewSwitch;


// changed RF063
//
// Liquid
//

#pragma GE_Type("..\\icons\\liquid.bmp")
typedef struct _Liquid
{
#pragma GE_Private
	geBitmap		*Texture;
#pragma GE_Published
	geVec3d			origin;
	char			*szEntityName;
	geWorld_Model	*Model;			// Name of model to attach to
	int				GravityCoeff;
	int				SpeedCoeff;
	GE_RGBA			TintColor;
	geFloat			Transparency;
	char			*Bitmap;
	char			*Alphamap;
	char			*DamageAttr;
	int				DamageAmt;
	geFloat			DamageDelay;
	char			*DamageAltAttr;
	int				DamageAltAmt;
	geFloat			DamageAltDelay;
	geBoolean		DamageIn;
	char			*SwimSound;
	char			*SurfaceSound;
	char			*InLiquidSound;
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(szEntityName, "")
#pragma GE_DefaultValue(GravityCoeff, "100")
#pragma GE_DefaultValue(SpeedCoeff, "100")
#pragma GE_DefaultValue(TintColor, "0 0 64")
#pragma GE_DefaultValue(Transparency, "64")
#pragma GE_DefaultValue(Bitmap, "water.bmp")
#pragma GE_DefaultValue(Alphamap, "a_water.bmp")
#pragma GE_DefaultValue(DamageAttr, "")
#pragma GE_DefaultValue(DamageAmt, "0")
#pragma GE_DefaultValue(DamageDelay, "0")
#pragma GE_DefaultValue(DamageAltAttr, "")
#pragma GE_DefaultValue(DamageAltAmt, "0")
#pragma GE_DefaultValue(DamageAltDelay, "0")
#pragma GE_DefaultValue(DamageIn, "False")
#pragma GE_DefaultValue(SwimSound, "")
#pragma GE_DefaultValue(SurfaceSound, "")
#pragma GE_DefaultValue(InLiquidSound, "")

#pragma GE_Documentation(szEntityName, "Name of entity")
#pragma GE_Documentation(Model, "Name of model to attach to")
#pragma GE_Documentation(GravityCoeff, "Percentage of gravity when in liquid (0 to 100)")
#pragma GE_Documentation(SpeedCoeff, "Percentage of speed when in liquid (0 to 100)")
#pragma GE_Documentation(TintColor, "Tint color of water")
#pragma GE_Documentation(Transparency, "Transparency of tint of water (0 to 255)")
#pragma GE_Documentation(Bitmap, "Name of bitmap used for tinting")
#pragma GE_Documentation(Alphamap, "Name of alpha map used for tinting")
#pragma GE_Documentation(DamageAttr, "Attribute that is damaged")
#pragma GE_Documentation(DamageAmt, "Amount of damage done")
#pragma GE_Documentation(DamageDelay, "Delay before more damage is done")
#pragma GE_Documentation(DamageAltAttr, "Alternate attribute that is damaged")
#pragma GE_Documentation(DamageAltAmt, "Amount of alternate damage done")
#pragma GE_Documentation(DamageAltDelay, "Delay before more alternate damage is done")
#pragma GE_Documentation(DamageIn, "if True then damage by being in liquid")
#pragma GE_Documentation(SwimSound, "Name of sound played when moving in liquid")
#pragma GE_Documentation(SurfaceSound, "Name of sound played when on surface in liquid")
#pragma GE_Documentation(InLiquidSound, "Name of sound played when walking in liquid")
} Liquid;

//
// ModifyAttribute
//

#pragma GE_Type("..\\icons\\modifyattrib.bmp")
typedef struct _ModifyAttribute
{
#pragma GE_Private
	geBoolean	active;
#pragma GE_Published
	geVec3d		origin;
	char		*Attribute;
	int			Amount;
	char		*TriggerName;
// changed QD 12/15/05
	geBoolean		PowerUp;
	geBoolean		SetMaximumAmount;
// end change
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(Attribute, "")
#pragma GE_DefaultValue(Amount, "0")
#pragma GE_DefaultValue(TriggerName, "")
// changed QD 12/15/05
#pragma GE_DefaultValue(PowerUp, "False")
#pragma GE_DefaultValue(SetMaximumAmount, "False")
// end change
#pragma GE_Documentation(Attribute, "Name of attribute to modify")
#pragma GE_Documentation(Amount, "Amount to change by")
#pragma GE_Documentation(TriggerName, "Name of trigger entity")
// changed QD 12/15/05
#pragma GE_Documentation(PowerUp, "Increase upper limit of AttributeName by AttributeAmount")
#pragma GE_Documentation(SetMaximumAmount, "Set amount of AttributeName or AttributeAltName to maximum")
// end change
} ModifyAttribute;
// end change RF063

// changed RF064
//
// ScriptPoint
//

#pragma GE_Type("..\\icons\\model.ico")
typedef struct _ScriptPoint
{
#pragma GE_Private

#pragma GE_Published
	geVec3d	origin;
	char	*szEntityName;
	char	*NextOrder;
	char	*NextPoint;
	geVec3d	Angle;
#pragma GE_Origin(origin)
#pragma GE_Angles(Angle)
#pragma GE_DefaultValue(szEntityName, "")
#pragma GE_DefaultValue(NextOrder, "")
#pragma GE_DefaultValue(NextPoint, "")
#pragma GE_Documentation(szEntityName, "Name of script point")
#pragma GE_Documentation(NextOrder, "Name of next script order to execute")
#pragma GE_Documentation(NextPoint, "Name of next script point in path")
} ScriptPoint;

//
// Pawn
//

#pragma GE_Type("..\\icons\\ai_nonplayer.bmp")
typedef struct _Pawn
{
#pragma GE_Private
	void		*Data;
	void		*Converse;
#pragma GE_Published
	geVec3d		origin;
	geVec3d		Angle;
	char		*szEntityName;	// Name of this entity
	char		*PawnType;
	char		*ScriptName;
	char		*ConvScriptName;
	char		*ConvOrder;
	char		*SpawnOrder;
	char		*SpawnPoint;
	char		*SpawnTrigger;
	geBoolean	HideFromRadar;
	char		*ChangeMaterial;
#pragma GE_Origin(origin)
#pragma GE_Angles(Angle)
#pragma GE_DefaultValue(szEntityName, "")
#pragma GE_DefaultValue(PawnType, "")
#pragma GE_DefaultValue(ScriptName, "")
#pragma GE_DefaultValue(ConvScriptName, "")
#pragma GE_DefaultValue(ConvOrder, "")
#pragma GE_DefaultValue(SpawnOrder, "")
#pragma GE_DefaultValue(SpawnPoint, "")
#pragma GE_DefaultValue(SpawnTrigger, "")
#pragma GE_DefaultValue(HideFromRadar, "False")
#pragma GE_DefaultValue(ChangeMaterial, "")
#pragma GE_Documentation(szEntityName, "Name of entity (used in scripting and triggers)")
#pragma GE_Documentation(PawnType, "Name of base Pawn definition")
#pragma GE_Documentation(ScriptName, "Name of Action Script file to use")
#pragma GE_Documentation(ConvScriptName, "Name of Conversation Script file to use")
#pragma GE_Documentation(ConvOrder, "Name of initial Conversation order to run")
#pragma GE_Documentation(SpawnOrder, "Name of initial Action order to run")
#pragma GE_Documentation(SpawnPoint, "Name of first point in Pawn path")
#pragma GE_Documentation(SpawnTrigger, "Name of trigger that will spawn entity")
#pragma GE_Documentation(HideFromRadar, "if true then does not show on radar")
#pragma GE_Documentation(ChangeMaterial, "name of section containing material change info")
} Pawn;

//
// CountDownTimer
//

#pragma GE_Type("..\\icons\\model.ico")
typedef struct _CountDownTimer
{
#pragma GE_Private
	geBoolean	bState;
	geBoolean	OldState;
	geBoolean	active;
	geBoolean	alive;
	geFloat		Time;
	int			index;
#pragma GE_Published
	geVec3d		origin;
	char		*szEntityName;
	char		*TriggerName;
	char		*Attribute;
	char		*SoundFile;
	geFloat		TimeAmt;
	char		*StopTrigger;
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(szEntityName, "")
#pragma GE_DefaultValue(TriggerName, "")
#pragma GE_DefaultValue(Attribute, "countdown")
#pragma GE_DefaultValue(TimeAmt, "0")
#pragma GE_DefaultValue(SoundFile, "")
#pragma GE_DefaultValue(StopTrigger, "")
#pragma GE_Documentation(szEntityName, "Name of script point")
#pragma GE_Documentation(TriggerName, "Name of trigger used to activate timer")
#pragma GE_Documentation(Attribute, "Name of attribute that is changed by timer")
#pragma GE_Documentation(TimeAmt, "Number of seconds that timer counts down")
#pragma GE_Documentation(SoundFile, "Name of sound file to play during countdown")
#pragma GE_Documentation(StopTrigger, "Name of trigger used to deactivate timer")
} CountDownTimer;

//
// ChangeAttribute
//

#pragma GE_Type("..\\icons\\model.ico")
typedef struct _ChangeAttribute
{
#pragma GE_Private
	geBoolean	active;
	geFloat		Count;
	geFloat		Increment;
	geFloat		Total;
	geBoolean	OldState;
#pragma GE_Published
	geVec3d		origin;
	char		*Attribute;
	geBoolean	Percentage;
	int			Amount;
	geFloat		Time;
	char		*StartTrigger;
	char		*StopTrigger;
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(Attribute, "")
#pragma GE_DefaultValue(Percentage, "True")
#pragma GE_DefaultValue(Amount, "-100")
#pragma GE_DefaultValue(StartTrigger, "")
#pragma GE_DefaultValue(StopTrigger, "")
#pragma GE_DefaultValue(Time, "10")
#pragma GE_Documentation(Attribute, "Name of attribute to change")
#pragma GE_Documentation(Percentage, "if true then Amount is percentage of Attribute")
#pragma GE_Documentation(StartTrigger, "Name of trigger entity to start change")
#pragma GE_Documentation(Amount, "Amount of change, negative is decreasing")
#pragma GE_Documentation(StopTrigger, "Name of trigger entity to stop change")
#pragma GE_Documentation(Time, "Number of seconds that change takes place over")
} ChangeAttribute;

//
// Overlay
//

#pragma GE_Type("..\\icons\\overlay.bmp")
typedef struct _Overlay
{
#pragma GE_Private
	geBitmap		*Texture;
	geBitmap		**FBitmap;
	geFloat			Time;
	int				CurTex;
	int				CycleDir;
	geFloat			Alpha;
#pragma GE_Published
	geVec3d			origin;
	char			*szEntityName;
	geWorld_Model	*Model;			// Name of model to attach to
	GE_RGBA			TintColor;
	geFloat			Transparency;
	geFloat			AlphaRate;
	char			*Bitmap;
	char			*Alphamap;
	geBoolean		Animated;
	int				BitmapCount;
	int				Style;
	geFloat			Speed;
	char			*TriggerName;
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(szEntityName, "")
#pragma GE_DefaultValue(TintColor, "0 0 64")
#pragma GE_DefaultValue(Transparency, "64")
#pragma GE_DefaultValue(Bitmap, "water.bmp")
#pragma GE_DefaultValue(Alphamap, "a_water.bmp")
#pragma GE_DefaultValue(Animated, "False")
#pragma GE_DefaultValue(BitmapCount, "1")
#pragma GE_DefaultValue(Style, "0")
#pragma GE_DefaultValue(Speed, "10")
#pragma GE_DefaultValue(TriggerName, "")
#pragma GE_DefaultValue(AlphaRate, "0")

#pragma GE_Documentation(szEntityName, "Name of entity")
#pragma GE_Documentation(Model, "Name of model to attach to")
#pragma GE_Documentation(TintColor, "Tint color of overlay")
#pragma GE_Documentation(Transparency, "Transparency of tint of overlay (0 to 255)")
#pragma GE_Documentation(Bitmap, "Name of bitmap used for tinting")
#pragma GE_Documentation(Alphamap, "Name of alpha map used for tinting")
#pragma GE_Documentation(Animated, "true if bitmap is animated")
#pragma GE_Documentation(BitmapCount, "number of bitmaps in sequence")
#pragma GE_Documentation(Style, "Animation style 0 to 3")
#pragma GE_Documentation(Speed, "# of bitmaps per second")
#pragma GE_Documentation(TriggerName, "Name of trigger used to activate overlay")
#pragma GE_Documentation(AlphaRate, "Amount to decrease alpha per second")
} Overlay;


//	Spout Entity

#pragma GE_Type("..\\icons\\sf_spout.bmp")
typedef struct ActorSpout
{
#pragma	GE_Private
	int			EffectList;
	int			EffectCount;
	geFloat		PauseTime;
	geBoolean 	PauseState;
	geBoolean	active;
	geVec3d		OriginOffset;	// Translation from model center
#pragma GE_Published
	geVec3d		origin;
	char		*szEntityName;
	geVec3d		Angles;
	geFloat		ParticleCreateRate;
	geFloat		MinScale;
	geFloat		MaxScale;
	geFloat		MinSpeed;
	geFloat		MaxSpeed;
	geFloat		MinUnitLife;
	geFloat		MaxUnitLife;
	int			SourceVariance;
	int			DestVariance;
	GE_RGBA		FillColor;
	GE_RGBA		AmbientColor;
// changed QD 07/21/04
	geBoolean	AmbientLightFromFloor;
// end change
	geBoolean	Gravity;
	char		*BaseName;
	int			NumActors;
	int			Style;
	geFloat		Alpha;
	geFloat		AlphaRate;
	geVec3d		BaseRotation;
	geVec3d		MinRotationSpeed;
	geVec3d		MaxRotationSpeed;
	geFloat		MinPauseTime;
	geFloat		MaxPauseTime;
	geFloat		TotalLife;
	geBoolean	Bounce;
	geBoolean	Solid;
	char 		*EntityName;	// Name of entity to attach to
	geWorld_Model *Model;		// Name of model to attach to
	char 		*BoneName;		// Name of actor bone to attach to
	char 		*TriggerName;	// Name of trigger entity
	geBoolean	EnvironmentMapping;
	geBoolean	AllMaterial;
	geFloat		PercentMapping;
	geFloat		PercentMaterial;
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
#pragma GE_DefaultValue( FillColor, "255.0 255.0 255.0" )
#pragma GE_DefaultValue( AmbientColor, "255.0 255.0 255.0" )
// changed QD 07/21/04
#pragma GE_DefaultValue( AmbientLightFromFloor, "True")
// end change
#pragma GE_DefaultValue( MinRotationSpeed, "0 0 0" )
#pragma GE_DefaultValue( MaxRotationSpeed, "0 0 0" )
#pragma GE_DefaultValue( BaseName, "")
#pragma GE_DefaultValue( NumActors, "0")
#pragma GE_DefaultValue( Style, "0")
#pragma GE_DefaultValue( Alpha, "255")
#pragma GE_DefaultValue( AlphaRate, "0")
#pragma GE_DefaultValue( Gravity, "True" )
#pragma GE_DefaultValue( TotalLife, "0.0" )
#pragma GE_DefaultValue( MinPauseTime, "0.0" )
#pragma GE_DefaultValue( MaxPauseTime, "0.0" )
#pragma GE_DefaultValue( EntityName, "")
#pragma GE_DefaultValue( BoneName, "")
#pragma GE_DefaultValue( TriggerName, "")
#pragma GE_DefaultValue( Bounce, "False")
#pragma GE_DefaultValue( Solid, "False")
#pragma GE_DefaultValue( EnvironmentMapping, "False")
#pragma GE_DefaultValue( AllMaterial, "False")
#pragma GE_DefaultValue( PercentMapping, "100" )
#pragma GE_DefaultValue( PercentMaterial, "100" )
#pragma GE_Documentation( szEntityName, "Name of this entity, if any")
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
#pragma GE_Documentation( FillColor, "Lighting fill color" )
#pragma GE_Documentation( AmbientColor, "Lighting ambient color" )
// changed QD 07/21/04
#pragma GE_Documentation(AmbientLightFromFloor, "Calculate ambient lighting instead of using AmbientColor")
// end change
#pragma GE_Documentation(Gravity, "true if affected by gravity" )
#pragma GE_Documentation(BaseName, "Base actor name" )
#pragma GE_Documentation(TotalLife, "How many seconds this spout lasts. Set to 0 for continuous." )
#pragma GE_Documentation(MinPauseTime, "Low range of randomly chosen pause time (seconds)" )
#pragma GE_Documentation(MaxPauseTime, "High range of randomly chosen pause time (seconds)" )
#pragma GE_Documentation(EntityName, "Name of entity to attach to")
#pragma GE_Documentation(Bounce, "True if particles bounce when they hit something")
#pragma GE_Documentation(Solid, "True if particles are solid")
#pragma GE_Documentation(Model, "Name of model to attach to")
#pragma GE_Documentation(BoneName, "Name of actor bone to attach to")
#pragma GE_Documentation(TriggerName, "Name of trigger entity to use")
#pragma GE_Documentation(EnvironmentMapping, "if true then use environmental mapping")
#pragma GE_Documentation(AllMaterial, "if true apply to all material")
#pragma GE_Documentation(PercentMapping, "percentage of mapping to apply")
#pragma GE_Documentation(PercentMaterial, "percentage of material to use")
} ActorSpout;

//
// EM_Morph Entity
//
// added by QD
//

#pragma GE_Type("..\\icons\\model.ico")
typedef struct EM_Morph
{
#pragma	GE_Private
	geBoolean	active;
	geBitmap	*CStartBmp;
	geBitmap	*CEndBmp;
	geBitmap	*CMorphBmp;
	geActor		*Actor;
	int			CurStage;
	geFloat		CurDelayToNextStage;
	int			StageDir;
	geBoolean	First;
#pragma GE_Published
	geVec3d		origin;
	char		*EntityName;
	char		*StartBmp;
	char		*EndBmp;
	int			MorphStages;
	geFloat		DelayToNextStage;
	char		*BitmapToAttachTo;
	geBoolean	Loop;
	char		*szEntityName;
	char		*TriggerName;
#pragma GE_Origin(origin)
#pragma GE_DefaultValue( MorphStages, "8" )
#pragma GE_DefaultValue( DelayToNextStage, "1.0" )
#pragma GE_DefaultValue( Loop, "True" )
#pragma GE_DefaultValue( TriggerName, "" )
#pragma GE_DefaultValue( EntityName, "" )

#pragma GE_Documentation( origin, "Location of effect" )
#pragma GE_Documentation( EntityName, "Entity whose actor texture the effect will attach to (optional)" )
#pragma GE_Documentation( MorphStages, "How many different stages of morph there will be" )
#pragma GE_Documentation( DelayToNextStage, "How many seconds to wait before going to next stage" )
#pragma GE_Documentation( BitmapToAttachTo, "Which world or actor bitmap the effect will attach to" )
#pragma GE_Documentation( Loop, "Whether or not to keep looping the morph animation" )
#pragma GE_Documentation( TriggerName, "Name of the associated trigger" )

} EM_Morph;

//
// CutScene entity
//
// provided by QD

#pragma GE_Type("..\\icons\\model.ico")
typedef struct CutScene
{
#pragma GE_Private
	geBoolean	active;
	geBoolean	played;
	geBoolean	triggeron;
#pragma GE_Published
	geVec3d		origin;
	char		*szCutScene;
	char		*szEntityName;
	char		*TriggerName;
	int			XPos;
	int			YPos;
	geBoolean	Center;
#pragma GE_Origin(origin)
#pragma GE_DefaultValue( TriggerName, "" )
#pragma GE_DefaultValue( szEntityName, "" )
#pragma GE_DefaultValue( Center, "True")
#pragma GE_DefaultValue( XPos, "160")
#pragma GE_DefaultValue( YPos, "120")

#pragma GE_Documentation( szEntityName, "Name of this entity" )
#pragma GE_Documentation( TriggerName, "Name of the associated trigger" )

} CutScene;

//
// ActorMaterial Change
//
// provided by QD

#pragma GE_Type("..\\icons\\model.ico")
typedef struct ActMaterial
{
#pragma GE_Private
	geBoolean	active;
	geActor		*Actor;
	geFloat		Time;
	int			CurMat;
	int			CycleDir;
#pragma GE_Published
	geVec3d		origin;
	char		*EntityName;
	char		*szEntityName;
	char		*TriggerName;
	char		*ChangeMaterial;
	geBoolean	ChangeLighting;
	GE_RGBA		FillColor;
	GE_RGBA		AmbientColor;
// changed QD 07/21/04
	geBoolean	AmbientLightFromFloor;
// end change
	int			MaterialCount;
	int			Style;
	geFloat		Speed;
#pragma GE_Origin(origin)
#pragma GE_DefaultValue( TriggerName, "" )
#pragma GE_DefaultValue( szEntityName, "" )
#pragma GE_DefaultValue( FillColor, "0 0 0" )
#pragma GE_DefaultValue( AmbientColor, "0 0 0" )
// changed QD 07/21/04
#pragma GE_DefaultValue(AmbientLightFromFloor, "True" )
// end change
#pragma GE_DefaultValue( ChangeLighting, "false" )
#pragma GE_DefaultValue( MaterialCount, "1" )
#pragma GE_DefaultValue( Style, "0" )
#pragma GE_DefaultValue( Speed, "10.0" )

#pragma GE_Documentation( origin, "Location of effect" )
#pragma GE_Documentation( szEntityName, "Name of this entity" )
#pragma GE_Documentation( TriggerName, "Name of the associated trigger" )
#pragma GE_Documentation( ChangeMaterial, "Name of section containing material change info" )
#pragma GE_Documentation( EntityName, "Entity whose actor texture the effect will attach to" )
#pragma GE_Documentation( FillColor, "Color of actor fill lighting" )
#pragma GE_Documentation( AmbientColor, "Color of actor ambient lighting" )
// changed QD 07/21/04
#pragma GE_Documentation( AmbientLightFromFloor, "Calculate ambient lighting instead of using AmbientColor" )
// end change
#pragma GE_Documentation( ChangeLighting, "if true the change actor lighting on material change" )
#pragma GE_Documentation( MaterialCount, "How many material sections in animation" )
#pragma GE_Documentation( Style, "Animation style 0 to 4" )
#pragma GE_Documentation( Speed, "# of materials per second" )

} ActMaterial;

//
// SkyDome
//

#pragma GE_Type("..\\icons\\model.ico")
typedef struct _SkyDome
{
#pragma GE_Published
	geVec3d		origin;
	char		*ScriptName;
	char		*StartOrder;
	char		*InitOrder;
	geBoolean	Enable;
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(ScriptName, "")
#pragma GE_DefaultValue(StartOrder, "")
#pragma GE_DefaultValue(InitOrder, "")
#pragma GE_DefaultValue(Enable, "True")

#pragma GE_Documentation(ScriptName, "Name of Action Script file to use")
#pragma GE_Documentation(StartOrder, "Name of runtime order")
#pragma GE_Documentation(InitOrder, "Name of initial order to run")
#pragma GE_Documentation(Enable, "if True then enable SkyDome")
} SkyDome;

//
// LiftBelt
//

#pragma GE_Type("..\\icons\\model.ico")
typedef struct _LiftBelt
{
#pragma GE_Private
	geFloat		Powerlevel;
	geBoolean	active;
	geFloat		Fuel;
	geBoolean	bState;
#pragma GE_Published
	geVec3d		origin;
	char		*szEntityName;
	char		*EnableAttr;
	char		*PowerAttr;
	geFloat		LiftForce;
	geFloat		PowerUse;
	geFloat		AccelRate;
	geBoolean	DropFast;
	geBoolean	EnableAlways;
#pragma GE_Origin(origin)
#pragma GE_DefaultValue(EnableAttr, "")
#pragma GE_DefaultValue(PowerAttr, "")
#pragma GE_DefaultValue(LiftForce, "0")
#pragma GE_DefaultValue(PowerUse, "1")
#pragma GE_DefaultValue(AccelRate, "1")
#pragma GE_DefaultValue(szEntityName, "")
#pragma GE_DefaultValue(DropFast, "False")
#pragma GE_DefaultValue(EnableAlways, "False")

#pragma GE_Documentation(EnableAttr, "attribute used to enable LiftBelt")
#pragma GE_Documentation(PowerAttr, "attribute used as fuel")
#pragma GE_Documentation(LiftForce, "maximum amount of up force generated")
#pragma GE_Documentation(PowerUse, "amount of fuel used per second")
#pragma GE_Documentation(AccelRate, "amount of lift force change per second")
#pragma GE_Documentation(szEntityName, "Name of this entity")
#pragma GE_Documentation(DropFast, "if True then drop quickly after force is done")
#pragma GE_Documentation(EnableAlways, "if True then enable without any EnableAttr")
} LiftBelt;

//
// Dynamic Spotlight by QD
//
#pragma GE_Type("..\\icons\\sf_dynlight.bmp")
typedef struct _DSpotLight
{
#pragma	GE_Private
	geFloat			LastTime;
	geFloat			IntervalWidth;
	int				NumFunctionValues;
	geLight			*DynLight;
	geBoolean		active;			// is activated
	geVec3d			OriginOffset;	// Translation from model center
	geVec3d			RealAngle;
#pragma GE_Published
	geVec3d			origin;
	char			*szEntityName;
	int				MinRadius;
	int				MaxRadius;
	geBoolean		InterpolateValues;
	char			*RadiusFunction;
	geFloat			RadiusSpeed;
	GE_RGBA			Color;
	char 			*EntityName;	// Name of entity to attach to
	geWorld_Model	*Model;			// Name of model to attach to
	char 			*BoneName;		// Name of actor bone to attach to
	char 			*TriggerName;	// Name of trigger entity
	geBoolean		CastShadows;
	geBoolean		Shadow;
	geBoolean		Rotate;
	geVec3d			angles;
	geFloat			arc;
	int				style;

#pragma GE_Origin(origin)
#pragma GE_DefaultValue(szEntityName, "")
#pragma GE_DefaultValue(MinRadius, "300")
#pragma GE_DefaultValue(MaxRadius, "300")
#pragma GE_DefaultValue(Color, "255.0 255.0 255.0")
#pragma GE_DefaultValue(RadiusFunction, "aza")
#pragma GE_DefaultValue(RadiusSpeed, "2.0")
#pragma GE_DefaultValue(InterpolateValues, "True")
#pragma GE_DefaultValue(EntityName, "")
#pragma GE_DefaultValue(BoneName, "")
#pragma GE_DefaultValue(TriggerName, "")
#pragma GE_DefaultValue(CastShadows, "False")
#pragma GE_DefaultValue(Shadow, "False")
#pragma GE_DefaultValue(Rotate, "False")
#pragma GE_DefaultValue(style, "0")
#pragma GE_Angles(angles)
#pragma GE_DefaultValue(angles, "0 0 0")
#pragma GE_Arc(arc)
#pragma GE_DefaultValue(arc, "45.0")
#pragma GE_Documentation(szEntityName, "Name of this entity, if any")
#pragma GE_Documentation(MinRadius, "Minimum radius of the light (texels)")
#pragma GE_Documentation(MaxRadius, "Maximum radius of the light (texels)")
#pragma GE_Documentation(InterpolateValues, "True or False.  Whether to interpolate between the RadiusFunction values")
#pragma GE_Documentation(RadiusFunction, "a-z, repeated (E.g. aabdfzddsfdz)  Specify light values over time.  a = Minimum z = maximum")
#pragma GE_Documentation(RadiusSpeed, "How fast to run through RadiusFunction values (seconds)")
#pragma GE_Documentation(Color, "Color of the light")
#pragma GE_Documentation(EntityName, "Name of entity to attach to")
#pragma GE_Documentation(Model, "Name of model to attach to")
#pragma GE_Documentation(BoneName, "Name of actor bone to attach to")
#pragma GE_Documentation(TriggerName, "Name of trigger entity to use")
#pragma GE_Documentation(CastShadows, "if true, light casts shadows")
#pragma GE_Documentation(Shadow, "use negative light")
#pragma GE_Documentation(Rotate, "rotate with the model or actor attached to")
#pragma GE_Documentation(angles, "direction of light; offset direction, if attached to a model or an actor")
#pragma GE_Documentation(style, "falloff style: 0=normal, 1=soft, 2=hard")
}	DSpotLight;

/*
// specials for curved surfaces
// CurvePointEnt-
#pragma GE_Type("..\\icons\\Player.ico")
typedef struct CurvePointEnt
{
#pragma GE_Published
	geVec3d	Origin;
	int		myID;
#pragma GE_Origin(Origin)
} CurvePointEnt;

// CurveEnt
#pragma GE_Type("..\\icons\\Player.ico")
typedef struct CurveEnt
{
#pragma GE_Published
	geVec3d	Origin;
	char	*CurvePointIDs;
	int		myID;
#pragma GE_Origin(Origin)
} CurveEnt;

// CurvePatchEnt
#pragma GE_Type("..\\icons\\Player.ico")
typedef struct CurvePatchEnt
{
#pragma GE_Published
	geVec3d	Origin;
	char	*CurveIDs;
	int		myID;
#pragma GE_Origin(Origin)
} CurvePatchEnt;

// CurvedSurfaceEnt
#pragma GE_Type("..\\icons\\Player.ico")
typedef struct CurvedSurfaceEnt
{
#pragma GE_Published
	geVec3d	Origin;
	int		myPatchID;
	int		FlatTexture;
	int		DefaultResX;
	int		DefaultResY;
	char	*TextureName;
#pragma GE_Origin(Origin)
#pragma GE_DefaultValue(DefaultResX, "15")
#pragma GE_DefaultValue(DefaultResY, "15")
} CurvedSurfaceEnt;


//
// VaporTrail
//
#pragma GE_Type("icons\\model.ico")
typedef struct _VaporTrail
{
#pragma GE_Private
	void *Data;
	geBoolean active;
	geVec3d  OriginOffset;      //Translation from model center
	geFloat time;
	int m_NumPoints;
#pragma GE_Published
	geVec3d origin;
	geFloat ParticleLife;
	geFloat StartWidth;
	geFloat Expansion;
	GE_RGBA StartColor;
	GE_RGBA EndColor;
	geVec3d Drift;
	geFloat ParticleCreateRate;
	char 	*EntityName;	// Name of entity to attach to
	geWorld_Model *Model;	// Name of model to attach to
	char 	*BoneName;	// Name of actor bone to attach to
	char 	*TriggerName;	// Name of trigger entity

#pragma GE_Origin(origin)
#pragma GE_DefaultValue(ParticleLife, "1")
#pragma GE_DefaultValue(StartWidth, "0.1")
#pragma GE_DefaultValue(Expansion, "0.1")
#pragma GE_DefaultValue(StartColor, "255 64 64")
#pragma GE_DefaultValue(EndColor, "255 255 64")
#pragma GE_DefaultValue(Drift, "0 0 0.5")
#pragma GE_DefaultValue(ParticleCreateRate, "0.1")
#pragma GE_DefaultValue( EntityName, "")
#pragma GE_DefaultValue( BoneName, "")
#pragma GE_DefaultValue( TriggerName, "")

#pragma GE_Documentation(ParticleLife, "Lifetime of each particle")
#pragma GE_Documentation(StartWidth, "staring width vapor trail")
#pragma GE_Documentation(Expansion, "expansion rate of vapor trail")
#pragma GE_Documentation(Drift, "velocity of vapor trail drift")
#pragma GE_Documentation(ParticleCreateRate, "time between particle creation")
#pragma GE_Documentation(StartColor, "Starting color of vapor")
#pragma GE_Documentation(EndColor, "Ending color of vapor")
#pragma GE_Documentation(EntityName, "Name of entity to attach to")
#pragma GE_Documentation(Model, "Name of model to attach to")
#pragma GE_Documentation(BoneName, "Name of actor bone to attach to")
#pragma GE_Documentation(TriggerName, "Name of trigger entity to use")
} VaporTrail;
*/
// end change RF064

#endif

#pragma warning( default : 4068 )

/* ----------------------------------- END OF FILE ------------------------------------ */
