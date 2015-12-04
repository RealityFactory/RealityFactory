/************************************************************************************//**
 * @file RGFConstants.h
 * @brief Reality Factory constants file
 *
 * This file contains various constants used throughout the RGF system.
 * @author Daniel Queteschiner
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_RGFCONSTANTS_H__
#define __RGF_RGFCONSTANTS_H__

// Various zone constants.  A "zone" is an area, defined by a brush, that
// ..has specific contents that change the way the framework handles it's
// ..operations on entities in that zone.

const int kNormalZone		= 0x0001;	///< Entity in normal space
const int kWaterZone		= 0x0002;	///< Entity is in the water
const int kLavaZone			= 0x0004;	///< Entity is in lava
const int kToxicGasZone		= 0x0008;	///< Entity is in toxic gas
const int kNoGravityZone	= 0x0010;	///< Entity is in zero-G
const int kFrozenZone		= 0x0020;	///< Entity is in frozen area
const int kSludgeZone		= 0x0040;	///< Entity is in sludge area
const int kSlowMotionZone	= 0x0080;	///< Entity is in slow-motion zone
const int kFastMotionZone	= 0x0100;	///< Entity is in fast-motion zone
const int kImpassibleZone	= 0x0200;	///< Entity is in an impassible zone
const int kClimbLaddersZone = 0x0400;
const int kUnclimbableZone	= 0x0800;
const int kLiquidZone		= 0x1000;
const int kInLiquidZone		= 0x2000;

// Various support and implementation constants

const float kFogDensity = 1500.0f;	///< Baseline teleport/field fog density

// Collision type constants for actors and the player

const int kNoCollision			= 0x0;		///< No collision
const int kCollideActor			= 0x0001;	///< Collided with an actor
const int kCollideWorldModel	= 0x0002;	///< Collided with a world model
const int kCollideTrigger		= 0x0003;	///< Collided with a nonmoving trigger
const int kCollidePlatform		= 0x0004;	///< Collided with a platform
const int kCollideNPC			= 0x0005;	///< Collided with an NPC
const int kCollideVehicle		= 0x0006;	///< Collided with a vehicle
const int kCollideWeapon		= 0x0007;
const int kCollideDoor			= 0x0008;
const int kCollideRecheck		= 0x0009;
const int kCollideMesh			= 0x000a;
const int kCollideMove			= 0xfffe;	///< Unknown collision, move OK
const int kCollideNoMove		= 0xffff;	///< Unknown collision but don't move

// File types known to the system

const int kActorFile		= 0x0000;		///< ACTOR files
const int kAudioFile		= 0x0001;		///< AUDIO files
const int kVideoFile		= 0x0002;		///< VIDEO files
const int kMIDIFile			= 0x0003;		///< MIDI files
const int kLevelFile		= 0x0004;		///< LEVEL files
const int kAudioStreamFile	= 0x0005;		///< AUDIO STREAM files
const int kBitmapFile		= 0x0006;		///< BITMAP files
const int kSavegameFile		= 0x0007;		///< SAVE GAME files
const int kTempFile			= 0x0008;		///< Temporary files
const int kInstallFile		= 0x0009;		///< Install files (main directory)
const int kScriptFile		= 0x000a;		///< SCRIPT files
const int kConfigFile		= 0x000b;		///< Config files
const int kLogFile			= 0x000c;		///< Config files
const int kScreenshotFile	= 0x000d;		///< Config files
const int kRawFile			= 0x0fff;		///< RAW filename, don't modify

// Debugging levels
//
// Low debug output = only report major problems
// Medium debug output = report higher-level system activities
// High debug output = if it can be traced, put it in the log file

const int kNoDebugOutput		= 0x0000;	///< No debug output
const int kLowDebugOutput		= 0x0001;	///< Low debug output
const int kMediumDebugOutput	= 0x0002;	///< Medium debug output
const int kHighDebugOutput		= 0x0003;	///< High debug output

// Camera tracking flags

const int kCameraTrackPosition		= 0x0001;	///< Camera will track position
const int kCameraTrackRotation		= 0x0002;	///< Camera will track rotation
const int kCameraTrackThirdPerson	= 0x0004;	///< Camera tracks "third person" mode
const int kCameraTrackFree			= 0x0008;	///< allows free floating camera
const int kCameraTrackIso			= 0x0010;	///< Camera tracks isometric view
const int kCameraTrackFixed			= 0x0020;	///< fixed camera

// Various utility consts

const int kMaxTicksPassed = 120;				///< Clamping value for elapsed time

#endif // __RGF_RGFCONSTANTS_H__

/* ----------------------------------- END OF FILE ------------------------------------ */
