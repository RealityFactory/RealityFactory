/************************************************************************************//**
 * @file sxCEGUIURect.cpp
 * @brief Implementation for RFSX URect object.
 * @author Daniel Queteschiner
 * @date November 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUIURect.h"
#include "sxCEGUIUDim.h"
#include "sxCEGUIUVector2.h"
#include "sxCEGUIRect.h"
#include "sxCEGUISize.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxURect

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxURect implementation
//////////////////////////////////////////////////////////////////////////////////////////

SX_IMPL_TYPE::SX_IMPL_TYPE()
{
}

SX_IMPL_TYPE::SX_IMPL_TYPE(const CEGUI::URect& urect)
	: m_URect(urect.d_min, urect.d_max)
{
}

SX_IMPL_TYPE::SX_IMPL_TYPE(const CEGUI::UVector2& min, const CEGUI::UVector2& max)
	: m_URect(min, max)
{
}


SX_IMPL_TYPE::SX_IMPL_TYPE(
	const CEGUI::UDim& left,
	const CEGUI::UDim& top,
	const CEGUI::UDim& right,
	const CEGUI::UDim& bottom)
	: m_URect(left, top, right, bottom)
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


bool SX_IMPL_TYPE::setValue(const skString& name, const skString& att, const skRValue& val)
{
	skRValue value = val;
	if(name == "min")
	{
		sxUVector2 *uvec = IS_UVECTOR2(value);
		if(!uvec)
			return false;

		m_URect.d_min = uvec->UVector2ConstRef();
		return true;
	}
	if(name == "max")
	{
		sxUVector2 *uvec = IS_UVECTOR2(value);
		if(!uvec)
			return false;

		m_URect.d_max = uvec->UVector2ConstRef();
		return true;
	}

	return skExecutable::setValue(name, att, val);
}


bool SX_IMPL_TYPE::getValue(const skString& name, const skString& att, skRValue& val)
{
	if(name == "min")
	{
		val.assignObject(new sxUVector2(m_URect.d_min), true);
		return true;
	}
	if(name == "max")
	{
		val.assignObject(new sxUVector2(m_URect.d_max), true);
		return true;
	}

	return skExecutable::getValue(name, att, val);
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxURect method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(asAbsolute)
{
	if(args.entries() != 1)
		return false;

	sxSize* sz = IS_SIZE(args[0]);
	if(!sz)
		return false;

	r_val.assignObject(new sxRect(caller->URect()->asAbsolute(sz->SizeConstRef())), true);
	return true;
}


SX_METHOD_IMPL(asRelative)
{
	if(args.entries() != 1)
		return false;

	sxSize* sz = IS_SIZE(args[0]);
	if(!sz)
		return false;

	r_val.assignObject(new sxRect(caller->URect()->asRelative(sz->SizeConstRef())), true);
	return true;
}


SX_METHOD_IMPL(getPosition)
{
	r_val.assignObject(new sxUVector2(caller->URect()->getPosition()), true);
	return true;
}


SX_METHOD_IMPL(getWidth)
{
	r_val.assignObject(new sxUDim(caller->URect()->getWidth()), true);
	return true;
}


SX_METHOD_IMPL(getHeight)
{
	r_val.assignObject(new sxUDim(caller->URect()->getHeight()), true);
	return true;
}


SX_METHOD_IMPL(getSize)
{
	r_val.assignObject(new sxUVector2(caller->URect()->getSize()), true);
	return true;
}


SX_METHOD_IMPL(setPosition)
{
	if(args.entries() != 1)
		return false;

	sxUVector2 *pos = IS_UVECTOR2(args[0]);
	if(!pos)
		return false;

	caller->URect()->setPosition(pos->UVector2ConstRef());
	return true;
}


SX_METHOD_IMPL(setWidth)
{
	if(args.entries() != 1)
		return false;

	sxUDim *w = IS_UDIM(args[0]);
	if(!w)
		return false;

	caller->URect()->setWidth(w->UDimConstRef());
	return true;
}


SX_METHOD_IMPL(setHeight)
{
	if(args.entries() != 1)
		return false;

	sxUDim *h = IS_UDIM(args[0]);
	if(!h)
		return false;

	caller->URect()->setHeight(h->UDimConstRef());
	return true;
}


SX_METHOD_IMPL(setSize)
{
	if(args.entries() != 1)
		return false;

	sxUVector2 *sz = IS_UVECTOR2(args[0]);
	if(!sz)
		return false;

	caller->URect()->setSize(sz->UVector2ConstRef());
	return true;
}


SX_METHOD_IMPL(offset)
{
	if(args.entries() != 1)
		return false;

	sxUVector2 *sz = IS_UVECTOR2(args[0]);
	if(!sz)
		return false;

	caller->URect()->offset(sz->UVector2ConstRef());
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxURect initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXURECT_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	// Add all methods here
	SX_ADD_METHOD(asAbsolute);
	SX_ADD_METHOD(asRelative);
	SX_ADD_METHOD(getPosition);
	SX_ADD_METHOD(getWidth);
	SX_ADD_METHOD(getHeight);
	SX_ADD_METHOD(getSize);
	SX_ADD_METHOD(setPosition);
	SX_ADD_METHOD(setWidth);
	SX_ADD_METHOD(setHeight);
	SX_ADD_METHOD(setSize);
	SX_ADD_METHOD(offset);
}

} // namespace RFSX

#undef SX_IMPL_TYPE // And that's it in a nutshell
