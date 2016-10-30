/************************************************************************************//**
 * @file CPawnManager.h
 * @brief CPawnManager class declaration
 * @author Ralph Deane
 *//*
 * Copyright (c) Ralph Deane; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_CPAWNMANAGER_H__
#define __RGF_CPAWNMANAGER_H__

#include "Simkin\\skScriptedExecutable.h"

#define DEBUGLINES	8
#define MAXFLAGS  500
#define MAXTEXT 20
#define MAXFILLAREA 20


typedef struct TxtMessage
{
	bool		ShowText;
	int			TextWidth;
	int			ScreenOffsetX;
	int			ScreenOffsetY;
	float		Alpha;
	char		Alignment;
	std::string FontName;
	std::string	EntityName;
	std::string	AnimName;
	std::string	TextSound;
	std::string	TextString;

} TxtMessage;


typedef struct FillSArea
{
	bool	DoFillScreenArea;
	bool	FillScreenAreaKeep;
	GE_Rect FillScreenAreaRect;
	GE_RGBA	FillScreenAreaColor;

} FillSArea;


typedef struct EventStack
{
	EventStack *next;
	EventStack *prev;
	bool		State;
	std::string EventName;

} EventStack;


class CConversation;
class ScriptedObject;


typedef struct BitampPreCache
{
	std::string Name;
	geBitmap*	Bitmap;

} BitmapPreCache;


typedef struct ActorPreCache
{
	std::string	Name;
	std::string	ActorName;
	geVec3d		Rotation;
	float		Scale;
	GE_RGBA		FillColor;
	GE_RGBA		AmbientColor;
	bool		AmbientLightFromFloor;
	bool		EnvironmentMapping;
	bool		AllMaterial;
	float		PercentMapping;
	float		PercentMaterial;

} ActorPreCache;


/**
 * @brief CPawnManager class
 */
class CPawnManager : public CRGFComponent
{
public:
	CPawnManager();	// Constructor
	~CPawnManager();

	void Tick(float timeElapsed);

	int HandleCollision(const geActor *pActor, const geActor *theActor, bool Gravity);

	int SaveTo(FILE *SaveFD, bool type);

	int RestoreFrom(FILE *RestoreFD, bool type);

	bool Converse(const geActor *pActor);

	/**
	 * @brief Given a name, locate the desired entity in the currently loaded
	 * level and return its user data.
	 */
	int LocateEntity(const char *szName, void **pEntityData);

	void AddEvent(const std::string& Event, bool state);

	bool GetEventState(const std::string& Event);

	bool GetConvFlag() const				{ return ConvFlag;			}

	void SetConvFlag(bool flag)				{ ConvFlag = flag;			}

	bool GetPawnFlag(int index) const		{ return PawnFlag[index];	}

	void SetPawnFlag(int index, bool flag)	{ PawnFlag[index] = flag;	}

	int GetBlock() const					{ return ConsoleBlock;		}

	void IncBlock()							{ ++ConsoleBlock;			}

	bool Area(const std::string& fromActorName, const std::string& toActorName, bool distanceMode,
				float minScreen, float maxScreen, float minDist, float maxDist,
				bool ignoreX, bool ignoreY, bool ignoreZ);

	void ShowText(int Nr);

	void FillScreenArea(int Nr);

	const std::vector<ActorPreCache>& GetAccessoryCache() const { return AccessoryCache; }

public:
	TxtMessage		TextMessage[MAXTEXT];
	StreamingAudio*	m_Streams;
	LPDIRECTSOUND	m_dsPtr;
	FillSArea		FillScrArea[MAXFILLAREA];

	friend class ScriptedObject;

private:
	void PreLoad(const std::string& filename);

private:

	bool ConvFlag;
	bool PawnFlag[MAXFLAGS];
	stdext::hash_map<std::string, std::string> m_Text;
	EventStack*	Events;
	std::vector<ActorPreCache> WeaponCache;
	std::vector<ActorPreCache> AccessoryCache;
	int ConsoleBlock;
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
