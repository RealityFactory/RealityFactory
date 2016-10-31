/************************************************************************************//**
 * @file CMenuEventHandler.cpp
 * @brief Menu event handler
 *
 * This file contains the implementation of the MenuEventHandler class.
 * @author Daniel Queteschiner
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "RabidFramework.h"
#include "CFileManager.h"
#include "CMenu.h"
#include "CMenuEventHandler.h"
#include "CGUIManager.h"
#include "CGameStateManager.h"
#include "CMenuState.h"
#include "CCDAudio.h"
#include "CCharacter.h"
#include "CLanguageManager.h"


CMenuEventHandler::CMenuEventHandler(CRFMenu& menu) : m_Menu(&menu), m_Action(0)
{
}


CMenuEventHandler::~CMenuEventHandler()
{
}


void CMenuEventHandler::SubscribeEvents()
{
	// main
	CCD->GUIManager()->SubscribeEvent(	"Menu/Main/NewGame",
										CEGUI::PushButton::EventClicked,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleMainNewGame, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/Main/Multiplayer",
										CEGUI::PushButton::EventClicked,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleMainMultiplayer, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/Main/LoadGame",
										CEGUI::PushButton::EventClicked,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleMainLoadGame, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/Main/SaveGame",
										CEGUI::PushButton::EventClicked,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleMainSaveGame, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/Main/Options",
										CEGUI::PushButton::EventClicked,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleMainOptions, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/Main/Mods",
										CEGUI::PushButton::EventClicked,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleMainMods, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/Main/Credits",
										CEGUI::PushButton::EventClicked,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleMainCredits, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/Main/Quit",
										CEGUI::PushButton::EventClicked,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleMainQuit, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/Main/Confirm/Quit/OK",
										CEGUI::PushButton::EventClicked,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleMainConfirmQuitOK, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/Main/Confirm/Quit/Cancel",
										CEGUI::PushButton::EventClicked,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleMainConfirmQuitCancel, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/Main/Confirm/Quit",
										CEGUI::FrameWindow::EventCloseClicked,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleMainConfirmQuitCancel, this));

	// multiplayer events
	CCD->GUIManager()->SubscribeEvent(	"Menu/Multiplayer/HostGame",
										CEGUI::PushButton::EventClicked,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleMultiplayerHostGame, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/Multiplayer/JoinGame",
										CEGUI::PushButton::EventClicked,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleMultiplayerJoinGame, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/Multiplayer/Quit",
										CEGUI::PushButton::EventClicked,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleQuit, this));

	// join game events
	CCD->GUIManager()->SubscribeEvent(	"Menu/JoinGame/JoinGame",
										CEGUI::PushButton::EventClicked,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleJoinGameJoinGame, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/JoinGame/Quit",
										CEGUI::PushButton::EventClicked,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleQuit, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/JoinGame/IP",
										CEGUI::Window::EventTextChanged,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleCharacterKey, this));

	// load game events
	CCD->GUIManager()->SubscribeEvent(	"Menu/LoadGame/SavegameList",
										CEGUI::Listbox::EventSelectionChanged,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleLoadGameSavegameListSelection, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/LoadGame/LoadGame",
										CEGUI::PushButton::EventClicked,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleLoadGameLoadGame, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/LoadGame/Quit",
										CEGUI::PushButton::EventClicked,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleQuit, this));

	// save game events
	CCD->GUIManager()->SubscribeEvent(	"Menu/SaveGame/SavegameList",
										CEGUI::Listbox::EventSelectionChanged,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleSaveGameSavegameListSelection, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/SaveGame/SaveGame",
										CEGUI::PushButton::EventClicked,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleSaveGameSaveGame, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/SaveGame/Quit",
										CEGUI::PushButton::EventClicked,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleQuit, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/SaveGame/DeleteSavegame",
										CEGUI::PushButton::EventClicked,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleSaveGameDeleteSavegame, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/SaveGame/Confirm/DeleteSavegame/OK",
										CEGUI::PushButton::EventClicked,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleSaveGameConfirmDeleteSavegameOK, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/SaveGame/Confirm/DeleteSavegame/Cancel",
										CEGUI::PushButton::EventClicked,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleSaveGameConfirmDeleteSavegameCancel, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/SaveGame/Confirm/DeleteSavegame",
										CEGUI::FrameWindow::EventCloseClicked,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleSaveGameConfirmDeleteSavegameCancel, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/SaveGame/Confirm/OverwriteSavegame/OK",
										CEGUI::PushButton::EventClicked,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleSaveGameConfirmOverwriteSavegameOK, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/SaveGame/Confirm/OverwriteSavegame/Cancel",
										CEGUI::PushButton::EventClicked,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleSaveGameConfirmOverwriteSavegameCancel, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/SaveGame/Confirm/OverwriteSavegame",
										CEGUI::FrameWindow::EventCloseClicked,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleSaveGameConfirmOverwriteSavegameCancel, this));

	// options events
	CCD->GUIManager()->SubscribeEvent(	"Menu/Options/Audio",
										CEGUI::PushButton::EventClicked,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleOptionsAudio, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/Options/Video",
										CEGUI::PushButton::EventClicked,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleOptionsVideo, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/Options/Controls",
										CEGUI::PushButton::EventClicked,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleOptionsControls, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/Options/Language",
										CEGUI::PushButton::EventClicked,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleOptionsLanguage, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/Options/Debug",
										CEGUI::PushButton::EventClicked,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleOptionsDebug, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/Options/Quit",
										CEGUI::PushButton::EventClicked,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleQuit, this));

	// audio events
	CCD->GUIManager()->SubscribeEvent(	"Menu/Audio/Quit",
										CEGUI::PushButton::EventClicked,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleQuit, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/Audio/SoundVolume",
										CEGUI::Slider::EventValueChanged,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleSoundVolume, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/Audio/MusicVolume",
										CEGUI::Slider::EventValueChanged,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleMusicVolume, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/Audio/CDMusic",
										CEGUI::Checkbox::EventCheckStateChanged,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleCDMusic, this));

	// video events
	CCD->GUIManager()->SubscribeEvent(	"Menu/Video/Quit",
										CEGUI::PushButton::EventClicked,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleQuit, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/Video/Gamma",
										CEGUI::Slider::EventValueChanged,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleGamma, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/Video/Detail",
										CEGUI::Slider::EventValueChanged,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleDetail, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/Video/Shadows",
										CEGUI::Checkbox::EventCheckStateChanged,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleShadows, this));

	// controls events
	CCD->GUIManager()->SubscribeEvent(	"Menu/Controls/Quit",
										CEGUI::PushButton::EventClicked,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleQuit, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/Controls/Advanced",
										CEGUI::PushButton::EventClicked,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleControlsAdvanced, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/Controls/Default",
										CEGUI::PushButton::EventClicked,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleControlsDefault, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/Controls/ActionKeyMap",
										CEGUI::MultiColumnList::EventSelectionChanged,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleControlsActionSetup, this));

	// advanced events
	CCD->GUIManager()->SubscribeEvent(	"Menu/Advanced/Quit",
										CEGUI::PushButton::EventClicked,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleQuit, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/Advanced/Crosshairs",
										CEGUI::Checkbox::EventCheckStateChanged,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleCrosshairs, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/Advanced/ReverseMouse",
										CEGUI::Checkbox::EventCheckStateChanged,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleReverseMouse, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/Advanced/MouseFilter",
										CEGUI::Checkbox::EventCheckStateChanged,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleMouseFilter, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/Advanced/MouseSensitivity",
										CEGUI::Slider::EventValueChanged,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleMouseSensitivity, this));

	// language events
	CCD->GUIManager()->SubscribeEvent(	"Menu/Language/Quit",
										CEGUI::PushButton::EventClicked,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleQuit, this));
	CLanguage *lang = CCD->LanguageManager()->GetFirstLanguage();
	while(lang)
	{
		std::string windowname = "Menu/Language/" + lang->GetName();
		CCD->GUIManager()->SubscribeEvent(	windowname,
											CEGUI::RadioButton::EventSelectStateChanged,
											CEGUI::Event::Subscriber(&CMenuEventHandler::handleLanguageRadio, this));
		lang = CCD->LanguageManager()->GetNextLanguage();
	}

	// debug events
	CCD->GUIManager()->SubscribeEvent(	"Menu/Debug/Quit",
										CEGUI::PushButton::EventClicked,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleQuit, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/Debug/DebugInfo",
										CEGUI::Checkbox::EventCheckStateChanged,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleDebugInfo, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/Debug/FrameRate",
										CEGUI::Checkbox::EventCheckStateChanged,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleFrameRate, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/Debug/NoClipping",
										CEGUI::Checkbox::EventCheckStateChanged,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleNoClipping, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/Debug/PlayerBoundingBox",
										CEGUI::Checkbox::EventCheckStateChanged,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handlePlayerBoundingBox, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/Debug/EntityBoundingBox",
										CEGUI::Checkbox::EventCheckStateChanged,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleEntityBoundingBox, this));
	// mods events
	CCD->GUIManager()->SubscribeEvent(	"Menu/Mods/Quit",
										CEGUI::PushButton::EventClicked,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleQuit, this));

	// credits events
	CCD->GUIManager()->SubscribeEvent(	"Menu/Credits/Quit",
										CEGUI::PushButton::EventClicked,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleQuit, this));

	// character events
	CCD->GUIManager()->SubscribeEvent(	"Menu/Character/Quit",
										CEGUI::PushButton::EventClicked,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleQuit, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/Character/Accept",
										CEGUI::PushButton::EventClicked,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleCharacterAccept, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/Character/Next",
										CEGUI::PushButton::EventClicked,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleCharacterNext, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/Character/Previous",
										CEGUI::PushButton::EventClicked,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleCharacterPrevious, this));

	// playername events
	CCD->GUIManager()->SubscribeEvent(	"Menu/PlayerName/Quit",
										CEGUI::PushButton::EventClicked,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleQuit, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/PlayerName/Accept",
										CEGUI::PushButton::EventClicked,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleNameAccept, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/PlayerName/Reset",
										CEGUI::PushButton::EventClicked,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleNameReset, this));

	// difficulty events
	CCD->GUIManager()->SubscribeEvent(	"Menu/Difficulty/Easy",
										CEGUI::PushButton::EventClicked,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleDifficultyEasy, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/Difficulty/Normal",
										CEGUI::PushButton::EventClicked,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleDifficultyNormal, this));
	CCD->GUIManager()->SubscribeEvent(	"Menu/Difficulty/Hard",
										CEGUI::PushButton::EventClicked,
										CEGUI::Event::Subscriber(&CMenuEventHandler::handleDifficultyHard, this));
}


void CMenuEventHandler::RemoveAllEvents()
{
	// main
	CCD->GUIManager()->RemoveAllEvents("Menu/Main/NewGame");
	CCD->GUIManager()->RemoveAllEvents("Menu/Main/Multiplayer");
	CCD->GUIManager()->RemoveAllEvents("Menu/Main/LoadGame");
	CCD->GUIManager()->RemoveAllEvents("Menu/Main/SaveGame");
	CCD->GUIManager()->RemoveAllEvents("Menu/Main/Options");
	CCD->GUIManager()->RemoveAllEvents("Menu/Main/Mods");
	CCD->GUIManager()->RemoveAllEvents("Menu/Main/Credits");
	CCD->GUIManager()->RemoveAllEvents("Menu/Main/Quit");
	CCD->GUIManager()->RemoveAllEvents("Menu/Main/Confirm/Quit/OK");
	CCD->GUIManager()->RemoveAllEvents("Menu/Main/Confirm/Quit/Cancel");
	CCD->GUIManager()->RemoveAllEvents("Menu/Main/Confirm/Quit");

	// multiplayer events
	CCD->GUIManager()->RemoveAllEvents("Menu/Multiplayer/HostGame");
	CCD->GUIManager()->RemoveAllEvents("Menu/Multiplayer/JoinGame");
	CCD->GUIManager()->RemoveAllEvents("Menu/Multiplayer/Quit");

	// join game events
	CCD->GUIManager()->RemoveAllEvents("Menu/JoinGame/JoinGame");
	CCD->GUIManager()->RemoveAllEvents("Menu/JoinGame/Quit");
	CCD->GUIManager()->RemoveAllEvents("Menu/JoinGame/IP");

	// load game events
	CCD->GUIManager()->RemoveAllEvents("Menu/LoadGame/SavegameList");
	CCD->GUIManager()->RemoveAllEvents("Menu/LoadGame/LoadGame");
	CCD->GUIManager()->RemoveAllEvents("Menu/LoadGame/Quit");

	// save game events
	CCD->GUIManager()->RemoveAllEvents("Menu/SaveGame/SavegameList");
	CCD->GUIManager()->RemoveAllEvents("Menu/SaveGame/SaveGame");
	CCD->GUIManager()->RemoveAllEvents("Menu/SaveGame/Quit");
	CCD->GUIManager()->RemoveAllEvents("Menu/SaveGame/DeleteSavegame");
	CCD->GUIManager()->RemoveAllEvents("Menu/SaveGame/Confirm/DeleteSavegame/OK");
	CCD->GUIManager()->RemoveAllEvents("Menu/SaveGame/Confirm/DeleteSavegame/Cancel");
	CCD->GUIManager()->RemoveAllEvents("Menu/SaveGame/Confirm/DeleteSavegame");
	CCD->GUIManager()->RemoveAllEvents("Menu/SaveGame/Confirm/OverwriteSavegame/OK");
	CCD->GUIManager()->RemoveAllEvents("Menu/SaveGame/Confirm/OverwriteSavegame/Cancel");
	CCD->GUIManager()->RemoveAllEvents("Menu/SaveGame/Confirm/OverwriteSavegame");

	// options events
	CCD->GUIManager()->RemoveAllEvents("Menu/Options/Audio");
	CCD->GUIManager()->RemoveAllEvents("Menu/Options/Video");
	CCD->GUIManager()->RemoveAllEvents("Menu/Options/Controls");
	CCD->GUIManager()->RemoveAllEvents("Menu/Options/Language");
	CCD->GUIManager()->RemoveAllEvents("Menu/Options/Debug");
	CCD->GUIManager()->RemoveAllEvents("Menu/Options/Quit");

	// audio events
	CCD->GUIManager()->RemoveAllEvents("Menu/Audio/Quit");
	CCD->GUIManager()->RemoveAllEvents("Menu/Audio/SoundVolume");
	CCD->GUIManager()->RemoveAllEvents("Menu/Audio/MusicVolume");
	CCD->GUIManager()->RemoveAllEvents("Menu/Audio/CDMusic");

	// video events
	CCD->GUIManager()->RemoveAllEvents("Menu/Video/Quit");
	CCD->GUIManager()->RemoveAllEvents("Menu/Video/Gamma");
	CCD->GUIManager()->RemoveAllEvents("Menu/Video/Detail");
	CCD->GUIManager()->RemoveAllEvents("Menu/Video/Shadows");

	// controls events
	CCD->GUIManager()->RemoveAllEvents("Menu/Controls/Quit");
	CCD->GUIManager()->RemoveAllEvents("Menu/Controls/Advanced");
	CCD->GUIManager()->RemoveAllEvents("Menu/Controls/Default");
	CCD->GUIManager()->RemoveAllEvents("Menu/Controls/ActionKeyMap");

	// advanced events
	CCD->GUIManager()->RemoveAllEvents("Menu/Advanced/Quit");
	CCD->GUIManager()->RemoveAllEvents("Menu/Advanced/Crosshairs");
	CCD->GUIManager()->RemoveAllEvents("Menu/Advanced/ReverseMouse");
	CCD->GUIManager()->RemoveAllEvents("Menu/Advanced/MouseFilter");
	CCD->GUIManager()->RemoveAllEvents("Menu/Advanced/MouseSensitivity");

	// language events
	CCD->GUIManager()->RemoveAllEvents("Menu/Language/Quit");
	CLanguage *lang = CCD->LanguageManager()->GetFirstLanguage();
	while(lang)
	{
		std::string windowname = "Menu/Language/" + lang->GetName();
		CCD->GUIManager()->RemoveAllEvents(windowname);
		lang = CCD->LanguageManager()->GetNextLanguage();
	}

	// debug events
	CCD->GUIManager()->RemoveAllEvents("Menu/Debug/Quit");
	CCD->GUIManager()->RemoveAllEvents("Menu/Debug/DebugInfo");
	CCD->GUIManager()->RemoveAllEvents("Menu/Debug/FrameRate");
	CCD->GUIManager()->RemoveAllEvents("Menu/Debug/NoClipping");
	CCD->GUIManager()->RemoveAllEvents("Menu/Debug/PlayerBoundingBox");
	CCD->GUIManager()->RemoveAllEvents("Menu/Debug/EntityBoundingBox");

	// mods events
	CCD->GUIManager()->RemoveAllEvents("Menu/Mods/Quit");

	// credits events
	CCD->GUIManager()->RemoveAllEvents("Menu/Credits/Quit");

	// character events
	CCD->GUIManager()->RemoveAllEvents("Menu/Character/Quit");
	CCD->GUIManager()->RemoveAllEvents("Menu/Character/Accept");
	CCD->GUIManager()->RemoveAllEvents("Menu/Character/Next");
	CCD->GUIManager()->RemoveAllEvents("Menu/Character/Previous");

	// playername events
	CCD->GUIManager()->RemoveAllEvents("Menu/PlayerName/Quit");
	CCD->GUIManager()->RemoveAllEvents("Menu/PlayerName/Accept");
	CCD->GUIManager()->RemoveAllEvents("Menu/PlayerName/Reset");

	// difficulty events
	CCD->GUIManager()->RemoveAllEvents("Menu/Difficulty/Easy");
	CCD->GUIManager()->RemoveAllEvents("Menu/Difficulty/Normal");
	CCD->GUIManager()->RemoveAllEvents("Menu/Difficulty/Hard");
}

// ------------------------------------------

bool CMenuEventHandler::handleMainNewGame(const CEGUI::EventArgs& /*e*/)
{
	m_Menu->PlayMouseClickSound();
	CCD->ShutDownNetWork();
	CCD->SetMultiPlayer(false, false);

	if(CCD->GetCharacterSelection())
	{
		m_Menu->SetActiveSection("character");
		return true;
	}
	if(CCD->GetNameSelection())
	{
		m_Menu->SetPlayerNameWindowText();
		m_Menu->SetActiveSection("playername");
		return true;
	}
	if(CCD->GetDifficultSelection())
	{
		m_Menu->SetActiveSection("difficulty");
		return true;
	}

	m_Menu->DoGame(false);
	return true;
}


