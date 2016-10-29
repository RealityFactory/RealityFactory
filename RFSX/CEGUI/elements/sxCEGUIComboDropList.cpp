/************************************************************************************//**
 * @file sxCEGUIComboDropList.cpp
 * @brief Implementation for RFSX ComboDropList object.
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUIComboDropList.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxComboDropList

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxComboDropList implementation
//////////////////////////////////////////////////////////////////////////////////////////

bool SX_IMPL_TYPE::method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
{
	if(h_method(this, m_name, args, r_val, ctxt))
		return true;
	else
		return sxListbox::method(m_name, args, r_val, ctxt);
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxComboDropList method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(setArmed)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	//void setArmed(bool setting);
	caller->ComboDropList()->setArmed(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(isArmed)
{
	//bool isArmed(void) const;
	r_val = caller->ComboDropList()->isArmed();
	return true;
}


SX_METHOD_IMPL(setAutoArmEnabled)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	//void setAutoArmEnabled(bool setting);
	caller->ComboDropList()->setAutoArmEnabled(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(isAutoArmEnabled)
{
	//bool isAutoArmEnabled(void) const;
	r_val = caller->ComboDropList()->isAutoArmEnabled();
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxComboDropList initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXCOMBODROPLIST_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(setArmed);
	SX_ADD_METHOD(isArmed);
	SX_ADD_METHOD(setAutoArmEnabled);
	SX_ADD_METHOD(isAutoArmEnabled);
}

} // namespace RFSX

#undef SX_IMPL_TYPE // And that's it in a nutshell
