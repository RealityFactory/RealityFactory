/************************************************************************************//**
 * @file CCommonData.cpp
 * @brief Global Data Pool class
 *
 * This file contains the class declaration for the Global Data Pool handling
 * class.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#include "RabidFramework.h"
#include "RGFScriptMethods.h"

extern geVFile *PassWord(char *m_VirtualFile, bool encrypt);
extern void CloseFile();

#include "Simkin\\skInterpreter.h" // change simkin
#include "IniFile.h"
#include "CAudioManager.h"
#include "CCDAudio.h"
#include "CMIDIAudio.h"
#include "CHeadsUpDisplay.h"
#include "CInventory.h"
#include "CArmour.h"
#include "CPolyShadow.h"
#include "Qx\\qxTerrainMgr.h"
#include "HawkNL\\nl.h"

skInterpreter interpreter;

/* ------------------------------------------------------------------------------------ */
// Constructor
//
// Initialize all common data pointers, store a pointer to the
// ..Genesis engine object.
/* ------------------------------------------------------------------------------------ */
CCommonData::CCommonData()
{
	VFS					= NULL;
	theGameEngine		= NULL;		// Genesis engine class
	theNetPlayerMgr		= NULL;
	theUserInput		= NULL;		// User input class
	thePlayer			= NULL;		// Player avatar class
	theAutoDoors		= NULL;		// Automatic doors class
	thePlatforms		= NULL;		// Moving platforms class
	theTeleports		= NULL;		// Teleporter class
	theFields			= NULL;		// Morphing fields class
	theCDPlayer			= NULL;		// CD Audio player class
	theMIDIPlayer		= NULL;		// MIDI Audio player class
	the3DAudio			= NULL;		// 3D Audio Source class
	theParticles		= NULL;		// Particle systems handler class
	theProps			= NULL;		// Static entity handler class
	theMeshes			= NULL;
	theSTToggles		= NULL;		// Soundtrack toggle handler class
	theStreams			= NULL;		// Streaming audio handler
	theVidText			= NULL;		// Video texture handler
	theCorona			= NULL;		// Ralph Deane's corona effects class
	theDynalite			= NULL;		// Ralph Deane's dynamic light class
	theElectric			= NULL;		// Ralph Deane's electric bolt class
	theProcTexture		= NULL;		// Ralph Deane's procedural texture class
	theHUD				= NULL;		// Heads-up display class
	thePathDatabase		= NULL;		// Path database
	theFollower			= NULL;		// Path follower class
	theRegistry			= NULL;		// Entity registry class
	theEffect			= NULL;		// Ralph Deane's Effect Manager
	theRain				= NULL;		// Ralph Deane's Rain Effect
	theSpout			= NULL;		// Ralph Deane's Spout Effect
	theActorSpout		= NULL;
	theMorph			= NULL;
	theCutScene			= NULL;
	theActMaterial		= NULL;
	theTerrainMgr		= NULL;
	theArmour			= NULL;
	theLiftBelt			= NULL;
	theWindGenerator	= NULL;
	theFloat			= NULL;		// Ralph Deane's Floating Effect
	theChaos			= NULL;		// Ralph Deane's Chaos Procedural
	theFlame			= NULL;		// Ralph Deane's Flame Effect
	theMenu				= NULL;		// Ralph Deane's Menu Manager
	theCollider			= NULL;		// Collision Detection Subsystem
	theActorManager		= NULL;		// Actor Manager subsystem
	theModelManager		= NULL;		// Model Manager subsystem
	theAudioManager		= NULL;		// Audio Manager subsystem
	theCameraManager	= NULL;		// Camera Manager subsystem
	theTriggers			= NULL;		// Ralph Deane's Generic Triggers
	theLogic			= NULL;		// Ralph Deane's Trigger Logic
	theMessage			= NULL;
	theWeapon			= NULL;
	theFirePoint		= NULL;
	theFlipBook			= NULL;
	theFoliage			= NULL;		// Pickles Jul 04
	theFlipTree			= NULL;		// PWX
	thePWXImage			= NULL;		// PWX
	thePolyShadow		= NULL;		// PWX
	theAreaCheck		= NULL;		// PWX
	theDecal			= NULL;
	theWallDecal		= NULL;
   theLevelController	= NULL;
	theAttribute		= NULL;
	theDamage			= NULL;
	theExplosion		= NULL;
	theCExplosion		= NULL;
	thePreEffect		= NULL;
	theScriptPoints		= NULL;
	thePawn				= NULL;
	theCountDownTimer	= NULL;
	theChangeAttribute	= NULL;
	theShake			= NULL;
	theFixedCamera		= NULL;
	theChangeLevel		= NULL;
	theViewSwitch		= NULL;
	theInventory		= NULL;
	theLiquid			= NULL;
	theCDSpot			= NULL;
	theOverlay			= NULL;

	srand((unsigned)time(NULL));

	FreeImage_Initialise();

	MethodHash = new CLongHashTable(RGF_SM_MAXMETHODS);
	FillHashMethods();	//change scripting

	// Initialize game state data
	m_InGameLoop = true;					// We start in the game loop
	m_TotalPlayTime = 0;					// With no time spent playing
	m_ChangeLevel = false;					// No level change yet
	m_DebugLevel = kHighDebugOutput;		// Default to HIGH debug output

	//	Set up directory defaults
	strcpy(m_LevelDirectory,		"levels"		);
	strcpy(m_ActorDirectory,		"actors"		);
	strcpy(m_BitmapDirectory,		"bitmaps"		);
	strcpy(m_AudioDirectory,		"audio"			);
	strcpy(m_VideoDirectory,		"video"			);
	strcpy(m_AudioStreamDirectory,	"audio"			);
	strcpy(m_MIDIDirectory,			"MIDI"			);
	strcpy(m_MenuDirectory,			"menu.ini"		);  // update #1
	strcpy(m_VirtualFile,			"Pack.vfs"		);
	strcpy(m_SplashScreen,			"rflogo.bmp"	);
	strcpy(m_SplashAudio,			"startup.wav"	);
	strcpy(m_PlayerName,			"Unnamed"		);

	m_CutScene[0]		= '\0';
	m_CutScene1[0]		= '\0';
	m_SplashScreen1[0]	= '\0';
	m_SplashAudio1[0]	= '\0';

	m_MouseControl		= true;
	m_headbob			= false;
	m_weaponposition	= false;
	Paused				= false;
	KeyPaused			= false;
	UseAngle			= true;
	jumpkey				= false;
	runkey				= false;
	crouchkey			= false;
	zoomkey				= false;
	lightkey			= false;
	ShowTrack			= false;
	KeepAttributes		= true;
	CSelect				= false;
	NSelect				= false;
	UseDialog			= false;
	m_bUseDInput		= true;
	loadkey				= false;
	savekey				= false;
	saving				= false;
	usekey				= false;
	invkey				= false;
	HasFocus			= true;
	dropkey				= false;
	reloadkey			= false;
	CDifficult			= false;
	DifficultLevel		= 1;
	consolekey			= false;
	consoleflag			= false;
	network				= false;
	if(nlInit())
		network			= true;
	multiplayer			= false;

	m_Language = 0;
	Logging = false;


	// Set up for timekeeping
	m_nTimerID = timeSetEvent(1, 0,	&TimerFunction, reinterpret_cast<DWORD>(this),
							TIME_PERIODIC | TIME_CALLBACK_FUNCTION);

	TimeCounter = LastTimePoll = 0;
	LastTimePassed_D = 0;
	LastTimePassed_F = 0;

	// Debug tracing time
	OutputDebugString("CCommonData initialized\n");
}

/* ------------------------------------------------------------------------------------ */
// Destructor
//
// Just clean up pointers, if needed.
/* ------------------------------------------------------------------------------------ */
CCommonData::~CCommonData()
{
	ShutdownLevel();
	ShutdownCommon();
	geVFile_CloseAPI();

	if(VFS!=NULL)
	{
		CloseFile();
	}



	FreeImage_DeInitialise();

	ClearHashMethods();
	SAFE_DELETE(MethodHash);

// start multiplayer
	if(network)
		nlShutdown();
// end multiplayer

	timeKillEvent(m_nTimerID);

	FILE *fd;
	if(fd = CCD->OpenRFFile(kRawFile, ".\\RealityFactory.log", "at"))
	{
		fprintf(fd, "Successful Shutdown");
		fclose(fd);
	}
}

/* ------------------------------------------------------------------------------------ */
// InitializeCommon
//
// This function initializes all the COMMON entities that survive
// ..between level loads.  These need only be initialized ONCE.
// ..Note that the engine setup is driven from RealityFactory.INI, if this
// ..file doesn't exist, the default (640x480, windowed) is used.
/* ------------------------------------------------------------------------------------ */
int CCommonData::InitializeCommon(HINSTANCE hInstance, char *szStartLevel, bool CommandLine)
{
	bool bFullScreen	= false;	// Default to windowed
	bool bSoftware		= false;	// Default to hardware acceleration
	char chTheDriver	= '(';		// Default to Direct3D
	int nHeight			= 800;		// Default to 800 high
	int nWidth			= 600;		// Default to 600 wide
	FILE *fd;						// Used for prefs file
	char szTitle[80]	= "Game";	// Game title
	geFloat fGamma		= 1.5f;
	bool UseCut			= false;
	bool UseCut1		= false;
	bool UseSecond		= false;
	bool UseFirst		= false;

	CmdLine = CommandLine;


	// Ok, let's see if we have an initialization file, and if so, read it in and parse it.
	if((fd = CCD->OpenRFFile(kRawFile, ".\\RealityFactory.ini", "rt")) != NULL)
	{
		// File there, parse it!
		char szInputLine[132];
		char *szAtom, *szArg;
		int nTemp;

		while(fgets(szInputLine, 132, fd) != NULL)
		{
			if(szInputLine[0] == ';')
				continue;				// Comment line

			if(strlen(szInputLine) <= 5)
				continue;				// Skip blank lines

			// All config commands are "thing=value"
			szAtom = strtok(szInputLine," =");
			szArg = strtok(NULL, " \n");

			if(!stricmp(szAtom, "leveldirectory"))
			{
				if(szArg != NULL)
					strcpy(m_LevelDirectory, szArg);
			}
			else if(!stricmp(szAtom, "bitmapdirectory"))
			{
				if(szArg != NULL)
					strcpy(m_BitmapDirectory, szArg);
			}
			else if(!stricmp(szAtom, "actordirectory"))
			{
				if(szArg != NULL)
					strcpy(m_ActorDirectory, szArg);
			}
			else if(!stricmp(szAtom, "audiodirectory"))
			{
				if(szArg != NULL)
					strcpy(m_AudioDirectory, szArg);
			}
			else if(!stricmp(szAtom, "audiostreamdirectory"))
			{
				if(szArg != NULL)
					strcpy(m_AudioStreamDirectory, szArg);
			}
			else if(!stricmp(szAtom, "videodirectory"))
			{
				if(szArg != NULL)
					strcpy(m_VideoDirectory, szArg);
			}
			else if(!stricmp(szAtom, "mididirectory"))
			{
				if(szArg != NULL)
					strcpy(m_MIDIDirectory, szArg);
			}
			else if(!stricmp(szAtom, "menu"))
			{
				if(szArg != NULL)
					strcpy(m_MenuDirectory, szArg);
			}
			else if(!stricmp(szAtom, "packfile"))
			{
				if(szArg != NULL)
					strcpy(m_VirtualFile, szArg);
			}
			else if(!stricmp(szAtom, "fullscreen"))
			{
				// Set fullscreen/windowed
				if(!stricmp(szArg, "true"))
					bFullScreen = true;
				else
					bFullScreen = false;
			}
			else if(!stricmp(szAtom, "logging"))
			{
				if(!stricmp(szArg, "true"))
					Logging = true;
				else
					Logging = false;
			}
			else if(!stricmp(szAtom, "usedialog"))
			{
				// Set UseDialog
				if(!stricmp(szArg, "true") && !CommandLine)
					UseDialog = true;
				else
					UseDialog = false;
			}
			else if(!stricmp(szAtom, "showtrack"))
			{
				// Set ShowTrack
				if(!stricmp(szArg, "true"))
					ShowTrack = true;
				else
					ShowTrack = false;
			}
			else if(!stricmp(szAtom, "usecharselect"))
			{
				if(!stricmp(szArg, "true"))
					CSelect = true;
				else
					CSelect = false;
			}
			else if(!stricmp(szAtom, "usedifficultlevel"))
			{
				if(!stricmp(szArg, "true"))
					CDifficult = true;
				else
					CDifficult = false;
			}
			else if(!stricmp(szAtom, "usefirst"))
			{
				if(!stricmp(szArg, "true"))
					UseFirst = true;
				else
					UseFirst = false;
			}
			else if(!stricmp(szAtom, "usecutscene"))
			{
				if(!stricmp(szArg, "true"))
					UseCut = true;
				else
					UseCut = false;
			}
			else if(!stricmp(szAtom, "splashscreen"))
			{
				if(szArg != NULL)
					strcpy(m_SplashScreen, szArg);
			}
			else if(!stricmp(szAtom, "splashaudio"))
			{
				if(szArg != NULL)
					strcpy(m_SplashAudio, szArg);
			}
			else if(!stricmp(szAtom, "cutscene"))
			{
				if(szArg != NULL)
					strcpy(m_CutScene, szArg);
			}
			else if(!stricmp(szAtom, "usesecond"))
			{
				if(!stricmp(szArg, "true"))
					UseSecond = true;
				else
					UseSecond = false;
			}
			else if(!stricmp(szAtom, "usecutscene1"))
			{
				if(!stricmp(szArg,"true"))
					UseCut1 = true;
				else
					UseCut1 = false;
			}
			else if(!stricmp(szAtom, "splashscreen1"))
			{
				if(szArg != NULL)
					strcpy(m_SplashScreen1, szArg);
			}
			else if(!stricmp(szAtom, "splashaudio1"))
			{
				if(szArg != NULL)
					strcpy(m_SplashAudio1, szArg);
			}
			else if(!stricmp(szAtom, "cutscene1"))
			{
				if(szArg != NULL)
					strcpy(m_CutScene1, szArg);
			}
			else if(!stricmp(szAtom, "weaponposition"))
			{
				// Set Weapon Positioning
				if(!stricmp(szArg, "on"))
					m_weaponposition = true;
				else
					m_weaponposition = false;
			}
			else if(!stricmp(szAtom, "playeravatar"))
			{
				if((szArg != NULL) && (strlen(szArg) > 0))
					strcpy(m_PlayerAvatar, szArg);
				else
					strcpy(m_PlayerAvatar, "virgil.act");
			}
			else if(!stricmp(szAtom, "gamename"))
			{
				memset(szTitle, 0, 80);
				while(szArg != NULL)
				{
					strcat(szTitle,szArg);
					strcat(szTitle, " ");
					szArg = strtok(NULL, " \n");
				}
			}
			else if(!stricmp(szAtom, "renderer"))
			{
			}
			else if(!stricmp(szAtom, "defaultdifficulty"))
			{
				if(szArg != NULL)
				{
					nTemp = atoi(szArg);
					DifficultLevel = nTemp;

					if(DifficultLevel < 1)
						DifficultLevel = 1;

					if(DifficultLevel > 3)
						DifficultLevel = 3;
				}
			}
			else if(!stricmp(szAtom, "width"))
			{
				// Set display width
				if(szArg != NULL)
					nTemp = atoi(szArg);

				if(nTemp > 0)
					nWidth = nTemp;
			}
			else if(!stricmp(szAtom, "height"))
			{
				// Set display height
				if(szArg != NULL)
					nTemp = atoi(szArg);

				if(nTemp > 0)
					nHeight = nTemp;
			}
			else if(!stricmp(szAtom, "driver"))
			{
				if(szArg != NULL)
				{
					if(!stricmp(szArg, "d3d"))
						chTheDriver = '(';
					else if(!stricmp(szArg, "d3d16"))
						chTheDriver = 'D';				// Use Direct3D in fullscreen
					else if(!stricmp(szArg, "opengl"))
						chTheDriver = 'O';
					else if(!stricmp(szArg, "wire"))
						chTheDriver = 'W';
					else if(!stricmp(szArg, "auto"))
						chTheDriver = 'A';				// Use Auto Detect
					else if(!stricmp(szArg, "pick"))
						chTheDriver = 'P';				// Pop a driver pick list
					else
					{
						ReportError("[WARNING] Bad driver selection in RealityFactory.ini\n", false);
						chTheDriver = 'P';
					}
				}
			}
			else if(!stricmp(szAtom, "startlevel") && !CommandLine)
			{
				if(szArg != NULL)
					strcpy(szStartLevel, szArg);
			}
			else if(!stricmp(szAtom, "defaultlanguage"))
			{
				if(szArg != NULL)
				{
					nTemp = atoi(szArg);
					m_Language = nTemp-1;

					if(m_Language < 0)
						m_Language = 0;

					if(m_Language > 4)
						m_Language = 4;
				}
			}
			else if(!stricmp(szAtom, "playername"))
			{
				if((szArg != NULL) && (strlen(szArg) > 0))
				{
					strncpy(m_PlayerName, szArg, 63);
					m_PlayerName[63] = '\0';
				}
				else
					strcpy(m_PlayerName, "Unnamed");
			}
			else if(!stricmp(szAtom, "usenameselect"))
			{
				if(!stricmp(szArg, "true"))
					NSelect = true;
				else
					NSelect = false;
			}
			else if(!stricmp(szAtom, "usedirectinput"))
			{
				if(!stricmp(szArg, "false"))
					m_bUseDInput = false;
			}
			else
				ReportError("[WARNING] Unknown command in RealityFactory.ini\n", false);
		}

		fclose(fd);
	}

	CCD->InitJoysticks(); // pickles Jul 04

	if(UseFirst)
	{
		if(UseCut)	m_SplashScreen[0] = '\0';
		else		m_CutScene[0] = '\0';
	}
	else
	{
		m_SplashScreen[0] = '\0';
		m_CutScene[0] = '\0';
	}

	if(UseSecond)
	{
		if(UseCut1)	m_SplashScreen1[0] = '\0';
		else		m_CutScene1[0] = '\0';
	}
	else
	{
		m_SplashScreen1[0] = '\0';
		m_CutScene1[0] = '\0';
	}

	theGameEngine = new CGenesisEngine(bFullScreen, nWidth, nHeight, szTitle, hInstance,
										chTheDriver, bSoftware, UseDialog, szStartLevel);

	if(theGameEngine == NULL)
	{
		ReportError("[ERROR] Failed to create Genesis3D Graphics engine!\n", true);
		return -1;
	}

	if(m_DebugLevel != kNoDebugOutput)
		theGameEngine->SetDebugging(true);			// Activate engine debugging

	ReportError("\nInitializing Game Shell...",										false);
	ReportError("-----------------------------------------------",					false);
	ReportError("--- Reality Factory "RF_VMAJS"."RF_VMINS"                 ---",	false);
	ReportError("--- Build Date: "__DATE__", Time: "__TIME__" ---",					false);
	ReportError("--- For more Information, visit:            ---",					false);
	ReportError("---    www.realityfactory.info              ---",					false);
	ReportError("-----------------------------------------------",					false);
	ReportError("\nParsed RealityFactory.ini file",									false);
	//	First, initialize the Genesis3D game engine
	ReportError("\nGenesis3D Initialized", false);

	FILE *fdInput = NULL;
	char szInputString[16];

	if((fdInput = fopen(m_VirtualFile, "rt")) == NULL)
	{
		VFS = NULL;
		ReportError("\n[INFO] No VFS - Reading from Real File System...", false);
	}
	else
	{
		fread(szInputString, 4, 1, fdInput);
		fclose(fdInput);

		if(memcmp(szInputString, "CF00", 4) == 0)
		{
			VFS = PassWord(m_VirtualFile, true);
			ReportError("\n[INFO] VFS detected (encrypted)...", false);
		}
		else
		{
			VFS = PassWord(m_VirtualFile, false);
			ReportError("\n[INFO] VFS detected (not encrypted)...", false);
		}
	}

	//	Fire up a camera for us to see through
	ReportError("\nInitializing Camera Manager...", false);
	theCameraManager = new CCameraManager();

	if(theCameraManager == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create Camera Manager", false);
		return -93;
	}

	geEngine_SetGamma(theGameEngine->Engine(), 1.0f);

	// We have a 3D engine, now initialize the user input subsystem
	ReportError("Initializing User Input Subsystem...", false);
	theUserInput = new CInput();

	if(theUserInput == NULL)
	{
		ReportError("[ERROR] Failed to create input subsystem", false);
		return -2;
	}

	//	The Audio Manager needs to be prepared...
	ReportError("Initializing Audio Manager Subsystem...", false);
	theAudioManager = new CAudioManager();

	if(theAudioManager == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create Audio Manager", false);
		return -92;
	}

	//	Get the CD Audio player working.  Note that it's not a fatal error to
	//	..not have a CD Player device, as this system may be using the General
	//	..MIDI soundtrack instead.
	ReportError("Initializing CD Audio Manager Subsystem...", false);
	theCDPlayer = new CCDAudio();

	if(theCDPlayer == NULL)
		theGameEngine->ReportError("[ERROR] CD Player failed to instantiate", false);

	//	Fire up the MIDI playback system.  Again, as with the CD Player, failure
	//	..to instantiate is not grounds for aborting game play, although I'd
	//	..personally consider the game unplayable without MY soundtrack (heh).
	ReportError("Initializing Midi Audio Manager Subsystem...", false);
	theMIDIPlayer = new CMIDIAudio();

	if(theMIDIPlayer == NULL)
		theGameEngine->ReportError("[ERROR] MIDI Player failed to instantiate", false);

	ReportError("Initializing RF Menu Manager Subsystem...", false);
	theMenu = new CRFMenu(szStartLevel); // new CRFMenu();

	if(theMenu == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create Menu subsystem", false);
		return -2;
	}

	ReportError("Initializing Collision Manager Subsystem...", false);
	theCollider = new Collider();

	if(theCollider == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create Collider subsystem", false);
		return -100;
	}

// start multiplayer
	ReportError("Initializing Network Manager Subsystem...", false);
	theNetPlayerMgr = new NetPlayerMgr();

	if(theNetPlayerMgr == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create NetPlayerMgr subsystem", false);
		return -100;
	}
// end multiplayer


	// Finally, initialize the AVIFile library.  This is done independent of
	// ..any AVI-specific classes so that we can guarantee only ONE instance
	// ..of the library is loaded.
	ReportError("Initializing AVIFile Video Subsystem...", false);
	AVIFileInit();

	// Common subsystems initialized, back to caller!
	return 0;
}

