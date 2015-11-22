/*

    CEffects.cpp               predefined effects handler

	(c) 1999 Ralph Deane

	This file contains the class implementation for the predefined 
	effects for RGF-based games.
*/

//	Include the One True Header

#include "RabidFramework.h"
#include <Ram.h>

extern geBitmap *TPool_Bitmap(char *DefaultBmp, char *DefaultAlpha, char *BName, char *AName);
extern geSound_Def *SPool_Sound(char *SName);

CPreEffect::CPreEffect()
{
	int i;
	
	for(i=0;i<MAXEXPITEM;i++)
	{
		Effects[i].Active = false;
		Effects[i].Data = NULL;
	}
	
	CIniFile AttrFile("effect.ini");
	if(!AttrFile.ReadFile())
	{
		CCD->ReportError("Can't open effects initialization file", false);
		return;
	}
	CString KeyName = AttrFile.FindFirstKey();
	CString Type;
	int effptr = 0;
	while(KeyName != "")
	{
		Type = AttrFile.GetValue(KeyName, "type");
		strcpy(Effects[effptr].Name,KeyName);
		if(Type=="spray")
		{
			CString Tname, Talpha, Vector;
			char szName[64], szAlpha[64];
			geVec3d convert;
			Spray *Sp;
			Effects[effptr].Type = EFF_SPRAY;
			Tname = AttrFile.GetValue(KeyName, "bitmapname");
			if(Tname!="")
			{
				Talpha = AttrFile.GetValue(KeyName, "alphamapname");
				if(Talpha=="")
					Talpha = Tname;
				strcpy(szName,Tname);
				strcpy(szAlpha,Talpha);
				Sp = GE_RAM_ALLOCATE_STRUCT(Spray);
				memset( Sp, 0, sizeof(Spray) );
				Sp->ColorMax.a = 255.0f;
				Sp->ColorMin.a = 255.0f;
				Sp->Texture = TPool_Bitmap(szName, szAlpha, NULL, NULL);
				Vector = AttrFile.GetValue(KeyName, "angles");
				if(Vector!="")
				{
					strcpy(szName,Vector);
					Sp->Dest = Extract(szName);
				}
				Vector = AttrFile.GetValue(KeyName, "colormax");
				if(Vector!="")
				{
					strcpy(szName,Vector);
					convert = Extract(szName);
					Sp->ColorMax.r = convert.X;
					Sp->ColorMax.g = convert.Y;
					Sp->ColorMax.b = convert.Z;
				}
				Vector = AttrFile.GetValue(KeyName, "colormaxalpha");
				if(Vector!="")
				{
					Sp->ColorMax.a = (float)AttrFile.GetValueI(KeyName, "colormaxalpha");
				}
				Vector = AttrFile.GetValue(KeyName, "colormin");
				if(Vector!="")
				{
					strcpy(szName,Vector);
					convert = Extract(szName);
					Sp->ColorMin.r = convert.X;
					Sp->ColorMin.g = convert.Y;
					Sp->ColorMin.b = convert.Z;
				}
				Vector = AttrFile.GetValue(KeyName, "colorminalpha");
				if(Vector!="")
				{
					Sp->ColorMin.a = (float)AttrFile.GetValueI(KeyName, "colorminalpha");
				}
				Sp->SourceVariance = AttrFile.GetValueI(KeyName, "sourcevariance");
				Sp->DestVariance = AttrFile.GetValueI(KeyName, "destvariance");
				Vector = AttrFile.GetValue(KeyName, "gravity");
				if(Vector!="")
				{
					strcpy(szName,Vector);
					Sp->Gravity = Extract(szName);
				}
				Sp->MaxScale = (float)AttrFile.GetValueF(KeyName, "maxscale");
				Sp->MinScale = (float)AttrFile.GetValueF(KeyName, "minscale");
				Sp->MaxSpeed = (float)AttrFile.GetValueF(KeyName, "maxspeed");
				Sp->MinSpeed = (float)AttrFile.GetValueF(KeyName, "minspeed");
				Sp->MaxUnitLife = (float)AttrFile.GetValueF(KeyName, "maxunitlife");
				Sp->MinUnitLife = (float)AttrFile.GetValueF(KeyName, "minunitlife");
				Sp->Rate = (float)AttrFile.GetValueF(KeyName, "particlecreationrate");
				Sp->SprayLife = (float)AttrFile.GetValueF(KeyName, "totallife");
				Vector = AttrFile.GetValue(KeyName, "bounce");
				Sp->Bounce = false;
				if(Vector=="true")
					Sp->Bounce = true;
				Effects[effptr].Data = Sp;
				Effects[effptr].Active = true;
				effptr +=1;
			}
		}
		else if(Type=="light")
		{
			CString Vector;
			char szName[64];
			geVec3d convert;
			Glow *Lite;
			strcpy(Effects[effptr].Name,KeyName);
			Effects[effptr].Type = EFF_LIGHT;
			Lite = GE_RAM_ALLOCATE_STRUCT(Glow);
			memset( Lite, 0, sizeof(Glow) );
			Lite->LightLife = 1.0f;
			Lite->Intensity = 1.0f;
			Vector = AttrFile.GetValue(KeyName, "colormax");
			if(Vector!="")
			{
				strcpy(szName,Vector);
				convert = Extract(szName);
				Lite->ColorMax.r = convert.X;
				Lite->ColorMax.g = convert.Y;
				Lite->ColorMax.b = convert.Z;
			}
			Vector = AttrFile.GetValue(KeyName, "colormin");
			if(Vector!="")
			{
				strcpy(szName,Vector);
				convert = Extract(szName);
				Lite->ColorMin.r = convert.X;
				Lite->ColorMin.g = convert.Y;
				Lite->ColorMin.b = convert.Z;
			}
			Lite->RadiusMax = (float)AttrFile.GetValueF(KeyName, "radiusmax");
			Lite->RadiusMin = (float)AttrFile.GetValueF(KeyName, "radiusmin");
			Lite->Intensity = (float)AttrFile.GetValueF(KeyName, "intensity");
			Lite->LightLife = (float)AttrFile.GetValueF(KeyName, "totallife");
			Lite->DoNotClip = false;
			Lite->CastShadows = true;
			Effects[effptr].Data = Lite;
			Effects[effptr].Active = true;
			effptr +=1;
		}
		else if(Type=="sprite")
		{
			CString Tname, Talpha, Vector;
			char szName[64], szAlpha[64];
			geVec3d convert;
			Sprite *Sp;
			Effects[effptr].Type = EFF_SPRITE;
			Tname = AttrFile.GetValue(KeyName, "basebitmapname");
			if(Tname!="")
			{
				Talpha = AttrFile.GetValue(KeyName, "basealphamapname");
				if(Talpha=="")
					Talpha = Tname;
				strcpy(szName,Tname);
				strcpy(szAlpha,Talpha);
				int Count = AttrFile.GetValueI(KeyName, "bitmapcount");
				if(Count>0)
				{
					Sp = GE_RAM_ALLOCATE_STRUCT(Sprite);
					memset( Sp, 0, sizeof(Sprite) );
					Sp->TotalTextures = Count;
					Sp->Texture = (geBitmap **)geRam_AllocateClear( sizeof( *( Sp->Texture ) ) * Count );
					for (int i = 0; i < Count; i++ )
					{
						char BmpName[256];
						char AlphaName[256];
						// build bmp and alpha names
						sprintf( BmpName, "%s%d%s", szName, i, ".bmp" );
						sprintf( AlphaName, "%s%d%s", szAlpha, i, ".bmp" );
						Sp->Texture[i] = TPool_Bitmap(BmpName, AlphaName, NULL, NULL);
					}
					Vector = AttrFile.GetValue(KeyName, "color");
					if(Vector!="")
					{
						strcpy(szName,Vector);
						convert = Extract(szName);
						Sp->Color.r = convert.X;
						Sp->Color.g = convert.Y;
						Sp->Color.b = convert.Z;
					}
					Sp->TextureRate = 1.0f/(float)AttrFile.GetValueF(KeyName, "texturerate");
					Sp->Scale = (float)AttrFile.GetValueF(KeyName, "scale");
					Sp->ScaleRate = (float)AttrFile.GetValueF(KeyName, "scalerate");
					Sp->Rotation = 0.0174532925199433f*(float)AttrFile.GetValueF(KeyName, "rotation");
					Sp->RotationRate = 0.0174532925199433f*(float)AttrFile.GetValueF(KeyName, "rotationrate");
					Sp->Color.a = (float)AttrFile.GetValueF(KeyName, "alpha");
					Sp->AlphaRate = (float)AttrFile.GetValueF(KeyName, "alpharate");
					Sp->Style = SPRITE_CYCLE_ONCE;
					Effects[effptr].Data = Sp;
					Effects[effptr].Active = true;
					effptr +=1;
				}
			}
		}
		else if(Type=="sound")
		{
			Snd *Sound;
			CString Vector;
			char szName[64];
			strcpy(Effects[effptr].Name,KeyName);
			Effects[effptr].Type = EFF_SND;
			Vector = AttrFile.GetValue(KeyName, "name");
			if(Vector!="")
			{
				Sound = GE_RAM_ALLOCATE_STRUCT(Snd);
				memset(Sound, 0, sizeof(Snd) );
				strcpy(szName,Vector);
				Sound->SoundDef = SPool_Sound(szName);
				Sound->Min = kAudibleRadius;
				Sound->Loop = false;
				Effects[effptr].Data = Sound;
				Effects[effptr].Active = true;
				effptr +=1;
			}
		}
		else if(Type=="corona")
		{
			EffCorona *C;
			CString Vector;
			geVec3d convert;
			char szName[64];
			strcpy(Effects[effptr].Name,KeyName);
			Effects[effptr].Type = EFF_CORONA;
			C = GE_RAM_ALLOCATE_STRUCT(EffCorona);
			memset(C, 0, sizeof(EffCorona) );
			C->Texture = TPool_Bitmap("Corona.Bmp", "Corona_a.Bmp", NULL, NULL);
			Vector = AttrFile.GetValue(KeyName, "color");
			if(Vector!="")
			{
				strcpy(szName,Vector);
				convert = Extract(szName);
				C->Vertex.r = convert.X;
				C->Vertex.g = convert.Y;
				C->Vertex.b = convert.Z;
			}
			C->FadeTime = (float)AttrFile.GetValueF(KeyName, "fadetime");
			C->MinRadius = (float)AttrFile.GetValueF(KeyName, "minradius");
			C->MaxRadius = (float)AttrFile.GetValueF(KeyName, "maxradius");
			C->MinRadiusDistance = (float)AttrFile.GetValueF(KeyName, "minradiusdistance");
			C->MaxRadiusDistance = (float)AttrFile.GetValueF(KeyName, "maxradiusdistance");
			C->MaxVisibleDistance = (float)AttrFile.GetValueF(KeyName, "maxvisibledistance");
			C->LifeTime = (float)AttrFile.GetValueF(KeyName, "totallife");
			Effects[effptr].Data = C;
			Effects[effptr].Active = true;
			effptr +=1;
		}
		else if(Type=="bolt")
		{
			CString Tname, Talpha, Vector;
			char szName[64], szAlpha[64];
			geVec3d convert;
			EBolt *Bl;
			Effects[effptr].Type = EFF_BOLT;
			Bl = GE_RAM_ALLOCATE_STRUCT(EBolt);
			memset( Bl, 0, sizeof(EBolt) );
			Tname = AttrFile.GetValue(KeyName, "bitmapname");
			if(Tname!="")
			{
				Talpha = AttrFile.GetValue(KeyName, "alphamapname");
				if(Talpha=="")
					Talpha = Tname;
				strcpy(szName,Tname);
				strcpy(szAlpha,Talpha);
				Bl->Bitmap = TPool_Bitmap(szName, szAlpha, NULL, NULL);
			}
			else
				Bl->Bitmap = TPool_Bitmap("bolt.bmp", "bolt.bmp", NULL, NULL);
			Vector = AttrFile.GetValue(KeyName, "color");
			if(Vector!="")
			{
				strcpy(szName,Vector);
				convert = Extract(szName);
				Bl->Color.r = convert.X;
				Bl->Color.g = convert.Y;
				Bl->Color.b = convert.Z;
				Bl->Color.a = 255.0f;
			}
			Vector = AttrFile.GetValue(KeyName, "endoffset");
			if(Vector!="")
			{
				strcpy(szName,Vector);
				convert = Extract(szName);
				Bl->End.X = convert.X;
				Bl->End.Y = convert.Y;
				Bl->End.Z = convert.Z;
			}
			Bl->Width = AttrFile.GetValueI(KeyName, "width");
			Bl->NumPoints = AttrFile.GetValueI(KeyName, "numberpoints");
			Bl->Wildness = (float)AttrFile.GetValueF(KeyName, "wildness");
			Vector = AttrFile.GetValue(KeyName, "intermittent");
			Bl->Intermittent = false;
			if(Vector=="true")
				Bl->Intermittent = true;
			Bl->MinFrequency = (float)AttrFile.GetValueF(KeyName, "minfrequency");
			Bl->MaxFrequency = (float)AttrFile.GetValueF(KeyName, "maxfrequency");
			Bl->DominantColor = AttrFile.GetValueI(KeyName, "dominantcolor");
			Bl->CompleteLife = (float)AttrFile.GetValueF(KeyName, "totallife");
			Effects[effptr].Data = Bl;
			Effects[effptr].Active = true;
			effptr +=1;
		}
		
		KeyName = AttrFile.FindNextKey();
	}
}

