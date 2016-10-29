/************************************************************************************//**
 * @file sxCEGUIMultiLineEditbox.cpp
 * @brief Implementation for RFSX MultiLineEditbox object.
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUIMultiLineEditbox.h"
#include "sxCEGUIScrollbar.h"
#include "../sxCEGUIImage.h"
#include "../sxCEGUIRect.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxMultiLineEditbox

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxTEMPLATE implementation
//////////////////////////////////////////////////////////////////////////////////////////

bool SX_IMPL_TYPE::method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
{
	if(h_method(this, m_name, args, r_val, ctxt))
		return true;
	else
		return sxWindow::method(m_name, args, r_val, ctxt);
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxMultiLineEditbox method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(hasInputFocus)
{
	//bool hasInputFocus() const;
	r_val = caller->MultiLineEditbox()->hasInputFocus();
	return true;
}


SX_METHOD_IMPL(isReadOnly)
{
	//bool isReadOnly() const;
	r_val = caller->MultiLineEditbox()->isReadOnly();
	return true;
}


SX_METHOD_IMPL(getCaratIndex)
{
	//unsigned long getCaratIndex() const;
	r_val = static_cast<int>(caller->MultiLineEditbox()->getCaratIndex());
	return true;
}


SX_METHOD_IMPL(getSelectionStartIndex)
{
	//unsigned long getSelectionStartIndex() const;
	r_val = static_cast<int>(caller->MultiLineEditbox()->getSelectionStartIndex());
	return true;
}


SX_METHOD_IMPL(getSelectionEndIndex)
{
	//unsigned long getSelectionEndIndex() const;
	r_val = static_cast<int>(caller->MultiLineEditbox()->getSelectionEndIndex());
	return true;
}


SX_METHOD_IMPL(getSelectionLength)
{
	//unsigned long getSelectionLength() const;
	r_val = static_cast<int>(caller->MultiLineEditbox()->getSelectionLength());
	return true;
}


SX_METHOD_IMPL(getMaxTextLength)
{
	//unsigned long getMaxTextLength() const;
	r_val = static_cast<int>(caller->MultiLineEditbox()->getMaxTextLength());
	return true;
}


SX_METHOD_IMPL(isWordWrapped)
{
	//bool isWordWrapped() const;
	r_val = caller->MultiLineEditbox()->isWordWrapped();
	return true;
}


SX_METHOD_IMPL(getVertScrollbar)
{
	//Scrollbar* getVertScrollbar() const;
	r_val.assignObject(new sxScrollbar(caller->MultiLineEditbox()->getVertScrollbar()), true);
	return true;
}


SX_METHOD_IMPL(isVertScrollbarAlwaysShown)
{
	//bool isVertScrollbarAlwaysShown() const;
	r_val = caller->MultiLineEditbox()->isVertScrollbarAlwaysShown();
	return true;
}


SX_METHOD_IMPL(getHorzScrollbar)
{
	//Scrollbar* getHorzScrollbar() const;
	r_val.assignObject(new sxScrollbar(caller->MultiLineEditbox()->getHorzScrollbar()), true);
	return true;
}


SX_METHOD_IMPL(getTextRenderArea)
{
	//Rect getTextRenderArea() const;
	r_val.assignObject(new sxRect(caller->MultiLineEditbox()->getTextRenderArea()), true);
	return true;
}


SX_METHOD_IMPL(getLineNumberFromIndex)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	//unsigned long getLineNumberFromIndex(unsigned long index) const;
	r_val = static_cast<int>(caller->MultiLineEditbox()->getLineNumberFromIndex(args[0].intValue()));
	return true;
}


SX_METHOD_IMPL(setReadOnly)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	//void setReadOnly(bool setting);
	caller->MultiLineEditbox()->setReadOnly(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(setCaratIndex)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	//void setCaratIndex(unsigned long carat_pos);
	caller->MultiLineEditbox()->setCaratIndex(args[0].intValue());
	return true;
}


SX_METHOD_IMPL(setSelection)
{
	skRValue::RType rtypes[] = { skRValue::T_Int, skRValue::T_Int };

	if(!CheckParams(2, 2, args, rtypes))
		return false;

	//void setSelection(unsigned long start_pos, unsigned long end_pos);
	caller->MultiLineEditbox()->setSelection(args[0].intValue(), args[1].intValue());
	return true;
}


SX_METHOD_IMPL(setMaxTextLength)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	//void setMaxTextLength(unsigned long max_len);
	caller->MultiLineEditbox()->setMaxTextLength(args[0].intValue());
	return true;
}


SX_METHOD_IMPL(ensureCaratIsVisible)
{
	//void ensureCaratIsVisible();
	caller->MultiLineEditbox()->ensureCaratIsVisible();
	return true;
}


SX_METHOD_IMPL(setWordWrapping)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	//void setWordWrapping(bool setting);
	caller->MultiLineEditbox()->setWordWrapping(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(setShowVertScrollbar)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	//void setShowVertScrollbar(bool setting);
	caller->MultiLineEditbox()->setShowVertScrollbar(args[0].boolValue());
	return true;
}

SX_METHOD_IMPL(setSelectionBrushImage)
{
	if(args.entries() != 1)
		return false;

	sxImage *image = IS_IMAGE(args[0]);
	if(!image)
		return false;

	//void setSelectionBrushImage(const Image* image);
	caller->MultiLineEditbox()->setSelectionBrushImage(image->Image());
	return true;
}


SX_METHOD_IMPL(getSelectionBrushImage)
{
	//const Image* getSelectionBrushImage() const;
	r_val.assignObject(new sxImage(caller->MultiLineEditbox()->getSelectionBrushImage()), true);
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxMultiLineEditbox initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXMULTILINEEDITBOX_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(hasInputFocus);
	SX_ADD_METHOD(isReadOnly);
	SX_ADD_METHOD(getCaratIndex);
	SX_ADD_METHOD(getSelectionStartIndex);
	SX_ADD_METHOD(getSelectionEndIndex);
	SX_ADD_METHOD(getSelectionLength);
	SX_ADD_METHOD(getMaxTextLength);
	SX_ADD_METHOD(isWordWrapped);
	SX_ADD_METHOD(getVertScrollbar);
	SX_ADD_METHOD(isVertScrollbarAlwaysShown);
	SX_ADD_METHOD(getHorzScrollbar);
	SX_ADD_METHOD(getTextRenderArea);
	SX_ADD_METHOD(getLineNumberFromIndex);
	SX_ADD_METHOD(setReadOnly);
	SX_ADD_METHOD(setCaratIndex);
	SX_ADD_METHOD(setSelection);
	SX_ADD_METHOD(setMaxTextLength);
	SX_ADD_METHOD(ensureCaratIsVisible);
	SX_ADD_METHOD(setWordWrapping);
	SX_ADD_METHOD(setShowVertScrollbar);
	SX_ADD_METHOD(setSelectionBrushImage);
	SX_ADD_METHOD(getSelectionBrushImage);
}

} // namespace RFSX

#undef SX_IMPL_TYPE
