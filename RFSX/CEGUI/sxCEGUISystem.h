/************************************************************************************//**
 * @file sxCEGUISystem.h
 * @brief Class declaration for RFSX System object
 * @author Daniel Queteschiner
 * @date December 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_SYSTEM_H_
#define _SX_SYSTEM_H_

#include "RFSX.h"

// sxSystem class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		sxRenderer getRenderer();

		void setDefaultFont(skString name);
		void setDefaultFont(sxFont font);
		sxFont getDefaultFont();

		void signalRedraw();
		bool isRedrawRequested();
		void renderGUI();

		sxWindow setGUISheet(sxWindow sheet);
		sxWindow getGUISheet();

		void setSingleClickTimeout(float timeout);
		void setMultiClickTimeout(float timeout);
		void setMultiClickToleranceAreaSize(sxSize sz);
		float getSingleClickTimeout();
		float getMultiClickTimeout();
		sxSize getMultiClickToleranceAreaSize();

		void setDefaultMouseCursor(skString imageset_name, skString image_name);
		sxImage getDefaultMouseCursor();		-- may return null!

		sxWindow getWindowContainingMouse();	-- may return null!

		void executeScriptFile(skString filename);
		int executeScriptGlobal(skString global_name);
		void executeScriptString(skString simkin_string);

		float getMouseMoveScaling();
		void setMouseMoveScaling(float scaling);

		void notifyWindowDestroyed(sxWindow window);

		bool isSystemKeyDown(int k);

		void setDefaultTooltip(sxTooltip tooltip);
		void setDefaultTooltip(skString tooltipType);
		sxTooltip getDefaultTooltip();			-- may return null!

		bool injectMouseMove(float dx, float dy);
		bool injectMouseLeaves();
		bool injectMouseButtonDown(int btn);
		bool injectMouseButtonUp(int btn);
		bool injectKeyDown(int keycode);
		bool injectKeyUp(int keycode);
		bool injectChar(int code_point);
		bool injectMouseWheelChange(float delta);
		bool injectMousePosition(float x, float y);
		bool injectTimePulse(float timeElapsed);

		//////////////////////////////////////////////
		// EventSet
		void addEvent(skString name);
		void removeEvent(skString name);
		void removeAllEvents();
		bool isEventPresent(skString name);
		sxEventConnection subscribeEvent(skString name, skString callback_name);
		void fireEvent(skString name, sxEventArgs args, skString eventnamespace = "");
		bool isMuted();
		void setMutedState(bool setting);
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxSystem

#define SXSYSTEM_METHODS 44

namespace RFSX
{

class SX_IMPL_TYPE: public skExecutable
{
public:
	SX_IMPL_TYPE();
	virtual ~SX_IMPL_TYPE();
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);
	virtual skExecutableIterator* createIterator(const skString& qualifier) { return createIterator(); }
	virtual skExecutableIterator* createIterator();
	virtual int executableType() const { return RFSXU_SYSTEM; }

	virtual CEGUI::EventSet* EventSet() { return CEGUI::System::getSingletonPtr(); }

protected:
	SX_METHOD_DECL(getRenderer);

	SX_METHOD_DECL(setDefaultFont);
	SX_METHOD_DECL(getDefaultFont);

	SX_METHOD_DECL(signalRedraw);
	SX_METHOD_DECL(isRedrawRequested);
	SX_METHOD_DECL(renderGUI);

	SX_METHOD_DECL(setGUISheet);
	SX_METHOD_DECL(getGUISheet);

	SX_METHOD_DECL(setSingleClickTimeout);
	SX_METHOD_DECL(setMultiClickTimeout);
	SX_METHOD_DECL(setMultiClickToleranceAreaSize);
	SX_METHOD_DECL(getSingleClickTimeout);
	SX_METHOD_DECL(getMultiClickTimeout);
	SX_METHOD_DECL(getMultiClickToleranceAreaSize);

	SX_METHOD_DECL(setDefaultMouseCursor);
	SX_METHOD_DECL(getDefaultMouseCursor);

	SX_METHOD_DECL(getWindowContainingMouse);

	SX_METHOD_DECL(executeScriptFile);
	SX_METHOD_DECL(executeScriptGlobal);
	SX_METHOD_DECL(executeScriptString);

	SX_METHOD_DECL(getMouseMoveScaling);
	SX_METHOD_DECL(setMouseMoveScaling);

	SX_METHOD_DECL(notifyWindowDestroyed);

	SX_METHOD_DECL(isSystemKeyDown);

	SX_METHOD_DECL(setDefaultTooltip);
	SX_METHOD_DECL(getDefaultTooltip);

	SX_METHOD_DECL(injectMouseMove);
	SX_METHOD_DECL(injectMouseLeaves);
	SX_METHOD_DECL(injectMouseButtonDown);
	SX_METHOD_DECL(injectMouseButtonUp);
	SX_METHOD_DECL(injectKeyDown);
	SX_METHOD_DECL(injectKeyUp);
	SX_METHOD_DECL(injectChar);
	SX_METHOD_DECL(injectMouseWheelChange);
	SX_METHOD_DECL(injectMousePosition);
	SX_METHOD_DECL(injectTimePulse);

	// EventSet
	SX_METHOD_DECL(addEvent);
	SX_METHOD_DECL(removeEvent);
	SX_METHOD_DECL(removeAllEvents);
	SX_METHOD_DECL(isEventPresent);
	SX_METHOD_DECL(subscribeEvent);
	SX_METHOD_DECL(fireEvent);
	SX_METHOD_DECL(isMuted);
	SX_METHOD_DECL(setMutedState);

private:
	static sxMHT<SX_IMPL_TYPE> h_method;

};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_SYSTEM_H_
