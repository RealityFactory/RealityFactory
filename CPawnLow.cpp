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
#include "Simkin\\skScriptedExecutable.h"
#include "Simkin\\skRValue.h"
#include "Simkin\\skRValueArray.h"
#include "Simkin\\skRuntimeException.h"
#include "Simkin\\skParseException.h"


extern geSound_Def *SPool_Sound(char *SName);

/* ------------------------------------------------------------------------------------ */
//	get a variable's value
/* ------------------------------------------------------------------------------------ */
bool ScriptedObject::getValue(const skString &fieldName,
							  const skString &attribute,
							  skRValue &value)
{
	skString sval;

	if(fieldName == "time")
	{
		value = lowTime;
		return true;
	}
	else if(fieldName == "ThinkTime")
	{
		value = ThinkTime;
		return true;
	}
	else if(fieldName == "DifficultyLevel")
	{
		value = CCD->GetDifficultLevel();
		return true;
	}
	else if(fieldName == "EntityName")
	{
		value = skString(szName);
		return true;
	}
	else if(fieldName == "health")
	{
		CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(Actor);

		if(!EffectC_IsStringNull(Attribute))
			value = theInv->Value(Attribute);
		else
			value = -1;

		return true;
	}
	else if(fieldName == "attack_finished")
	{
		value = attack_finished;
		return true;
	}
	else if(fieldName == "attack_state")
	{
		value = attack_state;
		return true;
	}
	else if(fieldName == "enemy_vis")
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
	else if(fieldName == "enemy_infront")
	{
		GetAngles(true);
		value = false;

		if(actoryaw == targetyaw)
			value = true;

		return true;
	}
	else if(fieldName == "enemy_range")
	{
		geExtBox theBox;
		CCD->ActorManager()->GetBoundingBox(TargetActor, &theBox);
		value = CCD->ActorManager()->DistanceFrom(LastTargetPoint, Actor) - theBox.Max.X;
		return true;
	}
	else if(fieldName == "player_range")
	{
		geExtBox theBox;
		CCD->ActorManager()->GetBoundingBox(CCD->Player()->GetActor(), &theBox);
		value = CCD->ActorManager()->DistanceFrom(CCD->Player()->Position(), Actor) - theBox.Max.X;
		return true;
	}
	else if(fieldName == "enemy_yaw")
	{
		GetAngles(true);
		value = targetyaw;
		return true;
	}
	else if(fieldName == "last_enemy_yaw")
	{
		GetAngles(false);
		value = targetyaw;
		return true;
	}
	else if(fieldName == "enemy_pitch")
	{
		GetAngles(true);
		value = targetpitch;
		return true;
	}
	else if(fieldName == "last_enemy_pitch")
	{
		GetAngles(false);
		value = targetpitch;
		return true;
	}
	else if(fieldName == "last_enemy_range")
	{
		value = CCD->ActorManager()->DistanceFrom(LastTargetPoint, Actor);
		return true;
	}
	else if(fieldName == "current_yaw")
	{
		GetAngles(true);
		value = actoryaw;
		return true;
	}
	else if(fieldName == "yaw_speed")
	{
		value = yaw_speed;
		return true;
	}
	else if(fieldName == "ideal_yaw")
	{
		value = ideal_yaw;
		return true;
	}
	else if(fieldName == "current_pitch")
	{
		GetAngles(true);
		value = actorpitch;
		return true;
	}
	else if(fieldName == "pitch_speed")
	{
		value = pitch_speed;
		return true;
	}
	else if(fieldName == "ideal_pitch")
	{
		value = ideal_pitch;
		return true;
	}
	else if(fieldName == "in_pain")
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
	else if(fieldName == "animate_at_end")
	{
		value = CCD->ActorManager()->EndAnimation(Actor);
		return true;
	}
	else if (fieldName == "IsFalling")
	{
		value = false;

// changed QD 07/15/06 - add additional ReallyFall check
		if(CCD->ActorManager()->Falling(Actor)==GE_TRUE)
			if(CCD->ActorManager()->ReallyFall(Actor) == RGF_SUCCESS)
				value = true;
// end change
		return true;
	}
	else if(fieldName == "current_X")
	{
		geVec3d Pos;
		CCD->ActorManager()->GetPosition(Actor, &Pos);
		value = Pos.X;
		return true;
	}
	else if(fieldName == "current_Y")
	{
		geVec3d Pos;
		CCD->ActorManager()->GetPosition(Actor, &Pos);
		value = Pos.Y;
		return true;
	}
	else if(fieldName == "current_Z")
	{
		geVec3d Pos;
		CCD->ActorManager()->GetPosition(Actor, &Pos);
		value = Pos.Z;
		return true;
	}
// Added By Pickles to RF07D --------------------------
	else if(fieldName == "player_Z")
	{
		geVec3d Pos;
		CCD->ActorManager()->GetPosition(CCD->Player()->GetActor(), &Pos);
		value = Pos.Z;
		return true;
	}
	else if(fieldName == "player_Y")
	{
		geVec3d Pos;
		CCD->ActorManager()->GetPosition(CCD->Player()->GetActor(), &Pos);
		value = Pos.Y;
		return true;
	}
	else if(fieldName == "player_X")
	{
		geVec3d Pos;
		CCD->ActorManager()->GetPosition(CCD->Player()->GetActor(), &Pos);
		value = Pos.X;
		return true;
	}
	else if(fieldName == "distancetopoint")
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
// pickles Jul 04
	else if(fieldName == "playertopoint")
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
//pickles Jul 04
	else if(fieldName == "key_pressed")
	{
		value = CCD->Input()->GetKeyboardInputNoWait();
		return true;
	}
	else if(fieldName == "player_vis")
	{
		GetAngles(true);
		bool flag = CCD->Pawns()->CanSee(FOV, Actor, CCD->Player()->GetActor(), FOVBone);
		value = flag;
		return true;
	}
	else if(fieldName == "target_name")
	{
		if(CCD->ActorManager()->IsActor(TargetActor))
			value = skString(CCD->ActorManager()->GetEntityName(TargetActor));
		else
			value = false;

		return true;
	}
	else if(fieldName == "enemy_X")
	{
		geVec3d Pos;
		CCD->ActorManager()->GetPosition(TargetActor, &Pos);
		value = Pos.X;
		return true;
	}
	else if(fieldName == "enemy_Y")
	{
		geVec3d Pos;
		CCD->ActorManager()->GetPosition(TargetActor, &Pos);
		value = Pos.Y;
		return true;
	}
	else if(fieldName == "enemy_Z")
	{
		geVec3d Pos;
		CCD->ActorManager()->GetPosition(TargetActor, &Pos);
		value = Pos.Z;
		return true;
	}
	else if(fieldName == "player_yaw")
	{
		geVec3d Orient;
		CCD->ActorManager()->GetRotation(CCD->Player()->GetActor(), &Orient);
		value = Orient.Y;
		return true;
	}
	else if(fieldName == "point_vis")
	{
		bool flag = CCD->Pawns()->CanSeePoint(FOV, Actor, &CurrentPoint, FOVBone);
		value = flag;
		return true;
	}
	else if(fieldName == "player_weapon")
	{
		value = CCD->Weapons()->GetCurrent();
		return true;
	}
	else if(fieldName == "point_name")
	{
		if(!Point)
		{
			value=skString("FALSE");
			return true;
		}

		value = skString(Point);
		return true;
	}
	else if(fieldName == "camera_pitch")
	{
		value = CCD->CameraManager()->GetTilt();
		return true;
	}
// END Added By Pickles ----------------------------------------
	else if(fieldName == "LODLevel")
	{
		int Level = 0;
		CCD->ActorManager()->GetLODLevel(Actor, &Level);
		value = Level;
		return true;
	}
// changed Nout 12/15/05
	// Gets the ScreenX position of the active Pawn
	else if(fieldName == "current_screen_X")
	{
		geVec3d Pos, ScreenPos;
		CCD->ActorManager()->GetPosition(Actor, &Pos);
		geCamera_Transform(CCD->CameraManager()->Camera(), &Pos, &ScreenPos);
		value = int((CCD->Engine()->Width()/2 -1) * (1.0f - ScreenPos.X / ScreenPos.Z));
		return true;
	}
	// Gets the ScreenY position of the active Pawn
	else if(fieldName == "current_screen_Y")
	{
		geVec3d Pos, ScreenPos;
		CCD->ActorManager()->GetPosition(Actor, &Pos);
		geCamera_Transform(CCD->CameraManager()->Camera(), &Pos, &ScreenPos);
		value = int((CCD->Engine()->Height()/2 - 1) * (1.0f + ScreenPos.Y / ScreenPos.Z));
		return true;
	}
	// Gets the ScreenX position of the player
	else if(fieldName == "player_screen_X")
	{
		geVec3d Pos, ScreenPos;
		Pos = CCD->Player()->Position();
		geCamera_Transform(CCD->CameraManager()->Camera(), &Pos, &ScreenPos);
		value = int((CCD->Engine()->Width()/2 - 1) * (1.0f - ScreenPos.X / ScreenPos.Z));
		return true;
	}
	// Gets the ScreenY position of the player
	else if(fieldName == "player_screen_Y")
	{
		geVec3d Pos, ScreenPos;
		Pos = CCD->Player()->Position();
		geCamera_Transform(CCD->CameraManager()->Camera(), &Pos, &ScreenPos);
		value = int((CCD->Engine()->Height()/2 - 1) * (1.0f + ScreenPos.Y / ScreenPos.Z));
		return true;
	}
	// Check if left mouse button is pressed
	else if(fieldName == "lbutton_pressed")
	{
		int SwapButton = GetSystemMetrics(SM_SWAPBUTTON);

		if(SwapButton)
		{
			value = (GetAsyncKeyState(VK_RBUTTON) < 0);
		}
		else
			value = (GetAsyncKeyState(VK_LBUTTON) < 0);

		return true;
	}
	// Check if right mouse button is pressed
	else if(fieldName == "rbutton_pressed")
	{
		int SwapButton = GetSystemMetrics(SM_SWAPBUTTON);

		if(SwapButton)
		{
			value = (GetAsyncKeyState(VK_LBUTTON) < 0);
		}
		else
			value = (GetAsyncKeyState(VK_RBUTTON) < 0);

		return true;
	}
	// Check if middle mouse button is pressed
	else if(fieldName == "mbutton_pressed")
	{
		value = (GetAsyncKeyState(VK_MBUTTON) < 0);
		return true;
	}
// end change by Nout
// changed AndyCR 07/15/06
	else if(fieldName == "player_animation")
	{
		value = skString(CCD->ActorManager()->GetMotion(CCD->Player()->GetActor()));
		return true;
	}
// end change AndyCR 07/15/06
// changed QD 07/15/06
	else if(fieldName == "player_viewpoint")
	{
		value = CCD->Player()->GetViewPoint();
		return true;
	}
// end change QD 07/15/06
	else
	{
		return skScriptedExecutable::getValue(fieldName, attribute, value);
	}

}

/* ------------------------------------------------------------------------------------ */
//	sets a variable's value
/* ------------------------------------------------------------------------------------ */
bool ScriptedObject::setValue(const skString &fieldName,
							  const skString &attribute,
							  const skRValue &value)
{
	if(fieldName == "lowTime")
	{
		lowTime = value.floatValue();
		return true;
	}
	else if(fieldName == "ThinkTime")
	{
		ThinkTime = value.floatValue();
		return true;
	}
	else if(fieldName == "think")
	{
		strcpy(thinkorder, value.str());
		return true;
	}
	else if(fieldName == "attack_finished")
	{
		attack_finished = value.floatValue();
		return true;
	}
	else if(fieldName == "attack_state")
	{
		attack_state = value.intValue();
		return true;
	}
	else if(fieldName == "yaw_speed")
	{
		yaw_speed = value.floatValue();
		return true;
	}
	else if(fieldName == "ideal_yaw")
	{
		ideal_yaw = value.floatValue();
		return true;
	}
	else if(fieldName == "pitch_speed")
	{
		pitch_speed = value.floatValue();
		return true;
	}
	else if(fieldName == "ideal_pitch")
	{
		ideal_pitch = value.floatValue();
		return true;
	}
	else
	{
		return skScriptedExecutable::setValue(fieldName, attribute, value);
	}
}

