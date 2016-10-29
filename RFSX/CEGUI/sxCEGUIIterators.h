/************************************************************************************//**
 * @file sxTemplate.h
 * @brief Class declaration for RFSX Iterator objects
 * @author Daniel Queteschiner
 * @date December 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_ITERATORS_H_
#define _SX_ITERATORS_H_

#include "RFSX.h"
/*
	Available iterators in 'for each' statements:
		sxFontIterator
		sxImagesetIterator
		sxImageIterator
		sxPropertyIterator
		sxEventIterator
		sxWindowIterator
		sxSchemeIterator

	TODO:
		sxWindowFactoryIterator
		sxFalagardMappingIterator
*/

namespace RFSX
{

class sxFontIterator : public skExecutableIterator
{
public:
	sxFontIterator();
	virtual ~sxFontIterator();

	virtual bool next(skRValue& value);

private:
	CEGUI::FontManager::FontIterator m_Iter;
};

//////////////////////////////////////////////////////////////////////////////////////////

class sxImagesetIterator : public skExecutableIterator
{
public:
	sxImagesetIterator();
	virtual ~sxImagesetIterator();

	virtual bool next(skRValue& value);

private:
	CEGUI::ImagesetManager::ImagesetIterator m_Iter;
};

//////////////////////////////////////////////////////////////////////////////////////////

class sxImageIterator : public skExecutableIterator
{
public:
	sxImageIterator(CEGUI::Imageset* imageset);
	virtual ~sxImageIterator();

	virtual bool next(skRValue& value);

private:
	CEGUI::Imageset::ImageIterator m_Iter;
};

//////////////////////////////////////////////////////////////////////////////////////////

class sxPropertyIterator : public skExecutableIterator
{
public:
	sxPropertyIterator(CEGUI::PropertySet* propertySet);
	virtual ~sxPropertyIterator();

	virtual bool next(skRValue& value);

private:
	CEGUI::PropertySet::Iterator m_Iter;
};

//////////////////////////////////////////////////////////////////////////////////////////

class sxEventIterator : public skExecutableIterator
{
public:
	sxEventIterator(CEGUI::EventSet* eventSet);
	virtual ~sxEventIterator();

	virtual bool next(skRValue& value);

private:
	CEGUI::EventSet::Iterator m_Iter;
};

//////////////////////////////////////////////////////////////////////////////////////////

class sxWindowIterator : public skExecutableIterator
{
public:
	sxWindowIterator();
	virtual ~sxWindowIterator();

	virtual bool next(skRValue& value);

private:
	CEGUI::WindowManager::WindowIterator m_Iter;
};

//////////////////////////////////////////////////////////////////////////////////////////

class sxSchemeIterator : public skExecutableIterator
{
public:
	sxSchemeIterator();
	virtual ~sxSchemeIterator();

	virtual bool next(skRValue& value);

private:
	CEGUI::SchemeManager::SchemeIterator m_Iter;
};

} // namespace RFSX

#endif // _SX_ITERATORS_H_
