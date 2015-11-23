#include "RabidFramework.h"

extern geSound_Def *SPool_Sound(char *SName);

#include "Simkin\\skScriptedExecutable.h"
#include "Simkin\\skRValue.h"
#include "Simkin\\skRValueArray.h"
#include "Simkin\\skRuntimeException.h"
#include "Simkin\\skParseException.h"

typedef enum
{
	MOVETOPOINT = 0,
		MOVEFORWARD,
		MOVEBACKWARD,
		MOVELEFT,
		MOVERIGHT,
		MOVE,
		PLAYANIMATION,
		LOOPANIMATION,
		ROTATE,
		ROTATETOPOINT,
		ROTATETOALIGN,
		ROTATEMOVETOPOINT,
		ROTATEMOVE,
		NEWORDER,
		NEXTORDER,
		NEXTPOINT,
		NEWPOINT,
		NEWPATH,
		RESTARTORDER,
		AVOIDORDER,
		RETURN,
		JUMPCMD,
		ADDTRIGGERORDER,
		DELTRIGGERORDER,
		SETEVENTSTATE,
		FACEPLAYER,
		ROTATETOPLAYER,
		TELEPORTTOPOINT,
		ANIMATIONSPEED,
		SETFLAG,
		STOPANIMATION,
		ATTRIBUTEORDER,
		REMOVE,
		SETNOCOLLISION,
		ALLOWUSE,
		CONVERSATION,
		FADEIN,
		FADEOUT,
		FIELDOFVIEW,
		SETGROUP,
		HOSTILEPLAYER,
		HOSTILEDIFF,
		HOSTILESAME,
		MOVETOTARGET,
		ROTATETOTARGET,
		ROTATEMOVETOTARGET,
		LOWLEVEL,
		BOXWIDTH,
		SCALE,
		DELAY
};


char *ActionText[] = 
{
	"MoveToPoint",
	"MoveForward",
	"MoveBackward",
	"MoveLeft",
	"MoveRight",
	"Move",
	"PlayAnimation",
	"LoopAnimation",
	"Rotate",
	"RotateToPoint",
	"RotateToAlign",
	"RotateMoveToPoint",
	"RotateMove",
	"NewOrder",
	"NextOrder",
	"NextPoint",
	"NewPoint",
	"NewPath",
	"RestartOrder",
	"AvoidOrder",
	"Return",
	"Jump",
	"AddTriggerOrder",
	"DelTriggerOrder",
	"SetEventState",
	"FacePlayer",
	"RotateToPlayer",
	"TeleportToPoint",
	"AnimationSpeed",
	"SetFlag",
	"StopAnimation",
	"AttributeOrder",
	"Remove",
	"SetNoCollision",
	"AllowUseKey",
	"Conversation",
	"FadeIn",
	"FadeOut",
	"SetFOV",
	"SetGroup",
	"HostilePlayer",
	"HostileDifferent",
	"HostileSame",
	"MoveToTarget",
	"RotateToTarget",
	"RotateMoveTotarget",
	"LowLevel",
	"BoxWidth",
	"Scale",
	"Delay"
};

