/************************************************************************************//**
 * @file CCommonData.h
 * @brief Global Data Pool class
 *
 * This file contains the class declaration for the Global Data Pool handling
 * class.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_CCOMMONDATA_H_
#define __RGF_CCOMMONDATA_H_

class CLevel;
class CPlayer;
class CActorManager;
class CModelManager;
class CWeapon;
class CEntityRegistry;
class CExplosionManager;
class EffManager;
class CPreEffect;
class CLanguageManager;
class CArmour;
class CAudioManager;
class CCDAudio;
class CGameStateManager;
class CGUIManager;
class CHeadsUpDisplay;
class sxConversationManager;
class sxInventory;
class CMIDIAudio;
class CPolyShadow;
class qxTerrainMgr;

/**
 * @brief Global Data Pool class
 *
 * All access to common RGF components MUST be performed via this class, and
 * all components MUST be registered via this class as well. This provides a
 * central point of access and eases debugging later in the RGF development cycle.
 */
class CCommonData
{
public:
	CCommonData();
	~CCommonData();

	inline sxLog* Log()		{ return sxLog::GetSingletonPtr(); }

	/**
	 * @brief Initialize common components
	 */
	int Initialize(HINSTANCE hInstance, char *szStartLevel, bool commandLine);
	void Shutdown();						///< Shut down common components

	void StartGame(bool commandLine);

	int SaveTo(FILE *saveFD);
	int RestoreFrom(FILE *restoreFD);

	/**
	 * @brief Play a GIF or AVI file
	 */
	void Play(const char *szFilename, int xPos, int yPos, bool center);

	bool ProcessLevelChange();					///< Process a level change

	void PlayOpeningCutScene();					///< Play opening cut scene
	void CheckMediaPlayers();					///< Poll all media players

	DWORD GetTimePassed_D();						///< Get amount of time passed, in msec
	float GetTimePassed_F();						///< Get amount of time passed, in msec
	DWORD FreeRunningCounter();						///< Return system free-running counter
	float FreeRunningCounter_F();					///< Same as aboe, only floating-point
	void ResetClock();								///< Reset the internal timer
	void Spin(DWORD dwMilliseconds);				///< Waste time

	void SetChangeLevelData(const _ChangeLevel *entity);	///< Set change level data
	void SetLevelData();

	// Inline data accessor member functions

	inline CGenesisEngine*		Engine()			{ return m_GameEngine;			}
	inline geWorld*				World()				{ return m_GameEngine->World(); }
	inline CGUIManager*			GUIManager()		{ return m_GUIManager;			}
	inline CLanguageManager*	LanguageManager()	{ return m_LanguageManager;		}
	// start multiplayer
	inline NetPlayerMgr*		NetPlayerManager()	{ return m_NetPlayerManager;	}
	// end multiplayer
	inline CAudioManager*		AudioManager()		{ return m_AudioManager;		}
	inline CCameraManager*		CameraManager()		{ return m_CameraManager;		}
	inline Collider*			Collision()			{ return m_Collider;			}
	inline CRFMenu*				MenuManager()		{ return m_Menu;				}
	inline CInput*				Input()				{ return m_InputManager;		}
	inline CCDAudio*			CDPlayer()			{ return m_CDPlayer;			}
	inline CMIDIAudio*			MIDIPlayer()		{ return m_MIDIPlayer;			}
	inline CArmour*				Armours()			{ return m_Armour;				}

	inline qxTerrainMgr*		TerrainManager();

	inline CLevel*		Level()				{ return m_Level;					}
	CPlayer*			Player();//			{ return m_Level->Player();			}
	CActorManager*		ActorManager();//	{ return m_Level->ActorManager();	}
	CModelManager*		ModelManager();//	{ return m_Level->ModelManager();	}
	CWeapon*			Weapons();//		{ return m_Level->WeaponManager();	}
	CHeadsUpDisplay*	HUD();//			{ return m_Level->HUD();			}
	CEntityRegistry*	EntityRegistry();//	{ return m_Level->EntityRegistry();	}
	EffManager*			EffectManager();//	{ return m_Level->EffectManager();	}
	CPreEffect*			Effect();//			{ return m_Level->PreEffects();		}

