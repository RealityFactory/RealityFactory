#include "RabidFramework.h"

#include "Simkin\\skScriptedExecutable.h"
#include "Simkin\\skRValue.h"
#include "Simkin\\skRValueArray.h"
#include "Simkin\\skRuntimeException.h"
#include "Simkin\\skParseException.h"

extern geSound_Def *SPool_Sound(char *SName);

// get a variable's value
bool ScriptedObject::getValue(const skString& fieldName, const skString& attribute, skRValue& value)
{
	skString sval;
	if (fieldName == "time")
	{
		value = lowTime;
		return true;
	}
	else if (fieldName == "ThinkTime")
	{
		value = ThinkTime;
		return true;
	}
	else if (fieldName == "DifficultyLevel")
	{
		value = CCD->GetDifficultLevel();
		return true;
	}
	else if (fieldName == "EntityName")
	{
		value = skString(szName);
		return true;
	}

	else if (fieldName == "health")
	{
		CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(Actor);
		if(!EffectC_IsStringNull(Attribute))
			value = theInv->Value(Attribute);
		else
			value = -1;
		return true;
	}
	else if (fieldName == "attack_finished")
	{
		value = attack_finished;
		return true;
	}
	else if (fieldName == "attack_state")
	{
		value = attack_state;
		return true;
	}
	else if (fieldName == "enemy_vis")
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
			LastTargetPoint.Y += ((theBox.Max.Y)/2.0f);
		}
		value = flag;
		return true;
	}

	else if (fieldName == "enemy_infront")
	{
		GetAngles(true);
		value = false;
		if(actoryaw == targetyaw)
			value = true;
		return true;
	}
	else if (fieldName == "enemy_range")
	{
		geExtBox theBox;
		CCD->ActorManager()->GetBoundingBox(TargetActor, &theBox);
		value = CCD->ActorManager()->DistanceFrom(LastTargetPoint, Actor) - theBox.Max.X;
		return true;
	}
	else if (fieldName == "player_range")
	{
		geExtBox theBox;
		CCD->ActorManager()->GetBoundingBox(CCD->Player()->GetActor(), &theBox);
		value = CCD->ActorManager()->DistanceFrom(CCD->Player()->Position(), Actor) - theBox.Max.X;
		return true;
	}
	else if (fieldName == "enemy_yaw")
	{
		GetAngles(true);
		value = targetyaw;
		return true;
	}
	else if (fieldName == "last_enemy_yaw")
	{
		GetAngles(false);
		value = targetyaw;
		return true;
	}
	else if (fieldName == "enemy_pitch")
	{
		GetAngles(true);
		value = targetpitch;
		return true;
	}
	else if (fieldName == "last_enemy_pitch")
	{
		GetAngles(false);
		value = targetpitch;
		return true;
	}
	else if (fieldName == "last_enemy_range")
	{
		value = CCD->ActorManager()->DistanceFrom(LastTargetPoint, Actor);
		return true;
	}
	else if (fieldName == "current_yaw")
	{
		GetAngles(true);
		value = actoryaw;
		return true;
	}
	else if (fieldName == "yaw_speed")
	{
		value = yaw_speed;
		return true;
	}
	else if (fieldName == "ideal_yaw")
	{
		value = ideal_yaw;
		return true;
	}
	else if (fieldName == "current_pitch")
	{
		GetAngles(true);
		value = actorpitch;
		return true;
	}
	else if (fieldName == "pitch_speed")
	{
		value = pitch_speed;
		return true;
	}
	else if (fieldName == "ideal_pitch")
	{
		value = ideal_pitch;
		return true;
	}
	else if (fieldName == "in_pain")
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
	else if (fieldName == "animate_at_end")
	{
		value = CCD->ActorManager()->EndAnimation(Actor);
		return true;
	}
	else if (fieldName == "IsFalling")
	{
		value = false;
		if(CCD->ActorManager()->Falling(Actor)==GE_TRUE)
			value = true;
		return true;
	}
	else if (fieldName == "current_X")
	{
		geVec3d Pos;
		CCD->ActorManager()->GetPosition(Actor, &Pos);
		value = Pos.X;
		return true;
	}
	else if (fieldName == "current_Y")
	{
		geVec3d Pos;
		CCD->ActorManager()->GetPosition(Actor, &Pos);
		value = Pos.Y;
		return true;
	}
	else if (fieldName == "current_Z")
	{
		geVec3d Pos;
		CCD->ActorManager()->GetPosition(Actor, &Pos);
		value = Pos.Z;
		return true;
	}

// Added By Pickles to RF07D --------------------------

	else if (fieldName == "player_Z")
	{
		geVec3d Pos;
		CCD->ActorManager()->GetPosition(CCD->Player()->GetActor(), &Pos);
		value = Pos.Z;
		return true;
	}
	else if (fieldName == "player_Y")
	{
		geVec3d Pos;
		CCD->ActorManager()->GetPosition(CCD->Player()->GetActor(), &Pos);
		value = Pos.Y;
		return true;
	}
	else if (fieldName == "player_X")
	{
		geVec3d Pos;
		CCD->ActorManager()->GetPosition(CCD->Player()->GetActor(), &Pos);
		value = Pos.X;
		return true;
	}
	else if (fieldName == "distancetopoint")
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
	else if (fieldName == "key_pressed")
	{
		value = CCD->Input()->GetKeyboardInputNoWait();
		return true;
	}
	else if (fieldName == "player_vis")
	{
		GetAngles(true);
		bool flag = CCD->Pawns()->CanSee(FOV, Actor, CCD->Player()->GetActor(), FOVBone);
		value = flag;
		return true;
	}
	else if (fieldName == "target_name")
	{
		if(CCD->ActorManager()->IsActor(TargetActor))
			value = skString(CCD->ActorManager()->GetEntityName(TargetActor));
		else
			value = false;
		return true;
	}
	else if (fieldName == "enemy_X")
	{
		geVec3d Pos;
		CCD->ActorManager()->GetPosition(TargetActor, &Pos);
		value = Pos.X;
		return true;
	}
	else if (fieldName == "enemy_Y")
	{
		geVec3d Pos;
		CCD->ActorManager()->GetPosition(TargetActor, &Pos);
		value = Pos.Y;
		return true;
	}
	else if (fieldName == "enemy_Z")
	{
		geVec3d Pos;
		CCD->ActorManager()->GetPosition(TargetActor, &Pos);
		value = Pos.Z;
		return true;
	}

	else if (fieldName == "player_yaw")
	{
		geVec3d Orient;
		CCD->ActorManager()->GetRotation(CCD->Player()->GetActor(), &Orient);
		value = Orient.Y;
		return true;
	}

	else if (fieldName == "point_vis")
	{
	bool flag = CCD->Pawns()->CanSeePoint(FOV, Actor, &CurrentPoint, FOVBone);
	value = flag;
	return true;
	}

	else if (fieldName == "player_weapon")
	{
		value = CCD->Weapons()->GetCurrent();
		return true;
	}

	else if (fieldName == "point_name")
	{
		if(!Point)
		{
			value=skString("FALSE");
			return true;
		}
		value = skString(Point);
		return true;
	}

	else if (fieldName == "camera_pitch")
	{
		value = CCD->CameraManager()->GetTilt();
		return true;
	}

