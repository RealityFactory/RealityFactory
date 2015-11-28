


//	Include the One True Header

#include "RabidFramework.h"
#include "Ram.h"

// start multiplayer
#include "HawkNL\\nl.h"
// end multiplayer

extern geBitmap *TPool_Bitmap(char *DefaultBmp, char *DefaultAlpha, char *BName, char *AName);

//#define BLIT

static int GetGammaPercent();
static void SetGamma(int percent);
static void SetDetail(int percent);
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

// Start Multiplayer
char ServerIP[50];
char PlayerName[50];
char IP[NL_MAX_STRING_LENGTH]="";
// end Multiplayer

#define GAMMAMIN   0.0f
#define GAMMAMAX   2.0f

#define MOUSEMIN	0.0005f
#define MOUSEMAX	0.005f

//------------------------------
// Menu Item Types
//------------------------------

typedef enum 
{
  END_LIST=0,
  CLICKABLE,
  IMAGE,
// changed RF063
  CHARIMAGE,
  SLIDER,
  BOX,
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
  int Width;        // width of graphic
  int Height;       // height of graphic
  int Image_Number; // index of Images bitmap to use
  int Image_X;      // X location on bitmap of graphic
  int Image_Y;      // Y location on bitmap of graphic
  int Mover_X;      // X location of mouse-over graphic
  int Mover_Y;      // Y location of mouse-over graphic
  int Action;       // type of action to perform when clicked on
  
  // Action type 0 - go to another menu
  // Action type 2 - go to another menu if level is loaded (Save)
  int title;        // index of title to display
  int background;   // index of background to use
  MenuItem *Next;   // menu to go to

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
  int Width;        // width of graphic
  int Height;       // height of graphic
  int Image_Number; // index of Images bitmap to use
  int Image_X;      // X location on bitmap of graphic
  int Image_Y;      // Y location on bitmap of graphic
// changed RF063
  int Animation;
} Image;

//--------------------------
// Slider menu type
//--------------------------

typedef struct Slider
{
  int Width;        // width of graphic
  int Height;       // height of graphic
  int Image_Number; // index of Images bitmap to use
  int Image_X;      // X location on bitmap of graphic
  int Image_Y;      // Y location on bitmap of graphic
  int Min_X;        // distance from origin of slider start
  int Max_X;        // distance from origin of slider end
  int On_the_Fly;	// call function each frame = 1
  int Current;      // inital slider location (Min_X <= >=Max_X)
  void (*proc)(int percent); // function to call when slider moves
  int Animation;
} Slider;

//--------------------------
// Box menu type
//--------------------------

typedef struct Box
{
  int Width;        // width of graphic
  int Height;       // height of graphic
  int Image_Number; // index of Images bitmap to use
  int Image_X;      // X location on bitmap of graphic
  int Image_Y;      // Y location on bitmap of graphic
  int Mover_X;		// X location of lit graphic
  int Mover_Y;		// Y location of lit graphic
  int Set_X;		// X location of set box graphic
  int Set_Y;		// Y location of set box graphic
  int Current;		// state of the box (on/off)
  void (*proc)(int current); // function to call when current changes
// changed RF063
  int Animation;
  int AnimationOver;
  int AnimationLit;
} Box;

//--------------------------
// Text menu type
//--------------------------

typedef struct Text
{
  int Font;			// font to use
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
  KEYCODES	key;	// value of key
} Keyname;

// start Multiplayer
//--------------------------
// TextEdit menu type
//--------------------------

typedef struct TextEdit
{
  int Font;			// font to use
  char text[255];		// text to display  
  int Set_X;        // X location on screen
  int Set_Y;        // Y location on screen
  int Width;        // width max of the TextEdit
  Keyname *keyname;
} TextEdit;
// end Multiplayer

//--------------------------
// Scroll bar menu type
//--------------------------

typedef struct ScrollBar
{
  int Image_Number; // index of Images bitmap to use
  int Up_X;			// offset to X of up arrow box
  int Up_Y;			// offset to Y of up arrow box
  int Up_Width;		// width of up arrow box
  int Up_Height;	// height of up arrow box
  int Dwn_X;		// offset to X of down arrow box
  int Dwn_Y;		// offset to Y of down arrow box
  int Dwn_Width;	// width of down arrow box
  int Dwn_Height;	// height of down arrow box
  int Up_Nor_X;     // X location of up normal bitmap of graphic
  int Up_Nor_Y;     // Y location of up normal bitmap of graphic
  int Dwn_Nor_X;    // X location of dwn normal bitmap of graphic
  int Dwn_Nor_Y;    // Y location of dwn normal bitmap of graphic
  int Up_Lit_X;     // X location of up lit bitmap of graphic
  int Up_Lit_Y;     // Y location of up lit bitmap of graphic
  int Up_Push_X;    // X location of up pushed bitmap of graphic
  int Up_Push_Y;    // Y location of up pushed bitmap of graphic
  int Dwn_Lit_X;    // X location of down lit bitmap of graphic
  int Dwn_Lit_Y;    // Y location of down lit bitmap of graphic
  int Dwn_Push_X;   // X location of down pushed bitmap of graphic
  int Dwn_Push_Y;   // Y location of down pushed bitmap of graphic
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
  int Width;        // width of graphic
  int Height;       // height of graphic
  int Image_Number; // index of Images bitmap to use
  int Image_X;      // X location on bitmap of graphic
  int Image_Y;      // Y location on bitmap of graphic

  int Start_X;		// upper left corner offset of line text
  int Start_Y;
  int Step;			// height of line
  int Max_Show;		// max lines to display
  Keydef *keydef;	// lines to display
  Keyname *keyname;
  int Key_X;		// offset from Start_X to key code text

  int Start;		// first line to display
  int Max;			// max # of lines - is calculated
  int Current;		// current line highlighted
  int Corner_X;		// upper left corner of highlight bar
  int Corner_Y;
  int Click_Width;	// width of clickable area
  int Rev_X;		// offset of highlight bar in graphic
  int Rev_Y;
  int Rev_Width;	// width of highlight bar
  int Rev_Height;	// height of highlight bar
  int RevC_X;		// offset of entry highlight bar in graphic
  int RevC_Y;
  int RevC_Width;	// width of entry highlight bar
  int RevC_Height;	// height of entry highlight bar
  int Font;
} Remap;

//--------------------------
// load/save box menu type
//--------------------------

typedef struct LSBox
{
  int Width;        // width of graphic
  int Height;       // height of graphic
  int Image_Number; // index of Images bitmap to use
  int Image_X;      // X location on bitmap of graphic
  int Image_Y;      // Y location on bitmap of graphic

  int Start_X;		// upper left corner offset of line text
  int Start_Y;
  int Step;			// height of line
  int Max_Show;		// max lines to display
  Savedef *text;		// lines to display

  int Start;		// first line to display
  int Max;			// max # of lines - is calculated
  int Current;		// current line highlighted
  int Corner_X;		// upper left corner of highlight bar
  int Corner_Y;
  int Click_Width;	// width of clickable area
  int Rev_X;		// offset of highlight bar in graphic
  int Rev_Y;
  int Rev_Width;	// width of highlight bar
  int Rev_Height;	// height of highlight bar
  int Font;
} LSBox;

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

Clickable QuitAdvanced =  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, -1, -1};
Box box1 =          {0,  0, 0, 0, 0, 0, 0, 0, 0, BOX_OFF, NULL, -1, -1, -1};
Text xhair_text =      {0, "Crosshair", 0};
Box box2 =          {0,  0, 0, 0, 0, 0, 0, 0, 0, BOX_OFF, SetReverse, -1, -1, -1};
Text rev_text =      {0, "Reverse mouse", 0};
Box box4 =          {0,  0, 0, 0, 0, 0, 0, 0, 0, BOX_OFF, SetFiltering, -1, -1, -1};
Text filter_text =      {0, "Mouse filter", 0};
Image Sens_Img     = {0, 0, 0, 0, 0, -1};
Text sens_text =      {0, "Mouse sensitivity", 0};
Slider Sens_Slide  = {0, 0, 0, 0, 0, 0, 0, 0, 0, SetSens, -1};

MenuItem AdvancedMenu[] = 
  {
     {EXIT_MENU, 0, 0, (void *)&QuitAdvanced},
	 {BOX,       0, 0, (void *)&box1},
     {TEXT,      0, 0, (void *)&xhair_text},
	 {BOX,       0, 0, (void *)&box2},
     {TEXT,      0, 0, (void *)&rev_text},
	 {BOX,       0, 0, (void *)&box4},
     {TEXT,      0, 0, (void *)&filter_text},
     {TEXT,      0, 0, (void *)&sens_text},
	 {IMAGE,     0, 0, (void *)&Sens_Img},
	 {SLIDER,    0, 0, (void *)&Sens_Slide},
     {END_LIST, 0, 0, NULL}
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
	{"Cntrl", KEY_CONTROL},
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

Clickable QuitControl =  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, -1, -1};
Clickable AdvancedItem = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, AdvancedMenu, NULL, 0, -1, -1};
Remap KeyMap = {0, 0, 0, 0, 0, 0, 0, 0, 0, Redef, Rename, 0, 0, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
ScrollBar KeyMapBar = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
						0, 0, 0, 0, &(KeyMap.Max), &(KeyMap.Start), 0, -1, -1, -1, -1, -1, -1};
Clickable ResetKey = {0, 0, 0, 0, 0, 0, 0, 3, 0, 0, NULL, ResetAction, 0, -1, -1};

MenuItem ControlMenu[] = 
  {
	 {CLICKABLE, 0, 0, (void *)&AdvancedItem},
     {EXIT_MENU, 0, 0, (void *)&QuitControl},
	 {REMAP,     0, 0, (void *)&KeyMap},
	 {SCROLLBAR, 0, 0, (void *)&KeyMapBar},
	 {CLICKABLE, 0, 0, (void *)&ResetKey},
     {END_LIST, 0, 0, NULL}
  };


//-------------------------------------
// Video Menu
//-------------------------------------

Clickable QuitVideo = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, -1, -1};
Text gam_text =     {0, "Gamma", 0};
Image Gamma_Img     = {0, 0, 0, 0, 0, -1};
Slider Gamma_Slide  = {0, 0, 0, 0, 0, 0, 0, 0, 0, SetGamma, -1};
Text det_text =     {0, "Detail Level", 0};
Image Detail_Img     = {0, 0, 0, 0, 0, -1};
Slider Detail_Slide  = {0, 0, 0, 0, 0, 0, 0, 0, 0, SetDetail, -1};

MenuItem VideoMenu[] = 
  {
     {EXIT_MENU, 0, 0, (void *)&QuitVideo},
	 {TEXT,      0, 0, (void *)&gam_text},
	 {IMAGE,     0, 0, (void *)&Gamma_Img},
	 {SLIDER,    0, 0, (void *)&Gamma_Slide},
	 {TEXT,      0, 0, (void *)&det_text},
	 {IMAGE,     0, 0, (void *)&Detail_Img},
	 {SLIDER,    0, 0, (void *)&Detail_Slide},
     {END_LIST, 0, 0, NULL}
  };

//-------------------------------------
// Audio Menu
//-------------------------------------

Clickable QuitAudio =   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, -1, -1};
Text vol_text =     {0, "Sound Volume", 0};
Image Vol_Img     = {0, 0, 0, 0,  0, -1};
Slider Vol_Slide  = {0, 0, 0, 0, 0, 0, 0, 0, 0, SetVol, -1};
Text mvol_text =    {0, "Music Volume", 0};
Image mVol_Img    = {0, 0, 0, 0,  0, -1};
Slider mVol_Slide = {0, 0, 0, 0, 0, 0, 0, 0, 0, SetmVol, -1};
Box box0 =          {0,  0, 0, 0, 0, 0, 0, 0, 0, BOX_OFF, SetCDPlayer, -1, -1, -1};
Text cd_text =      {0, "Play CD Music", 0};

MenuItem AudioMenu[] = 
  {
     {EXIT_MENU, 0, 0, (void *)&QuitAudio},
	 {TEXT,      0, 0, (void *)&vol_text},
	 {IMAGE,     0, 0, (void *)&Vol_Img},
	 {SLIDER,    0, 0, (void *)&Vol_Slide},
	 {TEXT,      0, 0, (void *)&mvol_text},
	 {IMAGE,     0, 0, (void *)&mVol_Img},
	 {SLIDER,    0, 0, (void *)&mVol_Slide},
	 {BOX,       0, 0, (void *)&box0},
     {TEXT,      0, 0, (void *)&cd_text},
     {END_LIST, 0, 0, NULL}
  };

//-------------------------------------
// Debug Menu
//-------------------------------------

Clickable QuitDebug =   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, -1, -1};
Box box5 =          {0,  0, 0, 0, 0, 0, 0, 0, 0, BOX_OFF, NULL, -1, -1, -1};
Text debug_text =      {0, "Debug Info", 0};
Box box6 =          {0,  0, 0, 0, 0, 0, 0, 0, 0, BOX_OFF, NULL, -1, -1, -1};
Text fps_text =      {0, "Frame Rate", 0};
Box box7 =          {0,  0, 0, 0, 0, 0, 0, 0, 0, BOX_OFF, SetClipping, -1, -1, -1};
Text clip_text =      {0, "No Clipping", 0};
Box box8 =          {0,  0, 0, 0, 0, 0, 0, 0, 0, BOX_OFF, SetBBox, -1, -1, -1};
Text bb_text =      {0, "Player Bounding Box", 0};
Box box9 =          {0,  0, 0, 0, 0, 0, 0, 0, 0, BOX_OFF, SetSEBBox, -1, -1, -1};
Text sebb_text =      {0, "Entity Bounding Box", 0};

MenuItem DebugMenu[] = 
  {
     {EXIT_MENU, 0, 0, (void *)&QuitDebug},
	 {BOX,       0, 0, (void *)&box5},
     {TEXT,      0, 0, (void *)&debug_text},
	 {BOX,       0, 0, (void *)&box6},
     {TEXT,      0, 0, (void *)&fps_text},
	 {BOX,       0, 0, (void *)&box7},
     {TEXT,      0, 0, (void *)&clip_text},
	 {BOX,       0, 0, (void *)&box8},
     {TEXT,      0, 0, (void *)&bb_text},
	 {BOX,       0, 0, (void *)&box9},
     {TEXT,      0, 0, (void *)&sebb_text},
     {END_LIST, 0, 0, NULL}
  };

//-------------------------------------
// Options Menu
//-------------------------------------

Clickable QuitOption =  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, -1, -1};
Clickable AudioItem =   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, AudioMenu, NULL, 0, -1, -1};
Clickable VideoItem =   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, VideoMenu, NULL, 0, -1, -1};
Clickable ControlItem = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ControlMenu, NULL, 0, -1, -1};
Clickable DebugItem =   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, DebugMenu, NULL, 0, -1, -1};

MenuItem OptionMenu[] = 
  {
     {CLICKABLE, 0, 0, (void *)&AudioItem},
     {CLICKABLE, 0, 0, (void *)&VideoItem},
	 {CLICKABLE, 0, 0, (void *)&ControlItem},
	 {CLICKABLE, 0, 0, (void *)&DebugItem},
     {EXIT_MENU, 0, 0, (void *)&QuitOption},
     {END_LIST, 0, 0, NULL}
  };

//---------------------
// saved game default
//---------------------
// changed RF063
Savedef SavedGame[16];
// end change RF063

//-------------------------------------
// Save Game Menu
//-------------------------------------

Clickable QuitSave =   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, -1, -1};
Clickable SaveSlot =   {0, 0, 0, 0, 0, 30, 0, 1, 0, 0, NULL, SetSlot, 0, -1, -1};
LSBox SaveBox = {0, 0, 0, 0, 0, 0, 0, 0, 0, SavedGame, 0, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0};
ScrollBar SaveBar = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
						0, 0, 0, 0, &(SaveBox.Max), &(SaveBox.Start), 0, -1, -1, -1, -1, -1, -1};

MenuItem SaveMenu[] = 
  {
     {EXIT_MENU, 0, 0, (void *)&QuitSave},
	 {CLICKABLE, 0, 0, (void *)&SaveSlot},
	 {LSBOX,     0, 0, (void *)&SaveBox},
	 {SCROLLBAR, 0, 0, (void *)&SaveBar},
     {END_LIST, 0, 0, NULL}
  };

//-------------------------------------
// Load Game Menu
//-------------------------------------

Clickable QuitLoad =   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, -1, -1};
Clickable LoadSlot =   {0, 0, 0, 0, 0, 0, 0, 4, 0, 0, NULL, GetSlot, 1, -1, -1};
LSBox LoadBox = {0, 0, 0, 0, 0, 0, 0, 0, 0, SavedGame, 0, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0};
ScrollBar LoadBar = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
						0, 0, 0, 0, &(LoadBox.Max), &(LoadBox.Start), 0, -1, -1, -1, -1, -1, -1};

MenuItem LoadGMenu[] = 
  {
     {EXIT_MENU, 0, 0, (void *)&QuitLoad},
	 {CLICKABLE, 0, 0, (void *)&LoadSlot},
	 {LSBOX,     0, 0, (void *)&LoadBox},
	 {SCROLLBAR, 0, 0, (void *)&LoadBar},
     {END_LIST, 0, 0, NULL}
  };


//-------------------------------------
// Credit Menu
//-------------------------------------

Clickable QuitCredit =   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, -1, -1};
Image Credit_Img     = {0, 0, 0, 0, 0, -1};

MenuItem CreditMenu[] = 
  {
     {EXIT_MENU, 0, 0, (void *)&QuitCredit},
	 {IMAGE,	 0, 0, (void *)&Credit_Img},
     {END_LIST, 0, 0, NULL}
  };

//-------------------------------------
// Multiplayer Game Menu
//-------------------------------------
// start Multiplayer

//
// Join Game menu
//

Text IPAdd_Text =  {0, "Server IP Address :", 0};
TextEdit IPAddress =  {0, "Enter Server IP here", 10,10,100,TextEditKeys};
Clickable LaunchJoinGame =  {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, NULL, RunJoinGame,1,-1,-1};
Clickable QuitJoinGame =   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0,-1,-1};
Text MultiplayerHelp_Text={0, "Enter a valid server ip, press ENTER and click on Join game", 0};
MenuItem JoinGameMenu[] = 
  {	 
	 {TEXT, 0, 0, (void *)&IPAdd_Text},
	 {TEXTEDIT, 0, 0, (void *)&IPAddress},
	 {CLICKABLE, 0, 0, (void *)&LaunchJoinGame},
     {EXIT_MENU, 0, 0, (void *)&QuitJoinGame}, 
     {TEXT, 0, 0, (void *)&MultiplayerHelp_Text},    
     {END_LIST, 0, 0, NULL}
  };

//
// Multiplayer menu
//

Clickable HostNewGame =   {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, NULL, RunHostGame, 1,-1,-1};
Clickable JoinMultiplayerGame =   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, JoinGameMenu, NULL, 0,-1,-1};		
Clickable QuitMulti =   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0,-1,-1};
Text PlayerIP_Text =  {0, "Your IP(s) :", 0};
Text PlayerIP1 =  {0,IP , 0};

MenuItem MultiMenu[] = 
  {
	 {CLICKABLE, 0, 0, (void *)&HostNewGame},
	 {CLICKABLE, 0, 0, (void *)&JoinMultiplayerGame},
     {EXIT_MENU, 0, 0, (void *)&QuitMulti},
     {TEXT, 0, 0, (void *)&PlayerIP_Text},
     {TEXT, 0, 0, (void *)&PlayerIP1},

     {END_LIST, 0, 0, NULL}
  };

// end Multiplayer

// changed RF063
//-------------------------------------
// Character Selection Menu
//-------------------------------------

Clickable AcceptSelect =   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 1, -1, -1};
Clickable CancelSelect =   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 1, -1, -1};
Clickable PrevSelect =   {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, NULL, PrevChar, 0, -1, -1};
Clickable NextSelect =   {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, NULL, NextChar, 0, -1, -1};

MenuItem SelectMenu[] = 
  {
     {EXIT_MENU, 0, 0, (void *)&AcceptSelect},
	 {CANCEL_MENU, 0, 0, (void *)&CancelSelect},
	 {CLICKABLE, 0, 0, (void *)&PrevSelect},
	 {CLICKABLE, 0, 0, (void *)&NextSelect},
	 {CHARIMAGE, 0, 0, NULL},
     {END_LIST, 0, 0, NULL}
  };

// end change RF063

//-------------------------------------
// Difficulty Selection Menu
//-------------------------------------

Clickable Difficultlow =   {0, 0, 0, 0, 0, 0, 0, 10, 0, 0, NULL, NULL, 1, -1, -1};
Clickable Difficultmid =   {0, 0, 0, 0, 0, 0, 0, 11, 0, 0, NULL, NULL, 1, -1, -1};
Clickable Difficulthi  =   {0, 0, 0, 0, 0, 0, 0, 12, 0, 0, NULL, NULL, 1, -1, -1};

MenuItem DifficultMenu[] = 
  {
     {EXIT_MENU, 0, 0, (void *)&Difficultlow},
	 {EXIT_MENU, 0, 0, (void *)&Difficultmid},
	 {EXIT_MENU, 0, 0, (void *)&Difficulthi},
     {END_LIST, 0, 0, NULL}
  };

//-------------------------------------
// Main Menu
//-------------------------------------

