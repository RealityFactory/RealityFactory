


//	Include the One True Header

#include "RabidFramework.h"
#include "Ram.h"

static int GetGammaPercent();
static void SetGamma(int percent);
static void SetDetail(int percent);
static void SetSens(int percent);
static void SetVol(int percent);
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

#define GAMMAMIN   0.5f
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
  SLIDER,
  BOX,
  TEXT,
  REMAP,
  SCROLLBAR,
  LSBOX,
  EXIT_MENU
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
} MenuTitle;

//-------------------------------------
// Advanced Menu
//-------------------------------------

Clickable QuitAdvanced =  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0};
Box box1 =          {0,  0, 0, 0, 0, 0, 0, 0, 0, BOX_OFF, NULL};
Text xhair_text =      {0, "Crosshair", 0};
Box box2 =          {0,  0, 0, 0, 0, 0, 0, 0, 0, BOX_OFF, SetReverse};
Text rev_text =      {0, "Reverse mouse", 0};
Box box4 =          {0,  0, 0, 0, 0, 0, 0, 0, 0, BOX_OFF, SetFiltering};
Text filter_text =      {0, "Mouse filter", 0};
Image Sens_Img     = {0, 0, 0, 0, 0};
Text sens_text =      {0, "Mouse sensitivity", 0};
Slider Sens_Slide  = {0, 0, 0, 0, 0, 0, 0, 0, 0, SetSens};

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

// names of all the currently defined actions

Keydef Redef[] =
{
	{"Move Forward", RGF_K_FORWARD},
	{"Move Backward", RGF_K_BACKWARD},
	{"Strafe Left", RGF_K_LEFT},
	{"Strafe Right", RGF_K_RIGHT},
	{"Jump", RGF_K_JUMP},
	{"Crouch", RGF_K_CROUCH},
	{"Run", RGF_K_RUN},
	{"Look Up", RGF_K_LOOKUP},
	{"Look Down", RGF_K_LOOKDOWN},
	{"Turn Left", RGF_K_TURN_LEFT},
	{"Turn Right", RGF_K_TURN_RIGHT},
	{"Look Straight", RGF_K_LOOKSTRAIGHT},
	{"Attack", RGF_K_FIRE},
	{"Alt Attack", RGF_K_ALTFIRE},
	{"Weapon 0", RGF_K_WEAPON_0},
	{"Weapon 1", RGF_K_WEAPON_1},
	{"Weapon 2", RGF_K_WEAPON_2},
	{"Weapon 3", RGF_K_WEAPON_3},
	{"Weapon 4", RGF_K_WEAPON_4},
	{"Weapon 5", RGF_K_WEAPON_5},
	{"Weapon 6", RGF_K_WEAPON_6},
	{"Weapon 7", RGF_K_WEAPON_7},
	{"Weapon 8", RGF_K_WEAPON_8},
	{"Weapon 9", RGF_K_WEAPON_9},
	{"Look Mode", RGF_K_LOOKMODE},
	{"Camera Mode", RGF_K_CAMERA},
	{"Zoom In", RGF_K_ZOOM_IN},
	{"Zoom Out", RGF_K_ZOOM_OUT},
	{"Camera Reset", RGF_K_CAMERA_RESET},
	{"Quick Save", RGF_K_QUICKSAVE},
	{"Quick Load", RGF_K_QUICKLOAD},
	{"1st Person", RGF_K_FIRST_PERSON_VIEW},
	{"3rd Person", RGF_K_THIRD_PERSON_VIEW},
	{"Isometric", RGF_K_ISO_VIEW},
	{"Screenshot", RGF_K_SCRNSHOT},
	{"Help", RGF_K_HELP},
	{"Hud", RGF_K_HUD},
	{NULL}
};

Clickable QuitControl =  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0};
Clickable AdvancedItem = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, AdvancedMenu, NULL, 0};
Remap KeyMap = {0, 0, 0, 0, 0, 0, 0, 0, 0, Redef, Rename, 0, 0, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
ScrollBar KeyMapBar = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
						0, 0, 0, 0, &(KeyMap.Max), &(KeyMap.Start), 0};
Clickable ResetKey = {0, 0, 0, 0, 0, 0, 0, 3, 0, 0, NULL, ResetAction, 0};

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

Clickable QuitVideo = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0};
Text gam_text =     {0, "Gamma", 0};
Image Gamma_Img     = {0, 0, 0, 0, 0};
Slider Gamma_Slide  = {0, 0, 0, 0, 0, 0, 0, 0, 0, SetGamma};
Text det_text =     {0, "Detail Level", 0};
Image Detail_Img     = {0, 0, 0, 0, 0};
Slider Detail_Slide  = {0, 0, 0, 0, 0, 0, 0, 0, 0, SetDetail};

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

