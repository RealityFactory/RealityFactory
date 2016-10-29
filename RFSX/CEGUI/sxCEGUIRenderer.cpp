/************************************************************************************//**
 * @file sxCEGUIRenderer.cpp
 * @brief Implementation for RFSX TEMPLATE object.
 * @author Daniel Queteschiner
 * @date December 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUIEventSet.h"
#include "sxCEGUIRenderer.h"
#include "sxCEGUITexture.h"
#include "sxCEGUISize.h"
#include "sxCEGUIRect.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxRenderer

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxRenderer implementation
//////////////////////////////////////////////////////////////////////////////////////////

SX_IMPL_TYPE::SX_IMPL_TYPE(CEGUI::Renderer* renderer)
	: m_Renderer(renderer)
{
}


SX_IMPL_TYPE::~SX_IMPL_TYPE()
{
}


bool SX_IMPL_TYPE::method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
{
	if(m_name == "getCurrentZ")
	{
		r_val = m_Renderer->getCurrentZ();
		return true;
	}
	if(m_name == "getZLayer")
	{
		if(args.entries() != 1)
			return false;

		if(args[0].type() != skRValue::T_Int)
			return false;

		r_val = m_Renderer->getZLayer(args[0].intValue());
		return true;
	}
	if(m_name == "isQueueingEnabled")
	{
		r_val = m_Renderer->isQueueingEnabled();
		return true;
	}
	if(m_name == "createTexture")
	{
		if(args.entries() > 2)
			return false;

		if(args.entries() == 0)
		{
			r_val.assignObject(new sxTexture(m_Renderer->createTexture()), true);
		}
		else if(args.entries() == 1)
		{
			if(args[0].type() != skRValue::T_Float)
				return false;

			r_val.assignObject(new sxTexture(m_Renderer->createTexture(args[0].floatValue())), true);
		}
		else if(args.entries() == 2)
		{
			if(args[0].type() != skRValue::T_String || args[1].type() != skRValue::T_String)
				return false;

			CEGUI::String filename		(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
			CEGUI::String resourcegroup	(reinterpret_cast<const CEGUI::utf8*>(args[1].str().c_str()));

			r_val.assignObject(new sxTexture(m_Renderer->createTexture(filename, resourcegroup)), true);
		}
		return true;
	}
	if(m_name == "destroyTexture")
	{
		if(args.entries() != 1)
			return false;

		sxTexture *tex = IS_TEXTURE(args[0]);
		if(!tex)
			return false;

		m_Renderer->destroyTexture(tex->Texture());
		return true;
	}
	if(m_name == "destroyAllTextures")
	{
		m_Renderer->destroyAllTextures();
		return true;
	}
	if(m_name == "getWidth")
	{
		r_val = m_Renderer->getWidth();
		return true;
	}
	if(m_name == "getHeight")
	{
		r_val = m_Renderer->getHeight();
		return true;
	}
	if(m_name == "getSize")
	{
		r_val.assignObject(new sxSize(m_Renderer->getSize()), true);
		return true;
	}
	if(m_name == "getRect")
	{
		r_val.assignObject(new sxRect(m_Renderer->getRect()), true);
		return true;
	}
	if(m_name == "getMaxTextureSize")
	{
		r_val = static_cast<int>(m_Renderer->getMaxTextureSize());
		return true;
	}
	if(m_name == "getHorzScreenDPI")
	{
		r_val = static_cast<int>(m_Renderer->getHorzScreenDPI());
		return true;
	}
	if(m_name == "getVertScreenDPI")
	{
		r_val = static_cast<int>(m_Renderer->getVertScreenDPI());
		return true;
	}

	return sxEventSet::method(m_name, args, r_val, ctxt);
}


} // namespace RFSX

#undef SX_IMPL_TYPE
