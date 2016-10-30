/************************************************************************************//**
 * @file CMenuState.h
 * @brief CMenuState class
 *
 * This file contains the declaration of the Menu State class.
 * @author Daniel Queteschiner
 * @date December 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_MENU_STATE_H
#define __RGF_MENU_STATE_H

#include "CGameState.h"

class CMenuState : public CGameState
{
public:
	~CMenuState() { }

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

	void RequestLanguageChange(const std::string& language);

	static CMenuState* GetSingletonPtr();

private:
	CMenuState();
	// The MenuState cannot be copied, so the copy constructor is private
	CMenuState(const CMenuState&) { }
	// The MenuState cannot be copied, so the assignment operator is private
	CMenuState& operator = (const CMenuState&);

	geCamera*				m_Camera;
	bool					m_ChangeLanguage;
	std::string				m_Language;

private:
	static CMenuState*		m_MenuState;
};

#endif
