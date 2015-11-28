/************************************************************************************//**
 * @file CMenu.cpp
 * @brief Menu class implementation
 ****************************************************************************************/

//	Include the One True Header
#include "RabidFramework.h"
#include "Ram.h"

// start multiplayer
#include "HawkNL\\nl.h"
// end multiplayer

extern geBitmap *TPool_Bitmap(char *DefaultBmp, char *DefaultAlpha, char *BName, char *AName);

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// #define BLIT

// start add Nout - Show save game image
// changed QD 12/15/05
//static void CopyFile(const char *srcPath, const char *destPath);
static void CopyImageFile(const char *srcPath, const char *destPath);
// end change
void ScaleBitmapFromFile(const char *FileName, int dst_width, int dst_height);
// end add Nout

static int GetGammaPercent();
static void SetGamma(int percent);
static void SetDetail(int percent);
// changed QD Shadows
static void SetStencilShadows(int current);
// end change
static void SetSens(int percent);
static void SetVol(int percent);
static void SetmVol(int percent);
// Start Multiplayer
static void RunJoinGame();
static void RunHostGame();
// end Multiplayer
static void RunGame();
static void SetCDPlayer(int current);
static void SetReverse(int current);
static void SetFiltering(int current);
static void SetBBox(int current);
static void SetSEBBox(int current);
static void SetClipping(int current);
static void ResetAction();
static void SetSlot();
static void GetSlot();
// changed RF063
static void PrevChar();
static void NextChar();
// end change RF063
// changed QD 12/15/05
static void AcceptChar();
static void SetName();
static void ResetName();
// end change
// change QD Language Menu
static void ChangeMenu();
static void LanguageRadio(int ID);
// end change QD
// Start Multiplayer
char ServerIP[50];
//char PlayerName[50];
char IP[NL_MAX_STRING_LENGTH]="";
// end Multiplayer

#define GAMMAMIN	0.0f
#define GAMMAMAX	2.0f

#define MOUSEMIN	0.0005f
#define MOUSEMAX	0.005f

#define FADETIME	0.0f

//------------------------------
// Menu Item Types
//------------------------------
typedef enum
{
	END_LIST=0,
	CLICKABLE,
// begin add Nout - Show save game image
	SAVEGAMEIMAGE,
//end add Nout
	IMAGE,
// changed RF063
	CHARIMAGE,
	SLIDER,
	BOX,
// changed QD Language Menu
	RADIO,
// end change QD
	TEXT,
// Start Multiplayer
	TEXTEDIT,
// end Multiplayer
	REMAP,
	SCROLLBAR,
	LSBOX,
	EXIT_MENU,
// changed RF063
	CANCEL_MENU

} MENU_ITEM_TYPE;


//------------------------------
// Box On/Off
//------------------------------
typedef enum
{
	BOX_OFF=0,
	BOX_ON

} BOX_ON_OFF;

//---------------------------
// Menu Item Structure
//---------------------------
typedef struct MenuItem
{
	MENU_ITEM_TYPE Type; // type of item
	int X;               // X location on screen
	int Y;               // Y location on screen
	void *data;          // pointer to item data

} MenuItem;

//---------------------------
// Clickable menu item
//---------------------------
typedef struct Clickable
{
	int Width;			// width of graphic
	int Height;			// height of graphic
	int Image_Number;	// index of Images bitmap to use
	int Image_X;		// X location on bitmap of graphic
	int Image_Y;		// Y location on bitmap of graphic
	int Mover_X;		// X location of mouse-over graphic
	int Mover_Y;		// Y location of mouse-over graphic
	int Action;			// type of action to perform when clicked on

	// Action type 0 - go to another menu
	// Action type 2 - go to another menu if level is loaded (Save)
	int title;			// index of title to display
	int background;		// index of background to use
	MenuItem *Next;		// menu to go to

	// Action type 1 - execute function
	// Action type 3 - execute function and clear remap flag
	// Action type 4 - execute function if SaveBox line is not empty, exit menu when done (Load)
	void (*proc)();
	int LoopOnce;		// render screen once more before executing function but without cursor
// changed RF063
	int Animation;
	int AnimationOver;

} Clickable;

//--------------------------
// Image menu type
//--------------------------
typedef struct Image
{
	int Width;			// width of graphic
	int Height;			// height of graphic
	int Image_Number;	// index of Images bitmap to use
	int Image_X;		// X location on bitmap of graphic
	int Image_Y;		// Y location on bitmap of graphic
// changed RF063
	int Animation;

} Image;

//--------------------------
// Slider menu type
//--------------------------
typedef struct Slider
{
	int Width;					// width of graphic
	int Height;					// height of graphic
	int Image_Number;			// index of Images bitmap to use
	int Image_X;				// X location on bitmap of graphic
	int Image_Y;				// Y location on bitmap of graphic
	int Min_X;					// distance from origin of slider start
	int Max_X;					// distance from origin of slider end
	int On_the_Fly;				// call function each frame = 1
	int Current;				// inital slider location (Min_X <= >=Max_X)
	void (*proc)(int percent);	// function to call when slider moves
	int Animation;

} Slider;

//--------------------------
// Box menu type
//--------------------------
typedef struct Box
{
	int Width;					// width of graphic
	int Height;					// height of graphic
	int Image_Number;			// index of Images bitmap to use
	int Image_X;				// X location on bitmap of graphic
	int Image_Y;				// Y location on bitmap of graphic
	int Mover_X;				// X location of lit graphic
	int Mover_Y;				// Y location of lit graphic
	int Set_X;					// X location of set box graphic
	int Set_Y;					// Y location of set box graphic
	int Current;				// state of the box (on/off)
	void (*proc)(int current);	// function to call when current changes
// changed RF063
	int Animation;
	int AnimationOver;
	int AnimationLit;

} Box;

// changed QD Language Menu
//--------------------------
// Radio button menu type
//--------------------------
typedef struct Radio
{
	int Width;				// width of graphic
	int Height;				// height of graphic
	int Image_Number;		// index of Images bitmap to use
	int Image_X;			// X location on bitmap of graphic
	int Image_Y;			// Y location on bitmap of graphic
	int Mover_X;			// X location of lit graphic
	int Mover_Y;			// Y location of lit graphic
	int Set_X;				// X location of set box graphic
	int Set_Y;				// Y location of set box graphic
	int Current;			// state of the box (on/off)
	int ID;
	void (*proc)(int ID);	// function to call when current changes
	int Animation;
	int AnimationOver;
	int AnimationLit;

} Radio;
// end change QD

//--------------------------
// Text menu type
//--------------------------
typedef struct Text
{
	int Font;		// font to use
	char *text;		// text to display
	int ingame;		// display if in game only

} Text;

//--------------------------
// Save/Load menu info
//--------------------------
typedef struct Savedef
{
	char *text;		// text to display in line
	int empty;		// = 0 if unused
// changed QD 12/15/05
	geBitmap *SGImage;
// end change

} Savedef;

//-----------------------------
// Action codes and their names
//-----------------------------
typedef struct Keydef
{
	char *text;			// name of action
	ACTIONCODES	action;	// action value

} Keydef;

//-----------------------------
// Key codes and their names
//-----------------------------
typedef struct Keyname
{
	char *text;		// name of key
	KEYCODES key;	// value of key

} Keyname;

// start Multiplayer
//--------------------------
// TextEdit menu type
//--------------------------
typedef struct TextEdit
{
	int Font;			// font to use
	char text[255];		// text to display
	int Set_X;			// X location on screen
	int Set_Y;			// Y location on screen
	int Width;			// width max of the TextEdit
	Keyname *keyname;

} TextEdit;
// end Multiplayer

//--------------------------
// Scroll bar menu type
//--------------------------
typedef struct ScrollBar
{
	int Image_Number;	// index of Images bitmap to use
	int Up_X;			// offset to X of up arrow box
	int Up_Y;			// offset to Y of up arrow box
	int Up_Width;		// width of up arrow box
	int Up_Height;		// height of up arrow box
	int Dwn_X;			// offset to X of down arrow box
	int Dwn_Y;			// offset to Y of down arrow box
	int Dwn_Width;		// width of down arrow box
	int Dwn_Height;		// height of down arrow box
	int Up_Nor_X;		// X location of up normal bitmap of graphic
	int Up_Nor_Y;		// Y location of up normal bitmap of graphic
	int Dwn_Nor_X;		// X location of dwn normal bitmap of graphic
	int Dwn_Nor_Y;		// Y location of dwn normal bitmap of graphic
	int Up_Lit_X;		// X location of up lit bitmap of graphic
	int Up_Lit_Y;		// Y location of up lit bitmap of graphic
	int Up_Push_X;		// X location of up pushed bitmap of graphic
	int Up_Push_Y;		// Y location of up pushed bitmap of graphic
	int Dwn_Lit_X;		// X location of down lit bitmap of graphic
	int Dwn_Lit_Y;		// Y location of down lit bitmap of graphic
	int Dwn_Push_X;		// X location of down pushed bitmap of graphic
	int Dwn_Push_Y;		// Y location of down pushed bitmap of graphic
	int *Max;			// pointer to max value of scroll bar
	int *Current;		// pointer to current value of scroll bar
	int Show;			// maximum # of lines that show on screen
// changed RF063
	int AnimationUp;
	int AnimationUpOver;
	int AnimationUpPush;
	int AnimationDwn;
	int AnimationDwnOver;
	int AnimationDwnPush;

} ScrollBar;

//--------------------------
// Remap menu type
//--------------------------
typedef struct Remap
{
	int Width;			// width of graphic
	int Height;			// height of graphic
	int Image_Number;	// index of Images bitmap to use
	int Image_X;		// X location on bitmap of graphic
	int Image_Y;		// Y location on bitmap of graphic

	int Start_X;		// upper left corner offset of line text
	int Start_Y;
	int Step;			// height of line
	int Max_Show;		// max lines to display
	Keydef *keydef;		// lines to display
	Keyname *keyname;
	int Key_X;			// offset from Start_X to key code text

	int Start;			// first line to display
	int Max;			// max # of lines - is calculated
	int Current;		// current line highlighted
	int Corner_X;		// upper left corner of highlight bar
	int Corner_Y;
	int Click_Width;	// width of clickable area
	int Rev_X;			// offset of highlight bar in graphic
	int Rev_Y;
	int Rev_Width;		// width of highlight bar
	int Rev_Height;		// height of highlight bar
	int RevC_X;			// offset of entry highlight bar in graphic
	int RevC_Y;
	int RevC_Width;		// width of entry highlight bar
	int RevC_Height;	// height of entry highlight bar
	int Font;

} Remap;

//--------------------------
// load/save box menu type
//--------------------------
typedef struct LSBox
{
	int Width;			// width of graphic
	int Height;			// height of graphic
	int Image_Number;	// index of Images bitmap to use
	int Image_X;		// X location on bitmap of graphic
	int Image_Y;		// Y location on bitmap of graphic

	int Start_X;		// upper left corner offset of line text
	int Start_Y;
	int Step;			// height of line
	int Max_Show;		// max lines to display
	Savedef *text;		// lines to display

	int Start;			// first line to display
	int Max;			// max # of lines - is calculated
	int Current;		// current line highlighted
	int Corner_X;		// upper left corner of highlight bar
	int Corner_Y;
	int Click_Width;	// width of clickable area
	int Rev_X;			// offset of highlight bar in graphic
	int Rev_Y;
	int Rev_Width;		// width of highlight bar
	int Rev_Height;		// height of highlight bar
	int Font;

} LSBox;


// start change Nout - Show save game image
//-----------------------------
// structure of SaveGameImage
//-----------------------------
typedef struct SaveGameImage
{
	int X;					// X location on screen
	int Y;					// Y location on screen
	int Width;				// width of graphic
	int Height;				// height of graphic
	int Image_X;			// X location on bitmap of graphic
	int Image_Y;			// Y location on bitmap of graphic
	char EmptySlotImage[64];// name of bitmap of graphic to be displayed when the slot is empty

} SaveGameImage;
// end change Nout

//-----------------------------
// structure of menu titles
//-----------------------------
typedef struct MenuTitle
{
	int X;            // X location on screen
	int Y;            // Y location on screen
	int Width;        // width of graphic
	int Height;       // height of graphic
	int Image_Number; // index of Titles bitmap to use
	int Image_X;      // X location on bitmap of graphic
	int Image_Y;      // Y location on bitmap of graphic
// changed RF063
	int Animation;
// end change RF063

} MenuTitle;

//-------------------------------------
// Advanced Menu
//-------------------------------------
Clickable QuitAdvanced	= {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, -1, -1};
Box box1				= {0, 0, 0, 0, 0, 0, 0, 0, 0, BOX_OFF, NULL, -1, -1, -1};
Text xhair_text			= {0, "", 0};
Box box2				= {0, 0, 0, 0, 0, 0, 0, 0, 0, BOX_OFF, SetReverse, -1, -1, -1};
Text rev_text			= {0, "", 0};
Box box4				= {0, 0, 0, 0, 0, 0, 0, 0, 0, BOX_OFF, SetFiltering, -1, -1, -1};
Text filter_text		= {0, "", 0};
Image Sens_Img			= {0, 0, 0, 0, 0, -1};
Text sens_text			= {0, "", 0};
Slider Sens_Slide		= {0, 0, 0, 0, 0, 0, 1, 0, 0, SetSens, -1};

MenuItem AdvancedMenu[] =
{
	{EXIT_MENU, 0, 0, (void*)&QuitAdvanced},
	{BOX,       0, 0, (void*)&box1},
	{TEXT,      0, 0, (void*)&xhair_text},
	{BOX,       0, 0, (void*)&box2},
	{TEXT,      0, 0, (void*)&rev_text},
	{BOX,       0, 0, (void*)&box4},
	{TEXT,      0, 0, (void*)&filter_text},
	{TEXT,      0, 0, (void*)&sens_text},
	{IMAGE,     0, 0, (void*)&Sens_Img},
	{SLIDER,    0, 0, (void*)&Sens_Slide},
	{END_LIST,	0, 0, NULL}
};

//-------------------------------------
// Control Menu
//-------------------------------------
// names of all the currently defined keys
Keyname Rename[] =
{
	{"1", KEY_1},
	{"2", KEY_2},
	{"3", KEY_3},
	{"4", KEY_4},
	{"5", KEY_5},
	{"6", KEY_6},
	{"7", KEY_7},
	{"8", KEY_8},
	{"9", KEY_9},
	{"0", KEY_0},
	{"-", KEY_MINUS},
	{"Backspace", KEY_BACK},
	{"Tab", KEY_TAB},
	{"Q", KEY_Q},
	{"W", KEY_W},
	{"E", KEY_E},
	{"R", KEY_R},
	{"T", KEY_T},
	{"Y", KEY_Y},
	{"U", KEY_U},
	{"I", KEY_I},
	{"O", KEY_O},
	{"P", KEY_P},
	{"Return", KEY_RETURN},
	{"Ctrl", KEY_CONTROL},
	{"A", KEY_A},
	{"S", KEY_S},
	{"D", KEY_D},
	{"F", KEY_F},
	{"G", KEY_G},
	{"H", KEY_H},
	{"J", KEY_J},
	{"K", KEY_K},
	{"L", KEY_L},
	{"Tilde", KEY_TILDE},
	{"Shift", KEY_SHIFT},
	{"Z", KEY_Z},
	{"X", KEY_X},
	{"C", KEY_C},
	{"V", KEY_V},
	{"B", KEY_B},
	{"N", KEY_N},
	{"M", KEY_M},
	{".", KEY_PERIOD},
	{"/", KEY_SLASH},
	{"Space", KEY_SPACE},
	{"Alt", KEY_ALT},
	{"+", KEY_PLUS},
	{"Ins", KEY_INSERT},
	{"Home", KEY_HOME},
	{"PgUp", KEY_PAGEUP},
	{"Del", KEY_DELETE},
	{"End", KEY_END},
	{"PgDn", KEY_PAGEDOWN},
	{"Up Arrow", KEY_UP},
	{"Down Arrow", KEY_DOWN},
	{"Left Arrow", KEY_LEFT},
	{"Right Arrow", KEY_RIGHT},
	{"F1", KEY_F1},
	{"F2", KEY_F2},
	{"F3", KEY_F3},
	{"F4", KEY_F3},
	{"F5", KEY_F5},
	{"F6", KEY_F6},
	{"F7", KEY_F7},
	{"F8", KEY_F8},
	{"F9", KEY_F9},
	{"F10", KEY_F10},
	{"F11", KEY_F11},
	{"F12", KEY_F12},
	{"PrtScrn", KEY_SYSRQ},
	{"Left Button", KEY_LBUTTON},
	{"Right Button", KEY_RBUTTON},
	{"Middle Button", KEY_MBUTTON},
	{"[", KEY_LBRACKET},
	{"]", KEY_RBRACKET},
	{"=", KEY_EQUAL},
	{"\\", KEY_BACKSLASH},
	{";", KEY_SEMICOLON},
	{",", KEY_COMMA},
	{"'", KEY_APOSTROPHE},
	{"<empty>", KEY_MAXIMUM},
	{NULL}
};

// start Multiplayer
Keyname TextEditKeys[] =
{
	{"1", KEY_1},
	{"2", KEY_2},
	{"3", KEY_3},
	{"4", KEY_4},
	{"5", KEY_5},
	{"6", KEY_6},
	{"7", KEY_7},
	{"8", KEY_8},
	{"9", KEY_9},
	{"0", KEY_0},
	{"-", KEY_MINUS},
	{" ", KEY_TAB},
	{"Q", KEY_Q},
	{"W", KEY_W},
	{"E", KEY_E},
	{"R", KEY_R},
	{"T", KEY_T},
	{"Y", KEY_Y},
	{"U", KEY_U},
	{"I", KEY_I},
	{"O", KEY_O},
	{"P", KEY_P},
	{"A", KEY_A},
	{"S", KEY_S},
	{"D", KEY_D},
	{"F", KEY_F},
	{"G", KEY_G},
	{"H", KEY_H},
	{"J", KEY_J},
	{"K", KEY_K},
	{"L", KEY_L},
	{"~", KEY_TILDE},
	{"Z", KEY_Z},
	{"X", KEY_X},
	{"C", KEY_C},
	{"V", KEY_V},
	{"B", KEY_B},
	{"N", KEY_N},
	{"M", KEY_M},
	{".", KEY_PERIOD},
	{"/", KEY_SLASH},
	{" ", KEY_SPACE},
	{"+", KEY_PLUS},
	{"F1", KEY_F1},
	{"F2", KEY_F2},
	{"F3", KEY_F3},
	{"F4", KEY_F3},
	{"F5", KEY_F5},
	{"F6", KEY_F6},
	{"F7", KEY_F7},
	{"F8", KEY_F8},
	{"F9", KEY_F9},
	{"F10", KEY_F10},
	{"F11", KEY_F11},
	{"F12", KEY_F12},
	{"[", KEY_LBRACKET},
	{"]", KEY_RBRACKET},
	{"=", KEY_EQUAL},
	{"\\", KEY_BACKSLASH},
	{".", KEY_SEMICOLON},
	{".", KEY_COMMA},
    {"'", KEY_APOSTROPHE},
	{"0", KEY_NUMPAD0},
    {"1", KEY_NUMPAD1},
    {"2", KEY_NUMPAD2},
    {"3", KEY_NUMPAD3},
    {"4", KEY_NUMPAD4},
    {"5", KEY_NUMPAD5},
    {"6", KEY_NUMPAD5},
    {"7", KEY_NUMPAD7},
    {"8", KEY_NUMPAD8},
    {"9", KEY_NUMPAD9},
    {".", KEY_DECIMAL},
	{"", KEY_MAXIMUM},
	{NULL}
};
// end Multiplayer

// names of all the currently defined actions
Keydef Redef[50];

//-------------------------------------
// Control Menu
//-------------------------------------
Clickable QuitControl	= {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, -1, -1};
Clickable AdvancedItem	= {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, AdvancedMenu, NULL, 0, -1, -1};
Remap KeyMap			= {0, 0, 0, 0, 0, 0, 0, 0, 0, Redef, Rename, 0, 0, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
ScrollBar KeyMapBar		= {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
						   0, 0, 0, 0, &(KeyMap.Max), &(KeyMap.Start), 0, -1, -1, -1, -1, -1, -1};
Clickable ResetKey		= {0, 0, 0, 0, 0, 0, 0, 3, 0, 0, NULL, ResetAction, 0, -1, -1};

MenuItem ControlMenu[] =
{
	{CLICKABLE, 0, 0, (void*)&AdvancedItem},
	{EXIT_MENU, 0, 0, (void*)&QuitControl},
	{REMAP,     0, 0, (void*)&KeyMap},
	{SCROLLBAR, 0, 0, (void*)&KeyMapBar},
	{CLICKABLE, 0, 0, (void*)&ResetKey},
	{END_LIST,	0, 0, NULL}
};

//-------------------------------------
// Video Menu
//-------------------------------------
Clickable QuitVideo = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, -1, -1};
Text gam_text		= {0, "", 0};
Image Gamma_Img     = {0, 0, 0, 0, 0, -1};
Slider Gamma_Slide  = {0, 0, 0, 0, 0, 0, 1, 0, 0, SetGamma, -1};
Text det_text		= {0, "", 0};
Image Detail_Img    = {0, 0, 0, 0, 0, -1};
Slider Detail_Slide = {0, 0, 0, 0, 0, 0, 1, 0, 0, SetDetail, -1};
// changed QD Shadows
Box ShadowBox		= {0, 0, 0, 0, 0, 0, 0, 0, 0, BOX_OFF, SetStencilShadows, -1, -1, -1};
Text shadow_text	= {0, "", 0};

MenuItem VideoMenu[] =
{
	{EXIT_MENU, 0, 0, (void*)&QuitVideo},
	{TEXT,      0, 0, (void*)&gam_text},
	{IMAGE,     0, 0, (void*)&Gamma_Img},
	{SLIDER,    0, 0, (void*)&Gamma_Slide},
	{TEXT,      0, 0, (void*)&det_text},
	{IMAGE,     0, 0, (void*)&Detail_Img},
	{SLIDER,    0, 0, (void*)&Detail_Slide},
	{BOX,		0, 0, (void*)&ShadowBox},
	{TEXT,		0, 0, (void*)&shadow_text},
	{END_LIST,	0, 0, NULL}
};
// end change

//-------------------------------------
// Audio Menu
//-------------------------------------
Clickable QuitAudio = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, -1, -1};
Text vol_text		= {0, "", 0};
Image Vol_Img		= {0, 0, 0, 0,  0, -1};
Slider Vol_Slide	= {0, 0, 0, 0, 0, 0, 1, 0, 0, SetVol, -1};
Text mvol_text		= {0, "", 0};
Image mVol_Img		= {0, 0, 0, 0,  0, -1};
Slider mVol_Slide	= {0, 0, 0, 0, 0, 0, 1, 0, 0, SetmVol, -1};
Box box0			= {0,  0, 0, 0, 0, 0, 0, 0, 0, BOX_OFF, SetCDPlayer, -1, -1, -1};
Text cd_text		= {0, "", 0};

MenuItem AudioMenu[] =
{
	{EXIT_MENU, 0, 0, (void*)&QuitAudio},
	{TEXT,      0, 0, (void*)&vol_text},
	{IMAGE,     0, 0, (void*)&Vol_Img},
	{SLIDER,    0, 0, (void*)&Vol_Slide},
	{TEXT,      0, 0, (void*)&mvol_text},
	{IMAGE,     0, 0, (void*)&mVol_Img},
	{SLIDER,    0, 0, (void*)&mVol_Slide},
	{BOX,       0, 0, (void*)&box0},
	{TEXT,      0, 0, (void*)&cd_text},
	{END_LIST,	0, 0, NULL}
};

//-------------------------------------
// Debug Menu
//-------------------------------------
Clickable QuitDebug = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, -1, -1};
Box box5			= {0, 0, 0, 0, 0, 0, 0, 0, 0, BOX_OFF, NULL, -1, -1, -1};
Text debug_text		= {0, "", 0};
Box box6			= {0, 0, 0, 0, 0, 0, 0, 0, 0, BOX_OFF, NULL, -1, -1, -1};
Text fps_text		= {0, "", 0};
Box box7			= {0, 0, 0, 0, 0, 0, 0, 0, 0, BOX_OFF, SetClipping, -1, -1, -1};
Text clip_text		= {0, "", 0};
Box box8			= {0, 0, 0, 0, 0, 0, 0, 0, 0, BOX_OFF, SetBBox, -1, -1, -1};
Text bb_text		= {0, "", 0};
Box box9			= {0, 0, 0, 0, 0, 0, 0, 0, 0, BOX_OFF, SetSEBBox, -1, -1, -1};
Text sebb_text		= {0, "", 0};

MenuItem DebugMenu[] =
{
	{EXIT_MENU, 0, 0, (void*)&QuitDebug},
	{BOX,       0, 0, (void*)&box5},
	{TEXT,      0, 0, (void*)&debug_text},
	{BOX,       0, 0, (void*)&box6},
	{TEXT,      0, 0, (void*)&fps_text},
	{BOX,       0, 0, (void*)&box7},
	{TEXT,      0, 0, (void*)&clip_text},
	{BOX,       0, 0, (void*)&box8},
	{TEXT,      0, 0, (void*)&bb_text},
	{BOX,       0, 0, (void*)&box9},
	{TEXT,      0, 0, (void*)&sebb_text},
	{END_LIST,	0, 0, NULL}
};

// changed QD Language Menu
//-------------------------------------
// Language Menu
//-------------------------------------
Clickable QuitLanguage	= {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, ChangeMenu, 0, -1, -1};
Radio boxL5				= {0, 0, 0, 0, 0, 0, 0, 0, 0, BOX_OFF, 4, LanguageRadio, -1, -1, -1};
Text L5text				= {-1, "", 0};
Radio boxL4				= {0, 0, 0, 0, 0, 0, 0, 0, 0, BOX_OFF, 3, LanguageRadio, -1, -1, -1};
Text L4text				= {-1, "", 0};
Radio boxL3				= {0, 0, 0, 0, 0, 0, 0, 0, 0, BOX_OFF, 2, LanguageRadio, -1, -1, -1};
Text L3text				= {-1, "", 0};
Radio boxL2				= {0, 0, 0, 0, 0, 0, 0, 0, 0, BOX_OFF, 1, LanguageRadio, -1, -1, -1};
Text L2text				= {-1, "", 0};
Radio boxL1				= {0, 0, 0, 0, 0, 0, 0, 0, 0, BOX_OFF, 0, LanguageRadio, -1, -1, -1};
Text L1text				= {-1, "", 0};

MenuItem LanguageMenu[] =
{
	{EXIT_MENU, 0, 0, (void*)&QuitLanguage},
	{RADIO,     0, 0, (void*)&boxL1},
	{TEXT,      0, 0, (void*)&L1text},
	{RADIO,     0, 0, (void*)&boxL2},
	{TEXT,      0, 0, (void*)&L2text},
	{RADIO,     0, 0, (void*)&boxL3},
	{TEXT,      0, 0, (void*)&L3text},
	{RADIO,     0, 0, (void*)&boxL4},
	{TEXT,      0, 0, (void*)&L4text},
	{RADIO,     0, 0, (void*)&boxL5},
	{TEXT,      0, 0, (void*)&L5text},
	{END_LIST,	0, 0, NULL}
};
// end change QD

//-------------------------------------
// Options Menu
//-------------------------------------
Clickable QuitOption	= {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, -1, -1};
Clickable AudioItem		= {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, AudioMenu, NULL, 0, -1, -1};
Clickable VideoItem		= {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, VideoMenu, NULL, 0, -1, -1};
Clickable ControlItem	= {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ControlMenu, NULL, 0, -1, -1};
Clickable DebugItem		= {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, DebugMenu, NULL, 0, -1, -1};
// changed QD Language Menu
Clickable LanguageItem	= {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, LanguageMenu, NULL, 0, -1, -1};

MenuItem OptionMenu[] =
{
	{CLICKABLE, 0, 0, (void*)&AudioItem},
	{CLICKABLE, 0, 0, (void*)&VideoItem},
	{CLICKABLE, 0, 0, (void*)&ControlItem},
	{CLICKABLE, 0, 0, (void*)&LanguageItem},
	{CLICKABLE, 0, 0, (void*)&DebugItem},
	{EXIT_MENU, 0, 0, (void*)&QuitOption},
	{END_LIST, 0, 0, NULL}
};
// end change

//---------------------
// saved game default
//---------------------
// changed RF063
Savedef SavedGame[16];
// end change RF063

// begin add Nout - Show save game image
SaveGameImage SaveScreen = {0, 0, 0, 0, 0, 0, NULL};
geBoolean LoadSGImage;
// end add Nout

//-------------------------------------
// Save Game Menu
//-------------------------------------
Clickable QuitSave	= {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, -1, -1};
Clickable SaveSlot	= {0, 0, 0, 0, 0, 30, 0, 1, 0, 0, NULL, SetSlot, 0, -1, -1};
LSBox SaveBox		= {0, 0, 0, 0, 0, 0, 0, 0, 0, SavedGame, 0, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0};
ScrollBar SaveBar	= {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					   0, 0, 0, 0, &(SaveBox.Max), &(SaveBox.Start), 0, -1, -1, -1, -1, -1, -1};

MenuItem SaveMenu[] =
{
	{EXIT_MENU,		0, 0, (void*)&QuitSave},
	{CLICKABLE,		0, 0, (void*)&SaveSlot},
	{LSBOX,			0, 0, (void*)&SaveBox},
	{SCROLLBAR,		0, 0, (void*)&SaveBar},
// begin add Nout - Show save game image
//	{SAVEGAMEIMAGE, 0, 0, (void*)&SaveScreen},
// end add Nout
	{END_LIST,		0, 0, NULL}
};

//-------------------------------------
// Load Game Menu
//-------------------------------------
Clickable QuitLoad	= {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, -1, -1};
Clickable LoadSlot	= {0, 0, 0, 0, 0, 0, 0, 4, 0, 0, NULL, GetSlot, 1, -1, -1};
LSBox LoadBox		= {0, 0, 0, 0, 0, 0, 0, 0, 0, SavedGame, 0, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0};
ScrollBar LoadBar	= {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					   0, 0, 0, 0, &(LoadBox.Max), &(LoadBox.Start), 0, -1, -1, -1, -1, -1, -1};

MenuItem LoadGMenu[] =
{
	{EXIT_MENU, 0, 0, (void*)&QuitLoad},
	{CLICKABLE, 0, 0, (void*)&LoadSlot},
	{LSBOX,     0, 0, (void*)&LoadBox},
	{SCROLLBAR, 0, 0, (void*)&LoadBar},
	{END_LIST,	0, 0, NULL}
};


//-------------------------------------
// Credit Menu
//-------------------------------------
Clickable QuitCredit = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, -1, -1};
Image Credit_Img     = {0, 0, 0, 0, 0, -1};

MenuItem CreditMenu[] =
{
	{EXIT_MENU, 0, 0, (void*)&QuitCredit},
	{IMAGE,		0, 0, (void*)&Credit_Img},
	{END_LIST,	0, 0, NULL}
};

//-------------------------------------
// Mods Menu
//-------------------------------------
Clickable QuitMods = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, -1, -1};

MenuItem ModsMenu[] =
{
	{EXIT_MENU, 0, 0, (void*)&QuitMods},
	{END_LIST,	0, 0, NULL}
};

//-------------------------------------
// Multiplayer Game Menu
//-------------------------------------
// start Multiplayer
//
// Join Game menu
//
Text IPAdd_Text				= {0, "", 0};
TextEdit IPAddress			= {0, "xxx.xxx.xxx.xxx", 10, 10, 100, TextEditKeys};
Clickable LaunchJoinGame	= {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, NULL, RunJoinGame, 1, -1, -1};
Clickable QuitJoinGame		= {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, -1, -1};
Text MultiplayerHelp_Text	= {0, "", 0};

MenuItem JoinGameMenu[] =
{
	{TEXT,		0, 0, (void*)&IPAdd_Text},
	{TEXTEDIT,	0, 0, (void*)&IPAddress},
	{CLICKABLE, 0, 0, (void*)&LaunchJoinGame},
	{EXIT_MENU, 0, 0, (void*)&QuitJoinGame},
	{TEXT,		0, 0, (void*)&MultiplayerHelp_Text},
	{END_LIST,	0, 0, NULL}
};

//
// Multiplayer menu
//
Clickable HostNewGame			= {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, NULL, RunHostGame, 1, -1, -1};
Clickable JoinMultiplayerGame	= {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, JoinGameMenu, NULL, 0, -1, -1};
Clickable QuitMulti				= {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, -1, -1};
Text PlayerIP_Text				= {0, "Your IP :", 0};
Text PlayerIP1					= {0, IP, 0};

MenuItem MultiMenu[] =
{
	{CLICKABLE, 0, 0, (void*)&HostNewGame},
	{CLICKABLE, 0, 0, (void*)&JoinMultiplayerGame},
	{EXIT_MENU, 0, 0, (void*)&QuitMulti},
	{TEXT,		0, 0, (void*)&PlayerIP_Text},
	{TEXT,		0, 0, (void*)&PlayerIP1},
	{END_LIST,	0, 0, NULL}
};
// end Multiplayer

// changed RF063
//-------------------------------------
// Character Selection Menu
//-------------------------------------
// changed QD 12/15/05
//Clickable AcceptSelect	= {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 1, -1, -1};
Clickable AcceptSelect	= {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, AcceptChar, 1, -1, -1};
// end change
Clickable CancelSelect	= {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 1, -1, -1};
Clickable PrevSelect	= {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, NULL, PrevChar, 0, -1, -1};
Clickable NextSelect	= {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, NULL, NextChar, 0, -1, -1};

MenuItem SelectMenu[] =
{
	{EXIT_MENU,		0, 0, (void*)&AcceptSelect},
	{CANCEL_MENU,	0, 0, (void*)&CancelSelect},
	{CLICKABLE,		0, 0, (void*)&PrevSelect},
	{CLICKABLE,		0, 0, (void*)&NextSelect},
	{CHARIMAGE,		0, 0, NULL},
	{END_LIST,		0, 0, NULL}
};
// end change RF063

// changed QD 12/15/05
//-------------------------------------
// PlayerName Menu
//-------------------------------------
Clickable AcceptName	= {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, NULL, SetName, 1, -1, -1};
Clickable CancelName	= {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 1, -1, -1};
TextEdit PlayerName		= {0, "", 10, 10, 100, TextEditKeys};
Clickable DefaultName	= {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, NULL, ResetName, 1, -1, -1};

MenuItem PlayerNameMenu[] =
{
	{EXIT_MENU,		0, 0, (void*)&AcceptName},
	{CANCEL_MENU,	0, 0, (void*)&CancelName},
	{TEXTEDIT,		0, 0, (void*)&PlayerName},
	{CLICKABLE,		0, 0, (void*)&DefaultName},
	{END_LIST,		0, 0, NULL}
};
// end change

//-------------------------------------
// Difficulty Selection Menu
//-------------------------------------
Clickable Difficultlow	= {0, 0, 0, 0, 0, 0, 0, 10, 0, 0, NULL, NULL, 1, -1, -1};
Clickable Difficultmid	= {0, 0, 0, 0, 0, 0, 0, 11, 0, 0, NULL, NULL, 1, -1, -1};
Clickable Difficulthi	= {0, 0, 0, 0, 0, 0, 0, 12, 0, 0, NULL, NULL, 1, -1, -1};

MenuItem DifficultMenu[] =
{
	{EXIT_MENU, 0, 0, (void*)&Difficultlow},
	{EXIT_MENU, 0, 0, (void*)&Difficultmid},
	{EXIT_MENU, 0, 0, (void*)&Difficulthi},
	{END_LIST,	0, 0, NULL}
};

//-------------------------------------
// Main Menu
//-------------------------------------
Clickable NewGame		= {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, NULL, RunGame, 1, -1, -1};
Clickable MultiPlayer	= {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, MultiMenu, NULL, 0, -1, -1};
Clickable LoadGame		= {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, LoadGMenu, NULL, 0, -1, -1};
Clickable SaveGame		= {0, 0, 0, 0, 0, 0, 0, 2, 0, 0, SaveMenu, NULL, 0, -1, -1};
Clickable Options		= {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, OptionMenu, NULL, 0, -1, -1};
Clickable Credits		= {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CreditMenu, NULL, 0, -1, -1};
Clickable Mods			= {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ModsMenu, NULL, 0, -1, -1};
Clickable QuitGame		= {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 1, -1, -1};
Text return_text		= {0, "", 1};

MenuItem MainMenu[] =
{
	{CLICKABLE, 0, 0, (void*)&NewGame},
	{CLICKABLE, 0, 0, (void*)&MultiPlayer},
	{CLICKABLE, 0, 0, (void*)&LoadGame},
	{CLICKABLE, 0, 0, (void*)&SaveGame},
	{CLICKABLE, 0, 0, (void*)&Options},
	{CLICKABLE, 0, 0, (void*)&Credits},
	{EXIT_MENU, 0, 0, (void*)&QuitGame},
	{TEXT,      0, 0, (void*)&return_text},
	{CLICKABLE, 0, 0, (void*)&Mods},
	{END_LIST,	0, 0, NULL}
};

//-------------------------------------
// Titles
//-------------------------------------
// changed RF063
MenuTitle MTitles[NUM_TITLES];
char errortext[256];
// end change RF063

/* ------------------------------------------------------------------------------------ */
//	FirstToken
/* ------------------------------------------------------------------------------------ */
char *FirstToken(char *string1, char *string2)
{
	return strtok(string1, string2);
}

/* ------------------------------------------------------------------------------------ */
//	NextToken
/* ------------------------------------------------------------------------------------ */
char *NextToken()
{
	char *temp;
	temp = strtok(NULL," \n");

	if(temp == (char*)NULL)
	{
		char szBug[256];
		sprintf(szBug, "*ERROR* File %s - Line %d: Missing menu token in line\n %s",
				__FILE__, __LINE__, errortext);
		CCD->ReportError(szBug, false);
		exit(-100);
	}

	return temp;
}

/* ------------------------------------------------------------------------------------ */
//	NextString
/* ------------------------------------------------------------------------------------ */
char *NextString()
{
	char *temp;
	temp = strtok(NULL,"\n");

	if(temp == (char*)NULL)
	{
		char szBug[256];
		sprintf(szBug, "*ERROR* File %s - Line %d: Missing menu token in line\n %s",
				__FILE__, __LINE__, errortext);
		CCD->ReportError(szBug, false);
		exit(-100);
	}

	return temp;
}

/* ------------------------------------------------------------------------------------ */
//	NextValue
/* ------------------------------------------------------------------------------------ */
int NextValue()
{
	char *temp;
	temp = NextToken();
	return atoi(temp);
}

/* ------------------------------------------------------------------------------------ */
//	NextFont
/* ------------------------------------------------------------------------------------ */
int NextFont()
{
	char *szArg;
	int index;
	szArg = NextToken();

	if(!stricmp(szArg,"FONT1"))
		index = FONT1;
	else if(!stricmp(szArg,"FONT2"))
		index = FONT2;
	else if(!stricmp(szArg,"FONT3"))
		index = FONT3;
	else if(!stricmp(szArg,"FONT4"))
		index = FONT4;
	else if(!stricmp(szArg,"FONT5"))
		index = FONT5;
	else if(!stricmp(szArg,"FONT6"))
		index = FONT6;
	else if(!stricmp(szArg,"FONT7"))
		index = FONT7;
	else if(!stricmp(szArg,"FONT8"))
		index = FONT8;
	else if(!stricmp(szArg,"FONT9"))
		index = FONT9;
	else if(!stricmp(szArg,"FONT10"))
		index = FONT10;
	else if(!stricmp(szArg,"FONT11"))
		index = FONT11;
	else if(!stricmp(szArg,"FONT12"))
		index = FONT12;
	else if(!stricmp(szArg,"FONT13"))
		index = FONT13;
	else if(!stricmp(szArg,"FONT14"))
		index = FONT14;
	else if(!stricmp(szArg,"FONT15"))
		index = FONT15;
	else if(!stricmp(szArg,"FONT16"))
		index = FONT16;
	else if(!stricmp(szArg,"FONT17"))
		index = FONT17;
	else if(!stricmp(szArg,"FONT18"))
		index = FONT18;
	else if(!stricmp(szArg,"FONT19"))
		index = FONT19;
	else if(!stricmp(szArg,"FONT20"))
		index = FONT20;
	else if(!stricmp(szArg,"FONT21"))
		index = FONT21;
	else if(!stricmp(szArg,"FONT22"))
		index = FONT22;
	else if(!stricmp(szArg,"FONT23"))
		index = FONT23;
	else if(!stricmp(szArg,"FONT24"))
		index = FONT24;
	else if(!stricmp(szArg,"FONT25"))
		index = FONT25;
	else if(!stricmp(szArg,"FONT26"))
		index = FONT26;
	else if(!stricmp(szArg,"FONT27"))
		index = FONT27;
	else if(!stricmp(szArg,"FONT28"))
		index = FONT28;
	else if(!stricmp(szArg,"FONT29"))
		index = FONT29;
	else
	{
		char szBug[256];
		sprintf(szBug, "*ERROR* File %s - Line %d: Bad font size name %s",
				__FILE__, __LINE__, szArg);
		CCD->ReportError(szBug, false);
		exit(-100);
	}

	return index;
}

/* ------------------------------------------------------------------------------------ */
//	Constructor
/* ------------------------------------------------------------------------------------ */
// changed QD 12/15/05
//CRFMenu::CRFMenu()
CRFMenu::CRFMenu(const char *szStartLevel)
// end change
{
	int i;

// start multiplayer

// 08.05.2004 - begin change gekido
	CCD->ReportError("Initializing Network...", false);
// 08.05.2004 - end change gekido

	if(CCD->GetNetwork())
	{
		if(nlSelectNetwork(NL_IP))
		{
			NLsocket serversock = nlOpen(0, NL_RELIABLE_PACKETS);

			if(serversock != NL_INVALID)
			{
				NLaddress   addr;
				nlGetLocalAddr(serversock, &addr);
				nlAddrToString(&addr, IP);
				nlClose(serversock);
			}
		}
	}
// end multiplayer

// changed QD 07/15/06
	theMIDIPlayer	= NULL;
	m_Streams		= NULL;
// end change QD 07/15/06
	Bottom			= (SoundList*)NULL;
	Screen			= 0;
	ScrnWait		= false;
	savetime		= 0.0f;
	musictype		= -1;
	mVolLevel		= 0.5f;
	DesignX			= 640;
	DesignY			= 480;
	HotX			= 0;
	HotY			= 0;
	TimeFade		= FADETIME;
	MusicFade		= false;
	fontloaded		= false;
// changed RF064
	theMIDIPlayer	= NULL;
	strcpy(Loading, "menu\\loading.bmp");
// end change RF064
// MENUFIX
	ingame			= 0;
// changed QD Language Menu
	CurrentLanguage = 0;

	for(i=0; i<5; i++)
	{
		strcpy(MenuInis[i],		CCD->MenuIni());
		strcpy(Convtxts[i],		"conversation.txt");
		strcpy(Messagetxts[i],	"message.txt");
	}


	return_text.text			= strdup("Press ESC to Return to Game");
	PlayerIP_Text.text			= strdup("Your IP :");
	MultiplayerHelp_Text.text	= strdup("Enter a valid server ip, press ENTER and click on Join game");
	IPAdd_Text.text				= strdup("Server IP Address :");
	sebb_text.text				= strdup("Entity Bounding Box");
	bb_text.text				= strdup("Player Bounding Box");
	clip_text.text				= strdup("No Clipping");
	fps_text.text				= strdup("Frame Rate");
	debug_text.text				= strdup("Debug Info");
	cd_text.text				= strdup("Play CD Music");
	mvol_text.text				= strdup("Music Volume");
	vol_text.text				= strdup("Sound Volume");
	det_text.text				= strdup("Detail Level");
	gam_text.text				= strdup("Gamma");
	sens_text.text				= strdup("Mouse sensitivity");
	filter_text.text			= strdup("Mouse filter");
	rev_text.text				= strdup("Reverse mouse");
	xhair_text.text				= strdup("Crosshair");
// changed QD Shadows
	shadow_text.text			= strdup("Enable Stencil Shadows");
// end change QD Shadows

	L1text.text = NULL;
	L2text.text = NULL;
	L3text.text = NULL;
	L4text.text = NULL;
	L5text.text = NULL;

// 08.05.2004 - begin change gekido
	CCD->ReportError("Loading Menu.ini...", false);
// 08.05.2004 - end change gekido

	LoadMenuIni(CCD->MenuIni());
// end change QD Language Menu

// changed RF063
	for(i=0; i<16; i++)
	{
		SavedGame[i].empty = 0;
		SavedGame[i].text = NULL;
// changed QD 12/15/05
		SavedGame[i].SGImage = NULL;
// end change
	}

// changed QD 12/15/05
	// Note: SavedGame[15].text must be NULL,
	// ..otherwise SaveBox.Max won't be calculated correctly
	SavedGame[0].text	= strdup("<Slot 1>"	);
	SavedGame[1].text	= strdup("<Slot 2>"	);
	SavedGame[2].text	= strdup("<Slot 3>"	);
	SavedGame[3].text	= strdup("<Slot 4>"	);
	SavedGame[4].text	= strdup("<Slot 5>"	);
	SavedGame[5].text	= strdup("<Slot 6>"	);
	SavedGame[6].text	= strdup("<Slot 7>"	);
	SavedGame[7].text	= strdup("<Slot 8>"	);
	SavedGame[8].text	= strdup("<Slot 9>"	);
	SavedGame[9].text	= strdup("<Slot 10>");
	SavedGame[10].text	= strdup("<Slot 11>");
	SavedGame[11].text	= strdup("<Slot 12>");
	SavedGame[12].text	= strdup("<Slot 13>");
	SavedGame[13].text	= strdup("<Slot 14>");
	SavedGame[14].text	= strdup("<Slot 15>");

	strcpy(PlayerName.text, CCD->GetDefaultPlayerName());
//end change

// changed RF064
	if(CCD->GetCSelect())
	{
// 08.05.2004 - begin change gekido
		CCD->ReportError("Loading Character.ini...", false);
// 08.05.2004 - end change gekido
		CIniFile AttrFile("character.ini");

		if(!AttrFile.ReadFile())
		{
			CCD->ReportError("*ERROR* Failed to open character.ini file", false);
			delete CCD;
			CCD = NULL;
			MessageBox(NULL, "Missing character INI file", "Fatal Error", MB_OK);
			exit(-100);
		}

		MaxSelect = CurrentSelect = 0;
		string KeyName = AttrFile.FindFirstKey();
		string Type, Vector;
		char szName[64];

		while(KeyName != "")
		{
			strcpy(CharSelect[MaxSelect].Name, KeyName.c_str());
			CharSelect[MaxSelect].Bitmap = NULL;
			Type = AttrFile.GetValue(KeyName, "image");

			if(Type != "")
			{
				strcpy(szName, Type.c_str());
				CharSelect[MaxSelect].Bitmap = CreateFromFileName(szName);

				if(!CharSelect[MaxSelect].Bitmap)
				{
					char szBug[256];
					sprintf(szBug, "*ERROR* File %s - Line %d: Bad character image file name %s",
							__FILE__, __LINE__, szName);
					CCD->ReportError(szBug, false);
					delete CCD;
					CCD = NULL;
					MessageBox(NULL, "Bad character image file name","Fatal Error", MB_OK);
					exit(-100);
				}

				geEngine_AddBitmap(CCD->Engine()->Engine(), CharSelect[MaxSelect].Bitmap);

				Type = AttrFile.GetValue(KeyName, "actorname");
				if(Type == "")
				{
					char szBug[256];
					sprintf(szBug, "*ERROR* File %s - Line %d: Missing character actor name %s",
							__FILE__, __LINE__, szName);
					CCD->ReportError(szBug, false);
					delete CCD;
					CCD = NULL;
					MessageBox(NULL, "Missing character actor name","Fatal Error", MB_OK);
					exit(-100);
				}

				strcpy(CharSelect[MaxSelect].ActorName, Type.c_str());

				Vector = AttrFile.GetValue(KeyName, "actorrotation");
				if(Vector != "")
				{
					strcpy(szName, Vector.c_str());
					CharSelect[MaxSelect].Rotation = Extract(szName);
					CharSelect[MaxSelect].Rotation.X *= GE_PIOVER180;
					CharSelect[MaxSelect].Rotation.Y *= GE_PIOVER180;
					CharSelect[MaxSelect].Rotation.Z *= GE_PIOVER180;
				}

				CharSelect[MaxSelect].ActorScale = (float)AttrFile.GetValueF(KeyName, "actorscale");

				CharSelect[MaxSelect].AnimSpeed = 1.0f;
				Type = AttrFile.GetValue(KeyName, "animationspeed");
				if(Type != "")
					CharSelect[MaxSelect].AnimSpeed = (float)AttrFile.GetValueF(KeyName, "animationspeed");

// changed QD 12/15/05
				// CharSelect[MaxSelect].ShadowSize = (float)AttrFile.GetValueF(KeyName, "shadowsize");
				CharSelect[MaxSelect].ShadowSize = 0.0f;
				Type = AttrFile.GetValue(KeyName, "shadowsize");
				if(Type != "")
					CharSelect[MaxSelect].ShadowSize = (float)AttrFile.GetValueF(KeyName, "shadowsize");


				CharSelect[MaxSelect].ShadowAlpha = 128.0f;
				Type = AttrFile.GetValue(KeyName, "shadowalpha");
				if(Type != "")
					CharSelect[MaxSelect].ShadowAlpha = (float)AttrFile.GetValueF(KeyName, "shadowalpha");

				CharSelect[MaxSelect].ShadowBitmap[0] = '\0';
				Type = AttrFile.GetValue(KeyName, "shadowbitmap");
				if(Type != "")
					strcpy(CharSelect[MaxSelect].ShadowBitmap, Type.c_str());

				CharSelect[MaxSelect].ShadowAlphamap[0] = '\0';
				Type = AttrFile.GetValue(KeyName, "shadowalphamap");
				if(Type != "")
					strcpy(CharSelect[MaxSelect].ShadowAlphamap, Type.c_str());

				CharSelect[MaxSelect].UseProjectedShadows = GE_FALSE;
				Type = AttrFile.GetValue(KeyName, "projectedshadows");
				if(Type == "true")
					CharSelect[MaxSelect].UseProjectedShadows = GE_TRUE;

				CharSelect[MaxSelect].UseStencilShadows = GE_FALSE;
				Type = AttrFile.GetValue(KeyName, "stencilshadows");
				if(Type == "true")
					CharSelect[MaxSelect].UseStencilShadows = GE_TRUE;

				// initialize with default start level
				strcpy(CharSelect[MaxSelect].StartLevel, szStartLevel);
				Type = AttrFile.GetValue(KeyName, "startlevel");
				if(Type != "")
					strcpy(CharSelect[MaxSelect].StartLevel, Type.c_str());

				strcpy(CharSelect[MaxSelect].Weapon, "weapon.ini");
				Type = AttrFile.GetValue(KeyName, "weaponfile");
				if(Type != "")
					strcpy(CharSelect[MaxSelect].Weapon, Type.c_str());
// end change
				geVec3d FillColor	 = {255.0f, 255.0f, 255.0f};
				geVec3d AmbientColor = {255.0f, 255.0f, 255.0f};

				Vector = AttrFile.GetValue(KeyName, "fillcolor");
				if(Vector != "")
				{
					strcpy(szName, Vector.c_str());
					FillColor = Extract(szName);
				}

				Vector = AttrFile.GetValue(KeyName, "ambientcolor");
				if(Vector != "")
				{
					strcpy(szName, Vector.c_str());
					AmbientColor = Extract(szName);
				}

				CharSelect[MaxSelect].FillColor.r = FillColor.X;
				CharSelect[MaxSelect].FillColor.g = FillColor.Y;
				CharSelect[MaxSelect].FillColor.b = FillColor.Z;
				CharSelect[MaxSelect].FillColor.a = 255.0f;

				CharSelect[MaxSelect].AmbientColor.r = AmbientColor.X;
				CharSelect[MaxSelect].AmbientColor.g = AmbientColor.Y;
				CharSelect[MaxSelect].AmbientColor.b = AmbientColor.Z;
				CharSelect[MaxSelect].AmbientColor.a = 255.0f;

// changed QD 07/21/04
				CharSelect[MaxSelect].AmbientLightFromFloor = GE_TRUE;
				Type = AttrFile.GetValue(KeyName, "ambientlightfromfloor");
				if(Type == "false")
					CharSelect[MaxSelect].AmbientLightFromFloor = GE_FALSE;
// end change


				CharSelect[MaxSelect].Attribute[0] = '\0';
				Type = AttrFile.GetValue(KeyName, "attributefile");
				if(Type != "")
					strcpy(CharSelect[MaxSelect].Attribute, Type.c_str());

				CharSelect[MaxSelect].pSetup[0] = '\0';
				Type = AttrFile.GetValue(KeyName, "playersetupfile");
				if(Type != "")
					strcpy(CharSelect[MaxSelect].pSetup, Type.c_str());

				CharSelect[MaxSelect].Environment[0] = '\0';
				Type = AttrFile.GetValue(KeyName, "environmentfile");
				if(Type != "")
					strcpy(CharSelect[MaxSelect].Environment, Type.c_str());

				CharSelect[MaxSelect].PlayerStart[0] = '\0';
				Type = AttrFile.GetValue(KeyName, "playerstartname");
				if(Type != "")
					strcpy(CharSelect[MaxSelect].PlayerStart, Type.c_str());

				CharSelect[MaxSelect].Hud[0] = '\0';
				Type = AttrFile.GetValue(KeyName, "hudfile");
				if(Type != "")
					strcpy(CharSelect[MaxSelect].Hud, Type.c_str());

				Type = AttrFile.GetValue(KeyName, "speed");
				CharSelect[MaxSelect].Speed = -1.0f;
				if(Type != "")
					CharSelect[MaxSelect].Speed = (float)AttrFile.GetValueF(KeyName, "speed");

				Type = AttrFile.GetValue(KeyName, "jumpspeed");
				CharSelect[MaxSelect].JumpSpeed = -1.0f;
				if(Type != "")
					CharSelect[MaxSelect].JumpSpeed = (float)AttrFile.GetValueF(KeyName, "jumpspeed");

				Type = AttrFile.GetValue(KeyName, "stepheight");
				CharSelect[MaxSelect].StepHeight = -1.0f;
				if(Type != "")
					CharSelect[MaxSelect].StepHeight = (float)AttrFile.GetValueF(KeyName, "stepheight");

				CharSelect[MaxSelect].SlopeSlide = -1.0f;
				if(Type != "")
					CharSelect[MaxSelect].SlopeSlide = (float)AttrFile.GetValueF(KeyName, "slopeslide");

				CharSelect[MaxSelect].SlopeSpeed = -1.0f;
				if(Type != "")
					CharSelect[MaxSelect].SlopeSpeed = (float)AttrFile.GetValueF(KeyName, "slopespeed");

				MaxSelect +=1;
			}

			KeyName = AttrFile.FindNextKey();
		}

		if(MaxSelect == 0)
		{
			CCD->ReportError("*ERROR* No characters defined", false);
			delete CCD;
			CCD = NULL;
			MessageBox(NULL, "No characters defined","Fatal Error", MB_OK);
			exit(-100);
		}
	}

	CMixer mixer(CCD->Engine()->WindowHandle(), MIXERLINE_COMPONENTTYPE_DST_SPEAKERS,
					NO_SOURCE, MIXERCONTROL_CONTROLTYPE_VOLUME);

	if(mixer.IsOk())
		WinVol = mixer.GetControlValue();
// end change RF064

#ifdef BLIT
	ScreenBmp = geBitmap_Create(DesignX, DesignY, 1, GE_PIXELFORMAT_16BIT_4444_ARGB);
	geBitmap_SetPreferredFormat(ScreenBmp, GE_PIXELFORMAT_16BIT_4444_ARGB);
	geBitmap_ClearMips(ScreenBmp);
	geEngine_AddBitmap(CCD->Engine()->Engine(), ScreenBmp);
#endif
// end change RF063

	// initalize all menu items that need it
// 08.05.2004 - begin change gekido
	CCD->ReportError("Initializing Menu", false);
// 08.05.2004 - end change gekido

	MenuInitalize();
}

/* ------------------------------------------------------------------------------------ */
//	LoadMenuIni
/* ------------------------------------------------------------------------------------ */
// changed QD Language Menu
void CRFMenu::LoadMenuIni(char *menuini)
{
// 08.05.2004 - begin change gekido
	CCD->ReportError("Parsing Menu.ini...", false);
// 08.05.2004 - end change gekido

	geVFile *MainFS;
	geVFile *SecondFS;
	geBitmap_Info	BmpInfo;
	int i;

	for(i=0; i<NUM_BACKGROUNDS; i++)
		Backgrounds[i] = (geBitmap*)NULL;

	for(i=0; i<NUM_IMAGES; i++)
		Images[i] = (geBitmap*)NULL;

	for(i=0; i<NUM_TITLES; i++)
		Titles[i] = (geBitmap*)NULL;

	for(i=0; i<NUM_FONTS; i++)
		MenuFont[i].Bitmap = (geBitmap*)NULL;

	for(i=0; i<NUM_ANIM; i++)
		Animation[i] = NULL;

// changed QD 12/15/05
	EmptySlotImage = NULL;
// end change

	if(CCD->OpenRFFile(&SecondFS, kInstallFile, menuini, GE_VFILE_OPEN_READONLY))
	{
		// File there, parse it!
		char szInputLine[256];
		char *szAtom;
		char menuline[256], menuline2[256];

// changed RF063
		while(geVFile_GetS(SecondFS, szInputLine, 256) == GE_TRUE)
// end change RF063
		{
			if(szInputLine[0] == ';')
				continue;				// Comment line

			if(strlen(szInputLine) <= 5)
				continue;				// Skip blank lines

// changed RF063
			strcpy(errortext,szInputLine);
			// All config commands are "thing=value"
			szAtom = FirstToken(szInputLine, " =");

			if(!stricmp(szAtom, "background"))
			{
				int index = NextValue();
				strcpy(menuline, "menu\\");
				strcat(menuline, NextToken());
				geBitmap *TB = CreateFromFileName(menuline);

				if(TB == (geBitmap *)NULL)
				{
					char szBug[256];
					sprintf(szBug, "*ERROR* Bad file name %s", menuline);
					CCD->ReportError(szBug, false);
					exit(-100);
				}

				geBitmap_GetInfo(TB, &BmpInfo, NULL);
				int TBw = BmpInfo.Width;
				int TBh = BmpInfo.Height;
				int TBx = (BmpInfo.Width - CCD->Engine()->Width()) / 2;

				if(TBx<0)
					TBx = 0;
				else
					TBw = CCD->Engine()->Width();

				int TBy = (BmpInfo.Height - CCD->Engine()->Height()) / 2;

				if(TBy<0)
					TBy = 0;
				else
					TBh = CCD->Engine()->Height();

				Backgrounds[index] = geBitmap_Create(TBw, TBh, BmpInfo.MaximumMip+1, BmpInfo.Format);
				geBitmap_Blit(TB, TBx, TBy, Backgrounds[index], 0, 0, TBw, TBh);
				geBitmap_Destroy(&TB);
				//Backgrounds[index] = CreateFromFileName(menuline);

				if(Backgrounds[index] == (geBitmap*)NULL)
				{
					char szBug[256];
					sprintf(szBug, "*ERROR* Bad file name %s", menuline);
					CCD->ReportError(szBug, false);
					exit(-100);
				}

				geEngine_AddBitmap(CCD->Engine()->Engine(), Backgrounds[index]);
			}
			else if(!stricmp(szAtom, "images"))
			{
				int index = NextValue();
				strcpy(menuline, "menu\\");
				strcat(menuline, NextToken());
				strcpy(menuline2, "menu\\");
				strcat(menuline2, NextToken());
				Images[index] = CreateFromFileAndAlphaNames(menuline, menuline2);

				if(Images[index] == (geBitmap *)NULL)
				{
					char szBug[256];
					sprintf(szBug, "*ERROR* Bad file name %s", menuline);
					CCD->ReportError(szBug, false);
					exit(-100);
				}

				geEngine_AddBitmap(CCD->Engine()->Engine(), Images[index]);
			}
			else if(!stricmp(szAtom, "titles"))
			{
				int index = NextValue();
				strcpy(menuline, "menu\\");
				strcat(menuline, NextToken());
				strcpy(menuline2, "menu\\");
				strcat(menuline2, NextToken());
				Titles[index] = CreateFromFileAndAlphaNames(menuline, menuline2);

				if(Titles[index] == (geBitmap*)NULL)
				{
					char szBug[256];
					sprintf(szBug, "*ERROR* Bad file name %s", menuline);
					CCD->ReportError(szBug, false);
					exit(-100);
				}

				geEngine_AddBitmap(CCD->Engine()->Engine(), Titles[index]);
			}
			else if(!stricmp(szAtom, "font"))
			{
				int index = NextFont();
				char fontline[132];
				strcpy(fontline, NextToken());
				strcpy(menuline, "fonts\\");
				strcat(menuline, fontline);
				strcat(menuline, ".bmp");
				strcpy(menuline2, "fonts\\a_");
				strcat(menuline2, fontline);
				strcat(menuline2, ".bmp");
				MenuFont[index].Bitmap = CreateFromFileAndAlphaNames(menuline, menuline2);

				if(MenuFont[index].Bitmap == (geBitmap*)NULL)
				{
					char szBug[256];
					sprintf(szBug, "*ERROR* Bad file name %s", menuline);
					CCD->ReportError(szBug, false);
					exit(-100);
				}

				geEngine_AddBitmap(CCD->Engine()->Engine(), MenuFont[index].Bitmap);

				for(int jj=0; jj<96; jj++)
					MenuFont[index].WBitmap[jj] = NULL;

				strcpy(menuline, "fonts\\");
				strcat(menuline, fontline);
				strcat(menuline, ".dat");
				geVFile *datFile;
				CCD->OpenRFFile(&datFile, kBitmapFile, menuline, GE_VFILE_OPEN_READONLY);
				geVFile_Read(datFile, &MenuFont[index].font_height, sizeof(int));

				for(int c=0; c<96; c++)
				{
					geVFile_Read(datFile, &MenuFont[index].dat[c].width,sizeof(int));
					geVFile_Read(datFile, &MenuFont[index].dat[c].x,	sizeof(int));
					geVFile_Read(datFile, &MenuFont[index].dat[c].y,	sizeof(int));
				}

				geVFile_Close(datFile);
			}
// changed RF063
			else if(!stricmp(szAtom, "animation"))
			{
				int index = NextValue();
				strcpy(menuline, "menu\\");
				strcat(menuline, NextToken());

				// changed QD 07/15/06
				if(Animation[index])
				{
					delete Animation[index];
					Animation[index] = 0;
				}
				// end change

				Animation[index] = new CAnimGif(menuline, kVideoFile);
			}
			else if(!stricmp(szAtom, "menutitle"))
			{
				int index = NextValue();
				MTitles[index].Image_Number = NextValue();
				MTitles[index].X = NextValue();
				MTitles[index].Y = NextValue();
				MTitles[index].Width = NextValue();
				MTitles[index].Height = NextValue();
				MTitles[index].Image_X = NextValue();
				MTitles[index].Image_Y = NextValue();
				MTitles[index].Animation = NextValue();
			}
			else if(!stricmp(szAtom, "designsize"))
			{
				DesignX = NextValue();
				DesignY = NextValue();
			}
			else if(!stricmp(szAtom, "hotspot"))
			{
				HotX = NextValue();
				HotY = NextValue();
			}
			else if(!stricmp(szAtom, "fadetime"))
			{
				TimeFade = ((float)NextValue())*0.001f;
			}
// end change RF063
// changed RF064
			else if(!stricmp(szAtom, "loadscreen"))
			{
				strcpy(Loading, "menu\\");
				strcat(Loading, NextToken());
			}
// end change RF064
			else if(!stricmp(szAtom, "cursor"))
			{
				strcpy(menuline, "menu\\");
				strcat(menuline, NextToken());
				strcpy(menuline2, "menu\\");
				strcat(menuline2, NextToken());
				Cursor = CreateFromFileAndAlphaNames(menuline, menuline2);

				if(Cursor == (geBitmap*)NULL)
				{
					char szBug[256];
					sprintf(szBug, "*ERROR* Bad file name %s", menuline);
					CCD->ReportError(szBug, false);
					exit(-100);
				}

				geEngine_AddBitmap(CCD->Engine()->Engine(), Cursor);
				AnimCursor = NextValue();
			}
			else if(!stricmp(szAtom,"crosshair"))
			{
				strcpy(menuline, "menu\\");
				strcat(menuline, NextToken());
				strcpy(menuline2, "menu\\");
				strcat(menuline2, NextToken());
				Crosshair = CreateFromFileAndAlphaNames(menuline, menuline2);

				if(Crosshair == (geBitmap*)NULL)
				{
					char szBug[256];
					sprintf(szBug, "*ERROR* Bad file name %s", menuline);
					CCD->ReportError(szBug, false);
					exit(-100);
				}

				FCrosshair = CreateFromFileAndAlphaNames(menuline, menuline2);
				geEngine_AddBitmap(CCD->Engine()->Engine(), FCrosshair);
			}
			else if(!stricmp(szAtom, "main"))
			{
				MainBack	= NextValue();
				MainTitle	= NextValue();
			}
			else if(!stricmp(szAtom, "newgame"))
			{
				MainMenu[0].X			= NextValue();
				MainMenu[0].Y			= NextValue();
				NewGame.Image_Number	= NextValue();
				NewGame.Width			= NextValue();
				NewGame.Height			= NextValue();
				NewGame.Image_X			= NextValue();
				NewGame.Image_Y			= NextValue();
				NewGame.Mover_X			= NextValue();
				NewGame.Mover_Y			= NextValue();
				// changed RF063
				NewGame.Animation		= NextValue();
				NewGame.AnimationOver	= NextValue();
			}
			else if(!stricmp(szAtom, "multiplayer"))
			{
				MainMenu[1].X				= NextValue();
				MainMenu[1].Y				= NextValue();
				MultiPlayer.Image_Number	= NextValue();
				MultiPlayer.Width			= NextValue();
				MultiPlayer.Height			= NextValue();
				MultiPlayer.Image_X			= NextValue();
				MultiPlayer.Image_Y			= NextValue();
				MultiPlayer.Mover_X			= NextValue();
				MultiPlayer.Mover_Y			= NextValue();
				MultiPlayer.background		= NextValue();
				MultiPlayer.title			= NextValue();
				// changed RF063
				MultiPlayer.Animation		= NextValue();
				MultiPlayer.AnimationOver	= NextValue();
			}
			else if(!stricmp(szAtom, "loadgame"))
			{
				MainMenu[2].X			= NextValue();
				MainMenu[2].Y			= NextValue();
				LoadGame.Image_Number	= NextValue();
				LoadGame.Width			= NextValue();
				LoadGame.Height			= NextValue();
				LoadGame.Image_X		= NextValue();
				LoadGame.Image_Y		= NextValue();
				LoadGame.Mover_X		= NextValue();
				LoadGame.Mover_Y		= NextValue();
				LoadGame.background		= NextValue();
				LoadGame.title			= NextValue();
				// changed RF063
				LoadGame.Animation		= NextValue();
				LoadGame.AnimationOver	= NextValue();
			}
			else if(!stricmp(szAtom, "savegame"))
			{
				MainMenu[3].X			= NextValue();
				MainMenu[3].Y			= NextValue();
				SaveGame.Image_Number	= NextValue();
				SaveGame.Width			= NextValue();
				SaveGame.Height			= NextValue();
				SaveGame.Image_X		= NextValue();
				SaveGame.Image_Y		= NextValue();
				SaveGame.Mover_X		= NextValue();
				SaveGame.Mover_Y		= NextValue();
				SaveGame.background		= NextValue();
				SaveGame.title			= NextValue();
				// changed RF063
				SaveGame.Animation		= NextValue();
				SaveGame.AnimationOver	= NextValue();
			}
			else if(!stricmp(szAtom, "options"))
			{
				MainMenu[4].X			= NextValue();
				MainMenu[4].Y			= NextValue();
				Options.Image_Number	= NextValue();
				Options.Width			= NextValue();
				Options.Height			= NextValue();
				Options.Image_X			= NextValue();
				Options.Image_Y			= NextValue();
				Options.Mover_X			= NextValue();
				Options.Mover_Y			= NextValue();
				Options.background		= NextValue();
				Options.title			= NextValue();
				// changed RF063
				Options.Animation		= NextValue();
				Options.AnimationOver	= NextValue();
			}
			else if(!stricmp(szAtom, "credits"))
			{
				MainMenu[5].X			= NextValue();
				MainMenu[5].Y			= NextValue();
				Credits.Image_Number	= NextValue();
				Credits.Width			= NextValue();
				Credits.Height			= NextValue();
				Credits.Image_X			= NextValue();
				Credits.Image_Y			= NextValue();
				Credits.Mover_X			= NextValue();
				Credits.Mover_Y			= NextValue();
				Credits.background		= NextValue();
				Credits.title			= NextValue();
				// changed RF063
				Credits.Animation		= NextValue();
				Credits.AnimationOver	= NextValue();
			}
			else if(!stricmp(szAtom, "mods"))
			{
				MainMenu[8].X		= NextValue();
				MainMenu[8].Y		= NextValue();
				Mods.Image_Number	= NextValue();
				Mods.Width			= NextValue();
				Mods.Height			= NextValue();
				Mods.Image_X		= NextValue();
				Mods.Image_Y		= NextValue();
				Mods.Mover_X		= NextValue();
				Mods.Mover_Y		= NextValue();
				Mods.background		= NextValue();
				Mods.title			= NextValue();
				Mods.Animation		= NextValue();
				Mods.AnimationOver	= NextValue();
			}
			else if(!stricmp(szAtom, "quitmain"))
			{
				MainMenu[6].X			= NextValue();
				MainMenu[6].Y			= NextValue();
				QuitGame.Image_Number	= NextValue();
				QuitGame.Width			= NextValue();
				QuitGame.Height			= NextValue();
				QuitGame.Image_X		= NextValue();
				QuitGame.Image_Y		= NextValue();
				QuitGame.Mover_X		= NextValue();
				QuitGame.Mover_Y		= NextValue();
				// changed RF063
				QuitGame.Animation		= NextValue();
				QuitGame.AnimationOver	= NextValue();
			}
			else if(!stricmp(szAtom, "returntext"))
			{
				MainMenu[7].X		= NextValue();
				MainMenu[7].Y		= NextValue();
				return_text.Font	= NextFont();
			}
// start multiplayer
			else if(!stricmp(szAtom, "hostnewgame"))
			{
				MultiMenu[0].X				= NextValue();
				MultiMenu[0].Y				= NextValue();
				HostNewGame.Image_Number	= NextValue();
				HostNewGame.Width			= NextValue();
				HostNewGame.Height			= NextValue();
				HostNewGame.Image_X			= NextValue();
				HostNewGame.Image_Y			= NextValue();
				HostNewGame.Mover_X			= NextValue();
				HostNewGame.Mover_Y			= NextValue();
				HostNewGame.background		= NextValue();
				HostNewGame.title			= NextValue();
				HostNewGame.Animation		= NextValue();
				HostNewGame.AnimationOver	= NextValue();
			}
			else if(!stricmp(szAtom, "joinmultiplayergame"))
			{
				MultiMenu[1].X						= NextValue();
				MultiMenu[1].Y						= NextValue();
				JoinMultiplayerGame.Image_Number	= NextValue();
				JoinMultiplayerGame.Width			= NextValue();
				JoinMultiplayerGame.Height			= NextValue();
				JoinMultiplayerGame.Image_X			= NextValue();
				JoinMultiplayerGame.Image_Y			= NextValue();
				JoinMultiplayerGame.Mover_X			= NextValue();
				JoinMultiplayerGame.Mover_Y			= NextValue();
				JoinMultiplayerGame.background		= NextValue();
				JoinMultiplayerGame.title			= NextValue();
				JoinMultiplayerGame.Animation		= NextValue();
				JoinMultiplayerGame.AnimationOver	= NextValue();
			}
			else if(!stricmp(szAtom, "quitmulti"))
			{
				MultiMenu[2].X			= NextValue();
				MultiMenu[2].Y			= NextValue();
				QuitMulti.Image_Number	= NextValue();
				QuitMulti.Width			= NextValue();
				QuitMulti.Height		= NextValue();
				QuitMulti.Image_X		= NextValue();
				QuitMulti.Image_Y		= NextValue();
				QuitMulti.Mover_X		= NextValue();
				QuitMulti.Mover_Y		= NextValue();
				// changed RF063
				QuitMulti.Animation		= NextValue();
				QuitMulti.AnimationOver = NextValue();
			}
			else if(!stricmp(szAtom, "playerip_text"))
			{
				MultiMenu[3].X		= NextValue();
				MultiMenu[3].Y		= NextValue();
				PlayerIP_Text.Font	= NextFont();
			}
            else if(!stricmp(szAtom, "playerip1"))
			{
				MultiMenu[4].X = NextValue();
				MultiMenu[4].Y = NextValue();
				PlayerIP1.Font = NextFont();
			}
			else if(!stricmp(szAtom, "ipadd_text"))
			{
				JoinGameMenu[0].X	= NextValue();
				JoinGameMenu[0].Y	= NextValue();
				IPAdd_Text.Font		= NextFont();
			}
			else if(!stricmp(szAtom, "ipaddress"))
			{
				JoinGameMenu[1].X	= NextValue();
				JoinGameMenu[1].Y	= NextValue();
				IPAddress.Font		= NextFont();
				IPAddress.Set_X		= JoinGameMenu[1].X;
				IPAddress.Set_Y		= JoinGameMenu[1].Y;
				IPAddress.Width		= NextValue();
			}
			else if(!stricmp(szAtom, "launchjoingame"))
			{
				JoinGameMenu[2].X			= NextValue();
				JoinGameMenu[2].Y			= NextValue();

				LaunchJoinGame.Image_Number = NextValue();
				LaunchJoinGame.Width		= NextValue();
				LaunchJoinGame.Height		= NextValue();
				LaunchJoinGame.Image_X		= NextValue();
				LaunchJoinGame.Image_Y		= NextValue();
				LaunchJoinGame.Mover_X		= NextValue();
				LaunchJoinGame.Mover_Y		= NextValue();
				LaunchJoinGame.Animation	= NextValue();
				LaunchJoinGame.AnimationOver= NextValue();
			}
			else if(!stricmp(szAtom, "quitjoingame"))
			{
				JoinGameMenu[3].X			= NextValue();
				JoinGameMenu[3].Y			= NextValue();
				QuitJoinGame.Image_Number	= NextValue();
				QuitJoinGame.Width			= NextValue();
				QuitJoinGame.Height			= NextValue();
				QuitJoinGame.Image_X		= NextValue();
				QuitJoinGame.Image_Y		= NextValue();
				QuitJoinGame.Mover_X		= NextValue();
				QuitJoinGame.Mover_Y		= NextValue();
				QuitJoinGame.Animation		= NextValue();
				QuitJoinGame.AnimationOver	= NextValue();
			}
            else if(!stricmp(szAtom, "multiplayerhelptext"))
			{
				JoinGameMenu[4].X			= NextValue();
				JoinGameMenu[4].Y			= NextValue();
				MultiplayerHelp_Text.Font	= NextFont();
			}
// end multiplayer
			else if(!stricmp(szAtom, "quitload"))
			{
				LoadGMenu[0].X			= NextValue();
				LoadGMenu[0].Y			= NextValue();
				QuitLoad.Image_Number	= NextValue();
				QuitLoad.Width			= NextValue();
				QuitLoad.Height			= NextValue();
				QuitLoad.Image_X		= NextValue();
				QuitLoad.Image_Y		= NextValue();
				QuitLoad.Mover_X		= NextValue();
				QuitLoad.Mover_Y		= NextValue();
				// changed RF063
				QuitLoad.Animation		= NextValue();
				QuitLoad.AnimationOver	= NextValue();
			}
			else if(!stricmp(szAtom, "load"))
			{
				LoadGMenu[1].X			= NextValue();
				LoadGMenu[1].Y			= NextValue();
				LoadSlot.Image_Number	= NextValue();
				LoadSlot.Width			= NextValue();
				LoadSlot.Height			= NextValue();
				LoadSlot.Image_X		= NextValue();
				LoadSlot.Image_Y		= NextValue();
				LoadSlot.Mover_X		= NextValue();
				LoadSlot.Mover_Y		= NextValue();
				// changed RF063
				LoadSlot.Animation		= NextValue();
				LoadSlot.AnimationOver	= NextValue();
			}
			else if(!stricmp(szAtom, "loadbox"))
			{
				LoadGMenu[2].X			= NextValue();
				LoadGMenu[2].Y			= NextValue();
				LoadBox.Image_Number	= NextValue();
				LoadBox.Width			= NextValue();
				LoadBox.Height			= NextValue();
				LoadBox.Image_X			= NextValue();
				LoadBox.Image_Y			= NextValue();
				LoadBox.Start_X			= NextValue();
				LoadBox.Start_Y			= NextValue();
				LoadBox.Step			= NextValue();
				LoadBox.Max_Show		= NextValue();
				LoadBox.Corner_X		= NextValue();
				LoadBox.Corner_Y		= NextValue();
				LoadBox.Click_Width		= NextValue();
				LoadBox.Rev_X			= NextValue();
				LoadBox.Rev_Y			= NextValue();
				LoadBox.Rev_Width		= NextValue();
				LoadBox.Rev_Height		= NextValue();
				LoadBox.Font			= NextFont();
			}
			else if(!stricmp(szAtom, "loadbar"))
			{
				LoadGMenu[3].X			= NextValue();
				LoadGMenu[3].Y			= NextValue();
				LoadBar.Image_Number	= NextValue();
				LoadBar.Up_Width		= NextValue();
				LoadBar.Up_Height		= NextValue();
				LoadBar.Up_Nor_X		= NextValue();
				LoadBar.Up_Nor_Y		= NextValue();
				LoadBar.Up_Lit_X		= NextValue();
				LoadBar.Up_Lit_Y		= NextValue();
				LoadBar.Up_Push_X		= NextValue();
				LoadBar.Up_Push_Y		= NextValue();
				LoadBar.Dwn_Width		= NextValue();
				LoadBar.Dwn_Height		= NextValue();
				LoadBar.Dwn_Nor_X		= NextValue();
				LoadBar.Dwn_Nor_Y		= NextValue();
				LoadBar.Dwn_Lit_X		= NextValue();
				LoadBar.Dwn_Lit_Y		= NextValue();
				LoadBar.Dwn_Push_X		= NextValue();
				LoadBar.Dwn_Push_Y		= NextValue();
				LoadBar.Up_X			= NextValue();
				LoadBar.Up_Y			= NextValue();
				LoadBar.Dwn_X			= NextValue();
				LoadBar.Dwn_Y			= NextValue();
				LoadBar.Show			= LoadBox.Max_Show;
				LoadBar.AnimationUp		= NextValue();
				LoadBar.AnimationUpOver = NextValue();
				LoadBar.AnimationUpPush = NextValue();
				LoadBar.AnimationDwn	= NextValue();
				LoadBar.AnimationDwnOver= NextValue();
				LoadBar.AnimationDwnPush= NextValue();
			}
			else if(!stricmp(szAtom, "quitsave"))
			{
				SaveMenu[0].X			= NextValue();
				SaveMenu[0].Y			= NextValue();
				QuitSave.Image_Number	= NextValue();
				QuitSave.Width			= NextValue();
				QuitSave.Height			= NextValue();
				QuitSave.Image_X		= NextValue();
				QuitSave.Image_Y		= NextValue();
				QuitSave.Mover_X		= NextValue();
				QuitSave.Mover_Y		= NextValue();
				// changed RF063
				QuitSave.Animation		= NextValue();
				QuitSave.AnimationOver	= NextValue();
			}
			else if(!stricmp(szAtom, "save"))
			{
				SaveMenu[1].X			= NextValue();
				SaveMenu[1].Y			= NextValue();
				SaveSlot.Image_Number	= NextValue();
				SaveSlot.Width			= NextValue();
				SaveSlot.Height			= NextValue();
				SaveSlot.Image_X		= NextValue();
				SaveSlot.Image_Y		= NextValue();
				SaveSlot.Mover_X		= NextValue();
				SaveSlot.Mover_Y		= NextValue();
				// changed RF063
				SaveSlot.Animation		= NextValue();
				SaveSlot.AnimationOver	= NextValue();
			}
			else if(!stricmp(szAtom, "savebox"))
			{
				SaveMenu[2].X			= NextValue();
				SaveMenu[2].Y			= NextValue();
				SaveBox.Image_Number	= NextValue();
				SaveBox.Width			= NextValue();
				SaveBox.Height			= NextValue();
				SaveBox.Image_X			= NextValue();
				SaveBox.Image_Y			= NextValue();
				SaveBox.Start_X			= NextValue();
				SaveBox.Start_Y			= NextValue();
				SaveBox.Step			= NextValue();
				SaveBox.Max_Show		= NextValue();
				SaveBox.Corner_X		= NextValue();
				SaveBox.Corner_Y		= NextValue();
				SaveBox.Click_Width		= NextValue();
				SaveBox.Rev_X			= NextValue();
				SaveBox.Rev_Y			= NextValue();
				SaveBox.Rev_Width		= NextValue();
				SaveBox.Rev_Height		= NextValue();
				SaveBox.Font			= NextFont();
			}
			else if(!stricmp(szAtom, "savebar"))
			{
				SaveMenu[3].X				= NextValue();
				SaveMenu[3].Y				= NextValue();
				SaveBar.Image_Number		= NextValue();
				SaveBar.Up_Width			= NextValue();
				SaveBar.Up_Height			= NextValue();
				SaveBar.Up_Nor_X			= NextValue();
				SaveBar.Up_Nor_Y			= NextValue();
				SaveBar.Up_Lit_X			= NextValue();
				SaveBar.Up_Lit_Y			= NextValue();
				SaveBar.Up_Push_X			= NextValue();
				SaveBar.Up_Push_Y			= NextValue();
				SaveBar.Dwn_Width			= NextValue();
				SaveBar.Dwn_Height			= NextValue();
				SaveBar.Dwn_Nor_X			= NextValue();
				SaveBar.Dwn_Nor_Y			= NextValue();
				SaveBar.Dwn_Lit_X			= NextValue();
				SaveBar.Dwn_Lit_Y			= NextValue();
				SaveBar.Dwn_Push_X			= NextValue();
				SaveBar.Dwn_Push_Y			= NextValue();
				SaveBar.Up_X				= NextValue();
				SaveBar.Up_Y				= NextValue();
				SaveBar.Dwn_X				= NextValue();
				SaveBar.Dwn_Y				= NextValue();
				SaveBar.Show				= SaveBox.Max_Show;
				SaveBar.AnimationUp			= NextValue();
				SaveBar.AnimationUpOver		= NextValue();
				SaveBar.AnimationUpPush		= NextValue();
				SaveBar.AnimationDwn		= NextValue();
				SaveBar.AnimationDwnOver	= NextValue();
				SaveBar.AnimationDwnPush	= NextValue();
			}
			// start change Nout - Show save game image
			else if(!stricmp(szAtom, "savegameimage"))
			{
				SaveScreen.X		= NextValue();
				SaveScreen.Y		= NextValue();
				SaveScreen.Width	= NextValue();
				SaveScreen.Height	= NextValue();
				SaveScreen.Image_X	= NextValue();
				SaveScreen.Image_Y	= NextValue();

				// changed QD 12/15/05
				/*
				strcpy(SaveScreen.EmptySlotImage,NextToken());
				if(strlen(SaveScreen.EmptySlotImage)<5) //if param empty, use the SaveScreen bitmap
					strcpy(SaveScreen.EmptySlotImage,"Media\\Bitmaps\\SaveScreen.bmp");
				*/

				SaveScreen.EmptySlotImage[0] = '\0';

				char *temp;
				temp = strtok(NULL," \n");

				if(SaveScreen.Width > 0 && SaveScreen.Height > 0 && temp)
				{
					strcpy(SaveScreen.EmptySlotImage, temp);

					EmptySlotImage = CreateFromFileName(SaveScreen.EmptySlotImage);

					if(EmptySlotImage == (geBitmap*)NULL)
					{
						char szBug[256];
						sprintf(szBug, "*ERROR* Bad file name %s", SaveScreen.EmptySlotImage);
						CCD->ReportError(szBug, false);
						exit(-100);
					}

					geBitmap_SetColorKey(EmptySlotImage, GE_TRUE, 255, GE_FALSE);
					geEngine_AddBitmap(CCD->Engine()->Engine(), EmptySlotImage);
				}
				// end change
			}
			// end change Nout
			else if(!stricmp(szAtom, "audio"))
			{
				OptionMenu[0].X			= NextValue();
				OptionMenu[0].Y			= NextValue();
				AudioItem.Image_Number	= NextValue();
				AudioItem.Width			= NextValue();
				AudioItem.Height		= NextValue();
				AudioItem.Image_X		= NextValue();
				AudioItem.Image_Y		= NextValue();
				AudioItem.Mover_X		= NextValue();
				AudioItem.Mover_Y		= NextValue();
				AudioItem.background	= NextValue();
				AudioItem.title			= NextValue();
				// changed RF063
				AudioItem.Animation		= NextValue();
				AudioItem.AnimationOver = NextValue();
			}
			else if(!stricmp(szAtom, "video"))
			{
				OptionMenu[1].X			= NextValue();
				OptionMenu[1].Y			= NextValue();
				VideoItem.Image_Number	= NextValue();
				VideoItem.Width			= NextValue();
				VideoItem.Height		= NextValue();
				VideoItem.Image_X		= NextValue();
				VideoItem.Image_Y		= NextValue();
				VideoItem.Mover_X		= NextValue();
				VideoItem.Mover_Y		= NextValue();
				VideoItem.background	= NextValue();
				VideoItem.title			= NextValue();
				// changed RF063
				VideoItem.Animation		= NextValue();
				VideoItem.AnimationOver = NextValue();
			}
			else if(!stricmp(szAtom, "control"))
			{
				OptionMenu[2].X				= NextValue();
				OptionMenu[2].Y				= NextValue();
				ControlItem.Image_Number	= NextValue();
				ControlItem.Width			= NextValue();
				ControlItem.Height			= NextValue();
				ControlItem.Image_X			= NextValue();
				ControlItem.Image_Y			= NextValue();
				ControlItem.Mover_X			= NextValue();
				ControlItem.Mover_Y			= NextValue();
				ControlItem.background		= NextValue();
				ControlItem.title			= NextValue();
				// changed RF063
				ControlItem.Animation		= NextValue();
				ControlItem.AnimationOver	= NextValue();
			}
// changed QD Language Menu
			else if(!stricmp(szAtom, "language"))
			{
				OptionMenu[3].X				= NextValue();
				OptionMenu[3].Y				= NextValue();
				LanguageItem.Image_Number	= NextValue();
				LanguageItem.Width			= NextValue();
				LanguageItem.Height			= NextValue();
				LanguageItem.Image_X		= NextValue();
				LanguageItem.Image_Y		= NextValue();
				LanguageItem.Mover_X		= NextValue();
				LanguageItem.Mover_Y		= NextValue();
				LanguageItem.background		= NextValue();
				LanguageItem.title			= NextValue();
				LanguageItem.Animation		= NextValue();
				LanguageItem.AnimationOver	= NextValue();
			}
			else if(!stricmp(szAtom, "debug"))
			{
				OptionMenu[4].X			= NextValue();
				OptionMenu[4].Y			= NextValue();
// end change QD
				DebugItem.Image_Number	= NextValue();
				DebugItem.Width			= NextValue();
				DebugItem.Height		= NextValue();
				DebugItem.Image_X		= NextValue();
				DebugItem.Image_Y		= NextValue();
				DebugItem.Mover_X		= NextValue();
				DebugItem.Mover_Y		= NextValue();
				DebugItem.background	= NextValue();
				DebugItem.title			= NextValue();
				// changed RF063
				DebugItem.Animation		= NextValue();
				DebugItem.AnimationOver = NextValue();
			}
			else if(!stricmp(szAtom, "quitoptions"))
			{
				OptionMenu[5].X				= NextValue();
				OptionMenu[5].Y				= NextValue();
				QuitOption.Image_Number		= NextValue();
				QuitOption.Width			= NextValue();
				QuitOption.Height			= NextValue();
				QuitOption.Image_X			= NextValue();
				QuitOption.Image_Y			= NextValue();
				QuitOption.Mover_X			= NextValue();
				QuitOption.Mover_Y			= NextValue();
				// changed RF063
				QuitOption.Animation		= NextValue();
				QuitOption.AnimationOver	= NextValue();
			}
// changed QD Language Menu
			else if(!stricmp(szAtom, "quitlanguage"))
			{
				LanguageMenu[0].X			= NextValue();
				LanguageMenu[0].Y			= NextValue();
				QuitLanguage.Image_Number	= NextValue();
				QuitLanguage.Width			= NextValue();
				QuitLanguage.Height			= NextValue();
				QuitLanguage.Image_X		= NextValue();
				QuitLanguage.Image_Y		= NextValue();
				QuitLanguage.Mover_X		= NextValue();
				QuitLanguage.Mover_Y		= NextValue();
				QuitLanguage.Animation		= NextValue();
				QuitLanguage.AnimationOver	= NextValue();
			}
			else if(!stricmp(szAtom, "l1box"))
			{
				LanguageMenu[1].X	= NextValue();
				LanguageMenu[1].Y	= NextValue();
				boxL1.Image_Number	= NextValue();
				boxL1.Width			= NextValue();
				boxL1.Height		= NextValue();
				boxL1.Image_X		= NextValue();
				boxL1.Image_Y		= NextValue();
				boxL1.Mover_X		= NextValue();
				boxL1.Mover_Y		= NextValue();
				boxL1.Set_X			= NextValue();
				boxL1.Set_Y			= NextValue();
				boxL1.Animation		= NextValue();
				boxL1.AnimationOver = NextValue();
				boxL1.AnimationLit	= NextValue();
			}
			else if(!stricmp(szAtom, "l1text"))
			{
				LanguageMenu[2].X	= NextValue();
				LanguageMenu[2].Y	= NextValue();
				L1text.Font			= NextFont();
			}
			else if(!stricmp(szAtom, "l2box"))
			{
				LanguageMenu[3].X	= NextValue();
				LanguageMenu[3].Y	= NextValue();
				boxL2.Image_Number	= NextValue();
				boxL2.Width			= NextValue();
				boxL2.Height		= NextValue();
				boxL2.Image_X		= NextValue();
				boxL2.Image_Y		= NextValue();
				boxL2.Mover_X		= NextValue();
				boxL2.Mover_Y		= NextValue();
				boxL2.Set_X			= NextValue();
				boxL2.Set_Y			= NextValue();
				boxL2.Animation		= NextValue();
				boxL2.AnimationOver = NextValue();
				boxL2.AnimationLit	= NextValue();
			}
			else if(!stricmp(szAtom, "l2text"))
			{
				LanguageMenu[4].X	= NextValue();
				LanguageMenu[4].Y	= NextValue();
				L2text.Font			= NextFont();
			}
			else if(!stricmp(szAtom, "l3box"))
			{
				LanguageMenu[5].X	= NextValue();
				LanguageMenu[5].Y	= NextValue();
				boxL3.Image_Number	= NextValue();
				boxL3.Width			= NextValue();
				boxL3.Height		= NextValue();
				boxL3.Image_X		= NextValue();
				boxL3.Image_Y		= NextValue();
				boxL3.Mover_X		= NextValue();
				boxL3.Mover_Y		= NextValue();
				boxL3.Set_X			= NextValue();
				boxL3.Set_Y			= NextValue();
				boxL3.Animation		= NextValue();
				boxL3.AnimationOver = NextValue();
				boxL3.AnimationLit	= NextValue();
			}
			else if(!stricmp(szAtom, "l3text"))
			{
				LanguageMenu[6].X	= NextValue();
				LanguageMenu[6].Y	= NextValue();
				L3text.Font			= NextFont();
			}
			else if(!stricmp(szAtom,"l4box"))
			{
				LanguageMenu[7].X	= NextValue();
				LanguageMenu[7].Y	= NextValue();
				boxL4.Image_Number	= NextValue();
				boxL4.Width			= NextValue();
				boxL4.Height		= NextValue();
				boxL4.Image_X		= NextValue();
				boxL4.Image_Y		= NextValue();
				boxL4.Mover_X		= NextValue();
				boxL4.Mover_Y		= NextValue();
				boxL4.Set_X			= NextValue();
				boxL4.Set_Y			= NextValue();
				boxL4.Animation		= NextValue();
				boxL4.AnimationOver = NextValue();
				boxL4.AnimationLit	= NextValue();
			}
			else if(!stricmp(szAtom, "l4text"))
			{
				LanguageMenu[8].X	= NextValue();
				LanguageMenu[8].Y	= NextValue();
				L4text.Font			= NextFont();
			}
			else if(!stricmp(szAtom, "l5box"))
			{
				LanguageMenu[9].X	= NextValue();
				LanguageMenu[9].Y	= NextValue();
				boxL5.Image_Number	= NextValue();
				boxL5.Width			= NextValue();
				boxL5.Height		= NextValue();
				boxL5.Image_X		= NextValue();
				boxL5.Image_Y		= NextValue();
				boxL5.Mover_X		= NextValue();
				boxL5.Mover_Y		= NextValue();
				boxL5.Set_X			= NextValue();
				boxL5.Set_Y			= NextValue();
				boxL5.Animation		= NextValue();
				boxL5.AnimationOver = NextValue();
				boxL5.AnimationLit	= NextValue();
			}
			else if(!stricmp(szAtom, "l5text"))
			{
				LanguageMenu[10].X	= NextValue();
				LanguageMenu[10].Y	= NextValue();
				L5text.Font			= NextFont();
			}
			else if(!stricmp(szAtom, "languagetext"))
			{
				// changed QD 12/15/05 - default is a better indicator ;-)
				char *language = NextToken();
				if(!stricmp("standard", language) || !stricmp("default", language))
					L1text.text = strdup("English");
				else
					L1text.text = strdup(language);

				language = NextToken();
				if(!stricmp("standard", language) || !stricmp("default", language))
					L2text.text = strdup("Deutsch");
				else
					L2text.text = strdup(language);

				language = NextToken();
				if(!stricmp("standard", language) || !stricmp("default", language))
					L3text.text = strdup("Francais");
				else
					L3text.text = strdup(language);

				language = NextToken();
				if(!stricmp("standard", language) || !stricmp("default", language))
					L4text.text = strdup("Espanol");
				else
					L4text.text = strdup(language);

				language = NextToken();
				if(!stricmp("standard", language) || !stricmp("default", language))
					L5text.text = strdup("Italiano");
				else
					L5text.text = strdup(language);
				// end change
			}
			else if(!stricmp(szAtom, "menuinis"))
			{
				strcpy(MenuInis[0], NextToken());
				strcpy(MenuInis[1], NextToken());
				strcpy(MenuInis[2], NextToken());
				strcpy(MenuInis[3], NextToken());
				strcpy(MenuInis[4], NextToken());
			}
			else if(!stricmp(szAtom, "convtxts"))
			{
				strcpy(Convtxts[0], NextToken());
				strcpy(Convtxts[1], NextToken());
				strcpy(Convtxts[2], NextToken());
				strcpy(Convtxts[3], NextToken());
				strcpy(Convtxts[4], NextToken());
			}
			else if(!stricmp(szAtom, "messagetxts"))
			{
				strcpy(Messagetxts[0], NextToken());
				strcpy(Messagetxts[1], NextToken());
				strcpy(Messagetxts[2], NextToken());
				strcpy(Messagetxts[3], NextToken());
				strcpy(Messagetxts[4], NextToken());
			}
// end change QD
			else if(!stricmp(szAtom, "quitdebug"))
			{
				DebugMenu[0].X			= NextValue();
				DebugMenu[0].Y			= NextValue();
				QuitDebug.Image_Number	= NextValue();
				QuitDebug.Width			= NextValue();
				QuitDebug.Height		= NextValue();
				QuitDebug.Image_X		= NextValue();
				QuitDebug.Image_Y		= NextValue();
				QuitDebug.Mover_X		= NextValue();
				QuitDebug.Mover_Y		= NextValue();
				// changed RF063
				QuitDebug.Animation		= NextValue();
				QuitDebug.AnimationOver = NextValue();
			}
			else if(!stricmp(szAtom, "debugbox"))
			{
				DebugMenu[1].X		= NextValue();
				DebugMenu[1].Y		= NextValue();
				box5.Image_Number	= NextValue();
				box5.Width			= NextValue();
				box5.Height			= NextValue();
				box5.Image_X		= NextValue();
				box5.Image_Y		= NextValue();
				box5.Mover_X		= NextValue();
				box5.Mover_Y		= NextValue();
				box5.Set_X			= NextValue();
				box5.Set_Y			= NextValue();
				box5.Animation		= NextValue();
				box5.AnimationOver	= NextValue();
				box5.AnimationLit	= NextValue();
			}
			else if(!stricmp(szAtom, "debugtext"))
			{
				DebugMenu[2].X	= NextValue();
				DebugMenu[2].Y	= NextValue();
				debug_text.Font = NextFont();
			}
			else if(!stricmp(szAtom, "fpsbox"))
			{
				DebugMenu[3].X		= NextValue();
				DebugMenu[3].Y		= NextValue();
				box6.Image_Number	= NextValue();
				box6.Width			= NextValue();
				box6.Height			= NextValue();
				box6.Image_X		= NextValue();
				box6.Image_Y		= NextValue();
				box6.Mover_X		= NextValue();
				box6.Mover_Y		= NextValue();
				box6.Set_X			= NextValue();
				box6.Set_Y			= NextValue();
				box6.Animation		= NextValue();
				box6.AnimationOver	= NextValue();
				box6.AnimationLit	= NextValue();
			}
			else if(!stricmp(szAtom, "fpstext"))
			{
				DebugMenu[4].X	= NextValue();
				DebugMenu[4].Y	= NextValue();
				fps_text.Font	= NextFont();
			}
			else if(!stricmp(szAtom, "clipbox"))
			{
				DebugMenu[5].X		= NextValue();
				DebugMenu[5].Y		= NextValue();
				box7.Image_Number	= NextValue();
				box7.Width			= NextValue();
				box7.Height			= NextValue();
				box7.Image_X		= NextValue();
				box7.Image_Y		= NextValue();
				box7.Mover_X		= NextValue();
				box7.Mover_Y		= NextValue();
				box7.Set_X			= NextValue();
				box7.Set_Y			= NextValue();
				box7.Animation		= NextValue();
				box7.AnimationOver	= NextValue();
				box7.AnimationLit	= NextValue();
			}
			else if(!stricmp(szAtom, "cliptext"))
			{
				DebugMenu[6].X = NextValue();
				DebugMenu[6].Y = NextValue();
				clip_text.Font = NextFont();
			}
			else if(!stricmp(szAtom, "bbbox"))
			{
				DebugMenu[7].X		= NextValue();
				DebugMenu[7].Y		= NextValue();
				box8.Image_Number	= NextValue();
				box8.Width			= NextValue();
				box8.Height			= NextValue();
				box8.Image_X		= NextValue();
				box8.Image_Y		= NextValue();
				box8.Mover_X		= NextValue();
				box8.Mover_Y		= NextValue();
				box8.Set_X			= NextValue();
				box8.Set_Y			= NextValue();
				box8.Animation		= NextValue();
				box8.AnimationOver	= NextValue();
				box8.AnimationLit	= NextValue();
			}
			else if(!stricmp(szAtom, "bbtext"))
			{
				DebugMenu[8].X	= NextValue();
				DebugMenu[8].Y	= NextValue();
				bb_text.Font	= NextFont();
			}
			else if(!stricmp(szAtom, "sebbbox"))
			{
				DebugMenu[9].X		= NextValue();
				DebugMenu[9].Y		= NextValue();
				box9.Image_Number	= NextValue();
				box9.Width			= NextValue();
				box9.Height			= NextValue();
				box9.Image_X		= NextValue();
				box9.Image_Y		= NextValue();
				box9.Mover_X		= NextValue();
				box9.Mover_Y		= NextValue();
				box9.Set_X			= NextValue();
				box9.Set_Y			= NextValue();
				box9.Animation		= NextValue();
				box9.AnimationOver	= NextValue();
				box9.AnimationLit	= NextValue();
			}
			else if(!stricmp(szAtom, "sebbtext"))
			{
				DebugMenu[10].X		= NextValue();
				DebugMenu[10].Y		= NextValue();
				sebb_text.Font		= NextFont();
			}
			else if(!stricmp(szAtom, "quitcredit"))
			{
				CreditMenu[0].X			= NextValue();
				CreditMenu[0].Y			= NextValue();
				QuitCredit.Image_Number = NextValue();
				QuitCredit.Width		= NextValue();
				QuitCredit.Height		= NextValue();
				QuitCredit.Image_X		= NextValue();
				QuitCredit.Image_Y		= NextValue();
				QuitCredit.Mover_X		= NextValue();
				QuitCredit.Mover_Y		= NextValue();
				// changed RF063
				QuitCredit.Animation	= NextValue();
				QuitCredit.AnimationOver= NextValue();
			}
			else if(!stricmp(szAtom, "quitmods"))
			{
				ModsMenu[0].X			= NextValue();
				ModsMenu[0].Y			= NextValue();
				QuitMods.Image_Number	= NextValue();
				QuitMods.Width			= NextValue();
				QuitMods.Height			= NextValue();
				QuitMods.Image_X		= NextValue();
				QuitMods.Image_Y		= NextValue();
				QuitMods.Mover_X		= NextValue();
				QuitMods.Mover_Y		= NextValue();
				QuitMods.Animation		= NextValue();
				QuitMods.AnimationOver	= NextValue();
			}
			else if(!stricmp(szAtom, "creditimg"))
			{
				CreditMenu[1].X			= NextValue();
				CreditMenu[1].Y			= NextValue();
				Credit_Img.Image_Number = NextValue();
				Credit_Img.Width		= NextValue();
				Credit_Img.Height		= NextValue();
				Credit_Img.Image_X		= NextValue();
				Credit_Img.Image_Y		= NextValue();
				Credit_Img.Animation	= NextValue();
			}
			else if(!stricmp(szAtom, "quitaudio"))
			{
				AudioMenu[0].X			= NextValue();
				AudioMenu[0].Y			= NextValue();
				QuitAudio.Image_Number	= NextValue();
				QuitAudio.Width			= NextValue();
				QuitAudio.Height		= NextValue();
				QuitAudio.Image_X		= NextValue();
				QuitAudio.Image_Y		= NextValue();
				QuitAudio.Mover_X		= NextValue();
				QuitAudio.Mover_Y		= NextValue();
				// changed RF063
				QuitAudio.Animation		= NextValue();
				QuitAudio.AnimationOver = NextValue();
			}
			else if(!stricmp(szAtom, "voltext"))
			{
				AudioMenu[1].X	= NextValue();
				AudioMenu[1].Y	= NextValue();
				vol_text.Font	= NextFont();
			}
			else if(!stricmp(szAtom, "volimg"))
			{
				AudioMenu[2].X			= NextValue();
				AudioMenu[2].Y			= NextValue();
				Vol_Img.Image_Number	= NextValue();
				Vol_Img.Width			= NextValue();
				Vol_Img.Height			= NextValue();
				Vol_Img.Image_X			= NextValue();
				Vol_Img.Image_Y			= NextValue();
				Vol_Img.Animation		= NextValue();
			}
			else if(!stricmp(szAtom, "volslider"))
			{
				AudioMenu[3].X			= NextValue();
				AudioMenu[3].Y			= NextValue();
				Vol_Slide.Image_Number	= NextValue();
				Vol_Slide.Width			= NextValue();
				Vol_Slide.Height		= NextValue();
				Vol_Slide.Image_X		= NextValue();
				Vol_Slide.Image_Y		= NextValue();
				Vol_Slide.Min_X			= NextValue();
				Vol_Slide.Max_X			= NextValue();
				// changed RF063
				Vol_Slide.Animation		= NextValue();
			}
			else if(!stricmp(szAtom, "mvoltext"))
			{
				AudioMenu[4].X = NextValue();
				AudioMenu[4].Y = NextValue();
				mvol_text.Font = NextFont();
			}
			else if(!stricmp(szAtom,"mvolimg"))
			{
				AudioMenu[5].X			= NextValue();
				AudioMenu[5].Y			= NextValue();
				mVol_Img.Image_Number	= NextValue();
				mVol_Img.Width			= NextValue();
				mVol_Img.Height			= NextValue();
				mVol_Img.Image_X		= NextValue();
				mVol_Img.Image_Y		= NextValue();
				mVol_Img.Animation		= NextValue();
			}
			else if(!stricmp(szAtom, "mvolslider"))
			{
				AudioMenu[6].X			= NextValue();
				AudioMenu[6].Y			= NextValue();
				mVol_Slide.Image_Number = NextValue();
				mVol_Slide.Width		= NextValue();
				mVol_Slide.Height		= NextValue();
				mVol_Slide.Image_X		= NextValue();
				mVol_Slide.Image_Y		= NextValue();
				mVol_Slide.Min_X		= NextValue();
				mVol_Slide.Max_X		= NextValue();
				mVol_Slide.Animation	= NextValue();
			}
			else if(!stricmp(szAtom, "cdbox"))
			{
				AudioMenu[7].X		= NextValue();
				AudioMenu[7].Y		= NextValue();
				box0.Image_Number	= NextValue();
				box0.Width			= NextValue();
				box0.Height			= NextValue();
				box0.Image_X		= NextValue();
				box0.Image_Y		= NextValue();
				box0.Mover_X		= NextValue();
				box0.Mover_Y		= NextValue();
				box0.Set_X			= NextValue();
				box0.Set_Y			= NextValue();
				box0.Animation		= NextValue();
				box0.AnimationOver	= NextValue();
				box0.AnimationLit	= NextValue();
			}
			else if(!stricmp(szAtom, "cdtext"))
			{
				AudioMenu[8].X	= NextValue();
				AudioMenu[8].Y	= NextValue();
				cd_text.Font	= NextFont();
			}
			else if(!stricmp(szAtom, "quitvideo"))
			{
				VideoMenu[0].X			= NextValue();
				VideoMenu[0].Y			= NextValue();
				QuitVideo.Image_Number	= NextValue();
				QuitVideo.Width			= NextValue();
				QuitVideo.Height		= NextValue();
				QuitVideo.Image_X		= NextValue();
				QuitVideo.Image_Y		= NextValue();
				QuitVideo.Mover_X		= NextValue();
				QuitVideo.Mover_Y		= NextValue();
				// changed RF063
				QuitVideo.Animation		= NextValue();
				QuitVideo.AnimationOver = NextValue();
			}
			else if(!stricmp(szAtom, "gammatext"))
			{
				VideoMenu[1].X	= NextValue();
				VideoMenu[1].Y	= NextValue();
				gam_text.Font	= NextFont();
			}
			else if(!stricmp(szAtom, "gammaimg"))
			{
				VideoMenu[2].X			= NextValue();
				VideoMenu[2].Y			= NextValue();
				Gamma_Img.Image_Number	= NextValue();
				Gamma_Img.Width			= NextValue();
				Gamma_Img.Height		= NextValue();
				Gamma_Img.Image_X		= NextValue();
				Gamma_Img.Image_Y		= NextValue();
				Gamma_Img.Animation		= NextValue();
			}
			else if(!stricmp(szAtom, "gammaslider"))
			{
				VideoMenu[3].X				= NextValue();
				VideoMenu[3].Y				= NextValue();
				Gamma_Slide.Image_Number	= NextValue();
				Gamma_Slide.Width			= NextValue();
				Gamma_Slide.Height			= NextValue();
				Gamma_Slide.Image_X			= NextValue();
				Gamma_Slide.Image_Y			= NextValue();
				Gamma_Slide.Min_X			= NextValue();
				Gamma_Slide.Max_X			= NextValue();
				// changed RF063
				Gamma_Slide.Animation		= NextValue();
			}
			else if(!stricmp(szAtom, "detailtext"))
			{
				VideoMenu[4].X	= NextValue();
				VideoMenu[4].Y	= NextValue();
				det_text.Font	= NextFont();
			}
			else if(!stricmp(szAtom, "detailimg"))
			{
				VideoMenu[5].X			= NextValue();
				VideoMenu[5].Y			= NextValue();
				Detail_Img.Image_Number = NextValue();
				Detail_Img.Width		= NextValue();
				Detail_Img.Height		= NextValue();
				Detail_Img.Image_X		= NextValue();
				Detail_Img.Image_Y		= NextValue();
				Detail_Img.Animation	= NextValue();
			}
			else if(!stricmp(szAtom, "detailslider"))
			{
				VideoMenu[6].X				= NextValue();
				VideoMenu[6].Y				= NextValue();
				Detail_Slide.Image_Number	= NextValue();
				Detail_Slide.Width			= NextValue();
				Detail_Slide.Height			= NextValue();
				Detail_Slide.Image_X		= NextValue();
				Detail_Slide.Image_Y		= NextValue();
				Detail_Slide.Min_X			= NextValue();
				Detail_Slide.Max_X			= NextValue();
				// changed RF063
				Detail_Slide.Animation		= NextValue();
			}
// changed QD Shadows
			else if(!stricmp(szAtom, "shadowbox"))
			{
				VideoMenu[7].X			= NextValue();
				VideoMenu[7].Y			= NextValue();
				ShadowBox.Image_Number	= NextValue();
				ShadowBox.Width			= NextValue();
				ShadowBox.Height		= NextValue();
				ShadowBox.Image_X		= NextValue();
				ShadowBox.Image_Y		= NextValue();
				ShadowBox.Mover_X		= NextValue();
				ShadowBox.Mover_Y		= NextValue();
				ShadowBox.Set_X			= NextValue();
				ShadowBox.Set_Y			= NextValue();
				ShadowBox.Animation		= NextValue();
				ShadowBox.AnimationOver = NextValue();
				ShadowBox.AnimationLit	= NextValue();
			}
			else if(!stricmp(szAtom, "shadowtext"))
			{
				VideoMenu[8].X		= NextValue();
				VideoMenu[8].Y		= NextValue();
				shadow_text.Font	= NextFont();
			}
// end change
			else if(!stricmp(szAtom, "advance"))
			{
				ControlMenu[0].X			= NextValue();
				ControlMenu[0].Y			= NextValue();
				AdvancedItem.Image_Number	= NextValue();
				AdvancedItem.Width			= NextValue();
				AdvancedItem.Height			= NextValue();
				AdvancedItem.Image_X		= NextValue();
				AdvancedItem.Image_Y		= NextValue();
				AdvancedItem.Mover_X		= NextValue();
				AdvancedItem.Mover_Y		= NextValue();
				AdvancedItem.background		= NextValue();
				AdvancedItem.title			= NextValue();
				// changed RF063
				AdvancedItem.Animation		= NextValue();
				AdvancedItem.AnimationOver	= NextValue();
			}
			else if(!stricmp(szAtom, "quitcontrol"))
			{
				ControlMenu[1].X			= NextValue();
				ControlMenu[1].Y			= NextValue();
				QuitControl.Image_Number	= NextValue();
				QuitControl.Width			= NextValue();
				QuitControl.Height			= NextValue();
				QuitControl.Image_X			= NextValue();
				QuitControl.Image_Y			= NextValue();
				QuitControl.Mover_X			= NextValue();
				QuitControl.Mover_Y			= NextValue();
				// changed RF063
				QuitControl.Animation		= NextValue();
				QuitControl.AnimationOver	= NextValue();
			}
			else if(!stricmp(szAtom, "remapbox"))
			{
				ControlMenu[2].X	= NextValue();
				ControlMenu[2].Y	= NextValue();
				KeyMap.Image_Number = NextValue();
				KeyMap.Width		= NextValue();
				KeyMap.Height		= NextValue();
				KeyMap.Image_X		= NextValue();
				KeyMap.Image_Y		= NextValue();
				KeyMap.Start_X		= NextValue();
				KeyMap.Start_Y		= NextValue();
				KeyMap.Step			= NextValue();
				KeyMap.Max_Show		= NextValue();
				KeyMap.Corner_X		= NextValue();
				KeyMap.Corner_Y		= NextValue();
				KeyMap.Click_Width	= NextValue();
				KeyMap.Key_X		= NextValue();
				KeyMap.Rev_X		= NextValue();
				KeyMap.Rev_Y		= NextValue();
				KeyMap.Rev_Width	= NextValue();
				KeyMap.Rev_Height	= NextValue();
				KeyMap.RevC_X		= NextValue();
				KeyMap.RevC_Y		= NextValue();
				KeyMap.RevC_Width	= NextValue();
				KeyMap.RevC_Height	= NextValue();
				KeyMap.Font			= NextFont();
			}
			else if(!stricmp(szAtom, "remapbar"))
			{
				ControlMenu[3].X			= NextValue();
				ControlMenu[3].Y			= NextValue();
				KeyMapBar.Image_Number		= NextValue();
				KeyMapBar.Up_Width			= NextValue();
				KeyMapBar.Up_Height			= NextValue();
				KeyMapBar.Up_Nor_X			= NextValue();
				KeyMapBar.Up_Nor_Y			= NextValue();
				KeyMapBar.Up_Lit_X			= NextValue();
				KeyMapBar.Up_Lit_Y			= NextValue();
				KeyMapBar.Up_Push_X			= NextValue();
				KeyMapBar.Up_Push_Y			= NextValue();
				KeyMapBar.Dwn_Width			= NextValue();
				KeyMapBar.Dwn_Height		= NextValue();
				KeyMapBar.Dwn_Nor_X			= NextValue();
				KeyMapBar.Dwn_Nor_Y			= NextValue();
				KeyMapBar.Dwn_Lit_X			= NextValue();
				KeyMapBar.Dwn_Lit_Y			= NextValue();
				KeyMapBar.Dwn_Push_X		= NextValue();
				KeyMapBar.Dwn_Push_Y		= NextValue();
				KeyMapBar.Up_X				= NextValue();
				KeyMapBar.Up_Y				= NextValue();
				KeyMapBar.Dwn_X				= NextValue();
				KeyMapBar.Dwn_Y				= NextValue();
				KeyMapBar.Show				= KeyMap.Max_Show;
				KeyMapBar.AnimationUp		= NextValue();
				KeyMapBar.AnimationUpOver	= NextValue();
				KeyMapBar.AnimationUpPush	= NextValue();
				KeyMapBar.AnimationDwn		= NextValue();
				KeyMapBar.AnimationDwnOver	= NextValue();
				KeyMapBar.AnimationDwnPush	= NextValue();
			}
			else if(!stricmp(szAtom, "default"))
			{
				ControlMenu[4].X		= NextValue();
				ControlMenu[4].Y		= NextValue();
				ResetKey.Image_Number	= NextValue();
				ResetKey.Width			= NextValue();
				ResetKey.Height			= NextValue();
				ResetKey.Image_X		= NextValue();
				ResetKey.Image_Y		= NextValue();
				ResetKey.Mover_X		= NextValue();
				ResetKey.Mover_Y		= NextValue();
				// changed RF063
				ResetKey.Animation		= NextValue();
				ResetKey.AnimationOver	= NextValue();
			}
			else if(!stricmp(szAtom, "quitadvance"))
			{
				AdvancedMenu[0].X			= NextValue();
				AdvancedMenu[0].Y			= NextValue();
				QuitAdvanced.Image_Number	= NextValue();
				QuitAdvanced.Width			= NextValue();
				QuitAdvanced.Height			= NextValue();
				QuitAdvanced.Image_X		= NextValue();
				QuitAdvanced.Image_Y		= NextValue();
				QuitAdvanced.Mover_X		= NextValue();
				QuitAdvanced.Mover_Y		= NextValue();
				// changed RF063
				QuitAdvanced.Animation		= NextValue();
				QuitAdvanced.AnimationOver	= NextValue();
			}
			else if(!stricmp(szAtom, "crossbox"))
			{
				AdvancedMenu[1].X	= NextValue();
				AdvancedMenu[1].Y	= NextValue();
				box1.Image_Number	= NextValue();
				box1.Width			= NextValue();
				box1.Height			= NextValue();
				box1.Image_X		= NextValue();
				box1.Image_Y		= NextValue();
				box1.Mover_X		= NextValue();
				box1.Mover_Y		= NextValue();
				box1.Set_X			= NextValue();
				box1.Set_Y			= NextValue();
				box1.Animation		= NextValue();
				box1.AnimationOver	= NextValue();
				box1.AnimationLit	= NextValue();
			}
			else if(!stricmp(szAtom, "crosstext"))
			{
				AdvancedMenu[2].X	= NextValue();
				AdvancedMenu[2].Y	= NextValue();
				xhair_text.Font		= NextFont();
			}
			else if(!stricmp(szAtom, "reversebox"))
			{
				AdvancedMenu[3].X	= NextValue();
				AdvancedMenu[3].Y	= NextValue();
				box2.Image_Number	= NextValue();
				box2.Width			= NextValue();
				box2.Height			= NextValue();
				box2.Image_X		= NextValue();
				box2.Image_Y		= NextValue();
				box2.Mover_X		= NextValue();
				box2.Mover_Y		= NextValue();
				box2.Set_X			= NextValue();
				box2.Set_Y			= NextValue();
				box2.Animation		= NextValue();
				box2.AnimationOver	= NextValue();
				box2.AnimationLit	= NextValue();
			}
			else if(!stricmp(szAtom, "reversetext"))
			{
				AdvancedMenu[4].X	= NextValue();
				AdvancedMenu[4].Y	= NextValue();
				rev_text.Font		= NextFont();
			}
			else if(!stricmp(szAtom, "filterbox"))
			{
				AdvancedMenu[5].X	= NextValue();
				AdvancedMenu[5].Y	= NextValue();
				box4.Image_Number	= NextValue();
				box4.Width			= NextValue();
				box4.Height			= NextValue();
				box4.Image_X		= NextValue();
				box4.Image_Y		= NextValue();
				box4.Mover_X		= NextValue();
				box4.Mover_Y		= NextValue();
				box4.Set_X			= NextValue();
				box4.Set_Y			= NextValue();
				box4.Animation		= NextValue();
				box4.AnimationOver	= NextValue();
				box4.AnimationLit	= NextValue();
			}
			else if(!stricmp(szAtom, "filtertext"))
			{
				AdvancedMenu[6].X	= NextValue();
				AdvancedMenu[6].Y	= NextValue();
				filter_text.Font	= NextFont();
			}
			else if(!stricmp(szAtom, "senstext"))
			{
				AdvancedMenu[7].X	= NextValue();
				AdvancedMenu[7].Y	= NextValue();
				sens_text.Font		= NextFont();
			}
			else if(!stricmp(szAtom, "sensimg"))
			{
				AdvancedMenu[8].X		= NextValue();
				AdvancedMenu[8].Y		= NextValue();
				Sens_Img.Image_Number	= NextValue();
				Sens_Img.Width			= NextValue();
				Sens_Img.Height			= NextValue();
				Sens_Img.Image_X		= NextValue();
				Sens_Img.Image_Y		= NextValue();
				Sens_Img.Animation		= NextValue();
			}
			else if(!stricmp(szAtom, "sensslider"))
			{
				AdvancedMenu[9].X		= NextValue();
				AdvancedMenu[9].Y		= NextValue();
				Sens_Slide.Image_Number = NextValue();
				Sens_Slide.Width		= NextValue();
				Sens_Slide.Height		= NextValue();
				Sens_Slide.Image_X		= NextValue();
				Sens_Slide.Image_Y		= NextValue();
				Sens_Slide.Min_X		= NextValue();
				Sens_Slide.Max_X		= NextValue();
				// changed RF063
				Sens_Slide.Animation	= NextValue();
			}
			else if(!stricmp(szAtom, "mouseclick"))
			{
				char file[256] = "menu\\";
				strcat(file, NextToken());

				if(!CCD->OpenRFFile(&MainFS, kAudioFile, file, GE_VFILE_OPEN_READONLY))
				{
					char szError[256];
					sprintf(szError, "*ERROR* Missing MouseClick Sound");
					CCD->ReportError(szError, false);
					CCD->ShutdownLevel();
					delete CCD;
					CCD = NULL;
					MessageBox(NULL, szError,"Menu", MB_OK);
					exit(-333);
				}

				mouseclick = geSound_LoadSoundDef(CCD->Engine()->AudioSystem(), MainFS);
				geVFile_Close(MainFS);
			}
			else if(!stricmp(szAtom, "keyclick"))
			{
				char file[256] = "menu\\";
				strcat(file, NextToken());

				if(!CCD->OpenRFFile(&MainFS, kAudioFile, file, GE_VFILE_OPEN_READONLY))
				{
					char szError[256];
					sprintf(szError, "*ERROR* Missing KeyClick Sound");
					CCD->ReportError(szError, false);
					CCD->ShutdownLevel();
					delete CCD;
					CCD = NULL;
					MessageBox(NULL, szError,"Menu", MB_OK);
					exit(-333);
				}

				keyclick = geSound_LoadSoundDef(CCD->Engine()->AudioSystem(), MainFS);
				geVFile_Close(MainFS);
			}
			else if(!stricmp(szAtom, "slideclick"))
			{
				char file[256] = "menu\\";
				strcat(file, NextToken());

				if(!CCD->OpenRFFile(&MainFS, kAudioFile, file, GE_VFILE_OPEN_READONLY))
				{
					char szError[256];
					sprintf(szError, "*ERROR* Missing SlideClick Sound");
					CCD->ReportError(szError, false);
					CCD->ShutdownLevel();
					delete CCD;
					CCD = NULL;
					MessageBox(NULL, szError,"Menu", MB_OK);
					exit(-333);
				}

				slideclick = geSound_LoadSoundDef(CCD->Engine()->AudioSystem(), MainFS);
				geVFile_Close(MainFS);
			}
			else if(!stricmp(szAtom, "music"))
			{
				musictype = -1;
				char *musicname = NextToken();
				int len = strlen(musicname)-4;

				if(stricmp((musicname+len),".mid")==0)
				{
					strcpy(music, musicname);
// changed RF064
					// changed QD 07/15/06
					if(theMIDIPlayer)
					{
						delete theMIDIPlayer;
						theMIDIPlayer = 0;
					}
					// end change

					theMIDIPlayer = new CMIDIAudio();

					if(theMIDIPlayer == NULL)
						CCD->ReportError("*WARNING* MIDI Player failed to instantiate", false);
					else
					{
						musictype = 1;
					}
// end change RF064
				}
				else
				{
					strcpy(music, CCD->GetDirectory(kAudioStreamFile));
					strcat(music, "\\");
					strcat(music, musicname);
					m_dsPtr = (LPDIRECTSOUND)geSound_GetDSound();

					// changed QD 07/15/06
					if(m_Streams)
					{
						delete m_Streams;
						m_Streams = 0;
					}
					// end change

					m_Streams = new StreamingAudio(m_dsPtr);

					if(m_Streams)
					{
						if(m_Streams->Create(music))
						{
							musictype = 0;
						}
					}
				}
			}
// changed RF063
			//else
			if(!stricmp(szAtom, "select"))
			{
				SelectBack	= NextValue();
				SelectTitle = NextValue();
			}
			else if(!stricmp(szAtom, "acceptchar"))
			{
				SelectMenu[0].X				= NextValue();
				SelectMenu[0].Y				= NextValue();
				AcceptSelect.Image_Number	= NextValue();
				AcceptSelect.Width			= NextValue();
				AcceptSelect.Height			= NextValue();
				AcceptSelect.Image_X		= NextValue();
				AcceptSelect.Image_Y		= NextValue();
				AcceptSelect.Mover_X		= NextValue();
				AcceptSelect.Mover_Y		= NextValue();
				AcceptSelect.Animation		= NextValue();
				AcceptSelect.AnimationOver	= NextValue();
			}
			else if(!stricmp(szAtom, "cancelchar"))
			{
				SelectMenu[1].X				= NextValue();
				SelectMenu[1].Y				= NextValue();
				CancelSelect.Image_Number	= NextValue();
				CancelSelect.Width			= NextValue();
				CancelSelect.Height			= NextValue();
				CancelSelect.Image_X		= NextValue();
				CancelSelect.Image_Y		= NextValue();
				CancelSelect.Mover_X		= NextValue();
				CancelSelect.Mover_Y		= NextValue();
				CancelSelect.Animation		= NextValue();
				CancelSelect.AnimationOver	= NextValue();
			}
			else if(!stricmp(szAtom, "prevchar"))
			{
				SelectMenu[2].X				= NextValue();
				SelectMenu[2].Y				= NextValue();
				PrevSelect.Image_Number		= NextValue();
				PrevSelect.Width			= NextValue();
				PrevSelect.Height			= NextValue();
				PrevSelect.Image_X			= NextValue();
				PrevSelect.Image_Y			= NextValue();
				PrevSelect.Mover_X			= NextValue();
				PrevSelect.Mover_Y			= NextValue();
				PrevSelect.Animation		= NextValue();
				PrevSelect.AnimationOver	= NextValue();
			}
			else if(!stricmp(szAtom, "nextchar"))
			{
				SelectMenu[3].X				= NextValue();
				SelectMenu[3].Y				= NextValue();
				NextSelect.Image_Number		= NextValue();
				NextSelect.Width			= NextValue();
				NextSelect.Height			= NextValue();
				NextSelect.Image_X			= NextValue();
				NextSelect.Image_Y			= NextValue();
				NextSelect.Mover_X			= NextValue();
				NextSelect.Mover_Y			= NextValue();
				NextSelect.Animation		= NextValue();
				NextSelect.AnimationOver	= NextValue();
			}
			else if(!stricmp(szAtom, "imagechar"))
			{
				SelectMenu[4].X = NextValue();
				SelectMenu[4].Y = NextValue();
			}
// changed QD 12/15/05
			else if(!stricmp(szAtom, "nameselect"))
			{
				PlayerNameBack	= NextValue();
				PlayerNameTitle = NextValue();
			}
			else if(!stricmp(szAtom, "acceptname"))
			{
				PlayerNameMenu[0].X			= NextValue();
				PlayerNameMenu[0].Y			= NextValue();
				AcceptName.Image_Number		= NextValue();
				AcceptName.Width			= NextValue();
				AcceptName.Height			= NextValue();
				AcceptName.Image_X			= NextValue();
				AcceptName.Image_Y			= NextValue();
				AcceptName.Mover_X			= NextValue();
				AcceptName.Mover_Y			= NextValue();
				AcceptName.Animation		= NextValue();
				AcceptName.AnimationOver	= NextValue();
			}
			else if(!stricmp(szAtom, "cancelname"))
			{
				PlayerNameMenu[1].X			= NextValue();
				PlayerNameMenu[1].Y			= NextValue();
				CancelName.Image_Number		= NextValue();
				CancelName.Width			= NextValue();
				CancelName.Height			= NextValue();
				CancelName.Image_X			= NextValue();
				CancelName.Image_Y			= NextValue();
				CancelName.Mover_X			= NextValue();
				CancelName.Mover_Y			= NextValue();
				CancelName.Animation		= NextValue();
				CancelName.AnimationOver	= NextValue();
			}
			else if(!stricmp(szAtom, "playername"))
			{
				PlayerNameMenu[2].X	= NextValue();
				PlayerNameMenu[2].Y	= NextValue();
				PlayerName.Font		= NextFont();
				PlayerName.Set_X	= PlayerNameMenu[2].X;
				PlayerName.Set_Y	= PlayerNameMenu[2].Y;
				PlayerName.Width	= NextValue();
			}
			else if(!stricmp(szAtom, "resetname"))
			{
				PlayerNameMenu[3].X			= NextValue();
				PlayerNameMenu[3].Y			= NextValue();
				DefaultName.Image_Number	= NextValue();
				DefaultName.Width			= NextValue();
				DefaultName.Height			= NextValue();
				DefaultName.Image_X			= NextValue();
				DefaultName.Image_Y			= NextValue();
				DefaultName.Mover_X			= NextValue();
				DefaultName.Mover_Y			= NextValue();
				DefaultName.Animation		= NextValue();
				DefaultName.AnimationOver	= NextValue();
			}
// end change
			else if(!stricmp(szAtom, "loadmsg"))
			{
				LoadFont = NextFont();
				strcpy(Loadmsg, strtok(NULL,"\n"));
			}
			else if(!stricmp(szAtom, "savemsg"))
			{
				SaveFont = NextFont();
				SavingTime = (float)NextValue();
				strcpy(Savemsg, strtok(NULL,"\n"));
			}
// end change RF063
			else if(!stricmp(szAtom, "difficult"))
			{
				DifficultBack	= NextValue();
				DifficultTitle	= NextValue();
			}
			else if(!stricmp(szAtom, "easy"))
			{
				DifficultMenu[0].X			= NextValue();
				DifficultMenu[0].Y			= NextValue();
				Difficultlow.Image_Number	= NextValue();
				Difficultlow.Width			= NextValue();
				Difficultlow.Height			= NextValue();
				Difficultlow.Image_X		= NextValue();
				Difficultlow.Image_Y		= NextValue();
				Difficultlow.Mover_X		= NextValue();
				Difficultlow.Mover_Y		= NextValue();
				Difficultlow.Animation		= NextValue();
				Difficultlow.AnimationOver	= NextValue();
			}
			else if(!stricmp(szAtom, "normal"))
			{
				DifficultMenu[1].X			= NextValue();
				DifficultMenu[1].Y			= NextValue();
				Difficultmid.Image_Number	= NextValue();
				Difficultmid.Width			= NextValue();
				Difficultmid.Height			= NextValue();
				Difficultmid.Image_X		= NextValue();
				Difficultmid.Image_Y		= NextValue();
				Difficultmid.Mover_X		= NextValue();
				Difficultmid.Mover_Y		= NextValue();
				Difficultmid.Animation		= NextValue();
				Difficultmid.AnimationOver	= NextValue();
			}
			else if(!stricmp(szAtom, "hard"))
			{
				DifficultMenu[2].X			= NextValue();
				DifficultMenu[2].Y			= NextValue();
				Difficulthi.Image_Number	= NextValue();
				Difficulthi.Width			= NextValue();
				Difficulthi.Height			= NextValue();
				Difficulthi.Image_X			= NextValue();
				Difficulthi.Image_Y			= NextValue();
				Difficulthi.Mover_X			= NextValue();
				Difficulthi.Mover_Y			= NextValue();
				Difficulthi.Animation		= NextValue();
				Difficulthi.AnimationOver	= NextValue();
			}

			if(!stricmp(szAtom, "xhairtext"))
			{
				free(xhair_text.text);
				xhair_text.text = strdup(NextString());
			}
			else if(!stricmp(szAtom, "revtext"))
			{
				free(rev_text.text);
				rev_text.text = strdup(NextString());
			}
			else if(!stricmp(szAtom, "mfiltertext"))
			{
				free(filter_text.text);
				filter_text.text = strdup(NextString());
			}
			else if(!stricmp(szAtom, "sensitivetext"))
			{
				free(sens_text.text);
				sens_text.text = strdup(NextString());
			}
			else if(!stricmp(szAtom, "vgammatext"))
			{
				free(gam_text.text);
				gam_text.text = strdup(NextString());
			}
			else if(!stricmp(szAtom, "vdetailtext"))
			{
				free(det_text.text);
				det_text.text = strdup(NextString());
			}
// changed QD Shadows
			else if(!stricmp(szAtom, "vshadowtext"))
			{
				free(shadow_text.text);
				shadow_text.text = strdup(NextString());
			}
// end change
			else if(!stricmp(szAtom, "volumetext"))
			{
				free(vol_text.text);
				vol_text.text = strdup(NextString());
			}
			else if(!stricmp(szAtom, "musicvoltext"))
			{
				free(mvol_text.text);
				mvol_text.text = strdup(NextString());
			}
			else if(!stricmp(szAtom, "cdplaytext"))
			{
				free(cd_text.text);
				cd_text.text = strdup(NextString());
			}
			else if(!stricmp(szAtom, "debuggingtext"))
			{
				free(debug_text.text);
				debug_text.text = strdup(NextString());
			}
			else if(!stricmp(szAtom, "fratetext"))
			{
				free(fps_text.text);
				fps_text.text = strdup(NextString());
			}
			else if(!stricmp(szAtom, "clippingtext"))
			{
				free(clip_text.text);
				clip_text.text = strdup(NextString());
			}
			else if(!stricmp(szAtom, "bboxtext"))
			{
				free(bb_text.text);
				bb_text.text = strdup(NextString());
			}
			else if(!stricmp(szAtom, "enitybbtext"))
			{
				free(sebb_text.text);
				sebb_text.text = strdup(NextString());
			}
			else if(!stricmp(szAtom, "ipaddtext"))
			{
				free(IPAdd_Text.text);
				IPAdd_Text.text = strdup(NextString());
			}
			else if(!stricmp(szAtom, "mphelptext"))
			{
				free(MultiplayerHelp_Text.text);
				MultiplayerHelp_Text.text = strdup(NextString());
			}
			else if(!stricmp(szAtom, "iptext"))
			{
				free(PlayerIP_Text.text);
				PlayerIP_Text.text = strdup(NextString());
			}
			else if(!stricmp(szAtom, "gamereturntext"))
			{
				free(return_text.text);
				return_text.text = strdup(NextString());
			}
/*
			else
			{
				char szBug[256];
				sprintf(szBug, "Unknown Menu command %s", szInputLine);
				CCD->ReportError(szBug, false);
			}
*/
		}

// changed RF063
		geVFile_Close(SecondFS);
// end change RF063
	}
	else
	{
		CCD->ReportError("*ERROR* Missing menu INI file", false);
		CCD->ShutdownLevel();
		delete CCD;
		CCD = NULL;
		MessageBox(NULL, "Missing menu INI file","Fatal Error", MB_OK);
		exit(-336);
	}
}

/* ------------------------------------------------------------------------------------ */
//	Destructor
/* ------------------------------------------------------------------------------------ */
CRFMenu::~CRFMenu()
{
	int i;

	if(L1text.text)
		free(L1text.text);
	if(L2text.text)
		free(L2text.text);
	if(L3text.text)
		free(L3text.text);
	if(L4text.text)
		free(L4text.text);
	if(L5text.text)
		free(L5text.text);

	L1text.text = NULL;
	L2text.text = NULL;
	L3text.text = NULL;
	L4text.text = NULL;
	L5text.text = NULL;

	free(return_text.text);
	free(PlayerIP_Text.text);
	free(MultiplayerHelp_Text.text);
	free(IPAdd_Text.text);
	free(sebb_text.text);
	free(bb_text.text);
	free(clip_text.text);
	free(fps_text.text);
	free(debug_text.text);
	free(cd_text.text);
	free(mvol_text.text);
	free(vol_text.text);
	free(det_text.text);
	free(gam_text.text);
	free(sens_text.text);
	free(filter_text.text);
	free(rev_text.text);
	free(xhair_text.text);
// changed QD Shadows
	free(shadow_text.text);
// end change

	// free click sounds
	geSound_FreeSoundDef(CCD->Engine()->AudioSystem(), mouseclick );
	geSound_FreeSoundDef(CCD->Engine()->AudioSystem(), keyclick );
	geSound_FreeSoundDef(CCD->Engine()->AudioSystem(), slideclick );

	// delete the bitmaps
	for(i=0; i<NUM_BACKGROUNDS; i++)
	{
		if(Backgrounds[i] != (geBitmap*)NULL)
		{
			geEngine_RemoveBitmap(CCD->Engine()->Engine(), Backgrounds[i]);
			geBitmap_Destroy(&Backgrounds[i]);
		}
	}

	for(i=0; i<NUM_IMAGES; i++)
	{
		if(Images[i] != (geBitmap*)NULL)
		{
			geEngine_RemoveBitmap(CCD->Engine()->Engine(), Images[i]);
			geBitmap_Destroy(&Images[i]);
		}
	}

	for(i=0; i<NUM_TITLES; i++)
	{
		if(Titles[i] != (geBitmap*)NULL)
		{
			geEngine_RemoveBitmap(CCD->Engine()->Engine(), Titles[i]);
			geBitmap_Destroy(&Titles[i]);
		}
	}

	for(i=0; i<NUM_FONTS; i++)
	{
		if(MenuFont[i].Bitmap != (geBitmap*)NULL)
		{
			geEngine_RemoveBitmap(CCD->Engine()->Engine(), MenuFont[i].Bitmap);
			geBitmap_Destroy(&MenuFont[i].Bitmap);
		}
	}

// changed RF063
	for(i=0; i<NUM_ANIM; i++)
	{
		if(Animation[i])
		{
			delete Animation[i];
			Animation[i] = 0;
		}
	}
// end change RF063

// changed QD 12/15/05
	if(EmptySlotImage)
	{
		geEngine_RemoveBitmap(CCD->Engine()->Engine(), EmptySlotImage);
		geBitmap_Destroy(&EmptySlotImage);
	}
// end change

// changed Nout 12/15/05
	if(Cursor)
	{
		geEngine_RemoveBitmap(CCD->Engine()->Engine(), Cursor);
		geBitmap_Destroy(&Cursor);
	}

	if(Crosshair)
	{
		geBitmap_Destroy(&Crosshair);
	}

	if(FCrosshair)
	{
		geEngine_RemoveBitmap(CCD->Engine()->Engine(), FCrosshair);
		geBitmap_Destroy(&FCrosshair);
	}
// end change

#ifdef BLIT
	if(ScreenBmp)
		geBitmap_Destroy(&ScreenBmp);
#endif

	if(CCD->GetCSelect())
	{
		if(MaxSelect>0)
		{
			for(i=0; i<MaxSelect; i++)
			{
				if(CharSelect[i].Bitmap)
				{
					geEngine_RemoveBitmap(CCD->Engine()->Engine(), CharSelect[i].Bitmap);
					geBitmap_Destroy(&CharSelect[i].Bitmap);
				}
			}
		}
	}

	for(i=0; i<16; i++)
	{
		if(SavedGame[i].text)
			free(SavedGame[i].text);

// changed QD 12/15/05
		if(SavedGame[i].SGImage)
		{
			geEngine_RemoveBitmap(CCD->Engine()->Engine(), SavedGame[i].SGImage);
			geBitmap_Destroy(&(SavedGame[i].SGImage));
		}
// end change
	}

// changed QD 12/15/05 delete temporary savescreen.bmp
	char filename[256];
	sprintf(filename, "%s\\SaveScreen.bmp", CCD->GetDirectory(kBitmapFile));
	unlink(filename);

// changed RF064
	CMixer mixer(CCD->Engine()->WindowHandle(), MIXERLINE_COMPONENTTYPE_DST_SPEAKERS,
		NO_SOURCE, MIXERCONTROL_CONTROLTYPE_VOLUME);

	if(mixer.IsOk())
		mixer.SetControlValue(WinVol);

	if(theMIDIPlayer != NULL)
	{
		delete theMIDIPlayer;
		theMIDIPlayer = NULL;
	}
// end change RF064

	return;
}

/* ------------------------------------------------------------------------------------ */
//	DoMenu
/* ------------------------------------------------------------------------------------ */
int CRFMenu::DoMenu(char *levelname)
{
// 08.05.2004 - begin change gekido
	CCD->ReportError("Entering CRFMenu::DoMenu()", false);
// 08.05.2004 - end change gekido

	M_CameraRect.Left	= 0;
	M_CameraRect.Right	= CCD->Engine()->Width() - 1;
	M_CameraRect.Top	= 0;
	M_CameraRect.Bottom = CCD->Engine()->Height() - 1;
	M_Camera = geCamera_Create(2.0f, &M_CameraRect);
	strcpy(LevelName, levelname);
	LoopOnce = 0;

	if(musictype != -1)
	{
// changed RF064
		if(musictype == 1)
			MIDIPlayer()->Play(music, true);
		else
			m_Streams->Play(true);
	}

	FadeSet(-1, TimeFade);

// 08.05.2004 - begin change gekido
//	CCD->ReportError("Entering ProcessMenu...", false);
// 08.05.2004 - end change gekido
	int ret = ProcessMenu(MainMenu, MainBack, MainTitle);

	if(musictype != -1)
	{
		if(musictype == 1)
			MIDIPlayer()->Stop();
		else
		{
			m_Streams->Delete();
			delete m_Streams;
		}
	}

// 08.05.2004 - begin change gekido
	CCD->ReportError("Saving Attributes & Settings files...", false);
// 08.05.2004 - end change gekido

	if(ingame == 1)
		CCD->Player()->SaveAttributesAscii("attributes.txt");
// end change RF064

	FILE *fd = CCD->OpenRFFile(kInstallFile, "setup.ini", "wb");

	if(!fd)
		CCD->ReportError("*WARNING* CRFMenu: Failed to create setup.ini file", false);
	else
	{
		fwrite(&Gamma_Slide.Current,	sizeof(int), 1, fd);
		fwrite(&Vol_Slide.Current,		sizeof(int), 1, fd);
		fwrite(&Sens_Slide.Current,		sizeof(int), 1, fd);
		fwrite(&box0.Current,			sizeof(int), 1, fd);
		fwrite(&box1.Current,			sizeof(int), 1, fd);
		fwrite(&box2.Current,			sizeof(int), 1, fd);
		fwrite(&box4.Current,			sizeof(int), 1, fd);
		fwrite(&Detail_Slide.Current,	sizeof(int), 1, fd);
		fwrite(&mVol_Slide.Current,		sizeof(int), 1, fd);
// changed QD Shadows
		fwrite(&ShadowBox.Current,		sizeof(int), 1, fd);
// end change
		fclose(fd);
	}

// 08.05.2004 - begin change gekido
	CCD->ReportError("Destroying Camera...", false);
// 08.05.2004 - end change gekido

	geCamera_Destroy(&M_Camera);

	return ret;
}

/* ------------------------------------------------------------------------------------ */
//	ProcessMenu
/* ------------------------------------------------------------------------------------ */
int CRFMenu::ProcessMenu(MenuItem *Menu, int Background_Number, int Title_Number)
{
	int x, y, i, max, click, temp;
	int slide_click, box_click, radio_click, scroll_click, remap_click, lsbox_click;
	int slide_x, escapeon, scroll_dir, remap_line, focus, remapf, lsbox_line;
	geBitmap_Info	BmpInfo;
	geRect 	 BitRect;
	POINT temppos;
	Clickable *data;
	Image *idata;
	Slider *sdata;
	Box *bdata;
// changed QD Language Menu
	Radio *radiodata;
// end change QD
	Text *tdata;
	Remap *rdata;
	ScrollBar *scdata;
	LSBox *lrdata;
// start multiplayer
	int textedit_click;
	TextEdit *tedata;
// end multiplayer

// 08.05.2004 - begin change gekido
	CCD->ReportError("Entering CRFMenu::ProcessMenu", false);
// 08.05.2004 - end change gekido

	x = (CCD->Engine()->Width() - DesignX) / 2;
	y = (CCD->Engine()->Height() - DesignY) / 2;
	int bx = 0;
	int by = 0;

// changed QD 12/15/05 - safety check
	if(Background_Number >= 0 && Background_Number < NUM_BACKGROUNDS)
	{
		if(Backgrounds[Background_Number])
		{
			geBitmap_GetInfo(Backgrounds[Background_Number], &BmpInfo, NULL);

			if(CCD->Engine()->Width() > BmpInfo.Width)
				bx = (CCD->Engine()->Width() - BmpInfo.Width) / 2;

			if(CCD->Engine()->Height() > BmpInfo.Height)
				by = (CCD->Engine()->Height() - BmpInfo.Height) / 2;
		}
	}
// end change

	max = 0;

	while(Menu[max].Type != END_LIST)
		max += 1;

	click					= -1;
	slide_click				= -1;
	slide_x					= -1;
	box_click				= -1;
// changed QD Language Menu
	radio_click				= -1;
// end change QD
	escapeon				= 0;
	scroll_click			= -1;
	scroll_dir				= 0;
	remap_click				= -1;
	remapf					= -1;
	focus					= -1;
	lsbox_click				= -1;
// Show save game image
//add Nout
//	char filename[64];
//  geBitmap *savescr		= (geBitmap*)NULL;
//	geBitmap *savescrbmp	= (geBitmap*)NULL;
//	LoadSGImage				= GE_TRUE;
//	int LastCurrent			= 0;
//end Nout
// start multiplayer
	textedit_click			= -1;
// end multiplayer

	POINT pos;

	if(!CCD->Engine()->FullScreen())
	{
		RECT client;
		GetClientRect(CCD->Engine()->WindowHandle(),&client);
		pos.x = client.left;
		pos.y = client.top;
		ClientToScreen(CCD->Engine()->WindowHandle(),&pos);
	}

// 08.05.2004 - begin change gekido
	CCD->ReportError("Entering Windows Message Loop, Rendering Game Menu", false);
// 08.05.2004 - end change gekido

	for(;;)
	{
		MSG msg;

		// If Winblows has something to say, take it in and pass it on in the
		// ..off-chance someone cares.
		while (PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE))
		{
			GetMessage(&msg, NULL, 0, 0 );
			// start multiplayer
			TranslateMessage(&msg);
			// end multiplayer
			DispatchMessage(&msg);
		}

		if(!CCD->GetHasFocus())
			continue;

// begin add Nout - Show save game image
/*		if(SaveScreen.Width > 0 && SaveScreen.Height > 0 && LoadSGImage == GE_TRUE)
		{
			if(LoadBox.text[LoadBox.Current].empty != 0 && SaveBox.text[SaveBox.Current].empty != 0)
				sprintf(filename, "SaveScreen%d.bmp", LastCurrent);
			else
				strcpy(filename, SaveScreen.EmptySlotImage);

			if(!EffectC_IsStringNull(filename))
			{
				geBitmap *savescr = CreateFromFileName(filename);

				if(savescr == (geBitmap*)NULL)
				{
					char szBug[256];
					sprintf(szBug, "File %s - Line %d: Bad file name %s", __FILE__, __LINE__, filename);
					CCD->ReportError(szBug, false);
					exit(-100);
				}

				geBitmap_GetInfo(savescr, &BmpInfo, NULL);
				savescrbmp = geBitmap_Create(BmpInfo.Width, BmpInfo.Height, BmpInfo.MaximumMip+1, BmpInfo.Format);
				geBitmap_BlitBitmap(savescr, savescrbmp);

				if(savescr)
					geBitmap_Destroy(&savescr);

				if(savescrbmp == (geBitmap*)NULL)
				{
					char szBug[256];
					sprintf(szBug, "File %s - Line %d: Bad SaveGame Screenshot File Name %s",
							__FILE__, __LINE__, filename);
					CCD->ReportError(szBug, false);
					exit(-100);
				}
				else
					geBitmap_SetColorKey(savescrbmp, GE_TRUE, 255, GE_FALSE);

				geEngine_AddBitmap(CCD->Engine()->Engine(), savescrbmp);
				LoadSGImage = GE_FALSE;
			}
		}*/
// end add Nout

		geEngine_BeginFrame(CCD->Engine()->Engine(), M_Camera, GE_TRUE);
// changed RF063
// changed QD 12/15/05
		if(Backgrounds[Background_Number] && Background_Number >= 0 && Background_Number < NUM_BACKGROUNDS)
			DrawBitmap(Backgrounds[Background_Number], NULL, bx, by);
// end change

		if(MTitles[Title_Number].Animation < 0 || Animation[MTitles[Title_Number].Animation] == NULL)
		{
			BitRect.Left	= MTitles[Title_Number].Image_X;
			BitRect.Top		= MTitles[Title_Number].Image_Y;
			BitRect.Right	= MTitles[Title_Number].Image_X+MTitles[Title_Number].Width;
			BitRect.Bottom	= MTitles[Title_Number].Image_Y+MTitles[Title_Number].Height;
			DrawBitmap(Titles[MTitles[Title_Number].Image_Number], &BitRect, MTitles[Title_Number].X+x, MTitles[Title_Number].Y+y);
		}
		else
		{
			geBitmap *theBmp = Animation[MTitles[Title_Number].Animation]->NextFrame(true);
			DrawBitmap(theBmp, NULL, MTitles[Title_Number].X+x, MTitles[Title_Number].Y+y);
		}
// end change RF063

		GetCursorPos(&temppos);

		if(!CCD->Engine()->FullScreen())
		{
			temppos.x -= pos.x;
			temppos.y -= pos.y;
		}

		if(Fading)
		{
			temppos.x = -1;
			temppos.y = -1;
		}

		for(i=0; i<max; i++)
		{
// changed QD 12/15/05 - replaced separate if(Menu[i].Type == ...) statements with if - else if
// changed RF063
			switch(Menu[i].Type)
			{
			case CLICKABLE:
			case EXIT_MENU:
			case CANCEL_MENU:
				//if(Menu[i].Type == CLICKABLE || Menu[i].Type == EXIT_MENU || Menu[i].Type == CANCEL_MENU)
				{
					data = (Clickable*)Menu[i].data;

					if((temppos.x >= Menu[i].X+x)
						&& (temppos.x <= (Menu[i].X + x + data->Width))
						&& (temppos.y >= (Menu[i].Y + y))
						&& (temppos.y <= (Menu[i].Y + y + data->Height))
						&& slide_click == -1)
					{
						if(data->Action == 2 && ingame == 0)
						{
							if(data->Animation < 0 || Animation[data->Animation] == NULL)
							{
								BitRect.Left	= data->Image_X;
								BitRect.Top		= data->Image_Y;
								BitRect.Right	= data->Image_X+data->Width;
								BitRect.Bottom	= data->Image_Y+data->Height;

								if(data->Width > 0 && data->Height > 0)
									DrawBitmap(Images[data->Image_Number], &BitRect, Menu[i].X+x, Menu[i].Y+y);
							}
							else
							{
								geBitmap *theBmp = Animation[data->Animation]->NextFrame(true);
								DrawBitmap(theBmp, NULL, Menu[i].X+x, Menu[i].Y+y);
							}
						}
						else
						{
							if(data->AnimationOver < 0 || Animation[data->AnimationOver] == NULL)
							{
								BitRect.Left	= data->Mover_X;
								BitRect.Top		= data->Mover_Y;
								BitRect.Right	= data->Mover_X+data->Width;
								BitRect.Bottom	= data->Mover_Y+data->Height;

								if(data->Width > 0 && data->Height > 0)
									DrawBitmap(Images[data->Image_Number], &BitRect, Menu[i].X+x, Menu[i].Y+y);
							}
							else
							{
								geBitmap *theBmp = Animation[data->AnimationOver]->NextFrame(true);
								DrawBitmap(theBmp, NULL, Menu[i].X+x, Menu[i].Y+y);
							}
// changed QD 02/01/07
							if(GetSystemMetrics(SM_SWAPBUTTON))
							{
								if((GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0 && focus == -1)
									click = i;
							}
							else
							{
								if((GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0 && focus == -1)
									click = i;
							}
// end change
						}
					}
					else
					{
						if(data->Animation < 0 || Animation[data->Animation] == NULL)
						{
							BitRect.Left	= data->Image_X;
							BitRect.Top		= data->Image_Y;
							BitRect.Right	= data->Image_X+data->Width;
							BitRect.Bottom	= data->Image_Y+data->Height;

							if(data->Width > 0 && data->Height > 0)
								DrawBitmap(Images[data->Image_Number], &BitRect, Menu[i].X+x, Menu[i].Y+y);
						}
						else
						{
							geBitmap *theBmp = Animation[data->Animation]->NextFrame(true);
							DrawBitmap(theBmp, NULL, Menu[i].X+x, Menu[i].Y+y);
						}
					}

					break;
				}
// end change RF063
			case IMAGE:	//else if(Menu[i].Type == IMAGE)
				{
					idata = (Image*)Menu[i].data;

					if(idata->Animation < 0 || Animation[idata->Animation] == NULL)
					{
						BitRect.Left	= idata->Image_X;
						BitRect.Top		= idata->Image_Y;
						BitRect.Right	= idata->Image_X+idata->Width;
						BitRect.Bottom	= idata->Image_Y+idata->Height;

						if(idata->Width > 0 && idata->Height > 0)
							DrawBitmap(Images[idata->Image_Number], &BitRect, Menu[i].X+x, Menu[i].Y+y);
					}
					else
					{
						geBitmap *theBmp = Animation[idata->Animation]->NextFrame(true);
						DrawBitmap(theBmp, NULL, Menu[i].X+x, Menu[i].Y+y);
					}

					break;
				}
// changed RF063
			case CHARIMAGE: //else if(Menu[i].Type == CHARIMAGE)
				{
					DrawBitmap(CharSelect[CurrentSelect].Bitmap, NULL, Menu[i].X+x, Menu[i].Y+y);

					break;
				}
// end change RF063
			case SLIDER: //else if(Menu[i].Type == SLIDER)
				{
					sdata = (Slider*)Menu[i].data;
					temp = sdata->Current;

// changed RF063
					if(sdata->Animation < 0 || Animation[sdata->Animation] == NULL)
					{
						BitRect.Left	= sdata->Image_X;
						BitRect.Top		= sdata->Image_Y;
						BitRect.Right	= sdata->Image_X+sdata->Width;
						BitRect.Bottom	= sdata->Image_Y+sdata->Height;

						if(sdata->Width > 0 && sdata->Height > 0)
							DrawBitmap(Images[sdata->Image_Number], &BitRect, Menu[i].X+x+temp, Menu[i].Y+y);
					}
					else
					{
						geBitmap *theBmp = Animation[sdata->Animation]->NextFrame(true);
						DrawBitmap(theBmp, NULL, Menu[i].X+x+temp, Menu[i].Y+y);
					}
// end change RF063

					if((temppos.x >= Menu[i].X+x+temp)
						&& (temppos.x <= (Menu[i].X+x+temp+sdata->Width))
						&& (temppos.y >= (Menu[i].Y+y))
						&& (temppos.y <= (Menu[i].Y+y+data->Height))
						&& (slide_click == -1))
					{
// changed QD 02/01/07
						if(GetSystemMetrics(SM_SWAPBUTTON))
						{
							if((GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0 && focus == -1)
								slide_click = i;
						}
						else
						{
							if((GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0 && focus == -1)
								slide_click = i;
						}
// end change

						if(slide_x == -1)
							slide_x = temppos.x;
					}

					break;
				}
			case BOX: // else if(Menu[i].Type == BOX)
				{
					bdata = (Box*)Menu[i].data;

					if((temppos.x >= Menu[i].X + x)
						&& (temppos.x <= (Menu[i].X + x + bdata->Width))
						&& (temppos.y >= (Menu[i].Y + y))
						&& (temppos.y <= (Menu[i].Y + y + bdata->Height))
						&& (slide_click == -1)
						&& (bdata->Current == 0))
					{
						if(bdata->AnimationOver < 0 || Animation[bdata->AnimationOver] == NULL)
						{
							BitRect.Left	= bdata->Mover_X;
							BitRect.Top		= bdata->Mover_Y;
							BitRect.Right	= bdata->Mover_X+bdata->Width;
							BitRect.Bottom	= bdata->Mover_Y+bdata->Height;

							if(bdata->Width > 0 && bdata->Height > 0)
								DrawBitmap(Images[bdata->Image_Number], &BitRect, Menu[i].X+x, Menu[i].Y+y);
						}
						else
						{
							geBitmap *theBmp = Animation[bdata->AnimationOver]->NextFrame(true);
							DrawBitmap(theBmp, NULL, Menu[i].X+x, Menu[i].Y+y);
						}

// changed QD 02/01/07
						if(GetSystemMetrics(SM_SWAPBUTTON))
						{
							if((GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0 && focus == -1)
								box_click = i;
						}
						else
						{
							if((GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0 && focus == -1)
								box_click = i;
						}
// end change
					}
					else
					{
						if((temppos.x >= Menu[i].X+x)
							&& (temppos.x <= (Menu[i].X+x+bdata->Width))
							&& (temppos.y >= (Menu[i].Y+y))
							&& (temppos.y <= (Menu[i].Y+y+bdata->Height))
							&& (slide_click == -1))
						{
// changed QD 02/01/07
							if(GetSystemMetrics(SM_SWAPBUTTON))
							{
								if((GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0 && focus == -1)
									box_click = i;
							}
							else
							{
								if((GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0 && focus == -1)
									box_click = i;
							}
// end change
						}

						if(bdata->Current == 0)
						{
							if(bdata->Animation < 0 || Animation[bdata->Animation] == NULL)
							{
								BitRect.Left	= bdata->Image_X;
								BitRect.Top		= bdata->Image_Y;
								BitRect.Right	= bdata->Image_X+bdata->Width;
								BitRect.Bottom	= bdata->Image_Y+bdata->Height;

								if(bdata->Width > 0 && bdata->Height > 0)
									DrawBitmap(Images[bdata->Image_Number], &BitRect, Menu[i].X+x, Menu[i].Y+y);
							}
							else
							{
								geBitmap *theBmp = Animation[bdata->Animation]->NextFrame(true);
								DrawBitmap(theBmp, NULL, Menu[i].X+x, Menu[i].Y+y);
							}
						}
						else
						{
							if(bdata->AnimationLit < 0 || Animation[bdata->AnimationLit] == NULL)
							{
								BitRect.Left	= bdata->Set_X;
								BitRect.Top		= bdata->Set_Y;
								BitRect.Right	= bdata->Set_X+bdata->Width;
								BitRect.Bottom	= bdata->Set_Y+bdata->Height;

								if(bdata->Width > 0 && bdata->Height > 0)
									DrawBitmap(Images[bdata->Image_Number], &BitRect, Menu[i].X+x, Menu[i].Y+y);
							}
							else
							{
								geBitmap *theBmp = Animation[bdata->AnimationLit]->NextFrame(true);
								DrawBitmap(theBmp, NULL, Menu[i].X+x, Menu[i].Y+y);
							}
						}
					}

					break;
				}
// changed QD Language Menu
			case RADIO: // else if(Menu[i].Type == RADIO)
				{
					radiodata = (Radio*)Menu[i].data;

					if((temppos.x >= Menu[i].X+x)
						&& (temppos.x <= (Menu[i].X+x+radiodata->Width))
						&& (temppos.y >= (Menu[i].Y+y))
						&& (temppos.y <= (Menu[i].Y+y+radiodata->Height))
						&& (slide_click == -1)
						&& (radiodata->Current == 0))
					{
						if(radiodata->AnimationOver < 0 || Animation[radiodata->AnimationOver] == NULL)
						{
							BitRect.Left	= radiodata->Mover_X;
							BitRect.Top		= radiodata->Mover_Y;
							BitRect.Right	= radiodata->Mover_X+radiodata->Width;
							BitRect.Bottom	= radiodata->Mover_Y+radiodata->Height;

							if(radiodata->Width > 0 && radiodata->Height > 0)
								DrawBitmap(Images[radiodata->Image_Number], &BitRect, Menu[i].X+x, Menu[i].Y+y);
						}
						else
						{
							geBitmap *theBmp = Animation[radiodata->AnimationOver]->NextFrame(true);
							DrawBitmap(theBmp, NULL, Menu[i].X+x, Menu[i].Y+y);
						}

// changed QD 02/01/07
						if(GetSystemMetrics(SM_SWAPBUTTON))
						{
							if((GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0 && focus == -1)
								radio_click = i;
						}
						else
						{
							if((GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0 && focus == -1)
								radio_click = i;
						}
// end change
					}
					else
					{
						if((temppos.x >= Menu[i].X+x)
							&& (temppos.x <= (Menu[i].X+x+radiodata->Width))
							&& (temppos.y >= (Menu[i].Y+y))
							&& (temppos.y <= (Menu[i].Y+y+radiodata->Height))
							&& (slide_click == -1))
						{
// changed QD 02/01/07
							if(GetSystemMetrics(SM_SWAPBUTTON))
							{
								if((GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0 && focus == -1)
									radio_click = i;
							}
							else
							{
								if((GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0 && focus == -1)
									radio_click = i;
							}
// end change
						}

						if(radiodata->Current == 0)
						{
							if(radiodata->Animation < 0 || Animation[radiodata->Animation] == NULL)
							{
								if(radiodata->Width > 0 && radiodata->Height > 0)
								{
									BitRect.Left	= radiodata->Image_X;
									BitRect.Top		= radiodata->Image_Y;
									BitRect.Right	= radiodata->Image_X+radiodata->Width;
									BitRect.Bottom	= radiodata->Image_Y+radiodata->Height;
									DrawBitmap(Images[radiodata->Image_Number], &BitRect, Menu[i].X+x, Menu[i].Y+y);
								}
							}
							else
							{
								geBitmap *theBmp = Animation[radiodata->Animation]->NextFrame(true);
								DrawBitmap(theBmp, NULL, Menu[i].X+x, Menu[i].Y+y);
							}
						}
						else
						{
							if(radiodata->AnimationLit < 0 || Animation[radiodata->AnimationLit] == NULL)
							{
								BitRect.Left	= radiodata->Set_X;
								BitRect.Top		= radiodata->Set_Y;
								BitRect.Right	= radiodata->Set_X+radiodata->Width;
								BitRect.Bottom	= radiodata->Set_Y+radiodata->Height;

								if(radiodata->Width > 0 && radiodata->Height > 0)
									DrawBitmap(Images[radiodata->Image_Number], &BitRect, Menu[i].X+x, Menu[i].Y+y);
							}
							else
							{
								geBitmap *theBmp = Animation[radiodata->AnimationLit]->NextFrame(true);
								DrawBitmap(theBmp, NULL, Menu[i].X+x, Menu[i].Y+y);
							}
						}
					}

					break;
				}
// end change QD
			case TEXT: // else if(Menu[i].Type == TEXT)
				{
					char *s;

					tdata = (Text*)Menu[i].data;

					if(tdata->Font != -1)
					{
						s = tdata->text;

						if(tdata->ingame == 0 || ingame == 1)
							MFontRect(s, tdata->Font, Menu[i].X+x, Menu[i].Y+y);
					}

					break;
				}
// start multiplayer
			case TEXTEDIT: //else if(Menu[i].Type == TEXTEDIT)
				{
					tedata = (TextEdit*)Menu[i].data;
					Keyname *KTEdata = tedata->keyname;

					int32 Size;

					if((temppos.x >= Menu[i].X+x)
						&& (temppos.x <= Menu[i].X+x+tedata->Width)
						&& (temppos.y >= Menu[i].Y+y)
						&& (temppos.y <= Menu[i].Y+y+30))
					{
// changed QD 02/01/07
						if(GetSystemMetrics(SM_SWAPBUTTON))
						{
							if((GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0 && textedit_click == -1)
							{
								strcpy(tedata->text, "_");
								geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), keyclick, 0.99f, 0.5f, 1.0f, false);
								textedit_click = i;
								focus = -1;
							}
						}
						else
						{
							if((GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0 && textedit_click == -1)
							{
								strcpy(tedata->text, "_");
								geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), keyclick, 0.99f, 0.5f, 1.0f, false);
								textedit_click = i;
								focus = -1;
							}
						}
// end change
					}

					if((GetAsyncKeyState(VK_RETURN) & 0x8000) != 0 && textedit_click == i)
					{
						//focus=-1;
						Size = strlen( tedata->text );
						tedata->text[Size-1] = '\0';
						strcpy(ServerIP,tedata->text);
						geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), keyclick, 0.99f, 0.5f, 1.0f, false);
						textedit_click = -1;
					}

					if(textedit_click == i && focus == -1)
					{
						// delete a character...
						if(((GetAsyncKeyState(VK_BACK) & 0x8000) != 0) || ((GetAsyncKeyState(VK_LEFT) & 0x8000) != 0))
						{
							// do nothing if there are no more characters left
							Size = strlen(tedata->text);

							// changed QD 12/15/05
							// strlen returns the number of characters, _excluding_ the terminating null character
							if(Size >= 2) //2)
							{
								// delete as character
								tedata->text[Size-2] = '\0';
								strcat(tedata->text, "_");
								geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), mouseclick, 0.99f, 0.0f, 1.0f, false);
							}
						}

						int keybrd = CCD->Input()->GetKeyboardInput();

						if(keybrd != -1)
						{
							char *s = NULL;
							int idex = 0;

							while(KTEdata[idex].text != NULL)
							{
								if(keybrd == KTEdata[idex].key)
								{
									s = KTEdata[idex].text;
									break;
								}

								idex++;
							}

							if(s != NULL)
							{
								// do nothing is there is no more room to add characters
								Size = strlen(tedata->text);

								if(Size+2 <= sizeof(tedata->text))
								{
									tedata->text[Size-1] = s[0];
									tedata->text[Size] = '\0';
									strcat(tedata->text, "_");

									//strcat(tedata->text,s);
									geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), keyclick, 0.99f, 0.5f, 1.0f, false);
									//textedit_click = i;
								}
							}
						}
					}

					MFontRect(tedata->text, tedata->Font, tedata->Set_X+x, tedata->Set_Y+y);

					break;
				}
// end multiplayer
			case REMAP: // else if(Menu[i].Type == REMAP)
				{
					rdata = (Remap*)Menu[i].data;
					Keydef *Kdata = rdata->keydef;
					Keyname *KNdata = rdata->keyname;

					if(rdata->Max == -1)
					{
						rdata->Max = 0;

						while(Kdata[rdata->Max].text != NULL)
						{
							rdata->Max += 1;
						}
					}

					BitRect.Left	= rdata->Image_X;
					BitRect.Top		= rdata->Image_Y;
					BitRect.Right	= rdata->Image_X+rdata->Width;
					BitRect.Bottom	= rdata->Image_Y+rdata->Height;

					// draw remap box background
					if(rdata->Width > 0 && rdata->Height > 0)
						DrawBitmap(Images[rdata->Image_Number], &BitRect, Menu[i].X+x, Menu[i].Y+y);

					if((GetAsyncKeyState(VK_RETURN) & 0x8000) == 0 && focus == i)
					{
						int keybrd = CCD->Input()->GetKeyboardInput();

						if(keybrd != -1)
						{
							char *s = NULL;
							int idex = 0;

							while(KNdata[idex].text != NULL)
							{
								if(keybrd == KNdata[idex].key)
								{
									s = KNdata[idex].text;
									break;
								}

								idex++;
							}

							if(s != NULL)
							{
								geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), keyclick, 0.99f, 0.5f, 1.0f, false);
								CCD->Input()->SetKeyAction(CCD->Input()->GetCodes(Kdata[remapf].action), RGF_K_NOACTION);
								CCD->Input()->SetKeyAction(keybrd, Kdata[remapf].action);
								focus = -1;
							}
						}
					}

					int tempstep = 0;

					for(int j=rdata->Start; j<rdata->Max; j++)
					{
						if(j < (rdata->Start+rdata->Max_Show))
						{
							char *s;

							if(j == rdata->Current)
							{
								if((GetAsyncKeyState(VK_RETURN) & 0x8000) != 0 && focus == -1)
								{
									geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), keyclick, 0.99f, 0.5f, 1.0f, false);
									remapf = j;
									focus = i;
								}

								if(focus == i)
								{
									BitRect.Left	= rdata->RevC_X;
									BitRect.Top		= rdata->RevC_Y;
									BitRect.Right	= rdata->RevC_X+rdata->RevC_Width;
									BitRect.Bottom	= rdata->RevC_Y+rdata->RevC_Height;
								}
								else
								{
									BitRect.Left	= rdata->Rev_X;
									BitRect.Top		= rdata->Rev_Y;
									BitRect.Right	= rdata->Rev_X+rdata->Rev_Width;
									BitRect.Bottom	= rdata->Rev_Y+rdata->Rev_Height;
								}

								if(rdata->Width > 0 && rdata->Height > 0)
									DrawBitmap(Images[rdata->Image_Number], &BitRect, Menu[i].X+x+rdata->Corner_X, Menu[i].Y+y+rdata->Corner_Y+tempstep);
							}


							s = Kdata[j].text;
							MFontRect(s, rdata->Font, Menu[i].X+x+rdata->Start_X, Menu[i].Y+y+rdata->Start_Y+tempstep);
							s = NULL;
							int kcode = CCD->Input()->GetCodes(Kdata[j].action);

							if(kcode == -1)
								kcode = KEY_MAXIMUM;

							int idex = 0;

							while(KNdata[idex].text != NULL)
							{
								if(kcode == KNdata[idex].key)
								{
									s = KNdata[idex].text;
									break;
								}

								idex++;
							}

							if(rdata->Font != -1)
								MFontRect(s, rdata->Font, Menu[i].X+x+rdata->Start_X+rdata->Key_X, Menu[i].Y+y+rdata->Start_Y+tempstep);
						}

						tempstep += rdata->Step;
					}

					if((temppos.x >= Menu[i].X+x+rdata->Corner_X)
						&& (temppos.x <= (Menu[i].X+x+rdata->Corner_X+rdata->Click_Width))
						&& (temppos.y >= (Menu[i].Y+y+rdata->Corner_Y))
						&& (temppos.y <= (Menu[i].Y+y+rdata->Corner_Y+(rdata->Max_Show*rdata->Step)))
						&& (slide_click == -1))
					{
// changed QD 02/01/07
						if(GetSystemMetrics(SM_SWAPBUTTON))
						{
							if((GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0 && focus == -1)
							{
								remap_click = i;
								remap_line = (temppos.y-(Menu[i].Y+y+rdata->Corner_Y))/rdata->Step;
							}
						}
						else
						{
							if((GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0 && focus == -1)
							{
								remap_click = i;
								remap_line = (temppos.y-(Menu[i].Y+y+rdata->Corner_Y))/rdata->Step;
							}
						}
// end change
					}

					break;
				}
			case SCROLLBAR: // else if(Menu[i].Type == SCROLLBAR)
				{
					geBitmap *theBmp;
					bool up = false;
					bool dwn = false;
					scdata = (ScrollBar*)Menu[i].data;

					if(scdata->AnimationUp < 0 || Animation[scdata->AnimationUp] == NULL)
					{
						BitRect.Left	= scdata->Up_Nor_X;
						BitRect.Top		= scdata->Up_Nor_Y;
						BitRect.Right	= scdata->Up_Nor_X+scdata->Up_Width;
						BitRect.Bottom	= scdata->Up_Nor_Y+scdata->Up_Height;

						if(scdata->Up_Width > 0 && scdata->Up_Height > 0)
							DrawBitmap(Images[scdata->Image_Number], &BitRect, Menu[i].X+x+scdata->Up_X, Menu[i].Y+y+scdata->Up_Y);
					}
					else
					{
						up = true;
					}

					if(scdata->AnimationDwn < 0 || Animation[scdata->AnimationDwn] == NULL)
					{
						BitRect.Left	= scdata->Dwn_Nor_X;
						BitRect.Top		= scdata->Dwn_Nor_Y;
						BitRect.Right	= scdata->Dwn_Nor_X+scdata->Dwn_Width;
						BitRect.Bottom	= scdata->Dwn_Nor_Y+scdata->Dwn_Height;

						if(scdata->Dwn_Width > 0 && scdata->Dwn_Height > 0)
							DrawBitmap(Images[scdata->Image_Number], &BitRect, Menu[i].X+x+scdata->Dwn_X, Menu[i].Y+y+scdata->Dwn_Y);
					}
					else
					{
						dwn = true;
					}

					if((temppos.x >= Menu[i].X+x+scdata->Up_X)
						&& (temppos.x <= (Menu[i].X+x+scdata->Up_X+scdata->Up_Width))
						&& (temppos.y >= (Menu[i].Y+y+scdata->Up_Y))
						&& (temppos.y <= (Menu[i].Y+y+scdata->Up_Y+scdata->Up_Height))
						&& (slide_click == -1))
					{
// changed QD 02/01/07
						if((GetSystemMetrics(SM_SWAPBUTTON) && (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0 && focus == -1) ||
							(!GetSystemMetrics(SM_SWAPBUTTON) && (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0 && focus == -1))
// end change
						{
							if(scdata->AnimationUpPush < 0 || Animation[scdata->AnimationUpPush] == NULL)
							{
								BitRect.Left	= scdata->Up_Push_X;
								BitRect.Top		= scdata->Up_Push_Y;
								BitRect.Right	= scdata->Up_Push_X+scdata->Up_Width;
								BitRect.Bottom	= scdata->Up_Push_Y+scdata->Up_Height;

								if(scdata->Up_Width > 0 && scdata->Up_Height > 0)
									DrawBitmap(Images[scdata->Image_Number], &BitRect, Menu[i].X+x+scdata->Up_X, Menu[i].Y+y+scdata->Up_Y);
							}
							else
							{
								theBmp = Animation[scdata->AnimationUpPush]->NextFrame(true);
								DrawBitmap(theBmp, NULL, Menu[i].X+x+scdata->Up_X, Menu[i].Y+y+scdata->Up_Y);
							}

							up = false;
							scroll_click = i;
							scroll_dir = 0;
						}
						else
						{
							if(scdata->AnimationUpOver < 0 || Animation[scdata->AnimationUpOver] == NULL)
							{
								BitRect.Left	= scdata->Up_Lit_X;
								BitRect.Top		= scdata->Up_Lit_Y;
								BitRect.Right	= scdata->Up_Lit_X+scdata->Up_Width;
								BitRect.Bottom	= scdata->Up_Lit_Y+scdata->Up_Height;

								if(scdata->Up_Width > 0 && scdata->Up_Height > 0)
									DrawBitmap(Images[scdata->Image_Number], &BitRect, Menu[i].X+x+scdata->Up_X, Menu[i].Y+y+scdata->Up_Y);
							}
							else
							{
								theBmp = Animation[scdata->AnimationUpOver]->NextFrame(true);
								DrawBitmap(theBmp, NULL, Menu[i].X+x+scdata->Up_X, Menu[i].Y+y+scdata->Up_Y);
							}

							up = false;
						}
					}

					if(up)
					{
						theBmp = Animation[scdata->AnimationUp]->NextFrame(true);
						DrawBitmap(theBmp, NULL, Menu[i].X+x+scdata->Up_X, Menu[i].Y+y+scdata->Up_Y);
					}

					if((temppos.x >= Menu[i].X+x+scdata->Dwn_X)
						&& (temppos.x <= (Menu[i].X+x+scdata->Dwn_X+scdata->Dwn_Width))
						&& (temppos.y >= (Menu[i].Y+y+scdata->Dwn_Y))
						&& (temppos.y <= (Menu[i].Y+y+scdata->Dwn_Y+scdata->Dwn_Height))
						&& (slide_click == -1))
					{
// changed QD 02/01/07
						if((GetSystemMetrics(SM_SWAPBUTTON) && (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0 && focus == -1) ||
							(!GetSystemMetrics(SM_SWAPBUTTON) && (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0 && focus == -1))
// end change
						{
							if(scdata->AnimationDwnPush<0 || Animation[scdata->AnimationDwnPush]==NULL)
							{
								BitRect.Left	= scdata->Dwn_Push_X;
								BitRect.Top		= scdata->Dwn_Push_Y;
								BitRect.Right	= scdata->Dwn_Push_X+scdata->Dwn_Width;
								BitRect.Bottom	= scdata->Dwn_Push_Y+scdata->Dwn_Height;

								if(scdata->Dwn_Width > 0 && scdata->Dwn_Height > 0)
									DrawBitmap(Images[scdata->Image_Number], &BitRect, Menu[i].X+x+scdata->Dwn_X, Menu[i].Y+y+scdata->Dwn_Y);
							}
							else
							{
								theBmp = Animation[scdata->AnimationDwnPush]->NextFrame(true);
								DrawBitmap(theBmp, NULL, Menu[i].X+x+scdata->Dwn_X, Menu[i].Y+y+scdata->Dwn_Y);
							}

							dwn = false;
							scroll_click = i;
							scroll_dir = 1;
						}
						else
						{
							if(scdata->AnimationDwnOver < 0 || Animation[scdata->AnimationDwnOver] == NULL)
							{
								BitRect.Left	= scdata->Dwn_Lit_X;
								BitRect.Top		= scdata->Dwn_Lit_Y;
								BitRect.Right	= scdata->Dwn_Lit_X+scdata->Dwn_Width;
								BitRect.Bottom	= scdata->Dwn_Lit_Y+scdata->Dwn_Height;

								if(scdata->Dwn_Width > 0 && scdata->Dwn_Height > 0)
									DrawBitmap(Images[scdata->Image_Number], &BitRect, Menu[i].X+x+scdata->Dwn_X, Menu[i].Y+y+scdata->Dwn_Y);
							}
							else
							{
								theBmp = Animation[scdata->AnimationDwnOver]->NextFrame(true);
								DrawBitmap(theBmp, NULL, Menu[i].X+x+scdata->Dwn_X, Menu[i].Y+y+scdata->Dwn_Y);
							}

							dwn = false;
						}
					}

					if(dwn)
					{
						theBmp = Animation[scdata->AnimationDwn]->NextFrame(true);
						DrawBitmap(theBmp, NULL, Menu[i].X+x+scdata->Dwn_X, Menu[i].Y+y+scdata->Dwn_Y);
					}

					break;
				}
			case LSBOX: //else if(Menu[i].Type == LSBOX)
				{
					lrdata = (LSBox*)Menu[i].data;
					Savedef *Kdata = lrdata->text;

					if(lrdata->Max == -1)
					{
						lrdata->Max = 0;

						while(Kdata[lrdata->Max].text != NULL)
						{
							lrdata->Max += 1;
						}
					}

					BitRect.Left	= lrdata->Image_X;
					BitRect.Top		= lrdata->Image_Y;
					BitRect.Right	= lrdata->Image_X+lrdata->Width;
					BitRect.Bottom	= lrdata->Image_Y+lrdata->Height;

					if(lrdata->Width > 0 && lrdata->Height > 0)
						DrawBitmap(Images[lrdata->Image_Number], &BitRect, Menu[i].X+x, Menu[i].Y+y);

					int tempstep = 0;

					for(int j=lrdata->Start; j<lrdata->Max; j++)
					{
						if(j < (lrdata->Start+lrdata->Max_Show))
						{
							char *s;

							if(j == lrdata->Current)
							{
								BitRect.Left	= lrdata->Rev_X;
								BitRect.Top		= lrdata->Rev_Y;
								BitRect.Right	= lrdata->Rev_X+lrdata->Rev_Width;
								BitRect.Bottom	= lrdata->Rev_Y+lrdata->Rev_Height;

								if(lrdata->Width > 0 && lrdata->Height > 0)
									DrawBitmap(Images[lrdata->Image_Number], &BitRect, Menu[i].X+x+lrdata->Corner_X, Menu[i].Y+y+lrdata->Corner_Y+tempstep);
							}

							s = Kdata[j].text;
							MFontRect(s, lrdata->Font, Menu[i].X+x+lrdata->Start_X, Menu[i].Y+y+lrdata->Start_Y+tempstep);
						}

						tempstep += lrdata->Step;
					}

					if((temppos.x >= Menu[i].X+x+lrdata->Corner_X)
						&& (temppos.x <= (Menu[i].X+x+lrdata->Corner_X+lrdata->Click_Width))
						&& (temppos.y >= (Menu[i].Y+y+lrdata->Corner_Y))
						&& (temppos.y <= (Menu[i].Y+y+lrdata->Corner_Y+(lrdata->Max_Show*lrdata->Step)))
						&& (slide_click == -1))
					{
// changed QD 02/01/07
						if(GetSystemMetrics(SM_SWAPBUTTON))
						{
							if((GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0 && focus == -1)
							{
								lsbox_click = i;
								lsbox_line = (temppos.y-(Menu[i].Y+y+lrdata->Corner_Y))/lrdata->Step;
							}
						}
						else
						{
							if((GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0 && focus == -1)
							{
								lsbox_click = i;
								lsbox_line = (temppos.y-(Menu[i].Y+y+lrdata->Corner_Y))/lrdata->Step;
							}
						}
// end change
					}

// begin add Nout - Show save game image
// changed QD 12/15/05
					if(lrdata->Current > -1)
					{
						if(lrdata->text[lrdata->Current].SGImage)
						{
							BitRect.Left	= SaveScreen.Image_X;
							BitRect.Top		= SaveScreen.Image_Y;
							BitRect.Right	= SaveScreen.Image_X+SaveScreen.Width;
							BitRect.Bottom	= SaveScreen.Image_Y+SaveScreen.Height;

							DrawBitmap(lrdata->text[lrdata->Current].SGImage, &BitRect, SaveScreen.X+x, SaveScreen.Y+y);
						}
						else if(EmptySlotImage)
						{
							BitRect.Left	= SaveScreen.Image_X;
							BitRect.Top		= SaveScreen.Image_Y;
							BitRect.Right	= SaveScreen.Image_X+SaveScreen.Width;
							BitRect.Bottom	= SaveScreen.Image_Y+SaveScreen.Height;

							DrawBitmap(EmptySlotImage, &BitRect, SaveScreen.X+x, SaveScreen.Y+y);
						}
					}

/*					BitRect.Left	= SaveScreen.Image_X;
					BitRect.Top		= SaveScreen.Image_Y;
					BitRect.Right	= SaveScreen.Image_X+SaveScreen.Width;
					BitRect.Bottom	= SaveScreen.Image_Y+SaveScreen.Height;

					if(SaveScreen.Width > 0 && SaveScreen.Height > 0)
						DrawBitmap(savescrbmp, &BitRect, SaveScreen.X, SaveScreen.Y);

					if(LastCurrent == lrdata->Current)
						LoadSGImage = GE_FALSE;
					else
					{
						LoadSGImage = GE_TRUE;			//set a flag to load the bitmap .BMP file
						LastCurrent = lrdata->Current;	//ensure the bitmap is loaded only when a new has to be loaded
					}
*/
// end change
// end add Nout
					break;
				}
			}
		}

#ifdef BLIT
		geEngine_DrawBitmap(CCD->Engine()->Engine(), ScreenBmp, NULL, 0, 0);
#endif

		if(LoopOnce == 0 && !Fading)
		{
			if(AnimCursor < 0 || Animation[AnimCursor] == NULL)
			{
				geEngine_DrawBitmap(CCD->Engine()->Engine(), Cursor, NULL, temppos.x-HotX, temppos.y-HotY);
			}
			else
			{
				geBitmap *theBmp = Animation[AnimCursor]->NextFrame(true);
				DrawBitmap(theBmp, NULL, temppos.x-HotX, temppos.y-HotY);
			}
		}

		if(Fading)
		{
			DoFade();
		}

		geEngine_EndFrame(CCD->Engine()->Engine());

		if((GetAsyncKeyState(VK_ESCAPE) & 0x8000) != 0 && escapeon == 0 && ingame == 1)
		{
			escapeon = 1;
			FadeSet(1, TimeFade);
		}

		if((GetAsyncKeyState(VK_ESCAPE) & 0x8000) == 0 && escapeon == 1 && !Fading)
		{
// 08.05.2004 - begin change gekido
			CCD->ReportError("Exiting Menu, Returning to Game", false);
// 08.05.2004 - end change gekido
			escapeon = 0;

			if(ingame == 1)
			{
				if(musictype != -1)
				{
					if(musictype == 1)
						MIDIPlayer()->Stop();
					else
						m_Streams->Stop();
				}

				CCD->AudioStreams()->PauseAll(); // restart streaming audio

				if(CCD->CDPlayer())
				{
					if(CCD->CDPlayer()->GetCdOn())
						CCD->CDPlayer()->Restore(); // restart CD music if CD is on
				}

				if(CCD->MIDIPlayer())
					CCD->MIDIPlayer()->Restore(); // restart midi if it was playing

				ResetVol();
				GameLevel();
			}
		}

		if(!Fading)
		{
			//--------------------------------------
			// if clicked on Clickable or Exit Menu
			//--------------------------------------
// changed QD 02/01/07
			if(click !=-1 && ((GetSystemMetrics(SM_SWAPBUTTON) && (GetAsyncKeyState(VK_RBUTTON) & 0x8000) == 0) ||
								(!GetSystemMetrics(SM_SWAPBUTTON) && (GetAsyncKeyState(VK_LBUTTON) & 0x8000) == 0)))
// end change
			{
				if(LoopOnce == 0)
					geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), mouseclick, 0.99f, 0.0f, 1.0f, false);

				data = (Clickable *)Menu[click].data;

				if(LoopOnce == 0 && !Fading)
				{
					FadeSet(1, TimeFade);
					LoopOnce = 3;
				}
				else if(LoopOnce == 3 && !Fading)
				{
					LoopOnce = 0;

					if(Menu[click].Type == EXIT_MENU)
					{
						FadeSet(-1, TimeFade);

						if(remapf != -1)
							CCD->Input()->SaveKeymap("keyboard.ini");

// changed QD Language Menu
						if(data->proc != NULL)
							data->proc();
// end change QD
						if(data->Action >= 10)
							return data->Action-9;

						return 0;
					}
// changed RF063
					else if(Menu[click].Type == CANCEL_MENU)
					{
						if(remapf != -1)
							CCD->Input()->SaveKeymap("keyboard.ini");
						return 1;
					}
// end change RF063
					else if(Menu[click].Type == CLICKABLE)
					{
						FadeSet(-1, TimeFade);

						if(data->Next != NULL && (data->Action == 0 || data->Action == 2))
						{
							ProcessMenu(data->Next, data->background, data->title);
						}
						else if(data->proc != NULL && data->Action == 1)
						{
							data->proc();
						}
						else if(data->proc != NULL && data->Action == 3)
						{
							data->proc();
							remapf = 0;
						}
						else if(data->proc != NULL && data->Action == 4)
						{
// changed RF064
							if(LoadBox.Current != -1)
							{
								if(LoadBox.text[LoadBox.Current].empty != 0)
								{
// end change RF064
									data->proc();
									return 0;
								}
							}
						}
					}

					click = -1;
				}
			}

			//--------------------------
			// if clicked on slider
			//--------------------------

// changed QD 02/01/07
			if(slide_click != -1 && ((GetSystemMetrics(SM_SWAPBUTTON) && (GetAsyncKeyState(VK_RBUTTON) & 0x8000) == 0) ||
								(!GetSystemMetrics(SM_SWAPBUTTON) && (GetAsyncKeyState(VK_LBUTTON) & 0x8000) == 0)))
// end change
			{
				temp = temppos.x-slide_x;
				sdata = (Slider*)Menu[slide_click].data;
				sdata->Current += temp;

				if(sdata->Current > sdata->Max_X)
					sdata->Current = sdata->Max_X;

				if(sdata->Current < sdata->Min_X)
					sdata->Current = sdata->Min_X;

				slide_x = temppos.x;
				temp = (sdata->Current*100)/sdata->Max_X;

				if(sdata->proc != NULL && sdata->On_the_Fly == 0)
					sdata->proc(temp);

				slide_click=-1;
			}

			if(slide_click == -1)
				slide_x = -1;

// changed QD 02/01/07
			if(slide_click != -1 && ((GetSystemMetrics(SM_SWAPBUTTON) && (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0) ||
								(!GetSystemMetrics(SM_SWAPBUTTON) && (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0)))
// end change
			{
				if(slide_x != temppos.x)
				{
					temp = temppos.x-slide_x;
					sdata = (Slider*)Menu[slide_click].data;
					sdata->Current += temp;

					if(sdata->Current > sdata->Max_X)
						sdata->Current = sdata->Max_X;

					if(sdata->Current < sdata->Min_X)
						sdata->Current = sdata->Min_X;

					slide_x = temppos.x;
					temp = (sdata->Current*100)/sdata->Max_X;
					geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), slideclick, 0.75f, 0.0f, 1.0f, false);

					if(sdata->proc != NULL && sdata->On_the_Fly == 1)
						sdata->proc(temp);
				}
			}

			//--------------------------
			// if clicked on box
			//--------------------------

// changed QD 02/01/07
			if(box_click != -1 && ((GetSystemMetrics(SM_SWAPBUTTON) && (GetAsyncKeyState(VK_RBUTTON) & 0x8000) == 0) ||
								(!GetSystemMetrics(SM_SWAPBUTTON) && (GetAsyncKeyState(VK_LBUTTON) & 0x8000) == 0)))
// end change
			{
				geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), mouseclick, 0.99f, 0.0f, 1.0f, false);
				bdata = (Box*)Menu[box_click].data;
				bdata->Current = (bdata->Current+1) & 1;

				if(bdata->proc != NULL)
					bdata->proc(bdata->Current);

				box_click = -1;
			}

// changed QD Language Menu
			//--------------------------
			// if clicked on radio box
			//--------------------------

// changed QD 02/01/07
			if(radio_click != -1 && ((GetSystemMetrics(SM_SWAPBUTTON) && (GetAsyncKeyState(VK_RBUTTON) & 0x8000) == 0) ||
								(!GetSystemMetrics(SM_SWAPBUTTON) && (GetAsyncKeyState(VK_LBUTTON) & 0x8000) == 0)))
// end change
			{
				geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), mouseclick, 0.99f, 0.0f, 1.0f, false);
				radiodata = (Radio*)Menu[radio_click].data;

				for(i=0; i<max; i++)
				{
					if(Menu[i].Type == RADIO)
					{
						Radio *radiodata1 = (Radio*)Menu[i].data;
						radiodata1->Current = 0;
					}
				}

				radiodata->Current = 1;

				if(radiodata->proc != NULL)
					radiodata->proc(radiodata->ID);

				radio_click = -1;
			}
// end change QD

			//--------------------------
			// if clicked on remap
			//--------------------------

// changed QD 02/01/07
			if(remap_click != -1 && ((GetSystemMetrics(SM_SWAPBUTTON) && (GetAsyncKeyState(VK_RBUTTON) & 0x8000) == 0) ||
								(!GetSystemMetrics(SM_SWAPBUTTON) && (GetAsyncKeyState(VK_LBUTTON) & 0x8000) == 0)))
// end change
			{
				geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), mouseclick, 0.99f, 0.0f, 1.0f, false);
				rdata = (Remap*)Menu[remap_click].data;
				rdata->Current = remap_line + rdata->Start;
				remap_click = -1;
			}

			//--------------------------
			// if clicked on scrollbar
			//--------------------------

// changed QD 02/01/07
			if(scroll_click != -1 && ((GetSystemMetrics(SM_SWAPBUTTON) && (GetAsyncKeyState(VK_RBUTTON) & 0x8000) == 0) ||
								(!GetSystemMetrics(SM_SWAPBUTTON) && (GetAsyncKeyState(VK_LBUTTON) & 0x8000) == 0)))
// end change
			{
				geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), mouseclick, 0.99f, 0.0f, 1.0f, false);
				scdata = (ScrollBar*)Menu[scroll_click].data;
				int max = *(scdata->Max);
				int current = *(scdata->Current);

				if(scroll_dir == 0)
				{
					if(current > 0)
						*(scdata->Current) -= 1;
				}
				else
				{
					if(current < (max-scdata->Show))
						*(scdata->Current) += 1;
				}

				scroll_click = -1;
			}

			//--------------------------
			// if clicked on lsbox
			//--------------------------

// changed QD 02/01/07
			if(lsbox_click != -1 && ((GetSystemMetrics(SM_SWAPBUTTON) && (GetAsyncKeyState(VK_RBUTTON) & 0x8000) == 0) ||
								(!GetSystemMetrics(SM_SWAPBUTTON) && (GetAsyncKeyState(VK_LBUTTON) & 0x8000) == 0)))
// end change
			{
				geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), mouseclick, 0.99f, 0.0f, 1.0f, false);
				lrdata = (LSBox*)Menu[lsbox_click].data;
				lrdata->Current = lsbox_line + lrdata->Start;
				lsbox_click = -1;
			}
		}
	}

	return 1;
}

/* ------------------------------------------------------------------------------------ */
//	DrawBitmap
/* ------------------------------------------------------------------------------------ */
void CRFMenu::DrawBitmap(const geBitmap *Bitmap, geRect *Source, uint32 x, uint32 y)
{
#ifdef BLIT
	geBitmap_Info	BmpInfo;
	int SrcPositionX;
	int SrcPositionY;
	int SizeX;
	int SizeY;

	if(!Source)
	{
		geBitmap_GetInfo(Bitmap, &BmpInfo, NULL);
		SizeX = BmpInfo.Width;
		SizeY = BmpInfo.Height;
		SrcPositionX = 0;
		SrcPositionY = 0;

		if(x == 0 && y == 0)
			geBitmap_BlitBitmap(Bitmap, ScreenBmp);
		else
			geBitmap_Blit(Bitmap, SrcPositionX, SrcPositionY, ScreenBmp, x, y, SizeX, SizeY);
	}
	else
	{
		SrcPositionX = Source->Left;
		SrcPositionY = Source->Top;
		SizeX = Source->Right - SrcPositionX;
		SizeY = Source->Bottom - SrcPositionY;
		geBitmap_Blit(Bitmap, SrcPositionX, SrcPositionY, ScreenBmp, x, y, SizeX, SizeY);
	}
#else
	geEngine_DrawBitmap(CCD->Engine()->Engine(), Bitmap, Source, x, y);
#endif
}

/* ------------------------------------------------------------------------------------ */
//	MFontRect
/* ------------------------------------------------------------------------------------ */
void CRFMenu::MFontRect(char *s, int FontNumber, int x, int y)
{
	//int charoff;
	char chr;

	if(MenuFont[FontNumber].Bitmap == NULL)
	{
		char szBug[256];
		sprintf(szBug, "*WARNING* File %s - Line %d: No defined Font # %d (FONT%d in Menu.ini)",
				__FILE__, __LINE__, FontNumber, FontNumber+1);
		CCD->ReportError(szBug, false);
		return;
	}

	if(s)
	{
		geEngine *theEngine = CCD->Engine()->Engine();
		//charoff = 0;
		while(*s != 0)
		{
			chr = *s-32;

			fRect.Top		= MenuFont[FontNumber].dat[chr].y;
			fRect.Bottom	= MenuFont[FontNumber].dat[chr].y+MenuFont[FontNumber].font_height;
			fRect.Left		= MenuFont[FontNumber].dat[chr].x;
			fRect.Right		= MenuFont[FontNumber].dat[chr].x+MenuFont[FontNumber].dat[chr].width;

			//DrawBitmap(MenuFont[FontNumber].Bitmap, &fRect, x+charoff, y);
			geEngine_DrawBitmap(theEngine, MenuFont[FontNumber].Bitmap, &fRect, x, y);

			x += MenuFont[FontNumber].dat[chr].width;
			s++;
		}
	}
}

/* ------------------------------------------------------------------------------------ */
//	setup bitmap rectangle for
//	the letter in the given font
/* ------------------------------------------------------------------------------------ */
void CRFMenu::FontRect(char *s, int FontNumber, int x, int y)
{
	//int charoff;
	char chr;

	if(MenuFont[FontNumber].Bitmap == NULL)
	{
		char szBug[256];
		sprintf(szBug, "*WARNING* File %s - Line %d: No defined Font # %d (FONT%d in Menu.ini)",
				__FILE__, __LINE__, FontNumber, FontNumber+1);
		CCD->ReportError(szBug, false);
		// changed QD 07/15/06
		return;
		/*
// changed RF064
		CCD->ShutdownLevel();
		delete CCD;
		CCD = NULL;
		MessageBox(NULL, szBug,"Fatal Error", MB_OK);
		exit(-336);
// end change RF064
		*/
		// end change
	}

	if(s)
	{
		geEngine *theEngine = CCD->Engine()->Engine();
		//charoff = 0;
		while(*s != 0)
		{
			chr = *s-32;

			fRect.Top		= MenuFont[FontNumber].dat[chr].y;
			fRect.Bottom	= MenuFont[FontNumber].dat[chr].y+MenuFont[FontNumber].font_height-1;
			fRect.Left		= MenuFont[FontNumber].dat[chr].x;
			fRect.Right		= MenuFont[FontNumber].dat[chr].x+MenuFont[FontNumber].dat[chr].width-1;

			//geEngine_DrawBitmap(CCD->Engine()->Engine(), MenuFont[FontNumber].Bitmap, &fRect, x+charoff, y);
			geEngine_DrawBitmap(CCD->Engine()->Engine(), MenuFont[FontNumber].Bitmap, &fRect, x, y);

			x += MenuFont[FontNumber].dat[chr].width;
			s++;
		}
	}
}

/* ------------------------------------------------------------------------------------ */
//	FontHeight
/* ------------------------------------------------------------------------------------ */
int CRFMenu::FontHeight(int FontNumber)
{
	if(MenuFont[FontNumber].Bitmap == NULL)
	{
		char szBug[256];
		sprintf(szBug, "*WARNING* File %s - Line %d: No defined Font # %d (FONT%d in Menu.ini)",
				__FILE__, __LINE__, FontNumber, FontNumber+1);
		CCD->ReportError(szBug, false);
		return 0;
	}

	return MenuFont[FontNumber].font_height;
}

/* ------------------------------------------------------------------------------------ */
//	FontWidth
/* ------------------------------------------------------------------------------------ */
int CRFMenu::FontWidth(int FontNumber, char *s)
{
	int charoff;
	char chr;

	if(MenuFont[FontNumber].Bitmap == NULL)
	{
		char szBug[256];
		sprintf(szBug, "*WARNING* File %s - Line %d: No defined Font # %d (FONT%d in Menu.ini)",
				__FILE__, __LINE__, FontNumber, FontNumber+1);
		CCD->ReportError(szBug, false);
		return 0;
	}

	if(s != NULL)
	{
		charoff = 0;

		while(*s!=0)
		{
			chr = *s-32;
			charoff += MenuFont[FontNumber].dat[chr].width;
			s++;
		}

		return charoff;
	}
	else
		return 0;
}

/* ------------------------------------------------------------------------------------ */
//	StopMenuMusic
/* ------------------------------------------------------------------------------------ */
void CRFMenu::StopMenuMusic()
{
	if(musictype != -1)
	{
		if(musictype == 1)
			MIDIPlayer()->Stop();
		else
			m_Streams->Stop();
	}
}

// changed RF064
/* ------------------------------------------------------------------------------------ */
//	SetMusicVol
/* ------------------------------------------------------------------------------------ */
void CRFMenu::SetMusicVol(float vol)
{
	CMixer mixer(CCD->Engine()->WindowHandle(), MIXERLINE_COMPONENTTYPE_DST_SPEAKERS,
		NO_SOURCE, MIXERCONTROL_CONTROLTYPE_VOLUME);

	if(!mixer.IsOk())
		return;

	DWORD dw = (DWORD)(vol*65535.0f);
	mixer.SetControlValue(dw);
}

/* ------------------------------------------------------------------------------------ */
//	SetmMusicVol
/* ------------------------------------------------------------------------------------ */
void CRFMenu::SetmMusicVol(float vol)
{
	mVolLevel = vol;
	LONG nVolume = (LONG)((vol*10000.0f)-10000);

	if(musictype != -1)
	{
		if(musictype != 1)
			m_Streams->SetVolume(nVolume);
	}

	if(CCD->AudioStreams())
		CCD->AudioStreams()->SetVolume(nVolume);

	if(CCD->SoundtrackToggles())
		CCD->SoundtrackToggles()->SetVolume(nVolume);

	CMixer mixer(CCD->Engine()->WindowHandle(), MIXERLINE_COMPONENTTYPE_DST_SPEAKERS,
		MIXERLINE_COMPONENTTYPE_SRC_SYNTHESIZER, MIXERCONTROL_CONTROLTYPE_VOLUME);

	if(mixer.IsOk())
	{
		DWORD dw = (DWORD)(vol*65535.0f);
		mixer.SetControlValue(dw);
	}

	CMixer mixercd(CCD->Engine()->WindowHandle(), MIXERLINE_COMPONENTTYPE_DST_SPEAKERS,
		MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC, MIXERCONTROL_CONTROLTYPE_VOLUME);

	if(mixercd.IsOk())
	{
		DWORD dw = (DWORD)(vol*65535.0f);
		mixercd.SetControlValue(dw);
	}
}
// end change RF064

/* ------------------------------------------------------------------------------------ */
//	GameLoop
//
// setup and run level
/* ------------------------------------------------------------------------------------ */
void CRFMenu::GameLoop()
{
	//	Now that we have the initial inventory and attributes for the
	//	..first level, we're going to save them off to a temp. file so
	//	..if the player gets killed, when we restart we can do a fast
	//	..and easy reload of the "game startup" attributes and
	//	..inventory.

/* changed RF064
	CCD->Player()->DisableFog();		// Turn off fogging for cut scene
	CCD->Player()->DisableClipPlane();	// Turn off the clipping plane as well

	// Play the opening cut scene, if one exists
	CCD->PlayOpeningCutScene();
end change RF064 */

// 08.05.2004 - begin change gekido
	CCD->ReportError("CRFMenu::GameLoop() - Setup and Run Level", false);
// 08.05.2004 - end change gekido

	CCD->Player()->ShowFog();			// Show fog, if enabled
	CCD->Player()->ActivateClipPlane();	// Activate clipping plane, if enabled

	//	On the load of the first level, we want to set up the player
	//	..attributes and HUD display format.
	CCD->HUD()->LoadConfiguration();
	CCD->Player()->SaveAttributes("pdoa.bin");
	CCD->Player()->SaveAttributesAscii("attributes.txt");
	CCD->SetLevelData();

	CCD->MenuManager()->SetInGame();
	GameLevel();
}

// changed RF063
/* ------------------------------------------------------------------------------------ */
//	GameLevel
//
//	Main game loop
/* ------------------------------------------------------------------------------------ */
void CRFMenu::GameLevel()
{
// 08.05.2004 - begin change gekido
	CCD->ReportError("CRFMenu::GameLevel() - Entering Inner Game Loop", false);
// 08.05.2004 - end change gekido
	MSG msg;
	int FirstFont;
	geRect firstRect;
	int framecount;

// changed Nout 12/15/05
	for(int i=0; i<MAXTEXT; i++)
		CCD->Pawns()->TextMessage[i].ShowText = false;
// end change

	CCD->Player()->ShowFog();

	//	Main game loop.  Until it's time for us to quit, we'll check for
	//	..motion, move the player, etc.
	if(box6.Current == BOX_ON)
		CCD->Engine()->ShowFrameRate(true);

	CCD->Engine()->ResetSystem();

	for(FirstFont=0; FirstFont<NUM_FONTS; FirstFont++)
	{
		if(MenuFont[FirstFont].Bitmap != NULL)
		{
			firstRect.Top		= MenuFont[FirstFont].dat[0].y;
			firstRect.Bottom	= MenuFont[FirstFont].dat[0].y+MenuFont[0].font_height;
			firstRect.Left		= MenuFont[FirstFont].dat[0].x;
			firstRect.Right		= MenuFont[FirstFont].dat[0].x+MenuFont[FirstFont].dat[0].width;

			break;
		}
	}

	FadeSet(-1, TimeFade);
	MusicSet();

	framecount = 0;

	// changed QD 02/01/07
// center mouse on window befor returning to game
	POINT pos;
	if(CCD->Engine()->FullScreen())
	{
		pos.x = CCD->Engine()->Width()/2;			// calculate the center of the screen
		pos.y = CCD->Engine()->Height()/2;			// calculate the center of the screen
		SetCursorPos(pos.x, pos.y);					// set the cursor in the center of the screen
	}
	else
	{
		RECT client;
		GetClientRect(CCD->Engine()->WindowHandle(),&client);	// get the client area of the window
		pos.x = client.right/2;									// calculate the center of the client area
		pos.y = client.bottom/2;								// calculate the center of the client area
		ClientToScreen(CCD->Engine()->WindowHandle(),&pos);		// convert to SCREEN coordinates
		SetCursorPos(pos.x,pos.y);								// put the cursor in the middle of the window
	}
// end change

	for(;;)
	{
		// If Winblows has something to say, take it in and pass it on in the
		// ..off-chance someone cares.
// start multiplayer
		while(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			GetMessage(&msg, NULL, 0, 0);
			TranslateMessage(&msg);
// end multiplayer
			DispatchMessage(&msg);
		}

		if(!CCD->GetHasFocus())
			continue;

		if(CCD->HandleGameInput() == true)
		{
			// Ok, send a Tick() call to all components that use time
// changed RF064
			if(CCD->Inventory()->GetStopTime())
				CCD->DispatchTick();
// end change RF064

			bool cflag = false;

			if(box1.Current == BOX_ON
				&& CCD->Player()->GetViewPoint() == FIRSTPERSON
				&& CCD->Weapons()->CrossHair())	// display crosshair
			{
				if(!CCD->Weapons()->CrossHairFixed() && CCD->Weapons()->GetCrossHair())
				{
					DisplayCrossHair();
					cflag = true;
				}
			}

			CCD->Pawns()->AnimateWeapon();
			//CCD->Weapons()->Display();
			CCD->Weapons()->DoAttack();
			CCD->Explosions()->Tick(CCD->GetTicksGoneBy());
			CCD->EffectManager()->Tick(CCD->GetTicksGoneBy());

			// Entities animated, render _EVERYTHING_
			CCD->Engine()->BeginFrame();				// Start up rendering for this frame

			geEngine_DrawBitmap(CCD->Engine()->Engine(), MenuFont[FirstFont].Bitmap, &firstRect, 0, 0);

			CCD->RenderComponents();						// Render the RGF components

			CCD->Engine()->RenderWorld();				// Render the world

// changed Nout 12/15/05
			//Fills a rectangle on the screen with color / alpha (can be used for color/alpha fading)
			for(int i=0; i<MAXFILLAREA; i++)
				CCD->Pawns()->FillScreenArea(i);
// end change

			if(box1.Current == BOX_ON
				&& (CCD->Player()->GetViewPoint() == FIRSTPERSON || CCD->Player()->GetViewPoint() == THIRDPERSON)
				&& !CCD->Player()->GetMonitorMode())
			{
				if(CCD->Weapons()->CrossHair() && CCD->Weapons()->CrossHairFixed() && CCD->Weapons()->GetCrossHair())
				{
					CCD->Weapons()->DisplayCrossHair();
				}
				else
				{
					if(!cflag)
					{
						geBitmap *theBmp;
						geBitmap_Info	BmpInfo;
						int x, y;

						theBmp = CCD->MenuManager()->GetFCrossHair();

						if(geBitmap_GetInfo(theBmp, &BmpInfo, NULL) == GE_TRUE)
						{
// changed RF063
							if(CCD->Player()->GetViewPoint() == THIRDPERSON
								&& !(CCD->Weapons()->GetCurrent() == -1 || CCD->Weapons()->GetCurrent() == 11))
							{
								geVec3d ProjectedPoint = CCD->Weapons()->GetProjectedPoint();
								x = (CCD->Engine()->Width() - BmpInfo.Width) / 2;
								y = (int)ProjectedPoint.Y - (BmpInfo.Height) / 2;
							}
							else
							{
								x = (CCD->Engine()->Width() - BmpInfo.Width) / 2;
								y = (CCD->Engine()->Height() - BmpInfo.Height) / 2;
							}
// end change RF063
							geEngine_DrawBitmap(CCD->Engine()->Engine(), theBmp, NULL, x, y);
						}
					}
				}
			}

			if(box5.Current == BOX_ON)
			{
				CCD->Weapons()->WeaponData();

				char szBug[256];
				geVec3d Pos = CCD->Player()->Position();
				sprintf(szBug, "Player X : %4.2f Player Y : %4.2f Player Z : %4.2f Actors %d", Pos.X, Pos.Y, Pos.Z, CCD->ActorManager()->CountActors());
				CCD->MenuManager()->WorldFontRect(szBug, FONT10, 5, CCD->Engine()->Height()- 20, 255.0f);
			}

			CCD->Messages()->Display();

			CCD->PWXImMgr()->Display();//PWX

			if(CCD->GetSaving())
			{
				savetime = SavingTime;
				CCD->SetSaving(false);
			}

			if(savetime > 0.0f)
			{
				savetime -= (CCD->LastElapsedTime_F()*0.001f);
				int width	= FontWidth(SaveFont, Savemsg);
				int height	= FontHeight(SaveFont);
				int xoffset = (CCD->Engine()->Width()-width)/2;
				int yoffset = (CCD->Engine()->Height()-height)/2;
				FontRect(Savemsg, SaveFont, xoffset, yoffset);
			}
			else
				savetime = 0.0f;

// changed RF064
			if(CCD->GetConsole())
			{
				// call the console render function here as it is active
				// begin change gekido
				CCD->ConsoleRender();
				// end change gekido
			}

			CCD->Inventory()->Display(); // render Inventory / HUD
// end change RF064

// changed Nout 12/15/05
			// Displays a text on the screen that tracks position with an Entity
			for(int ii=0; ii<MAXTEXT; ii++)
				CCD->Pawns()->ShowText(ii);
// end change

// changed QD 12/15/05
			CCD->Teleporters()->DoFade();
// end change

			if(Fading)
				DoFade();

			if(framecount < 2)
			{
				GE_Rect Rect;
				GE_RGBA	Color;

				Rect.Left = Rect.Top = 0;
				Rect.Right = CCD->Engine()->Width() - 1;
				Rect.Bottom = CCD->Engine()->Height() - 1;

				Color.r = 0.0f;
				Color.g = 0.0f;
				Color.b = 0.0f;
				Color.a = 255;

				geEngine_FillRect(CCD->Engine()->Engine(), &Rect, &Color);
				framecount += 1;
			}

			// Everything rendered, now end the frame.
			CCD->Engine()->EndFrame();					// All done, do the necessary flip

			if(CCD->GetPaused())
			{
				if(CCD->GetUseEffect())
				{
					geFog *theFog;
					int nTemp;
					CCD->CameraManager()->TrackMotion();
					geVec3d Pos;
					CCD->CameraManager()->GetPosition(&Pos);
					GE_RGBA cColor = CCD->GetFogColor();
					cColor.a = 255.0f;
					theFog = geWorld_AddFog(CCD->World());
					geFog_SetAttributes(theFog, &Pos, &cColor, 0.0f, 1500.0f, 240.0f);

					for(nTemp=1000; nTemp<6000; nTemp+=200)
					{
						CCD->Engine()->BeginFrame();
						geFog_SetAttributes(theFog, &Pos, &cColor, 0.0f,
											(geFloat)nTemp+500.0f, 240.0f+(geFloat)(nTemp/400));
						CCD->Engine()->RenderWorld();
						CCD->Engine()->EndFrame();
					}

					for(nTemp=6000; nTemp>0; nTemp-= 200)
					{
						CCD->Engine()->BeginFrame();
						geFog_SetAttributes(theFog, &Pos, &cColor, 0.0f,
							(geFloat)nTemp+500.0f, 240.0f+(geFloat)(nTemp/400));
						CCD->Engine()->RenderWorld();
						CCD->Engine()->EndFrame();
					}

					geWorld_RemoveFog(CCD->World(), theFog);
					savetime = 0.0f;
					CCD->Engine()->ResetSystem();
					framecount = 0;
				}

				CCD->SetPaused(false);
			}

			CCD->CheckMediaPlayers();						// Poll media players

			if(!CCD->Player()->GetAlive())
			{
				geEntity_EntitySet *pSet;
				geEntity *pEntity;
				pSet = geWorld_GetEntitySet(CCD->World(), "PlayerSetup");
				pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL);
				PlayerSetup *pSetup = (PlayerSetup*)geEntity_GetUserData(pEntity);

				if(pSetup->UseDeathFog)
				{
					geFog *theFog;
					int nTemp;
					CCD->CameraManager()->TrackMotion();
					geVec3d Pos;
					CCD->CameraManager()->GetPosition(&Pos);
					GE_RGBA cColor = pSetup->DeathFogColor;
					cColor.a = 255.0f;
					theFog = geWorld_AddFog(CCD->World());
					geFog_SetAttributes(theFog, &Pos, &cColor, 0.0f, 1500.0f, 50.0f);

					for(nTemp=1000; nTemp<6000; nTemp+=200)
					{
						CCD->Engine()->BeginFrame();
						geFog_SetAttributes(theFog, &Pos, &cColor, 0.0f,
							(geFloat)nTemp+500.0f, 50.0f+(geFloat)(nTemp/200));
						CCD->Engine()->RenderWorld();
						CCD->Engine()->EndFrame();
					}

					for(nTemp=6000; nTemp>=0; nTemp-=200)
					{
						CCD->Engine()->BeginFrame();
						geFog_SetAttributes(theFog, &Pos, &cColor, 0.0f,
							(geFloat)nTemp+500.0f, 50.0f+(geFloat)(nTemp/200));
						CCD->Engine()->RenderWorld();
						CCD->Engine()->EndFrame();
					}

					geWorld_RemoveFog(CCD->World(), theFog);
				}

				CCD->Player()->DisableFog();		// Fogging OFF
				CCD->Player()->DisableClipPlane();	// Clip plane OFF

				if(!pSetup->KeepAttrAtDeath)
					CCD->Player()->LoadAttributes("pdoa.bin");
				else
				{
					CCD->Player()->SaveAttributes("temp.bin");
					CCD->Player()->LoadAttributes("pdoa.bin");
					CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
					int health = theInv->Value("health");
					CCD->Player()->LoadAttributes("temp.bin");
					theInv->Set("health", health);
				}

				CCD->Weapons()->ClearWeapon();
				CCD->ProcessLevelChange();
				CCD->Player()->SetAlive(true);
				CCD->Player()->ShowFog();					// Fog on, if any there
				CCD->Player()->ActivateClipPlane();	// Clip plane on, if any there

				savetime = 0.0f;
				CCD->Engine()->ResetSystem();
				framecount = 0;
			}
			else
			{
				// The player has moved, frame updated and rendered.  What we do now is
				// ..check to see if, during the course of all this, the player hit a
				// ..changelevel entity.  If so, we need to flush the current level
				// ..and load the new one.
				// Version 053
				if(CCD->ChangeLevel())
				{
					// 08.05.2004 - begin change gekido
					CCD->ReportError("CRFMenu::GameLevel() - ChangeLevel Triggered, begin Changelevel Process...", false);
					// 08.05.2004 - end change gekido
					CCD->Player()->DisableFog();		// Fogging OFF
					CCD->Player()->DisableClipPlane();	// Clip plane OFF

					if(EffectC_IsStringNull(CCD->NextLevel()))
					{
						CCD->AudioStreams()->StopAll(); // stop old streaming audio that might be paused
						CCD->ShutdownLevel();
						CCD->MenuManager()->DeleteSound();

						if(CCD->CDPlayer())
							CCD->CDPlayer()->Stop();

						if(CCD->MIDIPlayer())
							CCD->MIDIPlayer()->Stop();

						if(musictype != -1)
						{
							if(musictype == 1)
								MIDIPlayer()->Play(music, true);
							else
								m_Streams->Play(true);
						}

						ingame = 0;
						CCD->SetChangeLevel(false);
						return;
					}

					CCD->Weapons()->ClearWeapon();
					CCD->ProcessLevelChange();			// Do the level change
					CCD->Player()->ShowFog();					// Fog on, if any there
					CCD->Player()->ActivateClipPlane();	// Clip plane on, if any there
					CCD->Player()->SaveAttributes("pdoa.bin");
					CCD->Player()->SaveAttributesAscii("attributes.txt");
					CCD->Engine()->ResetSystem();
					framecount = 0;
				}
			}
		}
		else
		{
			break;
		}
	}	// End main game loop

// changed QD 12/15/05
// change begin Nout - Show save game image
// save the screen just before switching to the menu (in RF dir)
	// geEngine_ScreenShot(CCD->Engine()->Engine(), "Media\\Bitmaps\\SaveScreen.bmp");
	// ScaleBitmapFromFile(".\\Media\\Bitmaps\\SaveScreen.bmp", SaveScreen.Width, SaveScreen.Height);
	if(SaveScreen.Width > 0 && SaveScreen.Height > 0)
	{
		char szTemp[256];
		strcpy(szTemp, CCD->GetDirectory(kBitmapFile));
		strcat(szTemp, "\\");
		strcat(szTemp, "SaveScreen.bmp");
		geEngine_ScreenShot(CCD->Engine()->Engine(), szTemp);
		ScaleBitmapFromFile(szTemp, SaveScreen.Width, SaveScreen.Height);
	}
// change end Nout
// end change

	while((GetAsyncKeyState(VK_ESCAPE) & 0x8000) != 0)
	{
	}

	if(CCD->AudioStreams())
		CCD->AudioStreams()->PauseAll();// pause streaming audio

	if(CCD->CDPlayer())
		CCD->CDPlayer()->Stop();		// stop CD music

	if(CCD->MIDIPlayer())
		CCD->MIDIPlayer()->Stop();		// stop midi music

/*
	geEngine_BeginFrame(CCD->Engine()->Engine(), CCD->CameraManager()->Camera(), GE_FALSE);
	geEngine_EndFrame(CCD->Engine()->Engine());
	CCD->ResetClock();
	FadeSet(1, TimeFade);
	MusicSet();

	while(Fading)
	{
		geEngine_BeginFrame(CCD->Engine()->Engine(), CCD->CameraManager()->Camera(), GE_FALSE);
		DoFade();
		geEngine_EndFrame(CCD->Engine()->Engine());
	}
*/

	CCD->Engine()->ShowFrameRate(false);
	ClearVol();
	CCD->Player()->DisableFog();

	if(musictype != -1)
	{
		if(musictype == 1)
			MIDIPlayer()->Play(music, true);
		else
			m_Streams->Play(true);
	}

	FadeSet(-1, TimeFade);
	MusicSet();
}
// end change RF063

/* ------------------------------------------------------------------------------------ */
//	DisplayCrossHair
/* ------------------------------------------------------------------------------------ */
void CRFMenu::DisplayCrossHair()
{

	geVec3d theRotation;
	geVec3d thePosition;
	geXForm3d Xf;
	geVec3d Back;
	geVec3d Direction;
	geExtBox theBox;
	GE_Collision Collision;
	GE_LVertex Vert;
	geActor *theActor = CCD->Player()->GetActor();

	Vert.r = Vert.g = Vert.b = Vert.a = 255.0f;

	theBox.Min.X = theBox.Min.Y = theBox.Min.Z = -0.1f;
	theBox.Max.X = theBox.Max.Y = theBox.Max.Z = 0.1f;

	geFloat CurrentDistance = 400.0f;

	CCD->CameraManager()->GetRotation(&theRotation);
	CCD->CameraManager()->GetPosition(&thePosition);

// changed QD 12/15/05
	//geXForm3d_SetIdentity(&Xf);
	//geXForm3d_RotateZ(&Xf, theRotation.Z);
	geXForm3d_SetZRotation(&Xf, theRotation.Z);
	geXForm3d_RotateX(&Xf, theRotation.X);
	geXForm3d_RotateY(&Xf, theRotation.Y);
	// translation doesn't influence the orientation
	//geXForm3d_Translate(&Xf, thePosition.X, thePosition.Y, thePosition.Z);
// end change

	geXForm3d_GetIn(&Xf, &Direction);
	geVec3d_AddScaled(&thePosition, &Direction, CurrentDistance, &Back);

	CCD->Collision()->IgnoreContents(false);
	CCD->Collision()->CheckLevel(RGF_COLLISIONLEVEL_1);

	char BoneHit[64];
	BoneHit[0] = '\0';

	if(CCD->Collision()->CheckForBoneCollision(&theBox.Min, &theBox.Max,
		thePosition, Back, &Collision, theActor, BoneHit, true))
		//if(CCD->Collision()->CheckForWCollision(&theBox.Min, &theBox.Max,
		//Pos, Back, &Collision, theActor))
	{
// changed QD 12/15/05
		// can't be negative (sqrt!)
		// CurrentDistance = (geFloat)fabs(geVec3d_DistanceBetween(&Collision.Impact, &Pos));
		// if(CurrentDistance < 0.0f)
		//	CurrentDistance = 0.0f;
		CurrentDistance = geVec3d_DistanceBetween(&(Collision.Impact), &thePosition);

		if(CurrentDistance > 400.0f)
		{
			CurrentDistance = 400.0f;
		}
		else
		{
			// change cursor lighting
			if(Collision.Actor && CCD->Weapons()->GetAllowLit())
			{
				geVec3d Fill = {0.0f, 1.0f, 0.0f};
				// changed QD 12/15/05
				//	geVec3d_Normalize(&Fill);
				geXForm3d	Xf;
				geXForm3d	XfT;
				geVec3d NewFillNormal;
				geActor_GetBoneTransform(Collision.Actor, RootBoneName(Collision.Actor), &Xf);
				geXForm3d_GetTranspose(&Xf, &XfT);
				geXForm3d_Rotate(&XfT, &Fill, &NewFillNormal);
				geActor_SetLightingOptions(Collision.Actor, GE_TRUE, &NewFillNormal, 255.0f, 255.0f, 255.0f,
											255.0f, 255.0f, 255.0f, GE_TRUE, 6, NULL, GE_FALSE);
				geVec3d LitColor = CCD->Weapons()->GetLitColor();

				Vert.r = LitColor.X;
				Vert.g = LitColor.Y;
				Vert.b = LitColor.Z;
				Vert.a = 255.0f;
			}
		}

		geVec3d_AddScaled(&thePosition, &Direction, CurrentDistance, &Back);
	}

	Vert.u = Vert.v = 0.0f;
	Vert.X = Back.X;
	Vert.Y = Back.Y;
	Vert.Z = Back.Z;

// Start Aug2003DCS - Added "| GE_RENDER_DEPTH_SORT_BF" so that crosshair always draws on top of wall decals
	// geWorld_AddPolyOnce(CCD->World(), &Vert, 1, CCD->Weapons()->GetCrossHair(),
	//	GE_TEXTURED_POINT, GE_RENDER_DO_NOT_OCCLUDE_SELF, 1.0f);
    geWorld_AddPolyOnce(CCD->World(), &Vert, 1, CCD->Weapons()->GetCrossHair(),
		GE_TEXTURED_POINT, GE_RENDER_DO_NOT_OCCLUDE_SELF | GE_RENDER_DEPTH_SORT_BF, 1.0f);
// End Aug2003DCS
}

// changed RF064
/* ------------------------------------------------------------------------------------ */
//	DisplaySplash
//
// display splash screen with color 255
// as transparent
/* ------------------------------------------------------------------------------------ */
void CRFMenu::DisplaySplash()
{
	geBitmap *theBmp;
	geBitmap_Info	BmpInfo;
	int x, y;

	theBmp = CreateFromFileName(Loading);

	if(theBmp != NULL)
	{
		if(geBitmap_GetInfo(theBmp, &BmpInfo, NULL) == GE_TRUE)
		{
			x = (CCD->Engine()->Width() - BmpInfo.Width)/2;
			y = (CCD->Engine()->Height() - BmpInfo.Height)/2;
			// change begin Nout - Show save game image
			geBitmap_SetColorKey(theBmp, GE_TRUE, 255, GE_FALSE);
	        // change end Nout

			if(geEngine_AddBitmap(CCD->Engine()->Engine(), theBmp) == GE_FALSE)
			{
				char szError[200];
				sprintf(szError, "*WARNING* File %s - Line %d: DisplaySplash: AddBitmap failed on '%s'\n",
						__FILE__, __LINE__, Loading);
				CCD->ReportError(szError, false);
				return;
			}
			// change begin Nout - Show save game image
			// geBitmap_SetColorKey(theBmp, GE_TRUE, 255, GE_FALSE);
			// change end Nout

			if(CCD->GetHasFocus())
			{
				geEngine_BeginFrame(CCD->Engine()->Engine(), M_Camera, GE_TRUE);

				if(geEngine_DrawBitmap(CCD->Engine()->Engine(), theBmp, NULL, x, y) == GE_FALSE)
				{
					char szError[200];
					sprintf(szError, "*WARNING* File %s - Line %d: DisplaySplash: DrawBitmap failed on '%s'\n",
							__FILE__, __LINE__, Loading);
					CCD->ReportError(szError, false);
				}

				geEngine_EndFrame(CCD->Engine()->Engine());
			}

			// changed QD 12/15/05
			geEngine_RemoveBitmap(CCD->Engine()->Engine(), theBmp);
			// end change
		}

		geBitmap_Destroy(&theBmp);
	}

	return;
}
// end change RF064

/* ------------------------------------------------------------------------------------ */
//	save a screen shot
/* ------------------------------------------------------------------------------------ */
void CRFMenu::ScreenShot()
{
	char filename[256];

	sprintf(filename, "screen%.3d.bmp",Screen);

	if(geEngine_ScreenShot(CCD->Engine()->Engine(), filename) == false)
	{
		CCD->ReportError("*WARNING* Failed to create screenshot file!", false);
		return;
	}

	Screen += 1;
}

/* ------------------------------------------------------------------------------------ */
//
// functions called by menu items
//
/* ------------------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------------------ */
//	MenuInitalize
//
//	initalize all menu items
/* ------------------------------------------------------------------------------------ */
void CRFMenu::MenuInitalize()
{
	// set gamma slider from engine gamma
	Gamma_Slide.Current=((Gamma_Slide.Max_X-Gamma_Slide.Min_X)*GetGammaPercent())/100;
	Detail_Slide.Current=Detail_Slide.Min_X + ((Detail_Slide.Max_X-Detail_Slide.Min_X)/2);
	Detail = 50;

	// set volume slider to max
	Vol_Slide.Current = Vol_Slide.Max_X/2;
	mVol_Slide.Current = mVol_Slide.Max_X/2;

	// set CD Music box from CD player status
	box0.Current = BOX_OFF;

	if(CCD->CDPlayer()->GetCdOn())
		box0.Current = BOX_ON;

	// set mouse sensitivity slider
	MouseSen = 0.002f;
	Sens_Slide.Current = ((Sens_Slide.Max_X-Sens_Slide.Min_X)*((int)(((MouseSen-MOUSEMIN)/(MOUSEMAX-MOUSEMIN))*100)))/100;

	// set mouse reverse
	MouseReverse = false;
	box2.Current = BOX_OFF;

	// set mouse filter
	MouseFilter = false;
	box4.Current = BOX_OFF;
	Filter.x = -1;

	// set crosshair
	box1.Current = BOX_OFF;

	// set no clipping
	NoClip = false;
	box7.Current = BOX_OFF;

	// set framerate box
	box5.Current = BOX_OFF;

	// set debug info box
	box6.Current = BOX_OFF;

	// set bounding box
	BoundingBox = false;
	box8.Current = BOX_OFF;
	SEBoundingBox = false;
	box9.Current = BOX_OFF;

// changed QD Shadows
	ShadowBox.Current = BOX_OFF;
	StencilShadows = false;
// end change

// changed QD Language Menu
	CurrentLanguage = CCD->GetLanguage();

	if(CurrentLanguage == 0)
		boxL1.Current = BOX_ON;
	else if(CurrentLanguage == 1)
		boxL2.Current = BOX_ON;
	else if(CurrentLanguage == 2)
		boxL3.Current = BOX_ON;
	else if(CurrentLanguage == 3)
		boxL4.Current = BOX_ON;
	else if(CurrentLanguage == 4)
		boxL5.Current = BOX_ON;
// end change

// begin add Nout - Show save game image
//	SaveBox.Current = 0;
//	LoadBox.Current = 0;
// end add Nout

	// load saved game file if it exists
// changed QD 12/15/05
	FILE *fd;

	if(CCD->FileExist(kSavegameFile, "savedgames.rgf"))
	{
		fd = CCD->OpenRFFile(kSavegameFile, "savedgames.rgf", "rb");

		if(fd)
		{
			fread(&SaveBox.Max, sizeof(int), 1, fd);

			for(int nTemp = 0; nTemp < SaveBox.Max; nTemp++)
			{
				// changed QD 12/15/05
				if(SaveBox.text[nTemp].text)
					free(SaveBox.text[nTemp].text);

				SaveBox.text[nTemp].text = (char*)malloc(30);
				// end change
				fread(SaveBox.text[nTemp].text, 30, 1, fd);
				fread(&SaveBox.text[nTemp].empty, sizeof(int), 1, fd);

				if(SaveBox.text[nTemp].empty != 0)
				{
					if(SaveScreen.Width > 0 && SaveScreen.Height > 0)
					{
						char filename[64];
						sprintf(filename, "SaveScreen%d.bmp", nTemp);

						if(CCD->FileExist(kBitmapFile, filename))
						{
							SavedGame[nTemp].SGImage = CreateFromFileName(filename);

							if(SavedGame[nTemp].SGImage == (geBitmap*)NULL)
							{
								char szBug[256];
								sprintf(szBug, "*ERROR* Bad file name %s", filename);
								CCD->ReportError(szBug, false);
								exit(-100);
							}

							geEngine_AddBitmap(CCD->Engine()->Engine(), SavedGame[nTemp].SGImage);
						}
					}
				}
			}

			fclose(fd);
		}
	}
// end change

	fd = CCD->OpenRFFile(kInstallFile, "setup.ini", "rb");

	if(fd)
	{
		fread(&Gamma_Slide.Current, sizeof(int), 1, fd);
		SetGamma((Gamma_Slide.Current*100)/Gamma_Slide.Max_X);

		fread(&Vol_Slide.Current, sizeof(int), 1, fd);
		SetVol((Vol_Slide.Current*100)/Vol_Slide.Max_X);

		fread(&Sens_Slide.Current, sizeof(int), 1, fd);
		MouseSen = ((((float)((Sens_Slide.Current*100)/Sens_Slide.Max_X)/100.0f)*(MOUSEMAX-MOUSEMIN))+MOUSEMIN);

		fread(&box0.Current, sizeof(int), 1, fd);

		if(CCD->CDPlayer()->GetCdOn())
			SetCDPlayer(box0.Current);

		fread(&box1.Current, sizeof(int), 1, fd);
		fread(&box2.Current, sizeof(int), 1, fd);

		if(box2.Current==BOX_ON)
			MouseReverse = true;
		else
			MouseReverse = false;

		fread(&box4.Current, sizeof(int), 1, fd);
		if(box4.Current==BOX_ON)
			MouseFilter = true;
		else
			MouseFilter = false;

		fread(&Detail_Slide.Current, sizeof(int), 1, fd);
		Detail = (Detail_Slide.Current*100)/Detail_Slide.Max_X;

		fread(&mVol_Slide.Current, sizeof(int), 1, fd);
		int percent = (mVol_Slide.Current*100)/mVol_Slide.Max_X;
		mVolLevel=((float)percent/100.0f);

		if(percent == 0)
			mVolLevel = 0.0f;

// changed QD Shadows
		fread(&ShadowBox.Current, sizeof(int), 1, fd);
		if(ShadowBox.Current==BOX_ON)
			StencilShadows = true;
		else
			StencilShadows = false;
// end change

		fclose(fd);
	}

// changed RF064
	int index = 0;
	bool flg = false;
	CIniFile AttrFile("control.ini");

	if(AttrFile.ReadFile())
	{
		string KeyName = AttrFile.FindFirstKey();
		string Type;

		while(KeyName != "")
		{
			if(KeyName == "Controls")
			{
				Type = AttrFile.GetValue(KeyName, "moveforward");

				if(Type == "true")
				{
					Redef[index].text = "Move Forward";
					Redef[index].action = RGF_K_FORWARD;
					index++;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_FORWARD);

				Type = AttrFile.GetValue(KeyName, "movebackward");

				if(Type == "true")
				{
					Redef[index].text = "Move Backward";
					Redef[index].action = RGF_K_BACKWARD;
					index++;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_BACKWARD);

				Type = AttrFile.GetValue(KeyName, "strafeleft");

				if(Type == "true")
				{
					Redef[index].text = "Strafe Left";
					Redef[index].action = RGF_K_LEFT;
					index++;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_LEFT);

				Type = AttrFile.GetValue(KeyName, "straferight");

				if(Type == "true")
				{
					Redef[index].text = "Strafe Right";
					Redef[index].action = RGF_K_RIGHT;
					index++;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_RIGHT);

				Type = AttrFile.GetValue(KeyName, "jump");

				if(Type == "true")
				{
					Redef[index].text = "Jump";
					Redef[index].action = RGF_K_JUMP;
					index++;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_JUMP);

				Type = AttrFile.GetValue(KeyName, "crouch");

				if(Type == "true")
				{
					Redef[index].text = "Crouch";
					Redef[index].action = RGF_K_CROUCH;
					index++;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_CROUCH);

				Type = AttrFile.GetValue(KeyName, "run");

				if(Type == "true")
				{
					Redef[index].text = "Run";
					Redef[index].action = RGF_K_RUN;
					index++;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_RUN);

				Type = AttrFile.GetValue(KeyName, "lookup");

				if(Type == "true")
				{
					Redef[index].text = "Look Up";
					Redef[index].action = RGF_K_LOOKUP;
					index++;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_LOOKUP);

				Type = AttrFile.GetValue(KeyName, "lookdown");

				if(Type == "true")
				{
					Redef[index].text = "Look Down";
					Redef[index].action = RGF_K_LOOKDOWN;
					index++;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_LOOKDOWN);

				Type = AttrFile.GetValue(KeyName, "turnleft");

				if(Type == "true")
				{
					Redef[index].text = "Turn Left";
					Redef[index].action = RGF_K_TURN_LEFT;
					index++;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_TURN_LEFT);

				Type = AttrFile.GetValue(KeyName, "turnright");

				if(Type == "true")
				{
					Redef[index].text = "Turn Right";
					Redef[index].action = RGF_K_TURN_RIGHT;
					index++;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_TURN_RIGHT);

				Type = AttrFile.GetValue(KeyName, "lookstraight");

				if(Type == "true")
				{
					Redef[index].text = "Look Straight";
					Redef[index].action = RGF_K_LOOKSTRAIGHT;
					index++;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_LOOKSTRAIGHT);

				Type = AttrFile.GetValue(KeyName, "attack");

				if(Type == "true")
				{
					Redef[index].text = "Attack";
					Redef[index].action = RGF_K_FIRE;
					index++;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_FIRE);

				Type = AttrFile.GetValue(KeyName, "altattack");

				if(Type == "true")
				{
					Redef[index].text = "Alt Attack";
					Redef[index].action = RGF_K_ALTFIRE;
					index++;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_ALTFIRE);

				Type = AttrFile.GetValue(KeyName, "weapon0");

				if(Type == "true")
				{
					Redef[index].text = "Weapon 0";
					Redef[index].action = RGF_K_WEAPON_0;
					index++;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_WEAPON_0);

				Type = AttrFile.GetValue(KeyName, "weapon1");
				if(Type == "true")
				{
					Redef[index].text = "Weapon 1";
					Redef[index].action = RGF_K_WEAPON_1;
					index++;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_WEAPON_1);

				Type = AttrFile.GetValue(KeyName, "weapon2");

				if(Type == "true")
				{
					Redef[index].text = "Weapon 2";
					Redef[index].action = RGF_K_WEAPON_2;
					index++;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_WEAPON_2);

				Type = AttrFile.GetValue(KeyName, "weapon3");

				if(Type == "true")
				{
					Redef[index].text = "Weapon 3";
					Redef[index].action = RGF_K_WEAPON_3;
					index++;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_WEAPON_3);

				Type = AttrFile.GetValue(KeyName, "weapon4");

				if(Type == "true")
				{
					Redef[index].text = "Weapon 4";
					Redef[index].action = RGF_K_WEAPON_4;
					index++;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_WEAPON_4);

				Type = AttrFile.GetValue(KeyName, "weapon5");

				if(Type == "true")
				{
					Redef[index].text = "Weapon 5";
					Redef[index].action = RGF_K_WEAPON_5;
					index++;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_WEAPON_5);

				Type = AttrFile.GetValue(KeyName, "weapon6");

				if(Type == "true")
				{
					Redef[index].text = "Weapon 6";
					Redef[index].action = RGF_K_WEAPON_6;
					index++;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_WEAPON_6);

				Type = AttrFile.GetValue(KeyName, "weapon7");

				if(Type == "true")
				{
					Redef[index].text = "Weapon 7";
					Redef[index].action = RGF_K_WEAPON_7;
					index++;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_WEAPON_7);

				Type = AttrFile.GetValue(KeyName, "weapon8");

				if(Type == "true")
				{
					Redef[index].text = "Weapon 8";
					Redef[index].action = RGF_K_WEAPON_8;
					index++;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_WEAPON_8);

				Type = AttrFile.GetValue(KeyName, "weapon9");

				if(Type == "true")
				{
					Redef[index].text = "Weapon 9";
					Redef[index].action = RGF_K_WEAPON_9;
					index++;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_WEAPON_9);

				Type = AttrFile.GetValue(KeyName, "lookmode");

				if(Type == "true")
				{
					Redef[index].text = "Look Mode";
					Redef[index].action = RGF_K_LOOKMODE;
					index++;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_LOOKMODE);

				Type = AttrFile.GetValue(KeyName, "cameramode");

				if(Type == "true")
				{
					Redef[index].text = "Camera Mode";
					Redef[index].action = RGF_K_CAMERA;
					index++;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_CAMERA);

				Type = AttrFile.GetValue(KeyName, "zoomin");

				if(Type == "true")
				{
					Redef[index].text = "Zoom In";
					Redef[index].action = RGF_K_ZOOM_IN;
					index++;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_ZOOM_IN);

				Type = AttrFile.GetValue(KeyName, "zoomout");

				if(Type == "true")
				{
					Redef[index].text = "Zoom Out";
					Redef[index].action = RGF_K_ZOOM_OUT;
					index++;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_ZOOM_OUT);

				Type = AttrFile.GetValue(KeyName, "zoomweapon");

				if(Type == "true")
				{
					Redef[index].text = "Zoom Weapon";
					Redef[index].action = RGF_K_ZOOM_WEAPON;
					index++;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_ZOOM_WEAPON);

				Type = AttrFile.GetValue(KeyName, "holsterweapon");

				if(Type == "true")
				{
					Redef[index].text = "Holster Weapon";
					Redef[index].action = RGF_K_HOLSTER_WEAPON;
					index++;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_HOLSTER_WEAPON);

				Type = AttrFile.GetValue(KeyName, "camerareset");

				if(Type == "true")
				{
					Redef[index].text = "Camera Reset";
					Redef[index].action = RGF_K_CAMERA_RESET;
					index++;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_CAMERA_RESET);

				Type = AttrFile.GetValue(KeyName, "quicksave");

				if(Type == "true")
				{
					Redef[index].text = "Quick Save";
					Redef[index].action = RGF_K_QUICKSAVE;
					index++;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_QUICKSAVE);

				Type = AttrFile.GetValue(KeyName, "quickload");

				if(Type == "true")
				{
					Redef[index].text = "Quick Load";
					Redef[index].action = RGF_K_QUICKLOAD;
					index++;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_QUICKLOAD);

				Type = AttrFile.GetValue(KeyName, "1stperson");

				if(Type == "true")
				{
					Redef[index].text = "1st Person";
					Redef[index].action = RGF_K_FIRST_PERSON_VIEW;
					index++;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_FIRST_PERSON_VIEW);

				Type = AttrFile.GetValue(KeyName, "3rdperson");

				if(Type == "true")
				{
					Redef[index].text = "3rd Person";
					Redef[index].action = RGF_K_THIRD_PERSON_VIEW;
					index++;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_THIRD_PERSON_VIEW);

				Type = AttrFile.GetValue(KeyName, "isometric");

				if(Type == "true")
				{
					Redef[index].text = "Isometric";
					Redef[index].action = RGF_K_ISO_VIEW;
					index++;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_ISO_VIEW);

				Type = AttrFile.GetValue(KeyName, "screenshot");

				if(Type == "true")
				{
					Redef[index].text = "Screenshot";
					Redef[index].action = RGF_K_SCRNSHOT;
					index++;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_SCRNSHOT);

				Type = AttrFile.GetValue(KeyName, "help");

				if(Type == "true")
				{
					Redef[index].text = "Help";
					Redef[index].action = RGF_K_HELP;
					index++;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_HELP);

				Type = AttrFile.GetValue(KeyName, "hud");

				if(Type == "true")
				{
					Redef[index].text = "Hud";
					Redef[index].action = RGF_K_HUD;
					index++;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_HUD);

				Type = AttrFile.GetValue(KeyName, "lightonoff");

				if(Type == "true")
				{
					Redef[index].text = "Light On/Off";
					Redef[index].action = RGF_K_LIGHT;
					index++;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_LIGHT);

				Type = AttrFile.GetValue(KeyName, "useitem");

				if(Type == "true")
				{
					Redef[index].text = "Use Item";
					Redef[index].action = RGF_K_USE;
					index++;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_USE);

				Type = AttrFile.GetValue(KeyName, "inventory");

				if(Type == "true")
				{
					Redef[index].text = "Inventory";
					Redef[index].action = RGF_K_INVENTORY;
					index++;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_INVENTORY);

				Type = AttrFile.GetValue(KeyName, "console");

				if(Type == "true")
				{
					Redef[index].text = "Console";
					Redef[index].action = RGF_K_CONSOLE;
					index++;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_CONSOLE);

				Type = AttrFile.GetValue(KeyName, "dropweapon");

				if(Type == "true")
				{
					Redef[index].text = "Drop Weapon";
					Redef[index].action = RGF_K_DROP;
					index++;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_DROP);

				Type = AttrFile.GetValue(KeyName, "reload");

				if(Type == "true")
				{
					Redef[index].text = "Reload Weapon";
					Redef[index].action = RGF_K_RELOAD;
					index++;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_RELOAD);

				Type = AttrFile.GetValue(KeyName, "powerup");

				if(Type == "true")
				{
					Redef[index].text = "Increase Power";
					Redef[index].action = RGF_K_POWERUP;
					index++;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_POWERUP);

				Type = AttrFile.GetValue(KeyName, "powerdown");

				if(Type == "true")
				{
					Redef[index].text = "Decrease Power";
					Redef[index].action = RGF_K_POWERDWN;
					index++;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_POWERDWN);

				Redef[index].text = NULL;
				flg = true;
				break;
			}
			KeyName = AttrFile.FindNextKey();
		}

		if(!flg)
		{
			char szError[256];
			sprintf(szError, "*ERROR* File %s - Line %d: Missing section in Control.ini", __FILE__, __LINE__);
			CCD->ReportError(szError, false);
			CCD->ShutdownLevel();
			delete CCD;
			CCD = NULL;
			MessageBox(NULL, szError, "Controls", MB_OK);
			exit(-333);
		}
	}
	else
	{
		Redef[index].text = "Move Forward";
		Redef[index].action = RGF_K_FORWARD;

		index++;

		Redef[index].text = "Move Backward";
		Redef[index].action = RGF_K_BACKWARD;

		index++;

		Redef[index].text = "Strafe Left";
		Redef[index].action = RGF_K_LEFT;

		index++;

		Redef[index].text = "Strafe Right";
		Redef[index].action = RGF_K_RIGHT;

		index++;

		Redef[index].text = "Jump";
		Redef[index].action = RGF_K_JUMP;

		index++;

		Redef[index].text = "Crouch";
		Redef[index].action = RGF_K_CROUCH;

		index++;

		Redef[index].text = "Run";
		Redef[index].action = RGF_K_RUN;

		index++;

		Redef[index].text = "Look Up";
		Redef[index].action = RGF_K_LOOKUP;

		index++;

		Redef[index].text = "Look Down";
		Redef[index].action = RGF_K_LOOKDOWN;

		index++;

		Redef[index].text = "Turn Left";
		Redef[index].action = RGF_K_TURN_LEFT;

		index++;

		Redef[index].text = "Turn Right";
		Redef[index].action = RGF_K_TURN_RIGHT;

		index++;

		Redef[index].text = "Look Straight";
		Redef[index].action = RGF_K_LOOKSTRAIGHT;

		index++;

		Redef[index].text = "Attack";
		Redef[index].action = RGF_K_FIRE;

		index++;

		Redef[index].text = "Alt Attack";
		Redef[index].action = RGF_K_ALTFIRE;

		index++;

		Redef[index].text = "Weapon 0";
		Redef[index].action = RGF_K_WEAPON_0;

		index++;

		Redef[index].text = "Weapon 1";
		Redef[index].action = RGF_K_WEAPON_1;

		index++;

		Redef[index].text = "Weapon 2";
		Redef[index].action = RGF_K_WEAPON_2;

		index++;

		Redef[index].text = "Weapon 3";
		Redef[index].action = RGF_K_WEAPON_3;

		index++;

		Redef[index].text = "Weapon 4";
		Redef[index].action = RGF_K_WEAPON_4;

		index++;

		Redef[index].text = "Weapon 5";
		Redef[index].action = RGF_K_WEAPON_5;

		index++;

		Redef[index].text = "Weapon 6";
		Redef[index].action = RGF_K_WEAPON_6;

		index++;

		Redef[index].text = "Weapon 7";
		Redef[index].action = RGF_K_WEAPON_7;

		index++;

		Redef[index].text = "Weapon 8";
		Redef[index].action = RGF_K_WEAPON_8;

		index++;

		Redef[index].text = "Weapon 9";
		Redef[index].action = RGF_K_WEAPON_9;

		index++;

		Redef[index].text = "Look Mode";
		Redef[index].action = RGF_K_LOOKMODE;

		index++;

		Redef[index].text = "Camera Mode";
		Redef[index].action = RGF_K_CAMERA;

		index++;

		Redef[index].text = "Zoom In";
		Redef[index].action = RGF_K_ZOOM_IN;

		index++;

		Redef[index].text = "Zoom Out";
		Redef[index].action = RGF_K_ZOOM_OUT;

		index++;

		Redef[index].text = "Zoom Weapon";
		Redef[index].action = RGF_K_ZOOM_WEAPON;

		index++;

		Redef[index].text = "Holster Weapon";
		Redef[index].action = RGF_K_HOLSTER_WEAPON;

		index++;

		Redef[index].text = "Camera Reset";
		Redef[index].action = RGF_K_CAMERA_RESET;

		index++;

		Redef[index].text = "Quick Save";
		Redef[index].action = RGF_K_QUICKSAVE;

		index++;

		Redef[index].text = "Quick Load";
		Redef[index].action = RGF_K_QUICKLOAD;

		index++;

		Redef[index].text = "1st Person";
		Redef[index].action = RGF_K_FIRST_PERSON_VIEW;

		index++;

		Redef[index].text = "3rd Person";
		Redef[index].action = RGF_K_THIRD_PERSON_VIEW;

		index++;

		Redef[index].text = "Isometric";
		Redef[index].action = RGF_K_ISO_VIEW;

		index++;

		Redef[index].text = "Screenshot";
		Redef[index].action = RGF_K_SCRNSHOT;

		index++;

		Redef[index].text = "Help";
		Redef[index].action = RGF_K_HELP;

		index++;

		Redef[index].text = "Hud";
		Redef[index].action = RGF_K_HUD;

		index++;

		Redef[index].text = "Light On/Off";
		Redef[index].action = RGF_K_LIGHT;

		index++;

		Redef[index].text = "Use Item";
		Redef[index].action = RGF_K_USE;

		index++;

		Redef[index].text = "Inventory";
		Redef[index].action = RGF_K_INVENTORY;

		index++;

		Redef[index].text = "Console";
		Redef[index].action = RGF_K_CONSOLE;

		index++;

		Redef[index].text = "Drop Weapon";
		Redef[index].action = RGF_K_DROP;

		index++;

		Redef[index].text = "Reload Weapon";
		Redef[index].action = RGF_K_RELOAD;

		index++;

		Redef[index].text = "Increase Power";
		Redef[index].action = RGF_K_POWERUP;

		index++;

		Redef[index].text = "Decrease Power";
		Redef[index].action = RGF_K_POWERDWN;

		index++;

		Redef[index].text = NULL;
	}
// end change RF064
}

/* ------------------------------------------------------------------------------------ */
//	PlaySoundDef
//
//	replacement for play sound to keep track
//	of all looping sounds
/* ------------------------------------------------------------------------------------ */
geSound	*CRFMenu::PlaySoundDef(geSound_System *SoundS, geSound_Def *SoundDef, geFloat Volume, geFloat Pan, geFloat Frequency, geBoolean Loop)
{
	geSound	*Sound;
	SoundList *pool;

	Sound = geSound_PlaySoundDef(SoundS, SoundDef, Volume, Pan, Frequency, Loop);

	if(Loop)
	{
		pool = GE_RAM_ALLOCATE_STRUCT(SoundList);
		memset(pool, 0xdd, sizeof(SoundList));
		pool->next = Bottom;
		Bottom = pool;

		if(pool->next)
			pool->next->prev = pool;

		pool->Sound = Sound;
		pool->Volume = Volume;
		pool->Pan = Pan;
		pool->Frequency = Frequency;
	}

	return Sound;
}

/* ------------------------------------------------------------------------------------ */
//	StopSound
//
//	replacement for stop sound to remove all dead entries
/* ------------------------------------------------------------------------------------ */
geBoolean CRFMenu::StopSound(geSound_System *SoundS, geSound *Sound)
{
	SoundList *pool, *temp;

	pool = Bottom;

	while(pool != NULL)
	{
		temp = pool->next;

		if(pool->Sound == Sound)
		{
			if(Bottom == pool)
			{
				Bottom = pool->next;

				if(Bottom != (SoundList *)NULL)
					Bottom->prev = (SoundList *)NULL;
			}
			else
			{
				pool->prev->next=pool->next;

				if(pool->next != (SoundList *)NULL)
					pool->next->prev=pool->prev;
			}

			geRam_Free(pool);
			break;
		}

		pool = temp;
	}

	return geSound_StopSound(SoundS, Sound);
}

/* ------------------------------------------------------------------------------------ */
//	zero volume of all looping sounds
/* ------------------------------------------------------------------------------------ */
void CRFMenu::ClearVol()
{
	SoundList *pool, *temp;

	pool = Bottom;

	while(pool != NULL)
	{
		temp = pool->next;

		if(geSound_SoundIsPlaying(CCD->Engine()->AudioSystem(), pool->Sound) == GE_TRUE)
		{
			geSound_ModifySound(CCD->Engine()->AudioSystem(), pool->Sound, 0.0f, 0.0f, 0.0f);
		}
		else
		{
			if(Bottom == pool)
			{
				Bottom = pool->next;

				if(Bottom != (SoundList *)NULL)
					Bottom->prev = (SoundList *)NULL;
			}
			else
			{
				pool->prev->next=pool->next;

				if(pool->next != (SoundList *)NULL)
					pool->next->prev=pool->prev;
			}

			free(pool);
		}

		pool = temp;
	}
}

/* ------------------------------------------------------------------------------------ */
//	ResetVol
/* ------------------------------------------------------------------------------------ */
void CRFMenu::ResetVol()
{
	SoundList *pool, *temp;

	pool = Bottom;

	while(pool != NULL)
	{
		temp = pool->next;
		geSound_ModifySound(CCD->Engine()->AudioSystem(), pool->Sound, pool->Volume, pool->Pan, pool->Frequency);
		pool = temp;
	}
}

/* ------------------------------------------------------------------------------------ */
//	clear out sound list
/* ------------------------------------------------------------------------------------ */
void CRFMenu::DeleteSound()
{
	SoundList *pool, *temp;

	pool = Bottom;

	while(pool != NULL)
	{
		temp = pool->next;
		free(pool);
		pool = temp;
	}

	Bottom = (SoundList*)NULL;
}

/* ------------------------------------------------------------------------------------ */
// get percentage from gamma setting
/* ------------------------------------------------------------------------------------ */
static int GetGammaPercent()
{
	float gamma;

	geEngine_GetGamma(CCD->Engine()->Engine(), &gamma);
	return (int)(((gamma-GAMMAMIN)/(GAMMAMAX-GAMMAMIN))*100);
}

/* ------------------------------------------------------------------------------------ */
//	set gamma from slider percentage
/* ------------------------------------------------------------------------------------ */
static void SetGamma(int percent)
{
	float gamma;

	gamma = (((float)percent/100.0f)*(GAMMAMAX-GAMMAMIN))+GAMMAMIN;
	geEngine_SetGamma(CCD->Engine()->Engine(), gamma);
}

/* ------------------------------------------------------------------------------------ */
//	set detail from slider percentage
/* ------------------------------------------------------------------------------------ */
static void SetDetail(int percent)
{
	CCD->MenuManager()->SetDetail(percent);

	if(CCD->MenuManager()->GetInGame()==1)
	{
		EnvironmentSetup *theState = CCD->Player()->GetEnvSetup();
		float detailevel = (float)CCD->MenuManager()->GetDetail();

		if(theState->EnableDistanceFog == GE_TRUE)
		{
			float start = theState->FogStartDistLow + ((theState->FogStartDistHigh - theState->FogStartDistLow)*(detailevel/100));
			float end = theState->TotalFogDistLow + ((theState->TotalFogDistHigh - theState->TotalFogDistLow)*(detailevel/100));
			CCD->Engine()->SetFogParameters(theState->DistanceFogColor,	start, end);
		}

		if(theState->UseFarClipPlane == GE_TRUE)
		{
			float dist = theState->FarClipPlaneDistLow + ((theState->FarClipPlaneDistHigh - theState->FarClipPlaneDistLow)*(detailevel/100));
			CCD->CameraManager()->SetFarClipPlane(dist);
		}
	}
}

// changed QD Shadows
/* ------------------------------------------------------------------------------------ */
//	SetStencilShadows
/* ------------------------------------------------------------------------------------ */
static void SetStencilShadows(int current)
{
	if(current == BOX_ON)
		CCD->MenuManager()->SetStencilShadows(true);
	else
		CCD->MenuManager()->SetStencilShadows(false);

	if(CCD->MenuManager()->GetInGame() == 1)
	{
		EnvironmentSetup *theState = CCD->Player()->GetEnvSetup();

		geEngine_SetStencilShadowsEnable(CCD->Engine()->Engine(), current, theState->SShadowsMaxLightToUse,
			theState->SShadowsColor.r, theState->SShadowsColor.g, theState->SShadowsColor.b, theState->SShadowsAlpha);
	}
}
// end change

/* ------------------------------------------------------------------------------------ */
//	set mouse sensitivity from slider percentage
/* ------------------------------------------------------------------------------------ */
static void SetSens(int percent)
{
	CCD->MenuManager()->SetMouseSen((((float)percent*0.01f)*(MOUSEMAX-MOUSEMIN))+MOUSEMIN);
}

/* ------------------------------------------------------------------------------------ */
//	set volume from slider percentage
/* ------------------------------------------------------------------------------------ */
static void SetVol(int percent)
{
	float vol;
// changed RF064
	vol = ((float)percent*0.01f);

	if(percent == 0)
		vol = 0.0f;

	CCD->MenuManager()->SetMusicVol(vol);
// end change RF064
}

/* ------------------------------------------------------------------------------------ */
//	set volume from slider percentage
/* ------------------------------------------------------------------------------------ */
static void SetmVol(int percent)
{
	float vol;
	vol = ((float)percent*0.01f);

	if(percent == 0)
		vol = 0.0f;

	CCD->MenuManager()->SetmMusicVol(vol);
}

// changed RF063
/* ------------------------------------------------------------------------------------ */
//	DoGame
/* ------------------------------------------------------------------------------------ */
void CRFMenu::DoGame(bool editor)
{
	useselect = false;
	usenameselect = false; // changed QD 12/15/05

	// changed QD 12/15/05
	//if(CCD->GetCSelect() || CCD->GetCDifficult())
	if(CCD->GetCSelect() || CCD->GetCDifficult() || CCD->GetNSelect())
	{
		if(editor)
		{
			M_CameraRect.Left	= 0;
			M_CameraRect.Right	= CCD->Engine()->Width() - 1;
			M_CameraRect.Top	= 0;
			M_CameraRect.Bottom = CCD->Engine()->Height() - 1;
			M_Camera = geCamera_Create(2.0f, &M_CameraRect);
			LoopOnce = 0;

			if(musictype != -1)
			{
				if(musictype == 1)
					MIDIPlayer()->Play(music, true);
				else
					m_Streams->Play(true);
			}
		}

		if(CCD->GetCSelect())
		{
			if(CCD->MenuManager()->ProcessMenu(SelectMenu, SelectBack, SelectTitle)==1)
			{
				FadeSet(-1, TimeFade);
				return;
			}

			useselect = true;
		}

		// changed QD 12/15/05
		if(CCD->GetNSelect())
		{
			if(CCD->MenuManager()->ProcessMenu(PlayerNameMenu, PlayerNameBack, PlayerNameTitle)==1)
			{
				FadeSet(-1, TimeFade);
				return;
			}

			usenameselect = true;
		}
		// end change

		if(CCD->GetCDifficult())
		{
			CCD->SetDifficultLevel(CCD->MenuManager()->ProcessMenu(DifficultMenu, DifficultBack, DifficultTitle));
		}
	}

	if(CCD->MenuManager()->GetMusicType() != -1)
	{
		CCD->MenuManager()->StopMenuMusic();
	}

	if(!editor)
	{
		if(CCD->MenuManager()->GetInGame() == 1)
		{
			CCD->AudioStreams()->StopAll(); // stop old streaming audio that might be paused
			CCD->ShutdownLevel();
			CCD->MenuManager()->DeleteSound();

			if(CCD->CDPlayer())
				CCD->CDPlayer()->Stop();

			if(CCD->MIDIPlayer())
				CCD->MIDIPlayer()->Stop();
		}
		CCD->MenuManager()->DisplaySplash(); // changed RF064
	}
	else
	{
		// changed QD 12/15/05
		//if(CCD->GetCSelect() || CCD->GetCDifficult())
		if(CCD->GetCSelect() || CCD->GetCDifficult() || CCD->GetNSelect())
			CCD->MenuManager()->DisplaySplash(); // changed RF064
	}

// start multiplayer
	if(CCD->GetMultiPlayer())
	{
		if(!CCD->NetPlayerManager()->Initialize(CCD->GetServer(), ServerIP))
		{
			CCD->ReportError("*WARNING* Cant Begin Multiplayer session", false);
			return;
		}
	}
// end multiplayer

	if((CCD->InitializeLevel(CCD->MenuManager()->GetLevelName())) != 0)
	{
		CCD->ReportError("*ERROR* Failed to initialize first level", false);
		CCD->ShutdownLevel();						// Clean up anything that got loaded
		delete CCD;
		CCD = NULL;   // Kill off the engine and such
		MessageBox(NULL, CCD->MenuManager()->GetLevelName(), "Failed to load level", MB_OK);
		exit(-333);
	}

// changed RF064
	CCD->Player()->DisableFog();				// Turn off fogging for cut scene
	CCD->Player()->DisableClipPlane();	// Turn off the clipping plane as well
// end change RF064

// changed RF064
	CCD->TerrainMgr()->Init();
// end change RF064

	//	Ok, move the player avatar to the correct player start in the
	//	..game level.
	if(CCD->Player()->MoveToStart() != RGF_SUCCESS)
	{
		CCD->ReportError("*ERROR* Failed to move player to start", false);
		CCD->ShutdownLevel();
		delete CCD;
		exit(-336);
	}

	//	Play the opening cut scene, if one exists
	CCD->PlayOpeningCutScene();

// start multiplayer
	if(CCD->GetMultiPlayer())
	{
		SetTimer(CCD->Engine()->WindowHandle(), NULL, TIMERINTERVAL, NULL);
	}
// end multiplayer

	CCD->MenuManager()->GameLoop();
}

/* ------------------------------------------------------------------------------------ */
//	ChangeCurrent - change selected character
/* ------------------------------------------------------------------------------------ */
void CRFMenu::ChangeCurrent(bool direction)
{
	if(direction)
	{
		CurrentSelect += 1;

		if(CurrentSelect >= MaxSelect)
			CurrentSelect = 0;
	}
	else
	{
		CurrentSelect -= 1;

		if(CurrentSelect < 0)
			CurrentSelect = MaxSelect-1;
	}
}

/* ------------------------------------------------------------------------------------ */
//	PrevChar
/* ------------------------------------------------------------------------------------ */
static void PrevChar()
{
	CCD->MenuManager()->ChangeCurrent(false);
}

/* ------------------------------------------------------------------------------------ */
//	NextChar
/* ------------------------------------------------------------------------------------ */
static void NextChar()
{
	CCD->MenuManager()->ChangeCurrent(true);
}

// changed QD 12/15/05
/* ------------------------------------------------------------------------------------ */
//	AcceptChar
//
//	Do character specific game preparations
/* ------------------------------------------------------------------------------------ */
static void AcceptChar()
{
	if(!CCD->GetUseDialog() && !CCD->GetCmdLine())
	{
		char *StartLevel = CCD->MenuManager()->GetCurrentStartLevel();

		if(StartLevel[0] != 0)
			CCD->MenuManager()->SetLevelName(StartLevel);
	}

	if(CCD->GetNSelect())
		strcpy(PlayerName.text, CCD->MenuManager()->GetCurrentName());

}

/* ------------------------------------------------------------------------------------ */
//	SetName - set Player Name
/* ------------------------------------------------------------------------------------ */
static void SetName()
{
	CCD->MenuManager()->SetSelectedName(PlayerName.text);
}

/* ------------------------------------------------------------------------------------ */
//	ResetName - reset Player Name
/* ------------------------------------------------------------------------------------ */
static void ResetName()
{
	if(CCD->MenuManager()->GetUseSelect())
		strcpy(PlayerName.text, CCD->MenuManager()->GetCurrentName());
	else
		strcpy(PlayerName.text, CCD->GetDefaultPlayerName());
}
// end change

/* ------------------------------------------------------------------------------------ */
//	Run the level
/* ------------------------------------------------------------------------------------ */

// start multiplayer
static void RunHostGame()
{
	CCD->ShutDownNetWork();
	CCD->SetMultiPlayer(true, true);
    CCD->MenuManager()->DoGame(false);
}

static void RunJoinGame()
{
	CCD->ShutDownNetWork();
	CCD->SetMultiPlayer(true, false);
    CCD->MenuManager()->DoGame(false);
}

static void RunGame()
{
	CCD->ShutDownNetWork();
	CCD->SetMultiPlayer(false, false);
	CCD->MenuManager()->DoGame(false);
}
// end multiplayer

// end change RF063

/* ------------------------------------------------------------------------------------ */
//	turn CD music on/off
/* ------------------------------------------------------------------------------------ */
static void SetCDPlayer(int current)
{
	if(current == BOX_ON)
	{
		CCD->CDPlayer()->SetCdOn(true);
	}
	else
	{
		CCD->CDPlayer()->SetCdOn(false);
		CCD->CDPlayer()->Stop();
	}
}

/* ------------------------------------------------------------------------------------ */
//	turn mouse reverse on/off
/* ------------------------------------------------------------------------------------ */
static void SetReverse(int current)
{
	if(current == BOX_ON)
		CCD->MenuManager()->SetMouseReverse(true);
	else
		CCD->MenuManager()->SetMouseReverse(false);
}

/* ------------------------------------------------------------------------------------ */
//	turn mouse filter on/off
/* ------------------------------------------------------------------------------------ */
static void SetFiltering(int current)
{
	if(current == BOX_ON)
		CCD->MenuManager()->SetMouseFilter(true);
	else
		CCD->MenuManager()->SetMouseFilter(false);
}

/* ------------------------------------------------------------------------------------ */
//	turn player bounding box on/off
/* ------------------------------------------------------------------------------------ */
static void SetBBox(int current)
{
	if(current == BOX_ON)
		CCD->MenuManager()->SetBoundBox(true);
	else
		CCD->MenuManager()->SetBoundBox(false);
}

/* ------------------------------------------------------------------------------------ */
//	turn entity bounding box on/off
/* ------------------------------------------------------------------------------------ */
static void SetSEBBox(int current)
{
	if(current == BOX_ON)
		CCD->MenuManager()->SetSEBoundBox(true);
	else
		CCD->MenuManager()->SetSEBoundBox(false);
}

/* ------------------------------------------------------------------------------------ */
//	turn no clipping on/off
/* ------------------------------------------------------------------------------------ */
static void SetClipping(int current)
{
	if(current == BOX_ON)
		CCD->MenuManager()->SetNoClip(true);
	else
		CCD->MenuManager()->SetNoClip(false);
}

/* ------------------------------------------------------------------------------------ */
//	reset keys to default actions
/* ------------------------------------------------------------------------------------ */
static void ResetAction()
{
	CCD->Input()->Default();
}

/* ------------------------------------------------------------------------------------ */
//	save game to current slot
/* ------------------------------------------------------------------------------------ */
static void SetSlot()
{
	if(SaveBox.Current != -1) // must have selected a slot
	{
		// slot must be visible in table
		if(SaveBox.Current >= SaveBox.Start && SaveBox.Current < (SaveBox.Max_Show+SaveBox.Start))
		{
			struct tm *newtime;
			time_t long_time;
			char *ttext;
			char filename[256];

			time(&long_time);
			newtime = localtime(&long_time);
			ttext = asctime(newtime);
			ttext += 4;
			strcpy(filename, CCD->Engine()->LevelName());
			*(strrchr(filename,'.')) = 0;

			// set date/time and level name
			// changed QD 12/15/05
			if(SaveBox.text[SaveBox.Current].text)
				free(SaveBox.text[SaveBox.Current].text);

			SaveBox.text[SaveBox.Current].text = (char*)malloc(30);
			// end change
			sprintf(SaveBox.text[SaveBox.Current].text, "%.12s %.16s", ttext, filename);

			// mark slot as in use
			SaveBox.text[SaveBox.Current].empty = 1;

			// save file of saved game info
			FILE *fd = CCD->OpenRFFile(kSavegameFile, "savedgames.rgf", "wb");

			if(fd == NULL)
			{
				char szBug[200];
				sprintf(szBug, "*WARNING* File %s - Line %d: Save Game: Failed to create savedgames.rgf\n",
						__FILE__, __LINE__);
				CCD->ReportError(szBug, false);
				return;										// Fatal error
			}

			fwrite(&SaveBox.Max, sizeof(int), 1, fd);

			for(int nTemp=0; nTemp<SaveBox.Max; nTemp++)
			{
				fwrite(SaveBox.text[nTemp].text, 30, 1, fd);
				fwrite(&SaveBox.text[nTemp].empty, sizeof(int), 1, fd);
			}

			fclose(fd);

			// save game information
			sprintf(filename, "savgame%d.sav", SaveBox.Current);
			FILE *outFD = CCD->OpenRFFile(kSavegameFile, filename, "wb");

			if(outFD == NULL)
			{
				CCD->ReportError("*WARNING* Failed to create savegame file!", false);
				return;
			}

			CCD->Engine()->SaveTo(outFD);
			CCD->MenuManager()->SaveTo(outFD, false);
			CCD->Player()->SaveTo(outFD);
			CCD->Doors()->SaveTo(outFD, false);
			CCD->Platforms()->SaveTo(outFD, false);
			CCD->Props()->SaveTo(outFD, false);
			// changed QD 02/01/07
			CCD->Meshes()->SaveTo(outFD, false);
			// end change
			CCD->Teleporters()->SaveTo(outFD, false);
			CCD->MorphingFields()->SaveTo(outFD);
			CCD->MIDIPlayer()->SaveTo(outFD);
			CCD->CDPlayer()->SaveTo(outFD);
			CCD->Triggers()->SaveTo(outFD, false);
			CCD->Logic()->SaveTo(outFD, false);
// changed RF063
			CCD->Attributes()->SaveTo(outFD, false);
			CCD->Damage()->SaveTo(outFD, false);
			CCD->CameraManager()->SaveTo(outFD);
			CCD->Weapons()->SaveTo(outFD);
			CCD->ElectricEffects()->SaveTo(outFD, false);
			CCD->CountDownTimers()->SaveTo(outFD, false);
			CCD->ChangeAttributes()->SaveTo(outFD, false);
			CCD->Changelevel()->SaveTo(outFD, false);
			CCD->ActMaterials()->SaveTo(outFD, false);
			CCD->ModelManager()->SaveTo(outFD, false);
			CCD->SaveTo(outFD);
// end change RF063

			fclose(outFD);

// changed QD 12/15/05
// add Nout - Show save game image
			if(SaveScreen.Width > 0 && SaveScreen.Height > 0)
			{
				char oldname[256];
				sprintf(oldname, "%s\\SaveScreen.bmp", CCD->GetDirectory(kBitmapFile));
				//sprintf(filename, "Media\\Bitmaps\\SaveScreen%d.bmp",SaveBox.Current);
				//CopyFile(oldname, filename);
				sprintf(filename, "%s\\SaveScreen%d.bmp", CCD->GetDirectory(kBitmapFile), SaveBox.Current);

				if(SaveBox.text[SaveBox.Current].SGImage)
				{
					geEngine_RemoveBitmap(CCD->Engine()->Engine(), SaveBox.text[SaveBox.Current].SGImage);
					geBitmap_Destroy(&(SaveBox.text[SaveBox.Current].SGImage));
				}

				unlink(filename); // delete if there is already a file with this name (old save game image)
				CopyImageFile(oldname, filename);

				{
					sprintf(filename, "SaveScreen%d.bmp", SaveBox.Current);
					SaveBox.text[SaveBox.Current].SGImage = CreateFromFileName(filename);

					if(SaveBox.text[SaveBox.Current].SGImage == (geBitmap*)NULL)
					{
						char szBug[256];
						sprintf(szBug, "*ERROR* SetSlot: Bad file name %s", filename);
						CCD->ReportError(szBug, false);
						exit(-100);
					}

					geEngine_AddBitmap(CCD->Engine()->Engine(), SaveBox.text[SaveBox.Current].SGImage);
				}
			}
// end add Nout
// end change
		}
	}
}

/* ------------------------------------------------------------------------------------ */
// load game from current slot and then run it
/* ------------------------------------------------------------------------------------ */
static void GetSlot()
{
// changed RF064
	if(LoadBox.Current != -1) // must have selected saved game
	{
		// must be visible in table
		if(LoadBox.Current >= LoadBox.Start && LoadBox.Current < (LoadBox.Max_Show+LoadBox.Start))
		{
			char filename[256];

			sprintf(filename, "savgame%d.sav",LoadBox.Current);
			FILE *inFD = CCD->OpenRFFile(kSavegameFile, filename, "rb");

			if(inFD == NULL)
			{
				CCD->ReportError("*WARNING* No savegame file to restore", false);
				return;
			}
// end change RF064

			CCD->MenuManager()->DisplaySplash(); // changed RF064
			CCD->ShutdownLevel();

			if(CCD->CDPlayer())
				CCD->CDPlayer()->Stop();

			if(CCD->MIDIPlayer())
				CCD->MIDIPlayer()->Stop();

			CCD->Engine()->RestoreFrom(inFD);
			CCD->MenuManager()->RestoreFrom(inFD, false);
			CCD->InitializeLevel(CCD->Engine()->LevelName());
			CCD->TerrainMgr()->Init(); // Pickles
			CCD->Player()->RestoreFrom(inFD);
			CCD->TerrainMgr()->Frame(); // Pickles
			CCD->Doors()->RestoreFrom(inFD, false);
			CCD->Platforms()->RestoreFrom(inFD, false);
			CCD->Props()->RestoreFrom(inFD, false);
			// changed QD 02/01/07
			CCD->Meshes()->RestoreFrom(inFD, false);
			// end change
			CCD->Teleporters()->RestoreFrom(inFD, false);
			CCD->MorphingFields()->RestoreFrom(inFD);
			CCD->MIDIPlayer()->RestoreFrom(inFD);
			CCD->CDPlayer()->RestoreFrom(inFD);
			CCD->Triggers()->RestoreFrom(inFD, false);
			CCD->Logic()->RestoreFrom(inFD, false);
			// changed RF063
			CCD->Attributes()->RestoreFrom(inFD, false);
			CCD->Damage()->RestoreFrom(inFD, false);
			CCD->CameraManager()->RestoreFrom(inFD);
			CCD->Weapons()->RestoreFrom(inFD);
			CCD->ElectricEffects()->RestoreFrom(inFD, false);
			CCD->CountDownTimers()->RestoreFrom(inFD, false);
			CCD->ChangeAttributes()->RestoreFrom(inFD, false);
			CCD->Changelevel()->RestoreFrom(inFD, false);
			CCD->ActMaterials()->RestoreFrom(inFD, false);
			CCD->ModelManager()->RestoreFrom(inFD, false);
			CCD->RestoreFrom(inFD);
			// end change RF063

			fclose(inFD);

			// run game
			if(CCD->MenuManager()->GetMusicType() != -1)
			{
				// changed RF063
				CCD->MenuManager()->StopMenuMusic();
				// end change RF063
			}

			CCD->MenuManager()->DisplaySplash(); // changed RF064
			CCD->MenuManager()->GameLoop();
			// set flag for game running
			CCD->MenuManager()->SetInGame();
		}
	}
}

// changed QD 12/15/05
static void CopyImageFile(const char *srcPath, const char *destPath)
{
	FREE_IMAGE_FORMAT fif = FreeImage_GetFIFFromFilename(srcPath);
	if(fif >= 0)
	{
		FIBITMAP *Fbmp = FreeImage_Load(fif, srcPath, 0);
		FreeImage_Save(fif, Fbmp, destPath, 0);
	}
}

/*
// add Nout - Show save game image
static void CopyFile(const char *srcPath, const char *destPath)
{
	CFile sourceFile;
	CFile destFile;
	BYTE buffer[16384];
	DWORD dwRead;

	CFileException ex;
	sourceFile.Open(srcPath, CFile::modeRead | CFile::shareDenyWrite, &ex);
	destFile.Open(destPath, CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate, &ex);

	do
	{
		dwRead = sourceFile.Read(buffer, 16384);
		destFile.Write(buffer, dwRead);
	} while (dwRead > 0);

	destFile.Close();
	sourceFile.Close();
}
*/
// end change

/* ------------------------------------------------------------------------------------ */
//	rescales a bitmap file
/* ------------------------------------------------------------------------------------ */
void ScaleBitmapFromFile(const char *FileName, int dst_width, int dst_height)
{
	FREE_IMAGE_FORMAT fif = FreeImage_GetFIFFromFilename(FileName);
	FREE_IMAGE_FILTER myfilter = FILTER_BOX;

	if(fif >= 0)
	{
		FIBITMAP *Fbmp = FreeImage_Load(fif, FileName, 0);
		Fbmp = FreeImage_ConvertTo32Bits(Fbmp);
		Fbmp = FreeImage_Rescale(Fbmp, dst_width, dst_height, myfilter);
		Fbmp = FreeImage_ConvertTo24Bits(Fbmp);
		FreeImage_Save(fif, Fbmp, FileName, 0);
		FreeImage_Unload(Fbmp);
	}
}

/*
static void ScaleBmp(short x,short y, short x2, short y2)
{
	float xstep,ystep,xratio,yratio;
	short xdiff,ydiff,xcount,ycount;
	long VStart= vid->Screen_Width*y2;

	ydiff = y2-y;
	xdiff = x2-x;
	xratio = (float)BitmapWidth/xdiff;
	yratio = (float)BitmapHeight/ydiff;

	for(ystep=0,ycount=y;   ycount<ydiff; ystep+=yratio,ycount++)
	{
		for(xstep=0,xcount=x;xcount<xdiff; xstep+=xratio,xcount++)
		{
			video_buffer[(y2-ycount)*Screen_Width+xcount]=
			Bmp[(short)ystep*(short)BitmapWidth+(short)xstep];
		}
	}
}
*/
//end add Nout

/* ------------------------------------------------------------------------------------ */
//	PowerOfTwo
/* ------------------------------------------------------------------------------------ */
static int PowerOfTwo(int value)
{
	if(value>128)
		return 256;
	if(value>64)
		return 128;
	if(value>32)
		return 64;
	if(value>16)
		return 32;
	if(value>8)
		return 16;
	return 8;
}

/* ------------------------------------------------------------------------------------ */
//	LoadWBitmap
/* ------------------------------------------------------------------------------------ */
void CRFMenu::LoadWBitmap()
{
	geBitmap *Alpha, *BAlpha;

	for(int i=0; i<NUM_FONTS; i++)
	{
		if(MenuFont[i].Bitmap != NULL)
		{
			BAlpha = geBitmap_GetAlpha(MenuFont[i].Bitmap);

			for(int chr=0; chr<96 ; chr++)
			{
				int w = PowerOfTwo(MenuFont[i].dat[chr].width);
				int h = PowerOfTwo(MenuFont[i].font_height);

				MenuFont[i].WBitmap[chr] = geBitmap_Create(w, h, 0, GE_PIXELFORMAT_16BIT_565_RGB);//GE_PIXELFORMAT_8BIT);

				geBitmap_Blit(MenuFont[i].Bitmap, MenuFont[i].dat[chr].x,
					MenuFont[i].dat[chr].y, MenuFont[i].WBitmap[chr], 0, 0,
					MenuFont[i].dat[chr].width, MenuFont[i].font_height);

				Alpha = geBitmap_Create(w, h, 0, GE_PIXELFORMAT_16BIT_565_RGB);

				geBitmap_Blit(BAlpha, MenuFont[i].dat[chr].x,
					MenuFont[i].dat[chr].y, Alpha, 0, 0,
					MenuFont[i].dat[chr].width, MenuFont[i].font_height);

				geWorld_AddBitmap(CCD->World(), MenuFont[i].WBitmap[chr]);
				geBitmap_SetPreferredFormat(MenuFont[i].WBitmap[chr],GE_PIXELFORMAT_32BIT_ARGB);
				geBitmap_SetAlpha(MenuFont[i].WBitmap[chr], Alpha);
				geBitmap_Destroy(&Alpha);
			}
		}
	}

	fontloaded = true;
}

/* ------------------------------------------------------------------------------------ */
//	UnLoadWBitmap
/* ------------------------------------------------------------------------------------ */
void CRFMenu::UnLoadWBitmap()
{
	if(!fontloaded)
		return;

	for(int i=0; i<NUM_FONTS; i++)
	{
		if(MenuFont[i].Bitmap != NULL)
		{
			for(int j=0; j<96; j++)
			{
				if(MenuFont[i].WBitmap[j])
				{
					geWorld_RemoveBitmap(CCD->World(), MenuFont[i].WBitmap[j]);
					geBitmap_Destroy(&MenuFont[i].WBitmap[j]);
				}
			}
		}
	}

	fontloaded = false;
}

/* ------------------------------------------------------------------------------------ */
//	WorldFontRect
/* ------------------------------------------------------------------------------------ */
void CRFMenu::WorldFontRect(char *s, int FontNumber, int x, int y, float Alpha)
{
	int charoff;
	char chr;

	if(MenuFont[FontNumber].Bitmap == NULL)
	{
		char szBug[256];
		sprintf(szBug, "*WARNING* File %s - Line %d: No defined Font # %d (FONT%d in Menu.ini)",
				__FILE__, __LINE__, FontNumber, FontNumber+1);
		CCD->ReportError(szBug, false);
		// changed QD 07/15/06
		return;
		/*
		sprintf(szBug, "ERROR - File %s - Line %d: No Font defined", __FILE__, __LINE__);
		CCD->ReportError(szBug, true);
		CCD->ShutdownLevel();
		delete CCD;
		CCD = NULL;
		exit(-336);
		*/
		// end change
	}

	if(s != NULL)
	{
		charoff = 0;

		while(*s != 0)
		{
			chr = *s-32;

			fRect.Top		= 0;
			fRect.Bottom	= MenuFont[FontNumber].font_height-1;
			fRect.Left		= 0;
			fRect.Right		= MenuFont[FontNumber].dat[chr].width-1;

			CCD->Engine()->DrawBitmap(MenuFont[FontNumber].WBitmap[chr], &fRect, x+charoff, y, Alpha, 1.0f);

			charoff += MenuFont[FontNumber].dat[chr].width;
			s++;
		}
	}
}

/* ------------------------------------------------------------------------------------ */
//	FadeSet
/* ------------------------------------------------------------------------------------ */
void CRFMenu::FadeSet(int Dir, float Time)
{
	Fading = false;

	if(Time > 0)
		Fading = true;

	FadeDir = Dir;

	if(FadeDir == -1)
		FadeAlpha = 255.0f;
	else
		FadeAlpha = 0.0f;

	CurrentFadeTime = CCD->FreeRunningCounter_F();
	FadeTime = Time;
}

/* ------------------------------------------------------------------------------------ */
//	DoFade
/* ------------------------------------------------------------------------------------ */
void CRFMenu::DoFade()
{
	float FadeDelta, VolDelta;
	float DeltaTime = (CCD->FreeRunningCounter_F() - CurrentFadeTime)*0.001f;

	if(FadeDir == -1)
	{
		FadeDelta = 255.0f - (255.0f * (DeltaTime/FadeTime));

		if(MusicFade)
		{
			VolDelta = OldMVol*(DeltaTime/FadeTime);

			if(VolDelta > OldMVol)
				VolDelta = OldMVol;
		}
	}
	else
	{
		FadeDelta = 255.0f * (DeltaTime/FadeTime);

		if(MusicFade)
		{
			VolDelta = OldMVol - (OldMVol*(DeltaTime/FadeTime));

			if(VolDelta < 0)
				VolDelta = 0;
		}
	}

	if(Fading)
	{
		FadeAlpha = FadeDelta;

		if(FadeAlpha >= 0.0f)
		{
			GE_Rect Rect;
			GE_RGBA	Color;
			Rect.Left = Rect.Top = 0;
			Rect.Right = CCD->Engine()->Width() - 1;
			Rect.Bottom = CCD->Engine()->Height() - 1;

			Color.r = 0.0f;
			Color.g = 0.0f;
			Color.b = 0.0f;
			Color.a = FadeAlpha;

			if(Color.a < 0.0f)
				Color.a = 0.0f;
			else if(Color.a > 255.0f)
				Color.a = 255.0f;

			geEngine_FillRect(CCD->Engine()->Engine(), &Rect, &Color);
		}

		if(MusicFade)
		{
			SetmMusicVol(VolDelta);
		}

		if(FadeDir == -1 && FadeAlpha < 0.0f)
			Fading = false;
		else if(FadeDir == 1 && FadeAlpha > 255.0f)
			Fading = false;

		if(!Fading && MusicFade)
		{
			MusicFade = false;
		}
	}
}

/* ------------------------------------------------------------------------------------ */
//	MusicSet
/* ------------------------------------------------------------------------------------ */
void CRFMenu::MusicSet()
{
	//MusicFade = true;
	//OldMVol = mVolLevel;
}

/* ------------------------------------------------------------------------------------ */
//	SaveTo
//
//	Save the current state of the menu off to an open file.
/* ------------------------------------------------------------------------------------ */
int CRFMenu::SaveTo(FILE *SaveFD, bool type)
{

	WRITEDATA(type, &useselect,		sizeof(bool),	1, SaveFD);
	WRITEDATA(type, &CurrentSelect, sizeof(int),	1, SaveFD);
// changed QD 12/15/05
	WRITEDATA(type, &usenameselect, sizeof(bool),	1, SaveFD);
// end change

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	RestoreFrom
//
//	Restore the state of the menu from an open file.
/* ------------------------------------------------------------------------------------ */
int CRFMenu::RestoreFrom(FILE *RestoreFD, bool type)
{
	READDATA(type, &useselect,		sizeof(bool),	1, RestoreFD);
	READDATA(type, &CurrentSelect,	sizeof(int),	1, RestoreFD);
// changed QD 12/15/05
	READDATA(type, &usenameselect,	sizeof(bool),	1, RestoreFD);
// end change

	return RGF_SUCCESS;
}

// change QD Language Menu
/* ------------------------------------------------------------------------------------ */
//	ChangeMenuIni
/* ------------------------------------------------------------------------------------ */
void CRFMenu::ChangeMenuIni()
{
	int i, l;

	//**********************************************************
	l = CCD->GetLanguage();

	if(CurrentLanguage == l)
		return;

	if(!stricmp(MenuInis[l], MenuInis[CurrentLanguage]))
		return;
	//**********************************************************

	// stop music
	if(musictype != -1)
	{
		if(musictype == 1)
		{
			MIDIPlayer()->Stop();
		}
		else
		{
			if(m_Streams)
			{
				m_Streams->Delete();
				delete m_Streams;
				m_Streams = 0;
			}
		}
	}

	// free click sounds
	geSound_FreeSoundDef(CCD->Engine()->AudioSystem(), mouseclick );
	geSound_FreeSoundDef(CCD->Engine()->AudioSystem(), keyclick );
	geSound_FreeSoundDef(CCD->Engine()->AudioSystem(), slideclick );

	// delete the bitmaps
	for(i=0; i<NUM_BACKGROUNDS; i++)
	{
		if(Backgrounds[i] != (geBitmap*)NULL)
		{
			geEngine_RemoveBitmap(CCD->Engine()->Engine(), Backgrounds[i]);
			geBitmap_Destroy(&Backgrounds[i]);
		}
	}

	for(i=0; i<NUM_IMAGES; i++)
	{
		if(Images[i] != (geBitmap*)NULL)
		{
			geEngine_RemoveBitmap(CCD->Engine()->Engine(), Images[i]);
			geBitmap_Destroy(&Images[i]);
		}
	}

	for(i=0; i<NUM_TITLES; i++)
	{
		if(Titles[i] != (geBitmap*)NULL)
		{
			geEngine_RemoveBitmap(CCD->Engine()->Engine(), Titles[i]);
			geBitmap_Destroy(&Titles[i]);
		}
	}

	if(ingame)
		UnLoadWBitmap();

	for(i=0; i<NUM_FONTS; i++)
	{
		if(MenuFont[i].Bitmap != (geBitmap*)NULL)
		{
			geEngine_RemoveBitmap(CCD->Engine()->Engine(), MenuFont[i].Bitmap);
			geBitmap_Destroy(&MenuFont[i].Bitmap);
		}
	}

	for(i=0; i<NUM_ANIM; i++)
	{
		if(Animation[i] != NULL)
			delete Animation[i];
	}

// changed QD 12/15/05
	if(EmptySlotImage)
	{
		geEngine_RemoveBitmap(CCD->Engine()->Engine(), EmptySlotImage);
		geBitmap_Destroy(&EmptySlotImage);
	}
// end change

	if(Cursor != (geBitmap*)NULL)
	{
		geEngine_RemoveBitmap(CCD->Engine()->Engine(), Cursor);
		geBitmap_Destroy(&Cursor);
	}

	if(Crosshair != (geBitmap*)NULL)
	{
		geBitmap_Destroy(&Crosshair);
	}

	if(FCrosshair != (geBitmap*)NULL)
	{
		geEngine_RemoveBitmap(CCD->Engine()->Engine(), FCrosshair);
		geBitmap_Destroy(&FCrosshair);
	}

	Cursor = NULL;
	Crosshair = NULL;
	FCrosshair = NULL;


	Reset();

	// changed QD 12/15/05 - default is a better indicator ;-)
	if(!stricmp("standard", MenuInis[l]) || !stricmp("default", MenuInis[l]))
		LoadMenuIni(CCD->MenuIni());
	else
		LoadMenuIni(MenuInis[l]);


	if(ingame)
		LoadWBitmap();

	if(musictype != -1)
	{
		if(musictype == 1)
			MIDIPlayer()->Play(music, true);
		else
			m_Streams->Play(true);
	}


	if(CCD->Pawns() != NULL)
	{
		// changed QD 12/15/05 - default is a better indicator ;-)
		if(!stricmp("standard", Convtxts[l]) || !stricmp("default", Convtxts[l]))
			CCD->Pawns()->LoadConv("conversation.txt");
		else
			CCD->Pawns()->LoadConv(Convtxts[l]);
	}

	if(CCD->Messages() != NULL)
	{
		// changed QD 12/15/05 - default is a better indicator ;-)
		if(!stricmp("standard", Messagetxts[l]) || !stricmp("default", Messagetxts[l]))
			CCD->Messages()->LoadText("message.txt");
		else
			CCD->Messages()->LoadText(Messagetxts[l]);
	}

	CurrentLanguage = l;
}

/* ------------------------------------------------------------------------------------ */
//	GetConvtxts
/* ------------------------------------------------------------------------------------ */
char *CRFMenu::GetConvtxts()
{
// changed QD 12/15/05 make it a static string
	static char defaultconvtxt[] = "conversation.txt";

	// changed QD 12/15/05 - default is a better indicator ;-)
	if(!stricmp("standard", Convtxts[CCD->GetLanguage()]) ||
		!stricmp("default", Convtxts[CCD->GetLanguage()]))
		return defaultconvtxt;// "conversation.txt";
// end change

	return Convtxts[CCD->GetLanguage()];
}

/* ------------------------------------------------------------------------------------ */
//	GetMessagetxts
/* ------------------------------------------------------------------------------------ */
char *CRFMenu::GetMessagetxts()
{
// changed QD 12/15/05 - make it a static string
	static char defaultmessagetxt[] = "message.txt";

	// changed QD 12/15/05 - default is a better indicator ;-)
	if(!stricmp("standard", Messagetxts[CCD->GetLanguage()]) ||
		!stricmp("default", Messagetxts[CCD->GetLanguage()]))
		return defaultmessagetxt;// "message.txt";
// end change

	return Messagetxts[CCD->GetLanguage()];
}

/* ------------------------------------------------------------------------------------ */
//	ChangeMenu
/* ------------------------------------------------------------------------------------ */
void ChangeMenu()
{
	if(L1text.text)
		free(L1text.text);
	if(L2text.text)
		free(L2text.text);
	if(L3text.text)
		free(L3text.text);
	if(L4text.text)
		free(L4text.text);
	if(L5text.text)
		free(L5text.text);

	L1text.text = NULL;
	L2text.text = NULL;
	L3text.text = NULL;
	L4text.text = NULL;
	L5text.text = NULL;

	CCD->MenuManager()->ChangeMenuIni();
}

/* ------------------------------------------------------------------------------------ */
//	LanguageRadio
/* ------------------------------------------------------------------------------------ */
void LanguageRadio(int ID)
{
	CCD->SetLanguage(ID);
}

/* ------------------------------------------------------------------------------------ */
//	Reset
//
//	reset menu to default values
/* ------------------------------------------------------------------------------------ */
void CRFMenu::Reset()
{
	int i;

	for(i=0;i<NUM_TITLES;i++)
	{
		MTitles[i].Image_Number = 0;
		MTitles[i].X			= 0;
		MTitles[i].Y			= 0;
		MTitles[i].Width		= 0;
		MTitles[i].Height		= 0;
		MTitles[i].Image_X		= 0;
		MTitles[i].Image_Y		= 0;
		MTitles[i].Animation	= -1;
	}

	for(i=0; i<8; i++)
	{
		MainMenu[i].X = 0;
		MainMenu[i].Y = 0;
	}

	for(i=0; i<4; i++)
	{
		LoadGMenu[i].X	= 0;
		LoadGMenu[i].Y	= 0;
		SaveMenu[i].X	= 0;
		SaveMenu[i].Y	= 0;
// changed QD 12/15/05
		PlayerNameMenu[i].X = 0;
		PlayerNameMenu[i].Y = 0;
// end change
	}

	for(i=0; i<5; i++)
	{
		MultiMenu[i].X		= 0;
		MultiMenu[i].Y		= 0;
		JoinGameMenu[i].X	= 0;
		JoinGameMenu[i].Y	= 0;
		ControlMenu[i].X	= 0;
		ControlMenu[i].Y	= 0;
		SelectMenu[i].X		= 0;
		SelectMenu[i].Y		= 0;
	}

	for(i=0; i<6; i++)
	{
		OptionMenu[i].X = 0;
		OptionMenu[i].Y = 0;
	}

	for(i=0; i<11; i++)
	{
		LanguageMenu[i].X = 0;
		LanguageMenu[i].Y = 0;
		DebugMenu[i].X = 0;
		DebugMenu[i].Y = 0;
	}



	CreditMenu[0].X = 0;
	CreditMenu[0].Y = 0;
	CreditMenu[1].X = 0;
	CreditMenu[1].Y = 0;

	ModsMenu[0].X = 0;
	ModsMenu[0].Y = 0;

	for(i=0; i<9; i++)
	{
		AudioMenu[i].X = 0;
		AudioMenu[i].Y = 0;
	}

// changed QD Shadows
	for(i=0; i<9; i++)
	{
		VideoMenu[i].X = 0;
		VideoMenu[i].Y = 0;
	}

	for(i=0; i<10; i++)
	{
		AdvancedMenu[i].X = 0;
		AdvancedMenu[i].Y = 0;
	}

	for(i=0; i<3; i++)
	{
		DifficultMenu[i].X = 0;
		DifficultMenu[i].Y = 0;
	}

	MainBack = 0;
	MainTitle = 0;

	NewGame.Image_Number	= 0;
	NewGame.Width			= 0;
	NewGame.Height			= 0;
	NewGame.Image_X			= 0;
	NewGame.Image_Y			= 0;
	NewGame.Mover_X			= 0;
	NewGame.Mover_Y			= 0;
	NewGame.Animation		= -1;
	NewGame.AnimationOver	= -1;

	MultiPlayer.Image_Number	= 0;
	MultiPlayer.Width			= 0;
	MultiPlayer.Height			= 0;
	MultiPlayer.Image_X			= 0;
	MultiPlayer.Image_Y			= 0;
	MultiPlayer.Mover_X			= 0;
	MultiPlayer.Mover_Y			= 0;
	MultiPlayer.background		= 0;
	MultiPlayer.title			= 0;
	MultiPlayer.Animation		= -1;
	MultiPlayer.AnimationOver	= -1;

	LoadGame.Image_Number	= 0;
	LoadGame.Width			= 0;
	LoadGame.Height			= 0;
	LoadGame.Image_X		= 0;
	LoadGame.Image_Y		= 0;
	LoadGame.Mover_X		= 0;
	LoadGame.Mover_Y		= 0;
	LoadGame.background		= 0;
	LoadGame.title			= 0;
	LoadGame.Animation		= -1;
	LoadGame.AnimationOver	= -1;

	SaveGame.Image_Number	= 0;
	SaveGame.Width			= 0;
	SaveGame.Height			= 0;
	SaveGame.Image_X		= 0;
	SaveGame.Image_Y		= 0;
	SaveGame.Mover_X		= 0;
	SaveGame.Mover_Y		= 0;
	SaveGame.background		= 0;
	SaveGame.title			= 0;
	SaveGame.Animation		= -1;
	SaveGame.AnimationOver	= -1;

// start change Nout - Show save game image
	SaveScreen.X		= 0;
	SaveScreen.Y		= 0;
	SaveScreen.Width	= 0;
	SaveScreen.Height	= 0;
	SaveScreen.Image_X	= 0;
	SaveScreen.Image_Y	= 0;
// end change Nout

	Options.Image_Number	= 0;
	Options.Width			= 0;
	Options.Height			= 0;
	Options.Image_X			= 0;
	Options.Image_Y			= 0;
	Options.Mover_X			= 0;
	Options.Mover_Y			= 0;
	Options.background		= 0;
	Options.title			= 0;
	Options.Animation		= -1;
	Options.AnimationOver	= -1;

	Credits.Image_Number	= 0;
	Credits.Width			= 0;
	Credits.Height			= 0;
	Credits.Image_X			= 0;
	Credits.Image_Y			= 0;
	Credits.Mover_X			= 0;
	Credits.Mover_Y			= 0;
	Credits.background		= 0;
	Credits.title			= 0;
	Credits.Animation		= -1;
	Credits.AnimationOver	= -1;

	Mods.Image_Number	= 0;
	Mods.Width			= 0;
	Mods.Height			= 0;
	Mods.Image_X		= 0;
	Mods.Image_Y		= 0;
	Mods.Mover_X		= 0;
	Mods.Mover_Y		= 0;
	Mods.background		= 0;
	Mods.title			= 0;
	Mods.Animation		= -1;
	Mods.AnimationOver	= -1;

	QuitGame.Image_Number	= 0;
	QuitGame.Width			= 0;
	QuitGame.Height			= 0;
	QuitGame.Image_X		= 0;
	QuitGame.Image_Y		= 0;
	QuitGame.Mover_X		= 0;
	QuitGame.Mover_Y		= 0;
	QuitGame.Animation		= -1;
	QuitGame.AnimationOver	= -1;

	return_text.Font = 0;

// start multiplayer
	HostNewGame.Image_Number	= 0;
	HostNewGame.Width			= 0;
	HostNewGame.Height			= 0;
	HostNewGame.Image_X			= 0;
	HostNewGame.Image_Y			= 0;
	HostNewGame.Mover_X			= 0;
	HostNewGame.Mover_Y			= 0;
	HostNewGame.background		= 0;
	HostNewGame.title			= 0;
	HostNewGame.Animation		= -1;
	HostNewGame.AnimationOver	= -1;

	JoinMultiplayerGame.Image_Number	= 0;
	JoinMultiplayerGame.Width			= 0;
	JoinMultiplayerGame.Height			= 0;
	JoinMultiplayerGame.Image_X			= 0;
	JoinMultiplayerGame.Image_Y			= 0;
	JoinMultiplayerGame.Mover_X			= 0;
	JoinMultiplayerGame.Mover_Y			= 0;
	JoinMultiplayerGame.background		= 0;
	JoinMultiplayerGame.title			= 0;
	JoinMultiplayerGame.Animation		= -1;
	JoinMultiplayerGame.AnimationOver	= -1;

	QuitMulti.Image_Number	= 0;
	QuitMulti.Width			= 0;
	QuitMulti.Height		= 0;
	QuitMulti.Image_X		= 0;
	QuitMulti.Image_Y		= 0;
	QuitMulti.Mover_X		= 0;
	QuitMulti.Mover_Y		= 0;
	QuitMulti.Animation		= -1;
	QuitMulti.AnimationOver = -1;

	PlayerIP_Text.Font = 0;
	PlayerIP1.Font = 0;

	IPAdd_Text.Font = 0;

	IPAddress.Font	= 0;
	IPAddress.Set_X = 10;
	IPAddress.Set_Y = 10;
	IPAddress.Width = 100;

	LaunchJoinGame.Image_Number		= 0;
	LaunchJoinGame.Width			= 0;
	LaunchJoinGame.Height			= 0;
	LaunchJoinGame.Image_X			= 0;
	LaunchJoinGame.Image_Y			= 0;
	LaunchJoinGame.Mover_X			= 0;
	LaunchJoinGame.Mover_Y			= 0;
	LaunchJoinGame.Animation		= -1;
	LaunchJoinGame.AnimationOver	= -1;

	QuitJoinGame.Image_Number	= 0;
	QuitJoinGame.Width			= 0;
	QuitJoinGame.Height			= 0;
	QuitJoinGame.Image_X		= 0;
	QuitJoinGame.Image_Y		= 0;
	QuitJoinGame.Mover_X		= 0;
	QuitJoinGame.Mover_Y		= 0;
	QuitJoinGame.Animation		= -1;
	QuitJoinGame.AnimationOver	= -1;

	MultiplayerHelp_Text.Font = 0;

	QuitLoad.Image_Number	= 0;
	QuitLoad.Width			= 0;
	QuitLoad.Height			= 0;
	QuitLoad.Image_X		= 0;
	QuitLoad.Image_Y		= 0;
	QuitLoad.Mover_X		= 0;
	QuitLoad.Mover_Y		= 0;
	QuitLoad.Animation		= -1;
	QuitLoad.AnimationOver	= -1;

	LoadSlot.Image_Number	= 0;
	LoadSlot.Width			= 0;
	LoadSlot.Height			= 0;
	LoadSlot.Image_X		= 0;
	LoadSlot.Image_Y		= 0;
	LoadSlot.Mover_X		= 0;
	LoadSlot.Mover_Y		= 0;
	LoadSlot.Animation		= 0;
	LoadSlot.AnimationOver	= 0;

	LoadBox.Image_Number	= 0;
	LoadBox.Width			= 0;
	LoadBox.Height			= 0;
	LoadBox.Image_X			= 0;
	LoadBox.Image_Y			= 0;
	LoadBox.Start_X			= 0;
	LoadBox.Start_Y			= 0;
	LoadBox.Step			= 0;
	LoadBox.Max_Show		= 0;
	LoadBox.Corner_X		= 0;
	LoadBox.Corner_Y		= 0;
	LoadBox.Click_Width		= 0;
	LoadBox.Rev_X			= 0;
	LoadBox.Rev_Y			= 0;
	LoadBox.Rev_Width		= 0;
	LoadBox.Rev_Height		= 0;
	LoadBox.Font			= 0;

	LoadBar.Image_Number	= 0;
	LoadBar.Up_Width		= 0;
	LoadBar.Up_Height		= 0;
	LoadBar.Up_Nor_X		= 0;
	LoadBar.Up_Nor_Y		= 0;
	LoadBar.Up_Lit_X		= 0;
	LoadBar.Up_Lit_Y		= 0;
	LoadBar.Up_Push_X		= 0;
	LoadBar.Up_Push_Y		= 0;
	LoadBar.Dwn_Width		= 0;
	LoadBar.Dwn_Height		= 0;
	LoadBar.Dwn_Nor_X		= 0;
	LoadBar.Dwn_Nor_Y		= 0;
	LoadBar.Dwn_Lit_X		= 0;
	LoadBar.Dwn_Lit_Y		= 0;
	LoadBar.Dwn_Push_X		= 0;
	LoadBar.Dwn_Push_Y		= 0;
	LoadBar.Up_X			= 0;
	LoadBar.Up_Y			= 0;
	LoadBar.Dwn_X			= 0;
	LoadBar.Dwn_Y			= 0;
	LoadBar.Show			= 0;
	LoadBar.AnimationUp		= -1;
	LoadBar.AnimationUpOver = -1;
	LoadBar.AnimationUpPush = -1;
	LoadBar.AnimationDwn	= -1;
	LoadBar.AnimationDwnOver= -1;
	LoadBar.AnimationDwnPush= -1;

	QuitSave.Image_Number	= 0;
	QuitSave.Width			= 0;
	QuitSave.Height			= 0;
	QuitSave.Image_X		= 0;
	QuitSave.Image_Y		= 0;
	QuitSave.Mover_X		= 0;
	QuitSave.Mover_Y		= 0;
	QuitSave.Animation		= -1;
	QuitSave.AnimationOver	= -1;

	SaveSlot.Image_Number	= 0;
	SaveSlot.Width			= 0;
	SaveSlot.Height			= 0;
	SaveSlot.Image_X		= 0;
	SaveSlot.Image_Y		= 0;
	SaveSlot.Mover_X		= 0;
	SaveSlot.Mover_Y		= 0;
	SaveSlot.Animation		= -1;
	SaveSlot.AnimationOver	= -1;

	SaveBox.Image_Number	= 0;
	SaveBox.Width			= 0;
	SaveBox.Height			= 0;
	SaveBox.Image_X			= 0;
	SaveBox.Image_Y			= 0;
	SaveBox.Start_X			= 0;
	SaveBox.Start_Y			= 0;
	SaveBox.Step			= 0;
	SaveBox.Max_Show		= 0;
	SaveBox.Corner_X		= 0;
	SaveBox.Corner_Y		= 0;
	SaveBox.Click_Width		= 0;
	SaveBox.Rev_X			= 0;
	SaveBox.Rev_Y			= 0;
	SaveBox.Rev_Width		= 0;
	SaveBox.Rev_Height		= 0;
	SaveBox.Font			= 0;

	SaveBar.Image_Number	= 0;
	SaveBar.Up_Width		= 0;
	SaveBar.Up_Height		= 0;
	SaveBar.Up_Nor_X		= 0;
	SaveBar.Up_Nor_Y		= 0;
	SaveBar.Up_Lit_X		= 0;
	SaveBar.Up_Lit_Y		= 0;
	SaveBar.Up_Push_X		= 0;
	SaveBar.Up_Push_Y		= 0;
	SaveBar.Dwn_Width		= 0;
	SaveBar.Dwn_Height		= 0;
	SaveBar.Dwn_Nor_X		= 0;
	SaveBar.Dwn_Nor_Y		= 0;
	SaveBar.Dwn_Lit_X		= 0;
	SaveBar.Dwn_Lit_Y		= 0;
	SaveBar.Dwn_Push_X		= 0;
	SaveBar.Dwn_Push_Y		= 0;
	SaveBar.Up_X			= 0;
	SaveBar.Up_Y			= 0;
	SaveBar.Dwn_X			= 0;
	SaveBar.Dwn_Y			= 0;
	SaveBar.Show			= 0;
	SaveBar.AnimationUp		= -1;
	SaveBar.AnimationUpOver = -1;
	SaveBar.AnimationUpPush = -1;
	SaveBar.AnimationDwn	= -1;
	SaveBar.AnimationDwnOver= -1;
	SaveBar.AnimationDwnPush= -1;

	AudioItem.Image_Number	= 0;
	AudioItem.Width			= 0;
	AudioItem.Height		= 0;
	AudioItem.Image_X		= 0;
	AudioItem.Image_Y		= 0;
	AudioItem.Mover_X		= 0;
	AudioItem.Mover_Y		= 0;
	AudioItem.background	= 0;
	AudioItem.title			= 0;
	AudioItem.Animation		= -1;
	AudioItem.AnimationOver = -1;

	VideoItem.Image_Number	= 0;
	VideoItem.Width			= 0;
	VideoItem.Height		= 0;
	VideoItem.Image_X		= 0;
	VideoItem.Image_Y		= 0;
	VideoItem.Mover_X		= 0;
	VideoItem.Mover_Y		= 0;
	VideoItem.background	= 0;
	VideoItem.title			= 0;
	VideoItem.Animation		= -1;
	VideoItem.AnimationOver = -1;

	ControlItem.Image_Number	= 0;
	ControlItem.Width			= 0;
	ControlItem.Height			= 0;
	ControlItem.Image_X			= 0;
	ControlItem.Image_Y			= 0;
	ControlItem.Mover_X			= 0;
	ControlItem.Mover_Y			= 0;
	ControlItem.background		= 0;
	ControlItem.title			= 0;
	ControlItem.Animation		= -1;
	ControlItem.AnimationOver	= -1;

	LanguageItem.Image_Number	= 0;
	LanguageItem.Width			= 0;
	LanguageItem.Height			= 0;
	LanguageItem.Image_X		= 0;
	LanguageItem.Image_Y		= 0;
	LanguageItem.Mover_X		= 0;
	LanguageItem.Mover_Y		= 0;
	LanguageItem.background		= 0;
	LanguageItem.title			= 0;
	LanguageItem.Animation		= -1;
	LanguageItem.AnimationOver	= -1;

	DebugItem.Image_Number	= 0;
	DebugItem.Width			= 0;
	DebugItem.Height		= 0;
	DebugItem.Image_X		= 0;
	DebugItem.Image_Y		= 0;
	DebugItem.Mover_X		= 0;
	DebugItem.Mover_Y		= 0;
	DebugItem.background	= 0;
	DebugItem.title			= 0;
	DebugItem.Animation		= -1;
	DebugItem.AnimationOver = -1;

	QuitOption.Image_Number		= 0;
	QuitOption.Width			= 0;
	QuitOption.Height			= 0;
	QuitOption.Image_X			= 0;
	QuitOption.Image_Y			= 0;
	QuitOption.Mover_X			= 0;
	QuitOption.Mover_Y			= 0;
	QuitOption.Animation		= -1;
	QuitOption.AnimationOver	= -1;

	QuitLanguage.Image_Number	= 0;
	QuitLanguage.Width			= 0;
	QuitLanguage.Height			= 0;
	QuitLanguage.Image_X		= 0;
	QuitLanguage.Image_Y		= 0;
	QuitLanguage.Mover_X		= 0;
	QuitLanguage.Mover_Y		= 0;
	QuitLanguage.Animation		= -1;
	QuitLanguage.AnimationOver	= -1;

	boxL1.Image_Number	= 0;
	boxL1.Width			= 0;
	boxL1.Height		= 0;
	boxL1.Image_X		= 0;
	boxL1.Image_Y		= 0;
	boxL1.Mover_X		= 0;
	boxL1.Mover_Y		= 0;
	boxL1.Set_X			= 0;
	boxL1.Set_Y			= 0;
	boxL1.Animation		= -1;
	boxL1.AnimationOver = -1;
	boxL1.AnimationLit	= -1;

	L1text.Font = -1;

	boxL2.Image_Number	= 0;
	boxL2.Width			= 0;
	boxL2.Height		= 0;
	boxL2.Image_X		= 0;
	boxL2.Image_Y		= 0;
	boxL2.Mover_X		= 0;
	boxL2.Mover_Y		= 0;
	boxL2.Set_X			= 0;
	boxL2.Set_Y			= 0;
	boxL2.Animation		= -1;
	boxL2.AnimationOver = -1;
	boxL2.AnimationLit	= -1;

	L2text.Font = -1;

	boxL3.Image_Number	= 0;
	boxL3.Width			= 0;
	boxL3.Height		= 0;
	boxL3.Image_X		= 0;
	boxL3.Image_Y		= 0;
	boxL3.Mover_X		= 0;
	boxL3.Mover_Y		= 0;
	boxL3.Set_X			= 0;
	boxL3.Set_Y			= 0;
	boxL3.Animation		= -1;
	boxL3.AnimationOver = -1;
	boxL3.AnimationLit	= -1;

	L3text.Font = -1;

	boxL4.Image_Number	= 0;
	boxL4.Width			= 0;
	boxL4.Height		= 0;
	boxL4.Image_X		= 0;
	boxL4.Image_Y		= 0;
	boxL4.Mover_X		= 0;
	boxL4.Mover_Y		= 0;
	boxL4.Set_X			= 0;
	boxL4.Set_Y			= 0;
	boxL4.Animation		= -1;
	boxL4.AnimationOver = -1;
	boxL4.AnimationLit	= -1;

	L4text.Font = -1;

	boxL5.Image_Number	= 0;
	boxL5.Width			= 0;
	boxL5.Height		= 0;
	boxL5.Image_X		= 0;
	boxL5.Image_Y		= 0;
	boxL5.Mover_X		= 0;
	boxL5.Mover_Y		= 0;
	boxL5.Set_X			= 0;
	boxL5.Set_Y			= 0;
	boxL5.Animation		= -1;
	boxL5.AnimationOver = -1;
	boxL5.AnimationLit	= -1;

	L5text.Font = -1;

	QuitDebug.Image_Number	= 0;
	QuitDebug.Width			= 0;
	QuitDebug.Height		= 0;
	QuitDebug.Image_X		= 0;
	QuitDebug.Image_Y		= 0;
	QuitDebug.Mover_X		= 0;
	QuitDebug.Mover_Y		= 0;
	QuitDebug.Animation		= -1;
	QuitDebug.AnimationOver = -1;

	box5.Image_Number	= 0;
	box5.Width			= 0;
	box5.Height			= 0;
	box5.Image_X		= 0;
	box5.Image_Y		= 0;
	box5.Mover_X		= 0;
	box5.Mover_Y		= 0;
	box5.Set_X			= 0;
	box5.Set_Y			= 0;
	box5.Animation		= -1;
	box5.AnimationOver	= -1;
	box5.AnimationLit	= -1;

	debug_text.Font = 0;

	box6.Image_Number	= 0;
	box6.Width			= 0;
	box6.Height			= 0;
	box6.Image_X		= 0;
	box6.Image_Y		= 0;
	box6.Mover_X		= 0;
	box6.Mover_Y		= 0;
	box6.Set_X			= 0;
	box6.Set_Y			= 0;
	box6.Animation		= -1;
	box6.AnimationOver	= -1;
	box6.AnimationLit	= -1;

	fps_text.Font = 0;

	box7.Image_Number	= 0;
	box7.Width			= 0;
	box7.Height			= 0;
	box7.Image_X		= 0;
	box7.Image_Y		= 0;
	box7.Mover_X		= 0;
	box7.Mover_Y		= 0;
	box7.Set_X			= 0;
	box7.Set_Y			= 0;
	box7.Animation		= -1;
	box7.AnimationOver	= -1;
	box7.AnimationLit	= -1;

	clip_text.Font = 0;

	box8.Image_Number	= 0;
	box8.Width			= 0;
	box8.Height			= 0;
	box8.Image_X		= 0;
	box8.Image_Y		= 0;
	box8.Mover_X		= 0;
	box8.Mover_Y		= 0;
	box8.Set_X			= 0;
	box8.Set_Y			= 0;
	box8.Animation		= -1;
	box8.AnimationOver	= -1;
	box8.AnimationLit	= -1;

	bb_text.Font = 0;

	box9.Image_Number	= 0;
	box9.Width			= 0;
	box9.Height			= 0;
	box9.Image_X		= 0;
	box9.Image_Y		= 0;
	box9.Mover_X		= 0;
	box9.Mover_Y		= 0;
	box9.Set_X			= 0;
	box9.Set_Y			= 0;
	box9.Animation		= -1;
	box9.AnimationOver	= -1;
	box9.AnimationLit	= -1;

	sebb_text.Font = 0;

	QuitCredit.Image_Number		= 0;
	QuitCredit.Width			= 0;
	QuitCredit.Height			= 0;
	QuitCredit.Image_X			= 0;
	QuitCredit.Image_Y			= 0;
	QuitCredit.Mover_X			= 0;
	QuitCredit.Mover_Y			= 0;
	QuitCredit.Animation		= -1;
	QuitCredit.AnimationOver	= -1;

	Credit_Img.Image_Number = 0;
	Credit_Img.Width		= 0;
	Credit_Img.Height		= 0;
	Credit_Img.Image_X		= 0;
	Credit_Img.Image_Y		= 0;
	Credit_Img.Animation	= -1;

	QuitAudio.Image_Number	= 0;
	QuitAudio.Width			= 0;
	QuitAudio.Height		= 0;
	QuitAudio.Image_X		= 0;
	QuitAudio.Image_Y		= 0;
	QuitAudio.Mover_X		= 0;
	QuitAudio.Mover_Y		= 0;
	QuitAudio.Animation		= -1;
	QuitAudio.AnimationOver = -1;

	vol_text.Font = 0;

	Vol_Img.Image_Number	= 0;
	Vol_Img.Width			= 0;
	Vol_Img.Height			= 0;
	Vol_Img.Image_X			= 0;
	Vol_Img.Image_Y			= 0;
	Vol_Img.Animation		= -1;

	Vol_Slide.Image_Number	= 0;
	Vol_Slide.Width			= 0;
	Vol_Slide.Height		= 0;
	Vol_Slide.Image_X		= 0;
	Vol_Slide.Image_Y		= 0;
	Vol_Slide.Min_X			= 0;
	Vol_Slide.Max_X			= 1;
	Vol_Slide.Animation		= -1;

	mvol_text.Font = 0;

	mVol_Img.Image_Number	= 0;
	mVol_Img.Width			= 0;
	mVol_Img.Height			= 0;
	mVol_Img.Image_X		= 0;
	mVol_Img.Image_Y		= 0;
	mVol_Img.Animation		= -1;

	mVol_Slide.Image_Number = 0;
	mVol_Slide.Width		= 0;
	mVol_Slide.Height		= 0;
	mVol_Slide.Image_X		= 0;
	mVol_Slide.Image_Y		= 0;
	mVol_Slide.Min_X		= 0;
	mVol_Slide.Max_X		= 1;
	mVol_Slide.Animation	= -1;

	box0.Image_Number	= 0;
	box0.Width			= 0;
	box0.Height			= 0;
	box0.Image_X		= 0;
	box0.Image_Y		= 0;
	box0.Mover_X		= 0;
	box0.Mover_Y		= 0;
	box0.Set_X			= 0;
	box0.Set_Y			= 0;
	box0.Animation		= -1;
	box0.AnimationOver	= -1;
	box0.AnimationLit	= -1;

	cd_text.Font = 0;

	QuitVideo.Image_Number	= 0;
	QuitVideo.Width			= 0;
	QuitVideo.Height		= 0;
	QuitVideo.Image_X		= 0;
	QuitVideo.Image_Y		= 0;
	QuitVideo.Mover_X		= 0;
	QuitVideo.Mover_Y		= 0;
	QuitVideo.Animation		= -1;
	QuitVideo.AnimationOver = -1;

	gam_text.Font = 0;

	Gamma_Img.Image_Number	= 0;
	Gamma_Img.Width			= 0;
	Gamma_Img.Height		= 0;
	Gamma_Img.Image_X		= 0;
	Gamma_Img.Image_Y		= 0;
	Gamma_Img.Animation		= -1;

	Gamma_Slide.Image_Number	= 0;
	Gamma_Slide.Width			= 0;
	Gamma_Slide.Height			= 0;
	Gamma_Slide.Image_X			= 0;
	Gamma_Slide.Image_Y			= 0;
	Gamma_Slide.Min_X			= 0;
	Gamma_Slide.Max_X			= 1;
	Gamma_Slide.Animation		= -1;

	det_text.Font = 0;

	Detail_Img.Image_Number = 0;
	Detail_Img.Width		= 0;
	Detail_Img.Height		= 0;
	Detail_Img.Image_X		= 0;
	Detail_Img.Image_Y		= 0;
	Detail_Img.Animation	= -1;

	Detail_Slide.Image_Number	= 0;
	Detail_Slide.Width			= 0;
	Detail_Slide.Height			= 0;
	Detail_Slide.Image_X		= 0;
	Detail_Slide.Image_Y		= 0;
	Detail_Slide.Min_X			= 0;
	Detail_Slide.Max_X			= 1;
	Detail_Slide.Animation		= -1;

// changed QD Shadows
	ShadowBox.Image_Number	= 0;
	ShadowBox.Width			= 0;
	ShadowBox.Height		= 0;
	ShadowBox.Image_X		= 0;
	ShadowBox.Image_Y		= 0;
	ShadowBox.Mover_X		= 0;
	ShadowBox.Mover_Y		= 0;
	ShadowBox.Set_X			= 0;
	ShadowBox.Set_Y			= 0;
	ShadowBox.Animation		= -1;
	ShadowBox.AnimationOver = -1;
	ShadowBox.AnimationLit	= -1;

	shadow_text.Font = 0;
// end change

	AdvancedItem.Image_Number	= 0;
	AdvancedItem.Width			= 0;
	AdvancedItem.Height			= 0;
	AdvancedItem.Image_X		= 0;
	AdvancedItem.Image_Y		= 0;
	AdvancedItem.Mover_X		= 0;
	AdvancedItem.Mover_Y		= 0;
	AdvancedItem.background		= 0;
	AdvancedItem.title			= 0;
	AdvancedItem.Animation		= -1;
	AdvancedItem.AnimationOver	= -1;

	QuitControl.Image_Number	= 0;
	QuitControl.Width			= 0;
	QuitControl.Height			= 0;
	QuitControl.Image_X			= 0;
	QuitControl.Image_Y			= 0;
	QuitControl.Mover_X			= 0;
	QuitControl.Mover_Y			= 0;
	QuitControl.Animation		= -1;
	QuitControl.AnimationOver	= -1;

	KeyMap.Image_Number = 0;
	KeyMap.Width		= 0;
	KeyMap.Height		= 0;
	KeyMap.Image_X		= 0;
	KeyMap.Image_Y		= 0;
	KeyMap.Start_X		= 0;
	KeyMap.Start_Y		= 0;
	KeyMap.Step			= 0;
	KeyMap.Max_Show		= 0;
	KeyMap.Corner_X		= 0;
	KeyMap.Corner_Y		= 0;
	KeyMap.Click_Width	= 0;
	KeyMap.Key_X		= 0;
	KeyMap.Rev_X		= 0;
	KeyMap.Rev_Y		= 0;
	KeyMap.Rev_Width	= 0;
	KeyMap.Rev_Height	= 0;
	KeyMap.RevC_X		= 0;
	KeyMap.RevC_Y		= 0;
	KeyMap.RevC_Width	= 0;
	KeyMap.RevC_Height	= 0;
	KeyMap.Font			= 0;

	KeyMapBar.Image_Number	= 0;
	KeyMapBar.Up_Width		= 0;
	KeyMapBar.Up_Height		= 0;
	KeyMapBar.Up_Nor_X		= 0;
	KeyMapBar.Up_Nor_Y		= 0;
	KeyMapBar.Up_Lit_X		= 0;
	KeyMapBar.Up_Lit_Y		= 0;
	KeyMapBar.Up_Push_X		= 0;
	KeyMapBar.Up_Push_Y		= 0;
	KeyMapBar.Dwn_Width		= 0;
	KeyMapBar.Dwn_Height	= 0;
	KeyMapBar.Dwn_Nor_X		= 0;
	KeyMapBar.Dwn_Nor_Y		= 0;
	KeyMapBar.Dwn_Lit_X		= 0;
	KeyMapBar.Dwn_Lit_Y		= 0;
	KeyMapBar.Dwn_Push_X	= 0;
	KeyMapBar.Dwn_Push_Y	= 0;
	KeyMapBar.Up_X			= 0;
	KeyMapBar.Up_Y			= 0;
	KeyMapBar.Dwn_X			= 0;
	KeyMapBar.Dwn_Y			= 0;
	KeyMapBar.Show			= 0;
	KeyMapBar.AnimationUp		= -1;
	KeyMapBar.AnimationUpOver	= -1;
	KeyMapBar.AnimationUpPush	= -1;
	KeyMapBar.AnimationDwn		= -1;
	KeyMapBar.AnimationDwnOver	= -1;
	KeyMapBar.AnimationDwnPush	= -1;

	ResetKey.Image_Number	= 0;
	ResetKey.Width			= 0;
	ResetKey.Height			= 0;
	ResetKey.Image_X		= 0;
	ResetKey.Image_Y		= 0;
	ResetKey.Mover_X		= 0;
	ResetKey.Mover_Y		= 0;
	ResetKey.Animation		= -1;
	ResetKey.AnimationOver	= -1;

	QuitAdvanced.Image_Number	= 0;
	QuitAdvanced.Width			= 0;
	QuitAdvanced.Height			= 0;
	QuitAdvanced.Image_X		= 0;
	QuitAdvanced.Image_Y		= 0;
	QuitAdvanced.Mover_X		= 0;
	QuitAdvanced.Mover_Y		= 0;
	QuitAdvanced.Animation		= -1;
	QuitAdvanced.AnimationOver	= -1;

	box1.Image_Number	= 0;
	box1.Width			= 0;
	box1.Height			= 0;
	box1.Image_X		= 0;
	box1.Image_Y		= 0;
	box1.Mover_X		= 0;
	box1.Mover_Y		= 0;
	box1.Set_X			= 0;
	box1.Set_Y			= 0;
	box1.Animation		= -1;
	box1.AnimationOver	= -1;
	box1.AnimationLit	= -1;

	xhair_text.Font = 0;

	box2.Image_Number	= 0;
	box2.Width			= 0;
	box2.Height			= 0;
	box2.Image_X		= 0;
	box2.Image_Y		= 0;
	box2.Mover_X		= 0;
	box2.Mover_Y		= 0;
	box2.Set_X			= 0;
	box2.Set_Y			= 0;
	box2.Animation		= -1;
	box2.AnimationOver	= -1;
	box2.AnimationLit	= -1;

	rev_text.Font = 0;

	box4.Image_Number	= 0;
	box4.Width			= 0;
	box4.Height			= 0;
	box4.Image_X		= 0;
	box4.Image_Y		= 0;
	box4.Mover_X		= 0;
	box4.Mover_Y		= 0;
	box4.Set_X			= 0;
	box4.Set_Y			= 0;
	box4.Animation		= -1;
	box4.AnimationOver	= -1;
	box4.AnimationLit	= -1;

	filter_text.Font = 0;

	sens_text.Font = 0;

	Sens_Img.Image_Number	= 0;
	Sens_Img.Width			= 0;
	Sens_Img.Height			= 0;
	Sens_Img.Image_X		= 0;
	Sens_Img.Image_Y		= 0;
	Sens_Img.Animation		= -1;

	Sens_Slide.Image_Number = 0;
	Sens_Slide.Width		= 0;
	Sens_Slide.Height		= 0;
	Sens_Slide.Image_X		= 0;
	Sens_Slide.Image_Y		= 0;
	Sens_Slide.Min_X		= 0;
	Sens_Slide.Max_X		= 1;
	Sens_Slide.Animation	= -1;

	SelectBack = 0;
	SelectTitle = 0;

	AcceptSelect.Image_Number	= 0;
	AcceptSelect.Width			= 0;
	AcceptSelect.Height			= 0;
	AcceptSelect.Image_X		= 0;
	AcceptSelect.Image_Y		= 0;
	AcceptSelect.Mover_X		= 0;
	AcceptSelect.Mover_Y		= 0;
	AcceptSelect.Animation		= -1;
	AcceptSelect.AnimationOver	= -1;

	CancelSelect.Image_Number	= 0;
	CancelSelect.Width			= 0;
	CancelSelect.Height			= 0;
	CancelSelect.Image_X		= 0;
	CancelSelect.Image_Y		= 0;
	CancelSelect.Mover_X		= 0;
	CancelSelect.Mover_Y		= 0;
	CancelSelect.Animation		= -1;
	CancelSelect.AnimationOver	= -1;

	PrevSelect.Image_Number		= 0;
	PrevSelect.Width			= 0;
	PrevSelect.Height			= 0;
	PrevSelect.Image_X			= 0;
	PrevSelect.Image_Y			= 0;
	PrevSelect.Mover_X			= 0;
	PrevSelect.Mover_Y			= 0;
	PrevSelect.Animation		= -1;
	PrevSelect.AnimationOver	= -1;

	NextSelect.Image_Number		= 0;
	NextSelect.Width			= 0;
	NextSelect.Height			= 0;
	NextSelect.Image_X			= 0;
	NextSelect.Image_Y			= 0;
	NextSelect.Mover_X			= 0;
	NextSelect.Mover_Y			= 0;
	NextSelect.Animation		= -1;
	NextSelect.AnimationOver	= -1;

// changed QD 12/15/05
	PlayerNameBack = 0;
	PlayerNameTitle = 0;

	AcceptName.Image_Number		= 0;
	AcceptName.Width			= 0;
	AcceptName.Height			= 0;
	AcceptName.Image_X			= 0;
	AcceptName.Image_Y			= 0;
	AcceptName.Mover_X			= 0;
	AcceptName.Mover_Y			= 0;
	AcceptName.Animation		= -1;
	AcceptName.AnimationOver	= -1;

	CancelName.Image_Number		= 0;
	CancelName.Width			= 0;
	CancelName.Height			= 0;
	CancelName.Image_X			= 0;
	CancelName.Image_Y			= 0;
	CancelName.Mover_X			= 0;
	CancelName.Mover_Y			= 0;
	CancelName.Animation		= -1;
	CancelName.AnimationOver	= -1;

	PlayerName.Font		= 0;
	PlayerName.Set_X	= 10;
	PlayerName.Set_Y	= 10;
	PlayerName.Width	= 100;

	DefaultName.Image_Number	= 0;
	DefaultName.Width			= 0;
	DefaultName.Height			= 0;
	DefaultName.Image_X			= 0;
	DefaultName.Image_Y			= 0;
	DefaultName.Mover_X			= 0;
	DefaultName.Mover_Y			= 0;
	DefaultName.Animation		= -1;
	DefaultName.AnimationOver	= -1;
// end change

	LoadFont = 0;

	SaveFont = 0;

	DifficultBack = 0;
	DifficultTitle = 0;

	Difficultlow.Image_Number	= 0;
	Difficultlow.Width			= 0;
	Difficultlow.Height			= 0;
	Difficultlow.Image_X		= 0;
	Difficultlow.Image_Y		= 0;
	Difficultlow.Mover_X		= 0;
	Difficultlow.Mover_Y		= 0;
	Difficultlow.Animation		= -1;
	Difficultlow.AnimationOver	= -1;

	Difficultmid.Image_Number	= 0;
	Difficultmid.Width			= 0;
	Difficultmid.Height			= 0;
	Difficultmid.Image_X		= 0;
	Difficultmid.Image_Y		= 0;
	Difficultmid.Mover_X		= 0;
	Difficultmid.Mover_Y		= 0;
	Difficultmid.Animation		= -1;
	Difficultmid.AnimationOver	= -1;

	Difficulthi.Image_Number	= 0;
	Difficulthi.Width			= 0;
	Difficulthi.Height			= 0;
	Difficulthi.Image_X			= 0;
	Difficulthi.Image_Y			= 0;
	Difficulthi.Mover_X			= 0;
	Difficulthi.Mover_Y			= 0;
	Difficulthi.Animation		= -1;
	Difficulthi.AnimationOver	= -1;

	QuitMods.Image_Number	= 0;
	QuitMods.Width			= 0;
	QuitMods.Height			= 0;
	QuitMods.Image_X		= 0;
	QuitMods.Image_Y		= 0;
	QuitMods.Mover_X		= 0;
	QuitMods.Mover_Y		= 0;
	QuitMods.Animation		= -1;
	QuitMods.AnimationOver	= -1;

	free(return_text.text);
	free(PlayerIP_Text.text);
	free(MultiplayerHelp_Text.text);
	free(IPAdd_Text.text);
	free(sebb_text.text);
	free(bb_text.text);
	free(clip_text.text);
	free(fps_text.text);
	free(debug_text.text);
	free(cd_text.text);
	free(mvol_text.text);
	free(vol_text.text);
	free(det_text.text);
	free(gam_text.text);
	free(sens_text.text);
	free(filter_text.text);
	free(rev_text.text);
	free(xhair_text.text);
// changed QD Shadows
	free(shadow_text.text);
// end change

	return_text.text			= strdup("Press ESC to Return to Game");
	PlayerIP_Text.text			= strdup("Your IP :");
	MultiplayerHelp_Text.text	= strdup("Enter a valid server ip, press ENTER and click on Join game");
	IPAdd_Text.text				= strdup("Server IP Address :");
	sebb_text.text				= strdup("Entity Bounding Box");
	bb_text.text				= strdup("Player Bounding Box");
	clip_text.text				= strdup("No Clipping");
	fps_text.text				= strdup("Frame Rate");
	debug_text.text				= strdup("Debug Info");
	cd_text.text				= strdup("Play CD Music");
	mvol_text.text				= strdup("Music Volume");
	vol_text.text				= strdup("Sound Volume");
	det_text.text				= strdup("Detail Level");
	gam_text.text				= strdup("Gamma");
	sens_text.text				= strdup("Mouse sensitivity");
	filter_text.text			= strdup("Mouse filter");
	rev_text.text				= strdup("Reverse mouse");
	xhair_text.text				= strdup("Crosshair");
// changed QD Shadows
	shadow_text.text			= strdup("Enable Stencil Shadows");
// end change
}
// end change QD


/* ----------------------------------- END OF FILE ------------------------------------ */
