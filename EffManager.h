/************************************************************************************//**
 * @file EffManager.h
 * @brief Effects Manager handler
 *
 * This file contains the class declarations for the Effects Manager that
 * handles special effects for RGF-based games.
 * @author Ralph Deane
 *//*
 * Copyright (c) 1999 Ralph Deane; All Rights Reserved.
 ****************************************************************************************/

#ifndef __RGF_EFFMANAGER_H_
#define __RGF_EFFMANAGER_H_

#define MAX_EFF_ITEMS   1000  // Maximun number of effects

#define EFF_LIGHT       1
#define EFF_SPRAY       2
#define EFF_SPRITE      3
#define EFF_SND         4
#define EFF_BOLT        5
#define EFF_CORONA      6
// changed RF064
#define EFF_ACTORSPRAY  7
// end change RF064

#define EFFECTC_CLIP_LEAF			1 //( 1 << 0 )
#define EFFECTC_CLIP_LINEOFSIGHT	2 //( 1 << 1 )
#define EFFECTC_CLIP_SEMICIRCLE		4 //( 1 << 2 )

#define SPRAY_SOURCE		1 //( 1 << 0 )
#define SPRAY_DEST			2 //( 1 << 1 )
#define SPRAY_FOLLOWTAIL	4 //( 1 << 2 )
#define SPRAY_ACTUALDEST	8 //( 1 << 3 )

typedef struct
{
	geVec3d		*ParticleGravity;	// RESERVED
	float		TimeRemaining;		// RESERVED
	float		PolyCount;			// RESERVED
	geXForm3d	Xf;					// RESERVED
	GE_LVertex	Vertex;				// RESERVED
	geBoolean	Paused;				// RESERVED, whether or not the sprite is paused
	int32		Leaf;				// RESERVED
	geBoolean	ShowAlways;			// RESERVED
	geBitmap	*Texture;			// texture to use
	float		SprayLife;			// life of effect
	float		Rate;				// add a new texture every "Rate" seconds
	geVec3d		*AnchorPoint;		// point to which particles are hooked to
	GE_RGBA		ColorMin;			// min values for each color
	GE_RGBA		ColorMax;			// max values for each color
	geVec3d		Gravity;			// gravity vector
	float		DistanceMax;		// distance past which the effect is not drawn
	float		DistanceMin;		// distance up to which no level of detail processing is done
	geVec3d		Source;				// source point
	int			SourceVariance;		// +/- units to vary the source point
	geVec3d		Dest;				// dest point
	geVec3d		Angle;
	geXForm3d	Xform;
	int			DestVariance;		// +/- units to vary the dest point
	float		MinScale;			// min scale for the art
	float		MaxScale;			// max scale for the art
	float		MinUnitLife;		// min life of each texture
	float		MaxUnitLife;		// max life of each texture
	float		MinSpeed;			// min speed of each texture
	float		MaxSpeed;			// max speed of each texture
	geBoolean	Bounce;

} Spray;

#define GLOW_POS		1 //( 1 << 0 )
#define GLOW_RADIUSMIN	2 //( 1 << 1 )
#define GLOW_RADIUSMAX	4 //( 1 << 2 )
#define GLOW_INTENSITY	8 //( 1 << 3 )

typedef struct
{
	geLight		*Light;			// RESERVED
	int32		Leaf;			// RESERVED
	float		LightLife;
	geVec3d		Pos;			// vector which light will follow
	float		RadiusMin;		// min light radius
	float		RadiusMax;		// max light radius
	GE_RGBA		ColorMin;		// min color info
	GE_RGBA		ColorMax;		// max color info
	float		Intensity;		// light intensity
	geBoolean	DoNotClip;		// whether or not clipping should be ignored
	geBoolean	CastShadows;	// whether or not the light should cast shadows
// change QD
	geBoolean	Spot;			// is it a spotlight?
	geFloat		Arc;			// angle of lightcone
	geVec3d		Direction;		// direction of spotlight
	int			Style;			// falloff style
// end change QD

} Glow;


#define SPRITE_POS		 1 //( 1 << 0 )
#define SPRITE_SCALE	 2 //( 1 << 1 )
#define SPRITE_ROTATION	 4 //( 1 << 2 )
#define SPRITE_COLOR	 8 //( 1 << 3 )
#define SPRITE_PAUSE	16 //( 1 << 4 )