// calls a method in this object
bool ScriptedObject::highmethod(const skString& methodName, skRValueArray& arguments,skRValue& returnValue)
{
	char param0[128], param7[128], param8[128];
	float param1, param3, param4, param5, param6;
	bool param2;
	
	param0[0] = '\0';
	param7[0] = '\0';
	param8[0] = '\0';
	
	if (IS_METHOD(methodName, "MoveToPoint"))
	{
		strcpy(param0, arguments[0].str());
		param1 = arguments[1].floatValue();
		strcpy(param8, arguments[2].str());
		AddAction(MOVETOPOINT, param0, param1, false, 0.0f, 0.0f, 0.0f, 0.0f, param8, NULL);
		return true;
	}
	else if (IS_METHOD(methodName, "RotateToPoint"))
	{
		strcpy(param0, arguments[0].str());
		param1 = arguments[1].floatValue();
		param2 = arguments[2].boolValue();
		strcpy(param8, arguments[3].str());
		AddAction(ROTATETOPOINT, param0, param1, param2, 0.0f, 0.0f, 0.0f, 0.0f, param8, NULL);
		return true;
	}
	else if (IS_METHOD(methodName, "NewOrder"))
	{
		strcpy(param0, arguments[0].str());
		AddAction(NEWORDER, param0, 0.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
		return true;
	}
	else if (IS_METHOD(methodName, "NextOrder"))
	{
		AddAction(NEXTORDER, NULL, 0.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
		return true;
	}
	else if (IS_METHOD(methodName, "RotateToAlign"))
	{
		strcpy(param0, arguments[0].str());
		param1 = arguments[1].floatValue();
		param2 = arguments[2].boolValue();
		strcpy(param8, arguments[3].str());
		AddAction(ROTATETOALIGN, param0, param1, param2, 0.0f, 0.0f, 0.0f, 0.0f, param8, NULL);
		return true;
	}
	else if (IS_METHOD(methodName, "NextPoint"))
	{
		AddAction(NEXTPOINT, NULL, 0.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
		return true;
	}
	else if (IS_METHOD(methodName, "Delay"))
	{
		strcpy(param0, arguments[0].str());
		param1 = arguments[1].floatValue();
		strcpy(param8, arguments[2].str());
		AddAction(DELAY, param0, param1, false, 0.0f, 0.0f, 0.0f, 0.0f, param8, NULL);
		return true;
	}
	else if (IS_METHOD(methodName, "PlayAnimation"))
	{
		strcpy(param0, arguments[0].str());
		param2 = arguments[1].boolValue();
		strcpy(param8, arguments[2].str());
		AddAction(PLAYANIMATION, param0, 0.0f, param2, 0.0f, 0.0f, 0.0f, 0.0f, param8, NULL);
		return true;
	}
	else if (IS_METHOD(methodName, "LoopAnimation"))
	{
		strcpy(param0, arguments[0].str());
		param1 = arguments[1].floatValue();
		strcpy(param8, arguments[2].str());
		AddAction(LOOPANIMATION, param0, param1, false, 0.0f, 0.0f, 0.0f, 0.0f, param8, NULL);
		return true;
	}
	else if (IS_METHOD(methodName, "Rotate"))
	{
		strcpy(param0, arguments[0].str());
		param1 = arguments[1].floatValue();
		param3 = arguments[2].floatValue();
		param4 = arguments[3].floatValue();
		param5 = arguments[4].floatValue();
		strcpy(param8, arguments[5].str());
		AddAction(ROTATE, param0, param1, false, param3, param4, param5, 0.0f, param8, NULL);
		return true;
	}
	else if (IS_METHOD(methodName, "RotateMoveToPoint"))
	{
		strcpy(param0, arguments[0].str());
		param1 = arguments[1].floatValue();
		param3 = arguments[2].floatValue();
		param2 = arguments[3].boolValue();
		strcpy(param8, arguments[4].str());
		AddAction(ROTATEMOVETOPOINT, param0, param1, param2, param3, 0.0f, 0.0f, 0.0f, param8, NULL);
		return true;
	}
	else if (IS_METHOD(methodName, "RotateMove"))
	{
		strcpy(param0, arguments[0].str());
		param1 = arguments[1].floatValue();
		param6 = arguments[2].floatValue();
		param3 = arguments[3].floatValue();
		param4 = arguments[4].floatValue();
		param5 = arguments[5].floatValue();
		strcpy(param8, arguments[6].str());
		AddAction(ROTATEMOVE, param0, param1, false, param3, param4, param5, param6, param8, NULL);
		return true;
	}
	else if (IS_METHOD(methodName, "NewPath"))
	{
		strcpy(param0, arguments[0].str());
		AddAction(NEWPATH, param0, 0.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
		return true;
	}
	else if (IS_METHOD(methodName, "RestartOrder"))
	{
		AddAction(RESTARTORDER, NULL, 0.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
		return true;
	}
	else if (IS_METHOD(methodName, "PlayerDistOrder"))
	{
		MinDistance = arguments[0].floatValue();
		if(MinDistance!=0.0f)
		{
			DistActive = true;
			strcpy(DistOrder, arguments[1].str());
		}
		else
			DistActive = false;
		return true;
	}
	else if (IS_METHOD(methodName, "Console"))
	{
		console = arguments[0].boolValue();
		if(console)
		{
			ConsoleHeader = (char *)malloc(128);
			*ConsoleHeader = '\0';
			ConsoleError = (char *)malloc(128);
			*ConsoleError = '\0';
			for(int i=0; i<5; i++)
			{
				ConsoleDebug[i] = (char *)malloc(64);
				*ConsoleDebug[i] = '\0';
			}
		}
		else
		{
			if(ConsoleHeader)
				free(ConsoleHeader);
			if(ConsoleError)
				free(ConsoleError);
			for(int i=0; i<5; i++)
			{
				if(ConsoleDebug[i])
					free(ConsoleDebug[i]);
			}
		}
		return true;
	}
	else if (IS_METHOD(methodName, "AddPainOrder"))
	{
		PainActive = true;
		strcpy(PainOrder, arguments[0].str());
		PainPercent = arguments[1].intValue();
		CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(Actor);
		OldAttributeAmount = theInv->Value(Attribute);
		return true;
	}
	else if (IS_METHOD(methodName, "FindTargetOrder"))
	{
		TargetDistance = arguments[0].floatValue();
		if(TargetDistance!=0.0f)
		{
			TargetDisable = false;
			TargetFind = true;
			strcpy(TargetOrder, arguments[1].str());
			strcpy(TargetAttr, arguments[2].str());
		}
		else
			TargetFind = false;
		return true;
	}
	else if (IS_METHOD(methodName, "NewPoint"))
	{
		strcpy(param0, arguments[0].str());
		AddAction(NEWPOINT, param0, 0.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
		return true;
	}
	else if (IS_METHOD(methodName, "MoveForward"))
	{
		strcpy(param0, arguments[0].str());
		param1 = arguments[1].floatValue();
		param3 = arguments[2].floatValue();
		strcpy(param8, arguments[3].str());
		AddAction(MOVEFORWARD, param0, param1, false, param3, 0.0f, 0.0f, 0.0f, param8, NULL);
		return true;
	}
	else if (IS_METHOD(methodName, "MoveBackward"))
	{
		strcpy(param0, arguments[0].str());
		param1 = arguments[1].floatValue();
		param3 = arguments[2].floatValue();
		strcpy(param8, arguments[3].str());
		AddAction(MOVEBACKWARD, param0, param1, false, param3, 0.0f, 0.0f, 0.0f, param8, NULL);
		return true;
	}
	else if (IS_METHOD(methodName, "MoveLeft"))
	{
		strcpy(param0, arguments[0].str());
		param1 = arguments[1].floatValue();
		param3 = arguments[2].floatValue();
		strcpy(param8, arguments[3].str());
		AddAction(MOVELEFT, param0, param1, false, param3, 0.0f, 0.0f, 0.0f, param8, NULL);
		return true;
	}
	else if (IS_METHOD(methodName, "MoveRight"))
	{
		strcpy(param0, arguments[0].str());
		param1 = arguments[1].floatValue();
		param3 = arguments[2].floatValue();
		strcpy(param8, arguments[3].str());
		AddAction(MOVERIGHT, param0, param1, false, param3, 0.0f, 0.0f, 0.0f, param8, NULL);
		return true;
	}
	else if (IS_METHOD(methodName, "Move"))
	{
		strcpy(param0, arguments[0].str());
		param1 = arguments[1].floatValue();
		param3 = arguments[2].floatValue();
		param4 = arguments[3].floatValue();
		param5 = arguments[4].floatValue();
		param6 = arguments[5].floatValue();
		strcpy(param8, arguments[6].str());
		AddAction(MOVE, param0, param1, false, param3, param4, param5, param6, param8, NULL);
		return true;
	}
	else if (IS_METHOD(methodName, "AvoidOrder"))
	{
		strcpy(param0, arguments[0].str());
		AddAction(AVOIDORDER, param0, 0.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
		return true;
	}
	else if (IS_METHOD(methodName, "Return"))
	{
		AddAction(RETURN, NULL, 0.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
		return true;
	}
	else if (IS_METHOD(methodName, "Jump"))
	{
		strcpy(param0, arguments[0].str());
		param1 = arguments[1].floatValue();
		param2 = arguments[2].boolValue();
		strcpy(param8, arguments[3].str());
		AddAction(JUMPCMD, param0, param1, param2, 0.0f, 0.0f, 0.0f, 0.0f, param8, NULL);
		return true;
	}
	else if (IS_METHOD(methodName, "AddTriggerOrder"))
	{
		strcpy(param0, arguments[0].str());
		strcpy(param7, arguments[1].str());
		param1 = arguments[2].floatValue();
		AddAction(ADDTRIGGERORDER, param0, param1, false, PTRIGGER, 0.0f, 0.0f, 0.0f, NULL, param7);
		return true;
	}
	else if (IS_METHOD(methodName, "DelTriggerOrder"))
	{
		strcpy(param0, arguments[0].str());
		AddAction(DELTRIGGERORDER, NULL, 0.0f, false, PTRIGGER, 0.0f, 0.0f, 0.0f, NULL, param0);
		return true;
	}
	else if (IS_METHOD(methodName, "SetEventState"))
	{
		strcpy(param0, arguments[0].str());
		param2 = arguments[1].boolValue();
		AddAction(SETEVENTSTATE, param0, 0.0f, param2, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
		return true;
	}
	else if (IS_METHOD(methodName, "FacePlayer"))
	{
		param2 = arguments[0].boolValue();
		bool para = arguments[1].boolValue();
		param1 = 0.0f;
		if(para)
			param1 = 1.0f;
		AddAction(FACEPLAYER, NULL, param1, param2, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
		return true;
	}
	else if (IS_METHOD(methodName, "RotateToPlayer"))
	{
		strcpy(param0, arguments[0].str());
		param1 = arguments[1].floatValue();
		param2 = arguments[2].boolValue();
		strcpy(param8, arguments[3].str());
		AddAction(ROTATETOPLAYER, param0, param1, param2, 0.0f, 0.0f, 0.0f, 0.0f, param8, NULL);
		return true;
	}
	else if (IS_METHOD(methodName, "TeleportToPoint"))
	{
		strcpy(param0, arguments[0].str());
		AddAction(TELEPORTTOPOINT, param0, 0.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
		return true;
	}
	else if (IS_METHOD(methodName, "AnimationSpeed"))
	{
		param1 = arguments[0].floatValue();
		AddAction(ANIMATIONSPEED, NULL, param1, false, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
		return true;
	}
	else if (IS_METHOD(methodName, "SetFlag"))
	{
		param1 = (float)arguments[0].intValue();
		param2 = arguments[1].boolValue();
		if(param1>=0.0f && param1<100.0f)
			AddAction(SETFLAG, NULL, param1, param2, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
		return true;
	}
	else if (IS_METHOD(methodName, "AddFlagOrder"))
	{
		strcpy(param0, arguments[2].str());
		param1 = (float)arguments[0].intValue();
		param2 = arguments[1].boolValue();
		sprintf(param7,"PawnFlag%d", (int)param1);
		if(param1>=0.0f && param1<100.0f)
			AddAction(ADDTRIGGERORDER, param0, 0.0f, param2, PFLAG, param1, 0.0f, 0.0f, NULL, param7);
		return true;
	}
	else if (IS_METHOD(methodName, "DelFlagOrder"))
	{
		param1 = (float)arguments[0].intValue();
		sprintf(param7,"PawnFlag%d", (int)param1);
		AddAction(DELTRIGGERORDER, NULL, param1, false, PFLAG, 0.0f, 0.0f, 0.0f, NULL, param7);
		return true;
	}
	else if (IS_METHOD(methodName, "AddTimerOrder"))
	{
		strcpy(param0, arguments[2].str());
		param1 = (float)arguments[0].intValue();
		param3 = (float)arguments[1].floatValue();
		sprintf(param7,"PawnTimer%d", (int)param1);
		AddAction(ADDTRIGGERORDER, param0, 0.0f, false, PTIMER, param1, param3, 0.0f, NULL, param7);
		return true;
	}
	else if (IS_METHOD(methodName, "DelTimerOrder"))
	{
		param1 = (float)arguments[0].intValue();
		sprintf(param7,"PawnTimer%d", (int)param1);
		AddAction(DELTRIGGERORDER, NULL, param1, false, PTIMER, 0.0f, 0.0f, 0.0f, NULL, param7);
		return true;
	}
	else if (IS_METHOD(methodName, "AddDistanceOrder"))
	{
		strcpy(param0, arguments[2].str());
		param1 = (float)arguments[0].intValue();
		param3 = (float)arguments[1].floatValue();
		sprintf(param7,"PlayerDist%d", (int)param1);
		AddAction(ADDTRIGGERORDER, param0, 0.0f, false, PDIST, param1, param3, 0.0f, NULL, param7);
		return true;
	}
	else if (IS_METHOD(methodName, "DelDistanceOrder"))
	{
		param1 = (float)arguments[0].intValue();
		sprintf(param7,"PlayerDist%d", (int)param1);
		AddAction(DELTRIGGERORDER, NULL, param1, false, PDIST, 0.0f, 0.0f, 0.0f, NULL, param7);
		return true;
	}
	else if (IS_METHOD(methodName, "AnimateStop"))
	{
		strcpy(param0, arguments[0].str());
		param1 = arguments[1].floatValue();
		strcpy(param8, arguments[2].str());
		AddAction(STOPANIMATION, param0, param1, false, 0.0f, 0.0f, 0.0f, 0.0f, param8, NULL);
		return true;
	}
	else if (IS_METHOD(methodName, "AttributeOrder"))
	{
		strcpy(param0, arguments[0].str());
		param1 = (float)arguments[1].intValue();
		strcpy(param8, arguments[2].str());
		AddAction(ATTRIBUTEORDER, param0, param1, false, 0.0f, 0.0f, 0.0f, 0.0f, NULL, param8);
		return true;
	}
	else if (IS_METHOD(methodName, "Remove"))
	{
		param2 = arguments[0].boolValue();
		AddAction(REMOVE, NULL, 0.0f, param2, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
		return true;
	}
	else if (IS_METHOD(methodName, "SetNoCollision"))
	{
		AddAction(SETNOCOLLISION, NULL, 0.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
		return true;
	}
	else if (IS_METHOD(methodName, "SetCollision"))
	{
		AddAction(SETNOCOLLISION, NULL, 0.0f, true, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
		return true;
	}
	else if (IS_METHOD(methodName, "AllowUseKey"))
	{
		param2 = arguments[0].boolValue();
		AddAction(ALLOWUSE, NULL, 0.0f, param2, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
		return true;
	}
	else if (IS_METHOD(methodName, "Conversation"))
	{
		AddAction(CONVERSATION, NULL, 0.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
		return true;
	}
	else if (IS_METHOD(methodName, "FadeIn"))
	{
		param1 = arguments[0].floatValue();
		param3 = arguments[1].floatValue();
		AddAction(FADEIN, NULL, param1, false, param3, 0.0f, 0.0f, 0.0f, NULL, NULL);
		return true;
	}
	else if (IS_METHOD(methodName, "FadeOut"))
	{
		param1 = arguments[0].floatValue();
		param3 = arguments[1].floatValue();
		AddAction(FADEOUT, NULL, param1, false, param3, 0.0f, 0.0f, 0.0f, NULL, NULL);
		return true;
	}
	else if (IS_METHOD(methodName, "SetFOV"))
	{
		param1 = arguments[0].floatValue();
		if (arguments.entries()==2)
			strcpy(param0, arguments[1].str());
		AddAction(FIELDOFVIEW, param0, param1, false, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
		return true;
	}
	else if (IS_METHOD(methodName, "SetGroup"))
	{
		strcpy(param0, arguments[0].str());
		AddAction(SETGROUP, param0, 0.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
		return true;
	}
	else if (IS_METHOD(methodName, "HostilePlayer"))
	{
		param2 = arguments[0].boolValue();
		AddAction(HOSTILEPLAYER, NULL, 0.0f, param2, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
		return true;
	}
	else if (IS_METHOD(methodName, "HostileDifferent"))
	{
		param2 = arguments[0].boolValue();
		AddAction(HOSTILEDIFF, NULL, 0.0f, param2, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
		return true;
	}
	else if (IS_METHOD(methodName, "HostileSame"))
	{
		param2 = arguments[0].boolValue();
		AddAction(HOSTILESAME, NULL, 0.0f, param2, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
		return true;
	}
	else if (IS_METHOD(methodName, "MoveToTarget"))
	{
		strcpy(param0, arguments[0].str());
		param1 = arguments[1].floatValue();
		strcpy(param8, arguments[2].str());
		AddAction(MOVETOTARGET, param0, param1, false, 0.0f, 0.0f, 0.0f, 0.0f, param8, NULL);
		return true;
	}
	else if (IS_METHOD(methodName, "RotateToTarget"))
	{
		strcpy(param0, arguments[0].str());
		param1 = arguments[1].floatValue();
		param2 = arguments[2].boolValue();
		strcpy(param8, arguments[3].str());
		AddAction(ROTATETOTARGET, param0, param1, param2, 0.0f, 0.0f, 0.0f, 0.0f, param8, NULL);
		return true;
	}
	else if (IS_METHOD(methodName, "RotateMoveToTarget"))
	{
		strcpy(param0, arguments[0].str());
		param1 = arguments[1].floatValue();
		param3 = arguments[2].floatValue();
		param2 = arguments[3].boolValue();
		strcpy(param8, arguments[4].str());
		AddAction(ROTATEMOVETOTARGET, param0, param1, param2, param3, 0.0f, 0.0f, 0.0f, param8, NULL);
		return true;
	}
	else if (IS_METHOD(methodName, "LowLevel"))
	{
		strcpy(param0, arguments[0].str());
		AddAction(LOWLEVEL, param0, 0.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
		return true;
	}
	else if (IS_METHOD(methodName, "BoxWidth"))
	{
		param1 = arguments[0].floatValue();
		AddAction(BOXWIDTH, NULL, param1, false, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
		return true;
	}
	else if (IS_METHOD(methodName, "Scale"))
	{
		param1 = arguments[0].floatValue();
		AddAction(SCALE, NULL, param1, false, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
		return true;
	}
	else if (IS_METHOD(methodName, "random"))
	{
		param1 = arguments[0].floatValue();
		param3 = arguments[1].floatValue();
		if(param1<=param3)
			returnValue = (int)EffectC_Frand(param1, param3);
		else
			returnValue = (int)EffectC_Frand(param3, param1);
		return true;
	}
	else
	{
		return skScriptedExecutable::method(methodName, arguments, returnValue);
	}
	
}

void ScriptedObject::Push()
{
	ActionStack	*pool;
	
	pool = GE_RAM_ALLOCATE_STRUCT(ActionStack);
	memset(pool, 0x0, sizeof(ActionStack));
	pool->next = Stack;
	Stack = pool;
	if(pool->next)
		pool->next->prev = pool;
	pool->Top = Top;
	pool->Bottom = Bottom;
	pool->Index = Index;
	memcpy(pool->Order, Order, 64);
	memcpy(pool->Point, Point, 64);
	memcpy(pool->NextOrder, NextOrder, 64);
	memcpy(pool->DistOrder, DistOrder, 64);
	pool->CurrentPoint = CurrentPoint;
	pool->ActionActive = ActionActive;
	pool->ValidPoint = ValidPoint;
	pool->Vec2Point = Vec2Point;
	pool->DistActive = DistActive;
	pool->MinDistance = MinDistance;
	Top = NULL;
	Bottom = NULL;
	Index = NULL;
}

void ScriptedObject::Pop()
{
	if(Stack)
	{
		ActionList *pool, *temp;
		pool = Bottom;
		while	(pool!= NULL)
		{
			temp = pool->next;
			geRam_Free(pool);
			pool = temp;
		}
		Bottom = Stack->Bottom;
		Top = Stack->Top;
		Index = Stack->Index;
		memcpy(Order, Stack->Order, 64);
		memcpy(Point, Stack->Point, 64);
		memcpy(NextOrder, Stack->NextOrder, 64);
		memcpy(DistOrder, Stack->DistOrder, 64);
		CurrentPoint = Stack->CurrentPoint;
		ActionActive = Stack->ActionActive;
		ValidPoint = Stack->ValidPoint;
		Vec2Point = Stack->Vec2Point;
		DistActive = Stack->DistActive;
		MinDistance = Stack->MinDistance;
		ActionStack *stemp;
		stemp = Stack->next;
		geRam_Free(Stack);
		Stack = stemp;
		if(Stack)
			Stack->prev = NULL;
	}
}

void ScriptedObject::RemoveTriggerStack(TriggerStack *tpool)
{
	if(tpool->prev)
		tpool->prev->next = tpool->next;
	else
	{
		Trigger = tpool->next;
		if(Trigger)
			tpool->next->prev = NULL;
	}
	geRam_Free(tpool);
}

void ScriptedObject::AddAction(int Action, char *AnimName, float Speed, bool Flag,
							   float Value1, float Value2, float Value3, float Value4, char *Sound, char *Trigger)
{
	ActionList	*pool;
	
	pool = GE_RAM_ALLOCATE_STRUCT(ActionList);
	memset(pool, 0x0, sizeof(ActionList));
	pool->next = Bottom;
	Bottom = pool;
	if(pool->next)
		pool->next->prev = pool;
	if(!Top)
		Top = pool;
	pool->Action = Action;
	pool->AnimName[0] = '\0';
	if(!EffectC_IsStringNull(AnimName))
		strcpy(pool->AnimName, AnimName);
	pool->SoundName[0] = '\0';
	if(!EffectC_IsStringNull(Sound))
		strcpy(pool->SoundName, Sound);
	pool->TriggerName[0] = '\0';
	if(!EffectC_IsStringNull(Trigger))
		strcpy(pool->TriggerName, Trigger);
	pool->Speed = Speed;
	pool->Flag = Flag;
	pool->Value1 = Value1;
	pool->Value2 = Value2;
	pool->Value3 = Value3;
	pool->Value4 = Value4;
}


void CPawn::AddEvent(char *Event, bool State)
{
	EventStack *pool, *temp;
	pool = Events;
	while(pool!= NULL)
	{
		temp = pool->next;
		if(!strcmp(pool->EventName, Event))
		{
			pool->State = State;
			return;
		}
		pool = temp;
	}
	pool = GE_RAM_ALLOCATE_STRUCT(EventStack);
	memset(pool, 0x0, sizeof(EventStack));
	pool->next = Events;
	Events = pool;
	if(pool->next)
		pool->next->prev = pool;
	strcpy(pool->EventName, Event);
	pool->State = State;
}

bool CPawn::GetEventState(char *Event)
{
	EventStack *pool, *temp;
	pool = Events;
	while(pool!= NULL)
	{
		temp = pool->next;
		if(!strcmp(pool->EventName, Event))
			return pool->State;
		pool = temp;
	}
	return false;
}

bool CPawn::CanSee(float FOV, geActor *Actor, geActor *TargetActor, char *Bone)
{
	geVec3d Pos, TgtPos, temp, In;
	float dotProduct;
	
	CCD->ActorManager()->GetPosition(Actor, &Pos);
	CCD->ActorManager()->InVector(Actor, &In);
	if(!EffectC_IsStringNull(Bone))
	{
		geXForm3d Xf;
		if(geActor_GetBoneTransform(Actor, Bone, &Xf)==GE_TRUE)
		{
			geVec3d_Copy(&(Xf.Translation), &Pos);
			geXForm3d_GetIn(&Xf, &In);
		}
	}
	CCD->ActorManager()->GetPosition(TargetActor, &TgtPos);
	geVec3d_Subtract(&TgtPos,&Pos,&temp);
	geVec3d_Normalize(&temp);
	dotProduct = geVec3d_DotProduct(&temp,&In);
	if(dotProduct > FOV)
	{
		if(CanSeeActorToActor(Actor, TargetActor))
			return true;
	}
	return false;
}

bool CPawn::PlayerDistance(float FOV, float distance, geActor *Actor, geVec3d DeadPos, char *Bone)
{
	bool flg = false;
	bool fov = true;
	if(FOV>=0.0f && Actor)
		fov = CanSee(FOV, Actor, CCD->Player()->GetActor(), Bone);
	
	if(fov)
	{
		if(distance>=0.0f)
		{
			if(Actor)
				flg = CCD->ActorManager()->DistanceFrom(Actor, CCD->Player()->GetActor())<distance;
			else
				flg = CCD->ActorManager()->DistanceFrom(DeadPos, CCD->Player()->GetActor())<distance;
		}
		else
		{
			if(Actor)
				flg = CCD->ActorManager()->DistanceFrom(Actor, CCD->Player()->GetActor())>fabs(distance);
			else
				flg = CCD->ActorManager()->DistanceFrom(DeadPos, CCD->Player()->GetActor())>fabs(distance);
		}
	}
	return flg;
}


bool CPawn::RotateToPoint(void *Data, float dwTicks)
{
	ScriptedObject *Object = (ScriptedObject *)Data;
	
	if(Object->StartAction)
	{
		if(!EffectC_IsStringNull(Object->Index->AnimName))
			CCD->ActorManager()->SetMotion(Object->Actor, Object->Index->AnimName);
		Object->StartAction = false;
	}
	geVec3d Pos, Orient;
	CCD->ActorManager()->GetPosition(Object->Actor, &Pos);
	geVec3d_Subtract(&Object->CurrentPoint, &Pos, &Orient);
	float l = geVec3d_Length(&Orient);
	float RotateAmt = 0.0f;
	float TiltAmt = 0.0f;
	if(l > 0.0f) 
	{
		float x = Orient.X;
		Orient.X = (float)( GE_PI*0.5 ) - 
			(float)acos(Orient.Y / l);
		Orient.Y = (float)atan2(x , Orient.Z ) + GE_PI;
		CCD->ActorManager()->GetRotate(Object->Actor, &Pos);
		while(Pos.Y<0.0f)
		{
			Pos.Y+=(GE_PI*2);
		}
		while(Pos.Y>=(GE_PI*2))
		{
			Pos.Y-=(GE_PI*2);
		}
		bool RotateLeft = false;
		RotateAmt = Pos.Y - Orient.Y;
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
			float amount = Object->Index->Speed * (dwTicks/1000.0f);
			if(amount>RotateAmt)
			{
				amount = RotateAmt;
			}
			RotateAmt -= amount;
			if(RotateLeft)
				CCD->ActorManager()->TurnRight(Object->Actor,0.0174532925199433f*amount);
			else
				CCD->ActorManager()->TurnLeft(Object->Actor,0.0174532925199433f*amount);
		}
		if(Object->Index->Flag)
		{
			while(Pos.X<-(GE_PI*2))
			{
				Pos.X+=(GE_PI*2);
			}
			while(Pos.X>=(GE_PI*2))
			{
				Pos.X-=(GE_PI*2);
			}
			bool TiltUp = false;
			TiltAmt = Pos.X - Orient.X;
			if(TiltAmt<0.0f)
			{
				TiltAmt = -TiltAmt;
				TiltUp = true;
			}
			if(TiltAmt>0.0f)
			{
				if(TiltAmt>GE_PI)
				{
					TiltAmt-=GE_PI;
					if(TiltUp)
						TiltUp=false;
					else
						TiltUp=true;
				}
				TiltAmt /= 0.0174532925199433f;
				float amount = Object->Index->Speed * (dwTicks/1000.0f);
				if(amount>TiltAmt)
				{
					amount = TiltAmt;
				}
				TiltAmt -= amount;
				if(TiltUp)
					CCD->ActorManager()->TiltUp(Object->Actor,0.0174532925199433f*amount);
				else
					CCD->ActorManager()->TiltDown(Object->Actor,0.0174532925199433f*amount);
			}
		}
		if(RotateAmt<=0.0f && TiltAmt<=0.0f)
			return true;
		return false;
	}
	return true;
}

bool CPawn::MoveToPoint(void *Data, float dwTicks)
{
	geVec3d Pos, Orient, NewPos;
	geXForm3d Xf;
	ScriptedObject *Object = (ScriptedObject *)Data;
	
	Object->AvoidMode = false;
	if(Object->StartAction)
	{
		if(!EffectC_IsStringNull(Object->Index->AnimName))
			CCD->ActorManager()->SetMotion(Object->Actor, Object->Index->AnimName);
		Object->StartAction = false;
		CCD->ActorManager()->GetPosition(Object->Actor, &Pos);
		geVec3d_Subtract(&Object->CurrentPoint, &Pos, &Object->Vec2Point);
		Orient = Object->Vec2Point;
		Object->Vec2Point.Y = 0.0f;
		geVec3d_Normalize(&Object->Vec2Point);
		float l = geVec3d_Length(&Orient);
		if(l > 0.0f) 
		{
			float x = Orient.X;
			Orient.X = (float)( GE_PI*0.5 ) - (float)acos(Orient.Y / l);
			Orient.Y = (float)atan2( x , Orient.Z ) + GE_PI;
			// roll is zero - always!!?
			Orient.Z = 0.0;
			
			geXForm3d_SetIdentity(&Xf);
			geXForm3d_RotateZ(&Xf, Orient.Z);
			geXForm3d_RotateX(&Xf, Orient.X);
			geXForm3d_RotateY(&Xf, Orient.Y);
			geVec3d_Set(&Xf.Translation, Pos.X,Pos.Y,Pos.Z);
			geXForm3d_GetIn(&Xf, &Object->TempPoint);
			//return false;
		}
		else
			return true;
	}
	
	CCD->ActorManager()->GetPosition(Object->Actor, &Pos);
	float distance = Object->Index->Speed * (dwTicks/1000.0f);
	if(fabs(Pos.X-Object->CurrentPoint.X)<distance && fabs(Pos.Z-Object->CurrentPoint.Z)<distance)
		return true;
	geVec3d_AddScaled (&Pos, &Object->TempPoint, distance, &NewPos);
	
	bool result = CCD->ActorManager()->ValidateMove(Pos, NewPos, Object->Actor, false);
	if(!result)
		Object->AvoidMode = true;
	return false;
}

bool CPawn::RotateToAlign(void *Data, float dwTicks)
{
	geVec3d Pos, Orient;
	ScriptedObject *Object = (ScriptedObject *)Data;
	
	if(Object->StartAction)
	{
		if(!EffectC_IsStringNull(Object->Index->AnimName))
			CCD->ActorManager()->SetMotion(Object->Actor, Object->Index->AnimName);
		Object->StartAction = false;
		char *EntityType = CCD->EntityRegistry()->GetEntityType(Object->Point);
		if(EntityType)
		{
			if(!stricmp(EntityType, "ScriptPoint"))
			{
				ScriptPoint *pProxy;
				CCD->ScriptPoints()->LocateEntity(Object->Point, (void**)&pProxy);
				Orient.Z = 0.0174532925199433f*(pProxy->Angle.Z);
				Orient.X = 0.0174532925199433f*(pProxy->Angle.X);
				Orient.Y = 0.0174532925199433f*(pProxy->Angle.Y-90.0f);
				CCD->ActorManager()->GetPosition(Object->Actor, &Pos);
				geXForm3d Xf;
				geXForm3d_SetIdentity(&Xf);
				geXForm3d_RotateZ(&Xf, Orient.Z);
				geXForm3d_RotateX(&Xf, Orient.X);
				geXForm3d_RotateY(&Xf, Orient.Y);
				geXForm3d_Translate(&Xf, Pos.X, Pos.Y, Pos.Z);
				geXForm3d_GetIn(&Xf, &Orient);
				geVec3d_AddScaled (&Pos, &Orient, 1.0f, &Object->TempPoint);
			}
		}
		//return false;
	}
	
	CCD->ActorManager()->GetPosition(Object->Actor, &Pos);
	geVec3d_Subtract(&Object->TempPoint, &Pos, &Orient);
	float l = geVec3d_Length(&Orient);
	float RotateAmt = 0.0f;
	float TiltAmt = 0.0f;
	if(l > 0.0f) 
	{
		float x = Orient.X;
		Orient.X = (float)( GE_PI*0.5 ) - 
			(float)acos(Orient.Y / l);
		Orient.Y = (float)atan2(x , Orient.Z ) + GE_PI;
		CCD->ActorManager()->GetRotate(Object->Actor, &Pos);
		while(Pos.Y<0.0f)
		{
			Pos.Y+=(GE_PI*2);
		}
		while(Pos.Y>=(GE_PI*2))
		{
			Pos.Y-=(GE_PI*2);
		}
		bool RotateLeft = false;
		RotateAmt = Pos.Y - Orient.Y;
		if(RotateAmt<0.0f)
		{
			RotateAmt = -RotateAmt;
			RotateLeft = true;
		}
		if(RotateAmt>-(GE_PI*2))
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
			float amount = Object->Index->Speed * (dwTicks/1000.0f);
			if(amount>RotateAmt)
			{
				amount = RotateAmt;
			}
			RotateAmt -= amount;
			if(RotateLeft)
				CCD->ActorManager()->TurnRight(Object->Actor,0.0174532925199433f*amount);
			else
				CCD->ActorManager()->TurnLeft(Object->Actor,0.0174532925199433f*amount);
		}
		if(Object->Index->Flag)
		{
			while(Pos.X<0.0f)
			{
				Pos.X+=(GE_PI*2);
			}
			while(Pos.X>=(GE_PI*2))
			{
				Pos.X-=(GE_PI*2);
			}
			bool TiltUp = false;
			TiltAmt = Pos.X - Orient.X;
			if(TiltAmt<0.0f)
			{
				TiltAmt = -TiltAmt;
				TiltUp = true;
			}
			if(TiltAmt>0.0f)
			{
				if(TiltAmt>GE_PI)
				{
					TiltAmt-=GE_PI;
					if(TiltUp)
						TiltUp=false;
					else
						TiltUp=true;
				}
				TiltAmt /= 0.0174532925199433f;
				float amount = Object->Index->Speed * (dwTicks/1000.0f);
				if(amount>TiltAmt)
				{
					amount = TiltAmt;
				}
				TiltAmt -= amount;
				if(TiltUp)
					CCD->ActorManager()->TiltUp(Object->Actor,0.0174532925199433f*amount);
				else
					CCD->ActorManager()->TiltDown(Object->Actor,0.0174532925199433f*amount);
			}
		}
		if(RotateAmt<=0.0f && TiltAmt<=0.0f)
			return true;
		return false;
	}
				
	return true;
}

bool CPawn::NextPoint(void *Data, float dwTicks)
{
	ScriptedObject *Object = (ScriptedObject *)Data;
	
	char *EntityType = CCD->EntityRegistry()->GetEntityType(Object->Point);
	if(EntityType)
	{
		if(!stricmp(EntityType, "ScriptPoint"))
		{
			ScriptPoint *pProxy;
			CCD->ScriptPoints()->LocateEntity(Object->Point, (void**)&pProxy);
			Object->ValidPoint = false;
			if(!EffectC_IsStringNull(pProxy->NextPoint))
			{
				strcpy(Object->Point, pProxy->NextPoint);
				Object->NextOrder[0] = '\0';
				if(!EffectC_IsStringNull(pProxy->NextOrder))
					strcpy(Object->NextOrder, pProxy->NextOrder);
				if(!EffectC_IsStringNull(Object->Point))
				{
					char *EntityType = CCD->EntityRegistry()->GetEntityType(Object->Point);
					if(EntityType)
					{
						if(!stricmp(EntityType, "ScriptPoint"))
						{
							ScriptPoint *pProxy;
							CCD->ScriptPoints()->LocateEntity(Object->Point, (void**)&pProxy);
							Object->CurrentPoint = pProxy->origin;
							Object->ValidPoint = true;
						}
					}
				}
			}
		}
	}
	return true;
}

bool CPawn::NextOrder(void *Data, float dwTicks)
{
	ScriptedObject *Object = (ScriptedObject *)Data;
	
	if(!EffectC_IsStringNull(Object->NextOrder))
	{
		strcpy(Object->Order, Object->NextOrder);
		Object->RunOrder = true;
		ActionList *pool, *temp;
		pool = Object->Bottom;
		while	(pool!= NULL)
		{
			temp = pool->next;
			geRam_Free(pool);
			pool = temp;
		}
		Object->Top = NULL;
		Object->Bottom = NULL;
		Object->Index = NULL;
	}
	
	return true;
}


bool CPawn::Rotate(void *Data, float dwTicks)
{
	ScriptedObject *Object = (ScriptedObject *)Data;
	bool flag = true;
	
	if(Object->StartAction)
	{
		if(!EffectC_IsStringNull(Object->Index->AnimName))
			CCD->ActorManager()->SetMotion(Object->Actor, Object->Index->AnimName);
		Object->StartAction = false;
	}
	CCD->ActorManager()->GetRotate(Object->Actor, &Object->TempPoint);
	float baseamount = Object->Index->Speed * (dwTicks/1000.0f);
	if(Object->Index->Value1>0.0f)
	{
		float amount = baseamount;
		if(Object->Index->Value1!=0.0f)
		{
			amount = -amount;
			if(amount<Object->Index->Value1)
				amount = Object->Index->Value1;
		}
		else
		{
			if(amount>Object->Index->Value1)
				amount = Object->Index->Value1;
		}
		Object->Index->Value1 -= amount;
		amount *= 0.0174532925199433f;
		Object->TempPoint.X += amount;
		flag = false;
	}
	if(Object->Index->Value2!=0.0f)
	{
		float amount = baseamount;
		if(Object->Index->Value2<0.0f)
		{
			amount = -amount;
			if(amount<Object->Index->Value2)
				amount = Object->Index->Value2;
		}
		else
		{
			if(amount>Object->Index->Value2)
				amount = Object->Index->Value2;
		}
		Object->Index->Value2 -= amount;
		amount *= 0.0174532925199433f;
		Object->TempPoint.Y += amount;
		flag = false;
	}
	if(Object->Index->Value3!=0.0f)
	{
		float amount = baseamount;
		if(Object->Index->Value3<0.0f)
		{
			amount = -amount;
			if(amount<Object->Index->Value3)
				amount = Object->Index->Value3;
		}
		else
		{
			if(amount>Object->Index->Value3)
				amount = Object->Index->Value3;
		}
		Object->Index->Value3 -= amount;
		amount *= 0.0174532925199433f;
		Object->TempPoint.Z += amount;
		flag = false;
	}
	CCD->ActorManager()->Rotate(Object->Actor, Object->TempPoint);
	
	return flag;
}

bool CPawn::RotateMoveToPoint(void *Data, float dwTicks)
{
	ScriptedObject *Object = (ScriptedObject *)Data;
	geVec3d Pos, NewPos, In;
	
	if(RotateToPoint(Data, dwTicks))
		return true;
	CCD->ActorManager()->GetPosition(Object->Actor, &Pos);
	float distance = Object->Index->Value1 * (dwTicks/1000.0f);
	CCD->ActorManager()->InVector(Object->Actor, &In);
	geVec3d_AddScaled (&Pos, &In, distance, &NewPos);
	
	bool result = CCD->ActorManager()->ValidateMove(Pos, NewPos, Object->Actor, false);
	return false;
}

bool CPawn::NextPath(void *Data, float dwTicks)
{
	ScriptedObject *Object = (ScriptedObject *)Data;
	
	char *EntityType = CCD->EntityRegistry()->GetEntityType(Object->Index->AnimName);
	if(EntityType)
	{
		if(!stricmp(EntityType, "ScriptPoint"))
		{
			ScriptPoint *pProxy;
			CCD->ScriptPoints()->LocateEntity(Object->Index->AnimName, (void**)&pProxy);
			Object->Order[0] = '\0';
			strcpy(Object->Point, Object->Index->AnimName);
			if(!EffectC_IsStringNull(pProxy->NextOrder))
			{
				strcpy(Object->Order, pProxy->NextOrder);
				Object->RunOrder = true;
				ActionList *pool, *temp;
				pool = Object->Bottom;
				while	(pool!= NULL)
				{
					temp = pool->next;
					geRam_Free(pool);
					pool = temp;
				}
				Object->Top = NULL;
				Object->Bottom = NULL;
				Object->Index = NULL;
			}
			
			Object->CurrentPoint = pProxy->origin;
			Object->ValidPoint = true;
		}
	}
	return true;
}


bool CPawn::RotateMove(void *Data, float dwTicks)
{
	ScriptedObject *Object = (ScriptedObject *)Data;
	geVec3d Pos, NewPos, In;
	
	if(Rotate(Data, dwTicks))
		return true;
	CCD->ActorManager()->GetPosition(Object->Actor, &Pos);
	float distance = Object->Index->Value4 * (dwTicks/1000.0f);
	CCD->ActorManager()->InVector(Object->Actor, &In);
	geVec3d_AddScaled (&Pos, &In, distance, &NewPos);
	
	bool result = CCD->ActorManager()->ValidateMove(Pos, NewPos, Object->Actor, false);
	return false;
}

bool CPawn::Move(void *Data, float dwTicks)
{
	ScriptedObject *Object = (ScriptedObject *)Data;
	geVec3d Pos, NewPos;
	bool retflg = false;
	
	if(!Object->Actor)
		return false;
	
	if(Object->StartAction)
	{
		Object->TotalDist = 0.0f;
		if(!EffectC_IsStringNull(Object->Index->AnimName))
			CCD->ActorManager()->SetMotion(Object->Actor, Object->Index->AnimName);
		Object->StartAction = false;
		CCD->ActorManager()->GetPosition(Object->Actor, &Pos);
		if(Object->Direction==RGF_K_BACKWARD || Object->Direction==RGF_K_FORWARD)
			CCD->ActorManager()->InVector(Object->Actor, &Object->Vec2Point);
		else
		{
			if(Object->Direction!=0)
				CCD->ActorManager()->LeftVector(Object->Actor, &Object->Vec2Point);
			else
			{
				geXForm3d thePosition;
				geVec3d Rotate;
				CCD->ActorManager()->GetRotate(Object->Actor, &Rotate);
				geXForm3d_SetIdentity(&thePosition);
				geXForm3d_RotateZ(&thePosition, Rotate.Z+Object->Index->Value4*0.0174532925199433f);
				geXForm3d_RotateX(&thePosition, Rotate.X+Object->Index->Value2*0.0174532925199433f);
				geXForm3d_RotateY(&thePosition, Rotate.Y+Object->Index->Value3*0.0174532925199433f);
				geXForm3d_Translate(&thePosition, Pos.X, Pos.Y, Pos.Z);
				geXForm3d_GetIn(&thePosition, &Object->Vec2Point);
			}
		}
	}
	
	CCD->ActorManager()->GetPosition(Object->Actor, &Pos);
	float distance = Object->Index->Speed * (dwTicks/1000.0f);
	if((distance+Object->TotalDist)>=Object->Index->Value1)
	{
		distance = Object->Index->Value1 - Object->TotalDist;
		retflg = true;
	}
	Object->TotalDist += distance;
	
	if(Object->Direction==RGF_K_BACKWARD || Object->Direction==RGF_K_RIGHT)
		distance = -distance;
	
	geVec3d_AddScaled (&Pos, &Object->Vec2Point, distance, &NewPos);
	
	bool result = CCD->ActorManager()->ValidateMove(Pos, NewPos, Object->Actor, false);
	
	if(!result)
		return true;
	return retflg;
}

bool CPawn::Jump(void *Data, float dwTicks)
{
	ScriptedObject *Object = (ScriptedObject *)Data;
	
	if(Object->StartAction)
	{
		if(!EffectC_IsStringNull(Object->Index->AnimName))
			CCD->ActorManager()->SetMotion(Object->Actor, Object->Index->AnimName);
		Object->StartAction = false;
		geVec3d theUp;
		CCD->ActorManager()->UpVector(Object->Actor, &theUp);
		CCD->ActorManager()->SetForce(Object->Actor, 0, theUp, Object->Index->Speed, Object->Index->Speed);
		if(Object->Index->Flag)
			return true;
		return false;
	}
	if(CCD->ActorManager()->Falling(Object->Actor) == GE_TRUE)
		return false;
	return true;
}

bool CPawn::AddTriggerOrder(void *Data, float dwTicks)
{
	ScriptedObject *Object = (ScriptedObject *)Data;
	if(!EffectC_IsStringNull(Object->Index->AnimName))
	{
		if(!EffectC_IsStringNull(Object->Index->TriggerName))
		{
			TriggerStack *tpool, *ttemp;
			tpool = Object->Trigger;
			while	(tpool!= NULL)
			{
				ttemp = tpool->next;
				if(!strcmp(Object->Index->TriggerName, tpool->TriggerName))
				{
					strcpy(tpool->OrderName, Object->Index->AnimName);
					tpool->Delay = Object->Index->Speed;
					tpool->Flag = Object->Index->Flag;
					tpool->Type = (int)Object->Index->Value1;
					tpool->PFlg = (int)Object->Index->Value2;
					tpool->Time = Object->Index->Value3;
					return true;
				}
				tpool = ttemp;
			}
			tpool = GE_RAM_ALLOCATE_STRUCT(TriggerStack);
			memset(tpool, 0x0, sizeof(TriggerStack));
			tpool->next = Object->Trigger;
			Object->Trigger = tpool;
			if(tpool->next)
				tpool->next->prev = tpool;
			strcpy(tpool->OrderName, Object->Index->AnimName);
			strcpy(tpool->TriggerName, Object->Index->TriggerName);
			tpool->Delay = Object->Index->Speed;
			tpool->Flag = Object->Index->Flag;
			tpool->Type = (int)Object->Index->Value1;
			tpool->PFlg = (int)Object->Index->Value2;
			tpool->Time = Object->Index->Value3;
		}
	}
	
	return true;
}

bool CPawn::DelTriggerOrder(void *Data, float dwTicks)
{
	ScriptedObject *Object = (ScriptedObject *)Data;
	
	if(!EffectC_IsStringNull(Object->Index->TriggerName))
	{
		TriggerStack *tpool, *ttemp;
		tpool = Object->Trigger;
		while	(tpool!= NULL)
		{
			ttemp = tpool->next;
			if(!strcmp(Object->Index->TriggerName, tpool->TriggerName))
			{
				Object->RemoveTriggerStack(tpool);
				return true;
			}
			tpool = ttemp;
		}
	}
	
	return true;
}

bool CPawn::RotateToPlayer(void *Data, float dwTicks)
{
	ScriptedObject *Object = (ScriptedObject *)Data;
	
	if(Object->StartAction)
	{
		if(!EffectC_IsStringNull(Object->Index->AnimName))
			CCD->ActorManager()->SetMotion(Object->Actor, Object->Index->AnimName);
		Object->StartAction = false;
	}
	geVec3d Pos, Orient, Play;
	Play = CCD->Player()->Position();
	CCD->ActorManager()->GetPosition(Object->Actor, &Pos);
	geVec3d_Subtract(&Play, &Pos, &Orient);
	float l = geVec3d_Length(&Orient);
	float RotateAmt = 0.0f;
	float TiltAmt = 0.0f;
	if(l > 0.0f) 
	{
		float x = Orient.X;
		Orient.X = (float)( GE_PI*0.5 ) - 
			(float)acos(Orient.Y / l);
		Orient.Y = (float)atan2(x , Orient.Z ) + GE_PI;
		CCD->ActorManager()->GetRotate(Object->Actor, &Pos);
		while(Pos.Y<0.0f)
		{
			Pos.Y+=(GE_PI*2);
		}
		while(Pos.Y>=(GE_PI*2))
		{
			Pos.Y-=(GE_PI*2);
		} 
		bool RotateLeft = false;
		RotateAmt = Pos.Y - Orient.Y;
		
		if(RotateAmt<0.0f)
		{
			RotateAmt = -RotateAmt;
			RotateLeft = true;
		}
		if(RotateAmt!=0.0f)
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
			
			float amount = Object->Index->Speed * (dwTicks/1000.0f);
			if(amount>RotateAmt)
			{
				amount = RotateAmt;
			}
			RotateAmt -= amount;
			if(RotateLeft)
				CCD->ActorManager()->TurnRight(Object->Actor,0.0174532925199433f*amount);
			else
				CCD->ActorManager()->TurnLeft(Object->Actor,0.0174532925199433f*amount);
		}
		if(Object->Index->Flag)
		{
			while(Pos.X<-(GE_PI*2))
			{
				Pos.X+=(GE_PI*2);
			}
			while(Pos.X>=(GE_PI*2))
			{
				Pos.X-=(GE_PI*2);
			}
			bool TiltUp = false;
			TiltAmt = Pos.X - Orient.X;
			if(TiltAmt<0.0f)
			{
				TiltAmt = -TiltAmt;
				TiltUp = true;
			}
			if(TiltAmt>0.0f)
			{
				if(TiltAmt>GE_PI)
				{
					TiltAmt-=GE_PI;
					if(TiltUp)
						TiltUp=false;
					else
						TiltUp=true;
				}
				TiltAmt /= 0.0174532925199433f;
				float amount = Object->Index->Speed * (dwTicks/1000.0f);
				if(amount>TiltAmt)
				{
					amount = TiltAmt;
				}
				TiltAmt -= amount;
				if(TiltUp)
					CCD->ActorManager()->TiltUp(Object->Actor,0.0174532925199433f*amount);
				else
					CCD->ActorManager()->TiltDown(Object->Actor,0.0174532925199433f*amount);
			}
		}
		if(RotateAmt==0.0f && TiltAmt==0.0f)
			return true;
		return false;
	}
	return true;
}

void CPawn::TickHigh(Pawn *pSource, ScriptedObject *Object, float dwTicks)
{
	skRValueArray args(1);
	skRValue ret;
	int yoffset = 0;
	TriggerStack *tpool, *ttemp;
	tpool = Object->Trigger;
	while	(tpool!= NULL)
	{
		ttemp = tpool->next;
		if(tpool->Type==PTIMER)
		{
			tpool->Time -= (dwTicks/1000.0f);
			if(tpool->Time<0.0f)
				tpool->Time = 0.0f;
		}
		tpool = ttemp;
	}
				
	if(!EffectC_IsStringNull(Object->DeadOrder))
	{
		if(!EffectC_IsStringNull(Object->Attribute))
		{
			CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(Object->Actor);
			if(theInv->Value(Object->Attribute)<=0)
			{
				strcpy(Object->Order, Object->DeadOrder);
				Object->DeadOrder[0] = '\0';
				Object->RunOrder = true;
				Object->ActionActive = false;
				ActionList *pool, *temp;
				pool = Object->Bottom;
				while	(pool!= NULL)
				{
					temp = pool->next;
					geRam_Free(pool);
					pool = temp;
				}
				Object->Top = NULL;
				Object->Bottom = NULL;
				Object->Index = NULL;
			}
		}
	}
	if(Object->active)
	{
		bool runflag = false;
		do
		{
			runflag = false;
			if(Object->TriggerWait)
			{
				Object->TriggerTime -= (dwTicks/1000.0f);
				if(Object->TriggerTime<=0.0f && !Object->TargetDisable)
				{
					Object->TriggerWait = false;
					strcpy(Object->Order, Object->TriggerOrder);
					Object->RunOrder = true;
					Object->ActionActive = false;
					ActionList *pool, *temp;
					pool = Object->Bottom;
					while	(pool!= NULL)
					{
						temp = pool->next;
						geRam_Free(pool);
						pool = temp;
					}
					Object->Top = NULL;
					Object->Bottom = NULL;
					Object->Index = NULL;
				}
			}
			else
			{
				if(Object->Trigger && !Object->TargetDisable)
				{
					TriggerStack *tpool, *ttemp;
					
					bool flag;
					tpool = Object->Trigger;
					while	(tpool!= NULL)
					{
						ttemp = tpool->next;
						switch(tpool->Type)
						{
						case PTRIGGER:
							flag = GetTriggerState(tpool->TriggerName);
							break;
						case PFLAG:
							flag = (GetPawnFlag(tpool->PFlg)==tpool->Flag);
							break;
						case PTIMER:
							flag = (tpool->Time==0.0f);
							break;
						case PDIST:
							flag = PlayerDistance(Object->FOV, tpool->Time, Object->Actor, Object->DeadPos, Object->FOVBone);
							break;
						}
						if(flag)
						{
							if(tpool->Delay>0.0f)
							{
								Object->TriggerWait = true;
								Object->TriggerTime = tpool->Delay;
								strcpy(Object->TriggerOrder, tpool->OrderName);
								Object->RemoveTriggerStack(tpool);
							}
							else
							{
								strcpy(Object->Order, tpool->OrderName);
								Object->RunOrder = true;
								Object->ActionActive = false;
								ActionList *pool, *temp;
								pool = Object->Bottom;
								while	(pool!= NULL)
								{
									temp = pool->next;
									geRam_Free(pool);
									pool = temp;
								}
								Object->Top = NULL;
								Object->Bottom = NULL;
								Object->Index = NULL;
								Object->RemoveTriggerStack(tpool);
							}
							break;
						}
						tpool = ttemp;
					}
				}
			}
			if(Object->TargetFind && !Object->TargetDisable)
			{
				Object->TargetActor = NULL;
				if(Object->FOV>=0.0f && Object->Actor)
				{
					float distance = Object->TargetDistance+100;
					bool done = false;
					if(Object->HostilePlayer)
					{
						if(CanSee(Object->FOV, Object->Actor, CCD->Player()->GetActor(), Object->FOVBone))
						{
							float dist = CCD->ActorManager()->DistanceFrom(Object->Actor, CCD->Player()->GetActor());
							if(dist<Object->TargetDistance)
							{
								Object->TargetActor = CCD->Player()->GetActor();
								done = true;
								distance = dist;
							}
						}
					}
					
					geActor *ActorsInRange[512];
					geVec3d Pos;
					
					CCD->ActorManager()->GetPosition(Object->Actor, &Pos);
					int nActorCount = CCD->ActorManager()->GetActorsInRange(Pos, 
						Object->TargetDistance, 512, &ActorsInRange[0]);
					
					if(nActorCount != 0)
					{
						if(Object->HostileDiff)
						{
							for(int nTemp = 0; nTemp < nActorCount; nTemp++)
							{
								if(ActorsInRange[nTemp]==Object->Actor)
									continue;
								int nType;
								CCD->ActorManager()->GetType(ActorsInRange[nTemp], &nType);
								if(nType==ENTITY_NPC && CCD->ActorManager()->GetGroup(ActorsInRange[nTemp]))
								{
									if(strcmp(Object->Group, CCD->ActorManager()->GetGroup(ActorsInRange[nTemp]))!=0)
									{
										CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(ActorsInRange[nTemp]);
										if(theInv->Has(Object->TargetAttr))
										{
											if(theInv->Value(Object->TargetAttr)>0)
											{
												if(CanSee(Object->FOV, Object->Actor, ActorsInRange[nTemp], Object->FOVBone))
												{
													float dist = CCD->ActorManager()->DistanceFrom(Object->Actor, ActorsInRange[nTemp]);
													if(dist<Object->TargetDistance)
													{
														done = true;
														if(distance>dist)
														{
															distance = dist;
															Object->TargetActor = ActorsInRange[nTemp];
														}
													}
												}
											}
										}
									}
								}
							}
						}
						if(Object->HostileSame)
						{
							for(int nTemp = 0; nTemp < nActorCount; nTemp++)
							{
								if(ActorsInRange[nTemp]==Object->Actor)
									continue;
								int nType;
								CCD->ActorManager()->GetType(ActorsInRange[nTemp], &nType);
								if(nType==ENTITY_NPC && CCD->ActorManager()->GetGroup(ActorsInRange[nTemp]))
								{
									if(!strcmp(Object->Group, CCD->ActorManager()->GetGroup(ActorsInRange[nTemp])))
									{
										CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(ActorsInRange[nTemp]);
										if(theInv->Has(Object->TargetAttr))
										{
											if(theInv->Value(Object->TargetAttr)>0)
											{
												if(CanSee(Object->FOV, Object->Actor, ActorsInRange[nTemp], Object->FOVBone))
												{
													float dist = CCD->ActorManager()->DistanceFrom(Object->Actor, ActorsInRange[nTemp]);
													if(dist<Object->TargetDistance)
													{
														done = true;
														if(distance>dist)
														{
															distance = dist;
															Object->TargetActor = ActorsInRange[nTemp];
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
					if(done)
					{
						strcpy(Object->Order, Object->TargetOrder);
						Object->RunOrder = true;
						Object->ActionActive = false;
						ActionList *pool, *temp;
						pool = Object->Bottom;
						while	(pool!= NULL)
						{
							temp = pool->next;
							geRam_Free(pool);
							pool = temp;
						}
						Object->Top = NULL;
						Object->Bottom = NULL;
						Object->Index = NULL;
						Object->TargetFind = false;
						CCD->ActorManager()->GetPosition(Object->TargetActor, &Object->LastTargetPoint);
					}
				}
			}
			if(Object->DistActive && !Object->TargetDisable)
			{
				bool flg = PlayerDistance(Object->FOV, Object->MinDistance, Object->Actor, Object->DeadPos, Object->FOVBone);
				if(flg)
				{
					strcpy(Object->Order, Object->DistOrder);
					Object->RunOrder = true;
					Object->ActionActive = false;
					ActionList *pool, *temp;
					pool = Object->Bottom;
					while	(pool!= NULL)
					{
						temp = pool->next;
						geRam_Free(pool);
						pool = temp;
					}
					Object->Top = NULL;
					Object->Bottom = NULL;
					Object->Index = NULL;
					Object->DistActive = false;
				}
			}
			if(Object->PainActive && !Object->TargetDisable)
			{
				CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(Object->Actor);
				int current = theInv->Value(Object->Attribute);
				if(current>0)
				{
					if(current<Object->OldAttributeAmount)
					{
						if((int)EffectC_Frand(1.0f, 10.0f) <= (Object->PainPercent/10))
						{
							if(!EffectC_IsStringNull(Object->PainOrder))
							{
								Object->Push();
								strcpy(Object->Order, Object->PainOrder);
								Object->RunOrder = true;
								Object->ActionActive = false;
								Object->TargetDisable = true;
								runflag = true;
							}
						}
					}
				}
				Object->OldAttributeAmount = current;
			}
			
			if(Object->RunOrder)
			{
				bool methoderror = false;
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
					sprintf(szBug, "Script Error for %s in Order %s", Object->szName, Object->Order);
					CCD->ReportError(szBug, false);
					if(Object->console)
						strcpy(Object->ConsoleError, szBug);
					methoderror = true;
				}
				Object->RunOrder = false;
				Object->Index = NULL;
				if(!methoderror)
					Object->ActionActive = false;
				else
					break;
			}
			
			if(!Object->ActionActive)
			{
				if(!Object->Index)
					Object->Index = Object->Top;
				else
					Object->Index = Object->Index->prev;
				Object->ActionActive = true;
				Object->StartAction = true;
				if(Object->SoundIndex!=-1)
				{
					CCD->EffectManager()->Item_Delete(EFF_SND, Object->SoundIndex);
					Object->SoundIndex = -1;
				}
				if(Object->Index)
				{
					if(!EffectC_IsStringNull(Object->Index->SoundName))
					{
						Snd Sound;
						
						memset( &Sound, 0, sizeof( Sound ) );
						if(!CCD->Player()->GetMonitorMode())
						{
							if(Object->Actor)
								CCD->ActorManager()->GetPosition(Object->Actor, &Sound.Pos);
							else
								Sound.Pos = Object->DeadPos;
						}
						else
						{
							FixedCamera *pSource = CCD->FixedCameras()->GetCamera();
							Sound.Pos.X = pSource->origin.X;
							Sound.Pos.Y = pSource->origin.Y;
							Sound.Pos.Z = pSource->origin.Z;
						}
						Sound.Min=kAudibleRadius;
						Sound.Loop=true;
						if(Object->Index->Action==STOPANIMATION)
							Sound.Loop=false;
						Sound.SoundDef = SPool_Sound(Object->Index->SoundName);
						Object->SoundIndex = CCD->EffectManager()->Item_Add(EFF_SND, (void *)&Sound);
					}
				}
			}
			else
			{
				if(!Object->Index)
				{
					if(Object->Stack)
					{
						Object->Pop();
						Object->ActionActive = true;
						Object->TargetDisable = false;
						Object->StartAction = true;
						if(Object->SoundIndex!=-1)
						{
							CCD->EffectManager()->Item_Delete(EFF_SND, Object->SoundIndex);
							Object->SoundIndex = -1;
						}
						if(!EffectC_IsStringNull(Object->Index->SoundName))
						{
							Snd Sound;
							
							memset( &Sound, 0, sizeof( Sound ) );
							if(Object->Actor)
								CCD->ActorManager()->GetPosition(Object->Actor, &Sound.Pos);
							else
								Sound.Pos = Object->DeadPos;
							Sound.Min=kAudibleRadius;
							Sound.Loop=true;
							if(Object->Index->Action==STOPANIMATION)
								Sound.Loop=false;
							Sound.SoundDef = SPool_Sound(Object->Index->SoundName);
							Object->SoundIndex = CCD->EffectManager()->Item_Add(EFF_SND, (void *)&Sound);
						}
					}
				}
			}
			
			if(Object->Index)
			{
				if(Object->console)
				{
					char szBug[256];
					sprintf(szBug, "%s %s %s %s",Object->szName, Object->Order, ActionText[Object->Index->Action], Object->Point);
					strcpy(Object->ConsoleHeader, szBug);
				}
				if(Object->FacePlayer && Object->Actor)
				{
					geVec3d Pos = CCD->Player()->Position();
					geVec3d Pos1;
					CCD->ActorManager()->GetPosition(Object->Actor, &Pos1);
					geVec3d LookRotation;
					geFloat x,l;
					geVec3d_Subtract(&Pos, &Pos1, &LookRotation);
					l = geVec3d_Length(&LookRotation);
					// protect from Div by Zero
					if(l > 0.0f) 
					{
						x = LookRotation.X;
						LookRotation.X = -((float)( GE_PI*0.5f) - (float)acos(LookRotation.Y / l));
						LookRotation.Y = (float)atan2( x , LookRotation.Z ) + GE_PI;
						// roll is zero - always!!?
						LookRotation.Z = 0.0;									
						if(!Object->FaceAxis)
							LookRotation.X = 0.0f;
						CCD->ActorManager()->Rotate(Object->Actor, LookRotation);
					}
				} 
				
				ActionList *pool, *temp;
				switch(Object->Index->Action)
				{
				case ROTATETOPOINT:
					if(Object->ValidPoint && !Object->FacePlayer && Object->Actor)
					{
						if(RotateToPoint((void *)Object, dwTicks))
							Object->ActionActive = false;
					}
					else
						Object->ActionActive = false;
					break;
				case MOVETOPOINT:
					if(Object->ValidPoint && Object->Actor)
					{
						if(MoveToPoint((void *)Object, dwTicks))
						{
							Object->ActionActive = false;
						}
						else
						{
							if(Object->AvoidMode && !EffectC_IsStringNull(Object->AvoidOrder))
							{
								Object->Push();
								strcpy(Object->Order, Object->AvoidOrder);
								Object->RunOrder = true;
								Object->ActionActive = false;
								Object->DistActive = false;
								runflag = true;
							}
						}
					}
					else
						Object->ActionActive = false;
					break;
				case NEWORDER:
					strcpy(Object->Order, Object->Index->AnimName);
					Object->RunOrder = true;
					Object->ActionActive = false;
					pool = Object->Bottom;
					while	(pool!= NULL)
					{
						temp = pool->next;
						geRam_Free(pool);
						pool = temp;
					}
					Object->Top = NULL;
					Object->Bottom = NULL;
					Object->Index = NULL;
					runflag = true;
					break;
				case NEXTORDER:
					if(Object->ValidPoint)
					{
						if(NextOrder((void *)Object, dwTicks))
							Object->ActionActive = false;
					}
					else
						Object->ActionActive = false;
					runflag = true;
					break;
				case ROTATETOALIGN:
					if(Object->ValidPoint && !Object->FacePlayer && Object->Actor)
					{
						if(RotateToAlign((void *)Object, dwTicks))
							Object->ActionActive = false;
					}
					else
						Object->ActionActive = false;
					break;
				case ROTATE:
					if(!Object->FacePlayer && Object->Actor)
					{
						if(Rotate((void *)Object, dwTicks))
							Object->ActionActive = false;
					}
					else
						Object->ActionActive = false;
					break;
				case NEXTPOINT:
					if(Object->ValidPoint)
					{
						if(NextPoint((void *)Object, dwTicks))
							Object->ActionActive = false;
					}
					else
						Object->ActionActive = false;
					runflag = true;
					break;
				case DELAY:
					if(Object->StartAction)
					{
						if(!EffectC_IsStringNull(Object->Index->AnimName) && Object->Actor)
							CCD->ActorManager()->SetMotion(Object->Actor, Object->Index->AnimName);
						Object->StartAction = false;
					}
					Object->Time += (dwTicks/1000.0f);
					if(Object->Time>=Object->Index->Speed)
					{
						Object->Time = 0.0f;
						Object->ActionActive = false;
					}
					break;
				case PLAYANIMATION:
					if(Object->Actor)
					{
						if(Object->StartAction)
						{
							if(!EffectC_IsStringNull(Object->Index->AnimName))
								CCD->ActorManager()->SetMotion(Object->Actor, Object->Index->AnimName);
							Object->StartAction = false;
							if(Object->Index->Flag)
								CCD->ActorManager()->SetHoldAtEnd(Object->Actor, true);
							else
								Object->ActionActive = false;
						}
						if(Object->Index->Flag)
						{
							if(CCD->ActorManager()->EndAnimation(Object->Actor))
							{
								CCD->ActorManager()->SetHoldAtEnd(Object->Actor, false);
								Object->ActionActive = false;
							}
						}
						else
							runflag = true;
					}
					else
						Object->ActionActive = false;
					break;
				case LOOPANIMATION:
					if(Object->Actor)
					{
						if(Object->StartAction)
						{
							if(!EffectC_IsStringNull(Object->Index->AnimName))
								CCD->ActorManager()->SetMotion(Object->Actor, Object->Index->AnimName);
							Object->StartAction = false;
						}
						Object->Time += (dwTicks/1000.0f);
						if(Object->Time>=Object->Index->Speed)
						{
							Object->Time = 0.0f;
							Object->ActionActive = false;
							runflag = true;
						}
					}
					else
						Object->ActionActive = false;
					break;
				case ROTATEMOVETOPOINT:
					if(Object->ValidPoint && !Object->FacePlayer && Object->Actor)
					{
						if(RotateMoveToPoint((void *)Object, dwTicks))
							Object->ActionActive = false;
					}
					else
						Object->ActionActive = false;
					break;
				case NEWPATH:
					if(NextPath((void *)Object, dwTicks))
					{
						Object->ActionActive = false;
						runflag = true;
					}
					break;
				case RESTARTORDER:
					Object->RunOrder = true;
					Object->ActionActive = false;
					pool = Object->Bottom;
					while	(pool!= NULL)
					{
						temp = pool->next;
						geRam_Free(pool);
						pool = temp;
					}
					Object->Top = NULL;
					Object->Bottom = NULL;
					Object->Index = NULL;
					break;
				case ROTATEMOVE:
					if(!Object->FacePlayer && Object->Actor)
					{
						if(RotateMove((void *)Object, dwTicks))
							Object->ActionActive = false;
					}
					else
						Object->ActionActive = false;
					break;
				case NEWPOINT:
					if(!EffectC_IsStringNull(Object->Index->AnimName))
					{
						char *EntityType = CCD->EntityRegistry()->GetEntityType(Object->Index->AnimName);
						if(EntityType)
						{
							if(!stricmp(EntityType, "ScriptPoint"))
							{
								strcpy(Object->Point, Object->Index->AnimName);
								ScriptPoint *pProxy;
								CCD->ScriptPoints()->LocateEntity(Object->Point, (void**)&pProxy);
								Object->CurrentPoint = pProxy->origin;
								Object->ValidPoint = true;
								Object->NextOrder[0] = '\0';
								if(!EffectC_IsStringNull(pProxy->NextOrder))
									strcpy(Object->NextOrder, pProxy->NextOrder);
							}
						}
					}
					Object->ActionActive = false;
					runflag = true;
					break;
				case MOVEFORWARD:
					Object->Direction = RGF_K_FORWARD;
					if(Move((void *)Object, dwTicks))
						Object->ActionActive = false;
					break;
				case MOVEBACKWARD:
					Object->Direction = RGF_K_BACKWARD;
					if(Move((void *)Object, dwTicks))
						Object->ActionActive = false;
					break;
				case MOVELEFT:
					Object->Direction = RGF_K_LEFT;
					if(Move((void *)Object, dwTicks))
						Object->ActionActive = false;
					break;
				case MOVERIGHT:
					Object->Direction = RGF_K_RIGHT;
					if(Move((void *)Object, dwTicks))
						Object->ActionActive = false;
					break;
				case MOVE:
					Object->Direction = 0;
					if(Move((void *)Object, dwTicks))
						Object->ActionActive = false;
					break;
				case AVOIDORDER:
					if(!EffectC_IsStringNull(Object->Index->AnimName))
						strcpy(Object->AvoidOrder, Object->Index->AnimName);
					Object->ActionActive = false;
					runflag = true;
					break;
				case RETURN:
					if(Object->Stack)
					{
						Object->Pop();
						Object->ActionActive = true;
						Object->StartAction = true;
						Object->TargetDisable = false;
					}
					runflag = true;
					break;
				case JUMPCMD:
					if(Object->Actor)
					{
						if(Jump((void *)Object, dwTicks))
							Object->ActionActive = false;
					}
					else
						Object->ActionActive = false;
					break;
				case ADDTRIGGERORDER:
					AddTriggerOrder((void *)Object, dwTicks);
					Object->ActionActive = false;
					runflag = true;
					break;
				case DELTRIGGERORDER:
					DelTriggerOrder((void *)Object, dwTicks);
					Object->ActionActive = false;
					runflag = true;
					break;
				case SETEVENTSTATE:
					AddEvent(Object->Index->AnimName, Object->Index->Flag);
					Object->ActionActive = false;
					runflag = true;
					break;
				case FACEPLAYER:
					Object->FacePlayer = Object->Index->Flag;
					Object->FaceAxis = true;
					if(Object->Index->Speed<1.0f)
						Object->FaceAxis = false;
					Object->ActionActive = false;
					break;
				case ROTATETOPLAYER:
					if(!Object->FacePlayer && Object->Actor)
					{
						if(RotateToPlayer((void *)Object, dwTicks))
							Object->ActionActive = false;
					}
					else
						Object->ActionActive = false;
					break;
				case TELEPORTTOPOINT:
					if(!EffectC_IsStringNull(Object->Index->AnimName) && Object->Actor)
					{
						char *EntityType = CCD->EntityRegistry()->GetEntityType(Object->Index->AnimName);
						if(EntityType)
						{
							if(!stricmp(EntityType, "ScriptPoint"))
							{
								ScriptPoint *pProxy;
								CCD->ScriptPoints()->LocateEntity(Object->Index->AnimName, (void**)&pProxy);
								CCD->ActorManager()->Position(Object->Actor, pProxy->origin);
							}
						}
					}
					Object->ActionActive = false;
					runflag = true;
					break;
				case ANIMATIONSPEED:
					if(Object->Actor)
						CCD->ActorManager()->SetAnimationSpeed(Object->Actor, Object->AnimSpeed*Object->Index->Speed);
					Object->ActionActive = false;
					runflag = true;
					break;
				case SETFLAG:
					SetPawnFlag((int)Object->Index->Speed, Object->Index->Flag);
					Object->ActionActive = false;
					runflag = true;
					break;
				case STOPANIMATION:
					if(Object->Actor)
					{
						if(Object->StartAction)
						{
							if(!EffectC_IsStringNull(Object->Index->AnimName))
								CCD->ActorManager()->SetMotion(Object->Actor, Object->Index->AnimName);
							Object->StartAction = false;
							CCD->ActorManager()->SetHoldAtEnd(Object->Actor, true);
							if(Object->Index->Speed<0.0f)
								Object->ActionActive = false;
						}
						else
						{
							if(Object->Index->Speed<=0.0f)
							{
								//CCD->ActorManager()->SetHoldAtEnd(Object->Actor, false);
								Object->ActionActive = false;
							}
							else
							{
								if(CCD->ActorManager()->EndAnimation(Object->Actor))
								{
									Object->Index->Speed -= (dwTicks*0.001f);
								}
							}
						}
					}
					else
						Object->ActionActive = false;
					break;
				case ATTRIBUTEORDER:
					if(!EffectC_IsStringNull(Object->Index->AnimName) && !EffectC_IsStringNull(Object->Index->TriggerName) && Object->Actor)
					{
						strcpy(Object->DeadOrder, Object->Index->TriggerName);
						strcpy(Object->Attribute, Object->Index->AnimName);
						CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(Object->Actor);
						theInv->AddAndSet(Object->Attribute, (int)Object->Index->Speed);
						theInv->SetValueLimits(Object->Attribute, 0, (int)Object->Index->Speed);
						Object->OldAttributeAmount = (int)Object->Index->Speed;
					}
					Object->ActionActive = false;
					runflag = true;
					break;
				case REMOVE:
					if(Object->Actor)
					{
						CCD->ActorManager()->GetPosition(Object->Actor, &Object->DeadPos);
						CCD->ActorManager()->RemoveActor(Object->Actor);
						geActor_Destroy(&Object->Actor);
						Object->Actor = NULL;
					}
					Object->ActionActive = false;
					if(Object->Index->Flag)
						Object->alive = false;
					break;
				case SETNOCOLLISION:
					if(Object->Actor)
					{
						if(!Object->Index->Flag)
							CCD->ActorManager()->SetNoCollide(Object->Actor);
						else
							CCD->ActorManager()->SetCollide(Object->Actor);
					}
					Object->ActionActive = false;
					runflag = true;
					break;
				case ALLOWUSE:
					Object->UseKey = Object->Index->Flag;
					Object->ActionActive = false;
					runflag = true;
					break;
				case CONVERSATION:
					CCD->Pawns()->RunConverse(Object->Converse, Object->szName, Object->Icon);
					Object->ActionActive = false;
					runflag = true;
					break;
				case FADEIN:
					if(Object->Actor)
					{
						geFloat fAlpha;
						if(Object->StartAction)
						{
							CCD->ActorManager()->GetAlpha(Object->Actor, &fAlpha);
							Object->Index->Value2 = fAlpha;
							if(Object->Index->Value1>Object->Index->Value2)
							{
								Object->Index->Value3 = (Object->Index->Value1-Object->Index->Value2)/Object->Index->Speed;
							}
							else
								Object->ActionActive = false;
							Object->StartAction = false;
						}
						else
						{
							Object->Index->Value2 += (Object->Index->Value3*(dwTicks*0.001f));
							if(Object->Index->Value2>=Object->Index->Value1)
							{
								Object->Index->Value2 = Object->Index->Value1;
								Object->ActionActive = false;
							}
							CCD->ActorManager()->SetAlpha(Object->Actor, Object->Index->Value2);
						}
					}
					else
						Object->ActionActive = false;
					break;
				case FADEOUT:
					if(Object->Actor)
					{
						geFloat fAlpha;
						if(Object->StartAction)
						{
							CCD->ActorManager()->GetAlpha(Object->Actor, &fAlpha);
							Object->Index->Value2 = fAlpha;
							if(Object->Index->Value1<Object->Index->Value2)
							{
								Object->Index->Value3 = (Object->Index->Value2-Object->Index->Value1)/Object->Index->Speed;
							}
							else
								Object->ActionActive = false;
							Object->StartAction = false;
						}
						else
						{
							Object->Index->Value2 -= (Object->Index->Value3*(dwTicks*0.001f));
							if(Object->Index->Value2<=Object->Index->Value1)
							{
								Object->Index->Value2 = Object->Index->Value1;
								Object->ActionActive = false;
							}
							CCD->ActorManager()->SetAlpha(Object->Actor, Object->Index->Value2);
						}
					}
					else
						Object->ActionActive = false;
					break;
				case FIELDOFVIEW:
					Object->FOV = Object->Index->Speed/2.0f;
					Object->FOV = (90.0f-Object->FOV)/90.0f;
					Object->FOVBone[0] = '\0';
					if(!EffectC_IsStringNull(Object->Index->AnimName))
						strcpy(Object->FOVBone, Object->Index->AnimName);
					Object->ActionActive = false;
					runflag = true;
					break;
				case SETGROUP:
					if(!EffectC_IsStringNull(Object->Index->AnimName))
					{
						strcpy(Object->Group, Object->Index->AnimName);
						CCD->ActorManager()->SetGroup(Object->Actor, Object->Group);
					}
					else
					{
						Object->Group[0] = '\0';
						CCD->ActorManager()->SetGroup(Object->Actor, NULL);
					}
					Object->ActionActive = false;
					runflag = true;
					break;
				case HOSTILEPLAYER:
					Object->HostilePlayer = Object->Index->Flag;
					Object->ActionActive = false;
					runflag = true;
					break;
				case HOSTILEDIFF:
					Object->HostileDiff = Object->Index->Flag;
					Object->ActionActive = false;
					runflag = true;
					break;
				case HOSTILESAME:
					Object->HostileSame = Object->Index->Flag;
					Object->ActionActive = false;
					runflag = true;
					break;
				case ROTATETOTARGET:
					if(Object->TargetActor && !Object->FacePlayer && Object->Actor)
					{
						if(Object->StartAction)
						{
							Object->SavePoint = Object->CurrentPoint;
							CCD->ActorManager()->GetPosition(Object->TargetActor, &Object->CurrentPoint);
						}
						if(RotateToPoint((void *)Object, dwTicks))
						{
							Object->ActionActive = false;
							Object->CurrentPoint = Object->SavePoint;
						}
					}
					else
						Object->ActionActive = false;
					break;
				case MOVETOTARGET:
					if(Object->TargetActor && Object->Actor)
					{
						if(Object->StartAction)
						{
							Object->SavePoint = Object->CurrentPoint;
							CCD->ActorManager()->GetPosition(Object->TargetActor, &Object->CurrentPoint);
						}
						if(MoveToPoint((void *)Object, dwTicks))
						{
							Object->ActionActive = false;
							Object->CurrentPoint = Object->SavePoint;
						}
						else
						{
							if(Object->AvoidMode && !EffectC_IsStringNull(Object->AvoidOrder))
							{
								Object->Push();
								strcpy(Object->Order, Object->AvoidOrder);
								Object->RunOrder = true;
								Object->ActionActive = false;
								Object->DistActive = false;
								runflag = true;
							}
						}
					}
					else
						Object->ActionActive = false;
					break;
				case ROTATEMOVETOTARGET:
					if(Object->TargetActor && !Object->FacePlayer && Object->Actor)
					{
						if(Object->StartAction)
						{
							Object->SavePoint = Object->CurrentPoint;
							CCD->ActorManager()->GetPosition(Object->TargetActor, &Object->CurrentPoint);
						}
						if(RotateMoveToPoint((void *)Object, dwTicks))
						{
							Object->ActionActive = false;
							Object->CurrentPoint = Object->SavePoint;
						}
					}
					else
						Object->ActionActive = false;
					break;
				case LOWLEVEL:
					Object->highlevel = false;
					Object->ActionActive = false;
					Object->lowTime = 0.0f;;
					Object->ThinkTime = 0.0f;
					Object->ElapseTime = 0.0f;
					strcpy(Object->Order, Object->Index->AnimName);
					Object->RunOrder = false;
					pool = Object->Bottom;
					while	(pool!= NULL)
					{
						temp = pool->next;
						geRam_Free(pool);
						pool = temp;
					}
					Object->Top = NULL;
					Object->Bottom = NULL;
					Object->Index = NULL;
					break;
				case BOXWIDTH:
					if(Object->Actor)
					{
						geExtBox theBox;
						CCD->ActorManager()->GetBoundingBox(Object->Actor, &theBox);
						float m_CurrentHeight = theBox.Max.Y;
						if(Object->Index->Speed>0.0f)
							CCD->ActorManager()->SetBBox(Object->Actor, Object->Index->Speed, -m_CurrentHeight*2.0f, Object->Index->Speed);
					}
					Object->ActionActive = false;
					runflag = true;
					break;
				case SCALE:
					if(Object->Actor)
					{
						if(Object->Index->Speed>0.0f)
						{
							float scale;
							CCD->ActorManager()->GetScale(Object->Actor, &scale);
							CCD->ActorManager()->SetScale(Object->Actor, Object->Index->Speed*scale);
						}
					}
					Object->ActionActive = false;
					runflag = true;
					break;
				}
			}
		} while(runflag);
	}
}