Clickable NewGame =  {0, 0, 0, 0,   0, 0,  0, 1, 0, 0, NULL, RunGame, 1, -1, -1};
Clickable MultiPlayer = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, MultiMenu, NULL, 0, -1, -1};
Clickable LoadGame = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, LoadGMenu, NULL, 0, -1, -1};
Clickable SaveGame = {0, 0, 0, 0, 0, 0, 0, 2, 0, 0, SaveMenu, NULL, 0, -1, -1};
Clickable Options =  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, OptionMenu, NULL, 0, -1, -1};
Clickable Credits =  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CreditMenu, NULL, 0, -1, -1};
Clickable QuitGame = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 1, -1, -1};
Text return_text =   {0, "Press ESC to Return to Game", 1};

MenuItem MainMenu[] = 
  {
	 {CLICKABLE, 0, 0, (void *)&NewGame},
	 {CLICKABLE, 0, 0, (void *)&MultiPlayer},
     {CLICKABLE, 0, 0, (void *)&LoadGame},
	 {CLICKABLE, 0, 0, (void *)&SaveGame},
     {CLICKABLE, 0, 0, (void *)&Options},
	 {CLICKABLE, 0, 0, (void *)&Credits},
     {EXIT_MENU, 0, 0, (void *)&QuitGame},
	 {TEXT,      0, 0, (void *)&return_text},
     {END_LIST, 0, 0, NULL}
  };

//-------------------------------------
// Titles
//-------------------------------------

// changed RF063
MenuTitle MTitles[NUM_TITLES];
char errortext[256];
// end change RF063

char *FirstToken(char *string1, char *string2)
{
	return strtok(string1,string2);
}

char *NextToken()
{
	char *temp;
	temp = strtok(NULL," \n");
	if(temp==(char *)NULL)
	{
		char szBug[256];
		sprintf(szBug, "Missing menu token in line\n %s", errortext);
		CCD->ReportError(szBug, false);
		exit(-100);
	}
	return temp;
}

int NextValue()
{
	char *temp;
	temp = NextToken();
	return atoi(temp);
}

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
		sprintf(szBug, "Bad font size name %s", szArg);
		CCD->ReportError(szBug, false);
		exit(-100);
	}
	return index;
}

