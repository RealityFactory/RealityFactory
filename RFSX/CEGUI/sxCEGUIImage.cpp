/************************************************************************************//**
 * @file sxCEGUIImage.cpp
 * @brief Implementation for RFSX Image object.
 * @author Daniel Queteschiner
 * @date December 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUIRect.h"
#include "sxCEGUIVector2.h"
#include "sxCEGUISize.h"
#include "sxCEGUIImage.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxImage

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxImage implementation
//////////////////////////////////////////////////////////////////////////////////////////

SX_IMPL_TYPE::~SX_IMPL_TYPE()
{
}


bool SX_IMPL_TYPE::method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
{
	if(h_method(this, m_name, args, r_val, ctxt))
		return true;
	else
		return skExecutable::method(m_name, args, r_val, ctxt);
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxImage method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(getSize)
{
	r_val.assignObject(new sxSize(caller->Image()->getSize()), true);
	return true;
}


SX_METHOD_IMPL(getWidth)
{
	r_val = caller->Image()->getWidth();
	return true;
}


SX_METHOD_IMPL(getHeight)
{
	r_val = caller->Image()->getHeight();
	return true;
}


SX_METHOD_IMPL(getOffsets)
{
	r_val.assignObject(new sxVector2(caller->Image()->getOffsets()), true);
	return true;
}


SX_METHOD_IMPL(getOffsetX)
{
	r_val = caller->Image()->getOffsetX();
	return true;
}


SX_METHOD_IMPL(getOffsetY)
{
	r_val = caller->Image()->getOffsetY();
	return true;
}


SX_METHOD_IMPL(getName)
{
	r_val = skString(caller->Image()->getName().c_str());
	return true;
}


SX_METHOD_IMPL(getImagesetName)
{
	r_val = skString(caller->Image()->getImagesetName().c_str());
	return true;
}


SX_METHOD_IMPL(getSourceTextureArea)
{
	r_val.assignObject(new sxRect(caller->Image()->getSourceTextureArea()), true);
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxImage initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXIMAGE_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(getSize);
	SX_ADD_METHOD(getWidth);
	SX_ADD_METHOD(getHeight);
	SX_ADD_METHOD(getOffsets);
	SX_ADD_METHOD(getOffsetX);
	SX_ADD_METHOD(getOffsetY);
	SX_ADD_METHOD(getName);
	SX_ADD_METHOD(getImagesetName);
	SX_ADD_METHOD(getSourceTextureArea);
}

} // namespace RFSX

#undef SX_IMPL_TYPE // And that's it in a nutshell