bool CMenuEventHandler::handleMainMultiplayer(const CEGUI::EventArgs& /*e*/)
{
	m_Menu->PlayMouseClickSound();
	m_Menu->SetActiveSection("multiplayer");
	return true;
}


bool CMenuEventHandler::handleMainLoadGame(const CEGUI::EventArgs& /*e*/)
{
	m_Menu->PlayMouseClickSound();
	m_Menu->SetActiveSection("loadgame");
	return true;
}


bool CMenuEventHandler::handleMainSaveGame(const CEGUI::EventArgs& /*e*/)
{
	m_Menu->PlayMouseClickSound();
	m_Menu->SetActiveSection("savegame");
	return true;
}


bool CMenuEventHandler::handleMainOptions(const CEGUI::EventArgs& /*e*/)
{
	m_Menu->PlayMouseClickSound();
	m_Menu->SetActiveSection("options");
	return true;
}


bool CMenuEventHandler::handleMainMods(const CEGUI::EventArgs& /*e*/)
{
	m_Menu->PlayMouseClickSound();
	m_Menu->SetActiveSection("mods");
	return true;
}


bool CMenuEventHandler::handleMainCredits(const CEGUI::EventArgs& /*e*/)
{
	m_Menu->PlayMouseClickSound();
	m_Menu->SetActiveSection("credits");
	return true;
}


