/************************************************************************************//**
 * @file sxCEGUIProgressBar.cpp
 * @brief Implementation for RFSX ProgressBar object.
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUIProgressBar.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxProgressBar

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxProgressBar implementation
//////////////////////////////////////////////////////////////////////////////////////////

bool SX_IMPL_TYPE::method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
{
	if(h_method(this, m_name, args, r_val, ctxt))
		return true;
	else
		return sxWindow::method(m_name, args, r_val, ctxt);
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxProgressBar method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(getProgress)
{
	//float getProgress() const;
	r_val = caller->ProgressBar()->getProgress();
	return true;
}


SX_METHOD_IMPL(getStep)
{
	//float getStep() const;
	r_val = caller->ProgressBar()->getStep();
	return true;
}


SX_METHOD_IMPL(setProgress)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Float)
		return false;

	//void setProgress(float progress);
	caller->ProgressBar()->setProgress(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(setStepSize)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Float)
		return false;

	//void setStepSize(float step);
	caller->ProgressBar()->setStepSize(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(step)
{
	//void step();
	caller->ProgressBar()->step();
	return true;
}


SX_METHOD_IMPL(adjustProgress)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Float)
		return false;

	//void adjustProgress(float delta);
	caller->ProgressBar()->adjustProgress(args[0].floatValue());
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxProgressBar initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXPROGRESSBAR_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(getProgress);
	SX_ADD_METHOD(getStep);
	SX_ADD_METHOD(setProgress);
	SX_ADD_METHOD(setStepSize);
	SX_ADD_METHOD(step);
	SX_ADD_METHOD(adjustProgress);
}

} // namespace RFSX

#undef SX_IMPL_TYPE
