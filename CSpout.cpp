/************************************************************************************//**
 * @file CSpout.cpp
 * @brief Spout and ActorSpout Entity Class
 ****************************************************************************************/

#include "RabidFramework.h"
#include "CSpout.h"

extern geBitmap *TPool_Bitmap(const char *DefaultBmp, const char *DefaultAlpha,
							  const char *BName, const char *AName);

/* ------------------------------------------------------------------------------------ */
// CSpout Constructor
/* ------------------------------------------------------------------------------------ */
CSpout::CSpout()
{
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "Spout");

	if(!pSet)
		return;

	geEntity *pEntity;

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Spout *S = static_cast<Spout*>(geEntity_GetUserData(pEntity));

		if(EffectC_IsStringNull(S->szEntityName))
		{
			char szName[128];
			geEntity_GetName(pEntity, szName, 128);
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

		if(S->MaxPauseTime > 0.0f)
			S->PauseTime = EffectC_Frand( S->MinPauseTime, S->MaxPauseTime );

		TPool_Bitmap("g_bubble.bmp", "a_bubble.bmp", S->BmpName, S->AlphaName);
		S->active = GE_FALSE;
		S->EffectList = -1;

		S->Angles.X = GE_PIOVER180*(S->Angles.X);
		S->Angles.Y = GE_PIOVER180*(S->Angles.Y-90.0f);
		S->Angles.Z = GE_PIOVER180*(S->Angles.Z);
	}

	return;
}


/* ------------------------------------------------------------------------------------ */
// Create
/* ------------------------------------------------------------------------------------ */
int CSpout::Create(Spout *S)
{
	int effect = -1;
	Spray Sp;
	geVec3d In;
	geXForm3d Xf;

	memset(&Sp, 0, sizeof(Sp));
	Sp.Texture = TPool_Bitmap("g_bubble.bmp", "a_bubble.bmp", S->BmpName, S->AlphaName);

	// variance
	Sp.SourceVariance = S->SourceVariance;
	if(Sp.SourceVariance < 0)
		Sp.SourceVariance = 0;
	Sp.DestVariance = S->DestVariance;
	if(Sp.DestVariance < 0)
		Sp.DestVariance = 0;

	// scale
	Sp.MinScale = S->MinScale;
	Sp.MaxScale = S->MaxScale;
	Sp.Bounce = S->Bounce;
	if(Sp.MinScale <= 0.0f)
		Sp.MinScale = 0.1f;
	if(Sp.MaxScale < Sp.MinScale)
		Sp.MaxScale = Sp.MinScale;

	// speed
	Sp.MinSpeed = S->MinSpeed;
	Sp.MaxSpeed = S->MaxSpeed;
	if(Sp.MinSpeed < 0.0f)
		Sp.MinSpeed = 0.0f;
	if(Sp.MaxSpeed < Sp.MinSpeed)
		Sp.MaxSpeed = Sp.MinSpeed;

	// unitlife
	Sp.MinUnitLife = S->MinUnitLife;
	Sp.MaxUnitLife = S->MaxUnitLife;
	if(Sp.MinUnitLife <= 0.0f)
		Sp.MinUnitLife = 0.1f;
	if(Sp.MaxUnitLife < Sp.MinUnitLife)
		Sp.MaxUnitLife = Sp.MinUnitLife;
	Sp.ColorMin.r = S->ColorMin.r;

	// color
	// red
	if(Sp.ColorMin.r < 0.0f)
		Sp.ColorMin.r = 0.0f;
	if(Sp.ColorMin.r > 255.0f)
		Sp.ColorMin.r = 255.0f;
	Sp.ColorMax.r = S->ColorMax.r;
	if(Sp.ColorMax.r < Sp.ColorMin.r)
		Sp.ColorMax.r = Sp.ColorMin.r;

	// green
	Sp.ColorMin.g = S->ColorMin.g;
	if(Sp.ColorMin.g < 0.0f)
		Sp.ColorMin.g = 0.0f;
	if(Sp.ColorMin.g > 255.0f)
		Sp.ColorMin.g = 255.0f;
	Sp.ColorMax.g = S->ColorMax.g;
	if(Sp.ColorMax.g < Sp.ColorMin.g)
		Sp.ColorMax.g = Sp.ColorMin.g;

	// blue
	Sp.ColorMin.b = S->ColorMin.b;
	if(Sp.ColorMin.b < 0.0f)
		Sp.ColorMin.b = 0.0f;
	if(Sp.ColorMin.b > 255.0f)
		Sp.ColorMin.b = 255.0f;
	Sp.ColorMax.b = S->ColorMax.b;
	if(Sp.ColorMax.b < Sp.ColorMin.b)
		Sp.ColorMax.b = Sp.ColorMin.b;

	// alpha
	Sp.ColorMin.a = 255.0f;
	Sp.ColorMax.a = 255.0f;

	// rate
	Sp.Rate = S->ParticleCreateRate;
	if(Sp.Rate < 0.0f)
		Sp.Rate = 0.1f;
	geVec3d_Copy(&(S->Gravity), &(Sp.Gravity));

	// setup orientation
	geXForm3d_SetXRotation(&Xf, S->Angles.X);
	geXForm3d_RotateY(&Xf, S->Angles.Y);
	geXForm3d_RotateZ(&Xf, S->Angles.Z);

	geVec3d_Copy(&(S->origin), &(Sp.Source));
	geXForm3d_GetIn(&Xf, &In);
	geVec3d_Inverse(&In);
	geVec3d_AddScaled(&(Sp.Source), &In, 50.0f, &(Sp.Dest));

	Sp.UseWind = S->UseWind;

	effect = CCD->EffectManager()->Item_Add(EFF_SPRAY, static_cast<void*>(&Sp));
	return effect;
}