bool CMenuEventHandler::handleMainQuit(const CEGUI::EventArgs& /*e*/)
{
	m_Menu->PlayMouseClickSound();
	if(CCD->GUIManager()->IsWindowPresent("Menu/Main/Confirm/Quit"))
	{
		CCD->GUIManager()->GetWindow("Menu/Main/Confirm/Quit")->setModalState(true);
		CCD->GUIManager()->GetWindow("Menu/Main/Confirm/Quit")->moveToFront();
		CCD->GUIManager()->ShowWindow("Menu/Main/Confirm/Quit");
		return true;
	}
	CGameStateManager::GetSingletonPtr()->RequestShutdown();
	return true;
}


bool CMenuEventHandler::handleMainConfirmQuitOK(const CEGUI::EventArgs& /*e*/)
{
	m_Menu->PlayMouseClickSound();
	CGameStateManager::GetSingletonPtr()->RequestShutdown();
	return true;
}


bool CMenuEventHandler::handleMainConfirmQuitCancel(const CEGUI::EventArgs& /*e*/)
{
	m_Menu->PlayMouseClickSound();
	CCD->GUIManager()->HideWindow("Menu/Main/Confirm/Quit");
	CCD->GUIManager()->GetWindow("Menu/Main/Confirm/Quit")->setModalState(false);
	return true;
}


