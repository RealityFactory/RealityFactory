/************************************************************************************//**
 * @file CMenuState.cpp
 * @brief CMenuState class
 *
 * This file contains the implementation of the Menu State class.
 * @author Daniel Queteschiner
 * @date December 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "RabidFramework.h"
#include "CMIDIAudio.h"
#include "CGUIManager.h"
#include "CLanguageManager.h"
#include "CMenuState.h"
#include "CPlayState.h"

CMenuState* CMenuState::m_MenuState = NULL;


CMenuState::CMenuState()
	: m_ChangeLanguage(false)
{
	// create a default window and set it as root
	m_DefaultWindow = (CEGUI::DefaultWindow*)CCD->GUIManager()->CreateDefaultWindow("Menu/Root");
}


void CMenuState::Enter()
{
	CCD->Log()->Debug("Enter MenuState");

	CCD->GUIManager()->SetSheet(m_DefaultWindow);
	if(!CCD->GUIManager()->IsCursorVisible())
	{
		CCD->Input()->CenterMouse();
		CCD->GUIManager()->ShowCursor();
	}

	geRect cameraRect;

	cameraRect.Left	= 0;
	cameraRect.Right	= CCD->Engine()->Width() - 1;
	cameraRect.Top		= 0;
	cameraRect.Bottom	= CCD->Engine()->Height() - 1;

	m_Camera = geCamera_Create(2.0f, &cameraRect);

	CCD->MenuManager()->Enter();
}


void CMenuState::Exit()
{
	CCD->Log()->Debug("Exit MenuState");

	geCamera_Destroy(&m_Camera);

	CCD->MenuManager()->Exit();
}


void CMenuState::Pause()
{
	CCD->Log()->Debug("Pause MenuState");

	CCD->MenuManager()->StopMusic();
}


void CMenuState::Resume()
{
	CCD->Log()->Debug("Resume MenuState");

	CCD->GUIManager()->SetSheet(m_DefaultWindow);
	if(!CCD->GUIManager()->IsCursorVisible())
	{
		CCD->Input()->CenterMouse();
		CCD->GUIManager()->ShowCursor();
	}

	if(CCD->MenuManager()->GetMusicType() != -1)
	{
		if(CCD->MenuManager()->GetMusicType() == 1)
			CCD->MenuManager()->MIDIPlayer()->Play(CCD->MenuManager()->GetMusic(), true);
		else
			CCD->MenuManager()->GetStreamingAudio()->Play(true);
	}

	// :TODO: remove explicit name string
	if(CCD->MenuManager()->GetInGame())
		CCD->GUIManager()->ShowWindow("Menu/Main/Return");
	else
		CCD->GUIManager()->HideWindow("Menu/Main/Return");

	CCD->MenuManager()->SetmMusicVol(CCD->MenuManager()->GetmVolLevel());

	CCD->MenuManager()->FadeSet(-1, CCD->MenuManager()->GetTimeFade());
}


void CMenuState::Update(float timeElapsed)
{
	if(m_ChangeLanguage)
	{
		CCD->LanguageManager()->SetActiveLanguage(m_Language);
		m_ChangeLanguage = false;
		CCD->Log()->Debug("Active Language: " + m_Language);
	}

	CCD->GUIManager()->Update(timeElapsed);
}


void CMenuState::Render()
{
	geEngine_BeginFrame(CCD->Engine()->Engine(), m_Camera, GE_TRUE);

	CCD->GUIManager()->Render();

	if(CCD->MenuManager()->IsFading())
		CCD->MenuManager()->DoFade();

	geEngine_EndFrame(CCD->Engine()->Engine());
}


void CMenuState::keyPressed(const OIS::KeyEvent &e)
{
	if(CCD->MenuManager()->IsFading())
		return;

	if(CCD->Input()->GetKeyAction(e.key) == RGF_K_SCRNSHOT)
	{
		CCD->MenuManager()->ScreenShot();
		return;
	}

	CCD->GUIManager()->KeyDown(e.key);

	if(CCD->Input()->GetKeyAction(e.key) == RGF_K_EXIT)
		if(CCD->MenuManager()->GetInGame())
			this->PushState(CPlayState::GetSingletonPtr());
}


void CMenuState::keyReleased(const OIS::KeyEvent &e)
{
	CCD->GUIManager()->KeyUp(e.key);
}


void CMenuState::mouseMoved(const OIS::MouseEvent &e)
{
	POINT tempPos;
	CCD->Input()->GetMousePos(&tempPos.x, &tempPos.y);
	CCD->GUIManager()->MousePosition(static_cast<float>(tempPos.x), static_cast<float>(tempPos.y));

	if(CCD->MenuManager()->IsFading())
		return;

	// update only if position has change, otherwise hovering of thumbs (slider, scrollbar) won't work
	if(e.state.Z.rel != 0)
		CCD->GUIManager()->MouseWheelChange(static_cast<float>(e.state.Z.rel)/120.f);
}


void CMenuState::mousePressed(const OIS::MouseEvent &/*e*/, OIS::MouseButtonID id)
{
	if(CCD->MenuManager()->IsFading())
		return;

	CCD->GUIManager()->MouseButtonDown(id);
}


void CMenuState::mouseReleased(const OIS::MouseEvent &/*e*/, OIS::MouseButtonID id)
{
	CCD->GUIManager()->MouseButtonUp(id);
}


void CMenuState::RequestLanguageChange(const std::string& language)
{
	m_Language = language;
	m_ChangeLanguage = true;
}


CMenuState* CMenuState::GetSingletonPtr()
{
	if(!m_MenuState)
	{
		m_MenuState = new CMenuState();
	}

	return m_MenuState;
}
