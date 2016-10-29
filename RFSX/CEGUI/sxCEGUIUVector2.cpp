/************************************************************************************//**
 * @file sxCEGUIUVector2.cpp
 * @brief Implementation for RFSX UVector2 object.
 * @author Daniel Queteschiner
 * @date November 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUIUVector2.h"
#include "sxCEGUIUDim.h"
#include "sxCEGUISize.h"
#include "sxCEGUIVector2.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxUVector2

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxUVector2 implementation
//////////////////////////////////////////////////////////////////////////////////////////

SX_IMPL_TYPE::SX_IMPL_TYPE()
{
}


SX_IMPL_TYPE::SX_IMPL_TYPE(const CEGUI::UVector2& val)
	: m_UVector2(val)
{
}


SX_IMPL_TYPE::SX_IMPL_TYPE(const CEGUI::UDim& x, const CEGUI::UDim& y)
	: m_UVector2(x, y)
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


bool SX_IMPL_TYPE::equals(const skiExecutable *other_object) const
{
	if(other_object->executableType() != RFSXU_UVECTOR2)
		return false;

	const sxUVector2 *other = static_cast<const sxUVector2*>(other_object);

	return (m_UVector2 == other->UVector2ConstRef());
}


bool SX_IMPL_TYPE::setValue(const skString& name, const skString& att, const skRValue& val)
{
	skRValue value = val;
	if(name == "x")
	{
		sxUDim *udim = IS_UDIM(value);
		if(!udim)
			return false;

		m_UVector2.d_x = udim->UDimConstRef();
		return true;
	}
	if(name == "y")
	{
		sxUDim *udim = IS_UDIM(value);
		if(!udim)
			return false;

		m_UVector2.d_y = udim->UDimConstRef();
		return true;
	}

	return skExecutable::setValue(name, att, val);
}


bool SX_IMPL_TYPE::getValue(const skString& name, const skString& att, skRValue& val)
{
	if(name == "x")
	{
		val.assignObject(new sxUDim(m_UVector2.d_x), true);
		return true;
	}
	if(name == "y")
	{
		val.assignObject(new sxUDim(m_UVector2.d_y), true);
		return true;
	}

	return skExecutable::getValue(name, att, val);
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxUVector2 method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(asAbsolute)
{
	if(args.entries() != 1)
		return false;

	sxSize* sz = IS_SIZE(args[0]);
	if(!sz)
		return false;

	r_val.assignObject(new sxVector2(caller->UVector2()->asAbsolute(sz->SizeConstRef())), true);
	return true;
}


SX_METHOD_IMPL(asRelative)
{
	if(args.entries() != 1)
		return false;

	sxSize* sz = IS_SIZE(args[0]);
	if(!sz)
		return false;

	r_val.assignObject(new sxVector2(caller->UVector2()->asRelative(sz->SizeConstRef())), true);
	return true;
}


SX_METHOD_IMPL(add)
{
	if(args.entries() != 1)
		return false;

	sxUVector2 *other = IS_UVECTOR2(args[0]);
	if(!other)
		return false;

	*(caller->UVector2()) += other->UVector2ConstRef();
	return true;
}


SX_METHOD_IMPL(subtract)
{
	if(args.entries() != 1)
		return false;

	sxUVector2 *other = IS_UVECTOR2(args[0]);
	if(!other)
		return false;

	*(caller->UVector2()) -= other->UVector2ConstRef();
	return true;
}


SX_METHOD_IMPL(multiply)
{
	if(args.entries() != 1)
		return false;

	sxUVector2 *other = IS_UVECTOR2(args[0]);
	if(!other)
		return false;

	*(caller->UVector2()) *= other->UVector2ConstRef();
	return true;
}


SX_METHOD_IMPL(divide)
{
	if(args.entries() != 1)
		return false;

	sxUVector2 *other = IS_UVECTOR2(args[0]);
	if(!other)
		return false;

	*(caller->UVector2()) /= other->UVector2ConstRef();
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxUVector2 initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXUVECTOR2_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(asAbsolute);
	SX_ADD_METHOD(asRelative);
	SX_ADD_METHOD(add);
	SX_ADD_METHOD(subtract);
	SX_ADD_METHOD(multiply);
	SX_ADD_METHOD(divide);
}

} // namespace RFSX

#undef SX_IMPL_TYPE // And that's it in a nutshell