bool CMenuEventHandler::handleMultiplayerHostGame(const CEGUI::EventArgs& /*e*/)
{
	m_Menu->PlayMouseClickSound();
	CCD->ShutDownNetWork();
	CCD->SetMultiPlayer(true, true);

	if(CCD->GetCharacterSelection())
	{
		m_Menu->SetActiveSection("character");
		return true;
	}
	if(CCD->GetNameSelection())
	{
		m_Menu->SetPlayerNameWindowText();
		m_Menu->SetActiveSection("playername");
		return true;
	}
	if(CCD->GetDifficultSelection())
	{
		m_Menu->SetActiveSection("difficulty");
		return true;
	}

	m_Menu->DoGame(false);
	return true;
}


bool CMenuEventHandler::handleMultiplayerJoinGame(const CEGUI::EventArgs& /*e*/)
{
	m_Menu->PlayMouseClickSound();
	m_Menu->SetActiveSection("joingame");
	return true;
}

// ------------------------------------------------------

bool CMenuEventHandler::handleJoinGameJoinGame(const CEGUI::EventArgs& /*e*/)
{
	m_Menu->PlayMouseClickSound();
	CCD->ShutDownNetWork();
	CCD->SetMultiPlayer(true, false);
	strcpy(m_Menu->m_ServerIP, CCD->GUIManager()->GetWindowText("Menu/JoinGame/IP"));

	if(CCD->GetCharacterSelection())
	{
		m_Menu->SetActiveSection("character");
		return true;
	}
	if(CCD->GetNameSelection())
	{
		m_Menu->SetPlayerNameWindowText();
		m_Menu->SetActiveSection("playername");
		return true;
	}
	// TODO: get difficulty from host?
	/*if(CCD->GetDifficultSelection())
	{
		m_Menu->SetActiveSection("difficulty");
		return true;
	}*/

	m_Menu->DoGame(false);
	return true;
}

