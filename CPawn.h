/************************************************************************************//**
 * @file CPawn.h
 * @brief CPawn class declaration
 * @author Ralph Deane
 *//*
 * Copyright (c) Ralph Deane; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_CPAWN_H__
#define __RGF_CPAWN_H__

#include "Simkin\\skScriptedExecutable.h"

#define DEBUGLINES	8
#define MAXFLAGS  500
#define MAXTEXT 20
#define MAXFILLAREA 20

#define PARMCHECK(x) CCD->Pawns()->ParmCheck(arguments.entries(), x, Order, szName, methodName)

typedef struct TxtMessage
{
	bool		ShowText;
	char		EntityName[64];
	char		AnimName[64];
	std::string	TextString;
	int			FontNr;
	int			TextWidth;
	char		TextSound[64];
	int			ScreenOffsetX;
	int			ScreenOffsetY;
	float		Alpha;
	char		Alignment;

} TxtMessage;

typedef struct FillSArea
{
	bool	DoFillScreenArea;
	bool	FillScreenAreaKeep;
	GE_Rect FillScreenAreaRect;
	GE_RGBA	FillScreenAreaColor;

} FillSArea;

typedef struct ActionList
{
	ActionList	*next;
	ActionList	*prev;
	char		AnimName[64];
	int			Action;
	float		Value1, Value2, Value3, Value4;
	float		Speed;
	bool		Flag;
	char		SoundName[256];
	char		TriggerName[256];

} ActionList;


typedef struct ActionStack
{
	ActionStack	*next;
	ActionStack *prev;

	ActionList	*Top;
	ActionList	*Bottom;
	ActionList	*Index;

	char		Order[64];
	char		Point[64];
	char		NextOrder[64];
	geVec3d		CurrentPoint;
	bool		ActionActive;
	bool		ValidPoint;
	geVec3d		Vec2Point;
	bool		DistActive;
	float		MinDistance;
	char		DistOrder[64];

} ActionStack;

typedef enum
{
	PTRIGGER = 0,
	PFLAG,
	PTIMER,
	PDIST,
	PSOUND,
	PCOLLIDE,
	PAREA,
	PEND
};

typedef struct TriggerStack
{
	TriggerStack	*next;
	TriggerStack	*prev;
	char			OrderName[64];
	float			Delay;
	int				Type;
	bool			Flag;
	int				PFlg;
	float			Time;
	float			Low;
	float			High;
	char			TriggerName[128];

} TriggerStack;

typedef struct EventStack
{
	EventStack *next;
	EventStack *prev;
	bool State;
	char EventName[128];

} EventStack;

/**
 * @brief ScriptConverse class
 */
class ScriptedConverse : public skScriptedExecutable
{
public:
	ScriptedConverse(const char *fileName);
	~ScriptedConverse();

	bool getValue(const skString &fieldName, const skString &attribute, skRValue &value);
	bool setValue(const skString &fieldName, const skString &attribute, const skRValue &value);
	bool method(const skString& methodName, skRValueArray &arguments, skRValue &returnValue, skExecutableContext &ctxt);

	int DoConversation(int charpersec);
	int DoSoundConversation(int charpersec, bool RenderHUD);


	void TextDisplay(const char *Text, int Width, int Font);
	int TextOut(int startline, int Height, int Font, int X, int Y);
	int TextOutLine(int startline, int Font, int X, int Y);

	void CreateCamera()		{ M_Camera = geCamera_Create(2.0f, &M_CameraRect); }
	void DestroyCamera()	{ geCamera_Destroy(&M_Camera); }

public:
	char		Order[64];
	geBitmap	*Background;
	geBitmap	*rBackground;	// changed Nout
	geBitmap	*Icon;
	geBitmap	*rIcon;			// changed Nout
	int			SpeachWindowX;
	int			SpeachWindowY;
	int			ReplyWindowX;
	int			ReplyWindowY;
	int			rBackgroundX;
	int			rBackgroundY;
	bool		DrawBackground;
	bool		DrawrBackground;
	bool		ReplyInSpeakWindow;
	StreamingAudio *m_Streams_reply;
	std::string	ReplySoundFileName[9];
	bool		ClearScreen;
	geVec3d		PawnPos;

