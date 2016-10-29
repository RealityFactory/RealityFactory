/************************************************************************************//**
 * @file sxCEGUITitlebar.h
 * @brief Class declaration for RFSX Titlebar object
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_TITLEBAR_H_
#define _SX_TITLEBAR_H_

#include "RFSX.h"
#include "../sxCEGUIWindow.h"

// sxTitlebar class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		bool isDraggingEnabled();
		void setDraggingEnabled(bool setting);
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxTitlebar

#define SXTITLEBAR_METHODS 2

namespace RFSX
{

class sxTitlebar: public sxWindow
{
public:
	SX_IMPL_TYPE(CEGUI::Titlebar* titlebar) : sxWindow(titlebar) {}
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);

	CEGUI::Titlebar* Titlebar() { return static_cast<CEGUI::Titlebar*>(Window()); }

protected:
	SX_METHOD_DECL(isDraggingEnabled);
	SX_METHOD_DECL(setDraggingEnabled);

private:
	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_TITLEBAR_H_
