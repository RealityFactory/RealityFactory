/************************************************************************************//**
 * @file sxCEGUIListbox.cpp
 * @brief Implementation for RFSX Listbox object.
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUIListbox.h"
#include "sxCEGUIListboxItem.h"
#include "sxCEGUIScrollbar.h"
#include "../sxCEGUIRect.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxListbox

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxListbox implementation
//////////////////////////////////////////////////////////////////////////////////////////

bool SX_IMPL_TYPE::method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
{
	if(h_method(this, m_name, args, r_val, ctxt))
		return true;
	else
		return sxWindow::method(m_name, args, r_val, ctxt);
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxListbox method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(getItemCount)
{
	//unsigned long getItemCount() const;
	r_val = static_cast<int>(caller->Listbox()->getItemCount());
	return true;
}


SX_METHOD_IMPL(getSelectedCount)
{
	//unsigned long getSelectedCount() const;
	r_val = static_cast<int>(caller->Listbox()->getSelectedCount());
	return true;
}


SX_METHOD_IMPL(getFirstSelectedItem)
{
	//ListboxItem* getFirstSelectedItem() const;
	CEGUI::ListboxItem *item = caller->Listbox()->getFirstSelectedItem();

	if(item)
		r_val.assignObject(new sxListboxItem(item), true);
	else
		r_val.assignObject(&ctxt.getInterpreter()->getNull());

	return true;
}


SX_METHOD_IMPL(getNextSelected)
{
	skRValue::RType rtypes[] = { skRValue::T_Object };

	if(!CheckParams(0, 1, args, rtypes))
		return false;

	CEGUI::ListboxItem *start_item = NULL;

	if(args.entries() == 1)
	{
		sxListboxItem *item = IS_LISTBOXITEM(args[0]);

		if(!item)
			return false;

		start_item = item->ListboxItem();
	}

	//ListboxItem* getNextSelected(const ListboxItem* start_item) const;
	CEGUI::ListboxItem *item = caller->Listbox()->getNextSelected(start_item);

	if(item)
		r_val.assignObject(new sxListboxItem(item), true);
	else
		r_val.assignObject(&ctxt.getInterpreter()->getNull());

	return true;
}


SX_METHOD_IMPL(getListboxItemFromIndex)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return true;

	//ListboxItem* getListboxItemFromIndex(unsigned long index) const;
	r_val.assignObject(new sxListboxItem(caller->Listbox()->getListboxItemFromIndex(args[0].intValue())), true);
	return true;
}


SX_METHOD_IMPL(getItemIndex)
{
	if(args.entries() != 1)
		return false;

	sxListboxItem *item = IS_LISTBOXITEM(args[0]);
	if(!item)
		return false;

	//unsigned long getItemIndex(const ListboxItem* item) const;
	r_val = static_cast<int>(caller->Listbox()->getItemIndex(item->ListboxItem()));
	return true;
}


SX_METHOD_IMPL(findItemWithText)
{
	skRValue::RType rtypes[] = { skRValue::T_String, skRValue::T_Object };

	if(!CheckParams(1, 2, args, rtypes))
		return false;

	CEGUI::String text(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	CEGUI::ListboxItem *start_item = NULL;

	if(args.entries() == 2)
	{
		if(args[1].obj() != &ctxt.getInterpreter()->getNull())
		{
			sxListboxItem *item = IS_LISTBOXITEM(args[1]);

			if(!item)
				return false;

			start_item = item->ListboxItem();
		}
	}

	//ListboxItem* findItemWithText(string text, const ListboxItem* start_item);
	CEGUI::ListboxItem *item = caller->Listbox()->findItemWithText(text, start_item);

	if(item)
		r_val.assignObject(new sxListboxItem(item), true);
	else
		r_val.assignObject(&ctxt.getInterpreter()->getNull());

	return true;
}


SX_METHOD_IMPL(isSortEnabled)
{
	//bool isSortEnabled() const;
	r_val = caller->Listbox()->isSortEnabled();
	return true;
}


SX_METHOD_IMPL(isMultiselectEnabled)
{
	//bool isMultiselectEnabled() const;
	r_val = caller->Listbox()->isMultiselectEnabled();
	return true;
}


SX_METHOD_IMPL(isItemSelected)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	//bool isItemSelected(unsigned long index) const;
	r_val = caller->Listbox()->isItemSelected(args[0].intValue());
	return true;
}


SX_METHOD_IMPL(isListboxItemInList)
{
	if(args.entries() != 1)
		return false;

	sxListboxItem *item = IS_LISTBOXITEM(args[0]);
	if(!item)
		return false;

	//bool isListboxItemInList(const ListboxItem* item) const;
	r_val = caller->Listbox()->isListboxItemInList(item->ListboxItem());
	return true;
}


SX_METHOD_IMPL(isVertScrollbarAlwaysShown)
{
	//bool isVertScrollbarAlwaysShown() const;
	r_val = caller->Listbox()->isVertScrollbarAlwaysShown();
	return true;
}


SX_METHOD_IMPL(isHorzScrollbarAlwaysShown)
{
	//bool isHorzScrollbarAlwaysShown() const;
	r_val = caller->Listbox()->isHorzScrollbarAlwaysShown();
	return true;
}


SX_METHOD_IMPL(resetList)
{
	//void resetList(void);
	caller->Listbox()->resetList();
	return true;
}


SX_METHOD_IMPL(addItem)
{
	if(args.entries() != 1)
		return false;

	if(args[0].obj() == &ctxt.getInterpreter()->getNull())
		return true;

	sxListboxItem *item = IS_LISTBOXITEM(args[0]);
	if(!item)
		return false;

	//void addItem(ListboxItem* item);
	caller->Listbox()->addItem(item->ListboxItem());
	return true;
}


SX_METHOD_IMPL(insertItem)
{
	skRValue::RType rtypes[] = { skRValue::T_Object, skRValue::T_Object };

	if(!CheckParams(1, 2, args, rtypes))
		return false;

	if(args[0].obj() == &ctxt.getInterpreter()->getNull())
		return true;

	sxListboxItem *item = IS_LISTBOXITEM(args[0]);
	if(!item)
		return false;

	CEGUI::ListboxItem *position = NULL;

	if(args.entries() == 2)
	{
		if(args[1].obj() != &ctxt.getInterpreter()->getNull())
		{
			sxListboxItem *pos = IS_LISTBOXITEM(args[1]);

			if(!pos)
				return false;

			position = pos->ListboxItem();
		}
	}

	//void insertItem(ListboxItem* item, const ListboxItem* position);
	caller->Listbox()->insertItem(item->ListboxItem(), position);
	return true;
}


SX_METHOD_IMPL(removeItem)
{
	if(args.entries() != 1)
		return false;

	sxListboxItem *item = IS_LISTBOXITEM(args[0]);
	if(!item)
		return false;

	//void removeItem(const ListboxItem* item);
	caller->Listbox()->removeItem(item->ListboxItem());
	return true;
}


SX_METHOD_IMPL(clearAllSelections)
{
	//void clearAllSelections(void);
	caller->Listbox()->clearAllSelections();
	return true;
}


SX_METHOD_IMPL(setSortingEnabled)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	//void setSortingEnabled(bool setting);
	caller->Listbox()->setSortingEnabled(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(setMultiselectEnabled)
{
	//void setMultiselectEnabled(bool setting);
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	//void setMultiselectEnabled(bool setting);
	caller->Listbox()->setMultiselectEnabled(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(setShowVertScrollbar)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	//void setShowVertScrollbar(bool setting);
	caller->Listbox()->setShowVertScrollbar(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(setShowHorzScrollbar)
{
	//void setShowHorzScrollbar(bool setting);
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	//void setShowHorzScrollbar(bool setting);
	caller->Listbox()->setShowHorzScrollbar(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(setItemSelectState)
{
	if(args.entries() != 2)
		return false;

	if(args[1].type() != skRValue::T_Bool)
		return false;

	//void setItemSelectState(unsigned long item_index, bool state);
	if(args[0].type() == skRValue::T_Int)
	{
		caller->Listbox()->setItemSelectState(args[0].intValue(), args[1].boolValue());
		return true;
	}

	//void setItemSelectState(ListboxItem* item, bool state);
	sxListboxItem *item = IS_LISTBOXITEM(args[0]);
	if(!item)
		return false;

	caller->Listbox()->setItemSelectState(item->ListboxItem(), args[1].boolValue());
	return true;
}


SX_METHOD_IMPL(handleUpdatedItemData)
{
	//void handleUpdatedItemData(void);
	caller->Listbox()->handleUpdatedItemData();
	return true;
}


SX_METHOD_IMPL(ensureItemIsVisible)
{
	if(args.entries() != 1)
		return false;

	//void ensureItemIsVisible(unsigned long item_index);
	if(args[0].type() == skRValue::T_Int)
	{
		caller->Listbox()->ensureItemIsVisible(args[0].intValue());
		return true;
	}

	//void ensureItemIsVisible(const ListboxItem* item);
	sxListboxItem *item = IS_LISTBOXITEM(args[0]);
	if(!item)
		return false;

	caller->Listbox()->ensureItemIsVisible(item->ListboxItem());
	return true;
}


SX_METHOD_IMPL(getListRenderArea)
{
	//Rect getListRenderArea() const;
	r_val.assignObject(new sxRect(caller->Listbox()->getListRenderArea()), true);
	return true;
}


SX_METHOD_IMPL(getVertScrollbar)
{
	//Scrollbar* getVertScrollbar() const;
	r_val.assignObject(new sxScrollbar(caller->Listbox()->getVertScrollbar()), true);
	return true;
}


SX_METHOD_IMPL(getHorzScrollbar)
{
	//Scrollbar* getHorzScrollbar() const;
	r_val.assignObject(new sxScrollbar(caller->Listbox()->getHorzScrollbar()), true);
	return true;
}


SX_METHOD_IMPL(getWidestItemWidth)
{
	//float getWidestItemWidth() const;
	r_val = caller->Listbox()->getWidestItemWidth();
	return true;
}


SX_METHOD_IMPL(getTotalItemsHeight)
{
	//float getTotalItemsHeight() const;
	r_val = caller->Listbox()->getTotalItemsHeight();
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxListbox initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXLISTBOX_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(getItemCount);
	SX_ADD_METHOD(getSelectedCount);
	SX_ADD_METHOD(getFirstSelectedItem);
	SX_ADD_METHOD(getNextSelected);
	SX_ADD_METHOD(getListboxItemFromIndex);
	SX_ADD_METHOD(getItemIndex);
	SX_ADD_METHOD(findItemWithText);
	SX_ADD_METHOD(isSortEnabled);
	SX_ADD_METHOD(isMultiselectEnabled);
	SX_ADD_METHOD(isItemSelected);
	SX_ADD_METHOD(isListboxItemInList);
	SX_ADD_METHOD(isVertScrollbarAlwaysShown);
	SX_ADD_METHOD(isHorzScrollbarAlwaysShown);
	SX_ADD_METHOD(resetList);
	SX_ADD_METHOD(addItem);
	SX_ADD_METHOD(insertItem);
	SX_ADD_METHOD(removeItem);
	SX_ADD_METHOD(clearAllSelections);
	SX_ADD_METHOD(setSortingEnabled);
	SX_ADD_METHOD(setMultiselectEnabled);
	SX_ADD_METHOD(setShowVertScrollbar);
	SX_ADD_METHOD(setShowHorzScrollbar);
	SX_ADD_METHOD(setItemSelectState);
	SX_ADD_METHOD(handleUpdatedItemData);
	SX_ADD_METHOD(ensureItemIsVisible);
	SX_ADD_METHOD(getListRenderArea);
	SX_ADD_METHOD(getVertScrollbar);
	SX_ADD_METHOD(getHorzScrollbar);
	SX_ADD_METHOD(getWidestItemWidth);
	SX_ADD_METHOD(getTotalItemsHeight);
}

} // namespace RFSX

#undef SX_IMPL_TYPE
