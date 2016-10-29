/************************************************************************************//**
 * @file sxCEGUIEventArgs.h
 * @brief Wrapper for CEGUI's base EventArgs class used with event signalling
 * @author Daniel Queteschiner
 * @date November 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_EVENTARGS_H_
#define _SX_EVENTARGS_H_

#include "RFSX.h"

// sxEventArgs class definition
/*
	Description:
		Wrapper for CEGUI's base EventArgs class used with event signalling

	Fields:
		bool handled(RW);
		bool hasWindow(RW);
*/


namespace RFSX
{

// sxEventArgs
/*
	Fields:
		bool handled(RW);
		bool hasWindow(RW);
*/

// Type-check and upcast macro
#define IS_EVENTARGS(o) CheckType<sxEventArgs>(o, RFSXU_EVENTARGS);

class sxEventArgs : public skExecutable
{
public:
	sxEventArgs(CEGUI::EventArgs* eventArgs) : m_EventArgs(eventArgs) {}
	virtual ~sxEventArgs() {}

	virtual bool setValue(const skString& name, const skString& att, const skRValue& val);
	virtual bool getValue(const skString& name, const skString& att, skRValue& val);
	virtual int executableType() const { return RFSXU_EVENTARGS; }

	CEGUI::EventArgs* EventArgs()						{ return m_EventArgs; }
	CEGUI::EventArgs& EventArgsRef()					{ return *m_EventArgs; }
	const CEGUI::EventArgs& EventArgsConstRef() const	{ return *m_EventArgs; }

private:
	CEGUI::EventArgs* m_EventArgs;
};


// sxMouseCursorEventArgs
/*
	Methods:
		sxImage image();
	Fields:
		bool handled;(RW)
		bool hasWindow;(RW)
		sxMouseCursor mouseCursor;(RO)
*/

class sxMouseCursorEventArgs : public sxEventArgs
{
public:
	sxMouseCursorEventArgs(CEGUI::MouseCursorEventArgs* eventArgs) : sxEventArgs(eventArgs) {}

	virtual bool getValue(const skString& name, const skString& att, skRValue& val);
	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);
	//virtual int executableType() const { return RFSXU_EVENTARGS; }

	CEGUI::MouseCursorEventArgs* MouseCursorEventArgs()
	{ return static_cast<CEGUI::MouseCursorEventArgs*>(EventArgs()); }
};


// sxWindowEventArgs
/*
	Fields:
		bool handled;(RW)
		bool hasWindow;(RW)
		sxWindow window;(RW)
*/

class sxWindowEventArgs : public sxEventArgs
{
public:
	sxWindowEventArgs(CEGUI::WindowEventArgs* eventArgs) : sxEventArgs(eventArgs) {}

	virtual bool setValue(const skString& name, const skString& att, const skRValue& val);
	virtual bool getValue(const skString& name, const skString& att, skRValue& val);
	//virtual int executableType() const { return RFSXU_EVENTARGS; }

	CEGUI::WindowEventArgs* WindowEventArgs()
	{ return static_cast<CEGUI::WindowEventArgs*>(EventArgs()); }
};


// sxUpdateEventArgs
/*
	Fields:
		bool handled;(RW)
		bool hasWindow;(RW)
		sxWindow window;(RW)
		float timeSinceLastFrame;(RW)
*/

class sxUpdateEventArgs : public sxWindowEventArgs
{
public:
	sxUpdateEventArgs(CEGUI::UpdateEventArgs* eventArgs) : sxWindowEventArgs(eventArgs) {}

	virtual bool setValue(const skString& name, const skString& att, const skRValue& val);
	virtual bool getValue(const skString& name, const skString& att, skRValue& val);
	//virtual int executableType() const { return RFSXU_EVENTARGS; }

	CEGUI::UpdateEventArgs* UpdateEventArgs()
	{ return static_cast<CEGUI::UpdateEventArgs*>(EventArgs()); }
};


// sxActivationEventArgs
/*
	Fields:
		bool handled;(RW)
		bool hasWindow;(RW)
		sxWindow window;(RW)
		sxWindow otherWindow;(RW)
*/

class sxActivationEventArgs : public sxWindowEventArgs
{
public:
	sxActivationEventArgs(CEGUI::ActivationEventArgs* eventArgs) : sxWindowEventArgs(eventArgs) {}

	virtual bool setValue(const skString& name, const skString& att, const skRValue& val);
	virtual bool getValue(const skString& name, const skString& att, skRValue& val);
	//virtual int executableType() const { return RFSXU_EVENTARGS; }

