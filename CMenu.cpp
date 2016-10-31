/************************************************************************************//**
 * @file CMenu.cpp
 * @brief Menu class implementation
 ****************************************************************************************/

#include "RabidFramework.h"
#include "Ram.h"
#include "CMenuEventHandler.h"
#include "CFileManager.h"
#include "IniFile.h"
#include "Mixer.h"
#include "CHeadsUpDisplay.h"
#include "CInventory.h"
#include "CCharacter.h"
#include "CGameStateManager.h"
#include "CPlayState.h"
#include "CMenuState.h"
#include "CGUIManager.h"
#include "CLanguageManager.h"
#include "CCDAudio.h"
#include "CMIDIAudio.h"

#include "CLevel.h"
#include "Qx\\qxTerrainMgr.h"

#include "CEGUI\\CEGUIListBoxTextItemEx.h"

#include "CActMaterial.h"
#include "CAttribute.h"
#include "CAutoDoors.h"
#include "CChange.h"
#include "CChangeLevel.h"
#include "CCountDown.h"
#include "CDamage.h"
#include "CElectric.h"
#include "CLogic.h"
#include "CMessage.h"
#include "CMorphingFields.h"
#include "CMovingPlatforms.h"
#include "CSoundtrackToggle.h"
#include "CStaticEntity.h"
#include "CStaticMesh.h"
#include "CTeleporter.h"
#include "CTriggers.h"

extern geBitmap *TPool_Bitmap(const char *DefaultBmp, const char *DefaultAlpha,
							  const char *BName, const char *AName);


static void CopyImageFile(const char *srcPath, const char *destPath);

#define GAMMAMIN	0.2f
#define GAMMAMAX	2.0f

#define MOUSEMIN	0.0005f
#define MOUSEMAX	0.005f

char errortext[256];

// ------------------------------------------------------

int CRFMenu::GetNumberFromFilename(const std::string& filename)
{
	// used to get the number of the savegame filename
	std::string number = filename;
	number.erase(number.find_last_of("."));
	number.erase(0, number.find_last_not_of("1234567890") + 1);
	return atoi(number.c_str());
}


void CRFMenu::FindSavegames()
{
	HANDLE searchHandle;
	WIN32_FIND_DATA findData;
	char label[128];
	std::string filename = CFileManager::GetSingletonPtr()->GetDirectory(kSavegameFile);
	filename += "/savegame*.sav";
	searchHandle = FindFirstFile(filename.c_str(), &findData);

	if(searchHandle != INVALID_HANDLE_VALUE)
	{
		do
		{
			FILE *save = CFileManager::GetSingletonPtr()->OpenRFFile(kSavegameFile, findData.cFileName, "rb");
			if(save)
			{
				fread(label, sizeof(char), 128, save);
				fclose(save);
			}

			label[127] = 0;
			m_Savegames[findData.cFileName] = label;

			int maxSave = GetNumberFromFilename(findData.cFileName);
			if(maxSave > m_SavegameCounter)
				m_SavegameCounter = maxSave;

		} while(FindNextFile(searchHandle, &findData));

		FindClose(searchHandle);
		++m_SavegameCounter;
	}
}


void CRFMenu::SetActiveSection(const std::string& section, bool skipFade)
{
	if(m_Sections.find(section) != m_Sections.end())
	{
		m_SectionStack.push(m_Sections[section]);
		if(!skipFade)
			FadeSet(1, m_TimeFade);
	}
	else
	{
		CCD->Log()->Debug("Menu section " + section + " does not exist!");
	}
}


void CRFMenu::QuitSection()
{
	m_SectionStack.pop();
	FadeSet(1, m_TimeFade);
}


void CRFMenu::PlayMouseClickSound()
{
	geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), m_MouseClick, 0.99f, 0.0f, 1.0f, false);
}


void CRFMenu::PlayKeyClickSound()
{
	geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), m_KeyClick, 0.99f, 0.5f, 1.0f, false);
}


void CRFMenu::PlaySlideClickSound()
{
	geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), m_SlideClick, 0.75f, 0.0f, 1.0f, false);
}

/* ------------------------------------------------------------------------------------ */
// Constructor
/* ------------------------------------------------------------------------------------ */
CRFMenu::CRFMenu(const char *szStartLevel) :
	m_FCrosshairs(NULL),
	m_ScreenshotCount(0),
	m_bScrnWait(false),
	m_InGame(0),
	m_bMusicFade(false),
	m_bCrosshairs(false),
	m_MouseClick(NULL),
	m_KeyClick(NULL),
	m_SlideClick(NULL),
	m_Bottom(NULL),
	m_MusicType(-1),
	m_Streams(NULL),
	m_MaxCharacters(0),
	m_SelectedCharacter(0),
	m_MIDIPlayer(NULL),
	m_mVolLevel(0.5f),
	m_TimeFade(0.0f),
	//m_Crosshairs(NULL),
	m_SavegameCounter(0),
	m_NewSavegameLabel("<New Savegame>"),
	m_KeyNameUnassigned("<Unassigned>")
{
// start multiplayer
	CCD->Log()->Notice("Initializing Network...");

	if(CCD->GetNetwork())
	{
		if(nlSelectNetwork(NL_IP))
		{
			NLsocket serversock = nlOpen(0, NL_RELIABLE_PACKETS);

			if(serversock != NL_INVALID)
			{
				NLaddress addr;
				nlGetLocalAddr(serversock, &addr);
				nlAddrToString(&addr, m_IP);
				nlClose(serversock);
			}
		}
	}
// end multiplayer

	strcpy(m_Loading, "menu\\loading.bmp");
	FindSavegames();

	memset(m_Characters, 0, sizeof(CCharacter*)*NUM_SELECT);

	if(CCD->GetCharacterSelection())
	{
		CCD->Log()->Notice("Loading Character.ini...");
		LoadCharacterConfiguration(szStartLevel);
	}

	CCD->Log()->Notice("Loading menu configuration file...");

	LoadConfiguration(CCD->LanguageManager()->GetActiveLanguage()->GetMenuConfigFilename());
	LoadText(CCD->LanguageManager()->GetActiveLanguage()->GetMenuTextFilename());

	CCD->Log()->Debug("Creating Audio Mixer...");
	CMixer mixer(CCD->Engine()->WindowHandle(), MIXERLINE_COMPONENTTYPE_DST_SPEAKERS,
					NO_SOURCE, MIXERCONTROL_CONTROLTYPE_VOLUME);

	if(mixer.IsOk())
		m_WinVol = mixer.GetControlValue();
}


/* ------------------------------------------------------------------------------------ */
// LoadCharacterConfiguration
/* ------------------------------------------------------------------------------------ */
void CRFMenu::LoadCharacterConfiguration(const std::string& startLevel)
{
	CIniFile iniFile("character.ini");

	if(!iniFile.ReadFile())
	{
		CCD->Log()->Critical("File %s - Line %d: Failed to open character.ini file!",
								__FILE__, __LINE__);
		delete CCD;
		exit(-100);
	}

	m_MaxCharacters = m_SelectedCharacter = 0;
	std::string keyName = iniFile.FindFirstKey();

	while(!keyName.empty())
	{
		m_Characters[m_MaxCharacters] = new CCharacter(keyName, startLevel);

		if(m_Characters[m_MaxCharacters]->LoadConfiguration(iniFile) != RGF_SUCCESS)
		{
			CCD->Log()->Error("File %s - Line %d: Failed to load configuration for character '%s'!",
										__FILE__, __LINE__, keyName.c_str());
			SAFE_DELETE(m_Characters[m_MaxCharacters]);
		}
		else
		{
			CCD->Log()->Debug("Successfully loaded configuration for character '" + keyName + "'");
			++m_MaxCharacters;
		}

		keyName = iniFile.FindNextKey();
	}

	if(m_MaxCharacters == 0)
	{
		CCD->Log()->Critical("File %s - Line %d: No characters defined!", __FILE__, __LINE__);
		delete CCD;
		exit(-100);
	}
}


/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
void CRFMenu::LoadText(const std::string& menutxt)
{
	CIniFile iniFile(menutxt);

	if(!iniFile.ReadFile())
	{
		CCD->Log()->Debug("Failed to open menu text file " + menutxt);
		return;
	}

	std::string value;
	std::string name;

	// Save/Load text
	value = iniFile.GetValue("SaveLoad", "newsavegame");
	if(!value.empty())
	{
		m_NewSavegameLabel = value;

		if(CCD->GUIManager()->IsWindowPresent("Menu/SaveGame/SavegameList"))
		{
			CEGUI::Listbox* savelistbox = static_cast<CEGUI::Listbox*>(CCD->GUIManager()->GetWindow("Menu/SaveGame/SavegameList"));
			CEGUI::ListboxTextItemEx *item = NULL;

			if(savelistbox->getItemCount() > 0)
				item = (CEGUI::ListboxTextItemEx*)savelistbox->getListboxItemFromIndex(0);

			if(item && item->getID() == m_SavegameCounter)
			{
				item->setText((CEGUI::utf8*)m_NewSavegameLabel.c_str());
			}
		}
	}

	// Window text
	name = iniFile.FindFirstName("WindowText");

	while(!name.empty())
	{
		value = iniFile.GetValue("WindowText", name);
		CCD->GUIManager()->SetWindowText(name, value);

		name = iniFile.FindNextName();
	}

	// Tooltip text
	name = iniFile.FindFirstName("TooltipText");

	while(!name.empty())
	{
		value = iniFile.GetValue("TooltipText", name);
		CCD->GUIManager()->SetTooltipText(name, value);

		name = iniFile.FindNextName();
	}

	// Action names
	name = iniFile.FindFirstName("ActionNames");

	while(!name.empty())
	{
		value = iniFile.GetValue("ActionNames", name);
		if(!value.empty())
			CCD->Input()->SetActionName(value, Name2ActionID(name));

		name = iniFile.FindNextName();
	}

	// Key names
	value = iniFile.GetValue("KeyNames", "mousebuttonleft");
	if(!value.empty())	CCD->Input()->SetMouseButtonName(value, OIS::MB_Left);

	value = iniFile.GetValue("KeyNames", "mousebuttonright");
	if(!value.empty())	CCD->Input()->SetMouseButtonName(value, OIS::MB_Right);

	value = iniFile.GetValue("KeyNames", "mousebuttonmiddle");
	if(!value.empty())	CCD->Input()->SetMouseButtonName(value, OIS::MB_Middle);

	value = iniFile.GetValue("KeyNames", "mousebutton3");
	if(!value.empty())	CCD->Input()->SetMouseButtonName(value, OIS::MB_Button3);

	value = iniFile.GetValue("KeyNames", "mousebutton4");
	if(!value.empty())	CCD->Input()->SetMouseButtonName(value, OIS::MB_Button4);

	value = iniFile.GetValue("KeyNames", "mousebutton5");
	if(!value.empty())	CCD->Input()->SetMouseButtonName(value, OIS::MB_Button5);

	value = iniFile.GetValue("KeyNames", "mousebutton6");
	if(!value.empty())	CCD->Input()->SetMouseButtonName(value, OIS::MB_Button6);

	value = iniFile.GetValue("KeyNames", "mousebutton7");
	if(!value.empty())	CCD->Input()->SetMouseButtonName(value, OIS::MB_Button7);

	value = iniFile.GetValue("KeyNames", "unassigned");
	if(!value.empty())	m_KeyNameUnassigned = value;

	FillActionMapList();
}


