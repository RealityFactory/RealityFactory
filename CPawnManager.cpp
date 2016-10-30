/************************************************************************************//**
 * @file CPawnManager.cpp
 * @brief CPawnManager class declaration
 * @author Ralph Deane
 *//*
 * Copyright (c) Ralph Deane; All rights reserved.
 ****************************************************************************************/

#include "RabidFramework.h"
#include <Ram.h>
#include "CFileManager.h"
#include "IniFile.h"
#include "CLanguageManager.h"
#include "CPawn.h"
#include "CPawnManager.h"
#include "CScriptManager.h"
#include "CConversation.h"
#include "CLevel.h"
#include "CScriptPoint.h"

extern geSound_Def *SPool_Sound(const char *SName);
extern geBitmap *TPool_Bitmap(const char *DefaultBmp, const char *DefaultAlpha,
							  const char *BName, const char *AName);

#include "Simkin\\skScriptedExecutable.h"
#include "Simkin\\skRValue.h"
#include "Simkin\\skRValueArray.h"
#include "Simkin\\skRuntimeException.h"
#include "Simkin\\skParseException.h"
#include "Simkin\\skBoundsException.h"
#include "Simkin\\skTreeNode.h"


/* ------------------------------------------------------------------------------------ */
// PawnManager Class
/* ------------------------------------------------------------------------------------ */
CPawnManager::CPawnManager()
{
	int i;

	for(i=0; i<MAXFILLAREA; ++i)
	{
		FillScrArea[i].DoFillScreenArea = false;
		FillScrArea[i].FillScreenAreaKeep = false;
	}

	Events		= NULL;

	for(i=0; i<MAXFLAGS; ++i)
		PawnFlag[i] = false;

	// Ok, check to see if there are Pawns in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "Pawn");

	if(pSet)
	{
		WeaponCache.resize(0);
		AccessoryCache.resize(0);

		CIniFile IniFile("pawn.ini");

		if(!IniFile.ReadFile())
		{
			CCD->Log()->Error("Failed to open Pawn.ini file");
			return;
		}

		std::string KeyName = IniFile.FindFirstKey();
		std::string type, vector, value;

		for(i=0; i<MAXTEXT; ++i)
			TextMessage[i].ShowText = false;

		KeyName = IniFile.FindFirstKey();
		while(!KeyName.empty())
		{
			type = IniFile.GetValue(KeyName, "type");

			if(type == "weapon")
			{
				WeaponCache.resize(WeaponCache.size()+1);
				int keynum = WeaponCache.size()-1;
				WeaponCache[keynum].Name = KeyName;

				WeaponCache[keynum].ActorName = IniFile.GetValue(KeyName, "actorname");

				vector = IniFile.GetValue(KeyName, "actorrotation");

				if(!vector.empty())
				{
					WeaponCache[keynum].Rotation = ToVec3d(vector);
					WeaponCache[keynum].Rotation.X *= GE_PIOVER180;
					WeaponCache[keynum].Rotation.Y *= GE_PIOVER180;
					WeaponCache[keynum].Rotation.Z *= GE_PIOVER180;
				}

				geActor* Actor = CCD->ActorManager()->SpawnActor(WeaponCache[keynum].ActorName,
					WeaponCache[keynum].Rotation, WeaponCache[keynum].Rotation, "", "", NULL);

				CCD->ActorManager()->RemoveActor(Actor);
				geActor_Destroy(&Actor);
				Actor = NULL;
				WeaponCache[keynum].Scale = static_cast<float>(IniFile.GetValueF(KeyName, "actorscale"));
				geVec3d FillColor = {0.0f, 0.0f, 0.0f};
				geVec3d AmbientColor = {0.0f, 0.0f, 0.0f};

				vector = IniFile.GetValue(KeyName, "fillcolor");

				if(!vector.empty())
				{
					FillColor = ToVec3d(vector);
				}

				vector = IniFile.GetValue(KeyName, "ambientcolor");

				if(!vector.empty())
				{
					AmbientColor = ToVec3d(vector);
				}

				WeaponCache[keynum].FillColor.r = FillColor.X;
				WeaponCache[keynum].FillColor.g = FillColor.Y;
				WeaponCache[keynum].FillColor.b = FillColor.Z;
				WeaponCache[keynum].FillColor.a = 255.0f;
				WeaponCache[keynum].AmbientColor.r = AmbientColor.X;
				WeaponCache[keynum].AmbientColor.g = AmbientColor.Y;
				WeaponCache[keynum].AmbientColor.b = AmbientColor.Z;
				WeaponCache[keynum].AmbientColor.a = 255.0f;

				WeaponCache[keynum].AmbientLightFromFloor = true;
				value = IniFile.GetValue(KeyName, "ambientlightfromfloor");
				if(value == "false")
					WeaponCache[keynum].AmbientLightFromFloor = false;

				WeaponCache[keynum].EnvironmentMapping = false;
				value = IniFile.GetValue(KeyName, "environmentmapping");
				if(value == "true")
				{
					WeaponCache[keynum].EnvironmentMapping = true;

					WeaponCache[keynum].AllMaterial = false;
					value = IniFile.GetValue(KeyName, "allmaterial");
					if(value == "true")
						WeaponCache[keynum].AllMaterial = true;

					WeaponCache[keynum].PercentMapping = static_cast<float>(IniFile.GetValueF(KeyName, "percentmapping"));
					WeaponCache[keynum].PercentMaterial = static_cast<float>(IniFile.GetValueF(KeyName, "percentmaterial"));
				}
			}
			else if(type == "accessory")
			{
				AccessoryCache.resize(AccessoryCache.size()+1);
				int keynum = AccessoryCache.size()-1;
				AccessoryCache[keynum].Name = KeyName;

				AccessoryCache[keynum].ActorName = IniFile.GetValue(KeyName, "actorname");

				vector = IniFile.GetValue(KeyName, "actorrotation");

				if(!vector.empty())
				{
					AccessoryCache[keynum].Rotation = ToVec3d(vector);
					AccessoryCache[keynum].Rotation.X *= GE_PIOVER180;
					AccessoryCache[keynum].Rotation.Y *= GE_PIOVER180;
					AccessoryCache[keynum].Rotation.Z *= GE_PIOVER180;
				}

				geActor* Actor = CCD->ActorManager()->SpawnActor(AccessoryCache[keynum].ActorName,
					AccessoryCache[keynum].Rotation, AccessoryCache[keynum].Rotation, "", "", NULL);

				CCD->ActorManager()->RemoveActor(Actor);
				geActor_Destroy(&Actor);
				Actor = NULL;
				AccessoryCache[keynum].Scale = static_cast<float>(IniFile.GetValueF(KeyName, "actorscale"));
				geVec3d FillColor = {0.0f, 0.0f, 0.0f};
				geVec3d AmbientColor = {0.0f, 0.0f, 0.0f};

				vector = IniFile.GetValue(KeyName, "fillcolor");

				if(!vector.empty())
				{
					FillColor = ToVec3d(vector);
				}

				vector = IniFile.GetValue(KeyName, "ambientcolor");

				if(!vector.empty())
				{
					AmbientColor = ToVec3d(vector);
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
				value = IniFile.GetValue(KeyName, "ambientlightfromfloor");
				if(value == "false")
					AccessoryCache[keynum].AmbientLightFromFloor = false;

				AccessoryCache[keynum].EnvironmentMapping = false;
				value = IniFile.GetValue(KeyName, "environmentmapping");
				if(value == "true")
				{
					AccessoryCache[keynum].EnvironmentMapping = true;

					AccessoryCache[keynum].AllMaterial = false;
					value = IniFile.GetValue(KeyName, "allmaterial");
					if(value == "true")
						AccessoryCache[keynum].AllMaterial = true;

					AccessoryCache[keynum].PercentMapping = static_cast<float>(IniFile.GetValueF(KeyName, "percentmapping"));
					AccessoryCache[keynum].PercentMaterial = static_cast<float>(IniFile.GetValueF(KeyName, "percentmaterial"));
				}
			}

			KeyName = IniFile.FindNextKey();
		}

		// Ok, we have Pawns somewhere.  Dig through 'em all.
		geEntity *pEntity;
		for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
			pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
		{
			Pawn *pSource = static_cast<Pawn*>(geEntity_GetUserData(pEntity));

			if(EffectC_IsStringNull(pSource->szEntityName))
			{
				char szName[128];
				geEntity_GetName(pEntity, szName, 128);
				pSource->szEntityName = szName;
			}

			// Ok, put this entity into the Global Entity Registry
			CCD->EntityRegistry()->AddEntity(pSource->szEntityName, "Pawn");
			pSource->Data = NULL;

			if(!EffectC_IsStringNull(pSource->ScriptName))
			{
				std::string script("scripts\\");
				script += pSource->ScriptName;

				try
				{
					pSource->Data = new ScriptedObject(script);
				}
				catch(skParseException &e)
				{
					CCD->Log()->Error("Parse Script Error for %s %s", pSource->szEntityName, e.toString());
					continue;
				}
				catch(skBoundsException &e)
				{
					CCD->Log()->Error("Bounds Script Error for %s %s", pSource->szEntityName, e.toString());
					continue;
				}
				catch(skRuntimeException &e)
				{
					CCD->Log()->Error("Runtime Script Error for %s %s", pSource->szEntityName, e.toString());
					continue;
				}
				catch(skTreeNodeReaderException &e)
				{
					CCD->Log()->Error("Reader Script Error for %s %s", pSource->szEntityName, e.toString());
					continue;
				}
				catch(...)
				{
					CCD->Log()->Error("Script Error for %s", pSource->szEntityName);
					continue;
				}

				PreLoad(script);
				ScriptedObject *Object = static_cast<ScriptedObject*>(pSource->Data);

				// add global object
				if(!EffectC_IsStringNull(pSource->szEntityName))
					ScriptManager::AddGlobalVariable(pSource->szEntityName, Object);

				Object->szName = pSource->szEntityName;

				KeyName = IniFile.FindFirstKey();
				bool found = false;

				while(KeyName != "")
				{
					type = IniFile.GetValue(KeyName, "type");

					if(type != "weapon" && type != "accessory")
					{
						if(KeyName == pSource->PawnType)
						{
							Object->m_Active = false;
							Object->m_ActorName = IniFile.GetValue(KeyName, "actorname");

							// rotation
							vector = IniFile.GetValue(KeyName, "actorrotation");
							if(!vector.empty())
							{
								Object->m_Rotation = ToVec3d(vector);
								Object->m_Rotation.X *= GE_PIOVER180;
								Object->m_Rotation.Y *= GE_PIOVER180;
								Object->m_Rotation.Z *= GE_PIOVER180;
							}
							geActor* Actor = CCD->ActorManager()->SpawnActor(Object->m_ActorName,
								Object->m_Rotation, Object->m_Rotation, "", "", NULL);

							CCD->ActorManager()->RemoveActor(Actor);
							geActor_Destroy(&Actor);
							Actor = NULL;

							// scale
							Object->m_Scale = static_cast<float>(IniFile.GetValueF(KeyName, "actorscale"));

							// animspeed
							Object->m_AnimSpeed = 1.0f;
							value = IniFile.GetValue(KeyName, "animationspeed");
							if(!value.empty())
								Object->m_AnimSpeed = static_cast<float>(IniFile.GetValueF(KeyName, "animationspeed"));

							// fillcolor
							geVec3d FillColor = {0.0f, 0.0f, 0.0f};
							vector = IniFile.GetValue(KeyName, "fillcolor");
							if(!vector.empty())
							{
								FillColor = ToVec3d(vector);
							}
							Object->m_FillColor.r = FillColor.X;
							Object->m_FillColor.g = FillColor.Y;
							Object->m_FillColor.b = FillColor.Z;
							Object->m_FillColor.a = 255.0f;

							// ambientcolor
							geVec3d AmbientColor = {0.0f, 0.0f, 0.0f};
							vector = IniFile.GetValue(KeyName, "ambientcolor");
							if(!vector.empty())
							{
								AmbientColor = ToVec3d(vector);
							}
							Object->m_AmbientColor.r = AmbientColor.X;
							Object->m_AmbientColor.g = AmbientColor.Y;
							Object->m_AmbientColor.b = AmbientColor.Z;
							Object->m_AmbientColor.a = 255.0f;

							// ambient lighting
							Object->m_AmbientLightFromFloor = true;
							value = IniFile.GetValue(KeyName, "ambientlightfromfloor");
							if(value == "false")
								Object->m_AmbientLightFromFloor = false;

							// environment mapping
							Object->m_EnvironmentMapping = false;
							value = IniFile.GetValue(KeyName, "environmentmapping");
							if(value == "true")
							{
								Object->m_EnvironmentMapping = true;

								Object->m_AllMaterial = false;
								value = IniFile.GetValue(KeyName, "allmaterial");
								if(value == "true")
									Object->m_AllMaterial = true;

								Object->m_PercentMapping = static_cast<float>(IniFile.GetValueF(KeyName, "percentmapping"));
								Object->m_PercentMaterial = static_cast<float>(IniFile.GetValueF(KeyName, "percentmaterial"));
							}

							// actoralpha
							Object->m_ActorAlpha = 255.0f;
							value = IniFile.GetValue(KeyName, "actoralpha");
							if(!value.empty())
								Object->m_ActorAlpha = static_cast<float>(IniFile.GetValueF(KeyName, "actoralpha"));

							// gravity
							Object->m_Gravity.X = Object->m_Gravity.Y = Object->m_Gravity.Z = 0.0f;
							value = IniFile.GetValue(KeyName, "subjecttogravity");
							if(value == "true")
								Object->m_Gravity = CCD->Level()->GetGravity();

							// bounding box
							Object->m_BoxAnim = IniFile.GetValue(KeyName, "boundingboxanimation");

							// rootbone
							Object->m_RootBone  = IniFile.GetValue(KeyName, "rootbone");
							Object->m_SpeakBone = IniFile.GetValue(KeyName, "speakbone");

							// shadowmap
							Object->m_ShadowSize = static_cast<float>(IniFile.GetValueF(KeyName, "shadowsize"));

							Object->m_ShadowAlpha = static_cast<float>(IniFile.GetValueF(KeyName, "shadowalpha"));
							Object->m_ShadowBitmap = IniFile.GetValue(KeyName, "shadowbitmap");
							Object->m_ShadowAlphamap = IniFile.GetValue(KeyName, "shadowalphamap");

							// projected shadows enabled per pawn type
							Object->m_ProjectedShadows = false;
							value = IniFile.GetValue(KeyName, "projectedshadows");
							if(value == "true")
								Object->m_ProjectedShadows = true;

							// stencil shadows enabled per pawn type
							Object->m_StencilShadows = GE_FALSE;
							value = IniFile.GetValue(KeyName, "stencilshadows");
							if(value == "true")
								Object->m_StencilShadows = GE_TRUE;

							// icon
							Object->m_Icon = IniFile.GetValue(KeyName, "icon");

							Object->m_Location = pSource->origin;
							Object->m_HideFromRadar = pSource->HideFromRadar;

							if(!EffectC_IsStringNull(pSource->SpawnOrder))
							{
								Object->Order = pSource->SpawnOrder;
								Object->m_RunOrder = true;
							}

							if(!EffectC_IsStringNull(pSource->SpawnPoint))
								Object->m_Point = pSource->SpawnPoint;

							if(!EffectC_IsStringNull(pSource->ChangeMaterial))
								Object->m_ChangeMaterial = pSource->ChangeMaterial;

							Object->m_YRotation = GE_PIOVER180*(pSource->Angle.Y-90.0f);
							Object->m_Alive = true;

							found = true;
							break;
						}
					}

					KeyName = IniFile.FindNextKey();
				}

				if(!found)
				{
					delete pSource->Data;
					pSource->Data = NULL;
					continue;
				}

				if(!EffectC_IsStringNull(pSource->ConvScriptName))
				{
					CEGUI::System::getSingleton().executeScriptFile(pSource->ConvScriptName);

					if(!EffectC_IsStringNull(pSource->ConvLayout))
						Object->m_Conversation = new CConversation(Object, pSource->ConvLayout);
					else
						Object->m_Conversation = new CConversation(Object);

					if(!EffectC_IsStringNull(pSource->ConvOrder))
						Object->m_Conversation->SetOrder(pSource->ConvOrder);
				}
			}
		}
	}
}

/* ------------------------------------------------------------------------------------ */
// Destructor
/* ------------------------------------------------------------------------------------ */
CPawnManager::~CPawnManager()
{
	EventStack *pool, *temp;
	pool = Events;

	while(pool != NULL)
	{
		temp = pool->next;
		geRam_Free(pool);
		pool = temp;
	}

	geEntity_EntitySet *pSet;

	// Ok, check to see if there are Pawns in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "Pawn");

	if(!pSet)
		return;

	geEntity *pEntity;

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Pawn *pSource = static_cast<Pawn*>(geEntity_GetUserData(pEntity));

		if(pSource->Data)
		{
			ScriptedObject *Object = static_cast<ScriptedObject*>(pSource->Data);
			ScriptManager::RemoveGlobalVariable(pSource->szEntityName);
			delete Object;
			pSource->Data = NULL;
		}
	}
}

