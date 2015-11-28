/*
  CLevelController.cpp:	 The Level Controller

  This file contains the Level Controller implementation
	
*/

//	Include the One True Header

#include "RabidFramework.h"
#include "CCommonData.h"

#include "Simkin\\skScriptedExecutable.h"
#include "Simkin\\skRValue.h"
#include "Simkin\\skRValueArray.h"
#include "Simkin\\skRuntimeException.h"
#include "Simkin\\skParseException.h"
#include "Simkin\\skBoundsException.h"
#include "Simkin\\skTreeNode.h"

extern geSound_Def *SPool_Sound(char *SName);

// ControllerObject
ControllerObject::ControllerObject(char *fileName) : skScriptedExecutable(fileName)
{
	Order[0] = '\0';
	ElapseTime = 0.0f;
   ThinkTime = 0.0;
   ThinkOrder[0] = '\0';
   szName[0] = '\0';
	console = false;
	ConsoleHeader = NULL;
	ConsoleError = NULL;
	for(int i=0; i<DEBUGLINES; i++)
		ConsoleDebug[i] = NULL;
}

ControllerObject::~ControllerObject()
{
	if(ConsoleHeader)
		free(ConsoleHeader);
	if(ConsoleError)
		free(ConsoleError);
	for(int i=0; i<DEBUGLINES; i++)
	{
		if(ConsoleDebug[i])
			free(ConsoleDebug[i]);
	}
}

