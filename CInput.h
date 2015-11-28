/*
	CInput.h:		Keyboard and mouse input handler

	(c) 2001 Ralph Deane

	This file contains the class declaration for the CInput
class that encapsulates all mouse and keyboard input for
RGF-based games.
*/

#ifndef __RGF_CINPUT_H__
#define __RGF_CINPUT_H__

class CInput
{
public:
  CInput();
  ~CInput();
  void Default();
  // Scan keyboard and mouse, return first input item
	int GetFirstInput();
	// Get next input item from input queue
  int GetNextInput();
	// Save current keymaps to a user file
  int SaveKeymap(char *szFilename);
	// Load saved keymaps from a user file
  int LoadKeymap(char *szFilename);
  int GetCodes(int action);
  int GetKeyboardInput();
  bool GetKeyCheck(int keytemp);
// changed RF064
  int GetKeyboardInputNoWait();
  void ClearCodes(int action);
// end change RF064
  void SetKeyAction(int key, int action)
  { m_RGFKeys[key] = action; }

private:
	// Scan the keyboard for input from appropriate keys
  void ScanKeyboardInput();
	int m_WindowKeys[100];		// Up to 100 Windows keys in map
	int m_RGFKeys[100];		// The 100 RGF keys they map to
  int m_nMappedKeys;								// Count of keys in keymap
  int m_KeyStackCount;							// Count of input events in stack
	int m_KeyStackPointer;						// Pointer to current input event
	int m_KeyStack[56];								// Up to 56 events in input queue
};

#endif
