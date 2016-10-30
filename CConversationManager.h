/************************************************************************************//**
 * @file CConversationManager.h
 * @brief ConversationManager class handler
 *
 * This file contains the class declaration for Conversation handling.
 * @author Daniel Queteschiner
 * @date August 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_CONVERSATIONMANAGER_H_
#define __RGF_CONVERSATIONMANAGER_H_

#include "RFSX\\RFSX.h"
#include <queue>

class CConversation;


#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxConversationManager

#ifdef SX_METHOD_ARGS
#undef SX_METHOD_ARGS
#endif
#define SX_METHOD_ARGS sxConversationMethodArgs

typedef struct SX_METHOD_ARGS SX_METHOD_ARGS;

#define SXCONVERSATIONMANAGER_METHODS 16

class SX_IMPL_TYPE : public skExecutable
{
	friend class CPlayer;
	friend class ScriptedObject;

public:
	virtual ~SX_IMPL_TYPE();
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);
	virtual int executableType() const { return RFSXG_CONVERSATION; }

	void Initialize();

	void StartConversation(CConversation* conversation);

	bool IsConversing() const	{ return m_IsConversing; }
	void StopConversing()		{ m_IsConversing = false; }

	void Update(float timeElapsed);

	void Enqueue(SX_METHOD_ARGS* ma);

	void Dequeue();
	void DequeueAll();

	void Skip() { if(m_IsSpeaking) m_Skip = true; }

	void SetTextDisplayTime(float time) { m_TextDisplayTime = time; }
	void SetTextScrollSpeed(float speed) { m_TextScrollSpeed = speed; }
	float GetTextDisplayTime() const	{ return m_TextDisplayTime; }
	float GetTextScrollSpeed() const	{ return m_TextScrollSpeed; }
	void SetTextWindow(const std::string& windowname);
	void SetIconWindow(const std::string& windowname);

	CConversation* GetConversation() { return m_Conversation; }

	void LoadText(const std::string& conversationtxt);

	const std::string& GetText(const std::string& name);

	bool LoadConfiguration(const std::string& filename);

	static SX_IMPL_TYPE* GetSingletonPtr();

	Q_METHOD_DECL(Run);
	Q_METHOD_DECL(Call);
	Q_METHOD_DECL(Speak);
	Q_METHOD_DECL(SetIcon);

protected:
	SX_METHOD_DECL(Run);
	SX_METHOD_DECL(Call);
	SX_METHOD_DECL(Speak);
	SX_METHOD_DECL(SetIcon);

	SX_METHOD_DECL(Skip);
	SX_METHOD_DECL(SkipAll);
	SX_METHOD_DECL(SetTextScrollSpeed); // characters per second
	SX_METHOD_DECL(SetTextDisplayTime); // seconds
	SX_METHOD_DECL(SetTextWindow);
	SX_METHOD_DECL(SetIconWindow);
	SX_METHOD_DECL(SetEventState);
	SX_METHOD_DECL(GetEventState);
	SX_METHOD_DECL(SetFlag);
	SX_METHOD_DECL(GetFlag);

	SX_METHOD_DECL(GetOwner);
	SX_METHOD_DECL(GetText);

private:
	SX_IMPL_TYPE();
	SX_IMPL_TYPE(const SX_IMPL_TYPE&) { }
	SX_IMPL_TYPE& operator = (const SX_IMPL_TYPE&);

	bool			m_IsConversing;
	bool			m_Skip;
	bool			m_IsSpeaking;
	float			m_TextDisplayTime; // milliseconds
	float			m_TextScrollSpeed; // characters per millisecond
	CConversation*	m_Conversation;
	CEGUI::Window*	m_TextWindow;
	CEGUI::Window*	m_IconWindow;

	stdext::hash_map<std::string, std::string>		m_Text;
	stdext::hash_map<std::string, CEGUI::Window*>	m_Layouts;
	std::queue<SX_METHOD_ARGS*>						m_MethodQueue;

private:

	static RFSX::sxMHT<SX_IMPL_TYPE>	h_method;
	static SX_IMPL_TYPE*				m_ConversationManager;
};

#undef SX_METHOD_ARGS
#undef SX_IMPL_TYPE

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
