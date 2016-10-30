/************************************************************************************//**
 * @file CConversation.cpp
 * @brief Conversation class handler
 *
 * This file contains the class implementation for Conversation handling.
 * @author Daniel Queteschiner
 * @date August 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "RabidFramework.h"
#include "CGUIManager.h"
#include "RFSX/CEGUI/sxCEGUIWindow.h"
#include "CConversation.h"

CConversation::CConversation(skExecutable* owner, const std::string& layout) :
	m_Owner(owner)
{
	if(layout.empty())
		m_Layout = NULL;
	else
		m_Layout = CCD->GUIManager()->LoadWindowLayout(layout);
}

CConversation::~CConversation()
{
}
