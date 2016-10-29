/************************************************************************************//**
 * @file sxCEGUIScrolledItemListBase.cpp
 * @brief Implementation for RFSX ScrolledItemListBase object.
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUIScrolledItemListBase.h"
#include "sxCEGUIScrollbar.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxScrolledItemListBase

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxScrolledItemListBase implementation
//////////////////////////////////////////////////////////////////////////////////////////

bool SX_IMPL_TYPE::method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
{
	if(h_method(this, m_name, args, r_val, ctxt))
		return true;
	else
		return sxItemListBase::method(m_name, args, r_val, ctxt);
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxScrolledItemListBase method functions
//////////////////////////////////////////////////////////////////////////////////////////
/*
SX_METHOD_IMPL(name_of_method)
{
	skRValue::RType rtypes[] = {	skRValue::T_Float,
									skRValue::T_Float,
									skRValue::T_Float }; // parameter type list

	//if(!CheckParams(3, 3, args, rtypes)) // uncomment for rigid type checking
		//return false;

	//r_val.assignObject(new sxVector(args[0].floatValue(), args[1].floatValue(), args[2].floatValue()), true);
	return true;
}*/
SX_METHOD_IMPL(isVertScrollbarAlwaysShown)
{
	//bool isVertScrollbarAlwaysShown() const;
	r_val = caller->ScrolledItemListBase()->isVertScrollbarAlwaysShown();
	return true;
}


SX_METHOD_IMPL(isHorzScrollbarAlwaysShown)
{
	//bool isHorzScrollbarAlwaysShown() const;
	r_val = caller->ScrolledItemListBase()->isHorzScrollbarAlwaysShown();
	return true;
}


SX_METHOD_IMPL(getVertScrollbar)
{
	//Scrollbar* getVertScrollbar() const;
	r_val.assignObject(new sxScrollbar(caller->ScrolledItemListBase()->getVertScrollbar()), true);
	return true;
}


SX_METHOD_IMPL(getHorzScrollbar)
{
	//Scrollbar* getHorzScrollbar() const;
	r_val.assignObject(new sxScrollbar(caller->ScrolledItemListBase()->getHorzScrollbar()), true);
	return true;
}


SX_METHOD_IMPL(setShowVertScrollbar)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	//void setShowVertScrollbar(bool mode);
	caller->ScrolledItemListBase()->setShowVertScrollbar(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(setShowHorzScrollbar)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	//void setShowHorzScrollbar(bool mode);
	caller->ScrolledItemListBase()->setShowHorzScrollbar(args[0].boolValue());
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxScrolledItemListBase initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXSCROLLEDITEMLISTBASE_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(isVertScrollbarAlwaysShown);
	SX_ADD_METHOD(isHorzScrollbarAlwaysShown);
	SX_ADD_METHOD(getVertScrollbar);
	SX_ADD_METHOD(getHorzScrollbar);
	SX_ADD_METHOD(setShowVertScrollbar);
	SX_ADD_METHOD(setShowHorzScrollbar);
}

} // namespace RFSX

#undef SX_IMPL_TYPE
