/************************************************************************************//**
 * @file sxCEGUITooltip.h
 * @brief Class declaration for RFSX Tooltip object
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_TOOLTIP_H_
#define _SX_TOOLTIP_H_

#include "RFSX.h"
#include "../sxCEGUIWindow.h"

// sxTooltip class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		void setTargetWindow(sxWindow wnd);
		void resetTimer();
		float getHoverTime();
		float getDisplayTime();
		float getFadeTime();
		void setHoverTime(float seconds);
		void setDisplayTime(float seconds);
		void setFadeTime(float seconds);
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxTooltip

#define SXTOOLTIP_METHODS 8

namespace RFSX
{

class SX_IMPL_TYPE: public sxWindow
{
public:
	SX_IMPL_TYPE(CEGUI::Tooltip* tooltip) : sxWindow(tooltip) {}
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);

	CEGUI::Tooltip* Tooltip() { return static_cast<CEGUI::Tooltip*>(Window()); }

protected:
	SX_METHOD_DECL(setTargetWindow);
	SX_METHOD_DECL(resetTimer);
	SX_METHOD_DECL(getHoverTime);
	SX_METHOD_DECL(getDisplayTime);
	SX_METHOD_DECL(getFadeTime);
	SX_METHOD_DECL(setHoverTime);
	SX_METHOD_DECL(setDisplayTime);
	SX_METHOD_DECL(setFadeTime);

private:
	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_TOOLTIP_H_
