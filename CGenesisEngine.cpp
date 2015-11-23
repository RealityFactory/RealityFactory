/*
CGenesisEngine.cpp:		Genesis3D engine encapsulation

  (c) 2001 Ralph Deane
  
	This file contains the class implementation for the Genesis3D
	engine wrapper.  This wrapper hides many of the gory details
	necessary to use the Genesis3D 1.0 engine.
*/

#include "RabidFramework.h"

//Dee 07-07-00
#include "AutoSelect.h"
// End Dee

//	This is the window procedure that will be used by the main
//	..engine wrapper.  I wish there was some way to do without
//	..it, I'll have to research it.

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	// start multiplayer
	if (CCD->NetPlayerManager()!=NULL)
    {
	    switch (iMessage) 
	    {
		    case WM_TIMER:	
		    	CCD->NetPlayerManager()->ServerClientCycle();
        }
	}
// end multiplayer
/*	if(iMessage==WM_CHAR)
	{
		if(CCD->GetConsole())
		{
			char chCharCode = (TCHAR) wParam;
			int key = (lParam>>16) & 255;

			char szBug[255];
			sprintf(szBug,"%x", key);
			CCD->ReportError(szBug, false);
		}
	} */
	return DefWindowProc(hWnd, iMessage, wParam, lParam);
}

//	CGenesisEngine
//
//	Construct a main window and initialize the engine, also start up the
//	..Genesis sound system.

CGenesisEngine::CGenesisEngine(bool fFullScreen, int nWidth, int nHeight,
							   char *szName, HINSTANCE theInstance, char chDriverID, bool bSoftware,
							   bool UseDialog, char *szStartLevel)
{
	WNDCLASS wc;
	HWND hWnd;
	
	unlink(".\\RealityFactory.log");			// Gun old log file
	
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_fFullScreen = fFullScreen;
	m_MasterVolume = 0;
	m_SplashAudio = NULL;
	m_World = NULL;
	m_bUseSoftware = bSoftware;
	m_CurrentLevel[0] = 0;
	m_SelectedDriverID = chDriverID;			// Use the desired driver
	fFogStart = fFogEnd = 0.0f;						// Fog start, end distances
	
	GetWindowRect(GetDesktopWindow(), &m_ScreenRect);
	
	// Set up and register application window class
	
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = theInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = (const char*)NULL;
	wc.lpszClassName = szName;
	RegisterClass(&wc);
	
	// Create the main window
	
	hWnd = CreateWindowEx(0, szName, szName, WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT, 
		nWidth, nHeight, NULL, NULL, theInstance, NULL);
	
	if(!hWnd)
	{
		OutputDebugString("Main window creation failure\n");
		ReportError("Main window creation failure", false);
		exit(-1);
	} 
	
	UpdateWindow(hWnd);
	
	SetWindowPos (hWnd, HWND_TOP, (((m_ScreenRect.right + m_ScreenRect.left) / 2) - (nWidth / 2)),
		(((m_ScreenRect.bottom + m_ScreenRect.top) / 2) - (nHeight / 2)),
		nWidth, nHeight, SWP_NOCOPYBITS | SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);
	
	m_wndMain = hWnd;						// Save for later use
	
	if(UseDialog)
	{
		char m_currentdir[512];
		_getcwd(m_currentdir, 512);
		ShowCursor(TRUE);
		CFileDialog dlg(TRUE, "bsp", NULL, OFN_HIDEREADONLY,
			"Level BSP Files (*.bsp)|*.bsp|All Files (*.*)|*.*||");
		
		dlg.m_ofn.lpstrTitle = "Load Level";
		TCHAR m_dir[512];
		strcpy(m_dir, m_currentdir);
		strcat(m_dir, "\\media\\levels");
		dlg.m_ofn.lpstrInitialDir = m_dir;
		if (dlg.DoModal() == IDOK) 
		{
			CString FileName = dlg.GetFileName();
			strcpy(szStartLevel, FileName);
			CString PathName = dlg.GetPathName();
			PathName = PathName.Left(PathName.ReverseFind('\\'));
			TCHAR m_lev[512];
			strcpy(m_lev, PathName);
			CCD->SetLevelDirectory(m_lev);
		}
		chdir(m_currentdir);
		
		ShowCursor(FALSE);
	}
	
	///////////////////////////////
	//Dee
	m_Instance = theInstance;			//Save for later
	//End Dee
	///////////////////////////////
	
	//	Ok, now create the engine and let 'er rip!
	
	if(CreateEngine(szName) == FALSE)
	{
		DestroyWindow(m_wndMain);					// Clean up
		ReportError("Genesis3D engine creation failure", false);
		exit(-2);
	}
	
	m_DebugEnabled = false;							// No debug by default
	m_bInFramePass = false;							// Not in frame rendering
	
	//	Now set up the sound system.
	
	m_Audio = geSound_CreateSoundSystem(m_wndMain);
	if(m_Audio == NULL)
	{
		DestroyWindow(m_wndMain);
		ReportError("Genesis3D audio subsystem creation failure", false);
		exit(-86);
	}
	
	geSound_SetMasterVolume(m_Audio, 0.99f);			// 100% audio for now.
	
	
	return;
}