CRFMenu::CRFMenu()
{
  geVFile *MainFS;
// changed RF063
//  FILE *fd;
  geBitmap_Info	BmpInfo;
  int i;

// start multiplayer
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

  Bottom = (SoundList *)NULL;
  Screen = 0;
  ScrnWait = false;
  savetime = 0.0f;
  musictype = -1;
  mVolLevel = 0.5f;
// changed RF064
  theMIDIPlayer = NULL;
  strcpy(Loading, "menu\\loading.bmp");
// end change RF064
// MENUFIX
  ingame = 0;

  for(i=0; i<NUM_BACKGROUNDS;i++)
		Backgrounds[i] = (geBitmap *)NULL;

  for(i=0; i<NUM_IMAGES;i++)
		Images[i] = (geBitmap *)NULL;

  for(i=0; i<NUM_TITLES;i++)
		Titles[i] = (geBitmap *)NULL;

  for(i=0; i<NUM_FONTS;i++)
		MenuFont[i].Bitmap = (geBitmap *)NULL;

  for(i=0; i<NUM_ANIM;i++)
		Animation[i] = NULL;

// changed RF063	
	geVFile *SecondFS;

	if(CCD->OpenRFFile(&SecondFS, kInstallFile, CCD->MenuIni(), GE_VFILE_OPEN_READONLY))
// end change RF063
	  {
		// File there, parse it!
		char szInputLine[256];
		char *szAtom;
		char menuline[256], menuline2[256];
// changed RF063
		while(geVFile_GetS(SecondFS, szInputLine, 256)==GE_TRUE)
// end change RF063
		  {
			if(szInputLine[0] == ';') 
			  continue;				// Comment line
			if(strlen(szInputLine) <= 5)
			  continue;				// Skip blank lines
// changed RF063
			strcpy(errortext,szInputLine);
			// All config commands are "thing=value"
			szAtom = FirstToken(szInputLine," =");
			if(!stricmp(szAtom,"background"))
			{
				int index = NextValue();
				strcpy(menuline,"menu\\");
				strcat(menuline,NextToken());
				geBitmap *TB = CreateFromFileName(menuline);
				if(TB == (geBitmap *)NULL)
				{
					char szBug[256];
					sprintf(szBug, "Bad file name %s", menuline);
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
				if(Backgrounds[index] == (geBitmap *)NULL)
				{
					char szBug[256];
					sprintf(szBug, "Bad file name %s", menuline);
					CCD->ReportError(szBug, false);
					exit(-100);
				}
				geEngine_AddBitmap(CCD->Engine()->Engine(), Backgrounds[index]);
			}
			else if(!stricmp(szAtom,"images"))
			{
				int index = NextValue();
				strcpy(menuline,"menu\\");
				strcat(menuline,NextToken());
				strcpy(menuline2,"menu\\");
				strcat(menuline2,NextToken());
				Images[index] = CreateFromFileAndAlphaNames(menuline, menuline2);
				if(Images[index] == (geBitmap *)NULL)
				{
					char szBug[256];
					sprintf(szBug, "Bad file name %s", menuline);
					CCD->ReportError(szBug, false);
					exit(-100);
				}
				geEngine_AddBitmap(CCD->Engine()->Engine(), Images[index]);
			}			
			else if(!stricmp(szAtom,"titles"))
			{
				int index = NextValue();
				strcpy(menuline,"menu\\");
				strcat(menuline,NextToken());
				strcpy(menuline2,"menu\\");
				strcat(menuline2,NextToken());
				Titles[index] = CreateFromFileAndAlphaNames(menuline, menuline2);
				if(Titles[index] == (geBitmap *)NULL)
				{
					char szBug[256];
					sprintf(szBug, "Bad file name %s", menuline);
					CCD->ReportError(szBug, false);
					exit(-100);
				}
				geEngine_AddBitmap(CCD->Engine()->Engine(), Titles[index]);
			}
			else if(!stricmp(szAtom,"font"))
			{
				int index = NextFont();
				char fontline[132];
				strcpy(fontline,NextToken());
				strcpy(menuline,"fonts\\");
				strcat(menuline,fontline);
				strcat(menuline,".bmp");
				strcpy(menuline2,"fonts\\a_");
				strcat(menuline2,fontline);
				strcat(menuline2,".bmp");
				MenuFont[index].Bitmap = CreateFromFileAndAlphaNames(menuline, menuline2);
				if(MenuFont[index].Bitmap == (geBitmap *)NULL)
				{
					char szBug[256];
					sprintf(szBug, "Bad file name %s", menuline);
					CCD->ReportError(szBug, false);
					exit(-100);
				}
				geEngine_AddBitmap(CCD->Engine()->Engine(), MenuFont[index].Bitmap);
				for(int jj=0;jj<96;jj++)
					MenuFont[index].WBitmap[jj] = NULL;

				strcpy(menuline,"fonts\\");
				strcat(menuline,fontline);
				strcat(menuline,".dat");
				geVFile *datFile;
				CCD->OpenRFFile(&datFile, kBitmapFile, menuline, GE_VFILE_OPEN_READONLY);
				geVFile_Read(datFile,&MenuFont[index].font_height,sizeof(int));
				for(int c=0;c<96;c++)
				{
					geVFile_Read(datFile,&MenuFont[index].dat[c].width,sizeof(int));
					geVFile_Read(datFile,&MenuFont[index].dat[c].x,sizeof(int));
					geVFile_Read(datFile,&MenuFont[index].dat[c].y,sizeof(int));
				}
				geVFile_Close(datFile);
			}
// changed RF063
			else if(!stricmp(szAtom,"animation"))
			{
				int index = NextValue();
				strcpy(menuline,"menu\\");
				strcat(menuline,NextToken());
				Animation[index] = new CAnimGif(menuline, kVideoFile);
			}
			else if(!stricmp(szAtom,"menutitle"))
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
// end change RF063
// changed RF064
			else if(!stricmp(szAtom,"loadscreen"))
			{
				strcpy(Loading,"menu\\");
				strcat(Loading,NextToken());
			}
// end change RF064
			else if(!stricmp(szAtom,"cursor"))
			{
				strcpy(menuline,"menu\\");
				strcat(menuline,NextToken());
				strcpy(menuline2,"menu\\");
				strcat(menuline2,NextToken());
				Cursor = CreateFromFileAndAlphaNames(menuline, menuline2);
				if(Cursor == (geBitmap *)NULL)
				{
					char szBug[256];
					sprintf(szBug, "Bad file name %s", menuline);
					CCD->ReportError(szBug, false);
					exit(-100);
				}
				geEngine_AddBitmap(CCD->Engine()->Engine(), Cursor);
				AnimCursor = NextValue();
			}
			else if(!stricmp(szAtom,"crosshair"))
			{
				strcpy(menuline,"menu\\");
				strcat(menuline,NextToken());
				strcpy(menuline2,"menu\\");
				strcat(menuline2,NextToken());
				Crosshair = CreateFromFileAndAlphaNames(menuline, menuline2);
				if(Crosshair == (geBitmap *)NULL)
				{
					char szBug[256];
					sprintf(szBug, "Bad file name %s", menuline);
					CCD->ReportError(szBug, false);
					exit(-100);
				}
				FCrosshair = CreateFromFileAndAlphaNames(menuline, menuline2);
				geEngine_AddBitmap(CCD->Engine()->Engine(), FCrosshair);
			}
			else if(!stricmp(szAtom,"main"))
			{
				MainBack = NextValue();
				MainTitle = NextValue();
			}
			else if(!stricmp(szAtom,"newgame"))
			{
				MainMenu[0].X = NextValue();
				MainMenu[0].Y = NextValue();
				NewGame.Image_Number = NextValue();
				NewGame.Width = NextValue();
				NewGame.Height = NextValue();
				NewGame.Image_X = NextValue();
				NewGame.Image_Y = NextValue();
				NewGame.Mover_X = NextValue();
				NewGame.Mover_Y = NextValue();
// changed RF063
				NewGame.Animation = NextValue();
				NewGame.AnimationOver = NextValue();
			}
			else if(!stricmp(szAtom,"multiplayer"))
			{
				MainMenu[1].X = NextValue();
				MainMenu[1].Y = NextValue();
				MultiPlayer.Image_Number = NextValue();
				MultiPlayer.Width = NextValue();
				MultiPlayer.Height = NextValue();
				MultiPlayer.Image_X = NextValue();
				MultiPlayer.Image_Y = NextValue();
				MultiPlayer.Mover_X = NextValue();
				MultiPlayer.Mover_Y = NextValue();
				MultiPlayer.background = NextValue();
				MultiPlayer.title = NextValue();
// changed RF063
				MultiPlayer.Animation = NextValue();
				MultiPlayer.AnimationOver = NextValue();
			}
			else if(!stricmp(szAtom,"loadgame"))
			{
				MainMenu[2].X = NextValue();
				MainMenu[2].Y = NextValue();
				LoadGame.Image_Number = NextValue();
				LoadGame.Width = NextValue();
				LoadGame.Height = NextValue();
				LoadGame.Image_X = NextValue();
				LoadGame.Image_Y = NextValue();
				LoadGame.Mover_X = NextValue();
				LoadGame.Mover_Y = NextValue();
				LoadGame.background = NextValue();
				LoadGame.title = NextValue();
// changed RF063
				LoadGame.Animation = NextValue();
				LoadGame.AnimationOver = NextValue();
			}
			else if(!stricmp(szAtom,"savegame"))
			{
				MainMenu[3].X = NextValue();
				MainMenu[3].Y = NextValue();
				SaveGame.Image_Number = NextValue();
				SaveGame.Width = NextValue();
				SaveGame.Height = NextValue();
				SaveGame.Image_X = NextValue();
				SaveGame.Image_Y = NextValue();
				SaveGame.Mover_X = NextValue();
				SaveGame.Mover_Y = NextValue();
				SaveGame.background = NextValue();
				SaveGame.title = NextValue();
// changed RF063
				SaveGame.Animation = NextValue();
				SaveGame.AnimationOver = NextValue();
			}
			else if(!stricmp(szAtom,"options"))
			{
				MainMenu[4].X = NextValue();
				MainMenu[4].Y = NextValue();
				Options.Image_Number = NextValue();
				Options.Width = NextValue();
				Options.Height = NextValue();
				Options.Image_X = NextValue();
				Options.Image_Y = NextValue();
				Options.Mover_X = NextValue();
				Options.Mover_Y = NextValue();
				Options.background = NextValue();
				Options.title = NextValue();
// changed RF063
				Options.Animation = NextValue();
				Options.AnimationOver = NextValue();
			}
			else if(!stricmp(szAtom,"credits"))
			{
				MainMenu[5].X = NextValue();
				MainMenu[5].Y = NextValue();
				Credits.Image_Number = NextValue();
				Credits.Width = NextValue();
				Credits.Height = NextValue();
				Credits.Image_X = NextValue();
				Credits.Image_Y = NextValue();
				Credits.Mover_X = NextValue();
				Credits.Mover_Y = NextValue();
				Credits.background = NextValue();
				Credits.title = NextValue();
// changed RF063
				Credits.Animation = NextValue();
				Credits.AnimationOver = NextValue();
			}
			else if(!stricmp(szAtom,"quitmain"))
			{
				MainMenu[6].X = NextValue();
				MainMenu[6].Y = NextValue();
				QuitGame.Image_Number = NextValue();
				QuitGame.Width = NextValue();
				QuitGame.Height = NextValue();
				QuitGame.Image_X = NextValue();
				QuitGame.Image_Y = NextValue();
				QuitGame.Mover_X = NextValue();
				QuitGame.Mover_Y = NextValue();
// changed RF063
				QuitGame.Animation = NextValue();
				QuitGame.AnimationOver = NextValue();
			}
			else if(!stricmp(szAtom,"returntext"))
			{
				MainMenu[7].X = NextValue();
				MainMenu[7].Y = NextValue();
				return_text.Font = NextFont();
			}
// start multiplayer
			else if(!stricmp(szAtom,"hostnewgame"))
			{
				MultiMenu[0].X = NextValue();
				MultiMenu[0].Y = NextValue();
				HostNewGame.Image_Number = NextValue();
				HostNewGame.Width = NextValue();
				HostNewGame.Height = NextValue();
				HostNewGame.Image_X = NextValue();
				HostNewGame.Image_Y = NextValue();
				HostNewGame.Mover_X = NextValue();
				HostNewGame.Mover_Y = NextValue();
				HostNewGame.background = NextValue();
				HostNewGame.title = NextValue();
				HostNewGame.Animation = NextValue();
				HostNewGame.AnimationOver = NextValue();
			}
			else if(!stricmp(szAtom,"joinmultiplayergame"))
			{
				MultiMenu[1].X = NextValue();
				MultiMenu[1].Y = NextValue();
				JoinMultiplayerGame.Image_Number = NextValue();
				JoinMultiplayerGame.Width = NextValue();
				JoinMultiplayerGame.Height = NextValue();
				JoinMultiplayerGame.Image_X = NextValue();
				JoinMultiplayerGame.Image_Y = NextValue();
				JoinMultiplayerGame.Mover_X = NextValue();
				JoinMultiplayerGame.Mover_Y = NextValue();
				JoinMultiplayerGame.background = NextValue();
				JoinMultiplayerGame.title = NextValue();
				JoinMultiplayerGame.Animation = NextValue();
				JoinMultiplayerGame.AnimationOver = NextValue();
			}
			else if(!stricmp(szAtom,"quitmulti"))
			{
				MultiMenu[2].X = NextValue();
				MultiMenu[2].Y = NextValue();
				QuitMulti.Image_Number = NextValue();
				QuitMulti.Width = NextValue();
				QuitMulti.Height = NextValue();
				QuitMulti.Image_X = NextValue();
				QuitMulti.Image_Y = NextValue();
				QuitMulti.Mover_X = NextValue();
				QuitMulti.Mover_Y = NextValue();
// changed RF063
				QuitMulti.Animation = NextValue();
				QuitMulti.AnimationOver = NextValue();
			}
			else if(!stricmp(szAtom,"playerip_text"))
			{
				MultiMenu[3].X = NextValue();
				MultiMenu[3].Y = NextValue();
				PlayerIP_Text.Font = NextFont();				
			}
            else if(!stricmp(szAtom,"playerip1"))
			{            
				MultiMenu[4].X = NextValue();
				MultiMenu[4].Y = NextValue();
				PlayerIP1.Font = NextFont();
			}
			else if(!stricmp(szAtom,"ipadd_text"))
			{
				JoinGameMenu[0].X = NextValue();
				JoinGameMenu[0].Y = NextValue();
				IPAdd_Text.Font = NextFont();				
			}
			else if(!stricmp(szAtom,"ipaddress"))
			{
				JoinGameMenu[1].X = NextValue();
				JoinGameMenu[1].Y = NextValue();
				IPAddress.Font = NextFont();
				IPAddress.Set_X = JoinGameMenu[1].X;
				IPAddress.Set_Y = JoinGameMenu[1].Y;
				IPAddress.Width = NextValue();
			}
			else if(!stricmp(szAtom,"launchjoingame"))
			{
				JoinGameMenu[2].X = NextValue();
				JoinGameMenu[2].Y = NextValue();
				LaunchJoinGame.Image_Number = NextValue();
				LaunchJoinGame.Width = NextValue();
				LaunchJoinGame.Height = NextValue();
				LaunchJoinGame.Image_X = NextValue();
				LaunchJoinGame.Image_Y = NextValue();
				LaunchJoinGame.Mover_X = NextValue();
				LaunchJoinGame.Mover_Y = NextValue();
				LaunchJoinGame.Animation = NextValue();
				LaunchJoinGame.AnimationOver = NextValue();
			}
			else if(!stricmp(szAtom,"quitjoingame"))
			{
				JoinGameMenu[3].X = NextValue();
				JoinGameMenu[3].Y = NextValue();
				QuitJoinGame.Image_Number = NextValue();
				QuitJoinGame.Width = NextValue();
				QuitJoinGame.Height = NextValue();
				QuitJoinGame.Image_X = NextValue();
				QuitJoinGame.Image_Y = NextValue();
				QuitJoinGame.Mover_X = NextValue();
				QuitJoinGame.Mover_Y = NextValue();
				QuitJoinGame.Animation = NextValue();
				QuitJoinGame.AnimationOver = NextValue();
			}
            else if(!stricmp(szAtom,"multiplayerhelptext"))
			{
				JoinGameMenu[4].X = NextValue();
				JoinGameMenu[4].Y = NextValue();
				MultiplayerHelp_Text.Font = NextFont();				
			}
// end multiplayer
			else if(!stricmp(szAtom,"quitload"))
			{
				LoadGMenu[0].X = NextValue();
				LoadGMenu[0].Y = NextValue();
				QuitLoad.Image_Number = NextValue();
				QuitLoad.Width = NextValue();
				QuitLoad.Height = NextValue();
				QuitLoad.Image_X = NextValue();
				QuitLoad.Image_Y = NextValue();
				QuitLoad.Mover_X = NextValue();
				QuitLoad.Mover_Y = NextValue();
// changed RF063
				QuitLoad.Animation = NextValue();
				QuitLoad.AnimationOver = NextValue();
			}
			else if(!stricmp(szAtom,"load"))
			{
				LoadGMenu[1].X = NextValue();
				LoadGMenu[1].Y = NextValue();
				LoadSlot.Image_Number = NextValue();
				LoadSlot.Width = NextValue();
				LoadSlot.Height = NextValue();
				LoadSlot.Image_X = NextValue();
				LoadSlot.Image_Y = NextValue();
				LoadSlot.Mover_X = NextValue();
				LoadSlot.Mover_Y = NextValue();
// changed RF063
				LoadSlot.Animation = NextValue();
				LoadSlot.AnimationOver = NextValue();
			}
			else if(!stricmp(szAtom,"loadbox"))
			{
				LoadGMenu[2].X = NextValue();
				LoadGMenu[2].Y = NextValue();
				LoadBox.Image_Number = NextValue();
				LoadBox.Width = NextValue();
				LoadBox.Height = NextValue();
				LoadBox.Image_X = NextValue();
				LoadBox.Image_Y = NextValue();
				LoadBox.Start_X = NextValue();
				LoadBox.Start_Y = NextValue();
				LoadBox.Step = NextValue();
				LoadBox.Max_Show = NextValue();
				LoadBox.Corner_X = NextValue();
				LoadBox.Corner_Y = NextValue();
				LoadBox.Click_Width = NextValue();
				LoadBox.Rev_X = NextValue();
				LoadBox.Rev_Y = NextValue();
				LoadBox.Rev_Width = NextValue();
				LoadBox.Rev_Height = NextValue();
				LoadBox.Font = NextFont();
			}
			else if(!stricmp(szAtom,"loadbar"))
			{
				LoadGMenu[3].X = NextValue();
				LoadGMenu[3].Y = NextValue();
				LoadBar.Image_Number = NextValue();
				LoadBar.Up_Width = NextValue();
				LoadBar.Up_Height = NextValue();
				LoadBar.Up_Nor_X = NextValue();
				LoadBar.Up_Nor_Y = NextValue();
				LoadBar.Up_Lit_X = NextValue();
				LoadBar.Up_Lit_Y = NextValue();
				LoadBar.Up_Push_X = NextValue();
				LoadBar.Up_Push_Y = NextValue();
				LoadBar.Dwn_Width = NextValue();
				LoadBar.Dwn_Height = NextValue();
				LoadBar.Dwn_Nor_X = NextValue();
				LoadBar.Dwn_Nor_Y = NextValue();
				LoadBar.Dwn_Lit_X = NextValue();
				LoadBar.Dwn_Lit_Y = NextValue();
				LoadBar.Dwn_Push_X = NextValue();
				LoadBar.Dwn_Push_Y = NextValue();
				LoadBar.Up_X = NextValue();
				LoadBar.Up_Y = NextValue();
				LoadBar.Dwn_X = NextValue();
				LoadBar.Dwn_Y = NextValue();
				LoadBar.Show = LoadBox.Max_Show;
				LoadBar.AnimationUp = NextValue();
				LoadBar.AnimationUpOver = NextValue();
				LoadBar.AnimationUpPush = NextValue();
				LoadBar.AnimationDwn = NextValue();
				LoadBar.AnimationDwnOver = NextValue();
				LoadBar.AnimationDwnPush = NextValue();
			}
			else if(!stricmp(szAtom,"quitsave"))
			{
				SaveMenu[0].X = NextValue();
				SaveMenu[0].Y = NextValue();
				QuitSave.Image_Number = NextValue();
				QuitSave.Width = NextValue();
				QuitSave.Height = NextValue();
				QuitSave.Image_X = NextValue();
				QuitSave.Image_Y = NextValue();
				QuitSave.Mover_X = NextValue();
				QuitSave.Mover_Y = NextValue();
// changed RF063
				QuitSave.Animation = NextValue();
				QuitSave.AnimationOver = NextValue();
			}
			else if(!stricmp(szAtom,"save"))
			{
				SaveMenu[1].X = NextValue();
				SaveMenu[1].Y = NextValue();
				SaveSlot.Image_Number = NextValue();
				SaveSlot.Width = NextValue();
				SaveSlot.Height = NextValue();
				SaveSlot.Image_X = NextValue();
				SaveSlot.Image_Y = NextValue();
				SaveSlot.Mover_X = NextValue();
				SaveSlot.Mover_Y = NextValue();
// changed RF063
				SaveSlot.Animation = NextValue();
				SaveSlot.AnimationOver = NextValue();
			}
			else if(!stricmp(szAtom,"savebox"))
			{
				SaveMenu[2].X = NextValue();
				SaveMenu[2].Y = NextValue();
				SaveBox.Image_Number = NextValue();
				SaveBox.Width = NextValue();
				SaveBox.Height = NextValue();
				SaveBox.Image_X = NextValue();
				SaveBox.Image_Y = NextValue();
				SaveBox.Start_X = NextValue();
				SaveBox.Start_Y = NextValue();
				SaveBox.Step = NextValue();
				SaveBox.Max_Show = NextValue();
				SaveBox.Corner_X = NextValue();
				SaveBox.Corner_Y = NextValue();
				SaveBox.Click_Width = NextValue();
				SaveBox.Rev_X = NextValue();
				SaveBox.Rev_Y = NextValue();
				SaveBox.Rev_Width = NextValue();
				SaveBox.Rev_Height = NextValue();
				SaveBox.Font = NextFont();
			}
			else if(!stricmp(szAtom,"savebar"))
			{
				SaveMenu[3].X = NextValue();
				SaveMenu[3].Y = NextValue();
				SaveBar.Image_Number = NextValue();
				SaveBar.Up_Width = NextValue();
				SaveBar.Up_Height = NextValue();
				SaveBar.Up_Nor_X = NextValue();
				SaveBar.Up_Nor_Y = NextValue();
				SaveBar.Up_Lit_X = NextValue();
				SaveBar.Up_Lit_Y = NextValue();
				SaveBar.Up_Push_X = NextValue();
				SaveBar.Up_Push_Y = NextValue();
				SaveBar.Dwn_Width = NextValue();
				SaveBar.Dwn_Height = NextValue();
				SaveBar.Dwn_Nor_X = NextValue();
				SaveBar.Dwn_Nor_Y = NextValue();
				SaveBar.Dwn_Lit_X = NextValue();
				SaveBar.Dwn_Lit_Y = NextValue();
				SaveBar.Dwn_Push_X = NextValue();
				SaveBar.Dwn_Push_Y = NextValue();
				SaveBar.Up_X = NextValue();
				SaveBar.Up_Y = NextValue();
				SaveBar.Dwn_X = NextValue();
				SaveBar.Dwn_Y = NextValue();
				SaveBar.Show = SaveBox.Max_Show;
				SaveBar.AnimationUp = NextValue();
				SaveBar.AnimationUpOver = NextValue();
				SaveBar.AnimationUpPush = NextValue();
				SaveBar.AnimationDwn = NextValue();
				SaveBar.AnimationDwnOver = NextValue();
				SaveBar.AnimationDwnPush = NextValue();
			}
			else if(!stricmp(szAtom,"audio"))
			{
				OptionMenu[0].X = NextValue();
				OptionMenu[0].Y = NextValue();
				AudioItem.Image_Number = NextValue();
				AudioItem.Width = NextValue();
				AudioItem.Height = NextValue();
				AudioItem.Image_X = NextValue();
				AudioItem.Image_Y = NextValue();
				AudioItem.Mover_X = NextValue();
				AudioItem.Mover_Y = NextValue();
				AudioItem.background = NextValue();
				AudioItem.title = NextValue();
// changed RF063
				AudioItem.Animation = NextValue();
				AudioItem.AnimationOver = NextValue();
			}
			else if(!stricmp(szAtom,"video"))
			{
				OptionMenu[1].X = NextValue();
				OptionMenu[1].Y = NextValue();
				VideoItem.Image_Number = NextValue();
				VideoItem.Width = NextValue();
				VideoItem.Height = NextValue();
				VideoItem.Image_X = NextValue();
				VideoItem.Image_Y = NextValue();
				VideoItem.Mover_X = NextValue();
				VideoItem.Mover_Y = NextValue();
				VideoItem.background = NextValue();
				VideoItem.title = NextValue();
// changed RF063
				VideoItem.Animation = NextValue();
				VideoItem.AnimationOver = NextValue();
			}
			else if(!stricmp(szAtom,"control"))
			{
				OptionMenu[2].X = NextValue();
				OptionMenu[2].Y = NextValue();
				ControlItem.Image_Number = NextValue();
				ControlItem.Width = NextValue();
				ControlItem.Height = NextValue();
				ControlItem.Image_X = NextValue();
				ControlItem.Image_Y = NextValue();
				ControlItem.Mover_X = NextValue();
				ControlItem.Mover_Y = NextValue();
				ControlItem.background = NextValue();
				ControlItem.title = NextValue();
// changed RF063
				ControlItem.Animation = NextValue();
				ControlItem.AnimationOver = NextValue();
			}
			else if(!stricmp(szAtom,"debug"))
			{
				OptionMenu[3].X = NextValue();
				OptionMenu[3].Y = NextValue();
				DebugItem.Image_Number = NextValue();
				DebugItem.Width = NextValue();
				DebugItem.Height = NextValue();
				DebugItem.Image_X = NextValue();
				DebugItem.Image_Y = NextValue();
				DebugItem.Mover_X = NextValue();
				DebugItem.Mover_Y = NextValue();
				DebugItem.background = NextValue();
				DebugItem.title = NextValue();
// changed RF063
				DebugItem.Animation = NextValue();
				DebugItem.AnimationOver = NextValue();
			}
			else if(!stricmp(szAtom,"quitoptions"))
			{
				OptionMenu[4].X = NextValue();
				OptionMenu[4].Y = NextValue();
				QuitOption.Image_Number = NextValue();
				QuitOption.Width = NextValue();
				QuitOption.Height = NextValue();
				QuitOption.Image_X = NextValue();
				QuitOption.Image_Y = NextValue();
				QuitOption.Mover_X = NextValue();
				QuitOption.Mover_Y = NextValue();
// changed RF063
				QuitOption.Animation = NextValue();
				QuitOption.AnimationOver = NextValue();
			}
			else if(!stricmp(szAtom,"quitdebug"))
			{
				DebugMenu[0].X = NextValue();
				DebugMenu[0].Y = NextValue();
				QuitDebug.Image_Number = NextValue();
				QuitDebug.Width = NextValue();
				QuitDebug.Height = NextValue();
				QuitDebug.Image_X = NextValue();
				QuitDebug.Image_Y = NextValue();
				QuitDebug.Mover_X = NextValue();
				QuitDebug.Mover_Y = NextValue();
// changed RF063
				QuitDebug.Animation = NextValue();
				QuitDebug.AnimationOver = NextValue();
			}
			else if(!stricmp(szAtom,"debugbox"))
			{
				DebugMenu[1].X = NextValue();
				DebugMenu[1].Y = NextValue();
				box5.Image_Number = NextValue();
				box5.Width = NextValue();
				box5.Height = NextValue();
				box5.Image_X = NextValue();
				box5.Image_Y = NextValue();
				box5.Mover_X = NextValue();
				box5.Mover_Y = NextValue();
				box5.Set_X = NextValue();
				box5.Set_Y = NextValue();
				box5.Animation = NextValue();
				box5.AnimationOver = NextValue();
				box5.AnimationLit = NextValue();
			}
			else if(!stricmp(szAtom,"debugtext"))
			{
				DebugMenu[2].X = NextValue();
				DebugMenu[2].Y = NextValue();
				debug_text.Font = NextFont();
			}
			else if(!stricmp(szAtom,"fpsbox"))
			{
				DebugMenu[3].X = NextValue();
				DebugMenu[3].Y = NextValue();
				box6.Image_Number = NextValue();
				box6.Width = NextValue();
				box6.Height = NextValue();
				box6.Image_X = NextValue();
				box6.Image_Y = NextValue();
				box6.Mover_X = NextValue();
				box6.Mover_Y = NextValue();
				box6.Set_X = NextValue();
				box6.Set_Y = NextValue();
				box6.Animation = NextValue();
				box6.AnimationOver = NextValue();
				box6.AnimationLit = NextValue();
			}
			else if(!stricmp(szAtom,"fpstext"))
			{
				DebugMenu[4].X = NextValue();
				DebugMenu[4].Y = NextValue();
				fps_text.Font = NextFont();
			}
			else if(!stricmp(szAtom,"clipbox"))
			{
				DebugMenu[5].X = NextValue();
				DebugMenu[5].Y = NextValue();
				box7.Image_Number = NextValue();
				box7.Width = NextValue();
				box7.Height = NextValue();
				box7.Image_X = NextValue();
				box7.Image_Y = NextValue();
				box7.Mover_X = NextValue();
				box7.Mover_Y = NextValue();
				box7.Set_X = NextValue();
				box7.Set_Y = NextValue();
				box7.Animation = NextValue();
				box7.AnimationOver = NextValue();
				box7.AnimationLit = NextValue();
			}
			else if(!stricmp(szAtom,"cliptext"))
			{
				DebugMenu[6].X = NextValue();
				DebugMenu[6].Y = NextValue();
				clip_text.Font = NextFont();
			}
			else if(!stricmp(szAtom,"bbbox"))
			{
				DebugMenu[7].X = NextValue();
				DebugMenu[7].Y = NextValue();
				box8.Image_Number = NextValue();
				box8.Width = NextValue();
				box8.Height = NextValue();
				box8.Image_X = NextValue();
				box8.Image_Y = NextValue();
				box8.Mover_X = NextValue();
				box8.Mover_Y = NextValue();
				box8.Set_X = NextValue();
				box8.Set_Y = NextValue();
				box8.Animation = NextValue();
				box8.AnimationOver = NextValue();
				box8.AnimationLit = NextValue();
			}
			else if(!stricmp(szAtom,"bbtext"))
			{
				DebugMenu[8].X = NextValue();
				DebugMenu[8].Y = NextValue();
				bb_text.Font = NextFont();
			}
			else if(!stricmp(szAtom,"sebbbox"))
			{
				DebugMenu[9].X = NextValue();
				DebugMenu[9].Y = NextValue();
				box9.Image_Number = NextValue();
				box9.Width = NextValue();
				box9.Height = NextValue();
				box9.Image_X = NextValue();
				box9.Image_Y = NextValue();
				box9.Mover_X = NextValue();
				box9.Mover_Y = NextValue();
				box9.Set_X = NextValue();
				box9.Set_Y = NextValue();
				box9.Animation = NextValue();
				box9.AnimationOver = NextValue();
				box9.AnimationLit = NextValue();
			}
			else if(!stricmp(szAtom,"sebbtext"))
			{
				DebugMenu[10].X = NextValue();
				DebugMenu[10].Y = NextValue();
				sebb_text.Font = NextFont();
			}
			else if(!stricmp(szAtom,"quitcredit"))
			{
				CreditMenu[0].X = NextValue();
				CreditMenu[0].Y = NextValue();
				QuitCredit.Image_Number = NextValue();
				QuitCredit.Width = NextValue();
				QuitCredit.Height = NextValue();
				QuitCredit.Image_X = NextValue();
				QuitCredit.Image_Y = NextValue();
				QuitCredit.Mover_X = NextValue();
				QuitCredit.Mover_Y = NextValue();
// changed RF063
				QuitCredit.Animation = NextValue();
				QuitCredit.AnimationOver = NextValue();
			}
			else if(!stricmp(szAtom,"creditimg"))
			{
				CreditMenu[1].X = NextValue();
				CreditMenu[1].Y = NextValue();
				Credit_Img.Image_Number = NextValue();
				Credit_Img.Width = NextValue();
				Credit_Img.Height = NextValue();
				Credit_Img.Image_X = NextValue();
				Credit_Img.Image_Y = NextValue();
				Credit_Img.Animation = NextValue();
			}
			else if(!stricmp(szAtom,"quitaudio"))
			{
				AudioMenu[0].X = NextValue();
				AudioMenu[0].Y = NextValue();
				QuitAudio.Image_Number = NextValue();
				QuitAudio.Width = NextValue();
				QuitAudio.Height = NextValue();
				QuitAudio.Image_X = NextValue();
				QuitAudio.Image_Y = NextValue();
				QuitAudio.Mover_X = NextValue();
				QuitAudio.Mover_Y = NextValue();
// changed RF063
				QuitAudio.Animation = NextValue();
				QuitAudio.AnimationOver = NextValue();
			}
			else if(!stricmp(szAtom,"voltext"))
			{
				AudioMenu[1].X = NextValue();
				AudioMenu[1].Y = NextValue();
				vol_text.Font = NextFont();
			}
			else if(!stricmp(szAtom,"volimg"))
			{
				AudioMenu[2].X = NextValue();
				AudioMenu[2].Y = NextValue();
				Vol_Img.Image_Number = NextValue();
				Vol_Img.Width = NextValue();
				Vol_Img.Height = NextValue();
				Vol_Img.Image_X = NextValue();
				Vol_Img.Image_Y = NextValue();
				Vol_Img.Animation = NextValue();
			}
			else if(!stricmp(szAtom,"volslider"))
			{
				AudioMenu[3].X = NextValue();
				AudioMenu[3].Y = NextValue();
				Vol_Slide.Image_Number = NextValue();
				Vol_Slide.Width = NextValue();
				Vol_Slide.Height = NextValue();
				Vol_Slide.Image_X = NextValue();
				Vol_Slide.Image_Y = NextValue();
				Vol_Slide.Min_X = NextValue();
				Vol_Slide.Max_X = NextValue();
// changed RF063
				Vol_Slide.Animation = NextValue();
			}
			else if(!stricmp(szAtom,"mvoltext"))
			{
				AudioMenu[4].X = NextValue();
				AudioMenu[4].Y = NextValue();
				mvol_text.Font = NextFont();
			}
			else if(!stricmp(szAtom,"mvolimg"))
			{
				AudioMenu[5].X = NextValue();
				AudioMenu[5].Y = NextValue();
				mVol_Img.Image_Number = NextValue();
				mVol_Img.Width = NextValue();
				mVol_Img.Height = NextValue();
				mVol_Img.Image_X = NextValue();
				mVol_Img.Image_Y = NextValue();
				mVol_Img.Animation = NextValue();
			}
			else if(!stricmp(szAtom,"mvolslider"))
			{
				AudioMenu[6].X = NextValue();
				AudioMenu[6].Y = NextValue();
				mVol_Slide.Image_Number = NextValue();
				mVol_Slide.Width = NextValue();
				mVol_Slide.Height = NextValue();
				mVol_Slide.Image_X = NextValue();
				mVol_Slide.Image_Y = NextValue();
				mVol_Slide.Min_X = NextValue();
				mVol_Slide.Max_X = NextValue();
				mVol_Slide.Animation = NextValue();
			}
			else if(!stricmp(szAtom,"cdbox"))
			{
				AudioMenu[7].X = NextValue();
				AudioMenu[7].Y = NextValue();
				box0.Image_Number = NextValue();
				box0.Width = NextValue();
				box0.Height = NextValue();
				box0.Image_X = NextValue();
				box0.Image_Y = NextValue();
				box0.Mover_X = NextValue();
				box0.Mover_Y = NextValue();
				box0.Set_X = NextValue();
				box0.Set_Y = NextValue();
				box0.Animation = NextValue();
				box0.AnimationOver = NextValue();
				box0.AnimationLit = NextValue();
			}
			else if(!stricmp(szAtom,"cdtext"))
			{
				AudioMenu[8].X = NextValue();
				AudioMenu[8].Y = NextValue();
				cd_text.Font = NextFont();
			}
			else if(!stricmp(szAtom,"quitvideo"))
			{
				VideoMenu[0].X = NextValue();
				VideoMenu[0].Y = NextValue();
				QuitVideo.Image_Number = NextValue();
				QuitVideo.Width = NextValue();
				QuitVideo.Height = NextValue();
				QuitVideo.Image_X = NextValue();
				QuitVideo.Image_Y = NextValue();
				QuitVideo.Mover_X = NextValue();
				QuitVideo.Mover_Y = NextValue();
// changed RF063
				QuitVideo.Animation = NextValue();
				QuitVideo.AnimationOver = NextValue();
			}
			else if(!stricmp(szAtom,"gammatext"))
			{
				VideoMenu[1].X = NextValue();
				VideoMenu[1].Y = NextValue();
				gam_text.Font = NextFont();
			}
			else if(!stricmp(szAtom,"gammaimg"))
			{
				VideoMenu[2].X = NextValue();
				VideoMenu[2].Y = NextValue();
				Gamma_Img.Image_Number = NextValue();
				Gamma_Img.Width = NextValue();
				Gamma_Img.Height = NextValue();
				Gamma_Img.Image_X = NextValue();
				Gamma_Img.Image_Y = NextValue();
				Gamma_Img.Animation = NextValue();
			}
			else if(!stricmp(szAtom,"gammaslider"))
			{
				VideoMenu[3].X = NextValue();
				VideoMenu[3].Y = NextValue();
				Gamma_Slide.Image_Number = NextValue();
				Gamma_Slide.Width = NextValue();
				Gamma_Slide.Height = NextValue();
				Gamma_Slide.Image_X = NextValue();
				Gamma_Slide.Image_Y = NextValue();
				Gamma_Slide.Min_X = NextValue();
				Gamma_Slide.Max_X = NextValue();
// changed RF063
				Gamma_Slide.Animation = NextValue();
			}
			else if(!stricmp(szAtom,"detailtext"))
			{
				VideoMenu[4].X = NextValue();
				VideoMenu[4].Y = NextValue();
				det_text.Font = NextFont();
			}
			else if(!stricmp(szAtom,"detailimg"))
			{
				VideoMenu[5].X = NextValue();
				VideoMenu[5].Y = NextValue();
				Detail_Img.Image_Number = NextValue();
				Detail_Img.Width = NextValue();
				Detail_Img.Height = NextValue();
				Detail_Img.Image_X = NextValue();
				Detail_Img.Image_Y = NextValue();
				Detail_Img.Animation = NextValue();
			}
			else if(!stricmp(szAtom,"detailslider"))
			{
				VideoMenu[6].X = NextValue();
				VideoMenu[6].Y = NextValue();
				Detail_Slide.Image_Number = NextValue();
				Detail_Slide.Width = NextValue();
				Detail_Slide.Height = NextValue();
				Detail_Slide.Image_X = NextValue();
				Detail_Slide.Image_Y = NextValue();
				Detail_Slide.Min_X = NextValue();
				Detail_Slide.Max_X = NextValue();
// changed RF063
				Detail_Slide.Animation = NextValue();
			}
			else if(!stricmp(szAtom,"advance"))
			{
				ControlMenu[0].X = NextValue();
				ControlMenu[0].Y = NextValue();
				AdvancedItem.Image_Number = NextValue();
				AdvancedItem.Width = NextValue();
				AdvancedItem.Height = NextValue();
				AdvancedItem.Image_X = NextValue();
				AdvancedItem.Image_Y = NextValue();
				AdvancedItem.Mover_X = NextValue();
				AdvancedItem.Mover_Y = NextValue();
				AdvancedItem.background = NextValue();
				AdvancedItem.title = NextValue();
// changed RF063
				AdvancedItem.Animation = NextValue();
				AdvancedItem.AnimationOver = NextValue();
			}
			else if(!stricmp(szAtom,"quitcontrol"))
			{
				ControlMenu[1].X = NextValue();
				ControlMenu[1].Y = NextValue();
				QuitControl.Image_Number = NextValue();
				QuitControl.Width = NextValue();
				QuitControl.Height = NextValue();
				QuitControl.Image_X = NextValue();
				QuitControl.Image_Y = NextValue();
				QuitControl.Mover_X = NextValue();
				QuitControl.Mover_Y = NextValue();
// changed RF063
				QuitControl.Animation = NextValue();
				QuitControl.AnimationOver = NextValue();
			}
			else if(!stricmp(szAtom,"remapbox"))
			{
				ControlMenu[2].X = NextValue();
				ControlMenu[2].Y = NextValue();
				KeyMap.Image_Number = NextValue();
				KeyMap.Width = NextValue();
				KeyMap.Height = NextValue();
				KeyMap.Image_X = NextValue();
				KeyMap.Image_Y = NextValue();
				KeyMap.Start_X = NextValue();
				KeyMap.Start_Y = NextValue();
				KeyMap.Step = NextValue();
				KeyMap.Max_Show = NextValue();
				KeyMap.Corner_X = NextValue();
				KeyMap.Corner_Y = NextValue();
				KeyMap.Click_Width = NextValue();
				KeyMap.Key_X = NextValue();
				KeyMap.Rev_X = NextValue();
				KeyMap.Rev_Y = NextValue();
				KeyMap.Rev_Width = NextValue();
				KeyMap.Rev_Height = NextValue();
				KeyMap.RevC_X = NextValue();
				KeyMap.RevC_Y = NextValue();
				KeyMap.RevC_Width = NextValue();
				KeyMap.RevC_Height = NextValue();
				KeyMap.Font = NextFont();
			}
			else if(!stricmp(szAtom,"remapbar"))
			{
				ControlMenu[3].X = NextValue();
				ControlMenu[3].Y = NextValue();
				KeyMapBar.Image_Number = NextValue();
				KeyMapBar.Up_Width = NextValue();
				KeyMapBar.Up_Height = NextValue();
				KeyMapBar.Up_Nor_X = NextValue();
				KeyMapBar.Up_Nor_Y = NextValue();
				KeyMapBar.Up_Lit_X = NextValue();
				KeyMapBar.Up_Lit_Y = NextValue();
				KeyMapBar.Up_Push_X = NextValue();
				KeyMapBar.Up_Push_Y = NextValue();
				KeyMapBar.Dwn_Width = NextValue();
				KeyMapBar.Dwn_Height = NextValue();
				KeyMapBar.Dwn_Nor_X = NextValue();
				KeyMapBar.Dwn_Nor_Y = NextValue();
				KeyMapBar.Dwn_Lit_X = NextValue();
				KeyMapBar.Dwn_Lit_Y = NextValue();
				KeyMapBar.Dwn_Push_X = NextValue();
				KeyMapBar.Dwn_Push_Y = NextValue();
				KeyMapBar.Up_X = NextValue();
				KeyMapBar.Up_Y = NextValue();
				KeyMapBar.Dwn_X = NextValue();
				KeyMapBar.Dwn_Y = NextValue();
				KeyMapBar.Show = KeyMap.Max_Show;
				KeyMapBar.AnimationUp = NextValue();
				KeyMapBar.AnimationUpOver = NextValue();
				KeyMapBar.AnimationUpPush = NextValue();
				KeyMapBar.AnimationDwn = NextValue();
				KeyMapBar.AnimationDwnOver = NextValue();
				KeyMapBar.AnimationDwnPush = NextValue();
			}
			else if(!stricmp(szAtom,"default"))
			{
				ControlMenu[4].X = NextValue();
				ControlMenu[4].Y = NextValue();
				ResetKey.Image_Number = NextValue();
				ResetKey.Width = NextValue();
				ResetKey.Height = NextValue();
				ResetKey.Image_X = NextValue();
				ResetKey.Image_Y = NextValue();
				ResetKey.Mover_X = NextValue();
				ResetKey.Mover_Y = NextValue();
// changed RF063
				ResetKey.Animation = NextValue();
				ResetKey.AnimationOver = NextValue();
			}
			else if(!stricmp(szAtom,"quitadvance"))
			{
				AdvancedMenu[0].X = NextValue();
				AdvancedMenu[0].Y = NextValue();
				QuitAdvanced.Image_Number = NextValue();
				QuitAdvanced.Width = NextValue();
				QuitAdvanced.Height = NextValue();
				QuitAdvanced.Image_X = NextValue();
				QuitAdvanced.Image_Y = NextValue();
				QuitAdvanced.Mover_X = NextValue();
				QuitAdvanced.Mover_Y = NextValue();
// changed RF063
				QuitAdvanced.Animation = NextValue();
				QuitAdvanced.AnimationOver = NextValue();
			}
			else if(!stricmp(szAtom,"crossbox"))
			{
				AdvancedMenu[1].X = NextValue();
				AdvancedMenu[1].Y = NextValue();
				box1.Image_Number = NextValue();
				box1.Width = NextValue();
				box1.Height = NextValue();
				box1.Image_X = NextValue();
				box1.Image_Y = NextValue();
				box1.Mover_X = NextValue();
				box1.Mover_Y = NextValue();
				box1.Set_X = NextValue();
				box1.Set_Y = NextValue();
				box1.Animation = NextValue();
				box1.AnimationOver = NextValue();
				box1.AnimationLit = NextValue();
			}
			else if(!stricmp(szAtom,"crosstext"))
			{
				AdvancedMenu[2].X = NextValue();
				AdvancedMenu[2].Y = NextValue();
				xhair_text.Font = NextFont();
			}
			else if(!stricmp(szAtom,"reversebox"))
			{
				AdvancedMenu[3].X = NextValue();
				AdvancedMenu[3].Y = NextValue();
				box2.Image_Number = NextValue();
				box2.Width = NextValue();
				box2.Height = NextValue();
				box2.Image_X = NextValue();
				box2.Image_Y = NextValue();
				box2.Mover_X = NextValue();
				box2.Mover_Y = NextValue();
				box2.Set_X = NextValue();
				box2.Set_Y = NextValue();
				box2.Animation = NextValue();
				box2.AnimationOver = NextValue();
				box2.AnimationLit = NextValue();
			}
			else if(!stricmp(szAtom,"reversetext"))
			{
				AdvancedMenu[4].X = NextValue();
				AdvancedMenu[4].Y = NextValue();
				rev_text.Font = NextFont();
			}
			else if(!stricmp(szAtom,"filterbox"))
			{
				AdvancedMenu[5].X = NextValue();
				AdvancedMenu[5].Y = NextValue();
				box4.Image_Number = NextValue();
				box4.Width = NextValue();
				box4.Height = NextValue();
				box4.Image_X = NextValue();
				box4.Image_Y = NextValue();
				box4.Mover_X = NextValue();
				box4.Mover_Y = NextValue();
				box4.Set_X = NextValue();
				box4.Set_Y = NextValue();
				box4.Animation = NextValue();
				box4.AnimationOver = NextValue();
				box4.AnimationLit = NextValue();
			}
			else if(!stricmp(szAtom,"filtertext"))
			{
				AdvancedMenu[6].X = NextValue();
				AdvancedMenu[6].Y = NextValue();
				filter_text.Font = NextFont();
			}
			else if(!stricmp(szAtom,"senstext"))
			{
				AdvancedMenu[7].X = NextValue();
				AdvancedMenu[7].Y = NextValue();
				sens_text.Font = NextFont();
			}
			else if(!stricmp(szAtom,"sensimg"))
			{
				AdvancedMenu[8].X = NextValue();
				AdvancedMenu[8].Y = NextValue();
				Sens_Img.Image_Number = NextValue();
				Sens_Img.Width = NextValue();
				Sens_Img.Height = NextValue();
				Sens_Img.Image_X = NextValue();
				Sens_Img.Image_Y = NextValue();
				Sens_Img.Animation = NextValue();
			}
			else if(!stricmp(szAtom,"sensslider"))
			{
				AdvancedMenu[9].X = NextValue();
				AdvancedMenu[9].Y = NextValue();
				Sens_Slide.Image_Number = NextValue();
				Sens_Slide.Width = NextValue();
				Sens_Slide.Height = NextValue();
				Sens_Slide.Image_X = NextValue();
				Sens_Slide.Image_Y = NextValue();
				Sens_Slide.Min_X = NextValue();
				Sens_Slide.Max_X = NextValue();
// changed RF063
				Sens_Slide.Animation = NextValue();
			}
			else if(!stricmp(szAtom,"mouseclick"))
			{
				char file[256] = "menu\\";
				strcat(file, NextToken());
				CCD->OpenRFFile(&MainFS, kAudioFile, file, GE_VFILE_OPEN_READONLY);
				mouseclick = geSound_LoadSoundDef(CCD->Engine()->AudioSystem(), MainFS);
				geVFile_Close(MainFS);
			}
			else if(!stricmp(szAtom,"keyclick"))
			{
				char file[256] = "menu\\";
				strcat(file, NextToken());
				CCD->OpenRFFile(&MainFS, kAudioFile, file, GE_VFILE_OPEN_READONLY);
				keyclick = geSound_LoadSoundDef(CCD->Engine()->AudioSystem(), MainFS);
				geVFile_Close(MainFS);
			}
			else if(!stricmp(szAtom,"slideclick"))
			{
				char file[256] = "menu\\";
				strcat(file, NextToken());
				CCD->OpenRFFile(&MainFS, kAudioFile, file, GE_VFILE_OPEN_READONLY);
				slideclick = geSound_LoadSoundDef(CCD->Engine()->AudioSystem(), MainFS);
				geVFile_Close(MainFS);
			}
			else if(!stricmp(szAtom,"music"))
			{
				musictype = -1;
				char *musicname = NextToken();
				int len = strlen(musicname)-4;
				if(stricmp((musicname+len),".mid")==0)
				{
					strcpy(music, musicname);
// changed RF064
					theMIDIPlayer = new CMIDIAudio();
					if(theMIDIPlayer == NULL)
						CCD->ReportError("MIDI Player failed to instantiate", false);
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
			else if(!stricmp(szAtom,"select"))
			{
				SelectBack = NextValue();
				SelectTitle = NextValue();
			}
			else if(!stricmp(szAtom,"acceptchar"))
			{
				SelectMenu[0].X = NextValue();
				SelectMenu[0].Y = NextValue();
				AcceptSelect.Image_Number = NextValue();
				AcceptSelect.Width = NextValue();
				AcceptSelect.Height = NextValue();
				AcceptSelect.Image_X = NextValue();
				AcceptSelect.Image_Y = NextValue();
				AcceptSelect.Mover_X = NextValue();
				AcceptSelect.Mover_Y = NextValue();
				AcceptSelect.Animation = NextValue();
				AcceptSelect.AnimationOver = NextValue();
			}
			else if(!stricmp(szAtom,"cancelchar"))
			{
				SelectMenu[1].X = NextValue();
				SelectMenu[1].Y = NextValue();
				CancelSelect.Image_Number = NextValue();
				CancelSelect.Width = NextValue();
				CancelSelect.Height = NextValue();
				CancelSelect.Image_X = NextValue();
				CancelSelect.Image_Y = NextValue();
				CancelSelect.Mover_X = NextValue();
				CancelSelect.Mover_Y = NextValue();
				CancelSelect.Animation = NextValue();
				CancelSelect.AnimationOver = NextValue();
			}
			else if(!stricmp(szAtom,"prevchar"))
			{
				SelectMenu[2].X = NextValue();
				SelectMenu[2].Y = NextValue();
				PrevSelect.Image_Number = NextValue();
				PrevSelect.Width = NextValue();
				PrevSelect.Height = NextValue();
				PrevSelect.Image_X = NextValue();
				PrevSelect.Image_Y = NextValue();
				PrevSelect.Mover_X = NextValue();
				PrevSelect.Mover_Y = NextValue();
				PrevSelect.Animation = NextValue();
				PrevSelect.AnimationOver = NextValue();
			}
			else if(!stricmp(szAtom,"nextchar"))
			{
				SelectMenu[3].X = NextValue();
				SelectMenu[3].Y = NextValue();
				NextSelect.Image_Number = NextValue();
				NextSelect.Width = NextValue();
				NextSelect.Height = NextValue();
				NextSelect.Image_X = NextValue();
				NextSelect.Image_Y = NextValue();
				NextSelect.Mover_X = NextValue();
				NextSelect.Mover_Y = NextValue();
				NextSelect.Animation = NextValue();
				NextSelect.AnimationOver = NextValue();
			}
			else if(!stricmp(szAtom,"imagechar"))
			{
				SelectMenu[4].X = NextValue();
				SelectMenu[4].Y = NextValue();
			}
			else if(!stricmp(szAtom,"loadmsg"))
			{
				LoadFont = NextFont();
				strcpy(Loadmsg, strtok(NULL,"\n"));
			}
			else if(!stricmp(szAtom,"savemsg"))
			{
				SaveFont = NextFont();
				SavingTime = (float)NextValue();
				strcpy(Savemsg, strtok(NULL,"\n"));
			}
// end change RF063
			else if(!stricmp(szAtom,"difficult"))
			{
				DifficultBack = NextValue();
				DifficultTitle = NextValue();
			}
			else if(!stricmp(szAtom,"easy"))
			{
				DifficultMenu[0].X = NextValue();
				DifficultMenu[0].Y = NextValue();
				Difficultlow.Image_Number = NextValue();
				Difficultlow.Width = NextValue();
				Difficultlow.Height = NextValue();
				Difficultlow.Image_X = NextValue();
				Difficultlow.Image_Y = NextValue();
				Difficultlow.Mover_X = NextValue();
				Difficultlow.Mover_Y = NextValue();
				Difficultlow.Animation = NextValue();
				Difficultlow.AnimationOver = NextValue();
			}
			else if(!stricmp(szAtom,"normal"))
			{
				DifficultMenu[1].X = NextValue();
				DifficultMenu[1].Y = NextValue();
				Difficultmid.Image_Number = NextValue();
				Difficultmid.Width = NextValue();
				Difficultmid.Height = NextValue();
				Difficultmid.Image_X = NextValue();
				Difficultmid.Image_Y = NextValue();
				Difficultmid.Mover_X = NextValue();
				Difficultmid.Mover_Y = NextValue();
				Difficultmid.Animation = NextValue();
				Difficultmid.AnimationOver = NextValue();
			}
			else if(!stricmp(szAtom,"hard"))
			{
				DifficultMenu[2].X = NextValue();
				DifficultMenu[2].Y = NextValue();
				Difficulthi.Image_Number = NextValue();
				Difficulthi.Width = NextValue();
				Difficulthi.Height = NextValue();
				Difficulthi.Image_X = NextValue();
				Difficulthi.Image_Y = NextValue();
				Difficulthi.Mover_X = NextValue();
				Difficulthi.Mover_Y = NextValue();
				Difficulthi.Animation = NextValue();
				Difficulthi.AnimationOver = NextValue();
			}
			else
			{
				char szBug[256];
				sprintf(szBug, "Unknown Menu command %s", szInputLine);
				CCD->ReportError(szBug, false);
			}
		}
// changed RF063
		geVFile_Close(SecondFS);
// end change RF063
	}
	else
	{
		CCD->ReportError("Missing menu INI file", false);
		CCD->ShutdownLevel();
		delete CCD;
		MessageBox(NULL, "Missing menu INI file","Fatal Error", MB_OK);
		exit(-336);
	}

// changed RF063
	for(i=0;i<16;i++)
	{
		SavedGame[i].empty = 0;
		SavedGame[i].text = NULL;
	}

	SavedGame[0].text = strdup("<Slot 1>                     ");
	SavedGame[1].text = strdup("<Slot 2>                     ");
	SavedGame[2].text = strdup("<Slot 3>                     ");
	SavedGame[3].text = strdup("<Slot 4>                     ");
	SavedGame[4].text = strdup("<Slot 5>                     ");
	SavedGame[5].text = strdup("<Slot 6>                     ");
	SavedGame[6].text = strdup("<Slot 7>                     ");
	SavedGame[7].text = strdup("<Slot 8>                     ");
	SavedGame[8].text = strdup("<Slot 9>                     ");
	SavedGame[9].text = strdup("<Slot 10>                    ");
	SavedGame[10].text = strdup("<Slot 11>                    ");
	SavedGame[11].text = strdup("<Slot 12>                    ");
	SavedGame[12].text = strdup("<Slot 13>                    ");
	SavedGame[13].text = strdup("<Slot 14>                    ");
	SavedGame[14].text = strdup("<Slot 15>                    ");
// changed RF064
	if(CCD->GetCSelect())
	{
		CIniFile AttrFile("character.ini");
		if(!AttrFile.ReadFile())
		{
			CCD->ReportError("Can't open character initialization file", false);
			delete CCD;
			MessageBox(NULL, "Missing character INI file","Fatal Error", MB_OK);
			exit(-100);
		}
		MaxSelect = CurrentSelect = 0;
		CString KeyName = AttrFile.FindFirstKey();
		CString Type, Vector;
		char szName[64];
		while(KeyName != "")
		{
			strcpy(CharSelect[MaxSelect].Name,KeyName);
			CharSelect[MaxSelect].Bitmap = NULL;
			Type = AttrFile.GetValue(KeyName, "image");
			if(Type!="")
			{
				strcpy(szName,Type);
				CharSelect[MaxSelect].Bitmap = CreateFromFileName(szName);
				if(!CharSelect[MaxSelect].Bitmap)
				{
					char szBug[256];
					sprintf(szBug, "Bad character image file name %s", szName);
					CCD->ReportError(szBug, false);
					delete CCD;
					MessageBox(NULL, "Bad character image file name","Fatal Error", MB_OK);
					exit(-100);
				}
				geEngine_AddBitmap(CCD->Engine()->Engine(), CharSelect[MaxSelect].Bitmap);
				Type = AttrFile.GetValue(KeyName, "actorname");
				if(Type=="")
				{
					char szBug[256];
					sprintf(szBug, "Missing character actor name %s", szName);
					CCD->ReportError(szBug, false);
					delete CCD;
					MessageBox(NULL, "Missing character actor name","Fatal Error", MB_OK);
					exit(-100);
				}
				strcpy(CharSelect[MaxSelect].ActorName, Type);
				Vector = AttrFile.GetValue(KeyName, "actorrotation");
				if(Vector!="")
				{
					strcpy(szName,Vector);
					CharSelect[MaxSelect].Rotation = Extract(szName);
					CharSelect[MaxSelect].Rotation.X *= 0.0174532925199433f;
					CharSelect[MaxSelect].Rotation.Y *= 0.0174532925199433f;
					CharSelect[MaxSelect].Rotation.Z *= 0.0174532925199433f;
				}
				CharSelect[MaxSelect].ActorScale = (float)AttrFile.GetValueF(KeyName, "actorscale");
				Type = AttrFile.GetValue(KeyName, "animationspeed");
				CharSelect[MaxSelect].AnimSpeed = 1.0f;
				if(Type!="")
					CharSelect[MaxSelect].AnimSpeed = (float)AttrFile.GetValueF(KeyName, "animationspeed");
				CharSelect[MaxSelect].ShadowSize = (float)AttrFile.GetValueF(KeyName, "shadowsize");
				geVec3d FillColor = {255.0f, 255.0f, 255.0f};
				geVec3d AmbientColor = {255.0f, 255.0f, 255.0f};
				Vector = AttrFile.GetValue(KeyName, "fillcolor");
				if(Vector!="")
				{
					strcpy(szName,Vector);
					FillColor = Extract(szName);
				}
				Vector = AttrFile.GetValue(KeyName, "ambientcolor");
				if(Vector!="")
				{
					strcpy(szName,Vector);
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

				CharSelect[MaxSelect].Attribute[0] = '\0';
				Type = AttrFile.GetValue(KeyName, "attributefile");
				if(Type!="")
					strcpy(CharSelect[MaxSelect].Attribute, Type);
				CharSelect[MaxSelect].pSetup[0] = '\0'; 
				Type = AttrFile.GetValue(KeyName, "playersetupfile");
				if(Type!="")
					strcpy(CharSelect[MaxSelect].pSetup, Type);
				CharSelect[MaxSelect].Environment[0] = '\0';
				Type = AttrFile.GetValue(KeyName, "environmentfile");
				if(Type!="")
					strcpy(CharSelect[MaxSelect].Environment, Type);
				CharSelect[MaxSelect].Hud[0] = '\0';
				Type = AttrFile.GetValue(KeyName, "hudfile");
				if(Type!="")
					strcpy(CharSelect[MaxSelect].Hud, Type);
				Type = AttrFile.GetValue(KeyName, "speed");
				CharSelect[MaxSelect].Speed = -1.0f;
				if(Type!="")
					CharSelect[MaxSelect].Speed = (float)AttrFile.GetValueF(KeyName, "speed");
				Type = AttrFile.GetValue(KeyName, "jumpspeed");
				CharSelect[MaxSelect].JumpSpeed = -1.0f;
				if(Type!="")
					CharSelect[MaxSelect].JumpSpeed = (float)AttrFile.GetValueF(KeyName, "jumpspeed");
				Type = AttrFile.GetValue(KeyName, "stepheight");
				CharSelect[MaxSelect].StepHeight = -1.0f;
				if(Type!="")
					CharSelect[MaxSelect].StepHeight = (float)AttrFile.GetValueF(KeyName, "stepheight");
				CharSelect[MaxSelect].SlopeSlide = -1.0f;
				if(Type!="")
					CharSelect[MaxSelect].SlopeSlide = (float)AttrFile.GetValueF(KeyName, "slopeslide");
				CharSelect[MaxSelect].SlopeSpeed = -1.0f;
				if(Type!="")
					CharSelect[MaxSelect].SlopeSpeed = (float)AttrFile.GetValueF(KeyName, "slopespeed");

				MaxSelect +=1;
			}
			KeyName = AttrFile.FindNextKey();
		}
		if(MaxSelect==0)
		{
			CCD->ReportError("No characters defined", false);
			delete CCD;
			MessageBox(NULL, "No characters defined","Fatal Error", MB_OK);
			exit(-100);
		}
	}

  CMixer mixer(CCD->Engine()->WindowHandle(), MIXERLINE_COMPONENTTYPE_DST_SPEAKERS,
                  NO_SOURCE, MIXERCONTROL_CONTROLTYPE_VOLUME);
  if (mixer.IsOk())
	WinVol = mixer.GetControlValue();
// end change RF064
	//ScreenBmp = geBitmap_Create(640, 480, 1, GE_PIXELFORMAT_16BIT_4444_ARGB);
	//geBitmap_SetPreferredFormat(ScreenBmp,GE_PIXELFORMAT_16BIT_4444_ARGB);
	//geBitmap_ClearMips(ScreenBmp);
	//geEngine_AddBitmap(CCD->Engine()->Engine(), ScreenBmp);

// end change RF063
  // initalize all menu items that need it

  MenuInitalize(); 
}

CRFMenu::~CRFMenu()
{
	int i;

  // free click sounds
  geSound_FreeSoundDef(CCD->Engine()->AudioSystem(), mouseclick );
  geSound_FreeSoundDef(CCD->Engine()->AudioSystem(), keyclick );
  geSound_FreeSoundDef(CCD->Engine()->AudioSystem(), slideclick );
  // delete the bitmaps
  for(i=0; i<NUM_BACKGROUNDS;i++)
  {
	  if(Backgrounds[i] != (geBitmap *)NULL)
		  geBitmap_Destroy(&Backgrounds[i]);
  }

  for(i=0; i<NUM_IMAGES;i++)
  {
		if(Images[i] != (geBitmap *)NULL)
			geBitmap_Destroy(&Images[i]);
  }

  for(i=0; i<NUM_TITLES;i++)
  {
		if(Titles[i] != (geBitmap *)NULL)
			geBitmap_Destroy(&Titles[i]);
  }

  for(i=0; i<NUM_FONTS;i++)
  {
		if(MenuFont[i].Bitmap != (geBitmap *)NULL)
		{
			geBitmap_Destroy(&MenuFont[i].Bitmap);
		}
  }

// changed RF063
  for(i=0; i<NUM_ANIM;i++)
  {
		if(Animation[i] != NULL)
			delete Animation[i];
  }
// end change RF063
  geBitmap_Destroy(&Cursor);
  geBitmap_Destroy(&Crosshair);
  geBitmap_Destroy(&FCrosshair);
  //geBitmap_Destroy(&ScreenBmp);
  if(CCD->GetCSelect())
  {
	  if(MaxSelect>0)
	  {
		  for(i=0; i<MaxSelect;i++)
		  {
			  if(CharSelect[i].Bitmap)
				  geBitmap_Destroy(&CharSelect[i].Bitmap);
		  }
	  }
  }
  for(i=0;i<16;i++)
  {
	if(SavedGame[i].text)
		free(SavedGame[i].text);
  }
// changed RF064
  CMixer mixer(CCD->Engine()->WindowHandle(), MIXERLINE_COMPONENTTYPE_DST_SPEAKERS,
                  NO_SOURCE, MIXERCONTROL_CONTROLTYPE_VOLUME);
  if (mixer.IsOk())
	mixer.SetControlValue(WinVol);
	
	if(theMIDIPlayer != NULL)
		delete theMIDIPlayer;
	theMIDIPlayer = NULL;

// end change RF064
  return;
}

int CRFMenu::DoMenu(char *levelname)
{
  M_CameraRect.Left = 0;
  M_CameraRect.Right = CCD->Engine()->Width() - 1;
  M_CameraRect.Top = 0;
  M_CameraRect.Bottom = CCD->Engine()->Height() - 1;
  M_Camera = geCamera_Create(2.0f, &M_CameraRect);
  strcpy(LevelName, levelname);
  LoopOnce = 0;

  if(musictype!=-1)
  {
// changed RF064
	  if(musictype==1)
		MIDIPlayer()->Play(music,true);
	  else
		m_Streams->Play(true);
  }

  FadeSet(-1, 1.5f);

  int ret = ProcessMenu(MainMenu, MainBack, MainTitle);

  if(musictype!=-1)
  {
	  if(musictype==1)
		MIDIPlayer()->Stop();
	  else
	  {
		m_Streams->Delete();
		delete m_Streams;
	  }
  }
// end change RF064
  FILE *fd = CCD->OpenRFFile(kInstallFile, "setup.ini", "wb");
  if(fd == NULL)
	CCD->ReportError("CRFMenu: can't create settings file", false);
  else
  {
	  fwrite(&Gamma_Slide.Current, sizeof(int), 1, fd);
	  fwrite(&Vol_Slide.Current, sizeof(int), 1, fd);
	  fwrite(&Sens_Slide.Current, sizeof(int), 1, fd);
	  fwrite(&box0.Current, sizeof(int), 1, fd);
	  fwrite(&box1.Current, sizeof(int), 1, fd);
	  fwrite(&box2.Current, sizeof(int), 1, fd);
	  fwrite(&box4.Current, sizeof(int), 1, fd);
	  fwrite(&Detail_Slide.Current, sizeof(int), 1, fd);
	  fwrite(&mVol_Slide.Current, sizeof(int), 1, fd);
	  fclose(fd);
  }	 

  geCamera_Destroy(&M_Camera);
  return ret;
}

int CRFMenu::ProcessMenu(MenuItem *Menu, int Background_Number, int Title_Number)
{
  int x, y, i, max, click, temp, slide_click, box_click, scroll_click, remap_click, lsbox_click;
  int slide_x, escapeon, scroll_dir, remap_line, focus, remapf, lsbox_line;
  geBitmap_Info	BmpInfo;
  geRect 	 BitRect;
  POINT temppos;
  Clickable *data;
  Image *idata;
  Slider *sdata;
  Box *bdata;
  Text *tdata;
  Remap *rdata;
  ScrollBar *scdata;
  LSBox *lrdata;
// start multiplayer
  int textedit_click;
  TextEdit *tedata;
// end multiplayer

  geBitmap_GetInfo(Backgrounds[Background_Number], &BmpInfo, NULL);
  x = (CCD->Engine()->Width() - 640) / 2;
  y = (CCD->Engine()->Height() - 480) / 2;
  int bx = 0;
  int by = 0;
  if(CCD->Engine()->Width() > BmpInfo.Width)
	  bx = (CCD->Engine()->Width() - BmpInfo.Width) / 2;
  if(CCD->Engine()->Height() > BmpInfo.Height)
	  by = (CCD->Engine()->Height() - BmpInfo.Height) / 2;
  max=0;
  while(Menu[max].Type != END_LIST)
	  max+=1;

  click=-1;
  slide_click=-1;
  slide_x=-1;
  box_click=-1;
  escapeon=0;
  scroll_click=-1;
  scroll_dir = 0;
  remap_click=-1;
  remapf = -1;
  focus = -1;
  lsbox_click = -1;

// start multiplayer
  textedit_click=-1;
// end multiplayer

  POINT pos;
  if(!CCD->Engine()->FullScreen())
  {
	RECT client;
	GetClientRect(CCD->Engine()->WindowHandle(),&client);
	pos.x =client.left;
	pos.y = client.top;
	ClientToScreen(CCD->Engine()->WindowHandle(),&pos);
  }

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

    geEngine_BeginFrame(CCD->Engine()->Engine(), M_Camera, GE_TRUE);
// changed RF063
	DrawBitmap(Backgrounds[Background_Number], NULL, bx, by );

	if(MTitles[Title_Number].Animation<0 || Animation[MTitles[Title_Number].Animation]==NULL)
	{
		BitRect.Left=MTitles[Title_Number].Image_X;
		BitRect.Top=MTitles[Title_Number].Image_Y;
		BitRect.Right=MTitles[Title_Number].Image_X+MTitles[Title_Number].Width;
		BitRect.Bottom=MTitles[Title_Number].Image_Y+MTitles[Title_Number].Height;
		DrawBitmap(Titles[MTitles[Title_Number].Image_Number], &BitRect, MTitles[Title_Number].X+x, MTitles[Title_Number].Y+y );
	}
	else
	{
		geBitmap *theBmp = Animation[MTitles[Title_Number].Animation]->NextFrame(true);
		DrawBitmap(theBmp, NULL, MTitles[Title_Number].X+x, MTitles[Title_Number].Y+y );
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

    for(i=0;i<max;i++)
	{
// changed RF063
	   if(Menu[i].Type==CLICKABLE || Menu[i].Type==EXIT_MENU || Menu[i].Type==CANCEL_MENU)
	   {
		   data=(Clickable *)Menu[i].data;
		   if((temppos.x>=Menu[i].X+x) && (temppos.x<=(Menu[i].X+x+data->Width)) && (temppos.y>=Menu[i].Y+y) && (temppos.y<=(Menu[i].Y+y+data->Height)) && slide_click==-1)
		   {
			   if(data->Action==2 && ingame==0)
			   {
				   if(data->Animation<0 || Animation[data->Animation]==NULL)
				   {
					   BitRect.Left=data->Image_X;
					   BitRect.Top=data->Image_Y;
					   BitRect.Right=data->Image_X+data->Width;
					   BitRect.Bottom=data->Image_Y+data->Height;
					   DrawBitmap(Images[data->Image_Number], &BitRect, Menu[i].X+x, Menu[i].Y+y );
				   }
				   else
				   {
					   geBitmap *theBmp = Animation[data->Animation]->NextFrame(true);
					   DrawBitmap(theBmp, NULL, Menu[i].X+x, Menu[i].Y+y );
				   }
			   }
			   else
			   {
				   if(data->AnimationOver<0 || Animation[data->AnimationOver]==NULL)
				   {
					   BitRect.Left=data->Mover_X;
					   BitRect.Top=data->Mover_Y;
					   BitRect.Right=data->Mover_X+data->Width;
					   BitRect.Bottom=data->Mover_Y+data->Height;
					   DrawBitmap(Images[data->Image_Number], &BitRect, Menu[i].X+x, Menu[i].Y+y );
				   }
				   else
				   {
					   geBitmap *theBmp = Animation[data->AnimationOver]->NextFrame(true);
					   DrawBitmap(theBmp, NULL, Menu[i].X+x, Menu[i].Y+y );
				   }
				   if((GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0 && focus == -1)
					   click=i;
			   }
		   }
		   else
		   {
			   if(data->Animation<0 || Animation[data->Animation]==NULL)
			   {
				   BitRect.Left=data->Image_X;
				   BitRect.Top=data->Image_Y;
				   BitRect.Right=data->Image_X+data->Width;
				   BitRect.Bottom=data->Image_Y+data->Height;
				   DrawBitmap(Images[data->Image_Number], &BitRect, Menu[i].X+x, Menu[i].Y+y );
			   }
			   else
			   {
				   geBitmap *theBmp = Animation[data->Animation]->NextFrame(true);
				   DrawBitmap(theBmp, NULL, Menu[i].X+x, Menu[i].Y+y );
			   }
		   }
	   }
// end change RF063
	   if(Menu[i].Type==IMAGE)
	   {
		   idata=(Image *)Menu[i].data;
		   if(idata->Animation<0 || Animation[idata->Animation]==NULL)
		   {
			   BitRect.Left=idata->Image_X;
			   BitRect.Top=idata->Image_Y;
			   BitRect.Right=idata->Image_X+idata->Width;
			   BitRect.Bottom=idata->Image_Y+idata->Height;
			   DrawBitmap(Images[idata->Image_Number], &BitRect, Menu[i].X+x, Menu[i].Y+y );
		   }
		   else
		   {
			   geBitmap *theBmp = Animation[idata->Animation]->NextFrame(true);
			   DrawBitmap(theBmp, NULL, Menu[i].X+x, Menu[i].Y+y );
		   }
	   }
// changed RF063
	   if(Menu[i].Type==CHARIMAGE)
	   {
		   DrawBitmap(CharSelect[CurrentSelect].Bitmap, NULL, Menu[i].X+x, Menu[i].Y+y );
	   }
// end change RF063
	   if(Menu[i].Type==SLIDER)
	   {
		   sdata=(Slider *)Menu[i].data;
		   temp=sdata->Current;
// changed RF063
		   if(sdata->Animation<0 || Animation[sdata->Animation]==NULL)
		   {
			   BitRect.Left=sdata->Image_X;
			   BitRect.Top=sdata->Image_Y;
			   BitRect.Right=sdata->Image_X+sdata->Width;
			   BitRect.Bottom=sdata->Image_Y+sdata->Height;
			   DrawBitmap(Images[sdata->Image_Number], &BitRect, Menu[i].X+x+temp, Menu[i].Y+y );
		   }
		   else
		   {
			   geBitmap *theBmp = Animation[sdata->Animation]->NextFrame(true);
			   DrawBitmap(theBmp, NULL, Menu[i].X+x+temp, Menu[i].Y+y );
		   }
// end change RF063
		   if((temppos.x>=Menu[i].X+x+temp) && (temppos.x<=(Menu[i].X+x+temp+sdata->Width)) && (temppos.y>=Menu[i].Y+y) && (temppos.y<=(Menu[i].Y+y+data->Height)) && slide_click==-1)
		   {
			   if((GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0 && focus == -1)
				   slide_click=i;
			   if(slide_x==-1)
				   slide_x=temppos.x;
		   }
	   }
	   if(Menu[i].Type==BOX)
	   {
		   bdata=(Box *)Menu[i].data;
		   if((temppos.x>=Menu[i].X+x) && (temppos.x<=(Menu[i].X+x+bdata->Width)) && (temppos.y>=Menu[i].Y+y) && (temppos.y<=(Menu[i].Y+y+bdata->Height)) && slide_click==-1 && bdata->Current==0)
		   {
			   if(bdata->AnimationOver<0 || Animation[bdata->AnimationOver]==NULL)
			   {
				   BitRect.Left=bdata->Mover_X;
				   BitRect.Top=bdata->Mover_Y;
				   BitRect.Right=bdata->Mover_X+bdata->Width;
				   BitRect.Bottom=bdata->Mover_Y+bdata->Height;
				   DrawBitmap(Images[bdata->Image_Number], &BitRect, Menu[i].X+x, Menu[i].Y+y );
			   }
			   else
			   {
				   geBitmap *theBmp = Animation[bdata->AnimationOver]->NextFrame(true);
				   DrawBitmap(theBmp, NULL, Menu[i].X+x, Menu[i].Y+y );
			   }
			   if((GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0 && focus == -1)
				   box_click=i;
		   }
		   else
		   {
			   if((temppos.x>=Menu[i].X+x) && (temppos.x<=(Menu[i].X+x+bdata->Width)) && (temppos.y>=Menu[i].Y+y) && (temppos.y<=(Menu[i].Y+y+bdata->Height)) && slide_click==-1)
			   {
				   if((GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0 && focus == -1)
					   box_click=i;
			   }
			   if(bdata->Current==0)
			   {
				   if(bdata->Animation<0 || Animation[bdata->Animation]==NULL)
				   {
					   BitRect.Left=bdata->Image_X;
					   BitRect.Top=bdata->Image_Y;
					   BitRect.Right=bdata->Image_X+bdata->Width;
					   BitRect.Bottom=bdata->Image_Y+bdata->Height;
					   DrawBitmap(Images[bdata->Image_Number], &BitRect, Menu[i].X+x, Menu[i].Y+y );
				   }
				   else
				   {
					   geBitmap *theBmp = Animation[bdata->Animation]->NextFrame(true);
					   DrawBitmap(theBmp, NULL, Menu[i].X+x, Menu[i].Y+y );
				   }
			   }
			   else
			   {
				   if(bdata->AnimationLit<0 || Animation[bdata->AnimationLit]==NULL)
				   {
					   BitRect.Left=bdata->Set_X;
					   BitRect.Top=bdata->Set_Y;
					   BitRect.Right=bdata->Set_X+bdata->Width;
					   BitRect.Bottom=bdata->Set_Y+bdata->Height;
					   DrawBitmap(Images[bdata->Image_Number], &BitRect, Menu[i].X+x, Menu[i].Y+y );
				   }
				   else
				   {
					   geBitmap *theBmp = Animation[bdata->AnimationLit]->NextFrame(true);
					   DrawBitmap(theBmp, NULL, Menu[i].X+x, Menu[i].Y+y );
				   }
			   }
		   }
	   }
       if(Menu[i].Type==TEXT)
	   {
         char *s;

         tdata=(Text *)Menu[i].data;
		 s=tdata->text;
		 if(tdata->ingame==0 || ingame==1)
			MFontRect(s, tdata->Font, Menu[i].X+x, Menu[i].Y+y);
       }
// start multiplayer
	   if(Menu[i].Type==TEXTEDIT)
	   {         
		   tedata=(TextEdit *)Menu[i].data;
		   Keyname *KTEdata = tedata->keyname;
		   
		   int32	Size;
		   
		   if( (temppos.x>=Menu[i].X+x) && (temppos.x<=Menu[i].X+x+tedata->Width) && (temppos.y>=Menu[i].Y+y) && (temppos.y<=Menu[i].Y+y+30) ) 
		   {
			   if((GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0 && textedit_click == -1)
			   {				    
				   strcpy(tedata->text,"_");									
				   geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), keyclick, 0.99f, 0.5f, 1.0f, false);
				   textedit_click=i;	
				   focus=-1;
			   }
		   }
		   if((GetAsyncKeyState(VK_RETURN) & 0x8000) != 0 && textedit_click == i)
		   {
			   //focus=-1;		 
			   Size = strlen( tedata->text );		
			   tedata->text[Size-1] = '\0';
			   strcpy(ServerIP,tedata->text);
			   geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), keyclick, 0.99f, 0.5f, 1.0f, false);
			   textedit_click=-1;
		   }
		   
		   if( textedit_click == i && focus==-1)
		   {
			   // delete a character...				
			   if ( ((GetAsyncKeyState(VK_BACK) & 0x8000) != 0) || ((GetAsyncKeyState(VK_LEFT) & 0x8000) != 0) )			
			   {		
				   
				   
				   // do nothing if there are no more characters left
				   Size = strlen( tedata->text );
				   if ( Size > 2 )
				   {
					   // delete as character					
					   tedata->text[Size-2] = '\0';
					   strcat(tedata->text,"_");				
					   geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), mouseclick, 0.99f, 0.0f, 1.0f, false);
				   }
			   }		
			   
			   int keybrd = CCD->Input()->GetKeyboardInput();
			   if(keybrd !=-1)
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
				   if(s!=NULL)
				   {		
					   // do nothing is there is no more room to add characters
					   Size = strlen( tedata->text );
					   if ( Size+2 <= sizeof( tedata->text ) )
					   {	
						   tedata->text[Size-1] = s[0];						
						   tedata->text[Size] = '\0';
						   strcat(tedata->text,"_");									
						   
						   //strcat(tedata->text,s);
						   geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), keyclick, 0.99f, 0.5f, 1.0f, false);
						   //textedit_click=i;
					   }
				   }
			   }
		   }
		   MFontRect(tedata->text, tedata->Font, tedata->Set_X+x, tedata->Set_Y+y);
       }
// end multiplayer
	   if(Menu[i].Type==REMAP)
	   {
		 rdata=(Remap *)Menu[i].data;
		 Keydef *Kdata = rdata->keydef;
		 Keyname *KNdata = rdata->keyname;
		 if(rdata->Max==-1)
		 {
           rdata->Max=0;
		   while(Kdata[rdata->Max].text!=NULL)
		   {
				rdata->Max+=1;
		   }
		 }
		 BitRect.Left=rdata->Image_X;
         BitRect.Top=rdata->Image_Y;
	     BitRect.Right=rdata->Image_X+rdata->Width;
	     BitRect.Bottom=rdata->Image_Y+rdata->Height;
		 DrawBitmap(Images[rdata->Image_Number], &BitRect, Menu[i].X+x, Menu[i].Y+y );
		 if((GetAsyncKeyState(VK_RETURN) & 0x8000) == 0 && focus == i)
		 {
			int keybrd = CCD->Input()->GetKeyboardInput();
			if(keybrd !=-1)
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
			  if(s!=NULL)
			  {
				 geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), keyclick, 0.99f, 0.5f, 1.0f, false);
				 CCD->Input()->SetKeyAction(CCD->Input()->GetCodes(Kdata[remapf].action), RGF_K_NOACTION);
				 CCD->Input()->SetKeyAction(keybrd, Kdata[remapf].action);
				 focus = -1;
			  }
			}
		 }
		 int tempstep=0;
		 for(int j=rdata->Start;j<rdata->Max;j++)
		 {
			if(j<(rdata->Start+rdata->Max_Show))
			{
			  char *s;

			  if(j==rdata->Current)
			  {
				 if((GetAsyncKeyState(VK_RETURN) & 0x8000) != 0 && focus == -1)
				 {
					geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), keyclick, 0.99f, 0.5f, 1.0f, false);
					remapf = j;
					focus = i;
				 }
				 if(focus == i)
				 {
					BitRect.Left=rdata->RevC_X;
					BitRect.Top=rdata->RevC_Y;
					BitRect.Right=rdata->RevC_X+rdata->RevC_Width;
					BitRect.Bottom=rdata->RevC_Y+rdata->RevC_Height;
				 }
				 else
				 {
					BitRect.Left=rdata->Rev_X;
					BitRect.Top=rdata->Rev_Y;
					BitRect.Right=rdata->Rev_X+rdata->Rev_Width;
					BitRect.Bottom=rdata->Rev_Y+rdata->Rev_Height;
				 }
				 DrawBitmap(Images[rdata->Image_Number], &BitRect, Menu[i].X+x+rdata->Corner_X, Menu[i].Y+y+rdata->Corner_Y+tempstep );
			  }
              s=Kdata[j].text;
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
			  MFontRect(s, rdata->Font, Menu[i].X+x+rdata->Start_X+rdata->Key_X, Menu[i].Y+y+rdata->Start_Y+tempstep);
			}
			tempstep+=rdata->Step;
		 }
		 if((temppos.x>=Menu[i].X+x+rdata->Corner_X) && (temppos.x<=(Menu[i].X+x+rdata->Corner_X+rdata->Click_Width)) && (temppos.y>=Menu[i].Y+y+rdata->Corner_Y) && (temppos.y<=(Menu[i].Y+y+rdata->Corner_Y+(rdata->Max_Show*rdata->Step))) && slide_click==-1)
			 {
			   if((GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0 && focus == -1)
			   {
				 remap_click=i;
				 remap_line = (temppos.y-(Menu[i].Y+y+rdata->Corner_Y))/rdata->Step;
			   }
			 }
	   }

	   if(Menu[i].Type==SCROLLBAR)
	   {
		   geBitmap *theBmp;
		   bool up = false;
		   bool dwn = false;
		   scdata=(ScrollBar *)Menu[i].data;
		   if(scdata->AnimationUp<0 || Animation[scdata->AnimationUp]==NULL)
		   {
			   BitRect.Left=scdata->Up_Nor_X;
			   BitRect.Top=scdata->Up_Nor_Y;
			   BitRect.Right=scdata->Up_Nor_X+scdata->Up_Width;
			   BitRect.Bottom=scdata->Up_Nor_Y+scdata->Up_Height;
			   DrawBitmap(Images[scdata->Image_Number], &BitRect, Menu[i].X+x+scdata->Up_X, Menu[i].Y+y+scdata->Up_Y );
		   }
		   else
		   {
			   up = true;
		   }
		   if(scdata->AnimationDwn<0 || Animation[scdata->AnimationDwn]==NULL)
		   {
			   BitRect.Left=scdata->Dwn_Nor_X;
			   BitRect.Top=scdata->Dwn_Nor_Y;
			   BitRect.Right=scdata->Dwn_Nor_X+scdata->Dwn_Width;
			   BitRect.Bottom=scdata->Dwn_Nor_Y+scdata->Dwn_Height;
			   DrawBitmap(Images[scdata->Image_Number], &BitRect, Menu[i].X+x+scdata->Dwn_X, Menu[i].Y+y+scdata->Dwn_Y );
		   }
		   else
		   {
			   dwn = true;
		   }
		   
		   if((temppos.x>=Menu[i].X+x+scdata->Up_X) && (temppos.x<=(Menu[i].X+x+scdata->Up_X+scdata->Up_Width)) && (temppos.y>=Menu[i].Y+y+scdata->Up_Y) && (temppos.y<=(Menu[i].Y+y+scdata->Up_Y+scdata->Up_Height)) && slide_click==-1)
		   {
			   if((GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0 && focus == -1)
			   {
				   if(scdata->AnimationUpPush<0 || Animation[scdata->AnimationUpPush]==NULL)
				   {
					   BitRect.Left=scdata->Up_Push_X;
					   BitRect.Top=scdata->Up_Push_Y;
					   BitRect.Right=scdata->Up_Push_X+scdata->Up_Width;
					   BitRect.Bottom=scdata->Up_Push_Y+scdata->Up_Height;
					   DrawBitmap(Images[scdata->Image_Number], &BitRect, Menu[i].X+x+scdata->Up_X, Menu[i].Y+y+scdata->Up_Y );
				   }
				   else
				   {
					   theBmp = Animation[scdata->AnimationUpPush]->NextFrame(true);
					   DrawBitmap(theBmp, NULL, Menu[i].X+x+scdata->Up_X, Menu[i].Y+y+scdata->Up_Y );
				   }
				   up = false;
				   scroll_click=i;
				   scroll_dir = 0;
			   }
			   else
			   {
				   if(scdata->AnimationUpOver<0 || Animation[scdata->AnimationUpOver]==NULL)
				   {
					   BitRect.Left=scdata->Up_Lit_X;
					   BitRect.Top=scdata->Up_Lit_Y;
					   BitRect.Right=scdata->Up_Lit_X+scdata->Up_Width;
					   BitRect.Bottom=scdata->Up_Lit_Y+scdata->Up_Height;
					   DrawBitmap(Images[scdata->Image_Number], &BitRect, Menu[i].X+x+scdata->Up_X, Menu[i].Y+y+scdata->Up_Y );
				   }
				   else
				   {
					   theBmp = Animation[scdata->AnimationUpOver]->NextFrame(true);
					   DrawBitmap(theBmp, NULL, Menu[i].X+x+scdata->Up_X, Menu[i].Y+y+scdata->Up_Y );
				   }
				   up = false;
			   }
		   }
		   if(up)
		   {
			   theBmp = Animation[scdata->AnimationUp]->NextFrame(true);
			   DrawBitmap(theBmp, NULL, Menu[i].X+x+scdata->Up_X, Menu[i].Y+y+scdata->Up_Y );
		   }
		   if((temppos.x>=Menu[i].X+x+scdata->Dwn_X) && (temppos.x<=(Menu[i].X+x+scdata->Dwn_X+scdata->Dwn_Width)) && (temppos.y>=Menu[i].Y+y+scdata->Dwn_Y) && (temppos.y<=(Menu[i].Y+y+scdata->Dwn_Y+scdata->Dwn_Height)) && slide_click==-1)
		   {
			   if((GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0 && focus == -1)
			   {
				   if(scdata->AnimationDwnPush<0 || Animation[scdata->AnimationDwnPush]==NULL)
				   {
					   BitRect.Left=scdata->Dwn_Push_X;
					   BitRect.Top=scdata->Dwn_Push_Y;
					   BitRect.Right=scdata->Dwn_Push_X+scdata->Dwn_Width;
					   BitRect.Bottom=scdata->Dwn_Push_Y+scdata->Dwn_Height;
					   DrawBitmap(Images[scdata->Image_Number], &BitRect, Menu[i].X+x+scdata->Dwn_X, Menu[i].Y+y+scdata->Dwn_Y );
				   }
				   else
				   {
					   theBmp = Animation[scdata->AnimationDwnPush]->NextFrame(true);
					   DrawBitmap(theBmp, NULL, Menu[i].X+x+scdata->Dwn_X, Menu[i].Y+y+scdata->Dwn_Y );
				   }
				   dwn = false;
				   scroll_click=i;
				   scroll_dir = 1;
			   }
			   else
			   {
				   if(scdata->AnimationDwnOver<0 || Animation[scdata->AnimationDwnOver]==NULL)
				   {
					   BitRect.Left=scdata->Dwn_Lit_X;
					   BitRect.Top=scdata->Dwn_Lit_Y;
					   BitRect.Right=scdata->Dwn_Lit_X+scdata->Dwn_Width;
					   BitRect.Bottom=scdata->Dwn_Lit_Y+scdata->Dwn_Height;
					   DrawBitmap(Images[scdata->Image_Number], &BitRect, Menu[i].X+x+scdata->Dwn_X, Menu[i].Y+y+scdata->Dwn_Y );
				   }
				   else
				   {
					   theBmp = Animation[scdata->AnimationDwnOver]->NextFrame(true);
					   DrawBitmap(theBmp, NULL, Menu[i].X+x+scdata->Dwn_X, Menu[i].Y+y+scdata->Dwn_Y );
				   }
				   dwn = false;
			   }
		   } 
		   if(dwn)
		   {
			   theBmp = Animation[scdata->AnimationDwn]->NextFrame(true);
			   DrawBitmap(theBmp, NULL, Menu[i].X+x+scdata->Dwn_X, Menu[i].Y+y+scdata->Dwn_Y );
		   }
	   }

	   if(Menu[i].Type==LSBOX)
	   {
		 lrdata=(LSBox *)Menu[i].data;
		 Savedef *Kdata = lrdata->text;
		 if(lrdata->Max==-1)
		 {
           lrdata->Max=0;
		   while(Kdata[lrdata->Max].text!=NULL)
		   {
				lrdata->Max+=1;
		   }
		 } 
		 BitRect.Left=lrdata->Image_X;
         BitRect.Top=lrdata->Image_Y;
	     BitRect.Right=lrdata->Image_X+lrdata->Width;
	     BitRect.Bottom=lrdata->Image_Y+lrdata->Height;
		 DrawBitmap(Images[lrdata->Image_Number], &BitRect, Menu[i].X+x, Menu[i].Y+y );
		 int tempstep=0; 
		 for(int j=lrdata->Start;j<lrdata->Max;j++)
		 {
			if(j<(lrdata->Start+lrdata->Max_Show))
			{
			  char *s;

			  if(j==lrdata->Current)
			  {
				BitRect.Left=lrdata->Rev_X;
				BitRect.Top=lrdata->Rev_Y;
				BitRect.Right=lrdata->Rev_X+lrdata->Rev_Width;
				BitRect.Bottom=lrdata->Rev_Y+lrdata->Rev_Height;
				DrawBitmap(Images[lrdata->Image_Number], &BitRect, Menu[i].X+x+lrdata->Corner_X, Menu[i].Y+y+lrdata->Corner_Y+tempstep );
			  }
              s=Kdata[j].text;
			  MFontRect(s, lrdata->Font, Menu[i].X+x+lrdata->Start_X, Menu[i].Y+y+lrdata->Start_Y+tempstep);
			} 
			tempstep+=lrdata->Step;
		 } 
		 if((temppos.x>=Menu[i].X+x+lrdata->Corner_X) && (temppos.x<=(Menu[i].X+x+lrdata->Corner_X+lrdata->Click_Width)) && (temppos.y>=Menu[i].Y+y+lrdata->Corner_Y) && (temppos.y<=(Menu[i].Y+y+lrdata->Corner_Y+(lrdata->Max_Show*lrdata->Step))) && slide_click==-1)
			 {
			   if((GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0 && focus == -1)
			   {
				 lsbox_click=i;
				 lsbox_line = (temppos.y-(Menu[i].Y+y+lrdata->Corner_Y))/lrdata->Step;
			   }
			 } 
	   }

	}

#ifdef BLIT
	geEngine_DrawBitmap(CCD->Engine()->Engine(), ScreenBmp, NULL, 0, 0);
#endif

	if(LoopOnce==0 && !Fading)
	{
		if(AnimCursor<0 || Animation[AnimCursor]==NULL)
		{
			geEngine_DrawBitmap(CCD->Engine()->Engine(), Cursor, NULL, temppos.x, temppos.y );
		}
		else
		{
			geBitmap *theBmp = Animation[AnimCursor]->NextFrame(true);
			DrawBitmap(theBmp, NULL, temppos.x, temppos.y );
		}
	}

	if(Fading)
	{
		DoFade();
	}

	geEngine_EndFrame(CCD->Engine()->Engine());

	if((GetAsyncKeyState(VK_ESCAPE) & 0x8000) != 0 && escapeon == 0)
		escapeon = 1;
	if((GetAsyncKeyState(VK_ESCAPE) & 0x8000) == 0 && escapeon == 1)
	{
		escapeon = 0;
		if(ingame == 1)
		{
			if(musictype!=-1)
			{
				if(musictype==1)
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
			GameLevel();
		}
	}

	if(!Fading)
	{
		//--------------------------------------
		// if clicked on Clickable or Exit Menu
		//--------------------------------------
		
		if(click!=-1 && (GetAsyncKeyState(VK_LBUTTON) & 0x8000) == 0)
		{
			if(LoopOnce==0)
				geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), mouseclick, 0.99f, 0.0f, 1.0f, false);
			data=(Clickable *)Menu[click].data;
			if(/*data->LoopOnce==1 && */ LoopOnce<2)
			{
				LoopOnce += 1;
			}
			else
			{
				LoopOnce = 0;
				if(Menu[click].Type==EXIT_MENU)
				{
					FadeSet(-1, 1.5f);
					if(remapf != -1)
						CCD->Input()->SaveKeymap("keyboard.ini");
					if(data->Action>=10)
						return data->Action-9;
					return 0;
				}
				// changed RF063
				if(Menu[click].Type==CANCEL_MENU)
				{
					if(remapf != -1)
						CCD->Input()->SaveKeymap("keyboard.ini");
					return 1;
				}
				// end change RF063
				if(Menu[click].Type==CLICKABLE)
				{
					if(data->Next!=NULL && (data->Action==0 || data->Action==2))
					{
						//FadeOut();
						FadeSet(-1, 1.5f);
						ProcessMenu(data->Next, data->background, data->title);
					}
					if(data->proc!=NULL && data->Action==1)
					{
						data->proc();
					}
					else if(data->proc!=NULL && data->Action==3)
					{
						data->proc();
						remapf = 0;
					}
					else if(data->proc!=NULL && data->Action==4)
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
				click=-1;
			}
		}
		
		//--------------------------
		// if clicked on slider
		//--------------------------
		
		if(slide_click!=-1 && (GetAsyncKeyState(VK_LBUTTON) & 0x8000) == 0)
		{
			temp=temppos.x-slide_x;
			sdata=(Slider *)Menu[slide_click].data;
			sdata->Current+=temp;
			if(sdata->Current>sdata->Max_X)
				sdata->Current=sdata->Max_X;
			if(sdata->Current<sdata->Min_X)
				sdata->Current=sdata->Min_X;
			slide_x=temppos.x;
			temp=(sdata->Current*100)/sdata->Max_X;
			if(sdata->proc!=NULL && sdata->On_the_Fly==0)
				sdata->proc(temp);
			slide_click=-1;
		}
		if(slide_click==-1)
			slide_x=-1;
		if(slide_click!=-1 && (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0)
		{
			if(slide_x!=temppos.x)
			{
				temp=temppos.x-slide_x;
				sdata=(Slider *)Menu[slide_click].data;
				sdata->Current+=temp;
				if(sdata->Current>sdata->Max_X)
					sdata->Current=sdata->Max_X;
				if(sdata->Current<sdata->Min_X)
					sdata->Current=sdata->Min_X;
				slide_x=temppos.x;
				temp=(sdata->Current*100)/sdata->Max_X;
				geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), slideclick, 0.75f, 0.0f, 1.0f, false);
				if(sdata->proc!=NULL && sdata->On_the_Fly==1)
					sdata->proc(temp);
			}
		} 
		
		//--------------------------
		// if clicked on box
		//--------------------------
		
		if(box_click!=-1 && (GetAsyncKeyState(VK_LBUTTON) & 0x8000) == 0)
		{
			geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), mouseclick, 0.99f, 0.0f, 1.0f, false);
			bdata=(Box *)Menu[box_click].data;
			bdata->Current=(bdata->Current+1) & 1;
			if(bdata->proc!=NULL)
				bdata->proc(bdata->Current);
			box_click=-1;
		}
		
		
		//--------------------------
		// if clicked on remap
		//--------------------------
		
		if(remap_click!=-1 && (GetAsyncKeyState(VK_LBUTTON) & 0x8000) == 0)
		{
			geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), mouseclick, 0.99f, 0.0f, 1.0f, false);
			rdata=(Remap *)Menu[remap_click].data;
			rdata->Current = remap_line + rdata->Start;
			remap_click=-1;
		}
		
		//--------------------------
		// if clicked on scrollbar
		//--------------------------
		
		if(scroll_click!=-1 && (GetAsyncKeyState(VK_LBUTTON) & 0x8000) == 0)
		{
			geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), mouseclick, 0.99f, 0.0f, 1.0f, false);
			scdata=(ScrollBar *)Menu[scroll_click].data;
			int max = *(scdata->Max);
			int current = *(scdata->Current);
			if(scroll_dir==0)
			{
				if(current>0)
					*(scdata->Current)-=1;
			}
			else
			{
				if(current<(max-scdata->Show))
					*(scdata->Current)+=1;
			}
			scroll_click=-1;
		}
		
		//--------------------------
		// if clicked on lsbox
		//--------------------------
		
		if(lsbox_click!=-1 && (GetAsyncKeyState(VK_LBUTTON) & 0x8000) == 0)
		{
			geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), mouseclick, 0.99f, 0.0f, 1.0f, false);
			lrdata=(LSBox *)Menu[lsbox_click].data;
			lrdata->Current = lsbox_line + lrdata->Start;
			lsbox_click=-1;
		}
	}

  }
  return 1;
}

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
		if(x==0 && y==0)
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
	geEngine_DrawBitmap(CCD->Engine()->Engine(), Bitmap, Source, x, y );
