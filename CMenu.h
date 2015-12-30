/************************************************************************************//**
 * @file CMenu.h
 * @brief CMenu class declaration
 ****************************************************************************************/

#ifndef __RGF_CRFMENU_H_
#define __RGF_CRFMENU_H_

#define NUM_BACKGROUNDS  20
#define NUM_TITLES       20
#define NUM_IMAGES       150
#define NUM_FONTS        30
#define NUM_SELECT		 10
#define NUM_ANIM		 70
#define FNAME_LENGHT	64
#define CHAR_RANGE	224

// Message
//------------------------------
// Font Names
//------------------------------

typedef enum
{
	FONT1=0,
	FONT2,
	FONT3,
	FONT4,
	FONT5,
	FONT6,
	FONT7,
	FONT8,
	FONT9,
	FONT10,
	FONT11,
	FONT12,
	FONT13,
	FONT14,
	FONT15,
	FONT16,
	FONT17,
	FONT18,
	FONT19,
	FONT20,
	FONT21,
	FONT22,
	FONT23,
	FONT24,
	FONT25,
	FONT26,
	FONT27,
	FONT28,
	FONT29

} FONT_NAME;

struct	MenuItem;

//---------------------------
// Font Structure
//---------------------------
/**
 * @brief Data describing width and position of a character on a font bitmap
 */
typedef struct Chardat
{
	int width;
	int x;
	int y;

} CharDat;

/**
 * @brief Bitmap font description
 */
typedef struct Font
{
  int		font_height;			// height of chars on font bitmap
  Chardat	dat[CHAR_RANGE];		// width an position of chars on font bitmap
  geBitmap	*Bitmap;				// font bitmap
  geBitmap	*WBitmap[CHAR_RANGE];	// separate character bitmaps

} Font;

/**
 * @brief Character selection
 */
typedef struct Selection
{
	char		Name[64];
	geBitmap	*Bitmap;			// image bitmap
	char		ActorName[64];
	float		ActorScale;
	geVec3d		Rotation;
	float		AnimSpeed;
	float		ShadowSize;
	GE_RGBA		FillColor;
	GE_RGBA		AmbientColor;
	geBoolean	AmbientLightFromFloor;
	float		ShadowAlpha;
	char		ShadowBitmap[64];
	char		ShadowAlphamap[64];
	geBoolean	UseProjectedShadows;
	geBoolean	UseStencilShadows;
	char		StartLevel[64];
	char		Weapon[64];
	char		Attribute[64];
	char		pSetup[64];
	float		JumpSpeed;
	float		Speed;
	float		StepHeight;
	char		Environment[64];
	char		PlayerStart[64];
	float		SlopeSpeed;
	float		SlopeSlide;
	char		Hud[64];

} Selection;


typedef struct	SoundList
{
	SoundList*	next;
	SoundList*	prev;
	geSound*	Sound;
	geFloat		Volume;
	geFloat		Pan;
	geFloat		Frequency;

} SoundList;


/**
 * @brief Class declaration for CRFMenu
 */
class CRFMenu : public CRGFComponent
{
public:
	explicit CRFMenu(const char* szStartLevel);
	~CRFMenu();

	int DoMenu(const char *levelname);
	int ProcessMenu(MenuItem *Menu, int Background_Number, int Title_Number);
	void DrawBitmap(const geBitmap *Bitmap, geRect *Source, uint32 x, uint32 y);
	void DoGame(bool editor);
	void ChangeCurrent(bool direction);
	void GameLoop();
	void GameLevel();
	void MenuInitalize();
	void ChangeMenuIni();
	char *GetConvtxts();
	char *GetMessagetxts();
	void DisplaySplash();
	void LoadWBitmap();
	void UnLoadWBitmap();
	void WorldFontRect(const char *s, int FontNumber, int x, int y, float Alpha);

	void MFontRect(const char *s, int FontNumber, int x, int y);
	void FontRect(const char *s, int FontNumber, int x, int y);
	void ScreenShot();

	geSound *PlaySoundDef(geSound_System *SoundS, geSound_Def *SoundDef, geFloat Volume, geFloat Pan, geFloat Frequency, geBoolean Loop);
	geBoolean StopSound(geSound_System *SoundS, geSound *Sound);
	void DeleteSound();
	void StopMenuMusic();

