/************************************************************************************//**
 * @file sxCEGUISpinner.cpp
 * @brief Implementation for RFSX Spinner object.
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUISpinner.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxSpinner

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxSpinner implementation
//////////////////////////////////////////////////////////////////////////////////////////

bool SX_IMPL_TYPE::method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
{
	if(h_method(this, m_name, args, r_val, ctxt))
		return true;
	else
		return sxWindow::method(m_name, args, r_val, ctxt);
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxSpinner method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(getCurrentValue)
{
	//float getCurrentValue() const;
	r_val = caller->Spinner()->getCurrentValue();
	return true;
}


SX_METHOD_IMPL(getStepSize)
{
	//float getStepSize() const;
	r_val = caller->Spinner()->getStepSize();
	return true;
}


SX_METHOD_IMPL(getMaximumValue)
{
	//float getMaximumValue() const;
	r_val = caller->Spinner()->getMaximumValue();
	return true;
}


SX_METHOD_IMPL(getMinimumValue)
{
	//float getMinimumValue() const;
	r_val = caller->Spinner()->getMinimumValue();
	return true;
}


SX_METHOD_IMPL(getTextInputMode)
{
	//TextInputMode getTextInputMode() const;
	r_val = caller->Spinner()->getTextInputMode();
	return true;
}


SX_METHOD_IMPL(setCurrentValue)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Float)
		return false;

	//void setCurrentValue(float value);
	caller->Spinner()->setCurrentValue(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(setStepSize)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Float)
		return false;

	//void setStepSize(float step);
	caller->Spinner()->setStepSize(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(setMaximumValue)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Float)
		return false;

	//void setMaximumValue(float maxValue);
	caller->Spinner()->setMaximumValue(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(setMinimumValue)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Float)
		return false;

	//void setMinimumValue(float minVaue);
	caller->Spinner()->setMinimumValue(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(setTextInputMode)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	//void setTextInputMode(TextInputMode mode);
	caller->Spinner()->setTextInputMode(static_cast<CEGUI::Spinner::TextInputMode>(args[0].intValue()));
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxSpinner initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXSPINNER_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(getCurrentValue);
	SX_ADD_METHOD(getStepSize);
	SX_ADD_METHOD(getMaximumValue);
	SX_ADD_METHOD(getMinimumValue);
	SX_ADD_METHOD(getTextInputMode);
	SX_ADD_METHOD(setCurrentValue);
	SX_ADD_METHOD(setStepSize);
	SX_ADD_METHOD(setMaximumValue);
	SX_ADD_METHOD(setMinimumValue);
	SX_ADD_METHOD(setTextInputMode);
}

} // namespace RFSX

#undef SX_IMPL_TYPE
