/************************************************************************************//**
 * @file CFlame.cpp
 * @brief Flame class implementation
 * @author Ralph Deane
 ****************************************************************************************/

//	Include the One True Header
#include "RabidFramework.h"
#include "CFlame.h"

extern geBitmap *TPool_Bitmap(const char *DefaultBmp, const char *DefaultAlpha,
							  const char *BName, const char *AName);

/* ------------------------------------------------------------------------------------ */
//	Constructor
/* ------------------------------------------------------------------------------------ */
CFlame::CFlame()
{
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "Flame");

	if(!pSet)
		return;

	geEntity *pEntity;

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Flame *S = static_cast<Flame*>(geEntity_GetUserData(pEntity));

		if(EffectC_IsStringNull(S->szEntityName))
		{
			char szName[128];
			geEntity_GetName(pEntity, szName, 128);
			S->szEntityName = szName;
		}

		// Ok, put this entity into the Global Entity Registry
		CCD->EntityRegistry()->AddEntity(S->szEntityName, "Flame");
		S->OriginOffset = S->origin;

		if(S->Model)
		{
			geVec3d ModelOrigin;
			geWorld_GetModelRotationalCenter(CCD->World(), S->Model, &ModelOrigin);
			geVec3d_Subtract(&(S->origin), &ModelOrigin, &(S->OriginOffset));
		}

		S->active		= GE_FALSE;
		S->EffectList0	= -1;
		S->EffectList1	= -1;

		TPool_Bitmap("flame03.bmp", "a_flame.bmp", S->BmpName, S->AlphaName);
	}

	return;
}

/* ------------------------------------------------------------------------------------ */
//	CreateSpray
//
//	Create Spray effect
/* ------------------------------------------------------------------------------------ */
int CFlame::CreateSpray(Flame *S)
{
	int effect = -1;
	Spray	Sp;
	geVec3d	In;
	geXForm3d Xf;

	memset(&Sp, 0, sizeof(Sp));

	Sp.Texture = TPool_Bitmap("flame03.Bmp", "a_flame.bmp", S->BmpName, S->AlphaName);

	// variance
	Sp.SourceVariance = static_cast<int>(static_cast<geFloat>(S->SourceVariance) * S->Scale);

	if(Sp.SourceVariance < 0)
		Sp.SourceVariance = 0;

	Sp.DestVariance = static_cast<int>(static_cast<geFloat>(S->DestVariance) * S->Scale);

	if(Sp.DestVariance < 0)
		Sp.DestVariance = 0;

	// scale
	Sp.MinScale = S->MinScale * S->Scale;
	Sp.MaxScale = S->MaxScale * S->Scale;

	if(Sp.MinScale <= 0.0f)
		Sp.MinScale = 0.1f;

	if(Sp.MaxScale < Sp.MinScale)
		Sp.MaxScale = Sp.MinScale;

	// speed
	Sp.MinSpeed = S->MinSpeed * S->Scale;
	Sp.MaxSpeed = S->MaxSpeed * S->Scale;

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

	// color
	// red
	Sp.ColorMin.r = S->ColorMin.r;

	if(Sp.ColorMin.r < 0.0f)
		Sp.ColorMin.r = 0.0f;
	else if(Sp.ColorMin.r > 255.0f)
		Sp.ColorMin.r = 255.0f;

	Sp.ColorMax.r = S->ColorMax.r;

	if(Sp.ColorMax.r < Sp.ColorMin.r)
		Sp.ColorMax.r = Sp.ColorMin.r;

	// green
	Sp.ColorMin.g = S->ColorMin.g;

	if(Sp.ColorMin.g < 0.0f)
		Sp.ColorMin.g = 0.0f;
	else if(Sp.ColorMin.g > 255.0f)
		Sp.ColorMin.g = 255.0f;

	Sp.ColorMax.g = S->ColorMax.g;

	if(Sp.ColorMax.g < Sp.ColorMin.g)
		Sp.ColorMax.g = Sp.ColorMin.g;

	// blue
	Sp.ColorMin.b = S->ColorMin.b;

	if(Sp.ColorMin.b < 0.0f)
		Sp.ColorMin.b = 0.0f;
	else if(Sp.ColorMin.b > 255.0f)
		Sp.ColorMin.b = 255.0f;

	Sp.ColorMax.b = S->ColorMax.b;

	if(Sp.ColorMax.b < Sp.ColorMin.b)
		Sp.ColorMax.b = Sp.ColorMin.b;

	// alpha
	Sp.ColorMin.a = 255.0f;
	Sp.ColorMax.a = 255.0f;

	// rate
	Sp.Rate = S->ParticleCreateRate;

	if(Sp.Rate < 0.0f )
		Sp.Rate = 0.1f;

	geVec3d_Copy(&(S->Gravity), &(Sp.Gravity));

	Sp.UseWind = S->UseWind;

	// setup orientation
	geXForm3d_SetXRotation(&Xf,  S->Angles.X	* GE_PIOVER180);
	geXForm3d_RotateY(&Xf, (S->Angles.Y-90.0f)	* GE_PIOVER180);
	geXForm3d_RotateZ(&Xf,  S->Angles.Z			* GE_PIOVER180);

	geVec3d_Copy(&(S->origin), &(Sp.Source));
	geXForm3d_GetIn(&Xf, &In);
	geVec3d_Inverse(&In);
	geVec3d_AddScaled(&(Sp.Source), &In, 50.0f, &(Sp.Dest));

	effect = CCD->EffectManager()->Item_Add(EFF_SPRAY, static_cast<void*>(&Sp));

	return effect;
}