/* ------------------------------------------------------------------------------------ */
//	calls a method in this object
/* ------------------------------------------------------------------------------------ */
bool ScriptedObject::lowmethod(const skString &methodName, skRValueArray &arguments,
							   skRValue &returnValue, skExecutableContext &ctxt)
{

	char param0[128], param4[128], param5[128]; // pickles Jul 04
	bool param2;
	float param1, param3;
	param0[0] = '\0';
	param4[0] = '\0';
	param5[0] = '\0'; // pickles Jul 04

	strcpy(param0, methodName);//change scripting
	long ComCall = CCD->GetHashCommand(param0);//change scripting

	//open switch
	switch(ComCall)
	{
	case 0:
		{
			return skScriptedExecutable::method(methodName, arguments, returnValue,ctxt);//change simkin
		}
	case 1:	// if (IS_METHOD(methodName, "HighLevel"))
		{
			//PARMCHECK(1);
			strcpy(param0, arguments[0].str());
			highlevel = true;
			strcpy(thinkorder, param0);
			RunOrder = true;
			ActionActive = false;
			return true;
		}
	case 2:	// else if (IS_METHOD(methodName, "Animate"))
		{
			//PARMCHECK(1);
			strcpy(param0, arguments[0].str());
			CCD->ActorManager()->SetMotion(Actor, param0);
			CCD->ActorManager()->SetHoldAtEnd(Actor, false);
			return true;
		}
	case 3:	// else if (IS_METHOD(methodName, "Gravity"))
		{
			PARMCHECK(1);
			bool flag = arguments[0].boolValue();
			float Gravity = 0.0f;

			if(flag)
				Gravity = CCD->Player()->GetGravity();
			CCD->ActorManager()->SetGravity(Actor, Gravity);
			return true;
		}
	case 4:	// else if (IS_METHOD(methodName, "PlaySound"))
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
	case 5:	// else if (IS_METHOD(methodName, "EnemyExist"))
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
	case 6:	// else if (IS_METHOD(methodName, "GetEventState"))
		{
			PARMCHECK(1);
			strcpy(param0, arguments[0].str());
			returnValue = (bool)GetTriggerState(param0);
			return true;
		}
	case 7:	// else if (IS_METHOD(methodName, "Integer"))
		{
			PARMCHECK(1);
			int temp = arguments[0].intValue();
			returnValue = (int)temp;
			return true;
		}
	case 8:	// else if (IS_METHOD(methodName, "GetAttribute"))
		{
			// USAGE:	GetAttribute(char *Attribute)
			//			GetAttribute(char *Attribute, char *EntityName)
			// PARMCHECK(1);
			strcpy(param0, arguments[0].str());

// changed QD 12/15/05
			//CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
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
				theInv = CCD->ActorManager()->Inventory(Actor);
// end change
			returnValue = (int)theInv->Value(param0);
			return true;
		}
// changed QD 01/2004
	case 9:	// else if (IS_METHOD(methodName, "GetCurFlipBook"))
		{
			PARMCHECK(1);
			strcpy(param0, arguments[0].str());
			FlipBook *pEntityData=NULL;
// changed QD 04/23/2004
			if(CCD->FlipBooks()->LocateEntity(param0, (void**)&pEntityData) == RGF_SUCCESS)
// end change
				returnValue = (int)pEntityData->CurTex;
			else
				returnValue = 0;

			return true;
		}
// end change
	case 10:	// else if (IS_METHOD(methodName, "ModifyAttribute"))
		{
			PARMCHECK(2);
			strcpy(param0, arguments[0].str());

// changed QD 12/15/05
			//CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
			CPersistentAttributes *theInv;

			if(arguments.entries() > 2)
			{
				strcpy(param4, arguments[2].str());

				if(!stricmp(param4, "Player"))
					theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
				else
					theInv = CCD->ActorManager()->Inventory(CCD->ActorManager()->GetByEntityName(param4));
			}
			else
				theInv = CCD->ActorManager()->Inventory(Actor);
// end change

			returnValue = (int)theInv->Modify(param0, arguments[1].intValue());
			return true;
		}
// Added By Pickles to RF07D --------------------------
	case 11:	// else if (IS_METHOD(methodName, "SetAttribute"))
		{
			PARMCHECK(2);
			strcpy(param0, arguments[0].str());

// changed QD 12/15/05
			//CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
			CPersistentAttributes *theInv;

			if(arguments.entries() > 2)
			{
				strcpy(param4, arguments[2].str());

				if(!stricmp(param4, "Player"))
					theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
				else
					theInv = CCD->ActorManager()->Inventory(CCD->ActorManager()->GetByEntityName(param4));
			}
			else
				theInv = CCD->ActorManager()->Inventory(Actor);
// end change

			returnValue = (int)theInv->Set(param0, arguments[1].intValue());
			return true;
		}
	case 12:	// else if (IS_METHOD(methodName, "SetPlayerWeapon"))
		{
			PARMCHECK(1);
			int temp = arguments[0].intValue();
			// changed QD 07/15/06
			if(temp<0)
				CCD->Weapons()->Holster();
			else
				CCD->Weapons()->SetWeapon(temp);
			// end change
			return true;
		}
	case 13:	// else if (IS_METHOD(methodName, "SetUseItem"))
		{
			PARMCHECK(1);
			strcpy(param0, arguments[0].str());
			CCD->Player()->SetUseAttribute(param0);
			CCD->HUD()->ActivateElement(param0,true);
			return true;
		}
	case 14:	// else if (IS_METHOD(methodName, "ClearUseItem"))
		{
			PARMCHECK(1);
			strcpy(param0, arguments[0].str());
			CCD->HUD()->ActivateElement(param0,false);
			CCD->Player()->DelUseAttribute(param0);
			return true;
		}
	case 15:	// else if (IS_METHOD(methodName, "StringCopy"))
		{
			// PARMCHECK(1);
			strcpy(param0, arguments[0].str());
			returnValue = skString(param0);
			return true;
		}
	case 16:	// else if (IS_METHOD(methodName, "LeftCopy"))
		{
			// PARMCHECK(2);
			int temp = arguments[1].intValue();
			// changed QD 07/15/06
			/*
			char* cstemp="";
			strncpy(cstemp,arguments[0].str(),temp);
			cstemp[temp]='\0';
			returnValue = skString(cstemp);
			*/
			temp = (temp<127)?temp:127;
			strncpy(param0, arguments[0].str(), temp);
			param0[temp] = 0;
			returnValue = skString(param0);
			// end change
			return true;
		}
	case 17:	// else if (IS_METHOD(methodName, "IsEntityVisible"))
		{
			PARMCHECK(1);
			strcpy(param0, arguments[0].str());
			bool flag = CCD->Pawns()->CanSee(FOV, Actor,CCD->ActorManager()->GetByEntityName(param0), FOVBone);
			returnValue = flag;
			return true;
		}
	case 18:	// else if (IS_METHOD(methodName, "DamageEntity"))
		{
			// PARMCHECK(3);
			float amount = arguments[0].floatValue();
			strcpy(param0, arguments[1].str());
			strcpy(param4, arguments[2].str());
			CCD->Damage()->DamageActor(CCD->ActorManager()->GetByEntityName(param4), amount, param0, amount, param0, "Melee");
			return true;
		}
	case 19:	// else if (IS_METHOD(methodName, "AnimateEntity"))
		{
			PARMCHECK(3);
			strcpy(param0, arguments[0].str());
			strcpy(param4, arguments[1].str());
			param2 = arguments[2].boolValue();
			CCD->ActorManager()->SetMotion(CCD->ActorManager()->GetByEntityName(param4), param0);
			CCD->ActorManager()->SetHoldAtEnd(CCD->ActorManager()->GetByEntityName(param4), param2);
			return true;
		}
	case 20:	// else if (IS_METHOD(methodName, "AnimateHold"))
		{
			// PARMCHECK(1);
			strcpy(param0, arguments[0].str());
			CCD->ActorManager()->SetMotion(Actor, param0);
			CCD->ActorManager()->SetHoldAtEnd(Actor, true);
			return true;
		}
	case 21:	// else if (IS_METHOD(methodName, "AnimateTarget"))
		{
			PARMCHECK(2);
			strcpy(param0, arguments[0].str());
			param2 = arguments[1].boolValue();
			CCD->ActorManager()->SetMotion(TargetActor, param0);
			CCD->ActorManager()->SetHoldAtEnd(TargetActor, param2);
			return true;
		}
	case 22:	// else if (IS_METHOD(methodName, "GetEntityX"))
		{
			PARMCHECK(1);
			strcpy(param0, arguments[0].str());
			geVec3d Pos;
			CCD->ActorManager()->GetPosition(CCD->ActorManager()->GetByEntityName(param0), &Pos);
			returnValue = Pos.X;
			return true;
		}
	case 23:	// else if (IS_METHOD(methodName, "GetEntityY"))
		{
			PARMCHECK(1);
			strcpy(param0, arguments[0].str());
			geVec3d Pos;
			CCD->ActorManager()->GetPosition(CCD->ActorManager()->GetByEntityName(param0), &Pos);
			returnValue = Pos.Y;
			return true;
		}
	case 24:	// else if (IS_METHOD(methodName, "GetEntityZ"))
		{
			PARMCHECK(1);
			strcpy(param0, arguments[0].str());
			geVec3d Pos;
			CCD->ActorManager()->GetPosition(CCD->ActorManager()->GetByEntityName(param0), &Pos);
			returnValue = Pos.Z;
			return true;
		}
	case 25:	// else if (IS_METHOD(methodName, "IsKeyDown"))
		{
// Changed RF071A Picklkes
			PARMCHECK(1);
			int temp = arguments[0].intValue();
			returnValue = false;

			if(CCD->Input()->GetKeyCheck(temp) == true)
				returnValue = true;

			return true;
		}
	case 26:	// else if (IS_METHOD(methodName, "GetEntityYaw"))
		{
			PARMCHECK(1);
			strcpy(param0, arguments[0].str());
			geVec3d Orient;
			geActor *MasterActor;
			strcpy(param0, arguments[0].str());
			MasterActor = CCD->ActorManager()->GetByEntityName(param0);

			if(!MasterActor)
				return true;

			CCD->ActorManager()->GetRotate(MasterActor, &Orient);
			returnValue = Orient.Y;
			return true;
		}
	case 27:	// else if (IS_METHOD(methodName, "MatchEntityAngles"))
		{
			PARMCHECK(1);
			GetAngles(true);
			geVec3d theRotation;
			geActor *MasterActor;
			strcpy(param0, arguments[0].str());
			MasterActor = CCD->ActorManager()->GetByEntityName(param0);

			if(!MasterActor)
				return true;

			CCD->ActorManager()->GetRotate(MasterActor, &theRotation);
			CCD->ActorManager()->Rotate(Actor, theRotation);
			return true;
		}
	case 28:	// else if (IS_METHOD(methodName, "FacePoint"))
		{
			if(ValidPoint)
				CCD->ActorManager()->RotateToFacePoint(Actor,CurrentPoint);

			return true;
		}
	case 29:	// else if (IS_METHOD(methodName, "NewPoint"))
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
// ADDED TO RF071 BY PICKLES
	case 30:	// else if (IS_METHOD(methodName, "GetPointYaw"))
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
	case 31:	// else if (IS_METHOD(methodName, "NextPoint"))
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
//END ADDED TO RF071 BY PICKLES
	case 32:	// else if (IS_METHOD(methodName, "SetTarget"))
		{
			PARMCHECK(1);
			geActor *MActor;
			strcpy(param0, arguments[0].str());
			MActor = CCD->ActorManager()->GetByEntityName(param0);

			if(!MActor)
				return true;

			TargetActor = MActor;
			return true;
		}
	case 33:	// else if (IS_METHOD(methodName, "GetDistanceTo"))
		{
			PARMCHECK(1);
			strcpy(param0, arguments[0].str());
			geVec3d Pos, tPos;
			geActor *MActor;
			MActor = CCD->ActorManager()->GetByEntityName(param0);

			if(!MActor)
				return true;

			CCD->ActorManager()->GetPosition(Actor, &tPos);
			CCD->ActorManager()->GetPosition(MActor, &Pos);
			returnValue = geVec3d_DistanceBetween(&tPos, &Pos);
			return true;
		}
	case 34:	// else if (IS_METHOD(methodName, "TeleportEntity"))
		{
			// USAGE : TeleportEntity(EntityName, Point)
			// TeleportEntity(EntityName, Point, OffsetX, OffsetY, OffsetZ)
			// TeleportEntity(EntityName, Point, OffsetX, OffsetY, OffsetZ, UseAngle)
			// PARMCHECK(2);
			strcpy(param0, arguments[1].str());
			strcpy(param4, arguments[0].str());
			char *EType = CCD->EntityRegistry()->GetEntityType(param0);
			geActor *tActor;
// changed Nout 12/15/05
			if(!stricmp(param4, "Player"))
				tActor = CCD->Player()->GetActor();
			else
// end change
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

// changed Nout 12/15/05
					if(arguments.entries() > 4)
					{
						Pos.X += arguments[2].floatValue();
						Pos.Y += arguments[3].floatValue();
						Pos.Z += arguments[4].floatValue();
					}
// end change
					CCD->ActorManager()->Position(tActor, Pos);

// changed QD 12/15/05
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
// end change

// Added Pickles RF071
// changed QD 12/15/05 - we're not teleporting this scripted object!
					//if(WeaponActor)
					//	CCD->ActorManager()->Position(WeaponActor, Pos);
// end change
// End Added Pickles RF071
				}
			}

			return true;
		}
	case 35:	// else if (IS_METHOD(methodName, "SaveAttributes"))
		{
			PARMCHECK(1);
			strcpy(param0, arguments[0].str());
			CCD->Player()->SaveAttributesAscii(param0);
			return true;
		}
	case 36:	// else if (IS_METHOD(methodName, "TraceToActor"))
		{
			PARMCHECK(4);
			geXForm3d Xf;
			geVec3d OldPos, Pos, Normal, theRotation, Direction;
			geActor *pActor;
			geFloat T;
			strcpy(param0, arguments[0].str());
			CCD->ActorManager()->GetPosition(Actor, &OldPos);
			returnValue = skString("FALSE");
			bool bone;

			if(WeaponActor)
				bone = geActor_GetBoneTransform(WeaponActor, param0, &Xf);
			else
				bone = geActor_GetBoneTransform(Actor, param0, &Xf);

			if(!bone)
				return true;

			geVec3d_Copy(&(Xf.Translation), &OldPos);
			CCD->ActorManager()->GetRotate(Actor, &theRotation);
			Pos = OldPos;

			// changed QD 12/15/05
			//geXForm3d_SetIdentity(&Xf);
			//geXForm3d_RotateZ(&Xf, theRotation.Z);
			geXForm3d_SetZRotation(&Xf, theRotation.Z);
			geXForm3d_RotateX(&Xf, theRotation.X);
			geXForm3d_RotateY(&Xf, theRotation.Y);
			//geXForm3d_Translate(&Xf, Pos.X, Pos.Y, Pos.Z);
			// end change

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
	case 37:	// else if (IS_METHOD(methodName, "AnimateBlend"))
		{
			// PARMCHECK(2);
			strcpy(param0, arguments[0].str());
			param1 = arguments[1].floatValue();
			strcpy(param4, CCD->ActorManager()->GetMotion(Actor));

			if(param1 > 0.0f)
			{
				CCD->ActorManager()->SetTransitionMotion(Actor, param0, param1, NULL);
				CCD->ActorManager()->SetHoldAtEnd(Actor, true);
			}

			return true;
		}
	case 38:	// else if (IS_METHOD(methodName, "AnimationSpeed"))
		{
			// PARMCHECK(1);
			CCD->ActorManager()->SetAnimationSpeed(Actor,arguments[0].floatValue());
			return true;
		}
	case 39:	// else if (IS_METHOD(methodName, "SetCollision"))
		{
			CCD->ActorManager()->SetCollide(Actor);

			if(WeaponActor)
			{
				CCD->ActorManager()->SetCollide(WeaponActor);
				CCD->ActorManager()->SetNoCollide(WeaponActor);
			}

			return true;
		}
	case 40:	// else if (IS_METHOD(methodName, "SetNoCollision"))
		{
			CCD->ActorManager()->SetNoCollide(Actor);
			return true;
		}
	case 41:	// else if (IS_METHOD(methodName, "DamageArea"))
		{
			PARMCHECK(3);
			char* dAttrib='\0';
			geVec3d Pos;
			geFloat dAmount,dRange;
			dAmount = arguments[0].floatValue();
			dRange = arguments[1].floatValue();
			strcpy(dAttrib, arguments[2].str());
			CCD->ActorManager()->GetPosition(Actor, &Pos);
			CCD->Damage()->DamageActorInRange(Pos,dRange,dAmount,dAttrib,0.0f,"","Explosion");
			CCD->Damage()->DamageModelInRange(Pos,dRange,dAmount,dAttrib,0.0f,"");
			return true;
		}
	case 42:	// else if (IS_METHOD(methodName, "PlayerMatchAngles"))
		{
			PARMCHECK(1);
			GetAngles(true);
			geVec3d theRotation;
			geActor *MasterActor;
			strcpy(param0, arguments[0].str());
			MasterActor = CCD->ActorManager()->GetByEntityName(param0);

			if(!MasterActor)
				return true;

			CCD->ActorManager()->GetRotate(MasterActor, &theRotation);
			CCD->ActorManager()->Rotate(CCD->Player()->GetActor(), theRotation);
			return true;
		}
	case 43:	// else if (IS_METHOD(methodName, "ConvertDegrees"))
		{
			PARMCHECK(1);
			param3 = arguments[0].floatValue();
			returnValue = param3*0.0174532925199433f;
			return true;
		}
	case 44:	// else if (IS_METHOD(methodName, "AttachCamera"))
		{
			CCD->CameraManager()->BindToActor(Actor);
			return true;
		}
	case 45:	// else if (IS_METHOD(methodName, "AttachCameraToBone"))
		{
			PARMCHECK(1);
			strcpy(param0, arguments[0].str());
			CCD->CameraManager()->BindToActorBone(Actor,param0);
			return true;
		}
	case 46:	// else if (IS_METHOD(methodName, "AttachCameraToEntity"))
		{
			PARMCHECK(2);
			strcpy(param0, arguments[0].str());
			strcpy(param4, arguments[1].str());
			CCD->CameraManager()->BindToActorBone(CCD->ActorManager()->GetByEntityName(param0),param4);
			return true;
		}
	case 47:	// else if (IS_METHOD(methodName, "DetachCamera"))
		{
			CCD->CameraManager()->BindToActor(CCD->Player()->GetActor());
			return true;
		}
	case 48:	// else if (IS_METHOD(methodName, "TiltCamera"))
		{
			PARMCHECK(1);
			param3 = arguments[0].floatValue();

			if(param3 < 0)
			{
				// changed QD 12/15/05
				CCD->CameraManager()->TiltUp(-param3); //(param3*(+1));
			}
			else
			{
				CCD->CameraManager()->TiltDown(param3);
			}

			return true;
		}
	case 49:	// else if (IS_METHOD(methodName, "PositionToPlatform"))
		{
// Changed Pickles rf07A
			PARMCHECK(5);
			geVec3d Pos,theRotation;
			geXForm3d Xf;
			strcpy(param0, arguments[0].str());
			MovingPlatform *pEntity;
			CCD->Platforms()->LocateEntity(param0, (void**)&pEntity);
			CCD->ModelManager()->GetPosition(pEntity->Model,&Pos);
			CCD->ModelManager()->GetRotation(pEntity->Model,&theRotation);

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

			bool flag = arguments[4].boolValue();

			if(flag)
				CCD->ActorManager()->Rotate(Actor, theRotation);

// Added Pickles RF071
			if(WeaponActor)
				CCD->ActorManager()->Position(WeaponActor, Pos);
// End Added Pickles RF071

			return true;
// End Changed Pickles rf07A
		}
	case 50:	// else if (IS_METHOD(methodName, "ActivateTrigger"))
		{
			// PARMCHECK(1);
			strcpy(param0, arguments[0].str());
			CCD->Triggers()->HandleTriggerEvent(param0);
			return true;
		}
// Added Pickles to RF071A
	case 51:	// else if (IS_METHOD(methodName, "UpdateEnemyVis"))
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
	case 52:	// else if (IS_METHOD(methodName, "TargetPlayer"))
		{
			TargetActor = CCD->Player()->GetActor();
			return true;
		}
	case 53:	// else if (IS_METHOD(methodName, "FireProjectileBlind"))
		{
			// PARMCHECK(6);
			geXForm3d Xf;
			geVec3d Pos, theRotation, Direction, Orient, tPoint;
			char *dAttrib ="\0";
			strcpy(param0, arguments[1].str());// Bone
			strcpy(param4, arguments[0].str());// Projectile
			strcpy(dAttrib, arguments[5].str());// Attribute
			bool bone;

			if(WeaponActor)
				bone = geActor_GetBoneTransform(WeaponActor, param0, &Xf);
			else
				bone = geActor_GetBoneTransform(Actor, param0, &Xf);

			if(!bone)
				return true;

			geVec3d_Copy(&(Xf.Translation), &Pos);
			CCD->ActorManager()->GetRotate(Actor, &theRotation);

			// changed QD 12/15/05
			//geXForm3d_SetIdentity(&Xf);
			//geXForm3d_RotateZ(&Xf, theRotation.Z);
			geXForm3d_SetZRotation(&Xf, theRotation.Z);
			geXForm3d_RotateX(&Xf, theRotation.X);
			geXForm3d_RotateY(&Xf, theRotation.Y);
			//geXForm3d_Translate(&Xf, Pos.X, Pos.Y, Pos.Z);

			geXForm3d_GetIn(&Xf, &Direction);
			geVec3d_AddScaled(&Pos, &Direction, 1000.0f, &tPoint);
			geVec3d_AddScaled(&Pos, &Direction, arguments[4].floatValue(), &Pos);

			geXForm3d_GetUp(&Xf, &Direction);
			geVec3d_AddScaled(&Pos, &Direction, arguments[3].floatValue(), &Pos);

			geXForm3d_GetLeft(&Xf, &Direction);
			geVec3d_AddScaled(&Pos, &Direction, arguments[2].floatValue(), &Pos);

			//geXForm3d_GetIn(&Xf, &Direction);
			//geVec3d_AddScaled(&Pos, &Direction, arguments[4].floatValue(), &Pos);

			geVec3d_Subtract(&tPoint, &Pos, &Orient);
			float l = geVec3d_Length(&Orient);

			if(l > 0.0f)
			{
				float x = Orient.X;
				// changed QD 12/15/05
				// Orient.X = (float)(GE_PI*0.5f) - (float)acos(Orient.Y / l);
				Orient.X = GE_PIOVER2 - (float)acos(Orient.Y / l);
				Orient.Y = (float)atan2(x, Orient.Z) + GE_PI;
				Orient.Z = 0.0f;	// roll is zero - always!!?

				CCD->Weapons()->Add_Projectile(Pos, Pos, Orient, param4, dAttrib, dAttrib);
			}

			return true;
		}
	case 54:	// else if (IS_METHOD(methodName, "SetTargetPoint"))
		{
			// PARMCHECK(3);
			geXForm3d Xf;
			geVec3d Pos, theRotation, Direction;

			CCD->ActorManager()->GetPosition(Actor, &Pos);
			CCD->ActorManager()->GetRotate(Actor, &theRotation);

			// changed QD 12/15/05
			//geXForm3d_SetIdentity(&Xf);
			//geXForm3d_RotateZ(&Xf, theRotation.Z);
			geXForm3d_SetZRotation(&Xf, theRotation.Z);
			geXForm3d_RotateX(&Xf, theRotation.X);
			geXForm3d_RotateY(&Xf, theRotation.Y);
			//geXForm3d_Translate(&Xf, Pos.X, Pos.Y, Pos.Z);
			// end change

			geXForm3d_GetUp(&Xf, &Direction);
			geVec3d_AddScaled(&Pos, &Direction, arguments[1].floatValue(), &Pos);

			geXForm3d_GetLeft(&Xf, &Direction);
			geVec3d_AddScaled(&Pos, &Direction, arguments[0].floatValue(), &Pos);

			geXForm3d_GetIn(&Xf, &Direction);
			geVec3d_AddScaled(&Pos, &Direction, arguments[2].floatValue(), &Pos);

			UpdateTargetPoint = Pos;
			return true;
		}
//-------------------- End Added by Pickles -----------------------------------
// begin added pickles 042004
	case 55:	// else if (IS_METHOD(methodName, "GetBoneX"))
		{
			// USAGE: GetBoneX("ENTITY NAME","BONE NAME");
			PARMCHECK(1);
			geVec3d Pos;
			strcpy(param0, arguments[0].str());// entity name
			strcpy(param4, arguments[0].str());// bone name
			CCD->ActorManager()->GetBonePosition(CCD->ActorManager()->GetByEntityName(param0),param4,&Pos);
			returnValue = Pos.X;
			return true;
		}
	case 56:	// else if (IS_METHOD(methodName, "GetBoneY"))
		{
			// USAGE: GetBoneY("ENTITY NAME","BONE NAME");
			PARMCHECK(2);
			geVec3d Pos;
			strcpy(param0, arguments[0].str());// entity name
			strcpy(param4, arguments[0].str());// bone name
			CCD->ActorManager()->GetBonePosition(CCD->ActorManager()->GetByEntityName(param0),param4,&Pos);
// changed QD 04/28/2004
			returnValue = Pos.Y;
// end change
			return true;
		}
	case 57:	// else if (IS_METHOD(methodName, "GetBoneZ"))
		{
			// USAGE: GetBoneZ("ENTITY NAME","BONE NAME");
			PARMCHECK(2);
			geVec3d Pos;
			strcpy(param0, arguments[0].str());// entity name
			strcpy(param4, arguments[0].str());// bone name
			CCD->ActorManager()->GetBonePosition(CCD->ActorManager()->GetByEntityName(param0),param4,&Pos);
// changed QD 04/28/2004
			returnValue = Pos.Z;
// end change
			return true;
		}
	case 58:	// else if (IS_METHOD(methodName, "GetBoneYaw"))
		{
			// USAGE: GetBoneYaw("ENTITY NAME","BONE NAME");
			PARMCHECK(2);
			geVec3d Pos;
			strcpy(param0, arguments[0].str());// entity name
			strcpy(param4, arguments[0].str());// bone name
			CCD->ActorManager()->GetBoneRotation(CCD->ActorManager()->GetByEntityName(param0),param4,&Pos);
			returnValue = Pos.Y;
			return true;
		}
	case 59:	// else if (IS_METHOD(methodName, "SetPosition"))
		{
			// USAGE; SetPosition("ENTITY NAME",X, Y, Z);
			PARMCHECK(4);
// changed QD 04/28/2004
			geVec3d Pos;
			geVec3d_Set(&Pos,0.f,0.f,0.f);
			strcpy(param0, arguments[0].str());// entity name
			Pos.X = arguments[1].floatValue();// x
			Pos.Y = arguments[2].floatValue();// y
			Pos.Z = arguments[3].floatValue();// z
// end change
			CCD->ActorManager()->Position(CCD->ActorManager()->GetByEntityName(param0), Pos);
			return true;
		}
// end added pickles 042004
// start pickles Jul 04
	case 60:	// else if (IS_METHOD(methodName, "IsButtonDown"))
		{
			PARMCHECK(2);
			int a = arguments[0].intValue();
			int b = arguments[1].intValue();
			returnValue = CCD->CheckJoystickButton(a, b);
			return true;
		}
	case 61:	// else if (IS_METHOD(methodName, "GetJoyAxisX"))
		{
			PARMCHECK(1);
			int a = arguments[0].intValue();
			returnValue = CCD->PollJoystickAxis(a, 0);
			return true;
		}
	case 62:	// else if (IS_METHOD(methodName, "GetJoyAxisY"))
		{
			PARMCHECK(1);
			int a = arguments[0].intValue();
			returnValue = CCD->PollJoystickAxis(a, 1);
			return true;
		}
	case 63:	// else if (IS_METHOD(methodName, "GetJoyAxisZ"))
		{
			PARMCHECK(1);
			int a = arguments[0].intValue();
			returnValue = CCD->PollJoystickAxis(a, 2);
			return true;
		}
	case 64:	// else if (IS_METHOD(methodName, "GetNumJoySticks"))
		{
			returnValue = CCD->GetNumJoys();
			return true;
		}
	case 65:	// else if (IS_METHOD(methodName, "SetBoundingBox"))
		{
			// USAGE SetBoundingBox(ANIMATION,width);
			// PARMCHECK(1);
			strcpy(param0, arguments[0].str());// animation
			float width = arguments[1].floatValue(); // width
			CCD->ActorManager()->SetBoundingBox(Actor,param0);
			geExtBox theBox;
			float CurrentHeight;
			CCD->ActorManager()->GetBoundingBox(Actor, &theBox);
			CurrentHeight = theBox.Max.Y;

			if(width > 0.0f)
				CCD->ActorManager()->SetBBox(Actor, width, -CurrentHeight*2.0f, width);

			return true;
		}
	case 66:	// else if (IS_METHOD(methodName, "GetBoneToBone"))
		{
			// USAGE: FLOAT GetBoneToBone(BONE NAME, TARGET ENTITY NAME, TARGET BONE NAME);
			PARMCHECK(3);
			geVec3d bpos1, bpos2;
			strcpy(param0, arguments[0].str());
			strcpy(param4, arguments[1].str());
			strcpy(param5, arguments[2].str());
			CCD->ActorManager()->GetBonePosition(Actor,param0,&bpos1);
			CCD->ActorManager()->GetBonePosition(CCD->ActorManager()->GetByEntityName(param4),param5,&bpos2);
			returnValue = geVec3d_DistanceBetween(&bpos1, &bpos2);
			return true;
		}
	case 67:	// else if (IS_METHOD(methodName, "SwitchView"))
		{
			// USAGE SwitchView(INT 0-2);
			PARMCHECK(1);
			int v = arguments[0].intValue();
			CCD->Player()->SwitchCamera(v);
			return true;
		}
	case 68:	// else if (IS_METHOD(methodName, "ForceEntityUp"))
		{
			PARMCHECK(2);
			float amount = arguments[0].floatValue();
			geVec3d theUp;
			geActor *mActor;
			strcpy(param0,arguments[1].str());
			mActor = CCD->ActorManager()->GetByEntityName(param0);
			CCD->ActorManager()->UpVector(mActor, &theUp);
			CCD->ActorManager()->SetForce(mActor, 0, theUp, amount, amount);
			return true;
		}
	case 69:	// else if (IS_METHOD(methodName, "ForceEntityDown"))
		{
			PARMCHECK(2);
			float amount = arguments[0].floatValue();
			geVec3d theUp;
			geActor *mActor;
			strcpy(param0,arguments[1].str());
			mActor = CCD->ActorManager()->GetByEntityName(param0);
			CCD->ActorManager()->UpVector(mActor, &theUp);
			geVec3d_Inverse(&theUp);
			CCD->ActorManager()->SetForce(mActor, 0, theUp, amount, amount);
			return true;
		}
	case 70:	// else if (IS_METHOD(methodName, "ForceEntityRight"))
		{
			PARMCHECK(2);
			float amount = arguments[0].floatValue();
			geVec3d theUp;
			geActor *mActor;
			strcpy(param0,arguments[1].str());
			mActor = CCD->ActorManager()->GetByEntityName(param0);
			CCD->ActorManager()->LeftVector(mActor, &theUp);
			geVec3d_Inverse(&theUp);
			CCD->ActorManager()->SetForce(mActor, 1, theUp, amount, amount);
			return true;
		}
	case 71:	// else if (IS_METHOD(methodName, "ForceEntityLeft"))
		{
			PARMCHECK(2);
			float amount = arguments[0].floatValue();
			geVec3d theUp;
			geActor *mActor;
			strcpy(param0,arguments[1].str());
			mActor = CCD->ActorManager()->GetByEntityName(param0);
			CCD->ActorManager()->LeftVector(mActor, &theUp);
			CCD->ActorManager()->SetForce(mActor, 1, theUp, amount, amount);
			return true;
		}
	case 72:	// else if (IS_METHOD(methodName, "ForceEntityForward"))
		{
			PARMCHECK(2);
			float amount = arguments[0].floatValue();
			geVec3d theUp;
			geActor *mActor;
			strcpy(param0,arguments[1].str());
			mActor = CCD->ActorManager()->GetByEntityName(param0);
			CCD->ActorManager()->InVector(mActor, &theUp);
			CCD->ActorManager()->SetForce(mActor, 2, theUp, amount, amount);
			return true;
		}
	case 73:	// else if (IS_METHOD(methodName, "ForceEntityBackward"))
		{
			// PARMCHECK(2);
			float amount = arguments[0].floatValue();
			geVec3d theUp;
			geActor *mActor;
			strcpy(param0,arguments[1].str());
			mActor = CCD->ActorManager()->GetByEntityName(param0);
			CCD->ActorManager()->InVector(mActor, &theUp);
			geVec3d_Inverse(&theUp);
			CCD->ActorManager()->SetForce(mActor, 2, theUp, amount, amount);
			return true;
		}
	case 74:	// else if (IS_METHOD(methodName, "GetGroundTexture")) //PWX
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
	case 75:	//	else if (IS_METHOD(methodName, "GetPlayerGroundTexture")) //PWX
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
	case 76:	// else if (IS_METHOD(methodName, "PositionToBone")) //PWX
		{
			// PARMCHECK(2);
			geVec3d bPos;
			strcpy(param0,arguments[0].str());
			strcpy(param4,arguments[1].str());
			CCD->ActorManager()->GetBonePosition(CCD->ActorManager()->GetByEntityName(param0),param4,&bPos);
			CCD->ActorManager()->Position(Actor,bPos);
			return true;
		}
	case 77:	// else if (IS_METHOD(methodName, "SetWeaponMatFromFlip"))
		{
			// USAGE: SetPlayerMaterial(FLIPBOOK szEntityName,Actor MatIndex,FlipBook Image Index, R,G,B);
			// PARMCHECK(6);
			geActor *Wactor;

			strcpy(param0, arguments[0].str());
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

			if(CCD->FlipBooks()->LocateEntity(param0, (void**)&pEntityData) == RGF_SUCCESS)
			{
				if(geActor_SetMaterial(Wactor, MatIndex, pEntityData->Bitmap[FlipIndex], R, G, B))
					returnValue = true;// Set Actor Material
			}

			return true;
		}
	case 78:	// else if (IS_METHOD(methodName, "SetShadowFromFlip")) // PWX
		{
			// USAGE: SetShadowFromFlip(FLIPBOOK szEntityName,ACTOR EntityName);
			// PARMCHECK(2);
			float tm;
			int tb;
			strcpy(param0, arguments[0].str());
			strcpy(param4, arguments[1].str());

			FlipBook *pEntityData = NULL;

			if(CCD->FlipBooks()->LocateEntity(param0, (void**)&pEntityData)==RGF_SUCCESS)
			{
				// find frame time of animation and set the proper bitmap
				geActor *wActor = CCD->ActorManager()->GetByEntityName(param4);
				tm = CCD->ActorManager()->GetAnimationTime(wActor)*30.f;
				tb = (int)(tm-1);

				if((tb > (pEntityData->BitmapCount - 1)) || tb < 0)
					tb = 0;

				CCD->ActorManager()->SetShadowBitmap(wActor,pEntityData->Bitmap[tb]);
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

		strcpy(param4,arguments[1].str());

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
		for(i = 0;i<mc;i++)
		{
			geActor_GetMaterial(Actor, i, &Bitmap, &R, &G, &B);
			geBody_SetMaterial(newBdy, i,Bitmap,R,G,B);
		}

		// add the new bdy to the actor
		geActor_SetBody(newDef,newBdy);

		//loop through the motions and copy them to the new actor
		mc = geActor_GetMotionCount(aDef);
		int nmc;
		for(i = 0;i<mc-1;i++)
		{
			Anim = geActor_GetMotionByIndex(aDef, mc);
			if(Anim)
				geActor_AddMotion(newDef, Anim, &nmc);
		}

		// save to the wrestlers folder
		strcpy(param0,"wrestlers\\");
		strcat(param0,arguments[0].str());
		df = geVFile_OpenNewSystem(NULL,GE_VFILE_TYPE_DOS,param0,NULL,GE_VFILE_OPEN_CREATE);

		returnValue = false;
		if(!df)
			return true;
		geActor_DefWriteToFile(newDef,df);
		geVFile_Close(df);//close file
		returnValue = true;

		//destroy everything
		//geBody_Destroy(&newBdy);
		//geMotion_Destroy(&Anim);
		//geBitmap_Destroy(&Bitmap);
		//geActor_DefDestroy(&newDef);

		return true;
	}
*/
	case 79:	// else if (IS_METHOD(methodName, "GetCollideDistance")) //PWX
		{
			// USAGE: GetCollideDistance(BONE_NAME ,Offset X,Offset Y, Offset Z);
			// PARMCHECK(4);
			geXForm3d Xf;
			geVec3d OldPos, Pos, theRotation, Direction;
			GE_Collision Collision;
			strcpy(param0, arguments[0].str());
			CCD->ActorManager()->GetPosition(Actor, &OldPos);
			float gd = -1.f;
			bool bone;
			bone = geActor_GetBoneTransform(Actor, param0, &Xf);

			if(!bone)
				return true;

			geVec3d_Copy(&(Xf.Translation), &OldPos);
			CCD->ActorManager()->GetRotate(Actor, &theRotation);
			//Pos = OldPos;

			// changed QD 12/15/05
			//geXForm3d_SetIdentity(&Xf);
			//geXForm3d_RotateZ(&Xf, theRotation.Z);
			geXForm3d_SetZRotation(&Xf, theRotation.Z);
			geXForm3d_RotateX(&Xf, theRotation.X);
			geXForm3d_RotateY(&Xf, theRotation.Y);
			//geXForm3d_Translate(&Xf, OldPos.X, OldPos.Y, OldPos.Z);
			// end change

			geXForm3d_GetUp(&Xf, &Direction);
			geVec3d_AddScaled(&OldPos, &Direction, arguments[2].floatValue(), &Pos);

			geXForm3d_GetLeft(&Xf, &Direction);
			geVec3d_AddScaled(&Pos, &Direction, arguments[1].floatValue(), &Pos);

			geXForm3d_GetIn(&Xf, &Direction);
			geVec3d_AddScaled(&Pos, &Direction, arguments[3].floatValue(), &Pos);

			if(geWorld_Collision(CCD->World(), NULL, NULL,
				&OldPos, &Pos, GE_CONTENTS_SOLID_CLIP | GE_CONTENTS_WINDOW,
				GE_COLLIDE_ALL, 0x0, NULL, NULL, &Collision))
				gd = geVec3d_DistanceBetween(&OldPos, &Collision.Impact);

			returnValue = gd;
			return true;
		}
	case 80:	// else if (IS_METHOD(methodName, "ResetAnimate")) // PWX
		{
			PARMCHECK(2);
			geVec3d pos;
			strcpy(param0, arguments[0].str());
			strcpy(param4, arguments[1].str());
			CCD->ActorManager()->GetBonePosition(Actor, param4, &pos);
			CCD->ActorManager()->SetMotion(Actor, param0);
			CCD->ActorManager()->SetHoldAtEnd(Actor, true);
			CCD->ActorManager()->Position(Actor, pos);
			return true;
		}
	case 81:	// else if (IS_METHOD(methodName, "WhereIsPlayer")) // PWX
		{
			geVec3d Pos1, Pos2, temp, Rot;
			geFloat fP, dP;

			strcpy(param0, arguments[0].str());
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
	case 82:	// else if (IS_METHOD(methodName, "WhereIsEntity")) // PWX
		{
			PARMCHECK(1);
			geVec3d Pos1, Pos2, temp, Rot;
			geFloat fP, dP;

			strcpy(param0, arguments[0].str());
			CCD->ActorManager()->GetPosition(Actor,&Pos1);
			CCD->ActorManager()->GetPosition(CCD->ActorManager()->GetByEntityName(param0),&Pos2);
			geVec3d_Subtract(&Pos2, &Pos1, &temp);
			geVec3d_Normalize(&temp);

			//	Do dotproduct.  If it's positive, then Actor2 is in front of Actor1
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
	case 83:	// else if (IS_METHOD(methodName, "InsertEvent")) // PWX
		{
			PARMCHECK(3);
			geActor_Def *aDef;
			float tKey;
			tKey = arguments[2].floatValue();
			strcpy(param0, arguments[0].str());
			strcpy(param4, arguments[1].str());
			aDef = geActor_GetActorDef(Actor);
			returnValue = false;

			if(geMotion_InsertEvent(geActor_GetMotionByName(aDef, param0),tKey, param4))
				returnValue = true;

			return true;
		}
	case 84:	// else if (IS_METHOD(methodName, "CheckForEvent")) // PWX
		{
			const char *evnt;
			geMotion *cMot;
			float StartTime, EndTime;

			cMot = geActor_GetMotionByName(geActor_GetActorDef(Actor), CCD->ActorManager()->GetMotion(Actor));
			EndTime = CCD->ActorManager()->GetAnimationTime(Actor,ANIMATION_CHANNEL_ROOT);
			StartTime = EndTime - (CCD->GetTicksGoneBy() * 0.001f);

			if(StartTime < 0.0f)
				StartTime = 0.0f;

			geMotion_SetupEventIterator(cMot, StartTime, EndTime);

			if(geMotion_GetNextEvent(cMot, &EndTime, &evnt) == GE_TRUE)
				returnValue = skString(evnt);
			else
				returnValue = false;

			return true;
		}
	case 85:	// else if (IS_METHOD(methodName, "PlayEventSound")) // PWX
		{
			// USAGE: Playevent sound entity,volume
			// PARMCHECK(1);

			const char *evnt;
			geMotion *cMot;
			float StartTime, EndTime;
			float volume;
			strcpy(param0, arguments[0].str());
			volume = arguments[1].floatValue();

			geActor *wActor = CCD->ActorManager()->GetByEntityName(param0);

			cMot = geActor_GetMotionByName(geActor_GetActorDef(wActor), CCD->ActorManager()->GetMotion(wActor));
			EndTime = CCD->ActorManager()->GetAnimationTime(wActor,ANIMATION_CHANNEL_ROOT);
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
/*	else if (IS_METHOD(methodName, "LoadSkin")) // PWX
	{
		//Usage: LoadSkin(Bitmap File Name in 'skins' Folder, Pawn Mat Index, R, G, B)
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
			Map = geBitmap_CreateFromFile(file);// Load Bitmap
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
	case 86:	// else if (IS_METHOD(methodName, "LoadAnimation")) // PWX
		{
			PARMCHECK(1);

			geVFile* file;
			geMotion *cMot;
			int ind;
			ind = 0;

			strcpy(param0, arguments[0].str());

			file = geVFile_OpenNewSystem(NULL, GE_VFILE_TYPE_DOS, param0, NULL, GE_VFILE_OPEN_READONLY);

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
/*	else if (IS_METHOD(methodName, "SaveAnimations")) // PWX
	{

		geVFile* df;
		geActor_Def* aDef;
		int Mc;
		int nM;

		aDef = geActor_GetActorDef(Actor);
		nM = geActor_GetMotionCount(aDef);

		for(Mc = 0;Mc < nM;Mc++)
		{
			strcpy(param0,"motions\\");
			strcat(param0,geActor_GetMotionName(aDef, Mc));
			df = geVFile_OpenNewSystem(NULL,GE_VFILE_TYPE_DOS,param0,NULL,GE_VFILE_OPEN_CREATE);
			geMotion_WriteToBinaryFile(geActor_GetMotionByIndex(aDef, Mc),df);
			geVFile_Close(df);//close file
		}
		returnValue = skString(param0);
		return true;
	}
*/
	case 87:	// else if (IS_METHOD(methodName, "StartSoundtrack"))
		{
			PARMCHECK(1);
			strcpy(param0, arguments[0].str());
			CCD->AudioStreams()->Play(param0, true, true);
			return true;
		}
	case 88:	// else if (IS_METHOD(methodName, "StopAllAudioStreams"))
		{
			CCD->AudioStreams()->StopAll();
			return true;
		}
// end pickles Jul 04
	case 89:	// else if (IS_METHOD(methodName, "ChangeYaw"))
		{
			ChangeYaw();
			return true;
		}
	case 90:	// else if (IS_METHOD(methodName, "ChangePitch"))
		{
			ChangePitch();
			return true;
		}
	case 91:	// else if (IS_METHOD(methodName, "random"))
		{
			// PARMCHECK(2);
			param1 = arguments[0].floatValue();
			param3 = arguments[1].floatValue();

			if(param1 <= param3)
				returnValue = (int)EffectC_Frand(param1, param3);
			else
				returnValue = (int)EffectC_Frand(param3, param1);

			return true;
		}
	case 92:	// else if (IS_METHOD(methodName, "walkmove"))
		{
			// PARMCHECK(2);
			float amount = arguments[1].floatValue() * ElapseTime;
			geXForm3d Xform;
			geVec3d In, NewPosition, SavedPosition;
			CCD->ActorManager()->GetPosition(Actor, &SavedPosition);

			// changed QD 12/15/05
			//geXForm3d_SetIdentity(&Xform);
			//geXForm3d_RotateY(&Xform, arguments[0].floatValue());
			//geXForm3d_Translate(&Xform, SavedPosition.X, SavedPosition.Y, SavedPosition.Z);
			// end change
			geXForm3d_SetYRotation(&Xform, arguments[0].floatValue());

			geXForm3d_GetIn(&Xform, &In);
			geVec3d_AddScaled(&SavedPosition, &In, amount, &NewPosition);
			returnValue = false;

			if(CCD->ActorManager()->ValidateMove(SavedPosition, NewPosition, Actor, false) == GE_TRUE)
				returnValue = true;

			return true;
		}
	case 93:	// else if (IS_METHOD(methodName, "flymove"))
		{
			PARMCHECK(3);
			float amount = arguments[2].floatValue() * ElapseTime;
			geXForm3d Xform;
			geVec3d In, NewPosition, SavedPosition;
			CCD->ActorManager()->GetPosition(Actor, &SavedPosition);

			// changed QD 12/15/05
			//geXForm3d_SetIdentity(&Xform);
			//geXForm3d_RotateX(&Xform, -arguments[0].floatValue());
			geXForm3d_SetXRotation(&Xform, -arguments[0].floatValue());
			geXForm3d_RotateY(&Xform, arguments[1].floatValue());
			//geXForm3d_Translate(&Xform, SavedPosition.X, SavedPosition.Y, SavedPosition.Z);
			// end change

			geXForm3d_GetIn(&Xform, &In);
			geVec3d_AddScaled(&SavedPosition, &In, amount, &NewPosition);
			returnValue = false;

			if(CCD->ActorManager()->ValidateMove(SavedPosition, NewPosition, Actor, false) == GE_TRUE)
				returnValue = true;

			return true;
		}
	case 94:	// else if (IS_METHOD(methodName, "Damage"))
		{
			PARMCHECK(2);
			float amount = arguments[0].floatValue();
			strcpy(param0, arguments[1].str());
			CCD->Damage()->DamageActor(TargetActor, amount, param0, amount, param0, "Melee");
			return true;
		}
	case 95:	// else if (IS_METHOD(methodName, "DamagePlayer"))
		{
			// PARMCHECK(2);
			float amount = arguments[0].floatValue();
			strcpy(param0, arguments[1].str());
			CCD->Damage()->DamageActor(CCD->Player()->GetActor(), amount, param0, amount, param0, "Melee");
			return true;
		}
	case 96:	// else if (IS_METHOD(methodName, "PositionToPlayer"))
		{
			// PARMCHECK(3);
			geXForm3d Xf;
			geVec3d Pos, theRotation, Direction;

			Pos = CCD->Player()->Position();

// changed QD 12/15/05
// added optional parameter to indicate whether the pawn should be rotatetd or not
			// if(arguments.entries() == 4)
			if(arguments.entries() > 3)
			{
				CCD->ActorManager()->GetRotate(CCD->Player()->GetActor(), &theRotation);

				if(arguments[3].boolValue())
				{
					//geXForm3d_SetIdentity(&Xf);
					//geXForm3d_RotateZ(&Xf, theRotation.Z);
					geXForm3d_SetZRotation(&Xf, theRotation.Z);
					geXForm3d_RotateX(&Xf, theRotation.X);
					geXForm3d_RotateY(&Xf, theRotation.Y);
					//geXForm3d_Translate(&Xf, Pos.X, Pos.Y, Pos.Z);

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
// end change QD 12/15/05
			}

			Pos.X += arguments[0].floatValue();
			Pos.Y += arguments[1].floatValue();
			Pos.Z += arguments[2].floatValue();
			CCD->ActorManager()->Position(Actor, Pos);

// Added Pickles RF071
			if(WeaponActor)
				CCD->ActorManager()->Position(WeaponActor, Pos);
// End Added Pickles RF071

			return true;
		}
	case 97:	// else if (IS_METHOD(methodName, "PlayerToPosition"))
		{
			PARMCHECK(3);
			geXForm3d Xf;
			geVec3d Pos, theRotation, Direction;

			CCD->ActorManager()->GetPosition(Actor, &Pos);

			if(arguments.entries() > 3) // Changed by Pickles RF07D
			{
				bool flag = arguments[3].boolValue();

				if(flag)
				{
					CCD->ActorManager()->GetRotate(Actor, &theRotation);

					// changed QD 12/15/05
					//geXForm3d_SetIdentity(&Xf);
					//geXForm3d_RotateZ(&Xf, theRotation.Z);
					geXForm3d_SetZRotation(&Xf, theRotation.Z);
					geXForm3d_RotateX(&Xf, theRotation.X);
					geXForm3d_RotateY(&Xf, theRotation.Y);
					//geXForm3d_Translate(&Xf, Pos.X, Pos.Y, Pos.Z);
					// end change

					geXForm3d_GetUp(&Xf, &Direction);
					geVec3d_AddScaled(&Pos, &Direction, arguments[1].floatValue(), &Pos);

					geXForm3d_GetLeft(&Xf, &Direction);
					geVec3d_AddScaled(&Pos, &Direction, arguments[0].floatValue(), &Pos);

					geXForm3d_GetIn(&Xf, &Direction);
					geVec3d_AddScaled(&Pos, &Direction, arguments[2].floatValue(), &Pos);

					CCD->ActorManager()->Position(CCD->Player()->GetActor(), Pos);

// Added By Pickles to RF07D
					if(arguments.entries() == 5)
					{
						bool flag1 = arguments[4].boolValue();

						if(flag1)
							CCD->ActorManager()->Rotate(CCD->Player()->GetActor(), theRotation);
					}
					else
						CCD->ActorManager()->Rotate(CCD->Player()->GetActor(), theRotation);
// END Added By Pickles to RF07D

					return true;
				}
			}

			Pos.X += arguments[0].floatValue();
			Pos.Y += arguments[1].floatValue();
			Pos.Z += arguments[2].floatValue();
			CCD->ActorManager()->Position(CCD->Player()->GetActor(), Pos);

			return true;
		}
	case 98:	// else if (IS_METHOD(methodName, "PositionToPawn"))
		{
			PARMCHECK(4);
			geXForm3d Xf;
			geVec3d Pos, OldPos, theRotation, Direction;
			geActor *MasterActor;

			strcpy(param0, arguments[0].str());
			MasterActor = CCD->ActorManager()->GetByEntityName(param0);

			if(!MasterActor)
				return true;

			CCD->ActorManager()->GetPosition(MasterActor, &Pos);
// Start PWX - Camera Code
			OldPos = Pos;
			OldPos.Y += 16.f;
// End PWX

			if(arguments.entries() > 4) // Changed by Pickles RF07D
			{
				bool flag = arguments[4].boolValue();

				if(flag)
				{
					CCD->ActorManager()->GetRotate(MasterActor, &theRotation);

					// changed QD 12/15/05
					//geXForm3d_SetIdentity(&Xf);
					//geXForm3d_RotateZ(&Xf, theRotation.Z);
					geXForm3d_SetZRotation(&Xf, theRotation.Z);
					geXForm3d_RotateX(&Xf, theRotation.X);
					geXForm3d_RotateY(&Xf, theRotation.Y);
					//geXForm3d_Translate(&Xf, Pos.X, Pos.Y, Pos.Z);
					// end change

					geXForm3d_GetUp(&Xf, &Direction);
					geVec3d_AddScaled(&Pos, &Direction, arguments[2].floatValue(), &Pos);

					geXForm3d_GetLeft(&Xf, &Direction);
					geVec3d_AddScaled(&Pos, &Direction, arguments[1].floatValue(), &Pos);

					geXForm3d_GetIn(&Xf, &Direction);
					geVec3d_AddScaled(&Pos, &Direction, arguments[3].floatValue(), &Pos);

// Start PWX - Camera Code
					GE_Collision Collision;
					geBoolean Result = geWorld_Collision(CCD->Engine()->World(), NULL, NULL, &OldPos, &Pos,
						GE_CONTENTS_SOLID_CLIP , GE_COLLIDE_ALL, 0,
						NULL, NULL, &Collision);

					if(Result)
					{
						Pos = Collision.Impact;
						geVec3d_AddScaled(&Pos, &(Collision.Plane.Normal), 2.0f, &Pos);
					}
// End PWX

					CCD->ActorManager()->Position(Actor, Pos);
// Added Pickles RF071
					if(WeaponActor)
						CCD->ActorManager()->Position(WeaponActor, Pos);
// End Added Pickles RF071

// Added by Pickles
					if(arguments.entries() == 6)
					{
						if(arguments[5].boolValue())
						{
							CCD->ActorManager()->Rotate(Actor, theRotation);

							// changed QD 12/15/05
							if(WeaponActor)
								CCD->ActorManager()->Rotate(WeaponActor, theRotation);
							// end change
						}
					}
					else
					{
						CCD->ActorManager()->Rotate(Actor, theRotation);

						// changed QD 12/15/05
						if(WeaponActor)
							CCD->ActorManager()->Rotate(WeaponActor, theRotation);
						// end change
					}
// END Added by Pickles

					return true;
				}
			}

			Pos.X += arguments[1].floatValue();
			Pos.Y += arguments[2].floatValue();
			Pos.Z += arguments[3].floatValue();
			CCD->ActorManager()->Position(Actor, Pos);

			// changed QD 12/15/05
			if(WeaponActor)
				CCD->ActorManager()->Position(WeaponActor, Pos);
			// end change

			return true;
		}
	case 99:	// else if (IS_METHOD(methodName, "SetKeyPause"))
		{
			PARMCHECK(1);
			bool flag = arguments[0].boolValue();
			CCD->SetKeyPaused(flag);
			return true;
		}
	case 100:	// else if (IS_METHOD(methodName, "PlayerRender"))
		{
			PARMCHECK(1);
			bool flag = arguments[0].boolValue();

			if(flag)
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
	case 101:	// else if (IS_METHOD(methodName, "PawnRender"))
		{
			PARMCHECK(1);
			bool flag = arguments[0].boolValue();

			if(flag)
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
	case 102:	// else if (IS_METHOD(methodName, "ChangeMaterial"))
		{
			PARMCHECK(1);
			strcpy(param0, arguments[0].str());

			if(!EffectC_IsStringNull(param0))
				CCD->ActorManager()->ChangeMaterial(Actor, param0);

			return true;
		}
	case 103:	// else if (IS_METHOD(methodName, "SetHoldAtEnd"))
		{
			PARMCHECK(1);
			bool amount = arguments[0].boolValue();
			CCD->ActorManager()->SetHoldAtEnd(Actor, amount);
			return true;
		}
	case 104:	// else if (IS_METHOD(methodName, "ForceUp"))
		{
			PARMCHECK(1);
			float amount = arguments[0].floatValue();
			geVec3d theUp;
			CCD->ActorManager()->UpVector(Actor, &theUp);
			CCD->ActorManager()->SetForce(Actor, 0, theUp, amount, amount);
			return true;
		}
	case 105:	// else if (IS_METHOD(methodName, "ForceDown"))
		{
			PARMCHECK(1);
			float amount = arguments[0].floatValue();
			geVec3d theUp;
			CCD->ActorManager()->UpVector(Actor, &theUp);
			geVec3d_Inverse(&theUp);
			CCD->ActorManager()->SetForce(Actor, 0, theUp, amount, amount);
			return true;
		}
	case 106:	// else if (IS_METHOD(methodName, "ForceRight"))
		{
			PARMCHECK(1);
			float amount = arguments[0].floatValue();
			geVec3d theRight;
			CCD->ActorManager()->LeftVector(Actor, &theRight);
			geVec3d_Inverse(&theRight);
			CCD->ActorManager()->SetForce(Actor, 1, theRight, amount, amount);
			return true;
		}
	case 107:	// else if (IS_METHOD(methodName, "ForceLeft"))
		{
			PARMCHECK(1);
			float amount = arguments[0].floatValue();
			geVec3d theLeft;
			CCD->ActorManager()->LeftVector(Actor, &theLeft);
			CCD->ActorManager()->SetForce(Actor, 1, theLeft, amount, amount);
			return true;
		}
	case 108:	// else if (IS_METHOD(methodName, "ForceForward"))
		{
			PARMCHECK(1);
			float amount = arguments[0].floatValue();
			geVec3d theIn;
			CCD->ActorManager()->InVector(Actor, &theIn);
			CCD->ActorManager()->SetForce(Actor, 2, theIn, amount, amount);
			return true;
		}
	case 109:	// else if (IS_METHOD(methodName, "ForceBackward"))
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
	case 110:	// else if (IS_METHOD(methodName, "UpdateTarget"))
		{
			UpdateTargetPoint = LastTargetPoint;
			return true;
		}
	case 111:	// else if (IS_METHOD(methodName, "FireProjectile"))
		{
			PARMCHECK(6);
			geXForm3d Xf;
			geVec3d theRotation, Pos, Direction, Orient, TargetPoint;
			geFloat x;
			geExtBox theBox;

			CCD->ActorManager()->GetBoundingBox(TargetActor, &theBox);
			TargetPoint = UpdateTargetPoint;
			// changed QD 12/15/05
			TargetPoint.Y -= (theBox.Max.Y*0.5f);// /2.0f);

			if(arguments.entries() == 7)
			{
				float height = arguments[6].floatValue();
				TargetPoint.Y += (theBox.Max.Y*height);
			}
			else
				TargetPoint.Y += (theBox.Max.Y*0.5f);

			strcpy(param4, arguments[0].str());
			strcpy(param0, arguments[1].str());
			strcpy(DamageAttr, arguments[5].str());

			float FireOffsetX = arguments[2].floatValue();
			float FireOffsetY = arguments[3].floatValue();
			float FireOffsetZ = arguments[4].floatValue();

			bool bone;

			if(WeaponActor)
				bone = geActor_GetBoneTransform(WeaponActor, param0, &Xf);
			else
				bone = geActor_GetBoneTransform(Actor, param0, &Xf);

			if(bone)
			{
				geVec3d_Copy(&(Xf.Translation), &Pos);
				CCD->ActorManager()->GetRotate(Actor, &theRotation);

				// changed QD 12/15/05
				//geXForm3d_SetIdentity(&Xf);
				//geXForm3d_RotateZ(&Xf, theRotation.Z);
				geXForm3d_SetZRotation(&Xf, theRotation.Z);
				geXForm3d_RotateX(&Xf, theRotation.X);
				geXForm3d_RotateY(&Xf, theRotation.Y);
				//geXForm3d_Translate(&Xf, Pos.X, Pos.Y, Pos.Z);
				// end change

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
					// changed QD 12/15/05
					// Orient.X = (float)(GE_PI*0.5f) - (float)acos(Orient.Y / l);
					Orient.X = GE_PIOVER2 - (float)acos(Orient.Y / l);
					Orient.Y = (float)atan2(x, Orient.Z) + GE_PI;
					Orient.Z = 0.0f;	// roll is zero - always!!?

					CCD->Weapons()->Add_Projectile(Pos, Pos, Orient, param4, DamageAttr, DamageAttr);
				}
			}

			return true;
		}
	case 112:	// else if (IS_METHOD(methodName, "AddExplosion"))
		{
			PARMCHECK(5);
			//geXForm3d Xf;
			//geVec3d theRotation, Direction;
			geVec3d Pos;

			strcpy(param0, arguments[0].str());
			strcpy(param4, arguments[1].str());

			float OffsetX = arguments[2].floatValue();
			float OffsetY = arguments[3].floatValue();
			float OffsetZ = arguments[4].floatValue();

			// changed QD 12/15/05
			/*if(geActor_GetBoneTransform(Actor, param4, &Xf))
			{
				geVec3d_Copy(&(Xf.Translation), &Pos);
				CCD->ActorManager()->GetRotate(Actor, &theRotation);

				// changed QD 12/15/05
				//geXForm3d_SetIdentity(&Xf);
				//geXForm3d_RotateZ(&Xf, theRotation.Z);
				geXForm3d_SetZRotation(&Xf, theRotation.Z);
				geXForm3d_RotateX(&Xf, theRotation.X);
				geXForm3d_RotateY(&Xf, theRotation.Y);
				//geXForm3d_Translate(&Xf, Pos.X, Pos.Y, Pos.Z);
				// end change

				geXForm3d_GetUp(&Xf, &Direction);
				geVec3d_AddScaled(&Pos, &Direction, OffsetY, &Pos);

				geXForm3d_GetLeft(&Xf, &Direction);
				geVec3d_AddScaled(&Pos, &Direction, OffsetX, &Pos);

				geXForm3d_GetIn(&Xf, &Direction);
				geVec3d_AddScaled(&Pos, &Direction, OffsetZ, &Pos);

				Pos.X = OffsetX;
				Pos.Y = OffsetY;
				Pos.Z = OffsetZ;

				CCD->Explosions()->AddExplosion(param0, Pos, Actor, param4);
			}*/
			if(geActor_DefHasBoneNamed(geActor_GetActorDef(Actor), param4))
			{
				Pos.X = OffsetX;
				Pos.Y = OffsetY;
				Pos.Z = OffsetZ;

				CCD->Explosions()->AddExplosion(param0, Pos, Actor, param4);
			}
			// end change

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
/* 07/15/2004 Wendell Buckner
    BUG FIX - Bone Collisions fail because we expect to hit the bone immediately after hitting the
	overall bounding box. So tag the actor as being hit at the bounding box level and after that check ONLY
	the bone bounding boxes until the whatever hit the overall bounding box no longer exists. */
	case 113:	// else if (IS_METHOD(methodName, "GetLastBoneHit"))
		{
			strcpy(param0, CCD->ActorManager()->GetLastBoneHit(Actor));
			returnValue = skString(param0);
			return true;
		}
	case 114:	// else if (IS_METHOD(methodName, "debug"))
		{
			PARMCHECK(1);
			strcpy(param0, arguments[0].str());

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
					strcpy(ConsoleDebug[index], param0);
				}
				else
				{
					for(i=1; i<DEBUGLINES; i++)
					{
						strcpy(ConsoleDebug[i-1], ConsoleDebug[i]);
					}

					strcpy(ConsoleDebug[DEBUGLINES-1], param0);
				}
			}

			return true;
		}
	case 115:	// else if (IS_METHOD(methodName, "SetEventState"))
		{
			// PARMCHECK(2);
			strcpy(param0, arguments[0].str());
			bool flag = arguments[1].boolValue();
			CCD->Pawns()->AddEvent(param0, flag);
			return true;
		}
	case 116:	// else if (IS_METHOD(methodName, "GetStringLength")) //PWX - ERROR
		{
			PARMCHECK(1);
			strcpy(param0,arguments[0].str());
// changed QD 12/15/05
			// returnValue = param0;
			returnValue = (int)strlen(param0);
// end change
			return true;
		}
	case 117:	// else if (IS_METHOD(methodName, "DrawText")) // PWX
		{
			PARMCHECK(8);
			strcpy(param0,arguments[0].str());			// string
			int Xpos = arguments[1].intValue();			// X pos
			int Ypos = arguments[2].intValue();			// Y pos
			float Alpha = arguments[3].floatValue();	// Alpha
			int Fnt = arguments[4].intValue();			// Font Number
			int R = arguments[5].intValue();			// Red
			int G = arguments[6].intValue();			// Green
			int B = arguments[7].intValue();			// Blue
			CCD->PWXImMgr()->AddImage(param0, NULL, Xpos, Ypos, Alpha, R, G, B, Fnt, 1.0f);
			return true;
		}
	case 118:	// if (IS_METHOD(methodName, "DrawFlipBookImage")) //PWX
		{
			PARMCHECK(9);
			strcpy(param0, arguments[0].str());			// Flipbook
			int FlipIndex = arguments[1].intValue();	// image ndex
			int Xpos = arguments[2].intValue();			// X pos
			int Ypos = arguments[3].intValue();			// Y pos
			float Alpha = arguments[4].floatValue();	// Alpha
			int R = arguments[5].intValue();			// Red
			int G = arguments[6].intValue();			// Green
			int B = arguments[7].intValue();			// Blue
			float ZDepth = arguments[8].floatValue();	// ZDepth
			FlipBook *pEntityData = NULL;

			if(CCD->FlipBooks()->LocateEntity(param0, (void**)&pEntityData)==RGF_SUCCESS)
			{
				CCD->PWXImMgr()->AddImage(NULL,pEntityData->Bitmap[FlipIndex],Xpos,Ypos,Alpha,R,G,B,0,ZDepth);
			}

			return true;
		}
	case 119:	// DrawPolyShadow
		{
			CCD->PlyShdw()->DrawShadow(Actor);
			return true;
		}
	case 120 :	// MatchPlayerAngles
		{
			geVec3d Rot;
			CCD->ActorManager()->GetRotate(CCD->Player()->GetActor(), &Rot);
			CCD->ActorManager()->Rotate(Actor, Rot);
			return true;
		}
	case 121:	// DamageAtBone
		{
			// USAGE: DamageAtBone(damage amount, radius, attribute, bone name)
			geVec3d Pos;
			geFloat dAmount,dRange;
			dAmount = arguments[0].floatValue();
			dRange = arguments[1].floatValue();
			strcpy(param0, arguments[2].str());
			strcpy(param4, arguments[3].str());
			CCD->ActorManager()->GetBonePosition(Actor, param4, &Pos);
			CCD->Damage()->DamageActorInRange(Pos, dRange, dAmount, param0, 0.0f, "", "");
			return true;
		}
	case 122:	//SaveActor
		{
			// USAGE: SaveActor(Actor Name)
			geVFile *df;
			geActor_Def *aDef;
			//strcpy(param4,arguments[1].str());
			//aDef = geActor_GetActorDef(CCD->ActorManager()->GetByEntityName(param4));
			aDef = geActor_GetActorDef(Actor);
			strcpy(param0, "savedactors\\");
			strcat(param0, arguments[0].str());
			df = geVFile_OpenNewSystem(NULL, GE_VFILE_TYPE_DOS, param0, NULL, GE_VFILE_OPEN_CREATE);
			returnValue = false;

			if(!df)
				return true;

			geActor_DefWriteToFile(aDef, df);
			geVFile_Close(df);//close file
			returnValue = true;
			return true;
		}

	case 123 :	// LookAtPawn
		{
			// USAGE: LookAtPawn(Pawn Name)
			geVec3d Pos1,Pos2,Rot;
			strcpy(param0,arguments[0].str());
			float heightoff = arguments[1].floatValue();
			CCD->ActorManager()->GetPosition(Actor, &Pos1);
			CCD->ActorManager()->GetPosition(CCD->ActorManager()->GetByEntityName(param0), &Pos2);
			Pos2.Y += heightoff;//add height offset to target point
			CCD->PathFollower()->GetRotationToFacePoint(Pos1, Pos2, &Rot);
			CCD->ActorManager()->Rotate(Actor, Rot);
			return true;
		}
	case 124:	// AutoWalk
		{
			// PARMCHECK(3);
			float amount = arguments[1].floatValue() * ElapseTime;
			float distance = arguments[2].floatValue(); // distance to keep from walls
			geXForm3d Xform;
			geVec3d In, NewPosition, SavedPosition, TestPos;

			CCD->ActorManager()->GetPosition(Actor, &SavedPosition);
			// changed QD 12/15/05
			//geXForm3d_SetIdentity(&Xform);
			//geXForm3d_RotateY(&Xform, arguments[0].floatValue());
			geXForm3d_SetYRotation(&Xform, arguments[0].floatValue());
			//geXForm3d_Translate(&Xform, SavedPosition.X, SavedPosition.Y, SavedPosition.Z);
			// end change

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
	case 125:	// FastDistance
		{
			// USAGE: BOOL = FastDistance(EntityName,distance);
			float result;
			strcpy(param0,arguments[0].str());
			float dist = arguments[1].floatValue();
			geVec3d Pos1, Pos2, dV;
			CCD->ActorManager()->GetPosition(Actor, &Pos1);
			CCD->ActorManager()->GetPosition(CCD->ActorManager()->GetByEntityName(param0), &Pos2);
			geVec3d_Subtract(&Pos1, &Pos2, &dV);
			result = geVec3d_DotProduct(&dV, &dV);
			returnValue = false;

			if(result < dist * dist)
				returnValue = true;

			return true;
		}
	case 126:	// StepHeight
		{
			// USAGE: StepHeight(float)
			float ht = arguments[0].floatValue();
			CCD->ActorManager()->SetStepHeight(Actor, ht);
			return true;
		}
	case 127:	// DrawHPoly
		{
			return true;
		}

	case 128:	// DrawVPoly
		{
			return true;
		}
	case 129:	// GetPitchToPoint
		{
			geVec3d Pos1,Rot;
			CCD->ActorManager()->GetPosition(Actor, &Pos1);
			CCD->PathFollower()->GetRotationToFacePoint(Pos1,CurrentPoint, &Rot);
			returnValue = Rot.X;
			return true;
		}
	case 130:	// GetYawToPoint
		{
			geVec3d Pos1,Rot;
			CCD->ActorManager()->GetPosition(Actor, &Pos1);
			CCD->PathFollower()->GetRotationToFacePoint(Pos1, CurrentPoint, &Rot);
			returnValue = Rot.Y;
			return true;
		}
	case 131:	// FastPointCheck
		{
			// USAGE: BOOL = FastPointCheck(distance);
			float result;
			float dist = arguments[0].floatValue();
			geVec3d Pos1, dV;
			CCD->ActorManager()->GetPosition(Actor, &Pos1);
			geVec3d_Subtract(&Pos1,&CurrentPoint, &dV);
			result = geVec3d_DotProduct(&dV, &dV);
			returnValue = false;

			if(result < dist * dist)
				returnValue = true;

			return true;
		}
// changed Nout/QD 12/15/05
	case 132:	// SetCameraWindow
		{
			// USAGE: SetCameraWindow(float FOV)
			// SetCameraWindow(float FOV, bool UseWidhtHeight, Left, Top, Width/Right, Height/Bottom);
			// Sets the size + zoom factor of the real engine camera

			if(arguments.entries() > 1)
			{
				PARMCHECK(6);
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
				PARMCHECK(1);
				CCD->CameraManager()->SetFOV(arguments[0].floatValue());
			}

			return true;
		}
	case 133:	// SetFixedCameraPosition
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
	case 134:	// SetFixedCameraRotation
		{
			// USAGE: SetFixedCameraRotation(RotX, RotY, RotZ)
			PARMCHECK(3);

			geVec3d Rot;
			Rot.X = 0.0174532925199433f*arguments[0].floatValue();
			Rot.Y = 0.0174532925199433f*arguments[1].floatValue();
			Rot.Z = 0.0174532925199433f*arguments[2].floatValue();
			CCD->FixedCameras()->SetRotation(Rot);

			return true;
		}
	case 135:	// SetFixedCameraFOV
		{
			// USAGE: SetFixedCameraFOV(float FOV)
			PARMCHECK(1);

			CCD->FixedCameras()->SetFOV(arguments[0].floatValue());

			return true;
		}
	case 136:	// MoveFixedCamera
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
	case 137:	// RotateFixedCamera
		{
			// USAGE: RotateFixedCamera(RotX, RotY, RotZ)
			PARMCHECK(3);

			geVec3d Rot;
			Rot.X = 0.0174532925199433f*arguments[0].floatValue();
			Rot.Y = 0.0174532925199433f*arguments[1].floatValue();
			Rot.Z = 0.0174532925199433f*arguments[2].floatValue();
			CCD->FixedCameras()->Rotate(Rot);

			return true;
		}
	case 138:	// DistanceBetweenEntities
		{
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
	case 139:	// SetEntityProperties
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
					//CCD->ActorManager()->GetRotation(pActor, &Rot);
					Rot.X = 0.0174532925199433f*arguments[9].floatValue();
					Rot.Y = 0.0174532925199433f*arguments[10].floatValue();
					Rot.Z = 0.0174532925199433f*arguments[11].floatValue();
					CCD->ActorManager()->Rotate(pActor, Rot);
				}
			}

			return true;
		}
	case 140:	// SetEntityLighting
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
	case 141:	// UpdateScriptPoint
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
							pProxy->Angle.X = arguments[5].intValue()*0.0174532925199433f;
							pProxy->Angle.Y = arguments[6].intValue()*0.0174532925199433f;
							pProxy->Angle.Z = arguments[7].intValue()*0.0174532925199433f;
						}
					}
				}
			}

			return true;
		}
	case 142:	// GetEntityScreenX
		{
			// USAGE:
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
				CCD->ActorManager()->GetPosition(Actor, &Pos);

			geCamera_Transform(CCD->CameraManager()->Camera(), &Pos, &ScreenPos);
			returnValue = int((CCD->Engine()->Width()/2 - 1) * (1.0f - ScreenPos.X / ScreenPos.Z));

			return true;
		}
	case 143:	// GetEntityScreenY
		{
			// USAGE:
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
				CCD->ActorManager()->GetPosition(Actor, &Pos);

			geCamera_Transform(CCD->CameraManager()->Camera(), &Pos, &ScreenPos);
			returnValue = int((CCD->Engine()->Height()/2 -1) * (1.0f + ScreenPos.Y / ScreenPos.Z));

			return true;
		}
	case 144:	// GetScreenWidth
		{
			// USAGE:
			// int = GetScreenWidth()
			// Gets the width of the screen in pixels
			returnValue = int(CCD->Engine()->Width());
			return true;
		}
	case 145:	// GetScreenHeight
		{
			// USAGE:
			// int = GetScreenHeight()
			// Gets the height of the screen in pixels
			returnValue = int(CCD->Engine()->Height());
			return true;
		}
	case 146:	// MouseSelect
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

			ShowCursor(TRUE);

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
	case 147:	// MouseControlledPlayer
		{
			// USAGE: MouseControlledPlayer(true/false)

			CCD->SetMouseControl(arguments[0].boolValue());
			return true;
		}
	case 148:	// ShowMouse
		{
			// USAGE: ShowMouse(true/false)

			if(arguments[0].boolValue())
				ShowCursor(TRUE);
			else
				ShowCursor(FALSE);

			return true;
		}
	case 149:	// GetMousePosX
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
				RectPos.x = 0;

			GetCursorPos(&MousePos);
			MousePos.x -= RectPos.x;
			returnValue = MousePos.x;

			return true;
		}
	case 150:	// GetMousePosY
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
				RectPos.y = 0;

			GetCursorPos(&MousePos);
			MousePos.y -= RectPos.y;
			returnValue = MousePos.y;

			return true;
		}
	case 151:	// SetMousePos
		{
			// USAGE: SetMousePos(int ScreenPosX, int ScreenPosY)

			POINT MousePos;
			MousePos.x = arguments[0].intValue();
			MousePos.y = arguments[1].intValue();
			SetCursorPos(MousePos.x, MousePos.y);
			return true;
		}
	case 152:	// SetGamma
		{
			// USAGE: SetGamma(Gamma)
			PARMCHECK(1);
			param1 = arguments[0].floatValue();
			geEngine_SetGamma(CCD->Engine()->Engine(), param1);
			return true;
		}
	case 153:	// GetGamma
		{
			// USAGE: GetGamma()
			geEngine_GetGamma(CCD->Engine()->Engine(), &param1);
			returnValue = param1;
			return true;
		}
	case 154:	// FillScreenArea
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
	case 155:	// RemoveScreenArea
		{
			// USAGE: RemoveScreenArea(Nr)
			PARMCHECK(1);

			int temp = arguments[0].intValue();

			if(temp < 0 || temp >= MAXFILLAREA)
				return true;

			CCD->Pawns()->FillScrArea[temp].DoFillScreenArea = false;

			return true;
		}
	case 156:	// ShowText
		{
			// USAGE: ShowText(Nr, EntityName, Animation, TextString, FontNr, TextSound,
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
			//strcpy(CCD->Pawns()->TextMessage[Nr].TextString, arguments[3].str());
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
	case 157:	// RemoveText
		{
			// USAGE: RemoveText(int Nr)
			PARMCHECK(1);

			int Nr = arguments[0].intValue();

			if(Nr < 0 || Nr >= MAXTEXT)
				return true;

			CCD->Pawns()->TextMessage[Nr].ShowText = false;

			return true;
		}
	case 158:	// ShowHudPicture
		{
			// USAGE:
			// ShowHudPicture(HUDpictureNr, IsVisible)
			// ShowHudPicture(HUDpictureNr, IsVisible, EntityName, ScreenX, ScreenY, DisplayTime)
			// ShowHudPicture(HUDpictureNr, IsVisible, EntityName, ScreenX, ScreenY, DisplayTime, WinX, WinY)

			geVec3d Pos, ScreenPos;
			int ScreenX = 0;
			int ScreenY = 0;
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

						geCamera_Transform(CCD->CameraManager()->Camera(), &Pos, &ScreenPos);
						ScreenX = int(((CCD->Engine()->Width()/2) -1) * (1.0f - (ScreenPos.X / ScreenPos.Z)));
						ScreenY = int(((CCD->Engine()->Height()/2) -1) * (1.0f + (ScreenPos.Y / ScreenPos.Z)));

						CCD->HUD()->SetElementLeftTop(element, ScreenX + arguments[3].intValue(), ScreenY + arguments[4].intValue());
					}
					else
						CCD->HUD()->SetElementLeftTop(element, arguments[3].intValue(), arguments[4].intValue());

					if(arguments.entries() > 7)
						CCD->HUD()->SetElementILeftTop(element, arguments[6].intValue(), arguments[7].intValue());
				}

			}

			return true;
		}
	case 159:	// SetHudDraw
		{
			// USAGE: SetHudDraw(true/false)
			PARMCHECK(1);

			if(arguments[0].boolValue())
				CCD->HUD()->Activate();
			else
				CCD->HUD()->Deactivate();

			return true;
		}
	case 160:	// GetHudDraw
		{
			// USAGE: bool = GetHudDraw()
			returnValue = CCD->HUD()->GetActive();
			return true;
		}
	case 161:	// SetAlpha
		{
			// USAGE: SetAlpha(Alpha)
			PARMCHECK(1);
			param1 = arguments[1].floatValue();

			CCD->ActorManager()->SetAlpha(Actor, param1);

			return true;
		}
	case 162:	// GetAlpha
		{
			// USAGE: float = GetAlpha()

			CCD->ActorManager()->GetAlpha(Actor, &param1);

			returnValue = param1;

			return true;
		}
	case 163:	// SetEntityAlpha
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
	case 164:	// GetEntityAlpha
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
	case 165:	// SetScale
		{
			// USAGE:
			// SetScale(Scale)
			// SetScale(ScaleX, ScaleY, ScaleZ)

			strcpy(param0, arguments[0].str());
			geVec3d Scale;

			Scale.X = arguments[1].floatValue();

			if(arguments.entries() > 3)
			{
				Scale.Y = arguments[2].floatValue();
				Scale.Z = arguments[3].floatValue();

				CCD->ActorManager()->SetScaleXYZ(Actor, Scale);

				return true;
			}

			CCD->ActorManager()->SetScale(Actor, Scale.X);

			return true;
		}
	case 166:	// SetEntityScale
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
	case 167:	// CheckArea
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
			strcpy(param0, arguments[0].str());
			strcpy(param4, arguments[1].str());
			param2 = arguments[2].boolValue();
			param1 = arguments[3].floatValue();
			param3 = arguments[4].floatValue();

			returnValue = bool(CCD->Pawns()->Area(param0, param4, param2, param1, param3,
								arguments[5].floatValue(), arguments[6].floatValue(),
								arguments[7].boolValue(), arguments[8].boolValue(), arguments[9].boolValue()));

			return true;
		}
	case 168:	// SetFlag
		{
			// USAGE: SetFlag(int FlagNr, true/false)
			int int1 = arguments[0].intValue();

			if(int1 < 0 || int1 >= MAXFLAGS)
				return true;

			param2 = arguments[1].boolValue();
			CCD->Pawns()->SetPawnFlag(int1, param2);
			return true;
		}
	case 169:	// GetFlag
		{
			// USAGE: bool = GetFlag(int FlagNr)
			int int1 = arguments[0].intValue();

			if(int1 < 0 || int1 >= MAXFLAGS)
				return true;

			returnValue = CCD->Pawns()->GetPawnFlag(int1);
			return true;
		}
