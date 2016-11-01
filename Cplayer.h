/************************************************************************************//**
 * @file CPlayer.h
 * @brief Player character encapsulation class
 *
 * This file contains the class declaration for the player character (avatar)
 * in an RGF-based game.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/


#ifndef __RGF_CPLAYER_H__
#define __RGF_CPLAYER_H__

#include "RFSX\\RFSX.h"

enum
{
	IDLE = 0,
	WALK,
	RUN,
	JUMP,
	STARTJUMP,
	FALL,
	LAND,
	CLIMBIDLE,
	CLIMBDOWN,
	CLIMBUP,
	SLIDELEFT,
	RUNSLIDELEFT,
	SLIDERIGHT,
	RUNSLIDERIGHT,
	CRAWL,
	CIDLE,
	CSTARTJUMP,
	CLAND,
	SLIDECLEFT,
	SLIDECRIGHT,
	I2WALK,
	I2RUN,
	W2IDLE,
	C2IDLE,
	CROUCH2IDLE,
	IDLE2CROUCH,
	SWIM2TREAD,
	TREAD2SWIM,
	IDLE2TREAD,
	SWIM2WALK,
	WALK2SWIM,
	TREAD2IDLE,
	JUMP2FALL,
	JUMP2TREAD,
	FALL2TREAD,
	FALL2CRAWL,
	FALL2WALK,
	FALL2JUMP,
	WALK2JUMP,
	WALK2CRAWL,
	CRAWL2WALK,
	IDLE2CRAWL,
	AIM2CROUCH,
	CROUCH2AIM,
	W2TREAD,
	FALL2RUN,
	CRAWL2RUN,
	SHOOT,
	SHOOT1,
	AIM,
	AIM1,
	WALKSHOOT,
	WALKSHOOT1,
	RUNSHOOT,
	RUNSHOOT1,
	SLIDELEFTSHOOT,
	SLIDELEFTSHOOT1,
	RUNSLIDELEFTSHOOT,
	RUNSLIDELEFTSHOOT1,
	SLIDERIGHTSHOOT,
	SLIDERIGHTSHOOT1,
	RUNSLIDERIGHTSHOOT,
	RUNSLIDERIGHTSHOOT1,
	JUMPSHOOT,
	JUMPSHOOT1,
	FALLSHOOT,
	FALLSHOOT1,
	CRAWLSHOOT,
	CRAWLSHOOT1,
	CSHOOT,
	CSHOOT1,
	CAIM,
	CAIM1,
	SLIDECLEFTSHOOT,
	SLIDECLEFTSHOOT1,
	SLIDECRIGHTSHOOT,
	SLIDECRIGHTSHOOT1,
	SWIM,
	TREADWATER,
	WALKBACK,
	RUNBACK,
	CRAWLBACK,
	WALKSHOOTBACK,
	WALKSHOOT1BACK,
	RUNSHOOTBACK,
	RUNSHOOT1BACK,
	CRAWLSHOOTBACK,
	CRAWLSHOOT1BACK,
	SWIMBACK,
	ANIMMAX
};

enum
{
	I2WALKTIME = 0,
	I2RUNTIME,
	W2IDLETIME,
	C2IDLETIME,
	CROUCH2IDLETIME,
	IDLE2CROUCHTIME,
	SWIM2TREADTIME,
	TREAD2SWIMTIME,
	IDLE2TREADTIME,
	TREAD2IDLETIME,
	SWIM2WALKTIME,
	WALK2SWIMTIME,
	JUMP2FALLTIME,
	JUMP2TREADTIME,
	FALL2TREADTIME,
	SLIDE2CROUCHTIME,
	SLIDE2IDLETIME,
	FALL2CRAWLTIME,
	FALL2WALKTIME,
	FALL2JUMPTIME,
	WALK2JUMPTIME,
	WALK2CRAWLTIME,
	CRAWL2WALKTIME,
	IDLE2CRAWLTIME,
	TRANSMAX
};


#define ANIMIDLE				CCD->Weapons()->PlayerAnim(IDLE)
#define ANIMWALK				CCD->Weapons()->PlayerAnim(WALK)
#define ANIMRUN					CCD->Weapons()->PlayerAnim(RUN)
#define ANIMJUMP				CCD->Weapons()->PlayerAnim(JUMP)
#define ANIMSTARTJUMP			CCD->Weapons()->PlayerAnim(STARTJUMP)
#define ANIMFALL				CCD->Weapons()->PlayerAnim(FALL)
#define ANIMLAND				CCD->Weapons()->PlayerAnim(LAND)
#define ANIMCLIMB				CCD->Weapons()->PlayerAnim(CLIMBIDLE)
#define ANIMCLIMBDOWN			CCD->Weapons()->PlayerAnim(CLIMBDOWN)
#define ANIMCLIMBUP				CCD->Weapons()->PlayerAnim(CLIMBUP)
#define ANIMSLIDELEFT			CCD->Weapons()->PlayerAnim(SLIDELEFT)
#define ANIMRUNSLIDELEFT		CCD->Weapons()->PlayerAnim(RUNSLIDELEFT)
#define ANIMSLIDERIGHT			CCD->Weapons()->PlayerAnim(SLIDERIGHT)
#define ANIMRUNSLIDERIGHT		CCD->Weapons()->PlayerAnim(RUNSLIDERIGHT)
#define ANIMCRAWL				CCD->Weapons()->PlayerAnim(CRAWL)
#define ANIMCIDLE				CCD->Weapons()->PlayerAnim(CIDLE)
#define ANIMI2WALK				CCD->Weapons()->PlayerAnim(I2WALK)
#define ANIMI2RUN				CCD->Weapons()->PlayerAnim(I2RUN)
#define ANIMW2IDLE				CCD->Weapons()->PlayerAnim(W2IDLE)
#define ANIMC2IDLE				CCD->Weapons()->PlayerAnim(C2IDLE)
#define ANIMCROUCH2IDLE			CCD->Weapons()->PlayerAnim(CROUCH2IDLE)
#define ANIMIDLE2CROUCH			CCD->Weapons()->PlayerAnim(IDLE2CROUCH)
#define ANIMSWIM2TREAD			CCD->Weapons()->PlayerAnim(SWIM2TREAD)
#define ANIMTREAD2SWIM			CCD->Weapons()->PlayerAnim(TREAD2SWIM)
#define ANIMIDLE2TREAD			CCD->Weapons()->PlayerAnim(IDLE2TREAD)
#define ANIMSWIM2WALK			CCD->Weapons()->PlayerAnim(SWIM2WALK)
#define ANIMWALK2SWIM			CCD->Weapons()->PlayerAnim(WALK2SWIM)
#define ANIMTREAD2IDLE			CCD->Weapons()->PlayerAnim(TREAD2IDLE)
#define ANIMJUMP2FALL			CCD->Weapons()->PlayerAnim(JUMP2FALL)
#define ANIMJUMP2TREAD			CCD->Weapons()->PlayerAnim(JUMP2TREAD)
#define ANIMFALL2TREAD			CCD->Weapons()->PlayerAnim(FALL2TREAD)
#define ANIMFALL2CRAWL			CCD->Weapons()->PlayerAnim(FALL2CRAWL)
#define ANIMFALL2WALK			CCD->Weapons()->PlayerAnim(FALL2WALK)
#define ANIMFALL2JUMP			CCD->Weapons()->PlayerAnim(FALL2JUMP)
#define ANIMWALK2JUMP			CCD->Weapons()->PlayerAnim(WALK2JUMP)
#define ANIMWALK2CRAWL			CCD->Weapons()->PlayerAnim(WALK2CRAWL)
#define ANIMCRAWL2WALK			CCD->Weapons()->PlayerAnim(CRAWL2WALK)
#define ANIMIDLE2CRAWL			CCD->Weapons()->PlayerAnim(IDLE2CRAWL)
#define ANIMAIM2CROUCH			CCD->Weapons()->PlayerAnim(AIM2CROUCH)
#define ANIMCROUCH2AIM			CCD->Weapons()->PlayerAnim(CROUCH2AIM)
#define ANIMW2TREAD				CCD->Weapons()->PlayerAnim(W2TREAD)
#define ANIMFALL2RUN			CCD->Weapons()->PlayerAnim(FALL2RUN)
#define ANIMCRAWL2RUN			CCD->Weapons()->PlayerAnim(CRAWL2RUN)
#define ANIMCSTARTJUMP			CCD->Weapons()->PlayerAnim(CSTARTJUMP)
#define ANIMCLAND				CCD->Weapons()->PlayerAnim(CLAND)
#define ANIMSLIDECLEFT			CCD->Weapons()->PlayerAnim(SLIDECLEFT)
#define ANIMSLIDECRIGHT			CCD->Weapons()->PlayerAnim(SLIDECRIGHT)
#define ANIMSHOOT				CCD->Weapons()->PlayerAnim(SHOOT)
#define ANIMSHOOT1				CCD->Weapons()->PlayerAnim(SHOOT1)
#define ANIMAIM					CCD->Weapons()->PlayerAnim(AIM)
#define ANIMAIM1				CCD->Weapons()->PlayerAnim(AIM1)
#define ANIMWALKSHOOT			CCD->Weapons()->PlayerAnim(WALKSHOOT)
#define ANIMWALKSHOOT1			CCD->Weapons()->PlayerAnim(WALKSHOOT1)
#define ANIMRUNSHOOT			CCD->Weapons()->PlayerAnim(RUNSHOOT)
#define ANIMRUNSHOOT1			CCD->Weapons()->PlayerAnim(RUNSHOOT1)
#define ANIMSLIDELEFTSHOOT		CCD->Weapons()->PlayerAnim(SLIDELEFTSHOOT)
#define ANIMSLIDELEFTSHOOT1		CCD->Weapons()->PlayerAnim(SLIDELEFTSHOOT1)
#define ANIMRUNSLIDELEFTSHOOT	CCD->Weapons()->PlayerAnim(RUNSLIDELEFTSHOOT)
#define ANIMRUNSLIDELEFTSHOOT1	CCD->Weapons()->PlayerAnim(RUNSLIDELEFTSHOOT1)
#define ANIMSLIDERIGHTSHOOT		CCD->Weapons()->PlayerAnim(SLIDERIGHTSHOOT)
#define ANIMSLIDERIGHTSHOOT1	CCD->Weapons()->PlayerAnim(SLIDERIGHTSHOOT1)
#define ANIMRUNSLIDERIGHTSHOOT	CCD->Weapons()->PlayerAnim(RUNSLIDERIGHTSHOOT)
#define ANIMRUNSLIDERIGHTSHOOT1	CCD->Weapons()->PlayerAnim(RUNSLIDERIGHTSHOOT1)
#define ANIMJUMPSHOOT			CCD->Weapons()->PlayerAnim(JUMPSHOOT)
#define ANIMJUMPSHOOT1			CCD->Weapons()->PlayerAnim(JUMPSHOOT1)
#define ANIMFALLSHOOT			CCD->Weapons()->PlayerAnim(FALLSHOOT)
#define ANIMFALLSHOOT1			CCD->Weapons()->PlayerAnim(FALLSHOOT1)
#define ANIMCRAWLSHOOT			CCD->Weapons()->PlayerAnim(CRAWLSHOOT)
#define ANIMCRAWLSHOOT1			CCD->Weapons()->PlayerAnim(CRAWLSHOOT1)
#define ANIMCSHOOT				CCD->Weapons()->PlayerAnim(CSHOOT)
#define ANIMCSHOOT1				CCD->Weapons()->PlayerAnim(CSHOOT1)
#define ANIMCAIM				CCD->Weapons()->PlayerAnim(CAIM)
#define ANIMCAIM1				CCD->Weapons()->PlayerAnim(CAIM1)
#define ANIMSLIDECLEFTSHOOT		CCD->Weapons()->PlayerAnim(SLIDECLEFTSHOOT)
#define ANIMSLIDECLEFTSHOOT1	CCD->Weapons()->PlayerAnim(SLIDECLEFTSHOOT1)
#define ANIMSLIDECRIGHTSHOOT	CCD->Weapons()->PlayerAnim(SLIDECRIGHTSHOOT)
#define ANIMSLIDECRIGHTSHOOT1	CCD->Weapons()->PlayerAnim(SLIDECRIGHTSHOOT1)
#define ANIMSWIM				CCD->Weapons()->PlayerAnim(SWIM)
#define ANIMTREADWATER			CCD->Weapons()->PlayerAnim(TREADWATER)
#define ANIMWALKBACK			CCD->Weapons()->PlayerAnim(WALKBACK)
#define ANIMRUNBACK				CCD->Weapons()->PlayerAnim(RUNBACK)
#define ANIMCRAWLBACK			CCD->Weapons()->PlayerAnim(CRAWLBACK)
#define ANIMWALKSHOOTBACK		CCD->Weapons()->PlayerAnim(WALKSHOOTBACK)
#define ANIMWALKSHOOT1BACK		CCD->Weapons()->PlayerAnim(WALKSHOOT1BACK)
#define ANIMRUNSHOOTBACK		CCD->Weapons()->PlayerAnim(RUNSHOOTBACK)
#define ANIMRUNSHOOT1BACK		CCD->Weapons()->PlayerAnim(RUNSHOOT1BACK)
#define ANIMCRAWLSHOOTBACK		CCD->Weapons()->PlayerAnim(CRAWLSHOOTBACK)
#define ANIMCRAWLSHOOT1BACK		CCD->Weapons()->PlayerAnim(CRAWLSHOOT1BACK)
#define ANIMSWIMBACK			CCD->Weapons()->PlayerAnim(SWIMBACK)


class StreamingAudio;
class CAudioStream;
class CPersistentAttributes;

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE CPlayer

#define SXPLAYER_METHODS 43

/**
 * @brief CPlayer handles the player character
 */
