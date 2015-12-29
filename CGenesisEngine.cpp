/************************************************************************************//**
 * @file CGenesisEngine.cpp
 * @brief Genesis3D engine encapsulation
 *
 * This file contains the class implementation for the Genesis3D engine wrapper.
 * This wrapper hides many of the gory details necessary to use the Genesis3D.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#include "RabidFramework.h"
#include "AutoSelect.h"
#include "resource.h"


/* ------------------------------------------------------------------------------------ */
// This is the window procedure that will be used by the main
// ..engine wrapper.  I wish there was some way to do without
// ..it, I'll have to research it.
/* ------------------------------------------------------------------------------------ */
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch (iMessage)
	{
	case WM_TIMER:
		if(CCD->NetPlayerManager() != NULL)
			CCD->NetPlayerManager()->ServerClientCycle();
		break;
	case WM_KILLFOCUS:
		{
			if(CCD != NULL)
			{
#ifndef _DEBUG
				ShowWindow(hWnd, SW_MINIMIZE);
				CCD->SetHasFocus(false);

				if(CCD->Engine() != NULL)
					geEngine_Activate(CCD->Engine()->Engine(), false);

				if(CCD->MenuManager())
				{
				}

				return 0;
#endif
			}
		}
		break;
	case WM_SETFOCUS:
		{
			if(CCD != NULL)
			{
				ShowWindow(hWnd, SW_SHOWNORMAL);
				CCD->SetHasFocus(true);

				if(CCD->Engine() != NULL)
					geEngine_Activate(CCD->Engine()->Engine(), true);

				if(CCD->MenuManager())
				{
				}

				return 0;
			}
		}
		break;
	case WM_WINDOWPOSCHANGED:
		{
			if(CCD->Engine() != NULL)
				geEngine_UpdateWindow(CCD->Engine()->Engine());
			return 0;
		}
    }
/*
	if(iMessage == WM_CHAR)
	{
		if(CCD->GetConsole())
		{
			char chCharCode = (TCHAR) wParam;
			int key = (lParam>>16) & 255;

			char szBug[255];
			sprintf(szBug,"%x", key);
			CCD->ReportError(szBug, false);
		}
	}
*/
	return DefWindowProc(hWnd, iMessage, wParam, lParam);
}

/* ------------------------------------------------------------------------------------ */
// CGenesisEngine
//
// Construct a main window and initialize the engine, also start up the
// ..Genesis sound system.
/* ------------------------------------------------------------------------------------ */
CGenesisEngine::CGenesisEngine(bool fFullScreen, int nWidth, int nHeight,
							   const char *szName, HINSTANCE theInstance,
							   char chDriverID, bool /*fSoftware*/,
							   bool UseDialog, char *szStartLevel)
{
	WNDCLASS wc;
	HWND hWnd;

	unlink(".\\RealityFactory.log");			// Gun old log file

	m_nWidth			= nWidth;
	m_nHeight			= nHeight;
	m_fFullScreen		= fFullScreen;
	m_MasterVolume		= 0;
	m_SplashAudio		= NULL;
	m_World				= NULL;
	m_fUseSoftware		= fSoftware;
	m_CurrentLevel[0]	= 0;
	m_SelectedDriverID	= chDriverID;			// Use the desired driver
	fFogStart = fFogEnd = 0.0f;					// Fog start, end distances

	if(UseDialog)
	{
		char m_currentdir[512];

		_getcwd(m_currentdir, 512);
		ShowCursor(TRUE);

		OPENFILENAME OpenFileName;
		TCHAR szFile[_MAX_PATH] = "\0";
		TCHAR szFileTitle[_MAX_PATH] = "\0";

		TCHAR m_dir[512];
		strcpy(m_dir, m_currentdir);
		strcat(m_dir, "\\media\\levels");

		// Fill in the OPENFILENAME structure to support a template and hook.
		OpenFileName.lStructSize		= sizeof(OPENFILENAME);
		OpenFileName.hwndOwner			= NULL;
		OpenFileName.hInstance			= theInstance;
		OpenFileName.lpstrFilter		= "Level BSP Files\0*.bsp\0All Files\0*.*\0\0";
		OpenFileName.lpstrCustomFilter	= NULL;
		OpenFileName.nMaxCustFilter		= 0;
		OpenFileName.nFilterIndex		= 0;
		OpenFileName.lpstrFile			= szFile;
		OpenFileName.nMaxFile			= sizeof(szFile);
		OpenFileName.lpstrFileTitle		= szFileTitle;
		OpenFileName.nMaxFileTitle		= sizeof(szFileTitle);
		OpenFileName.lpstrInitialDir	= m_dir;
		OpenFileName.lpstrTitle			= "Load Level";
		OpenFileName.nFileOffset		= 0;
		OpenFileName.nFileExtension		= 0;
		OpenFileName.lpstrDefExt		= NULL;
		OpenFileName.lCustData			= 0;
		OpenFileName.lpfnHook			= 0;
		OpenFileName.lpTemplateName		= 0;
		OpenFileName.Flags				= OFN_EXPLORER;

		// Call the common dialog function.
		if(GetOpenFileName(&OpenFileName))
		{
			std::string FileName = OpenFileName.lpstrFileTitle;
			strcpy(szStartLevel, FileName.c_str());
			std::string PathName = OpenFileName.lpstrFile;
			PathName = PathName.substr(0, PathName.rfind('\\'));
			TCHAR m_lev[512];
			strcpy(m_lev, PathName.c_str());

			CCD->SetLevelDirectory(m_lev);
		}

		chdir(m_currentdir);

		ShowCursor(FALSE);
	}

	GetWindowRect(GetDesktopWindow(), &m_ScreenRect);

	// Set up and register application window class

	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= (WNDPROC)WndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= theInstance;
	wc.hIcon			= LoadIcon(theInstance, MAKEINTRESOURCE(IDI_ICON1));
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= szName;
	RegisterClass(&wc);

	// Create the main window
	hWnd = CreateWindowEx(0, szName, szName, WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT,
							nWidth, nHeight, NULL, NULL, theInstance, NULL);

	if(!hWnd)
	{
		OutputDebugString("Main window creation failure\n");
		ReportError("[ERROR] Main window creation failure", false);
		exit(-1);
	}

	UpdateWindow(hWnd);

	SetWindowPos(hWnd, HWND_TOP,
				((m_ScreenRect.right + m_ScreenRect.left) / 2) - (nWidth / 2),
				((m_ScreenRect.bottom + m_ScreenRect.top) / 2) - (nHeight / 2),
				nWidth, nHeight,
				SWP_NOCOPYBITS | SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);

	m_wndMain = hWnd;					// Save for later use

	m_Instance = theInstance;			// Save for later

	// Ok, now create the engine and let 'er rip!
	if(CreateEngine(szName) == FALSE)
	{
		DestroyWindow(m_wndMain);					// Clean up
		ReportError("[ERROR] Genesis3D engine creation failure", false);
		exit(-2);
	}

	m_DebugEnabled = false;							// No debug by default
	m_fInFramePass = false;							// Not in frame rendering

	//	Now set up the sound system.
	m_Audio = geSound_CreateSoundSystem(m_wndMain);

	if(m_Audio == NULL)
	{
		DestroyWindow(m_wndMain);
		ReportError("[ERROR] Genesis3D audio subsystem creation failure", false);
		exit(-86);
	}

	geSound_SetMasterVolume(m_Audio, 0.99f);			// 100% audio for now.
}

