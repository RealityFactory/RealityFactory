/************************************************************************************//**
 * @file sxCEGUIScrollablePane.h
 * @brief Class declaration for RFSX ScrollablePane object
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_SCROLLABLEPANE_H_
#define _SX_SCROLLABLEPANE_H_

#include "RFSX.h"
#include "../sxCEGUIWindow.h"

// sxScrollablePane class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		sxScrolledContainer getContentPane(); -- sxScrolledContainer must not be modified

		bool isVertScrollbarAlwaysShown();
		void setShowVertScrollbar(bool setting);

		bool isHorzScrollbarAlwaysShown();
		void setShowHorzScrollbar(bool setting);

		bool isContentPaneAutoSized();
		void setContentPaneAutoSized(bool setting);

		sxRect getContentPaneArea();
		void setContentPaneArea(sxRect area);

		float getHorizontalStepSize();
		void setHorizontalStepSize(float step);

		float getHorizontalOverlapSize();
		void setHorizontalOverlapSize(float overlap);

		float getHorizontalScrollPosition();
		void setHorizontalScrollPosition(float position);

		float getVerticalStepSize();
		void setVerticalStepSize(float step);

		float getVerticalOverlapSize();
		void setVerticalOverlapSize(float overlap);

		float getVerticalScrollPosition();
		void setVerticalScrollPosition(float position);

		sxRect getViewableArea();
		sxScrollbar getVertScrollbar();
		sxScrollbar getHorzScrollbar();
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxScrollablePane

#define SXSCROLLABLEPANE_METHODS 24

namespace RFSX
{

class SX_IMPL_TYPE: public sxWindow
{
public:
	SX_IMPL_TYPE(CEGUI::ScrollablePane* pane) : sxWindow(pane) {}
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);

	CEGUI::ScrollablePane* ScrollablePane() { return static_cast<CEGUI::ScrollablePane*>(Window()); }

protected:
	SX_METHOD_DECL(getContentPane);
	SX_METHOD_DECL(isVertScrollbarAlwaysShown);
	SX_METHOD_DECL(setShowVertScrollbar);
	SX_METHOD_DECL(isHorzScrollbarAlwaysShown);
	SX_METHOD_DECL(setShowHorzScrollbar);
	SX_METHOD_DECL(isContentPaneAutoSized);
	SX_METHOD_DECL(setContentPaneAutoSized);
	SX_METHOD_DECL(getContentPaneArea);
	SX_METHOD_DECL(setContentPaneArea);
	SX_METHOD_DECL(getHorizontalStepSize);
	SX_METHOD_DECL(setHorizontalStepSize);
	SX_METHOD_DECL(getHorizontalOverlapSize);
	SX_METHOD_DECL(setHorizontalOverlapSize);
	SX_METHOD_DECL(getHorizontalScrollPosition);
	SX_METHOD_DECL(setHorizontalScrollPosition);
	SX_METHOD_DECL(getVerticalStepSize);
	SX_METHOD_DECL(setVerticalStepSize);
	SX_METHOD_DECL(getVerticalOverlapSize);
	SX_METHOD_DECL(setVerticalOverlapSize);
	SX_METHOD_DECL(getVerticalScrollPosition);
	SX_METHOD_DECL(setVerticalScrollPosition);
	SX_METHOD_DECL(getViewableArea);
	SX_METHOD_DECL(getVertScrollbar);
	SX_METHOD_DECL(getHorzScrollbar);

private:
	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_SCROLLABLEPANE_H_
