/************************************************************************************//**
 * @file CPawnHigh.cpp
 * @brief Pawn High Manager
 *
 * This file contains the class implementation of the Pawn High Manager system
 * for Reality Factory.
 * @author Ralph Deane
 *//*
 * Copyright (c) Ralph Deane; All rights reserved.
 ****************************************************************************************/

#include "RabidFramework.h"
#include <Ram.h>
#include "RGFScriptMethods.h"
#include "CScriptManager.h"
#include "CGUIManager.h"
#include "CHeadsUpDisplay.h"
#include "CInventory.h"
#include "CConversation.h"
#include "CConversationManager.h"
#include "CLanguageManager.h"
#include "CLevel.h"
#include "CScriptPoint.h"
#include "CPawn.h"
#include "CPawnManager.h"
#include "RFSX/sxVector.h"

extern geSound_Def *SPool_Sound(const char *SName);

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
	BLENDTOANIMATION,
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
	PLAYERDISTORDER,
	ADDPAINORDER,
	FINDTARGETORDER,
	FINDPOINTORDER,
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
	SETCOLLISION,
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
	GRAVITY,
	FIREPROJECTILE,
	ADDEFFECT,
	TARGETGROUP,
	TESTDAMAGEORDER,
	CHANGEMATERIAL,
	CHANGEWEAPONMATERIAL,
	ATTACHTOACTOR,
	DETACHFROMACTOR,
	ATTACHBLENDACTOR,
	DETACHBLENDACTOR,
	ATTACHACCESSORY,
	DETACHACCESSORY,
	SETWEAPON,
	REMOVEWEAPON,
	ISPUSHABLE,
	HIDEFROMRADAR,
	STEPHEIGHT,
	ISVEHICLE,
	ALIGN,
	SETKEYPAUSE,
	SETHUDDRAW,
	SOUNDLOOP,
	AUDIBLERADIUS,
	SPEAK,
	ROTATEAROUNDPOINTLEFT,
	ROTATEAROUNDPOINTRIGHT,
	SETLODDISTANCE,
	DELAY,
	SHOWTEXTDELAY,
	SHOWTEXT,
	REMOVETEXT,
	CONCAT,
	GETFLAG,
	GETEVENTSTATE,
	SETSCALE,
	BOXHEIGHT,
	MOUSECONTROLLEDPLAYER,
	PAUSESCRIPT,
	RESUMESCRIPT,
	ENDSCRIPT,
	DEBUG
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
	"BlendToAnimation",
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
	"PlayerDistOrder",
	"AddPainOrder",
	"FindTargetOrder",
	"FindPointOrder",
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
	"SetCollision",
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
	"Gravity",
	"FireProjectile",
	"AddExplosion",
	"TargetGroup",
	"TestDamageOrder",
	"ChangeMaterial",
	"ChangeWeaponMaterial",
	"AttachToActor",
	"DetachFromActor",
	"AttachBlendActor",
	"DetachBlendActor",
	"AttachAccessory",
	"DetachAccessory",
	"SetWeapon",
	"RemoveWeapon",
	"IsPushable",
	"HideFromRadar",
	"StepHeight",
	"IsVehicle",
	"Align",
	"SetKeyPause",
	"SetHudDraw",
	"SoundLoop",
	"AudibleRadius",
	"Speak"
	"RotateAroundPointLeft",
	"RotateAroundPointRight",
	"SetLODDistance",
	"Delay",
	"ShowTextDelay",
	"ShowText",
	"RemoveText",
	"Concat",
	"GetFlag",
	"GetEventState",
	"SetScale",
	"BoxHeight",
	"MouseControlledPlayer",
	"PauseScript",
	"ResumeScript",
	"EndScript",
	"debug"
};


#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE ScriptedObject

#ifdef SX_METHOD_ARGS
#undef SX_METHOD_ARGS
#endif
#define SX_METHOD_ARGS sxScriptedObjectMethodArgs

typedef struct SX_METHOD_ARGS
{
	bool (SX_IMPL_TYPE::*method) (float timeElapsed, skRValueArray& args);
	skRValueArray args;
	unsigned long action;
} SX_METHOD_ARGS;


void SX_IMPL_TYPE::Enqueue(SX_METHOD_ARGS* ma)
{
	if(ma)
	{
		if(m_MethodQueueStack.empty())
		{
			m_MethodQueueStack.push(new std::queue<SX_METHOD_ARGS*>());
			m_MethodQueueVariablesStack.push(new MethodQueueVariables());
		}

		m_MethodQueueStack.top()->push(ma);
	}
}

void SX_IMPL_TYPE::Dequeue()
{
	if(!m_MethodQueueStack.empty())
	{
		if(!m_MethodQueueStack.top()->empty())
		{
			delete m_MethodQueueStack.top()->front();
			m_MethodQueueStack.top()->pop();
		}
	}
}

void SX_IMPL_TYPE::DequeueTop()
{
	if(!m_MethodQueueStack.empty())
	{
		while(!m_MethodQueueStack.top()->empty())
		{
			delete m_MethodQueueStack.top()->front();
			m_MethodQueueStack.top()->pop();
		}
	}
}

void SX_IMPL_TYPE::DequeueAll()
{
	while(!m_MethodQueueStack.empty())
	{
		while(!m_MethodQueueStack.top()->empty())
		{
			delete m_MethodQueueStack.top()->front();
			m_MethodQueueStack.top()->pop();
		}

		delete m_MethodQueueStack.top();
		m_MethodQueueStack.pop();
		delete m_MethodQueueVariablesStack.top();
		m_MethodQueueVariablesStack.pop();
	}
}

#define USE_METHOD_QUEUE_STACK	1

