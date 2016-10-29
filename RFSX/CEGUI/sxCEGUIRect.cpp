/************************************************************************************//**
 * @file sxCEGUIRect.cpp
 * @brief Implementation for RFSX Rect object.
 * @author Daniel Queteschiner
 * @date November 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUIRect.h"
#include "sxCEGUIVector2.h"
#include "sxCEGUISize.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxRect

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxRect implementation
//////////////////////////////////////////////////////////////////////////////////////////

SX_IMPL_TYPE::SX_IMPL_TYPE()
	: m_Rect(0.0f, 0.0f, 0.0f, 0.0f)
{
}


SX_IMPL_TYPE::SX_IMPL_TYPE(const CEGUI::Rect& val)
	: m_Rect(val)
{
}


SX_IMPL_TYPE::SX_IMPL_TYPE(float left, float top, float right, float bottom)
	: m_Rect(left, top, right, bottom)
{
}


SX_IMPL_TYPE::SX_IMPL_TYPE(CEGUI::Point pos, CEGUI::Size sz)
	: m_Rect(pos, sz)
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
	if(other_object->executableType() != RFSXU_RECT)
		return false;

	const sxRect *other = static_cast<const sxRect*>(other_object);

	return (m_Rect == other->RectConstRef());
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxRect fields
//////////////////////////////////////////////////////////////////////////////////////////

bool SX_IMPL_TYPE::setValue(const skString& name, const skString& att, const skRValue& val)
{
	if(name == "top")
	{
		m_Rect.d_top = val.floatValue();
		return true;
	}
	if(name == "bottom")
	{
		m_Rect.d_bottom = val.floatValue();
		return true;
	}
	if(name == "left")
	{
		m_Rect.d_left = val.floatValue();
		return true;
	}
	if(name == "right")
	{
		m_Rect.d_right = val.floatValue();
		return true;
	}

	return skExecutable::setValue(name, att, val);
}


bool SX_IMPL_TYPE::getValue(const skString& name, const skString& att, skRValue& val)
{
	if(name == "top")
	{
		val = m_Rect.d_top;
		return true;
	}
	if(name == "bottom")
	{
		val = m_Rect.d_bottom;
		return true;
	}
	if(name == "left")
	{
		val = m_Rect.d_left;
		return true;
	}
	if(name == "right")
	{
		val = m_Rect.d_right;
		return true;
	}

	return skExecutable::getValue(name, att, val);
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxRect method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(getPosition)
{
	r_val.assignObject(new sxVector2(caller->Rect()->getPosition()), true);
	return true;
}


SX_METHOD_IMPL(getWidth)
{
	r_val = caller->Rect()->getWidth();
	return true;
}


SX_METHOD_IMPL(getHeight)
{
	r_val = caller->Rect()->getHeight();
	return true;
}


SX_METHOD_IMPL(getSize)
{
	r_val.assignObject(new sxSize(caller->Rect()->getSize()), true);
	return true;
}


SX_METHOD_IMPL(setPosition)
{
	if(args.entries() != 1)
		return false;

	sxPoint *other = IS_POINT(args[0]);
	if(!other)
		return false;

	caller->Rect()->setPosition(other->PointConstRef());
	return true;
}


SX_METHOD_IMPL(setWidth)
{
	if(args.entries() != 1)
		return false;

	caller->Rect()->setWidth(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(setHeight)
{
	if(args.entries() != 1)
		return false;

	caller->Rect()->setHeight(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(setSize)
{
	if(args.entries() != 1)
		return false;

	sxSize *sz = IS_SIZE(args[0]);
	if(!sz)
		return false;

	caller->Rect()->setSize(sz->SizeConstRef());
	return true;
}


SX_METHOD_IMPL(getIntersection)
{
	if(args.entries() != 1)
		return false;

	sxRect *other = IS_RECT(args[0]);
	if(!other)
		return false;

	r_val.assignObject(new sxRect(caller->Rect()->getIntersection(other->RectConstRef())), true);
	return true;
}


SX_METHOD_IMPL(isPointInRect)
{
	if(args.entries() != 1)
		return false;

	sxPoint *other = IS_POINT(args[0]);
	if(!other)
		return false;

	r_val = caller->Rect()->isPointInRect(other->PointConstRef());

	return true;
}


SX_METHOD_IMPL(offset)
{
	if(args.entries() != 1)
		return false;

	sxVector2 *other = IS_VECTOR2(args[0]);
	if(!other)
		return false;

	r_val.assignObject(new sxRect(caller->Rect()->offset(other->Vector2ConstRef())), true);
	return true;
}


SX_METHOD_IMPL(constrainSizeMax)
{
	if(args.entries() != 1)
		return false;

	sxSize *other = IS_SIZE(args[0]);
	if(!other)
		return false;

	r_val.assignObject(new sxRect(caller->Rect()->constrainSizeMax(other->SizeConstRef())), true);
	return true;
}


SX_METHOD_IMPL(constrainSizeMin)
{
	if(args.entries() != 1)
		return false;

	sxSize *other = IS_SIZE(args[0]);
	if(!other)
		return false;

	r_val.assignObject(new sxRect(caller->Rect()->constrainSizeMin(other->SizeConstRef())), true);
	return true;
}


SX_METHOD_IMPL(constrainSize)
{
	if(args.entries() != 2)
		return false;

	sxSize *max = IS_SIZE(args[0]);
	sxSize *min = IS_SIZE(args[1]);

	if(!max || !min)
		return false;

	r_val.assignObject(new sxRect(caller->Rect()->constrainSize(max->SizeConstRef(), min->SizeConstRef())), true);
	return true;
}


SX_METHOD_IMPL(scale)
{
	*(caller->Rect()) *= args[0].floatValue();
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxRect initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXRECT_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(getPosition);
	SX_ADD_METHOD(getWidth);
	SX_ADD_METHOD(getHeight);
	SX_ADD_METHOD(getSize);
	SX_ADD_METHOD(setPosition);
	SX_ADD_METHOD(setWidth);
	SX_ADD_METHOD(setHeight);
	SX_ADD_METHOD(setSize);
	SX_ADD_METHOD(getIntersection);
	SX_ADD_METHOD(offset);
	SX_ADD_METHOD(isPointInRect);
	SX_ADD_METHOD(constrainSizeMax);
	SX_ADD_METHOD(constrainSizeMin);
	SX_ADD_METHOD(constrainSize);
	SX_ADD_METHOD(scale);
}

} // namespace RFSX

#undef SX_IMPL_TYPE