//	~CGenesisEngine
//
//	Shut down the Genesis3D engine

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
	
	return;
}

//	CreateEngine
//
//	Set up the Genesis3D engine and attach it to the main window, which
//	..must exist prior to this member function being called.

bool CGenesisEngine::CreateEngine(char *szName)
{
	
	m_World = NULL;
	
	
	// Create the genesis engine
	
	m_theEngine = geEngine_Create(m_wndMain, szName, ".");
	if(!m_theEngine)
	{
		ReportError("geEngine_Create failure", false);
		return FALSE;
	}
	
	// Turn off the framerate counter
	
	geEngine_EnableFrameRateCounter(m_theEngine, GE_FALSE);
	
	// Create the genesis driver system
	
	m_DrvSys = geEngine_GetDriverSystem(m_theEngine);

	if(!m_DrvSys)
	{
		ReportError("geEngine_GetDriverSystem failure", false);
		return FALSE;
	}

// changed RF063
	geDriver *gDriver;
	const char *drvname = NULL;

	Voodoo = false;

	gDriver = geDriver_SystemGetNextDriver(m_DrvSys, NULL);
	if(gDriver)
	{
		while(1) 
		{
			geDriver_GetName(gDriver, &drvname);
			if(drvname[0] == 'G') 
			{
				Voodoo = true;
				break;		
			}
			gDriver = geDriver_SystemGetNextDriver(m_DrvSys, gDriver);
			if(!gDriver)
				break;
		}
	}

	if(Voodoo)
	{
		FILE *fd = CCD->OpenRFFile(kRawFile, "D3D24.ini", "wb");
		fputs("16", fd); fputs("\n", fd);
		fputs("16", fd); fputs("\n", fd);
		fclose(fd);
	}
	else
	{
		//FILE *fd = CCD->OpenRFFile(kRawFile, "D3D24.ini", "wb");
		//fputs("16", fd); fputs("\n", fd); // 32
		//fputs("16", fd); fputs("\n", fd); // 24
		//fclose(fd);
	}
// end change RF063

	/////////////////////////////////////////////////////////////////
	// Dee  07/07/00 - Added
	// Find any available driver and use the first mode it has
	/////////////////////////////////////////////////////////////////
	if(m_SelectedDriverID == 'A')
	{
		if(AutoDriver() == FALSE)
		{
			ReportError("Failed to locate any available Genesis3D driver", false);
			return FALSE;
		}
	}
	/////////////////////////////////////////////////////////////////
	// End Dee
	/////////////////////////////////////////////////////////////////
	
	/////////////////////////////////////////////////////////////////
	// Dee  12/07/00 - Added
	// Pop a Pick List to choose the mode you'd like (out of what's available)
	/////////////////////////////////////////////////////////////////
	else if(m_SelectedDriverID == 'P')
	{
		if(PickDriver() == FALSE)
		{
			//Probably hit "Pick Something for Me" or ESC...
			ReportError("Failed to successfully pick a Genesis3D driver", false);
			//So try to auto select for them
			ReportError("Auto-Detecting", false);
			if(AutoDriver() == FALSE)
			{
				ReportError("Failed to locate any available Genesis3D driver", false);
				return FALSE;
			}
		}
		//Dee 12-07-00
		//Added to support fullscreen or windowed driver change via pick list
		else
		{
			geEngine_ShutdownDriver(m_theEngine);
			ResetMainWindow(m_wndMain, m_nWidth, m_nHeight);
			
			if(!geEngine_SetDriverAndMode(m_theEngine, m_Driver, m_Mode)) 
			{
				ReportError("geEngine_SetDriverAndMode failure", false);
				return FALSE;
			}
		}
		//End Dee
		
	}
	/////////////////////////////////////////////////////////////////
	// End Dee
	/////////////////////////////////////////////////////////////////
	
	/////////////////////////////////////////////////////////////////
	// Dee 07/07/00 - now part of above if(m_Selected...) statement
	// Find ourselves a good driver
	/////////////////////////////////////////////////////////////////
	else
	{
		if(FindDriver() == FALSE)
		{
			ReportError("Failed to locate good Genesis3D driver", false);
			return FALSE;
		}
		
		// let the genesis engine intialize with our driver
		
		if(!geEngine_SetDriverAndMode(m_theEngine, m_Driver, m_Mode)) 
		{
			ReportError("geEngine_SetDriverAndMode failure", false);
			return FALSE;
		}
	}
	/////////////////////////////////////////////////////////////////
	// End Dee
	/////////////////////////////////////////////////////////////////
	
	
	//	Holy toledo, it all worked!  Back to the caller, then.
	
	return TRUE;
}

