/*
	CFloating.cpp:		Floating Particle handler

	(c) 1999 Ralph Deane

	This file contains the class implementation for the CFloat
class that encapsulates floating particle based special effects for
RGF-based games.
*/

//	Include the One True Header

#include "RabidFramework.h"

extern geBitmap *TPool_Bitmap(char *DefaultBmp, char *DefaultAlpha, char *BName, char *AName);

//--------------------------------------------------------------------
// CFloat Constructor
//--------------------------------------------------------------------

CFloat::CFloat()
{
  geEntity_EntitySet *	Set;
  geEntity *		Entity;
  int i;

  // test for any floating particle entities
  Set = geWorld_GetEntitySet(CCD->World(), "FloatingParticles");
  // there is none
  if (Set == NULL)
    return;

  // get the first entity
  Entity = geEntity_EntitySetGetNextEntity(Set, NULL);

  // wade thru all of them
  while	(Entity)
  {
    FloatingParticles *	S;
    Sprite	Spr;

    // get data
    S = (FloatingParticles *)geEntity_GetUserData(Entity);
	if(EffectC_IsStringNull(S->szEntityName))
	{
		char szName[128];
		geEntity_GetName(Entity, szName, 128);
		S->szEntityName = szName;
	}
	// Ok, put this entity into the Global Entity Registry
	CCD->EntityRegistry()->AddEntity(S->szEntityName, "FloatingParticles");
	S->OriginOffset = S->origin;
	if(S->Model)
	{
		geVec3d ModelOrigin;
		geWorld_GetModelRotationalCenter(CCD->World(), S->Model, &ModelOrigin);
		geVec3d_Subtract(&S->origin, &ModelOrigin, &S->OriginOffset);
  	}
    // get total number of sprites to use
    S->EffectCount=S->ParticleCount;

    // create dynamic array to hold index numbers
    S->EffectList = (int *)malloc(sizeof(int) * S->EffectCount);
    // create dynamic xform array
    S->Xf = (geXForm3d *)calloc( S->ParticleCount * sizeof ( *( S->Xf ) ) ,1);
    // create dynamic speed array
    S->Speed = (float *)calloc( S->ParticleCount * sizeof( *( S->Speed ) ) ,1);

    // get the bitmap used as the sprite
    // g_bubble/A__bubl are default bitmap names
    S->Bitmap=TPool_Bitmap("g_bubble.Bmp", "A_bubl.Bmp", S->BmpName, S->AlphaName);
    if( S->Color.r < 0.0f )
      S->Color.r = 0.0f;
    if( S->Color.g < 0.0f )
      S->Color.g = 0.0f;
    if( S->Color.b < 0.0f )
      S->Color.b = 0.0f;

    // clear out sprite data
    memset( &Spr, 0, sizeof( Spr ) );
    Spr.Texture = &S->Bitmap;
    Spr.Color.r = S->Color.r;
    Spr.Color.g = S->Color.g;
    Spr.Color.b = S->Color.b;
    Spr.Color.a = 255.0f;
	Spr.Pause = GE_TRUE;
    // only one sprite so don't cycle at all
    Spr.TotalTextures = 1;
    Spr.Style = SPRITE_CYCLE_NONE;
    // set sprite scale
    Spr.Scale = S->Scale;
	S->active = false;

    // set sprite position
	S->origin = S->OriginOffset;
	SetOriginOffset(S->EntityName, S->BoneName, S->Model, &(S->origin));
    geVec3d_Copy( &( S->origin ), &( S->BasePos ) );

    // do for required number of sprites
    for ( i = 0; i < S->EffectCount; i++ )
    {
		// set random direction
		geXForm3d_SetIdentity( &( S->Xf[i] ) );
		geXForm3d_RotateX( &( S->Xf[i] ), EffectC_Frand( -S->XSlant, S->XSlant ) );
		geXForm3d_RotateZ( &( S->Xf[i] ), EffectC_Frand( -S->ZSlant, S->ZSlant ) );

		// pick random start spot
		geVec3d_Copy( &( S->BasePos ), &( S->Xf[i].Translation ) );
		S->Xf[i].Translation.X += EffectC_Frand( -( S->Radius / 2.0f ), S->Radius / 2.0f );
		S->Xf[i].Translation.Z += EffectC_Frand( -( S->Radius / 2.0f ), S->Radius / 2.0f );
		geVec3d_Copy( &( S->Xf[i].Translation ), &( Spr.Pos ) );

		// set speed
		S->Speed[i] = EffectC_Frand( S->MinSpeed, S->MaxSpeed );

		// add effect to manager
        S->EffectList[i] = CCD->EffectManager()->Item_Add(EFF_SPRITE, (void *)&Spr);
    }

    // process next entity
    Entity = geEntity_EntitySetGetNextEntity(Set, Entity);
  }

  return;
}


//--------------------------------------------------------------------
// CFloat  Destructor
//--------------------------------------------------------------------

