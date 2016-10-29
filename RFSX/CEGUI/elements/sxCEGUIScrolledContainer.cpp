/************************************************************************************//**
 * @file sxCEGUIScrolledContainer.cpp
 * @brief Implementation for RFSX ScrolledContainer object.
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUIScrolledContainer.h"
#include "../sxCEGUIRect.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxScrolledContainer

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxScrolledContainer implementation
//////////////////////////////////////////////////////////////////////////////////////////

bool SX_IMPL_TYPE::method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
{
	if(h_method(this, m_name, args, r_val, ctxt))
		return true;
	else
		return sxWindow::method(m_name, args, r_val, ctxt);
}


//////////////////////////////////////////////////////////////////////////////////////////
// sxScrolledContainer method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(isContentPaneAutoSized)
{
	//bool isContentPaneAutoSized() const;
	r_val = caller->ScrolledContainer()->isContentPaneAutoSized();
	return true;
}


SX_METHOD_IMPL(setContentPaneAutoSized)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	//void setContentPaneAutoSized(bool setting);
	caller->ScrolledContainer()->setContentPaneAutoSized(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(getContentArea)
{
	//const Rect& getContentArea() const;
	r_val.assignObject(new sxRect(caller->ScrolledContainer()->getContentArea()), true);
	return true;
}


SX_METHOD_IMPL(setContentArea)
{
	if(args.entries() != 1)
		return false;

	sxRect *rect = IS_RECT(args[0]);
	if(!rect)
		return false;

	//void setContentArea(const Rect& area);
	caller->ScrolledContainer()->setContentArea(rect->RectConstRef());
	return true;
}


SX_METHOD_IMPL(getChildExtentsArea)
{
	//Rect getChildExtentsArea() const;
	r_val.assignObject(new sxRect(caller->ScrolledContainer()->getChildExtentsArea()), true);
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxScrolledContainer initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXSCROLLEDCONTAINER_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(isContentPaneAutoSized);
	SX_ADD_METHOD(setContentPaneAutoSized);
	SX_ADD_METHOD(getContentArea);
	SX_ADD_METHOD(setContentArea);
	SX_ADD_METHOD(getChildExtentsArea);
}

} // namespace RFSX

#undef SX_IMPL_TYPE // And that's it in a nutshell