/* ------------------------------------------------------------------------------------ */
// LoadConfiguration
/* ------------------------------------------------------------------------------------ */
void CRFMenu::LoadConfiguration(const std::string& filename)
{
	CCD->Log()->Debug("Parsing menu configuration file...");

	/*
	TODO
	crosshairs
	loadscreen
	loadmsg
	savemsg
	*/

	CIniFile iniFile(filename);

	if(!iniFile.ReadFile())
	{
		CCD->Log()->Error("File %s - Line %d: Failed to open menu config file '%s'",
							__FILE__, __LINE__, filename.c_str());
		return;
	}

	std::string section = iniFile.FindFirstKey();
	std::string value;

	// first load all schemes
	std::string name = iniFile.FindFirstName("Schemes");

	while(!name.empty())
	{
		value = iniFile.GetValue("Schemes", name);
		CCD->GUIManager()->LoadScheme(value);

		name = iniFile.FindNextName();
	}

	while(!section.empty())
	{
		if(section == "General")
		{
			m_TimeFade = static_cast<float>(iniFile.GetValueI(section, "fadetime")) * 0.001f;

			value = iniFile.GetValue(section, "aspectratio");
			if(!value.empty())
			{
				// extract ratio
				float ratio, designratio;
				int iwidth, iheight;
				std::string width = value, height = value;

				width.erase(width.find_first_of(":"));
				height.erase(0, height.find_last_of(":")+1);
				iwidth = atoi(width.c_str());
				iheight = atoi(height.c_str());
				ratio = (float)CCD->Engine()->Width()/(float)CCD->Engine()->Height();
				designratio = (float)iwidth/(float)iheight;

				if(ratio > designratio)
				{
					float relwidth = designratio/ratio;
					CMenuState::GetSingletonPtr()->GetDefaultWindow()->setWidth(CEGUI::UDim(relwidth, 0.0f));
					CMenuState::GetSingletonPtr()->GetDefaultWindow()->setXPosition(CEGUI::UDim((1.0f - relwidth)*0.5f, 0.0f));
				}
				else if(ratio < designratio)
				{
					float relheight = ratio/designratio;
					CMenuState::GetSingletonPtr()->GetDefaultWindow()->setHeight(CEGUI::UDim(relheight, 0.0f));
					CMenuState::GetSingletonPtr()->GetDefaultWindow()->setYPosition(CEGUI::UDim((1.0f - relheight)*0.5f, 0.0f));
				}
			}

			// TODO
			value = iniFile.GetValue(section, "crosshairs");
			if(!value.empty())
			{
				m_FCrosshairs = CreateFromFileName(value.c_str());
				if(m_FCrosshairs == NULL)
				{
					CCD->Log()->Error("Bad file name %s", value.c_str());
				}
				else
				{
					geEngine_AddBitmap(CCD->Engine()->Engine(), m_FCrosshairs);
				}
			}

			// TODO
			value = iniFile.GetValue(section, "loadscreen");
			if(!value.empty())
			{
				strcpy(m_Loading, value.c_str());
			}
		}
		else if(section == "Layouts")
		{
			std::string name = iniFile.FindFirstName(section);

			while(!name.empty())
			{
				if(m_Sections.find(name) == m_Sections.end())
				{
					value = iniFile.GetValue(section, name);

					if(!value.empty())
					{
						CEGUI::Window *win = CCD->GUIManager()->LoadWindowLayout(value);
						m_Sections[name] = win;
					}
				}

				name = iniFile.FindNextName();
			}

			// set main menu active
			if(m_Sections.find("main") != m_Sections.end())
			{
				m_SectionStack.push(m_Sections["main"]);
				CMenuState::GetSingletonPtr()->GetDefaultWindow()->addChildWindow(m_SectionStack.top());
			}
			// exit if there's no main menu
			else
			{
				CCD->Log()->Critical("No main menu defined!");
				delete CCD;
				exit(-100);
			}
		}
		else if(section == "Audio")
		{
			value = iniFile.GetValue(section, "mouseclick");
			if(!value.empty())
			{
				geVFile *vFile;

				if(CFileManager::GetSingletonPtr()->OpenRFFile(&vFile, kAudioFile, value.c_str(), GE_VFILE_OPEN_READONLY))
				{
					m_MouseClick = geSound_LoadSoundDef(CCD->Engine()->AudioSystem(), vFile);
					geVFile_Close(vFile);
				}
				else
				{
					CCD->Log()->Warning("Missing MouseClick Sound!");
				}
			}

			value = iniFile.GetValue(section, "keyclick");
			if(!value.empty())
			{
				geVFile *vFile;

				if(CFileManager::GetSingletonPtr()->OpenRFFile(&vFile, kAudioFile, value.c_str(), GE_VFILE_OPEN_READONLY))
				{
					m_KeyClick = geSound_LoadSoundDef(CCD->Engine()->AudioSystem(), vFile);
					geVFile_Close(vFile);
				}
				else
				{
					CCD->Log()->Warning("Missing KeyClick Sound!");
				}
			}

			value = iniFile.GetValue(section, "slideclick");
			if(!value.empty())
			{
				geVFile *vFile;

				if(CFileManager::GetSingletonPtr()->OpenRFFile(&vFile, kAudioFile, value.c_str(), GE_VFILE_OPEN_READONLY))
				{
					m_SlideClick = geSound_LoadSoundDef(CCD->Engine()->AudioSystem(), vFile);
					geVFile_Close(vFile);
				}
				else
				{
					CCD->Log()->Warning("Missing SlideClick Sound!");
				}
			}

			value = iniFile.GetValue(section, "music");
			if(!value.empty())
			{
				if(EndsWith(value, ".mid"))
				{
					m_Music = value;

					SAFE_DELETE(m_MIDIPlayer);

					m_MIDIPlayer = new CMIDIAudio();

					if(m_MIDIPlayer == NULL)
					{
						CCD->Log()->Warning("MIDI Player failed to instantiate");
					}
					else
					{
						m_MusicType = 1;
					}
				}
				else
				{
					m_Music = CFileManager::GetSingletonPtr()->GetDirectory(kAudioStreamFile);
					m_Music += "\\" + value;
					m_dsPtr = static_cast<LPDIRECTSOUND>(geSound_GetDSound());

					SAFE_DELETE(m_Streams);

					m_Streams = new StreamingAudio(m_dsPtr);

					if(m_Streams)
					{
						char tmp[128];
						strcpy(tmp, m_Music.c_str());
						if(m_Streams->Create(tmp))
						{
							m_MusicType = 0;
						}
					}
				}
			}
		}

		section = iniFile.FindNextKey();
	}

	// set window text
	SetPlayerNameWindowText();
	CCD->GUIManager()->SetWindowText("Menu/Multiplayer/IP", m_IP);
	CCD->GUIManager()->HideWindow("Menu/Main/Confirm/Quit");
	CCD->GUIManager()->HideWindow("Menu/SaveGame/Confirm/DeleteSavegame");
	CCD->GUIManager()->HideWindow("Menu/SaveGame/Confirm/OverwriteSavegame");

	if(m_InGame == 0)
	{
		CCD->GUIManager()->HideWindow("Menu/Main/Return");		// show only if in-game
		CCD->GUIManager()->DisableWindow("Menu/Main/SaveGame"); // enable only if in-game
	}

	DisableOrphanedButtons();

	if(CCD->GUIManager()->IsWindowPresent("Menu/LoadGame/SavegameImage"))
		m_LoadGameDefaultImage = CCD->GUIManager()->GetWindow("Menu/LoadGame/SavegameImage")->getProperty("Image");

	if(CCD->GUIManager()->IsWindowPresent("Menu/SaveGame/SavegameImage"))
		m_SaveGameDefaultImage = CCD->GUIManager()->GetWindow("Menu/SaveGame/SavegameImage")->getProperty("Image");

	// fill in savegames
	CEGUI::Listbox *loadlistbox = NULL;
	CEGUI::Listbox *savelistbox = NULL;

	if(CCD->GUIManager()->IsWindowPresent("Menu/LoadGame/SavegameList"))
		loadlistbox = static_cast<CEGUI::Listbox*>(CCD->GUIManager()->GetWindow("Menu/LoadGame/SavegameList"));
	if(CCD->GUIManager()->IsWindowPresent("Menu/SaveGame/SavegameList"))
		savelistbox = static_cast<CEGUI::Listbox*>(CCD->GUIManager()->GetWindow("Menu/SaveGame/SavegameList"));

	if(loadlistbox || savelistbox)
	{
		if(!m_Savegames.empty())
		{
			stdext::hash_map<std::string, std::string>::iterator iter = m_Savegames.begin();

			for(; iter!=m_Savegames.end(); ++iter)
			{
				int number = GetNumberFromFilename(iter->first);
				if(loadlistbox)
					loadlistbox->insertItem(new CEGUI::ListboxTextItemEx(iter->second, number), NULL);
				if(savelistbox)
					savelistbox->insertItem(new CEGUI::ListboxTextItemEx(iter->second, number), NULL);

				if(CCD->GUIManager()->IsWindowPresent("Menu/LoadGame/SavegameImage")
					|| CCD->GUIManager()->IsWindowPresent("Menu/SaveGame/SavegameImage"))
				{
					char filename[64];
					sprintf(filename, "savegame%d.jpg", number);

					if(CFileManager::GetSingletonPtr()->FileExist(kSavegameFile, filename))
					{
						CCD->GUIManager()->CreateImagesetFromImageFile(filename, filename, "saves");
					}
				}
			}
		}

		if(savelistbox)
			savelistbox->insertItem(new CEGUI::ListboxTextItemEx(m_NewSavegameLabel, m_SavegameCounter), NULL);
	}
}


