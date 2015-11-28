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
// changed RF063
#define NUM_ANIM		 70
// end change RF063
// changed QD Language Menu
#define FNAME_LENGHT	64
// end change QD

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
typedef struct Chardat
{
	int width;
	int x;
	int y;

} CharDat;


typedef struct Font
{
  int		font_height;	// # of chars down bitmap
  Chardat	dat[96];
  geBitmap	*Bitmap;		// font bitmap
  geBitmap	*WBitmap[96];

} Font;


typedef struct Selection
{
	char		Name[64];
	geBitmap	*Bitmap;		// image bitmap
	char		ActorName[64];
	float		ActorScale;
	geVec3d		Rotation;
	float		AnimSpeed;
	float		ShadowSize;
	GE_RGBA		FillColor;
	GE_RGBA		AmbientColor;
// changed QD 07/21/04
	geBoolean	AmbientLightFromFloor;
// end change
// changed QD 12/15/05
	float		ShadowAlpha;
	char		ShadowBitmap[64];
	char		ShadowAlphamap[64];
	geBoolean	UseProjectedShadows;
	geBoolean	UseStencilShadows;
	char		StartLevel[64];
	char		Weapon[64];
// end change
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
	SoundList	*next;
	SoundList	*prev;
	geSound		*Sound;
	geFloat		Volume;
	geFloat		Pan;
	geFloat		Frequency;

} SoundList;


// class declaration for CRFMenu
class CRFMenu : public CRGFComponent
{
public:
// changed QD 12/15/05
	//CRFMenu();		// Default constructor
	CRFMenu(const char *szStartLevel);
// end change
	~CRFMenu();

	int DoMenu(char *levelname);
	int ProcessMenu(MenuItem *Menu, int Background_Number, int Title_Number);
	void DrawBitmap(const geBitmap *Bitmap, geRect *Source, uint32 x, uint32 y);
// changed RF063
	void DoGame(bool editor);
	void ChangeCurrent(bool direction);
// end change RF063
	void GameLoop();
	void GameLevel();
	void MenuInitalize();
// changed QD Language Menu
	void ChangeMenuIni();
	char *GetConvtxts();
	char *GetMessagetxts();
// end change QD
// changed RF064
	void DisplaySplash();
	void LoadWBitmap();
	void UnLoadWBitmap();
	void WorldFontRect(char *s, int FontNumber, int x, int y, float Alpha);
// end change RF064

	void MFontRect(char *s, int FontNumber, int x, int y);
	void FontRect(char *s, int FontNumber, int x, int y);
	void ScreenShot();

	geSound *PlaySoundDef(geSound_System *SoundS, geSound_Def *SoundDef, geFloat Volume, geFloat Pan, geFloat Frequency, geBoolean Loop);
	geBoolean StopSound(geSound_System *SoundS, geSound *Sound);
	void DeleteSound();
	void StopMenuMusic();

	int SaveTo(FILE *SaveFD, bool type);
	int RestoreFrom(FILE *RestoreFD, bool type);

