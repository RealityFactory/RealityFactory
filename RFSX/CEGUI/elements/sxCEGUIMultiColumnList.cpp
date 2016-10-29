/************************************************************************************//**
 * @file sxCEGUIMultiColumnList.cpp
 * @brief Implementation for RFSX MultiColumnList object.
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUIMultiColumnList.h"
#include "sxCEGUIMCLGridRef.h"
#include "sxCEGUIListboxItem.h"
#include "sxCEGUIListHeader.h"
#include "sxCEGUIListHeaderSegment.h"
#include "sxCEGUIScrollbar.h"
#include "../sxCEGUIRect.h"
#include "../sxCEGUIUDim.h"


#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxMultiColumnList

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxMultiColumnList implementation
//////////////////////////////////////////////////////////////////////////////////////////

bool SX_IMPL_TYPE::method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
{
	if(h_method(this, m_name, args, r_val, ctxt))
		return true;
	else
		return sxWindow::method(m_name, args, r_val, ctxt);
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxMultiColumnList method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(isUserSortControlEnabled)
{
	//bool isUserSortControlEnabled() const;
	r_val = caller->MultiColumnList()->isUserSortControlEnabled();
	return true;
}


SX_METHOD_IMPL(isUserColumnSizingEnabled)
{
	//bool isUserColumnSizingEnabled() const;
	r_val = caller->MultiColumnList()->isUserColumnSizingEnabled();
	return true;
}


SX_METHOD_IMPL(isUserColumnDraggingEnabled)
{
	//bool isUserColumnDraggingEnabled() const;
	r_val = caller->MultiColumnList()->isUserColumnDraggingEnabled();
	return true;
}


SX_METHOD_IMPL(getColumnCount)
{
	//unsigned int getColumnCount() const;
	r_val = static_cast<int>(caller->MultiColumnList()->getColumnCount());
	return true;
}


SX_METHOD_IMPL(getRowCount)
{
	//unsigned int getRowCount() const;
	r_val = static_cast<int>(caller->MultiColumnList()->getRowCount());
	return true;
}


SX_METHOD_IMPL(getSortColumn)
{
	//unsigned int getSortColumn() const;
	r_val = static_cast<int>(caller->MultiColumnList()->getSortColumn());
	return true;
}


SX_METHOD_IMPL(getColumnWithID)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	//unsigned int getColumnWithID(unsigned int col_id) const;
	r_val = static_cast<int>(caller->MultiColumnList()->getColumnWithID(args[0].intValue()));
	return true;
}


SX_METHOD_IMPL(getColumnWithHeaderText)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	CEGUI::String text(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));

	//unsigned int getColumnWithHeaderText(string text) const;
	r_val = static_cast<int>(caller->MultiColumnList()->getColumnWithHeaderText(text));
	return true;
}


SX_METHOD_IMPL(getTotalColumnHeadersWidth)
{
	//UDim getTotalColumnHeadersWidth() const;
	r_val.assignObject(new sxUDim(caller->MultiColumnList()->getTotalColumnHeadersWidth()), true);
	return true;
}


SX_METHOD_IMPL(getColumnHeaderWidth)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	//UDim getColumnHeaderWidth(unsigned int col_idx) const;
	r_val.assignObject(new sxUDim(caller->MultiColumnList()->getColumnHeaderWidth(args[0].intValue())), true);
	return true;
}


SX_METHOD_IMPL(getSortDirection)
{
	//ListHeaderSegment::SortDirection getSortDirection() const;
	r_val = static_cast<int>(caller->MultiColumnList()->getSortDirection());
	return true;
}


SX_METHOD_IMPL(getHeaderSegmentForColumn)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	//ListHeaderSegment& getHeaderSegmentForColumn(unsigned int col_idx) const;
	r_val.assignObject(new sxListHeaderSegment(&(caller->MultiColumnList()->getHeaderSegmentForColumn(args[0].intValue()))), true);
	return true;
}


SX_METHOD_IMPL(getItemRowIndex)
{
	if(args.entries() != 1)
		return false;

	sxListboxItem *item = IS_LISTBOXITEM(args[0]);
	if(!item)
		return false;

	//unsigned int getItemRowIndex(const ListboxItem* item) const;
	r_val = static_cast<int>(caller->MultiColumnList()->getItemRowIndex(item->ListboxItem()));
	return true;
}


SX_METHOD_IMPL(getItemColumnIndex)
{
	if(args.entries() != 1)
		return false;

	sxListboxItem *item = IS_LISTBOXITEM(args[0]);
	if(!item)
		return false;

	//unsigned int getItemColumnIndex(const ListboxItem* item) const;
	r_val = static_cast<int>(caller->MultiColumnList()->getItemColumnIndex(item->ListboxItem()));
	return true;
}


SX_METHOD_IMPL(getItemGridReference)
{
	if(args.entries() != 1)
		return false;

	sxListboxItem *item = IS_LISTBOXITEM(args[0]);
	if(!item)
		return false;

	//MCLGridRef getItemGridReference(const ListboxItem* item) const;
	r_val.assignObject(new sxMCLGridRef(caller->MultiColumnList()->getItemGridReference(item->ListboxItem())), true);
	return true;
}


SX_METHOD_IMPL(getItemAtGridReference)
{
	if(args.entries() != 1)
		return false;

	sxMCLGridRef *gridRef = IS_MCLGRIDREF(args[0]);
	if(!gridRef)
		return false;

	//ListboxItem* getItemAtGridReference(const MCLGridRef& grid_ref) const;
	r_val.assignObject(new sxListboxItem(caller->MultiColumnList()->getItemAtGridReference(gridRef->MCLGridRefConstRef())), true);
	return true;
}


SX_METHOD_IMPL(isListboxItemInColumn)
{
	if(args.entries() != 2)
		return false;

	sxListboxItem *item = IS_LISTBOXITEM(args[0]);
	if(!item)
		return false;

	if(args[1].type() != skRValue::T_Int)
		return false;

	//bool isListboxItemInColumn(const ListboxItem* item, unsigned int col_idx) const;
	r_val = caller->MultiColumnList()->isListboxItemInColumn(item->ListboxItem(), args[1].intValue());
	return true;
}


SX_METHOD_IMPL(isListboxItemInRow)
{
	if(args.entries() != 2)
		return false;

	sxListboxItem *item = IS_LISTBOXITEM(args[0]);
	if(!item)
		return false;

	if(args[1].type() != skRValue::T_Int)
		return false;

	//bool isListboxItemInRow(const ListboxItem* item, unsigned int row_idx) const;
	r_val = caller->MultiColumnList()->isListboxItemInRow(item->ListboxItem(), args[1].intValue());
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
	r_val = caller->MultiColumnList()->isListboxItemInList(item->ListboxItem());
	return true;
}


SX_METHOD_IMPL(findColumnItemWithText)
{
	skRValue::RType rtypes[] = {	skRValue::T_String,
									skRValue::T_Int,
									skRValue::T_Object };

	if(!CheckParams(2, 3, args, rtypes))
		return false;

	CEGUI::String text(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	CEGUI::ListboxItem *start_item = NULL;

	if(args.entries() == 3)
	{
		sxListboxItem *item = IS_LISTBOXITEM(args[2]);

		if(!item)
			return false;

		start_item = item->ListboxItem();
	}

	//ListboxItem* findColumnItemWithText(string text, unsigned int col_idx, const ListboxItem* start_item) const;
	r_val.assignObject(new sxListboxItem(caller->MultiColumnList()->findColumnItemWithText(text, args[1].intValue(), start_item)), true);
	return true;
}


SX_METHOD_IMPL(findRowItemWithText)
{
	skRValue::RType rtypes[] = {	skRValue::T_String,
									skRValue::T_Int,
									skRValue::T_Object };

	if(!CheckParams(2, 3, args, rtypes))
		return false;

	CEGUI::String text(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	CEGUI::ListboxItem *start_item = NULL;

	if(args.entries() == 3)
	{
		sxListboxItem *start = IS_LISTBOXITEM(args[2]);

		if(!start)
			return false;

		start_item = start->ListboxItem();
	}

	//ListboxItem* findRowItemWithText(string text, unsigned int row_idx, const ListboxItem* start_item) const;
	r_val.assignObject(new sxListboxItem(caller->MultiColumnList()->findRowItemWithText(text, args[1].intValue(), start_item)), true);
	return true;
}


SX_METHOD_IMPL(findListItemWithText)
{
	skRValue::RType rtypes[] = { skRValue::T_String, skRValue::T_Object };

	if(!CheckParams(1, 2, args, rtypes))
		return false;

	CEGUI::String text(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	CEGUI::ListboxItem *start_item = NULL;

	if(args.entries() == 2)
	{
		sxListboxItem *start = IS_LISTBOXITEM(args[1]);

		if(!start)
			return false;

		start_item = start->ListboxItem();
	}

	//ListboxItem* findListItemWithText(string text, const ListboxItem* start_item) const;
	r_val.assignObject(new sxListboxItem(caller->MultiColumnList()->findListItemWithText(text, start_item)), true);
	return true;
}


SX_METHOD_IMPL(getFirstSelectedItem)
{
	//ListboxItem* getFirstSelectedItem() const;
	CEGUI::ListboxItem *item = caller->MultiColumnList()->getFirstSelectedItem();

	if(item)
		r_val.assignObject(new sxListboxItem(item), true);
	else
		r_val.assignObject(&ctxt.getInterpreter()->getNull());

	return true;
}


SX_METHOD_IMPL(getNextSelected)
{
	if(args.entries() > 1)
		return false;

	CEGUI::ListboxItem* start_item = NULL;

	if(args.entries() == 1)
	{
		sxListboxItem *start = IS_LISTBOXITEM(args[0]);

		if(!start)
			return false;

		start_item = start->ListboxItem();
	}

	//ListboxItem* getNextSelected(const ListboxItem* start_item) const;
	CEGUI::ListboxItem *item = caller->MultiColumnList()->getNextSelected(start_item);

	if(item)
		r_val.assignObject(new sxListboxItem(item), true);
	else
		r_val.assignObject(&ctxt.getInterpreter()->getNull());

	return true;
}


SX_METHOD_IMPL(getSelectedCount)
{
	//unsigned int getSelectedCount() const;
	r_val = static_cast<int>(caller->MultiColumnList()->getSelectedCount());
	return true;
}


SX_METHOD_IMPL(isItemSelected)
{
	if(args.entries() != 1)
		return false;

	sxMCLGridRef *gridRef = IS_MCLGRIDREF(args[0]);
	if(!gridRef)
		return false;

	//bool isItemSelected(const MCLGridRef& grid_ref) const;
	r_val = caller->MultiColumnList()->isItemSelected(gridRef->MCLGridRefConstRef());
	return true;
}


SX_METHOD_IMPL(getNominatedSelectionColumnID)
{
	//unsigned int getNominatedSelectionColumnID() const;
	r_val = static_cast<int>(caller->MultiColumnList()->getNominatedSelectionColumnID());
	return true;
}


SX_METHOD_IMPL(getNominatedSelectionColumn)
{
	//unsigned int getNominatedSelectionColumn() const;
	r_val = static_cast<int>(caller->MultiColumnList()->getNominatedSelectionColumn());
	return true;
}


SX_METHOD_IMPL(getNominatedSelectionRow)
{
	//unsigned int getNominatedSelectionRow() const;
	r_val = static_cast<int>(caller->MultiColumnList()->getNominatedSelectionRow());
	return true;
}


SX_METHOD_IMPL(getSelectionMode)
{
	//SelectionMode getSelectionMode() const;
	r_val = caller->MultiColumnList()->getSelectionMode();
	return true;
}


SX_METHOD_IMPL(isVertScrollbarAlwaysShown)
{
	//bool isVertScrollbarAlwaysShown() const;
	r_val = caller->MultiColumnList()->isVertScrollbarAlwaysShown();
	return true;
}


SX_METHOD_IMPL(isHorzScrollbarAlwaysShown)
{
	//bool isHorzScrollbarAlwaysShown() const;
	r_val = caller->MultiColumnList()->isHorzScrollbarAlwaysShown();
	return true;
}


SX_METHOD_IMPL(getColumnID)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	//unsigned int getColumnID(unsigned int col_idx) const;
	r_val = static_cast<int>(caller->MultiColumnList()->getColumnID(args[0].intValue()));
	return true;
}


SX_METHOD_IMPL(getRowID)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	//unsigned int getRowID(unsigned int row_idx) const;
	r_val = static_cast<int>(caller->MultiColumnList()->getRowID(args[0].intValue()));
	return true;
}


SX_METHOD_IMPL(getRowWithID)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	//unsigned int getRowWithID(unsigned int row_id) const;
	r_val = static_cast<int>(caller->MultiColumnList()->getRowWithID(args[0].intValue()));
	return true;
}


SX_METHOD_IMPL(getListRenderArea)
{
	//Rect getListRenderArea() const;
	r_val.assignObject(new sxRect(caller->MultiColumnList()->getListRenderArea()), true);
	return true;
}


SX_METHOD_IMPL(getVertScrollbar)
{
	//Scrollbar* getVertScrollbar() const;
	r_val.assignObject(new sxScrollbar(caller->MultiColumnList()->getVertScrollbar()), true);
	return true;
}


SX_METHOD_IMPL(getHorzScrollbar)
{
	//Scrollbar* getHorzScrollbar() const;
	r_val.assignObject(new sxScrollbar(caller->MultiColumnList()->getHorzScrollbar()), true);
	return true;
}


SX_METHOD_IMPL(getListHeader)
{
	//ListHeader* getListHeader() const;
	r_val.assignObject(new sxListHeader(caller->MultiColumnList()->getListHeader()), true);
	return true;
}


SX_METHOD_IMPL(getTotalRowsHeight)
{
	//float getTotalRowsHeight() const;
	r_val = caller->MultiColumnList()->getTotalRowsHeight();
	return true;
}


SX_METHOD_IMPL(getWidestColumnItemWidth)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	//float getWidestColumnItemWidth(unsigned int col_idx) const;
	r_val = caller->MultiColumnList()->getWidestColumnItemWidth(args[0].intValue());
	return true;
}


SX_METHOD_IMPL(getHighestRowItemHeight)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	//float getHighestRowItemHeight(unsigned int row_idx) const;
	r_val = caller->MultiColumnList()->getHighestRowItemHeight(args[0].intValue());
	return true;
}


SX_METHOD_IMPL(resetList)
{
	//void resetList();
	caller->MultiColumnList()->resetList();
	return true;
}


SX_METHOD_IMPL(addColumn)
{
	skRValue::RType rtypes[] = {	skRValue::T_String,
									skRValue::T_Int,
									skRValue::T_Object };

	if(!CheckParams(3, 3, args, rtypes))
		return false;

	CEGUI::String text(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));

	sxUDim *udim = IS_UDIM(args[2]);
	if(!udim)
		return false;

	//void addColumn(string text, unsigned int col_id, const UDim& width);
	caller->MultiColumnList()->addColumn(text, args[1].intValue(), udim->UDimConstRef());
	return true;
}


SX_METHOD_IMPL(insertColumn)
{
	skRValue::RType rtypes[] = {	skRValue::T_String,
									skRValue::T_Int,
									skRValue::T_Object,
									skRValue::T_Int };

	if(!CheckParams(4, 4, args, rtypes))
		return false;

	CEGUI::String text(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));

	sxUDim *udim = IS_UDIM(args[2]);
	if(!udim)
		return false;

	//void insertColumn(string text, unsigned int col_id, const UDim& width, unsigned int position);
	caller->MultiColumnList()->insertColumn(text, args[1].intValue(), udim->UDimConstRef(), args[3].intValue());
	return true;
}


SX_METHOD_IMPL(removeColumn)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	//void removeColumn(unsigned int col_idx);
	caller->MultiColumnList()->removeColumn(args[0].intValue());
	return true;
}


SX_METHOD_IMPL(removeColumnWithID)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	//void removeColumnWithID(unsigned int col_id);
	caller->MultiColumnList()->removeColumnWithID(args[0].intValue());
	return true;
}


SX_METHOD_IMPL(moveColumn)
{
	skRValue::RType rtypes[] = { skRValue::T_Int, skRValue::T_Int };

	if(!CheckParams(2, 2, args, rtypes))
		return false;

	//void moveColumn(unsigned int col_idx, unsigned int position);
	caller->MultiColumnList()->moveColumn(args[0].intValue(), args[1].intValue());
	return true;
}


SX_METHOD_IMPL(moveColumnWithID)
{
	skRValue::RType rtypes[] = { skRValue::T_Int, skRValue::T_Int };

	if(!CheckParams(2, 2, args, rtypes))
		return false;

	//void moveColumnWithID(unsigned int col_id, unsigned int position);
	caller->MultiColumnList()->moveColumnWithID(args[0].intValue(), args[1].intValue());
	return true;
}


SX_METHOD_IMPL(addRow)
{
	if(args.entries() > 3)
		return false;

	unsigned int row_id = 0;

	if(args.entries() == 1 || args.entries() == 3)
	{
		if(args[args.entries() - 1].type() != skRValue::T_Int)
			return false;

		row_id = args[args.entries() - 1].intValue();
	}

	//unsigned int addRow(unsigned int row_id = 0);
	if(args.entries() < 2)
	{
		caller->MultiColumnList()->addRow(row_id);
	}
	//unsigned int addRow(ListboxItem* item, unsigned int col_id, unsigned int row_id = 0);
	else
	{
		sxListboxItem *item = IS_LISTBOXITEM(args[0]);

		if(!item)
			return false;

		if(args[1].type() != skRValue::T_Int)
			return false;

		caller->MultiColumnList()->addRow(item->ListboxItem(), args[1].intValue(), row_id);
	}

	return true;
}


SX_METHOD_IMPL(insertRow)
{
	if(args.entries() > 4)
		return false;

	unsigned int row_id = 0;

	if(args.entries() == 2 || args.entries() == 4)
	{
		if(args[args.entries() - 1].type() != skRValue::T_Int)
			return false;

		row_id = args[args.entries() - 1].intValue();
	}

	//unsigned int insertRow(unsigned int row_idx, unsigned int row_id = 0);
	if(args.entries() < 3)
	{
		caller->MultiColumnList()->insertRow(args[0].intValue(), row_id);
	}
	//unsigned int insertRow(ListboxItem* item, unsigned int col_id, unsigned int row_idx, unsigned int row_id = 0);
	else
	{
		sxListboxItem *item = IS_LISTBOXITEM(args[0]);

		if(!item)
			return false;

		if(args[1].type() != skRValue::T_Int)
			return false;

		if(args[2].type() != skRValue::T_Int)
			return false;

		caller->MultiColumnList()->insertRow(item->ListboxItem(), args[1].intValue(), args[2].intValue(), row_id);
	}

	return true;
}


SX_METHOD_IMPL(removeRow)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	//void removeRow(unsigned int row_idx);
	caller->MultiColumnList()->removeRow(args[0].intValue());
	return true;
}


SX_METHOD_IMPL(setItem)
{
	if(args.entries() < 2)
		return false;

	sxListboxItem *item = IS_LISTBOXITEM(args[0]);
	if(!item)
		return false;

	//void setItem(ListboxItem* item, const MCLGridRef& position);
	if(args.entries() == 2)
	{
		sxMCLGridRef *gridRef = IS_MCLGRIDREF(args[1]);
		if(!gridRef)
			return false;

		caller->MultiColumnList()->setItem(item->ListboxItem(), gridRef->MCLGridRefConstRef());
		return true;
	}
	//void setItem(ListboxItem* item, unsigned int col_id, unsigned int row_idx);
	if(args.entries() == 3)
	{
		if(args[1].type() != skRValue::T_Int)
			return false;

		if(args[2].type() != skRValue::T_Int)
			return false;

		caller->MultiColumnList()->setItem(item->ListboxItem(), args[1].intValue(), args[2].intValue());
		return true;
	}

	return false;
}


SX_METHOD_IMPL(setSelectionMode)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	//void setSelectionMode(MultiColumnList::SelectionMode sel_mode);
	caller->MultiColumnList()->setSelectionMode(static_cast<CEGUI::MultiColumnList::SelectionMode>(args[0].intValue()));
	return true;
}


SX_METHOD_IMPL(setNominatedSelectionColumnID)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	//void setNominatedSelectionColumnID(unsigned int col_id);
	caller->MultiColumnList()->setNominatedSelectionColumnID(args[0].intValue());
	return true;
}


SX_METHOD_IMPL(setNominatedSelectionColumn)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	//void setNominatedSelectionColumn(unsigned int col_idx);
	caller->MultiColumnList()->setNominatedSelectionColumn(args[0].intValue());
	return true;
}


SX_METHOD_IMPL(setNominatedSelectionRow)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	//void setNominatedSelectionRow(unsigned int row_idx);
	caller->MultiColumnList()->setNominatedSelectionRow(args[0].intValue());
	return true;
}


SX_METHOD_IMPL(setSortDirection)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	//void setSortDirection(ListHeaderSegment::SortDirection direction);
	caller->MultiColumnList()->setSortDirection(static_cast<CEGUI::ListHeaderSegment::SortDirection>(args[0].intValue()));
	return true;
}


SX_METHOD_IMPL(setSortColumn)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	//void setSortColumn(unsigned int col_idx);
	caller->MultiColumnList()->setSortColumn(args[0].intValue());
	return true;
}


SX_METHOD_IMPL(setSortColumnByID)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	//void setSortColumnByID(unsigned int col_id);
	caller->MultiColumnList()->setSortColumnByID(args[0].intValue());
	return true;
}


SX_METHOD_IMPL(setShowVertScrollbar)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	//void setShowVertScrollbar(bool setting);
	caller->MultiColumnList()->setShowVertScrollbar(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(setShowHorzScrollbar)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	//void setShowHorzScrollbar(bool setting);
	caller->MultiColumnList()->setShowHorzScrollbar(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(clearAllSelections)
{
	//void clearAllSelections();
	caller->MultiColumnList()->clearAllSelections();
	return true;
}


SX_METHOD_IMPL(setItemSelectState)
{
	skRValue::RType rtypes[] = { skRValue::T_Object, skRValue::T_Bool };

	if(!CheckParams(2, 2, args, rtypes))
		return false;

	//void setItemSelectState(ListboxItem* item, bool state);
	sxListboxItem *item = IS_LISTBOXITEM(args[0]);
	if(item)
	{
		caller->MultiColumnList()->setItemSelectState(item->ListboxItem(), args[1].boolValue());
		return true;
	}

	//void setItemSelectState(const MCLGridRef& grid_ref, bool state);
	sxMCLGridRef *gridRef = IS_MCLGRIDREF(args[0]);
	if(gridRef)
	{
		caller->MultiColumnList()->setItemSelectState(gridRef->MCLGridRefConstRef(), args[1].boolValue());
		return true;
	}

	return false;
}


SX_METHOD_IMPL(handleUpdatedItemData)
{
	//void handleUpdatedItemData();
	caller->MultiColumnList()->handleUpdatedItemData();
	return true;
}


SX_METHOD_IMPL(setColumnHeaderWidth)
{
	if(args.entries() != 2)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	sxUDim *udim = IS_UDIM(args[1]);
	if(!udim)
		return false;

	//void setColumnHeaderWidth(unsigned int col_idx, const UDim& width);
	caller->MultiColumnList()->setColumnHeaderWidth(args[0].intValue(), udim->UDimConstRef());
	return true;
}


SX_METHOD_IMPL(setUserSortControlEnabled)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	//void setUserSortControlEnabled(bool setting);
	caller->MultiColumnList()->setUserSortControlEnabled(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(setUserColumnSizingEnabled)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	//void setUserColumnSizingEnabled(bool setting);
	caller->MultiColumnList()->setUserColumnSizingEnabled(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(setUserColumnDraggingEnabled)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	//void setUserColumnDraggingEnabled(bool setting);
	caller->MultiColumnList()->setUserColumnDraggingEnabled(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(autoSizeColumnHeader)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	//void autoSizeColumnHeader(unsigned int col_idx);
	caller->MultiColumnList()->autoSizeColumnHeader(args[0].intValue());
	return true;
}


SX_METHOD_IMPL(setRowID)
{
	skRValue::RType rtypes[] = { skRValue::T_Int, skRValue::T_Int };

	if(!CheckParams(2, 2, args, rtypes))
		return false;

	//void setRowID(unsigned int row_idx, unsigned int row_id);
	caller->MultiColumnList()->setRowID(args[0].intValue(), args[1].intValue());
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxMultiColumnList initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXMULTICOLUMNLIST_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(isUserSortControlEnabled);
	SX_ADD_METHOD(isUserColumnSizingEnabled);
	SX_ADD_METHOD(isUserColumnDraggingEnabled);
	SX_ADD_METHOD(getColumnCount);
	SX_ADD_METHOD(getRowCount);
	SX_ADD_METHOD(getSortColumn);
	SX_ADD_METHOD(getColumnWithID);
	SX_ADD_METHOD(getColumnWithHeaderText);
	SX_ADD_METHOD(getTotalColumnHeadersWidth);
	SX_ADD_METHOD(getColumnHeaderWidth);
	SX_ADD_METHOD(getSortDirection);
	SX_ADD_METHOD(getHeaderSegmentForColumn);
	SX_ADD_METHOD(getItemRowIndex);
	SX_ADD_METHOD(getItemColumnIndex);
	SX_ADD_METHOD(getItemGridReference);
	SX_ADD_METHOD(getItemAtGridReference);
	SX_ADD_METHOD(isListboxItemInColumn);
	SX_ADD_METHOD(isListboxItemInRow);
	SX_ADD_METHOD(isListboxItemInList);
	SX_ADD_METHOD(findColumnItemWithText);
	SX_ADD_METHOD(findRowItemWithText);
	SX_ADD_METHOD(findListItemWithText);
	SX_ADD_METHOD(getFirstSelectedItem);
	SX_ADD_METHOD(getNextSelected);
	SX_ADD_METHOD(getSelectedCount);
	SX_ADD_METHOD(isItemSelected);
	SX_ADD_METHOD(getNominatedSelectionColumnID);
	SX_ADD_METHOD(getNominatedSelectionColumn);
	SX_ADD_METHOD(getNominatedSelectionRow);
	SX_ADD_METHOD(getSelectionMode);
	SX_ADD_METHOD(isVertScrollbarAlwaysShown);
	SX_ADD_METHOD(isHorzScrollbarAlwaysShown);
	SX_ADD_METHOD(getColumnID);
	SX_ADD_METHOD(getRowID);
	SX_ADD_METHOD(getRowWithID);
	SX_ADD_METHOD(getListRenderArea);
	SX_ADD_METHOD(getVertScrollbar);
	SX_ADD_METHOD(getHorzScrollbar);
	SX_ADD_METHOD(getListHeader);
	SX_ADD_METHOD(getTotalRowsHeight);
	SX_ADD_METHOD(getWidestColumnItemWidth);
	SX_ADD_METHOD(getHighestRowItemHeight);
	SX_ADD_METHOD(resetList);
	SX_ADD_METHOD(addColumn);
	SX_ADD_METHOD(insertColumn);
	SX_ADD_METHOD(removeColumn);
	SX_ADD_METHOD(removeColumnWithID);
	SX_ADD_METHOD(moveColumn);
	SX_ADD_METHOD(moveColumnWithID);
	SX_ADD_METHOD(addRow);
	SX_ADD_METHOD(insertRow);
	SX_ADD_METHOD(removeRow);
	SX_ADD_METHOD(setItem);
	SX_ADD_METHOD(setSelectionMode);
	SX_ADD_METHOD(setNominatedSelectionColumnID);
	SX_ADD_METHOD(setNominatedSelectionColumn);
	SX_ADD_METHOD(setNominatedSelectionRow);
	SX_ADD_METHOD(setSortDirection);
	SX_ADD_METHOD(setSortColumn);
	SX_ADD_METHOD(setSortColumnByID);
	SX_ADD_METHOD(setShowVertScrollbar);
	SX_ADD_METHOD(setShowHorzScrollbar);
	SX_ADD_METHOD(clearAllSelections);
	SX_ADD_METHOD(setItemSelectState);
	SX_ADD_METHOD(handleUpdatedItemData);
	SX_ADD_METHOD(setColumnHeaderWidth);
	SX_ADD_METHOD(setUserSortControlEnabled);
	SX_ADD_METHOD(setUserColumnSizingEnabled);
	SX_ADD_METHOD(setUserColumnDraggingEnabled);
	SX_ADD_METHOD(autoSizeColumnHeader);
	SX_ADD_METHOD(setRowID);
}

} // namespace RFSX

#undef SX_IMPL_TYPE // And that's it in a nutshell
