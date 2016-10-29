/************************************************************************************//**
 * @file sxCEGUIProperty.cpp
 * @brief Implementation for RFSX Property object.
 * @author Daniel Queteschiner
 * @date December 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUIProperty.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxProperty

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxProperty implementation
//////////////////////////////////////////////////////////////////////////////////////////

SX_IMPL_TYPE::SX_IMPL_TYPE(CEGUI::Property* prop)
	: m_Property(prop)
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
// sxProperty method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(getHelp)
{
	r_val = skString(caller->Property()->getHelp().c_str());
	return true;
}


SX_METHOD_IMPL(getName)
{
	r_val = skString(caller->Property()->getName().c_str());
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxProperty initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXPROPERTY_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
    SX_ADD_METHOD(getHelp);
    SX_ADD_METHOD(getName);
}

} // namespace RFSX

#undef SX_IMPL_TYPE // And that's it in a nutshell
