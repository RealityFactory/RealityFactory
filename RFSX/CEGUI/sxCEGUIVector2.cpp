/************************************************************************************//**
 * @file sxCEGUIVector2.cpp
 * @brief Implementation for RFSX Vector2/Point object.
 * @author Daniel Queteschiner
 * @date November 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUIVector2.h"
#include "sxCEGUISize.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxVector2

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxVector2 implementation
//////////////////////////////////////////////////////////////////////////////////////////

SX_IMPL_TYPE::SX_IMPL_TYPE()
	: m_Vector2(0.0f, 0.0f)
{
}


SX_IMPL_TYPE::SX_IMPL_TYPE(const CEGUI::Vector2& val)
{
	m_Vector2 = val;
}


SX_IMPL_TYPE::SX_IMPL_TYPE(float x, float y)
	: m_Vector2(x, y)
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
	if(other_object->executableType() != RFSXU_VECTOR2)
		return false;

	const sxVector2 *other = static_cast<const sxVector2*>(other_object);

	return (m_Vector2 == other->Vector2ConstRef());
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxVector2 fields
//////////////////////////////////////////////////////////////////////////////////////////

bool SX_IMPL_TYPE::setValue(const skString& name, const skString& att, const skRValue& val)
{
	if(name == "x")
	{
		m_Vector2.d_x = val.floatValue();
		return true;
	}
	if(name == "y")
	{
		m_Vector2.d_y = val.floatValue();
		return true;
	}

	return skExecutable::setValue(name, att, val);
}


bool SX_IMPL_TYPE::getValue(const skString& name, const skString& att, skRValue& val)
{
	if(name == "x")
	{
		val = m_Vector2.d_x;
		return true;
	}
	if(name == "y")
	{
		val = m_Vector2.d_y;
		return true;
	}

	return skExecutable::getValue(name, att, val);
}


//////////////////////////////////////////////////////////////////////////////////////////
// sxVector2 method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(add)
{
	if(args.entries() != 1)
		return false;

	sxVector2 *other = IS_VECTOR2(args[0]);
	if(!other)
		return false;

	*(caller->Vector2()) += other->Vector2ConstRef();

	return true;
}


SX_METHOD_IMPL(subtract)
{
	if(args.entries() != 1)
		return false;

	sxVector2 *other = IS_VECTOR2(args[0]);
	if(!other)
		return false;

	*(caller->Vector2()) -= other->Vector2ConstRef();

	return true;
}


SX_METHOD_IMPL(asSize)
{
	r_val.assignObject(new sxSize(caller->Vector2()->d_x, caller->Vector2()->d_y), true);
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxVector2 initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXVECTOR2_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(add);
	SX_ADD_METHOD(subtract);
	SX_ADD_METHOD(asSize);
}

} // namespace RFSX

#undef SX_IMPL_TYPE