CPreEffect::~CPreEffect()
{
	int i;

	for(i=0;i<MAXEXPITEM;i++)
	{
		if(Effects[i].Data)
		{
			if(Effects[i].Type==EFF_SPRITE)
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

int CPreEffect::AddEffect(int k, geVec3d Position, geVec3d Offset)
{
	int index = -1;
	
	switch(Effects[k].Type)
	{
	case EFF_SPRAY:
		Spray Sp;
		geVec3d	In;
		geXForm3d	Xf;
		memcpy(&Sp, Effects[k].Data, sizeof( Sp ) );
		geVec3d_Copy( &(Position ), &( Sp.Source ) );
		geVec3d_Add( &( Sp.Source ), &Offset,&(Sp.Source) );
		geXForm3d_SetIdentity( &( Xf ) );
		geXForm3d_RotateX( &( Xf ), Sp.Dest.X / 57.3f );  
		geXForm3d_RotateY( &( Xf ), ( Sp.Dest.Y - 90.0f ) / 57.3f );  
		geXForm3d_RotateZ( &( Xf ), Sp.Dest.Z / 57.3f ); 
		geXForm3d_GetIn( &( Xf ), &In );
		geVec3d_Inverse( &In );
		geVec3d_Add( &( Sp.Source ), &In,&( Sp.Dest ) );
		index = CCD->EffectManager()->Item_Add(EFF_SPRAY, &Sp);
		break;
	case EFF_LIGHT:
		Glow Lite;
		memcpy(&Lite, Effects[k].Data, sizeof(Lite) );
		geVec3d_Copy( &(Position ), &( Lite.Pos) );
		geVec3d_Add( &(Lite.Pos), &Offset,&(Lite.Pos) );
		index = CCD->EffectManager()->Item_Add(EFF_LIGHT, &Lite);
		break;
	case EFF_SND:
		Snd Sound;
		memcpy(&Sound, Effects[k].Data, sizeof(Sound) );
		geVec3d_Copy( &(Position ), &( Sound.Pos) );
		geVec3d_Add( &(Sound.Pos), &Offset,&(Sound.Pos) );
		index = CCD->EffectManager()->Item_Add(EFF_SND, &Sound);
		break;
	case EFF_SPRITE:
		Sprite S;
		memcpy(&S, Effects[k].Data, sizeof(S) );
		geVec3d_Copy( &(Position ), &( S.Pos) );
		geVec3d_Add( &(S.Pos), &Offset,&(S.Pos) );
		index = CCD->EffectManager()->Item_Add(EFF_SPRITE, &S);
		break;
	case EFF_CORONA:
		EffCorona C;
		memcpy(&C, Effects[k].Data, sizeof(C) );
		C.Vertex.X = Position.X + Offset.X;
		C.Vertex.Y = Position.Y + Offset.Y;
		C.Vertex.Z = Position.Z + Offset.Z;
		index = CCD->EffectManager()->Item_Add(EFF_CORONA, &C);
		break;
	case EFF_BOLT:
		EBolt Bl;
		memcpy(&Bl, Effects[k].Data, sizeof(Bl) );
		geVec3d_Copy( &(Position ), &(Bl.Start) );
		geVec3d_Add( &(Bl.Start), &Offset,&(Bl.Start) );
		geVec3d_Add( &(Bl.Start), &(Bl.End),&(Bl.End) );
		index = CCD->EffectManager()->Item_Add(EFF_BOLT, &Bl);
		break;
	}
	return index;
}