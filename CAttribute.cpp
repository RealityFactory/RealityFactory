/*
CAttribute.cpp:		Attribute class implementation

  (c) 2001 Ralph Deane
  All Rights Reserved
  
	This file contains the class declaration for Attribute
	implementation.
*/

//	Include the One True Header

#include "RabidFramework.h"

extern geSound_Def *SPool_Sound(char *SName);
extern "C" void	DrawBoundBox(geWorld *World, const geVec3d *Pos, const geVec3d *Min, const geVec3d *Max);

//
//	Constructor
//

CAttribute::CAttribute()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

		
	//	Ok, check to see if there are Attributes in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "Attribute");
	
	if(pSet) 
	{
		//	Ok, we have Attributes somewhere.  Dig through 'em all.
		
		for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
		{
			Attribute *pSource = (Attribute*)geEntity_GetUserData(pEntity);
			if(EffectC_IsStringNull(pSource->szEntityName))
			{
				char szName[128];
				geEntity_GetName(pEntity, szName, 128);
				pSource->szEntityName = szName;
			}
			// Ok, put this entity into the Global Entity Registry
			CCD->EntityRegistry()->AddEntity(pSource->szEntityName, "Attribute");
			pSource->OriginOffset = pSource->origin;
			if(pSource->Model)
			{
				geVec3d ModelOrigin;
	    		geWorld_GetModelRotationalCenter(CCD->World(), pSource->Model, &ModelOrigin);
				geVec3d_Subtract(&pSource->origin, &ModelOrigin, &pSource->OriginOffset);
  			}
			pSource->active = GE_FALSE;
			pSource->bState = GE_FALSE;
			pSource->CallBack = GE_FALSE;
			pSource->alive = GE_TRUE;
			pSource->ActorRotation.X = 0.0174532925199433f*pSource->ActorRotation.X;
			pSource->ActorRotation.Y = 0.0174532925199433f*pSource->ActorRotation.Y;
			pSource->ActorRotation.Z = 0.0174532925199433f*pSource->ActorRotation.Z;
			pSource->Actor = CCD->ActorManager()->SpawnActor(pSource->szActorName, 
				pSource->origin, pSource->ActorRotation, "", "", NULL);
			if(!pSource->Actor)
			{
				char szError[256];
				sprintf(szError,"%s : Missing Actor '%s'", pSource->szEntityName, pSource->szActorName);
				CCD->ReportError(szError, false);
				CCD->ShutdownLevel();
				delete CCD;
				MessageBox(NULL, szError,"Attribute", MB_OK);
				exit(-333);
			}
			CCD->ActorManager()->RemoveActor(pSource->Actor);
			geActor_Destroy(&pSource->Actor); 
// start multiplayer
			pSource->Actor = NULL;
// end multiplayer
			if(!EffectC_IsStringNull(pSource->szSoundFile))
				SPool_Sound(pSource->szSoundFile);
			if(!EffectC_IsStringNull(pSource->szReSpawnSound))
				SPool_Sound(pSource->szReSpawnSound);
		}
	}

// changed RF063

	// ModifyAttribute

	pSet = geWorld_GetEntitySet(CCD->World(), "ModifyAttribute");
	
	if(pSet) 
	{
		//	Ok, we have Attributes somewhere.  Dig through 'em all.
		
		for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
		{
			ModifyAttribute *pSource = (ModifyAttribute*)geEntity_GetUserData(pEntity);
			pSource->active = false;
		}
	}
// end change RF063
}

CAttribute::~CAttribute()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	//	Ok, check to see if there are Attributes in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "Attribute");
	
	if(pSet) 
	{
		
		//	Ok, we have Attributes somewhere.  Dig through 'em all.
		
		for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
		{
			Attribute *pSource = (Attribute*)geEntity_GetUserData(pEntity);
			if(pSource->Actor)
			{
				if(pSource->active==GE_TRUE)
					geWorld_RemoveActor(CCD->World(), pSource->Actor);
				geActor_Destroy(&pSource->Actor);
			}
		}
	}
}


//
// Tick
//

