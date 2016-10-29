/************************************************************************************//**
 * @file sxCEGUITabControl.cpp
 * @brief Implementation for RFSX TabControl object.
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUITabControl.h"
#include "../sxCEGUIUDim.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxTabControl

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxTabControl implementation
//////////////////////////////////////////////////////////////////////////////////////////

bool SX_IMPL_TYPE::method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
{
	if(h_method(this, m_name, args, r_val, ctxt))
		return true;
	else
		return sxWindow::method(m_name, args, r_val, ctxt);
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxTabControl method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(getTabCount)
{
	r_val = static_cast<int>(caller->TabControl()->getTabCount());
	return true;
}


SX_METHOD_IMPL(setSelectedTab)
{
	if(args.entries() != 1)
		return false;

	//void setSelectedTab(string name);
	//void setSelectedTab(unsigned int ID);
	if(args[0].type() == skRValue::T_String)
	{
		caller->TabControl()->setSelectedTab(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
		return true;
	}
	if(args[0].type() == skRValue::T_Int)
	{
		caller->TabControl()->setSelectedTab(args[0].intValue());
		return true;
	}

	return false;
}


SX_METHOD_IMPL(setSelectedTabAtIndex)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	caller->TabControl()->setSelectedTabAtIndex(args[0].intValue());
	return true;
}


SX_METHOD_IMPL(getTabContentsAtIndex)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	//Window* getTabContentsAtIndex(unsigned int index) const;
	r_val.assignObject(new sxWindow(caller->TabControl()->getTabContentsAtIndex(args[0].intValue())), true);
	return true;
}


SX_METHOD_IMPL(getTabContents)
{
	if(args.entries() != 1)
		return false;

	//Window* getTabContents(string name) const;
	//Window* getTabContents(unsigned int ID) const;
	if(args[0].type() == skRValue::T_String)
	{
		r_val.assignObject(new sxWindow(caller->TabControl()->getTabContents(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()))), true);
		return true;
	}
	if(args[0].type() == skRValue::T_Int)
	{
		r_val.assignObject(new sxWindow(caller->TabControl()->getTabContents(args[0].intValue())), true);
		return true;
	}

	return false;
}


SX_METHOD_IMPL(isTabContentsSelected)
{
	if(args.entries() != 1)
		return false;

	//bool isTabContentsSelected(Window* wnd) const;
	sxWindow *win = IS_WINDOW(args[0]);
	if(!win)
		return false;

	r_val = caller->TabControl()->isTabContentsSelected(win->Window());
	return true;
}


SX_METHOD_IMPL(getSelectedTabIndex)
{
	r_val = static_cast<int>(caller->TabControl()->getSelectedTabIndex());
	return true;
}


SX_METHOD_IMPL(getTabHeight)
{
	//const UDim& getTabHeight() const;
	r_val.assignObject(new sxUDim(caller->TabControl()->getTabHeight()), true);
	return true;
}


SX_METHOD_IMPL(getTabTextPadding)
{
	//const UDim& getTabTextPadding() const;
	r_val.assignObject(new sxUDim(caller->TabControl()->getTabTextPadding()), true);
	return true;
}


SX_METHOD_IMPL(setTabHeight)
{
	if(args.entries() != 1)
		return false;

	sxUDim *udim = IS_UDIM(args[0]);
	if(!udim)
		return false;

	//void setTabHeight(const UDim& height);
	caller->TabControl()->setTabHeight(udim->UDimConstRef());
	return true;
}


SX_METHOD_IMPL(setTabTextPadding)
{
	if(args.entries() != 1)
		return false;

	sxUDim *udim = IS_UDIM(args[0]);
	if(!udim)
		return false;

	//void setTabTextPadding(const UDim& pad);
	caller->TabControl()->setTabTextPadding(udim->UDimConstRef());
	return true;
}


SX_METHOD_IMPL(addTab)
{
	if(args.entries() != 1)
		return false;

	sxWindow *win = IS_WINDOW(args[0]);
	if(!win)
		return false;

	//void addTab(Window* wnd);
	caller->TabControl()->addTab(win->Window());
	return true;
}


SX_METHOD_IMPL(removeTab)
{
	if(args.entries() != 1)
		return false;

	//void removeTab(string name);
	//void removeTab(unsigned int ID);
	if(args[0].type() == skRValue::T_String)
	{
		caller->TabControl()->removeTab(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
		return true;
	}
	if(args[0].type() == skRValue::T_Int)
	{
		caller->TabControl()->removeTab(args[0].intValue());
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxTabControl initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXTABCONTROL_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(getTabCount);
	SX_ADD_METHOD(setSelectedTab);
	SX_ADD_METHOD(setSelectedTabAtIndex);
	SX_ADD_METHOD(getTabContentsAtIndex);
	SX_ADD_METHOD(getTabContents);
	SX_ADD_METHOD(isTabContentsSelected);
	SX_ADD_METHOD(getSelectedTabIndex);
	SX_ADD_METHOD(getTabHeight);
	SX_ADD_METHOD(getTabTextPadding);
	SX_ADD_METHOD(setTabHeight);
	SX_ADD_METHOD(setTabTextPadding);
	SX_ADD_METHOD(addTab);
	SX_ADD_METHOD(removeTab);
}

} // namespace RFSX

#undef SX_IMPL_TYPE