// ------------------------------------------------------

bool CMenuEventHandler::handleLoadGameLoadGame(const CEGUI::EventArgs& /*e*/)
{
	m_Menu->PlayMouseClickSound();
	m_Menu->LoadGame();
	return true;
}


bool CMenuEventHandler::handleLoadGameSavegameListSelection(const CEGUI::EventArgs& /*e*/)
{
	CEGUI::Listbox *lbox = NULL;
	lbox = static_cast<CEGUI::Listbox*>(CCD->GUIManager()->GetWindow("Menu/LoadGame/SavegameList"));

	if(lbox->getFirstSelectedItem())
	{
		m_Menu->PlayMouseClickSound();

		if(CCD->GUIManager()->IsWindowPresent("Menu/LoadGame/SavegameImage"))
		{
			char filename[64];
			sprintf(filename, "savegame%d.jpg", lbox->getFirstSelectedItem()->getID());

			if(CFileManager::GetSingletonPtr()->FileExist(kSavegameFile, filename))
			{
				std::string value = "set:";
				value += filename;
				value += " image:full_image";
				CCD->GUIManager()->GetWindow("Menu/LoadGame/SavegameImage")->setProperty("Image", value);
			}
		}
	}
	else
	{
		if(CCD->GUIManager()->IsWindowPresent("Menu/LoadGame/SavegameImage"))
		{
			CCD->GUIManager()->GetWindow("Menu/LoadGame/SavegameImage")->setProperty("Image", m_Menu->m_LoadGameDefaultImage);
		}
	}

	return true;
}


// ------------------------------------------------------

bool CMenuEventHandler::handleSaveGameSaveGame(const CEGUI::EventArgs& /*e*/)
{
	m_Menu->PlayMouseClickSound();
	m_Menu->SaveGame();
	return true;
}


bool CMenuEventHandler::handleSaveGameSavegameListSelection(const CEGUI::EventArgs& /*e*/)
{
	CEGUI::Listbox *lbox = NULL;
	lbox = static_cast<CEGUI::Listbox*>(CCD->GUIManager()->GetWindow("Menu/SaveGame/SavegameList"));

	if(lbox->getFirstSelectedItem())
	{
		m_Menu->PlayMouseClickSound();

		if(CCD->GUIManager()->IsWindowPresent("Menu/SaveGame/SavegameImage"))
		{
			char filename[64];
			sprintf(filename, "savegame%d.jpg", lbox->getFirstSelectedItem()->getID());

			if(CFileManager::GetSingletonPtr()->FileExist(kSavegameFile, filename))
			{
				std::string value = "set:";
				value += filename;
				value += " image:full_image";
				CCD->GUIManager()->GetWindow("Menu/SaveGame/SavegameImage")->setProperty("Image", value);
			}
			else
			{
				CCD->GUIManager()->GetWindow("Menu/SaveGame/SavegameImage")->setProperty("Image", m_Menu->m_SaveGameDefaultImage);
			}
		}
	}
	else
	{
		if(CCD->GUIManager()->IsWindowPresent("Menu/SaveGame/SavegameImage"))
		{
			CCD->GUIManager()->GetWindow("Menu/SaveGame/SavegameImage")->setProperty("Image", m_Menu->m_SaveGameDefaultImage);
		}
	}

	return true;
}


bool CMenuEventHandler::handleSaveGameDeleteSavegame(const CEGUI::EventArgs& /*e*/)
{
	m_Menu->PlayMouseClickSound();
	m_Menu->DeleteSavegame();
	return true;
}


bool CMenuEventHandler::handleSaveGameConfirmDeleteSavegameOK(const CEGUI::EventArgs& /*e*/)
{
	m_Menu->PlayMouseClickSound();
	m_Menu->DeleteSavegame(false);
	CCD->GUIManager()->HideWindow("Menu/SaveGame/Confirm/DeleteSavegame");
	CCD->GUIManager()->GetWindow("Menu/SaveGame/Confirm/DeleteSavegame")->setModalState(false);
	return true;
}


