
#include "RabidFramework.h"

extern geSound_Def *SPool_Sound(char *SName);

#include "Simkin\\skScriptedExecutable.h"
#include "Simkin\\skRValue.h"
#include "Simkin\\skRValueArray.h"

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
	TESTFLAG,
	STOPANIMATION,
	ATTRIBUTEORDER,
	REMOVE,
	SETNOCOLLISION,
	ALLOWUSE,
	CONVERSATION,
	FADEIN,
	FADEOUT,
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
	"TestFlagOrder",
	"StopAnimation",
	"AttributeOrder",
	"Remove",
	"SetNoCollision",
	"AllowUseKey",
	"Conversation",
	"FadeIn",
	"FadeOut",
	"Delay"
};

//
// ScriptedObject class
//

ScriptedObject::ScriptedObject(char *fileName) : skScriptedExecutable(fileName)
{
	RunOrder = false;
	Actor = NULL;
	Top = NULL;
	Bottom = NULL;
	Index = NULL;
	Stack = NULL;
	active = false;
	ValidPoint = false;
	ActionActive = false;
	Time = 0.0f;
	DistActive = false;
	AvoidOrder[0] = '\0';
	AvoidMode = false;
	Trigger = NULL;
	TriggerWait = false;
	Icon = NULL;
	SoundIndex = -1;
	FacePlayer = false;
	FaceAxis = true;
	Attribute[0] = '\0';
	DeadOrder[0] = '\0';
	Converse = NULL;
	UseKey = true;
}

ScriptedObject::~ScriptedObject()
{
	if(Actor)
	{
		CCD->ActorManager()->RemoveActor(Actor);
		geActor_Destroy(&Actor);
	}
	if(Icon)
		geBitmap_Destroy(&Icon);

	ActionList *pool, *temp;
	pool = Bottom;
	while	(pool!= NULL)
	{
		temp = pool->next;
		geRam_Free(pool);
		pool = temp;
	}
	ActionStack *spool, *stemp;
	spool = Stack;
	while	(spool!= NULL)
	{
		stemp = spool->next;
		pool = spool->Bottom;
		while	(pool!= NULL)
		{
			temp = pool->next;
			geRam_Free(pool);
			pool = temp;
		}
		geRam_Free(spool);
		spool = stemp;
	}
	TriggerStack *tpool, *ttemp;
	tpool = Trigger;
	while	(tpool!= NULL)
	{
		ttemp = tpool->next;
		geRam_Free(tpool);
		tpool = ttemp;
	}
}

bool ScriptedObject::getValue(const skString& fieldName, const skString& attribute, skRValue& value)
{
/*
	if (fieldName == "m_param")
	{
		// copy the value
		value = m_param;

		// we found the variable
		return true;
	}
	else */
	{
		return skScriptedExecutable::getValue(fieldName, attribute, value);
	}

}

// sets a field's value
bool ScriptedObject::setValue(const skString& fieldName, const skString& attribute, const skRValue& value)
{
/*
	if (fieldName == "m_param")
	{
		// assign the new value
		m_param = value.intValue();

		// we found the variable
		return true;
	}
	else */
	{
		return skScriptedExecutable::setValue(fieldName, attribute, value);
	}
}

