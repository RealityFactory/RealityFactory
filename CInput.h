/************************************************************************************//**
 * @file CInput.h
 * @brief Keyboard and mouse input handler
 *
 * This file contains the class declaration for the CInput class that
 * encapsulates all mouse and keyboard input for RGF-based games.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved
 ****************************************************************************************/

#ifndef __RGF_CINPUT_H__
#define __RGF_CINPUT_H__

#include "RFSX\\RFSX.h"

namespace RFSX
{
	class sxKey;
	class sxMouseButton;
	class sxAction;
};

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE CInput

#define SXINPUT_METHODS 18


#define MAX_KEYCODES		256
#define MAX_MOUSEBUTTONS	8

/**
 * @brief Keyboard and mouse input handler
 */
class CInput : public OIS::KeyListener, OIS::MouseListener, OIS::JoyStickListener, skExecutable
{
public:
	virtual ~CInput();

	void Initialize(HWND hWnd, int width, int height);
	void Capture();

	void AddKeyListener(OIS::KeyListener *keyListener, const std::string& instanceName);
	void AddMouseListener(OIS::MouseListener *mouseListener, const std::string& instanceName);
	void AddJoystickListener(OIS::JoyStickListener *joystickListener, const std::string& instanceName);

	void RemoveKeyListener(const std::string& instanceName);
	void RemoveMouseListener(const std::string& instanceName);
	void RemoveJoystickListener(const std::string& instanceName);

	void RemoveKeyListener(OIS::KeyListener *keyListener);
	void RemoveMouseListener(OIS::MouseListener *mouseListener);
	void RemoveJoystickListener(OIS::JoyStickListener *joystickListener);

	void RemoveAllListeners();
	void RemoveAllKeyListeners();
	void RemoveAllMouseListeners();
	void RemoveAllJoystickListeners();

	void SetWindowExtents(int width, int height);

	OIS::Mouse*    GetMouse();
	OIS::Keyboard* GetKeyboard();
	OIS::JoyStick* GetJoystick(unsigned int index);

	int GetNumberOfJoysticks();

	static CInput* GetSingletonPtr();

	int GetKeyAction(OIS::KeyCode key);
	int GetMouseAction(OIS::MouseButtonID button);

	void SetKeyAction(OIS::KeyCode key, int action) { m_KeyActionMap[key] = action; }
	void SetMouseAction(OIS::MouseButtonID button, int action) { m_MouseActionMap[button] = action; }

	/**
	 * @brief Clear key that action is bound to
	 */
	void ClearCodes(int action);

	void ClearKeyAction(int action);
	void ClearMouseAction(int action);

	/**
	 * @brief Get key that action is bound to
	 */
	int GetKeyCode(int action);
	/**
	 * @brief Get mouse button that action is bound to
	 */
	int GetMouseButtonID(int action);

	const std::string& GetKeyName(OIS::KeyCode key);

	void SetMouseButtonName(const std::string& name, OIS::MouseButtonID button);
	const std::string& GetMouseButtonName(OIS::MouseButtonID button);
	void ResetMouseButtonNames();

	void SetActionName(const std::string& name, int action);
	const std::string& GetActionName(int action);
	void ResetActionNames();

private:
	CInput();
	CInput(const CInput&) { }
	CInput& operator = (const CInput&);

	/**
	 @returns	true to keep remainder of input buffer
				false to discard remainder of input buffer
	 */
	bool keyPressed(const OIS::KeyEvent &e);
	bool keyReleased(const OIS::KeyEvent &e);