bool CMenuEventHandler::handleSaveGameConfirmDeleteSavegameCancel(const CEGUI::EventArgs& /*e*/)
{
	m_Menu->PlayMouseClickSound();
	CCD->GUIManager()->HideWindow("Menu/SaveGame/Confirm/DeleteSavegame");
	CCD->GUIManager()->GetWindow("Menu/SaveGame/Confirm/DeleteSavegame")->setModalState(false);
	return true;
}


bool CMenuEventHandler::handleSaveGameConfirmOverwriteSavegameOK(const CEGUI::EventArgs& /*e*/)
{
	m_Menu->PlayMouseClickSound();
	m_Menu->SaveGame(false);
	CCD->GUIManager()->HideWindow("Menu/SaveGame/Confirm/OverwriteSavegame");
	CCD->GUIManager()->GetWindow("Menu/SaveGame/Confirm/OverwriteSavegame")->setModalState(false);
	return true;
}


bool CMenuEventHandler::handleSaveGameConfirmOverwriteSavegameCancel(const CEGUI::EventArgs& /*e*/)
{
	m_Menu->PlayMouseClickSound();
	CCD->GUIManager()->HideWindow("Menu/SaveGame/Confirm/OverwriteSavegame");
	CCD->GUIManager()->GetWindow("Menu/SaveGame/Confirm/OverwriteSavegame")->setModalState(false);
	return true;
}

// ------------------------------------------------------

bool CMenuEventHandler::handleOptionsAudio(const CEGUI::EventArgs& /*e*/)
{
	m_Menu->PlayMouseClickSound();
	m_Menu->SetActiveSection("audio");
	return true;
}


bool CMenuEventHandler::handleOptionsVideo(const CEGUI::EventArgs& /*e*/)
{
	m_Menu->PlayMouseClickSound();
	m_Menu->SetActiveSection("video");
	return true;
}


bool CMenuEventHandler::handleOptionsControls(const CEGUI::EventArgs& /*e*/)
{
	m_Menu->PlayMouseClickSound();
	m_Menu->SetActiveSection("controls");
	return true;
}


bool CMenuEventHandler::handleOptionsLanguage(const CEGUI::EventArgs& /*e*/)
{
	m_Menu->PlayMouseClickSound();
	m_Menu->SetActiveSection("language");
	return true;
}


bool CMenuEventHandler::handleOptionsDebug(const CEGUI::EventArgs& /*e*/)
{
	m_Menu->PlayMouseClickSound();
	m_Menu->SetActiveSection("debug");
	return true;
}

// -------------------------------------

bool CMenuEventHandler::handleSoundVolume(const CEGUI::EventArgs& e)
{
	float val = static_cast<CEGUI::Slider*>(static_cast<const CEGUI::WindowEventArgs&>(e).window)->getCurrentValue();
	m_Menu->SetMusicVol(val);
	return true;
}


bool CMenuEventHandler::handleMusicVolume(const CEGUI::EventArgs& e)
{
	float val = static_cast<CEGUI::Slider*>(static_cast<const CEGUI::WindowEventArgs&>(e).window)->getCurrentValue();
	m_Menu->SetmMusicVol(val);
	return true;
}


bool CMenuEventHandler::handleCDMusic(const CEGUI::EventArgs& e)
{
	m_Menu->PlayMouseClickSound();

	if(static_cast<CEGUI::Checkbox*>(static_cast<const CEGUI::WindowEventArgs&>(e).window)->isSelected())
	{
		CCD->CDPlayer()->SetCdOn(true);
	}
	else
	{
		CCD->CDPlayer()->SetCdOn(false);
		CCD->CDPlayer()->Stop();
	}

	return true;
}

// -------------------------------------

bool CMenuEventHandler::handleGamma(const CEGUI::EventArgs& e)
{
	m_Menu->PlaySlideClickSound();

	float val = static_cast<CEGUI::Slider*>(static_cast<const CEGUI::WindowEventArgs&>(e).window)->getCurrentValue();
	m_Menu->SetGamma(val);
	return true;
}


bool CMenuEventHandler::handleDetail(const CEGUI::EventArgs& e)
{
	m_Menu->PlaySlideClickSound();

	float val = static_cast<CEGUI::Slider*>(static_cast<const CEGUI::WindowEventArgs&>(e).window)->getCurrentValue();
	m_Menu->SetDetail(val);
	return true;
}


bool CMenuEventHandler::handleShadows(const CEGUI::EventArgs& e)
{
	m_Menu->PlayMouseClickSound();

	if(static_cast<CEGUI::Checkbox*>(static_cast<const CEGUI::WindowEventArgs&>(e).window)->isSelected())
	{
		m_Menu->SetStencilShadows(true);
	}
	else
	{
		m_Menu->SetStencilShadows(false);
	}

	return true;
}

// -------------------------------------

bool CMenuEventHandler::handleControlsAdvanced(const CEGUI::EventArgs& /*e*/)
{
	m_Menu->PlayMouseClickSound();
	m_Menu->SetActiveSection("advanced");
	return true;
}


bool CMenuEventHandler::handleControlsDefault(const CEGUI::EventArgs& /*e*/)
{
	m_Menu->PlayMouseClickSound();
	m_Menu->ResetActionMapList();

	return true;
}


