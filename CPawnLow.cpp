/************************************************************************************//**
 * @file CPawnLow.cpp
 * @brief Pawn Low Manager
 *
 * This file contains the class implementation of the Pawn Low Manager system
 * for Reality Factory.
 * @author Ralph Deane
 *//*
 * Copyright (c) Ralph Deane; All rights reserved.
 ****************************************************************************************/

#include "RabidFramework.h"
#include "RGFScriptMethods.h"
#include "CScriptManager.h"
#include "CGUIManager.h"
#include "CHeadsUpDisplay.h"
#include "CPolyShadow.h"
#include "CPlayState.h"
#include "CLevel.h"
#include "CInventory.h"

#include "CDamage.h"
#include "CFixedCamera.h"
#include "CFlipBook.h"
#include "CMovingPlatforms.h"
#include "CPathFollower.h"
#include "CScriptPoint.h"
#include "CTriggers.h"
#include "CWindGenerator.h"

#include "Simkin\\skScriptedExecutable.h"
#include "Simkin\\skRValue.h"
#include "Simkin\\skRValueArray.h"
#include "Simkin\\skRuntimeException.h"
#include "Simkin\\skParseException.h"

#include "CPawn.h"
#include "CPawnManager.h"

extern geSound_Def *SPool_Sound(const char *SName);

/* ------------------------------------------------------------------------------------ */
//	get a variable's value
/* ------------------------------------------------------------------------------------ */
bool ScriptedObject::getValue(const skString &fieldName,
							  const skString &attribute,
							  skRValue &value)
{
	long variable = ScriptManager::GetHashMethod(fieldName);

	switch(variable)
	{
	case RGF_SM_EXECUTIONLEVEL:
		{
			value = m_HighLevel;
			return true;
		}
	case RGF_SM_TIME:
		{
			value = m_lowTime;
			return true;
		}
	case RGF_SM_THINKTIME:
		{
			value = m_ThinkTime;
			return true;
		}
	case RGF_SM_DIFFICULTYLEVEL:
		{
			value = CCD->GetDifficultLevel();
			return true;
		}
	case RGF_SM_ICON:
		{
			value = skString(m_Icon.c_str());
			return true;
		}
	case RGF_SM_ENTITYNAME:
		{
			value = skString(szName.c_str());
			return true;
		}
	case RGF_SM_HEALTH:
		{
			CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(m_Actor);

			if(!m_Attribute.empty())
				value = theInv->Value(m_Attribute);
			else
				value = -1;

			return true;
		}
	case RGF_SM_ATTACK_FINISHED:
		{
			value = m_attack_finished;
			return true;
		}
	case RGF_SM_ATTACK_STATE:
		{
			value = m_attack_state;
			return true;
		}
	case RGF_SM_ENEMY_VIS:
		{
			if(!m_TargetActor)
			{
				value = false;
				return true;
			}

			bool flag = CanSee(m_FOV, m_Actor, m_TargetActor, m_FOVBone);

			if(flag)
			{
				CCD->ActorManager()->GetPosition(m_TargetActor, &m_LastTargetPoint);
				geExtBox theBox;
				CCD->ActorManager()->GetBoundingBox(m_TargetActor, &theBox);
				m_LastTargetPoint.Y += ((theBox.Max.Y)*0.5f);
			}

			value = flag;
			return true;
		}
	case RGF_SM_ENEMY_INFRONT:
		{
			GetAngles(true);
			value = false;

			if(m_actoryaw == m_targetyaw)
				value = true;

			return true;
		}
	case RGF_SM_ENEMY_RANGE:
		{
			geExtBox theBox;
			CCD->ActorManager()->GetBoundingBox(m_TargetActor, &theBox);
			value = CCD->ActorManager()->DistanceFrom(m_LastTargetPoint, m_Actor) - theBox.Max.X;
			return true;
		}
	case RGF_SM_PLAYER_RANGE:
		{
			geExtBox theBox;
			CCD->ActorManager()->GetBoundingBox(CCD->Player()->GetActor(), &theBox);
			value = CCD->ActorManager()->DistanceFrom(CCD->Player()->Position(), m_Actor) - theBox.Max.X;
			return true;
		}
	case RGF_SM_ENEMY_YAW:
		{
			GetAngles(true);
			value = m_targetyaw;
			return true;
		}
	case RGF_SM_LAST_ENEMY_YAW:
		{
			GetAngles(false);
			value = m_targetyaw;
			return true;
		}
	case RGF_SM_ENEMY_PITCH:
		{
			GetAngles(true);
			value = m_targetpitch;
			return true;
		}
	case RGF_SM_LAST_ENEMY_PITCH:
		{
			GetAngles(false);
			value = m_targetpitch;
			return true;
		}
	case RGF_SM_LAST_ENEMY_RANGE:
		{
			value = CCD->ActorManager()->DistanceFrom(m_LastTargetPoint, m_Actor);
			return true;
		}
	case RGF_SM_CURRENT_YAW:
		{
			GetAngles(true);
			value = m_actoryaw;
			return true;
		}
	case RGF_SM_YAW_SPEED:
		{
			value = m_yaw_speed;
			return true;
		}
	case RGF_SM_IDEAL_YAW:
		{
			value = m_ideal_yaw;
			return true;
		}
	case RGF_SM_CURRENT_PITCH:
		{
			GetAngles(true);
			value = m_actorpitch;
			return true;
		}
	case RGF_SM_PITCH_SPEED:
		{
			value = m_pitch_speed;
			return true;
		}
	case RGF_SM_IDEAL_PITCH:
		{
			value = m_ideal_pitch;
			return true;
		}
	case RGF_SM_IN_PAIN:
		{
			value = false;

			if(!m_Attribute.empty())
			{
				CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(m_Actor);

				if(m_OldAttributeAmount > theInv->Value(m_Attribute))
					value = true;

				m_OldAttributeAmount = theInv->Value(m_Attribute);
			}

			return true;
		}
	case RGF_SM_ANIMATE_AT_END:
		{
			value = CCD->ActorManager()->EndAnimation(m_Actor);
			return true;
		}
	case RGF_SM_ISFALLING:
		{
			value = false;

			if(CCD->ActorManager()->Falling(m_Actor) == GE_TRUE)
				if(CCD->ActorManager()->ReallyFall(m_Actor) == RGF_SUCCESS)
					value = true;

			return true;
		}
	case RGF_SM_CURRENT_X:
		{
			geVec3d Pos;
			CCD->ActorManager()->GetPosition(m_Actor, &Pos);
			value = Pos.X;
			return true;
		}
	case RGF_SM_CURRENT_Y:
		{
			geVec3d Pos;
			CCD->ActorManager()->GetPosition(m_Actor, &Pos);
			value = Pos.Y;
			return true;
		}
	case RGF_SM_CURRENT_Z:
		{
			geVec3d Pos;
			CCD->ActorManager()->GetPosition(m_Actor, &Pos);
			value = Pos.Z;
			return true;
		}
	case RGF_SM_PLAYER_X:
		{
			geVec3d Pos;
			CCD->ActorManager()->GetPosition(CCD->Player()->GetActor(), &Pos);
			value = Pos.X;
			return true;
		}
	case RGF_SM_PLAYER_Y:
		{
			geVec3d Pos;
			CCD->ActorManager()->GetPosition(CCD->Player()->GetActor(), &Pos);
			value = Pos.Y;
			return true;
		}
	case RGF_SM_PLAYER_Z:
		{
			geVec3d Pos;
			CCD->ActorManager()->GetPosition(CCD->Player()->GetActor(), &Pos);
			value = Pos.Z;
			return true;
		}
	case RGF_SM_DISTANCETOPOINT:
		{
			value = -1;

			if(m_ValidPoint)
			{
				geVec3d Pos;
				CCD->ActorManager()->GetPosition(m_Actor, &Pos);
				value = geVec3d_DistanceBetween(&m_CurrentPoint, &Pos);
			}

			return true;
		}
	case RGF_SM_PLAYERTOPOINT:
		{
			value = -1;

			if(m_ValidPoint)
			{
				geVec3d Pos;
				CCD->ActorManager()->GetPosition(CCD->Player()->GetActor(), &Pos);
				value = geVec3d_DistanceBetween(&m_CurrentPoint, &Pos);
			}

			return true;
		}
	case RGF_SM_KEY_PRESSED:
		{
			// TODO: change to new input system
			value = CCD->Input()->GetKeyboardInputNoWait();
			return true;
		}
	case RGF_SM_PLAYER_VIS:
		{
			GetAngles(true);
			value = CanSee(m_FOV, m_Actor, CCD->Player()->GetActor(), m_FOVBone);
			return true;
		}
	case RGF_SM_TARGET_NAME:
		{
			if(CCD->ActorManager()->IsActor(m_TargetActor))
				value = skString(CCD->ActorManager()->GetEntityName(m_TargetActor).c_str());
			else
				value = false;

			return true;
		}
	case RGF_SM_ENEMY_X:
		{
			geVec3d Pos;
			CCD->ActorManager()->GetPosition(m_TargetActor, &Pos);
			value = Pos.X;
			return true;
		}
	case RGF_SM_ENEMY_Y:
		{
			geVec3d Pos;
			CCD->ActorManager()->GetPosition(m_TargetActor, &Pos);
			value = Pos.Y;
			return true;
		}
	case RGF_SM_ENEMY_Z:
		{
			geVec3d Pos;
			CCD->ActorManager()->GetPosition(m_TargetActor, &Pos);
			value = Pos.Z;
			return true;
		}
	case RGF_SM_PLAYER_YAW:
		{
			geVec3d Orient;
			CCD->ActorManager()->GetRotation(CCD->Player()->GetActor(), &Orient);
			value = Orient.Y;
			return true;
		}
	case RGF_SM_POINT_VIS:
		{
			value = CanSeePoint(m_FOV, m_Actor, &m_CurrentPoint, m_FOVBone);
			return true;
		}
	case RGF_SM_PLAYER_WEAPON:
		{
			value = skString(CCD->Weapons()->GetCurrent().c_str());
			return true;
		}
	case RGF_SM_POINT_NAME:
		{
			if(m_Point.empty())
			{
				value = skString("FALSE");
				return true;
			}

			value = skString(m_Point.c_str());
			return true;
		}
	case RGF_SM_CAMERA_PITCH:
		{
			value = CCD->CameraManager()->GetTilt();
			return true;
		}
	case RGF_SM_LODLEVEL:
		{
			int Level = 0;
			CCD->ActorManager()->GetLODLevel(m_Actor, &Level);
			value = Level;
			return true;
		}
	case RGF_SM_CURRENT_SCREEN_X:
		{
			// Gets the ScreenX position of the active Pawn
			geVec3d Pos, ScreenPos;
			CCD->ActorManager()->GetPosition(m_Actor, &Pos);
			geCamera_TransformAndProject(CCD->CameraManager()->Camera(), &Pos, &ScreenPos);
			value = static_cast<int>(ScreenPos.X);
			return true;
		}
	case RGF_SM_CURRENT_SCREEN_Y:
		{
			// Gets the ScreenY position of the active Pawn
			geVec3d Pos, ScreenPos;
			CCD->ActorManager()->GetPosition(m_Actor, &Pos);
			geCamera_TransformAndProject(CCD->CameraManager()->Camera(), &Pos, &ScreenPos);
			value = static_cast<int>(ScreenPos.Y);
			return true;
		}
	case RGF_SM_PLAYER_SCREEN_X:
		{
			// Gets the ScreenX position of the player
			geVec3d Pos, ScreenPos;
			Pos = CCD->Player()->Position();
			geCamera_TransformAndProject(CCD->CameraManager()->Camera(), &Pos, &ScreenPos);
			value = static_cast<int>(ScreenPos.X);
			return true;
		}
	case RGF_SM_PLAYER_SCREEN_Y:
		{
			// Gets the ScreenY position of the player
			geVec3d Pos, ScreenPos;
			Pos = CCD->Player()->Position();
			geCamera_TransformAndProject(CCD->CameraManager()->Camera(), &Pos, &ScreenPos);
			value = static_cast<int>(ScreenPos.Y);
			return true;
		}
	case RGF_SM_LBUTTON_PRESSED:
		{
			// Check if left mouse button is pressed
			int SwapButton = GetSystemMetrics(SM_SWAPBUTTON);

			if(SwapButton)
			{
				value = (GetAsyncKeyState(VK_RBUTTON) < 0);
			}
			else
				value = (GetAsyncKeyState(VK_LBUTTON) < 0);

			return true;
		}
	case RGF_SM_RBUTTON_PRESSED:
		{
			// Check if right mouse button is pressed
			int SwapButton = GetSystemMetrics(SM_SWAPBUTTON);

			if(SwapButton)
			{
				value = (GetAsyncKeyState(VK_LBUTTON) < 0);
			}
			else
				value = (GetAsyncKeyState(VK_RBUTTON) < 0);

			return true;
		}
	case RGF_SM_MBUTTON_PRESSED:
		{
			// Check if middle mouse button is pressed
			value = (GetAsyncKeyState(VK_MBUTTON) < 0);
			return true;
		}
	case RGF_SM_PLAYER_ANIMATION:
		{
			value = skString(CCD->ActorManager()->GetMotion(CCD->Player()->GetActor()).c_str());
			return true;
		}
	case RGF_SM_PLAYER_VIEWPOINT:
		{
			value = CCD->Player()->GetViewPoint();
			return true;
		}
	default:
		{
			return skScriptedExecutable::getValue(fieldName, attribute, value);
		}
	}
}

/* ------------------------------------------------------------------------------------ */
//	sets a variable's value
/* ------------------------------------------------------------------------------------ */
bool ScriptedObject::setValue(const skString &fieldName,
							  const skString &attribute,
							  const skRValue &value)
{
	long variable = ScriptManager::GetHashMethod(fieldName);

	switch(variable)
	{
	case RGF_SM_LOWTIME:
		{
			m_lowTime = value.floatValue();
			return true;
		}
	case RGF_SM_THINKTIME:
		{
			m_ThinkTime = value.floatValue();
			return true;
		}
	case RGF_SM_THINK:
		{
			RestoreHLActorDynamicLighting();
			m_thinkorder = value.str().c_str();
			return true;
		}
	case RGF_SM_ICON:
		{
			m_Icon = value.str().c_str();
			return true;
		}
	case RGF_SM_ATTACK_FINISHED:
		{
			m_attack_finished = value.floatValue();
			return true;
		}
	case RGF_SM_ATTACK_STATE:
		{
			m_attack_state = value.intValue();
			return true;
		}
	case RGF_SM_YAW_SPEED:
		{
			m_yaw_speed = value.floatValue();
			return true;
		}
	case RGF_SM_IDEAL_YAW:
		{
			m_ideal_yaw = value.floatValue();
			return true;
		}
	case RGF_SM_PITCH_SPEED:
		{
			m_pitch_speed = value.floatValue();
			return true;
		}
	case RGF_SM_IDEAL_PITCH:
		{
			m_ideal_pitch = value.floatValue();
			return true;
		}
	default:
		{
			return skScriptedExecutable::setValue(fieldName, attribute, value);
		}
	}
}

