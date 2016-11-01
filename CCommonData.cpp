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
#include "CFileManager.h"
#include "IniFile.h"
#include "CLanguageManager.h"
#include "CScriptManager.h"
#include "CAudioManager.h"
#include "CGUIManager.h"
#include "CGameStateManager.h"
#include "CMenuState.h"
#include "CPlayState.h"
#include "CCDAudio.h"
#include "CMIDIAudio.h"
#include "CLevel.h"
#include "CHeadsUpDisplay.h"
#include "CConversationManager.h"
#include "CInventory.h"
#include "CArmour.h"
#include "CPolyShadow.h"
#include "Qx\\qxTerrainMgr.h"
#include "HawkNL\\nl.h"
#include <string.h>

/* ------------------------------------------------------------------------------------ */
// Constructor
//
// Initialize all common data pointers, store a pointer to the
// ..Genesis engine object.
/* ------------------------------------------------------------------------------------ */
CCommonData::CCommonData()
{
	m_GameEngine		= NULL;		// Genesis engine class
	m_NetPlayerManager	= NULL;		// multiplayer
	m_InputManager		= NULL;		// User input class
	m_CDPlayer			= NULL;		// CD Audio player class
	m_MIDIPlayer		= NULL;		// MIDI Audio player class
	m_Level				= NULL;
	m_Armour			= NULL;
	m_Menu				= NULL;		// Menu Manager
	m_Collider			= NULL;		// Collision Detection Subsystem
	m_AudioManager		= NULL;		// Audio Manager subsystem
	m_CameraManager		= NULL;		// Camera Manager subsystem
	m_Message[0]		= '\0';
	m_PolyShadow		= NULL;
	m_Inventory			= NULL;
	m_ConversationManager = NULL;

	srand(static_cast<unsigned>(time(NULL)));

	FreeImage_Initialise();

	RFSX::Install(&ScriptManager::Interpreter);

	// Initialize game state data
	m_ChangeLevel = false;					// No level change yet

	m_PlayerName = "Unnamed";

	m_SplashScreen[0]	= '\0';
	m_SplashAudio[0]	= '\0';
	m_CutScene[0]		= '\0';
	m_SplashScreen1[0]	= '\0';
	m_SplashAudio1[0]	= '\0';
	m_CutScene1[0]		= '\0';

	m_HeadBob			= false;
	m_PositionWeapon	= false;
	m_AlterKey			= false;
	m_Paused			= false;
	m_KeyPaused			= false;
	m_MouseControl		= true;
	m_InGameCursor		= false;

	m_UseAngle			= true;
	m_KeepAttributes	= true;
	m_SaveAttributesAsText = false;
	m_ShowTrack			= false;

	m_UseLevelDialog	= false;
	m_NameSelection		= false;
	m_CharacterSelection= false;
	m_DifficultSelection= false;
	m_DifficultLevel	= 1;

	m_HasFocus			= true;

	// start multiplayer
	m_Network			= false;
	if(nlInit())
		m_Network		= true;
	m_Multiplayer		= false;
	// end multiplayer

	// Set up for timekeeping
	m_nTimerID = timeSetEvent(1, 0,	&TimerFunction, reinterpret_cast<DWORD>(this),
							TIME_PERIODIC | TIME_CALLBACK_FUNCTION);

	m_TimeCounter = m_LastTimePoll = 0;
	m_LastTimePassed_D = 0;
	m_LastTimePassed_F = 0;

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
	Level()->Shutdown();
	Shutdown();

	CFileManager::Destroy();

	FreeImage_DeInitialise();

	// start multiplayer
	if(m_Network)
		nlShutdown();
	// end multiplayer

	timeKillEvent(m_nTimerID);

	Log()->Notice("Successful Shutdown.");
}

