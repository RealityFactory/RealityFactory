/************************************************************************************//**
 * @file sxCEGUIWindowManager.cpp
 * @brief Implementation for RFSX WindowManager object.
 * @author Daniel Queteschiner
 * @date December 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUIWindowManager.h"
#include "sxCEGUIWindow.h"
#include "sxCEGUIIterators.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxWindowManager

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxWindowManager implementation
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
	return new sxWindowIterator();
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxWindowManager method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(createWindow)
{
	skRValue::RType rtypes[] = { skRValue::T_String, skRValue::T_String };

	if(!CheckParams(1, 2, args, rtypes))
		return false;

	CEGUI::String type(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	CEGUI::String name;

	if(args.entries() == 2)
		name = reinterpret_cast<const CEGUI::utf8*>(args[1].str().c_str());

	try
	{
		CEGUI::Window *win = CEGUI::WindowManager::getSingletonPtr()->createWindow(type, name);
		r_val.assignObject(new sxWindow(win), true);
	}
	catch(CEGUI::AlreadyExistsException CEGUIDeadException(&e))
	{
		return false;
	}
	catch(CEGUI::InvalidRequestException CEGUIDeadException(&e))
	{
		return false;
	}

	return true;
}


SX_METHOD_IMPL(loadWindowLayout)
{
	if(args.entries() < 1 || args.entries() > 3)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	CEGUI::String type(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));


	//Window* loadWindowLayout(string filename, bool generateRandomPrefix);

	if(args.entries() == 2)
	{
		if(args[1].type() == skRValue::T_Bool)
		{
			try
			{
				CEGUI::Window *win = CEGUI::WindowManager::getSingletonPtr()->loadWindowLayout(type, args[1].boolValue());
				r_val.assignObject(new sxWindow(win), true);
			}
			catch(CEGUI::Exception CEGUIDeadException(&e))
			{
				return false;
			}
			catch(std::exception& e)
			{
				char errorBuffer[512];
				snprintf(errorBuffer, 512, "Exception of type 'std::exception' was thrown by function 'loadWindowLayout'\nMessage: %s", e.what());
				CEGUI::Logger::getSingleton().logEvent(errorBuffer, CEGUI::Errors);
				return false;
			}
			catch(...)
			{
				CEGUI::Logger::getSingletonPtr()->logEvent("Unknown exception thrown by function 'loadWindowLayout'", CEGUI::Errors);
				return false;
			}

			return true;
		}
	}


	//Window* loadWindowLayout(string filename, string name_prefix="", string resourcegroup="");

	CEGUI::String name_prefix;
	CEGUI::String resourcegroup;

	if(args.entries() == 2)
	{
		if(args[1].type() != skRValue::T_String)
			return false;

		CEGUI::String name_prefix = reinterpret_cast<const CEGUI::utf8*>(args[1].str().c_str());
	}

	if(args.entries() == 3)
	{
		if(args[2].type() != skRValue::T_String)
			return false;

		CEGUI::String resourcegroup = reinterpret_cast<const CEGUI::utf8*>(args[2].str().c_str());
	}


	try
	{
		CEGUI::Window *win = CEGUI::WindowManager::getSingletonPtr()->loadWindowLayout(type, name_prefix, resourcegroup);
		r_val.assignObject(new sxWindow(win), true);
	}
	catch(CEGUI::Exception CEGUIDeadException(&e))
	{
		return false;
	}
	catch(std::exception& e)
	{
		char errorBuffer[512];
		snprintf(errorBuffer, 512, "Exception of type 'std::exception' was thrown by function 'loadWindowLayout'\nMessage: %s", e.what());
		CEGUI::Logger::getSingleton().logEvent(errorBuffer, CEGUI::Errors);
		return false;
	}
	catch(...)
	{
		CEGUI::Logger::getSingletonPtr()->logEvent("Unknown exception thrown by function 'loadWindowLayout'", CEGUI::Errors);
		return false;
	}

	return true;
}


SX_METHOD_IMPL(destroyWindow)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() == skRValue::T_String)
	{
		CEGUI::WindowManager::getSingletonPtr()->destroyWindow(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
		return true;
	}

	sxWindow *win = IS_WINDOW(args[0]);
	if(!win)
		return false;

	CEGUI::WindowManager::getSingletonPtr()->destroyWindow(win->Window());
	return true;
}


SX_METHOD_IMPL(destroyAllWindows)
{
	CEGUI::WindowManager::getSingletonPtr()->destroyAllWindows();
	return true;
}


SX_METHOD_IMPL(getWindow)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	CEGUI::String name(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));

	try
	{
		CEGUI::Window *win = CEGUI::WindowManager::getSingletonPtr()->getWindow(name);
		r_val.assignObject(new sxWindow(win), true);
	}
	catch(CEGUI::UnknownObjectException CEGUIDeadException(&e))
	{
		return false;
	}

	return true;
}


SX_METHOD_IMPL(isWindowPresent)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	CEGUI::String name(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));

	r_val = CEGUI::WindowManager::getSingletonPtr()->isWindowPresent(name);
	return true;
}


SX_METHOD_IMPL(isDeadPoolEmpty)
{
	r_val = CEGUI::WindowManager::getSingletonPtr()->isDeadPoolEmpty();
	return true;
}


SX_METHOD_IMPL(cleanDeadPool)
{
	CEGUI::WindowManager::getSingletonPtr()->cleanDeadPool();
	return true;
}


SX_METHOD_IMPL(renameWindow)
{
	if(args.entries() != 2)
		return false;

	if(args[1].type() != skRValue::T_String)
		return false;

	CEGUI::String new_name(reinterpret_cast<const CEGUI::utf8*>(args[1].str().c_str()));

	if(args[0].type() == skRValue::T_String)
	{
		CEGUI::String window(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
		CEGUI::WindowManager::getSingletonPtr()->renameWindow(window, new_name);
		return true;
	}

	sxWindow *win = IS_WINDOW(args[0]);
	if(!win)
		return false;

	CEGUI::WindowManager::getSingletonPtr()->renameWindow(win->Window(), new_name);
	return true;
}


SX_METHOD_IMPL(setDefaultResourceGroup)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	CEGUI::String resourceGroup(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));

	CEGUI::WindowManager::getSingletonPtr()->setDefaultResourceGroup(resourceGroup);
	return true;
}


SX_METHOD_IMPL(getDefaultResourceGroup)
{
	r_val = skString(CEGUI::WindowManager::getSingletonPtr()->getDefaultResourceGroup().c_str());
	return true;
}


SX_METHOD_IMPL(lock)
{
	CEGUI::WindowManager::getSingletonPtr()->lock();
	return true;
}


SX_METHOD_IMPL(unlock)
{
	CEGUI::WindowManager::getSingletonPtr()->unlock();
	return true;
}


SX_METHOD_IMPL(isLocked)
{
	r_val = CEGUI::WindowManager::getSingletonPtr()->isLocked();
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxWindowManager initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXWINDOWMANAGER_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(createWindow);
	SX_ADD_METHOD(loadWindowLayout);
	SX_ADD_METHOD(destroyWindow);
	SX_ADD_METHOD(destroyAllWindows);
	SX_ADD_METHOD(getWindow);
	SX_ADD_METHOD(isWindowPresent);
	SX_ADD_METHOD(isDeadPoolEmpty);
	SX_ADD_METHOD(cleanDeadPool);
	SX_ADD_METHOD(renameWindow);
	SX_ADD_METHOD(setDefaultResourceGroup);
	SX_ADD_METHOD(getDefaultResourceGroup);
	SX_ADD_METHOD(lock);
	SX_ADD_METHOD(unlock);
	SX_ADD_METHOD(isLocked);
}

} // namespace RFSX

#undef SX_IMPL_TYPE // And that's it in a nutshell