// end change
// changed QD 12/15/05
	case 170:	// PowerUp
		{
			// USAGE:	PowerUp(char *AttributeName, int Amount, bool SetMaximumAmount)
			//			PowerUp(char *AttributeName, int Amount, bool SetMaximumAmount, EntityName)
			strcpy(param0, arguments[0].str());

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
				theInv = CCD->ActorManager()->Inventory(Actor);

			int NewHighLimit = theInv->High(param0) + arguments[1].intValue();;

			theInv->SetHighLimit(param0, NewHighLimit);

			if(arguments[2].boolValue())
				theInv->Set(param0, NewHighLimit);

			return true;
		}
	case 171:	// GetPowerUpLevel
		{
			// USAGE:	int = GetPowerUpLevel(char *AttributeName)
			//			int = GetPowerUpLevel(char *AttributeName, char *EntityName)
			strcpy(param0, arguments[0].str());

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
				theInv = CCD->ActorManager()->Inventory(Actor);

			returnValue = theInv->GetPowerUpLevel(param0);

			return true;
		}
	case 172:	// ActivateHudElement
		{
			// USAGE: ActivateHudElement(char *AttributeName, bool Flag)
			strcpy(param0, arguments[0].str());

			CCD->HUD()->ActivateElement(param0, arguments[1].boolValue());
			return true;
		}
	case 173:	// MoveEntity
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
	case 174:	// RotateEntity
		{
			// USAGE: RotateEntity(EntityName, AmountX, AmountY, AmountZ);
			strcpy(param0, arguments[0].str());
			geVec3d Amount;

			Amount.X = 0.0174532925199433f*arguments[1].floatValue();
			Amount.Y = 0.0174532925199433f*arguments[2].floatValue();
			Amount.Z = 0.0174532925199433f*arguments[3].floatValue();

			if(!stricmp(param0, "Player"))
				CCD->ActorManager()->AddRotation(CCD->Player()->GetActor(), Amount);
			else
				CCD->ActorManager()->AddRotation(CCD->ActorManager()->GetByEntityName(param0), Amount);

			return true;
		}
	case 175:	// SetEntityPosition
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
	case 176:	// SetEntityRotation
		{
			// USAGE: SetEntityRotation(EntityName, RotX, RotY, RotZ);
			strcpy(param0, arguments[0].str());
			geVec3d Rot;

			Rot.X = 0.0174532925199433f*arguments[1].floatValue();
			Rot.Y = 0.0174532925199433f*arguments[2].floatValue();
			Rot.Z = 0.0174532925199433f*arguments[3].floatValue();

			if(!stricmp(param0, "Player"))
				CCD->ActorManager()->Rotate(CCD->Player()->GetActor(), Rot);
			else
				CCD->ActorManager()->Rotate(CCD->ActorManager()->GetByEntityName(param0), Rot);

			return true;
		}
	case 177:	// AddAttribute
		{
			// USAGE:	AddAttribute(char *Attribute)
			//			AddAttribute(char *Attribute, EntityName)

			// changed QD 07/15/06 - add optional arguments
			//			AddAttribute(char *Attribute, int LowValue, int HighValue)
			//			AddAttribute(char *Attribute, int LowValue, int HighValue, char *EntityName)
			PARMCHECK(1);
			strcpy(param0, arguments[0].str());

			CPersistentAttributes *theInv;

			if(arguments.entries()==2 || arguments.entries()==4)
			{
				strcpy(param4, arguments[arguments.entries()-1].str());

				if(!stricmp(param4, "Player"))
					theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
				else
					theInv = CCD->ActorManager()->Inventory(CCD->ActorManager()->GetByEntityName(param4));
			}
			else
				theInv = CCD->ActorManager()->Inventory(Actor);

			returnValue = (int)theInv->Add(param0);

			if(arguments.entries()>2)
			{
				theInv->SetValueLimits(param0, arguments[1].intValue(), arguments[2].intValue());
			}
			// end change QD 07/15/06

			return true;
		}