Clickable QuitAudio =   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0};
Text vol_text =     {0, "Sound Volume", 0};
Image Vol_Img     = {0, 0, 0, 0,  0};
Slider Vol_Slide  = {0, 0, 0, 0, 0, 0, 0, 0, 0, SetVol};
Box box0 =          {0,  0, 0, 0, 0, 0, 0, 0, 0, BOX_OFF, SetCDPlayer};
Text cd_text =      {0, "Play CD Music", 0};

MenuItem AudioMenu[] = 
  {
     {EXIT_MENU, 0, 0, (void *)&QuitAudio},
	 {TEXT,      0, 0, (void *)&vol_text},
	 {IMAGE,     0, 0, (void *)&Vol_Img},
	 {SLIDER,    0, 0, (void *)&Vol_Slide},
	 {BOX,       0, 0, (void *)&box0},
     {TEXT,      0, 0, (void *)&cd_text},
     {END_LIST, 0, 0, NULL}
  };

//-------------------------------------
// Debug Menu
//-------------------------------------

Clickable QuitDebug =   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0};
Box box5 =          {0,  0, 0, 0, 0, 0, 0, 0, 0, BOX_OFF, NULL};
Text debug_text =      {0, "Debug Info", 0};
Box box6 =          {0,  0, 0, 0, 0, 0, 0, 0, 0, BOX_OFF, NULL};
Text fps_text =      {0, "Frame Rate", 0};
Box box7 =          {0,  0, 0, 0, 0, 0, 0, 0, 0, BOX_OFF, SetClipping};
Text clip_text =      {0, "No Clipping", 0};
Box box8 =          {0,  0, 0, 0, 0, 0, 0, 0, 0, BOX_OFF, SetBBox};
Text bb_text =      {0, "Player Bounding Box", 0};
Box box9 =          {0,  0, 0, 0, 0, 0, 0, 0, 0, BOX_OFF, SetSEBBox};
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

Clickable QuitOption =  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0};
Clickable AudioItem =   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, AudioMenu, NULL, 0};
Clickable VideoItem =   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, VideoMenu, NULL, 0};
Clickable ControlItem = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ControlMenu, NULL, 0};
Clickable DebugItem =   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, DebugMenu, NULL, 0};

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

Savedef SavedGame[] =
{
	{"<Slot 1>                     ", 0},
	{"<Slot 2>                     ", 0},
	{"<Slot 3>                     ", 0},
	{"<Slot 4>                     ", 0},
	{"<Slot 5>                     ", 0},
	{"<Slot 6>                     ", 0},
	{"<Slot 7>                     ", 0},
	{"<Slot 8>                     ", 0},
	{"<Slot 9>                     ", 0},
	{"<Slot 10>                    ", 0},
	{"<Slot 11>                    ", 0},
	{"<Slot 12>                    ", 0},
	{"<Slot 13>                    ", 0},
	{"<Slot 14>                    ", 0},
	{"<Slot 15>                    ", 0},
	{NULL}
};

//-------------------------------------
// Save Game Menu
//-------------------------------------

Clickable QuitSave =   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0};
Clickable SaveSlot =   {0, 0, 0, 0, 0, 30, 0, 1, 0, 0, NULL, SetSlot, 0};
LSBox SaveBox = {0, 0, 0, 0, 0, 0, 0, 0, 0, SavedGame, 0, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0};
ScrollBar SaveBar = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
						0, 0, 0, 0, &(SaveBox.Max), &(SaveBox.Start), 0};

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

Clickable QuitLoad =   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0};
Clickable LoadSlot =   {0, 0, 0, 0, 0, 0, 0, 4, 0, 0, NULL, GetSlot, 1};
LSBox LoadBox = {0, 0, 0, 0, 0, 0, 0, 0, 0, SavedGame, 0, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0};
ScrollBar LoadBar = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
						0, 0, 0, 0, &(LoadBox.Max), &(LoadBox.Start), 0};

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

Clickable QuitCredit =   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0};
Image Credit_Img     = {0, 0, 0, 0, 0};

MenuItem CreditMenu[] = 
  {
     {EXIT_MENU, 0, 0, (void *)&QuitCredit},
	 {IMAGE,	 0, 0, (void *)&Credit_Img},
     {END_LIST, 0, 0, NULL}
  };

//-------------------------------------
// Multiplayer Game Menu
//-------------------------------------

Clickable QuitMulti =   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0};

MenuItem MultiMenu[] = 
  {
     {EXIT_MENU, 0, 0, (void *)&QuitMulti},
     {END_LIST, 0, 0, NULL}
  };

//-------------------------------------
// Main Menu
//-------------------------------------

Clickable NewGame =  {0, 0, 0, 0,   0, 0,  0, 1, 0, 0, NULL, RunGame, 1};
Clickable MultiPlayer = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, MultiMenu, NULL, 0};
Clickable LoadGame = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, LoadGMenu, NULL, 0};
Clickable SaveGame = {0, 0, 0, 0, 0, 0, 0, 2, 0, 0, SaveMenu, NULL, 0};
Clickable Options =  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, OptionMenu, NULL, 0};
Clickable Credits =  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CreditMenu, NULL, 0};
Clickable QuitGame = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 1};
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

