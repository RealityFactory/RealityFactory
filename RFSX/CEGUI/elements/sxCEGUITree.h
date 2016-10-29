/************************************************************************************//**
 * @file sxCEGUITree.h
 * @brief Class declaration for RFSX Tree object
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_TREE_H_
#define _SX_TREE_H_

#include "RFSX.h"
#include "../sxCEGUIWindow.h"

// sxTree class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		int			getItemCount();
		int			getSelectedCount();

		sxTreeItem	getFirstSelectedItem();		-- may return null!
		sxTreeItem	getLastSelectedItem();		-- may return null!
		bool		isSortEnabled();
		bool		isMultiselectEnabled();
		bool		isItemTooltipsEnabled();

		sxTreeItem	findFirstItemWithText(skString text);							-- may return null!
		sxTreeItem	findNextItemWithText(skString text, sxTreeItem start_item=null);-- may return null!
		sxTreeItem	findFirstItemWithID(int searchID);								-- may return null!
		sxTreeItem	findNextItemWithID(int searchID, sxTreeItem start_item=null);	-- may return null!

		bool		isTreeItemInList(const TreeItem* item) const;
		void		resetList(void);
		void		addItem(TreeItem* item);

		void		insertItem(sxTreeItem item, sxTreeItem position);
		void		removeItem(sxTreeItem item);
		void		clearAllSelections();
		void		setSortingEnabled(bool setting);
		void		setMultiselectEnabled(bool setting);
		void		setItemSelectState(sxTreeItem item, bool state);
		void		setItemSelectState(int item_index, bool state);
		void		ensureItemIsVisible(sxTreeItem item);
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxTree

#define SXTREE_METHODS 21

namespace RFSX
{

class SX_IMPL_TYPE: public sxWindow
{
public:
	SX_IMPL_TYPE(CEGUI::Tree* tree) : sxWindow(tree) {}
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);

	CEGUI::Tree* Tree()	{ return static_cast<CEGUI::Tree*>(Window()); }

protected:
	SX_METHOD_DECL(getItemCount);
	SX_METHOD_DECL(getSelectedCount);
	SX_METHOD_DECL(getFirstSelectedItem);
	SX_METHOD_DECL(getLastSelectedItem);
	SX_METHOD_DECL(isSortEnabled);
	SX_METHOD_DECL(isMultiselectEnabled);
	SX_METHOD_DECL(isItemTooltipsEnabled);
	SX_METHOD_DECL(findFirstItemWithText);
	SX_METHOD_DECL(findNextItemWithText);
	SX_METHOD_DECL(findFirstItemWithID);
	SX_METHOD_DECL(findNextItemWithID);
	SX_METHOD_DECL(isTreeItemInList);
	SX_METHOD_DECL(resetList);
	SX_METHOD_DECL(addItem);
	SX_METHOD_DECL(insertItem);
	SX_METHOD_DECL(removeItem);
	SX_METHOD_DECL(clearAllSelections);
	SX_METHOD_DECL(setSortingEnabled);
	SX_METHOD_DECL(setMultiselectEnabled);
	SX_METHOD_DECL(setItemSelectState);
	SX_METHOD_DECL(ensureItemIsVisible);

private:
	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_TREE_H_