/* ------------------------------------------------------------------------------------ */
// ~CGenesisEngine
//
// Shut down the Genesis3D engine
/* ------------------------------------------------------------------------------------ */
CGenesisEngine::~CGenesisEngine()
{
	if(m_SplashAudio != NULL)
	{
		// Drop old splash audio effect
		geSound_FreeSoundDef(m_Audio, m_SplashAudio);
		m_SplashAudio = NULL;
	}

	geSound_DestroySoundSystem(m_Audio);
	m_Audio = NULL;

	if(m_World != NULL)
		geEngine_RemoveWorld(m_theEngine, m_World);

	if(m_World != NULL)
		geWorld_Free(m_World);

	if(m_theEngine != NULL)
	{
		geEngine_ShutdownDriver(m_theEngine);
		geEngine_Free(m_theEngine);
	}
}

/* ------------------------------------------------------------------------------------ */
// CreateEngine
//
// Set up the Genesis3D engine and attach it to the main window, which
// ..must exist prior to this member function being called.
/* ------------------------------------------------------------------------------------ */
bool CGenesisEngine::CreateEngine(const char *szName)
{
	m_World = NULL;

	// Create the genesis engine
	m_theEngine = geEngine_Create(m_wndMain, szName, ".");

	if(!m_theEngine)
	{
		ReportError("[ERROR] geEngine_Create failure", false);
		return false;
	}

	// Turn off the framerate counter
	geEngine_EnableFrameRateCounter(m_theEngine, GE_FALSE);

	// Create the genesis driver system
	m_DrvSys = geEngine_GetDriverSystem(m_theEngine);

	if(!m_DrvSys)
	{
		ReportError("[ERROR] geEngine_GetDriverSystem failure", false);
		return false;
	}

	// Find any available driver and use the first mode it has
	if(m_SelectedDriverID == 'A')
	{
		if(AutoDriver() == false)
		{
			ReportError("[ERROR] Failed to locate any available Genesis3D driver", false);
			return false;
		}
	}
	// Pop a Pick List to choose the mode you'd like (out of what's available)
	else if(m_SelectedDriverID == 'P')
	{
		if(PickDriver() == false)
		{
			// Probably hit "Pick Something for Me" or ESC...
			// So try to auto select for them
			ReportError("[INFO] Auto-Detecting Driver", false);
			if(AutoDriver() == false)
			{
				ReportError("[ERROR] Failed to locate any available Genesis3D driver", false);
				return false;
			}
		}

		// Added to support fullscreen or windowed driver change via pick list
		{
			geEngine_ShutdownDriver(m_theEngine);
			ResetMainWindow(m_wndMain, m_nWidth, m_nHeight);

			geBoolean cline;

			if(CCD->GetCmdLine())
				cline = geEngine_SetDriverAndModeNoSplash(m_theEngine, m_Driver, m_Mode);
			else
				cline = geEngine_SetDriverAndMode(m_theEngine, m_Driver, m_Mode);

			if(!cline)
			{
				ReportError("[ERROR] geEngine_SetDriverAndMode failure", false);
				return false;
			}
		}
	}
	// Find ourselves a good driver
	else
	{
		if(FindDriver() == FALSE)
		{
			ReportError("[ERROR] Failed to locate good Genesis3D driver", false);
			return false;
		}

		// let the genesis engine intialize with our driver
		geBoolean cline;

		if(CCD->GetCmdLine())
			cline = geEngine_SetDriverAndModeNoSplash(m_theEngine, m_Driver, m_Mode);
		else
			cline = geEngine_SetDriverAndMode(m_theEngine, m_Driver, m_Mode);

		if(!cline)
		{
			ReportError("[ERROR] geEngine_SetDriverAndMode failure", false);
			return false;
		}
	}

	// Holy toledo, it all worked!  Back to the caller, then.
	return true;
}

