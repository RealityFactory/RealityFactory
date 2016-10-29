/************************************************************************************//**
 * @file sxCEGUIScrollablePane.cpp
 * @brief Implementation for RFSX ScrollablePane object.
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUIScrollablePane.h"
#include "sxCEGUIScrollbar.h"
#include "sxCEGUIScrolledContainer.h"
#include "../sxCEGUIRect.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxScrollablePane

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxScrollablePane implementation
//////////////////////////////////////////////////////////////////////////////////////////

bool SX_IMPL_TYPE::method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
{
	if(h_method(this, m_name, args, r_val, ctxt))
		return true;
	else
		return sxWindow::method(m_name, args, r_val, ctxt);
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxScrollablePane method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(getContentPane)
{
	//const ScrolledContainer* getContentPane() const;
	r_val.assignObject(new sxScrolledContainer(const_cast<CEGUI::ScrolledContainer*>(caller->ScrollablePane()->getContentPane())), true);
	return true;
}


SX_METHOD_IMPL(isVertScrollbarAlwaysShown)
{
	//bool isVertScrollbarAlwaysShown() const;
	r_val = caller->ScrollablePane()->isVertScrollbarAlwaysShown();
	return true;
}


SX_METHOD_IMPL(setShowVertScrollbar)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	//void setShowVertScrollbar(bool setting);
	caller->ScrollablePane()->setShowVertScrollbar(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(isHorzScrollbarAlwaysShown)
{
	//bool isHorzScrollbarAlwaysShown() const;
	r_val = caller->ScrollablePane()->isHorzScrollbarAlwaysShown();
	return true;
}


SX_METHOD_IMPL(setShowHorzScrollbar)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	//void setShowHorzScrollbar(bool setting);
	caller->ScrollablePane()->setShowHorzScrollbar(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(isContentPaneAutoSized)
{
	//bool isContentPaneAutoSized() const;
	r_val = caller->ScrollablePane()->isContentPaneAutoSized();
	return true;
}


SX_METHOD_IMPL(setContentPaneAutoSized)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	//void setContentPaneAutoSized(bool setting);
	caller->ScrollablePane()->setContentPaneAutoSized(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(getContentPaneArea)
{
	//const Rect& getContentPaneArea() const;
	r_val.assignObject(new sxRect(caller->ScrollablePane()->getContentPaneArea()), true);
	return true;
}


SX_METHOD_IMPL(setContentPaneArea)
{
	if(args.entries() != 1)
		return false;

	sxRect *rect = IS_RECT(args[0]);
	if(!rect)
		return false;

	//void setContentPaneArea(const Rect& area);
	caller->ScrollablePane()->setContentPaneArea(rect->RectConstRef());
	return true;
}


SX_METHOD_IMPL(getHorizontalStepSize)
{
	//float getHorizontalStepSize() const;
	r_val = caller->ScrollablePane()->getHorizontalStepSize();
	return true;
}


SX_METHOD_IMPL(setHorizontalStepSize)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Float)
		return false;

	//void setHorizontalStepSize(float step);
	caller->ScrollablePane()->setHorizontalStepSize(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(getHorizontalOverlapSize)
{
	//float getHorizontalOverlapSize() const;
	r_val = caller->ScrollablePane()->getHorizontalOverlapSize();
	return true;
}


SX_METHOD_IMPL(setHorizontalOverlapSize)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Float)
		return false;

	//void setHorizontalOverlapSize(float overlap);
	caller->ScrollablePane()->setHorizontalOverlapSize(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(getHorizontalScrollPosition)
{
	//float getHorizontalScrollPosition() const;
	r_val = caller->ScrollablePane()->getHorizontalScrollPosition();
	return true;
}


SX_METHOD_IMPL(setHorizontalScrollPosition)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Float)
		return false;

	//void setHorizontalScrollPosition(float position);
	caller->ScrollablePane()->setHorizontalScrollPosition(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(getVerticalStepSize)
{
	//float getVerticalStepSize() const;
	r_val = caller->ScrollablePane()->getVerticalStepSize();
	return true;
}


SX_METHOD_IMPL(setVerticalStepSize)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Float)
		return false;

	//void setVerticalStepSize(float step);
	caller->ScrollablePane()->setVerticalStepSize(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(getVerticalOverlapSize)
{
	//float getVerticalOverlapSize() const;
	r_val = caller->ScrollablePane()->getVerticalOverlapSize();
	return true;
}


SX_METHOD_IMPL(setVerticalOverlapSize)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Float)
		return false;

	//void setVerticalOverlapSize(float overlap);
	caller->ScrollablePane()->setVerticalOverlapSize(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(getVerticalScrollPosition)
{
	//float getVerticalScrollPosition() const;
	r_val = caller->ScrollablePane()->getVerticalScrollPosition();
	return true;
}


SX_METHOD_IMPL(setVerticalScrollPosition)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Float)
		return false;

	//void setVerticalScrollPosition(float position);
	caller->ScrollablePane()->setVerticalScrollPosition(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(getViewableArea)
{
	//Rect getViewableArea() const;
	r_val.assignObject(new sxRect(caller->ScrollablePane()->getViewableArea()), true);
	return true;
}


SX_METHOD_IMPL(getVertScrollbar)
{
	//Scrollbar* getVertScrollbar() const;
	r_val.assignObject(new sxScrollbar(caller->ScrollablePane()->getVertScrollbar()), true);
	return true;
}


SX_METHOD_IMPL(getHorzScrollbar)
{
	//Scrollbar* getHorzScrollbar() const;
	r_val.assignObject(new sxScrollbar(caller->ScrollablePane()->getHorzScrollbar()), true);
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxScrollablePane initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXSCROLLABLEPANE_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(getContentPane);
	SX_ADD_METHOD(isVertScrollbarAlwaysShown);
	SX_ADD_METHOD(setShowVertScrollbar);
	SX_ADD_METHOD(isHorzScrollbarAlwaysShown);
	SX_ADD_METHOD(setShowHorzScrollbar);
	SX_ADD_METHOD(isContentPaneAutoSized);
	SX_ADD_METHOD(setContentPaneAutoSized);
	SX_ADD_METHOD(getContentPaneArea);
	SX_ADD_METHOD(setContentPaneArea);
	SX_ADD_METHOD(getHorizontalStepSize);
	SX_ADD_METHOD(setHorizontalStepSize);
	SX_ADD_METHOD(getHorizontalOverlapSize);
	SX_ADD_METHOD(setHorizontalOverlapSize);
	SX_ADD_METHOD(getHorizontalScrollPosition);
	SX_ADD_METHOD(setHorizontalScrollPosition);
	SX_ADD_METHOD(getVerticalStepSize);
	SX_ADD_METHOD(setVerticalStepSize);
	SX_ADD_METHOD(getVerticalOverlapSize);
	SX_ADD_METHOD(setVerticalOverlapSize);
	SX_ADD_METHOD(getVerticalScrollPosition);
	SX_ADD_METHOD(setVerticalScrollPosition);
	SX_ADD_METHOD(getViewableArea);
	SX_ADD_METHOD(getVertScrollbar);
	SX_ADD_METHOD(getHorzScrollbar);
}

} // namespace RFSX

#undef SX_IMPL_TYPE
