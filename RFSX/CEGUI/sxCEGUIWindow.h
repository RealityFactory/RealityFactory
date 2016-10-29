/************************************************************************************//**
 * @file sxCEGUIWindow.h
 * @brief Class declaration for RFSX Window object
 * @author Daniel Queteschiner
 * @date December 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_WINDOW_H_
#define _SX_WINDOW_H_

#include "RFSX.h"

// sxWindow class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		skString getType();
		skString getName();

		bool isDestroyedByParent();
		bool isAlwaysOnTop();
		bool isDisabled(bool localOnly = false);
		bool isVisible(bool localOnly = false);
		bool isActive();
		bool isClippedByParent();

		void setDestroyedByParent(bool setting);
		void setAlwaysOnTop(bool setting);
		void setEnabled(bool setting);
		void enable();
		void disable();
		void setVisible(bool setting);
		void show();
		void hide();
		void activate();
		void deactivate();

		void setClippedByParent(bool setting);

		int getID();
		void setID(int ID);

		int getChildCount();

		bool isChild(int ID);
		bool isChild(skString name);
		bool isChild(sxWindow window);
		bool isChildRecursive(int ID);

		void addChildWindow(skString name);
		void addChildWindow(sxWindow window);

		void removeChildWindow(int ID);
		void removeChildWindow(skString name);
		void removeChildWindow(sxWindow window);

		sxWindow getChild(int ID);
		sxWindow getChild(skString name);
		sxWindow getChildRecursive(int ID);					-- may return null!
		sxWindow getChildRecursive(skString name);			-- may return null!
		sxWindow getChildAtIdx(int ID);
		sxWindow getActiveChild();							-- may return null!
		sxWindow getChildAtPosition(sxVector2 pos);			-- may return null!
		sxWindow getTargetChildAtPosition(sxVector2 pos);	-- may return null!

		sxWindow getParent();								-- may return null!

		bool isAncestor(int ID);
		bool isAncestor(skString name);
		bool isAncestor(sxWindow window);

		void setFont(skString name);
		void setFont(sxFont font);
		sxFont getFont();

		void setText(skString text);
		void insertText(skString text, int position);
		void appendText(skString text);
		skString getText();

		bool inheritsAlpha();
		float getAlpha();
		float getEffectiveAlpha();
		void setAlpha(float a);
		void setInheritsAlpha(bool setting);

		void requestRedraw();

		bool isZOrderingEnabled();
		void setZOrderingEnabled(bool setting);

		sxRect getPixelRect();
		sxRect getInnerRect();
		sxRect getUnclippedPixelRect();
		sxRect getUnclippedInnerRect();

		sxSize getParentPixelSize();
		float getParentPixelWidth();
		float getParentPixelHeight();

		sxWindow getCaptureWindow();		-- may return null!
		void setRestoreCapture(bool setting);
		bool restoresOldCapture();

		bool distributesCapturedInputs();
		void setDistributesCapturedInputs(bool setting);

		bool captureInput();
		void releaseInput();

		bool isCapturedByThis();
		bool isCapturedByAncestor();
		bool isCapturedByChild();

		bool isHit(sxVector2 pos);

		bool isRiseOnClickEnabled();
		void setRiseOnClickEnabled(bool setting);

		sxImage getMouseCursor();			-- may return null!
		void setMouseCursor(skString imageset, skString image);

		void moveToFront();
		void moveToBack();

		bool wantsMultiClickEvents();
		bool isMouseAutoRepeatEnabled();
		float getAutoRepeatDelay();
		float getAutoRepeatRate();
		void setWantsMultiClickEvents(bool setting);
		void setMouseAutoRepeatEnabled(bool setting);
		void setAutoRepeatDelay(float delay);
		void setAutoRepeatRate(float rate);

		bool isUsingDefaultTooltip();
		sxTooltip getTooltip();				-- may return null!
		string getTooltipType();
		string getTooltipText();
		bool inheritsTooltipText();

		void setTooltip(sxTooltip tooltip);
		void setTooltipType(skString tooltipType);
		void setTooltipText(skString tip);
		void setInheritsTooltipText(bool setting);

		bool testClassName(skString class_name);

		bool isDragDropTarget()
		void setDragDropTarget(bool setting)
		void notifyDragDropItemEnters(sxDragContainer item);
		void notifyDragDropItemLeaves(sxDragContainer item);
		void notifyDragDropItemDropped(sxDragContainer item);

		int getVerticalAlignment();
		int getHorizontalAlignment();

		void setVerticalAlignment(int alignment);
		void setHorizontalAlignment(int alignment);

		skString getLookNFeel();
		void setLookNFeel(skString lnf);

		void setWindowRenderer(skString name);
		skString getWindowRendererName();

		bool getModalState();
		void setModalState(bool state);

		skString getUserString(skString name);
		bool isUserStringDefined(skString name);
		void setUserString(skString name, skString value);

		void beginInitialisation();
		void endInitialisation();

		// unified stuff
		void setArea(sxUDim xpos, sxUDim ypos, sxUDim width, sxUDim height);
		void setArea(sxUVector2 pos, sxUVector2 size);
		void setArea(sxURect area);
		void setPosition(sxUVector2 pos);
		void setXPosition(sxUDim x);
		void setYPosition(sxUDim y);
		void setSize(sxUVector2 size);
		void setWidth(sxUDim width);
		void setHeight(sxUDim height);
		void setMaxSize(sxUVector2 size);
		void setMinSize(sxUVector2 size);

		sxURect getArea();
		sxUVector2 getPosition();
		sxUDim getXPosition();
		sxUDim getYPosition();
		sxUVector2 getSize();
		sxUDim getWidth();
		sxUDim getHeight();
		sxUVector2 getMaxSize();
		sxUVector2 getMinSize();

		// event pass through
		bool isMousePassThroughEnabled();
		void setMousePassThroughEnabled(bool setting);

		void rename(skString new_name);

		//////////////////////////////////////////////
		// PropertySet
		void setProperty(skString name, skString value);
		skString getProperty(skString name);
		skString getPropertyDefault(skString name);
		skString getPropertyHelp(skString name);
		bool isPropertyPresent(skString name);
		bool isPropertyDefault(skString name);

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

	for each:	qualified iterator 'Property'
				qualified iterator 'Event'
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxWindow

#define SXWINDOW_METHODS 145

#define IS_WINDOW(o)		CheckType<RFSX::sxWindow>(o, RFSXU_WINDOW)

namespace RFSX
{

class SX_IMPL_TYPE: public skExecutable
{
public:
	SX_IMPL_TYPE(CEGUI::Window* window);
	virtual ~SX_IMPL_TYPE();
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);
	virtual skExecutableIterator* createIterator(const skString& qualifier);
	virtual int executableType() const { return RFSXU_WINDOW; }

	CEGUI::Window* Window()				{ return m_Window; }
	CEGUI::EventSet* EventSet()			{ return m_Window; }
	CEGUI::PropertySet* PropertySet()	{ return m_Window; }

protected:
	SX_METHOD_DECL(getType);
	SX_METHOD_DECL(getName);

	SX_METHOD_DECL(isDestroyedByParent);
	SX_METHOD_DECL(isAlwaysOnTop);
	SX_METHOD_DECL(isDisabled);
	SX_METHOD_DECL(isVisible);
	SX_METHOD_DECL(isActive);
	SX_METHOD_DECL(isClippedByParent);

	SX_METHOD_DECL(setDestroyedByParent);
	SX_METHOD_DECL(setAlwaysOnTop);
	SX_METHOD_DECL(setEnabled);
	SX_METHOD_DECL(enable);
	SX_METHOD_DECL(disable);
	SX_METHOD_DECL(setVisible);
	SX_METHOD_DECL(show);
	SX_METHOD_DECL(hide);
	SX_METHOD_DECL(activate);
	SX_METHOD_DECL(deactivate);

	SX_METHOD_DECL(setClippedByParent);

	SX_METHOD_DECL(getID);
	SX_METHOD_DECL(setID);

	SX_METHOD_DECL(getChildCount);

	SX_METHOD_DECL(isChild);
	SX_METHOD_DECL(isChildRecursive);

	SX_METHOD_DECL(addChildWindow);

	SX_METHOD_DECL(removeChildWindow);

	SX_METHOD_DECL(getChild);
	SX_METHOD_DECL(getChildRecursive);
	SX_METHOD_DECL(getChildAtIdx);
	SX_METHOD_DECL(getActiveChild);
	SX_METHOD_DECL(getChildAtPosition);
	SX_METHOD_DECL(getTargetChildAtPosition);

	SX_METHOD_DECL(getParent);

	SX_METHOD_DECL(isAncestor);

	SX_METHOD_DECL(setFont);
	SX_METHOD_DECL(getFont);

	SX_METHOD_DECL(setText);
	SX_METHOD_DECL(insertText);
	SX_METHOD_DECL(appendText);
	SX_METHOD_DECL(getText);

	SX_METHOD_DECL(inheritsAlpha);
	SX_METHOD_DECL(getAlpha);
	SX_METHOD_DECL(getEffectiveAlpha);
	SX_METHOD_DECL(setAlpha);
	SX_METHOD_DECL(setInheritsAlpha);

	SX_METHOD_DECL(requestRedraw);

	SX_METHOD_DECL(isZOrderingEnabled);
	SX_METHOD_DECL(setZOrderingEnabled);

	SX_METHOD_DECL(getPixelRect);
	SX_METHOD_DECL(getInnerRect);
	SX_METHOD_DECL(getUnclippedPixelRect);
	SX_METHOD_DECL(getUnclippedInnerRect);

	SX_METHOD_DECL(getParentPixelSize);
	SX_METHOD_DECL(getParentPixelWidth);
	SX_METHOD_DECL(getParentPixelHeight);

	SX_METHOD_DECL(getCaptureWindow);
	SX_METHOD_DECL(setRestoreCapture);
	SX_METHOD_DECL(restoresOldCapture);

	SX_METHOD_DECL(distributesCapturedInputs);
	SX_METHOD_DECL(setDistributesCapturedInputs);

	SX_METHOD_DECL(captureInput);
	SX_METHOD_DECL(releaseInput);

	SX_METHOD_DECL(isCapturedByThis);
	SX_METHOD_DECL(isCapturedByAncestor);
	SX_METHOD_DECL(isCapturedByChild);

	SX_METHOD_DECL(isHit);

	SX_METHOD_DECL(isRiseOnClickEnabled);
	SX_METHOD_DECL(setRiseOnClickEnabled);

	SX_METHOD_DECL(getMouseCursor);
	SX_METHOD_DECL(setMouseCursor);

	SX_METHOD_DECL(moveToFront);
	SX_METHOD_DECL(moveToBack);

	SX_METHOD_DECL(wantsMultiClickEvents);
	SX_METHOD_DECL(isMouseAutoRepeatEnabled);
	SX_METHOD_DECL(getAutoRepeatDelay);
	SX_METHOD_DECL(getAutoRepeatRate);
	SX_METHOD_DECL(setWantsMultiClickEvents);
	SX_METHOD_DECL(setMouseAutoRepeatEnabled);
	SX_METHOD_DECL(setAutoRepeatDelay);
	SX_METHOD_DECL(setAutoRepeatRate);

	SX_METHOD_DECL(isUsingDefaultTooltip);
	SX_METHOD_DECL(getTooltip);
	SX_METHOD_DECL(getTooltipType);
	SX_METHOD_DECL(getTooltipText);
	SX_METHOD_DECL(inheritsTooltipText);

	SX_METHOD_DECL(setTooltip);
	SX_METHOD_DECL(setTooltipType);
	SX_METHOD_DECL(setTooltipText);
	SX_METHOD_DECL(setInheritsTooltipText);

	SX_METHOD_DECL(testClassName);

	SX_METHOD_DECL(isDragDropTarget);
	SX_METHOD_DECL(setDragDropTarget);
	SX_METHOD_DECL(notifyDragDropItemEnters);
	SX_METHOD_DECL(notifyDragDropItemLeaves);
	SX_METHOD_DECL(notifyDragDropItemDropped);

	SX_METHOD_DECL(getVerticalAlignment);
	SX_METHOD_DECL(getHorizontalAlignment);

	SX_METHOD_DECL(setVerticalAlignment);
	SX_METHOD_DECL(setHorizontalAlignment);

	SX_METHOD_DECL(getLookNFeel);
	SX_METHOD_DECL(setLookNFeel);

	SX_METHOD_DECL(setWindowRenderer);
	SX_METHOD_DECL(getWindowRendererName);

	SX_METHOD_DECL(getModalState);
	SX_METHOD_DECL(setModalState);

	SX_METHOD_DECL(getUserString);
	SX_METHOD_DECL(isUserStringDefined);
	SX_METHOD_DECL(setUserString);

	SX_METHOD_DECL(beginInitialisation);
	SX_METHOD_DECL(endInitialisation);

	// unified stuff
	SX_METHOD_DECL(setArea);
	SX_METHOD_DECL(setPosition);
	SX_METHOD_DECL(setXPosition);
	SX_METHOD_DECL(setYPosition);
	SX_METHOD_DECL(setSize);
	SX_METHOD_DECL(setWidth);
	SX_METHOD_DECL(setHeight);
	SX_METHOD_DECL(setMaxSize);
	SX_METHOD_DECL(setMinSize);

	SX_METHOD_DECL(getArea);
	SX_METHOD_DECL(getPosition);
	SX_METHOD_DECL(getXPosition);
	SX_METHOD_DECL(getYPosition);
	SX_METHOD_DECL(getSize);
	SX_METHOD_DECL(getWidth);
	SX_METHOD_DECL(getHeight);
	SX_METHOD_DECL(getMaxSize);
	SX_METHOD_DECL(getMinSize);

	// event pass through
	SX_METHOD_DECL(isMousePassThroughEnabled);
	SX_METHOD_DECL(setMousePassThroughEnabled);

	SX_METHOD_DECL(rename);

	// PropertySet
	SX_METHOD_DECL(setProperty);
	SX_METHOD_DECL(getProperty);
	SX_METHOD_DECL(getPropertyDefault);
	SX_METHOD_DECL(getPropertyHelp);
	SX_METHOD_DECL(isPropertyPresent);
	SX_METHOD_DECL(isPropertyDefault);

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
	CEGUI::Window*	m_Window;

	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_WINDOW_H_
