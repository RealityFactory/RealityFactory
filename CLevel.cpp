/************************************************************************************//**
 * @file CLevel.cpp
 * @brief Genesis3D Level
 *
 * This file contains the class implementation for the Level class.
 * @author Daniel Queteschiner
 * @date December 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "RabidFramework.h"

#include "CHeadsUpDisplay.h"
#include "CScriptManager.h"
#include "Qx\\qxTerrainMgr.h"

#include "C3DAudioSource.h"
#include "CActMaterial.h"
#include "CAreaChecker.h"
#include "CAttribute.h"
#include "CAutoDoors.h"
#include "CChange.h"
#include "CChangeLevel.h"
#include "Chaos.h"
#include "CCorona.h"
#include "CCountDown.h"
#include "CutScene.h"
#include "CDamage.h"
#include "CDecal.h"
#include "CDSpotLight.h"
#include "CDynalite.h"
#include "CElectric.h"
#include "CFirePoint.h"
#include "CFixedCamera.h"
#include "CFlame.h"
#include "CFlipBook.h"
#include "CFlipTree.h"
#include "CFloating.h"
#include "CFoliage.h"
#include "CLiftBelt.h"
#include "CLiquid.h"
#include "CLogic.h"
#include "CMessage.h"
#include "CMorph.h"
#include "CMorphingFields.h"
#include "CMovingPlatforms.h"
#include "COverlay.h"
#include "CParticleSystem.h"
#include "CPathDatabase.h"
#include "CPathFollower.h"
#include "CPawnManager.h"
#include "CProcedural.h"
#include "CRain.h"
#include "CScriptPoint.h"
#include "CShake.h"
#include "CSoundtrackToggle.h"
#include "CSpout.h"
#include "CStaticEntity.h"
#include "CStaticMesh.h"
#include "CTeleporter.h"
#include "CTriggers.h"
#include "CVideoTexture.h"
#include "CViewSwitch.h"
#include "CWallDecal.h"
#include "CWindGenerator.h"

#include "CLevel.h"

#define DEFAULT_GRAVITY	(-490.0f)
/*
per level ini files:
* AttributeInfoFile
* HUDInfoFile
* Audible radius (EnvironmentSetup)
per character ini files
* Weapon
* AttributeInfoFile
* PlayerSetup
* Environment
* HUDInfoFile
*/
CLevel::CLevel() :
	m_ActorManager(NULL),
	m_ModelManager(NULL),
	m_WeaponManager(NULL),
	m_TerrainManager(NULL),
	m_ExplosionManager(NULL),
	m_PreEffects(NULL),
	m_EffectManager(NULL),
	m_HUD(NULL),
	m_AudibleRadius(360.f),
	m_LODAnimation(false),
	m_FogActive(false),
	m_ClipActive(false),
	m_Player(NULL),
	m_EntityRegistry(NULL),

	m_3DAudioSources(NULL),
	m_ActorMaterials(NULL),
	m_ActorSpouts(NULL),
	m_AreaChecks(NULL),
	m_Attributes(NULL),
	m_AudioStreams(NULL),
	m_AutoDoors(NULL),
	m_ChangeAttributes(NULL),
	m_ChangeLevels(NULL),
	m_Coronas(NULL),
	m_CountDownTimers(NULL),
	m_CutScenes(NULL),
	m_ChaosEffects(NULL),
	m_DestroyableModels(NULL),
	m_Decals(NULL),
	m_SpotLights(NULL),
	m_Lights(NULL),
	m_ElectricBolts(NULL),
	m_Explosions(NULL),
	m_FirePoints(NULL),
	m_FixedCameras(NULL),
	m_Flames(NULL),
	m_FlipBooks(NULL),
	m_FlipTrees(NULL),
	m_FloatingParticles(NULL),
	m_Foliage(NULL),
	m_LevelControllers(NULL),
	m_LiftBelts(NULL),
	m_Liquids(NULL),
	m_LogicGates(NULL),
	m_Messages(NULL),
	m_MorphingTextures(NULL),
	m_MorphingFields(NULL),
	m_Platforms(NULL),
	m_Overlays(NULL),
	m_ParticleSystems(NULL),
	m_PathDatabase(NULL),
	m_PathFollowers(NULL),
	m_PawnManager(NULL),
	m_ProceduralTextures(NULL),
	m_RainEffects(NULL),
	m_ScriptPoints(NULL),
	m_ScreenShakes(NULL),
	m_STToggles(NULL),
	m_Spouts(NULL),
	m_Props(NULL),
	m_Meshes(NULL),
	m_Teleporters(NULL),
	m_Triggers(NULL),
	m_VideoTextures(NULL),
	m_ViewSwitches(NULL),
	m_WallDecals(NULL),
	m_WindGenerator(NULL),
	m_EnvironmentSetup(NULL),
	m_SpeedCoeffLava(0.55f),
	m_SpeedCoeffSlowMotion(0.25f),
	m_SpeedCoeffFastMotion(2.0f)
{
	memset(m_LODDistance, 0, sizeof(float)*5);
	geVec3d_Set(&m_Gravity, 0.0f, DEFAULT_GRAVITY, 0.0f);
	geVec3d_Clear(&m_Wind);
	geVec3d_Clear(&m_InitialWind);
}


