/************************************************************************************//**
 * @file CMenu.h
 * @brief CMenu class declaration
 ****************************************************************************************/

#ifndef __RGF_CRFMENU_H_
#define __RGF_CRFMENU_H_

#include <hash_map>
#include <stack>
#include "HawkNL\\nl.h"

#define NUM_SELECT			10


typedef struct	SoundList
{
	SoundList*	next;
	SoundList*	prev;
	geSound*	Sound;
	float		Volume;
	float		Pan;
	float		Frequency;

} SoundList;


class CCharacter;
class CMIDIAudio;
class CMenuEventHandler;


/**
 * @brief Class declaration for CRFMenu
 */
class CRFMenu : public CRGFComponent
{
	friend class CMenuEventHandler;

public:
	explicit CRFMenu(const char* szStartLevel);
	~CRFMenu();

	void Enter();
	void Exit();

	void SetActiveSection(const std::string& section, bool skipFade = false);
	void QuitSection();

	void DrawBitmap(const geBitmap* bitmap, geRect* source, uint32 x, uint32 y);
	void DoGame(bool editor);

	/**
	 * @brief Prepare to load a different menu configuration file
	 */
	void ChangeConfiguration(const std::string& filename);
	void LoadText(const std::string& menutxt);

	void GameLoop();
	void LoadGame();
	void SaveGame(bool confirm = true);
	void DeleteSavegame(bool confirm = true);

	void DisplaySplash();

	/**
	 * @brief Save a screenshot
	 */
	void ScreenShot();

	//int GetSaveScreenWidth();
	//int GetSaveScreenHeight();

	geSound* PlaySoundDef(geSound_System *soundSystem, geSound_Def *soundDef, float volume,
							float pan, float frequency, geBoolean loop);
	geBoolean StopSound(geSound_System* soundSystem, geSound* sound);
	void DeleteSound();
	void StopMusic();

	int SaveTo(FILE *SaveFD, bool type);
	int RestoreFrom(FILE *RestoreFD, bool type);

	inline const char* GetLevelName() const	{	 return m_LevelName;			}
	inline void SetLevelName(const char* level)	{ strcpy(m_LevelName, level);	}

	inline void SetInGame(int inGame = 1)		{ m_InGame = inGame;		}
	inline int GetInGame() const				{ return m_InGame;			}

	inline float GetMouseSen() const			{ return m_MouseSen;		}

	/**
	 * @brief Turn mouse reverse on/off
	 */
	inline void SetMouseReverse(bool value)		{ m_bMouseReverse = value;	}
	inline bool IsMouseReversed() const			{ return m_bMouseReverse;	}

	/**
	 * @brief Turn mouse filter on/off
	 */
	inline void SetMouseFilter(bool value)		{ m_bMouseFilter = value;	}
	inline bool IsMouseFilterEnabled() const	{ return m_bMouseFilter;	}
	inline bool IsCrosshairsEnabled() const		{ return m_bCrosshairs;		}

	inline bool	IsFrameRateEnabled() const		{ return m_bFrameRate;		}
	inline bool	IsDebugInfoEnabled() const		{ return m_bDebugInfo;		}
	/**
	 * @brief Turn player bounding box on/off
	 */
	inline void SetBoundBox(bool value)			{ m_bBoundingBox = value;	}
	inline bool GetBoundBox() const				{ return m_bBoundingBox;	}
	/**
	 * @brief Turn entity bounding box on/off
	 */
	inline void SetSEBoundBox(bool value)		{ m_bSEBoundingBox = value;	}
	inline bool GetSEBoundBox() const			{ return m_bSEBoundingBox;	}
	/**
	 * @brief Turn no clipping on/off
	 */
	inline void SetNoClip(bool value)			{ m_bNoClip = value;		}
	inline bool GetNoClip() const				{ return m_bNoClip;			}

	inline void SetFilter(POINT value)			{ m_Filter = value;			}
	inline POINT GetFilter()					{ return m_Filter;			}

	inline void SetShot(bool value)				{ m_bScrnWait = value;		}
	inline bool GetShot() const					{ return m_bScrnWait;		}

	inline int GetMusicType() const				{ return m_MusicType;		}
	inline float GetDetail() const				{ return m_Detail;			}
	inline bool GetStencilShadows() const		{ return m_bStencilShadows;	}

	//inline geBitmap* GetCrossHair()				{ return m_Crosshairs;	}
	inline geBitmap* GetFCrossHair()			{ return m_FCrosshairs;	}

	//inline char *GetLoadmsg()					{ return m_Loadmsg;		}

	inline bool GetUseNameSelect() const		{ return m_bUseNameSelect;	}
	inline bool GetUseSelect() const			{ return m_bUseSelect;		}

	/**
	 * @brief Change currently selected player character
	 */
	void ChangeSelectedCharacter(bool direction);

