/****************************************************************************************/
/*																						*/
/*	CCommonData.cpp:		Global Data Pool class										*/
/*																						*/
/*	(c) 2001 Ralph Deane																*/
/*	All Rights Reserved																	*/
/*																						*/
/*	This file contains the class implementation for the Global Data						*/
/*	Pool handling class.  All access to common RGF components MUST be					*/
/*	performed via this class, and all components MUST be registered						*/
/*	via this class as well.  This provides a central point of access and				*/
/*	eases debugging later in the RGF development cycle.									*/
/*																						*/
/*	Edit History:																		*/
/*	=============																		*/
/*	07/15/06 QD:	- Added new lowlevel pawn commands to hash table					*/
/*					- Added a flag to disable the use of DirectInput (joystick)			*/
/*					  On some Win98 systems DInput.dll conflicts with HID.dll			*/
/*	08/13/04 Wendell Buckner:	- I really need the mouse look working in debug mode...	*/
/*																						*/
/****************************************************************************************/

#include "RabidFramework.h"

// changed RF063
extern geVFile *PassWord(char *m_VirtualFile, bool encrypt);
extern void CloseFile();
// end change RF063

// changed RF064
#include "Simkin\\skInterpreter.h" // change simkin
// end change RF064

// start multiplayer
#include "HawkNL\\nl.h"
// end multiplayer

skInterpreter interpreter;

/* ------------------------------------------------------------------------------------ */
//	Constructor
//
//	Initialize all common data pointers, store a pointer to the
//	..Genesis engine object.
/* ------------------------------------------------------------------------------------ */
CCommonData::CCommonData()
{
// changed QD 08/15/06
	VFS					= NULL;
// end change QD 08/15/06
	theGameEngine		= NULL;		// Genesis engine class
// start multiplayer
	theNetPlayerMgr		= NULL;
// end multiplayer
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
// changed QD 01/2004
	theMeshes			= NULL;
// end change
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
// changed RF064
	theActorSpout		= NULL;
	theMorph			= NULL;
	theCutScene			= NULL;
	theActMaterial		= NULL;
	theTerrainMgr		= NULL;
	theArmour			= NULL;
	theLiftBelt			= NULL;
// end change RF064
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
//Start Aug2003DCS
   theLevelController	= NULL;
//End Aug2003DCS
	theAttribute		= NULL;
	theDamage			= NULL;
	theExplosion		= NULL;
	theCExplosion		= NULL;
	thePreEffect		= NULL;
// changed RF064
	theScriptPoints		= NULL;
	thePawn				= NULL;
	theCountDownTimer	= NULL;
	theChangeAttribute	= NULL;
// end change RF064
	theShake			= NULL;
	theFixedCamera		= NULL;
	theChangeLevel		= NULL;
// changed RF063
	theViewSwitch		= NULL;
	theInventory		= NULL;
	theLiquid			= NULL;
	theCDSpot			= NULL;
// end change RF063
// changed RF064
	theOverlay			= NULL;

	//	skInterpreter::setInterpreter(new skInterpreter);//change simkin
	srand((unsigned)time(NULL));
// end change RF064

	FreeImage_Initialise();

	FillHashCommands_LowLevel();//change scripting

	//	Initialize game state data
	m_InGameLoop = true;					// We start in the game loop
	m_TotalPlayTime = 0;					// With no time spent playing
	m_ChangeLevel = false;					// No level change yet
	//	m_DebugLevel = kNoDebugOutput;		// Default to NO debug output
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

	m_CutScene[0]		= '\0';
	m_CutScene1[0]		= '\0';
	m_SplashScreen1[0]	= '\0';
	m_SplashAudio1[0]	= '\0';

// changed Nout 12/15/05
	m_MouseControl		= true;
// end change
	m_headbob			= false;
	m_weaponposition	= false;
// changed RF064
	Paused				= false;
	KeyPaused			= false;
// end change RF064
	UseAngle			= true;
	jumpkey				= false;
	runkey				= false;
	crouchkey			= false;
	zoomkey				= false;
	lightkey			= false;
	ShowTrack			= false;
	KeepAttributes		= true;
// changed RF063
	CSelect				= false;
// changed QD 12/15/05
	NSelect				= false;
	UseDialog			= false;
// end change
// changed QD 07/15/06
	m_bUseDInput		= true;
// end change
	loadkey				= false;
	savekey				= false;
	saving				= false;
	usekey				= false;
	invkey				= false;
	HasFocus			= true;
// end change RF063
// changed RF064
	dropkey				= false;
	reloadkey			= false;
	CDifficult			= false;
	DifficultLevel		= 1;
// end change RF064
// start multiplayer
	consolekey			= false;
	consoleflag			= false;
	network				= false;
	if(nlInit())
		network			= true;
	multiplayer			= false;
// end multiplayer

// begin change gekido 02.17.2004
// begin console vars
	ConsoleBuffer[CONSOLEMAXROWS][CONSOLEMAXCOLS] = 0;
	nCurrentRow = 0;	// what row of the console we're on
// end console vars
// end change gekido 02.17.2004
	m_Language = 0;
	Logging = false;

	//	Debug tracing time
	if(m_DebugLevel == kHighDebugOutput)
		OutputDebugString("CCommonData initialized\n");

// changed RF064
	//	Set up for timekeeping
	m_nTimerID = timeSetEvent(1, 0,	&TimerFunction, (DWORD)this,
				TIME_PERIODIC | TIME_CALLBACK_FUNCTION);

	TimeCounter = LastTimePoll = 0;
	LastTimePassed_D = 0;
	LastTimePassed_F = 0;
// end change RF064
	return;
}

/* ------------------------------------------------------------------------------------ */
//	Destructor
//
//	Just clean up pointers, if needed.
/* ------------------------------------------------------------------------------------ */
CCommonData::~CCommonData()
{
	ShutdownLevel();
	ShutdownCommon();
// changed RF063
	geVFile_CloseAPI();

	if(VFS!=NULL)
	{
		CloseFile();
	}
// end change RF063

// changed RF064

// start change simkin
	//delete skInterpreter::getInterpreter();
	//skInterpreter::setInterpreter(0);
// end change simkin

// end change RF064

	FreeImage_DeInitialise();

// changed QD 08/15/06 - fix memory leaks
	ClearHashCommands_LowLevel();
// end change QD 08/15/06

// start multiplayer
	if(network)
		nlShutdown();
// end multiplayer

// changed RF064
	timeKillEvent(m_nTimerID);
// end change RF064

// changed QD 12/15/05
	FILE *fd;
	if(fd = CCD->OpenRFFile(kRawFile, ".\\RealityFactory.log", "at"))
	{
		fprintf(fd, "Successful Shutdown");
		fclose(fd);
	}
// end change

	return;
}

/* ------------------------------------------------------------------------------------ */
//	InitializeCommon
//
//	This function initializes all the COMMON entities that survive
//	..between level loads.  These need only be initialized ONCE.
//	..Note that the engine setup is driven from RealityFactory.INI, if this
//	..file doesn't exist, the default (640x480, windowed) is used.
/* ------------------------------------------------------------------------------------ */
int CCommonData::InitializeCommon(HINSTANCE hInstance, char *szStartLevel, bool CommandLine)
{
	bool bFullScreen	= false;	// Default to windowed
	bool bSoftware		= false;	// Default to hardware acceleration
	char chTheDriver	= '(';		// Default to Direct3D
	int nHeight			= 800;		// Default to 800 high - begin change gekido - upped the default to 800x600
	int nWidth			= 600;		// Default to 600 wide - end change gekido
	FILE *fd;						// Used for prefs file
	char szTitle[80];				// Game title
	geFloat fGamma		= 1.5f;
// changed QD 12/15/05 - turned into member variable
	// bool UseDialog	= false;
// end change
	bool UseCut			= false;
	bool UseCut1		= false;
	bool UseSecond		= false;
	bool UseFirst		= false;

	CmdLine = CommandLine;

// begin change gekido
	CCD->ConsoleInit(30); // setup our console so we can use it for debugging output
// end change gekido

	//	Ok, let's see if we have an initialization file, and if so,
	//	..read it in and parse it.
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
			// Update #1
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
// changed RF063
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
// end change RF063
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
				if(szArg != NULL)
				{
					if(!stricmp(szArg, "software"))
						bSoftware = true;
					else
						bSoftware = false;
				}
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
					// changed QD  08/15/06 - software renderer is obsolete
					/*
					if(!stricmp(szArg, "windowed"))
						chTheDriver = 'S';				// Not used, only fullscreen counts
					// begin change gekido - removed glide support, we don't provide
					// ..the driver anymore, no point in supporting the renderer as an option
					//	else if(!stricmp(szArg,"glide"))
					//		chTheDriver = 'G';				// Use GLIDE in fullscreen
					else
					*/
					if(!stricmp(szArg, "d3d"))
						chTheDriver = '(';
					else if(!stricmp(szArg, "d3d16"))
						chTheDriver = 'D';				// Use Direct3D in fullscreen
					else if(!stricmp(szArg, "opengl"))
						chTheDriver = 'O';
					else if(!stricmp(szArg, "wire"))
						chTheDriver = 'W';
					//Dee 07-07-00
					else if(!stricmp(szArg, "auto"))
						chTheDriver = 'A';				// Use Auto Detect
					//End Dee
					//Dee 12-07-00
					else if(!stricmp(szArg, "pick"))
						chTheDriver = 'P';				// Pop a driver pick list
					//End Dee
					else
					{
						// changed QD 07/15/06
						ReportError("*WARNING* Bad driver selection in RealityFactory.ini\n", false);
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
// changed QD 12/15/05
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
// end change
// changed QD 07/15/06
			else if(!stricmp(szAtom, "usedirectinput"))
			{
				if(!stricmp(szArg, "false"))
					m_bUseDInput = false;
			}
// end change
			else
				ReportError("*WARNING* Unknown command in RealityFactory.ini\n", false);
		}

		fclose(fd);
	}

// changed QD 07/15/06 - moved below RF.ini parsing (need value of usedirectinput)
	CCD->InitJoysticks(); // pickles Jul 04
// end change

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

