/************************************************************************************//**
 * @file sxCEGUITreeItem.cpp
 * @brief Implementation for RFSX TreeItem object.
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUITreeItem.h"
#include "../sxCEGUIColour.h"
#include "../sxCEGUIColourRect.h"
#include "../sxCEGUIWindow.h"
#include "../sxCEGUIImage.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxTreeItem

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxTreeItem implementation
//////////////////////////////////////////////////////////////////////////////////////////

/*SX_IMPL_TYPE::SX_IMPL_TYPE()
{
}*/


SX_IMPL_TYPE::~SX_IMPL_TYPE()
{
}


bool SX_IMPL_TYPE::method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
{
	if(h_method(this, m_name, args, r_val, ctxt))
		return true;
	else
		return skExecutable::method(m_name, args, r_val, ctxt);
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxTreeItem method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(getTextColours)
{
	r_val.assignObject(new sxColourRect(caller->TreeItem()->getTextColours()), true);
	return true;
}


SX_METHOD_IMPL(setTextColours)
{
	if(args.entries() == 1)
	{
		sxColour *col = IS_COLOUR(args[0]);
		sxColourRect *crect = IS_COLOURRECT(args[0]);
		if(col)
		{
			caller->TreeItem()->setTextColours(col->ColourConstRef());
			return true;
		}
		else if(crect)
		{
			caller->TreeItem()->setTextColours(crect->ColourRectConstRef());
			return true;
		}
	}
	if(args.entries() == 4)
	{
		sxColour *top_left_colour		= IS_COLOUR(args[0]);
		sxColour *top_right_colour		= IS_COLOUR(args[1]);
		sxColour *bottom_left_colour	= IS_COLOUR(args[2]);
		sxColour *bottom_right_colour	= IS_COLOUR(args[3]);
		if(!top_left_colour || !top_right_colour || !bottom_left_colour || !bottom_right_colour)
			return false;

		caller->TreeItem()->setTextColours(	top_left_colour->ColourConstRef(),
											top_right_colour->ColourConstRef(),
											bottom_left_colour->ColourConstRef(),
											bottom_right_colour->ColourConstRef());
		return true;
	}

	return false;
}


SX_METHOD_IMPL(getText)
{
	r_val = skString(caller->TreeItem()->getText().c_str());
	return true;
}


SX_METHOD_IMPL(getTooltipText)
{
	r_val = skString(caller->TreeItem()->getTooltipText().c_str());
	return true;
}

SX_METHOD_IMPL(getID)
{
	r_val = static_cast<int>(caller->TreeItem()->getID());
	return true;
}


SX_METHOD_IMPL(isSelected)
{
	r_val = caller->TreeItem()->isSelected();
	return true;
}


SX_METHOD_IMPL(isDisabled)
{
	r_val = caller->TreeItem()->isDisabled();
	return true;
}


SX_METHOD_IMPL(isAutoDeleted)
{
	r_val = caller->TreeItem()->isAutoDeleted();
	return true;
}


SX_METHOD_IMPL(getOwnerWindow)
{
	const CEGUI::Window *win = caller->TreeItem()->getOwnerWindow();

	r_val.assignObject(new sxWindow(const_cast<CEGUI::Window*>(win)), true);
	return true;
}


SX_METHOD_IMPL(getSelectionColours)
{
	r_val.assignObject(new sxColourRect(caller->TreeItem()->getSelectionColours()), true);
	return true;
}


SX_METHOD_IMPL(setText)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	caller->TreeItem()->setText(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	return true;
}


SX_METHOD_IMPL(setTooltipText)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	caller->TreeItem()->setTooltipText(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	return true;
}


SX_METHOD_IMPL(setSelected)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	caller->TreeItem()->setSelected(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(setDisabled)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	caller->TreeItem()->setDisabled(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(setAutoDeleted)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	caller->TreeItem()->setAutoDeleted(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(setSelectionColours)
{
	if(args.entries() == 1)
	{
		sxColour *col = IS_COLOUR(args[0]);
		sxColourRect *crect = IS_COLOURRECT(args[0]);

		if(col)
		{
			caller->TreeItem()->setSelectionColours(col->ColourConstRef());
			return true;
		}
		else if(crect)
		{
			caller->TreeItem()->setSelectionColours(crect->ColourRectConstRef());
			return true;
		}
	}
	if(args.entries() == 4)
	{
		sxColour *top_left_colour		= IS_COLOUR(args[0]);
		sxColour *top_right_colour		= IS_COLOUR(args[1]);
		sxColour *bottom_left_colour	= IS_COLOUR(args[2]);
		sxColour *bottom_right_colour	= IS_COLOUR(args[3]);

		if(!top_left_colour || !top_right_colour || !bottom_left_colour || !bottom_right_colour)
			return false;

		caller->TreeItem()->setSelectionColours(top_left_colour->ColourConstRef(),
												top_right_colour->ColourConstRef(),
												bottom_left_colour->ColourConstRef(),
												bottom_right_colour->ColourConstRef());
		return true;
	}

	return false;
}


SX_METHOD_IMPL(getIsOpen)
{
	r_val = caller->TreeItem()->getIsOpen();
	return true;
}


SX_METHOD_IMPL(toggleIsOpen)
{
	caller->TreeItem()->toggleIsOpen();
	return true;
}


SX_METHOD_IMPL(getItemCount)
{
	r_val = static_cast<int>(caller->TreeItem()->getItemCount());
	return true;
}


SX_METHOD_IMPL(addItem)
{
	if(args.entries() != 1)
		return false;

	sxTreeItem *item = IS_TREEITEM(args[0]);
	if(!item)
		return false;

	caller->TreeItem()->addItem(item->TreeItem());
	return true;
}


SX_METHOD_IMPL(setIcon)
{
	if(args.entries() != 1)
		return false;

	sxImage *icon = IS_IMAGE(args[0]);
	if(!icon)
		return false;

	caller->TreeItem()->setIcon(*(icon->Image()));
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxTreeItem initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXTREEITEM_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(getTextColours);
	SX_ADD_METHOD(setTextColours);
	SX_ADD_METHOD(getText);
	SX_ADD_METHOD(getTooltipText);
	SX_ADD_METHOD(getID);
	SX_ADD_METHOD(isSelected);
	SX_ADD_METHOD(isDisabled);
	SX_ADD_METHOD(isAutoDeleted);
	SX_ADD_METHOD(getOwnerWindow);
	SX_ADD_METHOD(getSelectionColours);
	SX_ADD_METHOD(setText);
	SX_ADD_METHOD(setTooltipText);
	SX_ADD_METHOD(setSelected);
	SX_ADD_METHOD(setDisabled);
	SX_ADD_METHOD(setAutoDeleted);
	SX_ADD_METHOD(setSelectionColours);
	SX_ADD_METHOD(getIsOpen);
	SX_ADD_METHOD(toggleIsOpen);
	SX_ADD_METHOD(getItemCount);
	SX_ADD_METHOD(addItem);
	SX_ADD_METHOD(setIcon);
}

} // namespace RFSX

#undef SX_IMPL_TYPE
