/************************************************************************************//**
 * @file sxCEGUITitlebar.cpp
 * @brief Implementation for RFSX Titlebar object.
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUITitlebar.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxTitlebar

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxTitlebar implementation
//////////////////////////////////////////////////////////////////////////////////////////

bool SX_IMPL_TYPE::method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
{
	if(h_method(this, m_name, args, r_val, ctxt))
		return true;
	else
		return sxWindow::method(m_name, args, r_val, ctxt);
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxTitlebar method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(isDraggingEnabled)
{
	r_val = caller->Titlebar()->isDraggingEnabled();
	return true;
}

SX_METHOD_IMPL(setDraggingEnabled)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	caller->Titlebar()->setDraggingEnabled(args[0].boolValue());
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxTitlebar initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXTITLEBAR_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(isDraggingEnabled);
	SX_ADD_METHOD(setDraggingEnabled);
}

} // namespace RFSX

#undef SX_IMPL_TYPE
