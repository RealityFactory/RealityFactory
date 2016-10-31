/************************************************************************************//**
 * @file CMessage.cpp
 * @brief Message class implementation
 *
 * This file contains the class implementation for Message handling.
 * @author Ralph Deane
 * @author Daniel Queteschiner
 *//*
 * Copyright (c) 2001 Ralph Deane; All Rights Reserved.
 ****************************************************************************************/

// Include the One True Header
#include "RabidFramework.h"
#include <Ram.h>
#include "CFileManager.h"
#include "IniFile.h"
#include "CGameStateManager.h"
#include "CPlayState.h"
#include "CDialogState.h"
#include "CGUIManager.h"
#include "CLanguageManager.h"
#include "CMessage.h"

enum
{
	FADE_NONE = 0,
	FADE_IN,
	FADE_OUT
};


class MessageType
{
public:
	MessageType()
		: m_Scripted(false), m_TimeElapsed(0.f), m_DisplayTime(0.f),
		m_FadeInTime(0.f), m_FadeOutTime(0.f), m_FadeTime(0.f), m_FadeDir(FADE_NONE),
		m_Alpha(1.f), m_AlphaStep(0.f), m_Window(NULL), m_TextWindow(NULL)
	{ }

public:
	bool		m_Scripted; // modal or modeless
	float		m_TimeElapsed;
	float		m_DisplayTime;
	float		m_FadeInTime;
	float		m_FadeOutTime;
	float		m_FadeTime;
	int			m_FadeDir;
	float		m_Alpha;
	float		m_AlphaStep;
	CEGUI::Window* m_Window;
	CEGUI::Window* m_TextWindow;
};

/* ------------------------------------------------------------------------------------ */
// Constructor
/* ------------------------------------------------------------------------------------ */
CMessage::CMessage() :
	m_Active(false)
{
	// Ok, check to see if there are Messages in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "Message");

	if(!pSet)
		return;

	if(!LoadConfiguration(CCD->LanguageManager()->GetActiveLanguage()->GetMessageConfigFilename()))
		return;

	LoadText(CCD->LanguageManager()->GetActiveLanguage()->GetMessageTextFilename());

	m_Active = true;

	geEntity *pEntity;

	// Ok, we have Messages somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Message *pSource = static_cast<Message*>(geEntity_GetUserData(pEntity));

		if(EffectC_IsStringNull(pSource->szEntityName))
		{
			char szName[128];
			geEntity_GetName(pEntity, szName, 128);
			pSource->szEntityName = szName;
		}

		// Ok, put this entity into the Global Entity Registry
		CCD->EntityRegistry()->AddEntity(pSource->szEntityName, "Message");
		pSource->Time = 0.0f;

		if(m_MessageTypes.find(pSource->DisplayType) == m_MessageTypes.end())
		{
			CCD->Log()->Warning("Display Type %s of Message entity %s not defined",
				pSource->DisplayType, pSource->szEntityName);
		}
	}
}

/* ------------------------------------------------------------------------------------ */
// Destructor
/* ------------------------------------------------------------------------------------ */
CMessage::~CMessage()
{
	ClearMessageTypes();
}


void CMessage::ClearMessageTypes()
{
	stdext::hash_map<std::string, MessageType*>::iterator iter = m_MessageTypes.begin();

	for(; iter!=m_MessageTypes.end(); ++iter)
	{
		CCD->GUIManager()->DestroyWindow(iter->second->m_Window);
		delete (iter->second);
	}

	m_MessageTypes.clear();

	CCD->GUIManager()->CleanDeadWindowPool();
}


