/************************************************************************************//**
 * @file CInventory.cpp
 * @brief Inventory class implementation
 *
 * This file contains the class implementation for Inventory handling.
 * @author Daniel Queteschiner
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "RabidFramework.h"
#include "CFileManager.h"
#include "IniFile.h"
#include "CPlayState.h"
#include "CDialogState.h"
#include "CGUIManager.h"
#include "CLanguageManager.h"
#include "CScriptManager.h"
#include "CInventoryItem.h"
#include "CInventory.h"
#include "RFSX/CEGUI/sxCEGUIWindow.h"

class sxInventoryItemIterator : public skExecutableIterator
{
public:
	sxInventoryItemIterator()
		:m_Iter(sxInventory::GetSingletonPtr()->m_Items.begin())
	{}

	virtual ~sxInventoryItemIterator() {}

	virtual bool next(skRValue& value)
	{
		if(m_Iter == sxInventory::GetSingletonPtr()->m_Items.end())
			return false;

		value.assignObject(m_Iter->second);
		++m_Iter;

		return true;
	}

private:
	stdext::hash_map<std::string, sxInventoryItem*>::iterator m_Iter;
};



#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxInventory

SX_IMPL_TYPE* SX_IMPL_TYPE::m_Inventory = NULL;

SX_IMPL_TYPE::SX_IMPL_TYPE() :
	m_ShowAlways(false),
	m_SlotManagment(false)
{
}


SX_IMPL_TYPE::~SX_IMPL_TYPE()
{
	ClearItems();
}


void SX_IMPL_TYPE::Initialize()
{
	LoadConfiguration(CCD->LanguageManager()->GetActiveLanguage()->GetInventoryConfigFilename());
	LoadText(CCD->LanguageManager()->GetActiveLanguage()->GetInventoryTextFilename());
}


void SX_IMPL_TYPE::Show()
{
	if(m_Sections.find("main") != m_Sections.end())
	{
		m_Sections["main"]->show();
	}
}


void SX_IMPL_TYPE::Hide()
{
	if(m_ShowAlways)
		return;

	stdext::hash_map<std::string, CEGUI::Window*>::iterator iter = m_Sections.begin();
	for(; iter!=m_Sections.end(); ++iter)
	{
		iter->second->hide();
	}
}


bool SX_IMPL_TYPE::ContainsItem(const std::string& item)
{
	if(m_Items.find(item) != m_Items.end())
		return true;

	return false;

}


bool SX_IMPL_TYPE::ContainsItem(CEGUI::Window* window)
{
	if(m_ItemWindows.find(window) != m_ItemWindows.end())
		return true;

	return false;
}


sxInventoryItem* SX_IMPL_TYPE::GetItem(const std::string& item)
{
	if(m_Items.find(item) != m_Items.end())
		return m_Items[item];

	return NULL;
}


sxInventoryItem* SX_IMPL_TYPE::GetItem(CEGUI::Window* window)
{
	if(m_ItemWindows.find(window) != m_ItemWindows.end())
		return m_ItemWindows[window];

	return NULL;
}


const std::string& SX_IMPL_TYPE::GetText(const std::string& name)
{
	if(m_Text.find(name) != m_Text.end())
		return m_Text[name];

	static std::string empty;
	return empty;
}


void SX_IMPL_TYPE::UpdateItem(const std::string& item, bool valueOnly)
{
	if(m_Items.find(item) != m_Items.end())
		m_Items[item]->Update(valueOnly);
}


void SX_IMPL_TYPE::Update()
{
	{
		// update items
		stdext::hash_map<std::string, sxInventoryItem*>::iterator iter = m_Items.begin();
		for(; iter!=m_Items.end(); ++iter)
		{
			iter->second->Update();
		}
	}

	{
		// update windows
		stdext::hash_map<std::string, std::string>::iterator iter = m_Text.begin();
		for(; iter!=m_Text.end(); ++iter)
		{
			CCD->GUIManager()->SetWindowText(iter->first, iter->second);
		}
	}
}


void SX_IMPL_TYPE::LoadText(const std::string& inventorytxt)
{
	geVFile *MainFS;
	char szInputLine[256];
	std::string readinfo, keyname, text;

	m_Text.clear();

	if(!CFileManager::GetSingletonPtr()->OpenRFFile(&MainFS, kInstallFile, inventorytxt.c_str(), GE_VFILE_OPEN_READONLY))
		return;

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
	}

	geVFile_Close(MainFS);

	Update();
}


bool SX_IMPL_TYPE::LoadConfiguration(const std::string& filename)
{
	CIniFile iniFile(filename);

	if(!iniFile.ReadFile())
	{
		sxLog::GetSingletonPtr()->Warning("Failed to open " + filename + " file.");
		return false;
	}

	ClearItems();

	// destroy all windows
	stdext::hash_map<std::string, CEGUI::Window*>::iterator iter = m_Sections.begin();
	for(; iter!=m_Sections.end(); ++iter)
	{
		CCD->GUIManager()->DestroyWindow(iter->second);
	}
	CCD->GUIManager()->CleanDeadWindowPool();

	m_Sections.clear();


	std::string keyName = iniFile.FindFirstKey();
	std::string value;

	// First create items (without loading their configuration) so we can operate on
	// them in the init ('main') function of the script, e.g. add an item combination
	while(!keyName.empty())
	{
		if(	keyName != "General" &&
			keyName != "Schemes" &&
			keyName != "Layouts")
		{
			if(m_Items.find(keyName) != m_Items.end())
			{
				CCD->Log()->Warning("Redefinition of inventory item [" + keyName + "] in " + filename + " file.");
				keyName = iniFile.FindNextKey();
				continue;
			}

			sxInventoryItem *item = new sxInventoryItem(keyName);
			if(item)
			{
				m_Items[keyName] = item;
			}
		}

		keyName = iniFile.FindNextKey();
	}

	// General
	{
		m_GameState = iniFile.GetValue("General", "gamestate");

		if(m_GameState != "dialog")
		{
			value = iniFile.GetValue("General", "showalways");
			if(value == "true")
				m_ShowAlways = true;
		}

		value = iniFile.GetValue("General", "script");

		if(value.empty())
		{
			sxLog::GetSingletonPtr()->Warning("No script file specified for inventory in " + filename + " file.");
			return false;
		}
		else
		{
			CEGUI::System::getSingleton().executeScriptFile(value.c_str());
		}

		value = iniFile.GetValue("General", "slotmanagement");
		if(value == "true")
		{
			m_SlotManagment = true;
		}
		else
		{
			m_SlotManagment = false;
		}
	}

	// Check if main screen is defined
	{
		if(iniFile.GetValue("Layouts", "main").empty())
		{
			sxLog::GetSingletonPtr()->Error("No main inventory screen defined in " + filename + " file");
			return false;
		}
	}

	// Schemes
	{
		std::string name = iniFile.FindFirstName("Schemes");

		while(!name.empty())
		{
			value = iniFile.GetValue("Schemes", name);
			CCD->GUIManager()->LoadScheme(value);

			name = iniFile.FindNextName();
		}
	}

	keyName = iniFile.FindFirstKey();
	while(!keyName.empty())
	{
		if(keyName == "General")
		{
		}
		else if(keyName == "Schemes")
		{
		}
		else if(keyName == "Layouts")
		{
			std::string name = iniFile.FindFirstName(keyName);

			while(!name.empty())
			{
				if(m_Sections.find(name) == m_Sections.end())
				{
					value = iniFile.GetValue(keyName, name);

					if(!value.empty())
					{
						sxLog::GetSingletonPtr()->Debug("Load inventory layout: " + name + " " + value);
						CEGUI::Window *win = CCD->GUIManager()->LoadWindowLayout(value);
						m_Sections[name] = win;

						if(m_GameState == "dialog")
						{
							CDialogState::GetSingletonPtr()->GetDefaultWindow()->addChildWindow(m_Sections[name]);
						}
						else
						{
							CPlayState::GetSingletonPtr()->GetDefaultWindow()->addChildWindow(m_Sections[name]);
						}

						m_Sections[name]->hide();
					}
				}

				name = iniFile.FindNextName();
			}
		}

		keyName = iniFile.FindNextKey();
	}

	// Last but not least load the item configurations
	stdext::hash_map<std::string, sxInventoryItem*>::iterator itemIter = m_Items.begin();

	for(; itemIter!=m_Items.end(); ++itemIter)
	{
		if(itemIter->second->LoadConfiguration(iniFile) != RGF_SUCCESS)
		{
			SAFE_DELETE(itemIter->second);
			// hash_map.erase return an iterator that designates the first element remaining beyond any elements removed,
			// or a pointer to the end of the hash_map if no such element exists.
			itemIter = m_Items.erase(itemIter);
		}
		else
		{
			m_ItemWindows[itemIter->second->GetWindow()] = itemIter->second;
		}
	}

	Update();

	if(m_ShowAlways)
		Show();

	return true;
}


void SX_IMPL_TYPE::ClearItems()
{
	stdext::hash_map<std::string, sxInventoryItem*>::iterator iter = m_Items.begin();

	for(; iter!=m_Items.end(); ++iter)
	{
		SAFE_DELETE(iter->second);
	}

	m_Items.clear();
	m_ItemWindows.clear();
}


SX_IMPL_TYPE* SX_IMPL_TYPE::GetSingletonPtr()
{
	if(!m_Inventory)
	{
		m_Inventory = new sxInventory();
#ifdef _DEBUG
		InitMHT();
#endif
		ScriptManager::AddGlobalVariable("Inventory", m_Inventory);
	}

	return m_Inventory;
}


bool SX_IMPL_TYPE::method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
{
	if(h_method(this, m_name, args, r_val, ctxt))
		return true;
	else
		return skExecutable::method(m_name, args, r_val, ctxt);
}


skExecutableIterator* SX_IMPL_TYPE::createIterator()
{
	return new sxInventoryItemIterator();
}


SX_METHOD_IMPL(ContainsItem)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() == skRValue::T_String)
	{
		r_val = SX_IMPL_TYPE::GetSingletonPtr()->ContainsItem(args[0].str().c_str());
	}
	else
	{
		RFSX::sxWindow *win = RFSX::IS_WINDOW(args[0]);
		if(!win)
			return false;

		r_val = SX_IMPL_TYPE::GetSingletonPtr()->ContainsItem(win->Window());
	}

	return true;
}


SX_METHOD_IMPL(GetItem)
{
	if(args.entries() != 1)
		return false;

	sxInventoryItem *item = NULL;

	if(args[0].type() == skRValue::T_String)
	{
		item = SX_IMPL_TYPE::GetSingletonPtr()->GetItem(args[0].str().c_str());
	}
	else
	{
		RFSX::sxWindow *win = RFSX::IS_WINDOW(args[0]);
		if(!win)
			return false;

		item = SX_IMPL_TYPE::GetSingletonPtr()->GetItem(win->Window());
	}

	if(item)
		r_val.assignObject(item);
	else
		r_val.assignObject(&ctxt.getInterpreter()->getNull());

	return true;
}


SX_METHOD_IMPL(AddItemCombination)
{
	return true;
}


SX_METHOD_IMPL(RemoveItemCombination)
{
	return true;
}


SX_METHOD_IMPL(GetText)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	r_val = skString(SX_IMPL_TYPE::GetSingletonPtr()->GetText(args[0].str().c_str()).c_str());
	return true;
}


RFSX::SX_INIT_MHT(SXINVENTORY_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(ContainsItem);
	SX_ADD_METHOD(GetItem);
	SX_ADD_METHOD(AddItemCombination);
	SX_ADD_METHOD(RemoveItemCombination);
	SX_ADD_METHOD(GetText);
}

#undef SX_IMPL_TYPE

/* ----------------------------------- END OF FILE ------------------------------------ */
