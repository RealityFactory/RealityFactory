/************************************************************************************//**
 * @file sxCEGUICoordConverter.h
 * @brief Class declaration for RFSX CoordConverter object
 * @author Daniel Queteschiner
 * @date December 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_COORDCONVERTER_H_
#define _SX_COORDCONVERTER_H_

#include "RFSX.h"

// sxCoordConverter class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		float windowToScreenX(const Window& window, const UDim& x);
		float windowToScreenX(const Window& window, const float x);

		float windowToScreenY(const Window& window, const UDim& y);
		float windowToScreenY(const Window& window, const float y);

		Vector2 windowToScreen(const Window& window, const UVector2& vec);
		Vector2 windowToScreen(const Window& window, const Vector2& vec);

		Rect windowToScreen(const Window& window, const URect& rect);
		Rect windowToScreen(const Window& window, const Rect& rect);

		float screenToWindowX(const Window& window, const UDim& x);
		float screenToWindowX(const Window& window, const float x);

		float screenToWindowY(const Window& window, const UDim& y);
		float screenToWindowY(const Window& window, const float y);

		Vector2 screenToWindow(const Window& window, const UVector2& vec);
		Vector2 screenToWindow(const Window& window, const Vector2& vec);

		Rect screenToWindow(const Window& window, const URect& rect);
		Rect screenToWindow(const Window& window, const Rect& rect);
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxCoordConverter

#define SXCOORDCONVERTER_METHODS 6

namespace RFSX
{

class SX_IMPL_TYPE: public skExecutable
{
public:
	SX_IMPL_TYPE();
	virtual ~SX_IMPL_TYPE();
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);
	virtual int executableType() const { return RFSXU_COORDCONVERTER; }

protected:
	SX_METHOD_DECL(windowToScreenX);
	SX_METHOD_DECL(windowToScreenY);
	SX_METHOD_DECL(windowToScreen);
	SX_METHOD_DECL(screenToWindowX);
	SX_METHOD_DECL(screenToWindowY);
	SX_METHOD_DECL(screenToWindow);

private:
	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_COORDCONVERTER_H_
