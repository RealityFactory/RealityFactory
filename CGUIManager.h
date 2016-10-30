/************************************************************************************//**
 * @file CGUIManager.h
 * @brief Class declaration for GUI Manager
 * @author Daniel Queteschiner
 * @date July 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_CGUIMANAGER_H_
#define __RGF_CGUIMANAGER_H_

#include <CEGUISystem.h>
#include <CEGUIWindow.h>
#include "CEGUI\\CEGUIGenesis3DRenderer.h"
#include "CEGUI\\CEGUIGenesis3DResourceProvider.h"
#include "CEGUI\\CEGUISimkin.h"

#ifdef DrawText
#undef DrawText
#endif
#ifdef SetWindowText
#undef SetWindowText
#endif
#ifdef GetWindowText
#undef GetWindowText
#endif

typedef CEGUI::Window GUIWindow;

class CGUIManager
{
public:
	CGUIManager();
	CGUIManager(geEngine *engine, int width, int height, geWorld *world, geVFile *vfile, const std::string& logfile = "CEGUI.log");
	~CGUIManager();

	void SetResourceGroupDirectory(const std::string& resourceGroup, const std::string& directory);

	void Create(geEngine *engine, int width, int height, geWorld *world, geVFile *vfile, const std::string& logfile = "CEGUI.log");

	void LoadScheme(const std::string& scheme_filename);
	bool IsSchemePresent(const std::string& scheme_name) const;

	void SetDefaultMouseCursor(const std::string& imageset, const std::string& imagename);
	void SetDefaultFont(const std::string& ttffont);
	void SetDefaultTooltip(const std::string& tooltip);

	CEGUI::Window* SetSheet(CEGUI::Window *sheet);
	CEGUI::Window* GetSheet();

	void CleanDeadWindowPool();
	CEGUI::Window* LoadWindowLayout(const std::string& layout);
	CEGUI::Imageset* CreateImagesetFromImageFile(const std::string& imageset, const std::string& imagefile, const std::string& resourceGroup = "");
	void DestroyImageset(const std::string& imageset);

	CEGUI::Window* CreateDefaultWindow(const std::string& name);
	bool IsWindowPresent(const std::string& name);
	CEGUI::Window* GetWindow(const std::string& name);
	void HideWindow(const std::string& name);
	void ShowWindow(const std::string& name);
	void DisableWindow(const std::string& name);
	void EnableWindow(const std::string& name);
	void DestroyWindow(const std::string& name);
	void DestroyWindow(CEGUI::Window* window);

	void SubscribeEvent(const std::string& widget, const CEGUI::String& event, const CEGUI::Event::Subscriber& method);
	void RemoveAllEvents(const std::string& widget);

	void SetWindowText(const std::string& name, const std::string& text);
	const char* GetWindowText(const std::string& name);

	void SetTooltipText(const std::string& name, const std::string& text);

	void DrawText(const std::string& text, const std::string& fontname, const CEGUI::Rect& draw_area, float z, float x_scale = 1.0f, float y_scale = 1.0f);

	void Render();

	void HideCursor();
	void ShowCursor();
	void SetCursorVisible(bool visible);
	bool IsCursorVisible();
	void GetCursorPosition(float* pos_x, float* pos_y);

	bool Update(float timeElapsed);

	// input handler

	bool MouseWheelChange(float delta);
	bool MouseMove(float delta_x, float delta_y);
	bool MousePosition(float pos_x, float pos_y);
	bool MouseButtonDown(int button);
	bool MouseButtonUp(int button);

	bool KeyDown(unsigned int keycode);
	bool KeyUp(unsigned int keycode);
	bool Char(unsigned long code_point);

private:
	void Destroy();

private:
	CEGUI::System *m_System;
	CEGUI::Genesis3DRenderer *m_Renderer;
	CEGUI::Genesis3DResourceProvider *m_ResourceProvider;
	CEGUI::SimkinScriptModule *m_ScriptModule;

	std::vector<std::string> m_LoadedSchemeFiles;
};

#endif
