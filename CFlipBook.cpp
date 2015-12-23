/************************************************************************************//**
 * @file CFlipBook.cpp
 * @brief FlipBook class implementation
 ****************************************************************************************/

//	Include the One True Header
#include "RabidFramework.h"
#include <Ram.h>

extern geBitmap *TPool_Bitmap(const char *DefaultBmp, const char *DefaultAlpha,
							  const char *BName, const char *AName);

/* ------------------------------------------------------------------------------------ */
//	CFlipBook Constructor
/* ------------------------------------------------------------------------------------ */
CFlipBook::CFlipBook()
{
	geEntity *pEntity;

	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "FlipBook");

	if(!pSet)
		return;

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		FlipBook *S = static_cast<FlipBook*>(geEntity_GetUserData(pEntity));

		if(EffectC_IsStringNull(S->szEntityName))
		{
			char szName[128];
			geEntity_GetName(pEntity, szName, 128);
			S->szEntityName = szName;
		}

		// Ok, put this entity into the Global Entity Registry
		CCD->EntityRegistry()->AddEntity(S->szEntityName, "FlipBook");
		S->OriginOffset = S->origin;

		if(S->Model)
		{
			geVec3d ModelOrigin;
			geWorld_GetModelRotationalCenter(CCD->World(), S->Model, &ModelOrigin);
			geVec3d_Subtract(&S->origin, &ModelOrigin, &S->OriginOffset);
  		}

		S->active		= GE_FALSE;
		S->changed		= GE_FALSE;
		S->Bitmap		= NULL;
		S->EffectList	= -1;
		S->Time			= 0.0f;
		S->CurTex		= 0;
		S->CycleDir		= 1;

		if(!EffectC_IsStringNull(S->BmpNameBase))
		{
			S->Bitmap = static_cast<geBitmap**>(geRam_AllocateClear(sizeof(*(S->Bitmap))*S->BitmapCount));

			for(int i=0; i<S->BitmapCount; ++i)
			{
				char BmpName[256];
				char AlphaName[256];

				// build bmp and alpha names
				sprintf( BmpName, "%s%d%s", S->BmpNameBase, i, ".bmp" );

				if(!EffectC_IsStringNull(S->AlphaNameBase))
				{
					sprintf( AlphaName, "%s%d%s", S->AlphaNameBase, i, ".bmp" );
					S->Bitmap[i] = TPool_Bitmap(BmpName, AlphaName, NULL, NULL);
				}
				else
				{
					S->Bitmap[i] = TPool_Bitmap(BmpName, BmpName, NULL, NULL);
				}
			}

			if(!EffectC_IsStringNull(S->TextureName))
			{
				S->WorldBitmap = NULL;
				S->WorldBitmap = geWorld_GetBitmapByName(CCD->World(), S->TextureName);

// changed RF064
				if(S->WorldBitmap)
// end change Rf064
					geBitmap_ClearMips(S->WorldBitmap );
			}

			if(S->Style == 3)
				S->CurTex = (rand() % S->BitmapCount);
		}
	}
}

/* ------------------------------------------------------------------------------------ */
//	CreateSprite
//
//	Create Sprite effect
/* ------------------------------------------------------------------------------------ */
int CFlipBook::CreateSprite(FlipBook *S)
{
	int effect = -1;
    Sprite	Sp;

	memset(&Sp, 0, sizeof(Sp));

	Sp.Texture			= S->Bitmap;
	Sp.Pos				= S->origin;
	Sp.Color			= S->Color;
	Sp.TotalTextures	= S->BitmapCount;
	Sp.TextureRate		= 1.0f/S->Speed;
	Sp.Scale			= S->Scale;
	Sp.ScaleRate		= S->ScaleRate;
	Sp.Rotation			= GE_PIOVER180*S->Rotation;
	Sp.RotationRate		= GE_PIOVER180*S->RotationRate;
	Sp.AlphaRate		= S->AlphaRate;
	Sp.Style			= (Sprite_CycleStyle)S->Style;
	Sp.Color.a			= S->Alpha;

	effect = CCD->EffectManager()->Item_Add(EFF_SPRITE, static_cast<void*>(&Sp));
	return effect;
}

