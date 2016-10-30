/************************************************************************************//**
 * @file CConversation.h
 * @brief Conversation class handler
 *
 * This file contains the class declaration for Conversation handling.
 * @author Daniel Queteschiner
 * @date August 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_CONVERSATION_H_
#define __RGF_CONVERSATION_H_

class skExecutable;
class CEGUI::Window;

class CConversation
{
public:
	CConversation(skExecutable* owner, const std::string& layout = "");
	~CConversation();

	void SetOrder(const std::string& order) { m_Order = order; }
	const std::string& GetOrder() const		{ return m_Order; }

	skExecutable* GetOwner() const			{ return m_Owner; }

	CEGUI::Window* GetLayout() const		{ return m_Layout; }

private:

	std::string		m_Order;
	skExecutable*	m_Owner;
	CEGUI::Window*	m_Layout;
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
