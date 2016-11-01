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

#include <hash_map>
/**
 * @brief Projectile definition
 */
typedef struct Projectile
{
	Projectile*		next;
	Projectile*		prev;
	geActor*		Actor;
	geSound_Def*	BounceSoundDef;
	geSound_Def*	MoveSoundDef;
	geSound_Def*	ImpactSoundDef;
	geVec3d			Pos;
	geVec3d			Angles;
	geVec3d			In;
	geVec3d			Rotation;
	geVec3d			Direction;
	geExtBox		ExtBox;
	float			LifeTime;
	float			ShakeAmt;
	float			ShakeDecay;
	float			Damage;
	float			AltDamage;
	float			RadiusDamage;
	float			Radius;
	int				MoveSoundEffect;
	int				Decal;
	int				Effect[5];
	int				EffectType[5];
	bool			Gravity;
	bool			Bounce;
	bool			ShowBoth;
	bool			AttachActor;
	bool			BoneLevel;
	std::string		EffectBone[5];
	std::string		Explosion;
	std::string		ActorExplosion;
	std::string		Attribute;
	std::string		AltAttribute;
	std::string		Name;

} Projectile;

/**
 * @brief Projectile definition
 */
typedef struct ProjectileDefinition
{
	geVec3d		Rotation;
	float		LifeTime;
	float		ShakeAmt;
	float		ShakeDecay;
	float		Damage;
	float		AltDamage;
	float		RadiusDamage;
	float		Radius;
	float		Scale;
	float		BoxSize;
	float		Speed;
	int			Decal;
	geBoolean	Gravity;
	geBoolean	Bounce;
	bool		ShowBoth;
	bool		AttachActor;
	bool		BoneLevel;
	std::string	EffectName[5];
	std::string	EffectBone[5];
	std::string	Explosion;
	std::string	ActorExplosion;
	std::string	BounceSound;
	std::string	MoveSound;
	std::string	ImpactSound;
	std::string	ActorFile;
	std::string	ActorAnimation;

} ProjectileDefinition;

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
typedef struct WeaponDefinition
{
	char		Name[256];
	int			Slot;
	float		FireRate;
	float		MaxDeviationRadius;
	WeaponType	Catagory;
	std::string	Projectile;
	std::string	AttackSound;
	std::string	HitSound;
	std::string	EmptySound;
	std::string	UseSound;
	std::string	MeleeExplosion;
	std::string	Attribute;
	std::string	AltAttribute;
	float		MeleeDamage;
	float		MeleeAltDamage;
	int			FixedView;
	int			ShotperMag;
	int			ShotFired;
	int			MagAmt;
	bool		LooseMag;
	bool		WorksUnderwater;
	std::string	MuzzleFlash;
	std::string	MuzzleFlash3rd;
	std::string	Ammunition;
	std::string	ReloadSound;
	int			AmmoPerShot;
	bool		MeleeAmmo;
	geBitmap*	CrossHair;
	bool		CrossHairFixed;
	bool		AllowLit;
	geVec3d		LitColor;
	int			ZoomAmt;
	geBitmap*	ZoomOverlay;
	bool		MoveZoom;

	char		DropActor[256];
	geVec3d		DropActorRotation;
	geVec3d		DropActorOffset;
	geVec3d		DropFillColor;
	geVec3d		DropAmbientColor;
	geBoolean	DropAmbientLightFromFloor;
	geBoolean	DropEnvironmentMapping;
	geBoolean	DropAllMaterial;
	geBoolean	DropGravity;
	geBoolean	DropHideFromRadar;
	float		DropPercentMapping;
	float		DropPercentMaterial;
	float		DropScale;

	geActor*	VActor;
	geActor_Def* VActorDef;
	geVec3d		VActorRotation;
	geVec3d		VActorOffset;
	float		VScale;
	float		VAnimSpeed;
	std::string	VArm;
	std::string	VIdle;
	std::string	VAttack;
	std::string	VAltAttack;
	std::string	VHit;
	std::string	VAltHit;
	std::string	VWalk;
	std::string	VReload;
	std::string	VKeyReload;
	std::string	VAttackEmpty;
	std::string	VUse;
	geVec3d		VOffset;
	geVec3d		VMOffset;
	char		VBone[256];
	float		JerkAmt;
	float		JerkDecay;
	float		BobAmt;

	geActor*	PActor;
	geActor_Def*PActorDef;
	geVec3d		PActorRotation;
	geVec3d		POffset;
	char		PBone[256];
	float		PScale;
	float		PMOffset;
	std::string	Animations[ANIMMAX];
	std::string	DieAnim[5];
	std::string	InjuryAnim[5];
	int			DieAnimAmt;
	int			InjuryAnimAmt;
	float		F, H, J;
	float		G, K, L, Z;

} WeaponDefinition;

/**
 * @brief Max # of weapons
 * @note if MAX_WEAPONS changes, size of Slot in CCommonData must change too
 */
