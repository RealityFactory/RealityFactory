/************************************************************************************//**
 * @file CLevelController.cpp
 * @brief Level Controller
 *
 * This file contains the class implementation for the CLevelController that
 * encapsulates level control logic handling in the RGF.
 * @author David C. Swope
 *//*
 * Copyright (c) 2002 David C. Swope; All rights reserved.
 ****************************************************************************************/

// Include the One True Header
#include "RabidFramework.h"
#include "CHeadsUpDisplay.h"
#include "CInventory.h"

#include "C3DAudioSource.h"
#include "CAutoDoors.h"
#include "CFixedCamera.h"
#include "CMovingPlatforms.h"
#include "CTriggers.h"
#include "CWallDecal.h"
#include "Simkin\\skScriptedExecutable.h"
#include "Simkin\\skRValue.h"
#include "Simkin\\skRValueArray.h"
#include "Simkin\\skRuntimeException.h"
#include "Simkin\\skParseException.h"
#include "Simkin\\skBoundsException.h"
#include "Simkin\\skTreeNode.h"

extern geSound_Def *SPool_Sound(const char *SName);


/* ------------------------------------------------------------------------------------ */
// Constructor
/* ------------------------------------------------------------------------------------ */
ControllerObject::ControllerObject(char *fileName) : skScriptedExecutable(fileName,CCD->GetskContext())//change simkin
{
	Order[0]		= '\0';
	m_ThinkOrder[0]	= '\0';
	m_ElapseTime	= 0.0f;
	m_ThinkTime		= 0.0f;
	szName[0]		= '\0';
	m_Console		= false;
	m_ConsoleHeader	= NULL;
	m_ConsoleError	= NULL;

	memset(m_ConsoleDebug, 0, sizeof(char*)*DEBUGLINES);
}


/* ------------------------------------------------------------------------------------ */
// Destructor
/* ------------------------------------------------------------------------------------ */
ControllerObject::~ControllerObject()
{
	SAFE_DELETE_A(m_ConsoleHeader);
	SAFE_DELETE_A(m_ConsoleError);

	for(int i=0; i<DEBUGLINES; ++i)
	{
		SAFE_DELETE_A(m_ConsoleDebug[i]);
	}
}


