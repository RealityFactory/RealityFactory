/************************************************************************************//**
 * @file CDialogState.cpp
 * @brief CDialogState class
 *
 * This file contains the implementation of the Dialog State class.
 * @author Daniel Queteschiner
 * @date December 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "RabidFramework.h"
#include "CDialogState.h"
#include "CMenuState.h"

#include "CGUIManager.h"
#include "CHeadsUpDisplay.h"
#include "CConversationManager.h"
#include "CAudioManager.h"
#include "CMIDIAudio.h"
#include "CCDAudio.h"
// start multiplayer
#include "HawkNL\\nl.h"
// end multiplayer

#include "CLevel.h"
#include "CPawnManager.h" // :TODO: get rid of this include (fill screen area)


CDialogState* CDialogState::m_DialogState = NULL;

CDialogState::CDialogState() :
	m_UpdateLevel(true),
	m_RenderLevel(true),
	m_OldView(0)
{
	// create a default window and set it as root
	m_DefaultWindow = (CEGUI::DefaultWindow*)CCD->GUIManager()->CreateDefaultWindow("Dialog/Root");
}


void CDialogState::Enter()
{
	CCD->Log()->Debug("Enter DialogState");

	CCD->GUIManager()->SetSheet(m_DefaultWindow);
	if(!CCD->GUIManager()->IsCursorVisible())
	{
		CCD->Input()->CenterMouse();
		CCD->GUIManager()->MousePosition(0.5f * CCD->Engine()->Width(), 0.5f * CCD->Engine()->Height());
		CCD->GUIManager()->ShowCursor();
	}

	m_OldView = CCD->Player()->GetViewPoint();
	CCD->CameraManager()->SaveToS();
	CCD->Player()->SwitchCamera(FREEFLOATING);

	if(!CCD->Player()->FirstPersonView())
		CCD->Player()->LookMode(false);

	CCD->Player()->Moving(MOVEIDLE);
	CCD->Player()->SetSlideWalk(MOVEIDLE);
	CCD->Player()->SetJumping(GE_FALSE);

	sxConversationManager::GetSingletonPtr()->DequeueAll();
}


void CDialogState::Exit()
{
	CCD->Log()->Debug("Exit DialogState");

	int i = m_DefaultWindow->getChildCount() - 1;
	for(; i>=0; --i)
	{
		m_DefaultWindow->getChildAtIdx(i)->hide();
	}

	sxConversationManager::GetSingletonPtr()->StopConversing();
	// FIRST restore point of view, THEN restore saved values
	CCD->Player()->SwitchCamera(m_OldView);
	CCD->CameraManager()->RestoreFromS();
}


void CDialogState::Pause()
{
	CCD->Log()->Debug("Pause DialogState");
}


void CDialogState::Resume()
{
	CCD->Log()->Debug("Resume DialogState");

	CCD->GUIManager()->SetSheet(m_DefaultWindow);
	CCD->GUIManager()->ShowCursor();
}


void CDialogState::Update(float timeElapsed)
{
	CCD->Level()->Update(m_UpdateLevel ? timeElapsed : 0.f);

	sxConversationManager::GetSingletonPtr()->Update(timeElapsed);
	CCD->CameraManager()->Update(timeElapsed);

	CCD->Level()->ActorManager()->Update(m_UpdateLevel ? timeElapsed : 0.f);
	if(m_UpdateLevel)
	{
		CCD->Level()->HUD()->Tick(timeElapsed);
	}


	CCD->AudioManager()->Tick(timeElapsed);		// Audio updates
	CCD->GUIManager()->Update(timeElapsed);
	// start multiplayer
	CCD->NetPlayerManager()->Tick(timeElapsed);
	// end multiplayer

	CCD->CheckMediaPlayers();					// Poll media players
}


void CDialogState::Render()
{
	CCD->Engine()->BeginFrame();				// Start up rendering for this frame

	if(m_RenderLevel)
	{
		CCD->Level()->Render();					// Render the RGF components
		CCD->Engine()->RenderWorld();			// Render the world

		// :TODO: get rid of this
		// Fills a rectangle on the screen with color / alpha (can be used for color/alpha fading)
		for(int i=0; i<MAXFILLAREA; ++i)
			CCD->Level()->Pawns()->FillScreenArea(i);

		CCD->Level()->HUD()->Render();

		// Displays a text on the screen that tracks position with an Entity
		for(int ii=0; ii<MAXTEXT; ++ii)
			CCD->Level()->Pawns()->ShowText(ii);
	}

	CCD->GUIManager()->Render();

	// Everything rendered, now end the frame.
	CCD->Engine()->EndFrame();
}


void CDialogState::ProcessPressedAction(int action)
{
	switch(action)
	{
#ifdef _DEBUG
	case RGF_K_EXIT:
		{
			this->PopState(); // Exit state
			break;
		}
#endif
	case RGF_K_SCRNSHOT:
		{
			CCD->MenuManager()->ScreenShot();
			break;
		}
	}
}


void CDialogState::ProcessReleasedAction(int /*action*/)
{
}


void CDialogState::keyPressed(const OIS::KeyEvent &e)
{
	CCD->GUIManager()->KeyDown(e.key);
	if(e.key == OIS::KC_SPACE)
	{
		sxConversationManager::GetSingletonPtr()->Skip();
	}
	ProcessPressedAction(CCD->Input()->GetKeyAction(e.key));
}


void CDialogState::keyReleased(const OIS::KeyEvent &e)
{
	CCD->GUIManager()->KeyUp(e.key);
}


void CDialogState::mouseMoved(const OIS::MouseEvent &e)
{
	POINT tempPos;
	CCD->Input()->GetMousePos(&tempPos.x, &tempPos.y);
	CCD->GUIManager()->MousePosition(static_cast<float>(tempPos.x), static_cast<float>(tempPos.y));

	// update only if position has change, otherwise hovering of thumbs (slider, scrollbar) won't work
	if(e.state.Z.rel != 0)
		CCD->GUIManager()->MouseWheelChange(static_cast<float>(e.state.Z.rel)/120.f);
}


void CDialogState::mousePressed(const OIS::MouseEvent &/*e*/, OIS::MouseButtonID id)
{
	CCD->GUIManager()->MouseButtonDown(id);
	ProcessPressedAction(CCD->Input()->GetMouseAction(id));
}


void CDialogState::mouseReleased(const OIS::MouseEvent &/*e*/, OIS::MouseButtonID id)
{
	CCD->GUIManager()->MouseButtonUp(id);
}


CDialogState* CDialogState::GetSingletonPtr()
{
	if(!m_DialogState)
	{
		m_DialogState = new CDialogState();
	}

	return m_DialogState;
}

