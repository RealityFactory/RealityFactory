/************************************************************************************//**
 * @file sxCEGUISlider.cpp
 * @brief Implementation for RFSX Slider object.
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUISlider.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxSlider

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxSlider implementation
//////////////////////////////////////////////////////////////////////////////////////////

bool SX_IMPL_TYPE::method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
{
	if(h_method(this, m_name, args, r_val, ctxt))
		return true;
	else
		return sxWindow::method(m_name, args, r_val, ctxt);
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxSlider method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(getCurrentValue)
{
	//float getCurrentValue() const;
	r_val = caller->Slider()->getCurrentValue();
	return true;
}


SX_METHOD_IMPL(getMaxValue)
{
	//float getMaxValue() const;
	r_val = caller->Slider()->getMaxValue();
	return true;
}


SX_METHOD_IMPL(getClickStep)
{
	//float getClickStep() const;
	r_val = caller->Slider()->getClickStep();
	return true;
}


SX_METHOD_IMPL(setMaxValue)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Float)
		return false;

	//void setMaxValue(float maxVal);
	caller->Slider()->setMaxValue(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(setCurrentValue)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Float)
		return false;

	//void setCurrentValue(float value);
	caller->Slider()->setCurrentValue(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(setClickStep)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Float)
		return false;

	//void setClickStep(float step);
	caller->Slider()->setClickStep(args[0].floatValue());
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxSlider initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXSLIDER_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(getCurrentValue);
	SX_ADD_METHOD(getMaxValue);
	SX_ADD_METHOD(getClickStep);
	SX_ADD_METHOD(setMaxValue);
	SX_ADD_METHOD(setCurrentValue);
	SX_ADD_METHOD(setClickStep);
}

} // namespace RFSX

#undef SX_IMPL_TYPE