bool CMenuEventHandler::handleControlsActionSetup(const CEGUI::EventArgs& /*e*/)
{
	m_Menu->PlayMouseClickSound();

	CEGUI::MultiColumnList *listbox = static_cast<CEGUI::MultiColumnList*>(CCD->GUIManager()->GetWindow("Menu/Controls/ActionKeyMap"));
	CEGUI::ListboxItem *item = listbox->getFirstSelectedItem();
	if(!item) return false;

	m_Action = item->getID();

	CCD->GUIManager()->GetSheet()->disable();
	CCD->Input()->AddKeyListener(this, "ActionSetup");
	CCD->Input()->AddMouseListener(this, "ActionSetup");
	CCD->Input()->RemoveKeyListener("GameStateManager");
	CCD->Input()->RemoveMouseListener("GameStateManager");

	return true;
}

// -------------------------------------

bool CMenuEventHandler::handleCrosshairs(const CEGUI::EventArgs& e)
{
	m_Menu->PlayMouseClickSound();

	if(static_cast<CEGUI::Checkbox*>(static_cast<const CEGUI::WindowEventArgs&>(e).window)->isSelected())
	{
		m_Menu->m_bCrosshairs = true;
	}
	else
	{
		m_Menu->m_bCrosshairs = false;
	}

	return true;
}


bool CMenuEventHandler::handleReverseMouse(const CEGUI::EventArgs& e)
{
	m_Menu->PlayMouseClickSound();

	if(static_cast<CEGUI::Checkbox*>(static_cast<const CEGUI::WindowEventArgs&>(e).window)->isSelected())
	{
		m_Menu->SetMouseReverse(true);
	}
	else
	{
		m_Menu->SetMouseReverse(false);
	}

	return true;
}


bool CMenuEventHandler::handleMouseFilter(const CEGUI::EventArgs& e)
{
	m_Menu->PlayMouseClickSound();

	if(static_cast<CEGUI::Checkbox*>(static_cast<const CEGUI::WindowEventArgs&>(e).window)->isSelected())
	{
		m_Menu->SetMouseFilter(true);
	}
	else
	{
		m_Menu->SetMouseFilter(false);
	}

	return true;
}


bool CMenuEventHandler::handleMouseSensitivity(const CEGUI::EventArgs& e)
{
	float val = static_cast<CEGUI::Slider*>(static_cast<const CEGUI::WindowEventArgs&>(e).window)->getCurrentValue();
	m_Menu->SetMouseSensitivity(val);

	return true;
}

// -------------------------------------

bool CMenuEventHandler::handleLanguageRadio(const CEGUI::EventArgs& e)
{
	m_Menu->PlayMouseClickSound();

	CEGUI::RadioButton *radio = static_cast<CEGUI::RadioButton*>(static_cast<const CEGUI::WindowEventArgs&>(e).window);
	if(radio->isSelected())
	{
		std::string name = radio->getName().c_str();
		name.erase(0, name.rfind('/') + 1);
		CMenuState::GetSingletonPtr()->RequestLanguageChange(name);
	}

	return true;
}

// -------------------------------------

bool CMenuEventHandler::handleDebugInfo(const CEGUI::EventArgs& e)
{
	m_Menu->PlayMouseClickSound();

	if(static_cast<CEGUI::Checkbox*>(static_cast<const CEGUI::WindowEventArgs&>(e).window)->isSelected())
	{
		m_Menu->m_bDebugInfo = true;
	}
	else
	{
		m_Menu->m_bDebugInfo = false;
	}

	return true;
}


bool CMenuEventHandler::handleFrameRate(const CEGUI::EventArgs& e)
{
	m_Menu->PlayMouseClickSound();

	if(static_cast<CEGUI::Checkbox*>(static_cast<const CEGUI::WindowEventArgs&>(e).window)->isSelected())
	{
		m_Menu->m_bFrameRate = true;
	}
	else
	{
		m_Menu->m_bFrameRate = false;
	}

	return true;
}


bool CMenuEventHandler::handleNoClipping(const CEGUI::EventArgs& e)
{
	m_Menu->PlayMouseClickSound();

	if(static_cast<CEGUI::Checkbox*>(static_cast<const CEGUI::WindowEventArgs&>(e).window)->isSelected())
	{
		m_Menu->SetNoClip(true);
	}
	else
	{
		m_Menu->SetNoClip(false);
	}

	return true;
}


bool CMenuEventHandler::handlePlayerBoundingBox(const CEGUI::EventArgs& e)
{
	m_Menu->PlayMouseClickSound();

	if(static_cast<CEGUI::Checkbox*>(static_cast<const CEGUI::WindowEventArgs&>(e).window)->isSelected())
	{
		m_Menu->SetBoundBox(true);
	}
	else
	{
		m_Menu->SetBoundBox(false);
	}

	return true;
}


bool CMenuEventHandler::handleEntityBoundingBox(const CEGUI::EventArgs& e)
{
	m_Menu->PlayMouseClickSound();

	if(static_cast<CEGUI::Checkbox*>(static_cast<const CEGUI::WindowEventArgs&>(e).window)->isSelected())
	{
		m_Menu->SetSEBoundBox(true);
	}
	else
	{
		m_Menu->SetSEBoundBox(false);
	}

	return true;
}

// -------------------------------------