// changed RF063
	theGameEngine = new CGenesisEngine(bFullScreen, nWidth, nHeight, szTitle, hInstance,
										chTheDriver, bSoftware, UseDialog, szStartLevel);

	if(theGameEngine == NULL)
	{
		// changed QD 12/15/05
		//theGameEngine->ReportError("Can't create Genesis3d Graphics engine!\n", true);
		ReportError("*ERROR* Failed to create Genesis3D Graphics engine!\n", true);
		// end change
		return -1;
	}

	if(m_DebugLevel != kNoDebugOutput)
		theGameEngine->SetDebugging(true);			// Activate engine debugging

// 08.05.2004 - begin change gekido
	// need to update with each version
	ReportError("\nInitializing Game Shell...",				false);
	ReportError("--------------------------------------",	false);
	ReportError("--- Reality Factory 0.75A          ---",	false);
	ReportError("--- For more Information, visit:   ---",	false);
	ReportError("---    www.realityfactory.info.ms  ---",	false);
	ReportError("--------------------------------------",	false);
	ReportError("\nParsed RealityFactory.ini file",			false);
	//	First, initialize the Genesis3D game engine
	ReportError("\nGenesis3D Initialized", false);
// 08.05.2004 - end change gekido

	FILE *fdInput = NULL;
	char szInputString[16];

	if((fdInput = fopen(m_VirtualFile, "rt")) == NULL)
	{
		VFS = NULL;
		// changed Nout 12/15/05
		ReportError("\n*INFO* No VFS - Reading from Real File System...", false);
		// end change
	}
	else
	{
		fread(szInputString, 4, 1, fdInput);
		fclose(fdInput);

		if(memcmp(szInputString, "CF00", 4) == 0)
		{
			VFS = PassWord(m_VirtualFile, true);
			ReportError("\n*INFO* VFS detected (encrypted)...", false);
		}
		else
		{
			VFS = PassWord(m_VirtualFile, false);
			ReportError("\n*INFO* VFS detected (not encrypted)...", false);
		}
	}

	//	Fire up a camera for us to see through
	ReportError("\nInitializing Camera Manager...", false);
	theCameraManager = new CCameraManager();

	if(theCameraManager == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create Camera Manager", false);
		return -93;
	}
// end change RF063

	geEngine_SetGamma(theGameEngine->Engine(), 1.0f);

	//	We have a 3D engine, now initialize the user input subsystem
	ReportError("Initializing User Input Subsystem...", false);
	theUserInput = new CInput();

	if(theUserInput == NULL)
	{
		ReportError("*ERROR* Failed to create input subsystem", false);
		return -2;
	}

	//	The Audio Manager needs to be prepared...
	ReportError("Initializing Audio Manager Subsystem...", false);
	theAudioManager = new CAudioManager();

	if(theAudioManager == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create Audio Manager", false);
		return -92;
	}

	//	Get the CD Audio player working.  Note that it's not a fatal error to
	//	..not have a CD Player device, as this system may be using the General
	//	..MIDI soundtrack instead.
	ReportError("Initializing CD Audio Manager Subsystem...", false);
	theCDPlayer = new CCDAudio();

	if(theCDPlayer == NULL)
		theGameEngine->ReportError("*ERROR* CD Player failed to instantiate", false);

	//	Fire up the MIDI playback system.  Again, as with the CD Player, failure
	//	..to instantiate is not grounds for aborting game play, although I'd
	//	..personally consider the game unplayable without MY soundtrack (heh).
	ReportError("Initializing Midi Audio Manager Subsystem...", false);
	theMIDIPlayer = new CMIDIAudio();

	if(theMIDIPlayer == NULL)
		theGameEngine->ReportError("*ERROR* MIDI Player failed to instantiate", false);

	ReportError("Initializing RF Menu Manager Subsystem...", false);
// changed QD 12/15/05
	theMenu = new CRFMenu(szStartLevel); // new CRFMenu();
// end change

	if(theMenu == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create Menu subsystem", false);
		return -2;
	}

	ReportError("Initializing Collision Manager Subsystem...", false);
	theCollider = new Collider();

	if(theCollider == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create Collider subsystem", false);
		return -100;
	}

// start multiplayer
	ReportError("Initializing Network Manager Subsystem...", false);
	theNetPlayerMgr = new NetPlayerMgr();

	if(theNetPlayerMgr == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create NetPlayerMgr subsystem", false);
		return -100;
	}
// end multiplayer
	//	Finally, initialize the AVIFile library.  This is done independent of
	//	..any AVI-specific classes so that we can guarantee only ONE instance
	//	..of the library is loaded.
	ReportError("Initializing AVIFile Video Subsystem...", false);
	AVIFileInit();

	//	Common subsystems initialized, back to caller!
	return 0;
}

// start multiplayer
/* ------------------------------------------------------------------------------------ */
//	ShutDownNetWork
/* ------------------------------------------------------------------------------------ */
void CCommonData::ShutDownNetWork()
{
	if(theNetPlayerMgr != NULL)
		delete theNetPlayerMgr;

	theNetPlayerMgr = NULL;
	theNetPlayerMgr = new NetPlayerMgr();

	if(theNetPlayerMgr == NULL)
	{
		theGameEngine->ReportError("*WARNING* Failed to create NetPlayerMgr subsystem", false);
		return;
	}
}

/* ------------------------------------------------------------------------------------ */
//	SetMultiPlayer
/* ------------------------------------------------------------------------------------ */
void CCommonData::SetMultiPlayer(bool multi, bool Server)
{
	multiplayer = multi;
	server = Server;
}

/* ------------------------------------------------------------------------------------ */
//	GetMultiPlayer
/* ------------------------------------------------------------------------------------ */
bool CCommonData::GetMultiPlayer()
{
	return (multiplayer && CCD->GetNetwork());
}
// end multiplayer

// begin change gekido
// new console commands
/* ------------------------------------------------------------------------------------ */
//	ConsoleInit
/* ------------------------------------------------------------------------------------ */
void CCommonData::ConsoleInit(int nMaxRows)
{
	// initialize console
}

/* ------------------------------------------------------------------------------------ */
//	ConsoleRender
/* ------------------------------------------------------------------------------------ */
bool CCommonData::ConsoleRender()
{
	// renders the current buffer to screen
	return false;
}

/* ------------------------------------------------------------------------------------ */
//	ConsolePrint
/* ------------------------------------------------------------------------------------ */
bool CCommonData::ConsolePrint(char *szMsg, bool *bBox)
{
	// print to console
	return false;
}

/* ------------------------------------------------------------------------------------ */
//	ShowConsole
/* ------------------------------------------------------------------------------------ */
bool CCommonData::ShowConsole (bool bConsoleFlag, int nTransition)
{
	// show/hide console param1 - show/hide, param 2 - whether to use a slide in/out
	// ..transition when showing the console
	return false;
}

/* ------------------------------------------------------------------------------------ */
//	ConsoleExec
//
//	still not sure on the parameters for this, may change
/* ------------------------------------------------------------------------------------ */
bool CCommonData::ConsoleExec(char *szMsg, bool *bBox)
{
	// execute console command
	return false;
}

/* ------------------------------------------------------------------------------------ */
//	ConsoleClear
//
//	sample console command, 'clear'
/* ------------------------------------------------------------------------------------ */
bool CCommonData::ConsoleClear()
{
	// clears console
	return false;
}
// end change gekido

// change simkin start
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
void CCommonData::AddScriptedObject(char *objectName, skRValue Object)
{
	interpreter.addGlobalVariable(objectName,Object);
}

/* ------------------------------------------------------------------------------------ */
//	RemoveScriptedObject
/* ------------------------------------------------------------------------------------ */
void CCommonData::RemoveScriptedObject(char *objectName)
{
	interpreter.removeGlobalVariable(objectName);
}
// change simkin end


// 03.04.2004 - begin change gekido
//	- added extensive logging to the startup and shutdown functions to help debugging
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
// start multiplayer
	ReportError("Shutting Down Network Manager Subsystem...", false);
	if(theNetPlayerMgr != NULL)
		delete theNetPlayerMgr;
	theNetPlayerMgr = NULL;
// end multiplayer

	ReportError("Shutting Down Collision Manager Subsystem...", false);
	if(theCollider != NULL)
		delete theCollider;
	theCollider = NULL;

	ReportError("Shutting Down RF Menu Manager Subsystem...", false);
	if(theMenu != NULL)
		delete theMenu;
	theMenu = NULL;

	ReportError("Shutting Down CD Audio Manager Subsystem...", false);
	if(theCDPlayer != NULL)
		delete theCDPlayer;
	theCDPlayer = NULL;

	ReportError("Shutting Down Midi Audio Manager Subsystem...", false);
	if(theMIDIPlayer != NULL)
		delete theMIDIPlayer;
	theMIDIPlayer = NULL;

	ReportError("Shutting Down Audio Manager Subsystem...", false);
	if(theAudioManager != NULL)
		delete theAudioManager;
	theAudioManager = NULL;

	ReportError("Shutting Down User Input Manager Subsystem...", false);
	if(theUserInput != NULL)
		delete theUserInput;
	theUserInput = NULL;

	ReportError("Shutting Down Camera Manager Subsystem...", false);
	if(theCameraManager != NULL)
		delete theCameraManager;
	theCameraManager = NULL;

	ReportError("Shutting Down Graphics Subsystem...", false);
	if(theGameEngine != NULL)
		delete theGameEngine;
	theGameEngine = NULL;

	ReportError("Restoring Mouse Cursor...", false);
	ShowCursor(TRUE);

// changed Nout 12/15/05
	m_MouseControl = true;
// end change

	CloseJoysticks(); // pwx

	return;
}

/* ------------------------------------------------------------------------------------ */
//	InitializeLevel
//
//	This function loads a level, then initializes all the entity
//	..handler classes that deal with the various level-specific
//	..entities.
/* ------------------------------------------------------------------------------------ */
int CCommonData::InitializeLevel(char *szLevelName)
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
		sprintf(szBug, "*ERROR* File %s - Line %d: Level [%s] failed to load!",
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
		theGameEngine->ReportError("*ERROR* Failed to initialize entity registry!", false);
		return -30;
	}

// changed RF064
	ReportError("Initializing Terrain Manager...", false);
	theTerrainMgr = new qxTerrainMgr();
	if(theTerrainMgr == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create qxTerrainMgr handler", false);
		return -26;
	}
