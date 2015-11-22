/*
	CMessage.h:		Message class implementation

	(c) 2001 Ralph Deane
	All Rights Reserved

	This file contains the class declaration for Message
implementation.
*/

//	Include the One True Header

#include "RabidFramework.h"

//	Constructor
//

CMessage::CMessage()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

//	Ok, check to see if there are Messages in this world

	pSet = geWorld_GetEntitySet(CCD->World(), "Message");

	if(!pSet) 
		return;	

//	Ok, we have Messages somewhere.  Dig through 'em all.

	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	    pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		Message *pSource = (Message*)geEntity_GetUserData(pEntity);
		if(EffectC_IsStringNull(pSource->szEntityName))
		{
			char szName[128];
			geEntity_GetName(pEntity, szName, 128);
			pSource->szEntityName = szName;
		}
		// Ok, put this entity into the Global Entity Registry
		CCD->EntityRegistry()->AddEntity(pSource->szEntityName, "Message");
		pSource->active = GE_FALSE;
		pSource->Ticks = 0.0f;
	}
}


//	Destructor
//

CMessage::~CMessage()
{

}

//  Tick
//

void CMessage::Tick(float dwTicks)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

//	Ok, check to see if there are Messages in this world

	pSet = geWorld_GetEntitySet(CCD->World(), "Message");

	if(!pSet) 
		return;

//	Ok, we have Messages somewhere.  Dig through 'em all.

	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	    pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		Message *pSource = (Message*)geEntity_GetUserData(pEntity);
		if(!EffectC_IsStringNull(pSource->TriggerName))
		{
			if(GetTriggerState(pSource->TriggerName))
			{
				if(pSource->active==GE_FALSE)
				{
					pSource->Ticks = 0.0f;
					pSource->active=GE_TRUE;
				}
			}
		}
		if(pSource->active==GE_TRUE)
		{
			pSource->Ticks += dwTicks;
			if((pSource->Ticks*0.001f)>=pSource->TimeOn)
				pSource->active=GE_FALSE;
		}
	}

  return;
}

//  Display
//

void CMessage::Display()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	int YOffset = 4;

//	Ok, check to see if there are Messages in this world

	pSet = geWorld_GetEntitySet(CCD->World(), "Message");

	if(!pSet) 
		return;

//	Ok, we have Messages somewhere.  Dig through 'em all.

	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	    pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		Message *pSource = (Message*)geEntity_GetUserData(pEntity);

		if(pSource->active==GE_TRUE)
		{
			if(!EffectC_IsStringNull(pSource->Text))
			{
				CCD->MenuManager()->FontRect(pSource->Text, pSource->FontSize, 4, YOffset);
				YOffset += (CCD->MenuManager()->FontHeight(pSource->FontSize)+2);
			}
		}
	}

  return;
}


//	******************** CRGF Overrides ********************

//	LocateEntity
//
//	Given a name, locate the desired item in the currently loaded level
//	..and return it's user data.

int CMessage::LocateEntity(char *szName, void **pEntityData)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

//	Ok, check to see if there are Messages in this world

	pSet = geWorld_GetEntitySet(CCD->World(), "Message");

	if(!pSet) 
		return RGF_NOT_FOUND;									// No 3D audio sources

//	Ok, we have Messages somewhere.  Dig through 'em all.

	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	    pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
		{
		Message *pSource = (Message*)geEntity_GetUserData(pEntity);
		if(strcmp(pSource->szEntityName, szName) == 0)
		  {
			*pEntityData = (void *)pSource;
			return RGF_SUCCESS;
			}
		}

  return RGF_NOT_FOUND;								// Sorry, no such entity here
}

//	ReSynchronize
//
//	Correct internal timing to match current time, to make up for time lost
//	..when outside the game loop (typically in "menu mode").

int CMessage::ReSynchronize()
{
	return RGF_SUCCESS;
}

