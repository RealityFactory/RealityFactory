/************************************************************************************//**
 * @file CPawn.h
 * @brief CPawn class declaration
 * @author Ralph Deane
 *//*
 * Copyright (c) Ralph Deane; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_CPAWN_H__
#define __RGF_CPAWN_H__

#include <queue>
#include "RFSX\\RFSX.h"
#include "Simkin\\skScriptedExecutable.h"

#define DEBUGLINES	8
#define MAXFLAGS  500
#define MAXTEXT 20
#define MAXFILLAREA 20


typedef struct MethodQueueVariables
{
	std::string	Order;
	std::string	Point;
	std::string	NextOrder;
	std::string	DistOrder;
	geVec3d		CurrentPoint;
	bool		ActionActive;
	bool		ValidPoint;
	geVec3d		Vec2Point;
	bool		DistActive;
	float		MinDistance;

} MethodQueueVariables;


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


struct TriggerOrder
{
	TriggerOrder(
		const std::string& order = "",
		int		type = PTRIGGER,
		float	delay = 0.0f,
		bool	flag = false,
		int		pflg = 0,
		float	time = 0.0f,
		float	low = 0.0f,
		float	high = 0.0f
		) :
		Type(type),
		PFlg(pflg),
		Delay(delay),
		Time(time),
		Low(low),
		High(high),
		Flag(flag),
		OrderName(order)
	{}

	int				Type;
	int				PFlg;
	float			Delay;
	float			Time;
	float			Low;
	float			High;
	bool			Flag;
	std::string		OrderName;
};



class CConversation;

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE ScriptedObject

#ifdef SX_METHOD_ARGS
#undef SX_METHOD_ARGS
#endif
#define SX_METHOD_ARGS sxScriptedObjectMethodArgs

typedef struct SX_METHOD_ARGS SX_METHOD_ARGS;

/**
 * @brief ScriptedObject class
 */
class ScriptedObject : public skScriptedExecutable
{
public:
	explicit ScriptedObject(const std::string& filename);
	~ScriptedObject();

	bool getValue(const skString& fieldName, const skString& attribute, skRValue& value);
	bool setValue(const skString& fieldName, const skString& attribute, const skRValue& value);
	bool method(const skString& methodName, skRValueArray& arguments, skRValue& returnValue, skExecutableContext& ctxt);
	bool highmethod(const skString& methodName, skRValueArray& arguments, skRValue& returnValue, skExecutableContext& ctxt);
	bool lowmethod(const skString& methodName, skRValueArray& arguments, skRValue& returnValue, skExecutableContext& ctxt);

	void Push();
	void Pop();
	void Enqueue(SX_METHOD_ARGS* ma);
	void Dequeue();
	void DequeueTop();
	void DequeueAll();
	void GetAngles(bool flag);

	void Update(float timeElapsed);

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

	inline const std::string& GetIcon()const { return m_Icon; }

	void SetSpeakBone(const std::string& bone) { m_SpeakBone = bone; }
	const std::string& GetSpeakBone() const { return m_SpeakBone; }
	geVec3d GetSpeakBonePosition();
	geVec3d GetPosition();

public:
	bool		m_Active;
	bool		m_Alive;
	geBoolean	m_HighLevel;
	std::string	m_ActorName;
	float		m_ActorAlpha;
	std::string	szName;
	float		m_AnimSpeed;
	geVec3d		m_Rotation;
	float		m_YRotation;
	float		m_Scale;
	GE_RGBA		m_FillColor;
	GE_RGBA		m_AmbientColor;
	bool		m_AmbientLightFromFloor;
	bool		m_EnvironmentMapping;
	bool		m_AllMaterial;
	float		m_PercentMapping;
	float		m_PercentMaterial;
	float		m_ShadowSize;
	float		m_ShadowAlpha;
	std::string	m_ShadowBitmap;
	std::string	m_ShadowAlphamap;
	bool		m_ProjectedShadows;
	geBoolean	m_StencilShadows;
	bool		m_HideFromRadar;
	std::string	m_ChangeMaterial;
	std::string	m_Attribute;
	int			m_OldAttributeAmount;
	geVec3d		m_Gravity;
	std::string	m_BoxAnim;
	geVec3d		m_Location;
	std::string	m_RootBone;
	std::string	m_SpeakBone;
	std::string	Order;
	std::string	m_Point;
	bool		m_RunOrder;
	geVec3d		m_CurrentPoint;
	bool		m_ValidPoint;
	bool		m_ActionActive;
	bool		m_StartAction;
	geVec3d		m_TempPoint;
	geVec3d		m_Vec2Point;
	std::string	m_NextOrder;
	float		m_Time;
	bool		m_DistActive;
	float		m_MinDistance;
	std::string	m_DistOrder;
	std::string	m_PainOrder;
	bool		m_PainActive;
	int			m_PainPercent;
	std::string	m_AvoidOrder;
	std::string	m_DeadOrder;
	geVec3d		m_DeadPos;
	int			m_Direction;
	float		m_TotalDist;
	bool		m_AvoidMode;
	bool		m_TriggerWait;
	float		m_TriggerTime;
	std::string	m_TriggerOrder;
	int			m_SoundID;
	float		m_AudibleRadius;
	bool		m_FacePlayer;
	bool		m_FaceAxis;
	bool		m_UseKey;
	float		m_FOV;
	std::string	m_FOVBone;
	std::string	m_Group;
	bool		m_HostilePlayer;
	bool		m_HostileDiff;
	bool		m_HostileSame;
	bool		m_TargetFind;
	std::string	m_TargetOrder;
	std::string	m_TargetAttr;
	float		m_TargetDistance;
	bool		m_TargetDisable;
	std::string	m_TargetGroup;
	geVec3d		m_LastTargetPoint;
	geVec3d		m_SavePoint;
	bool		m_Collision;
	bool		m_Pushable;
	std::string	m_DamageAttr;
	geVec3d		m_UpdateTargetPoint;
	bool		m_Console;
	char*		m_ConsoleHeader;
	char*		m_ConsoleError;
	char*		m_ConsoleDebug[DEBUGLINES];
	char		m_Indicate[2];
	geVec3d		m_WRotation;
	geVec3d		m_WScale;
	bool		m_SoundLoop;
	float		m_Circle;

