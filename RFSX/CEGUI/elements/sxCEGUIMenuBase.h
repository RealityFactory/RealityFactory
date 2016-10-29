/************************************************************************************//**
 * @file sxCEGUIMenuBase.h
 * @brief Class declaration for RFSX MenuBase object
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_MENUBASE_H_
#define _SX_MENUBASE_H_

#include "RFSX.h"
#include "sxCEGUIItemListBase.h"

// sxMenuBase class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		float getItemSpacing();
		bool isMultiplePopupsAllowed();
		sxMenuItem getPopupMenuItem(); -- may return null!

		void setItemSpacing(float spacing);
		void changePopupMenuItem(sxMenuItem item = null);
		void setAllowMultiplePopups(bool setting);
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxMenuBase

#define SXMENUBASE_METHODS 6

namespace RFSX
{

class SX_IMPL_TYPE: public sxItemListBase
{
public:
	SX_IMPL_TYPE(CEGUI::MenuBase* menuBase) : sxItemListBase(menuBase) {}
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);

	CEGUI::MenuBase* MenuBase()	{ return static_cast<CEGUI::MenuBase*>(ItemListBase()); }
	CEGUI::Menubar* Menubar()	{ return static_cast<CEGUI::Menubar*>(ItemListBase()); }

protected:
	SX_METHOD_DECL(getItemSpacing);
	SX_METHOD_DECL(isMultiplePopupsAllowed);
	SX_METHOD_DECL(getPopupMenuItem);
	SX_METHOD_DECL(setItemSpacing);
	SX_METHOD_DECL(changePopupMenuItem);
	SX_METHOD_DECL(setAllowMultiplePopups);

private:
	static sxMHT<SX_IMPL_TYPE> h_method;
};

typedef sxMenuBase	sxMenubar;

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_MENUBASE_H_