	//Sound Conversation
	int SpeakShowTime;
	int ReplyShowTime;

	int BackgroundX;
	int BackgroundY;
	int IconX;
	int IconY;
	int SpeachX;
	int SpeachY;
	int SpeachWidth;
	int SpeachHeight;
	int SpeachFont;
	int ReplyX;
	int ReplyY;
	int ReplyWidth;
	int ReplyHeight;
	int ReplyFont;
	bool replyflg[9];
	std::string Text;
	StreamingAudio *m_Streams;

	int MyReplyWidth;
	int MyReplyHeight;
	bool ShowSelectedReply;

	LPDIRECTSOUND m_dsPtr;
	std::string Reply[9];

	std::vector<std::string> TextLines;
	bool ConvFlag;

	geBitmap *ReplyMenuBar;
	bool MouseReply;
	int	MouseRepPosX[9];
	int	MouseRepPosY[9];
	int	MouseRepWidth[9];
	int	MouseRepHeight[9];
	int	LastReplyNr;
	int ReplyMenuFont;
	bool RestoreBackground;
	int ReplyGifNr[9];
	int GifX;
	int GifY;
	StreamingAudio *m_Streams_click;
	StreamingAudio *m_Streams_mouseover;
	bool MouseMenu;
	geBitmap	*BGBitmap[9];
	std::string	SoundAtClick[9];
	std::string	SoundMouseOver[9];
	geBitmap	*MouseOverBitmap[9];

private:
	geCamera *M_Camera;
	geRect 	 M_CameraRect;

};


/**
 * @brief ScriptedObject class
 */
class ScriptedObject : public skScriptedExecutable
{
public:
	ScriptedObject(char *fileName);
	~ScriptedObject();

	bool getValue(const skString& fieldName, const skString& attribute, skRValue& value);
	bool setValue(const skString& fieldName, const skString& attribute, const skRValue& value);
	bool method(const skString& methodName, skRValueArray& arguments, skRValue& returnValue, skExecutableContext& ctxt);
	bool highmethod(const skString& methodName, skRValueArray& arguments, skRValue& returnValue, skExecutableContext& ctxt);
	bool lowmethod(const skString& methodName, skRValueArray& arguments, skRValue& returnValue, skExecutableContext& ctxt);

	void Push();
	void Pop();
	void RemoveTriggerStack(TriggerStack *tpool);
	void GetAngles(bool flag);

	// Quake 2 movement functions
	bool CheckBottom();
	bool MoveStep(const geVec3d &move);
	bool StepDirection(float yaw, float dist);
	void ChangeYaw();
	void ChangePitch();
	void NewChaseDir(float dist);
	float anglemod(float a);
	bool CloseEnough(float dist);
	void MoveToGoal(float dist);

public:
	bool		active;
	bool		alive;
	bool		highlevel;
	char		ActorName[64];
	float		ActorAlpha;
	char		szName[128];
	float		AnimSpeed;
	geVec3d		Rotation;
	float		YRotation;
	geFloat		Scale;
	GE_RGBA		FillColor;
	GE_RGBA		AmbientColor;
	bool		AmbientLightFromFloor;
	bool		EnvironmentMapping;
	bool		AllMaterial;
	float		PercentMapping;
	float		PercentMaterial;
	float		ShadowSize;
	geFloat		ShadowAlpha;
	char		ShadowBitmap[64];
	char		ShadowAlphamap[64];
	// projected shadows configurable per pawn type
	bool		ProjectedShadows;
	geBoolean	StencilShadows;
	bool		HideFromRadar;
	char		ChangeMaterial[64];
	char		Attribute[64];
	int			OldAttributeAmount;
	geVec3d		Gravity;
	char		BoxAnim[64];
	geVec3d		Location;
	char		RootBone[64];
	char		Order[64];
	char		Point[64];
	bool		RunOrder;
	geVec3d		CurrentPoint;
	bool		ValidPoint;
	bool		ActionActive;
	bool		StartAction;
	geVec3d		TempPoint;
	geVec3d		Vec2Point;
	char		NextOrder[64];
	float		Time;
	bool		DistActive;
	float		MinDistance;
	char		DistOrder[64];
	char		PainOrder[64];
	bool		PainActive;
	int			PainPercent;
	char		AvoidOrder[64];
	char		DeadOrder[64];
	geVec3d		DeadPos;
	int			Direction;
	float		TotalDist;
	bool		AvoidMode;
	bool		TriggerWait;
	float		TriggerTime;
	char		TriggerOrder[64];
	int			SoundIndex;
	float		AudibleRadius;
	bool		FacePlayer;
	bool		FaceAxis;
	bool		UseKey;
	float		FOV;
	char		FOVBone[64];
	char		Group[64];
	bool		HostilePlayer;
	bool		HostileDiff;
	bool		HostileSame;
	bool		TargetFind;
	char		TargetOrder[64];
	char		TargetAttr[64];
	float		TargetDistance;
	bool		TargetDisable;
	char		TargetGroup[64];
	geVec3d		LastTargetPoint;
	geVec3d		SavePoint;
	bool		collision;
	bool		pushable;
	bool		console;
	char		DamageAttr[64];
	geVec3d		UpdateTargetPoint;
	char		*ConsoleHeader;
	char		*ConsoleError;
	char		*ConsoleDebug[DEBUGLINES];
	char		Indicate[2];
	geVec3d		WRotation;
	geVec3d		WScale;
	bool		SoundLoop;
	float		Circle;