/* ------------------------------------------------------------------------------------ */
//	calls a method in this object
/* ------------------------------------------------------------------------------------ */
bool ScriptedObject::lowmethod(const skString &methodName, skRValueArray &arguments,
							   skRValue &returnValue, skExecutableContext &ctxt)
{
	char param0[128], param4[128], param5[128];
	float param1;
	param0[0] = '\0';
	param4[0] = '\0';
	param5[0] = '\0';

	long method = ScriptManager::GetHashMethod(methodName);

	switch(method)
	{
	case RGF_SM_HIGHLEVEL:
		{
			PARMCHECK(1);
			m_HighLevel = GE_TRUE;
			m_thinkorder = arguments[0].str().c_str();
			m_RunOrder = true;
			m_ActionActive = false;
			RestoreHLActorDynamicLighting();
			return true;
		}
	case RGF_SM_ANIMATE:
		{
			PARMCHECK(1);
			CCD->ActorManager()->SetMotion(m_Actor, arguments[0].str().c_str());
			CCD->ActorManager()->SetHoldAtEnd(m_Actor, false);
			return true;
		}
	case RGF_SM_GRAVITY:
		{
			PARMCHECK(1);
			geVec3d Gravity = { 0.0f, 0.0f, 0.0f };

			if(arguments[0].boolValue())
				Gravity = CCD->Level()->GetGravity();

			CCD->ActorManager()->SetGravity(m_Actor, Gravity);
			return true;
		}
	case RGF_SM_PLAYSOUND:
		{
			PARMCHECK(1);
			strcpy(param0, arguments[0].str());

			if(!EffectC_IsStringNull(param0))
			{
				if(arguments.entries() > 1)
					PlaySound(param0, -1, GE_FALSE, arguments[1].floatValue());
				else
					PlaySound(param0, -1, GE_FALSE);
			}
			return true;
		}
	case RGF_SM_SPEAK:
		{
			PARMCHECK(1);
			Q_Speak(0.f, arguments);
			return true;
		}
	case RGF_SM_ENEMYEXIST:
		{
			PARMCHECK(1);
			strcpy(param0, arguments[0].str());
			returnValue = 0;

			if(CCD->ActorManager()->IsActor(m_TargetActor))
			{
				if(m_TargetActor == CCD->Player()->GetActor())
					strcpy(param0, "health");

				CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(m_TargetActor);

				if(theInv->Has(param0))
				{
					if(theInv->Value(param0) > 0)
					{
						returnValue = 3;
					}
					else
					{
						returnValue = 2;
					}

					return true;
				}
				else
				{
					returnValue = 1;
				}
			}

			return true;
		}
	case RGF_SM_GETEVENTSTATE:
		{
			PARMCHECK(1);
			returnValue = GetTriggerState(arguments[0].str().c_str());
			return true;
		}
	case RGF_SM_INTEGER:
		{
			// deprecated - use built-in toInt method
			PARMCHECK(1);
			returnValue = arguments[0].intValue();
			return true;
		}
	case RGF_SM_GETATTRIBUTE:
		{
			// USAGE:	GetAttribute(char *Attribute)
			//			GetAttribute(char *Attribute, char *EntityName)
			// PARMCHECK(1);

			CPersistentAttributes *theInv;

			if(arguments.entries() > 1)
			{
				strcpy(param4, arguments[1].str());

				if(!stricmp(param4, "Player"))
					theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
				else
					theInv = CCD->ActorManager()->Inventory(CCD->ActorManager()->GetByEntityName(param4));
			}
			else
			{
				theInv = CCD->ActorManager()->Inventory(m_Actor);
			}

			returnValue = theInv->Value(arguments[0].str().c_str());
			return true;
		}
	case RGF_SM_GETCURFLIPBOOK:
		{
			PARMCHECK(1);
			FlipBook *pEntityData = NULL;

			if(CCD->Level()->FlipBooks()->LocateEntity(arguments[0].str().c_str(), reinterpret_cast<void**>(&pEntityData)) == RGF_SUCCESS)
				returnValue = pEntityData->CurTex;
			else
				returnValue = 0;

			return true;
		}
	case RGF_SM_MODIFYATTRIBUTE:
		{
			PARMCHECK(2);

			CPersistentAttributes *theInv;

			if(arguments.entries() > 2)
			{
				strcpy(param4, arguments[2].str());

				if(!stricmp(param4, "Player"))
				{
					// deprecated
					theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
					returnValue = theInv->Modify(arguments[0].str().c_str(), arguments[1].intValue());
					sxInventory::GetSingletonPtr()->UpdateItem(arguments[0].str().c_str(), true);
					return true;
				}
				else
				{
					theInv = CCD->ActorManager()->Inventory(CCD->ActorManager()->GetByEntityName(param4));
				}
			}
			else
			{
				theInv = CCD->ActorManager()->Inventory(m_Actor);
			}

			returnValue = theInv->Modify(arguments[0].str().c_str(), arguments[1].intValue());
			return true;
		}
	case RGF_SM_SETATTRIBUTE:
		{
			PARMCHECK(2);

			CPersistentAttributes *theInv;

			if(arguments.entries() > 2)
			{
				strcpy(param4, arguments[2].str());

				if(!stricmp(param4, "Player"))
				{
					// deprecated
					theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
					returnValue = theInv->Set(arguments[0].str().c_str(), arguments[1].intValue());
					sxInventory::GetSingletonPtr()->UpdateItem(arguments[0].str().c_str(), true);
					return true;
				}
				else
				{
					theInv = CCD->ActorManager()->Inventory(CCD->ActorManager()->GetByEntityName(param4));
				}
			}
			else
			{
				theInv = CCD->ActorManager()->Inventory(m_Actor);
			}

			returnValue = theInv->Set(arguments[0].str().c_str(), arguments[1].intValue());
			return true;
		}
	case RGF_SM_SETPLAYERWEAPON:
		{
			// depreacted - use Player.SetWeapon
			PARMCHECK(1);
			if(arguments[0].type() == skRValue::T_Int)
			{
				int slot = arguments[0].intValue();
				if(slot < 0 || slot >= MAX_WEAPONS)
					CCD->Weapons()->Holster();
				else
					CCD->Weapons()->SetWeapon(slot);
			}
			else if(arguments[0].type() == skRValue::T_String)
			{
				CCD->Weapons()->SetWeapon(arguments[0].str().c_str());
			}

			return true;
		}
	case RGF_SM_SETUSEITEM:
		{
			// deprecated - use Player.SetUseItem
			PARMCHECK(1);
			if(CCD->Player()->SetUseAttribute(arguments[0].str().c_str()))
				CCD->HUD()->ActivateElement(arguments[0].str().c_str(), true);
			return true;
		}
	case RGF_SM_CLEARUSEITEM:
		{
			// deprecated - use Player.ClearUseItem
			PARMCHECK(1);
			CCD->HUD()->ActivateElement(arguments[0].str().c_str(), false);
			CCD->Player()->DelUseAttribute(arguments[0].str().c_str());
			return true;
		}
	case RGF_SM_STRINGCOPY:
		{
			PARMCHECK(1);
			// DEPRECATED - use x = toString(y) instead
			returnValue = arguments[0].str();
			return true;
		}
	case RGF_SM_LEFTCOPY:
		{
			PARMCHECK(2);

			int length = arguments[1].intValue();
			int slength = arguments[0].str().length();

			if(length > slength)
				length = slength;

			returnValue = arguments[0].str().substr(0, length);

			return true;
		}
	case RGF_SM_RIGHTCOPY:
		{
			PARMCHECK(2);

			int length = arguments[1].intValue();
			int slength = arguments[0].str().length();

			if(length > slength)
				length = slength;

			returnValue = arguments[0].str().substr(slength-length);
			return true;
		}
	case RGF_SM_ISENTITYVSIBLE:
		{
			PARMCHECK(1);
			returnValue = CanSee(m_FOV, m_Actor, CCD->ActorManager()->GetByEntityName(arguments[0].str().c_str()), m_FOVBone);
			return true;
		}
	case RGF_SM_DAMAGEENTITY:
		{
			PARMCHECK(3);
			float amount = arguments[0].floatValue();
			CCD->Level()->Damage()->DamageActor(CCD->ActorManager()->GetByEntityName(arguments[2].str().c_str()),
										amount, arguments[1].str().c_str(),
										amount, arguments[1].str().c_str(),
										"Melee");
			return true;
		}
	case RGF_SM_ANIMATEENTITY:
		{
			PARMCHECK(3);
			CCD->ActorManager()->SetMotion(CCD->ActorManager()->GetByEntityName(arguments[1].str().c_str()), arguments[0].str().c_str());
			CCD->ActorManager()->SetHoldAtEnd(CCD->ActorManager()->GetByEntityName(arguments[1].str().c_str()), arguments[2].boolValue());
			return true;
		}
	case RGF_SM_ANIMATEHOLD:
		{
			PARMCHECK(1);
			CCD->ActorManager()->SetMotion(m_Actor, arguments[0].str().c_str());
			CCD->ActorManager()->SetHoldAtEnd(m_Actor, true);
			return true;
		}
	case RGF_SM_ANIMATETARGET:
		{
			PARMCHECK(2);
			CCD->ActorManager()->SetMotion(m_TargetActor, arguments[0].str().c_str());
			CCD->ActorManager()->SetHoldAtEnd(m_TargetActor, arguments[1].boolValue());
			return true;
		}
	case RGF_SM_GETENTITYX:
		{
			PARMCHECK(1);
			geVec3d Pos;
			CCD->ActorManager()->GetPosition(CCD->ActorManager()->GetByEntityName(arguments[0].str().c_str()), &Pos);
			returnValue = Pos.X;
			return true;
		}
	case RGF_SM_GETENTITYY:
		{
			PARMCHECK(1);
			geVec3d Pos;
			CCD->ActorManager()->GetPosition(CCD->ActorManager()->GetByEntityName(arguments[0].str().c_str()), &Pos);
			returnValue = Pos.Y;
			return true;
		}
	case RGF_SM_GETENTITYZ:
		{
			PARMCHECK(1);
			geVec3d Pos;
			CCD->ActorManager()->GetPosition(CCD->ActorManager()->GetByEntityName(arguments[0].str().c_str()), &Pos);
			returnValue = Pos.Z;
			return true;
		}
	case RGF_SM_ISKEYDOWN:
		{
			// deprecated - use Input.IsKeyDown
			PARMCHECK(1);
			returnValue = CCD->Input()->IsKeyDown(static_cast<OIS::KeyCode>(arguments[0].intValue()));

			return true;
		}
	case RGF_SM_GETENTITYYAW:
		{
			PARMCHECK(1);
			geVec3d Orient;
			geActor *MasterActor;
			MasterActor = CCD->ActorManager()->GetByEntityName(arguments[0].str().c_str());

			if(!MasterActor)
				return true;

			CCD->ActorManager()->GetRotate(MasterActor, &Orient);
			returnValue = Orient.Y;
			return true;
		}
	case RGF_SM_MATCHENTITYANGLES:
		{
			PARMCHECK(1);
			GetAngles(true);
			geVec3d theRotation;
			geActor *MasterActor;
			MasterActor = CCD->ActorManager()->GetByEntityName(arguments[0].str().c_str());

			if(!MasterActor)
				return true;

			CCD->ActorManager()->GetRotate(MasterActor, &theRotation);
			CCD->ActorManager()->Rotate(m_Actor, theRotation);
			return true;
		}
	case RGF_SM_FACEPOINT:
		{
			if(m_ValidPoint)
				CCD->ActorManager()->RotateToFacePoint(m_Actor, m_CurrentPoint);

			return true;
		}
	case RGF_SM_NEWPOINT:
		{
			PARMCHECK(1);
			strcpy(param0, arguments[0].str());
			const char *EntityType = CCD->Level()->EntityRegistry()->GetEntityType(param0);

			if(EntityType)
			{
				if(!stricmp(EntityType, "ScriptPoint"))
				{
					ScriptPoint *pProxy;
					if(CCD->Level()->ScriptPoints()->LocateEntity(param0, reinterpret_cast<void**>(&pProxy)) == RGF_SUCCESS)
					{
						m_CurrentPoint = pProxy->origin;
						m_ValidPoint = true;
						m_Point = param0;
						m_NextOrder.clear();

						if(!EffectC_IsStringNull(pProxy->NextOrder))
							m_NextOrder = pProxy->NextOrder;
					}
				}
			}

			return true;
		}
	case RGF_SM_GETPOINTYAW:
		{
			PARMCHECK(1);
			strcpy(param0, arguments[0].str());
			geVec3d pRotation;
			const char *EntityType = CCD->Level()->EntityRegistry()->GetEntityType(param0);
			returnValue = 0.0f;

			if(EntityType)
			{
				if(!stricmp(EntityType, "ScriptPoint"))
				{
					ScriptPoint *pProxy;
					if(CCD->Level()->ScriptPoints()->LocateEntity(param0, reinterpret_cast<void**>(&pProxy)) == RGF_SUCCESS)
					{
						pRotation = pProxy->Angle;
						returnValue = pRotation.Y;
					}
				}
			}

			return true;
		}
	case RGF_SM_NEXTPOINT:
		{
			const char *EntityType = CCD->Level()->EntityRegistry()->GetEntityType(m_Point.c_str());

			if(EntityType)
			{
				if(!stricmp(EntityType, "ScriptPoint"))
				{
					ScriptPoint *pProxy;
					if(CCD->Level()->ScriptPoints()->LocateEntity(m_Point.c_str(), reinterpret_cast<void**>(&pProxy)) == RGF_SUCCESS)
					{
						m_ValidPoint = false;

						if(!EffectC_IsStringNull(pProxy->NextPoint))
						{
							m_Point = pProxy->NextPoint;
							m_NextOrder.clear();

							if(!EffectC_IsStringNull(pProxy->NextOrder))
								m_NextOrder = pProxy->NextOrder;

							if(!m_Point.empty())
							{
								EntityType = CCD->Level()->EntityRegistry()->GetEntityType(m_Point.c_str());

								if(EntityType)
								{
									if(!stricmp(EntityType, "ScriptPoint"))
									{
										if(CCD->Level()->ScriptPoints()->LocateEntity(m_Point.c_str(), reinterpret_cast<void**>(&pProxy)) == RGF_SUCCESS)
										{
											m_CurrentPoint = pProxy->origin;
											m_ValidPoint = true;
										}
									}
								}
							}
						}
					}
				}
			}

			return true;
		}
	case RGF_SM_SETTARGET:
		{
			PARMCHECK(1);
			geActor *MActor;
			MActor = CCD->ActorManager()->GetByEntityName(arguments[0].str().c_str());

			if(!MActor)
				return true;

			m_TargetActor = MActor;
			return true;
		}
	case RGF_SM_GETDISTANCETO:
		{
			PARMCHECK(1);
			geVec3d Pos, tPos;
			geActor *MActor;
			MActor = CCD->ActorManager()->GetByEntityName(arguments[0].str().c_str());

			if(!MActor)
				return true;

			CCD->ActorManager()->GetPosition(m_Actor, &tPos);
			CCD->ActorManager()->GetPosition(MActor, &Pos);
			returnValue = geVec3d_DistanceBetween(&tPos, &Pos);
			return true;
		}
	case RGF_SM_TELEPORTTOPOINT:
		{
			if(arguments.entries() < 6)
			{
				if(arguments.entries() < 5)
				{
					if(arguments.entries() < 4)
					{
						if(arguments.entries() < 3)
						{
							if(arguments.entries() < 2)
							{
								arguments.append(0.0f);
							}
							arguments.append(0.0f);
						}
						arguments.append(0.0f);
					}
					arguments.append(false);
				}
				arguments.append(skString(""));
			}

			Q_TeleportToPoint(0.0f, arguments);

			return true;
		}
	case RGF_SM_TELEPORTENTITY:  // deprecated, use RGF_SM_TELEPORTTOPOINT
		{
			// USAGE:	TeleportEntity(EntityName, Point)
			//			TeleportEntity(EntityName, Point, OffsetX, OffsetY, OffsetZ)
			//			TeleportEntity(EntityName, Point, OffsetX, OffsetY, OffsetZ, UseAngle)
			PARMCHECK(2);
			strcpy(param0, arguments[1].str());
			strcpy(param4, arguments[0].str());

			geActor *tActor;

			if(!stricmp(param4, "Player"))
				tActor = CCD->Player()->GetActor();
			else
				tActor = CCD->ActorManager()->GetByEntityName(param4);

			if(!tActor)
				return true;

			const char *EType = CCD->Level()->EntityRegistry()->GetEntityType(param0);

			if(EType)
			{
				if(!stricmp(EType, "ScriptPoint"))
				{
					ScriptPoint *pProxy;
					if(CCD->Level()->ScriptPoints()->LocateEntity(param0, reinterpret_cast<void**>(&pProxy)) == RGF_SUCCESS)
					{
						geVec3d Pos = pProxy->origin;

						if(arguments.entries() > 4)
						{
							Pos.X += arguments[2].floatValue();
							Pos.Y += arguments[3].floatValue();
							Pos.Z += arguments[4].floatValue();
						}

						CCD->ActorManager()->Position(tActor, Pos);

						if(arguments.entries() > 5)
						{
							if(arguments[5].boolValue())
							{
								geVec3d RealAngle;
								geVec3d_Scale(&(pProxy->Angle), GE_PIOVER180, &RealAngle);
								RealAngle.Y -= GE_PIOVER2;

								CCD->ActorManager()->Rotate(tActor, RealAngle);

								if(tActor == m_Actor)
								{
									if(m_WeaponActor)
										CCD->ActorManager()->Rotate(m_WeaponActor, RealAngle);
								}

								if(CCD->Player()->GetActor() == tActor
									&& CCD->Player()->GetViewPoint() == FIRSTPERSON)
								{
									CCD->CameraManager()->SetRotation(RealAngle);
								}
							}
						}

						if(tActor == m_Actor)
						{
							if(m_WeaponActor)
								CCD->ActorManager()->Position(m_WeaponActor, Pos);
						}
					}
				}
			}

			return true;
		}
	case RGF_SM_SAVEATTRIBUTES:
		{
			// deprecated use player method
			PARMCHECK(1);
			CCD->Player()->SaveAttributesAscii(arguments[0].str().c_str());
			return true;
		}
	case RGF_SM_TRACETOACTOR:
		{
			PARMCHECK(4);
			geXForm3d Xf;
			returnValue = skString("FALSE");
			bool bone;

			if(m_WeaponActor)
				bone = geActor_GetBoneTransform(m_WeaponActor, arguments[0].str().c_str(), &Xf);
			else
				bone = geActor_GetBoneTransform(m_Actor, arguments[0].str().c_str(), &Xf);

			if(!bone)
				return true;

			geVec3d OldPos, Pos, Normal, theRotation, Direction;
			geActor *pActor;
			geFloat T;

			CCD->ActorManager()->GetPosition(m_Actor, &OldPos);
			geVec3d_Copy(&(Xf.Translation), &OldPos);
			CCD->ActorManager()->GetRotate(m_Actor, &theRotation);
			Pos = OldPos;

			geXForm3d_SetZRotation(&Xf, theRotation.Z);
			geXForm3d_RotateX(&Xf, theRotation.X);
			geXForm3d_RotateY(&Xf, theRotation.Y);

			geXForm3d_GetUp(&Xf, &Direction);
			geVec3d_AddScaled(&Pos, &Direction, arguments[2].floatValue(), &Pos);

			geXForm3d_GetLeft(&Xf, &Direction);
			geVec3d_AddScaled(&Pos, &Direction, arguments[1].floatValue(), &Pos);

			geXForm3d_GetIn(&Xf, &Direction);
			geVec3d_AddScaled(&Pos, &Direction, arguments[3].floatValue(), &Pos);

			if(CCD->ActorManager()->DoesRayHitActor(OldPos, Pos, &pActor, m_Actor, &T, &Normal) == GE_TRUE)
			{
				returnValue = skString(CCD->ActorManager()->GetEntityName(pActor).c_str());
			}

			return true;
		}
	case RGF_SM_ANIMATEBLEND:
		{
			PARMCHECK(2);
			param1 = arguments[1].floatValue();

			if(param1 > 0.0f)
			{
				CCD->ActorManager()->SetTransitionMotion(m_Actor, arguments[0].str().c_str(), param1, "");
				CCD->ActorManager()->SetHoldAtEnd(m_Actor, true);
			}

			return true;
		}
	case RGF_SM_ANIMATIONSPEED:
		{
			PARMCHECK(1);
			CCD->ActorManager()->SetAnimationSpeed(m_Actor, m_AnimSpeed * arguments[0].floatValue());
			return true;
		}
	case RGF_SM_SETCOLLISION:
		{
			CCD->ActorManager()->SetCollide(m_Actor);

			if(m_WeaponActor)
			{
				CCD->ActorManager()->SetCollide(m_WeaponActor);
				CCD->ActorManager()->SetNoCollide(m_WeaponActor);
			}

			return true;
		}
	case RGF_SM_SETNOCOLLISION:
		{
			CCD->ActorManager()->SetNoCollide(m_Actor);
			return true;
		}
	case RGF_SM_DAMAGEAREA:
		{
			PARMCHECK(3);

			geVec3d Pos;
			geFloat dAmount, dRange;

			dAmount = arguments[0].floatValue();
			dRange = arguments[1].floatValue();
			strcpy(param0, arguments[2].str());

			CCD->ActorManager()->GetPosition(m_Actor, &Pos);
			CCD->Level()->Damage()->DamageActorInRange(Pos, dRange, dAmount, param0, 0.0f, "", "Explosion");
			CCD->Level()->Damage()->DamageModelInRange(Pos, dRange, dAmount, param0, 0.0f, "");
			return true;
		}
	case RGF_SM_PLAYERMATCHANGLES:
		{
			// deprecated - use Player.MatchEntityAngles
			PARMCHECK(1);

			GetAngles(true);
			geVec3d theRotation;
			geActor *MasterActor;
			MasterActor = CCD->ActorManager()->GetByEntityName(arguments[0].str().c_str());

			if(!MasterActor)
				return true;

			CCD->ActorManager()->GetRotate(MasterActor, &theRotation);
			CCD->ActorManager()->Rotate(CCD->Player()->GetActor(), theRotation);
			return true;
		}
	case RGF_SM_CONVERTDEGREES:
		{
			// deprecated - use Math.deg2rad
			PARMCHECK(1);
			returnValue = arguments[0].floatValue()*GE_PIOVER180;
			return true;
		}
	case RGF_SM_ATTACHCAMERA:
		{
			CCD->CameraManager()->BindToActor(m_Actor);
			return true;
		}
	case RGF_SM_ATTACHCAMERATOBONE:
		{
			PARMCHECK(1);
			CCD->CameraManager()->BindToActorBone(m_Actor, arguments[0].str().c_str());
			return true;
		}
	case RGF_SM_ATTACHCAMERATOENTITY:
		{
			PARMCHECK(2);
			CCD->CameraManager()->BindToActorBone(CCD->ActorManager()->GetByEntityName(arguments[0].str().c_str()),
				arguments[1].str().c_str());
			return true;
		}
	case RGF_SM_DETACHCAMERA:
		{
			CCD->CameraManager()->BindToActor(CCD->Player()->GetActor());
			return true;
		}
	case RGF_SM_TILTCAMERA:
		{
			PARMCHECK(1);
			float param3 = arguments[0].floatValue();

			if(param3 < 0)
			{
				CCD->CameraManager()->TiltUp(-param3);
			}
			else
			{
				CCD->CameraManager()->TiltDown(param3);
			}

			return true;
		}
	case RGF_SM_POSITIONTOPLATFORM:
		{
			PARMCHECK(5);

			geVec3d Pos, theRotation;
			geXForm3d Xf;
			MovingPlatform *pEntity;

			if(CCD->Level()->Platforms()->LocateEntity(arguments[0].str().c_str(), reinterpret_cast<void**>(&pEntity)) == RGF_SUCCESS)
			{
				CCD->Level()->ModelManager()->GetPosition(pEntity->Model, &Pos);
				CCD->Level()->ModelManager()->GetRotation(pEntity->Model, &theRotation);

				if(pEntity->ParentModel)
				{
					geWorld_GetModelXForm(CCD->World(), pEntity->Model, &Xf);
					Pos.X += Xf.Translation.X;
					Pos.Y += Xf.Translation.Y;
					Pos.Z += Xf.Translation.Z;
					Pos.X += arguments[1].floatValue();
					Pos.Y += arguments[2].floatValue();
					Pos.Z += arguments[3].floatValue();
				}

				CCD->ActorManager()->Position(m_Actor, Pos);
				if(m_WeaponActor)
					CCD->ActorManager()->Position(m_WeaponActor, Pos);

				if(arguments[4].boolValue())
				{
					CCD->ActorManager()->Rotate(m_Actor, theRotation);

					if(m_WeaponActor)
							CCD->ActorManager()->Rotate(m_WeaponActor, theRotation);
				}
			}

			return true;
		}
	case RGF_SM_ACTIVATETRIGGER:
		{
			PARMCHECK(1);
			CCD->Level()->Triggers()->HandleTriggerEvent(arguments[0].str().c_str());
			return true;
		}
	case RGF_SM_UPDATEENEMYVIS:
		{
			GetAngles(true);

			if(!m_TargetActor)
				return true;

			CCD->ActorManager()->GetPosition(m_TargetActor, &m_LastTargetPoint);
			geExtBox theBox;
			CCD->ActorManager()->GetBoundingBox(m_TargetActor, &theBox);
			m_LastTargetPoint.Y += ((theBox.Max.Y)*0.5f);
			return true;
		}
	case RGF_SM_TARGETPLAYER:
		{
			m_TargetActor = CCD->Player()->GetActor();
			return true;
		}
	case RGF_SM_FIREPROJECTILEBLIND:
		{
			PARMCHECK(6);

			geXForm3d Xf;
			geVec3d Pos, theRotation, Direction, Orient, tPoint;
			strcpy(param0, arguments[1].str());	// Bone
			strcpy(param4, arguments[0].str());	// Projectile
			strcpy(param5, arguments[5].str());	// Attribute
			bool bone;

			if(m_WeaponActor)
				bone = geActor_GetBoneTransform(m_WeaponActor, param0, &Xf);
			else
				bone = geActor_GetBoneTransform(m_Actor, param0, &Xf);

			if(!bone)
				return true;

			geVec3d_Copy(&(Xf.Translation), &Pos);
			CCD->ActorManager()->GetRotate(m_Actor, &theRotation);

			geXForm3d_SetZRotation(&Xf, theRotation.Z);
			geXForm3d_RotateX(&Xf, theRotation.X);
			geXForm3d_RotateY(&Xf, theRotation.Y);

			geXForm3d_GetIn(&Xf, &Direction);
			geVec3d_AddScaled(&Pos, &Direction, 1000.0f, &tPoint);
			geVec3d_AddScaled(&Pos, &Direction, arguments[4].floatValue(), &Pos);

			geXForm3d_GetUp(&Xf, &Direction);
			geVec3d_AddScaled(&Pos, &Direction, arguments[3].floatValue(), &Pos);

			geXForm3d_GetLeft(&Xf, &Direction);
			geVec3d_AddScaled(&Pos, &Direction, arguments[2].floatValue(), &Pos);

			geVec3d_Subtract(&tPoint, &Pos, &Orient);
			float l = geVec3d_Length(&Orient);

			if(l > 0.0f)
			{
				float x = Orient.X;
				Orient.X = GE_PIOVER2 - acos(Orient.Y / l);
				Orient.Y = atan2(x, Orient.Z) + GE_PI;
				Orient.Z = 0.0f;	// roll is zero - always!!?

				CCD->Weapons()->AddProjectile(Pos, Pos, Orient, param4, param5, param5);
			}

			return true;
		}
	case RGF_SM_SETTARGETPOINT:
		{
			PARMCHECK(3);

			geXForm3d Xf;
			geVec3d Pos, theRotation, Direction;
			bool Offset = true;

			if(arguments.entries()>3)
				Offset = arguments[3].boolValue();

			if(Offset)
			{
				CCD->ActorManager()->GetPosition(m_Actor, &Pos);
				CCD->ActorManager()->GetRotate(m_Actor, &theRotation);

				geXForm3d_SetZRotation(&Xf, theRotation.Z);
				geXForm3d_RotateX(&Xf, theRotation.X);
				geXForm3d_RotateY(&Xf, theRotation.Y);

				geXForm3d_GetUp(&Xf, &Direction);
				geVec3d_AddScaled(&Pos, &Direction, arguments[1].floatValue(), &Pos);

				geXForm3d_GetLeft(&Xf, &Direction);
				geVec3d_AddScaled(&Pos, &Direction, arguments[0].floatValue(), &Pos);

				geXForm3d_GetIn(&Xf, &Direction);
				geVec3d_AddScaled(&Pos, &Direction, arguments[2].floatValue(), &Pos);
			}
			else
			{
				geVec3d_Set(&Pos, arguments[0].floatValue(), arguments[1].floatValue(), arguments[2].floatValue());
			}

			m_UpdateTargetPoint = Pos;
			return true;
		}
	case RGF_SM_GETBONEX:
		{
			// USAGE: GetBoneX("ENTITY NAME", "BONE NAME");
			PARMCHECK(1);
			geVec3d Pos;
			CCD->ActorManager()->GetBonePosition(CCD->ActorManager()->GetByEntityName(arguments[0].str().c_str()),
				arguments[1].str().c_str(), &Pos);
			returnValue = Pos.X;
			return true;
		}
	case RGF_SM_GETBONEY:
		{
			// USAGE: GetBoneY("ENTITY NAME", "BONE NAME");
			PARMCHECK(2);
			geVec3d Pos;
			CCD->ActorManager()->GetBonePosition(CCD->ActorManager()->GetByEntityName(arguments[0].str().c_str()),
				arguments[1].str().c_str(), &Pos);
			returnValue = Pos.Y;
			return true;
		}
	case RGF_SM_GETBONEZ:
		{
			// USAGE: GetBoneZ("ENTITY NAME", "BONE NAME");
			PARMCHECK(2);
			geVec3d Pos;
			CCD->ActorManager()->GetBonePosition(CCD->ActorManager()->GetByEntityName(arguments[0].str().c_str()),
				arguments[1].str().c_str(), &Pos);
			returnValue = Pos.Z;
			return true;
		}
	case RGF_SM_GETBONEYAW:
		{
			// USAGE: GetBoneYaw("ENTITY NAME", "BONE NAME");
			PARMCHECK(2);
			geVec3d Pos;
			CCD->ActorManager()->GetBoneRotation(CCD->ActorManager()->GetByEntityName(arguments[0].str().c_str()),
				arguments[1].str().c_str(), &Pos);
			returnValue = Pos.Y;
			return true;
		}
	case RGF_SM_SETPOSITION:
		{
			// USAGE: SetPosition("ENTITY NAME", X, Y, Z);
			PARMCHECK(4);
			geVec3d Pos;
			geVec3d_Set(&Pos, arguments[1].floatValue(), arguments[2].floatValue(), arguments[3].floatValue());

			CCD->ActorManager()->Position(CCD->ActorManager()->GetByEntityName(arguments[0].str().c_str()), Pos);
			return true;
		}
	case RGF_SM_ISBUTTONDOWN:
		{
			// deprecated - use: Input.IsJoystickButtonDown(int, int);
			PARMCHECK(2);
			returnValue = false;
			OIS::JoyStick* js = CCD->Input()->GetJoystick(arguments[0].intValue());
			if(js)
			{
				if(js->getJoyStickState().mButtons.size() > static_cast<unsigned int>(arguments[1].intValue()))
					returnValue = js->getJoyStickState().mButtons[arguments[1].intValue()];
			}

			return true;
		}
	case RGF_SM_GETJOYAXISX:
		{
			// deprecated - use: int Input.GetJoystickAxisX(int);
			PARMCHECK(1);
			returnValue = 0;
			OIS::JoyStick* js = CCD->Input()->GetJoystick(arguments[0].intValue());
			if(js)
			{
				if(js->getJoyStickState().mAxes.size() > 0)
					returnValue = js->getJoyStickState().mAxes[0].abs;
			}

			return true;
		}
	case RGF_SM_GETJOYAXISY:
		{
			// deprecated - use: int Input.GetJoystickAxisY(int);
			PARMCHECK(1);
			returnValue = 0;
			OIS::JoyStick* js = CCD->Input()->GetJoystick(arguments[0].intValue());
			if(js)
			{
				if(js->getJoyStickState().mAxes.size() > 1)
					returnValue = js->getJoyStickState().mAxes[1].abs;
			}

			return true;
		}
	case RGF_SM_GETJOYAXISZ:
		{
			// deprecated - use: int Input.GetJoystickAxisZ(int);
			PARMCHECK(1);
			returnValue = 0;
			OIS::JoyStick* js = CCD->Input()->GetJoystick(arguments[0].intValue());
			if(js)
			{
				if(js->getJoyStickState().mAxes.size() > 2)
					returnValue = js->getJoyStickState().mAxes[2].abs;
			}

			return true;
		}
	case RGF_SM_GETNUMJOYSTICKS:
		{
			// deprecated - use: int Input.GetNumberOfJoysticks();
			returnValue = CCD->Input()->GetNumberOfJoysticks();
			return true;
		}
	case RGF_SM_SETBOUNDINGBOX:
		{
			// USAGE SetBoundingBox(ANIMATION, width);
			PARMCHECK(2);
			float width = arguments[1].floatValue(); // width
			CCD->ActorManager()->SetBoundingBox(m_Actor, arguments[0].str().c_str());
			geExtBox theBox;
			CCD->ActorManager()->GetBoundingBox(m_Actor, &theBox);
			float CurrentHeight;
			CurrentHeight = theBox.Max.Y;

			if(width > 0.0f)
				CCD->ActorManager()->SetBBox(m_Actor, width, -CurrentHeight*2.0f, width);

			return true;
		}
	case RGF_SM_GETBONETOBONE:
		{
			// USAGE: FLOAT GetBoneToBone(BONE NAME, TARGET ENTITY NAME, TARGET BONE NAME);
			PARMCHECK(3);
			geVec3d bpos1, bpos2;
			CCD->ActorManager()->GetBonePosition(m_Actor, arguments[0].str().c_str(), &bpos1);
			CCD->ActorManager()->GetBonePosition(CCD->ActorManager()->GetByEntityName(arguments[1].str().c_str()),
				arguments[2].str().c_str(), &bpos2);
			returnValue = geVec3d_DistanceBetween(&bpos1, &bpos2);
			return true;
		}
	case RGF_SM_SWITCHVIEW:
		{
			// USAGE SwitchView(INT 0-2);
			PARMCHECK(1);
			CCD->Player()->SwitchCamera(arguments[0].intValue());
			return true;
		}
	case RGF_SM_FORCEENTITYUP:
		{
			PARMCHECK(2);
			geActor *actor = CCD->ActorManager()->GetByEntityName(arguments[1].str().c_str());
			if(actor)
			{
				float amount = arguments[0].floatValue();
				geVec3d theUp;
				CCD->ActorManager()->UpVector(actor, &theUp);
				CCD->ActorManager()->SetForce(actor, 0, theUp, amount, amount);
			}
			return true;
		}
	case RGF_SM_FORCEENTITYDOWN:
		{
			PARMCHECK(2);
			geActor *actor = CCD->ActorManager()->GetByEntityName(arguments[1].str().c_str());
			if(actor)
			{
				float amount = arguments[0].floatValue();
				geVec3d theUp;
				CCD->ActorManager()->UpVector(actor, &theUp);
				geVec3d_Inverse(&theUp);
				CCD->ActorManager()->SetForce(actor, 0, theUp, amount, amount);
			}
			return true;
		}
	case RGF_SM_FORCEENTITYRIGHT:
		{
			PARMCHECK(2);

			geActor *actor = CCD->ActorManager()->GetByEntityName(arguments[1].str().c_str());
			if(actor)
			{
				float amount = arguments[0].floatValue();
				geVec3d left;
				CCD->ActorManager()->LeftVector(actor, &left);
				geVec3d_Inverse(&left);
				CCD->ActorManager()->SetForce(actor, 1, left, amount, amount);
			}
			return true;
		}
	case RGF_SM_FORCEENTITYLEFT:
		{
			PARMCHECK(2);
			geActor *actor = CCD->ActorManager()->GetByEntityName(arguments[1].str().c_str());
			if(actor)
			{
				float amount = arguments[0].floatValue();
				geVec3d left;
				CCD->ActorManager()->LeftVector(actor, &left);
				CCD->ActorManager()->SetForce(actor, 1, left, amount, amount);
			}
			return true;
		}
	case RGF_SM_FORCEENTITYFORWARD:
		{
			PARMCHECK(2);
			geActor *actor = CCD->ActorManager()->GetByEntityName(arguments[1].str().c_str());
			if(actor)
			{
				float amount = arguments[0].floatValue();
				geVec3d in;
				CCD->ActorManager()->InVector(actor, &in);
				CCD->ActorManager()->SetForce(actor, 2, in, amount, amount);
			}
			return true;
		}
	case RGF_SM_FORCEENTITYBACKWARD:
		{
			PARMCHECK(2);
			geActor *actor = CCD->ActorManager()->GetByEntityName(arguments[1].str().c_str());
			if(actor)
			{
				float amount = arguments[0].floatValue();
				geVec3d in;
				CCD->ActorManager()->InVector(actor, &in);
				geVec3d_Inverse(&in);
				CCD->ActorManager()->SetForce(actor, 2, in, amount, amount);
			}
			return true;
		}
	case RGF_SM_GETGROUNDTEXTURE:
		{
			char Texture[256];
			geVec3d fPos,tPos;
			CCD->ActorManager()->GetPosition(m_Actor, &fPos);
			tPos = fPos;
			tPos.Y -= 1000.f;
			getSingleTextureNameByTrace(CCD->World(), &fPos, &tPos, Texture);
			returnValue = skString(Texture);
			return true;
		}
	case RGF_SM_GETPLAYERGROUNDTEXTURE:
		{
			// deprecated - use Player.GetGroundTexture
			char Texture[256];
			geVec3d fPos,tPos;
			CCD->ActorManager()->GetPosition(CCD->Player()->GetActor(),&fPos);
			tPos = fPos;
			tPos.Y -= 1000.f;
			getSingleTextureNameByTrace(CCD->World(), &fPos, &tPos, Texture);
			returnValue = skString(Texture);
			return true;
		}
	case RGF_SM_POSITIONTOBONE:
		{
			PARMCHECK(2);
			geVec3d bPos;
			CCD->ActorManager()->GetBonePosition(CCD->ActorManager()->GetByEntityName(arguments[0].str().c_str()),
				arguments[1].str().c_str(), &bPos);
			CCD->ActorManager()->Position(m_Actor, bPos);
			return true;
		}
	case RGF_SM_CHANGEMATERIAL:
		{
			PARMCHECK(1);
			strcpy(param0, arguments[0].str());

			if(!EffectC_IsStringNull(param0))
				CCD->ActorManager()->ChangeMaterial(m_Actor, param0);

			return true;
		}
	case RGF_SM_CHANGEWEAPONMATERIAL:
		{
			PARMCHECK(1);

			if(!m_WeaponActor)
				return true;

			strcpy(param0, arguments[0].str());

			if(!EffectC_IsStringNull(param0))
				CCD->ActorManager()->ChangeMaterial(m_WeaponActor, param0);

			return true;
		}
	case RGF_SM_SETWEAPONMATFROMFLIP:
		{
			// USAGE: SetWeaponMatFromFlip(FLIPBOOK szEntityName, Actor MatIndex, FlipBook Image Index, R, G, B);
			PARMCHECK(6);

			returnValue = false;

			if(!m_WeaponActor)
				return true;

			FlipBook *pEntityData = NULL;

			if(CCD->Level()->FlipBooks()->LocateEntity(arguments[0].str().c_str(), reinterpret_cast<void**>(&pEntityData)) == RGF_SUCCESS)
			{
				int matIndex = arguments[1].intValue();
				int flipIndex = arguments[2].intValue();
				float r = arguments[3].floatValue();
				float g = arguments[4].floatValue();
				float b = arguments[5].floatValue();

				if(geActor_SetMaterial(m_WeaponActor, matIndex, pEntityData->Bitmap[flipIndex], r, g, b))
					returnValue = true; // Set Actor Material
			}

			return true;
		}
	case RGF_SM_SETSHADOWFROMFLIP:
		{
			// USAGE: SetShadowFromFlip(FLIPBOOK szEntityName, ACTOR EntityName);
			PARMCHECK(2);

			FlipBook *pEntityData = NULL;

			if(CCD->Level()->FlipBooks()->LocateEntity(arguments[0].str().c_str(), reinterpret_cast<void**>(&pEntityData)) == RGF_SUCCESS)
			{
				float tm;
				int tb;

				// find frame time of animation and set the proper bitmap
				geActor *wActor = CCD->ActorManager()->GetByEntityName(arguments[1].str().c_str());
				tm = CCD->ActorManager()->GetAnimationTime(wActor)*30.f;
				tb = static_cast<int>(tm - 1.f);

				if((tb > (pEntityData->BitmapCount - 1)) || tb < 0)
					tb = 0;

				CCD->ActorManager()->SetShadowBitmap(wActor, pEntityData->Bitmap[tb]);
				returnValue = pEntityData->BitmapCount;
			}

			return true;
		}
/*	else if (IS_METHOD(methodName, "SaveActorAndSkins")) // PWX
	{
		PARMCHECK(2);
		geVFile* df;
		geActor_Def* aDef,*newDef;
		geBody *newBdy;
		geBitmap *Bitmap;
		geMotion *Anim;
		float R,G,B;

		strcpy(param4, arguments[1].str());

		//tActor = CCD->ActorManager()->GetByEntityName(param4);

		// create new actor def.
		aDef = geActor_GetActorDef(Actor);

		//get the body
		newBdy = geActor_GetBody(aDef);
		//create the new actor ref
		newDef = geActor_DefCreate();

		// loop through the material sand copy them
		int mc = geActor_GetMaterialCount(Actor);
		int i;
		for(i=0; i<mc; ++i)
		{
			geActor_GetMaterial(Actor, i, &Bitmap, &R, &G, &B);
			geBody_SetMaterial(newBdy, i, Bitmap, R, G, B);
		}

		// add the new bdy to the actor
		geActor_SetBody(newDef, newBdy);

		//loop through the motions and copy them to the new actor
		mc = geActor_GetMotionCount(aDef);
		int nmc;
		for(i=0; i<mc-1; ++i)
		{
			Anim = geActor_GetMotionByIndex(aDef, mc);
			if(Anim)
				geActor_AddMotion(newDef, Anim, &nmc);
		}

		// save to the wrestlers folder
		strcpy(param0, "wrestlers\\");
		strcat(param0, arguments[0].str());
		df = geVFile_OpenNewSystem(NULL, GE_VFILE_TYPE_DOS, param0, NULL, GE_VFILE_OPEN_CREATE);

		returnValue = false;
		if(!df)
			return true;
		geActor_DefWriteToFile(newDef,df);
		geVFile_Close(df); //close file
		returnValue = true;

		//destroy everything
		//geBody_Destroy(&newBdy);
		//geMotion_Destroy(&Anim);
		//geBitmap_Destroy(&Bitmap);
		//geActor_DefDestroy(&newDef);

		return true;
	}
*/
	case RGF_SM_GETCOLLIDEDISTANCE:
		{
			// USAGE: GetCollideDistance(BONE_NAME, Offset X, Offset Y, Offset Z);
			PARMCHECK(4);

			geXForm3d Xf;
			bool bone;

			bone = geActor_GetBoneTransform(m_Actor, arguments[0].str().c_str(), &Xf);

			if(!bone)
				return true;

			geVec3d OldPos, Pos, theRotation, Direction;
			GE_Collision Collision;
			CCD->ActorManager()->GetPosition(m_Actor, &OldPos);
			float gd = -1.f;

			geVec3d_Copy(&(Xf.Translation), &OldPos);
			CCD->ActorManager()->GetRotate(m_Actor, &theRotation);

			geXForm3d_SetZRotation(&Xf, theRotation.Z);
			geXForm3d_RotateX(&Xf, theRotation.X);
			geXForm3d_RotateY(&Xf, theRotation.Y);

			geXForm3d_GetUp(&Xf, &Direction);
			geVec3d_AddScaled(&OldPos, &Direction, arguments[2].floatValue(), &Pos);

			geXForm3d_GetLeft(&Xf, &Direction);
			geVec3d_AddScaled(&Pos, &Direction, arguments[1].floatValue(), &Pos);

			geXForm3d_GetIn(&Xf, &Direction);
			geVec3d_AddScaled(&Pos, &Direction, arguments[3].floatValue(), &Pos);

			if(geWorld_Collision(CCD->World(), NULL, NULL, &OldPos, &Pos,
				GE_CONTENTS_SOLID_CLIP | GE_CONTENTS_WINDOW, GE_COLLIDE_ALL,
				0x0, NULL, NULL, &Collision))
			{
				gd = geVec3d_DistanceBetween(&OldPos, &Collision.Impact);
			}

			returnValue = gd;
			return true;
		}
	case RGF_SM_RESETANIMATE:
		{
			PARMCHECK(2);

			geVec3d pos;
			CCD->ActorManager()->GetBonePosition(m_Actor, arguments[1].str().c_str(), &pos);
			CCD->ActorManager()->SetMotion(m_Actor, arguments[0].str().c_str());
			CCD->ActorManager()->SetHoldAtEnd(m_Actor, true);
			CCD->ActorManager()->Position(m_Actor, pos);
			return true;
		}
	case RGF_SM_WHEREISPLAYER:
		{
			geVec3d Pos1, Pos2, temp, Rot;
			geFloat fP, dP;

			CCD->ActorManager()->GetPosition(m_Actor, &Pos1);
			CCD->ActorManager()->GetPosition(CCD->Player()->GetActor(), &Pos2);
			geVec3d_Subtract(&Pos2, &Pos1, &temp);
			geVec3d_Normalize(&temp);

			// Do dotproduct. If it's positive, then Actor2 is in front of Actor1
			CCD->ActorManager()->InVector(m_Actor, &Rot);
			dP = geVec3d_DotProduct(&temp, &Rot);
			CCD->ActorManager()->LeftVector(m_Actor, &Rot);
			fP = geVec3d_DotProduct(&temp, &Rot);

			if(dP > 0.0f)
			{
				returnValue = 0; // Front

				if(dP < 0.5f)
				{
					if(fP > 0.0f)
						returnValue = 3; // Right
					else
						returnValue = 2; // Left
				}
			}
			else
			{
				returnValue = 1; // Back

				if(dP > -0.5f)
				{
					if(fP > 0.0f)
						returnValue = 3; // Left
					else
						returnValue = 2; // Right
				}
			}

			return true;
		}
	case RGF_SM_WHEREISENTITY:
		{
			PARMCHECK(1);

			geVec3d Pos1, Pos2, temp, Rot;
			geFloat fP, dP;

			CCD->ActorManager()->GetPosition(m_Actor, &Pos1);
			CCD->ActorManager()->GetPosition(CCD->ActorManager()->GetByEntityName(arguments[0].str().c_str()), &Pos2);
			geVec3d_Subtract(&Pos2, &Pos1, &temp);
			geVec3d_Normalize(&temp);

			// Do dotproduct.  If it's positive, then Actor2 is in front of Actor1
			CCD->ActorManager()->InVector(m_Actor, &Rot);
			dP = geVec3d_DotProduct(&temp, &Rot);
			CCD->ActorManager()->LeftVector(m_Actor, &Rot);
			fP = geVec3d_DotProduct(&temp, &Rot);

			if(dP > 0.0f)
			{
				returnValue = 0; // Front

				if(dP < 0.5f)
				{
					if(fP > 0.0f)
						returnValue = 3; // Right
					else
						returnValue = 2; // Left
				}
			}
			else
			{
				returnValue = 1; // Back

				if(dP > -0.5f)
				{
					if(fP > 0.0f)
						returnValue = 3; // Left
					else
						returnValue = 2; // Right
				}
			}

			return true;
		}
	case RGF_SM_INSERTEVENT:
		{
			PARMCHECK(3);

			geActor_Def *aDef;
			float tKey;
			tKey = arguments[2].floatValue();
			aDef = geActor_GetActorDef(m_Actor);
			returnValue = false;

			if(geMotion_InsertEvent(geActor_GetMotionByName(aDef, arguments[0].str().c_str()), tKey, arguments[1].str().c_str()))
				returnValue = true;

			return true;
		}
	case RGF_SM_CHECKFOREVENT:
		{
			const char *evnt;
			geMotion *cMot;
			float StartTime, EndTime;

			cMot = geActor_GetMotionByName(geActor_GetActorDef(m_Actor), CCD->ActorManager()->GetMotion(m_Actor).c_str());
			EndTime = CCD->ActorManager()->GetAnimationTime(m_Actor, ANIMATION_CHANNEL_ROOT);
			StartTime = EndTime - CCD->GetTicksGoneBy() * 0.001f;

			if(StartTime < 0.0f)
				StartTime = 0.0f;

			geMotion_SetupEventIterator(cMot, StartTime, EndTime);

			if(geMotion_GetNextEvent(cMot, &EndTime, &evnt) == GE_TRUE)
				returnValue = skString(evnt);
			else
				returnValue = false;

			return true;
		}
	case RGF_SM_PLAYEVENTSOUND:
		{
			// USAGE: PlayEventSound(Entity, Volume)

			PARMCHECK(2);

			const char *evnt;
			geMotion *cMot;
			float StartTime, EndTime;
			float volume;
			volume = arguments[1].floatValue();

			geActor *wActor = CCD->ActorManager()->GetByEntityName(arguments[0].str().c_str());

			cMot = geActor_GetMotionByName(geActor_GetActorDef(wActor), CCD->ActorManager()->GetMotion(wActor).c_str());
			EndTime = CCD->ActorManager()->GetAnimationTime(wActor, ANIMATION_CHANNEL_ROOT);
			StartTime = EndTime - (CCD->GetTicksGoneBy() * 0.001f);

			if(StartTime < 0.0f)
				StartTime = 0.0f;

			geMotion_SetupEventIterator(cMot, StartTime, EndTime);

			if(geMotion_GetNextEvent(cMot, &EndTime, &evnt) == GE_TRUE)
			{
				Snd	Sound;
				memset(&Sound, 0, sizeof(Sound));
				Sound.Min = volume;
				Sound.Loop = GE_FALSE;
				strcpy(param0, evnt);
				Sound.SoundDef = SPool_Sound(param0);

				if(Sound.SoundDef != NULL)
					CCD->EffectManager()->Item_Add(EFF_SND, static_cast<void*>(&Sound));
			}

			return true;
		}
/*	else if (IS_METHOD(methodName, "LoadSkin"))
	{
		// Usage: LoadSkin(Bitmap File Name in 'skins' Folder, Pawn Mat Index, R, G, B)
		PARMCHECK(5);
		geVFile* file;
		geBitmap* Map;
		float Red,Green,Blue;
		int MatIndex;

		// Get Parameters
		MatIndex = arguments[1].intValue();
		Red = arguments[2].floatValue();
		Green = arguments[3].floatValue();
		Blue = arguments[4].floatValue();

		// Build File Name
		strcpy(param0, arguments[0].str());

		//Open File
		CFileManager::GetSingletonPtr()->OpenRFFile(&file, kBitmapFile, param0, GE_VFILE_OPEN_READONLY);
		returnValue = false;
		if(file)
		{
			Map = geBitmap_CreateFromFile(file);	// Load Bitmap
			if(!Map)
				returnValue = false;
			geActor_SetMaterial(Actor, MatIndex, Map, Red, Green, Blue); // Set Actor Material
			geBitmap_Destroy(&Map);
			geVFile_Close(file);//close file
			returnValue = true;
		}

		return true;
	}
*/
	case RGF_SM_LOADANIMATION:
		{
			PARMCHECK(1);

			int ind = 0;

			geVFile* file = geVFile_OpenNewSystem(NULL, GE_VFILE_TYPE_DOS, arguments[0].str().c_str(), NULL, GE_VFILE_OPEN_READONLY);

			if(file != NULL)
			{
				geMotion *cMot = geMotion_CreateFromFile(file);

				if(cMot != NULL)
					geActor_AddMotion(geActor_GetActorDef(m_Actor), cMot, &ind);

				geVFile_Close(file); //close file
			}

			returnValue = ind;
			return true;
		}
/*	else if (IS_METHOD(methodName, "SaveAnimations"))
	{
		geVFile* df;
		geActor_Def* aDef;
		int Mc;
		int nM;

		aDef = geActor_GetActorDef(Actor);
		nM = geActor_GetMotionCount(aDef);

		for(Mc = 0; Mc < nM; ++Mc)
		{
			strcpy(param0, "motions\\");
			strcat(param0, geActor_GetMotionName(aDef, Mc));
			df = geVFile_OpenNewSystem(NULL, GE_VFILE_TYPE_DOS, param0, NULL,GE_VFILE_OPEN_CREATE);
			geMotion_WriteToBinaryFile(geActor_GetMotionByIndex(aDef, Mc), df);
			geVFile_Close(df); //close file
		}
		returnValue = skString(param0);
		return true;
	}
*/
	case RGF_SM_STARTSOUNDTRACK:
		{
			PARMCHECK(1);
			CCD->Level()->AudioStreams()->Play(arguments[0].str().c_str(), true, true);
			return true;
		}
	case RGF_SM_STOPALLAUDIOSTREAMS:
		{
			CCD->Level()->AudioStreams()->StopAll();
			return true;
		}
	case RGF_SM_CHANGEYAW:
		{
			ChangeYaw();
			return true;
		}
	case RGF_SM_CHANGEPITCH:
		{
			ChangePitch();
			return true;
		}
	case RGF_SM_RANDOM:
		{
			// deprecated - use Math.rand
			PARMCHECK(2);
			int param2 = arguments[0].intValue();
			int param3 = arguments[1].intValue();

			if(param2 < param3)
				returnValue = EffectC_rand(param2, param3);
			else
				returnValue = EffectC_rand(param3, param2);

			return true;
		}
	case RGF_SM_WALKMOVE:
		{
			PARMCHECK(2);

			float amount = arguments[1].floatValue() * m_ElapseTime;
			geXForm3d Xform;
			geVec3d In, NewPosition, SavedPosition;
			CCD->ActorManager()->GetPosition(m_Actor, &SavedPosition);

			geXForm3d_SetYRotation(&Xform, arguments[0].floatValue());

			geXForm3d_GetIn(&Xform, &In);
			geVec3d_AddScaled(&SavedPosition, &In, amount, &NewPosition);
			returnValue = false;

			if(CCD->ActorManager()->ValidateMove(SavedPosition, NewPosition, m_Actor, false) == GE_TRUE)
				returnValue = true;

			return true;
		}
	case RGF_SM_FLYMOVE:
		{
			PARMCHECK(3);

			float amount = arguments[2].floatValue() * m_ElapseTime;
			geXForm3d Xform;
			geVec3d In, NewPosition, SavedPosition;
			CCD->ActorManager()->GetPosition(m_Actor, &SavedPosition);

			geXForm3d_SetXRotation(&Xform, -arguments[0].floatValue());
			geXForm3d_RotateY(&Xform, arguments[1].floatValue());

			geXForm3d_GetIn(&Xform, &In);
			geVec3d_AddScaled(&SavedPosition, &In, amount, &NewPosition);
			returnValue = false;

			if(CCD->ActorManager()->ValidateMove(SavedPosition, NewPosition, m_Actor, false) == GE_TRUE)
				returnValue = true;

			return true;
		}
	case RGF_SM_DAMAGE:
		{
			PARMCHECK(2);

			float amount = arguments[0].floatValue();
			strcpy(param0, arguments[1].str());
			CCD->Level()->Damage()->DamageActor(m_TargetActor, amount, param0, amount, param0, "Melee");
			return true;
		}
	case RGF_SM_DAMAGEPLAYER:
		{
			PARMCHECK(2);

			float amount = arguments[0].floatValue();
			strcpy(param0, arguments[1].str());
			CCD->Level()->Damage()->DamageActor(CCD->Player()->GetActor(), amount, param0, amount, param0, "Melee");
			return true;
		}
	case RGF_SM_POSITIONTOPLAYER:
		{
			PARMCHECK(3);

			geXForm3d Xf;
			geVec3d Pos, theRotation, Direction;

			Pos = CCD->Player()->Position();

			if(arguments.entries() > 3)
			{
				CCD->ActorManager()->GetRotate(CCD->Player()->GetActor(), &theRotation);

				if(arguments[3].boolValue())
				{
					geXForm3d_SetZRotation(&Xf, theRotation.Z);
					geXForm3d_RotateX(&Xf, theRotation.X);
					geXForm3d_RotateY(&Xf, theRotation.Y);

					geXForm3d_GetUp(&Xf, &Direction);
					geVec3d_AddScaled(&Pos, &Direction, arguments[1].floatValue(), &Pos);

					geXForm3d_GetLeft(&Xf, &Direction);
					geVec3d_AddScaled(&Pos, &Direction, arguments[0].floatValue(), &Pos);

					geXForm3d_GetIn(&Xf, &Direction);
					geVec3d_AddScaled(&Pos, &Direction, arguments[2].floatValue(), &Pos);

				}
				else
				{
					Pos.X += arguments[0].floatValue();
					Pos.Y += arguments[1].floatValue();
					Pos.Z += arguments[2].floatValue();
				}

				CCD->ActorManager()->Position(m_Actor, Pos);

				if(m_WeaponActor)
					CCD->ActorManager()->Position(m_WeaponActor, Pos);

				if(arguments.entries() > 4)
				{
					if(arguments[4].boolValue())
					{
						CCD->ActorManager()->Rotate(m_Actor, theRotation);

						if(m_WeaponActor)
							CCD->ActorManager()->Rotate(m_WeaponActor, theRotation);
					}
				}

				return true;
			}

			Pos.X += arguments[0].floatValue();
			Pos.Y += arguments[1].floatValue();
			Pos.Z += arguments[2].floatValue();
			CCD->ActorManager()->Position(m_Actor, Pos);

			if(m_WeaponActor)
				CCD->ActorManager()->Position(m_WeaponActor, Pos);

			return true;
		}
	case RGF_SM_PLAYERTOPOSITION:
		{
			PARMCHECK(3);
			geXForm3d Xf;
			geVec3d Pos, theRotation, Direction;

			CCD->ActorManager()->GetPosition(m_Actor, &Pos);

			if(arguments.entries() > 3)
			{
				CCD->ActorManager()->GetRotate(m_Actor, &theRotation);

				if(arguments[3].boolValue())
				{
					geXForm3d_SetZRotation(&Xf, theRotation.Z);
					geXForm3d_RotateX(&Xf, theRotation.X);
					geXForm3d_RotateY(&Xf, theRotation.Y);

					geXForm3d_GetUp(&Xf, &Direction);
					geVec3d_AddScaled(&Pos, &Direction, arguments[1].floatValue(), &Pos);

					geXForm3d_GetLeft(&Xf, &Direction);
					geVec3d_AddScaled(&Pos, &Direction, arguments[0].floatValue(), &Pos);

					geXForm3d_GetIn(&Xf, &Direction);
					geVec3d_AddScaled(&Pos, &Direction, arguments[2].floatValue(), &Pos);
				}
				else
				{
					Pos.X += arguments[0].floatValue();
					Pos.Y += arguments[1].floatValue();
					Pos.Z += arguments[2].floatValue();
				}

				CCD->ActorManager()->Position(CCD->Player()->GetActor(), Pos);

				if(arguments.entries() > 4)
				{
					if(arguments[4].boolValue())
						CCD->ActorManager()->Rotate(CCD->Player()->GetActor(), theRotation);
				}

				return true;
			}

			Pos.X += arguments[0].floatValue();
			Pos.Y += arguments[1].floatValue();
			Pos.Z += arguments[2].floatValue();
			CCD->ActorManager()->Position(CCD->Player()->GetActor(), Pos);

			return true;
		}
	case RGF_SM_POSITIONTOPAWN:
		{
			PARMCHECK(4);
			geActor *MasterActor;

			MasterActor = CCD->ActorManager()->GetByEntityName(arguments[0].str().c_str());

			if(!MasterActor)
				return true;

			geVec3d Pos;
			CCD->ActorManager()->GetPosition(MasterActor, &Pos);

			if(arguments.entries() > 4)
			{
				geVec3d theRotation;

				CCD->ActorManager()->GetRotate(MasterActor, &theRotation);

				if(arguments[4].boolValue())
				{
					geXForm3d Xf;
					geVec3d OldPos, Direction;

					OldPos = Pos;
					OldPos.Y += 16.f;

					geXForm3d_SetZRotation(&Xf, theRotation.Z);
					geXForm3d_RotateX(&Xf, theRotation.X);
					geXForm3d_RotateY(&Xf, theRotation.Y);

					geXForm3d_GetUp(&Xf, &Direction);
					geVec3d_AddScaled(&Pos, &Direction, arguments[2].floatValue(), &Pos);

					geXForm3d_GetLeft(&Xf, &Direction);
					geVec3d_AddScaled(&Pos, &Direction, arguments[1].floatValue(), &Pos);

					geXForm3d_GetIn(&Xf, &Direction);
					geVec3d_AddScaled(&Pos, &Direction, arguments[3].floatValue(), &Pos);

					GE_Collision Collision;
					geBoolean Result = geWorld_Collision(CCD->Engine()->World(), NULL, NULL, &OldPos, &Pos,
										GE_CONTENTS_SOLID_CLIP , GE_COLLIDE_ALL, 0,
										NULL, NULL, &Collision);

					if(Result)
					{
						Pos = Collision.Impact;
						geVec3d_AddScaled(&Pos, &(Collision.Plane.Normal), 2.0f, &Pos);
					}
				}
				else
				{
					Pos.X += arguments[1].floatValue();
					Pos.Y += arguments[2].floatValue();
					Pos.Z += arguments[3].floatValue();
				}

				CCD->ActorManager()->Position(m_Actor, Pos);

				if(m_WeaponActor)
					CCD->ActorManager()->Position(m_WeaponActor, Pos);

				if(arguments.entries() > 5)
				{
					if(arguments[5].boolValue())
					{
						CCD->ActorManager()->Rotate(m_Actor, theRotation);

						if(m_WeaponActor)
							CCD->ActorManager()->Rotate(m_WeaponActor, theRotation);
					}
				}

				return true;
			}

			Pos.X += arguments[1].floatValue();
			Pos.Y += arguments[2].floatValue();
			Pos.Z += arguments[3].floatValue();
			CCD->ActorManager()->Position(m_Actor, Pos);

			if(m_WeaponActor)
				CCD->ActorManager()->Position(m_WeaponActor, Pos);

			return true;
		}
	case RGF_SM_SETKEYPAUSE:
		{
			// deprecated - use Input.SetEnabled
			PARMCHECK(1);
			CCD->SetKeyPaused(arguments[0].boolValue());
			return true;
		}
	case RGF_SM_PLAYERRENDER:
		{
			// deprecated - use Player.Render
			PARMCHECK(1);

			if(arguments[0].boolValue())
			{
				CCD->ActorManager()->SetCollide(CCD->Player()->GetActor());
				CCD->Weapons()->Rendering(true);
			}
			else
			{
				CCD->ActorManager()->SetNoCollide(CCD->Player()->GetActor());
				CCD->ActorManager()->SetActorFlags(CCD->Player()->GetActor(), 0);
				CCD->Weapons()->Rendering(false);
			}

			return true;
		}
	case RGF_SM_PAWNRENDER: // deprecated
	case RGF_SM_RENDER:
		{
			PARMCHECK(1);

			if(arguments[0].boolValue())
			{
				CCD->ActorManager()->SetCollide(m_Actor);

				if(m_WeaponActor)
				{
					CCD->ActorManager()->SetCollide(m_WeaponActor);
					CCD->ActorManager()->SetNoCollide(m_WeaponActor);
				}
			}
			else
			{
				CCD->ActorManager()->SetNoCollide(m_Actor);
				CCD->ActorManager()->SetActorFlags(m_Actor, 0);

				if(m_WeaponActor)
				{
					CCD->ActorManager()->SetActorFlags(m_WeaponActor, 0);
				}
			}

			return true;
		}
	case RGF_SM_SETHOLDATEND:
		{
			PARMCHECK(1);
			CCD->ActorManager()->SetHoldAtEnd(m_Actor, arguments[0].boolValue());
			return true;
		}
	case RGF_SM_FORCEUP:
		{
			PARMCHECK(1);
			float amount = arguments[0].floatValue();
			geVec3d theUp;
			CCD->ActorManager()->UpVector(m_Actor, &theUp);
			CCD->ActorManager()->SetForce(m_Actor, 0, theUp, amount, amount);
			return true;
		}
	case RGF_SM_FORCEDOWN:
		{
			PARMCHECK(1);
			float amount = arguments[0].floatValue();
			geVec3d theUp;
			CCD->ActorManager()->UpVector(m_Actor, &theUp);
			geVec3d_Inverse(&theUp);
			CCD->ActorManager()->SetForce(m_Actor, 0, theUp, amount, amount);
			return true;
		}
	case RGF_SM_FORCERIGHT:
		{
			PARMCHECK(1);
			float amount = arguments[0].floatValue();
			geVec3d theRight;
			CCD->ActorManager()->LeftVector(m_Actor, &theRight);
			geVec3d_Inverse(&theRight);
			CCD->ActorManager()->SetForce(m_Actor, 1, theRight, amount, amount);
			return true;
		}
	case RGF_SM_FORCELEFT:
		{
			PARMCHECK(1);
			float amount = arguments[0].floatValue();
			geVec3d theLeft;
			CCD->ActorManager()->LeftVector(m_Actor, &theLeft);
			CCD->ActorManager()->SetForce(m_Actor, 1, theLeft, amount, amount);
			return true;
		}
	case RGF_SM_FORCEFORWARD:
		{
			PARMCHECK(1);
			float amount = arguments[0].floatValue();
			geVec3d theIn;
			CCD->ActorManager()->InVector(m_Actor, &theIn);
			CCD->ActorManager()->SetForce(m_Actor, 2, theIn, amount, amount);
			return true;
		}
	case RGF_SM_FORCEBACKWARD:
		{
			PARMCHECK(1);
			float amount = arguments[0].floatValue();
			geVec3d theBack;
			CCD->ActorManager()->InVector(m_Actor, &theBack);
			geVec3d_Inverse(&theBack);
			CCD->ActorManager()->SetForce(m_Actor, 2, theBack, amount, amount);
			return true;
		}
/*	else if (IS_METHOD(methodName, "movetogoal"))
	{
		float amount = arguments[0].floatValue();
		MoveToGoal(amount);
		return true;
	}
*/
	case RGF_SM_UPDATETARGET:
		{
			m_UpdateTargetPoint = m_LastTargetPoint;
			return true;
		}
	case RGF_SM_FIREPROJECTILE:
		{
			PARMCHECK(6);
			geXForm3d Xf;
			geVec3d theRotation, Pos, Direction, Orient, TargetPoint;
			geFloat x;
			geExtBox theBox;

			TargetPoint = m_UpdateTargetPoint;

			if(CCD->ActorManager()->GetBoundingBox(m_TargetActor, &theBox) != RGF_NOT_FOUND)
			{
				if(arguments.entries() == 7)
				{
					float height = arguments[6].floatValue() - 0.5f;
					TargetPoint.Y += (theBox.Max.Y*height);
				}
			}

			m_DamageAttr = arguments[5].str().c_str();

			float FireOffsetX = arguments[2].floatValue();
			float FireOffsetY = arguments[3].floatValue();
			float FireOffsetZ = arguments[4].floatValue();

			bool bone;

			if(m_WeaponActor)
				bone = geActor_GetBoneTransform(m_WeaponActor, arguments[1].str().c_str(), &Xf);
			else
				bone = geActor_GetBoneTransform(m_Actor, arguments[1].str().c_str(), &Xf);

			if(bone)
			{
				geVec3d_Copy(&Xf.Translation, &Pos);
				CCD->ActorManager()->GetRotate(m_Actor, &theRotation);

				geXForm3d_SetZRotation(&Xf, theRotation.Z);
				geXForm3d_RotateX(&Xf, theRotation.X);
				geXForm3d_RotateY(&Xf, theRotation.Y);

				geXForm3d_GetUp(&Xf, &Direction);
				geVec3d_AddScaled(&Pos, &Direction, FireOffsetY, &Pos);

				geXForm3d_GetLeft(&Xf, &Direction);
				geVec3d_AddScaled(&Pos, &Direction, FireOffsetX, &Pos);

				geXForm3d_GetIn(&Xf, &Direction);
				geVec3d_AddScaled(&Pos, &Direction, FireOffsetZ, &Pos);

				geVec3d_Subtract(&TargetPoint, &Pos, &Orient);
				float l = geVec3d_Length(&Orient);

				if(l > 0.0f)
				{
					x = Orient.X;
					Orient.X = GE_PIOVER2 - acos(Orient.Y / l);
					Orient.Y = atan2(x, Orient.Z) + GE_PI;
					Orient.Z = 0.0f;	// roll is zero - always!!?

					CCD->Weapons()->AddProjectile(Pos, Pos, Orient, arguments[0].str().c_str(), m_DamageAttr, m_DamageAttr);
				}
			}

			return true;
		}
	case RGF_SM_ADDEXPLOSION:
		{
			PARMCHECK(5);
			geVec3d Pos;

			strcpy(param4, arguments[1].str());

			if(geActor_DefHasBoneNamed(geActor_GetActorDef(m_Actor), param4))
			{
				Pos.X = arguments[2].floatValue();
				Pos.Y = arguments[3].floatValue();
				Pos.Z = arguments[4].floatValue();

				CCD->Level()->ExplosionManager()->AddExplosion(arguments[0].str().c_str(), Pos, m_Actor, param4);
			}

			return true;
		}
/*	else if (IS_METHOD(methodName, "PortIn"))
	{
		PARMCHECK(1);
		int port = arguments[0].intValue();
		returnValue =  (int)_inp(port);
		return true;
	}
	else if (IS_METHOD(methodName, "PortOut"))
	{
		PARMCHECK(2);
		int port = arguments[0].intValue();
		int data = arguments[1].intValue();
		_outp(port, data);
		return true;
	}
*/
	case RGF_SM_GETLASTBONEHIT:
		{
			returnValue = skString(CCD->ActorManager()->GetLastBoneHit(m_Actor).c_str());
			return true;
		}
	case RGF_SM_DEBUG:
		{
			PARMCHECK(1);

			if(m_Console)
			{
				int index = -1;
				int i;

				for(i=0; i<DEBUGLINES; ++i)
				{
					if(EffectC_IsStringNull(m_ConsoleDebug[i]))
					{
						index = i;
						break;
					}
				}

				if(index != -1)
				{
					strcpy(m_ConsoleDebug[index], arguments[0].str());
				}
				else
				{
					for(i=1; i<DEBUGLINES; ++i)
					{
						strcpy(m_ConsoleDebug[i-1], m_ConsoleDebug[i]);
					}

					strcpy(m_ConsoleDebug[DEBUGLINES-1], arguments[0].str());
				}
			}

			return true;
		}
	case RGF_SM_SETEVENTSTATE:
		{
			PARMCHECK(2);
			CCD->Level()->Pawns()->AddEvent(arguments[0].str().c_str(), arguments[1].boolValue());
			return true;
		}
	case RGF_SM_GETSTRINGLENGTH:
		{
			///< @deprecated
			PARMCHECK(1);
			returnValue = static_cast<int>(arguments[0].str().length());
			return true;
		}
	case RGF_SM_DRAWTEXT:
		{
			///< @deprecated Use CEGUI functions instead: GUI.FontManager.getFont(fontname).drawText(text, draw_area, z);
			PARMCHECK(3);
			CEGUI::Rect draw_area(	arguments[1].floatValue(),
									arguments[2].floatValue(),
									static_cast<float>(CCD->Engine()->Width()),
									static_cast<float>(CCD->Engine()->Height()));
			CCD->GUIManager()->DrawText(arguments[0].str().c_str(), arguments[3].str().c_str(), draw_area, 1.f);
			return true;
		}
/*	case RGF_SM_DRAWFLIPBOOKIMAGE:
		{
			PARMCHECK(9);
			FlipBook *pEntityData = NULL;

			if(CCD->Level()->FlipBooks()->LocateEntity(arguments[0].str().c_str(), reinterpret_cast<void**>(&pEntityData)) == RGF_SUCCESS)
			{
				if(pEntityData->BitmapCount > arguments[1].intValue())
				{
					CCD->PWXImMgr()->AddImage(	NULL, pEntityData->Bitmap[arguments[1].intValue()],
												arguments[2].intValue(), arguments[3].intValue(),
												arguments[4].floatValue(),
												arguments[5].intValue(), arguments[6].intValue(), arguments[7].intValue(),
												0, arguments[8].floatValue());
				}
			}

			return true;
		}*/
	case RGF_SM_DRAWPOLYSHADOW:
		{
			CCD->PlyShdw()->DrawShadow(m_Actor);
			return true;
		}
	case RGF_SM_MATCHPLAYERANGLES:
		{
			geVec3d Rot;
			CCD->ActorManager()->GetRotate(CCD->Player()->GetActor(), &Rot);
			CCD->ActorManager()->Rotate(m_Actor, Rot);
			return true;
		}
	case RGF_SM_DAMAGEATBONE:
		{
			// USAGE: DamageAtBone(damage amount, radius, attribute, bone name)
			PARMCHECK(4);

			geVec3d Pos;
			CCD->ActorManager()->GetBonePosition(m_Actor, arguments[3].str().c_str(), &Pos);
			CCD->Level()->Damage()->DamageActorInRange(	Pos, arguments[1].floatValue(), arguments[0].floatValue(),
														arguments[2].str().c_str(), 0.0f, "", "");
			return true;
		}
	case RGF_SM_SAVEACTOR:
		{
			// USAGE: SaveActor(Actor Name)
			PARMCHECK(1);

			geVFile *df;
			geActor_Def *aDef;
			aDef = geActor_GetActorDef(m_Actor);
			strcpy(param0, "savedactors\\");
			strcat(param0, arguments[0].str());
			df = geVFile_OpenNewSystem(NULL, GE_VFILE_TYPE_DOS, param0, NULL, GE_VFILE_OPEN_CREATE);
			returnValue = false;

			if(!df)
				return true;

			geActor_DefWriteToFile(aDef, df);
			geVFile_Close(df);	// close file
			returnValue = true;
			return true;
		}
	case RGF_SM_LOOKATPAWN:
		{
			// USAGE: LookAtPawn(Pawn Name, Offset)
			PARMCHECK(2);
			geVec3d Pos1 ,Pos2, Rot;

			CCD->ActorManager()->GetPosition(m_Actor, &Pos1);
			CCD->ActorManager()->GetPosition(CCD->ActorManager()->GetByEntityName(arguments[0].str().c_str()), &Pos2);
			Pos2.Y += arguments[1].floatValue();	// add height offset to target point
			CCD->Level()->PathFollower()->GetRotationToFacePoint(Pos1, Pos2, &Rot);
			CCD->ActorManager()->Rotate(m_Actor, Rot);
			return true;
		}
	case RGF_SM_AUTOWALK:
		{
			PARMCHECK(3);

			float amount = arguments[1].floatValue() * m_ElapseTime;
			float distance = arguments[2].floatValue(); // distance to keep from walls
			geXForm3d Xform;
			geVec3d In, NewPosition, SavedPosition, TestPos;

			CCD->ActorManager()->GetPosition(m_Actor, &SavedPosition);

			geXForm3d_SetYRotation(&Xform, arguments[0].floatValue());

			geXForm3d_GetIn(&Xform, &In);
			geVec3d_AddScaled(&SavedPosition, &In, amount, &NewPosition);
			returnValue = false;

			GE_Collision Collision;
			geVec3d_AddScaled(&NewPosition, &In, distance, &TestPos);
			//Get ground plane and distance
			geBoolean Result = geWorld_Collision(CCD->Engine()->World(), NULL, NULL, &SavedPosition, &TestPos,
				GE_CONTENTS_SOLID_CLIP , GE_COLLIDE_MESHES | GE_COLLIDE_MODELS, 0, NULL, NULL, &Collision);

			if(Result)
			{
				geVec3d_AddScaled(&NewPosition, &Collision.Plane.Normal, amount, &NewPosition);
			}

			if(CCD->ActorManager()->ValidateMove(SavedPosition, NewPosition, m_Actor, false) == GE_TRUE)
				returnValue = true;

			return true;
		}
	case RGF_SM_FASTDISTANCE:
		{
			// USAGE: BOOL = FastDistance(EntityName,distance);
			PARMCHECK(2);

			float dist = arguments[1].floatValue();
			geVec3d Pos1, Pos2, dV;
			CCD->ActorManager()->GetPosition(m_Actor, &Pos1);
			CCD->ActorManager()->GetPosition(CCD->ActorManager()->GetByEntityName(arguments[0].str().c_str()), &Pos2);
			geVec3d_Subtract(&Pos1, &Pos2, &dV);

			returnValue = false;

			if(geVec3d_LengthSquared(&dV) < dist * dist)
				returnValue = true;

			return true;
		}
	case RGF_SM_STEPHEIGHT:
		{
			// USAGE: StepHeight(float)
			PARMCHECK(1);

			CCD->ActorManager()->SetStepHeight(m_Actor, arguments[0].floatValue());
			return true;
		}
	case RGF_SM_DRAWVPOLY:
	case RGF_SM_DRAWHPOLY:
		{
			return true;
		}
	case RGF_SM_GETPITCHTOPOINT:
		{
			geVec3d Pos1, Rot;
			CCD->ActorManager()->GetPosition(m_Actor, &Pos1);
			CCD->Level()->PathFollower()->GetRotationToFacePoint(Pos1, m_CurrentPoint, &Rot);
			returnValue = Rot.X;
			return true;
		}
	case RGF_SM_GETYAWTOPOINT:
		{
			geVec3d Pos1, Rot;
			CCD->ActorManager()->GetPosition(m_Actor, &Pos1);
			CCD->Level()->PathFollower()->GetRotationToFacePoint(Pos1, m_CurrentPoint, &Rot);
			returnValue = Rot.Y;
			return true;
		}
	case RGF_SM_FASTPOINTCHECK:
		{
			// USAGE: BOOL = FastPointCheck(distance);
			PARMCHECK(1);

			float dist = arguments[0].floatValue();
			geVec3d Pos1, dV;
			CCD->ActorManager()->GetPosition(m_Actor, &Pos1);
			geVec3d_Subtract(&Pos1, &m_CurrentPoint, &dV);

			returnValue = false;

			if(geVec3d_LengthSquared(&dV) < dist * dist)
				returnValue = true;

			return true;
		}
	case RGF_SM_SETCAMERAWINDOW:
		{
			// USAGE: SetCameraWindow(float FOV)
			// SetCameraWindow(float FOV, bool UseWidhtHeight, Left, Top, Width/Right, Height/Bottom);
			// Sets the size + zoom factor of the real engine camera

			PARMCHECK(1);

			if(arguments.entries() > 5)
			{
				geRect Rect;
				Rect.Left = arguments[2].intValue();
				Rect.Top = arguments[3].intValue();

				if(arguments[1].boolValue())
				{
					Rect.Right = Rect.Left + arguments[4].intValue();
					Rect.Bottom = Rect.Top + arguments[5].intValue();
				}
				else
				{
					Rect.Right = arguments[4].intValue();
					Rect.Bottom = arguments[5].intValue();
				}

				geCamera_SetAttributes(CCD->CameraManager()->Camera(), arguments[0].floatValue(), &Rect);
			}
			else
			{
				CCD->CameraManager()->SetFOV(arguments[0].floatValue());
			}

			return true;
		}
	case RGF_SM_SETFIXEDCAMERAPOSITION:
		{
			// USAGE: SetFixedCameraPosition(PosX, PosY, PosZ)
			PARMCHECK(3);

			geVec3d Pos;
			Pos.X = arguments[0].floatValue();
			Pos.Y = arguments[1].floatValue();
			Pos.Z = arguments[2].floatValue();
			CCD->Level()->FixedCameras()->SetPosition(Pos);

			return true;
		}
	case RGF_SM_SETFIXEDCAMERAROTATION:
		{
			// USAGE: SetFixedCameraRotation(RotX, RotY, RotZ)
			PARMCHECK(3);

			geVec3d Rot;
			Rot.X = GE_PIOVER180*arguments[0].floatValue();
			Rot.Y = GE_PIOVER180*arguments[1].floatValue();
			Rot.Z = GE_PIOVER180*arguments[2].floatValue();
			CCD->Level()->FixedCameras()->SetRotation(Rot);

			return true;
		}
	case RGF_SM_SETFIXEDCAMERAFOV:
		{
			// USAGE: SetFixedCameraFOV(float FOV)
			PARMCHECK(1);

			CCD->Level()->FixedCameras()->SetFOV(arguments[0].floatValue());

			return true;
		}
	case RGF_SM_MOVEFIXEDCAMERA:
		{
			// USAGE: MoveFixedCamera(PosX, PosY, PosZ)
			PARMCHECK(3);

			geVec3d Pos;
			Pos.X = arguments[0].floatValue();
			Pos.Y = arguments[1].floatValue();
			Pos.Z = arguments[2].floatValue();
			CCD->Level()->FixedCameras()->Move(Pos);

			return true;
		}
	case RGF_SM_ROTATEFIXEDCAMERA:
		{
			// USAGE: RotateFixedCamera(RotX, RotY, RotZ)
			PARMCHECK(3);

			geVec3d Rot;
			Rot.X = GE_PIOVER180 * arguments[0].floatValue();
			Rot.Y = GE_PIOVER180 * arguments[1].floatValue();
			Rot.Z = GE_PIOVER180 * arguments[2].floatValue();
			CCD->Level()->FixedCameras()->Rotate(Rot);

			return true;
		}
	case RGF_SM_DISTANCEBETWEENENTITIES:
		{
			// USAGE: DistanceBetweenEntities(FromEntity, ToEntity);
			// USAGE: DistanceBetweenEntities(FromEntity, ToEntity, OnlyXZ);
			// "Player" is a special Entity
			PARMCHECK(2);
			strcpy(param0, arguments[0].str());
			strcpy(param4, arguments[1].str());
			geVec3d Pos, tPos;

			if(!stricmp(param0, "Player"))
			{
				Pos = CCD->Player()->Position();
			}
			else
			{
				if(CCD->ActorManager()->GetPosition(CCD->ActorManager()->GetByEntityName(param0), &Pos) != RGF_SUCCESS)
				{
					returnValue = 0.f;
					return true;
				}
			}

			if(!stricmp(param4, "Player"))
			{
				tPos = CCD->Player()->Position();
			}
			else
			{
				if(CCD->ActorManager()->GetPosition(CCD->ActorManager()->GetByEntityName(param4), &tPos) != RGF_SUCCESS)
				{
					returnValue = 0.f;
					return true;
				}
			}

			if(arguments.entries() > 2)
				if(arguments[2].boolValue())
					Pos.Y = tPos.Y;

			returnValue = geVec3d_DistanceBetween(&tPos, &Pos);

			return true;
		}
	case RGF_SM_SETENTITYPROPERTIES:
		{
			// USAGE:
			// SetEntityProperties(EntityName, dPosX, dPosY, dPosZ)
			// SetEntityProperties(EntityName, dPosX, dPosY, dPosZ, Scale)
			// SetEntityProperties(EntityName, dPosX, dPosY, dPosZ, Scale, Alpha)
			// SetEntityProperties(EntityName, dPosX, dPosY, dPosZ, Scale, Alpha,
			//						FillColX, FillColY, FillColZ)
			// SetEntityProperties(EntityName, dPosX, dPosY, dPosZ, Scale, Alpha,
			//						FillColX, FillColY, FillColZ, RotX, RotY, RotZ)

			PARMCHECK(4);

			GE_RGBA	Color;

			strcpy(param0, arguments[0].str());

			if(arguments[0].str() != "")
			{
				geActor *pActor = NULL;

				if(!stricmp(param0, "Player"))
					pActor = CCD->Player()->GetActor();
				else
					pActor = CCD->ActorManager()->GetByEntityName(param0);

				geVec3d Pos;

				CCD->ActorManager()->GetPosition(pActor, &Pos);

				Pos.X += arguments[1].floatValue();
				Pos.Y += arguments[2].floatValue();
				Pos.Z += arguments[3].floatValue();

				CCD->ActorManager()->Position(pActor, Pos);

				if(arguments.entries() > 4)
					CCD->ActorManager()->SetScale(pActor, arguments[4].floatValue());

				if(arguments.entries() > 5)
					CCD->ActorManager()->SetAlpha(pActor, arguments[5].floatValue());

				if(arguments.entries() > 8)
				{
					Color.r = arguments[6].floatValue();
					Color.g = arguments[7].floatValue();
					Color.b = arguments[8].floatValue();
					CCD->ActorManager()->SetActorDynamicLighting(pActor, Color, Color, GE_FALSE);
				}

				if(arguments.entries() > 11)
				{
					geVec3d Rot;
					Rot.X = GE_PIOVER180*arguments[9].floatValue();
					Rot.Y = GE_PIOVER180*arguments[10].floatValue();
					Rot.Z = GE_PIOVER180*arguments[11].floatValue();
					CCD->ActorManager()->Rotate(pActor, Rot);
				}
			}

			return true;
		}
	case RGF_SM_SETENTITYLIGHTING:
		{
			// USAGE: SetEntityLighting(EntityName, FillR, FillG, FillB,
			//							AmbientR, AmbientG, AmbientB, AmbientFromFloor)

			PARMCHECK(8);

			if(arguments[0].str() != "")
			{
			    strcpy(param0, arguments[0].str());

				geActor *pActor;

				if(!stricmp(param0, "Player"))
					pActor = CCD->Player()->GetActor();
				else
					pActor = CCD->ActorManager()->GetByEntityName(param0);

				GE_RGBA FillColor;
				GE_RGBA AmbientColor;

				FillColor.a = AmbientColor.a = 255.f;

				FillColor.r = arguments[1].floatValue();
				FillColor.g = arguments[2].floatValue();
				FillColor.b = arguments[3].floatValue();

				AmbientColor.r = arguments[4].floatValue();
				AmbientColor.g = arguments[5].floatValue();
				AmbientColor.b = arguments[6].floatValue();

				CCD->ActorManager()->SetActorDynamicLighting(pActor, FillColor, AmbientColor, arguments[7].boolValue() ? GE_TRUE : GE_FALSE);
			}

			return true;
		}
	case RGF_SM_UPDATESCRIPTPOINT:
		{
			// USAGE:
			// UpdateScriptPoint(PointName, EntityName, OffsetX, OffsetY, OffsetZ)
			// UpdateScriptPoint(PointName, EntityName, OffsetX, OffsetY, OffsetZ,
			//						AngleX, AngleY, AngleZ)

			PARMCHECK(4);

			strcpy(param0, arguments[0].str());
			const char *EntityType = CCD->Level()->EntityRegistry()->GetEntityType(param0);
			geVec3d Pos;

			if(EntityType)
			{
				if(!stricmp(EntityType, "ScriptPoint"))
				{
					ScriptPoint *pProxy;

					if(CCD->Level()->ScriptPoints()->LocateEntity(param0, reinterpret_cast<void**>(&pProxy)) == RGF_SUCCESS)
					{
						strcpy(param0, arguments[1].str());

						if(!stricmp(param0, "Player"))
							Pos = CCD->Player()->Position();
						else
							CCD->ActorManager()->GetPosition(CCD->ActorManager()->GetByEntityName(param0), &Pos);

						pProxy->origin.X = Pos.X + arguments[2].floatValue();
						pProxy->origin.Y = Pos.Y + arguments[3].floatValue();
						pProxy->origin.Z = Pos.Z + arguments[4].floatValue();

						if(arguments.entries() > 7)
						{
							pProxy->Angle.X = arguments[5].intValue()*GE_PIOVER180;
							pProxy->Angle.Y = arguments[6].intValue()*GE_PIOVER180;
							pProxy->Angle.Z = arguments[7].intValue()*GE_PIOVER180;
						}
					}
				}
			}

			return true;
		}
	case RGF_SM_GETENTITYSCREENX:
		{
			// USAGE:
			// int = GetEntityScreenX()
			// int = GetEntityScreenX(EntityName)
			// Gets the screen position of any Pawn or Player. If EntityName="", the active pawn is used

			geVec3d Pos, ScreenPos;

			if(arguments.entries() > 0 && arguments[0].str() != "")
			{
				strcpy(param0, arguments[0].str());

				if(!stricmp(param0, "Player"))
					Pos = CCD->Player()->Position();
				else
					CCD->ActorManager()->GetPosition(CCD->ActorManager()->GetByEntityName(param0), &Pos);
			}
			else
			{
				CCD->ActorManager()->GetPosition(m_Actor, &Pos);
			}

			geCamera_TransformAndProject(CCD->CameraManager()->Camera(), &Pos, &ScreenPos);
			returnValue = static_cast<int>(ScreenPos.X);

			return true;
		}
	case RGF_SM_GETENTITYSCREENY:
		{
			// USAGE:
			// int = GetEntityScreenY()
			// int = GetEntityScreenY(EntityName)
			// Gets the screen position of any Pawn or the Player. If EntityName="", the active pawn is used

			geVec3d Pos, ScreenPos;
			strcpy(param0, arguments[0].str());

			if(arguments.entries() > 0 && arguments[0].str() != "")
			{
				if(!stricmp(param0, "Player"))
					Pos = CCD->Player()->Position();
				else
					CCD->ActorManager()->GetPosition(CCD->ActorManager()->GetByEntityName(param0), &Pos);
			}
			else
			{
				CCD->ActorManager()->GetPosition(m_Actor, &Pos);
			}

			geCamera_TransformAndProject(CCD->CameraManager()->Camera(), &Pos, &ScreenPos);
			returnValue = static_cast<int>(ScreenPos.Y);

			return true;
		}
	case RGF_SM_GETSCREENWIDTH:
		{
			// USAGE: int = GetScreenWidth()
			// Gets the width of the screen in pixels
			returnValue = CCD->Engine()->Width();
			return true;
		}
	case RGF_SM_GETSCREENHEIGHT:
		{
			// USAGE: int = GetScreenHeight()
			// Gets the height of the screen in pixels
			returnValue = CCD->Engine()->Height();
			return true;
		}
	case RGF_SM_MOUSESELECT:
		{
			// USAGE:
			// MouseSelect(HighlightOnCollision);
			// MouseSelect(HighlightOnCollision, PlayerIsSelectable);
			// MouseSelect(HighlightOnCollision, PlayerIsSelectable, FillR, FillG, FillB);
			// MouseSelect(HighlightOnCollision, PlayerIsSelectable, FillR, FillG, FillB,
			//				AmbientR, AmbientG, AmbientB);
			// Returns the EntityName of the pawn selected or "Player" if the player is selected
			// If PlayerIsSelectable = true, the player can be mouse-picked

			PARMCHECK(1);

			geXForm3d Xf;
			geVec3d dir, start, end;
			RECT client;
			POINT RectPos;
			POINT MousePos;
			GE_Collision MouseRayCollision;
			GE_RGBA	theFillColor = {255.0f, 255.0f, 255.0f};
			GE_RGBA	theAmbientColor = {255.0f, 255.0f, 255.0f};
			bool CollidePlayer = false;
			bool HighLight = arguments[0].boolValue();

			if(arguments.entries() > 1)
				CollidePlayer = arguments[1].boolValue();

			if(arguments.entries() > 4)
			{
				theFillColor.r = arguments[2].floatValue();
				theFillColor.g = arguments[3].floatValue();
				theFillColor.b = arguments[4].floatValue();
			}

			if(arguments.entries() > 7)
			{
				theAmbientColor.r = arguments[5].floatValue();
				theAmbientColor.g = arguments[6].floatValue();
				theAmbientColor.b = arguments[7].floatValue();
			}

			CCD->GUIManager()->ShowCursor();

			if(!CCD->Engine()->FullScreen())
			{
				GetClientRect(CCD->Engine()->WindowHandle(),&client);
				RectPos.x = client.left;
				RectPos.y = client.top;
				ClientToScreen(CCD->Engine()->WindowHandle(),&RectPos);
			}
			else
			{
				RectPos.x = 0;
				RectPos.y = 0;
			}

			CCD->CameraManager()->GetPosition(&start);
			geXForm3d_Translate(&Xf, start.X, start.Y, start.Z);
			GetCursorPos(&MousePos);

			MousePos.x -= RectPos.x;
			MousePos.y -= RectPos.y;

			geCamera_ScreenPointToWorld(CCD->CameraManager()->Camera(), MousePos.x, MousePos.y, &dir);
			geVec3d_AddScaled(&start, &dir, 10000.0f, &end);

			geBoolean Result = geWorld_Collision(CCD->World(), NULL, NULL,
								 &start, &end, GE_CONTENTS_CANNOT_OCCUPY,
								 GE_COLLIDE_ALL,
								 0xffffffff, NULL, NULL,
								 &MouseRayCollision);

			if(m_Prev_HL_Actor != MouseRayCollision.Actor)
			{
				RestoreHLActorDynamicLighting();
			}

			if( MouseRayCollision.Actor &&
				!m_Prev_HL_Actor &&
				HighLight &&
				(CollidePlayer || !(MouseRayCollision.Actor == CCD->Player()->GetActor())))
			{
				CCD->ActorManager()->GetActorDynamicLighting(MouseRayCollision.Actor,
															&m_Prev_HL_FillColor,
															&m_Prev_HL_AmbientColor,
															&m_Prev_HL_AmbientLightFromFloor);

				CCD->ActorManager()->SetActorDynamicLighting(MouseRayCollision.Actor,
															theFillColor,
															theAmbientColor,
															GE_FALSE);
				m_Prev_HL_Actor = MouseRayCollision.Actor;
			}

			if(MouseRayCollision.Actor == CCD->Player()->GetActor())
				returnValue = skString("Player");
			else
				returnValue = skString(CCD->ActorManager()->GetEntityName(MouseRayCollision.Actor).c_str());

			return true;
		}
	case RGF_SM_MOUSECONTROLLEDPLAYER:
		{
			// deprecated - use Player.SetMouseControlled
			// USAGE: MouseControlledPlayer(true/false)
			PARMCHECK(1);

			CCD->SetMouseControl(arguments[0].boolValue());
			return true;
		}
	case RGF_SM_SHOWMOUSE:
		{
			// deprecated - use GUI.MouseCursor.show(); GUI.MouseCursor.hide(); GUI.MouseCursor.setVisible();
			// USAGE: ShowMouse(true/false)
			PARMCHECK(1);

			if(!CCD->GUIManager()->IsCursorVisible() && arguments[0].boolValue())
			{
				CCD->Input()->CenterMouse();
				CCD->GUIManager()->MousePosition(0.5f * CCD->Engine()->Width(), 0.5f * CCD->Engine()->Height());
			}

			CCD->GUIManager()->SetCursorVisible(arguments[0].boolValue());
			CPlayState::GetSingletonPtr()->ShowCursor(arguments[0].boolValue());

			return true;
		}
	case RGF_SM_GETMOUSEPOSX:
		{
			// USAGE: int = GetMousePosX()
			RECT client;
			POINT RectPos;
			POINT MousePos;

			if(!CCD->Engine()->FullScreen())
			{
				GetClientRect(CCD->Engine()->WindowHandle(), &client);
				RectPos.x = client.left;
				RectPos.y = client.top;
				ClientToScreen(CCD->Engine()->WindowHandle(), &RectPos);
			}
			else
			{
				RectPos.x = 0;
			}

			GetCursorPos(&MousePos);
			MousePos.x -= RectPos.x;
			returnValue = MousePos.x;

			return true;
		}
	case RGF_SM_GETMOUSEPOSY:
		{
			// USAGE: int = GetMousePosY()
			RECT client;
			POINT RectPos;
			POINT MousePos;

			if(!CCD->Engine()->FullScreen())
			{
				GetClientRect(CCD->Engine()->WindowHandle(), &client);
				RectPos.x = client.left;
				RectPos.y = client.top;
				ClientToScreen(CCD->Engine()->WindowHandle(), &RectPos);
			}
			else
			{
				RectPos.y = 0;
			}

			GetCursorPos(&MousePos);
			MousePos.y -= RectPos.y;
			returnValue = MousePos.y;

			return true;
		}
	case RGF_SM_SETMOUSEPOS:
		{
			// USAGE: SetMousePos(int ScreenPosX, int ScreenPosY)
			PARMCHECK(2);
			CCD->Input()->SetMousePos(arguments[0].intValue(), arguments[1].intValue());
			CEGUI::System::getSingleton().injectMousePosition(arguments[0].floatValue(), arguments[1].floatValue());
			return true;
		}
	case RGF_SM_SETGAMMA:
		{
			// USAGE: SetGamma(Gamma)
			PARMCHECK(1);
			geEngine_SetGamma(CCD->Engine()->Engine(), arguments[0].floatValue());
			return true;
		}
	case RGF_SM_GETGAMMA:
		{
			// USAGE: GetGamma()
			geEngine_GetGamma(CCD->Engine()->Engine(), &param1);
			returnValue = param1;
			return true;
		}
	case RGF_SM_FILLSCREENAREA:
		{
			// deprecated
			// USAGE:
			// FillScreenArea(Nr, KeepVisible, Alpha);
			// FillScreenArea(Nr, KeepVisible, Alpha, R, G, B);
			// FillScreenArea(Nr, KeepVisible, Alpha, R, G, B, Left, Top, Right, Bottom);

			PARMCHECK(3);

			int temp = arguments[0].intValue();

			if(temp < 0 || temp >= MAXFILLAREA)
				return true;

			GE_Rect Rect;
			GE_RGBA	Color;
			Rect.Left = Rect.Top = 0;
			Rect.Right = CCD->Engine()->Width() - 1;
			Rect.Bottom = CCD->Engine()->Height() - 1;

			CCD->Level()->Pawns()->FillScrArea[temp].FillScreenAreaKeep = arguments[1].boolValue();

			Color.r = 0.0f;
			Color.g = 0.0f;
			Color.b = 0.0f;
			Color.a = 255.0f;

			if(arguments.entries() > 2)
				Color.a = arguments[2].floatValue();

			if(Color.a < 0.0f)
				Color.a = 0.0f;
			else if(Color.a > 255.0f)
				Color.a = 255.0f;

			if(arguments.entries() > 5)
			{
				Color.r = arguments[3].floatValue();
				Color.g = arguments[4].floatValue();
				Color.b = arguments[5].floatValue();
			}

			if(arguments.entries() > 9)
			{
				Rect.Left = arguments[6].intValue();
				Rect.Top = arguments[7].intValue();
				Rect.Right = arguments[8].intValue();
				Rect.Bottom = arguments[9].intValue();
			}

			CCD->Level()->Pawns()->FillScrArea[temp].FillScreenAreaRect = Rect;
			CCD->Level()->Pawns()->FillScrArea[temp].FillScreenAreaColor = Color;
			CCD->Level()->Pawns()->FillScrArea[temp].DoFillScreenArea = true;

			return true;
		}
	case RGF_SM_REMOVESCREENAREA:
		{
			// deprecated
			// USAGE: RemoveScreenArea(Nr)
			PARMCHECK(1);

			int temp = arguments[0].intValue();

			if(temp < 0 || temp >= MAXFILLAREA)
				return true;

			CCD->Level()->Pawns()->FillScrArea[temp].DoFillScreenArea = false;

			return true;
		}
	case RGF_SM_SHOWTEXT:
		{
			// deprecated
			// USAGE: ShowText(Nr, EntityName, Animation, TextString, FontName, TextSound,
			//					ScreenOffsetX, ScreenOffsetY, Align, Alpha);
			// Shows a TextMessage on the screen, attached to a Pawn or Player
			// Align can be Right, Left or Center.
			// The text can include a # to print it in multiple lines

			PARMCHECK(10);

			int Nr = arguments[0].intValue();

			if(Nr < 0 || Nr >= MAXTEXT)
				return true;

			CCD->Level()->Pawns()->TextMessage[Nr].EntityName		= arguments[1].str().c_str();
			CCD->Level()->Pawns()->TextMessage[Nr].AnimName			= arguments[2].str().c_str();
			CCD->Level()->Pawns()->TextMessage[Nr].TextString		= arguments[3].str().c_str();
			Replace(CCD->Level()->Pawns()->TextMessage[Nr].TextString, "<Player>", CCD->Player()->GetPlayerName());
			Replace(CCD->Level()->Pawns()->TextMessage[Nr].TextString, "<CR>", "\n");

			CCD->Level()->Pawns()->TextMessage[Nr].FontName			= arguments[4].str().c_str();
			CCD->Level()->Pawns()->TextMessage[Nr].TextSound		= arguments[5].str().c_str();

			CCD->Level()->Pawns()->TextMessage[Nr].ScreenOffsetX	= arguments[6].intValue();
			CCD->Level()->Pawns()->TextMessage[Nr].ScreenOffsetY	= arguments[7].intValue();

			strncpy(&(CCD->Level()->Pawns()->TextMessage[Nr].Alignment), arguments[8].str(), 1);

			CCD->Level()->Pawns()->TextMessage[Nr].Alpha			= arguments[9].floatValue();
			CCD->Level()->Pawns()->TextMessage[Nr].ShowText			= true;

			return true;
		}
	case RGF_SM_REMOVETEXT:
		{
			// deprecated
			// USAGE: RemoveText(int Nr)
			PARMCHECK(1);

			int Nr = arguments[0].intValue();

			if(Nr < 0 || Nr >= MAXTEXT)
				return true;

			CCD->Level()->Pawns()->TextMessage[Nr].ShowText = false;

			return true;
		}
	case RGF_SM_SHOWHUDPICTURE:
		{
			// deprecated
			// USAGE:
			// ShowHudPicture(HUDpictureNr, IsVisible)
			// ShowHudPicture(HUDpictureNr, IsVisible, EntityName, ScreenX, ScreenY, DisplayTime)
			// ShowHudPicture(HUDpictureNr, IsVisible, EntityName, ScreenX, ScreenY, DisplayTime, WinX, WinY)

			PARMCHECK(2);

			geVec3d Pos, ScreenPos;
			char element[16];

			sprintf(element, "picture%d", arguments[0].intValue());
			CCD->HUD()->ActivateElement(element, arguments[1].boolValue());

			if(arguments.entries() > 5)
			{
				CCD->HUD()->SetElementDisplayTime(element, arguments[5].floatValue());

				strcpy(param0, arguments[2].str());

				if(!EffectC_IsStringNull(param0))
				{
					if(!stricmp(param0, "Player"))
						Pos = CCD->Player()->Position();
					else if(!stricmp(param0, "Camera"))
						CCD->CameraManager()->GetPosition(&Pos);
					else
						CCD->ActorManager()->GetPosition(CCD->ActorManager()->GetByEntityName(param0),&Pos);

					geCamera_TransformAndProject(CCD->CameraManager()->Camera(), &Pos, &ScreenPos);

					CCD->HUD()->SetElementLeftTop(element,
													static_cast<int>(ScreenPos.X) + arguments[3].intValue(),
													static_cast<int>(ScreenPos.Y) + arguments[4].intValue());
				}
				else
				{
					CCD->HUD()->SetElementLeftTop(element, arguments[3].intValue(), arguments[4].intValue());
				}

				if(arguments.entries() > 7)
					CCD->HUD()->SetElementILeftTop(element, arguments[6].intValue(), arguments[7].intValue());
			}

			return true;
		}
	case RGF_SM_SETHUDDRAW:
		{
			// USAGE: SetHudDraw(true/false)
			PARMCHECK(1);

			if(arguments[0].boolValue())
				CCD->HUD()->Activate();
			else
				CCD->HUD()->Deactivate();

			return true;
		}
	case RGF_SM_GETHUDDRAW:
		{
			// USAGE: bool = GetHudDraw()
			returnValue = CCD->HUD()->GetActive();
			return true;
		}
	case RGF_SM_SETALPHA:
		{
			// USAGE: SetAlpha(Alpha)
			PARMCHECK(1);

			CCD->ActorManager()->SetAlpha(m_Actor, arguments[1].floatValue());

			return true;
		}
	case RGF_SM_GETALPHA:
		{
			// USAGE: float = GetAlpha()

			CCD->ActorManager()->GetAlpha(m_Actor, &param1);

			returnValue = param1;

			return true;
		}
	case RGF_SM_SETENTITYALPHA:
		{
			// USAGE: SetEntityAlpha(EntityName, Alpha)
			PARMCHECK(2);

			param1 = arguments[1].floatValue();

			if(arguments[0].str().equalsIgnoreCase("Player")) // if entityname = "Player", then use the player
				CCD->ActorManager()->SetAlpha(CCD->Player()->GetActor(), param1);
			else
				CCD->ActorManager()->SetAlpha(CCD->ActorManager()->GetByEntityName(arguments[0].str().c_str()), param1);

			return true;
		}
	case RGF_SM_GETENTITYALPHA:
		{
			// USAGE: float = GetEntityAlpha(EntityName)
			PARMCHECK(1);

			if(arguments[0].str().equalsIgnoreCase("Player")) // if entityname = "Player", then use the player
				CCD->ActorManager()->GetAlpha(CCD->Player()->GetActor(), &param1);
			else
				CCD->ActorManager()->GetAlpha(CCD->ActorManager()->GetByEntityName(arguments[0].str().c_str()), &param1);

			returnValue = param1;

			return true;
		}
	case RGF_SM_SETSCALE:
		{
			// USAGE:
			// SetScale(Scale)
			// SetScale(ScaleX, ScaleY, ScaleZ)
			PARMCHECK(1);

			geVec3d Scale;

			Scale.X = arguments[0].floatValue();

			if(arguments.entries() > 2)
			{
				Scale.Y = arguments[1].floatValue();
				Scale.Z = arguments[2].floatValue();

				CCD->ActorManager()->SetScaleXYZ(m_Actor, Scale);

				return true;
			}

			CCD->ActorManager()->SetScale(m_Actor, Scale.X);

			return true;
		}
	case RGF_SM_SETENTITYSCALE:
		{
			// USAGE:
			// SetEntityScale(EntityName, Scale)
			// SetEntityScale(EntityName, ScaleX, ScaleY, ScaleZ)
			PARMCHECK(2);

			geVec3d Scale;
			geActor *pActor;

			if(arguments[0].str().equalsIgnoreCase("Player"))
				pActor = CCD->Player()->GetActor();
			else
				pActor = CCD->ActorManager()->GetByEntityName(arguments[0].str().c_str());

			Scale.X = arguments[1].floatValue();

			if(arguments.entries() > 3)
			{
				Scale.Y = arguments[2].floatValue();
				Scale.Z = arguments[3].floatValue();

				CCD->ActorManager()->SetScaleXYZ(pActor, Scale);

				return true;
			}

			CCD->ActorManager()->SetScale(pActor, Scale.X);

			return true;
		}
	case RGF_SM_CHECKAREA:
		{
			// USAGE:
			// CheckArea(FromEntityName, ToEntityName, DistanceMode, MinSceenX, MaxSceenX,
			//				MinDistance, MaxDistance, IgnoreX, IgnoreY, IgnoreZ);

			// This order checks if the player or a pawn is within the defined area
			// The area is defined by Min and Max screenX coordinates and the min and max distance
			// the pawn is from the player/or the player is from the camera
			// DistanceMode = true: Min and Max screenX + Min and Max distance
			// DistanceMode = false: Min and Max screenX + Min and Max screenY
			// IgnoreX, IgnoreY, IgnoreZ: Includes or ignores the X, Y or Z coordinate in the distance
			// evaluation (only for distance, not for area!!)
			// Can be used to check if Player, Pawn, Camera or Mouse is in a screen box

			PARMCHECK(10);

			returnValue = CCD->Level()->Pawns()->Area(arguments[0].str().c_str(), arguments[1].str().c_str(),
								arguments[2].boolValue(), arguments[3].floatValue(), arguments[4].floatValue(),
								arguments[5].floatValue(), arguments[6].floatValue(),
								arguments[7].boolValue(), arguments[8].boolValue(), arguments[9].boolValue());

			return true;
		}
	case RGF_SM_SETFLAG:
		{
			// USAGE: SetFlag(int FlagNr, true/false)
			PARMCHECK(2);

			int int1 = arguments[0].intValue();

			if(int1 < 0 || int1 >= MAXFLAGS)
				return true;

			CCD->Level()->Pawns()->SetPawnFlag(int1, arguments[1].boolValue());
			return true;
		}
	case RGF_SM_GETFLAG:
		{
			// USAGE: bool = GetFlag(int FlagNr)
			PARMCHECK(1);

			int int1 = arguments[0].intValue();

			if(int1 < 0 || int1 >= MAXFLAGS)
				return true;

			returnValue = CCD->Level()->Pawns()->GetPawnFlag(int1);
			return true;
		}
	case RGF_SM_POWERUP:
		{
			// USAGE:	PowerUp(char *AttributeName, int Amount, bool SetMaximumAmount)
			//			PowerUp(char *AttributeName, int Amount, bool SetMaximumAmount, EntityName)
			PARMCHECK(3);

			strcpy(param0, arguments[0].str());

			CPersistentAttributes *theInv;

			if(arguments.entries() > 3)
			{
				strcpy(param4, arguments[3].str());

				if(!stricmp(param4, "Player"))
				{
					// deprecated
					theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());

					int NewHighLimit = theInv->High(param0) + arguments[1].intValue();
					theInv->SetHighLimit(param0, NewHighLimit);

					if(arguments[2].boolValue())
					{
						theInv->Set(param0, NewHighLimit);
						sxInventory::GetSingletonPtr()->UpdateItem(param0, true);
					}

					return true;
				}
				else
				{
					theInv = CCD->ActorManager()->Inventory(CCD->ActorManager()->GetByEntityName(param4));
				}
			}
			else
			{
				theInv = CCD->ActorManager()->Inventory(m_Actor);
			}

			int NewHighLimit = theInv->High(param0) + arguments[1].intValue();

			theInv->SetHighLimit(param0, NewHighLimit);

			if(arguments[2].boolValue())
				theInv->Set(param0, NewHighLimit);

			return true;
		}
	case RGF_SM_GETPOWERUPLEVEL:
		{
			// USAGE:	int = GetPowerUpLevel(char *AttributeName)
			//			int = GetPowerUpLevel(char *AttributeName, char *EntityName)
			PARMCHECK(1);

			CPersistentAttributes *theInv;

			if(arguments.entries() > 1)
			{
				strcpy(param4, arguments[1].str());

				if(!stricmp(param4, "Player"))
					theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
				else
					theInv = CCD->ActorManager()->Inventory(CCD->ActorManager()->GetByEntityName(param4));
			}
			else
			{
				theInv = CCD->ActorManager()->Inventory(m_Actor);
			}

			returnValue = theInv->GetPowerUpLevel(arguments[0].str().c_str());

			return true;
		}
	case RGF_SM_ACTIVATEHUDELEMENT:
		{
			// USAGE: ActivateHudElement(char *AttributeName, bool Flag)
			PARMCHECK(2);

			CCD->HUD()->ActivateElement(arguments[0].str().c_str(), arguments[1].boolValue());
			return true;
		}
	case RGF_SM_MOVEENTITY:
		{
			// USAGE: MoveEntity(EntityName, AmountX, AmountY, AmountZ);
			PARMCHECK(4);

			strcpy(param0, arguments[0].str());
			geVec3d Amount;

			Amount.X = arguments[1].floatValue();
			Amount.Y = arguments[2].floatValue();
			Amount.Z = arguments[3].floatValue();

			if(!stricmp(param0, "Player"))
				CCD->ActorManager()->AddTranslation(CCD->Player()->GetActor(), Amount);
			else
				CCD->ActorManager()->AddTranslation(CCD->ActorManager()->GetByEntityName(param0), Amount);

			return true;
		}
	case RGF_SM_ROTATEENTITY:
		{
			// USAGE: RotateEntity(EntityName, AmountX, AmountY, AmountZ);
			PARMCHECK(4);

			strcpy(param0, arguments[0].str());
			geVec3d Amount;

			Amount.X = GE_PIOVER180*arguments[1].floatValue();
			Amount.Y = GE_PIOVER180*arguments[2].floatValue();
			Amount.Z = GE_PIOVER180*arguments[3].floatValue();

			if(!stricmp(param0, "Player"))
				CCD->ActorManager()->AddRotation(CCD->Player()->GetActor(), Amount);
			else
				CCD->ActorManager()->AddRotation(CCD->ActorManager()->GetByEntityName(param0), Amount);

			return true;
		}
	case RGF_SM_SETENTITYPOSITION:
		{
			// USAGE: SetEntityPosition(EntityName, PosX, PosY, PosZ);
			PARMCHECK(4);

			strcpy(param0, arguments[0].str());
			geVec3d Pos;

			Pos.X = arguments[1].floatValue();
			Pos.Y = arguments[2].floatValue();
			Pos.Z = arguments[3].floatValue();

			if(!stricmp(param0, "Player"))
				CCD->ActorManager()->Position(CCD->Player()->GetActor(), Pos);
			else
				CCD->ActorManager()->Position(CCD->ActorManager()->GetByEntityName(param0), Pos);

			return true;
		}
	case RGF_SM_SETENTITYROTATION:
		{
			// USAGE: SetEntityRotation(EntityName, RotX, RotY, RotZ);
			PARMCHECK(4);

			strcpy(param0, arguments[0].str());
			geVec3d Rot;

			Rot.X = GE_PIOVER180*arguments[1].floatValue();
			Rot.Y = GE_PIOVER180*arguments[2].floatValue();
			Rot.Z = GE_PIOVER180*arguments[3].floatValue();

			if(!stricmp(param0, "Player"))
				CCD->ActorManager()->Rotate(CCD->Player()->GetActor(), Rot);
			else
				CCD->ActorManager()->Rotate(CCD->ActorManager()->GetByEntityName(param0), Rot);

			return true;
		}
	case RGF_SM_ADDATTRIBUTE:
		{
			// USAGE:	AddAttribute(char *Attribute)
			//			AddAttribute(char *Attribute, EntityName)
			//			AddAttribute(char *Attribute, int LowValue, int HighValue)
			//			AddAttribute(char *Attribute, int LowValue, int HighValue, char *EntityName)
			PARMCHECK(1);
			strcpy(param0, arguments[0].str());

			CPersistentAttributes *theInv;

			if(arguments.entries() == 4 || (arguments.entries() == 2 && arguments[1].type() == skRValue::T_String))
			{
				strcpy(param4, arguments[arguments.entries()-1].str());

				if(!stricmp(param4, "Player")) // deprecated
				{
					theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
					returnValue = theInv->Add(param0);

					int low  = (arguments[1].type() == skRValue::T_Int) ? arguments[1].intValue() : 0;
					int high = (arguments.entries() > 2) ? arguments[2].intValue() : 100;

					theInv->SetValueLimits(param0, low, high);

					sxInventory::GetSingletonPtr()->UpdateItem(param0);
					return true;
				}
				else
				{
					theInv = CCD->ActorManager()->Inventory(CCD->ActorManager()->GetByEntityName(param4));
				}
			}
			else
			{
				theInv = CCD->ActorManager()->Inventory(m_Actor);
			}

			returnValue = theInv->Add(param0);

			int low  = (arguments.entries() > 1 && arguments[1].type() == skRValue::T_Int) ? arguments[1].intValue() : 0;
			int high = (arguments.entries() > 2) ? arguments[2].intValue() : 100;

			theInv->SetValueLimits(param0, low, high);

			return true;
		}
	case RGF_SM_SETATTRIBUTEVALUELIMITS:
		{
			// USAGE:	SetAttributeValueLimits(char* Attribute, int LowValue, int HighValue)
			//			SetAttributeValueLimits(char* Attribute, int LowValue, int HighValue, char* EntityName)
			PARMCHECK(3);

			CPersistentAttributes *theInv;

			if(arguments.entries() > 3)
			{
				strcpy(param4, arguments[3].str());

				if(!stricmp(param4, "Player"))
					theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
				else
					theInv = CCD->ActorManager()->Inventory(CCD->ActorManager()->GetByEntityName(param4));
			}
			else
			{
				theInv = CCD->ActorManager()->Inventory(m_Actor);
			}

			theInv->SetValueLimits(arguments[0].str().c_str(), arguments[1].intValue(), arguments[2].intValue());

			return true;
		}
	case RGF_SM_NEARESTPOINT:
		{
			// USAGE:	NearestPoint(float MinDistance, float MaxDistance)
			PARMCHECK(2);

			returnValue = false;

			// Ok, check to see if there are ScriptPoints in this world
			geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "ScriptPoint");

			if(!pSet)
				return true;

			float MinDistance2 = arguments[0].floatValue()*arguments[0].floatValue();
			float MaxDistance2 = arguments[1].floatValue()*arguments[1].floatValue();
			float minDist2 = MaxDistance2;

			geVec3d Pos;
			CCD->ActorManager()->GetPosition(m_Actor, &Pos);

			geEntity *pEntity;

			// Ok, we have ScriptPoints somewhere.  Dig through 'em all.
			for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
				pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
			{
				ScriptPoint *pSource = static_cast<ScriptPoint*>(geEntity_GetUserData(pEntity));

				//calc new dist
				geVec3d Dist;
				geVec3d_Subtract(&pSource->origin, &Pos, &Dist);
				float newDist2 = geVec3d_LengthSquared(&Dist);

				// point is too close
				if(newDist2 < MinDistance2 || newDist2 > MaxDistance2)
					continue;

				if(newDist2 < minDist2)
				{
					// probably slowest test...
					if(CanSeePoint(m_FOV, m_Actor, &(pSource->origin), m_FOVBone))
					{
						minDist2 = newDist2;
						m_Point = pSource->szEntityName;
						m_CurrentPoint = pSource->origin;
						m_ValidPoint = true;
						returnValue = true;
					}
				}
			}

			return true;
		}
	case RGF_SM_SETFOV:
		{
			PARMCHECK(1);

			// NOTE QD: FOV is stored as acos of the half angle for direct comparison with dot products
			// acos(x) ~ (PI/2 - x) (x = angle in radians)
			// The approximation used here is not correct, but we've used it
			// for quite a while now, so leave it like this...
			// correct: FOV = GE_PIOVER2 - arguments[0].floatValue()*0.5f*GE_PIOVER180;

			float degrees = arguments[0].floatValue();

			if(degrees > 360.f)
			{
				m_FOV = -1.f;
			}
			else if(degrees < 0.f)
			{
				m_FOV = 1.f;
			}
			else
			{
				m_FOV = (180.f - degrees)/180.f;
			}

			m_FOVBone[0] = '\0';

			if(arguments.entries() > 1)
				m_FOVBone = arguments[1].str().c_str();

			return true;
		}
	case RGF_SM_SIN:
		{
			// deprecated
			PARMCHECK(1);
			returnValue = sin(arguments[0].floatValue());

			return true;
		}
	case RGF_SM_COS:
		{
			// deprecated
			PARMCHECK(1);
			returnValue = cos(arguments[0].floatValue());

			return true;
		}
	case RGF_SM_TAN:
		{
			// deprecated
			PARMCHECK(1);
			returnValue = tan(arguments[0].floatValue());

			return true;
		}
	case RGF_SM_ASIN:
		{
			// deprecated
			PARMCHECK(1);
			returnValue = asin(arguments[0].floatValue());

			return true;
		}
	case RGF_SM_ACOS:
		{
			// deprecated
			PARMCHECK(1);
			returnValue = acos(arguments[0].floatValue());

			return true;
		}
	case RGF_SM_ATAN:
		{
			// deprecated
			PARMCHECK(1);
			returnValue = atan(arguments[0].floatValue());

			return true;
		}
	case RGF_SM_SETGRAVITY:
		{
			PARMCHECK(3);
			geVec3d vec = { arguments[0].floatValue(), arguments[1].floatValue(), arguments[2].floatValue() };
			CCD->Level()->SetGravity(&vec);
			return true;
		}
	case RGF_SM_GETGRAVITYX:
		{
			returnValue = CCD->Level()->GetGravity().X;
			return true;
		}
	case RGF_SM_GETGRAVITYY:
		{
			returnValue = CCD->Level()->GetGravity().Y;
			return true;
		}
	case RGF_SM_GETGRAVITYZ:
		{
			returnValue = CCD->Level()->GetGravity().Z;
			return true;
		}
	case RGF_SM_SETWIND:
		{
			PARMCHECK(3);
			geVec3d vec = { arguments[0].floatValue(), arguments[1].floatValue(), arguments[2].floatValue() };
			CCD->Level()->SetWind(&vec);
			return true;
		}
	case RGF_SM_GETWINDX:
		{
			returnValue = CCD->Level()->GetWind().X;
			return true;
		}
	case RGF_SM_GETWINDY:
		{
			returnValue = CCD->Level()->GetWind().Y;
			return true;
		}
	case RGF_SM_GETWINDZ:
		{
			returnValue = CCD->Level()->GetWind().Z;
			return true;
		}
	case RGF_SM_SETWINDBASE:
		{
			PARMCHECK(3);
			geVec3d vec = { arguments[0].floatValue(), arguments[1].floatValue(), arguments[2].floatValue() };
			CCD->Level()->SetInitialWind(&vec);
			return true;
		}
	case RGF_SM_GETWINDBASEX:
		{
			returnValue = CCD->Level()->GetInitialWind().X;
			return true;
		}
	case RGF_SM_GETWINDBASEY:
		{
			returnValue = CCD->Level()->GetInitialWind().Y;
			return true;
		}
	case RGF_SM_GETWINDBASEZ:
		{
			returnValue = CCD->Level()->GetInitialWind().Z;
			return true;
		}
	case RGF_SM_SETWINDGENERATOR:
		{
			PARMCHECK(1);
			CCD->Level()->WindGenerator()->SetEnabled(arguments[0].boolValue());
			return true;
		}
	case RGF_SM_SETFORCEENABLED:
		{
			PARMCHECK(1);
			CCD->ActorManager()->SetForceEnabled(m_Actor, arguments[0].boolValue());
			return true;
		}
	case RGF_SM_ISINLIQUID:
		{
			int nZoneType;
			CCD->ActorManager()->GetActorZone(m_Actor, &nZoneType);

			if(nZoneType == kLiquidZone)
				returnValue = true;
			else
				returnValue = false;

			return true;
		}
	case RGF_SM_GETLIQUID:
		{
			int nZoneType;
			CCD->ActorManager()->GetActorZone(m_Actor, &nZoneType);

			if(nZoneType == kLiquidZone)
				returnValue = skString(CCD->ActorManager()->GetLiquid(m_Actor)->szEntityName);
			else
				returnValue = skString("");

			return true;
		}
	case RGF_SM_ENDSCRIPT:
		{
			m_Alive = false;
			return true;
		}
	default:
		{
			return skScriptedExecutable::method(methodName, arguments, returnValue, ctxt);
		}
	} // switch

	return true;
}