/* ------------------------------------------------------------------------------------ */
// FindDriver
//
// Locate a driver that'll do what we want, how we want it.
/* ------------------------------------------------------------------------------------ */
bool CGenesisEngine::FindDriver()
{
	long Height = 0, Width = 0;
	const char *modename = NULL;
	const char *drvname = NULL;

	if(m_fFullScreen)
	{
		ReportError("Searching for fullscreen driver", false);
		m_Driver = geDriver_SystemGetNextDriver(m_DrvSys, NULL);

		if(!m_Driver)
		{
			ReportError("[ERROR] geDriver_SystemGetNextDriver(FULLSCREEN) failure", false);
			return FALSE;
		}

		while(1)
		{
			geDriver_GetName(m_Driver, &drvname);

			if(drvname[0] == m_SelectedDriverID)
				break;							// This is the driver type we want

			m_Driver = geDriver_SystemGetNextDriver(m_DrvSys, m_Driver);

			if(!m_Driver)
			{
				ReportError("[ERROR] Unexpected SystemGetNextDriver error", false);
				return FALSE;
			}
		}

		// Ok, we have the DRIVER we want, look for the MODE we want
		m_Mode = geDriver_GetNextMode(m_Driver, NULL);

		while(1)
		{
			if(!m_Mode)
			{
				CCD->ReportError("GetNextMode - *No Mode* failure", false);
				return FALSE;
			}

			geDriver_ModeGetWidthHeight(m_Mode, &Width, &Height);

			if(Width == m_nWidth && Height == m_nHeight)
			{
				char szFoo[132];
				sprintf(szFoo, "Driver %s width %d height %d selected", drvname, Width, Height);
				CCD->ReportError(szFoo, false);
				break;							// Right DRIVER, right MODE, we're out!
			}

			m_Mode = geDriver_GetNextMode(m_Driver, m_Mode);
		}
	}
	else							// ** Not going full-screen
	{
		ReportError("Searching for windowed driver", false);
		m_Driver = geDriver_SystemGetNextDriver(m_DrvSys, NULL);

		if(!m_Driver)
		{
			ReportError("[ERROR] geDriver_SystemGetNextDriver(FULLSCREEN) failure", false);
			return FALSE;
		}

		while(1)
		{
			geDriver_GetName(m_Driver, &drvname);

			if(drvname[0] == m_SelectedDriverID)
			{
				if(m_SelectedDriverID == 'S')
				{
					if(!strnicmp(drvname, "Software (Window)", 17))
						break;							// This is the driver type we want
				}
				else
					break;
			}

			m_Driver = geDriver_SystemGetNextDriver(m_DrvSys, m_Driver);

			if(!m_Driver)
			{
				ReportError("[ERROR] Unexpected SystemGetNextDriver error", false);
				return FALSE;
			}
		}

		m_Mode = geDriver_GetNextMode(m_Driver, NULL);
		const char *pModeName;

		while(1)
		{
			if(!m_Mode)
			{
				CCD->ReportError("[ERROR] GetNextMode - *No Mode* failure", false);
				return FALSE;
			}

			geDriver_ModeGetName(m_Mode, &pModeName);

			// Try to find a windowed mode - we want an accelerated driver
			if(!strnicmp(pModeName, "Window", 6))
				break;

			m_Mode = geDriver_GetNextMode(m_Driver, m_Mode);
		}

		if(!(m_Driver && m_Mode))
		{
			ReportError("[ERROR] Failed to find windowed driver and mode", false);
			return FALSE;
		}
	}

	// Ok, driver AND mode found that we're happy with, bail this.
	return TRUE;
}

/* ------------------------------------------------------------------------------------ */
// AutoDriver
//
// Select any available driver and use it's first mode
/* ------------------------------------------------------------------------------------ */
bool CGenesisEngine::AutoDriver()
{
	ModeList	*DriverModeList;
	int			DriverModeListLength;
	int			Selection = 0;

	// Create a List of Available Drivers
	DriverModeList = ModeList_Create(m_theEngine, &DriverModeListLength, m_DrvSys, m_Driver, m_Mode);

	if(DriverModeList == NULL)
	{
		ReportError("[ERROR] No Driver List", false);
		ModeList_Destroy(DriverModeList);
		return false;
	}

	// Sort the List of Drivers
	AutoSelect_SortDriverList(DriverModeList, DriverModeListLength);

	// Pick the first Driver from the List
	if(AutoSelect_PickDriver(m_wndMain, m_theEngine, DriverModeList, DriverModeListLength, &Selection) == GE_FALSE)
	{
		ModeList_Destroy(DriverModeList);
		return false;
	}

	m_Driver	= DriverModeList[Selection].Driver;
	m_Mode		= DriverModeList[Selection].Mode;
	m_nWidth	= DriverModeList[Selection].Width;
	m_nHeight	= DriverModeList[Selection].Height;

	return true;
}

/* ------------------------------------------------------------------------------------ */
// PickDriver
//
// Pop up a pick list to choose the driver from
/* ------------------------------------------------------------------------------------ */
bool CGenesisEngine::PickDriver()
{
	ModeList	*DriverModeList;
	int			DriverModeListLength;
	int			Selection;

	// Create a List of Available Drivers
	DriverModeList = ModeList_Create(m_theEngine, &DriverModeListLength, m_DrvSys, m_Driver, m_Mode);

	if(DriverModeList == NULL)
	{
		ReportError("[ERROR] No Driver List", false);
		ModeList_Destroy(DriverModeList);
		return false;
	}

	// Pop up a Driver List to pick from
	ShowCursor(TRUE);						// Turn on the mouse cursor so we can pick with it

	if(DrvList_PickDriver(m_Instance, m_wndMain, DriverModeList, DriverModeListLength, &Selection) == GE_FALSE)
	{
		ShowCursor(FALSE);					// Turn the mouse cursor back off
		return false;
	}

	m_Driver	= DriverModeList[Selection].Driver;
	m_Mode		= DriverModeList[Selection].Mode;
	m_nWidth	= DriverModeList[Selection].Width;
	m_nHeight	= DriverModeList[Selection].Height;

	ShowCursor(FALSE);						// Turn the mouse cursor back off
	return true;
}