// end change RF064

	ReportError("Initializing Effects Manager...", false);
	theEffect = new EffManager();
	if(theEffect == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create Effect handler", false);
		return -26;
	}

	//	The Actor Manager is up next
	ReportError("Initializing Actor Manager...", false);
	theActorManager = new CActorManager();
	if(theActorManager == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create actor manager", false);
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
		theGameEngine->ReportError("*ERROR* Failed to create model manager", false);
		return -92;
	}

	//	Create the player avatar and load it.
	ReportError("Creating Player Avatar...", false);

	thePlayer = new CPlayer();
	if(thePlayer == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create player avatar!", false);
		return -4;
	}

	ReportError("Loading Player Avatar...", false);
	// changed QD 12/15/05
	if(thePlayer->LoadAvatar(m_PlayerAvatar, m_PlayerName) != RGF_SUCCESS)
	{
		theGameEngine->ReportError("*ERROR* Failed to load player avatar actor!", false);
		return -5;
	}

// changed RF064
	//	Set up the heads-up display (HUD) for the game
	ReportError("Initializing HUD...", false);
	theHUD = new CHeadsUpDisplay();
	if(theHUD == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create HUD class", false);
		return -3;
	}
// end change RF064

	ReportError("Initializing Damage Subsystem...", false);
	theDamage = new CDamage();
	if(theDamage == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create Damage subsystem", false);
		return -100;
	}

	//	Now various other level-specific items
	ReportError("Initializing FixedCamera Manager Subsystem...", false);
	theFixedCamera = new CFixedCamera();
	if(theFixedCamera == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create FixedCamera handling class",
			false);
		return -11;
	}

	ReportError("Loading Player Configuration...", false);
	thePlayer->LoadConfiguration();

	//	Set up automatic door handling
	ReportError("Initializing Automatic Door Manager Subsystem...", false);
	theAutoDoors = new CAutoDoors();
	if(theAutoDoors == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create automatic door handling class",
			false);
		return -6;
	}

	//	Start up the moving platform class
	ReportError("Initializing Moving Platform Manager Subsystem...", false);
	thePlatforms = new CMovingPlatforms();
	if(thePlatforms == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create moving platform handling class",
			false);
		return -8;
	}

	//	Activate teleports
	ReportError("Initializing Teleport Manager Subsystem...", false);
	theTeleports = new CTeleporter();
	if(theTeleports == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create teleporter handling class",
			false);
		return -10;
	}

	//	Load up the morphing field effects
	ReportError("Initializing MorphingField Effects Manager Subsystem...", false);
	theFields = new CMorphingFields();
	if(theFields == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create morphing field handling class", false);
		return -11;
	}

	//	Load and start up all 3D audio sources
	ReportError("Initializing 3d AudioSource Manager Subsystem...", false);
	the3DAudio = new C3DAudioSource();
	if(the3DAudio == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create 3D audio source handling class", false);
		return -12;
	}

	//	Fire up particle system handling
	ReportError("Initializing Particle Effects Manager Subsystem...", false);
	theParticles = new CParticleSystem();
	if(theParticles == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create particle system handler", false);
		return -13;
	}

	//	Load up all the static entities (props) for the level.
	ReportError("Initializing Static Entity Props Subsystem...", false);
	theProps = new CStaticEntity();
	if(theProps == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create static entity handler", false);
		return -14;
	}

// changed QD 01/2004
	//	Load up all the static meshes for the level.
// begin change gekido
	ReportError("Initializing Static Mesh Subsystem...", false);
// end change gekido
	theMeshes = new CStaticMesh();
	if(theMeshes == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create static mesh handler", false);
		return -14;
	}
// end change QD

	//	Set up soundtrack toggles for the level
	ReportError("Initializing Soundtrack Toggle Subsystem...", false);
	theSTToggles = new CSoundtrackToggle();
	if(theSTToggles == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create soundtrack toggle handler", false);
		return -15;
	}

	//	Fire up the streaming audio handler
	ReportError("Initializing Streaming Audio Manager Subsystem...", false);
	theStreams = new CAudioStream();
	if(theStreams == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create streaming audio handler", false);
		return -16;
	}

	//	and the video texture handler component
	ReportError("Initializing Video Texture Manager Subsystem...", false);
	theVidText = new CVideoTexture();
	if(theVidText == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create video texture handler", false);
		return -17;
	}

	// corona effect
	ReportError("Initializing Corona Manager Subsystem...", false);
	theCorona = new CCorona();
	if(theCorona == NULL)
    {
		theGameEngine->ReportError("*ERROR* Failed to create Corona handler", false);
		return -20;
    }

	// dynamic lights
	ReportError("Initializing Dynamic Light Manager Subsystem...", false);
	theDynalite = new CDynalite();
	if(theDynalite == NULL)
    {
		theGameEngine->ReportError("*ERROR* Failed to create Dynalite handler", false);
		return -21;
    }

	ReportError("Initializing ElectricBolt Manager Subsystem...", false);
	theElectric = new CElectric();
	if(theElectric == NULL)
    {
		theGameEngine->ReportError("*ERROR* Failed to create Electric handler", false);
		return -22;
    }

	ReportError("Initializing Procedural Texture Manager Subsystem...", false);
	theProcTexture = new CProcedural();
	if(theProcTexture == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create Procedural Texture handler", false);
		return -23;
	}

	//	Create the in-memory path database for this level.
	ReportError("Initializing Path Database...", false);
	thePathDatabase = new CPathDatabase();
	if(thePathDatabase == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create Path database", false);
		return -24;
	}

	//	Set up path followers to bind entities to motion paths
	ReportError("Initializing Path Followers...", false);
	theFollower = new CPathFollower();
	if(theFollower == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create Path follower", false);
		return -25;
	}

	//	Initialize Ralph Deane's Effects Manager and the various effects
	//	..that it handles.  Thanks for the great conribution, Ralph!
	ReportError("Initializing Rain Effects Manager...", false);
	theRain = new CRain();
	if(theRain == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create Rain handler", false);
		return -27;
	}

	ReportError("Initializing Spout Effects Manager...", false);
	theSpout = new CSpout();
	if(theSpout == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create Spout handler", false);
		return -28;
	}

// changed RF064
	ReportError("Initializing ActorSpout Effects Manager...", false);
	theActorSpout = new CActorSpout();
	if(theActorSpout == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create ActorSpout handler", false);
		return -28;
	}
// end change RF064

	ReportError("Initializing Floating Particle Effects Manager...", false);
	theFloat = new CFloat();
	if(theFloat == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create Floating Particle handler", false);
		return -29;
	}

	ReportError("Initializing eChaos Effects Manager...", false);
	theChaos = new Chaos();
	if(theChaos == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create Chaos handler", false);
		return -30;
	}

	ReportError("Initializing Flame Effects Manager...", false);
	theFlame = new CFlame();
	if(theFlame == NULL)
    {
		theGameEngine->ReportError("*ERROR* Failed to create Flame handler", false);
		return -32;
    }

// changed RF064
	ReportError("Initializing ScriptPoint Manager Subsystem...", false);
	theScriptPoints = new CScriptPoint();
	if(theScriptPoints == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create ScriptPoint handler", false);
		return -40;
	}

	ReportError("Initializing Pawn Manager Subsystem...", false);
	thePawn = new CPawn();
	if(thePawn == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create Pawn handler", false);
		return -40;
	}

	ReportError("Initializing ChangeAttribute Manager Subsystem...", false);
	theChangeAttribute = new CChangeAttribute();
	if(theChangeAttribute == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create ChangeAttribute handler", false);
		return -40;
	}

	ReportError("Initializing Countdown Manager Subsystem...", false);
	theCountDownTimer = new CCountDown();
	if(theCountDownTimer == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create CountDownTimer handler", false);
		return -40;
	}
// end change RF064

	//	Set up triggers
	ReportError("Initializing Trigger Manager Subsystem...", false);
	theTriggers = new CTriggers();
	if(theTriggers == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create trigger handling class", false);
		return -41;
	}

	//	Set up logic gates
	ReportError("Initializing LogicHandler Subsystem...", false);
	theLogic = new CLogic();
	if(theLogic == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create Logic handling class",
			false);
		return -42;
	}

	// Message
	ReportError("Initializing Message Manager Subsystem...", false);
	theMessage = new CMessage();
	if(theMessage == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create Message handling class", false);
		return -7;
	}

	ReportError("Initializing Effect Manager Subsystem...", false);
	thePreEffect = new CPreEffect();
	if(thePreEffect == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create Effect subsystem", false);
		return -100;
	}

	// Weapon
	ReportError("Initializing Weapon Manager Subsystem...", false);
	theWeapon = new CWeapon();
	if(theWeapon == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create Weapon handling class", false);
		return -7;
	}

	ReportError("Initializing FirePoint Manager Subsystem...", false);
	theFirePoint = new CFirePoint();
	if(theFirePoint == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create FirePoint handling class", false);
		return -7;
	}

	ReportError("Initializing Flipbook Manager Subsystem...", false);
	theFlipBook = new CFlipBook();
	if(theFlipBook == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create FlipBook handling class", false);
		return -7;
	}

	// pwx
	ReportError("Initializing AreaCheck Manager Subsystem...", false);
	theAreaCheck = new CAreaChecker();
	if(theAreaCheck == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create AreaCheck handling class", false);
		return -7;
	}
	// pwx

// Start Pickles Jul 04
	ReportError("Initializing Foliage Manager Subsystem...", false);
	theFoliage = new CFoliage();
	if(theFoliage == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create Foliage handling class", false);
		return -7;
	}

	ReportError("Initializing Tree Manager Subsystem...", false);
	theFlipTree = new CFlipTree();
	if(theFlipTree == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create Tree handling class", false);
		return -7;
	}
// End Pickles Jul 04

// Start PWX
	ReportError("Initializing PWXImage Manager Subsystem...", false);
	thePWXImage = new PWXImageManager();
	if(thePWXImage == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create PWX image handling class", false);
		return -7;
	}
// End PWX

// Start PWX
	ReportError("Initializing Shadow Manager Subsystem...", false);
	thePolyShadow = new CPolyShadow();
	if(thePolyShadow == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create Poly Shadow handling class", false);
		return -7;
	}