/* ------------------------------------------------------------------------------------ */
// Update
/* ------------------------------------------------------------------------------------ */
void CPawnManager::Tick(float dwTicks)
{
	ConsoleBlock = 0;

	// Ok, check to see if there are Pawns in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "Pawn");

	if(!pSet)
		return;

	geEntity *pEntity;
	skRValueArray args;
	skRValue ret;

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Pawn *pSource = static_cast<Pawn*>(geEntity_GetUserData(pEntity));

		if(pSource->Data)
		{
			ScriptedObject *Object = static_cast<ScriptedObject*>(pSource->Data);

			if(!EffectC_IsStringNull(pSource->SpawnTrigger) && !Object->m_Active)
			{
				if(!GetTriggerState(pSource->SpawnTrigger))
					continue;
			}

			Object->Update(dwTicks);

			if(Object->m_Console && ConsoleBlock < 4)
			{
				int x, y;

				x = 5;
				y = (115 * ConsoleBlock) + 5;

				geEngine_Printf(CCD->Engine()->Engine(), x, y, "%s", Object->m_ConsoleHeader);
				y += 10;

				if(!EffectC_IsStringNull(Object->m_ConsoleError))
					geEngine_Printf(CCD->Engine()->Engine(), x, y, "%s %s", Object->m_Indicate, Object->m_ConsoleError);

				for(int i=0; i<DEBUGLINES; ++i)
				{
					y += 10;

					if(!EffectC_IsStringNull(Object->m_ConsoleDebug[i]))
						geEngine_Printf(CCD->Engine()->Engine(), x, y, "%s", Object->m_ConsoleDebug[i]);
				}

				++ConsoleBlock;
			}
		}
	}
}

