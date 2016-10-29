/************************************************************************************//**
 * @file sxCEGUIItemEntry.h
 * @brief Class declaration for RFSX ItemEntry object
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_ITEMENTRY_H_
#define _SX_ITEMENTRY_H_

#include "RFSX.h"
#include "../sxCEGUIWindow.h"

// sxItemEntry class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		sxSize getItemPixelSize();
		sxItemListBase getOwnerList(); -- may return null!
		bool isSelected();
		bool isSelectable();
		void setSelected(bool setting);
		void select();
		void deselect();
		void setSelectable(bool setting);
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxItemEntry

#define SXITEMENTRY_METHODS 8

namespace RFSX
{

class SX_IMPL_TYPE: public sxWindow
{
public:
	SX_IMPL_TYPE(CEGUI::ItemEntry* entry) : sxWindow(entry) {}
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);

	CEGUI::ItemEntry* ItemEntry()	{ return static_cast<CEGUI::ItemEntry*>(Window()); }

protected:
	SX_METHOD_DECL(getItemPixelSize);
	SX_METHOD_DECL(getOwnerList);
	SX_METHOD_DECL(isSelected);
	SX_METHOD_DECL(isSelectable);
	SX_METHOD_DECL(setSelected);
	SX_METHOD_DECL(select);
	SX_METHOD_DECL(deselect);
	SX_METHOD_DECL(setSelectable);

private:
	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_ITEMENTRY_H_
