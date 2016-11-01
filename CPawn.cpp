/************************************************************************************//**
 * @file CPawn.cpp
 * @brief CPawn class declaration
 * @author Ralph Deane
 *//*
 * Copyright (c) Ralph Deane; All rights reserved.
 ****************************************************************************************/

#include "RabidFramework.h"
#include <Ram.h>
#include "IniFile.h"
#include "CLanguageManager.h"
#include "CScriptManager.h"
#include "CConversation.h"
#include "CLevel.h"
#include "CScriptPoint.h"
#include "CPawn.h"
#include "CPawnManager.h"

extern geSound_Def *SPool_Sound(const char *SName);
extern geBitmap *TPool_Bitmap(const char *DefaultBmp, const char *DefaultAlpha,
							  const char *BName, const char *AName);

#include "Simkin\\skScriptedExecutable.h"
#include "Simkin\\skRValue.h"
#include "Simkin\\skRValueArray.h"
#include "Simkin\\skRuntimeException.h"
#include "Simkin\\skParseException.h"
#include "Simkin\\skBoundsException.h"
#include "Simkin\\skTreeNode.h"


/* ------------------------------------------------------------------------------------ */
// ScriptedObject class
/* ------------------------------------------------------------------------------------ */
ScriptedObject::ScriptedObject(const std::string& filename)
	: skScriptedExecutable(filename.c_str(), ScriptManager::GetContext())
{
	m_HighLevel		= GE_TRUE;
	m_RunOrder		= false;
	m_Actor			= NULL;
	m_WeaponActor	= NULL;
	m_Active		= false;
	m_ValidPoint	= false;
	m_ActionActive	= false;
	m_Time			= 0.0f;
	m_DistActive	= false;
	m_AvoidMode		= false;
	m_PainActive	= false;
	m_TriggerWait	= false;
	m_SoundID		= -1;
	m_AudibleRadius	= CCD->Level()->GetAudibleRadius();
	m_FacePlayer	= false;
	m_FaceAxis		= true;
	m_Conversation	= NULL;
	m_UseKey		= true;
	m_FOV			= -2.0f;
	m_HostilePlayer	= true;
	m_HostileDiff	= false;
	m_HostileSame	= false;
	m_TargetFind	= false;
	m_TargetActor	= NULL;
	m_TargetDisable	= false;
	m_Console		= false;
	m_ConsoleHeader	= NULL;
	m_ConsoleError	= NULL;
	m_StreamingAudio = NULL;

	memset(m_ConsoleDebug, 0, sizeof(char*)*DEBUGLINES);

	strcpy(m_Indicate, "+");

	m_EnvironmentMapping= false;
	m_Collision			= false;
	m_Pushable			= false;
	m_SoundLoop			= false;
	m_PointFind			= false;
	m_ShadowAlpha		= 0.0f;
	m_ProjectedShadows	= false;
	m_StencilShadows	= GE_FALSE;
	m_AmbientLightFromFloor = true;
	m_Prev_HL_Actor		= NULL;
	m_Prev_HL_FillColor.r = m_Prev_HL_FillColor.g = m_Prev_HL_FillColor.b = m_Prev_HL_FillColor.a = 255.0f;
	m_Prev_HL_AmbientColor.r = m_Prev_HL_AmbientColor.g = m_Prev_HL_AmbientColor.b = m_Prev_HL_AmbientColor.a = 255.0f;
	m_Prev_HL_AmbientLightFromFloor = GE_TRUE;
}

/* ------------------------------------------------------------------------------------ */
//	Destructor
/* ------------------------------------------------------------------------------------ */
ScriptedObject::~ScriptedObject()
{
	if(m_Actor)
	{
		ScriptManager::RemoveGlobalVariable(CCD->ActorManager()->GetEntityName(m_Actor));
		CCD->ActorManager()->RemoveActorCheck(m_Actor);
		geActor_Destroy(&m_Actor);
		m_Actor = NULL;
	}

	if(m_WeaponActor)
	{
		CCD->ActorManager()->RemoveActorCheck(m_WeaponActor);
		geActor_Destroy(&m_WeaponActor);
		m_WeaponActor = NULL;
	}

	DequeueAll();

	ClearWatchedTriggers();

	if(m_StreamingAudio)
	{
		m_StreamingAudio->Delete();
		delete m_StreamingAudio;
	}

	delete m_Conversation;

	delete[] m_ConsoleHeader;

	delete[] m_ConsoleError;

	for(int i=0; i<DEBUGLINES; ++i)
	{
		delete[] (m_ConsoleDebug[i]);
	}
}