/* ------------------------------------------------------------------------------------ */
// CSpout  Destructor
/* ------------------------------------------------------------------------------------ */
CSpout::~CSpout()
{
	return;
}


/* ------------------------------------------------------------------------------------ */
// Tick
/* ------------------------------------------------------------------------------------ */
void CSpout::Tick(geFloat dwTicks)
{
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "Spout");

	if(!pSet)
		return;

	geEntity *pEntity;

	dwTicks *= 0.001f;

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Spout *S = static_cast<Spout*>(geEntity_GetUserData(pEntity));

		if(!EffectC_IsStringNull(S->TriggerName))
		{
			if(GetTriggerState(S->TriggerName))
			{
				if(S->active == GE_FALSE && S->TotalLife != -1.0f)
				{
					S->EffectList = Create(S);
					S->active = GE_TRUE;
				}
			}
			else
			{
				if(S->active == GE_TRUE)
				{
					if(S->EffectList != -1)
						CCD->EffectManager()->Item_Delete(EFF_SPRAY, S->EffectList);

					S->EffectList = -1;
					S->active = GE_FALSE;
				}
			}
		}
		else
		{
			if(S->active == GE_FALSE && S->TotalLife != -1.0f)
			{
				S->EffectList = Create(S);
				S->active = GE_TRUE;
			}
		}

		if(S->active == GE_TRUE)
		{
			// kill the effect if its time has run out
			if(S->TotalLife > 0.0f)
			{
				S->TotalLife -= dwTicks;

				if(S->TotalLife <= 0.0f)
				{
					S->TotalLife = -1.0f;
					CCD->EffectManager()->Item_Delete(EFF_SPRAY, S->EffectList);
					S->EffectList = -1;
					S->active = GE_FALSE;
					continue;
				}
			}
			else
			{
				// adjust pause time
				if(S->PauseTime > 0.0f)
				{
					S->PauseTime -= dwTicks;

					if(S->PauseTime <= 0.0f)
					{
						S->PauseTime = EffectC_Frand(S->MinPauseTime, S->MaxPauseTime);
						S->PauseState = !S->PauseState;
						CCD->EffectManager()->Item_Pause(EFF_SPRAY, S->EffectList, S->PauseState);
					}
				}
			}

			S->origin = S->OriginOffset;

			if(SetOriginOffset(S->EntityName, S->BoneName, S->Model, &(S->origin)))
			{
				// setup orientation
				geVec3d		In;
				geXForm3d	Xf;
				Spray		Sp;

				geXForm3d_SetXRotation(&Xf, S->Angles.X);
				geXForm3d_RotateY(&Xf, S->Angles.Y);
				geXForm3d_RotateZ(&Xf, S->Angles.Z);

				geVec3d_Copy(&(S->origin), &(Sp.Source));
				geXForm3d_GetIn(&Xf, &In);
				geVec3d_Inverse(&In);
				geVec3d_AddScaled(&(Sp.Source), &In, 50.0f, &(Sp.Dest));

				CCD->EffectManager()->Item_Modify(EFF_SPRAY, S->EffectList, static_cast<void*>(&Sp), SPRAY_SOURCE | SPRAY_ACTUALDEST);
			}
		}
	}
}


