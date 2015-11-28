/****************************************************************************************/
/*																						*/
/*	CCommonData.h:		Global Data Pool class											*/
/*																						*/
/*	(c) 2001 Ralph Deane																*/
/*	All Rights Reserved																	*/
/*																						*/
/*	This file contains the class declaration for the Global Data						*/
/*	Pool handling class.																*/
/*																						*/
/****************************************************************************************/


#ifndef __RGF_CCOMMONDATA_H_
#define __RGF_CCOMMONDATA_H_

#define CONSOLEMAXROWS 30
#define CONSOLEMAXCOLS 80


// start change scripting
#include "hashtable\\hash_table.h"

typedef CHashTable<long> CLongHashT;
// end change scripting


class CCommonData
{
public:
	CCommonData();
	~CCommonData();

	// Initialize common components
	int InitializeCommon(HINSTANCE hInstance, char *szStartLevel, bool CommandLine);
	void ShutdownCommon();						// Shut down common components

	int InitializeLevel(char *szLevelName);		// Load and initialize level
	void ShutdownLevel();						// Clean up & shutdown level

	bool HandleGameInput();						// Process MENU commands, etc.
	bool HandleMenuInput();						// Process keystrokes in GAME LOOP
// changed RF063
	int SaveTo(FILE *SaveFD);
	int RestoreFrom(FILE *RestoreFD);
	void Play(char *szFile, int XPos, int YPos, bool Center);
// end change RF063
	int DispatchTick();							// Send time tick to components
	bool ProcessLevelChange();					// Process a level change
	void RenderComponents();					// Render all components

	// begin change gekido
	// new console commands
	void ConsoleInit (int nMaxRows);			// initialize console
	bool ConsoleRender();						// renders the current buffer to screen
	bool ConsolePrint(char *szMsg, bool *bBox);	// print to console
	bool ShowConsole (bool bConsoleFlag, int nTransition);	// show/hide console param1 - show/hide, param 2 - whether to use a slide in/out transition when showing the console
	bool ConsoleExec(char *szMsg, bool *bBox);	// execute console command
	bool ConsoleClear();						// clears console
	// end change gekido

	// start pickles Jul 04
	void InitJoysticks();
	void CloseJoysticks();
	int	 PollJoystickAxis(int jn, int a);
	bool CheckJoystickButton(int jn, int bn);
	int  GetNumJoys();
	// end pickles Jul 04

	skExecutableContext GetskContext();							// change simkin
	void AddScriptedObject(char *objectName, skRValue Object);	// change simkin
	void RemoveScriptedObject(char *objectName);				// change simkin

	void FillHashCommands_LowLevel(void);				// change scripting
	long GetHashCommand(std::string key);				// change scripting
	bool AddHashCommand(std::string key,long value);	// change scripting
	void ClearHashCommands_LowLevel(void);				// changed QD 08/15/06

	void PlayOpeningCutScene();					// Play opening cut scene
	void CheckMediaPlayers();					// Poll all media players

	// Open RealityFactory file
	FILE *OpenRFFile(int nFileType, char *szFilename, char *szHow);
	// Use geVFile for RF file opening
	bool OpenRFFile(geVFile **theFP, int nFileType, char *szFilename, int nHow);
	bool FileExist(int nFileType, char *szFilename);
	char *GetDirectory(int nDirectoryType);		// Get configured directory

	DWORD GetTimePassed_D();					// Get amount of time passed, in msec
	geFloat GetTimePassed_F();					// Get amount of time passed, in msec
	DWORD FreeRunningCounter();					// Return system free-running counter
	geFloat FreeRunningCounter_F();				// Same as aboe, only floating-point
	void ResetClock();							// Reset the internal timer
	void Spin(DWORD dwMilliseconds);			// Waste time

	void SetChangeLevelData(struct _ChangeLevel *pItem);	// Set change level data
	void SetLevelData();

