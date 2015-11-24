/*
	RabidFramework.h:	Reality factory Header File

	(c) 2001 Ralph Deane

	This file includes ALL of the headers necessary to use the
Reality Factory (RGF) for creating Genesis3D-based games.
*/

//	Disable unknown pragma warnings (GEdit #pragmas used here)

#pragma warning(disable : 4068)

//	Disable forcing int to bool 'true' or 'false' warnings

#pragma warning(disable : 4800)

#ifndef __RGF_RABIDMASTERFRAMEWORK_INCLUDE_
#define __RGF_RABIDMASTERFRAMEWORK_INCLUDE_

//	We'll go LEAN and MEAN to drop the Windows stuff we don't want

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

typedef enum
{
	MOVEIDLE = 0,
	MOVEWALKFORWARD,
	MOVEWALKBACKWARD,
	MOVESLIDELEFT,
	MOVESLIDERIGHT,
	MOVEJUMP
} MoveAction;

enum
{
	FIRSTPERSON = 0,
	THIRDPERSON,
	ISOMETRIC,
	FIXEDCAMERA
};

#define QXASSERT assert
#define Frand EffectC_Frand

//	Standard Windows include

#define _CRTDBG_MAP_ALLOC

#include "stdafx.h"

#include <stdlib.h>
#include <crtdbg.h>

#include <mmsystem.h>
#include <vfw.h>
#include <time.h>
#include <dsound.h>
#include <stdio.h>
#include <stdarg.h>
#include <direct.h>
#include <conio.h>

//	Run-time library includes

#include <stdio.h>
#include <math.h>
#include <assert.h>

#include <commdlg.h>
#include <string.h>
#include <evcode.h>
#include <objbase.h>
#include <strmif.h>
#include <control.h>
#include <uuids.h>

//	Genesis3D includes

#include <Genesis.h>
#include <font.h>

//	Additional data types

typedef struct
{
  geFloat Top, Left;
	geFloat Bottom, Right;
} FloatRect;

//	RGF-specific include files

class CCommonData;

#include "IniFile.h"
// changed RF064
#include "Mixer.h"
#include "Qx\\qxTerrainMgr.h"
// end change RF064
#include "GameEntityDataTypes.h"		// Just what it says!  GEdit-usable.
#include "RGFStatus.h"							// Status codes
#include "RGFEvents.h"							// Event codes, etc.
#include "RGFTypes.h"								// Types, defs, etc.
#include "CRGFComponent.h"					// RGF base component interface
// changed RF063
#include "CAnimGif.h"
// end change RF063
#include "Collider.h"								// Collision utility class
#include "CCameraManager.h"					// Camera Manager class
#include "CAudioManager.h"					// Audio entity manager subsystem
#include "CMp3.h"
#include "CActorManager.h"					// Actor management subsystem
#include "CModelManager.h"					// Model management subsystem
#include "CPersistentAttributes.h"	// Persistent attributes class
#include "CGenesisEngine.h"					// Genesis3D engine wrapper
#include "CEntityRegistry.h"				// Entity registry class
#include "CMIDIAudio.h"							// General MIDI playback
#include "CCDAudio.h"								// CD Audio soundtrack class
#include "CPlayer.h"								// Player avatar class
#include "CInput.h"									// Input subsystem class
#include "CAutoDoors.h"							// Automatic door subsystem class
#include "CMovingPlatforms.h"				// Moving platforms subsystem class
#include "CTeleporter.h"						// Teleporter support
#include "CMorphingFields.h"				// Morphing field class
#include "C3DAudioSource.h"					// 3D Audio Source class
#include "CParticleSystem.h"				// Particle system handler class
#include "CStaticEntity.h"					// Static entity handler class
#include "CSoundtrackToggle.h"			// Soundtrack switcher
#include "CAVIPlayer.h"							// AVI player class
#include "CAudioStream.h"						// Streaming audio handler
#include "CVideoTexture.h"					// Video texture handler
#include "CCorona.h"								// Ralph Deane's Corona component
#include "CDynalite.h"							// Ralph Deane's Dynamic Light component
#include "CElectric.h"							// Ralph Deane's electrical bolt component
#include "CProcedural.h"						// Ralph Deane's procedural textures component
#include "CHeadsUpDisplay.h"				// Heads-up display component
#include "CPathDatabase.h"					// Path database component
#include "CPathFollower.h"					// Motion binding component
#include "EffManager.h"							// Ralph Deane's Effects Manager
#include "CRain.h"									// Ralph Deane's Rain Effect
#include "CSpout.h"									// Ralph Deane's Spout Effect
#include "CFloating.h"							// Ralph Deane's Floating Effect
#include "Chaos.h"									// Ralph Deane's Chaos Effect
#include "CFlame.h"									// Ralph Deane's Flame Entity
#include "CMenu.h"									// Ralph Deane's Menu Manager
#include "CAIController.h"					// AI Controller class
#include "Utilities.h"							// Ralph Deane's Utility Code
// changed RF064
#include "CScriptPoint.h"
#include "CPawn.h"
#include "CCountDown.h"
#include "CChange.h"
#include "CMorph.h"
#include "CutScene.h"
#include "CActMaterial.h"
#include "CArmour.h"
// end change RF064
#include "CTriggers.h"							// Ralph Deane's Generic Triggers
#include "CLogic.h"									// Ralph Deane's Trigger Logic
#include "CMessage.h"
#include "CWeapon.h"
#include "CDecal.h"
#include "CWallDecal.h"
#include "CAttribute.h"
#include "CDamage.h"
#include "CFirePoint.h"
#include "CFlipBook.h"
#include "CExplosion.h"
#include "CEffects.h"
#include "CChangeLevel.h"
#include "CShake.h"
#include "CFixedCamera.h"
// changed RF063
#include "CViewSwitch.h"
#include "CInventory.h"
#include "CLiquid.h"
#include "CDSpotLight.h"
// end change RF063

