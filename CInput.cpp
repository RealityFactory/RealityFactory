/************************************************************************************//**
 * @file CInput.cpp
 * @brief Keyboard and mouse input handler
 *
 * This file contains the class implementation for the CInput class that
 * encapsulates all mouse and keyboard input for RGF-based games.
 * @author Ralph Deane
 * @author Daniel Queteschiner
 *//*
 ****************************************************************************************/

#include "RabidFramework.h"
#include "CFileManager.h"
#include "CScriptManager.h"
#include "CInputKey.h"
#include "CInputMouseButton.h"
#include "CInputAction.h"
#include "RFSX/CEGUI/sxCEGUIVector2.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE CInput

CInput* CInput::m_InputManager;

std::string CInput::m_DefaultActionNames[] =
{
	"No Action",
	"Move Forward",
	"Move Backward",
	"Turn Left",
	"Turn Right",
	"Strafe Left",
	"Strafe Right",
	"Jump",
	"Crouch",
	"Look Up",
	"Look Down",
	"Look Straight",
	"Attack",
	"Alt Attack",
	"Weapon 0",
	"Weapon 1",
	"Weapon 2",
	"Weapon 3",
	"Weapon 4",
	"Weapon 5",
	"Weapon 6",
	"Weapon 7",
	"Weapon 8",
	"Weapon 9",
	"Run",
	"Camera Mode",
	"Zoom In",
	"Zoom Out",
	"Camera Reset",
	"Exit",
	"Quick Save",
	"Quick Load",
	"1st Person",
	"3rd Person",
	"Isometric",
	"Skip",
	"Help",
	"HUD",
	"Look Mode",
	"Screenshot",
	"Zoom Weapon",
	"Holster Weapon",
	"Light On/Off",
	"Use Item",
	"Inventory",
	"Console",
	"Drop Weapon",
	"Reload Weapon",
	"Increase Power",
	"Decrease Power",
	"Max Action"
};

std::string CInput::m_DefaultMouseButtonNames[] =
{
	"Left Mouse Button",
	"Right Mouse Button",
	"Middle Mouse Button",
	"Mouse Button 3",
	"Mouse Button 4",
	"Mouse Button 5",
	"Mouse Button 6",
	"Mouse Button 7"
};