bool CMessage::LoadConfiguration(const std::string& filename)
{
	CIniFile iniFile(filename);

	if(!iniFile.ReadFile())
	{
		CCD->Log()->Warning("Failed to open " + filename + " file.");
		return false;
	}

	ClearMessageTypes();

	std::string keyName = iniFile.FindFirstKey();
	std::string value;

	while(!keyName.empty())
	{
		if(m_MessageTypes.find(keyName) != m_MessageTypes.end())
		{
			CCD->Log()->Warning("Redefinition of Message Type [" + keyName + "] in " + filename + " file.");
			keyName = iniFile.FindNextKey();
			continue;
		}

		value = iniFile.GetValue(keyName, "layout");

		if(value.empty())
		{
			CCD->Log()->Warning("No layout file defined for Message Type [" + keyName + "]");
			keyName = iniFile.FindNextKey();
			continue;
		}

		m_MessageTypes[keyName] = new MessageType();

		m_MessageTypes[keyName]->m_Window = CCD->GUIManager()->LoadWindowLayout(value);

		value = iniFile.GetValue(keyName, "textwindow");
		m_MessageTypes[keyName]->m_TextWindow = CCD->GUIManager()->GetWindow(value);

		m_MessageTypes[keyName]->m_DisplayTime = static_cast<float>(iniFile.GetValueF(keyName, "displaytime"));
		m_MessageTypes[keyName]->m_FadeInTime = static_cast<float>(iniFile.GetValueF(keyName, "fadeintime"));
		m_MessageTypes[keyName]->m_FadeOutTime = static_cast<float>(iniFile.GetValueF(keyName, "fadeouttime"));

		m_MessageTypes[keyName]->m_Window->hide();

		{
			value = iniFile.GetValue(keyName, "script");
			if(!value.empty())
			{
				m_MessageTypes[keyName]->m_Scripted = true;
				CEGUI::System::getSingleton().executeScriptFile(value.c_str());
			}
		}

		if(iniFile.GetValue(keyName, "gamestate") == "dialog")
		{
			CDialogState::GetSingletonPtr()->GetDefaultWindow()->addChildWindow(m_MessageTypes[keyName]->m_Window);
		}
		else
		{
			CPlayState::GetSingletonPtr()->GetDefaultWindow()->addChildWindow(m_MessageTypes[keyName]->m_Window);
		}

		keyName = iniFile.FindNextKey();
	}

	return true;
}

