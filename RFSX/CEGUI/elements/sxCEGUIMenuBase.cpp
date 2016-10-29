/************************************************************************************//**
 * @file sxCEGUIMenuBase.cpp
 * @brief Implementation for RFSX MenuBase object.
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUIMenuBase.h"
#include "sxCEGUIMenuItem.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxMenuBase

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxMenuBase implementation
//////////////////////////////////////////////////////////////////////////////////////////

bool SX_IMPL_TYPE::method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
{
	if(h_method(this, m_name, args, r_val, ctxt))
		return true;
	else
		return sxItemListBase::method(m_name, args, r_val, ctxt);
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxMenuBase method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(getItemSpacing)
{
	//float getItemSpacing() const;
	r_val = caller->MenuBase()->getItemSpacing();
	return true;
}


SX_METHOD_IMPL(isMultiplePopupsAllowed)
{
	//bool isMultiplePopupsAllowed() const;
	r_val = caller->MenuBase()->isMultiplePopupsAllowed();
	return true;
}


SX_METHOD_IMPL(getPopupMenuItem)
{
	//MenuItem* getPopupMenuItem() const;
	CEGUI::MenuItem *item = caller->MenuBase()->getPopupMenuItem();

	if(item)
		r_val.assignObject(new sxMenuItem(item), true);
	else
		r_val.assignObject(&ctxt.getInterpreter()->getNull());

	return true;
}


SX_METHOD_IMPL(setItemSpacing)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Float)
		return false;

	//void setItemSpacing(float spacing);
	caller->MenuBase()->setItemSpacing(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(changePopupMenuItem)
{
	if(args.entries() > 1)
		return false;

	CEGUI::MenuItem *item = NULL;

	if(args.entries() == 1)
	{
		if(args[0].type() != skRValue::T_Object)
			return false;

		if(args[0].obj() != &ctxt.getInterpreter()->getNull())
		{
			sxMenuItem *sxitem = static_cast<sxMenuItem*>(args[0].obj());
			item = sxitem->MenuItem();
		}
	}

	//void changePopupMenuItem(MenuItem* item);
	caller->MenuBase()->changePopupMenuItem(item);
	return true;
}


SX_METHOD_IMPL(setAllowMultiplePopups)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	//void setAllowMultiplePopups(bool setting);
	caller->MenuBase()->setAllowMultiplePopups(args[0].boolValue());
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxMenuBase initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXMENUBASE_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(getItemSpacing);
	SX_ADD_METHOD(isMultiplePopupsAllowed);
	SX_ADD_METHOD(getPopupMenuItem);
	SX_ADD_METHOD(setItemSpacing);
	SX_ADD_METHOD(changePopupMenuItem);
	SX_ADD_METHOD(setAllowMultiplePopups);
}

} // namespace RFSX

#undef SX_IMPL_TYPE // And that's it in a nutshell
