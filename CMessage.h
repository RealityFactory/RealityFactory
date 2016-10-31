/************************************************************************************//**
 * @file CMessage.h
 * @brief Message class handler
 *
 * This file contains the class declaration for Message handling.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All Rights Reserved.
 ****************************************************************************************/

#ifndef __RGF_CMESSAGE_H_
#define __RGF_CMESSAGE_H_

#include <hash_map>

class MessageType;


class CMessage : public CRGFComponent
{
public:
	CMessage();
	~CMessage();

	void Tick(float timeElapsed);

	/**
	 * @brief Given a name, locate the desired entity in the currently loaded
	 * level and return its user data.
	 */
	int LocateEntity(const char *szName, void **pEntityData);

	/**
	 * @brief Correct internal timing to match current time, to make up for time
	 * lost when outside the game loop (typically in "menu mode").
	 */
	int ReSynchronize();

	void LoadText(const std::string& messagetxt);

	bool LoadConfiguration(const std::string& filename);

private:
	const utf8* GetText(const std::string& name);
	void ClearMessageTypes();

private:
	stdext::hash_map<std::string, std::string> m_Text;
	stdext::hash_map<std::string, MessageType*> m_MessageTypes;
	bool m_Active;
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