// start multiplayer
/* ------------------------------------------------------------------------------------ */
// ShutDownNetWork
/* ------------------------------------------------------------------------------------ */
void CCommonData::ShutDownNetWork()
{
	if(theNetPlayerMgr != NULL)
		delete theNetPlayerMgr;

	theNetPlayerMgr = NULL;
	theNetPlayerMgr = new NetPlayerMgr();

	if(theNetPlayerMgr == NULL)
	{
		theGameEngine->ReportError("[WARNING] Failed to create NetPlayerMgr subsystem", false);
		return;
	}
}

/* ------------------------------------------------------------------------------------ */
// SetMultiPlayer
/* ------------------------------------------------------------------------------------ */
void CCommonData::SetMultiPlayer(bool multi, bool Server)
{
	multiplayer = multi;
	server = Server;
}

/* ------------------------------------------------------------------------------------ */
// GetMultiPlayer
/* ------------------------------------------------------------------------------------ */
bool CCommonData::GetMultiPlayer()
{
	return (multiplayer && CCD->GetNetwork());
}


/* ------------------------------------------------------------------------------------ */
//	GetskContext
/* ------------------------------------------------------------------------------------ */
skExecutableContext CCommonData::GetskContext()
{
	skExecutableContext ctxt(&interpreter);
	return ctxt;
}

/* ------------------------------------------------------------------------------------ */
//	AddScriptedObject
/* ------------------------------------------------------------------------------------ */
void CCommonData::AddScriptedObject(const char *objectName, skRValue Object)
{
	interpreter.addGlobalVariable(objectName, Object);
}

/* ------------------------------------------------------------------------------------ */
//	RemoveScriptedObject
/* ------------------------------------------------------------------------------------ */
void CCommonData::RemoveScriptedObject(const char *objectName)
{
	interpreter.removeGlobalVariable(objectName);
}


/* ------------------------------------------------------------------------------------ */
//	ShutdownCommon
//
//	Shut down all the common classes, like the game engine and such,
//	..in preparation for exiting the game.
/* ------------------------------------------------------------------------------------ */
void CCommonData::ShutdownCommon()
{
	//	Shut down the AVI system
	ReportError("Shutting Down AVIFile Video Subsystem...", false);
	AVIFileExit();

	//	Clean up all the various subsystems before exiting.  Note that you
	//	..must delete all the components BEFORE you delete the engine, as
	//	..the engine is used to free sounds, etc. in many of the components.
	ReportError("Shutting Down Network Manager Subsystem...", false);
	SAFE_DELETE(theNetPlayerMgr);

	ReportError("Shutting Down Collision Manager Subsystem...", false);
	SAFE_DELETE(theCollider);

	ReportError("Shutting Down RF Menu Manager Subsystem...", false);
	SAFE_DELETE(theMenu);

	ReportError("Shutting Down CD Audio Manager Subsystem...", false);
	SAFE_DELETE(theCDPlayer);

	ReportError("Shutting Down Midi Audio Manager Subsystem...", false);
	SAFE_DELETE(theMIDIPlayer);

	ReportError("Shutting Down Audio Manager Subsystem...", false);
	SAFE_DELETE(theAudioManager);

	ReportError("Shutting Down User Input Manager Subsystem...", false);
	SAFE_DELETE(theUserInput);

	ReportError("Shutting Down Camera Manager Subsystem...", false);
	SAFE_DELETE(theCameraManager);

	ReportError("Shutting Down Graphics Subsystem...", false);
	SAFE_DELETE(theGameEngine);

	ReportError("Restoring Mouse Cursor...", false);
	ShowCursor(TRUE);

	m_MouseControl = true;

	CloseJoysticks(); // pwx
}

/* ------------------------------------------------------------------------------------ */
//	InitializeLevel
//
//	This function loads a level, then initializes all the entity
//	..handler classes that deal with the various level-specific
//	..entities.
/* ------------------------------------------------------------------------------------ */
int CCommonData::InitializeLevel(const char *szLevelName)
{
	kAudibleRadius = 360.0f;

	char szTemp[256];
	sprintf(szTemp, "Initializing Level: %s", szLevelName);
	ReportError(szTemp, false);

	ReportError("Configuring Camera Defaults...", false);
	theCameraManager->Defaults();

	if(theGameEngine->LoadLevel(szLevelName) == FALSE)
	{
		char szBug[200];
		sprintf(szBug, "[ERROR] File %s - Line %d: Level [%s] failed to load!",
				__FILE__, __LINE__, szLevelName);
		theGameEngine->ReportError(szBug, false);
		return -3;
	}

	//	Ok, we have to initialize the entity registry, so that all the
	//	..other components can register the entities they are responsible
	//	..for
	ReportError("Initializing Entity Registry...", false);
	theRegistry = new CEntityRegistry();
	if(theRegistry == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to initialize entity registry!", false);
		return -30;
	}

	ReportError("Initializing Terrain Manager...", false);
	theTerrainMgr = new qxTerrainMgr();
	if(theTerrainMgr == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create qxTerrainMgr handler", false);
		return -26;
	}

	ReportError("Initializing Effects Manager...", false);
	theEffect = new EffManager();
	if(theEffect == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create Effect handler", false);
		return -26;
	}

	//	The Actor Manager is up next
	ReportError("Initializing Actor Manager...", false);
	theActorManager = new CActorManager();
	if(theActorManager == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create actor manager", false);
		return -91;
	}

	for(int k=0; k<5; k++)
		LODdistance[k] = 0;

	LODAnimation = false;

	ReportError("Parsing Environment Setup Entity...", false);

	geEntity_EntitySet* lEntitySet;
	geEntity* lEntity;

	lEntitySet = geWorld_GetEntitySet(CCD->World(), "EnvironmentSetup");
	if(lEntitySet != NULL)
	{
		lEntity = geEntity_EntitySetGetNextEntity(lEntitySet, NULL);

		if(lEntity)
		{
			EnvironmentSetup *theState = (EnvironmentSetup*)geEntity_GetUserData(lEntity);

			LODdistance[0] = theState->LODdistance1;
			LODdistance[1] = theState->LODdistance2;
			LODdistance[2] = theState->LODdistance3;
			LODdistance[3] = theState->LODdistance4;
			LODdistance[4] = theState->LODdistance5;
			LODAnimation = theState->LODAnimation;
		}
	}

	//	Then the Model Manager comes into exitence
	ReportError("Initializing Model Manager...", false);

	theModelManager = new CModelManager();
	if(theModelManager == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create model manager", false);
		return -92;
	}

	//	Create the player avatar and load it.
	ReportError("Creating Player Avatar...", false);

	thePlayer = new CPlayer();
	if(thePlayer == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create player avatar!", false);
		return -4;
	}

	ReportError("Loading Player Avatar...", false);
	if(thePlayer->LoadAvatar(m_PlayerAvatar, m_PlayerName) != RGF_SUCCESS)
	{
		theGameEngine->ReportError("[ERROR] Failed to load player avatar actor!", false);
		return -5;
	}

	//	Set up the heads-up display (HUD) for the game
	ReportError("Initializing HUD...", false);
	theHUD = new CHeadsUpDisplay();
	if(theHUD == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create HUD class", false);
		return -3;
	}

	ReportError("Initializing Damage Subsystem...", false);
	theDamage = new CDamage();
	if(theDamage == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create Damage subsystem", false);
		return -100;
	}

	//	Now various other level-specific items
	ReportError("Initializing FixedCamera Manager Subsystem...", false);
	theFixedCamera = new CFixedCamera();
	if(theFixedCamera == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create FixedCamera handling class", false);
		return -11;
	}

	ReportError("Loading Player Configuration...", false);
	thePlayer->LoadConfiguration();

	//	Set up automatic door handling
	ReportError("Initializing Automatic Door Manager Subsystem...", false);
	theAutoDoors = new CAutoDoors();
	if(theAutoDoors == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create automatic door handling class", false);
		return -6;
	}

	//	Start up the moving platform class
	ReportError("Initializing Moving Platform Manager Subsystem...", false);
	thePlatforms = new CMovingPlatforms();
	if(thePlatforms == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create moving platform handling class", false);
		return -8;
	}

	//	Activate teleports
	ReportError("Initializing Teleport Manager Subsystem...", false);
	theTeleports = new CTeleporter();
	if(theTeleports == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create teleporter handling class", false);
		return -10;
	}

	//	Load up the morphing field effects
	ReportError("Initializing MorphingField Effects Manager Subsystem...", false);
	theFields = new CMorphingFields();
	if(theFields == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create morphing field handling class", false);
		return -11;
	}

	//	Load and start up all 3D audio sources
	ReportError("Initializing 3d AudioSource Manager Subsystem...", false);
	the3DAudio = new C3DAudioSource();
	if(the3DAudio == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create 3D audio source handling class", false);
		return -12;
	}

	//	Fire up particle system handling
	ReportError("Initializing Particle Effects Manager Subsystem...", false);
	theParticles = new CParticleSystem();
	if(theParticles == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create particle system handler", false);
		return -13;
	}

	//	Load up all the static entities (props) for the level.
	ReportError("Initializing Static Entity Props Subsystem...", false);
	theProps = new CStaticEntity();
	if(theProps == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create static entity handler", false);
		return -14;
	}

	//	Load up all the static meshes for the level.
	ReportError("Initializing Static Mesh Subsystem...", false);
	theMeshes = new CStaticMesh();
	if(theMeshes == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create static mesh handler", false);
		return -14;
	}

	//	Set up soundtrack toggles for the level
	ReportError("Initializing Soundtrack Toggle Subsystem...", false);
	theSTToggles = new CSoundtrackToggle();
	if(theSTToggles == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create soundtrack toggle handler", false);
		return -15;
	}

	//	Fire up the streaming audio handler
	ReportError("Initializing Streaming Audio Manager Subsystem...", false);
	theStreams = new CAudioStream();
	if(theStreams == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create streaming audio handler", false);
		return -16;
	}

	//	and the video texture handler component
	ReportError("Initializing Video Texture Manager Subsystem...", false);
	theVidText = new CVideoTexture();
	if(theVidText == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create video texture handler", false);
		return -17;
	}

	// corona effect
	ReportError("Initializing Corona Manager Subsystem...", false);
	theCorona = new CCorona();
	if(theCorona == NULL)
    {
		theGameEngine->ReportError("[ERROR] Failed to create Corona handler", false);
		return -20;
    }

	// dynamic lights
	ReportError("Initializing Dynamic Light Manager Subsystem...", false);
	theDynalite = new CDynalite();
	if(theDynalite == NULL)
    {
		theGameEngine->ReportError("[ERROR] Failed to create Dynalite handler", false);
		return -21;
    }

	ReportError("Initializing ElectricBolt Manager Subsystem...", false);
	theElectric = new CElectric();
	if(theElectric == NULL)
    {
		theGameEngine->ReportError("[ERROR] Failed to create Electric handler", false);
		return -22;
    }

	ReportError("Initializing Procedural Texture Manager Subsystem...", false);
	theProcTexture = new CProcedural();
	if(theProcTexture == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create Procedural Texture handler", false);
		return -23;
	}

	//	Create the in-memory path database for this level.
	ReportError("Initializing Path Database...", false);
	thePathDatabase = new CPathDatabase();
	if(thePathDatabase == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create Path database", false);
		return -24;
	}

	//	Set up path followers to bind entities to motion paths
	ReportError("Initializing Path Followers...", false);
	theFollower = new CPathFollower();
	if(theFollower == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create Path follower", false);
		return -25;
	}

	//	Initialize Ralph Deane's Effects Manager and the various effects
	//	..that it handles.  Thanks for the great conribution, Ralph!
	ReportError("Initializing Rain Effects Manager...", false);
	theRain = new CRain();
	if(theRain == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create Rain handler", false);
		return -27;
	}

	ReportError("Initializing Spout Effects Manager...", false);
	theSpout = new CSpout();
	if(theSpout == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create Spout handler", false);
		return -28;
	}

	ReportError("Initializing ActorSpout Effects Manager...", false);
	theActorSpout = new CActorSpout();
	if(theActorSpout == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create ActorSpout handler", false);
		return -28;
	}

	ReportError("Initializing Floating Particle Effects Manager...", false);
	theFloat = new CFloat();
	if(theFloat == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create Floating Particle handler", false);
		return -29;
	}

	ReportError("Initializing eChaos Effects Manager...", false);
	theChaos = new Chaos();
	if(theChaos == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create Chaos handler", false);
		return -30;
	}

	ReportError("Initializing Flame Effects Manager...", false);
	theFlame = new CFlame();
	if(theFlame == NULL)
    {
		theGameEngine->ReportError("[ERROR] Failed to create Flame handler", false);
		return -32;
    }

	ReportError("Initializing ScriptPoint Manager Subsystem...", false);
	theScriptPoints = new CScriptPoint();
	if(theScriptPoints == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create ScriptPoint handler", false);
		return -40;
	}

	ReportError("Initializing Pawn Manager Subsystem...", false);
	thePawn = new CPawn();
	if(thePawn == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create Pawn handler", false);
		return -40;
	}

	ReportError("Initializing ChangeAttribute Manager Subsystem...", false);
	theChangeAttribute = new CChangeAttribute();
	if(theChangeAttribute == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create ChangeAttribute handler", false);
		return -40;
	}

	ReportError("Initializing Countdown Manager Subsystem...", false);
	theCountDownTimer = new CCountDown();
	if(theCountDownTimer == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create CountDownTimer handler", false);
		return -40;
	}

	//	Set up triggers
	ReportError("Initializing Trigger Manager Subsystem...", false);
	theTriggers = new CTriggers();
	if(theTriggers == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create trigger handling class", false);
		return -41;
	}

	//	Set up logic gates
	ReportError("Initializing LogicHandler Subsystem...", false);
	theLogic = new CLogic();
	if(theLogic == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create Logic handling class",
			false);
		return -42;
	}

	// Message
	ReportError("Initializing Message Manager Subsystem...", false);
	theMessage = new CMessage();
	if(theMessage == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create Message handling class", false);
		return -7;
	}

	ReportError("Initializing Effect Manager Subsystem...", false);
	thePreEffect = new CPreEffect();
	if(thePreEffect == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create Effect subsystem", false);
		return -100;
	}

	// Weapon
	ReportError("Initializing Weapon Manager Subsystem...", false);
	theWeapon = new CWeapon();
	if(theWeapon == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create Weapon handling class", false);
		return -7;
	}

	ReportError("Initializing FirePoint Manager Subsystem...", false);
	theFirePoint = new CFirePoint();
	if(theFirePoint == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create FirePoint handling class", false);
		return -7;
	}

	ReportError("Initializing Flipbook Manager Subsystem...", false);
	theFlipBook = new CFlipBook();
	if(theFlipBook == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create FlipBook handling class", false);
		return -7;
	}

	// pwx
	ReportError("Initializing AreaCheck Manager Subsystem...", false);
	theAreaCheck = new CAreaChecker();
	if(theAreaCheck == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create AreaCheck handling class", false);
		return -7;
	}
	// pwx

	ReportError("Initializing Foliage Manager Subsystem...", false);
	theFoliage = new CFoliage();
	if(theFoliage == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create Foliage handling class", false);
		return -7;
	}

	ReportError("Initializing Tree Manager Subsystem...", false);
	theFlipTree = new CFlipTree();
	if(theFlipTree == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create Tree handling class", false);
		return -7;
	}

	ReportError("Initializing PWXImage Manager Subsystem...", false);
	thePWXImage = new PWXImageManager();
	if(thePWXImage == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create PWX image handling class", false);
		return -7;
	}

	ReportError("Initializing Shadow Manager Subsystem...", false);
	thePolyShadow = new CPolyShadow();
	if(thePolyShadow == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create Poly Shadow handling class", false);
		return -7;
	}

	// Decal
	ReportError("Initializing Decal Manager Subsystem...", false);
	theDecal = new CDecal();
	if(theDecal == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create Decal handling class", false);
		return -7;
	}

	ReportError("Initializing WallDecal Manager Subsystem...", false);
	theWallDecal = new CWallDecal();
	if(theWallDecal == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create WallDecal handling class", false);
		return -7;
	}

	ReportError("Initializing LevelController Manager Subsystem...", false);
	theLevelController = new CLevelController();
	if(theLevelController == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create LevelController handling class", false);
		return -7;
	}

	// Attribute
	ReportError("Initializing Attribute Manager Subsystem...", false);
	theAttribute = new CAttribute();
	if(theAttribute == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create Attribute handling class", false);
		return -7;
	}

	ReportError("Initializing Explosion Manager Subsystem...", false);
	theExplosion = new CExplosionInit();
	if(theExplosion == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create Explosion Manager subsystem", false);
		return -100;
	}

	ReportError("Initializing Explosion Subsystem...", false);
	theCExplosion = new CExplosion();
	if(theCExplosion == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create Explosion subsystem", false);
		return -100;
	}

	ReportError("Initializing ChangeLevel Manager Subsystem...", false);
	theChangeLevel = new CChangeLevel();
	if(theChangeLevel == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create ChangeLevel handling class", false);
		return -7;
	}

	ReportError("Initializing ScreenShake Subsystem...", false);
	theShake = new CShake();
	if(theShake == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create ScreenShake handler", false);
		return -40;
	}

	ReportError("Initializing ViewSwitch Subsystem...", false);
	theViewSwitch = new CViewSwitch();
	if(theViewSwitch == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create ViewSwitch handler", false);
		return -40;
	}

	ReportError("Initializing Inventory Subsystem...", false);
	theInventory = new CInventory();
	if(theInventory == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create Inventory handler", false);
		return -40;
	}

	ReportError("Initializing Liquid Subsystem...", false);
	theLiquid = new CLiquid();
	if(theLiquid == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create Liquid handler", false);
		return -40;
	}

	ReportError("Initializing Overlay Subsystem...", false);
	theOverlay = new COverlay();
	if(theOverlay == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create Overlay handler", false);
		return -40;
	}

	ReportError("Initializing TextureMorph Subsystem...", false);
	theMorph = new CMorph();
	if(theMorph == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create TextureMorph", false);
		return -7; //or something else
	}

	ReportError("Initializing CutScene Subsystem...", false);
	theCutScene = new CCutScene();
	if(theCutScene == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create CutScene", false);
		return -7; //or something else
	}

	ReportError("Initializing ActorMaterial Subsystem...", false);
	theActMaterial = new CActMaterial();
	if(theActMaterial == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create CActMaterial", false);
		return -7; //or something else
	}

	ReportError("Initializing Armour Subsystem...", false);
	theArmour = new CArmour();
	if(theArmour == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create CArmour", false);
		return -7; //or something else
	}

	ReportError("Initializing LiftBelt Manager...", false);
	theLiftBelt = new CLiftBelt();
	if(theLiftBelt == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create CLiftBelt", false);
		return -7; //or something else
	}

	ReportError("Initializing CDSpotlight Manager...", false);
	theCDSpot = new CDSpotLight();
	if(theCDSpot == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create CDSpotLight", false);
		return -7; //or something else
	}

	ReportError("Initializing WindGenerator...", false);
	theWindGenerator = new CWindGenerator();
	if(theWindGenerator == NULL)
	{
		theGameEngine->ReportError("[ERROR] Failed to create CWindGenerator", false);
		return -7; //or something else
	}

	ReportError("Initializing Fonts...", false);
	theMenu->LoadWBitmap();

	ReportError("Preparing to Launch Game...", false);

	//	All level classes up! Let's **PLAY**
	return 0;
}