// calls a method in this object
bool ScriptedObject::method(const skString& methodName, skRValueArray& arguments,skRValue& returnValue)
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
		AddAction(ADDTRIGGERORDER, param0, param1, false, 0.0f, 0.0f, 0.0f, 0.0f, NULL, param7);
		return true;
	}
	else if (IS_METHOD(methodName, "DelTriggerOrder"))
	{
		strcpy(param0, arguments[0].str());
		AddAction(DELTRIGGERORDER, param0, 0.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
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
	else if (IS_METHOD(methodName, "TestFlagOrder"))
	{
		strcpy(param0, arguments[2].str());
		param1 = (float)arguments[0].intValue();
		param2 = arguments[1].boolValue();
		if(param1>=0.0f && param1<100.0f)
			AddAction(TESTFLAG, param0, param1, param2, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
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

//
// ScriptConverse class
//

ScriptedConverse::ScriptedConverse(char *fileName) : skScriptedExecutable(fileName)
{
	M_CameraRect.Left = 0;
	M_CameraRect.Right = CCD->Engine()->Width() - 1;
	M_CameraRect.Top = 0;
	M_CameraRect.Bottom = CCD->Engine()->Height() - 1;
	m_Streams = NULL;
}

ScriptedConverse::~ScriptedConverse()
{
	if(m_Streams)
	{
		m_Streams->Delete();
		delete m_Streams;
	}
	m_Streams = NULL;
}

bool ScriptedConverse::getValue(const skString& fieldName, const skString& attribute, skRValue& value)
{
/*
	if (fieldName == "m_param")
	{
		// copy the value
		value = m_param;

		// we found the variable
		return true;
	}
	else */
	{
		return skScriptedExecutable::getValue(fieldName, attribute, value);
	}

}

// sets a field's value
bool ScriptedConverse::setValue(const skString& fieldName, const skString& attribute, const skRValue& value)
{
/*
	if (fieldName == "m_param")
	{
		// assign the new value
		m_param = value.intValue();

		// we found the variable
		return true;
	}
	else */
	{
		return skScriptedExecutable::setValue(fieldName, attribute, value);
	}
}

// calls a method in this object
bool ScriptedConverse::method(const skString& methodName, skRValueArray& arguments,skRValue& returnValue)
{
	char param0[128];
	int param1;
	bool param2;

	param0[0] = '\0';

	if (IS_METHOD(methodName, "random"))
	{
		if(arguments[0].floatValue()<=arguments[1].floatValue())
			returnValue = (int)EffectC_Frand(arguments[0].floatValue(), arguments[1].floatValue());
		else
			returnValue = (int)EffectC_Frand(arguments[1].floatValue(), arguments[0].floatValue());
		return true;
	}
	else if (IS_METHOD(methodName, "Speak"))
	{
		strcpy(param0, arguments[0].str());
		Text = CCD->Pawns()->GetText(param0);
		strcpy(param0, arguments[1].str());
		if(!EffectC_IsStringNull(param0))
		{
			char music[256];
			strcpy(music, CCD->GetDirectory(kAudioStreamFile));
			strcat(music, "\\");
			strcat(music, param0);
			m_dsPtr = (LPDIRECTSOUND)geSound_GetDSound();
			m_Streams = new StreamingAudio(m_dsPtr);
			if(m_Streams)
			{
				if(!m_Streams->Create(music))
				{
					delete m_Streams;
					m_Streams = NULL;
				}
			}
		}
		return true;
	}
	else if (IS_METHOD(methodName, "Reply"))
	{
		strcpy(param0, arguments[1].str());
		param1 = arguments[0].intValue();
		if(param1>9 || param1<1)
			return true;
		Reply[param1-1].Format("%d : ", param1);
		Reply[param1-1] += CCD->Pawns()->GetText(param0);
		Reply[param1-1].TrimRight();
		Reply[param1-1].TrimLeft();
		replyflg[param1-1] = true;
		return true;
	}
	else if (IS_METHOD(methodName, "Conversation"))
	{
		returnValue = DoConversation(arguments[0].intValue());
		return true;
	}
	else if (IS_METHOD(methodName, "NewConversation"))
	{
		strcpy(param0, arguments[0].str());
		param2 = arguments[1].boolValue();
		strcpy(Order, param0);
		ConvFlag = !param2;
		return true;
	}
	else if (IS_METHOD(methodName, "CustomIcon"))
	{
		strcpy(param0, arguments[0].str());
		param1 = arguments[1].intValue();
		if(param1>=0)
			IconX = param1;
		param1 = arguments[2].intValue();
		if(param1>=0)
			IconY = param1;
		geBitmap *TIcon = CCD->Pawns()->GetCache(param0);
		if(TIcon)
		{
			Icon = TIcon;
			if(Icon)
				geBitmap_SetColorKey(Icon, GE_TRUE, 255, GE_FALSE);
		}
		return true;
	}
	else if (IS_METHOD(methodName, "CustomBackground"))
	{
		strcpy(param0, arguments[0].str());
		geBitmap *TIcon = CCD->Pawns()->GetCache(param0);
		if(TIcon)
			Background = TIcon;
		return true;
	}
	else if (IS_METHOD(methodName, "CustomSpeak"))
	{
		param1 = arguments[0].intValue();
		if(param1>=0)
			SpeachX = param1;
		param1 = arguments[1].intValue();
		if(param1>=0)
			SpeachY = param1;
		param1 = arguments[2].intValue();
		if(param1>=0)
			SpeachWidth = param1;
		param1 = arguments[3].intValue();
		if(param1>=0)
			SpeachHeight = param1;
		param1 = arguments[4].intValue();
		if(param1>=0)
			SpeachFont = param1;
		return true;
	}
	else if (IS_METHOD(methodName, "CustomReply"))
	{
		param1 = arguments[0].intValue();
		if(param1>=0)
			ReplyX = param1;
		param1 = arguments[1].intValue();
		if(param1>=0)
			ReplyY = param1;
		param1 = arguments[2].intValue();
		if(param1>=0)
			ReplyWidth = param1;
		param1 = arguments[3].intValue();
		if(param1>=0)
			ReplyHeight = param1;
		param1 = arguments[4].intValue();
		if(param1>=0)
			ReplyFont = param1;
		return true;
	}
	else if (IS_METHOD(methodName, "SetFlag"))
	{
		param1 = arguments[0].intValue();
		if(param1>99)
			return true;
		param2 = arguments[1].boolValue();
		CCD->Pawns()->SetPawnFlag(param1, param2);
		return true;
	}
	else if (IS_METHOD(methodName, "GetFlag"))
	{
		param1 = arguments[0].intValue();
		if(param1>99)
			return true;
		returnValue = CCD->Pawns()->GetPawnFlag(param1);
		return true;
	}
	else if (IS_METHOD(methodName, "GetAttribute"))
	{
		strcpy(param0, arguments[0].str());
		CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
		returnValue = theInv->Value(param0);
		return true;
	}
	else if (IS_METHOD(methodName, "ModifyAttribute"))
	{
		strcpy(param0, arguments[0].str());
		param1 = arguments[1].intValue();
		CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
		theInv->Modify(param0, param1);
		return true;
	}
	else
	{
		return skScriptedExecutable::method(methodName, arguments, returnValue);
	}
}

int ScriptedConverse::DoConversation(int charpersec)
{
	int choice;
	char *temp = new char[Text.GetLength()+1];
	char *temp1 = NULL;
	DWORD OldTime = CCD->FreeRunningCounter();
	int counter = 1;
	if(charpersec==0)
		counter = Text.GetLength();
	DWORD ElapsedTime;
	int startline = 0;
	int replyline = 1;
	strcpy(temp, Text.Left(counter));

	CString Reply1 = "";
	for(int j=0;j<9;j++)
	{
		if(replyflg[j])
		{
			if(Reply1!="")
			{
				Reply1+=" ";
				Reply1.SetAt(Reply1.GetLength()-1, (char)1);
			}
			Reply1 += Reply[j];
		}
	}
	if(Reply1!="")
	{
		temp1 = new char[Reply1.GetLength()+1];
		strcpy(temp1, Reply1);
	}
//
// display scrolling text
//
	if(m_Streams)
	{
		m_Streams->Play(false);
	}

	while(counter<Text.GetLength())
	{
		MSG msg;
		while (PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE))
		{
			GetMessage(&msg, NULL, 0, 0 );
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		geEngine_BeginFrame(CCD->Engine()->Engine(), M_Camera, GE_TRUE);
		geEngine_DrawBitmap(CCD->Engine()->Engine(), Background, NULL, BackgroundX, BackgroundY );
		if(Icon)
			geEngine_DrawBitmap(CCD->Engine()->Engine(), Icon, NULL, BackgroundX+IconX, BackgroundY+IconY );
		ElapsedTime = (DWORD)(CCD->FreeRunningCounter() - OldTime);
		if(ElapsedTime > (DWORD)(1000/charpersec))
		{
			OldTime = CCD->FreeRunningCounter();
			counter +=1;
			strcpy(temp, Text.Left(counter));
		}
		TextDisplay(temp, SpeachWidth, SpeachFont);
		startline = TextOut(0, SpeachHeight, SpeachFont, SpeachX, SpeachY);
		
		geEngine_EndFrame(CCD->Engine()->Engine());
	}
//
// display text and allow scrolling
//
	int key = -1;
	bool keyup = false;
	bool keydwn = false;
	bool ckeyup = false;
	bool ckeydwn = false;
	while(1)
	{
		MSG msg;
		while (PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE))
		{
			GetMessage(&msg, NULL, 0, 0 );
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		bool kup = false;
		bool kdwn = false;
		bool ckup = false;
		bool ckdwn = false;
		geEngine_BeginFrame(CCD->Engine()->Engine(), M_Camera, GE_TRUE);
		geEngine_DrawBitmap(CCD->Engine()->Engine(), Background, NULL, BackgroundX, BackgroundY );
		if(Icon)
			geEngine_DrawBitmap(CCD->Engine()->Engine(), Icon, NULL, BackgroundX+IconX, BackgroundY+IconY );
		if(key==KEY_PAGEUP && startline!=0)
		{
			kup = true;
			if(!keyup)
			{
				keyup = true;
				if(startline>1)
					startline -=1;
			}
		}
		if(key==KEY_PAGEDOWN && startline!=0)
		{
			kdwn = true;
			if(!keydwn)
			{
				keydwn = true;
				startline +=1;
			}
		}
		TextDisplay(temp, SpeachWidth, SpeachFont);
		startline = TextOut(startline, SpeachHeight, SpeachFont, SpeachX, SpeachY);

		if(Reply1!="")
		{
			if(key==KEY_UP && replyline!=0)
			{
				ckup = true;
				if(!ckeyup)
				{
					ckeyup = true;
					if(replyline>1)
						replyline -=1;
				}
			}
			if(key==KEY_DOWN && replyline!=0)
			{
				ckdwn = true;
				if(!ckeydwn)
				{
					ckeydwn = true;
					replyline +=1;
				}
			}
			TextDisplay(temp1, ReplyWidth, ReplyFont);
			replyline = TextOut(replyline, ReplyHeight, ReplyFont, ReplyX, ReplyY);
		}

		geEngine_EndFrame(CCD->Engine()->Engine());

		if(Reply1!="")
		{
			if(key==KEY_1 && replyflg[0])
			{
				choice = 1;
				while(CCD->Input()->GetKeyboardInputNoWait()!=-1){};
				break;
			}
			if(key==KEY_2 && replyflg[1])
			{
				choice = 2;
				while(CCD->Input()->GetKeyboardInputNoWait()!=-1){};
				break;
			}
			if(key==KEY_3 && replyflg[2])
			{
				choice = 3;
				while(CCD->Input()->GetKeyboardInputNoWait()!=-1){};
				break;
			}
			if(key==KEY_4 && replyflg[3])
			{
				choice = 4;
				while(CCD->Input()->GetKeyboardInputNoWait()!=-1){};
				break;
			}
			if(key==KEY_5 && replyflg[4])
			{
				choice = 5;
				while(CCD->Input()->GetKeyboardInputNoWait()!=-1){};
				break;
			}
			if(key==KEY_6 && replyflg[5])
			{
				choice = 6;
				while(CCD->Input()->GetKeyboardInputNoWait()!=-1){};
				break;
			}
			if(key==KEY_7 && replyflg[6])
			{
				choice = 7;
				while(CCD->Input()->GetKeyboardInputNoWait()!=-1){};
				break;
			}
			if(key==KEY_8 && replyflg[7])
			{
				choice = 8;
				while(CCD->Input()->GetKeyboardInputNoWait()!=-1){};
				break;
			}
			if(key==KEY_9 && replyflg[8])
			{
				choice = 9;
				while(CCD->Input()->GetKeyboardInputNoWait()!=-1){};
				break;
			}
		}
		else
		{
			if(key==KEY_ESC)
			{
				choice = -1;
				while(CCD->Input()->GetKeyboardInputNoWait()!=-1){};
				break;
			}
		}
		if(!kup)
			keyup = false;
		if(!kdwn)
			keydwn = false;
		if(!ckup)
			ckeyup = false;
		if(!ckdwn)
			ckeydwn = false;
		
		
		key = CCD->Input()->GetKeyboardInputNoWait();
	}
	if(m_Streams)
	{
		m_Streams->Delete();
		delete m_Streams;
		m_Streams = NULL;
	}
	delete temp;
	if(temp1)
		delete temp1;
	return choice;
}

void ScriptedConverse::TextDisplay(char *Text, int Width, int Font)
{
	CString WText = Text;
	CString Line, Line1;
	int i, width, width1;
	char *temp = new char[strlen(Text)+1];
	int Index = 0;
	TextLines.RemoveAll();
	while(1)
	{
		Line = WText.Mid(Index);
		strcpy(temp, Line);
		width = CCD->MenuManager()->FontWidth(Font, temp);
		if(width<=Width)
		{
			if(Line.Find((char)1)==-1)
			{
				TextLines.Add(Line);
				break;
			}
		}
		i = 1;
		while(1)
		{
			Line1 = Line.Left(i);
			strcpy(temp, Line1);
			if(Line1[Line1.GetLength()-1]==1)
			{
				Line1 = Line.Left(i-1);
				TextLines.Add(Line1);
				Index += i;
				break;
			}
			width1 = CCD->MenuManager()->FontWidth(Font, temp);
			if(width1>Width)
			{
				if(Line1.Right(1)==" ")
				{
					Line1 = Line.Left(i-1);
					TextLines.Add(Line1);
					Index += i;
					break;
				}
				Line1 = Line.Left(i-1);
				int space = Line1.ReverseFind(' ');
				if(space==-1)
				{
					Line1 = Line.Left(i-1);
					TextLines.Add(Line1);
					Index += i;
					break;
				}
				Line1 = Line.Left(space);
				TextLines.Add(Line1);
				Index += (space+1);
				break;
			}
			i+=1;
		}
	}
	delete temp;
}

int ScriptedConverse::TextOut(int startline, int Height, int Font, int X, int Y)
{
	int sline = -1, eline;
	int THeight;
	int size = TextLines.GetSize();
	if(size>0)
	{
		if(startline==0)
		{
			sline = 0;
			eline = size;
			THeight = (size-sline) * (CCD->MenuManager()->FontHeight(Font)+2);
			while(THeight>Height)
			{
				sline += 1;
				THeight = (size-sline) * (CCD->MenuManager()->FontHeight(Font)+2);
			}
		}
		else
		{
			sline = startline-1;
			eline = size;
			THeight = (eline-sline) * (CCD->MenuManager()->FontHeight(Font)+2);
			if(sline>0 && THeight<Height)
			{
				while(THeight<Height)
				{
					sline -= 1;
					if(sline<0)
						break;
					THeight = (eline-sline) * (CCD->MenuManager()->FontHeight(Font)+2);
				}
				sline += 1;
			}
			else
			{
				while(THeight>Height)
				{
					eline -= 1;
					THeight = (eline-sline) * (CCD->MenuManager()->FontHeight(Font)+2);
				}
			}
		}
		int YOffset = Y;
		for(int i=sline; i<eline;i++)
		{
			CString Text = TextLines.GetAt(i);
			char *temp = new char[Text.GetLength()+1];
			strcpy(temp, Text);
			CCD->MenuManager()->FontRect(temp, Font, BackgroundX+X, BackgroundY+YOffset);
			YOffset += (CCD->MenuManager()->FontHeight(Font)+2);
			delete temp;
		}
	}
	return sline+1;
}

//
// Pawn Class
//

CPawn::CPawn()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	//	Ok, check to see if there are Pawns in this world
	
	Background = NULL;
	Icon = NULL;
	Events = NULL;
	for(int ii=0;ii<100;ii++)
		PawnFlag[ii] = false;

	pSet = geWorld_GetEntitySet(CCD->World(), "Pawn");
	
	if(pSet) 
	{
		LoadConv();
		Cache.SetSize(0);

		AttrFile.SetPath("pawn.ini");
		if(!AttrFile.ReadFile())
		{
			CCD->ReportError("Can't open Pawn initialization file", false);
			return;
		}
		CString KeyName = AttrFile.FindFirstKey();
		CString Type, Vector;
		geBitmap_Info	BmpInfo;
		char szName[64];
		while(KeyName != "")
		{
			if(!strcmp(KeyName,"Conversation"))
			{
				Type = AttrFile.GetValue(KeyName, "background");
				if(Type!="")
				{
					Type = "conversation\\"+Type;
					strcpy(szName,Type);
					Background = CreateFromFileName(szName);
					geEngine_AddBitmap(CCD->Engine()->Engine(), Background);
					geBitmap_GetInfo(Background, &BmpInfo, NULL);
					BackgroundX = (CCD->Engine()->Width() - BmpInfo.Width) / 2;
					BackgroundY = (CCD->Engine()->Height() - BmpInfo.Height) / 2;
				}
				IconX = AttrFile.GetValueI(KeyName, "iconx");
				IconY = AttrFile.GetValueI(KeyName, "icony");
				SpeachX = AttrFile.GetValueI(KeyName, "speachx");
				SpeachY = AttrFile.GetValueI(KeyName, "speachy");
				SpeachWidth = AttrFile.GetValueI(KeyName, "speachwidth");
				SpeachHeight = AttrFile.GetValueI(KeyName, "speachheight");
				SpeachFont = AttrFile.GetValueI(KeyName, "speachfont");
				ReplyX = AttrFile.GetValueI(KeyName, "replyx");
				ReplyY = AttrFile.GetValueI(KeyName, "replyy");
				ReplyWidth = AttrFile.GetValueI(KeyName, "replywidth");
				ReplyHeight = AttrFile.GetValueI(KeyName, "replyheight");
				ReplyFont = AttrFile.GetValueI(KeyName, "replyfont");
				break;
			}
			KeyName = AttrFile.FindNextKey();
		}

		KeyName = AttrFile.FindFirstKey();
		while(KeyName != "")
		{
			Type = AttrFile.GetValue(KeyName, "type");
			if(Type=="weapon")
			{
			}
			KeyName = AttrFile.FindNextKey();
		}

		//	Ok, we have Pawns somewhere.  Dig through 'em all.
		
		for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
		{
			Pawn *pSource = (Pawn*)geEntity_GetUserData(pEntity);
			if(EffectC_IsStringNull(pSource->szEntityName))
			{
				char szName[128];
				geEntity_GetName(pEntity, szName, 128);
				pSource->szEntityName = szName;
			}
			// Ok, put this entity into the Global Entity Registry
			CCD->EntityRegistry()->AddEntity(pSource->szEntityName, "Pawn");
			pSource->Data = NULL;
			pSource->Converse = NULL;
			if(!EffectC_IsStringNull(pSource->ScriptName))
			{
				char script[128] = "scripts\\";
				strcat(script, pSource->ScriptName);
				try
				{
					pSource->Data = new ScriptedObject(script);
				}
				catch (...)
				{
					char szBug[256];
					sprintf(szBug, "Script Error for %s", pSource->szEntityName);
					CCD->ReportError(szBug, false);
					continue;
				} 
				PreLoad(script);
				ScriptedObject *Object = (ScriptedObject *)pSource->Data;

				strcpy(Object->szName, pSource->szEntityName);

				KeyName = AttrFile.FindFirstKey();
				bool found = false;
				while(KeyName != "")
				{
					Type = AttrFile.GetValue(KeyName, "type");
					if(Type!="weapon")
					{
						if(!strcmp(KeyName,pSource->PawnType))
						{
							Object->active = false;
							Type = AttrFile.GetValue(KeyName, "actorname");
							strcpy(Object->ActorName, Type);
							Vector = AttrFile.GetValue(KeyName, "actorrotation");
							if(Vector!="")
							{
								strcpy(szName,Vector);
								Object->Rotation = Extract(szName);
								Object->Rotation.X *= 0.0174532925199433f;
								Object->Rotation.Y *= 0.0174532925199433f;
								Object->Rotation.Z *= 0.0174532925199433f;
							}
							geActor *Actor = CCD->ActorManager()->SpawnActor(Object->ActorName, 
								Object->Rotation, Object->Rotation, "", "", NULL);
							CCD->ActorManager()->RemoveActor(Actor);
							geActor_Destroy(&Actor);
							Object->Scale = (float)AttrFile.GetValueF(KeyName, "actorscale");
							Type = AttrFile.GetValue(KeyName, "animationspeed");
							Object->AnimSpeed = 1.0f;
							if(Type!="")
								Object->AnimSpeed = (float)AttrFile.GetValueF(KeyName, "animationspeed");
							geVec3d FillColor = {255.0f, 255.0f, 255.0f};
							geVec3d AmbientColor = {255.0f, 255.0f, 255.0f};
							Vector = AttrFile.GetValue(KeyName, "fillcolor");
							if(Vector!="")
							{
								strcpy(szName,Vector);
								FillColor = Extract(szName);
							}
							Vector = AttrFile.GetValue(KeyName, "ambientcolor");
							if(Vector!="")
							{
								strcpy(szName,Vector);
								AmbientColor = Extract(szName);
							}
							Object->FillColor.r = FillColor.X;
							Object->FillColor.g = FillColor.Y;
							Object->FillColor.b = FillColor.Z;
							Object->FillColor.a = 255.0f;
							Object->AmbientColor.r = AmbientColor.X;
							Object->AmbientColor.g = AmbientColor.Y;
							Object->AmbientColor.b = AmbientColor.Z;
							Object->AmbientColor.a = 255.0f;
							Object->ActorAlpha = 255.0f;
							Type = AttrFile.GetValue(KeyName, "actoralpha");
							if(Type!="")
								Object->ActorAlpha = (float)AttrFile.GetValueF(KeyName, "actoralpha");
							Object->Gravity = 0.0f;
							Type = AttrFile.GetValue(KeyName, "subjecttogravity");
							if(Type=="true")
								Object->Gravity = CCD->Player()->GetGravity();
							Object->BoxAnim[0] = '\0';
							Type = AttrFile.GetValue(KeyName, "boundingboxanimation");
							if(Type!="")
								strcpy(Object->BoxAnim, Type);
							Object->ShadowSize = (float)AttrFile.GetValueF(KeyName, "shadowsize");
							Object->Icon = NULL;
							Type = AttrFile.GetValue(KeyName, "icon");
							if(Type!="")
							{
								Type = "conversation\\"+Type;
								strcpy(szName,Type);
								Object->Icon = CreateFromFileName(szName);
								geEngine_AddBitmap(CCD->Engine()->Engine(), Object->Icon);
								geBitmap_SetColorKey(Object->Icon, GE_TRUE, 255, GE_FALSE);
							}
							
							Object->Location = pSource->origin;
							Object->HideFromRadar = pSource->HideFromRadar;
							Object->Order[0] = '\0';
							if(!EffectC_IsStringNull(pSource->SpawnOrder))
							{
								strcpy(Object->Order, pSource->SpawnOrder);
								Object->RunOrder = true;
							}
							Object->Point[0] = '\0';
							if(!EffectC_IsStringNull(pSource->SpawnPoint))
								strcpy(Object->Point, pSource->SpawnPoint);
							Object->ChangeMaterial[0] = '\0';
							if(!EffectC_IsStringNull(pSource->ChangeMaterial))
								strcpy(Object->ChangeMaterial, pSource->ChangeMaterial);
							Object->YRotation = 0.0174532925199433f*(pSource->Angle.Y-90.0f);
							Object->alive = true;
							
							found = true;
							break;
						}
					}
					KeyName = AttrFile.FindNextKey();
				}
				if(!found)
				{
					delete pSource->Data;
					pSource->Data = NULL;
					continue;
				}

				if(!EffectC_IsStringNull(pSource->ConvScriptName))
				{
					char script[128] = "scripts\\";
					strcat(script, pSource->ConvScriptName);
					try
					{
						pSource->Converse = new ScriptedConverse(script);
					}
					catch (...)
					{
						char szBug[256];
						sprintf(szBug, "Conversation Script Error for %s", Object->szName);
						CCD->ReportError(szBug, false);
						continue;
					}
					PreLoadC(script);
					ScriptedConverse *ConObject = (ScriptedConverse *)pSource->Converse;
					ConObject->Order[0] = '\0';
					if(!EffectC_IsStringNull(pSource->ConvOrder))
						strcpy(ConObject->Order, pSource->ConvOrder);
					Object->Converse = ConObject;
				}
			}
		}
	}
	return;
}

CPawn::~CPawn()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	//	Ok, check to see if there are Pawns in this world

	if(Background)
		geBitmap_Destroy(&Background);

	int keynum = Cache.GetSize();
	if(keynum>0)
	{
		for(int i=0;i<keynum;i++)
		{
			geBitmap_Destroy(&Cache[i].Bitmap);
		}
	}

	EventStack *pool, *temp;
	pool = Events;
	while(pool!= NULL)
	{
		temp = pool->next;
		geRam_Free(pool);
		pool = temp;
	}

	pSet = geWorld_GetEntitySet(CCD->World(), "Pawn");
	
	if(pSet) 
	{
		for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
		{
			Pawn *pSource = (Pawn*)geEntity_GetUserData(pEntity);
			if(pSource->Data)
			{
				ScriptedObject *Object = (ScriptedObject *)pSource->Data;
				delete Object;
				pSource->Data = NULL;
			}
			if(pSource->Converse)
			{
				ScriptedConverse *Converse = (ScriptedConverse *)pSource->Converse;
				delete Converse;
				pSource->Converse = NULL;
			}
		}
	}
}

void CPawn::Tick(float dwTicks)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	skRValueArray args(1);
	skRValue ret;
	int yoffset = 0;
	
	//	Ok, check to see if there are Pawns in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "Pawn");
	
	if(pSet) 
	{
		for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
		{
			Pawn *pSource = (Pawn*)geEntity_GetUserData(pEntity);
			if(pSource->Data)
			{
				ScriptedObject *Object = (ScriptedObject *)pSource->Data;
				
				if(!EffectC_IsStringNull(pSource->SpawnTrigger) && !Object->active)
				{
					if(!GetTriggerState(pSource->SpawnTrigger))
						continue;
				}
				if(!Object->alive)
					continue;
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
				if(!Object->active)
				{
					Object->active = true;
					Spawn(Object);
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
								Object->NextOrder[0] = '\0';
								if(!EffectC_IsStringNull(pProxy->NextOrder))
									strcpy(Object->NextOrder, pProxy->NextOrder);
							}
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
							if(Object->TriggerTime<=0.0f)
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
							if(Object->Trigger)
							{
								TriggerStack *tpool, *ttemp;
								tpool = Object->Trigger;
								while	(tpool!= NULL)
								{
									ttemp = tpool->next;
									if(GetTriggerState(tpool->TriggerName))
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
						if(Object->DistActive)
						{
							bool flg;
							if(Object->MinDistance>=0.0f)
							{
								if(Object->Actor)
									flg = CCD->ActorManager()->DistanceFrom(Object->Actor, CCD->Player()->GetActor())<Object->MinDistance;
								else
									flg = CCD->ActorManager()->DistanceFrom(Object->DeadPos, CCD->Player()->GetActor())<Object->MinDistance;
							}
							else
							{
								if(Object->Actor)
									flg = CCD->ActorManager()->DistanceFrom(Object->Actor, CCD->Player()->GetActor())>fabs(Object->MinDistance);
								else
									flg = CCD->ActorManager()->DistanceFrom(Object->DeadPos, CCD->Player()->GetActor())>fabs(Object->MinDistance);
							}
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
						if(Object->RunOrder)
						{
							bool methoderror = false;
							try
							{
								Object->method(skString(Object->Order), args, ret);
							}
							catch (...)
							{
								char szBug[256];
								sprintf(szBug, "Script Error for %s in Order %s", Object->szName, Object->Order);
								CCD->ReportError(szBug, false);
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
										Sound.SoundDef = SPool_Sound(Object->Index->SoundName);
										Object->SoundIndex = CCD->EffectManager()->Item_Add(EFF_SND, (void *)&Sound);
									}
								}
							}
						}
						
						if(Object->Index)
						{
							//geEngine_Printf(CCD->Engine()->Engine(), 2, yoffset,"%s %s %s",Object->szName, Object->Order, ActionText[Object->Index->Action]);

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
									LookRotation.X = -(float)( GE_PI*0.5 ) + 
										(float)acos(LookRotation.Y / l);
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
								if(Object->ValidPoint && !Object->FacePlayer && Object->Actor)
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
								if(Object->ValidPoint && !Object->FacePlayer && Object->Actor)
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
								}
								runflag = true;
								break;
							case JUMPCMD:
								if(Object->ValidPoint && Object->Actor)
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
							case TESTFLAG:
								if(GetPawnFlag((int)Object->Index->Speed)==Object->Index->Flag)
								{
									strcpy(Object->Order, Object->Index->AnimName);
									Object->RunOrder = true;
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
								runflag = true;
								Object->ActionActive = false;
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
											CCD->ActorManager()->SetHoldAtEnd(Object->Actor, false);
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
									CCD->ActorManager()->SetNoCollide(Object->Actor);
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
							}
						}
					} while(runflag);
				}
				yoffset +=10;
			}
		}
	}
}

