/*
	CWeapon.h:		Weapon class handler

	(c) 1999 Edward A. Averill, III
	All Rights Reserved

	This file contains the class declaration for Weapon
handling.
*/

#ifndef __RGF_CWEAPON_H_
#define __RGF_CWEAPON_H_

typedef struct Proj
{
  Proj    	*next;
  Proj    	*prev;
  geActor *Actor;
  geVec3d Pos;
  geVec3d Angles;
  geVec3d In;
  geVec3d Rotation;
  geVec3d Direction;
  geExtBox ExtBox;
  bool Gravity;
  bool Bounce;
  geSound_Def *BounceSoundDef;
  geSound_Def *MoveSoundDef;
  geSound_Def *ImpactSoundDef;
  int MoveSoundEffect;
  int Effect[5];
  int EffectType[5];
  char *EffectBone[5];
  geFloat LifeTime;
  int Decal;
  char *Explosion;
  float Damage;
  float RadiusDamage;
  float Radius;
  char *Attribute;
} Proj;

typedef struct DefaultProj
{
  bool active;
  char Name[64];
  char Actorfile[64];
  geVec3d Rotation;
  char ActorAnimation[64];
  geBoolean Gravity;
  geBoolean Bounce;
  char BounceSound[64];
  char MoveSound[64];
  char ImpactSound[64];
  geFloat LifeTime;
  int Decal;
  char Explosion[64];
  float Damage;
  float RadiusDamage;
  float Radius;
  float Scale;
  float BoxSize;
  float Speed;
  char Effect[5][64];
  char EffectBone[5][64];
} DefaultProj;

typedef enum
{
	MELEE,
	PROJECTILE,
	BEAM
} WeaponType;

typedef struct DefaultWeapons
{
	bool active;
	char Name[64];
	int Slot;
	float FireRate;
	WeaponType Catagory;
	char Projectile[64];
	char AttackSound[64];
	char HitSound[64];
	char EmptySound[64];
	float MeleeDamage;
	char Attribute[64];
	char Ammunition[64];
	int AmmoPerShot;
	char MuzzleFlash[64];
	geBitmap *CrossHair;
	bool CrossHairFixed;

	geActor *VActor;
	geActor_Def	*VActorDef;
	geVec3d VActorRotation;
	geVec3d VActorOffset;
	float VScale;
	float VAnimSpeed;
	char VArm[64];
	char VIdle[64];
	char VAttack[64];
	char VAltAttack[64];
	char VHit[64];
	char VAltHit[64];
	char VWalk[64];
	geVec3d VOffset;
	char VBone[64];

	char PActorName[64];
	geVec3d PActorRotation;
	geVec3d POffset;
	float PScale;

	geFloat F, H, J;
	geFloat G, K, L, Z;
} DefaultWeapons;

#define MAX_WEAPONS		40
#define MAX_PROJD       50

typedef enum
{
	VWEPCHANGE = 0,
	VWEPIDLE,
	VWEPWALK,
	VWEPATTACK,
	VWEPALTATTACK,
	VWEPHIT,
	VWEPALTHIT
} VwepAction;

class CWeapon : public CRGFComponent
{
public:
  CWeapon();
  ~CWeapon();
  void Tick(float dwTicks);
  int ReSynchronize();
  void Display();
  void DoAttack();
  void SetWeapon(int value);
  void WeaponData();
  void Attack(bool Alternate);
  void Add_Projectile(geVec3d Pos, geVec3d Front, geVec3d Orient, char *Projectile, char *PAttribute);
  bool CrossHair();
  bool CrossHairFixed();
  void DisplayCrossHair();
  geBitmap *GetCrossHair();
  void ReSetWeapon(int value);
  void ClearWeapon();
  int GetSlot(int index)
  { return Slot[index];}
  void SetSlot(int index, int flag)
  { Slot[index] = flag;}
  int GetCurrent()
  {return CurrentWeapon;}
  void SetCurrent(int value)
  { CurrentWeapon = value;}
  geVec3d GetCrossPoint()
  { return CrossPoint; }
  void SetView(int value)
  { ViewPoint = value; }
private:
  void DisplayFirstPerson(int index);
  void Sound(bool Attack, geVec3d Origin, bool Empty);
  void MeleeAttack(geActor *theActor, geVec3d theRotation, geVec3d thePosition, bool player);
  void ProjectileAttack();
  int PlaySound(geSound_Def *SoundDef, geVec3d Pos, bool Loop);
  void LoadDefaults();

  int Slot[10];
  int ViewPoint;
  int CurrentWeapon;
  int AttackTime;
  bool AttackFlag;
  bool AltAttack;

  float VAnimTime;
  float VMCounter;
  int VSequence;
  bool	VBlend;
  geMotion *VBactorMotion;
  geFloat	VBScale;
  geFloat	VBDiff;
  geFloat	VBOrigin;
  bool MFlash;

	Proj *Bottom;
	DefaultProj ProjD[MAX_PROJD];
	DefaultWeapons WeaponD[MAX_WEAPONS];

	geVec3d CrossPoint;
};

#endif

// --------------------------------------------------------------------------