	bool PointFind;
	char PointOrder[64];
	geActor	*Prev_HL_Actor;
	GE_RGBA Prev_HL_FillColor;
	GE_RGBA Prev_HL_AmbientColor;
	geBoolean Prev_HL_AmbientLightFromFloor;

// Low Level variables
	float		lowTime;
	float		ThinkTime;
	float		ElapseTime;
	geActor		*TargetActor;
	char		thinkorder[64];
	float		attack_finished;
	int			attack_state;
	float		yaw_speed;
	float		ideal_yaw;
	float		actoryaw;
	float		targetyaw;
	float		actorpitch;
	float		targetpitch;
	float		pitch_speed;
	float		ideal_pitch;
	geActor		*Actor;
	geActor		*WeaponActor;
	ActionList	*Top;
	ActionList	*Bottom;
	ActionList	*Index;
	ActionStack *Stack;
	TriggerStack *Trigger;
	geBitmap	*Icon;
	ScriptedConverse *Converse;

private:
	void AddAction(int Action, const char *AnimName, float Speed, bool Flag,
		float Value1, float Value2, float Value3, float Value4,
		const char *Sound, const char *Trigger);

};

typedef struct ConversationText
{
	std::string Text;
	std::string Name;

} ConversationText;

typedef struct BitampPreCache
{
	std::string Name;
	geBitmap *Bitmap;

} BitmapPreCache;

typedef struct ActorPreCache
{
	std::string	Name;
	char		ActorName[64];
	geVec3d		Rotation;
	geFloat		Scale;
	GE_RGBA		FillColor;
	GE_RGBA		AmbientColor;
	bool		AmbientLightFromFloor;
	bool		EnvironmentMapping;
	bool		AllMaterial;
	float		PercentMapping;
	float		PercentMaterial;

} ActorPreCache;

/**
 * @brief CPawn class
 */
class CPawn : public CRGFComponent
{
public:
	CPawn();	// Constructor
	~CPawn();

	void Tick(geFloat dwTicks);
	int HandleCollision(const geActor *pActor, const geActor *theActor, bool Gravity);
	int SaveTo(FILE *SaveFD, bool type);
	int RestoreFrom(FILE *RestoreFD, bool type);
	bool Converse(const geActor *pActor);
	void RunConverse(ScriptedConverse *Converse, const char *szName, geBitmap *OIcon);
	int LocateEntity(const char *szName, void **pEntityData);
	void AddEvent(const char *Event, bool State);
	bool GetEventState(const char *Event);
	std::string GetText(const char *Name);
	geBitmap *GetCache(const char *Name);
	void AnimateWeapon();

