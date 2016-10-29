/************************************************************************************//**
 * @file sxCEGUIUDim.cpp
 * @brief Implementation for RFSX UDim object.
 * @author Daniel Queteschiner
 * @date November 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUIUDim.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxUDim

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxUDim implementation
//////////////////////////////////////////////////////////////////////////////////////////

SX_IMPL_TYPE::SX_IMPL_TYPE()
{
}


SX_IMPL_TYPE::SX_IMPL_TYPE(const CEGUI::UDim& val)
	: m_UDim(val)
{
}


SX_IMPL_TYPE::SX_IMPL_TYPE(float scale, float offset)
{
	m_UDim.d_scale = scale;
	m_UDim.d_offset = offset;
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
	if(other_object->executableType() != RFSXU_UDIM)
		return false;

	const sxUDim *other = static_cast<const sxUDim*>(other_object);

	return (m_UDim == other->UDimConstRef());
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxUDim fields
//////////////////////////////////////////////////////////////////////////////////////////

bool SX_IMPL_TYPE::setValue(const skString& name, const skString& att, const skRValue& val)
{
	if(name == "scale")
	{
		m_UDim.d_scale = val.floatValue();
		return true;
	}
	if(name == "offset")
	{
		m_UDim.d_offset = val.floatValue();
		return true;
	}

	return skExecutable::setValue(name, att, val);
}


bool SX_IMPL_TYPE::getValue(const skString& name, const skString& att, skRValue& val)
{
	if(name == "scale")
	{
		val = m_UDim.d_scale;
		return true;
	}
	if(name == "offset")
	{
		val = m_UDim.d_offset;
		return true;
	}

	return skExecutable::getValue(name, att, val);
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxUDim method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(asAbsolute)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Float)
		return false;

	r_val = caller->UDim()->asAbsolute(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(asRelative)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Float)
		return false;

	r_val = caller->UDim()->asRelative(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(add)
{
	if(args.entries() != 1)
		return false;

	sxUDim *other = IS_UDIM(args[0]);
	if(!other)
		return false;

	*(caller->UDim()) += *(other->UDim());
	return true;
}


SX_METHOD_IMPL(subtract)
{
	if(args.entries() != 1)
		return false;

	sxUDim *other = IS_UDIM(args[0]);
	if(!other)
		return false;

	*(caller->UDim()) -= *(other->UDim());
	return true;
}


SX_METHOD_IMPL(multiply)
{
	if(args.entries() != 1)
		return false;

	sxUDim *other = IS_UDIM(args[0]);
	if(!other)
		return false;

	*(caller->UDim()) *= *(other->UDim());
	return true;
}


SX_METHOD_IMPL(divide)
{
	if(args.entries() != 1)
		return false;

	sxUDim *other = IS_UDIM(args[0]);
	if(!other)
		return false;

	*(caller->UDim()) /= *(other->UDim());
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxUDim initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXUDIM_METHODS);
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
