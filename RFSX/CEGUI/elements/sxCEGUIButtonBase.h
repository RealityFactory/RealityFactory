/************************************************************************************//**
 * @file sxCEGUIButtonBase.h
 * @brief Class declaration for RFSX ButtonBase object
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_BUTTONBASE_H_
#define _SX_BUTTONBASE_H_

#include "RFSX.h"
#include "../sxCEGUIWindow.h"

// sxButtonBase class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		bool isHovering();
		bool isPushed();
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxButtonBase

#define SXBUTTONBASE_METHODS 2

namespace RFSX
{

class SX_IMPL_TYPE: public sxWindow
{
public:
	SX_IMPL_TYPE(CEGUI::ButtonBase* button) : sxWindow(button) {}
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);

	CEGUI::ButtonBase* ButtonBase() { return static_cast<CEGUI::ButtonBase*>(Window()); }

protected:
	SX_METHOD_DECL(isHovering);
	SX_METHOD_DECL(isPushed);

private:
	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_BUTTONBASE_H_
