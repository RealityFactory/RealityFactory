/************************************************************************************//**
 * @file sxCEGUIListboxItem.h
 * @brief Class declaration for RFSX ListboxItem object
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_LISTBOXITEM_H_
#define _SX_LISTBOXITEM_H_

#include "RFSX.h"

// sxListboxItem class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		skString getText();
		int getID();
		bool isSelected();
		bool isDisabled();
		bool isAutoDeleted();
		sxWindow getOwnerWindow();
		sxColourRect getSelectionColours();
		sxImage getSelectionBrushImage();

		void setText(skString text);
		void setID(int item_id);
		void setSelected(bool setting);
		void setDisabled(bool setting);
		void setAutoDeleted(bool setting);
		void setOwnerWindow(sxWindow owner);
		void setSelectionColours(sxColourRect cols);
		void setSelectionColours(sxColour top_left_colour, sxColour top_right_colour, sxColour bottom_left_colour, sxColour bottom_right_colour);
		void setSelectionColours(sxColour col);
		void setSelectionBrushImage(sxImage image);
		void setSelectionBrushImage(skString imageset, skString image);

		sxSize getPixelSize();
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxListboxItem

#define SXLISTBOXITEM_METHODS 17

#define IS_LISTBOXITEM(o)	CheckType<sxListboxItem>(o, RFSXU_LISTBOXITEM);


namespace RFSX
{

class SX_IMPL_TYPE: public skExecutable
{
public:
	SX_IMPL_TYPE(CEGUI::ListboxItem* item) : m_ListboxItem(item) {}
	virtual ~SX_IMPL_TYPE() {}
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);
	virtual int executableType() const { return RFSXU_LISTBOXITEM; }

	CEGUI::ListboxItem* ListboxItem() { return m_ListboxItem; }

protected:
	SX_METHOD_DECL(getText);
	SX_METHOD_DECL(getID);
	SX_METHOD_DECL(isSelected);
	SX_METHOD_DECL(isDisabled);
	SX_METHOD_DECL(isAutoDeleted);
	SX_METHOD_DECL(getOwnerWindow);
	SX_METHOD_DECL(getSelectionColours);
	SX_METHOD_DECL(getSelectionBrushImage);
	SX_METHOD_DECL(setText);
	SX_METHOD_DECL(setID);
	SX_METHOD_DECL(setSelected);
	SX_METHOD_DECL(setDisabled);
	SX_METHOD_DECL(setAutoDeleted);
	SX_METHOD_DECL(setOwnerWindow);
	SX_METHOD_DECL(setSelectionColours);
	SX_METHOD_DECL(setSelectionBrushImage);
	SX_METHOD_DECL(getPixelSize);

private:
	CEGUI::ListboxItem*	m_ListboxItem;

	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_LISTBOXITEM_H_
