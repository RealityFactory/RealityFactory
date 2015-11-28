/****************************************************************************************/
/*																						*/
/*	CChange.cpp:	CChangeAttribute class												*/
/*																						*/
/*	(c) 2001 Ralph Deane																*/
/*																						*/
/****************************************************************************************/


//	Include the One True Header

#include "RabidFramework.h"

/* ------------------------------------------------------------------------------------ */
//	Constructor
/* ------------------------------------------------------------------------------------ */
CChangeAttribute::CChangeAttribute()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	Active = false;

	//	Ok, check to see if there are ChangeAttributes in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "ChangeAttribute");

	if(pSet)
	{
		//	Ok, we have ChangeAttributes somewhere.  Dig through 'em all.
		for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
			pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
		{
			ChangeAttribute *pSource = (ChangeAttribute*)geEntity_GetUserData(pEntity);

			pSource->active = GE_FALSE;
			pSource->OldState = GE_FALSE;
		}
	}
}

/* ------------------------------------------------------------------------------------ */
//	Destructor
/* ------------------------------------------------------------------------------------ */
CChangeAttribute::~CChangeAttribute()
{
}

/* ------------------------------------------------------------------------------------ */
//	Tick
/* ------------------------------------------------------------------------------------ */
void CChangeAttribute::Tick(float dwTicks)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	Active = false;

	//	Ok, check to see if there are ChangeAttributes in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "ChangeAttribute");

	if(pSet)
	{
		CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());

		//	Ok, we have ChangeAttributes somewhere.  Dig through 'em all.
		for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
			pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
		{
			ChangeAttribute *pSource = (ChangeAttribute*)geEntity_GetUserData(pEntity);

			if(!EffectC_IsStringNull(pSource->StopTrigger))
			{
				if(GetTriggerState(pSource->StopTrigger))
				{
					if(pSource->active)
					{
						pSource->active = GE_FALSE;
						continue;
					}
				}
			}

			if(!EffectC_IsStringNull(pSource->StartTrigger))
			{
				if(GetTriggerState(pSource->StartTrigger))
				{
					if(!pSource->OldState)
					{
						pSource->OldState = GE_TRUE;

						if(!pSource->active)
						{
							if(theInv->Has(pSource->Attribute))
							{
								pSource->active = GE_TRUE;
								pSource->Count = pSource->Time;
								pSource->Total = 0.0f;

								if(pSource->Percentage)
									pSource->Increment = (((float)pSource->Amount*(float)theInv->Value(pSource->Attribute))*0.01f)/pSource->Count;
								else
									pSource->Increment = (float)pSource->Amount/pSource->Count;
							}

							continue;
						}
					}
				}
				else
					pSource->OldState = GE_FALSE;
			}

			if(pSource->active)
			{
				pSource->Total += ((0.001f*dwTicks)*pSource->Increment);
				theInv->Modify(pSource->Attribute, (int)pSource->Total);
				pSource->Total -= (int)pSource->Total;
				pSource->Count -= (0.001f*dwTicks);

				if(pSource->Increment < 0.0f && !strcmp(pSource->Attribute, "health"))
					Active = true;

				if(pSource->Count <= 0.0f)
				{
					pSource->active = GE_FALSE;
					continue;
				}
			}
		}
	}
}

/* ------------------------------------------------------------------------------------ */
//	SaveTo
//
//	Save the current state of every  ChangeAttribute in the current world
//	..off to an open file.
/* ------------------------------------------------------------------------------------ */
int CChangeAttribute::SaveTo(FILE *SaveFD, bool type)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	//	Ok, check to see if there are  ChangeAttributes in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "ChangeAttribute");

	if(!pSet)
		return RGF_SUCCESS;

	//	Ok, we have ChangeAttributes somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	    pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		ChangeAttribute *pSource = (ChangeAttribute*)geEntity_GetUserData(pEntity);

		WRITEDATA(type, &(pSource->active),		sizeof(geBoolean),	1, SaveFD);
		WRITEDATA(type, &(pSource->Count),		sizeof(geFloat),	1, SaveFD);
		WRITEDATA(type, &(pSource->OldState),	sizeof(geBoolean),	1, SaveFD);
		WRITEDATA(type, &(pSource->Increment),	sizeof(geFloat),	1, SaveFD);
		WRITEDATA(type, &(pSource->Total),		sizeof(geFloat),	1, SaveFD);
	}

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	RestoreFrom
//
//	Restore the state of every ChangeAttribute in the current world from an open file.
/* ------------------------------------------------------------------------------------ */
int CChangeAttribute::RestoreFrom(FILE *RestoreFD, bool type)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	//	Ok, check to see if there are ChangeAttributes in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "ChangeAttribute");

	if(!pSet)
		return RGF_SUCCESS;

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		ChangeAttribute *pSource = (ChangeAttribute*)geEntity_GetUserData(pEntity);

		READDATA(type, &(pSource->active),		sizeof(geBoolean),	1, RestoreFD);
		READDATA(type, &(pSource->Count),		sizeof(geFloat),	1, RestoreFD);
		READDATA(type, &(pSource->OldState),	sizeof(geBoolean),	1, RestoreFD);
		READDATA(type, &(pSource->Increment),	sizeof(geFloat),	1, RestoreFD);
		READDATA(type, &(pSource->Total),		sizeof(geFloat),	1, RestoreFD);
    }

	return RGF_SUCCESS;
}

/* ----------------------------------- END OF FILE ------------------------------------ */