	virtual CEGUI::ActivationEventArgs* ActivationEventArgs()
	{ return static_cast<CEGUI::ActivationEventArgs*>(EventArgs()); }
};


// sxHeaderSequenceEventArgs
/*
	Fields:
		bool handled;(RW)
		bool hasWindow;(RW)
		sxWindow window;(RW)
		int oldIdx;(RW)
		int newIdx;(RW)
*/

class sxHeaderSequenceEventArgs : public sxWindowEventArgs
{
public:
	sxHeaderSequenceEventArgs(CEGUI::HeaderSequenceEventArgs* eventArgs) : sxWindowEventArgs(eventArgs) {}

	virtual bool setValue(const skString& name, const skString& att, const skRValue& val);
	virtual bool getValue(const skString& name, const skString& att, skRValue& val);
	//virtual int executableType() const { return RFSXU_EVENTARGS; }

	CEGUI::HeaderSequenceEventArgs* HeaderSequenceEventArgs()
	{ return static_cast<CEGUI::HeaderSequenceEventArgs*>(EventArgs()); }
};


// sxMouseEventArgs
/*
	Fields:
		bool handled;(RW)
		bool hasWindow;(RW)

		sxWindow window;(RW)

		sxVector2 position;
		sxVector2 moveDelta;
		int button;
		int sysKeys;
		float wheelChange;
*/

class sxMouseEventArgs : public sxWindowEventArgs
{
public:
	sxMouseEventArgs(CEGUI::MouseEventArgs* eventArgs) : sxWindowEventArgs(eventArgs) {}

	virtual bool setValue(const skString& name, const skString& att, const skRValue& val);
	virtual bool getValue(const skString& name, const skString& att, skRValue& val);
	//virtual int executableType() const { return RFSXU_EVENTARGS; }

	CEGUI::MouseEventArgs* MouseEventArgs()
	{ return static_cast<CEGUI::MouseEventArgs*>(EventArgs()); }
};


// sxKeyEventArgs
/*
	Fields:
		bool handled;(RW)
		bool hasWindow;(RW)

		sxWindow window;(RW)

		int codepoint;
		int scancode;
		int sysKeys;
*/

class sxKeyEventArgs : public sxWindowEventArgs
{
public:
	sxKeyEventArgs(CEGUI::KeyEventArgs* eventArgs) : sxWindowEventArgs(eventArgs) {}

	virtual bool setValue(const skString& name, const skString& att, const skRValue& val);
	virtual bool getValue(const skString& name, const skString& att, skRValue& val);
	//virtual int executableType() const { return RFSXU_EVENTARGS; }

	CEGUI::KeyEventArgs* KeyEventArgs()
	{ return static_cast<CEGUI::KeyEventArgs*>(EventArgs()); }
};


// sxDragDropEventArgs
/*
	Fields:
		bool handled;(RW)
		bool hasWindow;(RW)

		sxWindow window;(RW)
		sxDragContainer dragDropItem;
*/

class sxDragDropEventArgs : public sxWindowEventArgs
{
public:
	sxDragDropEventArgs(CEGUI::DragDropEventArgs* eventArgs) : sxWindowEventArgs(eventArgs) {}

	virtual bool setValue(const skString& name, const skString& att, const skRValue& val);
	virtual bool getValue(const skString& name, const skString& att, skRValue& val);
	//virtual int executableType() const { return RFSXU_EVENTARGS; }

	CEGUI::DragDropEventArgs* DragDropEventArgs()
	{ return static_cast<CEGUI::DragDropEventArgs*>(EventArgs()); }
};


// sxTreeEventArgs
/*
	Fields:
		bool handled;(RW)
		bool hasWindow;(RW)

		sxWindow window;(RW)
		TreeItem *treeItem;
*/

class sxTreeEventArgs : public sxWindowEventArgs
{
public:
	sxTreeEventArgs(CEGUI::TreeEventArgs* eventArgs) : sxWindowEventArgs(eventArgs) {}

	virtual bool setValue(const skString& name, const skString& att, const skRValue& val);
	virtual bool getValue(const skString& name, const skString& att, skRValue& val);
	//virtual int executableType() const { return RFSXU_EVENTARGS; }

	virtual CEGUI::TreeEventArgs* TreeEventArgs()
	{ return static_cast<CEGUI::TreeEventArgs*>(EventArgs()); }
};

} // namespace RFSX

#endif // _SX_EVENTARGS_H_
