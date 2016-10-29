/************************************************************************************//**
 * @file sxCEGUIImagesetManager.cpp
 * @brief Implementation for RFSX ImagesetManager object.
 * @author Daniel Queteschiner
 * @date December 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUIImagesetManager.h"
#include "sxCEGUIImageset.h"
#include "sxCEGUITexture.h"
#include "sxCEGUISize.h"
#include "sxCEGUIIterators.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxImagesetManager

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxImagesetManager implementation
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
	return new sxImagesetIterator();
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxImagesetManager method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(createImageset)
{
	//	Imageset* createImageset(string name, Texture* texture);
	//	Imageset* createImageset(string filename, string resourcegroup="");
	if(args.entries() < 1 || args.entries() > 2)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	CEGUI::Imageset* imageset = NULL;

	if(args.entries() == 1)
	{
		try
		{
			imageset = CEGUI::ImagesetManager::getSingleton().createImageset(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
		}
		catch(CEGUI::Exception CEGUIDeadException(&e))
		{
			return false;
		}
	}
	else
	{
		if(args[1].type() == skRValue::T_String)
		{
			try
			{
				imageset = CEGUI::ImagesetManager::getSingleton().createImageset(	reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()),
																					reinterpret_cast<const CEGUI::utf8*>(args[1].str().c_str()));
			}
			catch(CEGUI::Exception CEGUIDeadException(&e))
			{
				return false;
			}
		}
		else
		{
			sxTexture *texture = IS_TEXTURE(args[1]);
			if(!texture)
				return false;

			try
			{
				imageset = CEGUI::ImagesetManager::getSingleton().createImageset(	reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()),
																					texture->Texture());
			}
			catch(CEGUI::Exception CEGUIDeadException(&e))
			{
				return false;
			}
		}
	}

	if(!imageset)
		return false;

	r_val.assignObject(new sxImageset(imageset), true);

	return true;
}


SX_METHOD_IMPL(createImagesetFromImageFile)
{
	//Imageset* createImagesetFromImageFile(string name, string filename, string resourcegroup="");
	skRValue::RType rtypes[] = {	skRValue::T_String,
									skRValue::T_String,
									skRValue::T_String }; // parameter type list

	if(!CheckParams(2, 3, args, rtypes))
		return false;

	CEGUI::String name		(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	CEGUI::String filename	(reinterpret_cast<const CEGUI::utf8*>(args[1].str().c_str()));
	CEGUI::String resourcegroup;

	if(args.entries() > 2)
		resourcegroup = reinterpret_cast<const CEGUI::utf8*>(args[2].str().c_str());

	CEGUI::Imageset* imageset = NULL;

	try
	{
		imageset = CEGUI::ImagesetManager::getSingleton().createImagesetFromImageFile(name, filename, resourcegroup);
	}
	catch(CEGUI::Exception CEGUIDeadException(&e))
	{
		return false;
	}

	if(!imageset)
		return false;

	r_val.assignObject(new sxImageset(imageset), true);

	return true;
}


SX_METHOD_IMPL(destroyImageset)
{
	//void destroyImageset(Imageset* imageset);
	//void destroyImageset(string name);
	if(args.entries() != 1)
		return false;

	if(args[0].type() == skRValue::T_String)
	{
		CEGUI::ImagesetManager::getSingleton().destroyImageset(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	}
	else
	{
		sxImageset *imageset = IS_IMAGESET(args[0]);
		if(!imageset)
			return false;

		CEGUI::ImagesetManager::getSingleton().destroyImageset(imageset->Imageset());
	}

	return true;
}



SX_METHOD_IMPL(destroyAllImagesets)
{
	//void destroyAllImagesets();
	CEGUI::ImagesetManager::getSingleton().destroyAllImagesets();
	return true;
}


SX_METHOD_IMPL(getImageset)
{
	//Imageset* getImageset(string name) const;
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	CEGUI::Imageset *imageset = NULL;

	try
	{
		imageset = CEGUI::ImagesetManager::getSingleton().getImageset(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	}
	catch(CEGUI::Exception CEGUIDeadException(&e))
	{
		return false;
	}

	if(!imageset)
		return false;

	r_val.assignObject(new sxImageset(imageset), true);

	return true;
}


SX_METHOD_IMPL(isImagesetPresent)
{
	//bool isImagesetPresent(string name) const;
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	r_val = CEGUI::ImagesetManager::getSingleton().isImagesetPresent(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));

	return true;
}


SX_METHOD_IMPL(notifyScreenResolution)
{
	//void notifyScreenResolution(const Size& size);
	if(args.entries() != 1)
		return false;

	sxSize *sz = IS_SIZE(args[0]);
	if(!sz)
		return false;

	CEGUI::ImagesetManager::getSingleton().notifyScreenResolution(sz->SizeConstRef());
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxImagesetManager initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXIMAGESETMANAGER_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(createImageset);
	SX_ADD_METHOD(createImagesetFromImageFile);
	SX_ADD_METHOD(destroyImageset);
	SX_ADD_METHOD(destroyAllImagesets);
	SX_ADD_METHOD(getImageset);
	SX_ADD_METHOD(isImagesetPresent);
	SX_ADD_METHOD(notifyScreenResolution);
}

} // namespace RFSX

#undef SX_IMPL_TYPE
