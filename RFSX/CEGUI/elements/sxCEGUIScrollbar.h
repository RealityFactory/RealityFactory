/************************************************************************************//**
 * @file sxCEGUIScrollbar.h
 * @brief Class declaration for RFSX Scrollbar object
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_SCROLLBAR_H_
#define _SX_SCROLLBAR_H_

#include "RFSX.h"
#include "../sxCEGUIWindow.h"

// sxScrollbar class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		float getDocumentSize() const;
		float getPageSize() const;
		float getStepSize() const;
		float getOverlapSize() const;
		float getScrollPosition() const;

		void setDocumentSize(float document_size);
		void setPageSize(float page_size);
		void setStepSize(float step_size);
		void setOverlapSize(float overlap_size);
		void setScrollPosition(float position);
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxScrollbar

#define SXSCROLLBAR_METHODS 10

namespace RFSX
{

class SX_IMPL_TYPE: public sxWindow
{
public:
	SX_IMPL_TYPE(CEGUI::Scrollbar* scrollbar) : sxWindow(scrollbar) {}
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);

	CEGUI::Scrollbar* Scrollbar() { return static_cast<CEGUI::Scrollbar*>(Window()); }

protected:
	SX_METHOD_DECL(getDocumentSize);
	SX_METHOD_DECL(getPageSize);
	SX_METHOD_DECL(getStepSize);
	SX_METHOD_DECL(getOverlapSize);
	SX_METHOD_DECL(getScrollPosition);
	SX_METHOD_DECL(setDocumentSize);
	SX_METHOD_DECL(setPageSize);
	SX_METHOD_DECL(setStepSize);
	SX_METHOD_DECL(setOverlapSize);
	SX_METHOD_DECL(setScrollPosition);

private:
	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_SCROLLBAR_H_