/* ------------------------------------------------------------------------------------ */
//	GetAngles
/* ------------------------------------------------------------------------------------ */
void ScriptedObject::GetAngles(bool /*flag*/)
{
	geVec3d Pos, Orient, Play;

	Play = m_LastTargetPoint;
	CCD->ActorManager()->GetPosition(m_Actor, &Pos);
	geVec3d_Subtract(&Play, &Pos, &Orient);
	float l = geVec3d_Length(&Orient);

	if(l > 0.0f)
	{
		float x = Orient.X;
		Orient.X = GE_PIOVER2 - acos(Orient.Y / l);
		Orient.Y = atan2(x, Orient.Z) + GE_PI;
	}

	{
		CCD->ActorManager()->GetRotate(m_Actor, &Pos);

		while(Pos.Y < 0.0f)
		{
			Pos.Y += GE_2PI;
		}

		while(Pos.Y >= GE_2PI)
		{
			Pos.Y -= GE_2PI;
		}

		while(Pos.X < 0.0f)
		{
			Pos.X += GE_2PI;
		}

		while(Pos.X >= GE_2PI)
		{
			Pos.X -= GE_2PI;
		}
	}

	m_actoryaw = Pos.Y;
	m_targetyaw = Orient.Y;
	m_actorpitch = Pos.X;
	m_targetpitch = -Orient.X;
}