MenuTitle MTitles[11];

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
		CCD->ReportError("Missing menu token", false);
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
  FILE *fd;
  int i;

  Bottom = (SoundList *)NULL;
  Screen = 0;
  ScrnWait = false;
  musictype = -1;
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

    if((fd=CCD->OpenRFFile(kRawFile, CCD->MenuIni(), "rt")) != NULL)
	  {
		// File there, parse it!
		char szInputLine[132];
		char *szAtom;
		char menuline[132], menuline2[132];
		while(fgets(szInputLine, 132, fd) != NULL)
		  {
			if(szInputLine[0] == ';') 
			  continue;				// Comment line
			if(strlen(szInputLine) <= 5)
			  continue;				// Skip blank lines
			// All config commands are "thing=value"
			szAtom = FirstToken(szInputLine," =");
			if(!stricmp(szAtom,"background"))
			{
				int index = NextValue();
				strcpy(menuline,"menu\\");
				strcat(menuline,NextToken());
				Backgrounds[index] = CreateFromFileName(menuline);
				if(Backgrounds[index] == (geBitmap *)NULL)
				{
					char szBug[256];
					sprintf(szBug, "Bad file name %s", szAtom);
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
					sprintf(szBug, "Bad file name %s", szAtom);
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
					sprintf(szBug, "Bad file name %s", szAtom);
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
					sprintf(szBug, "Bad file name %s", szAtom);
					CCD->ReportError(szBug, false);
					exit(-100);
				}
				geEngine_AddBitmap(CCD->Engine()->Engine(), MenuFont[index].Bitmap);

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
			}
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
					sprintf(szBug, "Bad file name %s", szAtom);
					CCD->ReportError(szBug, false);
					exit(-100);
				}
				geEngine_AddBitmap(CCD->Engine()->Engine(), Cursor);
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
					sprintf(szBug, "Bad file name %s", szAtom);
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
			}
			else if(!stricmp(szAtom,"returntext"))
			{
				MainMenu[7].X = NextValue();
				MainMenu[7].Y = NextValue();
				return_text.Font = NextFont();
			}
			else if(!stricmp(szAtom,"quitmulti"))
			{
				MultiMenu[0].X = NextValue();
				MultiMenu[0].Y = NextValue();
				QuitMulti.Image_Number = NextValue();
				QuitMulti.Width = NextValue();
				QuitMulti.Height = NextValue();
				QuitMulti.Image_X = NextValue();
				QuitMulti.Image_Y = NextValue();
				QuitMulti.Mover_X = NextValue();
				QuitMulti.Mover_Y = NextValue();
			}
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
			}
			else if(!stricmp(szAtom,"cdbox"))
			{
				AudioMenu[4].X = NextValue();
				AudioMenu[4].Y = NextValue();
				box0.Image_Number = NextValue();
				box0.Width = NextValue();
				box0.Height = NextValue();
				box0.Image_X = NextValue();
				box0.Image_Y = NextValue();
				box0.Mover_X = NextValue();
				box0.Mover_Y = NextValue();
				box0.Set_X = NextValue();
				box0.Set_Y = NextValue();
			}
			else if(!stricmp(szAtom,"cdtext"))
			{
				AudioMenu[5].X = NextValue();
				AudioMenu[5].Y = NextValue();
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
				strcpy(music, CCD->GetDirectory(kAudioStreamFile));
				strcat(music, "\\");
				strcat(music, NextToken());
				m_dsPtr = (LPDIRECTSOUND)geSound_GetDSound();
				m_Streams = new StreamingAudio(m_dsPtr);
				musictype = -1;
				if(m_Streams)
				{
					if(m_Streams->Create(music))
					{
						musictype = 0;
					}
				}
			}
			else
			{
				char szBug[256];
				sprintf(szBug, "Unknown Menu command %s", szInputLine);
				CCD->ReportError(szBug, false);
			}
		}
		fclose(fd);
	}
	else
	{
		CCD->ReportError("Missing menu INI file", false);
		CCD->ShutdownLevel();
		delete CCD;
		MessageBox(NULL, "Missing menu INI file","Fatal Error", MB_OK);
		exit(-336);
	}

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
			geBitmap_Destroy(&MenuFont[i].Bitmap);
  }
