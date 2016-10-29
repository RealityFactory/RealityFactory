/************************************************************************************//**
 * @file sxCEGUIListHeaderSegment.cpp
 * @brief Implementation for RFSX ListHeaderSegment object.
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUIListHeaderSegment.h"
#include "../sxCEGUIVector2.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxListHeaderSegment

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxListHeaderSegment implementation
//////////////////////////////////////////////////////////////////////////////////////////

bool SX_IMPL_TYPE::method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
{
	if(h_method(this, m_name, args, r_val, ctxt))
		return true;
	else
		return sxWindow::method(m_name, args, r_val, ctxt);
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxListHeaderSegment method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(isSizingEnabled)
{
	//bool isSizingEnabled() const;
	r_val = caller->ListHeaderSegment()->isSizingEnabled();
	return true;
}


SX_METHOD_IMPL(getSortDirection)
{
	//SortDirection getSortDirection() const;
	r_val = caller->ListHeaderSegment()->getSortDirection();
	return true;
}


SX_METHOD_IMPL(isDragMovingEnabled)
{
	//bool isDragMovingEnabled() const;
	r_val = caller->ListHeaderSegment()->isDragMovingEnabled();
	return true;
}


SX_METHOD_IMPL(getDragMoveOffset)
{
	//const Vector2& getDragMoveOffset() const;
	r_val.assignObject(new sxVector2(caller->ListHeaderSegment()->getDragMoveOffset()), true);
	return true;
}


SX_METHOD_IMPL(isClickable)
{
	//bool isClickable() const;
	r_val = caller->ListHeaderSegment()->isClickable();
	return true;
}


SX_METHOD_IMPL(setSizingEnabled)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	//void setSizingEnabled(bool setting);
	caller->ListHeaderSegment()->setSizingEnabled(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(setSortDirection)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	//void setSortDirection(SortDirection sort_dir);
	caller->ListHeaderSegment()->setSortDirection(static_cast<CEGUI::ListHeaderSegment::SortDirection>(args[0].intValue()));
	return true;
}


SX_METHOD_IMPL(setDragMovingEnabled)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	//void setDragMovingEnabled(bool setting);
	caller->ListHeaderSegment()->setDragMovingEnabled(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(setClickable)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	//void setClickable(bool setting);
	caller->ListHeaderSegment()->setClickable(args[0].boolValue());
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxListHeaderSegment initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXLISTHEADERSEGMENT_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(isSizingEnabled);
	SX_ADD_METHOD(getSortDirection);
	SX_ADD_METHOD(isDragMovingEnabled);
	SX_ADD_METHOD(getDragMoveOffset);
	SX_ADD_METHOD(isClickable);
	SX_ADD_METHOD(setSizingEnabled);
	SX_ADD_METHOD(setSortDirection);
	SX_ADD_METHOD(setDragMovingEnabled);
	SX_ADD_METHOD(setClickable);
}

} // namespace RFSX

#undef SX_IMPL_TYPE // And that's it in a nutshell
