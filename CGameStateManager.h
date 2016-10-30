/************************************************************************************//**
 * @file CGameStateManager.h
 * @brief CGameStateManager class
 *
 * This file contains the declaration of the Game State Manager class.
 * @author Daniel Queteschiner
 * @date December 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_GAMESTATE_MANAGER_H_
#define __RGF_GAMESTATE_MANAGER_H_

#include <stack>
#include <OISMouse.h>
#include <OISKeyboard.h>

class CGameState;

class CGameStateManager : public OIS::KeyListener, public OIS::MouseListener
{
public:
	~CGameStateManager();

	void RunGame();
	void RequestShutdown();

	void ChangeState(CGameState *gameState);
	void PushState(CGameState *gameState);
	void PopState();
	CGameState* GetState() { return m_States.top(); }

	static CGameStateManager* GetSingletonPtr();

private:
	CGameStateManager();
	CGameStateManager(const CGameStateManager&) { }
	CGameStateManager& operator = (const CGameStateManager&);

	bool keyPressed(const OIS::KeyEvent &e);
	bool keyReleased(const OIS::KeyEvent &e);

	bool mouseMoved(const OIS::MouseEvent &e);
	bool mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id);

	CGameState*		m_MenuState;
	CGameState*		m_PlayState;
	CGameState*		m_DialogState;

	bool			m_bShutdown;

	std::stack<CGameState*> m_States;
	static CGameStateManager *m_GameStateManager;
};
#endif