CFloat::~CFloat()
{
  geEntity_EntitySet *	Set;
  geEntity *		Entity;

  Set = geWorld_GetEntitySet(CCD->World(), "FloatingParticles");
  if (Set == NULL)
    return;

  Entity = geEntity_EntitySetGetNextEntity(Set, NULL);

  while	(Entity)
  {
    FloatingParticles *	R;

    R = (FloatingParticles *)geEntity_GetUserData(Entity);
    // free any dynamic arrays
    free(R->EffectList);
    if ( R->Xf != NULL )
	free( R->Xf );
    if ( R->Speed != NULL )
	free( R->Speed );

    Entity = geEntity_EntitySetGetNextEntity(Set, Entity);
  }
  return;
}


void CFloat::Tick(float dwTicksIn)
{
  geEntity_EntitySet *	Set;
  geEntity *		Entity;
  int i;

  // get amount of time since last call
  float dwTicks = (float)(dwTicksIn)*0.001f;

  Set = geWorld_GetEntitySet(CCD->World(), "FloatingParticles");
  if (Set == NULL)
    return;

  // wade thru all entities
  Entity = geEntity_EntitySetGetNextEntity(Set, NULL);

  while	(Entity)
  {
    FloatingParticles *	S;
    geVec3d Movement;
    geVec3d  Pos;
    geVec3d  Offset;
    Sprite  Spr;

    // get data
    S = (FloatingParticles *)geEntity_GetUserData(Entity);

	if(!EffectC_IsStringNull(S->TriggerName))
		S->active = GetTriggerState(S->TriggerName);
	else
		S->active=true;

	S->origin = S->OriginOffset;
	SetOriginOffset(S->EntityName, S->BoneName, S->Model, &(S->origin));
	geVec3d_Subtract( &(S->origin), &( S->BasePos ), &Movement );
	geVec3d_Copy( &(S->origin), &( S->BasePos ) );

    // modify all sprites
    for ( i = 0; i < S->EffectCount; i++ )
    {
		if(S->active==GE_TRUE)
		{
			// adjust this particles orientation
			geVec3d_Copy( &( S->Xf[i].Translation ), &Pos );
			geVec3d_Set( &( S->Xf[i].Translation ), 0.0f, 0.0f, 0.0f );
			geXForm3d_RotateY( &( S->Xf[i] ), dwTicks * 5.0f );
			geVec3d_Copy( &Pos, &( S->Xf[i].Translation ) );

			// adjust particle position
			geXForm3d_GetUp( &( S->Xf[i] ), &Offset );
			geVec3d_AddScaled( &( S->Xf[i].Translation ), &Offset, dwTicks * S->Speed[i], &( S->Xf[i].Translation ) );
			geVec3d_Add( &( S->Xf[i].Translation ), &Movement, &( S->Xf[i].Translation ) );
			geVec3d_Copy( &( S->Xf[i].Translation ), &( Spr.Pos ) );

			// reset particle if it has hit its height limit
			if ( ( S->Xf[i].Translation.Y - S->BasePos.Y ) > S->Height )
			{
  				// set random direction
				geXForm3d_SetIdentity( &( S->Xf[i] ) );
				geXForm3d_RotateX( &( S->Xf[i] ), EffectC_Frand( -S->XSlant, S->XSlant ) );
				geXForm3d_RotateZ( &( S->Xf[i] ), EffectC_Frand( -S->ZSlant, S->ZSlant ) );

				// pick random start spot
				geVec3d_Copy( &( S->BasePos ), &( S->Xf[i].Translation ) );
				S->Xf[i].Translation.X += EffectC_Frand( -( S->Radius / 2.0f ), S->Radius / 2.0f );
				S->Xf[i].Translation.Z += EffectC_Frand( -( S->Radius / 2.0f ), S->Radius / 2.0f );
				geVec3d_Copy( &( S->Xf[i].Translation ), &( Spr.Pos ) );

				// set speed
				S->Speed[i] = EffectC_Frand( S->MinSpeed, S->MaxSpeed );
			}

			// modify sprite in effect manager
			Spr.Pause = GE_FALSE;
			CCD->EffectManager()->Item_Modify(EFF_SPRITE, S->EffectList[i], (void *)&Spr, SPRITE_POS | SPRITE_PAUSE);
		}
		else
		{
			Spr.Pause = GE_TRUE;
			// modify sprite in effect manager
			CCD->EffectManager()->Item_Modify(EFF_SPRITE, S->EffectList[i], (void *)&Spr, SPRITE_PAUSE);
		}
    }

    // get next entity
    Entity = geEntity_EntitySetGetNextEntity(Set, Entity);
  }

}


//	******************** CRGF Overrides ********************

//	LocateEntity
//
//	Given a name, locate the desired item in the currently loaded level
//	..and return it's user data.

int CFloat::LocateEntity(char *szName, void **pEntityData)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

//	Ok, check to see if there are Floating Particles in this world

	pSet = geWorld_GetEntitySet(CCD->World(), "FloatingParticles");

	if(!pSet) 
		return RGF_NOT_FOUND;

//	Ok, we have Floating Particles somewhere.  Dig through 'em all.

	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	    pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
		{
		FloatingParticles *pSource = (FloatingParticles*)geEntity_GetUserData(pEntity);
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

int CFloat::ReSynchronize()
{
	return RGF_SUCCESS;
}

