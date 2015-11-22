/*
CGenesisEngine.h:		Genesis3D engine encapsulation

  (c) 1999 Edward A. Averill, III
  
	This file contains the class declaration for the Genesis3D
	engine wrapper.  This wrapper hides many of the gory details
	necessary to use the Genesis3D 1.0 engine.
*/

#ifndef __RGF_CGENESISENGINE_H_
#define __RGF_CGENESISENGINE_H_

struct IncompleteTexture 
{ 
	geBitmap * Bitmap; 
	int Width;	// really 256 wide, use this value in DrawAlphaBitmapRect 
	int Height;	// really 256 high, use this value in DrawAlphaBitmapRect 
};

struct CompleteTexture 
{ 
	IncompleteTexture * TextureArray; 
	int TexturesWide; 
	int TexturesHigh; 
	int TotalWidth; 
	int TotalHeight; 
	//	enum {LeftRight, TopDown} FirstRowPattern; 
};
//BitmapBuffer is a reference to an array of bitmap pointers... 
//[0] [1] [2] [3] 
//[4] [5] [6] [7] 
//[8] [9] [10][11] 

// Render Flags for ALL render functions
#define DRV_RENDER_ALPHA		(1<<0)	// Render function should expect alpha set in vertices
#define DRV_RENDER_FLUSH		(1<<1)	// Render function should gaurentee that this render happens NOW
#define DRV_RENDER_NO_ZMASK		(1<<2)	// No zbuffering should be performed
#define DRV_RENDER_NO_ZWRITE	(1<<3)	// No z writing will be performed
#define DRV_RENDER_CLAMP_UV		(1<<4)	// Clamp UV in both directions

class CGenesisEngine : public CRGFComponent
{
public:
	CGenesisEngine(bool fFullScreen, int nWidth, int nHeight,
		char *szName, HINSTANCE theInstance, char chDriverID,
		bool bUseSoftwareRenderer, bool UseDialog, char *szStartLevel);
	~CGenesisEngine();						// Default destructor
	bool DrawAlphaBitmap(geBitmap * pBitmap, 
		geVec3d * VertUVArray, geCamera * ClipCamera,	FloatRect * PixelRect,
		FloatRect * PercentRect, geFloat   Alpha, 
		GE_RGBA* RGBA_Array = NULL);
	bool DrawAlphaBitmapRect(geBitmap * pBitmap,
		geRect * BitmapRect, geCamera * ClipCamera, FloatRect * PixelRect,
		FloatRect * PercentRect, geFloat Alpha, 
		GE_RGBA* RGBA_Array = NULL);
	bool BreakUpBigBitmap(geBitmap * pBitmap, 
		IncompleteTexture*& BitmapBuffer, int & NumWide, int & NumHigh);
	CompleteTexture BuildCompleteTexture(IncompleteTexture* 
		BitmapBuffer, int NumWide, int NumHigh);
	bool AddCompleteTextureToWorld(geWorld * w, CompleteTexture cp);
	bool DrawCompleteTexture(CompleteTexture cp, geCamera * ClipCamera, 
		FloatRect * ScreenRect, FloatRect * PercentRect, 
		geFloat Alpha, GE_RGBA* RGBA_Array = NULL);
	void ShowFrameRate(bool bHow);		// Guess what this does?
	bool LoadLevel(char *szLevelFilename);		// Load level into engine
	int BeginFrame();							// Start rendering frame
	int EndFrame();								// End rendering frame
	int RenderWorld();						// Render the G3D world
	int SetWorldSpaceXForm(geXForm3d *theXForm);	// Set worldspace transform
	int DisplaySplash(char *szSplashBMP, char *szAudioFile);
	// Display splash screen, with sound file
	int SetFogParameters(GE_RGBA FogColor, geFloat fFogStart, geFloat fFogEnd);
	// Set up fogging parameters
	void EnableFog(geBoolean FogOn);	// Enable/disable fog
	bool ReportError(char *szError, bool bMessageBoxIt);
	// Report error to error log file
	int SaveTo(FILE *SaveFD);			// Save engine data to file
	int RestoreFrom(FILE *RestoreFD);	// Restore engine data from file
	//	Accessor functions
	geEngine *Engine()
	{ return m_theEngine;}			// Get G3D engine pointer
	bool FullScreen()
	{ return m_fFullScreen;}		// Get fullscreen/windowed status
	geWorld *World()
	{ return m_World;}					// Get G3D World
	HWND WindowHandle()
	{ return m_wndMain;}				// Get main window handle
	int Width()
	{ return m_nWidth;}					// Window width
	int Height()
	{ return m_nHeight;}				// Window height
	geSound_System *AudioSystem()
	{ return m_Audio;}					// Audio system pointer
	char *LevelName()
	{ return m_CurrentLevel;}		// Current level name
	void SetDebugging(bool fOn)
	{ m_DebugEnabled = fOn;}		// Activate/deactivate debug output
private:
	//	Private member functions
	bool CreateEngine(char *szName);	// Create G3D engine
	bool FindDriver();						// Locate appropriate G3D driver
	/////////////////////////////////////////////////////////////////
	// Dee  07/07/00
	// Added to select any available driver and use it's first mode
	/////////////////////////////////////////////////////////////////
	bool AutoDriver();
	/////////////////////////////////////////////////////////////////
	//End Dee
	/////////////////////////////////////////////////////////////////
	
	/////////////////////////////////////////////////////////////////
	// Dee  12/07/00
	// Added to pop up a pick list of available drivers
	/////////////////////////////////////////////////////////////////
	bool PickDriver();
	/////////////////////////////////////////////////////////////////
	//End Dee
	/////////////////////////////////////////////////////////////////
	
	//	Private member variables
	int m_nWidth, m_nHeight;			// Window dimensions
	bool m_fFullScreen;						// Fullscreen/windowed flag
	bool m_bUseSoftware;					// Software rendering
	char m_SelectedDriverID;			// Driver selection ID, Genesis style
	RECT m_ScreenRect;						// Screen size rectangle
	HWND m_wndMain;								// Main window handle
	/////////////////////////////////////////////////////////////////
	//Dee
	//Added to support pop up driver window
	/////////////////////////////////////////////////////////////////
	HINSTANCE m_Instance;
	/////////////////////////////////////////////////////////////////
	//end Dee
	/////////////////////////////////////////////////////////////////
	geDriver_System	*m_DrvSys;		// G3D driver system pointer
	geDriver *m_Driver;						// G3D driver selected
	geDriver_Mode *m_Mode;				// G3D driver mode selected
	geWorld	*m_World;							// Current G3D world
	geEngine *m_theEngine;				// G3D engine pointer
	geVFile *m_Level;							// G3D level file handle
	geSound_System *m_Audio;			// G3D sound system
	geSound_Def *m_SplashAudio;		// Splash screen audio effect
	int m_MasterVolume;						// Master volume level
	int m_bInFramePass;						// TRUE if between BeginFrame()/EndFrame()
	char m_CurrentLevel[256];			// Current level name, if any
	bool m_DebugEnabled;					// Debug output enabled flag
	GE_RGBA FogColor;							// Color for distance fog
	geFloat fFogStart, fFogEnd;			// Fog start, end distances
	geFloat FarClipPlaneDistance;		// Distance to far clip plane
};

#endif