// end change
// changed QD 07/15/06
	case 178:	// SetAttributeValueLimits
		{
			// USAGE:	SetAttributeValueLimits(char* Attribute, int LowValue, int HighValue),
			//			SetAttributeValueLimits(char* Attribute, int LowValue, int HighValue, char* EntityName)

			PARMCHECK(3);
			strcpy(param0, arguments[0].str());

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
				theInv = CCD->ActorManager()->Inventory(Actor);

			theInv->SetValueLimits(param0, arguments[1].intValue(), arguments[2].intValue());

			return true;
		}
	case 179:	// sin
		{
			PARMCHECK(1);
			returnValue = (float)sin((double)arguments[0].floatValue());

			return true;
		}
	case 180:	// cos
		{
			PARMCHECK(1);
			returnValue = (float)cos((double)arguments[0].floatValue());

			return true;
		}
	case 181:	// tan
		{
			PARMCHECK(1);
			returnValue = (float)tan((double)arguments[0].floatValue());

			return true;
		}
	case 182:	// RightCopy
		{
			int length;
			int temp = arguments[1].intValue();
			temp = (temp<127)?temp:127;
			strncpy(param0, arguments[0].str(), 127);
			param0[127] = 0;
			length = strlen(param0);

			strncpy(param4, &param0[length-temp], temp);
			param4[temp] = 0;
			returnValue = skString(param4);
			return true;
		}
	case 183:	// NearestPoint(float MinDistance, float MaxDistance)
		{
			PARMCHECK(2);

			returnValue = false;

			//if(!TargetActor)
			//	return true;

			geEntity_EntitySet *pSet;
			geEntity *pEntity;
			// Ok, check to see if there are ScriptPoints in this world
			pSet = geWorld_GetEntitySet(CCD->World(), "ScriptPoint");

			if(!pSet)
				return true;

			geVec3d Pos, Dist;//TargetPos,
			float minDist2, MinDistance2, MaxDistance2;//minTargetDist2,
			float newDist2;//, newTargetDist2;

			MinDistance2 = arguments[0].floatValue()*arguments[0].floatValue();
			MaxDistance2 = arguments[1].floatValue()*arguments[1].floatValue();
			minDist2 = MaxDistance2;
			//minTargetDist2 = minDist2*10000.f;

			CCD->ActorManager()->GetPosition(Actor,	&Pos);
			//CCD->ActorManager()->GetPosition(TargetActor, &TargetPos);

			// Ok, we have ScriptPoints somewhere.  Dig through 'em all.
			for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
				pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
			{
				ScriptPoint *pSource = (ScriptPoint*)geEntity_GetUserData(pEntity);

				//calc new dist
				geVec3d_Subtract(&pSource->origin, &Pos, &Dist);
				newDist2 = geVec3d_LengthSquared(&Dist);
				//geVec3d_Subtract(&pSource->origin, &TargetPos, &Dist);
				//newTargetDist2 = geVec3d_LengthSquared(&Dist);

				// point is too close
				if(newDist2<MinDistance2 || newDist2>MaxDistance2)
					continue;

				if(newDist2<minDist2)// && newTargetDist2<=minTargetDist2)
				{
					// probably slowest test...
					if(CCD->Pawns()->CanSeePoint(FOV, Actor, &(pSource->origin), FOVBone))
					{
						minDist2 = newDist2;
						//minTargetDist2 = newTargetDist2;
						strcpy(Point, pSource->szEntityName);
						CurrentPoint= pSource->origin;
						ValidPoint = true;
						returnValue = true;
					}
				}
			}

			return true;
		}
	case 184:	// SetFOV - same as high level command
		{
			PARMCHECK(1);

			// NOTE QD: FOV is stored as acos of the half angle for direct comparison with dot products
			// acos(x) ~ (PI/2 - x) (x = angle in radians)
			// The approximation used here is not correct, but we've used it
			// for quite a while now, so leave it like this...
			// correct: FOV = GE_PIOVER2 - arguments[0].floatValue()*0.5f*GE_PIOVER180;

			//FOV = (90.0f-arguments[0].floatValue()*0.5f)/90.0f;
			FOV = (180.f - arguments[0].floatValue())/180.f;
			FOVBone[0] = '\0';
			if(arguments.entries()>1)
				strcpy(FOVBone, arguments[1].str());

			return true;
		}
