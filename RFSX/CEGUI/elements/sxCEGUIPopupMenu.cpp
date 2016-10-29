/************************************************************************************//**
 * @file sxCEGUIPopupMenu.cpp
 * @brief Implementation for RFSX PopupMenu object.
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUIPopupMenu.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxPopupMenu

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxPopupMenu implementation
//////////////////////////////////////////////////////////////////////////////////////////

bool SX_IMPL_TYPE::method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
{
	if(h_method(this, m_name, args, r_val, ctxt))
		return true;
	else
		return sxMenuBase::method(m_name, args, r_val, ctxt);
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxPopupMenu method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(getFadeInTime)
{
	//float getFadeInTime() const;
	r_val = caller->PopupMenu()->getFadeInTime();
	return true;
}


SX_METHOD_IMPL(getFadeOutTime)
{
	//float getFadeOutTime() const;
	r_val = caller->PopupMenu()->getFadeOutTime();
	return true;
}


SX_METHOD_IMPL(setFadeInTime)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Float)
		return false;

	//void setFadeInTime(float fadetime);
	caller->PopupMenu()->setFadeInTime(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(setFadeOutTime)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Float)
		return false;

	//void setFadeOutTime(float fadetime);
	caller->PopupMenu()->setFadeOutTime(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(openPopupMenu)
{
	//void openPopupMenu();
	caller->PopupMenu()->openPopupMenu();
	return true;
}


SX_METHOD_IMPL(closePopupMenu)
{
	//void closePopupMenu();
	caller->PopupMenu()->closePopupMenu();
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxPopupMenu initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXPOPUPMENU_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(getFadeInTime);
	SX_ADD_METHOD(getFadeOutTime);
	SX_ADD_METHOD(setFadeInTime);
	SX_ADD_METHOD(setFadeOutTime);
	SX_ADD_METHOD(openPopupMenu);
	SX_ADD_METHOD(closePopupMenu);
}

} // namespace RFSX

#undef SX_IMPL_TYPE // And that's it in a nutshell