CLevel::CLevel(const std::string& levelName)
{
	Initialize(levelName);
}


CLevel::~CLevel()
{
	Shutdown();
}


void CLevel::Update(float timeElapsed)
{
	// First, check the path follower so that any motion triggers get turned
	// ..on prior to tick()'ing entities that may need to know they've been
	// ..put into motion
	m_PathFollowers->Tick(timeElapsed);

	// EVIL HACK! You MUST check model motions before you check
	// ..actor motions or the system goes out of sync! LOOK,
	// ..JUST DO IT!

	// Give time to the model manager so animated world models move...
	m_ModelManager->Tick(timeElapsed);

	// Next, advance time for all the actors since this'll affect the
	// ..various actor animations and could cause bounding box changes.

	//theActorManager->Tick(m_dwTicksGoneBy);

	m_WindGenerator->Tick(timeElapsed);

	// Ok, deal out time to everyone else.
	m_Triggers->Tick(timeElapsed);				// Time to Triggers
	m_LogicGates->Tick(timeElapsed);			// Time to Logic
	m_AutoDoors->Tick(timeElapsed);				// Advance doors, if needed
	m_Platforms->Tick(timeElapsed);				// Advance platforms, if needed
	m_Teleporters->Tick(timeElapsed);			// Animate teleporter fields
	m_MorphingFields->Tick(timeElapsed);		// Animate morphing fields
	m_ParticleSystems->Tick(timeElapsed);		// Animate particle systems
	m_Props->Tick(timeElapsed);					// Animate static models
	m_Meshes->Tick(timeElapsed);
	m_AudioStreams->Tick(timeElapsed);			// Deal with streaming audio proxies
	m_STToggles->Tick(timeElapsed);				// Soundtrack toggles dealt with
	m_VideoTextures->Tick(timeElapsed);			// Video textures dealt with
	m_Coronas->Tick(timeElapsed);				// Update light coronas
	m_Lights->Tick(timeElapsed);				// Update dynamic lights
	m_ElectricBolts->Tick(timeElapsed);			// Update electrical effects
	m_ProceduralTextures->Tick(timeElapsed);	// Update procedural textures
	m_RainEffects->Tick(timeElapsed);			// Time to Rain Effect
	m_Spouts->Tick(timeElapsed);				// Time to Spout Effect
	m_ActorSpouts->Tick(timeElapsed);
	m_MorphingTextures->Tick(timeElapsed);
	m_ActorMaterials->Tick(timeElapsed);
	m_FloatingParticles->Tick(timeElapsed);		// Time to Float Effect
	m_ChaosEffects->Tick(timeElapsed);			// Time to Chaos Procedural
	m_Flames->Tick(timeElapsed);				// Time to Flame Effect
	m_3DAudioSources->Tick(timeElapsed);		// Time to 3D Audio Source
	m_Decals->Tick(timeElapsed);
	m_WallDecals->Tick(timeElapsed);
	m_TerrainManager->Frame();
	m_WeaponManager->Tick(timeElapsed);
	m_FirePoints->Tick(timeElapsed);
	m_FlipBooks->Tick(timeElapsed);
	m_Foliage->Tick(timeElapsed);
	m_FlipTrees->Tick(timeElapsed);
	m_AreaChecks->Tick(timeElapsed);
	m_Attributes->Tick(timeElapsed);
	m_DestroyableModels->Tick(timeElapsed);
	m_Explosions->Tick(timeElapsed);
	m_CountDownTimers->Tick(timeElapsed);
	m_ChangeAttributes->Tick(timeElapsed);
	m_Overlays->Tick(timeElapsed);
	m_LiftBelts->Tick(timeElapsed);
	m_SpotLights->Tick(timeElapsed);
	m_ChangeLevels->Tick(timeElapsed);
	m_ScreenShakes->Tick(timeElapsed);
	m_FixedCameras->Tick();
	m_ViewSwitches->Tick();
	m_LevelControllers->Tick(timeElapsed);

	m_Player->Tick(timeElapsed);
	m_PawnManager->Tick(timeElapsed);

	m_WeaponManager->DoAttack();
	m_ExplosionManager->Tick(timeElapsed);
	m_EffectManager->Tick(timeElapsed);
}