/* ------------------------------------------------------------------------------------ */
// HandleCollision
/* ------------------------------------------------------------------------------------ */
int CPawnManager::HandleCollision(const geActor *pActor, const geActor *theActor, bool Gravity)
{
	if(theActor != CCD->Player()->GetActor())
		return RGF_SUCCESS;

	// Ok, check to see if there are Pawns in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "Pawn");

	if(pSet)
	{
		geEntity *pEntity;

		for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
			pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
		{
			Pawn *pSource = static_cast<Pawn*>(geEntity_GetUserData(pEntity));

			if(pSource->Data)
			{
				ScriptedObject *Object = static_cast<ScriptedObject*>(pSource->Data);

				if(!Object->m_Alive)
					continue;

				if(Object->m_Active)
				{
					if(!Object->m_Actor)
						continue;

					if(Object->m_Actor != pActor)
						continue;

					Object->m_Collision = true;

					if(Object->m_Pushable && !Gravity)
					{
						geVec3d In, SavedPosition, NewPosition;
						int direct = CCD->Player()->LastDirection();

						if(direct == RGF_K_FORWARD || direct == RGF_K_BACKWARD)
							CCD->Player()->GetIn(&In);
						else
							CCD->Player()->GetLeft(&In);

						CCD->ActorManager()->GetPosition(Object->m_Actor, &SavedPosition);
						geFloat speed = CCD->Player()->LastMovementSpeed();

						if(direct == RGF_K_BACKWARD || direct == RGF_K_RIGHT)
							speed = -speed;

						geVec3d_AddScaled(&SavedPosition, &In, speed, &NewPosition);
						bool result = CCD->ActorManager()->ValidateMove(SavedPosition, NewPosition, Object->m_Actor, false);

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
// AddEvent
/* ------------------------------------------------------------------------------------ */
void CPawnManager::AddEvent(const std::string& Event, bool State)
{
	EventStack *pool, *temp;
	pool = Events;

	while(pool!= NULL)
	{
		temp = pool->next;

		if(pool->EventName == Event)
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

	pool->EventName = Event;
	pool->State = State;
}

/* ------------------------------------------------------------------------------------ */
// GetEventState
/* ------------------------------------------------------------------------------------ */
bool CPawnManager::GetEventState(const std::string& Event)
{
	EventStack *pool, *temp;
	pool = Events;

	while(pool != NULL)
	{
		temp = pool->next;

		if(pool->EventName == Event)
			return pool->State;

		pool = temp;
	}

	return false;
}


/* ------------------------------------------------------------------------------------ */
// PreLoad
//
// Preload scripts (sound files)
/* ------------------------------------------------------------------------------------ */
void CPawnManager::PreLoad(const std::string& filename)
{
	geVFile *fdInput = NULL;
	char szInputString[1024] = {};

	if(!CFileManager::GetSingletonPtr()->OpenRFFile(&fdInput, kScriptFile, filename.c_str(), GE_VFILE_OPEN_READONLY))
	{
		CCD->Log()->Warning("File %s - Line %d: Failed to open file: '%s'",
							__FILE__, __LINE__, filename.c_str());
		return;
	}

	while(geVFile_GetS(fdInput, szInputString, 1024) == GE_TRUE)
	{
		if(szInputString[0] == ';' || strlen(szInputString) <= 1)
			continue;

		std::string str(szInputString);
		TrimRight(str);
		MakeLower(str);
		Replace(str, "\\\\", "\\");
		int i = str.find(".wav");

		if(i >= 0 && i < static_cast<int>(str.length()))
		{
			int j = i-1;

			while(!(str[j] == '"' || str[j] == '[') && j >= 0)
			{
				--j;
			}

			if(j >= 0)
			{
				SPool_Sound(str.substr(j+1, i-j+3).c_str());
			}
		}
	}

	geVFile_Close(fdInput);
}

/* ------------------------------------------------------------------------------------ */
// ShowText
/* ------------------------------------------------------------------------------------ */
void CPawnManager::ShowText(int Nr)
{
	if(Nr < 0 || Nr >= MAXTEXT)
		return;

	if(TextMessage[Nr].ShowText)
	{
		if(!CEGUI::FontManager::getSingleton().isFontPresent(TextMessage[Nr].FontName.c_str()))
			return;

		geVec3d Pos, ScreenPos;
		geActor *Actor;

		if(!TextMessage[Nr].EntityName.empty()) // If no entity defined, then use absolute screen position
		{
			if(TextMessage[Nr].EntityName == "Player")
			{
				Pos = CCD->Player()->Position();
				Actor = CCD->Player()->GetActor();
			}
			else
			{
				Actor = CCD->ActorManager()->GetByEntityName(TextMessage[Nr].EntityName.c_str());
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

		float width = 0.f;
		int lines = 1;


		CEGUI::Font *font = CEGUI::FontManager::getSingleton().getFont(TextMessage[Nr].FontName.c_str());

		// :TODO: load message text from a language specific text file
		switch(TextMessage[Nr].Alignment)
		{
		case 'L':	// left
		case 'l':
			{
				CEGUI::Rect format_area;
				lines = font->getFormattedLineCount((const utf8*)TextMessage[Nr].TextString.c_str(), format_area, CEGUI::RightAligned);
				width = font->getFormattedTextExtent((const utf8*)TextMessage[Nr].TextString.c_str(), format_area, CEGUI::RightAligned);

				CEGUI::Rect draw_area(	ScreenPos.X - width,
										ScreenPos.Y - lines * font->getLineSpacing(),
										ScreenPos.X,
										ScreenPos.Y);
				CEGUI::colour col(1.f, 1.f, 1.f, TextMessage[Nr].Alpha/255.f);
				CEGUI::ColourRect colour_rect(col);

				font->drawText((const utf8*)TextMessage[Nr].TextString.c_str(), draw_area, 1.0f, CEGUI::RightAligned, colour_rect);

				break;
			}
		case 'C':	// center
		case 'c':
			{
				CEGUI::Rect format_area;
				lines = font->getFormattedLineCount((const utf8*)TextMessage[Nr].TextString.c_str(), format_area, CEGUI::Centred);
				width = font->getFormattedTextExtent((const utf8*)TextMessage[Nr].TextString.c_str(), format_area, CEGUI::Centred);

				CEGUI::Rect draw_area(	ScreenPos.X - width * 0.5f,
										ScreenPos.Y - lines * font->getLineSpacing(),
										ScreenPos.X + width * 0.5f,
										ScreenPos.Y);
				CEGUI::colour col(1.f, 1.f, 1.f, TextMessage[Nr].Alpha/255.f);
				CEGUI::ColourRect colour_rect(col);

				font->drawText((const utf8*)TextMessage[Nr].TextString.c_str(), draw_area, 1.0f, CEGUI::Centred, colour_rect);

				break;
			}
		default:	// right
			{
				CEGUI::Rect format_area;
				lines = font->getFormattedLineCount((const utf8*)TextMessage[Nr].TextString.c_str(), format_area, CEGUI::LeftAligned);
				width = font->getFormattedTextExtent((const utf8*)TextMessage[Nr].TextString.c_str(), format_area, CEGUI::LeftAligned);

				CEGUI::Rect draw_area(	ScreenPos.X,
										ScreenPos.Y - lines * font->getLineSpacing(),
										ScreenPos.X + width,
										ScreenPos.Y);
				CEGUI::colour col(1.f, 1.f, 1.f, TextMessage[Nr].Alpha/255.f);
				CEGUI::ColourRect colour_rect(col);

				font->drawText((const utf8*)TextMessage[Nr].TextString.c_str(), draw_area, 1.0f, CEGUI::LeftAligned, colour_rect);

				break;
			}
		}
	}
}

/* ------------------------------------------------------------------------------------ */
// FillScreenArea
/* ------------------------------------------------------------------------------------ */
void CPawnManager::FillScreenArea(int Nr)
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

// ******************** CRGF Overrides ********************

/* ------------------------------------------------------------------------------------ */
// LocateEntity
//
// Given a name, locate the desired item in the currently loaded level
// ..and return it's user data.
/* ------------------------------------------------------------------------------------ */
int CPawnManager::LocateEntity(const char *szName, void **pEntityData)
{
	// Ok, check to see if there are Pawns in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "Pawn");

	if(!pSet)
		return RGF_NOT_FOUND;

	geEntity *pEntity;

	// Ok, we have Pawns.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Pawn *pTheEntity = static_cast<Pawn*>(geEntity_GetUserData(pEntity));

		if(!strcmp(pTheEntity->szEntityName, szName))
		{
			*pEntityData = static_cast<void*>(pTheEntity);
			return RGF_SUCCESS;
		}
	}

	return RGF_NOT_FOUND;								// Sorry, no such entity here
}

/* ------------------------------------------------------------------------------------ */
// SaveTo
//
// Save the current state of every Pawn in the current world off to an open file.
/* ------------------------------------------------------------------------------------ */
int CPawnManager::SaveTo(FILE * /*SaveFD*/, bool /*type*/)
{
#if 0
	// Ok, check to see if there are Pawns in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "Pawn");

	if(!pSet)
		return RGF_NOT_FOUND;

	geEntity *pEntity;

	// Ok, we have Pawns.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Pawn *pSource = static_cast<Pawn*>(geEntity_GetUserData(pEntity));

		if(pSource->Data)
		{
			ScriptedObject *Object = static_cast<ScriptedObject*>(pSource->Data);
		}
	}
#endif
	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// RestoreFrom
//
// Restore the state of every Pawn in the current world from an open file.
/* ------------------------------------------------------------------------------------ */
int CPawnManager::RestoreFrom(FILE * /*RestoreFD*/, bool /*type*/)
{
#if 0
	// Ok, check to see if there are Pawns in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "Pawn");

	if(!pSet)
		return RGF_NOT_FOUND;

	geEntity *pEntity;

	// Ok, we have Pawns.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Pawn *pSource = static_cast<Pawn*>(geEntity_GetUserData(pEntity));

		if(pSource->Data)
		{
			ScriptedObject *Object = static_cast<ScriptedObject*>(pSource->Data);
		}
	}
#endif
	return RGF_SUCCESS;
}

/* ----------------------------------- END OF FILE ------------------------------------ */
