/************************************************************************************//**
 * @file sxCEGUIScrollbar.cpp
 * @brief Implementation for RFSX Scrollbar object.
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUIScrollbar.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxScrollbar

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxScrollbar implementation
//////////////////////////////////////////////////////////////////////////////////////////

bool SX_IMPL_TYPE::method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
{
	if(h_method(this, m_name, args, r_val, ctxt))
		return true;
	else
		return sxWindow::method(m_name, args, r_val, ctxt);
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxScrollbar method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(getDocumentSize)
{
	//float getDocumentSize() const;
	r_val = caller->Scrollbar()->getDocumentSize();
	return true;
}


SX_METHOD_IMPL(getPageSize)
{
	//float getPageSize() const;
	r_val = caller->Scrollbar()->getPageSize();
	return true;
}


SX_METHOD_IMPL(getStepSize)
{
	//float getStepSize() const;
	r_val = caller->Scrollbar()->getStepSize();
	return true;
}


SX_METHOD_IMPL(getOverlapSize)
{
	//float getOverlapSize() const;
	r_val = caller->Scrollbar()->getOverlapSize();
	return true;
}


SX_METHOD_IMPL(getScrollPosition)
{
	//float getScrollPosition() const;
	r_val = caller->Scrollbar()->getScrollPosition();
	return true;
}


SX_METHOD_IMPL(setDocumentSize)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Float)
		return false;

	//void setDocumentSize(float document_size);
	caller->Scrollbar()->setDocumentSize(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(setPageSize)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Float)
		return false;

	//void setPageSize(float page_size);
	caller->Scrollbar()->setPageSize(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(setStepSize)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Float)
		return false;

	//void setStepSize(float step_size);
	caller->Scrollbar()->setStepSize(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(setOverlapSize)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Float)
		return false;

	//void setOverlapSize(float overlap_size);
	caller->Scrollbar()->setOverlapSize(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(setScrollPosition)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Float)
		return false;

	//void setScrollPosition(float position);
	caller->Scrollbar()->setScrollPosition(args[0].floatValue());
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxScrollbar initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXSCROLLBAR_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(getDocumentSize);
	SX_ADD_METHOD(getPageSize);
	SX_ADD_METHOD(getStepSize);
	SX_ADD_METHOD(getOverlapSize);
	SX_ADD_METHOD(getScrollPosition);
	SX_ADD_METHOD(setDocumentSize);
	SX_ADD_METHOD(setPageSize);
	SX_ADD_METHOD(setStepSize);
	SX_ADD_METHOD(setOverlapSize);
	SX_ADD_METHOD(setScrollPosition);
}

} // namespace RFSX

#undef SX_IMPL_TYPE
