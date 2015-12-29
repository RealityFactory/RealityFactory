/************************************************************************************//**
 * @file CGenesisEngine.h
 * @brief Genesis3D engine encapsulation
 *
 * This file contains the class declaration for the Genesis3D engine wrapper.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_CGENESISENGINE_H_
#define __RGF_CGENESISENGINE_H_

struct IncompleteTexture
{
	geBitmap	*Bitmap;
	int			Width;	///< really 256 wide, use this value in DrawAlphaBitmapRect
	int			Height;	///< really 256 high, use this value in DrawAlphaBitmapRect
};


struct CompleteTexture
{
	IncompleteTexture *TextureArray;
	int TexturesWide;
	int TexturesHigh;
	int TotalWidth;
	int TotalHeight;
	//	enum {LeftRight, TopDown} FirstRowPattern;
};

// BitmapBuffer is a reference to an array of bitmap pointers...
// [0] [1] [2] [3]
// [4] [5] [6] [7]
// [8] [9] [10][11]

// Render Flags for ALL render functions
#define DRV_RENDER_ALPHA		(1<<0)	///< Render function should expect alpha set in vertices
#define DRV_RENDER_FLUSH		(1<<1)	///< Render function should gaurentee that this render happens NOW
#define DRV_RENDER_NO_ZMASK		(1<<2)	///< No zbuffering should be performed
#define DRV_RENDER_NO_ZWRITE	(1<<3)	///< No z writing will be performed
#define DRV_RENDER_CLAMP_UV		(1<<4)	///< Clamp UV in both directions

/**
 * @brief This wrapper hides many of the gory details necessary to use the
 * Genesis3D engine.
 */
class CGenesisEngine : public CRGFComponent
{
public:
	/**
	 * @brief Constructor
	 *
	 * Construct a main window and initialize the engine, also start up the
	 * Genesis3D sound system.
	 */
	CGenesisEngine(bool fFullScreen, int nWidth, int nHeight, const char *szName,
					HINSTANCE theInstance, char chDriverID,	bool bUseSoftwareRenderer,
					bool UseDialog, char *szStartLevel);
	~CGenesisEngine();						///< Default destructor

	bool DrawAlphaBitmap(geBitmap *pBitmap, geVec3d *VertUVArray, geCamera *ClipCamera,
						FloatRect *PixelRect, FloatRect *PercentRect, geFloat Alpha,
						GE_RGBA *RGBA_Array, float zdepth);

	bool DrawAlphaBitmapRect(geBitmap *pBitmap, geRect *BitmapRect, geCamera *ClipCamera,
							FloatRect *PixelRect, FloatRect *PercentRect, geFloat Alpha,
							GE_RGBA *RGBA_Array, float zdepth);

	bool BreakUpBigBitmap(geBitmap *pBitmap, IncompleteTexture *&BitmapBuffer,
							int &NumWide, int &NumHigh);

	CompleteTexture BuildCompleteTexture(IncompleteTexture *BitmapBuffer,
											int NumWide, int NumHigh);

	bool AddCompleteTextureToWorld(const CompleteTexture &cp);

	bool DrawCompleteTexture(const CompleteTexture &cp, geCamera *ClipCamera,
							FloatRect *ScreenRect, FloatRect *PercentRect,
							geFloat Alpha, GE_RGBA *RGBA_Array = NULL);


	bool DrawBitmap(geBitmap *pBitmap, geRect *BitmapRect, int x, int y, float zdepth);
	bool DrawBitmap(geBitmap *pBitmap, geRect *BitmapRect, int x, int y, float Alpha, float zdepth);
	CompleteTexture BitmapToComplete(geBitmap *pBitmap);
	void DeleteCompleteTexture(CompleteTexture cp);
	void DrawComplete(const CompleteTexture &cp, int x, int y);


	void ShowFrameRate(bool bHow);					///< Show/Hide frame rate
	bool LoadLevel(const char *szLevelFilename);	///< Load level into engine

	int BeginFrame();								///< Start rendering frame
	int EndFrame();									///< End rendering frame

	int RenderWorld();								///< Render the Genesis3D world
	int SetWorldSpaceXForm(geXForm3d *theXForm);	///< Set worldspace transform

	/**
	 * @brief Display splash screen, with sound file
	 */
	int DisplaySplash(const char *szSplashBMP, const char *szAudioFile);

	/**
	 * @brief Set up fogging parameters
	 */
	int SetFogParameters(GE_RGBA FogColor, geFloat fFogStart, geFloat fFogEnd);
	void EnableFog(geBoolean FogOn);				///< Enable/disable fog

	/**
	 * @brief Report error to error log file
	 */
	bool ReportError(const char *szError, bool bMessageBoxIt);

	int SaveTo(FILE *SaveFD);						///< Save engine data to file
	int RestoreFrom(FILE *RestoreFD);				///< Restore engine data from file
	void ResetSystem();

	//	Accessor functions
	geEngine *Engine()				{ return m_theEngine;	}	///< Get Genesis3D engine pointer
	bool FullScreen() const			{ return m_fFullScreen;	}	///< Get fullscreen/windowed status
	geWorld *World()				{ return m_World;		}	///< Get Genesis3D World
	HWND WindowHandle()				{ return m_wndMain;		}	///< Get main window handle
	int Width() const				{ return m_nWidth;		}	///< Window width
	int Height() const				{ return m_nHeight;		}	///< Window height
	geSound_System *AudioSystem()	{ return m_Audio;		}	///< Audio system pointer
	char *LevelName()				{ return m_CurrentLevel;}	///< Current level name

	void SetDebugging(bool fOn)		{ m_DebugEnabled = fOn;	}	///< Activate/deactivate debug output
	float GetFogEnd()				{ return fFogEnd;		}
	float GetFogStart()				{ return fFogStart;		}

private:
	//	Private member functions
	bool CreateEngine(const char *szName);	///< Create Genesis3D engine
	bool FindDriver();						///< Locate appropriate Genesis3D driver

	/**
	 * @brief Select any available driver and use it's first mode
	 */
	bool AutoDriver();

	/**
	 * @brief Pop up a pick list of available drivers
	 */
	bool PickDriver();

private:
	//	Private member variables
	int			m_nWidth, m_nHeight;		///< Window dimensions
	bool		m_fFullScreen;				///< Fullscreen/windowed flag
	char		m_SelectedDriverID;			///< Driver selection ID, Genesis style
	RECT		m_ScreenRect;				///< Screen size rectangle
	HWND		m_wndMain;					///< Main window handle
	HINSTANCE	m_Instance;					///< Support pop up driver window

	geDriver_System	*m_DrvSys;				///< Genesis3D driver system pointer
	geDriver		*m_Driver;				///< Genesis3D driver selected
	geDriver_Mode	*m_Mode;				///< Genesis3D driver mode selected
	geWorld			*m_World;				///< Current Genesis3D world
	geEngine		*m_theEngine;			///< Genesis3D engine pointer
	geVFile			*m_Level;				///< Genesis3D level file handle
	geSound_System	*m_Audio;				///< Genesis3D sound system
	geSound_Def		*m_SplashAudio;			///< Splash screen audio effect
	int				m_MasterVolume;			///< Master volume level
	bool			m_fInFramePass;			///< TRUE if between BeginFrame()/EndFrame()
	char			m_CurrentLevel[256];	///< Current level name, if any
	bool			m_DebugEnabled;			///< Debug output enabled flag
	GE_RGBA			FogColor;				///< Color for distance fog
	geFloat			fFogStart, fFogEnd;		///< Fog start, end distances
	geFloat			FarClipPlaneDistance;	///< Distance to far clip plane
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