// End PWX

	// Decal
	ReportError("Initializing Decal Manager Subsystem...", false);
	theDecal = new CDecal();
	if(theDecal == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create Decal handling class", false);
		return -7;
	}

	ReportError("Initializing WallDecal Manager Subsystem...", false);
	theWallDecal = new CWallDecal();
	if(theWallDecal == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create WallDecal handling class", false);
		return -7;
	}

// Start Aug2003DCS
	ReportError("Initializing LevelController Manager Subsystem...", false);
	theLevelController = new CLevelController();
	if(theLevelController == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create LevelController handling class", false);
		return -7;
	}
// End Aug2003DCS

	// Attribute
	ReportError("Initializing Attribute Manager Subsystem...", false);
	theAttribute = new CAttribute();
	if(theAttribute == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create Attribute handling class", false);
		return -7;
	}

	ReportError("Initializing Explosion Manager Subsystem...", false);
	theExplosion = new CExplosionInit();
	if(theExplosion == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create Explosion Manager subsystem", false);
		return -100;
	}

	ReportError("Initializing Explosion Subsystem...", false);
	theCExplosion = new CExplosion();
	if(theCExplosion == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create Explosion subsystem", false);
		return -100;
	}

	ReportError("Initializing ChangeLevel Manager Subsystem...", false);
	theChangeLevel = new CChangeLevel();
	if(theChangeLevel == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create ChangeLevel handling class", false);
		return -7;
	}

	ReportError("Initializing ScreenShake Subsystem...", false);
	theShake = new CShake();
	if(theShake == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create ScreenShake handler", false);
		return -40;
	}

// changed RF063
	ReportError("Initializing ViewSwitch Subsystem...", false);
	theViewSwitch = new CViewSwitch();
	if(theViewSwitch == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create ViewSwitch handler", false);
		return -40;
	}

	ReportError("Initializing Inventory Subsystem...", false);
	theInventory = new CInventory();
	if(theInventory == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create Inventory handler", false);
		return -40;
	}

	ReportError("Initializing Liquid Subsystem...", false);
	theLiquid = new CLiquid();
	if(theLiquid == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create Liquid handler", false);
		return -40;
	}
// end change RF063

// changed RF064
	ReportError("Initializing Overlay Subsystem...", false);
	theOverlay = new COverlay();
	if(theOverlay == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create Overlay handler", false);
		return -40;
	}

	ReportError("Initializing TextureMorph Subsystem...", false);
	theMorph = new CMorph();
	if(theMorph == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create TextureMorph", false);
		return -7; //or something else
	}

	ReportError("Initializing CutScene Subsystem...", false);
	theCutScene = new CCutScene();
	if(theCutScene == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create CutScene", false);
		return -7; //or something else
	}

	ReportError("Initializing ActorMaterial Subsystem...", false);
	theActMaterial = new CActMaterial();
	if(theActMaterial == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create CActMaterial", false);
		return -7; //or something else
	}

	ReportError("Initializing Armour Subsystem...", false);
	theArmour = new CArmour();
	if(theArmour == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create CArmour", false);
		return -7; //or something else
	}

	ReportError("Initializing LiftBelt Manager...", false);
	theLiftBelt = new CLiftBelt();
	if(theLiftBelt == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create CLiftBelt", false);
		return -7; //or something else
	}

	ReportError("Initializing CDSpotlight Manager...", false);
	theCDSpot = new CDSpotLight();
	if(theCDSpot == NULL)
	{
		theGameEngine->ReportError("*ERROR* Failed to create CDSpotLight", false);
		return -7; //or something else
	}

	ReportError("Initializing Fonts...", false);
	theMenu->LoadWBitmap();

	ReportError("Preparing to Launch Game...", false);
// end change RF064

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
// start multiplayer
	if(theNetPlayerMgr != NULL)
	{
		theNetPlayerMgr->DeletePlayers();
	}
// end multiplayer

// changed RF064
	if(theMenu)
		theMenu->UnLoadWBitmap();

	if(theCDSpot != NULL)
		delete theCDSpot;
	theCDSpot = NULL;

	if(theLiftBelt != NULL)
		delete theLiftBelt;
	theLiftBelt = NULL;

	if(theArmour != NULL)
		delete theArmour;
	theArmour = NULL;

	if(theActMaterial != NULL)
		delete theActMaterial;
	theActMaterial = NULL;

	if(theCutScene != NULL)
		delete theCutScene;
	theCutScene = NULL;

	if(theMorph != NULL)
		delete theMorph;
	theMorph = NULL;

	if(theOverlay != NULL)
		delete theOverlay;
	theOverlay = NULL;
// end change RF064

// changed RF063
	if(theLiquid != NULL)
		delete theLiquid;
	theLiquid = NULL;

	if(theInventory != NULL)
		delete theInventory;
	theInventory = NULL;

	if(theViewSwitch != NULL)
		delete theViewSwitch;
	theViewSwitch = NULL;
// end change RF063

	if(theShake != NULL)
		delete theShake;
	theShake = NULL;

	if(theChangeLevel != NULL)
		delete theChangeLevel;
	theChangeLevel = NULL;

	if(theCExplosion != NULL)
		delete theCExplosion;
	theCExplosion = NULL;

	if(theExplosion != NULL)
		delete theExplosion;
	theExplosion = NULL;

	// Attribute
	if(theAttribute != NULL)
		delete theAttribute;
	theAttribute = NULL;

	// Decal
	if(theDecal != NULL)
		delete theDecal;
	theDecal = NULL;

	if(theWallDecal != NULL)
		delete theWallDecal;
	theWallDecal = NULL;

// Start Aug2003DCS
	if(theLevelController != NULL)
		delete theLevelController;
	theLevelController = NULL;
// End Aug2003DCS

	if(theFlipBook != NULL)
		delete theFlipBook;
	theFlipBook = NULL;

// Start pickles Jul 04
	if(theFoliage != NULL)
		delete theFoliage;
	theFoliage = NULL;

	if(theFlipTree != NULL)
		delete theFlipTree;
	theFlipTree = NULL;
// end Pickles Jul 04

// Start PWX
	if(thePWXImage != NULL)
		delete thePWXImage;
	thePWXImage = NULL;

	if(thePolyShadow != NULL)
		delete thePolyShadow;
	thePolyShadow = NULL;

	if(theAreaCheck != NULL)
		delete theAreaCheck;
	theAreaCheck = NULL;
// end PWX

	// Weapon
	if(theFirePoint != NULL)
		delete theFirePoint;
	theFirePoint = NULL;

	if(theWeapon != NULL)
		delete theWeapon;
	theWeapon = NULL;

	if(thePreEffect != NULL)
		delete thePreEffect;
	thePreEffect = NULL;

	// Message
	if(theMessage != NULL)
		delete theMessage;
	theMessage = NULL;

// changed RF064
	if(theScriptPoints != NULL)
		delete theScriptPoints;
	theScriptPoints = NULL;

	if(thePawn != NULL)
		delete thePawn;
	thePawn = NULL;

	if(theChangeAttribute != NULL)
		delete theChangeAttribute;
	theChangeAttribute = NULL;

	if(theCountDownTimer != NULL)
		delete theCountDownTimer;
	theCountDownTimer = NULL;
// end change RF064

	if(theLogic != NULL)
		delete theLogic;
	theLogic = NULL;

	if(theTriggers != NULL)
		delete theTriggers;
	theTriggers = NULL;

	if(theFlame != NULL)
		delete theFlame;
	theFlame = NULL;

	if(theChaos != NULL)
		delete theChaos;
	theChaos = NULL;

	if(theRain != NULL)
		delete theRain;
	theRain = NULL;

// changed RF064
	if(theActorSpout != NULL)
		delete theActorSpout;
	theActorSpout = NULL;
// end change RF064

	if(theSpout != NULL)
		delete theSpout;
	theSpout = NULL;

	if(theFloat != NULL)
		delete theFloat;
	theFloat = NULL;

	if(theEffect != NULL)
		delete theEffect;
	theEffect = NULL;

	if(theFollower != NULL)
		delete theFollower;
	theFollower = NULL;

	if(thePathDatabase != NULL)
		delete thePathDatabase;
	thePathDatabase = NULL;

	if(theProcTexture != NULL)
		delete theProcTexture;
	theProcTexture = NULL;

	if(theCorona != NULL)
		delete theCorona;
	theCorona = NULL;

	if(theDynalite != NULL)
		delete theDynalite;
	theDynalite = NULL;

	if(theElectric != NULL)
		delete theElectric;
	theElectric = NULL;

	if(theVidText != NULL)
		delete theVidText;
	theVidText = NULL;

	if(theStreams != NULL)
		delete theStreams;
	theStreams = NULL;

	if(theSTToggles != NULL)
		delete theSTToggles;
	theSTToggles = NULL;

	if(theProps != NULL)
		delete theProps;
	theProps = NULL;

// changed QD 01/2004
	if(theMeshes != NULL)
		delete theMeshes;
	theMeshes = NULL;
// end change

	if(theParticles != NULL)
		delete theParticles;
	theParticles = NULL;

	if(the3DAudio != NULL)
		delete the3DAudio;
	the3DAudio = NULL;

	if(thePlatforms != NULL)
		delete thePlatforms;
	thePlatforms = NULL;

	if(theFixedCamera != NULL)
		delete theFixedCamera;
	theFixedCamera = NULL;

	if(theAutoDoors != NULL)
		delete theAutoDoors;
	theAutoDoors = NULL;

	if(thePlayer != NULL)
		delete thePlayer;
	thePlayer = NULL;

	if(theTeleports != NULL)
		delete theTeleports;
	theTeleports = NULL;

	if(theFields != NULL)
		delete theFields;
	theFields = NULL;

// changed RF064
	if(theHUD != NULL)
		delete theHUD;
	theHUD = NULL;
// end change Rf064

	if(theModelManager != NULL)
		delete theModelManager;
	theModelManager = NULL;

	if(theActorManager != NULL)
		delete theActorManager;
	theActorManager = NULL;

	if(theRegistry != NULL)
		delete theRegistry;
	theRegistry = NULL;

