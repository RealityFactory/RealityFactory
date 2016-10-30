/************************************************************************************//**
 * @file sxDialogState.h
 * @brief Implementation for RFSX DialogState object
 * @author Daniel Queteschiner
 * @date December 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "CDialogState.h"
#include "sxDialogState.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxDialogState

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxDialogState implementation
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


//////////////////////////////////////////////////////////////////////////////////////////
// sxDialogState method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(UpdateLevel)
{
	if(args.entries() != 1)
		return false;

	CDialogState::GetSingletonPtr()->UpdateLevel(args[0].boolValue());
	return true;
}

SX_METHOD_IMPL(RenderLevel)
{
	if(args.entries() != 1)
		return false;

	CDialogState::GetSingletonPtr()->RenderLevel(args[0].boolValue());

	return true;
}

SX_METHOD_IMPL(Enter)
{
	CDialogState::GetSingletonPtr()->PushState(CDialogState::GetSingletonPtr());

	return true;
}

SX_METHOD_IMPL(Exit)
{
	CDialogState::GetSingletonPtr()->PopState();

	return true;
}


//////////////////////////////////////////////////////////////////////////////////////////
// sxDialogState initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXDIALOGSTATE_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(UpdateLevel);
	SX_ADD_METHOD(RenderLevel);
	SX_ADD_METHOD(Enter);
	SX_ADD_METHOD(Exit);
}

} // namespace RFSX

#undef SX_IMPL_TYPE
