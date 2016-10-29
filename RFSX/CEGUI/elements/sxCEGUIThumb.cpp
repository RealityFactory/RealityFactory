/************************************************************************************//**
 * @file sxCEGUIThumb.cpp
 * @brief Implementation for RFSX Thumb object.
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUIThumb.h"
#include "../sxCEGUIVector2.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxThumb

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxThumb implementation
//////////////////////////////////////////////////////////////////////////////////////////

bool SX_IMPL_TYPE::method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
{
	if(h_method(this, m_name, args, r_val, ctxt))
		return true;
	else
		return sxPushButton::method(m_name, args, r_val, ctxt);
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxThumb method functions -- add actual method processing code here
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(isHotTracked)
{
	r_val = caller->Thumb()->isHotTracked();
	return true;
}


SX_METHOD_IMPL(isVertFree)
{
	r_val = caller->Thumb()->isVertFree();
	return true;
}


SX_METHOD_IMPL(isHorzFree)
{
	r_val = caller->Thumb()->isHorzFree();
	return true;
}


SX_METHOD_IMPL(setHotTracked)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	caller->Thumb()->setHotTracked(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(setVertFree)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	caller->Thumb()->setVertFree(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(setHorzFree)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	caller->Thumb()->setHorzFree(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(getVertRange)
{
	std::pair<float,float> range_pair = caller->Thumb()->getVertRange();
	r_val.assignObject(new sxVector2(range_pair.first, range_pair.second), true);
	return true;
}


SX_METHOD_IMPL(getHorzRange)
{
	std::pair<float,float> range_pair = caller->Thumb()->getHorzRange();
	r_val.assignObject(new sxVector2(range_pair.first, range_pair.second), true);
	return true;
}


SX_METHOD_IMPL(setVertRange)
{
	skRValue::RType rtypes[] = { skRValue::T_Float, skRValue::T_Float };

	if(!CheckParams(2, 2, args, rtypes))
		return false;

	caller->Thumb()->setVertRange(args[0].floatValue(), args[1].floatValue());
	return true;
}


SX_METHOD_IMPL(setHorzRange)
{
	skRValue::RType rtypes[] = { skRValue::T_Float, skRValue::T_Float };

	if(!CheckParams(2, 2, args, rtypes))
		return false;

	caller->Thumb()->setHorzRange(args[0].floatValue(), args[1].floatValue());
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxThumb initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXTHUMB_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(isHotTracked);
	SX_ADD_METHOD(isVertFree);
	SX_ADD_METHOD(isHorzFree);
	SX_ADD_METHOD(setHotTracked);
	SX_ADD_METHOD(setVertFree);
	SX_ADD_METHOD(setHorzFree);
	SX_ADD_METHOD(getVertRange);
	SX_ADD_METHOD(getHorzRange);
	SX_ADD_METHOD(setVertRange);
	SX_ADD_METHOD(setHorzRange);
}

} // namespace RFSX

#undef SX_IMPL_TYPE // And that's it in a nutshell
