/************************************************************************************//**
 * @file sxCEGUIMenuItem.cpp
 * @brief Implementation for RFSX sxMenuItem object.
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUIMenuItem.h"
#include "sxCEGUIPopupMenu.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxMenuItem

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxMenuItem implementation
//////////////////////////////////////////////////////////////////////////////////////////

bool SX_IMPL_TYPE::method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
{
	if(h_method(this, m_name, args, r_val, ctxt))
		return true;
	else
		return sxItemEntry::method(m_name, args, r_val, ctxt);
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxMenuItem method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(isHovering)
{
	//bool isHovering() const;
	r_val = caller->MenuItem()->isHovering();
	return true;
}


SX_METHOD_IMPL(isPushed)
{
	//bool isPushed() const;
	r_val = caller->MenuItem()->isPushed();
	return true;
}


SX_METHOD_IMPL(isOpened)
{
	//bool isOpened() const;
	r_val = caller->MenuItem()->isOpened();
	return true;
}


SX_METHOD_IMPL(getPopupMenu)
{
	//PopupMenu* getPopupMenu() const;
	CEGUI::PopupMenu *menu = caller->MenuItem()->getPopupMenu();

	if(menu)
		r_val.assignObject(new sxPopupMenu(menu), true);
	else
		r_val.assignObject(&ctxt.getInterpreter()->getNull());

	return true;
}


SX_METHOD_IMPL(setPopupMenu)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Object)
		return false;

	CEGUI::PopupMenu *menu = NULL;

	if(args[0].obj() != &ctxt.getInterpreter()->getNull())
	{
		sxPopupMenu *popup = static_cast<sxPopupMenu*>(args[0].obj());
		menu = popup->PopupMenu();
	}

	//void setPopupMenu(PopupMenu* popup);
	caller->MenuItem()->setPopupMenu(menu);
	return true;
}


SX_METHOD_IMPL(openPopupMenu)
{
	if(args.entries() > 1)
		return false;

	bool notify = true;

	if(args.entries() == 1)
	{
		if(args[0].type() != skRValue::T_Bool)
			return false;

		notify = args[0].boolValue();
	}

	//void openPopupMenu(bool notify = true);
	caller->MenuItem()->openPopupMenu(notify);
	return true;
}


SX_METHOD_IMPL(closePopupMenu)
{
	if(args.entries() > 1)
		return false;

	bool notify = true;

	if(args.entries() == 1)
	{
		if(args[0].type() != skRValue::T_Bool)
			return false;

		notify = args[0].boolValue();
	}

	//void closePopupMenu(bool notify=true);
	caller->MenuItem()->closePopupMenu(notify);
	return true;
}


SX_METHOD_IMPL(togglePopupMenu)
{
	//bool togglePopupMenu();
	r_val = caller->MenuItem()->togglePopupMenu();
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxMenuItem initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXMENUITEM_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(isHovering);
	SX_ADD_METHOD(isPushed);
	SX_ADD_METHOD(isOpened);
	SX_ADD_METHOD(getPopupMenu);
	SX_ADD_METHOD(setPopupMenu);
	SX_ADD_METHOD(openPopupMenu);
	SX_ADD_METHOD(closePopupMenu);
	SX_ADD_METHOD(togglePopupMenu);
}

} // namespace RFSX

#undef SX_IMPL_TYPE // And that's it in a nutshell
