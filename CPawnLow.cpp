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
#include "CHeadsUpDisplay.h"
#include "CPolyShadow.h"
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

extern geSound_Def *SPool_Sound(const char *SName);

/* ------------------------------------------------------------------------------------ */
//	get a variable's value
/* ------------------------------------------------------------------------------------ */
bool ScriptedObject::getValue(const skString &fieldName,
							  const skString &attribute,
							  skRValue &value)
{
	long variable = CCD->GetHashMethod(fieldName.c_str());

	switch(variable)
	{
	case RGF_SM_TIME:
		{
			value = lowTime;
			return true;
		}
	case RGF_SM_THINKTIME:
		{
			value = ThinkTime;
			return true;
		}
	case RGF_SM_DIFFICULTYLEVEL:
		{
			value = CCD->GetDifficultLevel();
			return true;
		}
	case RGF_SM_ENTITYNAME:
		{
			value = skString(szName);
			return true;
		}
	case RGF_SM_HEALTH:
		{
			CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(Actor);

			if(!EffectC_IsStringNull(Attribute))
				value = theInv->Value(Attribute);
			else
				value = -1;

			return true;
		}
	case RGF_SM_ATTACK_FINISHED:
		{
			value = attack_finished;
			return true;
		}
	case RGF_SM_ATTACK_STATE:
		{
			value = attack_state;
			return true;
		}
	case RGF_SM_ENEMY_VIS:
		{
			if(!TargetActor)
			{
				value = false;
				return true;
			}

			bool flag = CCD->Pawns()->CanSee(FOV, Actor, TargetActor, FOVBone);

			if(flag)
			{
				CCD->ActorManager()->GetPosition(TargetActor, &LastTargetPoint);
				geExtBox theBox;
				CCD->ActorManager()->GetBoundingBox(TargetActor, &theBox);
				LastTargetPoint.Y += ((theBox.Max.Y)*0.5f);// /2.0f);
			}

			value = flag;
			return true;
		}
	case RGF_SM_ENEMY_INFRONT:
		{
			GetAngles(true);
			value = false;

			if(actoryaw == targetyaw)
				value = true;

			return true;
		}
	case RGF_SM_ENEMY_RANGE:
		{
			geExtBox theBox;
			CCD->ActorManager()->GetBoundingBox(TargetActor, &theBox);
			value = CCD->ActorManager()->DistanceFrom(LastTargetPoint, Actor) - theBox.Max.X;
			return true;
		}
	case RGF_SM_PLAYER_RANGE:
		{
			geExtBox theBox;
			CCD->ActorManager()->GetBoundingBox(CCD->Player()->GetActor(), &theBox);
			value = CCD->ActorManager()->DistanceFrom(CCD->Player()->Position(), Actor) - theBox.Max.X;
			return true;
		}
	case RGF_SM_ENEMY_YAW:
		{
			GetAngles(true);
			value = targetyaw;
			return true;
		}
	case RGF_SM_LAST_ENEMY_YAW:
		{
			GetAngles(false);
			value = targetyaw;
			return true;
		}
	case RGF_SM_ENEMY_PITCH:
		{
			GetAngles(true);
			value = targetpitch;
			return true;
		}
	case RGF_SM_LAST_ENEMY_PITCH:
		{
			GetAngles(false);
			value = targetpitch;
			return true;
		}
	case RGF_SM_LAST_ENEMY_RANGE:
		{
			value = CCD->ActorManager()->DistanceFrom(LastTargetPoint, Actor);
			return true;
		}
	case RGF_SM_CURRENT_YAW:
		{
			GetAngles(true);
			value = actoryaw;
			return true;
		}
	case RGF_SM_YAW_SPEED:
		{
			value = yaw_speed;
			return true;
		}
	case RGF_SM_IDEAL_YAW:
		{
			value = ideal_yaw;
			return true;
		}
	case RGF_SM_CURRENT_PITCH:
		{
			GetAngles(true);
			value = actorpitch;
			return true;
		}
	case RGF_SM_PITCH_SPEED:
		{
			value = pitch_speed;
			return true;
		}
	case RGF_SM_IDEAL_PITCH:
		{
			value = ideal_pitch;
			return true;
		}
	case RGF_SM_IN_PAIN:
		{
			value = false;

			if(!EffectC_IsStringNull(Attribute))
			{
				CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(Actor);

				if(OldAttributeAmount>theInv->Value(Attribute))
					value = true;

				OldAttributeAmount = theInv->Value(Attribute);
			}

			return true;
		}
	case RGF_SM_ANIMATE_AT_END:
		{
			value = CCD->ActorManager()->EndAnimation(Actor);
			return true;
		}
	case RGF_SM_ISFALLING:
		{
			value = false;

			if(CCD->ActorManager()->Falling(Actor)==GE_TRUE)
				if(CCD->ActorManager()->ReallyFall(Actor) == RGF_SUCCESS)
					value = true;

			return true;
		}
	case RGF_SM_CURRENT_X:
		{
			geVec3d Pos;
			CCD->ActorManager()->GetPosition(Actor, &Pos);
			value = Pos.X;
			return true;
		}
	case RGF_SM_CURRENT_Y:
		{
			geVec3d Pos;
			CCD->ActorManager()->GetPosition(Actor, &Pos);
			value = Pos.Y;
			return true;
		}
	case RGF_SM_CURRENT_Z:
		{
			geVec3d Pos;
			CCD->ActorManager()->GetPosition(Actor, &Pos);
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

			if(ValidPoint)
			{
				geVec3d Pos;
				CCD->ActorManager()->GetPosition(Actor, &Pos);
				value = geVec3d_DistanceBetween(&CurrentPoint, &Pos);
			}

			return true;
		}
	case RGF_SM_PLAYERTOPOINT:
		{
			value = -1;

			if(ValidPoint)
			{
				geVec3d Pos;
				CCD->ActorManager()->GetPosition(CCD->Player()->GetActor(), &Pos);
				value = geVec3d_DistanceBetween(&CurrentPoint, &Pos);
			}

			return true;
		}
	case RGF_SM_KEY_PRESSED:
		{
			value = CCD->Input()->GetKeyboardInputNoWait();
			return true;
		}
	case RGF_SM_PLAYER_VIS:
		{
			GetAngles(true);
			bool flag = CCD->Pawns()->CanSee(FOV, Actor, CCD->Player()->GetActor(), FOVBone);
			value = flag;
			return true;
		}
	case RGF_SM_TARGET_NAME:
		{
			if(CCD->ActorManager()->IsActor(TargetActor))
				value = skString(CCD->ActorManager()->GetEntityName(TargetActor));
			else
				value = false;

			return true;
		}
	case RGF_SM_ENEMY_X:
		{
			geVec3d Pos;
			CCD->ActorManager()->GetPosition(TargetActor, &Pos);
			value = Pos.X;
			return true;
		}
	case RGF_SM_ENEMY_Y:
		{
			geVec3d Pos;
			CCD->ActorManager()->GetPosition(TargetActor, &Pos);
			value = Pos.Y;
			return true;
		}
	case RGF_SM_ENEMY_Z:
		{
			geVec3d Pos;
			CCD->ActorManager()->GetPosition(TargetActor, &Pos);
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
			bool flag = CCD->Pawns()->CanSeePoint(FOV, Actor, &CurrentPoint, FOVBone);
			value = flag;
			return true;
		}
	case RGF_SM_PLAYER_WEAPON:
		{
			value = CCD->Weapons()->GetCurrent();
			return true;
		}
	case RGF_SM_POINT_NAME:
		{
			if(!Point)
			{
				value = skString("FALSE");
				return true;
			}

			value = skString(Point);
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
			CCD->ActorManager()->GetLODLevel(Actor, &Level);
			value = Level;
			return true;
		}
	case RGF_SM_CURRENT_SCREEN_X:
		{
			// Gets the ScreenX position of the active Pawn
			geVec3d Pos, ScreenPos;
			CCD->ActorManager()->GetPosition(Actor, &Pos);
			geCamera_TransformAndProject(CCD->CameraManager()->Camera(), &Pos, &ScreenPos);
			value = static_cast<int>(ScreenPos.X);
			return true;
		}
	case RGF_SM_CURRENT_SCREEN_Y:
		{
			// Gets the ScreenY position of the active Pawn
			geVec3d Pos, ScreenPos;
			CCD->ActorManager()->GetPosition(Actor, &Pos);
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
			value = skString(CCD->ActorManager()->GetMotion(CCD->Player()->GetActor()));
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
	long variable = CCD->GetHashMethod(fieldName.c_str());

	switch(variable)
	{
	case RGF_SM_LOWTIME:
		{
			lowTime = value.floatValue();
			return true;
		}
	case RGF_SM_THINKTIME:
		{
			ThinkTime = value.floatValue();
			return true;
		}
	case RGF_SM_THINK:
		{
			strcpy(thinkorder, value.str());
			return true;
		}
	case RGF_SM_ATTACK_FINISHED:
		{
			attack_finished = value.floatValue();
			return true;
		}
	case RGF_SM_ATTACK_STATE:
		{
			attack_state = value.intValue();
			return true;
		}
	case RGF_SM_YAW_SPEED:
		{
			yaw_speed = value.floatValue();
			return true;
		}
	case RGF_SM_IDEAL_YAW:
		{
			ideal_yaw = value.floatValue();
			return true;
		}
	case RGF_SM_PITCH_SPEED:
		{
			pitch_speed = value.floatValue();
			return true;
		}
	case RGF_SM_IDEAL_PITCH:
		{
			ideal_pitch = value.floatValue();
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
	float param1, param3;
	param0[0] = '\0';
	param4[0] = '\0';
	param5[0] = '\0';

	long method = CCD->GetHashMethod(methodName.c_str());//change scripting

	switch(method)
	{
	case RGF_SM_HIGHLEVEL:
		{
			//PARMCHECK(1);
			highlevel = true;
			strcpy(thinkorder, arguments[0].str());
			RunOrder = true;
			ActionActive = false;
			return true;
		}
	case RGF_SM_ANIMATE:
		{
			//PARMCHECK(1);
			CCD->ActorManager()->SetMotion(Actor, arguments[0].str().c_str());//param0);
			CCD->ActorManager()->SetHoldAtEnd(Actor, false);
			return true;
		}
	case RGF_SM_GRAVITY:
		{
			PARMCHECK(1);
			geVec3d Gravity = { 0.0f, 0.0f, 0.0f };

			if(arguments[0].boolValue())
				Gravity = CCD->Player()->GetGravity();

			CCD->ActorManager()->SetGravity(Actor, Gravity);
			return true;
		}
	case RGF_SM_PLAYSOUND:
		{
			PARMCHECK(1);
			strcpy(param0, arguments[0].str());

			if(!EffectC_IsStringNull(param0))
			{
				geVec3d Position;
				CCD->ActorManager()->GetPosition(Actor, &Position);
				Snd Sound;
				memset(&Sound, 0, sizeof(Sound));
				geVec3d_Copy(&Position, &(Sound.Pos));
				Sound.Min = CCD->GetAudibleRadius();

				if(arguments.entries() == 2)
					Sound.Min = arguments[1].floatValue();

				Sound.Loop = GE_FALSE;
				Sound.SoundDef = SPool_Sound(param0);

				if(Sound.SoundDef != NULL)
					CCD->EffectManager()->Item_Add(EFF_SND, (void*)&Sound);
			}
			return true;
		}
	case RGF_SM_ENEMYEXIST:
		{
			PARMCHECK(1);
			strcpy(param0, arguments[0].str());
			returnValue = 0;

			if(CCD->ActorManager()->IsActor(TargetActor))
			{
				if(TargetActor == CCD->Player()->GetActor())
					strcpy(param0, "health");

				CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(TargetActor);

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
				theInv = CCD->ActorManager()->Inventory(Actor);
			}

			returnValue = theInv->Value(arguments[0].str().c_str());
			return true;
		}
	case RGF_SM_GETCURFLIPBOOK:
		{
			PARMCHECK(1);
			FlipBook *pEntityData = NULL;

			if(CCD->FlipBooks()->LocateEntity(arguments[0].str().c_str(), (void**)&pEntityData) == RGF_SUCCESS)
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
					theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
				}
				else
				{
					theInv = CCD->ActorManager()->Inventory(CCD->ActorManager()->GetByEntityName(param4));
				}
			}
			else
			{
				theInv = CCD->ActorManager()->Inventory(Actor);
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
					theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
				}
				else
				{
					theInv = CCD->ActorManager()->Inventory(CCD->ActorManager()->GetByEntityName(param4));
				}
			}
			else
			{
				theInv = CCD->ActorManager()->Inventory(Actor);
			}

			returnValue = theInv->Set(arguments[0].str().c_str(), arguments[1].intValue());
			return true;
		}
	case RGF_SM_SETPLAYERWEAPON:
		{
			PARMCHECK(1);
			int temp = arguments[0].intValue();
			if(temp<0 || temp>=MAX_WEAPONS)
				CCD->Weapons()->Holster();
			else
				CCD->Weapons()->SetWeapon(temp);
			return true;
		}
	case RGF_SM_SETUSEITEM:
		{
			PARMCHECK(1);
			CCD->Player()->SetUseAttribute(arguments[0].str().c_str());
			CCD->HUD()->ActivateElement(arguments[0].str().c_str(), true);
			return true;
		}
	case RGF_SM_CLEARUSEITEM:
		{
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
			returnValue = CCD->Pawns()->CanSee(FOV, Actor, CCD->ActorManager()->GetByEntityName(arguments[0].str().c_str()), FOVBone);
			return true;
		}
	case RGF_SM_DAMAGEENTITY:
		{
			// PARMCHECK(3);
			float amount = arguments[0].floatValue();
			CCD->Damage()->DamageActor(CCD->ActorManager()->GetByEntityName(arguments[2].str().c_str()),
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
			// PARMCHECK(1);
			CCD->ActorManager()->SetMotion(Actor, arguments[0].str().c_str());
			CCD->ActorManager()->SetHoldAtEnd(Actor, true);
			return true;
		}
	case RGF_SM_ANIMATETARGET:
		{
			PARMCHECK(2);
			CCD->ActorManager()->SetMotion(TargetActor, arguments[0].str().c_str());
			CCD->ActorManager()->SetHoldAtEnd(TargetActor, arguments[1].boolValue());
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
			PARMCHECK(1);
			returnValue = false;

			if(CCD->Input()->GetKeyCheck(arguments[0].intValue()) == true)
				returnValue = true;

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
			CCD->ActorManager()->Rotate(Actor, theRotation);
			return true;
		}
	case RGF_SM_FACEPOINT:
		{
			if(ValidPoint)
				CCD->ActorManager()->RotateToFacePoint(Actor, CurrentPoint);

			return true;
		}
	case RGF_SM_NEWPOINT:
		{
			PARMCHECK(1);
			strcpy(param0, arguments[0].str());
			char *EntityType = CCD->EntityRegistry()->GetEntityType(param0);

			if(EntityType)
			{
				if(!stricmp(EntityType, "ScriptPoint"))
				{
					ScriptPoint *pProxy;
					CCD->ScriptPoints()->LocateEntity(param0, (void**)&pProxy);
					CurrentPoint = pProxy->origin;
					ValidPoint = true;
					strcpy(Point, param0);
					NextOrder[0] = '\0';

					if(!EffectC_IsStringNull(pProxy->NextOrder))
						strcpy(NextOrder, pProxy->NextOrder);
				}
			}

			return true;
		}
	case RGF_SM_GETPOINTYAW:
		{
			PARMCHECK(1);
			strcpy(param0, arguments[0].str());
			geVec3d pRotation;
			char *EntityType = CCD->EntityRegistry()->GetEntityType(param0);

			if(EntityType)
			{
				if(!stricmp(EntityType, "ScriptPoint"))
				{
					ScriptPoint *pProxy;
					CCD->ScriptPoints()->LocateEntity(param0, (void**)&pProxy);
					pRotation = pProxy->Angle;
					returnValue = pRotation.Y;
				}
			}

			return true;
		}
	case RGF_SM_NEXTPOINT:
		{
			char *EntityType = CCD->EntityRegistry()->GetEntityType(Point);

			if(EntityType)
			{
				if(!stricmp(EntityType, "ScriptPoint"))
				{
					ScriptPoint *pProxy;
					CCD->ScriptPoints()->LocateEntity(Point, (void**)&pProxy);
					ValidPoint = false;

					if(!EffectC_IsStringNull(pProxy->NextPoint))
					{
						strcpy(Point, pProxy->NextPoint);
						NextOrder[0] = '\0';

						if(!EffectC_IsStringNull(pProxy->NextOrder))
							strcpy(NextOrder, pProxy->NextOrder);

						if(!EffectC_IsStringNull(Point))
						{
							char *EntityType = CCD->EntityRegistry()->GetEntityType(Point);

							if(EntityType)
							{
								if(!stricmp(EntityType, "ScriptPoint"))
								{
									ScriptPoint *pProxy;
									CCD->ScriptPoints()->LocateEntity(Point, (void**)&pProxy);
									CurrentPoint = pProxy->origin;
									ValidPoint = true;
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

			TargetActor = MActor;
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

			CCD->ActorManager()->GetPosition(Actor, &tPos);
			CCD->ActorManager()->GetPosition(MActor, &Pos);
			returnValue = geVec3d_DistanceBetween(&tPos, &Pos);
			return true;
		}
	case RGF_SM_TELEPORTENTITY:
		{
			// USAGE:	TeleportEntity(EntityName, Point)
			//			TeleportEntity(EntityName, Point, OffsetX, OffsetY, OffsetZ)
			//			TeleportEntity(EntityName, Point, OffsetX, OffsetY, OffsetZ, UseAngle)
			strcpy(param0, arguments[1].str());
			strcpy(param4, arguments[0].str());
			char *EType = CCD->EntityRegistry()->GetEntityType(param0);
			geActor *tActor;

			if(!stricmp(param4, "Player"))
				tActor = CCD->Player()->GetActor();
			else
				tActor = CCD->ActorManager()->GetByEntityName(param4);

			if(!tActor)
				return true;

			if(EType)
			{
				if(!stricmp(EType, "ScriptPoint"))
				{
					ScriptPoint *pProxy;
					CCD->ScriptPoints()->LocateEntity(param0, (void**)&pProxy);
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
							geVec3d_Scale(&(pProxy->Angle), 0.0174532925199433f, &RealAngle);
							RealAngle.Y -= GE_PIOVER2;

							CCD->ActorManager()->Rotate(tActor, RealAngle);

							if(CCD->Player()->GetActor() == tActor
								&& CCD->Player()->GetViewPoint() == FIRSTPERSON)
							{
								CCD->CameraManager()->Rotate(RealAngle);
							}
						}
					}

					if(tActor == Actor && WeaponActor)
						CCD->ActorManager()->Position(WeaponActor, Pos);
				}
			}

			return true;
		}
	case RGF_SM_SAVEATTRIBUTES:
		{
			PARMCHECK(1);
			CCD->Player()->SaveAttributesAscii(arguments[0].str().c_str());
			return true;
		}
	case RGF_SM_TRACETOACTOR:
		{
			PARMCHECK(4);
			geXForm3d Xf;
			geVec3d OldPos, Pos, Normal, theRotation, Direction;
			geActor *pActor;
			geFloat T;
			CCD->ActorManager()->GetPosition(Actor, &OldPos);
			returnValue = skString("FALSE");
			bool bone;

			if(WeaponActor)
				bone = geActor_GetBoneTransform(WeaponActor, arguments[0].str().c_str(), &Xf);
			else
				bone = geActor_GetBoneTransform(Actor, arguments[0].str().c_str(), &Xf);

			if(!bone)
				return true;

			geVec3d_Copy(&(Xf.Translation), &OldPos);
			CCD->ActorManager()->GetRotate(Actor, &theRotation);
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

			if(CCD->ActorManager()->DoesRayHitActor(OldPos, Pos, &pActor, Actor, &T, &Normal) == GE_TRUE)
				returnValue = skString(CCD->ActorManager()->GetEntityName(pActor));

			return true;
		}
	case RGF_SM_ANIMATEBLEND:
		{
			// PARMCHECK(2);
			param1 = arguments[1].floatValue();

			if(param1 > 0.0f)
			{
				CCD->ActorManager()->SetTransitionMotion(Actor, arguments[0].str().c_str(), param1, NULL);
				CCD->ActorManager()->SetHoldAtEnd(Actor, true);
			}

			return true;
		}
	case RGF_SM_ANIMATIONSPEED:
		{
			// PARMCHECK(1);
			CCD->ActorManager()->SetAnimationSpeed(Actor, arguments[0].floatValue());
			return true;
		}
	case RGF_SM_SETCOLLISION:
		{
			CCD->ActorManager()->SetCollide(Actor);

			if(WeaponActor)
			{
				CCD->ActorManager()->SetCollide(WeaponActor);
				CCD->ActorManager()->SetNoCollide(WeaponActor);
			}

			return true;
		}
	case RGF_SM_SETNOCOLLISION:
		{
			CCD->ActorManager()->SetNoCollide(Actor);
			return true;
		}
	case RGF_SM_DAMAGEAREA:
		{
			PARMCHECK(3);
			geVec3d Pos;
			geFloat dAmount,dRange;
			dAmount = arguments[0].floatValue();
			dRange = arguments[1].floatValue();
			strcpy(param0, arguments[2].str());
			CCD->ActorManager()->GetPosition(Actor, &Pos);
			CCD->Damage()->DamageActorInRange(Pos, dRange, dAmount, param0, 0.0f, "", "Explosion");
			CCD->Damage()->DamageModelInRange(Pos, dRange, dAmount, param0, 0.0f, "");
			return true;
		}
	case RGF_SM_PLAYERMATCHANGLES:
		{
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
			PARMCHECK(1);
			returnValue = arguments[0].floatValue()*GE_PIOVER180;
			return true;
		}
	case RGF_SM_ATTACHCAMERA:
		{
			CCD->CameraManager()->BindToActor(Actor);
			return true;
		}
	case RGF_SM_ATTACHCAMERATOBONE:
		{
			PARMCHECK(1);
			CCD->CameraManager()->BindToActorBone(Actor, arguments[0].str().c_str());
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
			param3 = arguments[0].floatValue();

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
			geVec3d Pos,theRotation;
			geXForm3d Xf;
			MovingPlatform *pEntity;
			CCD->Platforms()->LocateEntity(arguments[0].str().c_str(), (void**)&pEntity);
			CCD->ModelManager()->GetPosition(pEntity->Model, &Pos);
			CCD->ModelManager()->GetRotation(pEntity->Model, &theRotation);

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

			CCD->ActorManager()->Position(Actor, Pos);

			if(arguments[4].boolValue())
				CCD->ActorManager()->Rotate(Actor, theRotation);

			if(WeaponActor)
				CCD->ActorManager()->Position(WeaponActor, Pos);

			return true;
		}
	case RGF_SM_ACTIVATETRIGGER:
		{
			// PARMCHECK(1);
			CCD->Triggers()->HandleTriggerEvent(arguments[0].str().c_str());
			return true;
		}
	case RGF_SM_UPDATEENEMYVIS:
		{
			GetAngles(true);

			if(!TargetActor)
				return true;

			CCD->ActorManager()->GetPosition(TargetActor, &LastTargetPoint);
			geExtBox theBox;
			CCD->ActorManager()->GetBoundingBox(TargetActor, &theBox);
			LastTargetPoint.Y += ((theBox.Max.Y)*0.5f); // /2.0f);
			return true;
		}
	case RGF_SM_TARGETPLAYER:
		{
			TargetActor = CCD->Player()->GetActor();
			return true;
		}
	case RGF_SM_FIREPROJECTILEBLIND:
		{
			// PARMCHECK(6);
			geXForm3d Xf;
			geVec3d Pos, theRotation, Direction, Orient, tPoint;
			strcpy(param0, arguments[1].str());	// Bone
			strcpy(param4, arguments[0].str());	// Projectile
			strcpy(param5, arguments[5].str());	// Attribute
			bool bone;

			if(WeaponActor)
				bone = geActor_GetBoneTransform(WeaponActor, param0, &Xf);
			else
				bone = geActor_GetBoneTransform(Actor, param0, &Xf);

			if(!bone)
				return true;

			geVec3d_Copy(&(Xf.Translation), &Pos);
			CCD->ActorManager()->GetRotate(Actor, &theRotation);

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

				CCD->Weapons()->Add_Projectile(Pos, Pos, Orient, param4, param5, param5);
			}

			return true;
		}
	case RGF_SM_SETTARGETPOINT:
		{
			// PARMCHECK(3);
			geXForm3d Xf;
			geVec3d Pos, theRotation, Direction;
			bool Offset = true;

			if(arguments.entries()>3)
				Offset = arguments[3].boolValue();

			if(Offset)
			{
				CCD->ActorManager()->GetPosition(Actor, &Pos);
				CCD->ActorManager()->GetRotate(Actor, &theRotation);

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

			UpdateTargetPoint = Pos;
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
			PARMCHECK(2);
			returnValue = CCD->CheckJoystickButton(arguments[0].intValue(), arguments[1].intValue());
			return true;
		}
	case RGF_SM_GETJOYAXISX:
		{
			PARMCHECK(1);
			returnValue = CCD->PollJoystickAxis(arguments[0].intValue(), 0);
			return true;
		}
	case RGF_SM_GETJOYAXISY:
		{
			PARMCHECK(1);
			returnValue = CCD->PollJoystickAxis(arguments[0].intValue(), 1);
			return true;
		}
	case RGF_SM_GETJOYAXISZ:
		{
			PARMCHECK(1);
			returnValue = CCD->PollJoystickAxis(arguments[0].intValue(), 2);
			return true;
		}
	case RGF_SM_GETNUMJOYSTICKS:
		{
			returnValue = CCD->GetNumJoys();
			return true;
		}
	case RGF_SM_SETBOUNDINGBOX:
		{
			// USAGE SetBoundingBox(ANIMATION, width);
			// PARMCHECK(1);
			float width = arguments[1].floatValue(); // width
			CCD->ActorManager()->SetBoundingBox(Actor, arguments[0].str().c_str());
			geExtBox theBox;
			CCD->ActorManager()->GetBoundingBox(Actor, &theBox);
			float CurrentHeight;
			CurrentHeight = theBox.Max.Y;

			if(width > 0.0f)
				CCD->ActorManager()->SetBBox(Actor, width, -CurrentHeight*2.0f, width);

			return true;
		}
	case RGF_SM_GETBONETOBONE:
		{
			// USAGE: FLOAT GetBoneToBone(BONE NAME, TARGET ENTITY NAME, TARGET BONE NAME);
			PARMCHECK(3);
			geVec3d bpos1, bpos2;
			CCD->ActorManager()->GetBonePosition(Actor, arguments[0].str().c_str(), &bpos1);
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
			float amount = arguments[0].floatValue();
			geVec3d theUp;
			geActor *mActor;
			mActor = CCD->ActorManager()->GetByEntityName(arguments[1].str().c_str());
			CCD->ActorManager()->UpVector(mActor, &theUp);
			CCD->ActorManager()->SetForce(mActor, 0, theUp, amount, amount);
			return true;
		}
	case RGF_SM_FORCEENTITYDOWN:
		{
			PARMCHECK(2);
			float amount = arguments[0].floatValue();
			geVec3d theUp;
			geActor *mActor;
			mActor = CCD->ActorManager()->GetByEntityName(arguments[1].str().c_str());
			CCD->ActorManager()->UpVector(mActor, &theUp);
			geVec3d_Inverse(&theUp);
			CCD->ActorManager()->SetForce(mActor, 0, theUp, amount, amount);
			return true;
		}
	case RGF_SM_FORCEENTITYRIGHT:
		{
			PARMCHECK(2);
			float amount = arguments[0].floatValue();
			geVec3d theUp;
			geActor *mActor;
			mActor = CCD->ActorManager()->GetByEntityName(arguments[1].str().c_str());
			CCD->ActorManager()->LeftVector(mActor, &theUp);
			geVec3d_Inverse(&theUp);
			CCD->ActorManager()->SetForce(mActor, 1, theUp, amount, amount);
			return true;
		}
	case RGF_SM_FORCEENTITYLEFT:
		{
			PARMCHECK(2);
			float amount = arguments[0].floatValue();
			geVec3d theUp;
			geActor *mActor;
			mActor = CCD->ActorManager()->GetByEntityName(arguments[1].str().c_str());
			CCD->ActorManager()->LeftVector(mActor, &theUp);
			CCD->ActorManager()->SetForce(mActor, 1, theUp, amount, amount);
			return true;
		}
	case RGF_SM_FORCEENTITYFORWARD:
		{
			PARMCHECK(2);
			float amount = arguments[0].floatValue();
			geVec3d theUp;
			geActor *mActor;
			mActor = CCD->ActorManager()->GetByEntityName(arguments[1].str().c_str());
			CCD->ActorManager()->InVector(mActor, &theUp);
			CCD->ActorManager()->SetForce(mActor, 2, theUp, amount, amount);
			return true;
		}
	case RGF_SM_FORCEENTITYBACKWARD:
		{
			// PARMCHECK(2);
			float amount = arguments[0].floatValue();
			geVec3d theUp;
			geActor *mActor;
			mActor = CCD->ActorManager()->GetByEntityName(arguments[1].str().c_str());
			CCD->ActorManager()->InVector(mActor, &theUp);
			geVec3d_Inverse(&theUp);
			CCD->ActorManager()->SetForce(mActor, 2, theUp, amount, amount);
			return true;
		}
	case RGF_SM_GETGROUNDTEXTURE:
		{
			char Texture[256];
			geVec3d fPos,tPos;
			CCD->ActorManager()->GetPosition(Actor,&fPos);
			tPos = fPos;
			tPos.Y -= 1000.f;
			getSingleTextureNameByTrace(CCD->World(), &fPos, &tPos, Texture);
			returnValue = skString(Texture);
			return true;
		}
	case RGF_SM_GETPLAYERGROUNDTEXTURE:
		{
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
			// PARMCHECK(2);
			geVec3d bPos;
			CCD->ActorManager()->GetBonePosition(CCD->ActorManager()->GetByEntityName(arguments[0].str().c_str()),
				arguments[1].str().c_str(), &bPos);
			CCD->ActorManager()->Position(Actor, bPos);
			return true;
		}
	case RGF_SM_SETWEAPONMATFROMFLIP:
		{
			// USAGE: SetWeaponMatFromFlip(FLIPBOOK szEntityName, Actor MatIndex, FlipBook Image Index, R, G, B);
			// PARMCHECK(6);
			geActor *Wactor;

			int MatIndex = arguments[1].intValue();
			int FlipIndex = arguments[2].intValue();
			float R = arguments[3].floatValue();
			float G = arguments[4].floatValue();
			float B = arguments[5].floatValue();

			FlipBook *pEntityData = NULL;

			if(CCD->Player()->GetViewPoint() == FIRSTPERSON)
				Wactor = CCD->Weapons()->GetVActor();
			else
				Wactor = CCD->Weapons()->GetPActor();

			returnValue = false;

			if(CCD->FlipBooks()->LocateEntity(arguments[0].str().c_str(), (void**)&pEntityData) == RGF_SUCCESS)
			{
				if(geActor_SetMaterial(Wactor, MatIndex, pEntityData->Bitmap[FlipIndex], R, G, B))
					returnValue = true;// Set Actor Material
			}

			return true;
		}
	case RGF_SM_SETSHADOWFROMFLIP:
		{
			// USAGE: SetShadowFromFlip(FLIPBOOK szEntityName, ACTOR EntityName);
			// PARMCHECK(2);
			float tm;
			int tb;

			FlipBook *pEntityData = NULL;

			if(CCD->FlipBooks()->LocateEntity(arguments[0].str().c_str(), (void**)&pEntityData)==RGF_SUCCESS)
			{
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
			// PARMCHECK(4);
			geXForm3d Xf;
			geVec3d OldPos, Pos, theRotation, Direction;
			GE_Collision Collision;
			CCD->ActorManager()->GetPosition(Actor, &OldPos);
			float gd = -1.f;
			bool bone;
			bone = geActor_GetBoneTransform(Actor, arguments[0].str().c_str(), &Xf);

			if(!bone)
				return true;

			geVec3d_Copy(&(Xf.Translation), &OldPos);
			CCD->ActorManager()->GetRotate(Actor, &theRotation);

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
			CCD->ActorManager()->GetBonePosition(Actor, arguments[1].str().c_str(), &pos);
			CCD->ActorManager()->SetMotion(Actor, arguments[0].str().c_str());
			CCD->ActorManager()->SetHoldAtEnd(Actor, true);
			CCD->ActorManager()->Position(Actor, pos);
			return true;
		}
	case RGF_SM_WHEREISPLAYER:
		{
			geVec3d Pos1, Pos2, temp, Rot;
			geFloat fP, dP;

			CCD->ActorManager()->GetPosition(Actor, &Pos1);
			CCD->ActorManager()->GetPosition(CCD->Player()->GetActor(), &Pos2);
			geVec3d_Subtract(&Pos2, &Pos1, &temp);
			geVec3d_Normalize(&temp);

			// Do dotproduct. If it's positive, then Actor2 is in front of Actor1
			CCD->ActorManager()->InVector(Actor, &Rot);
			dP = geVec3d_DotProduct(&temp, &Rot);
			CCD->ActorManager()->LeftVector(Actor, &Rot);
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

			CCD->ActorManager()->GetPosition(Actor, &Pos1);
			CCD->ActorManager()->GetPosition(CCD->ActorManager()->GetByEntityName(arguments[0].str().c_str()), &Pos2);
			geVec3d_Subtract(&Pos2, &Pos1, &temp);
			geVec3d_Normalize(&temp);

			// Do dotproduct.  If it's positive, then Actor2 is in front of Actor1
			CCD->ActorManager()->InVector(Actor, &Rot);
			dP = geVec3d_DotProduct(&temp, &Rot);
			CCD->ActorManager()->LeftVector(Actor, &Rot);
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
			aDef = geActor_GetActorDef(Actor);
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

			cMot = geActor_GetMotionByName(geActor_GetActorDef(Actor), CCD->ActorManager()->GetMotion(Actor));
			EndTime = CCD->ActorManager()->GetAnimationTime(Actor,ANIMATION_CHANNEL_ROOT);
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
			// PARMCHECK(2);

			const char *evnt;
			geMotion *cMot;
			float StartTime, EndTime;
			float volume;
			volume = arguments[1].floatValue();

			geActor *wActor = CCD->ActorManager()->GetByEntityName(arguments[0].str().c_str());

			cMot = geActor_GetMotionByName(geActor_GetActorDef(wActor), CCD->ActorManager()->GetMotion(wActor));
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
					CCD->EffectManager()->Item_Add(EFF_SND, (void*)&Sound);
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
		CCD->OpenRFFile(&file, kBitmapFile, param0, GE_VFILE_OPEN_READONLY);
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

			geVFile* file;
			geMotion *cMot;
			int ind;
			ind = 0;

			file = geVFile_OpenNewSystem(NULL, GE_VFILE_TYPE_DOS, arguments[0].str().c_str(), NULL, GE_VFILE_OPEN_READONLY);

			if(file != NULL)
			{
				cMot = geMotion_CreateFromFile(file);

				if(cMot != NULL)
					geActor_AddMotion(geActor_GetActorDef(Actor), cMot, &ind);

				geVFile_Close(file);//close file
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
			CCD->AudioStreams()->Play(arguments[0].str().c_str(), true, true);
			return true;
		}
	case RGF_SM_STOPALLAUDIOSTREAMS:
		{
			CCD->AudioStreams()->StopAll();
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
			// PARMCHECK(2);
			int param1 = arguments[0].intValue();
			int param3 = arguments[1].intValue();

			if(param1 < param3)
				returnValue = EffectC_rand(param1, param3);
			else
				returnValue = EffectC_rand(param3, param1);

			return true;
		}
	case RGF_SM_WALKMOVE:
		{
			// PARMCHECK(2);
			float amount = arguments[1].floatValue() * ElapseTime;
			geXForm3d Xform;
			geVec3d In, NewPosition, SavedPosition;
			CCD->ActorManager()->GetPosition(Actor, &SavedPosition);

			geXForm3d_SetYRotation(&Xform, arguments[0].floatValue());

			geXForm3d_GetIn(&Xform, &In);
			geVec3d_AddScaled(&SavedPosition, &In, amount, &NewPosition);
			returnValue = false;

			if(CCD->ActorManager()->ValidateMove(SavedPosition, NewPosition, Actor, false) == GE_TRUE)
				returnValue = true;

			return true;
		}
	case RGF_SM_FLYMOVE:
		{
			PARMCHECK(3);

			float amount = arguments[2].floatValue() * ElapseTime;
			geXForm3d Xform;
			geVec3d In, NewPosition, SavedPosition;
			CCD->ActorManager()->GetPosition(Actor, &SavedPosition);

			geXForm3d_SetXRotation(&Xform, -arguments[0].floatValue());
			geXForm3d_RotateY(&Xform, arguments[1].floatValue());

			geXForm3d_GetIn(&Xform, &In);
			geVec3d_AddScaled(&SavedPosition, &In, amount, &NewPosition);
			returnValue = false;

			if(CCD->ActorManager()->ValidateMove(SavedPosition, NewPosition, Actor, false) == GE_TRUE)
				returnValue = true;

			return true;
		}
	case RGF_SM_DAMAGE:
		{
			PARMCHECK(2);

			float amount = arguments[0].floatValue();
			strcpy(param0, arguments[1].str());
			CCD->Damage()->DamageActor(TargetActor, amount, param0, amount, param0, "Melee");
			return true;
		}
	case RGF_SM_DAMAGEPLAYER:
		{
			// PARMCHECK(2);

			float amount = arguments[0].floatValue();
			strcpy(param0, arguments[1].str());
			CCD->Damage()->DamageActor(CCD->Player()->GetActor(), amount, param0, amount, param0, "Melee");
			return true;
		}
	case RGF_SM_POSITIONTOPLAYER:
		{
			// PARMCHECK(3);

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

					CCD->ActorManager()->Position(Actor, Pos);

					if(WeaponActor)
						CCD->ActorManager()->Position(WeaponActor, Pos);
				}

				if(arguments.entries() > 4)
				{
					if(arguments[4].boolValue())
					{
						CCD->ActorManager()->Rotate(Actor, theRotation);

						if(WeaponActor)
							CCD->ActorManager()->Rotate(WeaponActor, theRotation);
					}
				}

				return true;
			}

			Pos.X += arguments[0].floatValue();
			Pos.Y += arguments[1].floatValue();
			Pos.Z += arguments[2].floatValue();
			CCD->ActorManager()->Position(Actor, Pos);

			if(WeaponActor)
				CCD->ActorManager()->Position(WeaponActor, Pos);

			return true;
		}
	case RGF_SM_PLAYERTOPOSITION:
		{
			PARMCHECK(3);
			geXForm3d Xf;
			geVec3d Pos, theRotation, Direction;

			CCD->ActorManager()->GetPosition(Actor, &Pos);

			if(arguments.entries() > 3)
			{
				bool flag = arguments[3].boolValue();

				if(flag)
				{
					CCD->ActorManager()->GetRotate(Actor, &theRotation);

					geXForm3d_SetZRotation(&Xf, theRotation.Z);
					geXForm3d_RotateX(&Xf, theRotation.X);
					geXForm3d_RotateY(&Xf, theRotation.Y);

					geXForm3d_GetUp(&Xf, &Direction);
					geVec3d_AddScaled(&Pos, &Direction, arguments[1].floatValue(), &Pos);

					geXForm3d_GetLeft(&Xf, &Direction);
					geVec3d_AddScaled(&Pos, &Direction, arguments[0].floatValue(), &Pos);

					geXForm3d_GetIn(&Xf, &Direction);
					geVec3d_AddScaled(&Pos, &Direction, arguments[2].floatValue(), &Pos);

					CCD->ActorManager()->Position(CCD->Player()->GetActor(), Pos);

					if(arguments.entries() == 5)
					{
						bool flag1 = arguments[4].boolValue();

						if(flag1)
							CCD->ActorManager()->Rotate(CCD->Player()->GetActor(), theRotation);
					}
					else
						CCD->ActorManager()->Rotate(CCD->Player()->GetActor(), theRotation);

					return true;
				}
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
			geXForm3d Xf;
			geVec3d Pos, OldPos, theRotation, Direction;
			geActor *MasterActor;

			MasterActor = CCD->ActorManager()->GetByEntityName(arguments[0].str().c_str());

			if(!MasterActor)
				return true;

			CCD->ActorManager()->GetPosition(MasterActor, &Pos);
			OldPos = Pos;
			OldPos.Y += 16.f;

			if(arguments.entries() > 4)
			{
				bool flag = arguments[4].boolValue();

				if(flag)
				{
					CCD->ActorManager()->GetRotate(MasterActor, &theRotation);

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

					CCD->ActorManager()->Position(Actor, Pos);
					if(WeaponActor)
						CCD->ActorManager()->Position(WeaponActor, Pos);

					if(arguments.entries() == 6)
					{
						if(arguments[5].boolValue())
						{
							CCD->ActorManager()->Rotate(Actor, theRotation);

							if(WeaponActor)
								CCD->ActorManager()->Rotate(WeaponActor, theRotation);
						}
					}
					else
					{
						CCD->ActorManager()->Rotate(Actor, theRotation);

						if(WeaponActor)
							CCD->ActorManager()->Rotate(WeaponActor, theRotation);
					}

					return true;
				}
			}

			Pos.X += arguments[1].floatValue();
			Pos.Y += arguments[2].floatValue();
			Pos.Z += arguments[3].floatValue();
			CCD->ActorManager()->Position(Actor, Pos);

			if(WeaponActor)
				CCD->ActorManager()->Position(WeaponActor, Pos);

			return true;
		}
	case RGF_SM_SETKEYPAUSE:
		{
			PARMCHECK(1);
			CCD->SetKeyPaused(arguments[0].boolValue());
			return true;
		}
	case RGF_SM_PLAYERRENDER:
		{
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
	case RGF_SM_PAWNRENDER:
		{
			PARMCHECK(1);

			if(arguments[0].boolValue())
			{
				CCD->ActorManager()->SetCollide(Actor);

				if(WeaponActor)
				{
					CCD->ActorManager()->SetCollide(WeaponActor);
					CCD->ActorManager()->SetNoCollide(WeaponActor);
				}
			}
			else
			{
				CCD->ActorManager()->SetNoCollide(Actor);
				CCD->ActorManager()->SetActorFlags(Actor, 0);

				if(WeaponActor)
				{
					CCD->ActorManager()->SetActorFlags(WeaponActor, 0);
				}
			}

			return true;
		}
	case RGF_SM_CHANGEMATERIAL:
		{
			PARMCHECK(1);
			strcpy(param0, arguments[0].str());

			if(!EffectC_IsStringNull(param0))
				CCD->ActorManager()->ChangeMaterial(Actor, param0);

			return true;
		}
	case RGF_SM_SETHOLDATEND:
		{
			PARMCHECK(1);
			CCD->ActorManager()->SetHoldAtEnd(Actor, arguments[0].boolValue());
			return true;
		}
	case RGF_SM_FORCEUP:
		{
			PARMCHECK(1);
			float amount = arguments[0].floatValue();
			geVec3d theUp;
			CCD->ActorManager()->UpVector(Actor, &theUp);
			CCD->ActorManager()->SetForce(Actor, 0, theUp, amount, amount);
			return true;
		}
	case RGF_SM_FORCEDOWN:
		{
			PARMCHECK(1);
			float amount = arguments[0].floatValue();
			geVec3d theUp;
			CCD->ActorManager()->UpVector(Actor, &theUp);
			geVec3d_Inverse(&theUp);
			CCD->ActorManager()->SetForce(Actor, 0, theUp, amount, amount);
			return true;
		}
	case RGF_SM_FORCERIGHT:
		{
			PARMCHECK(1);
			float amount = arguments[0].floatValue();
			geVec3d theRight;
			CCD->ActorManager()->LeftVector(Actor, &theRight);
			geVec3d_Inverse(&theRight);
			CCD->ActorManager()->SetForce(Actor, 1, theRight, amount, amount);
			return true;
		}
	case RGF_SM_FORCELEFT:
		{
			PARMCHECK(1);
			float amount = arguments[0].floatValue();
			geVec3d theLeft;
			CCD->ActorManager()->LeftVector(Actor, &theLeft);
			CCD->ActorManager()->SetForce(Actor, 1, theLeft, amount, amount);
			return true;
		}
	case RGF_SM_FORCEFORWARD:
		{
			PARMCHECK(1);
			float amount = arguments[0].floatValue();
			geVec3d theIn;
			CCD->ActorManager()->InVector(Actor, &theIn);
			CCD->ActorManager()->SetForce(Actor, 2, theIn, amount, amount);
			return true;
		}
	case RGF_SM_FORCEBACKWARD:
		{
			PARMCHECK(1);
			float amount = arguments[0].floatValue();
			geVec3d theBack;
			CCD->ActorManager()->InVector(Actor, &theBack);
			geVec3d_Inverse(&theBack);
			CCD->ActorManager()->SetForce(Actor, 2, theBack, amount, amount);
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
			UpdateTargetPoint = LastTargetPoint;
			return true;
		}
	case RGF_SM_FIREPROJECTILE:
		{
			PARMCHECK(6);
			geXForm3d Xf;
			geVec3d theRotation, Pos, Direction, Orient, TargetPoint;
			geFloat x;
			geExtBox theBox;


			TargetPoint = UpdateTargetPoint;

			if(CCD->ActorManager()->GetBoundingBox(TargetActor, &theBox) != RGF_NOT_FOUND)
			{
				if(arguments.entries() == 7)
				{
					float height = arguments[6].floatValue() - 0.5f;
					TargetPoint.Y += (theBox.Max.Y*height);
				}
			}

			strcpy(DamageAttr, arguments[5].str());

			float FireOffsetX = arguments[2].floatValue();
			float FireOffsetY = arguments[3].floatValue();
			float FireOffsetZ = arguments[4].floatValue();

			bool bone;

			if(WeaponActor)
				bone = geActor_GetBoneTransform(WeaponActor, arguments[1].str().c_str(), &Xf);
			else
				bone = geActor_GetBoneTransform(Actor, arguments[1].str().c_str(), &Xf);

			if(bone)
			{
				geVec3d_Copy(&Xf.Translation, &Pos);
				CCD->ActorManager()->GetRotate(Actor, &theRotation);

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

					CCD->Weapons()->Add_Projectile(Pos, Pos, Orient, arguments[0].str().c_str(), DamageAttr, DamageAttr);
				}
			}

			return true;
		}
	case RGF_SM_ADDEXPLOSION:
		{
			PARMCHECK(5);
			geVec3d Pos;

			strcpy(param4, arguments[1].str());

			if(geActor_DefHasBoneNamed(geActor_GetActorDef(Actor), param4))
			{
				Pos.X = arguments[2].floatValue();
				Pos.Y = arguments[3].floatValue();
				Pos.Z = arguments[4].floatValue();

				CCD->Explosions()->AddExplosion(arguments[0].str().c_str(), Pos, Actor, param4);
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
			returnValue = skString(CCD->ActorManager()->GetLastBoneHit(Actor));
			return true;
		}
	case RGF_SM_DEBUG:
		{
			PARMCHECK(1);

			if(console)
			{
				int index = -1;
				int i;

				for(i=0; i<DEBUGLINES; i++)
				{
					if(EffectC_IsStringNull(ConsoleDebug[i]))
					{
						index = i;
						break;
					}
				}

				if(index != -1)
				{
					strcpy(ConsoleDebug[index], arguments[0].str().c_str());
				}
				else
				{
					for(i=1; i<DEBUGLINES; i++)
					{
						strcpy(ConsoleDebug[i-1], ConsoleDebug[i]);
					}

					strcpy(ConsoleDebug[DEBUGLINES-1], arguments[0].str().c_str());
				}
			}

			return true;
		}
	case RGF_SM_SETEVENTSTATE:
		{
			// PARMCHECK(2);
			CCD->Pawns()->AddEvent(arguments[0].str().c_str(), arguments[1].boolValue());
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
			PARMCHECK(8);
			CCD->PWXImMgr()->AddImage(	arguments[0].str().c_str(), NULL,
										arguments[1].intValue(), arguments[2].intValue(),
										arguments[3].floatValue(),
										arguments[5].intValue(), arguments[6].intValue(), arguments[7].intValue(),
										arguments[4].intValue(), 1.0f);
			return true;
		}
	case RGF_SM_DRAWFLIPBOOKIMAGE:
		{
			PARMCHECK(9);
			FlipBook *pEntityData = NULL;

			if(CCD->FlipBooks()->LocateEntity(arguments[0].str().c_str(), (void**)&pEntityData)==RGF_SUCCESS)
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
		}
	case RGF_SM_DRAWPOLYSHADOW:
		{
			CCD->PlyShdw()->DrawShadow(Actor);
			return true;
		}
	case RGF_SM_MATCHPLAYERANGLES:
		{
			geVec3d Rot;
			CCD->ActorManager()->GetRotate(CCD->Player()->GetActor(), &Rot);
			CCD->ActorManager()->Rotate(Actor, Rot);
			return true;
		}
	case RGF_SM_DAMAGEATBONE:
		{
			// USAGE: DamageAtBone(damage amount, radius, attribute, bone name)
			geVec3d Pos;
			CCD->ActorManager()->GetBonePosition(Actor, arguments[3].str().c_str(), &Pos);
			CCD->Damage()->DamageActorInRange(	Pos, arguments[1].floatValue(), arguments[0].floatValue(),
												arguments[2].str().c_str(), 0.0f, "", "");
			return true;
		}
	case RGF_SM_SAVEACTOR:
		{
			// USAGE: SaveActor(Actor Name)
			geVFile *df;
			geActor_Def *aDef;
			aDef = geActor_GetActorDef(Actor);
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
			geVec3d Pos1 ,Pos2, Rot;

			CCD->ActorManager()->GetPosition(Actor, &Pos1);
			CCD->ActorManager()->GetPosition(CCD->ActorManager()->GetByEntityName(arguments[0].str().c_str()), &Pos2);
			Pos2.Y += arguments[1].floatValue();	// add height offset to target point
			CCD->PathFollower()->GetRotationToFacePoint(Pos1, Pos2, &Rot);
			CCD->ActorManager()->Rotate(Actor, Rot);
			return true;
		}
	case RGF_SM_AUTOWALK:
		{
			// PARMCHECK(3);
			float amount = arguments[1].floatValue() * ElapseTime;
			float distance = arguments[2].floatValue(); // distance to keep from walls
			geXForm3d Xform;
			geVec3d In, NewPosition, SavedPosition, TestPos;

			CCD->ActorManager()->GetPosition(Actor, &SavedPosition);
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

			if(CCD->ActorManager()->ValidateMove(SavedPosition, NewPosition, Actor, false) == GE_TRUE)
				returnValue = true;

			return true;
		}
	case RGF_SM_FASTDISTANCE:
		{
			// USAGE: BOOL = FastDistance(EntityName,distance);
			float dist = arguments[1].floatValue();
			geVec3d Pos1, Pos2, dV;
			CCD->ActorManager()->GetPosition(Actor, &Pos1);
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
			CCD->ActorManager()->SetStepHeight(Actor, arguments[0].floatValue());
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
			CCD->ActorManager()->GetPosition(Actor, &Pos1);
			CCD->PathFollower()->GetRotationToFacePoint(Pos1, CurrentPoint, &Rot);
			returnValue = Rot.X;
			return true;
		}
	case RGF_SM_GETYAWTOPOINT:
		{
			geVec3d Pos1, Rot;
			CCD->ActorManager()->GetPosition(Actor, &Pos1);
			CCD->PathFollower()->GetRotationToFacePoint(Pos1, CurrentPoint, &Rot);
			returnValue = Rot.Y;
			return true;
		}
	case RGF_SM_FASTPOINTCHECK:
		{
			// USAGE: BOOL = FastPointCheck(distance);
			float dist = arguments[0].floatValue();
			geVec3d Pos1, dV;
			CCD->ActorManager()->GetPosition(Actor, &Pos1);
			geVec3d_Subtract(&Pos1, &CurrentPoint, &dV);
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
			CCD->FixedCameras()->SetPosition(Pos);

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
			CCD->FixedCameras()->SetRotation(Rot);

			return true;
		}
	case RGF_SM_SETFIXEDCAMERAFOV:
		{
			// USAGE: SetFixedCameraFOV(float FOV)
			PARMCHECK(1);

			CCD->FixedCameras()->SetFOV(arguments[0].floatValue());

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
			CCD->FixedCameras()->Move(Pos);

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
			CCD->FixedCameras()->Rotate(Rot);

			return true;
		}
	case RGF_SM_DISTANCEBETWEENENTITIES:
		{
			// USAGE: DistanceBetweenEntities(FromEntity, ToEntity);
			// USAGE: DistanceBetweenEntities(FromEntity, ToEntity, OnlyXZ);
			// "Player" is a special Entity
			PARMCHECK(3);
			strcpy(param0, arguments[0].str());
			strcpy(param4, arguments[1].str());
			geVec3d Pos, tPos;

			if(!stricmp(param0, "Player"))
				Pos = CCD->Player()->Position();
			else
				CCD->ActorManager()->GetPosition(CCD->ActorManager()->GetByEntityName(param0), &Pos);

			if(!stricmp(param4, "Player"))
				Pos = CCD->Player()->Position();
			else
				CCD->ActorManager()->GetPosition(CCD->ActorManager()->GetByEntityName(param4), &tPos);

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

			geVec3d Pos, Rot;
			GE_RGBA	Color;
			geActor *pActor = NULL;

			strcpy(param0, arguments[0].str());

			if(arguments[0].str() != "")
			{
				if(!stricmp(param0, "Player"))
					pActor = CCD->Player()->GetActor();
				else
					pActor = CCD->ActorManager()->GetByEntityName(param0);

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

			strcpy(param0, arguments[0].str());

			if(arguments[0].str() != "")
			{
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

				if(arguments[7].boolValue())
					CCD->ActorManager()->SetActorDynamicLighting(pActor, FillColor, AmbientColor, GE_TRUE);
				else
					CCD->ActorManager()->SetActorDynamicLighting(pActor, FillColor, AmbientColor, GE_FALSE);

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
			char *EntityType = CCD->EntityRegistry()->GetEntityType(param0);
			geVec3d Pos;

			if(EntityType)
			{
				if(!stricmp(EntityType, "ScriptPoint"))
				{
					ScriptPoint *pProxy;
					CCD->ScriptPoints()->LocateEntity(param0, (void**)&pProxy);

					if(pProxy)
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

			PARMCHECK(1);

			geVec3d Pos, ScreenPos;
			strcpy(param0, arguments[0].str());

			if(arguments[0].str() != "")
			{
				if(!stricmp(param0, "Player"))
					Pos = CCD->Player()->Position();
				else
					CCD->ActorManager()->GetPosition(CCD->ActorManager()->GetByEntityName(param0), &Pos);
			}
			else
			{
				CCD->ActorManager()->GetPosition(Actor, &Pos);
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

			PARMCHECK(1);

			geVec3d Pos, ScreenPos;
			strcpy(param0, arguments[0].str());

			if(arguments[0].str() != "")
			{
				if(!stricmp(param0, "Player"))
					Pos = CCD->Player()->Position();
				else
					CCD->ActorManager()->GetPosition(CCD->ActorManager()->GetByEntityName(param0), &Pos);
			}
			else
			{
				CCD->ActorManager()->GetPosition(Actor, &Pos);
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

			geXForm3d Xf;
			geVec3d dir, start, end;
			RECT client;
			POINT RectPos;
			POINT MousePos;
			GE_Collision MouseRayCollision;
			GE_RGBA	theFillColor = {255.0f, 255.0f, 255.0f};
			GE_RGBA	theAmbientColor = {255.0f, 255.0f, 255.0f};
			bool Result = false;
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

			CCD->MenuManager()->ShowCursor(true);

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

			Result = geWorld_Collision(CCD->World(), NULL, NULL,
								 &start, &end, GE_CONTENTS_CANNOT_OCCUPY,
								 GE_COLLIDE_ALL,
								 0xffffffff, NULL, NULL,
								 &MouseRayCollision);

			if(Prev_HL_Actor != MouseRayCollision.Actor)
			{
				CCD->ActorManager()->SetActorDynamicLighting(Prev_HL_Actor,
															Prev_HL_FillColor,
															Prev_HL_AmbientColor,
															Prev_HL_AmbientLightFromFloor);
				Prev_HL_Actor = NULL;
			}

			if(MouseRayCollision.Actor && !Prev_HL_Actor && HighLight
				&& (CollidePlayer || !(MouseRayCollision.Actor == CCD->Player()->GetActor())))
			{
				CCD->ActorManager()->GetActorDynamicLighting(MouseRayCollision.Actor,
															&Prev_HL_FillColor,
															&Prev_HL_AmbientColor,
															&Prev_HL_AmbientLightFromFloor);

				CCD->ActorManager()->SetActorDynamicLighting(MouseRayCollision.Actor,
															theFillColor,
															theAmbientColor,
															GE_FALSE);
				Prev_HL_Actor = MouseRayCollision.Actor;
			}

			if(MouseRayCollision.Actor == CCD->Player()->GetActor())
				returnValue = skString("Player");
			else
				returnValue = skString(CCD->ActorManager()->GetEntityName(MouseRayCollision.Actor));

			return true;
		}
	case RGF_SM_MOUSECONTROLLEDPLAYER:
		{
			// USAGE: MouseControlledPlayer(true/false)

			CCD->SetMouseControl(arguments[0].boolValue());
			return true;
		}
	case RGF_SM_SHOWMOUSE:
		{
			// USAGE: ShowMouse(true/false)

			if(arguments[0].boolValue())
				CCD->MenuManager()->ShowCursor(true);
			else
				CCD->MenuManager()->ShowCursor(false);

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
			SetCursorPos(arguments[0].intValue(), arguments[1].intValue());
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
			// USAGE:
			// FillScreenArea(Nr, KeepVisible, Alpha);
			// FillScreenArea(Nr, KeepVisible, Alpha, R, G, B);
			// FillScreenArea(Nr, KeepVisible, Alpha, R, G, B, Left, Top, Right, Bottom);

			int temp = arguments[0].intValue();

			if(temp < 0 || temp >= MAXFILLAREA)
				return true;

			GE_Rect Rect;
			GE_RGBA	Color;
			Rect.Left = Rect.Top = 0;
			Rect.Right = CCD->Engine()->Width() - 1;
			Rect.Bottom = CCD->Engine()->Height() - 1;

			CCD->Pawns()->FillScrArea[temp].FillScreenAreaKeep = arguments[1].boolValue();

			Color.r = 0.0f;
			Color.g = 0.0f;
			Color.b = 0.0f;

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

			CCD->Pawns()->FillScrArea[temp].FillScreenAreaRect = Rect;
			CCD->Pawns()->FillScrArea[temp].FillScreenAreaColor = Color;
			CCD->Pawns()->FillScrArea[temp].DoFillScreenArea = true;

			return true;

		}
	case RGF_SM_REMOVESCREENAREA:
		{
			// USAGE: RemoveScreenArea(Nr)
			PARMCHECK(1);

			int temp = arguments[0].intValue();

			if(temp < 0 || temp >= MAXFILLAREA)
				return true;

			CCD->Pawns()->FillScrArea[temp].DoFillScreenArea = false;

			return true;
		}
	case RGF_SM_SHOWTEXT:
		{
			// USAGE: ShowText(Nr, EntityName, Animation, TextString, FontName, TextSound,
			//					ScreenOffsetX, ScreenOffsetY, Align, Alpha);
			// Shows a TextMessage on the screen, attached to a Pawn or Player
			// Align can be Right, Left or Center.
			// The text can include a # to print it in multiple lines

			PARMCHECK(10);

			int Nr = arguments[0].intValue();

			if(Nr < 0 || Nr >= MAXTEXT)
				return true;

			strcpy(CCD->Pawns()->TextMessage[Nr].EntityName, arguments[1].str());
			strcpy(CCD->Pawns()->TextMessage[Nr].AnimName, arguments[2].str());
			CCD->Pawns()->TextMessage[Nr].TextString = arguments[3].str();
			Replace(CCD->Pawns()->TextMessage[Nr].TextString, "<Player>", CCD->Player()->GetPlayerName());

			CCD->Pawns()->TextMessage[Nr].FontNr = arguments[4].intValue();

			strcpy(CCD->Pawns()->TextMessage[Nr].TextSound, arguments[5].str());

			CCD->Pawns()->TextMessage[Nr].ScreenOffsetX = arguments[6].intValue();
			CCD->Pawns()->TextMessage[Nr].ScreenOffsetY = arguments[7].intValue();

			strncpy(&(CCD->Pawns()->TextMessage[Nr].Alignment), arguments[8].str(), 1);

			CCD->Pawns()->TextMessage[Nr].Alpha = arguments[9].floatValue();
			CCD->Pawns()->TextMessage[Nr].ShowText = true;

			return true;
		}
	case RGF_SM_REMOVETEXT:
		{
			// USAGE: RemoveText(int Nr)
			PARMCHECK(1);

			int Nr = arguments[0].intValue();

			if(Nr < 0 || Nr >= MAXTEXT)
				return true;

			CCD->Pawns()->TextMessage[Nr].ShowText = false;

			return true;
		}
	case RGF_SM_SHOWHUDPICTURE:
		{
			// USAGE:
			// ShowHudPicture(HUDpictureNr, IsVisible)
			// ShowHudPicture(HUDpictureNr, IsVisible, EntityName, ScreenX, ScreenY, DisplayTime)
			// ShowHudPicture(HUDpictureNr, IsVisible, EntityName, ScreenX, ScreenY, DisplayTime, WinX, WinY)

			geVec3d Pos, ScreenPos;
			char element[16];

			if(arguments.entries() > 1)
			{
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

						CCD->HUD()->SetElementLeftTop(element, (int)ScreenPos.X + arguments[3].intValue(), (int)ScreenPos.Y + arguments[4].intValue());
					}
					else
						CCD->HUD()->SetElementLeftTop(element, arguments[3].intValue(), arguments[4].intValue());

					if(arguments.entries() > 7)
						CCD->HUD()->SetElementILeftTop(element, arguments[6].intValue(), arguments[7].intValue());
				}
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

			CCD->ActorManager()->SetAlpha(Actor, arguments[1].floatValue());

			return true;
		}
	case RGF_SM_GETALPHA:
		{
			// USAGE: float = GetAlpha()

			CCD->ActorManager()->GetAlpha(Actor, &param1);

			returnValue = param1;

			return true;
		}
	case RGF_SM_SETENTITYALPHA:
		{
			// USAGE: SetEntityAlpha(EntityName, Alpha)
			PARMCHECK(2);
			strcpy(param0, arguments[0].str());
			param1 = arguments[1].floatValue();

			if(!stricmp(param0, "Player")) // if entityname = "Player", then use the player
				CCD->ActorManager()->SetAlpha(CCD->Player()->GetActor(), param1);
			else
				CCD->ActorManager()->SetAlpha(CCD->ActorManager()->GetByEntityName(param0), param1);

			return true;
		}
	case RGF_SM_GETENTITYALPHA:
		{
			// USAGE: float = GetEntityAlpha(EntityName)
			PARMCHECK(1);

			strcpy(param0, arguments[0].str());

			if(stricmp(param0, "Player")) // if entityname = "Player", then use the player
				CCD->ActorManager()->GetAlpha(CCD->Player()->GetActor(), &param1);
			else
				CCD->ActorManager()->GetAlpha(CCD->ActorManager()->GetByEntityName(param0), &param1);

			returnValue = param1;

			return true;
		}
	case RGF_SM_SETSCALE:
		{
			// USAGE:
			// SetScale(Scale)
			// SetScale(ScaleX, ScaleY, ScaleZ)

			geVec3d Scale;

			Scale.X = arguments[0].floatValue();

			if(arguments.entries() > 2)
			{
				Scale.Y = arguments[1].floatValue();
				Scale.Z = arguments[2].floatValue();

				CCD->ActorManager()->SetScaleXYZ(Actor, Scale);

				return true;
			}

			CCD->ActorManager()->SetScale(Actor, Scale.X);

			return true;
		}
	case RGF_SM_SETENTITYSCALE:
		{
			// USAGE:
			// SetEntityScale(EntityName, Scale)
			// SetEntityScale(EntityName, ScaleX, ScaleY, ScaleZ)

			strcpy(param0, arguments[0].str());
			geVec3d Scale;
			geActor *pActor;

			if(!stricmp(param0, "Player"))
				pActor = CCD->Player()->GetActor();
			else
				pActor = CCD->ActorManager()->GetByEntityName(param0);

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

			returnValue = bool(CCD->Pawns()->Area(arguments[0].str().c_str(), arguments[1].str().c_str(),
								arguments[2].boolValue(), arguments[3].floatValue(), arguments[4].floatValue(),
								arguments[5].floatValue(), arguments[6].floatValue(),
								arguments[7].boolValue(), arguments[8].boolValue(), arguments[9].boolValue()));

			return true;
		}
	case RGF_SM_SETFLAG:
		{
			// USAGE: SetFlag(int FlagNr, true/false)
			int int1 = arguments[0].intValue();

			if(int1 < 0 || int1 >= MAXFLAGS)
				return true;

			CCD->Pawns()->SetPawnFlag(int1, arguments[1].boolValue());
			return true;
		}
	case RGF_SM_GETFLAG:
		{
			// USAGE: bool = GetFlag(int FlagNr)
			int int1 = arguments[0].intValue();

			if(int1 < 0 || int1 >= MAXFLAGS)
				return true;

			returnValue = CCD->Pawns()->GetPawnFlag(int1);
			return true;
		}
	case RGF_SM_POWERUP:
		{
			// USAGE:	PowerUp(char *AttributeName, int Amount, bool SetMaximumAmount)
			//			PowerUp(char *AttributeName, int Amount, bool SetMaximumAmount, EntityName)
			strcpy(param0, arguments[0].str());

			CPersistentAttributes *theInv;

			if(arguments.entries() > 3)
			{
				strcpy(param4, arguments[3].str());

				if(!stricmp(param4, "Player"))
				{
					theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
				}
				else
				{
					theInv = CCD->ActorManager()->Inventory(CCD->ActorManager()->GetByEntityName(param4));
				}
			}
			else
			{
				theInv = CCD->ActorManager()->Inventory(Actor);
			}

			int NewHighLimit = theInv->High(param0) + arguments[1].intValue();;

			theInv->SetHighLimit(param0, NewHighLimit);

			if(arguments[2].boolValue())
				theInv->Set(param0, NewHighLimit);

			return true;
		}
	case RGF_SM_GETPOWERUPLEVEL:
		{
			// USAGE:	int = GetPowerUpLevel(char *AttributeName)
			//			int = GetPowerUpLevel(char *AttributeName, char *EntityName)

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
				theInv = CCD->ActorManager()->Inventory(Actor);
			}

			returnValue = theInv->GetPowerUpLevel(arguments[0].str().c_str());

			return true;
		}
	case RGF_SM_ACTIVATEHUDELEMENT:
		{
			// USAGE: ActivateHudElement(char *AttributeName, bool Flag)

			CCD->HUD()->ActivateElement(arguments[0].str().c_str(), arguments[1].boolValue());
			return true;
		}
	case RGF_SM_MOVEENTITY:
		{
			// USAGE: MoveEntity(EntityName, AmountX, AmountY, AmountZ);
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

			if(arguments.entries()==2 || arguments.entries()==4)
			{
				strcpy(param4, arguments[arguments.entries()-1].str());

				if(!stricmp(param4, "Player"))
				{
					theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
				}
				else
				{
					theInv = CCD->ActorManager()->Inventory(CCD->ActorManager()->GetByEntityName(param4));
				}
			}
			else
			{
				theInv = CCD->ActorManager()->Inventory(Actor);
			}

			returnValue = (int)theInv->Add(param0);

			if(arguments.entries()>2)
			{
				theInv->SetValueLimits(param0, arguments[1].intValue(), arguments[2].intValue());
			}

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
				theInv = CCD->ActorManager()->Inventory(Actor);
			}

			theInv->SetValueLimits(arguments[0].str().c_str(), arguments[1].intValue(), arguments[2].intValue());

			return true;
		}
	case RGF_SM_NEARESTPOINT:
		{
			// USAGE:	NearestPoint(float MinDistance, float MaxDistance)
			PARMCHECK(2);

			returnValue = false;

			geEntity_EntitySet *pSet;
			geEntity *pEntity;
			// Ok, check to see if there are ScriptPoints in this world
			pSet = geWorld_GetEntitySet(CCD->World(), "ScriptPoint");

			if(!pSet)
				return true;

			geVec3d Pos, Dist;
			float minDist2, MinDistance2, MaxDistance2;
			float newDist2;

			MinDistance2 = arguments[0].floatValue()*arguments[0].floatValue();
			MaxDistance2 = arguments[1].floatValue()*arguments[1].floatValue();
			minDist2 = MaxDistance2;

			CCD->ActorManager()->GetPosition(Actor,	&Pos);

			// Ok, we have ScriptPoints somewhere.  Dig through 'em all.
			for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
				pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
			{
				ScriptPoint *pSource = static_cast<ScriptPoint*>(geEntity_GetUserData(pEntity));

				//calc new dist
				geVec3d_Subtract(&pSource->origin, &Pos, &Dist);
				newDist2 = geVec3d_LengthSquared(&Dist);

				// point is too close
				if(newDist2<MinDistance2 || newDist2>MaxDistance2)
					continue;

				if(newDist2<minDist2)
				{
					// probably slowest test...
					if(CCD->Pawns()->CanSeePoint(FOV, Actor, &(pSource->origin), FOVBone))
					{
						minDist2 = newDist2;
						strcpy(Point, pSource->szEntityName);
						CurrentPoint= pSource->origin;
						ValidPoint = true;
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

			FOV = (180.f - arguments[0].floatValue())/180.f;
			FOVBone[0] = '\0';
			if(arguments.entries()>1)
				strcpy(FOVBone, arguments[1].str());

			return true;
		}
	case RGF_SM_SIN:
		{
			PARMCHECK(1);
			returnValue = sin(arguments[0].floatValue());

			return true;
		}
	case RGF_SM_COS:
		{
			PARMCHECK(1);
			returnValue = cos(arguments[0].floatValue());

			return true;
		}
	case RGF_SM_TAN:
		{
			PARMCHECK(1);
			returnValue = tan(arguments[0].floatValue());

			return true;
		}
	case RGF_SM_ASIN:
		{
			PARMCHECK(1);
			returnValue = asin(arguments[0].floatValue());

			return true;
		}
	case RGF_SM_ACOS:
		{
			PARMCHECK(1);
			returnValue = acos(arguments[0].floatValue());

			return true;
		}
	case RGF_SM_ATAN:
		{
			PARMCHECK(1);
			returnValue = atan(arguments[0].floatValue());

			return true;
		}
	case RGF_SM_SETGRAVITY:
		{
			PARMCHECK(3);
			geVec3d vec = { arguments[0].floatValue(), arguments[1].floatValue(), arguments[2].floatValue() };
			CCD->Player()->SetGravity(&vec);
			return true;
		}
	case RGF_SM_GETGRAVITYX:
		{
			returnValue = CCD->Player()->GetGravity().X;
			return true;
		}
	case RGF_SM_GETGRAVITYY:
		{
			returnValue = CCD->Player()->GetGravity().Y;
			return true;
		}
	case RGF_SM_GETGRAVITYZ:
		{
			returnValue = CCD->Player()->GetGravity().Z;
			return true;
		}
	case RGF_SM_SETWIND:
		{
			PARMCHECK(3);
			geVec3d vec = { arguments[0].floatValue(), arguments[1].floatValue(), arguments[2].floatValue() };
			CCD->Player()->SetWind(&vec);
			return true;
		}
	case RGF_SM_GETWINDX:
		{
			returnValue = CCD->Player()->GetWind().X;
			return true;
		}
	case RGF_SM_GETWINDY:
		{
			returnValue = CCD->Player()->GetWind().Y;
			return true;
		}
	case RGF_SM_GETWINDZ:
		{
			returnValue = CCD->Player()->GetWind().Z;
			return true;
		}
	case RGF_SM_SETWINDBASE:
		{
			PARMCHECK(3);
			geVec3d vec = { arguments[0].floatValue(), arguments[1].floatValue(), arguments[2].floatValue() };
			CCD->Player()->SetInitialWind(&vec);
			return true;
		}
	case RGF_SM_GETWINDBASEX:
		{
			returnValue = CCD->Player()->GetInitialWind().X;
			return true;
		}
	case RGF_SM_GETWINDBASEY:
		{
			returnValue = CCD->Player()->GetInitialWind().Y;
			return true;
		}
	case RGF_SM_GETWINDBASEZ:
		{
			returnValue = CCD->Player()->GetInitialWind().Z;
			return true;
		}
	case RGF_SM_SETWINDGENERATOR:
		{
			PARMCHECK(1);
			CCD->WindGenerator()->SetEnabled(arguments[0].boolValue());
			return true;
		}
	case RGF_SM_SETFORCEENABLED:
		{
			PARMCHECK(1);
			CCD->ActorManager()->SetForceEnabled(Actor, arguments[0].boolValue());
			return true;
		}
	case RGF_SM_ISINLIQUID:
		{
			int nZoneType;
			CCD->ActorManager()->GetActorZone(Actor, &nZoneType);

			if(nZoneType == kLiquidZone)
				returnValue = true;
			else
				returnValue = false;

			return true;
		}
	case RGF_SM_GETLIQUID:
		{
			int nZoneType;
			CCD->ActorManager()->GetActorZone(Actor, &nZoneType);

			if(nZoneType == kLiquidZone)
				returnValue = skString(CCD->ActorManager()->GetLiquid(Actor)->szEntityName);
			else
				returnValue = skString("");

			return true;
		}
	case RGF_SM_ENDSCRIPT:
		{
			alive=false;
			return true;
		}
	default:
		{
			return skScriptedExecutable::method(methodName, arguments, returnValue, ctxt);//change simkin
		}
	} // switch

	return true;
}

/* ------------------------------------------------------------------------------------ */
//	GetAngles
/* ------------------------------------------------------------------------------------ */
void ScriptedObject::GetAngles(bool flag)
{
	geVec3d Pos, Orient, Play;

	Play = LastTargetPoint;
	CCD->ActorManager()->GetPosition(Actor, &Pos);
	geVec3d_Subtract(&Play, &Pos, &Orient);
	float l = geVec3d_Length(&Orient);

	if(l > 0.0f)
	{
		float x = Orient.X;
		Orient.X = GE_PIOVER2 - acos(Orient.Y / l);
		Orient.Y = atan2(x, Orient.Z) + GE_PI;
	}
	{

		CCD->ActorManager()->GetRotate(Actor, &Pos);

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

	actoryaw = Pos.Y;
	targetyaw = Orient.Y;
	actorpitch = Pos.X;
	targetpitch = -Orient.X;
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

	geActor_GetExtBox(Actor, &theBox);
	Start.Y = theBox.Min.Y;
	Stop.Y = theBox.Min.Y - 1.0f;
	bool flag = false;

	for(x=0; x<=1; ++x)
	{
		for(y=0; y<=1; ++y)
		{
			Start.X = x ? theBox.Max.X : theBox.Min.X;
			Start.Z = y ? theBox.Max.Z : theBox.Min.Z;
			trace = CCD->Collision()->Trace(&Start, &Stop, NULL, NULL, Actor, &Collision);

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

	CCD->ActorManager()->GetStepHeight(Actor, &MaxStep);
	Start.Y = theBox.Min.Y;

	Start.X = Stop.X = (theBox.Min.X + theBox.Max.X)*0.5f;
	Start.Z = Stop.Z = (theBox.Min.Z + theBox.Max.Z)*0.5f;
	Stop.Y = Start.Y - (2.0f*MaxStep);
	trace = CCD->Collision()->Trace(&Start, &Stop, NULL, NULL, Actor, &Collision);

	if(trace.fraction == 1.0f)
		return false;

	mid = bottom = trace.endpos.Y;

	for(x=0; x<=1; ++x)
	{
		for(y=0; y<=1; ++y)
		{
			Start.X = Stop.X = x ? theBox.Max.X : theBox.Min.X;
			Start.Z = Stop.Z = y ? theBox.Max.Z : theBox.Min.Z;

			trace = CCD->Collision()->Trace(&Start, &Stop, NULL, NULL, Actor, &Collision);

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

	CCD->ActorManager()->GetPosition(Actor, &oldorg);
	geVec3d_Add(&oldorg, &move, &neworg);
	CCD->ActorManager()->GetStepHeight(Actor, &stepsize);
	neworg.Y += stepsize;
	end = neworg;
	end.Y -= stepsize*2.0f;
	CCD->ActorManager()->GetBoundingBox(Actor, &theBox);
	trace = CCD->Collision()->Trace(&neworg, &end, &theBox.Min, &theBox.Max, Actor, &Collision);

	if(trace.allsolid)
	{
		return false;
	}

	if(trace.startsolid)
	{
		neworg.Y -= stepsize;
		trace = CCD->Collision()->Trace(&neworg, &end, &theBox.Min, &theBox.Max, Actor, &Collision);

		if(trace.allsolid || trace.startsolid)
			return false;
	}

	if(trace.fraction == 1.0f)
	{
		return false;
	}

	geVec3d_Add(&oldorg, &move, &neworg);
	CCD->ActorManager()->ValidateMove(oldorg, trace.endpos, Actor, false);

	if(!CheckBottom())
	{
		CCD->ActorManager()->Position(Actor, oldorg);
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
	float idealangle = anglemod(ideal_yaw/0.0174532925199433f)*0.0174532925199433f;
	float RotateAmt = actoryaw - idealangle;

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

			if(RotateLeft)
				RotateLeft = false;
			else
				RotateLeft = true;
		}

		RotateAmt *= GE_180OVERPI;

		float amount = yaw_speed * ElapseTime;

		if(amount > RotateAmt)
		{
			amount = RotateAmt;
		}

		if(RotateLeft)
			CCD->ActorManager()->TurnRight(Actor, 0.0174532925199433f*amount);
		else
			CCD->ActorManager()->TurnLeft(Actor, 0.0174532925199433f*amount);
	}
}

/* ------------------------------------------------------------------------------------ */
//	ChangePitch
/* ------------------------------------------------------------------------------------ */
void ScriptedObject::ChangePitch()
{
	GetAngles(true);
	bool RotateUp = false;
	float idealangle = anglemod(ideal_pitch/0.0174532925199433f)*0.0174532925199433f;
	float RotateAmt = actorpitch - idealangle;

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

			if(RotateUp)
				RotateUp = false;
			else
				RotateUp = true;
		}

		RotateAmt *= GE_180OVERPI;

		float amount = pitch_speed * ElapseTime;

		if(amount > RotateAmt)
		{
			amount = RotateAmt;
		}

		if(RotateUp)
			CCD->ActorManager()->TiltUp(Actor, 0.0174532925199433f*amount);
		else
			CCD->ActorManager()->TiltDown(Actor, 0.0174532925199433f*amount);
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

	ideal_yaw = 0.0174532925199433f*yaw;
	ChangeYaw();

	CCD->ActorManager()->GetPosition(Actor, &oldorigin);

	geXForm3d_SetYRotation(&Xform, ideal_yaw);

	geXForm3d_GetIn(&Xform, &In);
	geVec3d_Scale(&In, dist, &move);

	if(MoveStep(move))
	{
		GetAngles(true);
		float selfangle = anglemod(actoryaw*GE_180OVERPI);// /0.0174532925199433f);
		float idealangle = anglemod(ideal_yaw*GE_180OVERPI);// /0.0174532925199433f);
		float delta = anglemod(selfangle - idealangle);

		if(!(delta > 315.f || delta < 45.f))
		{
			CCD->ActorManager()->Position(Actor, oldorigin);
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

	float yaw = ideal_yaw*GE_180OVERPI;// /0.0174532925199433f;
	olddir = anglemod((int)(yaw/45.0f)*45.0f);
	turnaround = anglemod(olddir - 180.0f);

	CCD->ActorManager()->GetPosition(Actor, &Pos);
	deltax = LastTargetPoint.X - Pos.X;
	deltay = LastTargetPoint.Z - Pos.Z;

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

	ideal_yaw = olddir*0.0174532925199433f;
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
	CCD->ActorManager()->GetBoundingBox(TargetActor, &theBox);
	CCD->ActorManager()->GetBoundingBox(Actor, &theBox1);
	float delta = CCD->ActorManager()->DistanceFrom(LastTargetPoint, Actor) - theBox.Max.X - theBox1.Max.X;

	if(delta < dist)
		return true;

	return false;
}

/* ------------------------------------------------------------------------------------ */
//	MoveToGoal
/* ------------------------------------------------------------------------------------ */
void ScriptedObject::MoveToGoal(float dist)
{
	dist = dist*ElapseTime;

	if(CloseEnough(dist))
		return;

	if(!StepDirection(ideal_yaw/0.0174532925199433f, dist))
	{
		NewChaseDir(dist);
	}
}
/* ------------------------------------------------------------------------------------ */
// end Quake2 MoveToGoal
/* ------------------------------------------------------------------------------------ */


/* ------------------------------------------------------------------------------------ */
//	TickLow
/* ------------------------------------------------------------------------------------ */
void CPawn::TickLow(Pawn *pSource, ScriptedObject *Object, float dwTicks)
{
	skRValueArray args;
	skRValue ret;

	if(Object->console)
	{
		char szBug[256];
		sprintf(szBug, "%s %s",Object->szName, Object->Order);
		strcpy(Object->ConsoleHeader, szBug);
	}

	dwTicks*=0.001f;
	Object->lowTime += dwTicks;
	Object->ElapseTime += dwTicks;
	Object->ThinkTime -= dwTicks;

	if(Object->ThinkTime > 0.0f)
		return;

	strcpy(Object->thinkorder, Object->Order);

	try
	{
		Object->method(skString(Object->Order), args, ret,CCD->GetskContext()); //change simkin
	}
	catch(skRuntimeException & e)
	{
		if(!strcmp(Object->Indicate, "+"))
			strcpy(Object->Indicate, " ");
		else
			strcpy(Object->Indicate, "+");
		if(Object->console)
			strcpy(Object->ConsoleError, e.toString());
	}
	catch(skParseException & e)
	{
		if(!strcmp(Object->Indicate, "+"))
			strcpy(Object->Indicate, " ");
		else
			strcpy(Object->Indicate, "+");
		if(Object->console)
			strcpy(Object->ConsoleError, e.toString());
	}
	catch(...)
	{
		char szBug[256];
		sprintf(szBug, "Low Level Script Error for %s in Order %s", Object->szName, Object->Order);
		CCD->ReportError(szBug, false);
		if(Object->console)
			strcpy(Object->ConsoleError, szBug);
	}

	strcpy(Object->Order, Object->thinkorder);
	Object->ElapseTime = 0.0f;
}

/* ----------------------------------- END OF FILE ------------------------------------ */
