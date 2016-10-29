/************************************************************************************//**
 * @file sxCEGUITexture.h
 * @brief Class declaration for RFSX Texture object
 * @author Daniel Queteschiner
 * @date November 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

// change these to your
#ifndef _SX_TEXTURE_H_
#define _SX_TEXTURE_H_

// include extra headers here
#include "RFSX.h"

// sxTexture class definition
/*
	Description:
		Wrapper for CEGUI::Texture class
		Texture objects are created via the Renderer.

	Methods:
		int getWidth();
		int getHeight();
		void loadFromFile(skString filename, skString resourcegroup="");
		sxRenderer getRenderer();
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxTexture

#define SXTEXTURE_METHODS 4

// Type-check and upcast macro
#define IS_TEXTURE(o)	CheckType<sxTexture>(o, RFSXU_TEXTURE)

namespace RFSX
{

class SX_IMPL_TYPE: public skExecutable
{
public:
	SX_IMPL_TYPE(CEGUI::Texture* texture);
	virtual ~SX_IMPL_TYPE();
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);
	virtual int executableType() const { return RFSXU_TEXTURE; }

	CEGUI::Texture* Texture() { return m_Texture; }

protected:
	SX_METHOD_DECL(getWidth);
	SX_METHOD_DECL(getHeight);
	SX_METHOD_DECL(loadFromFile);
	SX_METHOD_DECL(getRenderer);

private:
	CEGUI::Texture* m_Texture;

	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_TEXTURE_H_