	// Inline data accessor member functions
	inline bool ReportError(char *szMsg, bool bBox) // changed QD 12/15/05
	{
		if(theGameEngine == NULL)
		{
			OutputDebugString(szMsg);
			OutputDebugString("\n");
			return false;
		}
		else
			return theGameEngine->ReportError(szMsg, bBox);
	}

	inline CGenesisEngine *Engine()				{ return theGameEngine;			}
// start multiplayer
	inline NetPlayerMgr *NetPlayerManager()		{ return theNetPlayerMgr;		}
// end multiplayer
	inline geWorld *World()						{ return theGameEngine->World();}
	inline CInput *Input()						{ return theUserInput;			}
	inline CPlayer *Player()					{ return thePlayer;				}
	inline CAutoDoors *Doors()					{ return theAutoDoors;			}
	inline CMovingPlatforms *Platforms()		{ return thePlatforms;			}
	inline CTeleporter *Teleporters()			{ return theTeleports;			}
	inline CMorphingFields *MorphingFields()	{ return theFields;				}
	inline CCDAudio *CDPlayer()					{ return theCDPlayer;			}
	inline CMIDIAudio *MIDIPlayer()				{ return theMIDIPlayer;			}
	inline C3DAudioSource *Audio3D()			{ return the3DAudio;			}
	inline CParticleSystem *Particles()			{ return theParticles;			}
	inline CStaticEntity *Props()				{ return theProps;				}
	// changed QD 01/2004
	inline CStaticMesh *Meshes()				{ return theMeshes;				}
	// end change
	inline CSoundtrackToggle *SoundtrackToggles() { return theSTToggles;		}
	inline CAudioStream *AudioStreams()			{ return theStreams;			}
	inline CVideoTexture *VideoTextures()		{ return theVidText;			}
	inline CCorona *Coronas()					{ return theCorona;				}
	inline CDynalite *DynamicLights()			{ return theDynalite;			}
	inline CElectric *ElectricEffects()			{ return theElectric;			}
	inline CProcedural *ProceduralTextures()	{ return theProcTexture;		}
	inline CHeadsUpDisplay *HUD()				{ return theHUD;				}
	inline CPathDatabase *PathDatabase()		{ return thePathDatabase;		}
	inline CEntityRegistry *EntityRegistry()	{ return theRegistry;			}
	inline CPathFollower *PathFollower()		{ return theFollower;			}
	inline EffManager *EffectManager()			{ return theEffect;				}
	inline CRain *RainEffect()					{ return theRain;				}
	inline CSpout *SpoutEffect()				{ return theSpout;				}
// changed RF064
	inline CActorSpout *ActorSpoutEffect()		{ return theActorSpout;			}
	inline CMorph * Morphs()					{ return theMorph;				}
	inline CCutScene *CutScenes()				{ return theCutScene;			}
	inline CActMaterial *ActMaterials()			{ return theActMaterial;		}
	inline qxTerrainMgr *TerrainMgr()			{ return theTerrainMgr;			}
	inline CArmour *Armours()					{ return theArmour;				}
	inline CLiftBelt *LiftBelts()				{ return theLiftBelt;			}
// end change RF064
	inline CFloat *FloatEffect()				{ return theFloat;				}
	inline Chaos *ChaosEffect()					{ return theChaos;				}
	inline CFlame *FlameEffect()				{ return theFlame;				}
	inline CRFMenu *MenuManager()				{ return theMenu;				}
	inline CAttribute *Attributes()				{ return theAttribute;			}
	inline Collider *Collision()				{ return theCollider;			}
	inline CActorManager *ActorManager()		{ return theActorManager;		}
	inline CModelManager *ModelManager()		{ return theModelManager;		}
	inline CAudioManager *AudioManager()		{ return theAudioManager;		}
	inline CCameraManager *CameraManager()		{ return theCameraManager;		}
	inline CTriggers *Triggers()				{ return theTriggers;			}
	inline CLogic *Logic()						{ return theLogic;				}
	inline CDamage *Damage()					{ return theDamage;				}
// changed RF064
	inline CScriptPoint *ScriptPoints()			{ return theScriptPoints;		}
	inline CPawn *Pawns()						{ return thePawn;				}
	inline CCountDown *CountDownTimers()		{ return theCountDownTimer;		}
	inline CChangeAttribute *ChangeAttributes() { return theChangeAttribute;	}
// end change RF064
	inline bool InGameMode()					{ return m_InGameLoop;			}
	// Time inlines
	inline DWORD LastElapsedTime_D()			{ return LastTimePassed_D;		}
	inline geFloat LastElapsedTime_F()			{ return LastTimePassed_F;		}
	inline __int64 CurrentGameTime_L()			{ return LastTimePoll;			}
	inline DWORD CurrentGameTime_D()			{ return (DWORD)LastTimePoll;	}
	inline geFloat CurrentGameTime_F()			{ return (float)LastTimePoll;	}
	//	Game state inlines
	inline void GoToGameMode()					{ m_InGameLoop = true;			}
	inline void GoToMenuMode()					{ m_InGameLoop = false;			}
	inline int SetDebugLevel(int theLevel)
	{ int nOldLevel = m_DebugLevel; m_DebugLevel = theLevel; return nOldLevel;	}
	inline int DebugLevel()						{ return m_DebugLevel;			}
	// Update #1
	inline char *MenuIni()						{ return m_MenuDirectory;		}
	inline bool HeadBob()						{ return m_headbob;				}
	inline bool WeaponPosition()				{ return m_weaponposition;		}
	// Control change level flag
	inline void SetChangeLevel(bool fHit)		{ m_ChangeLevel = fHit;			}
	inline bool ChangeLevel()					{ return m_ChangeLevel;			}
	inline CMessage *Messages()					{ return theMessage;			}
	inline CWeapon *Weapons()					{ return theWeapon;				}
	inline CFirePoint *FirePoints()				{ return theFirePoint;			}
	inline CDecal *Decals()						{ return theDecal;				}
	inline CWallDecal *WallDecals()				{ return theWallDecal;			}
   //Start Aug2003DCS
	inline CLevelController *LevelControllers() { return theLevelController;	}
   //End Aug2003DCS
	inline CFlipBook *FlipBooks()				{ return theFlipBook;			}
	// start Pickles Jul 04
	inline CFoliage *Foliage()					{ return theFoliage;			}
	inline CFlipTree *FlipTree()				{ return theFlipTree;			}
	// end Pickles Jul 04
	// start Pickles PWX
	inline PWXImageManager *PWXImMgr()			{ return thePWXImage;			}
	inline CPolyShadow *PlyShdw()				{ return thePolyShadow;			}
	inline CAreaChecker *AreaCheck()			{ return theAreaCheck;			}
	// end Pickles PWX
	inline CExplosionInit *Explosions()			{ return theExplosion;			}
	inline CExplosion *CExplosions()			{ return theCExplosion;			}
	inline CPreEffect *Effect()					{ return thePreEffect;			}
	inline CChangeLevel *Changelevel()			{ return theChangeLevel;		}
	inline CShake *SShake()						{ return theShake;				}
	inline CFixedCamera *FixedCameras()			{ return theFixedCamera;		}
	inline geVec3d PRotation()					{ return m_playerotation;		}
	inline geVec3d CRotation()					{ return theRotation;			}
	inline geVec3d POffset()					{ return Offset;				}
	inline char *StartName()					{ return m_StartPointName;		}
	inline char *SplashScreen()					{ return m_SplashScreen;		}
	inline char *SplashAudio()					{ return m_SplashAudio;			}
	inline char *CutScene()						{ return m_CutScene;			}
	inline char *SplashScreen1()				{ return m_SplashScreen1;		}
	inline char *SplashAudio1()					{ return m_SplashAudio1;		}
	inline char *CutScene1()					{ return m_CutScene1;			}
// changed QD 12/15/05
	inline bool GetNSelect()					{ return NSelect;				}
// end change
// changed RF063
	inline bool GetCSelect()					{ return CSelect;				}
	inline bool GetSaving()						{ return saving;				}
	inline void SetSaving(bool flag)			{ saving = flag;				}
	inline CViewSwitch *ViewSwitchs()			{ return theViewSwitch;			}
	inline CInventory *Inventory()				{ return theInventory;			}
	inline CLiquid *Liquids()					{ return theLiquid;				}
	inline CDSpotLight *CDSpot()				{ return theCDSpot;				}
	inline bool GetLogging()					{ return Logging;				}
// end change RF063
	inline bool GetPaused()						{ return Paused;				}
	inline void SetPaused(bool flag)			{ Paused = flag;				}
	inline float GetAudibleRadius()				{ return kAudibleRadius;		}
	inline void SetAudibleRadius(float radius)	{ kAudibleRadius = radius;		}
	inline float GetTicksGoneBy()				{ return dwTicksGoneBy;			}
// changed RF064
	inline COverlay *Overlays()					{ return theOverlay;			}
	inline void SetKeyPaused(bool flag)			{ KeyPaused = flag;				}
	inline bool GetCDifficult()					{ return CDifficult;			}
	inline int GetDifficultLevel()				{ return DifficultLevel;		}
	inline void SetDifficultLevel(int level)	{ DifficultLevel = level;		}
	inline bool GetCmdLine()					{ return CmdLine;				}
// end change RF064
	inline GE_RGBA GetFogColor()				{ return cColor;				}
	inline bool GetUseEffect()					{ return UseEffect;				}
	inline char *NextLevel()					{ return m_NewLevel;			}
	inline void SetLevelDirectory(char *szArg)
						{ if(szArg != NULL)	strcpy(m_LevelDirectory, szArg);	}
	inline bool GetHasFocus()					{ return HasFocus;				}
	inline void SetHasFocus(bool flag)			{ HasFocus = flag;				}
// start multiplayer
	inline bool GetConsole()					{ return consoleflag;			}
	inline bool GetNetwork()					{ return network;				}
	void ShutDownNetWork();
	bool GetMultiPlayer();
	void SetMultiPlayer(bool multi, bool Server);
	inline bool GetServer()						{ return server;				}
// end multiplayer
	inline int GetLanguage()					{ return m_Language;			}
	inline void SetLanguage(int language)		{ m_Language = language;		}
	inline float GetLODdistance(int index)		{ return LODdistance[index];	}
	inline bool GetLODAnimation()				{ return LODAnimation;			}
// changed Nout 12/15/05
	inline bool GetMouseControl()				{ return m_MouseControl;		}
	inline void SetMouseControl(bool flag)		{ m_MouseControl = flag;		}
// end change
// changed QD 12/15/05
	inline bool GetUseDialog()					{ return UseDialog;				}
	inline char *GetDefaultPlayerName()			{ return m_PlayerName;			}
// end change

private:
	CGenesisEngine		*theGameEngine;		// Genesis engine class
// start multiplayer
	NetPlayerMgr		*theNetPlayerMgr;
// end multiplayer
	CInput				*theUserInput;		// User input class
	CPlayer				*thePlayer;			// Player avatar class
	CAutoDoors			*theAutoDoors;		// Automatic doors class
	CMovingPlatforms	*thePlatforms;		// Moving platforms class
	CTeleporter			*theTeleports;		// Teleporter class
	CMorphingFields		*theFields;			// Morphing fields class
	CCDAudio			*theCDPlayer;		// CD Audio player class
	CMIDIAudio			*theMIDIPlayer;		// MIDI Audio player class
	C3DAudioSource		*the3DAudio;		// 3D Audio Source class
	CParticleSystem		*theParticles;		// Particle systems handler class
	CStaticEntity		*theProps;			// Static entity handler class
	// changed QD 01/2004
	CStaticMesh			*theMeshes;			// new staticmesh system
	// end change
	CSoundtrackToggle	*theSTToggles;		// Soundtrack toggle handler class
	CAudioStream		*theStreams;		// Streaming audio handler
	CVideoTexture		*theVidText;		// Video texture handler
	CCorona				*theCorona;			// Ralph Deane's corona effects class
	CDynalite			*theDynalite;		// Ralph Deane's dynamic light class
	CElectric			*theElectric;		// Ralph Deane's electric bolt class
	CProcedural			*theProcTexture;	// Ralph Deane's procedural texture class
	CHeadsUpDisplay		*theHUD;			// Heads-up display class
	CPathDatabase		*thePathDatabase;	// Path database
	CPathFollower		*theFollower;		// Path follower class
	CEntityRegistry		*theRegistry;		// Entity registry class
	EffManager			*theEffect;			// Ralph Deane's Effects Manager
	CRain				*theRain;			// Ralph Deane's Rain Effect
	CSpout				*theSpout;			// Ralph Deane's Spout Effect
// changed RF064
	CActorSpout			*theActorSpout;
// end change RF064
	CFloat				*theFloat;			// Ralph Deane's Floating Effect
	CFlame				*theFlame;			// Ralph Deane's Flame Effect
	Chaos				*theChaos;			// Ralph Deane's Chaos Procedural
	CRFMenu				*theMenu;			// Ralph Deane's Menu Manager
	Collider			*theCollider;		// Collision detection subsystem
	CActorManager		*theActorManager;	// Actor manager subsystem
	CModelManager		*theModelManager;	// Model manager subsystem
	CAudioManager		*theAudioManager;	// Audio manager subsystem
	CCameraManager		*theCameraManager;	// Camera manager subsystem
	CTriggers			*theTriggers;		// Ralph Deane's Triggers class
	CLogic				*theLogic;			// Ralph Deane's Trigger Logic
	CMessage			*theMessage;
	CWeapon				*theWeapon;
	CFirePoint			*theFirePoint;
	CFlipBook			*theFlipBook;
	CFoliage			*theFoliage;		//Pickles Jul 04
	CFlipTree			*theFlipTree;		//Pickles Jul 04
	PWXImageManager		*thePWXImage;		//PWX
	CPolyShadow			*thePolyShadow;		//PWX
	CAreaChecker		*theAreaCheck;		//PWX
	CDecal				*theDecal;
	CWallDecal			*theWallDecal;
	//Start Aug2003DCS
	CLevelController	*theLevelController;
   //End Aug2003DCS
	CAttribute			*theAttribute;
	CDamage				*theDamage;
	CExplosionInit		*theExplosion;
	CExplosion			*theCExplosion;
	CPreEffect			*thePreEffect;
	CChangeLevel		*theChangeLevel;
	CShake				*theShake;
	CFixedCamera		*theFixedCamera;
// changed RF063
	CViewSwitch			*theViewSwitch;
	CInventory			*theInventory;
	CLiquid				*theLiquid;
// end change RF063
// changed RF064
	COverlay			*theOverlay;
	CScriptPoint		*theScriptPoints;
	CPawn				*thePawn;
	CCountDown			*theCountDownTimer;
	CChangeAttribute	*theChangeAttribute;
	CMorph				*theMorph;
	CCutScene			*theCutScene;
	CActMaterial		*theActMaterial;
	qxTerrainMgr		*theTerrainMgr;
	CArmour				*theArmour;
	CLiftBelt			*theLiftBelt;
	CDSpotLight			*theCDSpot;
	Joystick			*joysticks[4];		// pickles Jul 04
	float kAudibleRadius;
// end change RF064
	// Timekeeping information
	__int64		PerformanceFrequency;		// Performance Counter Frequency
	__int64		LastTimePoll;				// Last time we polled the time
	DWORD		LastTimePassed_D;			// Last elapsed time as DWORD
	geFloat		LastTimePassed_F;			// Last elapsed time as FLOAT
	//	Global game state information
	bool		m_InGameLoop;				// Game loop/menu loop flag
	DWORD		m_TotalPlayTime;			// Total game time, this session
	int			m_DebugLevel;				// Internal debug output level
	//	Level change information
	bool		m_ChangeLevel;				// We hit a level change
	char		m_NewLevel[256];			// New level filename from changelevel
	char		m_SplashScreen[256];		// Splash screen filename from changelevel
	char		m_SplashAudio[256];			// Audio to play with splash screen
	char		m_CutScene[256];			// Cut scene to play from changelevel
	char		m_SplashScreen1[256];		// Splash screen filename from changelevel
	char		m_SplashAudio1[256];		// Audio to play with splash screen
	char		m_CutScene1[256];			// Cut scene to play from changelevel
	char		m_Message[256];
	int			m_Font;
// changed Nout 12/15/05
	bool		m_MouseControl;				// use mouse to control player
// end change
	geVec3d		m_playerotation;
	geVec3d		theRotation;
	geVec3d		Offset;
	char		m_StartPointName[256];
	bool		KeepAttributes;
	bool		SplashHold;
	float		m_defaultdistance, m_cameraX, m_cameraY;
	geVec3d		theTranslation, theRotate;
	int			ViewPoint;
    // changed QD 12/15/05 MAX_WEAPONS = 40
	int			Slot[40];
    int			CurrentWeapon;
	bool		Paused;
// changed RF064
	bool		KeyPaused;
	bool		CDifficult;
	int			DifficultLevel;
	geFloat		dwTicksGoneBy;
// end change RF064
	bool		UseEffect;
	GE_RGBA		cColor;
	bool		UseAngle;
	bool		ShowTrack;
	//	Configuration data
	char	m_PlayerAvatar[256];			// Player avatar actor file
	char	m_ActorDirectory[256];			// Actor directory
	char	m_BitmapDirectory[256];			// Bitmap directory
	char	m_LevelDirectory[256];			// Level directory
	char	m_AudioDirectory[256];			// Audio directory
	char	m_VideoDirectory[256];			// Video directory
	char	m_AudioStreamDirectory[256];	// Audio stream directory
	char	m_MIDIDirectory[256];			// MIDI file directory
	char	m_MenuDirectory[256];			// menu ini file directory
	char	m_VirtualFile[256];
// changed QD 12/15/05
	char	m_PlayerName[64];
	bool	NSelect;
// end change
// changed RF063
	bool	CSelect;
// end change RF063
// changed QD 12/15/05
	bool	UseDialog;
// end change
// changed QD 07/15/06 - dinput may conflict with hid.dll on win98, provide flag to disable
	bool	m_bUseDInput;
// end change
	bool	m_headbob;
	bool	m_weaponposition;
	bool	jumpkey;
	bool	runkey;
	bool	crouchkey;
	bool	zoomkey;
	bool	lightkey;
// changed RF063
	bool	loadkey;
	bool	savekey;
	bool	saving;
	bool	usekey;
	bool	invkey;
// end change RF063
// changed RF064
	bool	dropkey;
	bool	reloadkey;
// end change Rf064
	geVFile *VFS;
// start multiplayer
	bool	consolekey;
	bool	consoleflag;
	bool	network;
	bool	multiplayer;
	bool	server;
// end multiplayer
// begin change gekido 02.17.2004
// start console vars
	// current contents of the console
	char	ConsoleBuffer[CONSOLEMAXROWS][CONSOLEMAXCOLS];
	int		nCurrentRow;	// what row of the console we're on
// end console vars
// end change gekido
	//change scripting
	CLongHashT CommandHash;
// changed RF064
	static void CALLBACK TimerFunction(UINT uID, UINT uMsg, DWORD dwUser,
	                 DWORD dw1, DWORD dw2);		// Static timer callback
	int		m_nTimerID;
	__int64 TimeCounter;
	bool	HasFocus;
	bool	CmdLine;
// end change RF064
	int		m_Language;
	bool	Logging;
	float	LODdistance[5];
	bool	LODAnimation;
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