/* ------------------------------------------------------------------------------------ */
// method
/* ------------------------------------------------------------------------------------ */
bool ControllerObject::method(const skString& methodName, skRValueArray& arguments,
							  skRValue& returnValue, skExecutableContext& ctxt)
{
	char  string1[128], string2[128];
	float float1;
	bool  bool1;

	string1[0] = '\0';
	string2[0] = '\0';

	if(IS_METHOD(methodName, "SetPlatformTargetTime"))
	{
		PARMCHECK(2);
		strcpy(string1, arguments[0].str());
		float1 = arguments[1].floatValue();
		MovingPlatform *pEntity;
		CCD->Platforms()->LocateEntity(string1, (void**)&pEntity);
		CCD->ModelManager()->SetTargetTime(pEntity->Model, float1);
		return true;
	}
	else if(IS_METHOD(methodName, "GetPlatformTargetTime"))
	{
		PARMCHECK(1);
		strcpy(string1, arguments[0].str());
		MovingPlatform *pEntity;
		CCD->Platforms()->LocateEntity(string1, (void**)&pEntity);
		CCD->ModelManager()->GetTargetTime(pEntity->Model, &float1);
		returnValue = float1;
		return true;
	}
	else if(IS_METHOD(methodName, "GetPlatformCurrentTime"))
	{
		PARMCHECK(1);
		strcpy(string1, arguments[0].str());
		MovingPlatform *pEntity;
		CCD->Platforms()->LocateEntity(string1, (void**)&pEntity);
		CCD->ModelManager()->GetModelCurrentTime(pEntity->Model, &float1);
		returnValue = float1;
		return true;
	}
	else if(IS_METHOD(methodName, "PlatformCollision"))
	{
		PARMCHECK(1);
		strcpy(string1, arguments[0].str());
		MovingPlatform *pEntity;
		CCD->Platforms()->LocateEntity(string1, (void**)&pEntity);
		CCD->ModelManager()->ModelInCollision(pEntity->Model, &bool1);
		returnValue = bool1;
		return true;
	}

	if(IS_METHOD(methodName, "SetDoorTargetTime"))
	{
		PARMCHECK(2);
		strcpy(string1, arguments[0].str());
		float1 = arguments[1].floatValue();
		Door *pEntity;
		CCD->Doors()->LocateEntity(string1, (void**)&pEntity);
		CCD->ModelManager()->SetTargetTime(pEntity->Model, float1);
		return true;
	}
	else if(IS_METHOD(methodName, "GetDoorTargetTime"))
	{
		PARMCHECK(1);
		strcpy(string1, arguments[0].str());
		Door *pEntity;
		CCD->Doors()->LocateEntity(string1, (void**)&pEntity);
		CCD->ModelManager()->GetTargetTime(pEntity->Model, &float1);
		returnValue = float1;
		return true;
	}
	else if(IS_METHOD(methodName, "GetDoorCurrentTime"))
	{
		PARMCHECK(1);
		strcpy(string1, arguments[0].str());
		Door *pEntity;
		CCD->Doors()->LocateEntity(string1, (void**)&pEntity);
		CCD->ModelManager()->GetModelCurrentTime(pEntity->Model, &float1);
		returnValue = float1;
		return true;
	}
	else if(IS_METHOD(methodName, "DoorCollision"))
	{
		PARMCHECK(1);
		strcpy(string1, arguments[0].str());
		Door *pEntity;
		CCD->Doors()->LocateEntity(string1, (void**)&pEntity);
		CCD->ModelManager()->ModelInCollision(pEntity->Model, &bool1);
		returnValue = bool1;
		return true;
	}
	else if(IS_METHOD(methodName, "ShowWallDecal"))
	{
		PARMCHECK(1);
		strcpy(string1, arguments[0].str());
		CCD->WallDecals()->SetProgrammedTrigger(string1, GE_TRUE);
		return true;
	}
	else if(IS_METHOD(methodName, "HideWallDecal"))
	{
		PARMCHECK(1);
		strcpy(string1, arguments[0].str());
		CCD->WallDecals()->SetProgrammedTrigger(string1, GE_FALSE);
		return true;
	}
	else if(IS_METHOD(methodName, "SetWallDecalBitmap"))
	{
		PARMCHECK(2);
		strcpy(string1, arguments[0].str());
		int bmnum = arguments[1].intValue();
		CCD->WallDecals()->SetCurrentBitmap(string1, bmnum);
		return true;
	}
	else if(IS_METHOD(methodName, "ActivateTrigger"))
	{
		PARMCHECK(1);
		strcpy(string1, arguments[0].str());
		CCD->Triggers()->HandleTriggerEvent(string1);
		return true;
	}
	else if(IS_METHOD(methodName, "GetEventState"))
	{
		PARMCHECK(1);
		returnValue = GetTriggerState(arguments[0].str().c_str());
		return true;
	}
	else if(IS_METHOD(methodName, "SetEventState"))
	{
		PARMCHECK(2);
		strcpy(string1, arguments[0].str());
		bool flag = arguments[1].boolValue();
		CCD->Pawns()->AddEvent(string1, flag);
		return true;
	}
	else if(IS_METHOD(methodName, "Start3DAudioSource"))
	{
		PARMCHECK(1);
		strcpy(string1, arguments[0].str());
		CCD->Audio3D()->SetProgrammedTrigger(string1, GE_TRUE);
		return true;
	}
	else if(IS_METHOD(methodName, "Stop3DAudioSource"))
	{
		PARMCHECK(1);
		strcpy(string1, arguments[0].str());
		CCD->Audio3D()->SetProgrammedTrigger(string1, GE_FALSE);
		return true;
	}
	else if(IS_METHOD(methodName, "Get3DAudioSourceState"))
	{
		PARMCHECK(1);
		strcpy(string1, arguments[0].str());
		bool state = CCD->Audio3D()->IsPlaying(string1);
		returnValue = (bool)state;
		return true;
	}
	else if(IS_METHOD(methodName, "PlaySound"))
	{
		PARMCHECK(1);
		strcpy(string1, arguments[0].str());

		if(!EffectC_IsStringNull(string1))
		{
			geVec3d Position = CCD->Player()->Position();
			Snd Sound;

			memset(&Sound, 0, sizeof(Sound));
			geVec3d_Copy(&Position, &(Sound.Pos));
			Sound.Min = CCD->GetAudibleRadius();

			if(arguments.entries() == 2)
				Sound.Min = arguments[1].floatValue();

			Sound.Loop = GE_FALSE;
			Sound.SoundDef = SPool_Sound(string1);

			if(Sound.SoundDef != NULL)
				CCD->EffectManager()->Item_Add(EFF_SND, static_cast<void*>(&Sound));
		}

		return true;
	}
	// ShowText(Nr, EntityName, Animation, TextString, FontNr, TextSound, ScreenOffsetX, ScreenOffsetY, Align, Alpha);
	// Shows a TextMessage on the screen, attached to a Pawn or Player
	// Align can be Right, Left or Centre. If left open, Right is used.
	// The text can include a # to print it in multiple lines
	else if(IS_METHOD(methodName, "ShowText"))
	{
		int Nr = arguments[0].intValue();

		if(Nr < 0 || Nr >= MAXTEXT)
			return true;

		if(arguments.entries() > 1)
		{
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
		}
		else
		{
			CCD->Pawns()->TextMessage[Nr].ShowText = false;
		}

		return true;
	}
	else if(IS_METHOD(methodName, "RemoveText"))
	{
		int Nr = arguments[0].intValue();

		if(Nr < 0 || Nr >= MAXTEXT)
			return true;

		CCD->Pawns()->TextMessage[Nr].ShowText = false;

		return true;
	}
	else if(IS_METHOD(methodName, "SetPlatformSpeed"))
	{
		// SetPlatformSpeed(PlatformName, Speed)
		PARMCHECK(2);
		MovingPlatform *pEntity;

		strcpy(string1, arguments[0].str());
		float1 = arguments[1].floatValue();

		CCD->Platforms()->LocateEntity(string1, (void**)&pEntity);
		CCD->ModelManager()->SetAnimationSpeed(pEntity->Model, float1);

		return true;
	}
	else if(IS_METHOD(methodName, "SetDoorSpeed"))
	{
		// SetDoorSpeed(DoorName, Speed)

		PARMCHECK(2);
		Door *pEntity;

		strcpy(string1, arguments[0].str());
		float1 = arguments[1].floatValue();

		CCD->Doors()->LocateEntity(string1, (void**)&pEntity);
		CCD->ModelManager()->SetAnimationSpeed(pEntity->Model, float1);

		return true;
	}
	else if(IS_METHOD(methodName, "SetPlatformToTargetTime"))
	{
		// SetPlatformTimeNow(PlatformName, KeyFrame)
		// move a platform immediately to a given timeposition

		PARMCHECK(2);
		MovingPlatform *pEntity;

		strcpy(string1, arguments[0].str());
		float1 = arguments[1].floatValue();

		CCD->Platforms()->LocateEntity(string1, (void**)&pEntity);
        CCD->ModelManager()->SetToTargetTime(pEntity->Model, float1);

		return true;
	}
	else if(IS_METHOD(methodName, "SetDoorToTargetTime"))
	{
		// SetDoorTimeNow(DoorName, KeyFrame)
		// move a door immediately to a given timeposition

		PARMCHECK(2);
		Door *pEntity;

		strcpy(string1, arguments[0].str());
		float1 = arguments[1].floatValue();

		CCD->Doors()->LocateEntity(string1, (void**)&pEntity);
        CCD->ModelManager()->SetToTargetTime(pEntity->Model, float1);

		return true;
	}
	else if(IS_METHOD(methodName, "SetFlag"))
	{
		// SetFlag(FlagNr, BooleanValue)

		int int1 = arguments[0].intValue();

		if(int1 >= MAXFLAGS)
			return true;

		bool1 = arguments[1].boolValue();
		CCD->Pawns()->SetPawnFlag(int1, bool1);

		return true;
	}
	else if(IS_METHOD(methodName, "GetFlag"))
	{
		// bool GetFlag(FlagNr)

		int int1 = arguments[0].intValue();

		if(int1 >= MAXFLAGS)
		{
			returnValue = false;
			return true;
		}

		returnValue = CCD->Pawns()->GetPawnFlag(int1);

		return true;
	}
	// Distance:PlatformDistance(PlatformName, EntityName, IgnoreHeight)
	// Returns the distance form the platform center to the origin of an Entity
	// Apart from Pawns also 2 special Entity names are supported: "Player" and "Camera"
	else if(IS_METHOD(methodName, "PlatformDistance"))
	{
		PARMCHECK(3);
		geVec3d PlatformPos, EntityPos;
		MovingPlatform *pEntity;

		strcpy(string1, arguments[0].str());

		if(CCD->Platforms()->LocateEntity(string1, (void**)&pEntity) == RGF_SUCCESS)
		{
			CCD->ModelManager()->GetPosition(pEntity->Model, &PlatformPos);
			strcpy(string1, arguments[1].str());

			if(!stricmp(string1, "Player"))
				EntityPos = CCD->Player()->Position();
			else if(!stricmp(string1, "Camera"))
				CCD->CameraManager()->GetPosition(&EntityPos);
			else
				CCD->ActorManager()->GetPosition(CCD->ActorManager()->GetByEntityName(string1), &EntityPos);

			if(arguments.entries() > 2)
				if(arguments[2].boolValue())
					PlatformPos.Y = EntityPos.Y;

			returnValue = geVec3d_DistanceBetween(&PlatformPos, &EntityPos);
		}
		else
		{
			returnValue = 0;
		}

		return true;
	}
	// Distance:DoorDistance(DoorName, EntityName, IgnoreHeight)
	// Gives the distance from the platform centre to the origin of an Entity
	// Apart from Pawns also 2 special Entity names are supported: "Player" and "Camera"
	else if (IS_METHOD(methodName, "DoorDistance"))
	{
		PARMCHECK(3);
		geVec3d DoorPos, EntityPos;
		Door *pEntity;

		strcpy(string1, arguments[0].str());

		if(CCD->Doors()->LocateEntity(string1, (void**)&pEntity) == RGF_SUCCESS)
		{
			CCD->ModelManager()->GetPosition(pEntity->Model, &DoorPos);
			strcpy(string1, arguments[1].str());

			if(!stricmp(string1, "Player"))
				EntityPos = CCD->Player()->Position();
			else if(!stricmp(string1, "Camera"))
				CCD->CameraManager()->GetPosition(&EntityPos);
			else
				CCD->ActorManager()->GetPosition(CCD->ActorManager()->GetByEntityName(string1), &EntityPos);

			if(arguments.entries() > 2)
				if(arguments[2].boolValue())
					DoorPos.Y = EntityPos.Y;

			returnValue = geVec3d_DistanceBetween(&DoorPos, &EntityPos);
		}
		else
		{
			returnValue = 0;
		}

		return true;
	}
	else if(IS_METHOD(methodName, "Console"))
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
	else if(IS_METHOD(methodName, "debug"))
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
				strcpy(m_ConsoleDebug[index], arguments[0].str().c_str());
			}
			else
			{
				for(i=1; i<DEBUGLINES; ++i)
				{
					strcpy(m_ConsoleDebug[i-1], m_ConsoleDebug[i]);
				}

				strcpy(m_ConsoleDebug[DEBUGLINES-1], arguments[0].str().c_str());
			}
		}

		return true;
	}
	else if(IS_METHOD(methodName, "random"))
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
	else if(IS_METHOD(methodName, "StringCopy"))
	{
		PARMCHECK(1);
		returnValue = arguments[0].str();
		return true;
	}
	else if(IS_METHOD(methodName, "LeftCopy"))
	{
		PARMCHECK(2);

		int length = arguments[1].intValue();
		int slength = arguments[0].str().length();

		if(length > slength)
			length = slength;

		returnValue = arguments[0].str().substr(0, length);

		return true;
	}
	else if(IS_METHOD(methodName, "RightCopy"))
	{
		PARMCHECK(2);

		int length = arguments[1].intValue();
		int slength = arguments[0].str().length();

		if(length > slength)
			length = slength;

		returnValue = arguments[0].str().substr(slength-length);
		return true;
	}
	else if(IS_METHOD(methodName, "Integer"))
	{
		PARMCHECK(1);
		returnValue = arguments[0].intValue();
		return true;
	}
	else if(IS_METHOD(methodName, "ModifyAttribute"))
	{
		PARMCHECK(2);

		CPersistentAttributes *theInv;

		if(arguments.entries() > 2)
		{
			strcpy(string1, arguments[2].str());

			if(!stricmp(string1, "Player"))
			{
				theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
			}
			else
			{
				theInv = CCD->ActorManager()->Inventory(CCD->ActorManager()->GetByEntityName(string1));
			}
		}
		else
		{
			theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
		}

		returnValue = theInv->Modify(arguments[0].str().c_str(), arguments[1].intValue());

		return true;
	}
	else if(IS_METHOD(methodName, "GetAttribute"))
	{
		PARMCHECK(1);

		CPersistentAttributes *theInv;

		if(arguments.entries() > 1)
		{
			strcpy(string1, arguments[1].str());

			if(!stricmp(string1, "Player"))
			{
				theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
			}
			else
			{
				theInv = CCD->ActorManager()->Inventory(CCD->ActorManager()->GetByEntityName(string1));
			}
		}
		else
		{
			theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
		}

		returnValue = theInv->Value(arguments[0].str().c_str());

		return true;
	}
	else if(IS_METHOD(methodName, "SetAttribute"))
	{
		PARMCHECK(2);

		CPersistentAttributes *theInv;

		if(arguments.entries() > 2)
		{
			strcpy(string1, arguments[2].str());

			if(!stricmp(string1, "Player"))
			{
				theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
			}
			else
			{
				theInv = CCD->ActorManager()->Inventory(CCD->ActorManager()->GetByEntityName(string1));
			}
		}
		else
		{
			theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
		}

		returnValue = theInv->Set(arguments[0].str().c_str(), arguments[1].intValue());

		return true;
	}
	else if(IS_METHOD(methodName, "AddAttribute"))
	{
		// USAGE:	AddAttribute(char *Attribute)
		//			AddAttribute(char *Attribute, char *EntityName)
		//			AddAttribute(char *Attribute, int LowValue, int HighValue)
		//			AddAttribute(char *Attribute, int LowValue, int HighValue, char *EntityName)

		PARMCHECK(1);
		strcpy(string1, arguments[0].str());

		CPersistentAttributes *theInv;

		if(arguments.entries() == 2 || arguments.entries() == 4)
		{
			strcpy(string2, arguments[arguments.entries()-1].str());

			if(!stricmp(string2, "Player"))
			{
				theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
			}
			else
			{
				theInv = CCD->ActorManager()->Inventory(CCD->ActorManager()->GetByEntityName(string2));
			}
		}
		else
		{
			theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
		}

		returnValue = theInv->Add(string1);

		if(arguments.entries() > 2)
		{
			theInv->SetValueLimits(string1, arguments[1].intValue(), arguments[2].intValue());
		}

		return true;
	}
	else if(IS_METHOD(methodName, "SetAttributeValueLimits"))
	{
		// USAGE:	SetAttributeValueLimits(char* Attribute, int LowValue, int HighValue)
		//			SetAttributeValueLimits(char* Attribute, int LowValue, int HighValue, char* EntityName)

		PARMCHECK(3);

		CPersistentAttributes *theInv;

		if(arguments.entries() > 3)
		{
			strcpy(string1, arguments[3].str());

			if(!stricmp(string1, "Player"))
			{
				theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
			}
			else
			{
				theInv = CCD->ActorManager()->Inventory(CCD->ActorManager()->GetByEntityName(string1));
			}
		}
		else
		{
			theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
		}

		theInv->SetValueLimits(arguments[0].str().c_str(), arguments[1].intValue(), arguments[2].intValue());

		return true;
	}
	else if(IS_METHOD(methodName, "sin"))
	{
		PARMCHECK(1);
		returnValue = sin(arguments[0].floatValue());

		return true;
	}
	else if(IS_METHOD(methodName, "cos"))
	{
		PARMCHECK(1);
		returnValue = cos(arguments[0].floatValue());

		return true;
	}
	else if(IS_METHOD(methodName, "tan"))
	{
		PARMCHECK(1);
		returnValue = tan(arguments[0].floatValue());

		return true;
	}
	else if(IS_METHOD(methodName, "asin"))
	{
		PARMCHECK(1);
		returnValue = asin(arguments[0].floatValue());

		return true;
	}
	else if(IS_METHOD(methodName, "acos"))
	{
		PARMCHECK(1);
		returnValue = acos(arguments[0].floatValue());

		return true;
	}
	else if(IS_METHOD(methodName, "atan"))
	{
		PARMCHECK(1);
		returnValue = atan(arguments[0].floatValue());

		return true;
	}
	else if(IS_METHOD(methodName, "SetPlayerWeapon"))
	{
		PARMCHECK(1);
		CCD->Weapons()->SetWeapon(arguments[0].intValue());
		return true;
	}
	else if(IS_METHOD(methodName, "SetUseItem"))
	{
		PARMCHECK(1);
		strcpy(string1, arguments[0].str());
		CCD->Player()->SetUseAttribute(string1);
		CCD->HUD()->ActivateElement(string1, true);
		return true;
	}
	else if(IS_METHOD(methodName, "IsKeyDown"))
	{
		PARMCHECK(1);
		int temp = arguments[0].intValue();
		returnValue = false;

		if(CCD->Input()->GetKeyCheck(temp) == true)
			returnValue = true;

		return true;
	}
	else if(IS_METHOD(methodName, "SetFixedCameraPosition"))
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
	else if(IS_METHOD(methodName, "SetFixedCameraRotation"))
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
	else if(IS_METHOD(methodName, "SetFixedCameraFOV"))
	{
		// USAGE: SetFixedCameraFOV(float FOV)
		PARMCHECK(1);

		CCD->FixedCameras()->SetFOV(arguments[0].floatValue());

		return true;
	}
	else if(IS_METHOD(methodName, "MoveFixedCamera"))
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
	else if(IS_METHOD(methodName, "RotateFixedCamera"))
	{
		// USAGE: RotateFixedCamera(RotX, RotY, RotZ)
		PARMCHECK(3);

		geVec3d Rot;
		Rot.X = GE_PIOVER180*arguments[0].floatValue();
		Rot.Y = GE_PIOVER180*arguments[1].floatValue();
		Rot.Z = GE_PIOVER180*arguments[2].floatValue();
		CCD->FixedCameras()->Rotate(Rot);

		return true;
	}
	else
	{
		return skScriptedExecutable::method(methodName, arguments, returnValue, ctxt);
	}
}