// ******************** CRGF Overrides ********************

/* ------------------------------------------------------------------------------------ */
// LocateEntity
//
// Given a name, locate the desired item in the currently loaded level
// ..and return it's user data.
/* ------------------------------------------------------------------------------------ */
int CSpout::LocateEntity(const char *szName, void **pEntityData)
{
	// Ok, check to see if there are Spout in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "Spout");

	if(!pSet)
		return RGF_NOT_FOUND;

	geEntity *pEntity;

	// Ok, we have Spout somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Spout *pSource = static_cast<Spout*>(geEntity_GetUserData(pEntity));

		if(!strcmp(pSource->szEntityName, szName))
		{
			*pEntityData = static_cast<void*>(pSource);
			return RGF_SUCCESS;
		}
	}

	return RGF_NOT_FOUND;								// Sorry, no such entity here
}

/* ------------------------------------------------------------------------------------ */
// ReSynchronize
//
// Correct internal timing to match current time, to make up for time lost
// ..when outside the game loop (typically in "menu mode").
/* ------------------------------------------------------------------------------------ */
int CSpout::ReSynchronize()
{
	return RGF_SUCCESS;
}


/****************************************************************************************/
/*									ActorSpout											*/
/****************************************************************************************/

/* ------------------------------------------------------------------------------------ */
// CActorSpout Constructor
/* ------------------------------------------------------------------------------------ */
CActorSpout::CActorSpout()
{
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "ActorSpout");

	if(!pSet)
		return;

	geEntity *pEntity;

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		ActorSpout *S = static_cast<ActorSpout*>(geEntity_GetUserData(pEntity));

		if(EffectC_IsStringNull(S->szEntityName))
		{
			char szName[128];
			geEntity_GetName(pEntity, szName, 128);
			S->szEntityName = szName;
		}

		// Ok, put this entity into the Global Entity Registry
		CCD->EntityRegistry()->AddEntity(S->szEntityName, "ActorSpout");
		S->OriginOffset = S->origin;

		if(S->Model)
		{
			geVec3d ModelOrigin;
			geWorld_GetModelRotationalCenter(CCD->World(), S->Model, &ModelOrigin);
			geVec3d_Subtract(&(S->origin), &ModelOrigin, &(S->OriginOffset));
		}

		if(S->MinPauseTime < 0.0f)
			S->MinPauseTime = 0.0f;

		if(S->MaxPauseTime < S->MinPauseTime)
			S->MaxPauseTime = S->MinPauseTime;

		if(S->MaxPauseTime > 0.0f)
			S->PauseTime = EffectC_Frand(S->MinPauseTime, S->MaxPauseTime);

		S->active = GE_FALSE;
		S->EffectList = -1;

		char Name[64];

		for(int i=0; i<S->NumActors; ++i)
		{
			sprintf(Name, "%s%d%s", S->BaseName, i, ".act");
			geActor *Actor = CCD->ActorManager()->SpawnActor(Name,
				S->origin, S->BaseRotation, "", "", NULL);

			if(!Actor)
			{
				char szError[256];
				sprintf(szError, "[ERROR] File %s - Line %d: %s : Missing Actor '%s'",
						__FILE__, __LINE__, S->szEntityName, Name);
				CCD->ReportError(szError, false);
				CCD->ShutdownLevel();
				delete CCD;
				CCD = NULL;
				MessageBox(NULL, szError,"ActorSpout", MB_OK);
				exit(-333);
			}

			CCD->ActorManager()->RemoveActor(Actor);
			geActor_Destroy(&Actor);
			Actor = NULL;
		}
	}

	return;
}


