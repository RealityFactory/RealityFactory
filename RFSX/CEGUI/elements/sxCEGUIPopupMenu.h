/************************************************************************************//**
 * @file sxCEGUIPopupMenu.h
 * @brief Class declaration for RFSX PopupMenu object
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_POPUPMENU_H_
#define _SX_POPUPMENU_H_

#include "RFSX.h"
#include "sxCEGUIMenuBase.h"

// sxPopupMenu class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		float getFadeInTime();
		float getFadeOutTime();

		void setFadeInTime(float fadetime);
		void setFadeOutTime(float fadetime);
		void openPopupMenu();
		void closePopupMenu();
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxPopupMenu

#define SXPOPUPMENU_METHODS 6

namespace RFSX
{

class SX_IMPL_TYPE: public sxMenuBase
{
public:
	SX_IMPL_TYPE(CEGUI::PopupMenu* popupMenu) : sxMenuBase(popupMenu) {}
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);

	CEGUI::PopupMenu* PopupMenu()	{ return static_cast<CEGUI::PopupMenu*>(MenuBase()); }

protected:
	SX_METHOD_DECL(getFadeInTime);
	SX_METHOD_DECL(getFadeOutTime);
	SX_METHOD_DECL(setFadeInTime);
	SX_METHOD_DECL(setFadeOutTime);
	SX_METHOD_DECL(openPopupMenu);
	SX_METHOD_DECL(closePopupMenu);

private:
	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_POPUPMENU_H_