	inline CPolyShadow*	PlyShdw()				{ return m_PolyShadow;			}

	inline const char* StartName() const		{ return m_StartPointName;		}
	inline const char* SplashScreen() const		{ return m_SplashScreen;		}
	inline const char* SplashAudio() const		{ return m_SplashAudio;			}
	inline const char* CutScene() const			{ return m_CutScene;			}
	inline const char* SplashScreen1() const	{ return m_SplashScreen1;		}
	inline const char* SplashAudio1() const		{ return m_SplashAudio1;		}
	inline const char* CutScene1() const		{ return m_CutScene1;			}

	inline bool HeadBob() const					{ return m_HeadBob;				}
	inline bool PositionWeapon() const			{ return m_PositionWeapon;		}
	inline bool GetAlterKey() const				{ return m_AlterKey;			}

	// Control change level flag
	inline void SetChangeLevel(bool fHit)		{ m_ChangeLevel = fHit;			}
	inline bool ChangeLevel() const				{ return m_ChangeLevel;			}

	inline bool GetSaving() const				{ return m_Saving;				}
	inline void SetSaving(bool flag)			{ m_Saving = flag;				}

	inline bool GetCharacterSelection() const	{ return m_CharacterSelection;	}
	inline bool GetNameSelection() const		{ return m_NameSelection;		}

	inline bool GetPaused() const				{ return m_Paused;				}
	inline void SetPaused(bool flag)			{ m_Paused = flag;				}

	inline bool GetKeyPaused() const			{ return m_KeyPaused;			}
	inline void SetKeyPaused(bool flag)			{ m_KeyPaused = flag;			}

	inline bool GetDifficultSelection() const	{ return m_DifficultSelection;	}
	inline int GetDifficultLevel() const		{ return m_DifficultLevel;		}
	inline void SetDifficultLevel(int level)	{ m_DifficultLevel = level;		}

	inline bool GetCmdLine() const				{ return m_CmdLine;				}

	inline GE_RGBA GetFogColor()				{ return m_EffectColor;			}
	inline bool GetUseEffect() const			{ return m_UseEffect;			}
	inline const char* NextLevel() const		{ return m_NewLevel;			}

	inline bool GetHasFocus() const				{ return m_HasFocus;			}
	inline void SetHasFocus(bool flag)			{ m_HasFocus = flag;			}

	// Time inlines
	inline DWORD LastElapsedTime_D() const		{ return m_LastTimePassed_D;	}
	inline float LastElapsedTime_F() const		{ return m_LastTimePassed_F;	}
	inline __int64 CurrentGameTime_L() const	{ return m_LastTimePoll;		}
	inline DWORD CurrentGameTime_D()			{ return static_cast<DWORD>(m_LastTimePoll); }
	inline float CurrentGameTime_F()			{ return static_cast<float>(m_LastTimePoll); }
	inline float GetTicksGoneBy() const			{ return m_dwTicksGoneBy;		}

// start multiplayer

	inline bool GetNetwork() const				{ return m_Network;				}
	void ShutDownNetWork();
	bool GetMultiPlayer();
	void SetMultiPlayer(bool multi, bool Server);
	inline bool GetServer() const				{ return m_Server;				}
// end multiplayer

	inline bool GetMouseControl() const			{ return m_MouseControl;		}
	inline void SetMouseControl(bool flag)		{ m_MouseControl = flag;		}
	inline bool GetInGameCursor() const			{ return m_InGameCursor;		}

	inline bool GetUseLevelDialog() const		{ return m_UseLevelDialog;		}
	inline const std::string& GetDefaultPlayerName()	{ return m_PlayerName;	}
	inline const std::string& GetDefaultPlayerAvatar()	{ return m_PlayerAvatar;}

