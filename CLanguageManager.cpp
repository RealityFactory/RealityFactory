/************************************************************************************//**
 * @file CLanguageManager.cpp
 * @brief Language Manager class
 *
 * This file contains the implementation of the Language Manager class.
 * @author Daniel Queteschiner
 * @date August 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "RabidFramework.h"
#include "IniFile.h"
#include "CLanguageManager.h"
#include "CLevel.h"
#include "CMessage.h"
#include "CInventory.h"
#include "CConversationManager.h"


CLanguageManager::CLanguageManager(const std::string& defaultLanguage) :
	m_ActiveLanguage(NULL),
	m_NumLanguages(0),
	m_DefaultLanguageName(defaultLanguage)
{
	LoadConfiguration();
}


CLanguageManager::~CLanguageManager()
{
	std::vector<CLanguage*>::iterator iter = m_Languages.begin();
	for(; iter!=m_Languages.end(); ++iter)
	{
		delete (*iter);
	}
	m_Languages.clear();
}


CLanguage* CLanguageManager::GetFirstLanguage()
{
	m_NumLanguages = 0;

	if(m_NumLanguages < m_Languages.size())
		return m_Languages[m_NumLanguages];
	else
		return NULL;
}


CLanguage* CLanguageManager::GetNextLanguage()
{
	++m_NumLanguages;

	if(m_NumLanguages < m_Languages.size())
		return m_Languages[m_NumLanguages];
	else
		return NULL;
}


void CLanguageManager::SetActiveLanguage(const std::string& name)
{
	std::vector<CLanguage*>::iterator iter = m_Languages.begin();
	for(; iter!=m_Languages.end(); ++iter)
	{
		if((*iter)->GetName() == name)
		{
			ChangeLanguage(*iter);
			return;
		}
	}
}


void CLanguageManager::ChangeLanguage(CLanguage *newLanguage)
{
	if(newLanguage == m_ActiveLanguage)
		return;

	CLanguage *oldLanguage = m_ActiveLanguage;
	m_ActiveLanguage = newLanguage;

	// menu
	if(newLanguage->GetMenuConfigFilename() != oldLanguage->GetMenuConfigFilename())
		CCD->MenuManager()->ChangeConfiguration(newLanguage->GetMenuConfigFilename());
	else if(newLanguage->GetMenuTextFilename() != oldLanguage->GetMenuTextFilename())
		CCD->MenuManager()->LoadText(newLanguage->GetMenuTextFilename());

	// conversation
	if(newLanguage->GetConversationConfigFilename() != oldLanguage->GetConversationConfigFilename())
		sxConversationManager::GetSingletonPtr()->LoadConfiguration(newLanguage->GetConversationConfigFilename());

	if(newLanguage->GetConversationTextFilename() != oldLanguage->GetConversationTextFilename())
		sxConversationManager::GetSingletonPtr()->LoadText(newLanguage->GetConversationTextFilename());

	// message
	if(newLanguage->GetMessageConfigFilename() != oldLanguage->GetMessageConfigFilename())
		if(CCD->MenuManager()->GetInGame())
			CCD->Level()->Messages()->LoadConfiguration(newLanguage->GetMessageConfigFilename());

	if(newLanguage->GetMessageTextFilename() != oldLanguage->GetMessageTextFilename())
		if(CCD->MenuManager()->GetInGame())
			CCD->Level()->Messages()->LoadText(newLanguage->GetMessageTextFilename());

	// inventory
	if(newLanguage->GetInventoryConfigFilename() != oldLanguage->GetInventoryConfigFilename())
		if(CCD->MenuManager()->GetInGame())
			sxInventory::GetSingletonPtr()->LoadConfiguration(newLanguage->GetInventoryConfigFilename());

	if(newLanguage->GetInventoryTextFilename() != oldLanguage->GetInventoryTextFilename())
		if(CCD->MenuManager()->GetInGame())
			sxInventory::GetSingletonPtr()->LoadText(newLanguage->GetInventoryTextFilename());
}


void CLanguageManager::LoadConfiguration()
{
	CIniFile iniFile("language.ini");

	if(!iniFile.ReadFile())
	{
		CCD->Log()->Debug("Failed to open language.ini file!");
	}
	else
	{
		std::string section = iniFile.FindFirstKey();

		while(!section.empty())
		{
			CLanguage *language = new CLanguage(section);

			if(language->LoadConfiguration(iniFile) == RGF_SUCCESS)
			{
				m_Languages.push_back(language);
				if(section == m_DefaultLanguageName)
					m_ActiveLanguage = language;
			}
			else
			{
				delete language;
			}

			section = iniFile.FindNextKey();
		}
	}

	if(m_Languages.empty())
	{
		CLanguage *language = new CLanguage(m_DefaultLanguageName);
		m_Languages.push_back(language);
	}

	if(m_ActiveLanguage == NULL)
		m_ActiveLanguage = m_Languages[0];
}