/* ------------------------------------------------------------------------------------ */
// Create
/* ------------------------------------------------------------------------------------ */
int CActorSpout::Create(ActorSpout *S)
{
	int effect = -1;
	ActorSpray	Sp;
	geVec3d	In;
	geXForm3d	Xf;

	memset(&Sp, 0, sizeof(Sp));
	strcpy(Sp.BaseName, S->BaseName);
	Sp.NumActors = S->NumActors;
	Sp.Style = S->Style;
	Sp.Alpha = S->Alpha;
	Sp.AlphaRate = S->AlphaRate;

	Sp.BaseRotation.X = GE_PIOVER180*S->BaseRotation.X;
	Sp.BaseRotation.Y = GE_PIOVER180*S->BaseRotation.Y;
	Sp.BaseRotation.Z = GE_PIOVER180*S->BaseRotation.Z;

	Sp.MinRotationSpeed.X = GE_PIOVER180*S->MinRotationSpeed.X;
	Sp.MinRotationSpeed.Y = GE_PIOVER180*S->MinRotationSpeed.Y;
	Sp.MinRotationSpeed.Z = GE_PIOVER180*S->MinRotationSpeed.Z;
	Sp.MaxRotationSpeed.X = GE_PIOVER180*S->MaxRotationSpeed.X;
	Sp.MaxRotationSpeed.Y = GE_PIOVER180*S->MaxRotationSpeed.Y;
	Sp.MaxRotationSpeed.Z = GE_PIOVER180*S->MaxRotationSpeed.Z;

	// variance
	Sp.SourceVariance = S->SourceVariance;
	if(Sp.SourceVariance < 0)
		Sp.SourceVariance = 0;
	Sp.DestVariance = S->DestVariance;
	if(Sp.DestVariance < 0)
		Sp.DestVariance = 0;

	// scale
	Sp.MinScale = S->MinScale;
	Sp.MaxScale = S->MaxScale;
	Sp.Bounce = S->Bounce;
	Sp.Solid = S->Solid;
	if(Sp.MinScale <= 0.0f)
		Sp.MinScale = 0.1f;
	if(Sp.MaxScale < Sp.MinScale)
		Sp.MaxScale = Sp.MinScale;

	// speed
	Sp.MinSpeed = S->MinSpeed;
	Sp.MaxSpeed = S->MaxSpeed;
	if(Sp.MinSpeed < 0.0f)
		Sp.MinSpeed = 0.0f;
	if(Sp.MaxSpeed < Sp.MinSpeed)
		Sp.MaxSpeed = Sp.MinSpeed;

	// unitlife
	Sp.MinUnitLife = S->MinUnitLife;
	Sp.MaxUnitLife = S->MaxUnitLife;
	if(Sp.MinUnitLife <= 0.0f)
		Sp.MinUnitLife = 0.1f;
	if(Sp.MaxUnitLife < Sp.MinUnitLife)
		Sp.MaxUnitLife = Sp.MinUnitLife;

	// fillcolor
	Sp.FillColor = S->FillColor;
	// ambientcolor
	Sp.AmbientColor = S->AmbientColor;
	Sp.AmbientLightFromFloor = S->AmbientLightFromFloor;

	Sp.Rate = S->ParticleCreateRate;
	if(Sp.Rate < 0.0f)
		Sp.Rate = 0.1f;
	Sp.Gravity = S->Gravity;
	Sp.EnvironmentMapping = S->EnvironmentMapping;
	Sp.AllMaterial = S->AllMaterial;
	Sp.PercentMapping = S->PercentMapping;
	Sp.PercentMaterial = S->PercentMaterial;

	// setup orientation
	geXForm3d_SetZRotation(&Xf,  S->Angles.Z * GE_PIOVER180);
	geXForm3d_RotateX(&Xf, -S->Angles.X * GE_PIOVER180);
	geXForm3d_RotateY(&Xf, (S->Angles.Y - 90.0f) * GE_PIOVER180);

	geVec3d_Copy(&(S->origin), &(Sp.Source));

	geXForm3d_GetIn(&Xf, &In);
	geVec3d_Inverse(&In);
	geVec3d_AddScaled(&(Sp.Source), &In, 50.0f, &(Sp.Dest));

	effect = CCD->EffectManager()->Item_Add(EFF_ACTORSPRAY, static_cast<void*>(&Sp));
	return effect;
}


/* ------------------------------------------------------------------------------------ */
// CActorSpout  Destructor
/* ------------------------------------------------------------------------------------ */
CActorSpout::~CActorSpout()
{
	return;
}


