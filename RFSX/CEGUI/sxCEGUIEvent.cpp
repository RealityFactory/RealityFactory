/************************************************************************************//**
 * @file sxCEGUIEvent.cpp
 * @brief Implementation for RFSX Event object.
 * @author Daniel Queteschiner
 * @date November 2009
 *//*
 * Copyright (c) 2009 ; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUIEvent.h"
#include "sxCEGUIEventArgs.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxEvent

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxEvent implementation
//////////////////////////////////////////////////////////////////////////////////////////

SX_IMPL_TYPE::SX_IMPL_TYPE(CEGUI::Event* e)
	: m_Event(e)
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
// sxEvent method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(getName)
{
	r_val = skString(caller->Event()->getName().c_str());
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxEvent initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXEVENT_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(getName);
}

} // namespace RFSX

#undef SX_IMPL_TYPE // And that's it in a nutshell
