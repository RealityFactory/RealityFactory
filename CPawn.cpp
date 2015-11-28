/************************************************************************************//**
 * @file CPawn.cpp
 * @brief CPawn class declaration
 * @author Ralph Deane
 *//*
 * Copyright (c) Ralph Deane; All rights reserved.
 ****************************************************************************************/

#include "RabidFramework.h"

extern geSound_Def *SPool_Sound(const char *SName);
// changed QD 06/26/04
extern geBitmap *TPool_Bitmap(const char *DefaultBmp, const char *DefaultAlpha,
							  const char *BName, const char *AName);
// end change

#include "Simkin\\skScriptedExecutable.h"
#include "Simkin\\skRValue.h"
#include "Simkin\\skRValueArray.h"
#include "Simkin\\skRuntimeException.h"
#include "Simkin\\skParseException.h"
#include "Simkin\\skBoundsException.h"
#include "Simkin\\skTreeNode.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/* ------------------------------------------------------------------------------------ */
// ScriptedObject class
/* ------------------------------------------------------------------------------------ */
ScriptedObject::ScriptedObject(char *fileName) : skScriptedExecutable(fileName,CCD->GetskContext())// change simkin
{
	highlevel		= true;
	RunOrder		= false;
	Actor			= NULL;
	WeaponActor		= NULL;
	Top				= NULL;
	Bottom			= NULL;
	Index			= NULL;
	Stack			= NULL;
	active			= false;
	ValidPoint		= false;
	ActionActive	= false;
	Time			= 0.0f;
	DistActive		= false;
	AvoidOrder[0]	= '\0';
	AvoidMode		= false;
	PainOrder[0]	= '\0';
	PainActive		= false;
	Trigger			= NULL;
	TriggerWait		= false;
	Icon			= NULL;
	SoundIndex		= -1;
	AudibleRadius	= CCD->GetAudibleRadius();
	FacePlayer		= false;
	FaceAxis		= true;
	Attribute[0]	= '\0';
	DeadOrder[0]	= '\0';
	szName[0]		= '\0';
	Converse		= NULL;
	UseKey			= true;
	FOV				= -2.0f;
	Group[0]		= '\0';
	TargetGroup[0]	= '\0';
	HostilePlayer	= true;
	HostileDiff		= false;
	HostileSame		= false;
	TargetFind		= false;
	TargetActor		= NULL;
	TargetDisable	= false;
	console			= false;
	ConsoleHeader	= NULL;
	ConsoleError	= NULL;

	for(int i=0; i<DEBUGLINES; i++)
		ConsoleDebug[i] = NULL;

	strcpy(Indicate, "+");

	EnvironmentMapping	= false;
	collision			= false;
	pushable			= false;
	SoundLoop			= false;
// Added QD FindPointOrder code
	PointFind			= false;
// changed QD 06/26/04
	ShadowAlpha			= 0.0f;
	ShadowBitmap[0]		= '\0';
	ShadowAlphamap[0]	= '\0';
// end change
// begin change gekido
// projected shadows
	ProjectedShadows	= false;
// changed QD Shadows
	StencilShadows		= GE_FALSE;
// end change
// changed QD 07/21/04
	AmbientLightFromFloor = true;
// end change
// changed Nout 12/15/05
	Prev_HL_Actor		= NULL;
	Prev_HL_FillColor.r	= Prev_HL_FillColor.g = Prev_HL_FillColor.b = Prev_HL_FillColor.a = 255.0f;
	Prev_HL_AmbientColor.r = Prev_HL_AmbientColor.g = Prev_HL_AmbientColor.b = Prev_HL_AmbientColor.a = 255.0f;
	Prev_HL_AmbientLightFromFloor = GE_TRUE;
// end change
}

