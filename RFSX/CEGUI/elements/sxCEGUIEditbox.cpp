/************************************************************************************//**
 * @file sxCEGUIEditbox.cpp
 * @brief Implementation for RFSX Editbox object.
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUIEditbox.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxEditbox

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxEditbox implementation
//////////////////////////////////////////////////////////////////////////////////////////

bool SX_IMPL_TYPE::method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
{
	if(h_method(this, m_name, args, r_val, ctxt))
		return true;
	else
		return sxWindow::method(m_name, args, r_val, ctxt);
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxEditbox method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(hasInputFocus)
{
	//bool hasInputFocus() const;
	r_val = caller->Editbox()->hasInputFocus();
	return true;
}


SX_METHOD_IMPL(isReadOnly)
{
	//bool isReadOnly() const;
	r_val = caller->Editbox()->isReadOnly();
	return true;
}


SX_METHOD_IMPL(isTextMasked)
{
	//bool isTextMasked() const;
	r_val = caller->Editbox()->isTextMasked();
	return true;
}


SX_METHOD_IMPL(isTextValid)
{
	//bool isTextValid() const;
	r_val = caller->Editbox()->isTextValid();
	return true;
}


SX_METHOD_IMPL(getValidationString)
{
	//string getValidationString() const;
	r_val = skString(caller->Editbox()->getValidationString().c_str());
	return true;
}


SX_METHOD_IMPL(getCaratIndex)
{
	//unsigned long getCaratIndex() const;
	r_val = static_cast<int>(caller->Editbox()->getCaratIndex());
	return true;
}


SX_METHOD_IMPL(getSelectionStartIndex)
{
	//unsigned long getSelectionStartIndex() const;
	r_val = static_cast<int>(caller->Editbox()->getSelectionStartIndex());
	return true;
}


SX_METHOD_IMPL(getSelectionEndIndex)
{
	//unsigned long getSelectionEndIndex() const;
	r_val = static_cast<int>(caller->Editbox()->getSelectionEndIndex());
	return true;
}


SX_METHOD_IMPL(getSelectionLength)
{
	//unsigned long getSelectionLength() const;
	r_val = static_cast<int>(caller->Editbox()->getSelectionLength());
	return true;
}


SX_METHOD_IMPL(getMaskCodePoint)
{
	//unsigned long getMaskCodePoint() const;
	r_val = static_cast<int>(caller->Editbox()->getMaskCodePoint());
	return true;
}


SX_METHOD_IMPL(getMaxTextLength)
{
	//unsigned long getMaxTextLength() const;
	r_val = static_cast<int>(caller->Editbox()->getMaxTextLength());
	return true;
}


SX_METHOD_IMPL(setReadOnly)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	//void setReadOnly(bool setting);
	caller->Editbox()->setReadOnly(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(setTextMasked)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	//void setTextMasked(bool setting);
	caller->Editbox()->setTextMasked(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(setValidationString)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	//void setValidationString(string validation_string);
	caller->Editbox()->setValidationString(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	return true;
}


SX_METHOD_IMPL(setCaratIndex)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	//void setCaratIndex(unsigned long carat_pos);
	caller->Editbox()->setCaratIndex(args[0].intValue());
	return true;
}


SX_METHOD_IMPL(setSelection)
{
	skRValue::RType rtypes[] = { skRValue::T_Int, skRValue::T_Int };

	if(!CheckParams(2, 2, args, rtypes))
		return false;

	//void setSelection(unsigned long start_pos, unsigned long end_pos);
	caller->Editbox()->setSelection(args[0].intValue(), args[1].intValue());
	return true;
}


SX_METHOD_IMPL(setMaskCodePoint)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	//void setMaskCodePoint(unsigned long code_point);
	caller->Editbox()->setMaskCodePoint(args[0].intValue());
	return true;
}


SX_METHOD_IMPL(setMaxTextLength)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	//void setMaxTextLength(unsigned long max_len);
	caller->Editbox()->setMaxTextLength(args[0].intValue());
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxEditbox initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXEDITBOX_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(hasInputFocus);
	SX_ADD_METHOD(isReadOnly);
	SX_ADD_METHOD(isTextMasked);
	SX_ADD_METHOD(isTextValid);
	SX_ADD_METHOD(getValidationString);
	SX_ADD_METHOD(getCaratIndex);
	SX_ADD_METHOD(getSelectionStartIndex);
	SX_ADD_METHOD(getSelectionEndIndex);
	SX_ADD_METHOD(getSelectionLength);
	SX_ADD_METHOD(getMaskCodePoint);
	SX_ADD_METHOD(getMaxTextLength);
	SX_ADD_METHOD(setReadOnly);
	SX_ADD_METHOD(setTextMasked);
	SX_ADD_METHOD(setValidationString);
	SX_ADD_METHOD(setCaratIndex);
	SX_ADD_METHOD(setSelection);
	SX_ADD_METHOD(setMaskCodePoint);
	SX_ADD_METHOD(setMaxTextLength);
}

} // namespace RFSX

#undef SX_IMPL_TYPE
