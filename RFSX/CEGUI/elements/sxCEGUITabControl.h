/************************************************************************************//**
 * @file sxCEGUITabControl.h
 * @brief Class declaration for RFSX TabControl object
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_TABCONTROL_H_
#define _SX_TABCONTROL_H_

#include "RFSX.h"
#include "../sxCEGUIWindow.h"

// sxTabControl class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		int getTabCount();
		void setSelectedTab(skString name);
		void setSelectedTab(int ID);
		void setSelectedTabAtIndex(int index);

		sxWindow getTabContentsAtIndex(int index);
		sxWindow getTabContents(skString name);
		sxWindow getTabContents(int ID);

		bool isTabContentsSelected(sxWindow wnd);
		int getSelectedTabIndex();

		sxUDim getTabHeight();
		sxUDim getTabTextPadding();

		void setTabHeight(sxUDim height);
		void setTabTextPadding(sxUDim pad);

		void addTab(sxWindow wnd);

		void removeTab(int ID);
		void removeTab(skString name);
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxTabControl

#define SXTABCONTROL_METHODS 13

namespace RFSX
{

class SX_IMPL_TYPE: public sxWindow
{
public:
	SX_IMPL_TYPE(CEGUI::TabControl* control) : sxWindow(control) {}
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);

	CEGUI::TabControl* TabControl() { return static_cast<CEGUI::TabControl*>(Window()); }

protected:
	SX_METHOD_DECL(getTabCount);
	SX_METHOD_DECL(setSelectedTab);
	SX_METHOD_DECL(setSelectedTabAtIndex);
	SX_METHOD_DECL(getTabContentsAtIndex);
	SX_METHOD_DECL(getTabContents);
	SX_METHOD_DECL(isTabContentsSelected);
	SX_METHOD_DECL(getSelectedTabIndex);
	SX_METHOD_DECL(getTabHeight);
	SX_METHOD_DECL(getTabTextPadding);
	SX_METHOD_DECL(setTabHeight);
	SX_METHOD_DECL(setTabTextPadding);
	SX_METHOD_DECL(addTab);
	SX_METHOD_DECL(removeTab);

private:
	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_TABCONTROL_H_