;
  geBitmap_Destroy(&Cursor);
  geBitmap_Destroy(&Crosshair);
  geBitmap_Destroy(&FCrosshair);
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
	m_Streams->Play(true);
  }
  int ret = ProcessMenu(MainMenu, MainBack, MainTitle);

  if(musictype!=-1)
  {
	m_Streams->Delete();
	delete m_Streams;
  }

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

  geBitmap_GetInfo(Backgrounds[Background_Number], &BmpInfo, NULL);
  x = (CCD->Engine()->Width() - BmpInfo.Width) / 2;
  y = (CCD->Engine()->Height() - BmpInfo.Height) / 2;
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

  while(1)
  {
	MSG msg;

	// If Winblows has something to say, take it in and pass it on in the
	// ..off-chance someone cares.
	if(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
	{
		GetMessage(&msg, NULL, 0, 0);
		DispatchMessage(&msg);
	}

    geEngine_BeginFrame(CCD->Engine()->Engine(), M_Camera, GE_TRUE);

    geEngine_DrawBitmap(CCD->Engine()->Engine(), Backgrounds[Background_Number], NULL, x, y );
	BitRect.Left=MTitles[Title_Number].Image_X;
    BitRect.Top=MTitles[Title_Number].Image_Y;
	BitRect.Right=MTitles[Title_Number].Image_X+MTitles[Title_Number].Width;
	BitRect.Bottom=MTitles[Title_Number].Image_Y+MTitles[Title_Number].Height;
    geEngine_DrawBitmap(CCD->Engine()->Engine(), Titles[MTitles[Title_Number].Image_Number], &BitRect, MTitles[Title_Number].X+x, MTitles[Title_Number].Y+y );

	GetCursorPos(&temppos);
    for(i=0;i<max;i++)
	{
	   if(Menu[i].Type==CLICKABLE || Menu[i].Type==EXIT_MENU)
	   {
		   data=(Clickable *)Menu[i].data;
		   if((temppos.x>=Menu[i].X+x) && (temppos.x<=(Menu[i].X+x+data->Width)) && (temppos.y>=Menu[i].Y+y) && (temppos.y<=(Menu[i].Y+y+data->Height)) && slide_click==-1)
		   {
			 if(data->Action==2 && ingame==0)
			 {
				BitRect.Left=data->Image_X;
				BitRect.Top=data->Image_Y;
				BitRect.Right=data->Image_X+data->Width;
				BitRect.Bottom=data->Image_Y+data->Height;
			 }
			 else
			 {
				BitRect.Left=data->Mover_X;
				BitRect.Top=data->Mover_Y;
				BitRect.Right=data->Mover_X+data->Width;
				BitRect.Bottom=data->Mover_Y+data->Height;
				if((GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0 && focus == -1)
					click=i;
			 }
		   }
		   else
		   {
             BitRect.Left=data->Image_X;
             BitRect.Top=data->Image_Y;
	         BitRect.Right=data->Image_X+data->Width;
	         BitRect.Bottom=data->Image_Y+data->Height;
		   }
           geEngine_DrawBitmap(CCD->Engine()->Engine(), Images[data->Image_Number], &BitRect, Menu[i].X+x, Menu[i].Y+y );
	   }
	   if(Menu[i].Type==IMAGE)
	   {
		   idata=(Image *)Menu[i].data;
           BitRect.Left=idata->Image_X;
           BitRect.Top=idata->Image_Y;
	       BitRect.Right=idata->Image_X+idata->Width;
	       BitRect.Bottom=idata->Image_Y+idata->Height;
           geEngine_DrawBitmap(CCD->Engine()->Engine(), Images[idata->Image_Number], &BitRect, Menu[i].X+x, Menu[i].Y+y );
	   }
	   if(Menu[i].Type==SLIDER)
	   {
		   sdata=(Slider *)Menu[i].data;
           BitRect.Left=sdata->Image_X;
           BitRect.Top=sdata->Image_Y;
	       BitRect.Right=sdata->Image_X+sdata->Width;
	       BitRect.Bottom=sdata->Image_Y+sdata->Height;
		   temp=sdata->Current;
           geEngine_DrawBitmap(CCD->Engine()->Engine(), Images[sdata->Image_Number], &BitRect, Menu[i].X+x+temp, Menu[i].Y+y );
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
             BitRect.Left=bdata->Mover_X;
             BitRect.Top=bdata->Mover_Y;
	         BitRect.Right=bdata->Mover_X+bdata->Width;
	         BitRect.Bottom=bdata->Mover_Y+bdata->Height;
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
               BitRect.Left=bdata->Image_X;
               BitRect.Top=bdata->Image_Y;
	           BitRect.Right=bdata->Image_X+bdata->Width;
	           BitRect.Bottom=bdata->Image_Y+bdata->Height;
			 }
			 else
			 {
               BitRect.Left=bdata->Set_X;
               BitRect.Top=bdata->Set_Y;
	           BitRect.Right=bdata->Set_X+bdata->Width;
	           BitRect.Bottom=bdata->Set_Y+bdata->Height;
			 }
		   }
           geEngine_DrawBitmap(CCD->Engine()->Engine(), Images[bdata->Image_Number], &BitRect, Menu[i].X+x, Menu[i].Y+y );
	   }
       if(Menu[i].Type==TEXT)
	   {
         char *s;

         tdata=(Text *)Menu[i].data;
		 s=tdata->text;
		 if(tdata->ingame==0 || ingame==1)
			FontRect(s, tdata->Font, Menu[i].X+x, Menu[i].Y+y);
       }
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
		 geEngine_DrawBitmap(CCD->Engine()->Engine(), Images[rdata->Image_Number], &BitRect, Menu[i].X+x, Menu[i].Y+y );
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
				 geEngine_DrawBitmap(CCD->Engine()->Engine(), Images[rdata->Image_Number], &BitRect, Menu[i].X+x+rdata->Corner_X, Menu[i].Y+y+rdata->Corner_Y+tempstep );
			  }
              s=Kdata[j].text;
			  FontRect(s, rdata->Font, Menu[i].X+x+rdata->Start_X, Menu[i].Y+y+rdata->Start_Y+tempstep);
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
			  FontRect(s, rdata->Font, Menu[i].X+x+rdata->Start_X+rdata->Key_X, Menu[i].Y+y+rdata->Start_Y+tempstep);
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
			scdata=(ScrollBar *)Menu[i].data;
			BitRect.Left=scdata->Up_Nor_X;
			BitRect.Top=scdata->Up_Nor_Y;
			BitRect.Right=scdata->Up_Nor_X+scdata->Up_Width;
			BitRect.Bottom=scdata->Up_Nor_Y+scdata->Up_Height;
			geEngine_DrawBitmap(CCD->Engine()->Engine(), Images[scdata->Image_Number], &BitRect, Menu[i].X+x+scdata->Up_X, Menu[i].Y+y+scdata->Up_Y );
			BitRect.Left=scdata->Dwn_Nor_X;
			BitRect.Top=scdata->Dwn_Nor_Y;
			BitRect.Right=scdata->Dwn_Nor_X+scdata->Dwn_Width;
			BitRect.Bottom=scdata->Dwn_Nor_Y+scdata->Dwn_Height;
			geEngine_DrawBitmap(CCD->Engine()->Engine(), Images[scdata->Image_Number], &BitRect, Menu[i].X+x+scdata->Dwn_X, Menu[i].Y+y+scdata->Dwn_Y );
			
			if((temppos.x>=Menu[i].X+x+scdata->Up_X) && (temppos.x<=(Menu[i].X+x+scdata->Up_X+scdata->Up_Width)) && (temppos.y>=Menu[i].Y+y+scdata->Up_Y) && (temppos.y<=(Menu[i].Y+y+scdata->Up_Y+scdata->Up_Height)) && slide_click==-1)
			{
			   if((GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0 && focus == -1)
			   {
		         BitRect.Left=scdata->Up_Push_X;
                 BitRect.Top=scdata->Up_Push_Y;
	             BitRect.Right=scdata->Up_Push_X+scdata->Up_Width;
	             BitRect.Bottom=scdata->Up_Push_Y+scdata->Up_Height;
				 geEngine_DrawBitmap(CCD->Engine()->Engine(), Images[scdata->Image_Number], &BitRect, Menu[i].X+x+scdata->Up_X, Menu[i].Y+y+scdata->Up_Y );
				 scroll_click=i;
				 scroll_dir = 0;
			   }
			   else
			   {
		         BitRect.Left=scdata->Up_Lit_X;
                 BitRect.Top=scdata->Up_Lit_Y;
	             BitRect.Right=scdata->Up_Lit_X+scdata->Up_Width;
	             BitRect.Bottom=scdata->Up_Lit_Y+scdata->Up_Height;
				 geEngine_DrawBitmap(CCD->Engine()->Engine(), Images[scdata->Image_Number], &BitRect, Menu[i].X+x+scdata->Up_X, Menu[i].Y+y+scdata->Up_Y );
			   }
			}
			if((temppos.x>=Menu[i].X+x+scdata->Dwn_X) && (temppos.x<=(Menu[i].X+x+scdata->Dwn_X+scdata->Dwn_Width)) && (temppos.y>=Menu[i].Y+y+scdata->Dwn_Y) && (temppos.y<=(Menu[i].Y+y+scdata->Dwn_Y+scdata->Dwn_Height)) && slide_click==-1)
			 {
			   if((GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0 && focus == -1)
			   {
		         BitRect.Left=scdata->Dwn_Push_X;
                 BitRect.Top=scdata->Dwn_Push_Y;
	             BitRect.Right=scdata->Dwn_Push_X+scdata->Dwn_Width;
	             BitRect.Bottom=scdata->Dwn_Push_Y+scdata->Dwn_Height;
				 geEngine_DrawBitmap(CCD->Engine()->Engine(), Images[scdata->Image_Number], &BitRect, Menu[i].X+x+scdata->Dwn_X, Menu[i].Y+y+scdata->Dwn_Y );
				 scroll_click=i;
				 scroll_dir = 1;
			   }
			   else
			   {
		         BitRect.Left=scdata->Dwn_Lit_X;
                 BitRect.Top=scdata->Dwn_Lit_Y;
	             BitRect.Right=scdata->Dwn_Lit_X+scdata->Dwn_Width;
	             BitRect.Bottom=scdata->Dwn_Lit_Y+scdata->Dwn_Height;
				 geEngine_DrawBitmap(CCD->Engine()->Engine(), Images[scdata->Image_Number], &BitRect, Menu[i].X+x+scdata->Dwn_X, Menu[i].Y+y+scdata->Dwn_Y );
			   }
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
		 geEngine_DrawBitmap(CCD->Engine()->Engine(), Images[lrdata->Image_Number], &BitRect, Menu[i].X+x, Menu[i].Y+y );
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
				geEngine_DrawBitmap(CCD->Engine()->Engine(), Images[lrdata->Image_Number], &BitRect, Menu[i].X+x+lrdata->Corner_X, Menu[i].Y+y+lrdata->Corner_Y+tempstep );
			  }
              s=Kdata[j].text;
// BUG FIX
			  FontRect(s, lrdata->Font, Menu[i].X+x+lrdata->Start_X, Menu[i].Y+y+lrdata->Start_Y+tempstep);
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

	if(LoopOnce==0)
		geEngine_DrawBitmap(CCD->Engine()->Engine(), Cursor, NULL, temppos.x, temppos.y );

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
				m_Streams->Stop();
			}
			CCD->AudioStreams()->PauseAll(); // restart streaming audio
			if(CCD->CDPlayer())
			{
				if(CCD->CDPlayer()->GetCdOn())
					CCD->CDPlayer()->Restore(); // restart CD music if CD is on
				if(CCD->MIDIPlayer())
					CCD->MIDIPlayer()->Restore(); // restart midi if it was playing
			}
			GameLevel();
		}
	}

