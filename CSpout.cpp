
//	Include the One True Header

#include "RabidFramework.h"

extern geBitmap *TPool_Bitmap(char *DefaultBmp, char *DefaultAlpha, char *BName, char *AName);

//--------------------------------------------------------------------
// CSpout Constructor
//--------------------------------------------------------------------

CSpout::CSpout()
{
  geEntity_EntitySet *	Set;
  geEntity *		Entity;


  Set = geWorld_GetEntitySet(CCD->World(), "Spout");
  if (Set == NULL)
    return;

  Entity = geEntity_EntitySetGetNextEntity(Set, NULL);

  while	(Entity)
  {
    Spout *	S;

    S = (Spout *)geEntity_GetUserData(Entity);
	if(EffectC_IsStringNull(S->szEntityName))
	{
		char szName[128];
		geEntity_GetName(Entity, szName, 128);
		S->szEntityName = szName;
	}
	// Ok, put this entity into the Global Entity Registry
	CCD->EntityRegistry()->AddEntity(S->szEntityName, "Spout");
	S->OriginOffset = S->origin;
	if(S->Model)
	{
		geVec3d ModelOrigin;
		geWorld_GetModelRotationalCenter(CCD->World(), S->Model, &ModelOrigin);
		geVec3d_Subtract(&S->origin, &ModelOrigin, &S->OriginOffset);
  	}
    if(S->MinPauseTime < 0.0f)
      S->MinPauseTime = 0.0f;
    if(S->MaxPauseTime < S->MinPauseTime)
      S->MaxPauseTime = S->MinPauseTime;
    if (S->MaxPauseTime > 0.0f)
	S->PauseTime = EffectC_Frand( S->MinPauseTime, S->MaxPauseTime );
	TPool_Bitmap("lvsmoke.Bmp", "A_lvsmoke.Bmp", S->BmpName, S->AlphaName);
	S->active=false;
	S->EffectList = -1;

    Entity = geEntity_EntitySetGetNextEntity(Set, Entity);
  }

  return;
}

int CSpout::Create(Spout *S)
{
	int effect = -1;
    Spray	Sp;
	geVec3d	In;
    geXForm3d	Xf;

	memset( &Sp, 0, sizeof( Sp ) );
    Sp.Texture=TPool_Bitmap("lvsmoke.Bmp", "A_lvsmoke.Bmp", S->BmpName, S->AlphaName);
    Sp.SourceVariance = S->SourceVariance;
    if(Sp.SourceVariance < 0 )
      Sp.SourceVariance = 0;
    Sp.DestVariance = S->DestVariance;
    if(Sp.DestVariance < 0 )
      Sp.DestVariance = 0;
    Sp.MinScale = S->MinScale;
    Sp.MaxScale = S->MaxScale;
	Sp.Bounce = S->Bounce;
    if(Sp.MinScale <= 0.0f )
      Sp.MinScale = 0.1f;
    if(Sp.MaxScale < Sp.MinScale )
      Sp.MaxScale = Sp.MinScale;
    Sp.MinSpeed = S->MinSpeed;
    Sp.MaxSpeed = S->MaxSpeed;
    if(Sp.MinSpeed < 0.0f )
      Sp.MinSpeed = 0.0f;
    if(Sp.MaxSpeed < Sp.MinSpeed )
      Sp.MaxSpeed = Sp.MinSpeed;
    Sp.MinUnitLife = S->MinUnitLife;
    Sp.MaxUnitLife = S->MaxUnitLife;
    if(Sp.MinUnitLife <= 0.0f )
      Sp.MinUnitLife = 0.1f;
    if(Sp.MaxUnitLife < Sp.MinUnitLife )
      Sp.MaxUnitLife = Sp.MinUnitLife;
    Sp.ColorMin.r = S->ColorMin.r;
    if(Sp.ColorMin.r < 0.0f )
      Sp.ColorMin.r = 0.0f;
    if(Sp.ColorMin.r > 255.0f )
      Sp.ColorMin.r = 255.0f;
    Sp.ColorMax.r = S->ColorMax.r;
    if(Sp.ColorMax.r < Sp.ColorMin.r )
      Sp.ColorMax.r = Sp.ColorMin.r;   
    Sp.ColorMin.g = S->ColorMin.g;
    if(Sp.ColorMin.g < 0.0f )
      Sp.ColorMin.g = 0.0f;
    if(Sp.ColorMin.g > 255.0f )
      Sp.ColorMin.g = 255.0f;
    Sp.ColorMax.g = S->ColorMax.g;
    if(Sp.ColorMax.g < Sp.ColorMin.g )
      Sp.ColorMax.g = Sp.ColorMin.g;
    Sp.ColorMin.b = S->ColorMin.b;
    if(Sp.ColorMin.b < 0.0f )
      Sp.ColorMin.b = 0.0f;
    if(Sp.ColorMin.b > 255.0f )
      Sp.ColorMin.b = 255.0f;
    Sp.ColorMax.b = S->ColorMax.b;
    if(Sp.ColorMax.b < Sp.ColorMin.b )
      Sp.ColorMax.b = Sp.ColorMin.b;
    Sp.ColorMin.a = 255.0f;
    Sp.ColorMax.a = 255.0f;
    Sp.Rate = S->ParticleCreateRate;
    if(Sp.Rate < 0.0f )
      Sp.Rate = 0.1f;
    geVec3d_Copy( &( S->Gravity ), &( Sp.Gravity ) );

    // setup orientation
    geXForm3d_SetIdentity( &( Xf ) );
    geXForm3d_RotateX( &( Xf ), S->Angles.X / 57.3f );  
    geXForm3d_RotateY( &( Xf ), ( S->Angles.Y - 90.0f ) / 57.3f );  
    geXForm3d_RotateZ( &( Xf ), S->Angles.Z / 57.3f ); 
    geVec3d_Copy( &( S->origin ), &( Sp.Source ) );
    geXForm3d_GetIn( &( Xf ), &In );
    geVec3d_Inverse( &In );
    geVec3d_AddScaled( &( Sp.Source ), &In, 50.0f, &( Sp.Dest ) );

    effect = CCD->EffectManager()->Item_Add(EFF_SPRAY, (void *)&Sp);
	return effect;
}