// END Added By Pickles ----------------------------------------

	else if (fieldName == "LODLevel")
	{
		int Level = 0;
		CCD->ActorManager()->GetLODLevel(Actor, &Level);
		value = Level;
		return true;
	}
	else 
	{
		return skScriptedExecutable::getValue(fieldName, attribute, value);
	}

}

// sets a variable's value
bool ScriptedObject::setValue(const skString& fieldName, const skString& attribute, const skRValue& value)
{
	if (fieldName == "lowTime")
	{
		lowTime = value.floatValue();
		return true;
	}
	else if (fieldName == "ThinkTime")
	{
		ThinkTime = value.floatValue();
		return true;
	}
	else if (fieldName == "think")
	{
		strcpy(thinkorder, value.str());
		return true;
	}
	else if (fieldName == "attack_finished")
	{
		attack_finished = value.floatValue();
		return true;
	}
	else if (fieldName == "attack_state")
	{
		attack_state = value.intValue();
		return true;
	}
	else if (fieldName == "yaw_speed")
	{
		yaw_speed = value.floatValue();
		return true;
	}
	else if (fieldName == "ideal_yaw")
	{
		ideal_yaw = value.floatValue();
		return true;
	}
	else if (fieldName == "pitch_speed")
	{
		pitch_speed = value.floatValue();
		return true;
	}
	else if (fieldName == "ideal_pitch")
	{
		ideal_pitch = value.floatValue();
		return true;
	}
	else 
	{
		return skScriptedExecutable::setValue(fieldName, attribute, value);
	}
}

