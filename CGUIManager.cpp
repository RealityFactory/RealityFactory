/************************************************************************************//**
 * @file CGUIManager.cpp
 * @brief Implementation of GUI Manager
 * @author Daniel Queteschiner
 * @date July 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include <CEGUI.h>
#include <OIS.h>
#include "Simkin\\skInterpreter.h"
#include "Simkin\\skTreeNodeObject.h"
#include "CGUIManager.h"
#include "CScriptManager.h"
#include "IniFile.h"


CGUIManager::CGUIManager(void) :
	m_System(NULL),
	m_Renderer(NULL),
	m_ResourceProvider(NULL),
	m_ScriptModule(NULL)
{
}


CGUIManager::CGUIManager(
	geEngine *engine,
	int width,
	int height,
	geWorld *world,
	geVFile *vfile,
	const std::string& logfile) :
	m_System(NULL),
	m_Renderer(NULL),
	m_ResourceProvider(NULL),
	m_ScriptModule(NULL)
{
	Create(engine, width, height, world, vfile, logfile);
}


CGUIManager::~CGUIManager(void)
{
	Destroy();
}


void CGUIManager::Create(
	geEngine *engine,
	int width,
	int height,
	geWorld *world,
	geVFile *vfile,
	const std::string& logfile)
{
	Destroy();

	m_Renderer = new CEGUI::Genesis3DRenderer(engine, width, height, world);
	m_ResourceProvider = new CEGUI::Genesis3DResourceProvider(vfile);
	m_ScriptModule = new CEGUI::SimkinScriptModule(&ScriptManager::Interpreter);

	std::string schemes("CEGUI\\schemes\\");
	std::string imagesets("CEGUI\\imagesets\\");
	std::string fonts("CEGUI\\fonts\\");
	std::string layouts("CEGUI\\layouts\\");
	std::string looknfeels("CEGUI\\looknfeel\\");
	std::string saves("saves\\");
	std::string configFile;
	std::string xmlParser;
	std::string tooltip;
	std::string mousecursor;

	CIniFile iniFile("internal_config.ini");

	if(!iniFile.ReadFile())
	{
		CEGUI::Logger::getSingleton().logEvent("Failed to open internal_config.ini file");
	}
	else
	{
		std::string	value;
		value = iniFile.GetValue("CEGUI", "SchemeDirectory");
		if(!value.empty())	schemes = value + "\\";

		value = iniFile.GetValue("CEGUI", "ImagesetDirectory");
		if(!value.empty())	imagesets = value + "\\";

		value = iniFile.GetValue("CEGUI", "FontDirectory");
		if(!value.empty())	fonts = value + "\\";

		value = iniFile.GetValue("CEGUI", "LayoutDirectory");
		if(!value.empty())	layouts = value + "\\";

		value = iniFile.GetValue("CEGUI", "LookNFeelDirectory");
		if(!value.empty())	looknfeels = value + "\\";

		xmlParser = iniFile.GetValue("CEGUI", "DefaultXMLParser");

		configFile = iniFile.GetValue("CEGUI", "ConfigDirectory") +
			"\\" + iniFile.GetValue("CEGUI", "ConfigFile");

		tooltip = iniFile.GetValue("CEGUI", "DefaultTooltip");
		mousecursor = iniFile.GetValue("CEGUI", "DefaultMouseCursor");
	}

	SetResourceGroupDirectory("schemes",	schemes);
	SetResourceGroupDirectory("imagesets",	imagesets);
	SetResourceGroupDirectory("fonts",		fonts);
	SetResourceGroupDirectory("layouts",	layouts);
	SetResourceGroupDirectory("looknfeels",	looknfeels);
	SetResourceGroupDirectory("saves",		saves);
	SetResourceGroupDirectory("scripts",	"scripts\\");

	CEGUI::Scheme::setDefaultResourceGroup("schemes");
	CEGUI::Imageset::setDefaultResourceGroup("imagesets");
	CEGUI::Font::setDefaultResourceGroup("fonts");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeels");
	CEGUI::WindowManager::setDefaultResourceGroup("layouts");
	CEGUI::ScriptModule::setDefaultResourceGroup("scripts");

	if(!xmlParser.empty())
		CEGUI::System::setDefaultXMLParserName((const CEGUI::utf8*)xmlParser.c_str());

	m_System = new CEGUI::System(	m_Renderer,
									m_ResourceProvider,
									0,
									m_ScriptModule,
									configFile,
									logfile);

	if(!tooltip.empty())
		CEGUI::System::getSingleton().setDefaultTooltip(tooltip);

	if(!mousecursor.empty())
		CEGUI::System::getSingleton().setDefaultMouseCursor(
			CEGUI::PropertyHelper::stringToImage((const CEGUI::utf8*)mousecursor.c_str()));
}


void CGUIManager::Destroy()
{
	delete m_System;
	m_System = NULL;

	delete m_ScriptModule;
	m_ScriptModule = NULL;

	delete m_ResourceProvider;
	m_ResourceProvider = NULL;

	delete m_Renderer;
	m_Renderer = NULL;
}


void CGUIManager::SetResourceGroupDirectory(const std::string& resourceGroup, const std::string& directory)
{
	if(m_ResourceProvider)
	{
		m_ResourceProvider->setResourceGroupDirectory(resourceGroup, directory);
	}
}


void CGUIManager::LoadScheme(const std::string& scheme_filename)
{
	std::vector<std::string>::iterator iter = m_LoadedSchemeFiles.begin();
	for(; iter!=m_LoadedSchemeFiles.end(); ++iter)
		if(*iter == scheme_filename)
			return;

	std::string scheme(scheme_filename);

	size_t found = scheme.rfind(".scheme");
	if(found != std::string::npos)
	{
		scheme.erase(found);
	}

	// assume that scheme name is same as scheme_filename without .scheme file extension
	if(!CEGUI::SchemeManager::getSingleton().isSchemePresent(scheme))
	{
		CEGUI::SchemeManager::getSingleton().loadScheme(scheme_filename);
	}

	m_LoadedSchemeFiles.push_back(scheme_filename);
}


void CGUIManager::SetDefaultMouseCursor(const std::string& imageset, const std::string& imagename)
{
	CEGUI::System::getSingleton().setDefaultMouseCursor(imageset, imagename);
}


void CGUIManager::SetDefaultFont(const std::string& ttffont)
{
	if(!CEGUI::FontManager::getSingleton().isFontPresent(ttffont))
	{
		CEGUI::FontManager::getSingleton().createFont(ttffont);
	}

	CEGUI::System::getSingleton().setDefaultFont(ttffont);
}


void CGUIManager::SetDefaultTooltip(const std::string& tooltip)
{
	CEGUI::System::getSingleton().setDefaultTooltip(tooltip);
}


CEGUI::Window* CGUIManager::SetSheet(CEGUI::Window *sheet)
{
	return CEGUI::System::getSingleton().setGUISheet(sheet);
}


CEGUI::Window* CGUIManager::GetSheet()
{
	return CEGUI::System::getSingleton().getGUISheet();
}


void CGUIManager::Render()
{
	if(m_System)
		CEGUI::System::getSingleton().renderGUI();
}


void CGUIManager::CleanDeadWindowPool()
{
	CEGUI::WindowManager::getSingletonPtr()->cleanDeadPool();
}


CEGUI::Window* CGUIManager::LoadWindowLayout(const std::string& layout)
{
	return CEGUI::WindowManager::getSingletonPtr()->loadWindowLayout(layout);
}


CEGUI::Imageset* CGUIManager::CreateImagesetFromImageFile(const std::string& imageset, const std::string& imagefile, const std::string& resourceGroup)
{
	if(CEGUI::ImagesetManager::getSingleton().isImagesetPresent(imageset))
		return CEGUI::ImagesetManager::getSingleton().getImageset(imageset);

	return CEGUI::ImagesetManager::getSingleton().createImagesetFromImageFile(imageset, imagefile, resourceGroup);
}


void CGUIManager::DestroyImageset(const std::string& imageset)
{
	if(CEGUI::ImagesetManager::getSingleton().isImagesetPresent(imageset))
		CEGUI::ImagesetManager::getSingleton().destroyImageset(imageset);
}


CEGUI::Window* CGUIManager::CreateDefaultWindow(const std::string& name)
{
	return CEGUI::WindowManager::getSingletonPtr()->createWindow("DefaultWindow", name);
}


bool CGUIManager::IsWindowPresent(const std::string& name)
{
	return CEGUI::WindowManager::getSingletonPtr()->isWindowPresent(name);
}


CEGUI::Window* CGUIManager::GetWindow(const std::string& name)
{
	if(IsWindowPresent(name))
		return CEGUI::WindowManager::getSingletonPtr()->getWindow(name);

	return NULL;
}


void CGUIManager::HideWindow(const std::string& name)
{
	if(IsWindowPresent(name))
	{
		GetWindow(name)->hide();
	}
}


void CGUIManager::ShowWindow(const std::string& name)
{
	if(IsWindowPresent(name))
	{
		GetWindow(name)->show();
	}
}


void CGUIManager::DisableWindow(const std::string& name)
{
	if(IsWindowPresent(name))
	{
		GetWindow(name)->disable();
	}
}


void CGUIManager::EnableWindow(const std::string& name)
{
	if(IsWindowPresent(name))
	{
		GetWindow(name)->enable();
	}
}


void CGUIManager::DestroyWindow(CEGUI::Window* window)
{
	CEGUI::WindowManager::getSingletonPtr()->destroyWindow(window);
}


void CGUIManager::DestroyWindow(const std::string& name)
{
	CEGUI::WindowManager::getSingletonPtr()->destroyWindow(name);
}


void CGUIManager::SubscribeEvent(const std::string& widget, const CEGUI::String& event, const CEGUI::Event::Subscriber& method)
{
	if(IsWindowPresent(widget))
	{
		GetWindow(widget)->subscribeEvent(event, method);
	}
}


void CGUIManager::RemoveAllEvents(const std::string& widget)
{
	if(IsWindowPresent(widget))
	{
		GetWindow(widget)->removeAllEvents();
	}
}


void CGUIManager::SetWindowText(const std::string& name, const std::string& text)
{
	if(IsWindowPresent(name))
	{
		GetWindow(name)->setText((CEGUI::utf8*)text.c_str());
	}
}


const char* CGUIManager::GetWindowText(const std::string& name)
{
	if(IsWindowPresent(name))
	{
		return GetWindow(name)->getText().c_str();
	}

	return "";
}


void CGUIManager::SetTooltipText(const std::string& name, const std::string& text)
{
	if(IsWindowPresent(name))
	{
		GetWindow(name)->setTooltipText((CEGUI::utf8*)text.c_str());
	}
}


void CGUIManager::DrawText(
	const std::string& text,
	const std::string& fontname,
	const CEGUI::Rect& draw_area,
	float z,
	float x_scale,
	float y_scale)
{
	CEGUI::Font* font;
	if(!fontname.empty() && CEGUI::FontManager::getSingleton().isFontPresent(fontname))
		font = CEGUI::FontManager::getSingleton().getFont(fontname);
	else
		font = CEGUI::System::getSingleton().getDefaultFont();

	font->drawText((CEGUI::utf8*)text.c_str(), draw_area, z, x_scale, y_scale);
}

//----------------------------------------------

void CGUIManager::HideCursor()
{
	CEGUI::MouseCursor::getSingleton().hide();
}


void CGUIManager::ShowCursor()
{
	CEGUI::MouseCursor::getSingleton().show();
}


void CGUIManager::SetCursorVisible(bool visible)
{
	CEGUI::MouseCursor::getSingleton().setVisible(visible);
}


bool CGUIManager::IsCursorVisible()
{
	return CEGUI::MouseCursor::getSingleton().isVisible();
}

void CGUIManager::GetCursorPosition(float* pos_x, float* pos_y)
{
	CEGUI::Point pos = CEGUI::MouseCursor::getSingleton().getPosition();
	if(pos_x) *pos_x = pos.d_x;
	if(pos_y) *pos_y = pos.d_y;
}

//----------------------------------------------

bool CGUIManager::MouseMove(float delta_x, float delta_y)
{
	return CEGUI::System::getSingleton().injectMouseMove(delta_x, delta_y);
}


bool CGUIManager::MousePosition(float pos_x, float pos_y)
{
	return CEGUI::System::getSingleton().injectMousePosition(pos_x, pos_y);
}


bool CGUIManager::MouseButtonDown(int button)//OIS::MouseButtonID button)
{
	CEGUI::MouseButton mbutton = CEGUI::NoButton;
	switch(button)
	{
	case OIS::MB_Left://VK_LBUTTON:
		mbutton = CEGUI::LeftButton;
		break;
	case OIS::MB_Right://VK_RBUTTON:
		mbutton = CEGUI::RightButton;
		break;
	case OIS::MB_Middle://VK_MBUTTON:
		mbutton = CEGUI::MiddleButton;
		break;
	case OIS::MB_Button3://0x05:
		mbutton = CEGUI::X1Button;
		break;
	case OIS::MB_Button4://0x06:
		mbutton = CEGUI::X2Button;
		break;
	}

	return CEGUI::System::getSingleton().injectMouseButtonDown(mbutton);
}


bool CGUIManager::MouseButtonUp(int button)
{
	CEGUI::MouseButton mbutton = CEGUI::NoButton;

	switch(button)
	{
	case OIS::MB_Left://VK_LBUTTON:
		mbutton = CEGUI::LeftButton;
		break;
	case OIS::MB_Right://VK_RBUTTON:
		mbutton = CEGUI::RightButton;
		break;
	case OIS::MB_Middle://VK_MBUTTON:
		mbutton = CEGUI::MiddleButton;
		break;
	case OIS::MB_Button3://0x05:
		mbutton = CEGUI::X1Button;
		break;
	case OIS::MB_Button4://0x06:
		mbutton = CEGUI::X2Button;
		break;
	}

	return CEGUI::System::getSingleton().injectMouseButtonUp(mbutton);
}


bool CGUIManager::KeyDown(unsigned int keycode)
{
	return CEGUI::System::getSingleton().injectKeyDown(keycode);
}


bool CGUIManager::KeyUp(unsigned int keycode)
{
	return CEGUI::System::getSingleton().injectKeyUp(keycode);
}


bool CGUIManager::Char(unsigned long code_point)
{
	return CEGUI::System::getSingleton().injectChar(code_point);
}


bool CGUIManager::MouseWheelChange(float delta)
{
	return CEGUI::System::getSingleton().injectMouseWheelChange(delta);
}


bool CGUIManager::Update(float timeElapsed)
{
	return CEGUI::System::getSingleton().injectTimePulse(timeElapsed*0.001f);
}

