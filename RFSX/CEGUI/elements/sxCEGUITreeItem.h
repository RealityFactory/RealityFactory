/************************************************************************************//**
 * @file sxCEGUITreeItem.h
 * @brief Class declaration for RFSX TreeItem object
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_TREEITEM_H_
#define _SX_TREEITEM_H_

#include "RFSX.h"

// sxTreeItem class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		sxColourRect  getTextColours();
		void		setTextColours(sxColourRect cols);
		void		setTextColours(	sxColour top_left_colour,
									sxColour top_right_colour,
									sxColour bottom_left_colour,
									sxColour bottom_right_colour);
		void		setTextColours(sxColour col);
		skString	getText();
		skString	getTooltipText();
		int			getID();
		//void*		getUserData();
		bool		isSelected();
		bool		isDisabled();
		bool		isAutoDeleted();
		sxWindow	getOwnerWindow();		-- Window must not be modified!
		sxColourRect  getSelectionColours();
		void		setText(skString text);
		void		setTooltipText(skString text);
		//void		setUserData(void* item_data);
		void		setSelected(bool setting);
		void		setDisabled(bool setting);
		void		setAutoDeleted(bool setting);
		void		setSelectionColours(sxColourRect cols);
		void		setSelectionColours(colour top_left_colour, colour top_right_colour, colour bottom_left_colour, colour bottom_right_colour);
		void		setSelectionColours(colour col);
		bool		getIsOpen();
		void		toggleIsOpen();
		int			getItemCount();
		void		addItem(sxTreeItem item);
		void		setIcon(sxImage icon);
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxTreeItem

#define SXTREEITEM_METHODS 21

#define IS_TREEITEM(o)	CheckType<sxTreeItem>(o, RFSXU_TREEITEM);


namespace RFSX
{

class SX_IMPL_TYPE: public skExecutable
{
public:
	SX_IMPL_TYPE(CEGUI::TreeItem* item) : m_TreeItem(item) {}
	virtual ~SX_IMPL_TYPE();
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);
	virtual int executableType() const { return RFSXU_TREEITEM; }

	CEGUI::TreeItem* TreeItem() { return m_TreeItem; }

protected:
	SX_METHOD_DECL(getTextColours);
	SX_METHOD_DECL(setTextColours);
	SX_METHOD_DECL(getText);
	SX_METHOD_DECL(getTooltipText);
	SX_METHOD_DECL(getID);
	SX_METHOD_DECL(isSelected);
	SX_METHOD_DECL(isDisabled);
	SX_METHOD_DECL(isAutoDeleted);
	SX_METHOD_DECL(getOwnerWindow);
	SX_METHOD_DECL(getSelectionColours);
	SX_METHOD_DECL(setText);
	SX_METHOD_DECL(setTooltipText);
	SX_METHOD_DECL(setSelected);
	SX_METHOD_DECL(setDisabled);
	SX_METHOD_DECL(setAutoDeleted);
	SX_METHOD_DECL(setSelectionColours);
	SX_METHOD_DECL(getIsOpen);
	SX_METHOD_DECL(toggleIsOpen);
	SX_METHOD_DECL(getItemCount);
	SX_METHOD_DECL(addItem);
	SX_METHOD_DECL(setIcon);

private:
	CEGUI::TreeItem*	m_TreeItem;

	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_TREEITEM_H_
