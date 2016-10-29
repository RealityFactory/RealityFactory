/************************************************************************************//**
 * @file sxCEGUICheckbox.cpp
 * @brief Implementation for RFSX Checkbox object.
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUICheckbox.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxCheckbox

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxCheckbox implementation
//////////////////////////////////////////////////////////////////////////////////////////

bool SX_IMPL_TYPE::method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
{
	if(h_method(this, m_name, args, r_val, ctxt))
		return true;
	else
		return sxButtonBase::method(m_name, args, r_val, ctxt);
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxCheckbox method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(isSelected)
{
	r_val = caller->Checkbox()->isSelected();
	return true;
}

SX_METHOD_IMPL(setSelected)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	caller->Checkbox()->setSelected(args[0].boolValue());
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxCheckbox initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXCHECKBOX_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(isSelected);
	SX_ADD_METHOD(setSelected);
}

} // namespace RFSX

#undef SX_IMPL_TYPE
