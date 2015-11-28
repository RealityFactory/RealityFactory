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

/**
 * @brief Keyboard and mouse input handler
 */
class CInput
{
public:
	/**
	 * @brief Default constructor
	 */
	CInput();

	/**
	 * @brief Destructor
	 */
	~CInput();

	/**
	 * @brief Set default key values
	 */
	void Default();

	/**
	 * @brief Scan keyboard and mouse, return first input item
	 *
	 * This routine scans the keyboard and the mouse buttons, gathers in the
	 * current state of the input system and returns the FIRST key status from
	 * the input stack.  To retrieve the rest of the keys in the input stack,
	 * use GetNextInput(). A return value of RGF_NO_INPUT means no user input.
	 */
	int GetFirstInput();

	/**
	 * @brief Get next input item from input queue
	 *
	 * This routine pulls the next key down off the user input stack, if there
	 * is one, and returns it to the caller. A return value of RGF_NO_INPUT
	 * indicates 'end of input'.
	 */
	int GetNextInput();

	/**
	 * @brief Save current keymaps to a user file
	 *
	 * Take the current Windows-keys to RGF-keys maps and save them off to a file.
	 * @param szFilename filename to save keymaps to
	 */
	int SaveKeymap(const char *szFilename);

	/**
	 * @brief Load saved keymaps from a user file
	 *
	 * Load a Windows-keys to RGF-keys map from a file into memory.
	 * @param szFilename filename to load keymaps from
	 */
	int LoadKeymap(const char *szFilename);

	/**
	 * @brief Get key that action is bound to
	 */
	int GetCodes(int action);

	/**
	 * @brief Get action that is bound to key
	 */
	int GetRGFKey(int key);

	/**
	 * @brief Get a key from the keyboard and wait until released
	 */
	int GetKeyboardInput();

	/**
	 * @brief Get a key from the keyboard with no wait
	 */
	int GetKeyboardInputNoWait();

	/**
	 * @brief Check key status
	 */
	bool GetKeyCheck(int keytemp);

	/**
	 * @brief Get input translated to ASCII character
	 */
	int GetAscii();

	/**
	 * @brief Clear key that action is bound to
	 */
	void ClearCodes(int action);

	/**
	 * @brief Assign an action to a key
	 * @param key Key to bind action to
	 * @param action Type of action to bind to the key
	 */
	void SetKeyAction(int key, int action) { m_RGFKeys[key] = action; }

private:

	/**
	 * @brief Scan the keyboard for input from appropriate keys
	 *
	 * Check the current state of the keyboard and return what's happening. All
	 * currently pressed keys are put into the 'key stack', a FIFO list of
	 * pressed keys that can be sequentially processed by the caller. Note that
	 * the three modifier keys (SHIFT, CONTROL, and ALT) are always guaranteed
	 * to appear prior to any other keypresses.
	 */
	void ScanKeyboardInput();

private:

	int m_WindowKeys[100];		///< Up to 100 Windows keys in map
	int m_RGFKeys[100];			///< The 100 RGF keys they map to
	int m_nMappedKeys;			///< Count of keys in keymap
	int m_KeyStackCount;		///< Count of input events in stack
	int m_KeyStackPointer;		///< Pointer to current input event
	int m_KeyStack[56];			///< Up to 56 events in input queue
};


#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
