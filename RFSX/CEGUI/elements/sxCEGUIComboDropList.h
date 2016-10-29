/************************************************************************************//**
 * @file sxCEGUIComboDropList.h
 * @brief Class declaration for RFSX ComboDropList object
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_COMBODROPLIST_H_
#define _SX_COMBODROPLIST_H_

#include "RFSX.h"
#include "sxCEGUIListbox.h"

// sxComboDropList class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		void	setArmed(bool setting);
		bool	isArmed(void) const;
		void	setAutoArmEnabled(bool setting);
		bool	isAutoArmEnabled(void) const;
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxComboDropList

#define SXCOMBODROPLIST_METHODS 4

namespace RFSX
{

class SX_IMPL_TYPE: public sxListbox
{
public:
	SX_IMPL_TYPE(CEGUI::ComboDropList* list) : sxListbox(list) {}
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);

	CEGUI::ComboDropList* ComboDropList() { return static_cast<CEGUI::ComboDropList*>(Listbox()); }

protected:
	SX_METHOD_DECL(setArmed);
	SX_METHOD_DECL(isArmed);
	SX_METHOD_DECL(setAutoArmEnabled);
	SX_METHOD_DECL(isAutoArmEnabled);

private:
	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_COMBODROPLIST_H_