//	FindDriver
//
//	Locate a driver that'll do what we want, how we want it.

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
			ReportError("geDriver_SystemGetNextDriver(FULLSCREEN) failure", false);
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
				ReportError("Unexpected SystemGetNextDriver error", false);
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
				sprintf(szFoo,"Driver %s width %d height %d selected", drvname,
					Width, Height);
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
			ReportError("geDriver_SystemGetNextDriver(FULLSCREEN) failure", false);
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
				ReportError("Unexpected SystemGetNextDriver error", false);
				return FALSE;
			}
		}
		m_Mode = geDriver_GetNextMode(m_Driver, NULL);
		const char *pModeName;
		while(1) 
		{ 
			if(!m_Mode)
			{
				CCD->ReportError("GetNextMode - *No Mode* failure", false);
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
			ReportError("Couldn't find windowed driver and mode", false);
			return FALSE;
		}
	}
	
	//	Ok, driver AND mode found that we're happy with, bail this.
	
	return TRUE;
}


/////////////////////////////////////////////////////////////////
// Dee  12/07/00
// Added to select any available driver and use it's first mode
/////////////////////////////////////////////////////////////////
bool CGenesisEngine::AutoDriver()
{
	
	ModeList			   *DriverModeList;
	int						DriverModeListLength;
	int						Selection = 0;
	
	// Create a List of Available Drivers
	DriverModeList = ModeList_Create(m_theEngine, &DriverModeListLength, m_DrvSys, m_Driver, m_Mode);
	if (DriverModeList == NULL)
	{
		ReportError("No Driver List", false);
		ModeList_Destroy(DriverModeList);
		return FALSE;
	}
	// Sort the List of Drivers
	AutoSelect_SortDriverList(DriverModeList, DriverModeListLength);
	//Pick the first Driver from the List
	if(AutoSelect_PickDriver(m_wndMain, m_theEngine, DriverModeList, DriverModeListLength, &Selection)==FALSE)
	{
		ModeList_Destroy(DriverModeList);
		return FALSE;
	}
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////
// End Dee
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
// Dee  12/07/00
// Added to pop up a pick list to choose the driver from
/////////////////////////////////////////////////////////////////
bool CGenesisEngine::PickDriver()
{
	
	ModeList			   *DriverModeList;
	int						DriverModeListLength;
	int						Selection;
	
	// Create a List of Available Drivers
	DriverModeList = ModeList_Create(m_theEngine, &DriverModeListLength, m_DrvSys, m_Driver, m_Mode);
	if (DriverModeList == NULL)
	{
		ReportError("No Driver List", false);
		ModeList_Destroy(DriverModeList);
		return FALSE;
	}
	
	//Pop up a Driver List to pick from
	ShowCursor(TRUE);						// Turn on the mouse cursor so we can pick with it
	if(DrvList_PickDriver(m_Instance, m_wndMain, DriverModeList, DriverModeListLength, &Selection)==FALSE)
	{
		ShowCursor(FALSE);						// Turn the mouse cursor back off
		return FALSE;
	}
	m_Driver=DriverModeList[Selection].Driver;
	m_Mode=DriverModeList[Selection].Mode;
	m_nWidth=DriverModeList[Selection].Width;
	m_nHeight=DriverModeList[Selection].Height;

	ShowCursor(FALSE);						// Turn the mouse cursor back off
    return TRUE;
}

/////////////////////////////////////////////////////////////////
// End Dee
/////////////////////////////////////////////////////////////////

//
// The DrawAlphaBitmap and its associated routines were
// written and provided by Andy Campbell aka Assassin
//

bool CGenesisEngine::DrawAlphaBitmap(	 
									 geBitmap * pBitmap, 
									 geVec3d * VertUVArray, 
									 geCamera * ClipCamera,	// if null, uses full screen 
									 FloatRect * PixelRect,	// pixels in the "camera" view 
									 FloatRect * PercentRect,// percent of the "camera" view 
									 geFloat   Alpha, 
									 GE_RGBA* RGBA_Array) 
{ 
	// set up variables 
	GE_TLVertex vertex[4]; 
	geFloat fUVAdd = 0.0f; 
	geFloat fWidthBmp = 0; 
	geFloat fHeightBmp = 0; 
	geRect ClipRect = {0,0,0,0}; 
	geRect UseRect = {0,0,0,0}; 
	geVec3d DefaultUVArray[4] =  {{0,0,0},{1,0,0},{1,1,0},{0,1,0}}; 
	GE_RGBA DefaultRGBA_Array[4] = {{255,255,255,Alpha}, 
	{255,255,255,Alpha}, 
	{255,255,255,Alpha}, 
	{255,255,255,Alpha}};
	geBitmap_Info TempInfo, TempInfo2; 
	
	if(pBitmap) 
		geBitmap_GetInfo(pBitmap, &TempInfo, &TempInfo2); 
	else 
		TempInfo.Height = TempInfo.Width = 8; 
	
	fWidthBmp = (geFloat)TempInfo.Width; 
	fHeightBmp = (geFloat)TempInfo.Height;
	
	// 
	// Clip 2d viewport 
	// 
	
	if(!ClipCamera) 
    { 
		ClipRect.Top = 0; 
		ClipRect.Left = 0; 
		ClipRect.Bottom = m_nHeight-1; 
		ClipRect.Right = m_nWidth-1; 
    } 
	else 
		geCamera_GetClippingRect( ClipCamera, &ClipRect ); 
	
	if(!VertUVArray) 
		VertUVArray = &DefaultUVArray[0];
	if(!RGBA_Array) 
		RGBA_Array = &DefaultRGBA_Array[0];
	
	if(PixelRect) 
	{ 
		UseRect.Top = int32(PixelRect->Top + ClipRect.Top); 
		UseRect.Left = int32(PixelRect->Left + ClipRect.Left); 
		UseRect.Bottom = int32(PixelRect->Bottom + ClipRect.Top); 
		UseRect.Right = int32(PixelRect->Right + ClipRect.Left); 
	} 
	else 
	{ 
		if(PercentRect) 
		{ 
			UseRect.Top	= int32(ClipRect.Top	+ 0.01f * PercentRect->Top	 
				* (ClipRect.Bottom	- ClipRect.Top)); 
			UseRect.Left	= int32(ClipRect.Left	+ 0.01f * PercentRect->Left	 
				* (ClipRect.Right	- ClipRect.Left)); 
			UseRect.Bottom	= int32(ClipRect.Top	+ 0.01f * PercentRect->Bottom	 
				* (ClipRect.Bottom	- ClipRect.Top)); 
			UseRect.Right	= int32(ClipRect.Left	+ 0.01f * PercentRect->Right	 
				* (ClipRect.Right	- ClipRect.Left)); 
		} 
		else 
			UseRect = ClipRect; 
	}
	
	geFloat UVbreak = 0.0f;
	vertex[0].x = (geFloat)UseRect.Left; 
	vertex[0].y = (geFloat)UseRect.Top; 
	vertex[0].z = 0.05f; 
	vertex[0].r = RGBA_Array[0].r; 
	vertex[0].g = RGBA_Array[0].g; 
	vertex[0].b = RGBA_Array[0].b; 
	vertex[0].a = RGBA_Array[0].a; 
	vertex[0].u = VertUVArray[0].X + UVbreak/fWidthBmp; 
	vertex[0].v = VertUVArray[0].Y + UVbreak/fHeightBmp;
	vertex[1].x = (geFloat)UseRect.Right; 
	vertex[1].y = (geFloat)UseRect.Top; 
	vertex[1].z = vertex[0].z; 
	vertex[1].r = RGBA_Array[1].r; 
	vertex[1].g = RGBA_Array[1].g; 
	vertex[1].b = RGBA_Array[1].b; 
	vertex[1].a = RGBA_Array[1].a; 
	vertex[1].u = VertUVArray[1].X - UVbreak/fWidthBmp; 
	vertex[1].v = VertUVArray[1].Y + UVbreak/fHeightBmp;
	vertex[2].x = (geFloat)UseRect.Right; 
	vertex[2].y = (geFloat)UseRect.Bottom; 
	vertex[2].z = vertex[0].z; 
	vertex[2].r = RGBA_Array[2].r; 
	vertex[2].g = RGBA_Array[2].g; 
	vertex[2].b = RGBA_Array[2].b; 
	vertex[2].a = RGBA_Array[2].a; 
	vertex[2].u = VertUVArray[2].X - UVbreak/fWidthBmp; 
	vertex[2].v = VertUVArray[2].Y - UVbreak/fHeightBmp;
	vertex[3].x = (geFloat)UseRect.Left; 
	vertex[3].y = (geFloat)UseRect.Bottom; 
	vertex[3].z = vertex[0].z; 
	vertex[3].r = RGBA_Array[3].r; 
	vertex[3].g = RGBA_Array[3].g; 
	vertex[3].b = RGBA_Array[3].b; 
	vertex[3].a = RGBA_Array[3].a; 
	vertex[3].u = VertUVArray[3].X + UVbreak/fWidthBmp; 
	vertex[3].v = VertUVArray[3].Y - UVbreak/fHeightBmp;
	
	if(vertex[0].x < ClipRect.Left) 
    { 
		if(vertex[1].x <= ClipRect.Left) 
		{ 
			ReportError("Clipping Rect has negative dimension", NULL); 
			return false; 
		}
		fUVAdd = (ClipRect.Left-vertex[0].x)/(vertex[1].x-vertex[0].x); 
		fUVAdd *= (vertex[1].u-vertex[0].u); 
		fWidthBmp -= ( ClipRect.Left - vertex[0].x );
		vertex[0].u += fUVAdd; 
		vertex[3].u = vertex[0].u;
		vertex[0].x = (geFloat)ClipRect.Left; 
		vertex[3].x = vertex[0].x; 
    }
	if(vertex[0].y < ClipRect.Top) 
    { 
		if(vertex[2].y <= ClipRect.Top)
		{ 
			ReportError("Clipping Rect has negative dimension", false); 
			return false; 
		}
		fUVAdd = (ClipRect.Top-vertex[0].y)/(vertex[3].y-vertex[0].y); 
		fUVAdd *= (vertex[3].v-vertex[0].v); 
		fHeightBmp -= ( ClipRect.Top - vertex[0].y );
		vertex[0].v += fUVAdd; 
		vertex[1].v = vertex[0].v;
		vertex[0].y = (geFloat)ClipRect.Top; 
		vertex[1].y = vertex[0].y; 
    }
	if(vertex[1].x > ClipRect.Right) 
    { 
		if(vertex[0].x >= ClipRect.Right ) 
		{ 
			//ReportError("Clipping Rect has negative dimension", false); 
			return false; 
		}
		fUVAdd = (vertex[1].x-ClipRect.Right)/(vertex[1].x-vertex[0].x); 
		fUVAdd *= (vertex[1].u-vertex[0].u);
		vertex[1].u -= fUVAdd; 
		vertex[2].u = vertex[1].u;
		vertex[1].x = (geFloat)ClipRect.Right - 1; 
		vertex[2].x = vertex[1].x; 
    }
	if(vertex[2].y > ClipRect.Bottom) 
    { 
		if(vertex[0].y >= ClipRect.Bottom ) 
		{ 
			//ReportError("Clipping Rect has negative dimension", false); 
			return false; 
		}
		fUVAdd = (vertex[2].y-ClipRect.Bottom)/(vertex[3].y-vertex[0].y); 
		fUVAdd *= (vertex[3].v-vertex[0].v);
		vertex[2].v -= fUVAdd; 
		vertex[3].v = vertex[2].v;
		vertex[2].y = (geFloat)ClipRect.Bottom - 1; 
		vertex[3].y = vertex[2].y; 
    }
	geEngine_RenderPoly(m_theEngine,vertex,  
		4, pBitmap, (Alpha != 255 ? DRV_RENDER_ALPHA : 0 ) |  
		DRV_RENDER_CLAMP_UV | DRV_RENDER_NO_ZMASK | 
		DRV_RENDER_NO_ZWRITE ); 
	return true; 
}

//	DrawAlphaBitmapRect

bool CGenesisEngine::DrawAlphaBitmapRect(geBitmap * pBitmap,
										 geRect * BitmapRect, geCamera * ClipCamera, FloatRect * PixelRect,
										 FloatRect * PercentRect, geFloat Alpha, GE_RGBA* RGBA_Array)
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
		BitmapRect->Right = (TempInfo.Width-1);
		BitmapRect->Bottom = (TempInfo.Height-1);
	}
	UVArray[0].X = (geFloat)BitmapRect->Left / (TempInfo.Width-1);
	UVArray[0].Y = (geFloat)BitmapRect->Top / (TempInfo.Height-1);
	UVArray[1].X = (geFloat)BitmapRect->Right / (TempInfo.Width-1);
	UVArray[1].Y = (geFloat)BitmapRect->Top / (TempInfo.Height-1);
	UVArray[2].X = (geFloat)BitmapRect->Right / (TempInfo.Width-1);
	UVArray[2].Y = (geFloat)BitmapRect->Bottom / (TempInfo.Height-1);
	UVArray[3].X = (geFloat)BitmapRect->Left / (TempInfo.Width-1);
	UVArray[3].Y = (geFloat)BitmapRect->Bottom / (TempInfo.Height-1);
	return DrawAlphaBitmap(pBitmap, UVArray, ClipCamera, PixelRect, PercentRect, Alpha, RGBA_Array);
}

