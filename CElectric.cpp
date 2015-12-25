/************************************************************************************//**
 * @file CElectric.cpp
 * @brief Electric Bolt handler
 *
 * This file contains the class implementation for the CElectric class that
 * encapsulates all Electric bolt special effects for RGF-based games.
 * @author Ralph Deane
 *//*
 * Copyright (c) 1999 Ralph Deane; All rights reserved.
 ****************************************************************************************/

//	Include the One True Header
#include "RabidFramework.h"

extern geSound_Def *SPool_Sound(const char *SName);
extern geBitmap *TPool_Bitmap(const char *DefaultBmp, const char *DefaultAlpha,
							  const char *BName, const char *AName);

// Prototypes
Electric_BoltEffect* Electric_BoltEffectCreate(
	geBitmap				*Texture,		/* The texture we map onto the bolt			*/
	geBitmap				*Texture2,		/* The texture we map onto the bolt			*/
	int 					NumPolys,		/* Number of polys, must be power of 2		*/
	int						Width,			/* Width in world units of the bolt			*/
	geFloat 				Wildness);		/* How wild the bolt is (0 to 1 inclusive)	*/

void Electric_BoltEffectDestroy(Electric_BoltEffect *Effect);

void Electric_BoltEffectAnimate(
	Electric_BoltEffect		*Effect,
	const geVec3d			*start,			/* Starting point of the bolt				*/
	const geVec3d			*end);			/* Ending point of the bolt					*/

void Electric_BoltEffectRender(
	Electric_BoltEffect		*Effect,		/* Bolt to render							*/
	const geXForm3d			*XForm);		/* Transform of our point of view			*/

void Electric_BoltEffectSetColorInfo(
	Electric_BoltEffect		*Effect,
	GE_RGBA					*BaseColor,		/* Base color of the bolt (2 colors should be the same */
	int						DominantColor);	/* Which color is the one to leave fixed */



/* ------------------------------------------------------------------------------------ */
// Electric functions
/* ------------------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------------------ */
//	logBase2
/* ------------------------------------------------------------------------------------ */
static int logBase2(int n)
{
	int	i = 0;

	while(!(n & 1))
	{
		n = n>>1;
		++i;
	}

	return i;
}

/* ------------------------------------------------------------------------------------ */
//	IsPowerOf2
/* ------------------------------------------------------------------------------------ */
static geBoolean IsPowerOf2(int n)
{
	if(n == 0)
		return GE_TRUE;

	while(!(n & 1))
		n = n>>1;

	if(n & ~1)
		return GE_FALSE;

	return GE_TRUE;
}

/* ------------------------------------------------------------------------------------ */
//	Electric_BoltEffectCreate
/* ------------------------------------------------------------------------------------ */
Electric_BoltEffect* Electric_BoltEffectCreate(geBitmap	*Bitmap,
											   geBitmap	*Bitmap2,
											   int			NumPolys,
											   int			Width,
											   geFloat		Wildness)
{
	Electric_BoltEffect *be;
	GE_RGBA				color;

	be = (Electric_BoltEffect*)malloc(sizeof(*be));

	if(!be)
		return be;

	memset(be, 0, sizeof(*be));

	be->beCenterPoints = (geVec3d*)malloc(sizeof(*be->beCenterPoints) * (NumPolys + 1));

	if(!be->beCenterPoints)
	{
		if(be->beCenterPoints)
			free(be->beCenterPoints);

		return NULL;
	}

	be->beBitmap	= Bitmap;
	be->beNumPoints	= NumPolys;
	be->beWildness	= Wildness;
	be->beWidth		= Width;

	color.r = 160.0f;
	color.g = 160.0f;
	color.b = 255.0f;

	Electric_BoltEffectSetColorInfo(be, &color, ELECTRIC_BOLT_BLUEDOMINANT);

	return be;
}

/* ------------------------------------------------------------------------------------ */
//	Electric_BoltEffectDestroy
/* ------------------------------------------------------------------------------------ */
void Electric_BoltEffectDestroy(Electric_BoltEffect *Effect)
{
	free(Effect->beCenterPoints);
	free(Effect);
}

/* ------------------------------------------------------------------------------------ */
//	GaussRand
/* ------------------------------------------------------------------------------------ */
static geFloat GaussRand(void)
{
	int	i;
	int	r;

	r = 0;

	for(i=0; i<6; ++i)
		r = r + rand() - rand();

	return static_cast<geFloat>(r)/(static_cast<geFloat>(RAND_MAX)*6.0f);
}