/* ------------------------------------------------------------------------------------ */
//	Quake 2 MoveToGoal functions
/* ------------------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------------------ */
//	CheckBottom
/* ------------------------------------------------------------------------------------ */
bool ScriptedObject::CheckBottom()
{
	geVec3d Start, Stop;
	geExtBox theBox;
	TraceData trace;
	GE_Collision Collision;
	geFloat MaxStep;
	float mid, bottom;
	int x, y;

	geActor_GetExtBox(m_Actor, &theBox);
	Start.Y = theBox.Min.Y;
	Stop.Y = theBox.Min.Y - 1.0f;
	bool flag = false;

	for(x=0; x<=1; ++x)
	{
		for(y=0; y<=1; ++y)
		{
			Start.X = x ? theBox.Max.X : theBox.Min.X;
			Start.Z = y ? theBox.Max.Z : theBox.Min.Z;
			trace = CCD->Collision()->Trace(&Start, &Stop, NULL, NULL, m_Actor, &Collision);

			if(trace.fraction == 1.0f)
			{
				flag = true;
				break;
			}
		}

		if(flag)
			break;
	}

	if(!flag)
		return true;

	CCD->ActorManager()->GetStepHeight(m_Actor, &MaxStep);
	Start.Y = theBox.Min.Y;

	Start.X = Stop.X = (theBox.Min.X + theBox.Max.X)*0.5f;
	Start.Z = Stop.Z = (theBox.Min.Z + theBox.Max.Z)*0.5f;
	Stop.Y = Start.Y - (2.0f*MaxStep);
	trace = CCD->Collision()->Trace(&Start, &Stop, NULL, NULL, m_Actor, &Collision);

	if(trace.fraction == 1.0f)
		return false;

	mid = bottom = trace.endpos.Y;

	for(x=0; x<=1; ++x)
	{
		for(y=0; y<=1; ++y)
		{
			Start.X = Stop.X = x ? theBox.Max.X : theBox.Min.X;
			Start.Z = Stop.Z = y ? theBox.Max.Z : theBox.Min.Z;

			trace = CCD->Collision()->Trace(&Start, &Stop, NULL, NULL, m_Actor, &Collision);

			if(trace.fraction != 1.0f && trace.endpos.Y > bottom)
				bottom = trace.endpos.Y;

			if(trace.fraction == 1.0f || mid - trace.endpos.Y > MaxStep)
				return false;
		}
	}

	return true;
}