bool ControllerObject::method(const skString& methodName, skRValueArray& arguments, skRValue& returnValue)
{
	char  string1[128];
	float float1;
   bool  bool1;
//	int   int1;

	string1[0] = '\0';

	if (IS_METHOD(methodName, "SetPlatformTargetTime"))
	{
		PARMCHECK(2);
		strcpy(string1, arguments[0].str());
		float1 = arguments[1].floatValue();
      MovingPlatform *pEntity;
      CCD->Platforms()->LocateEntity(string1, (void**)&pEntity);
      CCD->ModelManager()->SetTargetTime(pEntity->Model, float1);
		return true;
	}
	else if (IS_METHOD(methodName, "GetPlatformTargetTime"))
	{
		PARMCHECK(1);
		strcpy(string1, arguments[0].str());
      MovingPlatform *pEntity;
      CCD->Platforms()->LocateEntity(string1, (void**)&pEntity);
      CCD->ModelManager()->GetTargetTime(pEntity->Model, &float1);
   	returnValue = (float)float1;
		return true;
	}
	else if (IS_METHOD(methodName, "GetPlatformCurrentTime"))
	{
		PARMCHECK(1);
		strcpy(string1, arguments[0].str());
      MovingPlatform *pEntity;
      CCD->Platforms()->LocateEntity(string1, (void**)&pEntity);
      CCD->ModelManager()->GetModelCurrentTime(pEntity->Model, &float1);
   	returnValue = (float)float1;
		return true;
	}
	else if (IS_METHOD(methodName, "PlatformCollision"))
	{
		PARMCHECK(1);
		strcpy(string1, arguments[0].str());
      MovingPlatform *pEntity;
      CCD->Platforms()->LocateEntity(string1, (void**)&pEntity);
      CCD->ModelManager()->ModelInCollision(pEntity->Model, &bool1);
   	returnValue = (bool)bool1;
		return true;
	}
	if (IS_METHOD(methodName, "SetDoorTargetTime"))
	{
		PARMCHECK(2);
		strcpy(string1, arguments[0].str());
		float1 = arguments[1].floatValue();
      Door *pEntity;
      CCD->Doors()->LocateEntity(string1, (void**)&pEntity);
      CCD->ModelManager()->SetTargetTime(pEntity->Model, float1);
		return true;
	}
	else if (IS_METHOD(methodName, "GetDoorTargetTime"))
	{
		PARMCHECK(1);
		strcpy(string1, arguments[0].str());
      Door *pEntity;
      CCD->Doors()->LocateEntity(string1, (void**)&pEntity);
      CCD->ModelManager()->GetTargetTime(pEntity->Model, &float1);
   	returnValue = (float)float1;
		return true;
	}
	else if (IS_METHOD(methodName, "GetDoorCurrentTime"))
	{
		PARMCHECK(1);
		strcpy(string1, arguments[0].str());
      Door *pEntity;
      CCD->Doors()->LocateEntity(string1, (void**)&pEntity);
      CCD->ModelManager()->GetModelCurrentTime(pEntity->Model, &float1);
   	returnValue = (float)float1;
		return true;
	}
	else if (IS_METHOD(methodName, "DoorCollision"))
	{
		PARMCHECK(1);
		strcpy(string1, arguments[0].str());
      Door *pEntity;
      CCD->Doors()->LocateEntity(string1, (void**)&pEntity);
      CCD->ModelManager()->ModelInCollision(pEntity->Model, &bool1);
   	returnValue = (bool)bool1;
		return true;
	}
	else if (IS_METHOD(methodName, "ShowWallDecal"))
	{
		PARMCHECK(1);
		strcpy(string1, arguments[0].str());
      CCD->WallDecals()->SetProgrammedTrigger(string1, GE_TRUE);
		return true;
	}
	else if (IS_METHOD(methodName, "HideWallDecal"))
	{
		PARMCHECK(1);
		strcpy(string1, arguments[0].str());
      CCD->WallDecals()->SetProgrammedTrigger(string1, GE_FALSE);
		return true;
	}
	else if (IS_METHOD(methodName, "SetWallDecalBitmap"))
	{
		PARMCHECK(2);
		strcpy(string1, arguments[0].str());
		int bmnum = arguments[1].intValue();
      CCD->WallDecals()->SetCurrentBitmap(string1, bmnum);
		return true;
	}
	else if (IS_METHOD(methodName, "ActivateTrigger"))
	{
		PARMCHECK(1);
		strcpy(string1, arguments[0].str());
      CCD->Triggers()->HandleTriggerEvent(string1);
		return true;
	}
	else if (IS_METHOD(methodName, "GetEventState"))
	{
		PARMCHECK(1);
		strcpy(string1, arguments[0].str());
		returnValue = (bool)GetTriggerState(string1);
		return true;
	}
	else if (IS_METHOD(methodName, "SetEventState"))
	{
		PARMCHECK(2);
		strcpy(string1, arguments[0].str());
		bool flag = arguments[1].boolValue();
		CCD->Pawns()->AddEvent(string1, flag);
		return true;
	}
	else if (IS_METHOD(methodName, "Start3DAudioSource"))
	{
		PARMCHECK(1);
		strcpy(string1, arguments[0].str());
      CCD->Audio3D()->SetProgrammedTrigger(string1, GE_TRUE);
		return true;
	}
	else if (IS_METHOD(methodName, "Stop3DAudioSource"))
	{
		PARMCHECK(1);
		strcpy(string1, arguments[0].str());
      CCD->Audio3D()->SetProgrammedTrigger(string1, GE_FALSE);
		return true;
	}
	else if (IS_METHOD(methodName, "Get3DAudioSourceState"))
	{
		PARMCHECK(1);
		strcpy(string1, arguments[0].str());
      bool state = CCD->Audio3D()->IsPlaying(string1);
      returnValue = (bool)state;
		return true;
	}
	else if (IS_METHOD(methodName, "PlaySound"))
	{
		PARMCHECK(1);
		strcpy(string1, arguments[0].str());
		if(!EffectC_IsStringNull(string1))
		{
			geVec3d Position = CCD->Player()->Position();
			Snd 	Sound;
			memset( &Sound, 0, sizeof( Sound ) );
			geVec3d_Copy( &(Position), &( Sound.Pos ) );
			Sound.Min=CCD->GetAudibleRadius();
			if(arguments.entries()==2)
				Sound.Min=arguments[1].floatValue();
			Sound.Loop=GE_FALSE;
			Sound.SoundDef=SPool_Sound(string1);
			if(Sound.SoundDef!=NULL)
				CCD->EffectManager()->Item_Add(EFF_SND, (void *)&Sound);
		}
		return true;
	}
	else if (IS_METHOD(methodName, "Console"))
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
			for(int i=0; i<DEBUGLINES; i++)
			{
				if(ConsoleDebug[i])
					free(ConsoleDebug[i]);
			}
		}
		return true;
	}
	else if (IS_METHOD(methodName, "debug"))
	{
		PARMCHECK(1);
		strcpy(string1, arguments[0].str());
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
				strcpy(ConsoleDebug[index], string1);
			}
			else
			{
				for(i=1;i<DEBUGLINES;i++)
				{
					strcpy(ConsoleDebug[i-1], ConsoleDebug[i]);
				}
				strcpy(ConsoleDebug[DEBUGLINES-1], string1); 
			}
		}
		return true;
	}
	else if (IS_METHOD(methodName, "random"))
	{
		PARMCHECK(2);
		float param1 = arguments[0].floatValue();
		float param3 = arguments[1].floatValue();
		if(param1<=param3)
			returnValue = (int)EffectC_Frand(param1, param3);
		else
			returnValue = (int)EffectC_Frand(param3, param1);
		return true;
	}


	else if (IS_METHOD(methodName, "StringCopy"))
	{
		PARMCHECK(1);
		strcpy(string1, arguments[0].str());
		returnValue = skString(string1);
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
	else if (IS_METHOD(methodName, "Integer"))
	{
		PARMCHECK(1);
		int temp = arguments[0].intValue();
		returnValue = (int)temp;
		return true;
	}
	else if (IS_METHOD(methodName, "ModifyAtribute"))
	{
		PARMCHECK(2);
		strcpy(string1, arguments[0].str());
		CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
		returnValue = (int)theInv->Modify(string1, arguments[1].intValue());
		return true;
	}

	else if (IS_METHOD(methodName, "GetAttribute"))
	{
		PARMCHECK(1);
		strcpy(string1, arguments[0].str());
		CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
		returnValue = (int)theInv->Value(string1);
		return true;
	}

	else if (IS_METHOD(methodName, "SetAttribute"))
	{
		PARMCHECK(2);
		strcpy(string1, arguments[0].str());
		CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
		returnValue = (int)theInv->Set(string1, arguments[1].intValue());
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
		strcpy(string1, arguments[0].str());
		CCD->Player()->SetUseAttribute(string1);
		CCD->HUD()->ActivateElement(string1,true);
		return true;
	}

	else
	{
		return skScriptedExecutable::method(methodName, arguments, returnValue);
	}
}