/* ------------------------------------------------------------------------------------ */
//	subdivide
/* ------------------------------------------------------------------------------------ */
static void subdivide(Electric_BoltEffect	*be,
					  const geVec3d			*start,
					  const geVec3d			*end,
					  geFloat	 			s,
					  int	 				n)
{
	geVec3d	tmp;

	if(n == 0)
	{
		be->beCurrentPoint++;
		*be->beCurrentPoint = *end;
		return;
	}

	tmp.X = (end->X + start->X) * 0.5f + s * GaussRand();
	tmp.Y = (end->Y + start->Y) * 0.5f + s * GaussRand();
	tmp.Z = (end->Z + start->Z) * 0.5f + s * GaussRand();

	subdivide(be, start, &tmp,	s * 0.5f, n - 1);
	subdivide(be, &tmp,  end,	s * 0.5f, n - 1);
}

#define	LIGHTNINGWIDTH 8.0f

/* ------------------------------------------------------------------------------------ */
//	genLightning
/* ------------------------------------------------------------------------------------ */
static void genLightning(Electric_BoltEffect	*be,
						 int					RangeLow,
						 int 					RangeHigh,
						 const geVec3d			*start,
						 const geVec3d			*end)
{
	geFloat	length;
	int		seed;

	/* Manhattan length is good enough for this */
	length = fabs(start->X - end->X) + fabs(start->Y - end->Y) + fabs(start->Z - end->Z);

	seed = rand();

	srand(seed);

	be->beCurrentPoint					= be->beCenterPoints + RangeLow;
	be->beCenterPoints[RangeLow]		= *start;
	be->beCenterPoints[RangeHigh] 		= *end;

	subdivide(be, start, end, length * be->beWildness, logBase2(RangeHigh - RangeLow));
}

/* ------------------------------------------------------------------------------------ */
//	Electric_BoltEffectSetColorInfo
/* ------------------------------------------------------------------------------------ */
void Electric_BoltEffectSetColorInfo(Electric_BoltEffect	*Effect,
									 GE_RGBA				*BaseColor,
									 int					DominantColor)
{
	Effect->beBaseColors[0]		= BaseColor->r;
	Effect->beBaseColors[1]		= BaseColor->g;
	Effect->beBaseColors[2]		= BaseColor->b;
	Effect->beCurrentColors[0]	= BaseColor->r;
	Effect->beCurrentColors[1]	= BaseColor->g;
	Effect->beCurrentColors[2]	= BaseColor->b;
	Effect->beDominantColor 	= DominantColor;
}

