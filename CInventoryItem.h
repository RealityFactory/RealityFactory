/************************************************************************************//**
 * @file CInventoryItem.h
 * @brief InventoryItem class
 *
 * This file contains the declaration of the InventoryItem class.
 * @author Daniel Queteschiner
 * @date March 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_INVENTORYITEM_H_
#define __RGF_INVENTORYITEM_H_

#include "RFSX\\RFSX.h"

class ItemCombination;

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxInventoryItem

#define SXINVENTORYITEM_METHODS 13

// Type-check and upcast macro
#define IS_INVENTORYITEM(o)	 RFSX::CheckType<sxInventoryItem>(o, RFSXU_INVENTORYITEM)

class SX_IMPL_TYPE : public skExecutable
{
public:
	explicit SX_IMPL_TYPE(const std::string& attribute);
	~SX_IMPL_TYPE();

	/**
	 * @brief Get the name of the item / the attribute the item represents.
	 */
	const std::string& GetName() const { return m_Attribute; }

	/**
	 * @brief Set the category group the item belongs to.
	 * @param catagory Name of the category group the item belongs to.
	 */
	void SetCategory(const std::string& category) { m_Category = category; }

	/**
	 * @brief Get the name of the category group the item belongs to.
	 */
	const std::string& GetCategory() const { return m_Category; }

	/**
	 * @brief Get window representing the item.
	 */
	CEGUI::Window* GetWindow() { return m_Window; }

	/**
	 * @brief Get window holding the item's description.
	 */
	CEGUI::Window* GetTextWindow() { return m_TextWindow; }

	/**
	 * @brief Get window holding the item's value.
	 */
	CEGUI::Window* GetValueWindow() { return m_ValueWindow; }

	/**
	 * @brief Get description of the item.
	 */
	const std::string& GetText() const;

	/**
	 * @brief Get description of the item.
	 */
	const std::string& GetTooltipText() const;

	/**
	 * @brief Select the item for use with the Use key.
	 * @param decrease Amount that the item will be decreased when used.
	 */
	void SelectForUse(int decrease = 1);

	/**
	 * @brief Deselect the item from use with the Use key.
	 */
	void DeselectFromUse();

	/**
	 * @brief Modify a different attribute.
	 * @param attribute Attribute to be modified.
	 * @param amount Amount that the attribute will be modifies.
	 * @param decrease Amount that the item will be decreased.
	 */
	void Modify(const std::string& attribute, int amount, int decrease = 1);

	/**
	 * @brief Try to combine the item with a different item.
	 * @param other Item to combine the item with.
	 * @param increaseCombination Amount that the item resulting from the combination will be intcreased.
	 * @param decrease Amount that the item will be decreased.
	 * @param decreaseOther Amount that the other item will be decreased.
	 * @return Returns 0 if items are not combineable, 1 on successful combination,
	 *			-1 if amount of item is too low, -2 if amount of other item is too low.
	 */
	int Combine(const std::string& other);
	int Combine(const std::string& other, const std::string& result, int increaseResult = 1, int decrease = 1, int decreaseOther = 1);

	bool HasCombination(const std::string& other);
	int AddCombination(const std::string& other, const std::string& result, int increaseResult = 1, int decrease = 1, int decreaseOther = 1);
	int RemoveCombination(const std::string& other);

	/**
	 * @brief Update the window(s) representing the item.
	 * @param valueOnly If true, only the value window and show/hide state will be updated
	 */
	void Update(bool valueOnly = false);

	/**
	 * @brief Load item definition from configuration file.
	 */
	int LoadConfiguration(CIniFile& iniFile);

public:
	static void InitMHT();
	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);
	virtual int executableType() const { return RFSXU_INVENTORYITEM; }

protected:
	SX_METHOD_DECL(GetName);
	SX_METHOD_DECL(SetCategory);
	SX_METHOD_DECL(GetCategory);
	SX_METHOD_DECL(GetWindow);
	SX_METHOD_DECL(GetTextWindow);
	SX_METHOD_DECL(GetValueWindow);
	SX_METHOD_DECL(GetText);
	SX_METHOD_DECL(SelectForUse);
	SX_METHOD_DECL(DeselectFromUse);
	SX_METHOD_DECL(Modify);
	SX_METHOD_DECL(Combine);
	SX_METHOD_DECL(AddCombination);
	SX_METHOD_DECL(RemoveCombination);

private:

	std::string		m_Attribute;
	std::string		m_Category;
	std::string		m_TooltipSection;
	std::string		m_ParentWindowBaseName;

	CEGUI::Window*	m_ParentWindow;

	CEGUI::Window*	m_Window;
	CEGUI::Window*	m_TextWindow;
	CEGUI::Window*	m_ValueWindow;

	stdext::hash_map<std::string, ItemCombination*> m_Combinations;

	int				m_SelectDecrease;

	static RFSX::sxMHT<SX_IMPL_TYPE> h_method;
};

#undef SX_IMPL_TYPE

#endif // __RGF_INVENTORYITEM_H_