void CLevel::Render()
{
	m_ParticleSystems->Render();	// Render all particle systems

	if(CCD->ShowTrack())
		m_ScriptPoints->Render();

	m_Overlays->Render();
	m_TerrainManager->Render();
	m_Meshes->Render();

	return;
}


int CLevel::Initialize(const std::string& levelName)
{
	CCD->Log()->Notice("Initializing Level: " + levelName);

	m_AudibleRadius = 360.0f;

	CCD->Log()->Debug("Configuring Camera Defaults...");
	CCD->CameraManager()->Defaults();

	CCD->Log()->Debug("Loading Level File...");
	if(CCD->Engine()->LoadLevel(levelName.c_str()) == FALSE)
	{
		CCD->Log()->Critical("Failed to load Level [" + levelName + "]!");
		return -11;
	}

	// Ok, we have to initialize the entity registry, so that all the other
	// ..components can register the entities they are responsible for
	CCD->Log()->Debug("Initializing Entity Registry...");
	m_EntityRegistry = new CEntityRegistry();
	if(m_EntityRegistry == NULL)
	{
		CCD->Log()->Critical("Failed to create Entity Registry!");
		return -12;
	}

	CCD->Log()->Debug("Initializing Terrain Manager...");
	m_TerrainManager = new qxTerrainMgr();
	if(m_TerrainManager == NULL)
	{
		CCD->Log()->Critical("Failed to create Terrain Manager!");
		return -13;
	}

	CCD->Log()->Debug("Initializing Effects Manager...");
	m_EffectManager = new EffManager();
	if(m_EffectManager == NULL)
	{
		CCD->Log()->Critical("Failed to create Effect Manager!");
		return -14;
	}

	// The Actor Manager is up next
	CCD->Log()->Debug("Initializing Actor Manager...");
	m_ActorManager = new CActorManager();
	if(m_ActorManager == NULL)
	{
		CCD->Log()->Critical("Failed to create Actor Manager!");
		return -15;
	}

	ParseEnvironmentSetup();

	// Then the Model Manager comes into exitence
	CCD->Log()->Debug("Initializing Model Manager...");
	m_ModelManager = new CModelManager();
	if(m_ModelManager == NULL)
	{
		CCD->Log()->Critical("Failed to create Model Manager!");
		return -16;
	}

	// Create the player avatar and load it.
	CCD->Log()->Debug("Creating Player...");
	m_Player = new CPlayer();
	if(m_Player == NULL)
	{
		CCD->Log()->Critical("Failed to create Player!");
		return -17;
	}

	CCD->Log()->Debug("Loading Player Avatar...");
	if(m_Player->LoadAvatar(CCD->GetDefaultPlayerAvatar(), CCD->GetDefaultPlayerName()) != RGF_SUCCESS)
	{
		CCD->Log()->Critical("Failed to load Player Actor!");
		return -18;
	}

	ScriptManager::AddGlobalVariable("Player", m_Player);

	// Set up the heads-up display (HUD) for the game
	CCD->Log()->Debug("Initializing HUD...");
	m_HUD = new CHeadsUpDisplay();
	if(m_HUD == NULL)
	{
		CCD->Log()->Critical("Failed to create HUD!");
		return -19;
	}

	CCD->Log()->Debug("Initializing Destroyable Models Manager Subsystem...");
	m_DestroyableModels = new CDamage();
	if(m_DestroyableModels == NULL)
	{
		CCD->Log()->Critical("Failed to create Destroyable Models Manager Subsystem!");
		return -20;
	}

	// other level-specific items

	CCD->Log()->Debug("Initializing Fixed Camera Manager Subsystem...");
	m_FixedCameras = new CFixedCamera();
	if(m_FixedCameras == NULL)
	{
		CCD->Log()->Critical("Failed to create Fixed Camera handling class!");
		return -21;
	}

	CCD->Log()->Debug("Loading Player Configuration...");
	m_Player->LoadConfiguration();

	// Set up automatic door handling
	CCD->Log()->Debug("Initializing Automatic Door Manager Subsystem...");
	m_AutoDoors = new CAutoDoors();
	if(m_AutoDoors == NULL)
	{
		CCD->Log()->Critical("Failed to create Automatic Door handling class!");
		return -22;
	}

	// Start up the moving platform class
	CCD->Log()->Debug("Initializing Moving Platform Manager Subsystem...");
	m_Platforms = new CMovingPlatforms();
	if(m_Platforms == NULL)
	{
		CCD->Log()->Critical("Failed to create Moving Platform handling class!");
		return -23;
	}

	// Activate teleports
	CCD->Log()->Debug("Initializing Teleport Manager Subsystem...");
	m_Teleporters = new CTeleporter();
	if(m_Teleporters == NULL)
	{
		CCD->Log()->Critical("Failed to create Teleporter handling class!");
		return -24;
	}

	// Load up the morphing field effects
	CCD->Log()->Debug("Initializing Morphing Field Effects Manager Subsystem...");
	m_MorphingFields = new CMorphingFields();
	if(m_MorphingFields == NULL)
	{
		CCD->Log()->Critical("Failed to create Morphing Field handling class");
		return -25;
	}

	// Load and start up all 3D audio sources
	CCD->Log()->Debug("Initializing 3d Audio Source Manager Subsystem...");
	m_3DAudioSources = new C3DAudioSource();
	if(m_3DAudioSources == NULL)
	{
		CCD->Log()->Critical("Failed to create 3D Audio Source handling class!");
		return -26;
	}

	// Fire up particle system handling
	CCD->Log()->Debug("Initializing Particle Effects Manager Subsystem...");
	m_ParticleSystems = new CParticleSystem();
	if(m_ParticleSystems == NULL)
	{
		CCD->Log()->Critical("Failed to create Particle System handler!");
		return -27;
	}

	// Load up all the static entities (props) for the level.
	CCD->Log()->Debug("Initializing Static Entity Props Subsystem...");
	m_Props = new CStaticEntity();
	if(m_Props == NULL)
	{
		CCD->Log()->Critical("Failed to create Static Entity handler!");
		return -28;
	}

	// Load up all the static meshes for the level.
	CCD->Log()->Debug("Initializing Static Mesh Subsystem...");
	m_Meshes = new CStaticMesh();
	if(m_Meshes == NULL)
	{
		CCD->Log()->Critical("Failed to create Static Mesh handler!");
		return -29;
	}

	// Set up soundtrack toggles for the level
	CCD->Log()->Debug("Initializing Soundtrack Toggle Subsystem...");
	m_STToggles = new CSoundtrackToggle();
	if(m_STToggles == NULL)
	{
		CCD->Log()->Critical("Failed to create Soundtrack Toggle handler!");
		return -30;
	}

	// Fire up the streaming audio handler
	CCD->Log()->Debug("Initializing Streaming Audio Manager Subsystem...");
	m_AudioStreams = new CAudioStream();
	if(m_AudioStreams == NULL)
	{
		CCD->Log()->Critical("Failed to create Streaming Audio handler!");
		return -31;
	}

	// and the video texture handler component
	CCD->Log()->Debug("Initializing Video Texture Manager Subsystem...");
	m_VideoTextures = new CVideoTexture();
	if(m_VideoTextures == NULL)
	{
		CCD->Log()->Critical("Failed to create Video Texture handler!");
		return -32;
	}

	// corona effect
	CCD->Log()->Debug("Initializing Corona Manager Subsystem...");
	m_Coronas = new CCorona();
	if(m_Coronas == NULL)
	{
		CCD->Log()->Critical("Failed to create Corona handler!");
		return -33;
	}

	// dynamic lights
	CCD->Log()->Debug("Initializing Dynamic Light Manager Subsystem...");
	m_Lights = new CDynalite();
	if(m_Lights == NULL)
	{
		CCD->Log()->Critical("Failed to create Dynamic Light handler!");
		return -34;
	}

	CCD->Log()->Debug("Initializing ElectricBolt Manager Subsystem...");
	m_ElectricBolts = new CElectric();
	if(m_ElectricBolts == NULL)
	{
		CCD->Log()->Critical("Failed to create Electric handler!");
		return -35;
	}

	CCD->Log()->Debug("Initializing Procedural Texture Manager Subsystem...");
	m_ProceduralTextures = new CProcedural();
	if(m_ProceduralTextures == NULL)
	{
		CCD->Log()->Critical("Failed to create Procedural Texture handler!");
		return -36;
	}

	// Create the in-memory path database for this level.
	CCD->Log()->Debug("Initializing Path Database...");
	m_PathDatabase = new CPathDatabase();
	if(m_PathDatabase == NULL)
	{
		CCD->Log()->Critical("Failed to create Path Database!");
		return -37;
	}

	// Set up path followers to bind entities to motion paths
	CCD->Log()->Debug("Initializing Path Followers...");
	m_PathFollowers = new CPathFollower();
	if(m_PathFollowers == NULL)
	{
		CCD->Log()->Critical("Failed to create Path Follower!");
		return -38;
	}

	// Initialize the Effects Manager and the various effects
	// ..that it handles.
	CCD->Log()->Debug("Initializing Rain Effects Manager...");
	m_RainEffects = new CRain();
	if(m_RainEffects == NULL)
	{
		CCD->Log()->Critical("Failed to create Rain handler!");
		return -39;
	}

	CCD->Log()->Debug("Initializing Spout Effects Manager...");
	m_Spouts = new CSpout();
	if(m_Spouts == NULL)
	{
		CCD->Log()->Critical("Failed to create Spout handler!");
		return -40;
	}

	CCD->Log()->Debug("Initializing Actor Spout Effects Manager...");
	m_ActorSpouts = new CActorSpout();
	if(m_ActorSpouts == NULL)
	{
		CCD->Log()->Critical("Failed to create Actor Spout handler!");
		return -41;
	}

	CCD->Log()->Debug("Initializing Floating Particle Effects Manager...");
	m_FloatingParticles = new CFloat();
	if(m_FloatingParticles == NULL)
	{
		CCD->Log()->Critical("Failed to create Floating Particle handler!");
		return -42;
	}

	CCD->Log()->Debug("Initializing Chaos Effects Manager...");
	m_ChaosEffects = new Chaos();
	if(m_ChaosEffects == NULL)
	{
		CCD->Log()->Critical("Failed to create Chaos handler!");
		return -43;
	}

	CCD->Log()->Debug("Initializing Flame Effects Manager...");
	m_Flames = new CFlame();
	if(m_Flames == NULL)
	{
		CCD->Log()->Critical("Failed to create Flame handler!");
		return -44;
	}

	CCD->Log()->Debug("Initializing Script Point Manager Subsystem...");
	m_ScriptPoints = new CScriptPoint();
	if(m_ScriptPoints == NULL)
	{
		CCD->Log()->Critical("Failed to create Script Point handler!");
		return -45;
	}

	CCD->Log()->Debug("Initializing Pawn Manager Subsystem...");
	m_PawnManager = new CPawnManager();
	if(m_PawnManager == NULL)
	{
		CCD->Log()->Critical("Failed to create Pawn handler!");
		return -46;
	}

	CCD->Log()->Debug("Initializing ChangeAttribute Manager Subsystem...");
	m_ChangeAttributes = new CChangeAttribute();
	if(m_ChangeAttributes == NULL)
	{
		CCD->Log()->Critical("[ERROR] Failed to create ChangeAttribute handler!");
		return -47;
	}

	CCD->Log()->Debug("Initializing Countdown Manager Subsystem...");
	m_CountDownTimers = new CCountDown();
	if(m_CountDownTimers == NULL)
	{
		CCD->Log()->Critical("Failed to create CountDownTimer handler!");
		return -48;
	}

	// Set up triggers
	CCD->Log()->Debug("Initializing Trigger Manager Subsystem...");
	m_Triggers = new CTriggers();
	if(m_Triggers == NULL)
	{
		CCD->Log()->Critical("Failed to create trigger handling class!");
		return -49;
	}

	// Set up logic gates
	CCD->Log()->Debug("Initializing LogicHandler Subsystem...");
	m_LogicGates = new CLogic();
	if(m_LogicGates == NULL)
	{
		CCD->Log()->Critical("Failed to create Logic handling class!");
		return -50;
	}

	// Message
	CCD->Log()->Debug("Initializing Message Manager Subsystem...");
	m_Messages = new CMessage();
	if(m_Messages == NULL)
	{
		CCD->Log()->Critical("Failed to create Message handling class!");
		return -51;
	}

	CCD->Log()->Debug("Initializing Effect Manager Subsystem...");
	m_PreEffects = new CPreEffect();
	if(m_PreEffects == NULL)
	{
		CCD->Log()->Critical("Failed to create Effect subsystem!");
		return -52;
	}

	// Weapon
	CCD->Log()->Debug("Initializing Weapon Manager Subsystem...");
	m_WeaponManager = new CWeapon();
	if(m_WeaponManager == NULL)
	{
		CCD->Log()->Critical("Failed to create Weapon handling class!");
		return -53;
	}

	CCD->Log()->Debug("Initializing FirePoint Manager Subsystem...");
	m_FirePoints = new CFirePoint();
	if(m_FirePoints == NULL)
	{
		CCD->Log()->Critical("Failed to create FirePoint handling class!");
		return -54;
	}

	CCD->Log()->Debug("Initializing Flipbook Manager Subsystem...");
	m_FlipBooks = new CFlipBook();
	if(m_FlipBooks == NULL)
	{
		CCD->Log()->Critical("Failed to create FlipBook handling class!");
		return -55;
	}

	CCD->Log()->Debug("Initializing AreaCheck Manager Subsystem...");
	m_AreaChecks = new CAreaChecker();
	if(m_AreaChecks == NULL)
	{
		CCD->Log()->Critical("Failed to create AreaCheck handling class!");
		return -56;
	}

	CCD->Log()->Debug("Initializing Foliage Manager Subsystem...");
	m_Foliage = new CFoliage();
	if(m_Foliage == NULL)
	{
		CCD->Log()->Critical("Failed to create Foliage handling class!");
		return -57;
	}

	CCD->Log()->Debug("Initializing Tree Manager Subsystem...");
	m_FlipTrees = new CFlipTree();
	if(m_FlipTrees == NULL)
	{
		CCD->Log()->Critical("Failed to create Tree handling class!");
		return -58;
	}

	// Decal
	CCD->Log()->Debug("Initializing Decal Manager Subsystem...");
	m_Decals = new CDecal();
	if(m_Decals == NULL)
	{
		CCD->Log()->Critical("Failed to create Decal handling class!");
		return -61;
	}

	CCD->Log()->Debug("Initializing WallDecal Manager Subsystem...");
	m_WallDecals = new CWallDecal();
	if(m_WallDecals == NULL)
	{
		CCD->Log()->Critical("Failed to create WallDecal handling class!");
		return -62;
	}

	CCD->Log()->Debug("Initializing LevelController Manager Subsystem...");
	m_LevelControllers = new CLevelController();
	if(m_LevelControllers == NULL)
	{
		CCD->Log()->Critical("Failed to create LevelController handling class!");
		return -63;
	}

	// Attribute
	CCD->Log()->Debug("Initializing Attribute Manager Subsystem...");
	m_Attributes = new CAttribute();
	if(m_Attributes == NULL)
	{
		CCD->Log()->Critical("Failed to create Attribute handling class!");
		return -64;
	}

	CCD->Log()->Debug("Initializing Explosion Manager Subsystem...");
	m_ExplosionManager = new CExplosionManager();
	if(m_ExplosionManager == NULL)
	{
		CCD->Log()->Critical("Failed to create Explosion Manager subsystem!");
		return -65;
	}

	CCD->Log()->Debug("Initializing Explosion Subsystem...");
	m_Explosions = new CExplosion();
	if(m_Explosions == NULL)
	{
		CCD->Log()->Critical("Failed to create Explosion subsystem!");
		return -66;
	}

	CCD->Log()->Debug("Initializing ChangeLevel Manager Subsystem...");
	m_ChangeLevels = new CChangeLevel();
	if(m_ChangeLevels == NULL)
	{
		CCD->Log()->Critical("Failed to create ChangeLevel handling class!");
		return -67;
	}

	CCD->Log()->Debug("Initializing ScreenShake Subsystem...");
	m_ScreenShakes = new CShake();
	if(m_ScreenShakes == NULL)
	{
		CCD->Log()->Critical("Failed to create ScreenShake handler!");
		return -68;
	}

	CCD->Log()->Debug("Initializing ViewSwitch Subsystem...");
	m_ViewSwitches = new CViewSwitch();
	if(m_ViewSwitches == NULL)
	{
		CCD->Log()->Critical("Failed to create ViewSwitch handler!");
		return -69;
	}

	CCD->Log()->Debug("Initializing Liquid Subsystem...");
	m_Liquids = new CLiquid();
	if(m_Liquids == NULL)
	{
		CCD->Log()->Critical("Failed to create Liquid handler!");
		return -71;
	}

	CCD->Log()->Debug("Initializing Overlay Subsystem...");
	m_Overlays = new COverlay();
	if(m_Overlays == NULL)
	{
		CCD->Log()->Critical("Failed to create Overlay handler!");
		return -72;
	}

	CCD->Log()->Debug("Initializing TextureMorph Subsystem...");
	m_MorphingTextures = new CMorph();
	if(m_MorphingTextures == NULL)
	{
		CCD->Log()->Critical("Failed to create TextureMorph handler!");
		return -73;
	}

	CCD->Log()->Debug("Initializing CutScene Subsystem...");
	m_CutScenes = new CCutScene();
	if(m_CutScenes == NULL)
	{
		CCD->Log()->Critical("Failed to create CutScene handler!");
		return -74;
	}

	CCD->Log()->Debug("Initializing Actor Material Subsystem...");
	m_ActorMaterials = new CActMaterial();
	if(m_ActorMaterials == NULL)
	{
		CCD->Log()->Critical("Failed to create Actor Material handler!");
		return -75;
	}

	CCD->Log()->Debug("Initializing Lift Belt Manager...");
	m_LiftBelts = new CLiftBelt();
	if(m_LiftBelts == NULL)
	{
		CCD->Log()->Critical("Failed to create Lift Belt handler!");
		return -77;
	}

	CCD->Log()->Debug("Initializing Dynamic Spotlight Manager...");
	m_SpotLights = new CDSpotLight();
	if(m_SpotLights == NULL)
	{
		CCD->Log()->Critical("Failed to create Dynamic SpotLight handler!");
		return -78;
	}

	CCD->Log()->Debug("Initializing Wind Generator...");
	m_WindGenerator = new CWindGenerator();
	if(m_WindGenerator == NULL)
	{
		CCD->Log()->Critical("Failed to create Wind Generator!");
		return -80;
	}

	CCD->Log()->Notice("Preparing to Launch Game...");

	// All level classes up! Let's **PLAY**
	return 0;
}