void CPawn::Spawn(void *Data)
{
	ScriptedObject *Object = (ScriptedObject *)Data;
	geVec3d theRotation;

	Object->Actor = CCD->ActorManager()->SpawnActor(Object->ActorName, 
		Object->Location, Object->Rotation, Object->BoxAnim, Object->BoxAnim, NULL);
	CCD->ActorManager()->SetActorDynamicLighting(Object->Actor, Object->FillColor, Object->AmbientColor);
	CCD->ActorManager()->SetShadow(Object->Actor, Object->ShadowSize);
	CCD->ActorManager()->SetHideRadar(Object->Actor, Object->HideFromRadar);
	CCD->ActorManager()->SetScale(Object->Actor, Object->Scale);
	CCD->ActorManager()->SetType(Object->Actor, ENTITY_NPC);
	CCD->ActorManager()->SetAutoStepUp(Object->Actor, true);
	CCD->ActorManager()->SetBoxChange(Object->Actor, false);
	CCD->ActorManager()->SetColDetLevel(Object->Actor, RGF_COLLISIONLEVEL_2);
	CCD->ActorManager()->SetAnimationSpeed(Object->Actor, Object->AnimSpeed);
	CCD->ActorManager()->SetTilt(Object->Actor, true);
	if(!EffectC_IsStringNull(Object->ChangeMaterial))
		CCD->ActorManager()->ChangeMaterial(Object->Actor, Object->ChangeMaterial);
	if(!stricmp(Object->BoxAnim, "nocollide"))
	{
		CCD->ActorManager()->SetNoCollide(Object->Actor);
		geWorld_SetActorFlags(CCD->World(), Object->Actor, 0);
	}
	else if(!EffectC_IsStringNull(Object->BoxAnim))
	{
		CCD->ActorManager()->SetBoundingBox(Object->Actor, Object->BoxAnim);
	}
	CCD->ActorManager()->SetGravity(Object->Actor, Object->Gravity);
	CCD->ActorManager()->SetAlpha(Object->Actor, Object->ActorAlpha);

	theRotation.X = 0.0f; 
	theRotation.Y = Object->YRotation;
	theRotation.Z = 0.0f; 
	CCD->ActorManager()->Rotate(Object->Actor, theRotation);
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

//geEngine_Printf(CCD->Engine()->Engine(), 0,20,"Vec = %f %f %f",Object->Vec2Point.X,Object->Vec2Point.Y,Object->Vec2Point.Z);
//geEngine_Printf(CCD->Engine()->Engine(), 0,10,"Mov = %f %f %f",MoveVec.X,MoveVec.Y,MoveVec.Z);

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

bool CPawn::Converse(geActor *pActor)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	skRValueArray args(1);
	skRValue ret;

	//	Ok, check to see if there are Pawns in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "Pawn");
	
	if(pSet) 
	{
		for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
		{
			Pawn *pSource = (Pawn*)geEntity_GetUserData(pEntity);
			if(pSource->Data)
			{
				ScriptedObject *Object = (ScriptedObject *)pSource->Data;
				if(Object->Actor==pActor && Object->active && Object->UseKey)
				{
					if(pSource->Converse)
					{
						ScriptedConverse *Converse = (ScriptedConverse *)pSource->Converse;
						RunConverse(Converse, Object->szName, Object->Icon);
					}
					return true;
				}
			}
		}
	}
	return false;
}

