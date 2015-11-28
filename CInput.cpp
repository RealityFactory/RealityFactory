/*
	CInput.cpp:		Keyboard and mouse input handler

	(c) 2001 Ralph Deane

	This file contains the class implementation for the CInput
class that encapsulates all mouse and keyboard input for
RGF-based games.
*/

//	You only need the one, master include file.

#include "RabidFramework.h"

//	CInput
//
//	Default constructor

CInput::CInput()
{
//	Ok, let's set up the default input keymaps

  	memset(m_WindowKeys, 0, sizeof(int) * 100);
	memset(m_RGFKeys, 0, sizeof(int) * 100);

  	m_nMappedKeys = KEY_MAXIMUM;			// # of keys we have mapped

	if(LoadKeymap("keyboard.ini") == RGF_SUCCESS)
		return;

	Default();
	return;
}

void CInput::Default()
{
	m_WindowKeys[KEY_ESC] = VK_ESCAPE;		m_RGFKeys[KEY_ESC] = RGF_K_EXIT;
	m_WindowKeys[KEY_1] = 0x31;				m_RGFKeys[KEY_1] = RGF_K_WEAPON_1;
	m_WindowKeys[KEY_2] = 0x32;				m_RGFKeys[KEY_2] = RGF_K_WEAPON_2;
	m_WindowKeys[KEY_3] = 0x33;				m_RGFKeys[KEY_3] = RGF_K_WEAPON_3;
	m_WindowKeys[KEY_4] = 0x34;				m_RGFKeys[KEY_4] = RGF_K_WEAPON_4;
	m_WindowKeys[KEY_5] = 0x35;				m_RGFKeys[KEY_5] = RGF_K_WEAPON_5;
	m_WindowKeys[KEY_6] = 0x36;				m_RGFKeys[KEY_6] = RGF_K_WEAPON_6;
	m_WindowKeys[KEY_7] = 0x37;				m_RGFKeys[KEY_7] = RGF_K_WEAPON_7;
	m_WindowKeys[KEY_8] = 0x38;				m_RGFKeys[KEY_8] = RGF_K_WEAPON_8;
	m_WindowKeys[KEY_9] = 0x39;				m_RGFKeys[KEY_9] = RGF_K_WEAPON_9;
	m_WindowKeys[KEY_0] = 0x30;				m_RGFKeys[KEY_0] = RGF_K_WEAPON_0;
	m_WindowKeys[KEY_MINUS] = VK_SUBTRACT;	m_RGFKeys[KEY_MINUS] = RGF_K_ZOOM_IN;
	m_WindowKeys[KEY_BACK] = VK_BACK;		m_RGFKeys[KEY_BACK] = RGF_K_ZOOM_WEAPON;
	m_WindowKeys[KEY_TAB] = VK_TAB;			m_RGFKeys[KEY_TAB] = RGF_K_HUD;
	m_WindowKeys[KEY_Q] = 0x51;
	m_WindowKeys[KEY_W] = 0x57;
	m_WindowKeys[KEY_E] = 0x45;
// changed RF064
	m_WindowKeys[KEY_R] = 0x52;				m_RGFKeys[KEY_R] = RGF_K_RELOAD;
// end change RF064
	m_WindowKeys[KEY_T] = 0x54;
	m_WindowKeys[KEY_Y] = 0x59;
// changed RF063
	m_WindowKeys[KEY_U] = 0x55;				m_RGFKeys[KEY_U] = RGF_K_USE;
	m_WindowKeys[KEY_I] = 0x49;				m_RGFKeys[KEY_I] = RGF_K_INVENTORY;
// end change RF063
	m_WindowKeys[KEY_O] = 0x4f;
// changed RF064
	m_WindowKeys[KEY_P] = 0x50;				m_RGFKeys[KEY_P] = RGF_K_DROP;
// end change RF064
	m_WindowKeys[KEY_RETURN] = VK_RETURN;	
	m_WindowKeys[KEY_A] = 0x41;				m_RGFKeys[KEY_A] = RGF_K_CROUCH;
	m_WindowKeys[KEY_S] = 0x53;				m_RGFKeys[KEY_S] = RGF_K_FORWARD;
	m_WindowKeys[KEY_D] = 0x44;				m_RGFKeys[KEY_D] = RGF_K_JUMP;
	m_WindowKeys[KEY_F] = 0x46;
	m_WindowKeys[KEY_G] = 0x47;
	m_WindowKeys[KEY_H] = 0x48;				m_RGFKeys[KEY_H] = RGF_K_HOLSTER_WEAPON;
	m_WindowKeys[KEY_J] = 0x4a;
	m_WindowKeys[KEY_K] = 0x4b;
	m_WindowKeys[KEY_L] = 0x4c;				m_RGFKeys[KEY_L] = RGF_K_LIGHT;
	m_WindowKeys[KEY_SHIFT] = VK_SHIFT;		m_RGFKeys[KEY_SHIFT] = RGF_K_RUN;
	m_WindowKeys[KEY_Z] = 0x5a;				m_RGFKeys[KEY_Z] = RGF_K_LEFT; 
	m_WindowKeys[KEY_X] = 0x58;				m_RGFKeys[KEY_X] = RGF_K_BACKWARD;
	m_WindowKeys[KEY_C] = 0x43;				m_RGFKeys[KEY_C] = RGF_K_RIGHT; 
	m_WindowKeys[KEY_V] = 0x56;
	m_WindowKeys[KEY_B] = 0x42;
	m_WindowKeys[KEY_N] = 0x4e;
	m_WindowKeys[KEY_M] = 0x4d;
// start multiplayer
	m_WindowKeys[KEY_PERIOD] = 0xbe;
// end multiplayer
	m_WindowKeys[KEY_SLASH] = 191;
	m_WindowKeys[KEY_SPACE] = VK_SPACE;		m_RGFKeys[KEY_SPACE] = RGF_K_SKIP;
	m_WindowKeys[KEY_ALT] = VK_MENU;		m_RGFKeys[KEY_ALT] = RGF_K_LOOKMODE;
	m_WindowKeys[KEY_CONTROL] = VK_CONTROL;	m_RGFKeys[KEY_CONTROL] = RGF_K_CAMERA;
	m_WindowKeys[KEY_PLUS] = VK_ADD;		m_RGFKeys[KEY_PLUS] = RGF_K_ZOOM_OUT;
	m_WindowKeys[KEY_INSERT] = VK_INSERT;	m_RGFKeys[KEY_INSERT] = RGF_K_CAMERA_RESET;
	m_WindowKeys[KEY_HOME] = VK_HOME;
	m_WindowKeys[KEY_PAGEUP] = VK_PRIOR;	m_RGFKeys[KEY_PAGEUP] = RGF_K_POWERUP;
	m_WindowKeys[KEY_DELETE] = VK_DELETE;	m_RGFKeys[KEY_DELETE] = RGF_K_LOOKSTRAIGHT;
	m_WindowKeys[KEY_END] = VK_END;
	m_WindowKeys[KEY_PAGEDOWN] = VK_NEXT;	m_RGFKeys[KEY_PAGEDOWN] = RGF_K_POWERDWN;
	m_WindowKeys[KEY_UP] = VK_UP;			m_RGFKeys[KEY_UP] = RGF_K_LOOKUP;
	m_WindowKeys[KEY_DOWN] = VK_DOWN;		m_RGFKeys[KEY_DOWN] = RGF_K_LOOKDOWN;
	m_WindowKeys[KEY_LEFT] = VK_LEFT;		m_RGFKeys[KEY_LEFT] = RGF_K_TURN_LEFT;
	m_WindowKeys[KEY_RIGHT] = VK_RIGHT;		m_RGFKeys[KEY_RIGHT] = RGF_K_TURN_RIGHT;
	m_WindowKeys[KEY_F1] = VK_F1;			m_RGFKeys[KEY_F1] = RGF_K_FIRST_PERSON_VIEW;
	m_WindowKeys[KEY_F2] = VK_F2;			m_RGFKeys[KEY_F2] = RGF_K_THIRD_PERSON_VIEW;
	m_WindowKeys[KEY_F3] = VK_F3;			m_RGFKeys[KEY_F3] = RGF_K_ISO_VIEW;
	m_WindowKeys[KEY_F4] = VK_F4;
	m_WindowKeys[KEY_F5] = VK_F5;
	m_WindowKeys[KEY_F6] = VK_F6;
	m_WindowKeys[KEY_F7] = VK_F7;
	m_WindowKeys[KEY_F8] = VK_F8;			m_RGFKeys[KEY_F8] = RGF_K_SCRNSHOT; // Update #1
	m_WindowKeys[KEY_F9] = VK_F9;			m_RGFKeys[KEY_F9] = RGF_K_QUICKSAVE;
	m_WindowKeys[KEY_F10] = VK_F10;			m_RGFKeys[KEY_F10] = RGF_K_QUICKLOAD;
	m_WindowKeys[KEY_F11] = VK_F11;	
	m_WindowKeys[KEY_F12] = VK_F12;			m_RGFKeys[KEY_F12] = RGF_K_HELP;
	m_WindowKeys[KEY_SYSRQ] = VK_SNAPSHOT;
	m_WindowKeys[KEY_LBUTTON] = VK_LBUTTON;	m_RGFKeys[KEY_LBUTTON] = RGF_K_ALTFIRE;
	m_WindowKeys[KEY_RBUTTON] = VK_RBUTTON;	m_RGFKeys[KEY_RBUTTON] = RGF_K_FIRE;
	m_WindowKeys[KEY_MBUTTON] = 0x04; // update #2
	m_WindowKeys[KEY_LBRACKET] = 0xdb;
	m_WindowKeys[KEY_RBRACKET] = 0xdd;
	m_WindowKeys[KEY_EQUAL] = 0xbb;
	m_WindowKeys[KEY_BACKSLASH] = 0xdc;
	m_WindowKeys[KEY_SEMICOLON] = 0xba;
	m_WindowKeys[KEY_COMMA] = 0xbc;
// start multiplayer
	m_WindowKeys[KEY_TILDE] = 0xc0;		m_RGFKeys[KEY_TILDE] = RGF_K_CONSOLE;
// end multiplayer
	m_WindowKeys[KEY_APOSTROPHE] = 0xde;
// start multiplayer
	m_WindowKeys[KEY_DECIMAL] = VK_DECIMAL;
// end multiplayer

  return;
}

