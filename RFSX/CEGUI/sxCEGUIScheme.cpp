/************************************************************************************//**
 * @file sxCEGUIScheme.cpp
 * @brief Implementation for RFSX Scheme object.
 * @author Daniel Queteschiner
 * @date December 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUIScheme.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxScheme

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxScheme implementation
//////////////////////////////////////////////////////////////////////////////////////////

SX_IMPL_TYPE::SX_IMPL_TYPE(CEGUI::Scheme *scheme)
	: m_Scheme(scheme)
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
// sxScheme method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(loadResources)
{
	caller->Scheme()->loadResources();
	return true;
}


SX_METHOD_IMPL(unloadResources)
{
	caller->Scheme()->unloadResources();
	return true;
}


SX_METHOD_IMPL(resourcesLoaded)
{
	r_val = caller->Scheme()->resourcesLoaded();
	return true;
}


SX_METHOD_IMPL(getName)
{
	r_val = skString(caller->Scheme()->getName().c_str());
	return true;
}


SX_METHOD_IMPL(setDefaultResourceGroup)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	caller->Scheme()->setDefaultResourceGroup(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	return true;
}


SX_METHOD_IMPL(getDefaultResourceGroup)
{
	r_val = skString(caller->Scheme()->getDefaultResourceGroup().c_str());
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxScheme initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXSCHEME_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(loadResources);
	SX_ADD_METHOD(unloadResources);
	SX_ADD_METHOD(resourcesLoaded);
	SX_ADD_METHOD(getName);
	SX_ADD_METHOD(setDefaultResourceGroup);
	SX_ADD_METHOD(getDefaultResourceGroup);
}

} // namespace RFSX

#undef SX_IMPL_TYPE
