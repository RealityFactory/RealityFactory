/************************************************************************************//**
 * @file CLanguage.h
 * @brief Language class
 *
 * This file contains the declaration of the Language class.
 * @author Daniel Queteschiner
 * @date August 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_LANGUAGE_H_
#define __RGF_LANGUAGE_H_


class CLanguage
{
public:
	explicit CLanguage(const std::string& name);
	~CLanguage();

	void SetName(const std::string& name) { m_Name = name; }
	const std::string& GetName() const { return m_Name; }

	// menu
	void SetMenuConfigFilename(const std::string& filename) { m_MenuConfigFilename = filename; }
	const std::string& GetMenuConfigFilename() const { return m_MenuConfigFilename; }

	void SetMenuTextFilename(const std::string& filename) { m_MenuTextFilename = filename; }
	const std::string& GetMenuTextFilename() const { return m_MenuTextFilename; }

	// conversation
	void SetConversationConfigFilename(const std::string& filename) { m_ConversationConfigFilename = filename; }
	const std::string& GetConversationConfigFilename() const { return m_ConversationConfigFilename; }

	void SetConversationTextFilename(const std::string& filename) { m_ConversationTextFilename = filename; }
	const std::string& GetConversationTextFilename() const { return m_ConversationTextFilename; }

	void SetDubbingDirectory(const std::string& directory) { m_DubbingDirectory = directory; }
	const std::string& GetDubbingDirectory() const { return m_DubbingDirectory; }

	// message
	void SetMessageConfigFilename(const std::string& filename) { m_MessageConfigFilename = filename; }
	const std::string& GetMessageConfigFilename() const { return m_MessageConfigFilename; }

	void SetMessageTextFilename(const std::string& filename) { m_MessageTextFilename = filename; }
	const std::string& GetMessageTextFilename() const { return m_MessageTextFilename; }

	// inventory
	void SetInventoryConfigFilename(const std::string& filename) { m_InventoryConfigFilename = filename; }
	const std::string& GetInventoryConfigFilename() const { return m_InventoryConfigFilename; }

	void SetInventoryTextFilename(const std::string& filename) { m_InventoryTextFilename = filename; }
	const std::string& GetInventoryTextFilename() const { return m_InventoryTextFilename; }

	int LoadConfiguration(CIniFile& iniFile);

private:

	std::string m_Name;
	std::string m_MenuConfigFilename;
	std::string m_MenuTextFilename;
	std::string m_ConversationConfigFilename;
	std::string m_ConversationTextFilename;
	std::string m_DubbingDirectory;
	std::string m_MessageConfigFilename;
	std::string m_MessageTextFilename;
	std::string m_InventoryConfigFilename;
	std::string m_InventoryTextFilename;
};

#endif
