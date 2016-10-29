/************************************************************************************//**
 * @file sxCEGUIImageset.cpp
 * @brief Implementation for RFSX Imageset object.
 * @author Daniel Queteschiner
 * @date December 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUIImageset.h"
#include "sxCEGUIImage.h"
#include "sxCEGUITexture.h"
#include "sxCEGUIVector2.h"
#include "sxCEGUISize.h"
#include "sxCEGUIRect.h"
#include "sxCEGUIIterators.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxImageset

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxImageset implementation
//////////////////////////////////////////////////////////////////////////////////////////

SX_IMPL_TYPE::SX_IMPL_TYPE(CEGUI::Imageset* imageset)
	: m_Imageset(imageset)
{
}


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


skExecutableIterator* SX_IMPL_TYPE::createIterator()
{
	return new sxImageIterator(this->Imageset());
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxImageset method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(getTexture)
{
	r_val.assignObject(new sxTexture(caller->Imageset()->getTexture()), true);
	return true;
}


SX_METHOD_IMPL(getName)
{
	r_val = skString(caller->Imageset()->getName().c_str());
	return true;
}


SX_METHOD_IMPL(getImageCount)
{
	r_val = static_cast<int>(caller->Imageset()->getImageCount());
	return true;
}


SX_METHOD_IMPL(isImageDefined)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	r_val = caller->Imageset()->isImageDefined(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	return true;
}


SX_METHOD_IMPL(getImage)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	CEGUI::String name(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	r_val.assignObject(new sxImage(&caller->Imageset()->getImage(name)), true);
	return true;
}


SX_METHOD_IMPL(undefineImage)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	caller->Imageset()->undefineImage(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	return true;
}


SX_METHOD_IMPL(undefineAllImages)
{
	caller->Imageset()->undefineAllImages();
	return true;
}


SX_METHOD_IMPL(getImageSize)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	r_val.assignObject(new sxSize(caller->Imageset()->getImageSize(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()))), true);
	return true;
}


SX_METHOD_IMPL(getImageWidth)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	r_val = caller->Imageset()->getImageWidth(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	return true;
}


SX_METHOD_IMPL(getImageHeight)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	r_val = caller->Imageset()->getImageHeight(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	return true;
}


SX_METHOD_IMPL(getImageOffset)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	r_val.assignObject(new sxVector2(caller->Imageset()->getImageOffset(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()))), true);
	return true;
}


SX_METHOD_IMPL(getImageOffsetX)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	r_val = caller->Imageset()->getImageOffsetX(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	return true;
}


SX_METHOD_IMPL(getImageOffsetY)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	r_val = caller->Imageset()->getImageOffsetY(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	return true;
}


SX_METHOD_IMPL(defineImage)
{
	skRValue::RType rtypes[] = {	skRValue::T_String, skRValue::T_Object,
									skRValue::T_Object, skRValue::T_Object};

	if(!CheckParams(3, 4, args, rtypes))
		return false;

	CEGUI::String name(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));

	if(args.entries() == 3)
	{
		sxRect *rect = IS_RECT(args[1]);
		if(!rect)
			return false;

		sxVector2 *offset = IS_VECTOR2(args[2]);
		if(!offset)
			return false;

		caller->Imageset()->defineImage(name, rect->RectConstRef(), offset->Vector2ConstRef());
	}
	else
	{
		sxVector2 *pos = IS_VECTOR2(args[1]);
		if(!pos)
			return false;

		sxSize *size = IS_SIZE(args[2]);
		if(!size)
			return false;

		sxVector2 *offset = IS_VECTOR2(args[3]);
		if(!offset)
			return false;

		caller->Imageset()->defineImage(name, pos->Vector2ConstRef(), size->SizeConstRef(), offset->Vector2ConstRef());
	}

	return true;
}


SX_METHOD_IMPL(isAutoScaled)
{
	r_val = caller->Imageset()->isAutoScaled();
	return true;
}


SX_METHOD_IMPL(setAutoScalingEnabled)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	caller->Imageset()->setAutoScalingEnabled(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(getNativeResolution)
{
	r_val.assignObject(new sxSize(caller->Imageset()->getNativeResolution()), true);
	return true;
}


SX_METHOD_IMPL(setNativeResolution)
{
	if(args.entries() != 1)
		return false;

	sxSize *size = IS_SIZE(args[0]);
	if(!size)
		return false;

	caller->Imageset()->setNativeResolution(size->SizeConstRef());
	return true;
}


SX_METHOD_IMPL(notifyScreenResolution)
{
	if(args.entries() != 1)
		return false;

	sxSize *size = IS_SIZE(args[0]);
	if(!size)
		return false;

	caller->Imageset()->notifyScreenResolution(size->SizeConstRef());
	return true;
}


SX_METHOD_IMPL(setDefaultResourceGroup)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	CEGUI::Imageset::setDefaultResourceGroup(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	return true;
}


SX_METHOD_IMPL(getDefaultResourceGroup)
{
	r_val = skString(CEGUI::Imageset::getDefaultResourceGroup().c_str());
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxImageset initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXIMAGESET_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(getTexture);
	SX_ADD_METHOD(getName);
	SX_ADD_METHOD(getImageCount);
	SX_ADD_METHOD(isImageDefined);
	SX_ADD_METHOD(getImage);
	SX_ADD_METHOD(undefineImage);
	SX_ADD_METHOD(undefineAllImages);
	SX_ADD_METHOD(getImageSize);
	SX_ADD_METHOD(getImageWidth);
	SX_ADD_METHOD(getImageHeight);
	SX_ADD_METHOD(getImageOffset);
	SX_ADD_METHOD(getImageOffsetX);
	SX_ADD_METHOD(getImageOffsetY);
	SX_ADD_METHOD(defineImage);
	SX_ADD_METHOD(isAutoScaled);
	SX_ADD_METHOD(setAutoScalingEnabled);
	SX_ADD_METHOD(getNativeResolution);
	SX_ADD_METHOD(setNativeResolution);
	SX_ADD_METHOD(notifyScreenResolution);
	SX_ADD_METHOD(setDefaultResourceGroup);
	SX_ADD_METHOD(getDefaultResourceGroup);
}

} // namespace RFSX

#undef SX_IMPL_TYPE