// start multiplayer
#include "CNetwork.h"
// end multiplayer

#include "CCommonData.h"						// Common data handler component

//	Various zone constants.  A "zone" is an area, defined by a brush, that
//	..has specific contents that change the way the framework handles it's
//	..operations on entities in that zone.

const int kNormalZone =		0x0001;			// Entity in normal space
const int kWaterZone =		0x0002;			// Entity is in the water
const int kLavaZone =		0x0004;				// Entity is in lava
const int kToxicGasZone =	0x0008;		// Entity is in toxic gas
const int kNoGravityZone =	0x0010;	// Entity is in zero-G
const int kFrozenZone =		0x0020;			// Entity is in frozen area
const int kSludgeZone =		0x0040;			// Entity is in sludge area
const int kSlowMotionZone = 0x0080;	// Entity is in slow-motion zone
const int kFastMotionZone = 0x0100;	// Entity is in fast-motion zone
const int kImpassibleZone = 0x0200;	// Entity is in an impassible zone
const int kClimbLaddersZone = 0x0400;
// changed RF063
const int kUnclimbableZone = 0x0800;
const int kLiquidZone = 0x1000;
const int kInLiquidZone = 0x2000;
// end change RF063
//	Various support and implementation constants

const geFloat kFogDensity = 1500.0f;			// Baseline teleport/field fog density

//	Collision type constants for actors and the player

const int kNoCollision = 0x0;						// No collision
const int kCollideActor = 0x0001;				// Collided with an actor
const int kCollideWorldModel = 0x0002;	// Collided with a world model
const int kCollideTrigger = 0x0003;			// Collided with a nonmoving trigger
const int kCollidePlatform = 0x0004;		// Collided with a platform
const int kCollideNPC = 0x0005;					// Collided with an NPC
const int kCollideVehicle = 0x0006;			// Collided with a vehicle
const int kCollideWeapon = 0x0007;
const int kCollideDoor = 0x0008;
const int kCollideRecheck = 0x0009;
const int kCollideMove = 0xfffe;				// Unknown collision, move OK
const int kCollideNoMove = 0xffff;			// Unknown collision but don't move

//	File types known to the system

const int kActorFile = 0x0000;					// ACTOR files
const int kAudioFile = 0x0001;					// AUDIO files
const int kVideoFile = 0x0002;					// VIDEO files
const int kMIDIFile =  0x0003;					// MIDI files
const int kLevelFile = 0x0004;					// LEVEL files
const int kAudioStreamFile = 0x0005;		// AUDIO STREAM files
const int kBitmapFile = 0x0006;					// BITMAP files
const int kSavegameFile = 0x0007;				// SAVE GAME files
const int kTempFile = 0x0008;						// Temporary files
const int kInstallFile = 0x0009;				// Install files (main directory)
const int kRawFile = 0x0fff;						// RAW filename, don't modify

//	Debugging levels
//
//	Low debug output = only report major problems
//	Medium debug output = report higher-level system activities
//	High debug output = if it can be traced, put it in the log file

const int kNoDebugOutput = 0x0000;			// No debug output
const int kLowDebugOutput = 0x0001;			// Low debug output
const int kMediumDebugOutput = 0x0002;	// Medium debug output
const int kHighDebugOutput = 0x0003;		// High debug output

//	Camera tracking flags

const int kCameraTrackPosition = 0x0001;		// Camera will track position
const int kCameraTrackRotation = 0x0002;		// Camera will track rotation
const int kCameraTrackThirdPerson = 0x0004;	// Camera tracks "third person" mode
// Mode
const int kCameraTrackFree = 0x0008;		// allows free floating camera
const int kCameraTrackIso = 0x0010;			// Camera tracks isometric view
const int kCameraTrackFixed = 0x0020;

//	Various utility consts

const int kMaxTicksPassed = 120;					// Clamping value for elapsed time

//	All include files set up.  Now, we need to define the global
//	..class pointers for all of the subsystems.  This is not the
//	..most oject-oriented, data-hiding-and-encapsulation way to
//	..do this, but it's the easiest in the long run for times
//	..where the various subsystems need to be able to communicate
//	..with each other.  Deal with it.

#ifdef _THE_MASTER_MODULE_

//	If we're in the MASTER MODULE, as defined by having the above
//	..preprocessor variable defined, we set up an initialized pointer
//	..to the common data handler class.

CCommonData *CCD = NULL;								// Common data handler class

#else						// Not in the master module

extern CCommonData *CCD;								// Common data handler class

#endif					// _THE_MASTER_MODULE_

#endif					// __RABIDMASTERFRAMEWORK_INCLUDE_
