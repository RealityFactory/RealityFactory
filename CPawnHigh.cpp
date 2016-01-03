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
#include "CHeadsUpDisplay.h"
#include "CInventory.h"
#include "CScriptPoint.h"
#include "CPawn.h"

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
	ROTATEAROUNDPOINT,
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
	"RotateAroundPoint",
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

/* ------------------------------------------------------------------------------------ */
//	calls a method in this object
/* ------------------------------------------------------------------------------------ */
bool ScriptedObject::highmethod(const skString& methodName, skRValueArray& arguments,
								skRValue& returnValue,skExecutableContext &ctxt)
{
	char param0[128], param7[128], param8[128];
	float param1, param3, param4; //param5, param6;
	bool param2;

	param0[0] = '\0';
	param7[0] = '\0';
	param8[0] = '\0';


	switch(ScriptManager::GetHashMethod(methodName))
	{
	case RGF_SM_MOVETOPOINT:
		{
			PARMCHECK(3);
			AddAction(MOVETOPOINT, arguments[0].str().c_str(), arguments[1].floatValue(),
				false, 0.0f, 0.0f, 0.0f, 0.0f, arguments[2].str().c_str(), NULL);
			return true;
		}
	case RGF_SM_ROTATETOPOINT:
		{
			PARMCHECK(4);
			AddAction(ROTATETOPOINT, arguments[0].str().c_str(), arguments[1].floatValue(),
				arguments[2].boolValue(), 0.0f, 0.0f, 0.0f, 0.0f, arguments[3].str().c_str(), NULL);
			return true;
		}
	case RGF_SM_NEWORDER:
		{
			PARMCHECK(1);
			AddAction(NEWORDER, arguments[0].str().c_str(), 0.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
			return true;
		}
	case RGF_SM_NEXTORDER:
		{
			AddAction(NEXTORDER, NULL, 0.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
			return true;
		}
	case RGF_SM_ROTATETOALIGN:
		{
			PARMCHECK(4);
			AddAction(ROTATETOALIGN, arguments[0].str().c_str(), arguments[1].floatValue(),
				arguments[2].boolValue(), 0.0f, 0.0f, 0.0f, 0.0f, arguments[3].str().c_str(), NULL);
			return true;
		}
	case RGF_SM_NEXTPOINT:
		{
			AddAction(NEXTPOINT, NULL, 0.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
			return true;
		}
	case RGF_SM_DELAY:
		{
			PARMCHECK(3);
			AddAction(DELAY, arguments[0].str().c_str(), arguments[1].floatValue(),
				false, 0.0f, 0.0f, 0.0f, 0.0f, arguments[2].str().c_str(), NULL);
			return true;
		}
	case RGF_SM_PLAYANIMATION:
		{
			PARMCHECK(3);
			AddAction(PLAYANIMATION, arguments[0].str().c_str(), 0.0f,
				arguments[1].boolValue(), 0.0f, 0.0f, 0.0f, 0.0f, arguments[2].str().c_str(), NULL);
			return true;
		}
	case RGF_SM_BLENDTOANIMATION:
		{
			PARMCHECK(4);
			AddAction(BLENDTOANIMATION, arguments[0].str().c_str(), arguments[1].floatValue(),
				arguments[2].boolValue(), 0.0f, 0.0f, 0.0f, 0.0f, arguments[3].str().c_str(), NULL);
			return true;
		}
	case RGF_SM_LOOPANIMATION:
		{
			PARMCHECK(3);
			AddAction(LOOPANIMATION, arguments[0].str().c_str(), arguments[1].floatValue(),
				false, 0.0f, 0.0f, 0.0f, 0.0f, arguments[2].str().c_str(), NULL);
			return true;
		}
	case RGF_SM_ROTATE:
		{
			PARMCHECK(6);
			AddAction(ROTATE, arguments[0].str().c_str(), arguments[1].floatValue(),
				false, arguments[2].floatValue(), arguments[3].floatValue(),
				arguments[4].floatValue(), 0.0f, arguments[5].str().c_str(), NULL);
			return true;
		}
	case RGF_SM_ROTATEMOVETOPOINT:
		{
			PARMCHECK(5);
			AddAction(ROTATEMOVETOPOINT, arguments[0].str().c_str(), arguments[1].floatValue(),
				arguments[3].boolValue(), arguments[2].floatValue(), 0.0f, 0.0f, 0.0f,
				arguments[4].str().c_str(), NULL);
			return true;
		}
	case RGF_SM_ROTATEMOVE:
		{
			PARMCHECK(7);
			AddAction(ROTATEMOVE, arguments[0].str().c_str(), arguments[1].floatValue(),
				false, arguments[3].floatValue(), arguments[4].floatValue(), arguments[5].floatValue(),
				arguments[2].floatValue(), arguments[6].str().c_str(), NULL);
			return true;
		}
	case RGF_SM_NEWPATH:
		{
			PARMCHECK(1);
			AddAction(NEWPATH, arguments[0].str().c_str(), 0.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
			return true;
		}
	case RGF_SM_RESTARTORDER:
		{
			AddAction(RESTARTORDER, NULL, 0.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
			return true;
		}
	case RGF_SM_PLAYERDISTORDER:
		{
			PARMCHECK(2);
			MinDistance = arguments[0].floatValue();

			if(MinDistance != 0.0f)
			{
				DistActive = true;
				strcpy(DistOrder, arguments[1].str());
			}
			else
				DistActive = false;

			return true;
		}
	case RGF_SM_CONSOLE:
		{
			PARMCHECK(1);
			console = arguments[0].boolValue();

			if(console)
			{
				ConsoleHeader = (char *)malloc(128);
				*ConsoleHeader = '\0';
				ConsoleError = (char *)malloc(128);
				*ConsoleError = '\0';

				for(int i=0; i<DEBUGLINES; i++)
				{
					ConsoleDebug[i] = (char*)malloc(64);
					*ConsoleDebug[i] = '\0';
				}
			}
			else
			{
				SAFE_FREE(ConsoleHeader);
				SAFE_FREE(ConsoleError);

				for(int i=0; i<DEBUGLINES; i++)
				{
					SAFE_FREE(ConsoleDebug[i]);
				}
			}

			return true;
		}
	case RGF_SM_AUDIBLERADIUS:
		{
			PARMCHECK(1);
			AudibleRadius = arguments[0].floatValue();
			return true;
		}
	case RGF_SM_ADDPAINORDER:
		{
			PARMCHECK(2);
			PainActive = true;
			strcpy(PainOrder, arguments[0].str());
			PainPercent = arguments[1].intValue();
			CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(Actor);
			OldAttributeAmount = theInv->Value(Attribute);
			return true;
		}
	case RGF_SM_FINDTARGETORDER:
		{
			PARMCHECK(3);
			TargetDistance = arguments[0].floatValue();

			if(TargetDistance != 0.0f)
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
	case RGF_SM_FINDPOINTORDER:
		{
			PARMCHECK(1);
			PointFind = true;
			strcpy(PointOrder, arguments[0].str());

			if(EffectC_IsStringNull(PointOrder))
				PointFind = false;

			return true;
		}
	case RGF_SM_NEWPOINT:
		{
			PARMCHECK(1);
			AddAction(NEWPOINT, arguments[0].str().c_str(), 0.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
			return true;
		}
	case RGF_SM_MOVEFORWARD:
		{
			PARMCHECK(4);
			AddAction(MOVEFORWARD, arguments[0].str().c_str(), arguments[1].floatValue(),
				false, arguments[2].floatValue(), 0.0f, 0.0f, 0.0f, arguments[3].str().c_str(), NULL);
			return true;
		}
	case RGF_SM_MOVEBACKWARD:
		{
			PARMCHECK(4);
			AddAction(MOVEBACKWARD, arguments[0].str().c_str(), arguments[1].floatValue(),
				false, arguments[2].floatValue(), 0.0f, 0.0f, 0.0f, arguments[3].str().c_str(), NULL);
			return true;
		}
	case RGF_SM_MOVELEFT:
		{
			PARMCHECK(4);
			AddAction(MOVELEFT, arguments[0].str().c_str(), arguments[1].floatValue(),
				false, arguments[2].floatValue(), 0.0f, 0.0f, 0.0f, arguments[3].str().c_str(), NULL);
			return true;
		}
	case RGF_SM_MOVERIGHT:
		{
			PARMCHECK(4);
			AddAction(MOVERIGHT, arguments[0].str().c_str(), arguments[1].floatValue(),
				false, arguments[2].floatValue(), 0.0f, 0.0f, 0.0f, arguments[3].str().c_str(), NULL);
			return true;
		}
	case RGF_SM_MOVE:
		{
			PARMCHECK(7);
			AddAction(MOVE, arguments[0].str().c_str(), arguments[1].floatValue(),
				false, arguments[2].floatValue(), arguments[3].floatValue(), arguments[4].floatValue(),
				arguments[5].floatValue(), arguments[6].str().c_str(), NULL);
			return true;
		}
	case RGF_SM_AVOIDORDER:
		{
			PARMCHECK(1);
			AddAction(AVOIDORDER, arguments[0].str().c_str(), 0.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
			return true;
		}
	case RGF_SM_RETURN:
		{
			AddAction(RETURN, NULL, 0.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
			return true;
		}
	case RGF_SM_ALIGN:
		{
			AddAction(ALIGN, NULL, 0.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
			return true;
		}
	case RGF_SM_JUMP:
		{
			PARMCHECK(4);
			AddAction(JUMPCMD, arguments[0].str().c_str(), arguments[1].floatValue(),
				arguments[2].boolValue(), 0.0f, 0.0f, 0.0f, 0.0f, arguments[3].str().c_str(), NULL);
			return true;
		}
	case RGF_SM_ADDTRIGGERORDER:
		{
			PARMCHECK(3);
			AddAction(ADDTRIGGERORDER, arguments[0].str().c_str(), arguments[2].floatValue(),
				false, PTRIGGER, 0.0f, 0.0f, 0.0f, NULL, arguments[1].str().c_str());
			return true;
		}
	case RGF_SM_DELTRIGGERORDER:
		{
			PARMCHECK(1);
			AddAction(DELTRIGGERORDER, arguments[0].str().c_str(), 0.0f, false, PTRIGGER, 0.0f, 0.0f, 0.0f, NULL, param0);
			return true;
		}
	case RGF_SM_SETEVENTSTATE:
		{
			PARMCHECK(2);
			AddAction(SETEVENTSTATE, arguments[0].str().c_str(), 0.0f, arguments[1].boolValue(), 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
			return true;
		}
	case RGF_SM_FACEPLAYER:
		{
			PARMCHECK(2);
			param1 = 0.0f;

			if(arguments[1].boolValue())
				param1 = 1.0f;

			AddAction(FACEPLAYER, NULL, param1, arguments[0].boolValue(), 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
			return true;
		}
	case RGF_SM_ROTATETOPLAYER:
		{
			PARMCHECK(4);
			AddAction(ROTATETOPLAYER, arguments[0].str().c_str(), arguments[1].floatValue(),
				arguments[2].boolValue(), 0.0f, 0.0f, 0.0f, 0.0f, arguments[3].str().c_str(), NULL);
			return true;
		}
	case RGF_SM_ROTATEAROUNDPOINTLEFT:
		{
			PARMCHECK(4);
			AddAction(ROTATEAROUNDPOINT, arguments[0].str().c_str(), arguments[1].floatValue(),
				false, 0.0f, arguments[2].floatValue(), 0.0f, 0.0f, arguments[3].str().c_str(), NULL);
			return true;
		}
	case RGF_SM_ROTATEAROUNDPOINTRIGHT:
		{
			PARMCHECK(4);
			AddAction(ROTATEAROUNDPOINT, arguments[0].str().c_str(), arguments[1].floatValue(),
				false, 1.0f, arguments[2].floatValue(), 0.0f, 0.0f, arguments[3].str().c_str(), NULL);
			return true;
		}
	case RGF_SM_TELEPORTTOPOINT:
		{
			PARMCHECK(1);
			float Offx = 0.0f;
			float Offy = 0.0f;
			float Offz = 0.0f;
			param2 = false;
			param8[0] = 0;

			// - use scriptpoint angles as actor orientation after teleporting
			// - specify actor to teleport
			if(arguments.entries() > 3)
			{
				Offx = arguments[1].floatValue();
				Offy = arguments[2].floatValue();
				Offz = arguments[3].floatValue();

				if(arguments.entries() > 4)
				{
					param2 = arguments[4].boolValue();

					if(arguments.entries() > 5)
						strcpy(param8, arguments[5].str());
				}
			}

			AddAction(TELEPORTTOPOINT, arguments[0].str().c_str(), 0.0f, param2, Offx, Offy, Offz, 0.0f, NULL, param8);

			return true;
		}
	case RGF_SM_ANIMATIONSPEED:
		{
			PARMCHECK(1);
			AddAction(ANIMATIONSPEED, NULL, arguments[0].floatValue(), false, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
			return true;
		}
	case RGF_SM_SETFLAG:
		{
			PARMCHECK(2);
			param1 = (float)arguments[0].intValue();

			if(param1 >= 0.0f && param1 < MAXFLAGS)
				AddAction(SETFLAG, NULL, param1, arguments[1].boolValue(), 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);

			return true;
		}
	case RGF_SM_ADDFLAGORDER:
		{
			PARMCHECK(3);
			param1 = (float)arguments[0].intValue();
			sprintf(param7, "PawnFlag%d", (int)param1);

			if(param1 >= 0.0f && param1 < MAXFLAGS)
				AddAction(ADDTRIGGERORDER, arguments[2].str().c_str(), 0.0f,
					arguments[1].boolValue(), PFLAG, param1, 0.0f, 0.0f, NULL, param7);

			return true;
		}
	case RGF_SM_DELFLAGORDER:
		{
			PARMCHECK(1);
			param1 = (float)arguments[0].intValue();
			sprintf(param7, "PawnFlag%d", (int)param1);
			AddAction(DELTRIGGERORDER, NULL, param1, false, PFLAG, 0.0f, 0.0f, 0.0f, NULL, param7);
			return true;
		}
	case RGF_SM_CHANGEMATERIAL:
		{
			PARMCHECK(1);
			AddAction(CHANGEMATERIAL, arguments[0].str().c_str(), 0.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
			return true;
		}
	case RGF_SM_ADDTIMERORDER:
		{
			PARMCHECK(3);
			param1 = (float)arguments[0].intValue();
			sprintf(param7, "PawnTimer%d", (int)param1);
			AddAction(ADDTRIGGERORDER, arguments[2].str().c_str(), 0.0f,
				false, PTIMER, param1, arguments[1].floatValue(), 0.0f, NULL, param7);
			return true;
		}
	case RGF_SM_DELTIMERORDER:
		{
			PARMCHECK(1);
			param1 = (float)arguments[0].intValue();
			sprintf(param7, "PawnTimer%d", (int)param1);
			AddAction(DELTRIGGERORDER, NULL, param1, false, PTIMER, 0.0f, 0.0f, 0.0f, NULL, param7);
			return true;
		}
	case RGF_SM_ADDRANDOMSOUND:
		{
			PARMCHECK(3);
			param1 = (float)arguments[0].intValue();
			sprintf(param7, "RandomSound%d", (int)param1);
			AddAction(ADDTRIGGERORDER, arguments[3].str().c_str(), 0.0f,
				false, PSOUND, param1, arguments[1].floatValue(), arguments[2].floatValue(), NULL, param7);
			return true;
		}
	case RGF_SM_DELRANDOMSOUND:
		{
			PARMCHECK(1);
			param1 = (float)arguments[0].intValue();
			sprintf(param7, "RandomSound%d", (int)param1);
			AddAction(DELTRIGGERORDER, NULL, param1, false, PSOUND, 0.0f, 0.0f, 0.0f, NULL, param7);
			return true;
		}
	case RGF_SM_ADDDISTANCEORDER:
		{
			PARMCHECK(3);
			param1 = (float)arguments[0].intValue();
			sprintf(param7, "PlayerDist%d", (int)param1);
			AddAction(ADDTRIGGERORDER, arguments[2].str().c_str(), 0.0f,
				false, PDIST, param1, arguments[1].floatValue(), 0.0f, NULL, param7);
			return true;
		}
	case RGF_SM_DELDISTANCEORDER:
		{
			PARMCHECK(1);
			param1 = (float)arguments[0].intValue();
			sprintf(param7, "PlayerDist%d", (int)param1);
			AddAction(DELTRIGGERORDER, NULL, param1, false, PDIST, 0.0f, 0.0f, 0.0f, NULL, param7);
			return true;
		}
	case RGF_SM_ADDCOLLISIONORDER:
		{
			PARMCHECK(1);
			sprintf(param7, "PlayerColl%d", 1);
			AddAction(ADDTRIGGERORDER, arguments[0].str().c_str(), 0.0f, false, PCOLLIDE, 1.0f, 0.0f, 0.0f, NULL, param7);
			return true;
		}
	case RGF_SM_DELCOLLISIONORDER:
		{
			sprintf(param7, "PlayerColl%d", 1);
			AddAction(DELTRIGGERORDER, NULL, 1.0f, false, PCOLLIDE, 0.0f, 0.0f, 0.0f, NULL, param7);
			return true;
		}
	case RGF_SM_ANIMATESTOP:
		{
			PARMCHECK(3);
			AddAction(STOPANIMATION, arguments[0].str().c_str(), arguments[1].floatValue(),
				false, 0.0f, 0.0f, 0.0f, 0.0f, arguments[2].str().c_str(), NULL);
			return true;
		}
	case RGF_SM_ATTRIBUTEORDER:
		{
			PARMCHECK(3);
			AddAction(ATTRIBUTEORDER, arguments[0].str().c_str(), (float)arguments[1].intValue(),
				false, 0.0f, 0.0f, 0.0f, 0.0f, NULL, arguments[2].str().c_str());
			return true;
		}
	case RGF_SM_REMOVE:
		{
			PARMCHECK(1);
			AddAction(REMOVE, NULL, 0.0f, arguments[0].boolValue(), 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
			return true;
		}
	case RGF_SM_SETKEYPAUSE:
		{
			PARMCHECK(1);
			AddAction(SETKEYPAUSE, NULL, 0.0f, arguments[0].boolValue(), 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
			return true;
		}
	case RGF_SM_SETNOCOLLISION:
		{
			AddAction(SETNOCOLLISION, NULL, 0.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
			return true;
		}
	case RGF_SM_SETCOLLISION:
		{
			AddAction(SETNOCOLLISION, NULL, 0.0f, true, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
			return true;
		}
	case RGF_SM_ALLOWUSEKEY:
		{
			PARMCHECK(1);
			AddAction(ALLOWUSE, NULL, 0.0f, arguments[0].boolValue(), 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
			return true;
		}
	case RGF_SM_SETHUDDRAW:
		{
			PARMCHECK(1);
			AddAction(SETHUDDRAW, NULL, 0.0f, arguments[0].boolValue(), 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
			return true;
		}
	case RGF_SM_HIDEFROMRADAR:
		{
			PARMCHECK(1);
			AddAction(HIDEFROMRADAR, NULL, 0.0f, arguments[0].boolValue(), 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
			return true;
		}
	case RGF_SM_CONVERSATION:
		{
			// Conversation();
			// Conversation(OrderName);
			// Allows to define a different start-order to be used for this conversation
			if(arguments.entries() > 0)
				strcpy(Converse->Order, arguments[0].str());
			CCD->Pawns()->SetConvFlag(false);
			AddAction(CONVERSATION, NULL, 0.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
			return true;
		}
	case RGF_SM_FADEIN:
		{
			PARMCHECK(2);
			AddAction(FADEIN, NULL, arguments[0].floatValue(), false, arguments[1].floatValue(), 0.0f, 0.0f, 0.0f, NULL, NULL);
			return true;
		}
	case RGF_SM_FADEOUT:
		{
			PARMCHECK(2);
			AddAction(FADEOUT, NULL, arguments[0].floatValue(), false, arguments[1].floatValue(), 0.0f, 0.0f, 0.0f, NULL, NULL);
			return true;
		}
	case RGF_SM_SETFOV:
		{
			PARMCHECK(1);

			if(arguments.entries() == 2)
				strcpy(param0, arguments[1].str());

			AddAction(FIELDOFVIEW, param0, arguments[0].floatValue(), false, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
			return true;
		}
	case RGF_SM_STEPHEIGHT:
		{
			PARMCHECK(1);
			AddAction(STEPHEIGHT, NULL, arguments[0].floatValue(), false, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
			return true;
		}
	case RGF_SM_SETGROUP:
		{
			PARMCHECK(1);
			AddAction(SETGROUP, arguments[0].str().c_str(), 0.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
			return true;
		}
	case RGF_SM_HOSTILEPLAYER:
		{
			PARMCHECK(1);
			AddAction(HOSTILEPLAYER, NULL, 0.0f, arguments[0].boolValue(), 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
			return true;
		}
	case RGF_SM_HOSTILEDIFFERENT:
		{
			PARMCHECK(1);
			AddAction(HOSTILEDIFF, NULL, 0.0f, arguments[0].boolValue(), 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
			return true;
		}
	case RGF_SM_HOSTILESAME:
		{
			PARMCHECK(1);
			AddAction(HOSTILESAME, NULL, 0.0f, arguments[0].boolValue(), 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
			return true;
		}
	case RGF_SM_GRAVITY:
		{
			PARMCHECK(1);
			AddAction(GRAVITY, NULL, 0.0f, arguments[0].boolValue(), 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
			return true;
		}
	case RGF_SM_SOUNDLOOP:
		{
			PARMCHECK(1);
			AddAction(SOUNDLOOP, NULL, 0.0f, arguments[0].boolValue(), 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
			return true;
		}
	case RGF_SM_ISPUSHABLE:
		{
			PARMCHECK(1);
			AddAction(ISPUSHABLE, NULL, 0.0f, arguments[0].boolValue(), 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
			return true;
		}
	case RGF_SM_ISVEHICLE:
		{
			PARMCHECK(1);
			AddAction(ISVEHICLE, NULL, 0.0f, arguments[0].boolValue(), 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
			return true;
		}
	case RGF_SM_MOVETOTARGET:
		{
			PARMCHECK(3);
			AddAction(MOVETOTARGET, arguments[0].str().c_str(), arguments[1].floatValue(),
				false, 0.0f, 0.0f, 0.0f, 0.0f, arguments[2].str().c_str(), NULL);
			return true;
		}
	case RGF_SM_ROTATETOTARGET:
		{
			PARMCHECK(3);
			AddAction(ROTATETOTARGET, arguments[0].str().c_str(), arguments[1].floatValue(),
				arguments[2].boolValue(), 0.0f, 0.0f, 0.0f, 0.0f, arguments[3].str().c_str(), NULL);
			return true;
		}
	case RGF_SM_ROTATEMOVETOTARGET:
		{
			PARMCHECK(5);
			AddAction(ROTATEMOVETOTARGET, arguments[0].str().c_str(), arguments[1].floatValue(),
				arguments[3].boolValue(), arguments[2].floatValue(), 0.0f, 0.0f, 0.0f, arguments[4].str().c_str(), NULL);
			return true;
		}
	case RGF_SM_LOWLEVEL:
		{
			PARMCHECK(1);
			AddAction(LOWLEVEL, arguments[0].str().c_str(), 0.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
			return true;
		}
	case RGF_SM_BOXWIDTH:
		{
			PARMCHECK(1);
			AddAction(BOXWIDTH, NULL, arguments[0].floatValue(), false, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
			return true;
		}
	case RGF_SM_BOXHEIGHT:
		{
			PARMCHECK(1);
			AddAction(BOXHEIGHT, NULL, arguments[0].floatValue(), false, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
			return true;
		}
	case RGF_SM_SCALE:
		{
			// USAGE:	Scale(Scale)
			//			Scale(ScaleX, ScaleY, ScaleZ)
			PARMCHECK(1);
			param1 = arguments[0].floatValue();

			if(arguments.entries() > 2)
			{
				param3 = arguments[1].floatValue();
				param4 = arguments[2].floatValue();
			}
			else
				param3 = param4 = param1;

			AddAction(SCALE, NULL, param1, false, param3, param4, 0.0f, 0.0f, NULL, NULL);
			return true;
		}
	case RGF_SM_SETSCALE:
		{
			PARMCHECK(1);
			param1 = arguments[0].floatValue();

			if(arguments.entries() > 2)
			{
				param3 = arguments[1].floatValue();
				param4 = arguments[2].floatValue();
			}
			else
				param3 = param4 = param1;

			AddAction(SETSCALE, NULL, param1, false, param3, param4, 0.0f, 0.0f, NULL, NULL);
			return true;
		}
	case RGF_SM_FIREPROJECTILE:
		{
			PARMCHECK(7);
			strcpy(param0, arguments[0].str());
			param1 = (float)strlen(param0);
			strcat(param0, arguments[5].str());
			AddAction(FIREPROJECTILE, param0, param1, false,
				arguments[2].floatValue(), arguments[3].floatValue(), arguments[4].floatValue(),
				0.0f, arguments[6].str().c_str(), arguments[1].str().c_str());
			return true;
		}
	case RGF_SM_ADDEXPLOSION:
		{
			PARMCHECK(5);
			AddAction(ADDEFFECT, arguments[0].str().c_str(), 0.0f, false,
				arguments[2].floatValue(), arguments[3].floatValue(), arguments[4].floatValue(),
				0.0f, NULL, arguments[1].str().c_str());
			return true;
		}
	case RGF_SM_TARGETGROUP:
		{
			PARMCHECK(1);
			AddAction(TARGETGROUP, arguments[0].str().c_str(), 0.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
			return true;
		}
	case RGF_SM_TESTDAMAGEORDER:
		{
			PARMCHECK(2);
			AddAction(TESTDAMAGEORDER, arguments[1].str().c_str(), arguments[0].floatValue(), false, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
			return true;
		}
	case RGF_SM_SETLODDISTANCE:
		{
			PARMCHECK(5);
			AddAction(SETLODDISTANCE, NULL, arguments[0].floatValue(), false, arguments[1].floatValue(),
				arguments[2].floatValue(), arguments[3].floatValue(), arguments[4].floatValue(), NULL, NULL);
			return true;
		}
	case RGF_SM_ATTACHTOACTOR:
		{
			float x0,y0,z0,x1,y1,z1;
			PARMCHECK(9);
			strcpy(param0, arguments[0].str());
			param3 = (float)strlen(param0);
			strcpy(param7, arguments[1].str());
			param1 = (float)strlen(param7);
			strcat(param0, param7);
			strcpy(param7, arguments[2].str());
			strcat(param0, param7);
			x0 = arguments[3].floatValue();
			y0 = arguments[4].floatValue();
			z0 = arguments[5].floatValue();
			x1 = arguments[6].floatValue();
			y1 = arguments[7].floatValue();
			z1 = arguments[8].floatValue();
			sprintf(param7,"%f %f %f %f %f %f", x0,y0,z0,x1,y1,z1);
			AddAction(ATTACHTOACTOR, param0, param3, false, param1, 0.0f, 0.0f, 0.0f, NULL, param7);
			return true;
		}
	case RGF_SM_DETACHFROMACTOR:
		{
			PARMCHECK(1);
			AddAction(DETACHFROMACTOR, arguments[0].str().c_str(), 0.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
			return true;
		}
	case RGF_SM_ATTACHBLENDACTOR:
		{
			PARMCHECK(1);

			if(arguments.entries() > 1)
			{
				AddAction(ATTACHBLENDACTOR, arguments[0].str().c_str(), 0.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, NULL, arguments[1].str().c_str());
			}
			else
			{
				AddAction(ATTACHBLENDACTOR, arguments[0].str().c_str(), 0.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
			}
			return true;
		}
	case RGF_SM_DETACHBLENDACTOR:
		{
			PARMCHECK(1);

			if(arguments.entries() > 1)
			{
				AddAction(DETACHBLENDACTOR, arguments[0].str().c_str(), 0.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, NULL, arguments[1].str().c_str());
			}
			else
			{
				AddAction(DETACHBLENDACTOR, arguments[0].str().c_str(), 0.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
			}
			return true;
		}
	case RGF_SM_ATTACHACCESSORY:
		{
			PARMCHECK(1);

			if(arguments.entries() > 1)
			{
				AddAction(ATTACHACCESSORY, arguments[0].str().c_str(), 0.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, NULL, arguments[1].str().c_str());
			}
			else
			{
				AddAction(ATTACHACCESSORY, arguments[0].str().c_str(), 0.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
			}
			return true;
		}
	case RGF_SM_DETACHACCESSORY:
		{
			PARMCHECK(1);

			if(arguments.entries() > 1)
			{
				AddAction(DETACHACCESSORY, arguments[0].str().c_str(), 0.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, NULL, arguments[1].str().c_str());
			}
			else
			{
				AddAction(DETACHACCESSORY, arguments[0].str().c_str(), 0.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
			}
			return true;
		}
	case RGF_SM_SETWEAPON:
		{
			PARMCHECK(1);
			AddAction(SETWEAPON, arguments[0].str().c_str(), 0.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
			return true;
		}
	case RGF_SM_REMOVEWEAPON:
		{
			AddAction(REMOVEWEAPON, NULL, 0.0f, false, 0.0f, 0.0f, 0.0f, 0.0f, NULL, NULL);
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
				strcpy(CCD->Pawns()->TextMessage[Nr].EntityName,	arguments[1].str());
				strcpy(CCD->Pawns()->TextMessage[Nr].AnimName,		arguments[2].str());

				CCD->Pawns()->TextMessage[Nr].TextString =			arguments[3].str();
				Replace(CCD->Pawns()->TextMessage[Nr].TextString, "<Player>", CCD->Player()->GetPlayerName());

				CCD->Pawns()->TextMessage[Nr].FontNr =				arguments[4].intValue();
				strcpy(CCD->Pawns()->TextMessage[Nr].TextSound,		arguments[6].str());
				CCD->Pawns()->TextMessage[Nr].ScreenOffsetX =		arguments[7].intValue();
				CCD->Pawns()->TextMessage[Nr].ScreenOffsetY =		arguments[8].intValue();
				strncpy(&(CCD->Pawns()->TextMessage[Nr].Alignment), arguments[9].str(), 1);
				CCD->Pawns()->TextMessage[Nr].Alpha =				arguments[10].floatValue();

				AddAction(SHOWTEXTDELAY, arguments[2].str().c_str(), arguments[5].floatValue(),
					true, float(Nr), 0.0f, 0.0f, 0.0f, arguments[6].str().c_str(), NULL);
			}
			else
			{
				AddAction(SHOWTEXT, NULL, 0.0f, false, float(Nr), 0.0f, 0.0f, 0.0f, NULL, NULL);
			}

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
				strcpy(CCD->Pawns()->TextMessage[Nr].EntityName,	arguments[1].str());
				strcpy(CCD->Pawns()->TextMessage[Nr].AnimName,		arguments[2].str());
				CCD->Pawns()->TextMessage[Nr].TextString =			arguments[3].str();
				Replace(CCD->Pawns()->TextMessage[Nr].TextString, "<Player>", CCD->Player()->GetPlayerName());

				CCD->Pawns()->TextMessage[Nr].FontNr =				arguments[4].intValue();
				strcpy(CCD->Pawns()->TextMessage[Nr].TextSound,		arguments[5].str());
				CCD->Pawns()->TextMessage[Nr].ScreenOffsetX =		arguments[6].intValue();
				CCD->Pawns()->TextMessage[Nr].ScreenOffsetY =		arguments[7].intValue();
				strncpy(&(CCD->Pawns()->TextMessage[Nr].Alignment), arguments[8].str(), 1);
				CCD->Pawns()->TextMessage[Nr].Alpha =				arguments[9].floatValue();

				AddAction(SHOWTEXT, arguments[2].str().c_str(), 0.0f,
					true, float(Nr), 0.0f, 0.0f, 0.0f, arguments[5].str().c_str(), NULL);
			}
			else
			{
				AddAction(SHOWTEXT, NULL, 0.0f, false, float(Nr), 0.0f, 0.0f, 0.0f, NULL, NULL);
			}

			return true;
		}
	case RGF_SM_REMOVETEXT:
		{
			int Nr = arguments[0].intValue();

			if(Nr < 0 || Nr >= MAXTEXT)
				return true;

			AddAction(SHOWTEXT, NULL, 0.0f, false, float(Nr), 0.0f, 0.0f, 0.0f, NULL, NULL);

			return true;
		}
	case RGF_SM_GETCONVREPLYNR:
		{
			// GetConvReplyNr: returns the number of the last reply message choosen from a conversation
			returnValue = Converse->LastReplyNr;
			return true;
		}
	case RGF_SM_CONCAT:
		{
			// Concatenate string elements to 1 new string
			PARMCHECK(2);

			strcpy(param0, arguments[0].str());
			strcat(param0, arguments[1].str());

			if(arguments.entries() > 2)
			{
				for(int i=2; i<int(arguments.entries()); i++)
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
				strcpy(param7, arguments[1].str());

				if(!stricmp(param7, "Player"))
				{
					theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
				}
				else
				{
					theInv = CCD->ActorManager()->Inventory(CCD->ActorManager()->GetByEntityName(param7));
				}
			}
			else
			{
				theInv = CCD->ActorManager()->Inventory(Actor);
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
				strcpy(param7, arguments[2].str());

				if(!stricmp(param7, "Player"))
				{
					theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
				}
				else
				{
					theInv = CCD->ActorManager()->Inventory(CCD->ActorManager()->GetByEntityName(param7));
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
				strcpy(param7, arguments[2].str());

				if(!stricmp(param7, "Player"))
				{
					theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
				}
				else
				{
					theInv = CCD->ActorManager()->Inventory(CCD->ActorManager()->GetByEntityName(param7));
				}
			}
			else
			{
				theInv = CCD->ActorManager()->Inventory(Actor);
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

					return true;
				}
				else
				{
					theInv = CCD->ActorManager()->Inventory(CCD->ActorManager()->GetByEntityName(entity));
				}
			}
			else
			{
				theInv = CCD->ActorManager()->Inventory(Actor);
			}

			returnValue = theInv->Add(param0);

			int low  = (arguments.entries() > 1 && arguments[1].type() == skRValue::T_Int) ? arguments[1].intValue() : 0;
			int high = (arguments.entries() > 2) ? arguments[2].intValue() : 100;

			theInv->SetValueLimits(param0, low, high);

			return true;
		}
	case RGF_SM_SETATTRIBUTEVALUELIMITS:
		{
			// USAGE:	SetAttributeValueLimits(char* Attribute, int LowValue, int HighValue),
			//			SetAttributeValueLimits(char* Attribute, int LowValue, int HighValue, char* EntityName)

			PARMCHECK(3);

			CPersistentAttributes *theInv;

			if(arguments.entries() > 3)
			{
				strcpy(param7, arguments[3].str());

				if(!stricmp(param7, "Player"))
				{
					theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
				}
				else
				{
					theInv = CCD->ActorManager()->Inventory(CCD->ActorManager()->GetByEntityName(param7));
				}
			}
			else
			{
				theInv = CCD->ActorManager()->Inventory(Actor);
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
			int temp = arguments[0].intValue();

			if(temp >= 0 && temp < MAXFLAGS)
				returnValue = CCD->Pawns()->GetPawnFlag(temp);

			return true;
		}
	case RGF_SM_MOUSECONTROLLEDPLAYER:
		{
			// MouseControlledPlayer(true/false)
			CCD->SetMouseControl(arguments[0].boolValue());
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
			alive=false;
			return true;
		}
	default:
		{
			return skScriptedExecutable::method(methodName, arguments, returnValue, ctxt); // change simkin
		}
	}
}


/* ------------------------------------------------------------------------------------ */
//	Push
/* ------------------------------------------------------------------------------------ */
void ScriptedObject::Push()
{
	ActionStack	*pool;

	pool = GE_RAM_ALLOCATE_STRUCT(ActionStack);
	memset(pool, 0x0, sizeof(ActionStack));
	pool->next = Stack;
	Stack = pool;

	if(pool->next)
		pool->next->prev = pool;

	pool->Top		= Top;
	pool->Bottom	= Bottom;
	pool->Index		= Index;

	memcpy(pool->Order,		Order,		64);
	memcpy(pool->Point,		Point,		64);
	memcpy(pool->NextOrder, NextOrder,	64);
	memcpy(pool->DistOrder, DistOrder,	64);

	pool->CurrentPoint	= CurrentPoint;
	pool->ActionActive	= ActionActive;
	pool->ValidPoint	= ValidPoint;
	pool->Vec2Point		= Vec2Point;
	pool->DistActive	= DistActive;
	pool->MinDistance	= MinDistance;

	Top		= NULL;
	Bottom	= NULL;
	Index	= NULL;
}

/* ------------------------------------------------------------------------------------ */
//	Pop
/* ------------------------------------------------------------------------------------ */
void ScriptedObject::Pop()
{
	if(Stack)
	{
		ActionList *pool, *temp;
		pool = Bottom;

		while(pool != NULL)
		{
			temp = pool->next;
			geRam_Free(pool);
			pool = temp;
		}

		Bottom	= Stack->Bottom;
		Top		= Stack->Top;
		Index	= Stack->Index;

		memcpy(Order,		Stack->Order,		64);
		memcpy(Point,		Stack->Point,		64);
		memcpy(NextOrder,	Stack->NextOrder,	64);
		memcpy(DistOrder,	Stack->DistOrder,	64);

		CurrentPoint	= Stack->CurrentPoint;
		ActionActive	= Stack->ActionActive;
		ValidPoint		= Stack->ValidPoint;
		Vec2Point		= Stack->Vec2Point;
		DistActive		= Stack->DistActive;
		MinDistance		= Stack->MinDistance;

		ActionStack *stemp;
		stemp = Stack->next;
		geRam_Free(Stack);
		Stack = stemp;

		if(Stack)
			Stack->prev = NULL;
	}
}

/* ------------------------------------------------------------------------------------ */
//	RemoveTriggerStack
/* ------------------------------------------------------------------------------------ */
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

/* ------------------------------------------------------------------------------------ */
//	AddAction
/* ------------------------------------------------------------------------------------ */
void ScriptedObject::AddAction(int Action, const char *AnimName, float Speed, bool Flag,
							   float Value1, float Value2, float Value3, float Value4,
							   const char *Sound, const char *Trigger)
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
	{
		strcpy(pool->SoundName, Sound);
	}

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

/* ------------------------------------------------------------------------------------ */
//	AddEvent
/* ------------------------------------------------------------------------------------ */
void CPawn::AddEvent(const char *Event, bool State)
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

/* ------------------------------------------------------------------------------------ */
//	GetEventState
/* ------------------------------------------------------------------------------------ */
bool CPawn::GetEventState(const char *Event)
{
	EventStack *pool, *temp;
	pool = Events;

	while(pool != NULL)
	{
		temp = pool->next;

		if(!strcmp(pool->EventName, Event))
			return pool->State;

		pool = temp;
	}

	return false;
}

/* ------------------------------------------------------------------------------------ */
//	CanSee
/* ------------------------------------------------------------------------------------ */
bool CPawn::CanSee(float FOV, const geActor *Actor, const geActor *TargetActor, const char *Bone)
{
	geVec3d Pos, TgtPos, temp, In;
	float dotProduct;

	CCD->ActorManager()->GetPosition(Actor, &Pos);
	CCD->ActorManager()->InVector(Actor, &In);

	if(!EffectC_IsStringNull(Bone))
	{
		geXForm3d Xf;

		if(geActor_GetBoneTransform(Actor, Bone, &Xf) == GE_TRUE)
		{
			geVec3d_Copy(&(Xf.Translation), &Pos);
			geXForm3d_GetIn(&Xf, &In);
		}
	}

	CCD->ActorManager()->GetPosition(TargetActor, &TgtPos);
	geVec3d_Subtract(&TgtPos, &Pos, &temp);
	geVec3d_Normalize(&temp);
	dotProduct = geVec3d_DotProduct(&temp, &In);

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
bool CPawn::CanSeePoint(float FOV, const geActor *Actor, const geVec3d *TargetPoint, const char *Bone)
{
	geVec3d Pos, temp, In;
	float dotProduct;

	CCD->ActorManager()->GetPosition(Actor, &Pos);
	CCD->ActorManager()->InVector(Actor, &In);

	if(!EffectC_IsStringNull(Bone))
	{
		geXForm3d Xf;

		if(geActor_GetBoneTransform(Actor, Bone, &Xf) == GE_TRUE)
		{
			geVec3d_Copy(&(Xf.Translation), &Pos);
			geXForm3d_GetIn(&Xf, &In);
		}
	}

	geVec3d_Subtract(TargetPoint, &Pos, &temp);
	geVec3d_Normalize(&temp);
	dotProduct = geVec3d_DotProduct(&temp, &In);

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
bool CPawn::PlayerDistance(float FOV, float distance, const geActor *Actor,
						   const geVec3d &DeadPos, const char *Bone)
{
	bool flg = false;
	bool fov = true;

	if(FOV > -2.0f && Actor)
		fov = CanSee(FOV, Actor, CCD->Player()->GetActor(), Bone);

	if(fov)
	{
		if(distance >= 0.0f)
		{
			if(Actor)
				flg = CCD->ActorManager()->DistanceFrom(Actor, CCD->Player()->GetActor()) < distance;
			else
				flg = CCD->ActorManager()->DistanceFrom(DeadPos, CCD->Player()->GetActor()) < distance;
		}
		else
		{
			if(Actor)
				flg = CCD->ActorManager()->DistanceFrom(Actor, CCD->Player()->GetActor()) > fabs(distance);
			else
				flg = CCD->ActorManager()->DistanceFrom(DeadPos, CCD->Player()->GetActor()) > fabs(distance);
		}
	}

	return flg;
}

/* ------------------------------------------------------------------------------------ */
//	AreaOrder
//
//	detects if Entity1 is within:
//	if DistanceMode=true: 2 distances defined and within 2 screen widths (from entity2)
//	if DistanceMode=false: 2 screen heights and within 2 screen widths (from entity2)
/* ------------------------------------------------------------------------------------ */
bool CPawn::Area(const char *FromActorName, const char *ToActorName, bool DistanceMode,
				 float MinScr, float MaxScr, float MinDist, float MaxDist,
				 bool IgnoreX, bool IgnoreY, bool IgnoreZ)
{
	geVec3d Pos, ToPos, ScreenPos;
	RECT client;
	POINT RectPos;
	POINT MousePos;
	bool flg = false;
	float Distance;
	int ScreenX, ScreenY;

	if(!stricmp(FromActorName, "Mouse"))
	{
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

		GetCursorPos(&MousePos);
		CCD->MenuManager()->ShowCursor(true);
		ScreenX = (int)MousePos.x - RectPos.x;
		ScreenY = (int)MousePos.y - RectPos.y;
		Pos.X = (float)ScreenX;
		Pos.Y = (float)ScreenY;
		Pos.Z = 0.0f;
	}
	else
	{
		if(!stricmp(FromActorName, "Player"))
			Pos = CCD->Player()->Position();
		else if(!stricmp(FromActorName, "Camera"))
			CCD->CameraManager()->GetPosition(&Pos);
		else
			CCD->ActorManager()->GetPosition(CCD->ActorManager()->GetByEntityName(FromActorName),&Pos);

		geCamera_TransformAndProject(CCD->CameraManager()->Camera(), &Pos, &ScreenPos);
		ScreenX = (int)ScreenPos.X;
		ScreenY = (int)ScreenPos.Y;
	}

	if(!strcmp(ToActorName, ""))
		DistanceMode = false;
	else if(!stricmp(ToActorName, "Player"))
		ToPos = CCD->Player()->Position();
	else
		CCD->ActorManager()->GetPosition(CCD->ActorManager()->GetByEntityName(ToActorName), &ToPos);

	if(!stricmp(FromActorName, "Mouse")) //convert it to screen coordinates
	{
		geCamera_TransformAndProject(CCD->CameraManager()->Camera(), &ToPos, &ToPos);
		ToPos.Z = 0.0f;
	}

	if(IgnoreX)
		Pos.X = ToPos.X;
	if(IgnoreY)
		Pos.Y = ToPos.Y;
	if(IgnoreZ)
		Pos.Z = ToPos.Z;

	Distance = geVec3d_DistanceBetween(&Pos, &ToPos);

	if(DistanceMode)
		flg = ((Distance >= MinDist) && (Distance <= MaxDist) && (ScreenX >= MinScr) && (ScreenX <= MaxScr));
	else
		flg = ((ScreenY >= MinDist) && (ScreenY <= MaxDist) && (ScreenX >= MinScr) && (ScreenX <= MaxScr));

	if(flg)
		return true;
	else
		return false;
}

/* ------------------------------------------------------------------------------------ */
//	RotateToPoint
/* ------------------------------------------------------------------------------------ */
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
		Orient.X = GE_PIOVER2 - (float)acos(Orient.Y / l);
		Orient.Y = (float)atan2(x, Orient.Z) + GE_PI;
	}
	{
		CCD->ActorManager()->GetRotate(Object->Actor, &Pos);


		while(Pos.Y < 0.0f)		{ Pos.Y += GE_2PI; }
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
			float amount = Object->Index->Speed * (dwTicks*0.001f); // /1000.0f);

			if(amount>RotateAmt)
			{
				amount = RotateAmt;
			}

			RotateAmt -= amount;

			if(RotateLeft)
				CCD->ActorManager()->TurnRight(Object->Actor, 0.0174532925199433f*amount);
			else
				CCD->ActorManager()->TurnLeft(Object->Actor, 0.0174532925199433f*amount);
		}

		if(Object->Index->Flag)
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
				float amount = Object->Index->Speed * (dwTicks*0.001f);// /1000.0f);

				if(amount > TiltAmt)
				{
					amount = TiltAmt;
				}

				TiltAmt -= amount;

				if(TiltUp)
					CCD->ActorManager()->TiltUp(Object->Actor, 0.0174532925199433f*amount);
				else
					CCD->ActorManager()->TiltDown(Object->Actor, 0.0174532925199433f*amount);
			}
		}

		if(RotateAmt <= 0.0f && TiltAmt <= 0.0f)
			return true;

		return false;
	}

	return true;
}

/* ------------------------------------------------------------------------------------ */
//	RotateAroundPoint
/* ------------------------------------------------------------------------------------ */
bool CPawn::RotateAroundPoint(void *Data, float dwTicks)
{
	ScriptedObject *Object = (ScriptedObject *)Data;

	if(Object->StartAction)
	{
		if(!EffectC_IsStringNull(Object->Index->AnimName))
			CCD->ActorManager()->SetMotion(Object->Actor, Object->Index->AnimName);

		Object->StartAction = false;
		Object->Circle = CCD->ActorManager()->DistanceFrom(Object->CurrentPoint, Object->Actor);
	}

	geVec3d Pos1;
	CCD->ActorManager()->GetPosition(Object->Actor, &Pos1);
	geVec3d LookRotation;
	geFloat x,l;
	geVec3d_Subtract(&Object->CurrentPoint, &Pos1, &LookRotation);
	l = geVec3d_Length(&LookRotation);

	// protect from Div by Zero
	if(l > 0.0f)
	{
		x = LookRotation.X;
		LookRotation.X = -(GE_PIOVER2 - (float)acos(LookRotation.Y / l));
		LookRotation.Y = (float)atan2(x, LookRotation.Z) + GE_PI;
		LookRotation.Z = 0.0;	// roll is zero - always!!?


		LookRotation.X = 0.0f;
		CCD->ActorManager()->Rotate(Object->Actor, LookRotation);
	}

	Object->Index->Value2 -= (dwTicks*0.001f);// /1000.0f);

	if(Object->Index->Value2 <= 0.0f)
		return true;

	float distance = Object->Index->Speed * (dwTicks*0.001f);// /1000.0f);

	if(Object->Index->Value1 != 0.0f)
		distance = -distance;

	if(CCD->ActorManager()->MoveLeft(Object->Actor, distance) != RGF_SUCCESS)
		return true;

	float Circle = CCD->ActorManager()->DistanceFrom(Object->CurrentPoint, Object->Actor);

	if(Circle != Object->Circle)
	{
		geVec3d In;
		CCD->ActorManager()->InVector(Object->Actor, &In);
		float Amount = Circle - Object->Circle;

		if(CCD->ActorManager()->MoveForward(Object->Actor, Amount) != RGF_SUCCESS)
			return true;
	}

	return false;
}

/* ------------------------------------------------------------------------------------ */
//	MoveToPoint
/* ------------------------------------------------------------------------------------ */
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
			Orient.X = GE_PIOVER2 - (float)acos(Orient.Y / l);
			Orient.Y = (float)atan2(x, Orient.Z) + GE_PI;
			Orient.Z = 0.0f;	// roll is zero - always!!?

			geXForm3d_SetXRotation(&Xf, Orient.X);
			geXForm3d_RotateY(&Xf, Orient.Y);
			geXForm3d_GetIn(&Xf, &(Object->TempPoint));
		}
		else
			return true;
	}

	CCD->ActorManager()->GetPosition(Object->Actor, &Pos);
	float distance = Object->Index->Speed * (dwTicks*0.001f);// /1000.0f);

	if(fabs(Pos.X-Object->CurrentPoint.X) < distance && fabs(Pos.Z-Object->CurrentPoint.Z) < distance)
		return true;

	geVec3d_AddScaled(&Pos, &(Object->TempPoint), distance, &NewPos);

	bool result = CCD->ActorManager()->ValidateMove(Pos, NewPos, Object->Actor, false);

	if(!result)
		Object->AvoidMode = true;

	return false;
}

/* ------------------------------------------------------------------------------------ */
//	RotateToAlign
/* ------------------------------------------------------------------------------------ */
bool CPawn::RotateToAlign(void *Data, float dwTicks)
{
	geVec3d Pos, Orient;
	ScriptedObject *Object = (ScriptedObject*)Data;

	if(Object->StartAction)
	{
		if(!EffectC_IsStringNull(Object->Index->AnimName))
			CCD->ActorManager()->SetMotion(Object->Actor, Object->Index->AnimName);

		Object->StartAction = false;
		const char *EntityType = CCD->EntityRegistry()->GetEntityType(Object->Point);

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
				geXForm3d_SetZRotation(&Xf, Orient.Z);
				geXForm3d_RotateX(&Xf, Orient.X);
				geXForm3d_RotateY(&Xf, Orient.Y);

				geXForm3d_GetIn(&Xf, &Orient);
				geVec3d_Add(&Pos, &Orient, &(Object->TempPoint));
			}
		}
	}

	CCD->ActorManager()->GetPosition(Object->Actor, &Pos);
	geVec3d_Subtract(&Object->TempPoint, &Pos, &Orient);
	float l = geVec3d_Length(&Orient);
	float RotateAmt = 0.0f;
	float TiltAmt = 0.0f;

	if(l > 0.0f)
	{
		float x = Orient.X;
		Orient.X = GE_PIOVER2 - (float)acos(Orient.Y / l);
		Orient.Y = (float)atan2(x, Orient.Z) + GE_PI;
		CCD->ActorManager()->GetRotate(Object->Actor, &Pos);

		while(Pos.Y < 0.0f)
		{
			Pos.Y += GE_2PI;
		}

		while(Pos.Y >= GE_2PI)
		{
			Pos.Y -= GE_2PI;
		}

		bool RotateLeft = false;
		RotateAmt = Pos.Y - Orient.Y;

		if(RotateAmt < 0.0f)
		{
			RotateAmt = -RotateAmt;
			RotateLeft = true;
		}

		if(RotateAmt > 0.f) //-2.f*GE_PI)
		{
			if(RotateAmt > GE_PI)
			{
				RotateAmt = GE_2PI - RotateAmt; //-= GE_PI;

				if(RotateLeft)
					RotateLeft = false;
				else
					RotateLeft = true;
			}

			RotateAmt *= GE_180OVERPI; // /= 0.0174532925199433f;
			float amount = Object->Index->Speed * (dwTicks*0.001f);// /1000.0f);

			if(amount>RotateAmt)
			{
				amount = RotateAmt;
			}

			RotateAmt -= amount;

			if(RotateLeft)
				CCD->ActorManager()->TurnRight(Object->Actor, 0.0174532925199433f*amount);
			else
				CCD->ActorManager()->TurnLeft(Object->Actor, 0.0174532925199433f*amount);
		}

		if(Object->Index->Flag)
		{
			while(Pos.X < 0.0f)
			{
				Pos.X += GE_2PI;
			}

			while(Pos.X >= GE_2PI)
			{
				Pos.X -= GE_2PI;
			}

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
					TiltAmt = GE_2PI - TiltAmt;//-= GE_PI;

					if(TiltUp)
						TiltUp = false;
					else
						TiltUp = true;
				}

				TiltAmt *= GE_180OVERPI; // /= 0.0174532925199433f;
				float amount = Object->Index->Speed * (dwTicks*0.001f); // /1000.0f);

				if(amount > TiltAmt)
				{
					amount = TiltAmt;
				}

				TiltAmt -= amount;

				if(TiltUp)
					CCD->ActorManager()->TiltUp(Object->Actor, 0.0174532925199433f*amount);
				else
					CCD->ActorManager()->TiltDown(Object->Actor, 0.0174532925199433f*amount);
			}
		}

		if(RotateAmt <= 0.0f && TiltAmt <= 0.0f)
			return true;

		return false;
	}

	return true;
}

/* ------------------------------------------------------------------------------------ */
//	NextPoint
/* ------------------------------------------------------------------------------------ */
bool CPawn::NextPoint(void *Data, float dwTicks)
{
	ScriptedObject *Object = (ScriptedObject *)Data;

	const char *EntityType = CCD->EntityRegistry()->GetEntityType(Object->Point);

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
					const char *EntityType = CCD->EntityRegistry()->GetEntityType(Object->Point);

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

/* ------------------------------------------------------------------------------------ */
//	NextOrder
/* ------------------------------------------------------------------------------------ */
bool CPawn::NextOrder(void *Data, float dwTicks)
{
	ScriptedObject *Object = (ScriptedObject *)Data;

	if(!EffectC_IsStringNull(Object->NextOrder))
	{
		strcpy(Object->Order, Object->NextOrder);
		Object->RunOrder = true;
		ActionList *pool, *temp;
		pool = Object->Bottom;

		while(pool != NULL)
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

/* ------------------------------------------------------------------------------------ */
//	Rotate
/* ------------------------------------------------------------------------------------ */
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
	float baseamount = Object->Index->Speed * (dwTicks*0.001f);// /1000.0f);

	if(Object->Index->Value1 != 0.0f)
	{
		float amount = baseamount;

		if(Object->Index->Value1 < 0.0f)
		{
			amount = -amount;

			if(amount < Object->Index->Value1)
				amount = Object->Index->Value1;
		}
		else
		{
			if(amount > Object->Index->Value1)
				amount = Object->Index->Value1;
		}

		Object->Index->Value1 -= amount;
		amount *= 0.0174532925199433f;
		Object->TempPoint.X += amount;
		flag = false;
	}

	if(Object->Index->Value2 != 0.0f)
	{
		float amount = baseamount;

		if(Object->Index->Value2 < 0.0f)
		{
			amount = -amount;

			if(amount < Object->Index->Value2)
				amount = Object->Index->Value2;
		}
		else
		{
			if(amount > Object->Index->Value2)
				amount = Object->Index->Value2;
		}

		Object->Index->Value2 -= amount;
		amount *= 0.0174532925199433f;
		Object->TempPoint.Y += amount;
		flag = false;
	}

	if(Object->Index->Value3 != 0.0f)
	{
		float amount = baseamount;

		if(Object->Index->Value3 < 0.0f)
		{
			amount = -amount;

			if(amount < Object->Index->Value3)
				amount = Object->Index->Value3;
		}
		else
		{
			if(amount > Object->Index->Value3)
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

/* ------------------------------------------------------------------------------------ */
//	RotateMoveToPoint
/* ------------------------------------------------------------------------------------ */
bool CPawn::RotateMoveToPoint(void *Data, float dwTicks)
{
	ScriptedObject *Object = (ScriptedObject *)Data;
	geVec3d Pos, NewPos, In;

	if(RotateToPoint(Data, dwTicks))
		return true;

	CCD->ActorManager()->GetPosition(Object->Actor, &Pos);
	float distance = Object->Index->Value1 * (dwTicks*0.001f); // /1000.0f);
	CCD->ActorManager()->InVector(Object->Actor, &In);
	geVec3d_AddScaled(&Pos, &In, distance, &NewPos);

	bool result = CCD->ActorManager()->ValidateMove(Pos, NewPos, Object->Actor, false);
	return false;
}

/* ------------------------------------------------------------------------------------ */
//	NextPath
/* ------------------------------------------------------------------------------------ */
bool CPawn::NextPath(void *Data, float dwTicks)
{
	ScriptedObject *Object = (ScriptedObject *)Data;

	const char *EntityType = CCD->EntityRegistry()->GetEntityType(Object->Index->AnimName);

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

				while(pool != NULL)
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

/* ------------------------------------------------------------------------------------ */
//	RotateMove
/* ------------------------------------------------------------------------------------ */
bool CPawn::RotateMove(void *Data, float dwTicks)
{
	ScriptedObject *Object = (ScriptedObject *)Data;
	geVec3d Pos, NewPos, In;

	if(Rotate(Data, dwTicks))
		return true;

	CCD->ActorManager()->GetPosition(Object->Actor, &Pos);
	float distance = Object->Index->Value4 * (dwTicks*0.001f);// /1000.0f);
	CCD->ActorManager()->InVector(Object->Actor, &In);
	geVec3d_AddScaled(&Pos, &In, distance, &NewPos);

	bool result = CCD->ActorManager()->ValidateMove(Pos, NewPos, Object->Actor, false);
	return false;
}

/* ------------------------------------------------------------------------------------ */
//	Move
/* ------------------------------------------------------------------------------------ */
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

		if(Object->Direction == RGF_K_BACKWARD || Object->Direction == RGF_K_FORWARD)
			CCD->ActorManager()->InVector(Object->Actor, &Object->Vec2Point);
		else
		{
			if(Object->Direction != 0)
				CCD->ActorManager()->LeftVector(Object->Actor, &Object->Vec2Point);
			else
			{
				geXForm3d thePosition;
				geVec3d Rotate;
				CCD->ActorManager()->GetRotate(Object->Actor, &Rotate);

				geXForm3d_SetZRotation(&thePosition,Rotate.Z+Object->Index->Value4*0.0174532925199433f);
				geXForm3d_RotateX(&thePosition,		Rotate.X+Object->Index->Value2*0.0174532925199433f);
				geXForm3d_RotateY(&thePosition,		Rotate.Y+Object->Index->Value3*0.0174532925199433f);
				geXForm3d_GetIn(&thePosition, &(Object->Vec2Point));
			}
		}
	}

	CCD->ActorManager()->GetPosition(Object->Actor, &Pos);
	float distance = Object->Index->Speed * (dwTicks*0.001f); // /1000.0f);

	if((distance+Object->TotalDist) >= Object->Index->Value1)
	{
		distance = Object->Index->Value1 - Object->TotalDist;
		retflg = true;
	}

	Object->TotalDist += distance;

	if(Object->Direction == RGF_K_BACKWARD || Object->Direction == RGF_K_RIGHT)
		distance = -distance;

	geVec3d_AddScaled(&Pos, &Object->Vec2Point, distance, &NewPos);

	bool result = CCD->ActorManager()->ValidateMove(Pos, NewPos, Object->Actor, false);

	if(!result)
		return true;

	return retflg;
}

/* ------------------------------------------------------------------------------------ */
//	Jump
/* ------------------------------------------------------------------------------------ */
bool CPawn::Jump(void *Data, float dwTicks)
{
	ScriptedObject *Object = (ScriptedObject*)Data;

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

/* ------------------------------------------------------------------------------------ */
//	AddTriggerOrder
/* ------------------------------------------------------------------------------------ */
bool CPawn::AddTriggerOrder(void *Data, float dwTicks)
{
	ScriptedObject *Object = (ScriptedObject *)Data;

	if(!EffectC_IsStringNull(Object->Index->AnimName))
	{
		if(!EffectC_IsStringNull(Object->Index->TriggerName))
		{
			TriggerStack *tpool, *ttemp;
			tpool = Object->Trigger;

			while(tpool != NULL)
			{
				ttemp = tpool->next;

				if(!strcmp(Object->Index->TriggerName, tpool->TriggerName))
				{
					strcpy(tpool->OrderName, Object->Index->AnimName);
					tpool->Delay = Object->Index->Speed;
					tpool->Flag = Object->Index->Flag;
					tpool->Type = (int)Object->Index->Value1;
					tpool->PFlg = (int)Object->Index->Value2;
					tpool->Low = Object->Index->Value3;
					tpool->High = Object->Index->Value4;
					tpool->Time = Object->Index->Value3;
					return true;
				}

				tpool = ttemp;
			}

			tpool = GE_RAM_ALLOCATE_STRUCT(TriggerStack);
			memset(tpool, 0x0, sizeof(TriggerStack));
			tpool->next = Object->Trigger;
			tpool->prev = NULL;
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
			tpool->Low = Object->Index->Value3;
			tpool->High = Object->Index->Value4;
		}
	}

	return true;
}

/* ------------------------------------------------------------------------------------ */
//	DelTriggerOrder
/* ------------------------------------------------------------------------------------ */
bool CPawn::DelTriggerOrder(void *Data, float dwTicks)
{
	ScriptedObject *Object = (ScriptedObject *)Data;

	if(!EffectC_IsStringNull(Object->Index->TriggerName))
	{
		TriggerStack *tpool, *ttemp;
		tpool = Object->Trigger;

		while(tpool != NULL)
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

/* ------------------------------------------------------------------------------------ */
//	RotateToPlayer
/* ------------------------------------------------------------------------------------ */
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
		Orient.X = GE_PIOVER2 - (float)acos(Orient.Y / l);
		Orient.Y = (float)atan2(x, Orient.Z) + GE_PI;
		CCD->ActorManager()->GetRotate(Object->Actor, &Pos);


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

			float amount = Object->Index->Speed * (dwTicks*0.001f); // /1000.0f);

			if(amount > RotateAmt)
			{
				amount = RotateAmt;
			}

			RotateAmt -= amount;

			if(RotateLeft)
				CCD->ActorManager()->TurnRight(Object->Actor, 0.0174532925199433f*amount);
			else
				CCD->ActorManager()->TurnLeft(Object->Actor, 0.0174532925199433f*amount);
		}

		if(Object->Index->Flag)
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
				float amount = Object->Index->Speed * (dwTicks*0.001f);// /1000.0f);

				if(amount > TiltAmt)
				{
					amount = TiltAmt;
				}

				TiltAmt -= amount;

				if(TiltUp)
					CCD->ActorManager()->TiltUp(Object->Actor, 0.0174532925199433f*amount);
				else
					CCD->ActorManager()->TiltDown(Object->Actor, 0.0174532925199433f*amount);
			}
		}

		if(RotateAmt == 0.0f && TiltAmt == 0.0f)
			return true;

		return false;
	}

	return true;
}

/* ------------------------------------------------------------------------------------ */
//	TickHigh
/* ------------------------------------------------------------------------------------ */
void CPawn::TickHigh(Pawn *pSource, ScriptedObject *Object, float dwTicks)
{
	skRValueArray args; // change simkin
	skRValue ret;
	int yoffset = 0;
	TriggerStack *tpool, *ttemp;
	tpool = Object->Trigger;

	while(tpool!= NULL)
	{
		ttemp = tpool->next;

		if(tpool->Type == PTIMER || tpool->Type == PSOUND)
		{
			tpool->Time -= (dwTicks*0.001f);// /1000.0f);

			if(tpool->Time < 0.0f)
				tpool->Time = 0.0f;
		}

		tpool = ttemp;
	}

	if(!EffectC_IsStringNull(Object->DeadOrder))
	{
		if(!EffectC_IsStringNull(Object->Attribute))
		{
			CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(Object->Actor);

			if(theInv->Value(Object->Attribute) <= 0)
			{
				strcpy(Object->Order, Object->DeadOrder);
				Object->DeadOrder[0] = '\0';
				Object->RunOrder = true;
				Object->ActionActive = false;
				ActionList *pool, *temp;
				pool = Object->Bottom;

				while(pool != NULL)
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
				Object->TriggerTime -= (dwTicks*0.001f);// /1000.0f);

				if(Object->TriggerTime <= 0.0f && !Object->TargetDisable)
				{
					Object->TriggerWait = false;
					strcpy(Object->Order, Object->TriggerOrder);
					Object->RunOrder = true;
					Object->ActionActive = false;
					ActionList *pool, *temp;
					pool = Object->Bottom;

					while(pool != NULL)
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

					while(tpool != NULL)
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
						case PCOLLIDE:
							flag = Object->collision;
							break;
						case PSOUND:
							flag = false;
							if(tpool->Time == 0.0f)
							{
								tpool->Time=EffectC_Frand(tpool->Low, tpool->High);
								Snd Sound;

								memset( &Sound, 0, sizeof( Sound ) );
								{
									if(Object->Actor)
										CCD->ActorManager()->GetPosition(Object->Actor, &Sound.Pos);
									else
										Sound.Pos = Object->DeadPos;
								}
								Sound.Min = Object->AudibleRadius;
								Sound.Loop = GE_FALSE;
								Sound.SoundDef = SPool_Sound(tpool->OrderName);
								CCD->EffectManager()->Item_Add(EFF_SND, (void*)&Sound);
							}
							break;
						}
						if(flag)
						{
							if(tpool->Delay > 0.0f)
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

								while(pool != NULL)
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
			if(Object->PointFind)
			{
				if(Object->FOV > -2.0f && Object->Actor)
				{
					float dist = 9000000.0f;// only ScripPoints within 3000 texels
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
							ScriptPoint *pSource = (ScriptPoint*)geEntity_GetUserData(pEntity);

							// search for nearest visible ScripPoint
							if(CanSeePoint(Object->FOV, Object->Actor, &(pSource->origin), Object->FOVBone))
							{
								float newdist;
								geVec3d Pos, Dist;

								//calc newdist
								CCD->ActorManager()->GetPosition(Object->Actor, &Pos);
								geVec3d_Subtract(&pSource->origin, &Pos, &Dist);
								newdist = geVec3d_LengthSquared(&Dist);

								if(newdist < dist)
								{
									dist = newdist;
									strcpy(Object->Point, pSource->szEntityName);
									Object->CurrentPoint= pSource->origin;
									Object->ValidPoint = true;
									done = true;
								}
							}
						}
					}

					if(done)
					{
						strcpy(Object->Order, Object->PointOrder);
						Object->RunOrder = true;
						Object->ActionActive = false;
						ActionList *pool, *temp;
						pool = Object->Bottom;

						while(pool != NULL)
						{
							temp = pool->next;
							geRam_Free(pool);
							pool = temp;
						}

						Object->Top = NULL;
						Object->Bottom = NULL;
						Object->Index = NULL;
						Object->PointFind = false;
					}
				}
			}

			if(Object->TargetFind && !Object->TargetDisable)
			{
				if(Object->FOV > -2.0f && Object->Actor)
				{
					float distance = Object->TargetDistance+100.f;
					bool done = false;

					if(Object->HostilePlayer)
					{
						if(CanSee(Object->FOV, Object->Actor, CCD->Player()->GetActor(), Object->FOVBone))
						{
							float dist = CCD->ActorManager()->DistanceFrom(Object->Actor, CCD->Player()->GetActor());

							if(dist < Object->TargetDistance)
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
					int nActorCount = CCD->ActorManager()->GetActorsInRange(Pos, Object->TargetDistance,
																			512, &ActorsInRange[0]);

					if(nActorCount != 0)
					{
						if(Object->HostileDiff)
						{
							for(int nTemp=0; nTemp<nActorCount; nTemp++)
							{
								if(ActorsInRange[nTemp] == Object->Actor)
									continue;

								int nType;
								CCD->ActorManager()->GetType(ActorsInRange[nTemp], &nType);

								if((nType == ENTITY_NPC || nType == ENTITY_VEHICLE)
									&& CCD->ActorManager()->GetGroup(ActorsInRange[nTemp]))
								{
									bool TG = false;

									if(!EffectC_IsStringNull(Object->TargetGroup))
									{
										if(!strcmp(Object->TargetGroup, CCD->ActorManager()->GetGroup(ActorsInRange[nTemp])))
											TG = true;
									}
									else
									{
										if(strcmp(Object->Group, CCD->ActorManager()->GetGroup(ActorsInRange[nTemp])) != 0)
											TG = true;
									}

									if(TG)
									{
										CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(ActorsInRange[nTemp]);

										if(theInv->Has(Object->TargetAttr))
										{
											if(theInv->Value(Object->TargetAttr) > 0)
											{
												if(CanSee(Object->FOV, Object->Actor, ActorsInRange[nTemp], Object->FOVBone))
												{
													float dist = CCD->ActorManager()->DistanceFrom(Object->Actor, ActorsInRange[nTemp]);

													if(dist < Object->TargetDistance)
													{
														done = true;

														if(distance > dist)
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
							for(int nTemp=0; nTemp<nActorCount; nTemp++)
							{
								if(ActorsInRange[nTemp]==Object->Actor)
									continue;

								int nType;
								CCD->ActorManager()->GetType(ActorsInRange[nTemp], &nType);

								if((nType == ENTITY_NPC || nType == ENTITY_VEHICLE)
									&& CCD->ActorManager()->GetGroup(ActorsInRange[nTemp]))
								{
									if(!strcmp(Object->Group, CCD->ActorManager()->GetGroup(ActorsInRange[nTemp])))
									{
										CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(ActorsInRange[nTemp]);

										if(theInv->Has(Object->TargetAttr))
										{
											if(theInv->Value(Object->TargetAttr) > 0)
											{
												if(CanSee(Object->FOV, Object->Actor, ActorsInRange[nTemp], Object->FOVBone))
												{
													float dist = CCD->ActorManager()->DistanceFrom(Object->Actor, ActorsInRange[nTemp]);

													if(dist < Object->TargetDistance)
													{
														done = true;

														if(distance > dist)
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

						while(pool != NULL)
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
				bool flg = PlayerDistance(Object->FOV, Object->MinDistance, Object->Actor,
											Object->DeadPos, Object->FOVBone);

				if(flg)
				{
					strcpy(Object->Order, Object->DistOrder);
					Object->RunOrder = true;
					Object->ActionActive = false;
					ActionList *pool, *temp;
					pool = Object->Bottom;

					while(pool != NULL)
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

				if(current > 0)
				{
					if(current < Object->OldAttributeAmount)
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
					Object->method(skString(Object->Order), args, ret, ScriptManager::GetContext());
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

				if(Object->SoundIndex != -1 && !Object->SoundLoop)
				{
					CCD->EffectManager()->Item_Delete(EFF_SND, Object->SoundIndex);
					Object->SoundIndex = -1;
				}

				if(Object->Index)
				{
					if(!EffectC_IsStringNull(Object->Index->SoundName))
					{
						if(Object->SoundIndex != -1)
						{
							CCD->EffectManager()->Item_Delete(EFF_SND, Object->SoundIndex);
							Object->SoundIndex = -1;
						}

						Snd Sound;

						memset(&Sound, 0, sizeof(Sound));
						{
							if(Object->Actor)
								CCD->ActorManager()->GetPosition(Object->Actor, &Sound.Pos);
							else
								Sound.Pos = Object->DeadPos;
						}

						Sound.Min = Object->AudibleRadius;
						Sound.Loop = GE_TRUE;

						if(Object->Index->Action == STOPANIMATION)
							Sound.Loop = GE_FALSE;

						Sound.SoundDef = SPool_Sound(Object->Index->SoundName);
						Object->SoundIndex = CCD->EffectManager()->Item_Add(EFF_SND, (void*)&Sound);
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

						if(Object->SoundIndex != -1 && !Object->SoundLoop)
						{
							CCD->EffectManager()->Item_Delete(EFF_SND, Object->SoundIndex);
							Object->SoundIndex = -1;
						}

						if(!EffectC_IsStringNull(Object->Index->SoundName))
						{
							if(Object->SoundIndex != -1)
							{
								CCD->EffectManager()->Item_Delete(EFF_SND, Object->SoundIndex);
								Object->SoundIndex = -1;
							}

							Snd Sound;

							memset( &Sound, 0, sizeof( Sound ) );

							if(Object->Actor)
								CCD->ActorManager()->GetPosition(Object->Actor, &Sound.Pos);
							else
								Sound.Pos = Object->DeadPos;

							Sound.Min = Object->AudibleRadius;
							Sound.Loop = GE_TRUE;

							if(Object->Index->Action == STOPANIMATION)
								Sound.Loop = GE_FALSE;

							Sound.SoundDef = SPool_Sound(Object->Index->SoundName);
							Object->SoundIndex = CCD->EffectManager()->Item_Add(EFF_SND, (void*)&Sound);
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
						LookRotation.X = -(GE_PIOVER2 - (float)acos(LookRotation.Y / l));
						LookRotation.Y = (float)atan2(x, LookRotation.Z) + GE_PI;
						LookRotation.Z = 0.0f;	// roll is zero - always!!?

						if(!Object->FaceAxis)
							LookRotation.X = 0.0f;

						CCD->ActorManager()->Rotate(Object->Actor, LookRotation);
					}
				}

				ActionList *pool, *temp;
				geVec3d Gravity = { 0.f, 0.f, 0.f };

				switch(Object->Index->Action)
				{
				case ROTATETOPOINT:
					if(Object->ValidPoint && !Object->FacePlayer && Object->Actor)
					{
						if(RotateToPoint((void*)Object, dwTicks))
							Object->ActionActive = false;
					}
					else
						Object->ActionActive = false;
					break;
				case MOVETOPOINT:
					if(Object->ValidPoint && Object->Actor)
					{
						if(MoveToPoint((void*)Object, dwTicks))
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
					while(pool != NULL)
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
						if(NextOrder((void*)Object, dwTicks))
							Object->ActionActive = false;
					}
					else
						Object->ActionActive = false;
					runflag = true;
					break;
				case ROTATETOALIGN:
					if(Object->ValidPoint && !Object->FacePlayer && Object->Actor)
					{
						if(RotateToAlign((void*)Object, dwTicks))
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
						if(NextPoint((void*)Object, dwTicks))
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
					Object->Time += (dwTicks*0.001f);// /1000.0f);
					if(Object->Time >= Object->Index->Speed)
					{
						Object->Time = 0.0f;
						Object->ActionActive = false;
					}
					break;
				case SHOWTEXTDELAY:
					if(Object->StartAction)
					{
						Object->StartAction = false;

						if(Object->Index->Flag)
						{
							if(!EffectC_IsStringNull(Object->Index->AnimName) && Object->Actor)
								CCD->ActorManager()->SetMotion(Object->Actor, Object->Index->AnimName);

							CCD->Pawns()->TextMessage[int(Object->Index->Value1)].ShowText = true;
						}
						else
						{
							CCD->Pawns()->TextMessage[int(Object->Index->Value1)].ShowText = false;
							Object->ActionActive = false;
							break;
						}
					}
					Object->Time += (dwTicks*0.001f);
					if((Object->Time>=Object->Index->Speed) || !(Object->Index->Flag))
					{
						Object->Time = 0.0f;
						CCD->Pawns()->TextMessage[int(Object->Index->Value1)].ShowText = false;
						Object->ActionActive = false;
					}
					break;
				case SHOWTEXT:
					if(Object->StartAction)
					{
						if(Object->Index->Flag)
						{
							if(!EffectC_IsStringNull(Object->Index->AnimName) && Object->Actor)
								CCD->ActorManager()->SetMotion(Object->Actor, Object->Index->AnimName);

							CCD->Pawns()->TextMessage[int(Object->Index->Value1)].ShowText = true;
						}
						else
							CCD->Pawns()->TextMessage[int(Object->Index->Value1)].ShowText = false;

						Object->StartAction = false;
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
				case BLENDTOANIMATION:
					if(Object->Actor)
					{
						if(Object->StartAction)
						{
							if(!EffectC_IsStringNull(Object->Index->AnimName))
							{
								if(Object->Index->Speed > 0.0f)
								{
									CCD->ActorManager()->SetTransitionMotion(Object->Actor, Object->Index->AnimName, Object->Index->Speed, NULL);
									CCD->ActorManager()->SetNextMotion(Object->Actor, Object->Index->AnimName);
								}
								else
									CCD->ActorManager()->SetMotion(Object->Actor, Object->Index->AnimName);

								Object->StartAction = false;

								if(Object->Index->Flag)
									CCD->ActorManager()->SetHoldAtEnd(Object->Actor, true);
								else
									Object->ActionActive = false;
							}

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
						Object->Time += (dwTicks*0.001f);// /1000.0f);
						if(Object->Time >= Object->Index->Speed)
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
						if(RotateMoveToPoint((void*)Object, dwTicks))
							Object->ActionActive = false;
					}
					else
						Object->ActionActive = false;
					break;
				case NEWPATH:
					if(NextPath((void*)Object, dwTicks))
					{
						Object->ActionActive = false;
						runflag = true;
					}
					break;
				case RESTARTORDER:
					Object->RunOrder = true;
					Object->ActionActive = false;
					pool = Object->Bottom;
					while(pool != NULL)
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
						if(RotateMove((void*)Object, dwTicks))
							Object->ActionActive = false;
					}
					else
						Object->ActionActive = false;
					break;
				case NEWPOINT:
					if(!EffectC_IsStringNull(Object->Index->AnimName))
					{
						const char *EntityType = CCD->EntityRegistry()->GetEntityType(Object->Index->AnimName);
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
					if(Move((void*)Object, dwTicks))
						Object->ActionActive = false;
					break;
				case MOVEBACKWARD:
					Object->Direction = RGF_K_BACKWARD;
					if(Move((void*)Object, dwTicks))
						Object->ActionActive = false;
					break;
				case MOVELEFT:
					Object->Direction = RGF_K_LEFT;
					if(Move((void*)Object, dwTicks))
						Object->ActionActive = false;
					break;
				case MOVERIGHT:
					Object->Direction = RGF_K_RIGHT;
					if(Move((void*)Object, dwTicks))
						Object->ActionActive = false;
					break;
				case MOVE:
					Object->Direction = 0;
					if(Move((void*)Object, dwTicks))
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
						if(Jump((void*)Object, dwTicks))
							Object->ActionActive = false;
					}
					else
						Object->ActionActive = false;
					break;
				case ADDTRIGGERORDER:
					AddTriggerOrder((void*)Object, dwTicks);
					Object->ActionActive = false;
					runflag = true;
					break;
				case DELTRIGGERORDER:
					DelTriggerOrder((void*)Object, dwTicks);
					Object->ActionActive = false;
					runflag = true;
					break;
				case SETEVENTSTATE:
					AddEvent(Object->Index->AnimName, Object->Index->Flag);
					Object->ActionActive = false;
					break;
				case FACEPLAYER:
					Object->FacePlayer = Object->Index->Flag;
					Object->FaceAxis = true;
					if(Object->Index->Speed < 1.0f)
						Object->FaceAxis = false;
					Object->ActionActive = false;
					break;
				case ROTATETOPLAYER:
					if(!Object->FacePlayer && Object->Actor)
					{
						if(RotateToPlayer((void*)Object, dwTicks))
							Object->ActionActive = false;
					}
					else
						Object->ActionActive = false;
					break;
				case ROTATEAROUNDPOINT:
					if(Object->ValidPoint && !Object->FacePlayer && Object->Actor)
					{
						if(RotateAroundPoint((void*)Object, dwTicks))
							Object->ActionActive = false;
					}
					else
						Object->ActionActive = false;
					break;
				case TELEPORTTOPOINT:
					if(!EffectC_IsStringNull(Object->Index->AnimName) && Object->Actor)
					{
						const char *EntityType = CCD->EntityRegistry()->GetEntityType(Object->Index->AnimName);

						if(EntityType)
						{
							if(!stricmp(EntityType, "ScriptPoint"))
							{
								ScriptPoint *pProxy;
								CCD->ScriptPoints()->LocateEntity(Object->Index->AnimName, (void**)&pProxy);

								geActor *tActor;
								if(!EffectC_IsStringNull(Object->Index->TriggerName))
								{
									if(!stricmp(Object->Index->TriggerName, "Player")) // Teleport Player
										tActor = CCD->Player()->GetActor();
									else  // Teleport Pawn
										tActor = CCD->ActorManager()->GetByEntityName(Object->Index->TriggerName);
								}
								else
									tActor = Object->Actor; // Teleport Script-pawn

								geVec3d Pos = pProxy->origin;
								Pos.X += Object->Index->Value1;
								Pos.Y += Object->Index->Value2;
								Pos.Z += Object->Index->Value3;
								CCD->ActorManager()->Position(tActor, Pos);

								// rotate to match scriptpoint angle
								if(Object->Index->Flag)
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

							if(Object->Index->Speed < 0.0f)
								Object->ActionActive = false;
						}
						else
						{
							if(Object->Index->Speed <= 0.0f)
							{
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
						CCD->ActorManager()->RemoveActorCheck(Object->Actor);
						geActor_Destroy(&Object->Actor);
						Object->Actor = NULL;

						if(Object->WeaponActor)
						{
							CCD->ActorManager()->RemoveActorCheck(Object->WeaponActor);
							geActor_Destroy(&Object->WeaponActor);
							Object->WeaponActor = NULL;
						}
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

							if(Object->Index->Value2 >= Object->Index->Value1)
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

							if(Object->Index->Value1 < Object->Index->Value2)
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

							if(Object->Index->Value2 <= Object->Index->Value1)
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
					Object->FOV = Object->Index->Speed*0.5f; // /2.0f;
					Object->FOV = (90.0f-Object->FOV)/90.0f;
					Object->FOVBone[0] = '\0';
					if(!EffectC_IsStringNull(Object->Index->AnimName))
						strcpy(Object->FOVBone, Object->Index->AnimName);
					Object->ActionActive = false;
					runflag = true;
					break;
				case STEPHEIGHT:
					if(Object->Actor)
					{
						if(Object->Index->Speed <= 0.0f)
							CCD->ActorManager()->SetAutoStepUp(Object->Actor, false);
						else
						{
							CCD->ActorManager()->SetAutoStepUp(Object->Actor, true);
							CCD->ActorManager()->SetStepHeight(Object->Actor, Object->Index->Speed);
						}
					}
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
						if(RotateToPoint((void*)Object, dwTicks))
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
						if(MoveToPoint((void*)Object, dwTicks))
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

						if(RotateMoveToPoint((void*)Object, dwTicks))
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
					Object->lowTime = 0.0f;
					Object->ThinkTime = 0.0f;
					Object->ElapseTime = 0.0f;
					strcpy(Object->Order, Object->Index->AnimName);
					Object->RunOrder = false;
					pool = Object->Bottom;
					while(pool != NULL)
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
						if(Object->Index->Speed > 0.0f)
						{
							geExtBox theBox;
							CCD->ActorManager()->GetBoundingBox(Object->Actor, &theBox);
							CCD->ActorManager()->SetBBox(Object->Actor, Object->Index->Speed, -theBox.Max.Y*2.0f, Object->Index->Speed);
						}
					}
					Object->ActionActive = false;
					runflag = true;
					break;
				case BOXHEIGHT:
					if(Object->Actor)
					{
						if(Object->Index->Speed > 0.0f)
						{
							geExtBox theBox;
							CCD->ActorManager()->GetBoundingBox(Object->Actor, &theBox);
							CCD->ActorManager()->SetBBox(Object->Actor, theBox.Max.X*2.0f, -(Object->Index->Speed)*2.0f, theBox.Max.Z*2.0f);
						}
					}
					Object->ActionActive = false;
					runflag = true;
					break;
				case SCALE:
					if(Object->Actor)
					{
						if(Object->Index->Speed != Object->Index->Value1
							|| Object->Index->Speed != Object->Index->Value2
							|| Object->Index->Value1 != Object->Index->Value2)
						{
							geVec3d Scale;

							CCD->ActorManager()->GetScaleXYZ(Object->Actor, &Scale);

							Scale.X *= Object->Index->Speed;
							Scale.Y *= Object->Index->Value1;
							Scale.Z *= Object->Index->Value2;

							CCD->ActorManager()->SetScaleXYZ(Object->Actor, Scale);

							if(Object->WeaponActor)
							{
								Scale.X = Object->Index->Speed*Object->WScale.X;
								Scale.Y = Object->Index->Value1*Object->WScale.Y;
								Scale.Z = Object->Index->Value2*Object->WScale.Z;

								CCD->ActorManager()->SetScaleXYZ(Object->WeaponActor, Scale);
							}
						}
						else
						{
							float scale;
							CCD->ActorManager()->GetScale(Object->Actor, &scale);
							CCD->ActorManager()->SetScale(Object->Actor, Object->Index->Speed*scale);

							if(Object->WeaponActor)
								CCD->ActorManager()->SetScale(Object->WeaponActor, Object->Index->Speed*Object->WScale.X);
						}
					}
					Object->ActionActive = false;
					runflag = true;
					break;
				case SETSCALE:
					if(Object->Actor)
					{
						if(Object->Index->Speed != Object->Index->Value1
							|| Object->Index->Speed != Object->Index->Value2
							|| Object->Index->Value1 != Object->Index->Value2)
						{
							geVec3d Scale;

							if(Object->WeaponActor)
							{
								CCD->ActorManager()->GetScaleXYZ(Object->Actor, &Scale);

								Scale.X = Object->Index->Speed*(Object->WScale.X/Scale.X);
								Scale.Y = Object->Index->Value1*(Object->WScale.Y/Scale.Y);
								Scale.Z = Object->Index->Value2*(Object->WScale.Z/Scale.Z);

								CCD->ActorManager()->SetScaleXYZ(Object->WeaponActor, Scale);
							}

							Scale.X = Object->Index->Speed;
							Scale.Y = Object->Index->Value1;
							Scale.Z = Object->Index->Value2;

							CCD->ActorManager()->SetScaleXYZ(Object->Actor, Scale);
						}
						else
						{
							if(Object->WeaponActor)
							{
								float scale;
								CCD->ActorManager()->GetScale(Object->Actor, &scale);
								scale = Object->Index->Speed*(Object->WScale.X/scale);
								CCD->ActorManager()->SetScale(Object->WeaponActor, scale);
							}

							CCD->ActorManager()->SetScale(Object->Actor, Object->Index->Speed);
						}
					}
					Object->ActionActive = false;
					runflag = true;
					break;
				case GRAVITY:
					if(Object->Actor)
					{
						Gravity.X = Gravity.Y = Gravity.Z = 0.0f;

						if(Object->Index->Flag)
							Gravity = CCD->Player()->GetGravity();

						CCD->ActorManager()->SetGravity(Object->Actor, Gravity);
					}
					Object->ActionActive = false;
					runflag = true;
					break;
				case ISVEHICLE:
					if(Object->Actor)
					{
						if(Object->Index->Flag)
							CCD->ActorManager()->SetType(Object->Actor, ENTITY_VEHICLE);
						else
							CCD->ActorManager()->SetType(Object->Actor, ENTITY_NPC);
					}
					Object->ActionActive = false;
					runflag = true;
					break;
				case ISPUSHABLE:
					if(Object->Actor)
					{
						if(Object->Index->Flag)
							Object->pushable = true;
						else
							Object->pushable = false;
					}
					Object->ActionActive = false;
					runflag = true;
					break;
				case HIDEFROMRADAR:
					if(Object->Actor)
					{
						CCD->ActorManager()->SetHideRadar(Object->Actor, Object->Index->Flag);
					}
					Object->ActionActive = false;
					runflag = true;
					break;
				case FIREPROJECTILE:
					{
						if(Object->Actor)
						{
							char *Strn = Object->Index->AnimName;
							char Proj[64];
							geXForm3d Xf;
							geVec3d theRotation, Pos, Direction, Orient, TargetPoint;
							strncpy(Proj, Strn, (int)Object->Index->Speed);
							Proj[(int)Object->Index->Speed] = '\0';
							Strn = Strn + (int)Object->Index->Speed;
							bool bone;

							if(Object->WeaponActor)
								bone = geActor_GetBoneTransform(Object->WeaponActor, Object->Index->TriggerName, &Xf);
							else
								bone = geActor_GetBoneTransform(Object->Actor, Object->Index->TriggerName, &Xf);

							if(bone)
							{
								geVec3d_Copy(&(Xf.Translation), &Pos);
								CCD->ActorManager()->GetRotate(Object->Actor, &theRotation);

								geXForm3d_SetZRotation(&Xf, theRotation.Z);
								geXForm3d_RotateX(&Xf, theRotation.X);
								geXForm3d_RotateY(&Xf, theRotation.Y);

								geXForm3d_GetIn(&Xf, &Direction);
								geVec3d_AddScaled(&Pos, &Direction, 1000.0f, &TargetPoint);
								geVec3d_AddScaled(&Pos, &Direction, Object->Index->Value3, &Pos);

								geXForm3d_GetUp(&Xf, &Direction);
								geVec3d_AddScaled(&Pos, &Direction, Object->Index->Value2, &Pos);

								geXForm3d_GetLeft(&Xf, &Direction);
								geVec3d_AddScaled(&Pos, &Direction, Object->Index->Value1, &Pos);


								geVec3d_Subtract(&TargetPoint, &Pos, &Orient);
								float l = geVec3d_Length(&Orient);

								if(l > 0.0f)
								{
									float x = Orient.X;
									Orient.X = GE_PIOVER2 - (float)acos(Orient.Y / l);
									Orient.Y = (float)atan2(x, Orient.Z) + GE_PI;
									Orient.Z = 0.0f;	// roll is zero - always!!?

									CCD->Weapons()->Add_Projectile(Pos, Pos, Orient, Proj, Strn, Strn);
								}
							}
						}

						Object->ActionActive = false;
						runflag = true;
					}
					break;
				case ADDEFFECT:
					{
						if(Object->Actor)
						{
							geVec3d Pos;

							if(geActor_DefHasBoneNamed(geActor_GetActorDef(Object->Actor), Object->Index->TriggerName))
							{
								Pos.X = Object->Index->Value1;
								Pos.Y = Object->Index->Value2;
								Pos.Z = Object->Index->Value3;

								CCD->Explosions()->AddExplosion(Object->Index->AnimName, Pos, Object->Actor, Object->Index->TriggerName);
							}
						}

						Object->ActionActive = false;
						runflag = true;
					}
					break;
				case TARGETGROUP:
					if(!EffectC_IsStringNull(Object->Index->AnimName))
						strcpy(Object->TargetGroup, Object->Index->AnimName);
					Object->ActionActive = false;
					runflag = true;
					break;
				case TESTDAMAGEORDER:
					if(Object->Actor)
					{
						if(!EffectC_IsStringNull(Object->Attribute))
						{
							if(!EffectC_IsStringNull(Object->Index->AnimName))
							{
								CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(Object->Actor);
								int amt = abs(theInv->GetModifyAmt(Object->Attribute));
								if((int)Object->Index->Speed<=amt)
								{
									strcpy(Object->Order, Object->Index->AnimName);
									Object->RunOrder = true;
									pool = Object->Bottom;

									while(pool != NULL)
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
					}
					Object->ActionActive = false;
					runflag = false;
					break;
				case CHANGEMATERIAL:
					if(Object->Actor)
					{
						if(!EffectC_IsStringNull(Object->Index->AnimName))
							CCD->ActorManager()->ChangeMaterial(Object->Actor, Object->Index->AnimName);
					}
					Object->ActionActive = false;
					runflag = true;
					break;
				case ATTACHTOACTOR:
					if(Object->Actor)
					{
						char master[64], masterbone[64], slavebone[64];
						char numeric[128];
						char *szAtom;
						geVec3d Position, Rotation;
						geActor *MasterActor;

						strncpy(master, Object->Index->AnimName, (int)Object->Index->Speed);
						master[(int)Object->Index->Speed] = '\0';

						strncpy(masterbone, (Object->Index->AnimName+(int)Object->Index->Speed), (int)Object->Index->Value1);
						masterbone[(int)Object->Index->Value1] = '\0';

						strcpy(slavebone, (Object->Index->AnimName+(int)Object->Index->Speed+(int)Object->Index->Value1));
						strcpy(numeric, Object->Index->TriggerName);

						szAtom = strtok(numeric," ");
						Position.X = (float)atof(szAtom);
						szAtom = strtok(NULL," \n");
						Position.Y = (float)atof(szAtom);
						szAtom = strtok(NULL," \n");
						Position.Z = (float)atof(szAtom);

						szAtom = strtok(NULL," \n");
						Rotation.X = (float)atof(szAtom)*0.0174532925199433f;
						szAtom = strtok(NULL," \n");
						Rotation.Y = (float)atof(szAtom)*0.0174532925199433f;
						szAtom = strtok(NULL," \n");
						Rotation.Z = (float)atof(szAtom)*0.0174532925199433f;



						MasterActor = CCD->ActorManager()->GetByEntityName(master);

						if(!MasterActor)
							break;

						CCD->ActorManager()->ActorAttach(MasterActor,  masterbone,
														Object->Actor, slavebone,
														Position, Rotation);
						Object->ActionActive = false;
						runflag = true;
					}
					else
					{
						Object->ActionActive = false;
						runflag = true;
					}
					break;
				case DETACHFROMACTOR:
					if(Object->Actor)
					{
						geActor *Actor;
						Actor = CCD->ActorManager()->GetByEntityName(Object->Index->AnimName);

						if(!Actor)
							break;

						CCD->ActorManager()->DetachFromActor(Object->Actor, Actor);
					}
					Object->ActionActive = false;
					runflag = true;
					break;
				case ATTACHBLENDACTOR:
					if(!EffectC_IsStringNull(Object->Index->TriggerName))
					{
						geActor *SlaveActor;
						SlaveActor = CCD->ActorManager()->GetByEntityName(Object->Index->AnimName);
						if(!SlaveActor)
							break;

						geActor *MasterActor;
						if(!stricmp(Object->Index->TriggerName, "Player"))
							MasterActor = CCD->Player()->GetActor();
						else
							MasterActor = CCD->ActorManager()->GetByEntityName(Object->Index->TriggerName);

						if(!MasterActor)
							break;

						CCD->ActorManager()->ActorAttachBlend(SlaveActor, MasterActor);
					}
					else
					{
						if(Object->Actor)
						{
							geActor *SlaveActor;
							SlaveActor = CCD->ActorManager()->GetByEntityName(Object->Index->AnimName);

							if(!SlaveActor)
								break;

							CCD->ActorManager()->ActorAttachBlend(SlaveActor, Object->Actor);

						}
					}
					Object->ActionActive = false;
					runflag = true;
					break;
				case DETACHBLENDACTOR:
					if(!EffectC_IsStringNull(Object->Index->TriggerName))
					{
						geActor *SlaveActor;
						SlaveActor = CCD->ActorManager()->GetByEntityName(Object->Index->AnimName);
						if(SlaveActor)
							break;

						geActor *MasterActor;
						if(!stricmp(Object->Index->TriggerName, "Player"))
							MasterActor = CCD->Player()->GetActor();
						else
							MasterActor = CCD->ActorManager()->GetByEntityName(Object->Index->TriggerName);

						if(!MasterActor)
							break;

						CCD->ActorManager()->DetachBlendFromActor(MasterActor, SlaveActor);
					}
					else
					{
						if(Object->Actor)
						{
							geActor *SlaveActor;
							SlaveActor = CCD->ActorManager()->GetByEntityName(Object->Index->AnimName);

							if(!SlaveActor)
								break;

							CCD->ActorManager()->DetachBlendFromActor(Object->Actor, SlaveActor);
						}
					}
					Object->ActionActive = false;
					runflag = true;
					break;
				case ATTACHACCESSORY:
					{
						geActor *MasterActor = NULL;
						std::string EntityName;
						if(!EffectC_IsStringNull(Object->Index->TriggerName))
						{
							EntityName = Object->Index->TriggerName;
							if(!stricmp(Object->Index->TriggerName, "Player"))
								MasterActor = CCD->Player()->GetActor();
							else
								MasterActor = CCD->ActorManager()->GetByEntityName(Object->Index->TriggerName);
						}
						else
						{
							EntityName = CCD->ActorManager()->GetEntityName(Object->Actor);
							MasterActor = Object->Actor;
						}

						if(MasterActor)
						{
							unsigned int keynum = AccessoryCache.size();
							if(keynum>0)
							{
								for(unsigned int i=0; i<keynum; i++)
								{
									if(!strcmp(AccessoryCache[i].Name.c_str(), Object->Index->AnimName))
									{
										geActor *SlaveActor;
										SlaveActor = CCD->ActorManager()->SpawnActor(AccessoryCache[i].ActorName,
											Object->Location, AccessoryCache[i].Rotation, "", "", NULL);

										if(!SlaveActor)
											break;

										EntityName += Object->Index->AnimName;
										CCD->ActorManager()->SetEntityName(SlaveActor, EntityName.c_str());

										if(AccessoryCache[i].EnvironmentMapping)
											SetEnvironmentMapping(	SlaveActor, true,
																	AccessoryCache[i].AllMaterial,
																	AccessoryCache[i].PercentMapping,
																	AccessoryCache[i].PercentMaterial);

										CCD->ActorManager()->ActorAttachBlend(SlaveActor, MasterActor);
										break;
									}
								}
							}
						}
						Object->ActionActive = false;
						runflag = true;
						break;
					}
				case DETACHACCESSORY:
					{
						geActor *MasterActor = NULL;
						std::string EntityName;
						if(!EffectC_IsStringNull(Object->Index->TriggerName))
						{
							EntityName = Object->Index->TriggerName;
							if(!stricmp(Object->Index->TriggerName, "Player"))
								MasterActor = CCD->Player()->GetActor();
							else
								MasterActor = CCD->ActorManager()->GetByEntityName(Object->Index->TriggerName);
						}
						else
						{
							EntityName = CCD->ActorManager()->GetEntityName(Object->Actor);
							MasterActor = Object->Actor;
						}

						if(MasterActor)
						{
							EntityName += Object->Index->AnimName;
							geActor *SlaveActor = CCD->ActorManager()->GetByEntityName(EntityName.c_str());
							if(SlaveActor)
							{
								CCD->ActorManager()->DetachBlendFromActor(MasterActor, SlaveActor);
								CCD->ActorManager()->RemoveActor(SlaveActor);
								geActor_Destroy(&SlaveActor);
							}
						}

						Object->ActionActive = false;
						runflag = true;
						break;
					}
				case ALIGN:
					if(Object->Actor)
					{
						const char *EntityType = CCD->EntityRegistry()->GetEntityType(Object->Point);

						if(EntityType)
						{
							if(!stricmp(EntityType, "ScriptPoint"))
							{
								ScriptPoint *pProxy;
								geVec3d Orient;
								CCD->ScriptPoints()->LocateEntity(Object->Point, (void**)&pProxy);
								Orient.Z = 0.0174532925199433f*(pProxy->Angle.Z);
								Orient.X = 0.0174532925199433f*(pProxy->Angle.X);
								Orient.Y = 0.0174532925199433f*(pProxy->Angle.Y-90.0f);
								CCD->ActorManager()->Rotate(Object->Actor, Orient);
							}
						}
					}
					Object->ActionActive = false;
					runflag = true;
					break;
				case SETKEYPAUSE:
					CCD->SetKeyPaused(Object->Index->Flag);
					Object->ActionActive = false;
					runflag = true;
					break;
				case SETHUDDRAW:
					if(Object->Index->Flag)
						CCD->HUD()->Activate();
					else
						CCD->HUD()->Deactivate();
					Object->ActionActive = false;
					runflag = true;
					break;
				case SOUNDLOOP:
					Object->SoundLoop = Object->Index->Flag;
					Object->ActionActive = false;
					runflag = true;
					break;
				case REMOVEWEAPON:
					if(Object->Actor)
					{
						if(Object->WeaponActor)
						{
							CCD->ActorManager()->RemoveActor(Object->WeaponActor);
							geActor_Destroy(&Object->WeaponActor);
							Object->WeaponActor = NULL;
						}
					}
					Object->ActionActive = false;
					runflag = true;
					break;
				case SETLODDISTANCE:
					if(Object->Actor)
					{
						CCD->ActorManager()->SetLODdistance(Object->Actor, Object->Index->Speed,
							Object->Index->Value1, Object->Index->Value2, Object->Index->Value3,
							Object->Index->Value4);
					}
					Object->ActionActive = false;
					runflag = true;
					break;
				case SETWEAPON:
					if(Object->Actor)
					{
						unsigned int keynum = WeaponCache.size();
						if(keynum>0)
						{
							for(unsigned int i=0; i<keynum; i++)
							{
								if(!strcmp(WeaponCache[i].Name.c_str(), Object->Index->AnimName))
								{
									if(Object->WeaponActor)
									{
										CCD->ActorManager()->RemoveActor(Object->WeaponActor);
										geActor_Destroy(&Object->WeaponActor);
										Object->WeaponActor = NULL;
									}

									Object->WeaponActor = CCD->ActorManager()->SpawnActor(WeaponCache[i].ActorName,
										Object->Location, WeaponCache[i].Rotation, "", "", NULL);
									CCD->ActorManager()->SetActorDynamicLighting(Object->WeaponActor, WeaponCache[i].FillColor, WeaponCache[i].AmbientColor, WeaponCache[i].AmbientLightFromFloor);
									Object->WRotation = WeaponCache[i].Rotation;
									Object->WScale.X = Object->WScale.Y = Object->WScale.Z = WeaponCache[i].Scale;
									CCD->ActorManager()->SetScale(Object->WeaponActor, Object->WScale.X);
									CCD->ActorManager()->SetNoCollide(Object->WeaponActor);
									if(WeaponCache[i].EnvironmentMapping)
										SetEnvironmentMapping(Object->WeaponActor, true, WeaponCache[i].AllMaterial, WeaponCache[i].PercentMapping, WeaponCache[i].PercentMaterial);
									break;
								}
							}
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


/* ----------------------------------- END OF FILE ------------------------------------ */
