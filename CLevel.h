/************************************************************************************//**
 * @file CLevel.h
 * @brief Genesis3D Level handler
 *
 * This file contains the declaration of the Level class.
 * @author Daniel Queteschiner
 * @date December 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_LEVEL_H_
#define __RGF_LEVEL_H_

class CActorManager;
class CModelManager;
class CWeapon;
class qxTerrainMgr;
class CExplosionManager;
class CPreEffect;
class EffManager;
class CHeadsUpDisplay;
class CPlayer;
class CEntityRegistry;
class C3DAudioSource;
class CActMaterial;
class CActorSpout;
class CAreaChecker;
class CAttribute;
class CAudioStream;
class CAutoDoors;
class CChangeAttribute;
class CChangeLevel;
class CCorona;
class CCountDown;
class CCutScene;
class Chaos;
class CDamage;
class CDecal;
class CDSpotLight;
class CDynalite;
class CElectric;
class CExplosion;
class CFirePoint;
class CFixedCamera;
class CFlame;
class CFlipBook;
class CFlipTree;
class CFloat;
class CFoliage;
class CLevelController;
class CLiftBelt;
class CLiquid;
class CLogic;
class CMessage;
class CMorph;
class CMorphingFields;
class CMovingPlatforms;
class COverlay;
class CParticleSystem;
class CPathDatabase;
class CPathFollower;
class CPawnManager;
class CProcedural;
class CRain;
class CScriptPoint;
class CShake;
class CSoundtrackToggle;
class CSpout;
class CStaticEntity;
class CStaticMesh;
class CTeleporter;
class CTriggers;
class CVideoTexture;
class CViewSwitch;
class CWallDecal;
class CWindGenerator;


class CLevel
{
public:
	CLevel();
	explicit CLevel(const std::string& levelName);
	~CLevel();

	/**
	 * @brief Load and initialize level
	 */
	int Initialize(const std::string& levelName);

	/**
	 * @brief Clean up & shutdown level
	 */
	void Shutdown();

	void Update(float timeElapsed);

	void Render();

	inline float GetAudibleRadius() const			{ return m_AudibleRadius;		}
	inline void SetAudibleRadius(float radius)		{ m_AudibleRadius = radius;		}

	inline float GetLODdistance(int index) const	{ return m_LODDistance[index];	}
	inline bool GetLODAnimation() const				{ return m_LODAnimation;		}


	inline CPlayer*				Player()			{ return m_Player;				}
	inline CActorManager*		ActorManager()		{ return m_ActorManager;		}
	inline CModelManager*		ModelManager()		{ return m_ModelManager;		}
	inline CWeapon*				WeaponManager()		{ return m_WeaponManager;		}
	inline CHeadsUpDisplay*		HUD()				{ return m_HUD;					}
	inline CEntityRegistry*		EntityRegistry()	{ return m_EntityRegistry;		}
	inline CExplosionManager*	ExplosionManager()	{ return m_ExplosionManager;	}
	inline EffManager*			EffectManager()		{ return m_EffectManager;		}
	inline CPreEffect*			PreEffects()		{ return m_PreEffects;			}
	inline qxTerrainMgr*		TerrainManager()	{ return m_TerrainManager;		}

	inline C3DAudioSource*		Audio3D()			{ return m_3DAudioSources;		}
	inline CActMaterial*		ActorMaterials()	{ return m_ActorMaterials;		}
	inline CActorSpout*			ActorSpouts()		{ return m_ActorSpouts;			}
	inline CAreaChecker*		AreaChecks()		{ return m_AreaChecks;			}
	inline CAttribute*			Attributes()		{ return m_Attributes;			}
	inline CAudioStream*		AudioStreams()		{ return m_AudioStreams;		}
	inline CAutoDoors*			Doors()				{ return m_AutoDoors;			}
	inline CChangeAttribute*	ChangeAttributes()	{ return m_ChangeAttributes;	}
	inline CChangeLevel*		ChangeLevels()		{ return m_ChangeLevels;		}
	inline CCorona*				Coronas()			{ return m_Coronas;				}
	inline CCountDown*			CountDownTimers()	{ return m_CountDownTimers;		}
	inline CCutScene*			CutScenes()			{ return m_CutScenes;			}
	inline Chaos*				ChaosEffects()		{ return m_ChaosEffects;		}
	inline CDamage*				Damage()			{ return m_DestroyableModels;	}
	inline CDecal*				Decals()			{ return m_Decals;				}
	inline CDSpotLight*			SpotLights()		{ return m_SpotLights;			}
	inline CDynalite*			Lights()			{ return m_Lights;				}
	inline CElectric*			ElectricBolts()		{ return m_ElectricBolts;		}
	inline CExplosion*			Explosions()		{ return m_Explosions;			}
	inline CFirePoint*			FirePoints()		{ return m_FirePoints;			}
	inline CFixedCamera*		FixedCameras()		{ return m_FixedCameras;		}
	inline CFlame*				Flames()			{ return m_Flames;				}
	inline CFlipBook*			FlipBooks()			{ return m_FlipBooks;			}
	inline CFlipTree*			FlipTrees()			{ return m_FlipTrees;			}
	inline CFloat*				FloatingParticles()	{ return m_FloatingParticles;	}
	inline CFoliage*			Foliage()			{ return m_Foliage;				}
	inline CLevelController*	LevelControllers()	{ return m_LevelControllers;	}
	inline CLiftBelt*			LiftBelts()			{ return m_LiftBelts;			}
	inline CLiquid*				Liquids()			{ return m_Liquids;				}
	inline CLogic*				LogicGates()		{ return m_LogicGates;			}
	inline CMessage*			Messages()			{ return m_Messages;			}
	inline CMorph*				MorphingTextures()	{ return m_MorphingTextures;	}
	inline CMorphingFields*		MorphingFields()	{ return m_MorphingFields;		}
	inline CMovingPlatforms*	Platforms()			{ return m_Platforms;			}
	inline COverlay*			Overlays()			{ return m_Overlays;			}
	inline CParticleSystem*		ParticleSystems()	{ return m_ParticleSystems;		}
	inline CPathDatabase*		PathDatabase()		{ return m_PathDatabase;		}
	inline CPathFollower*		PathFollower()		{ return m_PathFollowers;		}
	inline CPawnManager*		Pawns()				{ return m_PawnManager;			}
	inline CProcedural*			ProceduralTextures(){ return m_ProceduralTextures;	}
	inline CRain*				RainEffects()		{ return m_RainEffects;			}
	inline CScriptPoint*		ScriptPoints()		{ return m_ScriptPoints;		}
	inline CShake*				ScreenShakes()		{ return m_ScreenShakes;		}
	inline CSoundtrackToggle*	SoundtrackToggles() { return m_STToggles;			}
	inline CSpout*				Spouts()			{ return m_Spouts;				}
	inline CStaticEntity*		Props()				{ return m_Props;				}
	inline CStaticMesh*			Meshes()			{ return m_Meshes;				}
	inline CTeleporter*			Teleporters()		{ return m_Teleporters;			}
	inline CTriggers*			Triggers()			{ return m_Triggers;			}
	inline CVideoTexture*		VideoTextures()		{ return m_VideoTextures;		}
	inline CViewSwitch*			ViewSwitches()		{ return m_ViewSwitches;		}
	inline CWallDecal*			WallDecals()		{ return m_WallDecals;			}
	inline CWindGenerator*		WindGenerator()		{ return m_WindGenerator;		}

	inline EnvironmentSetup*	GetEnvironmentSetup(){ return m_EnvironmentSetup;	}

	void ShowFog();							///< Activate fog, if used
	void DisableFog();						///< Unconditionally disable fog

	void ActivateClipPlane();				///< Activate clip plane, if used
	void DisableClipPlane();				///< Unconditionally disable clip plane


	geVec3d GetGravity() const						{ return m_Gravity;						}
	void	SetGravity(const geVec3d* vec)			{ geVec3d_Copy(vec, &m_Gravity);		}

	geVec3d GetWind() const							{ return m_Wind;						}
	void	SetWind(const geVec3d* vec)				{ geVec3d_Copy(vec, &m_Wind);			}
	void	ModifyWind(const geVec3d* vec)			{ geVec3d_Add(vec, &m_Wind, &m_Wind);	}

	geVec3d GetInitialWind() const					{ return m_InitialWind;					}
	void	SetInitialWind(const geVec3d* vec)		{ geVec3d_Copy(vec, &m_InitialWind);	}

	inline float GetSpeedCoeffLava() const			{ return m_SpeedCoeffLava;				}
	inline float GetSpeedCoeffSlowMotion() const	{ return m_SpeedCoeffSlowMotion;		}
	inline float GetSpeedCoeffFastMotion() const	{ return m_SpeedCoeffFastMotion;		}
