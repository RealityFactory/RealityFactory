/************************************************************************************//**
 * @file sxCEGUISpinner.h
 * @brief Class declaration for RFSX TEMPLATE object
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_SPINNER_H_
#define _SX_SPINNER_H_

#include "RFSX.h"
#include "../sxCEGUIWindow.h"

// sxSpinner class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		float getCurrentValue();
		float getStepSize();
		float getMaximumValue();
		float getMinimumValue();
		int getTextInputMode();

		void setCurrentValue(float value);
		void setStepSize(float step);
		void setMaximumValue(float maxValue);
		void setMinimumValue(float minVaue);
		void setTextInputMode(int mode);
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxSpinner

#define SXSPINNER_METHODS 10

namespace RFSX
{

class SX_IMPL_TYPE: public sxWindow
{
public:
	SX_IMPL_TYPE(CEGUI::Spinner* spinner) : sxWindow(spinner) {}
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);

	CEGUI::Spinner* Spinner() { return static_cast<CEGUI::Spinner*>(Window()); }

protected:
	SX_METHOD_DECL(getCurrentValue);
	SX_METHOD_DECL(getStepSize);
	SX_METHOD_DECL(getMaximumValue);
	SX_METHOD_DECL(getMinimumValue);
	SX_METHOD_DECL(getTextInputMode);
	SX_METHOD_DECL(setCurrentValue);
	SX_METHOD_DECL(setStepSize);
	SX_METHOD_DECL(setMaximumValue);
	SX_METHOD_DECL(setMinimumValue);
	SX_METHOD_DECL(setTextInputMode);

private:
	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_SPINNER_H_
