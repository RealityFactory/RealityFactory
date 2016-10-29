/************************************************************************************//**
 * @file sxCEGUIRenderer.h
 * @brief Class declaration for RFSX Renderer object
 * @author Daniel Queteschiner
 * @date December 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_RENDERER_H_
#define _SX_RENDERER_H_

#include "RFSX.h"

// sxRenderer class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		float getCurrentZ();
		float getZLayer(int layer);

		bool isQueueingEnabled();

		sxTexture createTexture();
		sxTexture createTexture(float size);
		sxTexture createTexture(skString filename, skString resourcegroup);

		void destroyTexture(sxTexture tex);
		void destroyAllTextures();

		float getWidth();
		float getHeight();
		sxSize getSize();
		sxRect getRect();

		int getMaxTextureSize();
		int getHorzScreenDPI();
		int getVertScreenDPI();
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxRenderer

namespace RFSX
{

class SX_IMPL_TYPE: public sxEventSet
{
public:
	SX_IMPL_TYPE(CEGUI::Renderer* renderer);
	virtual ~SX_IMPL_TYPE();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);
	virtual int executableType() const { return RFSXU_RENDERER; }

	CEGUI::Renderer* Renderer()			{ return m_Renderer; }
	virtual CEGUI::EventSet* EventSet() { return m_Renderer; }

private:
	CEGUI::Renderer* m_Renderer;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_RENDERER_H_