/* ------------------------------------------------------------------------------------ */
//	CFlipBook  Destructor
/* ------------------------------------------------------------------------------------ */
CFlipBook::~CFlipBook()
{
	geEntity *pEntity;

	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "FlipBook");

	if(!pSet)
		return;

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		FlipBook *S = static_cast<FlipBook*>(geEntity_GetUserData(pEntity));

		if(S->Bitmap)
			geRam_Free(S->Bitmap);
	}
}

/* ------------------------------------------------------------------------------------ */
//	Tick
/* ------------------------------------------------------------------------------------ */
void CFlipBook::Tick(geFloat dwTicks)
{
	geEntity *pEntity;

	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "FlipBook");

	if(!pSet)
		return;

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		FlipBook *S = static_cast<FlipBook*>(geEntity_GetUserData(pEntity));

		if(S->changed)
			continue;

		if(!EffectC_IsStringNull(S->TriggerName))
		{
			if(GetTriggerState(S->TriggerName))
			{
				if(S->active == GE_FALSE && !EffectC_IsStringNull(S->BmpNameBase))
				{
					if(EffectC_IsStringNull(S->TextureName))
						S->EffectList = CreateS(S);

					S->active = GE_TRUE;
				}
			}
			else
			{
				if(S->active == GE_TRUE)
				{
					if(EffectC_IsStringNull(S->TextureName))
					{
						if(S->EffectList != -1)
							CCD->EffectManager()->Item_Delete(EFF_SPRITE, S->EffectList);

						S->EffectList = -1;
					}

					S->active = GE_FALSE;
				}
			}
		}
		else
		{
			if(S->active == GE_FALSE && !EffectC_IsStringNull(S->BmpNameBase))
			{
				if(EffectC_IsStringNull(S->TextureName))
					S->EffectList = CreateS(S);

				S->active = GE_TRUE;
			}
		}

		if(S->active == GE_TRUE)
		{
			if(S->AtLevelStart)
				S->changed = GE_TRUE;

			if(EffectC_IsStringNull(S->TextureName))
			{
				S->origin = S->OriginOffset;

				if(SetOriginOffset(S->EntityName, S->BoneName, S->Model, &(S->origin)))
				{
					Sprite Sp;
					geVec3d_Copy(&(S->origin), &(Sp.Pos));
					CCD->EffectManager()->Item_Modify(EFF_SPRITE, S->EffectList, static_cast<void*>(&Sp), SPRITE_POS);
				}
			}
			else
			{
				if(S->WorldBitmap)
					geBitmap_BlitBitmap(S->Bitmap[S->CurTex], S->WorldBitmap);

				S->Time += dwTicks;

// changed RF064
				if(S->Time >= (1000.f/S->Speed))
// end change RF064
				{
					S->Time = 0.0f;
					switch(S->Style)
					{
					case 0:
						break;
					case 2:
						S->CurTex += S->CycleDir;
						if(S->CurTex >= S->BitmapCount || S->CurTex < 0)
						{
							S->CycleDir = -S->CycleDir;
							S->CurTex += S->CycleDir;
						}
						break;
					case 3:
						S->CurTex = (rand() % S->BitmapCount);
						break;
					default:
						S->CurTex += 1;
						if(S->CurTex >= S->BitmapCount)
							S->CurTex = 0;
						break;
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
int CFlipBook::LocateEntity(const char *szName, void **pEntityData)
{
	geEntity *pEntity;

	// Ok, check to see if there are FlipBooks in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "FlipBook");

	if(!pSet)
		return RGF_NOT_FOUND;

	// Ok, we have FlipBooks somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	    pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		FlipBook *pSource = static_cast<FlipBook*>(geEntity_GetUserData(pEntity));

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
int CFlipBook::ReSynchronize()
{
	return RGF_SUCCESS;
}


/* ----------------------------------- END OF FILE ------------------------------------ */
