/************************************************************************************//**
 * @file sxCEGUIFrameWindow.h
 * @brief Class declaration for RFSX FrameWindow object
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_FRAMEWINDOW_H_
#define _SX_FRAMEWINDOW_H_

#include "RFSX.h"
#include "../sxCEGUIWindow.h"

// sxFrameWindow class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		bool isSizingEnabled();
		bool isFrameEnabled();
		bool isTitleBarEnabled();
		bool isCloseButtonEnabled();
		bool isRollupEnabled();
		bool isRolledup();
		bool isDragMovingEnabled();

		void setSizingEnabled(bool enabled);
		void setFrameEnabled(bool enabled);
		void setTitleBarEnabled(bool enabled);
		void setCloseButtonEnabled(bool enabled);
		void setRollupEnabled(bool enabled);
		void setDragMovingEnabled(bool enabled);

		float getSizingBorderThickness();
		void setSizingBorderThickness(float pixels);

		void toggleRollup();

		void offsetPixelPosition(sxVector2 offset);
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxFrameWindow

#define SXFRAMEWINDOW_METHODS 17

namespace RFSX
{

class SX_IMPL_TYPE: public sxWindow
{
public:
	SX_IMPL_TYPE(CEGUI::FrameWindow* window) : sxWindow(window) {}
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);

	CEGUI::FrameWindow* FrameWindow() { return static_cast<CEGUI::FrameWindow*>(Window()); }

protected:
	SX_METHOD_DECL(isSizingEnabled);
	SX_METHOD_DECL(isFrameEnabled);
	SX_METHOD_DECL(isTitleBarEnabled);
	SX_METHOD_DECL(isCloseButtonEnabled);
	SX_METHOD_DECL(isRollupEnabled);
	SX_METHOD_DECL(isRolledup);
	SX_METHOD_DECL(isDragMovingEnabled);
	SX_METHOD_DECL(setSizingEnabled);
	SX_METHOD_DECL(setFrameEnabled);
	SX_METHOD_DECL(setTitleBarEnabled);
	SX_METHOD_DECL(setCloseButtonEnabled);
	SX_METHOD_DECL(setRollupEnabled);
	SX_METHOD_DECL(setDragMovingEnabled);
	SX_METHOD_DECL(getSizingBorderThickness);
	SX_METHOD_DECL(setSizingBorderThickness);
	SX_METHOD_DECL(toggleRollup);
	SX_METHOD_DECL(offsetPixelPosition);

private:
	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_FRAMEWINDOW_H_