void CRFMenu::DisableOrphanedButtons()
{
	// disable buttons if they lead nowhere
	if(m_Sections.find("multiplayer") == m_Sections.end())
		CCD->GUIManager()->DisableWindow("Menu/Main/Multiplayer");

	if(m_Sections.find("joingame") == m_Sections.end())
		CCD->GUIManager()->DisableWindow("Menu/Multiplayer/JoinGame");

	if(m_Sections.find("loadgame") == m_Sections.end())
		CCD->GUIManager()->DisableWindow("Menu/Main/LoadGame");

	if(m_Sections.find("options") == m_Sections.end())
		CCD->GUIManager()->DisableWindow("Menu/Main/Options");

	if(m_Sections.find("audio") == m_Sections.end())
		CCD->GUIManager()->DisableWindow("Menu/Options/Audio");

	if(m_Sections.find("video") == m_Sections.end())
		CCD->GUIManager()->DisableWindow("Menu/Options/Video");

	if(m_Sections.find("controls") == m_Sections.end())
		CCD->GUIManager()->DisableWindow("Menu/Options/Controls");

	if(m_Sections.find("advanced") == m_Sections.end())
		CCD->GUIManager()->DisableWindow("Menu/Controls/Advanced");

	if(m_Sections.find("language") == m_Sections.end())
		CCD->GUIManager()->DisableWindow("Menu/Options/Language");

	if(m_Sections.find("debug") == m_Sections.end())
		CCD->GUIManager()->DisableWindow("Menu/Options/Debug");

	if(m_Sections.find("mods") == m_Sections.end())
		CCD->GUIManager()->DisableWindow("Menu/Main/Mods");

	if(m_Sections.find("credits") == m_Sections.end())
		CCD->GUIManager()->DisableWindow("Menu/Main/Credits");
}

/* ------------------------------------------------------------------------------------ */
// Destructor
/* ------------------------------------------------------------------------------------ */
CRFMenu::~CRFMenu()
{
	int i;

	// free click sounds
	if(m_MouseClick)
	{
		geSound_FreeSoundDef(CCD->Engine()->AudioSystem(), m_MouseClick);
		m_MouseClick = NULL;
	}
	if(m_KeyClick)
	{
		geSound_FreeSoundDef(CCD->Engine()->AudioSystem(), m_KeyClick);
		m_KeyClick = NULL;
	}
	if(m_SlideClick)
	{
		geSound_FreeSoundDef(CCD->Engine()->AudioSystem(), m_SlideClick);
		m_SlideClick = NULL;
	}

	if(m_FCrosshairs)
	{
		geEngine_RemoveBitmap(CCD->Engine()->Engine(), m_FCrosshairs);
		geBitmap_Destroy(&m_FCrosshairs);
	}

	for(i=0; i<NUM_SELECT; ++i)
		delete m_Characters[i];

	// delete temporary savegame.bmp
	char filename[_MAX_PATH];
	sprintf(filename, "%s\\savegame.bmp", CFileManager::GetSingletonPtr()->GetDirectory(kSavegameFile));
	unlink(filename);

	CMixer mixer(CCD->Engine()->WindowHandle(),
					MIXERLINE_COMPONENTTYPE_DST_SPEAKERS,
					NO_SOURCE, MIXERCONTROL_CONTROLTYPE_VOLUME);

	if(mixer.IsOk())
		mixer.SetControlValue(m_WinVol);

	delete m_MIDIPlayer;

	m_Sections.clear();
	while(!m_SectionStack.empty())
		m_SectionStack.pop();

	delete m_EventHandler;
}


void CRFMenu::Enter()
{
	if(m_MusicType != -1)
	{
		if(m_MusicType == 1)
			MIDIPlayer()->Play(m_Music.c_str(), true);
		else
			m_Streams->Play(true);
	}

	SetmMusicVol(m_mVolLevel);

	FadeSet(-1, m_TimeFade);
}


void CRFMenu::Exit()
{
	if(m_MusicType != -1)
	{
		if(m_MusicType == 1)
		{
			MIDIPlayer()->Stop();
		}
		else
		{
			m_Streams->Delete();
			SAFE_DELETE(m_Streams);
		}
	}

	CCD->Log()->Debug("Saving Attributes & Settings files...");

	if(m_InGame == 1)
		CCD->Player()->SaveAttributesAscii("attributes.txt");

	SaveSettings();

	CCD->Input()->SaveKeymap("keyboard.ini");
}


void CRFMenu::SaveSettings()
{
	FILE *fd = CFileManager::GetSingletonPtr()->OpenRFFile(kConfigFile, "setup.ini", "wb");

	if(!fd)
	{
		CCD->Log()->Warning("Failed to create setup.ini file");
	}
	else
	{
		CEGUI::Slider* slider;
		CEGUI::Checkbox *checkbox;
		float gamma = GetGamma();
		float volume = 0.5f;
		float sensitivity = (m_MouseSen - MOUSEMIN)/(MOUSEMAX - MOUSEMIN);
		bool cdmusic = false;

		slider = static_cast<CEGUI::Slider*>(CCD->GUIManager()->GetWindow("Menu/Audio/SoundVolume"));
		if(slider) volume = slider->getCurrentValue();

		checkbox = static_cast<CEGUI::Checkbox*>(CCD->GUIManager()->GetWindow("Menu/Audio/CDMusic"));
		if(checkbox) cdmusic = checkbox->isSelected();

		fwrite(&gamma,				sizeof(float),	1, fd);
		fwrite(&volume,				sizeof(float),	1, fd);
		fwrite(&sensitivity,		sizeof(float),	1, fd);
		fwrite(&cdmusic,			sizeof(bool),	1, fd);
		fwrite(&m_bCrosshairs,		sizeof(bool),	1, fd);
		fwrite(&m_bMouseReverse,	sizeof(bool),	1, fd);
		fwrite(&m_bMouseFilter,		sizeof(bool),	1, fd);
		fwrite(&m_Detail,			sizeof(float),	1, fd);
		fwrite(&m_mVolLevel,		sizeof(float),	1, fd);
		fwrite(&m_bStencilShadows,	sizeof(bool),	1, fd);
		fclose(fd);
	}
}

/* ------------------------------------------------------------------------------------ */
// DrawBitmap
/* ------------------------------------------------------------------------------------ */
void CRFMenu::DrawBitmap(const geBitmap *Bitmap, geRect *Source, uint32 x, uint32 y)
{
	geEngine_DrawBitmap(CCD->Engine()->Engine(), Bitmap, Source, x, y);
}

/* ------------------------------------------------------------------------------------ */
// Stop Menu Music
/* ------------------------------------------------------------------------------------ */
void CRFMenu::StopMusic()
{
	if(m_MusicType != -1)
	{
		if(m_MusicType == 1)
			MIDIPlayer()->Stop();
		else
			m_Streams->Stop();
	}
}

/* ------------------------------------------------------------------------------------ */
// SetMusicVol
/* ------------------------------------------------------------------------------------ */
void CRFMenu::SetMusicVol(float vol)
{
	CMixer mixer(CCD->Engine()->WindowHandle(),
					MIXERLINE_COMPONENTTYPE_DST_SPEAKERS,
					NO_SOURCE,
					MIXERCONTROL_CONTROLTYPE_VOLUME);

	if(!mixer.IsOk())
		return;

	DWORD dw = static_cast<DWORD>(vol * 65535.0f);
	mixer.SetControlValue(dw);
}

/* ------------------------------------------------------------------------------------ */
// SetmMusicVol
// vol in range [0; 1]
/* ------------------------------------------------------------------------------------ */
void CRFMenu::SetmMusicVol(float vol)
{
	m_mVolLevel = vol;
	//long volume = static_cast<long>(vol * 10000.f - 10000.f); // [-10000; 0] (min/max in DirectSound)
	long volume = static_cast<long>(vol * 10000.f);

	if(volume <= 1)
		volume = -10000;
	else if(volume > 10000)
		volume = 0;
	else
		volume = static_cast<long>(log10(static_cast<float>(volume)) * 2500.f - 10000.f);

	// [-10000; 0] (min/max in DirectSound)
	if(m_MusicType != -1)
	{
		if(m_MusicType != 1)
			m_Streams->SetVolume(volume);
	}

	if(CCD->Level())
	{
		if(CCD->Level()->AudioStreams())
			CCD->Level()->AudioStreams()->SetVolume(volume);

		if(CCD->Level()->SoundtrackToggles())
			CCD->Level()->SoundtrackToggles()->SetVolume(volume);
	}

	CMixer mixer(CCD->Engine()->WindowHandle(),
					MIXERLINE_COMPONENTTYPE_DST_SPEAKERS,
					MIXERLINE_COMPONENTTYPE_SRC_SYNTHESIZER,
					MIXERCONTROL_CONTROLTYPE_VOLUME);

	if(mixer.IsOk())
	{
		DWORD dw = static_cast<DWORD>(vol * 65535.0f);
		mixer.SetControlValue(dw);
	}

	CMixer mixercd(CCD->Engine()->WindowHandle(),
					MIXERLINE_COMPONENTTYPE_DST_SPEAKERS,
					MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC,
					MIXERCONTROL_CONTROLTYPE_VOLUME);

	if(mixercd.IsOk())
	{
		DWORD dw = static_cast<DWORD>(vol * 65535.0f);
		mixercd.SetControlValue(dw);
	}
}

