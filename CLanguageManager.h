/************************************************************************************//**
 * @file CLanguageManager.h
 * @brief Language Manager class
 *
 * This file contains the declaration of the Language Manager class.
 * @author Daniel Queteschiner
 * @date August 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_LANGUAGEMANAGER_H_
#define __RGF_LANGUAGEMANAGER_H_

#include "CLanguage.h"

class CLanguageManager
{
public:
	explicit CLanguageManager(const std::string& defaultLanguage);
	~CLanguageManager();

	void SetActiveLanguage(const std::string& name);
	CLanguage* GetActiveLanguage() { return m_ActiveLanguage; }

	CLanguage* GetFirstLanguage();
	CLanguage* GetNextLanguage();

private:
	void LoadConfiguration();
	void ChangeLanguage(CLanguage *newLanguage);

private:
	std::vector<CLanguage*> m_Languages;
	CLanguage* m_ActiveLanguage;
	unsigned int m_NumLanguages;
	std::string m_DefaultLanguageName;
};

#endif
