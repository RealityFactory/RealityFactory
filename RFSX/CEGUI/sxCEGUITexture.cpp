/************************************************************************************//**
 * @file sxCEGUITexture.cpp
 * @brief Implementation for RFSX Texture object.
 * @author Daniel Queteschiner
 * @date November 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUITexture.h"
#include "sxCEGUIEventSet.h"
#include "sxCEGUIRenderer.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxTexture

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxTexture implementation
//////////////////////////////////////////////////////////////////////////////////////////

SX_IMPL_TYPE::SX_IMPL_TYPE(CEGUI::Texture* texture)
	: m_Texture(texture)
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

//////////////////////////////////////////////////////////////////////////////////////////
// sxTexture method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(getWidth)
{
	r_val = caller->Texture()->getWidth();
	return true;
}


SX_METHOD_IMPL(getHeight)
{
	r_val = caller->Texture()->getHeight();
	return true;
}


SX_METHOD_IMPL(loadFromFile)
{
	skRValue::RType rtypes[] = { skRValue::T_String, skRValue::T_String };

	if(!CheckParams(1, 2, args, rtypes))
		return false;

	CEGUI::String filename(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	CEGUI::String resourcegroup;

	if(args.entries() == 2)
		resourcegroup = reinterpret_cast<const CEGUI::utf8*>(args[1].str().c_str());

	caller->Texture()->loadFromFile(filename, resourcegroup);

	return true;
}


SX_METHOD_IMPL(getRenderer)
{
	r_val.assignObject(new sxRenderer(caller->Texture()->getRenderer()), true);
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxTexture initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXTEXTURE_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	// Add all methods here
	SX_ADD_METHOD(getWidth);
	SX_ADD_METHOD(getHeight);
	SX_ADD_METHOD(loadFromFile);
	SX_ADD_METHOD(getRenderer);
}

} // namespace RFSX

#undef SX_IMPL_TYPE // And that's it in a nutshell
