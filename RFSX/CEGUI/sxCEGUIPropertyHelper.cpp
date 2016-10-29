/************************************************************************************//**
 * @file sxCEGUIPropertyHelper.cpp
 * @brief Implementation for RFSX PropertyHelper object.
 * @author Daniel Queteschiner
 * @date December 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUIPropertyHelper.h"
#include "sxCEGUISize.h"
#include "sxCEGUIVector2.h"
#include "sxCEGUIRect.h"
#include "sxCEGUIImage.h"
#include "sxCEGUIColour.h"
#include "sxCEGUIColourRect.h"
#include "sxCEGUIUDim.h"
#include "sxCEGUIUVector2.h"
#include "sxCEGUIURect.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxPropertyHelper

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxPropertyHelper implementation
//////////////////////////////////////////////////////////////////////////////////////////

SX_IMPL_TYPE::SX_IMPL_TYPE()
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

//////////////////////////////////////////////////////////////////////////////////////////
// sxPropertyHelper method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(stringToFloat)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	CEGUI::String str(args[0].str().c_str());
	r_val = CEGUI::PropertyHelper::stringToFloat(str);
	return true;
}


SX_METHOD_IMPL(stringToBool)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	CEGUI::String str(args[0].str().c_str());
	r_val = CEGUI::PropertyHelper::stringToBool(str);
	return true;
}


SX_METHOD_IMPL(stringToSize)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	CEGUI::String str(args[0].str().c_str());
	r_val.assignObject(new sxSize(CEGUI::PropertyHelper::stringToSize(str)), true);
	return true;
}


SX_METHOD_IMPL(stringToPoint)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	CEGUI::String str(args[0].str().c_str());
	r_val.assignObject(new sxPoint(CEGUI::PropertyHelper::stringToPoint(str)), true);
	return true;
}


SX_METHOD_IMPL(stringToRect)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	CEGUI::String str(args[0].str().c_str());
	r_val.assignObject(new sxRect(CEGUI::PropertyHelper::stringToRect(str)), true);
	return true;
}


SX_METHOD_IMPL(stringToImage)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	CEGUI::String str(args[0].str().c_str());
	const CEGUI::Image *image = CEGUI::PropertyHelper::stringToImage(str);

	if(image)
		r_val.assignObject(new sxImage(image), true);
	else
		r_val.assignObject(&ctxt.getInterpreter()->getNull());

	return true;
}


SX_METHOD_IMPL(stringToColour)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	CEGUI::String str(args[0].str().c_str());
	r_val.assignObject(new sxColour(CEGUI::PropertyHelper::stringToColour(str)), true);
	return true;
}


SX_METHOD_IMPL(stringToColourRect)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	CEGUI::String str(args[0].str().c_str());
	r_val.assignObject(new sxColourRect(CEGUI::PropertyHelper::stringToColourRect(str)), true);
	return true;
}


SX_METHOD_IMPL(stringToUDim)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	CEGUI::String str(args[0].str().c_str());
	r_val.assignObject(new sxUDim(CEGUI::PropertyHelper::stringToUDim(str)), true);
	return true;
}


SX_METHOD_IMPL(stringToUVector2)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	CEGUI::String str(args[0].str().c_str());
	r_val.assignObject(new sxUVector2(CEGUI::PropertyHelper::stringToUVector2(str)), true);
	return true;
}


SX_METHOD_IMPL(stringToURect)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	CEGUI::String str(args[0].str().c_str());
	r_val.assignObject(new sxURect(CEGUI::PropertyHelper::stringToURect(str)), true);
	return true;
}


SX_METHOD_IMPL(floatToString)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Float)
		return false;

	CEGUI::String str = CEGUI::PropertyHelper::floatToString(args[0].floatValue());
	r_val = skString(str.c_str());
	return true;
}


SX_METHOD_IMPL(boolToString)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	CEGUI::String str = CEGUI::PropertyHelper::boolToString(args[0].boolValue());
	r_val = skString(str.c_str());
	return true;
}


SX_METHOD_IMPL(sizeToString)
{
	if(args.entries() != 1)
		return false;

	sxSize *sz = IS_SIZE(args[0]);
	if(!sz)
		return false;

	CEGUI::String str = CEGUI::PropertyHelper::sizeToString(sz->SizeConstRef());
	r_val = skString(str.c_str());
	return true;
}


SX_METHOD_IMPL(pointToString)
{
	if(args.entries() != 1)
		return false;

	sxPoint *point = IS_POINT(args[0]);
	if(!point)
		return false;

	CEGUI::String str = CEGUI::PropertyHelper::pointToString(point->PointConstRef());
	r_val = skString(str.c_str());
	return true;
}


SX_METHOD_IMPL(rectToString)
{
	if(args.entries() != 1)
		return false;

	sxRect *rect = IS_RECT(args[0]);
	if(!rect)
		return false;

	CEGUI::String str = CEGUI::PropertyHelper::rectToString(rect->RectConstRef());
	r_val = skString(str.c_str());
	return true;
}


SX_METHOD_IMPL(imageToString)
{
	if(args.entries() != 1)
		return false;

	sxImage *image = IS_IMAGE(args[0]);
	if(!image)
		return false;

	CEGUI::String str = CEGUI::PropertyHelper::imageToString(image->Image());
	r_val = skString(str.c_str());
	return true;
}


SX_METHOD_IMPL(colourToString)
{
	if(args.entries() != 1)
		return false;

	sxColour *col = IS_COLOUR(args[0]);
	if(!col)
		return false;

	CEGUI::String str = CEGUI::PropertyHelper::colourToString(col->ColourConstRef());
	r_val = skString(str.c_str());
	return true;
}


SX_METHOD_IMPL(colourRectToString)
{
	if(args.entries() != 1)
		return false;

	sxColourRect *crect = IS_COLOURRECT(args[0]);
	if(!crect)
		return false;

	CEGUI::String str = CEGUI::PropertyHelper::colourRectToString(crect->ColourRectConstRef());
	r_val = skString(str.c_str());
	return true;
}


SX_METHOD_IMPL(udimToString)
{
	if(args.entries() != 1)
		return false;

	sxUDim *udim = IS_UDIM(args[0]);
	if(!udim)
		return false;

	CEGUI::String str = CEGUI::PropertyHelper::udimToString(udim->UDimConstRef());
	r_val = skString(str.c_str());
	return true;
}


SX_METHOD_IMPL(uvector2ToString)
{
	if(args.entries() != 1)
		return false;

	sxUVector2 *uvec = IS_UVECTOR2(args[0]);
	if(!uvec)
		return false;

	CEGUI::String str = CEGUI::PropertyHelper::uvector2ToString(uvec->UVector2ConstRef());
	r_val = skString(str.c_str());
	return true;
}


SX_METHOD_IMPL(urectToString)
{
	if(args.entries() != 1)
		return false;

	sxURect *urect = IS_URECT(args[0]);
	if(!urect)
		return false;

	CEGUI::String str = CEGUI::PropertyHelper::urectToString(urect->URectConstRef());
	r_val = skString(str.c_str());
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxPropertyHelper initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXPROPERTYHELPER_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(stringToFloat);
	SX_ADD_METHOD(stringToBool);
	SX_ADD_METHOD(stringToSize);
	SX_ADD_METHOD(stringToPoint);
	SX_ADD_METHOD(stringToRect);
	SX_ADD_METHOD(stringToImage);
	SX_ADD_METHOD(stringToColour);
	SX_ADD_METHOD(stringToColourRect);
	SX_ADD_METHOD(stringToUDim);
	SX_ADD_METHOD(stringToUVector2);
	SX_ADD_METHOD(stringToURect);

	SX_ADD_METHOD(floatToString);
	SX_ADD_METHOD(boolToString);
	SX_ADD_METHOD(sizeToString);
	SX_ADD_METHOD(pointToString);
	SX_ADD_METHOD(rectToString);
	SX_ADD_METHOD(imageToString);
	SX_ADD_METHOD(colourToString);
	SX_ADD_METHOD(colourRectToString);
	SX_ADD_METHOD(udimToString);
	SX_ADD_METHOD(uvector2ToString);
	SX_ADD_METHOD(urectToString);
}

} // namespace RFSX

#undef SX_IMPL_TYPE
