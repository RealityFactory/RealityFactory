/************************************************************************************//**
 * @file sxCEGUITooltip.cpp
 * @brief Implementation for RFSX Tooltip object.
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUITooltip.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxTooltip

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxTooltip implementation
//////////////////////////////////////////////////////////////////////////////////////////

bool SX_IMPL_TYPE::method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
{
	if(h_method(this, m_name, args, r_val, ctxt))
		return true;
	else
		return sxWindow::method(m_name, args, r_val, ctxt);
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxTooltip method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(setTargetWindow)
{
	if(args.entries() != 1)
		return false;

	sxWindow *win = IS_WINDOW(args[0]);
	if(!win)
		return false;

	//void setTargetWindow(Window* wnd);
	caller->Tooltip()->setTargetWindow(win->Window());
	return true;
}


SX_METHOD_IMPL(resetTimer)
{
	//void resetTimer();
	caller->Tooltip()->resetTimer();
	return true;
}


SX_METHOD_IMPL(getHoverTime)
{
	//float getHoverTime() const;
	r_val = caller->Tooltip()->getHoverTime();
	return true;
}


SX_METHOD_IMPL(getDisplayTime)
{
	//float getDisplayTime() const;
	r_val = caller->Tooltip()->getDisplayTime();
	return true;
}


SX_METHOD_IMPL(getFadeTime)
{
	//float getFadeTime() const;
	r_val = caller->Tooltip()->getFadeTime();
	return true;
}


SX_METHOD_IMPL(setHoverTime)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Float)
		return false;

	//void setHoverTime(float seconds);
	caller->Tooltip()->setHoverTime(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(setDisplayTime)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Float)
		return false;

	//void setDisplayTime(float seconds);
	caller->Tooltip()->setDisplayTime(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(setFadeTime)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Float)
		return false;

	//void setFadeTime(float seconds);
	caller->Tooltip()->setFadeTime(args[0].floatValue());
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxTooltip initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXTOOLTIP_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(setTargetWindow);
	SX_ADD_METHOD(resetTimer);
	SX_ADD_METHOD(getHoverTime);
	SX_ADD_METHOD(getDisplayTime);
	SX_ADD_METHOD(getFadeTime);
	SX_ADD_METHOD(setHoverTime);
	SX_ADD_METHOD(setDisplayTime);
	SX_ADD_METHOD(setFadeTime);
}

} // namespace RFSX

#undef SX_IMPL_TYPE
