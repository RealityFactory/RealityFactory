/************************************************************************************//**
 * @file CLanguage.cpp
 * @brief Language class
 *
 * This file contains the implementation of the Language class.
 * @author Daniel Queteschiner
 * @date August 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include <string>
#include "RGFStatus.h"
#include "IniFile.h"
#include "CLanguage.h"


CLanguage::CLanguage(const std::string& name) :
	m_Name(name),
	m_MenuConfigFilename("menu.ini"),
	m_MenuTextFilename("menu.txt"),
	m_ConversationConfigFilename("conversation.ini"),
	m_ConversationTextFilename("conversation.txt"),
	m_DubbingDirectory("media\\audio"),
	m_MessageConfigFilename("message.ini"),
	m_MessageTextFilename("message.txt"),
	m_InventoryConfigFilename("inventory.ini"),
	m_InventoryTextFilename("inventory.txt")
{
}


CLanguage::~CLanguage()
{
}


int CLanguage::LoadConfiguration(CIniFile& iniFile)
{
	if(m_Name.empty())
		return RGF_FAILURE;

	std::string value;

	value = iniFile.GetValue(m_Name, "menu");
	if(!value.empty())
		m_MenuConfigFilename = value;

	value = iniFile.GetValue(m_Name, "menutext");
	if(!value.empty())
		m_MenuTextFilename = value;

	value = iniFile.GetValue(m_Name, "conversation");
	if(!value.empty())
		m_ConversationConfigFilename = value;

	value = iniFile.GetValue(m_Name, "conversationtext");
	if(!value.empty())
		m_ConversationTextFilename = value;

	value = iniFile.GetValue(m_Name, "dubbingdirectory");
	if(!value.empty())
		m_DubbingDirectory = value;

	value = iniFile.GetValue(m_Name, "message");
	if(!value.empty())
		m_MessageConfigFilename = value;

	value = iniFile.GetValue(m_Name, "messagetext");
	if(!value.empty())
		m_MessageTextFilename = value;

	value = iniFile.GetValue(m_Name, "inventory");
	if(!value.empty())
		m_InventoryConfigFilename = value;

	value = iniFile.GetValue(m_Name, "inventorytext");
	if(!value.empty())
		m_InventoryTextFilename = value;

	return RGF_SUCCESS;
}