/* ------------------------------------------------------------------------------------ */
//	Electric_BoltEffectAnimate
/* ------------------------------------------------------------------------------------ */
void Electric_BoltEffectAnimate(Electric_BoltEffect *Effect,
								const geVec3d		*start,
								const geVec3d		*end)
{
	int		dominant;
	int		nonDominant1;
	int		nonDominant2;
	geVec3d	SubdivideStart;
	geVec3d	SubdivideEnd;
	int		LowIndex;
	int		HighIndex;

	Effect->beStart = *start;
	Effect->beEnd	= *end;

	dominant = Effect->beDominantColor;
	nonDominant1 = (dominant + 1) % 3;
	nonDominant2 = (dominant + 2) % 3;

	if(Effect->beBaseColors[nonDominant1] == Effect->beCurrentColors[nonDominant1])
	{
		int	DecayRate;
		int	Spike;
		int Mod = static_cast<int>(Effect->beBaseColors[dominant] - Effect->beBaseColors[nonDominant1]);

		if(Mod <= 0)
			Mod = 1;

		DecayRate = rand() % Mod;
		DecayRate = max(DecayRate, 5);
		Effect->beDecayRate = DecayRate;

		if(Effect->beBaseColors[nonDominant1] >= 1.0f)
			Spike = rand() % static_cast<int>(Effect->beBaseColors[nonDominant1]);
		else
			Spike = 0;

		Effect->beCurrentColors[nonDominant1] -= Spike;
		Effect->beCurrentColors[nonDominant2] -= Spike;
	}
	else
	{
		Effect->beCurrentColors[nonDominant1] += Effect->beDecayRate;
		Effect->beCurrentColors[nonDominant2] += Effect->beDecayRate;

		if(Effect->beCurrentColors[nonDominant1] > Effect->beBaseColors[nonDominant1])
		{
			Effect->beCurrentColors[nonDominant1] = Effect->beBaseColors[nonDominant1];
			Effect->beCurrentColors[nonDominant2] = Effect->beBaseColors[nonDominant2];
		}
	}

	if(Effect->beInitialized && Effect->beNumPoints > 64)
	{
		int P1 = 0;
		int P2 = 0;
		int P3 = 0;
		int P4 = 0;

		switch(rand() % 7)
		{
			case 0:
				genLightning(Effect, 0, Effect->beNumPoints, start, end);
				return;
			case 1:
			case 2:
			case 3:
				P1 = 0;
				P2 = Effect->beNumPoints / 2;
				P3 = P2 + Effect->beNumPoints / 4;
				P4 = Effect->beNumPoints;
				break;
			case 4:
			case 5:
			case 6:
				P1 = 0;
				P3 = Effect->beNumPoints / 2;
				P2 = P3 - Effect->beNumPoints / 4;
				P4 = Effect->beNumPoints;
				break;
		}

		SubdivideStart = Effect->beCenterPoints[P1];
		SubdivideEnd = Effect->beCenterPoints[P2];
		genLightning(Effect, P1, P2, &SubdivideStart, &SubdivideEnd);
		SubdivideStart = Effect->beCenterPoints[P2];
		SubdivideEnd = Effect->beCenterPoints[P3];
		genLightning(Effect, P2, P3, &SubdivideStart, &SubdivideEnd);
		SubdivideStart = Effect->beCenterPoints[P3];
		SubdivideEnd = Effect->beCenterPoints[P4];
		genLightning(Effect, P3, P4, &SubdivideStart, &SubdivideEnd);
	}
	else
	{
		Effect->beInitialized = 1;
		LowIndex = 0;
		HighIndex = Effect->beNumPoints;
		SubdivideStart = *start;
		SubdivideEnd   = *end;

		genLightning(Effect, LowIndex, HighIndex, &SubdivideStart, &SubdivideEnd);
	}
}

#define	LIGHTNINGALPHA	255.0f //cell division

/* ------------------------------------------------------------------------------------ */
//	Electric_BoltEffectRender
/* ------------------------------------------------------------------------------------ */
void Electric_BoltEffectRender(Electric_BoltEffect	*be,
							   const geXForm3d		*XForm)
{
	geVec3d			perp;
	geVec3d			temp;
	geVec3d			in;
	GE_LVertex 		verts[4];
	int				i;

	geVec3d_Subtract(&be->beStart, &be->beEnd, &temp);
	geXForm3d_GetIn(XForm, &in);

	geVec3d_CrossProduct(&in, &temp, &perp);
	geVec3d_Normalize(&perp);

	geVec3d_Scale(&perp, be->beWidth*0.5f, &perp);

	/*
		We've got the perpendicular to the camera in the
		rough direction of the electric bolt center.  Walk
		the left and right sides, constructing verts, then
		do the drawing.
	*/

	for(i=0; i<be->beNumPoints-1; ++i)
	{
		geVec3d	temp;

		geVec3d_Subtract(&be->beCenterPoints[i], &perp, &temp);
		verts[0].X = temp.X;
		verts[0].Y = temp.Y;
		verts[0].Z = temp.Z;
		verts[0].u = 0.0f;
		verts[0].v = 0.0f;
		verts[0].r = be->beCurrentColors[0];
		verts[0].g = be->beCurrentColors[1];
		verts[0].b = be->beCurrentColors[2];
		verts[0].a = LIGHTNINGALPHA;

		geVec3d_Subtract(&be->beCenterPoints[i + 1], &perp, &temp);
		verts[1].X = temp.X;
		verts[1].Y = temp.Y;
		verts[1].Z = temp.Z;
		verts[1].u = 0.0f;
		verts[1].v = 1.0f;
		verts[1].r = be->beCurrentColors[0];
		verts[1].g = be->beCurrentColors[1];
		verts[1].b = be->beCurrentColors[2];
		verts[1].a = LIGHTNINGALPHA;

		geVec3d_Add(&be->beCenterPoints[i + 1], &perp, &temp);
		verts[2].X = temp.X;
		verts[2].Y = temp.Y;
		verts[2].Z = temp.Z;
		verts[2].u = 1.0f;
		verts[2].v = 1.0f;
		verts[2].r = be->beCurrentColors[0];
		verts[2].g = be->beCurrentColors[1];
		verts[2].b = be->beCurrentColors[2];
		verts[2].a = LIGHTNINGALPHA;

		geVec3d_Add(&be->beCenterPoints[i], &perp, &temp);
		verts[3].X = temp.X;
		verts[3].Y = temp.Y;
		verts[3].Z = temp.Z;
		verts[3].u = 1.0f;
		verts[3].v = 0.0f;
		verts[3].r = be->beCurrentColors[0];
		verts[3].g = be->beCurrentColors[1];
		verts[3].b = be->beCurrentColors[2];
		verts[3].a = LIGHTNINGALPHA;

		// cell division
		// long nLeafID;
		// geWorld_GetLeaf(CCD->World(), &temp, &nLeafID);
		// If the BSP leaf the entity is in might be visible,
		// ..go ahead and add it.
		// if(geWorld_MightSeeLeaf(CCD->World(), nLeafID) == GE_TRUE)
		// cell division
		geWorld_AddPolyOnce(CCD->World(),
							verts,
							4,
							be->beBitmap,
							GE_TEXTURED_POLY,
							GE_RENDER_DO_NOT_OCCLUDE_OTHERS | GE_RENDER_DEPTH_SORT_BF ,
							1.0f);
	}
}

