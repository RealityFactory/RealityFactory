/************************************************************************************//**
 * @file sxCEGUIPushButton.h
 * @brief Class declaration for RFSX TEMPLATE object
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_PUSHBUTTON_H_
#define _SX_PUSHBUTTON_H_

#include "RFSX.h"
#include "sxCEGUIButtonBase.h"

// sxPushButton class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxPushButton

namespace RFSX
{

class SX_IMPL_TYPE: public sxButtonBase
{
public:
	SX_IMPL_TYPE(CEGUI::PushButton* button) : sxButtonBase(button) {}

	CEGUI::PushButton* PushButton() { return static_cast<CEGUI::PushButton*>(ButtonBase()); }
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_PUSHBUTTON_H_
