/************************************************************************************//**
 * @file CParticleSystem.h
 * @brief Particle Systems Handler header file
 *
 * This file contains the class declaration and data types for the RGF particle
 * system handler classes.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_CPARTICLESYSTEM_H_
#define __RGF_CPARTICLESYSTEM_H_

/**
 * @brief Particle contains information for individual particles
 */
typedef struct _Particle
{
	GE_LVertex	Vertex;			///< Vertex color, this particle
	gePoly		*Poly;			///< Particle polygon
	unsigned	Flags;			///< Any flags for this particle
	_Particle	*Next;			///< Next particle in list
	_Particle	*Prev;			///< Previous particle in list
	float		LifeTime;		///< Particles msec. left to live
	float		Velocity;		///< Particles velocity
	geVec3d		Pos;			///< Particle position
	geVec3d		Rotation;		///< Particle rotation
	geBitmap	*Texture;		///< Particle texture

} theParticle;

/**
 * @brief ParticleSystem contains information for entire arrays of particles
 */
typedef	struct _ParticleSystem
{
	theParticle *Particles;			///< Head of linked list of particles
	geVec3d		Emitter;			///< Emitter position in the world
	int			Count;				///< # of particles in this system
	int			Maximum;			///< Max. # of particles this system can have
	int			SpawnSpeed;			///< # of particles/second emitted
	float		Lifetime;			///< Lifetime in msec. of each particle
	geBitmap	*Texture;			///< Texture for particles
	float		SystemLife;			///< Lifetime of particle system, in msec.
	float		Gravity;			///< Baseline gravity for particle system
	float		Scale;				///< Scaling of particles in system
	bool		bRunning;			///< Particle system running?
	bool		bVisible;			///< Particle system visible/invisible?
	int			nType;				///< Particle system type
	GE_RGBA		Color;				///< Particle color
	float		InitialVelocity;	///< Particle initial velocity
	float		EffectRadius;		///< Max. radius of effect (not used in all system types)
	bool		bForever;			///< Particle system lives until manually killed
	float		AlphaDecay;			///< Alpha-blending particle decay value
	char		szName[256];		///< Particle system name

} ParticleSystem;

//	Constants for particle system types:
//	SHOCKWAVE			- particles emitted on hoizontal plane
//	FOUNTAIN			- particles emitted with gravity, velocity
//	RAIN				- particles emitted a random distance on X/Z planes
//	SPHERE				- particles emitted with varying velocity vectors
//	COLUMN				- particles emitted with velocity, no gravity
//	EXPLOSIVE ARRAY		- particles emitted UPWARD (+Y) from emitter plane
//							(stole this from The Slayers Anime, Go Lina Inverse!
//	SPIRAL ARM			- Kinda like the Milky Way Galaxy
//	TRAIL				- Typical particle trail
//	GUARDIAN			- Interesting pseudo-columnal-spiral effect
//	IMPLODING SPHERE	- Hey, an imploding sphere!
//	IMPLODING SHOCKWAVE - Imploding shockwave, cool!
//	IMPLODING SPIRAL ARM- Collapsing galaxy theory is true

const int kPSystem_Shockwave		= 0x0001;	///< SHOCKWAVE particle system
const int kPSystem_Fountain			= 0x0002;	///< FOUNTAIN particle system
const int kPSystem_Rain				= 0x0003;	///< RAIN particle system
const int kPSystem_Sphere			= 0x0004;	///< SPHERE particle system
const int kPSystem_Column			= 0x0005;	///< COLUMN particle system
const int kPSystem_ExplosiveArray	= 0x0006;	///< Tribute to Lina Inverse!
const int kPSystem_SpiralArm		= 0x0007;	///< SPIRAL ARM particle system
const int kPSystem_Trail			= 0x0008;	///< TRAIL particle system
const int kPSystem_Guardian			= 0x0009;	///< GUARDIAN EFFECT particle system
const int kPSystem_ImplodeSphere	= 0x000a;	///< Imploding sphere particle system
const int kPSystem_ImplodeShockwave = 0x000b;	///< Imploding shockwave
const int kPSystem_ImplodeSpiralArm = 0x000c;	///< Imploding spiral arms

//#define PARTICLE_RENDERSTYLE	GE_RENDER_DO_NOT_OCCLUDE_OTHERS
#define PARTICLE_RENDERSTYLE	GE_RENDER_DEPTH_SORT_BF | GE_RENDER_DO_NOT_OCCLUDE_OTHERS


/**
 * @brief CParticleSystem class handles particles and particle systems
 */
class CParticleSystem : public CRGFComponent
{
public:
	CParticleSystem();				///< Default constructor
	~CParticleSystem();				///< Default destructor