/* ------------------------------------------------------------------------------------ */
// Tick
/* ------------------------------------------------------------------------------------ */
void CMessage::Tick(float timeElapsed)
{
	// Ok, check to see if there are Messages in this world
	if(!m_Active)
		return;

	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "Message");

	if(!pSet)
		return;

	geEntity *pEntity;

	// Ok, we have Messages somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Message *pSource = static_cast<Message*>(geEntity_GetUserData(pEntity));
		pSource->Time -= timeElapsed * 0.001f;

		if(pSource->Time <= 0.0f && !EffectC_IsStringNull(pSource->TriggerName))
		{
			if(GetTriggerState(pSource->TriggerName))
			{
				std::string type = pSource->DisplayType;

				if(m_MessageTypes.find(type) != m_MessageTypes.end())
				{
					pSource->Time = m_MessageTypes[type]->m_DisplayTime;
					if(pSource->Time <= 0.1f) pSource->Time = 1.f;

					if(m_MessageTypes[type]->m_Window->isVisible())
					{
						if(m_MessageTypes[type]->m_Scripted)
							continue;

						if((m_MessageTypes[type]->m_DisplayTime - m_MessageTypes[type]->m_TimeElapsed) <= m_MessageTypes[type]->m_FadeOutTime)
						{
							m_MessageTypes[type]->m_FadeTime = m_MessageTypes[type]->m_FadeInTime;
							m_MessageTypes[type]->m_FadeDir = FADE_NONE;
							m_MessageTypes[type]->m_TimeElapsed = m_MessageTypes[type]->m_Alpha * m_MessageTypes[type]->m_FadeInTime;

							if(m_MessageTypes[type]->m_FadeTime > 0.0f)
							{
								m_MessageTypes[type]->m_FadeDir = FADE_IN;
								m_MessageTypes[type]->m_AlphaStep = 1.0f / m_MessageTypes[type]->m_FadeInTime;
							}
						}
						else if(m_MessageTypes[type]->m_TimeElapsed > m_MessageTypes[type]->m_FadeInTime)
						{
							m_MessageTypes[type]->m_FadeTime = m_MessageTypes[type]->m_FadeOutTime;
							m_MessageTypes[type]->m_TimeElapsed = m_MessageTypes[type]->m_FadeInTime;
							m_MessageTypes[type]->m_FadeDir = FADE_NONE;

							if(m_MessageTypes[type]->m_FadeTime > 0.0f)
							{
								m_MessageTypes[type]->m_FadeDir = FADE_OUT;
								m_MessageTypes[type]->m_AlphaStep = 1.0f / m_MessageTypes[type]->m_FadeTime;
							}
						}
					}
					else
					{
						m_MessageTypes[type]->m_TextWindow->setText(GetText(pSource->TextName));

						m_MessageTypes[type]->m_TimeElapsed = 0.0f;

						m_MessageTypes[type]->m_FadeDir = FADE_NONE;
						m_MessageTypes[type]->m_Alpha = 1.0f;

						if(!m_MessageTypes[type]->m_Scripted)
						{
							m_MessageTypes[type]->m_FadeTime = m_MessageTypes[type]->m_FadeInTime;

							if(m_MessageTypes[type]->m_FadeTime > 0.0f)
							{
								m_MessageTypes[type]->m_FadeDir = FADE_IN;
								m_MessageTypes[type]->m_Alpha = 0.0f;
								m_MessageTypes[type]->m_AlphaStep = 1.0f / m_MessageTypes[type]->m_FadeTime;
							}
							else
							{
								m_MessageTypes[type]->m_FadeTime = m_MessageTypes[type]->m_FadeOutTime;

								if(m_MessageTypes[type]->m_FadeTime > 0.0f)
								{
									m_MessageTypes[type]->m_FadeDir = FADE_OUT;
									m_MessageTypes[type]->m_AlphaStep = 1.0f / m_MessageTypes[type]->m_FadeTime;
								}
							}
						}

						m_MessageTypes[type]->m_Window->show();
					}

					m_MessageTypes[type]->m_Window->setAlpha(m_MessageTypes[type]->m_Alpha);
					m_MessageTypes[type]->m_TextWindow->setText(GetText(pSource->TextName));
				}
			}
		}
	}

	stdext::hash_map<std::string, MessageType*>::iterator iter = m_MessageTypes.begin();
	for(; iter!=m_MessageTypes.end(); ++iter)
	{
		if(iter->second->m_Scripted)
			continue;

		if(iter->second->m_Window->isVisible())
		{
			iter->second->m_TimeElapsed += timeElapsed * 0.001f;

			if((iter->second->m_TimeElapsed) >= iter->second->m_DisplayTime)
			{
				iter->second->m_Window->hide();
			}
			else
			{
				// fade in/out
				switch(iter->second->m_FadeDir)
				{
				case FADE_IN:
					{
						if(iter->second->m_TimeElapsed <= iter->second->m_FadeTime)
						{
							iter->second->m_Alpha += iter->second->m_AlphaStep * timeElapsed * 0.001f;

							if(iter->second->m_Alpha > 1.0f)
								iter->second->m_Alpha = 1.0f;

							iter->second->m_Window->setAlpha(iter->second->m_Alpha);
						}
						else
						{
							iter->second->m_FadeTime = iter->second->m_FadeOutTime;
							iter->second->m_FadeDir = FADE_NONE;
							iter->second->m_Alpha = 1.0f;
							iter->second->m_Window->setAlpha(1.0f);

							if(iter->second->m_FadeTime > 0.0f)
							{
								iter->second->m_FadeDir = FADE_OUT;
								iter->second->m_Alpha = 1.0f;
								iter->second->m_AlphaStep = 1.0f/iter->second->m_FadeTime;
							}
						}
						break;
					}
				case FADE_OUT:
					{
						if((iter->second->m_DisplayTime - iter->second->m_TimeElapsed) <= iter->second->m_FadeTime)
						{
							iter->second->m_Alpha -= iter->second->m_AlphaStep * timeElapsed * 0.001f;

							if(iter->second->m_Alpha < 0.0f)
							{
								iter->second->m_Alpha = 0.0f;
								iter->second->m_Window->hide();
							}

							iter->second->m_Window->setAlpha(iter->second->m_Alpha);
						}
						break;
					}
				}
			}
		}
	}
}