	bool		m_PointFind;
	std::string m_PointOrder;

	geActor*	m_TargetActor;

private:
	geActor*	m_Prev_HL_Actor;
	GE_RGBA		m_Prev_HL_FillColor;
	GE_RGBA		m_Prev_HL_AmbientColor;
	geBoolean	m_Prev_HL_AmbientLightFromFloor;
	std::set<int> m_SoundHandles;
	StreamingAudio *m_StreamingAudio;

	void RestoreHLActorDynamicLighting();

private:
	// Low Level variables
	float		m_lowTime;
	float		m_ThinkTime;
	float		m_ElapseTime;
	std::string	m_thinkorder;
	float		m_attack_finished;
	int			m_attack_state;
	float		m_yaw_speed;
	float		m_ideal_yaw;
	float		m_actoryaw;
	float		m_targetyaw;
	float		m_actorpitch;
	float		m_targetpitch;
	float		m_pitch_speed;
	float		m_ideal_pitch;

public:
	geActor*			m_Actor;
	geActor*			m_WeaponActor;

	stdext::hash_map<std::string, TriggerOrder*>	m_WatchedTriggers;
	std::stack<std::queue<SX_METHOD_ARGS*>*>		m_MethodQueueStack;
	std::stack<MethodQueueVariables*>				m_MethodQueueVariablesStack;

	std::string			m_Icon;

	CConversation*		m_Conversation;

private:
	void Spawn();
	void AnimateWeapon();
	void UpdateHigh(float timeElapsed);
	void UpdateLow(float timeElapsed);

	int  PlaySound(const char* sound, int soundHandle = -1, geBoolean loop = GE_TRUE, float radius = -1.0f);
	void UpdateSounds();
	void StopSound(int soundHandle);

	void UpdateWatchedTriggers(float timeElapsed);
	void CheckWatchedTriggers();
	void ClearWatchedTriggers();

	void FindScriptPoint();
	void FindTarget();

	void ClearActionList();

	bool PlayerDistance(float minDistance);

	bool CanSee(float FOV, const geActor *Actor, const geActor* TargetActor, const std::string& Bone);
	bool CanSeePoint(float FOV, const geActor *Actor, const geVec3d* TargetPoint, const std::string& Bone);

