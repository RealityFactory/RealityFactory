/************************************************************************************//**
 * @file CDialogState.h
 * @brief CDialogState class
 *
 * This file contains the declaration of the Dialog State class.
 * @author Daniel Queteschiner
 * @date December 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_DIALOG_STATE_H
#define __RGF_DIALOG_STATE_H

#include "CGameState.h"

class CDialogState : public CGameState
{
public:
	~CDialogState() { }

	void Enter();
	void Exit();

	void Pause();
	void Resume();
	void Update(float timeElapsed);
	void Render();

	void keyPressed(const OIS::KeyEvent &e);
	void keyReleased(const OIS::KeyEvent &e);

	void mouseMoved(const OIS::MouseEvent &e);
	void mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id);
	void mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id);

	void UpdateLevel(bool update) { m_UpdateLevel = update; }
	void RenderLevel(bool render) { m_RenderLevel = render; }

	static CDialogState* GetSingletonPtr();

private:
	CDialogState();
	// The DialogState cannot be copied, so the copy constructor is private
	CDialogState(const CDialogState&) { }
	// The DialogState cannot be copied, so the assignment operator is private
	CDialogState & operator = (const CDialogState&);

	void ProcessPressedAction(int action);
	void ProcessReleasedAction(int action);
	bool m_UpdateLevel;
	bool m_RenderLevel;
	int m_OldView;

	static CDialogState *m_DialogState;
};
#endif
