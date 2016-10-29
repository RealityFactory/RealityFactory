/************************************************************************************//**
 * @file sxCEGUISize.cpp
 * @brief Implementation for RFSX Size object.
 * @author Daniel Queteschiner
 * @date November 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUISize.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxSize

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxSize implementation
//////////////////////////////////////////////////////////////////////////////////////////

SX_IMPL_TYPE::SX_IMPL_TYPE()
{
}


SX_IMPL_TYPE::SX_IMPL_TYPE(const CEGUI::Size& val)
	: m_Size(val)
{
}


SX_IMPL_TYPE::SX_IMPL_TYPE(float width, float height)
	: m_Size(width, height)
{
}


SX_IMPL_TYPE::~SX_IMPL_TYPE()
{
}


bool SX_IMPL_TYPE::equals(const skiExecutable *other_object) const
{
	if(other_object->executableType() != RFSXU_SIZE)
		return false;

	const sxSize *other = static_cast<const sxSize*>(other_object);

	return (m_Size == other->SizeConstRef());
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxSize fields
//////////////////////////////////////////////////////////////////////////////////////////

bool SX_IMPL_TYPE::setValue(const skString& name, const skString& att, const skRValue& val)
{
	if(name == "width")
	{
		m_Size.d_width = val.floatValue();
		return true;
	}
	if(name == "height")
	{
		m_Size.d_height = val.floatValue();
		return true;
	}

	return skExecutable::setValue(name, att, val);
}


bool SX_IMPL_TYPE::getValue(const skString& name, const skString& att, skRValue& val)
{
	if(name == "width")
	{
		val = m_Size.d_width;
		return true;
	}
	if(name == "height")
	{
		val = m_Size.d_height;
		return true;
	}

	return skExecutable::getValue(name, att, val);
}

} // namespace RFSX

#undef SX_IMPL_TYPE // And that's it in a nutshell