//--------------------------------------------------------------------
// CSpout  Destructor
//--------------------------------------------------------------------

CSpout::~CSpout()
{
  return;
}


void CSpout::Tick(float dwTicksIn)
{
  geEntity_EntitySet *	Set;
  geEntity *		Entity;

  float dwTicks = (float)(dwTicksIn);

  Set = geWorld_GetEntitySet(CCD->World(), "Spout");
  if (Set == NULL)
    return;

  Entity = geEntity_EntitySetGetNextEntity(Set, NULL);

  while	(Entity)
  {
    Spout *	S;

    S = (Spout *)geEntity_GetUserData(Entity);

	if(!EffectC_IsStringNull(S->TriggerName))
	{
		if(GetTriggerState(S->TriggerName))
		{
			if(S->active==false && S->TotalLife != -1.0f)
			{
				S->EffectList = Create(S);
				S->active=true;
			}
		}
		else
		{
			if(S->active==GE_TRUE)
			{
				if(S->EffectList!=-1)
					CCD->EffectManager()->Item_Delete(EFF_SPRAY, S->EffectList);
				S->EffectList=-1;
				S->active=false;
			}
		}
	}
	else
	{
		if(S->active==false && S->TotalLife != -1.0f)
		{
			S->EffectList = Create(S);
			S->active=true;
		}
	}
	if(S->active==GE_TRUE)
	{
		// kill the effect if its time has run out
		if ( S->TotalLife > 0.0f )
		{
			S->TotalLife -= (dwTicks*0.001f);
			if ( S->TotalLife <= 0.0f )
			{
				S->TotalLife = -1.0f;
				CCD->EffectManager()->Item_Delete(EFF_SPRAY, S->EffectList);
				S->EffectList=-1;
				S->active=false;
				continue;
			}
		}
        else
        {
			// adjust pause time
			if ( S->PauseTime > 0.0f )
			{
				S->PauseTime -= (dwTicks*0.001f);
				if ( S->PauseTime <= 0.0f )
				{
					S->PauseTime = EffectC_Frand( S->MinPauseTime, S->MaxPauseTime );
					S->PauseState = !S->PauseState;
					CCD->EffectManager()->Item_Pause(EFF_SPRAY, S->EffectList, S->PauseState);
				}
			}
        }
		S->origin = S->OriginOffset;
		if(SetOriginOffset(S->EntityName, S->BoneName, S->Model, &(S->origin)))
		{
			// setup orientation
			geVec3d	In;
			geXForm3d	Xf;
			Spray		Sp;

			geXForm3d_SetIdentity( &( Xf ) );
			geXForm3d_RotateX( &( Xf ), S->Angles.X / 57.3f );  
			geXForm3d_RotateY( &( Xf ), ( S->Angles.Y - 90.0f ) / 57.3f );  
			geXForm3d_RotateZ( &( Xf ), S->Angles.Z / 57.3f ); 
			geVec3d_Copy( &( S->origin ), &( Sp.Source ) );
			geXForm3d_GetIn( &( Xf ), &In );
			geVec3d_Inverse( &In );
			geVec3d_AddScaled( &( Sp.Source ), &In, 50.0f, &( Sp.Dest ) );

			CCD->EffectManager()->Item_Modify(EFF_SPRAY, S->EffectList, (void *)&Sp, SPRAY_SOURCE | SPRAY_ACTUALDEST);
		}
	} 
    Entity = geEntity_EntitySetGetNextEntity(Set, Entity);
  }
}