/* ------------------------------------------------------------------------------------ */
//	Constructor
/* ------------------------------------------------------------------------------------ */
CElectric::CElectric()
{
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "ElectricBolt");

	if(!pSet)
		return;

	geEntity *pEntity;
	// SPool_Sound("loopbzzt.wav"); //cell division
	// SPool_Sound("onebzzt.wav"); //cell division

	// Ok, we have electric bolts somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		ElectricBolt *pBolt = static_cast<ElectricBolt*>(geEntity_GetUserData(pEntity));

		if(EffectC_IsStringNull(pBolt->szEntityName))
		{
			char szName[128];
			geEntity_GetName(pEntity, szName, 128);
			pBolt->szEntityName = szName;
		}

		// Ok, put this entity into the Global Entity Registry
		CCD->EntityRegistry()->AddEntity(pBolt->szEntityName, "ElectricBolt");
		pBolt->OriginOffset = pBolt->origin;

		if(pBolt->Model)
		{
			geVec3d ModelOrigin;
			geWorld_GetModelRotationalCenter(CCD->World(), pBolt->Model, &ModelOrigin);
			geVec3d_Subtract(&pBolt->origin, &ModelOrigin, &pBolt->OriginOffset);
		}

		pBolt->effect	= -1;
		pBolt->active		= false;
		pBolt->Bitmap		= TPool_Bitmap("bolt.bmp", "bolt.bmp", pBolt->BmpName, pBolt->AlphaName);

		if(!pBolt->Bitmap)
		{
			char szBug[256];
			sprintf(szBug, "[WARNING] File %s - Line %d: %s: Failed to create Bolt bitmap : %s %s\n",
					__FILE__, __LINE__, pBolt->szEntityName, pBolt->BmpName, pBolt->AlphaName);
			CCD->ReportError(szBug, false);
			continue;
		}

		if(!pBolt->Terminus)
		{
			char szBug[256];
			sprintf(szBug, "[WARNING] File %s - Line %d: ElectricBolt entity '%s' has no terminus.\n",
					__FILE__, __LINE__, pBolt->szEntityName);
			CCD->ReportError(szBug, false);
			continue;
		}

		pBolt->Bolt = Electric_BoltEffectCreate(pBolt->Bitmap,
												NULL,
												pBolt->NumPoints,
												pBolt->Width,
												pBolt->Wildness);
		if(!pBolt->Bolt)
		{
			char szBug[256];
			sprintf(szBug, "[WARNING] File %s - Line %d: %s: Failed to create Bolt\n",
					__FILE__, __LINE__, pBolt->szEntityName);
			CCD->ReportError(szBug, false);
			continue;
		}

		Electric_BoltEffectSetColorInfo(pBolt->Bolt, &pBolt->Color, pBolt->DominantColor);
		pBolt->bState = false;
		pBolt->DoingDamage = false;

