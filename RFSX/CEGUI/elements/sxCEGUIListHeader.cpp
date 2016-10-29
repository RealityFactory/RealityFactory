/************************************************************************************//**
 * @file sxCEGUIListHeader.cpp
 * @brief Implementation for RFSX ListHeader object.
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUIListHeader.h"
#include "sxCEGUIListHeaderSegment.h"
#include "../sxCEGUIUDim.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxListHeader

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxListHeader implementation
//////////////////////////////////////////////////////////////////////////////////////////

bool SX_IMPL_TYPE::method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
{
	if(h_method(this, m_name, args, r_val, ctxt))
		return true;
	else
		return sxWindow::method(m_name, args, r_val, ctxt);
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxListHeader method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(getColumnCount)
{
	//unsigned int getColumnCount() const;
	r_val = static_cast<int>(caller->ListHeader()->getColumnCount());
	return true;
}


SX_METHOD_IMPL(getSegmentFromColumn)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	//ListHeaderSegment& getSegmentFromColumn(unsigned int column) const;
	r_val.assignObject(new sxListHeaderSegment(&caller->ListHeader()->getSegmentFromColumn(args[0].intValue())), true);
	return true;
}


SX_METHOD_IMPL(getSegmentFromID)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	//ListHeaderSegment& getSegmentFromID(unsigned int id) const;
	r_val.assignObject(new sxListHeaderSegment(&caller->ListHeader()->getSegmentFromID(args[0].intValue())), true);
	return true;
}


SX_METHOD_IMPL(getSortSegment)
{
	//ListHeaderSegment& getSortSegment() const;
	r_val.assignObject(new sxListHeaderSegment(&caller->ListHeader()->getSortSegment()), true);
	return true;
}


SX_METHOD_IMPL(getColumnFromSegment)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Object)
		return false;

	sxListHeaderSegment *segment = static_cast<sxListHeaderSegment*>(args[0].obj());

	//unsigned int getColumnFromSegment(const ListHeaderSegment& segment) const;
	r_val = static_cast<int>(caller->ListHeader()->getColumnFromSegment(*(segment->ListHeaderSegment())));
	return true;
}


SX_METHOD_IMPL(getColumnFromID)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	//unsigned int getColumnFromID(unsigned int id) const;
	r_val = static_cast<int>(caller->ListHeader()->getColumnFromID(args[0].intValue()));
	return true;
}


SX_METHOD_IMPL(getSortColumn)
{
	//unsigned int getSortColumn() const;
	r_val = static_cast<int>(caller->ListHeader()->getSortColumn());
	return true;
}


SX_METHOD_IMPL(getColumnWithText)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	//unsigned int getColumnWithText(string text) const;
	r_val = static_cast<int>(caller->ListHeader()->getColumnWithText(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str())));
	return true;
}


SX_METHOD_IMPL(getPixelOffsetToSegment)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Object)
		return false;

	sxListHeaderSegment *segment = static_cast<sxListHeaderSegment*>(args[0].obj());

	//float getPixelOffsetToSegment(const ListHeaderSegment& segment) const;
	r_val = caller->ListHeader()->getPixelOffsetToSegment(*(segment->ListHeaderSegment()));
	return true;
}


SX_METHOD_IMPL(getPixelOffsetToColumn)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	//float getPixelOffsetToColumn(unsigned int column) const;
	r_val = caller->ListHeader()->getPixelOffsetToColumn(args[0].intValue());
	return true;
}


SX_METHOD_IMPL(getTotalSegmentsPixelExtent)
{
	//float getTotalSegmentsPixelExtent() const;
	r_val = caller->ListHeader()->getTotalSegmentsPixelExtent();
	return true;
}


SX_METHOD_IMPL(getColumnWidth)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	//UDim  getColumnWidth(unsigned int column) const;
	r_val.assignObject(new sxUDim(caller->ListHeader()->getColumnWidth(args[0].intValue())), true);
	return true;
}


SX_METHOD_IMPL(getSortDirection)
{
	//ListHeaderSegment::SortDirection getSortDirection() const;
	//r_val = static_cast<int>(caller->ListHeader()->getSortDirection());
	r_val = caller->ListHeader()->getSortDirection();
	return true;
}


SX_METHOD_IMPL(isSortingEnabled)
{
	//bool isSortingEnabled() const;
	r_val = caller->ListHeader()->isSortingEnabled();
	return true;
}


SX_METHOD_IMPL(isColumnSizingEnabled)
{
	//bool isColumnSizingEnabled() const;
	r_val = caller->ListHeader()->isColumnSizingEnabled();
	return true;
}


SX_METHOD_IMPL(isColumnDraggingEnabled)
{
	//bool isColumnDraggingEnabled() const;
	r_val = caller->ListHeader()->isColumnDraggingEnabled();
	return true;
}


SX_METHOD_IMPL(getSegmentOffset)
{
	//float getSegmentOffset() const;
	r_val = caller->ListHeader()->getSegmentOffset();
	return true;
}


SX_METHOD_IMPL(setSortingEnabled)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	//void setSortingEnabled(bool setting);
	caller->ListHeader()->setSortingEnabled(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(setSortDirection)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	//void setSortDirection(ListHeaderSegment::SortDirection direction);
	caller->ListHeader()->setSortDirection(static_cast<CEGUI::ListHeaderSegment::SortDirection>(args[0].intValue()));
	return true;
}


SX_METHOD_IMPL(setSortSegment)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Object)
		return false;

	sxListHeaderSegment *segment = static_cast<sxListHeaderSegment*>(args[0].obj());

	//void setSortSegment(const ListHeaderSegment& segment);
	caller->ListHeader()->setSortSegment(*(segment->ListHeaderSegment()));
	return true;
}


SX_METHOD_IMPL(setSortColumn)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	//void setSortColumn(unsigned int column);
	caller->ListHeader()->setSortColumn(args[0].intValue());
	return true;
}


SX_METHOD_IMPL(setSortColumnFromID)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	//void setSortColumnFromID(unsigned int id);
	caller->ListHeader()->setSortColumnFromID(args[0].intValue());
	return true;
}


SX_METHOD_IMPL(setColumnSizingEnabled)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	//void setColumnSizingEnabled(bool setting);
	caller->ListHeader()->setColumnSizingEnabled(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(setColumnDraggingEnabled)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	//void setColumnDraggingEnabled(bool setting);
	caller->ListHeader()->setColumnDraggingEnabled(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(addColumn)
{
	skRValue::RType rtypes[] = {	skRValue::T_String,
									skRValue::T_Int,
									skRValue::T_Object };

	if(!CheckParams(3, 3, args, rtypes))
		return false;

	sxUDim *udim = IS_UDIM(args[2]);
	if(!udim)
		return false;

	//void addColumn(string text, unsigned int id, const UDim& width);
	caller->ListHeader()->addColumn(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()),
									args[1].intValue(),
									udim->UDimConstRef());
	return true;
}


SX_METHOD_IMPL(insertColumn)
{
	if(args.entries() != 4)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	if(args[1].type() != skRValue::T_Int)
		return false;

	sxUDim *udim = IS_UDIM(args[2]);
	if(!udim)
		return false;

	//void insertColumn(string text, unsigned int id, const UDim& width, unsigned int position);
	if(args[3].type() == skRValue::T_Int)
	{
		caller->ListHeader()->insertColumn(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()),
											args[1].intValue(),
											udim->UDimConstRef(),
											args[3].intValue());
		return true;
	}

	//void insertColumn(string text, unsigned int id, const UDim& width, const ListHeaderSegment& position);
	if(args[3].type() != skRValue::T_Object)
		return false;

	sxListHeaderSegment *segment = static_cast<sxListHeaderSegment*>(args[3].obj());

	caller->ListHeader()->insertColumn(	reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()),
										args[1].intValue(),
										udim->UDimConstRef(),
										*(segment->ListHeaderSegment()));
	return true;
}


SX_METHOD_IMPL(removeColumn)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	//void removeColumn(unsigned int column);
	caller->ListHeader()->removeColumn(args[0].intValue());
	return true;
}


SX_METHOD_IMPL(removeSegment)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Object)
		return false;

	sxListHeaderSegment *segment = static_cast<sxListHeaderSegment*>(args[0].obj());

	//void removeSegment(const ListHeaderSegment& segment);
	caller->ListHeader()->removeSegment(*(segment->ListHeaderSegment()));
	return true;
}


SX_METHOD_IMPL(moveColumn)
{
	if(args.entries() != 2)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	//void moveColumn(unsigned int column, unsigned int position);
	if(args[1].type() == skRValue::T_Int)
	{
		caller->ListHeader()->moveColumn(args[0].intValue(), args[1].intValue());
		return true;
	}

	//void moveColumn(unsigned int column, const ListHeaderSegment& position);
	if(args[1].type() != skRValue::T_Object)
		return false;

	sxListHeaderSegment *segment = static_cast<sxListHeaderSegment*>(args[1].obj());

	caller->ListHeader()->moveColumn(args[0].intValue(), *(segment->ListHeaderSegment()));

	return true;
}


SX_METHOD_IMPL(moveSegment)
{
	if(args.entries() != 2)
		return false;

	if(args[0].type() != skRValue::T_Object)
		return false;

	sxListHeaderSegment *segment = static_cast<sxListHeaderSegment*>(args[0].obj());

	//void moveSegment(const ListHeaderSegment& segment, unsigned int position);
	if(args[1].type() == skRValue::T_Int)
	{
		caller->ListHeader()->moveSegment(*(segment->ListHeaderSegment()), args[1].intValue());
		return true;
	}

	//void moveSegment(const ListHeaderSegment& segment, const ListHeaderSegment& position);
	if(args[1].type() != skRValue::T_Object)
		return false;

	sxListHeaderSegment *position = static_cast<sxListHeaderSegment*>(args[1].obj());

	caller->ListHeader()->moveSegment(*(segment->ListHeaderSegment()), *(position->ListHeaderSegment()));

	return true;
}


SX_METHOD_IMPL(setSegmentOffset)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Float)
		return false;

	//void setSegmentOffset(float offset);
	caller->ListHeader()->setSegmentOffset(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(setColumnWidth)
{
	if(args.entries() != 2)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	sxUDim *udim = IS_UDIM(args[1]);
	if(!udim)
		return false;

	//void setColumnWidth(unsigned int column, const UDim& width);
	caller->ListHeader()->setColumnWidth(args[0].intValue(), udim->UDimConstRef());
	return true;
}


//////////////////////////////////////////////////////////////////////////////////////////
// sxListHeader initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXLISTHEADER_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(getColumnCount);
	SX_ADD_METHOD(getSegmentFromColumn);
	SX_ADD_METHOD(getSegmentFromID);
	SX_ADD_METHOD(getSortSegment);
	SX_ADD_METHOD(getColumnFromSegment);
	SX_ADD_METHOD(getColumnFromID);
	SX_ADD_METHOD(getSortColumn);
	SX_ADD_METHOD(getColumnWithText);
	SX_ADD_METHOD(getPixelOffsetToSegment);
	SX_ADD_METHOD(getPixelOffsetToColumn);
	SX_ADD_METHOD(getTotalSegmentsPixelExtent);
	SX_ADD_METHOD(getColumnWidth);
	SX_ADD_METHOD(getSortDirection);
	SX_ADD_METHOD(isSortingEnabled);
	SX_ADD_METHOD(isColumnSizingEnabled);
	SX_ADD_METHOD(isColumnDraggingEnabled);
	SX_ADD_METHOD(getSegmentOffset);
	SX_ADD_METHOD(setSortingEnabled);
	SX_ADD_METHOD(setSortDirection);
	SX_ADD_METHOD(setSortSegment);
	SX_ADD_METHOD(setSortColumn);
	SX_ADD_METHOD(setSortColumnFromID);
	SX_ADD_METHOD(setColumnSizingEnabled);
	SX_ADD_METHOD(setColumnDraggingEnabled);
	SX_ADD_METHOD(addColumn);
	SX_ADD_METHOD(insertColumn);
	SX_ADD_METHOD(removeColumn);
	SX_ADD_METHOD(removeSegment);
	SX_ADD_METHOD(moveColumn);
	SX_ADD_METHOD(moveSegment);
	SX_ADD_METHOD(setSegmentOffset);
	SX_ADD_METHOD(setColumnWidth);
}

} // namespace RFSX

#undef SX_IMPL_TYPE // And that's it in a nutshell
