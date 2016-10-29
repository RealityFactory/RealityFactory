/************************************************************************************//**
 * @file sxCEGUITabButton.cpp
 * @brief Implementation for RFSX TabButton object.
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUITabButton.h"
#include "../sxCEGUIWindow.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxTabButton

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxTabButton implementation
//////////////////////////////////////////////////////////////////////////////////////////

bool SX_IMPL_TYPE::method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
{
	if(h_method(this, m_name, args, r_val, ctxt))
		return true;
	else
		return sxButtonBase::method(m_name, args, r_val, ctxt);
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxTabButton method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(isSelected)
{
	r_val = caller->TabButton()->isSelected();
	return true;
}


SX_METHOD_IMPL(setSelected)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	caller->TabButton()->setSelected(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(setTargetWindow)
{
	if(args.entries() != 1)
		return false;

	sxWindow *win = IS_WINDOW(args[0]);
	if(!win)
		false;

	caller->TabButton()->setTargetWindow(win->Window());
	return true;
}


SX_METHOD_IMPL(getTargetWindow)
{
	r_val.assignObject(new sxWindow(caller->TabButton()->getTargetWindow()), true);
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxTabButton initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXTABBUTTON_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(isSelected);
	SX_ADD_METHOD(setSelected);
	SX_ADD_METHOD(setTargetWindow);
	SX_ADD_METHOD(getTargetWindow);
}

} // namespace RFSX

#undef SX_IMPL_TYPE // And that's it in a nutshell