/* ------------------------------------------------------------------------------------ */
//	ShutdownLevel
//
//	This function deletes all the level-related entities in preparation
//	..either to load a new level or exit the game.
/* ------------------------------------------------------------------------------------ */
void CCommonData::ShutdownLevel()
{
	if(theNetPlayerMgr != NULL)
	{
		theNetPlayerMgr->DeletePlayers();
	}

	if(theMenu)
		theMenu->UnLoadWBitmap();

	SAFE_DELETE(theCDSpot);
	SAFE_DELETE(theLiftBelt);
	SAFE_DELETE(theArmour);
	SAFE_DELETE(theActMaterial);
	SAFE_DELETE(theCutScene);
	SAFE_DELETE(theMorph);
	SAFE_DELETE(theOverlay);
	SAFE_DELETE(theLiquid);
	SAFE_DELETE(theInventory);
	SAFE_DELETE(theViewSwitch);
	SAFE_DELETE(theShake);
	SAFE_DELETE(theChangeLevel);
	SAFE_DELETE(theCExplosion);
	SAFE_DELETE(theExplosion);
	SAFE_DELETE(theAttribute);
	SAFE_DELETE(theDecal);
	SAFE_DELETE(theWallDecal);
	SAFE_DELETE(theLevelController);
	SAFE_DELETE(theFlipBook);
	SAFE_DELETE(theFoliage);
	SAFE_DELETE(theFlipTree);
	SAFE_DELETE(thePWXImage);
	SAFE_DELETE(thePolyShadow);
	SAFE_DELETE(theAreaCheck);
	SAFE_DELETE(theFirePoint);
	SAFE_DELETE(theWeapon);
	SAFE_DELETE(thePreEffect);
	SAFE_DELETE(theMessage);
	SAFE_DELETE(theScriptPoints);
	SAFE_DELETE(thePawn);
	SAFE_DELETE(theChangeAttribute);
	SAFE_DELETE(theCountDownTimer);
	SAFE_DELETE(theLogic);
	SAFE_DELETE(theTriggers);
	SAFE_DELETE(theFlame);
	SAFE_DELETE(theChaos);
	SAFE_DELETE(theRain);
	SAFE_DELETE(theActorSpout);
	SAFE_DELETE(theSpout);
	SAFE_DELETE(theFloat);
	SAFE_DELETE(theEffect);
	SAFE_DELETE(theFollower);
	SAFE_DELETE(thePathDatabase);
	SAFE_DELETE(theProcTexture);
	SAFE_DELETE(theCorona);
	SAFE_DELETE(theDynalite);
	SAFE_DELETE(theElectric);
	SAFE_DELETE(theVidText);
	SAFE_DELETE(theStreams);
	SAFE_DELETE(theSTToggles);
	SAFE_DELETE(theProps);
	SAFE_DELETE(theMeshes);
	SAFE_DELETE(theParticles);
	SAFE_DELETE(the3DAudio);
	SAFE_DELETE(thePlatforms);
	SAFE_DELETE(theFixedCamera);
	SAFE_DELETE(theAutoDoors);
	SAFE_DELETE(thePlayer);
	SAFE_DELETE(theTeleports);
	SAFE_DELETE(theFields);
	SAFE_DELETE(theHUD);
	SAFE_DELETE(theModelManager);
	SAFE_DELETE(theActorManager);
	SAFE_DELETE(theRegistry);
	SAFE_DELETE(theTerrainMgr);
	SAFE_DELETE(theEffect);
	SAFE_DELETE(theDamage);
	SAFE_DELETE(theWindGenerator);
}

/* ------------------------------------------------------------------------------------ */
//	HandleMenuInput
//
//	Read the keyboard and handle MENU REQUESTS AND ACTIONS.
/* ------------------------------------------------------------------------------------ */
bool CCommonData::HandleMenuInput()
{
	CCD->ReportError("Menu input called with no menu code", false);
	return true;
}

