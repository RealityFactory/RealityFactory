/*
CCommonData.h:		Global Data Pool class

  (c) 2001 Ralph Deane
  All Rights Reserved
  
	This file contains the class declaration for the Global Data
	Pool handling class.
*/

#ifndef __RGF_CCOMMONDATA_H_
#define __RGF_CCOMMONDATA_H_

class CCommonData
{
public:
	CCommonData();
	~CCommonData();
	int InitializeCommon(HINSTANCE hInstance, char *szStartLevel, bool CommandLine);
	// Initialize common components
	void ShutdownCommon();						// Shut down common components
	int InitializeLevel(char *szLevelName);	// Load and initialize level
	void ShutdownLevel();							// Clean up & shutdown level
	bool HandleGameInput();						// Process MENU commands, etc.
	bool HandleMenuInput();						// Process keystrokes in GAME LOOP
// changed RF063
	int SaveTo(FILE *SaveFD);
	int RestoreFrom(FILE *RestoreFD);
	void Play(char *szFile, int XPos, int YPos, bool Center);
// end change RF063
	int DispatchTick();								// Send time tick to components
	bool ProcessLevelChange();				// Process a level change
	void RenderComponents();					// Render all components
	void PlayOpeningCutScene();				// Play opening cut scene
	void CheckMediaPlayers();					// Poll all media players
	FILE *OpenRFFile(int nFileType, char *szFilename, char *szHow);	
	// Open RealityFactory file
	bool OpenRFFile(geVFile **theFP, int nFileType, char *szFilename, int nHow);	
	// Use geFile for RF file opening
	char *GetDirectory(int nDirectoryType);	// Get configured directory
	DWORD GetTimePassed_D();					// Get amount of time passed, in msec
	geFloat GetTimePassed_F();					// Get amount of time passed, in msec
	DWORD FreeRunningCounter();				// Return system free-running counter
	geFloat FreeRunningCounter_F();			// Same as aboe, only floating-point
	void ResetClock();								// Reset the internal timer
	void Spin(DWORD dwMilliseconds);	// Waste time
	void SetChangeLevelData(struct _ChangeLevel *pItem);	// Set change level data
	void SetLevelData();
	//	Inline data accessor member functions
	inline bool ReportError(char *szMsg, bool *bBox)
	{ if(theGameEngine == NULL)
	{ OutputDebugString(szMsg); OutputDebugString("\n"); return false;}
	else  return theGameEngine->ReportError(szMsg, bBox);}
	inline CGenesisEngine *Engine() { return theGameEngine;}
	inline geWorld *World() { return theGameEngine->World();}
	inline CInput *Input() { return theUserInput;}
	inline CPlayer *Player() { return thePlayer;}
	inline CAutoDoors *Doors() { return theAutoDoors;}
	inline CMovingPlatforms *Platforms() { return thePlatforms;}
	inline CTeleporter *Teleporters() { return theTeleports;}
	inline CMorphingFields *MorphingFields() { return theFields;}
	inline CCDAudio *CDPlayer() { return theCDPlayer;}
	inline CMIDIAudio *MIDIPlayer() { return theMIDIPlayer;}
	inline C3DAudioSource *Audio3D() { return the3DAudio;}
	inline CParticleSystem *Particles() { return theParticles;}
	inline CStaticEntity *Props() { return theProps;}
	inline CSoundtrackToggle *SoundtrackToggles() { return theSTToggles;}
	inline CAudioStream *AudioStreams() { return theStreams;}
	inline CVideoTexture *VideoTextures() { return theVidText;}
	inline CCorona *Coronas() { return theCorona;}
	inline CDynalite *DynamicLights() { return theDynalite;}
	inline CElectric *ElectricEffects() { return theElectric;}
	inline CProcedural *ProceduralTextures() { return theProcTexture;}
	inline CHeadsUpDisplay *HUD() { return theHUD;}
	inline CPathDatabase *PathDatabase() { return thePathDatabase;}
	inline CEntityRegistry *EntityRegistry() { return theRegistry;}
	inline CPathFollower *PathFollower() { return theFollower;}
	inline EffManager *EffectManager() { return theEffect; }
	inline CRain *RainEffect() { return theRain; }
	inline CSpout *SpoutEffect() { return theSpout; }
	inline CFloat *FloatEffect() { return theFloat; }
	inline Chaos *ChaosEffect() { return theChaos;}
	inline CFlame *FlameEffect() { return theFlame; }
	inline CRFMenu *MenuManager() { return theMenu; }
	inline CAttribute *Attributes() { return theAttribute; }
	inline CNPC *NPCManager() { return theNPC;}
#ifdef RF063
	inline CEnemy *EnemyManager() { return theEnemy;}
#endif
	inline Collider *Collision() { return theCollider;}
	inline CActorManager *ActorManager() { return theActorManager;}
	inline CModelManager *ModelManager() { return theModelManager;}
	inline CAudioManager *AudioManager() { return theAudioManager;}
	inline CCameraManager *CameraManager() { return theCameraManager;}
	inline CTriggers *Triggers() { return theTriggers;}
	inline CLogic *Logic() { return theLogic;}
	inline CDamage *Damage() { return theDamage;}
	inline CTrack *Track() { return theTrack;}
#ifdef RF063
	inline CTrackPoint *TrackPoints() { return theTrackPoints;}
	inline CTrackStart *TrackStarts() { return theTrackStarts;}
#endif
	inline CNPCPathPoint *NPCPoint() { return theNPCPoint;}
	inline bool InGameMode() { return m_InGameLoop;}
	// Time inlines
	inline DWORD LastElapsedTime_D() { return LastTimePassed_D;}
	inline geFloat LastElapsedTime_F() { return LastTimePassed_F;}
	inline __int64 CurrentGameTime_L() { return LastTimePoll;}
	inline DWORD CurrentGameTime_D() { return (DWORD)LastTimePoll;}
	inline geFloat CurrentGameTime_F() { return (float)LastTimePoll;}
	//	Game state inlines
	inline void GoToGameMode() { m_InGameLoop = true;}
	inline void GoToMenuMode() { m_InGameLoop = false;}
	inline int SetDebugLevel(int theLevel) 
	{ int nOldLevel = m_DebugLevel; m_DebugLevel = theLevel;
		  return nOldLevel;}
	inline int DebugLevel() { return m_DebugLevel;}
	inline char *MenuIni() { return m_MenuDirectory; } // Update #1
	inline bool HeadBob() {return m_headbob;}
	inline bool WeaponPosition() {return m_weaponposition;}
	inline void SetChangeLevel(bool fHit)
	{ m_ChangeLevel = fHit;}				// Control change level flag
	inline bool ChangeLevel() { return m_ChangeLevel;}
	inline CMessage *Messages() { return theMessage;}
	inline CWeapon *Weapons() { return theWeapon;}
	inline CFirePoint *FirePoints() { return theFirePoint;}
	inline CDecal *Decals() { return theDecal;}
	inline CWallDecal *WallDecals() { return theWallDecal;}
	inline CFlipBook *FlipBooks() { return theFlipBook;}
	inline CExplosionInit *Explosions() { return theExplosion;}
	inline CExplosion *CExplosions() { return theCExplosion;}
	inline CPreEffect *Effect() { return thePreEffect;}
	inline CChangeLevel *Changelevel() { return theChangeLevel;}
	inline CShake *SShake() { return theShake;}
	inline CFixedCamera *FixedCameras() { return theFixedCamera;}
	inline geVec3d PRotation() { return m_playerotation;}
	inline geVec3d CRotation() { return theRotation;}
	inline geVec3d POffset() { return Offset;}
	inline char *StartName() { return m_StartPointName;}
	inline char *SplashScreen() { return m_SplashScreen;}
	inline char *SplashAudio() { return m_SplashAudio;}
	inline char *CutScene() { return m_CutScene;}
	inline char *SplashScreen1() { return m_SplashScreen1;}
	inline char *SplashAudio1() { return m_SplashAudio1;}
	inline char *CutScene1() { return m_CutScene1;}
// changed RF063
	inline bool GetCSelect() { return CSelect;}
	inline bool GetSaving() { return saving;}
	inline void SetSaving(bool flag) { saving = flag;}
	inline CViewSwitch *ViewSwitchs() { return theViewSwitch;}
	inline CInventory *Inventory() { return theInventory;}
	inline CLiquid *Liquids() { return theLiquid;}
// end change RF063
	inline bool GetPaused() { return Paused;}
	inline void SetPaused(bool flag) { Paused = flag;}
	inline GE_RGBA GetFogColor() { return cColor;}
	inline bool GetUseEffect() { return UseEffect;}
	inline char *NextLevel() { return m_NewLevel; }
	inline void SetLevelDirectory(char *szArg) 
		{ if(szArg != NULL)
			strcpy(m_LevelDirectory, szArg);
		}
private:
	CGenesisEngine *theGameEngine;		// Genesis engine class
	CInput *theUserInput;							// User input class
	CPlayer *thePlayer;								// Player avatar class
	CAutoDoors *theAutoDoors;					// Automatic doors class
	CMovingPlatforms *thePlatforms;		// Moving platforms class
	CTeleporter *theTeleports;				// Teleporter class
	CMorphingFields *theFields;				// Morphing fields class
	CCDAudio *theCDPlayer;						// CD Audio player class
	CMIDIAudio *theMIDIPlayer;				// MIDI Audio player class
	C3DAudioSource *the3DAudio;				// 3D Audio Source class
	CParticleSystem *theParticles;		// Particle systems handler class
	CStaticEntity *theProps;					// Static entity handler class
	CSoundtrackToggle *theSTToggles;	// Soundtrack toggle handler class
	CAudioStream *theStreams;					// Streaming audio handler
	CVideoTexture *theVidText;				// Video texture handler
	CCorona *theCorona;								// Ralph Deane's corona effects class
	CDynalite *theDynalite;						// Ralph Deane's dynamic light class
	CElectric *theElectric;						// Ralph Deane's electric bolt class
	CProcedural *theProcTexture;			// Ralph Deane's procedural texture class
	CHeadsUpDisplay *theHUD;					// Heads-up display class
	CPathDatabase *thePathDatabase;		// Path database
	CPathFollower *theFollower;				// Path follower class
	CEntityRegistry *theRegistry;			// Entity registry class
	EffManager *theEffect;						// Ralph Deane's Effects Manager
	CRain *theRain;										// Ralph Deane's Rain Effect
	CSpout *theSpout;									// Ralph Deane's Spout Effect
	CFloat *theFloat;									// Ralph Deane's Floating Effect
	CFlame *theFlame;									// Ralph Deane's Flame Effect
	Chaos *theChaos;									// Ralph Deane's Chaos Procedural
	CRFMenu *theMenu;										// Ralph Deane's Menu Manager
	CNPC *theNPC;											// Non-Player Character Manager
#ifdef RF063
	CEnemy *theEnemy;
#endif
	Collider *theCollider;						// Collision detection subsystem
	CActorManager *theActorManager;		// Actor manager subsystem
	CModelManager *theModelManager;		// Model manager subsystem
	CAudioManager *theAudioManager;		// Audio manager subsystem
	CCameraManager *theCameraManager;	// Camera manager subsystem
	CTriggers *theTriggers;						// Ralph Deane's Triggers class
	CLogic *theLogic;									// Ralph Deane's Trigger Logic
	CMessage *theMessage;
	CWeapon  *theWeapon;
	CFirePoint  *theFirePoint;
	CFlipBook  *theFlipBook;
	CDecal  *theDecal;
	CWallDecal  *theWallDecal;
	CAttribute  *theAttribute;
	CDamage  *theDamage;
	CExplosionInit  *theExplosion;
	CExplosion  *theCExplosion;
	CPreEffect  *thePreEffect;
	CChangeLevel *theChangeLevel;
	CShake *theShake;
	CFixedCamera *theFixedCamera;
// changed RF063
	CViewSwitch *theViewSwitch;
	CInventory * theInventory;
	CLiquid * theLiquid;
// end change RF063
	CTrack *theTrack;
#ifdef RF063
	CTrackPoint *theTrackPoints;
	CTrackStart *theTrackStarts;
#endif
	CNPCPathPoint *theNPCPoint;
	// Timekeeping information
	__int64 PerformanceFrequency;			// Performance Counter Frequency
	__int64 LastTimePoll;							// Last time we polled the time
	DWORD LastTimePassed_D;						// Last elapsed time as DWORD
	geFloat LastTimePassed_F;						// Last elapsed time as FLOAT
	//	Global game state information
	bool m_InGameLoop;								// Game loop/menu loop flag
	DWORD m_TotalPlayTime;						// Total game time, this session
	int m_DebugLevel;									// Internal debug output level
	//	Level change information
	bool m_ChangeLevel;								// We hit a level change
	char m_NewLevel[256];					// New level filename from changelevel
	char m_SplashScreen[256];			// Splash screen filename from changelevel
	char m_SplashAudio[256];			// Audio to play with splash screen
	char m_CutScene[256];				// Cut scene to play from changelevel
	char m_SplashScreen1[256];			// Splash screen filename from changelevel
	char m_SplashAudio1[256];			// Audio to play with splash screen
	char m_CutScene1[256];				// Cut scene to play from changelevel
	char m_Message[256];
	int m_Font;
	geVec3d m_playerotation;
	geVec3d theRotation;
	geVec3d Offset;
	char m_StartPointName[256];
	bool KeepAttributes;
	float m_defaultdistance, m_cameraX, m_cameraY;
	geVec3d theTranslation, theRotate;
	int ViewPoint;
    int Slot[10];
    int CurrentWeapon;
	bool Paused;
	bool UseEffect;
	GE_RGBA cColor;
	bool UseAngle;
	bool ShowTrack;
	//	Configuration data
	char m_PlayerAvatar[256];					// Player avatar actor file
	char m_ActorDirectory[256];				// Actor directory
	char m_BitmapDirectory[256];			// Bitmap directory
	char m_LevelDirectory[256];				// Level directory
	char m_AudioDirectory[256];				// Audio directory
	char m_VideoDirectory[256];				// Video directory
	char m_AudioStreamDirectory[256];	// Audio stream directory
	char m_MIDIDirectory[256];				// MIDI file directory
	char m_MenuDirectory[256];				// menu ini file directory
	char m_VirtualFile[256];
// changed RF063
	bool CSelect;
// end change RF063
	bool m_headbob;
	bool m_weaponposition;
	bool jumpkey;
	bool runkey;
	bool crouchkey;
	bool zoomkey;
	bool lightkey;
// changed RF063
	bool loadkey;
	bool savekey;
	bool saving;
	bool usekey;
	bool invkey;
// end change RF063
	geVFile *VFS;
};

#endif

// --------------------------------------------------------------------------