class CPlayer : public skExecutable
{
public:
	CPlayer();
	~CPlayer();

	int LoadAvatar(const std::string& file, const std::string& name);///< Load player character avatar
	int LoadConfiguration();										///< Load player configuration from PlayerSetup entity
	int LoadAttributesConfiguration(const std::string& filename);	///< Load player attribute definitions
	int LoadEnvironmentalAudio();									///< Load sounds for player movement
	int MoveToStart();												///< Move player to start

	void CheckMouseLook();									///< Process mouse-look
	void CheckKeyLook(int keyRotate, float timeElapsed);	///< Process key-look

	int ProcessMove(bool playerMoved);		///< Move player character
	int Move(int how, float speed);
	void StartJump();						///< Initiate a jump

	void Tick(float timeElapsed);

	geXForm3d ViewPoint();					///< Return players viewpoint
	void GetExtBoxWS(geExtBox* box);		///< Get ExtBox in worldspace
	void GetIn(geVec3d* in);				///< Get actors heading
	void GetLeft(geVec3d* left);

	int SaveTo(FILE* saveFD);				///< Save player
	int RestoreFrom(FILE* restoreFD);		///< Load player

	void SwitchCamera(int mode);
	void LookMode(bool bLookOn);			///< Set/unset third-person lookmode
	void SwitchToFirstPerson();				///< First-person mode
	void SwitchToThirdPerson();				///< Third-person mode