/* ------------------------------------------------------------------------------------ */
// GameLoop
//
// setup and run level
/* ------------------------------------------------------------------------------------ */
void CRFMenu::GameLoop()
{
	// Now that we have the initial inventory and attributes for the
	// ..first level, we're going to save them off to a temp. file so
	// ..if the player gets killed, when we restart we can do a fast
	// ..and easy reload of the "game startup" attributes and
	// ..inventory.

	CCD->Log()->Debug("Setup and Run Level");

	CCD->Level()->ShowFog();			// Show fog, if enabled
	CCD->Level()->ActivateClipPlane();	// Activate clipping plane, if enabled

	// On the load of the first level, we want to set up the player
	// ..attributes and HUD display format.
	CCD->HUD()->LoadConfiguration();
	CCD->Player()->SaveAttributes("pdoa.bin");
	CCD->Player()->SaveAttributesAscii("attributes.txt");
	CCD->SetLevelData();

	SetInGame();
	CCD->GUIManager()->EnableWindow("Menu/Main/SaveGame");

	CGameStateManager::GetSingletonPtr()->PushState(CPlayState::GetSingletonPtr());
}

/* ------------------------------------------------------------------------------------ */
// DisplayCrossHair
/* ------------------------------------------------------------------------------------ */
void CRFMenu::DisplayCrossHair()
{
	geVec3d theRotation;
	geVec3d thePosition;
	geXForm3d Xf;
	geVec3d Back;
	geVec3d Direction;
	geExtBox theBox;
	GE_Collision Collision;
	GE_LVertex Vert;
	geActor *theActor = CCD->Player()->GetActor();

	Vert.r = Vert.g = Vert.b = Vert.a = 255.0f;

	theBox.Min.X = theBox.Min.Y = theBox.Min.Z = -0.1f;
	theBox.Max.X = theBox.Max.Y = theBox.Max.Z = 0.1f;

	float CurrentDistance = 400.0f;

	CCD->CameraManager()->GetRotation(&theRotation);
	CCD->CameraManager()->GetPosition(&thePosition);

	geXForm3d_SetZRotation(&Xf, theRotation.Z);
	geXForm3d_RotateX(&Xf, theRotation.X);
	geXForm3d_RotateY(&Xf, theRotation.Y);

	geXForm3d_GetIn(&Xf, &Direction);
	geVec3d_AddScaled(&thePosition, &Direction, CurrentDistance, &Back);

	CCD->Collision()->IgnoreContents(false);
	CCD->Collision()->CheckLevel(RGF_COLLISIONLEVEL_1);

	char BoneHit[64];
	BoneHit[0] = '\0';

	if(CCD->Collision()->CheckForBoneCollision(&theBox.Min, &theBox.Max,
		thePosition, Back, &Collision, theActor, BoneHit, true))
	{
		CurrentDistance = geVec3d_DistanceBetween(&(Collision.Impact), &thePosition);

		if(CurrentDistance > 400.0f)
		{
			CurrentDistance = 400.0f;
		}
		else
		{
			// change cursor lighting
			if(Collision.Actor && CCD->Weapons()->GetAllowLit())
			{
				geVec3d Fill = {0.0f, 1.0f, 0.0f};
				geXForm3d	Xf;
				geXForm3d	XfT;
				geVec3d NewFillNormal;
				geActor_GetBoneTransform(Collision.Actor, RootBoneName(Collision.Actor), &Xf);
				geXForm3d_GetTranspose(&Xf, &XfT);
				geXForm3d_Rotate(&XfT, &Fill, &NewFillNormal);
				geActor_SetLightingOptions(Collision.Actor, GE_TRUE, &NewFillNormal,
											255.0f, 255.0f, 255.0f,
											255.0f, 255.0f, 255.0f,
											GE_TRUE, 6, NULL, GE_FALSE);
				geVec3d LitColor = CCD->Weapons()->GetLitColor();

				Vert.r = LitColor.X;
				Vert.g = LitColor.Y;
				Vert.b = LitColor.Z;
				Vert.a = 255.0f;
			}
		}

		geVec3d_AddScaled(&thePosition, &Direction, CurrentDistance, &Back);
	}

	Vert.u = Vert.v = 0.0f;
	Vert.X = Back.X;
	Vert.Y = Back.Y;
	Vert.Z = Back.Z;


	// Add "| GE_RENDER_DEPTH_SORT_BF" so that crosshair always draws on top of wall decals
	geWorld_AddPolyOnce(CCD->World(),
						&Vert,
						1,
						CCD->Weapons()->GetCrossHair(),
						GE_TEXTURED_POINT,
						GE_RENDER_DO_NOT_OCCLUDE_SELF | GE_RENDER_DEPTH_SORT_BF,
						1.0f);
}


/* ------------------------------------------------------------------------------------ */
// TODO: DisplaySplash
// except for color key, function is very similar to CGenesisEngine::DisplaySplash
//
// display splash screen with color 255 as transparent
/* ------------------------------------------------------------------------------------ */
void CRFMenu::DisplaySplash()
{
	geBitmap *theBmp = CreateFromFileName(m_Loading);
	geBitmap_Info BmpInfo;

	if(theBmp != NULL)
	{
		if(geBitmap_GetInfo(theBmp, &BmpInfo, NULL) == GE_TRUE)
		{
			int x = (CCD->Engine()->Width() - BmpInfo.Width) / 2;
			int y = (CCD->Engine()->Height() - BmpInfo.Height) / 2;

			geBitmap_SetColorKey(theBmp, GE_TRUE, 255, GE_FALSE);

			if(geEngine_AddBitmap(CCD->Engine()->Engine(), theBmp) == GE_FALSE)
			{
				CCD->Log()->Warning("File %s - Line %d: DisplaySplash: AddBitmap failed on '%s'",
									__FILE__, __LINE__, m_Loading);
				return;
			}

			if(CCD->GetHasFocus())
			{
				geRect cameraRect;
				cameraRect.Left	= 0;
				cameraRect.Right	= CCD->Engine()->Width() - 1;
				cameraRect.Top	= 0;
				cameraRect.Bottom = CCD->Engine()->Height() - 1;
				geCamera *camera = geCamera_Create(2.0f, &cameraRect);

				geEngine_BeginFrame(CCD->Engine()->Engine(), camera, GE_TRUE);

				if(geEngine_DrawBitmap(CCD->Engine()->Engine(), theBmp, NULL, x, y) == GE_FALSE)
				{
					CCD->Log()->Warning("File %s - Line %d: DisplaySplash: DrawBitmap failed on '%s'",
										__FILE__, __LINE__, m_Loading);
				}

				geEngine_EndFrame(CCD->Engine()->Engine());
				geCamera_Destroy(&camera);
			}

			geEngine_RemoveBitmap(CCD->Engine()->Engine(), theBmp);
		}

		geBitmap_Destroy(&theBmp);
	}
}

/* ------------------------------------------------------------------------------------ */
// save a screen shot
/* ------------------------------------------------------------------------------------ */
void CRFMenu::ScreenShot()
{
	char filename[MAX_PATH];

	sprintf(filename, "%s\\screen%.3d.bmp",
		CFileManager::GetSingletonPtr()->GetDirectory(kScreenshotFile),
		m_ScreenshotCount);

	if(geEngine_ScreenShot(CCD->Engine()->Engine(), filename) == GE_FALSE)
	{
		sxLog::GetSingletonPtr()->Debug("Failed to create screenshot file");
		return;
	}

	sxLog::GetSingletonPtr()->Debug("Screenshot saved to %s", filename);

	++m_ScreenshotCount;
}


/* ------------------------------------------------------------------------------------ */
// initialize all menu items
/* ------------------------------------------------------------------------------------ */
void CRFMenu::Initialize()
{
	CCD->Log()->Debug("Initializing Menu...");

	InitializeWidgets();

	// now that all widgets are initialized set up the event handling
	m_EventHandler = new CMenuEventHandler(*this);
	m_EventHandler->SubscribeEvents();

	LoadControlConfiguration();

	FillActionMapList();
}


void CRFMenu::InitializeWidgets()
{
	float gamma = GetGamma();
	m_Detail = 0.5f;
	m_bStencilShadows = false;
	float volume = 0.5f;
	float sensitivity;
	bool cdmusic = false;

	// set CD Music box from CD player status
	if(CCD->CDPlayer()->GetCdOn())
	{
		cdmusic = true;
	}

	// set mouse sensitivity slider
	m_MouseSen = 0.002f;
	sensitivity = (m_MouseSen - MOUSEMIN)/(MOUSEMAX - MOUSEMIN);

	// set mouse reverse
	m_bMouseReverse = false;

	// set mouse filter
	m_bMouseFilter = false;
	m_Filter.x = -1;

	// set crosshair
	m_bCrosshairs = false;

	// set no clipping
	m_bNoClip = false;

	// set framerate box
	m_bFrameRate = false;

	// set debug info box
	m_bDebugInfo = false;

	// set bounding box
	m_bBoundingBox = false;
	m_bSEBoundingBox = false;


	// load setup file if it exists

	sxLog::GetSingletonPtr()->SetMBPriority(LP_CRITICAL);
	FILE *fd;
	fd = CFileManager::GetSingletonPtr()->OpenRFFile(kConfigFile, "setup.ini", "rb");
	sxLog::GetSingletonPtr()->SetMBPriority(LP_ERROR);

	if(fd)
	{
		fread(&gamma,				sizeof(float),	1, fd);
		SetGamma(gamma);

		fread(&volume,				sizeof(float),	1, fd);
		fread(&sensitivity,			sizeof(float),	1, fd);
		m_MouseSen = sensitivity * (MOUSEMAX - MOUSEMIN) + MOUSEMIN;

		fread(&cdmusic,				sizeof(bool),	1, fd);
		fread(&m_bCrosshairs,		sizeof(bool),	1, fd);
		fread(&m_bMouseReverse,		sizeof(bool),	1, fd);
		fread(&m_bMouseFilter,		sizeof(bool),	1, fd);
		fread(&m_Detail,			sizeof(float),	1, fd);
		fread(&m_mVolLevel,			sizeof(float),	1, fd);
		fread(&m_bStencilShadows,	sizeof(bool),	1, fd);

		fclose(fd);
	}

	CEGUI::Checkbox *checkbox;
	CEGUI::Slider *slider;
	CEGUI::RadioButton *radio;

	// language
	std::string windowname = "Menu/Language/" + CCD->LanguageManager()->GetActiveLanguage()->GetName();
	radio = static_cast<CEGUI::RadioButton*>(CCD->GUIManager()->GetWindow(windowname));
	if(radio) radio->setSelected(true);

	// video
	slider = static_cast<CEGUI::Slider*>(CCD->GUIManager()->GetWindow("Menu/Video/Gamma"));
	if(slider) slider->setCurrentValue(gamma);

	slider = static_cast<CEGUI::Slider*>(CCD->GUIManager()->GetWindow("Menu/Video/Detail"));
	if(slider) slider->setCurrentValue(m_Detail);

	checkbox = static_cast<CEGUI::Checkbox*>(CCD->GUIManager()->GetWindow("Menu/Video/Shadows"));
	if(checkbox) checkbox->setSelected(m_bStencilShadows);

	// advanced
	slider = static_cast<CEGUI::Slider*>(CCD->GUIManager()->GetWindow("Menu/Advanced/MouseSensitivity"));
	if(slider) slider->setCurrentValue(sensitivity);

	checkbox = static_cast<CEGUI::Checkbox*>(CCD->GUIManager()->GetWindow("Menu/Advanced/Crosshairs"));
	if(checkbox) checkbox->setSelected(m_bCrosshairs);

	checkbox = static_cast<CEGUI::Checkbox*>(CCD->GUIManager()->GetWindow("Menu/Advanced/ReverseMouse"));
	if(checkbox) checkbox->setSelected(m_bMouseReverse);

	checkbox = static_cast<CEGUI::Checkbox*>(CCD->GUIManager()->GetWindow("Menu/Advanced/MouseFilter"));
	if(checkbox) checkbox->setSelected(m_bMouseFilter);

	// audio
	checkbox = static_cast<CEGUI::Checkbox*>(CCD->GUIManager()->GetWindow("Menu/Audio/CDMusic"));
	if(checkbox) checkbox->setSelected(cdmusic);
	if(CCD->CDPlayer()->GetCdOn() && !cdmusic)
	{
		CCD->CDPlayer()->SetCdOn(false);
		CCD->CDPlayer()->Stop();
	}

	slider = static_cast<CEGUI::Slider*>(CCD->GUIManager()->GetWindow("Menu/Audio/SoundVolume"));
	if(slider) slider->setCurrentValue(volume);
	SetMusicVol(volume);

	slider = static_cast<CEGUI::Slider*>(CCD->GUIManager()->GetWindow("Menu/Audio/MusicVolume"));
	if(slider) slider->setCurrentValue(m_mVolLevel);
	SetmMusicVol(m_mVolLevel);

	if(CCD->GetCharacterSelection() && CCD->GUIManager()->IsWindowPresent("Menu/Character/CharacterImage"))
		CCD->GUIManager()->GetWindow("Menu/Character/CharacterImage")->setProperty("Image",	(const CEGUI::utf8*)(GetSelectedCharacter()->GetImage().c_str()));
}