//--------------------------------------
// if clicked on Clickable or Exit Menu
//--------------------------------------

	if(click!=-1 && (GetAsyncKeyState(VK_LBUTTON) & 0x8000) == 0)
	{
	  if(LoopOnce==0)
		geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), mouseclick, 0.99f, 0.0f, 1.0f, false);
	  data=(Clickable *)Menu[click].data;
	  if(data->LoopOnce==1 && LoopOnce<2)
	  {
		  LoopOnce += 1;
	  }
	  else
	  {
		  LoopOnce = 0;
		  if(Menu[click].Type==EXIT_MENU)
		  {
			  if(remapf != -1)
				CCD->Input()->SaveKeymap("keyboard.ini");
			  return 0;
		  }
		  if(Menu[click].Type==CLICKABLE)
		  {
			if(data->Next!=NULL && (data->Action==0 || data->Action==2))
				ProcessMenu(data->Next, data->background, data->title);
			if(data->proc!=NULL && data->Action==1)
				data->proc();
			if(data->proc!=NULL && data->Action==3)
			{
				data->proc();
				remapf = 0;
			}
			if(data->proc!=NULL && data->Action==4)
			{
				if(SaveBox.Current != -1)
				{
					if(SaveBox.text[SaveBox.Current].empty != 0)
					{
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
  return 1;
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

	CCD->Player()->DisableFog();				// Turn off fogging for cut scene
	CCD->Player()->DisableClipPlane();	// Turn off the clipping plane as well

//	Play the opening cut scene, if one exists

	CCD->PlayOpeningCutScene();

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

void CRFMenu::GameLevel()
{
	MSG msg;

	CCD->Player()->ShowFog();

//	Main game loop.  Until it's time for us to quit, we'll check for
//	..motion, move the player, etc.

  if(box6.Current == BOX_ON)
	CCD->Engine()->ShowFrameRate(true);

  geWorld_AddBitmap(CCD->World(), Crosshair);

  for(;;)
  {
		// If Winblows has something to say, take it in and pass it on in the
		// ..off-chance someone cares.
		if(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			GetMessage(&msg, NULL, 0, 0);
			DispatchMessage(&msg);
		}

		if(CCD->HandleGameInput() == true)
		{
			// Ok, send a Tick() call to all components that use time
			CCD->DispatchTick();
			if(box1.Current==BOX_ON && CCD->Player()->InFirstPerson() && CCD->Weapons()->CrossHair())	// display crosshair
			{
				if(!CCD->Weapons()->CrossHairFixed())
					DisplayCrossHair();
			}
			// Entities animated, render _EVERYTHING_
			CCD->Engine()->BeginFrame();				// Start up rendering for this frame
			CCD->RenderComponents();						// Render the RGF components
			CCD->Weapons()->Display();
			CCD->Weapons()->DoAttack();
			CCD->Engine()->RenderWorld();				// Render the world
			
			if(box1.Current==BOX_ON && CCD->Player()->InFirstPerson() && CCD->Weapons()->CrossHair())	// display crosshair
			{
				if(CCD->Weapons()->CrossHairFixed())
					CCD->Weapons()->DisplayCrossHair();
			}

			//CCD->NPCManager()->Debug();
			CCD->HUD()->Render();								// Render the HUD
			
			if(box5.Current == BOX_ON)
			{
				CCD->Weapons()->WeaponData();
				
				char szBug[256];
				geVec3d Pos = CCD->Player()->Position();
				sprintf(szBug, "Player X : %4.2f Player Y : %4.2f Player Z : %4.2f Actors %d", Pos.X, Pos.Y, Pos.Z, CCD->ActorManager()->CountActors());
				CCD->MenuManager()->FontRect(szBug, FONT10, 5, CCD->Engine()->Height()- 20);
			}
			
			CCD->Messages()->Display();
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
					for(nTemp = 1000; nTemp < 6000; nTemp += 400)
					{
						CCD->Engine()->BeginFrame();
						geFog_SetAttributes(theFog, &Pos, &cColor,
							0.0f, (geFloat)nTemp+500, 50.0f+(geFloat)(nTemp/200));
						CCD->Engine()->RenderWorld();
						CCD->Engine()->EndFrame();
					}
					for(nTemp = 6000; nTemp >= 0; nTemp -= 400)
					{
						CCD->Engine()->BeginFrame();
						geFog_SetAttributes(theFog, &Pos, &cColor,
							0.0f, (geFloat)nTemp+500, 50.0f+(geFloat)(nTemp/200));
						CCD->Engine()->RenderWorld();
						CCD->Engine()->EndFrame();
					}
					geWorld_RemoveFog(CCD->World(), theFog);
				}
				
				geWorld_RemoveBitmap(CCD->World(), Crosshair);
				CCD->Player()->DisableFog();			// Fogging OFF
				CCD->Player()->DisableClipPlane();	// Clip plane OFF
				CCD->Player()->LoadAttributes("pdoa.bin");
				CCD->Weapons()->ClearWeapon();
				CCD->ProcessLevelChange();
				CCD->Player()->SetAlive(true);
				CCD->Player()->ShowFog();					// Fog on, if any there
				CCD->Player()->ActivateClipPlane();	// Clip plane on, if any there
				geWorld_AddBitmap(CCD->World(), Crosshair);
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
					geWorld_RemoveBitmap(CCD->World(), Crosshair);
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
							m_Streams->Play(true);
						}
						ingame = 0;
						CCD->SetChangeLevel(false);
						return; 
					}
					CCD->ProcessLevelChange();			// Do the level change
					CCD->Player()->ShowFog();					// Fog on, if any there
					CCD->Player()->ActivateClipPlane();	// Clip plane on, if any there
					CCD->Player()->SaveAttributes("pdoa.bin");
					geWorld_AddBitmap(CCD->World(), Crosshair);
				}
			}
		}
		else
		{
			break;
		}

	}					// End main game loop
	geWorld_RemoveBitmap(CCD->World(), Crosshair);

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
		m_Streams->Play(true);
	}
}


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
	Vert.r = Vert.g = Vert.b = Vert.a = 255.0f;
	Vert.u = Vert.v = 0.0f;
	CCD->Collision()->IgnoreContents(false);
	CCD->Collision()->CheckLevel(RGF_COLLISIONLEVEL_1);
	if(CCD->Collision()->CheckForCollision(&theBox.Min, &theBox.Max,
				Pos, Back, &Collision, theActor))
	{
		CurrentDistance = (geFloat)fabs(geVec3d_DistanceBetween(&Collision.Impact, &Pos));
		if(CurrentDistance < 0.0f)
			CurrentDistance = 0.0f;
		if(CurrentDistance > 400.0f)
			CurrentDistance = 400.0f;
		geVec3d_AddScaled (&Pos, &Direction, CurrentDistance, &Back);
		int Percentage = 100;
		if(Collision.Actor)
		{
		}
		if(Collision.Model)
		{
			CCD->Damage()->IsDestroyable(Collision.Model, &Percentage);
		}
	}
	Vert.X = Back.X;
	Vert.Y = Back.Y;
	Vert.Z = Back.Z;
	Vert.r = Vert.g = Vert.b = 255.0f;
	geWorld_AddPolyOnce(CCD->World(), &Vert, 1, CCD->Weapons()->GetCrossHair(), GE_TEXTURED_POINT, GE_RENDER_DO_NOT_OCCLUDE_SELF, 1.0f);
}