/* ------------------------------------------------------------------------------------ */
// InitializeCommon
//
// This function initializes all the COMMON entities that survive
// ..between level loads.  These need only be initialized ONCE.
// ..Note that the engine setup is driven from RealityFactory.INI, if this
// ..file doesn't exist, the default (640x480, windowed) is used.
/* ------------------------------------------------------------------------------------ */
int CCommonData::Initialize(HINSTANCE hInstance, char *szStartLevel, bool CommandLine)
{
	bool bFullScreen	= false;	// Default to windowed
	bool bSoftware		= false;	// Default to hardware acceleration
	char chTheDriver	= '(';		// Default to Direct3D
	int nHeight			= 800;		// Default to 800 high
	int nWidth			= 600;		// Default to 600 wide
	FILE *fd;						// Used for prefs file
	char szTitle[80]	= "Game";	// Game title
	char virtualFile[256] = "pack.vfs";
	bool UseCut			= false;
	bool UseCut1		= false;
	bool UseFirst		= false;
	bool UseSecond		= false;
	int logPriority		= LP_NOTICE;
	std::string defaultLanguage("English");

	m_CmdLine = CommandLine;

	Log()->Debug("Parsing RealityFactory.ini file");

	// Ok, let's see if we have an initialization file, and if so, read it in and parse it.
	if((fd = fopen(".\\RealityFactory.ini", "rt")) != NULL)
	{
		// File there, parse it!
		char szInputLine[132];

		while(fgets(szInputLine, 132, fd) != NULL)
		{
			if(szInputLine[0] == ';')
				continue;				// Comment line

			if(strlen(szInputLine) <= 5)
				continue;				// Skip blank lines

			// All config commands are "thing=value"
			char *szAtom = strtok(szInputLine," =");
			char *szArg  = strtok(NULL, " \n");

			if(!stricmp(szAtom, "packfile"))
			{
				if(szArg != NULL)	strcpy(virtualFile, szArg);
			}
			else if(!stricmp(szAtom, "fullscreen"))
			{
				// Set fullscreen/windowed
				bFullScreen = !stricmp(szArg, "true");
			}
			else if(!stricmp(szAtom, "logging"))
			{
				if(!stricmp(szArg, "true")) ///< @deprecated There are several log priorities now
				{
					logPriority = LP_DEBUG;
					Log()->Info("RealityFactory.ini: value 'true' for key 'logging' is deprecated");
				}
				else if(!stricmp(szArg, "debug"))		logPriority = LP_DEBUG;
				else if(!stricmp(szArg, "info"))		logPriority = LP_INFO;
				else if(!stricmp(szArg, "notice"))		logPriority = LP_NOTICE;
				else if(!stricmp(szArg, "warning"))		logPriority = LP_WARNING;
				else if(!stricmp(szArg, "error"))		logPriority = LP_ERROR;
			}
			else if(!stricmp(szAtom, "width"))
			{
				// Set display width
				int nTemp = szArg ? atoi(szArg) : 0;
				if(nTemp > 0)		nWidth = nTemp;
			}
			else if(!stricmp(szAtom, "height"))
			{
				// Set display height
				int nTemp = szArg ? atoi(szArg) : 0;
				if(nTemp > 0)		nHeight = nTemp;
			}
			else if(!stricmp(szAtom, "driver"))
			{
				if(szArg != NULL)
				{
					if(!stricmp(szArg, "d3d"))			chTheDriver = '(';
					else if(!stricmp(szArg, "d3d16"))	chTheDriver = 'D';	// Use Direct3D in fullscreen
					else if(!stricmp(szArg, "opengl"))	chTheDriver = 'O';
					else if(!stricmp(szArg, "wire"))	chTheDriver = 'W';
					else if(!stricmp(szArg, "auto"))	chTheDriver = 'A';	// Use Auto Detect
					else if(!stricmp(szArg, "pick"))	chTheDriver = 'P';	// Pop a driver pick list
					else
					{
						Log()->Warning("Bad driver selection in RealityFactory.ini.");
						chTheDriver = 'P';
					}
				}
			}
			else
			{
				Log()->Warning("Unknown option '%s' in RealityFactory.ini.", szAtom);
			}
		}

		fclose(fd);
	}

	// Set log priority
	Log()->SetPriority(logPriority);

	// Initialize Pawn Method Hash Table
	ScriptManager::InitMHT();

	// set up all directories
	CFileManager::GetSingletonPtr()->Create(virtualFile);

	std::string logDir(CFileManager::GetSingletonPtr()->GetDirectory(kLogFile));
	// set log file
	if(!sxLog::GetSingletonPtr()->SetFile(logDir + "\\RealityFactory.log", false))
	{
		MessageBox(NULL, "Failed to open Log file!", "RF Error Report", MB_ICONERROR | MB_OK);
	}

	// Parse the internal_config.ini file
	{
		CIniFile iniFile("internal_config.ini");

		if(!iniFile.ReadFile())
		{
			sxLog::GetSingletonPtr()->Critical("File %s - Line %d: Failed to open internal_config.ini file!", __FILE__, __LINE__);
		}
		else
		{
			std::string value;

			// General
			value = iniFile.GetValue("General", "GameName");
			if(!value.empty())	strcpy(szTitle, value.c_str());

			m_CharacterSelection = iniFile.GetValueB("General", "UseCharSelect");

			m_NameSelection = iniFile.GetValueB("General", "UseNameSelect");

			m_DifficultSelection = iniFile.GetValueB("General", "UseDifficultLevel");

			m_DifficultLevel = iniFile.GetValueI("General", "DefaultDifficulty");
			if(m_DifficultLevel < 1)		m_DifficultLevel = 1;
			else if(m_DifficultLevel > 3)	m_DifficultLevel = 3;

			value = iniFile.GetValue("General", "DefaultLanguage");
			if(!value.empty())	defaultLanguage = value;

			value = iniFile.GetValue("General", "StartLevel");
			if(!value.empty() && !CommandLine)	strcpy(szStartLevel, value.c_str());

			m_InGameCursor = iniFile.GetValueB("General", "InGameCursor");
			if(m_InGameCursor)	m_MouseControl = false;

			m_SaveAttributesAsText = iniFile.GetValueB("General", "SaveAttributesAsText");

			// Player
			value = iniFile.GetValue("Player", "PlayerName");
			if(!value.empty())	m_PlayerName = value;

			m_PlayerAvatar = iniFile.GetValue("Player", "PlayerAvatar");
			if(m_PlayerAvatar.empty())	m_PlayerAvatar = "virgil.act";

			// Debug
			value = iniFile.GetValue("Debug", "WeaponPosition");
			if(value == "On" || value == "on")	m_PositionWeapon = true;
			else								m_PositionWeapon = false;

			// weapon positioning with alternate key configuration
			if(iniFile.GetValue("Debug", "AlternateKey") == "true")
				m_AlterKey = true;

			if(CommandLine)	m_UseLevelDialog = false;
			else			m_UseLevelDialog = iniFile.GetValueB("Debug", "UseLevelDialog");

			m_ShowTrack	= iniFile.GetValueB("Debug", "ShowTrack");

			// SplashScreen
			UseFirst = iniFile.GetValueB("SplashScreen", "UseFirst");

			UseCut = iniFile.GetValueB("SplashScreen", "UseCutScene");

			value = iniFile.GetValue("SplashScreen", "SplashScreen");
			if(!value.empty())	strcpy(m_SplashScreen, value.c_str());

			value = iniFile.GetValue("SplashScreen", "SplashAudio");
			if(!value.empty())	strcpy(m_SplashAudio, value.c_str());

			value = iniFile.GetValue("SplashScreen", "CutScene");
			if(!value.empty())	strcpy(m_CutScene, value.c_str());

			UseSecond = iniFile.GetValueB("SplashScreen", "UseSecond");

			UseCut1 = iniFile.GetValueB("SplashScreen", "UseCutScene1");

			value = iniFile.GetValue("SplashScreen", "SplashScreen1");
			if(!value.empty())	strcpy(m_SplashScreen1, value.c_str());

			value = iniFile.GetValue("SplashScreen", "SplashAudio1");
			if(!value.empty())	strcpy(m_SplashAudio1, value.c_str());

			value = iniFile.GetValue("SplashScreen", "CutScene1");
			if(!value.empty())	strcpy(m_CutScene1, value.c_str());
		}
	}

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

	Log()->Notice("Initializing Genesis3D Graphics Engine...");
	m_GameEngine = new CGenesisEngine(bFullScreen, nWidth, nHeight, szTitle, hInstance,
										chTheDriver, bSoftware, m_UseLevelDialog, szStartLevel);

	if(m_GameEngine == NULL)
	{
		Log()->Critical("Failed to create Genesis3D Graphics Engine!");
		return -1;
	}

	// Fire up a camera for us to see through
	Log()->Notice("Initializing Camera Manager...");
	m_CameraManager = CCameraManager::GetSingletonPtr();

	if(m_CameraManager == NULL)
	{
		Log()->Critical("Failed to create Camera Manager!");
		return -93;
	}

	geEngine_SetGamma(m_GameEngine->Engine(), 1.0f);

	Log()->Notice("Initializing GUI Subsystem...");
	m_GUIManager = new CGUIManager(	m_GameEngine->Engine(),
									m_GameEngine->Width(),
									m_GameEngine->Height(),
									m_GameEngine->DummyWorld(),
									CFileManager::GetSingletonPtr()->GetVFS(),
									logDir + "\\CEGUI.log");

	if(m_GUIManager == NULL)
	{
		Log()->Critical("Failed to create GUI subsystem!");
		return -2;
	}

	{
		std::string savesDir(CFileManager::GetSingletonPtr()->GetDirectory(kSavegameFile));
		m_GUIManager->SetResourceGroupDirectory("saves", savesDir + "\\");
	}

	Log()->Notice("Initializing Game State Manager Subsystem...");
	m_GameStateManager = CGameStateManager::GetSingletonPtr();

	if(m_GameStateManager == NULL)
	{
		Log()->Critical("Failed to create Game State Manager!");
		return -2;
	}
	CPlayState::GetSingletonPtr()->ShowCursor(m_InGameCursor);


	Log()->Notice("Initializing Language Manager Subsystem...");
	m_LanguageManager = new CLanguageManager(defaultLanguage);

	if(m_LanguageManager == NULL)
	{
		Log()->Critical("Failed to create Language Manager!");
		return -2;
	}


	// We have a 3D engine, now initialize the user input subsystem
	Log()->Notice("Initializing User Input Subsystem...");
	m_InputManager = CInput::GetSingletonPtr();

	if(m_InputManager == NULL)
	{
		Log()->Critical("Failed to create input subsystem!");
		return -2;
	}
	m_InputManager->Initialize(m_GameEngine->WindowHandle(), m_GameEngine->Width(), m_GameEngine->Height());
	m_InputManager->AddKeyListener(m_GameStateManager, "GameStateManager");
	m_InputManager->AddMouseListener(m_GameStateManager, "GameStateManager");


	// The Audio Manager needs to be prepared...
	Log()->Notice("Initializing Audio Manager Subsystem...");
	m_AudioManager = new CAudioManager();

	if(m_AudioManager == NULL)
	{
		Log()->Critical("Failed to create Audio Manager!");
		return -92;
	}


	// Get the CD Audio player working.  Note that it's not a fatal error to
	// ..not have a CD Player device, as this system may be using the General
	// ..MIDI soundtrack instead.
	Log()->Notice("Initializing CD Audio Manager Subsystem...");
	m_CDPlayer = new CCDAudio();

	if(m_CDPlayer == NULL)
	{
		Log()->Error("CD Player failed to instantiate!");
	}


	// Fire up the MIDI playback system.  Again, as with the CD Player, failure
	// ..to instantiate is not grounds for aborting game play, although I'd
	// ..personally consider the game unplayable without MY soundtrack (heh).
	Log()->Notice("Initializing Midi Audio Manager Subsystem...");
	m_MIDIPlayer = new CMIDIAudio();

	if(m_MIDIPlayer == NULL)
	{
		Log()->Error("MIDI Player failed to instantiate!");
	}


	Log()->Notice("Initializing Level Subsystem...");
	m_Level = new CLevel();

	if(m_Level == NULL)
	{
		Log()->Critical("Failed to create Level handler!");
		return -2;
	}


	Log()->Notice("Initializing RF Menu Manager Subsystem...");
	m_Menu = new CRFMenu(szStartLevel);

	if(m_Menu == NULL)
	{
		Log()->Critical("Failed to create Menu subsystem!");
		return -2;
	}
	m_Menu->Initialize();


	Log()->Notice("Initializing Collision Manager Subsystem...");
	m_Collider = new Collider();

	if(m_Collider == NULL)
	{
		Log()->Critical("Failed to create Collider subsystem");
		return -100;
	}


	// start multiplayer
	Log()->Notice("Initializing Network Manager Subsystem...");
	m_NetPlayerManager = new NetPlayerMgr();

	if(m_NetPlayerManager == NULL)
	{
		Log()->Critical("Failed to create Network Manager subsystem!");
		return -100;
	}
	// end multiplayer


	// Finally, initialize the AVIFile library.  This is done independent of
	// ..any AVI-specific classes so that we can guarantee only ONE instance
	// ..of the library is loaded.
	Log()->Notice("Initializing AVIFile Video Subsystem...");
	AVIFileInit();


	Log()->Notice("Initializing Shadow Manager Subsystem...");
	m_PolyShadow = new CPolyShadow();

	if(m_PolyShadow == NULL)
	{
		Log()->Critical("Failed to create Poly Shadow handling class!");
		return -2;
	}


	Log()->Notice("Initializing Armour Subsystem...");
	m_Armour = new CArmour();

	if(m_Armour == NULL)
	{
		Log()->Critical("Failed to create Armour handler!");
		return -2;
	}


	Log()->Notice("Initializing Inventory Subsystem...");
	m_Inventory = sxInventory::GetSingletonPtr();

	if(m_Inventory == NULL)
	{
		Log()->Critical("Failed to create Inventory handler!");
		return -2;
	}
	m_Inventory->Initialize();


	Log()->Notice("Initializing Conversation Subsystem...");
	m_ConversationManager = sxConversationManager::GetSingletonPtr();

	if(m_ConversationManager == NULL)
	{
		Log()->Critical("Failed to create Conversation handler!");
		return -2;
	}
	m_ConversationManager->Initialize();

	// Common subsystems initialized, back to caller!
	return 0;
}

