/************************************************************************************//**
 * @file sxCEGUIProgressBar.h
 * @brief Class declaration for RFSX ProgressBar object
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_PROGRESSBAR_H_
#define _SX_PROGRESSBAR_H_

#include "RFSX.h"
#include "../sxCEGUIWindow.h"

// sxProgressBar class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		float getProgress();
		float getStep();
		void setProgress(float progress);
		void setStepSize(float step);
		void step();
		void adjustProgress(float delta);
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxProgressBar

#define SXPROGRESSBAR_METHODS 6

namespace RFSX
{

class SX_IMPL_TYPE: public sxWindow
{
public:
	SX_IMPL_TYPE(CEGUI::ProgressBar* progressBar) : sxWindow(progressBar) {}
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);

	CEGUI::ProgressBar* ProgressBar() { return static_cast<CEGUI::ProgressBar*>(Window()); }

protected:
	SX_METHOD_DECL(getProgress);
	SX_METHOD_DECL(getStep);
	SX_METHOD_DECL(setProgress);
	SX_METHOD_DECL(setStepSize);
	SX_METHOD_DECL(step);
	SX_METHOD_DECL(adjustProgress);

private:
	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_PROGRESSBAR_H_