// changed RF064
	if(theTerrainMgr != NULL)
		delete theTerrainMgr;
	theTerrainMgr = NULL;
// end change RF064

	if(theEffect != NULL)
		delete theEffect;
	theEffect = NULL;

	if(theDamage != NULL)
		delete theDamage;
	theDamage = NULL;

	return;
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
// changed RF063
	bool screen, jump, run, crouch, zoom, light, save, load, use, inv;
	static int nLoopTimer = timeGetTime();	// Loop timer
// start multiplayer
	bool fconsole = false;
// end multiplayer
// changed RF064
	bool fdrop = false;
	bool freload = false;
// end change RF064
	frun = fhud = flook = fcamera = screen = jump = false;				// Clear modifiers
	run = crouch = zoom = light = save = load = use = inv = false;
// end change RF063

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
// changed RF063
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
// end change RF063
// start multiplayer
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
// end multiplayer
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
// changed RF064
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
// end change RF064
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
// changed RF063
				save = true;
				if(!savekey)
				{

					FILE *outFD = CCD->OpenRFFile(kSavegameFile, "savegame.rgf", "wb");
					if(outFD == NULL)
					{
						CCD->ReportError("Failed to create savegame file!", false);
						break;
					}
// start multiplayer
					theGameEngine->SaveTo(outFD);
					theMenu->SaveTo(outFD, false);
					thePlayer->SaveTo(outFD);
					theAutoDoors->SaveTo(outFD, false);
					thePlatforms->SaveTo(outFD, false);
					theProps->SaveTo(outFD, false);
					// changed QD 01/2004
					theMeshes->SaveTo(outFD, false);
					// end change
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
// end multiplayer
					SaveTo(outFD);
					fclose(outFD);
					savekey = true;
					saving = true;
				}
// end change RF063
			}
			break;
		case RGF_K_QUICKLOAD:
			{
// Changed RF063
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
// start multiplayer
					theAutoDoors->RestoreFrom(inFD, false);
					thePlatforms->RestoreFrom(inFD, false);
					theProps->RestoreFrom(inFD, false);
					// changed QD 01/2004
					theMeshes->RestoreFrom(inFD, false);
					// end change
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
// end multiplayer
					RestoreFrom(inFD);
					fclose(inFD);
					loadkey = true;
				}
// end change RF063
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

// changed RF063
	if(!load)
		loadkey = false;

	if(!save)
		savekey = false;

	if(!use)
		usekey = false;

	if(!inv)
		invkey = false;
// end change RF063
// start multiplayer
	if(!fconsole)
		consolekey = false;
// end multiplayer
// changed RF064
	if(!fdrop)
		dropkey = false;

	if(!freload)
		reloadkey = false;
// end change RF064
	if(!zoom)
	{
		theCameraManager->SetZoom(false);
		zoomkey = false;
	}

	if(!thePlayer->FirstPersonView())
		thePlayer->LookMode(flook);				// Set lookmode

	bPlayerMoved = thePlayer->DoMovements();	//Manage player "wanted" movements

	thePlayer->CheckKeyLook(keyrotate); // update #2

// changed 12/15/05
#ifdef _DEBUG
	   thePlayer->CheckMouseLook();
#else
	   if(m_MouseControl)
		   thePlayer->CheckMouseLook();
#endif
// end change

	thePlayer->ProcessMove(bPlayerMoved);

	return bKeepPlaying;
}

// changed RF063
/* ------------------------------------------------------------------------------------ */
//	SaveTo
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
	// changed QD 12/15/05 MAX_WEAPONS = 40
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
// changed QD 12/15/05
	fwrite(&NSelect,			sizeof(bool),		1,		SaveFD);
// end change

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	RestoreFrom
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
	// changed QD 12/15/05 MAX_WEAPONS = 40
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
// changed QD 12/15/05
	fread(&NSelect,				sizeof(bool),		1,		RestoreFD);
// end change

	theHUD->LoadConfiguration();
	theHUD->Activate();

	return RGF_SUCCESS;
}
// end change RF063

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

	//	Ok, deal out time to everyone else.
// start multiplayer
	theNetPlayerMgr->Tick(dwTicksGoneBy);
// end multiplayer
	theAutoDoors->Tick(dwTicksGoneBy);		// Advance doors, if needed
	thePlatforms->Tick(dwTicksGoneBy);		// Advance platforms, if needed
	theTeleports->Tick(dwTicksGoneBy);		// Animate teleporter fields
	theFields->Tick(dwTicksGoneBy);			// Animate morphing fields
	theParticles->Tick(dwTicksGoneBy);		// Animate particle systems
	theProps->Tick(dwTicksGoneBy);			// Animate static models
	// changed QD 01/2004
	theMeshes->Tick(dwTicksGoneBy);
	// end change
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
// changed RF064
	theActorSpout->Tick(dwTicksGoneBy);
	theMorph->Tick(dwTicksGoneBy);
	theActMaterial->Tick(dwTicksGoneBy);
// end change RF064
	theFloat->Tick(dwTicksGoneBy);			// Time to Ralph Deane's Float Effect
	theChaos->Tick(dwTicksGoneBy);			// Time to Ralph Deane's Chaos Procedural
	theFlame->Tick(dwTicksGoneBy);			// Time to Ralph Deane's Flame Effect
	the3DAudio->Tick(dwTicksGoneBy);		// Time to 3D Audio Source
	theTriggers->Tick(dwTicksGoneBy);		// Time to Ralph Deane's Triggers
	theLogic->Tick(dwTicksGoneBy);			// Time to Ralph Deane's Logic
	theMessage->Tick(dwTicksGoneBy);
	theDecal->Tick(dwTicksGoneBy);
// changed RF064
	theWallDecal->Tick(dwTicksGoneBy);
	theTerrainMgr->Frame();
// end change RF064
	theWeapon->Tick(dwTicksGoneBy);
	theFirePoint->Tick(dwTicksGoneBy);
	theFlipBook->Tick(dwTicksGoneBy);
	theFoliage->Tick(dwTicksGoneBy);		// Pickles Jul 04
	theFlipTree->Tick(dwTicksGoneBy);		// Pickles Jul 04
	theAreaCheck->Tick(dwTicksGoneBy);		// Pickles pwx
	theAttribute->Tick(dwTicksGoneBy);
	theDamage->Tick(dwTicksGoneBy);
	theCExplosion->Tick(dwTicksGoneBy);
// changed RF064
	thePawn->Tick(dwTicksGoneBy);
	theCountDownTimer->Tick(dwTicksGoneBy);
	theChangeAttribute->Tick(dwTicksGoneBy);
	theOverlay->Tick(dwTicksGoneBy);
	theLiftBelt->Tick(dwTicksGoneBy);
	theCDSpot->Tick(dwTicksGoneBy);
// end change RF064
	theChangeLevel->Tick(dwTicksGoneBy);
	theShake->Tick(dwTicksGoneBy);
	theFixedCamera->Tick();
// changed RF063
	theViewSwitch->Tick();
// end change RF063
// Start Aug2003DCS
   	theLevelController->Tick(dwTicksGoneBy);	// Execute the LevelControllers
// End Aug2003DCS
	theCameraManager->Tick(dwTicksGoneBy);

	//	Finally, now that everything has moved, update all audio
	theAudioManager->Tick(dwTicksGoneBy);	// Audio updates

	thePlayer->Tick(dwTicksGoneBy);

	theActorManager->Tick(dwTicksGoneBy);

	theHUD->Tick(dwTicksGoneBy);
// changed RF064
	theCutScene->Tick(dwTicksGoneBy);
