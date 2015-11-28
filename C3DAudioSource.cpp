/*
	C3DAudioSource.cpp:		3D Audio Source class implementation

	(c) 2001 Ralph Deane
	All Rights Reserved

	This file contains the class implementation for 3D audio source
handling.
*/

//	Include the One True Header

#include "RabidFramework.h"

extern geSound_Def *SPool_Sound(char *SName);

//	Constructor
//
//	Load up all 3D audio sources, load and initialize sounds and set the
//	..entities to default values.

C3DAudioSource::C3DAudioSource()
{
   geEntity_EntitySet *pSet;
   geEntity *pEntity;
   
  	Count = 0;
   
   //	Ok, check to see if there are 3D audio sources in this world
   
   pSet = geWorld_GetEntitySet(CCD->World(), "AudioSource3D");
   
   if(!pSet) 
      return;									// No 3D audio sources
   
   //	Ok, we have 3D audio sources somewhere.  Dig through 'em all.
   
   for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
   pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
   {
      AudioSource3D *pSource = (AudioSource3D*)geEntity_GetUserData(pEntity);
      if(EffectC_IsStringNull(pSource->szEntityName))
      {
         char szName[128];
         geEntity_GetName(pEntity, szName, 128);
         pSource->szEntityName = szName;
      }
      Count++;								// Kick source count
      // Ok, put this entity into the Global Entity Registry
      CCD->EntityRegistry()->AddEntity(pSource->szEntityName, "AudioSource3D");
      pSource->OriginOffset = pSource->origin;
      if(pSource->Model)
      {
         geVec3d ModelOrigin;
         geWorld_GetModelRotationalCenter(CCD->World(), pSource->Model, &ModelOrigin);
         geVec3d_Subtract(&pSource->origin, &ModelOrigin, &pSource->OriginOffset);
      }
      // Reset all the data for each 3D audio source
      pSource->effect = (int *)malloc(sizeof(int) * 1);
      pSource->effect[0] = -1;
      pSource->active=false;
      if(!EffectC_IsStringNull(pSource->szSoundFile))
      {
         SPool_Sound(pSource->szSoundFile);
		 // changed QD 02/12/2005
		 // set the entity active if there's no trigger
		 if(EffectC_IsStringNull(pSource->TriggerName))
		 {
			 pSource->effect[0] = Create(pSource->origin, pSource->szSoundFile, pSource->fRadius, pSource->bLoopSound);
			 pSource->active=true;
		 }
		 // end change
      }
      //Start Aug2003DCS
      pSource->ProgrammedTrigger = GE_FALSE;
      //End Aug2003DCS
	}
   
   
   //	Audio loaded and running, let's go!
   
   return;
}

//Start Aug2003DCS   Added Looping
int C3DAudioSource::Create(geVec3d Origin, char *SoundFile, float radius, geBoolean Looping)
//End Aug2003DCS
{
   int effect = -1;
   Snd Sound;
   memset( &Sound, 0, sizeof( Sound ) );
   geVec3d_Copy( &(Origin), &( Sound.Pos ) );
   Sound.Min=radius;
   //Start Aug2003DCS
   //    Sound.Loop=GE_TRUE;
   Sound.Loop=Looping;
   //End Aug2003DCS
   Sound.SoundDef=SPool_Sound(SoundFile);
   if(!Sound.SoundDef)
   {
      char szError[256];
      sprintf(szError,"Can't open audio file '%s'\n", SoundFile);
      CCD->ReportError(szError, false);
   }
   if(Sound.SoundDef!=NULL)
      effect = CCD->EffectManager()->Item_Add(EFF_SND, (void *)&Sound);
   return effect;
}


//	Destructor
//
//	Clean up all audio we have loaded.

C3DAudioSource::~C3DAudioSource()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

  	if(Count == 0)
	  return;						// Don't waste CPU cycles

//	Ok, check to see if there are 3D audio sources in this world

	pSet = geWorld_GetEntitySet(CCD->World(), "AudioSource3D");

	if(!pSet) 
		return;									// No sources

//	Ok, we have 3D audio sources somewhere.  Dig through 'em all and release
//	..the audio (if any).

	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	    pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		AudioSource3D *pSource = (AudioSource3D*)geEntity_GetUserData(pEntity);
		free(pSource->effect);
	}

//	3D audio sources cleaned up.  Bail this mess.

  return;
}

//	Tick
//

void C3DAudioSource::Tick(float dwTicks)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	if(Count == 0)
		return;						// Don't waste CPU cycles

//	Ok, check to see if there are 3D audio sources in this world

	pSet = geWorld_GetEntitySet(CCD->World(), "AudioSource3D");

	if(!pSet) 
		return;									// No sources

