/************************************************************************************//**
 * @file RabidFrameworkMain.cpp
 * @brief Reality Factory Main Program File
 *
 * This is the main execution path for RealityFactory. This code handles level
 * loading and changing, time-tick dispatching, and general user input functionality.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

// This is the MAIN application, so we define the preprocessor
// ..variable that lets the master include file set up all the
// ..proper globals (euuuww, globals, but this is the fastest
// ..way to do things).

#define _THE_MASTER_MODULE_

#include "RabidFramework.h"
#include <process.h>

static const char *GetOSName(LPOSVERSIONINFO versionInfo);
static void DisplayAllSplashScreens();
static void DisplaySplashScreen(const char *splashScreen,
								const char *splashAudio,
								const char *cutScene);

/* ------------------------------------------------------------------------------------ */
// WinMain
//
// This is the initial entry point for all Windows programs.
// Here, we will initialize all the subsystems and drop down into our game's main loop.
/* ------------------------------------------------------------------------------------ */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR lpszCmdParam, int /*nCmdShow*/)
{
	DWORD nLoopTimer = 0;

	char m_currentdir[512];
	_getcwd(m_currentdir, 512);
	chdir(m_currentdir);

	// _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	// _CrtSetBreakAlloc(24312);

	//	Initialize the Common Data class that handles components
	CCD = NULL;
	CCD = new CCommonData();

	if(CCD == NULL)
	{
		OutputDebugString("CCommonData init failure, exiting\n");
		MessageBox(NULL, "CCommonData init failure, exiting", "GAME ERROR",
				MB_ICONSTOP | MB_OK);
		exit(-666);
	}

	// Ok, we'll check the command line and see if we got an argument from
	// rfEdit or some other spawning program. This also lets us drive the
	// first level run from the command line, useful in many circumstances.


	bool spawnVideoSetup = false;
	FILE *fd = fopen("D3D24.ini", "r");

	if(!fd)
	{
		spawnVideoSetup = true;
	}
	else
	{
		fclose(fd);
	}

	ShowCursor(FALSE);				// Turn off the mouse cursor

	bool multiplayerLaunch = false; // directly launching a multiplayer game, bypasses menu
	bool commandLine = false;		// debug launch
	char szFirstLevel[256] = {};

	// check command line parameters if any
	if((lpszCmdParam != NULL) && (strlen(lpszCmdParam) > 0))
	{
		char *szFoo = strtok(lpszCmdParam, " ");

		if(!stricmp("-map", szFoo))
		{
			szFoo = strtok(NULL, "\n\000"); // remove any null/ugly chars
			strcpy(szFirstLevel, szFoo);
			szFirstLevel[strlen(szFirstLevel)] = 0;
			strcat(szFirstLevel, ".bsp");
			commandLine = true;
		}
		else if(!stricmp("-video", szFoo))
		{
			spawnVideoSetup = true;
		}

		// can pass +port & +connect to the rf client to directly launch into a multiplayer game
		// should also provide a +dedicated TRUE flag as well to launch a server directly add
		// launch multiplayer from command line

		// do we have a server port? NOTE only useful if we have a serverIP as well
		char szServerPort[256]	= {};
		char szServerIP[256]	= {};

		if(!stricmp("+port", szFoo))
		{
			szFoo = strtok(NULL, "\n\000");	// remove any null/ugly chars
			strcpy(szServerPort, ":");
			strcpy(szServerPort, szFoo);
			szServerPort[strlen(szServerPort)] = 0;
  		    CCD->ReportError("Caching Server Port...", false);
		}
		// do we have a server ip, launch directly into the server?
		else if(!stricmp("+connect", szFoo))
		{
			// connect to 'szFoo' IP address:Port
			// TODO: Set our server ip:port to connect to automatically, bypass menu
			szFoo = strtok(NULL, "\n\000"); // remove any null/ugly chars
			strcpy(szServerIP, szFoo);
			szServerIP[strlen(szServerIP)] = 0;

			if(!EffectC_IsStringNull(szServerPort))
			{
				strcat(szServerPort, szServerIP);	// check to see if we have a port set
			}
			else
			{
				strcat(szServerIP, ":1972");		// tag our default port on instead
			}

			multiplayerLaunch = true; // launch directly into game
			CCD->ReportError("Caching Server IP...Launching Multiplayer Game directly", false);
		}
	}

	if(spawnVideoSetup)
	{
		CCD->ReportError("No Renderer Config (d3d24.ini), running videosetup", false);

		char *args[2];
		args[0] = "video";
		args[1] = NULL;

		_spawnv(_P_WAIT, "videosetup.exe", args);
	}


	// Fine, let's initialize the Genesis engine & etc.
	int result = CCD->Initialize(hInstance, szFirstLevel, commandLine);

	if(result != 0)
	{
		OutputDebugString("RGF initialization failure, exiting\n");
		MessageBox(NULL, "RGF Initialization Failure", "GAME ERROR",
			MB_ICONSTOP | MB_OK);
		delete CCD;						// Drop everything to prevent leaks
		CCD = NULL;
		exit(result);					// Failure to initialize RGF
	}

    CCD->ReportError("Launching Reality Factory Game Shell...", false);

	if(!commandLine)
	{
		// No command line arg, if there wasn't anything from the .ini file, error exit.
		if(szFirstLevel[0] == 0)
		{
			delete CCD;					// Unknown command line parm
			CCD = NULL;
			MessageBox(NULL,"No Level to Load","RF Error", MB_OK);
			exit(-335);
		}
	}

	// Center the invisible mouse cursor
	RECT client;
	POINT pos;

	if(CCD->Engine()->FullScreen())
    {
		pos.x = CCD->Engine()->Width()/2;	// calculate the center of the screen
		pos.y = CCD->Engine()->Height()/2;	// calculate the center of the screen
		SetCursorPos(pos.x, pos.y);			// set the cursor in the center of the screen
    }
	else
    {
		GetClientRect(CCD->Engine()->WindowHandle(),&client);	// get the client area of the window
		pos.x = client.right/2;									// calculate the center of the client area
		pos.y = client.bottom/2;								// calculate the center of the client area
		ClientToScreen(CCD->Engine()->WindowHandle(),&pos);		// convert to SCREEN coordinates
		SetCursorPos(pos.x,pos.y);								// put the cursor in the middle of the window
    }

	if(commandLine)
	{
		CCD->ReportError("Launching Preview from Editor, bypassing Genesis3D Logo for DEBUG purposes ONLY...", false);

		// show logos
		DisplayAllSplashScreens();

		CCD->MenuManager()->SetLevelName(szFirstLevel);
		CCD->ReportError("Previewing Level as SinglePlayer Client...", false);
		CCD->MenuManager()->DoGame(true);
	}
	// added direct multiplayer launch from command line
	// todo:  update the server IP & Port and launch the multiplayer game instead
	// todo:  provide for dedicated server launching as well as remote client launching
	else if(multiplayerLaunch)
	{
		// show logos
		DisplayAllSplashScreens();


		// launch our network game
		// todo:  first have to set the serverip & port from our command line variables
		CCD->MenuManager()->SetLevelName(szFirstLevel);
		CCD->ReportError("Launching Game as Multiplayer Client...", false);
		CCD->MenuManager()->DoGame(true);
	}
	else
	{

		// show logos
		DisplayAllSplashScreens();
	}

	CCD->MenuManager()->DoMenu(szFirstLevel);
	CCD->ReportError("Shutting Down Reality Factory Game Shell...", false);
	CCD->ShutdownLevel();					// Kill off level-specific entities

	delete CCD;								// Kill off the engine and such

	return(0);								// This exits the Windows application
}


