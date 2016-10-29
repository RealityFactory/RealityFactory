/************************************************************************************//**
 * @file sxCEGUITabButton.h
 * @brief Class declaration for RFSX TabButton object
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_TABBUTTON_H_
#define _SX_TABBUTTON_H_

#include "RFSX.h"
#include "sxCEGUIButtonBase.h"

// sxTabButton class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		bool isSelected();
		void setSelected(bool select);
		void setTargetWindow(sxWindow wnd);
		sxWindow getTargetWindow();
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxTabButton

#define SXTABBUTTON_METHODS 4

namespace RFSX
{

class SX_IMPL_TYPE: public sxButtonBase
{
public:
	SX_IMPL_TYPE(CEGUI::TabButton* button) : sxButtonBase(button) {}
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);

	CEGUI::TabButton* TabButton() { return static_cast<CEGUI::TabButton*>(ButtonBase()); }

protected:
	SX_METHOD_DECL(isSelected);
	SX_METHOD_DECL(setSelected);
	SX_METHOD_DECL(setTargetWindow);
	SX_METHOD_DECL(getTargetWindow);

private:
	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_TABBUTTON_H_
