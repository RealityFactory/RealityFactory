/************************************************************************************//**
 * @file Cweapon.h
 * @brief Weapon class handler
 *
 * This file contains the class declaration for Weapon handling.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_CWEAPON_H_
#define __RGF_CWEAPON_H_

/**
 * @brief Projectile definition
 */
typedef struct Proj
{
	Proj    	*next;
	Proj    	*prev;
	geActor		*Actor;
	char		*Name;
	geVec3d		Pos;
	geVec3d		Angles;
	geVec3d		In;
	geVec3d		Rotation;
	geVec3d		Direction;
	geExtBox	ExtBox;
	bool		Gravity;
	bool		 Bounce;
	geSound_Def *BounceSoundDef;
	geSound_Def *MoveSoundDef;
	geSound_Def *ImpactSoundDef;
	int			MoveSoundEffect;
	int			Effect[5];
	int			EffectType[5];
	char		*EffectBone[5];
	geFloat		LifeTime;
	int			Decal;
	char		*Explosion;
	char		*ActorExplosion;
// changed Rf064
	bool		ShowBoth;
	bool		AttachActor;
	bool		BoneLevel;
// end change RF064
	float		ShakeAmt;
	float		ShakeDecay;
	float		Damage;
	float		RadiusDamage;
	float		Radius;
	char		*Attribute;
// changed RF063
	float		AltDamage;
	char		*AltAttribute;
// end change RF063
} Proj;

/**
 * @brief Default Projectile definition
 */
typedef struct DefaultProj
{
	bool		active;
	char		Name[64];
	char		Actorfile[64];
	geVec3d		Rotation;
	char		ActorAnimation[64];
	geBoolean	Gravity;
	geBoolean	Bounce;
	char		BounceSound[64];
	char		MoveSound[64];
	char		ImpactSound[64];
	geFloat		LifeTime;
	int			Decal;
	char		Explosion[64];
	char		ActorExplosion[64];
// changed Rf064
	bool		ShowBoth;
	bool		AttachActor;
	bool		BoneLevel;
// end change RF064
	float		ShakeAmt;
	float		ShakeDecay;
	float		Damage;
// changed RF063
	float		AltDamage;
// end change RF063
	float		RadiusDamage;
	float		Radius;
	float		Scale;
	float		BoxSize;
	float		Speed;
	char		Effect[5][64];
	char		EffectBone[5][64];

} DefaultProj;

/**
 * @brief Weapon types
 */
typedef enum
{
	MELEE,
	PROJECTILE,
	BEAM

} WeaponType;

/**
 * @brief Default Weapon dfinition
 */
typedef struct DefaultWeapons
{
	bool		active;
	char		Name[64];
	int			Slot;
	float		FireRate;
	WeaponType	Catagory;
	char		Projectile[64];
	char		AttackSound[64];
	char		HitSound[64];
	char		EmptySound[64];
	char		UseSound[64];
	float		MeleeDamage;
	char		MeleeExplosion[64];
	char		Attribute[64];
// changed RF063
    char		AltAttribute[64];
	float		MeleeAltDamage;
	bool		WorksUnderwater;
// end change RF063
// changed RF064
	int			FixedView;
	int			ShotperMag;
	int			ShotFired;
	char		ReloadSound[64];
	bool		LooseMag;
	int			MagAmt;
	char		MuzzleFlash3rd[64];
// end change RF064
	char		Ammunition[64];
	int			AmmoPerShot;
	bool		MeleeAmmo;
	char		MuzzleFlash[64];
	geBitmap	*CrossHair;
	bool		CrossHairFixed;
	bool		AllowLit;
	geVec3d		LitColor;
	int			ZoomAmt;
	geBitmap	*ZoomOverlay;
	bool		MoveZoom;

	char		DropActor[128];
	geVec3d		DropActorRotation;
	geVec3d		DropActorOffset;
	float		DropScale;
	geVec3d		DropFillColor;
	geVec3d		DropAmbientColor;
	geBoolean	DropAmbientLightFromFloor;
	geBoolean	DropEnvironmentMapping;
	geBoolean	DropAllMaterial;
	float		DropPercentMapping;
	float		DropPercentMaterial;
	geBoolean	DropGravity;
	geBoolean	DropHideFromRadar;

	geActor		*VActor;
	geActor_Def	*VActorDef;
	geVec3d		VActorRotation;
	geVec3d		VActorOffset;
	float		VScale;
	float		VAnimSpeed;
	char		VArm[64];
	char		VIdle[64];
	char		VAttack[64];
	char		VAltAttack[64];
	char		VHit[64];
	char		VAltHit[64];
	char		VWalk[64];
// changed RF064
	char		VReload[64];
	char		VKeyReload[64];
	char		VAttackEmpty[64];
	char		VUse[64];
// end change RF064
	geVec3d		VOffset;
	geVec3d		VMOffset;
	char		VBone[64];
// changed RF063
	float		JerkAmt;
	float		JerkDecay;
	float		BobAmt;
// end change RF063

	geActor		*PActor;
	geActor_Def	*PActorDef;
	geVec3d		PActorRotation;
	geVec3d		POffset;
	char		PBone[64];
	float		PScale;
	char		Animations[ANIMMAX][64];
// changed RF063
	float		PMOffset;
	char		DieAnim[5][64];
	int			DieAnimAmt;
	char		InjuryAnim[5][64];
	int			InjuryAnimAmt;
// end change RF063
	geFloat		F, H, J;
	geFloat		G, K, L, Z;

} DefaultWeapons;

