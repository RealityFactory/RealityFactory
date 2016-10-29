/************************************************************************************//**
 * @file sxCEGUIGUISheet.h
 * @brief Class declaration for RFSX GUISheet object
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_GUISHEET_H_
#define _SX_GUISHEET_H_

#include "RFSX.h"
#include "../sxCEGUIWindow.h"

// sxGUISheet class definition
/*
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxGUISheet

namespace RFSX
{

class SX_IMPL_TYPE: public sxWindow
{
public:
	SX_IMPL_TYPE(CEGUI::GUISheet* sheet) : sxWindow(sheet) {}

	CEGUI::GUISheet* GUISheet()			{ return static_cast<CEGUI::GUISheet*>(Window()); }
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_GUISHEET_H_
