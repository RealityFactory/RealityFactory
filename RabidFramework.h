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
	FIXEDCAMERA,
	FREEFLOATING
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

#include <FreeImage.h>
#include <OIS.h>

//	Genesis3D includes
#include <Genesis.h>

//	Additional data types

typedef struct
{
	geFloat Top, Left;
	geFloat Bottom, Right;
} FloatRect;

//	RGF-specific include files

class CCommonData;

#include "CLog.h"
#include "GameEntityDataTypes.h"		// Just what it says!  GEdit-usable.
#include "RGFStatus.h"					// Status codes
#include "RGFEvents.h"					// Event codes, etc.
#include "RGFTypes.h"					// Types, defs, etc.
#include "RGFConstants.h"				// Constants
#include "CRGFComponent.h"				// RGF base component interface
#include "Collider.h"					// Collision utility class
#include "CCameraManager.h"				// Camera Manager class
#include "CActorManager.h"				// Actor management subsystem
#include "CModelManager.h"				// Model management subsystem
#include "EffManager.h"					// Ralph Deane's Effects Manager
#include "Utilities.h"					// Ralph Deane's Utility Code
#include "CPersistentAttributes.h"		// Persistent attributes class
#include "CGenesisEngine.h"				// Genesis3D engine wrapper
#include "CEntityRegistry.h"			// Entity registry class
#include "CPlayer.h"					// Player avatar class
#include "CWeapon.h"
#include "CInput.h"						// Input subsystem class
#include "CAnimGif.h"
#include "CAVIPlayer.h"					// AVI player class
#include "CAudioStream.h"				// Streaming audio handler
#include "CMenu.h"						// Ralph Deane's Menu Manager
#include "CExplosion.h"
#include "CEffects.h"
#include "CNetwork.h"
#include "CLevelController.h"
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