/* ------------------------------------------------------------------------------------ */
//	calls a method in this object
/* ------------------------------------------------------------------------------------ */
bool ScriptedObject::highmethod(const skString& methodName, skRValueArray& arguments,
								skRValue& returnValue,skExecutableContext &ctxt)
{
	char param0[128];
	int iparam;

	param0[0] = '\0';

	switch(ScriptManager::GetHashMethod(methodName))
	{
	case RGF_SM_MOVETOPOINT:
		{
			PARMCHECK(3);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(MoveToPoint);
			ma->args = arguments;
			ma->action = MOVETOPOINT;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_ROTATETOPOINT:
		{
			PARMCHECK(4);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(RotateToPoint);
			ma->args = arguments;
			ma->action = ROTATETOPOINT;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_NEWORDER:
		{
			PARMCHECK(1);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(NewOrder);
			ma->args = arguments;
			ma->action = NEWORDER;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_NEXTORDER:
		{
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(NextOrder);
			ma->args = arguments;
			ma->action = NEXTORDER;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_ROTATETOALIGN:
		{
			PARMCHECK(4);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(RotateToAlign);
			ma->args = arguments;
			ma->action = ROTATETOALIGN;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_NEXTPOINT:
		{
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(NextPoint);
			ma->args = arguments;
			ma->action = NEXTPOINT;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_DELAY:
		{
			PARMCHECK(3);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(Delay);
			ma->args = arguments;
			ma->action = DELAY;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_PLAYANIMATION:
		{
			PARMCHECK(3);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(PlayAnimation);
			ma->args = arguments;
			ma->action = PLAYANIMATION;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_BLENDTOANIMATION:
		{
			PARMCHECK(4);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(BlendToAnimation);
			ma->args = arguments;
			ma->action = BLENDTOANIMATION;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_LOOPANIMATION:
		{
			PARMCHECK(3);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(LoopAnimation);
			ma->args = arguments;
			ma->action = LOOPANIMATION;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_ROTATE:
		{
			PARMCHECK(6);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(Rotate);
			ma->args = arguments;
			ma->action = ROTATE;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_ROTATEMOVETOPOINT:
		{
			PARMCHECK(5);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(RotateMoveToPoint);
			ma->args = arguments;
			ma->action = ROTATEMOVETOPOINT;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_ROTATEMOVE:
		{
			PARMCHECK(7);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(RotateMove);
			ma->args = arguments;
			ma->action = ROTATEMOVE;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_NEWPATH:
		{
			PARMCHECK(1);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(NewPath);
			ma->args = arguments;
			ma->action = NEWPATH;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_RESTARTORDER:
		{
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(RestartOrder);
			ma->args = arguments;
			ma->action = RESTARTORDER;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_PLAYERDISTORDER:
		{
			PARMCHECK(2);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(PlayerDistOrder);
			ma->args = arguments;
			ma->action = PLAYERDISTORDER;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_CONSOLE:
		{
			PARMCHECK(1);
			m_Console = arguments[0].boolValue();

			if(m_Console)
			{
				m_ConsoleHeader = new char[128];
				*m_ConsoleHeader = '\0';
				m_ConsoleError = new char[128];
				*m_ConsoleError = '\0';

				for(int i=0; i<DEBUGLINES; ++i)
				{
					m_ConsoleDebug[i] = new char[64];
					*m_ConsoleDebug[i] = '\0';
				}
			}
			else
			{
				SAFE_DELETE_A(m_ConsoleHeader);
				SAFE_DELETE_A(m_ConsoleError);

				for(int i=0; i<DEBUGLINES; ++i)
				{
					SAFE_DELETE_A(m_ConsoleDebug[i]);
				}
			}

			return true;
		}
	case RGF_SM_AUDIBLERADIUS:
		{
			PARMCHECK(1);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(AudibleRadius);
			ma->args = arguments;
			ma->action = AUDIBLERADIUS;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_ADDPAINORDER:
		{
			PARMCHECK(2);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(AddPainOrder);
			ma->args = arguments;
			ma->action = ADDPAINORDER;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_FINDTARGETORDER:
		{
			PARMCHECK(3);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(FindTargetOrder);
			ma->args = arguments;
			ma->action = FINDTARGETORDER;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_FINDPOINTORDER:
		{
			PARMCHECK(1);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(FindPointOrder);
			ma->args = arguments;
			ma->action = FINDPOINTORDER;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_NEWPOINT:
		{
			PARMCHECK(1);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(NewPoint);
			ma->args = arguments;
			ma->action = NEWPOINT;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_MOVEFORWARD:
		{
			PARMCHECK(4);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(MoveForward);
			ma->args = arguments;
			ma->action = MOVEFORWARD;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_MOVEBACKWARD:
		{
			PARMCHECK(4);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(MoveBackward);
			ma->args = arguments;
			ma->action = MOVEBACKWARD;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_MOVELEFT:
		{
			PARMCHECK(4);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(MoveLeft);
			ma->args = arguments;
			ma->action = MOVELEFT;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_MOVERIGHT:
		{
			PARMCHECK(4);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(MoveRight);
			ma->args = arguments;
			ma->action = MOVERIGHT;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_MOVE:
		{
			PARMCHECK(3);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			if(arguments.entries() < 7)
			{
				if(arguments.entries() < 6)
				{
					if(arguments.entries() < 5)
					{
						if(arguments.entries() < 4)
						{
							ma->args.append(0.0f);
						}
						ma->args.append(0.0f);
					}
					ma->args.append(0.0f);
				}
				ma->args.append(skString(""));
			}
			ma->method = Q_P_FUNC(Move);
			ma->args = arguments;
			ma->action = MOVE;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_AVOIDORDER:
		{
			PARMCHECK(1);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(AvoidOrder);
			ma->args = arguments;
			ma->action = AVOIDORDER;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_RETURN:
		{
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(Return);
			ma->args = arguments;
			ma->action = RETURN;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_ALIGN:
		{
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(Align);
			ma->args = arguments;
			ma->action = ALIGN;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_JUMP:
		{
			PARMCHECK(4);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(Jump);
			ma->args = arguments;
			ma->action = JUMPCMD;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_ADDTRIGGERORDER:
		{
			PARMCHECK(3);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(AddTriggerOrder);
			ma->args.append(arguments[1].str());
			ma->args.append(arguments[0].str());
			ma->args.append(PTRIGGER);
			ma->args.append(arguments[2].floatValue());
			ma->args.append(false);
			ma->args.append(0);
			ma->args.append(0.0f);
			ma->args.append(0.0f);
			ma->args.append(0.0f);
			ma->action = ADDTRIGGERORDER;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_DELTRIGGERORDER:
		{
			PARMCHECK(1);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(DelTriggerOrder);
			ma->args = arguments;
			ma->action = DELTRIGGERORDER;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_SETEVENTSTATE:
		{
			PARMCHECK(2);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(SetEventState);
			ma->args = arguments;
			ma->action = SETEVENTSTATE;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_FACEPLAYER:
		{
			PARMCHECK(2);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(FacePlayer);
			ma->args = arguments;
			ma->action = FACEPLAYER;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_ROTATETOPLAYER:
		{
			PARMCHECK(4);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(RotateToPlayer);
			ma->args = arguments;
			ma->action = ROTATETOPLAYER;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_ROTATEAROUNDPOINTLEFT:
		{
			PARMCHECK(4);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(RotateAroundPoint);
			ma->args = arguments;
			ma->args.append(false);
			ma->action = ROTATEAROUNDPOINTLEFT;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_ROTATEAROUNDPOINTRIGHT:
		{
			PARMCHECK(4);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(RotateAroundPoint);
			ma->args = arguments;
			ma->args.append(true);
			ma->action = ROTATEAROUNDPOINTRIGHT;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_TELEPORTTOPOINT:
		{
			PARMCHECK(1);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(TeleportToPoint);
			ma->args = arguments;
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
								ma->args.append(0.0f);
							}
							ma->args.append(0.0f);
						}
						ma->args.append(0.0f);
					}
					ma->args.append(false);
				}
				ma->args.append(skString(""));
			}

			ma->action = TELEPORTTOPOINT;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_ANIMATIONSPEED:
		{
			PARMCHECK(1);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(AnimationSpeed);
			ma->args = arguments;
			ma->action = ANIMATIONSPEED;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_SETFLAG:
		{
			PARMCHECK(2);
			iparam = arguments[0].intValue();

			if(iparam >= 0 && iparam < MAXFLAGS)
			{
				SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
				ma->method = Q_P_FUNC(SetFlag);
				ma->args = arguments;
				ma->action = SETFLAG;
				Enqueue(ma);
			}
			return true;
		}
	case RGF_SM_ADDFLAGORDER:
		{
			PARMCHECK(3);
			iparam = arguments[0].intValue();

			if(iparam >= 0 && iparam < MAXFLAGS)
			{
				sprintf(param0, "PawnFlag%d", iparam);

				SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
				ma->method = Q_P_FUNC(AddTriggerOrder);
				ma->args.append(param0);
				ma->args.append(arguments[2].str());
				ma->args.append(PFLAG);
				ma->args.append(0.0f);
				ma->args.append(arguments[1].boolValue());
				ma->args.append(arguments[0].intValue());
				ma->args.append(0.0f);
				ma->args.append(0.0f);
				ma->args.append(0.0f);
				ma->action = ADDTRIGGERORDER;
				Enqueue(ma);
			}
			return true;
		}
	case RGF_SM_DELFLAGORDER:
		{
			PARMCHECK(1);
			sprintf(param0, "PawnFlag%d", arguments[0].intValue());

			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(DelTriggerOrder);
			ma->args.append(param0);
			ma->action = DELTRIGGERORDER;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_CHANGEMATERIAL:
		{
			PARMCHECK(1);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(ChangeMaterial);
			ma->args = arguments;
			ma->action = CHANGEMATERIAL;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_CHANGEWEAPONMATERIAL:
		{
			PARMCHECK(1);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(ChangeWeaponMaterial);
			ma->args = arguments;
			ma->action = CHANGEWEAPONMATERIAL;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_ADDTIMERORDER:
		{
			PARMCHECK(3);
			iparam = arguments[0].intValue();
			sprintf(param0, "PawnTimer%d", iparam);

			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(AddTriggerOrder);
			ma->args.append(param0);
			ma->args.append(arguments[2].str());
			ma->args.append(PTIMER);
			ma->args.append(0.0f);
			ma->args.append(false);
			ma->args.append(iparam);
			ma->args.append(arguments[1].floatValue());
			ma->args.append(0.0f);
			ma->args.append(0.0f);
			ma->action = ADDTRIGGERORDER;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_DELTIMERORDER:
		{
			PARMCHECK(1);
			sprintf(param0, "PawnTimer%d", arguments[0].intValue());

			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(DelTriggerOrder);
			ma->args.append(param0);
			ma->action = DELTRIGGERORDER;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_SPEAK:
		{
			PARMCHECK(1);
			if(sxConversationManager::GetSingletonPtr()->IsConversing())
			{
				skRValueArray setIconArgs;
				setIconArgs.append(skString(m_Icon.c_str()));
				sxConversationManager::M_SetIcon(sxConversationManager::GetSingletonPtr(), setIconArgs, returnValue, ctxt);
				skRValue pos(new RFSX::sxVector(GetSpeakBonePosition()), true);
				arguments.append(pos);
				sxConversationManager::M_Speak(sxConversationManager::GetSingletonPtr(), arguments, returnValue, ctxt);
				return true;
			}

			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(Speak);
			ma->args = arguments;
			ma->action = SPEAK;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_ADDRANDOMSOUND:
		{
			PARMCHECK(3);
			iparam = arguments[0].intValue();
			sprintf(param0, "RandomSound%d", iparam);

			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(AddTriggerOrder);
			ma->args.append(param0);
			ma->args.append(arguments[3].str());
			ma->args.append(PSOUND);
			ma->args.append(0.0f);
			ma->args.append(false);
			ma->args.append(iparam);
			ma->args.append(0.0f);
			ma->args.append(arguments[1].floatValue());
			ma->args.append(arguments[2].floatValue());
			ma->action = ADDTRIGGERORDER;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_DELRANDOMSOUND:
		{
			PARMCHECK(1);
			sprintf(param0, "RandomSound%d", arguments[0].intValue());

			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(DelTriggerOrder);
			ma->args.append(param0);
			ma->action = DELTRIGGERORDER;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_ADDDISTANCEORDER:
		{
			PARMCHECK(3);
			iparam = arguments[0].intValue();
			sprintf(param0, "PlayerDist%d", iparam);

			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(AddTriggerOrder);
			ma->args.append(param0);
			ma->args.append(arguments[2].str());
			ma->args.append(PDIST);
			ma->args.append(0.0f);
			ma->args.append(false);
			ma->args.append(iparam);
			ma->args.append(arguments[1].floatValue());
			ma->args.append(0.0f);
			ma->args.append(0.0f);
			ma->action = ADDTRIGGERORDER;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_DELDISTANCEORDER:
		{
			PARMCHECK(1);
			sprintf(param0, "PlayerDist%d", arguments[0].intValue());

			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(DelTriggerOrder);
			ma->args.append(param0);
			ma->action = DELTRIGGERORDER;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_ADDCOLLISIONORDER:
		{
			PARMCHECK(1);
			sprintf(param0, "PlayerColl%d", 1);

			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(AddTriggerOrder);
			ma->args.append(param0);
			ma->args.append(arguments[0].str());
			ma->args.append(PCOLLIDE);
			ma->args.append(0.0f);
			ma->args.append(false);
			ma->args.append(1);
			ma->args.append(0.0f);
			ma->args.append(0.0f);
			ma->args.append(0.0f);
			ma->action = ADDTRIGGERORDER;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_DELCOLLISIONORDER:
		{
			sprintf(param0, "PlayerColl%d", 1);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(DelTriggerOrder);
			ma->args.append(param0);
			ma->action = DELTRIGGERORDER;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_ANIMATESTOP:
		{
			PARMCHECK(3);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(AnimateStop);
			ma->args = arguments;
			ma->action = STOPANIMATION;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_ATTRIBUTEORDER:
		{
			PARMCHECK(3);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(AttributeOrder);
			ma->args = arguments;
			ma->action = ATTRIBUTEORDER;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_REMOVE:
		{
			PARMCHECK(1);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(Remove);
			ma->args = arguments;
			ma->action = REMOVE;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_SETKEYPAUSE:
		{
			PARMCHECK(1);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(SetKeyPause);
			ma->args = arguments;
			ma->action = SETKEYPAUSE;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_SETNOCOLLISION:
		{
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(SetNoCollision);
			ma->action = SETNOCOLLISION;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_SETCOLLISION:
		{
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(SetCollision);
			ma->action = SETCOLLISION;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_ALLOWUSEKEY:
		{
			PARMCHECK(1);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(AllowUseKey);
			ma->args = arguments;
			ma->action = ALLOWUSE;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_SETHUDDRAW:
		{
			PARMCHECK(1);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(SetHudDraw);
			ma->args = arguments;
			ma->action = SETHUDDRAW;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_HIDEFROMRADAR:
		{
			PARMCHECK(1);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(HideFromRadar);
			ma->args = arguments;
			ma->action = HIDEFROMRADAR;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_CONVERSATION:
		{
			// Conversation();
			// Conversation(OrderName);
			// Allows to define a different start-order to be used for this conversation
			if(arguments.entries() > 0)
				m_Conversation->SetOrder(arguments[0].str().c_str());

			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(Conversation);
			ma->action = CONVERSATION;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_FADEIN:
		{
			PARMCHECK(2);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(FadeIn);
			ma->args = arguments;
			ma->action = FADEIN;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_FADEOUT:
		{
			PARMCHECK(2);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(FadeOut);
			ma->args = arguments;
			ma->action = FADEOUT;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_SETFOV:
		{
			PARMCHECK(1);

			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(SetFOV);
			ma->args = arguments;
			ma->action = FIELDOFVIEW;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_STEPHEIGHT:
		{
			PARMCHECK(1);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(StepHeight);
			ma->args = arguments;
			ma->action = STEPHEIGHT;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_SETGROUP:
		{
			PARMCHECK(1);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(SetGroup);
			ma->args = arguments;
			ma->action = SETGROUP;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_HOSTILEPLAYER:
		{
			PARMCHECK(1);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(HostilePlayer);
			ma->args = arguments;
			ma->action = HOSTILEPLAYER;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_HOSTILEDIFFERENT:
		{
			PARMCHECK(1);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(HostileDifferent);
			ma->args = arguments;
			ma->action = HOSTILEDIFF;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_HOSTILESAME:
		{
			PARMCHECK(1);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(HostileSame);
			ma->args = arguments;
			ma->action = HOSTILESAME;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_GRAVITY:
		{
			PARMCHECK(1);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(Gravity);
			ma->args = arguments;
			ma->action = GRAVITY;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_SOUNDLOOP:
		{
			PARMCHECK(1);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(SoundLoop);
			ma->args = arguments;
			ma->action = SOUNDLOOP;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_ISPUSHABLE:
		{
			PARMCHECK(1);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(IsPushable);
			ma->args = arguments;
			ma->action = ISPUSHABLE;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_ISVEHICLE:
		{
			PARMCHECK(1);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(IsVehicle);
			ma->args = arguments;
			ma->action = ISVEHICLE;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_MOVETOTARGET:
		{
			PARMCHECK(3);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(MoveToTarget);
			ma->args = arguments;
			ma->action = MOVETOTARGET;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_ROTATETOTARGET:
		{
			PARMCHECK(3);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(RotateToTarget);
			ma->args = arguments;
			ma->action = ROTATETOTARGET;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_ROTATEMOVETOTARGET:
		{
			PARMCHECK(5);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(RotateMoveToTarget);
			ma->args = arguments;
			ma->action = ROTATEMOVETOTARGET;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_LOWLEVEL:
		{
			PARMCHECK(1);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(LowLevel);
			ma->args = arguments;
			ma->action = LOWLEVEL;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_BOXWIDTH:
		{
			PARMCHECK(1);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(BoxWidth);
			ma->args = arguments;
			ma->action = BOXWIDTH;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_BOXHEIGHT:
		{
			PARMCHECK(1);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(BoxHeight);
			ma->args = arguments;
			ma->action = BOXHEIGHT;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_SCALE:
		{
			// USAGE:	Scale(Scale)
			//			Scale(ScaleX, ScaleY, ScaleZ)
			PARMCHECK(1);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(Scale);
			ma->args = arguments;
			ma->action = SCALE;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_SETSCALE:
		{
			PARMCHECK(1);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(SetScale);
			ma->args = arguments;
			ma->action = SETSCALE;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_FIREPROJECTILE:
		{
			PARMCHECK(7);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(FireProjectile);
			ma->args = arguments;
			ma->action = FIREPROJECTILE;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_ADDEXPLOSION:
		{
			PARMCHECK(5);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(AddExplosion);
			ma->args = arguments;
			ma->action = ADDEFFECT;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_TARGETGROUP:
		{
			PARMCHECK(1);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(TargetGroup);
			ma->args = arguments;
			ma->action = TARGETGROUP;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_TESTDAMAGEORDER:
		{
			PARMCHECK(2);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(TestDamageOrder);
			ma->args = arguments;
			ma->action = TESTDAMAGEORDER;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_SETLODDISTANCE:
		{
			PARMCHECK(5);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(SetLODDistance);
			ma->args = arguments;
			ma->action = SETLODDISTANCE;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_ATTACHTOACTOR:
		{
			PARMCHECK(9);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(AttachToActor);
			ma->args = arguments;
			ma->action = ATTACHTOACTOR;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_DETACHFROMACTOR:
		{
			PARMCHECK(1);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(DetachFromActor);
			ma->args = arguments;
			ma->action = DETACHFROMACTOR;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_ATTACHBLENDACTOR:
		{
			PARMCHECK(1);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(AttachBlendActor);
			ma->args = arguments;
			ma->action = ATTACHBLENDACTOR;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_DETACHBLENDACTOR:
		{
			PARMCHECK(1);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(DetachBlendActor);
			ma->args = arguments;
			ma->action = DETACHBLENDACTOR;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_ATTACHACCESSORY:
		{
			PARMCHECK(1);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(AttachAccessory);
			ma->args = arguments;
			ma->action = ATTACHACCESSORY;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_DETACHACCESSORY:
		{
			PARMCHECK(1);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(DetachAccessory);
			ma->args = arguments;
			ma->action = DETACHACCESSORY;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_SETWEAPON:
		{
			PARMCHECK(1);
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(SetWeapon);
			ma->args = arguments;
			ma->action = SETWEAPON;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_REMOVEWEAPON:
		{
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(RemoveWeapon);
			ma->args = arguments;
			ma->action = REMOVEWEAPON;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_RANDOM:
		{
			PARMCHECK(2);
			int param1 = arguments[0].intValue();
			int param3 = arguments[1].intValue();

			if(param1 < param3)
				returnValue = EffectC_rand(param1, param3);
			else
				returnValue = EffectC_rand(param3, param1);

			return true;
		}
	case RGF_SM_DEBUG:
		{
			PARMCHECK(1);

			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(debug);
			ma->args = arguments;
			ma->action = DEBUG;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_SHOWTEXTDELAY:
		{
			// ShowTextDelay(Nr, EntityName, Animation, TextString, FontName, Time,
			//				 TextSound, ScreenOffsetX, ScreenOffsetY, Align, Alpha)
			// If EntityName is empty, use fixed screen-coordinates, If EntityName="Player", use the player as Entity
			// Align can be Left, Right or Center and defines the alignment relative to the origin

			int Nr = arguments[0].intValue();

			if(Nr < 0 || Nr >= MAXTEXT)
				return true;

			if(arguments.entries() > 10)
			{
				CCD->Level()->Pawns()->TextMessage[Nr].EntityName			= arguments[1].str().c_str();
				CCD->Level()->Pawns()->TextMessage[Nr].AnimName				= arguments[2].str().c_str();

				CCD->Level()->Pawns()->TextMessage[Nr].TextString			= arguments[3].str().c_str();
				Replace(CCD->Level()->Pawns()->TextMessage[Nr].TextString, "<Player>", CCD->Player()->GetPlayerName());
				Replace(CCD->Level()->Pawns()->TextMessage[Nr].TextString, "<CR>", "\n");

				CCD->Level()->Pawns()->TextMessage[Nr].FontName				= arguments[4].str().c_str();
				CCD->Level()->Pawns()->TextMessage[Nr].TextSound			= arguments[6].str().c_str();
				CCD->Level()->Pawns()->TextMessage[Nr].ScreenOffsetX		= arguments[7].intValue();
				CCD->Level()->Pawns()->TextMessage[Nr].ScreenOffsetY		= arguments[8].intValue();
				strncpy(&(CCD->Level()->Pawns()->TextMessage[Nr].Alignment), arguments[9].str(), 1);
				CCD->Level()->Pawns()->TextMessage[Nr].Alpha				= arguments[10].floatValue();
			}

			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(ShowTextDelay);
			ma->args = arguments;
			ma->action = SHOWTEXTDELAY;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_SHOWTEXT:
		{
			// ShowText(Nr, EntityName, Animation, TextString, FontName, TextSound, ScreenOffsetX, ScreenOffsetY, Align, Alpha)
			// If EntityName is empty, use fixed screen-coordinates, If EntityName="Player", use the player as Entity
			// Align can be Left, Right or Center and defines the alignment relative to the origin

			int Nr = arguments[0].intValue();

			if(Nr < 0 || Nr >= MAXTEXT)
				return true;

			if(arguments.entries() > 9)
			{
				CCD->Level()->Pawns()->TextMessage[Nr].EntityName			= arguments[1].str().c_str();
				CCD->Level()->Pawns()->TextMessage[Nr].AnimName				= arguments[2].str().c_str();
				CCD->Level()->Pawns()->TextMessage[Nr].TextString			= arguments[3].str().c_str();
				Replace(CCD->Level()->Pawns()->TextMessage[Nr].TextString, "<Player>", CCD->Player()->GetPlayerName());
				Replace(CCD->Level()->Pawns()->TextMessage[Nr].TextString, "<CR>", "\n");

				CCD->Level()->Pawns()->TextMessage[Nr].FontName				= arguments[4].str().c_str();
				CCD->Level()->Pawns()->TextMessage[Nr].TextSound			= arguments[5].str().c_str();
				CCD->Level()->Pawns()->TextMessage[Nr].ScreenOffsetX		= arguments[6].intValue();
				CCD->Level()->Pawns()->TextMessage[Nr].ScreenOffsetY		= arguments[7].intValue();
				strncpy(&(CCD->Level()->Pawns()->TextMessage[Nr].Alignment), arguments[8].str(), 1);
				CCD->Level()->Pawns()->TextMessage[Nr].Alpha				= arguments[9].floatValue();
			}

			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(ShowText);
			ma->args = arguments;
			ma->action = SHOWTEXT;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_REMOVETEXT:
		{
			int Nr = arguments[0].intValue();

			if(Nr < 0 || Nr >= MAXTEXT)
				return true;

			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(RemoveText);
			ma->args = arguments;
			ma->action = REMOVETEXT;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_CONCAT:
		{
			// deprecated

			// Concatenate string elements to 1 new string
			PARMCHECK(2);

			strcpy(param0, arguments[0].str());
			strcat(param0, arguments[1].str());

			if(arguments.entries() > 2)
			{
				for(unsigned int i=2; i<arguments.entries(); ++i)
					strcat(param0, arguments[i].str());
			}

			returnValue = skString(param0);
			return true;
		}
	case RGF_SM_GETATTRIBUTE:
		{
			PARMCHECK(1);

			CPersistentAttributes *theInv;

			if(arguments.entries() > 1)
			{
				strcpy(param0, arguments[1].str());

				if(!stricmp(param0, "Player"))
				{
					// deprecated
					theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
				}
				else
				{
					theInv = CCD->ActorManager()->Inventory(CCD->ActorManager()->GetByEntityName(param0));
				}
			}
			else
			{
				theInv = CCD->ActorManager()->Inventory(m_Actor);
			}

			returnValue = theInv->Value(arguments[0].str().c_str());
			return true;
		}
	case RGF_SM_MODIFYATTRIBUTE:
		{
			PARMCHECK(2);

			CPersistentAttributes *theInv;

			if(arguments.entries() > 2)
			{
				strcpy(param0, arguments[2].str());

				if(!stricmp(param0, "Player"))
				{
					// deprecated
					theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
					returnValue = theInv->Modify(arguments[0].str().c_str(), arguments[1].intValue());
					sxInventory::GetSingletonPtr()->UpdateItem(arguments[0].str().c_str(), true);
					return true;
				}
				else
				{
					theInv = CCD->ActorManager()->Inventory(CCD->ActorManager()->GetByEntityName(param0));
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
				strcpy(param0, arguments[2].str());

				if(!stricmp(param0, "Player"))
				{
					// deprecated
					theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
					returnValue = theInv->Set(arguments[0].str().c_str(), arguments[1].intValue());
					sxInventory::GetSingletonPtr()->UpdateItem(arguments[0].str().c_str(), true);
					return true;
				}
				else
				{
					theInv = CCD->ActorManager()->Inventory(CCD->ActorManager()->GetByEntityName(param0));
				}
			}
			else
			{
				theInv = CCD->ActorManager()->Inventory(m_Actor);
			}

			returnValue = theInv->Set(arguments[0].str().c_str(), arguments[1].intValue());
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
				char entity[128];
				strcpy(entity, arguments[arguments.entries()-1].str());

				if(!stricmp(entity, "Player")) // deprecated
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
					theInv = CCD->ActorManager()->Inventory(CCD->ActorManager()->GetByEntityName(entity));
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
				strcpy(param0, arguments[3].str());

				if(!stricmp(param0, "Player"))
				{
					// deprecated
					theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
				}
				else
				{
					theInv = CCD->ActorManager()->Inventory(CCD->ActorManager()->GetByEntityName(param0));
				}
			}
			else
			{
				theInv = CCD->ActorManager()->Inventory(m_Actor);
			}

			theInv->SetValueLimits(arguments[0].str().c_str(), arguments[1].intValue(), arguments[2].intValue());

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
	case RGF_SM_GETFLAG:
		{
			// GetFlag
			iparam = arguments[0].intValue();

			if(iparam >= 0 && iparam < MAXFLAGS)
				returnValue = CCD->Level()->Pawns()->GetPawnFlag(iparam);

			return true;
		}
	case RGF_SM_MOUSECONTROLLEDPLAYER:
		{
			// deprecated
			// MouseControlledPlayer(true/false)
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(MouseControlledPlayer);
			ma->args = arguments;
			ma->action = MOUSECONTROLLEDPLAYER;
			Enqueue(ma);
			return true;
		}
	case RGF_SM_GETEVENTSTATE:
		{
			// GetEventState
			PARMCHECK(1);
			returnValue = GetTriggerState(arguments[0].str().c_str());
			return true;
		}
	case RGF_SM_ENDSCRIPT:
		{
			SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
			ma->method = Q_P_FUNC(EndScript);
			ma->args = arguments;
			ma->action = ENDSCRIPT;
			Enqueue(ma);
			return true;
		}
	default:
		{
			return skScriptedExecutable::method(methodName, arguments, returnValue, ctxt);
		}
	}
}


/* ------------------------------------------------------------------------------------ */
//	Push
/* ------------------------------------------------------------------------------------ */
void ScriptedObject::Push()
{
#if USE_METHOD_QUEUE_STACK
	if(m_MethodQueueStack.empty())
	{
		m_MethodQueueStack.push(new std::queue<SX_METHOD_ARGS*>());
		m_MethodQueueVariablesStack.push(new MethodQueueVariables());

		m_MethodQueueVariablesStack.top()->Order		= Order;
		m_MethodQueueVariablesStack.top()->Point		= m_Point;
		m_MethodQueueVariablesStack.top()->NextOrder	= m_NextOrder;
		m_MethodQueueVariablesStack.top()->DistOrder	= m_DistOrder;

		m_MethodQueueVariablesStack.top()->CurrentPoint	= m_CurrentPoint;
		m_MethodQueueVariablesStack.top()->ActionActive	= m_ActionActive;
		m_MethodQueueVariablesStack.top()->ValidPoint	= m_ValidPoint;
		m_MethodQueueVariablesStack.top()->Vec2Point	= m_Vec2Point;
		m_MethodQueueVariablesStack.top()->DistActive	= m_DistActive;
		m_MethodQueueVariablesStack.top()->MinDistance	= m_MinDistance;
	}
#else
	ActionStack	*pool;

	pool = GE_RAM_ALLOCATE_STRUCT(ActionStack);
	memset(pool, 0x0, sizeof(ActionStack));
	pool->next = m_Stack;
	m_Stack = pool;

	if(pool->next)
		pool->next->prev = pool;

	pool->Top		= Top;
	pool->Bottom	= Bottom;
	pool->Index		= m_CurrentAction;

	pool->Order		= Order;
	pool->Point		= m_Point;
	pool->NextOrder	= m_NextOrder;
	pool->DistOrder	= m_DistOrder;

	pool->CurrentPoint	= m_CurrentPoint;
	pool->ActionActive	= m_ActionActive;
	pool->ValidPoint	= m_ValidPoint;
	pool->Vec2Point		= m_Vec2Point;
	pool->DistActive	= m_DistActive;
	pool->MinDistance	= m_MinDistance;

	Top		= NULL;
	Bottom	= NULL;
	m_CurrentAction	= NULL;
#endif
}

/* ------------------------------------------------------------------------------------ */
//	Pop
/* ------------------------------------------------------------------------------------ */
void ScriptedObject::Pop()
{
#if USE_METHOD_QUEUE_STACK
	if(!m_MethodQueueStack.empty())
	{
		while(!m_MethodQueueStack.top()->empty())
		{
			delete m_MethodQueueStack.top()->front();
			m_MethodQueueStack.top()->pop();
		}

		delete m_MethodQueueStack.top();
		m_MethodQueueStack.pop();

		Order		= m_MethodQueueVariablesStack.top()->Order;
		m_Point		= m_MethodQueueVariablesStack.top()->Point;
		m_NextOrder	= m_MethodQueueVariablesStack.top()->NextOrder;
		m_DistOrder	= m_MethodQueueVariablesStack.top()->DistOrder;

		m_CurrentPoint	= m_MethodQueueVariablesStack.top()->CurrentPoint;
		m_ActionActive	= m_MethodQueueVariablesStack.top()->ActionActive;
		m_ValidPoint	= m_MethodQueueVariablesStack.top()->ValidPoint;
		m_Vec2Point		= m_MethodQueueVariablesStack.top()->Vec2Point;
		m_DistActive	= m_MethodQueueVariablesStack.top()->DistActive;
		m_MinDistance	= m_MethodQueueVariablesStack.top()->MinDistance;

		delete m_MethodQueueVariablesStack.top();
		m_MethodQueueVariablesStack.pop();
	}
#else
	if(m_Stack)
	{
		ActionList *pool, *temp;
		pool = Bottom;

		while(pool != NULL)
		{
			temp = pool->next;
			geRam_Free(pool);
			pool = temp;
		}

		Bottom	= m_Stack->Bottom;
		Top		= m_Stack->Top;
		m_CurrentAction	= m_Stack->Index;

		Order		= m_Stack->Order;
		m_Point		= m_Stack->Point;
		m_NextOrder	= m_Stack->NextOrder;
		m_DistOrder	= m_Stack->DistOrder;

		m_CurrentPoint	= m_Stack->CurrentPoint;
		m_ActionActive	= m_Stack->ActionActive;
		m_ValidPoint	= m_Stack->ValidPoint;
		m_Vec2Point		= m_Stack->Vec2Point;
		m_DistActive	= m_Stack->DistActive;
		m_MinDistance	= m_Stack->MinDistance;

		ActionStack *stemp;
		stemp = m_Stack->next;
		geRam_Free(m_Stack);
		m_Stack = stemp;

		if(m_Stack)
			m_Stack->prev = NULL;
	}
#endif
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
Q_METHOD_IMPL(MoveToPoint)
{
	if(!m_ValidPoint || !m_Actor)
		return true;

	if(MoveToPoint(timeElapsed, args))
	{
		return true;
	}
	else
	{
		if(m_AvoidMode && !m_AvoidOrder.empty())
		{
			Push();
			Order = m_AvoidOrder;
			m_RunOrder = true;
			m_ActionActive = false;
			m_DistActive = false;
			return true;
		}
	}

	return false;
}

Q_METHOD_IMPL(RotateToPoint)
{
	if(!m_ValidPoint || m_FacePlayer || !m_Actor)
		return true;

	if(m_StartAction)
	{
		if(args.entries() > 3)
		{
			if(args[3].str() != "")
			{
				m_SoundID = PlaySound(args[3].str().c_str(), m_SoundID);
			}
		}
	}

	return RotateToPoint(
		timeElapsed,
		args[0].str().c_str(),
		args[1].floatValue(),
		args[2].boolValue());
}

Q_METHOD_IMPL(NewOrder)
{
	Order = args[0].str().c_str();
	ClearActionList();

	return true;
}

Q_METHOD_IMPL(NextOrder)
{
	if(!m_ValidPoint)
		return true;

	if(!m_NextOrder.empty())
	{
		Order = m_NextOrder;
		m_RunOrder = true;

#if USE_METHOD_QUEUE_STACK
		DequeueTop();
#else
		ActionList *pool, *temp;
		pool = Bottom;

		while(pool != NULL)
		{
			temp = pool->next;
			geRam_Free(pool);
			pool = temp;
		}

		Top = NULL;
		Bottom = NULL;
		m_CurrentAction = NULL;
#endif
	}

	return true;
}

// RotateToAlign(string animation, float rotationSpeed, bool xyRotation, string sound);
Q_METHOD_IMPL(RotateToAlign)
{
	if(!m_ValidPoint || m_FacePlayer || !m_Actor)
		return true;

	geVec3d Pos, Orient;

	if(m_StartAction)
	{
		if(args[0].str() != "")
		{
			CCD->ActorManager()->SetHoldAtEnd(m_Actor, false);
			CCD->ActorManager()->SetMotion(m_Actor, args[0].str().c_str());
		}

		if(args.entries() > 3)
		{
			if(args[3].str() != "")
			{
				m_SoundID = PlaySound(args[3].str().c_str(), m_SoundID);
			}
		}

		m_StartAction = false;

		const char *EntityType = CCD->EntityRegistry()->GetEntityType(m_Point.c_str());

		if(EntityType)
		{
			if(!stricmp(EntityType, "ScriptPoint"))
			{
				ScriptPoint *pProxy;
				CCD->Level()->ScriptPoints()->LocateEntity(m_Point.c_str(), reinterpret_cast<void**>(&pProxy));

				Orient.Z = GE_PIOVER180 * (pProxy->Angle.Z);
				Orient.X = GE_PIOVER180 * (pProxy->Angle.X);
				Orient.Y = GE_PIOVER180 * (pProxy->Angle.Y-90.0f);

				CCD->ActorManager()->GetPosition(m_Actor, &Pos);

				geXForm3d Xf;

				geXForm3d_SetZRotation(&Xf, Orient.Z);
				geXForm3d_RotateX(&Xf, Orient.X);
				geXForm3d_RotateY(&Xf, Orient.Y);

				geXForm3d_GetIn(&Xf, &Orient);

				geVec3d_Add(&Pos, &Orient, &m_TempPoint);
			}
		}
	}

	CCD->ActorManager()->GetPosition(m_Actor, &Pos);
	geVec3d_Subtract(&m_TempPoint, &Pos, &Orient);
	float l = geVec3d_Length(&Orient);
	float RotateAmt = 0.0f;
	float TiltAmt = 0.0f;

	if(l > 0.0f)
	{
		float x = Orient.X;

		Orient.X = GE_PIOVER2 - acos(Orient.Y / l);
		Orient.Y = atan2(x, Orient.Z) + GE_PI;
		CCD->ActorManager()->GetRotate(m_Actor, &Pos);

		while(Pos.Y < 0.0f)		{ Pos.Y += GE_2PI; }
		while(Pos.Y >= GE_2PI)	{ Pos.Y -= GE_2PI; }

		bool RotateLeft = false;
		RotateAmt = Pos.Y - Orient.Y;

		if(RotateAmt < 0.0f)
		{
			RotateAmt = -RotateAmt;
			RotateLeft = true;
		}

		if(RotateAmt > 0.f)
		{
			if(RotateAmt > GE_PI)
			{
				RotateAmt = GE_2PI - RotateAmt;

				if(RotateLeft)	RotateLeft = false;
				else			RotateLeft = true;
			}

			RotateAmt *= GE_180OVERPI;
			float amount = args[1].floatValue() * (timeElapsed * 0.001f);

			if(amount>RotateAmt)
			{
				amount = RotateAmt;
			}

			RotateAmt -= amount;

			if(RotateLeft)	CCD->ActorManager()->TurnRight(m_Actor, GE_PIOVER180 * amount);
			else			CCD->ActorManager()->TurnLeft (m_Actor, GE_PIOVER180 * amount);
		}

		if(args[2].boolValue())
		{
			while(Pos.X < 0.0f)		{ Pos.X += GE_2PI; }
			while(Pos.X >= GE_2PI)	{ Pos.X -= GE_2PI; }

			bool TiltUp = false;
			TiltAmt = Pos.X - Orient.X;

			if(TiltAmt < 0.0f)
			{
				TiltAmt = -TiltAmt;
				TiltUp = true;
			}

			if(TiltAmt > 0.0f)
			{
				if(TiltAmt > GE_PI)
				{
					TiltAmt = GE_2PI - TiltAmt;

					if(TiltUp)	TiltUp = false;
					else		TiltUp = true;
				}

				TiltAmt *= GE_180OVERPI;
				float amount = args[1].floatValue() * (timeElapsed * 0.001f);

				if(amount > TiltAmt)
				{
					amount = TiltAmt;
				}

				TiltAmt -= amount;

				if(TiltUp)	CCD->ActorManager()->TiltUp  (m_Actor, GE_PIOVER180 * amount);
				else		CCD->ActorManager()->TiltDown(m_Actor, GE_PIOVER180 * amount);
			}
		}

		if(RotateAmt <= 0.0f && TiltAmt <= 0.0f)
			return true;

		return false;
	}

	return true;
}

Q_METHOD_IMPL(NextPoint)
{
	if(!m_ValidPoint)
		return true;

	const char *EntityType = CCD->EntityRegistry()->GetEntityType(m_Point.c_str());

	if(EntityType)
	{
		if(!stricmp(EntityType, "ScriptPoint"))
		{
			ScriptPoint *pProxy;
			CCD->Level()->ScriptPoints()->LocateEntity(m_Point.c_str(), reinterpret_cast<void**>(&pProxy));
			m_ValidPoint = false;

			if(!EffectC_IsStringNull(pProxy->NextPoint))
			{
				m_Point = pProxy->NextPoint;
				m_NextOrder.clear();

				if(!m_Point.empty())
				{
					const char *EntityType = CCD->EntityRegistry()->GetEntityType(m_Point.c_str());

					if(EntityType)
					{
						if(!stricmp(EntityType, "ScriptPoint"))
						{
							ScriptPoint *pProxy;
							CCD->Level()->ScriptPoints()->LocateEntity(m_Point.c_str(), reinterpret_cast<void**>(&pProxy));

							if(!EffectC_IsStringNull(pProxy->NextOrder))
								m_NextOrder = pProxy->NextOrder;
							m_CurrentPoint = pProxy->origin;
							m_ValidPoint = true;
						}
					}
				}
			}
		}
	}

	return true;
}

Q_METHOD_IMPL(Delay)
{
	if(m_StartAction)
	{
		if(args[0].str().length() > 0 && m_Actor)
		{
			CCD->ActorManager()->SetHoldAtEnd(m_Actor, false);
			CCD->ActorManager()->SetMotion(m_Actor, args[0].str().c_str());
		}

		if(args.entries() > 2)
		{
			if(args[2].str() != "")
			{
				m_SoundID = PlaySound(args[2].str().c_str(), m_SoundID);
			}
		}

		m_StartAction = false;
		m_Time = 0.0f;
	}

	m_Time += (timeElapsed * 0.001f);

	if(m_Time >= args[1].floatValue())
	{
		m_Time = 0.0f;
		return true;
	}

	return false;
}

Q_METHOD_IMPL(PlayAnimation)
{
	if(!m_Actor)
		return true;

	if(m_StartAction)
	{
		if(args[0].str() != "")
			CCD->ActorManager()->SetMotion(m_Actor, args[0].str().c_str());

		if(args.entries() > 2)
		{
			if(args[2].str() != "")
			{
				m_SoundID = PlaySound(args[2].str().c_str(), m_SoundID);
			}
		}

		m_StartAction = false;

		if(args[1].boolValue())
			CCD->ActorManager()->SetHoldAtEnd(m_Actor, true);
		else
		{
			CCD->ActorManager()->SetHoldAtEnd(m_Actor, false);
			return true;
		}
	}

	if(args[1].boolValue())
	{
		// check if animation is at end
		if(CCD->ActorManager()->EndAnimation(m_Actor))
		{
			CCD->ActorManager()->SetHoldAtEnd(m_Actor, false);
			return true;
		}
	}

	return false;
}

Q_METHOD_IMPL(BlendToAnimation)
{
	if(!m_Actor)
		return true;

	if(m_StartAction)
	{
		m_StartAction = false;
		if(args[0].str() != "")
		{
			if(args[1].floatValue() > 0.0f)
			{
				CCD->ActorManager()->SetTransitionMotion(m_Actor, args[0].str().c_str(), args[1].floatValue(), "");
				CCD->ActorManager()->SetNextMotion(m_Actor, args[0].str().c_str());
			}
			else
			{
				CCD->ActorManager()->SetMotion(m_Actor, args[0].str().c_str());
			}
		}

		if(args.entries() > 3)
		{
			if(args[3].str() != "")
			{
				m_SoundID = PlaySound(args[3].str().c_str(), m_SoundID);
			}
		}

		if(args[2].boolValue())
		{
			CCD->ActorManager()->SetHoldAtEnd(m_Actor, true);
		}
		else
		{
			CCD->ActorManager()->SetHoldAtEnd(m_Actor, false);
			return true;
		}
	}

	if(args[2].boolValue())
	{
		// check if animation is at end
		if(CCD->ActorManager()->EndAnimation(m_Actor))
		{
			CCD->ActorManager()->SetHoldAtEnd(m_Actor, false);
			return true;
		}
	}

	return false;
}

Q_METHOD_IMPL(LoopAnimation)
{
	if(!m_Actor)
		return true;

	if(m_StartAction)
	{
		if(args[0].str() != "")
		{
			CCD->ActorManager()->SetHoldAtEnd(m_Actor, false);
			CCD->ActorManager()->SetMotion(m_Actor, args[0].str().c_str());
		}

		if(args.entries() > 2)
		{
			if(args[2].str() != "")
			{
				m_SoundID = PlaySound(args[2].str().c_str(), m_SoundID);
			}
		}

		m_StartAction = false;
		m_Time = 0.0f;
	}

	m_Time += (timeElapsed * 0.001f);

	if(m_Time >= args[1].floatValue())
	{
		m_Time = 0.0f;
		return true;
	}

	return false;
}

Q_METHOD_IMPL(Rotate)
{
	if(m_FacePlayer || !m_Actor)
		return true;

	if(m_StartAction)
	{
		if(args.entries() > 5)
		{
			if(args[5].str() != "")
			{
				m_SoundID = PlaySound(args[5].str().c_str(), m_SoundID);
			}
		}
	}

	geVec3d angle;
	geVec3d_Set(&angle, args[2].floatValue(), args[3].floatValue(), args[4].floatValue());
	bool ret = Rotate(timeElapsed, args[0].str().c_str(), args[1].floatValue(), &angle);
	args[2] = angle.X;
	args[3] = angle.Y;
	args[4] = angle.Z;
	return ret;
}

Q_METHOD_IMPL(RotateMoveToPoint)
{
	if(!m_ValidPoint || m_FacePlayer || !m_Actor)
		return true;

	if(m_StartAction)
	{
		if(args.entries() > 4)
		{
			if(args[4].str() != "")
			{
				m_SoundID = PlaySound(args[4].str().c_str(), m_SoundID);
			}
		}
	}

	return RotateMoveToPoint(
		timeElapsed,
		args[0].str().c_str(),
		args[1].floatValue(),
		args[2].floatValue(),
		args[3].boolValue());
}

Q_METHOD_IMPL(RotateMove)
{
	if(m_FacePlayer || !m_Actor)
		return true;

	if(m_StartAction)
	{
		if(args.entries() > 6)
		{
			if(args[6].str() != "")
			{
				m_SoundID = PlaySound(args[6].str().c_str(), m_SoundID);
			}
		}
	}

	geVec3d angle;
	geVec3d_Set(&angle, args[3].floatValue(), args[4].floatValue(), args[5].floatValue());

	if(Rotate(timeElapsed, args[0].str().c_str(), args[1].floatValue(), &angle))
		return true;

	args[3] = angle.X;
	args[4] = angle.Y;
	args[5] = angle.Z;

	geVec3d Pos, NewPos, In;
	CCD->ActorManager()->GetPosition(m_Actor, &Pos);

	float distance = args[2].floatValue() * (timeElapsed * 0.001f);
	CCD->ActorManager()->InVector(m_Actor, &In);
	geVec3d_AddScaled(&Pos, &In, distance, &NewPos);

	bool result = CCD->ActorManager()->ValidateMove(Pos, NewPos, m_Actor, false);
	return false;
}

Q_METHOD_IMPL(NewPath)
{
	const char *EntityType = CCD->EntityRegistry()->GetEntityType(args[0].str().c_str());

	if(EntityType)
	{
		if(!stricmp(EntityType, "ScriptPoint"))
		{
			ScriptPoint *pProxy;
			CCD->Level()->ScriptPoints()->LocateEntity(args[0].str().c_str(), reinterpret_cast<void**>(&pProxy));
			Order.clear();
			m_Point = args[0].str().c_str();

			if(!EffectC_IsStringNull(pProxy->NextOrder))
			{
				Order = pProxy->NextOrder;
				m_RunOrder = true;

#if USE_METHOD_QUEUE_STACK
				DequeueTop();
#else
				ActionList *pool, *temp;
				pool = Bottom;

				while(pool != NULL)
				{
					temp = pool->next;
					geRam_Free(pool);
					pool = temp;
				}

				Top = NULL;
				Bottom = NULL;
				m_CurrentAction = NULL;
#endif
			}

			m_CurrentPoint = pProxy->origin;
			m_ValidPoint = true;
		}
	}

	return true;
}

Q_METHOD_IMPL(RestartOrder)
{
	ClearActionList();
	// Note: despite the fact that the action is finished
	// we have to return false here or we will cause an endless loop
	// m_RunOrder set to true in ClearActionList() will do the rest
	return false;
}

Q_METHOD_IMPL(PlayerDistOrder)
{
	m_MinDistance = args[0].floatValue();

	if(m_MinDistance != 0.0f)
	{
		m_DistActive = true;
		m_DistOrder = args[1].str().c_str();
	}
	else
	{
		m_DistActive = false;
	}

	return true;
}

Q_METHOD_IMPL(AudibleRadius)
{
	m_AudibleRadius = args[0].floatValue();

	return true;
}

Q_METHOD_IMPL(AddPainOrder)
{
	m_PainActive = true;
	m_PainOrder = args[0].str().c_str();
	m_PainPercent = args[1].intValue();
	CPersistentAttributes *attributes = CCD->ActorManager()->Inventory(m_Actor);
	m_OldAttributeAmount = attributes->Value(m_Attribute);

	return true;
}

Q_METHOD_IMPL(FindTargetOrder)
{
	m_TargetDistance = args[0].floatValue();

	if(m_TargetDistance != 0.0f)
	{
		m_TargetDisable = false;
		m_TargetFind = true;
		m_TargetOrder = args[1].str().c_str();
		m_TargetAttr = args[2].str().c_str();
	}
	else
	{
		m_TargetFind = false;
	}

	return true;
}

Q_METHOD_IMPL(FindPointOrder)
{
	m_PointFind = true;
	m_PointOrder = args[0].str().c_str();

	if(m_PointOrder.empty())
		m_PointFind = false;

	return true;
}

Q_METHOD_IMPL(NewPoint)
{
	if(args[0].str() != "")
	{
		const char *EntityType = CCD->EntityRegistry()->GetEntityType(args[0].str().c_str());

		if(EntityType)
		{
			if(!stricmp(EntityType, "ScriptPoint"))
			{
				m_Point = args[0].str().c_str();
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

	return true;
}

Q_METHOD_IMPL(MoveForward)
{
	m_Direction = RGF_K_FORWARD;

	if(m_StartAction)
	{
		if(args.entries() > 3)
		{
			if(args[3].str() != "")
			{
				m_SoundID = PlaySound(args[3].str().c_str(), m_SoundID);
			}
		}
	}

	return Move(timeElapsed, args[0].str().c_str(), args[1].floatValue(), args[2].floatValue());
}

Q_METHOD_IMPL(MoveBackward)
{
	m_Direction = RGF_K_BACKWARD;

	if(m_StartAction)
	{
		if(args.entries() > 3)
		{
			if(args[3].str() != "")
			{
				m_SoundID = PlaySound(args[3].str().c_str(), m_SoundID);
			}
		}
	}

	return Move(timeElapsed, args[0].str().c_str(), args[1].floatValue(), args[2].floatValue());
}

Q_METHOD_IMPL(MoveLeft)
{
	m_Direction = RGF_K_LEFT;

	if(m_StartAction)
	{
		if(args.entries() > 3)
		{
			if(args[3].str() != "")
			{
				m_SoundID = PlaySound(args[3].str().c_str(), m_SoundID);
			}
		}
	}

	return Move(timeElapsed, args[0].str().c_str(), args[1].floatValue(), args[2].floatValue());
}

Q_METHOD_IMPL(MoveRight)
{
	m_Direction = RGF_K_RIGHT;

	if(m_StartAction)
	{
		if(args.entries() > 3)
		{
			if(args[3].str() != "")
			{
				m_SoundID = PlaySound(args[3].str().c_str(), m_SoundID);
			}
		}
	}

	return Move(timeElapsed, args[0].str().c_str(), args[1].floatValue(), args[2].floatValue());
}

Q_METHOD_IMPL(Move)
{
	m_Direction = 0;

	if(m_StartAction)
	{
		if(args.entries() > 6)
		{
			if(args[6].str() != "")
			{
				m_SoundID = PlaySound(args[6].str().c_str(), m_SoundID);
			}
		}
	}

	return Move(timeElapsed, args[0].str().c_str(), args[1].floatValue(), args[2].floatValue(),
		args[3].floatValue(), args[4].floatValue(), args[5].floatValue());
}

Q_METHOD_IMPL(AvoidOrder)
{
	if(args[0].str() != "")
		m_AvoidOrder = args[0].str().c_str();

	return true;
}

Q_METHOD_IMPL(Return)
{
	// TODO
#if USE_METHOD_QUEUE_STACK
	if(!m_MethodQueueVariablesStack.empty())
#else
	if(m_Stack)
#endif
	{
		Pop();
		m_ActionActive = true;
		m_StartAction = true;
		m_TargetDisable = false;
	}

	return true;
}

Q_METHOD_IMPL(Align)
{
	if(m_Actor)
	{
		const char *EntityType = CCD->EntityRegistry()->GetEntityType(m_Point.c_str());

		if(EntityType)
		{
			if(!stricmp(EntityType, "ScriptPoint"))
			{
				ScriptPoint *pProxy;
				geVec3d Orient;
				CCD->Level()->ScriptPoints()->LocateEntity(m_Point.c_str(), reinterpret_cast<void**>(&pProxy));
				Orient.Z = GE_PIOVER180 * (pProxy->Angle.Z);
				Orient.X = GE_PIOVER180 * (pProxy->Angle.X);
				Orient.Y = GE_PIOVER180 * (pProxy->Angle.Y - 90.0f);
				CCD->ActorManager()->Rotate(m_Actor, Orient);
			}
		}
	}

	return true;
}

Q_METHOD_IMPL(Jump)
{
	if(!m_Actor)
		return true;

	if(m_StartAction)
	{
		if(args[0].str() != "")
		{
			CCD->ActorManager()->SetHoldAtEnd(m_Actor, false);
			CCD->ActorManager()->SetMotion(m_Actor, args[0].str().c_str());
		}

		if(args.entries() > 3)
		{
			if(args[3].str() != "")
			{
				m_SoundID = PlaySound(args[3].str().c_str(), m_SoundID);
			}
		}

		m_StartAction = false;
		geVec3d theUp;

		CCD->ActorManager()->UpVector(m_Actor, &theUp);
		CCD->ActorManager()->SetForce(m_Actor, 0, theUp, args[1].floatValue(), args[1].floatValue());

		if(args[2].boolValue())
			return true;

		return false;
	}

	if(CCD->ActorManager()->Falling(m_Actor) == GE_TRUE)
		return false;

	return true;
}

Q_METHOD_IMPL(AddTriggerOrder)
{
	if(args[1].str() != "") // order to execute
	{
		if(args[0].str() != "") // triggername
		{
			std::string TriggerName(args[0].str().c_str());

			// if trigger is already watched, update
			if(m_WatchedTriggers.find(TriggerName) != m_WatchedTriggers.end())
			{
				m_WatchedTriggers[TriggerName]->OrderName	= args[1].str().c_str();
				m_WatchedTriggers[TriggerName]->Type		= args[2].intValue();
				m_WatchedTriggers[TriggerName]->Delay		= args[3].floatValue();
				m_WatchedTriggers[TriggerName]->Flag		= args[4].boolValue();
				m_WatchedTriggers[TriggerName]->PFlg		= args[5].intValue();
				m_WatchedTriggers[TriggerName]->Time		= args[6].floatValue();
				m_WatchedTriggers[TriggerName]->Low			= args[7].floatValue();
				m_WatchedTriggers[TriggerName]->High		= args[8].floatValue();
			}
			else // create new entry
			{
				TriggerOrder *trigger = new TriggerOrder(
					args[1].str().c_str(),
					args[2].intValue(),
					args[3].floatValue(),
					args[4].boolValue(),
					args[5].intValue(),
					args[6].floatValue(),
					args[7].floatValue(),
					args[8].floatValue()
					);

				if(trigger)
				{
					m_WatchedTriggers[TriggerName] = trigger;
				}
			}
		}
	}

	return true;
}

Q_METHOD_IMPL(DelTriggerOrder)
{
	if(args[0].str() != "")
	{
		std::string TriggerName(args[0].str().c_str());
		if(m_WatchedTriggers.find(TriggerName) != m_WatchedTriggers.end())
		{
			SAFE_DELETE(m_WatchedTriggers[TriggerName]);
			m_WatchedTriggers.erase(TriggerName);
		}
	}

	return true;
}

Q_METHOD_IMPL(SetEventState)
{
	CCD->Level()->Pawns()->AddEvent(args[0].str().c_str(), args[1].boolValue());

	return true;
}

Q_METHOD_IMPL(FacePlayer)
{
	m_FacePlayer = args[0].boolValue();
	m_FaceAxis = args[1].boolValue();

	return true;
}

Q_METHOD_IMPL(RotateToPlayer)
{
	if(m_FacePlayer || !m_Actor)
		return true;

	if(m_StartAction)
	{
		if(args[0].str() != "")
		{
			CCD->ActorManager()->SetHoldAtEnd(m_Actor, false);
			CCD->ActorManager()->SetMotion(m_Actor, args[0].str().c_str());
		}

		if(args.entries() > 3)
		{
			if(args[3].str() != "")
			{
				m_SoundID = PlaySound(args[3].str().c_str(), m_SoundID);
			}
		}

		m_StartAction = false;
	}

	geVec3d Pos, Orient, Play;

	Play = CCD->Player()->Position();
	CCD->ActorManager()->GetPosition(m_Actor, &Pos);
	geVec3d_Subtract(&Play, &Pos, &Orient);

	float l = geVec3d_Length(&Orient);
	float RotateAmt = 0.0f;
	float TiltAmt = 0.0f;

	if(l > 0.0f)
	{
		float x = Orient.X;

		Orient.X = GE_PIOVER2 - acos(Orient.Y / l);
		Orient.Y = atan2(x, Orient.Z) + GE_PI;
		CCD->ActorManager()->GetRotate(m_Actor, &Pos);

		while(Pos.Y <  0.0f)	{ Pos.Y += GE_2PI; }
		while(Pos.Y >= GE_2PI)	{ Pos.Y -= GE_2PI; }

		bool RotateLeft = false;
		RotateAmt = Pos.Y - Orient.Y;

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

				if(RotateLeft)	RotateLeft = false;
				else			RotateLeft = true;
			}

			RotateAmt *= GE_180OVERPI;

			float amount = args[1].floatValue() * (timeElapsed * 0.001f);

			if(amount > RotateAmt)
			{
				amount = RotateAmt;
			}

			RotateAmt -= amount;

			if(RotateLeft)	CCD->ActorManager()->TurnRight(m_Actor, GE_PIOVER180 * amount);
			else			CCD->ActorManager()->TurnLeft (m_Actor, GE_PIOVER180 * amount);
		}

		if(args[2].boolValue())
		{
			while(Pos.X < 0.0f)		{ Pos.X += GE_2PI; }
			while(Pos.X >= GE_2PI)	{ Pos.X -= GE_2PI; }

			bool TiltUp = false;
			TiltAmt = Pos.X - Orient.X;

			if(TiltAmt < 0.0f)
			{
				TiltAmt = -TiltAmt;
				TiltUp = true;
			}

			if(TiltAmt > 0.0f)
			{
				if(TiltAmt > GE_PI)
				{
					TiltAmt = GE_2PI - TiltAmt;

					if(TiltUp)	TiltUp = false;
					else		TiltUp = true;
				}

				TiltAmt *= GE_180OVERPI;
				float amount = args[1].floatValue() * (timeElapsed * 0.001f);

				if(amount > TiltAmt)
				{
					amount = TiltAmt;
				}

				TiltAmt -= amount;

				if(TiltUp)	CCD->ActorManager()->TiltUp  (m_Actor, GE_PIOVER180 * amount);
				else		CCD->ActorManager()->TiltDown(m_Actor, GE_PIOVER180 * amount);
			}
		}

		if(RotateAmt == 0.0f && TiltAmt == 0.0f)
			return true;

		return false;
	}

	return true;
}

Q_METHOD_IMPL(RotateAroundPoint)
{
	if(!m_ValidPoint || m_FacePlayer || !m_Actor)
		return true;

	if(m_StartAction)
	{
		if(args[0].str() != "")
		{
			CCD->ActorManager()->SetHoldAtEnd(m_Actor, false);
			CCD->ActorManager()->SetMotion(m_Actor, args[0].str().c_str());
		}

		if(args.entries() > 3)
		{
			if(args[3].type() == skRValue::T_String && args[3].str() != "")
			{
				m_SoundID = PlaySound(args[3].str().c_str(), m_SoundID);
			}
		}

		m_StartAction = false;
		m_Time = 0.0f;
		m_Circle = CCD->ActorManager()->DistanceFrom(m_CurrentPoint, m_Actor);
	}

	geVec3d Pos1;
	CCD->ActorManager()->GetPosition(m_Actor, &Pos1);

	geVec3d LookRotation;
	geFloat x, l;

	geVec3d_Subtract(&m_CurrentPoint, &Pos1, &LookRotation);
	l = geVec3d_Length(&LookRotation);

	// protect from Div by Zero
	if(l > 0.0f)
	{
		x = LookRotation.X;

		LookRotation.X = -(GE_PIOVER2 - acos(LookRotation.Y / l));
		LookRotation.Y = atan2(x, LookRotation.Z) + GE_PI;
		LookRotation.Z = 0.0;	// roll is zero - always!!?

		LookRotation.X = 0.0f;
		CCD->ActorManager()->Rotate(m_Actor, LookRotation);
	}

	m_Time += (timeElapsed * 0.001f);

	if(m_Time >= args[2].floatValue())
	{
		m_Time = 0.0f;
		return true;
	}

	float distance = args[1].floatValue() * (timeElapsed * 0.001f);

	if(args[args.entries()-1].boolValue())
		distance = -distance;

	if(CCD->ActorManager()->MoveLeft(m_Actor, distance) != RGF_SUCCESS)
	{
		m_Time = 0.0f;
		return true;
	}

	float circle = CCD->ActorManager()->DistanceFrom(m_CurrentPoint, m_Actor);

	if(circle != m_Circle)
	{
		geVec3d In;
		CCD->ActorManager()->InVector(m_Actor, &In);
		float Amount = circle - m_Circle;

		if(CCD->ActorManager()->MoveForward(m_Actor, Amount) != RGF_SUCCESS)
		{
			m_Time = 0.0f;
			return true;
		}
	}

	return false;
}

Q_METHOD_IMPL(TeleportToPoint)
{
	if(args[0].str() != "")
	{
		const char *EntityType = CCD->EntityRegistry()->GetEntityType(args[0].str().c_str());

		if(EntityType)
		{
			if(!stricmp(EntityType, "ScriptPoint"))
			{
				ScriptPoint *pProxy;
				CCD->Level()->ScriptPoints()->LocateEntity(args[0].str().c_str(), reinterpret_cast<void**>(&pProxy));

				geActor *tActor;
				if(args[5].str() != "")
				{
					if(args[5].str().equalsIgnoreCase("Player")) // Teleport Player
						tActor = CCD->Player()->GetActor();
					else	// Teleport Pawn
						tActor = CCD->ActorManager()->GetByEntityName(args[5].str().c_str());
				}
				else
				{
					tActor = m_Actor; // Teleport Script-pawn
				}

				if(!tActor)
					return true;

				geVec3d Pos = pProxy->origin;
				Pos.X += args[1].floatValue();
				Pos.Y += args[2].floatValue();
				Pos.Z += args[3].floatValue();

				CCD->ActorManager()->Position(tActor, Pos);

				// rotate to match scriptpoint angle
				if(args[4].boolValue())
				{
					geVec3d RealAngle;
					geVec3d_Scale(&(pProxy->Angle), GE_PIOVER180, &RealAngle);
					RealAngle.Y -= GE_PIOVER2;

					CCD->ActorManager()->Rotate(tActor, RealAngle);

					if(	CCD->Player()->GetActor() == tActor &&
						CCD->Player()->GetViewPoint() == FIRSTPERSON)
					{
						CCD->CameraManager()->SetRotation(RealAngle);
					}
				}
			}
		}
	}

	return true;
}

Q_METHOD_IMPL(AnimationSpeed)
{
	if(m_Actor)
		CCD->ActorManager()->SetAnimationSpeed(m_Actor, m_AnimSpeed * args[0].floatValue());

	return true;
}

Q_METHOD_IMPL(SetFlag)
{
	CCD->Level()->Pawns()->SetPawnFlag(args[0].intValue(), args[1].boolValue());

	return true;
}

Q_METHOD_IMPL(ChangeMaterial)
{
	if(m_Actor)
	{
		if(args[0].str() != "")
			CCD->ActorManager()->ChangeMaterial(m_Actor, args[0].str().c_str());
	}

	return true;
}

Q_METHOD_IMPL(ChangeWeaponMaterial)
{
	if(m_WeaponActor)
	{
		if(args[0].str() != "")
			CCD->ActorManager()->ChangeMaterial(m_WeaponActor, args[0].str().c_str());
	}

	return true;
}

Q_METHOD_IMPL(AnimateStop)
{
	if(!m_Actor)
		return true;

	if(m_StartAction)
	{
		if(args[0].str() != "")
			CCD->ActorManager()->SetMotion(m_Actor, args[0].str().c_str());

		if(args.entries() > 2)
		{
			if(args[2].str() != "")
			{
				m_SoundID = PlaySound(args[2].str().c_str(), m_SoundID, GE_FALSE);
			}
		}

		m_StartAction = false;
		CCD->ActorManager()->SetHoldAtEnd(m_Actor, true);

		if(args[1].floatValue() <= 0.0f)
			return true;

		m_Time = 0.0f;
	}
	else
	{
		if(m_Time >= args[1].floatValue())
		{
			m_Time = 0.0f;
			return true;
		}
		else
		{
			if(CCD->ActorManager()->EndAnimation(m_Actor))
			{
				m_Time += (timeElapsed * 0.001f);
			}
		}
	}

	return false;
}

Q_METHOD_IMPL(AttributeOrder)
{
	if(m_Actor && args[0].str() != "" && args[2].str() != "")
	{
		m_Attribute = args[0].str().c_str();
		m_DeadOrder = args[2].str().c_str();
		CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(m_Actor);
		m_OldAttributeAmount = args[1].intValue();
		theInv->AddAndSet(m_Attribute, m_OldAttributeAmount);
		theInv->SetValueLimits(m_Attribute, 0, m_OldAttributeAmount);
	}

	return true;
}

Q_METHOD_IMPL(Remove)
{
	if(m_Actor)
	{
		CCD->ActorManager()->GetPosition(m_Actor, &m_DeadPos);
		CCD->ActorManager()->RemoveActorCheck(m_Actor);
		geActor_Destroy(&m_Actor);
		m_Actor = NULL;

		if(m_WeaponActor)
		{
			CCD->ActorManager()->RemoveActorCheck(m_WeaponActor);
			geActor_Destroy(&m_WeaponActor);
			m_WeaponActor = NULL;
		}
	}

	if(args[0].boolValue())
	{
		m_Alive = false;
	}

	return true;
}

Q_METHOD_IMPL(SetKeyPause)
{
	CCD->SetKeyPaused(args[0].boolValue());

	return true;
}

Q_METHOD_IMPL(SetNoCollision)
{
	CCD->ActorManager()->SetNoCollide(m_Actor);

	return true;
}

Q_METHOD_IMPL(SetCollision)
{
	CCD->ActorManager()->SetCollide(m_Actor);

	return true;
}

Q_METHOD_IMPL(AllowUseKey)
{
	m_UseKey = args[0].boolValue();

	return true;
}

Q_METHOD_IMPL(SetHudDraw)
{
	if(args[0].boolValue())
		CCD->HUD()->Activate();
	else
		CCD->HUD()->Deactivate();

	return true;
}

Q_METHOD_IMPL(HideFromRadar)
{
	if(m_Actor)
	{
		CCD->ActorManager()->SetHideRadar(m_Actor, args[0].boolValue());
	}

	return true;
}

Q_METHOD_IMPL(Conversation)
{
	sxConversationManager::GetSingletonPtr()->StartConversation(m_Conversation);
	return true;
}

Q_METHOD_IMPL(FadeIn)
{
	if(!m_Actor)
		return true;

	if(m_StartAction)
	{
		float alpha;

		CCD->ActorManager()->GetAlpha(m_Actor, &alpha);

		if(args[0].floatValue() > 0.0f && args[1].floatValue() > alpha)
		{
			args.append((args[1].floatValue() - alpha) / args[0].floatValue());
		}
		else
		{
			return true;
		}

		m_StartAction = false;
	}
	else
	{
		float alpha;
		CCD->ActorManager()->GetAlpha(m_Actor, &alpha);

		alpha += (args[2].floatValue() * (timeElapsed * 0.001f));

		if(alpha >= args[1].floatValue())
		{
			alpha = args[1].floatValue();
			CCD->ActorManager()->SetAlpha(m_Actor, alpha);
			return true;
		}

		CCD->ActorManager()->SetAlpha(m_Actor, alpha);
	}

	return false;
}

Q_METHOD_IMPL(FadeOut)
{
	if(!m_Actor)
		return true;

	if(m_StartAction)
	{
		float alpha;

		CCD->ActorManager()->GetAlpha(m_Actor, &alpha);

		if(args[0].floatValue() > 0.0f && args[1].floatValue() < alpha)
		{
			args.append((alpha - args[1].floatValue()) / args[0].floatValue());
		}
		else
		{
			return true;
		}

		m_StartAction = false;
	}
	else
	{
		float alpha;
		CCD->ActorManager()->GetAlpha(m_Actor, &alpha);

		alpha -= (args[2].floatValue() * (timeElapsed * 0.001f));

		if(alpha <= args[1].floatValue())
		{
			alpha = args[1].floatValue();
			CCD->ActorManager()->SetAlpha(m_Actor, alpha);
			return true;
		}

		CCD->ActorManager()->SetAlpha(m_Actor, alpha);
	}

	return false;
}

Q_METHOD_IMPL(SetFOV)
{
	// NOTE: FOV is stored as acos of the half angle for direct comparison with dot products
	// acos(x) ~ (PI/2 - x - ...) (x = angle in radians)
	// The approximation used here is not correct, but we've used it
	// for quite a while now, so leave it like this...
	// correct: FOV = GE_PIOVER2 - args[0].floatValue()*0.5f*GE_PIOVER180;

	if(args[0].floatValue() > 360.f)
	{
		m_FOV = -1.f;
	}
	else if(args[0].floatValue() < 0.f)
	{
		m_FOV = 1.f;
	}
	else
	{
		m_FOV = (180.0f - args[0].floatValue())/180.0f;
	}

	m_FOVBone.clear();

	if(args.entries() > 1)
	{
		if(args[1].str() != "")
		{
			m_FOVBone = args[1].str().c_str();
		}
	}

	return true;
}

Q_METHOD_IMPL(StepHeight)
{
	if(m_Actor)
	{
		if(args[0].floatValue() <= 0.0f)
		{
			CCD->ActorManager()->SetAutoStepUp(m_Actor, false);
		}
		else
		{
			CCD->ActorManager()->SetAutoStepUp(m_Actor, true);
			CCD->ActorManager()->SetStepHeight(m_Actor, args[0].floatValue());
		}
	}

	return true;
}

Q_METHOD_IMPL(SetGroup)
{
	m_Group = args[0].str().c_str();
	CCD->ActorManager()->SetGroup(m_Actor, m_Group);

	return true;
}

Q_METHOD_IMPL(HostilePlayer)
{
	m_HostilePlayer = args[0].boolValue();

	return true;
}

Q_METHOD_IMPL(HostileDifferent)
{
	m_HostileDiff = args[0].boolValue();

	return true;
}

Q_METHOD_IMPL(HostileSame)
{
	m_HostileSame = args[0].boolValue();

	return true;
}

Q_METHOD_IMPL(Gravity)
{
	if(m_Actor)
	{
		geVec3d Gravity = { 0.f, 0.f, 0.f };

		if(args[0].boolValue())
			Gravity = CCD->Level()->GetGravity();

		CCD->ActorManager()->SetGravity(m_Actor, Gravity);
	}

	return true;
}

Q_METHOD_IMPL(SoundLoop)
{
	m_SoundLoop = args[0].boolValue();

	return true;
}

Q_METHOD_IMPL(Speak)
{
	if(sxConversationManager::GetSingletonPtr()->IsConversing())
	{
		skRValue r_val;
		skRValueArray setIconArgs;
		setIconArgs.append(skString(m_Icon.c_str()));
		sxConversationManager::M_SetIcon(sxConversationManager::GetSingletonPtr(), setIconArgs, r_val, ScriptManager::GetContext());
		skRValue pos(new RFSX::sxVector(GetSpeakBonePosition()), true);
		args.append(pos);
		return sxConversationManager::M_Speak(sxConversationManager::GetSingletonPtr(), args, r_val, ScriptManager::GetContext());
	}
	else
	{
		// TODO display text?
		if(args.entries() > 1)
		{
			if(args[1].type() != skRValue::T_String)
				return true;

			if(args[1].str() != "")
			{
				if(m_StreamingAudio)
				{
					m_StreamingAudio->Delete();
					delete m_StreamingAudio;
				}

				m_StreamingAudio = new StreamingAudio((LPDIRECTSOUND)geSound_GetDSound());

				if(m_StreamingAudio)
				{
					char filename[_MAX_PATH];
					strcpy(filename, CCD->LanguageManager()->GetActiveLanguage()->GetDubbingDirectory().c_str());
					strcat(filename, "\\");
					strcat(filename, args[1].str().c_str());

					if(!m_StreamingAudio->Create(filename))
					{
						SAFE_DELETE(m_StreamingAudio);
					}
					else
					{
						m_StreamingAudio->Play(false);
					}
				}
			}
		}
	}

	return true;
}

Q_METHOD_IMPL(IsPushable)
{
	if(m_Actor)
	{
		m_Pushable = args[0].boolValue();
	}

	return true;
}

Q_METHOD_IMPL(IsVehicle)
{
	if(m_Actor)
	{
		if(args[0].boolValue())
			CCD->ActorManager()->SetType(m_Actor, ENTITY_VEHICLE);
		else
			CCD->ActorManager()->SetType(m_Actor, ENTITY_NPC);
	}

	return true;
}

Q_METHOD_IMPL(MoveToTarget)
{
	if(!m_TargetActor || !m_Actor)
		return true;

	if(m_StartAction)
	{
		m_SavePoint = m_CurrentPoint;
		CCD->ActorManager()->GetPosition(m_TargetActor, &m_CurrentPoint);
		m_StartAction = false;
	}

	if(MoveToPoint(timeElapsed, args))
	{
		m_CurrentPoint = m_SavePoint;
		return true;
	}
	else
	{
		if(m_AvoidMode && !m_AvoidOrder.empty())
		{
			Push();
			Order = m_AvoidOrder;
			m_RunOrder = true;
			m_ActionActive = false;
			m_DistActive = false;
			return true;
		}
	}

	return false;
}

Q_METHOD_IMPL(RotateToTarget)
{
	if(!m_TargetActor || m_FacePlayer || !m_Actor)
		return true;

	if(m_StartAction)
	{
		if(args.entries() > 3)
		{
			if(args[3].str() != "")
			{
				m_SoundID = PlaySound(args[3].str().c_str(), m_SoundID);
			}
		}

		m_SavePoint = m_CurrentPoint;
		CCD->ActorManager()->GetPosition(m_TargetActor, &m_CurrentPoint);
	}

	if(RotateToPoint(
		timeElapsed,
		args[0].str().c_str(),
		args[1].floatValue(),
		args[2].boolValue()))
	{
		m_CurrentPoint = m_SavePoint;
		return true;
	}

	return false;
}

Q_METHOD_IMPL(RotateMoveToTarget)
{
	if(!m_TargetActor || m_FacePlayer || !m_Actor)
		return true;

	if(m_StartAction)
	{
		m_SavePoint = m_CurrentPoint;
		CCD->ActorManager()->GetPosition(m_TargetActor, &m_CurrentPoint);

		if(args.entries() > 4)
		{
			if(args[4].str() != "")
			{
				m_SoundID = PlaySound(args[4].str().c_str(), m_SoundID);
			}
		}
	}

	if(RotateMoveToPoint(
		timeElapsed,
		args[0].str().c_str(),
		args[1].floatValue(),
		args[2].floatValue(),
		args[3].boolValue()))
	{
		m_CurrentPoint = m_SavePoint;
		return true;
	}

	return false;
}

Q_METHOD_IMPL(LowLevel)
{
	m_HighLevel = GE_FALSE;
	m_lowTime = 0.0f;
	m_ThinkTime = 0.0f;
	m_ElapseTime = 0.0f;

	// TODO
	//this->Order = this->m_CurrentAction->AnimName;
	Order = args[0].str().c_str();
	m_RunOrder = false;
	m_ActionActive = false;

#if USE_METHOD_QUEUE_STACK
	StopSound(m_SoundID);
	DequeueTop();
#else
	ActionList *pool, *temp;
	pool = this->Bottom;
	while(pool != NULL)
	{
		temp = pool->next;
		geRam_Free(pool);
		pool = temp;
	}

	this->Top = NULL;
	this->Bottom = NULL;
	this->m_CurrentAction = NULL;
#endif
	return true;
}

Q_METHOD_IMPL(BoxWidth)
{
	if(m_Actor)
	{
		if(args[0].floatValue() > 0.0f)
		{
			geExtBox theBox;
			CCD->ActorManager()->GetBoundingBox(m_Actor, &theBox);
			CCD->ActorManager()->SetBBox(m_Actor, args[0].floatValue(), -theBox.Max.Y*2.0f, args[0].floatValue());
		}
	}

	return true;
}

Q_METHOD_IMPL(BoxHeight)
{
	if(m_Actor)
	{
		if(args[0].floatValue() > 0.0f)
		{
			geExtBox theBox;
			CCD->ActorManager()->GetBoundingBox(m_Actor, &theBox);
			CCD->ActorManager()->SetBBox(m_Actor, theBox.Max.X * 2.0f, -(args[0].floatValue()) * 2.0f, theBox.Max.Z * 2.0f);
		}
	}

	return true;
}

Q_METHOD_IMPL(Scale)
{
	if(m_Actor)
	{
		if(	args.entries() > 1 &&
		   (args[0].floatValue() != args[1].floatValue() ||
			args[0].floatValue() != args[2].floatValue() ||
			args[1].floatValue() != args[2].floatValue()))
		{
			geVec3d Scale;

			CCD->ActorManager()->GetScaleXYZ(m_Actor, &Scale);

			Scale.X *= args[0].floatValue();
			Scale.Y *= args[1].floatValue();
			Scale.Z *= args[2].floatValue();

			CCD->ActorManager()->SetScaleXYZ(m_Actor, Scale);

			if(m_WeaponActor)
			{
				Scale.X = args[0].floatValue() * m_WScale.X;
				Scale.Y = args[1].floatValue() * m_WScale.Y;
				Scale.Z = args[2].floatValue() * m_WScale.Z;

				CCD->ActorManager()->SetScaleXYZ(m_WeaponActor, Scale);
			}
		}
		else
		{
			float scale;

			CCD->ActorManager()->GetScale(m_Actor, &scale);
			CCD->ActorManager()->SetScale(m_Actor, args[0].floatValue() * scale);

			if(m_WeaponActor)
			{
				CCD->ActorManager()->SetScale(m_WeaponActor, args[0].floatValue() * m_WScale.X);
			}
		}
	}

	return true;
}

Q_METHOD_IMPL(SetScale)
{
	if(m_Actor)
	{
		if(	args.entries() > 1 &&
		   (args[0].floatValue() != args[1].floatValue() ||
			args[0].floatValue() != args[2].floatValue() ||
			args[1].floatValue() != args[2].floatValue()))
		{
			geVec3d Scale;

			if(m_WeaponActor)
			{
				CCD->ActorManager()->GetScaleXYZ(m_Actor, &Scale);

				Scale.X = args[0].floatValue() * (m_WScale.X / Scale.X);
				Scale.Y = args[1].floatValue() * (m_WScale.Y / Scale.Y);
				Scale.Z = args[2].floatValue() * (m_WScale.Z / Scale.Z);

				CCD->ActorManager()->SetScaleXYZ(m_WeaponActor, Scale);
			}

			Scale.X = args[0].floatValue();
			Scale.Y = args[1].floatValue();
			Scale.Z = args[2].floatValue();

			CCD->ActorManager()->SetScaleXYZ(m_Actor, Scale);
		}
		else
		{
			if(m_WeaponActor)
			{
				float scale;
				CCD->ActorManager()->GetScale(m_Actor, &scale);
				scale = args[0].floatValue() * (m_WScale.X / scale);
				CCD->ActorManager()->SetScale(m_WeaponActor, scale);
			}

			CCD->ActorManager()->SetScale(m_Actor, args[0].floatValue());
		}
	}

	return true;
}

Q_METHOD_IMPL(FireProjectile)
{
	if(m_Actor)
	{
		if(args.entries() > 6)
		{
			if(args[6].str() != "")
			{
				m_SoundID = PlaySound(args[6].str().c_str(), m_SoundID);
			}
		}

		geXForm3d Xf;
		bool bone;

		if(m_WeaponActor)
			bone = geActor_GetBoneTransform(m_WeaponActor, args[1].str().c_str(), &Xf);
		else
			bone = geActor_GetBoneTransform(m_Actor, args[1].str().c_str(), &Xf);

		if(bone)
		{
			geVec3d theRotation, Pos, Direction, Orient, TargetPoint;

			geVec3d_Copy(&Xf.Translation, &Pos);
			CCD->ActorManager()->GetRotate(m_Actor, &theRotation);

			geXForm3d_SetZRotation(&Xf, theRotation.Z);
			geXForm3d_RotateX(&Xf, theRotation.X);
			geXForm3d_RotateY(&Xf, theRotation.Y);

			geXForm3d_GetIn(&Xf, &Direction);
			geVec3d_AddScaled(&Pos, &Direction, 1000.0f, &TargetPoint);
			geVec3d_AddScaled(&Pos, &Direction, args[4].floatValue(), &Pos);

			geXForm3d_GetUp(&Xf, &Direction);
			geVec3d_AddScaled(&Pos, &Direction, args[3].floatValue(), &Pos);

			geXForm3d_GetLeft(&Xf, &Direction);
			geVec3d_AddScaled(&Pos, &Direction, args[2].floatValue(), &Pos);

			geVec3d_Subtract(&TargetPoint, &Pos, &Orient);
			float l = geVec3d_Length(&Orient);

			if(l > 0.0f)
			{
				float x = Orient.X;
				Orient.X = GE_PIOVER2 - acos(Orient.Y / l);
				Orient.Y = atan2(x, Orient.Z) + GE_PI;
				Orient.Z = 0.0f;	// roll is zero - always!!?

				std::string Projectile, Attribute;
				Projectile = args[0].str().c_str();
				Attribute = args[5].str().c_str();

				CCD->Weapons()->AddProjectile(Pos, Pos, Orient, Projectile, Attribute, Attribute);
			}
		}
	}

	return true;
}

Q_METHOD_IMPL(AddExplosion)
{
	if(m_Actor)
	{
		geVec3d Pos;

		if(geActor_DefHasBoneNamed(geActor_GetActorDef(m_Actor), args[1].str().c_str()))
		{
			Pos.X = args[2].floatValue();
			Pos.Y = args[3].floatValue();
			Pos.Z = args[4].floatValue();

			CCD->Level()->ExplosionManager()->AddExplosion(args[0].str().c_str(), Pos, m_Actor, args[1].str().c_str());
		}
	}

	return true;
}

Q_METHOD_IMPL(TargetGroup)
{
	if(args[0].str() != "")
		m_TargetGroup = args[0].str().c_str();

	return true;
}

Q_METHOD_IMPL(TestDamageOrder)
{
	if(!m_Actor)
		return true;

	if(m_Attribute.empty())
		return true;

	if(args[1].str() == "")
		return true;

	CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(m_Actor);

	int amt = abs(theInv->GetModifyAmt(m_Attribute));

	if(args[0].intValue() <= amt)
	{
		Order = args[1].str().c_str();
		m_RunOrder = true;

		// clear queue
#if USE_METHOD_QUEUE_STACK
		DequeueTop();
#else
		ActionList *pool, *temp;
		pool = this->Bottom;

		while(pool != NULL)
		{
			temp = pool->next;
			geRam_Free(pool);
			pool = temp;
		}

		Top = NULL;
		Bottom = NULL;
		m_CurrentAction = NULL;
#endif
	}

	return true;
}

Q_METHOD_IMPL(SetLODDistance)
{
	if(m_Actor)
	{
		CCD->ActorManager()->SetLODdistance(
			m_Actor,
			args[0].floatValue(),
			args[1].floatValue(),
			args[2].floatValue(),
			args[3].floatValue(),
			args[4].floatValue());
	}

	return true;
}

Q_METHOD_IMPL(AttachToActor)
{
	if(!m_Actor)
		return true;

	geActor *slaveActor = CCD->ActorManager()->GetByEntityName(args[0].str().c_str());

	if(!slaveActor)
		return true;

	geVec3d Position, Rotation;
	Position.X = args[3].floatValue();
	Position.Y = args[4].floatValue();
	Position.Z = args[5].floatValue();

	Rotation.X = args[6].floatValue();
	Rotation.Y = args[7].floatValue();
	Rotation.Z = args[8].floatValue();

	CCD->ActorManager()->ActorAttach(	slaveActor, args[1].str().c_str(),
										m_Actor, args[2].str().c_str(),
										Position, Rotation);

	return true;
}

Q_METHOD_IMPL(DetachFromActor)
{
	if(m_Actor)
	{
		geActor *actor = CCD->ActorManager()->GetByEntityName(args[0].str().c_str());

		if(!actor)
			return true;

		CCD->ActorManager()->DetachFromActor(m_Actor, actor);
	}

	return true;
}

Q_METHOD_IMPL(AttachBlendActor)
{
	if(args.entries() > 1)
	{
		geActor *slaveActor = CCD->ActorManager()->GetByEntityName(args[0].str().c_str());

		if(!slaveActor)
			return true;

		geActor *masterActor;

		if(args[1].str().equalsIgnoreCase("Player"))
			masterActor = CCD->Player()->GetActor();
		else
			masterActor = CCD->ActorManager()->GetByEntityName(args[1].str().c_str());

		if(!masterActor)
			return true;

		CCD->ActorManager()->ActorAttachBlend(slaveActor, masterActor);
	}
	else
	{
		if(m_Actor)
		{
			geActor *slaveActor = CCD->ActorManager()->GetByEntityName(args[0].str().c_str());

			if(!slaveActor)
				return true;

			CCD->ActorManager()->ActorAttachBlend(slaveActor, m_Actor);
		}
	}

	return true;
}

Q_METHOD_IMPL(DetachBlendActor)
{
	if(args.entries() > 1)
	{
		geActor *slaveActor = CCD->ActorManager()->GetByEntityName(args[0].str().c_str());

		if(!slaveActor)
			return true;

		geActor *masterActor;

		if(args[1].str().equalsIgnoreCase("Player"))
			masterActor = CCD->Player()->GetActor();
		else
			masterActor = CCD->ActorManager()->GetByEntityName(args[1].str().c_str());

		if(!masterActor)
			return true;

		CCD->ActorManager()->DetachBlendFromActor(masterActor, slaveActor);
	}
	else
	{
		if(m_Actor)
		{
			geActor *slaveActor = CCD->ActorManager()->GetByEntityName(args[0].str().c_str());

			if(!slaveActor)
				return true;

			CCD->ActorManager()->DetachBlendFromActor(m_Actor, slaveActor);
		}
	}

	return true;
}

Q_METHOD_IMPL(AttachAccessory)
{
	geActor *masterActor = NULL;
	std::string entityName;

	if(args.entries() > 1)
	{
		entityName = args[1].str().c_str();

		if(args[1].str().equalsIgnoreCase("Player"))
			masterActor = CCD->Player()->GetActor();
		else
			masterActor = CCD->ActorManager()->GetByEntityName(entityName.c_str());
	}
	else
	{
		entityName = CCD->ActorManager()->GetEntityName(m_Actor);
		masterActor = m_Actor;
	}

	if(masterActor)
	{
		unsigned int keynum = CCD->Level()->Pawns()->AccessoryCache.size();

		for(unsigned int i=0; i<keynum; ++i)
		{
			if(CCD->Level()->Pawns()->AccessoryCache[i].Name == args[0].str().c_str())
			{
				entityName += args[0].str().c_str();

				if(CCD->ActorManager()->GetByEntityName(entityName.c_str()))
					break;

				geActor *slaveActor = CCD->ActorManager()->SpawnActor(
										CCD->Level()->Pawns()->AccessoryCache[i].ActorName,
										m_Location,
										CCD->Level()->Pawns()->AccessoryCache[i].Rotation,
										"", "",
										NULL);

				if(!slaveActor)
					break;

				CCD->ActorManager()->SetEntityName(slaveActor, entityName.c_str());

				if(CCD->Level()->Pawns()->AccessoryCache[i].EnvironmentMapping)
				{
					SetEnvironmentMapping(	slaveActor, true,
											CCD->Level()->Pawns()->AccessoryCache[i].AllMaterial,
											CCD->Level()->Pawns()->AccessoryCache[i].PercentMapping,
											CCD->Level()->Pawns()->AccessoryCache[i].PercentMaterial);
				}

				CCD->ActorManager()->ActorAttachBlend(slaveActor, masterActor);
				break;
			}
		}
	}

	return true;
}

Q_METHOD_IMPL(DetachAccessory)
{
	geActor *masterActor = NULL;
	std::string entityName;

	if(args.entries() > 1)
	{
		entityName = args[1].str().c_str();

		if(args[1].str().equalsIgnoreCase("Player"))
			masterActor = CCD->Player()->GetActor();
		else
			masterActor = CCD->ActorManager()->GetByEntityName(entityName.c_str());
	}
	else
	{
		entityName = CCD->ActorManager()->GetEntityName(m_Actor);
		masterActor = m_Actor;
	}

	if(masterActor)
	{
		entityName += args[0].str().c_str();
		geActor *slaveActor = CCD->ActorManager()->GetByEntityName(entityName.c_str());

		if(slaveActor)
		{
			CCD->ActorManager()->DetachBlendFromActor(masterActor, slaveActor);
			CCD->ActorManager()->RemoveActorCheck(slaveActor);
			geActor_Destroy(&slaveActor);
		}
	}

	return true;
}

Q_METHOD_IMPL(SetWeapon)
{
	if(m_Actor)
	{
		unsigned int keynum = CCD->Level()->Pawns()->WeaponCache.size();

		for(unsigned int i=0; i<keynum; ++i)
		{
			if(CCD->Level()->Pawns()->WeaponCache[i].Name == args[0].str().c_str())
			{
				if(m_WeaponActor)
				{
					CCD->ActorManager()->RemoveActorCheck(m_WeaponActor);
					geActor_Destroy(&m_WeaponActor);
					m_WeaponActor = NULL;
				}

				m_WeaponActor = CCD->ActorManager()->SpawnActor(
									CCD->Level()->Pawns()->WeaponCache[i].ActorName,
									m_Location,
									CCD->Level()->Pawns()->WeaponCache[i].Rotation,
									"",
									"",
									NULL);

				CCD->ActorManager()->SetActorDynamicLighting(
					m_WeaponActor,
					CCD->Level()->Pawns()->WeaponCache[i].FillColor,
					CCD->Level()->Pawns()->WeaponCache[i].AmbientColor,
					CCD->Level()->Pawns()->WeaponCache[i].AmbientLightFromFloor);

				m_WRotation = CCD->Level()->Pawns()->WeaponCache[i].Rotation;

				m_WScale.X = m_WScale.Y = m_WScale.Z = CCD->Level()->Pawns()->WeaponCache[i].Scale;

				CCD->ActorManager()->SetScale(m_WeaponActor, m_WScale.X);

				CCD->ActorManager()->SetNoCollide(m_WeaponActor);

				if(CCD->Level()->Pawns()->WeaponCache[i].EnvironmentMapping)
				{
					SetEnvironmentMapping(
						m_WeaponActor,
						true,
						CCD->Level()->Pawns()->WeaponCache[i].AllMaterial,
						CCD->Level()->Pawns()->WeaponCache[i].PercentMapping,
						CCD->Level()->Pawns()->WeaponCache[i].PercentMaterial);
				}

				break;
			}
		}
	}

	return true;
}

Q_METHOD_IMPL(RemoveWeapon)
{
	if(m_Actor)
	{
		if(m_WeaponActor)
		{
			CCD->ActorManager()->RemoveActorCheck(m_WeaponActor);
			geActor_Destroy(&m_WeaponActor);
			m_WeaponActor = NULL;
		}
	}

	return true;
}

Q_METHOD_IMPL(debug)
{
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
			strcpy(m_ConsoleDebug[index], args[0].str().c_str());
		}
		else
		{
			for(i=1; i<DEBUGLINES; ++i)
			{
				strcpy(m_ConsoleDebug[i-1], m_ConsoleDebug[i]);
			}

			strcpy(m_ConsoleDebug[DEBUGLINES-1], args[0].str().c_str());
		}
	}

	return true;
}

Q_METHOD_IMPL(ShowTextDelay)
{
	if(m_StartAction)
	{
		m_StartAction = false;

		if(args.entries() > 9)
		{
			if(m_Actor && args[2].str() != "")
			{
				CCD->ActorManager()->SetMotion(m_Actor, args[2].str().c_str());
				CCD->ActorManager()->SetHoldAtEnd(m_Actor, false);
			}

			CCD->Level()->Pawns()->TextMessage[args[0].intValue()].ShowText = true;

			if(args[6].str() != "")
			{
				PlaySound(args[6].str().c_str(), -1, GE_FALSE);
			}
		}
		else
		{
			CCD->Level()->Pawns()->TextMessage[args[0].intValue()].ShowText = false;
			return true;
		}
	}

	m_Time += timeElapsed * 0.001f;

	if(m_Time >= args[5].floatValue())
	{
		m_Time = 0.0f;
		CCD->Level()->Pawns()->TextMessage[args[0].intValue()].ShowText = false;
		return true;
	}

	return false;
}

Q_METHOD_IMPL(ShowText)
{
	if(m_Actor && args[2].str() != "")
	{
		CCD->ActorManager()->SetMotion(m_Actor, args[2].str().c_str());
		CCD->ActorManager()->SetHoldAtEnd(m_Actor, false);
	}

	if(args.entries() > 5)
	{
		if(args[5].str() != "")
		{
			PlaySound(args[5].str().c_str(), -1, GE_FALSE);
		}
	}

	CCD->Level()->Pawns()->TextMessage[args[0].intValue()].ShowText = true;

	return true;
}

Q_METHOD_IMPL(RemoveText)
{
	CCD->Level()->Pawns()->TextMessage[args[0].intValue()].ShowText = false;

	return true;
}

Q_METHOD_IMPL(MouseControlledPlayer)
{
	// deprecated
	CCD->SetMouseControl(args[0].boolValue());
	return true;
}

Q_METHOD_IMPL(EndScript)
{
	m_Alive = false;
	return true;
}


/* ------------------------------------------------------------------------------------ */
//	CanSee
/* ------------------------------------------------------------------------------------ */
bool ScriptedObject::CanSee(float FOV, const geActor *Actor, const geActor *TargetActor, const std::string& Bone)
{
	geVec3d pos, targetPos, temp, in;
	float dotProduct;

	CCD->ActorManager()->GetPosition(Actor, &pos);
	CCD->ActorManager()->InVector(Actor, &in);

	if(!Bone.empty())
	{
		geXForm3d xf;

		if(geActor_GetBoneTransform(Actor, Bone.c_str(), &xf) == GE_TRUE)
		{
			geVec3d_Copy(&xf.Translation, &pos);
			geXForm3d_GetIn(&xf, &in);
		}
	}

	CCD->ActorManager()->GetPosition(TargetActor, &targetPos);
	geVec3d_Subtract(&targetPos, &pos, &temp);
	geVec3d_Normalize(&temp);
	dotProduct = geVec3d_DotProduct(&temp, &in);

	if(dotProduct > FOV)
	{
		if(CanSeeActorToActor(Actor, TargetActor))
			return true;
	}

	return false;
}

/* ------------------------------------------------------------------------------------ */
//	CanSeePoint
/* ------------------------------------------------------------------------------------ */
bool ScriptedObject::CanSeePoint(float FOV, const geActor *Actor, const geVec3d *TargetPoint, const std::string& Bone)
{
	geVec3d pos, temp, in;
	float dotProduct;

	CCD->ActorManager()->GetPosition(Actor, &pos);
	CCD->ActorManager()->InVector(Actor, &in);

	if(!Bone.empty())
	{
		geXForm3d xf;

		if(geActor_GetBoneTransform(Actor, Bone.c_str(), &xf) == GE_TRUE)
		{
			geVec3d_Copy(&xf.Translation, &pos);
			geXForm3d_GetIn(&xf, &in);
		}
	}

	geVec3d_Subtract(TargetPoint, &pos, &temp);
	geVec3d_Normalize(&temp);
	dotProduct = geVec3d_DotProduct(&temp, &in);

	if(dotProduct > FOV)
	{
		if(CanSeeActorToPoint(Actor, TargetPoint))
			return true;
	}

	return false;
}

/* ------------------------------------------------------------------------------------ */
//	PlayerDistance
/* ------------------------------------------------------------------------------------ */
bool ScriptedObject::PlayerDistance(float minDistance)
{
	bool flg = false;
	bool fov = true;

	if(m_FOV > -2.0f && m_Actor)
		fov = CanSee(m_FOV, m_Actor, CCD->Player()->GetActor(), m_FOVBone);

	if(fov)
	{
		if(minDistance >= 0.0f)
		{
			if(m_Actor)
				flg = CCD->ActorManager()->DistanceFrom(m_Actor, CCD->Player()->GetActor()) < minDistance;
			else
				flg = CCD->ActorManager()->DistanceFrom(m_DeadPos, CCD->Player()->GetActor()) < minDistance;
		}
		else
		{
			if(m_Actor)
				flg = CCD->ActorManager()->DistanceFrom(m_Actor, CCD->Player()->GetActor()) > fabs(minDistance);
			else
				flg = CCD->ActorManager()->DistanceFrom(m_DeadPos, CCD->Player()->GetActor()) > fabs(minDistance);
		}
	}

	return flg;
}


/* ------------------------------------------------------------------------------------ */
//	MoveToPoint
/* ------------------------------------------------------------------------------------ */
bool ScriptedObject::MoveToPoint(float timeElapsed, skRValueArray& args)
{
	geVec3d Pos, Orient, NewPos;
	geXForm3d Xf;

	m_AvoidMode = false;

	if(m_StartAction)
	{
		if(args[0].str() != "")
		{
			CCD->ActorManager()->SetHoldAtEnd(m_Actor, false);
			CCD->ActorManager()->SetMotion(m_Actor, args[0].str().c_str());
		}

		if(args.entries() > 2)
		{
			if(args[2].str() != "")
			{
				m_SoundID = PlaySound(args[2].str().c_str(), m_SoundID);
			}
		}

		m_StartAction = false;
		CCD->ActorManager()->GetPosition(m_Actor, &Pos);
		geVec3d_Subtract(&m_CurrentPoint, &Pos, &m_Vec2Point);
		Orient = m_Vec2Point;
		m_Vec2Point.Y = 0.0f;
		geVec3d_Normalize(&m_Vec2Point);
		float l = geVec3d_Length(&Orient);

		if(l > 0.0f)
		{
			float x = Orient.X;

			Orient.X = GE_PIOVER2 - acos(Orient.Y / l);
			Orient.Y = atan2(x, Orient.Z) + GE_PI;
			Orient.Z = 0.0f;	// roll is zero - always!!?

			geXForm3d_SetXRotation(&Xf, Orient.X);
			geXForm3d_RotateY(&Xf, Orient.Y);

			geXForm3d_GetIn(&Xf, &m_TempPoint);
		}
		else
		{
			return true;
		}
	}

	CCD->ActorManager()->GetPosition(m_Actor, &Pos);

	float distance = args[1].floatValue() * (timeElapsed * 0.001f);

	if(	fabs(Pos.X - m_CurrentPoint.X) < distance &&
		fabs(Pos.Z - m_CurrentPoint.Z) < distance)
		return true;

	geVec3d_AddScaled(&Pos, &m_TempPoint, distance, &NewPos);

	bool result = CCD->ActorManager()->ValidateMove(Pos, NewPos, m_Actor, false);

	if(!result)
		m_AvoidMode = true;

	return false;
}


/* ------------------------------------------------------------------------------------ */
//	RotateToPoint
/* ------------------------------------------------------------------------------------ */
bool ScriptedObject::RotateToPoint(
	float timeElapsed,
	const std::string& animation,
	float rSpeed,
	bool rotateXY)
{
	if(m_StartAction)
	{
		if(!animation.empty())
		{
			CCD->ActorManager()->SetMotion(m_Actor, animation);
			CCD->ActorManager()->SetHoldAtEnd(m_Actor, false);
		}

		m_StartAction = false;
	}

	geVec3d pos, orient;
	CCD->ActorManager()->GetPosition(m_Actor, &pos);
	geVec3d_Subtract(&m_CurrentPoint, &pos, &orient);

	float l = geVec3d_Length(&orient);
	float rotateAmt = 0.0f;
	float tiltAmt = 0.0f;

	if(l > 0.0f)
	{
		float x = orient.X;
		orient.X = GE_PIOVER2 - acos(orient.Y / l);
		orient.Y = atan2(x, orient.Z) + GE_PI;
	}

	{
		CCD->ActorManager()->GetRotate(m_Actor, &pos);

		while(pos.Y < 0.0f)		{ pos.Y += GE_2PI; }
		while(pos.Y >= GE_2PI)	{ pos.Y -= GE_2PI; }

		bool rotateLeft = false;
		rotateAmt = pos.Y - orient.Y;

		if(rotateAmt < 0.0f)
		{
			rotateAmt = -rotateAmt;
			rotateLeft = true;
		}

		if(rotateAmt > 0.0f)
		{
			if(rotateAmt > GE_PI)
			{
				rotateAmt = GE_2PI - rotateAmt;

				if(rotateLeft)	{ rotateLeft = false; }
				else			{ rotateLeft = true;  }
			}

			rotateAmt *= GE_180OVERPI;
			float amount = rSpeed * (timeElapsed * 0.001f);

			if(amount > rotateAmt)
			{
				amount = rotateAmt;
			}

			rotateAmt -= amount;

			if(rotateLeft)	CCD->ActorManager()->TurnRight(m_Actor, GE_PIOVER180 * amount);
			else			CCD->ActorManager()->TurnLeft (m_Actor, GE_PIOVER180 * amount);
		}

		if(rotateXY)
		{
			while(pos.X < 0.0f)		{ pos.X += GE_2PI; }
			while(pos.X >= GE_2PI)	{ pos.X -= GE_2PI; }

			bool tiltUp = false;
			tiltAmt = pos.X - orient.X;

			if(tiltAmt < 0.0f)
			{
				tiltAmt = -tiltAmt;
				tiltUp = true;
			}

			if(tiltAmt > 0.0f)
			{
				if(tiltAmt > GE_PI)
				{
					tiltAmt = GE_2PI - tiltAmt;

					if(tiltUp)	{ tiltUp = false; }
					else		{ tiltUp = true;  }
				}

				tiltAmt *= GE_180OVERPI;
				float amount = rSpeed * (timeElapsed * 0.001f);

				if(amount > tiltAmt)
				{
					amount = tiltAmt;
				}

				tiltAmt -= amount;

				if(tiltUp)	CCD->ActorManager()->TiltUp  (m_Actor, GE_PIOVER180 * amount);
				else		CCD->ActorManager()->TiltDown(m_Actor, GE_PIOVER180 * amount);
			}
		}

		if(rotateAmt <= 0.0f && tiltAmt <= 0.0f)
			return true;

		return false;
	}

	return true;
}


/* ------------------------------------------------------------------------------------ */
//	Rotate
/* ------------------------------------------------------------------------------------ */
bool ScriptedObject::Rotate(
	float timeElapsed,
	const std::string& animation,
	float speed,
	geVec3d *angle)
{
	if(m_StartAction)
	{
		if(!animation.empty())
		{
			CCD->ActorManager()->SetMotion(m_Actor, animation);
			CCD->ActorManager()->SetHoldAtEnd(m_Actor, false);
		}

		m_StartAction = false;
	}

	CCD->ActorManager()->GetRotate(m_Actor, &m_TempPoint);

	float baseamount = speed * (timeElapsed * 0.001f);

	bool flag = true;

	if(angle->X != 0.0f)
	{
		float amount = baseamount;

		if(angle->X < 0.0f)
		{
			amount = -amount;

			if(amount < angle->X)
				amount = angle->X;
		}
		else
		{
			if(amount > angle->X)
				amount = angle->X;
		}

		angle->X -= amount;
		amount *= GE_PIOVER180;
		m_TempPoint.X += amount;
		flag = false;
	}

	if(angle->Y != 0.0f)
	{
		float amount = baseamount;

		if(angle->Y < 0.0f)
		{
			amount = -amount;

			if(amount < angle->Y)
				amount = angle->Y;
		}
		else
		{
			if(amount > angle->Y)
				amount = angle->Y;
		}

		angle->Y -= amount;
		amount *= GE_PIOVER180;
		m_TempPoint.Y += amount;
		flag = false;
	}

	if(angle->Z != 0.0f)
	{
		float amount = baseamount;

		if(angle->Z < 0.0f)
		{
			amount = -amount;

			if(amount < angle->Z)
				amount = angle->Z;
		}
		else
		{
			if(amount > angle->Z)
				amount = angle->Z;
		}

		angle->Z -= amount;
		amount *= GE_PIOVER180;
		m_TempPoint.Z += amount;
		flag = false;
	}

	CCD->ActorManager()->Rotate(m_Actor, m_TempPoint);

	return flag;
}

/* ------------------------------------------------------------------------------------ */
//	RotateMoveToPoint
/* ------------------------------------------------------------------------------------ */
bool ScriptedObject::RotateMoveToPoint(
	float timeElapsed,
	const std::string& animation,
	float rSpeed,
	float speed,
	bool rotateXY)
{
	if(RotateToPoint(timeElapsed, animation, rSpeed, rotateXY))
		return true;

	geVec3d Pos, NewPos, In;

	CCD->ActorManager()->GetPosition(m_Actor, &Pos);
	float distance = speed * (timeElapsed * 0.001f);
	CCD->ActorManager()->InVector(m_Actor, &In);
	geVec3d_AddScaled(&Pos, &In, distance, &NewPos);

	CCD->ActorManager()->ValidateMove(Pos, NewPos, m_Actor, false);

	return false;
}

/* ------------------------------------------------------------------------------------ */
//	Move
/* ------------------------------------------------------------------------------------ */
bool ScriptedObject::Move(
	float timeElapsed,
	const std::string& animation,
	float speed,
	float totalDistance,
	float angleX,
	float angleY,
	float angleZ)
{
	if(!m_Actor)
		return true;

	geVec3d Pos, NewPos;
	bool retflg = false;

	if(m_StartAction)
	{
		m_TotalDist = 0.0f;

		if(!animation.empty())
		{
			CCD->ActorManager()->SetHoldAtEnd(m_Actor, false);
			CCD->ActorManager()->SetMotion(m_Actor, animation);
		}

		m_StartAction = false;
		CCD->ActorManager()->GetPosition(m_Actor, &Pos);

		if(m_Direction == RGF_K_BACKWARD || m_Direction == RGF_K_FORWARD)
		{
			CCD->ActorManager()->InVector(m_Actor, &m_Vec2Point);
		}
		else
		{
			if(m_Direction != 0)
			{
				CCD->ActorManager()->LeftVector(m_Actor, &m_Vec2Point);
			}
			else
			{
				geXForm3d thePosition;
				geVec3d Rotate;
				CCD->ActorManager()->GetRotate(m_Actor, &Rotate);

				geXForm3d_SetZRotation(&thePosition,Rotate.Z + angleZ * GE_PIOVER180);
				geXForm3d_RotateX(&thePosition,		Rotate.X + angleX * GE_PIOVER180);
				geXForm3d_RotateY(&thePosition,		Rotate.Y + angleY * GE_PIOVER180);

				geXForm3d_GetIn(&thePosition, &m_Vec2Point);
			}
		}
	}

	CCD->ActorManager()->GetPosition(m_Actor, &Pos);

	float distance = speed * (timeElapsed * 0.001f);

	if((distance + m_TotalDist) >= totalDistance)
	{
		distance = totalDistance - m_TotalDist;
		retflg = true;
	}

	m_TotalDist += distance;

	if(m_Direction == RGF_K_BACKWARD || m_Direction == RGF_K_RIGHT)
		distance = -distance;

	geVec3d_AddScaled(&Pos, &m_Vec2Point, distance, &NewPos);

	bool result = CCD->ActorManager()->ValidateMove(Pos, NewPos, m_Actor, false);

	if(!result)
		return true;

	return retflg;
}


/* ------------------------------------------------------------------------------------ */
//	AreaOrder
//
//	detects if Entity1 is within:
//	if DistanceMode=true: 2 distances defined and within 2 screen widths (from entity2)
//	if DistanceMode=false: 2 screen heights and within 2 screen widths (from entity2)
/* ------------------------------------------------------------------------------------ */
bool CPawnManager::Area(
	const std::string& FromActorName,
	const std::string& ToActorName,
	bool DistanceMode,
	float MinScr, float MaxScr,
	float MinDist, float MaxDist,
	bool IgnoreX, bool IgnoreY, bool IgnoreZ)
{
	geVec3d Pos, ScreenPos;
	int ScreenX, ScreenY;

	if(FromActorName == "Mouse")
	{
		POINT MousePos;

		CCD->Input()->GetMousePos(&MousePos.x, &MousePos.y);
		CCD->GUIManager()->ShowCursor();
		ScreenX = static_cast<int>(MousePos.x);
		ScreenY = static_cast<int>(MousePos.y);
		Pos.X = static_cast<float>(MousePos.x);
		Pos.Y = static_cast<float>(MousePos.y);
		Pos.Z = 0.0f;
	}
	else
	{
		if(FromActorName == "Player")
		{
			Pos = CCD->Player()->Position();
		}
		else if(FromActorName == "Camera")
		{
			CCD->CameraManager()->GetPosition(&Pos);
		}
		else
		{
			CCD->ActorManager()->GetPosition(CCD->ActorManager()->GetByEntityName(FromActorName.c_str()), &Pos);
		}

		geCamera_TransformAndProject(CCD->CameraManager()->Camera(), &Pos, &ScreenPos);
		ScreenX = static_cast<int>(ScreenPos.X);
		ScreenY = static_cast<int>(ScreenPos.Y);
	}

	geVec3d ToPos;

	if(ToActorName.empty())
	{
		DistanceMode = false;
	}
	else if(ToActorName == "Player")
	{
		ToPos = CCD->Player()->Position();
	}
	else
	{
		CCD->ActorManager()->GetPosition(CCD->ActorManager()->GetByEntityName(ToActorName.c_str()), &ToPos);
	}

	if(FromActorName == "Mouse") //convert it to screen coordinates
	{
		geCamera_TransformAndProject(CCD->CameraManager()->Camera(), &ToPos, &ToPos);
		ToPos.Z = 0.0f;
	}

	if(DistanceMode)
	{
		if(IgnoreX)		Pos.X = ToPos.X;
		if(IgnoreY)		Pos.Y = ToPos.Y;
		if(IgnoreZ)		Pos.Z = ToPos.Z;

		float DistanceSq = geVec3d_DistanceBetweenSquared(&Pos, &ToPos);

		return ((DistanceSq >= MinDist*MinDist) && (DistanceSq <= MaxDist*MaxDist) && (ScreenX >= MinScr) && (ScreenX <= MaxScr));
	}
	else
	{
		return ((ScreenY >= MinDist) && (ScreenY <= MaxDist) && (ScreenX >= MinScr) && (ScreenX <= MaxScr));
	}
}


/* ------------------------------------------------------------------------------------ */
//	ClearWatchedTriggers
/* ------------------------------------------------------------------------------------ */
void ScriptedObject::ClearWatchedTriggers()
{
	stdext::hash_map<std::string, TriggerOrder*>::iterator iter = m_WatchedTriggers.begin();

	for(; iter!=m_WatchedTriggers.end(); ++iter)
	{
		delete (iter->second);
	}

	m_WatchedTriggers.clear();
}


/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
void ScriptedObject::UpdateWatchedTriggers(float timeElapsed)
{
	stdext::hash_map<std::string, TriggerOrder*>::iterator iter = m_WatchedTriggers.begin();
	for(; iter!=m_WatchedTriggers.end(); ++iter)
	{
		if(iter->second->Type == PTIMER || iter->second->Type == PSOUND)
		{
			iter->second->Time -= (timeElapsed * 0.001f);

			if(iter->second->Time < 0.0f)
				iter->second->Time = 0.0f;
		}
	}
}


/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
void ScriptedObject::CheckWatchedTriggers()
{
	if(!m_TargetDisable)
	{
		bool flag = false;

		stdext::hash_map<std::string, TriggerOrder*>::iterator iter = m_WatchedTriggers.begin();
		for(; iter!=m_WatchedTriggers.end(); ++iter)
		{
			switch(iter->second->Type)
			{
			case PTRIGGER:
				flag = GetTriggerState(iter->first.c_str());
				break;
			case PFLAG:
				flag = (CCD->Level()->Pawns()->GetPawnFlag(iter->second->PFlg) == iter->second->Flag);
				break;
			case PTIMER:
				flag = (iter->second->Time == 0.0f);
				break;
			case PDIST:
				flag = PlayerDistance(iter->second->Time);
				break;
			case PCOLLIDE:
				flag = m_Collision;
				break;
			case PSOUND:
				flag = false;
				if(iter->second->Time == 0.0f)
				{
					iter->second->Time = EffectC_Frand(iter->second->Low, iter->second->High);
					PlaySound(iter->second->OrderName.c_str(), -1, GE_FALSE);
				}
				break;
			}

			if(flag)
			{
				if(iter->second->Delay > 0.0f)
				{
					m_TriggerWait = true;
					m_TriggerTime = iter->second->Delay;
					m_TriggerOrder = iter->second->OrderName;
				}
				else
				{
					Order = iter->second->OrderName;
					ClearActionList();
				}

				delete (iter->second);
				m_WatchedTriggers.erase(iter);

				break;
			}
		}
	}
}


/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
void ScriptedObject::FindScriptPoint()
{
	if(m_FOV > -2.0f && m_Actor)
	{
		float dist = 9000000.0f;	// only ScripPoints within 3000 texels
		bool done = false;

		geEntity_EntitySet *pSet;
		geEntity *pEntity;

		// Ok, check to see if there are ScriptPoints in this world
		pSet = geWorld_GetEntitySet(CCD->World(), "ScriptPoint");

		if(pSet)
		{
			// Ok, we have ScriptPoints somewhere.  Dig through 'em all.
			for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
				pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
			{
				ScriptPoint *pSource = static_cast<ScriptPoint*>(geEntity_GetUserData(pEntity));

				// search for nearest visible ScripPoint
				if(CanSeePoint(m_FOV, m_Actor, &(pSource->origin), m_FOVBone))
				{
					float newdist;
					geVec3d Pos, Dist;

					// calculate newdist
					CCD->ActorManager()->GetPosition(m_Actor, &Pos);
					geVec3d_Subtract(&pSource->origin, &Pos, &Dist);
					newdist = geVec3d_LengthSquared(&Dist);

					if(newdist < dist)
					{
						dist = newdist;
						m_Point = pSource->szEntityName;
						m_CurrentPoint= pSource->origin;
						m_ValidPoint = true;
						done = true;
					}
				}
			}
		}

		if(done)
		{
			Order = m_PointOrder;
			ClearActionList();
			m_PointFind = false;
		}
	}
}


/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
void ScriptedObject::FindTarget()
{
	if(m_FOV > -2.0f && m_Actor)
	{
		float distance = m_TargetDistance + 100.f;
		bool done = false;

		if(m_HostilePlayer)
		{
			if(CanSee(m_FOV, m_Actor, CCD->Player()->GetActor(), m_FOVBone))
			{
				float dist = CCD->ActorManager()->DistanceFrom(m_Actor, CCD->Player()->GetActor());

				if(dist < m_TargetDistance)
				{
					m_TargetActor = CCD->Player()->GetActor();
					done = true;
					distance = dist;
				}
			}
		}

		geActor *ActorsInRange[512];
		geVec3d Pos;

		CCD->ActorManager()->GetPosition(m_Actor, &Pos);
		int nActorCount = CCD->ActorManager()->GetActorsInRange(Pos, m_TargetDistance, 512, &ActorsInRange[0]);

		if(nActorCount != 0)
		{
			if(m_HostileDiff)
			{
				for(int nTemp=0; nTemp<nActorCount; ++nTemp)
				{
					if(ActorsInRange[nTemp] == m_Actor)
						continue;

					int nType;
					CCD->ActorManager()->GetType(ActorsInRange[nTemp], &nType);

					if((nType == ENTITY_NPC || nType == ENTITY_VEHICLE) &&
						!CCD->ActorManager()->GetGroup(ActorsInRange[nTemp]).empty())
					{
						bool TG = false;

						if(!m_TargetGroup.empty())
						{
							if(m_TargetGroup == CCD->ActorManager()->GetGroup(ActorsInRange[nTemp]))
								TG = true;
						}
						else
						{
							if(m_Group != CCD->ActorManager()->GetGroup(ActorsInRange[nTemp]))
								TG = true;
						}

						if(TG)
						{
							CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(ActorsInRange[nTemp]);

							if(theInv->Has(m_TargetAttr))
							{
								if(theInv->Value(m_TargetAttr) > 0)
								{
									if(CanSee(m_FOV, m_Actor, ActorsInRange[nTemp], m_FOVBone))
									{
										float dist = CCD->ActorManager()->DistanceFrom(m_Actor, ActorsInRange[nTemp]);

										if(dist < m_TargetDistance)
										{
											done = true;

											if(distance > dist)
											{
												distance = dist;
												m_TargetActor = ActorsInRange[nTemp];
											}
										}
									}
								}
							}
						}
					}
				}
			}

			if(m_HostileSame)
			{
				for(int nTemp=0; nTemp<nActorCount; ++nTemp)
				{
					if(ActorsInRange[nTemp] == m_Actor)
						continue;

					int nType;
					CCD->ActorManager()->GetType(ActorsInRange[nTemp], &nType);

					if((nType == ENTITY_NPC || nType == ENTITY_VEHICLE) &&
						!CCD->ActorManager()->GetGroup(ActorsInRange[nTemp]).empty())
					{
						if(m_Group == CCD->ActorManager()->GetGroup(ActorsInRange[nTemp]))
						{
							CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(ActorsInRange[nTemp]);

							if(theInv->Has(m_TargetAttr))
							{
								if(theInv->Value(m_TargetAttr) > 0)
								{
									if(CanSee(m_FOV, m_Actor, ActorsInRange[nTemp], m_FOVBone))
									{
										float dist = CCD->ActorManager()->DistanceFrom(m_Actor, ActorsInRange[nTemp]);

										if(dist < m_TargetDistance)
										{
											done = true;

											if(distance > dist)
											{
												distance = dist;
												m_TargetActor = ActorsInRange[nTemp];
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
			Order = m_TargetOrder;
			ClearActionList();

			m_TargetFind = false;
			CCD->ActorManager()->GetPosition(m_TargetActor, &m_LastTargetPoint);
		}
	}
}


/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
void ScriptedObject::ClearActionList()
{
	m_RunOrder = true;
	m_ActionActive = false;

#if USE_METHOD_QUEUE_STACK
	DequeueTop();

#else
	ActionList *pool, *temp;
	pool = Bottom;

	while(pool != NULL)
	{
		temp = pool->next;
		geRam_Free(pool);
		pool = temp;
	}

	Top = NULL;
	Bottom = NULL;
	m_CurrentAction = NULL;
#endif
}


/* ------------------------------------------------------------------------------------ */
//	UpdateHigh
/* ------------------------------------------------------------------------------------ */
void ScriptedObject::UpdateHigh(float dwTicks)
{
	UpdateWatchedTriggers(dwTicks);

	// check if object has died
	if(!m_DeadOrder.empty())
	{
		if(!m_Attribute.empty())
		{
			CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(m_Actor);

			if(theInv->Value(m_Attribute) <= 0)
			{
				Order = m_DeadOrder;
				m_DeadOrder.clear();

				ClearActionList();
			}
		}
	}

	if(!m_Active)
		return;

	skRValueArray args;
	skRValue ret;

	bool runflag = false;

	do
	{
		runflag = false;

		if(m_TriggerWait)
		{
			m_TriggerTime -= (dwTicks * 0.001f);

			if(m_TriggerTime <= 0.0f && !m_TargetDisable)
			{
				m_TriggerWait = false;
				Order = m_TriggerOrder;
				ClearActionList();
			}
		}
		else
		{
			CheckWatchedTriggers();
		}

		if(m_PointFind)
		{
			FindScriptPoint();
		}

		if(m_TargetFind && !m_TargetDisable)
		{
			FindTarget();
		}

		if(m_DistActive && !m_TargetDisable)
		{
			if(PlayerDistance(m_MinDistance))
			{
				Order = m_DistOrder;
				ClearActionList();

				m_DistActive = false;
			}
		}

		if(m_PainActive && !m_TargetDisable)
		{
			CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(m_Actor);
			int current = theInv->Value(m_Attribute);

			if(current > 0)
			{
				if(current < m_OldAttributeAmount)
				{
					if(EffectC_rand(1, 10) <= (m_PainPercent/10))
					{
						if(!m_PainOrder.empty())
						{
							Push();
							Order = m_PainOrder;
							m_RunOrder = true;
							m_ActionActive = false;
							m_TargetDisable = true;
							runflag = true;
						}
					}
				}
			}

			m_OldAttributeAmount = current;
		}

		if(m_RunOrder)
		{
			bool methoderror = false;

			try
			{
				method(skString(Order.c_str()), args, ret, ScriptManager::GetContext());
			}
			catch(skRuntimeException &e)
			{
				if(!strcmp(m_Indicate, "+"))
					strcpy(m_Indicate, " ");
				else
					strcpy(m_Indicate, "+");

				if(m_Console)
					strcpy(m_ConsoleError, e.toString());
			}
			catch(skParseException &e)
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
				std::string text = "Script Error for " + szName + " in Order " + Order;
				CCD->Log()->Error(text);

				if(m_Console)
					strcpy(m_ConsoleError, text.c_str());

				methoderror = true;
			}

			m_RunOrder = false;

#if USE_METHOD_QUEUE_STACK
#else
			this->m_CurrentAction = NULL;
#endif

			if(!methoderror)
				m_ActionActive = false;
			else
				break;
		}

		if(!m_ActionActive)
		{
#if USE_METHOD_QUEUE_STACK
			// nothing to do
#else
			if(!this->m_CurrentAction)
				this->m_CurrentAction = this->Top;
			else
				this->m_CurrentAction = this->m_CurrentAction->prev;
#endif

			m_ActionActive = true;
			m_StartAction = true;

			if(!m_SoundLoop)
			{
				StopSound(m_SoundID);
			}
		}
		else // m_ActionActive == true
		{
			if(m_MethodQueueStack.size() > 1)
			{
				if(m_MethodQueueStack.top()->empty())
				{
					Pop();
					m_TargetDisable = false;
					m_StartAction = true;
				}
			}
		}

		if(m_FacePlayer && m_Actor)
		{
			geVec3d Pos = CCD->Player()->Position();
			geVec3d Pos1;
			CCD->ActorManager()->GetPosition(m_Actor, &Pos1);
			geVec3d LookRotation;
			geFloat x, l;
			geVec3d_Subtract(&Pos, &Pos1, &LookRotation);
			l = geVec3d_Length(&LookRotation);

			// protect from Div by Zero
			if(l > 0.0f)
			{
				x = LookRotation.X;

				LookRotation.X = -(GE_PIOVER2 - acos(LookRotation.Y / l));
				LookRotation.Y = atan2(x, LookRotation.Z) + GE_PI;
				LookRotation.Z = 0.0f;	// roll is zero - always!!?

				if(!m_FaceAxis)
					LookRotation.X = 0.0f;

				CCD->ActorManager()->Rotate(m_Actor, LookRotation);
			}
		}

		if(!m_MethodQueueStack.empty())
		{
			if(!m_MethodQueueStack.top()->empty())
			{
				if(m_Console)
				{
					std::string text = szName + " " + Order + " " +
										ActionText[m_MethodQueueStack.top()->front()->action] +
										" " + m_Point;
					strcpy(m_ConsoleHeader, text.c_str());
				}

				if((this->*(m_MethodQueueStack.top()->front()->method))(dwTicks, m_MethodQueueStack.top()->front()->args))
				{
					Dequeue();
					runflag = m_Alive;
					m_ActionActive = false;
				}
				else
				{
					m_ActionActive = true;
				}
			}
		}
	} while(runflag);
}

#undef SX_IMPL_TYPE

/* ----------------------------------- END OF FILE ------------------------------------ */