	int SaveAttributesAscii(const std::string& saveFile);
	int SaveAttributes(const std::string& saveFile);	///< Save inventory and attributes
	int LoadAttributes(const std::string& saveFile);	///< Load inventory and attributes

	CPersistentAttributes* GetAttributes()	{ return m_Attr; }

	bool GetUseAttribute(const std::string& attr);
	bool SetUseAttribute(const std::string& attr);
	bool DelUseAttribute(const std::string& attr);


	void Backtrack();						///< Force player to previous position in position history

	bool DoMovements(void);

	void UseItem();

	std::string GetDieAnim();
	std::string GetInjuryAnim();

	void SetCrouch(bool value);
	bool GetCrouch() const				{ return m_Crouch;				}
	void FlipCrouch()					{ m_Crouch = !m_Crouch;			}

	void Moving(int value)				{ m_Moving = value;				}
	int GetMoving() const				{ return m_Moving;				}

	int GetSlideWalk() const			{ return m_SlideWalk;			}
	void SetSlideWalk(int value)		{ m_SlideWalk = value;			}

	float Speed() const					{ return m_CurrentSpeed;		}	///< Get player speed

	geVec3d Position();
	float LastMovementSpeed() const		{ return m_LastMovementSpeed;	}

	bool FirstPersonView() const		{ return m_FirstPersonView;		}

