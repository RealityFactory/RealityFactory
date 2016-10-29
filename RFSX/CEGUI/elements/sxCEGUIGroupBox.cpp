/************************************************************************************//**
 * @file sxCEGUIGroupBox.cpp
 * @brief Implementation for RFSX GroupBox object.
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUIGroupBox.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxGroupBox

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxGroupBox implementation
//////////////////////////////////////////////////////////////////////////////////////////

bool SX_IMPL_TYPE::method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
{
	if(h_method(this, m_name, args, r_val, ctxt))
		return true;
	else
		return sxWindow::method(m_name, args, r_val, ctxt);
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxGroupBox method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(drawAroundWidget)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	//bool drawAroundWidget(string name);
	r_val = caller->GroupBox()->drawAroundWidget(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxGroupBox initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXGROUPBOX_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(drawAroundWidget);
}

} // namespace RFSX

#undef SX_IMPL_TYPE