/* ------------------------------------------------------------------------------------ */
// The DrawAlphaBitmap and its associated routines were
// written and provided by Andy Campbell aka Assassin
/* ------------------------------------------------------------------------------------ */
bool CGenesisEngine::DrawAlphaBitmap(geBitmap	*pBitmap,
									 geVec3d	*VertUVArray,
									 geCamera	*ClipCamera,	// if null, uses full screen
									 FloatRect	*PixelRect,		// pixels in the "camera" view
									 FloatRect	*PercentRect,	// percent of the "camera" view
									 geFloat	Alpha,
									 GE_RGBA	*RGBA_Array,
									 float		zdepth)
{
	// set up variables
	GE_TLVertex vertex[4];
	geFloat fUVAdd = 0.0f;
	geFloat fWidthBmp = 0;
	geFloat fHeightBmp = 0;
	geRect ClipRect = {0, 0, 0, 0};
	geRect UseRect = {0, 0, 0, 0};
	geVec3d DefaultUVArray[4] =	{	{0.0f, 0.0f, 0.0f},
									{1.0f, 0.0f, 0.0f},
									{1.0f, 1.0f, 0.0f},
									{0.0f, 1.0f, 0.0f}};
	GE_RGBA DefaultRGBA_Array[4] = {{255, 255, 255, Alpha},
									{255, 255, 255, Alpha},
									{255, 255, 255, Alpha},
									{255, 255, 255, Alpha}};
	geBitmap_Info TempInfo, TempInfo2;

	if(pBitmap)
		geBitmap_GetInfo(pBitmap, &TempInfo, &TempInfo2);
	else
		TempInfo.Height = TempInfo.Width = 8;

	fWidthBmp = static_cast<geFloat>(TempInfo.Width);
	fHeightBmp = static_cast<geFloat>(TempInfo.Height);

	//
	// Clip 2d viewport
	//
	if(!ClipCamera)
	{
		ClipRect.Top	= 0;
		ClipRect.Left	= 0;
		ClipRect.Bottom = m_nHeight - 1;
		ClipRect.Right	= m_nWidth - 1;
	}
	else
	{
		geCamera_GetClippingRect(ClipCamera, &ClipRect);
	}

	if(!VertUVArray)
		VertUVArray = &DefaultUVArray[0];
	if(!RGBA_Array)
		RGBA_Array = &DefaultRGBA_Array[0];

	if(PixelRect)
	{
		UseRect.Top		= static_cast<int32>(PixelRect->Top)	+ ClipRect.Top;
		UseRect.Left	= static_cast<int32>(PixelRect->Left)	+ ClipRect.Left;
		UseRect.Bottom	= static_cast<int32>(PixelRect->Bottom)	+ ClipRect.Top;
		UseRect.Right	= static_cast<int32>(PixelRect->Right)	+ ClipRect.Left;
	}
	else
	{
		if(PercentRect)
		{
			UseRect.Top		= int32(ClipRect.Top	+ 0.01f * PercentRect->Top
								* (ClipRect.Bottom	- ClipRect.Top));
			UseRect.Left	= int32(ClipRect.Left	+ 0.01f * PercentRect->Left
								* (ClipRect.Right	- ClipRect.Left));
			UseRect.Bottom	= int32(ClipRect.Top	+ 0.01f * PercentRect->Bottom
								* (ClipRect.Bottom	- ClipRect.Top));
			UseRect.Right	= int32(ClipRect.Left	+ 0.01f * PercentRect->Right
								* (ClipRect.Right	- ClipRect.Left));
		}
		else
		{
			UseRect = ClipRect;
		}
	}

	geFloat UVbreak = 0.0f;
	vertex[0].x = static_cast<geFloat>(UseRect.Left);
	vertex[0].y = static_cast<geFloat>(UseRect.Top);
	vertex[0].z = zdepth;
	vertex[0].r = RGBA_Array[0].r;
	vertex[0].g = RGBA_Array[0].g;
	vertex[0].b = RGBA_Array[0].b;
	vertex[0].a = RGBA_Array[0].a;
	vertex[0].u = VertUVArray[0].X + UVbreak / fWidthBmp;
	vertex[0].v = VertUVArray[0].Y + UVbreak / fHeightBmp;

	vertex[1].x = static_cast<geFloat>(UseRect.Right);
	vertex[1].y = static_cast<geFloat>(UseRect.Top);
	vertex[1].z = vertex[0].z;
	vertex[1].r = RGBA_Array[1].r;
	vertex[1].g = RGBA_Array[1].g;
	vertex[1].b = RGBA_Array[1].b;
	vertex[1].a = RGBA_Array[1].a;
	vertex[1].u = VertUVArray[1].X - UVbreak / fWidthBmp;
	vertex[1].v = VertUVArray[1].Y + UVbreak / fHeightBmp;

	vertex[2].x = static_cast<geFloat>(UseRect.Right);
	vertex[2].y = static_cast<geFloat>(UseRect.Bottom);
	vertex[2].z = vertex[0].z;
	vertex[2].r = RGBA_Array[2].r;
	vertex[2].g = RGBA_Array[2].g;
	vertex[2].b = RGBA_Array[2].b;
	vertex[2].a = RGBA_Array[2].a;
	vertex[2].u = VertUVArray[2].X - UVbreak / fWidthBmp;
	vertex[2].v = VertUVArray[2].Y - UVbreak / fHeightBmp;

	vertex[3].x = static_cast<geFloat>(UseRect.Left);
	vertex[3].y = static_cast<geFloat>(UseRect.Bottom);
	vertex[3].z = vertex[0].z;
	vertex[3].r = RGBA_Array[3].r;
	vertex[3].g = RGBA_Array[3].g;
	vertex[3].b = RGBA_Array[3].b;
	vertex[3].a = RGBA_Array[3].a;
	vertex[3].u = VertUVArray[3].X + UVbreak / fWidthBmp;
	vertex[3].v = VertUVArray[3].Y - UVbreak / fHeightBmp;

	if(vertex[0].x < ClipRect.Left)
	{
		if(vertex[1].x <= ClipRect.Left)
		{
			return false;
		}

		fUVAdd = (ClipRect.Left - vertex[0].x)/(vertex[1].x - vertex[0].x);
		fUVAdd *= (vertex[1].u - vertex[0].u);
		fWidthBmp -= (ClipRect.Left - vertex[0].x);
		vertex[0].u += fUVAdd;
		vertex[3].u = vertex[0].u;
		vertex[0].x = static_cast<geFloat>(ClipRect.Left);
		vertex[3].x = vertex[0].x;
	}

	if(vertex[0].y < ClipRect.Top)
	{
		if(vertex[2].y <= ClipRect.Top)
		{
			return false;
		}

		fUVAdd = (ClipRect.Top - vertex[0].y)/(vertex[3].y - vertex[0].y);
		fUVAdd *= (vertex[3].v - vertex[0].v);
		fHeightBmp -= (ClipRect.Top - vertex[0].y);
		vertex[0].v += fUVAdd;
		vertex[1].v = vertex[0].v;
		vertex[0].y = static_cast<geFloat>(ClipRect.Top);
		vertex[1].y = vertex[0].y;
	}

	if(vertex[1].x > ClipRect.Right)
	{
		if(vertex[0].x >= ClipRect.Right)
		{
			return false;
		}

		fUVAdd = (vertex[1].x - ClipRect.Right)/(vertex[1].x - vertex[0].x);
		fUVAdd *= (vertex[1].u - vertex[0].u);
		vertex[1].u -= fUVAdd;
		vertex[2].u = vertex[1].u;
		vertex[1].x = static_cast<geFloat>(ClipRect.Right - 1);
		vertex[2].x = vertex[1].x;
	}

	if(vertex[2].y > ClipRect.Bottom)
	{
		if(vertex[0].y >= ClipRect.Bottom )
		{
			return false;
		}

		fUVAdd = (vertex[2].y - ClipRect.Bottom)/(vertex[3].y - vertex[0].y);
		fUVAdd *= (vertex[3].v - vertex[0].v);
		vertex[2].v -= fUVAdd;
		vertex[3].v = vertex[2].v;
		vertex[2].y = static_cast<geFloat>(ClipRect.Bottom - 1);
		vertex[3].y = vertex[2].y;
	}

	geEngine_RenderPoly(m_theEngine,vertex,
		4, pBitmap, (Alpha != 255 ? DRV_RENDER_ALPHA : 0 ) |
		DRV_RENDER_CLAMP_UV | DRV_RENDER_NO_ZMASK | DRV_RENDER_NO_ZWRITE | DRV_RENDER_FLUSH);

	return true;
}

