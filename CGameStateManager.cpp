/************************************************************************************//**
 * @file CGameStateManager.cpp
 * @brief CGameStateManager class
 *
 * This file contains the implementation of the Game State Manager class.
 * @author Daniel Queteschiner
 * @date December 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "RabidFramework.h"
#include "CGameState.h"
#include "CPlayState.h"
#include "CDialogState.h"
#include "CMenuState.h"
#include "CGameStateManager.h"


CGameStateManager* CGameStateManager::m_GameStateManager = NULL;


CGameStateManager::CGameStateManager()
{
	m_DialogState = CDialogState::GetSingletonPtr();
	m_PlayState  = CPlayState::GetSingletonPtr();
	m_MenuState = CMenuState::GetSingletonPtr();
	m_bShutdown = false;
}


CGameStateManager::~CGameStateManager()
{
	// Clean up all the states
	while(!m_States.empty())
	{
		m_States.top()->Exit();
		m_States.pop();
	}

	delete m_DialogState;
	delete m_PlayState;
	delete m_MenuState;
}


void CGameStateManager::RunGame()
{
	MSG msg;
	float timeElapsed;

	CCD->Log()->Debug("Entering Main Game Loop");

	while(!m_bShutdown)
	{
		// Deal with platform specific issues
		while(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			GetMessage(&msg, NULL, 0, 0);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if(!CCD->GetHasFocus())
		{
			CCD->ResetClock();
			continue;
		}

		timeElapsed = CCD->GetTimePassed_F();

		if(timeElapsed <= 0.0f)
			continue;

		CCD->Input()->Capture();

		// Update current state
		m_States.top()->Update(timeElapsed);

		// Render next frame
		m_States.top()->Render();
	}

	CCD->MenuManager()->SetInGame(0);
}


void CGameStateManager::ChangeState(CGameState *gameState)
{
	// Cleanup the current state
	if(!m_States.empty())
	{
		m_States.top()->Exit();
		m_States.pop();
	}

	// Store and init the new state
	m_States.push(gameState);
	m_States.top()->Enter();
}


void CGameStateManager::PushState(CGameState *gameState)
{
	// Pause current state
	if(!m_States.empty())
	{
		m_States.top()->Pause();
	}

	// Store and init the new state
	m_States.push(gameState);
	m_States.top()->Enter();
}


void CGameStateManager::PopState()
{
	// Cleanup the current state
	if(!m_States.empty())
	{
		m_States.top()->Exit();
		m_States.pop();
	}

	// Resume previous state
	if(!m_States.empty())
	{
		m_States.top()->Resume();
	}
}


void CGameStateManager::RequestShutdown()
{
	m_bShutdown = true;
}


bool CGameStateManager::keyPressed(const OIS::KeyEvent &e)
{
	if(m_States.empty())
		return true;

	// Call keyPressed of current state
	m_States.top()->keyPressed(e);

	return true;
}


bool CGameStateManager::keyReleased(const OIS::KeyEvent &e)
{
	if(m_States.empty())
		return true;

	// Call keyReleased of current state
	m_States.top()->keyReleased(e);

	return true;
}


bool CGameStateManager::mouseMoved(const OIS::MouseEvent &e)
{
	if(m_States.empty())
		return true;

	// Call mouseMoved of current state
	m_States.top()->mouseMoved(e);

	return true;
}


bool CGameStateManager::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	if(m_States.empty())
		return true;

	// Call mousePressed of current state
	m_States.top()->mousePressed(e, id);

	return true;
}


bool CGameStateManager::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	if(m_States.empty())
		return true;

	// Call mouseReleased of current state
	m_States.top()->mouseReleased(e, id);

	return true;
}


CGameStateManager* CGameStateManager::GetSingletonPtr()
{
	if(!m_GameStateManager)
	{
		m_GameStateManager = new CGameStateManager();
	}

	return m_GameStateManager;
}
