/************************************************************************************//**
 * @file sxCEGUIListboxTextItem.cpp
 * @brief Implementation for RFSX ListboxTextItem object.
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUIListboxTextItem.h"
#include "../sxCEGUIFont.h"
#include "../sxCEGUIColour.h"
#include "../sxCEGUIColourRect.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxListboxTextItem

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxListboxTextItem implementation
//////////////////////////////////////////////////////////////////////////////////////////

bool SX_IMPL_TYPE::method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
{
	if(h_method(this, m_name, args, r_val, ctxt))
		return true;
	else
		return sxListboxItem::method(m_name, args, r_val, ctxt);
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxListboxTextItem method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(getFont)
{
	//Font* getFont() const;
	r_val.assignObject(new sxFont(caller->ListboxTextItem()->getFont()), true);
	return true;
}


SX_METHOD_IMPL(getTextColours)
{
	//ColourRect getTextColours() const;
	r_val.assignObject(new sxColourRect(caller->ListboxTextItem()->getTextColours()), true);
	return true;
}


SX_METHOD_IMPL(setFont)
{
	if(args.entries() != 1)
		return false;

	//void setFont(string font_name);
	if(args[0].type() == skRValue::T_String)
	{
		caller->ListboxTextItem()->setFont(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
		return true;
	}

	//void setFont(Font* font);
	sxFont *font = IS_FONT(args[0]);
	if(!font)
		return false;

	caller->ListboxTextItem()->setFont(font->Font());
	return true;
}


SX_METHOD_IMPL(setTextColours)
{
	//void setTextColours(colour top_left_colour, colour top_right_colour, colour bottom_left_colour, colour bottom_right_colour);
	//void setTextColours(const ColourRect& cols);
	//void setTextColours(colour col);
	if(args.entries() == 1)
	{
		sxColour *col = IS_COLOUR(args[0]);
		sxColourRect *crect = IS_COLOURRECT(args[0]);

		if(col)
		{
			caller->ListboxTextItem()->setTextColours(col->ColourConstRef());
			return true;
		}
		else if(crect)
		{
			caller->ListboxTextItem()->setTextColours(crect->ColourRectConstRef());
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

		caller->ListboxTextItem()->setTextColours(	top_left_colour->ColourConstRef(),
													top_right_colour->ColourConstRef(),
													bottom_left_colour->ColourConstRef(),
													bottom_right_colour->ColourConstRef());
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxListboxTextItem initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXLISTBOXTEXTITEM_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(getFont);
	SX_ADD_METHOD(getTextColours);
	SX_ADD_METHOD(setFont);
	SX_ADD_METHOD(setTextColours);
}

} // namespace RFSX

#undef SX_IMPL_TYPE // And that's it in a nutshell
