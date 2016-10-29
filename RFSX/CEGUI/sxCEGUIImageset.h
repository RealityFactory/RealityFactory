/************************************************************************************//**
 * @file sxCEGUIImageset.h
 * @brief Class declaration for RFSX Imageset object
 * @author Daniel Queteschiner
 * @date December 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_IMAGESET_H_
#define _SX_IMAGESET_H_

#include "RFSX.h"

// sxImageset class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		sxTexture getTexture();
		skString getName();
		int getImageCount();
		bool isImageDefined(skString name);
		sxImage getImage(skString name);
		void undefineImage(skString name);
		void undefineAllImages();
		sxSize getImageSize(skString name);
		float getImageWidth(skString name);
		float getImageHeight(skString name);
		sxVector2 getImageOffset(skString name);
		float getImageOffsetX(skString name);
		float getImageOffsetY(skstring name);
		void defineImage(skString name, sxVector2 pos, sxSize size, sxVector2 render_offset);
		void defineImage(skString name, sxRect rect, sxVector2 render_offset);
		bool isAutoScaled();
		void setAutoScalingEnabled(bool setting);
		sxSize getNativeResolution();
		void setNativeResolution(sxSize sz);
		void notifyScreenResolution(sxSize sz);
		void setDefaultResourceGroup(skString resourceGroup);
		skString getDefaultResourceGroup();

	for each: ImageIterator
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxImageset

#define SXIMAGESET_METHODS 21

// Type-check and upcast macro
#define IS_IMAGESET(o) CheckType<sxImageset>(o, RFSXU_IMAGESET);

namespace RFSX
{

class SX_IMPL_TYPE: public skExecutable
{
public:
	SX_IMPL_TYPE(CEGUI::Imageset* imageset);
	virtual ~SX_IMPL_TYPE();
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);
	virtual skExecutableIterator* createIterator(const skString& qualifier) { return createIterator(); }
	virtual skExecutableIterator* createIterator();
	virtual int executableType() const { return RFSXU_IMAGESET; }

	CEGUI::Imageset* Imageset()  { return m_Imageset; }

protected:
	SX_METHOD_DECL(getTexture);
	SX_METHOD_DECL(getName);
	SX_METHOD_DECL(getImageCount);
	SX_METHOD_DECL(isImageDefined);
	SX_METHOD_DECL(getImage);
	SX_METHOD_DECL(undefineImage);
	SX_METHOD_DECL(undefineAllImages);
	SX_METHOD_DECL(getImageSize);
	SX_METHOD_DECL(getImageWidth);
	SX_METHOD_DECL(getImageHeight);
	SX_METHOD_DECL(getImageOffset);
	SX_METHOD_DECL(getImageOffsetX);
	SX_METHOD_DECL(getImageOffsetY);
	SX_METHOD_DECL(defineImage);
	SX_METHOD_DECL(isAutoScaled);
	SX_METHOD_DECL(setAutoScalingEnabled);
	SX_METHOD_DECL(getNativeResolution);
	SX_METHOD_DECL(setNativeResolution);
	SX_METHOD_DECL(notifyScreenResolution);
	SX_METHOD_DECL(setDefaultResourceGroup);
	SX_METHOD_DECL(getDefaultResourceGroup);

private:
	SX_IMPL_TYPE() {}
	CEGUI::Imageset* m_Imageset;

	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_IMAGESET_H_