	// Weapon
	geActor* GetActor()					{ return m_Actor;				}
	int GetViewPoint() const			{ return m_ViewPoint;			}
	int LastDirection() const			{ return m_LastDirection;		}

	void SetRunning(geBoolean running)	{ m_Running = running;			}
	void FlipRunning()					{ m_Running = !m_Running;		}

	void SetJumping(geBoolean jumping)	{ m_Jumping = jumping;			}

	const std::string& GetAnimations(int index) const	{ return m_Animations[index];	}

	bool GetAlive() const				{ return m_Alive;				}
	void SetAlive(bool flag)			{ m_Alive = flag;				}

	bool GetDying() const				{ return m_Dying;				}
	void SetDying(bool flag)			{ m_Dying = flag;				}

	bool GetMonitorState() const	{ return m_MonitorState;		}
	void SetMonitorState(bool flag)	{ m_MonitorState = flag;		}
	bool GetMonitorMode() const		{ return m_MonitorMode;			}

	bool GetDeathSpace() const		{ return m_DeathSpace;			}
	void SetDeathSpace(bool flag)	{ m_DeathSpace = flag;			}

	int GetMirror() const			{ return m_Mirror;				}

	bool GetRestoreOxy() const		{ return m_RestoreOxy;			}
	void SetRestoreOxy(bool flag)	{ m_RestoreOxy = flag;			}