/* ------------------------------------------------------------------------------------ */
// Tick
/* ------------------------------------------------------------------------------------ */
void CActorSpout::Tick(geFloat dwTicks)
{
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "ActorSpout");

	if(!pSet)
		return;

	geEntity *pEntity;

	dwTicks *= 0.001f;

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		ActorSpout *S = static_cast<ActorSpout*>(geEntity_GetUserData(pEntity));

		if(!EffectC_IsStringNull(S->TriggerName))
		{
			if(GetTriggerState(S->TriggerName))
			{
				if(S->active == GE_FALSE && S->TotalLife != -1.0f)
				{
					S->EffectList = Create(S);
					S->active = GE_TRUE;
				}
			}
			else
			{
				if(S->active == GE_TRUE)
				{
					if(S->EffectList != -1)
						CCD->EffectManager()->Item_Delete(EFF_ACTORSPRAY, S->EffectList);

					S->EffectList = -1;
					S->active = GE_FALSE;
				}
			}
		}
		else
		{
			if(S->active == GE_FALSE && S->TotalLife != -1.0f)
			{
				S->EffectList = Create(S);
				S->active = GE_TRUE;
			}
		}

		if(S->active == GE_TRUE)
		{
			// kill the effect if its time has run out
			if(S->TotalLife > 0.0f)
			{
				S->TotalLife -= dwTicks;

				if(S->TotalLife <= 0.0f)
				{
					S->TotalLife = -1.0f;
					CCD->EffectManager()->Item_Delete(EFF_ACTORSPRAY, S->EffectList);
					S->EffectList = -1;
					S->active = GE_FALSE;
					continue;
				}
			}
			else
			{
				// adjust pause time
				if(S->PauseTime > 0.0f)
				{
					S->PauseTime -= dwTicks;

					if(S->PauseTime <= 0.0f)
					{
						S->PauseTime = EffectC_Frand(S->MinPauseTime, S->MaxPauseTime);
						S->PauseState = !S->PauseState;
						CCD->EffectManager()->Item_Pause(EFF_ACTORSPRAY, S->EffectList, S->PauseState);
					}
				}
			}

			S->origin = S->OriginOffset;

			if(SetOriginOffset(S->EntityName, S->BoneName, S->Model, &(S->origin)))
			{
				// setup orientation
				geVec3d		In;
				geXForm3d	Xf;
				ActorSpray	Sp;

				geXForm3d_SetZRotation(&Xf,  S->Angles.Z * GE_PIOVER180);
				geXForm3d_RotateX(&Xf, -S->Angles.X * GE_PIOVER180);
				geXForm3d_RotateY(&Xf, (S->Angles.Y - 90.0f) * GE_PIOVER180);

				geVec3d_Copy(&(S->origin), &(Sp.Source));

				geXForm3d_GetIn(&Xf, &In);
				geVec3d_Inverse(&In);
				geVec3d_AddScaled(&(Sp.Source), &In, 50.0f, &(Sp.Dest));
				CCD->EffectManager()->Item_Modify(EFF_ACTORSPRAY, S->EffectList, static_cast<void*>(&Sp), SPRAY_SOURCE | SPRAY_ACTUALDEST);
			}
		}
	}
}

// ******************** CRGF Overrides ********************

/* ------------------------------------------------------------------------------------ */
// LocateEntity
//
// Given a name, locate the desired item in the currently loaded level
// ..and return it's user data.
/* ------------------------------------------------------------------------------------ */
int CActorSpout::LocateEntity(const char *szName, void **pEntityData)
{
	// Ok, check to see if there are Spout in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "ActorSpout");

	if(!pSet)
		return RGF_NOT_FOUND;

	geEntity *pEntity;

	// Ok, we have Spout somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		ActorSpout *pSource = static_cast<ActorSpout*>(geEntity_GetUserData(pEntity));

		if(!strcmp(pSource->szEntityName, szName))
		{
			*pEntityData = static_cast<void*>(pSource);
			return RGF_SUCCESS;
		}
	}

	return RGF_NOT_FOUND;								// Sorry, no such entity here
}

/* ------------------------------------------------------------------------------------ */
// ReSynchronize
//
// Correct internal timing to match current time, to make up for time lost
// ..when outside the game loop (typically in "menu mode").
/* ------------------------------------------------------------------------------------ */
int CActorSpout::ReSynchronize()
{
	return RGF_SUCCESS;
}


/* ----------------------------------- END OF FILE ------------------------------------ */