/* ------------------------------------------------------------------------------------ */
// DrawAlphaBitmapRect
/* ------------------------------------------------------------------------------------ */
bool CGenesisEngine::DrawAlphaBitmapRect(geBitmap	*pBitmap,
										 geRect		*BitmapRect,
										 geCamera	*ClipCamera,
										 FloatRect	*PixelRect,
										 FloatRect	*PercentRect,
										 geFloat	Alpha,
										 GE_RGBA	*RGBA_Array,
										 float		zdepth)
{
	geVec3d UVArray[4];
	geBitmap_Info TempInfo, TempInfo2;
	geRect DefaultRect;

	if(pBitmap)
		geBitmap_GetInfo(pBitmap, &TempInfo, &TempInfo2);
	else
		TempInfo.Height = TempInfo.Width = 8;

	if(!BitmapRect)
	{
		BitmapRect = &DefaultRect;
		BitmapRect->Top = 0;
		BitmapRect->Left = 0;
		BitmapRect->Right = (TempInfo.Width - 1);
		BitmapRect->Bottom = (TempInfo.Height - 1);
	}

	UVArray[0].X = static_cast<geFloat>(BitmapRect->Left  ) / static_cast<geFloat>(TempInfo.Width  - 1);
	UVArray[0].Y = static_cast<geFloat>(BitmapRect->Top   ) / static_cast<geFloat>(TempInfo.Height - 1);
	UVArray[1].X = static_cast<geFloat>(BitmapRect->Right ) / static_cast<geFloat>(TempInfo.Width  - 1);
	UVArray[1].Y = static_cast<geFloat>(BitmapRect->Top   ) / static_cast<geFloat>(TempInfo.Height - 1);
	UVArray[2].X = static_cast<geFloat>(BitmapRect->Right ) / static_cast<geFloat>(TempInfo.Width  - 1);
	UVArray[2].Y = static_cast<geFloat>(BitmapRect->Bottom) / static_cast<geFloat>(TempInfo.Height - 1);
	UVArray[3].X = static_cast<geFloat>(BitmapRect->Left  ) / static_cast<geFloat>(TempInfo.Width  - 1);
	UVArray[3].Y = static_cast<geFloat>(BitmapRect->Bottom) / static_cast<geFloat>(TempInfo.Height - 1);

	return DrawAlphaBitmap(pBitmap, UVArray, ClipCamera, PixelRect, PercentRect, Alpha, RGBA_Array, zdepth);
}

