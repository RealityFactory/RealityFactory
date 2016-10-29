/************************************************************************************//**
 * @file sxCEGUIFontManager.cpp
 * @brief Implementation for RFSX FontManager object.
 * @author Daniel Queteschiner
 * @date November 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUIFontManager.h"
#include "sxCEGUIFont.h"
#include "sxCEGUISize.h"
#include "sxCEGUIIterators.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxFontManager

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxFontManager implementation
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
	return new sxFontIterator();
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxFontManager method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(createFont)
{
	skRValue::RType rtypes[] = {	skRValue::T_String, skRValue::T_String,
									skRValue::T_String, skRValue::T_String };

	if(!CheckParams(1, 4, args, rtypes))
		return false;

	CEGUI::Font *font = NULL;
	CEGUI::String resourceGroup;

	if(args.entries() < 3)
	{
		CEGUI::String filename(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));

		if(args.entries() == 2)
			resourceGroup = reinterpret_cast<const CEGUI::utf8*>(args[1].str().c_str());

		try
		{
			font = CEGUI::FontManager::getSingleton().createFont(filename, resourceGroup);
		}
		catch(CEGUI::AlreadyExistsException CEGUIDeadException(&e))
		{
			return false;
		}
		catch(CEGUI::InvalidRequestException CEGUIDeadException(&e))
		{
			return false;
		}
	}
	else
	{
		CEGUI::String type		(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
		CEGUI::String name		(reinterpret_cast<const CEGUI::utf8*>(args[1].str().c_str()));
		CEGUI::String fontname	(reinterpret_cast<const CEGUI::utf8*>(args[2].str().c_str()));

		if(args.entries() == 4)
			resourceGroup = reinterpret_cast<const CEGUI::utf8*>(args[3].str().c_str());

		try
		{
			font = CEGUI::FontManager::getSingleton().createFont(type, name, fontname, resourceGroup);
		}
		catch(CEGUI::AlreadyExistsException CEGUIDeadException(&e))
		{
			return false;
		}
		catch(CEGUI::InvalidRequestException CEGUIDeadException(&e))
		{
			return false;
		}
	}

	if(!font)
		return false;

	r_val.assignObject(new sxFont(font), true);
	return true;
}


SX_METHOD_IMPL(destroyFont)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() == skRValue::T_String)
	{
		CEGUI::FontManager::getSingleton().destroyFont(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
		return true;
	}

	sxFont *font = IS_FONT(args[0]);
	if(!font)
		return false;

	CEGUI::FontManager::getSingleton().destroyFont(font->Font());
	return true;
}


SX_METHOD_IMPL(destroyAllFonts)
{
	CEGUI::FontManager::getSingleton().destroyAllFonts();
	return true;
}


SX_METHOD_IMPL(isFontPresent)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	r_val = CEGUI::FontManager::getSingleton().isFontPresent(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	return true;
}


SX_METHOD_IMPL(getFont)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	CEGUI::Font *font = CEGUI::FontManager::getSingleton().getFont(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));

	r_val.assignObject(new sxFont(font), true);
	return true;
}


SX_METHOD_IMPL(notifyScreenResolution)
{
	if(args.entries() != 1)
		return false;

	sxSize *sz = IS_SIZE(args[0]);
	if(!sz)
		return false;

	CEGUI::FontManager::getSingleton().notifyScreenResolution(*(sz->Size()));
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxFontManager initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXFONTMANAGER_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(createFont);
	SX_ADD_METHOD(destroyFont);
	SX_ADD_METHOD(destroyAllFonts);
	SX_ADD_METHOD(isFontPresent);
	SX_ADD_METHOD(getFont);
	SX_ADD_METHOD(notifyScreenResolution);
}

} // namespace RFSX

#undef SX_IMPL_TYPE