	int SaveTo(FILE *SaveFD, bool type);
	int RestoreFrom(FILE *RestoreFD, bool type);

	void DisplayCursor();
	inline void ShowCursor(bool bShow)			{ bShowCursor = bShow;		}
	inline bool GetShowCursor()					{ return bShowCursor;		}
	inline const char *GetLevelName()			{ return LevelName;			}
	inline void SetLevelName(const char *level)	{ strcpy(LevelName, level); }
	inline void SetInGame()						{ ingame = 1;				}
	inline int GetInGame()						{ return ingame;			}
	inline void SetMouseSen(float value)		{ MouseSen = value;			}
	inline float GetMouseSen()					{ return MouseSen;			}
	inline void SetMouseReverse(bool value)		{ MouseReverse = value;		}
	inline bool GetMouseReverse()				{ return MouseReverse;		}
	inline void SetMouseFilter(bool value)		{ MouseFilter = value;		}
	inline bool GetMouseFilter()				{ return MouseFilter;		}
	inline void SetBoundBox(bool value)			{ BoundingBox = value;		}
	inline bool GetBoundBox()					{ return BoundingBox;		}
	inline void SetSEBoundBox(bool value)		{ SEBoundingBox = value;	}
	inline bool GetSEBoundBox()					{ return SEBoundingBox;		}
	inline void SetNoClip(bool value)			{ NoClip = value;			}
	inline bool GetNoClip()						{ return NoClip;			}
	inline void SetFilter(POINT value)			{ Filter = value;			}
	inline POINT GetFilter()					{ return Filter;			}
	inline void SetShot(bool value)				{ ScrnWait = value;			}
	inline bool GetShot()						{ return ScrnWait;			}
	inline int GetMusicType()					{ return musictype;			}
	inline int GetDetail()						{ return Detail;			}
	inline void SetDetail(int value)			{ Detail = value;			}
	inline void SetStencilShadows(bool value)	{ StencilShadows = value;	}
	inline bool GetStencilShadows()				{ return StencilShadows;	}
	// Message
	int FontHeight(int FontNumber);
	int FontWidth(int FontNumber, const char *s);
	inline geBitmap *GetCrossHair()				{ return Crosshair;	}
	inline geBitmap *GetFCrossHair()			{ return FCrosshair;}
	inline char *GetLoadmsg()					{ return Loadmsg;	}
	inline int GetLoadFont()					{ return LoadFont;	}
	void SetMusicVol(float vol);
	void SetmMusicVol(float vol);
	inline bool GetUseNameSelect()			{ return usenameselect;	}
	inline bool GetUseSelect()				{ return useselect;		}
	inline const char *GetCurrentActor()	{ return CharSelect[CurrentSelect].ActorName;			}
	inline const char *GetCurrentName()		{ return CharSelect[CurrentSelect].Name;				}
	inline char *GetCurrentAttr()			{ return CharSelect[CurrentSelect].Attribute;			}
	inline const char *GetCurrentpSetup()	{ return CharSelect[CurrentSelect].pSetup;				}
	inline const char *GetCurrentEnv()		{ return CharSelect[CurrentSelect].Environment;			}
	inline const char *GetCurrentPS()		{ return CharSelect[CurrentSelect].PlayerStart;			}
	inline char *GetCurrentHud()			{ return CharSelect[CurrentSelect].Hud;					}
	inline float GetCurrentScale()			{ return CharSelect[CurrentSelect].ActorScale;			}
	inline geVec3d GetCurrentRotation()		{ return CharSelect[CurrentSelect].Rotation;			}
	inline float GetCurrentSpeed()			{ return CharSelect[CurrentSelect].AnimSpeed;			}
	inline float GetCurrentShadow()			{ return CharSelect[CurrentSelect].ShadowSize;			}
	inline float GetCurrentShadowAlpha()	{ return CharSelect[CurrentSelect].ShadowAlpha;			}
	inline char *GetCurrentShadowBitmap()	{ return CharSelect[CurrentSelect].ShadowBitmap;		}
	inline char *GetCurrentShadowAlphamap()	{ return CharSelect[CurrentSelect].ShadowAlphamap;		}
	inline geBoolean GetCurrentPShadows()	{ return CharSelect[CurrentSelect].UseProjectedShadows;	}
	inline geBoolean GetCurrentSShadows()	{ return CharSelect[CurrentSelect].UseStencilShadows;	}
	inline const char *GetCurrentStartLevel(){ return CharSelect[CurrentSelect].StartLevel;			}
	inline const char *GetCurrentWeapon()	{ return CharSelect[CurrentSelect].Weapon;				}
	inline float GetCurrentJumpSpeed()		{ return CharSelect[CurrentSelect].JumpSpeed;			}
	inline float GetCurrentMSpeed()			{ return CharSelect[CurrentSelect].Speed;				}
	inline float GetCurrentStep()			{ return CharSelect[CurrentSelect].StepHeight;			}
	inline GE_RGBA GetCurrentFillColor()	{ return CharSelect[CurrentSelect].FillColor;			}
	inline GE_RGBA GetCurrentAmbientColor()	{ return CharSelect[CurrentSelect].AmbientColor;		}
	inline geBoolean GetCurrentAmbientLightFromFloor() { return CharSelect[CurrentSelect].AmbientLightFromFloor; }

