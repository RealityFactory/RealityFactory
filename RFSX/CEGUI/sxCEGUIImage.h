/************************************************************************************//**
 * @file sxCEGUIImage.h
 * @brief Class declaration for RFSX Image object
 * @author Daniel Queteschiner
 * @date December 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_IMAGE_H_
#define _SX_IMAGE_H_

#include "RFSX.h"

// sxImage class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		sxSize getSize();
		float getWidth();
		float getHeight();
		sxVector2 getOffsets();
		float getOffsetX();
		float getOffsetY();
		skString getName();
		skString getImagesetName();
		sxRect getSourceTextureArea();
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxImage

#define SXIMAGE_METHODS 9

// Type-check and upcast macro
#define IS_IMAGE(o) CheckType<sxImage>(o, RFSXU_IMAGE);

namespace RFSX
{

class SX_IMPL_TYPE: public skExecutable
{
public:
	SX_IMPL_TYPE(const CEGUI::Image* image) : m_Image(image) {}
	virtual ~SX_IMPL_TYPE();
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);
	virtual int executableType() const { return RFSXU_IMAGE; }

	const CEGUI::Image* Image()				{ return m_Image; }

protected:
	SX_METHOD_DECL(getSize);
	SX_METHOD_DECL(getWidth);
	SX_METHOD_DECL(getHeight);
	SX_METHOD_DECL(getOffsets);
	SX_METHOD_DECL(getOffsetX);
	SX_METHOD_DECL(getOffsetY);
	SX_METHOD_DECL(getName);
	SX_METHOD_DECL(getImagesetName);
	SX_METHOD_DECL(getSourceTextureArea);
	//void draw(...)

private:
	const CEGUI::Image*	m_Image;

	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_IMAGE_H_