/* ------------------------------------------------------------------------------------ */
// getValue
/* ------------------------------------------------------------------------------------ */
bool ControllerObject::getValue(const skString& fieldName, const skString& attribute, skRValue& value)
{
	if(fieldName == "time")
	{
		value = m_ElapseTime;
		return true;
	}
	else if(fieldName == "ThinkTime")
	{
		value = m_ThinkTime;
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
	else if(fieldName == "key_pressed")
	{
		value = CCD->Input()->GetKeyboardInputNoWait();
		return true;
	}
	else if(fieldName == "player_X")
	{
		geVec3d Pos;
		Pos = CCD->Player()->Position();
		value = Pos.X;
		return true;
	}
	else if(fieldName == "player_Y")
	{
		geVec3d Pos;
		Pos = CCD->Player()->Position();
		value = Pos.Y;
		return true;
	}
	else if(fieldName == "player_Z")
	{
		geVec3d Pos;
		Pos = CCD->Player()->Position();
		value = Pos.Z;
		return true;
	}
	// Check if left mouse button is pressed
	else if(fieldName == "lbutton_pressed")
	{
		// value = !((GetAsyncKeyState(VK_LBUTTON) & 0x8000) == 0);
		int SwapButton = GetSystemMetrics(SM_SWAPBUTTON);

		if(SwapButton)
		{
			value = (GetAsyncKeyState(VK_RBUTTON) < 0);
		}
		else
		{
			value = (GetAsyncKeyState(VK_LBUTTON) < 0);
		}

		return true;
	}
	// Check if right mouse button is pressed
	else if(fieldName == "rbutton_pressed")
	{
		// value = !((GetAsyncKeyState(VK_RBUTTON) & 0x8000) == 0);
		int SwapButton = GetSystemMetrics(SM_SWAPBUTTON);

		if(SwapButton)
		{
			value = (GetAsyncKeyState(VK_LBUTTON) < 0);
		}
		else
		{
			value = (GetAsyncKeyState(VK_RBUTTON) < 0);
		}

		return true;
	}
	// Check if middle mouse button is pressed
	else if(fieldName == "mbutton_pressed")
	{
		// value = !((GetAsyncKeyState(VK_MBUTTON) & 0x8000) == 0);
		value = (GetAsyncKeyState(VK_MBUTTON) < 0);

		return true;
	}
	else
		return skScriptedExecutable::getValue(fieldName, attribute, value);
}


/* ------------------------------------------------------------------------------------ */
// setValue
/* ------------------------------------------------------------------------------------ */
bool ControllerObject::setValue(const skString& fieldName, const skString& attribute, const skRValue& value)
{
	if(fieldName == "think")
	{
		strcpy(Order, value.str());
		return true;
	}
	else if(fieldName == "ThinkTime")
	{
		m_ThinkTime = value.floatValue();
		return true;
	}
	else if(fieldName == "time")
	{
		m_ElapseTime = value.floatValue();
		return true;
	}
	else
	{
		return skScriptedExecutable::setValue(fieldName, attribute, value);
	}
}


/* ------------------------------------------------------------------------------------ */
// CLevelController
//
// Default constructor
/* ------------------------------------------------------------------------------------ */
CLevelController::CLevelController() : m_ConsoleBlock(0)
{
	// Ok, check to see if there is a LevelController in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "LevelController");

	if(!pSet)
		return;									// No LevelController, how odd...

	geEntity *pEntity;
	// Ok, we have a LevelController.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		LevelController *pLC = static_cast<LevelController*>(geEntity_GetUserData(pEntity));

		// Check for entity name
		if(EffectC_IsStringNull(pLC->szEntityName))
		{
			char szName[128];
			geEntity_GetName(pEntity, szName, 128);
			pLC->szEntityName = szName;
		}

		// Ok, put this entity into the Global Entity Registry
		CCD->EntityRegistry()->AddEntity(pLC->szEntityName, "LevelController");

		// Load the script
		if(!EffectC_IsStringNull(pLC->ScriptName))
		{
			char script[128] = "scripts\\";
			strcat(script, pLC->ScriptName);

			try
			{
				pLC->Data = new ControllerObject(script);
			}
			catch(skParseException &e)
			{
				char szBug[256];
				sprintf(szBug, "Parse Script Error for Controller script");
				CCD->ReportError(szBug, false);
				strcpy(szBug, e.toString());
				CCD->ReportError(szBug, false);
				return;
			}
			catch(skBoundsException &e)
			{
				char szBug[256];
				sprintf(szBug, "Bounds Script Error for Controller script");
				CCD->ReportError(szBug, false);
				strcpy(szBug, e.toString());
				CCD->ReportError(szBug, false);
				return;
			}
			catch(skRuntimeException &e)
			{
				char szBug[256];
				sprintf(szBug, "Runtime Script Error for Controller script");
				CCD->ReportError(szBug, false);
				strcpy(szBug, e.toString());
				CCD->ReportError(szBug, false);
				return;
			}
			catch(skTreeNodeReaderException &e)
			{
				char szBug[256];
				sprintf(szBug, "Reader Script Error for Controller script");
				CCD->ReportError(szBug, false);
				strcpy(szBug, e.toString());
				CCD->ReportError(szBug, false);
				return;
			}
			catch (...)
			{
				char szBug[256];
				sprintf(szBug, "Script Error for Controller script");
				CCD->ReportError(szBug, false);
				return;
			}

			ControllerObject *Object = static_cast<ControllerObject*>(pLC->Data);

			strcpy(Object->szName, pLC->szEntityName);

			if(!EffectC_IsStringNull(pLC->InitOrder))
			{
				skRValueArray args;
				skRValue ret;

				strcpy(Object->Order, pLC->InitOrder);

				try
				{
					Object->method(skString(Object->Order), args, ret,CCD->GetskContext()); //change simkin
				}
				catch(skRuntimeException &e)
				{
					char szBug[256];
					sprintf(szBug, "Runtime Script Error for Controller script");
					CCD->ReportError(szBug, false);
					strcpy(szBug, e.toString());
					CCD->ReportError(szBug, false);
					return;
				}
				catch(skParseException &e)
				{
					char szBug[256];
					sprintf(szBug, "Parse Script Error for Controller script");
					CCD->ReportError(szBug, false);
					strcpy(szBug, e.toString());
					CCD->ReportError(szBug, false);
					return;
				}
				catch(...)
				{
					char szBug[256];
					sprintf(szBug, "Script Error for Controller script");
					CCD->ReportError(szBug, false);
					return;
				}
			}

			if(!EffectC_IsStringNull(pLC->StartOrder))
				strcpy(Object->Order, pLC->StartOrder);
			else
				Object->Order[0] = '\0';
		}
	}
}