/* ------------------------------------------------------------------------------------ */
//	Destructor
/* ------------------------------------------------------------------------------------ */
ScriptedObject::~ScriptedObject()
{
	if(Actor)
	{
		CCD->RemoveScriptedObject(CCD->ActorManager()->GetEntityName(Actor));//change scripting
		CCD->ActorManager()->RemoveActor(Actor);
		geActor_Destroy(&Actor);
		Actor = NULL;
	}

	if(WeaponActor)
	{
		CCD->ActorManager()->RemoveActor(WeaponActor);
		geActor_Destroy(&WeaponActor);
		WeaponActor = NULL;
	}

	if(Icon)
	{
		geEngine_RemoveBitmap(CCD->Engine()->Engine(), Icon);
		geBitmap_Destroy(&Icon);
	}

	ActionList *pool, *temp;
	pool = Bottom;

	while(pool != NULL)
	{
		temp = pool->next;
		geRam_Free(pool);
		pool = temp;
	}

	ActionStack *spool, *stemp;
	spool = Stack;

	while(spool != NULL)
	{
		stemp = spool->next;
		pool = spool->Bottom;

		while(pool != NULL)
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

	while(tpool != NULL)
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

/* ------------------------------------------------------------------------------------ */
//	calls a method in this object
/* ------------------------------------------------------------------------------------ */
bool ScriptedObject::method(const skString &methodName, skRValueArray &arguments,
							skRValue &returnValue, skExecutableContext &ctxt)//change skimkin
{
	bool flag;

	if(highlevel)
		flag = highmethod(methodName, arguments, returnValue,ctxt);//change simkin
	else
		flag = lowmethod(methodName, arguments, returnValue,ctxt); //change simkin

	return flag;
}

/* ------------------------------------------------------------------------------------ */
// Pawn Class
/* ------------------------------------------------------------------------------------ */
CPawn::CPawn()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	Background	= NULL;
	Icon		= NULL;
// changed Nout 16/09/2003
	rBackground = NULL;
	rIcon		= NULL;
// end change
// changed Nout 12/15/05
	ReplyMenuBar= NULL;
	int i;

	for(i=0; i<MAXFILLAREA; i++)
	{
		FillScrArea[i].DoFillScreenArea = false;
		FillScrArea[i].FillScreenAreaKeep = false;
	}

	for(i=0; i<9; i++)
		GifFile[i] = NULL;

	GifX = GifY = 0;
// end change
	Events		= NULL;

	for(i=0; i<MAXFLAGS; i++)
		PawnFlag[i] = false;

	// Ok, check to see if there are Pawns in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "Pawn");

	if(pSet)
	{
		LoadConv(CCD->MenuManager()->GetConvtxts());
		BitmapCache.resize(0);
		WeaponCache.resize(0);
		AccessoryCache.resize(0);

		AttrFile.SetPath("pawn.ini");

		if(!AttrFile.ReadFile())
		{
			CCD->ReportError("[ERROR] Failed to open Pawn.ini file\n", false);
			return;
		}

		std::string KeyName = AttrFile.FindFirstKey();
		std::string Type, Vector;

		geBitmap_Info	BmpInfo;
		char szName[64];
// changed Nout 12/15/05
		char szNameAlpha[64];

		for(i=0; i<MAXTEXT; i++)
			TextMessage[i].ShowText = false;
// end change

		while(KeyName != "")
		{
			if(!strcmp(KeyName.c_str(), "Conversation"))
			{
				Type = AttrFile.GetValue(KeyName, "background");
				if(Type != "")
				{
					Type = "conversation\\"+Type;
					strcpy(szName, Type.c_str());

// changed Nout 12/15/05
					Type = AttrFile.GetValue(KeyName, "backgroundalpha");
					if(Type != "")
						Type = "conversation\\"+Type;
					strcpy(szNameAlpha, Type.c_str());

					Background = CreateFromFileAndAlphaNames(szName, szNameAlpha);
/*
					geBitmap *TB = CreateFromFileName(szName);
					geBitmap_GetInfo(TB, &BmpInfo, NULL);

					int TBw = BmpInfo.Width;
					int TBh = BmpInfo.Height;
					int TBx = (BmpInfo.Width - CCD->Engine()->Width()) / 2;

					if(TBx < 0)
						TBx = 0;
					else
						TBw = CCD->Engine()->Width();

					int TBy = (BmpInfo.Height - CCD->Engine()->Height()) / 2;

					if(TBy < 0)
						TBy = 0;
					else
						TBh = CCD->Engine()->Height();

					Background = geBitmap_Create(TBw, TBh, BmpInfo.MaximumMip+1, BmpInfo.Format);
					geBitmap_Blit(TB, TBx, TBy, Background, 0, 0, TBw, TBh);
					geBitmap_Destroy(&TB);
*/
// end change
					geEngine_AddBitmap(CCD->Engine()->Engine(), Background);
					geBitmap_GetInfo(Background, &BmpInfo, NULL);
					BackgroundX = (CCD->Engine()->Width() - BmpInfo.Width) / 2;
					BackgroundY = (CCD->Engine()->Height() - BmpInfo.Height) / 2;
				}

// begin add Nout 16/09/2003
				Type = AttrFile.GetValue(KeyName, "replybackground");
				if(Type != "")
				{
					Type = "conversation\\"+Type;
					strcpy(szName, Type.c_str());

// changed Nout 12/15/05
					Type = AttrFile.GetValue(KeyName, "replybackgroundalpha");
					if(Type!="")
						Type = "conversation\\"+Type;
					strcpy(szNameAlpha, Type.c_str());

					rBackground = CreateFromFileAndAlphaNames(szName, szNameAlpha);
/*
					geBitmap *rTB = CreateFromFileName(szName);
					geBitmap_GetInfo(rTB, &BmpInfo, NULL);

					  int rTBw = BmpInfo.Width;
					int rTBh = BmpInfo.Height;
					int rTBx = (BmpInfo.Width - CCD->Engine()->Width()) / 2;

					if(rTBx < 0)
						rTBx = 0;
					else
						rTBw = CCD->Engine()->Width();

					int rTBy = (BmpInfo.Height - CCD->Engine()->Height()) / 2;

					if(rTBy < 0)
						rTBy = 0;
					else
						rTBh = CCD->Engine()->Height();

					rBackground = geBitmap_Create(rTBw, rTBh, BmpInfo.MaximumMip+1, BmpInfo.Format);
					geBitmap_Blit(rTB, rTBx, rTBy, rBackground, 0, 0, rTBw, rTBh);
					geBitmap_Destroy(&rTB);
*/
					geEngine_AddBitmap(CCD->Engine()->Engine(), rBackground);
					geBitmap_GetInfo(rBackground, &BmpInfo, NULL);
					rBackgroundX = (CCD->Engine()->Width() - BmpInfo.Width) / 2;
					rBackgroundY = (CCD->Engine()->Height() - BmpInfo.Height) / 2;
				}
// end add Nout 16/09/2003

// changed Nout 12/15/05
				Type = AttrFile.GetValue(KeyName, "replymenubar");
				if(Type!="")
				{
					Type = "conversation\\"+Type;
					strcpy(szName, Type.c_str());

					Type = AttrFile.GetValue(KeyName, "replymenubaralpha");
					if(Type!="")
						Type = "conversation\\"+Type;
					strcpy(szNameAlpha, Type.c_str());

					ReplyMenuBar = CreateFromFileAndAlphaNames(szName, szNameAlpha);

					geEngine_AddBitmap(CCD->Engine()->Engine(), ReplyMenuBar);
					//geBitmap_GetInfo(ReplyMenuBar, &BmpInfo, NULL);
				}

				//preload gif-files for MenuReply
				for(int i=0; i<9; i++)
				{
					GifFile[i] = NULL;
					sprintf(szName, "giffile%d", i);
					Type = AttrFile.GetValue(KeyName, szName);
					if(Type!="")
					{
						strcpy(szName, Type.c_str());
						GifFile[i] = new CAnimGif(szName, kVideoFile);
					}
				}

				ReplyMenuFont = AttrFile.GetValueI(KeyName, "replymenufont");
				GifX = AttrFile.GetValueI(KeyName, "gifx");
				GifY = AttrFile.GetValueI(KeyName, "gify");
// end change

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

// begin add Nout 16/09/2003
				SpeachWindowX = AttrFile.GetValueI(KeyName, "speachwindowx");
				if(SpeachWindowX >= 0)
					BackgroundX = SpeachWindowX;

				SpeachWindowY = AttrFile.GetValueI(KeyName, "speachwindowy");
				if(SpeachWindowY >= 0)
					BackgroundY = SpeachWindowY;

				ReplyWindowX = AttrFile.GetValueI(KeyName, "replywindowx");
				if(SpeachWindowX >= 0)
					rBackgroundX = ReplyWindowX;

				ReplyWindowY = AttrFile.GetValueI(KeyName, "replywindowy");
				if(SpeachWindowY >= 0)
					rBackgroundY = ReplyWindowY;
// end add Nout 16/09/2003
				break;
			}

			KeyName = AttrFile.FindNextKey();
		}

		KeyName = AttrFile.FindFirstKey();
		while(KeyName != "")
		{
			Type = AttrFile.GetValue(KeyName, "type");

			if(Type == "weapon")
			{
				WeaponCache.resize(WeaponCache.size()+1);
				int keynum = WeaponCache.size()-1;
				WeaponCache[keynum].Name = KeyName;

				Type = AttrFile.GetValue(KeyName, "actorname");
				strcpy(WeaponCache[keynum].ActorName, Type.c_str());

				Vector = AttrFile.GetValue(KeyName, "actorrotation");

				if(Vector != "")
				{
					strcpy(szName, Vector.c_str());
					WeaponCache[keynum].Rotation = Extract(szName);
					WeaponCache[keynum].Rotation.X *= GE_PIOVER180;
					WeaponCache[keynum].Rotation.Y *= GE_PIOVER180;
					WeaponCache[keynum].Rotation.Z *= GE_PIOVER180;
				}

				geActor *Actor = CCD->ActorManager()->SpawnActor(WeaponCache[keynum].ActorName,
					WeaponCache[keynum].Rotation, WeaponCache[keynum].Rotation, "", "", NULL);

				CCD->ActorManager()->RemoveActor(Actor);
				geActor_Destroy(&Actor);
				Actor = NULL;
				WeaponCache[keynum].Scale = (float)AttrFile.GetValueF(KeyName, "actorscale");
				geVec3d FillColor = {0.0f, 0.0f, 0.0f};
				geVec3d AmbientColor = {0.0f, 0.0f, 0.0f};

				Vector = AttrFile.GetValue(KeyName, "fillcolor");

				if(Vector != "")
				{
					strcpy(szName, Vector.c_str());
					FillColor = Extract(szName);
				}

				Vector = AttrFile.GetValue(KeyName, "ambientcolor");

				if(Vector != "")
				{
					strcpy(szName, Vector.c_str());
					AmbientColor = Extract(szName);
				}

				WeaponCache[keynum].FillColor.r = FillColor.X;
				WeaponCache[keynum].FillColor.g = FillColor.Y;
				WeaponCache[keynum].FillColor.b = FillColor.Z;
				WeaponCache[keynum].FillColor.a = 255.0f;
				WeaponCache[keynum].AmbientColor.r = AmbientColor.X;
				WeaponCache[keynum].AmbientColor.g = AmbientColor.Y;
				WeaponCache[keynum].AmbientColor.b = AmbientColor.Z;
				WeaponCache[keynum].AmbientColor.a = 255.0f;

// changed QD 07/21/04
				WeaponCache[keynum].AmbientLightFromFloor = true;

				Type = AttrFile.GetValue(KeyName, "ambientlightfromfloor");

				if(Type == "false")
					WeaponCache[keynum].AmbientLightFromFloor = false;
// end change

				WeaponCache[keynum].EnvironmentMapping = false;

				Type = AttrFile.GetValue(KeyName, "environmentmapping");

				if(Type == "true")
				{
					WeaponCache[keynum].EnvironmentMapping = true;
					WeaponCache[keynum].AllMaterial = false;
					Type = AttrFile.GetValue(KeyName, "allmaterial");

					if(Type == "true")
						WeaponCache[keynum].AllMaterial = true;

					WeaponCache[keynum].PercentMapping = (float)AttrFile.GetValueF(KeyName, "percentmapping");
					WeaponCache[keynum].PercentMaterial = (float)AttrFile.GetValueF(KeyName, "percentmaterial");
				}
			}
			else if(Type == "accessory")
			{
				AccessoryCache.resize(AccessoryCache.size()+1);
				int keynum = AccessoryCache.size()-1;
				AccessoryCache[keynum].Name = KeyName;

				Type = AttrFile.GetValue(KeyName, "actorname");
				strcpy(AccessoryCache[keynum].ActorName, Type.c_str());

				Vector = AttrFile.GetValue(KeyName, "actorrotation");

				if(Vector != "")
				{
					strcpy(szName, Vector.c_str());
					AccessoryCache[keynum].Rotation = Extract(szName);
					AccessoryCache[keynum].Rotation.X *= GE_PIOVER180;
					AccessoryCache[keynum].Rotation.Y *= GE_PIOVER180;
					AccessoryCache[keynum].Rotation.Z *= GE_PIOVER180;
				}

				geActor *Actor = CCD->ActorManager()->SpawnActor(AccessoryCache[keynum].ActorName,
					AccessoryCache[keynum].Rotation, AccessoryCache[keynum].Rotation, "", "", NULL);

				CCD->ActorManager()->RemoveActor(Actor);
				geActor_Destroy(&Actor);
				Actor = NULL;
				AccessoryCache[keynum].Scale = (float)AttrFile.GetValueF(KeyName, "actorscale");
				geVec3d FillColor = {0.0f, 0.0f, 0.0f};
				geVec3d AmbientColor = {0.0f, 0.0f, 0.0f};

				Vector = AttrFile.GetValue(KeyName, "fillcolor");

				if(Vector != "")
				{
					strcpy(szName, Vector.c_str());
					FillColor = Extract(szName);
				}

				Vector = AttrFile.GetValue(KeyName, "ambientcolor");

				if(Vector != "")
				{
					strcpy(szName, Vector.c_str());
					AmbientColor = Extract(szName);
				}

				AccessoryCache[keynum].FillColor.r = FillColor.X;
				AccessoryCache[keynum].FillColor.g = FillColor.Y;
				AccessoryCache[keynum].FillColor.b = FillColor.Z;
				AccessoryCache[keynum].FillColor.a = 255.0f;
				AccessoryCache[keynum].AmbientColor.r = AmbientColor.X;
				AccessoryCache[keynum].AmbientColor.g = AmbientColor.Y;
				AccessoryCache[keynum].AmbientColor.b = AmbientColor.Z;
				AccessoryCache[keynum].AmbientColor.a = 255.0f;

				AccessoryCache[keynum].AmbientLightFromFloor = true;

				Type = AttrFile.GetValue(KeyName, "ambientlightfromfloor");

				if(Type == "false")
					AccessoryCache[keynum].AmbientLightFromFloor = false;

				AccessoryCache[keynum].EnvironmentMapping = false;

				Type = AttrFile.GetValue(KeyName, "environmentmapping");

				if(Type == "true")
				{
					AccessoryCache[keynum].EnvironmentMapping = true;
					AccessoryCache[keynum].AllMaterial = false;
					Type = AttrFile.GetValue(KeyName, "allmaterial");

					if(Type == "true")
						AccessoryCache[keynum].AllMaterial = true;

					AccessoryCache[keynum].PercentMapping = (float)AttrFile.GetValueF(KeyName, "percentmapping");
					AccessoryCache[keynum].PercentMaterial = (float)AttrFile.GetValueF(KeyName, "percentmaterial");
				}
			}

			KeyName = AttrFile.FindNextKey();
		}

		// Ok, we have Pawns somewhere.  Dig through 'em all.
		for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
			pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
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
				ScriptedObject *Object = (ScriptedObject*)pSource->Data;

// start change scripting
// add global object
				if(!EffectC_IsStringNull(pSource->szEntityName))
					CCD->AddScriptedObject(pSource->szEntityName,Object);
// end change scripting

				strcpy(Object->szName, pSource->szEntityName);

				KeyName = AttrFile.FindFirstKey();
				bool found = false;

				while(KeyName != "")
				{
					Type = AttrFile.GetValue(KeyName, "type");

					if(Type != "weapon")
					{
						if(!strcmp(KeyName.c_str(), pSource->PawnType))
						{
							Object->active = false;
							Type = AttrFile.GetValue(KeyName, "actorname");
							strcpy(Object->ActorName, Type.c_str());

							// rotation
							Vector = AttrFile.GetValue(KeyName, "actorrotation");
							if(Vector != "")
							{
								strcpy(szName, Vector.c_str());
								Object->Rotation = Extract(szName);
								Object->Rotation.X *= GE_PIOVER180;
								Object->Rotation.Y *= GE_PIOVER180;
								Object->Rotation.Z *= GE_PIOVER180;
							}
							geActor *Actor = CCD->ActorManager()->SpawnActor(Object->ActorName,
								Object->Rotation, Object->Rotation, "", "", NULL);

							CCD->ActorManager()->RemoveActor(Actor);
							geActor_Destroy(&Actor);
							Actor = NULL;

							// scale
							Object->Scale = (float)AttrFile.GetValueF(KeyName, "actorscale");

							// animspeed
							Type = AttrFile.GetValue(KeyName, "animationspeed");
							Object->AnimSpeed = 1.0f;
							if(Type != "")
								Object->AnimSpeed = (float)AttrFile.GetValueF(KeyName, "animationspeed");

							// fillcolor
							geVec3d FillColor = {0.0f, 0.0f, 0.0f};
							Vector = AttrFile.GetValue(KeyName, "fillcolor");
							if(Vector != "")
							{
								strcpy(szName, Vector.c_str());
								FillColor = Extract(szName);
							}
							Object->FillColor.r = FillColor.X;
							Object->FillColor.g = FillColor.Y;
							Object->FillColor.b = FillColor.Z;
							Object->FillColor.a = 255.0f;

							// ambientcolor
							geVec3d AmbientColor = {0.0f, 0.0f, 0.0f};
							Vector = AttrFile.GetValue(KeyName, "ambientcolor");
							if(Vector != "")
							{
								strcpy(szName, Vector.c_str());
								AmbientColor = Extract(szName);
							}
							Object->AmbientColor.r = AmbientColor.X;
							Object->AmbientColor.g = AmbientColor.Y;
							Object->AmbientColor.b = AmbientColor.Z;
							Object->AmbientColor.a = 255.0f;
// changed QD 07/21/04
							// ambient lighting
							Object->AmbientLightFromFloor = true;
							Type = AttrFile.GetValue(KeyName, "ambientlightfromfloor");
							if(Type == "false")
								Object->AmbientLightFromFloor = false;
// end change
							// environment mapping
							Object->EnvironmentMapping = false;
							Type = AttrFile.GetValue(KeyName, "environmentmapping");
							if(Type == "true")
							{
								Object->EnvironmentMapping = true;
								Object->AllMaterial = false;
								Type = AttrFile.GetValue(KeyName, "allmaterial");
								if(Type == "true")
									Object->AllMaterial = true;
								Object->PercentMapping = (float)AttrFile.GetValueF(KeyName, "percentmapping");
								Object->PercentMaterial = (float)AttrFile.GetValueF(KeyName, "percentmaterial");
							}

							// actoralpha
							Object->ActorAlpha = 255.0f;
							Type = AttrFile.GetValue(KeyName, "actoralpha");
							if(Type != "")
								Object->ActorAlpha = (float)AttrFile.GetValueF(KeyName, "actoralpha");

							// gravity
							Object->Gravity.X = Object->Gravity.Y = Object->Gravity.Z = 0.0f;
							Type = AttrFile.GetValue(KeyName, "subjecttogravity");
							if(Type == "true")
								Object->Gravity = CCD->Player()->GetGravity();

							// bounding box
							Object->BoxAnim[0] = '\0';
							Type = AttrFile.GetValue(KeyName, "boundingboxanimation");
							if(Type != "")
								strcpy(Object->BoxAnim, Type.c_str());

							// rootbone
							Object->RootBone[0] = '\0';
							Type = AttrFile.GetValue(KeyName, "rootbone");
							if(Type != "")
								strcpy(Object->RootBone, Type.c_str());

							// shadowmap
							Object->ShadowSize = (float)AttrFile.GetValueF(KeyName, "shadowsize");
// changed QD 06/26/04
							Object->ShadowAlpha = (geFloat)AttrFile.GetValueF(KeyName, "shadowalpha");
							Type = AttrFile.GetValue(KeyName, "shadowbitmap");
							if(Type != "")
								strcpy(Object->ShadowBitmap, Type.c_str());
							Type = AttrFile.GetValue(KeyName, "shadowalphamap");
							if(Type != "")
								strcpy(Object->ShadowAlphamap, Type.c_str());
// end change
// begin change gekido
							// projected shadows enabled per pawn type
							// yes there is obscene amounts of output to the debug log, might want to trim this down
							Type = AttrFile.GetValue(KeyName, "projectedshadows");
							if(Type != "")
							{
								/* char szBug[256];
								sprintf(szBug, "Setting Projected Shadows for %s", Object->szName);
								CCD->ReportError(szBug, false);*/

								if(!strcmp(Type.c_str(), "true"))
								{
									// char szBug[256];
									Object->ProjectedShadows = true;
									// sprintf(szBug, "   Projected Shadows : %s", Type);
									// CCD->ReportError(szBug, false);
								}
								else
								{
									// char szBug[256];
									Object->ProjectedShadows = false;
									// sprintf(szBug, "   Projected Shadows : %s", Type);
									// CCD->ReportError(szBug, false);
								}
							}
// end change
// changed QD Shadows
							// stencil shadows enabled per pawn type
							Type = AttrFile.GetValue(KeyName, "stencilshadows");
							if(Type != "")
							{
								//char szBug[256];
								//sprintf(szBug, "Setting Stencil Shadows for %s", Object->szName);
								//CCD->ReportError(szBug, false);

								if(!strcmp(Type.c_str(), "true"))
									Object->StencilShadows = GE_TRUE;
								else
									Object->ProjectedShadows = GE_FALSE;
							}
// end change
							// icon
							Object->Icon = NULL;
							Type = AttrFile.GetValue(KeyName, "icon");
							if(Type != "")
							{
								Type = "conversation\\"+Type;
								strcpy(szName, Type.c_str());
								Object->Icon = CreateFromFileName(szName);
								// changed QD 07/15/06
								if(Object->Icon == (geBitmap *)NULL)
								{
									char szError[256];
									sprintf(szError, "[WARNING] CPawn: Failed to create icon image %s", szName);
									CCD->ReportError(szError, false);
								}
								else
								{
									geEngine_AddBitmap(CCD->Engine()->Engine(), Object->Icon);
									geBitmap_SetColorKey(Object->Icon, GE_TRUE, 255, GE_FALSE);
								}
								// end change
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
							Object->YRotation = GE_PIOVER180*(pSource->Angle.Y-90.0f);
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

/* ------------------------------------------------------------------------------------ */
//	Destructor
/* ------------------------------------------------------------------------------------ */
CPawn::~CPawn()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

// changed QD 12/15/05
	if(Background)
	{
		geEngine_RemoveBitmap(CCD->Engine()->Engine(), Background);
		geBitmap_Destroy(&Background);
	}

	if(rBackground)
	{
		geEngine_RemoveBitmap(CCD->Engine()->Engine(), rBackground);
		geBitmap_Destroy(&rBackground);
	}

	if(ReplyMenuBar)
	{
		geEngine_RemoveBitmap(CCD->Engine()->Engine(), ReplyMenuBar);
		geBitmap_Destroy(&ReplyMenuBar);
	}

	for(int i=0; i<9; i++)
	{
		if(GifFile[i])
		{
			delete GifFile[i];
			GifFile[i] = NULL;

		}
	}
// end change

	unsigned int keynum = BitmapCache.size();

	if(keynum > 0)
	{
		for(unsigned int i=0; i<keynum; i++)
		{
			// changed QD 07/15/06
			if(BitmapCache[i].Bitmap)
			{
				// changed QD 12/15/05
				geEngine_RemoveBitmap(CCD->Engine()->Engine(), BitmapCache[i].Bitmap);
				// end change
				geBitmap_Destroy(&(BitmapCache[i].Bitmap));
			}
			// end change
		}
	}

	EventStack *pool, *temp;
	pool = Events;

	while(pool != NULL)
	{
		temp = pool->next;
		geRam_Free(pool);
		pool = temp;
	}

	// Ok, check to see if there are Pawns in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "Pawn");

	if(!pSet)
		return;

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Pawn *pSource = (Pawn*)geEntity_GetUserData(pEntity);

		if(pSource->Data)
		{
			ScriptedObject *Object = (ScriptedObject *)pSource->Data;
			CCD->RemoveScriptedObject(pSource->szEntityName);//change scripting - remove object
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

/* ------------------------------------------------------------------------------------ */
//	Tick
/* ------------------------------------------------------------------------------------ */
void CPawn::Tick(geFloat dwTicks)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	skRValueArray args;// change simkin
	skRValue ret;

	ConsoleBlock = 0;

	// Ok, check to see if there are Pawns in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "Pawn");

	if(!pSet)
		return;

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Pawn *pSource = (Pawn*)geEntity_GetUserData(pEntity);

		if(pSource->Data)
		{
			ScriptedObject *Object = (ScriptedObject*)pSource->Data;

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

			Object->collision = false;

			if(Object->console && ConsoleBlock < 4)
			{
				int x, y;

				x = 5;
				y = (115*ConsoleBlock)+5;

				geEngine_Printf(CCD->Engine()->Engine(), x, y,"%s",Object->ConsoleHeader);
				y += 10;

				if(!EffectC_IsStringNull(Object->ConsoleError))
					geEngine_Printf(CCD->Engine()->Engine(), x, y,"%s %s",Object->Indicate, Object->ConsoleError);

				for(int i=0; i<DEBUGLINES; i++)
				{
					y += 10;

					if(!EffectC_IsStringNull(Object->ConsoleDebug[i]))
						geEngine_Printf(CCD->Engine()->Engine(), x, y,"%s",Object->ConsoleDebug[i]);
				}

				ConsoleBlock += 1;
			}
		}
	}
}

/* ------------------------------------------------------------------------------------ */
//	HandleCollision
/* ------------------------------------------------------------------------------------ */
int CPawn::HandleCollision(const geActor *pActor, const geActor *theActor, bool Gravity)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	if(theActor != CCD->Player()->GetActor())
		return RGF_SUCCESS;

	// Ok, check to see if there are Pawns in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "Pawn");

	if(pSet)
	{
		for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
			pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
		{
			Pawn *pSource = (Pawn*)geEntity_GetUserData(pEntity);

			if(pSource->Data)
			{
				ScriptedObject *Object = (ScriptedObject *)pSource->Data;

				if(!Object->alive)
					continue;

				if(Object->active)
				{
					if(!Object->Actor)
						continue;

					if(Object->Actor != pActor)
						continue;

					Object->collision = true;

					if(Object->pushable && !Gravity)
					{
						geVec3d In, SavedPosition, NewPosition;
						int direct = CCD->Player()->LastDirection();

						if(direct == RGF_K_FORWARD || direct == RGF_K_BACKWARD)
							CCD->Player()->GetIn(&In);
						else
							CCD->Player()->GetLeft(&In);

						CCD->ActorManager()->GetPosition(Object->Actor, &SavedPosition);
						geFloat speed = CCD->Player()->LastMovementSpeed();

						if(direct == RGF_K_BACKWARD || direct == RGF_K_RIGHT)
							speed = -speed;

						geVec3d_AddScaled(&SavedPosition, &In, speed, &NewPosition);
						bool result = CCD->ActorManager()->ValidateMove(SavedPosition, NewPosition, Object->Actor, false);

						if(result)
							return RGF_RECHECK;
					}

					return RGF_SUCCESS;
				}
			}
		}
	}

	return RGF_FAILURE;
}

/* ------------------------------------------------------------------------------------ */
//	AnimateWeapon
/* ------------------------------------------------------------------------------------ */
void CPawn::AnimateWeapon()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	// Ok, check to see if there are Pawns in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "Pawn");

	if(!pSet)
		return;

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Pawn *pSource = (Pawn*)geEntity_GetUserData(pEntity);

		if(pSource->Data)
		{
			ScriptedObject *Object = (ScriptedObject *)pSource->Data;

			if(!Object->alive)
				continue;

			if(Object->active)
			{
				if(Object->WeaponActor && Object->Actor)
				{
					geXForm3d XForm;
					geVec3d theRotation;
					geVec3d thePosition;
					geMotion *ActorMotion;
					geMotion *ActorBMotion;

					CCD->ActorManager()->GetRotate(Object->Actor, &theRotation);
					CCD->ActorManager()->GetPosition(Object->Actor, &thePosition);

					// changed QD 12/15/05
					//geXForm3d_SetIdentity(&XForm);
					//geXForm3d_RotateZ(&XForm, Object->WRotation.Z + theRotation.Z);
					geXForm3d_SetZRotation(&XForm, Object->WRotation.Z + theRotation.Z);
					// end change
					geXForm3d_RotateX(&XForm, Object->WRotation.X + theRotation.X);
					geXForm3d_RotateY(&XForm, Object->WRotation.Y + theRotation.Y);
					geXForm3d_Translate(&XForm, thePosition.X, thePosition.Y, thePosition.Z);

					ActorMotion = CCD->ActorManager()->GetpMotion(Object->Actor);
					ActorBMotion = CCD->ActorManager()->GetpBMotion(Object->Actor);

					if(ActorMotion)
					{
						if(CCD->ActorManager()->GetTransitionFlag(Object->Actor))
						{
							geActor_SetPose(Object->WeaponActor, ActorMotion, 0.0f, &XForm);

							if(ActorBMotion)
							{
								float BM = (CCD->ActorManager()->GetBlendAmount(Object->Actor)
									- CCD->ActorManager()->GetAnimationTime(Object->Actor))
									/ CCD->ActorManager()->GetBlendAmount(Object->Actor);

								if(BM < 0.0f)
									BM = 0.0f;

								geActor_BlendPose(Object->WeaponActor, ActorBMotion,
									CCD->ActorManager()->GetStartTime(Object->Actor), &XForm, BM);
							}
						}
						else
						{
							geActor_SetPose(Object->WeaponActor, ActorMotion, CCD->ActorManager()->GetAnimationTime(Object->Actor), &XForm);

							if(ActorBMotion)
							{
								geActor_BlendPose(Object->WeaponActor, ActorBMotion,
									CCD->ActorManager()->GetAnimationTime(Object->Actor), &XForm,
									CCD->ActorManager()->GetBlendAmount(Object->Actor));
							}
						}
					}
					else
					{
						geActor_ClearPose(Object->WeaponActor, &XForm);
					}

					geFloat fAlpha;
					CCD->ActorManager()->GetAlpha(Object->Actor, &fAlpha);
					geActor_SetAlpha(Object->WeaponActor, fAlpha);
				}
			}
		}
	}
}

/* ------------------------------------------------------------------------------------ */
//	Spawn
/* ------------------------------------------------------------------------------------ */
void CPawn::Spawn(void *Data)
{
	ScriptedObject *Object = (ScriptedObject *)Data;
	geVec3d theRotation;

	Object->Actor = CCD->ActorManager()->SpawnActor(Object->ActorName,
		Object->Location, Object->Rotation, Object->BoxAnim, Object->BoxAnim, NULL);

// changed QD 07/21/04
//	CCD->ActorManager()->SetActorDynamicLighting(Object->Actor, Object->FillColor, Object->AmbientColor);
	CCD->ActorManager()->SetActorDynamicLighting(Object->Actor, Object->FillColor,
												 Object->AmbientColor, Object->AmbientLightFromFloor);
// end change

	CCD->ActorManager()->SetShadow(Object->Actor, Object->ShadowSize);

// changed QD 06/26/04
	if(Object->ShadowAlpha > 0.0f)
		CCD->ActorManager()->SetShadowAlpha(Object->Actor, Object->ShadowAlpha);

	if(!EffectC_IsStringNull(Object->ShadowBitmap))
		CCD->ActorManager()->SetShadowBitmap(Object->Actor, TPool_Bitmap(Object->ShadowBitmap, Object->ShadowAlphamap, NULL, NULL));
// end change

// begin change gekido
	// projected shadows per pawn type
	CCD->ActorManager()->SetProjectedShadows(Object->Actor, Object->ProjectedShadows);
// end change gekido

// changed QD Shadows
	CCD->ActorManager()->SetStencilShadows(Object->Actor, Object->StencilShadows);
// end change

	CCD->ActorManager()->SetHideRadar(Object->Actor, Object->HideFromRadar);
	CCD->ActorManager()->SetScale(Object->Actor, Object->Scale);
	CCD->ActorManager()->SetType(Object->Actor, ENTITY_NPC);
	CCD->ActorManager()->SetAutoStepUp(Object->Actor, true);
	CCD->ActorManager()->SetBoxChange(Object->Actor, false);
	CCD->ActorManager()->SetColDetLevel(Object->Actor, RGF_COLLISIONLEVEL_2);
	CCD->ActorManager()->SetAnimationSpeed(Object->Actor, Object->AnimSpeed);
	CCD->ActorManager()->SetTilt(Object->Actor, true);

	if(!EffectC_IsStringNull(Object->RootBone))
		CCD->ActorManager()->SetRoot(Object->Actor, Object->RootBone);

	if(Object->EnvironmentMapping)
		SetEnvironmentMapping(Object->Actor, true, Object->AllMaterial, Object->PercentMapping, Object->PercentMaterial);

	if(!EffectC_IsStringNull(Object->ChangeMaterial))
		CCD->ActorManager()->ChangeMaterial(Object->Actor, Object->ChangeMaterial);

	if(!stricmp(Object->BoxAnim, "nocollide"))
	{
		CCD->ActorManager()->SetNoCollide(Object->Actor);
		CCD->ActorManager()->SetActorFlags(Object->Actor, 0);
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
	CCD->ActorManager()->SetEntityName(Object->Actor, Object->szName);
}

/* ------------------------------------------------------------------------------------ */
//	PreLoad
//
//	Preload scripts (sound files)
/* ------------------------------------------------------------------------------------ */
void CPawn::PreLoad(const char *filename)
{
// changed Nout 12/15/05
	//FILE *fdInput = NULL;
	geVFile *fdInput = NULL;
// end change
	char szInputString[1024] = {""};
	std::string str;
	int i, j;
	char file[256];

// changed Nout 12/15/05
/*	fdInput = fopen(filename, "rt");

	if(!fdInput)
		return;

	while(fgets(szInputString, 1024, fdInput) != NULL)
*/
	if(!CCD->OpenRFFile(&fdInput, kScriptFile, filename, GE_VFILE_OPEN_READONLY))
	{
		char szError[256];
		sprintf(szError, "[WARNING] File %s - Line %d: Failed to open file: '%s'",
			__FILE__, __LINE__, filename);
		CCD->ReportError(szError, false);
		return;
	}

	while(geVFile_GetS(fdInput, szInputString, 1024) == GE_TRUE)
// end change
	{
		if(szInputString[0] == ';' || strlen(szInputString) <= 1)
			continue;

		str = szInputString;
		TrimRight(str);
		MakeLower(str);
// changed Nout 12/15/05 - just in case someone is typing the path this way...
		Replace(str, "\\\\", "\\");
// end change
		i = str.find(".wav");

		if(i >= 0 && i < (int)str.length())
		{
			j = i-1;

			while(!(str[j] == '"' || str[j] == '[') && j >= 0)
			{
				j--;
			}

			if(j >= 0)
			{
				strcpy(file, str.substr(j+1, i-j+3).c_str());
				SPool_Sound(file);
			}
		}
	}

// changed Nout 12/15/05
	//fclose(fdInput);
	geVFile_Close(fdInput);
// end change
}

// changed Nout 12/15/05
/* ------------------------------------------------------------------------------------ */
//	ShowText
/* ------------------------------------------------------------------------------------ */
void CPawn::ShowText(int Nr)
{
	if(Nr < 0 || Nr >= MAXTEXT)
		return;

	if(TextMessage[Nr].ShowText)
	{
		geVec3d Pos, ScreenPos;
		geActor *Actor;

		if(strlen(TextMessage[Nr].EntityName) > 0) //If no entity defined, then use absolute screen position
		{
			if(strstr(TextMessage[Nr].EntityName, "Player"))
			{
				Pos = CCD->Player()->Position();
				Actor = CCD->Player()->GetActor();
			}
			else
			{
				Actor = CCD->ActorManager()->GetByEntityName(TextMessage[Nr].EntityName);
				CCD->ActorManager()->GetPosition(Actor, &Pos);
			}

			geCamera_TransformAndProject(CCD->CameraManager()->Camera(), &Pos, &ScreenPos);
		}
		else
		{
			ScreenPos.X = 0.0f;
			ScreenPos.Y = 0.0f;
		}

		ScreenPos.Z = 0.0f;
		ScreenPos.X += TextMessage[Nr].ScreenOffsetX;
		ScreenPos.Y += TextMessage[Nr].ScreenOffsetY;

		int TextWidth = 0;
		char Text[256];
		strcpy(Text, TextMessage[Nr].TextString.c_str());
	    int LineNr = 0;
// changed QD 12/15/05 - use <CR> as indicator for a new line as it's done in conversation texts
		/*char *result = NULL;
		result = strtok(Text , "#");

		while( result != NULL )
		{
			TextWidth = CCD->MenuManager()->FontWidth(TextMessage[Nr].FontNr, result);

			if(strstr(TextMessage[Nr].Alignment, "Left"))
			{
				CCD->MenuManager()->WorldFontRect(result,
					TextMessage[Nr].FontNr,
					int(ScreenPos.X-TextWidth),
					int(ScreenPos.Y+(LineNr*(2+CCD->MenuManager()->FontHeight(TextMessage[Nr].FontNr)))),
					float(TextMessage[Nr].Alpha));
			}
			else if(strstr(TextMessage[Nr].Alignment, "Center"))
			{
				CCD->MenuManager()->WorldFontRect(result,
					TextMessage[Nr].FontNr,
					int(ScreenPos.X-(TextWidth/2)),
					int(ScreenPos.Y+(LineNr*(2+CCD->MenuManager()->FontHeight(TextMessage[Nr].FontNr)))),
					float(TextMessage[Nr].Alpha));
			}
			else
			{
				CCD->MenuManager()->WorldFontRect(result,
					TextMessage[Nr].FontNr,
					int(ScreenPos.X),
					int(ScreenPos.Y+(LineNr*(2+CCD->MenuManager()->FontHeight(TextMessage[Nr].FontNr)))),
					float(TextMessage[Nr].Alpha));
			}

			result = strtok(NULL, "#");
			LineNr += 1;
		}
		*/
		char *CR = NULL;
		char *Line = Text;

		//if(strstr(TextMessage[Nr].Alignment, "left"))
		switch(TextMessage[Nr].Alignment)
		{
		case 'L':
		case 'l':
			{
				while(*Line)
				{
					CR = strstr(Line, "<CR>");
					if(CR)
						*CR = 0;

					TextWidth = CCD->MenuManager()->FontWidth(TextMessage[Nr].FontNr, Line);

					CCD->MenuManager()->WorldFontRect(Line,
						TextMessage[Nr].FontNr,
						int(ScreenPos.X-TextWidth),
						int(ScreenPos.Y+(LineNr*(2+CCD->MenuManager()->FontHeight(TextMessage[Nr].FontNr)))),
						float(TextMessage[Nr].Alpha));

					if(CR)
					{
						Line = CR + 4;
						LineNr++;
					}
					else
						break;
				}

				break;
			}
		case 'C':
		case 'c':	//	else if(strstr(TextMessage[Nr].Alignment, "center"))
			{
				while(*Line)
				{
					CR = strstr(Line, "<CR>");
					if(CR)
						*CR = 0;

					TextWidth = CCD->MenuManager()->FontWidth(TextMessage[Nr].FontNr, Line);

					CCD->MenuManager()->WorldFontRect(Line,
						TextMessage[Nr].FontNr,
						int(ScreenPos.X-(TextWidth/2)),
						int(ScreenPos.Y+(LineNr*(2+CCD->MenuManager()->FontHeight(TextMessage[Nr].FontNr)))),
						float(TextMessage[Nr].Alpha));

					if(CR)
					{
						Line = CR + 4;
						LineNr++;
					}
					else
						break;
				}

				break;
			}
		default:	//	else
			{
				while(*Line)
				{
					CR = strstr(Line, "<CR>");
					if(CR)
						*CR = 0;

					CCD->MenuManager()->WorldFontRect(Line,
						TextMessage[Nr].FontNr,
						int(ScreenPos.X),
						int(ScreenPos.Y+(LineNr*(2+CCD->MenuManager()->FontHeight(TextMessage[Nr].FontNr)))),
						float(TextMessage[Nr].Alpha));

					if(CR)
					{
						Line = CR + 4;
						LineNr++;
					}
					else
						break;
				}

				break;
			}
		}
// end change
	}
}

/* ------------------------------------------------------------------------------------ */
//	FillScreenArea
/* ------------------------------------------------------------------------------------ */
void CPawn::FillScreenArea(int Nr)
{
	if(Nr < 0 || Nr >= MAXFILLAREA)
		return;

	if(FillScrArea[Nr].DoFillScreenArea)
	{
		geEngine_FillRect(CCD->Engine()->Engine(), &FillScrArea[Nr].FillScreenAreaRect,
							&FillScrArea[Nr].FillScreenAreaColor);

		if(!(FillScrArea[Nr].FillScreenAreaKeep))
			FillScrArea[Nr].DoFillScreenArea = false;
	}
}
// end change

/* ------------------------------------------------------------------------------------ */
//	GetCache
/* ------------------------------------------------------------------------------------ */
geBitmap *CPawn::GetCache(const char *Name)
{
	int keynum = BitmapCache.size();

	if(keynum > 0)
	{
		for(int i=0; i<keynum; i++)
		{
			if(!strcmp(BitmapCache[i].Name.c_str(), Name))
			{
				return BitmapCache[i].Bitmap;
			}
		}
	}

	return NULL;
}

//	******************** CRGF Overrides ********************

/* ------------------------------------------------------------------------------------ */
//	LocateEntity
//
//	Given a name, locate the desired item in the currently loaded level
//	..and return it's user data.
/* ------------------------------------------------------------------------------------ */
int CPawn::LocateEntity(const char *szName, void **pEntityData)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	// Ok, check to see if there are Pawns in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "Pawn");

	if(!pSet)
		return RGF_NOT_FOUND;

	// Ok, we have Pawns.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Pawn *pTheEntity = (Pawn*)geEntity_GetUserData(pEntity);

		if(!strcmp(pTheEntity->szEntityName, szName))
		{
			*pEntityData = (void*)pTheEntity;
			return RGF_SUCCESS;
		}
	}

	return RGF_NOT_FOUND;								// Sorry, no such entity here
}

/* ------------------------------------------------------------------------------------ */
//	ParmCheck
/* ------------------------------------------------------------------------------------ */
void CPawn::ParmCheck(int Entries, int Desired, const char *Order, const char *szName, const skString &methodname)
{
	if(Entries >= Desired)
		return;

	char param0[128];
	strcpy(param0, methodname);
	char szError[256];
	sprintf(szError, "[ERROR] Incorrect # of parameters in command '%s' in Script '%s'  Order '%s'",
			param0, szName, Order);
	CCD->ReportError(szError, false);
	CCD->ShutdownLevel();
	delete CCD;
	CCD = NULL;
	MessageBox(NULL, szError, "Pawn", MB_OK);
	exit(-333);
}


/* ------------------------------------------------------------------------------------ */
//	SaveTo
//
//	Save the current state of every Pawn in the current world off to an open file.
/* ------------------------------------------------------------------------------------ */
int CPawn::SaveTo(FILE *SaveFD, bool type)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	// Ok, check to see if there are Pawns in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "Pawn");

	if(!pSet)
		return RGF_NOT_FOUND;

	//	Ok, we have Pawns.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Pawn *pSource = (Pawn*)geEntity_GetUserData(pEntity);

		if(pSource->Data)
		{
			ScriptedObject *Object = (ScriptedObject*)pSource->Data;
		}
	}

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	RestoreFrom
//
//	Restore the state of every Pawn in the current world from an open file.
/* ------------------------------------------------------------------------------------ */
int CPawn::RestoreFrom(FILE *RestoreFD, bool type)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	// Ok, check to see if there are Pawns in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "Pawn");

	if(!pSet)
		return RGF_NOT_FOUND;

	// Ok, we have Pawns.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Pawn *pSource = (Pawn*)geEntity_GetUserData(pEntity);

		if(pSource->Data)
		{
			ScriptedObject *Object = (ScriptedObject*)pSource->Data;
		}
	}

	return RGF_SUCCESS;
}


/* ----------------------------------- END OF FILE ------------------------------------ */