//	~CInput
//
//	Default destructor

CInput::~CInput()
{
  return;
}

//	GetFirstInput
//
//	This routine scans the keyboard and the mouse buttons, gathers in
//	..the current state of the input system and returns the
//	..FIRST key status from the input stack.  To retrieve the
//	..rest of the keys in the input stack, use GetNextInput().
//	..A return value of RGF_NO_INPUT means no user input.

int CInput::GetFirstInput()
{
  ScanKeyboardInput();

	if(m_KeyStackCount == 0)
	  return RGF_NO_INPUT;				// No input from the user

  m_KeyStackCount--;						// One less on the stack

  return m_KeyStack[m_KeyStackPointer++];
}

//	GetNextInput
//
//	This routine pulls the next key down off the user input
//	..stack, if there is one, and returns it to the caller.
//	..A return value of RGF_NO_INPUT indicates 'end of input'.

int CInput::GetNextInput()
{
  if(m_KeyStackCount == 0)
	  return RGF_NO_INPUT;				// We're fresh out of input

  m_KeyStackCount--;						// One less input to process

	return m_KeyStack[m_KeyStackPointer++];
}

//	ScanKeyboardInput
//
//	Check the current state of the keyboard and return what's
//	..happening.  All currently pressed keys are put into the
//	..'key stack', a FIFO list of pressed keys that can be
//	..sequentially processed by the caller.  Note that the
//	..three modifier keys (SHIFT, CONTROL, and ALT) are always
//	..guaranteed to appear prior to any other keypresses.

