/************************************************************************************//**
 * @file CProcedural.cpp
 * @brief Procedural handler
 *
 * This file contains the class implementation for the CProcedural class that
 * encapsulates Procedural handling in the RGF.
 * @author  Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

// You only need the one, master include file.
#include "RabidFramework.h"
#include "CProcedural.h"

extern Procedural *Water_Create(const char *TextureName, geWorld  *World);
extern void Water_Destroy(Procedural *Water);
extern geBoolean Water_Animate(Procedural *Water, geFloat ElapsedTime);

extern Procedural *Smoke_Create(const char *TextureName, geWorld  *World, const char *StrParms);
extern void Smoke_Destroy(Procedural *Proc);
extern geBoolean Smoke_Animate(Procedural *Smoke, geFloat ElapsedTime);

extern Procedural *Fire_Create(const char *TextureName, geWorld  *World, const char *ParmStart);
extern void Fire_Destroy(Procedural * P);
extern geBoolean Fire_Animate(Procedural * P,geFloat time);

extern Procedural *ElectricFx_Create(const char *TextureName, geWorld  *World, const char *StrParms);
extern void	ElectricFx_Destroy(Procedural *Proc);
extern geBoolean	ElectricFx_Animate(Procedural *Fx, geFloat ElapsedTime);

extern Procedural *Plasma_Create(const char *TextureName, geWorld  *World, const char *ParmStart);
extern void	Plasma_Destroy(Procedural *Proc);
extern geBoolean Plasma_Animate(Procedural *Plasma, float ElapsedTime);

extern Procedural *BumpMap_Create(const char *TextureName, geWorld  *World, const char *ParmStart);
extern void	BumpMap_Destroy(Procedural *Proc);
extern geBoolean BumpMap_Animate(Procedural *BumpMap, geFloat ElapsedTime);

extern Procedural *Flow_Create(const char *TextureName, geWorld  *World, const char *ParmStart);
extern void	Flow_Destroy(Procedural *Proc);
extern geBoolean Flow_Animate(Procedural *BumpMap, geFloat ElapsedTime);

extern Procedural *Particles_Create(const char *TextureName, geWorld  *World, const char *InputParams);
extern void Particles_Destroy(Procedural * Proc);
extern geBoolean Particles_Animate(Procedural * Proc,geFloat time);


/* ------------------------------------------------------------------------------------ */
//	CProcedural
//
//	Default constructor.  Set up for texture replacement by scanning
//	..for texture replacement indicators.
/* ------------------------------------------------------------------------------------ */
CProcedural::CProcedural()
{
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "TextureProc");

	if(!pSet)
		return;	// Not on this level.

	geEntity *pEntity;

	// Ok, we have procedurals somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		TextureProc *pTex = static_cast<TextureProc*>(geEntity_GetUserData(pEntity));

		if(!EffectC_IsStringNull(pTex->szTextureName) && !EffectC_IsStringNull(pTex->szProcName))
		{
			if(!stricmp(pTex->szProcName, "water"))
			{
				pTex->Proc = Water_Create(pTex->szTextureName, CCD->World());
				continue;
			}
			else if(!stricmp(pTex->szProcName, "smoke"))
			{
				pTex->Proc = Smoke_Create(pTex->szTextureName,CCD->World(),pTex->Parameter);
				continue;
			}
			else if(!stricmp(pTex->szProcName, "fire"))
			{
				pTex->Proc = Fire_Create(pTex->szTextureName,CCD->World(),pTex->Parameter);
				continue;
			}
			else if(!stricmp(pTex->szProcName, "electric"))
			{
				pTex->Proc = ElectricFx_Create(pTex->szTextureName,CCD->World(),pTex->Parameter);
				continue;
			}
			else if(!stricmp(pTex->szProcName, "plasma"))
			{
				pTex->Proc = Plasma_Create(pTex->szTextureName,CCD->World(),pTex->Parameter);
				continue;
			}
			else if(!stricmp(pTex->szProcName, "bumpmap"))
			{
				pTex->Proc = BumpMap_Create(pTex->szTextureName,CCD->World(),pTex->Parameter);
				continue;
			}
			else if(!stricmp(pTex->szProcName, "flow"))
			{
				pTex->Proc = Flow_Create(pTex->szTextureName,CCD->World(),pTex->Parameter);
				continue;
			}
			else if(!stricmp(pTex->szProcName, "particle"))
			{
				pTex->Proc = Particles_Create(pTex->szTextureName,CCD->World(),pTex->Parameter);
				continue;
			}
		}
	}
}

