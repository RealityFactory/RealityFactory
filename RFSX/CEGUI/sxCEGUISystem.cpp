/************************************************************************************//**
 * @file sxCEGUISystem.cpp
 * @brief Implementation for RFSX System object.
 * @author Daniel Queteschiner
 * @date December 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUISystem.h"
#include "sxCEGUIEventSet.h"
#include "sxCEGUIRenderer.h"
#include "sxCEGUIFont.h"
#include "sxCEGUIWindow.h"
#include "sxCEGUISize.h"
#include "sxCEGUIImage.h"
#include "sxCEGUIEventArgs.h"
#include "sxCEGUIEventConnection.h"
#include "sxCEGUIIterators.h"
#include "elements/sxCEGUITooltip.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxSystem

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxSystem implementation
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


skExecutableIterator* SX_IMPL_TYPE::createIterator()
{
	return new sxEventIterator(this->EventSet());
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxSystem method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(getRenderer)
{
	r_val.assignObject(new sxRenderer(CEGUI::System::getSingletonPtr()->getRenderer()), true);
	return true;
}


SX_METHOD_IMPL(setDefaultFont)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() == skRValue::T_String)
	{
		CEGUI::System::getSingletonPtr()->setDefaultFont(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
		return true;
	}

	sxFont *font = IS_FONT(args[0]);
	if(!font)
		return false;

	CEGUI::System::getSingletonPtr()->setDefaultFont(font->Font());
	return true;
}


SX_METHOD_IMPL(getDefaultFont)
{
	r_val.assignObject(new sxFont(CEGUI::System::getSingletonPtr()->getDefaultFont()), true);
	return true;
}


SX_METHOD_IMPL(signalRedraw)
{
	CEGUI::System::getSingletonPtr()->signalRedraw();
	return true;
}


SX_METHOD_IMPL(isRedrawRequested)
{
	r_val = CEGUI::System::getSingletonPtr()->isRedrawRequested();
	return true;
}


SX_METHOD_IMPL(renderGUI)
{
	CEGUI::System::getSingletonPtr()->renderGUI();
	return true;
}


SX_METHOD_IMPL(setGUISheet)
{
	if(args.entries() != 1)
		return false;

	sxWindow *win = IS_WINDOW(args[0]);
	if(!win)
		return false;

	r_val.assignObject(new sxWindow(CEGUI::System::getSingletonPtr()->setGUISheet(win->Window())), true);
	return true;
}


SX_METHOD_IMPL(getGUISheet)
{
	r_val.assignObject(new sxWindow(CEGUI::System::getSingletonPtr()->getGUISheet()), true);
	return true;
}


SX_METHOD_IMPL(setSingleClickTimeout)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Float)
		return false;

	CEGUI::System::getSingletonPtr()->setSingleClickTimeout(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(setMultiClickTimeout)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Float)
		return false;

	CEGUI::System::getSingletonPtr()->setMultiClickTimeout(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(setMultiClickToleranceAreaSize)
{
	if(args.entries() != 1)
		return false;

	sxSize *sz = IS_SIZE(args[0]);
	if(!sz)
		return false;

	CEGUI::System::getSingletonPtr()->setMultiClickToleranceAreaSize(sz->SizeConstRef());
	return true;
}


SX_METHOD_IMPL(getSingleClickTimeout)
{
	r_val = static_cast<float>(CEGUI::System::getSingletonPtr()->getSingleClickTimeout());
	return true;
}


SX_METHOD_IMPL(getMultiClickTimeout)
{
	r_val = static_cast<float>(CEGUI::System::getSingletonPtr()->getMultiClickTimeout());
	return true;
}


SX_METHOD_IMPL(getMultiClickToleranceAreaSize)
{
	r_val.assignObject(new sxSize(CEGUI::System::getSingletonPtr()->getMultiClickToleranceAreaSize()), true);
	return true;
}


SX_METHOD_IMPL(setDefaultMouseCursor)
{
	skRValue::RType rtypes[] = { skRValue::T_String, skRValue::T_String };

	if(!CheckParams(2, 2, args, rtypes))
		return false;

	CEGUI::System::getSingletonPtr()->setDefaultMouseCursor(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()),
															reinterpret_cast<const CEGUI::utf8*>(args[1].str().c_str()));
	return true;
}


SX_METHOD_IMPL(getDefaultMouseCursor)
{
	const CEGUI::Image* image = CEGUI::System::getSingletonPtr()->getDefaultMouseCursor();

	if(image)
		r_val.assignObject(new sxImage(image), true);
	else
		r_val.assignObject(&ctxt.getInterpreter()->getNull());

	return true;
}


SX_METHOD_IMPL(getWindowContainingMouse)
{
	CEGUI::Window* win = CEGUI::System::getSingletonPtr()->getWindowContainingMouse();

	if(win)
		r_val.assignObject(new sxWindow(win), true);
	else
		r_val.assignObject(&ctxt.getInterpreter()->getNull());

	return true;
}


SX_METHOD_IMPL(executeScriptFile)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	CEGUI::System::getSingletonPtr()->executeScriptFile(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	return true;
}


SX_METHOD_IMPL(executeScriptGlobal)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	r_val = CEGUI::System::getSingletonPtr()->executeScriptGlobal(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	return true;
}


SX_METHOD_IMPL(executeScriptString)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	CEGUI::System::getSingletonPtr()->executeScriptString(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	return true;
}


SX_METHOD_IMPL(getMouseMoveScaling)
{
	r_val = CEGUI::System::getSingletonPtr()->getMouseMoveScaling();
	return true;
}


SX_METHOD_IMPL(setMouseMoveScaling)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	CEGUI::System::getSingletonPtr()->setMouseMoveScaling(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(notifyWindowDestroyed)
{
	if(args.entries() != 1)
		return false;

	sxWindow *win = IS_WINDOW(args[0]);
	if(!win)
		return false;

	CEGUI::System::getSingletonPtr()->notifyWindowDestroyed(win->Window());
	return true;
}


SX_METHOD_IMPL(isSystemKeyDown)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	CEGUI::SystemKey k = static_cast<CEGUI::SystemKey>(args[0].intValue());

	r_val = (k & CEGUI::System::getSingletonPtr()->getSystemKeys()) != 0;
	return true;
}


SX_METHOD_IMPL(setDefaultTooltip)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() == skRValue::T_String)
	{
		//void setDefaultTooltip(string tooltipType);
		CEGUI::System::getSingletonPtr()->setDefaultTooltip(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
		return true;
	}
	if(args[0].type() == skRValue::T_Object)
	{
		CEGUI::Tooltip *tooltip = NULL;

		if(args[0].obj() != &ctxt.getInterpreter()->getNull())
		{
			sxTooltip *tip = static_cast<sxTooltip*>(IS_WINDOW(args[0]));

			if(!tip)
				return false;

			tooltip = tip->Tooltip();
		}

		//void setDefaultTooltip(Tooltip* tooltip);
		CEGUI::System::getSingletonPtr()->setDefaultTooltip(tooltip);
		return true;
	}

	return false;
}


SX_METHOD_IMPL(getDefaultTooltip)
{
	//Tooltip* getDefaultTooltip(void) const;
	CEGUI::Tooltip *tooltip = CEGUI::System::getSingletonPtr()->getDefaultTooltip();

	if(tooltip)
		r_val.assignObject(new sxTooltip(tooltip), true);
	else
		r_val.assignObject(&ctxt.getInterpreter()->getNull());

	return true;
}


SX_METHOD_IMPL(injectMouseMove)
{
	skRValue::RType rtypes[] = { skRValue::T_Float, skRValue::T_Float };

	if(!CheckParams(2, 2, args, rtypes))
		return false;

	r_val = CEGUI::System::getSingletonPtr()->injectMouseMove(args[0].floatValue(), args[1].floatValue());
	return true;
}


SX_METHOD_IMPL(injectMouseLeaves)
{
	r_val = CEGUI::System::getSingletonPtr()->injectMouseLeaves();
	return true;
}


SX_METHOD_IMPL(injectMouseButtonDown)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	CEGUI::MouseButton btn = static_cast<CEGUI::MouseButton>(args[0].intValue());
	r_val = CEGUI::System::getSingletonPtr()->injectMouseButtonDown(btn);
	return true;
}


SX_METHOD_IMPL(injectMouseButtonUp)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	CEGUI::MouseButton btn = static_cast<CEGUI::MouseButton>(args[0].intValue());
	r_val = CEGUI::System::getSingletonPtr()->injectMouseButtonUp(btn);
	return true;
}


SX_METHOD_IMPL(injectKeyDown)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	r_val = CEGUI::System::getSingletonPtr()->injectKeyDown(args[0].intValue());
	return true;
}


SX_METHOD_IMPL(injectKeyUp)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	r_val = CEGUI::System::getSingletonPtr()->injectKeyUp(args[0].intValue());
	return true;
}


SX_METHOD_IMPL(injectChar)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	r_val = CEGUI::System::getSingletonPtr()->injectChar(args[0].intValue());
	return true;
}


SX_METHOD_IMPL(injectMouseWheelChange)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Float)
		return false;

	r_val = CEGUI::System::getSingletonPtr()->injectMouseWheelChange(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(injectMousePosition)
{
	skRValue::RType rtypes[] = { skRValue::T_String, skRValue::T_String };

	if(!CheckParams(2, 2, args, rtypes))
		return false;

	r_val = CEGUI::System::getSingletonPtr()->injectMousePosition(args[0].floatValue(), args[1].floatValue());
	return true;
}


SX_METHOD_IMPL(injectTimePulse)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Float)
		return false;

	r_val = CEGUI::System::getSingletonPtr()->injectTimePulse(args[0].floatValue());
	return true;
}


// EventSet
SX_METHOD_IMPL(addEvent)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	caller->EventSet()->addEvent(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	return true;
}


SX_METHOD_IMPL(removeEvent)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	caller->EventSet()->removeEvent(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	return true;
}


SX_METHOD_IMPL(removeAllEvents)
{
	caller->EventSet()->removeAllEvents();
	return true;
}


SX_METHOD_IMPL(isEventPresent)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	r_val = caller->EventSet()->isEventPresent(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	return true;
}


SX_METHOD_IMPL(subscribeEvent)
{
	skRValue::RType rtypes[] = { skRValue::T_String, skRValue::T_String };

	if(!CheckParams(2, 2, args, rtypes))
		return false;

	CEGUI::String name(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	CEGUI::String callback_name(reinterpret_cast<const CEGUI::utf8*>(args[1].str().c_str()));

	r_val.assignObject(new sxEventConnection(caller->EventSet()->subscribeScriptedEvent(name, callback_name)), true);
	return true;
}


SX_METHOD_IMPL(fireEvent)
{
	skRValue::RType rtypes[] = {	skRValue::T_String,
									skRValue::T_Object,
									skRValue::T_String };

	if(!CheckParams(2, 3, args, rtypes))
		return false;

	CEGUI::String name(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	sxEventArgs *eargs = (sxEventArgs*)(args[1].obj());
	CEGUI::String eventnamespace;

	if(args.entries() == 3)
		eventnamespace = reinterpret_cast<const CEGUI::utf8*>(args[2].str().c_str());

	caller->EventSet()->fireEvent(name, eargs->EventArgsRef(), eventnamespace);
	return true;
}


SX_METHOD_IMPL(isMuted)
{
	r_val = caller->EventSet()->isMuted();
	return true;
}


SX_METHOD_IMPL(setMutedState)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	caller->EventSet()->setMutedState(args[0].boolValue());
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxSystem initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXSYSTEM_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(getRenderer);

	SX_ADD_METHOD(setDefaultFont);
	SX_ADD_METHOD(getDefaultFont);

	SX_ADD_METHOD(signalRedraw);
	SX_ADD_METHOD(isRedrawRequested);
	SX_ADD_METHOD(renderGUI);

	SX_ADD_METHOD(setGUISheet);
	SX_ADD_METHOD(getGUISheet);

	SX_ADD_METHOD(setSingleClickTimeout);
	SX_ADD_METHOD(setMultiClickTimeout);
	SX_ADD_METHOD(setMultiClickToleranceAreaSize);
	SX_ADD_METHOD(getSingleClickTimeout);
	SX_ADD_METHOD(getMultiClickTimeout);
	SX_ADD_METHOD(getMultiClickToleranceAreaSize);

	SX_ADD_METHOD(setDefaultMouseCursor);
	SX_ADD_METHOD(getDefaultMouseCursor);

	SX_ADD_METHOD(getWindowContainingMouse);

	SX_ADD_METHOD(executeScriptFile);
	SX_ADD_METHOD(executeScriptGlobal);
	SX_ADD_METHOD(executeScriptString);

	SX_ADD_METHOD(getMouseMoveScaling);
	SX_ADD_METHOD(setMouseMoveScaling);

	SX_ADD_METHOD(notifyWindowDestroyed);

	SX_ADD_METHOD(isSystemKeyDown);

	SX_ADD_METHOD(setDefaultTooltip);
	SX_ADD_METHOD(getDefaultTooltip);

	SX_ADD_METHOD(injectMouseMove);
	SX_ADD_METHOD(injectMouseLeaves);
	SX_ADD_METHOD(injectMouseButtonDown);
	SX_ADD_METHOD(injectMouseButtonUp);
	SX_ADD_METHOD(injectKeyDown);
	SX_ADD_METHOD(injectKeyUp);
	SX_ADD_METHOD(injectChar);
	SX_ADD_METHOD(injectMouseWheelChange);
	SX_ADD_METHOD(injectMousePosition);
	SX_ADD_METHOD(injectTimePulse);

	// EventSet
	SX_ADD_METHOD(addEvent);
	SX_ADD_METHOD(removeEvent);
	SX_ADD_METHOD(removeAllEvents);
	SX_ADD_METHOD(isEventPresent);
	SX_ADD_METHOD(subscribeEvent);
	SX_ADD_METHOD(fireEvent);
	SX_ADD_METHOD(isMuted);
	SX_ADD_METHOD(setMutedState);
}

} // namespace RFSX

#undef SX_IMPL_TYPE // And that's it in a nutshell
