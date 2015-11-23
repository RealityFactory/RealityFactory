/*
	CFirePoint.cpp:		FirePoint handler

	(c) 1999 Ralph Deane

	This file contains the class implementation for the CFirePoint
class that encapsulates all weapon firing for
RGF-based games.
*/

//	Include the One True Header

#include "RabidFramework.h"

extern geSound_Def *SPool_Sound(char *SName);

//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------

CFirePoint::CFirePoint()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	pSet = geWorld_GetEntitySet(CCD->World(), "FirePoint");

  if(pSet == NULL)
	  return;						// None there.

//	Ok, we have FirePoints somewhere.  Dig through 'em all.

	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	    pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		FirePoint *pFire = (FirePoint*)geEntity_GetUserData(pEntity);
		if(EffectC_IsStringNull(pFire->szEntityName))
		{
			char szName[128];
			geEntity_GetName(pEntity, szName, 128);
			pFire->szEntityName = szName;
		}
		// Ok, put this entity into the Global Entity Registry
		CCD->EntityRegistry()->AddEntity(pFire->szEntityName, "FirePoint");
		pFire->OriginOffset = pFire->origin;
        if(pFire->Model)
		{
            geVec3d ModelOrigin;
	    	geWorld_GetModelRotationalCenter(CCD->World(), pFire->Model, &ModelOrigin);
            geVec3d_Subtract(&pFire->origin, &ModelOrigin, &pFire->OriginOffset);
  		}

		pFire->active=false;
		pFire->Tick = 0.0f;
		pFire->FireRate = EffectC_Frand(pFire->MinFireRate, pFire->MaxFireRate);
		pFire->Angle.X = -0.0174532925199433f*pFire->Angle.X;
		pFire->Angle.Y = 0.0174532925199433f*(pFire->Angle.Y-90.0f);
		pFire->Angle.Z = 0.0174532925199433f*pFire->Angle.Z;
		pFire->RealAngle = pFire->Angle;
		if(!EffectC_IsStringNull(pFire->szSoundFile))
			SPool_Sound(pFire->szSoundFile);
	}

  return;
}

//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------

CFirePoint::~CFirePoint()
{
  return;
}

//--------------------------------------------------------------------
// Tick
//--------------------------------------------------------------------

void CFirePoint::Tick(float dwTicks)
{
  geEntity_EntitySet *	Set;
  geEntity *		Entity;

  if(CCD->World() == NULL)
	return;

  Set = geWorld_GetEntitySet(CCD->World(), "FirePoint");
  if(Set == NULL)
    return;

	Entity = geEntity_EntitySetGetNextEntity(Set, NULL);
	while	(Entity)
	{
		FirePoint *	Fire;

		Fire = (FirePoint *)geEntity_GetUserData(Entity);
		if(!EffectC_IsStringNull(Fire->TriggerName))
		{
			if(GetTriggerState(Fire->TriggerName))
			{
				if(Fire->active==false)
				{
					Fire->Tick=Fire->FireRate;
					Fire->active=true;
				}
			}
			else
			{
				Fire->active=false;
			}
		}
		else
		{
			if(Fire->active==false)
			{
				Fire->Tick=Fire->FireRate;
				Fire->active=true;
			}
		}
		if(Fire->active==GE_TRUE)
		{
            Fire->origin = Fire->OriginOffset;
			SetOriginOffset(Fire->EntityName, Fire->BoneName, Fire->Model, &(Fire->origin));
			if(Fire->Model)
			{
				geXForm3d Xf;
				geVec3d  Tmp;
				geWorld_GetModelXForm(CCD->World(), Fire->Model, &Xf); 
				geXForm3d_GetEulerAngles(&Xf, &Tmp);
				geVec3d_Add(&Fire->Angle, &Tmp, &Fire->RealAngle);
				Fire->RealAngle.Z=0.0f;
			}
			else if(!EffectC_IsStringNull(Fire->EntityName))
			{
				SetAngle(Fire->EntityName, Fire->BoneName, &Fire->RealAngle);
				Fire->RealAngle.Z=0.0f;
			}

			Fire->Tick+=(dwTicks*0.001f);
			if(Fire->Tick>=Fire->FireRate)
			{
				Fire->Tick=0.0f;
// changed RF063
				CCD->Weapons()->Add_Projectile(Fire->origin, Fire->origin, Fire->RealAngle, Fire->Projectile, Fire->Attribute, Fire->AltAttribute);
// end change RF063
				Fire->FireRate = EffectC_Frand(Fire->MinFireRate, Fire->MaxFireRate);
				if(!EffectC_IsStringNull(Fire->szSoundFile))
				{
					Snd Sound;
					memset( &Sound, 0, sizeof( Sound ) );
					geVec3d_Copy( &(Fire->origin), &( Sound.Pos ) );
					Sound.Min=CCD->GetAudibleRadius();
					Sound.Loop=false;
					Sound.SoundDef = SPool_Sound(Fire->szSoundFile);
					CCD->EffectManager()->Item_Add(EFF_SND, (void *)&Sound);
				}
			}
		}

		Entity = geEntity_EntitySetGetNextEntity(Set, Entity);
	}

	return;
}

//	******************** CRGF Overrides ********************

//	LocateEntity
//
//	Given a name, locate the desired item in the currently loaded level
//	..and return it's user data.

int CFirePoint::LocateEntity(char *szName, void **pEntityData)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

//	Ok, check to see if there are FirePoints in this world

	pSet = geWorld_GetEntitySet(CCD->World(), "FirePoint");

	if(!pSet) 
		return RGF_NOT_FOUND;

//	Ok, we have FirePoints.  Dig through 'em all.

	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	    pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
		{
		FirePoint *pTheEntity = (FirePoint*)geEntity_GetUserData(pEntity);
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

int CFirePoint::ReSynchronize()
{

	return RGF_SUCCESS;
}
