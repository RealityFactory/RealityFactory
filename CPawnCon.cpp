/************************************************************************************//**
 * @file CPawnCon.cpp
 * @brief Pawn Conversation
 * @author Ralph Deane
 *//*
 * Copyright (c) Ralph Deane; All Rights Reserved.
 ****************************************************************************************/

#include "RabidFramework.h"
#include "CConversationManager.h"
#include "CPawn.h"
#include "CPawnManager.h"


/* ------------------------------------------------------------------------------------ */
//	Converse - run conversation from collision (use key interaction)
/* ------------------------------------------------------------------------------------ */
bool CPawnManager::Converse(const geActor* pActor)
{
	geEntity_EntitySet *pSet;

	// Ok, check to see if there are Pawns in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "Pawn");

	if(pSet)
	{
		geEntity *pEntity;
		for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
			pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
		{
			Pawn *pSource = static_cast<Pawn*>(geEntity_GetUserData(pEntity));

			if(pSource->Data)
			{
				ScriptedObject *object = static_cast<ScriptedObject*>(pSource->Data);

				if(object->m_Actor == pActor && object->m_Active && object->m_UseKey)
				{
					if(object->m_Conversation)
					{
						sxConversationManager::GetSingletonPtr()->StartConversation(object->m_Conversation);
					}

					return true;
				}
			}
		}
	}

	return false;
}

/* ----------------------------------- END OF FILE ------------------------------------ */
