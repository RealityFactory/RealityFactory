/************************************************************************************//**
 * @file sxCEGUIButtonBase.cpp
 * @brief Implementation for RFSX ButtonBase object.
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUIButtonBase.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxButtonBase

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxButtonBase implementation
//////////////////////////////////////////////////////////////////////////////////////////

bool SX_IMPL_TYPE::method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
{
	if(h_method(this, m_name, args, r_val, ctxt))
		return true;
	else
		return sxWindow::method(m_name, args, r_val, ctxt);
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxButtonBase method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(isHovering)
{
	r_val = caller->ButtonBase()->isHovering();
	return true;
}


SX_METHOD_IMPL(isPushed)
{
	r_val = caller->ButtonBase()->isPushed();
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxButtonBase initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXBUTTONBASE_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(isHovering);
	SX_ADD_METHOD(isPushed);
}

} // namespace RFSX

#undef SX_IMPL_TYPE
