/************************************************************************************//**
 * @file CEffects.cpp
 * @brief Predefined effects handler
 *
 * This file contains the class implementation for the predefined effects for
 * RGF-based games.
 * @author Ralph Deane
 *//*
 * Copyright (c) 1999 Ralph Deane; All rights reserved.
 ****************************************************************************************/

//	Include the One True Header
#include "RabidFramework.h"
#include "IniFile.h"
#include <Ram.h>

extern geSound_Def *SPool_Sound(const char *SName);
extern geBitmap *TPool_Bitmap(const char *DefaultBmp, const char *DefaultAlpha,
							  const char *BName, const char *AName);

/* ------------------------------------------------------------------------------------ */
//	Constructor
/* ------------------------------------------------------------------------------------ */
CPreEffect::CPreEffect()
{
	int i;

	for(i=0; i<MAXEXPITEM; i++)
	{
		Effects[i].Active = GE_FALSE;
		Effects[i].Data = NULL;
	}

	CIniFile AttrFile("effect.ini");

	if(!AttrFile.ReadFile())
	{
		CCD->Log()->Warning("Failed to open effect.ini file.");
		return;
	}

	std::string KeyName = AttrFile.FindFirstKey();
	std::string Type;
	int effptr = 0;

	while(KeyName != "" && effptr<MAXEXPITEM)
	{
		Type = AttrFile.GetValue(KeyName, "type");
		strncpy(Effects[effptr].Name, KeyName.c_str(), 63);
		Effects[effptr].Name[63] = 0;
		if(Type == "spray")
		{
			std::string Tname, Talpha, Vector;
			char szName[64], szAlpha[64];
			geVec3d convert;
			Spray *Sp;

			Effects[effptr].Type = EFF_SPRAY;
			Tname = AttrFile.GetValue(KeyName, "bitmapname");

			if(Tname != "")
			{
				Talpha = AttrFile.GetValue(KeyName, "alphamapname");

				if(Talpha == "")
					Talpha = Tname;

				strcpy(szName, Tname.c_str());
				strcpy(szAlpha, Talpha.c_str());

				Sp = GE_RAM_ALLOCATE_STRUCT(Spray);
				memset(Sp, 0, sizeof(Spray));
				Sp->ColorMax.a = 255.0f;
				Sp->ColorMin.a = 255.0f;
				Sp->Texture = TPool_Bitmap(szName, szAlpha, NULL, NULL);

				Vector = AttrFile.GetValue(KeyName, "angles");

				if(Vector != "")
				{
					strcpy(szName, Vector.c_str());
					Sp->Angle = Extract(szName);
				}

				Vector = AttrFile.GetValue(KeyName, "colormax");

				if(Vector != "")
				{
					strcpy(szName, Vector.c_str());
					convert = Extract(szName);
					Sp->ColorMax.r = convert.X;
					Sp->ColorMax.g = convert.Y;
					Sp->ColorMax.b = convert.Z;
				}

				Vector = AttrFile.GetValue(KeyName, "colormaxalpha");

				if(Vector != "")
				{
					Sp->ColorMax.a = (float)AttrFile.GetValueI(KeyName, "colormaxalpha");
				}

				Vector = AttrFile.GetValue(KeyName, "colormin");

				if(Vector != "")
				{
					strcpy(szName, Vector.c_str());
					convert = Extract(szName);
					Sp->ColorMin.r = convert.X;
					Sp->ColorMin.g = convert.Y;
					Sp->ColorMin.b = convert.Z;
				}

				Vector = AttrFile.GetValue(KeyName, "colorminalpha");

				if(Vector != "")
				{
					Sp->ColorMin.a = (float)AttrFile.GetValueI(KeyName, "colorminalpha");
				}

				Sp->SourceVariance	= AttrFile.GetValueI(KeyName, "sourcevariance");
				Sp->DestVariance	= AttrFile.GetValueI(KeyName, "destvariance");

				Vector = AttrFile.GetValue(KeyName, "gravity");

				if(Vector != "")
				{
					strcpy(szName, Vector.c_str());
					Sp->Gravity = Extract(szName);
				}

				Sp->MaxScale	= (float)AttrFile.GetValueF(KeyName, "maxscale");
				Sp->MinScale	= (float)AttrFile.GetValueF(KeyName, "minscale");
				Sp->MaxSpeed	= (float)AttrFile.GetValueF(KeyName, "maxspeed");
				Sp->MinSpeed	= (float)AttrFile.GetValueF(KeyName, "minspeed");
				Sp->MaxUnitLife = (float)AttrFile.GetValueF(KeyName, "maxunitlife");
				Sp->MinUnitLife = (float)AttrFile.GetValueF(KeyName, "minunitlife");
				Sp->Rate		= (float)AttrFile.GetValueF(KeyName, "particlecreationrate");
				Sp->SprayLife	= (float)AttrFile.GetValueF(KeyName, "totallife");
				Sp->Bounce		= GE_FALSE;

				Vector = AttrFile.GetValue(KeyName, "bounce");

				if(Vector == "true")
					Sp->Bounce = GE_TRUE;

				Effects[effptr].Data = Sp;
				Effects[effptr].Active = GE_TRUE;
				effptr +=1;
			}
		}
		else if(Type == "light")
		{
			std::string Vector;
			char szName[64];
			geVec3d convert;
			Glow *Lite;

			strcpy(Effects[effptr].Name, KeyName.c_str());
			Effects[effptr].Type = EFF_LIGHT;
			Lite = GE_RAM_ALLOCATE_STRUCT(Glow);
			memset(Lite, 0, sizeof(Glow));
			Lite->LightLife = 1.0f;
			Lite->Intensity = 1.0f;
			Vector = AttrFile.GetValue(KeyName, "colormax");

			if(Vector != "")
			{
				strcpy(szName, Vector.c_str());
				convert = Extract(szName);
				Lite->ColorMax.r = convert.X;
				Lite->ColorMax.g = convert.Y;
				Lite->ColorMax.b = convert.Z;
			}

			Vector = AttrFile.GetValue(KeyName, "colormin");

			if(Vector != "")
			{
				strcpy(szName, Vector.c_str());
				convert = Extract(szName);
				Lite->ColorMin.r = convert.X;
				Lite->ColorMin.g = convert.Y;
				Lite->ColorMin.b = convert.Z;
			}

			Vector = AttrFile.GetValue(KeyName, "offsetangles");

			if(Vector != "")
			{
				strcpy(szName, Vector.c_str());
				Lite->Direction = Extract(szName);
			}

			Lite->Spot = GE_FALSE;

			Type = AttrFile.GetValue(KeyName, "spotlight");

			if(Type == "true")
				Lite->Spot = GE_TRUE;

			Lite->Arc			= (float)AttrFile.GetValueF(KeyName, "arc");
			Lite->Style			= AttrFile.GetValueI(KeyName, "style");

			Lite->RadiusMax		= (float)AttrFile.GetValueF(KeyName, "radiusmax");
			Lite->RadiusMin		= (float)AttrFile.GetValueF(KeyName, "radiusmin");
			Lite->Intensity		= (float)AttrFile.GetValueF(KeyName, "intensity");
			Lite->LightLife		= (float)AttrFile.GetValueF(KeyName, "totallife");
			Lite->DoNotClip		= GE_FALSE;
			Lite->CastShadows	= GE_TRUE;

			Effects[effptr].Data = Lite;
			Effects[effptr].Active = GE_TRUE;
			effptr +=1;
		}
		else if(Type == "sprite")
		{
			std::string Tname, Talpha, Vector;
			char szName[64], szAlpha[64];
			geVec3d convert;
			Sprite *Sp;

			Effects[effptr].Type = EFF_SPRITE;
			Tname = AttrFile.GetValue(KeyName, "basebitmapname");

			if(Tname != "")
			{
				Talpha = AttrFile.GetValue(KeyName, "basealphamapname");

				if(Talpha == "")
					Talpha = Tname;

				strcpy(szName, Tname.c_str());
				strcpy(szAlpha, Talpha.c_str());
				int Count = AttrFile.GetValueI(KeyName, "bitmapcount");

				if(Count > 0)
				{
					Sp = GE_RAM_ALLOCATE_STRUCT(Sprite);
					memset(Sp, 0, sizeof(Sprite));
					Sp->TotalTextures = Count;
					Sp->Texture = (geBitmap**)geRam_AllocateClear(sizeof(geBitmap*)*Count);

					for(int i=0; i<Count; i++ )
					{
						char BmpName[256];
						char AlphaName[256];
						// build bmp and alpha names
						sprintf(BmpName, "%s%d%s", szName, i, ".bmp");
						sprintf(AlphaName, "%s%d%s", szAlpha, i, ".bmp");
						Sp->Texture[i] = TPool_Bitmap(BmpName, AlphaName, NULL, NULL);
					}

					Vector = AttrFile.GetValue(KeyName, "color");

					if(Vector != "")
					{
						strcpy(szName, Vector.c_str());
						convert = Extract(szName);
						Sp->Color.r = convert.X;
						Sp->Color.g = convert.Y;
						Sp->Color.b = convert.Z;
					}

					Sp->TextureRate		= 1.0f/(float)AttrFile.GetValueF(KeyName, "texturerate");
					Sp->Scale			= (float)AttrFile.GetValueF(KeyName, "scale");
					Sp->ScaleRate		= (float)AttrFile.GetValueF(KeyName, "scalerate");
					Sp->Rotation		= GE_PIOVER180*(float)AttrFile.GetValueF(KeyName, "rotation");
					Sp->RotationRate	= GE_PIOVER180*(float)AttrFile.GetValueF(KeyName, "rotationrate");
					Sp->Color.a			= (float)AttrFile.GetValueF(KeyName, "alpha");
					Sp->AlphaRate		= (float)AttrFile.GetValueF(KeyName, "alpharate");
					Sp->LifeTime		= (float)AttrFile.GetValueF(KeyName, "lifetime");

					Tname = AttrFile.GetValue(KeyName, "style");

					if(Tname == "")
						Sp->Style = SPRITE_CYCLE_ONCE;
					else
					{
						if(Tname == "none")
							Sp->Style = SPRITE_CYCLE_NONE;
						else if(Tname == "reset")
							Sp->Style = SPRITE_CYCLE_RESET;
						else if(Tname == "reverse")
							Sp->Style = SPRITE_CYCLE_REVERSE;
						else if(Tname == "random")
							Sp->Style = SPRITE_CYCLE_RANDOM;
						else
							Sp->Style = SPRITE_CYCLE_ONCE;
					}
					Effects[effptr].Data = Sp;
					Effects[effptr].Active = GE_TRUE;
					effptr +=1;
				}
			}
		}
		else if(Type == "sound")
		{
			Snd *Sound;
			std::string Vector;
			char szName[64];

			strcpy(Effects[effptr].Name, KeyName.c_str());
			Effects[effptr].Type = EFF_SND;
			Vector = AttrFile.GetValue(KeyName, "name");

			if(Vector != "")
			{
				Sound = GE_RAM_ALLOCATE_STRUCT(Snd);
				memset(Sound, 0, sizeof(Snd));
				strcpy(szName, Vector.c_str());
				Sound->SoundDef = SPool_Sound(szName);
				Sound->Min = CCD->GetAudibleRadius();
				Sound->Loop = GE_FALSE;
				Effects[effptr].Data = Sound;
				Effects[effptr].Active = GE_TRUE;
				effptr +=1;
			}
		}
		else if(Type == "corona")
		{
			EffCorona *C;
			std::string Vector;
			geVec3d convert;
			char szName[64];

			strcpy(Effects[effptr].Name, KeyName.c_str());
			Effects[effptr].Type = EFF_CORONA;
			C = GE_RAM_ALLOCATE_STRUCT(EffCorona);
			memset(C, 0, sizeof(EffCorona));
			C->Texture = TPool_Bitmap("corona.bmp", "a_corona.bmp", NULL, NULL);
			Vector = AttrFile.GetValue(KeyName, "color");

			if(Vector != "")
			{
				strcpy(szName, Vector.c_str());
				convert = Extract(szName);
				C->Vertex.r = convert.X;
				C->Vertex.g = convert.Y;
				C->Vertex.b = convert.Z;
			}

			C->FadeTime				= (float)AttrFile.GetValueF(KeyName, "fadetime");
			C->MinRadius			= (float)AttrFile.GetValueF(KeyName, "minradius");
			C->MaxRadius			= (float)AttrFile.GetValueF(KeyName, "maxradius");
			C->MinRadiusDistance	= (float)AttrFile.GetValueF(KeyName, "minradiusdistance");
			C->MaxRadiusDistance	= (float)AttrFile.GetValueF(KeyName, "maxradiusdistance");
			C->MaxVisibleDistance	= (float)AttrFile.GetValueF(KeyName, "maxvisibledistance");
			C->LifeTime				= (float)AttrFile.GetValueF(KeyName, "totallife");

			Effects[effptr].Data = C;
			Effects[effptr].Active = GE_TRUE;
			effptr +=1;
		}
		else if(Type == "bolt")
		{
			std::string Tname, Talpha, Vector;
			char szName[64], szAlpha[64];
			geVec3d convert;
			EBolt *Bl;

			Effects[effptr].Type = EFF_BOLT;
			Bl = GE_RAM_ALLOCATE_STRUCT(EBolt);
			memset(Bl, 0, sizeof(EBolt));

			Tname = AttrFile.GetValue(KeyName, "bitmapname");

			if(Tname != "")
			{
				Talpha = AttrFile.GetValue(KeyName, "alphamapname");

				if(Talpha == "")
					Talpha = Tname;

				strcpy(szName, Tname.c_str());
				strcpy(szAlpha, Talpha.c_str());
				Bl->Bitmap = TPool_Bitmap(szName, szAlpha, NULL, NULL);
			}
			else
				Bl->Bitmap = TPool_Bitmap("bolt.bmp", "bolt.bmp", NULL, NULL);

			Vector = AttrFile.GetValue(KeyName, "color");

			if(Vector != "")
			{
				strcpy(szName, Vector.c_str());
				convert = Extract(szName);
				Bl->Color.r = convert.X;
				Bl->Color.g = convert.Y;
				Bl->Color.b = convert.Z;
				Bl->Color.a = 255.0f;
			}

			Vector = AttrFile.GetValue(KeyName, "endoffset");

			if(Vector != "")
			{
				strcpy(szName, Vector.c_str());
				convert = Extract(szName);
				Bl->EndOffset.X = convert.X;
				Bl->EndOffset.Y = convert.Y;
				Bl->EndOffset.Z = convert.Z;
			}

			Bl->Width			= AttrFile.GetValueI(KeyName, "width");
			Bl->NumPoints		= AttrFile.GetValueI(KeyName, "numberpoints");
			Bl->Wildness		= (float)AttrFile.GetValueF(KeyName, "wildness");
			Bl->Intermittent	= GE_FALSE;

			Vector = AttrFile.GetValue(KeyName, "intermittent");

			if(Vector == "true")
				Bl->Intermittent = GE_TRUE;

			Bl->MinFrequency	= (float)AttrFile.GetValueF(KeyName, "minfrequency");
			Bl->MaxFrequency	= (float)AttrFile.GetValueF(KeyName, "maxfrequency");
			Bl->DominantColor	= AttrFile.GetValueI(KeyName, "dominantcolor");
			Bl->CompleteLife	= (float)AttrFile.GetValueF(KeyName, "totallife");

			Effects[effptr].Data = Bl;
			Effects[effptr].Active = GE_TRUE;
			effptr +=1;
		}
		else if(Type == "actorspray")
		{
			std::string Tname, Vector;
			char szName[64];
			geVec3d convert;
			ActorSpray *Sp;

			Effects[effptr].Type = EFF_ACTORSPRAY;
			Tname = AttrFile.GetValue(KeyName, "basename");

			if(Tname != "")
			{
				Sp = GE_RAM_ALLOCATE_STRUCT(ActorSpray);
				memset(Sp, 0, sizeof(ActorSpray));
				strcpy(Sp->BaseName, Tname.c_str());
				Sp->NumActors = AttrFile.GetValueI(KeyName, "numberactors");
				Sp->Style = AttrFile.GetValueI(KeyName, "style");
				Sp->Alpha = (float)AttrFile.GetValueF(KeyName, "alpha");
				Sp->AlphaRate = (float)AttrFile.GetValueF(KeyName, "alpharate");

				Vector = AttrFile.GetValue(KeyName, "baserotation");

				if(Vector != "")
				{
					strcpy(szName, Vector.c_str());
					convert = Extract(szName);

					Sp->BaseRotation.X = GE_PIOVER180*convert.X;
					Sp->BaseRotation.Y = GE_PIOVER180*convert.Y;
					Sp->BaseRotation.Z = GE_PIOVER180*convert.Z;
				}

				char Name[64];
				convert.X = 0.0f; convert.Y = 0.0f; convert.Z = 0.0f;

				for(int i=0; i<Sp->NumActors; i++)
				{
					sprintf(Name, "%s%d%s", Sp->BaseName, i, ".act");
					geActor *Actor = CCD->ActorManager()->SpawnActor(Name,
						convert, Sp->BaseRotation, "", "", NULL);

					if(!Actor)
					{
						CCD->Log()->Critical("File %s - Line %d: %s : Missing Actor '%s'",
												__FILE__, __LINE__, KeyName, Name);
						CCD->ShutdownLevel();
						delete CCD;
						CCD = NULL;
						exit(-333);
					}

					CCD->ActorManager()->RemoveActor(Actor);
					geActor_Destroy(&Actor);
					Actor = NULL;
				}

				Vector = AttrFile.GetValue(KeyName, "minrotationspeed");

				if(Vector != "")
				{
					strcpy(szName, Vector.c_str());
					convert = Extract(szName);
					Sp->MinRotationSpeed.X = GE_PIOVER180*convert.X;
					Sp->MinRotationSpeed.Y = GE_PIOVER180*convert.Y;
					Sp->MinRotationSpeed.Z = GE_PIOVER180*convert.Z;
				}

				Vector = AttrFile.GetValue(KeyName, "maxrotationspeed");

				if(Vector != "")
				{
					strcpy(szName, Vector.c_str());
					convert = Extract(szName);
					Sp->MaxRotationSpeed.X = GE_PIOVER180*convert.X;
					Sp->MaxRotationSpeed.Y = GE_PIOVER180*convert.Y;
					Sp->MaxRotationSpeed.Z = GE_PIOVER180*convert.Z;
				}

				Sp->FillColor.a = 255.0f;
				Sp->AmbientColor.a = 255.0f;

				Vector = AttrFile.GetValue(KeyName, "angles");

				if(Vector != "")
				{
					strcpy(szName, Vector.c_str());
					Sp->Angle = Extract(szName);
				}

				Vector = AttrFile.GetValue(KeyName, "fillcolor");

				if(Vector != "")
				{
					strcpy(szName, Vector.c_str());
					convert = Extract(szName);
					Sp->FillColor.r = convert.X;
					Sp->FillColor.g = convert.Y;
					Sp->FillColor.b = convert.Z;
				}

				Vector = AttrFile.GetValue(KeyName, "ambientcolor");

				if(Vector != "")
				{
					strcpy(szName, Vector.c_str());
					convert = Extract(szName);
					Sp->AmbientColor.r = convert.X;
					Sp->AmbientColor.g = convert.Y;
					Sp->AmbientColor.b = convert.Z;
				}

				Sp->AmbientLightFromFloor = GE_TRUE;

				Vector = AttrFile.GetValue(KeyName, "ambientlightfromfloor");

				if(Vector == "false")
					Sp->AmbientLightFromFloor = GE_FALSE;

				Sp->EnvironmentMapping = GE_FALSE;

				Vector = AttrFile.GetValue(KeyName, "environmentmapping");

				if(Vector == "true")
				{
					Sp->EnvironmentMapping = GE_TRUE;
					Sp->AllMaterial	= GE_FALSE;

					Vector = AttrFile.GetValue(KeyName, "allmaterial");

					if(Vector == "true")
						Sp->AllMaterial = GE_TRUE;

					Sp->PercentMapping	= (float)AttrFile.GetValueF(KeyName, "percentmapping");
					Sp->PercentMaterial = (float)AttrFile.GetValueF(KeyName, "percentmaterial");
				}

				Sp->SourceVariance	= AttrFile.GetValueI(KeyName, "sourcevariance");
				Sp->DestVariance	= AttrFile.GetValueI(KeyName, "destvariance");
				Sp->Gravity			= true;

				Vector = AttrFile.GetValue(KeyName, "gravity");

				if(Vector == "false")
					Sp->Gravity = false;

				Sp->MaxScale	= (float)AttrFile.GetValueF(KeyName, "maxscale");
				Sp->MinScale	= (float)AttrFile.GetValueF(KeyName, "minscale");
				Sp->MaxSpeed	= (float)AttrFile.GetValueF(KeyName, "maxspeed");
				Sp->MinSpeed	= (float)AttrFile.GetValueF(KeyName, "minspeed");
				Sp->MaxUnitLife = (float)AttrFile.GetValueF(KeyName, "maxunitlife");
				Sp->MinUnitLife = (float)AttrFile.GetValueF(KeyName, "minunitlife");
				Sp->Rate		= (float)AttrFile.GetValueF(KeyName, "particlecreationrate");
				Sp->SprayLife	= (float)AttrFile.GetValueF(KeyName, "totallife");
				Sp->Bounce		= GE_FALSE;

				Vector = AttrFile.GetValue(KeyName, "bounce");

				if(Vector == "true")
					Sp->Bounce = GE_TRUE;

				Sp->Solid = GE_FALSE;

				Vector = AttrFile.GetValue(KeyName, "solid");

				if(Vector == "true")
					Sp->Solid = GE_TRUE;

				Effects[effptr].Data = Sp;
				Effects[effptr].Active = GE_TRUE;
				effptr +=1;
			}
		}

		KeyName = AttrFile.FindNextKey();
	}
}


