/************************************************************************************//**
 * @file sxNew.cpp
 * @brief Implementation for RFSX object generator.
 * @author D.R. Gunter
 *//*
 * Copyright (c) 2008 D.R. Gunter; All rights reserved.
 ****************************************************************************************/

#include "sxNew.h"
#include "sxVector.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxNew

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxNew implementation
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
// sxNew method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(Vector)
{
	skRValue::RType rtypes[] = {	skRValue::T_Float,
									skRValue::T_Float,
									skRValue::T_Float };

	if(!CheckParams(3, 3, args, rtypes))
		return false;

	r_val.assignObject(new sxVector(args[0].floatValue(), args[1].floatValue(), args[2].floatValue()), true);
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxNew initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXNEW_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(Vector);
}

} // namespace RFSX

#undef SX_IMPL_TYPE