/* ------------------------------------------------------------------------------------ */
//	CreateGlow
//
//	Create Glow effect
/* ------------------------------------------------------------------------------------ */
int CFlame::CreateGlow(Flame *S)
{
	int effect = -1;
	Glow Gl;

	if(S->GIntensity == 0.0f)
		return -1;

	// clear out the light data
	memset(&Gl, 0, sizeof(Gl));

	// copy the position
	geVec3d_Copy(&(S->origin), &(Gl.Pos));

	// setup light data
	Gl.RadiusMin = S->GRadiusMin * S->Scale;
	Gl.RadiusMax = S->GRadiusMax * S->Scale;
	Gl.ColorMin.r = S->GColorMin.r;
	Gl.ColorMax.r = S->GColorMax.r;
	Gl.ColorMin.g = S->GColorMin.g;
	Gl.ColorMax.g = S->GColorMax.g;
	Gl.ColorMin.b = S->GColorMin.b;
	Gl.ColorMax.b = S->GColorMax.b;

	// red
	if(Gl.ColorMin.r < 0.0f)
		Gl.ColorMin.r = 0.0f;
	else if(Gl.ColorMin.r > 255.0f)
		Gl.ColorMin.r = 255.0f;

	if(Gl.ColorMax.r < Gl.ColorMin.r)
		Gl.ColorMax.r = Gl.ColorMin.r;

	// green
	if(Gl.ColorMin.g < 0.0f)
		Gl.ColorMin.g = 0.0f;
	else if(Gl.ColorMin.g > 255.0f)
		Gl.ColorMin.g = 255.0f;

	if(Gl.ColorMax.g < Gl.ColorMin.g)
		Gl.ColorMax.g = Gl.ColorMin.g;

	// blue
	if(Gl.ColorMin.b < 0.0f)
		Gl.ColorMin.b = 0.0f;
	else if(Gl.ColorMin.b > 255.0f)
		Gl.ColorMin.b = 255.0f;

	if(Gl.ColorMax.b < Gl.ColorMin.b)
		Gl.ColorMax.b = Gl.ColorMin.b;

	Gl.ColorMin.a	= 255.0f;
	Gl.ColorMax.a	= 255.0f;
	Gl.Intensity	= S->GIntensity;
	Gl.DoNotClip	= false;
	Gl.CastShadows	= S->GCastShadows;
	Gl.Spot			= false;

	// add the light to the manager
	effect = CCD->EffectManager()->Item_Add(EFF_LIGHT, static_cast<void*>(&Gl));

	return effect;
}

/* ------------------------------------------------------------------------------------ */
//	Destructor
/* ------------------------------------------------------------------------------------ */
CFlame::~CFlame()
{
	return;
}

