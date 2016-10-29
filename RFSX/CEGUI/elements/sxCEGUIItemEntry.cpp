/************************************************************************************//**
 * @file sxCEGUIItemEntry.cpp
 * @brief Implementation for RFSX ItemEntry object.
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUIItemEntry.h"
#include "sxCEGUIItemListBase.h"
#include "../sxCEGUISize.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxItemEntry

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxItemEntry implementation
//////////////////////////////////////////////////////////////////////////////////////////

bool SX_IMPL_TYPE::method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
{
	if(h_method(this, m_name, args, r_val, ctxt))
		return true;
	else
		return sxWindow::method(m_name, args, r_val, ctxt);
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxItemEntry method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(getItemPixelSize)
{
	//Size getItemPixelSize();
	r_val.assignObject(new sxSize(caller->ItemEntry()->getItemPixelSize()), true);
	return true;
}


SX_METHOD_IMPL(getOwnerList)
{
	//ItemListBase* getOwnerList() const;
	CEGUI::ItemListBase *listBase = caller->ItemEntry()->getOwnerList();

	if(listBase)
		r_val.assignObject(new sxItemListBase(listBase), true);
	else
		r_val.assignObject(&ctxt.getInterpreter()->getNull());

	return true;
}


SX_METHOD_IMPL(isSelected)
{
	//bool isSelected() const;
	r_val = caller->ItemEntry()->isSelected();
	return true;
}


SX_METHOD_IMPL(isSelectable)
{
	//bool isSelectable() const;
	r_val = caller->ItemEntry()->isSelectable();
	return true;
}


SX_METHOD_IMPL(setSelected)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	//void setSelected(bool setting);
	caller->ItemEntry()->setSelected(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(select)
{
	//void select();
	caller->ItemEntry()->select();
	return true;
}


SX_METHOD_IMPL(deselect)
{
	//void deselect();
	caller->ItemEntry()->deselect();
	return true;
}


SX_METHOD_IMPL(setSelectable)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	//void setSelectable(bool setting);
	caller->ItemEntry()->setSelectable(args[0].boolValue());
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxItemEntry initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXITEMENTRY_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(getItemPixelSize);
	SX_ADD_METHOD(getOwnerList);
	SX_ADD_METHOD(isSelected);
	SX_ADD_METHOD(isSelectable);
	SX_ADD_METHOD(setSelected);
	SX_ADD_METHOD(select);
	SX_ADD_METHOD(deselect);
	SX_ADD_METHOD(setSelectable);
}

} // namespace RFSX

#undef SX_IMPL_TYPE