	inline CCharacter* GetSelectedCharacter()	{ return m_Characters[m_SelectedCharacter]; }

	inline CMIDIAudio *MIDIPlayer()				{ return m_MIDIPlayer;	}

	void SetMusicVol(float vol);
	void SetmMusicVol(float vol);

	inline float GetmVolLevel() const			{ return m_mVolLevel;	}
	inline StreamingAudio* GetStreamingAudio()	{ return m_Streams;		}
	inline const std::string& GetMusic() const	{ return m_Music;		}

	/**
	 * @brief Get selected playername
	 */
	inline const std::string& GetSelectedName() const		{ return m_SelectedName;	}

	/**
	 * @brief Set selected playername
	 */
	inline void SetSelectedName(const std::string& name)	{ m_SelectedName = name;	}

	void ClearVolume();
	void ResetVolume();

	void DisplayCrossHair();

	void FadeSet(int dir, float time);
	void DoFade();
	bool IsFading() const { return m_bFading; }

	inline float GetTimeFade() const { return m_TimeFade; }

	/**
	 * @brief Initialize all menu items
	 */
	void Initialize();

private:

	void SaveSettings();
	/**
	 * @brief Reset menu data to initial values
	 */
	void Reset();

	void InitializeWidgets();

	/**
	 * @brief Load menu data from an ini file
	 */
	void LoadConfiguration(const std::string& filename);

	/**
	 * @brief Load player character data from the character.ini file
	 */
	void LoadCharacterConfiguration(const std::string& startLevel);

	void LoadControlConfiguration();

	int Name2ActionID(const std::string& name);

	void FindSavegames();
	int GetNumberFromFilename(const std::string& filename);
	std::string MakeSavegameFilename(int number);
	std::string MakeSavegameLabel();

	void FillActionMapList();
	void ResetActionMapList();
	void RefreshActionMapList();

	void DisableOrphanedButtons();

	float GetGamma();
	void SetGamma(float gamma);

	void SetDetail(float value);

	void SetMouseSensitivity(float value);

	void SetStencilShadows(bool state);

	void PlayMouseClickSound();
	void PlayKeyClickSound();
	void PlaySlideClickSound();

	void SetPlayerNameWindowText();

private:
	//geBitmap*	m_Crosshairs;
	geBitmap*	m_FCrosshairs;
	char		m_LevelName[256];

	std::string	m_SelectedName;
	bool		m_bUseNameSelect;
	int			m_ScreenshotCount;
	bool		m_bScrnWait;
	int			m_InGame;

	bool		m_bFading;
	int			m_FadeDir;
	float		m_FadeTime;
	float		m_CurrentFadeTime;
	float		m_FadeAlpha;
	bool		m_bMusicFade;
	float		m_OldMVol;

	float		m_MouseSen;
	bool		m_bMouseReverse;
	bool		m_bMouseFilter;
	bool		m_bCrosshairs;

	bool		m_bFrameRate;
	bool		m_bDebugInfo;
	bool		m_bBoundingBox;
	bool		m_bSEBoundingBox;
	bool		m_bNoClip;
	POINT		m_Filter;
	float		m_Detail;
	bool		m_bStencilShadows;

	geSound_Def*	m_MouseClick;
	geSound_Def*	m_KeyClick;
	geSound_Def*	m_SlideClick;
	SoundList*		m_Bottom;
	std::string		m_Music;
	int				m_MusicType;
	StreamingAudio*	m_Streams;
	LPDIRECTSOUND	m_dsPtr;

	CCharacter*	m_Characters[NUM_SELECT];
	int			m_MaxCharacters;
	int			m_SelectedCharacter;
	bool		m_bUseSelect;

	/*float		m_SaveTime;
	float		m_SavingTime;
	char		m_Savemsg[256];
	int			m_SaveFont;
	char		m_Loadmsg[256];
	int			m_LoadFont;*/
	DWORD		m_WinVol;
	CMIDIAudio*	m_MIDIPlayer;
	char		m_Loading[128];
	float		m_mVolLevel;
	float		m_TimeFade;

	std::stack<CEGUI::Window*> m_SectionStack;
	stdext::hash_map<std::string, CEGUI::Window*> m_Sections;

	stdext::hash_map<std::string, std::string> m_Text;

	CEGUI::String m_LoadGameDefaultImage;
	CEGUI::String m_SaveGameDefaultImage;

	CMenuEventHandler*	m_EventHandler;

	char m_ServerIP[50];
	char m_IP[NL_MAX_STRING_LENGTH];

	int m_SavegameCounter;
	stdext::hash_map<std::string, std::string> m_Savegames;
	std::string m_NewSavegameLabel;
	std::string m_KeyNameUnassigned;
	std::vector<int> m_Controls;
};


#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
