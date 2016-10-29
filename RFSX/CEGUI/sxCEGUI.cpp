/***************************************************************************************
 * @file sxCEGUI.cpp
 * @brief
 * @author Daniel Queteschiner
 * @date November 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUI.h"
#include "sxCEGUIGlobalEventSet.h"
#include "sxCEGUIMouseCursor.h"
#include "sxCEGUIFontManager.h"
#include "sxCEGUIImagesetManager.h"
#include "sxCEGUISystem.h"
#include "sxCEGUIWindowManager.h"
#include "sxCEGUISchemeManager.h"
#include "sxCEGUIPropertyHelper.h"
#include "sxCEGUILogger.h"
#include "sxCEGUIKey.h"
#include "sxCEGUICoordConverter.h"

#include "elements/sxCEGUIButtonBase.h"
#include "elements/sxCEGUICheckbox.h"
#include "elements/sxCEGUICombobox.h"
#include "elements/sxCEGUIComboDropList.h"
#include "elements/sxCEGUIDragContainer.h"
#include "elements/sxCEGUIEditbox.h"
#include "elements/sxCEGUIFrameWindow.h"
#include "elements/sxCEGUIGUISheet.h"
#include "elements/sxCEGUIItemEntry.h"
#include "elements/sxCEGUIItemListBase.h"
#include "elements/sxCEGUIItemListbox.h"
#include "elements/sxCEGUIListbox.h"
#include "elements/sxCEGUIListHeader.h"
#include "elements/sxCEGUIListHeaderSegment.h"
#include "elements/sxCEGUIMenuBase.h"
#include "elements/sxCEGUIMenuItem.h"
#include "elements/sxCEGUIMultiColumnList.h"
#include "elements/sxCEGUIMultiLineEditbox.h"
#include "elements/sxCEGUIPopupMenu.h"
#include "elements/sxCEGUIProgressBar.h"
#include "elements/sxCEGUIPushButton.h"
#include "elements/sxCEGUIRadioButton.h"
#include "elements/sxCEGUIScrollablePane.h"
#include "elements/sxCEGUIScrollbar.h"
#include "elements/sxCEGUIScrolledContainer.h"
#include "elements/sxCEGUIScrolledItemListBase.h"
#include "elements/sxCEGUISlider.h"
#include "elements/sxCEGUISpinner.h"
#include "elements/sxCEGUITabButton.h"
#include "elements/sxCEGUITabControl.h"
#include "elements/sxCEGUIThumb.h"
#include "elements/sxCEGUITooltip.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxGUI

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxGUI implementation
//////////////////////////////////////////////////////////////////////////////////////////

SX_IMPL_TYPE::SX_IMPL_TYPE()
{
	m_pFontManager		= new sxFontManager();
	m_pImagesetManager	= new sxImagesetManager();
	m_pGlobalEventSet	= new sxGlobalEventSet();
	m_pMouseCursor		= new sxMouseCursor();
	m_pSystem			= new sxSystem();
	m_pWindowManager	= new sxWindowManager();
	m_pSchemeManager	= new sxSchemeManager();
	m_pPropertyHelper	= new sxPropertyHelper();
	m_pLogger			= new sxLogger();
	m_pKey				= new sxKey();
	m_pCoordConverter	= new sxCoordConverter();

	InitEnumHT();
}


SX_IMPL_TYPE::~SX_IMPL_TYPE()
{
	delete m_pFontManager;
	delete m_pImagesetManager;
	delete m_pGlobalEventSet;
	delete m_pMouseCursor;
	delete m_pSystem;
	delete m_pWindowManager;
	delete m_pSchemeManager;
	delete m_pPropertyHelper;
	delete m_pLogger;
	delete m_pKey;
	delete m_pCoordConverter;
}


bool SX_IMPL_TYPE::method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
{
	if(h_method(this, m_name, args, r_val, ctxt))
		return true;
	else
		return skExecutable::method(m_name, args, r_val, ctxt);
}


bool SX_IMPL_TYPE::getValue(const skString& name ,const skString& att, skRValue& val)
{
	if(name == "FontManager")
	{
		val.assignObject(m_pFontManager);
		return true;
	}
	if(name == "ImagesetManager")
	{
		val.assignObject(m_pImagesetManager);
		return true;
	}
	if(name == "GlobalEventSet")
	{
		val.assignObject(m_pGlobalEventSet);
		return true;
	}
	if(name == "MouseCursor")
	{
		val.assignObject(m_pMouseCursor);
		return true;
	}
	if(name == "SchemeManager")
	{
		val.assignObject(m_pSchemeManager);
		return true;
	}
	if(name == "System")
	{
		val.assignObject(m_pSystem);
		return true;
	}
	if(name == "WindowManager")
	{
		val.assignObject(m_pWindowManager);
		return true;
	}
	if(name == "PropertyHelper")
	{
		val.assignObject(m_pPropertyHelper);
		return true;
	}
	if(name == "Logger")
	{
		val.assignObject(m_pLogger);
		return true;
	}
	if(name == "Key")
	{
		val.assignObject(m_pKey);
		return true;
	}
	if(name == "CoordConverter")
	{
		val.assignObject(m_pCoordConverter);
		return true;
	}

	int *intValue = h_Enum.value(name);
	if(intValue)
	{
		val = *intValue;
		return true;
	}

	return skExecutable::getValue(name, att, val);
}


//////////////////////////////////////////////////////////////////////////////////////////
// sxGUI method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(toMouseCursorEventArgs)
{
	if(args.entries() != 1)
		return false;

	sxEventArgs *eargs = IS_EVENTARGS(args[0]);
	if(!eargs)
		return false;

	r_val.assignObject(new sxMouseCursorEventArgs(static_cast<CEGUI::MouseCursorEventArgs*>(eargs->EventArgs())), true);
	return true;
}


SX_METHOD_IMPL(toWindowEventArgs)
{
	if(args.entries() != 1)
		return false;

	sxEventArgs *eargs = IS_EVENTARGS(args[0]);
	if(!eargs)
		return false;

	r_val.assignObject(new sxWindowEventArgs(static_cast<CEGUI::WindowEventArgs*>(eargs->EventArgs())), true);
	return true;
}


SX_METHOD_IMPL(toActivationEventArgs)
{
	if(args.entries() != 1)
		return false;

	sxEventArgs *eargs = IS_EVENTARGS(args[0]);
	if(!eargs)
		return false;

	r_val.assignObject(new sxActivationEventArgs(static_cast<CEGUI::ActivationEventArgs*>(eargs->EventArgs())), true);
	return true;
}


SX_METHOD_IMPL(toHeaderSequenceEventArgs)
{
	if(args.entries() != 1)
		return false;

	sxEventArgs *eargs = IS_EVENTARGS(args[0]);
	if(!eargs)
		return false;

	r_val.assignObject(new sxHeaderSequenceEventArgs(static_cast<CEGUI::HeaderSequenceEventArgs*>(eargs->EventArgs())), true);
	return true;
}


SX_METHOD_IMPL(toMouseEventArgs)
{
	if(args.entries() != 1)
		return false;

	sxEventArgs *eargs = IS_EVENTARGS(args[0]);
	if(!eargs)
		return false;

	r_val.assignObject(new sxMouseEventArgs(static_cast<CEGUI::MouseEventArgs*>(eargs->EventArgs())), true);
	return true;
}


SX_METHOD_IMPL(toKeyEventArgs)
{
	if(args.entries() != 1)
		return false;

	sxEventArgs *eargs = IS_EVENTARGS(args[0]);
	if(!eargs)
		return false;

	r_val.assignObject(new sxKeyEventArgs(static_cast<CEGUI::KeyEventArgs*>(eargs->EventArgs())), true);
	return true;
}


SX_METHOD_IMPL(toDragDropEventArgs)
{
	if(args.entries() != 1)
		return false;

	sxEventArgs *eargs = IS_EVENTARGS(args[0]);
	if(!eargs)
		return false;

	r_val.assignObject(new sxDragDropEventArgs(static_cast<CEGUI::DragDropEventArgs*>(eargs->EventArgs())), true);
	return true;
}


SX_METHOD_IMPL(toTreeEventArgs)
{
	if(args.entries() != 1)
		return false;

	sxEventArgs *eargs = IS_EVENTARGS(args[0]);
	if(!eargs)
		return false;

	r_val.assignObject(new sxTreeEventArgs(static_cast<CEGUI::TreeEventArgs*>(eargs->EventArgs())), true);
	return true;
}


SX_METHOD_IMPL(toButtonBase)
{
	if(args.entries() != 1)
		return false;

	sxWindow *wnd = IS_WINDOW(args[0]);
	if(!wnd)
		return false;

	r_val.assignObject(new sxButtonBase(static_cast<CEGUI::ButtonBase*>(wnd->Window())), true);
	return true;
}


SX_METHOD_IMPL(toCheckbox)
{
	if(args.entries() != 1)
		return false;

	sxWindow *wnd = IS_WINDOW(args[0]);
	if(!wnd)
		return false;

	r_val.assignObject(new sxCheckbox(static_cast<CEGUI::Checkbox*>(wnd->Window())), true);
	return true;
}


SX_METHOD_IMPL(toCombobox)
{
	if(args.entries() != 1)
		return false;

	sxWindow *wnd = IS_WINDOW(args[0]);
	if(!wnd)
		return false;

	r_val.assignObject(new sxCombobox(static_cast<CEGUI::Combobox*>(wnd->Window())), true);
	return true;
}


SX_METHOD_IMPL(toComboDropList)
{
	if(args.entries() != 1)
		return false;

	sxWindow *wnd = IS_WINDOW(args[0]);
	if(!wnd)
		return false;

	r_val.assignObject(new sxComboDropList(static_cast<CEGUI::ComboDropList*>(wnd->Window())), true);
	return true;
}


SX_METHOD_IMPL(toDragContainer)
{
	if(args.entries() != 1)
		return false;

	sxWindow *wnd = IS_WINDOW(args[0]);
	if(!wnd)
		return false;

	r_val.assignObject(new sxDragContainer(static_cast<CEGUI::DragContainer*>(wnd->Window())), true);
	return true;
}


SX_METHOD_IMPL(toEditbox)
{
	if(args.entries() != 1)
		return false;

	sxWindow *wnd = IS_WINDOW(args[0]);
	if(!wnd)
		return false;

	r_val.assignObject(new sxEditbox(static_cast<CEGUI::Editbox*>(wnd->Window())), true);
	return true;
}


SX_METHOD_IMPL(toFrameWindow)
{
	if(args.entries() != 1)
		return false;

	sxWindow *wnd = IS_WINDOW(args[0]);
	if(!wnd)
		return false;

	r_val.assignObject(new sxFrameWindow(static_cast<CEGUI::FrameWindow*>(wnd->Window())), true);
	return true;
}


SX_METHOD_IMPL(toGUISheet)
{
	if(args.entries() != 1)
		return false;

	sxWindow *wnd = IS_WINDOW(args[0]);
	if(!wnd)
		return false;

	r_val.assignObject(new sxGUISheet(static_cast<CEGUI::GUISheet*>(wnd->Window())), true);
	return true;
}


SX_METHOD_IMPL(toItemEntry)
{
	if(args.entries() != 1)
		return false;

	sxWindow *wnd = IS_WINDOW(args[0]);
	if(!wnd)
		return false;

	r_val.assignObject(new sxItemEntry(static_cast<CEGUI::ItemEntry*>(wnd->Window())), true);
	return true;
}


SX_METHOD_IMPL(toItemListBase)
{
	if(args.entries() != 1)
		return false;

	sxWindow *wnd = IS_WINDOW(args[0]);
	if(!wnd)
		return false;

	r_val.assignObject(new sxItemListBase(static_cast<CEGUI::ItemListBase*>(wnd->Window())), true);
	return true;
}


SX_METHOD_IMPL(toItemListbox)
{
	if(args.entries() != 1)
		return false;

	sxWindow *wnd = IS_WINDOW(args[0]);
	if(!wnd)
		return false;

	r_val.assignObject(new sxItemListbox(static_cast<CEGUI::ItemListbox*>(wnd->Window())), true);
	return true;
}


SX_METHOD_IMPL(toListbox)
{
	if(args.entries() != 1)
		return false;

	sxWindow *wnd = IS_WINDOW(args[0]);
	if(!wnd)
		return false;

	r_val.assignObject(new sxListbox(static_cast<CEGUI::Listbox*>(wnd->Window())), true);
	return true;
}


SX_METHOD_IMPL(toListHeader)
{
	if(args.entries() != 1)
		return false;

	sxWindow *wnd = IS_WINDOW(args[0]);
	if(!wnd)
		return false;

	r_val.assignObject(new sxListHeader(static_cast<CEGUI::ListHeader*>(wnd->Window())), true);
	return true;
}


SX_METHOD_IMPL(toListHeaderSegment)
{
	if(args.entries() != 1)
		return false;

	sxWindow *wnd = IS_WINDOW(args[0]);
	if(!wnd)
		return false;

	r_val.assignObject(new sxListHeaderSegment(static_cast<CEGUI::ListHeaderSegment*>(wnd->Window())), true);
	return true;
}


SX_METHOD_IMPL(toMenubar)
{
	if(args.entries() != 1)
		return false;

	sxWindow *wnd = IS_WINDOW(args[0]);
	if(!wnd)
		return false;

	r_val.assignObject(new sxMenubar(static_cast<CEGUI::Menubar*>(wnd->Window())), true);
	return true;
}


SX_METHOD_IMPL(toMenuBase)
{
	if(args.entries() != 1)
		return false;

	sxWindow *wnd = IS_WINDOW(args[0]);
	if(!wnd)
		return false;

	r_val.assignObject(new sxMenuBase(static_cast<CEGUI::MenuBase*>(wnd->Window())), true);
	return true;
}


SX_METHOD_IMPL(toMenuItem)
{
	if(args.entries() != 1)
		return false;

	sxWindow *wnd = IS_WINDOW(args[0]);
	if(!wnd)
		return false;

	r_val.assignObject(new sxMenuItem(static_cast<CEGUI::MenuItem*>(wnd->Window())), true);
	return true;
}


SX_METHOD_IMPL(toMultiColumnList)
{
	if(args.entries() != 1)
		return false;

	sxWindow *wnd = IS_WINDOW(args[0]);
	if(!wnd)
		return false;

	r_val.assignObject(new sxMultiColumnList(static_cast<CEGUI::MultiColumnList*>(wnd->Window())), true);
	return true;
}


SX_METHOD_IMPL(toMultiLineEditbox)
{
	if(args.entries() != 1)
		return false;

	sxWindow *wnd = IS_WINDOW(args[0]);
	if(!wnd)
		return false;

	r_val.assignObject(new sxMultiLineEditbox(static_cast<CEGUI::MultiLineEditbox*>(wnd->Window())), true);
	return true;
}


SX_METHOD_IMPL(toPopupMenu)
{
	if(args.entries() != 1)
		return false;

	sxWindow *wnd = IS_WINDOW(args[0]);
	if(!wnd)
		return false;

	r_val.assignObject(new sxPopupMenu(static_cast<CEGUI::PopupMenu*>(wnd->Window())), true);
	return true;
}


SX_METHOD_IMPL(toProgressBar)
{
	if(args.entries() != 1)
		return false;

	sxWindow *wnd = IS_WINDOW(args[0]);
	if(!wnd)
		return false;

	r_val.assignObject(new sxProgressBar(static_cast<CEGUI::ProgressBar*>(wnd->Window())), true);
	return true;
}


SX_METHOD_IMPL(toPushButton)
{
	if(args.entries() != 1)
		return false;

	sxWindow *wnd = IS_WINDOW(args[0]);
	if(!wnd)
		return false;

	r_val.assignObject(new sxPushButton(static_cast<CEGUI::PushButton*>(wnd->Window())), true);
	return true;
}


SX_METHOD_IMPL(toRadioButton)
{
	if(args.entries() != 1)
		return false;

	sxWindow *wnd = IS_WINDOW(args[0]);
	if(!wnd)
		return false;

	r_val.assignObject(new sxRadioButton(static_cast<CEGUI::RadioButton*>(wnd->Window())), true);
	return true;
}


SX_METHOD_IMPL(toScrollablePane)
{
	if(args.entries() != 1)
		return false;

	sxWindow *wnd = IS_WINDOW(args[0]);
	if(!wnd)
		return false;

	r_val.assignObject(new sxScrollablePane(static_cast<CEGUI::ScrollablePane*>(wnd->Window())), true);
	return true;
}


SX_METHOD_IMPL(toScrollbar)
{
	if(args.entries() != 1)
		return false;

	sxWindow *wnd = IS_WINDOW(args[0]);
	if(!wnd)
		return false;

	r_val.assignObject(new sxScrollbar(static_cast<CEGUI::Scrollbar*>(wnd->Window())), true);
	return true;
}


SX_METHOD_IMPL(toScrolledContainer)
{
	if(args.entries() != 1)
		return false;

	sxWindow *wnd = IS_WINDOW(args[0]);
	if(!wnd)
		return false;

	r_val.assignObject(new sxScrolledContainer(static_cast<CEGUI::ScrolledContainer*>(wnd->Window())), true);
	return true;
}


SX_METHOD_IMPL(toScrolledItemListBase)
{
	if(args.entries() != 1)
		return false;

	sxWindow *wnd = IS_WINDOW(args[0]);
	if(!wnd)
		return false;

	r_val.assignObject(new sxScrolledItemListBase(static_cast<CEGUI::ScrolledItemListBase*>(wnd->Window())), true);
	return true;
}


SX_METHOD_IMPL(toSlider)
{
	if(args.entries() != 1)
		return false;

	sxWindow *wnd = IS_WINDOW(args[0]);
	if(!wnd)
		return false;

	r_val.assignObject(new sxSlider(static_cast<CEGUI::Slider*>(wnd->Window())), true);
	return true;
}


SX_METHOD_IMPL(toSpinner)
{
	if(args.entries() != 1)
		return false;

	sxWindow *wnd = IS_WINDOW(args[0]);
	if(!wnd)
		return false;

	r_val.assignObject(new sxSpinner(static_cast<CEGUI::Spinner*>(wnd->Window())), true);
	return true;
}


SX_METHOD_IMPL(toTabButton)
{
	if(args.entries() != 1)
		return false;

	sxWindow *wnd = IS_WINDOW(args[0]);
	if(!wnd)
		return false;

	r_val.assignObject(new sxTabButton(static_cast<CEGUI::TabButton*>(wnd->Window())), true);
	return true;
}


SX_METHOD_IMPL(toTabControl)
{
	if(args.entries() != 1)
		return false;

	sxWindow *wnd = IS_WINDOW(args[0]);
	if(!wnd)
		return false;

	r_val.assignObject(new sxTabControl(static_cast<CEGUI::TabControl*>(wnd->Window())), true);
	return true;
}


SX_METHOD_IMPL(toThumb)
{
	if(args.entries() != 1)
		return false;

	sxWindow *wnd = IS_WINDOW(args[0]);
	if(!wnd)
		return false;

	r_val.assignObject(new sxThumb(static_cast<CEGUI::Thumb*>(wnd->Window())), true);
	return true;
}


SX_METHOD_IMPL(toTooltip)
{
	if(args.entries() != 1)
		return false;

	sxWindow *wnd = IS_WINDOW(args[0]);
	if(!wnd)
		return false;

	r_val.assignObject(new sxTooltip(static_cast<CEGUI::Tooltip*>(wnd->Window())), true);
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxGUI initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXGUI_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(toMouseCursorEventArgs);
	SX_ADD_METHOD(toWindowEventArgs);
	SX_ADD_METHOD(toActivationEventArgs);
	SX_ADD_METHOD(toHeaderSequenceEventArgs);
	SX_ADD_METHOD(toMouseEventArgs);
	SX_ADD_METHOD(toKeyEventArgs);
	SX_ADD_METHOD(toDragDropEventArgs);
	SX_ADD_METHOD(toTreeEventArgs);

	SX_ADD_METHOD(toButtonBase);
	SX_ADD_METHOD(toCheckbox);
	SX_ADD_METHOD(toCombobox);
	SX_ADD_METHOD(toComboDropList);
	SX_ADD_METHOD(toDragContainer);
	SX_ADD_METHOD(toEditbox);
	SX_ADD_METHOD(toFrameWindow);
	SX_ADD_METHOD(toGUISheet);
	SX_ADD_METHOD(toItemEntry);
	SX_ADD_METHOD(toItemListBase);
	SX_ADD_METHOD(toItemListbox);
	SX_ADD_METHOD(toListbox);
	SX_ADD_METHOD(toListHeader);
	SX_ADD_METHOD(toListHeaderSegment);
	SX_ADD_METHOD(toMenubar);
	SX_ADD_METHOD(toMenuBase);
	SX_ADD_METHOD(toMenuItem);
	SX_ADD_METHOD(toMultiColumnList);
	SX_ADD_METHOD(toMultiLineEditbox);
	SX_ADD_METHOD(toPopupMenu);
	SX_ADD_METHOD(toProgressBar);
	SX_ADD_METHOD(toPushButton);
	SX_ADD_METHOD(toRadioButton);
	SX_ADD_METHOD(toScrollablePane);
	SX_ADD_METHOD(toScrollbar);
	SX_ADD_METHOD(toScrolledContainer);
	SX_ADD_METHOD(toScrolledItemListBase);
	SX_ADD_METHOD(toSlider);
	SX_ADD_METHOD(toSpinner);
	SX_ADD_METHOD(toTabButton);
	SX_ADD_METHOD(toTabControl);
	SX_ADD_METHOD(toThumb);
	SX_ADD_METHOD(toTooltip);
}


void SX_IMPL_TYPE::InitEnumHT()
{
	h_Enum.insertKeyAndValue("Errors",		new int(CEGUI::Errors));
	h_Enum.insertKeyAndValue("Warnings",	new int(CEGUI::Warnings));
	h_Enum.insertKeyAndValue("Standard",	new int(CEGUI::Standard));
	h_Enum.insertKeyAndValue("Informative", new int(CEGUI::Informative));
	h_Enum.insertKeyAndValue("Insane",		new int(CEGUI::Insane));
	// TextFormatting
	h_Enum.insertKeyAndValue("LeftAligned",			new int(CEGUI::LeftAligned));
	h_Enum.insertKeyAndValue("RightAligned",		new int(CEGUI::RightAligned));
	h_Enum.insertKeyAndValue("Centred",				new int(CEGUI::Centred));
	h_Enum.insertKeyAndValue("Justified",			new int(CEGUI::Justified));
	h_Enum.insertKeyAndValue("WordWrapLeftAligned", new int(CEGUI::WordWrapLeftAligned));
	h_Enum.insertKeyAndValue("WordWrapRightAligned",new int(CEGUI::WordWrapRightAligned));
	h_Enum.insertKeyAndValue("WordWrapCentred",		new int(CEGUI::WordWrapCentred));
	h_Enum.insertKeyAndValue("WordWrapJustified",	new int(CEGUI::WordWrapJustified));
	// MouseButton
	h_Enum.insertKeyAndValue("LeftButton",			new int(CEGUI::LeftButton));
	h_Enum.insertKeyAndValue("RightButton",			new int(CEGUI::RightButton));
	h_Enum.insertKeyAndValue("MiddleButton",		new int(CEGUI::MiddleButton));
	h_Enum.insertKeyAndValue("X1Button",			new int(CEGUI::X1Button));
	h_Enum.insertKeyAndValue("X2Button",			new int(CEGUI::X2Button));
	h_Enum.insertKeyAndValue("MouseButtonCount",	new int(CEGUI::MouseButtonCount));
	h_Enum.insertKeyAndValue("NoButton",			new int(CEGUI::NoButton));
	// MouseCursorImage
	h_Enum.insertKeyAndValue("BlankMouseCursor",	new int(CEGUI::BlankMouseCursor));
	h_Enum.insertKeyAndValue("DefaultMouseCursor",	new int(CEGUI::DefaultMouseCursor));
	//SystemKey
	h_Enum.insertKeyAndValue("LeftMouse",	new int(CEGUI::LeftMouse));
	h_Enum.insertKeyAndValue("RightMouse",	new int(CEGUI::RightMouse));
	h_Enum.insertKeyAndValue("Shift",		new int(CEGUI::Shift));
	h_Enum.insertKeyAndValue("Control",		new int(CEGUI::Control));
	h_Enum.insertKeyAndValue("MiddleMouse", new int(CEGUI::MiddleMouse));
	h_Enum.insertKeyAndValue("X1Mouse",		new int(CEGUI::X1Mouse));
	h_Enum.insertKeyAndValue("X2Mouse",		new int(CEGUI::X2Mouse));
	h_Enum.insertKeyAndValue("Alt",			new int(CEGUI::Alt));
	// VerticalAlignment
	h_Enum.insertKeyAndValue("VA_TOP",		new int(CEGUI::VA_TOP));
	h_Enum.insertKeyAndValue("VA_CENTRE",	new int(CEGUI::VA_CENTRE));
	h_Enum.insertKeyAndValue("VA_BOTTOM",	new int(CEGUI::VA_BOTTOM));
	// HorizontalAlignment
	h_Enum.insertKeyAndValue("HA_LEFT",		new int(CEGUI::HA_LEFT));
	h_Enum.insertKeyAndValue("HA_CENTRE",	new int(CEGUI::HA_CENTRE));
	h_Enum.insertKeyAndValue("HA_RIGHT",	new int(CEGUI::HA_RIGHT));
	// Spinner (TextInputMode)
	h_Enum.insertKeyAndValue("FloatingPoint",	new int(CEGUI::Spinner::FloatingPoint));
	h_Enum.insertKeyAndValue("Integer",			new int(CEGUI::Spinner::Integer));
	h_Enum.insertKeyAndValue("Hexadecimal",		new int(CEGUI::Spinner::Hexadecimal));
	h_Enum.insertKeyAndValue("Octal",			new int(CEGUI::Spinner::Octal));
	// ListHeaderSegment (SortDirection)
	h_Enum.insertKeyAndValue("None",		new int(CEGUI::ListHeaderSegment::None));
	h_Enum.insertKeyAndValue("Ascending",	new int(CEGUI::ListHeaderSegment::Ascending));
	h_Enum.insertKeyAndValue("Descending",	new int(CEGUI::ListHeaderSegment::Descending));
	// MultiColumnList (SelectionMode)
	h_Enum.insertKeyAndValue("RowSingle",				new int(CEGUI::MultiColumnList::RowSingle));
	h_Enum.insertKeyAndValue("RowMultiple",				new int(CEGUI::MultiColumnList::RowMultiple));
	h_Enum.insertKeyAndValue("CellSingle",				new int(CEGUI::MultiColumnList::CellSingle));
	h_Enum.insertKeyAndValue("CellMultiple",			new int(CEGUI::MultiColumnList::CellMultiple));
	h_Enum.insertKeyAndValue("NominatedColumnSingle",	new int(CEGUI::MultiColumnList::NominatedColumnSingle));
	h_Enum.insertKeyAndValue("NominatedColumnMultiple",	new int(CEGUI::MultiColumnList::NominatedColumnMultiple));
	h_Enum.insertKeyAndValue("ColumnSingle",			new int(CEGUI::MultiColumnList::ColumnSingle));
	h_Enum.insertKeyAndValue("ColumnMultiple",			new int(CEGUI::MultiColumnList::ColumnMultiple));
	h_Enum.insertKeyAndValue("NominatedRowSingle",		new int(CEGUI::MultiColumnList::NominatedRowSingle));
	h_Enum.insertKeyAndValue("NominatedRowMultiple",	new int(CEGUI::MultiColumnList::NominatedRowMultiple));
	// ItemListBase (SortMode)
	h_Enum.insertKeyAndValue("SM_Ascending",	new int(CEGUI::ItemListBase::Ascending));
	h_Enum.insertKeyAndValue("SM_Descending",	new int(CEGUI::ItemListBase::Descending));
	h_Enum.insertKeyAndValue("SM_UserSort",		new int(CEGUI::ItemListBase::UserSort));
}

} // namespace RFSX

#undef SX_IMPL_TYPE
