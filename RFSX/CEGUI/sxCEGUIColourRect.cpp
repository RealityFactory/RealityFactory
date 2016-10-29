/************************************************************************************//**
 * @file sxCEGUIColourRect.cpp
 * @brief Implementation for RFSX TEMPLATE object.
 * @author Daniel Queteschiner
 * @date November 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUIColourRect.h"
#include "sxCEGUIColour.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxColourRect

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxColourRect implementation
//////////////////////////////////////////////////////////////////////////////////////////

SX_IMPL_TYPE::SX_IMPL_TYPE()
{
}


SX_IMPL_TYPE::SX_IMPL_TYPE(const CEGUI::ColourRect& val)
{
	m_ColourRect = val;
}


SX_IMPL_TYPE::SX_IMPL_TYPE(const CEGUI::colour& col)
	: m_ColourRect(col)
{
}


SX_IMPL_TYPE::SX_IMPL_TYPE(const CEGUI::colour& top_left, const CEGUI::colour& top_right,
				 const CEGUI::colour& bottom_left, const CEGUI::colour& bottom_right)
{
	m_ColourRect.d_top_left = top_left;
	m_ColourRect.d_top_right = top_right;
	m_ColourRect.d_bottom_left = bottom_left;
	m_ColourRect.d_bottom_right = bottom_right;
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

//////////////////////////////////////////////////////////////////////////////////////////
// sxColourRect fields
//////////////////////////////////////////////////////////////////////////////////////////

bool SX_IMPL_TYPE::setValue(const skString& name, const skString& att, const skRValue& val)
{
	skRValue value = val;
	if(name == "top_left")
	{
		sxColour *other = IS_COLOUR(value);
		if(!other)
			return false;

		m_ColourRect.d_top_left = *(other->Colour());
		return true;
	}
	if(name == "top_right")
	{
		sxColour *other = IS_COLOUR(value);
		if(!other)
			return false;

		m_ColourRect.d_top_right = *(other->Colour());
		return true;
	}
	if(name == "bottom_left")
	{
		sxColour *other = IS_COLOUR(value);
		if(!other)
			return false;

		m_ColourRect.d_bottom_left = *(other->Colour());
		return true;
	}
	if(name == "bottom_right")
	{
		sxColour *other = IS_COLOUR(value);
		if(!other)
			return false;

		m_ColourRect.d_bottom_right = *(other->Colour());
		return true;
	}

	return skExecutable::setValue(name, att, val);
}


bool SX_IMPL_TYPE::getValue(const skString& name, const skString& att, skRValue& val)
{
	if(name == "top_left")
	{
		val.assignObject(new sxColour(m_ColourRect.d_top_left), true);
		return true;
	}
	if(name == "top_right")
	{
		val.assignObject(new sxColour(m_ColourRect.d_top_right), true);
		return true;
	}
	if(name == "bottom_left")
	{
		val.assignObject(new sxColour(m_ColourRect.d_bottom_left), true);
		return true;
	}
	if(name == "bottom_right")
	{
		val.assignObject(new sxColour(m_ColourRect.d_bottom_right), true);
		return true;
	}

	return skExecutable::getValue(name, att, val);
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxColourRect method functions -- add actual method processing code here
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(setAlpha)
{
	caller->ColourRect()->setAlpha(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(setTopAlpha)
{
	caller->ColourRect()->setTopAlpha(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(setBottomAlpha)
{
	caller->ColourRect()->setBottomAlpha(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(setLeftAlpha)
{
	caller->ColourRect()->setLeftAlpha(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(setRightAlpha)
{
	caller->ColourRect()->setRightAlpha(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(isMonochromatic)
{
	r_val = caller->ColourRect()->isMonochromatic();
	return true;
}


SX_METHOD_IMPL(getSubRectangle)
{
	skRValue::RType rtypes[] = {	skRValue::T_Float, skRValue::T_Float,
									skRValue::T_Float, skRValue::T_Float }; // parameter type list

	if(!CheckParams(4, 4, args, rtypes))
		return false;

	CEGUI::ColourRect rect = caller->ColourRect()->getSubRectangle(args[0].floatValue(), args[1].floatValue(), args[2].floatValue(), args[3].floatValue());
	r_val.assignObject(new sxColourRect(rect), true);
	return true;
}


SX_METHOD_IMPL(getColourAtPoint)
{
	skRValue::RType rtypes[] = { skRValue::T_Float, skRValue::T_Float };

	if(!CheckParams(2, 2, args, rtypes))
		return false;

	CEGUI::colour col = caller->ColourRect()->getColourAtPoint(args[0].floatValue(), args[1].floatValue());
	r_val.assignObject(new sxColour(col), true);
	return true;
}


SX_METHOD_IMPL(setColours)
{
	sxColour *other = IS_COLOUR(args[0]);
	if(!other)
		return false;

	caller->ColourRect()->setColours(*(other->Colour()));
	return true;
}


SX_METHOD_IMPL(modulateAlpha)
{
	caller->ColourRect()->modulateAlpha(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(modulateColours)
{
	sxColourRect *other = IS_COLOURRECT(args[0]);
	if(!other)
		return false;

	*(caller->ColourRect()) *= *(other->ColourRect());
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxColourRect initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXCOLOURRECT_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(setAlpha);
	SX_ADD_METHOD(setTopAlpha);
	SX_ADD_METHOD(setBottomAlpha);
	SX_ADD_METHOD(setLeftAlpha);
	SX_ADD_METHOD(setRightAlpha);
	SX_ADD_METHOD(isMonochromatic);
	SX_ADD_METHOD(getSubRectangle);
	SX_ADD_METHOD(getColourAtPoint);
	SX_ADD_METHOD(setColours);
	SX_ADD_METHOD(modulateAlpha);
	SX_ADD_METHOD(modulateColours);
}

} // namespace RFSX

#undef SX_IMPL_TYPE // And that's it in a nutshell
