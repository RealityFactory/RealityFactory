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
		bool flag = CCD->Pawns()->CanSee(FOV, Actor, TargetActor, FOVBone);
		if(flag)
			CCD->ActorManager()->GetPosition(TargetActor, &LastTargetPoint);
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
	else 
	{
		return skScriptedExecutable::setValue(fieldName, attribute, value);
	}
}

// calls a method in this object
bool ScriptedObject::lowmethod(const skString& methodName, skRValueArray& arguments,skRValue& returnValue)
{
	char param0[128], param4[128];
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
	else if (IS_METHOD(methodName, "ChangeYaw"))
	{
		ChangeYaw();
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
	else if (IS_METHOD(methodName, "Damage"))
	{
		PARMCHECK(2);
		float amount = arguments[0].floatValue();
		strcpy(param0, arguments[1].str());
		CCD->Damage()->DamageActor(TargetActor, amount, param0, amount, param0, "Melee");
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
		if(arguments.entries()==7)
		{
			float height = arguments[7].floatValue();
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
		if(geActor_GetBoneTransform(Actor, param0, &Xf))
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
	else if (IS_METHOD(methodName, "AddEffect"))
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
			CCD->Explosions()->AddExplosion(param0, Pos, Actor, param4);
		}
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
	targetpitch = Orient.X;
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
			RotateAmt-=GE_PI;
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
		RotateAmt -= amount;
		if(RotateLeft)
			CCD->ActorManager()->TurnRight(Actor,0.0174532925199433f*amount);
		else
			CCD->ActorManager()->TurnLeft(Actor,0.0174532925199433f*amount);
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