//-----------------------------------------
// display splash screen with color 255
// as transparent
//-----------------------------------------

void CRFMenu::DisplaySplash(char *szSplashBMP)
{
	geBitmap *theBmp;
	geBitmap_Info	BmpInfo;
	int x, y;
	char menuline[132];

	strcpy(menuline,"menu\\");
	strcat(menuline,szSplashBMP);
	theBmp = CreateFromFileName(menuline);

	if(theBmp != NULL)
	{
		if(geBitmap_GetInfo(theBmp, &BmpInfo, NULL) == GE_TRUE)
		{
			x = (CCD->Engine()->Width() - BmpInfo.Width) / 2;
			y = (CCD->Engine()->Height() - BmpInfo.Height) / 2;
			if(geEngine_AddBitmap(CCD->Engine()->Engine(), theBmp) == GE_FALSE)
			{
				char szError[200];
				sprintf(szError,"DisplaySplash: addbitmap failed on '%s'\n", szSplashBMP);
				CCD->ReportError(szError, false);
				return;
			}
			geBitmap_SetColorKey(theBmp, GE_TRUE, 255, GE_FALSE);
			geEngine_BeginFrame(CCD->Engine()->Engine(), M_Camera, GE_FALSE);
			if(geEngine_DrawBitmap(CCD->Engine()->Engine(), theBmp, NULL, x, y ) == GE_FALSE)
			{
				char szError[200];
				sprintf(szError,"DisplaySplash: drawbitmap failed on '%s'\n", szSplashBMP);
				CCD->ReportError(szError, false);
			}
			geEngine_EndFrame(CCD->Engine()->Engine());
		 }
		 geBitmap_Destroy(&theBmp);
	}
	return;
}

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
	Vol_Slide.Current = Vol_Slide.Max_X;
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
	if(fd == NULL)
	{
		CCD->ReportError("Menu: can't open savedgames.rgf\n", false);
	}
	else
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
	if(fd == NULL)
		CCD->ReportError("CRFMenu: can't open settings file. using defaults", false);
	else
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
	  fclose(fd);
	}	 

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

  vol=(((float)percent/100.0f)*0.23f)+0.76f;
  if(percent == 0)
	  vol = 0.0f;
  geSound_SetMasterVolume(CCD->Engine()->AudioSystem(), vol);