private:
	void ParseEnvironmentSetup();

private:
	CActorManager*		m_ActorManager;
	CModelManager*		m_ModelManager;
	CWeapon*			m_WeaponManager;
	qxTerrainMgr*		m_TerrainManager;
	CExplosionManager*	m_ExplosionManager;
	CPreEffect*			m_PreEffects;
	EffManager*			m_EffectManager;
	CHeadsUpDisplay*	m_HUD;

	float				m_AudibleRadius;
	float				m_LODDistance[5];
	bool				m_LODAnimation;
	bool				m_FogActive;
	bool				m_ClipActive;

	CPlayer*			m_Player;

	CEntityRegistry*	m_EntityRegistry;

	C3DAudioSource*		m_3DAudioSources;
	CActMaterial*		m_ActorMaterials;
	CActorSpout*		m_ActorSpouts;
	CAreaChecker*		m_AreaChecks;
	CAttribute*			m_Attributes;
	CAudioStream*		m_AudioStreams;
	CAutoDoors*			m_AutoDoors;
	CChangeAttribute*	m_ChangeAttributes;
	CChangeLevel*		m_ChangeLevels;
	CCorona*			m_Coronas;
	CCountDown*			m_CountDownTimers;
	CCutScene*			m_CutScenes;
	Chaos*				m_ChaosEffects;
	CDamage*			m_DestroyableModels;
	CDecal*				m_Decals;
	CDSpotLight*		m_SpotLights;
	CDynalite*			m_Lights;
	CElectric*			m_ElectricBolts;
	CExplosion*			m_Explosions;
	CFirePoint*			m_FirePoints;
	CFixedCamera*		m_FixedCameras;
	CFlame*				m_Flames;
	CFlipBook*			m_FlipBooks;
	CFlipTree*			m_FlipTrees;
	CFloat*				m_FloatingParticles;
	CFoliage*			m_Foliage;
	CLevelController*	m_LevelControllers;
	CLiftBelt*			m_LiftBelts;
	CLiquid*			m_Liquids;
	CLogic*				m_LogicGates;
	CMessage*			m_Messages;
	CMorph*				m_MorphingTextures;
	CMorphingFields*	m_MorphingFields;
	CMovingPlatforms*	m_Platforms;
	COverlay*			m_Overlays;
	CParticleSystem*	m_ParticleSystems;
	CPathDatabase*		m_PathDatabase;
	CPathFollower*		m_PathFollowers;
	CPawnManager*		m_PawnManager;
	CProcedural*		m_ProceduralTextures;
	CRain*				m_RainEffects;
	CScriptPoint*		m_ScriptPoints;
	CShake*				m_ScreenShakes;
	CSoundtrackToggle*	m_STToggles;
	CSpout*				m_Spouts;
	CStaticEntity*		m_Props;
	CStaticMesh*		m_Meshes;
	CTeleporter*		m_Teleporters;
	CTriggers*			m_Triggers;
	CVideoTexture*		m_VideoTextures;
	CViewSwitch*		m_ViewSwitches;
	CWallDecal*			m_WallDecals;
	CWindGenerator*		m_WindGenerator;
	EnvironmentSetup*	m_EnvironmentSetup;

	geVec3d				m_Gravity;
	geVec3d				m_Wind;
	geVec3d				m_InitialWind;

	float				m_SpeedCoeffLava;
	float				m_SpeedCoeffSlowMotion;
	float				m_SpeedCoeffFastMotion;
};

#endif // __RGF_LEVEL_H_
