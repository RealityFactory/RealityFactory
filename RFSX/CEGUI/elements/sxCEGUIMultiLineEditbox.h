/************************************************************************************//**
 * @file sxCEGUIMultiLineEditbox.h
 * @brief Class declaration for RFSX MultiLineEditbox object
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_MULTILINEEDITBOX_H_
#define _SX_MULTILINEEDITBOX_H_

#include "RFSX.h"
#include "../sxCEGUIWindow.h"

// sxMultiLineEditbox class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		bool hasInputFocus();
		bool isReadOnly();
		int getCaratIndex();
		int getSelectionStartIndex();
		int getSelectionEndIndex();
		int getSelectionLength();
		int getMaxTextLength();
		bool isWordWrapped();

		sxScrollbar getVertScrollbar();
		bool isVertScrollbarAlwaysShown();
		sxScrollbar getHorzScrollbar();
		sxRect getTextRenderArea();
		int getLineNumberFromIndex(int index);

		void setReadOnly(bool setting);
		void setCaratIndex(int carat_pos);
		void setSelection(int start_pos, int end_pos);
		void setMaxTextLength(int max_len);
		void ensureCaratIsVisible();
		void setWordWrapping(bool setting);
		void setShowVertScrollbar(bool setting);

		void setSelectionBrushImage(sxImage image);
		sxImage getSelectionBrushImage();
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxMultiLineEditbox

#define SXMULTILINEEDITBOX_METHODS 22

namespace RFSX
{

class SX_IMPL_TYPE: public sxWindow
{
public:
	SX_IMPL_TYPE(CEGUI::MultiLineEditbox* editbox) : sxWindow(editbox) {}
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);

	CEGUI::MultiLineEditbox* MultiLineEditbox() { return static_cast<CEGUI::MultiLineEditbox*>(Window()); }

protected:
	SX_METHOD_DECL(hasInputFocus);
	SX_METHOD_DECL(isReadOnly);
	SX_METHOD_DECL(getCaratIndex);
	SX_METHOD_DECL(getSelectionStartIndex);
	SX_METHOD_DECL(getSelectionEndIndex);
	SX_METHOD_DECL(getSelectionLength);
	SX_METHOD_DECL(getMaxTextLength);
	SX_METHOD_DECL(isWordWrapped);
	SX_METHOD_DECL(getVertScrollbar);
	SX_METHOD_DECL(isVertScrollbarAlwaysShown);
	SX_METHOD_DECL(getHorzScrollbar);
	SX_METHOD_DECL(getTextRenderArea);
	SX_METHOD_DECL(getLineNumberFromIndex);
	SX_METHOD_DECL(setReadOnly);
	SX_METHOD_DECL(setCaratIndex);
	SX_METHOD_DECL(setSelection);
	SX_METHOD_DECL(setMaxTextLength);
	SX_METHOD_DECL(ensureCaratIsVisible);
	SX_METHOD_DECL(setWordWrapping);
	SX_METHOD_DECL(setShowVertScrollbar);
	SX_METHOD_DECL(setSelectionBrushImage);
	SX_METHOD_DECL(getSelectionBrushImage);

private:
	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_MULTILINEEDITBOX_H_