bool ControllerObject::getValue(const skString& fieldName, const skString& attribute, skRValue& value)
{
	if (fieldName == "time")
	{
		value = ElapseTime;
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
	else
		return skScriptedExecutable::getValue(fieldName, attribute, value);
}

bool ControllerObject::setValue(const skString& fieldName, const skString& attribute, const skRValue& value)
{
	if (fieldName == "think")
	{
		strcpy(Order, value.str());
		return true;
	}
   else if (fieldName == "ThinkTime")
   {
      ThinkTime = value.floatValue();
      return true;
   }
	if (fieldName == "time")
	{
		ElapseTime = value.floatValue();
		return true;
	}
	else
		return skScriptedExecutable::setValue(fieldName, attribute, value);
}


//	CLevelController
//
//	Default constructor

CLevelController::CLevelController()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
   
	//	Ok, check to see if there is a LevelController in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "LevelController");
	
	if (!pSet) 
		return;									// No LevelController, how odd...
	
	//	Ok, we have a LevelController.
	
   for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity; pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
   {
		LevelController *pLC = (LevelController*)geEntity_GetUserData(pEntity);
      // Check for entity name
		if (EffectC_IsStringNull(pLC->szEntityName))
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
			catch(skParseException e)
			{
				char szBug[256];
				sprintf(szBug, "Parse Script Error for Controller script");
				CCD->ReportError(szBug, false);
				strcpy(szBug, e.toString());
				CCD->ReportError(szBug, false);
				return;
			}
			catch(skBoundsException e)
			{
				char szBug[256];
				sprintf(szBug, "Bounds Script Error for Controller script");
				CCD->ReportError(szBug, false);
				strcpy(szBug, e.toString());
				CCD->ReportError(szBug, false);
				return;
			}
			catch(skRuntimeException e)
			{
				char szBug[256];
				sprintf(szBug, "Runtime Script Error for Controller script");
				CCD->ReportError(szBug, false);
				strcpy(szBug, e.toString());
				CCD->ReportError(szBug, false);
				return;
			}
			catch(skTreeNodeReaderException e)
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
         
         ControllerObject *Object;
         Object = (ControllerObject*)pLC->Data;

			strcpy(Object->szName, pLC->szEntityName);

         if(!EffectC_IsStringNull(pLC->InitOrder))
			{
				skRValueArray args(1);
				skRValue ret;

				strcpy(Object->Order, pLC->InitOrder);
				try
				{
					Object->method(skString(Object->Order), args, ret);
				}
				catch(skRuntimeException e)
				{
					char szBug[256];
					sprintf(szBug, "Runtime Script Error for Controller script");
					CCD->ReportError(szBug, false);
					strcpy(szBug, e.toString());
					CCD->ReportError(szBug, false);
					return;
				}
				catch(skParseException e)
				{
					char szBug[256];
					sprintf(szBug, "Parse Script Error for Controller script");
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
			}
			if(!EffectC_IsStringNull(pLC->StartOrder))
				strcpy(Object->Order, pLC->StartOrder);
			else
				Object->Order[0] = '\0';
      }
   }
   //Ok, we're done!
	return;
}

