/************************************************************************************//**
 * @file sxCEGUIListHeader.h
 * @brief Class declaration for RFSX ListHeader object
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_LISTHEADER_H_
#define _SX_LISTHEADER_H_

#include "RFSX.h"
#include "../sxCEGUIWindow.h"

// sxListHeader class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		int getColumnCount();

		ListHeaderSegment& getSegmentFromColumn(int column);
		ListHeaderSegment& getSegmentFromID(int id);
		ListHeaderSegment& getSortSegment();

		int getColumnFromSegment(sxListHeaderSegment segment);
		int getColumnFromID(int id);
		int getSortColumn();
		int getColumnWithText(skString text);

		float getPixelOffsetToSegment(sxListHeaderSegment segment);
		float getPixelOffsetToColumn(int column);
		float getTotalSegmentsPixelExtent();
		sxUDim getColumnWidth(int column);

		int getSortDirection();

		bool isSortingEnabled();
		bool isColumnSizingEnabled();
		bool isColumnDraggingEnabled();

		float getSegmentOffset();

		void setSortingEnabled(bool setting);
		void setSortDirection(int direction);
		void setSortSegment(sxListHeaderSegment segment);
		void setSortColumn(int column);
		void setSortColumnFromID(int id);

		void setColumnSizingEnabled(bool setting);
		void setColumnDraggingEnabled(bool setting);
		void addColumn(skString text, int id, sxUDim width);

		void insertColumn(skString text, int id, sxUDim width, int position);
		void insertColumn(skString text, int id, sxUDim width, sxListHeaderSegment position);
		void removeColumn(int column);
		void removeSegment(sxListHeaderSegment segment);

		void moveColumn(int column, int position);
		void moveColumn(int column, sxListHeaderSegment position);

		void moveSegment(sxListHeaderSegment segment, int position);
		void moveSegment(sxListHeaderSegment segment, sxListHeaderSegment position);

		void setSegmentOffset(float offset);

		void setColumnWidth(int column, sxUDim width);
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxListHeader

#define SXLISTHEADER_METHODS 32

namespace RFSX
{

class SX_IMPL_TYPE: public sxWindow
{
public:
	SX_IMPL_TYPE(CEGUI::ListHeader* header) : sxWindow(header) {}
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);

	CEGUI::ListHeader* ListHeader() { return static_cast<CEGUI::ListHeader*>(Window()); }

protected:
	SX_METHOD_DECL(getColumnCount);
	SX_METHOD_DECL(getSegmentFromColumn);
	SX_METHOD_DECL(getSegmentFromID);
	SX_METHOD_DECL(getSortSegment);
	SX_METHOD_DECL(getColumnFromSegment);
	SX_METHOD_DECL(getColumnFromID);
	SX_METHOD_DECL(getSortColumn);
	SX_METHOD_DECL(getColumnWithText);
	SX_METHOD_DECL(getPixelOffsetToSegment);
	SX_METHOD_DECL(getPixelOffsetToColumn);
	SX_METHOD_DECL(getTotalSegmentsPixelExtent);
	SX_METHOD_DECL(getColumnWidth);
	SX_METHOD_DECL(getSortDirection);
	SX_METHOD_DECL(isSortingEnabled);
	SX_METHOD_DECL(isColumnSizingEnabled);
	SX_METHOD_DECL(isColumnDraggingEnabled);
	SX_METHOD_DECL(getSegmentOffset);
	SX_METHOD_DECL(setSortingEnabled);
	SX_METHOD_DECL(setSortDirection);
	SX_METHOD_DECL(setSortSegment);
	SX_METHOD_DECL(setSortColumn);
	SX_METHOD_DECL(setSortColumnFromID);
	SX_METHOD_DECL(setColumnSizingEnabled);
	SX_METHOD_DECL(setColumnDraggingEnabled);
	SX_METHOD_DECL(addColumn);
	SX_METHOD_DECL(insertColumn);
	SX_METHOD_DECL(removeColumn);
	SX_METHOD_DECL(removeSegment);
	SX_METHOD_DECL(moveColumn);
	SX_METHOD_DECL(moveSegment);
	SX_METHOD_DECL(setSegmentOffset);
	SX_METHOD_DECL(setColumnWidth);

private:
	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_LISTHEADER_H_