// start multiplayer
/* ------------------------------------------------------------------------------------ */
// ShutDownNetWork
/* ------------------------------------------------------------------------------------ */
void CCommonData::ShutDownNetWork()
{
	SAFE_DELETE(m_NetPlayerManager);

	m_NetPlayerManager = new NetPlayerMgr();

	if(m_NetPlayerManager == NULL)
	{
		Log()->Error("Failed to create Network Manager subsystem!", false);
		return;
	}
}

/* ------------------------------------------------------------------------------------ */
// SetMultiPlayer
/* ------------------------------------------------------------------------------------ */
void CCommonData::SetMultiPlayer(bool multi, bool server)
{
	m_Multiplayer = multi;
	m_Server = server;
}

/* ------------------------------------------------------------------------------------ */
// GetMultiPlayer
/* ------------------------------------------------------------------------------------ */
bool CCommonData::GetMultiPlayer()
{
	return (m_Multiplayer && GetNetwork());
}
// end multiplayer


void CCommonData::StartGame(bool commandLine)
{
	m_GameStateManager->ChangeState(CMenuState::GetSingletonPtr());

	if(commandLine)
	{
		if(GetCharacterSelection())
		{
			m_Menu->SetActiveSection("character", true);
		}
		else if(GetNameSelection())
		{
			m_Menu->SetActiveSection("playername", true);
		}
		else if(GetDifficultSelection())
		{
			m_Menu->SetActiveSection("difficulty", true);
		}
		else
		{
			m_Menu->DoGame(true);
		}
	}

	m_GameStateManager->RunGame();
}