void CLevel::Shutdown()
{
	// start multiplayer
	if(CCD->NetPlayerManager() != NULL)
	{
		CCD->NetPlayerManager()->DeletePlayers();
	}
	// end multiplayer

	SAFE_DELETE(m_SpotLights);
	SAFE_DELETE(m_LiftBelts);
	SAFE_DELETE(m_ActorMaterials);
	SAFE_DELETE(m_CutScenes);
	SAFE_DELETE(m_MorphingTextures);
	SAFE_DELETE(m_Overlays);
	SAFE_DELETE(m_Liquids);
	SAFE_DELETE(m_ViewSwitches);
	SAFE_DELETE(m_ScreenShakes);
	SAFE_DELETE(m_ChangeLevels);
	SAFE_DELETE(m_Explosions);
	SAFE_DELETE(m_ExplosionManager);
	SAFE_DELETE(m_Attributes);
	SAFE_DELETE(m_Decals);
	SAFE_DELETE(m_WallDecals);
	SAFE_DELETE(m_LevelControllers);
	SAFE_DELETE(m_FlipBooks);
	SAFE_DELETE(m_Foliage);
	SAFE_DELETE(m_FlipTrees);
	SAFE_DELETE(m_AreaChecks);
	SAFE_DELETE(m_FirePoints);
	SAFE_DELETE(m_WeaponManager);
	SAFE_DELETE(m_PreEffects);
	SAFE_DELETE(m_Messages);
	SAFE_DELETE(m_ScriptPoints);
	SAFE_DELETE(m_PawnManager);
	SAFE_DELETE(m_ChangeAttributes);
	SAFE_DELETE(m_CountDownTimers);
	SAFE_DELETE(m_LogicGates);
	SAFE_DELETE(m_Triggers);
	SAFE_DELETE(m_Flames);
	SAFE_DELETE(m_ChaosEffects);
	SAFE_DELETE(m_RainEffects);
	SAFE_DELETE(m_ActorSpouts);
	SAFE_DELETE(m_Spouts);
	SAFE_DELETE(m_FloatingParticles);
	SAFE_DELETE(m_EffectManager);
	SAFE_DELETE(m_PathFollowers);
	SAFE_DELETE(m_PathDatabase);
	SAFE_DELETE(m_ProceduralTextures);
	SAFE_DELETE(m_Coronas);
	SAFE_DELETE(m_Lights);
	SAFE_DELETE(m_ElectricBolts);
	SAFE_DELETE(m_VideoTextures);
	SAFE_DELETE(m_AudioStreams);
	SAFE_DELETE(m_STToggles);
	SAFE_DELETE(m_Props);
	SAFE_DELETE(m_Meshes);
	SAFE_DELETE(m_ParticleSystems);
	SAFE_DELETE(m_3DAudioSources);
	SAFE_DELETE(m_Platforms);
	SAFE_DELETE(m_FixedCameras);
	SAFE_DELETE(m_AutoDoors);
	ScriptManager::RemoveGlobalVariable("Player");
	SAFE_DELETE(m_Player);
	SAFE_DELETE(m_Teleporters);
	SAFE_DELETE(m_MorphingFields);
	SAFE_DELETE(m_HUD);
	SAFE_DELETE(m_ModelManager);
	SAFE_DELETE(m_ActorManager);
	SAFE_DELETE(m_EntityRegistry);
	SAFE_DELETE(m_TerrainManager);
	SAFE_DELETE(m_EffectManager);
	SAFE_DELETE(m_DestroyableModels);
	SAFE_DELETE(m_WindGenerator);
}