	inline float GetCurrentSlopeSlide()		{ return CharSelect[CurrentSelect].SlopeSlide; }
	inline float GetCurrentSlopeSpeed()		{ return CharSelect[CurrentSelect].SlopeSpeed; }
	inline CMIDIAudio *MIDIPlayer()			{ return theMIDIPlayer;	}
	inline float GetmVolLevel()				{ return mVolLevel;		}
	// playername selection
	inline const char *GetSelectedName()			{ return SelectedName;			}
	inline void SetSelectedName(const char *Name)	{ strcpy(SelectedName, Name);	}

private:
	void ClearVol();
	void ResetVol();
	void DisplayCrossHair();
	void FadeSet(int Dir, float Time);
	void MusicSet();
	void DoFade();
	void LoadMenuIni(const char *menuini);
	void Reset();

private:
	geBitmap	*ScreenBmp;
	geBitmap	*Backgrounds[NUM_BACKGROUNDS];
	geBitmap	*Titles[NUM_TITLES];
	geBitmap	*Images[NUM_IMAGES];
	Font       	MenuFont[NUM_FONTS];
	geBitmap	*Cursor;
	geBitmap	*Crosshair;
	geBitmap	*FCrosshair;
	geBitmap	*EmptySlotImage;
	geRect		M_CameraRect;
	geCamera 	*M_Camera;
	geRect		fRect;
	char		LevelName[256];
	int			MainBack;
	int			MainTitle;
	int			SelectBack;
	int			SelectTitle;
	int			DifficultBack;
	int			DifficultTitle;
	int			PlayerNameBack;
	int			PlayerNameTitle;
	char		SelectedName[255];
	bool		usenameselect;
	bool		bShowCursor;
	int			Screen;
	bool		ScrnWait;
	int			ingame;
	int			LoopOnce;
	bool		Fading;
	int			FadeDir;
	float		FadeTime;
	float		CurrentFadeTime;
	float		FadeAlpha;
	bool		MusicFade;
	float		OldMVol;
	float		MouseSen;
	bool		MouseReverse;
	bool		MouseFilter;
	bool		BoundingBox;
	bool		SEBoundingBox;
	bool		NoClip;
	POINT		Filter;
	int			Detail;
	bool		StencilShadows;
	geSound_Def *mouseclick;
	geSound_Def *keyclick;
	geSound_Def *slideclick;
	SoundList	*Bottom;
	char		music[256];
	int			musictype;
	StreamingAudio *m_Streams;
	LPDIRECTSOUND m_dsPtr;
	Selection	CharSelect[NUM_SELECT];
	int			MaxSelect;
	int			CurrentSelect;
	float		savetime;
	float		SavingTime;
	char		Savemsg[256];
	int			SaveFont;
	char		Loadmsg[256];
	int			LoadFont;
	CAnimGif	*Animation[NUM_ANIM];
	int			AnimCursor;
	DWORD		WinVol;
	bool		useselect;
	CMIDIAudio	*theMIDIPlayer;
	char		Loading[128];
	float		mVolLevel;
	int			DesignX;
	int			DesignY;
	int			HotX;
	int			HotY;
	float		TimeFade;
	bool		fontloaded;
	int			CurrentLanguage;
	char		MenuInis[5][FNAME_LENGHT];
	char		Convtxts[5][FNAME_LENGHT];
	char		Messagetxts[5][FNAME_LENGHT];
};


#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