/* ------------------------------------------------------------------------------------ */
//	MoveStep
/* ------------------------------------------------------------------------------------ */
bool ScriptedObject::MoveStep(const geVec3d &move)
{
	geVec3d oldorg, neworg, end;
	float stepsize;
	geExtBox theBox;
	TraceData trace;
	GE_Collision Collision;

	CCD->ActorManager()->GetPosition(m_Actor, &oldorg);
	geVec3d_Add(&oldorg, &move, &neworg);
	CCD->ActorManager()->GetStepHeight(m_Actor, &stepsize);
	neworg.Y += stepsize;
	end = neworg;
	end.Y -= stepsize*2.0f;
	CCD->ActorManager()->GetBoundingBox(m_Actor, &theBox);
	trace = CCD->Collision()->Trace(&neworg, &end, &theBox.Min, &theBox.Max, m_Actor, &Collision);

	if(trace.allsolid)
	{
		return false;
	}

	if(trace.startsolid)
	{
		neworg.Y -= stepsize;
		trace = CCD->Collision()->Trace(&neworg, &end, &theBox.Min, &theBox.Max, m_Actor, &Collision);

		if(trace.allsolid || trace.startsolid)
			return false;
	}

	if(trace.fraction == 1.0f)
	{
		return false;
	}

	geVec3d_Add(&oldorg, &move, &neworg);
	CCD->ActorManager()->ValidateMove(oldorg, trace.endpos, m_Actor, false);

	if(!CheckBottom())
	{
		CCD->ActorManager()->Position(m_Actor, oldorg);
		return false;
	}

	return true;
}

