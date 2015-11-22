/*
CPlayer.h:		Player character encapsulation class

  (c) 1999 Edward A. Averill, III
  
	This file contains the class declaration for the player
	character (avatar) in an RGF-based game.
*/

#ifndef __RGF_CPLAYER_H__
#define __RGF_CPLAYER_H__

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
	
	void SetCrouch(bool value);
	bool GetCrouch()
	{ return m_crouch; }
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
	void SetJumping(geBoolean Jumping)
	{ m_Jumping = Jumping; }
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
	geSound_Def *DefaultMotion;			// Default motion sound
	geSound *DefaultMotionHandle;		// Default motion sound handle
	geSound_Def *Contents[16];			// Sounds for each 'environment type'
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
	int lighteffect;
	geFloat LiteRadiusMin;
	geFloat LiteRadiusMax;
	GE_RGBA LiteColorMin;
	GE_RGBA LiteColorMax;
	geFloat LiteIntensity;
	geFloat StaminaDelay;
	geFloat StaminaTime;
	bool Allow3rdLook;

};

#endif
