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
#include "CLevel.h"
#include "IniFile.h"
#include <Ram.h>

extern geSound_Def *SPool_Sound(const char *SName);
extern geBitmap *TPool_Bitmap(const char *DefaultBmp, const char *DefaultAlpha,
							  const char *BName, const char *AName);

class EffectDefinition
{
public:
	EffectDefinition(int type, void* data): m_Type(type), m_Data(data) {}
	~EffectDefinition()
	{
		if(m_Data != NULL)
		{
			if(m_Type == EFF_SPRITE)
			{
				Sprite *sprite = static_cast<Sprite*>(m_Data);
				if(sprite->Texture)	geRam_Free(sprite->Texture);
			}
			geRam_Free(m_Data);
		}
	}

	int		m_Type;
	void*	m_Data;
};


/* ------------------------------------------------------------------------------------ */
// Constructor
/* ------------------------------------------------------------------------------------ */
CPreEffect::CPreEffect()
{
	CIniFile iniFile("effect.ini");

	if(!iniFile.ReadFile())
	{
		CCD->Log()->Warning("Failed to open effect.ini file.");
		return;
	}

	std::string effectName = iniFile.FindFirstKey();
	std::string effectType;

	while(!effectName.empty())
	{
		if(EffectExists(effectName))
		{
			CCD->Log()->Warning("Redefinition of effect [" + effectName + "] in effect.ini file.");
			effectName = iniFile.FindNextKey();
			continue;
		}

		effectType = iniFile.GetValue(effectName, "type");

		if(effectType == "spray")
		{
			std::string textureName = iniFile.GetValue(effectName, "bitmapname");

			if(!textureName.empty())
			{
				std::string textureAlphaName(iniFile.GetValue(effectName, "alphamapname"));

				if(textureAlphaName.empty())
					textureAlphaName = textureName;

				Spray *spray = GE_RAM_ALLOCATE_STRUCT(Spray);
				memset(spray, 0, sizeof(Spray));
				spray->Texture = TPool_Bitmap(textureName.c_str(), textureAlphaName.c_str(), NULL, NULL);

				std::string value(iniFile.GetValue(effectName, "angles"));

				if(!value.empty())
				{
					spray->Angle = ToVec3d(value);
				}

				value = iniFile.GetValue(effectName, "colormax");

				if(!value.empty())
				{
					spray->ColorMax = ToRGBA(value);
				}

				value = iniFile.GetValue(effectName, "colormaxalpha");

				if(!value.empty())
				{
					spray->ColorMax.a = static_cast<float>(iniFile.GetValueI(effectName, "colormaxalpha"));
				}

				value = iniFile.GetValue(effectName, "colormin");

				if(!value.empty())
				{
					spray->ColorMin = ToRGBA(value);
				}

				value = iniFile.GetValue(effectName, "colorminalpha");

				if(!value.empty())
				{
					spray->ColorMin.a = static_cast<float>(iniFile.GetValueI(effectName, "colorminalpha"));
				}

				spray->SourceVariance	= iniFile.GetValueI(effectName, "sourcevariance");
				spray->DestVariance		= iniFile.GetValueI(effectName, "destvariance");

				value = iniFile.GetValue(effectName, "gravity");

				if(!value.empty())
				{
					spray->Gravity = ToVec3d(value);
				}

				spray->MaxScale		= static_cast<float>(iniFile.GetValueF(effectName, "maxscale"));
				spray->MinScale		= static_cast<float>(iniFile.GetValueF(effectName, "minscale"));
				spray->MaxSpeed		= static_cast<float>(iniFile.GetValueF(effectName, "maxspeed"));
				spray->MinSpeed		= static_cast<float>(iniFile.GetValueF(effectName, "minspeed"));
				spray->MaxUnitLife	= static_cast<float>(iniFile.GetValueF(effectName, "maxunitlife"));
				spray->MinUnitLife	= static_cast<float>(iniFile.GetValueF(effectName, "minunitlife"));
				spray->Rate			= static_cast<float>(iniFile.GetValueF(effectName, "particlecreationrate"));
				spray->SprayLife	= static_cast<float>(iniFile.GetValueF(effectName, "totallife"));
				spray->Bounce		= GE_FALSE;

				value = iniFile.GetValue(effectName, "bounce");

				if(value == "true")
				{
					spray->Bounce = GE_TRUE;
				}

				m_Effects[effectName] = new EffectDefinition(EFF_SPRAY, spray);
			}
		}
		else if(effectType == "light")
		{
			Glow *light = GE_RAM_ALLOCATE_STRUCT(Glow);
			memset(light, 0, sizeof(Glow));
			light->LightLife = 1.0f;
			light->Intensity = 1.0f;

			std::string value(iniFile.GetValue(effectName, "colormax"));

			if(!value.empty())
			{
				light->ColorMax = ToRGBA(value);
			}

			value = iniFile.GetValue(effectName, "colormin");

			if(!value.empty())
			{
				light->ColorMin = ToRGBA(value);
			}

			value = iniFile.GetValue(effectName, "offsetangles");

			if(!value.empty())
			{
				light->Direction = ToVec3d(value);
			}

			light->Spot = GE_FALSE;

			value = iniFile.GetValue(effectName, "spotlight");

			if(value == "true")
				light->Spot = GE_TRUE;

			light->Arc			= static_cast<float>(iniFile.GetValueF(effectName, "arc"));
			light->Style		= iniFile.GetValueI(effectName, "style");

			light->RadiusMax	= static_cast<float>(iniFile.GetValueF(effectName, "radiusmax"));
			light->RadiusMin	= static_cast<float>(iniFile.GetValueF(effectName, "radiusmin"));
			light->Intensity	= static_cast<float>(iniFile.GetValueF(effectName, "intensity"));
			light->LightLife	= static_cast<float>(iniFile.GetValueF(effectName, "totallife"));
			light->DoNotClip	= GE_FALSE;
			light->CastShadows	= GE_TRUE;

			m_Effects[effectName] = new EffectDefinition(EFF_LIGHT, light);
		}
		else if(effectType == "sprite")
		{
			std::string textureName(iniFile.GetValue(effectName, "basebitmapname"));

			if(!textureName.empty())
			{
				std::string textureAlphaName(iniFile.GetValue(effectName, "basealphamapname"));

				if(textureAlphaName.empty())
					textureAlphaName = textureName;

				int textureCount = iniFile.GetValueI(effectName, "bitmapcount");

				if(textureCount > 0)
				{
					Sprite *sprite = GE_RAM_ALLOCATE_STRUCT(Sprite);
					memset(sprite, 0, sizeof(Sprite));
					sprite->TotalTextures = textureCount;
					sprite->Texture = static_cast<geBitmap**>(geRam_AllocateClear(sizeof(geBitmap*) * textureCount));

					for(int i=0; i<textureCount; ++i)
					{
						char bmpName[256];
						char alphaName[256];
						// build bmp and alpha names
						sprintf(bmpName, "%s%d%s", textureName.c_str(), i, ".bmp");
						sprintf(alphaName, "%s%d%s", textureAlphaName.c_str(), i, ".bmp");
						sprite->Texture[i] = TPool_Bitmap(bmpName, alphaName, NULL, NULL);
					}

					std::string value(iniFile.GetValue(effectName, "color"));

					if(!value.empty())
					{
						sprite->Color = ToRGBA(value);
					}

					sprite->TextureRate		= 1.0f/static_cast<float>(iniFile.GetValueF(effectName, "texturerate"));
					sprite->Scale			= static_cast<float>(iniFile.GetValueF(effectName, "scale"));
					sprite->ScaleRate		= static_cast<float>(iniFile.GetValueF(effectName, "scalerate"));
					sprite->Rotation		= GE_PIOVER180*static_cast<float>(iniFile.GetValueF(effectName, "rotation"));
					sprite->RotationRate	= GE_PIOVER180*static_cast<float>(iniFile.GetValueF(effectName, "rotationrate"));
					sprite->Color.a			= static_cast<float>(iniFile.GetValueF(effectName, "alpha"));
					sprite->AlphaRate		= static_cast<float>(iniFile.GetValueF(effectName, "alpharate"));
					sprite->LifeTime		= static_cast<float>(iniFile.GetValueF(effectName, "lifetime"));

					value = iniFile.GetValue(effectName, "style");

					if(value.empty())
					{
						sprite->Style = SPRITE_CYCLE_ONCE;
					}
					else
					{
						if(value == "none")
							sprite->Style = SPRITE_CYCLE_NONE;
						else if(value == "reset")
							sprite->Style = SPRITE_CYCLE_RESET;
						else if(value == "reverse")
							sprite->Style = SPRITE_CYCLE_REVERSE;
						else if(value == "random")
							sprite->Style = SPRITE_CYCLE_RANDOM;
						else
							sprite->Style = SPRITE_CYCLE_ONCE;
					}

					m_Effects[effectName] = new EffectDefinition(EFF_SPRITE, sprite);
				}
			}
		}
		else if(effectType == "sound")
		{
			std::string name(iniFile.GetValue(effectName, "name"));

			if(!name.empty())
			{
				Snd *sound = GE_RAM_ALLOCATE_STRUCT(Snd);
				memset(sound, 0, sizeof(Snd));
				sound->SoundDef = SPool_Sound(name.c_str());
				sound->Min = CCD->Level()->GetAudibleRadius();
				sound->Loop = GE_FALSE;

				m_Effects[effectName] = new EffectDefinition(EFF_SND, sound);
			}
		}
		else if(effectType == "corona")
		{
			EffCorona *corona = GE_RAM_ALLOCATE_STRUCT(EffCorona);
			memset(corona, 0, sizeof(EffCorona));
			corona->Texture = TPool_Bitmap("corona.bmp", "a_corona.bmp", NULL, NULL);

			std::string value(iniFile.GetValue(effectName, "color"));

			if(!value.empty())
			{
				geVec3d convert = ToVec3d(value);
				corona->Vertex.r = convert.X;
				corona->Vertex.g = convert.Y;
				corona->Vertex.b = convert.Z;
			}

			corona->FadeTime			= static_cast<float>(iniFile.GetValueF(effectName, "fadetime"));
			corona->MinRadius			= static_cast<float>(iniFile.GetValueF(effectName, "minradius"));
			corona->MaxRadius			= static_cast<float>(iniFile.GetValueF(effectName, "maxradius"));
			corona->MinRadiusDistance	= static_cast<float>(iniFile.GetValueF(effectName, "minradiusdistance"));
			corona->MaxRadiusDistance	= static_cast<float>(iniFile.GetValueF(effectName, "maxradiusdistance"));
			corona->MaxVisibleDistance	= static_cast<float>(iniFile.GetValueF(effectName, "maxvisibledistance"));
			corona->LifeTime			= static_cast<float>(iniFile.GetValueF(effectName, "totallife"));

			m_Effects[effectName] = new EffectDefinition(EFF_CORONA, corona);
		}
		else if(effectType == "bolt")
		{
			EBolt *bolt = GE_RAM_ALLOCATE_STRUCT(EBolt);
			memset(bolt, 0, sizeof(EBolt));

			std::string textureName(iniFile.GetValue(effectName, "bitmapname"));

			if(!textureName.empty())
			{
				std::string textureAlphaName(iniFile.GetValue(effectName, "alphamapname"));

				if(textureAlphaName.empty())
					textureAlphaName = textureName;

				bolt->Bitmap = TPool_Bitmap(textureName.c_str(), textureAlphaName.c_str(), NULL, NULL);
			}
			else
			{
				bolt->Bitmap = TPool_Bitmap("bolt.bmp", "bolt.bmp", NULL, NULL);
			}

			std::string value(iniFile.GetValue(effectName, "color"));

			if(!value.empty())
			{
				bolt->Color = ToRGBA(value);
			}

			value = iniFile.GetValue(effectName, "endoffset");

			if(!value.empty())
			{
				bolt->EndOffset = ToVec3d(value);
			}

			bolt->Width			= iniFile.GetValueI(effectName, "width");
			bolt->NumPoints		= iniFile.GetValueI(effectName, "numberpoints");
			bolt->Wildness		= static_cast<float>(iniFile.GetValueF(effectName, "wildness"));
			bolt->Intermittent	= GE_FALSE;

			value = iniFile.GetValue(effectName, "intermittent");

			if(value == "true")
			{
				bolt->Intermittent = GE_TRUE;
			}

			bolt->MinFrequency	= static_cast<float>(iniFile.GetValueF(effectName, "minfrequency"));
			bolt->MaxFrequency	= static_cast<float>(iniFile.GetValueF(effectName, "maxfrequency"));
			bolt->DominantColor	= iniFile.GetValueI(effectName, "dominantcolor");
			bolt->CompleteLife	= static_cast<float>(iniFile.GetValueF(effectName, "totallife"));

			m_Effects[effectName] = new EffectDefinition(EFF_BOLT, bolt);
		}
		else if(effectType == "actorspray")
		{
			std::string actorName(iniFile.GetValue(effectName, "basename"));

			if(!actorName.empty())
			{
				ActorSpray *aSpray = GE_RAM_ALLOCATE_STRUCT(ActorSpray);
				memset(aSpray, 0, sizeof(ActorSpray));
				strcpy(aSpray->BaseName, actorName.c_str());
				aSpray->NumActors	= iniFile.GetValueI(effectName, "numberactors");
				aSpray->Style		= iniFile.GetValueI(effectName, "style");
				aSpray->Alpha		= static_cast<float>(iniFile.GetValueF(effectName, "alpha"));
				aSpray->AlphaRate	= static_cast<float>(iniFile.GetValueF(effectName, "alpharate"));

				std::string value(iniFile.GetValue(effectName, "baserotation"));

				if(!value.empty())
				{
					aSpray->BaseRotation = ToVec3d(value);
					geVec3d_Scale(&aSpray->BaseRotation, GE_PIOVER180, &aSpray->BaseRotation);
				}

				char name[128];
				geVec3d zero;
				geVec3d_Clear(&zero);

				for(int i=0; i<aSpray->NumActors; ++i)
				{
					sprintf(name, "%s%d%s", aSpray->BaseName, i, ".act");
					geActor *actor = CCD->ActorManager()->SpawnActor(	name,
																		zero,
																		aSpray->BaseRotation,
																		"", "", NULL);
					if(!actor)
					{
						CCD->Log()->Critical("File %s - Line %d: %s : Missing Actor '%s'",
												__FILE__, __LINE__, effectName.c_str(), name);

						delete CCD;
						CCD = NULL;
						exit(-333);
					}

					CCD->ActorManager()->RemoveActor(actor);
					geActor_Destroy(&actor);
					actor = NULL;
				}

				value = iniFile.GetValue(effectName, "minrotationspeed");

				if(!value.empty())
				{
					aSpray->MinRotationSpeed = ToVec3d(value);
					geVec3d_Scale(&aSpray->MinRotationSpeed, GE_PIOVER180, &aSpray->MinRotationSpeed);
				}

				value = iniFile.GetValue(effectName, "maxrotationspeed");

				if(!value.empty())
				{
					aSpray->MaxRotationSpeed = ToVec3d(value);
					geVec3d_Scale(&aSpray->MaxRotationSpeed, GE_PIOVER180, &aSpray->MaxRotationSpeed);
				}

				aSpray->FillColor.a = 255.0f;
				aSpray->AmbientColor.a = 255.0f;

				value = iniFile.GetValue(effectName, "angles");

				if(!value.empty())
				{
					aSpray->Angle = ToVec3d(value);
				}

				value = iniFile.GetValue(effectName, "fillcolor");

				if(!value.empty())
				{
					aSpray->FillColor = ToRGBA(value);
				}

				value = iniFile.GetValue(effectName, "ambientcolor");

				if(!value.empty())
				{
					aSpray->AmbientColor = ToRGBA(value);
				}

				aSpray->AmbientLightFromFloor = GE_TRUE;

				value = iniFile.GetValue(effectName, "ambientlightfromfloor");

				if(value == "false")
					aSpray->AmbientLightFromFloor = GE_FALSE;

				aSpray->EnvironmentMapping = GE_FALSE;

				value = iniFile.GetValue(effectName, "environmentmapping");

				if(value == "true")
				{
					aSpray->EnvironmentMapping = GE_TRUE;
					aSpray->AllMaterial	= GE_FALSE;

					value = iniFile.GetValue(effectName, "allmaterial");

					if(value == "true")
						aSpray->AllMaterial = GE_TRUE;

					aSpray->PercentMapping	= static_cast<float>(iniFile.GetValueF(effectName, "percentmapping"));
					aSpray->PercentMaterial = static_cast<float>(iniFile.GetValueF(effectName, "percentmaterial"));
				}

				aSpray->SourceVariance	= iniFile.GetValueI(effectName, "sourcevariance");
				aSpray->DestVariance	= iniFile.GetValueI(effectName, "destvariance");
				aSpray->Gravity			= true;

				value = iniFile.GetValue(effectName, "gravity");

				if(value == "false")
					aSpray->Gravity = false;

				aSpray->MaxScale	= static_cast<float>(iniFile.GetValueF(effectName, "maxscale"));
				aSpray->MinScale	= static_cast<float>(iniFile.GetValueF(effectName, "minscale"));
				aSpray->MaxSpeed	= static_cast<float>(iniFile.GetValueF(effectName, "maxspeed"));
				aSpray->MinSpeed	= static_cast<float>(iniFile.GetValueF(effectName, "minspeed"));
				aSpray->MaxUnitLife = static_cast<float>(iniFile.GetValueF(effectName, "maxunitlife"));
				aSpray->MinUnitLife = static_cast<float>(iniFile.GetValueF(effectName, "minunitlife"));
				aSpray->Rate		= static_cast<float>(iniFile.GetValueF(effectName, "particlecreationrate"));
				aSpray->SprayLife	= static_cast<float>(iniFile.GetValueF(effectName, "totallife"));
				aSpray->Bounce		= GE_FALSE;

				value = iniFile.GetValue(effectName, "bounce");

				if(value == "true")
					aSpray->Bounce = GE_TRUE;

				aSpray->Solid = GE_FALSE;

				value = iniFile.GetValue(effectName, "solid");

				if(value == "true")
					aSpray->Solid = GE_TRUE;

				m_Effects[effectName] = new EffectDefinition(EFF_ACTORSPRAY, aSpray);
			}
		}

		effectName = iniFile.FindNextKey();
	}
}