void CAttribute::Tick(float dwTicks)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	//	Ok, check to see if there are Attributes in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "Attribute");
	
	if(pSet) 
	{
		
		//	Ok, we have Attributes somewhere.  Dig through 'em all.
		
		for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
		{
			Attribute *pSource = (Attribute*)geEntity_GetUserData(pEntity);
			if(pSource->CallBack==GE_TRUE)
			{
				pSource->CallBackCount-=1;
				if(pSource->CallBackCount==0)
					pSource->CallBack=GE_FALSE;
			}
			if(!pSource->alive)
			{
				if(pSource->ReSpawn)
				{
					pSource->Tick+=dwTicks;
					if(pSource->Tick>=(pSource->Delay*1000.0f))
					{
						pSource->alive=GE_TRUE;
						if(!EffectC_IsStringNull(pSource->szReSpawnSound))
						{
							Snd 	Sound;
							memset( &Sound, 0, sizeof( Sound ) );
							geVec3d_Copy( &(pSource->origin), &( Sound.Pos ) );
							Sound.Min=CCD->GetAudibleRadius();
							Sound.Loop=GE_FALSE;
							Sound.SoundDef=SPool_Sound(pSource->szReSpawnSound);
							if(Sound.SoundDef!=NULL)
								CCD->EffectManager()->Item_Add(EFF_SND, (void *)&Sound);
						} 
					}
				}
				continue;
			}
			pSource->origin = pSource->OriginOffset;
			SetOriginOffset(pSource->EntityName, pSource->BoneName, pSource->Model, &(pSource->origin));
			if(!EffectC_IsStringNull(pSource->TriggerName))
			{
				if(GetTriggerState(pSource->TriggerName))
				{
					if(pSource->active==GE_FALSE)
					{
						pSource->Actor = CCD->ActorManager()->SpawnActor(pSource->szActorName, 
							pSource->origin, pSource->ActorRotation, "", "", NULL);//pSource->Actor);
						CCD->ActorManager()->SetType(pSource->Actor, ENTITY_ATTRIBUTE_MOD);
						CCD->ActorManager()->SetScale(pSource->Actor, pSource->Scale);
						CCD->ActorManager()->Position(pSource->Actor, pSource->origin);
						if(pSource->Gravity)
							CCD->ActorManager()->SetGravity(pSource->Actor, CCD->Player()->GetGravity());
						CCD->ActorManager()->SetActorDynamicLighting(pSource->Actor, pSource->FillColor, pSource->AmbientColor);
						CCD->ActorManager()->SetShadow(pSource->Actor, pSource->ShadowSize);
// changed RF064
						CCD->ActorManager()->SetHideRadar(pSource->Actor, pSource->HideFromRadar);
						if(!EffectC_IsStringNull(pSource->ChangeMaterial))
							CCD->ActorManager()->ChangeMaterial(pSource->Actor, pSource->ChangeMaterial);
// end change RF064
						pSource->active=GE_TRUE;
						pSource->bState = GE_TRUE;
					}
				}
				else
				{
					if(pSource->active==GE_TRUE)
					{
						CCD->ActorManager()->RemoveActor(pSource->Actor);
						geActor_Destroy(&pSource->Actor);
						pSource->active=GE_FALSE;
						pSource->bState = GE_FALSE;
					}
				}
			}
			else
			{
				if(pSource->active==GE_FALSE)
				{
					pSource->Actor = CCD->ActorManager()->SpawnActor(pSource->szActorName, 
						pSource->origin, pSource->ActorRotation, "", "", NULL);//pSource->Actor);
					CCD->ActorManager()->SetType(pSource->Actor, ENTITY_ATTRIBUTE_MOD);
					CCD->ActorManager()->SetScale(pSource->Actor, pSource->Scale);
					CCD->ActorManager()->Position(pSource->Actor, pSource->origin);
					if(pSource->Gravity)
						CCD->ActorManager()->SetGravity(pSource->Actor, CCD->Player()->GetGravity());
					CCD->ActorManager()->SetActorDynamicLighting(pSource->Actor, pSource->FillColor, pSource->AmbientColor);
					CCD->ActorManager()->SetShadow(pSource->Actor, pSource->ShadowSize);
// changed RF064
					CCD->ActorManager()->SetHideRadar(pSource->Actor, pSource->HideFromRadar);
					if(!EffectC_IsStringNull(pSource->ChangeMaterial))
						CCD->ActorManager()->ChangeMaterial(pSource->Actor, pSource->ChangeMaterial);
// end change RF064
					pSource->active=GE_TRUE;
					pSource->bState = GE_TRUE;
				}
			}
		}
	}