/* ------------------------------------------------------------------------------------ */
// Shutdown
//
// Shut down all the common classes, like the game engine and such,
// ..in preparation for exiting the game.
/* ------------------------------------------------------------------------------------ */
void CCommonData::Shutdown()
{
	SAFE_DELETE(m_Level);
	SAFE_DELETE(m_Armour);
	SAFE_DELETE(m_PolyShadow);
	SAFE_DELETE(m_Inventory);
	SAFE_DELETE(m_ConversationManager);

	// Shut down the AVI system
	Log()->Notice("Shutting Down AVIFile Video Subsystem...");
	AVIFileExit();

	// Clean up all the various subsystems before exiting.  Note that you
	// ..must delete all the components BEFORE you delete the engine, as
	// ..the engine is used to free sounds, etc. in many of the components.
	// start multiplayer
	Log()->Notice("Shutting Down Network Manager Subsystem...");
	SAFE_DELETE(m_NetPlayerManager);
	// end multiplayer

	Log()->Notice("Shutting Down Collision Manager Subsystem...");
	SAFE_DELETE(m_Collider);

	Log()->Notice("Shutting Down Game State Manager Subsystem...");
	SAFE_DELETE(m_GameStateManager);

	Log()->Notice("Shutting Down Menu Manager Subsystem...");
	SAFE_DELETE(m_Menu);

	Log()->Notice("Shutting Down User Input Manager Subsystem...");
	SAFE_DELETE(m_InputManager);

	Log()->Notice("Shutting Down CD Audio Manager Subsystem...");
	SAFE_DELETE(m_CDPlayer);

	Log()->Notice("Shutting Down Midi Audio Manager Subsystem...");
	SAFE_DELETE(m_MIDIPlayer);

	Log()->Notice("Shutting Down Audio Manager Subsystem...");
	SAFE_DELETE(m_AudioManager);

	Log()->Notice("Shutting Down Language Manager Subsystem...");
	SAFE_DELETE(m_LanguageManager);

	Log()->Notice("Shutting Down GUI Manager Subsystem...");
	SAFE_DELETE(m_GUIManager);

	Log()->Notice("Shutting Down Camera Manager Subsystem...");
	SAFE_DELETE(m_CameraManager);

	Log()->Notice("Shutting Down Graphics Subsystem...");
	SAFE_DELETE(m_GameEngine);

	Log()->Notice("Restoring Mouse Cursor...");
	ShowCursor(TRUE);
}