void CInput::ScanKeyboardInput()
{
  m_KeyStackCount = 0;
	m_KeyStackPointer = 0;

//	First, check for SHIFT, CONTROL, and ALT

  if((GetAsyncKeyState(m_WindowKeys[KEY_CONTROL]) & 0x8000) != 0)
		m_KeyStack[m_KeyStackCount++] = m_RGFKeys[KEY_CONTROL];

  if((GetAsyncKeyState(m_WindowKeys[KEY_SHIFT]) & 0x8000) != 0)
		m_KeyStack[m_KeyStackCount++] = m_RGFKeys[KEY_SHIFT];

  if((GetAsyncKeyState(m_WindowKeys[KEY_ALT]) & 0x8000) != 0)
		m_KeyStack[m_KeyStackCount++] = m_RGFKeys[KEY_ALT];

//	Ok, we've checked the state of the modifier keys, now on
//	..to the rest we're going to process.  We have a table
//	..of WINDOWS KEYS (the usual VK_whatever) that are mapped
//	..to RGF KEYS (in the m_RGFKeys array).  By doing this,
//	..it's possible to save and load user keymaps.  Note that
//	..it's not possible to remap the modifier keys, this is
//	..by design.

  for(int nTemp = 0; nTemp < m_nMappedKeys; nTemp++)
  {
    if((GetAsyncKeyState(m_WindowKeys[nTemp]) & 0x8000) != 0)
		  m_KeyStack[m_KeyStackCount++] = m_RGFKeys[nTemp];
  }

  return;
}

