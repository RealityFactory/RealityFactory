/*
	CRain.cpp:		Rain handler

	(c) 1999 Ralph Deane

	This file contains the class implementation for the CRain
class that encapsulates rain based special effects for
RGF-based games.
*/

//	Include the One True Header

#include "RabidFramework.h"

#define RAINM_RADIUSTOEFFECTRATIO	50

extern geBitmap *TPool_Bitmap(char *DefaultBmp, char *DefaultAlpha, char *BName, char *AName);

//--------------------------------------------------------------------
// CRain Constructor
//--------------------------------------------------------------------

CRain::CRain()
{
  geEntity_EntitySet *	Set;
  geEntity *		Entity;

  Set = geWorld_GetEntitySet(CCD->World(), "Rain");
  if (Set == NULL)
    return;

  // get first rain entity
  Entity = geEntity_EntitySetGetNextEntity(Set, NULL);

  // wade thru them all
  while	(Entity)
  {
    Rain *	R;

    // get data
    R = (Rain *)geEntity_GetUserData(Entity);
	if(EffectC_IsStringNull(R->szEntityName))
	{
		char szName[128];
		geEntity_GetName(Entity, szName, 128);
		R->szEntityName = szName;
	}
	// Ok, put this entity into the Global Entity Registry
	CCD->EntityRegistry()->AddEntity(R->szEntityName, "Rain");
	R->OriginOffset = R->origin;
	if(R->Model)
	{
		geVec3d ModelOrigin;
		geWorld_GetModelRotationalCenter(CCD->World(), R->Model, &ModelOrigin);
		geVec3d_Subtract(&R->origin, &ModelOrigin, &R->OriginOffset);
  	}
    // calculate number of Sprays needed
    R->EffectCount = (int)R->Radius / RAINM_RADIUSTOEFFECTRATIO;
    // create dynamic array to hold effect indexes
    R->EffectList = (int *)malloc(sizeof(int) * R->EffectCount);

    // make sure data is valid
    if( R->Severity < 0.0f )
      R->Severity=0.0f;
    if( R->Severity > 1.0f )
      R->Severity=1.0f;
    if( R->DropLife <= 0.0f )
      R->DropLife=0.1f;
	R->active = false;
	TPool_Bitmap("rain.Bmp", "A_rain.Bmp", R->BmpName, R->AlphaName);

    // get next entity
    Entity = geEntity_EntitySetGetNextEntity(Set, Entity);
  }
  return;
}

int CRain::Create(Rain *R)
{
	int effect = -1;
    Spray	Sp;

    // clear out spray data
    memset( &Sp, 0, sizeof( Sp ) );

    // get bitmap for use as texture
    // rain/a_rain are default bitmap names
    Sp.Texture=TPool_Bitmap("rain.Bmp", "A_rain.Bmp", R->BmpName, R->AlphaName);

    // setup spray data
    Sp.MinScale = 0.5f;
    Sp.MaxScale = 1.5f;
	Sp.ShowAlways=GE_TRUE;
    Sp.Rate = ( 1.1f - R->Severity ) * 0.1f;
    geVec3d_Copy( &( R->Gravity ), &( Sp.Gravity ) );
    // set source position
    geVec3d_Copy( &( R->origin ), &( Sp.Source ) );
    Sp.SourceVariance = (int)( R->Radius / 2.0f );
    Sp.MinUnitLife = R->DropLife;
    Sp.MaxUnitLife = R->DropLife;
    // set destination position
    geVec3d_AddScaled( &( Sp.Source ), &( Sp.Gravity ), Sp.MinUnitLife, &( Sp.Dest ) );
    Sp.DestVariance = (int)( R->Radius / 2.0f );
    memcpy( &( Sp.ColorMin ), &( R->ColorMin ), sizeof( Sp.ColorMin ) );
    memcpy( &( Sp.ColorMax ), &( R->ColorMax ), sizeof( Sp.ColorMax ) );
    Sp.ColorMin.a = Sp.ColorMax.a = 255.0f;
	effect=CCD->EffectManager()->Item_Add(EFF_SPRAY, (void *)&Sp);
	return effect;
}

//--------------------------------------------------------------------
// CRain  Destructor
//--------------------------------------------------------------------

CRain::~CRain()
{
  geEntity_EntitySet *	Set;
  geEntity *		Entity;

  Set = geWorld_GetEntitySet(CCD->World(), "Rain");
  if (Set == NULL)
    return;

  Entity = geEntity_EntitySetGetNextEntity(Set, NULL);

  while	(Entity)
  {
    Rain *	R;

    R = (Rain *)geEntity_GetUserData(Entity);
    // free dynamic array
    free(R->EffectList);

    Entity = geEntity_EntitySetGetNextEntity(Set, Entity);
  }

  return;
}

//-------------------------------------------------------
// Tick
//-------------------------------------------------------

void CRain::Tick(float dwTicks)
{
  geEntity_EntitySet *	Set;
  geEntity *		Entity;
  int i;
  Spray	Sp;

  Set = geWorld_GetEntitySet(CCD->World(), "Rain");
  if (Set == NULL)
    return;

  Entity = geEntity_EntitySetGetNextEntity(Set, NULL);

  while	(Entity)
  {
    Rain *	R;

    R = (Rain *)geEntity_GetUserData(Entity);

	if(!EffectC_IsStringNull(R->TriggerName))
	{
		if(GetTriggerState(R->TriggerName))
		{
			if(R->active==false)
			{
				for ( i = 0; i < R->EffectCount; i++ )
					R->EffectList[i]=Create(R);
				R->active=true;
			}
		}
		else
		{
			if(R->active==GE_TRUE)
			{
				for ( i = 0; i < R->EffectCount; i++ )
					CCD->EffectManager()->Item_Delete(EFF_SPRAY, R->EffectList[i]);
				R->active=false;
			}
		}
	}
	else
	{
		if(R->active==false)
		{
			for ( i = 0; i < R->EffectCount; i++ )
				R->EffectList[i]=Create(R);
			R->active=true;
		}
	}
	if(R->active==GE_TRUE)
	{
		R->origin = R->OriginOffset;
		if(SetOriginOffset(R->EntityName, R->BoneName, R->Model, &(R->origin)))
		{
  			geVec3d_Copy( &(R->origin), &( Sp.Source ) );
			geVec3d_AddScaled( &( Sp.Source ), &( Sp.Gravity ), Sp.MinUnitLife, &( Sp.Dest ) );
			// adjust position
			for ( i = 0; i < R->EffectCount; i++ )
				CCD->EffectManager()->Item_Modify(EFF_SPRAY, R->EffectList[i], (void *)&Sp, SPRAY_SOURCE | SPRAY_ACTUALDEST);
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

int CRain::LocateEntity(char *szName, void **pEntityData)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

//	Ok, check to see if there are Rain in this world

	pSet = geWorld_GetEntitySet(CCD->World(), "Rain");

	if(!pSet) 
		return RGF_NOT_FOUND;

//	Ok, we have Rain somewhere.  Dig through 'em all.

	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	    pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
		{
		Rain *pSource = (Rain*)geEntity_GetUserData(pEntity);
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

int CRain::ReSynchronize()
{
	return RGF_SUCCESS;
}