/* ------------------------------------------------------------------------------------ */
// SaveTo
/* ------------------------------------------------------------------------------------ */
int CCommonData::SaveTo(FILE *SaveFD)
{
	// TODO fix
	//fwrite(&m_JumpKey,					sizeof(bool),		1,		SaveFD);
	//fwrite(&m_RunKey,					sizeof(bool),		1,		SaveFD);
	//fwrite(&m_CrouchKey,				sizeof(bool),		1,		SaveFD);
	//fwrite(&m_LightKey,					sizeof(bool),		1,		SaveFD);
	fwrite(&m_NewLevel,					sizeof(char),		256,	SaveFD);
	fwrite(&m_SplashScreen,				sizeof(char),		256,	SaveFD);
	fwrite(&m_SplashAudio,				sizeof(char),		256,	SaveFD);
	fwrite(&m_CutScene,					sizeof(char),		256,	SaveFD);
	fwrite(&m_Message,					sizeof(char),		256,	SaveFD);
	//fwrite(&m_Font,						sizeof(int),		1,		SaveFD);
	fwrite(&m_StartPointName,			sizeof(char),		256,	SaveFD);
	fwrite(&m_PlayerOffset,				sizeof(geVec3d),	1,		SaveFD);
	fwrite(&m_UseAngle,					sizeof(bool),		1,		SaveFD);
	fwrite(&m_KeepAttributes,			sizeof(bool),		1,		SaveFD);
	// TODO BUG:
	//fwrite(&m_CurrentWeapon,			sizeof(int),		1,		SaveFD);
	// MAX_WEAPONS = 40
	//fwrite(&m_Slot,						sizeof(int),		40,		SaveFD);

	fwrite(&m_CameraRotation,			sizeof(geVec3d),	1,		SaveFD);
	fwrite(&m_CameraTranslationOffset,	sizeof(geVec3d),	1,		SaveFD);
	fwrite(&m_CameraRotationOffset,		sizeof(geVec3d),	1,		SaveFD);
	fwrite(&m_DefaultDistance,			sizeof(float),		1,		SaveFD);
	fwrite(&m_CameraX,					sizeof(float),		1,		SaveFD);
	fwrite(&m_CameraY,					sizeof(float),		1,		SaveFD);
	fwrite(&m_PlayerRotation,			sizeof(geVec3d),	1,		SaveFD);
	fwrite(&m_ViewPoint,				sizeof(int),		1,		SaveFD);
	fwrite(&m_DifficultLevel,			sizeof(int),		1,		SaveFD);
	//fwrite(&m_ZoomKey,					sizeof(bool),		1,		SaveFD);
	//fwrite(&m_UseKey,					sizeof(bool),		1,		SaveFD);
	//fwrite(&m_InvKey,					sizeof(bool),		1,		SaveFD);
	//fwrite(&m_DropKey,					sizeof(bool),		1,		SaveFD);
	//fwrite(&m_ReloadKey,				sizeof(bool),		1,		SaveFD);
	fwrite(&m_CharacterSelection,		sizeof(bool),		1,		SaveFD);
	fwrite(&m_NameSelection,			sizeof(bool),		1,		SaveFD);

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// RestoreFrom
/* ------------------------------------------------------------------------------------ */
int CCommonData::RestoreFrom(FILE *RestoreFD)
{
	// TODO fix
	//fread(&m_JumpKey,					sizeof(bool),		1,		RestoreFD);
	//fread(&m_RunKey,					sizeof(bool),		1,		RestoreFD);
	//fread(&m_CrouchKey,					sizeof(bool),		1,		RestoreFD);
	//fread(&m_LightKey,					sizeof(bool),		1,		RestoreFD);
	fread(&m_NewLevel,					sizeof(char),		256,	RestoreFD);
	fread(&m_SplashScreen,				sizeof(char),		256,	RestoreFD);
	fread(&m_SplashAudio,				sizeof(char),		256,	RestoreFD);
	fread(&m_CutScene,					sizeof(char),		256,	RestoreFD);
	fread(&m_Message,					sizeof(char),		256,	RestoreFD);
	//fread(&m_Font,						sizeof(int),		1,		RestoreFD);
	fread(&m_StartPointName,			sizeof(char),		256,	RestoreFD);
	fread(&m_PlayerOffset,				sizeof(geVec3d),	1,		RestoreFD);
	fread(&m_UseAngle,					sizeof(bool),		1,		RestoreFD);
	fread(&m_KeepAttributes,			sizeof(bool),		1,		RestoreFD);
	// TODO BUG
	//fread(&m_CurrentWeapon,				sizeof(int),		1,		RestoreFD);
	// MAX_WEAPONS = 40
	//fread(&m_Slot,						sizeof(int),		40,		RestoreFD);

	fread(&m_CameraRotation,			sizeof(geVec3d),	1,		RestoreFD);
	fread(&m_CameraTranslationOffset,	sizeof(geVec3d),	1,		RestoreFD);
	fread(&m_CameraRotationOffset,		sizeof(geVec3d),	1,		RestoreFD);
	fread(&m_DefaultDistance,			sizeof(float),		1,		RestoreFD);
	fread(&m_CameraX,					sizeof(float),		1,		RestoreFD);
	fread(&m_CameraY,					sizeof(float),		1,		RestoreFD);
	fread(&m_PlayerRotation,			sizeof(geVec3d),	1,		RestoreFD);
	fread(&m_ViewPoint,					sizeof(int),		1,		RestoreFD);
	fread(&m_DifficultLevel,			sizeof(int),		1,		RestoreFD);
	//fread(&m_ZoomKey,					sizeof(bool),		1,		RestoreFD);
	//fread(&m_UseKey,					sizeof(bool),		1,		RestoreFD);
	//fread(&m_InvKey,					sizeof(bool),		1,		RestoreFD);
	//fread(&m_DropKey,					sizeof(bool),		1,		RestoreFD);
	//fread(&m_ReloadKey,					sizeof(bool),		1,		RestoreFD);
	fread(&m_CharacterSelection,		sizeof(bool),		1,		RestoreFD);
	fread(&m_NameSelection,				sizeof(bool),		1,		RestoreFD);

	HUD()->LoadConfiguration();
	HUD()->Activate();

	return RGF_SUCCESS;
}


// TODO remove
qxTerrainMgr*		CCommonData::TerrainManager()
{ return m_Level->TerrainManager();		}

CPlayer*			CCommonData::Player()
{ return m_Level->Player();				}

CActorManager*		CCommonData::ActorManager()
{ return m_Level->ActorManager();		}

CModelManager*		CCommonData::ModelManager()
{ return m_Level->ModelManager();		}

CWeapon*			CCommonData::Weapons()
{ return m_Level->WeaponManager();		}

CHeadsUpDisplay*	CCommonData::HUD()
{ return m_Level->HUD();				}

CEntityRegistry*	CCommonData::EntityRegistry()
{ return m_Level->EntityRegistry();		}

EffManager*			CCommonData::EffectManager()
{ return m_Level->EffectManager();		}

CPreEffect*			CCommonData::Effect()
{ return m_Level->PreEffects();			}

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
	if(m_MIDIPlayer)
		m_MIDIPlayer->Stop();	// Shut down MIDI, if any

	if(m_CDPlayer)
		m_CDPlayer->Stop();		// Shut down CD Audio, if any

	Level()->AudioStreams()->StopAll();

	// Uh-oh, time to change levels!  This is somewhat messy but
	// ..hopefully easy to understand.  We shut down the current level
	// ..and load the new one here.

	if(!EffectC_IsStringNull(m_NewLevel))
		strcpy(szLevelFile, m_NewLevel);	// Save off before...

	Log()->Debug("Attempting changelevel to level '%s'", szLevelFile);

	// Save off player attributes during level change..
	Player()->SaveAttributes("temp.bin");
	Player()->SaveAttributesAscii("attributes.txt");

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

		if(EndsWith(File, ".bmp"))
		{
			m_GameEngine->DisplaySplash(m_SplashScreen, m_SplashAudio);
		}
		else
		{
			CIniFile AttrFile;
			AttrFile.SetPath("splash.ini");

			if(AttrFile.ReadFile())
			{
				std::string keyName = AttrFile.FindFirstKey();
				std::string name, value;

				while(!keyName.empty())
				{
					if(keyName == m_SplashScreen)
					{
						int count = 0;
						name = AttrFile.FindFirstName(keyName);
						value = AttrFile.FindFirstValue();

						while(!name.empty())
						{
							++count;
							name = AttrFile.FindNextName();
							value = AttrFile.FindNextValue();
						}

						int index = EffectC_rand(1, count);
						count = 1;
						name = AttrFile.FindFirstName(keyName);
						value = AttrFile.FindFirstValue();

						while(count != index)
						{
							++count;
							name = AttrFile.FindNextName();
							value = AttrFile.FindNextValue();
						}

						m_GameEngine->DisplaySplash(name.c_str(), m_SplashAudio);
						break;
					}

					keyName = AttrFile.FindNextKey();
				}
			}
		}

		if(EffectC_IsStringNull(m_NewLevel))
			Spin(5000);
	}
	else
	{
		if(!EffectC_IsStringNull(m_Message))
		{
			if(m_HasFocus)
			{
				geRect cameraRect;
				geCamera *camera;
				cameraRect.Left		= 0;
				cameraRect.Right	= m_GameEngine->Width()  - 1;
				cameraRect.Top		= 0;
				cameraRect.Bottom	= m_GameEngine->Height() - 1;
				camera = geCamera_Create(2.0f, &cameraRect);

				if(camera)
				{
					CEGUI::Font* font;
					if(!m_Font.empty() && CEGUI::FontManager::getSingleton().isFontPresent(m_Font))
						font = CEGUI::FontManager::getSingleton().getFont(m_Font);
					else
						font = CEGUI::System::getSingleton().getDefaultFont();

					float textWidth = font->getTextExtent(m_Message);
					float fontHeight = font->getFontHeight();
					CEGUI::Rect draw_area( (m_GameEngine->Width() - textWidth) / 2.f,
											m_GameEngine->Height() / 2.f - fontHeight,
											m_GameEngine->Width()  - 1.f,
											m_GameEngine->Height() - 1.f);
					for(int i=0; i<3; ++i) // max triple buffer
					{
						geEngine_BeginFrame(m_GameEngine->Engine(), camera, GE_FALSE);
						m_GUIManager->DrawText(m_Message, m_Font, draw_area, 1.0f);
						geEngine_EndFrame(m_GameEngine->Engine());
					}

					geCamera_Destroy(&camera);
				}
			}
		}
	}

	// Shut the current level down
	Level()->Shutdown();

	SetKeyPaused(false);

	// Ok, load in the new level

	if(!EffectC_IsStringNull(m_NewLevel))
	{
		if(Level()->Initialize(szLevelFile) != 0)
		{
			// I regard this as a catastrophic error, so we'll log it and
			// ..commit suicide.
			Log()->Critical("File %s - Line %d: Failed to load Level %s",
							__FILE__, __LINE__, szLevelFile);

			Level()->Shutdown();
			Shutdown();
			exit(-334);
		}

		HUD()->LoadConfiguration();

		// Restore the player attributes for the new level
		if(m_KeepAttributes)
			Player()->LoadAttributes("temp.bin");
		else
			Player()->LoadAttributes("pdoa.bin");

		m_KeepAttributes = true;

		TerrainManager()->Init();

		// Move the player avatar to the start of the new level
		Player()->MoveToStart();

		geVec3d ActPos = Player()->Position();
		geVec3d_Subtract(&ActPos, &m_PlayerOffset, &ActPos);
		ActorManager()->Position(Player()->GetActor(), ActPos);

		if(!Player()->GetMonitorMode())
		{
			//m_Player->SwitchCamera(ViewPoint);
			if(m_ViewPoint == Player()->GetViewPoint())
			{
				if(!m_UseAngle)
				{
					Level()->ActorManager()->Rotate(Player()->GetActor(), m_PlayerRotation);
					CameraManager()->SetRotation(m_CameraRotation);
				}
				CameraManager()->SetCameraOffset(m_CameraTranslationOffset, m_CameraRotationOffset);
				CameraManager()->Set3rdData(m_DefaultDistance, m_CameraX, m_CameraY);
			}
		}

		int i;

		// MAX_WEAPONS = 40
		for(i=0; i<MAX_WEAPONS; ++i)
			Level()->WeaponManager()->SetSlot(i, m_Slot[i]);

		if(!Player()->GetMonitorMode())
			Level()->WeaponManager()->ResetWeapon(m_CurrentWeapon);

		geEntity_EntitySet *pSet;
		geEntity *pEntity;

		pSet = geWorld_GetEntitySet(World(), "PlayerSetup");
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

		m_Font = pSetup->DeathFont;

		if(!EffectC_IsStringNull(pSetup->Deathlevel))
		{
			strncpy(m_NewLevel, pSetup->Deathlevel, 255);
			m_NewLevel[255] = '\0';
		}

		m_ChangeLevel = false;
		m_Paused = true;

		if(m_SplashHold && !EffectC_IsStringNull(m_SplashScreen))
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
		return false;
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
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(m_GameEngine->World(), "PlayerSetup");

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
	if(m_CDPlayer)
		m_CDPlayer->Check();		// If we have a CD player, check it.

	if(m_MIDIPlayer)
		m_MIDIPlayer->Check();		// If we have a MIDI player, check it.
}