	int GetHeadBobbing() const		{ return m_HeadBobbing;			}

	bool GetContinue() const		{ return m_ContinueAfterDeath;	}

	int LightValue() const			{ return static_cast<int>(m_CurrentLiteLife);	}
	int LightLife() const			{ return static_cast<int>(m_LiteLife);			}
	void FlipLight()				{ m_LightOn = !m_LightOn;		}
	void ModifyLight(int amount);

	float GetScale() const			{ return m_Scale;				}
	float GetHeight() const			{ return m_CurrentHeight;		}

	float GetSlideSlope() const		{ return m_SlideSlope;			}
	float GetSlideSpeed() const		{ return m_SlideSpeed;			}

	// start multiplayer
	const char* GetActorName() const	{ return m_ActorName;			}
	// end multiplayer
	void SetPlayerName(const std::string& name)	{m_PlayerName =name;	}
	const std::string& GetPlayerName()  const	{ return m_PlayerName;	}

	inline const std::string& GetIcon() const	{ return m_Icon;		}
	inline void SetIcon(const std::string& icon) { m_Icon = icon;	}

	void AddPosition();				///< Add current position to position history

	int  PlaySound(const char* sound, int soundHandle = -1, geBoolean loop = GE_TRUE, float radius = -1.0f);
	void UpdateSounds();
	void StopSound(int soundHandle);

	void SetSpeakBone(const std::string& bone) { m_SpeakBone = bone; }
	const std::string& GetSpeakBone() const { return m_SpeakBone; }
	geVec3d GetSpeakBonePosition();

