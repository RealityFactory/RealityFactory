/************************************************************************************//**
 * @file sxCEGUIIterators.cpp
 * @brief Implementation for RFSX Iterator objects.
 * @author Daniel Queteschiner
 * @date December 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUIIterators.h"
#include "sxCEGUIImageset.h"
#include "sxCEGUIImage.h"
#include "sxCEGUIProperty.h"
#include "sxCEGUIEvent.h"
#include "sxCEGUIFont.h"
#include "sxCEGUIWindow.h"
#include "sxCEGUIScheme.h"


namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////

sxFontIterator::sxFontIterator()
	: m_Iter(CEGUI::FontManager::getSingleton().getIterator())
{
}

sxFontIterator::~sxFontIterator()
{
}

bool sxFontIterator::next(skRValue& value)
{
	if(m_Iter.isAtEnd())
		return false;

	value.assignObject(new sxFont(m_Iter.getCurrentValue()), true);
	++m_Iter;

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////

sxImagesetIterator::sxImagesetIterator()
	: m_Iter(CEGUI::ImagesetManager::getSingleton().getIterator())
{
}

sxImagesetIterator::~sxImagesetIterator()
{
}

bool sxImagesetIterator::next(skRValue& value)
{
	if(m_Iter.isAtEnd())
		return false;

	value.assignObject(new sxImageset(m_Iter.getCurrentValue()), true);
	++m_Iter;

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////

sxImageIterator::sxImageIterator(CEGUI::Imageset* imageset)
	: m_Iter(imageset->getIterator())
{
}

sxImageIterator::~sxImageIterator()
{
}

bool sxImageIterator::next(skRValue& value)
{
	if(m_Iter.isAtEnd())
		return false;

	value.assignObject(new sxImage(&m_Iter.getCurrentValue()), true);
	++m_Iter;

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////

sxPropertyIterator::sxPropertyIterator(CEGUI::PropertySet* propertySet)
	: m_Iter(propertySet->getIterator())
{
}

sxPropertyIterator::~sxPropertyIterator()
{
}

bool sxPropertyIterator::next(skRValue& value)
{
	if(m_Iter.isAtEnd())
		return false;

	value.assignObject(new sxProperty(m_Iter.getCurrentValue()), true);
	++m_Iter;

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////

sxEventIterator::sxEventIterator(CEGUI::EventSet* eventSet)
	: m_Iter(eventSet->getIterator())
{
}

sxEventIterator::~sxEventIterator()
{
}

bool sxEventIterator::next(skRValue& value)
{
	if(m_Iter.isAtEnd())
		return false;

	value.assignObject(new sxEvent(m_Iter.getCurrentValue()), true);
	++m_Iter;

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////

sxWindowIterator::sxWindowIterator()
	: m_Iter(CEGUI::WindowManager::getSingleton().getIterator())
{
}

sxWindowIterator::~sxWindowIterator()
{
}

bool sxWindowIterator::next(skRValue& value)
{
	if(m_Iter.isAtEnd())
		return false;

	value.assignObject(new sxWindow(m_Iter.getCurrentValue()), true);
	++m_Iter;

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////

sxSchemeIterator::sxSchemeIterator()
	: m_Iter(CEGUI::SchemeManager::getSingleton().getIterator())
{
}

sxSchemeIterator::~sxSchemeIterator()
{
}

bool sxSchemeIterator::next(skRValue& value)
{
	if(m_Iter.isAtEnd())
		return false;

	value.assignObject(new sxScheme(m_Iter.getCurrentValue()), true);
	++m_Iter;

	return true;
}

} // namespace RFSX