// changed RF063
		if(!EffectC_IsStringNull(pBolt->SoundFile))
			SPool_Sound(pBolt->SoundFile);
// end change RF063

	}

	pSet = geWorld_GetEntitySet(CCD->World(), "ElectricBoltTerminus");

	if(!pSet)
		return;					// Bogus, but there it is...

	// Ok, we have bolt terminuses somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		ElectricBoltTerminus *pTerminus = static_cast<ElectricBoltTerminus*>(geEntity_GetUserData(pEntity));

		if(EffectC_IsStringNull(pTerminus->szEntityName))
		{
			char szName[128];
			geEntity_GetName(pEntity, szName, 128);
			pTerminus->szEntityName = szName;
		}

		// Ok, put this entity into the Global Entity Registry
		CCD->EntityRegistry()->AddEntity(pTerminus->szEntityName, "ElectricBoltTerminus");
		pTerminus->OriginOffset = pTerminus->origin;

		if(pTerminus->Model)
		{
			geVec3d ModelOrigin;
	    	geWorld_GetModelRotationalCenter(CCD->World(), pTerminus->Model, &ModelOrigin);
			geVec3d_Subtract(&pTerminus->origin, &ModelOrigin, &pTerminus->OriginOffset);
  		}
	}

	return;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
int CElectric::Create(const geVec3d &Origin, ElectricBolt *pBolt)
{
	int effect = -1;
	//cell division - removes audio from bolts
	/*
	Snd Sound;
	memset( &Sound, 0, sizeof( Sound ) );
    geVec3d_Copy( &(Origin), &( Sound.Pos ) );
    Sound.Min=pBolt->fRadius;
    Sound.Loop=!pBolt->Intermittent;
	if(EffectC_IsStringNull(pBolt->SoundFile))
	{
		if(!pBolt->Intermittent)
			Sound.SoundDef=SPool_Sound("loopbzzt.wav");
		else
			Sound.SoundDef=SPool_Sound("onebzzt.wav");
	}
	else
		Sound.SoundDef=SPool_Sound(pBolt->SoundFile);
	if(!Sound.SoundDef)
	{
		char szError[256];
		sprintf(szError,"Can't open audio file '%s'\n", pBolt->SoundFile);
		CCD->ReportError(szError, false);
	}
	if(Sound.SoundDef!=NULL)
		effect = CCD->EffectManager()->Item_Add(EFF_SND, (void *)&Sound);
	*/
	// end cell division
	return effect;
}

/* ------------------------------------------------------------------------------------ */
//	Destructor
/* ------------------------------------------------------------------------------------ */
CElectric::~CElectric()
{
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "ElectricBolt");

	if(!pSet)
		return;

	geEntity *pEntity;

	// Ok, we have electric bolts somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		ElectricBolt *pBolt = (ElectricBolt*)geEntity_GetUserData(pEntity);

		if(pBolt->Bolt)
			Electric_BoltEffectDestroy(pBolt->Bolt);
	}

	return;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
static geFloat frand(geFloat Low, geFloat High)
{
	geFloat	Range;

	//assert(High > Low);

	Range = High - Low;

	return ((geFloat)(((rand() % 1000) + 1))) / 1000.0f * Range + Low;
}

