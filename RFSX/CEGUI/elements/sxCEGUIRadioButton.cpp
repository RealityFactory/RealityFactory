/************************************************************************************//**
 * @file sxCEGUIRadioButton.cpp
 * @brief Implementation for RFSX RadioButton object.
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUIRadioButton.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxRadioButton

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxRadioButton implementation
//////////////////////////////////////////////////////////////////////////////////////////

bool SX_IMPL_TYPE::method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
{
	if(h_method(this, m_name, args, r_val, ctxt))
		return true;
	else
		return sxButtonBase::method(m_name, args, r_val, ctxt);
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxRadioButton method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(isSelected)
{
	r_val = caller->RadioButton()->isSelected();
	return true;
}


SX_METHOD_IMPL(setSelected)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	caller->RadioButton()->setSelected(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(getGroupID)
{
	r_val = static_cast<int>(caller->RadioButton()->getGroupID());
	return true;
}


SX_METHOD_IMPL(setGroupID)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	caller->RadioButton()->setGroupID(args[0].intValue());
	return true;
}


SX_METHOD_IMPL(getSelectedButtonInGroup)
{
	CEGUI::RadioButton* button = caller->RadioButton()->getSelectedButtonInGroup();

	if(button)
		r_val.assignObject(new sxRadioButton(button), true);
	else
		r_val.assignObject(&ctxt.getInterpreter()->getNull());

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxRadioButton initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXRADIOBUTTON_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(isSelected);
	SX_ADD_METHOD(setSelected);
	SX_ADD_METHOD(getGroupID);
	SX_ADD_METHOD(setGroupID);
	SX_ADD_METHOD(getSelectedButtonInGroup);
}

} // namespace RFSX

#undef SX_IMPL_TYPE
