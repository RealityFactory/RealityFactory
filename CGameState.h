/************************************************************************************//**
 * @file CGameState.h
 * @brief CGameState class
 *
 * This file contains the declaration of the Game State base class.
 * @author Daniel Queteschiner
 * @date December 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_GAME_STATE_H
#define __RGF_GAME_STATE_H

#include <CEGUI.h>
#include <OISMouse.h>
#include <OISKeyboard.h>

class CGameState
{
public:
	virtual ~CGameState() { }

	virtual void Enter()  = 0;
	virtual void Exit()   = 0;

	virtual void Pause()  = 0;
	virtual void Resume() = 0;
	virtual void Update(float timeElapsed) = 0;
	virtual void Render() = 0;

	virtual void keyPressed(const OIS::KeyEvent &e)  = 0;
	virtual void keyReleased(const OIS::KeyEvent &e) = 0;

	virtual void mouseMoved(const OIS::MouseEvent &e) = 0;
	virtual void mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)  = 0;
	virtual void mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id) = 0;

	void ChangeState(CGameState *state);
	void PushState(CGameState *state);
	void PopState();
	CEGUI::DefaultWindow* GetDefaultWindow();

protected:
	CGameState() : m_DefaultWindow(NULL) { }

protected:
	CEGUI::DefaultWindow*	m_DefaultWindow;

private:
	CGameState(const CGameState&) { }
	CGameState& operator = (const CGameState&);
};

#endif
