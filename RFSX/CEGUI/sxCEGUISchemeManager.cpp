/************************************************************************************//**
 * @file sxCEGUISchemeManager.cpp
 * @brief Implementation for RFSX TEMPLATE object.
 * @author Daniel Queteschiner
 * @date December 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUISchemeManager.h"
#include "sxCEGUIScheme.h"
#include "sxCEGUIIterators.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxSchemeManager

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxSchemeManager implementation
//////////////////////////////////////////////////////////////////////////////////////////

SX_IMPL_TYPE::SX_IMPL_TYPE()
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
	return new sxSchemeIterator();
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxSchemeManager method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(loadScheme)
{
	skRValue::RType rtypes[] = { skRValue::T_String, skRValue::T_String };

	if(!CheckParams(1, 2, args, rtypes))
		return false;

	CEGUI::String filename(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	CEGUI::String resourcegroup;

	if(args.entries() == 2)
		resourcegroup = reinterpret_cast<const CEGUI::utf8*>(args[1].str().c_str());

	try
	{
		CEGUI::Scheme *scheme = CEGUI::SchemeManager::getSingletonPtr()->loadScheme(filename, resourcegroup);
		if(!scheme)
			return false;
		r_val.assignObject(new sxScheme(scheme), true);
	}
	catch(CEGUI::InvalidRequestException CEGUIDeadException(&e))
	{
		return false;
	}

	return true;
}


SX_METHOD_IMPL(unloadScheme)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	CEGUI::SchemeManager::getSingletonPtr()->unloadScheme(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	return true;
}


SX_METHOD_IMPL(isSchemePresent)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	r_val = CEGUI::SchemeManager::getSingletonPtr()->isSchemePresent(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	return true;
}


SX_METHOD_IMPL(getScheme)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	CEGUI::String name(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));

	try
	{
		CEGUI::Scheme *scheme = CEGUI::SchemeManager::getSingletonPtr()->getScheme(name);
		if(!scheme)
			return false;
		r_val.assignObject(new sxScheme(scheme), true);
	}
	catch(CEGUI::UnknownObjectException CEGUIDeadException(&e))
	{
		return false;
	}

	return true;
}


SX_METHOD_IMPL(unloadAllSchemes)
{
	CEGUI::SchemeManager::getSingletonPtr()->unloadAllSchemes();
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxSchemeManager initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXSCHEMEMANAGER_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(loadScheme);
	SX_ADD_METHOD(unloadScheme);
	SX_ADD_METHOD(isSchemePresent);
	SX_ADD_METHOD(getScheme);
	SX_ADD_METHOD(unloadAllSchemes);
}

} // namespace RFSX

#undef SX_IMPL_TYPE // And that's it in a nutshell
