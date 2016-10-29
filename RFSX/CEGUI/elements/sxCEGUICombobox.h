/************************************************************************************//**
 * @file sxCEGUICombobox.h
 * @brief Class declaration for RFSX Combobox object
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_COMBOBOX_H_
#define _SX_COMBOBOX_H_

#include "RFSX.h"
#include "../sxCEGUIWindow.h"

// sxCombobox class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		bool getSingleClickEnabled();
		bool isDropDownListVisible();

		sxEditbox getEditbox();
		sxPushButton getPushButton();
		sxComboDropList getDropList();

		bool hasInputFocus();
		bool isReadOnly();
		bool isTextValid();
		skString getValidationString();

		int getCaratIndex();
		int getSelectionStartIndex();
		int getSelectionEndIndex();
		int getSelectionLength();
		int getMaxTextLength();

		void activateEditbox();

		int getItemCount();
		sxListboxItem getSelectedItem();	-- may return null!
		sxListboxItem getListboxItemFromIndex(int index);
		int getItemIndex(sxListboxItem item);
		bool isItemSelected(int index);
		sxListboxItem findItemWithText(skString text, sxListboxItem start_item=null);	-- may return null!
		bool isListboxItemInList(sxListboxItem item);

		void resetList();
		void addItem(sxListboxItem item);
		void insertItem(sxListboxItem item, sxListboxItem position=null);
		void removeItem(sxListboxItem item);
		void clearAllSelections();
		void setSortingEnabled(bool setting);
		void setShowVertScrollbar(bool setting);
		void setShowHorzScrollbar(bool setting);
		void setItemSelectState(sxListboxItem item, bool state);
		void setItemSelectState(int item_index, bool state);
		void handleUpdatedListItemData();

		bool isSortEnabled();
		bool isVertScrollbarAlwaysShown();
		bool isHorzScrollbarAlwaysShown();

		void showDropList();
		void hideDropList();
		void setSingleClickEnabled(bool setting);

		void setReadOnly(bool setting);
		void setValidationString(skString validation_string);
		void setCaratIndex(int carat_pos);
		void setSelection(int start_pos, int end_pos);
		void setMaxTextLength(int max_len);
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxCombobox

#define SXCOMBOBOX_METHODS 43

namespace RFSX
{

class SX_IMPL_TYPE: public sxWindow
{
public:
	SX_IMPL_TYPE(CEGUI::Combobox* combobox) : sxWindow(combobox) {}
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);

	CEGUI::Combobox* Combobox() { return static_cast<CEGUI::Combobox*>(Window()); }

protected:
	SX_METHOD_DECL(getSingleClickEnabled);
	SX_METHOD_DECL(isDropDownListVisible);
	SX_METHOD_DECL(getEditbox);
	SX_METHOD_DECL(getPushButton);
	SX_METHOD_DECL(getDropList);
	SX_METHOD_DECL(hasInputFocus);
	SX_METHOD_DECL(isReadOnly);
	SX_METHOD_DECL(isTextValid);
	SX_METHOD_DECL(getValidationString);
	SX_METHOD_DECL(getCaratIndex);
	SX_METHOD_DECL(getSelectionStartIndex);
	SX_METHOD_DECL(getSelectionEndIndex);
	SX_METHOD_DECL(getSelectionLength);
	SX_METHOD_DECL(getMaxTextLength);
	SX_METHOD_DECL(activateEditbox);
	SX_METHOD_DECL(getItemCount);
	SX_METHOD_DECL(getSelectedItem);
	SX_METHOD_DECL(getListboxItemFromIndex);
	SX_METHOD_DECL(getItemIndex);
	SX_METHOD_DECL(isItemSelected);
	SX_METHOD_DECL(findItemWithText);
	SX_METHOD_DECL(isListboxItemInList);
	SX_METHOD_DECL(resetList);
	SX_METHOD_DECL(addItem);
	SX_METHOD_DECL(insertItem);
	SX_METHOD_DECL(removeItem);
	SX_METHOD_DECL(clearAllSelections);
	SX_METHOD_DECL(setSortingEnabled);
	SX_METHOD_DECL(setShowVertScrollbar);
	SX_METHOD_DECL(setShowHorzScrollbar);
	SX_METHOD_DECL(setItemSelectState);
	SX_METHOD_DECL(handleUpdatedListItemData);
	SX_METHOD_DECL(isSortEnabled);
	SX_METHOD_DECL(isVertScrollbarAlwaysShown);
	SX_METHOD_DECL(isHorzScrollbarAlwaysShown);
	SX_METHOD_DECL(showDropList);
	SX_METHOD_DECL(hideDropList);
	SX_METHOD_DECL(setSingleClickEnabled);
	SX_METHOD_DECL(setReadOnly);
	SX_METHOD_DECL(setValidationString);
	SX_METHOD_DECL(setCaratIndex);
	SX_METHOD_DECL(setSelection);
	SX_METHOD_DECL(setMaxTextLength);

private:
	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_COMBOBOX_H_
