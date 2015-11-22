/*
CPlayer.h:		Player character encapsulation class

  (c) 2001 Ralph Deane
  
	This file contains the class declaration for the player
	character (avatar) in an RGF-based game.
*/

#ifndef __RGF_CPLAYER_H__
#define __RGF_CPLAYER_H__

enum {
	IDLE = 0,
	I2WALK,
	I2RUN,
	WALK,
	W2IDLE,
	RUN,
	JUMP,
	STARTJUMP,
	FALL,
	LAND,
	SLIDELEFT,
	RUNSLIDELEFT,
	SLIDERIGHT,
	RUNSLIDERIGHT,
	CRAWL,
	CIDLE,
	C2IDLE,
	CSTARTJUMP,
	CLAND,
	SLIDECLEFT,
	SLIDECRIGHT,
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
// changed RF063
	SWIM,
	TREADWATER,
// end change RF063
	ANIMMAX
};

#define ANIMIDLE				CCD->Weapons()->PlayerAnim(IDLE)
#define ANIMI2WALK				CCD->Weapons()->PlayerAnim(I2WALK)
#define ANIMI2RUN				CCD->Weapons()->PlayerAnim(I2RUN)
#define ANIMWALK				CCD->Weapons()->PlayerAnim(WALK)
#define ANIMW2IDLE				CCD->Weapons()->PlayerAnim(W2IDLE)
#define ANIMRUN					CCD->Weapons()->PlayerAnim(RUN)
#define ANIMJUMP				CCD->Weapons()->PlayerAnim(JUMP)
#define ANIMSTARTJUMP			CCD->Weapons()->PlayerAnim(STARTJUMP)
#define ANIMFALL				CCD->Weapons()->PlayerAnim(FALL)
#define ANIMLAND				CCD->Weapons()->PlayerAnim(LAND)
#define ANIMSLIDELEFT			CCD->Weapons()->PlayerAnim(SLIDELEFT)
#define ANIMRUNSLIDELEFT		CCD->Weapons()->PlayerAnim(RUNSLIDELEFT)
#define ANIMSLIDERIGHT			CCD->Weapons()->PlayerAnim(SLIDERIGHT)
#define ANIMRUNSLIDERIGHT		CCD->Weapons()->PlayerAnim(RUNSLIDERIGHT)
#define ANIMCRAWL				CCD->Weapons()->PlayerAnim(CRAWL)
#define ANIMCIDLE				CCD->Weapons()->PlayerAnim(CIDLE)
#define ANIMC2IDLE				CCD->Weapons()->PlayerAnim(C2IDLE)
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
#define ANIMRUNSLIDERIGHTSHOOT 	CCD->Weapons()->PlayerAnim(RUNSLIDERIGHTSHOOT)
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
// changed RF063
#define ANIMSWIM				CCD->Weapons()->PlayerAnim(SWIM)
#define ANIMTREADWATER			CCD->Weapons()->PlayerAnim(TREADWATER)
// end change RF063
#define ANIMDIE					GetDieAnim()
#define ANIMINJURY				GetInjuryAnim()

class CAudioStream;
class CPersistentAttributes;

class CPlayer
{
public:
	CPlayer();
	~CPlayer();
	int LoadAvatar(char *szFile);	// Load player character avatar
	int LoadConfiguration();
	// Load player configuration from PlayerSetup entity
	int LoadEnvironmentalAudio();	// Load sounds for player movement
	int MoveToStart();	// Move player to start
	void CheckMouseLook();				// Process mouse-look
	void CheckKeyLook(int keyrotate);	// Process key-look update #2
	int ProcessMove(bool bPlayerMoved);		// Move player character
	int Move(int nHow, geFloat fSpeed);
	void StartJump();							// Initiate a jump
	void SwitchCamera(int mode);
	void Tick(geFloat dwTicks);
	geXForm3d ViewPoint();				// Return players viewpoint
	void GetExtBoxWS(geExtBox *theBox);	// Get ExtBox in worldspace
	void GetIn(geVec3d *In);			// Get actors heading
	void GetLeft(geVec3d *Left);
	int SaveTo(FILE *SaveFD);	// Save player
	int RestoreFrom(FILE *RestoreFD);	// Load player
	void LookMode(bool bLookOn);	// Set/unset third-person lookmode
	void SwitchToFirstPerson();	// First-person mode
	void SwitchToThirdPerson();	// Third-person mode
	int SaveAttributes(char *szSaveFile);			// Save inventory and attributes
	int LoadAttributes(char *szSaveFile);			// Load inventory and attributes
	void Backtrack();										// Force player to previous position in
	// ..position history
	void ShowFog();							// Activate fog, if used
	void DisableFog();					// Unconditionally disable fog
	void ActivateClipPlane();		// Activate clip plane, if used
	void DisableClipPlane();		// Unconditionally disable clip plane
	bool DoMovements(void);
	void UseItem();
	char *GetDieAnim();
	char *GetInjuryAnim();
	