// end change QD 07/15/06
// changed QD 02/01/07
	case 185:	// asin
		{
			PARMCHECK(1);
			returnValue = (float)asin((double)arguments[0].floatValue());

			return true;
		}
	case 186:	// acos
		{
			PARMCHECK(1);
			returnValue = (float)acos((double)arguments[0].floatValue());

			return true;
		}
	case 187:	// atan
		{
			PARMCHECK(1);
			returnValue = (float)atan((double)arguments[0].floatValue());

			return true;
		}
// end change
	} // close switch statement

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
		// changed QD 12/15/05
		// Orient.X = (float)(GE_PI*0.5f) - (float)acos(Orient.Y / l);
		Orient.X = GE_PIOVER2 - (float)acos(Orient.Y / l);
		Orient.Y = (float)atan2(x, Orient.Z) + GE_PI;
	}
	// changed QD 12/15/05
	{

		CCD->ActorManager()->GetRotate(Actor, &Pos);

		while(Pos.Y < 0.0f)
		{
			// changed QD 12/15/05
			Pos.Y += GE_2PI;
		}

		// changed QD 12/15/05
		while(Pos.Y >= GE_2PI)
		{
			// changed QD 12/15/05
			Pos.Y -= GE_2PI;
		}

		while(Pos.X < 0.0f)
		{
			// changed QD 12/15/05
			Pos.X += GE_2PI;
		}

		// changed QD 12/15/05
		while(Pos.X >= GE_2PI)
		{
			// changed QD 12/15/05
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
	float	mid, bottom;
	int x,y;

	geActor_GetExtBox(Actor, &theBox);
	Start.Y = theBox.Min.Y;
	Stop.Y = theBox.Min.Y - 1.0f;
	bool flag = false;

	for(x=0; x<=1; x++)
	{
		for(y=0; y<=1; y++)
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

	for(x=0; x<=1; x++)
	{
		for(y=0; y<=1; y++)
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
			// changed QD 09/21/03
			// changed QD 12/15/05
			RotateAmt = GE_2PI - RotateAmt; //RotateAmt -= GE_PI;

			if(RotateLeft)
				RotateLeft = false;
			else
				RotateLeft = true;
		}

		// changed QD 12/15/05
		RotateAmt *= GE_180OVERPI; // /= 0.0174532925199433f;

		float amount = yaw_speed * ElapseTime;

		if(amount > RotateAmt)
		{
			amount = RotateAmt;
		}

		// changed QD 09/21/03
		// RotateAmt -= amount;

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
			// changed QD 09/21/03
			// changed QD 12/15/05
			RotateAmt = GE_2PI - RotateAmt;// RotateAmt -= GE_PI;

			if(RotateUp)
				RotateUp = false;
			else
				RotateUp = true;
		}

		// changed QD 12/15/05
		RotateAmt *= GE_180OVERPI; // /= 0.0174532925199433f;

		float amount = pitch_speed * ElapseTime;

		if(amount > RotateAmt)
		{
			amount = RotateAmt;
		}

		// changed QD 09/21/03
		// RotateAmt -= amount;

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
	float delta;
	geXForm3d Xform;
	geVec3d In;

	ideal_yaw = 0.0174532925199433f*yaw;
	ChangeYaw();

	CCD->ActorManager()->GetPosition(Actor, &oldorigin);

	// changed QD 12/15/05
	//geXForm3d_SetIdentity(&Xform);
	//geXForm3d_RotateY(&Xform, ideal_yaw);
	geXForm3d_SetYRotation(&Xform, ideal_yaw);
	//geXForm3d_Translate(&Xform, oldorigin.X, oldorigin.Y, oldorigin.Z);
	// end change

	geXForm3d_GetIn(&Xform, &In);
	geVec3d_Scale(&In, dist, &move);

	if(MoveStep(move))
	{
		GetAngles(true);
		// changed QD 12/15/05
		float selfangle = anglemod(actoryaw*GE_180OVERPI);// /0.0174532925199433f);
		float idealangle = anglemod(ideal_yaw*GE_180OVERPI);// /0.0174532925199433f);
		delta = anglemod(selfangle - idealangle);

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
	float	tdir, olddir, turnaround;

	// changed QD 12/15/05
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
			tdir = (float)ttdir;

			if(tdir != turnaround && StepDirection(tdir, dist))
				return;
		}
	}
	else
	{
		for(int ttdir=315; ttdir >=0; ttdir-=45)
		{
			tdir = (float)ttdir;

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
	a = (360.0f/65536.0f) * ((int)(a*(65536.0f/360.0f)) & 65535);
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
		NewChaseDir (dist);
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
	skRValueArray args; //change simkin
	skRValue ret;

	if(Object->console)
	{
		char szBug[256];
		sprintf(szBug, "%s %s",Object->szName, Object->Order);
		strcpy(Object->ConsoleHeader, szBug);
	}

// changed QD 07/15/06
	/*
	Object->lowTime += (dwTicks*0.001f);
	Object->ElapseTime += (dwTicks*0.001f);
	Object->ThinkTime -= (dwTicks*0.001f);
	*/
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
	catch(skRuntimeException e)
	{
		if(!strcmp(Object->Indicate, "+"))
			strcpy(Object->Indicate, " ");
		else
			strcpy(Object->Indicate, "+");
		if(Object->console)
			strcpy(Object->ConsoleError, e.toString());
	}
	catch(skParseException e)
	{
		if(!strcmp(Object->Indicate, "+"))
			strcpy(Object->Indicate, " ");
		else
			strcpy(Object->Indicate, "+");
		if(Object->console)
			strcpy(Object->ConsoleError, e.toString());
	}
	catch (...)
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