/* ------------------------------------------------------------------------------------ */
// Default constructor
/* ------------------------------------------------------------------------------------ */
CInput::CInput() : m_InputSystem(0), m_Mouse(0), m_Keyboard(0)
{
	memset(m_KeyActionMap, 0, sizeof(m_KeyActionMap));
	memset(m_MouseActionMap, 0, sizeof(m_MouseActionMap));

	m_MousePos.x = m_MousePos.y = 0;

	m_pKey = new RFSX::sxKey();
	m_pMouseButton = new RFSX::sxMouseButton();
	m_pAction = new RFSX::sxAction();

	ResetActionNames();
	ResetMouseButtonNames();

	if(LoadKeymap("keyboard.ini") == RGF_SUCCESS)
		return;

	Default();
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
CInput::~CInput()
{
	ScriptManager::RemoveGlobalVariable("Input");

	delete m_pKey;
	delete m_pMouseButton;
	delete m_pAction;

	if(m_InputSystem)
	{
		if(m_Mouse)
		{
			m_InputSystem->destroyInputObject(m_Mouse);
		}

		if(m_Keyboard)
		{
			m_InputSystem->destroyInputObject(m_Keyboard);
		}

		if(m_Joysticks.size() > 0)
		{
			itJoystick    = m_Joysticks.begin();
			itJoystickEnd = m_Joysticks.end();

			for(; itJoystick != itJoystickEnd; ++itJoystick)
			{
				m_InputSystem->destroyInputObject(*itJoystick);
			}

			m_Joysticks.clear();
		}

		OIS::InputManager::destroyInputSystem(m_InputSystem);

		// Clear Listeners
		m_KeyListeners.clear();
		m_MouseListeners.clear();
		m_JoystickListeners.clear();
	}
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
void CInput::Initialize(HWND hWnd, int width, int height)
{
	if(!m_InputSystem)
	{
		// Setup basic variables
		OIS::ParamList paramList;
		std::ostringstream windowHndStr;

		// Fill parameter list
		windowHndStr << (unsigned int)hWnd;
		paramList.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

		if(!CCD->Engine()->FullScreen())
		{
			paramList.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND" )));
			paramList.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
		}

		// Create inputsystem
		m_InputSystem = OIS::InputManager::createInputSystem(paramList);

		// If possible create a buffered keyboard
		if(m_InputSystem->getNumberOfDevices(OIS::OISKeyboard) > 0)
		{
			m_Keyboard = static_cast<OIS::Keyboard*>(m_InputSystem->createInputObject(OIS::OISKeyboard, true));
			m_Keyboard->setEventCallback(this);
		}

		// If possible create a buffered mouse
		if(m_InputSystem->getNumberOfDevices(OIS::OISMouse) > 0)
		{
			m_Mouse = static_cast<OIS::Mouse*>(m_InputSystem->createInputObject(OIS::OISMouse, true));
			m_Mouse->setEventCallback(this);

			// Set mouse region
			this->SetWindowExtents(width, height);
		}

		// If possible create all joysticks in buffered mode
		if(m_InputSystem->getNumberOfDevices(OIS::OISJoyStick) > 0)
		{
			m_Joysticks.resize(m_InputSystem->getNumberOfDevices(OIS::OISJoyStick));

			itJoystick    = m_Joysticks.begin();
			itJoystickEnd = m_Joysticks.end();

			for(; itJoystick != itJoystickEnd; ++itJoystick)
			{
				(*itJoystick) = static_cast<OIS::JoyStick*>(m_InputSystem->createInputObject(OIS::OISJoyStick, true));
				(*itJoystick)->setEventCallback(this);
			}
		}

		sxLog::GetSingletonPtr()->Info("Found %d joysticks", m_InputSystem->getNumberOfDevices(OIS::OISJoyStick));
	}
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
void CInput::Capture()
{
	// Need to capture / update each device every frame
	if(m_Mouse)
	{
		m_Mouse->capture();
	}

	if(m_Keyboard)
	{
		m_Keyboard->capture();
	}

	if(m_Joysticks.size() > 0)
	{
		itJoystick    = m_Joysticks.begin();
		itJoystickEnd = m_Joysticks.end();

		for(; itJoystick != itJoystickEnd; ++itJoystick)
		{
			(*itJoystick)->capture();
		}
	}
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
void CInput::AddKeyListener(OIS::KeyListener *keyListener, const std::string& instanceName)
{
	if(m_Keyboard)
	{
		// Check for duplicate items
		itKeyListener = m_KeyListeners.find(instanceName);
		if(itKeyListener == m_KeyListeners.end())
		{
			m_KeyListeners[instanceName] = keyListener;
		}
		else
		{
			// Duplicate Item
		}
	}
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
void CInput::AddMouseListener(OIS::MouseListener *mouseListener, const std::string& instanceName)
{
	if(m_Mouse)
	{
		// Check for duplicate items
		itMouseListener = m_MouseListeners.find(instanceName);
		if(itMouseListener == m_MouseListeners.end())
		{
			m_MouseListeners[instanceName] = mouseListener;
		}
		else
		{
			// Duplicate Item
		}
	}
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
void CInput::AddJoystickListener(OIS::JoyStickListener *joystickListener, const std::string& instanceName)
{
	if(m_Joysticks.size() > 0)
	{
		// Check for duplicate items
		itJoystickListener = m_JoystickListeners.find(instanceName);
		if(itJoystickListener != m_JoystickListeners.end())
		{
			m_JoystickListeners[instanceName] = joystickListener;
		}
		else
		{
			// Duplicate Item
		}
	}
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
void CInput::RemoveKeyListener(const std::string& instanceName)
{
	// Check if item exists
	itKeyListener = m_KeyListeners.find(instanceName);
	if(itKeyListener != m_KeyListeners.end())
	{
		m_KeyListeners.erase(itKeyListener);
	}
	else
	{
		// Doesn't Exist
	}
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
void CInput::RemoveMouseListener(const std::string& instanceName)
{
	// Check if item exists
	itMouseListener = m_MouseListeners.find(instanceName);
	if(itMouseListener != m_MouseListeners.end())
	{
		m_MouseListeners.erase(itMouseListener);
	}
	else
	{
		// Doesn't Exist
	}
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
void CInput::RemoveJoystickListener(const std::string& instanceName)
{
	// Check if item exists
	itJoystickListener = m_JoystickListeners.find(instanceName);
	if(itJoystickListener != m_JoystickListeners.end())
	{
		m_JoystickListeners.erase(itJoystickListener);
	}
	else
	{
		// Doesn't Exist
	}
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
void CInput::RemoveKeyListener(OIS::KeyListener *keyListener)
{
	itKeyListener    = m_KeyListeners.begin();
	itKeyListenerEnd = m_KeyListeners.end();
	for(; itKeyListener != itKeyListenerEnd; ++itKeyListener)
	{
		if(itKeyListener->second == keyListener)
		{
			m_KeyListeners.erase(itKeyListener);
			break;
		}
	}
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
void CInput::RemoveMouseListener(OIS::MouseListener *mouseListener)
{
	itMouseListener    = m_MouseListeners.begin();
	itMouseListenerEnd = m_MouseListeners.end();
	for(; itMouseListener != itMouseListenerEnd; ++itMouseListener)
	{
		if(itMouseListener->second == mouseListener)
		{
			m_MouseListeners.erase(itMouseListener);
			break;
		}
	}
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
void CInput::RemoveJoystickListener(OIS::JoyStickListener *joystickListener)
{
	itJoystickListener    = m_JoystickListeners.begin();
	itJoystickListenerEnd = m_JoystickListeners.end();
	for(; itJoystickListener != itJoystickListenerEnd; ++itJoystickListener)
	{
		if(itJoystickListener->second == joystickListener)
		{
			m_JoystickListeners.erase(itJoystickListener);
			break;
		}
	}
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
void CInput::RemoveAllListeners()
{
	m_KeyListeners.clear();
	m_MouseListeners.clear();
	m_JoystickListeners.clear();
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
void CInput::RemoveAllKeyListeners()
{
	m_KeyListeners.clear();
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
void CInput::RemoveAllMouseListeners()
{
	m_MouseListeners.clear();
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
void CInput::RemoveAllJoystickListeners()
{
	m_JoystickListeners.clear();
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
void CInput::SetWindowExtents(int width, int height)
{
	// Set mouse region (if window resizes, we should alter this to reflect as well)
	const OIS::MouseState &mouseState = m_Mouse->getMouseState();
	mouseState.width  = width;
	mouseState.height = height;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
OIS::Mouse* CInput::GetMouse()
{
	return m_Mouse;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
OIS::Keyboard* CInput::GetKeyboard()
{
	return m_Keyboard;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
OIS::JoyStick* CInput::GetJoystick(unsigned int index)
{
	// Make sure it's a valid index
	if(index < m_Joysticks.size())
	{
		return m_Joysticks[index];
	}

	return NULL;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
int CInput::GetNumberOfJoysticks()
{
	// Cast to keep compiler happy
	return static_cast<int>(m_Joysticks.size());
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
bool CInput::keyPressed(const OIS::KeyEvent &e)
{
	if(m_KeyListeners.empty())
		return true;

	itKeyListener    = m_KeyListeners.begin();
	itKeyListenerEnd = m_KeyListeners.end();
	for(; itKeyListener != itKeyListenerEnd; ++itKeyListener)
	{
		if(!itKeyListener->second->keyPressed(e))
			return false;
	}

	return true;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
bool CInput::keyReleased(const OIS::KeyEvent &e)
{
	if(m_KeyListeners.empty())
		return true;

	itKeyListener    = m_KeyListeners.begin();
	itKeyListenerEnd = m_KeyListeners.end();
	for(; itKeyListener != itKeyListenerEnd; ++itKeyListener)
	{
		if(!itKeyListener->second->keyReleased(e))
			return false;
	}

	return true;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
bool CInput::mouseMoved(const OIS::MouseEvent &e)
{
	// keep track of our own mouse position as OIS starts out at (0,0)
	// and this can't be changed
	if(!CCD->Engine()->FullScreen())
	{
		// relative mouse coordinate are garbage in window mode
		m_MousePos.x = e.state.X.abs;
		m_MousePos.y = e.state.Y.abs;
	}
	else
	{
		m_MousePos.x += e.state.X.rel;
		m_MousePos.y += e.state.Y.rel;
	}

	if(m_MouseListeners.empty())
		return true;

	itMouseListener    = m_MouseListeners.begin();
	itMouseListenerEnd = m_MouseListeners.end();
	for(; itMouseListener != itMouseListenerEnd; ++itMouseListener)
	{
		if(!itMouseListener->second->mouseMoved(e))
			return false;
	}

	return true;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
bool CInput::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	if(m_MouseListeners.empty())
		return true;

	itMouseListener    = m_MouseListeners.begin();
	itMouseListenerEnd = m_MouseListeners.end();
	for(; itMouseListener != itMouseListenerEnd; ++itMouseListener)
	{
		if(!itMouseListener->second->mousePressed(e, id))
			return false;
	}

	return true;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
bool CInput::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	if(m_MouseListeners.empty())
		return true;

	itMouseListener    = m_MouseListeners.begin();
	itMouseListenerEnd = m_MouseListeners.end();
	for(; itMouseListener != itMouseListenerEnd; ++itMouseListener)
	{
		if(!itMouseListener->second->mouseReleased(e, id))
			return false;
	}

	return true;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
bool CInput::povMoved(const OIS::JoyStickEvent &e, int pov)
{
	itJoystickListener    = m_JoystickListeners.begin();
	itJoystickListenerEnd = m_JoystickListeners.end();
	for(; itJoystickListener != itJoystickListenerEnd; ++itJoystickListener)
	{
		itJoystickListener->second->povMoved(e, pov);
	}

	return true;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
bool CInput::axisMoved(const OIS::JoyStickEvent &e, int axis)
{
	itJoystickListener    = m_JoystickListeners.begin();
	itJoystickListenerEnd = m_JoystickListeners.end();
	for(; itJoystickListener != itJoystickListenerEnd; ++itJoystickListener)
	{
		itJoystickListener->second->axisMoved(e, axis);
	}

	return true;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
bool CInput::sliderMoved(const OIS::JoyStickEvent &e, int sliderID)
{
	itJoystickListener    = m_JoystickListeners.begin();
	itJoystickListenerEnd = m_JoystickListeners.end();
	for(; itJoystickListener != itJoystickListenerEnd; ++itJoystickListener)
	{
		itJoystickListener->second->sliderMoved(e, sliderID);
	}

	return true;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
bool CInput::buttonPressed(const OIS::JoyStickEvent &e, int button)
{
	itJoystickListener    = m_JoystickListeners.begin();
	itJoystickListenerEnd = m_JoystickListeners.end();
	for(; itJoystickListener != itJoystickListenerEnd; ++itJoystickListener)
	{
		itJoystickListener->second->buttonPressed(e, button);
	}

	return true;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
bool CInput::buttonReleased(const OIS::JoyStickEvent &e, int button)
{
	itJoystickListener    = m_JoystickListeners.begin();
	itJoystickListenerEnd = m_JoystickListeners.end();
	for(; itJoystickListener != itJoystickListenerEnd; ++itJoystickListener)
	{
		itJoystickListener->second->buttonReleased(e, button);
	}

	return true;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
CInput* CInput::GetSingletonPtr()
{
	if(!m_InputManager)
	{
		m_InputManager = new CInput();
		ScriptManager::AddGlobalVariable("Input", m_InputManager);
	}

	return m_InputManager;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
int CInput::GetKeyAction(OIS::KeyCode key)
{
	return m_KeyActionMap[key];
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
int CInput::GetMouseAction(OIS::MouseButtonID button)
{
	return m_MouseActionMap[button];
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
void CInput::ClearKeyAction(int action)
{
	int id = GetKeyCode(action);
	if(id != -1)
		SetKeyAction((OIS::KeyCode)id, RGF_K_NOACTION);
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
void CInput::ClearMouseAction(int action)
{
	int id = GetMouseButtonID(action);
	if(id != -1)
		SetMouseAction((OIS::MouseButtonID)id, RGF_K_NOACTION);
}


/* ------------------------------------------------------------------------------------ */
//	Default
//
//	Set default values
/* ------------------------------------------------------------------------------------ */
void CInput::Default()
{
	m_KeyActionMap[OIS::KC_ESCAPE]	= RGF_K_EXIT;
	m_KeyActionMap[OIS::KC_1]		= RGF_K_WEAPON_1;
	m_KeyActionMap[OIS::KC_2]		= RGF_K_WEAPON_2;
	m_KeyActionMap[OIS::KC_3]		= RGF_K_WEAPON_3;
	m_KeyActionMap[OIS::KC_4]		= RGF_K_WEAPON_4;
	m_KeyActionMap[OIS::KC_5]		= RGF_K_WEAPON_5;
	m_KeyActionMap[OIS::KC_6]		= RGF_K_WEAPON_6;
	m_KeyActionMap[OIS::KC_7]		= RGF_K_WEAPON_7;
	m_KeyActionMap[OIS::KC_8]		= RGF_K_WEAPON_8;
	m_KeyActionMap[OIS::KC_9]		= RGF_K_WEAPON_9;
	m_KeyActionMap[OIS::KC_0]		= RGF_K_WEAPON_0;
	m_KeyActionMap[OIS::KC_BACK]	= RGF_K_ZOOM_WEAPON;
	m_KeyActionMap[OIS::KC_TAB]		= RGF_K_HUD;
	m_KeyActionMap[OIS::KC_W]		= RGF_K_FORWARD;
	m_KeyActionMap[OIS::KC_E]		= RGF_K_JUMP;
	m_KeyActionMap[OIS::KC_R]		= RGF_K_RELOAD;
	m_KeyActionMap[OIS::KC_U]		= RGF_K_USE;
	m_KeyActionMap[OIS::KC_I]		= RGF_K_INVENTORY;
	m_KeyActionMap[OIS::KC_P]		= RGF_K_DROP;
	m_KeyActionMap[OIS::KC_LCONTROL]= RGF_K_CAMERA;
	m_KeyActionMap[OIS::KC_A]		= RGF_K_LEFT;
	m_KeyActionMap[OIS::KC_S]		= RGF_K_BACKWARD;
	m_KeyActionMap[OIS::KC_D]		= RGF_K_RIGHT;
	m_KeyActionMap[OIS::KC_H]		= RGF_K_HOLSTER_WEAPON;
	m_KeyActionMap[OIS::KC_L]		= RGF_K_LIGHT;
	m_KeyActionMap[OIS::KC_GRAVE]	= RGF_K_CONSOLE;
	m_KeyActionMap[OIS::KC_LSHIFT]	= RGF_K_RUN;
	m_KeyActionMap[OIS::KC_C]		= RGF_K_CROUCH;
	m_KeyActionMap[OIS::KC_LMENU]	= RGF_K_LOOKMODE; // Alt
	m_KeyActionMap[OIS::KC_SPACE]	= RGF_K_SKIP;
	m_KeyActionMap[OIS::KC_F1]		= RGF_K_FIRST_PERSON_VIEW;
	m_KeyActionMap[OIS::KC_F2]		= RGF_K_THIRD_PERSON_VIEW;
	m_KeyActionMap[OIS::KC_F3]		= RGF_K_ISO_VIEW;
	m_KeyActionMap[OIS::KC_F8]		= RGF_K_SCRNSHOT;
	m_KeyActionMap[OIS::KC_F9]		= RGF_K_QUICKSAVE;
	m_KeyActionMap[OIS::KC_F10]		= RGF_K_QUICKLOAD;
	m_KeyActionMap[OIS::KC_SUBTRACT]= RGF_K_ZOOM_IN;
	m_KeyActionMap[OIS::KC_ADD]		= RGF_K_ZOOM_OUT;
	m_KeyActionMap[OIS::KC_F12]		= RGF_K_HELP;
	m_KeyActionMap[OIS::KC_UP]		= RGF_K_LOOKUP;
	m_KeyActionMap[OIS::KC_PGUP]	= RGF_K_POWERUP;
	m_KeyActionMap[OIS::KC_LEFT]	= RGF_K_TURN_LEFT;
	m_KeyActionMap[OIS::KC_RIGHT]	= RGF_K_TURN_RIGHT;
	m_KeyActionMap[OIS::KC_DOWN]	= RGF_K_LOOKDOWN;
	m_KeyActionMap[OIS::KC_PGDOWN]	= RGF_K_POWERDWN;
	m_KeyActionMap[OIS::KC_INSERT]	= RGF_K_CAMERA_RESET;
	m_KeyActionMap[OIS::KC_DELETE]	= RGF_K_LOOKSTRAIGHT;

	if(GetSystemMetrics(SM_SWAPBUTTON))
	{
		m_MouseActionMap[OIS::MB_Left]	= RGF_K_ALTFIRE;
		m_MouseActionMap[OIS::MB_Right]	= RGF_K_FIRE;
	}
	else
	{
		m_MouseActionMap[OIS::MB_Left]	= RGF_K_FIRE;
		m_MouseActionMap[OIS::MB_Right]	= RGF_K_ALTFIRE;
	}
}


/* ------------------------------------------------------------------------------------ */
// GetKeyboardInputNoWait
//
// get a key from the keyboard with no wait
/* ------------------------------------------------------------------------------------ */
int CInput::GetKeyboardInputNoWait()
{
	if(m_Keyboard)
	{
		for(int nKey=0; nKey<MAX_KEYCODES; ++nKey)
		{
			if(m_KeyActionMap[nKey])
				if(m_Keyboard->isKeyDown(static_cast<OIS::KeyCode>(nKey)))
					return nKey;
		}
	}

	return 0;
}

/* ------------------------------------------------------------------------------------ */
// IsKeyDown
/* ------------------------------------------------------------------------------------ */
bool CInput::IsKeyDown(OIS::KeyCode key)
{
	if(m_Keyboard)
		return m_Keyboard->isKeyDown(key);

	return false;
}

/* ------------------------------------------------------------------------------------ */
// Check mouse button status
/* ------------------------------------------------------------------------------------ */
bool CInput::IsMouseButtonDown(OIS::MouseButtonID button)
{
	if(m_Mouse)
		return m_Mouse->getMouseState().buttonDown(button);

	return false;
}


/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
int CInput::GetJoystickAxis(unsigned int joystick, unsigned int axis)
{
	if(joystick < m_Joysticks.size())
	{
		if(m_Joysticks[joystick]->getJoyStickState().mAxes.size() > axis)
			return m_Joysticks[joystick]->getJoyStickState().mAxes[axis].abs;
	}

	return 0;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
bool CInput::IsJoystickButtonDown(unsigned int joystick, unsigned int button)
{
	if(joystick < m_Joysticks.size())
	{
		if(m_Joysticks[joystick]->getJoyStickState().mButtons.size() > button)
			return m_Joysticks[joystick]->getJoyStickState().mButtons[button];
	}

	return false;
}


/* ------------------------------------------------------------------------------------ */
// SaveKeymap
//
// Take the current keys to actions maps and save them off to a file.
/* ------------------------------------------------------------------------------------ */
int CInput::SaveKeymap(const std::string& filename)
{
	FILE *fd = CFileManager::GetSingletonPtr()->OpenRFFile(kConfigFile, filename.c_str(), "wb");

	if(!fd)
	{
		CCD->Log()->Error("File %s - Line %d: Failed to create keymap file %s",
							__FILE__, __LINE__, filename.c_str());
		return RGF_FAILURE;
	}

	for(int nKey=0; nKey<MAX_KEYCODES; ++nKey)
	{
		fwrite(&m_KeyActionMap[nKey],	sizeof(int), 1, fd);
	}

	for(int nButton=0; nButton<MAX_MOUSEBUTTONS; ++nButton)
	{
		fwrite(&m_MouseActionMap[nButton],sizeof(int), 1, fd);
	}

	int swap = GetSystemMetrics(SM_SWAPBUTTON);
	fwrite(&swap, sizeof(int), 1, fd);

	fclose(fd);

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// LoadKeymap
//
// Load a Windows-keys to RGF-keys map from a file into memory.
/* ------------------------------------------------------------------------------------ */
int CInput::LoadKeymap(const std::string& filename)
{
	sxLog::GetSingletonPtr()->SetMBPriority(LP_CRITICAL);
	FILE *fd = CFileManager::GetSingletonPtr()->OpenRFFile(kConfigFile, filename.c_str(), "rb");
	sxLog::GetSingletonPtr()->SetMBPriority(LP_ERROR);

	if(!fd)
		return RGF_FAILURE;

	for(int nKey=0; nKey<MAX_KEYCODES; ++nKey)
	{
		fread(&m_KeyActionMap[nKey],	sizeof(int), 1, fd);
	}

	for(int nButton=0; nButton<MAX_MOUSEBUTTONS; ++nButton)
	{
		fread(&m_MouseActionMap[nButton],sizeof(int), 1, fd);
	}

	int swap;
	fread(&swap, sizeof(int), 1, fd);

	fclose(fd);

	if(swap != GetSystemMetrics(SM_SWAPBUTTON))
	{
		int temp = m_MouseActionMap[OIS::MB_Left];
		m_MouseActionMap[OIS::MB_Left] = m_MouseActionMap[OIS::MB_Right];
		m_MouseActionMap[OIS::MB_Right] = temp;
	}

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
int CInput::GetKeyCode(int action)
{
	for(int nKey=0; nKey<MAX_KEYCODES; ++nKey)
	{
		if(m_KeyActionMap[nKey] == action)
			return nKey;
	}

	return -1;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
int CInput::GetMouseButtonID(int action)
{
	for(int nButton=0; nButton<MAX_MOUSEBUTTONS; ++nButton)
	{
		if(m_MouseActionMap[nButton] == action)
			return nButton;
	}

	return -1;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
const std::string& CInput::GetKeyName(OIS::KeyCode key)
{
	return m_Keyboard->getAsString(key);
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
void CInput::SetMouseButtonName(const std::string& name, OIS::MouseButtonID button)
{
	m_MouseButtonNames[button] = name;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
const std::string& CInput::GetMouseButtonName(OIS::MouseButtonID button)
{
	return m_MouseButtonNames[button];
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
void CInput::ResetMouseButtonNames()
{
	for(int i=0; i<MAX_MOUSEBUTTONS; ++i)
		m_MouseButtonNames[i] = m_DefaultMouseButtonNames[i];
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
void CInput::SetActionName(const std::string& name, int action)
{
	if(action < 0 || action >= RGF_K_MAXACTION)
		return;

	m_ActionNames[action] = name;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
const std::string& CInput::GetActionName(int action)
{
	if(action < 0 || action >= RGF_K_MAXACTION)
		return m_ActionNames[0];

	return m_ActionNames[action];
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
void CInput::ResetActionNames()
{
	for(int i=0; i<RGF_K_MAXACTION; ++i)
		m_ActionNames[i] = m_DefaultActionNames[i];
}

/* ------------------------------------------------------------------------------------ */
// Clear key/button that action is bound to
/* ------------------------------------------------------------------------------------ */
void CInput::ClearCodes(int action)
{
	for(int nKey=0; nKey<MAX_MOUSEBUTTONS; ++nKey)
	{
		if(m_MouseActionMap[nKey] == action)
		{
			m_MouseActionMap[nKey] = RGF_K_NOACTION;
			return;
		}
	}

	for(int nKey=0; nKey<MAX_KEYCODES; ++nKey)
	{
		if(m_KeyActionMap[nKey] == action)
		{
			m_KeyActionMap[nKey] = RGF_K_NOACTION;
			return;
		}
	}
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
void CInput::GetMousePos(long *pos_x, long *pos_y)
{
	if(pos_x)	*pos_x = m_MousePos.x;
	if(pos_y)	*pos_y = m_MousePos.y;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
void CInput::SetMousePos(long pos_x, long pos_y)
{
	if(pos_x < 0) pos_x = 0;
	else if(pos_x > CCD->Engine()->Width()) pos_x = CCD->Engine()->Width();
	if(pos_y < 0) pos_y = 0;
	else if(pos_y > CCD->Engine()->Height()) pos_y = CCD->Engine()->Height();

	SetCursorPos(pos_x, pos_y);

	m_MousePos.x = pos_x;
	m_MousePos.y = pos_y;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
void CInput::CenterMouse(long *center_x, long *center_y)
{
	RECT client;
	POINT mPos;

	if(CCD->Engine()->FullScreen())
	{
		m_MousePos.x = CCD->Engine()->Width()/2;			// calculate the center of the screen
		m_MousePos.y = CCD->Engine()->Height()/2;			// calculate the center of the screen
		mPos.x = CCD->Engine()->Width()/2;			// calculate the center of the screen
		mPos.y = CCD->Engine()->Height()/2;			// calculate the center of the screen
	}
	else
	{
		GetClientRect(CCD->Engine()->WindowHandle(), &client);	// get the client area of the window
		m_MousePos.x = client.right/2;								// calculate the center of the client area
		m_MousePos.y = client.bottom/2;								// calculate the center of the client area
		ClientToScreen(CCD->Engine()->WindowHandle(), &m_MousePos);	// convert to SCREEN coordinates
		mPos.x = client.right/2;								// calculate the center of the client area
		mPos.y = client.bottom/2;								// calculate the center of the client area
		ClientToScreen(CCD->Engine()->WindowHandle(), &mPos);	// convert to SCREEN coordinates
	}

	SetCursorPos(mPos.x, mPos.y);					// put the cursor in the middle of the window

	if(center_x) *center_x = mPos.x;
	if(center_y) *center_y = mPos.y;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */

bool SX_IMPL_TYPE::method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
{
	if(h_method(this, m_name, args, r_val, ctxt))
		return true;
	else
		return skExecutable::method(m_name, args, r_val, ctxt);
}


bool SX_IMPL_TYPE::getValue(const skString& name ,const skString& att, skRValue& val)
{
	if(name == "Key")
	{
		val.assignObject(m_pKey);
		return true;
	}
	if(name == "MouseButton")
	{
		val.assignObject(m_pMouseButton);
		return true;
	}
	if(name == "Action")
	{
		val.assignObject(m_pAction);
		return true;
	}

	return skExecutable::getValue(name, att, val);
}


//////////////////////////////////////////////////////////////////////////////////////////
// CInput method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(GetActionName)
{
	if(args.entries() != 1)
		return false;

	r_val = skString(caller->GetActionName(args[0].intValue()).c_str());
	return true;
}


SX_METHOD_IMPL(GetKey) // from action
{
	if(args.entries() != 1)
		return false;

	r_val = caller->GetKeyCode(args[0].intValue());
	return true;
}


SX_METHOD_IMPL(GetKeyAction)
{
	if(args.entries() != 1)
		return false;

	r_val = caller->GetKeyAction(static_cast<OIS::KeyCode>(args[0].intValue()));
	return true;
}


SX_METHOD_IMPL(GetKeyName)
{
	if(args.entries() != 1)
		return false;

	r_val = skString(caller->GetKeyName(static_cast<OIS::KeyCode>(args[0].intValue())).c_str());
	return true;
}


SX_METHOD_IMPL(GetMouseButton) // from action
{
	if(args.entries() != 1)
		return false;

	r_val = caller->GetMouseButtonID(args[0].intValue());
	return true;
}


SX_METHOD_IMPL(GetMouseButtonAction)
{
	if(args.entries() != 1)
		return false;

	r_val = caller->GetMouseAction(static_cast<OIS::MouseButtonID>(args[0].intValue()));
	return true;
}


SX_METHOD_IMPL(GetMouseButtonName)
{
	if(args.entries() != 1)
		return false;

	r_val = skString(caller->GetMouseButtonName(static_cast<OIS::MouseButtonID>(args[0].intValue())).c_str());
	return true;
}

SX_METHOD_IMPL(GetJoystickAxisX)
{
	if(args.entries() != 1)
		return false;

	r_val = caller->GetJoystickAxis(args[0].intValue(), 0);
	return true;
}


SX_METHOD_IMPL(GetJoystickAxisY)
{
	if(args.entries() != 1)
		return false;

	r_val = caller->GetJoystickAxis(args[0].intValue(), 1);
	return true;
}


SX_METHOD_IMPL(GetJoystickAxisZ)
{
	if(args.entries() != 1)
		return false;

	r_val = caller->GetJoystickAxis(args[0].intValue(), 2);
	return true;
}


SX_METHOD_IMPL(GetNumberOfJoysticks)
{
	r_val = caller->GetNumberOfJoysticks();
	return true;
}


SX_METHOD_IMPL(IsJoystickButtonDown)
{
	if(args.entries() != 2)
		return false;

	r_val = caller->IsJoystickButtonDown(args[0].intValue(), args[1].intValue());
	return true;
}


SX_METHOD_IMPL(IsKeyDown)
{
	if(args.entries() != 1)
		return false;

	r_val = caller->IsKeyDown(static_cast<OIS::KeyCode>(args[0].intValue()));
	return true;
}


SX_METHOD_IMPL(IsMouseButtonDown)
{
	if(args.entries() != 1)
		return false;

	r_val = caller->IsMouseButtonDown(static_cast<OIS::MouseButtonID>(args[0].intValue()));
	return true;
}


SX_METHOD_IMPL(KeyDown)
{
	r_val = caller->GetKeyboardInputNoWait();
	return true;
}


SX_METHOD_IMPL(SetEnabled)
{
	if(args.entries() != 1)
		return false;

	// TODO: move from ccd to input class
	CCD->SetKeyPaused(!args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(SetMousePosition) // expects client-area coordinates
{
	if(args.entries() != 1)
		return false;

	RFSX::sxVector2 *vec = RFSX::IS_VECTOR2(args[0]);
	if(!vec)
		return false;

	POINT mPos = { static_cast<long>(vec->Vector2()->d_x), static_cast<long>(vec->Vector2()->d_y) };
	if(!CCD->Engine()->FullScreen())
	{
		ClientToScreen(CCD->Engine()->WindowHandle(), &mPos);
	}

	caller->SetMousePos(mPos.x, mPos.y);
	CEGUI::System::getSingleton().injectMousePosition(static_cast<float>(mPos.x), static_cast<float>(mPos.y));
	return true;
}


SX_METHOD_IMPL(GetMousePosition) // returns client-area coordinates
{
	POINT mPos;
	caller->GetMousePos(&mPos.x, &mPos.y);
	if(!CCD->Engine()->FullScreen())
	{
		ScreenToClient(CCD->Engine()->WindowHandle(), &mPos);
	}

	r_val.assignObject(new RFSX::sxVector2(static_cast<float>(mPos.x), static_cast<float>(mPos.y)), true);
	return true;
}


RFSX::SX_INIT_MHT(SXINPUT_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(GetActionName);
	SX_ADD_METHOD(GetKey);
	SX_ADD_METHOD(GetKeyAction);
	SX_ADD_METHOD(GetKeyName);
	SX_ADD_METHOD(GetMouseButton);
	SX_ADD_METHOD(GetMouseButtonAction);
	SX_ADD_METHOD(GetMouseButtonName);
	SX_ADD_METHOD(GetJoystickAxisX);
	SX_ADD_METHOD(GetJoystickAxisY);
	SX_ADD_METHOD(GetJoystickAxisZ);
	SX_ADD_METHOD(GetNumberOfJoysticks);
	SX_ADD_METHOD(IsJoystickButtonDown);
	SX_ADD_METHOD(IsKeyDown);
	SX_ADD_METHOD(IsMouseButtonDown);
	SX_ADD_METHOD(KeyDown);
	SX_ADD_METHOD(SetEnabled);
	SX_ADD_METHOD(SetMousePosition);
	SX_ADD_METHOD(GetMousePosition);
}


#undef SX_IMPL_TYPE

/* ----------------------------------- END OF FILE ------------------------------------ */

