/************************************************************************************//**
 * @file sxFile.cpp
 * @brief Implementation for RFSX File function library
 * @author D.R. Gunter
 *//*
 * Copyright (c) 2008 D.R. Gunter; All rights reserved.
 ****************************************************************************************/

#include "sxFile.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxFile

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxFile implementation
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
// sxFile method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(Load)
{
	skRValue::RType rtypes[] = { skRValue::T_String };

	if(!CheckParams(1, 1, args, rtypes))
		return false;

	skTreeNodeObject* obj = 0;
	if(obj = new skTreeNodeObject(args[0].str(), skTreeNode::read(args[0].str(), ctxt), true))
	{
		r_val.assignObject(obj, true);
		return true;
	}

	return false;
}


SX_METHOD_IMPL(Save)
{
	skRValue::RType rtypes[] = { skRValue::T_String, skRValue::T_Object };

	if(!CheckParams(2, 2, args, rtypes))
		return false;

	skiExecutable* obj = args[1].obj();
	if(obj->executableType() == TREENODE_TYPE)
	{
		((skTreeNodeObject*)obj)->getNode()->write(args[0].str());
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// sxFile initialization
//////////////////////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXFILE_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	// hash the methods
	SX_ADD_METHOD(Load);
	SX_ADD_METHOD(Save);
}

} // namespace RFSX

#undef SX_IMPL_TYPE