// takes Bitmap
// returns BitmapBuffer, Width and Height
//
bool CGenesisEngine::BreakUpBigBitmap(geBitmap * pBitmap, 
									  IncompleteTexture*& BitmapBuffer, int & NumWide, int & NumHigh) 
{ 
	int WidthBmp, HeightBmp; 
	gePixelFormat PFormat; 
	int BitmapsWide, BitmapsHigh; 
	int i,j;
	
	if(!pBitmap) 
	{ 
		ReportError("BreakUpBigBitmap needs a bitmap", false); 
		return false; 
	} 
	
	if(!BitmapBuffer) 
	{ 
		ReportError("BreakUpBigBitmap needs a bitmapbuffer", false); 
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
	int Width=0, Height=0;  
	geBitmap * Bitmap = NULL; 
	
	for(i = 0; i < BitmapsHigh; i++) 
	{ 
		for(j = 0; j < BitmapsWide; j++) 
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
				ReportError("BreakUpBigBitmap could not create a bitmap", false); 
				// MEMORY LEAK
				return false; 
			} 
			if(!geBitmap_Blit(  pBitmap, j*256, i*256, Bitmap, 0, 0, Width, Height)) 
			{  
				ReportError("BreakUpBigBitmap could not blit a bitmap", false); 
				// MEMORY LEAK
				return false; 
			} 
			if(!geBitmap_SetPreferredFormat(Bitmap, GE_PIXELFORMAT_8BIT))
			{ 
				ReportError("BreakUpBigBitmap could not change formats", false); 
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

CompleteTexture CGenesisEngine::BuildCompleteTexture(IncompleteTexture* 
													 BitmapBuffer, int NumWide, int NumHigh) 
{ 
	CompleteTexture ret;
	ret.TextureArray = BitmapBuffer; 
	//	ret.FirstRowPattern = LeftRight; 
	ret.TexturesHigh = NumHigh; 
	ret.TexturesWide = NumWide; 
	ret.TotalHeight = BitmapBuffer[NumHigh*NumWide].Height + (NumHigh-1) * 256; 
	ret.TotalWidth = BitmapBuffer[NumHigh*NumWide].Width + (NumWide-1) * 256;
	
	return ret; 
}

bool CGenesisEngine::AddCompleteTextureToWorld(CompleteTexture cp) 
{ 
	int i, bmpcount; 
	bool retval = true; 
	bmpcount = cp.TexturesHigh * cp.TexturesWide; 
	for(i = 0; i < bmpcount; i++) 
	{ 
		if(!geWorld_AddBitmap(CCD->World(), cp.TextureArray[i].Bitmap)) 
			retval = false; 
	} 
	return retval; 
}

void CGenesisEngine::DeleteCompleteTexture(CompleteTexture cp) 
{ 
	int i, bmpcount; 
	bool retval = true; 
	bmpcount = cp.TexturesHigh * cp.TexturesWide; 
	for(i = 0; i < bmpcount; i++) 
	{ 
		geWorld_RemoveBitmap(CCD->World(), cp.TextureArray[i].Bitmap);
		geBitmap_Destroy(&cp.TextureArray[i].Bitmap);
	} 
	delete cp.TextureArray;
	cp.TextureArray = NULL;
	return; 
}

CompleteTexture CGenesisEngine::BitmapToComplete(geBitmap * pBitmap)
{
	IncompleteTexture *BitmapBuffer;
	int NumWide, NumHigh;
	CompleteTexture cp;

	BreakUpBigBitmap(pBitmap, BitmapBuffer, NumWide, NumHigh); 
	cp = BuildCompleteTexture(BitmapBuffer, NumWide, NumHigh); 
	AddCompleteTextureToWorld(cp);

	cp.TotalWidth = NumWide*256; 
	cp.TotalHeight = NumHigh*256; 
	return cp;
}

void CGenesisEngine::DrawComplete(CompleteTexture cp, int x, int y)
{
	FloatRect ScreenRect;

	ScreenRect.Left = (float)x;
	ScreenRect.Top = (float)y;
	ScreenRect.Right = ScreenRect.Left + cp.TotalWidth;
	ScreenRect.Bottom = ScreenRect.Top + cp.TotalHeight;
	DrawCompleteTexture(cp, CCD->CameraManager()->Camera(), 
						 &ScreenRect, 
						 NULL, 255.0f, NULL);
}

bool CGenesisEngine::DrawCompleteTexture(CompleteTexture cp, 
										 geCamera * ClipCamera, 
										 FloatRect * ScreenRect, 
										 FloatRect * PercentRect, 
										 geFloat Alpha, 
										 GE_RGBA* RGBA_Array) // ignored currently 
{ 
	bool retval = true; 
	int i, x, wide, high; 
	geBitmap * bmp = NULL; 
	geFloat Top, Bottom, Left, Right; 
	FloatRect RealScreenRect; 
	FloatRect UseScreenRect; 
	geRect BitmapRect; 
	bool RectInPercent = true;
	
	if(ScreenRect == NULL && PercentRect != NULL) 
		RectInPercent = true; 
	
	if(RectInPercent && PercentRect != NULL) 
		RealScreenRect = *PercentRect; 
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
	
	for(x = 0; x < i; x++) 
	{ 
		bmp = cp.TextureArray[x].Bitmap; 
		BitmapRect.Left = 0; 
		BitmapRect.Top = 0; 
		BitmapRect.Right = cp.TextureArray[x].Width; 
		BitmapRect.Bottom = cp.TextureArray[x].Height; 
		if(RectInPercent) 
		{ 
			UseScreenRect.Top    = (((Bottom-Top)/high)*(x/wide)) + Top; 
			UseScreenRect.Bottom = (((Bottom-Top)/high)*(x/wide)  + Top + ((Bottom-Top)/high)); 
			UseScreenRect.Left   = (((Right-Left)/wide)*(x%wide)) + Left; 
			UseScreenRect.Right  = (((Right-Left)/wide)*(x%wide)  + Left + ((Right-Left)/wide)); 
			if(!DrawAlphaBitmapRect(bmp, &BitmapRect, ClipCamera, NULL, &UseScreenRect, Alpha)) 
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
			if(!DrawAlphaBitmapRect(bmp, &BitmapRect, ClipCamera, &UseScreenRect, NULL, Alpha)) 
				retval = false; 
		} 
	}
	
	return retval; 
}
//
// End of DrawAlphaBitmap routines
//

// changed RF064
bool CGenesisEngine::DrawBitmap(geBitmap *pBitmap, geRect *BitmapRect, int x, int y)
{
	FloatRect PixelRect;
	bool ret;
	geRect Rect;

	PixelRect.Top = (float)y;
	PixelRect.Left = (float)x;

	if(BitmapRect)
	{
		PixelRect.Bottom = PixelRect.Top+(float)(BitmapRect->Bottom-BitmapRect->Top);
		PixelRect.Right = PixelRect.Left+(float)(BitmapRect->Right-BitmapRect->Left);
		ret = DrawAlphaBitmapRect(pBitmap, BitmapRect, 
								 CCD->CameraManager()->Camera(), &PixelRect,
								 NULL, 255.0f, NULL);
	}
	else
	{
		Rect.Top = 0;
		Rect.Left = 0;
		Rect.Bottom = geBitmap_Height(pBitmap);
		Rect.Right = geBitmap_Width(pBitmap);
		PixelRect.Bottom = PixelRect.Top+(float)(Rect.Bottom);
		PixelRect.Right = PixelRect.Left+(float)(Rect.Right);
		ret = DrawAlphaBitmapRect(pBitmap, &Rect, 
								 CCD->CameraManager()->Camera(), &PixelRect,
								 NULL, 255.0f, NULL);
	}
	return ret;
}
// end change RF064

//	ShowFrameRate
//
//	Turn frame rate display on/off

void CGenesisEngine::ShowFrameRate(bool bHow)
{
	geEngine_EnableFrameRateCounter(m_theEngine, bHow);
	
	return;
}

//	LoadLevel
//
//	Load a level file into the current engine.

bool CGenesisEngine::LoadLevel(char *szLevelFileName)
{
	//	Check to see if we have a level loaded, and if so, gun it!
	
	if(m_World != NULL)
	{
		geEngine_RemoveWorld(m_theEngine, m_World);
		geWorld_Free(m_World);
		m_World = NULL;
	}
	
	//	Ok, try to open up the desired level file
	
	CCD->OpenRFFile(&m_Level, kLevelFile, szLevelFileName,
								GE_VFILE_OPEN_READONLY);
	
	if(!m_Level)
	{
		char szBug[256];
		sprintf(szBug, "Failed to load level '%s'", szLevelFileName);
		ReportError(szBug, false);
		return FALSE;
	}
	
	//	Level open, now try to load it into Genesis3D
	
	m_World = geWorld_Create(m_Level);
	geVFile_Close(m_Level);
	
	if(!m_World) 
	{
		char szBug[256];
		sprintf(szBug, "Loaded level '%s' failed to World create",szLevelFileName);
		ReportError(szBug, false);
		return FALSE;
	}
	
	if(geEngine_AddWorld(m_theEngine, m_World)== GE_FALSE)
	{
		char szBug[256];
		sprintf(szBug, "Loaded level '%s' failed to AddWorld",szLevelFileName);
		ReportError(szBug, false);
		return FALSE;
	}
	
	strcpy(m_CurrentLevel, szLevelFileName);		// Save level name
	
	//	Level loaded and locked into the engine.
	
	return TRUE;
}

//	BeginFrame
//
//	Set up the engine to prepare for entities, etc. to be rendered into
//	..it.

int CGenesisEngine::BeginFrame()
{
	if(m_bInFramePass)
		return RGF_FAILURE;					// Already in a BeginFrame()/EndFrame() block
	
	if(geEngine_BeginFrame(m_theEngine, CCD->CameraManager()->Camera(), GE_TRUE) == GE_FALSE)
	{
		ReportError("CGenesisEngine::BeginFrame failed", false);
		exit(-1);
		return RGF_FAILURE;
	}
	
	m_bInFramePass = true;
	
	return RGF_SUCCESS;
}

//	EndFrame
//
//	All rendering is done, now clean up and flip or blit the results
//	..into the main window.

int CGenesisEngine::EndFrame()
{
	if(!m_bInFramePass)
		return RGF_FAILURE;				// Not in BeginFrame()/EndFrame() block
	
	if(geEngine_EndFrame(m_theEngine) == GE_FALSE)
	{
		ReportError("CGenesisEngine::EndFrame failed", false);
		return RGF_FAILURE;
	}
	
	m_bInFramePass = false;
	
	return RGF_SUCCESS;
}

//	RenderWorld
//
//	Render the BSP-based world the Genesis engine is running.

int CGenesisEngine::RenderWorld()
{
	if(!m_bInFramePass)
		return RGF_FAILURE;				// Not in BeginFrame()/EndFrame() block
	
	//	Update the camera position for bindings, etc.
	
	CCD->CameraManager()->TrackMotion();
	CCD->CameraManager()->RenderView();

	//	We're ready, render it!
	
	if(geEngine_RenderWorld(m_theEngine, m_World, CCD->CameraManager()->Camera(), 0.00f) == GE_FALSE)
	{
		ReportError("CGenesisEngine::RenderWorld failed", false);
		return RGF_FAILURE;
	}
	
	return RGF_SUCCESS;
}

//	DisplaySplash
//
//	Throw a bitmap up on the screen as a splash screen.  This is used for
//	..displaying "Loading" bitmaps, intro screens, credit screens, etc.
//	..Note that the next render cycle will over-write what's on the screen.

int CGenesisEngine::DisplaySplash(char *szSplashBMP, char *szAudioFile)
{
	geBitmap *theBmp;
	geBitmap_Info	BmpInfo;
	int x, y;
	
	//////////////////////////////////////////////////////////////////////
	//Dee
	//Need to adjust window sizes postions etc. to allow for windowed mode
	//	int tWindowLeft, tWindowTop;
	
	//	tWindowLeft=(m_ScreenRect.right + m_ScreenRect.left - m_nWidth) / 2;
	//	tWindowTop=(m_ScreenRect.bottom + m_ScreenRect.top - m_nHeight) / 2;
	//	tWindowLeft=(m_ScreenRect.right - m_nWidth) / 2;
	//	tWindowTop=(m_ScreenRect.bottom - m_nHeight) / 2;
	//End Dee
	//////////////////////////////////////////////////////////////////////
	
	theBmp = CreateFromFileName(szSplashBMP);
	
	if(theBmp != NULL)
	{
		if(geBitmap_GetInfo(theBmp, &BmpInfo, NULL) == GE_TRUE)
		{
			x = (m_nWidth - BmpInfo.Width) / 2;
			y = (m_nHeight - BmpInfo.Height) / 2;
			if(geEngine_AddBitmap(m_theEngine, theBmp) == GE_FALSE)
			{
				char szError[200];
				sprintf(szError,"DisplaySplash: addbitmap failed on '%s'\n", szSplashBMP);
				ReportError(szError, false);
				return RGF_FAILURE;
			}
			if(m_bInFramePass)
				EndFrame();
			// Version 053
			geEngine_BeginFrame(m_theEngine, CCD->CameraManager()->Camera(), GE_TRUE);
			m_bInFramePass = true;
			if(geEngine_DrawBitmap(m_theEngine, theBmp, NULL, x, y ) == GE_FALSE)
			{
				char szError[200];
				sprintf(szError,"DisplaySplash: drawbitmap failed on '%s'\n", szSplashBMP);
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
			CCD->OpenRFFile(&SoundFile, kAudioFile, szAudioFile,
				GE_VFILE_OPEN_READONLY);
			if(!SoundFile)
			{
				char szError[256];
				sprintf(szError,"Can't open splash audio file '%s'\n", szAudioFile);
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

//	SetFogParameters
//
//	Set up parameters to be used for distance fogging

int CGenesisEngine::SetFogParameters(GE_RGBA theColor, geFloat fStart, geFloat fEnd)
{
	FogColor = theColor;
	fFogStart = fStart;
	fFogEnd = fEnd;
	
	return RGF_SUCCESS;
}

//  EnableFog
//
//	Turn distance fogging on and off

void CGenesisEngine::EnableFog(geBoolean FogOn)
{
	geEngine_SetFogEnable(m_theEngine, FogOn, 
		FogColor.r, FogColor.g, FogColor.b, 
		fFogStart, fFogEnd);
	
	return;
}

//	ReportError
//
//	Log an error message to the RGF log file.  If bMessageBoxIt is
//	..TRUE, the user gets a Windows message box containing the
//	..offending message.

bool CGenesisEngine::ReportError(char *szError, bool bMessageBoxIt)
{
	FILE *fd;
	
	if(m_DebugEnabled == false)
		return true;										// DEBUG disabled, ignore call
	
	int OldDebugLevel = CCD->SetDebugLevel(kNoDebugOutput);
	
	if((fd=CCD->OpenRFFile(kRawFile, ".\\RealityFactory.log","at")) == NULL)
	{
		OutputDebugString("Error logging failure!\n");
		if(bMessageBoxIt)
			MessageBox(NULL, "Error Logging Failure!","RGF Error Report",
			MB_ICONEXCLAMATION | MB_OK);
		CCD->SetDebugLevel(OldDebugLevel);
		return false;						// Seriously bad - error not logged
	}
	
	struct tm *newtime;
	time_t aclock;
	
	//	Log date and time of error, as well
	
	time(&aclock);
	newtime = localtime(&aclock);
	
//	fputs(asctime(newtime), fd);
	fputs(szError, fd); fputs("\n", fd);
	
	fclose(fd);
	
	//	Ok, out to a file, now to be sure we can track it in the
	//	..debugger
	
//	OutputDebugString(asctime(newtime));
	OutputDebugString(szError); OutputDebugString("\n");
	
	if(bMessageBoxIt)
		MessageBox(NULL, szError, "Game Error", MB_ICONEXCLAMATION | MB_OK);
	
	CCD->SetDebugLevel(OldDebugLevel);
	
	return true;							// Error logged
}

//	SaveTo
//
//	Save off the current level name to the supplied file

int CGenesisEngine::SaveTo(FILE *SaveFD)
{
	fwrite(&m_CurrentLevel, 1, 256, SaveFD);
	
	return RGF_SUCCESS;
}

//	RestoreFrom
//
//	Restore the current level name from the supplied file.  Will
//	..automatically load the level.

int CGenesisEngine::RestoreFrom(FILE *RestoreFD)
{
	fread(&m_CurrentLevel, 1, 256, RestoreFD);
	
	return RGF_SUCCESS;			// And load the level
}
