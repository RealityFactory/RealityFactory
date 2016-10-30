/************************************************************************************//**
 * @file CInventory.h
 * @brief Inventory class handler
 *
 * This file contains the class declaration for Inventory handling.
 * @author Daniel Queteschiner
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_INVENTORY_H_
#define __RGF_INVENTORY_H_

#include "RFSX\\RFSX.h"

class sxInventoryItem;

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxInventory

#define SXINVENTORY_METHODS 5

class SX_IMPL_TYPE : public skExecutable
{
	friend class sxInventoryItemIterator;
public:
	~SX_IMPL_TYPE();
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);
	virtual skExecutableIterator* createIterator(const skString& /*qualifier*/) { return createIterator(); }
	virtual skExecutableIterator* createIterator();
	virtual int executableType() const { return RFSXG_INVENTORY; }

	void Initialize();

	void Show();
	void Hide();

	bool GetSlotManagement() const { return m_SlotManagment; }

	bool ContainsItem(const std::string& item);
	bool ContainsItem(CEGUI::Window* window);

	sxInventoryItem* GetItem(const std::string& item);
	sxInventoryItem* GetItem(CEGUI::Window* window);

	const std::string& GetText(const std::string& name);

	void UpdateItem(const std::string& item, bool valueOnly = false);

	void Update();

	void LoadText(const std::string& inventorytxt);

	bool LoadConfiguration(const std::string& filename);

	static SX_IMPL_TYPE* GetSingletonPtr();

protected:
	// expose to simkin
	SX_METHOD_DECL(ContainsItem);
	SX_METHOD_DECL(GetItem);
	SX_METHOD_DECL(AddItemCombination);
	SX_METHOD_DECL(RemoveItemCombination);
	SX_METHOD_DECL(GetText);

private:
	SX_IMPL_TYPE();
	SX_IMPL_TYPE(const SX_IMPL_TYPE&) { }
	SX_IMPL_TYPE& operator = (const SX_IMPL_TYPE&);

	void ClearItems();

	bool		m_ShowAlways;
	bool		m_SlotManagment;
	std::string m_GameState;

	stdext::hash_map<std::string, std::string>			m_Text;
	stdext::hash_map<std::string, sxInventoryItem*>		m_Items;
	stdext::hash_map<CEGUI::Window*, sxInventoryItem*>	m_ItemWindows;
	stdext::hash_map<std::string, CEGUI::Window*>		m_Sections;

private:
	static RFSX::sxMHT<SX_IMPL_TYPE> h_method;

	static SX_IMPL_TYPE*	m_Inventory;
};

#undef SX_IMPL_TYPE

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