void CRFMenu::LoadControlConfiguration()
{
	CIniFile iniFile("control.ini");

	if(iniFile.ReadFile())
	{
		bool flg = false;
		std::string name = iniFile.FindFirstName("Controls");

		while(!name.empty())
		{
			flg = true;

			std::string value = iniFile.GetValue("Controls", name);

			if(value == "true")
				m_Controls.push_back(Name2ActionID(name));
			else
				CCD->Input()->ClearCodes(Name2ActionID(name));

			name = iniFile.FindNextName();
		}

		if(!flg)
		{
			CCD->Log()->Critical("File %s - Line %d: Missing section in Control.ini", __FILE__, __LINE__);
			delete CCD;
			exit(-333);
		}
	}
	else
	{
		for(int i=1; i<RGF_K_MAXACTION; ++i)
			m_Controls.push_back(i);
	}
}


void CRFMenu::FillActionMapList()
{
	CEGUI::MultiColumnList *listbox = static_cast<CEGUI::MultiColumnList*>(CCD->GUIManager()->GetWindow("Menu/Controls/ActionKeyMap"));
	if(listbox)
	{
		listbox->resetList();

		std::vector<int>::iterator iter = m_Controls.begin();
		for(; iter!=m_Controls.end(); ++iter)
		{
			int action = *iter;
			int row = listbox->addRow();

			// Action ID Name
			CEGUI::ListboxTextItemEx *item = new CEGUI::ListboxTextItemEx((const CEGUI::utf8*)CCD->Input()->GetActionName(action).c_str(), action);
			listbox->setItem(item, CEGUI::MCLGridRef(row, 0));

			// Mapped Column
			std::string keyname = m_KeyNameUnassigned;
			int id = -1;

			if((id = CCD->Input()->GetKeyCode(action)) != -1)
			{
				keyname = CCD->Input()->GetKeyName((OIS::KeyCode)id);
				item = new CEGUI::ListboxTextItemEx(keyname, 0);
			}
			else
			{
				if((id = CCD->Input()->GetMouseButtonID(action)) != -1)
				{
					keyname = CCD->Input()->GetMouseButtonName((OIS::MouseButtonID)id);
				}

				item = new CEGUI::ListboxTextItemEx((const CEGUI::utf8*)keyname.c_str(), 0);
			}

			//item = new CEGUI::ListboxTextItemEx((const CEGUI::utf8*)keyname.c_str(), 0);
			listbox->setItem(item, CEGUI::MCLGridRef(row, 1));
		}
	}
}

/* ------------------------------------------------------------------------------------ */
// PlaySoundDef
//
// replacement for play sound to keep track of all looping sounds
/* ------------------------------------------------------------------------------------ */
geSound* CRFMenu::PlaySoundDef(geSound_System *SoundS, geSound_Def *SoundDef,
							   geFloat Volume, geFloat Pan, geFloat Frequency,
							   geBoolean Loop)
{
	geSound	*Sound;
	SoundList *pool;

	Sound = geSound_PlaySoundDef(SoundS, SoundDef, Volume, Pan, Frequency, Loop);

	if(Loop)
	{
		pool = GE_RAM_ALLOCATE_STRUCT(SoundList);
		memset(pool, 0xdd, sizeof(SoundList));
		pool->next = m_Bottom;
		m_Bottom = pool;

		if(pool->next)
			pool->next->prev = pool;

		pool->Sound = Sound;
		pool->Volume = Volume;
		pool->Pan = Pan;
		pool->Frequency = Frequency;
	}

	return Sound;
}

/* ------------------------------------------------------------------------------------ */
// StopSound
//
// replacement for stop sound to remove all dead entries
/* ------------------------------------------------------------------------------------ */
geBoolean CRFMenu::StopSound(geSound_System *SoundS, geSound *Sound)
{
	SoundList *pool, *temp;

	pool = m_Bottom;

	while(pool != NULL)
	{
		temp = pool->next;

		if(pool->Sound == Sound)
		{
			if(m_Bottom == pool)
			{
				m_Bottom = pool->next;

				if(m_Bottom != NULL)
					m_Bottom->prev = NULL;
			}
			else
			{
				pool->prev->next = pool->next;

				if(pool->next != NULL)
					pool->next->prev = pool->prev;
			}

			geRam_Free(pool);
			break;
		}

		pool = temp;
	}

	return geSound_StopSound(SoundS, Sound);
}

/* ------------------------------------------------------------------------------------ */
// zero volume of all looping sounds
/* ------------------------------------------------------------------------------------ */
void CRFMenu::ClearVolume()
{
	SoundList *pool, *temp;

	pool = m_Bottom;

	while(pool != NULL)
	{
		temp = pool->next;

		if(geSound_SoundIsPlaying(CCD->Engine()->AudioSystem(), pool->Sound) == GE_TRUE)
		{
			geSound_ModifySound(CCD->Engine()->AudioSystem(), pool->Sound, 0.0f, 0.0f, 0.0f);
		}
		else
		{
			if(m_Bottom == pool)
			{
				m_Bottom = pool->next;

				if(m_Bottom != NULL)
					m_Bottom->prev = NULL;
			}
			else
			{
				pool->prev->next = pool->next;

				if(pool->next != NULL)
					pool->next->prev = pool->prev;
			}

			free(pool);
		}

		pool = temp;
	}
}

/* ------------------------------------------------------------------------------------ */
// ResetVolume
/* ------------------------------------------------------------------------------------ */
void CRFMenu::ResetVolume()
{
	SoundList *pool, *temp;

	pool = m_Bottom;

	while(pool != NULL)
	{
		temp = pool->next;
		geSound_ModifySound(CCD->Engine()->AudioSystem(),
							pool->Sound,
							pool->Volume,
							pool->Pan,
							pool->Frequency);
		pool = temp;
	}
}

/* ------------------------------------------------------------------------------------ */
// clear out sound list
/* ------------------------------------------------------------------------------------ */
void CRFMenu::DeleteSound()
{
	SoundList *pool, *temp;

	pool = m_Bottom;

	while(pool != NULL)
	{
		temp = pool->next;
		free(pool);
		pool = temp;
	}

	m_Bottom = NULL;
}

/* ------------------------------------------------------------------------------------ */
// get percentage from gamma setting (0.0 ... 1.0)
/* ------------------------------------------------------------------------------------ */
float CRFMenu::GetGamma()
{
	float gamma;
	geEngine_GetGamma(CCD->Engine()->Engine(), &gamma);
	return (gamma - GAMMAMIN) / (GAMMAMAX - GAMMAMIN);
}

/* ------------------------------------------------------------------------------------ */
// set gamma from slider percentage
/* ------------------------------------------------------------------------------------ */
void CRFMenu::SetGamma(float gamma)
{
	gamma = gamma * (GAMMAMAX - GAMMAMIN) + GAMMAMIN;
	geEngine_SetGamma(CCD->Engine()->Engine(), gamma);
}

/* ------------------------------------------------------------------------------------ */
// set detail from slider percentage
/* ------------------------------------------------------------------------------------ */
void CRFMenu::SetDetail(float value)
{
	m_Detail = value;

	if(m_InGame)
	{
		EnvironmentSetup *envSetup = CCD->Level()->GetEnvironmentSetup();

		if(envSetup)
		{
			if(envSetup->EnableDistanceFog == GE_TRUE)
			{
				float start = envSetup->FogStartDistLow + (envSetup->FogStartDistHigh - envSetup->FogStartDistLow) * m_Detail;
				float end = envSetup->TotalFogDistLow + (envSetup->TotalFogDistHigh - envSetup->TotalFogDistLow) * m_Detail;
				CCD->Engine()->SetFogParameters(envSetup->DistanceFogColor,	start, end);
			}

			if(envSetup->UseFarClipPlane == GE_TRUE)
			{
				float dist = envSetup->FarClipPlaneDistLow + (envSetup->FarClipPlaneDistHigh - envSetup->FarClipPlaneDistLow) * m_Detail;
				CCD->CameraManager()->SetFarClipPlane(dist);
			}
		}
	}
}

