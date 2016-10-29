/************************************************************************************//**
 * @file sxCEGUIRadioButton.h
 * @brief Class declaration for RFSX TEMPLATE object
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_RADIOBUTTON_H_
#define _SX_RADIOBUTTON_H_

#include "RFSX.h"
#include "sxCEGUIButtonBase.h"

// sxRadioButton class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		bool isSelected();
		void setSelected(bool select);
		int getGroupID();
		void setGroupID(int group);
		sxRadioButton getSelectedButtonInGroup(); -- may return null!
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxRadioButton

#define SXRADIOBUTTON_METHODS 5

namespace RFSX
{

class SX_IMPL_TYPE: public sxButtonBase
{
public:
	SX_IMPL_TYPE(CEGUI::RadioButton* button) : sxButtonBase(button) {}
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);

	CEGUI::RadioButton* RadioButton() { return static_cast<CEGUI::RadioButton*>(ButtonBase()); }

protected:
	SX_METHOD_DECL(isSelected);
	SX_METHOD_DECL(setSelected);
	SX_METHOD_DECL(getGroupID);
	SX_METHOD_DECL(setGroupID);
	SX_METHOD_DECL(getSelectedButtonInGroup);

private:
	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_RADIOBUTTON_H_
