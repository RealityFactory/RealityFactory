/************************************************************************************//**
 * @file sxCEGUIEditbox.h
 * @brief Class declaration for RFSX Editbox object
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_EDITBOX_H_
#define _SX_EDITBOX_H_

#include "RFSX.h"
#include "../sxCEGUIWindow.h"

// sxEditbox class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		bool hasInputFocus();
		bool isReadOnly();
		bool isTextMasked();
		bool isTextValid();

		skString getValidationString();

		int getCaratIndex();
		int getSelectionStartIndex();
		int getSelectionEndIndex();
		int getSelectionLength();
		int getMaskCodePoint();
		int getMaxTextLength();

		void setReadOnly(bool setting);
		void setTextMasked(bool setting);
		void setValidationString(skString validation_string);
		void setCaratIndex(int carat_pos);
		void setSelection(int start_pos, int end_pos);
		void setMaskCodePoint(int code_point);
		void setMaxTextLength(int max_len);
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxEditbox

#define SXEDITBOX_METHODS 18

namespace RFSX
{

class SX_IMPL_TYPE: public sxWindow
{
public:
	SX_IMPL_TYPE(CEGUI::Editbox* editbox) : sxWindow(editbox) {}
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);

	CEGUI::Editbox* Editbox() { return static_cast<CEGUI::Editbox*>(Window()); }

protected:
	SX_METHOD_DECL(hasInputFocus);
	SX_METHOD_DECL(isReadOnly);
	SX_METHOD_DECL(isTextMasked);
	SX_METHOD_DECL(isTextValid);
	SX_METHOD_DECL(getValidationString);
	SX_METHOD_DECL(getCaratIndex);
	SX_METHOD_DECL(getSelectionStartIndex);
	SX_METHOD_DECL(getSelectionEndIndex);
	SX_METHOD_DECL(getSelectionLength);
	SX_METHOD_DECL(getMaskCodePoint);
	SX_METHOD_DECL(getMaxTextLength);
	SX_METHOD_DECL(setReadOnly);
	SX_METHOD_DECL(setTextMasked);
	SX_METHOD_DECL(setValidationString);
	SX_METHOD_DECL(setCaratIndex);
	SX_METHOD_DECL(setSelection);
	SX_METHOD_DECL(setMaskCodePoint);
	SX_METHOD_DECL(setMaxTextLength);

private:
	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_EDITBOX_H_
