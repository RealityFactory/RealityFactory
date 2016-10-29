/************************************************************************************//**
 * @file sxCEGUIItemListbox.cpp
 * @brief Implementation for RFSX ItemListbox object.
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUIItemListbox.h"
#include "sxCEGUIItemEntry.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxItemListbox

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxItemListbox implementation
//////////////////////////////////////////////////////////////////////////////////////////

bool SX_IMPL_TYPE::method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
{
	if(h_method(this, m_name, args, r_val, ctxt))
		return true;
	else
		return sxScrolledItemListBase::method(m_name, args, r_val, ctxt);
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxItemListbox method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(getSelectedCount)
{
	//unsigned long getSelectedCount() const;
	r_val = static_cast<int>(caller->ItemListbox()->getSelectedCount());
	return true;
}


SX_METHOD_IMPL(getLastSelectedItem)
{
	//ItemEntry* getLastSelectedItem() const;
	CEGUI::ItemEntry *item = caller->ItemListbox()->getLastSelectedItem();

	if(item)
		r_val.assignObject(new sxItemEntry(item), true);
	else
		r_val.assignObject(&ctxt.getInterpreter()->getNull());

	return true;
}


SX_METHOD_IMPL(getFirstSelectedItem)
{
	if(args.entries() > 1)
		return false;

	unsigned long start_index = 0;

	if(args.entries() == 1)
	{
		if(args[0].type() != skRValue::T_Int)
			return false;

		start_index = args[0].intValue();
	}

	//ItemEntry* getFirstSelectedItem(unsigned long start_index=0) const;
	CEGUI::ItemEntry *item = caller->ItemListbox()->getFirstSelectedItem(start_index);

	if(item)
		r_val.assignObject(new sxItemEntry(item), true);
	else
		r_val.assignObject(&ctxt.getInterpreter()->getNull());

	return true;
}


SX_METHOD_IMPL(getNextSelectedItem)
{
	//ItemEntry* getNextSelectedItem() const;
	CEGUI::ItemEntry *item = caller->ItemListbox()->getNextSelectedItem();

	if(item)
		r_val.assignObject(new sxItemEntry(item), true);
	else
		r_val.assignObject(&ctxt.getInterpreter()->getNull());

	return true;
}


SX_METHOD_IMPL(getNextSelectedItemAfter)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Object)
		return false;

	CEGUI::ItemEntry* start_item = NULL;

	if(args[0].obj() != &ctxt.getInterpreter()->getNull())
	{
		sxItemEntry *start = static_cast<sxItemEntry*>(args[0].obj());
		start_item = start->ItemEntry();
	}

	//ItemEntry* getNextSelectedItemAfter(const ItemEntry* start_item) const;
	CEGUI::ItemEntry *item = caller->ItemListbox()->getNextSelectedItemAfter(start_item);

	if(item)
		r_val.assignObject(new sxItemEntry(item), true);
	else
		r_val.assignObject(&ctxt.getInterpreter()->getNull());

	return true;
}


SX_METHOD_IMPL(isMultiSelectEnabled)
{
	//bool isMultiSelectEnabled() const;
	r_val = caller->ItemListbox()->isMultiSelectEnabled();
	return true;
}


SX_METHOD_IMPL(isItemSelected)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	//bool isItemSelected(unsigned long index) const;
	r_val = caller->ItemListbox()->isItemSelected(args[0].intValue());
	return true;
}


SX_METHOD_IMPL(setMultiSelectEnabled)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	//void setMultiSelectEnabled(bool state);
	caller->ItemListbox()->setMultiSelectEnabled(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(clearAllSelections)
{
	//void clearAllSelections();
	caller->ItemListbox()->clearAllSelections();
	return true;
}


SX_METHOD_IMPL(selectRange)
{
	skRValue::RType rtypes[] = { skRValue::T_Int, skRValue::T_Int };

	if(!CheckParams(2, 2, args, rtypes))
		return false;

	//void selectRange(unsigned long a, unsigned long z);
	caller->ItemListbox()->selectRange(args[0].intValue(), args[1].intValue());
	return true;
}


SX_METHOD_IMPL(selectAllItems)
{
	//void selectAllItems();
	caller->ItemListbox()->selectAllItems();
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxItemListbox initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXITEMLISTBOX_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(getSelectedCount);
	SX_ADD_METHOD(getLastSelectedItem);
	SX_ADD_METHOD(getFirstSelectedItem);
	SX_ADD_METHOD(getNextSelectedItem);
	SX_ADD_METHOD(getNextSelectedItemAfter);
	SX_ADD_METHOD(isMultiSelectEnabled);
	SX_ADD_METHOD(isItemSelected);
	SX_ADD_METHOD(setMultiSelectEnabled);
	SX_ADD_METHOD(clearAllSelections);
	SX_ADD_METHOD(selectRange);
	SX_ADD_METHOD(selectAllItems);
}

} // namespace RFSX

#undef SX_IMPL_TYPE // And that's it in a nutshell