// calls a method in this object
bool ScriptedObject::lowmethod(const skString& methodName, skRValueArray& arguments,skRValue& returnValue)
{
	char param0[128], param4[128];
	bool param2;
	float param1, param3;
	param0[0] = '\0';
	param4[0] = '\0';

	if (IS_METHOD(methodName, "HighLevel"))
	{
		PARMCHECK(1);
		strcpy(param0, arguments[0].str());
		highlevel = true;
		strcpy(thinkorder, param0);
		RunOrder = true;
		ActionActive = false;
		return true;
	}
	else if (IS_METHOD(methodName, "Animate"))
	{
		PARMCHECK(1);
		strcpy(param0, arguments[0].str());
		CCD->ActorManager()->SetMotion(Actor, param0);
		CCD->ActorManager()->SetHoldAtEnd(Actor, false);
		return true;
	}
	else if (IS_METHOD(methodName, "Gravity"))
	{
		PARMCHECK(1);
		bool flag = arguments[0].boolValue();
		float Gravity = 0.0f;
		if(flag)
			Gravity = CCD->Player()->GetGravity();
		CCD->ActorManager()->SetGravity(Actor, Gravity);
		return true;
	}
	
	else if (IS_METHOD(methodName, "PlaySound"))
	{
		PARMCHECK(1);
		strcpy(param0, arguments[0].str());
		if(!EffectC_IsStringNull(param0))
		{
			geVec3d Position;
			CCD->ActorManager()->GetPosition(Actor, &Position);
			Snd 	Sound;
			memset( &Sound, 0, sizeof( Sound ) );
			geVec3d_Copy( &(Position), &( Sound.Pos ) );
			Sound.Min=CCD->GetAudibleRadius();
			if(arguments.entries()==2)
				Sound.Min=arguments[1].floatValue();
			Sound.Loop=GE_FALSE;
			Sound.SoundDef=SPool_Sound(param0);
			if(Sound.SoundDef!=NULL)
				CCD->EffectManager()->Item_Add(EFF_SND, (void *)&Sound);
		}
		return true;
	}
	else if (IS_METHOD(methodName, "EnemyExist"))
	{
		PARMCHECK(1);
		strcpy(param0, arguments[0].str());
		returnValue = 0;
		if(CCD->ActorManager()->IsActor(TargetActor))
		{
			if(TargetActor==CCD->Player()->GetActor())
				strcpy(param0, "health");
			CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(TargetActor);
			if(theInv->Has(param0))
			{
				if(theInv->Value(param0)>0)
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
	else if (IS_METHOD(methodName, "GetEventState"))
	{
		PARMCHECK(1);
		strcpy(param0, arguments[0].str());
		returnValue = (bool)GetTriggerState(param0);
		return true;
	}
	else if (IS_METHOD(methodName, "SetEventState"))
	{
		PARMCHECK(2);
		strcpy(param0, arguments[0].str());
		bool flag = arguments[1].boolValue();
		CCD->Pawns()->AddEvent(param0, flag);
		return true;
	}
	else if (IS_METHOD(methodName, "Integer"))
	{
		PARMCHECK(1);
		int temp = arguments[0].intValue();
		returnValue = (int)temp;
		return true;
	}
	else if (IS_METHOD(methodName, "GetAttribute"))
	{
		PARMCHECK(1);
		strcpy(param0, arguments[0].str());
		CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
		returnValue = (int)theInv->Value(param0);
		return true;
	}
	else if (IS_METHOD(methodName, "ModifyAttribute"))
	{
		PARMCHECK(2);
		strcpy(param0, arguments[0].str());
		CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
		returnValue = (int)theInv->Modify(param0, arguments[1].intValue());
		return true;
	}

// Added By Pickles to RF07D --------------------------

	else if (IS_METHOD(methodName, "SetAttribute"))
	{
		PARMCHECK(2);
		strcpy(param0, arguments[0].str());
		CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
		returnValue = (int)theInv->Set(param0, arguments[1].intValue());
		return true;
	}
	else if (IS_METHOD(methodName, "SetPlayerWeapon"))
	{
		PARMCHECK(1);
		int temp =arguments[0].intValue();
		CCD->Weapons()->SetWeapon(temp);
		return true;
	}
	else if (IS_METHOD(methodName, "SetUseItem"))
	{
		PARMCHECK(1);
		strcpy(param0, arguments[0].str());
		CCD->Player()->SetUseAttribute(param0);
		CCD->HUD()->ActivateElement(param0,true);
		return true;
	}
	else if (IS_METHOD(methodName, "ClearUseItem"))
	{
		PARMCHECK(1);
		strcpy(param0, arguments[0].str());
		CCD->HUD()->ActivateElement(param0,false);
		CCD->Player()->DelUseAttribute(param0);
		return true;
	}
	else if (IS_METHOD(methodName, "StringCopy"))
	{
		PARMCHECK(1);
		strcpy(param0, arguments[0].str());
		returnValue = skString(param0);
		return true;
	}

		else if (IS_METHOD(methodName, "LeftCopy"))
	{
		PARMCHECK(2);
		int temp = arguments[1].intValue();
		char* cstemp="";
		strncpy(cstemp,arguments[0].str(),temp);
		cstemp[temp]='\0';
		returnValue = skString(cstemp);
		return true;
	}
		else if (IS_METHOD(methodName, "IsEntityVisible"))
	{
		PARMCHECK(1);
		strcpy(param0, arguments[0].str());
		bool flag = CCD->Pawns()->CanSee(FOV, Actor,CCD->ActorManager()->GetByEntityName(param0), FOVBone);
		returnValue = flag;
		return true;
	}
		else if (IS_METHOD(methodName, "DamageEntity"))
	{
		PARMCHECK(3);
		float amount = arguments[0].floatValue();
		strcpy(param0, arguments[1].str());
		strcpy(param4, arguments[2].str());
		CCD->Damage()->DamageActor(CCD->ActorManager()->GetByEntityName(param4), amount, param0, amount, param0, "Melee");
		return true;
	}
		else if (IS_METHOD(methodName, "AnimateEntity"))
	{
		PARMCHECK(3);
		strcpy(param0, arguments[0].str());
		strcpy(param4, arguments[1].str());
		param2 = arguments[2].boolValue();
		CCD->ActorManager()->SetMotion(CCD->ActorManager()->GetByEntityName(param4), param0);
		CCD->ActorManager()->SetHoldAtEnd(CCD->ActorManager()->GetByEntityName(param4), param2);
		return true;
	}
	else if (IS_METHOD(methodName, "AnimateHold"))
	{
		PARMCHECK(1);
		strcpy(param0, arguments[0].str());
		CCD->ActorManager()->SetMotion(Actor, param0);
		CCD->ActorManager()->SetHoldAtEnd(Actor, true);
		return true;
	}
		else if (IS_METHOD(methodName, "AnimateTarget"))
	{
		PARMCHECK(2);
		strcpy(param0, arguments[0].str());
		param2 = arguments[1].boolValue();
		CCD->ActorManager()->SetMotion(TargetActor, param0);
		CCD->ActorManager()->SetHoldAtEnd(TargetActor, param2);
		return true;
	}
		else if (IS_METHOD(methodName, "GetEntityX"))
	{
		PARMCHECK(1);
		strcpy(param0, arguments[0].str());
		geVec3d Pos;
		CCD->ActorManager()->GetPosition(CCD->ActorManager()->GetByEntityName(param0), &Pos);
		returnValue = Pos.X;
		return true;
	}
		else if (IS_METHOD(methodName, "GetEntityY"))
	{
		PARMCHECK(1);
		strcpy(param0, arguments[0].str());
		geVec3d Pos;
		CCD->ActorManager()->GetPosition(CCD->ActorManager()->GetByEntityName(param0), &Pos);
		returnValue = Pos.Y;
		return true;
	}
		else if (IS_METHOD(methodName, "GetEntityZ"))
	{
		PARMCHECK(1);
		strcpy(param0, arguments[0].str());
		geVec3d Pos;
		CCD->ActorManager()->GetPosition(CCD->ActorManager()->GetByEntityName(param0), &Pos);
		returnValue = Pos.Z;
		return true;
	}
		else if (IS_METHOD(methodName, "IsKeyDown"))
	{
		// Changed RF071A Picklkes
		PARMCHECK(1);
		int temp = arguments[0].intValue();
		returnValue=false;
		if(CCD->Input()->GetKeyCheck(temp) == true)
				returnValue=true;
		return true;
	}

		else if (IS_METHOD(methodName, "GetEntityYaw"))
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

	else if (IS_METHOD(methodName, "MatchEntityAngles"))
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

	else if (IS_METHOD(methodName, "FacePoint"))
	{
		if(ValidPoint)
			CCD->ActorManager()->RotateToFacePoint(Actor,CurrentPoint);
	return true;
	}

	else if (IS_METHOD(methodName, "NewPoint"))
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

	//ADDED TO RF071 BY PICKLES	

	else if (IS_METHOD(methodName, "GetPointYaw"))
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
				returnValue=pRotation.Y;
				}
			}
	return true;
	}


	else if (IS_METHOD(methodName, "NextPoint"))
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

	else if (IS_METHOD(methodName, "SetTarget"))
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

	else if (IS_METHOD(methodName, "GetDistanceTo"))
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

	else if (IS_METHOD(methodName, "TeleportEntity"))
	{
	PARMCHECK(2);
	strcpy(param0, arguments[1].str());
	strcpy(param4, arguments[0].str());
	char *EType = CCD->EntityRegistry()->GetEntityType(param0);
	geActor *tActor;
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
				CCD->ActorManager()->Position(tActor, Pos);

				// Added Pickles RF071
				if(WeaponActor)
					CCD->ActorManager()->Position(WeaponActor, Pos);
				// End Added Pickles RF071
			}
		}
	return true;
	}

	else if (IS_METHOD(methodName, "SaveAttributes"))
	{
	PARMCHECK(1);
	strcpy(param0, arguments[0].str());
	CCD->Player()->SaveAttributesAscii(param0);
	return true;
	}

	else if (IS_METHOD(methodName, "TraceToActor"))
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
	Pos=OldPos;
	geXForm3d_SetIdentity(&Xf);
	geXForm3d_RotateZ(&Xf, theRotation.Z);
	geXForm3d_RotateX(&Xf, theRotation.X);
	geXForm3d_RotateY(&Xf, theRotation.Y);
	geXForm3d_Translate(&Xf, Pos.X, Pos.Y, Pos.Z);
	geXForm3d_GetUp(&Xf, &Direction);
	geVec3d_AddScaled (&Pos, &Direction, arguments[2].floatValue(), &Pos);
	geXForm3d_GetLeft(&Xf, &Direction);
	geVec3d_AddScaled (&Pos, &Direction, arguments[1].floatValue(), &Pos);
	geXForm3d_GetIn(&Xf, &Direction);
	geVec3d_AddScaled (&Pos, &Direction, arguments[3].floatValue(), &Pos);
		if(CCD->ActorManager()->DoesRayHitActor(OldPos, Pos, &pActor, Actor, &T, &Normal) == GE_TRUE)
			returnValue = skString(CCD->ActorManager()->GetEntityName(pActor));
	return true;
	}

	else if (IS_METHOD(methodName, "AnimateBlend"))
	{
	PARMCHECK(2);
	strcpy(param0, arguments[0].str());
	param1 = arguments[1].floatValue();
	if(param1 > 0.0f)
		{
		CCD->ActorManager()->SetTransitionMotion(Actor, param0, param1, NULL);
		CCD->ActorManager()->SetHoldAtEnd(Actor, true);
		}
	return true;
	}

	else if (IS_METHOD(methodName, "AnimationSpeed"))
	{
		PARMCHECK(1);
		CCD->ActorManager()->SetAnimationSpeed(Actor,arguments[0].floatValue());
		return true;
	}

	else if (IS_METHOD(methodName, "SetCollision"))
	{
		CCD->ActorManager()->SetCollide(Actor);
			if(WeaponActor)
			{
				CCD->ActorManager()->SetCollide(WeaponActor);
				CCD->ActorManager()->SetNoCollide(WeaponActor);
			}
		return true;
	}

	else if (IS_METHOD(methodName, "SetNoCollision"))
	{
		CCD->ActorManager()->SetNoCollide(Actor);
		return true;
	}

	else if (IS_METHOD(methodName, "DamageArea"))
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

	else if (IS_METHOD(methodName, "PlayerMatchAngles"))
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

	else if (IS_METHOD(methodName, "ConvertDegrees"))
	{
		PARMCHECK(1);
		param3 = arguments[0].floatValue();
		returnValue = param3*0.0174532925199433f;
		return true;
	}


	else if (IS_METHOD(methodName, "AttachCamera"))
	{
		CCD->CameraManager()->BindToActor(Actor);
		return true;
	}

	else if (IS_METHOD(methodName, "AttachCameraToBone"))
	{
		PARMCHECK(1);
		strcpy(param0, arguments[0].str());
		CCD->CameraManager()->BindToActorBone(Actor,param0);
		return true;
	}

	else if (IS_METHOD(methodName, "AttachCameraToEntity"))
	{
		PARMCHECK(2);
		strcpy(param0, arguments[0].str());	
		strcpy(param4, arguments[1].str());
		CCD->CameraManager()->BindToActorBone(CCD->ActorManager()->GetByEntityName(param0),param4);
		return true;
	}

	else if (IS_METHOD(methodName, "DetachCamera"))
	{
		CCD->CameraManager()->BindToActor(CCD->Player()->GetActor());
		return true;
	}

	else if (IS_METHOD(methodName, "TiltCamera"))
	{
		PARMCHECK(1);
		param3 = arguments[0].floatValue();
		if(param3 < 0)
		{
		CCD->CameraManager()->TiltUp(param3*(+1));
		}
		else
		{
		CCD->CameraManager()->TiltDown(param3);
		}
		return true;
	}

	else if (IS_METHOD(methodName, "PositionToPlatform"))
	{
		//Changed Pickles rf07A
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
		//Changed Pickles rf07A
	}

	else if (IS_METHOD(methodName, "ActivateTrigger"))
	{
		PARMCHECK(1);
		strcpy(param0, arguments[0].str());
		CCD->Triggers()->HandleTriggerEvent(param0);
		return true;
	}
	// Added Pickles to RF071A
	else if (IS_METHOD(methodName, "UpdateEnemyVis"))
	{
		GetAngles(true);
		if(!TargetActor)
			return true;
		CCD->ActorManager()->GetPosition(TargetActor, &LastTargetPoint);
		geExtBox theBox;
		CCD->ActorManager()->GetBoundingBox(TargetActor, &theBox);
		LastTargetPoint.Y += ((theBox.Max.Y)/2.0f);
		return true;
	}

	else if (IS_METHOD(methodName, "TargetPlayer"))
	{
		TargetActor = CCD->Player()->GetActor();
		return true;
	}

	else if (IS_METHOD(methodName, "FireProjectileBlind"))
	{
	PARMCHECK(6);
	geXForm3d Xf;
	geVec3d Pos, theRotation, Direction, Orient,tPoint;
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
		geXForm3d_SetIdentity(&Xf);
		geXForm3d_RotateZ(&Xf, theRotation.Z);
		geXForm3d_RotateX(&Xf, theRotation.X);
		geXForm3d_RotateY(&Xf, theRotation.Y);
		geXForm3d_Translate(&Xf, Pos.X, Pos.Y, Pos.Z);
		geXForm3d_GetIn(&Xf, &Direction);
		geVec3d_AddScaled (&Pos, &Direction, 1000.0f, &tPoint);
		geXForm3d_GetUp(&Xf, &Direction);
		geVec3d_AddScaled (&Pos, &Direction, arguments[3].floatValue(), &Pos);
		geXForm3d_GetLeft(&Xf, &Direction);
		geVec3d_AddScaled (&Pos, &Direction, arguments[2].floatValue(), &Pos);
		geXForm3d_GetIn(&Xf, &Direction);
		geVec3d_AddScaled (&Pos, &Direction, arguments[4].floatValue(), &Pos);
		geVec3d_Subtract(&tPoint, &Pos, &Orient);
		float l = geVec3d_Length(&Orient);
			if(l > 0.0f) 
			{
				float x = Orient.X;
				Orient.X = (float)( GE_PI*0.5 ) - (float)acos(Orient.Y / l);
				Orient.Y = (float)atan2( x , Orient.Z ) + GE_PI;
				// roll is zero - always!!?
				Orient.Z = 0.0;
				CCD->Weapons()->Add_Projectile(Pos, Pos, Orient, param4, dAttrib, dAttrib);
			}
	return true;
	}

	else if (IS_METHOD(methodName, "SetTargetPoint"))
	{
	PARMCHECK(3);
	geXForm3d Xf;
	geVec3d Pos, theRotation, Direction;
	CCD->ActorManager()->GetPosition(Actor, &Pos);
	CCD->ActorManager()->GetRotate(Actor, &theRotation);
	geXForm3d_SetIdentity(&Xf);
	geXForm3d_RotateZ(&Xf, theRotation.Z);
	geXForm3d_RotateX(&Xf, theRotation.X);
	geXForm3d_RotateY(&Xf, theRotation.Y);
	geXForm3d_Translate(&Xf, Pos.X, Pos.Y, Pos.Z);
	geXForm3d_GetUp(&Xf, &Direction);
	geVec3d_AddScaled (&Pos, &Direction, arguments[1].floatValue(), &Pos);
	geXForm3d_GetLeft(&Xf, &Direction);
	geVec3d_AddScaled (&Pos, &Direction, arguments[0].floatValue(), &Pos);
	geXForm3d_GetIn(&Xf, &Direction);
	geVec3d_AddScaled (&Pos, &Direction, arguments[2].floatValue(), &Pos);
	UpdateTargetPoint=Pos;
	return true;
	}
