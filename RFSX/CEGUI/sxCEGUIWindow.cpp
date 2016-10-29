/************************************************************************************//**
 * @file sxCEGUIWindow.cpp
 * @brief Implementation for RFSX TEMPLATE object.
 * @author Daniel Queteschiner
 * @date December 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUIWindow.h"
#include "sxCEGUIEventArgs.h"
#include "sxCEGUIEventConnection.h"
#include "sxCEGUIUVector2.h"
#include "sxCEGUIVector2.h"
#include "sxCEGUIURect.h"
#include "sxCEGUIRect.h"
#include "sxCEGUIUDim.h"
#include "sxCEGUISize.h"
#include "sxCEGUIFont.h"
#include "sxCEGUIImage.h"
#include "sxCEGUIIterators.h"
#include "elements/sxCEGUITooltip.h"
#include "elements/sxCEGUIDragContainer.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxWindow

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxWindow implementation
//////////////////////////////////////////////////////////////////////////////////////////

SX_IMPL_TYPE::SX_IMPL_TYPE(CEGUI::Window* window)
	: m_Window(window)
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


skExecutableIterator* SX_IMPL_TYPE::createIterator(const skString& qualifier)
{
	if(qualifier == "Event")
		return new sxEventIterator(this->EventSet());

	if(qualifier == "Property")
		return new sxPropertyIterator(this->PropertySet());

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxWindow method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(getType)
{
	r_val = skString(caller->Window()->getType().c_str());
	return true;
}


SX_METHOD_IMPL(getName)
{
	r_val = skString(caller->Window()->getName().c_str());
	return true;
}


SX_METHOD_IMPL(isDestroyedByParent)
{
	r_val = caller->Window()->isDestroyedByParent();
	return true;
}


SX_METHOD_IMPL(isAlwaysOnTop)
{
	r_val = caller->Window()->isAlwaysOnTop();
	return true;
}


SX_METHOD_IMPL(isDisabled)
{
	if(args.entries() > 1)
		return false;

	bool localOnly = false;

	if(args.entries() == 1)
	{
		if(args[0].type() != skRValue::T_Bool)
			return false;

		localOnly = args[0].boolValue();
	}

	r_val = caller->Window()->isDisabled(localOnly);
	return true;
}


SX_METHOD_IMPL(isVisible)
{
	if(args.entries() > 1)
		return false;

	bool localOnly = false;

	if(args.entries() == 1)
	{
		if(args[0].type() != skRValue::T_Bool)
			return false;

		localOnly = args[0].boolValue();
	}

	r_val = caller->Window()->isVisible(localOnly);
	return true;
}


SX_METHOD_IMPL(isActive)
{
	r_val = caller->Window()->isActive();
	return true;
}


SX_METHOD_IMPL(isClippedByParent)
{
	r_val = caller->Window()->isClippedByParent();
	return true;
}


SX_METHOD_IMPL(setDestroyedByParent)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	caller->Window()->setDestroyedByParent(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(setAlwaysOnTop)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	caller->Window()->setAlwaysOnTop(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(setEnabled)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	caller->Window()->setEnabled(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(enable)
{
	caller->Window()->enable();
	return true;
}


SX_METHOD_IMPL(disable)
{
	caller->Window()->disable();
	return true;
}


SX_METHOD_IMPL(setVisible)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	caller->Window()->setVisible(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(show)
{
	caller->Window()->show();
	return true;
}


SX_METHOD_IMPL(hide)
{
	caller->Window()->hide();
	return true;
}


SX_METHOD_IMPL(activate)
{
	caller->Window()->activate();
	return true;
}


SX_METHOD_IMPL(deactivate)
{
	caller->Window()->deactivate();
	return true;
}


SX_METHOD_IMPL(setClippedByParent)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	caller->Window()->setClippedByParent(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(getID)
{
	r_val = static_cast<int>(caller->Window()->getID());
	return true;
}


SX_METHOD_IMPL(setID)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	caller->Window()->setID(args[0].intValue());
	return true;
}


SX_METHOD_IMPL(getChildCount)
{
	r_val = static_cast<int>(caller->Window()->getChildCount());
	return true;
}


SX_METHOD_IMPL(isChild)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() == skRValue::T_Int)
	{
		r_val = caller->Window()->isChild(args[0].intValue());
	}
	else if(args[0].type() == skRValue::T_String)
	{
		r_val = caller->Window()->isChild(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	}
	else
	{
		sxWindow *win = IS_WINDOW(args[0]);
		if(!win)
			return false;

		r_val = caller->Window()->isChild(win->Window());
	}

	return true;
}


SX_METHOD_IMPL(isChildRecursive)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	r_val = caller->Window()->isChildRecursive(args[0].intValue());
	return true;
}


SX_METHOD_IMPL(addChildWindow)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() == skRValue::T_String)
	{
		caller->Window()->addChildWindow(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	}
	else
	{
		sxWindow *win = IS_WINDOW(args[0]);
		if(!win)
			return false;

		caller->Window()->addChildWindow(win->Window());
	}

	return true;
}


SX_METHOD_IMPL(removeChildWindow)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() == skRValue::T_Int)
	{
		caller->Window()->removeChildWindow(args[0].intValue());
	}
	else if(args[0].type() == skRValue::T_String)
	{
		caller->Window()->removeChildWindow(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	}
	else
	{
		sxWindow *win = IS_WINDOW(args[0]);
		if(!win)
			return false;

		caller->Window()->removeChildWindow(win->Window());
	}

	return true;
}


SX_METHOD_IMPL(getChild)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() == skRValue::T_Int)
	{
		CEGUI::Window *win = NULL;

		try
		{
			win = caller->Window()->getChild(args[0].intValue());
			//r_val.assignObject(new sxWindow(caller->Window()->getChild(args[0].intValue())), true);
		}
		catch(CEGUI::UnknownObjectException CEGUIDeadException(&e))
		{
			//return false;
		}

		if(win)
			r_val.assignObject(new sxWindow(win), true);
		else
			r_val.assignObject(&ctxt.getInterpreter()->getNull());

		return true;
	}

	if(args[0].type() == skRValue::T_String)
	{
		CEGUI::Window *win = NULL;

		try
		{
			win = caller->Window()->getChild(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
			//r_val.assignObject(new sxWindow(), true);
		}
		catch(CEGUI::UnknownObjectException CEGUIDeadException(&e))
		{
			//return false;
		}

		if(win)
			r_val.assignObject(new sxWindow(win), true);
		else
			r_val.assignObject(&ctxt.getInterpreter()->getNull());

		return true;
	}

	return false;
}


SX_METHOD_IMPL(getChildRecursive)
{
	if(args.entries() != 1)
		return false;

	CEGUI::Window *win = NULL;

	if(args[0].type() == skRValue::T_Int)
	{
		win = caller->Window()->getChildRecursive(args[0].intValue());
	}
	else if(args[0].type() == skRValue::T_String)
	{
		win = caller->Window()->getChildRecursive(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	}
	else
	{
		return false;
	}

	if(win)
		r_val.assignObject(new sxWindow(win), true);
	else
		r_val.assignObject(&ctxt.getInterpreter()->getNull());

	return false;
}


SX_METHOD_IMPL(getChildAtIdx)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	r_val.assignObject(new sxWindow(caller->Window()->getChildAtIdx(args[0].intValue())), true);
	return true;
}


SX_METHOD_IMPL(getActiveChild)
{
	CEGUI::Window *win = caller->Window()->getActiveChild();

	if(win)
		r_val.assignObject(new sxWindow(win), true);
	else
		r_val.assignObject(&ctxt.getInterpreter()->getNull());

	return true;
}


SX_METHOD_IMPL(getChildAtPosition)
{
	if(args.entries() != 1)
		return false;

	sxVector2 *vec = IS_VECTOR2(args[0]);
	if(!vec)
		return false;

	CEGUI::Window *win = caller->Window()->getChildAtPosition(vec->Vector2ConstRef());

	if(win)
		r_val.assignObject(new sxWindow(win), true);
	else
		r_val.assignObject(&ctxt.getInterpreter()->getNull());

	return true;
}


SX_METHOD_IMPL(getTargetChildAtPosition)
{
	if(args.entries() != 1)
		return false;

	sxVector2 *vec = IS_VECTOR2(args[0]);
	if(!vec)
		return false;

	CEGUI::Window *win = caller->Window()->getTargetChildAtPosition(vec->Vector2ConstRef());

	if(win)
		r_val.assignObject(new sxWindow(win), true);
	else
		r_val.assignObject(&ctxt.getInterpreter()->getNull());

	return true;
}


SX_METHOD_IMPL(getParent)
{
	CEGUI::Window *win = caller->Window()->getParent();

	if(win)
		r_val.assignObject(new sxWindow(win), true);
	else
		r_val.assignObject(&ctxt.getInterpreter()->getNull());

	return true;
}


SX_METHOD_IMPL(isAncestor)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() == skRValue::T_Int)
	{
		r_val = caller->Window()->isAncestor(args[0].intValue());
	}
	else if(args[0].type() == skRValue::T_String)
	{
		r_val = caller->Window()->isAncestor(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	}
	else
	{
		sxWindow *win = IS_WINDOW(args[0]);
		if(!win)
			return false;

		r_val = caller->Window()->isAncestor(win->Window());
	}

	return true;
}


SX_METHOD_IMPL(setFont)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() == skRValue::T_String)
	{
		caller->Window()->setFont(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	}
	else
	{
		sxFont *font = IS_FONT(args[0]);
		if(!font)
			return false;

		caller->Window()->setFont(font->Font());
	}

	return true;
}


SX_METHOD_IMPL(getFont)
{
	r_val.assignObject(new sxFont(caller->Window()->getFont()), true);
	return true;
}


SX_METHOD_IMPL(setText)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	caller->Window()->setText(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	return true;
}


SX_METHOD_IMPL(insertText)
{
	skRValue::RType rtypes[] = { skRValue::T_String, skRValue::T_Int };

	if(!CheckParams(2, 2, args, rtypes))
		return false;

	caller->Window()->insertText(	reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()),
									static_cast<unsigned long>(args[1].intValue()));
	return true;
}


SX_METHOD_IMPL(appendText)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	caller->Window()->appendText(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	return true;
}


SX_METHOD_IMPL(getText)
{
	r_val = skString(caller->Window()->getText().c_str());
	return true;
}


SX_METHOD_IMPL(inheritsAlpha)
{
	r_val = caller->Window()->inheritsAlpha();
	return true;
}


SX_METHOD_IMPL(getAlpha)
{
	r_val = caller->Window()->getAlpha();
	return true;
}


SX_METHOD_IMPL(getEffectiveAlpha)
{
	r_val = caller->Window()->getEffectiveAlpha();
	return true;
}


SX_METHOD_IMPL(setAlpha)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Float)
		return false;

	caller->Window()->setAlpha(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(setInheritsAlpha)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	caller->Window()->setInheritsAlpha(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(requestRedraw)
{
	caller->Window()->requestRedraw();
	return true;
}


SX_METHOD_IMPL(isZOrderingEnabled)
{
	r_val = caller->Window()->isZOrderingEnabled();
	return true;
}


SX_METHOD_IMPL(setZOrderingEnabled)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	caller->Window()->setZOrderingEnabled(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(getPixelRect)
{
	r_val.assignObject(new sxRect(caller->Window()->getPixelRect()), true);
	return true;
}


SX_METHOD_IMPL(getInnerRect)
{
	r_val.assignObject(new sxRect(caller->Window()->getInnerRect()), true);
	return true;
}


SX_METHOD_IMPL(getUnclippedPixelRect)
{
	r_val.assignObject(new sxRect(caller->Window()->getUnclippedPixelRect()), true);
	return true;
}


SX_METHOD_IMPL(getUnclippedInnerRect)
{
	r_val.assignObject(new sxRect(caller->Window()->getUnclippedInnerRect()), true);
	return true;
}


SX_METHOD_IMPL(getParentPixelSize)
{
	r_val.assignObject(new sxSize(caller->Window()->getParentPixelSize()), true);
	return true;
}


SX_METHOD_IMPL(getParentPixelWidth)
{
	r_val = caller->Window()->getParentPixelWidth();
	return true;
}


SX_METHOD_IMPL(getParentPixelHeight)
{
	r_val = caller->Window()->getParentPixelHeight();
	return true;
}


SX_METHOD_IMPL(getCaptureWindow)
{
	CEGUI::Window *win = CEGUI::Window::getCaptureWindow();

	if(win)
		r_val.assignObject(new sxWindow(win), true);
	else
		r_val.assignObject(&ctxt.getInterpreter()->getNull());

	return true;
}


SX_METHOD_IMPL(setRestoreCapture)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	caller->Window()->setRestoreCapture(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(restoresOldCapture)
{
	r_val = caller->Window()->restoresOldCapture();
	return true;
}


SX_METHOD_IMPL(distributesCapturedInputs)
{
	r_val = caller->Window()->distributesCapturedInputs();
	return true;
}


SX_METHOD_IMPL(setDistributesCapturedInputs)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	caller->Window()->setDistributesCapturedInputs(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(captureInput)
{
	r_val = caller->Window()->captureInput();
	return true;
}


SX_METHOD_IMPL(releaseInput)
{
	caller->Window()->releaseInput();
	return true;
}


SX_METHOD_IMPL(isCapturedByThis)
{
	r_val = caller->Window()->isCapturedByThis();
	return true;
}


SX_METHOD_IMPL(isCapturedByAncestor)
{
	r_val = caller->Window()->isCapturedByAncestor();
	return true;
}


SX_METHOD_IMPL(isCapturedByChild)
{
	r_val = caller->Window()->isCapturedByChild();
	return true;
}


SX_METHOD_IMPL(isHit)
{
	if(args.entries() != 1)
		return false;

	sxVector2 *vec = IS_VECTOR2(args[0]);
	if(!vec)
		return false;

	r_val = caller->Window()->isHit(vec->Vector2ConstRef());
	return true;
}


SX_METHOD_IMPL(isRiseOnClickEnabled)
{
	r_val = caller->Window()->isRiseOnClickEnabled();
	return true;
}


SX_METHOD_IMPL(setRiseOnClickEnabled)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	caller->Window()->setRiseOnClickEnabled(args[0].boolValue());
	return true;
}



SX_METHOD_IMPL(getMouseCursor)
{
	const CEGUI::Image* image = caller->Window()->getMouseCursor();

	if(image)
		r_val.assignObject(new sxImage(image), true);
	else
		r_val.assignObject(&ctxt.getInterpreter()->getNull());

	return true;
}


SX_METHOD_IMPL(setMouseCursor)
{
	skRValue::RType rtypes[] = { skRValue::T_String, skRValue::T_String };

	if(!CheckParams(2, 2, args, rtypes))
		return false;

	caller->Window()->setMouseCursor(	reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()),
										reinterpret_cast<const CEGUI::utf8*>(args[1].str().c_str()));
	return true;
}


SX_METHOD_IMPL(moveToFront)
{
	caller->Window()->moveToFront();
	return true;
}


SX_METHOD_IMPL(moveToBack)
{
	caller->Window()->moveToBack();
	return true;
}


SX_METHOD_IMPL(wantsMultiClickEvents)
{
	r_val = caller->Window()->wantsMultiClickEvents();
	return true;
}


SX_METHOD_IMPL(isMouseAutoRepeatEnabled)
{
	r_val = caller->Window()->isMouseAutoRepeatEnabled();
	return true;
}


SX_METHOD_IMPL(getAutoRepeatDelay)
{
	r_val = caller->Window()->getAutoRepeatDelay();
	return true;
}


SX_METHOD_IMPL(getAutoRepeatRate)
{
	r_val = caller->Window()->getAutoRepeatRate();
	return true;
}


SX_METHOD_IMPL(setWantsMultiClickEvents)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	caller->Window()->setWantsMultiClickEvents(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(setMouseAutoRepeatEnabled)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	caller->Window()->setMouseAutoRepeatEnabled(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(setAutoRepeatDelay)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Float)
		return false;

	caller->Window()->setAutoRepeatDelay(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(setAutoRepeatRate)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Float)
		return false;

	caller->Window()->setAutoRepeatRate(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(isUsingDefaultTooltip)
{
	r_val = caller->Window()->isUsingDefaultTooltip();
	return true;
}


SX_METHOD_IMPL(getTooltip)
{
	//Tooltip* getTooltip() const;
	CEGUI::Tooltip *tooltip = caller->Window()->getTooltip();

	if(tooltip)
		r_val.assignObject(new sxTooltip(tooltip), true);
	else
		r_val.assignObject(&ctxt.getInterpreter()->getNull());

	return true;
}


SX_METHOD_IMPL(getTooltipType)
{
	r_val = skString(caller->Window()->getTooltipType().c_str());
	return true;
}


SX_METHOD_IMPL(getTooltipText)
{
	r_val = skString(caller->Window()->getTooltipText().c_str());
	return true;
}


SX_METHOD_IMPL(inheritsTooltipText)
{
	r_val = caller->Window()->inheritsTooltipText();
	return true;
}


SX_METHOD_IMPL(setTooltip)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Object)
		return false;

	CEGUI::Tooltip *tooltip = NULL;

	if(args[0].obj() != &ctxt.getInterpreter()->getNull())
	{
		sxTooltip *tip = static_cast<sxTooltip*>(IS_WINDOW(args[0]));

		if(!tip)
			return false;

		tooltip = tip->Tooltip();
	}

	//void setTooltip(Tooltip* tooltip);
	caller->Window()->setTooltip(tooltip);
	return true;
}


SX_METHOD_IMPL(setTooltipType)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	caller->Window()->setTooltipType(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	return true;
}


SX_METHOD_IMPL(setTooltipText)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	caller->Window()->setTooltipText(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	return true;
}


SX_METHOD_IMPL(setInheritsTooltipText)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	caller->Window()->setInheritsTooltipText(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(testClassName)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	r_val = caller->Window()->testClassName(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	return true;
}


SX_METHOD_IMPL(isDragDropTarget)
{
	r_val = caller->Window()->isDragDropTarget();
	return true;
}


SX_METHOD_IMPL(setDragDropTarget)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	caller->Window()->setDragDropTarget(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(notifyDragDropItemEnters)
{
	if(args.entries() != 1)
		return false;

	sxDragContainer* item = static_cast<sxDragContainer*>(IS_WINDOW(args[0]));
	if(!item)
		return false;

	//void notifyDragDropItemEnters(DragContainer* item);
	caller->Window()->notifyDragDropItemEnters(item->DragContainer());
	return true;
}


SX_METHOD_IMPL(notifyDragDropItemLeaves)
{
	if(args.entries() != 1)
		return false;

	sxDragContainer* item = static_cast<sxDragContainer*>(IS_WINDOW(args[0]));
	if(!item)
		return false;

	//void notifyDragDropItemLeaves(DragContainer* item);
	caller->Window()->notifyDragDropItemLeaves(item->DragContainer());
	return true;
}


SX_METHOD_IMPL(notifyDragDropItemDropped)
{
	if(args.entries() != 1)
		return false;

	sxDragContainer* item = static_cast<sxDragContainer*>(IS_WINDOW(args[0]));
	if(!item)
		return false;

	//void notifyDragDropItemDropped(DragContainer* item);
	caller->Window()->notifyDragDropItemDropped(item->DragContainer());
	return true;
}


SX_METHOD_IMPL(getVerticalAlignment)
{
	r_val = caller->Window()->getVerticalAlignment();
	return true;
}


SX_METHOD_IMPL(getHorizontalAlignment)
{
	r_val = caller->Window()->getHorizontalAlignment();
	return true;
}


SX_METHOD_IMPL(setVerticalAlignment)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	caller->Window()->setVerticalAlignment(static_cast<CEGUI::VerticalAlignment>(args[0].intValue()));
	return true;
}


SX_METHOD_IMPL(setHorizontalAlignment)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	caller->Window()->setHorizontalAlignment(static_cast<CEGUI::HorizontalAlignment>(args[0].intValue()));
	return true;
}


SX_METHOD_IMPL(getLookNFeel)
{
	r_val = skString(caller->Window()->getLookNFeel().c_str());
	return true;
}


SX_METHOD_IMPL(setLookNFeel)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	try
	{
		caller->Window()->setLookNFeel(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	}
	catch(CEGUI::Exception CEGUIDeadException(&e))
	{
		return false;
	}

	return true;
}


SX_METHOD_IMPL(setWindowRenderer)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	try
	{
		caller->Window()->setWindowRenderer(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	}
	catch(CEGUI::Exception CEGUIDeadException(&e))
	{
		return false;
	}

	return true;
}


SX_METHOD_IMPL(getWindowRendererName)
{
	r_val = skString(caller->Window()->getWindowRendererName().c_str());
	return true;
}


SX_METHOD_IMPL(getModalState)
{
	r_val = caller->Window()->getModalState();
	return true;
}


SX_METHOD_IMPL(setModalState)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	caller->Window()->setModalState(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(getUserString)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	try
	{
		CEGUI::String s = caller->Window()->getUserString(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
		r_val = skString(s.c_str());
	}
	catch(CEGUI::UnknownObjectException CEGUIDeadException(&e))
	{
		return false;
	}

	return true;
}


SX_METHOD_IMPL(isUserStringDefined)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	r_val = caller->Window()->isUserStringDefined(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	return true;
}


SX_METHOD_IMPL(setUserString)
{
	skRValue::RType rtypes[] = { skRValue::T_String, skRValue::T_String };

	if(!CheckParams(2, 2, args, rtypes))
		return false;

	caller->Window()->setUserString(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()),
									reinterpret_cast<const CEGUI::utf8*>(args[1].str().c_str()));
	return true;
}


SX_METHOD_IMPL(beginInitialisation)
{
	caller->Window()->beginInitialisation();
	return true;
}


SX_METHOD_IMPL(endInitialisation)
{
	caller->Window()->endInitialisation();
	return true;
}


// unified stuff
SX_METHOD_IMPL(setArea)
{
	switch(args.entries())
	{
	case 1:
		{
			sxURect *urect = IS_URECT(args[0]);
			if(!urect)
				return false;

			caller->Window()->setArea(urect->URectConstRef());
			return true;
		}
	case 2:
		{
			sxUVector2 *pos	= IS_UVECTOR2(args[0]);
			sxUVector2 *size= IS_UVECTOR2(args[1]);
			if(!pos || ! size)
				return false;

			caller->Window()->setArea(pos->UVector2ConstRef(), size->UVector2ConstRef());
			return true;
		}
	case 4:
		{
			sxUDim *xpos	= IS_UDIM(args[0]);
			sxUDim *ypos	= IS_UDIM(args[1]);
			sxUDim *width	= IS_UDIM(args[2]);
			sxUDim *height	= IS_UDIM(args[3]);
			if(!xpos || !ypos || !width || !height)
				return false;

			caller->Window()->setArea(	xpos->UDimConstRef(),
										ypos->UDimConstRef(),
										width->UDimConstRef(),
										height->UDimConstRef());
			return true;
		}
	default:
		return false;
	}
}


SX_METHOD_IMPL(setPosition)
{
	if(args.entries() != 1)
		return false;

	sxUVector2 *uvec = IS_UVECTOR2(args[0]);
	if(!uvec)
		return false;

	caller->Window()->setPosition(uvec->UVector2ConstRef());
	return true;
}


SX_METHOD_IMPL(setXPosition)
{
	if(args.entries() != 1)
		return false;

	sxUDim *x = IS_UDIM(args[0]);
	if(!x)
		return false;

	caller->Window()->setXPosition(x->UDimConstRef());
	return true;
}


SX_METHOD_IMPL(setYPosition)
{
	if(args.entries() != 1)
		return false;

	sxUDim *y = IS_UDIM(args[0]);
	if(!y)
		return false;

	caller->Window()->setYPosition(y->UDimConstRef());
	return true;
}


SX_METHOD_IMPL(setSize)
{
	if(args.entries() != 1)
		return false;

	sxUVector2 *size= IS_UVECTOR2(args[0]);
	if(!size)
		return false;

	caller->Window()->setSize(size->UVector2ConstRef());
	return true;
}


SX_METHOD_IMPL(setWidth)
{
	if(args.entries() != 1)
		return false;

	sxUDim *width = IS_UDIM(args[0]);
	if(!width)
		return false;

	caller->Window()->setWidth(width->UDimConstRef());
	return true;
}


SX_METHOD_IMPL(setHeight)
{
	if(args.entries() != 1)
		return false;

	sxUDim *height = IS_UDIM(args[0]);
	if(!height)
		return false;

	caller->Window()->setHeight(height->UDimConstRef());
	return true;
}


SX_METHOD_IMPL(setMaxSize)
{
	if(args.entries() != 1)
		return false;

	sxUVector2 *size= IS_UVECTOR2(args[0]);
	if(!size)
		return false;

	caller->Window()->setMaxSize(size->UVector2ConstRef());
	return true;
}


SX_METHOD_IMPL(setMinSize)
{
	if(args.entries() != 1)
		return false;

	sxUVector2 *size= IS_UVECTOR2(args[0]);
	if(!size)
		return false;

	caller->Window()->setMinSize(size->UVector2ConstRef());
	return true;
}


SX_METHOD_IMPL(getArea)
{
	r_val.assignObject(new sxURect(caller->Window()->getArea()), true);
	return true;
}


SX_METHOD_IMPL(getPosition)
{
	r_val.assignObject(new sxUVector2(caller->Window()->getPosition()), true);
	return true;
}


SX_METHOD_IMPL(getXPosition)
{
	r_val.assignObject(new sxUDim(caller->Window()->getXPosition()), true);
	return true;
}


SX_METHOD_IMPL(getYPosition)
{
	r_val.assignObject(new sxUDim(caller->Window()->getYPosition()), true);
	return true;
}


SX_METHOD_IMPL(getSize)
{
	r_val.assignObject(new sxUVector2(caller->Window()->getSize()), true);
	return true;
}


SX_METHOD_IMPL(getWidth)
{
	r_val.assignObject(new sxUDim(caller->Window()->getWidth()), true);
	return true;
}


SX_METHOD_IMPL(getHeight)
{
	r_val.assignObject(new sxUDim(caller->Window()->getHeight()), true);
	return true;
}


SX_METHOD_IMPL(getMaxSize)
{
	r_val.assignObject(new sxUVector2(caller->Window()->getMaxSize()), true);
	return true;
}


SX_METHOD_IMPL(getMinSize)
{
	r_val.assignObject(new sxUVector2(caller->Window()->getMinSize()), true);
	return true;
}


// event pass through
SX_METHOD_IMPL(isMousePassThroughEnabled)
{
	r_val = caller->Window()->isMousePassThroughEnabled();
	return true;
}


SX_METHOD_IMPL(setMousePassThroughEnabled)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	caller->Window()->setMousePassThroughEnabled(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(rename)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	try
	{
		caller->Window()->rename(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	}
	catch(CEGUI::Exception CEGUIDeadException(&e))
	{
		return false;
	}

	return true;
}


// PropertySet
SX_METHOD_IMPL(setProperty)
{
	skRValue::RType rtypes[] = { skRValue::T_String, skRValue::T_String };

	if(!CheckParams(2, 2, args, rtypes))
		return false;

	CEGUI::String name (reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	CEGUI::String value(reinterpret_cast<const CEGUI::utf8*>(args[1].str().c_str()));

	caller->PropertySet()->setProperty(name, value);
	return true;
}


SX_METHOD_IMPL(getProperty)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	CEGUI::String name(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	r_val = skString(caller->PropertySet()->getProperty(name).c_str());
	return true;
}


SX_METHOD_IMPL(getPropertyDefault)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	CEGUI::String name(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	r_val = skString(caller->PropertySet()->getPropertyDefault(name).c_str());
	return true;
}


SX_METHOD_IMPL(getPropertyHelp)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	CEGUI::String name(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	r_val = skString(caller->PropertySet()->getPropertyHelp(name).c_str());
	return true;
}


SX_METHOD_IMPL(isPropertyPresent)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	r_val = caller->PropertySet()->isPropertyPresent(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	return true;
}


SX_METHOD_IMPL(isPropertyDefault)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	r_val = caller->PropertySet()->isPropertyDefault(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
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

	try
	{
		r_val.assignObject(new sxEventConnection(caller->EventSet()->subscribeScriptedEvent(name, callback_name)), true);
	}
	catch(CEGUI::UnknownObjectException CEGUIDeadException(&e))
	{
		return false;
	}

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
// sxWindow initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXWINDOW_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(getType);
	SX_ADD_METHOD(getName);

	SX_ADD_METHOD(isDestroyedByParent);
	SX_ADD_METHOD(isAlwaysOnTop);
	SX_ADD_METHOD(isDisabled);
	SX_ADD_METHOD(isVisible);
	SX_ADD_METHOD(isActive);
	SX_ADD_METHOD(isClippedByParent);

	SX_ADD_METHOD(setDestroyedByParent);
	SX_ADD_METHOD(setAlwaysOnTop);
	SX_ADD_METHOD(setEnabled);
	SX_ADD_METHOD(enable);
	SX_ADD_METHOD(disable);
	SX_ADD_METHOD(setVisible);
	SX_ADD_METHOD(show);
	SX_ADD_METHOD(hide);
	SX_ADD_METHOD(activate);
	SX_ADD_METHOD(deactivate);

	SX_ADD_METHOD(setClippedByParent);

	SX_ADD_METHOD(getID);
	SX_ADD_METHOD(setID);

	SX_ADD_METHOD(getChildCount);

	SX_ADD_METHOD(isChild);
	SX_ADD_METHOD(isChildRecursive);

	SX_ADD_METHOD(addChildWindow);

	SX_ADD_METHOD(removeChildWindow);

	SX_ADD_METHOD(getChild);
	SX_ADD_METHOD(getChildRecursive);
	SX_ADD_METHOD(getChildAtIdx);
	SX_ADD_METHOD(getActiveChild);
	SX_ADD_METHOD(getChildAtPosition);
	SX_ADD_METHOD(getTargetChildAtPosition);

	SX_ADD_METHOD(getParent);

	SX_ADD_METHOD(isAncestor);

	SX_ADD_METHOD(setFont);
	SX_ADD_METHOD(getFont);

	SX_ADD_METHOD(setText);
	SX_ADD_METHOD(insertText);
	SX_ADD_METHOD(appendText);
	SX_ADD_METHOD(getText);

	SX_ADD_METHOD(inheritsAlpha);
	SX_ADD_METHOD(getAlpha);
	SX_ADD_METHOD(getEffectiveAlpha);
	SX_ADD_METHOD(setAlpha);
	SX_ADD_METHOD(setInheritsAlpha);

	SX_ADD_METHOD(requestRedraw);

	SX_ADD_METHOD(isZOrderingEnabled);
	SX_ADD_METHOD(setZOrderingEnabled);

	SX_ADD_METHOD(getPixelRect);
	SX_ADD_METHOD(getInnerRect);
	SX_ADD_METHOD(getUnclippedPixelRect);
	SX_ADD_METHOD(getUnclippedInnerRect);

	SX_ADD_METHOD(getParentPixelSize);
	SX_ADD_METHOD(getParentPixelWidth);
	SX_ADD_METHOD(getParentPixelHeight);

	SX_ADD_METHOD(getCaptureWindow);
	SX_ADD_METHOD(setRestoreCapture);
	SX_ADD_METHOD(restoresOldCapture);

	SX_ADD_METHOD(distributesCapturedInputs);
	SX_ADD_METHOD(setDistributesCapturedInputs);

	SX_ADD_METHOD(captureInput);
	SX_ADD_METHOD(releaseInput);

	SX_ADD_METHOD(isCapturedByThis);
	SX_ADD_METHOD(isCapturedByAncestor);
	SX_ADD_METHOD(isCapturedByChild);

	SX_ADD_METHOD(isHit);

	SX_ADD_METHOD(isRiseOnClickEnabled);
	SX_ADD_METHOD(setRiseOnClickEnabled);

	SX_ADD_METHOD(getMouseCursor);
	SX_ADD_METHOD(setMouseCursor);

	SX_ADD_METHOD(moveToFront);
	SX_ADD_METHOD(moveToBack);

	SX_ADD_METHOD(wantsMultiClickEvents);
	SX_ADD_METHOD(isMouseAutoRepeatEnabled);
	SX_ADD_METHOD(getAutoRepeatDelay);
	SX_ADD_METHOD(getAutoRepeatRate);
	SX_ADD_METHOD(setWantsMultiClickEvents);
	SX_ADD_METHOD(setMouseAutoRepeatEnabled);
	SX_ADD_METHOD(setAutoRepeatDelay);
	SX_ADD_METHOD(setAutoRepeatRate);

	SX_ADD_METHOD(isUsingDefaultTooltip);
	SX_ADD_METHOD(getTooltip);
	SX_ADD_METHOD(getTooltipType);
	SX_ADD_METHOD(getTooltipText);
	SX_ADD_METHOD(inheritsTooltipText);

	SX_ADD_METHOD(setTooltip);
	SX_ADD_METHOD(setTooltipType);
	SX_ADD_METHOD(setTooltipText);
	SX_ADD_METHOD(setInheritsTooltipText);

	SX_ADD_METHOD(testClassName);

	SX_ADD_METHOD(isDragDropTarget);
	SX_ADD_METHOD(setDragDropTarget);
	SX_ADD_METHOD(notifyDragDropItemEnters);
	SX_ADD_METHOD(notifyDragDropItemLeaves);
	SX_ADD_METHOD(notifyDragDropItemDropped);

	SX_ADD_METHOD(getVerticalAlignment);
	SX_ADD_METHOD(getHorizontalAlignment);

	SX_ADD_METHOD(setVerticalAlignment);
	SX_ADD_METHOD(setHorizontalAlignment);

	SX_ADD_METHOD(getLookNFeel);
	SX_ADD_METHOD(setLookNFeel);

	SX_ADD_METHOD(setWindowRenderer);
	SX_ADD_METHOD(getWindowRendererName);

	SX_ADD_METHOD(getModalState);
	SX_ADD_METHOD(setModalState);

	SX_ADD_METHOD(getUserString);
	SX_ADD_METHOD(isUserStringDefined);
	SX_ADD_METHOD(setUserString);

	SX_ADD_METHOD(beginInitialisation);
	SX_ADD_METHOD(endInitialisation);

	// unified stuff
	SX_ADD_METHOD(setArea);
	SX_ADD_METHOD(setPosition);
	SX_ADD_METHOD(setXPosition);
	SX_ADD_METHOD(setYPosition);
	SX_ADD_METHOD(setSize);
	SX_ADD_METHOD(setWidth);
	SX_ADD_METHOD(setHeight);
	SX_ADD_METHOD(setMaxSize);
	SX_ADD_METHOD(setMinSize);

	SX_ADD_METHOD(getArea);
	SX_ADD_METHOD(getPosition);
	SX_ADD_METHOD(getXPosition);
	SX_ADD_METHOD(getYPosition);
	SX_ADD_METHOD(getSize);
	SX_ADD_METHOD(getWidth);
	SX_ADD_METHOD(getHeight);
	SX_ADD_METHOD(getMaxSize);
	SX_ADD_METHOD(getMinSize);

	// event pass through
	SX_ADD_METHOD(isMousePassThroughEnabled);
	SX_ADD_METHOD(setMousePassThroughEnabled);

	SX_ADD_METHOD(rename);

	// PropertySet
	SX_ADD_METHOD(setProperty);
	SX_ADD_METHOD(getProperty);
	SX_ADD_METHOD(getPropertyDefault);
	SX_ADD_METHOD(getPropertyHelp);
	SX_ADD_METHOD(isPropertyPresent);
	SX_ADD_METHOD(isPropertyDefault);

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
