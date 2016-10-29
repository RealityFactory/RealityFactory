/************************************************************************************//**
 * @file sxCEGUIEventConnection.cpp
 * @brief Implementation for RFSX EventConnection object.
 * @author Daniel Queteschiner
 * @date December 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUIEventConnection.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxEventConnection

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxEventConnection implementation
//////////////////////////////////////////////////////////////////////////////////////////

SX_IMPL_TYPE::SX_IMPL_TYPE(CEGUI::RefCounted<CEGUI::BoundSlot> slot)
	: m_Slot(slot)
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
// sxEventConnection method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(connected)
{
	r_val = caller->Slot()->connected();
	return true;
}


SX_METHOD_IMPL(disconnect)
{
	caller->Slot()->disconnect();
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxEventConnection initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXEVENTCONNECTION_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(connected);
	SX_ADD_METHOD(disconnect);
}

} // namespace RFSX

#undef SX_IMPL_TYPE
