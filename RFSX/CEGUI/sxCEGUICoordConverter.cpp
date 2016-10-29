/************************************************************************************//**
 * @file sxCEGUICoordConverter.cpp
 * @brief Implementation for RFSX CoordConverter object.
 * @author Daniel Queteschiner
 * @date December 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUICoordConverter.h"
#include "sxCEGUIWindow.h"
#include "sxCEGUIUVector2.h"
#include "sxCEGUIVector2.h"
#include "sxCEGUIURect.h"
#include "sxCEGUIRect.h"
#include "sxCEGUIUDim.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxCoordConverter

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxCoordConverter implementation
//////////////////////////////////////////////////////////////////////////////////////////

SX_IMPL_TYPE::SX_IMPL_TYPE()
{
}


SX_IMPL_TYPE::~SX_IMPL_TYPE()
{
}


bool SX_IMPL_TYPE::method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
{
	if(h_method(this, m_name, args, r_val, ctxt))
		return true;
	else
		return skExecutable::method(m_name, args, r_val, ctxt);
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxCoordConverter method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(windowToScreenX)
{
	if(args.entries() != 2)
		return false;

	sxWindow *win = IS_WINDOW(args[0]);
	if(!win)
		return false;

	if(args[1].type() == skRValue::T_Float)
	{
		//float windowToScreenX(const Window& window, const float x);
		r_val = CEGUI::CoordConverter::windowToScreenX(*(win->Window()), args[1].floatValue());
		return true;
	}

	sxUDim *udim = IS_UDIM(args[1]);
	if(!udim)
		return false;

	//float windowToScreenX(const Window& window, const UDim& x);
	r_val = CEGUI::CoordConverter::windowToScreenX(*(win->Window()), udim->UDimConstRef());
	return true;
}


SX_METHOD_IMPL(windowToScreenY)
{
	if(args.entries() != 2)
		return false;

	sxWindow *win = IS_WINDOW(args[0]);
	if(!win)
		return false;

	if(args[1].type() == skRValue::T_Float)
	{
		//float windowToScreenY(const Window& window, const float y);
		r_val = CEGUI::CoordConverter::windowToScreenY(*(win->Window()), args[1].floatValue());
		return true;
	}

	sxUDim *udim = IS_UDIM(args[1]);
	if(!udim)
		return false;

	//float windowToScreenY(const Window& window, const UDim& y);
	r_val = CEGUI::CoordConverter::windowToScreenY(*(win->Window()), udim->UDimConstRef());
	return true;
}


SX_METHOD_IMPL(windowToScreen)
{
	if(args.entries() != 2)
		return false;

	sxWindow *win = IS_WINDOW(args[0]);
	if(!win)
		return false;

	//Vector2 windowToScreen(const Window& window, const UVector2& vec);
	{
		sxUVector2 *uvec = IS_UVECTOR2(args[1]);
		if(uvec)
		{
			r_val.assignObject(new sxVector2(CEGUI::CoordConverter::windowToScreen(*(win->Window()), uvec->UVector2ConstRef())), true);
			return true;
		}
	}

	//Vector2 windowToScreen(const Window& window, const Vector2& vec);
	{
		sxVector2 *vec = IS_VECTOR2(args[1]);
		if(vec)
		{
			r_val.assignObject(new sxVector2(CEGUI::CoordConverter::windowToScreen(*(win->Window()), vec->Vector2ConstRef())), true);
			return true;
		}
	}

	//Rect windowToScreen(const Window& window, const URect& rect);
	{
		sxURect *urect = IS_URECT(args[1]);
		if(urect)
		{
			r_val.assignObject(new sxRect(CEGUI::CoordConverter::windowToScreen(*(win->Window()), urect->URectConstRef())), true);
			return true;
		}
	}

	//Rect windowToScreen(const Window& window, const Rect& rect);
	{
		sxRect *rect = IS_RECT(args[1]);
		if(rect)
		{
			r_val.assignObject(new sxRect(CEGUI::CoordConverter::windowToScreen(*(win->Window()), rect->RectConstRef())), true);
			return true;
		}
	}
	return false;
}


SX_METHOD_IMPL(screenToWindowX)
{
	if(args.entries() != 2)
		return false;

	sxWindow *win = IS_WINDOW(args[0]);
	if(!win)
		return false;

	if(args[1].type() == skRValue::T_Float)
	{
		//float screenToWindowX(const Window& window, const float x);
		r_val = CEGUI::CoordConverter::screenToWindowX(*(win->Window()), args[1].floatValue());
		return true;
	}

	sxUDim *udim = IS_UDIM(args[1]);
	if(!udim)
		return false;

	//float screenToWindowX(const Window& window, const UDim& x);
	r_val = CEGUI::CoordConverter::screenToWindowX(*(win->Window()), udim->UDimConstRef());
	return true;
}


SX_METHOD_IMPL(screenToWindowY)
{
	if(args.entries() != 2)
		return false;

	sxWindow *win = IS_WINDOW(args[0]);
	if(!win)
		return false;

	if(args[1].type() == skRValue::T_Float)
	{
		//float screenToWindowY(const Window& window, const float y);
		r_val = CEGUI::CoordConverter::screenToWindowY(*(win->Window()), args[1].floatValue());
		return true;
	}

	sxUDim *udim = IS_UDIM(args[1]);
	if(!udim)
		return false;

	//float screenToWindowY(const Window& window, const UDim& y);
	r_val = CEGUI::CoordConverter::screenToWindowY(*(win->Window()), udim->UDimConstRef());
	return true;
}


SX_METHOD_IMPL(screenToWindow)
{
	if(args.entries() != 2)
		return false;

	sxWindow *win = IS_WINDOW(args[0]);
	if(!win)
		return false;

	//Vector2 screenToWindow(const Window& window, const UVector2& vec);
	{
		sxUVector2 *uvec = IS_UVECTOR2(args[1]);
		if(uvec)
		{
			r_val.assignObject(new sxVector2(CEGUI::CoordConverter::screenToWindow(*(win->Window()), uvec->UVector2ConstRef())), true);
			return true;
		}
	}

	//Vector2 screenToWindow(const Window& window, const Vector2& vec);
	{
		sxVector2 *vec = IS_VECTOR2(args[1]);
		if(vec)
		{
			r_val.assignObject(new sxVector2(CEGUI::CoordConverter::screenToWindow(*(win->Window()), vec->Vector2ConstRef())), true);
			return true;
		}
	}

	//Rect screenToWindow(const Window& window, const URect& rect);
	{
		sxURect *urect = IS_URECT(args[1]);
		if(urect)
		{
			r_val.assignObject(new sxRect(CEGUI::CoordConverter::screenToWindow(*(win->Window()), urect->URectConstRef())), true);
			return true;
		}
	}

	//Rect screenToWindow(const Window& window, const Rect& rect);
	{
		sxRect *rect = IS_RECT(args[1]);
		if(rect)
		{
			r_val.assignObject(new sxRect(CEGUI::CoordConverter::screenToWindow(*(win->Window()), rect->RectConstRef())), true);
			return true;
		}
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxCoordConverter initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXCOORDCONVERTER_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(windowToScreenX);
	SX_ADD_METHOD(windowToScreenY);
	SX_ADD_METHOD(windowToScreen);
	SX_ADD_METHOD(screenToWindowX);
	SX_ADD_METHOD(screenToWindowY);
	SX_ADD_METHOD(screenToWindow);
}

} // namespace RFSX

#undef SX_IMPL_TYPE // And that's it in a nutshell
