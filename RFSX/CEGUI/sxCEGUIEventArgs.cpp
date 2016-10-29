/***************************************************************************************
 * @file sxCEGUIEventArgs.cpp
 * @brief
 * @author Daniel Queteschiner
 * @date November 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUIEventArgs.h"
#include "sxCEGUIImage.h"
#include "sxCEGUIMouseCursor.h"
#include "sxCEGUIWindow.h"
#include "sxCEGUIVector2.h"
#include "elements/sxCEGUIDragContainer.h"
#include "elements/sxCEGUITreeItem.h"

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxEventArgs implementation
//////////////////////////////////////////////////////////////////////////////////////////

bool sxEventArgs::setValue(const skString& name, const skString& att, const skRValue& val)
{
	if(name == "handled")
	{
		EventArgs()->handled = val.boolValue();
		return true;
	}
	if(name == "hasWindow")
	{
		EventArgs()->d_hasWindow = val.boolValue();
		return true;
	}

	return skExecutable::setValue(name, att, val);
}

bool sxEventArgs::getValue(const skString& name, const skString& att, skRValue& val)
{
	if(name == "handled")
	{
		val = EventArgs()->handled;
		return true;
	}
	if(name == "hasWindow")
	{
		val = EventArgs()->d_hasWindow;
		return true;
	}

	return skExecutable::getValue(name, att, val);
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxMouseCursorEventArgs implementation
//////////////////////////////////////////////////////////////////////////////////////////

bool sxMouseCursorEventArgs::method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
{
	if(m_name == "image")
	{
		if(MouseCursorEventArgs()->image)
			r_val.assignObject(new sxImage(MouseCursorEventArgs()->image), true);
		else
			r_val.assignObject(&ctxt.getInterpreter()->getNull());

		return true;
	}

	return sxEventArgs::method(m_name, args, r_val, ctxt);
}

bool sxMouseCursorEventArgs::getValue(const skString& name, const skString& att, skRValue& val)
{
	if(name == "mouseCursor")
	{
		val.assignObject(new sxMouseCursor(), true);
		return true;
	}

	return sxEventArgs::getValue(name, att, val);
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxWindowEventArgs implementation
//////////////////////////////////////////////////////////////////////////////////////////

bool sxWindowEventArgs::setValue(const skString& name, const skString& att, const skRValue& val)
{
	if(name == "window")
	{
		skRValue value = val;
		sxWindow *win = IS_WINDOW(value);
		if(!win)
			return false;

		WindowEventArgs()->window = win->Window();
		return true;
	}

	return sxEventArgs::setValue(name, att, val);
}

bool sxWindowEventArgs::getValue(const skString& name, const skString& att, skRValue& val)
{
	if(name == "window")
	{
		val.assignObject(new sxWindow(WindowEventArgs()->window), true);
		return true;
	}

	return sxEventArgs::getValue(name, att, val);
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxUpdateEventArgs implementation
//////////////////////////////////////////////////////////////////////////////////////////

bool sxUpdateEventArgs::setValue(const skString& name, const skString& att, const skRValue& val)
{
	if(name == "timeSinceLastFrame")
	{
		UpdateEventArgs()->d_timeSinceLastFrame = val.floatValue();
		return true;
	}

	return sxWindowEventArgs::setValue(name, att, val);
}

bool sxUpdateEventArgs::getValue(const skString& name, const skString& att, skRValue& val)
{
	if(name == "timeSinceLastFrame")
	{
		val = UpdateEventArgs()->d_timeSinceLastFrame;
		return true;
	}

	return sxWindowEventArgs::getValue(name, att, val);
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxActivationEventArgs implementation
//////////////////////////////////////////////////////////////////////////////////////////

bool sxActivationEventArgs::setValue(const skString& name, const skString& att, const skRValue& val)
{
	if(name == "otherWindow")
	{
		skRValue value = val;
		sxWindow *win = IS_WINDOW(value);
		if(!win)
			return false;

		ActivationEventArgs()->otherWindow = win->Window();
		return true;
	}

	return sxWindowEventArgs::setValue(name, att, val);
}

bool sxActivationEventArgs::getValue(const skString& name, const skString& att, skRValue& val)
{
	if(name == "otherWindow")
	{
		val.assignObject(new sxWindow(ActivationEventArgs()->otherWindow), true);
		return true;
	}

	return sxWindowEventArgs::getValue(name, att, val);
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxHeaderSequenceEventArgs implementation
//////////////////////////////////////////////////////////////////////////////////////////

bool sxHeaderSequenceEventArgs::setValue(const skString& name, const skString& att, const skRValue& val)
{
	if(name == "oldIdx")
	{
		HeaderSequenceEventArgs()->d_oldIdx = val.intValue();
		return true;
	}
	if(name == "newIdx")
	{
		HeaderSequenceEventArgs()->d_newIdx = val.intValue();
		return true;
	}

	return sxWindowEventArgs::setValue(name, att, val);
}

bool sxHeaderSequenceEventArgs::getValue(const skString& name, const skString& att, skRValue& val)
{
	if(name == "oldIdx")
	{
		val = static_cast<int>(HeaderSequenceEventArgs()->d_oldIdx);
		return true;
	}
	if(name == "newIdx")
	{
		val = static_cast<int>(HeaderSequenceEventArgs()->d_newIdx);
		return true;
	}

	return sxWindowEventArgs::getValue(name, att, val);
}


//////////////////////////////////////////////////////////////////////////////////////////
// sxMouseEventArgs implementation
//////////////////////////////////////////////////////////////////////////////////////////

bool sxMouseEventArgs::setValue(const skString& name, const skString& att, const skRValue& val)
{
	if(name == "position")
	{
		skRValue value = val;
		sxVector2 *vec = IS_VECTOR2(value);
		if(!vec)
			return false;

		MouseEventArgs()->position = vec->Vector2ConstRef();
		return true;
	}
	if(name == "moveDelta")
	{
		skRValue value = val;
		sxVector2 *vec = IS_VECTOR2(value);
		if(!vec)
			return false;

		MouseEventArgs()->moveDelta = vec->Vector2ConstRef();
		return true;
	}
	if(name == "button")
	{
		MouseEventArgs()->button = static_cast<CEGUI::MouseButton>(val.intValue());
		return true;
	}
	if(name == "sysKeys")
	{
		MouseEventArgs()->sysKeys = static_cast<unsigned int>(val.intValue());
		return true;
	}
	if(name == "wheelChange")
	{
		MouseEventArgs()->wheelChange = val.floatValue();
		return true;
	}

	return sxWindowEventArgs::setValue(name, att, val);
}

bool sxMouseEventArgs::getValue(const skString& name, const skString& att, skRValue& val)
{
	if(name == "position")
	{
		val.assignObject(new sxVector2(MouseEventArgs()->position), true);
		return true;
	}
	if(name == "moveDelta")
	{
		val.assignObject(new sxVector2(MouseEventArgs()->moveDelta), true);
		return true;
	}
	if(name == "button")
	{
		val = static_cast<int>(MouseEventArgs()->button);
		return true;
	}
	if(name == "sysKeys")
	{
		val = static_cast<int>(MouseEventArgs()->sysKeys);
		return true;
	}
	if(name == "wheelChange")
	{
		val = MouseEventArgs()->wheelChange;
		return true;
	}

	return sxWindowEventArgs::getValue(name, att, val);
}


//////////////////////////////////////////////////////////////////////////////////////////
// sxKeyEventArgs implementation
//////////////////////////////////////////////////////////////////////////////////////////

bool sxKeyEventArgs::setValue(const skString& name, const skString& att, const skRValue& val)
{
	if(name == "codepoint")
	{
		KeyEventArgs()->codepoint = static_cast<unsigned long>(val.intValue());
		return true;
	}
	if(name == "scancode")
	{
		KeyEventArgs()->scancode = static_cast<CEGUI::Key::Scan>(val.intValue());
		return true;
	}
	if(name == "sysKeys")
	{
		KeyEventArgs()->sysKeys = static_cast<unsigned int>(val.intValue());
		return true;
	}

	return sxWindowEventArgs::setValue(name, att, val);
}

bool sxKeyEventArgs::getValue(const skString& name, const skString& att, skRValue& val)
{
	if(name == "codepoint")
	{
		val = static_cast<int>(KeyEventArgs()->codepoint);
		return true;
	}
	if(name == "scancode")
	{
		val = static_cast<int>(KeyEventArgs()->scancode);
		return true;
	}
	if(name == "sysKeys")
	{
		val = static_cast<int>(KeyEventArgs()->sysKeys);
		return true;
	}

	return sxWindowEventArgs::getValue(name, att, val);
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxDragDropEventArgs implementation
//////////////////////////////////////////////////////////////////////////////////////////

bool sxDragDropEventArgs::setValue(const skString& name, const skString& att, const skRValue& val)
{
	if(name == "dragDropItem")
	{
		skRValue value = val;
		sxDragContainer *drag = static_cast<sxDragContainer*>(IS_WINDOW(value));
		if(!drag)
			return false;

		DragDropEventArgs()->dragDropItem = drag->DragContainer();
		return true;
	}

	return sxWindowEventArgs::setValue(name, att, val);
}

bool sxDragDropEventArgs::getValue(const skString& name, const skString& att, skRValue& val)
{
	if(name == "dragDropItem")
	{
		val.assignObject(new sxDragContainer(DragDropEventArgs()->dragDropItem), true);
		return true;
	}

	return sxWindowEventArgs::getValue(name, att, val);
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxTreeEventArgs implementation
//////////////////////////////////////////////////////////////////////////////////////////

bool sxTreeEventArgs::setValue(const skString& name, const skString& att, const skRValue& val)
{
	if(name == "treeItem")
	{
		skRValue value = val;
		sxTreeItem *item = IS_TREEITEM(value);
		if(!item)
			return false;

		TreeEventArgs()->treeItem = item->TreeItem();
		return true;
	}

	return sxWindowEventArgs::setValue(name, att, val);
}

bool sxTreeEventArgs::getValue(const skString& name, const skString& att, skRValue& val)
{
	if(name == "treeItem")
	{
		val.assignObject(new sxTreeItem(TreeEventArgs()->treeItem), true);
		return true;
	}

	return sxWindowEventArgs::getValue(name, att, val);
}

} // namespace RFSX

#undef SX_IMPL_TYPE