/* ------------------------------------------------------------------------------------ */
//	ChangeYaw
/* ------------------------------------------------------------------------------------ */
void ScriptedObject::ChangeYaw()
{
	GetAngles(true);
	bool RotateLeft = false;
	float idealangle = anglemod(m_ideal_yaw/GE_PIOVER180)*GE_PIOVER180;
	float RotateAmt = m_actoryaw - idealangle;

	if(RotateAmt < 0.0f)
	{
		RotateAmt = -RotateAmt;
		RotateLeft = true;
	}

	if(RotateAmt > 0.0f)
	{
		if(RotateAmt > GE_PI)
		{
			RotateAmt = GE_2PI - RotateAmt;

			RotateLeft = !RotateLeft;
		}

		RotateAmt *= GE_180OVERPI;

		float amount = m_yaw_speed * m_ElapseTime;

		if(amount > RotateAmt)
		{
			amount = RotateAmt;
		}

		if(RotateLeft)
			CCD->ActorManager()->TurnRight(m_Actor, GE_PIOVER180 * amount);
		else
			CCD->ActorManager()->TurnLeft(m_Actor, GE_PIOVER180 * amount);
	}
}

/* ------------------------------------------------------------------------------------ */
//	ChangePitch
/* ------------------------------------------------------------------------------------ */
void ScriptedObject::ChangePitch()
{
	GetAngles(true);
	bool RotateUp = false;
	float idealangle = anglemod(m_ideal_pitch/GE_PIOVER180)*GE_PIOVER180;
	float RotateAmt = m_actorpitch - idealangle;

	if(RotateAmt < 0.0f)
	{
		RotateAmt = -RotateAmt;
		RotateUp = true;
	}

	if(RotateAmt > 0.0f)
	{
		if(RotateAmt > GE_PI)
		{
			RotateAmt = GE_2PI - RotateAmt;

			RotateUp = !RotateUp;
		}

		RotateAmt *= GE_180OVERPI;

		float amount = m_pitch_speed * m_ElapseTime;

		if(amount > RotateAmt)
		{
			amount = RotateAmt;
		}

		if(RotateUp)
			CCD->ActorManager()->TiltUp(m_Actor, GE_PIOVER180 * amount);
		else
			CCD->ActorManager()->TiltDown(m_Actor, GE_PIOVER180 * amount);
	}
}