/* ------------------------------------------------------------------------------------ */
//	HandleGameInput
//
//	Read the keyboard and process all user input in the GAME LOOP
/* ------------------------------------------------------------------------------------ */
bool CCommonData::HandleGameInput()
{
	bool bKeepPlaying = true;					// Assume we keep playing
	bool frun, fhud, flook, fcamera;			// Key alteration states
	int nInputEvent;									// Input event ID
	int keyrotate = 0; // update #2
	bool bPlayerMoved = false;
	static bool FrameRateShowing = false;
	geExtBox CBox;
	geVec3d CPosition;
	int ch;

	if(Paused || KeyPaused)
		return bKeepPlaying;

	if(thePlayer->GetDying())
		return bKeepPlaying;

	frun = flook = fcamera = false;				// Clear modifiers
	bool screen, jump, run, crouch, zoom, light, save, load, use, inv;
	static int nLoopTimer = timeGetTime();	// Loop timer
	bool fconsole = false;
	bool fdrop = false;
	bool freload = false;
	frun = fhud = flook = fcamera = screen = jump = false;				// Clear modifiers
	run = crouch = zoom = light = save = load = use = inv = false;

	// Check for input from the user, building a keystroke queue,
	nInputEvent = theUserInput->GetFirstInput();
	thePlayer->Moving(MOVEIDLE);				// 03/22/2000 eaa3 Ralph Deane fix added
	thePlayer->SetSlideWalk(MOVEIDLE);
	thePlayer->SetJumping(GE_FALSE);

	int nFoo = 0;
	while(nInputEvent != RGF_NO_INPUT)
	{
		switch(nInputEvent)
		{
		case RGF_K_EXIT:
			bKeepPlaying = false;				// Exit game
			CCD->ActorManager()->GetBoundingBox(CCD->Player()->GetActor(), &CBox);

			for(ch=0; ch<50; ch++)
			{
				CCD->ActorManager()->GetPosition(CCD->Player()->GetActor(), &CPosition);

				if(CCD->Collision()->CheckSolid(&CPosition, &CBox, CCD->Player()->GetActor()))
				{
					CCD->Player()->AddPosition();
					break;
				}

				CCD->Player()->Backtrack();
			}
			return bKeepPlaying;
			break;
		case RGF_K_FORWARD:
			if(thePlayer->GetMoving() == MOVESLIDELEFT || thePlayer->GetMoving() == MOVESLIDERIGHT)
				thePlayer->SetSlideWalk(thePlayer->GetMoving());
			thePlayer->Moving(MOVEWALKFORWARD);
			break;
		case RGF_K_BACKWARD:
			if(thePlayer->GetMoving() == MOVESLIDELEFT || thePlayer->GetMoving() == MOVESLIDERIGHT)
				thePlayer->SetSlideWalk(thePlayer->GetMoving());
			thePlayer->Moving(MOVEWALKBACKWARD);
			break;
		case RGF_K_RUN:
			run = true;
			if(!runkey)
			{
				thePlayer->FlipRunning();
				runkey = true;
			}
			break;
		case RGF_K_CAMERA:
			fcamera = true;
			break;
		case RGF_K_LOOKMODE:
			flook = true;
			break;
		case RGF_K_USE:
			use = true;
			if(!usekey)
			{
				thePlayer->UseItem();
				usekey = true;
			}
			break;
		case RGF_K_INVENTORY:
			if(!invkey)
			{
				theInventory->SetActive(true);
				KeyPaused = true;
			}
			break;
		case RGF_K_HUD:
			fconsole = true;
			if(!consolekey)
			{
				consoleflag = !consoleflag;

				if(!consoleflag)
				{
					CCD->HUD()->Activate();
				}
				else
				{
					CCD->HUD()->Deactivate();
				}
				consolekey = true;
			}
			break;
		case RGF_K_JUMP:
			jump = true;
			if(!jumpkey)
			{
				thePlayer->SetJumping(GE_TRUE);	// Start player jumping
				jumpkey = true;
			}
			break;
		case RGF_K_LEFT:
			if(thePlayer->GetMoving() == MOVEWALKFORWARD || thePlayer->GetMoving() == MOVEWALKBACKWARD)
				thePlayer->SetSlideWalk(thePlayer->GetMoving());
			thePlayer->Moving(MOVESLIDELEFT);
			break;
		case RGF_K_RIGHT:
			if(thePlayer->GetMoving() == MOVEWALKFORWARD || thePlayer->GetMoving() == MOVEWALKBACKWARD)
				thePlayer->SetSlideWalk(thePlayer->GetMoving());
			thePlayer->Moving(MOVESLIDERIGHT);
			break;
		case RGF_K_LOOKUP:
			if(fcamera)
				theCameraManager->CameraRotX(GE_FALSE); // Mode
			else // update #2
				keyrotate |= 1;
			break;
		case RGF_K_TURN_LEFT:
			if(fcamera)
				theCameraManager->CameraRotY(GE_FALSE, 5.0f); // Mode
			else // update #2
				keyrotate |= 2;
			break;
		case RGF_K_TURN_RIGHT:
			if(fcamera)
				theCameraManager->CameraRotY(GE_TRUE, 5.0f); // Mode
			else // update #2
				keyrotate |= 4;
			break;
		case RGF_K_LOOKDOWN:
			if(fcamera)
				theCameraManager->CameraRotX(GE_TRUE); // Mode
			else // update #2
				keyrotate |= 8;
			break;
		case RGF_K_ZOOM_WEAPON:
			zoom = true;
			if(!zoomkey)
			{
				theCameraManager->SetZoom(true);
				zoomkey = true;
			}
			break;
		case RGF_K_DROP:
			fdrop = true;
			if(!dropkey)
			{
				theWeapon->DropWeapon();
				dropkey = true;
			}
			break;
		case RGF_K_RELOAD:
			freload = true;
			if(!reloadkey)
			{
				theWeapon->KeyReload();
				reloadkey = true;
			}
			break;
		case RGF_K_POWERUP:
			theLiftBelt->ChangeLift(true);
			break;
		case RGF_K_POWERDWN:
			theLiftBelt->ChangeLift(false);
			break;
		case RGF_K_HOLSTER_WEAPON:
			theWeapon->Holster();
			break;
		case RGF_K_WEAPON_1:
			theWeapon->SetWeapon(0);
			break;
		case RGF_K_WEAPON_2:
			theWeapon->SetWeapon(1);
			break;
		case RGF_K_WEAPON_3:
			theWeapon->SetWeapon(2);
			break;
		case RGF_K_WEAPON_4:
			theWeapon->SetWeapon(3);
			break;
		case RGF_K_WEAPON_5:
			theWeapon->SetWeapon(4);
			break;
		case RGF_K_WEAPON_6:
			theWeapon->SetWeapon(5);
			break;
		case RGF_K_WEAPON_7:
			theWeapon->SetWeapon(6);
			break;
		case RGF_K_WEAPON_8:
			theWeapon->SetWeapon(7);
			break;
		case RGF_K_WEAPON_9:
			theWeapon->SetWeapon(8);
			break;
		case RGF_K_WEAPON_0:
			theWeapon->SetWeapon(9);
			break;
		case RGF_K_FIRE:
			theWeapon->Attack(GE_FALSE);
			break;
		case RGF_K_ALTFIRE:
			theWeapon->Attack(GE_TRUE);
			break;
		case RGF_K_FIRST_PERSON_VIEW:
			if(CCD->CameraManager()->GetSwitchAllowed() && CCD->CameraManager()->GetSwitch1st())
				thePlayer->SwitchCamera(0);
			break;
		case RGF_K_THIRD_PERSON_VIEW:
			if(CCD->CameraManager()->GetSwitchAllowed() && CCD->CameraManager()->GetSwitch3rd())
				thePlayer->SwitchCamera(1);
			break;
		case RGF_K_ISO_VIEW:
			if(CCD->CameraManager()->GetSwitchAllowed() && CCD->CameraManager()->GetSwitchIso())
				thePlayer->SwitchCamera(2);
			break;

		case RGF_K_SCRNSHOT:
			screen = true;
			if(theMenu->GetShot() == false)
			{
				theMenu->SetShot(true);
				theMenu->ScreenShot();
			}
			break;
		case RGF_K_CAMERA_RESET:
			CCD->CameraManager()->ResetCamera();
			break;
		case RGF_K_ZOOM_OUT:
			CCD->CameraManager()->ChangeDistance(GE_TRUE, 5.0f);
			break;
		case RGF_K_ZOOM_IN:
			CCD->CameraManager()->ChangeDistance(GE_FALSE, 5.0f);
			break;
		case RGF_K_CROUCH:
			crouch = true;
			if(!crouchkey)
			{
				thePlayer->FlipCrouch();
				crouchkey = true;
			}
			break;
		case RGF_K_LIGHT:
			light = true;
			if(!lightkey)
			{
				thePlayer->FlipLight();
				lightkey = true;
			}
			break;
		case RGF_K_QUICKSAVE:
			{
				save = true;
				if(!savekey)
				{

					FILE *outFD = CCD->OpenRFFile(kSavegameFile, "savegame.rgf", "wb");
					if(outFD == NULL)
					{
						CCD->ReportError("Failed to create savegame file!", false);
						break;
					}
					theGameEngine->SaveTo(outFD);
					theMenu->SaveTo(outFD, false);
					thePlayer->SaveTo(outFD);
					theAutoDoors->SaveTo(outFD, false);
					thePlatforms->SaveTo(outFD, false);
					theProps->SaveTo(outFD, false);
					theMeshes->SaveTo(outFD, false);
					theTeleports->SaveTo(outFD, false);
					theFields->SaveTo(outFD);
					theMIDIPlayer->SaveTo(outFD);
					theCDPlayer->SaveTo(outFD);
					theTriggers->SaveTo(outFD, false);
					theLogic->SaveTo(outFD, false);
					theAttribute->SaveTo(outFD, false);
					theDamage->SaveTo(outFD, false);
					theCameraManager->SaveTo(outFD);
					theWeapon->SaveTo(outFD);
					theElectric->SaveTo(outFD, false);
					theCountDownTimer->SaveTo(outFD, false);
					theChangeAttribute->SaveTo(outFD, false);
					theChangeLevel->SaveTo(outFD, false);
					theActMaterial->SaveTo(outFD, false);
					theModelManager->SaveTo(outFD, false);
					SaveTo(outFD);
					fclose(outFD);
					savekey = true;
					saving = true;
				}
			}
			break;
		case RGF_K_QUICKLOAD:
			{
				load = true;
				if(!loadkey)
				{
					FILE *inFD = CCD->OpenRFFile(kSavegameFile, "savegame.rgf", "rb");

					if(inFD == NULL)
					{
						theGameEngine->ReportError("No savegame.rgf file to restore", false);
						break;
					}

					geRect 	 M_CameraRect;
					geCamera *M_Camera;

					M_CameraRect.Left = 0;
					M_CameraRect.Right = CCD->Engine()->Width() - 1;
					M_CameraRect.Top = 0;
					M_CameraRect.Bottom = CCD->Engine()->Height() - 1;
					M_Camera = geCamera_Create(2.0f, &M_CameraRect);

					if(HasFocus)
					{
						geEngine_BeginFrame(CCD->Engine()->Engine(), M_Camera, GE_FALSE);
						int width = CCD->MenuManager()->FontWidth(CCD->MenuManager()->GetLoadFont(), CCD->MenuManager()->GetLoadmsg());
						int height = CCD->MenuManager()->FontHeight(CCD->MenuManager()->GetLoadFont());
						int xoffset = (CCD->Engine()->Width()-width)/2;
						int yoffset = (CCD->Engine()->Height()-height)/2;
						CCD->MenuManager()->FontRect(CCD->MenuManager()->GetLoadmsg(), CCD->MenuManager()->GetLoadFont(), xoffset, yoffset);
						geEngine_EndFrame(CCD->Engine()->Engine());
						geEngine_BeginFrame(CCD->Engine()->Engine(), M_Camera, GE_FALSE);
						CCD->MenuManager()->FontRect(CCD->MenuManager()->GetLoadmsg(), CCD->MenuManager()->GetLoadFont(), xoffset, yoffset);
						geEngine_EndFrame(CCD->Engine()->Engine());
					}

					geCamera_Destroy(&M_Camera);
					// Restoring will do a level change to the level that
					// ..is in the savegame file
					ShutdownLevel();

					if(theCDPlayer)
						theCDPlayer->Stop();

					if(theMIDIPlayer)
						theMIDIPlayer->Stop();

					theGameEngine->RestoreFrom(inFD);
					theMenu->RestoreFrom(inFD, false);
					InitializeLevel(theGameEngine->LevelName());
					TerrainMgr()->Init(); // Pickles
					thePlayer->RestoreFrom(inFD);
					theTerrainMgr->Frame(); // Pickles
					theAutoDoors->RestoreFrom(inFD, false);
					thePlatforms->RestoreFrom(inFD, false);
					theProps->RestoreFrom(inFD, false);
					theMeshes->RestoreFrom(inFD, false);
					theTeleports->RestoreFrom(inFD, false);
					theFields->RestoreFrom(inFD);
					theMIDIPlayer->RestoreFrom(inFD);
					theCDPlayer->RestoreFrom(inFD);
					theTriggers->RestoreFrom(inFD, false);
					theLogic->RestoreFrom(inFD, false);
					theAttribute->RestoreFrom(inFD, false);
					theDamage->RestoreFrom(inFD, false);
					theCameraManager->RestoreFrom(inFD);
					theWeapon->RestoreFrom(inFD);
					theElectric->RestoreFrom(inFD, false);
					theCountDownTimer->RestoreFrom(inFD, false);
					theChangeAttribute->RestoreFrom(inFD, false);
					theChangeLevel->RestoreFrom(inFD, false);
					theActMaterial->RestoreFrom(inFD, false);
					theModelManager->RestoreFrom(inFD, false);
					RestoreFrom(inFD);
					fclose(inFD);
					loadkey = true;
				}
			}
			break;
		default:
			break;
		}

		nInputEvent = theUserInput->GetNextInput();
	}							// End input processing while()

	if(screen == false)
		theMenu->SetShot(false);

	if(!jump)
		jumpkey = false;

	if(!run)
		runkey = false;

	if(!crouch)
		crouchkey = false;

	if(!light)
		lightkey = false;

	if(!load)
		loadkey = false;

	if(!save)
		savekey = false;

	if(!use)
		usekey = false;

	if(!inv)
		invkey = false;
	if(!fconsole)
		consolekey = false;
	if(!fdrop)
		dropkey = false;

	if(!freload)
		reloadkey = false;
	if(!zoom)
	{
		theCameraManager->SetZoom(false);
		zoomkey = false;
	}

	if(!thePlayer->FirstPersonView())
		thePlayer->LookMode(flook);				// Set lookmode

	bPlayerMoved = thePlayer->DoMovements();	//Manage player "wanted" movements

	thePlayer->CheckKeyLook(keyrotate); // update #2

#ifdef _DEBUG
	   thePlayer->CheckMouseLook();
#else
	   if(m_MouseControl)
		   thePlayer->CheckMouseLook();
#endif

	thePlayer->ProcessMove(bPlayerMoved);

	return bKeepPlaying;
}