	Q_METHOD_DECL(Speak);

private:
	geSound_Def* LoadAudioClip(const std::string& filename);	///< Load audio clip
	void SetJump();

private:
	std::string m_Icon;
	int			m_ViewPoint;			///< 0=first-person, 1=third-person
	bool		m_JumpActive;			///< Player is jumping
	bool		m_FirstPersonView;		///< Player in first-person view
	bool		m_LookMode;				///< TRUE if in third-person "look mode"
	int			m_Moving;				///< Non-zero if in third-person and moving
	geBoolean	m_Running;				///< Demande de course
	geBoolean	m_Jumping;				///< Demande de saut
	float		m_JumpSpeed;			///< Speed of jump
	float		m_CurrentHeight;		///< Player current height (mod if crouching)
	float		m_CurrentSpeed;			///< Player current speed
	float		m_SpeedCoeff;
	float		m_SpeedCoeffBackward;
	float		m_SpeedCoeffSideward;
	float		m_SpeedCoeffRun;
	float		m_Speed;				///< Player normal speed
	float		m_StepHeight;			///< Max. step-up height
	geVec3d		m_Translation;			///< Actor translation
	geActor*	m_Actor;				///< Genesis actor for player
	float		m_LastMovementSpeed;	///< Velocity of our last motion
	CPersistentAttributes* m_Attr;		///< Player attributes
	geVec3d		m_PositionHistory[50];	///< Last 50 places the player was
	int			m_PositionHistoryPtr;	///< Pointer into position history array
	geSound_Def* m_DefaultMotion[3];	///< Default motion sound
	geSound*	m_DefaultMotionHandle;	///< Default motion sound handle
	geSound_Def* m_Contents[16][3];		///< Sounds for each 'environment type'
	geSound*	m_ContentsHandles[16];	///< Sound handles for each 'environment type'
	float		m_Scale;				///< Actor scale factor

	int			m_SlideWalk;
	bool		m_Crouch;
	bool		m_OldCrouch;
	bool		m_Run;

	bool		m_HeadBobbing;
	float		m_HeadBobSpeed;			///< Speed of head bob
	float		m_HeadBobLimit;			///< Max. amount of head bob

	int			m_LookFlags;
	geVec3d		m_LookRotation;
	geVec3d		m_LookPosition;
	int			m_LastDirection;
	bool		m_Alive;
	bool		m_RealJumping;

	bool		m_LightActive;
	bool		m_LightOn;
	int			m_LightEffect;

	bool		m_LiteSpot;
	float		m_LiteArc;
	geVec3d		m_LiteOffset;
	int			m_LiteStyle;

	float		m_LiteRadiusMin;
	float		m_LiteRadiusMax;
	GE_RGBA		m_LiteColorMin;
	GE_RGBA		m_LiteColorMax;
	float		m_LiteIntensity;
	float		m_LiteLife;
	float		m_CurrentLiteLife;
	bool		m_DecayLite;
	float		m_LiteDecay;
	float		m_CurrentLiteDecay;
	float		m_LiteTime;
	float		m_LiteDecayTime;
	std::string	m_LiteBone;

	std::string	m_StaminaName;
	float		m_StaminaDelay;
	float		m_StaminaTime;

	std::string	m_StaminaName1[20];
	float		m_StaminaDelay1[20];
	float		m_StaminaTime1[20];

	bool		m_Allow3rdLook;
	std::string	m_Animations[ANIMMAX];

	int			m_LastHealth;
	bool		m_Injured;
	bool		m_Dying;
	std::string	m_DieAnim[5];
	int			m_DieAnimAmt;
	std::string	m_InjuryAnim[5];
	int			m_InjuryAnimAmt;
	std::string	m_LastMotion;

	geSound_Def *m_DieSound[5];
	int			m_DieSoundAmt;

	geSound_Def *m_InjurySound[5];
	int			m_InjurySoundAmt;

	geSound_Def *m_LandSound[5];
	int			m_LandSoundAmt;

	std::string m_SpeakBone;

	bool		m_Falling;
	bool		m_OldFalling;
	bool		m_FallInjure;
	geVec3d		m_FallStart;
	float		m_MinFallDist;
	float		m_MaxFallDist;
	float		m_FallDamage;

	GE_RGBA		m_FillColor;
	GE_RGBA		m_AmbientColor;
	geBoolean	m_AmbientLightFromFloor;
	geBoolean	m_EnvironmentMapping;
	geBoolean	m_AllMaterial;
	float		m_PercentMapping;
	float		m_PercentMaterial;