#endif
}


void CRFMenu::MFontRect(char *s, int FontNumber, int x, int y)
{
  int charoff;
  char chr;

  if(MenuFont[FontNumber].Bitmap == NULL)
  {
		char szBug[256];
		sprintf(szBug, "No defined Font # %d", FontNumber);
		CCD->ReportError(szBug, false);
		return;
  }
  if(s!=NULL)
  {
	  charoff = 0;
      while (*s!=0)
	  {
			chr = *s-32;
			fRect.Top=MenuFont[FontNumber].dat[chr].y;
			fRect.Bottom=MenuFont[FontNumber].dat[chr].y+MenuFont[FontNumber].font_height;
			fRect.Left=MenuFont[FontNumber].dat[chr].x;
			fRect.Right=MenuFont[FontNumber].dat[chr].x+MenuFont[FontNumber].dat[chr].width;
			DrawBitmap(MenuFont[FontNumber].Bitmap, &fRect, x+charoff, y);
			charoff+=MenuFont[FontNumber].dat[chr].width;
			s++;
	  }
  }
}

//-----------------------------
// setup bitmap rectangle for
// the letter in the given font
//-----------------------------

void CRFMenu::FontRect(char *s, int FontNumber, int x, int y)
{
  int charoff;
  char chr;

  if(MenuFont[FontNumber].Bitmap == NULL)
  {
		char szBug[256];
		sprintf(szBug, "No defined Font # %d", FontNumber);
		CCD->ReportError(szBug, false);
// changed RF064
		CCD->ShutdownLevel();
		delete CCD;
		MessageBox(NULL, szBug,"Fatal Error", MB_OK);
		exit(-336);
// end change RF064
  }
  if(s!=NULL)
  {
	  charoff = 0;
      while (*s!=0)
	  {
			chr = *s-32;
			fRect.Top=MenuFont[FontNumber].dat[chr].y;
			fRect.Bottom=MenuFont[FontNumber].dat[chr].y+MenuFont[FontNumber].font_height-1;
			fRect.Left=MenuFont[FontNumber].dat[chr].x;
			fRect.Right=MenuFont[FontNumber].dat[chr].x+MenuFont[FontNumber].dat[chr].width-1;
			geEngine_DrawBitmap(CCD->Engine()->Engine(), MenuFont[FontNumber].Bitmap, &fRect, x+charoff, y);
			charoff+=MenuFont[FontNumber].dat[chr].width;
			s++;
	  }
  }
}