/**
 * @brief Max # of weapons
 * @note if MAX_WEAPONS changes, size of Slot in CCommonData must change too
 */
#define MAX_WEAPONS		40
#define MAX_PROJD       50

 /**
  * @brief Possible actions of 1st person weapons
  */
typedef enum
{
	VWEPCHANGE = 0,
	VWEPHOLSTER,
	VWEPIDLE,
	VWEPWALK,
	VWEPATTACK,
	VWEPALTATTACK,
// changed RF064
	VWEPRELOAD,
	VWEPKEYRELOAD,
	VWEPATTACKEMPTY,
	VWEPUSE,
// end change RF064
	VWEPHIT,
	VWEPALTHIT

} VwepAction;

/**
 * @brief CWeapon handles weapons and projectiles
 */
class CWeapon : public CRGFComponent
{
public:
	CWeapon();
	~CWeapon();

	void Tick(geFloat dwTicks);
	int ReSynchronize();
	void Display();
	void Holster();
	void DoAttack();

	// changed RF063
	int SaveTo(FILE *SaveFD);
	int RestoreFrom(FILE *RestoreFD);

	void ChangeWeapon(const char *name);
	char *DieAnim();
	char *InjuryAnim();
	// end change RF063

	void SetWeapon(int value);
	void WeaponData();
	void Attack(bool Alternate);
	// changed RF063
	void Add_Projectile(const geVec3d &Pos, const geVec3d &Front, const geVec3d &Orient,
						const char *Projectile, char *PAttribute, char *PAltAttribute);
	// end change RF063

	bool CrossHair();
	bool CrossHairFixed();
	void DisplayCrossHair();
	geBitmap *GetCrossHair();

	int ZoomAmount();
	void DisplayZoom();

	void Rendering(bool flag);
	void ReSetWeapon(int value);
	void ClearWeapon();
	char *PlayerAnim(int index);
	int GetSlot(int index)				{ return Slot[index];	}
	void SetSlot(int index, int flag)	{ Slot[index] = flag;	}
	int GetCurrent()					{ return CurrentWeapon;	}
	void SetCurrent(int value)			{ CurrentWeapon = value;}

	//start pickles Jul 04
	geActor	*GetVActor();
	geActor	*GetPActor();
	//end pickles Jul 04

	geVec3d GetCrossPoint()			{ return CrossPoint; }

	// changed RF064
	void KeyReload();
	void DropWeapon();
	void Use();
	void SetView(int value)			{ ViewPoint = value; OldViewPoint = value;	}
	const char *GetWeaponName()		{ return WeaponD[CurrentWeapon].Name;		}
	const char *GetWeaponAmmo()		{ return WeaponD[CurrentWeapon].Ammunition; }
	int GetMagSize()				{ return WeaponD[CurrentWeapon].ShotperMag;	}
	int GetShotFired()				{ return WeaponD[CurrentWeapon].ShotFired;	}
	int GetMagAmt()					{ return WeaponD[CurrentWeapon].MagAmt;		}
	geVec3d GetLitColor()			{ return WeaponD[CurrentWeapon].LitColor;	}
	bool GetAllowLit()				{ return WeaponD[CurrentWeapon].AllowLit;	}
	bool GetAllowMoveZoom()			{ return WeaponD[CurrentWeapon].MoveZoom;	}
	// end change RF064

	bool GetAttackFlag()			{ return AttackFlag;}
	void SetAttackFlag(bool flag)	{ AttackFlag = flag;}

	// changed RF063
	geVec3d GetProjectedPoint()		{ return ProjectedPoint; }
	int GetFixedView();
	// end change RF063

private:
	void DisplayThirdPerson(int index);
	void DisplayFirstPerson(int index);
	void Sound(bool Attack, const geVec3d &Origin, bool Empty);
	void MeleeAttack();
	void ProjectileAttack();
	int PlaySound(geSound_Def *SoundDef, const geVec3d &Pos, bool Loop);
	void LoadDefaults();
	// changed RF064
	void DoChange();
	// end change RF064
	void SpawnWeaponAttribute(int index);

private:
	// changed QD 12/15/05
	//int				Slot[10];
	int				Slot[MAX_WEAPONS];
	int				ViewPoint;
	// changed RF064
	int				OldViewPoint;
	bool			dropflag;
	// end change RF064
	int				CurrentWeapon;
	int				AttackTime;
	bool			AttackFlag;
	bool			AltAttack;

	float			VAnimTime;
	float			VMCounter;
	int				VSequence;
	bool			VBlend;
	geMotion		*VBactorMotion;
	geFloat			VBScale;
	geFloat			VBDiff;
	geFloat			VBOrigin;
	bool			MFlash;

	Proj			*Bottom;
	DefaultProj		ProjD[MAX_PROJD];
	DefaultWeapons	WeaponD[MAX_WEAPONS];

	geVec3d			CrossPoint;
	// changed RF063
	geVec3d			ProjectedPoint;
	// end change RF063
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