/* ------------------------------------------------------------------------------------ */
// ~CLevelController
//
// Default destructor, clean up anything we allocated on construction
/* ------------------------------------------------------------------------------------ */
CLevelController::~CLevelController()
{
	// Ok, check to see if there are LevelControllers in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "LevelController");

	if(pSet)
	{
		geEntity *pEntity;
		for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
			pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
		{
			LevelController *pSource = static_cast<LevelController*>(geEntity_GetUserData(pEntity));

			if(pSource->Data)
			{
				ControllerObject *Object = static_cast<ControllerObject*>(pSource->Data);
				delete Object;
				pSource->Data = NULL;
			}
		}
	}
}


/* ------------------------------------------------------------------------------------ */
// Tick
/* ------------------------------------------------------------------------------------ */
void CLevelController::Tick(geFloat dwTicks)
{
	m_ConsoleBlock = 0;

	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "LevelController");

	if(pSet)
	{
		geEntity *pEntity;
		for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
			pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
		{
			LevelController *pLC = static_cast<LevelController*>(geEntity_GetUserData(pEntity));

			ControllerObject *Object = static_cast<ControllerObject*>(pLC->Data);

			if(!EffectC_IsStringNull(Object->Order))
			{
				skRValueArray args;
				skRValue ret;

				if(Object->m_Console)
				{
					char szBug[256];
					sprintf(szBug, "%s %s", Object->szName, Object->Order);
					strcpy(Object->m_ConsoleHeader, szBug);
				}

				if(Object->m_Console && m_ConsoleBlock < 4)
				{
					int x, y;

					x = 5;
					y = (115 * m_ConsoleBlock) + 5;

					geEngine_Printf(CCD->Engine()->Engine(), x, y, "%s", Object->m_ConsoleHeader);
					y += 10;

					if(!EffectC_IsStringNull(Object->m_ConsoleError))
						geEngine_Printf(CCD->Engine()->Engine(), x, y, "%s", Object->m_ConsoleError);

					for(int i=0; i<DEBUGLINES; ++i)
					{
						y += 10;

						if(!EffectC_IsStringNull(Object->m_ConsoleDebug[i]))
							geEngine_Printf(CCD->Engine()->Engine(), x, y, "%s", Object->m_ConsoleDebug[i]);
					}

					++m_ConsoleBlock;
				}

				Object->m_ElapseTime += (dwTicks*0.001f);
				Object->m_ThinkTime  -= (dwTicks*0.001f);

				if(Object->m_ThinkTime > 0.0f)
					continue;

				Object->m_ThinkTime = 0.0f;
				strcpy(Object->m_ThinkOrder, Object->Order);

				try
				{
					Object->method(skString(Object->Order), args, ret,CCD->GetskContext());
				}
				catch(skRuntimeException &e)
				{
					char szBug[256];
					sprintf(szBug, "Runtime Script Error for Controller script");
					CCD->ReportError(szBug, false);
					strcpy(szBug, e.toString());
					CCD->ReportError(szBug, false);
					return;
				}
				catch(skParseException &e)
				{
					char szBug[256];
					sprintf(szBug, "Parse Script Error for Controller script");
					CCD->ReportError(szBug, false);
					strcpy(szBug, e.toString());
					CCD->ReportError(szBug, false);
					return;
				}
				catch(...)
				{
					char szBug[256];
					sprintf(szBug, "Script Error for Controller script");
					CCD->ReportError(szBug, false);
					return;
				}
			}
		}
	}
}


/* ----------------------------------- END OF FILE ------------------------------------ */