int CRFMenu::FontHeight(int FontNumber)
{
	if(MenuFont[FontNumber].Bitmap == NULL)
	{
		char szBug[256];
		sprintf(szBug, "No defined Font # %d", FontNumber);
		CCD->ReportError(szBug, false);
		return 0;
	}
	return MenuFont[FontNumber].font_height;
}

int CRFMenu::FontWidth(int FontNumber, char *s)
{
  int charoff;
  char chr;

  if(MenuFont[FontNumber].Bitmap == NULL)
  {
		char szBug[256];
		sprintf(szBug, "No defined Font # %d", FontNumber);
		CCD->ReportError(szBug, false);
		return 0;
  }
  if(s!=NULL)
  {
	  charoff = 0;
      while (*s!=0)
	  {
			chr = *s-32;
			charoff+=MenuFont[FontNumber].dat[chr].width;
			s++;
	  }
	  return charoff;
  }
  else
	  return 0;
}

void CRFMenu::StopMenuMusic()
{
	if(musictype!=-1)
	{
		if(musictype==1)
			MIDIPlayer()->Stop();
		else
			m_Streams->Stop();
	 }
}

// changed RF064
void CRFMenu::SetMusicVol(float vol)
{
	CMixer mixer(CCD->Engine()->WindowHandle(), MIXERLINE_COMPONENTTYPE_DST_SPEAKERS,
                  NO_SOURCE, MIXERCONTROL_CONTROLTYPE_VOLUME);
	if (!mixer.IsOk())
		return;

	DWORD dw = (DWORD)(vol*65535.0f);
	mixer.SetControlValue(dw);
}
 