/* ------------------------------------------------------------------------------------ */
// TimerFunction
/* ------------------------------------------------------------------------------------ */
void CALLBACK CCommonData::TimerFunction(UINT /*uID*/, UINT /*uMsg*/,
										 DWORD dwUser, DWORD /*dw1*/, DWORD /*dw2*/)
{
	CCommonData *thePointer = reinterpret_cast<CCommonData*>(dwUser);

	if(thePointer->m_HasFocus)
		thePointer->m_TimeCounter += 1;
}

/* ------------------------------------------------------------------------------------ */
// Get the amount of time passed since the last call, in milliseconds.
// ..Return the result as a DWORD value.
/* ------------------------------------------------------------------------------------ */
DWORD CCommonData::GetTimePassed_D()
{
	float DeltaTime;

	DeltaTime = static_cast<float>(m_TimeCounter - m_LastTimePoll);

	m_LastTimePoll = m_TimeCounter;

	m_LastTimePassed_D = static_cast<DWORD>(DeltaTime);	// Last elapsed time as DWORD
	m_LastTimePassed_F = DeltaTime;						// Need in both places

	return m_LastTimePassed_D;
}

/* ------------------------------------------------------------------------------------ */
// Get the amount of time passed since the last call, in milliseconds.
// ..Return the result as a floating-point value.
/* ------------------------------------------------------------------------------------ */
float CCommonData::GetTimePassed_F()
{
	float DeltaTime;

	DeltaTime = static_cast<float>(m_TimeCounter - m_LastTimePoll);

	m_LastTimePoll = m_TimeCounter;

	m_LastTimePassed_D = static_cast<DWORD>(DeltaTime);	// Last elapsed time as DWORD
	m_LastTimePassed_F = DeltaTime;						// Need in both places

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
	m_LastTimePoll = m_TimeCounter;

	m_LastTimePassed_D = 0;
	m_LastTimePassed_F = 0.0f;
}