//	******************** CRGF Overrides ********************

/* ------------------------------------------------------------------------------------ */
// LocateEntity
//
// Given a name, locate the desired item in the currently loaded level
// ..and return it's user data.
/* ------------------------------------------------------------------------------------ */
int CMessage::LocateEntity(const char *szName, void **pEntityData)
{
	// Ok, check to see if there are Messages in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "Message");

	if(!pSet)
		return RGF_NOT_FOUND;									// No Messages

	geEntity *pEntity;

	// Ok, we have Messages somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Message *pSource = static_cast<Message*>(geEntity_GetUserData(pEntity));

		if(!strcmp(pSource->szEntityName, szName))
		{
			*pEntityData = static_cast<void*>(pSource);
			return RGF_SUCCESS;
		}
	}

	return RGF_NOT_FOUND;								// Sorry, no such entity here
}

/* ------------------------------------------------------------------------------------ */
// ReSynchronize
//
// Correct internal timing to match current time, to make up for time lost
// ..when outside the game loop (typically in "menu mode").
/* ------------------------------------------------------------------------------------ */
int CMessage::ReSynchronize()
{
	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// LoadText
/* ------------------------------------------------------------------------------------ */
void CMessage::LoadText(const std::string& messagetxt)
{
	geVFile *MainFS;
	m_Text.clear();

	if(!CFileManager::GetSingletonPtr()->OpenRFFile(&MainFS, kInstallFile, messagetxt.c_str(), GE_VFILE_OPEN_READONLY))
		return;

	char szInputLine[256];
	std::string readinfo, keyname, text;

	while(geVFile_GetS(MainFS, szInputLine, 256) == GE_TRUE)
	{
		if(strlen(szInputLine) <= 1)
			readinfo.clear();
		else
			readinfo = szInputLine;

		if(!readinfo.empty() && readinfo[0] != ';')
		{
			TrimRight(readinfo);

			// if trim operation results in empty string...
			if(readinfo.length() <= 1)
				continue;

			if(readinfo[0] == '[' && readinfo[readinfo.length()-1] == ']')
			{
				if(!keyname.empty() && !text.empty())
				{
					m_Text[keyname] = text;
					Replace(m_Text[keyname], "<Player>", CCD->Player()->GetPlayerName());
				}

				keyname = readinfo;
				TrimLeft(keyname, "[");
				TrimRight(keyname, "]");
				text.clear();
			}
			else
			{
				if(readinfo == "<CR>")
				{
					text += '\n';
				}
				else
				{
					if(!text.empty() && text[text.length()-1] != '\n')
						text += " ";

					text += readinfo;
				}
			}
		}
	}

	if(!keyname.empty() && !text.empty())
	{
		m_Text[keyname] = text;
		Replace(m_Text[keyname], "<Player>", CCD->Player()->GetPlayerName());
	}

	geVFile_Close(MainFS);
}

/* ------------------------------------------------------------------------------------ */
// GetText
/* ------------------------------------------------------------------------------------ */
const utf8* CMessage::GetText(const std::string& name)
{
	if(m_Text.find(name) != m_Text.end())
		return reinterpret_cast<const utf8*>(m_Text[name].c_str());

	return NULL;
}


/* ----------------------------------- END OF FILE ------------------------------------ */
