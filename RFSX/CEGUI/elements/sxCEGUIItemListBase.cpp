/************************************************************************************//**
 * @file sxCEGUIItemListBase.cpp
 * @brief Implementation for RFSX ItemListBase object.
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUIItemListBase.h"
#include "sxCEGUIItemEntry.h"
#include "../sxCEGUIRect.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxItemListBase

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxItemListBase implementation
//////////////////////////////////////////////////////////////////////////////////////////

bool SX_IMPL_TYPE::method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
{
	if(h_method(this, m_name, args, r_val, ctxt))
		return true;
	else
		return sxWindow::method(m_name, args, r_val, ctxt);
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxItemListBase method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(getItemCount)
{
	//unsigned int getItemCount() const;
	r_val = static_cast<int>(caller->ItemListBase()->getItemCount());
	return true;
}


SX_METHOD_IMPL(getItemFromIndex)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	//ItemEntry* getItemFromIndex(unsigned int index) const;
	r_val.assignObject(new sxItemEntry(caller->ItemListBase()->getItemFromIndex(args[0].intValue())), true);
	return true;
}


SX_METHOD_IMPL(getItemIndex)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Object)
		return false;

	sxItemEntry *item = static_cast<sxItemEntry*>(args[0].obj());

	//unsigned int getItemIndex(const ItemEntry* item) const;
	r_val = static_cast<int>(caller->ItemListBase()->getItemIndex(item->ItemEntry()));
	return true;
}


SX_METHOD_IMPL(findItemWithText)
{
	if(args.entries() > 2)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	CEGUI::String text(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	CEGUI::ItemEntry *start_item = NULL;

	if(args.entries() == 2)
	{
		if(args[1].type() != skRValue::T_Object)
			return false;

		sxItemEntry *item = static_cast<sxItemEntry*>(args[1].obj());

		start_item = item->ItemEntry();
	}

	//ItemEntry* findItemWithText(string text, const ItemEntry* start_item=null);
	CEGUI::ItemEntry *item = caller->ItemListBase()->findItemWithText(text, start_item);

	if(item)
		r_val.assignObject(new sxItemEntry(item), true);
	else
		r_val.assignObject(&ctxt.getInterpreter()->getNull());

	return true;
}


SX_METHOD_IMPL(isItemInList)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Object)
		return false;

	sxItemEntry *item = static_cast<sxItemEntry*>(args[0].obj());

	//bool isItemInList(const ItemEntry* item) const;
	r_val = caller->ItemListBase()->isItemInList(item->ItemEntry());
	return true;
}


SX_METHOD_IMPL(isAutoResizeEnabled)
{
	//bool isAutoResizeEnabled() const;
	r_val = caller->ItemListBase()->isAutoResizeEnabled();
	return true;
}


SX_METHOD_IMPL(isSortEnabled)
{
	//bool isSortEnabled() const;
	r_val = caller->ItemListBase()->isSortEnabled();
	return true;
}


SX_METHOD_IMPL(getSortMode)
{
	//SortMode getSortMode() const;
	r_val = static_cast<int>(caller->ItemListBase()->getSortMode());
	return true;
}


SX_METHOD_IMPL(getItemRenderArea)
{
	//Rect getItemRenderArea() const;
	r_val.assignObject(new sxRect(caller->ItemListBase()->getItemRenderArea()), true);
	return true;
}


SX_METHOD_IMPL(getContentPane)
{
	//Window* getContentPane() const;
	r_val.assignObject(new sxWindow(caller->ItemListBase()->getContentPane()), true);
	return true;
}


SX_METHOD_IMPL(resetList)
{
	//void resetList();
	caller->ItemListBase()->resetList();
	return true;
}


SX_METHOD_IMPL(addItem)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Object)
		return false;

	if(args[0].obj() == &ctxt.getInterpreter()->getNull())
		return true; // nothing happens

	sxItemEntry *item = static_cast<sxItemEntry*>(args[0].obj());

	//void addItem(ItemEntry* item);
	caller->ItemListBase()->addItem(item->ItemEntry());
	return true;
}


SX_METHOD_IMPL(insertItem)
{
	skRValue::RType rtypes[] = { skRValue::T_Object, skRValue::T_Object };

	if(!CheckParams(1, 2, args, rtypes))
		return false;

	if(args[0].obj() == &ctxt.getInterpreter()->getNull())
		return true; // nothing happens

	CEGUI::ItemEntry *item = NULL;
	CEGUI::ItemEntry *position = NULL;

	sxItemEntry *sxitem = static_cast<sxItemEntry*>(args[0].obj());
	item = sxitem->ItemEntry();

	if(args.entries() == 2)
	{
		if(args[1].obj() != &ctxt.getInterpreter()->getNull())
		{
			sxItemEntry *pos = static_cast<sxItemEntry*>(args[1].obj());
			position = pos->ItemEntry();
		}
	}

	//void insertItem(ItemEntry* item, const ItemEntry* position);
	caller->ItemListBase()->insertItem(item, position);
	return true;
}


SX_METHOD_IMPL(removeItem)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Object)
		return false;

	sxItemEntry *item = static_cast<sxItemEntry*>(args[0].obj());

	//void removeItem(ItemEntry* item);
	caller->ItemListBase()->removeItem(item->ItemEntry());
	return true;
}


SX_METHOD_IMPL(handleUpdatedItemData)
{
	//void handleUpdatedItemData();
	caller->ItemListBase()->handleUpdatedItemData();
	return true;
}


SX_METHOD_IMPL(setAutoResizeEnabled)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	//void setAutoResizeEnabled(bool setting);
	caller->ItemListBase()->setAutoResizeEnabled(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(sizeToContent)
{
	//void sizeToContent();
	caller->ItemListBase()->sizeToContent();
	return true;
}


SX_METHOD_IMPL(setSortEnabled)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	//void setSortEnabled(bool setting);
	caller->ItemListBase()->setSortEnabled(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(setSortMode)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	//void setSortMode(SortMode mode);
	caller->ItemListBase()->setSortMode(static_cast<CEGUI::ItemListBase::SortMode>(args[0].intValue()));
	return true;
}


SX_METHOD_IMPL(sortList)
{
	//void sortList();
	caller->ItemListBase()->sortList();
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxItemListBase initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXITEMLISTBASE_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(getItemCount);
	SX_ADD_METHOD(getItemFromIndex);
	SX_ADD_METHOD(getItemIndex);
	SX_ADD_METHOD(findItemWithText);
	SX_ADD_METHOD(isItemInList);
	SX_ADD_METHOD(isAutoResizeEnabled);
	SX_ADD_METHOD(isSortEnabled);
	SX_ADD_METHOD(getSortMode);
	SX_ADD_METHOD(getItemRenderArea);
    SX_ADD_METHOD(getContentPane);
	SX_ADD_METHOD(resetList);
	SX_ADD_METHOD(addItem);
	SX_ADD_METHOD(insertItem);
	SX_ADD_METHOD(removeItem);
	SX_ADD_METHOD(handleUpdatedItemData);
	SX_ADD_METHOD(setAutoResizeEnabled);
	SX_ADD_METHOD(sizeToContent);
	SX_ADD_METHOD(setSortEnabled);
	SX_ADD_METHOD(setSortMode);
	SX_ADD_METHOD(sortList);
}

} // namespace RFSX

#undef SX_IMPL_TYPE