/* ------------------------------------------------------------------------------------ */
// SetStencilShadows
/* ------------------------------------------------------------------------------------ */
void CRFMenu::SetStencilShadows(bool state)
{
	m_bStencilShadows = state;

	if(m_InGame)
	{
		EnvironmentSetup *theState = CCD->Level()->GetEnvironmentSetup();

		if(theState)
		{
			geEngine_SetStencilShadowsEnable(CCD->Engine()->Engine(),
											state,
											theState->SShadowsMaxLightToUse,
											theState->SShadowsColor.r,
											theState->SShadowsColor.g,
											theState->SShadowsColor.b,
											theState->SShadowsAlpha);
		}
	}
}

/* ------------------------------------------------------------------------------------ */
// set mouse sensitivity from slider percentage
/* ------------------------------------------------------------------------------------ */
void CRFMenu::SetMouseSensitivity(float value)
{
	m_MouseSen = value * (MOUSEMAX - MOUSEMIN) + MOUSEMIN;
}

/* ------------------------------------------------------------------------------------ */
// DoGame
/* ------------------------------------------------------------------------------------ */
void CRFMenu::DoGame(bool editor)
{
	m_bUseSelect = false;
	m_bUseNameSelect = false;

	if(CCD->GetCharacterSelection() || CCD->GetDifficultSelection() || CCD->GetNameSelection())
	{
		if(editor)
		{
			if(m_MusicType != -1)
			{
				if(m_MusicType == 1)
					MIDIPlayer()->Play(m_Music.c_str(), true);
				else
					m_Streams->Play(true);
			}
		}

		if(CCD->GetCharacterSelection())
		{
			m_bUseSelect = true;
		}

		if(CCD->GetNameSelection())
		{
			m_bUseNameSelect = true;
		}

		while(CMenuState::GetSingletonPtr()->GetDefaultWindow()->getChildCount() > 0)
		{
			CMenuState::GetSingletonPtr()->GetDefaultWindow()->removeChildWindow(CMenuState::GetSingletonPtr()->GetDefaultWindow()->getChildAtIdx(0));
		}

		while(!m_SectionStack.empty())
			m_SectionStack.pop();

		m_SectionStack.push(m_Sections["main"]);

		// add active section
		CMenuState::GetSingletonPtr()->GetDefaultWindow()->addChildWindow(m_SectionStack.top());
	}

	StopMusic();

	if(!editor)
	{
		if(GetInGame() == 1)
		{
			CCD->Level()->AudioStreams()->StopAll(); // stop old streaming audio that might be paused
			CCD->Level()->Shutdown();
			DeleteSound();

			if(CCD->CDPlayer())
				CCD->CDPlayer()->Stop();

			if(CCD->MIDIPlayer())
				CCD->MIDIPlayer()->Stop();
		}

		DisplaySplash();
	}
	else
	{
		if(CCD->GetCharacterSelection() || CCD->GetDifficultSelection() || CCD->GetNameSelection())
			DisplaySplash();
	}

// start multiplayer
	if(CCD->GetMultiPlayer())
	{
		if(!CCD->NetPlayerManager()->Initialize(CCD->GetServer(), m_ServerIP))
		{
			CCD->Log()->Warning("Can't begin Multiplayer session");
			return;
		}
	}
// end multiplayer
	CPlayState::GetSingletonPtr()->ShowCursor(CCD->GetInGameCursor());
	CCD->SetMouseControl(!CCD->GetInGameCursor());

	if(CCD->Level()->Initialize(GetLevelName()) != 0)
	{
		CCD->Log()->Critical("Failed to initialize first level");
		delete CCD;						// Kill off the engine and such
		exit(-333);
	}

	CCD->Level()->DisableFog();				// Turn off fogging for cut scene
	CCD->Level()->DisableClipPlane();			// Turn off the clipping plane as well

	CCD->Level()->TerrainManager()->Init();

	// Ok, move the player avatar to the correct player start in the
	// ..game level.
	if(CCD->Player()->MoveToStart() != RGF_SUCCESS)
	{
		CCD->Log()->Critical("Failed to move player to start");
		delete CCD;
		exit(-336);
	}

	// Play the opening cut scene, if one exists
	CCD->PlayOpeningCutScene();

	// start multiplayer
	if(CCD->GetMultiPlayer())
	{
		SetTimer(CCD->Engine()->WindowHandle(), NULL, TIMERINTERVAL, NULL);
	}
	// end multiplayer

	GameLoop();
}

/* ------------------------------------------------------------------------------------ */
// change selected character
/* ------------------------------------------------------------------------------------ */
void CRFMenu::ChangeSelectedCharacter(bool direction)
{
	if(direction)
	{
		++m_SelectedCharacter;

		if(m_SelectedCharacter >= m_MaxCharacters)
			m_SelectedCharacter = 0;
	}
	else
	{
		--m_SelectedCharacter;

		if(m_SelectedCharacter < 0)
			m_SelectedCharacter = m_MaxCharacters - 1;
	}

	if(CCD->GUIManager()->IsWindowPresent("Menu/Character/CharacterImage"))
		CCD->GUIManager()->GetWindow("Menu/Character/CharacterImage")->setProperty("Image", (const CEGUI::utf8*)(GetSelectedCharacter()->GetImage().c_str()));
}


/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
void CRFMenu::SetPlayerNameWindowText()
{
	if(CCD->GetCharacterSelection())
	{
		CCD->GUIManager()->SetWindowText("Menu/PlayerName/PlayerNameStatic",	GetSelectedCharacter()->GetName());
		CCD->GUIManager()->SetWindowText("Menu/PlayerName/PlayerName",			GetSelectedCharacter()->GetName());
	}
	else
	{
		CCD->GUIManager()->SetWindowText("Menu/PlayerName/PlayerNameStatic",	CCD->GetDefaultPlayerName());
		CCD->GUIManager()->SetWindowText("Menu/PlayerName/PlayerName",			CCD->GetDefaultPlayerName());
	}
}

/* ------------------------------------------------------------------------------------ */
// reset keys to default actions
/* ------------------------------------------------------------------------------------ */
void CRFMenu::ResetActionMapList()
{
	CCD->Input()->Default();
	RefreshActionMapList();
}