// end change RF064

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	ProcessLevelChange
//
//	Process a level change entity trigger.  This will shut down our
//	..current level and load the new one!
/* ------------------------------------------------------------------------------------ */
bool CCommonData::ProcessLevelChange()
{
	char szLevelFile[256];

	m_ChangeLevel = true;

	//	Kill the current soundtrack.
	if(theMIDIPlayer)
		theMIDIPlayer->Stop();		// Shut down MIDI, if any

	if(theCDPlayer)
		theCDPlayer->Stop();		// Shut down CD Audio, if any

// changed RF063
	CCD->AudioStreams()->StopAll();

	// Uh-oh, time to change levels!  This is somewhat messy but
	// ..hopefully easy to understand.  We shut down the current level
	// ..and load the new one here.

	if(!EffectC_IsStringNull(m_NewLevel))
		strcpy(szLevelFile, m_NewLevel);	// Save off before...

	//if(m_DebugLevel == kHighDebugOutput)
	if(Logging)
	{
		char szDebug[512];
		sprintf(szDebug, "*INFO* Attempting changelevel to level '%s'", szLevelFile);
		CCD->ReportError(szDebug, false);
	}

	// Save off player attributes during level change..
	thePlayer->SaveAttributes("temp.bin");
	thePlayer->SaveAttributesAscii("attributes.txt");

	//	Check for a cut scene to play..
	if(!EffectC_IsStringNull(m_CutScene))
	{
// changed RF063
		Play(m_CutScene, 0, 0, true);
// end change RF063
	}

	// Numero uno, display the level-changing splash screen.
	if(!EffectC_IsStringNull(m_SplashScreen))
	{
		CString File = m_SplashScreen;
		File.MakeLower();

		if(File.Find(".bmp") == -1)
		{
			CIniFile AttrFile;
			AttrFile.SetPath("splash.ini");

			if(AttrFile.ReadFile())
			{
				CString KeyName = AttrFile.FindFirstKey();
				CString Type, Value;

				while(KeyName != "")
				{
					if(!strcmp(KeyName, m_SplashScreen))
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
						strcpy(name, Type);
						theGameEngine->DisplaySplash(name, m_SplashAudio);
						break;
					}

					KeyName = AttrFile.FindNextKey();
				}
			}
		}
		else
			theGameEngine->DisplaySplash(m_SplashScreen, m_SplashAudio);

// changed RF064
		if(EffectC_IsStringNull(m_NewLevel))
			Spin(5000);
// end change RF064
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

	ShutdownLevel();
// changed RF064
	CCD->SetKeyPaused(false);
// end change RF064
	// Ok, load in the new level

	if(!EffectC_IsStringNull(m_NewLevel))
	{
		// Shut the current level down

		//ShutdownLevel();
		if(InitializeLevel(szLevelFile) != 0)
		{
			// I regard this as a catastrophic error, so we'll log it and
			// ..commit suicide.
			char szBug[256];
			sprintf(szBug, "*ERROR* File %s - Line %d: Failed to load new level %s",
					__FILE__, __LINE__, szLevelFile);
			theGameEngine->ReportError(szBug, false);
			ShutdownLevel();
			ShutdownCommon();
			exit(-334);
		}

		CCD->HUD()->LoadConfiguration();

		//	Restore the player attributes for the new level
		if(KeepAttributes)
			thePlayer->LoadAttributes(".\\temp.bin");
		else
			thePlayer->LoadAttributes("pdoa.bin");

		KeepAttributes = true;

		// Move the player avatar to the start of the new level
		CCD->TerrainMgr()->Init();

		thePlayer->MoveToStart();

		geVec3d ActPos = thePlayer->Position();
		geVec3d_Subtract(&ActPos, &Offset, &ActPos);
		CCD->ActorManager()->Position(thePlayer->GetActor(), ActPos);

// changed RF064
		if(!thePlayer->GetMonitorMode())
		{
			//thePlayer->SwitchCamera(ViewPoint);
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
// end change RF064

		int i;

		// changed QD 12/15/05 MAX_WEAPONS = 40
		for(i=0; i<40; i++)
			theWeapon->SetSlot(i, Slot[i]);

// changed RF064
		if(!thePlayer->GetMonitorMode())
			theWeapon->ReSetWeapon(CurrentWeapon);
// end change RF064

		geEntity_EntitySet *pSet;
		geEntity *pEntity;

		pSet = geWorld_GetEntitySet(CCD->World(), "PlayerSetup");
		pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL);
		PlayerSetup *pSetup = (PlayerSetup*)geEntity_GetUserData(pEntity);
// changed QD 07/15/06
		//strcpy(m_SplashAudio, "");
		//strcpy(m_CutScene, "");
		m_SplashAudio[0] = '\0';
		m_CutScene[0] = '\0';

		if(EffectC_IsStringNull(pSetup->DeathMessage))
		{
			//strcpy(m_Message, "");
			m_Message[0] = '\0';
		}
		else
		{
			//strcpy(m_Message, pSetup->DeathMessage);
			strncpy(m_Message, pSetup->DeathMessage, 255);
			m_Message[255] = '\0';
		}

		m_Font = pSetup->DeathFontSize;

		if(!EffectC_IsStringNull(pSetup->Deathlevel))
		{
			//strcpy(m_NewLevel, pSetup->Deathlevel);
			strncpy(m_NewLevel, pSetup->Deathlevel, 255);
			m_NewLevel[255] = '\0';
		}
// end change QD 07/15/06

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

		//strcpy(m_SplashScreen, "");
		m_SplashScreen[9] = '\0';

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

// changed RF064
//	theProps->Render(thePlayer->ViewPoint(),
//		LastElapsedTime_D());
	if(ShowTrack)
		theScriptPoints->Render();

	theOverlay->Render();
	theTerrainMgr->Render();
// end change RF064

	return;
}

// start change scripting
/* ------------------------------------------------------------------------------------ */
//	GetHashCommand
/* ------------------------------------------------------------------------------------ */
long CCommonData::GetHashCommand(std::string key)
{
	long *value = CommandHash.GetMember(key);

	if(value)
		return *value;

	return 0;
}

/* ------------------------------------------------------------------------------------ */
//	AddHashCommand
/* ------------------------------------------------------------------------------------ */
bool CCommonData::AddHashCommand(std::string key,long value)
{
	bool add;
	long *vlong = new long;

	*vlong = value;
	add = CommandHash.AddKey(key, vlong);

	return add;
}

// changed QD 08/15/06
/* ------------------------------------------------------------------------------------ */
//	FillHashCommands_LowLevel
/* ------------------------------------------------------------------------------------ */
void CCommonData::ClearHashCommands_LowLevel(void)
{
	// default destructor of hashtable does not free user allocated memory!
	CommandHash.RemoveAllKey(true);
}
// end change QD 08/15/06

/* ------------------------------------------------------------------------------------ */
//	FillHashCommands_LowLevel
/* ------------------------------------------------------------------------------------ */
void CCommonData::FillHashCommands_LowLevel(void)
{
	AddHashCommand("HighLevel",					1);		// HighLevel
	AddHashCommand("Animate",					2);		// Animate
	AddHashCommand("Gravity",					3);		// Gravity
	AddHashCommand("PlaySound",					4);		// PlaySound
	AddHashCommand("EnemyExist",				5);		// EnemyExist
	AddHashCommand("GetEventState",				6);		// GetEventState
	AddHashCommand("Integer",					7);		// Integer
	AddHashCommand("GetAttribute",				8);		// GetAttribute
	AddHashCommand("GetCurFlipBook",			9);		// GetCurFlipBook
	AddHashCommand("ModifyAttribute",			10);	// ModifyAttribute
	AddHashCommand("SetAttribute",				11);	// SetAttribute
	AddHashCommand("SetPlayerWeapon",			12);	// SetPlayerWeapon
	AddHashCommand("SetUseItem",				13);	// SetUseItem
	AddHashCommand("ClearUseItem",				14);	// ClearUseItem
	AddHashCommand("StringCopy",				15);	// StringCopy
	AddHashCommand("LeftCopy",					16);	// LeftCopy
	AddHashCommand("IsEntityVsible",			17);	// IsEntityVisible
	AddHashCommand("DamageEntity",				18);	// DamageEntity
	AddHashCommand("AnimateEntity",				19);	// AnimateEntity
	AddHashCommand("AnimateHold",				20);	// AnimateHold
	AddHashCommand("AnimateTarget",				21);	// AnimateTarget
	AddHashCommand("GetEntityX",				22);	// GetEntityX
	AddHashCommand("GetEntityY",				23);	// GetEntityY
	AddHashCommand("GetEntityZ",				24);	// GetEntityZ
	AddHashCommand("IsKeyDown",					25);	// IsKeyDown
	AddHashCommand("GetEntityYaw",				26);	// GetEntityYaw
	AddHashCommand("MatchEntityAngles",			27);	// MatchEntityAngles
	AddHashCommand("FacePoint",					28);	// FacePoint
	AddHashCommand("NewPoint",					29);	// NewPoint
	AddHashCommand("GetPointYaw",				30);	// GetPointYaw
	AddHashCommand("NextPoint",					31);	// NextPoint
	AddHashCommand("SetTarget",					32);	// SetTarget
	AddHashCommand("GetDistanceTo",				33);	// GetDistanceTo
	AddHashCommand("TeleportEntity",			34);	// TeleportEntity
	AddHashCommand("SaveAttributes",			35);	// SaveAttributes
	AddHashCommand("TraceToActor",				36);	// TraceToActor
	AddHashCommand("AnimateBlend",				37);	// AnimateBlend
	AddHashCommand("AnimationSpeed",			38);	// AnimationSpeed
	AddHashCommand("SetCollision",				39);	// SetCollision
	AddHashCommand("SetNoCollision",			40);	// SetNoCollision
	AddHashCommand("DamageArea",				41);	// DamageArea
	AddHashCommand("PlayerMatchAngles",			42);	// PlayerMatchAngles
	AddHashCommand("ConvertDegrees",			43);	// ConvertDegrees
	AddHashCommand("AttachCamera",				44);	// AttachCamera
	AddHashCommand("AttachCameraToBone",		45);	// AttachCameraToBone
	AddHashCommand("AttachCameraToEntity",		46);	// AttachCameraToEntity
	AddHashCommand("DetachCamera",				47);	// DetachCamera
	AddHashCommand("TiltCamera",				48);	// TiltCamera
	AddHashCommand("PositionToPlatform",		49);	// PositionToPlatform
	AddHashCommand("ActivateTrigger",			50);	// ActivateTrigger
	AddHashCommand("UpdateEnemyVis",			51);	// UpdateEnemyVis
	AddHashCommand("TargetPlayer",				52);	// TargetPlayer
	AddHashCommand("FireProjectileBlind",		53);	// FireProjectileBlind
	AddHashCommand("SetTargetPoint",			54);	// SetTargetPoint
	AddHashCommand("GetBoneX",					55);	// GetBoneX
	AddHashCommand("GetBoneY",					56);	// GetBoneY
	AddHashCommand("GetBoneZ",					57);	// GetBoneZ
	AddHashCommand("GetBoneYaw",				58);	// GetBoneYaw
	AddHashCommand("SetPosition",				59);	// SetPosition
	AddHashCommand("IsButtonDown",				60);	// IsButtonDown
	AddHashCommand("GetJoyAxisX",				61);	// GetJoystickAxisX
	AddHashCommand("GetJoyAxisY",				62);	// GetJoystickAxisY
	AddHashCommand("GetJoyAxisZ",				63);	// GetJoystickAxisZ
	AddHashCommand("GetNumJoysticks",			64);	// GetNumJoysticks
	AddHashCommand("SetBoundingBox",			65);	// SetBoundingBox
	AddHashCommand("GetBoneToBone",				66);	// GetBoneToBone
	AddHashCommand("SwitchView",				67);	// SwitchView
	AddHashCommand("ForceEntityUp",				68);	// ForceEntityUp
	AddHashCommand("ForceEntityDown",			69);	// ForceEntityDown
	AddHashCommand("ForceEntityRight",			70);	// ForceEntityLeft
	AddHashCommand("ForceEntityLeft",			71);	// ForceEntityRight
	AddHashCommand("ForceEntityForward",		72);	// ForceEntityBackward
	AddHashCommand("ForceEntityBackward",		73);	// ForceEntityForward
	AddHashCommand("GetGroundTexture",			74);	// GetGroundTexture
	AddHashCommand("GetPlayerGroundTexture",	75);	// GetPlayerGroundTexture
	AddHashCommand("PositionToBone",			76);	// PositionToBone
	AddHashCommand("SetWeaponMatFromFlip",		77);	// SetWeaponMatFromFlip
	AddHashCommand("SetShadowFromFlip",			78);	// SetShadowFromFlip
	AddHashCommand("GetCollideDistance",		79);	// GetCollideDistance
	AddHashCommand("ResetAnimate",				80);	// ResetAnimate
	AddHashCommand("WhereIsPlayer",				81);	// WhereIsPlayer
	AddHashCommand("WhereIsEntity",				82);	// WhereIsEntity
	AddHashCommand("InsertEvent",				83);	// InsertEvent
	AddHashCommand("CheckForEvent",				84);	// CheckForEvent
	AddHashCommand("PlayEventSound",			85);	// PlayEventSound
	AddHashCommand("LoadAnimation",				86);	// LoadAnimation
	AddHashCommand("StartSoundTrack",			87);	// StartSoundTrack
	AddHashCommand("StopAllAudioStreams",		88);	// StopAllAudioStreams
	AddHashCommand("ChangeYaw",					89);	// ChangeYaw
	AddHashCommand("ChangePitch",				90);	// ChangePitch
	AddHashCommand("random",					91);	// random
	AddHashCommand("walkmove",					92);	// walkmove
	AddHashCommand("flymove",					93);	// flymove
	AddHashCommand("Damage",					94);	// Damage
	AddHashCommand("DamagePlayer",				95);	// DamagePlayer
	AddHashCommand("PositionToPlayer",			96);	// PositionToPlayer
	AddHashCommand("PlayerToPosition",			97);	// PlayerToPosition
	AddHashCommand("PositionToPawn",			98);	// PositionToPawn
	AddHashCommand("SetKeyPause",				99);	// SetKeyPause
	AddHashCommand("PlayerRender",				100);	// PlayerRender
	AddHashCommand("PawnRender",				101);	// PawnRender
	AddHashCommand("ChangeMaterial",			102);	// ChangeMaterial *
	AddHashCommand("SetHoldAtEnd",				103);	// SetHoldAtEnd
	AddHashCommand("ForceUp",					104);	// ForceUp
	AddHashCommand("ForceDown",					105);	// ForceDown
	AddHashCommand("ForceRight",				106);	// ForceLeft
	AddHashCommand("ForceLeft",					107);	// ForceRight
	AddHashCommand("ForceForward",				108);	// ForceBackward
	AddHashCommand("ForceBackward",				109);	// ForceForward
	AddHashCommand("UpdateTarget",				110);	// UpdateTarget
	AddHashCommand("FireProjectile",			111);	// FireProjectile
	AddHashCommand("AddExplosion",				112);	// AddExplosion
	AddHashCommand("GetLastBoneHit",			113);	// GetLastBoneHit
	AddHashCommand("debug",						114);	// debug
	AddHashCommand("SetEventState",				115);	// SetEventState
	AddHashCommand("GetStringLength",			116);	// GetStringLength
	AddHashCommand("DrawText",					117);	// DrawText
	AddHashCommand("DrawFlipBookImage",			118);	// DrawFlipBookImage
	AddHashCommand("DrawPolyShadow",			119);	// DrawPolyShadow
	AddHashCommand("MatchPlayerAngles",			120);	// MatchPlayerAngles
	AddHashCommand("DamageAtBone",				121);	// DamageAtBone
	AddHashCommand("SaveActor",					122);	// SaveActor
	AddHashCommand("LookAtPawn",				123);	// LookAtPawn
	AddHashCommand("AutoWalk",					124);	// AutoWalk
	AddHashCommand("FastDistance",				125);	// FastDistance
	AddHashCommand("StepHeight",				126);	// StepHeight
	AddHashCommand("DrawVPoly",					127);	// DrawVPoly
	AddHashCommand("DrawHPoly",					128);	// DrawHPoly
	AddHashCommand("GetPitchToPoint",			129);	// GetPitchToPoint
	AddHashCommand("GetYawToPoint",				130);	// GetYawToPoint
	AddHashCommand("FastPointCheck",			131);	// FastPointCheck
// changed Nout 12/15/05
	AddHashCommand("SetCameraWindow",			132);	// SetCameraWindow
	AddHashCommand("SetFixedCameraPosition",	133);	// SetFixedCameraPosition
	AddHashCommand("SetFixedCameraRotation",	134);	// SetFixedCameraRotation
	AddHashCommand("SetFixedCameraFOV",			135);	// SetFixedCameraFOV
	AddHashCommand("MoveFixedCamera",			136);	// MoveFixedCamera
	AddHashCommand("RotateFixedCamera",			137);	// RotateFixedCamera
	AddHashCommand("DistanceBetweenEntities",	138);	// DistanceBetweenEntities
	AddHashCommand("SetEntityProperties",		139);	// SetEntityProperties
	AddHashCommand("SetEntityLighting",			140);	// SetEntityLighting
	AddHashCommand("UpdateScriptPoint",			141);	// UpdateScriptPoint
	AddHashCommand("GetEntityScreenX",			142);	// GetEntityScreenX
	AddHashCommand("GetEntityScreenY",			143);	// GetEntityScreenY
	AddHashCommand("GetScreenWidth",			144);	// GetScreenWidth
	AddHashCommand("GetScreenHeight",			145);	// GetScreenHeight
	AddHashCommand("MouseSelect",				146);	// MouseSelect
	AddHashCommand("MouseControlledPlayer",		147);	// MouseControlledPlayer
	AddHashCommand("ShowMouse",					148);	// ShowMouse
	AddHashCommand("GetMousePosX",				149);	// GetMousePosX
	AddHashCommand("GetMousePosY",				150);	// GetMousePosY
	AddHashCommand("SetMousePos",				151);	// SetMousePos
	AddHashCommand("SetGamma",					152);	// SetGamma
	AddHashCommand("GetGamma",					153);	// GetGamma
	AddHashCommand("FillScreenArea",			154);	// FillScreenArea
	AddHashCommand("RemoveScreenArea",			155);	// RemoveScreenArea
	AddHashCommand("ShowText",					156);	// ShowText
	AddHashCommand("RemoveText",				157);	// RemoveText
	AddHashCommand("ShowHudPicture",			158);	// ShowHudPicture
	AddHashCommand("SetHudDraw",				159);	// SetHudDraw
	AddHashCommand("GetHudDraw",				160);	// GetHudDraw
	AddHashCommand("SetAlpha",					161);	// SetAlpha
	AddHashCommand("GetAlpha",					162);	// GetAlpha
	AddHashCommand("SetEntityAlpha",			163);	// SetEntityAlpha
	AddHashCommand("GetEntityAlpha",			164);	// GetEntityAlpha
	AddHashCommand("SetScale",					165);	// SetScale
	AddHashCommand("SetEntityScale",			166);	// SetEntityScale
	AddHashCommand("CheckArea",					167);	// CheckArea
	AddHashCommand("SetFlag",					168);	// SetFlag
	AddHashCommand("GetFlag",					169);	// GetFlag
	AddHashCommand("PowerUp",					170);	// PowerUp
	AddHashCommand("GetPowerUpLevel",			171);	// GetPowerUpLevel
	AddHashCommand("ActivateHudElement",		172);	// ActivateHudElement
	AddHashCommand("MoveEntity",				173);	// MoveEntity
	AddHashCommand("RotateEntity",				174);	// RotateEntity
	AddHashCommand("SetEntityPosition",			175);	// SetEntityPosition
	AddHashCommand("SetEntityRotation",			176);	// SetEntityRotation
	AddHashCommand("AddAttribute",				177);	// AddAttribute
// end change Nout 12/15/05
// changed QD 07/15/06
	AddHashCommand("SetAttributeValueLimits",	178);	// SetAttributeValueLimits
	AddHashCommand("sin",						179);	// sin
	AddHashCommand("cos",						180);	// cos
	AddHashCommand("tan",						181);	// tan
	AddHashCommand("RightCopy",					182);	// RightCopy
	AddHashCommand("NearestPoint",				183);	// NearestPoint
	AddHashCommand("SetFOV",					184);	// SetFOV
// end change QD 07/15/06
//	AddHashCommand("SetSlowMotion",				185);	// SetSlowMotion
//	AddHashCommand("SetGlobal",					186);	// SetGlobal
//	AddHashCommand("GetGlobal",					187);	// GetGlobal
//	AddHashCommand("SaveGlobals",				188);	// SaveGlobals
//	AddHashCommand("LoadGlobals",				189);	// LoadGlobals
//	AddHashCommand("SetRenderHintBox",			190);	// SetRenderHintBox

}
//end change scripting


// start pickles Jul 04
/* ------------------------------------------------------------------------------------ */
//	InitJoysticks
/* ------------------------------------------------------------------------------------ */
void CCommonData::InitJoysticks()
{
// changed QD 07/15/06
	if(!m_bUseDInput)
		return;

    unsigned int numJoysticks = Joystick::deviceCount();

	if(Logging)
	{
		char Info[128];
		sprintf(Info, "*INFO*  Found  %d joysticks", numJoysticks);
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

	for(i=0; i<numJoysticks; i++)
	{
        joysticks[i] = new Joystick(i);
        joysticks[i]->open();
    }
// end change

	return;
}

/* ------------------------------------------------------------------------------------ */
//	PollJoystickAxis
/* ------------------------------------------------------------------------------------ */
int CCommonData::PollJoystickAxis(int jn, int a)
{
// changed QD 07/15/06
	if(!m_bUseDInput)
		return -1;

	//int ax;
    DIJOYSTATE2 js;
    joysticks[jn]->poll(&js);

	if(a == 0)
	{
		//ax = js.lX;
		//return ax;
		return js.lX;
	}

	if(a == 1)
	{
		//ax = js.lY;
		//return ax;
		return js.lY;
	}

	if(a == 2)
	{
		//ax = js.lZ;
		//return ax;
		return js.lZ;
	}
// end change

	return -1;//return error
}

/* ------------------------------------------------------------------------------------ */
//	GetNumJoys
/* ------------------------------------------------------------------------------------ */
int CCommonData::GetNumJoys()
{
// changed QD 07/15/06
	if(!m_bUseDInput)
		return 0;
	//int jc;
	//jc = Joystick::deviceCount();
	//return jc;
	return (int)(Joystick::deviceCount());
// end change
}

/* ------------------------------------------------------------------------------------ */
//	CheckJoystickButton
/* ------------------------------------------------------------------------------------ */
bool CCommonData::CheckJoystickButton(int jn, int bn)
{
// changed QD 07/15/06
	if(!m_bUseDInput)
		return false;
// end change

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
// changed QD 07/15/06
	if(!m_bUseDInput)
		return;
// end change

	unsigned int i;
	unsigned int numJoysticks = Joystick::deviceCount();

    // Close the joysticks.
    for(i=0; i<numJoysticks; i++)
	{
		// changed QD 07/15/06
		if(joysticks[i])
		{
	        joysticks[i]->close();
			delete joysticks[i];
			joysticks[i] = NULL;
		}
		// end change
    }

	return;
}
// end pickles Jul 04


/* ------------------------------------------------------------------------------------ */
//	PlayOpeningCutScene
//
//	Get the opening cut scene file from the PlayerSetup entity and
//	..play it, if one exists.
/* ------------------------------------------------------------------------------------ */
void CCommonData::PlayOpeningCutScene()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	//	Ok, check to see if there's a PlayerSetup around...
	pSet = geWorld_GetEntitySet(theGameEngine->World(), "PlayerSetup");

	if(!pSet)
		return;									// No setup?

	//	Ok, get the setup information.  There should only be one, so
	//	..we'll just take the first one we run into.
	pEntity = geEntity_EntitySetGetNextEntity(pSet, NULL);
	PlayerSetup *pSetup = (PlayerSetup*)geEntity_GetUserData(pEntity);

	if((pSetup->OpeningCutScene != NULL) && (strlen(pSetup->OpeningCutScene) != 0))
	{
// changed RF063
		Play(pSetup->OpeningCutScene, 160, 120, true);
// end change RF063
	}

	return;
}

// changed RF063
/* ------------------------------------------------------------------------------------ */
//	Play
//
// Play a .gif or .avi file
/* ------------------------------------------------------------------------------------ */
void CCommonData::Play(char *szFile, int XPos, int YPos, bool Center)
{
	CString File = szFile;
	File.MakeLower();
	int i = File.Find(".gif");

	if(i >= 0 && i < File.GetLength())
	{
		CAnimGif *SplashAVI = new CAnimGif(szFile, kVideoFile);
		SplashAVI->Play(XPos, YPos, Center);
		delete SplashAVI;
	}
	else
	{
		CAVIPlayer *SplashAVI = new CAVIPlayer;
		SplashAVI->Play(szFile, XPos, YPos, Center);
		delete SplashAVI;
	}
}
// end change RF063

/* ------------------------------------------------------------------------------------ */
//	CheckMediaPlayers
//
//	Poll the status of all of our active media players
/* ------------------------------------------------------------------------------------ */
void CCommonData::CheckMediaPlayers()
{
	if(theCDPlayer)
		theCDPlayer->Check();							// If we have a CD player, check it.

	if(theMIDIPlayer)
		theMIDIPlayer->Check();						// If we have a MIDI player, check it.

	return;
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
FILE *CCommonData::OpenRFFile(int nFileType, char *szFilename, char *szHow)
{
	char szTemp[256];

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
		strcpy(szTemp, ".\\");
		strcat(szTemp, szFilename);
		break;
// changed Nout 12/15/05
	case kScriptFile:
		strcpy(szTemp, ".\\");
		strcat(szTemp, szFilename);
		break;
// end change
	case kRawFile:
		strcpy(szTemp, szFilename);
		break;
	default:
		sprintf(szTemp, "*WARNING* OpenRFFile: bad type '%d' for '%s'", nFileType, szFilename);
		CCD->ReportError(szTemp, false);
		return NULL;
	}

	FILE *pTemp = fopen(szTemp, szHow);				// Open the damn thing.

	//	Debug tracing time
	if(m_DebugLevel == kHighDebugOutput && !pTemp)
	{
		char szDebug[512];
		sprintf(szDebug, "*WARNING* File open attempt failed on type '%d', file '%s'", nFileType, szTemp);
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
bool CCommonData::OpenRFFile(geVFile **theFp, int nFileType, char *szFilename, int nHow)
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
// changed RF063
		strcpy(szTemp,"install\\");
		strcat(szTemp, szFilename);
// end change RF063
		break;
// changed Nout 12/15/05
	case kScriptFile:
		strcpy(szTemp, ".\\");
		strcat(szTemp, szFilename);
		break;
// end change
	case kRawFile:
		strcpy(szTemp, szFilename);
		break;
	default:
		sprintf(szTemp, "*WARNING* OpenRFFile(V): bad type '%d' for '%s'", nFileType, szFilename);
		CCD->ReportError(szTemp, false);
		return NULL;
	}

	//	Ok, open the file up.
	*theFp = geVFile_OpenNewSystem(NULL, GE_VFILE_TYPE_DOS, szTemp, NULL, nHow);


	if(!(*theFp) && VFS)
	{
// changed Nout/QD 12/15/05
		//if(m_DebugLevel == kHighDebugOutput)
		if(Logging)
		{
			char szDebug[256];
			sprintf(szDebug, "*INFO* File '%s' not found in Real File System, searching in VFS", szTemp);
			CCD->ReportError(szDebug, false);
		}
// end change
		*theFp = geVFile_Open(VFS, szTemp, nHow);
	}

	//	Debug tracing time
	if(m_DebugLevel == kHighDebugOutput && !(*theFp))
	{
		char szDebug[512];
		sprintf(szDebug, "\n*WARNING* OpenRFFile: file open attempt failed on type '%d', file '%s'\n", nFileType, szTemp);
		CCD->ReportError(szDebug, false);
	}

	//	If it worked, return true, otherwise return false.
	if(*theFp != NULL)
	{
		if(Logging)
		{
			char szDebug[256];
			sprintf(szDebug, "*INFO* Loaded %s", szTemp);
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
bool CCommonData::FileExist(int nFileType, char *szFilename)
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
// changed RF063
		strcpy(szTemp,"install\\");
		strcat(szTemp, szFilename);
// end change RF063
		break;
// changed Nout 12/15/05
	case kScriptFile:
		strcpy(szTemp, ".\\");
		strcat(szTemp, szFilename);
		break;
// end change
	case kRawFile:
		strcpy(szTemp, szFilename);
		break;
	default:
		sprintf(szTemp, "*WARNING* File %s - Line %d: FileExist: bad type '%d' for '%s'",
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
// changed Nout 12/15/05
	case kScriptFile:
		thePtr = ".\\";
		break;
// end change
	case kRawFile:
		thePtr = ".\\";
		break;
	default:
		{
			char szTemp[256];
			sprintf(szTemp, "*WARNING* File %s - Line %d: GetDirectory: bad type '%d'",
					__FILE__, __LINE__, nType);
			CCD->ReportError(szTemp, false);
		}
		return NULL;
	}

	return thePtr;				// Back to caller with...whatever.
}

// changed RF064
/* ------------------------------------------------------------------------------------ */
//	TimerFunction
/* ------------------------------------------------------------------------------------ */
void CALLBACK CCommonData::TimerFunction(UINT uID, UINT uMsg,
										 DWORD dwUser, DWORD dw1, DWORD dw2)
{
	CCommonData *thePointer = (CCommonData*)dwUser;

	if(thePointer->HasFocus)
		thePointer->TimeCounter += 1;

	return;
}

/* ------------------------------------------------------------------------------------ */
//	Get the amount of time passed since the last call, in milliseconds.
//	..Return the result as a DWORD value.
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
//	Get the amount of time passed since the last call, in milliseconds.
//	..Return the result as a floating-point value.
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
//	Reset the internal timer count variable to the current instant.
//	..This effectively "resets" the game clock.  This is useful for
//	..ignoring a large amount of passed time, such as time spent in
//	..a menu mode or during a level or content load.
/* ------------------------------------------------------------------------------------ */
void CCommonData::ResetClock()
{
	LastTimePoll = TimeCounter;

	LastTimePassed_D = (DWORD)0;
	LastTimePassed_F = (geFloat)0.0f;

	return;
}

/* ------------------------------------------------------------------------------------ */
//	Return the current performance counter as a DWORD.  This supplies
//	..a free-running counter of DWORD width.
/* ------------------------------------------------------------------------------------ */
DWORD CCommonData::FreeRunningCounter()
{
	return (DWORD)TimeCounter;
}

/* ------------------------------------------------------------------------------------ */
//	Return the current performance counter as a float.  This supplies
//	..a free-running counter of floating-point precision
/* ------------------------------------------------------------------------------------ */
geFloat CCommonData::FreeRunningCounter_F()
{
	geFloat TheRealTime;

	TheRealTime = (geFloat)TimeCounter;

	return TheRealTime;
}

/* ------------------------------------------------------------------------------------ */
//	Waste time, useful for preventing the Genesis3D engine from
//	..being called so fast that the numbers returned are too
//	..small to be accurate.
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

	return;
}
// end change RF064

/* ------------------------------------------------------------------------------------ */
//	SetChangeLevelData
//
//	Store away all the information we need to do a level change.
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
		strcpy(m_NewLevel, pItem->szNewLevel);

	strcpy(m_SplashScreen, pItem->szSplashFile);
	strcpy(m_SplashAudio, pItem->szAudioFile);
	strcpy(m_CutScene, pItem->szCutScene);
	strcpy(m_Message, pItem->szMessage);

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
		// changed QD 12/15/05 MAX_WEAPONS = 40
		for(i=0; i<40; i++)
			Slot[i] = theWeapon->GetSlot(i);

		CurrentWeapon = theWeapon->GetCurrent();
	}

	UseEffect = pItem->UseEffect;
	cColor = pItem->EffectColor;

	return;
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
	PlayerSetup *pSetup = (PlayerSetup*)geEntity_GetUserData(pEntity);

	if(!EffectC_IsStringNull(pSetup->Deathlevel))
		strcpy(m_NewLevel, pSetup->Deathlevel);
	else
		strcpy(m_NewLevel, theMenu->GetLevelName());

	strcpy(m_SplashScreen, "");
	strcpy(m_SplashAudio, "");
	strcpy(m_CutScene, "");

	if(EffectC_IsStringNull(pSetup->DeathMessage))
		strcpy(m_Message, "");
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

	// changed QD 12/15/05 MAX_WEAPONS = 40
	for(i=0; i<40; i++)
		Slot[i] = theWeapon->GetSlot(i);

	CurrentWeapon = theWeapon->GetCurrent();
	KeepAttributes = true;
	SplashHold = false;

	return;
}

/* ----------------------------------- END OF FILE ------------------------------------ */