	inline bool	ShowTrack() const				{ return m_ShowTrack;			}
	inline bool	SaveAttributesAsText() const	{ return m_SaveAttributesAsText;}

private:
	CGUIManager*			m_GUIManager;
	CGameStateManager*		m_GameStateManager;
	CLanguageManager*		m_LanguageManager;

	CGenesisEngine*			m_GameEngine;		///< Genesis engine class
// start multiplayer
	NetPlayerMgr*			m_NetPlayerManager;
// end multiplayer
	CInput*					m_InputManager;		///< User input class
	CLevel*					m_Level;
	CCDAudio*				m_CDPlayer;			///< CD Audio player class
	CMIDIAudio*				m_MIDIPlayer;		///< MIDI Audio player class
	CRFMenu*				m_Menu;				///< Menu Manager
	Collider*				m_Collider;			///< Collision detection subsystem
	CAudioManager*			m_AudioManager;		///< Audio manager subsystem
	CCameraManager*			m_CameraManager;	///< Camera manager subsystem
	CPolyShadow*			m_PolyShadow;
	sxInventory*			m_Inventory;
	sxConversationManager*	m_ConversationManager;
	CArmour*				m_Armour;


	// Timekeeping information
	__int64		m_LastTimePoll;				///< Last time we polled the time
	DWORD		m_LastTimePassed_D;			///< Last elapsed time as DWORD
	float		m_LastTimePassed_F;			///< Last elapsed time as FLOAT
	// Global game state information
#if 0
	DWORD		m_TotalPlayTime;			///< Total game time, this session
#endif

	// Level change information
	bool		m_ChangeLevel;				///< We hit a level change
	char		m_NewLevel[256];			///< New level filename from changelevel
	char		m_SplashScreen[256];		///< Splash screen filename from changelevel
	char		m_SplashAudio[256];			///< Audio to play with splash screen
	char		m_CutScene[256];			///< Cut scene to play from changelevel
	char		m_SplashScreen1[256];		///< Splash screen filename from changelevel
	char		m_SplashAudio1[256];		///< Audio to play with splash screen
	char		m_CutScene1[256];			///< Cut scene to play from changelevel
	char		m_Message[256];
	std::string	m_Font;
	geVec3d		m_PlayerRotation;
	geVec3d		m_CameraRotation;
	geVec3d		m_PlayerOffset;
	char		m_StartPointName[256];
	bool		m_KeepAttributes;
	bool		m_SplashHold;
	float		m_DefaultDistance, m_CameraX, m_CameraY;
	geVec3d		m_CameraTranslationOffset;
	geVec3d		m_CameraRotationOffset;
	int			m_ViewPoint;
	bool		m_UseEffect;	// use fog effect at level change
	GE_RGBA		m_EffectColor;	// fog color
	bool		m_UseAngle;

	std::string	m_Slot[40];		// MAX_WEAPONS = 40 !
	std::string	m_CurrentWeapon;

	bool		m_Paused;
	bool		m_KeyPaused;
	bool		m_MouseControl;				///< use mouse to control player
	bool		m_InGameCursor;

	float		m_dwTicksGoneBy;

	bool		m_ShowTrack;

	// Configuration data
	std::string	m_PlayerName;
	std::string	m_PlayerAvatar;				///< Player avatar actor file
	bool	m_NameSelection;
	bool	m_DifficultSelection;
	int		m_DifficultLevel;
	bool	m_CharacterSelection;
	bool	m_SaveAttributesAsText;

	bool	m_UseLevelDialog;

	bool	m_HeadBob;
	bool	m_PositionWeapon;				///< do in-game weapon positioning
	bool	m_AlterKey;

// start multiplayer
	bool	m_Saving;
	bool	m_Network;
	bool	m_Multiplayer;
	bool	m_Server;
// end multiplayer

	static void CALLBACK TimerFunction(UINT uID, UINT uMsg, DWORD dwUser,
						DWORD dw1, DWORD dw2);		///< Static timer callback
	int		m_nTimerID;
	__int64 m_TimeCounter;
	bool	m_HasFocus;
	bool	m_CmdLine;

	float	m_LODdistance[5];
	bool	m_LODAnimation;
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
