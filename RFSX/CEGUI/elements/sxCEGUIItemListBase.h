/************************************************************************************//**
 * @file sxCEGUIItemListBase.h
 * @brief Class declaration for RFSX ItemListBase object
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_ITEMLISTBASE_H_
#define _SX_ITEMLISTBASE_H_

#include "RFSX.h"
#include "../sxCEGUIWindow.h"

// sxItemListBase class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		int getItemCount();
		sxItemEntry getItemFromIndex(int index);
		int getItemIndex(sxItemEntry item);
		sxItemEntry findItemWithText(skString text, sxItemEntry start_item=null); -- may return null!
		bool isItemInList(sxItemEntry item);
		bool isAutoResizeEnabled();
		bool isSortEnabled();
		int getSortMode();
		sxRect getItemRenderArea();
		sxWindow getContentPane();

		void resetList();
		void addItem(sxItemEntry item);
		void insertItem(sxItemEntry item, sxItemEntry position=null);
		void removeItem(sxItemEntry item);
		void handleUpdatedItemData();
		void setAutoResizeEnabled(bool setting);
		void sizeToContent();
		void setSortEnabled(bool setting);
		void setSortMode(int mode);
		void sortList();
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxItemListBase

#define SXITEMLISTBASE_METHODS 20

namespace RFSX
{

class SX_IMPL_TYPE: public sxWindow
{
public:
	SX_IMPL_TYPE(CEGUI::ItemListBase* listBase) : sxWindow(listBase) {}
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);

	CEGUI::ItemListBase* ItemListBase()	{ return static_cast<CEGUI::ItemListBase*>(Window()); }

protected:
	SX_METHOD_DECL(getItemCount);
	SX_METHOD_DECL(getItemFromIndex);
	SX_METHOD_DECL(getItemIndex);
	SX_METHOD_DECL(findItemWithText);
	SX_METHOD_DECL(isItemInList);
	SX_METHOD_DECL(isAutoResizeEnabled);
	SX_METHOD_DECL(isSortEnabled);
	SX_METHOD_DECL(getSortMode);
	SX_METHOD_DECL(getItemRenderArea);
	SX_METHOD_DECL(getContentPane);
	SX_METHOD_DECL(resetList);
	SX_METHOD_DECL(addItem);
	SX_METHOD_DECL(insertItem);
	SX_METHOD_DECL(removeItem);
	SX_METHOD_DECL(handleUpdatedItemData);
	SX_METHOD_DECL(setAutoResizeEnabled);
	SX_METHOD_DECL(sizeToContent);
	SX_METHOD_DECL(setSortEnabled);
	SX_METHOD_DECL(setSortMode);
	SX_METHOD_DECL(sortList);

private:
	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_ITEMLISTBASE_H_
