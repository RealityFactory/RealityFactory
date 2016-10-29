/************************************************************************************//**
 * @file sxCEGUICombobox.cpp
 * @brief Implementation for RFSX Combobox object.
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUICombobox.h"
#include "sxCEGUIEditbox.h"
#include "sxCEGUIPushButton.h"
#include "sxCEGUIComboDropList.h"
#include "sxCEGUIListboxItem.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxCombobox

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxCombobox implementation
//////////////////////////////////////////////////////////////////////////////////////////

bool SX_IMPL_TYPE::method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
{
	if(h_method(this, m_name, args, r_val, ctxt))
		return true;
	else
		return sxWindow::method(m_name, args, r_val, ctxt);
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxCombobox method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(getSingleClickEnabled)
{
	//bool getSingleClickEnabled() const;
	r_val = caller->Combobox()->getSingleClickEnabled();
	return true;
}


SX_METHOD_IMPL(isDropDownListVisible)
{
	//bool isDropDownListVisible() const;
	r_val = caller->Combobox()->isDropDownListVisible();
	return true;
}


SX_METHOD_IMPL(getEditbox)
{
	//Editbox* getEditbox() const;
	r_val.assignObject(new sxEditbox(caller->Combobox()->getEditbox()), true);
	return true;
}

SX_METHOD_IMPL(getPushButton)
{
	//PushButton* getPushButton() const;
	r_val.assignObject(new sxPushButton(caller->Combobox()->getPushButton()), true);
	return true;
}


SX_METHOD_IMPL(getDropList)
{
	//ComboDropList* getDropList() const;
	r_val.assignObject(new sxComboDropList(caller->Combobox()->getDropList()), true);
	return true;
}


SX_METHOD_IMPL(hasInputFocus)
{
	//bool hasInputFocus() const;
	r_val = caller->Combobox()->hasInputFocus();
	return true;
}


SX_METHOD_IMPL(isReadOnly)
{
	//bool isReadOnly() const;
	r_val = caller->Combobox()->isReadOnly();
	return true;
}


SX_METHOD_IMPL(isTextValid)
{
	//bool isTextValid() const;
	r_val = caller->Combobox()->isTextValid();
	return true;
}


SX_METHOD_IMPL(getValidationString)
{
	//string getValidationString() const;
	r_val = skString(caller->Combobox()->getValidationString().c_str());
	return true;
}


SX_METHOD_IMPL(getCaratIndex)
{
	//unsigned long getCaratIndex() const;
	r_val = static_cast<int>(caller->Combobox()->getCaratIndex());
	return true;
}


SX_METHOD_IMPL(getSelectionStartIndex)
{
	//unsigned long getSelectionStartIndex() const;
	r_val = static_cast<int>(caller->Combobox()->getSelectionStartIndex());
	return true;
}


SX_METHOD_IMPL(getSelectionEndIndex)
{
	//unsigned long getSelectionEndIndex() const;
	r_val = static_cast<int>(caller->Combobox()->getSelectionEndIndex());
	return true;
}



SX_METHOD_IMPL(getSelectionLength)
{
	//unsigned long getSelectionLength() const;
	r_val = static_cast<int>(caller->Combobox()->getSelectionLength());
	return true;
}


SX_METHOD_IMPL(getMaxTextLength)
{
	//unsigned long getMaxTextLength() const;
	r_val = static_cast<int>(caller->Combobox()->getMaxTextLength());
	return true;
}


SX_METHOD_IMPL(activateEditbox)
{
	//void activateEditbox();
	caller->Combobox()->activateEditbox();
	return true;
}


SX_METHOD_IMPL(getItemCount)
{
	//unsigned long getItemCount() const;
	r_val = static_cast<int>(caller->Combobox()->getItemCount());
	return true;
}


SX_METHOD_IMPL(getSelectedItem)
{
	CEGUI::ListboxItem *item = caller->Combobox()->getSelectedItem();
	//ListboxItem* getSelectedItem() const;

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
		return false;

	//ListboxItem* getListboxItemFromIndex(unsigned long index) const;
	r_val.assignObject(new sxListboxItem(caller->Combobox()->getListboxItemFromIndex(args[0].intValue())), true);
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
	r_val = static_cast<int>(caller->Combobox()->getItemIndex(item->ListboxItem()));
	return true;
}


SX_METHOD_IMPL(isItemSelected)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	//bool isItemSelected(unsigned long index) const;
	r_val = caller->Combobox()->isItemSelected(args[0].intValue());
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
	CEGUI::ListboxItem *item = caller->Combobox()->findItemWithText(text, start_item);

	if(item)
		r_val.assignObject(new sxListboxItem(item), true);
	else
		r_val.assignObject(&ctxt.getInterpreter()->getNull());

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
	r_val = caller->Combobox()->isListboxItemInList(item->ListboxItem());
	return true;
}


SX_METHOD_IMPL(resetList)
{
	//void resetList();
	caller->Combobox()->resetList();
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
	caller->Combobox()->addItem(item->ListboxItem());
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
	caller->Combobox()->insertItem(item->ListboxItem(), position);
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
	caller->Combobox()->removeItem(item->ListboxItem());
	return true;
}


SX_METHOD_IMPL(clearAllSelections)
{
	//void clearAllSelections();
	caller->Combobox()->clearAllSelections();
	return true;
}


SX_METHOD_IMPL(setSortingEnabled)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	//void setSortingEnabled(bool setting);
	caller->Combobox()->setSortingEnabled(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(setShowVertScrollbar)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	//void setShowVertScrollbar(bool setting);
	caller->Combobox()->setShowVertScrollbar(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(setShowHorzScrollbar)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	//void setShowHorzScrollbar(bool setting);
	caller->Combobox()->setShowHorzScrollbar(args[0].boolValue());
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
		caller->Combobox()->setItemSelectState(args[0].intValue(), args[1].boolValue());
		return true;
	}

	//void setItemSelectState(ListboxItem* item, bool state);
	sxListboxItem *item = IS_LISTBOXITEM(args[0]);
	if(!item)
		return false;

	caller->Combobox()->setItemSelectState(item->ListboxItem(), args[1].boolValue());
	return true;
}


SX_METHOD_IMPL(handleUpdatedListItemData)
{
	//void handleUpdatedListItemData();
	caller->Combobox()->handleUpdatedListItemData();
	return true;
}


SX_METHOD_IMPL(isSortEnabled)
{
	//bool isSortEnabled() const;
	r_val = caller->Combobox()->isSortEnabled();
	return true;
}


SX_METHOD_IMPL(isVertScrollbarAlwaysShown)
{
	//bool isVertScrollbarAlwaysShown() const;
	r_val = caller->Combobox()->isVertScrollbarAlwaysShown();
	return true;
}


SX_METHOD_IMPL(isHorzScrollbarAlwaysShown)
{
	//bool isHorzScrollbarAlwaysShown() const;
	r_val = caller->Combobox()->isHorzScrollbarAlwaysShown();
	return true;
}


SX_METHOD_IMPL(showDropList)
{
	//void showDropList();
	caller->Combobox()->showDropList();
	return true;
}


SX_METHOD_IMPL(hideDropList)
{
	//void hideDropList();
	caller->Combobox()->hideDropList();
	return true;
}


SX_METHOD_IMPL(setSingleClickEnabled)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	//void setSingleClickEnabled(bool setting);
	caller->Combobox()->setSingleClickEnabled(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(setReadOnly)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	//void setReadOnly(bool setting);
	caller->Combobox()->setReadOnly(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(setValidationString)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	//void setValidationString(string validation_string);
	caller->Combobox()->setValidationString(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	return true;
}


SX_METHOD_IMPL(setCaratIndex)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	//void setCaratIndex(unsigned long carat_pos);
	caller->Combobox()->setCaratIndex(args[0].intValue());
	return true;
}


SX_METHOD_IMPL(setSelection)
{
	skRValue::RType rtypes[] = { skRValue::T_Int, skRValue::T_Int };

	if(!CheckParams(2, 2, args, rtypes))
		return false;

	//void setSelection(unsigned long start_pos, unsigned long end_pos);
	caller->Combobox()->setSelection(args[0].intValue(), args[1].intValue());
	return true;
}


SX_METHOD_IMPL(setMaxTextLength)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	//void setMaxTextLength(unsigned long max_len);
	caller->Combobox()->setMaxTextLength(args[0].intValue());
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxCombobox initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXCOMBOBOX_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(getSingleClickEnabled);
	SX_ADD_METHOD(isDropDownListVisible);
	SX_ADD_METHOD(getEditbox);
	SX_ADD_METHOD(getPushButton);
	SX_ADD_METHOD(getDropList);
	SX_ADD_METHOD(hasInputFocus);
	SX_ADD_METHOD(isReadOnly);
	SX_ADD_METHOD(isTextValid);
	SX_ADD_METHOD(getValidationString);
	SX_ADD_METHOD(getCaratIndex);
	SX_ADD_METHOD(getSelectionStartIndex);
	SX_ADD_METHOD(getSelectionEndIndex);
	SX_ADD_METHOD(getSelectionLength);
	SX_ADD_METHOD(getMaxTextLength);
	SX_ADD_METHOD(activateEditbox);
	SX_ADD_METHOD(getItemCount);
	SX_ADD_METHOD(getSelectedItem);
	SX_ADD_METHOD(getListboxItemFromIndex);
	SX_ADD_METHOD(getItemIndex);
	SX_ADD_METHOD(isItemSelected);
	SX_ADD_METHOD(findItemWithText);
	SX_ADD_METHOD(isListboxItemInList);
	SX_ADD_METHOD(resetList);
	SX_ADD_METHOD(addItem);
	SX_ADD_METHOD(insertItem);
	SX_ADD_METHOD(removeItem);
	SX_ADD_METHOD(clearAllSelections);
	SX_ADD_METHOD(setSortingEnabled);
	SX_ADD_METHOD(setShowVertScrollbar);
	SX_ADD_METHOD(setShowHorzScrollbar);
	SX_ADD_METHOD(setItemSelectState);
	SX_ADD_METHOD(handleUpdatedListItemData);
	SX_ADD_METHOD(isSortEnabled);
	SX_ADD_METHOD(isVertScrollbarAlwaysShown);
	SX_ADD_METHOD(isHorzScrollbarAlwaysShown);
	SX_ADD_METHOD(showDropList);
	SX_ADD_METHOD(hideDropList);
	SX_ADD_METHOD(setSingleClickEnabled);
	SX_ADD_METHOD(setReadOnly);
	SX_ADD_METHOD(setValidationString);
	SX_ADD_METHOD(setCaratIndex);
	SX_ADD_METHOD(setSelection);
	SX_ADD_METHOD(setMaxTextLength);
}

} // namespace RFSX

#undef SX_IMPL_TYPE // And that's it in a nutshell
