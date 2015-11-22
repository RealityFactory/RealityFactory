/*
	RabidFrameworkMain.cpp:		RealityFactory Main Program File

	(c) 1999 Edward A. Averill, III

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

//  _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

//  _CrtSetBreakAlloc(11203);

//	Initialize the Common Data class that handles components

  CCD = new CCommonData();
	if(CCD == NULL)
	  {
		OutputDebugString("CCommonData init failure, exiting\n");
		MessageBox(NULL, "CCommonData init failure, exiting", "GAME ERROR",
				MB_ICONSTOP | MB_OK);
		exit(-666);
		}

  ShowCursor(FALSE);						// Turn off the mouse cursor
  
//	Ok, we'll check the command line and see if we got an argument from
//	..rfEdit or some other spawning program.  This also lets us drive
//	..the first level run from the command-line, useful in many
//	..circumstances.

	szFirstLevel[0] = 0;
	CommandLine = false;

	if((lpszCmdParam != NULL) && (strlen(lpszCmdParam) > 0))
	{
		char *szFoo = strtok(lpszCmdParam," ");
		if(stricmp("-map", szFoo) != 0)
		 {
			delete CCD;					// Unknown command line parm
			MessageBox(NULL,lpszCmdParam,"RGF: command line error", MB_OK);
			exit(-335);
		}
		szFoo = strtok(NULL, "\n\000");
		strcpy(szFirstLevel, szFoo);
		szFirstLevel[strlen(szFirstLevel)] = 0;
		strcat(szFirstLevel,".bsp");
		CommandLine = true;
	}

//	Fine, let's initialize the Genesis engine & etc.

	if((nResult = CCD->InitializeCommon(hInstance, szFirstLevel, CommandLine)) != 0)
	  {
		OutputDebugString("RGF initialization failure, exiting\n");
		MessageBox(NULL, "RGF Initialization Failure", "GAME ERROR",
			MB_ICONSTOP | MB_OK);
		delete CCD;						// Drop everything to prevent leaks
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
			MessageBox(NULL,"No Level to Load","RF Error", MB_OK);
			exit(-335);
		}
	}

//	Center the invisible mouse cursor

	POINT theCursor;
	theCursor.x = 320; theCursor.y = 240;
	ClientToScreen(CCD->Engine()->WindowHandle(), &theCursor);
	SetCursorPos(theCursor.x, theCursor.y);

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
				CAVIPlayer *SplashAVI = new CAVIPlayer; 
				SplashAVI->Play(CCD->CutScene(), 160, 120, true);
				delete SplashAVI;
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
				CAVIPlayer *SplashAVI = new CAVIPlayer; 
				SplashAVI->Play(CCD->CutScene1(), 160, 120, true);
				delete SplashAVI;
			}
		}
		CCD->MenuManager()->SetLevelName(szFirstLevel);
		if((nResult = CCD->InitializeLevel(szFirstLevel)) != 0)
		{
			CCD->ReportError("Couldn't initialize first level", false);
			CCD->ShutdownLevel();						// Clean up anything that got loaded
			delete CCD;						// Kill off the engine and such
			MessageBox(NULL, szFirstLevel,"RGF: Can't load level", MB_OK);
			exit(-333);
		}

//	Ok, move the player avatar to the correct player start in the
//	..game level.

		if(CCD->Player()->MoveToStart() != RGF_SUCCESS)
		{
			CCD->ReportError("Can't move player to start", false);
			CCD->ShutdownLevel();
			delete CCD;
			exit(-336);
		}
		CCD->MenuManager()->GameLoop();
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
				CAVIPlayer *SplashAVI = new CAVIPlayer; 
				SplashAVI->Play(CCD->CutScene(), 160, 120, true);
				delete SplashAVI;
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
				CAVIPlayer *SplashAVI = new CAVIPlayer; 
				SplashAVI->Play(CCD->CutScene1(), 160, 120, true);
				delete SplashAVI;
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