/* ------------------------------------------------------------------------------------ */
//	Tick
/* ------------------------------------------------------------------------------------ */
void CFlame::Tick(geFloat dwTicks)
{
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "Flame");

	if(!pSet)
		return;

	geEntity *pEntity;

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Flame *S = static_cast<Flame*>(geEntity_GetUserData(pEntity));

		if(!EffectC_IsStringNull(S->TriggerName))
		{
			if(GetTriggerState(S->TriggerName))
			{
				if(S->active == GE_FALSE)
				{
					S->EffectList0	= CreateSpray(S);
					S->EffectList1	= CreateGlow(S);
					S->active		= GE_TRUE;
				}
			}
			else
			{
				if(S->active == GE_TRUE)
				{
					if(S->EffectList0 != -1)
						CCD->EffectManager()->Item_Delete(EFF_SPRAY, S->EffectList0);

					if(S->EffectList1 != -1)
						CCD->EffectManager()->Item_Delete(EFF_LIGHT, S->EffectList1);

					S->EffectList0	= -1;
					S->EffectList1	= -1;
					S->active		= GE_FALSE;
				}
			}
		}
		else
		{
			if(S->active == GE_FALSE)
			{
				S->EffectList0	= CreateSpray(S);
				S->EffectList1	= CreateGlow(S);
				S->active		= GE_TRUE;
			}
		}

		if(S->active == GE_TRUE)
		{
			// kill the effect if its time has run out
			if(S->TotalLife > 0.0f)
			{
				S->TotalLife -= (dwTicks*0.001f);

				if(S->TotalLife <= 0.0f)
				{
					S->TotalLife = 0.0f;

					if(S->EffectList0 != -1)
						CCD->EffectManager()->Item_Delete(EFF_SPRAY, S->EffectList0);

					if(S->EffectList1 != -1)
						CCD->EffectManager()->Item_Delete(EFF_LIGHT, S->EffectList1);

					S->EffectList0	= -1;
					S->EffectList1	= -1;
					S->active		= GE_FALSE;
					continue;
				}
			}

			S->origin = S->OriginOffset;

			if(SetOriginOffset(S->EntityName, S->BoneName, S->Model, &(S->origin)))
			{
				// setup orientation
				geVec3d	In;
				geXForm3d	Xf;
				Spray		Sp;
				Glow 		Gl;

				geXForm3d_SetXRotation(&Xf,  S->Angles.X	* GE_PIOVER180);
				geXForm3d_RotateY(&Xf, (S->Angles.Y-90.0f)  * GE_PIOVER180);
				geXForm3d_RotateZ(&Xf,  S->Angles.Z			* GE_PIOVER180);

				geVec3d_Copy(&(S->origin), &(Sp.Source));
				geXForm3d_GetIn(&Xf, &In );
				geVec3d_Inverse(&In);
				geVec3d_AddScaled(&(Sp.Source), &In, 50.0f, &(Sp.Dest));

				CCD->EffectManager()->Item_Modify(	EFF_SPRAY,
													S->EffectList0,
													static_cast<void*>(&Sp),
													SPRAY_SOURCE | SPRAY_ACTUALDEST);

				if(S->EffectList1 != -1)
				{
					geVec3d_Copy(&(Sp.Dest), &(Gl.Pos));
					CCD->EffectManager()->Item_Modify(	EFF_LIGHT,
														S->EffectList1,
														static_cast<void*>(&Gl),
														GLOW_POS);
				}
			}
		}
	}
}


//	******************** CRGF Overrides ********************

/* ------------------------------------------------------------------------------------ */
//	LocateEntity
//
//	Given a name, locate the desired item in the currently loaded level
//	..and return it's user data.
/* ------------------------------------------------------------------------------------ */
int CFlame::LocateEntity(const char *szName, void **pEntityData)
{
	// Ok, check to see if there are Flames in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "Flame");

	if(!pSet)
		return RGF_NOT_FOUND;

	geEntity *pEntity;

	// Ok, we have Flames somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Flame *pSource = static_cast<Flame*>(geEntity_GetUserData(pEntity));

		if(!strcmp(pSource->szEntityName, szName))
		{
			*pEntityData = static_cast<void*>(pSource);
			return RGF_SUCCESS;
		}
	}

	return RGF_NOT_FOUND;								// Sorry, no such entity here
}

/* ------------------------------------------------------------------------------------ */
//	ReSynchronize
//
//	Correct internal timing to match current time, to make up for time lost
//	..when outside the game loop (typically in "menu mode").
/* ------------------------------------------------------------------------------------ */
int CFlame::ReSynchronize()
{
	return RGF_SUCCESS;
}


/* ----------------------------------- END OF FILE ------------------------------------ */
