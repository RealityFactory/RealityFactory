/*
	RabidFrameworkMain.cpp:		RealityFactory Main Program File

	(c) 2001 Ralph Deane

	This is the main execution path for RealityFactory.  This code handles
level loading and changing, time-tick dispatching, and general user input
functionality.

*/

//	This is the MAIN application, so we define the preprocessor
//	..variable that lets the master include file set up all the
//	..proper globals (euuuww, globals, but this is the fastest
//	..way to do things).

#define _THE_MASTER_MODULE_

//	You only need one include file.

#include "RabidFramework.h"
#include <process.h>

//	WinMain
//
//	This is the initial entry point for all Windows programs.
//
//	Here, we will initialize all the subsystems and drop down into our
//	..game's main loop.

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	int nResult;
	bool CommandLine;
	DWORD nLoopTimer = 0;
  char szFirstLevel[256];
// changed RF064
  char m_currentdir[512];

  _getcwd(m_currentdir, 512);
  chdir(m_currentdir);
// end change RF064

//  _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

//  _CrtSetBreakAlloc(24312);

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

//	Ok, we'll check the command line and see if we got an argument from
//	..rfEdit or some other spawning program.  This also lets us drive
//	..the first level run from the command-line, useful in many
//	..circumstances.

	szFirstLevel[0] = 0;
	CommandLine = false;

	FILE *fd;
	bool vidsetup = false;
	fd = fopen("D3D24.ini","r");
	if(!fd)
		vidsetup = true;

	ShowCursor(FALSE);						// Turn off the mouse cursor

	if((lpszCmdParam != NULL) && (strlen(lpszCmdParam) > 0))
	{
		char *szFoo = strtok(lpszCmdParam," ");
		if(!stricmp("-map", szFoo))
		{
			szFoo = strtok(NULL, "\n\000");
			strcpy(szFirstLevel, szFoo);
			szFirstLevel[strlen(szFirstLevel)] = 0;
			strcat(szFirstLevel,".bsp");
			CommandLine = true;
		}
		else if(!stricmp("-video", szFoo))
			vidsetup = true;
	}

	if(vidsetup)
	{
		char *args[2];
		args[0] = "video";
		args[1] = NULL;

		_spawnv(_P_WAIT, "videosetup.exe", args);
	}

//	Fine, let's initialize the Genesis engine & etc.

	if((nResult = CCD->InitializeCommon(hInstance, szFirstLevel, CommandLine)) != 0)
	  {
		OutputDebugString("RGF initialization failure, exiting\n");
		MessageBox(NULL, "RGF Initialization Failure", "GAME ERROR",
			MB_ICONSTOP | MB_OK);
		delete CCD;						// Drop everything to prevent leaks
		CCD = NULL;
	  exit(nResult);							// Failure to initialize RGF
		}

  CCD->ReportError("Game started", false);

	if(!CommandLine)
	{
		// No command line arg, if there wasn't anything from the .ini
		// ..file, error exit.
		if(szFirstLevel[0] == 0)
		{
			delete CCD;					// Unknown command line parm
			CCD = NULL;
			MessageBox(NULL,"No Level to Load","RF Error", MB_OK);
			exit(-335);
		}
	}

//	Center the invisible mouse cursor

	RECT client;
	POINT pos;
	if(CCD->Engine()->FullScreen())
    {
		pos.x = CCD->Engine()->Width()/2;			// calculate the center of the screen
		pos.y = CCD->Engine()->Height()/2;			// calculate the center of the screen
		SetCursorPos(pos.x, pos.y);	// set the cursor in the center of the screen
    }
	else
    {
		GetClientRect(CCD->Engine()->WindowHandle(),&client);	// get the client area of the window
		pos.x = client.right/2;						// calculate the center of the client area
		pos.y = client.bottom/2;					// calculate the center of the client area
		ClientToScreen(CCD->Engine()->WindowHandle(),&pos);		// convert to SCREEN coordinates
		SetCursorPos(pos.x,pos.y);					// put the cursor in the middle of the window
    }

	if(CommandLine)
	{
		if(!EffectC_IsStringNull(CCD->SplashScreen()))
		{
			if(!EffectC_IsStringNull(CCD->SplashAudio()))
			{
				CCD->Engine()->DisplaySplash(CCD->SplashScreen(),CCD->SplashAudio());
				CCD->Spin(3000);
			}
		}
		else
		{
			if(!EffectC_IsStringNull(CCD->CutScene()))
			{
// changed RF063
				CCD->Play(CCD->CutScene(), 160, 120, true);
// end change RF063
			}
		}
		if(!EffectC_IsStringNull(CCD->SplashScreen1()))
		{
			if(!EffectC_IsStringNull(CCD->SplashAudio1()))
			{
				CCD->Engine()->DisplaySplash(CCD->SplashScreen1(),CCD->SplashAudio1());
				CCD->Spin(3000);
			}
		}
		else
		{
			if(!EffectC_IsStringNull(CCD->CutScene1()))
			{
// changed RF063 
				CCD->Play(CCD->CutScene1(), 160, 120, true);
// end change RF063
			}
		}
// changed RF063
		CCD->MenuManager()->SetLevelName(szFirstLevel);
		CCD->MenuManager()->DoGame(true);
// end change RF063
	}
	else
	{	
		if(!EffectC_IsStringNull(CCD->SplashScreen()))
		{
			if(!EffectC_IsStringNull(CCD->SplashAudio()))
			{
				CCD->Engine()->DisplaySplash(CCD->SplashScreen(),CCD->SplashAudio());
				CCD->Spin(3000);
			}
		}
		else
		{
			if(!EffectC_IsStringNull(CCD->CutScene()))
			{
// changed RF063 
				CCD->Play(CCD->CutScene(), 160, 120, true);
// end change RF063
			}
		}
		if(!EffectC_IsStringNull(CCD->SplashScreen1()))
		{
			if(!EffectC_IsStringNull(CCD->SplashAudio1()))
			{
				CCD->Engine()->DisplaySplash(CCD->SplashScreen1(),CCD->SplashAudio1());
				CCD->Spin(3000);
			}
		}
		else
		{
			if(!EffectC_IsStringNull(CCD->CutScene1()))
			{
// changed RF063
				CCD->Play(CCD->CutScene1(), 160, 120, true);
// end change RF063
			}
		}
	}

	CCD->MenuManager()->DoMenu(szFirstLevel);
	CCD->ReportError("Game exiting", false);
	CCD->ShutdownLevel();					// Kill off level-specific entities
	delete CCD;				// Kill off the engine and such

	return(0);								// This exits the Windows application

}

// ----------------------------------------------------------------------------