/* ------------------------------------------------------------------------------------ */
//	Destructor
/* ------------------------------------------------------------------------------------ */
CProcedural::~CProcedural()
{
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "TextureProc");

	if(!pSet)
		return;

	geEntity *pEntity;

	// Ok, we have procedurals somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		TextureProc *pTex = static_cast<TextureProc*>(geEntity_GetUserData(pEntity));

		if(!EffectC_IsStringNull(pTex->szTextureName) && !EffectC_IsStringNull(pTex->szProcName))
		{
			if(!stricmp(pTex->szProcName, "water"))
			{
				Water_Destroy(pTex->Proc);
				continue;
			}
			else if(!stricmp(pTex->szProcName, "smoke"))
			{
				Smoke_Destroy(pTex->Proc);
				continue;
			}
			else if(!stricmp(pTex->szProcName, "fire"))
			{
				Fire_Destroy(pTex->Proc);
				continue;
			}
			else if(!stricmp(pTex->szProcName, "electric"))
			{
				ElectricFx_Destroy(pTex->Proc);
				continue;
			}
			else if(!stricmp(pTex->szProcName, "plasma"))
			{
				Plasma_Destroy(pTex->Proc);
				continue;
			}
			else if(!stricmp(pTex->szProcName, "bumpmap"))
			{
				BumpMap_Destroy(pTex->Proc);
				continue;
			}
			else if(!stricmp(pTex->szProcName, "flow"))
			{
				Flow_Destroy(pTex->Proc);
				continue;
			}
			else if(!stricmp(pTex->szProcName, "particle"))
			{
				Particles_Destroy(pTex->Proc);
				continue;
			}
		}
	}
}

/* ------------------------------------------------------------------------------------ */
//	Tick
/* ------------------------------------------------------------------------------------ */
void CProcedural::Tick(geFloat dwTicksIn)
{
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "TextureProc");

	if(!pSet)
		return;

	geEntity *pEntity;
	geFloat dwTicks = dwTicksIn * 0.001f;

	// Ok, we have procedurals somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		TextureProc *pTex = static_cast<TextureProc*>(geEntity_GetUserData(pEntity));

		if(pTex->DistanceFlag)
		{
			geVec3d temp;
			geVec3d_Subtract(&(pTex->origin), &CCD->Player()->Position(), &temp);

			if(geVec3d_DotProduct(&temp, &temp) > pTex->Distance*pTex->Distance)
				continue;
		}

		if(!EffectC_IsStringNull(pTex->TriggerName))
		{
			if(!GetTriggerState(pTex->TriggerName))
				continue;
		}

		if(!EffectC_IsStringNull(pTex->szTextureName) && !EffectC_IsStringNull(pTex->szProcName))
		{
			if(!stricmp(pTex->szProcName, "water"))
			{
				Water_Animate(pTex->Proc, dwTicks);
				continue;
			}
			else if(!stricmp(pTex->szProcName, "smoke"))
			{
				Smoke_Animate(pTex->Proc, dwTicks);
				continue;
			}
			else if(!stricmp(pTex->szProcName, "fire"))
			{
				Fire_Animate(pTex->Proc, dwTicks);
				continue;
			}
			else if(!stricmp(pTex->szProcName, "electric"))
			{
				ElectricFx_Animate(pTex->Proc, dwTicks);
				continue;
			}
			else if(!stricmp(pTex->szProcName, "plasma"))
			{
				Plasma_Animate(pTex->Proc, dwTicks);
				continue;
			}
			else if(!stricmp(pTex->szProcName, "bumpmap"))
			{
				BumpMap_Animate(pTex->Proc, dwTicks);
				continue;
			}
			else if(!stricmp(pTex->szProcName, "flow"))
			{
				Flow_Animate(pTex->Proc, dwTicks);
				continue;
			}
			else if(!stricmp(pTex->szProcName, "particle"))
			{
				Particles_Animate(pTex->Proc, dwTicks);
				continue;
			}
		}
	}

	return;
}

/* ------------------------------------------------------------------------------------ */
//	ReSynchronize
//
//	Correct internal timing to match current time, to make up for time lost
//	..when outside the game loop (typically in "menu mode").
/* ------------------------------------------------------------------------------------ */
int CProcedural::ReSynchronize()
{
	return RGF_SUCCESS;
}

/* ----------------------------------- END OF FILE ------------------------------------ */
