/************************************************************************************//**
 * @file sxCEGUIMCLGridRef.cpp
 * @brief Implementation for RFSX MCLGridRef object.
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUIMCLGridRef.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxMCLGridRef

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxMCLGridRef implementation
//////////////////////////////////////////////////////////////////////////////////////////
SX_IMPL_TYPE::SX_IMPL_TYPE(unsigned int r, unsigned int c)
	: m_MCLGridRef(r, c)
{
}


SX_IMPL_TYPE::SX_IMPL_TYPE(const CEGUI::MCLGridRef& gridRef)
	: m_MCLGridRef(gridRef)
{
}


SX_IMPL_TYPE::~SX_IMPL_TYPE()
{
}


bool SX_IMPL_TYPE::equals(const skiExecutable *other_object) const
{
	if(other_object->executableType() != RFSXU_MCLGRIDREF)
		return false;

	const sxMCLGridRef *other = static_cast<const sxMCLGridRef*>(other_object);

	return (m_MCLGridRef == other->MCLGridRefConstRef());
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxMCLGridRef fields
//////////////////////////////////////////////////////////////////////////////////////////

bool SX_IMPL_TYPE::setValue(const skString& name, const skString& att, const skRValue& val)
{
	if(name == "row")
	{
		MCLGridRef()->row = val.intValue();
		return true;
	}
	if(name == "column")
	{
		MCLGridRef()->column = val.intValue();
		return true;
	}

	return skExecutable::setValue(name, att, val);
}


bool SX_IMPL_TYPE::getValue(const skString& name, const skString& att, skRValue& val)
{
	if(name == "row")
	{
		val = static_cast<int>(MCLGridRef()->row);
		return true;
	}
	if(name == "column")
	{
		val = static_cast<int>(MCLGridRef()->column);
		return true;
	}

	return skExecutable::getValue(name, att, val);
}

} // namespace RFSX

#undef SX_IMPL_TYPE