void CRFMenu::SetmMusicVol(float vol)
{
	mVolLevel = vol;
	LONG nVolume = (LONG)((vol*10000.0f)-10000);
	if(musictype!=-1)
	{
		if(musictype!=1)
			m_Streams->SetVolume(nVolume);
	}
	if(CCD->AudioStreams())
		CCD->AudioStreams()->SetVolume(nVolume);
	CMixer mixer(CCD->Engine()->WindowHandle(), MIXERLINE_COMPONENTTYPE_DST_SPEAKERS,
				MIXERLINE_COMPONENTTYPE_SRC_SYNTHESIZER, MIXERCONTROL_CONTROLTYPE_VOLUME);
	if (mixer.IsOk())
	{
		DWORD dw = (DWORD)(vol*65535.0f);
		mixer.SetControlValue(dw);
	}
	CMixer mixercd(CCD->Engine()->WindowHandle(), MIXERLINE_COMPONENTTYPE_DST_SPEAKERS,
				MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC, MIXERCONTROL_CONTROLTYPE_VOLUME);
	if (mixercd.IsOk())
	{
		DWORD dw = (DWORD)(vol*65535.0f);
		mixercd.SetControlValue(dw);
	}
}
// end change RF064

//------------------------------
// setup and run level
//------------------------------

void CRFMenu::GameLoop()
{
//	Now that we have the initial inventory and attributes for the
//	..first level, we're going to save them off to a temp. file so
//	..if the player gets killed, when we restart we can do a fast
//	..and easy reload of the "game startup" attributes and
//	..inventory.

/* changed RF064
	CCD->Player()->DisableFog();				// Turn off fogging for cut scene
	CCD->Player()->DisableClipPlane();	// Turn off the clipping plane as well

//	Play the opening cut scene, if one exists

	CCD->PlayOpeningCutScene();
end change RF064 */

	CCD->Player()->ShowFog();					// Show fog, if enabled
	CCD->Player()->ActivateClipPlane();	// Activate clipping plane, if enabled

	
  //	On the load of the first level, we want to set up the player
//	..attributes and HUD display format.

	CCD->HUD()->LoadConfiguration();
	CCD->Player()->SaveAttributes("pdoa.bin");
	CCD->SetLevelData();

	CCD->MenuManager()->SetInGame();
	GameLevel();
}