typedef enum
{
	SPRITE_CYCLE_NONE,
	SPRITE_CYCLE_RESET,
	SPRITE_CYCLE_REVERSE,
	SPRITE_CYCLE_RANDOM,
	SPRITE_CYCLE_ONCE

} Sprite_CycleStyle;

typedef struct
{
	GE_LVertex	Vertex[4];		// RESERVED, vert info
	int32		Direction;		// RESERVED
	float		ElapsedTime;	// RESERVED
	int32		CurrentTexture;	// RESERVED
	int32		Leaf;			// RESERVED
	geBoolean	Pause;			// RESERVED, whether or not the sprite is paused
	geBoolean	ShowAlways;		// RESERVED
	geVec3d		Pos;			// location
	GE_RGBA		Color;			// color
	geBitmap	**Texture;		// list of textures to use
	int32		TotalTextures;	// total number of textures in the list
	float		TextureRate;	// every how many seconds to switch to the next texture
	float		Scale;			// how to scale the art
	float		ScaleRate;		// how much to subtract from scale each second
	float		RotationRate;	// how much to add to art rotation each second (radians)
	float		AlphaRate;		// how much to subtract from alpha each second
	Sprite_CycleStyle	Style;	// how to cycle through the images
	float		Rotation;		// art rotation amount (radians)
// changed RF064
	float		LifeTime;
	float		CurrentLife;
// end change RF064

} Sprite;

#define SND_POS					1 //( 1 << 0 )
#define SND_MINAUDIBLEVOLUME	0.1f

typedef struct
{
	geSound		*Sound;			// RESERVED, pointer to the active sound
	geBoolean	Paused;
	geFloat		LastVolume;		// RESERVED, its volume the last time it was modified
	geFloat		LastPan;		// RESERVED, its pan the last time it was modified
	geSound_Def	*SoundDef;		// sound def to play from
	geVec3d		Pos;			// location of the sound
	geFloat		Min;			// min distance whithin which sound is at max volume
	geBoolean	Loop;			// whether or not to loop it

} Snd;

#define BOLT_START		1 //( 1 << 0 )
#define BOLT_END		2 //( 1 << 1 )
#define BOLT_COLOR		4 //( 1 << 2 )
#define BOLT_ENDOFFSET	8 //( 1 << 3 )

typedef struct EBolt
{
	float		CompleteLife;
	geFloat		LastTime;
	geFloat		LastBoltTime;
	Electric_BoltEffect	*Bolt;
	geBitmap	*Bitmap;
	geVec3d		Start;
	geVec3d		End;
	geVec3d		EndOffset;
	geXForm3d	Xform;
	int			Width;
	int			NumPoints;
	geBoolean	Intermittent;
	geFloat		MinFrequency;
	geFloat		MaxFrequency;
	geFloat		Wildness;
	int			DominantColor;
	GE_RGBA		Color;
} EBolt;


#define CORONA_POS	1 //( 1 << 0 )

typedef struct
{
	float		LastVisibleRadius;	// RESERVED, last visible radius
	int32		Leaf;				// RESERVED, leaf it resides in
	geBoolean	Paused;				// RESERVED, whether or not the sprite is paused
	geFloat		LifeTime;
	geBitmap	*Texture;			// texture to use
	GE_LVertex	Vertex;				// location and color info
	geFloat		FadeTime;			// how many seconds to spend fading away the corona
	float		MinRadius;			// mix corona radius
	float		MaxRadius;			// max corona radius
	float		MaxRadiusDistance;	// above this distance, corona is capped at MaxRadius
	float		MinRadiusDistance;	// below this distance, corona is capped at MinRadius
	float		MaxVisibleDistance;	// beyond this distance the corona is not visible

} EffCorona;

// changed RF064