/* ------------------------------------------------------------------------------------ */
// Tick - do time based actions
/* ------------------------------------------------------------------------------------ */
geBoolean CElectric::Tick(geFloat dwTicks)
{
	if(CCD->World() == NULL)
		return GE_TRUE;

	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "ElectricBoltTerminus");

	if(!pSet)
		return GE_TRUE;					// Bogus, but there it is...

	geEntity *pEntity;

	// Ok, we have bolt terminuses somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		ElectricBoltTerminus *pTerminus = static_cast<ElectricBoltTerminus*>(geEntity_GetUserData(pEntity));
		pTerminus->origin = pTerminus->OriginOffset;
		SetOriginOffset(pTerminus->EntityName, pTerminus->BoneName, pTerminus->Model, &(pTerminus->origin));
	}

	pSet = geWorld_GetEntitySet(CCD->World(), "ElectricBolt");

	if(!pSet)
		return GE_TRUE;

	// Mode
	geXForm3d XForm = CCD->CameraManager()->ViewPoint();

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		ElectricBolt	*Bolt;
		geVec3d			MidPoint;
		int32			Leaf;

		Bolt = static_cast<ElectricBolt*>(geEntity_GetUserData(pEntity));

		if(Bolt->DoingDamage)
		{
			Bolt->DTime += dwTicks*0.001f;

			if(Bolt->DTime >= Bolt->DamageTime)
				Bolt->DoingDamage = GE_FALSE;
		}

		if(!EffectC_IsStringNull(Bolt->TriggerName))
		{
			if(GetTriggerState(Bolt->TriggerName))
			{
				if(Bolt->active == GE_FALSE)
				{
					if(!Bolt->Intermittent)
						Bolt->effect = Create(Bolt->origin, Bolt);

					Bolt->active = GE_TRUE;
				}
			}
			else
			{
				if(Bolt->effect != -1)
				{
					CCD->EffectManager()->Item_Delete(EFF_SND, Bolt->effect);
					Bolt->effect = -1;
				}

				Bolt->active = GE_FALSE;
			}
		}
		else
		{
			if(Bolt->active == GE_FALSE)
			{
				if(!Bolt->Intermittent)
					Bolt->effect = Create(Bolt->origin, Bolt);

				Bolt->active = GE_TRUE;
			}
		}

		if(Bolt->active == GE_TRUE)
		{
			Bolt->origin = Bolt->OriginOffset;
			SetOriginOffset(Bolt->EntityName, Bolt->BoneName, Bolt->Model, &(Bolt->origin));
			geVec3d_Subtract(&(Bolt->Terminus->origin), &(Bolt->origin), &MidPoint);
			geVec3d_AddScaled(&(Bolt->origin), &MidPoint, 0.5f, &MidPoint);

			geWorld_GetLeaf(CCD->World(), &MidPoint, &Leaf);

			Bolt->LastTime += dwTicks;

			if((Bolt->Intermittent == GE_FALSE) ||
				(Bolt->LastTime - Bolt->LastBoltTime > frand(Bolt->MaxFrequency, Bolt->MinFrequency)))
			{
				Electric_BoltEffectAnimate(Bolt->Bolt,
											&Bolt->origin,
											&Bolt->Terminus->origin);

				if(Bolt->Intermittent)
					Create(Bolt->origin, Bolt);

				Bolt->LastBoltTime = Bolt->LastTime;
			}

			if((Bolt->Intermittent == GE_FALSE))
			{
				Snd Sound;
				geVec3d_Copy(&(MidPoint), &(Sound.Pos));

				if(Bolt->effect != -1)
					CCD->EffectManager()->Item_Modify(EFF_SND, Bolt->effect, static_cast<void*>(&Sound), SND_POS);
			}

			if(Bolt->LastTime - Bolt->LastBoltTime <= LIGHTNINGSTROKEDURATION)
			{
				if(geWorld_MightSeeLeaf(CCD->World(), Leaf) == GE_TRUE)
					Electric_BoltEffectRender(Bolt->Bolt, &XForm);

				CheckCollision(Bolt);
			}
		}
	}

	return GE_TRUE;
}