	float		m_UseRange;
	std::string	m_UseAttribute[10];
	float		m_BoxWidth;
	float		m_LiquidTime;
	int			m_InLiquid;
	int			m_SwimSound;
	int			m_SurfaceSound;
	int			m_InLiquidSound;
	bool		m_OnLadder;
	std::set<int> m_SoundHandles;

	char		m_ActorName[50];
	std::string	m_PlayerName;

	std::string	m_FallDamageAttr;
	float		m_TranTime[TRANSMAX];
	bool		m_AlwaysRun;
	bool		m_NoCrouchJump;
	int			m_Mirror;

	float		m_MinJumpOnDist;
	float		m_MaxJumpOnDist;
	float		m_JumpOnDamage;
	std::string	m_JumpOnDamageAttr;

	bool		m_LockView;
	bool		m_MonitorState;
	bool		m_MonitorMode;
	bool		m_RestoreOxy;
	std::string	m_ChangeMaterial;
	bool		m_ContinueAfterDeath;
	bool		m_DeathSpace;

	float		m_SlideSlope;
	float		m_SlideSpeed;

	StreamingAudio *m_StreamingAudio; // speaking

	bool		m_FirstFrame;
	// soundtrack
	std::string	m_szCDTrack;
	std::string	m_szMIDIFile;
	std::string	m_szStreamingAudio;
	bool		m_bSoundtrackLoops;

public:
	static void InitMHT();
	virtual bool getValue(const skString& name, const skString& att, skRValue& val);
	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);
	virtual int executableType() const { return RFSXG_PLAYER; }

protected:
	SX_METHOD_DECL(GetName);
	SX_METHOD_DECL(SetWeapon);
	SX_METHOD_DECL(SetWeaponMatFromFlip);
	SX_METHOD_DECL(ChangeWeaponMaterial);
	SX_METHOD_DECL(SetUseItem);
	SX_METHOD_DECL(ClearUseItem);
	SX_METHOD_DECL(HasAttribute);
	SX_METHOD_DECL(SetAttribute);
	SX_METHOD_DECL(GetAttribute);
	SX_METHOD_DECL(ModifyAttribute);
	SX_METHOD_DECL(AddAttribute);
	SX_METHOD_DECL(SetAttributeValueLimits);
	SX_METHOD_DECL(GetAttributeLowLimit);
	SX_METHOD_DECL(GetAttributeHighLimit);
	SX_METHOD_DECL(PowerUp);
	SX_METHOD_DECL(GetPowerUpLevel);
	SX_METHOD_DECL(SetMouseControlled);
	SX_METHOD_DECL(AttachBlendActor);
	SX_METHOD_DECL(DetachBlendActor);
	SX_METHOD_DECL(AttachAccessory);

	SX_METHOD_DECL(DetachAccessory);
	SX_METHOD_DECL(Render);
	SX_METHOD_DECL(MatchEntityAngles);
	SX_METHOD_DECL(ChangeMaterial);
	SX_METHOD_DECL(SetLighting);
	SX_METHOD_DECL(SetAlpha);
	SX_METHOD_DECL(GetAlpha);
	SX_METHOD_DECL(SetScale);
	SX_METHOD_DECL(SetPosition);
	SX_METHOD_DECL(GetPosition);
	SX_METHOD_DECL(SetRotation);
	SX_METHOD_DECL(GetRotation);
	SX_METHOD_DECL(Move);
	SX_METHOD_DECL(Rotate);
	SX_METHOD_DECL(GetScreenPosition);
	SX_METHOD_DECL(GetGroundTexture);
	SX_METHOD_DECL(AttachWindow);
	SX_METHOD_DECL(DetachWindow);
	SX_METHOD_DECL(Speak);
	SX_METHOD_DECL(PlaySound);

	SX_METHOD_DECL(SetIcon);
	SX_METHOD_DECL(GetIcon);
	SX_METHOD_DECL(SaveAttributes);

private:
	static RFSX::sxMHT<SX_IMPL_TYPE> h_method;
};

#undef SX_IMPL_TYPE

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