//	Ok, we have 3D audio sources somewhere.  Dig through 'em all.

	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	    pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		AudioSource3D *pSource = (AudioSource3D*)geEntity_GetUserData(pEntity);
		
		if(!EffectC_IsStringNull(pSource->TriggerName))
		{
        //Start Aug2003DCS
			if( ((strcmp(pSource->TriggerName,"*Programmed*") == 0) && pSource->ProgrammedTrigger) ||
             ((strcmp(pSource->TriggerName,"*Programmed*") != 0) && GetTriggerState(pSource->TriggerName)) )
			{
				if(pSource->active==false)
				{
					pSource->effect[0] = Create(pSource->origin, pSource->szSoundFile, pSource->fRadius, pSource->bLoopSound);
					pSource->active=true;
					if (!pSource->bLoopSound)
						pSource->ProgrammedTrigger = GE_FALSE;
				}
			}
			else
			{
				if (pSource->bLoopSound)
				{
					if(pSource->effect[0]!=-1)
					{	
    					CCD->EffectManager()->Item_Delete(EFF_SND, pSource->effect[0]);
						pSource->effect[0]=-1;
					}
					pSource->active=false;
				}
				else
				{
					if ((pSource->effect[0]!=-1) && !CCD->EffectManager()->Item_Alive(pSource->effect[0]))
					{
    					CCD->EffectManager()->Item_Delete(EFF_SND, pSource->effect[0]);
						pSource->effect[0]=-1;
					}
					pSource->active=false;
				}
			}
        //End Aug2003DCS
		}
		
		if(pSource->active==GE_TRUE)
		{
			pSource->origin = pSource->OriginOffset;
			if(SetOriginOffset(pSource->EntityName, pSource->BoneName, pSource->Model, &(pSource->origin)))
			{
				if(pSource->effect[0]!=-1)
				{
					Snd Sound;
					geVec3d_Copy( &(pSource->origin), &(Sound.Pos) );
      				CCD->EffectManager()->Item_Modify(EFF_SND, pSource->effect[0], (void *)&Sound, SND_POS);
				}
			} 
		}
	}

  return;
}

//Start Aug2003DCS
//	SetProgrammedTrigger
//
//	Given a name, locate the desired item in the currently loaded level
//	..and set it's ProgrammedTrigger boolean.

int C3DAudioSource::SetProgrammedTrigger(char *szName, geBoolean Flag)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	//	Ok, check to see if there are 3D Audio Sources in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "AudioSource3D");
	
	if(!pSet) 
		return RGF_NOT_FOUND;									// No 3D audio sources
	
	//	Ok, we have 3D audio sources.  Dig through 'em all.
	
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet,NULL); pEntity; 
		pEntity=geEntity_EntitySetGetNextEntity(pSet,pEntity)) 
	{
		AudioSource3D *pSource = (AudioSource3D*)geEntity_GetUserData(pEntity);
		if(strcmp(pSource->szEntityName, szName) == 0)
		{
			pSource->ProgrammedTrigger = Flag;
			return RGF_SUCCESS;
		}
	}
	
	return RGF_NOT_FOUND;								// Sorry, no such entity here
}

//	IsPlaying
//
//	Given a name, locate the desired sound in the currently loaded level
//	..and return true if it is playing, false if not.

geBoolean C3DAudioSource::IsPlaying(char *szName)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	//	Ok, check to see if there are 3D Audio Sources in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "AudioSource3D");
	
	if(!pSet) 
		return GE_FALSE;									// No 3D audio sources
	
	//	Ok, we have 3D audio sources.  Dig through 'em all.
	
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet,NULL); pEntity; pEntity=geEntity_EntitySetGetNextEntity(pSet,pEntity)) 
	{
		AudioSource3D *pSource = (AudioSource3D*)geEntity_GetUserData(pEntity);
		if(strcmp(pSource->szEntityName, szName) == 0)
		{
			if (pSource->effect[0]!=-1)
				return GE_TRUE;
			else
				return GE_FALSE;
		}
	}
	
	return GE_FALSE;								// Sorry, no such entity here
}
//End Aug2003DCS


//	******************** CRGF Overrides ********************

//	LocateEntity
//
//	Given a name, locate the desired item in the currently loaded level
//	..and return it's user data.

int C3DAudioSource::LocateEntity(char *szName, void **pEntityData)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

//	Ok, check to see if there are 3D audio sources in this world

	pSet = geWorld_GetEntitySet(CCD->World(), "AudioSource3D");

	if(!pSet) 
		return RGF_NOT_FOUND;									// No 3D audio sources

//	Ok, we have 3D audio sources somewhere.  Dig through 'em all.

	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	    pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		AudioSource3D *pSource = (AudioSource3D*)geEntity_GetUserData(pEntity);
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

int C3DAudioSource::ReSynchronize()
{
	return RGF_SUCCESS;
}