// changed RF063

	// ModifyAttribute

	pSet = geWorld_GetEntitySet(CCD->World(), "ModifyAttribute");
	
	if(pSet) 
	{
		//	Ok, we have Attributes somewhere.  Dig through 'em all.
		
		for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
		{
			ModifyAttribute *pSource = (ModifyAttribute*)geEntity_GetUserData(pEntity);
			if(!EffectC_IsStringNull(pSource->TriggerName))
			{
				if(GetTriggerState(pSource->TriggerName))
				{
					if(pSource->active==GE_FALSE)
					{
						CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
						if(theInv->Has(pSource->Attribute))
						{
							theInv->Modify(pSource->Attribute, pSource->Amount);
							pSource->active = true;
						}
					}
				}
				else
				{
					pSource->active = false;
				}
			}
		}
	}
// end change RF063
}


bool CAttribute::HandleCollision(geActor *theTarget, geActor *pActor)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	//	Ok, check to see if there are Attributes in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "Attribute");
	
	if(!pSet) 
		return false;	
	
	//	Ok, we have Attributes somewhere.  Dig through 'em all.
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		Attribute *pSource = (Attribute*)geEntity_GetUserData(pEntity);
		if(pSource->Actor != pActor)
			continue;	  // Not this one, keep looking
// changed RF063
		if(pSource->PlayerOnly && theTarget!=CCD->Player()->GetActor())
			return false;
// end change RF063
		if(pSource->active == GE_FALSE)
			continue;
// changed RF063		
		bool flag = false;

		CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(theTarget);
		if(!EffectC_IsStringNull(pSource->AttributeName))
		{
			if(theInv->Has(pSource->AttributeName))
			{
				if(theInv->Value(pSource->AttributeName)<theInv->High(pSource->AttributeName)
					|| pSource->AttributeAmount<0)
				{
					theInv->Modify(pSource->AttributeName, pSource->AttributeAmount);
					flag = true;
				}
			}
		}
		if(!flag)
		{
			if(!EffectC_IsStringNull(pSource->AttributeAltName))
			{
				if(theInv->Has(pSource->AttributeAltName))
				{
					if(theInv->Value(pSource->AttributeAltName)<theInv->High(pSource->AttributeAltName)
						|| pSource->AttributeAltAmount<0)
					{
						theInv->Modify(pSource->AttributeAltName, pSource->AttributeAltAmount);
						flag = true;
					}
				}
			}
		}

		if(flag)
		{
			CCD->ActorManager()->RemoveActor(pSource->Actor);
			geActor_Destroy(&pSource->Actor);
			
			pSource->active=GE_FALSE;
			pSource->bState = GE_FALSE;
			pSource->alive=GE_FALSE;
			pSource->Tick=0.0f;
			pSource->CallBack = GE_TRUE;
			pSource->CallBackCount = 2;
			if(!EffectC_IsStringNull(pSource->szSoundFile))
			{
				Snd 	Sound;
				memset( &Sound, 0, sizeof( Sound ) );
				geVec3d_Copy( &(pSource->origin), &( Sound.Pos ) );
				Sound.Min=CCD->GetAudibleRadius();
				Sound.Loop=GE_FALSE;
				Sound.SoundDef=SPool_Sound(pSource->szSoundFile);
				if(Sound.SoundDef!=NULL)
					CCD->EffectManager()->Item_Add(EFF_SND, (void *)&Sound);
			} 
			return true;
		}
// end change RF063
	}
	return false;
}

// changed RF063
//	SaveTo
//
//	Save Attributes to a supplied file

