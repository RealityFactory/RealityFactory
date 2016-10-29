/************************************************************************************//**
 * @file sxCEGUIListboxItem.cpp
 * @brief Implementation for RFSX ListboxItem object.
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUIListboxItem.h"
#include "../sxCEGUIColour.h"
#include "../sxCEGUIColourRect.h"
#include "../sxCEGUIImage.h"
#include "../sxCEGUIWindow.h"
#include "../sxCEGUISize.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxListboxItem

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxListboxItem implementation
//////////////////////////////////////////////////////////////////////////////////////////

bool SX_IMPL_TYPE::method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
{
	if(h_method(this, m_name, args, r_val, ctxt))
		return true;
	else
		return skExecutable::method(m_name, args, r_val, ctxt);
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxListboxItem method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(getText)
{
	//string getText() const;
	r_val = skString(caller->ListboxItem()->getText().c_str());
	return true;
}


SX_METHOD_IMPL(getID)
{
	//unsigned int getID() const;
	r_val = static_cast<int>(caller->ListboxItem()->getID());
	return true;
}


SX_METHOD_IMPL(isSelected)
{
	//bool isSelected() const;
	r_val = caller->ListboxItem()->isSelected();
	return true;
}


SX_METHOD_IMPL(isDisabled)
{
	//bool isDisabled() const;
	r_val = caller->ListboxItem()->isDisabled();
	return true;
}


SX_METHOD_IMPL(isAutoDeleted)
{
	//bool isAutoDeleted() const;
	r_val = caller->ListboxItem()->isAutoDeleted();
	return true;
}


SX_METHOD_IMPL(getOwnerWindow)
{
	//const Window* getOwnerWindow();
	const CEGUI::Window *win = caller->ListboxItem()->getOwnerWindow();

	r_val.assignObject(new sxWindow(const_cast<CEGUI::Window*>(win)), true);
	return true;
}


SX_METHOD_IMPL(getSelectionColours)
{
	//ColourRect getSelectionColours() const;
	r_val.assignObject(new sxColourRect(caller->ListboxItem()->getSelectionColours()), true);
	return true;
}


SX_METHOD_IMPL(getSelectionBrushImage)
{
	//const Image* getSelectionBrushImage() const;
	r_val.assignObject(new sxImage(caller->ListboxItem()->getSelectionBrushImage()), true);
	return true;
}


SX_METHOD_IMPL(setText)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	//void setText(string text);
	caller->ListboxItem()->setText(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	return true;
}


SX_METHOD_IMPL(setID)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	//void setID(unsigned int item_id);
	caller->ListboxItem()->setID(args[0].intValue());
	return true;
}


SX_METHOD_IMPL(setSelected)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	//void setSelected(bool setting);
	caller->ListboxItem()->setSelected(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(setDisabled)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	//void setDisabled(bool setting);
	caller->ListboxItem()->setDisabled(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(setAutoDeleted)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	//void setAutoDeleted(bool setting);
	caller->ListboxItem()->setAutoDeleted(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(setOwnerWindow)
{
	if(args.entries() != 1)
		return false;

	sxWindow *win = IS_WINDOW(args[0]);
	if(!win)
		return false;

	//void setOwnerWindow(const Window* owner);
	caller->ListboxItem()->setOwnerWindow(win->Window());
	return true;
}


SX_METHOD_IMPL(setSelectionColours)
{
	//void setSelectionColours(const ColourRect& cols);
	//void setSelectionColours(colour top_left_colour, colour top_right_colour, colour bottom_left_colour, colour bottom_right_colour);
	//void setSelectionColours(colour col);
	if(args.entries() == 1)
	{
		sxColour *col = IS_COLOUR(args[0]);

		if(col)
		{
			caller->ListboxItem()->setSelectionColours(col->ColourConstRef());
			return true;
		}

		sxColourRect *crect = IS_COLOURRECT(args[0]);

		if(crect)
		{
			caller->ListboxItem()->setSelectionColours(crect->ColourRectConstRef());
			return true;
		}
	}
	else if(args.entries() == 4)
	{
		sxColour *top_left_colour		= IS_COLOUR(args[0]);
		sxColour *top_right_colour		= IS_COLOUR(args[1]);
		sxColour *bottom_left_colour	= IS_COLOUR(args[2]);
		sxColour *bottom_right_colour	= IS_COLOUR(args[3]);

		if(!top_left_colour || !top_right_colour || !bottom_left_colour || !bottom_right_colour)
			return false;

		caller->ListboxItem()->setSelectionColours(	top_left_colour->ColourConstRef(),
													top_right_colour->ColourConstRef(),
													bottom_left_colour->ColourConstRef(),
													bottom_right_colour->ColourConstRef());
		return true;
	}

	return false;
}


SX_METHOD_IMPL(setSelectionBrushImage)
{
	//void setSelectionBrushImage(const Image* image);
	if(args.entries() == 1)
	{
		sxImage *image = IS_IMAGE(args[0]);

		if(!image)
			return false;

		caller->ListboxItem()->setSelectionBrushImage(image->Image());
		return true;
	}

	//void setSelectionBrushImage(string imageset, string image);
	skRValue::RType rtypes[] = { skRValue::T_String, skRValue::T_String };

	if(!CheckParams(2, 2, args, rtypes))
		return false;

	//void setDragCursorImage(string imageset, string image);
	caller->ListboxItem()->setSelectionBrushImage(
		reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()),
		reinterpret_cast<const CEGUI::utf8*>(args[1].str().c_str()));

	return true;
}


SX_METHOD_IMPL(getPixelSize)
{
	//Size getPixelSize() const;
	r_val.assignObject(new sxSize(caller->ListboxItem()->getPixelSize()), true);
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxListboxItem initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXLISTBOXITEM_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(getText);
	SX_ADD_METHOD(getID);
	SX_ADD_METHOD(isSelected);
	SX_ADD_METHOD(isDisabled);
	SX_ADD_METHOD(isAutoDeleted);
	SX_ADD_METHOD(getOwnerWindow);
	SX_ADD_METHOD(getSelectionColours);
	SX_ADD_METHOD(getSelectionBrushImage);
	SX_ADD_METHOD(setText);
	SX_ADD_METHOD(setID);
	SX_ADD_METHOD(setSelected);
	SX_ADD_METHOD(setDisabled);
	SX_ADD_METHOD(setAutoDeleted);
	SX_ADD_METHOD(setOwnerWindow);
	SX_ADD_METHOD(setSelectionColours);
	SX_ADD_METHOD(setSelectionBrushImage);
	SX_ADD_METHOD(getPixelSize);
}

} // namespace RFSX

#undef SX_IMPL_TYPE