//	******************** CRGF Overrides ********************

//	LocateEntity
//
//	Given a name, locate the desired item in the currently loaded level
//	..and return it's user data.

int CSpout::LocateEntity(char *szName, void **pEntityData)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

//	Ok, check to see if there are Spout in this world

	pSet = geWorld_GetEntitySet(CCD->World(), "Spout");

	if(!pSet) 
		return RGF_NOT_FOUND;

//	Ok, we have Spout somewhere.  Dig through 'em all.

	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	    pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
		{
		Spout *pSource = (Spout*)geEntity_GetUserData(pEntity);
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

int CSpout::ReSynchronize()
{
	return RGF_SUCCESS;
}


//--------------------------------------------------------------------
// CActorSpout Constructor
//--------------------------------------------------------------------

CActorSpout::CActorSpout()
{
  geEntity_EntitySet *	Set;
  geEntity *		Entity;


  Set = geWorld_GetEntitySet(CCD->World(), "ActorSpout");
  if (Set == NULL)
    return;

  Entity = geEntity_EntitySetGetNextEntity(Set, NULL);

  while	(Entity)
  {
    ActorSpout *	S;

    S = (ActorSpout *)geEntity_GetUserData(Entity);
	if(EffectC_IsStringNull(S->szEntityName))
	{
		char szName[128];
		geEntity_GetName(Entity, szName, 128);
		S->szEntityName = szName;
	}
	// Ok, put this entity into the Global Entity Registry
	CCD->EntityRegistry()->AddEntity(S->szEntityName, "ActorSpout");
	S->OriginOffset = S->origin;
	if(S->Model)
	{
		geVec3d ModelOrigin;
		geWorld_GetModelRotationalCenter(CCD->World(), S->Model, &ModelOrigin);
		geVec3d_Subtract(&S->origin, &ModelOrigin, &S->OriginOffset);
  	}
    if(S->MinPauseTime < 0.0f)
      S->MinPauseTime = 0.0f;
    if(S->MaxPauseTime < S->MinPauseTime)
      S->MaxPauseTime = S->MinPauseTime;
    if (S->MaxPauseTime > 0.0f)
	S->PauseTime = EffectC_Frand( S->MinPauseTime, S->MaxPauseTime );
	S->active=false;
	S->EffectList = -1;

	char Name[64];
	for(int i=0;i<S->NumActors;i++)
	{
		sprintf(Name, "%s%d%s", S->BaseName, i, ".act" );
		geActor *Actor = CCD->ActorManager()->SpawnActor(Name, 
			S->origin, S->BaseRotation, "", "", NULL);
		if(!Actor)
		{
			char szError[256];
			sprintf(szError,"%s : Missing Actor '%s'", S->szEntityName, Name);
			CCD->ReportError(szError, false);
			CCD->ShutdownLevel();
			delete CCD;
			MessageBox(NULL, szError,"ActorSpout", MB_OK);
			exit(-333);
		}
		CCD->ActorManager()->RemoveActor(Actor);
		geActor_Destroy(&Actor); 
	}

    Entity = geEntity_EntitySetGetNextEntity(Set, Entity);
  }

  return;
}

int CActorSpout::Create(ActorSpout *S)
{
	int effect = -1;
    ActorSpray	Sp;
	geVec3d	In;
    geXForm3d	Xf;

	memset( &Sp, 0, sizeof( Sp ) );
	strcpy(Sp.BaseName, S->BaseName);
	Sp.NumActors = S->NumActors;
	Sp.Style = S->Style;
	Sp.Alpha = S->Alpha;
	Sp.AlphaRate = S->AlphaRate;
	Sp.BaseRotation = S->BaseRotation;
	Sp.MinRotationSpeed.X = 0.0174532925199433f*S->MinRotationSpeed.X;
	Sp.MinRotationSpeed.Y = 0.0174532925199433f*S->MinRotationSpeed.Y;
	Sp.MinRotationSpeed.Z = 0.0174532925199433f*S->MinRotationSpeed.Z;
	Sp.MaxRotationSpeed.X = 0.0174532925199433f*S->MaxRotationSpeed.X;
	Sp.MaxRotationSpeed.Y = 0.0174532925199433f*S->MaxRotationSpeed.Y;
	Sp.MaxRotationSpeed.Z = 0.0174532925199433f*S->MaxRotationSpeed.Z;

    Sp.SourceVariance = S->SourceVariance;
    if(Sp.SourceVariance < 0 )
      Sp.SourceVariance = 0;
    Sp.DestVariance = S->DestVariance;
    if(Sp.DestVariance < 0 )
      Sp.DestVariance = 0;
    Sp.MinScale = S->MinScale;
    Sp.MaxScale = S->MaxScale;
	Sp.Bounce = S->Bounce;
    if(Sp.MinScale <= 0.0f )
      Sp.MinScale = 0.1f;
    if(Sp.MaxScale < Sp.MinScale )
      Sp.MaxScale = Sp.MinScale;
    Sp.MinSpeed = S->MinSpeed;
    Sp.MaxSpeed = S->MaxSpeed;
    if(Sp.MinSpeed < 0.0f )
      Sp.MinSpeed = 0.0f;
    if(Sp.MaxSpeed < Sp.MinSpeed )
      Sp.MaxSpeed = Sp.MinSpeed;
    Sp.MinUnitLife = S->MinUnitLife;
    Sp.MaxUnitLife = S->MaxUnitLife;
    if(Sp.MinUnitLife <= 0.0f )
      Sp.MinUnitLife = 0.1f;
    if(Sp.MaxUnitLife < Sp.MinUnitLife )
      Sp.MaxUnitLife = Sp.MinUnitLife;
    Sp.FillColor = S->FillColor;
    Sp.AmbientColor = S->AmbientColor;
    Sp.Rate = S->ParticleCreateRate;
    if(Sp.Rate < 0.0f )
      Sp.Rate = 0.1f;
	Sp.Gravity = S->Gravity;

    // setup orientation
    geXForm3d_SetIdentity( &( Xf ) );
    geXForm3d_RotateZ( &( Xf ), S->Angles.Z / 57.3f ); 
    geXForm3d_RotateX( &( Xf ), -S->Angles.X / 57.3f );  
    geXForm3d_RotateY( &( Xf ), ( S->Angles.Y - 90.0f ) / 57.3f );  
    geVec3d_Copy( &( S->origin ), &( Sp.Source ) );
    geXForm3d_GetIn( &( Xf ), &In );
    geVec3d_Inverse( &In );
    geVec3d_AddScaled( &( Sp.Source ), &In, 50.0f, &( Sp.Dest ) );

    effect = CCD->EffectManager()->Item_Add(EFF_ACTORSPRAY, (void *)&Sp);
	return effect;
}

//--------------------------------------------------------------------
// CActorSpout  Destructor
//--------------------------------------------------------------------

CActorSpout::~CActorSpout()
{
  return;
}


void CActorSpout::Tick(float dwTicksIn)
{
  geEntity_EntitySet *	Set;
  geEntity *		Entity;

  float dwTicks = (float)(dwTicksIn);

  Set = geWorld_GetEntitySet(CCD->World(), "ActorSpout");
  if (Set == NULL)
    return;

  Entity = geEntity_EntitySetGetNextEntity(Set, NULL);

  while	(Entity)
  {
    ActorSpout *	S;

    S = (ActorSpout *)geEntity_GetUserData(Entity);

	if(!EffectC_IsStringNull(S->TriggerName))
	{
		if(GetTriggerState(S->TriggerName))
		{
			if(S->active==false && S->TotalLife != -1.0f)
			{
				S->EffectList = Create(S);
				S->active=true;
			}
		}
		else
		{
			if(S->active==GE_TRUE)
			{
				if(S->EffectList!=-1)
					CCD->EffectManager()->Item_Delete(EFF_ACTORSPRAY, S->EffectList);
				S->EffectList=-1;
				S->active=false;
			}
		}
	}
	else
	{
		if(S->active==false && S->TotalLife != -1.0f)
		{
			S->EffectList = Create(S);
			S->active=true;
		}
	}
	if(S->active==GE_TRUE)
	{
		// kill the effect if its time has run out
		if ( S->TotalLife > 0.0f )
		{
			S->TotalLife -= (dwTicks*0.001f);
			if ( S->TotalLife <= 0.0f )
			{
				S->TotalLife = -1.0f;
				CCD->EffectManager()->Item_Delete(EFF_ACTORSPRAY, S->EffectList);
				S->EffectList=-1;
				S->active=false;
				continue;
			}
		}
        else
        {
			// adjust pause time
			if ( S->PauseTime > 0.0f )
			{
				S->PauseTime -= (dwTicks*0.001f);
				if ( S->PauseTime <= 0.0f )
				{
					S->PauseTime = EffectC_Frand( S->MinPauseTime, S->MaxPauseTime );
					S->PauseState = !S->PauseState;
					CCD->EffectManager()->Item_Pause(EFF_ACTORSPRAY, S->EffectList, S->PauseState);
				}
			}
        }
		S->origin = S->OriginOffset;
		if(SetOriginOffset(S->EntityName, S->BoneName, S->Model, &(S->origin)))
		{
			// setup orientation
			geVec3d	In;
			geXForm3d	Xf;
			ActorSpray		Sp;

			geXForm3d_SetIdentity( &( Xf ) );
			geXForm3d_RotateX( &( Xf ), S->Angles.X / 57.3f );  
			geXForm3d_RotateY( &( Xf ), ( S->Angles.Y - 90.0f ) / 57.3f );  
			geXForm3d_RotateZ( &( Xf ), S->Angles.Z / 57.3f ); 
			geVec3d_Copy( &( S->origin ), &( Sp.Source ) );
			geXForm3d_GetIn( &( Xf ), &In );
			geVec3d_Inverse( &In );
			geVec3d_AddScaled( &( Sp.Source ), &In, 50.0f, &( Sp.Dest ) );
			CCD->EffectManager()->Item_Modify(EFF_ACTORSPRAY, S->EffectList, (void *)&Sp, SPRAY_SOURCE | SPRAY_ACTUALDEST);
		}
	} 
    Entity = geEntity_EntitySetGetNextEntity(Set, Entity);
  }
}



//	******************** CRGF Overrides ********************

//	LocateEntity
//
//	Given a name, locate the desired item in the currently loaded level
//	..and return it's user data.

int CActorSpout::LocateEntity(char *szName, void **pEntityData)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

//	Ok, check to see if there are Spout in this world

	pSet = geWorld_GetEntitySet(CCD->World(), "ActorSpout");

	if(!pSet) 
		return RGF_NOT_FOUND;

//	Ok, we have Spout somewhere.  Dig through 'em all.

	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	    pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
		{
		ActorSpout *pSource = (ActorSpout*)geEntity_GetUserData(pEntity);
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

int CActorSpout::ReSynchronize()
{
	return RGF_SUCCESS;
}