//PWX
int CInput::GetRGFKey(int key)
{
	int value;
	value = m_RGFKeys[key];
	return value;
}
//PWX

//-------------------------------
// get a key from the keyboard and wait until released
//-------------------------------

int CInput::GetKeyboardInput()
{
	  for(int nTemp = 0; nTemp < m_nMappedKeys; nTemp++)
	  {
		if((GetAsyncKeyState(m_WindowKeys[nTemp]) & 0x8000) != 0)
		{
			while((GetAsyncKeyState(m_WindowKeys[nTemp]) & 0x8000) != 0)
			{
			}
			return nTemp;
		}
	  }
	  return -1;
}

// changed RF064
//-------------------------------
// get a key from the keyboard with no wait
//-------------------------------

int CInput::GetKeyboardInputNoWait()
{
	  for(int nTemp = 0; nTemp < m_nMappedKeys; nTemp++)
	  {
		if((GetAsyncKeyState(m_WindowKeys[nTemp]) & 0x8000) != 0)
		{
			return nTemp;
		}
	  }
	  return -1;
}

//-----------------------------------------------------------------------
bool CInput::GetKeyCheck(int keytemp)
{
		if((GetAsyncKeyState(m_WindowKeys[keytemp]) & 0x8000) != 0)
		{
			return true;
		}
	  return false;
}
//---------------------------------------------------------------------------

// end change RF064

//	SaveKeymap
//
//	Take the current Windows-keys to RGF-keys maps and save 'em
//	..off to a file.

int CInput::SaveKeymap(char *szFilename)
{
  FILE *fd = CCD->OpenRFFile(kInstallFile, szFilename, "wb");

	if(fd == NULL)
	  {
		char szBug[200];
		sprintf(szBug,"CInput: can't create keymap file %s\n", szFilename);
		CCD->ReportError(szBug, false);
		return RGF_FAILURE;										// Fatal error
		}

  for(int nTemp = 0; nTemp < 100; nTemp++)
	  {
		fwrite(&m_WindowKeys[nTemp], sizeof(int), 1, fd);		// Window key out
		fwrite(&m_RGFKeys[nTemp], sizeof(int), 1, fd);				// RGF key out
		}
		 
  fclose(fd);

  return RGF_SUCCESS;
}

//	LoadKeymap
//
//	Load a Windows-keys to RGF-keys map from a file into memory.

int CInput::LoadKeymap(char *szFilename)
{
  FILE *fd = CCD->OpenRFFile(kInstallFile, szFilename, "rb");

	if(fd == NULL)
		return RGF_FAILURE;	

  for(int nTemp = 0; nTemp < 100; nTemp++)
	  {
		fread(&m_WindowKeys[nTemp], sizeof(int), 1, fd);		// Window key in
		fread(&m_RGFKeys[nTemp], sizeof(int), 1, fd);			// RGF key in
		}
		 
  fclose(fd);

  return RGF_SUCCESS;
}

//---------------------------------
// get key that action is bound to
//---------------------------------

int CInput::GetCodes(int action)
{
	for(int nTemp = 0; nTemp < m_nMappedKeys; nTemp++)
	{
		if(m_RGFKeys[nTemp] == action)
			return nTemp;
	}
	return -1;
}


//---------------------------------
// clear key that action is bound to
//---------------------------------

void CInput::ClearCodes(int action)
{
	for(int nTemp = 0; nTemp < m_nMappedKeys; nTemp++)
	{
		if(m_RGFKeys[nTemp] == action)
		{
			m_RGFKeys[nTemp] = 0;
			return;
		}
	}
	return;
}

//	-------------------------------------------------------------------------


