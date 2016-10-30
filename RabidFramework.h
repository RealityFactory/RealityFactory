/************************************************************************************//**
 * @file RabidFramework.h
 * @brief Reality Factory Header File
 *
 * This file includes common headers necessary to use
 * Reality Factory (RGF) for creating Genesis3D-based games.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

//	Disable unknown pragma warnings (GEdit #pragmas used here)
#pragma warning(disable : 4068)

//	Disable forcing int to bool 'true' or 'false' warnings
#pragma warning(disable : 4800)

// Disable unsafe warning
#pragma warning (disable : 4996)

// Disable warning: 'identifier': identifier was truncated to 'number' characters in the debug information
#pragma warning (disable : 4786)


#ifndef __RGF_RABIDMASTERFRAMEWORK_INCLUDE_
#define __RGF_RABIDMASTERFRAMEWORK_INCLUDE_

//	We'll go LEAN and MEAN to drop the Windows stuff we don't want
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifdef _DEBUG
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)
#else
#define DEBUG_NEW new
#endif

/**
 * @brief Move actions
 */
typedef enum
{
	MOVEIDLE = 0,
	MOVEWALKFORWARD,
	MOVEWALKBACKWARD,
	MOVESLIDELEFT,
	MOVESLIDERIGHT,
	MOVEJUMP

} MoveAction;

/**
 * @brief Camera views
 */
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

#define RF_VERSION_MAJOR	0
#define RF_VERSION_MINOR	76
#define RF_VMAJS			"0"
#define RF_VMINS			"76   "

#define SAFE_DELETE(p)		{ delete (p);	(p)=NULL; }
#define SAFE_DELETE_A(p)	{ delete[] (p);	(p)=NULL; }
#define SAFE_FREE(p)		{ free(p);		(p)=NULL; }

#include <CEGUI.h>	// include before windows.h or you'll get messages about undefining min/max
#include <windows.h>
#include <stdlib.h>

#include <mmsystem.h>
#include <vfw.h>
#include <time.h>
#include <dsound.h>
#include <stdio.h>
#include <stdarg.h>
#include <direct.h>
#include <conio.h>

//	Run-time library includes

#include <math.h>
#include <assert.h>

#include <commdlg.h>
#include <string>
#include <evcode.h>
#include <objbase.h>
#include <strmif.h>
#include <control.h>
#include <uuids.h>

//	Genesis3D includes
#include <FreeImage.h>
#include <Genesis.h>
#include <font.h>

#include <joystick.h>
#pragma comment(lib,"dinput8.lib")

//	Additional data types

typedef struct
{
	geFloat Top, Left;
	geFloat Bottom, Right;
} FloatRect;

//	RGF-specific include files

class CCommonData;

#include "CLog.h"
#include "IniFile.h"
#include "Mixer.h"
#include "Qx\\qxTerrainMgr.h"
#include "GameEntityDataTypes.h"		// Just what it says!  GEdit-usable.
#include "RGFStatus.h"					// Status codes
#include "RGFEvents.h"					// Event codes, etc.
#include "RGFTypes.h"					// Types, defs, etc.
#include "RGFConstants.h"				// Constants
#include "CRGFComponent.h"				// RGF base component interface
#include "CAnimGif.h"
#include "Collider.h"					// Collision utility class
#include "CCameraManager.h"				// Camera Manager class
#include "CAudioManager.h"				// Audio entity manager subsystem
#include "OggAudio.h"
#include "CMp3.h"
#include "CActorManager.h"				// Actor management subsystem
#include "CModelManager.h"				// Model management subsystem
#include "CPersistentAttributes.h"		// Persistent attributes class
#include "CGenesisEngine.h"				// Genesis3D engine wrapper
#include "CEntityRegistry.h"			// Entity registry class
#include "CMIDIAudio.h"					// General MIDI playback
#include "CCDAudio.h"					// CD Audio soundtrack class
#include "CPlayer.h"					// Player avatar class
#include "CInput.h"						// Input subsystem class
#include "CAutoDoors.h"					// Automatic door subsystem class
#include "CMovingPlatforms.h"			// Moving platforms subsystem class
#include "CTeleporter.h"				// Teleporter support
#include "CMorphingFields.h"			// Morphing field class
#include "C3DAudioSource.h"				// 3D Audio Source class
#include "CParticleSystem.h"			// Particle system handler class
#include "CStaticEntity.h"				// Static entity handler class
#include "CAVIPlayer.h"					// AVI player class
#include "CAudioStream.h"				// Streaming audio handler
#include "CSoundtrackToggle.h"			// Soundtrack switcher
#include "CVideoTexture.h"				// Video texture handler
#include "CCorona.h"					// Ralph Deane's Corona component
#include "CDynalite.h"					// Ralph Deane's Dynamic Light component
#include "CElectric.h"					// Ralph Deane's electrical bolt component
#include "CProcedural.h"				// Ralph Deane's procedural textures component
#include "CHeadsUpDisplay.h"			// Heads-up display component
#include "CPathDatabase.h"				// Path database component
#include "CPathFollower.h"				// Motion binding component
#include "EffManager.h"					// Ralph Deane's Effects Manager
#include "CRain.h"						// Ralph Deane's Rain Effect
#include "CSpout.h"						// Ralph Deane's Spout Effect
#include "CFloating.h"					// Ralph Deane's Floating Effect
#include "Chaos.h"						// Ralph Deane's Chaos Effect
#include "CFlame.h"						// Ralph Deane's Flame Entity
#include "CMenu.h"						// Ralph Deane's Menu Manager
#include "Utilities.h"					// Ralph Deane's Utility Code
#include "CScriptPoint.h"
#include "CPawn.h"
#include "CCountDown.h"
#include "CChange.h"
#include "CMorph.h"
#include "CutScene.h"
#include "CActMaterial.h"
#include "CArmour.h"
#include "CLiftBelt.h"
#include "CTriggers.h"					// Ralph Deane's Generic Triggers
#include "CLogic.h"						// Ralph Deane's Trigger Logic
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
#include "CViewSwitch.h"
#include "CInventory.h"
#include "CLiquid.h"
#include "COverlay.h"
#include "CDSpotLight.h"
#include "CStaticMesh.h"

#include "PWXImageManager.h"
#include "CPolyShadow.h"	// Fake Shadow
#include "CFlipTree.h"		// Pickles Jul 04
#include "CFoliage.h"		// Pickles Jul 04
#include "CAreaChecker.h"	// Pickles Oct 04

#include "CNetwork.h"
#include "CLevelController.h"

#include "CWindGenerator.h"

#include "CCommonData.h"				// Common data handler component


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

CCommonData *CCD = NULL;						///< Common data handler class

#else					// Not in the master module

extern CCommonData *CCD;						///< Common data handler class

#endif					// _THE_MASTER_MODULE_

#endif					// __RABIDMASTERFRAMEWORK_INCLUDE_

/* ----------------------------------- END OF FILE ------------------------------------ */
