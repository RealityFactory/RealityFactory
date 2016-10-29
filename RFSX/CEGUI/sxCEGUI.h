/************************************************************************************//**
 * @file sxCEGUI.h
 * @brief Class declaration for RFSX global GUI object.
 * @author Daniel Queteschiner
 * @date November 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_CEGUI_H_
#define _SX_CEGUI_H_

#include "RFSX.h"
#include "sxCEGUIEventArgs.h"

// sxGUI class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		sxMouseCursorEventArgs		toMouseCursorEventArgs(sxEventArgs args);
		sxWindowEventArgs			toWindowEventArgs(sxEventArgs args);
		sxActivationEventArgs		toActivationEventArgs(sxEventArgs args);
		sxHeaderSequenceEventArgs	toHeaderSequenceEventArgs(sxEventArgs args);
		sxMouseEventArgs			toMouseEventArgs(sxEventArgs args);
		sxKeyEventArgs				toKeyEventArgs(sxEventArgs args);
		sxDragDropEventArgs			toDragDropEventArgs(sxEventArgs args);
		sxTreeEventArgs				toTreeEventArgs(sxEventArgs args);

		sxButtonBase			toButtonBase(sxWindow wnd);
		sxCheckbox				toCheckbox(sxWindow wnd);
		sxCombobox				toCombobox(sxWindow wnd);
		sxComboDropList			toComboDropList(sxWindow wnd);
		sxDragContainer			toDragContainer(sxWindow wnd);
		sxEditbox				toEditbox(sxWindow wnd);
		sxFrameWindow			toFrameWindow(sxWindow wnd);
		sxGUISheet				toGUISheet(sxWindow wnd);
		sxItemEntry				toItemEntry(sxWindow wnd);
		sxItemListBase			toItemListBase(sxWindow wnd);
		sxItemListbox			toItemListbox(sxWindow wnd);
		sxListbox				toListbox(sxWindow wnd);
		sxListHeader			toListHeader(sxWindow wnd);
		sxListHeaderSegment		toListHeaderSegment(sxWindow wnd);
		sxMenubar				toMenubar(sxWindow wnd);
		sxMenuBase				toMenuBase(sxWindow wnd);
		sxMenuItem				toMenuItem(sxWindow wnd);
		sxMultiColumnList		toMultiColumnList(sxWindow wnd);
		sxMultiLineEditbox		toMultiLineEditbox(sxWindow wnd);
		sxPopupMenu				toPopupMenu(sxWindow wnd);
		sxProgressBar			toProgressBar(sxWindow wnd);
		sxPushButton			toPushButton(sxWindow wnd);
		sxRadioButton			toRadioButton(sxWindow wnd);
		sxScrollablePane		toScrollablePane(sxWindow wnd);
		sxScrollbar				toScrollbar(sxWindow wnd);
		sxScrolledContainer		toScrolledContainer(sxWindow wnd);
		sxScrolledItemListBase	toScrolledItemListBase(sxWindow wnd);
		sxSlider				toSlider(sxWindow wnd);
		sxSpinner				toSpinner(sxWindow wnd);
		sxTabButton				toTabButton(sxWindow wnd);
		sxTabControl			toTabControl(sxWindow wnd);
		sxThumb					toThumb(sxWindow wnd);
		sxTooltip				toTooltip(sxWindow wnd);

	Fields:
		sxFontManager		FontManager;(RO)
		sxImagesetManager	ImagesetManager;(RO)
		sxGlobalEventSet	GlobalEventSet;(RO)
		sxMouseCursor		MouseCursor;(RO)
		sxSchemeManager		SchemeManager;(RO)
		sxSystem			System;(RO)
		sxWindowManager		WindowManager;(RO)
		sxPropertyHelper	PropertyHelper;(RO)
		sxLogger			Logger;(RO)
		sxKey				Key;(RO)
		sxCoordConverter	CoordConverter;(RO)

	TODO:
		ScriptWindowHelper ?
		WindowFactoryManager
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxGUI

#define SXGUI_METHODS 41

namespace RFSX
{
class sxFontManager;
class sxImagesetManager;
class sxGlobalEventSet;
class sxMouseCursor;
class sxSchemeManager;
class sxSystem;
class sxWindowManager;
class sxPropertyHelper;
class sxLogger;
class sxKey;
class sxCoordConverter;

class SX_IMPL_TYPE: public skExecutable
{
public:
	SX_IMPL_TYPE();
	virtual ~SX_IMPL_TYPE();
	static void InitMHT();

	virtual bool getValue(const skString& name ,const skString& att, skRValue& val);
	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);
	virtual int executableType() const { return RFSXG_GUI; }

protected:
	SX_METHOD_DECL(toMouseCursorEventArgs);
	SX_METHOD_DECL(toWindowEventArgs);
	SX_METHOD_DECL(toActivationEventArgs);
	SX_METHOD_DECL(toHeaderSequenceEventArgs);
	SX_METHOD_DECL(toMouseEventArgs);
	SX_METHOD_DECL(toKeyEventArgs);
	SX_METHOD_DECL(toDragDropEventArgs);
	SX_METHOD_DECL(toTreeEventArgs);

	SX_METHOD_DECL(toButtonBase);
	SX_METHOD_DECL(toCheckbox);
	SX_METHOD_DECL(toCombobox);
	SX_METHOD_DECL(toComboDropList);
	SX_METHOD_DECL(toDragContainer);
	SX_METHOD_DECL(toEditbox);
	SX_METHOD_DECL(toFrameWindow);
	SX_METHOD_DECL(toGUISheet);
	SX_METHOD_DECL(toItemEntry);
	SX_METHOD_DECL(toItemListBase);
	SX_METHOD_DECL(toItemListbox);
	SX_METHOD_DECL(toListbox);
	SX_METHOD_DECL(toListHeader);
	SX_METHOD_DECL(toListHeaderSegment);
	SX_METHOD_DECL(toMenubar);
	SX_METHOD_DECL(toMenuBase);
	SX_METHOD_DECL(toMenuItem);
	SX_METHOD_DECL(toMultiColumnList);
	SX_METHOD_DECL(toMultiLineEditbox);
	SX_METHOD_DECL(toPopupMenu);
	SX_METHOD_DECL(toProgressBar);
	SX_METHOD_DECL(toPushButton);
	SX_METHOD_DECL(toRadioButton);
	SX_METHOD_DECL(toScrollablePane);
	SX_METHOD_DECL(toScrollbar);
	SX_METHOD_DECL(toScrolledContainer);
	SX_METHOD_DECL(toScrolledItemListBase);
	SX_METHOD_DECL(toSlider);
	SX_METHOD_DECL(toSpinner);
	SX_METHOD_DECL(toTabButton);
	SX_METHOD_DECL(toTabControl);
	SX_METHOD_DECL(toThumb);
	SX_METHOD_DECL(toTooltip);

private:
	sxFontManager*		m_pFontManager;
	sxImagesetManager*	m_pImagesetManager;
	sxGlobalEventSet*	m_pGlobalEventSet;
	sxMouseCursor*		m_pMouseCursor;
	sxSchemeManager*	m_pSchemeManager;
	sxSystem*			m_pSystem;
	sxWindowManager*	m_pWindowManager;
	sxPropertyHelper*	m_pPropertyHelper;
	sxLogger*			m_pLogger;
	sxKey*				m_pKey;
	sxCoordConverter*	m_pCoordConverter;


	void InitEnumHT();
	skTSHashTable<int> h_Enum;

	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif	// _SX_CEGUI_H_