	/**
	 * @brief Load bitmap for particles
	 */
	int LoadParticleMap(int *pmHandle, const char *szPrimary, const char *szAlpha);

	/**
	 * @brief Create particle system
	 */
	int Create(int *psHandle, int kSystemType, int nTexHandle,
				const geVec3d &emitterPos, float Scale, const char *szName);

	int CreateSound(const geVec3d &Origin, const char *SoundFile, float radius);

	int Rewind(int psHandle);					///< Restart from beginning

	int Pause(int psHandle, bool bPause);		///< Pause/unpause particle system

	int Hide(int psHandle, bool bShowIt);		///< Hide/unhide particle system

	int Destroy(int psHandle);					///< Destroy a particle system

	int SetColor(int psHandle, const GE_RGBA &clrColor, bool bChangeExisting);	///< Set particle color

	int SetEmitter(int psHandle, const geVec3d &emitterPos);					///< Set emitter position

	int SetTexture(int psHandle, int pmHandle, bool bChangeExisting);			///< Set particle texture

	int SetVelocity(int psHandle, float fVelocity, bool bChangeExisting);		///< Set initial velocity

	int SetParticleLife(int psHandle, int nLifeInMsec);							///< Set particle life

	int SetEffectRadius(int psHandle, float fRadius);							///< Set effect radius

	int SetSystemLife(int psHandle, int nLife);									///< Set system lifetime

	int SetBirthRate(int psHandle, int nBirthRate);								///< Set particle birth rate

	int SetMaxSize(int psHandle, int nMaxParticles);							///< Set max. # of particles in system

	void Tick(geFloat dwTicks);													///< Step time in all particle systems

	/**
	 * @brief Render all particle systems in range
	 */
	int Render();

	/**
	 * @brief Given a name, locate the desired entity in the currently loaded
	 * level and return its user data.
	 */
	int LocateEntity(const char *szName, void **pEntityData);

	/**
	 * @brief Correct internal timing to match current time, to make up for time
	 * lost when outside the game loop (typically in "menu mode").
	 */
	int ReSynchronize();

private:
	int Allocate();								///< Allocate particle system

	int Remove(int nHandle);					///< Remove particle system

	int Clear(int nHandle);						///< Clear all particles from system

	/**
	 * @brief Add new particle to system
	 */
	theParticle *Spawn(int nHandle, const GE_RGBA &theColor,
						const geVec3d &Pos,	const geVec3d &Rotation,
						float fVelocity);

	/**
	 * @brief Add new imploding particle to system
	 */
	theParticle *ReverseSpawn(int nHandle, const GE_RGBA &theColor,
						const geVec3d &Pos,	const geVec3d &Rotation,
						float fVelocity);

	/**
	 * @brief Update particle system
	 */
	void Sweep(int nHandle, float dwMsec);

	void ApplyGravity(int nHandle, theParticle *theParticle);

	/**
	 * @brief Render particle system
	 */
	void RenderSystem(int nHandle, const geVec3d &PlayerPos);

	void SetupShockwave(int nHandle);				///< Load initial shockwave particles

	void AddShockwaveParticle(int nHandle);			///< Add new shockwave particle

	void SetupSpiralArm(int nHandle);				///< Load initial spiral arm particles

	void AddSpiralArmParticle(int nHandle);			///< Add new spiral arm particle

	void SetupFountain(int nHandle);				///< Load initial fountain particles

	void AddFountainParticle(int nHandle);			///< Add new fountain particle

	void SetupRain(int nHandle);					///< Load initial rain particles

	void AddRainParticle(int nHandle);				///< Add new rain particle

	void SetupSphere(int nHandle);					///< Load initial sphere particles

	void AddSphereParticle(int nHandle);			///< Add new sphere particle

	void SetupTrail(int nHandle);					///< Load initial trail particles

	void AddTrailParticle(int nHandle);				///< Add new trail particle

	void SetupGuardian(int nHandle);				///< Load initial guardian-effect particles

	void AddGuardianParticle(int nHandle);			///< Add new guardian-effect particle

	void SetupImplodeSphere(int nHandle);			///< Load initial imploding sphere particles

	void AddImplodeSphereParticle(int nHandle);		///< Add new imploding sphere particle

	void SetupImplodeShockwave(int nHandle);		///< Load initial shockwave particles

	void AddImplodeShockwaveParticle(int nHandle);	///< Add new shockwave particle

	void SetupImplodeSpiralArm(int nHandle);		///< Load initial spiral arm particles

	void AddImplodeSpiralArmParticle(int nHandle);	///< Add new spiral arm particle

private:
	ParticleSystem *theList[50];		///< Up to 50 simultaneous particle systems active
	geBitmap *bmpList[50];				///< Up to 50 bitmaps for particle systems
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