int CAttribute::SaveTo(FILE *SaveFD, bool type)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	//	Ok, check to see if there are Attributes in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "Attribute");
	
	if(!pSet) 
		return RGF_SUCCESS;									// No fields
	
	//	Ok, we have Attributes somewhere.  Dig through 'em all.
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		Attribute *pSource = (Attribute*)geEntity_GetUserData(pEntity);
		WRITEDATA(&pSource->alive, sizeof(geBoolean), 1, SaveFD);
		WRITEDATA(&pSource->active, sizeof(geBoolean), 1, SaveFD);
		WRITEDATA(&pSource->Tick, sizeof(geFloat), 1, SaveFD);
		WRITEDATA(&pSource->CallBack, sizeof(geBoolean), 1, SaveFD);
		WRITEDATA(&pSource->CallBackCount, sizeof(int), 1, SaveFD);
		WRITEDATA(&pSource->bState, sizeof(geBoolean), 1, SaveFD);
	}
	
	return RGF_SUCCESS;
}

//	RestoreFrom
//
//	Restore Attributes from a supplied file

int CAttribute::RestoreFrom(FILE *RestoreFD, bool type)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	//	Ok, check to see if there are Attributes in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "Attribute");
	
	if(!pSet) 
		return RGF_SUCCESS;	
	
	//	Ok, we have Attributes somewhere.  Dig through 'em all.
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		Attribute *pSource = (Attribute*)geEntity_GetUserData(pEntity);
		READDATA(&pSource->alive, sizeof(geBoolean), 1, RestoreFD);
		READDATA(&pSource->active, sizeof(geBoolean), 1, RestoreFD);
		READDATA(&pSource->Tick, sizeof(geFloat), 1, RestoreFD);
		READDATA(&pSource->CallBack, sizeof(geBoolean), 1, RestoreFD);
		READDATA(&pSource->CallBackCount, sizeof(int), 1, RestoreFD);
		READDATA(&pSource->bState, sizeof(geBoolean), 1, RestoreFD);
// changed RF064
		if(pSource->active==GE_TRUE)
		{
			pSource->Actor = CCD->ActorManager()->SpawnActor(pSource->szActorName, 
				pSource->origin, pSource->ActorRotation, "", "", NULL);//pSource->Actor);
			CCD->ActorManager()->SetType(pSource->Actor, ENTITY_ATTRIBUTE_MOD);
			CCD->ActorManager()->SetScale(pSource->Actor, pSource->Scale);
			CCD->ActorManager()->Position(pSource->Actor, pSource->origin);
			if(pSource->Gravity)
				CCD->ActorManager()->SetGravity(pSource->Actor, CCD->Player()->GetGravity());
			CCD->ActorManager()->SetActorDynamicLighting(pSource->Actor, pSource->FillColor, pSource->AmbientColor);
			CCD->ActorManager()->SetShadow(pSource->Actor, pSource->ShadowSize);
			CCD->ActorManager()->SetHideRadar(pSource->Actor, pSource->HideFromRadar);
			if(!EffectC_IsStringNull(pSource->ChangeMaterial))
				CCD->ActorManager()->ChangeMaterial(pSource->Actor, pSource->ChangeMaterial);
		}
// end change RF064
	}
	
	return RGF_SUCCESS;
}
// end change RF063

//	******************** CRGF Overrides ********************

//	LocateEntity
//
//	Given a name, locate the desired item in the currently loaded level
//	..and return it's user data.

int CAttribute::LocateEntity(char *szName, void **pEntityData)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	//	Ok, check to see if there are Attributes in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "Attribute");
	
	if(!pSet) 
		return RGF_NOT_FOUND;									// No static entity proxies
	
	//	Ok, we have static entity proxies.  Dig through 'em all.
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		Attribute *pTheEntity = (Attribute*)geEntity_GetUserData(pEntity);
		if(strcmp(pTheEntity->szEntityName, szName) == 0)
		{
			*pEntityData = (void *)pTheEntity;
			return RGF_SUCCESS;
		}
	}
	
	return RGF_NOT_FOUND;								// Sorry, no such entity here
}

//	ReSynchronize
//
//	Correct internal timing to match current time, to make up for time lost
//	..when outside the game loop (typically in "menu mode").

int CAttribute::ReSynchronize()
{
	
	return RGF_SUCCESS;
}
