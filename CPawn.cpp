
#include "RabidFramework.h"

extern geSound_Def *SPool_Sound(char *SName);

#include "Simkin\\skScriptedExecutable.h"
#include "Simkin\\skRValue.h"
#include "Simkin\\skRValueArray.h"
#include "Simkin\\skRuntimeException.h"
#include "Simkin\\skParseException.h"
#include "Simkin\\skBoundsException.h"
#include "Simkin\\skTreeNode.h"

//
// ScriptedObject class
//

ScriptedObject::ScriptedObject(char *fileName) : skScriptedExecutable(fileName)
{
	highlevel = true;
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
	PainOrder[0] = '\0';
	PainActive = false;
	Trigger = NULL;
	TriggerWait = false;
	Icon = NULL;
	SoundIndex = -1;
	AudibleRadius = CCD->GetAudibleRadius();
	FacePlayer = false;
	FaceAxis = true;
	Attribute[0] = '\0';
	DeadOrder[0] = '\0';
	szName[0] = '\0';
	Converse = NULL;
	UseKey = true;
	FOV = -2.0f;
	Group[0] = '\0';
	TargetGroup[0] = '\0';
	HostilePlayer = true;
	HostileDiff = false;
	HostileSame = false;
	TargetFind = false;
	TargetActor = NULL;
	TargetDisable = false;
	console = false;
	ConsoleHeader = NULL;
	ConsoleError = NULL;
	for(int i=0; i<DEBUGLINES; i++)
		ConsoleDebug[i] = NULL;
	strcpy(Indicate, "+"); 
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

// calls a method in this object
bool ScriptedObject::method(const skString& methodName, skRValueArray& arguments,skRValue& returnValue)
{
	bool flag;

	if(highlevel)
		flag = highmethod(methodName, arguments, returnValue);
	else
		flag = lowmethod(methodName, arguments, returnValue);

	return flag;
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
	for(int ii=0;ii<MAXFLAGS;ii++)
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
				catch(skParseException e)
				{
					char szBug[256];
					sprintf(szBug, "Parse Script Error for %s", pSource->szEntityName);
					CCD->ReportError(szBug, false);
					strcpy(szBug, e.toString());
					CCD->ReportError(szBug, false);
					continue;
				}
				catch(skBoundsException e)
				{
					char szBug[256];
					sprintf(szBug, "Bounds Script Error for %s", pSource->szEntityName);
					CCD->ReportError(szBug, false);
					strcpy(szBug, e.toString());
					CCD->ReportError(szBug, false);
					continue;
				}
				catch(skRuntimeException e)
				{
					char szBug[256];
					sprintf(szBug, "Runtime Script Error for %s", pSource->szEntityName);
					CCD->ReportError(szBug, false);
					strcpy(szBug, e.toString());
					CCD->ReportError(szBug, false);
					continue;
				}
				catch(skTreeNodeReaderException e)
				{
					char szBug[256];
					sprintf(szBug, "Reader Script Error for %s", pSource->szEntityName);
					CCD->ReportError(szBug, false);
					strcpy(szBug, e.toString());
					CCD->ReportError(szBug, false);
					continue;
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
							geVec3d FillColor = {0.0f, 0.0f, 0.0f};
							geVec3d AmbientColor = {0.0f, 0.0f, 0.0f};
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
					catch(skParseException e)
					{
						char szBug[256];
						sprintf(szBug, "Parse Conversation Script Error for %s", Object->szName);
						CCD->ReportError(szBug, false);
						strcpy(szBug, e.toString());
						CCD->ReportError(szBug, false);
						continue;
					}
					catch(skBoundsException e)
					{
						char szBug[256];
						sprintf(szBug, "Bounds Conversation Script Error for %s", Object->szName);
						CCD->ReportError(szBug, false);
						strcpy(szBug, e.toString());
						CCD->ReportError(szBug, false);
						continue;
					}
					catch(skRuntimeException e)
					{
						char szBug[256];
						sprintf(szBug, "Runtime Conversation Script Error for %s", Object->szName);
						CCD->ReportError(szBug, false);
						strcpy(szBug, e.toString());
						CCD->ReportError(szBug, false);
						continue;
					}
					catch(skTreeNodeReaderException e)
					{
						char szBug[256];
						sprintf(szBug, "Reader Conversation Script Error for %s", Object->szName);
						CCD->ReportError(szBug, false);
						strcpy(szBug, e.toString());
						CCD->ReportError(szBug, false);
						continue;
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
	
	//	Ok, check to see if there are Pawns in this world
	
	ConsoleBlock = 0;

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
				if(Object->highlevel)
					TickHigh(pSource, Object, dwTicks);
				else
					TickLow(pSource, Object, dwTicks);
				
				
				if(Object->console && ConsoleBlock<4)
				{
					int x,y;
					
					x = 5;
					y= (115*ConsoleBlock)+5;

					geEngine_Printf(CCD->Engine()->Engine(), x, y,"%s",Object->ConsoleHeader);
					y += 10;
					if(!EffectC_IsStringNull(Object->ConsoleError))
						geEngine_Printf(CCD->Engine()->Engine(), x, y,"%s %s",Object->Indicate, Object->ConsoleError);

					for(int i=0;i<DEBUGLINES;i++)
					{
						y += 10;
						if(!EffectC_IsStringNull(Object->ConsoleDebug[i]))
							geEngine_Printf(CCD->Engine()->Engine(), x, y,"%s",Object->ConsoleDebug[i]);
					}

					ConsoleBlock+=1;
				}
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

void CPawn::PreLoad(char *filename)
{
	FILE *fdInput = NULL;
	char szInputString[1024] = {""};
	CString str;
	int i, j;
	char file[256];

	fdInput = fopen(filename, "rt");
	if(!fdInput)
		return;

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
			while(!(str.GetAt(j)=='"' || str.GetAt(j)=='[') && j>=0)
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

void CPawn::ParmCheck(int Entries, int Desired, char *Order, char *szName, const skString& methodname)
{
	if(Entries>=Desired)
		return;
	char param0[128];
	strcpy(param0, methodname);
	char szError[256];
	sprintf(szError,"Incorrect # of parameters in command '%s' in Script '%s'  Order  '%s'", param0, szName, Order);
	CCD->ReportError(szError, false);
	CCD->ShutdownLevel();
	delete CCD;
	MessageBox(NULL, szError,"Player", MB_OK);
	exit(-333);
}


//	SaveTo
//
//	Save the current state of every Pawn in the current world
//	..off to an open file.

int CPawn::SaveTo(FILE *SaveFD, bool type)
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
		Pawn *pSource = (Pawn*)geEntity_GetUserData(pEntity);
		if(pSource->Data)
		{
			ScriptedObject *Object = (ScriptedObject *)pSource->Data;
				
		}
	}

	return RGF_SUCCESS;
}

//	RestoreFrom
//
//	Restore the state of every Pawn in the current world from an
//	..open file.

int CPawn::RestoreFrom(FILE *RestoreFD, bool type)
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
		Pawn *pSource = (Pawn*)geEntity_GetUserData(pEntity);
		if(pSource->Data)
		{
			ScriptedObject *Object = (ScriptedObject *)pSource->Data;
				
		}
	}
	
	return RGF_SUCCESS;
}