/* ------------------------------------------------------------------------------------ */
// Destructor
/* ------------------------------------------------------------------------------------ */
CPreEffect::~CPreEffect()
{
	int i;

	for(i=0; i<MAXEXPITEM; i++)
	{
		if(Effects[i].Data)
		{
			if(Effects[i].Type == EFF_SPRITE)
			{
				Sprite *Sp;
				Sp = (Sprite *)Effects[i].Data;

				if(Sp->Texture)
					geRam_Free(Sp->Texture);
			}

			geRam_Free(Effects[i].Data);
		}
	}
}


/* ------------------------------------------------------------------------------------ */
// AddEffect
/* ------------------------------------------------------------------------------------ */
int CPreEffect::AddEffect(int k, const geVec3d &Position, const geVec3d &Offset)
{
	int index = -1;

	switch(Effects[k].Type)
	{
	case EFF_SPRAY:
		{
		Spray Sp;
		geVec3d	In;
		geXForm3d	Xf;
		memcpy(&Sp, Effects[k].Data, sizeof(Sp));
		geVec3d_Copy(&Position, &(Sp.Source));
		geVec3d_Add(&(Sp.Source ), &Offset, &(Sp.Source));
		geXForm3d_SetXRotation(&Xf, (Sp.Angle.X * GE_PIOVER180));
		geXForm3d_RotateY(&Xf,		(Sp.Angle.Y - 90.0f) * GE_PIOVER180); // / 57.3f);
		geXForm3d_RotateZ(&Xf,		(Sp.Angle.Z * GE_PIOVER180)); // / 57.3f));
		geXForm3d_GetIn(&Xf, &In);
		geVec3d_Inverse(&In);
		geVec3d_Add(&(Sp.Source), &In, &(Sp.Dest));
		index = CCD->EffectManager()->Item_Add(EFF_SPRAY, &Sp);
		break;
		}
	case EFF_LIGHT:
		{
		Glow Lite;
		memcpy(&Lite, Effects[k].Data, sizeof(Lite));
		geVec3d_Copy(&(Position), &(Lite.Pos));
		geVec3d_Add(&(Lite.Pos), &Offset,&(Lite.Pos));
		index = CCD->EffectManager()->Item_Add(EFF_LIGHT, &Lite);
		break;
		}
	case EFF_SND:
		{
		Snd Sound;
		memcpy(&Sound, Effects[k].Data, sizeof(Sound));
		geVec3d_Copy(&Position, &(Sound.Pos));
		geVec3d_Add(&(Sound.Pos), &Offset, &(Sound.Pos));
		index = CCD->EffectManager()->Item_Add(EFF_SND, &Sound);
		break;
		}
	case EFF_SPRITE:
		{
		Sprite S;
		memcpy(&S, Effects[k].Data, sizeof(S));
		geVec3d_Copy(&Position, &(S.Pos));
		geVec3d_Add(&(S.Pos), &Offset, &(S.Pos));
		index = CCD->EffectManager()->Item_Add(EFF_SPRITE, &S);
		break;
		}
	case EFF_CORONA:
		{
		EffCorona C;
		memcpy(&C, Effects[k].Data, sizeof(C));
		C.Vertex.X = Position.X + Offset.X;
		C.Vertex.Y = Position.Y + Offset.Y;
		C.Vertex.Z = Position.Z + Offset.Z;
		index = CCD->EffectManager()->Item_Add(EFF_CORONA, &C);
		break;
		}
	case EFF_BOLT:
		{
		EBolt Bl;
		memcpy(&Bl, Effects[k].Data, sizeof(Bl));
		geVec3d_Copy(&Position, &(Bl.Start));
		geVec3d_Add(&(Bl.Start), &Offset, &(Bl.Start));
		geVec3d_Add(&(Bl.Start), &(Bl.EndOffset), &(Bl.End));
		index = CCD->EffectManager()->Item_Add(EFF_BOLT, &Bl);
		break;
		}
	case EFF_ACTORSPRAY:
		{
		ActorSpray aSp;
		memcpy(&aSp, Effects[k].Data, sizeof(aSp));
		geVec3d_Copy(&Position, &(aSp.Source));
		geVec3d_Add(&(aSp.Source), &Offset,&(aSp.Source));
		geXForm3d_SetZRotation(&Xf, (aSp.Angle.Z * GE_PIOVER180));
		geXForm3d_RotateX(&Xf,-(aSp.Angle.X * GE_PIOVER180)); // / 57.3f);
		geXForm3d_RotateY(&Xf, (aSp.Angle.Y - 90.0f) * GE_PIOVER180); // / 57.3f);
		geXForm3d_GetIn(&Xf, &In);
		geVec3d_Inverse(&In);
		geVec3d_Add(&(aSp.Source), &In, &(aSp.Dest));
		index = CCD->EffectManager()->Item_Add(EFF_ACTORSPRAY, &aSp);
		break;
		}
	}

	return index;
}


/* ----------------------------------- END OF FILE ------------------------------------ */