/* ------------------------------------------------------------------------------------ */
//	Update
/* ------------------------------------------------------------------------------------ */
void ScriptedObject::Update(float timeElapsed)
{
	if(!m_Alive)
		return;

	if(!m_Active)
	{
		m_Active = true;
		Spawn();

		if(!m_Point.empty())
		{
			const char *EntityType = CCD->EntityRegistry()->GetEntityType(m_Point.c_str());

			if(EntityType)
			{
				if(!stricmp(EntityType, "ScriptPoint"))
				{
					ScriptPoint *pProxy;

					CCD->Level()->ScriptPoints()->LocateEntity(m_Point.c_str(), reinterpret_cast<void**>(&pProxy));
					m_CurrentPoint = pProxy->origin;
					m_ValidPoint = true;
					m_NextOrder.clear();

					if(!EffectC_IsStringNull(pProxy->NextOrder))
						m_NextOrder = pProxy->NextOrder;
				}
			}
		}
	}

	switch(m_HighLevel)
	{
	case GE_TRUE:
		UpdateHigh(timeElapsed);
		break;
	case GE_FALSE:
		UpdateLow(timeElapsed);
		break;
	}

	// update sound positions
	UpdateSounds();

	m_Collision = false;

	AnimateWeapon();
}


/* ------------------------------------------------------------------------------------ */
//	Spawn
/* ------------------------------------------------------------------------------------ */
void ScriptedObject::Spawn()
{
	m_Actor = CCD->ActorManager()->SpawnActor(m_ActorName, m_Location, m_Rotation,
											m_BoxAnim, m_BoxAnim, NULL);

	CCD->ActorManager()->SetActorDynamicLighting(	m_Actor,
													m_FillColor,
													m_AmbientColor,
													m_AmbientLightFromFloor);

	CCD->ActorManager()->SetShadow(m_Actor, m_ShadowSize);

	if(m_ShadowAlpha > 0.0f)
	{
		CCD->ActorManager()->SetShadowAlpha(m_Actor, m_ShadowAlpha);
	}

	if(!m_ShadowBitmap.empty())
	{
		CCD->ActorManager()->SetShadowBitmap(m_Actor, TPool_Bitmap(	m_ShadowBitmap.c_str(),
																	m_ShadowAlphamap.c_str(),
																	NULL, NULL));
	}

	CCD->ActorManager()->SetProjectedShadows(m_Actor, m_ProjectedShadows);

	CCD->ActorManager()->SetStencilShadows(m_Actor, m_StencilShadows);

	CCD->ActorManager()->SetHideRadar(m_Actor, m_HideFromRadar);
	CCD->ActorManager()->SetScale(m_Actor, m_Scale);
	CCD->ActorManager()->SetType(m_Actor, ENTITY_NPC);
	CCD->ActorManager()->SetAutoStepUp(m_Actor, true);
	CCD->ActorManager()->SetBoxChange(m_Actor, false);
	CCD->ActorManager()->SetColDetLevel(m_Actor, RGF_COLLISIONLEVEL_2);
	CCD->ActorManager()->SetAnimationSpeed(m_Actor, m_AnimSpeed);
	CCD->ActorManager()->SetTilt(m_Actor, true);

	if(!m_RootBone.empty())
	{
		CCD->ActorManager()->SetRoot(m_Actor, m_RootBone);
	}

	if(m_EnvironmentMapping)
	{
		SetEnvironmentMapping(	m_Actor,
								true,
								m_AllMaterial,
								m_PercentMapping,
								m_PercentMaterial);
	}

	if(!m_ChangeMaterial.empty())
	{
		CCD->ActorManager()->ChangeMaterial(m_Actor, m_ChangeMaterial);
	}

	if(m_BoxAnim == "nocollide")
	{
		CCD->ActorManager()->SetNoCollide(m_Actor);
		CCD->ActorManager()->SetActorFlags(m_Actor, 0);
	}
	else if(!m_BoxAnim.empty())
	{
		CCD->ActorManager()->SetBoundingBox(m_Actor, m_BoxAnim);
	}

	CCD->ActorManager()->SetGravity(m_Actor, m_Gravity);
	CCD->ActorManager()->SetAlpha(m_Actor, m_ActorAlpha);

	geVec3d rotation;
	rotation.X = 0.0f;
	rotation.Y = m_YRotation;
	rotation.Z = 0.0f;

	CCD->ActorManager()->Rotate(m_Actor, rotation);
	CCD->ActorManager()->SetEntityName(m_Actor, szName);
}


