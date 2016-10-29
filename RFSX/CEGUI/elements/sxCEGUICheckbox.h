/************************************************************************************//**
 * @file sxCEGUICheckbox.h
 * @brief Class declaration for RFSX Checkbox object
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_CHECKBOX_H_
#define _SX_CHECKBOX_H_

#include "RFSX.h"
#include "sxCEGUIButtonBase.h"

// sxCheckbox class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		bool isSelected();
		void setSelected(bool select);
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxCheckbox

#define SXCHECKBOX_METHODS 2

namespace RFSX
{

class SX_IMPL_TYPE: public sxButtonBase
{
public:
	SX_IMPL_TYPE(CEGUI::Checkbox* box) : sxButtonBase(box) {}
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);

	CEGUI::Checkbox* Checkbox() { return static_cast<CEGUI::Checkbox*>(ButtonBase()); }

protected:
	SX_METHOD_DECL(isSelected);
	SX_METHOD_DECL(setSelected);

private:
	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_CHECKBOX_H_