//	~CLevelController
//
//	Default destructor, clean up anything we allocated on construction

CLevelController::~CLevelController()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	//	Ok, check to see if there are LevelControllers in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "LevelController");
	
	if(pSet) 
	{
		for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity; pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
		{
			LevelController *pSource = (LevelController*)geEntity_GetUserData(pEntity);
			if(pSource->Data)
			{
				ControllerObject *Object = (ControllerObject *)pSource->Data;
				delete Object;
				pSource->Data = NULL;
			}
		}
	}
}


void CLevelController::Tick(geFloat TimeTicks)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	ConsoleBlock = 0;

	pSet = geWorld_GetEntitySet(CCD->World(), "LevelController");
	
	if(pSet) 
	{
		for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
		{
			LevelController *pLC = (LevelController*)geEntity_GetUserData(pEntity);

         ControllerObject *Object;
         Object = (ControllerObject*)pLC->Data;

         if(!EffectC_IsStringNull(Object->Order))
		   {
			   skRValueArray args(1);
			   skRValue ret;

				if(Object->console)
				{
					char szBug[256];
					sprintf(szBug, "%s %s",Object->szName, Object->Order);
					strcpy(Object->ConsoleHeader, szBug);
				}
            if(Object->console && ConsoleBlock<4)
			   {
				   int x,y;
				   
				   x = 5;
				   y= (115*ConsoleBlock)+5;

				   geEngine_Printf(CCD->Engine()->Engine(), x, y,"%s",Object->ConsoleHeader);
				   y += 10;
				   if(!EffectC_IsStringNull(Object->ConsoleError))
					   geEngine_Printf(CCD->Engine()->Engine(), x, y,"%s", Object->ConsoleError);

				   for(int i=0;i<DEBUGLINES;i++)
				   {
					   y += 10;
					   if(!EffectC_IsStringNull(Object->ConsoleDebug[i]))
						   geEngine_Printf(CCD->Engine()->Engine(), x, y,"%s",Object->ConsoleDebug[i]);
				   }

				   ConsoleBlock+=1;
			   }
	         Object->ElapseTime += (TimeTicks*0.001f);
	         Object->ThinkTime -= (TimeTicks*0.001f);
	         if(Object->ThinkTime>0.0f)
		         continue;
            Object->ThinkTime = 0.0;
	         strcpy(Object->ThinkOrder, Object->Order);

			   try
			   {
				   Object->method(skString(Object->Order), args, ret);
			   }
			   catch(skRuntimeException e)
			   {
				   char szBug[256];
				   sprintf(szBug, "Runtime Script Error for Controller script");
				   CCD->ReportError(szBug, false);
				   strcpy(szBug, e.toString());
				   CCD->ReportError(szBug, false);
				   return;
			   }
			   catch(skParseException e)
			   {
				   char szBug[256];
				   sprintf(szBug, "Parse Script Error for Controller script");
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

         }
		}
	}

   return;
}