typedef struct
{
	float		TimeRemaining;		// RESERVED
	float		PolyCount;			// RESERVED
	geXForm3d	Xf;					// RESERVED
	geBoolean	Paused;				// RESERVED, whether or not the sprite is paused
	int			ActorNum;			// RESERVED
	bool		Direction;
	int			NumActors;
	char		BaseName[64];
	geVec3d		BaseRotation;
	geVec3d		MinRotationSpeed;
	geVec3d		MaxRotationSpeed;
	GE_RGBA		FillColor;
	GE_RGBA		AmbientColor;
// changed QD 07/21/04
	geBoolean	AmbientLightFromFloor;
// end change
	int			Style;
	float		Alpha;
	float		AlphaRate;
	float		SprayLife;			// life of effect
	float		Rate;				// add a new actor every "Rate" seconds
	bool		Gravity;			// gravity flag
	float		DistanceMax;		// distance past which the effect is not drawn
	float		DistanceMin;		// distance up to which no level of detail processing is done
	geVec3d		Source;				// source point
	int			SourceVariance;		// +/- units to vary the source point
	geVec3d		Dest;				// dest point
	geVec3d		Angle;
	geXForm3d	Xform;
	int			DestVariance;		// +/- units to vary the dest point
	float		MinScale;			// min scale for the art
	float		MaxScale;			// max scale for the art
	float		MinUnitLife;		// min life of each actor
	float		MaxUnitLife;		// max life of each actor
	float		MinSpeed;			// min speed of each actor
	float		MaxSpeed;			// max speed of each actor
	geBoolean	Bounce;
	geBoolean	Solid;
	geBoolean	EnvironmentMapping;
	geBoolean	AllMaterial;
	geFloat		PercentMapping;
	geFloat		PercentMaterial;

} ActorSpray;

typedef	struct	ActorParticle_System	ActorParticle_System;
// end change RF064

typedef	struct	Particle_System	Particle_System;

typedef struct Eff_Item
{
	geBoolean	Active;
	int			Type;
	void		*Data;
	geBoolean	Pause;
	geBoolean	RemoveNext;

} Eff_Item;

class EffManager : public CRGFComponent
{
public:
	EffManager();
	~EffManager();

	void Tick(float dwTicks);
	int Item_Add(int Itype, void *Idata);
	void Item_Modify(int Itype, int Index, void *Data, uint32 Flags);
	void Item_Delete(int Itype, int Index);
	void Item_Pause(int Itype, int Index, geBoolean Flag);
	// changed RF063
	bool Item_Alive(int Index);

private:
	// SPRAY
	void *Spray_Add(void *Data);
	void Spray_Remove(Spray *Data);
	geBoolean Spray_Process(Spray  *Data, float TimeDelta);
	geBoolean Spray_Modify(Spray *Data, Spray *NewData, uint32 Flags);

	// GLOW
	void *Glow_Add(void *Data);
	void Glow_Remove(Glow *Data);
	geBoolean Glow_Process(Glow  *Data, float TimeDelta);
	geBoolean Glow_Modify(Glow *Data, Glow *NewData, uint32 Flags);

	// SPRITE
	void *Sprite_Add(void *Data);
	void Sprite_Remove(Sprite *Data);
	geBoolean Sprite_Process(Sprite  *Data, float TimeDelta);
	geBoolean Sprite_Modify(Sprite *Data, Sprite *NewData, uint32 Flags);

	// SOUND
	void *Snd_Add(void *Data);
	void Snd_Remove(Snd *Data);
	geBoolean Snd_Process(Snd *Data, float TimeDelta);
	geBoolean Snd_Modify(Snd *Data, Snd *NewData, uint32 Flags);

	// BOLT
	void *Bolt_Add(void *Data);
	void Bolt_Remove(EBolt *Data);
	geBoolean Bolt_Process(EBolt  *Data, float TimeDelta);
	geBoolean Bolt_Modify(EBolt *Data, EBolt *NewData, uint32 Flags);
	void Bolt_Pause(EBolt *Data, geBoolean Pause );

	// CORONA
	void *Corona_Add(void *Data);
	void Corona_Remove(EffCorona *Data);
	geBoolean Corona_Process(EffCorona *Data, float TimeDelta);
	geBoolean Corona_Modify(EffCorona *Data, EffCorona *NewData, uint32 Flags);

	// changed RF064
	// ACTORSPRAY
	void *ActorSpray_Add(void *Data);
	void ActorSpray_Remove(ActorSpray *Data);
	geBoolean ActorSpray_Process(ActorSpray *Data, float TimeDelta);
	geBoolean ActorSpray_Modify(ActorSpray *Data, ActorSpray *NewData, uint32 Flags);
	// end change RF064

	Eff_Item Item[MAX_EFF_ITEMS];
	Particle_System	*Ps;
	// changed RF064
	ActorParticle_System *APs;
	// end change RF064
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