//---------------------------------
// Main game loop
//---------------------------------
// changed RF063
void CRFMenu::GameLevel()
{
	MSG msg;
	int FirstFont;
	geRect	 firstRect;

	CCD->Player()->ShowFog();

//	Main game loop.  Until it's time for us to quit, we'll check for
//	..motion, move the player, etc.

  if(box6.Current == BOX_ON)
	CCD->Engine()->ShowFrameRate(true);

  CCD->ResetClock();
  CCD->Engine()->BeginFrame();
  CCD->Engine()->EndFrame();
  CCD->ResetClock();

  for(FirstFont=0; FirstFont<NUM_FONTS; FirstFont++)
  {
		if(MenuFont[FirstFont].Bitmap != NULL)
		{
			
			firstRect.Top=MenuFont[FirstFont].dat[0].y;
			firstRect.Bottom=MenuFont[FirstFont].dat[0].y+MenuFont[0].font_height;
			firstRect.Left=MenuFont[FirstFont].dat[0].x;
			firstRect.Right=MenuFont[FirstFont].dat[0].x+MenuFont[FirstFont].dat[0].width;
			break;
		}
  }

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
			if(box1.Current==BOX_ON
				&& CCD->Player()->GetViewPoint()==FIRSTPERSON
				&& CCD->Weapons()->CrossHair())	// display crosshair
			{
				if(!CCD->Weapons()->CrossHairFixed() && CCD->Weapons()->GetCrossHair())
				{
					DisplayCrossHair();
					cflag = true;
				}
			}

			CCD->Pawns()->AnimateWeapon();
			CCD->Weapons()->Display();
			CCD->Weapons()->DoAttack();
			CCD->Explosions()->Tick(CCD->GetTicksGoneBy());
			CCD->EffectManager()->Tick(CCD->GetTicksGoneBy());	

			// Entities animated, render _EVERYTHING_
			CCD->Engine()->BeginFrame();				// Start up rendering for this frame

			geEngine_DrawBitmap(CCD->Engine()->Engine(), MenuFont[FirstFont].Bitmap, &firstRect, 0, 0);

			CCD->RenderComponents();						// Render the RGF components

			CCD->Engine()->RenderWorld();				// Render the world
			
			if(box1.Current==BOX_ON
				&& (CCD->Player()->GetViewPoint()==FIRSTPERSON || CCD->Player()->GetViewPoint()==THIRDPERSON))
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
							if(CCD->Player()->GetViewPoint()==THIRDPERSON && !(CCD->Weapons()->GetCurrent()==-1 || CCD->Weapons()->GetCurrent()==11))
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
							geEngine_DrawBitmap(CCD->Engine()->Engine(), theBmp, NULL, x, y );
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
			if(CCD->GetSaving())
			{
				savetime = SavingTime;
				CCD->SetSaving(false);
			}

			if(savetime>0.0f)
			{
				savetime -= (CCD->LastElapsedTime_F()*0.001f);
				int width = FontWidth(SaveFont, Savemsg);
				int height = FontHeight(SaveFont);
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
			}

			CCD->Inventory()->Display();
// end change RF064

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
					geFog_SetAttributes(theFog, &Pos, &cColor, 0.0f,
						1500.0f, 240.0f);
					for(nTemp = 1000; nTemp < 6000; nTemp += 200)
					{
						CCD->Engine()->BeginFrame();
						geFog_SetAttributes(theFog, &Pos, &cColor,
							0.0f, (geFloat)nTemp+500, 240.0f+(geFloat)(nTemp/400));
						CCD->Engine()->RenderWorld();
						CCD->Engine()->EndFrame();
					}
					for(nTemp = 6000; nTemp > 0; nTemp -= 200)
					{
						CCD->Engine()->BeginFrame();
						geFog_SetAttributes(theFog, &Pos, &cColor,
							0.0f, (geFloat)nTemp+500, 240.0f+(geFloat)(nTemp/400));
						CCD->Engine()->RenderWorld();
						CCD->Engine()->EndFrame();
					}
					geWorld_RemoveFog(CCD->World(), theFog);
					savetime = 0.0f;
					CCD->ResetClock();
					CCD->Engine()->BeginFrame();
					CCD->Engine()->EndFrame();
					CCD->ResetClock();
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
					geFog_SetAttributes(theFog, &Pos, &cColor, 0.0f,
						1500.0f, 50.0f);
					for(nTemp = 1000; nTemp < 6000; nTemp += 200)
					{
						CCD->Engine()->BeginFrame();
						geFog_SetAttributes(theFog, &Pos, &cColor,
							0.0f, (geFloat)nTemp+500, 50.0f+(geFloat)(nTemp/200));
						CCD->Engine()->RenderWorld();
						CCD->Engine()->EndFrame();
					}
					for(nTemp = 6000; nTemp >= 0; nTemp -= 200)
					{
						CCD->Engine()->BeginFrame();
						geFog_SetAttributes(theFog, &Pos, &cColor,
							0.0f, (geFloat)nTemp+500, 50.0f+(geFloat)(nTemp/200));
						CCD->Engine()->RenderWorld();
						CCD->Engine()->EndFrame();
					}
					geWorld_RemoveFog(CCD->World(), theFog);
				}
				
				CCD->Player()->DisableFog();			// Fogging OFF
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
				CCD->ResetClock();
				CCD->Engine()->BeginFrame();
				CCD->Engine()->EndFrame();
				CCD->ResetClock();
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
					CCD->Player()->DisableFog();			// Fogging OFF
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
						if(musictype!=-1)
						{
							if(musictype==1)
								MIDIPlayer()->Play(music,true);
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
					CCD->ResetClock();
					CCD->Engine()->BeginFrame();
					CCD->Engine()->EndFrame();
					CCD->ResetClock();
				}
			}
		}
		else
		{
			break;
		}

	}					// End main game loop

  	while((GetAsyncKeyState(VK_ESCAPE) & 0x8000) != 0)
	{
	}
	CCD->AudioStreams()->PauseAll(); // pause streaming audio
	if(CCD->CDPlayer())
		CCD->CDPlayer()->Stop(); // stop CD music
	if(CCD->MIDIPlayer())
	  CCD->MIDIPlayer()->Stop(); // stop midi music
	CCD->Engine()->ShowFrameRate(false);
	ClearVol();
	CCD->Player()->DisableFog(); 
	if(musictype!=-1)
	{
		if(musictype==1)
			MIDIPlayer()->Play(music,true);
		else
			m_Streams->Play(true);
	}
}
// end change RF063

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
	
	geXForm3d_SetIdentity(&Xf);
	geXForm3d_RotateZ(&Xf, theRotation.Z);
	geXForm3d_RotateX(&Xf, theRotation.X);
	geXForm3d_RotateY(&Xf, theRotation.Y);
	geXForm3d_Translate(&Xf, thePosition.X, thePosition.Y, thePosition.Z);
	
	geVec3d Pos = Xf.Translation;
	geXForm3d_GetIn(&Xf, &Direction);
	geVec3d_AddScaled (&Pos, &Direction, CurrentDistance, &Back);
	CCD->Collision()->IgnoreContents(false);
	CCD->Collision()->CheckLevel(RGF_COLLISIONLEVEL_1);
	char BoneHit[64];
	BoneHit[0] = '\0';
	if(CCD->Collision()->CheckForBoneCollision(&theBox.Min, &theBox.Max,
				Pos, Back, &Collision, theActor, BoneHit, true))
	//if(CCD->Collision()->CheckForWCollision(&theBox.Min, &theBox.Max,
				//Pos, Back, &Collision, theActor))
	{
		CurrentDistance = (geFloat)fabs(geVec3d_DistanceBetween(&Collision.Impact, &Pos));
		if(CurrentDistance < 0.0f)
			CurrentDistance = 0.0f;
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
				geVec3d_Normalize(&Fill);
				geXForm3d	Xf;
				geXForm3d	XfT;
				geVec3d NewFillNormal;
				geActor_GetBoneTransform(Collision.Actor, NULL, &Xf );
				geXForm3d_GetTranspose( &Xf, &XfT );
				geXForm3d_Rotate( &XfT, &Fill, &NewFillNormal );
				geActor_SetLightingOptions(Collision.Actor, GE_TRUE, &NewFillNormal, 255.0f, 255.0f, 255.0f,
					255.0f, 255.0f, 255.0f, GE_TRUE, 6, NULL, GE_FALSE);
				geVec3d LitColor = CCD->Weapons()->GetLitColor();
				Vert.r = LitColor.X;
				Vert.g = LitColor.Y;
				Vert.b = LitColor.Z;
				Vert.a = 255.0f;
			}
		}
		geVec3d_AddScaled (&Pos, &Direction, CurrentDistance, &Back);
	}
	Vert.u = Vert.v = 0.0f;
	Vert.X = Back.X;
	Vert.Y = Back.Y;
	Vert.Z = Back.Z;
	geWorld_AddPolyOnce(CCD->World(), &Vert, 1, CCD->Weapons()->GetCrossHair(), GE_TEXTURED_POINT, GE_RENDER_DO_NOT_OCCLUDE_SELF, 1.0f);
}


//-----------------------------------------
// display splash screen with color 255
// as transparent
//-----------------------------------------
// changed RF064
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
			x = (CCD->Engine()->Width() - BmpInfo.Width) / 2;
			y = (CCD->Engine()->Height() - BmpInfo.Height) / 2;
			if(geEngine_AddBitmap(CCD->Engine()->Engine(), theBmp) == GE_FALSE)
			{
				char szError[200];
				sprintf(szError,"DisplaySplash: addbitmap failed on '%s'\n", Loading);
				CCD->ReportError(szError, false);
				return;
			}
			geBitmap_SetColorKey(theBmp, GE_TRUE, 255, GE_FALSE);
			if(CCD->GetHasFocus())
			{
				geEngine_BeginFrame(CCD->Engine()->Engine(), M_Camera, GE_TRUE);
				if(geEngine_DrawBitmap(CCD->Engine()->Engine(), theBmp, NULL, x, y ) == GE_FALSE)
				{
					char szError[200];
					sprintf(szError,"DisplaySplash: drawbitmap failed on '%s'\n", Loading);
					CCD->ReportError(szError, false);
				}
				geEngine_EndFrame(CCD->Engine()->Engine());
			}
		 }
		 geBitmap_Destroy(&theBmp);
	}
	return;
}
// end change RF064
//----------------------------
// save a screen shot
//----------------------------

void CRFMenu::ScreenShot()
{
	char filename[256];

	sprintf(filename, "screen%.3d.bmp",Screen);
	if(geEngine_ScreenShot(CCD->Engine()->Engine(), filename) == false)
	{
		CCD->ReportError("Can't create screenshot file!", false);
		return;
	}
	Screen+=1;
}


//-------------------------------------------------------------
//
// functions called by menu items
//
//-------------------------------------------------------------

//----------------------------------------
// initalize all menu items
//----------------------------------------

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

// load saved game file if it exists
	FILE *fd = CCD->OpenRFFile(kInstallFile, "savedgames.rgf", "rb");
	if(fd)
	{
		fread(&SaveBox.Max, sizeof(int), 1, fd);
		for(int nTemp = 0; nTemp < SaveBox.Max; nTemp++)
		{
			fread(SaveBox.text[nTemp].text, 30, 1, fd);
			fread(&SaveBox.text[nTemp].empty, sizeof(int), 1, fd);
		}
		fclose(fd);
	}

	
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

	  fclose(fd);
	}	 
// changed RF064
	int index = 0;
	bool flg = false;
	CIniFile AttrFile("control.ini");
	if(AttrFile.ReadFile())
	{
		CString KeyName = AttrFile.FindFirstKey();
		CString Type;
		while(KeyName != "")
		{
			if(KeyName=="Controls")
			{
				Type = AttrFile.GetValue(KeyName, "moveforward");
				if(Type=="true")
				{
					Redef[index].text = "Move Forward";
					Redef[index].action = RGF_K_FORWARD;
					index += 1;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_FORWARD);

				Type = AttrFile.GetValue(KeyName, "movebackward");
				if(Type=="true")
				{
					Redef[index].text = "Move Backward";
					Redef[index].action = RGF_K_BACKWARD;
					index += 1;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_BACKWARD);

				Type = AttrFile.GetValue(KeyName, "strafeleft");
				if(Type=="true")
				{
					Redef[index].text = "Strafe Left";
					Redef[index].action = RGF_K_LEFT;
					index += 1;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_LEFT);

				Type = AttrFile.GetValue(KeyName, "straferight");
				if(Type=="true")
				{
					Redef[index].text = "Strafe Right";
					Redef[index].action = RGF_K_RIGHT;
					index += 1;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_RIGHT);

				Type = AttrFile.GetValue(KeyName, "jump");
				if(Type=="true")
				{
					Redef[index].text = "Jump";
					Redef[index].action = RGF_K_JUMP;
					index += 1;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_JUMP);

				Type = AttrFile.GetValue(KeyName, "crouch");
				if(Type=="true")
				{
					Redef[index].text = "Crouch";
					Redef[index].action = RGF_K_CROUCH;
					index += 1;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_CROUCH);

				Type = AttrFile.GetValue(KeyName, "run");
				if(Type=="true")
				{
					Redef[index].text = "Run";
					Redef[index].action = RGF_K_RUN;
					index += 1;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_RUN);

				Type = AttrFile.GetValue(KeyName, "lookup");
				if(Type=="true")
				{
					Redef[index].text = "Look Up";
					Redef[index].action = RGF_K_LOOKUP;
					index += 1;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_LOOKUP);

				Type = AttrFile.GetValue(KeyName, "lookdown");
				if(Type=="true")
				{
					Redef[index].text = "Look Down";
					Redef[index].action = RGF_K_LOOKDOWN;
					index += 1;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_LOOKDOWN);

				Type = AttrFile.GetValue(KeyName, "turnleft");
				if(Type=="true")
				{
					Redef[index].text = "Turn Left";
					Redef[index].action = RGF_K_TURN_LEFT;
					index += 1;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_TURN_LEFT);

				Type = AttrFile.GetValue(KeyName, "turnright");
				if(Type=="true")
				{
					Redef[index].text = "Turn Right";
					Redef[index].action = RGF_K_TURN_RIGHT;
					index += 1;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_TURN_RIGHT);

				Type = AttrFile.GetValue(KeyName, "lookstraight");
				if(Type=="true")
				{
					Redef[index].text = "Look Straight";
					Redef[index].action = RGF_K_LOOKSTRAIGHT;
					index += 1;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_LOOKSTRAIGHT);

				Type = AttrFile.GetValue(KeyName, "attack");
				if(Type=="true")
				{
					Redef[index].text = "Attack";
					Redef[index].action = RGF_K_FIRE;
					index += 1;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_FIRE);

				Type = AttrFile.GetValue(KeyName, "altattack");
				if(Type=="true")
				{
					Redef[index].text = "Alt Attack";
					Redef[index].action = RGF_K_ALTFIRE;
					index += 1;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_ALTFIRE);

				Type = AttrFile.GetValue(KeyName, "weapon0");
				if(Type=="true")
				{
					Redef[index].text = "Weapon 0";
					Redef[index].action = RGF_K_WEAPON_0;
					index += 1;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_WEAPON_0);

				Type = AttrFile.GetValue(KeyName, "weapon1");
				if(Type=="true")
				{
					Redef[index].text = "Weapon 1";
					Redef[index].action = RGF_K_WEAPON_1;
					index += 1;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_WEAPON_1);

				Type = AttrFile.GetValue(KeyName, "weapon2");
				if(Type=="true")
				{
					Redef[index].text = "Weapon 2";
					Redef[index].action = RGF_K_WEAPON_2;
					index += 1;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_WEAPON_2);

				Type = AttrFile.GetValue(KeyName, "weapon3");
				if(Type=="true")
				{
					Redef[index].text = "Weapon 3";
					Redef[index].action = RGF_K_WEAPON_3;
					index += 1;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_WEAPON_3);

				Type = AttrFile.GetValue(KeyName, "weapon4");
				if(Type=="true")
				{
					Redef[index].text = "Weapon 4";
					Redef[index].action = RGF_K_WEAPON_4;
					index += 1;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_WEAPON_4);

				Type = AttrFile.GetValue(KeyName, "weapon5");
				if(Type=="true")
				{
					Redef[index].text = "Weapon 5";
					Redef[index].action = RGF_K_WEAPON_5;
					index += 1;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_WEAPON_5);

				Type = AttrFile.GetValue(KeyName, "weapon6");
				if(Type=="true")
				{
					Redef[index].text = "Weapon 6";
					Redef[index].action = RGF_K_WEAPON_6;
					index += 1;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_WEAPON_6);

				Type = AttrFile.GetValue(KeyName, "weapon7");
				if(Type=="true")
				{
					Redef[index].text = "Weapon 7";
					Redef[index].action = RGF_K_WEAPON_7;
					index += 1;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_WEAPON_7);

				Type = AttrFile.GetValue(KeyName, "weapon8");
				if(Type=="true")
				{
					Redef[index].text = "Weapon 8";
					Redef[index].action = RGF_K_WEAPON_8;
					index += 1;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_WEAPON_8);

				Type = AttrFile.GetValue(KeyName, "weapon9");
				if(Type=="true")
				{
					Redef[index].text = "Weapon 9";
					Redef[index].action = RGF_K_WEAPON_9;
					index += 1;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_WEAPON_9);

				Type = AttrFile.GetValue(KeyName, "lookmode");
				if(Type=="true")
				{
					Redef[index].text = "Look Mode";
					Redef[index].action = RGF_K_LOOKMODE;
					index += 1;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_LOOKMODE);

				Type = AttrFile.GetValue(KeyName, "cameramode");
				if(Type=="true")
				{
					Redef[index].text = "Camera Mode";
					Redef[index].action = RGF_K_CAMERA;
					index += 1;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_CAMERA);

				Type = AttrFile.GetValue(KeyName, "zoomin");
				if(Type=="true")
				{
					Redef[index].text = "Zoom In";
					Redef[index].action = RGF_K_ZOOM_IN;
					index += 1;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_ZOOM_IN);

				Type = AttrFile.GetValue(KeyName, "zoomout");
				if(Type=="true")
				{
					Redef[index].text = "Zoom Out";
					Redef[index].action = RGF_K_ZOOM_OUT;
					index += 1;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_ZOOM_OUT);

				Type = AttrFile.GetValue(KeyName, "zoomweapon");
				if(Type=="true")
				{
					Redef[index].text = "Zoom Weapon";
					Redef[index].action = RGF_K_ZOOM_WEAPON;
					index += 1;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_ZOOM_WEAPON);

				Type = AttrFile.GetValue(KeyName, "holsterweapon");
				if(Type=="true")
				{
					Redef[index].text = "Holster Weapon";
					Redef[index].action = RGF_K_HOLSTER_WEAPON;
					index += 1;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_HOLSTER_WEAPON);

				Type = AttrFile.GetValue(KeyName, "camerareset");
				if(Type=="true")
				{
					Redef[index].text = "Camera Reset";
					Redef[index].action = RGF_K_CAMERA_RESET;
					index += 1;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_CAMERA_RESET);

				Type = AttrFile.GetValue(KeyName, "quicksave");
				if(Type=="true")
				{
					Redef[index].text = "Quick Save";
					Redef[index].action = RGF_K_QUICKSAVE;
					index += 1;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_QUICKSAVE);

				Type = AttrFile.GetValue(KeyName, "quickload");
				if(Type=="true")
				{
					Redef[index].text = "Quick Load";
					Redef[index].action = RGF_K_QUICKLOAD;
					index += 1;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_QUICKLOAD);

				Type = AttrFile.GetValue(KeyName, "1stperson");
				if(Type=="true")
				{
					Redef[index].text = "1st Person";
					Redef[index].action = RGF_K_FIRST_PERSON_VIEW;
					index += 1;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_FIRST_PERSON_VIEW);

				Type = AttrFile.GetValue(KeyName, "3rdperson");
				if(Type=="true")
				{
					Redef[index].text = "3rd Person";
					Redef[index].action = RGF_K_THIRD_PERSON_VIEW;
					index += 1;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_THIRD_PERSON_VIEW);

				Type = AttrFile.GetValue(KeyName, "isometric");
				if(Type=="true")
				{
					Redef[index].text = "Isometric";
					Redef[index].action = RGF_K_ISO_VIEW;
					index += 1;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_ISO_VIEW);

				Type = AttrFile.GetValue(KeyName, "screenshot");
				if(Type=="true")
				{
					Redef[index].text = "Screenshot";
					Redef[index].action = RGF_K_SCRNSHOT;
					index += 1;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_SCRNSHOT);

				Type = AttrFile.GetValue(KeyName, "help");
				if(Type=="true")
				{
					Redef[index].text = "Help";
					Redef[index].action = RGF_K_HELP;
					index += 1;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_HELP);

				Type = AttrFile.GetValue(KeyName, "hud");
				if(Type=="true")
				{
					Redef[index].text = "Hud";
					Redef[index].action = RGF_K_HUD;
					index += 1;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_HUD);

				Type = AttrFile.GetValue(KeyName, "lightonoff");
				if(Type=="true")
				{
					Redef[index].text = "Light On/Off";
					Redef[index].action = RGF_K_LIGHT;
					index += 1;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_LIGHT);

				Type = AttrFile.GetValue(KeyName, "useitem");
				if(Type=="true")
				{
					Redef[index].text = "Use Item";
					Redef[index].action = RGF_K_USE;
					index += 1;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_USE);

				Type = AttrFile.GetValue(KeyName, "inventory");
				if(Type=="true")
				{
					Redef[index].text = "Inventory";
					Redef[index].action = RGF_K_INVENTORY;
					index += 1;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_INVENTORY);

				Type = AttrFile.GetValue(KeyName, "console");
				if(Type=="true")
				{
					Redef[index].text = "Console";
					Redef[index].action = RGF_K_CONSOLE;
					index += 1;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_CONSOLE);

				Type = AttrFile.GetValue(KeyName, "dropweapon");
				if(Type=="true")
				{
					Redef[index].text = "Drop Weapon";
					Redef[index].action = RGF_K_DROP;
					index += 1;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_DROP);

				Type = AttrFile.GetValue(KeyName, "reload");
				if(Type=="true")
				{
					Redef[index].text = "Reload Weapon";
					Redef[index].action = RGF_K_RELOAD;
					index += 1;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_RELOAD);

				Type = AttrFile.GetValue(KeyName, "powerup");
				if(Type=="true")
				{
					Redef[index].text = "Increase Power";
					Redef[index].action = RGF_K_POWERUP;
					index += 1;
				}
				else
					CCD->Input()->ClearCodes(RGF_K_POWERUP);

				Type = AttrFile.GetValue(KeyName, "powerdown");
				if(Type=="true")
				{
					Redef[index].text = "Decrease Power";
					Redef[index].action = RGF_K_POWERDWN;
					index += 1;
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
			sprintf(szError,"Missing section in Control.ini");
			CCD->ReportError(szError, false);
			CCD->ShutdownLevel();
			delete CCD;
			MessageBox(NULL, szError,"Controls", MB_OK);
			exit(-333);
		}
	}
	else
	{
		Redef[index].text = "Move Forward";
		Redef[index].action = RGF_K_FORWARD;
		index += 1;
		Redef[index].text = "Move Backward";
		Redef[index].action = RGF_K_BACKWARD;
		index += 1;
		Redef[index].text = "Strafe Left";
		Redef[index].action = RGF_K_LEFT;
		index += 1;
		Redef[index].text = "Strafe Right";
		Redef[index].action = RGF_K_RIGHT;
		index += 1;
		Redef[index].text = "Jump";
		Redef[index].action = RGF_K_JUMP;
		index += 1;
		Redef[index].text = "Crouch";
		Redef[index].action = RGF_K_CROUCH;
		index += 1;
		Redef[index].text = "Run";
		Redef[index].action = RGF_K_RUN;
		index += 1;
		Redef[index].text = "Look Up";
		Redef[index].action = RGF_K_LOOKUP;
		index += 1;
		Redef[index].text = "Look Down";
		Redef[index].action = RGF_K_LOOKDOWN;
		index += 1;
		Redef[index].text = "Turn Left";
		Redef[index].action = RGF_K_TURN_LEFT;
		index += 1;
		Redef[index].text = "Turn Right";
		Redef[index].action = RGF_K_TURN_RIGHT;
		index += 1;
		Redef[index].text = "Look Straight";
		Redef[index].action = RGF_K_LOOKSTRAIGHT;
		index += 1;
		Redef[index].text = "Attack";
		Redef[index].action = RGF_K_FIRE;
		index += 1;
		Redef[index].text = "Alt Attack";
		Redef[index].action = RGF_K_ALTFIRE;
		index += 1;
		Redef[index].text = "Weapon 0";
		Redef[index].action = RGF_K_WEAPON_0;
		index += 1;
		Redef[index].text = "Weapon 1";
		Redef[index].action = RGF_K_WEAPON_1;
		index += 1;
		Redef[index].text = "Weapon 2";
		Redef[index].action = RGF_K_WEAPON_2;
		index += 1;
		Redef[index].text = "Weapon 3";
		Redef[index].action = RGF_K_WEAPON_3;
		index += 1;
		Redef[index].text = "Weapon 4";
		Redef[index].action = RGF_K_WEAPON_4;
		index += 1;
		Redef[index].text = "Weapon 5";
		Redef[index].action = RGF_K_WEAPON_5;
		index += 1;
		Redef[index].text = "Weapon 6";
		Redef[index].action = RGF_K_WEAPON_6;
		index += 1;
		Redef[index].text = "Weapon 7";
		Redef[index].action = RGF_K_WEAPON_7;
		index += 1;
		Redef[index].text = "Weapon 8";
		Redef[index].action = RGF_K_WEAPON_8;
		index += 1;
		Redef[index].text = "Weapon 9";
		Redef[index].action = RGF_K_WEAPON_9;
		index += 1;
		Redef[index].text = "Look Mode";
		Redef[index].action = RGF_K_LOOKMODE;
		index += 1;
		Redef[index].text = "Camera Mode";
		Redef[index].action = RGF_K_CAMERA;
		index += 1;
		Redef[index].text = "Zoom In";
		Redef[index].action = RGF_K_ZOOM_IN;
		index += 1;
		Redef[index].text = "Zoom Out";
		Redef[index].action = RGF_K_ZOOM_OUT;
		index += 1;
		Redef[index].text = "Zoom Weapon";
		Redef[index].action = RGF_K_ZOOM_WEAPON;
		index += 1;
		Redef[index].text = "Holster Weapon";
		Redef[index].action = RGF_K_HOLSTER_WEAPON;
		index += 1;
		Redef[index].text = "Camera Reset";
		Redef[index].action = RGF_K_CAMERA_RESET;
		index += 1;
		Redef[index].text = "Quick Save";
		Redef[index].action = RGF_K_QUICKSAVE;
		index += 1;
		Redef[index].text = "Quick Load";
		Redef[index].action = RGF_K_QUICKLOAD;
		index += 1;
		Redef[index].text = "1st Person";
		Redef[index].action = RGF_K_FIRST_PERSON_VIEW;
		index += 1;
		Redef[index].text = "3rd Person";
		Redef[index].action = RGF_K_THIRD_PERSON_VIEW;
		index += 1;
		Redef[index].text = "Isometric";
		Redef[index].action = RGF_K_ISO_VIEW;
		index += 1;
		Redef[index].text = "Screenshot";
		Redef[index].action = RGF_K_SCRNSHOT;
		index += 1;
		Redef[index].text = "Help";
		Redef[index].action = RGF_K_HELP;
		index += 1;
		Redef[index].text = "Hud";
		Redef[index].action = RGF_K_HUD;
		index += 1;
		Redef[index].text = "Light On/Off";
		Redef[index].action = RGF_K_LIGHT;
		index += 1;
		Redef[index].text = "Use Item";
		Redef[index].action = RGF_K_USE;
		index += 1;
		Redef[index].text = "Inventory";
		Redef[index].action = RGF_K_INVENTORY;
		index += 1;
		Redef[index].text = "Console";
		Redef[index].action = RGF_K_CONSOLE;
		index += 1;
		Redef[index].text = "Drop Weapon";
		Redef[index].action = RGF_K_DROP;
		index += 1;
		Redef[index].text = "Reload Weapon";
		Redef[index].action = RGF_K_RELOAD;
		index += 1;
		Redef[index].text = "Increase Power";
		Redef[index].action = RGF_K_POWERUP;
		index += 1;
		Redef[index].text = "Decrease Power";
		Redef[index].action = RGF_K_POWERDWN;
		index += 1;
		Redef[index].text = NULL;
	}
// end change RF064
}

//--------------------------------------------
// replacement for play sound to keep track
// of all looping sounds
//--------------------------------------------

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
	}
	return Sound;
}

