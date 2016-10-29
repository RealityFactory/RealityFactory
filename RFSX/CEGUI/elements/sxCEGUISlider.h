/************************************************************************************//**
 * @file sxCEGUISlider.h
 * @brief Class declaration for RFSX Slider object
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_SLIDER_H_
#define _SX_SLIDER_H_

#include "RFSX.h"
#include "../sxCEGUIWindow.h"

// sxSlider class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		float getCurrentValue() const;
		float getMaxValue() const;
		float getClickStep() const;

		void setMaxValue(float maxVal);
		void setCurrentValue(float value);
		void setClickStep(float step);
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxSlider

#define SXSLIDER_METHODS 6

namespace RFSX
{

class SX_IMPL_TYPE: public sxWindow
{
public:
	SX_IMPL_TYPE(CEGUI::Slider* slider) : sxWindow(slider) {}
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);

	CEGUI::Slider* Slider() { return static_cast<CEGUI::Slider*>(Window()); }

protected:
	SX_METHOD_DECL(getCurrentValue);
	SX_METHOD_DECL(getMaxValue);
	SX_METHOD_DECL(getClickStep);
	SX_METHOD_DECL(setMaxValue);
	SX_METHOD_DECL(setCurrentValue);
	SX_METHOD_DECL(setClickStep);

private:
	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_SLIDER_H_
