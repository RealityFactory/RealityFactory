/************************************************************************************//**
 * @file CGameState.cpp
 * @brief CGameState class
 *
 * This file contains the implementation of the Game State base class.
 * @author Daniel Queteschiner
 * @date December 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "CGameStateManager.h"
#include "CGameState.h"


void CGameState::ChangeState(CGameState *state)
{
	CGameStateManager::GetSingletonPtr()->ChangeState(state);
}

void CGameState::PushState(CGameState *state)
{
	CGameStateManager::GetSingletonPtr()->PushState(state);
}

void CGameState::PopState()
{
	CGameStateManager::GetSingletonPtr()->PopState();
}

CEGUI::DefaultWindow* CGameState::GetDefaultWindow()
{
	return m_DefaultWindow;
}