/* ------------------------------------------------------------------------------------ */
// SaveTo
/* ------------------------------------------------------------------------------------ */
int CCommonData::SaveTo(FILE *SaveFD)
{
	fwrite(&jumpkey,			sizeof(bool),		1,		SaveFD);
	fwrite(&runkey,				sizeof(bool),		1,		SaveFD);
	fwrite(&crouchkey,			sizeof(bool),		1,		SaveFD);
	fwrite(&lightkey,			sizeof(bool),		1,		SaveFD);
	fwrite(&m_NewLevel,			sizeof(char),		256,	SaveFD);
	fwrite(&m_SplashScreen,		sizeof(char),		256,	SaveFD);
	fwrite(&m_SplashAudio,		sizeof(char),		256,	SaveFD);
	fwrite(&m_CutScene,			sizeof(char),		256,	SaveFD);
	fwrite(&m_Message,			sizeof(char),		256,	SaveFD);
	fwrite(&m_Font,				sizeof(int),		1,		SaveFD);
	fwrite(&m_StartPointName,	sizeof(char),		256,	SaveFD);
	fwrite(&Offset,				sizeof(geVec3d),	1,		SaveFD);
	fwrite(&UseAngle,			sizeof(bool),		1,		SaveFD);
	fwrite(&KeepAttributes,		sizeof(bool),		1,		SaveFD);
	fwrite(&CurrentWeapon,		sizeof(int),		1,		SaveFD);
	fwrite(&Slot,				sizeof(int),		40,		SaveFD);
	fwrite(&theRotation,		sizeof(geVec3d),	1,		SaveFD);
	fwrite(&theTranslation,		sizeof(geVec3d),	1,		SaveFD);
	fwrite(&theRotate,			sizeof(geVec3d),	1,		SaveFD);
	fwrite(&m_defaultdistance,	sizeof(float),		1,		SaveFD);
	fwrite(&m_cameraX,			sizeof(float),		1,		SaveFD);
	fwrite(&m_cameraY,			sizeof(float),		1,		SaveFD);
	fwrite(&m_playerotation,	sizeof(geVec3d),	1,		SaveFD);
	fwrite(&ViewPoint,			sizeof(int),		1,		SaveFD);
	fwrite(&DifficultLevel,		sizeof(int),		1,		SaveFD);
	fwrite(&zoomkey,			sizeof(bool),		1,		SaveFD);
	fwrite(&usekey,				sizeof(bool),		1,		SaveFD);
	fwrite(&invkey,				sizeof(bool),		1,		SaveFD);
	fwrite(&dropkey,			sizeof(bool),		1,		SaveFD);
	fwrite(&reloadkey,			sizeof(bool),		1,		SaveFD);
	fwrite(&CSelect,			sizeof(bool),		1,		SaveFD);
	fwrite(&NSelect,			sizeof(bool),		1,		SaveFD);

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// RestoreFrom
/* ------------------------------------------------------------------------------------ */
int CCommonData::RestoreFrom(FILE *RestoreFD)
{
	fread(&jumpkey,				sizeof(bool),		1,		RestoreFD);
	fread(&runkey,				sizeof(bool),		1,		RestoreFD);
	fread(&crouchkey,			sizeof(bool),		1,		RestoreFD);
	fread(&lightkey,			sizeof(bool),		1,		RestoreFD);
	fread(&m_NewLevel,			sizeof(char),		256,	RestoreFD);
	fread(&m_SplashScreen,		sizeof(char),		256,	RestoreFD);
	fread(&m_SplashAudio,		sizeof(char),		256,	RestoreFD);
	fread(&m_CutScene,			sizeof(char),		256,	RestoreFD);
	fread(&m_Message,			sizeof(char),		256,	RestoreFD);
	fread(&m_Font,				sizeof(int),		1,		RestoreFD);
	fread(&m_StartPointName,	sizeof(char),		256,	RestoreFD);
	fread(&Offset,				sizeof(geVec3d),	1,		RestoreFD);
	fread(&UseAngle,			sizeof(bool),		1,		RestoreFD);
	fread(&KeepAttributes,		sizeof(bool),		1,		RestoreFD);
	fread(&CurrentWeapon,		sizeof(int),		1,		RestoreFD);
	fread(&Slot,				sizeof(int),		40,		RestoreFD);
	fread(&theRotation,			sizeof(geVec3d),	1,		RestoreFD);
	fread(&theTranslation,		sizeof(geVec3d),	1,		RestoreFD);
	fread(&theRotate,			sizeof(geVec3d),	1,		RestoreFD);
	fread(&m_defaultdistance,	sizeof(float),		1,		RestoreFD);
	fread(&m_cameraX,			sizeof(float),		1,		RestoreFD);
	fread(&m_cameraY,			sizeof(float),		1,		RestoreFD);
	fread(&m_playerotation,		sizeof(geVec3d),	1,		RestoreFD);
	fread(&ViewPoint,			sizeof(int),		1,		RestoreFD);
	fread(&DifficultLevel,		sizeof(int),		1,		RestoreFD);
	fread(&zoomkey,				sizeof(bool),		1,		RestoreFD);
	fread(&usekey,				sizeof(bool),		1,		RestoreFD);
	fread(&invkey,				sizeof(bool),		1,		RestoreFD);
	fread(&dropkey,				sizeof(bool),		1,		RestoreFD);
	fread(&reloadkey,			sizeof(bool),		1,		RestoreFD);
	fread(&CSelect,				sizeof(bool),		1,		RestoreFD);
	fread(&NSelect,				sizeof(bool),		1,		RestoreFD);

	theHUD->LoadConfiguration();
	theHUD->Activate();

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	DispatchTick
//
//	Calls the Tick() function for all components that require a
//	..time pulse for animation or world-checking purposes.
/* ------------------------------------------------------------------------------------ */
int CCommonData::DispatchTick()
{
	//	We are the Master Clock.  We will pass in the number of milliseconds
	//	..that have passed to each of the Tick() time functions in each
	//	..of the game components.
	dwTicksGoneBy = CCD->GetTimePassed_F();

	if(!HasFocus)
	{
		dwTicksGoneBy = 0.0f;
		return RGF_SUCCESS;
	}

	if(dwTicksGoneBy <= 0.0f)
		return RGF_SUCCESS;					// No time passed?  Ignore call!

	//	First, check the path follower so that any motion triggers get turned
	//	..on prior to tick()'ing entities that may need to know they've
	//	..been put into motion
	theFollower->Tick(dwTicksGoneBy);

	//	EVIL HACK!  You MUST check model motions before you check
	//	..actor motions or the system goes out of sync!  LOOK,
	//	..JUST DO IT!

	//	Give time to the model manager so animated world models move...

	theModelManager->Tick(dwTicksGoneBy);

	//	Next, advance time for all the actors since this'll affect the
	//	..various actor animations and could cause bounding box changes.

	//theActorManager->Tick(dwTicksGoneBy);

	theWindGenerator->Tick(dwTicksGoneBy);

	//	Ok, deal out time to everyone else.
	theNetPlayerMgr->Tick(dwTicksGoneBy);
	theAutoDoors->Tick(dwTicksGoneBy);		// Advance doors, if needed
	thePlatforms->Tick(dwTicksGoneBy);		// Advance platforms, if needed
	theTeleports->Tick(dwTicksGoneBy);		// Animate teleporter fields
	theFields->Tick(dwTicksGoneBy);			// Animate morphing fields
	theParticles->Tick(dwTicksGoneBy);		// Animate particle systems
	theProps->Tick(dwTicksGoneBy);			// Animate static models
	theMeshes->Tick(dwTicksGoneBy);
	theStreams->Tick(dwTicksGoneBy);		// Deal with streaming audio proxies
	theSTToggles->Tick(dwTicksGoneBy);		// Soundtrack toggles dealt with
	theVidText->Tick(dwTicksGoneBy);		// Video textures dealt with
	theCorona->Tick(dwTicksGoneBy);			// Update light coronas
	theDynalite->Tick(dwTicksGoneBy);		// Update dynamic lights
	theElectric->Tick(dwTicksGoneBy);		// Update electrical effects
	theProcTexture->Tick(dwTicksGoneBy);	// Update procedural textures
	theExplosion->Tick(dwTicksGoneBy);
	theEffect->Tick(dwTicksGoneBy);			// Time to Ralph Deane's Effects Manager
	theRain->Tick(dwTicksGoneBy);			// Time to Ralph Deane's Rain Effect
	theSpout->Tick(dwTicksGoneBy);			// Time to Ralph Deane's Spout Effect
	theActorSpout->Tick(dwTicksGoneBy);
	theMorph->Tick(dwTicksGoneBy);
	theActMaterial->Tick(dwTicksGoneBy);
	theFloat->Tick(dwTicksGoneBy);			// Time to Ralph Deane's Float Effect
	theChaos->Tick(dwTicksGoneBy);			// Time to Ralph Deane's Chaos Procedural
	theFlame->Tick(dwTicksGoneBy);			// Time to Ralph Deane's Flame Effect
	the3DAudio->Tick(dwTicksGoneBy);		// Time to 3D Audio Source
	theTriggers->Tick(dwTicksGoneBy);		// Time to Ralph Deane's Triggers
	theLogic->Tick(dwTicksGoneBy);			// Time to Ralph Deane's Logic
	theMessage->Tick(dwTicksGoneBy);
	theDecal->Tick(dwTicksGoneBy);
	theWallDecal->Tick(dwTicksGoneBy);
	theTerrainMgr->Frame();
	theWeapon->Tick(dwTicksGoneBy);
	theFirePoint->Tick(dwTicksGoneBy);
	theFlipBook->Tick(dwTicksGoneBy);
	theFoliage->Tick(dwTicksGoneBy);		// Pickles Jul 04
	theFlipTree->Tick(dwTicksGoneBy);		// Pickles Jul 04
	theAreaCheck->Tick(dwTicksGoneBy);		// Pickles pwx
	theAttribute->Tick(dwTicksGoneBy);
	theDamage->Tick(dwTicksGoneBy);
	theCExplosion->Tick(dwTicksGoneBy);
	thePawn->Tick(dwTicksGoneBy);
	theCountDownTimer->Tick(dwTicksGoneBy);
	theChangeAttribute->Tick(dwTicksGoneBy);
	theOverlay->Tick(dwTicksGoneBy);
	theLiftBelt->Tick(dwTicksGoneBy);
	theCDSpot->Tick(dwTicksGoneBy);
	theChangeLevel->Tick(dwTicksGoneBy);
	theShake->Tick(dwTicksGoneBy);
	theFixedCamera->Tick();
	theViewSwitch->Tick();
   	theLevelController->Tick(dwTicksGoneBy);	// Execute the LevelControllers
	theCameraManager->Tick(dwTicksGoneBy);

	//	Finally, now that everything has moved, update all audio
	theAudioManager->Tick(dwTicksGoneBy);	// Audio updates

	thePlayer->Tick(dwTicksGoneBy);

	theActorManager->Tick(dwTicksGoneBy);
	theHUD->Tick(dwTicksGoneBy);
	theCutScene->Tick(dwTicksGoneBy);

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// ProcessLevelChange
//
// Process a level change entity trigger.  This will shut down our
// ..current level and load the new one!
/* ------------------------------------------------------------------------------------ */
bool CCommonData::ProcessLevelChange()
{
	char szLevelFile[256];

	m_ChangeLevel = true;

	// Kill the current soundtrack.
	if(theMIDIPlayer)
		theMIDIPlayer->Stop();		// Shut down MIDI, if any

	if(theCDPlayer)
		theCDPlayer->Stop();		// Shut down CD Audio, if any

	CCD->AudioStreams()->StopAll();

	// Uh-oh, time to change levels!  This is somewhat messy but
	// ..hopefully easy to understand.  We shut down the current level
	// ..and load the new one here.

	if(!EffectC_IsStringNull(m_NewLevel))
		strcpy(szLevelFile, m_NewLevel);	// Save off before...

	if(Logging)
	{
		char szDebug[512];
		sprintf(szDebug, "[INFO] Attempting changelevel to level '%s'", szLevelFile);
		CCD->ReportError(szDebug, false);
	}

	// Save off player attributes during level change..
	thePlayer->SaveAttributes("temp.bin");
	thePlayer->SaveAttributesAscii("attributes.txt");

	//	Check for a cut scene to play..
	if(!EffectC_IsStringNull(m_CutScene))
	{
		Play(m_CutScene, 0, 0, true);
	}

	// Numero uno, display the level-changing splash screen.
	if(!EffectC_IsStringNull(m_SplashScreen))
	{
		std::string File = m_SplashScreen;
		MakeLower(File);

		if(File.find(".bmp") == -1)
		{
			CIniFile AttrFile;
			AttrFile.SetPath("splash.ini");

			if(AttrFile.ReadFile())
			{
				std::string KeyName = AttrFile.FindFirstKey();
				std::string Type, Value;

				while(KeyName != "")
				{
					if(!strcmp(KeyName.c_str(), m_SplashScreen))
					{
						int count = 0;
						Type = AttrFile.FindFirstName(KeyName);
						Value = AttrFile.FindFirstValue();

						while(Type != "")
						{
							count += 1;
							Type = AttrFile.FindNextName();
							Value = AttrFile.FindNextValue();
						}

						int index = (int)EffectC_Frand(1.0f, (float)count+1.0f);
						count = 1;
						Type = AttrFile.FindFirstName(KeyName);
						Value = AttrFile.FindFirstValue();

						while(count!=index)
						{
							count += 1;
							Type = AttrFile.FindNextName();
							Value = AttrFile.FindNextValue();
						}

						char name[128];
						strcpy(name, Type.c_str());
						theGameEngine->DisplaySplash(name, m_SplashAudio);
						break;
					}

					KeyName = AttrFile.FindNextKey();
				}
			}
		}
		else
			theGameEngine->DisplaySplash(m_SplashScreen, m_SplashAudio);

		if(EffectC_IsStringNull(m_NewLevel))
			Spin(5000);
	}
	else
	{
		if(!EffectC_IsStringNull(m_Message))
		{
			geRect M_CameraRect;
			geCamera *M_Camera;
			M_CameraRect.Left = 0;
			M_CameraRect.Right = CCD->Engine()->Width() - 1;
			M_CameraRect.Top = 0;
			M_CameraRect.Bottom = CCD->Engine()->Height() - 1;
			M_Camera = geCamera_Create(2.0f, &M_CameraRect);

			if(HasFocus)
			{
				geEngine_BeginFrame(CCD->Engine()->Engine(), M_Camera, GE_FALSE);
				int width = CCD->MenuManager()->FontWidth(m_Font, m_Message);
				int height = CCD->MenuManager()->FontHeight(m_Font);
				int xoffset = (CCD->Engine()->Width()-width)/2;
				int yoffset = (CCD->Engine()->Height()-height)/2;
				CCD->MenuManager()->FontRect(m_Message, m_Font, xoffset, yoffset);
				geEngine_EndFrame(CCD->Engine()->Engine());

				geEngine_BeginFrame(CCD->Engine()->Engine(), M_Camera, GE_FALSE);
				CCD->MenuManager()->FontRect(m_Message, m_Font, xoffset, yoffset);
				geEngine_EndFrame(CCD->Engine()->Engine());
			}

			geCamera_Destroy(&M_Camera);
		}
	}

	// Shut the current level down
	ShutdownLevel();
	CCD->SetKeyPaused(false);
	// Ok, load in the new level

	if(!EffectC_IsStringNull(m_NewLevel))
	{
		// Shut the current level down

		if(InitializeLevel(szLevelFile) != 0)
		{
			// I regard this as a catastrophic error, so we'll log it and
			// ..commit suicide.
			char szBug[256];
			sprintf(szBug, "[ERROR] File %s - Line %d: Failed to load new level %s",
					__FILE__, __LINE__, szLevelFile);
			theGameEngine->ReportError(szBug, false);
			ShutdownLevel();
			ShutdownCommon();
			exit(-334);
		}

		CCD->HUD()->LoadConfiguration();

		// Restore the player attributes for the new level
		if(KeepAttributes)
			thePlayer->LoadAttributes(".\\temp.bin");
		else
			thePlayer->LoadAttributes("pdoa.bin");

		KeepAttributes = true;

		CCD->TerrainMgr()->Init();

		// Move the player avatar to the start of the new level
		thePlayer->MoveToStart();

		geVec3d ActPos = thePlayer->Position();
		geVec3d_Subtract(&ActPos, &Offset, &ActPos);
		CCD->ActorManager()->Position(thePlayer->GetActor(), ActPos);

		if(!thePlayer->GetMonitorMode())
		{
			if(ViewPoint == thePlayer->GetViewPoint())
			{
				if(!UseAngle)
				{
					CCD->ActorManager()->Rotate(thePlayer->GetActor(), m_playerotation);
					CCD->CameraManager()->Rotate(theRotation);
				}
				CCD->CameraManager()->SetCameraOffset(theTranslation, theRotate);
				CCD->CameraManager()->Set3rdData(m_defaultdistance, m_cameraX, m_cameraY);
			}
		}

		int i;

		// MAX_WEAPONS = 40
		for(i=0; i<40; i++)
			theWeapon->SetSlot(i, Slot[i]);

		if(!thePlayer->GetMonitorMode())
			theWeapon->ReSetWeapon(CurrentWeapon);

		geEntity_EntitySet *pSet;
		geEntity *pEntity;

		pSet = geWorld_GetEntitySet(CCD->World(), "PlayerSetup");
		pEntity = geEntity_EntitySetGetNextEntity(pSet, NULL);
		PlayerSetup *pSetup = static_cast<PlayerSetup*>(geEntity_GetUserData(pEntity));

		m_SplashAudio[0] = '\0';
		m_CutScene[0] = '\0';

		if(EffectC_IsStringNull(pSetup->DeathMessage))
		{
			m_Message[0] = '\0';
		}
		else
		{
			strncpy(m_Message, pSetup->DeathMessage, 255);
			m_Message[255] = '\0';
		}

		m_Font = pSetup->DeathFontSize;

		if(!EffectC_IsStringNull(pSetup->Deathlevel))
		{
			strncpy(m_NewLevel, pSetup->Deathlevel, 255);
			m_NewLevel[255] = '\0';
		}

		m_ChangeLevel = false;
		Paused = true;

		if(SplashHold && !EffectC_IsStringNull(m_SplashScreen))
		{
			while((GetAsyncKeyState(VK_SPACE) & 0x8000) == 0)
			{
			}

			while((GetAsyncKeyState(VK_SPACE) & 0x8000) != 0)
			{
			}
		}

		m_SplashScreen[0] = '\0';

		return true;						// Level change successful!
	}
	else
	{
		while(theUserInput->GetKeyboardInput() != -1)
		{
		}

		return false;
	}
}

/* ------------------------------------------------------------------------------------ */
//	RenderComponents
//
//	Call the Render() member function for all renderable components.
/* ------------------------------------------------------------------------------------ */
void CCommonData::RenderComponents()
{
	theParticles->Render();	// Render all particle systems

	if(ShowTrack)
		theScriptPoints->Render();

	theOverlay->Render();
	theTerrainMgr->Render();
}

/* ------------------------------------------------------------------------------------ */
//	GetHashMethod
/* ------------------------------------------------------------------------------------ */
long CCommonData::GetHashMethod(std::string key)
{
	long *value = MethodHash->GetMember(key);

	if(value)
		return *value;

	return 0;
}

/* ------------------------------------------------------------------------------------ */
//	AddHashMethod
/* ------------------------------------------------------------------------------------ */
bool CCommonData::AddHashMethod(std::string key, long value)
{
	bool add;
	long *vlong = new long;

	*vlong = value;
	add = MethodHash->AddKey(key, vlong);

	return add;
}

/* ------------------------------------------------------------------------------------ */
//	FillHashMethods
/* ------------------------------------------------------------------------------------ */
void CCommonData::ClearHashMethods(void)
{
	// default destructor of hashtable does not free user allocated memory!
	MethodHash->RemoveAllKey(true);
}

/* ------------------------------------------------------------------------------------ */
//	FillHashMethods
/* ------------------------------------------------------------------------------------ */
void CCommonData::FillHashMethods(void)
{
	// low methods
	AddHashMethod("HighLevel",				RGF_SM_HIGHLEVEL);
	AddHashMethod("Animate",				RGF_SM_ANIMATE);
	AddHashMethod("Gravity",				RGF_SM_GRAVITY);
	AddHashMethod("PlaySound",				RGF_SM_PLAYSOUND);
	AddHashMethod("EnemyExist",				RGF_SM_ENEMYEXIST);
	AddHashMethod("GetEventState",			RGF_SM_GETEVENTSTATE);
	AddHashMethod("Integer",				RGF_SM_INTEGER);
	AddHashMethod("GetAttribute",			RGF_SM_GETATTRIBUTE);
	AddHashMethod("GetCurFlipBook",			RGF_SM_GETCURFLIPBOOK);
	AddHashMethod("ModifyAttribute",		RGF_SM_MODIFYATTRIBUTE);
	AddHashMethod("SetAttribute",			RGF_SM_SETATTRIBUTE);
	AddHashMethod("SetPlayerWeapon",		RGF_SM_SETPLAYERWEAPON);
	AddHashMethod("SetUseItem",				RGF_SM_SETUSEITEM);
	AddHashMethod("ClearUseItem",			RGF_SM_CLEARUSEITEM);
	AddHashMethod("StringCopy",				RGF_SM_STRINGCOPY);
	AddHashMethod("LeftCopy",				RGF_SM_LEFTCOPY);
	AddHashMethod("IsEntityVsible",			RGF_SM_ISENTITYVSIBLE);
	AddHashMethod("DamageEntity",			RGF_SM_DAMAGEENTITY);
	AddHashMethod("AnimateEntity",			RGF_SM_ANIMATEENTITY);
	AddHashMethod("AnimateHold",			RGF_SM_ANIMATEHOLD);
	AddHashMethod("AnimateTarget",			RGF_SM_ANIMATETARGET);
	AddHashMethod("GetEntityX",				RGF_SM_GETENTITYX);
	AddHashMethod("GetEntityY",				RGF_SM_GETENTITYY);
	AddHashMethod("GetEntityZ",				RGF_SM_GETENTITYZ);
	AddHashMethod("IsKeyDown",				RGF_SM_ISKEYDOWN);
	AddHashMethod("GetEntityYaw",			RGF_SM_GETENTITYYAW);
	AddHashMethod("MatchEntityAngles",		RGF_SM_MATCHENTITYANGLES);
	AddHashMethod("FacePoint",				RGF_SM_FACEPOINT);
	AddHashMethod("NewPoint",				RGF_SM_NEWPOINT);
	AddHashMethod("GetPointYaw",			RGF_SM_GETPOINTYAW);
	AddHashMethod("NextPoint",				RGF_SM_NEXTPOINT);
	AddHashMethod("SetTarget",				RGF_SM_SETTARGET);
	AddHashMethod("GetDistanceTo",			RGF_SM_GETDISTANCETO);
	AddHashMethod("TeleportEntity",			RGF_SM_TELEPORTENTITY);
	AddHashMethod("SaveAttributes",			RGF_SM_SAVEATTRIBUTES);
	AddHashMethod("TraceToActor",			RGF_SM_TRACETOACTOR);
	AddHashMethod("AnimateBlend",			RGF_SM_ANIMATEBLEND);
	AddHashMethod("AnimationSpeed",			RGF_SM_ANIMATIONSPEED);
	AddHashMethod("SetCollision",			RGF_SM_SETCOLLISION);
	AddHashMethod("SetNoCollision",			RGF_SM_SETNOCOLLISION);
	AddHashMethod("DamageArea",				RGF_SM_DAMAGEAREA);
	AddHashMethod("PlayerMatchAngles",		RGF_SM_PLAYERMATCHANGLES);
	AddHashMethod("ConvertDegrees",			RGF_SM_CONVERTDEGREES);
	AddHashMethod("AttachCamera",			RGF_SM_ATTACHCAMERA);
	AddHashMethod("AttachCameraToBone",		RGF_SM_ATTACHCAMERATOBONE);
	AddHashMethod("AttachCameraToEntity",	RGF_SM_ATTACHCAMERATOENTITY);
	AddHashMethod("DetachCamera",			RGF_SM_DETACHCAMERA);
	AddHashMethod("TiltCamera",				RGF_SM_TILTCAMERA);
	AddHashMethod("PositionToPlatform",		RGF_SM_POSITIONTOPLATFORM);
	AddHashMethod("ActivateTrigger",		RGF_SM_ACTIVATETRIGGER);
	AddHashMethod("UpdateEnemyVis",			RGF_SM_UPDATEENEMYVIS);
	AddHashMethod("TargetPlayer",			RGF_SM_TARGETPLAYER);
	AddHashMethod("FireProjectileBlind",	RGF_SM_FIREPROJECTILEBLIND);
	AddHashMethod("SetTargetPoint",			RGF_SM_SETTARGETPOINT);
	AddHashMethod("GetBoneX",				RGF_SM_GETBONEX);
	AddHashMethod("GetBoneY",				RGF_SM_GETBONEY);
	AddHashMethod("GetBoneZ",				RGF_SM_GETBONEZ);
	AddHashMethod("GetBoneYaw",				RGF_SM_GETBONEYAW);
	AddHashMethod("SetPosition",			RGF_SM_SETPOSITION);
	AddHashMethod("IsButtonDown",			RGF_SM_ISBUTTONDOWN);
	AddHashMethod("GetJoyAxisX",			RGF_SM_GETJOYAXISX);
	AddHashMethod("GetJoyAxisY",			RGF_SM_GETJOYAXISY);
	AddHashMethod("GetJoyAxisZ",			RGF_SM_GETJOYAXISZ);
	AddHashMethod("GetNumJoysticks",		RGF_SM_GETNUMJOYSTICKS);
	AddHashMethod("SetBoundingBox",			RGF_SM_SETBOUNDINGBOX);
	AddHashMethod("GetBoneToBone",			RGF_SM_GETBONETOBONE);
	AddHashMethod("SwitchView",				RGF_SM_SWITCHVIEW);
	AddHashMethod("ForceEntityUp",			RGF_SM_FORCEENTITYUP);
	AddHashMethod("ForceEntityDown",		RGF_SM_FORCEENTITYDOWN);
	AddHashMethod("ForceEntityRight",		RGF_SM_FORCEENTITYRIGHT);
	AddHashMethod("ForceEntityLeft",		RGF_SM_FORCEENTITYLEFT);
	AddHashMethod("ForceEntityForward",		RGF_SM_FORCEENTITYFORWARD);
	AddHashMethod("ForceEntityBackward",	RGF_SM_FORCEENTITYBACKWARD);
	AddHashMethod("GetGroundTexture",		RGF_SM_GETGROUNDTEXTURE);
	AddHashMethod("GetPlayerGroundTexture",	RGF_SM_GETPLAYERGROUNDTEXTURE);
	AddHashMethod("PositionToBone",			RGF_SM_POSITIONTOBONE);
	AddHashMethod("SetWeaponMatFromFlip",	RGF_SM_SETWEAPONMATFROMFLIP);
	AddHashMethod("SetShadowFromFlip",		RGF_SM_SETSHADOWFROMFLIP);
	AddHashMethod("GetCollideDistance",		RGF_SM_GETCOLLIDEDISTANCE);
	AddHashMethod("ResetAnimate",			RGF_SM_RESETANIMATE);
	AddHashMethod("WhereIsPlayer",			RGF_SM_WHEREISPLAYER);
	AddHashMethod("WhereIsEntity",			RGF_SM_WHEREISENTITY);
	AddHashMethod("InsertEvent",			RGF_SM_INSERTEVENT);
	AddHashMethod("CheckForEvent",			RGF_SM_CHECKFOREVENT);
	AddHashMethod("PlayEventSound",			RGF_SM_PLAYEVENTSOUND);
	AddHashMethod("LoadAnimation",			RGF_SM_LOADANIMATION);
	AddHashMethod("StartSoundTrack",		RGF_SM_STARTSOUNDTRACK);
	AddHashMethod("StopAllAudioStreams",	RGF_SM_STOPALLAUDIOSTREAMS);
	AddHashMethod("ChangeYaw",				RGF_SM_CHANGEYAW);
	AddHashMethod("ChangePitch",			RGF_SM_CHANGEPITCH);
	AddHashMethod("random",					RGF_SM_RANDOM);
	AddHashMethod("walkmove",				RGF_SM_WALKMOVE);
	AddHashMethod("flymove",				RGF_SM_FLYMOVE);
	AddHashMethod("Damage",					RGF_SM_DAMAGE);
	AddHashMethod("DamagePlayer",			RGF_SM_DAMAGEPLAYER);
	AddHashMethod("PositionToPlayer",		RGF_SM_POSITIONTOPLAYER);
	AddHashMethod("PlayerToPosition",		RGF_SM_PLAYERTOPOSITION);
	AddHashMethod("PositionToPawn",			RGF_SM_POSITIONTOPAWN);
	AddHashMethod("SetKeyPause",			RGF_SM_SETKEYPAUSE);
	AddHashMethod("PlayerRender",			RGF_SM_PLAYERRENDER);
	AddHashMethod("PawnRender",				RGF_SM_PAWNRENDER);
	AddHashMethod("ChangeMaterial",			RGF_SM_CHANGEMATERIAL);
	AddHashMethod("SetHoldAtEnd",			RGF_SM_SETHOLDATEND);
	AddHashMethod("ForceUp",				RGF_SM_FORCEUP);
	AddHashMethod("ForceDown",				RGF_SM_FORCEDOWN);
	AddHashMethod("ForceRight",				RGF_SM_FORCERIGHT);
	AddHashMethod("ForceLeft",				RGF_SM_FORCELEFT);
	AddHashMethod("ForceForward",			RGF_SM_FORCEFORWARD);
	AddHashMethod("ForceBackward",			RGF_SM_FORCEBACKWARD);
	AddHashMethod("UpdateTarget",			RGF_SM_UPDATETARGET);
	AddHashMethod("FireProjectile",			RGF_SM_FIREPROJECTILE);
	AddHashMethod("AddExplosion",			RGF_SM_ADDEXPLOSION);
	AddHashMethod("GetLastBoneHit",			RGF_SM_GETLASTBONEHIT);
	AddHashMethod("debug",					RGF_SM_DEBUG);
	AddHashMethod("SetEventState",			RGF_SM_SETEVENTSTATE);
	AddHashMethod("GetStringLength",		RGF_SM_GETSTRINGLENGTH);
	AddHashMethod("DrawText",				RGF_SM_DRAWTEXT);
	AddHashMethod("DrawFlipBookImage",		RGF_SM_DRAWFLIPBOOKIMAGE);
	AddHashMethod("DrawPolyShadow",			RGF_SM_DRAWPOLYSHADOW);
	AddHashMethod("MatchPlayerAngles",		RGF_SM_MATCHPLAYERANGLES);
	AddHashMethod("DamageAtBone",			RGF_SM_DAMAGEATBONE);
	AddHashMethod("SaveActor",				RGF_SM_SAVEACTOR);
	AddHashMethod("LookAtPawn",				RGF_SM_LOOKATPAWN);
	AddHashMethod("AutoWalk",				RGF_SM_AUTOWALK);
	AddHashMethod("FastDistance",			RGF_SM_FASTDISTANCE);
	AddHashMethod("StepHeight",				RGF_SM_STEPHEIGHT);
	AddHashMethod("DrawVPoly",				RGF_SM_DRAWVPOLY);
	AddHashMethod("DrawHPoly",				RGF_SM_DRAWHPOLY);
	AddHashMethod("GetPitchToPoint",		RGF_SM_GETPITCHTOPOINT);
	AddHashMethod("GetYawToPoint",			RGF_SM_GETYAWTOPOINT);
	AddHashMethod("FastPointCheck",			RGF_SM_FASTPOINTCHECK);
	AddHashMethod("SetCameraWindow",		RGF_SM_SETCAMERAWINDOW);
	AddHashMethod("SetFixedCameraPosition",	RGF_SM_SETFIXEDCAMERAPOSITION);
	AddHashMethod("SetFixedCameraRotation",	RGF_SM_SETFIXEDCAMERAROTATION);
	AddHashMethod("SetFixedCameraFOV",		RGF_SM_SETFIXEDCAMERAFOV);
	AddHashMethod("MoveFixedCamera",		RGF_SM_MOVEFIXEDCAMERA);
	AddHashMethod("RotateFixedCamera",		RGF_SM_ROTATEFIXEDCAMERA);
	AddHashMethod("DistanceBetweenEntities",RGF_SM_DISTANCEBETWEENENTITIES);
	AddHashMethod("SetEntityProperties",	RGF_SM_SETENTITYPROPERTIES);
	AddHashMethod("SetEntityLighting",		RGF_SM_SETENTITYLIGHTING);
	AddHashMethod("UpdateScriptPoint",		RGF_SM_UPDATESCRIPTPOINT);
	AddHashMethod("GetEntityScreenX",		RGF_SM_GETENTITYSCREENX);
	AddHashMethod("GetEntityScreenY",		RGF_SM_GETENTITYSCREENY);
	AddHashMethod("GetScreenWidth",			RGF_SM_GETSCREENWIDTH);
	AddHashMethod("GetScreenHeight",		RGF_SM_GETSCREENHEIGHT);
	AddHashMethod("MouseSelect",			RGF_SM_MOUSESELECT);
	AddHashMethod("MouseControlledPlayer",	RGF_SM_MOUSECONTROLLEDPLAYER);
	AddHashMethod("ShowMouse",				RGF_SM_SHOWMOUSE);
	AddHashMethod("GetMousePosX",			RGF_SM_GETMOUSEPOSX);
	AddHashMethod("GetMousePosY",			RGF_SM_GETMOUSEPOSY);
	AddHashMethod("SetMousePos",			RGF_SM_SETMOUSEPOS);
	AddHashMethod("SetGamma",				RGF_SM_SETGAMMA);
	AddHashMethod("GetGamma",				RGF_SM_GETGAMMA);
	AddHashMethod("FillScreenArea",			RGF_SM_FILLSCREENAREA);
	AddHashMethod("RemoveScreenArea",		RGF_SM_REMOVESCREENAREA);
	AddHashMethod("ShowText",				RGF_SM_SHOWTEXT);
	AddHashMethod("RemoveText",				RGF_SM_REMOVETEXT);
	AddHashMethod("ShowHudPicture",			RGF_SM_SHOWHUDPICTURE);
	AddHashMethod("SetHudDraw",				RGF_SM_SETHUDDRAW);
	AddHashMethod("GetHudDraw",				RGF_SM_GETHUDDRAW);
	AddHashMethod("SetAlpha",				RGF_SM_SETALPHA);
	AddHashMethod("GetAlpha",				RGF_SM_GETALPHA);
	AddHashMethod("SetEntityAlpha",			RGF_SM_SETENTITYALPHA);
	AddHashMethod("GetEntityAlpha",			RGF_SM_GETENTITYALPHA);
	AddHashMethod("SetScale",				RGF_SM_SETSCALE);
	AddHashMethod("SetEntityScale",			RGF_SM_SETENTITYSCALE);
	AddHashMethod("CheckArea",				RGF_SM_CHECKAREA);
	AddHashMethod("SetFlag",				RGF_SM_SETFLAG);
	AddHashMethod("GetFlag",				RGF_SM_GETFLAG);
	AddHashMethod("PowerUp",				RGF_SM_POWERUP);
	AddHashMethod("GetPowerUpLevel",		RGF_SM_GETPOWERUPLEVEL);
	AddHashMethod("ActivateHudElement",		RGF_SM_ACTIVATEHUDELEMENT);
	AddHashMethod("MoveEntity",				RGF_SM_MOVEENTITY);
	AddHashMethod("RotateEntity",			RGF_SM_ROTATEENTITY);
	AddHashMethod("SetEntityPosition",		RGF_SM_SETENTITYPOSITION);
	AddHashMethod("SetEntityRotation",		RGF_SM_SETENTITYROTATION);
	AddHashMethod("AddAttribute",			RGF_SM_ADDATTRIBUTE);
	AddHashMethod("SetAttributeValueLimits",RGF_SM_SETATTRIBUTEVALUELIMITS);
	AddHashMethod("RightCopy",				RGF_SM_RIGHTCOPY);
	AddHashMethod("NearestPoint",			RGF_SM_NEARESTPOINT);
	AddHashMethod("SetFOV",					RGF_SM_SETFOV);
	AddHashMethod("sin",					RGF_SM_SIN);
	AddHashMethod("cos",					RGF_SM_COS);
	AddHashMethod("tan",					RGF_SM_TAN);
	AddHashMethod("asin",					RGF_SM_ASIN);
	AddHashMethod("acos",					RGF_SM_ACOS);
	AddHashMethod("atan",					RGF_SM_ATAN);
	AddHashMethod("SetGravity",				RGF_SM_SETGRAVITY);
	AddHashMethod("GetGravityX",			RGF_SM_GETGRAVITYX);
	AddHashMethod("GetGravityY",			RGF_SM_GETGRAVITYY);
	AddHashMethod("GetGravityZ",			RGF_SM_GETGRAVITYZ);
	AddHashMethod("SetWind",				RGF_SM_SETWIND);
	AddHashMethod("GetWindX",				RGF_SM_GETWINDX);
	AddHashMethod("GetWindY",				RGF_SM_GETWINDY);
	AddHashMethod("GetWindZ",				RGF_SM_GETWINDZ);
	AddHashMethod("SetWindBase",			RGF_SM_SETWINDBASE);
	AddHashMethod("GetWindBaseX",			RGF_SM_GETWINDBASEX);
	AddHashMethod("GetWindBaseY",			RGF_SM_GETWINDBASEY);
	AddHashMethod("GetWindBaseZ",			RGF_SM_GETWINDBASEZ);
	AddHashMethod("SetWindGenerator",		RGF_SM_SETWINDGENERATOR);
	AddHashMethod("SetForceEnabled",		RGF_SM_SETFORCEENABLED);
	AddHashMethod("IsInLiquid",				RGF_SM_ISINLIQUID);
	AddHashMethod("GetLiquid",				RGF_SM_GETLIQUID);
	AddHashMethod("EndScript",				RGF_SM_ENDSCRIPT);
	//AddHashMethod("SetSlowMotion",		RGF_SM_SETSLOWMOTION);

	// high methods
	AddHashMethod("MoveToPoint",			RGF_SM_MOVETOPOINT);
	AddHashMethod("RotateToPoint",			RGF_SM_ROTATETOPOINT);
	AddHashMethod("NewOrder",				RGF_SM_NEWORDER);
	AddHashMethod("NextOrder",				RGF_SM_NEXTORDER);
	AddHashMethod("RotateToAlign",			RGF_SM_ROTATETOALIGN);
	//AddHashMethod("NextPoint",			RGF_SM_NEXTPOINT);				// same as low method
	AddHashMethod("Delay",					RGF_SM_DELAY);
	AddHashMethod("PlayAnimation",			RGF_SM_PLAYANIMATION);
	AddHashMethod("BlendToAnimation",		RGF_SM_BLENDTOANIMATION);
	AddHashMethod("LoopAnimation",			RGF_SM_LOOPANIMATION);
	AddHashMethod("Rotate",					RGF_SM_ROTATE);
	AddHashMethod("RotateMoveToPoint",		RGF_SM_ROTATEMOVETOPOINT);
	AddHashMethod("RotateMove",				RGF_SM_ROTATEMOVE);
	AddHashMethod("NewPath",				RGF_SM_NEWPATH);
	AddHashMethod("RestartOrder",			RGF_SM_RESTARTORDER);
	AddHashMethod("PlayerDistOrder",		RGF_SM_PLAYERDISTORDER);
	AddHashMethod("Console",				RGF_SM_CONSOLE);
	AddHashMethod("AudibleRadius",			RGF_SM_AUDIBLERADIUS);
	AddHashMethod("AddPainOrder",			RGF_SM_ADDPAINORDER);
	AddHashMethod("FindTargetOrder",		RGF_SM_FINDTARGETORDER);
	AddHashMethod("FindPointOrder",			RGF_SM_FINDPOINTORDER);
	//AddHashMethod("NewPoint",				RGF_SM_NEWPOINT);				// same as low method
	AddHashMethod("MoveForward",			RGF_SM_MOVEFORWARD);
	AddHashMethod("MoveBackward",			RGF_SM_MOVEBACKWARD);
	AddHashMethod("MoveLeft",				RGF_SM_MOVELEFT);
	AddHashMethod("MoveRight",				RGF_SM_MOVERIGHT);
	AddHashMethod("Move",					RGF_SM_MOVE);
	AddHashMethod("AvoidOrder",				RGF_SM_AVOIDORDER);
	AddHashMethod("Return",					RGF_SM_RETURN);
	AddHashMethod("Align",					RGF_SM_ALIGN);
	AddHashMethod("Jump",					RGF_SM_JUMP);
	AddHashMethod("AddTriggerOrder",		RGF_SM_ADDTRIGGERORDER);
	AddHashMethod("DelTriggerOrder",		RGF_SM_DELTRIGGERORDER);
	//AddHashMethod("SetEventState",		RGF_SM_SETEVENTSTATE);			// same as low method
	AddHashMethod("FacePlayer",				RGF_SM_FACEPLAYER);
	AddHashMethod("RotateToPlayer",			RGF_SM_ROTATETOPLAYER);
	AddHashMethod("RotateAroundPointLeft",	RGF_SM_ROTATEAROUNDPOINTLEFT);
	AddHashMethod("RotateAroundPointRight",	RGF_SM_ROTATEAROUNDPOINTRIGHT);
	AddHashMethod("TeleportToPoint",		RGF_SM_TELEPORTTOPOINT);
	//AddHashMethod("AnimationSpeed",		RGF_SM_ANIMATIONSPEED);			// same as low method
	//AddHashMethod("SetFlag",				RGF_SM_SETFLAG);				// same as low method
	AddHashMethod("AddFlagOrder",			RGF_SM_ADDFLAGORDER);
	AddHashMethod("DelFlagOrder",			RGF_SM_DELFLAGORDER);
	//AddHashMethod("ChangeMaterial",		RGF_SM_CHANGEMATERIAL);			// same as low method
	AddHashMethod("AddTimerOrder",			RGF_SM_ADDTIMERORDER);
	AddHashMethod("DelTimerOrder",			RGF_SM_DELTIMERORDER);
	AddHashMethod("AddRandomSound",			RGF_SM_ADDRANDOMSOUND);
	AddHashMethod("DelRandomSound",			RGF_SM_DELRANDOMSOUND);
	AddHashMethod("AddDistanceOrder",		RGF_SM_ADDDISTANCEORDER);
	AddHashMethod("DelDistanceOrder",		RGF_SM_DELDISTANCEORDER);
	AddHashMethod("AddCollisionOrder",		RGF_SM_ADDCOLLISIONORDER);
	AddHashMethod("DelCollisionOrder",		RGF_SM_DELCOLLISIONORDER);
	AddHashMethod("AnimateStop",			RGF_SM_ANIMATESTOP);
	AddHashMethod("AttributeOrder",			RGF_SM_ATTRIBUTEORDER);
	AddHashMethod("Remove",					RGF_SM_REMOVE);
	//AddHashMethod("SetKeyPause",			RGF_SM_SETKEYPAUSE);			// same as low method
	//AddHashMethod("SetNoCollision",		RGF_SM_SETNOCOLLISION);			// same as low method
	//AddHashMethod("SetCollision",			RGF_SM_SETCOLLISION);			// same as low method
	AddHashMethod("AllowUseKey",			RGF_SM_ALLOWUSEKEY);
	//AddHashMethod("SetHudDraw",			RGF_SM_SETHUDDRAW);				// same as low method
	AddHashMethod("HideFromRadar",			RGF_SM_HIDEFROMRADAR);
	AddHashMethod("Conversation",			RGF_SM_CONVERSATION);
	AddHashMethod("FadeIn",					RGF_SM_FADEIN);
	AddHashMethod("FadeOut",				RGF_SM_FADEOUT);
	//AddHashMethod("SetFOV",				RGF_SM_SETFOV);					// same as low method
	//AddHashMethod("StepHeight",			RGF_SM_STEPHEIGHT);				// same as low method
	AddHashMethod("SetGroup",				RGF_SM_SETGROUP);
	AddHashMethod("HostilePlayer",			RGF_SM_HOSTILEPLAYER);
	AddHashMethod("HostileDifferent",		RGF_SM_HOSTILEDIFFERENT);
	AddHashMethod("HostileSame",			RGF_SM_HOSTILESAME);
	//AddHashMethod("Gravity",				RGF_SM_GRAVITY);				// same as low method
	AddHashMethod("SoundLoop",				RGF_SM_SOUNDLOOP);
	AddHashMethod("IsPushable",				RGF_SM_ISPUSHABLE);
	AddHashMethod("IsVehicle",				RGF_SM_ISVEHICLE);
	AddHashMethod("MoveToTarget",			RGF_SM_MOVETOTARGET);
	AddHashMethod("RotateToTarget",			RGF_SM_ROTATETOTARGET);
	AddHashMethod("RotateMoveToTarget",		RGF_SM_ROTATEMOVETOTARGET);
	AddHashMethod("LowLevel",				RGF_SM_LOWLEVEL);
	AddHashMethod("BoxWidth",				RGF_SM_BOXWIDTH);
	AddHashMethod("BoxHeight",				RGF_SM_BOXHEIGHT);
	AddHashMethod("Scale",					RGF_SM_SCALE);
	//AddHashMethod("SetScale",				RGF_SM_SETSCALE);				// same as low method
	//AddHashMethod("FireProjectile",		RGF_SM_FIREPROJECTILE);			// same as low method
	//AddHashMethod("AddExplosion",			RGF_SM_ADDEXPLOSION);			// same as low method
	AddHashMethod("TargetGroup",			RGF_SM_TARGETGROUP);
	AddHashMethod("TestDamageOrder",		RGF_SM_TESTDAMAGEORDER);
	AddHashMethod("SetLODDistance",			RGF_SM_SETLODDISTANCE);
	AddHashMethod("AttachToActor",			RGF_SM_ATTACHTOACTOR);
	AddHashMethod("DetachFromActor",		RGF_SM_DETACHFROMACTOR);
	AddHashMethod("AttachBlendActor",		RGF_SM_ATTACHBLENDACTOR);
	AddHashMethod("DetachBlendActor",		RGF_SM_DETACHBLENDACTOR);
	AddHashMethod("AttachAccessory",		RGF_SM_ATTACHACCESSORY);
	AddHashMethod("DetachAccessory",		RGF_SM_DETACHACCESSORY);
	AddHashMethod("SetWeapon",				RGF_SM_SETWEAPON);
	AddHashMethod("RemoveWeapon",			RGF_SM_REMOVEWEAPON);
	//AddHashMethod("random",				RGF_SM_RANDOM);					// same as low method
	//AddHashMethod("debug",				RGF_SM_DEBUG);					// same as low method
	AddHashMethod("ShowTextDelay",			RGF_SM_SHOWTEXTDELAY);
	//AddHashMethod("ShowText",				RGF_SM_SHOWTEXT);				// same as low method
	//AddHashMethod("RemoveText",			RGF_SM_REMOVETEXT);				// same as low method
	AddHashMethod("GetConvReplyNr",			RGF_SM_GETCONVREPLYNR);
	AddHashMethod("Concat",					RGF_SM_CONCAT);
	//AddHashMethod("GetAttribute",			RGF_SM_GETATTRIBUTE);			// same as low method
	//AddHashMethod("ModifyAttribute",		RGF_SM_MODIFYATTRIBUTE);		// same as low method
	//AddHashMethod("SetAttribute",			RGF_SM_SETATTRIBUTE);			// same as low method
	//AddHashMethod("AddAttribute",			RGF_SM_ADDATTRIBUTE);			// same as low method
	//AddHashMethod("SetAttributeValueLimits",RGF_SM_SETATTRIBUTEVALUELIMITS);	// same as low method
	//AddHashMethod("GetFlag",				RGF_SM_GETFLAG);				// same as low method
	//AddHashMethod("MouseControlledPlayer",RGF_SM_MOUSECONTROLLEDPLAYER);	// same as low method
	//AddHashMethod("GetEventState",		RGF_SM_GETEVENTSTATE);			// same as low method
	//AddHashMethod("EndScript",			RGF_SM_ENDSCRIPT);				// same as low method

	// readable variables
	AddHashMethod("time",					RGF_SM_TIME);
	AddHashMethod("ThinkTime",				RGF_SM_THINKTIME);
	AddHashMethod("DifficultyLevel",		RGF_SM_DIFFICULTYLEVEL);
	AddHashMethod("EntityName",				RGF_SM_ENTITYNAME);
	AddHashMethod("health",					RGF_SM_HEALTH);
	AddHashMethod("attack_finished",		RGF_SM_ATTACK_FINISHED);
	AddHashMethod("attack_state",			RGF_SM_ATTACK_STATE);
	AddHashMethod("enemy_vis",				RGF_SM_ENEMY_VIS);
	AddHashMethod("enemy_infront",			RGF_SM_ENEMY_INFRONT);
	AddHashMethod("enemy_range",			RGF_SM_ENEMY_RANGE);
	AddHashMethod("player_range",			RGF_SM_PLAYER_RANGE);
	AddHashMethod("enemy_yaw",				RGF_SM_ENEMY_YAW);
	AddHashMethod("last_enemy_yaw",			RGF_SM_LAST_ENEMY_YAW);
	AddHashMethod("enemy_pitch",			RGF_SM_ENEMY_PITCH);
	AddHashMethod("last_enemy_pitch",		RGF_SM_LAST_ENEMY_PITCH);
	AddHashMethod("last_enemy_range",		RGF_SM_LAST_ENEMY_RANGE);
	AddHashMethod("current_yaw",			RGF_SM_CURRENT_YAW);
	AddHashMethod("yaw_speed",				RGF_SM_YAW_SPEED);
	AddHashMethod("ideal_yaw",				RGF_SM_IDEAL_YAW);
	AddHashMethod("current_pitch",			RGF_SM_CURRENT_PITCH);
	AddHashMethod("pitch_speed",			RGF_SM_PITCH_SPEED);
	AddHashMethod("ideal_pitch",			RGF_SM_IDEAL_PITCH);
	AddHashMethod("in_pain",				RGF_SM_IN_PAIN);
	AddHashMethod("animate_at_end",			RGF_SM_ANIMATE_AT_END);
	AddHashMethod("IsFalling",				RGF_SM_ISFALLING);
	AddHashMethod("current_X",				RGF_SM_CURRENT_X);
	AddHashMethod("current_Y",				RGF_SM_CURRENT_Y);
	AddHashMethod("current_Z",				RGF_SM_CURRENT_Z);
	AddHashMethod("player_X",				RGF_SM_PLAYER_X);
	AddHashMethod("player_Y",				RGF_SM_PLAYER_Y);
	AddHashMethod("player_Z",				RGF_SM_PLAYER_Z);
	AddHashMethod("distancetopoint",		RGF_SM_DISTANCETOPOINT);
	AddHashMethod("playertopoint",			RGF_SM_PLAYERTOPOINT);
	AddHashMethod("key_pressed",			RGF_SM_KEY_PRESSED);
	AddHashMethod("player_vis",				RGF_SM_PLAYER_VIS);
	AddHashMethod("target_name",			RGF_SM_TARGET_NAME);
	AddHashMethod("enemy_X",				RGF_SM_ENEMY_X);
	AddHashMethod("enemy_Y",				RGF_SM_ENEMY_Y);
	AddHashMethod("enemy_Z",				RGF_SM_ENEMY_Z);
	AddHashMethod("player_yaw",				RGF_SM_PLAYER_YAW);
	AddHashMethod("point_vis",				RGF_SM_POINT_VIS);
	AddHashMethod("player_weapon",			RGF_SM_PLAYER_WEAPON);
	AddHashMethod("point_name",				RGF_SM_POINT_NAME);
	AddHashMethod("camera_pitch",			RGF_SM_CAMERA_PITCH);
	AddHashMethod("LODLevel",				RGF_SM_LODLEVEL);
	AddHashMethod("current_screen_X",		RGF_SM_CURRENT_SCREEN_X);
	AddHashMethod("current_screen_Y",		RGF_SM_CURRENT_SCREEN_Y);
	AddHashMethod("player_screen_X",		RGF_SM_PLAYER_SCREEN_X);
	AddHashMethod("player_screen_Y",		RGF_SM_PLAYER_SCREEN_Y);
	AddHashMethod("lbutton_pressed",		RGF_SM_LBUTTON_PRESSED);
	AddHashMethod("rbutton_pressed",		RGF_SM_RBUTTON_PRESSED);
	AddHashMethod("mbutton_pressed",		RGF_SM_MBUTTON_PRESSED);
	AddHashMethod("player_animation",		RGF_SM_PLAYER_ANIMATION);
	AddHashMethod("player_viewpoint",		RGF_SM_PLAYER_VIEWPOINT);

	// writeable variables
	AddHashMethod("lowTime",				RGF_SM_LOWTIME);
	//AddHashMethod("ThinkTime",			RGF_SM_THINKTIME);				// same as readable
	AddHashMethod("think",					RGF_SM_THINK);
	//AddHashMethod("attack_finished",		RGF_SM_ATTACK_FINISHED);		// same as readable
	//AddHashMethod("attack_state",			RGF_SM_ATTACK_STATE);			// same as readable
	//AddHashMethod("yaw_speed",			RGF_SM_YAW_SPEED);				// same as readable
	//AddHashMethod("ideal_yaw",			RGF_SM_IDEAL_YAW);				// same as readable
	//AddHashMethod("pitch_speed",			RGF_SM_PITCH_SPEED);			// same as readable
	//AddHashMethod("ideal_pitch",			RGF_SM_IDEAL_PITCH);			// same as readable
}


/* ------------------------------------------------------------------------------------ */
//	InitJoysticks
/* ------------------------------------------------------------------------------------ */
void CCommonData::InitJoysticks()
{
	if(!m_bUseDInput)
		return;

    unsigned int numJoysticks = Joystick::deviceCount();

	if(Logging)
	{
		char Info[128];
		sprintf(Info, "[INFO]  Found  %d joysticks", numJoysticks);
		CCD->ReportError(Info, false);
	}

	for(unsigned int i=0; i<4; i++)
	{
		joysticks[i] = NULL;
	}

	// Initialize all of the joysticks on the system.
	if(numJoysticks < 1)
		return;
	else if(numJoysticks > 4)
		numJoysticks = 4;

	for(unsigned int j=0; j<numJoysticks; j++)
	{
        joysticks[j] = new Joystick(j);
        joysticks[j]->open();
    }
}

/* ------------------------------------------------------------------------------------ */
//	PollJoystickAxis
/* ------------------------------------------------------------------------------------ */
int CCommonData::PollJoystickAxis(int jn, int a)
{
	if(!m_bUseDInput)
		return -1;

	DIJOYSTATE2 js;
	joysticks[jn]->poll(&js);

	if(a == 0)
	{
		return js.lX;
	}

	if(a == 1)
	{
		return js.lY;
	}

	if(a == 2)
	{
		return js.lZ;
	}

	return -1;
}

/* ------------------------------------------------------------------------------------ */
//	GetNumJoys
/* ------------------------------------------------------------------------------------ */
int CCommonData::GetNumJoys()
{
	if(!m_bUseDInput)
		return 0;

	return (int)(Joystick::deviceCount());
}

/* ------------------------------------------------------------------------------------ */
//	CheckJoystickButton
/* ------------------------------------------------------------------------------------ */
bool CCommonData::CheckJoystickButton(int jn, int bn)
{
	if(!m_bUseDInput)
		return false;

	DIJOYSTATE2 js;
	joysticks[jn]->poll(&js);

	if(js.rgbButtons[bn] & 0x80)
	{
		return true;
    }

	return false;
}

/* ------------------------------------------------------------------------------------ */
//	CloseJoysticks
/* ------------------------------------------------------------------------------------ */
void CCommonData::CloseJoysticks()
{
	if(!m_bUseDInput)
		return;

	unsigned int i;
	unsigned int numJoysticks = Joystick::deviceCount();

    // Close the joysticks.
    for(i=0; i<numJoysticks; i++)
	{
		if(joysticks[i])
		{
	        joysticks[i]->close();
			delete joysticks[i];
			joysticks[i] = NULL;
		}
    }
}


/* ------------------------------------------------------------------------------------ */
// PlayOpeningCutScene
//
// Get the opening cut scene file from the PlayerSetup entity and
// ..play it, if one exists.
/* ------------------------------------------------------------------------------------ */
void CCommonData::PlayOpeningCutScene()
{
	// Ok, check to see if there's a PlayerSetup around...
	geEntity_EntitySet *pSet;
	pSet = geWorld_GetEntitySet(theGameEngine->World(), "PlayerSetup");

	if(!pSet)
		return;									// No setup?

	// Ok, get the setup information.  There should only be one, so
	// ..we'll just take the first one we run into.
	geEntity *pEntity = geEntity_EntitySetGetNextEntity(pSet, NULL);
	PlayerSetup *pSetup = static_cast<PlayerSetup*>(geEntity_GetUserData(pEntity));

	if((pSetup->OpeningCutScene != NULL) && (strlen(pSetup->OpeningCutScene) != 0))
	{
		Play(pSetup->OpeningCutScene, 160, 120, true);
	}
}


/* ------------------------------------------------------------------------------------ */
// Play
//
// Play a .gif or .avi file
/* ------------------------------------------------------------------------------------ */
void CCommonData::Play(const char *szFile, int xPos, int yPos, bool center)
{
	std::string file(szFile);
	MakeLower(file);

	if(EndsWith(file, ".gif"))
	{
		CAnimGif *splash = new CAnimGif(szFile, kVideoFile);
		splash->Play(xPos, yPos, center);
		delete splash;
	}
	else
	{
		CAVIPlayer *splash = new CAVIPlayer;
		splash->Play(szFile, xPos, yPos, center);
		delete splash;
	}
}


/* ------------------------------------------------------------------------------------ */
// CheckMediaPlayers
//
// Poll the status of all of our active media players
/* ------------------------------------------------------------------------------------ */
void CCommonData::CheckMediaPlayers()
{
	if(theCDPlayer)
		theCDPlayer->Check();							// If we have a CD player, check it.

	if(theMIDIPlayer)
		theMIDIPlayer->Check();						// If we have a MIDI player, check it.
}

/* ------------------------------------------------------------------------------------ */
//	OpenRFFile
//
//	This version of OpenRFFile opens a file of a specified type via the
//	.."normal" fopen() calls.  This is NOT to be used to open geVFile
//	..entities, there is an overload that handles this kind of file open
//	..as well.
//
//	This makes it easy to insure that you're opening files in the right
//	..directory for your installation as all the directory names are
//	..loaded from the RealityFactory.ini file in the same directory as
//	..the game runner.
/* ------------------------------------------------------------------------------------ */
FILE *CCommonData::OpenRFFile(int nFileType, const char *szFilename, const char *szHow)
{
	char szTemp[256];

	// Ok, based on the TYPE of file we're looking for, build a "proper"
	// ..filename (fully qualified, more or less).

	switch(nFileType)
	{
    case kActorFile:
		strcpy(szTemp, m_ActorDirectory);
		strcat(szTemp, "\\");
		strcat(szTemp, szFilename);
		break;
    case kAudioFile:
		strcpy(szTemp, m_AudioDirectory);
		strcat(szTemp, "\\");
		strcat(szTemp, szFilename);
		break;
    case kVideoFile:
		strcpy(szTemp, m_VideoDirectory);
		strcat(szTemp, "\\");
		strcat(szTemp, szFilename);
		break;
	case kMIDIFile:
		strcpy(szTemp, m_MIDIDirectory);
		strcat(szTemp, "\\");
		strcat(szTemp, szFilename);
		break;
	case kLevelFile:
		strcpy(szTemp, m_LevelDirectory);
		strcat(szTemp, "\\");
		strcat(szTemp, szFilename);
		break;
	case kAudioStreamFile:
		strcpy(szTemp, m_AudioStreamDirectory);
		strcat(szTemp, "\\");
		strcat(szTemp, szFilename);
		break;
	case kBitmapFile:
		strcpy(szTemp, m_BitmapDirectory);
		strcat(szTemp, "\\");
		strcat(szTemp, szFilename);
		break;
	case kSavegameFile:
		strcpy(szTemp,".\\");
		strcat(szTemp, szFilename);
		break;
	case kTempFile:
		strcpy(szTemp, ".\\");
		strcat(szTemp, szFilename);
		break;
	case kInstallFile:
		strcpy(szTemp, ".\\");
		strcat(szTemp, szFilename);
		break;
	case kScriptFile:
		strcpy(szTemp, ".\\");
		strcat(szTemp, szFilename);
		break;
	case kRawFile:
		strcpy(szTemp, szFilename);
		break;
	default:
		sprintf(szTemp, "[WARNING] OpenRFFile: bad type '%d' for '%s'", nFileType, szFilename);
		CCD->ReportError(szTemp, false);
		return NULL;
	}

	FILE *pTemp = fopen(szTemp, szHow);				// Open the damn thing.

	// Debug tracing time
	if(m_DebugLevel == kHighDebugOutput && !pTemp)
	{
		char szDebug[512];
		sprintf(szDebug, "[WARNING] File open attempt failed on type '%d', file '%s'", nFileType, szTemp);
		CCD->ReportError(szDebug, false);
	}

	return pTemp;									// Back to the caller with it.
}

/* ------------------------------------------------------------------------------------ */
//	OpenRFFile
//
//	This version of OpenRFFile operates identically to the 'fopen()' based
//	..version, but uses the geVFile system to open up virtual files.  This
//	..is provided since some of the G3D API relies on geVFiles.
/* ------------------------------------------------------------------------------------ */
bool CCommonData::OpenRFFile(geVFile **theFp, int nFileType, const char *szFilename, int nHow)
{
	char szTemp[256];

	*theFp = NULL;

	//	Ok, based on the TYPE of file we're looking for, build a "proper"
	//	..filename (fully qualified, more or less).
	switch(nFileType)
	{
    case kActorFile:
		strcpy(szTemp, m_ActorDirectory);
		strcat(szTemp, "\\");
		strcat(szTemp, szFilename);
		break;
    case kAudioFile:
		strcpy(szTemp, m_AudioDirectory);
		strcat(szTemp, "\\");
		strcat(szTemp, szFilename);
		break;
    case kVideoFile:
		strcpy(szTemp, m_VideoDirectory);
		strcat(szTemp, "\\");
		strcat(szTemp, szFilename);
		break;
	case kMIDIFile:
		strcpy(szTemp, m_MIDIDirectory);
		strcat(szTemp, "\\");
		strcat(szTemp, szFilename);
		break;
	case kLevelFile:
		strcpy(szTemp, m_LevelDirectory);
		strcat(szTemp, "\\");
		strcat(szTemp, szFilename);
		break;
	case kAudioStreamFile:
		strcpy(szTemp, m_AudioStreamDirectory);
		strcat(szTemp, "\\");
		strcat(szTemp, szFilename);
		break;
	case kBitmapFile:
		strcpy(szTemp, m_BitmapDirectory);
		strcat(szTemp, "\\");
		strcat(szTemp, szFilename);
		break;
	case kSavegameFile:
		strcpy(szTemp,".\\");
		strcat(szTemp, szFilename);
		break;
	case kTempFile:
		strcpy(szTemp, ".\\");
		strcat(szTemp, szFilename);
		break;
	case kInstallFile:
		strcpy(szTemp,"install\\");
		strcat(szTemp, szFilename);
		break;
	case kScriptFile:
		strcpy(szTemp, ".\\");
		strcat(szTemp, szFilename);
		break;
	case kRawFile:
		strcpy(szTemp, szFilename);
		break;
	default:
		sprintf(szTemp, "[WARNING] OpenRFFile(V): bad type '%d' for '%s'", nFileType, szFilename);
		CCD->ReportError(szTemp, false);
		return NULL;
	}

	//	Ok, open the file up.
	*theFp = geVFile_OpenNewSystem(NULL, GE_VFILE_TYPE_DOS, szTemp, NULL, nHow);


	if(!(*theFp) && VFS)
	{
		if(Logging)
		{
			char szDebug[256];
			sprintf(szDebug, "[INFO] File '%s' not found in Real File System, searching in VFS", szTemp);
			CCD->ReportError(szDebug, false);
		}
		*theFp = geVFile_Open(VFS, szTemp, nHow);
	}

	//	Debug tracing time
	if(m_DebugLevel == kHighDebugOutput && !(*theFp))
	{
		char szDebug[512];
		sprintf(szDebug, "\n[WARNING] OpenRFFile: file open attempt failed on type '%d', file '%s'\n", nFileType, szTemp);
		CCD->ReportError(szDebug, false);
	}

	//	If it worked, return true, otherwise return false.
	if(*theFp != NULL)
	{
		if(Logging)
		{
			char szDebug[256];
			sprintf(szDebug, "[INFO] Opened file %s", szTemp);
			CCD->ReportError(szDebug, false);
		}

		return true;
	}
	else
		return false;
}

/* ------------------------------------------------------------------------------------ */
//	FileExist
/* ------------------------------------------------------------------------------------ */
bool CCommonData::FileExist(int nFileType, const char *szFilename)
{
	char szTemp[256];
	geVFile *theFp = NULL;

	//	Ok, based on the TYPE of file we're looking for, build a "proper"
	//	..filename (fully qualified, more or less).
	switch(nFileType)
	{
    case kActorFile:
		strcpy(szTemp, m_ActorDirectory);
		strcat(szTemp, "\\");
		strcat(szTemp, szFilename);
		break;
    case kAudioFile:
		strcpy(szTemp, m_AudioDirectory);
		strcat(szTemp, "\\");
		strcat(szTemp, szFilename);
		break;
    case kVideoFile:
		strcpy(szTemp, m_VideoDirectory);
		strcat(szTemp, "\\");
		strcat(szTemp, szFilename);
		break;
	case kMIDIFile:
		strcpy(szTemp, m_MIDIDirectory);
		strcat(szTemp, "\\");
		strcat(szTemp, szFilename);
		break;
	case kLevelFile:
		strcpy(szTemp, m_LevelDirectory);
		strcat(szTemp, "\\");
		strcat(szTemp, szFilename);
		break;
	case kAudioStreamFile:
		strcpy(szTemp, m_AudioStreamDirectory);
		strcat(szTemp, "\\");
		strcat(szTemp, szFilename);
		break;
	case kBitmapFile:
		strcpy(szTemp, m_BitmapDirectory);
		strcat(szTemp, "\\");
		strcat(szTemp, szFilename);
		break;
	case kSavegameFile:
		strcpy(szTemp,".\\");
		strcat(szTemp, szFilename);
		break;
	case kTempFile:
		strcpy(szTemp, ".\\");
		strcat(szTemp, szFilename);
		break;
	case kInstallFile:
		strcpy(szTemp,"install\\");
		strcat(szTemp, szFilename);
		break;
	case kScriptFile:
		strcpy(szTemp, ".\\");
		strcat(szTemp, szFilename);
		break;
	case kRawFile:
		strcpy(szTemp, szFilename);
		break;
	default:
		sprintf(szTemp, "[WARNING] File %s - Line %d: FileExist: bad type '%d' for '%s'",
				__FILE__, __LINE__, nFileType, szFilename);
		CCD->ReportError(szTemp, false);
		return false;
	}

	//	Ok, open the file up.
	theFp = geVFile_OpenNewSystem(NULL, GE_VFILE_TYPE_DOS, szTemp, NULL, GE_VFILE_OPEN_READONLY);

	if(!(theFp) && VFS)
		theFp = geVFile_Open(VFS, szTemp, GE_VFILE_OPEN_READONLY);

	if(theFp != NULL)
	{
		geVFile_Close(theFp);
		return true;
	}
	else
		return false;
}

/* ------------------------------------------------------------------------------------ */
//	GetDirectory
//
//	Given the type of directory desired, return a pointer to the appropriate
//	..configured directory.
/* ------------------------------------------------------------------------------------ */
char *CCommonData::GetDirectory(int nType)
{
	char *thePtr = NULL;

	switch(nType)
	{
    case kActorFile:
		thePtr = m_ActorDirectory;
		break;
    case kAudioFile:
		thePtr = m_AudioDirectory;
		break;
    case kVideoFile:
		thePtr = m_VideoDirectory;
		break;
	case kMIDIFile:
		thePtr = m_MIDIDirectory;
		break;
	case kLevelFile:
		thePtr = m_LevelDirectory;
		break;
	case kAudioStreamFile:
		thePtr = m_AudioStreamDirectory;
		break;
	case kBitmapFile:
		thePtr = m_BitmapDirectory;
		break;
	case kSavegameFile:
		thePtr = ".\\";
		break;
	case kTempFile:
		thePtr = ".\\";
		break;
	case kInstallFile:
		thePtr = ".\\";
		break;
	case kScriptFile:
		thePtr = ".\\";
		break;
	case kRawFile:
		thePtr = ".\\";
		break;
	default:
		{
			char szTemp[256];
			sprintf(szTemp, "[WARNING] File %s - Line %d: GetDirectory: bad type '%d'",
					__FILE__, __LINE__, nType);
			CCD->ReportError(szTemp, false);
		}
		return NULL;
	}

	return thePtr;				// Back to caller with...whatever.
}

/* ------------------------------------------------------------------------------------ */
// TimerFunction
/* ------------------------------------------------------------------------------------ */
void CALLBACK CCommonData::TimerFunction(UINT /*uID*/, UINT /*uMsg*/,
										 DWORD dwUser, DWORD /*dw1*/, DWORD /*dw2*/)
{
	CCommonData *thePointer = reinterpret_cast<CCommonData*>(dwUser);

	if(thePointer->HasFocus)
		thePointer->TimeCounter += 1;
}

/* ------------------------------------------------------------------------------------ */
// Get the amount of time passed since the last call, in milliseconds.
// ..Return the result as a DWORD value.
/* ------------------------------------------------------------------------------------ */
DWORD CCommonData::GetTimePassed_D()
{
	geFloat DeltaTime;

	DeltaTime = (geFloat)TimeCounter - (geFloat)LastTimePoll;

	LastTimePoll = TimeCounter;

	LastTimePassed_D = (DWORD)DeltaTime;		// Last elapsed time as DWORD
	LastTimePassed_F = DeltaTime;						// Need in both places

	return (DWORD)DeltaTime;
}

/* ------------------------------------------------------------------------------------ */
// Get the amount of time passed since the last call, in milliseconds.
// ..Return the result as a floating-point value.
/* ------------------------------------------------------------------------------------ */
geFloat CCommonData::GetTimePassed_F()
{

	geFloat DeltaTime;

	DeltaTime = (geFloat)TimeCounter - (geFloat)LastTimePoll;

	LastTimePoll = TimeCounter;

	LastTimePassed_D = (DWORD)DeltaTime;		// Last elapsed time as DWORD
	LastTimePassed_F = DeltaTime;						// Need in both places

	return DeltaTime;
}

/* ------------------------------------------------------------------------------------ */
// Reset the internal timer count variable to the current instant.
// ..This effectively "resets" the game clock.  This is useful for
// ..ignoring a large amount of passed time, such as time spent in
// ..a menu mode or during a level or content load.
/* ------------------------------------------------------------------------------------ */
void CCommonData::ResetClock()
{
	LastTimePoll = TimeCounter;

	LastTimePassed_D = (DWORD)0;
	LastTimePassed_F = (geFloat)0.0f;
}

/* ------------------------------------------------------------------------------------ */
// Return the current performance counter as a DWORD.  This supplies
// ..a free-running counter of DWORD width.
/* ------------------------------------------------------------------------------------ */
DWORD CCommonData::FreeRunningCounter()
{
	return (DWORD)TimeCounter;
}

/* ------------------------------------------------------------------------------------ */
// Return the current performance counter as a float.  This supplies
// ..a free-running counter of floating-point precision
/* ------------------------------------------------------------------------------------ */
geFloat CCommonData::FreeRunningCounter_F()
{
	geFloat TheRealTime;

	TheRealTime = (geFloat)TimeCounter;

	return TheRealTime;
}

/* ------------------------------------------------------------------------------------ */
// Waste time, useful for preventing the Genesis3D engine from
// ..being called so fast that the numbers returned are too
// ..small to be accurate.
/* ------------------------------------------------------------------------------------ */
void CCommonData::Spin(DWORD dwMilliseconds)
{
	__int64 fStart, fCurrent;

	fStart = TimeCounter;

	for(;;)
	{
		if((GetAsyncKeyState(VK_SPACE) & 0x8000) != 0)
			break;

		fCurrent = TimeCounter;

		if((fCurrent - fStart) > dwMilliseconds)
			break;
	}
}

/* ------------------------------------------------------------------------------------ */
// SetChangeLevelData
//
// Store away all the information we need to do a level change.
/* ------------------------------------------------------------------------------------ */
void CCommonData::SetChangeLevelData(struct _ChangeLevel *pItem)
{
	int i;

	if(EffectC_IsStringNull(pItem->szNewLevel))
	{
		m_NewLevel[0] = '\0';
		return;
	}
	else
	{
		strcpy(m_NewLevel, pItem->szNewLevel);
	}

	strcpy(m_SplashScreen,	pItem->szSplashFile	);
	strcpy(m_SplashAudio,	pItem->szAudioFile	);
	strcpy(m_CutScene,		pItem->szCutScene	);
	strcpy(m_Message,		pItem->szMessage	);

	m_Font = pItem->Font;
	strcpy(m_StartPointName, pItem->StartPointName);
	CCD->ActorManager()->GetRotate(thePlayer->GetActor(), &m_playerotation);
	ViewPoint = thePlayer->GetViewPoint();
	CCD->CameraManager()->GetRotation(&theRotation);
	CCD->CameraManager()->GetCameraOffset(&theTranslation, &theRotate);
	CCD->CameraManager()->Get3rdData(&m_defaultdistance, &m_cameraX, &m_cameraY);

	if(pItem->UseOffset)
	{
		geVec3d PP = thePlayer->Position();
		geVec3d_Subtract(&pItem->origin, &PP, &Offset);
	}
	else
	{
		Offset.X = 0.0f;
		Offset.Y = 0.0f;
		Offset.Z = 0.0f;
	}

	UseAngle = pItem->UseAngle;
	KeepAttributes = pItem->KeepAttributes;
	SplashHold = pItem->SplashHold;

	if(KeepAttributes)
	{
		for(i=0; i<40; i++)
			Slot[i] = theWeapon->GetSlot(i);

		CurrentWeapon = theWeapon->GetCurrent();
	}

	UseEffect = pItem->UseEffect;
	cColor = pItem->EffectColor;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
void CCommonData::SetLevelData()
{
	int i;
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	pSet = geWorld_GetEntitySet(CCD->World(), "PlayerSetup");
	pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL);
	PlayerSetup *pSetup = static_cast<PlayerSetup*>(geEntity_GetUserData(pEntity));

	if(!EffectC_IsStringNull(pSetup->Deathlevel))
		strcpy(m_NewLevel, pSetup->Deathlevel);
	else
		strcpy(m_NewLevel, theMenu->GetLevelName());

	m_SplashScreen[0] = '\0';
	m_SplashAudio[0] = '\0';
	m_CutScene[0] = '\0';

	if(EffectC_IsStringNull(pSetup->DeathMessage))
		m_Message[0] = '\0';
	else
		strcpy(m_Message, pSetup->DeathMessage);

	m_Font = pSetup->DeathFontSize;
	strcpy(m_StartPointName, "");
	CCD->ActorManager()->GetRotate(thePlayer->GetActor(), &m_playerotation);
	ViewPoint = thePlayer->GetViewPoint();
	CCD->CameraManager()->GetRotation(&theRotation);
	CCD->CameraManager()->GetCameraOffset(&theTranslation, &theRotate);
	CCD->CameraManager()->Get3rdData(&m_defaultdistance, &m_cameraX, &m_cameraY);
	Offset.X =0.0f;
	Offset.Y =0.0f;
	Offset.Z =0.0f;

	for(i=0; i<40; i++)
		Slot[i] = theWeapon->GetSlot(i);

	CurrentWeapon = theWeapon->GetCurrent();
	KeepAttributes = true;
	SplashHold = false;
}

/* ----------------------------------- END OF FILE ------------------------------------ */