	bool mouseMoved(const OIS::MouseEvent &e);
	bool mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id);

	bool povMoved(const OIS::JoyStickEvent &e, int pov);
	bool axisMoved(const OIS::JoyStickEvent &e, int axis);
	bool sliderMoved(const OIS::JoyStickEvent &e, int sliderID);
	bool buttonPressed(const OIS::JoyStickEvent &e, int button);
	bool buttonReleased(const OIS::JoyStickEvent &e, int button);

	OIS::InputManager *m_InputSystem;
	OIS::Mouse        *m_Mouse;
	OIS::Keyboard     *m_Keyboard;

	std::vector<OIS::JoyStick*> m_Joysticks;
	std::vector<OIS::JoyStick*>::iterator itJoystick;
	std::vector<OIS::JoyStick*>::iterator itJoystickEnd;

	std::map<std::string, OIS::KeyListener*> m_KeyListeners;
	std::map<std::string, OIS::MouseListener*> m_MouseListeners;
	std::map<std::string, OIS::JoyStickListener*> m_JoystickListeners;

	std::map<std::string, OIS::KeyListener*>::iterator itKeyListener;
	std::map<std::string, OIS::MouseListener*>::iterator itMouseListener;
	std::map<std::string, OIS::JoyStickListener*>::iterator itJoystickListener;

	std::map<std::string, OIS::KeyListener*>::iterator itKeyListenerEnd;
	std::map<std::string, OIS::MouseListener*>::iterator itMouseListenerEnd;
	std::map<std::string, OIS::JoyStickListener*>::iterator itJoystickListenerEnd;

	int m_KeyActionMap[MAX_KEYCODES];
	int m_MouseActionMap[MAX_MOUSEBUTTONS];
	std::string m_MouseButtonNames[MAX_MOUSEBUTTONS];
	std::string m_ActionNames[RGF_K_MAXACTION];

	static std::string m_DefaultMouseButtonNames[];
	static std::string m_DefaultActionNames[];
	static CInput *m_InputManager;

public:
	/**
	 * @brief Set default key values
	 */
	void Default();

	/**
	 * @brief Save current keymaps to a user file
	 *
	 * Take the current Windows-keys to RGF-keys maps and save them off to a file.
	 * @param szFilename filename to save keymaps to
	 */
	int SaveKeymap(const std::string& filename);

	/**
	 * @brief Load saved keymaps from a user file
	 *
	 * Load a Windows-keys to RGF-keys map from a file into memory.
	 * @param szFilename filename to load keymaps from
	 */
	int LoadKeymap(const std::string& filename);

	/**
	 * @brief Get a key from the keyboard with no wait
	 */
	int GetKeyboardInputNoWait();

	/**
	 * @brief Check key status
	 */
	bool IsKeyDown(OIS::KeyCode key);

	/**
	 * @brief Check mouse button status
	 */
	bool IsMouseButtonDown(OIS::MouseButtonID button);

	/**
	 * @brief Set the mouse position
	 * @param pos_x x position in SCREEN coordinates
	 * @param pos_y y position in SCREEN coordinates
	 */
	void SetMousePos(long pos_x, long pos_y);

	void GetMousePos(long *pos_x, long *pos_y);

	/**
	 * @brief Centers the mouse on the game window
	 * @param center_x Recieves the x position in SCREEN coordinates
	 * @param center_y Recieves the y position in SCREEN coordinates
	 */
	void CenterMouse(long *center_x = NULL, long *center_y = NULL);

	int GetJoystickAxis(unsigned int joystick, unsigned int axis);
	bool IsJoystickButtonDown(unsigned int joystick, unsigned int button);

private:
	POINT m_MousePos; // SCREEN coordinates

	/*
	 * Scripting Interface
	 */
public:
	static void InitMHT();

	virtual bool getValue(const skString& name ,const skString& att, skRValue& val);
	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);
	virtual int executableType() const { return RFSXG_INPUT; }

protected:
	SX_METHOD_DECL(GetActionName);
	SX_METHOD_DECL(GetKey);
	SX_METHOD_DECL(GetKeyAction);
	SX_METHOD_DECL(GetKeyName);
	SX_METHOD_DECL(GetMouseButton);
	SX_METHOD_DECL(GetMouseButtonAction);
	SX_METHOD_DECL(GetMouseButtonName);
	SX_METHOD_DECL(GetJoystickAxisX);
	SX_METHOD_DECL(GetJoystickAxisY);
	SX_METHOD_DECL(GetJoystickAxisZ);
	SX_METHOD_DECL(GetNumberOfJoysticks);
	SX_METHOD_DECL(IsJoystickButtonDown);
	SX_METHOD_DECL(IsKeyDown);
	SX_METHOD_DECL(IsMouseButtonDown);
	SX_METHOD_DECL(KeyDown);
	SX_METHOD_DECL(SetEnabled);
	SX_METHOD_DECL(SetMousePosition);
	SX_METHOD_DECL(GetMousePosition);

private:
	RFSX::sxKey*			m_pKey;
	RFSX::sxMouseButton*	m_pMouseButton;
	RFSX::sxAction*			m_pAction;

	static RFSX::sxMHT<SX_IMPL_TYPE> h_method;
};

#undef SX_IMPL_TYPE

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