//-------------------- End Added by Pickles -----------------------------------

	else if (IS_METHOD(methodName, "ChangeYaw"))
	{
		ChangeYaw();
		return true;
	}
	else if (IS_METHOD(methodName, "ChangePitch"))
	{
		ChangePitch();
		return true;
	}
	else if (IS_METHOD(methodName, "random"))
	{
		PARMCHECK(2);
		param1 = arguments[0].floatValue();
		param3 = arguments[1].floatValue();
		if(param1<=param3)
			returnValue = (int)EffectC_Frand(param1, param3);
		else
			returnValue = (int)EffectC_Frand(param3, param1);
		return true;
	}
	else if (IS_METHOD(methodName, "walkmove"))
	{
		PARMCHECK(2);
		float amount = arguments[1].floatValue() * ElapseTime;
		geXForm3d Xform;
		geVec3d In, NewPosition, SavedPosition;
		CCD->ActorManager()->GetPosition(Actor, &SavedPosition);
		geXForm3d_SetIdentity(&Xform);
		geXForm3d_RotateY(&Xform, arguments[0].floatValue());
		geXForm3d_Translate(&Xform, SavedPosition.X, SavedPosition.Y, SavedPosition.Z);
		geXForm3d_GetIn(&Xform, &In);
		geVec3d_AddScaled(&SavedPosition, &In, amount, &NewPosition);
		returnValue = false;
		if(CCD->ActorManager()->ValidateMove(SavedPosition, NewPosition, Actor, false)==GE_TRUE)
			returnValue = true;
		return true;
	}
	else if (IS_METHOD(methodName, "flymove"))
	{
		PARMCHECK(3);
		float amount = arguments[2].floatValue() * ElapseTime;
		geXForm3d Xform;
		geVec3d In, NewPosition, SavedPosition;
		CCD->ActorManager()->GetPosition(Actor, &SavedPosition);
		geXForm3d_SetIdentity(&Xform);
		geXForm3d_RotateX(&Xform, -arguments[0].floatValue());
		geXForm3d_RotateY(&Xform, arguments[1].floatValue());
		geXForm3d_Translate(&Xform, SavedPosition.X, SavedPosition.Y, SavedPosition.Z);
		geXForm3d_GetIn(&Xform, &In);
		geVec3d_AddScaled(&SavedPosition, &In, amount, &NewPosition);
		returnValue = false;
		if(CCD->ActorManager()->ValidateMove(SavedPosition, NewPosition, Actor, false)==GE_TRUE)
			returnValue = true;
		return true;
	}
	else if (IS_METHOD(methodName, "Damage"))
	{
		PARMCHECK(2);
		float amount = arguments[0].floatValue();
		strcpy(param0, arguments[1].str());
		CCD->Damage()->DamageActor(TargetActor, amount, param0, amount, param0, "Melee");
		return true;
	}
	else if (IS_METHOD(methodName, "DamagePlayer"))
	{
		PARMCHECK(2);
		float amount = arguments[0].floatValue();
		strcpy(param0, arguments[1].str());
		CCD->Damage()->DamageActor(CCD->Player()->GetActor(), amount, param0, amount, param0, "Melee");
		return true;
	}
	else if (IS_METHOD(methodName, "PositionToPlayer"))
	{
		PARMCHECK(3);
		geXForm3d Xf;
		geVec3d Pos, theRotation, Direction;

		Pos = CCD->Player()->Position();

		if (arguments.entries()==4)
		{
			bool flag = arguments[3].boolValue();
			if(flag)
			{
				CCD->ActorManager()->GetRotate(CCD->Player()->GetActor(), &theRotation);
				geXForm3d_SetIdentity(&Xf);
				geXForm3d_RotateZ(&Xf, theRotation.Z);
				geXForm3d_RotateX(&Xf, theRotation.X);
				geXForm3d_RotateY(&Xf, theRotation.Y);
				geXForm3d_Translate(&Xf, Pos.X, Pos.Y, Pos.Z);
				geXForm3d_GetUp(&Xf, &Direction);
				geVec3d_AddScaled (&Pos, &Direction, arguments[1].floatValue(), &Pos);
				geXForm3d_GetLeft(&Xf, &Direction);
				geVec3d_AddScaled (&Pos, &Direction, arguments[0].floatValue(), &Pos);
				geXForm3d_GetIn(&Xf, &Direction);
				geVec3d_AddScaled (&Pos, &Direction, arguments[2].floatValue(), &Pos);
				CCD->ActorManager()->Position(Actor, Pos);
				CCD->ActorManager()->Rotate(Actor, theRotation);
				return true;
			}
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
	else if (IS_METHOD(methodName, "PlayerToPosition"))
	{
		PARMCHECK(3);
		geXForm3d Xf;
		geVec3d Pos, theRotation, Direction;

		CCD->ActorManager()->GetPosition(Actor, &Pos);

		if (arguments.entries() > 3) // Changed by Pickles RF07D
		{
			bool flag = arguments[3].boolValue();
			if(flag)
			{
				CCD->ActorManager()->GetRotate(Actor, &theRotation);
				geXForm3d_SetIdentity(&Xf);
				geXForm3d_RotateZ(&Xf, theRotation.Z);
				geXForm3d_RotateX(&Xf, theRotation.X);
				geXForm3d_RotateY(&Xf, theRotation.Y);
				geXForm3d_Translate(&Xf, Pos.X, Pos.Y, Pos.Z);
				geXForm3d_GetUp(&Xf, &Direction);
				geVec3d_AddScaled (&Pos, &Direction, arguments[1].floatValue(), &Pos);
				geXForm3d_GetLeft(&Xf, &Direction);
				geVec3d_AddScaled (&Pos, &Direction, arguments[0].floatValue(), &Pos);
				geXForm3d_GetIn(&Xf, &Direction);
				geVec3d_AddScaled (&Pos, &Direction, arguments[2].floatValue(), &Pos);
				CCD->ActorManager()->Position(CCD->Player()->GetActor(), Pos);
			// Added By Pickles to RF07D
			if (arguments.entries() == 5)
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

	else if (IS_METHOD(methodName, "PositionToPawn"))
	{
		PARMCHECK(4);
		geXForm3d Xf;
		geVec3d Pos, theRotation, Direction;
		geActor *MasterActor;

		strcpy(param0, arguments[0].str());
		MasterActor = CCD->ActorManager()->GetByEntityName(param0);
		if(!MasterActor)
			return true;
		CCD->ActorManager()->GetPosition(MasterActor, &Pos);

		if (arguments.entries() > 4) // Changed by Pickles RF07D
		{
			bool flag = arguments[4].boolValue();
			if(flag)
			{
				CCD->ActorManager()->GetRotate(MasterActor, &theRotation);
				geXForm3d_SetIdentity(&Xf);
				geXForm3d_RotateZ(&Xf, theRotation.Z);
				geXForm3d_RotateX(&Xf, theRotation.X);
				geXForm3d_RotateY(&Xf, theRotation.Y);
				geXForm3d_Translate(&Xf, Pos.X, Pos.Y, Pos.Z);
				geXForm3d_GetUp(&Xf, &Direction);
				geVec3d_AddScaled (&Pos, &Direction, arguments[2].floatValue(), &Pos);
				geXForm3d_GetLeft(&Xf, &Direction);
				geVec3d_AddScaled (&Pos, &Direction, arguments[1].floatValue(), &Pos);
				geXForm3d_GetIn(&Xf, &Direction);
				geVec3d_AddScaled (&Pos, &Direction, arguments[3].floatValue(), &Pos);
				CCD->ActorManager()->Position(Actor, Pos);
				// Added by Pickles ---------------
				if(arguments.entries() == 6)
				{
					bool flag1 = arguments[5].boolValue();
					if(flag1)
					CCD->ActorManager()->Rotate(Actor, theRotation);
				}
				else
				{
					CCD->ActorManager()->Rotate(Actor, theRotation);
				}
					// END Added by Pickles ---------------
				// Added Pickles RF071
				if(WeaponActor)
					CCD->ActorManager()->Position(WeaponActor, Pos);
				// End Added Pickles RF071

				return true;
			}
		}

		Pos.X += arguments[1].floatValue();
		Pos.Y += arguments[2].floatValue();
		Pos.Z += arguments[3].floatValue();
		CCD->ActorManager()->Position(Actor, Pos);

		return true;
	}
	else if (IS_METHOD(methodName, "SetKeyPause"))
	{
		PARMCHECK(1);
		bool flag = arguments[0].boolValue();
		CCD->SetKeyPaused(flag);
		return true;
	}
	else if (IS_METHOD(methodName, "PlayerRender"))
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
	else if (IS_METHOD(methodName, "PawnRender"))
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
	else if (IS_METHOD(methodName, "ChangeMaterial"))
	{
		PARMCHECK(1);
		strcpy(param0, arguments[0].str());
		if(!EffectC_IsStringNull(param0))
			CCD->ActorManager()->ChangeMaterial(Actor, param0);
		return true;
	}
	else if (IS_METHOD(methodName, "SetHoldAtEnd"))
	{
		PARMCHECK(1);
		bool amount = arguments[0].boolValue();
		CCD->ActorManager()->SetHoldAtEnd(Actor, amount);
		return true;
	}
	else if (IS_METHOD(methodName, "ForceUp"))
	{
		PARMCHECK(1);
		float amount = arguments[0].floatValue();
		geVec3d theUp;
		CCD->ActorManager()->UpVector(Actor, &theUp);
		CCD->ActorManager()->SetForce(Actor, 0, theUp, amount, amount);
		return true;
	}
	else if (IS_METHOD(methodName, "ForceDown"))
	{
		PARMCHECK(1);
		float amount = arguments[0].floatValue();
		geVec3d theUp;
		CCD->ActorManager()->UpVector(Actor, &theUp);
		geVec3d_Inverse(&theUp);
		CCD->ActorManager()->SetForce(Actor, 0, theUp, amount, amount);
		return true;
	}
	else if (IS_METHOD(methodName, "ForceRight"))
	{
		PARMCHECK(1);
		float amount = arguments[0].floatValue();
		geVec3d theUp;
		CCD->ActorManager()->LeftVector(Actor, &theUp);
		geVec3d_Inverse(&theUp);
		CCD->ActorManager()->SetForce(Actor, 1, theUp, amount, amount);
		return true;
	}
	else if (IS_METHOD(methodName, "ForceLeft"))
	{
		PARMCHECK(1);
		float amount = arguments[0].floatValue();
		geVec3d theUp;
		CCD->ActorManager()->LeftVector(Actor, &theUp);
		CCD->ActorManager()->SetForce(Actor, 1, theUp, amount, amount);
		return true;
	}
	else if (IS_METHOD(methodName, "ForceForward"))
	{
		PARMCHECK(1);
		float amount = arguments[0].floatValue();
		geVec3d theUp;
		CCD->ActorManager()->InVector(Actor, &theUp);
		CCD->ActorManager()->SetForce(Actor, 2, theUp, amount, amount);
		return true;
	}
	else if (IS_METHOD(methodName, "ForceBackward"))
	{
		PARMCHECK(1);
		float amount = arguments[0].floatValue();
		geVec3d theUp;
		CCD->ActorManager()->InVector(Actor, &theUp);
		geVec3d_Inverse(&theUp);
		CCD->ActorManager()->SetForce(Actor, 2, theUp, amount, amount);
		return true;
	}
	else if (IS_METHOD(methodName, "movetogoal"))
	{
		float amount = arguments[0].floatValue();
		MoveToGoal(amount);
		return true;
	}
	else if (IS_METHOD(methodName, "UpdateTarget"))
	{
		UpdateTargetPoint = LastTargetPoint;
		return true;
	}
	else if (IS_METHOD(methodName, "FireProjectile"))
	{
		PARMCHECK(6);
		geXForm3d Xf;
		geVec3d theRotation, Pos, Direction, Orient, TargetPoint;
		geFloat x;
		geExtBox theBox;

		CCD->ActorManager()->GetBoundingBox(TargetActor, &theBox);
		TargetPoint = UpdateTargetPoint;
		TargetPoint.Y -= (theBox.Max.Y/2.0f);
		if(arguments.entries()==7)
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
			geXForm3d_SetIdentity(&Xf);
			geXForm3d_RotateZ(&Xf, theRotation.Z);
			geXForm3d_RotateX(&Xf, theRotation.X);
			geXForm3d_RotateY(&Xf, theRotation.Y);
			geXForm3d_Translate(&Xf, Pos.X, Pos.Y, Pos.Z);
			geXForm3d_GetUp(&Xf, &Direction);
			geVec3d_AddScaled (&Pos, &Direction, FireOffsetY, &Pos);
			geXForm3d_GetLeft(&Xf, &Direction);
			geVec3d_AddScaled (&Pos, &Direction, FireOffsetX, &Pos);
			geXForm3d_GetIn(&Xf, &Direction);
			geVec3d_AddScaled (&Pos, &Direction, FireOffsetZ, &Pos);
			geVec3d_Subtract(&TargetPoint, &Pos, &Orient);
			float l = geVec3d_Length(&Orient);
			if(l > 0.0f) 
			{
				x = Orient.X;
				Orient.X = (float)( GE_PI*0.5 ) - (float)acos(Orient.Y / l);
				Orient.Y = (float)atan2( x , Orient.Z ) + GE_PI;
				// roll is zero - always!!?
				Orient.Z = 0.0;
				CCD->Weapons()->Add_Projectile(Pos, Pos, Orient, param4, DamageAttr, DamageAttr);
			}
		}
		return true;
	}
	else if (IS_METHOD(methodName, "AddExplosion"))
	{
		PARMCHECK(5);
		geXForm3d Xf;
		geVec3d theRotation, Pos, Direction;

		strcpy(param0, arguments[0].str());
		strcpy(param4, arguments[1].str());
		float OffsetX = arguments[2].floatValue();
		float OffsetY = arguments[3].floatValue();
		float OffsetZ = arguments[4].floatValue();
		if(geActor_GetBoneTransform(Actor, param4, &Xf))
		{
			geVec3d_Copy(&(Xf.Translation), &Pos);
			CCD->ActorManager()->GetRotate(Actor, &theRotation);
			geXForm3d_SetIdentity(&Xf);
			geXForm3d_RotateZ(&Xf, theRotation.Z);
			geXForm3d_RotateX(&Xf, theRotation.X);
			geXForm3d_RotateY(&Xf, theRotation.Y);
			geXForm3d_Translate(&Xf, Pos.X, Pos.Y, Pos.Z);
			geXForm3d_GetUp(&Xf, &Direction);
			geVec3d_AddScaled (&Pos, &Direction, OffsetY, &Pos);
			geXForm3d_GetLeft(&Xf, &Direction);
			geVec3d_AddScaled (&Pos, &Direction, OffsetX, &Pos);
			geXForm3d_GetIn(&Xf, &Direction);
			geVec3d_AddScaled (&Pos, &Direction, OffsetZ, &Pos);
			Pos.X = OffsetX;
			Pos.Y = OffsetY;
			Pos.Z = OffsetZ;
			CCD->Explosions()->AddExplosion(param0, Pos, Actor, param4);
		}
		return true;
	}
	else if (IS_METHOD(methodName, "PortIn"))
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
	else if (IS_METHOD(methodName, "debug"))
	{
		PARMCHECK(1);
		strcpy(param0, arguments[0].str());
		if(console)
		{
			int index = -1;
			int i;
			for(i=0;i<DEBUGLINES;i++)
			{
				if(EffectC_IsStringNull(ConsoleDebug[i]))
				{
					index = i;
					break;
				}
			}
			if(index!=-1)
			{
				strcpy(ConsoleDebug[index], param0);
			}
			else
			{
				for(i=1;i<DEBUGLINES;i++)
				{
					strcpy(ConsoleDebug[i-1], ConsoleDebug[i]);
				}
				strcpy(ConsoleDebug[DEBUGLINES-1], param0); 
			}
		}
		return true;
	}
	else
	{
		return skScriptedExecutable::method(methodName, arguments, returnValue);
	}
}

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
		Orient.X = (float)( GE_PI*0.5 ) - 
			(float)acos(Orient.Y / l);
		Orient.Y = (float)atan2(x , Orient.Z ) + GE_PI;
		CCD->ActorManager()->GetRotate(Actor, &Pos);
		while(Pos.Y<0.0f)
		{
			Pos.Y+=(GE_PI*2);
		}
		while(Pos.Y>=(GE_PI*2))
		{
			Pos.Y-=(GE_PI*2);
		} 
		while(Pos.X<0.0f)
		{
			Pos.X+=(GE_PI*2);
		}
		while(Pos.X>=(GE_PI*2))
		{
			Pos.X-=(GE_PI*2);
		}
	}
	actoryaw = Pos.Y;
	targetyaw = Orient.Y;
	actorpitch = Pos.X;
	targetpitch = -Orient.X;
}

//
// Quake 2 MoveToGoal functions
//
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
	for	(x=0 ; x<=1 ; x++)
	{
		for	(y=0 ; y<=1 ; y++)
		{
			Start.X = x ? theBox.Max.X : theBox.Min.X;
			Start.Z = y ? theBox.Max.Z : theBox.Min.Z;
			trace = CCD->Collision()->Trace(&Start, &Stop, NULL, NULL, Actor, &Collision);
			if (trace.fraction == 1.0f)
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
	Stop.Y = Start.Y - (2*MaxStep);
	trace = CCD->Collision()->Trace(&Start, &Stop, NULL, NULL, Actor, &Collision);

	if (trace.fraction == 1.0)
		return false;
	mid = bottom = trace.endpos.Y;

	for	(x=0 ; x<=1 ; x++)
	{
		for	(y=0 ; y<=1 ; y++)
		{
			Start.X = Stop.X = x ? theBox.Max.X : theBox.Min.X;
			Start.Z = Stop.Z = y ? theBox.Max.Z : theBox.Min.Z;
			
			trace = CCD->Collision()->Trace(&Start, &Stop, NULL, NULL, Actor, &Collision);
			
			if (trace.fraction != 1.0 && trace.endpos.Y > bottom)
				bottom = trace.endpos.Y;
			if (trace.fraction == 1.0 || mid - trace.endpos.Y > MaxStep)
				return false;
		}
	} 
	return true;
}

bool ScriptedObject::MoveStep(geVec3d move)
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
	end.Y -= stepsize*2;
	CCD->ActorManager()->GetBoundingBox(Actor, &theBox);
	trace = CCD->Collision()->Trace(&neworg, &end, &theBox.Min, &theBox.Max, Actor, &Collision);
	if (trace.allsolid)
	{
		return false;
	}
	if (trace.startsolid)
	{
		neworg.Y -= stepsize;
		trace = CCD->Collision()->Trace(&neworg, &end, &theBox.Min, &theBox.Max, Actor, &Collision);
		if (trace.allsolid || trace.startsolid)
			return false;
	}
	if (trace.fraction == 1.0f)
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

void ScriptedObject::ChangeYaw()
{
	GetAngles(true);
	bool RotateLeft = false;
	float idealangle = anglemod(ideal_yaw/0.0174532925199433f)*0.0174532925199433f;
	float RotateAmt = actoryaw - idealangle;
	
	if(RotateAmt<0.0f)
	{
		RotateAmt = -RotateAmt;
		RotateLeft = true;
	}
	if(RotateAmt>0.0f)
	{
		if(RotateAmt>GE_PI)
		{
		// changed QuestOfDreams 09/21/03
		RotateAmt=2.0f*GE_PI-RotateAmt;
		//RotateAmt-=GE_PI;
			if(RotateLeft)
				RotateLeft=false;
			else
				RotateLeft=true;
		}
		RotateAmt /= 0.0174532925199433f;
		
		float amount = yaw_speed * ElapseTime;
		if(amount>RotateAmt)
		{
			amount = RotateAmt;
		}
	// changed QuestOfDreams 09/21/03
	// RotateAmt -= amount;
		if(RotateLeft)
			CCD->ActorManager()->TurnRight(Actor,0.0174532925199433f*amount);
		else
			CCD->ActorManager()->TurnLeft(Actor,0.0174532925199433f*amount);
	}
}

void ScriptedObject::ChangePitch()
{
	GetAngles(true);
	bool RotateUp = false;
	float idealangle = anglemod(ideal_pitch/0.0174532925199433f)*0.0174532925199433f;
	float RotateAmt = actorpitch - idealangle;
	
	if(RotateAmt<0.0f)
	{
		RotateAmt = -RotateAmt;
		RotateUp = true;
	}
	if(RotateAmt>0.0f)
	{
		if(RotateAmt>GE_PI)
		{
		// changed QuestOfDreams 09/21/03
		RotateAmt=2.0f*GE_PI-RotateAmt;//RotateAmt-=GE_PI;
			if(RotateUp)
				RotateUp=false;
			else
				RotateUp=true;
		}
		RotateAmt /= 0.0174532925199433f;
		
		float amount = pitch_speed * ElapseTime;
		if(amount>RotateAmt)
		{
			amount = RotateAmt;
		}
		// changed QuestOfDreams 09/21/03
		// RotateAmt -= amount;
		if(RotateUp)
			CCD->ActorManager()->TiltUp(Actor,0.0174532925199433f*amount);
		else
			CCD->ActorManager()->TiltDown(Actor,0.0174532925199433f*amount);
	}
}

bool ScriptedObject::StepDirection(float yaw, float dist)
{
	geVec3d move, oldorigin;
	float delta;
	geXForm3d Xform;
	geVec3d In;

	ideal_yaw = 0.0174532925199433f*yaw;
	ChangeYaw();

	CCD->ActorManager()->GetPosition(Actor, &oldorigin);
	geXForm3d_SetIdentity(&Xform);
	geXForm3d_RotateY(&Xform, ideal_yaw);
	geXForm3d_Translate(&Xform, oldorigin.X, oldorigin.Y, oldorigin.Z);
	geXForm3d_GetIn(&Xform, &In);
	geVec3d_Scale(&In, dist, &move);

	if(MoveStep(move))
	{
		GetAngles(true);
		float selfangle = anglemod(actoryaw/0.0174532925199433f);
		float idealangle = anglemod(ideal_yaw/0.0174532925199433f);
		delta = anglemod(selfangle - idealangle);
		if(!(delta > 315 || delta < 45))
		{
			CCD->ActorManager()->Position(Actor, oldorigin);
		}
		return true;
	}
	return false;
}

#define	DI_NODIR	-1.0f

void ScriptedObject::NewChaseDir(float dist)
{
	float	deltax,deltay;
	geVec3d	Pos;
	float d[3];
	float	tdir, olddir, turnaround;

	float yaw = ideal_yaw/0.0174532925199433f;
	olddir = anglemod((int)(yaw/45.0f)*45.0f);
	turnaround = anglemod(olddir - 180.0f);

	CCD->ActorManager()->GetPosition(Actor, &Pos);
	deltax = LastTargetPoint.X - Pos.X;
	deltay = LastTargetPoint.Z - Pos.Z;

	if(deltax>10.0f)
		d[1]= 0.0f;
	else if(deltax<-10.0f)
		d[1]= 180.0f;
	else
		d[1]= DI_NODIR;

	if(deltay<-10.0f)
		d[2]= 270.0f;
	else if(deltay>10.0f)
		d[2]= 90.0f;
	else
		d[2]= DI_NODIR;

	if (d[1] != DI_NODIR && d[2] != DI_NODIR)
	{
		if (d[1] == 0)
			tdir = d[2] == 90.0f ? 45.0f : 315.0f; 
		else
			tdir = d[2] == 90.0f ? 135.0f : 215.0f; 
			
		if (tdir != turnaround && StepDirection(tdir, dist))
			return;
	}
	if ( ((rand()&3) & 1) ||  fabs(deltay)>fabs(deltax))
	{
		tdir=d[1];
		d[1]=d[2];
		d[2]=tdir;
	}

	if (d[1]!=DI_NODIR && d[1]!=turnaround 
		&& StepDirection(d[1], dist))
			return;

	if (d[2]!=DI_NODIR && d[2]!=turnaround
		&& StepDirection(d[2], dist))
			return;

	if (olddir!=DI_NODIR && StepDirection(olddir, dist))
			return;

	if (rand()&1) 
	{
		for (int ttdir=0 ; ttdir<=315 ; ttdir += 45)
		{
			tdir = (float)ttdir;
			if (tdir!=turnaround && StepDirection(tdir, dist) )
					return;
		}
	}
	else
	{
		for (int ttdir=315 ; ttdir >=0 ; ttdir -= 45)
		{
			tdir = (float)ttdir;
			if (tdir!=turnaround && StepDirection(tdir, dist) )
					return;
		}
	}

	if (turnaround != DI_NODIR && StepDirection(turnaround, dist) )
			return;

	ideal_yaw = olddir*0.0174532925199433f;
}

float ScriptedObject::anglemod(float a)
{
	a = (360.0f/65536.0f) * ((int)(a*(65536.0f/360.0f)) & 65535);
	return a;
}

bool ScriptedObject::CloseEnough(float dist)
{
	geExtBox theBox, theBox1;
	CCD->ActorManager()->GetBoundingBox(TargetActor, &theBox);
	CCD->ActorManager()->GetBoundingBox(Actor, &theBox1);
	float delta = CCD->ActorManager()->DistanceFrom(LastTargetPoint, Actor) - theBox.Max.X - theBox1.Max.X;

	if(delta<dist)
		return true;
	return false;
}

void ScriptedObject::MoveToGoal(float dist)
{
	dist = dist*ElapseTime;
	if(CloseEnough(dist))
		return;
	if (!StepDirection (ideal_yaw/0.0174532925199433f, dist))
	{
		NewChaseDir (dist);
	}
}

//
// end Quake2 MoveToGoal
//

void CPawn::TickLow(Pawn *pSource, ScriptedObject *Object, float dwTicks)
{
	skRValueArray args(1);
	skRValue ret;

	if(Object->console)
	{
		char szBug[256];
		sprintf(szBug, "%s %s",Object->szName, Object->Order);
		strcpy(Object->ConsoleHeader, szBug);
	}
	Object->lowTime += (dwTicks*0.001f);
	Object->ElapseTime += (dwTicks*0.001f);
	Object->ThinkTime -= (dwTicks*0.001f);
	if(Object->ThinkTime>0.0f)
		return;

	strcpy(Object->thinkorder, Object->Order);

	try
	{
		Object->method(skString(Object->Order), args, ret);
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