/* ------------------------------------------------------------------------------------ */
//	CheckCollision
/* ------------------------------------------------------------------------------------ */
void CElectric::CheckCollision(ElectricBolt *Bolt)
{
	GE_Collision	Collision;
	Electric_BoltEffect *be = Bolt->Bolt;

	Bolt->bState = GE_FALSE;

	for(int i=0; i<be->beNumPoints-1; ++i)
	{
		if(CCD->Collision()->CheckForWCollision(NULL, NULL,
				be->beCenterPoints[i], be->beCenterPoints[i + 1], &Collision, NULL))
		{
			int percent;

			if(!CCD->Damage()->IsDestroyable(Collision.Model, &percent))
				Collision.Model = NULL;

			if(Collision.Actor || Collision.Model)
			{
				Bolt->bState = GE_TRUE;

				if(Bolt->DoDamage)
				{
					if(!Bolt->DoingDamage)
					{
						Bolt->DoingDamage = GE_TRUE;
						Bolt->DTime = 0.0f;
// change RF063
						if(Collision.Actor)
							CCD->Damage()->DamageActor(Collision.Actor, Bolt->DamageAmt, Bolt->DamageAttribute, Bolt->DamageAltAmt, Bolt->DamageAltAttribute, "Bolt");

						if(Collision.Model)
							CCD->Damage()->DamageModel(Collision.Model, Bolt->DamageAmt, Bolt->DamageAttribute, Bolt->DamageAltAmt, Bolt->DamageAltAttribute);
// end change RF063
						return;
					}
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
int CElectric::LocateEntity(const char *szName, void **pEntityData)
{
	//	This is a SPECIAL CASE due to electric bolt and terminus pairing.
	//	..If the pEntityData pointer contains the value 0, we're looking
	//	..for an ElectricBolt, otherwise we're looking for an
	//	..ElectricBoltTerminus

	if(*pEntityData == 0)
	{
		// Ok, check to see if there are electric bolts in this world
		geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "ElectricBolt");

		if(!pSet)
			return RGF_NOT_FOUND;									// No electric bolts

		geEntity *pEntity;

		// Ok, we have electric bolts. Dig through 'em all.
		for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
			pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
		{
			ElectricBolt *pTheEntity = static_cast<ElectricBolt*>(geEntity_GetUserData(pEntity));

			if(!strcmp(pTheEntity->szEntityName, szName))
			{
				*pEntityData = static_cast<void*>(pTheEntity);
				return RGF_SUCCESS;
			}
		}

		return RGF_NOT_FOUND;						// Not an electric bolt
	}
	else
	{
		// Ok, check to see if there are electric bolt terminii in this world
		geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "ElectricBoltTerminus");

		if(!pSet)
			return RGF_NOT_FOUND;					// No electric bolts

		geEntity *pEntity;

		// Ok, we have electric bolt termini. Dig through 'em all.
		for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
			pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
		{
			ElectricBoltTerminus *pTheEntity = static_cast<ElectricBoltTerminus*>(geEntity_GetUserData(pEntity));

			if(!strcmp(pTheEntity->szEntityName, szName))
			{
				*pEntityData = static_cast<void*>(pTheEntity);
				return RGF_SUCCESS;
			}
		}

		return RGF_NOT_FOUND;						// Not an electric bolt
	}

	return RGF_NOT_FOUND;								// Sorry, no such entity here
}

/* ------------------------------------------------------------------------------------ */
//	ReSynchronize
//
//	Correct internal timing to match current time, to make up for time lost
//	..when outside the game loop (typically in "menu mode").
/* ------------------------------------------------------------------------------------ */
int CElectric::ReSynchronize()
{
	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	SaveTo
//
//	Save the current state of every  ElectricBolt in the current world
//	..off to an open file.
/* ------------------------------------------------------------------------------------ */
int CElectric::SaveTo(FILE *SaveFD, bool type)
{
	// Ok, check to see if there are  ElectricBolt in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "ElectricBolt");

	if(!pSet)
		return RGF_SUCCESS;

	geEntity *pEntity;

	// Ok, we have logic gates somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		ElectricBolt *pSource = static_cast<ElectricBolt*>(geEntity_GetUserData(pEntity));

		WRITEDATA(type, &pSource->active,		sizeof(geBoolean),	1, SaveFD);
		WRITEDATA(type, &pSource->bState,		sizeof(geBoolean),	1, SaveFD);
		WRITEDATA(type, &pSource->DTime,		sizeof(geFloat),	1, SaveFD);
		WRITEDATA(type, &pSource->DoingDamage,	sizeof(geBoolean),	1, SaveFD);
	}

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	RestoreFrom
//
//	Restore the state of every ElectricBolt in the current world from an
//	..open file.
/* ------------------------------------------------------------------------------------ */
int CElectric::RestoreFrom(FILE *RestoreFD, bool type)
{
	// Ok, check to see if there are  ElectricBolt in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "ElectricBolt");

	if(!pSet)
		return RGF_SUCCESS;									// No gates, whatever...

	geEntity *pEntity;

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		ElectricBolt *pSource = static_cast<ElectricBolt*>(geEntity_GetUserData(pEntity));

		READDATA(type, &pSource->active,		sizeof(geBoolean),	1, RestoreFD);
		READDATA(type, &pSource->bState,		sizeof(geBoolean),	1, RestoreFD);
		READDATA(type, &pSource->DTime,			sizeof(geFloat),	1, RestoreFD);
		READDATA(type, &pSource->DoingDamage,	sizeof(geBoolean),	1, RestoreFD);
	}

	return RGF_SUCCESS;
}


/* ----------------------------------- END OF FILE ------------------------------------ */
