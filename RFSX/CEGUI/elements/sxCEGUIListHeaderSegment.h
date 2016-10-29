/************************************************************************************//**
 * @file sxCEGUIListHeaderSegment.h
 * @brief Class declaration for RFSX ListHeaderSegment object
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_LISTHEADERSEGMENT_H_
#define _SX_LISTHEADERSEGMENT_H_

#include "RFSX.h"
#include "../sxCEGUIWindow.h"

// sxListHeaderSegment class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		bool isSizingEnabled();
		int getSortDirection();
		bool isDragMovingEnabled();
		sxVector2 getDragMoveOffset();
		bool isClickable();

		void setSizingEnabled(bool setting);
		void setSortDirection(int sort_dir);
		void setDragMovingEnabled(bool setting);
		void setClickable(bool setting);
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxListHeaderSegment

#define SXLISTHEADERSEGMENT_METHODS 9

namespace RFSX
{

class SX_IMPL_TYPE: public sxWindow
{
public:
	SX_IMPL_TYPE(CEGUI::ListHeaderSegment* segment) : sxWindow(segment) {}
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);

	CEGUI::ListHeaderSegment* ListHeaderSegment() { return static_cast<CEGUI::ListHeaderSegment*>(Window()); }

protected:
	SX_METHOD_DECL(isSizingEnabled);
	SX_METHOD_DECL(getSortDirection);
	SX_METHOD_DECL(isDragMovingEnabled);
	SX_METHOD_DECL(getDragMoveOffset);
	SX_METHOD_DECL(isClickable);
	SX_METHOD_DECL(setSizingEnabled);
	SX_METHOD_DECL(setSortDirection);
	SX_METHOD_DECL(setDragMovingEnabled);
	SX_METHOD_DECL(setClickable);

private:
	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_LISTHEADERSEGMENT_H_