void CPawn::RunConverse(ScriptedConverse *Converse, char *szName, geBitmap *OIcon)
{
	skRValueArray args(1);
	skRValue ret;

	if(!EffectC_IsStringNull(Converse->Order))
	{
		bool methoderror = false;
		try
		{
			Converse->CreateCamera();
			do
			{
				Converse->Background = Background;
				Converse->Icon = OIcon;
				Converse->BackgroundX = BackgroundX;
				Converse->BackgroundY = BackgroundY;
				Converse->IconX = IconX;
				Converse->IconY = IconY;
				Converse->SpeachX = SpeachX;
				Converse->SpeachY = SpeachY;
				Converse->SpeachWidth = SpeachWidth;
				Converse->SpeachHeight = SpeachHeight;
				Converse->SpeachFont = SpeachFont;
				Converse->ReplyX = ReplyX;
				Converse->ReplyY = ReplyY;
				Converse->ReplyWidth = ReplyWidth;
				Converse->ReplyHeight = ReplyHeight;
				Converse->ReplyFont = ReplyFont;
				Converse->ConvFlag = true;
				for(int i=0;i<9;i++)
					Converse->replyflg[i] = false;
				Converse->method(skString(Converse->Order), args, ret);
			} while (!Converse->ConvFlag);
		}
		catch (...)
		{
			char szBug[256];
			sprintf(szBug, "Conversation Script Error for %s in Order %s", szName, Converse->Order);
			CCD->ReportError(szBug, false);
			methoderror = true;
		}
		Converse->DestroyCamera();
		CCD->ResetClock();
		CCD->Engine()->BeginFrame();
		CCD->Engine()->EndFrame();
		CCD->ResetClock();
	}
}