	inline char *GetLevelName()					{ return LevelName;			}
	inline void SetLevelName(char *level)		{ strcpy(LevelName, level); }
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
// changed QD Shadows
	inline void SetStencilShadows(bool value)	{ StencilShadows = value;	}
	inline bool GetStencilShadows()				{ return StencilShadows;	}
// end change
	// Message
	int FontHeight(int FontNumber);
	int FontWidth(int FontNumber, char *s);
	inline geBitmap *GetCrossHair()				{ return Crosshair;	}
	inline geBitmap *GetFCrossHair()			{ return FCrosshair;}
// changed RF063
	inline char *GetLoadmsg()					{ return Loadmsg;	}
	inline int GetLoadFont()					{ return LoadFont;	}
// end change RF063
// changed RF064
	void SetMusicVol(float vol);
	void SetmMusicVol(float vol);
// changed QD 12/15/05
	inline bool GetUseNameSelect()			{ return usenameselect;	}
// end change
	inline bool GetUseSelect()				{ return useselect;		}
	inline char *GetCurrentActor()			{ return CharSelect[CurrentSelect].ActorName;			}
	inline char *GetCurrentName()			{ return CharSelect[CurrentSelect].Name;				}
	inline char *GetCurrentAttr()			{ return CharSelect[CurrentSelect].Attribute;			}
	inline char *GetCurrentpSetup()			{ return CharSelect[CurrentSelect].pSetup;				}
	inline char *GetCurrentEnv()			{ return CharSelect[CurrentSelect].Environment;			}
	inline char *GetCurrentPS()				{ return CharSelect[CurrentSelect].PlayerStart;			}
	inline char *GetCurrentHud()			{ return CharSelect[CurrentSelect].Hud;					}
	inline float GetCurrentScale()			{ return CharSelect[CurrentSelect].ActorScale;			}
	inline geVec3d GetCurrentRotation()		{ return CharSelect[CurrentSelect].Rotation;			}
	inline float GetCurrentSpeed()			{ return CharSelect[CurrentSelect].AnimSpeed;			}
	inline float GetCurrentShadow()			{ return CharSelect[CurrentSelect].ShadowSize;			}
// changed QD 12/15/05
	inline float GetCurrentShadowAlpha()	{ return CharSelect[CurrentSelect].ShadowAlpha;			}
	inline char *GetCurrentShadowBitmap()	{ return CharSelect[CurrentSelect].ShadowBitmap;		}
	inline char *GetCurrentShadowAlphamap()	{ return CharSelect[CurrentSelect].ShadowAlphamap;		}
	inline geBoolean GetCurrentPShadows()	{ return CharSelect[CurrentSelect].UseProjectedShadows;	}
	inline geBoolean GetCurrentSShadows()	{ return CharSelect[CurrentSelect].UseStencilShadows;	}
	inline char *GetCurrentStartLevel()		{ return CharSelect[CurrentSelect].StartLevel;			}
	inline char *GetCurrentWeapon()			{ return CharSelect[CurrentSelect].Weapon;				}
// end change
	inline float GetCurrentJumpSpeed()		{ return CharSelect[CurrentSelect].JumpSpeed;			}
	inline float GetCurrentMSpeed()			{ return CharSelect[CurrentSelect].Speed;				}
	inline float GetCurrentStep()			{ return CharSelect[CurrentSelect].StepHeight;			}
	inline GE_RGBA GetCurrentFillColor()	{ return CharSelect[CurrentSelect].FillColor;			}
	inline GE_RGBA GetCurrentAmbientColor()	{ return CharSelect[CurrentSelect].AmbientColor;		}
// changed QD 07/21/04
	inline geBoolean GetCurrentAmbientLightFromFloor() { return CharSelect[CurrentSelect].AmbientLightFromFloor; }
// end change

	inline float GetCurrentSlopeSlide()		{ return CharSelect[CurrentSelect].SlopeSlide; }
	inline float GetCurrentSlopeSpeed()		{ return CharSelect[CurrentSelect].SlopeSpeed; }
	inline CMIDIAudio *MIDIPlayer()			{ return theMIDIPlayer;			}
	inline float GetmVolLevel()				{ return mVolLevel;				}
// end change RF064
// changed 12/15/05
	// playername selection
	inline char *GetSelectedName()			{ return SelectedName;			}
	inline void SetSelectedName(char *Name)	{ strcpy(SelectedName, Name);	}
// end change

private:
	void ClearVol();
	void ResetVol();
	void DisplayCrossHair();
	void FadeSet(int Dir, float Time);
	void MusicSet();
	void DoFade();
// changed QD Language Menu
	void LoadMenuIni(char *menuini);
	void Reset();
// end change QD

private:
	geBitmap	*ScreenBmp;
	geBitmap	*Backgrounds[NUM_BACKGROUNDS];
	geBitmap	*Titles[NUM_TITLES];
	geBitmap	*Images[NUM_IMAGES];
	Font       	MenuFont[NUM_FONTS];
	geBitmap	*Cursor;
	geBitmap	*Crosshair;
	geBitmap	*FCrosshair;
// changed QD 12/15/05
	geBitmap	*EmptySlotImage;
// end change
	geRect		M_CameraRect;
	geCamera 	*M_Camera;
	geRect		fRect;
	char		LevelName[256];
	int			MainBack;
	int			MainTitle;
// changed RF063
	int			SelectBack;
	int			SelectTitle;
// end change RF063
// changed RF064
	int			DifficultBack;
	int			DifficultTitle;
// end change RF064
// changed QD 12/15/05
	int			PlayerNameBack;
	int			PlayerNameTitle;
	char		SelectedName[255];
	bool		usenameselect;
// end change
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
// changed QD Shadows
	bool		StencilShadows;
// end change
	geSound_Def *mouseclick;
	geSound_Def *keyclick;
	geSound_Def *slideclick;
	SoundList	*Bottom;
	char		music[256];
	int			musictype;
	StreamingAudio *m_Streams;
	LPDIRECTSOUND m_dsPtr;
// changed RF063
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
// end change RF063
// changed RF064
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
// end change RF064
// changed QD Language Menu
	int			CurrentLanguage;
	char		MenuInis[5][FNAME_LENGHT];
	char		Convtxts[5][FNAME_LENGHT];
	char		Messagetxts[5][FNAME_LENGHT];
// end change QD
};


#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