/* ------------------------------------------------------------------------------------ */
// Return the current performance counter as a DWORD.  This supplies
// ..a free-running counter of DWORD width.
/* ------------------------------------------------------------------------------------ */
DWORD CCommonData::FreeRunningCounter()
{
	return static_cast<DWORD>(m_TimeCounter);
}

/* ------------------------------------------------------------------------------------ */
// Return the current performance counter as a float.  This supplies
// ..a free-running counter of floating-point precision
/* ------------------------------------------------------------------------------------ */
float CCommonData::FreeRunningCounter_F()
{
	return static_cast<geFloat>(m_TimeCounter);
}

/* ------------------------------------------------------------------------------------ */
// Waste time, useful for preventing the Genesis3D engine from
// ..being called so fast that the numbers returned are too
// ..small to be accurate.
/* ------------------------------------------------------------------------------------ */
void CCommonData::Spin(DWORD dwMilliseconds)
{
	__int64 fStart, fCurrent;

	fStart = m_TimeCounter;

	for(;;)
	{
		if((GetAsyncKeyState(VK_SPACE) & 0x8000) != 0)
			break;

		fCurrent = m_TimeCounter;

		if((fCurrent - fStart) > dwMilliseconds)
			break;
	}
}

/* ------------------------------------------------------------------------------------ */
// SetChangeLevelData
//
// Store away all the information we need to do a level change.
/* ------------------------------------------------------------------------------------ */
void CCommonData::SetChangeLevelData(const _ChangeLevel *pItem)
{
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
	Level()->ActorManager()->GetRotate(Player()->GetActor(), &m_PlayerRotation);
	m_ViewPoint = Player()->GetViewPoint();
	CameraManager()->GetRotation(&m_CameraRotation);
	CameraManager()->GetCameraOffset(&m_CameraTranslationOffset, &m_CameraRotationOffset);
	CameraManager()->Get3rdData(&m_DefaultDistance, &m_CameraX, &m_CameraY);

	if(pItem->UseOffset)
	{
		geVec3d PP = Player()->Position();
		geVec3d_Subtract(&pItem->origin, &PP, &m_PlayerOffset);
	}
	else
	{
		m_PlayerOffset.X = 0.0f;
		m_PlayerOffset.Y = 0.0f;
		m_PlayerOffset.Z = 0.0f;
	}

	m_UseAngle = pItem->UseAngle;
	m_KeepAttributes = pItem->KeepAttributes;
	m_SplashHold = pItem->SplashHold;

	if(m_KeepAttributes)
	{
		int i;
		for(i=0; i<MAX_WEAPONS; ++i)
			m_Slot[i] = Level()->WeaponManager()->GetSlot(i);

		m_CurrentWeapon = Level()->WeaponManager()->GetCurrent();
	}
	else
	{
		int i;
		for(i=0; i<MAX_WEAPONS; ++i)
			m_Slot[i].clear();
		m_CurrentWeapon.clear();
	}

	m_UseEffect = pItem->UseEffect;
	m_EffectColor = pItem->EffectColor;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
void CCommonData::SetLevelData()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	pSet = geWorld_GetEntitySet(World(), "PlayerSetup");
	pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL);
	PlayerSetup *pSetup = static_cast<PlayerSetup*>(geEntity_GetUserData(pEntity));

	if(!EffectC_IsStringNull(pSetup->Deathlevel))
		strcpy(m_NewLevel, pSetup->Deathlevel);
	else
		strcpy(m_NewLevel, m_Menu->GetLevelName());

	m_SplashScreen[0] = '\0';
	m_SplashAudio[0] = '\0';
	m_CutScene[0] = '\0';

	if(EffectC_IsStringNull(pSetup->DeathMessage))
		m_Message[0] = '\0';
	else
		strcpy(m_Message, pSetup->DeathMessage);

	m_Font = pSetup->DeathFont;
	strcpy(m_StartPointName, "");
	Level()->ActorManager()->GetRotate(Player()->GetActor(), &m_PlayerRotation);
	m_ViewPoint = Player()->GetViewPoint();
	CameraManager()->GetRotation(&m_CameraRotation);
	CameraManager()->GetCameraOffset(&m_CameraTranslationOffset, &m_CameraRotationOffset);
	CameraManager()->Get3rdData(&m_DefaultDistance, &m_CameraX, &m_CameraY);
	m_PlayerOffset.X = 0.0f;
	m_PlayerOffset.Y = 0.0f;
	m_PlayerOffset.Z = 0.0f;

	int i;
	for(i=0; i<MAX_WEAPONS; ++i)
		m_Slot[i] = Weapons()->GetSlot(i);

	m_CurrentWeapon = Weapons()->GetCurrent();
	m_KeepAttributes = true;
	m_SplashHold = false;
}

/* ----------------------------------- END OF FILE ------------------------------------ */
