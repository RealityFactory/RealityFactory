/************************************************************************************//**
 * @file sxCEGUIFont.cpp
 * @brief Implementation for RFSX Font object.
 * @author Daniel Queteschiner
 * @date November 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUIFont.h"
#include "sxCEGUIRect.h"
#include "sxCEGUISize.h"
#include "sxCEGUIIterators.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxFont

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxFont implementation
//////////////////////////////////////////////////////////////////////////////////////////

SX_IMPL_TYPE::SX_IMPL_TYPE(CEGUI::Font *font)
	: m_Font(font)
{
}


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

skExecutableIterator* SX_IMPL_TYPE::createIterator()
{
	return new sxPropertyIterator(this->PropertySet());
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxFont method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(getProperty)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	CEGUI::String s = caller->Font()->getProperty(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	r_val = skString(s.c_str());
	return true;
}


SX_METHOD_IMPL(setProperty)
{
	skRValue::RType rtypes[] = { skRValue::T_String, skRValue::T_String };

	if(!CheckParams(2, 2, args, rtypes))
		return false;

	caller->Font()->setProperty(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()),
								reinterpret_cast<const CEGUI::utf8*>(args[1].str().c_str()));
	return true;
}


SX_METHOD_IMPL(getPropertyDefault)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	CEGUI::String s = caller->Font()->getPropertyDefault(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	r_val = skString(s.c_str());
	return true;
}


SX_METHOD_IMPL(getPropertyHelp)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	CEGUI::String s = caller->Font()->getPropertyHelp(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	r_val = skString(s.c_str());
	return true;
}


SX_METHOD_IMPL(isPropertyPresent)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	r_val = caller->Font()->isPropertyPresent(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	return true;
}


SX_METHOD_IMPL(isPropertyDefault)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	r_val = caller->Font()->isPropertyDefault(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	return true;
}


SX_METHOD_IMPL(setDefaultResourceGroup)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	CEGUI::Font::setDefaultResourceGroup(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	return true;
}


SX_METHOD_IMPL(getDefaultResourceGroup)
{
	r_val = skString(CEGUI::Font::getDefaultResourceGroup().c_str());
	return true;
}


SX_METHOD_IMPL(load)
{
	caller->Font()->load();
	return true;
}


SX_METHOD_IMPL(isCodepointAvailable)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	r_val = caller->Font()->isCodepointAvailable(static_cast<unsigned int>(args[0].intValue()));
	return true;
}


SX_METHOD_IMPL(setNativeResolution)
{
	if(args.entries() != 1)
		return false;

	sxSize *sz = IS_SIZE(args[0]);
	if(!sz)
		return false;

	caller->Font()->setNativeResolution(*(sz->Size()));
	return true;
}


SX_METHOD_IMPL(notifyScreenResolution)
{
	if(args.entries() != 1)
		return false;

	sxSize *sz = IS_SIZE(args[0]);
	if(!sz)
		return false;

	caller->Font()->notifyScreenResolution(*(sz->Size()));
	return true;
}


SX_METHOD_IMPL(getTextExtent)
{
	skRValue::RType rtypes[] = { skRValue::T_String, skRValue::T_Float };

	if(!CheckParams(1, 2, args, rtypes))
		return false;

	CEGUI::String text(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	float x_scale = (args.entries() == 2) ? args[1].floatValue() : 1.0f;

	r_val = caller->Font()->getTextExtent(text, x_scale);
	return true;
}


SX_METHOD_IMPL(getLineSpacing)
{
	skRValue::RType rtypes[] = { skRValue::T_Float };

	if(!CheckParams(0, 1, args, rtypes))
		return false;

	float y_scale = (args.entries() == 1) ? args[0].floatValue() : 1.0f;

	r_val = caller->Font()->getLineSpacing(y_scale);
	return true;
}


SX_METHOD_IMPL(getFontHeight)
{
	skRValue::RType rtypes[] = { skRValue::T_Float };

	if(!CheckParams(0, 1, args, rtypes))
		return false;

	float y_scale = (args.entries() == 1) ? args[0].floatValue() : 1.0f;

	r_val = caller->Font()->getFontHeight(y_scale);
	return true;
}


SX_METHOD_IMPL(getCharAtPixel)
{
	skRValue::RType rtypes0[] = { skRValue::T_String, skRValue::T_Float, skRValue::T_Float };
	skRValue::RType rtypes1[] = { skRValue::T_String, skRValue::T_Int, skRValue::T_Float, skRValue::T_Float };

	if(CheckParams(2, 3, args, rtypes0))
	{
		CEGUI::String text(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
		float pixel = args[1].floatValue();
		float x_scale = (args.entries() == 3) ? args[2].floatValue() : 1.0f;

		r_val = static_cast<int>(caller->Font()->getCharAtPixel(text, pixel, x_scale));
		return true;
	}
	else if(CheckParams(3, 4, args, rtypes1))
	{
		CEGUI::String text(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
		size_t start_char = args[1].intValue();
		float pixel = args[2].floatValue();
		float x_scale = (args.entries() == 4) ? args[3].floatValue() : 1.0f;

	    r_val = static_cast<int>(caller->Font()->getCharAtPixel(text, start_char, pixel, x_scale));
		return true;
	}

	return false;
}


SX_METHOD_IMPL(getFormattedLineCount)
{
	skRValue::RType rtypes[] = {	skRValue::T_String,
									skRValue::T_Object,
									skRValue::T_Int,
									skRValue::T_Float };

	if(!CheckParams(3, 4, args, rtypes))
		return false;

	sxRect *format_area = IS_RECT(args[1]);
	if(!format_area)
		return false;

	CEGUI::String text(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	CEGUI::TextFormatting fmt = static_cast<CEGUI::TextFormatting>(args[2].intValue());
	float x_scale = (args.entries() == 4) ? args[3].floatValue() : 1.0f;

	r_val = static_cast<int>(caller->Font()->getFormattedLineCount(text, *(format_area->Rect()), fmt, x_scale));
	return true;
}


SX_METHOD_IMPL(getFormattedTextExtent)
{
	skRValue::RType rtypes[] = {	skRValue::T_String,
									skRValue::T_Object,
									skRValue::T_Int,
									skRValue::T_Float };

	if(!CheckParams(3, 4, args, rtypes))
		return false;

	sxRect *format_area = IS_RECT(args[1]);
	if(!format_area)
		return false;

	CEGUI::String text(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	CEGUI::TextFormatting fmt = static_cast<CEGUI::TextFormatting>(args[2].intValue());
	float x_scale = (args.entries() == 4) ? args[3].floatValue() : 1.0f;

	r_val = caller->Font()->getFormattedTextExtent(text, *(format_area->Rect()), fmt, x_scale);
	return true;
}


SX_METHOD_IMPL(drawText)
{
	skRValue::RType rtypes[] = {	skRValue::T_String,
									skRValue::T_Object,
									skRValue::T_Float,
									skRValue::T_Float,
									skRValue::T_Float };

	if(!CheckParams(2, 5, args, rtypes))
		return false;

	sxRect *draw_area = IS_RECT(args[1]);
	if(!draw_area)
		return false;

	CEGUI::String text(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	float z       = (args.entries() > 2) ? args[2].floatValue() : 1.0f;
	float x_scale = (args.entries() > 3) ? args[3].floatValue() : 1.0f;
	float y_scale = (args.entries() > 4) ? args[4].floatValue() : 1.0f;

	caller->Font()->drawText(text, *(draw_area->Rect()), z, x_scale, y_scale);
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxFont initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXFONT_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	// Add all methods here
	SX_ADD_METHOD(getProperty);
	SX_ADD_METHOD(setProperty);
	SX_ADD_METHOD(getPropertyDefault);
	SX_ADD_METHOD(getPropertyHelp);
	SX_ADD_METHOD(isPropertyPresent);
	SX_ADD_METHOD(isPropertyDefault);
	SX_ADD_METHOD(setDefaultResourceGroup);
	SX_ADD_METHOD(getDefaultResourceGroup);
	SX_ADD_METHOD(load);
	SX_ADD_METHOD(isCodepointAvailable);
	SX_ADD_METHOD(setNativeResolution);
	SX_ADD_METHOD(notifyScreenResolution);
	SX_ADD_METHOD(getTextExtent);
	SX_ADD_METHOD(getLineSpacing);
	SX_ADD_METHOD(getFontHeight);
	SX_ADD_METHOD(getCharAtPixel);
	SX_ADD_METHOD(getFormattedLineCount);
	SX_ADD_METHOD(getFormattedTextExtent);
	SX_ADD_METHOD(drawText);
}

} // namespace RFSX

#undef SX_IMPL_TYPE // And that's it in a nutshell