	bool MoveToPoint(float timeElapsed, skRValueArray& args);
	bool RotateToPoint(float timeElapsed, const std::string& animation, float rSpeed, bool rotateXY);
	bool Rotate(float timeElapsed, const std::string& animation, float speed, geVec3d *angle);
	bool RotateMoveToPoint(float timeElapsed, const std::string& animation, float rSpeed, float speed, bool rotateXY);
	bool Move(float timeElapsed, const std::string& animation, float speed, float totalDistance,
		float angleX = 0.0f, float angleY = 0.0f, float angleZ = 0.0f);

private:
	// returning true if finished, else false
	Q_METHOD_DECL(MoveToPoint);
	Q_METHOD_DECL(RotateToPoint);
	Q_METHOD_DECL(NewOrder);
	Q_METHOD_DECL(NextOrder);
	Q_METHOD_DECL(RotateToAlign);
	Q_METHOD_DECL(NextPoint);
	Q_METHOD_DECL(Delay);
	Q_METHOD_DECL(PlayAnimation);
	Q_METHOD_DECL(BlendToAnimation);
	Q_METHOD_DECL(LoopAnimation);
	Q_METHOD_DECL(Rotate);
	Q_METHOD_DECL(RotateMoveToPoint);
	Q_METHOD_DECL(RotateMove);
	Q_METHOD_DECL(NewPath);
	Q_METHOD_DECL(RestartOrder);
	Q_METHOD_DECL(PlayerDistOrder);
	//Q_METHOD_DECL(Console);
	Q_METHOD_DECL(AudibleRadius);
	Q_METHOD_DECL(AddPainOrder);
	Q_METHOD_DECL(FindTargetOrder);
	Q_METHOD_DECL(FindPointOrder);
	Q_METHOD_DECL(NewPoint);
	Q_METHOD_DECL(MoveForward);
	Q_METHOD_DECL(MoveBackward);
	Q_METHOD_DECL(MoveLeft);
	Q_METHOD_DECL(MoveRight);
	Q_METHOD_DECL(Move);
	Q_METHOD_DECL(AvoidOrder);
	Q_METHOD_DECL(Return);
	Q_METHOD_DECL(Align);
	Q_METHOD_DECL(Jump);
	Q_METHOD_DECL(AddTriggerOrder);
	Q_METHOD_DECL(DelTriggerOrder);
	Q_METHOD_DECL(SetEventState);
	Q_METHOD_DECL(FacePlayer);
	Q_METHOD_DECL(RotateToPlayer);
	Q_METHOD_DECL(RotateAroundPoint);
	//Q_METHOD_DECL(RotateAroundPointLeft);
	//Q_METHOD_DECL(RotateAroundPointRight);
	Q_METHOD_DECL(TeleportToPoint);
	Q_METHOD_DECL(AnimationSpeed);
	Q_METHOD_DECL(SetFlag);
	//Q_METHOD_DECL(AddFlagOrder);
	//Q_METHOD_DECL(DelFlagOrder);
	Q_METHOD_DECL(ChangeMaterial);
	Q_METHOD_DECL(ChangeWeaponMaterial);
	//Q_METHOD_DECL(AddTimerOrder);
	//Q_METHOD_DECL(DelTimerOrder);
	//Q_METHOD_DECL(AddRandomSound);
	//Q_METHOD_DECL(DelRandomSound);
	//Q_METHOD_DECL(AddDistanceOrder);
	//Q_METHOD_DECL(DelDistanceOrder);
	//Q_METHOD_DECL(AddCollisionOrder);
	//Q_METHOD_DECL(DelCollisionOrder);
	Q_METHOD_DECL(AnimateStop);
	Q_METHOD_DECL(AttributeOrder);
	Q_METHOD_DECL(Remove);
	Q_METHOD_DECL(SetKeyPause);
	Q_METHOD_DECL(SetNoCollision);
	Q_METHOD_DECL(SetCollision);
	Q_METHOD_DECL(AllowUseKey);
	Q_METHOD_DECL(SetHudDraw);
	Q_METHOD_DECL(HideFromRadar);
	Q_METHOD_DECL(Conversation);
	Q_METHOD_DECL(FadeIn);
	Q_METHOD_DECL(FadeOut);
	Q_METHOD_DECL(SetFOV);
	Q_METHOD_DECL(StepHeight);
	Q_METHOD_DECL(SetGroup);
	Q_METHOD_DECL(HostilePlayer);
	Q_METHOD_DECL(HostileDifferent);
	Q_METHOD_DECL(HostileSame);
	Q_METHOD_DECL(Gravity);
	Q_METHOD_DECL(SoundLoop);
	Q_METHOD_DECL(Speak);
	Q_METHOD_DECL(IsPushable);
	Q_METHOD_DECL(IsVehicle);
	Q_METHOD_DECL(MoveToTarget);
	Q_METHOD_DECL(RotateToTarget);
	Q_METHOD_DECL(RotateMoveToTarget);
	Q_METHOD_DECL(LowLevel);
	Q_METHOD_DECL(BoxWidth);
	Q_METHOD_DECL(BoxHeight);
	Q_METHOD_DECL(Scale);
	Q_METHOD_DECL(SetScale);
	Q_METHOD_DECL(FireProjectile);
	Q_METHOD_DECL(AddExplosion);
	Q_METHOD_DECL(TargetGroup);
	Q_METHOD_DECL(TestDamageOrder);
	Q_METHOD_DECL(SetLODDistance);
	Q_METHOD_DECL(AttachToActor);
	Q_METHOD_DECL(DetachFromActor);
	Q_METHOD_DECL(AttachBlendActor);
	Q_METHOD_DECL(DetachBlendActor);
	Q_METHOD_DECL(AttachAccessory);
	Q_METHOD_DECL(DetachAccessory);
	Q_METHOD_DECL(SetWeapon);
	Q_METHOD_DECL(RemoveWeapon);
	//Q_METHOD_DECL(random);
	Q_METHOD_DECL(debug);
	Q_METHOD_DECL(ShowTextDelay);
	Q_METHOD_DECL(ShowText);
	Q_METHOD_DECL(RemoveText);
	//Q_METHOD_DECL(GetConvReplyNr);
	//Q_METHOD_DECL(Concat);
	//Q_METHOD_DECL(GetAttribute);
	//Q_METHOD_DECL(ModifyAttribute);
	//Q_METHOD_DECL(SetAttribute);
	//Q_METHOD_DECL(AddAttribute);
	//Q_METHOD_DECL(SetAttributeValueLimits);
	//Q_METHOD_DECL(GetFlag);
	Q_METHOD_DECL(MouseControlledPlayer);
	//Q_METHOD_DECL(GetEventState);
	Q_METHOD_DECL(EndScript);
};

#undef SX_IMPL_TYPE

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
