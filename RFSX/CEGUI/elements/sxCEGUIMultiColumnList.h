/************************************************************************************//**
 * @file sxCEGUIMultiColumnList.h
 * @brief Class declaration for RFSX MultiColumnList object
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_MULTICOLUMNLIST_H_
#define _SX_MULTICOLUMNLIST_H_

#include "RFSX.h"
#include "../sxCEGUIWindow.h"

// sxMultiColumnList class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		bool isUserSortControlEnabled();
		bool isUserColumnSizingEnabled();
		bool isUserColumnDraggingEnabled();

		int getColumnCount();
		int getRowCount();
		int getSortColumn();
		int getColumnWithID(int col_id);
		int getColumnWithHeaderText(skString text);

		sxUDim getTotalColumnHeadersWidth();
		sxUDim getColumnHeaderWidth(int col_idx);

		int getSortDirection();
		sxListHeaderSegment getHeaderSegmentForColumn(int col_idx);

		int getItemRowIndex(sxListboxItem item);
		int getItemColumnIndex(sxListboxItem item);

		sxMCLGridRef getItemGridReference(sxListboxItem item);
		sxListboxItem getItemAtGridReference(sxMCLGridRef grid_ref);

		bool isListboxItemInColumn(sxListboxItem item, int col_idx);
		bool isListboxItemInRow(sxListboxItem item, int row_idx);
		bool isListboxItemInList(sxListboxItem item);

		sxListboxItem findColumnItemWithText(skString text, int col_idx, sxListboxItem start_item=null);
		sxListboxItem findRowItemWithText(skString text, int row_idx, sxListboxItem start_item=null);
		sxListboxItem findListItemWithText(skString text, sxListboxItem start_item=null);
		sxListboxItem getFirstSelectedItem();
		sxListboxItem getNextSelected(sxListboxItem start_item);

		int getSelectedCount();
		bool isItemSelected(sxMCLGridRef grid_ref);

		int getNominatedSelectionColumnID();
		int getNominatedSelectionColumn();
		int getNominatedSelectionRow();

		int getSelectionMode();

		bool isVertScrollbarAlwaysShown();
		bool isHorzScrollbarAlwaysShown();

		int getColumnID(int col_idx);
		int getRowID(int row_idx);
		int getRowWithID(int row_id);

		sxRect getListRenderArea();

		sxScrollbar getVertScrollbar();
		sxScrollbar getHorzScrollbar();
		sxListHeader getListHeader();

		float getTotalRowsHeight();
		float getWidestColumnItemWidth(int col_idx);
		float getHighestRowItemHeight(int row_idx);

		void resetList();

		void addColumn(skString text, int col_id, sxUDim width);
		void insertColumn(skString text, int col_id, sxUDim width, int position);
		void removeColumn(int col_idx);
		void removeColumnWithID(int col_id);
		void moveColumn(int col_idx, int position);
		void moveColumnWithID(int col_id, int position);

		int addRow(int row_id = 0);
		int addRow(sxListboxItem item, int col_id, int row_id = 0);
		int insertRow(int row_idx,  int row_id = 0);
		int insertRow(sxListboxItem item, int col_id, int row_idx, int row_id = 0);
		void removeRow(int row_idx);

		void setItem(sxListboxItem item, sxMCLGridRef position);
		void setItem(sxListboxItem item, int col_id, int row_idx);

		void setSelectionMode(int sel_mode);
		void setNominatedSelectionColumnID(int col_id);
		void setNominatedSelectionColumn(int col_idx);
		void setNominatedSelectionRow(int row_idx);

		void setSortDirection(int direction);
		void setSortColumn(int col_idx);
		void setSortColumnByID(int col_id);

		void setShowVertScrollbar(bool setting);
		void setShowHorzScrollbar(bool setting);

		void clearAllSelections();

		void setItemSelectState(sxListboxItem item, bool state);
		void setItemSelectState(sxMCLGridRef grid_ref, bool state);

		void handleUpdatedItemData();

		void setColumnHeaderWidth(int col_idx, sxUDim width);
		void setUserSortControlEnabled(bool setting);
		void setUserColumnSizingEnabled(bool setting);
		void setUserColumnDraggingEnabled(bool setting);
		void autoSizeColumnHeader(int col_idx);

		void setRowID(int row_idx, int row_id);
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxMultiColumnList

#define SXMULTICOLUMNLIST_METHODS 71

namespace RFSX
{

class SX_IMPL_TYPE: public sxWindow
{
public:
	SX_IMPL_TYPE(CEGUI::MultiColumnList* mcl) : sxWindow(mcl) {}
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);

	CEGUI::MultiColumnList* MultiColumnList()	{ return static_cast<CEGUI::MultiColumnList*>(Window()); }

protected:
	SX_METHOD_DECL(isUserSortControlEnabled);
	SX_METHOD_DECL(isUserColumnSizingEnabled);
	SX_METHOD_DECL(isUserColumnDraggingEnabled);
	SX_METHOD_DECL(getColumnCount);
	SX_METHOD_DECL(getRowCount);
	SX_METHOD_DECL(getSortColumn);
	SX_METHOD_DECL(getColumnWithID);
	SX_METHOD_DECL(getColumnWithHeaderText);
	SX_METHOD_DECL(getTotalColumnHeadersWidth);
	SX_METHOD_DECL(getColumnHeaderWidth);
	SX_METHOD_DECL(getSortDirection);
	SX_METHOD_DECL(getHeaderSegmentForColumn);
	SX_METHOD_DECL(getItemRowIndex);
	SX_METHOD_DECL(getItemColumnIndex);
	SX_METHOD_DECL(getItemGridReference);
	SX_METHOD_DECL(getItemAtGridReference);
	SX_METHOD_DECL(isListboxItemInColumn);
	SX_METHOD_DECL(isListboxItemInRow);
	SX_METHOD_DECL(isListboxItemInList);
	SX_METHOD_DECL(findColumnItemWithText);
	SX_METHOD_DECL(findRowItemWithText);
	SX_METHOD_DECL(findListItemWithText);
	SX_METHOD_DECL(getFirstSelectedItem);
	SX_METHOD_DECL(getNextSelected);
	SX_METHOD_DECL(getSelectedCount);
	SX_METHOD_DECL(isItemSelected);
	SX_METHOD_DECL(getNominatedSelectionColumnID);
	SX_METHOD_DECL(getNominatedSelectionColumn);
	SX_METHOD_DECL(getNominatedSelectionRow);
	SX_METHOD_DECL(getSelectionMode);
	SX_METHOD_DECL(isVertScrollbarAlwaysShown);
	SX_METHOD_DECL(isHorzScrollbarAlwaysShown);
	SX_METHOD_DECL(getColumnID);
	SX_METHOD_DECL(getRowID);
	SX_METHOD_DECL(getRowWithID);
	SX_METHOD_DECL(getListRenderArea);
	SX_METHOD_DECL(getVertScrollbar);
	SX_METHOD_DECL(getHorzScrollbar);
	SX_METHOD_DECL(getListHeader);
	SX_METHOD_DECL(getTotalRowsHeight);
	SX_METHOD_DECL(getWidestColumnItemWidth);
	SX_METHOD_DECL(getHighestRowItemHeight);
	SX_METHOD_DECL(resetList);
	SX_METHOD_DECL(addColumn);
	SX_METHOD_DECL(insertColumn);
	SX_METHOD_DECL(removeColumn);
	SX_METHOD_DECL(removeColumnWithID);
	SX_METHOD_DECL(moveColumn);
	SX_METHOD_DECL(moveColumnWithID);
	SX_METHOD_DECL(addRow);
	SX_METHOD_DECL(insertRow);
	SX_METHOD_DECL(removeRow);
	SX_METHOD_DECL(setItem);
	SX_METHOD_DECL(setSelectionMode);
	SX_METHOD_DECL(setNominatedSelectionColumnID);
	SX_METHOD_DECL(setNominatedSelectionColumn);
	SX_METHOD_DECL(setNominatedSelectionRow);
	SX_METHOD_DECL(setSortDirection);
	SX_METHOD_DECL(setSortColumn);
	SX_METHOD_DECL(setSortColumnByID);
	SX_METHOD_DECL(setShowVertScrollbar);
	SX_METHOD_DECL(setShowHorzScrollbar);
	SX_METHOD_DECL(clearAllSelections);
	SX_METHOD_DECL(setItemSelectState);
	SX_METHOD_DECL(handleUpdatedItemData);
	SX_METHOD_DECL(setColumnHeaderWidth);
	SX_METHOD_DECL(setUserSortControlEnabled);
	SX_METHOD_DECL(setUserColumnSizingEnabled);
	SX_METHOD_DECL(setUserColumnDraggingEnabled);
	SX_METHOD_DECL(autoSizeColumnHeader);
	SX_METHOD_DECL(setRowID);

private:
	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_MULTICOLUMNLIST_H_