//------------------------------------------
// rep[lacement for stop sound to
// remove all dead entries
//------------------------------------------

geBoolean CRFMenu::StopSound(geSound_System *SoundS, geSound *Sound)
{
  SoundList *pool, *temp;

  pool = Bottom;
  while	(pool!= NULL)
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

//-----------------------------------
// zero volume of all looping sounds
//-----------------------------------

void CRFMenu::ClearVol()
{
  SoundList *pool, *temp;

  pool = Bottom;
  while	(pool!= NULL)
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

//-------------------------------
// clear out sound list
//-------------------------------

void CRFMenu::DeleteSound()
{
  SoundList *pool, *temp;

  pool = Bottom;
  while	(pool!= NULL)
  {
	temp = pool->next;
    free(pool);
	pool = temp;
  }
  Bottom = (SoundList *)NULL;
}

//-----------------------------------
// get percentage from gamma setting
//-----------------------------------

static int GetGammaPercent()
{
  float gamma;

  geEngine_GetGamma(CCD->Engine()->Engine(), &gamma);
  return (int)(((gamma-GAMMAMIN)/(GAMMAMAX-GAMMAMIN))*100);

}

//-----------------------------------
// set gamma from slider percentage
//-----------------------------------

static void SetGamma(int percent)
{
  float gamma;

  gamma=(((float)percent/100.0f)*(GAMMAMAX-GAMMAMIN))+GAMMAMIN;
  geEngine_SetGamma(CCD->Engine()->Engine(), gamma);
}


//-----------------------------------
// set detail from slider percentage
//-----------------------------------

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
			CCD->Engine()->SetFogParameters(theState->DistanceFogColor,
				start, end);
		}
		if(theState->UseFarClipPlane == GE_TRUE)
		{
			float dist = theState->FarClipPlaneDistLow + ((theState->FarClipPlaneDistHigh - theState->FarClipPlaneDistLow)*(detailevel/100));
			CCD->CameraManager()->SetFarClipPlane(dist);
		}
	}
}

//----------------------------------------------
// set mouse sensitivity from slider percentage
//----------------------------------------------

static void SetSens(int percent)
{
  CCD->MenuManager()->SetMouseSen((((float)percent/100.0f)*(MOUSEMAX-MOUSEMIN))+MOUSEMIN);
}

//-------------------------------------
// set volume from slider percentage
//-------------------------------------

static void SetVol(int percent)
{
  float vol;
// changed RF064
  vol=((float)percent/100.0f);
  if(percent == 0)
	  vol = 0.0f;
  CCD->MenuManager()->SetMusicVol(vol);
// end change RF064
}

//-------------------------------------
// set volume from slider percentage
//-------------------------------------

static void SetmVol(int percent)
{
  float vol;
  vol=((float)percent/100.0f);
  if(percent == 0)
	  vol = 0.0f;
  CCD->MenuManager()->SetmMusicVol(vol);
}

// changed RF063

void CRFMenu::DoGame(bool editor)
{
	useselect = false;

	if(CCD->GetCSelect() || CCD->GetCDifficult())
	{
		if(editor)
		{
			M_CameraRect.Left = 0;
			M_CameraRect.Right = CCD->Engine()->Width() - 1;
			M_CameraRect.Top = 0;
			M_CameraRect.Bottom = CCD->Engine()->Height() - 1;
			M_Camera = geCamera_Create(2.0f, &M_CameraRect);
			LoopOnce = 0;
			if(musictype!=-1)
			{
				if(musictype==1)
					MIDIPlayer()->Play(music,true);
				else
					m_Streams->Play(true);
			}
		}
		if(CCD->GetCSelect())
		{
			if(CCD->MenuManager()->ProcessMenu(SelectMenu, SelectBack, SelectTitle)==1)
				return;
			useselect = true;
		}
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
		if(CCD->GetCSelect() || CCD->GetCDifficult())
			CCD->MenuManager()->DisplaySplash(); // changed RF064
	}
// start multiplayer
	if(CCD->GetMultiPlayer())
	{
		if(!CCD->NetPlayerManager()->Initialize(CCD->GetServer(), ServerIP))
		{
			CCD->ReportError("Cant Begin Multiplayer session", false);
			return;
		}
	}
// end multiplayer	
	if((CCD->InitializeLevel(CCD->MenuManager()->GetLevelName())) != 0)
	{
		CCD->ReportError("Couldn't initialize first level", false);
		CCD->ShutdownLevel();						// Clean up anything that got loaded
		delete CCD;						// Kill off the engine and such
		MessageBox(NULL, CCD->MenuManager()->GetLevelName(),"RGF: Can't load level", MB_OK);
		exit(-333);
	}

// changed RF064
	CCD->Player()->DisableFog();				// Turn off fogging for cut scene
	CCD->Player()->DisableClipPlane();	// Turn off the clipping plane as well

// end change RF064

	//	Ok, move the player avatar to the correct player start in the
	//	..game level.
// changed RF064
	CCD->TerrainMgr()->Init();
// end change RF064	
	if(CCD->Player()->MoveToStart() != RGF_SUCCESS)
	{
		CCD->ReportError("Can't move player to start", false);
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

void CRFMenu::ChangeCurrent(bool direction)
{
	if(direction)
	{
		CurrentSelect+=1;
		if(CurrentSelect>=MaxSelect)
			CurrentSelect = 0;
	}
	else
	{
		CurrentSelect-=1;
		if(CurrentSelect<0)
			CurrentSelect = MaxSelect-1;
	}
}

static void PrevChar()
{
	CCD->MenuManager()->ChangeCurrent(false);
}

static void NextChar()
{
	CCD->MenuManager()->ChangeCurrent(true);
}

//-------------------------------------
// Run the level
//-------------------------------------

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

// end chnge RF063

//------------------------
// turn CD music on/off
//------------------------

static void SetCDPlayer(int current)
{
	if(current ==BOX_ON)
	{
		CCD->CDPlayer()->SetCdOn(true);
	}
	else
	{
		CCD->CDPlayer()->SetCdOn(false);
		CCD->CDPlayer()->Stop();
	}
}

//--------------------------
// turn mouse reverse on/off
//--------------------------

static void SetReverse(int current)
{
	if(current ==BOX_ON)
		CCD->MenuManager()->SetMouseReverse(true);
	else
		CCD->MenuManager()->SetMouseReverse(false);
}

//-------------------------
// turn mouse filter on/off
//-------------------------

static void SetFiltering(int current)
{
	if(current ==BOX_ON)
		CCD->MenuManager()->SetMouseFilter(true);
	else
		CCD->MenuManager()->SetMouseFilter(false);
}

//--------------------------------
// turn player bounding box on/off
//--------------------------------

static void SetBBox(int current)
{
	if(current ==BOX_ON)
		CCD->MenuManager()->SetBoundBox(true);
	else
		CCD->MenuManager()->SetBoundBox(false);
}

//--------------------------------
// turn entity bounding box on/off
//--------------------------------

static void SetSEBBox(int current)
{
	if(current ==BOX_ON)
		CCD->MenuManager()->SetSEBoundBox(true);
	else
		CCD->MenuManager()->SetSEBoundBox(false);
}

//--------------------------------
// turn no clipping on/off
//--------------------------------

static void SetClipping(int current)
{
	if(current ==BOX_ON)
		CCD->MenuManager()->SetNoClip(true);
	else
		CCD->MenuManager()->SetNoClip(false);
}

//-------------------------------
// reset keys to default actions
//-------------------------------

static void ResetAction()
{
	CCD->Input()->Default();
}

//---------------------------
// save game to current slot
//---------------------------

static void SetSlot()
{
	if(SaveBox.Current != -1) // must have selected a slot
	{
		// slot must be visible in table
		if(SaveBox.Current>=SaveBox.Start && SaveBox.Current<(SaveBox.Max_Show+SaveBox.Start))
		{
			struct tm *newtime;
			time_t long_time;
			char *ttext;
			char filename[256];

			time( &long_time );
			newtime = localtime( &long_time ); 
			ttext = asctime( newtime );
			ttext+=4;
			strcpy(filename, CCD->Engine()->LevelName());
			*(strrchr(filename,'.')) = 0;
			// set date/time and level name

			sprintf(SaveBox.text[SaveBox.Current].text, "%.12s %.16s", ttext, filename);

			// mark slot as in use  
			SaveBox.text[SaveBox.Current].empty = 1;

			// save file of saved game info
			FILE *fd = CCD->OpenRFFile(kInstallFile, "savedgames.rgf", "wb");
			if(fd == NULL)
			{
				char szBug[200];
				sprintf(szBug,"Save Game: can't create savedgames.rgf\n");
				CCD->ReportError(szBug, false);
				return;										// Fatal error
			}
			fwrite(&SaveBox.Max, sizeof(int), 1, fd);
			for(int nTemp = 0; nTemp < SaveBox.Max; nTemp++)
			{
				fwrite(SaveBox.text[nTemp].text, 30, 1, fd);
				fwrite(&SaveBox.text[nTemp].empty, sizeof(int), 1, fd);
			}
			fclose(fd); 

			// save game information
			sprintf(filename, "savgame%d.sav",SaveBox.Current); 
			FILE *outFD = CCD->OpenRFFile(kSavegameFile, filename, "wb");
			if(outFD == NULL)
			{
				CCD->ReportError("Can't create savegame file!", false);
				return;
			}
			CCD->Engine()->SaveTo(outFD);
			CCD->MenuManager()->SaveTo(outFD, false);
			CCD->Player()->SaveTo(outFD);
			CCD->Doors()->SaveTo(outFD, false);
			CCD->Platforms()->SaveTo(outFD, false);
			CCD->Props()->SaveTo(outFD, false);
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
		}
	}
}

//----------------------------
// load game from current slot
// and then run it
//----------------------------

static void GetSlot()
{
// changed RF064
	if(LoadBox.Current != -1) // must have selected saved game
	{
		// must be visible in table
		if(LoadBox.Current>=LoadBox.Start && LoadBox.Current<(LoadBox.Max_Show+LoadBox.Start))
		{
			char filename[256];

			sprintf(filename, "savgame%d.sav",LoadBox.Current);
			FILE *inFD = CCD->OpenRFFile(kSavegameFile, filename, "rb");
			if(inFD == NULL)
			{
				CCD->ReportError("No savegame file to restore", false);
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
			CCD->Player()->RestoreFrom(inFD);
			CCD->Doors()->RestoreFrom(inFD, false);
			CCD->Platforms()->RestoreFrom(inFD, false);
			CCD->Props()->RestoreFrom(inFD, false);
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
}

void CRFMenu::UnLoadWBitmap()
{ 
	for(int i=0; i<NUM_FONTS; i++)
	{
		if(MenuFont[i].Bitmap != NULL)
		{
			for(int j=0;j<96;j++)
			{
				if(MenuFont[i].WBitmap[j])
				{
					geWorld_RemoveBitmap(CCD->World(), MenuFont[i].WBitmap[j]);
					geBitmap_Destroy(&MenuFont[i].WBitmap[j]);
				}
			}
		}
	} 
}

void CRFMenu::WorldFontRect(char *s, int FontNumber, int x, int y, float Alpha)
{
  int charoff;
  char chr;

  if(MenuFont[FontNumber].Bitmap == NULL)
  {
		char szBug[256];
		sprintf(szBug, "No defined Font # %d", FontNumber);
		CCD->ReportError(szBug, false);
		CCD->ShutdownLevel();
		delete CCD;
		MessageBox(NULL, szBug,"Fatal Error", MB_OK);
		exit(-336);
  }
  if(s!=NULL)
  {
	  charoff = 0;
      while (*s!=0)
	  {
			chr = *s-32;
			fRect.Top=0;
			fRect.Bottom=MenuFont[FontNumber].font_height-1;
			fRect.Left=0;
			fRect.Right=MenuFont[FontNumber].dat[chr].width-1;
			CCD->Engine()->DrawBitmap(MenuFont[FontNumber].WBitmap[chr], &fRect, x+charoff, y, Alpha, 1.0f);
			charoff+=MenuFont[FontNumber].dat[chr].width;
			s++;
	  }
  } 
}

void CRFMenu::FadeSet(int Dir, float Time)
{
	Fading = false;
	FadeDir = Dir;
	if(FadeDir==-1)
		FadeAlpha = 255.0f;
	else
		FadeAlpha = 0.0f;
	CurrentFadeTime = CCD->FreeRunningCounter_F();
	FadeTime = Time;
}

void CRFMenu::DoFade()
{
	float FadeDelta;
	float DeltaTime = (CCD->FreeRunningCounter_F() - CurrentFadeTime)/1000.0f;
	if(FadeDir==-1)
	{
		FadeDelta = 255.0f - (255.0f * (DeltaTime/FadeTime));
		if(FadeAlpha==0.0f)
		{
			Fading = false;
			FadeDir = 1;
		}
	}
	else
	{
		FadeDelta = 255.0f * (DeltaTime/FadeTime);
		if(FadeAlpha==255.0f)
		{
			Fading = false;
			FadeDir = -1;
		}
	}
	if(Fading)
	{

		FadeAlpha = FadeDelta;
		if(FadeAlpha > 255.0f) 
			FadeAlpha = 255.0f;
		if ( FadeAlpha < 0.0f ) 
			FadeAlpha = 0.0f;	// transparent
		if (FadeAlpha > 0.0f)
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
			geEngine_FillRect(CCD->Engine()->Engine(), &Rect, &Color );
		}
	}
}

void CRFMenu::FadeOut()
{
	FadeSet(1, 1.5f);
	for(int i=0;i<255;i++)
	{
		if(CCD->GetHasFocus())
		{
			geEngine_BeginFrame(CCD->Engine()->Engine(), M_Camera, GE_FALSE);
			DoFade();
			geEngine_EndFrame(CCD->Engine()->Engine());
		}
	}
}


//	SaveTo
//
//	Save the current state of the menu
//	..off to an open file.

int CRFMenu::SaveTo(FILE *SaveFD, bool type)
{

	WRITEDATA(&useselect, sizeof(geBoolean), 1, SaveFD);
	WRITEDATA(&CurrentSelect, sizeof(int), 1, SaveFD);

  return RGF_SUCCESS;
}

//	RestoreFrom
//
//	Restore the state of the menu from an
//	..open file.

int CRFMenu::RestoreFrom(FILE *RestoreFD, bool type)
{
	READDATA(&useselect, sizeof(geBoolean), 1, RestoreFD);
	READDATA(&CurrentSelect, sizeof(int), 1, RestoreFD);

  return RGF_SUCCESS;
}