/* ------------------------------------------------------------------------------------ */
// Destructor
/* ------------------------------------------------------------------------------------ */
CPreEffect::~CPreEffect()
{
	stdext::hash_map<std::string, EffectDefinition*>::iterator iter = m_Effects.begin();

	for(; iter!=m_Effects.end(); ++iter)
	{
		delete (iter->second);
	}

	m_Effects.clear();
}


/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
bool CPreEffect::EffectExists(const std::string& effectName)
{
	return (m_Effects.find(effectName) != m_Effects.end());
}


/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
int CPreEffect::EffectType(const std::string& effectName)
{
	return m_Effects[effectName]->m_Type;
}


/* ------------------------------------------------------------------------------------ */
// AddEffect
/* ------------------------------------------------------------------------------------ */
int CPreEffect::AddEffect(const std::string& effectName, const geVec3d& position, const geVec3d& offset)
{
	int index = -1;

	switch(m_Effects[effectName]->m_Type)
	{
	case EFF_SPRAY:
		{
			Spray spray;
			geVec3d in;
			geXForm3d xForm;

			memcpy(&spray, m_Effects[effectName]->m_Data, sizeof(spray));
			geVec3d_Copy(&position, &spray.Source);
			geVec3d_Add(&spray.Source, &offset, &spray.Source);

			geXForm3d_SetXRotation(&xForm,	(spray.Angle.X * GE_PIOVER180));
			geXForm3d_RotateY(&xForm,		(spray.Angle.Y - 90.0f) * GE_PIOVER180);
			geXForm3d_RotateZ(&xForm,		(spray.Angle.Z * GE_PIOVER180));

			geXForm3d_GetIn(&xForm, &in);
			geVec3d_Inverse(&in);
			geVec3d_Add(&spray.Source, &in, &spray.Dest);
			index = CCD->EffectManager()->Item_Add(EFF_SPRAY, &spray);
			break;
		}
	case EFF_LIGHT:
		{
			Glow light;

			memcpy(&light, m_Effects[effectName]->m_Data, sizeof(light));
			geVec3d_Copy(&position, &light.Pos);
			geVec3d_Add(&light.Pos, &offset, &light.Pos);

			index = CCD->EffectManager()->Item_Add(EFF_LIGHT, &light);
			break;
		}
	case EFF_SND:
		{
			Snd sound;

			memcpy(&sound, m_Effects[effectName]->m_Data, sizeof(sound));
			geVec3d_Copy(&position, &sound.Pos);
			geVec3d_Add(&sound.Pos, &offset, &sound.Pos);

			index = CCD->EffectManager()->Item_Add(EFF_SND, &sound);
			break;
		}
	case EFF_SPRITE:
		{
			Sprite sprite;

			memcpy(&sprite, m_Effects[effectName]->m_Data, sizeof(sprite));
			geVec3d_Copy(&position, &sprite.Pos);
			geVec3d_Add(&sprite.Pos, &offset, &sprite.Pos);

			index = CCD->EffectManager()->Item_Add(EFF_SPRITE, &sprite);
			break;
		}
	case EFF_CORONA:
		{
			EffCorona corona;

			memcpy(&corona, m_Effects[effectName]->m_Data, sizeof(corona));
			corona.Vertex.X = position.X + offset.X;
			corona.Vertex.Y = position.Y + offset.Y;
			corona.Vertex.Z = position.Z + offset.Z;

			index = CCD->EffectManager()->Item_Add(EFF_CORONA, &corona);
			break;
		}
	case EFF_BOLT:
		{
			EBolt bolt;

			memcpy(&bolt, m_Effects[effectName]->m_Data, sizeof(bolt));
			geVec3d_Copy(&position, &bolt.Start);
			geVec3d_Add(&bolt.Start, &offset, &bolt.Start);
			geVec3d_Add(&bolt.Start, &bolt.EndOffset, &bolt.End);

			index = CCD->EffectManager()->Item_Add(EFF_BOLT, &bolt);
			break;
		}
	case EFF_ACTORSPRAY:
		{
			ActorSpray aSpray;
			geVec3d in;
			geXForm3d xForm;

			memcpy(&aSpray, m_Effects[effectName]->m_Data, sizeof(aSpray));
			geVec3d_Copy(&position, &aSpray.Source);
			geVec3d_Add(&aSpray.Source, &offset, &aSpray.Source);

			geXForm3d_SetZRotation(&xForm,	 (aSpray.Angle.Z * GE_PIOVER180));
			geXForm3d_RotateX(&xForm,		-(aSpray.Angle.X * GE_PIOVER180));
			geXForm3d_RotateY(&xForm,		 (aSpray.Angle.Y - 90.0f) * GE_PIOVER180);

			geXForm3d_GetIn(&xForm, &in);
			geVec3d_Inverse(&in);
			geVec3d_Add(&aSpray.Source, &in, &aSpray.Dest);

			index = CCD->EffectManager()->Item_Add(EFF_ACTORSPRAY, &aSpray);
			break;
		}
	}

	return index;
}


/* ----------------------------------- END OF FILE ------------------------------------ */