/* ------------------------------------------------------------------------------------ */
// Get the operating system name from the osversioninfo struct
/* ------------------------------------------------------------------------------------ */
const char *GetOSName(LPOSVERSIONINFO versionInfo)
{
	if(versionInfo->dwPlatformId == VER_PLATFORM_WIN32s)
	{
		return "Windows 3.1";
	}
	else if(versionInfo->dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
	{
		if(versionInfo->dwMinorVersion == 0)	return "Windows 95";
		if(versionInfo->dwMinorVersion == 10)	return "Windows 98";
		if(versionInfo->dwMinorVersion == 90)	return "Windows ME";
	}
	else if(versionInfo->dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		if(versionInfo->dwMajorVersion == 3)
		{
			return "Windows NT 3.5";
		}
		else if(versionInfo->dwMajorVersion == 4)
		{
			return "Windows NT 4";
		}
		else if(versionInfo->dwMajorVersion == 5)
		{
			if(versionInfo->dwMinorVersion == 0)
				return "Windows 2000";
			if(versionInfo->dwMinorVersion == 1)
				return "Windows XP";
			if(versionInfo->dwMinorVersion == 2)
				return "Windows Home Server / Windows Server 2003";
		}
		else if(versionInfo->dwMajorVersion == 6)
		{
			if(versionInfo->dwMinorVersion == 0)
				return "Windows Vista / Windows Server 2008";
			if(versionInfo->dwMinorVersion == 1)
				return "Windows 7 / Windows Server 2008 R2";
			if(versionInfo->dwMinorVersion == 2)
				return "Windows 8 / Windows Server 2012";
			if(versionInfo->dwMinorVersion == 3)
				return "Windows 8.1 / Windows Server 2012 R2";
		}
		else if(versionInfo->dwMajorVersion == 10)
		{
			if(versionInfo->dwMinorVersion == 0)
				return "Windows 10 / Windows Server";
		}
	}

	return "Windows";
}


/* ------------------------------------------------------------------------------------ */
// Display Splash Screens
/* ------------------------------------------------------------------------------------ */
void DisplayAllSplashScreens()
{
	DisplaySplashScreen(CCD->SplashScreen(), CCD->SplashAudio(), CCD->CutScene());
	DisplaySplashScreen(CCD->SplashScreen1(),CCD->SplashAudio1(),CCD->CutScene1());
}


void DisplaySplashScreen(
	const char *splashScreen,
	const char *splashAudio,
	const char *cutScene)
{
	if(splashScreen)
	{
		CCD->Engine()->DisplaySplash(splashScreen, splashAudio);
		CCD->Spin(3000);
	}
	else if(cutScene)
	{
		CCD->Play(cutScene, 160, 120, true);
	}
}

/* ----------------------------------- END OF FILE ------------------------------------ */
