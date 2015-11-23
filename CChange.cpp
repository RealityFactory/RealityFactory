

//	Include the One True Header

#include "RabidFramework.h"

//
//	Constructor
//

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
		
		for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
		{
			ChangeAttribute *pSource = (ChangeAttribute*)geEntity_GetUserData(pEntity);

			pSource->active = GE_FALSE;
			pSource->OldState = GE_FALSE;
		}
	}
}

CChangeAttribute::~CChangeAttribute()
{
}


//
// Tick
//

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
		
		for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
		{
			ChangeAttribute *pSource = (ChangeAttribute*)geEntity_GetUserData(pEntity);
			if(!EffectC_IsStringNull(pSource->StopTrigger))
			{
				if(GetTriggerState(pSource->StopTrigger))
				{
					if(pSource->active)
					{
						pSource->active = false;
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
								pSource->active = true;
								pSource->Count = pSource->Time;
								pSource->Total = 0.0f;
								if(pSource->Percentage)
									pSource->Increment = (((float)pSource->Amount*(float)theInv->Value(pSource->Attribute))/100.0f)/pSource->Count;
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
				if(pSource->Increment<0.0f && !strcmp(pSource->Attribute, "health"))
					Active = true;
				if(pSource->Count<=0.0f)
				{
					pSource->active = false;
					continue;
				}
			}
		}
	}
}



//	SaveTo
//
//	Save the current state of every  ChangeAttribute in the current world
//	..off to an open file.

int CChangeAttribute::SaveTo(FILE *SaveFD, bool type)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

//	Ok, check to see if there are  ChangeAttributes in this world

	pSet = geWorld_GetEntitySet(CCD->World(), "ChangeAttribute");

	if(!pSet)
		return RGF_SUCCESS;

//	Ok, we have ChangeAttributes somewhere.  Dig through 'em all.

	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	    pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity))
		{
		ChangeAttribute *pSource = (ChangeAttribute*)geEntity_GetUserData(pEntity);
		WRITEDATA(&pSource->active, sizeof(geBoolean), 1, SaveFD);
		WRITEDATA(&pSource->Count, sizeof(geFloat), 1, SaveFD);
		WRITEDATA(&pSource->OldState, sizeof(geBoolean), 1, SaveFD);
		WRITEDATA(&pSource->Increment, sizeof(geFloat), 1, SaveFD);
		WRITEDATA(&pSource->Total, sizeof(geFloat), 1, SaveFD);
	  }

  return RGF_SUCCESS;
}

//	RestoreFrom
//
//	Restore the state of every ChangeAttribute in the current world from an
//	..open file.

int CChangeAttribute::RestoreFrom(FILE *RestoreFD, bool type)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

//	Ok, check to see if there are ChangeAttributes in this world

	pSet = geWorld_GetEntitySet(CCD->World(), "ChangeAttribute");

	if(!pSet)
		return RGF_SUCCESS;

  for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
    pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity))
	  {
		ChangeAttribute *pSource = (ChangeAttribute*)geEntity_GetUserData(pEntity);
		READDATA(&pSource->active, sizeof(geBoolean), 1, RestoreFD);
		READDATA(&pSource->Count, sizeof(geFloat), 1, RestoreFD);
		READDATA(&pSource->OldState, sizeof(geBoolean), 1, RestoreFD);
		READDATA(&pSource->Increment, sizeof(geFloat), 1, RestoreFD);
		READDATA(&pSource->Total, sizeof(geFloat), 1, RestoreFD);
    }

  return RGF_SUCCESS;
}