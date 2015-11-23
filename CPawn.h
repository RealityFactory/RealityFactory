#ifndef __RGF_CPAWN_H__ 
#define __RGF_CPAWN_H__

#include "Simkin\\skScriptedExecutable.h"

#define DEBUGLINES	8
#define MAXFLAGS  500

#define PARMCHECK(x) CCD->Pawns()->ParmCheck(arguments.entries(), x, Order, szName, methodName)

typedef struct ActionList
{
	ActionList *next;
	ActionList *prev;
	char AnimName[64];
	int Action;
	float Value1, Value2, Value3, Value4;
	float Speed;
	bool Flag;
	char SoundName[256];
	char TriggerName[256];
} ActionList;

typedef struct ActionStack
{
	ActionStack *next;
	ActionStack *prev;
	ActionList *Top;
	ActionList *Bottom;
	ActionList *Index;
	char Order[64];
	char Point[64];
	char NextOrder[64];
	geVec3d CurrentPoint;
	bool ActionActive;
	bool ValidPoint;
	geVec3d	Vec2Point;
	bool DistActive;
	float MinDistance;
	char DistOrder[64];
} ActionStack;

typedef enum
{
	PTRIGGER = 0,
	PFLAG,
	PTIMER,
	PDIST,
	PEND
};

typedef struct TriggerStack
{
	TriggerStack *next;
	TriggerStack *prev;
	char OrderName[64];
	float Delay;
	int Type;
	bool Flag;
	int PFlg;
	float Time;
	char TriggerName[128];
} TriggerStack;

typedef struct EventStack
{
	EventStack *next;
	EventStack *prev;
	bool State;
	char EventName[128];
} EventStack;


//
// ScriptConverse class
//

class ScriptedConverse : public skScriptedExecutable
{
public:
	ScriptedConverse(char *fileName);
	~ScriptedConverse();
	bool getValue (const skString& fieldName, const skString& attribute, skRValue& value);
    bool setValue (const skString& fieldName, const skString& attribute, const skRValue& value);
    bool method (const skString& methodName, skRValueArray& arguments,skRValue& returnValue);
	int DoConversation(int charpersec);
	void TextDisplay(char *Text, int Width, int Font);
	int TextOut(int startline, int Height, int Font, int X, int Y);
	void CreateCamera()
	{ M_Camera = geCamera_Create(2.0f, &M_CameraRect); }
	void DestroyCamera()
	{ geCamera_Destroy(&M_Camera); }

	char	Order[64];
	geBitmap	*Background;
	geBitmap	*Icon;
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
	CString Text;
	StreamingAudio *m_Streams;
	LPDIRECTSOUND m_dsPtr;
	CString Reply[9];
	CStringArray TextLines;
	bool ConvFlag;
private:
	geCamera *M_Camera;
	geRect 	 M_CameraRect;
	
};


//
// ScriptedObject class
//

class ScriptedObject : public skScriptedExecutable
{
public:
	ScriptedObject(char *fileName);
	~ScriptedObject();
	bool getValue (const skString& fieldName, const skString& attribute, skRValue& value);
    bool setValue (const skString& fieldName, const skString& attribute, const skRValue& value);
    bool method (const skString& methodName, skRValueArray& arguments,skRValue& returnValue);
	bool highmethod(const skString& methodName, skRValueArray& arguments,skRValue& returnValue);
	bool lowmethod(const skString& methodName, skRValueArray& arguments,skRValue& returnValue);
	void Push();
	void Pop();
	void RemoveTriggerStack(TriggerStack *tpool);
	void GetAngles(bool flag);
// Quake 2 movement functions
	bool CheckBottom();
	bool MoveStep(geVec3d move);
	bool StepDirection(float yaw, float dist);
	void ChangeYaw();
	void NewChaseDir(float dist);
	float anglemod(float a);
	bool CloseEnough(float dist);
	void MoveToGoal(float dist);

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
	float		ShadowSize;
	bool		HideFromRadar;
	char		ChangeMaterial[64];
	char		Attribute[64];
	int			OldAttributeAmount;
	float		Gravity;
	char		BoxAnim[64];
	geVec3d		Location;
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
	bool		console;
	char		DamageAttr[64];
	geVec3d		UpdateTargetPoint;
	char		*ConsoleHeader;
	char		*ConsoleError;
	char		*ConsoleDebug[DEBUGLINES];
	char		Indicate[2];

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



	geActor		*Actor;
	ActionList	*Top;
	ActionList	*Bottom;
	ActionList	*Index;
	ActionStack *Stack;
	TriggerStack *Trigger;
	geBitmap	*Icon;
	ScriptedConverse *Converse;


private:
	void AddAction(int Action, char *AnimName, float Speed, bool Flag,
		float Value1, float Value2, float Value3, float Value4,
		char *Sound, char *Trigger);
};

typedef struct ConversationText
{
	CString Text;
	CString Name;
} ConversationText;

typedef struct PreCache
{
	CString Name;
	geBitmap *Bitmap;
} PreCache;

class CPawn : public CRGFComponent
{
public:
	CPawn();	// Constructor
	~CPawn();
	void Tick(float dwTicks);
	bool Converse(geActor *pActor);
	void RunConverse(ScriptedConverse *Converse, char *szName, geBitmap *OIcon);
	int LocateEntity(char *szName, void **pEntityData);
	void AddEvent(char *Event, bool State);
	bool GetEventState(char *Event);
	CString GetText(char *Name);
	geBitmap *GetCache(char *Name);
	bool GetConvFlag()
	{ return ConvFlag; }
	void SetConvFlag(bool flag)
	{ ConvFlag = flag; }
	bool GetPawnFlag(int index)
	{ return PawnFlag[index]; }
	void SetPawnFlag(int index, bool flag)
	{ PawnFlag[index] = flag; }
	bool CanSee(float FOV, geActor *Actor, geActor *TargetActor, char *Bone);
	int GetBlock()
	{ return ConsoleBlock; }
	void IncBlock()
	{ ConsoleBlock += 1; }
	void ParmCheck(int Entries, int Desired, char *Order, char *szName, const skString& methodname);
private:
	void TickHigh(Pawn *pSource, ScriptedObject *Object, float dwTicks);
	void TickLow(Pawn *pSource, ScriptedObject *Object, float dwTicks);
	void Spawn(void *Data);
	bool RotateToPoint(void *Data, float dwTicks);
	bool RotateToAlign(void *Data, float dwTicks);
	bool Rotate(void *Data, float dwTicks);
	bool MoveToPoint(void *Data, float dwTicks);
	bool Move(void *Data, float dwTicks);
	bool RotateMoveToPoint(void *Data, float dwTicks);
	bool RotateMove(void *Data, float dwTicks);
	bool NextPoint(void *Data, float dwTicks);
	bool NextOrder(void *Data, float dwTicks);
	bool NextPath(void *Data, float dwTicks);
	bool Jump(void *Data, float dwTicks);
	bool AddTriggerOrder(void *Data, float dwTicks);
	bool DelTriggerOrder(void *Data, float dwTicks);
	bool RotateToPlayer(void *Data, float dwTicks);
	void PreLoad(char *filename);
	void PreLoadC(char *filename);
	void LoadConv();
	bool PlayerDistance(float FOV, float distance, geActor *Actor, geVec3d DeadPos, char *Bone);

	CIniFile AttrFile;
	geBitmap	*Background;
	geBitmap	*Icon;
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
	CArray<ConversationText, ConversationText> Text;
	EventStack *Events;
	CArray<PreCache, PreCache> Cache;
	int ConsoleBlock;
};

#endif