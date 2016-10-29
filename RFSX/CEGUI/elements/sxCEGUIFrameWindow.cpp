/************************************************************************************//**
 * @file sxCEGUIFrameWindow.cpp
 * @brief Implementation for RFSX FrameWindow object.
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUIFrameWindow.h"
#include "../sxCEGUIVector2.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxFrameWindow

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxFrameWindow implementation
//////////////////////////////////////////////////////////////////////////////////////////

bool SX_IMPL_TYPE::method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
{
	if(h_method(this, m_name, args, r_val, ctxt))
		return true;
	else
		return sxWindow::method(m_name, args, r_val, ctxt);
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxFrameWindow method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(isSizingEnabled)
{
	//bool isSizingEnabled() const;
	r_val = caller->FrameWindow()->isSizingEnabled();
	return true;
}


SX_METHOD_IMPL(isFrameEnabled)
{
	//bool isFrameEnabled() const;
	r_val = caller->FrameWindow()->isFrameEnabled();
	return true;
}


SX_METHOD_IMPL(isTitleBarEnabled)
{
	//bool isTitleBarEnabled() const;
	r_val = caller->FrameWindow()->isTitleBarEnabled();
	return true;
}


SX_METHOD_IMPL(isCloseButtonEnabled)
{
	//bool isCloseButtonEnabled() const;
	r_val = caller->FrameWindow()->isCloseButtonEnabled();
	return true;
}


SX_METHOD_IMPL(isRollupEnabled)
{
	//bool isRollupEnabled() const;
	r_val = caller->FrameWindow()->isRollupEnabled();
	return true;
}


SX_METHOD_IMPL(isRolledup)
{
	//bool isRolledup() const;
	r_val = caller->FrameWindow()->isRolledup();
	return true;
}


SX_METHOD_IMPL(isDragMovingEnabled)
{
	//bool isDragMovingEnabled() const;
	r_val = caller->FrameWindow()->isDragMovingEnabled();
	return true;
}


SX_METHOD_IMPL(setSizingEnabled)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	//void setSizingEnabled(bool enabled);
	caller->FrameWindow()->setSizingEnabled(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(setFrameEnabled)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	//void setFrameEnabled(bool enabled);
	caller->FrameWindow()->setFrameEnabled(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(setTitleBarEnabled)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	//void setTitleBarEnabled(bool enabled);
	caller->FrameWindow()->setTitleBarEnabled(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(setCloseButtonEnabled)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	//void setCloseButtonEnabled(bool enabled);
	caller->FrameWindow()->setCloseButtonEnabled(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(setRollupEnabled)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	//void setRollupEnabled(bool enabled);
	caller->FrameWindow()->setRollupEnabled(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(setDragMovingEnabled)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	//void setDragMovingEnabled(bool enabled);
	caller->FrameWindow()->setDragMovingEnabled(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(getSizingBorderThickness)
{
	//float getSizingBorderThickness() const;
	r_val = caller->FrameWindow()->getSizingBorderThickness();
	return true;
}


SX_METHOD_IMPL(setSizingBorderThickness)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Float)
		return false;

	//void setSizingBorderThickness(float pixels);
	caller->FrameWindow()->setSizingBorderThickness(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(toggleRollup)
{
	//void toggleRollup();
	caller->FrameWindow()->toggleRollup();
	return true;
}


SX_METHOD_IMPL(offsetPixelPosition)
{
	if(args.entries() != 1)
		return false;

	sxVector2 *vec2 = IS_VECTOR2(args[0]);
	if(!vec2)
		return false;

	//void offsetPixelPosition(const Vector2& offset);
	caller->FrameWindow()->offsetPixelPosition(vec2->Vector2ConstRef());
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxFrameWindow initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXFRAMEWINDOW_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(isSizingEnabled);
	SX_ADD_METHOD(isFrameEnabled);
	SX_ADD_METHOD(isTitleBarEnabled);
	SX_ADD_METHOD(isCloseButtonEnabled);
	SX_ADD_METHOD(isRollupEnabled);
	SX_ADD_METHOD(isRolledup);
	SX_ADD_METHOD(isDragMovingEnabled);
	SX_ADD_METHOD(setSizingEnabled);
	SX_ADD_METHOD(setFrameEnabled);
	SX_ADD_METHOD(setTitleBarEnabled);
	SX_ADD_METHOD(setCloseButtonEnabled);
	SX_ADD_METHOD(setRollupEnabled);
	SX_ADD_METHOD(setDragMovingEnabled);
	SX_ADD_METHOD(getSizingBorderThickness);
	SX_ADD_METHOD(setSizingBorderThickness);
	SX_ADD_METHOD(toggleRollup);
	SX_ADD_METHOD(offsetPixelPosition);
}

} // namespace RFSX

#undef SX_IMPL_TYPE