	void SetCrouch(bool value);
	bool GetCrouch()
	{ return m_crouch; }
	void FlipCrouch()
	{ m_crouch = !m_crouch; }
	void Moving(int nValue)
	{ m_Moving = nValue; }
	int GetSlideWalk()
	{ return m_SlideWalk; }
	void SetSlideWalk(int value)
	{ m_SlideWalk = value; }
	int GetMoving()
	{ return m_Moving; }  
	geFloat Speed()
	{ return m_CurrentSpeed;}					// Get player speed
	geVec3d Position(); 
	geFloat LastMovementSpeed()
	{ return m_LastMovementSpeed;}
	bool InFirstPerson()
	{ return m_FirstPersonView;}
	bool FirstPersonView()
	{ return m_FirstPersonView;}
	// Weapon
	geActor	*GetActor()
	{ return Actor; }
	geFloat GetGravity()
	{ return m_Gravity;}
	int GetViewPoint() 
	{ return m_PlayerViewPoint; }
	EnvironmentSetup *GetEnvSetup()
	{ return EnvSetup; }
	int LastDirection()
	{ return m_lastdirection; }
	bool GetAlive()
	{ return Alive; }
	void SetAlive(bool flag)
	{ Alive = flag; }
	void SetRunning(geBoolean Running)
	{ m_Running = Running; }
	void FlipRunning()
	{ m_Running = !m_Running; }
	void SetJumping(geBoolean Jumping)
	{ m_Jumping = Jumping; }
	char *GetAnimations(int index)
	{ return Animations[index]; }
	bool GetDying()
	{ return Dying; }
	void FlipLight()
	{ lighton = !lighton; }
// changed RF063
	char *GetUseAttribute()
	{ return UseAttribute; }
	void SetUseAttribute(char *Attr)
	{ strcpy(UseAttribute, Attr); }
	int LightValue()
	{ return (int)CurrentLiteLife; }
	int LightLife()
	{ return (int)LiteLife; }
	void ModifyLight(int amount);
	float GetScale()
	{ return m_Scale; }
	float GetHeight()
	{ return m_CurrentHeight; }
	int PlayerViewPoint()
	{ return m_PlayerViewPoint;}
// end change RF063
private:
	void AddPosition();						// Add current position to position history
	geSound_Def *LoadAudioClip(char *szFilename);		// Load audio clip
	void SetJump();
	
	int m_PlayerViewPoint;				// 0=first-person, 1=third-person
	bool m_JumpActive;						// Player is jumping
	bool m_FirstPersonView;				// Player in first-person view
	bool m_LookMode;							// TRUE if in third-person "look mode"
	int m_Moving;									// Non-zero if in third-person and moving
	geBoolean	m_Running;			//Demande de course
	geBoolean	m_Jumping;			//Demande de saut
	geFloat m_JumpSpeed;						// Speed of jump
	geFloat m_CurrentHeight;				// Player current height (mod if crouching)
	geFloat m_CurrentSpeed;					// Player current speed
	geFloat m_CoeffSpeed;
	geFloat m_Speed;								// Player normal speed
	geFloat m_StepHeight;						// Max. step-up height
	geVec3d m_Translation;				// Actor translation
	geActor	*Actor;								// Genesis actor for player
	geFloat m_Gravity;						// Current gravity level
	geFloat m_LastMovementSpeed;		// Velocity of our last motion
	CPersistentAttributes *m_Attr;	// Player attributes
	geVec3d m_PositionHistory[50];	// Last 50 places the player was
	int m_PositionHistoryPtr;			// Pointer into position history array
	EnvAudioSetup *EnvAudio;			// Environmental audio data
	geSound_Def *DefaultMotion[3];			// Default motion sound
	geSound *DefaultMotionHandle;		// Default motion sound handle
	geSound_Def *Contents[16][3];			// Sounds for each 'environment type'
	geSound *ContentsHandles[16];		// Sound handles for each 'environment type'
	geFloat m_Scale;									// Actor scale factor
	bool m_FogActive;								// This player gets fog
	bool m_ClipActive;							// This player gets far-plane clipping

	int m_SlideWalk;
	bool m_crouch;
	bool m_oldcrouch;
	bool m_run;
	float m_HeadBobSpeed;			// eaa3 12/18/2000 Speed of head bob
	float m_HeadBobLimit;			// eaa3 12/18/2000 Max. amount of head bob
	
	int LookFlags;
	geVec3d LookRotation;
	geVec3d LookPosition;
	EnvironmentSetup *EnvSetup;
	int m_lastdirection;
	bool Alive;
	bool RealJumping;
	bool lightactive;
	bool lighton;
	int lighteffect;
	geFloat LiteRadiusMin;
	geFloat LiteRadiusMax;
	GE_RGBA LiteColorMin;
	GE_RGBA LiteColorMax;
	geFloat LiteIntensity;
	geFloat LiteLife;
	geFloat CurrentLiteLife;
	bool DecayLite;
	geFloat LiteDecay;
	geFloat CurrentLiteDecay;
	geFloat LiteTime;
	geFloat LiteDecayTime;
	char LiteBone[64];
	char StaminaName[64];
	geFloat StaminaDelay;
	geFloat StaminaTime;
	bool Allow3rdLook;
	char Animations[ANIMMAX][64];
	int LastHealth;
	bool Injured;
	bool Dying;
	char DieAnim[5][64];
	int DieAnimAmt;
	char InjuryAnim[5][64];
	int InjuryAnimAmt;
	char LastMotion[64];
	geSound_Def *DieSound[5];
	int DieSoundAmt;
	geSound_Def *InjurySound[5];
	int InjurySoundAmt;
	geSound_Def *LandSound[5];
	int LandSoundAmt;
	bool Falling;
	bool OldFalling;
	bool FallInjure;
	geVec3d FallStart;
	float MinFallDist;
	float MaxFallDist;
	float FallDamage;
	GE_RGBA	FillColor;
	GE_RGBA	AmbientColor;
// changed RF063
	float UseRange;
	char UseAttribute[64];
	float BoxWidth;
	float LiquidTime;
	int InLiquid;
	int SwimSound;
	int SurfaceSound;
	int InLiquidSound;
// end change RF063
};

#endif
