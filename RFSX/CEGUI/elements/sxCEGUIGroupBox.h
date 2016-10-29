/************************************************************************************//**
 * @file sxCEGUIGroupBox.h
 * @brief Class declaration for RFSX GroupBox object
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_GROUPBOX_H_
#define _SX_GROUPBOX_H_

#include "RFSX.h"
#include "../sxCEGUIWindow.h"

// sxGroupBox class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		bool drawAroundWidget(skString name);
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxGroupBox

#define SXGROUPBOX_METHODS 1

namespace RFSX
{

class SX_IMPL_TYPE: public sxWindow
{
public:
	SX_IMPL_TYPE(CEGUI::GroupBox* box) : sxWindow(box) {}
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);

	CEGUI::GroupBox* GroupBox() { return static_cast<CEGUI::GroupBox*>(Window()); }

protected:
	SX_METHOD_DECL(drawAroundWidget);

private:
	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_GROUPBOX_H_