/* ------------------------------------------------------------------------------------ */
//	AnimateWeapon
/* ------------------------------------------------------------------------------------ */
void ScriptedObject::AnimateWeapon()
{
	if(!m_Alive)
		return;

	if(!m_Active)
		return;

	if(m_WeaponActor && m_Actor)
	{
		geXForm3d XForm;
		geVec3d theRotation;
		geVec3d thePosition;
		geMotion *ActorMotion;
		geMotion *ActorBMotion;

		CCD->ActorManager()->GetRotate(m_Actor, &theRotation);
		CCD->ActorManager()->GetPosition(m_Actor, &thePosition);

		geXForm3d_SetZRotation(&XForm, m_WRotation.Z + theRotation.Z);
		geXForm3d_RotateX(&XForm, m_WRotation.X + theRotation.X);
		geXForm3d_RotateY(&XForm, m_WRotation.Y + theRotation.Y);
		geXForm3d_Translate(&XForm, thePosition.X, thePosition.Y, thePosition.Z);

		ActorMotion = CCD->ActorManager()->GetpMotion(m_Actor);
		ActorBMotion = CCD->ActorManager()->GetpBMotion(m_Actor);

		if(ActorMotion)
		{
			if(CCD->ActorManager()->GetTransitionFlag(m_Actor))
			{
				geActor_SetPose(m_WeaponActor, ActorMotion, 0.0f, &XForm);

				if(ActorBMotion)
				{
					float BM = (CCD->ActorManager()->GetBlendAmount(m_Actor) -
								CCD->ActorManager()->GetAnimationTime(m_Actor)) /
								CCD->ActorManager()->GetBlendAmount(m_Actor);

					if(BM < 0.0f)
						BM = 0.0f;

					geActor_BlendPose(m_WeaponActor, ActorBMotion,
							CCD->ActorManager()->GetStartTime(m_Actor), &XForm, BM);
				}
			}
			else
			{
				geActor_SetPose(m_WeaponActor, ActorMotion, CCD->ActorManager()->GetAnimationTime(m_Actor), &XForm);

				if(ActorBMotion)
				{
					geActor_BlendPose(m_WeaponActor, ActorBMotion,
						CCD->ActorManager()->GetAnimationTime(m_Actor), &XForm,
						CCD->ActorManager()->GetBlendAmount(m_Actor));
				}
			}
		}
		else
		{
			geActor_ClearPose(m_WeaponActor, &XForm);
		}

		float alpha;
		CCD->ActorManager()->GetAlpha(m_Actor, &alpha);
		geActor_SetAlpha(m_WeaponActor, alpha);
	}
}


geVec3d ScriptedObject::GetPosition()
{
	if(m_Actor)
	{
		geVec3d position;
		CCD->ActorManager()->GetPosition(m_Actor, &position);
		return position;
	}
	else
	{
		return m_DeadPos;
	}
}


geVec3d ScriptedObject::GetSpeakBonePosition()
{
	if(m_SpeakBone.empty())
	{
		return GetPosition();
	}
	else
	{
		geVec3d position;
		if(CCD->ActorManager()->GetBonePosition(m_Actor, m_SpeakBone, &position) != RGF_SUCCESS)
		{
			CCD->Log()->Debug("Failed to find bone '%s' in actor '%s'", m_SpeakBone, m_ActorName);
			return GetPosition();
		}
		return position;
	}
}


int ScriptedObject::PlaySound(const char* sound, int soundHandle /*= -1*/, geBoolean loop /*= GE_TRUE*/, float radius /*= -1.0f*/)
{
	if(!sound)
		return soundHandle;

	StopSound(soundHandle);

	Snd Sound;
	memset(&Sound, 0, sizeof(Sound));

	Sound.Pos = GetPosition();

	if(radius > 0.0f)
		Sound.Min = radius;
	else
		Sound.Min = m_AudibleRadius;

	Sound.Loop = loop;
	Sound.SoundDef = SPool_Sound(sound);

	int newSoundHandle = -1;
	if(Sound.SoundDef != NULL)
	{
		newSoundHandle = CCD->EffectManager()->Item_Add(EFF_SND, static_cast<void*>(&Sound));

		if(newSoundHandle != -1)
			m_SoundHandles.insert(newSoundHandle);
	}

	return newSoundHandle;
}


void ScriptedObject::UpdateSounds()
{
	geVec3d position = GetPosition();
	for(std::set<int>::iterator iter = m_SoundHandles.begin(); iter != m_SoundHandles.end();)
	{
		// update sound position
		if(CCD->EffectManager()->Item_Alive(*iter))
		{
			Snd sound;
			memset(&sound, 0, sizeof(sound));

			sound.Pos = position;

			CCD->EffectManager()->Item_Modify(EFF_SND, *iter, static_cast<void*>(&sound), SND_POS);
			iter++;
		}
		else
		{
			m_SoundHandles.erase(iter++); // increment to next iterator then erase current iterator
		}
	}

	if(m_StreamingAudio)
	{
		if(m_StreamingAudio->IsPlaying())
		{
			position = GetSpeakBonePosition();
			m_StreamingAudio->Modify3D(&position, m_AudibleRadius);
		}
		else
		{
			m_StreamingAudio->Delete();
			SAFE_DELETE(m_StreamingAudio);
		}
	}
}


void ScriptedObject::StopSound(int soundHandle)
{
	if(soundHandle != -1)
	{
		CCD->EffectManager()->Item_Delete(EFF_SND, soundHandle);
		m_SoundHandles.erase(soundHandle);
		if(soundHandle == m_SoundID) m_SoundID = -1;
	}
}

/* ------------------------------------------------------------------------------------ */
//	calls a method in this object
/* ------------------------------------------------------------------------------------ */
bool ScriptedObject::method(const skString& methodName, skRValueArray& arguments,
							skRValue& returnValue, skExecutableContext& ctxt)
{
	switch(m_HighLevel)
	{
	case GE_FALSE:
		return lowmethod(methodName, arguments, returnValue, ctxt);
	default:
		return highmethod(methodName, arguments, returnValue, ctxt);
	}
}

/* ----------------------------------- END OF FILE ------------------------------------ */
