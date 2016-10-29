/************************************************************************************//**
 * @file sxCEGUIListbox.h
 * @brief Class declaration for RFSX Listbox object
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_LISTBOX_H_
#define _SX_LISTBOX_H_

#include "RFSX.h"
#include "../sxCEGUIWindow.h"

// sxListbox class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		int getItemCount();
		int getSelectedCount();
		sxListboxItem getFirstSelectedItem();	-- may return null!
		sxListboxItem getNextSelected(sxListboxItem start_item=null);	-- may return null!
		sxListboxItem getListboxItemFromIndex(int index);
		int getItemIndex(sxListboxItem item);
		sxListboxItem findItemWithText(skString text, sxListboxItem start_item=null);	-- may return null!
		bool isSortEnabled();
		bool isMultiselectEnabled();
		bool isItemSelected(int index);
		bool isListboxItemInList(sxListboxItem item);
		bool isVertScrollbarAlwaysShown();
		bool isHorzScrollbarAlwaysShown();

		void resetList();
		void addItem(sxListboxItem item);
		void insertItem(sxListboxItem item, sxListboxItem position=null);
		void removeItem(sxListboxItem item);
		void clearAllSelections();
		void setSortingEnabled(bool setting);
		void setMultiselectEnabled(bool setting);
		void setShowVertScrollbar(bool setting);
		void setShowHorzScrollbar(bool setting);
		void setItemSelectState(sxListboxItem item, bool state);
		void setItemSelectState(int item_index, bool state);
		void handleUpdatedItemData();
		void ensureItemIsVisible(int item_index);
		void ensureItemIsVisible(sxListboxItem item);

		sxRect getListRenderArea();
		sxScrollbar getVertScrollbar();
		sxScrollbar getHorzScrollbar();
		float getWidestItemWidth();
		float getTotalItemsHeight();
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxListbox

#define SXLISTBOX_METHODS 30

namespace RFSX
{

class SX_IMPL_TYPE: public sxWindow
{
public:
	SX_IMPL_TYPE(CEGUI::Listbox* listbox) : sxWindow(listbox) {}
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);

	CEGUI::Listbox* Listbox() { return static_cast<CEGUI::Listbox*>(Window()); }

protected:
	SX_METHOD_DECL(getItemCount);
	SX_METHOD_DECL(getSelectedCount);
	SX_METHOD_DECL(getFirstSelectedItem);
	SX_METHOD_DECL(getNextSelected);
	SX_METHOD_DECL(getListboxItemFromIndex);
	SX_METHOD_DECL(getItemIndex);
	SX_METHOD_DECL(findItemWithText);
	SX_METHOD_DECL(isSortEnabled);
	SX_METHOD_DECL(isMultiselectEnabled);
	SX_METHOD_DECL(isItemSelected);
	SX_METHOD_DECL(isListboxItemInList);
	SX_METHOD_DECL(isVertScrollbarAlwaysShown);
	SX_METHOD_DECL(isHorzScrollbarAlwaysShown);
	SX_METHOD_DECL(resetList);
	SX_METHOD_DECL(addItem);
	SX_METHOD_DECL(insertItem);
	SX_METHOD_DECL(removeItem);
	SX_METHOD_DECL(clearAllSelections);
	SX_METHOD_DECL(setSortingEnabled);
	SX_METHOD_DECL(setMultiselectEnabled);
	SX_METHOD_DECL(setShowVertScrollbar);
	SX_METHOD_DECL(setShowHorzScrollbar);
	SX_METHOD_DECL(setItemSelectState);
	SX_METHOD_DECL(handleUpdatedItemData);
	SX_METHOD_DECL(ensureItemIsVisible);
	SX_METHOD_DECL(getListRenderArea);
	SX_METHOD_DECL(getVertScrollbar);
	SX_METHOD_DECL(getHorzScrollbar);
	SX_METHOD_DECL(getWidestItemWidth);
	SX_METHOD_DECL(getTotalItemsHeight);

private:
	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_LISTBOX_H_