/* ------------------------------------------------------------------------------------ */
// takes Bitmap
// returns BitmapBuffer, Width and Height
/* ------------------------------------------------------------------------------------ */
bool CGenesisEngine::BreakUpBigBitmap(geBitmap			*pBitmap,
									  IncompleteTexture	*&BitmapBuffer,
									  int				&NumWide,
									  int				&NumHigh)
{
	int WidthBmp, HeightBmp;
	gePixelFormat PFormat;
	int BitmapsWide, BitmapsHigh;
	int i, j;

	if(!pBitmap)
	{
		ReportError("[ERROR] BreakUpBigBitmap needs a bitmap", false);
		return false;
	}

	if(!BitmapBuffer)
	{
		ReportError("[ERROR] BreakUpBigBitmap needs a bitmapbuffer", false);
		return false;
	}

	{
		geBitmap_Info TempInfo, TempInfo2;
		geBitmap_GetInfo(pBitmap, &TempInfo, &TempInfo2);
		WidthBmp = TempInfo.Width;
		HeightBmp = TempInfo.Height;
		PFormat = TempInfo.Format;
	}

	NumWide = BitmapsWide = (WidthBmp-1) / 256 + 1;
	NumHigh = BitmapsHigh = (HeightBmp-1) / 256 + 1;
	BitmapBuffer = new IncompleteTexture[BitmapsHigh * BitmapsWide];
	int Width = 0, Height = 0;
	geBitmap *Bitmap = NULL;

	for(i=0; i<BitmapsHigh; ++i)
	{
		for(j=0; j<BitmapsWide; ++j)
		{
			if(j < BitmapsWide-1)
				Width = 256;
			else
				Width = (WidthBmp-1) % 256 + 1;

			if(i < BitmapsHigh-1)
				Height = 256;
			else
				Height = (HeightBmp-1) % 256 + 1;

			Bitmap = geBitmap_Create(256, 256, 1, PFormat);

			if(!Bitmap)
			{
				ReportError("[ERROR] BreakUpBigBitmap could not create a bitmap", false);
				delete BitmapBuffer;
				// MEMORY LEAK
				return false;
			}

			if(!geBitmap_Blit(pBitmap, j<<8, i<<8,/* j*256, i*256,*/ Bitmap, 0, 0, Width, Height))
			{
				ReportError("[ERROR] BreakUpBigBitmap could not blit a bitmap", false);
				delete BitmapBuffer;
				geBitmap_Destroy(&pBitmap);
				// MEMORY LEAK
				return false;
			}

			if(!geBitmap_SetPreferredFormat(Bitmap, GE_PIXELFORMAT_8BIT))
			{
				ReportError("[ERROR] BreakUpBigBitmap could not change formats", false);
				delete BitmapBuffer;
				geBitmap_Destroy(&pBitmap);
				// MEMORY LEAK
				return false;
			}

			BitmapBuffer[j + i*BitmapsWide].Width = Width;
			BitmapBuffer[j + i*BitmapsWide].Height = Height;
			BitmapBuffer[j + i*BitmapsWide].Bitmap = Bitmap;
		}
	}

	return true;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
CompleteTexture CGenesisEngine::BuildCompleteTexture(IncompleteTexture *BitmapBuffer,
													 int NumWide, int NumHigh)
{
	CompleteTexture ret;

	ret.TextureArray	= BitmapBuffer;
	ret.TexturesHigh	= NumHigh;
	ret.TexturesWide	= NumWide;
	ret.TotalHeight		= BitmapBuffer[NumHigh*NumWide].Height	+ (NumHigh-1) * 256;
	ret.TotalWidth		= BitmapBuffer[NumHigh*NumWide].Width	+ (NumWide-1) * 256;

	return ret;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
bool CGenesisEngine::AddCompleteTextureToWorld(const CompleteTexture &cp)
{
	int i, bmpcount;
	bool retval = true;

	bmpcount = cp.TexturesHigh * cp.TexturesWide;

	for(i=0; i<bmpcount; ++i)
	{
		if(!geWorld_AddBitmap(CCD->World(), cp.TextureArray[i].Bitmap))
			retval = false;
	}

	return retval;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
void CGenesisEngine::DeleteCompleteTexture(CompleteTexture cp)
{
	int i, bmpcount;
	bool retval = true;

	bmpcount = cp.TexturesHigh * cp.TexturesWide;

	for(i=0; i<bmpcount; ++i)
	{
		geWorld_RemoveBitmap(CCD->World(), cp.TextureArray[i].Bitmap);
		geBitmap_Destroy(&(cp.TextureArray[i].Bitmap));
	}

	delete cp.TextureArray;
	cp.TextureArray = NULL;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
CompleteTexture CGenesisEngine::BitmapToComplete(geBitmap *pBitmap)
{
	IncompleteTexture *BitmapBuffer;
	int NumWide, NumHigh;
	CompleteTexture cp;

	BreakUpBigBitmap(pBitmap, BitmapBuffer, NumWide, NumHigh);
	cp = BuildCompleteTexture(BitmapBuffer, NumWide, NumHigh);
	AddCompleteTextureToWorld(cp);

	cp.TotalWidth = NumWide * 256;
	cp.TotalHeight = NumHigh * 256;

	return cp;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
void CGenesisEngine::DrawComplete(const CompleteTexture &cp, int x, int y)
{
	FloatRect ScreenRect;

	ScreenRect.Left		= static_cast<float>(x);
	ScreenRect.Top		= static_cast<float>(y);
	ScreenRect.Right	= ScreenRect.Left + cp.TotalWidth;
	ScreenRect.Bottom	= ScreenRect.Top + cp.TotalHeight;

	DrawCompleteTexture(cp, CCD->CameraManager()->Camera(),
						 &ScreenRect,
						 NULL, 255.0f, NULL);
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
bool CGenesisEngine::DrawCompleteTexture(const CompleteTexture	&cp,
										 geCamera			*ClipCamera,
										 FloatRect			*ScreenRect,
										 FloatRect			*PercentRect,
										 geFloat			Alpha,
										 GE_RGBA			* /*RGBA_Array*/) // ignored currently
{
	bool retval = true;
	int i, x, wide, high;
	geFloat Top, Bottom, Left, Right;
	FloatRect RealScreenRect;
	FloatRect UseScreenRect;
	geRect BitmapRect;
	bool RectInPercent = true;

	if(ScreenRect == NULL && PercentRect != NULL)
		RectInPercent = true;

	if(RectInPercent && PercentRect != NULL)
	{
		RealScreenRect = *PercentRect;
	}
	else if(ScreenRect != NULL)
	{
		RealScreenRect = *ScreenRect;
		RectInPercent = false;
	}
	else
	{
		RealScreenRect.Top = 0;
		RealScreenRect.Bottom = 100;
		RealScreenRect.Left = 0;
		RealScreenRect.Right = 100;
		RectInPercent = true;
	}

	Top = RealScreenRect.Top;
	Bottom = RealScreenRect.Bottom;
	Left = RealScreenRect.Left;
	Right = RealScreenRect.Right;
	wide = cp.TexturesWide;
	high = cp.TexturesHigh;
	i = wide * high;

	for(x=0; x<i; ++x)
	{
		geBitmap *bmp = cp.TextureArray[x].Bitmap;

		BitmapRect.Left		= 0;
		BitmapRect.Top		= 0;
		BitmapRect.Right	= cp.TextureArray[x].Width;
		BitmapRect.Bottom	= cp.TextureArray[x].Height;

		if(RectInPercent)
		{
			UseScreenRect.Top    = (((Bottom-Top)/high)*(x/wide)) + Top;
			UseScreenRect.Bottom = (((Bottom-Top)/high)*(x/wide)  + Top + ((Bottom-Top)/high));
			UseScreenRect.Left   = (((Right-Left)/wide)*(x%wide)) + Left;
			UseScreenRect.Right  = (((Right-Left)/wide)*(x%wide)  + Left + ((Right-Left)/wide));

			if(!DrawAlphaBitmapRect(bmp, &BitmapRect, ClipCamera, NULL, &UseScreenRect, Alpha, NULL, 1.0f))
				retval = false;
		}
		else
		{
			if(high > 1)
			{
				UseScreenRect.Top = ((Bottom-Top)/high)*(x/wide) + Top;
				UseScreenRect.Bottom = ((Bottom-Top)/high)*(x/wide) + Top +
					cp.TextureArray[x].Height*((Bottom-Top)/(high*256.0f));
			}
			else
			{
				UseScreenRect.Top = RealScreenRect.Top;
				UseScreenRect.Bottom = RealScreenRect.Bottom;
			}

			if(wide > 1)
			{
				UseScreenRect.Left = ((Right-Left)/wide)*(x%wide) + Left;
				UseScreenRect.Right = ((Right-Left)/wide)*(x%wide) + Left +
					cp.TextureArray[x].Width*((Right-Left)/(wide*256.0f));
			}
			else
			{
				UseScreenRect.Left = RealScreenRect.Left;
				UseScreenRect.Right = RealScreenRect.Right;
			}

			if(!DrawAlphaBitmapRect(bmp, &BitmapRect, ClipCamera, &UseScreenRect, NULL, Alpha, NULL, 1.0f))
				retval = false;
		}
	}

	return retval;
}
/* ------------------------------------------------------------------------------------ */
// End of DrawAlphaBitmap routines
/* ------------------------------------------------------------------------------------ */


/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
bool CGenesisEngine::DrawBitmap(geBitmap *pBitmap, geRect *BitmapRect, int x, int y, float zdepth)
{
	FloatRect PixelRect;
	bool ret;
	geRect Rect;

	PixelRect.Top = static_cast<float>(y);
	PixelRect.Left = static_cast<float>(x);

	if(BitmapRect)
	{
		PixelRect.Bottom = PixelRect.Top  + static_cast<float>(BitmapRect->Bottom - BitmapRect->Top);
		PixelRect.Right  = PixelRect.Left + static_cast<float>(BitmapRect->Right - BitmapRect->Left);

		ret = DrawAlphaBitmapRect(pBitmap, BitmapRect,
									CCD->CameraManager()->Camera(), &PixelRect,
									NULL, 255.0f, NULL, zdepth);
	}
	else
	{
		Rect.Top	= 0;
		Rect.Left	= 0;
		Rect.Bottom = geBitmap_Height(pBitmap) - 1;
		Rect.Right	= geBitmap_Width(pBitmap) - 1;

		PixelRect.Bottom = PixelRect.Top  + static_cast<float>(Rect.Bottom);
		PixelRect.Right	 = PixelRect.Left + static_cast<float>(Rect.Right);

		ret = DrawAlphaBitmapRect(pBitmap, &Rect,
									CCD->CameraManager()->Camera(), &PixelRect,
									NULL, 255.0f, NULL, zdepth);
	}

	return ret;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
bool CGenesisEngine::DrawBitmap(geBitmap *pBitmap, geRect *BitmapRect, int x, int y, float Alpha, float zdepth)
{
	FloatRect PixelRect;
	bool ret;
	geRect Rect;

	PixelRect.Top	= static_cast<float>(y);
	PixelRect.Left	= static_cast<float>(x);

	if(BitmapRect)
	{
		PixelRect.Bottom = PixelRect.Top  + static_cast<float>(BitmapRect->Bottom - BitmapRect->Top);
		PixelRect.Right  = PixelRect.Left + static_cast<float>(BitmapRect->Right - BitmapRect->Left);

		ret = DrawAlphaBitmapRect(pBitmap, BitmapRect,
									CCD->CameraManager()->Camera(), &PixelRect,
									NULL, Alpha, NULL, zdepth);
	}
	else
	{
		Rect.Top	= 0;
		Rect.Left	= 0;
		Rect.Bottom = geBitmap_Height(pBitmap)-1;
		Rect.Right	= geBitmap_Width(pBitmap)-1;

		PixelRect.Bottom = PixelRect.Top  + static_cast<float>(Rect.Bottom);
		PixelRect.Right  = PixelRect.Left + static_cast<float>(Rect.Right);

		ret = DrawAlphaBitmapRect(pBitmap, &Rect,
									CCD->CameraManager()->Camera(), &PixelRect,
									NULL, Alpha, NULL, zdepth);
	}

	return ret;
}

/* ------------------------------------------------------------------------------------ */
// ShowFrameRate
//
// Turn frame rate display on/off
/* ------------------------------------------------------------------------------------ */
void CGenesisEngine::ShowFrameRate(bool bHow)
{
	geEngine_EnableFrameRateCounter(m_theEngine, bHow);
}

/* ------------------------------------------------------------------------------------ */
// LoadLevel
//
// Load a level file into the current engine.
/* ------------------------------------------------------------------------------------ */
bool CGenesisEngine::LoadLevel(const char *szLevelFileName)
{
	// Check to see if we have a level loaded, and if so, gun it!
	if(m_World != NULL)
	{
		geEngine_RemoveWorld(m_theEngine, m_World);
		geWorld_Free(m_World);
		m_World = NULL;
	}

	// Ok, try to open up the desired level file
	CCD->OpenRFFile(&m_Level, kLevelFile, szLevelFileName, GE_VFILE_OPEN_READONLY);

	if(!m_Level)
	{
		char szBug[256];
		sprintf(szBug, "[ERROR] File %s - Line %d: Failed to load level '%s'",
				__FILE__, __LINE__, szLevelFileName);
		ReportError(szBug, false);
		return FALSE;
	}

	// Level open, now try to load it into Genesis3D
	m_World = geWorld_Create(m_Level);
	geVFile_Close(m_Level);

	if(!m_World)
	{
		char szBug[256];
		sprintf(szBug, "[ERROR] File %s - Line %d: Loaded level '%s' failed to create World",
				__FILE__, __LINE__, szLevelFileName);
		ReportError(szBug, false);
		return FALSE;
	}

	if(geEngine_AddWorld(m_theEngine, m_World) == GE_FALSE)
	{
		char szBug[256];
		sprintf(szBug, "[ERROR] File %s - Line %d: Loaded level '%s' failed to add World",
				__FILE__, __LINE__, szLevelFileName);
		ReportError(szBug, false);
		return FALSE;
	}

	strcpy(m_CurrentLevel, szLevelFileName);		// Save level name

	// Level loaded and locked into the engine.
	return TRUE;
}

/* ------------------------------------------------------------------------------------ */
// BeginFrame
//
// Set up the engine to prepare for entities, etc. to be rendered into it.
/* ------------------------------------------------------------------------------------ */
int CGenesisEngine::BeginFrame()
{
	if(m_fInFramePass)
		return RGF_FAILURE;					// Already in a BeginFrame()/EndFrame() block

	if(geEngine_BeginFrame(m_theEngine, CCD->CameraManager()->Camera(), GE_TRUE) == GE_FALSE)
	{
		ReportError("[ERROR] CGenesisEngine::BeginFrame failed", false);
		exit(-1);
		return RGF_FAILURE;
	}

	m_fInFramePass = true;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// EndFrame
//
// All rendering is done, now clean up and flip or blit the results
// ..into the main window.
/* ------------------------------------------------------------------------------------ */
int CGenesisEngine::EndFrame()
{
	if(!m_fInFramePass)
		return RGF_FAILURE;				// Not in BeginFrame()/EndFrame() block

	if(geEngine_EndFrame(m_theEngine) == GE_FALSE)
	{
		ReportError("[ERROR] CGenesisEngine::EndFrame failed", false);
		return RGF_FAILURE;
	}

	m_fInFramePass = false;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// RenderWorld
//
// Render the BSP-based world the Genesis engine is running.
/* ------------------------------------------------------------------------------------ */
int CGenesisEngine::RenderWorld()
{
	if(!m_fInFramePass)
		return RGF_FAILURE;				// Not in BeginFrame()/EndFrame() block

	// Update the camera position for bindings, etc.
	CCD->CameraManager()->TrackMotion();
	CCD->CameraManager()->RenderView();

	// We're ready, render it!
	if(geEngine_RenderWorld(m_theEngine, m_World, CCD->CameraManager()->Camera(), 0.00f) == GE_FALSE)
	{
		ReportError("[ERROR] CGenesisEngine::RenderWorld failed", false);
		return RGF_FAILURE;
	}

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// DisplaySplash
//
// Throw a bitmap up on the screen as a splash screen.  This is used for
// ..displaying "Loading" bitmaps, intro screens, credit screens, etc.
// ..Note that the next render cycle will over-write what's on the screen.
/* ------------------------------------------------------------------------------------ */
int CGenesisEngine::DisplaySplash(const char *szSplashBMP, const char *szAudioFile)
{
	geBitmap *theBmp = CreateFromFileName(szSplashBMP);
	geBitmap_Info BmpInfo;

	if(theBmp != NULL)
	{
		if(geBitmap_GetInfo(theBmp, &BmpInfo, NULL) == GE_TRUE)
		{
			int x = (m_nWidth - BmpInfo.Width) / 2;
			int y = (m_nHeight - BmpInfo.Height) / 2;

			if(geEngine_AddBitmap(m_theEngine, theBmp) == GE_FALSE)
			{
				char szError[200];
				sprintf(szError, "[ERROR] File %s - Line %d: DisplaySplash: AddBitmap failed on '%s'\n",
						__FILE__, __LINE__, szSplashBMP);
				ReportError(szError, false);
				return RGF_FAILURE;
			}

			if(m_fInFramePass)
				EndFrame();

			geEngine_BeginFrame(m_theEngine, CCD->CameraManager()->Camera(), GE_TRUE);
			m_fInFramePass = true;

			if(geEngine_DrawBitmap(m_theEngine, theBmp, NULL, x, y ) == GE_FALSE)
			{
				char szError[200];
				sprintf(szError,"[WARNING] File %s - Line %d: DisplaySplash: DrawBitmap failed on '%s'\n",
						__FILE__, __LINE__, szSplashBMP);
				ReportError(szError, false);
			}

			EndFrame();
		}

		// Ok, we've got the splash up, load and play the audio effect
		if(m_SplashAudio != NULL)
		{
			// Drop previous sound effect
			geSound_FreeSoundDef(m_Audio, m_SplashAudio);
			m_SplashAudio = NULL;
		}

		if(szAudioFile && (strlen(szAudioFile) > 0))
		{
			// Load and play some splash-screen audio
			geVFile *SoundFile;
			CCD->OpenRFFile(&SoundFile, kAudioFile, szAudioFile, GE_VFILE_OPEN_READONLY);

			if(!SoundFile)
			{
				char szError[256];
				sprintf(szError, "[WARNING] File %s - Line %d: Failed to open Splash Audio file '%s'\n",
						__FILE__, __LINE__, szAudioFile);
				ReportError(szError, false);
			}
			else
			{
				m_SplashAudio = geSound_LoadSoundDef(m_Audio, SoundFile);
				geVFile_Close(SoundFile);
				geSound_PlaySoundDef(m_Audio, m_SplashAudio, 1.0f, 0, 0, GE_FALSE);
			}
		}

		geEngine_RemoveBitmap(m_theEngine, theBmp);
		geBitmap_Destroy(&theBmp);
	}

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SetFogParameters
//
// Set up parameters to be used for distance fogging
/* ------------------------------------------------------------------------------------ */
int CGenesisEngine::SetFogParameters(GE_RGBA theColor, geFloat fStart, geFloat fEnd)
{
	FogColor = theColor;
	fFogStart = fStart;
	fFogEnd = fEnd;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// EnableFog
//
// Turn distance fogging on and off
/* ------------------------------------------------------------------------------------ */
void CGenesisEngine::EnableFog(geBoolean FogOn)
{
	geEngine_SetFogEnable(m_theEngine, FogOn,
		FogColor.r, FogColor.g, FogColor.b,
		fFogStart, fFogEnd);
}

/* ------------------------------------------------------------------------------------ */
//	ReportError
//
//	Log an error message to the RGF log file.  If bMessageBoxIt is
//	..TRUE, the user gets a Windows message box containing the
//	..offending message.
/* ------------------------------------------------------------------------------------ */
bool CGenesisEngine::ReportError(const char *szError, bool bMessageBoxIt)
{
	FILE *fd;

	if(m_DebugEnabled == false)
		return true;										// DEBUG disabled, ignore call

	int OldDebugLevel = CCD->SetDebugLevel(kNoDebugOutput);

	if((fd = CCD->OpenRFFile(kRawFile, ".\\RealityFactory.log", "at")) == NULL)
	{
		OutputDebugString("Error logging failure!\n");

		if(bMessageBoxIt)
			MessageBox(NULL, "Error Logging Failure!", "RGF Error Report",
			MB_ICONEXCLAMATION | MB_OK);

		CCD->SetDebugLevel(OldDebugLevel);
		return false;						// Seriously bad - error not logged
	}

	struct tm *newtime;
	time_t aclock;

	// Log date and time of error, as well
	time(&aclock);
	newtime = localtime(&aclock);

	// fputs(asctime(newtime), fd);
	fputs(szError, fd); fputs("\n", fd);

	fclose(fd);

	// Ok, out to a file, now to be sure we can track it in the debugger

	// OutputDebugString(asctime(newtime));
	OutputDebugString(szError);
	OutputDebugString("\n");

	if(bMessageBoxIt)
		MessageBox(NULL, szError, "Game Error", MB_ICONEXCLAMATION | MB_OK);

	CCD->SetDebugLevel(OldDebugLevel);

	return true;							// Error logged
}

/* ------------------------------------------------------------------------------------ */
// SaveTo
//
// Save off the current level name to the supplied file
/* ------------------------------------------------------------------------------------ */
int CGenesisEngine::SaveTo(FILE *SaveFD)
{
	fwrite(&m_CurrentLevel, 1, 256, SaveFD);

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// RestoreFrom
//
// Restore the current level name from the supplied file.  Will
// ..automatically load the level.
/* ------------------------------------------------------------------------------------ */
int CGenesisEngine::RestoreFrom(FILE *RestoreFD)
{
	fread(&m_CurrentLevel, 1, 256, RestoreFD);

	return RGF_SUCCESS;			// And load the level
}

/* ------------------------------------------------------------------------------------ */
//	ResetSystem
/* ------------------------------------------------------------------------------------ */
void CGenesisEngine::ResetSystem()
{
	CCD->ResetClock();
	BeginFrame();
	GE_Rect Rect;
	GE_RGBA	Color;

	Rect.Left = Rect.Top = 0;
	Rect.Right = Width() - 1;
	Rect.Bottom = Height() - 1;

	Color.r = 0.0f;
	Color.g = 0.0f;
	Color.b = 0.0f;
	Color.a = 255;

	geEngine_FillRect(Engine(), &Rect, &Color);
	EndFrame();

	CCD->ResetClock();
}

/* ----------------------------------- END OF FILE ------------------------------------ */