//  if(CCD->MenuManager()->GetInGame() == 1)
//	CCD->AudioStreams()->SetVolume(vol);
}

//-------------------------------------
// Run the level
//-------------------------------------

static void RunGame()
{
	if(CCD->MenuManager()->GetMusicType() != -1)
	{
		CCD->MenuManager()->StopMenuMusic();
	}
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
	CCD->MenuManager()->DisplaySplash("loading.bmp");
	if((CCD->InitializeLevel(CCD->MenuManager()->GetLevelName())) != 0)
	{
		CCD->ReportError("Couldn't initialize first level", false);
		CCD->ShutdownLevel();						// Clean up anything that got loaded
		delete CCD;						// Kill off the engine and such
		MessageBox(NULL, CCD->MenuManager()->GetLevelName(),"RGF: Can't load level", MB_OK);
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

			time( &long_time );                /* Get time as long integer. */
			newtime = localtime( &long_time ); /* Convert to local time. */
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
			CCD->Player()->SaveTo(outFD);
			CCD->Doors()->SaveTo(outFD);
			CCD->Platforms()->SaveTo(outFD);
			CCD->Props()->SaveTo(outFD);
			CCD->Teleporters()->SaveTo(outFD);
			CCD->MorphingFields()->SaveTo(outFD);

			CCD->MIDIPlayer()->SaveTo(outFD);					
			CCD->CDPlayer()->SaveTo(outFD);
// EFFECTS
			CCD->Triggers()->SaveTo(outFD);					
			CCD->Logic()->SaveTo(outFD);
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
	if(SaveBox.Current != -1) // must have selected saved game
	{
		// must be visible in table
		if(SaveBox.Current>=SaveBox.Start && SaveBox.Current<(SaveBox.Max_Show+SaveBox.Start))
		{
			char filename[256];

			sprintf(filename, "savgame%d.sav",SaveBox.Current);
			FILE *inFD = CCD->OpenRFFile(kSavegameFile, filename, "rb");
			if(inFD == NULL)
			{
				CCD->ReportError("No savegame file to restore", false);
				return;
			}
			CCD->MenuManager()->DisplaySplash("loading.bmp");
			CCD->ShutdownLevel();
			if(CCD->CDPlayer())
				CCD->CDPlayer()->Stop();
			if(CCD->MIDIPlayer())
				CCD->MIDIPlayer()->Stop();
			CCD->Engine()->RestoreFrom(inFD);
			CCD->InitializeLevel(CCD->Engine()->LevelName());
			CCD->Player()->RestoreFrom(inFD);
			CCD->Doors()->RestoreFrom(inFD);
			CCD->Platforms()->RestoreFrom(inFD);
			CCD->Props()->RestoreFrom(inFD);
			CCD->Teleporters()->RestoreFrom(inFD);
			CCD->MorphingFields()->RestoreFrom(inFD);

			CCD->MIDIPlayer()->RestoreFrom(inFD);
			CCD->CDPlayer()->RestoreFrom(inFD);
// EFFECTS
			CCD->Triggers()->RestoreFrom(inFD);					
			CCD->Logic()->RestoreFrom(inFD);
			fclose(inFD);
			// run game
			if(CCD->MenuManager()->GetMusicType() != -1)
			{

			}
			CCD->MenuManager()->DisplaySplash("loading.bmp");
			CCD->MenuManager()->GameLoop();
			// set flag for game running
			CCD->MenuManager()->SetInGame();
		}
	}
}
