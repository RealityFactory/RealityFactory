/************************************************************************************//**
 * @file sxCEGUIKey.cpp
 * @brief Implementation for RFSX Key object.
 * @author Daniel Queteschiner
 * @date December 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUIKey.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxKey

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxKey implementation
//////////////////////////////////////////////////////////////////////////////////////////

SX_IMPL_TYPE::SX_IMPL_TYPE()
{
	InitKeyHT();
}


SX_IMPL_TYPE::~SX_IMPL_TYPE()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxKey fields
//////////////////////////////////////////////////////////////////////////////////////////

bool SX_IMPL_TYPE::getValue(const skString& name, const skString& att, skRValue& val)
{
	int *intValue = h_Key.value(name);
	if(!intValue)
		return false;

	val = *intValue;

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxKey initialization
//////////////////////////////////////////////////////////////////////////////////////////

void SX_IMPL_TYPE::InitKeyHT()
{
	h_Key.insertKeyAndValue("Escape",		new int(CEGUI::Key::Escape));
	h_Key.insertKeyAndValue("One",			new int(CEGUI::Key::One));
	h_Key.insertKeyAndValue("Two",			new int(CEGUI::Key::Two));
	h_Key.insertKeyAndValue("Three",		new int(CEGUI::Key::Three));
	h_Key.insertKeyAndValue("Four",			new int(CEGUI::Key::Four));
	h_Key.insertKeyAndValue("Five",			new int(CEGUI::Key::Five));
	h_Key.insertKeyAndValue("Six",			new int(CEGUI::Key::Six));
	h_Key.insertKeyAndValue("Seven",		new int(CEGUI::Key::Seven));
	h_Key.insertKeyAndValue("Eight",		new int(CEGUI::Key::Eight));
	h_Key.insertKeyAndValue("Nine",			new int(CEGUI::Key::Nine));
	h_Key.insertKeyAndValue("Zero",			new int(CEGUI::Key::Zero));
	h_Key.insertKeyAndValue("Minus",		new int(CEGUI::Key::Minus));		/* - on main keyboard */
	h_Key.insertKeyAndValue("Equals",		new int(CEGUI::Key::Equals));
	h_Key.insertKeyAndValue("Backspace",	new int(CEGUI::Key::Backspace));	/* backspace */
	h_Key.insertKeyAndValue("Tab",			new int(CEGUI::Key::Tab));
	h_Key.insertKeyAndValue("Q",			new int(CEGUI::Key::Q));
	h_Key.insertKeyAndValue("W",			new int(CEGUI::Key::W));
	h_Key.insertKeyAndValue("E",			new int(CEGUI::Key::E));
	h_Key.insertKeyAndValue("R",			new int(CEGUI::Key::R));
	h_Key.insertKeyAndValue("T",			new int(CEGUI::Key::T));
	h_Key.insertKeyAndValue("Y",			new int(CEGUI::Key::Y));
	h_Key.insertKeyAndValue("U",			new int(CEGUI::Key::U));
	h_Key.insertKeyAndValue("I",			new int(CEGUI::Key::I));
	h_Key.insertKeyAndValue("O",			new int(CEGUI::Key::O));
	h_Key.insertKeyAndValue("P",			new int(CEGUI::Key::P));
	h_Key.insertKeyAndValue("LeftBracket",	new int(CEGUI::Key::LeftBracket));
	h_Key.insertKeyAndValue("RightBracket", new int(CEGUI::Key::RightBracket));
	h_Key.insertKeyAndValue("Return",		new int(CEGUI::Key::Return));		/* Enter on main keyboard */
	h_Key.insertKeyAndValue("LeftControl",	new int(CEGUI::Key::LeftControl));
	h_Key.insertKeyAndValue("A",			new int(CEGUI::Key::A));
	h_Key.insertKeyAndValue("S",			new int(CEGUI::Key::S));
	h_Key.insertKeyAndValue("D",			new int(CEGUI::Key::D));
	h_Key.insertKeyAndValue("F",			new int(CEGUI::Key::F));
	h_Key.insertKeyAndValue("G",			new int(CEGUI::Key::G));
	h_Key.insertKeyAndValue("H",			new int(CEGUI::Key::H));
	h_Key.insertKeyAndValue("J",			new int(CEGUI::Key::J));
	h_Key.insertKeyAndValue("K",			new int(CEGUI::Key::K));
	h_Key.insertKeyAndValue("L",			new int(CEGUI::Key::L));
	h_Key.insertKeyAndValue("Semicolon",	new int(CEGUI::Key::Semicolon));
	h_Key.insertKeyAndValue("Apostrophe",	new int(CEGUI::Key::Apostrophe));
	h_Key.insertKeyAndValue("Grave",		new int(CEGUI::Key::Grave));		/* accent grave */
	h_Key.insertKeyAndValue("LeftShift",	new int(CEGUI::Key::LeftShift));
	h_Key.insertKeyAndValue("Backslash",	new int(CEGUI::Key::Backslash));
	h_Key.insertKeyAndValue("Z",			new int(CEGUI::Key::Z));
	h_Key.insertKeyAndValue("X",			new int(CEGUI::Key::X));
	h_Key.insertKeyAndValue("C",			new int(CEGUI::Key::C));
	h_Key.insertKeyAndValue("V",			new int(CEGUI::Key::V));
	h_Key.insertKeyAndValue("B",			new int(CEGUI::Key::B));
	h_Key.insertKeyAndValue("N",			new int(CEGUI::Key::N));
	h_Key.insertKeyAndValue("M",			new int(CEGUI::Key::M));
	h_Key.insertKeyAndValue("Comma",		new int(CEGUI::Key::Comma));
	h_Key.insertKeyAndValue("Period",		new int(CEGUI::Key::Period));		/* . on main keyboard */
	h_Key.insertKeyAndValue("Slash",		new int(CEGUI::Key::Slash));		/* '/' on main keyboard */
	h_Key.insertKeyAndValue("RightShift",	new int(CEGUI::Key::RightShift));
	h_Key.insertKeyAndValue("Multiply",		new int(CEGUI::Key::Multiply));		/* * on numeric keypad */
	h_Key.insertKeyAndValue("LeftAlt",		new int(CEGUI::Key::LeftAlt));		/* left Alt */
	h_Key.insertKeyAndValue("Space",		new int(CEGUI::Key::Space));
	h_Key.insertKeyAndValue("Capital",		new int(CEGUI::Key::Capital));
	h_Key.insertKeyAndValue("F1",			new int(CEGUI::Key::F1));
	h_Key.insertKeyAndValue("F2",			new int(CEGUI::Key::F2));
	h_Key.insertKeyAndValue("F3",			new int(CEGUI::Key::F3));
	h_Key.insertKeyAndValue("F4",			new int(CEGUI::Key::F4));
	h_Key.insertKeyAndValue("F5",			new int(CEGUI::Key::F5));
	h_Key.insertKeyAndValue("F6",			new int(CEGUI::Key::F6));
	h_Key.insertKeyAndValue("F7",			new int(CEGUI::Key::F7));
	h_Key.insertKeyAndValue("F8",			new int(CEGUI::Key::F8));
	h_Key.insertKeyAndValue("F9",			new int(CEGUI::Key::F9));
	h_Key.insertKeyAndValue("F10",			new int(CEGUI::Key::F10));
	h_Key.insertKeyAndValue("NumLock",		new int(CEGUI::Key::NumLock));
	h_Key.insertKeyAndValue("ScrollLock",	new int(CEGUI::Key::ScrollLock));	/* Scroll Lock */
	h_Key.insertKeyAndValue("Numpad7",		new int(CEGUI::Key::Numpad7));
	h_Key.insertKeyAndValue("Numpad8",		new int(CEGUI::Key::Numpad8));
	h_Key.insertKeyAndValue("Numpad9",		new int(CEGUI::Key::Numpad9));
	h_Key.insertKeyAndValue("Subtract",		new int(CEGUI::Key::Subtract));		/* - on numeric keypad */
	h_Key.insertKeyAndValue("Numpad4",		new int(CEGUI::Key::Numpad4));
	h_Key.insertKeyAndValue("Numpad5",		new int(CEGUI::Key::Numpad5));
	h_Key.insertKeyAndValue("Numpad6",		new int(CEGUI::Key::Numpad6));
	h_Key.insertKeyAndValue("Add",			new int(CEGUI::Key::Add));			/* + on numeric keypad */
	h_Key.insertKeyAndValue("Numpad1",		new int(CEGUI::Key::Numpad1));
	h_Key.insertKeyAndValue("Numpad2",		new int(CEGUI::Key::Numpad2));
	h_Key.insertKeyAndValue("Numpad3",		new int(CEGUI::Key::Numpad3));
	h_Key.insertKeyAndValue("Numpad0",		new int(CEGUI::Key::Numpad0));
	h_Key.insertKeyAndValue("Decimal",		new int(CEGUI::Key::Decimal));		/* . on numeric keypad */
	h_Key.insertKeyAndValue("OEM_102",		new int(CEGUI::Key::OEM_102));		/* < > | on UK/Germany keyboards */
	h_Key.insertKeyAndValue("F11",			new int(CEGUI::Key::F11));
	h_Key.insertKeyAndValue("F12",			new int(CEGUI::Key::F12));
	h_Key.insertKeyAndValue("F13",			new int(CEGUI::Key::F13));			/* (NEC PC98) */
	h_Key.insertKeyAndValue("F14",			new int(CEGUI::Key::F14));			/* (NEC PC98) */
	h_Key.insertKeyAndValue("F15",			new int(CEGUI::Key::F15));			/* (NEC PC98) */
	h_Key.insertKeyAndValue("Kana",			new int(CEGUI::Key::Kana));			/* (Japanese keyboard) */
	h_Key.insertKeyAndValue("ABNT_C1",		new int(CEGUI::Key::ABNT_C1));		/* / ? on Portugese (Brazilian) keyboards */
	h_Key.insertKeyAndValue("Convert",		new int(CEGUI::Key::Convert));		/* (Japanese keyboard) */
	h_Key.insertKeyAndValue("NoConvert",	new int(CEGUI::Key::NoConvert));	/* (Japanese keyboard) */
	h_Key.insertKeyAndValue("Yen",			new int(CEGUI::Key::Yen));			/* (Japanese keyboard) */
	h_Key.insertKeyAndValue("ABNT_C2",		new int(CEGUI::Key::ABNT_C2));		/* Numpad . on Portugese (Brazilian) keyboards */
	h_Key.insertKeyAndValue("NumpadEquals", new int(CEGUI::Key::NumpadEquals));	/* = on numeric keypad (NEC PC98) */
	h_Key.insertKeyAndValue("PrevTrack",	new int(CEGUI::Key::PrevTrack));	/* Previous Track (KC_CIRCUMFLEX on Japanese keyboard) */
	h_Key.insertKeyAndValue("At",			new int(CEGUI::Key::At));			/* (NEC PC98) */
	h_Key.insertKeyAndValue("Colon",		new int(CEGUI::Key::Colon));		/* (NEC PC98) */
	h_Key.insertKeyAndValue("Underline",	new int(CEGUI::Key::Underline));	/* (NEC PC98) */
	h_Key.insertKeyAndValue("Kanji",		new int(CEGUI::Key::Kanji));		/* (Japanese keyboard) */
	h_Key.insertKeyAndValue("Stop",			new int(CEGUI::Key::Stop));			/* (NEC PC98) */
	h_Key.insertKeyAndValue("AX",			new int(CEGUI::Key::AX));			/* (Japan AX) */
	h_Key.insertKeyAndValue("Unlabeled",	new int(CEGUI::Key::Unlabeled));	/* (J3100) */
	h_Key.insertKeyAndValue("NextTrack",	new int(CEGUI::Key::NextTrack));	/* Next Track */
	h_Key.insertKeyAndValue("NumpadEnter",	new int(CEGUI::Key::NumpadEnter));	/* Enter on numeric keypad */
	h_Key.insertKeyAndValue("RightControl", new int(CEGUI::Key::RightControl));
	h_Key.insertKeyAndValue("Mute",			new int(CEGUI::Key::Mute));			/* Mute */
	h_Key.insertKeyAndValue("Calculator",	new int(CEGUI::Key::Calculator));	/* Calculator */
	h_Key.insertKeyAndValue("PlayPause",	new int(CEGUI::Key::PlayPause));	/* Play / Pause */
	h_Key.insertKeyAndValue("MediaStop",	new int(CEGUI::Key::MediaStop));	/* Media Stop */
	h_Key.insertKeyAndValue("VolumeDown",	new int(CEGUI::Key::VolumeDown));	/* Volume - */
	h_Key.insertKeyAndValue("VolumeUp",		new int(CEGUI::Key::VolumeUp));		/* Volume + */
	h_Key.insertKeyAndValue("WebHome",		new int(CEGUI::Key::WebHome));		/* Web home */
	h_Key.insertKeyAndValue("NumpadComma",	new int(CEGUI::Key::NumpadComma));	/* , on numeric keypad (NEC PC98) */
	h_Key.insertKeyAndValue("Divide",		new int(CEGUI::Key::Divide));		/* / on numeric keypad */
	h_Key.insertKeyAndValue("SysRq",		new int(CEGUI::Key::SysRq));
	h_Key.insertKeyAndValue("RightAlt",		new int(CEGUI::Key::RightAlt));		/* right Alt */
	h_Key.insertKeyAndValue("Pause",		new int(CEGUI::Key::Pause));		/* Pause */
	h_Key.insertKeyAndValue("Home",			new int(CEGUI::Key::Home));			/* Home on arrow keypad */
	h_Key.insertKeyAndValue("ArrowUp",		new int(CEGUI::Key::ArrowUp));		/* UpArrow on arrow keypad */
	h_Key.insertKeyAndValue("PageUp",		new int(CEGUI::Key::PageUp));		/* PgUp on arrow keypad */
	h_Key.insertKeyAndValue("ArrowLeft",	new int(CEGUI::Key::ArrowLeft));	/* LeftArrow on arrow keypad */
	h_Key.insertKeyAndValue("ArrowRight",	new int(CEGUI::Key::ArrowRight));	/* RightArrow on arrow keypad */
	h_Key.insertKeyAndValue("End",			new int(CEGUI::Key::End));			/* End on arrow keypad */
	h_Key.insertKeyAndValue("ArrowDown",	new int(CEGUI::Key::ArrowDown));	/* DownArrow on arrow keypad */
	h_Key.insertKeyAndValue("PageDown",		new int(CEGUI::Key::PageDown));		/* PgDn on arrow keypad */
	h_Key.insertKeyAndValue("Insert",		new int(CEGUI::Key::Insert));		/* Insert on arrow keypad */
	h_Key.insertKeyAndValue("Delete",		new int(CEGUI::Key::Delete));		/* Delete on arrow keypad */
	h_Key.insertKeyAndValue("LeftWindows",	new int(CEGUI::Key::LeftWindows));	/* Left Windows key */
	h_Key.insertKeyAndValue("RightWindow",	new int(CEGUI::Key::RightWindow));	/* Right Windows key */
	h_Key.insertKeyAndValue("AppMenu",		new int(CEGUI::Key::AppMenu));		/* AppMenu key */
	h_Key.insertKeyAndValue("Power",		new int(CEGUI::Key::Power));		/* System Power */
	h_Key.insertKeyAndValue("Sleep",		new int(CEGUI::Key::Sleep));		/* System Sleep */
	h_Key.insertKeyAndValue("Wake",			new int(CEGUI::Key::Wake));			/* System Wake */
	h_Key.insertKeyAndValue("WebSearch",	new int(CEGUI::Key::WebSearch));	/* Web Search */
	h_Key.insertKeyAndValue("WebFavorites", new int(CEGUI::Key::WebFavorites));	/* Web Favorites */
	h_Key.insertKeyAndValue("WebRefresh",	new int(CEGUI::Key::WebRefresh));	/* Web Refresh */
	h_Key.insertKeyAndValue("WebStop",		new int(CEGUI::Key::WebStop));		/* Web Stop */
	h_Key.insertKeyAndValue("WebForward",	new int(CEGUI::Key::WebForward));	/* Web Forward */
	h_Key.insertKeyAndValue("WebBack",		new int(CEGUI::Key::WebBack));		/* Web Back */
	h_Key.insertKeyAndValue("MyComputer",	new int(CEGUI::Key::MyComputer));	/* My Computer */
	h_Key.insertKeyAndValue("Mail",			new int(CEGUI::Key::Mail));			/* Mail */
	h_Key.insertKeyAndValue("MediaSelect",	new int(CEGUI::Key::MediaSelect));	/* Media Select */
}

} // namespace RFSX

#undef SX_IMPL_TYPE // And that's it in a nutshell
