/************************************************************************************//**
 * @file CPlayState.h
 * @brief CPlayState class
 *
 * This file contains the declaration of the Play State class.
 * @author Daniel Queteschiner
 * @date December 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_PLAY_STATE_H
#define __RGF_PLAY_STATE_H

#include "CGameState.h"

class CPlayState : public CGameState
{
public:
	~CPlayState() { }

	void Enter();
	void Exit();

	void Pause();
	void Resume();
	void Update(float timeElapsed);
	void Render();

	void ShowCursor(bool show) { m_ShowCursor = show; }

	void keyPressed(const OIS::KeyEvent &e);
	void keyReleased(const OIS::KeyEvent &e);

	void mouseMoved(const OIS::MouseEvent &e);
	void mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id);
	void mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id);

	static CPlayState* GetSingletonPtr();

private:
	CPlayState();
	// The PlayState cannot be copied, so the copy constructor is private
	CPlayState(const CPlayState&) { }
	// The PlayState cannot be copied, so the assignment operator is private
	CPlayState & operator = (const CPlayState&);

	bool	m_ShowCursor;
	bool	m_CameraMode;
	int		m_KeyRotation;
	int		m_Zooming;
	int		m_Attacking;

	void ProcessPressedAction(int action);
	void ProcessReleasedAction(int action);

	static CPlayState *m_PlayState;
};
#endif
