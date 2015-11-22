

#ifndef __RGF_CRFMENU_H_
#define __RGF_CRFMENU_H_

#define NUM_BACKGROUNDS  15
#define NUM_TITLES       15
#define NUM_IMAGES       150
#define NUM_FONTS        30

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
  int font_height;    // # of chars down bitmap
  Chardat dat[96];
  geBitmap *Bitmap;   // font bitmap
} Font;


typedef struct	SoundList
{
  SoundList		*next;
  SoundList		*prev;
  geSound		*Sound;
} SoundList;

// class declaration for CRFMenu
class CRFMenu : public CRGFComponent
{
public:
  CRFMenu();		// Default constructor
  ~CRFMenu();	
  int DoMenu(char *levelname);
  void GameLoop();
  void GameLevel();
  void MenuInitalize();
  void DisplaySplash(char *szSplashBMP);
  void FontRect(char *s, int FontNumber, int x, int y);
  void ScreenShot();
  geSound *PlaySoundDef(geSound_System *SoundS, geSound_Def *SoundDef, geFloat Volume, geFloat Pan, geFloat Frequency, geBoolean Loop);
  geBoolean StopSound(geSound_System *SoundS, geSound *Sound);
  void DeleteSound();
  char *GetLevelName()
  { return LevelName; }
  void SetLevelName(char *level)
  { strcpy(LevelName, level); }
  void SetInGame()
  { ingame = 1; }
  int GetInGame()
  { return ingame; }
  void SetMouseSen(float value)
  { MouseSen = value; }
  float GetMouseSen()
  { return MouseSen; }
  void SetMouseReverse(bool value)
  { MouseReverse = value; }
  bool GetMouseReverse()
  { return MouseReverse; }
  void SetMouseFilter(bool value)
  { MouseFilter = value; }
  bool GetMouseFilter()
  { return MouseFilter; }
  void SetBoundBox(bool value)
  { BoundingBox = value; }
  bool GetBoundBox()
  { return BoundingBox; }
  void SetSEBoundBox(bool value)
  { SEBoundingBox = value; }
  bool GetSEBoundBox()
  { return SEBoundingBox; }
  void SetNoClip(bool value)
  { NoClip = value; }
  bool GetNoClip()
  { return NoClip; }
  void SetFilter(POINT value)
  { Filter = value; }
  POINT GetFilter()
  { return Filter; }
  void SetShot(bool value)
  { ScrnWait = value; }
  bool GetShot()
  { return ScrnWait; }
  int GetMusicType()
  { return musictype; }
  int GetDetail()
  { return Detail; }
  void SetDetail(int value)
  { Detail = value; }
// Message
  int FontHeight(int FontNumber);
  int FontWidth(int FontNumber, char *s);
  geBitmap *GetCrossHair()
  { return Crosshair; }
  geBitmap *GetFCrossHair()
  { return FCrosshair; }
  void StopMenuMusic()
  { m_Streams->Stop(); }
private:
  int ProcessMenu(MenuItem *Menu, int Background_Number, int Title_Number);
  void ClearVol();
  void DisplayCrossHair();

//  void FontRect(char letter, int FontNumber);

  geBitmap	 *Backgrounds[NUM_BACKGROUNDS];
  geBitmap	 *Titles[NUM_TITLES];
  geBitmap	 *Images[NUM_IMAGES];
  Font       	 MenuFont[NUM_FONTS];
  geBitmap	 *Cursor;
  geBitmap	 *Crosshair;
  geBitmap	 *FCrosshair;
  geRect 	 M_CameraRect;
  geCamera 	 *M_Camera;
  geRect	 fRect;
  char		 LevelName[256];
  int		 MainBack;
  int		 MainTitle;
  int		 Screen;
  bool		 ScrnWait;
  int		 ingame;
  int		 LoopOnce;
  float		 MouseSen;
  bool		 MouseReverse;
  bool		 MouseFilter;
  bool		 BoundingBox;
  bool		 SEBoundingBox;
  bool		 NoClip;
  POINT		 Filter;
  int		 Detail;
  geSound_Def *mouseclick;
  geSound_Def *keyclick;
  geSound_Def *slideclick;
  SoundList	 *Bottom;
  char music[256];
  int musictype;
  StreamingAudio *m_Streams;
  LPDIRECTSOUND m_dsPtr;
};


#endif
