/************************************************************************************//**
 * @file sxCEGUIItemListbox.h
 * @brief Class declaration for RFSX ItemListbox object
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_ITEMLISTBOX_H_
#define _SX_ITEMLISTBOX_H_

#include "RFSX.h"
#include "sxCEGUIScrolledItemListBase.h"

// sxItemListbox class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		unsigned long getSelectedCount() const;
		ItemEntry* getLastSelectedItem() const;	-- may return null!
		ItemEntry* getFirstSelectedItem(unsigned long start_index=0) const;	-- may return null!
		ItemEntry* getNextSelectedItem() const;	-- may return null!
		ItemEntry* getNextSelectedItemAfter(const ItemEntry* start_item) const;	-- may return null!
		bool isMultiSelectEnabled() const;
		bool isItemSelected(unsigned long index) const;
		void setMultiSelectEnabled(bool state);
		void clearAllSelections();
		void selectRange(unsigned long a, unsigned long z);
		void selectAllItems();
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxItemListbox

#define SXITEMLISTBOX_METHODS 11

namespace RFSX
{

class SX_IMPL_TYPE: public sxScrolledItemListBase
{
public:
	SX_IMPL_TYPE(CEGUI::ItemListbox* listbox) : sxScrolledItemListBase(listbox) {}
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);

	CEGUI::ItemListbox* ItemListbox()	{ return static_cast<CEGUI::ItemListbox*>(ScrolledItemListBase()); }

protected:
	SX_METHOD_DECL(getSelectedCount);
	SX_METHOD_DECL(getLastSelectedItem);
	SX_METHOD_DECL(getFirstSelectedItem);
	SX_METHOD_DECL(getNextSelectedItem);
	SX_METHOD_DECL(getNextSelectedItemAfter);
	SX_METHOD_DECL(isMultiSelectEnabled);
	SX_METHOD_DECL(isItemSelected);
	SX_METHOD_DECL(setMultiSelectEnabled);
	SX_METHOD_DECL(clearAllSelections);
	SX_METHOD_DECL(selectRange);
	SX_METHOD_DECL(selectAllItems);

private:
	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_ITEMLISTBOX_H_