void CLevel::ParseEnvironmentSetup()
{
	memset(m_LODDistance, 0, sizeof(float)*5);
	m_LODAnimation = false;
	geVec3d_Set(&m_Gravity, 0.0f, DEFAULT_GRAVITY, 0.0f);
	geVec3d_Clear(&m_Wind);
	geVec3d_Clear(&m_InitialWind);
	m_FogActive = false;
	m_ClipActive = false;
	m_AudibleRadius = 360.f;
	m_SpeedCoeffLava = 0.55f;
	m_SpeedCoeffSlowMotion = 0.25f;
	m_SpeedCoeffFastMotion = 2.0f;

	CCD->Log()->Debug("Parsing EnvironmentSetup Entity...");

	geEntity_EntitySet *entitySet = geWorld_GetEntitySet(CCD->World(), "EnvironmentSetup");

	if(entitySet != NULL)
	{
		geEntity *entity = geEntity_EntitySetGetNextEntity(entitySet, NULL);

		if(entity != NULL)
		{
			m_EnvironmentSetup = static_cast<EnvironmentSetup*>(geEntity_GetUserData(entity));

			m_LODDistance[0] = m_EnvironmentSetup->LODdistance1;
			m_LODDistance[1] = m_EnvironmentSetup->LODdistance2;
			m_LODDistance[2] = m_EnvironmentSetup->LODdistance3;
			m_LODDistance[3] = m_EnvironmentSetup->LODdistance4;
			m_LODDistance[4] = m_EnvironmentSetup->LODdistance5;
			m_LODAnimation   = m_EnvironmentSetup->LODAnimation;

			if(	m_EnvironmentSetup->Gravity.X != 0.0f ||
				m_EnvironmentSetup->Gravity.Y != 0.0f ||
				m_EnvironmentSetup->Gravity.Z != 0.0f)
			{
				m_Gravity = m_EnvironmentSetup->Gravity;		// Over-ride default gravity
			}

			if(	m_EnvironmentSetup->WindSpeed.X != 0.0f ||
				m_EnvironmentSetup->WindSpeed.Y != 0.0f ||
				m_EnvironmentSetup->WindSpeed.Z != 0.0f)
			{
				m_Wind = m_EnvironmentSetup->WindSpeed;
				m_InitialWind = m_EnvironmentSetup->WindSpeed;
			}

			if(m_EnvironmentSetup->AudibleRadius != 0.0f)
			{
				m_AudibleRadius = m_EnvironmentSetup->AudibleRadius;
			}

			float detailLevel = CCD->MenuManager()->GetDetail();

			if(m_EnvironmentSetup->EnableDistanceFog == GE_TRUE)
			{
				float start = m_EnvironmentSetup->FogStartDistLow +
					(m_EnvironmentSetup->FogStartDistHigh - m_EnvironmentSetup->FogStartDistLow) *
					detailLevel;
				float end = m_EnvironmentSetup->TotalFogDistLow +
					(m_EnvironmentSetup->TotalFogDistHigh - m_EnvironmentSetup->TotalFogDistLow) *
					detailLevel;
				CCD->Engine()->SetFogParameters(m_EnvironmentSetup->DistanceFogColor, start, end);
				m_FogActive = true;
			}

			if(m_EnvironmentSetup->UseFarClipPlane == GE_TRUE)
			{
				float dist = m_EnvironmentSetup->FarClipPlaneDistLow +
					(m_EnvironmentSetup->FarClipPlaneDistHigh - m_EnvironmentSetup->FarClipPlaneDistLow) *
					detailLevel;
				CCD->CameraManager()->SetFarClipPlane(dist);
				m_ClipActive = true;
			}

			CCD->CameraManager()->SetShakeMin(m_EnvironmentSetup->MinShakeDist);

			if(CCD->MenuManager()->GetStencilShadows())
			{
				geEngine_SetStencilShadowsEnable(
					CCD->Engine()->Engine(),
					GE_TRUE,
					m_EnvironmentSetup->SShadowsMaxLightToUse,
					m_EnvironmentSetup->SShadowsColor.r,
					m_EnvironmentSetup->SShadowsColor.g,
					m_EnvironmentSetup->SShadowsColor.b,
					m_EnvironmentSetup->SShadowsAlpha);
			}

			if(m_EnvironmentSetup->SpeedCoeffLava != 0)
				m_SpeedCoeffLava = m_EnvironmentSetup->SpeedCoeffLava * 0.01f;
			if(m_EnvironmentSetup->SpeedCoeffSlowMotion != 0)
				m_SpeedCoeffSlowMotion = m_EnvironmentSetup->SpeedCoeffSlowMotion * 0.01f;
			if(m_EnvironmentSetup->SpeedCoeffFastMotion != 0)
				m_SpeedCoeffFastMotion = m_EnvironmentSetup->SpeedCoeffFastMotion * 0.01f;
		}
	}
}

/* ------------------------------------------------------------------------------------ */
// If fogging is enabled, activate it.
/* ------------------------------------------------------------------------------------ */
void CLevel::ShowFog()
{
	CCD->Engine()->EnableFog(m_FogActive);
}

/* ------------------------------------------------------------------------------------ */
// Unconditionally disable fog.
/* ------------------------------------------------------------------------------------ */
void CLevel::DisableFog()
{
	CCD->Engine()->EnableFog(GE_FALSE);
}

/* ------------------------------------------------------------------------------------ */
// Activate the far clip plane, if used.
/* ------------------------------------------------------------------------------------ */
void CLevel::ActivateClipPlane()
{
	CCD->CameraManager()->EnableFarClipPlane(m_ClipActive);
}

/* ------------------------------------------------------------------------------------ */
// Unconditionally disable the far clip plane.
/* ------------------------------------------------------------------------------------ */
void CLevel::DisableClipPlane()
{
	CCD->CameraManager()->EnableFarClipPlane(GE_FALSE);
}