void CPawn::PreLoad(char *filename)
{
	FILE *fdInput = NULL;
	char szInputString[1024] = {""};
	CString str;
	int i, j;
	char file[256];

	fdInput = fopen(filename, "rt");
	while(fgets(szInputString, 1024, fdInput) != NULL)
	{
		if(szInputString[0] == ';' || strlen(szInputString) <= 1)
			continue;
		str = szInputString;
		str.TrimRight();
		str.MakeLower();
		i = str.Find(".wav");
		if(i>=0 && i<str.GetLength())
		{
			j=i-1;
			while(str.GetAt(j)!='"' && j>=0)
			{
				j-=1;
			}
			if(j>=0)
			{
				strcpy(file, str.Mid(j+1,i-j+3));
				SPool_Sound(file);
			}
		} 
	}
	fclose(fdInput);
}


void CPawn::PreLoadC(char *filename)
{
	FILE *fdInput = NULL;
	char szInputString[1024] = {""};
	CString str;
	int i, j;
	char file[256];

	fdInput = fopen(filename, "rt");
	while(fgets(szInputString, 1024, fdInput) != NULL)
	{
		if(szInputString[0] == ';' || strlen(szInputString) <= 1)
			continue;
		str = szInputString;
		str.TrimRight();
		str.MakeLower();
		i = str.Find(".bmp");
		if(i>=0 && i<str.GetLength())
		{
			j=i-1;
			while(str.GetAt(j)!='"' && j>=0)
			{
				j-=1;
			}
			if(j>=0)
			{
				strcpy(file, "conversation\\");
				strcat(file, str.Mid(j+1,i-j+3));
				Cache.SetSize(Cache.GetSize()+1);
				int keynum = Cache.GetSize()-1;
				Cache[keynum].Name = str.Mid(j+1,i-j+3);
				Cache[keynum].Bitmap = CreateFromFileName(file);
				geEngine_AddBitmap(CCD->Engine()->Engine(), Cache[keynum].Bitmap);
			}
		} 
	}
	fclose(fdInput);
}