bool CMenuEventHandler::handleCharacterAccept(const CEGUI::EventArgs& /*e*/)
{
	m_Menu->PlayMouseClickSound();

	if(!CCD->GetUseLevelDialog() && !CCD->GetCmdLine())
	{
		std::string startLevel = m_Menu->GetSelectedCharacter()->GetStartLevel();

		if(!startLevel.empty())
			m_Menu->SetLevelName(startLevel.c_str());
	}

	if(CCD->GetNameSelection())
	{
		m_Menu->SetPlayerNameWindowText();
		m_Menu->SetActiveSection("playername");
		return true;
	}

	if(CCD->GetDifficultSelection())
	{
		m_Menu->SetActiveSection("difficulty");
		return true;
	}

	m_Menu->DoGame(false);
	return true;
}


bool CMenuEventHandler::handleCharacterNext(const CEGUI::EventArgs& /*e*/)
{
	m_Menu->PlayMouseClickSound();
	m_Menu->ChangeSelectedCharacter(true);
	return true;
}


bool CMenuEventHandler::handleCharacterPrevious(const CEGUI::EventArgs& /*e*/)
{
	m_Menu->PlayMouseClickSound();
	m_Menu->ChangeSelectedCharacter(false);
	return true;
}

// -------------------------------------

bool CMenuEventHandler::handleNameAccept(const CEGUI::EventArgs& /*e*/)
{
	m_Menu->PlayMouseClickSound();
	m_Menu->SetSelectedName(CCD->GUIManager()->GetWindowText("Menu/PlayerName/PlayerName"));

	if(CCD->GetDifficultSelection())
	{
		m_Menu->SetActiveSection("difficulty");
		return true;
	}

	m_Menu->DoGame(false);
	return true;
}


bool CMenuEventHandler::handleNameReset(const CEGUI::EventArgs& /*e*/)
{
	m_Menu->PlayMouseClickSound();
	m_Menu->SetPlayerNameWindowText();
	return true;
}

// -------------------------------------

bool CMenuEventHandler::handleDifficultyEasy(const CEGUI::EventArgs& /*e*/)
{
	m_Menu->PlayMouseClickSound();
	CCD->SetDifficultLevel(1);
	m_Menu->DoGame(false);
	return true;
}


bool CMenuEventHandler::handleDifficultyNormal(const CEGUI::EventArgs& /*e*/)
{
	m_Menu->PlayMouseClickSound();
	CCD->SetDifficultLevel(2);
	m_Menu->DoGame(false);
	return true;
}

bool CMenuEventHandler::handleDifficultyHard(const CEGUI::EventArgs& /*e*/)
{
	m_Menu->PlayMouseClickSound();
	CCD->SetDifficultLevel(3);
	m_Menu->DoGame(false);
	return true;
}

// -------------------------------------

bool CMenuEventHandler::handleQuit(const CEGUI::EventArgs& /*e*/)
{
	m_Menu->PlayMouseClickSound();
	m_Menu->QuitSection();
	return true;
}


bool CMenuEventHandler::handleCharacterKey(const CEGUI::EventArgs& /*e*/)
{
	m_Menu->PlayKeyClickSound();
	return true;
}

// -------------------------------------

bool CMenuEventHandler::keyPressed(const OIS::KeyEvent &e)
{
	m_Menu->PlayKeyClickSound();

	if(e.key == OIS::KC_ESCAPE)
	{
		CEGUI::MultiColumnList *listbox = static_cast<CEGUI::MultiColumnList*>(CCD->GUIManager()->GetWindow("Menu/Controls/ActionKeyMap"));
		listbox->clearAllSelections();
	}
	else
	{
		CCD->Input()->ClearKeyAction(m_Action);
		CCD->Input()->ClearMouseAction(m_Action);
		CCD->Input()->SetKeyAction(e.key, m_Action);

		m_Menu->RefreshActionMapList();
	}

	CCD->Input()->AddKeyListener(CGameStateManager::GetSingletonPtr(), "GameStateManager");
	CCD->Input()->AddMouseListener(CGameStateManager::GetSingletonPtr(), "GameStateManager");
	CCD->Input()->RemoveKeyListener("ActionSetup");
	CCD->Input()->RemoveMouseListener("ActionSetup");
	CCD->GUIManager()->GetSheet()->enable();

	return false;
}


bool CMenuEventHandler::keyReleased(const OIS::KeyEvent &/*e*/)
{
	return false;
}


bool CMenuEventHandler::mouseMoved(const OIS::MouseEvent &e)
{
	CCD->GUIManager()->MousePosition(static_cast<float>(e.state.X.abs), static_cast<float>(e.state.Y.abs));
	return true;
}


bool CMenuEventHandler::mousePressed(const OIS::MouseEvent &/*e*/, OIS::MouseButtonID id)
{
	m_Menu->PlayMouseClickSound();

	CCD->Input()->ClearKeyAction(m_Action);
	CCD->Input()->ClearMouseAction(m_Action);
	CCD->Input()->SetMouseAction(id, m_Action);

	m_Menu->RefreshActionMapList();

	CCD->Input()->AddKeyListener(CGameStateManager::GetSingletonPtr(), "GameStateManager");
	CCD->Input()->AddMouseListener(CGameStateManager::GetSingletonPtr(), "GameStateManager");
	CCD->Input()->RemoveKeyListener("ActionSetup");
	CCD->Input()->RemoveMouseListener("ActionSetup");
	CCD->GUIManager()->GetSheet()->enable();

	return false;
}


bool CMenuEventHandler::mouseReleased(const OIS::MouseEvent &/*e*/, OIS::MouseButtonID /*id*/)
{
	return false;
}
