/************************************************************************************//**
 * @file CMenuEventHandler.h
 * @brief Menu event handler
 *
 *
 * @author Daniel Queteschiner
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All Rights Reserved.
 ****************************************************************************************/

#ifndef __RGF_MENUEVENTHANDLER_H_
#define __RGF_MENUEVENTHANDLER_H_

#include <OISMouse.h>
#include <OISKeyboard.h>

class CRFMenu;

class CMenuEventHandler : public OIS::KeyListener, public OIS::MouseListener
{
public:
	explicit CMenuEventHandler(CRFMenu& menu);
	~CMenuEventHandler();

	void SubscribeEvents();
	void RemoveAllEvents();

private:
	bool handleMainNewGame(const CEGUI::EventArgs& e);
	bool handleMainMultiplayer(const CEGUI::EventArgs& e);
	bool handleMainLoadGame(const CEGUI::EventArgs& e);
	bool handleMainSaveGame(const CEGUI::EventArgs& e);
	bool handleMainOptions(const CEGUI::EventArgs& e);
	bool handleMainMods(const CEGUI::EventArgs& e);
	bool handleMainCredits(const CEGUI::EventArgs& e);
	bool handleMainQuit(const CEGUI::EventArgs& e);
	bool handleMainConfirmQuitOK(const CEGUI::EventArgs& e);
	bool handleMainConfirmQuitCancel(const CEGUI::EventArgs& e);
	bool handleMultiplayerHostGame(const CEGUI::EventArgs& e);
	bool handleMultiplayerJoinGame(const CEGUI::EventArgs& e);
	bool handleJoinGameJoinGame(const CEGUI::EventArgs& e);
	bool handleLoadGameLoadGame(const CEGUI::EventArgs& e);
	bool handleLoadGameSavegameListSelection(const CEGUI::EventArgs& e);
	bool handleSaveGameSaveGame(const CEGUI::EventArgs& e);
	bool handleSaveGameSavegameListSelection(const CEGUI::EventArgs& e);
	bool handleSaveGameDeleteSavegame(const CEGUI::EventArgs& e);
	bool handleSaveGameConfirmDeleteSavegameOK(const CEGUI::EventArgs& e);
	bool handleSaveGameConfirmDeleteSavegameCancel(const CEGUI::EventArgs& e);
	bool handleSaveGameConfirmOverwriteSavegameOK(const CEGUI::EventArgs& e);
	bool handleSaveGameConfirmOverwriteSavegameCancel(const CEGUI::EventArgs& e);
	bool handleOptionsAudio(const CEGUI::EventArgs& e);
	bool handleOptionsVideo(const CEGUI::EventArgs& e);
	bool handleOptionsControls(const CEGUI::EventArgs& e);
	bool handleOptionsLanguage(const CEGUI::EventArgs& e);
	bool handleOptionsDebug(const CEGUI::EventArgs& e);
	bool handleSoundVolume(const CEGUI::EventArgs& e);
	bool handleMusicVolume(const CEGUI::EventArgs& e);
	bool handleCDMusic(const CEGUI::EventArgs& e);
	bool handleGamma(const CEGUI::EventArgs& e);
	bool handleDetail(const CEGUI::EventArgs& e);
	bool handleShadows(const CEGUI::EventArgs& e);
	bool handleControlsAdvanced(const CEGUI::EventArgs& e);
	bool handleControlsDefault(const CEGUI::EventArgs& e);
	bool handleControlsActionSetup(const CEGUI::EventArgs& e);
	bool handleCrosshairs(const CEGUI::EventArgs& e);
	bool handleReverseMouse(const CEGUI::EventArgs& e);
	bool handleMouseFilter(const CEGUI::EventArgs& e);
	bool handleMouseSensitivity(const CEGUI::EventArgs& e);
	bool handleLanguageRadio(const CEGUI::EventArgs& e);
	bool handleDebugInfo(const CEGUI::EventArgs& e);
	bool handleFrameRate(const CEGUI::EventArgs& e);
	bool handleNoClipping(const CEGUI::EventArgs& e);
	bool handlePlayerBoundingBox(const CEGUI::EventArgs& e);
	bool handleEntityBoundingBox(const CEGUI::EventArgs& e);
	bool handleQuit(const CEGUI::EventArgs& e);
	bool handleCharacterKey(const CEGUI::EventArgs& e);

	bool handleCharacterAccept(const CEGUI::EventArgs& e);
	bool handleCharacterNext(const CEGUI::EventArgs& e);
	bool handleCharacterPrevious(const CEGUI::EventArgs& e);

	bool handleNameAccept(const CEGUI::EventArgs& e);
	bool handleNameReset(const CEGUI::EventArgs& e);

	bool handleDifficultyEasy(const CEGUI::EventArgs& e);
	bool handleDifficultyNormal(const CEGUI::EventArgs& e);
	bool handleDifficultyHard(const CEGUI::EventArgs& e);

	bool keyPressed(const OIS::KeyEvent &e);
	bool keyReleased(const OIS::KeyEvent &e);

	bool mouseMoved(const OIS::MouseEvent &e);
	bool mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id);

private:
	CRFMenu *m_Menu;
	unsigned int m_Action;
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