void CPawn::LoadConv()
{
	geVFile *MainFS;
	char szInputLine[256];
	CString readinfo, keyname, text;

	Text.SetSize(0);

	if(!CCD->OpenRFFile(&MainFS, kInstallFile, "conversation.txt", GE_VFILE_OPEN_READONLY))
		return;

	keyname = "";
	text = "";

	while(geVFile_GetS(MainFS, szInputLine, 256)==GE_TRUE)
	{
		if(strlen(szInputLine) <= 1)
			readinfo = "";
		else
			readinfo = szInputLine;

		if (readinfo != "" && readinfo[0] != ';')
		{
			readinfo.TrimRight();
			if (readinfo[0] == '[' && readinfo[readinfo.GetLength()-1] == ']')
			{
				if(keyname!="" && text!="")
				{
					Text.SetSize(Text.GetSize()+1);
					int keynum = Text.GetSize()-1;
					Text[keynum].Name = keyname;
					Text[keynum].Text = text;
				}
				keyname = readinfo;
				keyname.TrimLeft('[');
				keyname.TrimRight(']');
				text = "";
			}
			else
			{
				if(readinfo=="<CR>")
				{
					text+=" ";
					text.SetAt(text.GetLength()-1, (char)1);
				}
				else
				{
					if(text!="" && text[text.GetLength()-1]!=1)
						text+=" ";
					text+=readinfo;
				}
			}
		}
	}
	if(keyname!="" && text!="")
	{
		Text.SetSize(Text.GetSize()+1);
		int keynum = Text.GetSize()-1;
		Text[keynum].Name = keyname;
		Text[keynum].Text = text;
	}
	geVFile_Close(MainFS);
}