/* ------------------------------------------------------------------------------------ */
// refresh the action map listbox
/* ------------------------------------------------------------------------------------ */
void CRFMenu::RefreshActionMapList()
{
	CEGUI::MultiColumnList *listbox = static_cast<CEGUI::MultiColumnList*>(CCD->GUIManager()->GetWindow("Menu/Controls/ActionKeyMap"));

	if(listbox)
	{
		float pos = 0.f;
		CEGUI::Scrollbar *scrollbar = listbox->getVertScrollbar();
		if(scrollbar) pos = scrollbar->getScrollPosition();
		FillActionMapList();
		if(scrollbar) scrollbar->setScrollPosition(pos);
	}
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
std::string CRFMenu::MakeSavegameFilename(int number)
{
	std::ostringstream oss;
	oss << "savegame" << number << ".sav";
	return oss.str();
}


std::string CRFMenu::MakeSavegameLabel()
{
	struct tm *newtime;
	time_t long_time;
	std::string timetext;
	std::string filename;

	time(&long_time);
	newtime = localtime(&long_time);
	timetext = asctime(newtime);
	// alternative to asctime:
	// size_t strftime (char* ptr, size_t maxsize, const char* format, const struct tm* timeptr);
	timetext.erase(0, 4);
	TrimRight(timetext);
	filename = CCD->Engine()->LevelName();
	filename.erase(filename.find_last_of("."));

	return timetext + " " + filename;
}


void CRFMenu::DeleteSavegame(bool confirm)
{
	if(CCD->GUIManager()->IsWindowPresent("Menu/SaveGame/SavegameList"))
	{
		CEGUI::Listbox *savelistbox = NULL;
		savelistbox = static_cast<CEGUI::Listbox*>(CCD->GUIManager()->GetWindow("Menu/SaveGame/SavegameList"));

		if(savelistbox->getFirstSelectedItem())
		{
			int number = savelistbox->getFirstSelectedItem()->getID();

			if(number == m_SavegameCounter)
				return;

			if(confirm && CCD->GUIManager()->IsWindowPresent("Menu/SaveGame/Confirm/DeleteSavegame"))
			{
				CCD->GUIManager()->GetWindow("Menu/SaveGame/Confirm/DeleteSavegame")->setModalState(true);
				CCD->GUIManager()->GetWindow("Menu/SaveGame/Confirm/DeleteSavegame")->moveToFront();
				CCD->GUIManager()->ShowWindow("Menu/SaveGame/Confirm/DeleteSavegame");
				return;
			}

			std::string filename = MakeSavegameFilename(number);
			char imagename[_MAX_PATH];
			sprintf(imagename, "savegame%d.jpg", number);

			bool deleted = CFileManager::GetSingletonPtr()->DeleteRFFile(kSavegameFile, filename.c_str());
			deleted = (CFileManager::GetSingletonPtr()->DeleteRFFile(kSavegameFile, imagename)|| deleted);
			if(deleted)
			{
				savelistbox->removeItem(savelistbox->getFirstSelectedItem());
				if(CCD->GUIManager()->IsWindowPresent("Menu/LoadGame/SavegameList"))
				{
					CEGUI::Listbox *loadlistbox = static_cast<CEGUI::Listbox*>(CCD->GUIManager()->GetWindow("Menu/LoadGame/SavegameList"));
					loadlistbox->removeItem(loadlistbox->findItemWithText(m_Savegames[filename], NULL));
				}
				m_Savegames.erase(filename);
			}
		}
	}
}


void CRFMenu::SaveGame(bool confirm)
{
	if(CCD->GUIManager()->IsWindowPresent("Menu/SaveGame/SavegameList"))
	{
		CEGUI::Listbox *savelistbox = NULL;
		savelistbox = static_cast<CEGUI::Listbox*>(CCD->GUIManager()->GetWindow("Menu/SaveGame/SavegameList"));

		if(savelistbox->getFirstSelectedItem())
		{
			int number = savelistbox->getFirstSelectedItem()->getID();
			std::string filename = MakeSavegameFilename(number);
			std::string label = MakeSavegameLabel();
			label.resize(127); label.resize(128);

			if(number != m_SavegameCounter)
			{
				if(confirm && CCD->GUIManager()->IsWindowPresent("Menu/SaveGame/Confirm/OverwriteSavegame"))
				{
					CCD->GUIManager()->GetWindow("Menu/SaveGame/Confirm/OverwriteSavegame")->setModalState(true);
					CCD->GUIManager()->GetWindow("Menu/SaveGame/Confirm/OverwriteSavegame")->moveToFront();
					CCD->GUIManager()->ShowWindow("Menu/SaveGame/Confirm/OverwriteSavegame");
					return;
				}
			}

			// save game information
			FILE *outFD = CFileManager::GetSingletonPtr()->OpenRFFile(kSavegameFile, filename.c_str(), "wb");

			if(outFD == NULL)
			{
				CCD->Log()->Warning("Failed to create savegame file");
				return;
			}

			fwrite(label.c_str(), sizeof(char), 128, outFD);

			CCD->Engine()->SaveTo(outFD);
			CCD->MenuManager()->SaveTo(outFD, false);
			CCD->Player()->SaveTo(outFD);
			CCD->Level()->Doors()->SaveTo(outFD, false);
			CCD->Level()->Platforms()->SaveTo(outFD, false);
			CCD->Level()->Props()->SaveTo(outFD, false);
			CCD->Level()->Meshes()->SaveTo(outFD, false);
			CCD->Level()->Teleporters()->SaveTo(outFD, false);
			CCD->Level()->MorphingFields()->SaveTo(outFD);
			CCD->MIDIPlayer()->SaveTo(outFD);
			CCD->CDPlayer()->SaveTo(outFD);
			CCD->Level()->Triggers()->SaveTo(outFD, false);
			CCD->Level()->LogicGates()->SaveTo(outFD, false);
			CCD->Level()->Attributes()->SaveTo(outFD, false);
			CCD->Level()->Damage()->SaveTo(outFD, false);
			CCD->CameraManager()->SaveTo(outFD);
			CCD->Weapons()->SaveTo(outFD);
			CCD->Level()->ElectricBolts()->SaveTo(outFD, false);
			CCD->Level()->CountDownTimers()->SaveTo(outFD, false);
			CCD->Level()->ChangeAttributes()->SaveTo(outFD, false);
			CCD->Level()->ChangeLevels()->SaveTo(outFD, false);
			CCD->Level()->ActorMaterials()->SaveTo(outFD, false);
			CCD->Level()->ModelManager()->SaveTo(outFD, false);
			CCD->SaveTo(outFD);

			fclose(outFD);

			savelistbox->getFirstSelectedItem()->setText(label);
			savelistbox->requestRedraw();
			m_Savegames[filename] = label;

			if(CCD->GUIManager()->IsWindowPresent("Menu/LoadGame/SavegameImage")
				|| CCD->GUIManager()->IsWindowPresent("Menu/SaveGame/SavegameImage"))
			{
				char oldname[_MAX_PATH];
				char newname[_MAX_PATH];
				sprintf(oldname, "%s\\savegame.bmp", CFileManager::GetSingletonPtr()->GetDirectory(kSavegameFile));
				sprintf(newname, "%s\\savegame%d.jpg", CFileManager::GetSingletonPtr()->GetDirectory(kSavegameFile), number);

				unlink(newname);
				CopyImageFile(oldname, newname);

				sprintf(newname, "savegame%d.jpg", number);
				CCD->GUIManager()->DestroyImageset(newname);
				CCD->GUIManager()->CreateImagesetFromImageFile(newname, newname, "saves");

				std::string value = "set:";
				value += newname;
				value += " image:full_image";

				if(CCD->GUIManager()->IsWindowPresent("Menu/SaveGame/SavegameImage"))
					CCD->GUIManager()->GetWindow("Menu/SaveGame/SavegameImage")->setProperty("Image", value);

				if(CCD->GUIManager()->IsWindowPresent("Menu/LoadGame/SavegameImage")
					&& CCD->GUIManager()->IsWindowPresent("Menu/LoadGame/SavegameList"))
				{
					CEGUI::Listbox *loadlistbox = static_cast<CEGUI::Listbox*>(CCD->GUIManager()->GetWindow("Menu/LoadGame/SavegameList"));
					if(loadlistbox->getFirstSelectedItem())
					{
						if(loadlistbox->getFirstSelectedItem()->getID() == number)
							CCD->GUIManager()->GetWindow("Menu/LoadGame/SavegameImage")->setProperty("Image", value);
					}
				}
			}

			// update savegame listbox
			if(number == m_SavegameCounter) // add new savegame item
			{
				if(CCD->GUIManager()->IsWindowPresent("Menu/LoadGame/SavegameList"))
				{
					CEGUI::Listbox *loadlistbox = static_cast<CEGUI::Listbox*>(CCD->GUIManager()->GetWindow("Menu/LoadGame/SavegameList"));
					loadlistbox->insertItem(new CEGUI::ListboxTextItemEx(label, m_SavegameCounter), NULL);
				}

				++m_SavegameCounter;
				savelistbox->insertItem(new CEGUI::ListboxTextItemEx(m_NewSavegameLabel, m_SavegameCounter), NULL);
			}
			else // update already existing savegame entry
			{
				if(CCD->GUIManager()->IsWindowPresent("Menu/LoadGame/SavegameList"))
				{
					CEGUI::Listbox *loadlistbox = static_cast<CEGUI::Listbox*>(CCD->GUIManager()->GetWindow("Menu/LoadGame/SavegameList"));
					loadlistbox->resetList();

					stdext::hash_map<std::string, std::string>::iterator iter = m_Savegames.begin();

					for(; iter!=m_Savegames.end(); ++iter)
					{
						int number = GetNumberFromFilename(iter->first);
						loadlistbox->addItem(new CEGUI::ListboxTextItemEx(iter->second, number));
					}
				}
			}
		}
	}
}


void CRFMenu::LoadGame()
{
	if(CCD->GUIManager()->IsWindowPresent("Menu/LoadGame/SavegameList"))
	{
		CEGUI::Listbox *lbox = NULL;
		lbox = static_cast<CEGUI::Listbox*>(CCD->GUIManager()->GetWindow("Menu/LoadGame/SavegameList"));

		if(lbox->getFirstSelectedItem())
		{
			char filename[64];

			sprintf(filename, "savegame%d.sav", lbox->getFirstSelectedItem()->getID());

			FILE *inFD = CFileManager::GetSingletonPtr()->OpenRFFile(kSavegameFile, filename, "rb");

			if(inFD == NULL)
			{
				CCD->Log()->Warning("No savegame file to restore");
				return;
			}

			DisplaySplash();
			CCD->Level()->Shutdown();

			if(CCD->CDPlayer())
				CCD->CDPlayer()->Stop();

			if(CCD->MIDIPlayer())
				CCD->MIDIPlayer()->Stop();

			CPlayState::GetSingletonPtr()->ShowCursor(CCD->GetInGameCursor());
			CCD->SetMouseControl(!CCD->GetInGameCursor());

			char label[128];
			fread(label, sizeof(char), 128, inFD);
			CCD->Engine()->RestoreFrom(inFD);
			CCD->MenuManager()->RestoreFrom(inFD, false);
			CCD->Level()->Initialize(CCD->Engine()->LevelName());
			CCD->TerrainManager()->Init();
			CCD->Player()->RestoreFrom(inFD);
			CCD->TerrainManager()->Frame();
			CCD->Level()->Doors()->RestoreFrom(inFD, false);
			CCD->Level()->Platforms()->RestoreFrom(inFD, false);
			CCD->Level()->Props()->RestoreFrom(inFD, false);
			CCD->Level()->Meshes()->RestoreFrom(inFD, false);
			CCD->Level()->Teleporters()->RestoreFrom(inFD, false);
			CCD->Level()->MorphingFields()->RestoreFrom(inFD);
			CCD->MIDIPlayer()->RestoreFrom(inFD);
			CCD->CDPlayer()->RestoreFrom(inFD);
			CCD->Level()->Triggers()->RestoreFrom(inFD, false);
			CCD->Level()->LogicGates()->RestoreFrom(inFD, false);
			CCD->Level()->Attributes()->RestoreFrom(inFD, false);
			CCD->Level()->Damage()->RestoreFrom(inFD, false);
			CCD->CameraManager()->RestoreFrom(inFD);
			CCD->Weapons()->RestoreFrom(inFD);
			CCD->Level()->ElectricBolts()->RestoreFrom(inFD, false);
			CCD->Level()->CountDownTimers()->RestoreFrom(inFD, false);
			CCD->Level()->ChangeAttributes()->RestoreFrom(inFD, false);
			CCD->Level()->ChangeLevels()->RestoreFrom(inFD, false);
			CCD->Level()->ActorMaterials()->RestoreFrom(inFD, false);
			CCD->Level()->ModelManager()->RestoreFrom(inFD, false);
			CCD->RestoreFrom(inFD);

			fclose(inFD);

			// run game
			StopMusic(); // stop menu music
			DisplaySplash();
			GameLoop();
		}
	}
}


static void CopyImageFile(const char *srcPath, const char *destPath)
{
	FREE_IMAGE_FORMAT fif = FreeImage_GetFIFFromFilename(srcPath);
	if(fif >= 0)
	{
		FIBITMAP *Fbmp = FreeImage_Load(fif, srcPath, 0);
		FreeImage_Save(FIF_JPEG, Fbmp, destPath, JPEG_QUALITYSUPERB);
	}
}

/* ------------------------------------------------------------------------------------ */
// FadeSet
/* ------------------------------------------------------------------------------------ */
void CRFMenu::FadeSet(int Dir, float Time)
{
	m_bFading = false;

	if(Time > 0)
		m_bFading = true;

	m_FadeDir = Dir;

	if(m_FadeDir == -1)
		m_FadeAlpha = 255.0f;
	else
		m_FadeAlpha = 0.0f;

	m_CurrentFadeTime = CCD->FreeRunningCounter_F();
	m_FadeTime = Time;
}

/* ------------------------------------------------------------------------------------ */
// DoFade
/* ------------------------------------------------------------------------------------ */
void CRFMenu::DoFade()
{
	float FadeDelta, VolDelta;
	float DeltaTime = (CCD->FreeRunningCounter_F() - m_CurrentFadeTime) * 0.001f;

	if(m_FadeDir == -1)
	{
		FadeDelta = 255.0f - (255.0f * (DeltaTime / m_FadeTime));

		if(m_bMusicFade)
		{
			VolDelta = m_OldMVol * (DeltaTime / m_FadeTime);

			if(VolDelta > m_OldMVol)
				VolDelta = m_OldMVol;
		}
	}
	else
	{
		FadeDelta = 255.0f * (DeltaTime / m_FadeTime);

		if(m_bMusicFade)
		{
			VolDelta = m_OldMVol - (m_OldMVol * (DeltaTime / m_FadeTime));

			if(VolDelta < 0)
				VolDelta = 0;
		}
	}

	if(m_bFading)
	{
		m_FadeAlpha = FadeDelta;

		if(m_FadeAlpha >= 0.0f)
		{
			GE_Rect Rect;
			GE_RGBA	Color;
			Rect.Left = Rect.Top = 0;
			Rect.Right = CCD->Engine()->Width() - 1;
			Rect.Bottom = CCD->Engine()->Height() - 1;

			Color.r = 0.0f;
			Color.g = 0.0f;
			Color.b = 0.0f;
			Color.a = m_FadeAlpha;

			if(Color.a < 0.0f)
				Color.a = 0.0f;
			else if(Color.a > 255.0f)
				Color.a = 255.0f;

			geEngine_FillRect(CCD->Engine()->Engine(), &Rect, &Color);
		}

		if(m_bMusicFade)
		{
			SetmMusicVol(VolDelta);
		}

		if(m_FadeDir == -1 && m_FadeAlpha < 0.0f)
		{
			m_bFading = false;
		}
		else if(m_FadeDir == 1 && m_FadeAlpha > 255.0f)
		{
			m_bFading = false;
		}

		if(!m_bFading && m_bMusicFade)
		{
			m_bMusicFade = false;
		}

		if(m_FadeDir == 1 && !m_bFading)
		{
			// remove all children
			while(CMenuState::GetSingletonPtr()->GetDefaultWindow()->getChildCount() > 0)
			{
				CMenuState::GetSingletonPtr()->GetDefaultWindow()->removeChildWindow(CMenuState::GetSingletonPtr()->GetDefaultWindow()->getChildAtIdx(0));
			}
			// add active section
			CMenuState::GetSingletonPtr()->GetDefaultWindow()->addChildWindow(m_SectionStack.top());
			FadeSet(-1, m_TimeFade);
		}
	}
}

/* ------------------------------------------------------------------------------------ */
// SaveTo
//
// Save the current state of the menu off to an open file.
/* ------------------------------------------------------------------------------------ */
int CRFMenu::SaveTo(FILE *SaveFD, bool type)
{
	WRITEDATA(type, &m_bUseSelect,			sizeof(bool),	1, SaveFD);
	WRITEDATA(type, &m_SelectedCharacter,	sizeof(int),	1, SaveFD);
	WRITEDATA(type, &m_bUseNameSelect,		sizeof(bool),	1, SaveFD);

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// RestoreFrom
//
// Restore the state of the menu from an open file.
/* ------------------------------------------------------------------------------------ */
int CRFMenu::RestoreFrom(FILE *RestoreFD, bool type)
{
	READDATA(type, &m_bUseSelect,			sizeof(bool),	1, RestoreFD);
	READDATA(type, &m_SelectedCharacter,	sizeof(int),	1, RestoreFD);
	READDATA(type, &m_bUseNameSelect,		sizeof(bool),	1, RestoreFD);

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// ChangeMenuIni
/* ------------------------------------------------------------------------------------ */
void CRFMenu::ChangeConfiguration(const std::string& filename)
{
	// stop music
	if(m_MusicType != -1)
	{
		if(m_MusicType == 1)
		{
			MIDIPlayer()->Stop();
		}
		else
		{
			if(m_Streams)
			{
				m_Streams->Delete();
				SAFE_DELETE(m_Streams);
			}
		}
	}

	// free click sounds
	if(m_MouseClick)
	{
		geSound_FreeSoundDef(CCD->Engine()->AudioSystem(), m_MouseClick);
		m_MouseClick = NULL;
	}
	if(m_KeyClick)
	{
		geSound_FreeSoundDef(CCD->Engine()->AudioSystem(), m_KeyClick);
		m_KeyClick = NULL;
	}
	if(m_SlideClick)
	{
		geSound_FreeSoundDef(CCD->Engine()->AudioSystem(), m_SlideClick);
		m_SlideClick = NULL;
	}

	if(m_FCrosshairs != NULL)
	{
		geEngine_RemoveBitmap(CCD->Engine()->Engine(), m_FCrosshairs);
		geBitmap_Destroy(&m_FCrosshairs);
		m_FCrosshairs = NULL;
	}

	SaveSettings();

	Reset();

	// load new menu configuration file
	LoadConfiguration(filename);
	// reload text / load new text
	LoadText(CCD->LanguageManager()->GetActiveLanguage()->GetMenuTextFilename());

	InitializeWidgets();

	m_EventHandler->SubscribeEvents();

	FillActionMapList();

	m_SectionStack.push(m_Sections["options"]);
	m_SectionStack.push(m_Sections["language"]);
	while(CMenuState::GetSingletonPtr()->GetDefaultWindow()->getChildCount() > 0)
	{
		CMenuState::GetSingletonPtr()->GetDefaultWindow()->removeChildWindow(CMenuState::GetSingletonPtr()->GetDefaultWindow()->getChildAtIdx(0));
	}
	CMenuState::GetSingletonPtr()->GetDefaultWindow()->addChildWindow(m_SectionStack.top());

	if(m_MusicType != -1)
	{
		if(m_MusicType == 1)
			MIDIPlayer()->Play(m_Music.c_str(), true);
		else
			m_Streams->Play(true);
	}
}


/* ------------------------------------------------------------------------------------ */
// Reset
//
// reset menu to default values
/* ------------------------------------------------------------------------------------ */
void CRFMenu::Reset()
{
	//m_LoadFont = 0;
	//m_SaveFont = 0;

	m_EventHandler->RemoveAllEvents();

	// remove all children
	while(CMenuState::GetSingletonPtr()->GetDefaultWindow()->getChildCount() > 0)
	{
		CMenuState::GetSingletonPtr()->GetDefaultWindow()->removeChildWindow(CMenuState::GetSingletonPtr()->GetDefaultWindow()->getChildAtIdx(0));
	}

	// destroy all windows
	stdext::hash_map<std::string, CEGUI::Window*>::iterator iter = m_Sections.begin();
	for(; iter!=m_Sections.end(); ++iter)
	{
		CCD->GUIManager()->DestroyWindow(iter->second);
	}
	CCD->GUIManager()->CleanDeadWindowPool();

	m_Sections.clear();
	while(!m_SectionStack.empty())
		m_SectionStack.pop();

	CCD->Input()->ResetActionNames();
	CCD->Input()->ResetMouseButtonNames();
	m_KeyNameUnassigned = "<Unassigned>";
}


int CRFMenu::Name2ActionID(const std::string& name)
{
	if(name == "noaction")		return RGF_K_NOACTION;
	if(name == "moveforward")	return RGF_K_FORWARD;
	if(name == "movebackward")	return RGF_K_BACKWARD;
	if(name == "turnleft")		return RGF_K_TURN_LEFT;
	if(name == "turnright")		return RGF_K_TURN_RIGHT;
	if(name == "strafeleft")	return RGF_K_LEFT;
	if(name == "straferight")	return RGF_K_RIGHT;
	if(name == "jump")			return RGF_K_JUMP;
	if(name == "crouch")		return RGF_K_CROUCH;
	if(name == "lookup")		return RGF_K_LOOKUP;
	if(name == "lookdown")		return RGF_K_LOOKDOWN;
	if(name == "lookstraight")	return RGF_K_LOOKSTRAIGHT;
	if(name == "attack")		return RGF_K_FIRE;
	if(name == "altattack")		return RGF_K_ALTFIRE;
	if(name == "weapon0")		return RGF_K_WEAPON_0;
	if(name == "weapon1")		return RGF_K_WEAPON_1;
	if(name == "weapon2")		return RGF_K_WEAPON_2;
	if(name == "weapon3")		return RGF_K_WEAPON_3;
	if(name == "weapon4")		return RGF_K_WEAPON_4;
	if(name == "weapon5")		return RGF_K_WEAPON_5;
	if(name == "weapon6")		return RGF_K_WEAPON_6;
	if(name == "weapon7")		return RGF_K_WEAPON_7;
	if(name == "weapon8")		return RGF_K_WEAPON_8;
	if(name == "weapon9")		return RGF_K_WEAPON_9;
	if(name == "run")			return RGF_K_RUN;
	if(name == "cameramode")	return RGF_K_CAMERA;
	if(name == "zoomin")		return RGF_K_ZOOM_IN;
	if(name == "zoomout")		return RGF_K_ZOOM_OUT;
	if(name == "camerareset")	return RGF_K_CAMERA_RESET;
	if(name == "quicksave")		return RGF_K_QUICKSAVE;
	if(name == "quickload")		return RGF_K_QUICKLOAD;
	if(name == "1stperson")		return RGF_K_FIRST_PERSON_VIEW;
	if(name == "3rdperson")		return RGF_K_THIRD_PERSON_VIEW;
	if(name == "isometric")		return RGF_K_ISO_VIEW;
	if(name == "skip")			return RGF_K_SKIP;
	if(name == "help")			return RGF_K_HELP;
	if(name == "hud")			return RGF_K_HUD;
	if(name == "lookmode")		return RGF_K_LOOKMODE;
	if(name == "screenshot")	return RGF_K_SCRNSHOT;
	if(name == "zoomweapon")	return RGF_K_ZOOM_WEAPON;
	if(name == "holsterweapon")	return RGF_K_HOLSTER_WEAPON;
	if(name == "lightonoff")	return RGF_K_LIGHT;
	if(name == "useitem")		return RGF_K_USE;
	if(name == "inventory")		return RGF_K_INVENTORY;
	if(name == "console")		return RGF_K_CONSOLE;
	if(name == "dropweapon")	return RGF_K_DROP;
	if(name == "reload")		return RGF_K_RELOAD;
	if(name == "powerup")		return RGF_K_POWERUP;
	if(name == "powerdown")		return RGF_K_POWERDWN;

	return -1;
}

/* ----------------------------------- END OF FILE ------------------------------------ */