/* ------------------------------------------------------------------------------------ */
//	StepDirection
/* ------------------------------------------------------------------------------------ */
bool ScriptedObject::StepDirection(float yaw, float dist)
{
	geVec3d move, oldorigin;
	geXForm3d Xform;
	geVec3d In;

	m_ideal_yaw = GE_PIOVER180 * yaw;
	ChangeYaw();

	CCD->ActorManager()->GetPosition(m_Actor, &oldorigin);

	geXForm3d_SetYRotation(&Xform, m_ideal_yaw);

	geXForm3d_GetIn(&Xform, &In);
	geVec3d_Scale(&In, dist, &move);

	if(MoveStep(move))
	{
		GetAngles(true);
		float selfangle = anglemod(m_actoryaw * GE_180OVERPI);
		float idealangle = anglemod(m_ideal_yaw * GE_180OVERPI);
		float delta = anglemod(selfangle - idealangle);

		if(!(delta > 315.f || delta < 45.f))
		{
			CCD->ActorManager()->Position(m_Actor, oldorigin);
		}

		return true;
	}

	return false;
}

#define	DI_NODIR	-1.0f

/* ------------------------------------------------------------------------------------ */
//	NewChaseDir
/* ------------------------------------------------------------------------------------ */
void ScriptedObject::NewChaseDir(float dist)
{
	float	deltax,deltay;
	geVec3d	Pos;
	float d[3];
	float tdir, olddir, turnaround;

	float yaw = m_ideal_yaw * GE_180OVERPI;
	olddir = anglemod(static_cast<float>(static_cast<int>(yaw/45.0f))*45.0f);
	turnaround = anglemod(olddir - 180.0f);

	CCD->ActorManager()->GetPosition(m_Actor, &Pos);
	deltax = m_LastTargetPoint.X - Pos.X;
	deltay = m_LastTargetPoint.Z - Pos.Z;

	if(deltax > 10.0f)
		d[1] = 0.0f;
	else if(deltax < -10.0f)
		d[1] = 180.0f;
	else
		d[1] = DI_NODIR;

	if(deltay < -10.0f)
		d[2] = 270.0f;
	else if(deltay > 10.0f)
		d[2] = 90.0f;
	else
		d[2] = DI_NODIR;

	if(d[1] != DI_NODIR && d[2] != DI_NODIR)
	{
		if(d[1] == 0)
			tdir = d[2] == 90.0f ? 45.0f : 315.0f;
		else
			tdir = d[2] == 90.0f ? 135.0f : 215.0f;

		if(tdir != turnaround && StepDirection(tdir, dist))
			return;
	}

	if(((rand()&3) & 1) ||  fabs(deltay) > fabs(deltax))
	{
		tdir = d[1];
		d[1] = d[2];
		d[2] = tdir;
	}

	if(d[1] != DI_NODIR && d[1] != turnaround && StepDirection(d[1], dist))
		return;

	if(d[2] != DI_NODIR && d[2] != turnaround && StepDirection(d[2], dist))
		return;

	if(olddir != DI_NODIR && StepDirection(olddir, dist))
		return;

	if(rand()&1)
	{
		for(int ttdir=0; ttdir<=315; ttdir+=45)
		{
			tdir = static_cast<float>(ttdir);

			if(tdir != turnaround && StepDirection(tdir, dist))
				return;
		}
	}
	else
	{
		for(int ttdir=315; ttdir >=0; ttdir-=45)
		{
			tdir = static_cast<float>(ttdir);

			if(tdir != turnaround && StepDirection(tdir, dist))
				return;
		}
	}

	if(turnaround != DI_NODIR && StepDirection(turnaround, dist))
		return;

	m_ideal_yaw = olddir * GE_PIOVER180;
}