#define MAX_WEAPONS		40

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
	VWEPRELOAD,
	VWEPKEYRELOAD,
	VWEPATTACKEMPTY,
	VWEPUSE,
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

	void Tick(float timeElapsed);

	/**
	 * @brief Correct internal timing to match current time, to make up for time
	 * lost when outside the game loop (typically in "menu mode").
	 */
	int ReSynchronize();

	void Display();

	void Holster();

	void DoAttack();

	int SaveTo(FILE* saveFD);

	int RestoreFrom(FILE* restoreFD);

	std::string DieAnim();

	std::string InjuryAnim();

	void SetWeapon(int slot);

	void SetWeapon(const std::string& weapon);

	void WeaponData();

	void Attack(bool Alternate);

	void AddProjectile(const geVec3d& pos, const geVec3d& front, const geVec3d& orient,
		const std::string& projName, const std::string& PAttribute, const std::string& PAltAttribute);

	bool CrossHair();

	bool CrossHairFixed();

	void DisplayCrossHair();

	geBitmap* GetCrossHair();

	int ZoomAmount();

	void DisplayZoom();

	void Rendering(bool flag);

	void ResetWeapon(const std::string& value);

	void ClearWeapon();

	const std::string& PlayerAnim(int index);

	const std::string& GetSlot(int index) const			{ return m_Slot[index];		}

	void SetSlot(int index, const std::string& flag)	{ m_Slot[index] = flag;		}

	const std::string& GetCurrent() const		{ return m_CurrentWeapon;	}

	void SetCurrent(const std::string& value)	{ m_CurrentWeapon = value;	}

	/**
	 * @brief Get 1st person actor of the currently active weapon
	 */
	geActor* GetVActor();

	/**
	 * @brief Get 3rd person actor of the currently active weapon
	 */
	geActor* GetPActor();

	geVec3d GetCrossPoint() const		{ return m_CrossPoint; }

	void KeyReload();

	void DropWeapon();

	void Use();

	void SetView(int value)				{	m_ViewPoint = value; m_OldViewPoint = value;	}

	const std::string& GetWeaponName() const	{	return m_CurrentWeapon;	}

	const std::string& GetWeaponAmmo()	{	if(m_CurrentWeapon.empty()) return m_CurrentWeapon;
											return m_Weapons[m_CurrentWeapon]->Ammunition;	}

	int GetMagSize()					{	if(m_CurrentWeapon.empty()) return 0;
											return m_Weapons[m_CurrentWeapon]->ShotperMag;	}

	int GetShotFired()					{	if(m_CurrentWeapon.empty()) return 0;
											return m_Weapons[m_CurrentWeapon]->ShotFired;	}

	int GetMagAmt()						{	if(m_CurrentWeapon.empty()) return 0;
											return m_Weapons[m_CurrentWeapon]->MagAmt;		}

	geVec3d GetLitColor()				{	if(m_CurrentWeapon.empty())
											{
												geVec3d LitColor = {255.0f, 255.0f, 255.0f};
												return LitColor;
											}
											return m_Weapons[m_CurrentWeapon]->LitColor;	}

	bool GetAllowLit()					{	if(m_CurrentWeapon.empty()) return false;
											return m_Weapons[m_CurrentWeapon]->AllowLit;	}

	bool GetAllowMoveZoom()				{	if(m_CurrentWeapon.empty()) return true;
											return m_Weapons[m_CurrentWeapon]->MoveZoom;	}

	bool GetAttackFlag() const			{	return m_AttackFlag;		}

	void SetAttackFlag(bool flag)		{	m_AttackFlag = flag;		}

	geVec3d GetProjectedPoint() const	{	return m_ProjectedPoint;	}

	int GetFixedView();

private:
	void DisplayThirdPerson(const std::string& weaponName);

	void DisplayFirstPerson(const std::string& weaponName);

	void Sound(bool attack, const geVec3d& origin, bool empty);

	void MeleeAttack();

	void ProjectileAttack();

	int PlaySound(geSound_Def* soundDef, const geVec3d& pos, bool loop);

	void LoadDefaults();

	void DoChange();

	void SpawnWeaponAttribute(const std::string& weaponName);

private:
	bool			m_DropFlag;
	bool			m_Holstered;
	bool			m_AttackFlag;
	bool			m_AltAttack;
	int				m_AttackTime;
	int				m_ViewPoint;
	int				m_OldViewPoint;

	int				m_VSequence;
	float			m_VAnimTime;
	float			m_VMCounter;
	float			m_VBScale;
	float			m_VBDiff;
	float			m_VBOrigin;
	geMotion*		m_VBactorMotion;
	bool			m_VBlend;
	bool			m_MFlash;
	geVec3d			m_CrossPoint;
	geVec3d			m_ProjectedPoint;
	Projectile*		m_Bottom;

	std::string		m_CurrentWeapon;
	std::string		m_Slot[MAX_WEAPONS];

	stdext::hash_map<std::string, ProjectileDefinition*> m_Projectiles;
	stdext::hash_map<std::string, WeaponDefinition*> m_Weapons;

};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
