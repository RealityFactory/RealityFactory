/************************************************************************************//**
 * @file sxCEGUIMenuItem.h
 * @brief Class declaration for RFSX MenuItem object
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_MENUITEM_H_
#define _SX_MENUITEM_H_

#include "RFSX.h"
#include "sxCEGUIItemEntry.h"

// sxMenuItem class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		bool isHovering() const;
		bool isPushed() const;
		bool isOpened() const;
		PopupMenu* getPopupMenu() const; -- may return null!

		void setPopupMenu(PopupMenu* popup); -- null for clearing
		void openPopupMenu(bool notify = true);
		void closePopupMenu(bool notify = true);
		bool togglePopupMenu();
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxMenuItem

#define SXMENUITEM_METHODS 8

namespace RFSX
{

class SX_IMPL_TYPE: public sxItemEntry
{
public:
	SX_IMPL_TYPE(CEGUI::MenuItem* item) : sxItemEntry(item) {}
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);

	CEGUI::MenuItem* MenuItem()	{ return static_cast<CEGUI::MenuItem*>(ItemEntry()); }

protected:
	SX_METHOD_DECL(isHovering);
	SX_METHOD_DECL(isPushed);
	SX_METHOD_DECL(isOpened);
	SX_METHOD_DECL(getPopupMenu);
	SX_METHOD_DECL(setPopupMenu);
	SX_METHOD_DECL(openPopupMenu);
	SX_METHOD_DECL(closePopupMenu);
	SX_METHOD_DECL(togglePopupMenu);

private:
	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_MENUITEM_H_