	bool GetConvFlag()						{ return ConvFlag;			}
	void SetConvFlag(bool flag)				{ ConvFlag = flag;			}
	bool GetPawnFlag(int index)				{ return PawnFlag[index];	}
	void SetPawnFlag(int index, bool flag)	{ PawnFlag[index] = flag;	}

	void GetGifXY(int *pGifX, int *pGifY)	{ if(pGifX)*pGifX=GifX; if(pGifY)*pGifY=GifY; }

	bool CanSee(float FOV, const geActor *Actor, const geActor *TargetActor, const char *Bone);
	bool CanSeePoint(float FOV, const geActor *Actor, const geVec3d *TargetPoint, const char *Bone);

	int GetBlock()	{ return ConsoleBlock;	}
	void IncBlock()	{ ConsoleBlock += 1;	}

	void ParmCheck(int Entries, int Desired, const char *Order, const char *szName, const skString &methodname);
	void LoadConv(const char *convtxt);

	// changed Nout 12/15/05
	bool Area(const char *FromActorName, const char *ToActorName, bool DistanceMode,
				float MinScr, float MaxScr, float MinDist, float MaxDist,
				bool IgnoreX, bool IgnoreY, bool IgnoreZ);
	void ShowText(int Nr);
	void FillScreenArea(int Nr);

public:
	TxtMessage		TextMessage[MAXTEXT];
	StreamingAudio	*m_Streams;
	LPDIRECTSOUND	m_dsPtr;
	FillSArea		FillScrArea[MAXFILLAREA];
	CAnimGif		*GifFile[9];
private:
	void TickHigh(Pawn *pSource, ScriptedObject *Object, float dwTicks);
	void TickLow(Pawn *pSource, ScriptedObject *Object, float dwTicks);
	void Spawn(void *Data);
	bool RotateToPoint		(void *Data, float dwTicks);
	bool RotateAroundPoint	(void *Data, float dwTicks);
	bool RotateToAlign		(void *Data, float dwTicks);
	bool Rotate				(void *Data, float dwTicks);
	bool MoveToPoint		(void *Data, float dwTicks);
	bool Move				(void *Data, float dwTicks);
	bool RotateMoveToPoint	(void *Data, float dwTicks);
	bool RotateMove			(void *Data, float dwTicks);
	bool NextPoint			(void *Data, float dwTicks);
	bool NextOrder			(void *Data, float dwTicks);
	bool NextPath			(void *Data, float dwTicks);
	bool Jump				(void *Data, float dwTicks);
	bool AddTriggerOrder	(void *Data, float dwTicks);
	bool DelTriggerOrder	(void *Data, float dwTicks);
	bool RotateToPlayer		(void *Data, float dwTicks);
	void PreLoad(const char *filename);
	void PreLoadC(const char *filename);
	bool PlayerDistance(float FOV, float distance, const geActor *Actor, const geVec3d &DeadPos, const char *Bone);

private:
	CIniFile AttrFile;
	geBitmap	*Background;
	geBitmap	*rBackground;	// changed Nout
	geBitmap	*Icon;
	geBitmap	*rIcon;			// changed Nout

	int SpeachWindowX;
	int SpeachWindowY;
	int ReplyWindowX;
	int ReplyWindowY;
	int rBackgroundX;
	int rBackgroundY;

	geBitmap *ReplyMenuBar;
	int ReplyMenuFont;
	int GifX;
	int GifY;

	int BackgroundX;
	int BackgroundY;
	int IconX;
	int IconY;
	int SpeachX;
	int SpeachY;
	int SpeachWidth;
	int SpeachHeight;
	int SpeachFont;
	int ReplyX;
	int ReplyY;
	int ReplyWidth;
	int ReplyHeight;
	int ReplyFont;
	bool ConvFlag;
	bool PawnFlag[MAXFLAGS];
	std::vector<ConversationText> Text;
	EventStack *Events;
	std::vector<ActorPreCache> WeaponCache;
	std::vector<ActorPreCache> AccessoryCache;
	std::vector<BitmapPreCache> BitmapCache;
	int ConsoleBlock;
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