CString CPawn::GetText(char *Name)
{
	int size = Text.GetSize();
	if(size<1)
		return "";
	for(int i=0;i<size;i++)
	{
		if(Text[i].Name==Name)
			return Text[i].Text;
	}
	return "";
}

geBitmap *CPawn::GetCache(char *Name)
{
	int keynum = Cache.GetSize();
	if(keynum>0)
	{
		for(int i=0;i<keynum;i++)
		{
			if(!strcmp(Cache[i].Name, Name))
			{
				return Cache[i].Bitmap;
			}
		}
	}
	return NULL;
}

//	******************** CRGF Overrides ********************

//	LocateEntity
//
//	Given a name, locate the desired item in the currently loaded level
//	..and return it's user data.

int CPawn::LocateEntity(char *szName, void **pEntityData)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	//	Ok, check to see if there are Pawns in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "Pawn");
	
	if(!pSet) 
		return RGF_NOT_FOUND;
	
	//	Ok, we have Pawns.  Dig through 'em all.
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		Pawn *pTheEntity = (Pawn*)geEntity_GetUserData(pEntity);
		if(strcmp(pTheEntity->szEntityName, szName) == 0)
		{
			*pEntityData = (void *)pTheEntity;
			return RGF_SUCCESS;
		}
	}
	
	return RGF_NOT_FOUND;								// Sorry, no such entity here
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