/* ------------------------------------------------------------------------------------ */
//	anglemod
/* ------------------------------------------------------------------------------------ */
float ScriptedObject::anglemod(float a)
{
	a = (360.0f/65536.0f) * (static_cast<int>(a*(65536.0f/360.0f)) & 65535);
	return a;
}

/* ------------------------------------------------------------------------------------ */
//	CloseEnough
/* ------------------------------------------------------------------------------------ */
bool ScriptedObject::CloseEnough(float dist)
{
	geExtBox theBox, theBox1;
	CCD->ActorManager()->GetBoundingBox(m_TargetActor, &theBox);
	CCD->ActorManager()->GetBoundingBox(m_Actor, &theBox1);
	float delta = CCD->ActorManager()->DistanceFrom(m_LastTargetPoint, m_Actor) - theBox.Max.X - theBox1.Max.X;

	if(delta < dist)
		return true;

	return false;
}

/* ------------------------------------------------------------------------------------ */
//	MoveToGoal
/* ------------------------------------------------------------------------------------ */
void ScriptedObject::MoveToGoal(float dist)
{
	dist = dist * m_ElapseTime;

	if(CloseEnough(dist))
		return;

	if(!StepDirection(m_ideal_yaw/GE_PIOVER180, dist))
	{
		NewChaseDir(dist);
	}
}
/* ------------------------------------------------------------------------------------ */
// end Quake2 MoveToGoal
/* ------------------------------------------------------------------------------------ */

void ScriptedObject::RestoreHLActorDynamicLighting()
{
	if(m_Prev_HL_Actor)
	{
		CCD->ActorManager()->SetActorDynamicLighting(m_Prev_HL_Actor,
													m_Prev_HL_FillColor,
													m_Prev_HL_AmbientColor,
													m_Prev_HL_AmbientLightFromFloor);
		m_Prev_HL_Actor = NULL;
	}
}

/* ------------------------------------------------------------------------------------ */
//	UpdateLow
/* ------------------------------------------------------------------------------------ */
void ScriptedObject::UpdateLow(float timeElapsed)
{
	skRValueArray args;
	skRValue ret;

	if(m_Console)
	{
		std::string text = szName + " " + Order;
		strcpy(m_ConsoleHeader, text.c_str());
	}

	timeElapsed *= 0.001f;
	m_lowTime += timeElapsed;
	m_ElapseTime += timeElapsed;
	m_ThinkTime -= timeElapsed;

	if(m_ThinkTime > 0.0f)
		return;

	m_thinkorder = Order;

	try
	{
		method(skString(Order.c_str()), args, ret, ScriptManager::GetContext());
	}
	catch(skRuntimeException & e)
	{
		if(!strcmp(m_Indicate, "+"))
			strcpy(m_Indicate, " ");
		else
			strcpy(m_Indicate, "+");

		if(m_Console)
			strcpy(m_ConsoleError, e.toString());
	}
	catch(skParseException & e)
	{
		if(!strcmp(m_Indicate, "+"))
			strcpy(m_Indicate, " ");
		else
			strcpy(m_Indicate, "+");

		if(m_Console)
			strcpy(m_ConsoleError, e.toString());
	}
	catch(...)
	{
		std::string text("Low Level Script Error for " + szName + " in Order " + Order);
		CCD->Log()->Error(text);

		if(m_Console)
			strcpy(m_ConsoleError, text.c_str());
	}

	Order = m_thinkorder;
	m_ElapseTime = 0.0f;
}

/* ----------------------------------- END OF FILE ------------------------------------ */
