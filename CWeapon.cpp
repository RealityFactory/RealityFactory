/************************************************************************************//**
 * @file CWeapon.cpp
 * @brief Weapon class implementation
 *
 * This file contains the class implementation for Weapon handling.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#include "RabidFramework.h"
#include "IniFile.h"
#include "CHeadsUpDisplay.h"
#include "CAttribute.h"
#include "CDamage.h"
#include "CDecal.h"
#include "CLiquid.h"

extern geSound_Def *SPool_Sound(const char *SName);
extern geBitmap *TPool_Bitmap(const char *DefaultBmp, const char *DefaultAlpha,
							  const char *BName, const char *AName);
extern "C" void	DrawBoundBox(geWorld *World, const geVec3d *Pos, const geVec3d *Min, const geVec3d *Max);

/* ------------------------------------------------------------------------------------ */
// Constructor
/* ------------------------------------------------------------------------------------ */
CWeapon::CWeapon()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	int i;

	for(i=0; i<MAX_WEAPONS; i++)
		Slot[i] = -1;

	for(i=0; i<MAX_PROJD; i++)
	{
		memset(&ProjD[i], 0, sizeof(DefaultProj));
		ProjD[i].active = false;
	}

	for(i=0; i<MAX_WEAPONS;i++)
	{
		memset(&WeaponD[i], 0, sizeof(DefaultWeapons));
		WeaponD[i].active = false;
		WeaponD[i].VAnimSpeed = 1.0f;
	}

	CurrentWeapon	= -1;
	Bottom			= (Proj *)NULL;
	AttackFlag		= false;
	AttackTime		= 0;
	AltAttack		= false;
	VAnimTime		= -1.0f;
	VMCounter		= 0.0f;
	VSequence		= VWEPIDLE;
	VBlend			= false;	// not used
	VBactorMotion	= NULL;		// not used
	VBScale			= 0.0f;		// not used
	VBDiff			= 0.0f;		// not used
	VBOrigin		= 0.0f;		// not used
	MFlash = false;
	dropflag = false;

	geVec3d_Clear(&CrossPoint); // not used
	geVec3d_Clear(&ProjectedPoint);

	//	Ok, check to see if there's a PlayerSetup around...
	pSet = geWorld_GetEntitySet(CCD->World(), "PlayerSetup");

	if(!pSet)
	{
		CCD->Log()->Error("No PlayerSetup found! Bad level");
		return;
	}

	pEntity = geEntity_EntitySetGetNextEntity(pSet, NULL);

	if(pEntity)
	{
		PlayerSetup *theStart = (PlayerSetup *)geEntity_GetUserData(pEntity);
		ViewPoint = theStart->LevelViewPoint;
		OldViewPoint = ViewPoint;
	}
	else
	{
		CCD->Log()->Error("Failed to locate PlayerSetup in world");
		return;
	}

	LoadDefaults();
}

/* ------------------------------------------------------------------------------------ */
// Destructor
/* ------------------------------------------------------------------------------------ */
CWeapon::~CWeapon()
{
	Proj *d, *next;

	d = Bottom;

	while(d != NULL)
	{
		next = d->prev;
		CCD->ActorManager()->RemoveActor(d->Actor);
		geActor_Destroy(&d->Actor);
		delete d;
		d = next;
	}

	Bottom = (Proj*)NULL;

	for(int i=0; i<MAX_WEAPONS; i++)
	{
		if(WeaponD[i].active)
		{
			if(WeaponD[i].VActorDef)
			{
				geActor_DefDestroy(&(WeaponD[i].VActorDef));
				WeaponD[i].VActorDef = NULL;
			}

			if(WeaponD[i].VActor)
			{
				geWorld_RemoveActor(CCD->World(), WeaponD[i].VActor);
				geActor_Destroy(&WeaponD[i].VActor);
				WeaponD[i].VActor = NULL;
			}

			if(WeaponD[i].CrossHair)
			{
				if(WeaponD[i].CrossHairFixed)
					geEngine_RemoveBitmap(CCD->Engine()->Engine(), WeaponD[i].CrossHair);
				else
					geWorld_RemoveBitmap(CCD->World(), WeaponD[i].CrossHair);

				geBitmap_Destroy(&(WeaponD[i].CrossHair));
				WeaponD[i].CrossHair = NULL;
			}

			if(WeaponD[i].ZoomOverlay)
			{
				geWorld_RemoveBitmap(CCD->World(), WeaponD[i].ZoomOverlay);
				geBitmap_Destroy(&WeaponD[i].ZoomOverlay);
				WeaponD[i].ZoomOverlay = NULL;
			}

			if(WeaponD[i].PActorDef)
			{
				geActor_DefDestroy(&(WeaponD[i].PActorDef));
				WeaponD[i].PActorDef = NULL;
			}

			if(WeaponD[i].PActor)
			{
				geWorld_RemoveActor(CCD->World(), WeaponD[i].PActor);
				geActor_Destroy(&(WeaponD[i].PActor));
				WeaponD[i].PActor = NULL;

			}
		}
	}
}

/* ------------------------------------------------------------------------------------ */
//	ChangeWeapon
/* ------------------------------------------------------------------------------------ */
void CWeapon::ChangeWeapon(const char *name)
{
	int i;

	for(i=0; i<MAX_WEAPONS; i++)
	{
		if(WeaponD[i].active)
		{
			if(!strcmp(WeaponD[i].Name, name))
			{
				SetWeapon(WeaponD[i].Slot);
				return;
			}
		}
	}
}

/* ------------------------------------------------------------------------------------ */
// Tick
/* ------------------------------------------------------------------------------------ */
void CWeapon::Tick(geFloat dwTicks)
{
	int i;

	for(i=0; i<MAX_WEAPONS; i++)
	{
		if(WeaponD[i].active)
		{
			CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());

			if(theInv->Has(WeaponD[i].Name))
			{
				if(theInv->Value(WeaponD[i].Name) > 0)
				{
					Slot[WeaponD[i].Slot] = i;

					if((CurrentWeapon == -1 || CurrentWeapon == MAX_WEAPONS) // if no weapon is set
						&& WeaponD[i].Slot == MAX_WEAPONS-1)	// and this is the default weapon
					{
						SetWeapon(WeaponD[i].Slot);			// then set this one as current weapon
					}
				}
			}
		}
	}

	if(CurrentWeapon == -1) // still no weapon?
	{
		for(i=0; i<MAX_WEAPONS; i++)
		{
			if(WeaponD[i].active)
			{
				CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());

				if(theInv->Has(WeaponD[i].Name))	// does this weapon exist in the inventory?
				{
					if(theInv->Value(WeaponD[i].Name) > 0) // do we have at least 1 of this type?
					{
						Slot[WeaponD[i].Slot] = i;			// fill the slot with this weapon (-1 = empty)

						if(CurrentWeapon == -1)				// no current weapon? use this one
						{
							SetWeapon(WeaponD[i].Slot);
						}
					}
				}
			}
		}
	}

	// take care of any projectiles that are active
	Proj *d, *next;

	d = Bottom;

	dwTicks *= 0.001f;

	while(d != NULL)
	{
		geVec3d	tempPos;
		geVec3d	tempPos1;
		geVec3d	Distance;
		GE_Collision Collision;
		geActor *Actor = NULL;
		geWorld_Model *Model = NULL;
		char BoneHit[64];

		BoneHit[0] = '\0';

		next = d->prev;
		CCD->ActorManager()->GetPosition(d->Actor, &tempPos);

		// Movement
		geVec3d_Scale(&(d->Direction), dwTicks, &Distance);
		geVec3d_Add(&tempPos, &Distance, &tempPos1);

		// LifeTime Checking
		bool Alive = true;

		d->LifeTime -= dwTicks;

		if(d->LifeTime <= 0.0f)
			Alive = false;

		// Bouncing
		if(d->Bounce && Alive)
		{
			float totalTravelled = 1.0f ;	// keeps track of fraction of path travelled (1.0=complete)
			float margin = 0.001f ;			// margin to be satisfied with (1.0 - margin == 1.0)
			int loopCounter = 0 ;			// safety valve for endless collisions in tight corners
			const int maxLoops = 10 ;		// max. nr of collisions this frame
			bool PSound = true;

			CCD->Collision()->IgnoreContents(false);
			CCD->Collision()->CheckLevel(RGF_COLLISIONLEVEL_1);

			while(CCD->Collision()->CheckForBoneCollision(&d->ExtBox.Min, &d->ExtBox.Max, tempPos, tempPos1, &Collision, d->Actor, BoneHit, d->BoneLevel, d))
			{
				// Process hit here
				int nHitType = CCD->Collision()->ProcessCollision(Collision, d->Actor, false);

				if(nHitType == kNoCollision)
					break;

				// impact damage
				geVec3d Zero = {0.0f, 0.0f, 0.0f};

				if(Collision.Actor)
				{
					CCD->Damage()->DamageActor(Collision.Actor, d->Damage, d->Attribute, d->AltDamage, d->AltAttribute, d->Name);

					if(d->AttachActor && !EffectC_IsStringNull(BoneHit) && d->BoneLevel)
						CCD->Explosions()->AddExplosion(d->ActorExplosion, Zero, Collision.Actor, BoneHit);
					else
						CCD->Explosions()->AddExplosion(d->ActorExplosion, Collision.Impact);
				}

				if(Collision.Model)
					CCD->Damage()->DamageModel(Collision.Model, d->Damage, d->Attribute, d->AltDamage, d->AltAttribute);

				if(PSound)
				{
					PlaySound(d->BounceSoundDef, Collision.Impact, false);
					PSound = false;
				}

				float ratio ;
				float elasticity = 1.3f ;
				float friction = 0.9f ; // loses (1 minus friction) of speed

				CollisionCalcRatio(Collision, tempPos, tempPos1, &ratio);
				CollisionCalcImpactPoint(Collision, tempPos, tempPos1, 1.0f, ratio, &tempPos1);
				CollisionCalcVelocityImpact(Collision, d->Direction, elasticity, friction, &(d->Direction));

				if(ratio >= 0)
					totalTravelled += (1.0f - totalTravelled) * ratio ;

				if(totalTravelled >= 1.0f - margin)
					break ;

				if(++loopCounter >= maxLoops) // safety check
					break ;
			}
		}

		// No Bouncing
		if(!d->Bounce && Alive)
		{
			CCD->Collision()->IgnoreContents(false);
			CCD->Collision()->CheckLevel(RGF_COLLISIONLEVEL_1);

/* 07/15/2004 Wendell Buckner
	BUG FIX - Bone Collisions fail because we expect to hit the bone immediately after hitting the
	overall bounding box. So tag the actor as being hit at the bounding box level and after that check ONLY
	the bone bounding boxes until the whatever hit the overall bounding box no longer exists.
			if(CCD->Collision()->CheckForBoneCollision(&d->ExtBox.Min, &d->ExtBox.Max,tempPos, tempPos1, &Collision, d->Actor, BoneHit, d->BoneLevel)) */
			if(CCD->Collision()->CheckForBoneCollision(&d->ExtBox.Min, &d->ExtBox.Max, tempPos, tempPos1, &Collision, d->Actor, BoneHit, d->BoneLevel, d))
			{
				// Handle collision here
				int nHitType = CCD->Collision()->ProcessCollision(Collision, d->Actor, false);

				if(nHitType != kNoCollision)
				{
					Alive = false;
					geVec3d_Add(&(Collision.Impact), &(Collision.Plane.Normal), &(d->Pos));
					Actor = Collision.Actor;
					Model = Collision.Model;

					if(nHitType == kCollideNoMove)
					{
						CCD->Decals()->AddDecal(d->Decal, &(Collision.Impact), &(Collision.Plane.Normal), Collision.Model);
					}
				}
			}
		}

		// Move and Pose
		if(Alive)
		{
			d->Pos = tempPos1;
			CCD->ActorManager()->Position(d->Actor, d->Pos);
			CCD->ActorManager()->GetBoundingBox(d->Actor, &d->ExtBox);

			if(d->MoveSoundEffect != -1)
			{
				Snd Sound;
				geVec3d_Copy(&(d->Pos), &(Sound.Pos));
				CCD->EffectManager()->Item_Modify(EFF_SND, d->MoveSoundEffect, (void*)&Sound, SND_POS);
			}

			for(int j=0; j<5; j++)
			{
				if(d->Effect[j] != -1)
				{
					geXForm3d thePosition;

					if(EffectC_IsStringNull(d->EffectBone[j]))
						geActor_GetBoneTransform(d->Actor, RootBoneName(d->Actor), &thePosition);
					else
					{
						if(!geActor_GetBoneTransform(d->Actor, d->EffectBone[j], &thePosition))
							geActor_GetBoneTransform(d->Actor, RootBoneName(d->Actor), &thePosition);
					}

					switch(d->EffectType[j])
					{
						case EFF_SPRAY:
							Spray	Sp;
							geVec3d_Copy(&(thePosition.Translation), &(Sp.Source));
							geVec3d_AddScaled(&(Sp.Source), &(d->In), 50.0f, &(Sp.Dest));
							CCD->EffectManager()->Item_Modify(EFF_SPRAY, d->Effect[j], (void*)&Sp, SPRAY_SOURCE | SPRAY_ACTUALDEST);
							break;
						case EFF_LIGHT:
							Glow  Gl;
							geVec3d_Copy(&(thePosition.Translation), &(Gl.Pos));
							CCD->EffectManager()->Item_Modify(EFF_LIGHT, d->Effect[j], (void*)&Gl, GLOW_POS);
							break;
						case EFF_SND:
							Snd Sound;
							geVec3d_Copy( &(thePosition.Translation), &( Sound.Pos) );
							CCD->EffectManager()->Item_Modify(EFF_SND, d->Effect[j], (void*)&Sound, SND_POS);
							break;
						case EFF_SPRITE:
							Sprite S;
							geVec3d_Copy( &(thePosition.Translation), &( S.Pos) );
							CCD->EffectManager()->Item_Modify(EFF_SPRITE, d->Effect[j], (void*)&S, SPRITE_POS);
							break;
						case EFF_CORONA:
							EffCorona C;
							C.Vertex.X = thePosition.Translation.X;
							C.Vertex.Y = thePosition.Translation.Y;
							C.Vertex.Z = thePosition.Translation.Z;
							CCD->EffectManager()->Item_Modify(EFF_CORONA, d->Effect[j], (void*)&C, CORONA_POS);
							break;
						case EFF_BOLT:
							EBolt Bl;
							geVec3d	Temp;
							geVec3d_Subtract( &(Bl.Start), &(Bl.End),&(Temp) );
							geVec3d_Copy( &(thePosition.Translation), &(Bl.Start) );
							geVec3d_Add( &(Bl.Start), &(Temp),&(Bl.End) );
							CCD->EffectManager()->Item_Modify(EFF_BOLT, d->Effect[j], (void*)&Bl, BOLT_START | BOLT_END);
							break;
					}
				}
			}
		}
		else // Delete when done
		{
			if(d->MoveSoundEffect != -1)
				CCD->EffectManager()->Item_Delete(EFF_SND, d->MoveSoundEffect);

			for(int j=0; j<5; ++j)
			{
				if(d->Effect[j] != -1)
				{
					CCD->EffectManager()->Item_Delete(d->EffectType[j], d->Effect[j]);
				}
			}

			PlaySound(d->ImpactSoundDef, d->Pos, false);

			// inflict damage
			geVec3d Zero = {0.0f, 0.0f, 0.0f};

			if(Actor)
			{
				CCD->Damage()->DamageActor(Actor, d->Damage, d->Attribute,
											d->AltDamage, d->AltAttribute, d->Name);

				if(d->AttachActor && !EffectC_IsStringNull(BoneHit) && d->BoneLevel)
					CCD->Explosions()->AddExplosion(d->ActorExplosion, Zero, Actor, BoneHit);
				else
				{
					CCD->Explosions()->AddExplosion(d->ActorExplosion, d->Pos);
				}

				if(d->ShowBoth)
					CCD->Explosions()->AddExplosion(d->Explosion, d->Pos);
			}
			else if(Model)
			{
				CCD->Damage()->DamageModel(Model, d->Damage, d->Attribute, d->AltDamage, d->AltAttribute);
				CCD->Explosions()->AddExplosion(d->Explosion, d->Pos);
			}
			else
			{
				CCD->Explosions()->AddExplosion(d->Explosion, d->Pos);
			}

			// create explosion here
			if(d->ShakeAmt>0.0f)
				CCD->CameraManager()->SetShake(d->ShakeAmt, d->ShakeDecay, d->Pos);


			// Handle explosion damage here
			if(d->RadiusDamage > 0.0f)
			{
				CCD->Damage()->DamageActorInRange(d->Pos, d->Radius, d->RadiusDamage, d->Attribute, d->RadiusDamage, d->Attribute, "Explosion");
				CCD->Damage()->DamageModelInRange(d->Pos, d->Radius, d->RadiusDamage, d->Attribute, d->RadiusDamage, d->Attribute);
			}

			if(Bottom == d)
				Bottom = d->prev;

			if(d->prev != NULL)
				d->prev->next = d->next;

			if(d->next != NULL)
				d->next->prev = d->prev;

/* 07/15/2004 Wendell Buckner
	BUG FIX - Bone Collisions fail because we expect to hit the bone immediately after hitting the
	overall bounding box. So tag the actor as being hit at the bounding box level and after that check ONLY
	the bone bounding boxes until the whatever hit the overall bounding box no longer exists. */
			CCD->ActorManager()->RemoveCollideObject(COLMaxBBox, NULL, (void*)d);

			CCD->ActorManager()->RemoveActor(d->Actor);
			geActor_Destroy(&d->Actor);
			delete d;
			d = NULL;
		}

		d = next;
	}
}

/* ------------------------------------------------------------------------------------ */
// Display
/* ------------------------------------------------------------------------------------ */
void CWeapon::Display()
{
	if(CCD->Player()->GetMonitorMode())
		return;

	if(CurrentWeapon == -1 || CurrentWeapon == MAX_WEAPONS)
		return;

	if(ViewPoint == FIRSTPERSON)
	{
		int index = CurrentWeapon;
		if(WeaponD[index].PActor)
			geWorld_SetActorFlags(CCD->World(), WeaponD[index].PActor, CCD->Player()->GetMirror());

		geWorld_SetActorFlags(CCD->World(), WeaponD[index].VActor, GE_ACTOR_RENDER_NORMAL);
		DisplayFirstPerson(index); // CurrentWeapon may be altered here!

		if(CCD->Player()->GetMirror()!=0 && WeaponD[index].PActor)
			DisplayThirdPerson(index);
	}
	else
	{
		if(WeaponD[CurrentWeapon].VActor)
			geWorld_SetActorFlags(CCD->World(), WeaponD[CurrentWeapon].VActor, 0);

		if(WeaponD[CurrentWeapon].PActor)
		{
			geWorld_SetActorFlags(CCD->World(), WeaponD[CurrentWeapon].PActor, GE_ACTOR_RENDER_NORMAL | GE_ACTOR_RENDER_MIRRORS);
			DisplayThirdPerson(CurrentWeapon);
		}
	}
}

/* ------------------------------------------------------------------------------------ */
// DisplayThirdPerson
/* ------------------------------------------------------------------------------------ */
void CWeapon::DisplayThirdPerson(int index)
{
	geXForm3d XForm;
	geVec3d theRotation;
	geVec3d thePosition;
	geMotion *ActorMotion;
	geMotion *ActorBMotion;

	geActor *Actor = CCD->Player()->GetActor();
	CCD->ActorManager()->GetRotate(Actor, &theRotation);
	CCD->ActorManager()->GetPosition(Actor, &thePosition);
	geActor_SetScale(WeaponD[index].PActor, WeaponD[index].PScale, WeaponD[index].PScale, WeaponD[index].PScale);

	geXForm3d_SetZRotation(&XForm, WeaponD[index].PActorRotation.Z + theRotation.Z);
	geXForm3d_RotateX(&XForm, WeaponD[index].PActorRotation.X + theRotation.X);
	geXForm3d_RotateY(&XForm, WeaponD[index].PActorRotation.Y + theRotation.Y);

	geXForm3d_Translate(&XForm, thePosition.X, thePosition.Y, thePosition.Z);

	ActorMotion = CCD->ActorManager()->GetpMotion(Actor);
	ActorBMotion = CCD->ActorManager()->GetpBMotion(Actor);

	if(ActorMotion)
	{
		if(CCD->ActorManager()->GetTransitionFlag(Actor))
		{
			geActor_SetPose(WeaponD[index].PActor, ActorMotion, 0.0f, &XForm);

			if(ActorBMotion)
			{
				float BM = (CCD->ActorManager()->GetBlendAmount(Actor)
					- CCD->ActorManager()->GetAnimationTime(Actor))
					/CCD->ActorManager()->GetBlendAmount(Actor);

				if(BM < 0.0f)
					BM = 0.0f;

				geActor_BlendPose(WeaponD[index].PActor, ActorBMotion,
					CCD->ActorManager()->GetStartTime(Actor), &XForm, BM);
			}
		}
		else
		{
			geActor_SetPose(WeaponD[index].PActor, ActorMotion, CCD->ActorManager()->GetAnimationTime(Actor), &XForm);

			if(ActorBMotion)
			{
				geActor_BlendPose(WeaponD[index].PActor, ActorBMotion,
					CCD->ActorManager()->GetAnimationTime(Actor), &XForm,
					CCD->ActorManager()->GetBlendAmount(Actor));
			}
		}
	}
	else
	{
		geActor_ClearPose(WeaponD[index].PActor, &XForm);
	}

	geFloat fAlpha;
	CCD->ActorManager()->GetAlpha(Actor, &fAlpha);
	geActor_SetAlpha(WeaponD[index].PActor, fAlpha);

	if(ViewPoint == FIRSTPERSON)
		return;

	geXForm3d Xf;
	geVec3d Front, Back;
	geVec3d Direction;//, Pos;
	geExtBox theBox;
	GE_Collision Collision;
	geFloat CurrentDistance;

	theBox.Min.X = theBox.Min.Y = theBox.Min.Z = -1.0f;
	theBox.Max.X = theBox.Max.Y = theBox.Max.Z = 1.0f;
	geActor *theActor = CCD->Player()->GetActor();
	CurrentDistance = 4000.0f;

	if(WeaponD[CurrentWeapon].PBone[0]!='\0')
		geActor_GetBoneTransform(WeaponD[CurrentWeapon].PActor, WeaponD[CurrentWeapon].PBone, &Xf);
	else
		geActor_GetBoneTransform(WeaponD[CurrentWeapon].PActor, RootBoneName(WeaponD[CurrentWeapon].PActor), &Xf);

	thePosition = Xf.Translation;
	CCD->ActorManager()->GetRotate(theActor, &theRotation);
	geVec3d CRotation, CPosition;
	CCD->CameraManager()->GetCameraOffset(&CPosition, &CRotation);

	geXForm3d_SetZRotation(&Xf, theRotation.Z);
	geXForm3d_RotateX(&Xf, theRotation.X-CCD->CameraManager()->GetTilt());
	geXForm3d_RotateY(&Xf, theRotation.Y);

	//Pos = Xf.Translation;
	geXForm3d_GetIn(&Xf, &Direction);
	geVec3d_AddScaled(&thePosition, &Direction, CurrentDistance, &Back);

	geVec3d_Copy(&thePosition, &Front);

	if(geWorld_Collision(CCD->World(), &theBox.Min, &theBox.Max, &Front,
		&Back, GE_VISIBLE_CONTENTS, GE_COLLIDE_ALL, 0, NULL, NULL, &Collision))
    {
		CurrentDistance = geVec3d_DistanceBetween(&Collision.Impact, &Front);

		if(CurrentDistance > 4000.0f)
			CurrentDistance = 4000.0f;

		geVec3d_AddScaled(&thePosition, &Direction, CurrentDistance, &Back);
	}

	geCamera_TransformAndProject(CCD->CameraManager()->Camera(), &Back, &ProjectedPoint);
}

/* ------------------------------------------------------------------------------------ */
// DisplayFirstPerson
/* ------------------------------------------------------------------------------------ */
void CWeapon::DisplayFirstPerson(int index)
{
	geVec3d theRotation;
	geVec3d thePosition;

	if(CCD->WeaponPosition()) // used to move actor around in setup mode
	{
		if(!CCD->Player()->GetAlterKey())
		{
			if((GetAsyncKeyState(0x69) & 0x8000) != 0)
				WeaponD[index].G += 0.01f;
			if((GetAsyncKeyState(0x66) & 0x8000) != 0)
				WeaponD[index].G -= 0.01f;

			if((GetAsyncKeyState(0x67) & 0x8000) != 0 && (GetAsyncKeyState(0x60) & 0x8000) == 0)
				WeaponD[index].F += 0.1f;
			if((GetAsyncKeyState(0x68) & 0x8000) != 0 && (GetAsyncKeyState(0x60) & 0x8000) == 0)
				WeaponD[index].F -= 0.1f;

			if((GetAsyncKeyState(0x64) & 0x8000) != 0 && (GetAsyncKeyState(0x60) & 0x8000) == 0)
				WeaponD[index].H += 0.1f;
			if((GetAsyncKeyState(0x65) & 0x8000) != 0 && (GetAsyncKeyState(0x60) & 0x8000) == 0)
				WeaponD[index].H -= 0.1f;

			if((GetAsyncKeyState(0x61) & 0x8000) != 0 && (GetAsyncKeyState(0x60) & 0x8000) == 0)
				WeaponD[index].J += 0.1f;
			if((GetAsyncKeyState(0x62) & 0x8000) != 0 && (GetAsyncKeyState(0x60) & 0x8000) == 0)
				WeaponD[index].J -= 0.1f;

			if((GetAsyncKeyState(0x67) & 0x8000) != 0 && (GetAsyncKeyState(0x60) & 0x8000) != 0)
				WeaponD[index].K += 0.1f;
			if((GetAsyncKeyState(0x68) & 0x8000) != 0 && (GetAsyncKeyState(0x60) & 0x8000) != 0)
				WeaponD[index].K -= 0.1f;

			if((GetAsyncKeyState(0x64) & 0x8000) != 0 && (GetAsyncKeyState(0x60) & 0x8000) != 0)
				WeaponD[index].L += 0.1f;
			if((GetAsyncKeyState(0x65) & 0x8000) != 0 && (GetAsyncKeyState(0x60) & 0x8000) != 0)
				WeaponD[index].L -= 0.1f;

			if((GetAsyncKeyState(0x61) & 0x8000) != 0 && (GetAsyncKeyState(0x60) & 0x8000) != 0)
				WeaponD[index].Z += 0.1f;
			if((GetAsyncKeyState(0x62) & 0x8000) != 0 && (GetAsyncKeyState(0x60) & 0x8000) != 0)
				WeaponD[index].Z -= 0.1f;
		}
		else
		{
			if((GetAsyncKeyState(0x51) & 0x8000) != 0)
				WeaponD[index].G += 0.01f;
			if((GetAsyncKeyState(0x57) & 0x8000) != 0)
				WeaponD[index].G -= 0.01f;

			if((GetAsyncKeyState(0x4A) & 0x8000) != 0 && (GetAsyncKeyState(0x59) & 0x8000) == 0)
				WeaponD[index].F += 0.1f;
			if((GetAsyncKeyState(0x4B) & 0x8000) != 0 && (GetAsyncKeyState(0x59) & 0x8000) == 0)
				WeaponD[index].F -= 0.1f;

			if((GetAsyncKeyState(0x4D) & 0x8000) != 0 && (GetAsyncKeyState(0x59) & 0x8000) == 0)
				WeaponD[index].H += 0.1f;
			if((GetAsyncKeyState(0x4E) & 0x8000) != 0 && (GetAsyncKeyState(0x59) & 0x8000) == 0)
				WeaponD[index].H -= 0.1f;

			if((GetAsyncKeyState(0x4F) & 0x8000) != 0 && (GetAsyncKeyState(0x59) & 0x8000) == 0)
				WeaponD[index].J += 0.1f;
			if((GetAsyncKeyState(0x50) & 0x8000) != 0 && (GetAsyncKeyState(0x59) & 0x8000) == 0)
				WeaponD[index].J -= 0.1f;

			if((GetAsyncKeyState(0x4A) & 0x8000) != 0 && (GetAsyncKeyState(0x59) & 0x8000) != 0)
				WeaponD[index].K += 0.1f;
			if((GetAsyncKeyState(0x4B) & 0x8000) != 0 && (GetAsyncKeyState(0x59) & 0x8000) != 0)
				WeaponD[index].K -= 0.1f;

			if((GetAsyncKeyState(0x4D) & 0x8000) != 0 && (GetAsyncKeyState(0x59) & 0x8000) != 0)
				WeaponD[index].L += 0.1f;
			if((GetAsyncKeyState(0x4E) & 0x8000) != 0 && (GetAsyncKeyState(0x59) & 0x8000) != 0)
				WeaponD[index].L -= 0.1f;

			if((GetAsyncKeyState(0x4F) & 0x8000) != 0 && (GetAsyncKeyState(0x59) & 0x8000) != 0)
				WeaponD[index].Z += 0.1f;
			if((GetAsyncKeyState(0x50) & 0x8000) != 0 && (GetAsyncKeyState(0x59) & 0x8000) != 0)
				WeaponD[index].Z -= 0.1f;
		}
	}

	geXForm3d XForm;
	geXForm3d_SetIdentity(&XForm);

	CCD->CameraManager()->TrackMotion();

	CCD->CameraManager()->GetRotation(&theRotation);

	if(CCD->WeaponPosition())
	{
		geActor_SetScale(WeaponD[index].VActor,
			WeaponD[index].VScale+WeaponD[index].G,
			WeaponD[index].VScale+WeaponD[index].G,
			WeaponD[index].VScale+WeaponD[index].G);
		geXForm3d_RotateX(&XForm, WeaponD[index].VActorRotation.X + WeaponD[index].K*GE_PIOVER180);
		geXForm3d_RotateZ(&XForm, WeaponD[index].VActorRotation.Z + WeaponD[index].L*GE_PIOVER180);
		geXForm3d_RotateY(&XForm, WeaponD[index].VActorRotation.Y + WeaponD[index].Z*GE_PIOVER180);
	}
	else
	{
		geActor_SetScale(WeaponD[index].VActor,
			WeaponD[index].VScale,
			WeaponD[index].VScale,
			WeaponD[index].VScale);
		// rotate actor into place
		geXForm3d_RotateX(&XForm, WeaponD[index].VActorRotation.X);
		geXForm3d_RotateZ(&XForm, WeaponD[index].VActorRotation.Z);
		geXForm3d_RotateY(&XForm, WeaponD[index].VActorRotation.Y);
	}

	geXForm3d_RotateX(&XForm, theRotation.Z);						// rotate left/right
	geXForm3d_RotateZ(&XForm, theRotation.X - 0.87266462599715f);	// rotate up/down
	geXForm3d_RotateY(&XForm, theRotation.Y + GE_PIOVER2);			// swing side to side

	CCD->CameraManager()->GetPosition(&thePosition);
	float shx, shy, shz;
	CCD->CameraManager()->GetShake(&shx, &shy);
	shz = 0.0f;

	if(CCD->Player()->GetHeadBobbing())
	{
		if(WeaponD[index].BobAmt > 0.0f)
			shz = CCD->CameraManager()->GetHeadBobOffset()/WeaponD[index].BobAmt;
	}

	geXForm3d_Translate(&XForm, thePosition.X+shx, thePosition.Y+shy, thePosition.Z);

	geVec3d Forward;
	geVec3d Right;
	geVec3d Down;

	geXForm3d_GetLeft(&XForm, &Forward);
	geXForm3d_GetUp(&XForm, &Right);
	geXForm3d_GetIn(&XForm, &Down);

	if(CCD->WeaponPosition())
	{
		geVec3d_Scale(&Forward, WeaponD[index].VActorOffset.X + WeaponD[index].F, &Forward);	// in
		geVec3d_Scale(&Right,	WeaponD[index].VActorOffset.Y + WeaponD[index].H, &Right);		// up
		geVec3d_Scale(&Down,	WeaponD[index].VActorOffset.Z + WeaponD[index].J, &Down);		// left
	}
	else
	{
		geVec3d_Scale(&Forward, WeaponD[index].VActorOffset.X, &Forward);	// in
		geVec3d_Scale(&Right,	WeaponD[index].VActorOffset.Y, &Right);		// up
		geVec3d_Scale(&Down,	WeaponD[index].VActorOffset.Z, &Down);		// left
	}

	geXForm3d_Translate(&XForm, Forward.X,	Forward.Y,	Forward.Z);
	geXForm3d_Translate(&XForm, Right.X,	Right.Y,	Right.Z);
	geXForm3d_Translate(&XForm, Down.X,		Down.Y,		Down.Z);

	geXForm3d_GetIn(&XForm, &Forward);
	geVec3d_AddScaled(&XForm.Translation, &Forward, shz, &XForm.Translation);

	geFloat deltaTime, tStart, tEnd; //, bStart, bEnd;
	geMotion *ActorMotion;

	if(VAnimTime < 0.0f)
		VAnimTime = (float)CCD->FreeRunningCounter();
	deltaTime = 0.001f * (geFloat)(CCD->FreeRunningCounter() - VAnimTime);
	if(VSequence == VWEPHOLSTER)
		VMCounter -= (deltaTime*WeaponD[index].VAnimSpeed);
	else
		VMCounter += (deltaTime*WeaponD[index].VAnimSpeed);

	if(VSequence == VWEPCHANGE)
		ActorMotion = geActor_GetMotionByName(WeaponD[index].VActorDef, WeaponD[index].VArm);

	if(VSequence == VWEPHOLSTER)
		ActorMotion = geActor_GetMotionByName(WeaponD[index].VActorDef, WeaponD[index].VArm);

	if(VSequence == VWEPRELOAD)
		ActorMotion = geActor_GetMotionByName(WeaponD[index].VActorDef, WeaponD[index].VReload);

	if(VSequence == VWEPKEYRELOAD)
		ActorMotion = geActor_GetMotionByName(WeaponD[index].VActorDef, WeaponD[index].VKeyReload);

	if(VSequence == VWEPATTACKEMPTY)
		ActorMotion = geActor_GetMotionByName(WeaponD[index].VActorDef, WeaponD[index].VAttackEmpty);

	if(VSequence == VWEPALTATTACK)
	{
		ActorMotion = geActor_GetMotionByName(WeaponD[index].VActorDef, WeaponD[index].VAltAttack);
		if(!ActorMotion)
		{
			ActorMotion = geActor_GetMotionByName(WeaponD[index].VActorDef, WeaponD[index].VAttack);
			VSequence = VWEPATTACK;
		}
	}

	if(VSequence == VWEPUSE)
		ActorMotion = geActor_GetMotionByName(WeaponD[index].VActorDef, WeaponD[index].VUse);

	if(VSequence == VWEPHIT)
		ActorMotion = geActor_GetMotionByName(WeaponD[index].VActorDef, WeaponD[index].VHit);

	if(VSequence == VWEPALTHIT)
	{
		ActorMotion = geActor_GetMotionByName(WeaponD[index].VActorDef, WeaponD[index].VAltHit);
		if(!ActorMotion)
		{
			ActorMotion = geActor_GetMotionByName(WeaponD[index].VActorDef, WeaponD[index].VHit);
			VSequence = VWEPHIT;
		}
	}

	if(VSequence == VWEPWALK)
		ActorMotion = geActor_GetMotionByName(WeaponD[index].VActorDef, WeaponD[index].VWalk);

	if(VSequence == VWEPIDLE)
		ActorMotion = geActor_GetMotionByName(WeaponD[index].VActorDef, WeaponD[index].VIdle);

	if(VSequence == VWEPATTACK)
		ActorMotion = geActor_GetMotionByName(WeaponD[index].VActorDef, WeaponD[index].VAttack);

	if(!ActorMotion) // no animation so use default
		ActorMotion = geActor_GetMotionByName(WeaponD[index].VActorDef, WeaponD[index].VIdle);

	// test if at end of animation
	if(ActorMotion)
	{
		geMotion_GetTimeExtents(ActorMotion, &tStart, &tEnd);

		if(VMCounter <= tStart && VSequence == VWEPHOLSTER) // finish holster
		{
			CurrentWeapon = MAX_WEAPONS;
			geWorld_SetActorFlags(CCD->World(), WeaponD[index].VActor, 0);

			if(dropflag)
			{
				CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
				theInv->Modify(WeaponD[index].Name, -1);
				Slot[WeaponD[index].Slot] = -1;
				dropflag = false;
				SpawnWeaponAttribute(index);
			}
		}

		if(VMCounter > tEnd && VSequence == VWEPCHANGE) // switch from change to idle
		{
			VSequence = VWEPIDLE;
			ActorMotion = geActor_GetMotionByName(WeaponD[index].VActorDef, WeaponD[index].VIdle);
		}

		if(VMCounter > tEnd && (VSequence == VWEPATTACK || VSequence == VWEPATTACKEMPTY
			|| VSequence == VWEPALTATTACK || VSequence == VWEPHIT || VSequence == VWEPUSE
			|| VSequence == VWEPALTHIT || VSequence == VWEPRELOAD
			|| VSequence == VWEPKEYRELOAD)) // switch from shoot to idle
		{
			geActor *theActor = CCD->Player()->GetActor();
			CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(theActor);

			if(VSequence == VWEPKEYRELOAD || VSequence == VWEPRELOAD)
			{
				WeaponD[index].ShotFired = 0;

				if(theInv->Value(WeaponD[index].Ammunition)>=WeaponD[index].ShotperMag)
					WeaponD[index].MagAmt = WeaponD[index].ShotperMag;
				else
					WeaponD[index].MagAmt = theInv->Value(WeaponD[index].Ammunition);
			}

			bool magflag = false;

			if(WeaponD[index].Catagory == PROJECTILE && VSequence == VWEPATTACK)
			{
				if(theInv->Value(WeaponD[index].Ammunition) >= WeaponD[index].AmmoPerShot)
				{
					if(WeaponD[index].ShotperMag > 0 && WeaponD[index].ShotFired >= WeaponD[index].ShotperMag)
					{
						magflag = true;
						VSequence = VWEPRELOAD;
						ActorMotion = geActor_GetMotionByName(WeaponD[index].VActorDef, WeaponD[index].VReload);

						if(!EffectC_IsStringNull(WeaponD[CurrentWeapon].ReloadSound))
						{
							Snd Sound;
							memset(&Sound, 0, sizeof(Sound));
							CCD->CameraManager()->GetPosition(&Sound.Pos);
							Sound.Min = CCD->GetAudibleRadius();
							Sound.Loop = GE_FALSE;
							Sound.SoundDef = SPool_Sound(WeaponD[CurrentWeapon].ReloadSound);
							CCD->EffectManager()->Item_Add(EFF_SND, (void*)&Sound);
						}
					}
				}
			}

			if(!magflag)
			{
				if(CCD->Player()->GetMoving() == MOVEIDLE)
				{
					VSequence = VWEPIDLE;
					ActorMotion = geActor_GetMotionByName(WeaponD[index].VActorDef, WeaponD[index].VIdle);
				}
				else
				{
					VSequence = VWEPWALK;
					ActorMotion = geActor_GetMotionByName(WeaponD[index].VActorDef, WeaponD[index].VWalk);
				}
			}
			VBlend = false;
		}

		if(VMCounter > tEnd)
			VMCounter = 0.0f; // Wrap animation

		if(VSequence == VWEPWALK && VMCounter == 0.0f && (CCD->Player()->GetMoving() == MOVEIDLE))
		{
			VSequence = VWEPIDLE;
			VBlend = false; // all done blending
		}

		if(VSequence == VWEPIDLE && VMCounter == 0.0f && (CCD->Player()->GetMoving() != MOVEIDLE))
		{
			VSequence = VWEPWALK;
			VBlend = false; // all done blending
		}

		// set actor animation

		geActor_SetPose(WeaponD[index].VActor, ActorMotion, VMCounter, &XForm);
	}
	else
	{
		geActor_ClearPose(WeaponD[index].VActor, &XForm);
	}

	VAnimTime = (float)CCD->FreeRunningCounter();
}

/* ------------------------------------------------------------------------------------ */
// Rendering
/* ------------------------------------------------------------------------------------ */
void CWeapon::Rendering(bool flag)
{
	if(CurrentWeapon == -1 || CurrentWeapon == MAX_WEAPONS)
		return;

	if(ViewPoint == FIRSTPERSON)
	{
		if(!flag)
		{
			if(WeaponD[CurrentWeapon].VActor)
				geWorld_SetActorFlags(CCD->World(), WeaponD[CurrentWeapon].VActor, 0);
		}
		else
		{
			if(WeaponD[CurrentWeapon].VActor)
			{
				geWorld_SetActorFlags(CCD->World(), WeaponD[CurrentWeapon].VActor, GE_ACTOR_RENDER_NORMAL);
			}
		}
	}
	else
	{
		if(!flag)
		{
			if(WeaponD[CurrentWeapon].PActor)
				geWorld_SetActorFlags(CCD->World(), WeaponD[CurrentWeapon].PActor, 0);
		}
		else
		{
			if(WeaponD[CurrentWeapon].PActor)
			{
				geWorld_SetActorFlags(CCD->World(), WeaponD[CurrentWeapon].PActor, GE_ACTOR_RENDER_NORMAL | GE_ACTOR_RENDER_MIRRORS);
			}
		}
	}
}

/* ------------------------------------------------------------------------------------ */
// Holster
/* ------------------------------------------------------------------------------------ */
void CWeapon::Holster()
{
	if(CurrentWeapon == -1 || CurrentWeapon == MAX_WEAPONS)
		return;

	if(WeaponD[CurrentWeapon].Slot == MAX_WEAPONS-1)
		return;

	if(WeaponD[CurrentWeapon].Catagory == PROJECTILE
		|| (WeaponD[CurrentWeapon].Catagory == MELEE && WeaponD[CurrentWeapon].MeleeAmmo == true))
		CCD->HUD()->ActivateElement(WeaponD[CurrentWeapon].Ammunition, false);

	CCD->CameraManager()->CancelZoom();

	if(ViewPoint == FIRSTPERSON)
	{
		if(VSequence == VWEPHOLSTER)
			return;

		geMotion *ActorMotion = ActorMotion = geActor_GetMotionByName(WeaponD[CurrentWeapon].VActorDef, WeaponD[CurrentWeapon].VArm);

		if(ActorMotion)
		{
			geFloat tStart, tEnd;
			geMotion_GetTimeExtents(ActorMotion, &tStart, &tEnd);
			VMCounter = tEnd;
			VSequence = VWEPHOLSTER;
			VAnimTime = -1.0f;
			AttackTime = 0;
			VBlend = false;
			AttackFlag = false;
		}
		else
		{
			CurrentWeapon = MAX_WEAPONS;
		}
	}
	else
	{
			AttackFlag = false;
			geWorld_SetActorFlags(CCD->World(), WeaponD[CurrentWeapon].PActor, 0);

			if(dropflag)
			{
				CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
				theInv->Modify(WeaponD[CurrentWeapon].Name, -1);
				Slot[WeaponD[CurrentWeapon].Slot] = -1;
				dropflag = false;
				SpawnWeaponAttribute(CurrentWeapon);
			}

			CurrentWeapon = MAX_WEAPONS;
	}
}

/* ------------------------------------------------------------------------------------ */
// Use
/* ------------------------------------------------------------------------------------ */
void CWeapon::Use()
{
	if(CurrentWeapon == -1 || CurrentWeapon == MAX_WEAPONS)
		return;

	if(ViewPoint == FIRSTPERSON)
	{
		if(EffectC_IsStringNull(WeaponD[CurrentWeapon].VUse))
			return;

		if(VSequence == VWEPUSE)
			return;

		VSequence = VWEPUSE;
		VAnimTime = -1.0f;
		AttackTime = 0;
		VMCounter = 0.0f;
		VBlend = false;
		AttackFlag = false;

		if(!EffectC_IsStringNull(WeaponD[CurrentWeapon].UseSound))
		{
			Snd Sound;
			geVec3d Origin = CCD->Player()->Position();
			memset(&Sound, 0, sizeof(Sound));
			geVec3d_Copy(&Origin, &(Sound.Pos));
			Sound.Min = CCD->GetAudibleRadius();
			Sound.Loop = GE_FALSE;
			Sound.SoundDef = SPool_Sound(WeaponD[CurrentWeapon].UseSound);

			if(Sound.SoundDef)
				CCD->EffectManager()->Item_Add(EFF_SND, (void*)&Sound);
		}
	}
	else
	{
	}
}

/* ------------------------------------------------------------------------------------ */
// DoAttack
/* ------------------------------------------------------------------------------------ */
void CWeapon::DoAttack()
{
	if(AttackFlag)
	{
		AttackFlag = false;

		if(WeaponD[CurrentWeapon].Catagory == PROJECTILE)
		{
			ProjectileAttack();
		}
		else if(WeaponD[CurrentWeapon].Catagory == MELEE)
		{
			MeleeAttack();
		}
		else if(WeaponD[CurrentWeapon].Catagory == BEAM)
		{
		}
	}
}

/* ------------------------------------------------------------------------------------ */
// DisplayZoom
/* ------------------------------------------------------------------------------------ */
void CWeapon::DisplayZoom()
{
	if(CurrentWeapon == -1 || CurrentWeapon == MAX_WEAPONS)
		return;

	if(CCD->CameraManager()->GetZooming())
	{
		if(WeaponD[CurrentWeapon].ZoomOverlay)
		{
			CCD->Engine()->DrawAlphaBitmap(
									 WeaponD[CurrentWeapon].ZoomOverlay,
									 NULL,
									 NULL,	// if null, uses full screen
									 NULL,	// pixels in the "camera" view
									 NULL,// percent of the "camera" view
									 255.0f,
									 NULL, 1.0f);
		}
	}
}

/* ------------------------------------------------------------------------------------ */
// SaveTo
/* ------------------------------------------------------------------------------------ */
int CWeapon::SaveTo(FILE *SaveFD)
{
	fwrite(&ViewPoint,		sizeof(int),	1,	SaveFD);
	fwrite(&Slot,			sizeof(int),	10,	SaveFD);
	fwrite(&CurrentWeapon,	sizeof(int),	1,	SaveFD);

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// RestoreFrom
/* ------------------------------------------------------------------------------------ */
int CWeapon::RestoreFrom(FILE *RestoreFD)
{
	fread(&ViewPoint,		sizeof(int),	1,	RestoreFD);
	fread(&Slot,			sizeof(int),	10, RestoreFD);
	fread(&CurrentWeapon,	sizeof(int),	1,	RestoreFD);
	ReSetWeapon(CurrentWeapon);

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SetWeapon
//
// change to new weapon
/* ------------------------------------------------------------------------------------ */
void CWeapon::SetWeapon(int value)
{
	if(Slot[value] == -1)
		return;

	if(!(CurrentWeapon == -1 || CurrentWeapon == MAX_WEAPONS))
	{
		if(WeaponD[CurrentWeapon].Slot == value) // same weapon?
			return;

		// stop displaying old weapon
		if(WeaponD[CurrentWeapon].VActor)
			geWorld_SetActorFlags(CCD->World(), WeaponD[CurrentWeapon].VActor, 0);

		if(WeaponD[CurrentWeapon].PActor)
			geWorld_SetActorFlags(CCD->World(), WeaponD[CurrentWeapon].PActor, 0);

		if(WeaponD[CurrentWeapon].Catagory == PROJECTILE
			|| (WeaponD[CurrentWeapon].Catagory == MELEE && WeaponD[CurrentWeapon].MeleeAmmo == true))
			CCD->HUD()->ActivateElement(WeaponD[CurrentWeapon].Ammunition, false);
	}

	char *Motion = CCD->ActorManager()->GetMotion(CCD->Player()->GetActor());
	int index = -1;

	for(int i=0; i<ANIMMAX; ++i)
	{
		if(!strcmp(Motion, WeaponD[CurrentWeapon].Animations[i]))
		{
			index = i;
			break;
		}
	}

	CurrentWeapon = Slot[value]; // change to new weapon

	if(index != -1)
	{
		CCD->ActorManager()->SetMotion(CCD->Player()->GetActor(), PlayerAnim(index));
	}

	if(WeaponD[CurrentWeapon].Catagory == PROJECTILE
		|| (WeaponD[CurrentWeapon].Catagory == MELEE && WeaponD[CurrentWeapon].MeleeAmmo == true))
	{
		CCD->HUD()->ActivateElement(WeaponD[CurrentWeapon].Ammunition, true);

		if(WeaponD[CurrentWeapon].ShotperMag > 0)
		{
			if(WeaponD[CurrentWeapon].MagAmt == -1)
			{
				geActor *theActor = CCD->Player()->GetActor();
				CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(theActor);

				if(theInv->Value(WeaponD[CurrentWeapon].Ammunition) >= WeaponD[CurrentWeapon].ShotperMag)
					WeaponD[CurrentWeapon].MagAmt = WeaponD[CurrentWeapon].ShotperMag;
				else
					WeaponD[CurrentWeapon].MagAmt = theInv->Value(WeaponD[CurrentWeapon].Ammunition);
			}
		}
	}

	DoChange();
	VSequence = VWEPCHANGE;
	VAnimTime = -1.0f;
	AttackTime = 0;
	VMCounter = 0.0f;
	VBlend = false;
	AttackFlag = false;
}

/* ------------------------------------------------------------------------------------ */
// DoChange
/* ------------------------------------------------------------------------------------ */
void CWeapon::DoChange()
{
	if(WeaponD[CurrentWeapon].FixedView != -1 && !CCD->Player()->GetMonitorMode())
	{
		if(ViewPoint != WeaponD[CurrentWeapon].FixedView)
		{
			int TempView = OldViewPoint;
			CCD->Player()->SwitchCamera(WeaponD[CurrentWeapon].FixedView);
			OldViewPoint = TempView;
		}
	}
	else
	{
		if(ViewPoint != OldViewPoint)
		{
			int TempView = OldViewPoint;
			CCD->Player()->SwitchCamera(OldViewPoint);
			OldViewPoint = TempView;
		}
	}
	if(ViewPoint == FIRSTPERSON)
	{
		if(WeaponD[CurrentWeapon].VActor)
			geWorld_SetActorFlags(CCD->World(), WeaponD[CurrentWeapon].VActor, GE_ACTOR_RENDER_NORMAL);
	}
	else
	{
		if(WeaponD[CurrentWeapon].PActor)
			geWorld_SetActorFlags(CCD->World(), WeaponD[CurrentWeapon].PActor, GE_ACTOR_RENDER_NORMAL | GE_ACTOR_RENDER_MIRRORS);
	}
}

/* ------------------------------------------------------------------------------------ */
// ClearWeapon
/* ------------------------------------------------------------------------------------ */
void CWeapon::ClearWeapon()
{
	if(!(CurrentWeapon == -1 || CurrentWeapon == MAX_WEAPONS))
	{
		if(WeaponD[CurrentWeapon].Catagory == PROJECTILE
			|| (WeaponD[CurrentWeapon].Catagory == MELEE && WeaponD[CurrentWeapon].MeleeAmmo == true))
			CCD->HUD()->ActivateElement(WeaponD[CurrentWeapon].Ammunition, false);
	}
}

/* ------------------------------------------------------------------------------------ */
// ResetWeapon
/* ------------------------------------------------------------------------------------ */
void CWeapon::ReSetWeapon(int value)
{
	char *Motion = CCD->ActorManager()->GetMotion(CCD->Player()->GetActor());
	int index = -1;

	if(!(value == -1 || value == MAX_WEAPONS))
	{
		for(int i=0; i<ANIMMAX; ++i)
		{
			if(!strcmp(Motion, WeaponD[CurrentWeapon].Animations[i]))
			{
				index = i;
				break;
			}
		}
	}

	CurrentWeapon = value; // change to new weapon

	if(index != -1)
	{
		CCD->ActorManager()->SetMotion(CCD->Player()->GetActor(), PlayerAnim(index));
	}

	if(value == -1 || value == MAX_WEAPONS)
		return;

	if(WeaponD[CurrentWeapon].Catagory == PROJECTILE
		|| (WeaponD[CurrentWeapon].Catagory == MELEE && WeaponD[CurrentWeapon].MeleeAmmo == true))
	{
		CCD->HUD()->ActivateElement(WeaponD[CurrentWeapon].Ammunition, true);

		if(WeaponD[CurrentWeapon].Catagory == PROJECTILE)
		{
			if(WeaponD[CurrentWeapon].MagAmt == -1)
			{
				geActor *theActor = CCD->Player()->GetActor();
				CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(theActor);

				if(theInv->Value(WeaponD[CurrentWeapon].Ammunition) >= WeaponD[CurrentWeapon].ShotperMag)
					WeaponD[CurrentWeapon].MagAmt = WeaponD[CurrentWeapon].ShotperMag;
				else
					WeaponD[CurrentWeapon].MagAmt = theInv->Value(WeaponD[CurrentWeapon].Ammunition);
			}
		}
	}

	DoChange();
	VSequence = VWEPCHANGE;
	VAnimTime = -1.0f;
	AttackTime = 0;
	VMCounter = 0.0f;
	VBlend = false;
	AttackFlag = false;
}

/* ------------------------------------------------------------------------------------ */
// CrossHair
/* ------------------------------------------------------------------------------------ */
bool CWeapon::CrossHair()
{
	if(CurrentWeapon == -1 || CurrentWeapon == MAX_WEAPONS)
		return false;

	if(WeaponD[CurrentWeapon].Catagory != PROJECTILE
		|| !(ViewPoint == FIRSTPERSON || ViewPoint == THIRDPERSON))
		return false;

	return true;
}

/* ------------------------------------------------------------------------------------ */
// CrossHairFixed
/* ------------------------------------------------------------------------------------ */
bool CWeapon::CrossHairFixed()
{
	return WeaponD[CurrentWeapon].CrossHairFixed;
}

/* ------------------------------------------------------------------------------------ */
// DisplayCrossHair
/* ------------------------------------------------------------------------------------ */
void CWeapon::DisplayCrossHair()
{
	geBitmap *theBmp;
	geBitmap_Info	BmpInfo;
	int x, y;
	theBmp = WeaponD[CurrentWeapon].CrossHair;

	if(theBmp)
	{
		if(geBitmap_GetInfo(theBmp, &BmpInfo, NULL) == GE_TRUE)
		{
			x = (CCD->Engine()->Width() - BmpInfo.Width) / 2;
			y = (CCD->Engine()->Height() - BmpInfo.Height) / 2;
			geEngine_DrawBitmap(CCD->Engine()->Engine(), theBmp, NULL, x, y);
		}
	}
}

/* ------------------------------------------------------------------------------------ */
// GetCrossHair
/* ------------------------------------------------------------------------------------ */
geBitmap *CWeapon::GetCrossHair()
{
	geBitmap *theBmp;
	theBmp = WeaponD[CurrentWeapon].CrossHair;
	return theBmp;
}

/* ------------------------------------------------------------------------------------ */
// PlayerAnim
/* ------------------------------------------------------------------------------------ */
char *CWeapon::PlayerAnim(int index)
{
	if(!(CurrentWeapon == -1 || CurrentWeapon == MAX_WEAPONS))
	{
		if(!EffectC_IsStringNull(WeaponD[CurrentWeapon].Animations[index]))
		{
			return WeaponD[CurrentWeapon].Animations[index];
		}
	}

	return CCD->Player()->GetAnimations(index);
}

/* ------------------------------------------------------------------------------------ */
// DieAnim
/* ------------------------------------------------------------------------------------ */
char *CWeapon::DieAnim()
{
	if(!(CurrentWeapon == -1 || CurrentWeapon == MAX_WEAPONS))
	{
		if(WeaponD[CurrentWeapon].DieAnimAmt > 0)
		{
			return WeaponD[CurrentWeapon].DieAnim[rand()%WeaponD[CurrentWeapon].DieAnimAmt];
		}
	}

	return NULL;
}

/* ------------------------------------------------------------------------------------ */
// InjuryAnim
/* ------------------------------------------------------------------------------------ */
char *CWeapon::InjuryAnim()
{
	if(!(CurrentWeapon == -1 || CurrentWeapon == MAX_WEAPONS))
	{
		if(WeaponD[CurrentWeapon].InjuryAnimAmt > 0)
		{
			return WeaponD[CurrentWeapon].InjuryAnim[rand()%WeaponD[CurrentWeapon].InjuryAnimAmt];
		}
	}

	return NULL;
}

/* ------------------------------------------------------------------------------------ */
// WeaponData
/* ------------------------------------------------------------------------------------ */
void CWeapon::WeaponData()
{
	char szData[256];

	if(!(CurrentWeapon == -1 || CurrentWeapon == MAX_WEAPONS) && ViewPoint == FIRSTPERSON)
	{
		if(CCD->WeaponPosition())
		{
			// VActorRotation converted to radians at load time
			// display rotation in degrees
			sprintf(szData,"Rotation : X= %.2f, Y= %.2f, Z= %.2f",
				WeaponD[CurrentWeapon].VActorRotation.X*GE_180OVERPI + WeaponD[CurrentWeapon].K,
				WeaponD[CurrentWeapon].VActorRotation.Y*GE_180OVERPI + WeaponD[CurrentWeapon].Z,
				WeaponD[CurrentWeapon].VActorRotation.Z*GE_180OVERPI + WeaponD[CurrentWeapon].L);

			CCD->MenuManager()->WorldFontRect(szData, FONT10, 5, CCD->Engine()->Height()- 40, 255.0f);

			sprintf(szData,"Offset : X= %.2f, Y= %.2f, Z= %.2f, Scale : %.2f",
				WeaponD[CurrentWeapon].VActorOffset.X + WeaponD[CurrentWeapon].F,
				WeaponD[CurrentWeapon].VActorOffset.Y + WeaponD[CurrentWeapon].H,
				WeaponD[CurrentWeapon].VActorOffset.Z + WeaponD[CurrentWeapon].J,
				WeaponD[CurrentWeapon].VScale+WeaponD[CurrentWeapon].G);

			CCD->MenuManager()->WorldFontRect(szData, FONT10, 5, CCD->Engine()->Height()- 30, 255.0f);
		}
	}
}

/* ------------------------------------------------------------------------------------ */
// KeyReload
/* ------------------------------------------------------------------------------------ */
void CWeapon::KeyReload()
{
	if(CurrentWeapon == -1 || CurrentWeapon == MAX_WEAPONS)
		return;
	if(WeaponD[CurrentWeapon].Catagory != PROJECTILE)
		return;
	if(WeaponD[CurrentWeapon].ShotperMag == 0)
		return;
	if(WeaponD[CurrentWeapon].ShotFired == 0)
		return;

	if(ViewPoint == FIRSTPERSON)
	{
		if(VSequence == VWEPCHANGE || VSequence == VWEPHOLSTER || VSequence == VWEPATTACKEMPTY
			|| VSequence == VWEPRELOAD || VSequence == VWEPKEYRELOAD)
			return; // shooting or changing weapon

		geActor *theActor = CCD->Player()->GetActor();
		CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(theActor);

		if(theInv->Value(WeaponD[CurrentWeapon].Ammunition)>=WeaponD[CurrentWeapon].AmmoPerShot)
		{
			VSequence = VWEPRELOAD;

			if(!EffectC_IsStringNull(WeaponD[CurrentWeapon].VKeyReload))
				VSequence = VWEPKEYRELOAD;

			VMCounter = 0.0f;

			int LeftInMag = WeaponD[CurrentWeapon].ShotperMag - WeaponD[CurrentWeapon].ShotFired;

			if(LeftInMag > 0 && WeaponD[CurrentWeapon].LooseMag)
				theInv->Modify(WeaponD[CurrentWeapon].Ammunition, -LeftInMag);

			if(!EffectC_IsStringNull(WeaponD[CurrentWeapon].ReloadSound))
			{
				Snd Sound;
				memset(&Sound, 0, sizeof(Sound));
				CCD->CameraManager()->GetPosition(&Sound.Pos);
				Sound.Min = CCD->GetAudibleRadius();
				Sound.Loop = GE_FALSE;
				Sound.SoundDef = SPool_Sound(WeaponD[CurrentWeapon].ReloadSound);
				CCD->EffectManager()->Item_Add(EFF_SND, (void*)&Sound);
			}
		}
	}
	else
	{
	}
}

/* ------------------------------------------------------------------------------------ */
// DropWeapon
/* ------------------------------------------------------------------------------------ */
void CWeapon::DropWeapon()
{
	if(CurrentWeapon == -1 || CurrentWeapon == MAX_WEAPONS)
		return;

	dropflag = true;
	Holster();
}

/* ------------------------------------------------------------------------------------ */
// SpawnWeaponAttribute
/* ------------------------------------------------------------------------------------ */
void CWeapon::SpawnWeaponAttribute(int index)
{
	if(WeaponD[index].DropActor[0])
	{
		Attribute tmpAttr;
		memset(&tmpAttr, 0, sizeof(Attribute));

		geVec3d Dir, theRotation;
		geVec3d Pos = CCD->Player()->Position();
		geXForm3d Xf;

		if(ViewPoint == FIRSTPERSON)
		{
			CCD->CameraManager()->GetRotation(&theRotation);
		}
		else
		{
			CCD->ActorManager()->GetRotate(CCD->Player()->GetActor(), &theRotation);
		}

		geXForm3d_SetYRotation(&Xf, theRotation.Y);

		Pos.Y += WeaponD[index].DropActorOffset.Y;

		geXForm3d_GetIn(&Xf, &Dir);
		geVec3d_AddScaled(&Pos, &Dir, WeaponD[index].DropActorOffset.Z, &Pos);

		geXForm3d_GetLeft(&Xf, &Dir);
		geVec3d_AddScaled(&Pos, &Dir, WeaponD[index].DropActorOffset.X, &Pos);

		theRotation.X = WeaponD[index].DropActorRotation.X;
		theRotation.Y = theRotation.Y*GE_180OVERPI + WeaponD[index].DropActorRotation.Y;
		theRotation.Z = WeaponD[index].DropActorRotation.Z;

		tmpAttr.origin					= Pos;
		tmpAttr.szActorName				= WeaponD[index].DropActor;
		tmpAttr.ActorRotation			= theRotation;
		tmpAttr.AmbientColor.r			= WeaponD[index].DropAmbientColor.X;
		tmpAttr.AmbientColor.g			= WeaponD[index].DropAmbientColor.Y;
		tmpAttr.AmbientColor.b			= WeaponD[index].DropAmbientColor.Z;
		tmpAttr.FillColor.r				= WeaponD[index].DropFillColor.X;
		tmpAttr.FillColor.g				= WeaponD[index].DropFillColor.Y;
		tmpAttr.FillColor.b				= WeaponD[index].DropFillColor.Z;
		tmpAttr.AmbientLightFromFloor	= WeaponD[index].DropAmbientLightFromFloor;
		tmpAttr.EnvironmentMapping		= WeaponD[index].DropEnvironmentMapping;
		tmpAttr.AllMaterial				= WeaponD[index].DropAllMaterial;
		tmpAttr.PercentMapping			= WeaponD[index].DropPercentMapping;
		tmpAttr.PercentMaterial			= WeaponD[index].DropPercentMaterial;
		tmpAttr.Gravity					= WeaponD[index].DropGravity;
		tmpAttr.Scale					= WeaponD[index].DropScale;
		tmpAttr.AttributeName			= WeaponD[index].Name;
		tmpAttr.AttributeAmount			= 1;
		tmpAttr.HideFromRadar			= WeaponD[index].DropHideFromRadar;

		CCD->Attributes()->AddAttributeEntity(&tmpAttr);
	}
}

/* ------------------------------------------------------------------------------------ */
// Attack
/* ------------------------------------------------------------------------------------ */
void CWeapon::Attack(bool Alternate)
{
	if(CurrentWeapon == -1 || CurrentWeapon == MAX_WEAPONS)
		return;

	AltAttack = Alternate;

	if(ViewPoint == FIRSTPERSON)
	{

		if(EffectC_IsStringNull(WeaponD[CurrentWeapon].VAttack))
			return;

		if(VSequence == VWEPCHANGE || VSequence == VWEPHOLSTER || VSequence == VWEPATTACKEMPTY
			|| VSequence == VWEPRELOAD || VSequence == VWEPKEYRELOAD
			|| (WeaponD[CurrentWeapon].Catagory==MELEE
			&& (VSequence == VWEPATTACK || VSequence == VWEPALTATTACK
			|| VSequence == VWEPHIT || VSequence == VWEPALTHIT || VSequence == VWEPUSE)))
			return; // shooting or changing weapon

		if(AttackTime != 0)
		{
			int dtime = CCD->FreeRunningCounter()-AttackTime;

			if(dtime < (WeaponD[CurrentWeapon].FireRate*1000.0f))
				return; // too soon
		}

		geActor *theActor = CCD->Player()->GetActor();
		CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(theActor);

		if(WeaponD[CurrentWeapon].ShotperMag > 0
			&& WeaponD[CurrentWeapon].ShotFired >= WeaponD[CurrentWeapon].ShotperMag)
		{
			if(theInv->Value(WeaponD[CurrentWeapon].Ammunition)<WeaponD[CurrentWeapon].AmmoPerShot)
				return;

			VSequence = VWEPRELOAD;
			VMCounter = 0.0f;
			return;
		}

		AttackTime = CCD->FreeRunningCounter();
		VSequence = VWEPATTACK; // shooting animation

		if(WeaponD[CurrentWeapon].Catagory == MELEE)
		{
			if(rand() % 2)
				VSequence = VWEPALTATTACK;
		}

		if(WeaponD[CurrentWeapon].Catagory == PROJECTILE)
		{
			if(theInv->Value(WeaponD[CurrentWeapon].Ammunition) < WeaponD[CurrentWeapon].AmmoPerShot)
			{
				if(!EffectC_IsStringNull(WeaponD[CurrentWeapon].VAttackEmpty))
					VSequence = VWEPATTACKEMPTY;
			}
		}

		VMCounter = 0.0f;
		AttackFlag = true;
	}
	else // 3rd person
	{
		if(!WeaponD[CurrentWeapon].PActor)
			return;

		if(AttackTime != 0)
		{
			if(WeaponD[CurrentWeapon].ShotperMag > 0
				&& WeaponD[CurrentWeapon].ShotFired >= WeaponD[CurrentWeapon].ShotperMag)
			{
				WeaponD[CurrentWeapon].ShotFired = 0;
				geActor *theActor = CCD->Player()->GetActor();
				CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(theActor);

				if(theInv->Value(WeaponD[CurrentWeapon].Ammunition) >= WeaponD[CurrentWeapon].ShotperMag)
					WeaponD[CurrentWeapon].MagAmt = WeaponD[CurrentWeapon].ShotperMag;
				else
					WeaponD[CurrentWeapon].MagAmt = theInv->Value(WeaponD[CurrentWeapon].Ammunition);
			}

			int dtime = CCD->FreeRunningCounter()-AttackTime;

			if(dtime < (WeaponD[CurrentWeapon].FireRate*1000.0f))
				return; // too soon
		}

		AttackTime = CCD->FreeRunningCounter();
		AttackFlag = true;
	}
}

/* ------------------------------------------------------------------------------------ */
// Sound
/* ------------------------------------------------------------------------------------ */
void CWeapon::Sound(bool Attack, const geVec3d &Origin, bool Empty)
{
	Snd Sound;

	memset(&Sound, 0, sizeof(Sound));
    geVec3d_Copy(&Origin, &(Sound.Pos));
    Sound.Min = CCD->GetAudibleRadius();
    Sound.Loop = GE_FALSE;

	if(Attack)
	{
		if(!Empty)
		{
			if(EffectC_IsStringNull(WeaponD[CurrentWeapon].AttackSound))
				return;

			Sound.SoundDef = SPool_Sound(WeaponD[CurrentWeapon].AttackSound);
		}
		else
		{
			if(EffectC_IsStringNull(WeaponD[CurrentWeapon].EmptySound))
				return;

			Sound.SoundDef = SPool_Sound(WeaponD[CurrentWeapon].EmptySound);
		}
	}
	else
	{
		if(EffectC_IsStringNull(WeaponD[CurrentWeapon].HitSound))
			return;

		Sound.SoundDef = SPool_Sound(WeaponD[CurrentWeapon].HitSound);
	}

	if(!Sound.SoundDef)
	{
		char szError[256];
		sprintf(szError, "[WARNING] File %s - Line %d: Failed to open audio file for Weapon %d",
				__FILE__, __LINE__, Slot[CurrentWeapon]);
		CCD->ReportError(szError, false);
		return;
	}

	CCD->EffectManager()->Item_Add(EFF_SND, (void*)&Sound);
}


/* ------------------------------------------------------------------------------------ */
// MeleeAttack
/* ------------------------------------------------------------------------------------ */
void CWeapon::MeleeAttack()
{
	geExtBox theBox;
	geVec3d Forward, Pos;
	GE_Collision Collision;
	geXForm3d Xf;
	geVec3d theRotation, thePosition;

	CCD->ActorManager()->GetPosition(CCD->Player()->GetActor(), &thePosition);
	CCD->ActorManager()->GetBoundingBox(CCD->Player()->GetActor(), &theBox);
	thePosition.Y += theBox.Max.Y*0.5f; // /2.0f;
	CCD->ActorManager()->GetRotate(CCD->Player()->GetActor(), &theRotation);

	float temp = (theBox.Max.Y-theBox.Min.Y)*0.25f;// /4.0f;
	theBox.Min.Y = -temp;
	theBox.Max.Y = temp;
	float tiltp = (CCD->CameraManager()->GetTiltPercent()*2.0f)-1.0f;
	thePosition.Y += (temp*tiltp);

	geXForm3d_SetZRotation(&Xf, theRotation.Z);
	geXForm3d_RotateX(&Xf, theRotation.X);
	geXForm3d_RotateY(&Xf, theRotation.Y);
	geXForm3d_Translate(&Xf, thePosition.X, thePosition.Y, thePosition.Z);

	geXForm3d_GetIn(&Xf, &Forward);
	float distance = 1.25f*(((theBox.Max.X) < (theBox.Max.Z)) ? (theBox.Max.X) : (theBox.Max.Z));
	geVec3d_AddScaled(&Xf.Translation, &Forward, distance, &Pos);

	Collision.Actor = NULL;
	Collision.Model = NULL;


	bool result = false;
	geWorld_Model *pModel;
	geActor *pActor;
	theBox.Min.X += Pos.X;
	theBox.Min.Y += Pos.Y;
	theBox.Min.Z += Pos.Z;
	theBox.Max.X += Pos.X;
	theBox.Max.Y += Pos.Y;
	theBox.Max.Z += Pos.Z;

	if(CCD->ActorManager()->DoesBoxHitActor(Pos, theBox, &pActor, CCD->Player()->GetActor()) == GE_TRUE)
	{
		Collision.Model = NULL;
		Collision.Actor = pActor;		// Actor we hit
		result = true;
	}
	else if(CCD->ModelManager()->DoesBoxHitModel(Pos, theBox, &pModel) == GE_TRUE)
	{
		Collision.Actor = NULL;
		Collision.Model = pModel;		// Model we hit
		result = true;
	}

	if(result)
	{
		bool Stamina = true;

		if(!EffectC_IsStringNull(WeaponD[CurrentWeapon].Ammunition))
		{
			CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());

			if(theInv->Value(WeaponD[CurrentWeapon].Ammunition)<WeaponD[CurrentWeapon].AmmoPerShot)
				Stamina = false;
			else
				theInv->Modify(WeaponD[CurrentWeapon].Ammunition, -WeaponD[CurrentWeapon].AmmoPerShot);
		}

		if(Stamina)
		{
			int nHitType = CCD->Collision()->ProcessCollision(Collision, NULL, false);

			if(nHitType != kNoCollision)
			{
				if(VSequence == VWEPATTACK)
					VSequence = VWEPHIT;
				else
					VSequence = VWEPALTHIT;

				Sound(false, thePosition, false);

				// Process damage here
				if(Collision.Actor != NULL)
				{
					CCD->Damage()->DamageActor(Collision.Actor, WeaponD[CurrentWeapon].MeleeDamage, WeaponD[CurrentWeapon].Attribute, WeaponD[CurrentWeapon].MeleeAltDamage, WeaponD[CurrentWeapon].AltAttribute, WeaponD[CurrentWeapon].Name);
					CCD->Explosions()->AddExplosion(WeaponD[CurrentWeapon].MeleeExplosion, Pos);
				}

				if(Collision.Model)
					CCD->Damage()->DamageModel(Collision.Model, WeaponD[CurrentWeapon].MeleeDamage, WeaponD[CurrentWeapon].Attribute, WeaponD[CurrentWeapon].MeleeAltDamage, WeaponD[CurrentWeapon].AltAttribute);
			}
			else
			{
				Sound(true, thePosition, false);
			}
		}
		else
		{
			Sound(true, thePosition, false);
		}
	}
	else
	{
		Sound(true, thePosition, false);
	}
}

/* ------------------------------------------------------------------------------------ */
// ProjectileAttack
/* ------------------------------------------------------------------------------------ */
void CWeapon::ProjectileAttack()
{
	geVec3d theRotation;
	geVec3d thePosition;
	geXForm3d Xf;
	geVec3d Front, Back;
	geVec3d Direction, Pos, Orient;
	geExtBox theBox;
	GE_Collision Collision;
	geFloat CurrentDistance, x;

	theBox.Min.X = theBox.Min.Y = theBox.Min.Z = -1.0f;
	theBox.Max.X = theBox.Max.Y = theBox.Max.Z = 1.0f;
	geActor *theActor = CCD->Player()->GetActor();
	CurrentDistance = 4000.0f;

	if(ViewPoint == FIRSTPERSON)
	{
		CCD->CameraManager()->GetRotation(&theRotation);
		CCD->CameraManager()->GetPosition(&thePosition);

		geXForm3d_SetZRotation(&Xf, theRotation.Z);
		geXForm3d_RotateX(&Xf, theRotation.X);
		geXForm3d_RotateY(&Xf, theRotation.Y);
	}
	else
	{
		if(WeaponD[CurrentWeapon].PBone[0] != '\0')
		{
			if(WeaponD[CurrentWeapon].PActor)
				geActor_GetBoneTransform(WeaponD[CurrentWeapon].PActor, WeaponD[CurrentWeapon].PBone, &Xf );
			else
				geActor_GetBoneTransform(theActor, WeaponD[CurrentWeapon].PBone, &Xf );
		}
		else
		{
			geActor_GetBoneTransform(theActor, RootBoneName(theActor), &Xf );
		}

		thePosition = Xf.Translation;
		CCD->ActorManager()->GetRotate(theActor, &theRotation);
		geVec3d CRotation, CPosition;
		CCD->CameraManager()->GetCameraOffset(&CPosition, &CRotation);

		geXForm3d_SetZRotation(&Xf, theRotation.Z);
		geXForm3d_RotateX(&Xf, theRotation.X-CCD->CameraManager()->GetTilt());
		geXForm3d_RotateY(&Xf, theRotation.Y);
	}

	Pos = thePosition;

	geXForm3d_GetIn(&Xf, &Direction);
	geVec3d_AddScaled(&Pos, &Direction, CurrentDistance, &Back);

	geVec3d_Copy(&Pos, &Front);

	if(geWorld_Collision(CCD->World(), &theBox.Min, &theBox.Max, &Front, &Back,
		GE_VISIBLE_CONTENTS, GE_COLLIDE_ALL, 0, NULL, NULL, &Collision))
    {
		CurrentDistance = geVec3d_DistanceBetween(&Collision.Impact, &Front);

		if(CurrentDistance > 4000.0f)
			CurrentDistance = 4000.0f;

		geVec3d_AddScaled(&Pos, &Direction, CurrentDistance, &Back);

	}

	// add launchoffsets to position
	if(ViewPoint == FIRSTPERSON)
	{
		if(WeaponD[CurrentWeapon].VBone[0] != '\0')
		{
			geXForm3d Xf;

			if(geActor_GetBoneTransform(WeaponD[CurrentWeapon].VActor, WeaponD[CurrentWeapon].VBone, &Xf) == GE_TRUE)
			{
				geVec3d_Copy(&(Xf.Translation), &Pos);
			}
			else
			{
				geXForm3d_GetUp(&Xf, &Direction);
				geVec3d_AddScaled(&Pos, &Direction, WeaponD[CurrentWeapon].VOffset.Y, &Pos);

				geXForm3d_GetLeft(&Xf, &Direction);
				geVec3d_AddScaled(&Pos, &Direction, WeaponD[CurrentWeapon].VOffset.X, &Pos);
			}
		}
		else
		{
			geXForm3d_GetUp(&Xf, &Direction);
			geVec3d_AddScaled(&Pos, &Direction, WeaponD[CurrentWeapon].VOffset.Y, &Pos);

			geXForm3d_GetLeft(&Xf, &Direction);
			geVec3d_AddScaled(&Pos, &Direction, WeaponD[CurrentWeapon].VOffset.X, &Pos);
		}
	}
	else
	{
		geXForm3d_GetUp(&Xf, &Direction);
		geVec3d_AddScaled(&Pos, &Direction, WeaponD[CurrentWeapon].POffset.Y, &Pos);

		geXForm3d_GetLeft(&Xf, &Direction);
		geVec3d_AddScaled(&Pos, &Direction, WeaponD[CurrentWeapon].POffset.X, &Pos);
	}

	// direction from actual launch point to target
	geVec3d_Subtract(&Back, &Pos, &Orient);
	float l = geVec3d_Length(&Orient);

	// protect from Div by Zero
	if(l > 0.0f)
	{
		x = Orient.X;
		Orient.X = GE_PIOVER2 - (float)acos(Orient.Y / l);
		Orient.Y = (float)atan2(x, Orient.Z) + GE_PI;
		Orient.Z = 0.0f;	// roll is zero - always!!?

		geXForm3d_SetZRotation(&Xf, Orient.Z);
		geXForm3d_RotateX(&Xf, Orient.X);
		geXForm3d_RotateY(&Xf, Orient.Y);
		geVec3d_Set(&(Xf.Translation), Pos.X, Pos.Y, Pos.Z);

		geXForm3d_GetIn(&Xf, &Direction);
		geVec3d_Add(&Pos, &Direction, &Front);
		Collision.Actor = (geActor*)NULL;
		CCD->Collision()->IgnoreContents(false);

		while(CCD->Collision()->CheckForCollision(&theBox.Min, &theBox.Max, Front, Front, &Collision))
		{
			if(Collision.Actor != theActor)
				break;

			geVec3d_Add(&Front, &Direction, &Front);
			Collision.Actor = (geActor*)NULL;
		}

		if(ViewPoint == FIRSTPERSON)
		{
			geVec3d_AddScaled(&Front, &Direction, WeaponD[CurrentWeapon].VOffset.Z, &Pos);
			geVec3d_AddScaled(&WeaponD[CurrentWeapon].VMOffset, &Direction, WeaponD[CurrentWeapon].VOffset.Z, &WeaponD[CurrentWeapon].VMOffset);
		}
		else
		{
			geVec3d_AddScaled(&Front, &Direction, WeaponD[CurrentWeapon].POffset.Z, &Pos);
		}

		geVec3d_Subtract(&Pos, &thePosition, &WeaponD[CurrentWeapon].VMOffset);

		GE_Contents ZoneContents;

		if(geWorld_GetContents(CCD->World(), &Pos, &theBox.Min,
			&theBox.Max, GE_COLLIDE_MODELS, 0, NULL, NULL, &ZoneContents) == GE_TRUE)
		{
			Liquid *LQ = CCD->Liquids()->IsLiquid(ZoneContents.Model);

			if(LQ)
			{
				if(!WeaponD[CurrentWeapon].WorksUnderwater)
				{
					Sound(true, thePosition, true);
					return;
				}
			}
		}
		CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(theActor);

		if(theInv->Value(WeaponD[CurrentWeapon].Ammunition) >= WeaponD[CurrentWeapon].AmmoPerShot)
		{
			theInv->Modify(WeaponD[CurrentWeapon].Ammunition, -WeaponD[CurrentWeapon].AmmoPerShot);
			Add_Projectile(Pos, Front, Orient, WeaponD[CurrentWeapon].Projectile, WeaponD[CurrentWeapon].Attribute, WeaponD[CurrentWeapon].AltAttribute);
			WeaponD[CurrentWeapon].ShotFired += WeaponD[CurrentWeapon].AmmoPerShot;
			if(ViewPoint == FIRSTPERSON)
			{
				if(WeaponD[CurrentWeapon].JerkAmt > 0.0f)
				{
					CCD->CameraManager()->SetJerk(WeaponD[CurrentWeapon].JerkAmt,WeaponD[CurrentWeapon].JerkDecay);
				}

				if(WeaponD[CurrentWeapon].MuzzleFlash[0] != '\0')
				{
					if(WeaponD[CurrentWeapon].VBone[0] != '\0')
					{
						CCD->Explosions()->AddExplosion(WeaponD[CurrentWeapon].MuzzleFlash, WeaponD[CurrentWeapon].VOffset, WeaponD[CurrentWeapon].VActor, WeaponD[CurrentWeapon].VBone);
						MFlash = false;
					}
					else
					{
						CCD->Explosions()->AddExplosion(WeaponD[CurrentWeapon].MuzzleFlash, Pos);
					}
				}
			}
			else
			{
				if(WeaponD[CurrentWeapon].PBone[0] != '\0')
				{
					geVec3d theRotation, thePosition, Direction;
					CCD->ActorManager()->GetRotate(theActor, &theRotation);
					geXForm3d_SetIdentity(&Xf);
					geXForm3d_RotateX(&Xf, CCD->CameraManager()->GetTilt());
					thePosition = Xf.Translation;

					geXForm3d_GetIn(&Xf, &Direction);
					geVec3d_AddScaled(&thePosition, &Direction, WeaponD[CurrentWeapon].POffset.Z+WeaponD[CurrentWeapon].PMOffset, &thePosition);

					geXForm3d_GetUp(&Xf, &Direction);
					geVec3d_AddScaled(&thePosition, &Direction, WeaponD[CurrentWeapon].POffset.Y, &thePosition);

					geXForm3d_GetLeft(&Xf, &Direction);
					geVec3d_AddScaled(&thePosition, &Direction, WeaponD[CurrentWeapon].POffset.X, &thePosition);

					CCD->Explosions()->AddExplosion(WeaponD[CurrentWeapon].MuzzleFlash3rd, thePosition, WeaponD[CurrentWeapon].PActor, WeaponD[CurrentWeapon].PBone, true);
					MFlash = false;
				}
				else
				{
					geVec3d_AddScaled(&Front, &Direction, WeaponD[CurrentWeapon].PMOffset, &Front);
					CCD->Explosions()->AddExplosion(WeaponD[CurrentWeapon].MuzzleFlash3rd, Front);
				}
			}

			Sound(true, thePosition, false);
		}
		else
		{
			Sound(true, thePosition, true);
		}
	}
}

/* ------------------------------------------------------------------------------------ */
// AddProjectile
/* ------------------------------------------------------------------------------------ */
void CWeapon::Add_Projectile(const geVec3d &Pos, const geVec3d &Front, const geVec3d &Orient,
							 const char *Projectile, char *PAttribute, char *PAltAttribute)
{
	Proj *d;
	int Type = -1;

	for(int i=0; i<MAX_PROJD; i++)
	{
		if(ProjD[i].active)
		{
			if(!stricmp(ProjD[i].Name, Projectile))
			{
				Type = i;
				break;
			}
		}
	}

	if(Type == -1)
		return;

	if(ProjD[Type].active)
	{
		geXForm3d XForm;
		d = new Proj;

		d->Rotation = ProjD[Type].Rotation;
		d->Angles = Orient;
		d->Pos = Pos;
		d->Gravity = ProjD[Type].Gravity;
		d->LifeTime = ProjD[Type].LifeTime;
		d->Bounce = ProjD[Type].Bounce;
		d->BounceSoundDef = NULL;

		if(!EffectC_IsStringNull(ProjD[Type].BounceSound))
			d->BounceSoundDef = SPool_Sound(ProjD[Type].BounceSound);

		d->MoveSoundDef = NULL;

		if(!EffectC_IsStringNull(ProjD[Type].MoveSound))
			d->MoveSoundDef = SPool_Sound(ProjD[Type].MoveSound);

		d->ImpactSoundDef = NULL;

		if(!EffectC_IsStringNull(ProjD[Type].ImpactSound))
			d->ImpactSoundDef = SPool_Sound(ProjD[Type].ImpactSound);

		d->MoveSoundEffect = PlaySound(d->MoveSoundDef, d->Pos, true);
		d->Decal = ProjD[Type].Decal;
		d->Explosion = ProjD[Type].Explosion;
		d->ActorExplosion = ProjD[Type].ActorExplosion;
		d->ShowBoth = ProjD[Type].ShowBoth;
		d->AttachActor = ProjD[Type].AttachActor;
		d->BoneLevel = ProjD[Type].BoneLevel;
		d->ShakeAmt = ProjD[Type].ShakeAmt;
		d->ShakeDecay = ProjD[Type].ShakeDecay;
		d->Damage = ProjD[Type].Damage;
		d->AltDamage = ProjD[Type].AltDamage;
		d->AltAttribute = PAltAttribute;
		d->RadiusDamage = ProjD[Type].RadiusDamage;
		d->Radius = ProjD[Type].Radius;
		d->Attribute = PAttribute;
		d->Name = ProjD[Type].Name;

		geXForm3d_SetZRotation(&XForm, Orient.Z);
		geXForm3d_RotateX(&XForm, Orient.X);
		geXForm3d_RotateY(&XForm, Orient.Y);

		geXForm3d_GetIn(&XForm, &(d->Direction));
		geVec3d_Scale(&(d->Direction), ProjD[Type].Speed, &(d->Direction));

		geXForm3d_SetYRotation(&XForm, -GE_PIOVER2);
		geXForm3d_GetIn(&XForm, &(d->In));
		geVec3d_Inverse(&(d->In));

		d->Actor = CCD->ActorManager()->SpawnActor(ProjD[Type].Actorfile,
			d->Pos, d->Rotation, ProjD[Type].ActorAnimation, ProjD[Type].ActorAnimation, NULL);

		CCD->ActorManager()->Rotate(d->Actor, d->Angles);
		CCD->ActorManager()->SetType(d->Actor, ENTITY_PROJECTILE);
		CCD->ActorManager()->SetScale(d->Actor, ProjD[Type].Scale);
		CCD->ActorManager()->SetBoxChange(d->Actor, false);
		CCD->ActorManager()->SetNoCollide(d->Actor);
		CCD->ActorManager()->SetBBox(d->Actor, ProjD[Type].BoxSize, ProjD[Type].BoxSize, ProjD[Type].BoxSize);
		CCD->ActorManager()->GetBoundingBox(d->Actor, &d->ExtBox);
		CCD->ActorManager()->SetStepHeight(d->Actor, -1.0f);
		CCD->ActorManager()->SetHideRadar(d->Actor, true);

		if(d->Gravity)
			CCD->ActorManager()->SetGravity(d->Actor, CCD->Player()->GetGravity());

		GE_Collision	Collision;
		CCD->Collision()->IgnoreContents(false);
		CCD->Collision()->CheckLevel(RGF_COLLISIONLEVEL_1);
		char BoneHit[64];
		BoneHit[0] = '\0';

		if(CCD->Collision()->CheckForBoneCollision(&d->ExtBox.Min, &d->ExtBox.Max,
				Front, d->Pos, &Collision, d->Actor, BoneHit, d->BoneLevel))
		{
			int nHitType = CCD->Collision()->ProcessCollision(Collision, d->Actor, false);

			if(nHitType != kNoCollision)
			{
				if(d->MoveSoundEffect != -1)
					CCD->EffectManager()->Item_Delete(EFF_SND, d->MoveSoundEffect);

				PlaySound(d->ImpactSoundDef, d->Pos, false);

				// inflict damage
				if(Collision.Actor)
					CCD->Damage()->DamageActor(Collision.Actor, d->Damage, d->Attribute, d->AltDamage, d->AltAttribute, d->Name);

				if(Collision.Model)
					CCD->Damage()->DamageModel(Collision.Model, d->Damage, d->Attribute, d->AltDamage, d->AltAttribute);

				// create explosion here
				CCD->Explosions()->AddExplosion(d->Explosion, d->Pos);

				if(d->ShakeAmt>0.0f)
					CCD->CameraManager()->SetShake(d->ShakeAmt, d->ShakeDecay, d->Pos);

				if(Collision.Actor)
					CCD->Explosions()->AddExplosion(d->ActorExplosion, d->Pos);

				// Handle explosion damage here
				if(d->RadiusDamage > 0.0f)
				{
					CCD->Damage()->DamageActorInRange(d->Pos, d->Radius, d->RadiusDamage, d->Attribute, d->RadiusDamage, d->AltAttribute, "Explosion");
					CCD->Damage()->DamageModelInRange(d->Pos, d->Radius, d->RadiusDamage, d->Attribute, d->RadiusDamage, d->AltAttribute);
				}

				CCD->ActorManager()->RemoveActor(d->Actor);
				geActor_Destroy(&d->Actor);
				delete d;
				return;
			}
		}

		for(int i=0; i<5; ++i)
		{
			d->Effect[i] = -1;
			geXForm3d thePosition;

			if(EffectC_IsStringNull(ProjD[Type].EffectBone[i]))
				geActor_GetBoneTransform(d->Actor, RootBoneName(d->Actor), &(thePosition));
			else
			{
				if(!geActor_GetBoneTransform(d->Actor, ProjD[Type].EffectBone[i], &(thePosition)))
					geActor_GetBoneTransform(d->Actor, RootBoneName(d->Actor), &(thePosition));
			}

			if(ProjD[Type].Effect[i][0] != '\0')
			{
				for(int k=0; k<MAXEXPITEM; k++)
				{
					if(CCD->Effect()->EffectActive(k))
					{
						if(!stricmp(ProjD[Type].Effect[i], CCD->Effect()->EffectName(k)))
						{
							geVec3d Zero = {0.0f, 0.0f, 0.0f};
							d->Effect[i] = CCD->Effect()->AddEffect(k, thePosition.Translation, Zero);
							d->EffectType[i] = CCD->Effect()->EffectType(k);
							d->EffectBone[i] = ProjD[Type].EffectBone[i];
							break;
						}
					}
				}
			}
		}

		d->next = NULL;
		d->prev = Bottom;

		if(Bottom != NULL)
			Bottom->next = d;

		Bottom = d;
		return;
	}
}

/* ------------------------------------------------------------------------------------ */
// PlaySound
/* ------------------------------------------------------------------------------------ */
int CWeapon::PlaySound(geSound_Def *SoundDef, const geVec3d &Pos, bool Loop)
{
	Snd Sound;

	if(!SoundDef)
		return -1;

	memset(&Sound, 0, sizeof(Sound));
	geVec3d_Copy(&Pos, &(Sound.Pos));
    Sound.Min = CCD->GetAudibleRadius();
	Sound.Loop = Loop;
	Sound.SoundDef = SoundDef;

	return CCD->EffectManager()->Item_Add(EFF_SND, (void*)&Sound);
}

/* ------------------------------------------------------------------------------------ */
// ZoomAmount
/* ------------------------------------------------------------------------------------ */
int CWeapon::ZoomAmount()
{
	if(!(CurrentWeapon == -1 || CurrentWeapon == MAX_WEAPONS) && ViewPoint == FIRSTPERSON)
	{
		if(WeaponD[CurrentWeapon].ZoomAmt > 1)
		{
			if(WeaponD[CurrentWeapon].ZoomAmt > 20)
				return 20;

			return WeaponD[CurrentWeapon].ZoomAmt;
		}
	}

	return 0;
}


/* ------------------------------------------------------------------------------------ */
// GetVActor
/* ------------------------------------------------------------------------------------ */
geActor* CWeapon::GetVActor()
{
	return WeaponD[CurrentWeapon].VActor;
}

/* ------------------------------------------------------------------------------------ */
// GetPActor
/* ------------------------------------------------------------------------------------ */
geActor* CWeapon::GetPActor()
{
	return WeaponD[CurrentWeapon].PActor;
}

/* ------------------------------------------------------------------------------------ */
// LoadDefaults
/* ------------------------------------------------------------------------------------ */
void CWeapon::LoadDefaults()
{
	char weaponini[64];

	if(CCD->MenuManager()->GetUseSelect())
		strcpy(weaponini, CCD->MenuManager()->GetCurrentWeapon());
	else
		strcpy(weaponini, "weapon.ini");

	CIniFile AttrFile(weaponini);

	if(!AttrFile.ReadFile())
	{
		CCD->Log()->Error("Failed to open " + weaponini + " file");
		return;
	}

	std::string KeyName = AttrFile.FindFirstKey();
	std::string Type, Vector;
	geVFile *ActorFile;
	char szName[64], szAlpha[64];
	int projptr = 0;
	int weapptr = 0;

	while(KeyName != "")
	{
		Type = AttrFile.GetValue(KeyName, "type");

		// -------------------------------------------------------------------------------
		//								PROJECTILES
		// -------------------------------------------------------------------------------
		if(Type == "projectile")
		{
			strcpy(ProjD[projptr].Name, KeyName.c_str());
			Type = AttrFile.GetValue(KeyName, "actor");
			strcpy(ProjD[projptr].Actorfile, Type.c_str());
			Vector = AttrFile.GetValue(KeyName, "rotation");

			if(Vector != "")
			{
				CCD->Log()->Warning("Redefinition of proj [" + KeyName + "] in " + weaponini + " file.");
				strcpy(szName,Vector.c_str());
				ProjD[projptr].Rotation = Extract(szName);
				ProjD[projptr].Rotation.X *= GE_PIOVER180;
				ProjD[projptr].Rotation.Y *= GE_PIOVER180;
				ProjD[projptr].Rotation.Z *= GE_PIOVER180;
			}

			geActor *Actor = CCD->ActorManager()->SpawnActor(ProjD[projptr].Actorfile,
							ProjD[projptr].Rotation, ProjD[projptr].Rotation, "", "", NULL);
			CCD->ActorManager()->RemoveActor(Actor);
			geActor_Destroy(&Actor);
			Actor = NULL;

			ProjD[projptr].Scale = (float)AttrFile.GetValueF(KeyName, "scale");

			Type = AttrFile.GetValue(KeyName, "animation");
			if(Type != "")
				strcpy(ProjD[projptr].ActorAnimation, Type.c_str());
			else
				ProjD[projptr].ActorAnimation[0] = '\0';

			ProjD[projptr].Bounce = false;
			Vector = AttrFile.GetValue(KeyName, "bounce");
			if(Vector == "true")
				ProjD[projptr].Bounce = true;

			ProjD[projptr].Gravity = false;
			Vector = AttrFile.GetValue(KeyName, "gravity");
			if(Vector == "true")
				ProjD[projptr].Gravity = true;

			ProjD[projptr].Speed = (float)AttrFile.GetValueF(KeyName, "speed");
			ProjD[projptr].LifeTime = (float)AttrFile.GetValueF(KeyName, "lifetime");

			Type = AttrFile.GetValue(KeyName, "explosion");
			strcpy(ProjD[projptr].Explosion, Type.c_str());

			Type = AttrFile.GetValue(KeyName, "actorexplosion");
			strcpy(ProjD[projptr].ActorExplosion, Type.c_str());

			ProjD[projptr].ShowBoth = false;
			Vector = AttrFile.GetValue(KeyName, "showboth");
			if(Vector == "true")
				ProjD[projptr].ShowBoth = true;

			ProjD[projptr].AttachActor = false;
			Vector = AttrFile.GetValue(KeyName, "attachactor");
			if(Vector == "true")
				ProjD[projptr].AttachActor = true;

			ProjD[projptr].BoneLevel = true;
			Vector = AttrFile.GetValue(KeyName, "bonelevel");
			if(Vector == "false")
				ProjD[projptr].BoneLevel = false;

			ProjD[projptr].ShakeAmt		= (float)AttrFile.GetValueF(KeyName, "shakeamount");
			ProjD[projptr].ShakeDecay	= (float)AttrFile.GetValueF(KeyName, "shakedecay");
			ProjD[projptr].BoxSize		= (float)AttrFile.GetValueF(KeyName, "boundingbox");
			ProjD[projptr].Damage		= (float)AttrFile.GetValueF(KeyName, "damage");
			ProjD[projptr].AltDamage	= (float)AttrFile.GetValueF(KeyName, "altdamage");
			ProjD[projptr].Radius		= (float)AttrFile.GetValueF(KeyName, "explosionradius");
			ProjD[projptr].RadiusDamage = (float)AttrFile.GetValueF(KeyName, "explosiondamage");
			ProjD[projptr].Decal = AttrFile.GetValueI(KeyName, "decal");

			Type = AttrFile.GetValue(KeyName, "movesound");
			strcpy(ProjD[projptr].MoveSound, Type.c_str());
			if(Type != "")
				SPool_Sound(ProjD[projptr].MoveSound);

			Type = AttrFile.GetValue(KeyName, "impactsound");
			strcpy(ProjD[projptr].ImpactSound, Type.c_str());
			if(Type != "")
				SPool_Sound(ProjD[projptr].ImpactSound);

			Type = AttrFile.GetValue(KeyName, "bouncesound");
			strcpy(ProjD[projptr].BounceSound, Type.c_str());
			if(Type != "")
				SPool_Sound(ProjD[projptr].BounceSound);

			Type = AttrFile.GetValue(KeyName, "effect0");
			if(Type != "")
			{
				strcpy(ProjD[projptr].Effect[0], Type.c_str());
				Vector = AttrFile.GetValue(KeyName, "effectbone0");
				if(Vector != "")
					strcpy(ProjD[projptr].EffectBone[0], Vector.c_str());
			}

			Type = AttrFile.GetValue(KeyName, "effect1");
			if(Type != "")
			{
				Vector = AttrFile.GetValue(KeyName, "effectbone1");
				strcpy(ProjD[projptr].Effect[1], Type.c_str());
				if(Vector != "")
					strcpy(ProjD[projptr].EffectBone[1], Vector.c_str());
			}

			Type = AttrFile.GetValue(KeyName, "effect2");
			if(Type != "")
			{
				Vector = AttrFile.GetValue(KeyName, "effectbone2");
				strcpy(ProjD[projptr].Effect[2], Type.c_str());
				if(Vector != "")
					strcpy(ProjD[projptr].EffectBone[2], Vector.c_str());
			}

			Type = AttrFile.GetValue(KeyName, "effect3");
			if(Type != "")
			{
				strcpy(ProjD[projptr].Effect[3], Type.c_str());
				Vector = AttrFile.GetValue(KeyName, "effectbone3");
				if(Vector != "")
					strcpy(ProjD[projptr].EffectBone[3], Vector.c_str());
			}

			Type = AttrFile.GetValue(KeyName, "effect4");
			if(Type != "")
			{
				strcpy(ProjD[projptr].Effect[4], Type.c_str());
				Vector = AttrFile.GetValue(KeyName, "effectbone4");
				if(Vector != "")
					strcpy(ProjD[projptr].EffectBone[4], Vector.c_str());
			}

			ProjD[projptr].active = true;
			projptr += 1;
		}
		// -------------------------------------------------------------------------------
		//								WEAPONS
		// -------------------------------------------------------------------------------
		else if(Type == "weapon")
		{
			WeaponD[weapptr].VActor = NULL;
			WeaponD[weapptr].PActor = NULL;

			for(int k=0; k<ANIMMAX; k++)
				WeaponD[weapptr].Animations[k][0] = '\0';

			strcpy(WeaponD[weapptr].Name, KeyName.c_str());
			Type = AttrFile.GetValue(KeyName, "slot");
			if(Type == "default")
				WeaponD[weapptr].Slot = MAX_WEAPONS-1;
			else
				WeaponD[weapptr].Slot = AttrFile.GetValueI(KeyName, "slot");

			WeaponD[weapptr].Catagory = MELEE;
			Type = AttrFile.GetValue(KeyName, "catagory");
			if(Type == "projectile")
				WeaponD[weapptr].Catagory = PROJECTILE;
			else if(Type == "beam")
				WeaponD[weapptr].Catagory = BEAM;

			// ---------------------------------------------------------------------------
			//							Weapon Catagory
			// ---------------------------------------------------------------------------
			switch(WeaponD[weapptr].Catagory)
			{
			case MELEE:
				WeaponD[weapptr].MeleeDamage = (float)AttrFile.GetValueF(KeyName, "damage");
				WeaponD[weapptr].MeleeAltDamage = (float)AttrFile.GetValueF(KeyName, "altdamage");
				Type = AttrFile.GetValue(KeyName, "hitsound");
				strcpy(WeaponD[weapptr].HitSound, Type.c_str());
				if(Type != "")
					SPool_Sound(WeaponD[weapptr].HitSound);

				Type = AttrFile.GetValue(KeyName, "meleexplosion");
				strcpy(WeaponD[weapptr].MeleeExplosion, Type.c_str());
				WeaponD[weapptr].MeleeAmmo = false;

				Type = AttrFile.GetValue(KeyName, "meleeammoactivate");
				if(Type == "true")
					WeaponD[weapptr].MeleeAmmo = true;
				break;
			case PROJECTILE:
				Vector = AttrFile.GetValue(KeyName, "projectile");
				strcpy(WeaponD[weapptr].Projectile, Vector.c_str());

				Vector = AttrFile.GetValue(KeyName, "muzzleflash");
				strcpy(WeaponD[weapptr].MuzzleFlash, Vector.c_str());
				Vector = AttrFile.GetValue(KeyName, "muzzleflash3rd");
				if(Vector != "")
					strcpy(WeaponD[weapptr].MuzzleFlash3rd, Vector.c_str());
				else
					strcpy(WeaponD[weapptr].MuzzleFlash3rd, WeaponD[weapptr].MuzzleFlash);

				WeaponD[weapptr].WorksUnderwater = true;
				Type = AttrFile.GetValue(KeyName, "worksunderwater");
				if(Type == "false")
					WeaponD[weapptr].WorksUnderwater = false;
				WeaponD[weapptr].MeleeAmmo = false;
				break;
			case BEAM:
				break;
			}

			// ---------------------------------------------------------------------------
			//							Weapon ViewPoint
			// ---------------------------------------------------------------------------
			WeaponD[weapptr].FixedView = -1;
			Type = AttrFile.GetValue(KeyName, "forceviewto");
			if(Type == "firstperson")
				WeaponD[weapptr].FixedView = FIRSTPERSON;
			else if(Type == "thirdperson")
				WeaponD[weapptr].FixedView = THIRDPERSON;
			else if(Type == "isometric")
				WeaponD[weapptr].FixedView = ISOMETRIC;
			else if(Type == "fixed")
				WeaponD[weapptr].FixedView = FIXEDCAMERA;

			// ---------------------------------------------------------------------------
			//							Weapon Sounds
			// ---------------------------------------------------------------------------

			Type = AttrFile.GetValue(KeyName, "attacksound");
			strcpy(WeaponD[weapptr].AttackSound, Type.c_str());
			if(Type != "")
				SPool_Sound(WeaponD[weapptr].AttackSound);

			Type = AttrFile.GetValue(KeyName, "emptysound");
			strcpy(WeaponD[weapptr].EmptySound, Type.c_str());
			if(Type != "")
				SPool_Sound(WeaponD[weapptr].EmptySound);

			Type = AttrFile.GetValue(KeyName, "usesound");
			strcpy(WeaponD[weapptr].UseSound, Type.c_str());
			if(Type != "")
				SPool_Sound(WeaponD[weapptr].UseSound);

			Type = AttrFile.GetValue(KeyName, "attribute");
			strcpy(WeaponD[weapptr].Attribute, Type.c_str());
			Type = AttrFile.GetValue(KeyName, "reloadsound");
			strcpy(WeaponD[weapptr].ReloadSound, Type.c_str());
			if(Type != "")
				SPool_Sound(WeaponD[weapptr].ReloadSound);

			Type = AttrFile.GetValue(KeyName, "altattribute");
			strcpy(WeaponD[weapptr].AltAttribute, Type.c_str());
			Type = AttrFile.GetValue(KeyName, "ammunition");
			strcpy(WeaponD[weapptr].Ammunition, Type.c_str());
			WeaponD[weapptr].AmmoPerShot = AttrFile.GetValueI(KeyName, "ammopershot");
			WeaponD[weapptr].FireRate = (float)AttrFile.GetValueF(KeyName, "firerate");

			bool activeflag = false;

			// ---------------------------------------------------------------------------
			//							Drop Weapon Attribute
			// ---------------------------------------------------------------------------
			WeaponD[weapptr].DropActor[0] = 0;
			Vector = AttrFile.GetValue(KeyName, "dropactor");
			if(Vector != "")
			{
				strcpy(WeaponD[weapptr].DropActor, Vector.c_str());
				geActor *Actor = CCD->ActorManager()->SpawnActor(WeaponD[weapptr].DropActor,
					WeaponD[weapptr].DropActorOffset, WeaponD[weapptr].DropActorRotation, "", "", NULL);

				if(!Actor)
				{
					CCD->Log()->Warning("File %s - Line %d: %s : Missing Actor '%s'",
						__FILE__, __LINE__, KeyName.c_str(), WeaponD[weapptr].DropActor);
				}
				else
				{
					CCD->ActorManager()->RemoveActor(Actor);
					geActor_Destroy(&Actor);
					Actor = NULL;
				}

				Vector = AttrFile.GetValue(KeyName, "dropoffset");
				if(Vector != "")
				{
					strcpy(szName, Vector.c_str());
					WeaponD[weapptr].DropActorOffset = Extract(szName);
				}

				WeaponD[weapptr].DropScale = 1.0f;
				if(AttrFile.GetValueF(KeyName, "dropscale"))
					WeaponD[weapptr].DropScale = (float)AttrFile.GetValueF(KeyName, "dropscale");

				geVec3d_Set(&(WeaponD[weapptr].DropFillColor), 255.0f, 255.0f, 255.0f);
				geVec3d_Set(&(WeaponD[weapptr].DropAmbientColor), 255.0f, 255.0f, 255.0f);

				Vector = AttrFile.GetValue(KeyName, "dropfillcolor");
				if(Vector != "")
				{
					strcpy(szName, Vector.c_str());
					WeaponD[weapptr].DropFillColor = Extract(szName);
				}

				Vector = AttrFile.GetValue(KeyName, "dropambientcolor");
				if(Vector != "")
				{
					strcpy(szName, Vector.c_str());
					WeaponD[weapptr].DropAmbientColor = Extract(szName);
				}

				WeaponD[weapptr].DropAmbientLightFromFloor = GE_TRUE;
				Vector = AttrFile.GetValue(KeyName, "dropambientlightfromfloor");
				if(Vector == "false")
					WeaponD[weapptr].DropAmbientLightFromFloor = GE_FALSE;

				Vector = AttrFile.GetValue(KeyName, "dropenvironmentmapping");
				if(Vector == "true")
				{
					WeaponD[weapptr].DropEnvironmentMapping = GE_TRUE;

					Vector = AttrFile.GetValue(KeyName, "dropallmaterial");
					if(Vector == "true")
						WeaponD[weapptr].DropAllMaterial = GE_TRUE;

					WeaponD[weapptr].DropPercentMapping = (float)AttrFile.GetValueF(KeyName, "droppercentmapping");
					WeaponD[weapptr].DropPercentMaterial = (float)AttrFile.GetValueF(KeyName, "droppercentmaterial");
				}

				Vector = AttrFile.GetValue(KeyName, "droprotation");
				if(Vector != "")
				{
					strcpy(szName, Vector.c_str());
					WeaponD[weapptr].DropActorRotation = Extract(szName);
				}

				Vector = AttrFile.GetValue(KeyName, "dropgravity");
				if(Vector == "true")
					WeaponD[weapptr].DropGravity = GE_TRUE;

				Vector = AttrFile.GetValue(KeyName, "drophidefromradar");
				if(Vector == "true")
					WeaponD[weapptr].DropHideFromRadar = GE_TRUE;
			}


			// ---------------------------------------------------------------------------
			//							1st Person Weapon
			// ---------------------------------------------------------------------------
			Vector = AttrFile.GetValue(KeyName, "viewactor");

			if(Vector != "")
			{
				strcpy(szName, Vector.c_str());
				CCD->OpenRFFile(&ActorFile, kActorFile, szName, GE_VFILE_OPEN_READONLY);

				if(ActorFile)
				{
					WeaponD[weapptr].VActorDef = geActor_DefCreateFromFile(ActorFile);

					if(WeaponD[weapptr].VActorDef)
					{
						WeaponD[weapptr].VActor = geActor_Create(WeaponD[weapptr].VActorDef);
						geWorld_AddActor(CCD->World(), WeaponD[weapptr].VActor, 0, 0xffffffff);
						geVec3d Fill = {0.0f, 1.0f, 0.0f};
						// changed QD 12/15/05
						// geVec3d_Normalize(&Fill);
						geXForm3d	Xf;
						geXForm3d	XfT;
						geVec3d FillColor = {255.0f, 255.0f, 255.0f};
						geVec3d AmbientColor = {255.0f, 255.0f, 255.0f};
						geBoolean AmbientLightFromFloor = GE_TRUE;
						geVec3d NewFillNormal;
						geActor_GetBoneTransform(WeaponD[weapptr].VActor, RootBoneName(WeaponD[weapptr].VActor), &Xf );
						geXForm3d_GetTranspose(&Xf, &XfT);
						geXForm3d_Rotate(&XfT, &Fill, &NewFillNormal);

						Vector = AttrFile.GetValue(KeyName, "viewfillcolor");
						if(Vector != "")
						{
							strcpy(szName, Vector.c_str());
							FillColor = Extract(szName);
						}

						Vector = AttrFile.GetValue(KeyName, "viewambientcolor");
						if(Vector != "")
						{
							strcpy(szName, Vector.c_str());
							AmbientColor = Extract(szName);
						}

						Vector = AttrFile.GetValue(KeyName, "viewambientlightfromfloor");
						if(Vector == "false")
							AmbientLightFromFloor = GE_FALSE;

						geActor_SetStaticLightingOptions(WeaponD[weapptr].VActor, AmbientLightFromFloor, GE_TRUE, 3);

						geActor_SetLightingOptions(WeaponD[weapptr].VActor, GE_TRUE, &NewFillNormal, FillColor.X, FillColor.Y, FillColor.Z,
						AmbientColor.X, AmbientColor.Y, AmbientColor.Z, AmbientLightFromFloor, 6, NULL, GE_FALSE);

						Vector = AttrFile.GetValue(KeyName, "environmentmapping");
						if(Vector == "true")
						{
							bool all = false;
							Vector = AttrFile.GetValue(KeyName, "allmaterial");

							if(Vector == "true")
								all = true;

							float percent = (float)AttrFile.GetValueF(KeyName, "percentmapping");
							float percentm = (float)AttrFile.GetValueF(KeyName, "percentmaterial");
							SetEnvironmentMapping(WeaponD[weapptr].VActor, true, all, percent, percentm);
						}

						Vector = AttrFile.GetValue(KeyName, "viewrotation");
						if(Vector != "")
						{
							strcpy(szName, Vector.c_str());
							WeaponD[weapptr].VActorRotation = Extract(szName);
							geVec3d_Scale(&(WeaponD[weapptr].VActorRotation), GE_PIOVER180, &(WeaponD[weapptr].VActorRotation));
						}

						Vector = AttrFile.GetValue(KeyName, "viewoffset");
						if(Vector != "")
						{
							strcpy(szName, Vector.c_str());
							WeaponD[weapptr].VActorOffset = Extract(szName);
						}

						WeaponD[weapptr].VScale = (float)AttrFile.GetValueF(KeyName, "viewscale");
						float tspeed = (float)AttrFile.GetValueF(KeyName, "viewanimspeed");

						if(tspeed > 0.0f)
							WeaponD[weapptr].VAnimSpeed = tspeed;

						Vector = AttrFile.GetValue(KeyName, "viewarmanim");
						strcpy(WeaponD[weapptr].VArm, Vector.c_str());
						Vector = AttrFile.GetValue(KeyName, "viewidleanim");
						strcpy(WeaponD[weapptr].VIdle, Vector.c_str());
						WeaponD[weapptr].VAttack[0] = '\0';
						Vector = AttrFile.GetValue(KeyName, "viewattackanim");
						strcpy(WeaponD[weapptr].VAttack, Vector.c_str());
						Vector = AttrFile.GetValue(KeyName, "viewwalkanim");
						strcpy(WeaponD[weapptr].VWalk, Vector.c_str());

						geVec3d LitColor = {255.0f, 255.0f, 255.0f};

						switch(WeaponD[weapptr].Catagory)
						{
						case MELEE:
							Vector = AttrFile.GetValue(KeyName, "viewaltattackanim");
							strcpy(WeaponD[weapptr].VAltAttack, Vector.c_str());
							Vector = AttrFile.GetValue(KeyName, "viewhitanim");
							strcpy(WeaponD[weapptr].VHit, Vector.c_str());
							Vector = AttrFile.GetValue(KeyName, "viewalthitanim");
							strcpy(WeaponD[weapptr].VAltHit, Vector.c_str());
							WeaponD[weapptr].VUse[0] = '\0';
							Vector = AttrFile.GetValue(KeyName, "viewuseanim");
							strcpy(WeaponD[weapptr].VUse, Vector.c_str());
							break;
						case PROJECTILE:
							Vector = AttrFile.GetValue(KeyName, "viewlaunchoffset");
							if(Vector != "")
							{
								strcpy(szName, Vector.c_str());
								WeaponD[weapptr].VOffset = Extract(szName);
							}
							WeaponD[weapptr].VReload[0] = '\0';
							Vector = AttrFile.GetValue(KeyName, "viewreloadanim");
							strcpy(WeaponD[weapptr].VReload, Vector.c_str());
							WeaponD[weapptr].ShotperMag = AttrFile.GetValueI(KeyName, "shotpermagazine");
							WeaponD[weapptr].ShotFired = 0;
							WeaponD[weapptr].MagAmt = -1;
							WeaponD[weapptr].VKeyReload[0] = '\0';
							Vector = AttrFile.GetValue(KeyName, "viewkeyreloadanim");
							strcpy(WeaponD[weapptr].VKeyReload, Vector.c_str());
							WeaponD[weapptr].VAttackEmpty[0] = '\0';
							Vector = AttrFile.GetValue(KeyName, "viewattackemptyanim");
							strcpy(WeaponD[weapptr].VAttackEmpty, Vector.c_str());
							WeaponD[weapptr].LooseMag = false;
							Vector = AttrFile.GetValue(KeyName, "loosemag");
							if(Vector == "true")
								WeaponD[weapptr].LooseMag = true;
							Vector = AttrFile.GetValue(KeyName, "viewlaunchbone");
							if(Vector != "")
								strcpy(WeaponD[weapptr].VBone, Vector.c_str());
							WeaponD[weapptr].CrossHairFixed = false;
							Vector = AttrFile.GetValue(KeyName, "crosshairfixed");
							if(Vector == "true")
								WeaponD[weapptr].CrossHairFixed = true;
							WeaponD[weapptr].CrossHair = NULL;
							Vector = AttrFile.GetValue(KeyName, "crosshair");
							if(Vector != "")
							{
								strcpy(szName, Vector.c_str());
								strcpy(szAlpha, Vector.c_str());
								Vector = AttrFile.GetValue(KeyName, "crosshairalpha");

								if(Vector != "")
									strcpy(szAlpha, Vector.c_str());

								WeaponD[weapptr].CrossHair = CreateFromFileAndAlphaNames(szName, szAlpha);

								if(WeaponD[weapptr].CrossHairFixed)
									geEngine_AddBitmap(CCD->Engine()->Engine(), WeaponD[weapptr].CrossHair);
								else
									geWorld_AddBitmap(CCD->World(), WeaponD[weapptr].CrossHair);
							}
							Vector = AttrFile.GetValue(KeyName, "crosshairlitcolor");
							if(Vector != "")
							{
								strcpy(szName, Vector.c_str());
								LitColor = Extract(szName);
							}
							WeaponD[weapptr].LitColor = LitColor;
							WeaponD[weapptr].AllowLit = false;
							Vector = AttrFile.GetValue(KeyName, "allowlitcrosshair");
							if(Vector == "true")
								WeaponD[weapptr].AllowLit = true;
							WeaponD[weapptr].ZoomAmt = AttrFile.GetValueI(KeyName, "zoomamount");
							WeaponD[weapptr].ZoomOverlay = NULL;
							Vector = AttrFile.GetValue(KeyName, "zoomoverlay");
							if(Vector != "")
							{
								strcpy(szName, Vector.c_str());
								strcpy(szAlpha, Vector.c_str());
								Vector = AttrFile.GetValue(KeyName, "zoomoverlayalpha");

								if(Vector != "")
									strcpy(szAlpha, Vector.c_str());

								WeaponD[weapptr].ZoomOverlay = CreateFromFileAndAlphaNames(szName, szAlpha);
								geWorld_AddBitmap(CCD->World(), WeaponD[weapptr].ZoomOverlay);
							}
							WeaponD[weapptr].MoveZoom = true;
							Vector = AttrFile.GetValue(KeyName, "allowmovezoom");
							if(Vector == "false")
								WeaponD[weapptr].MoveZoom = false;
							WeaponD[weapptr].JerkAmt = (float)AttrFile.GetValueF(KeyName, "recoilamount");
							WeaponD[weapptr].JerkDecay = (float)AttrFile.GetValueF(KeyName, "recoildecay");
							WeaponD[weapptr].BobAmt = (float)AttrFile.GetValueF(KeyName, "bobamount");
							break;
						case BEAM:
							break;
						}

						activeflag = true;
					}
					geVFile_Close(ActorFile);
				}
			}

			// ---------------------------------------------------------------------------
			//							3rd Person Weapon
			// ---------------------------------------------------------------------------
			Vector = AttrFile.GetValue(KeyName, "playeractor");

			if(Vector != "")
			{
				strcpy(szName, Vector.c_str());
				CCD->OpenRFFile(&ActorFile, kActorFile, szName, GE_VFILE_OPEN_READONLY);

				if(ActorFile)
				{
					WeaponD[weapptr].PActorDef = geActor_DefCreateFromFile(ActorFile);

					if(WeaponD[weapptr].PActorDef)
					{
						WeaponD[weapptr].PActor = geActor_Create(WeaponD[weapptr].PActorDef);
						geWorld_AddActor(CCD->World(), WeaponD[weapptr].PActor, 0, 0xffffffff);

						geVec3d Fill = {0.0f, 1.0f, 0.0f};
						geXForm3d	Xf;
						geXForm3d	XfT;
						geVec3d FillColor = {255.0f, 255.0f, 255.0f};
						geVec3d AmbientColor = {255.0f, 255.0f, 255.0f};
						geBoolean AmbientLightFromFloor = GE_TRUE;
						geVec3d NewFillNormal;
						geActor_GetBoneTransform(WeaponD[weapptr].PActor, RootBoneName(WeaponD[weapptr].PActor), &Xf);
						geXForm3d_GetTranspose(&Xf, &XfT);
						geXForm3d_Rotate(&XfT, &Fill, &NewFillNormal);

						Vector = AttrFile.GetValue(KeyName, "playerfillcolor");
						if(Vector != "")
						{
							strcpy(szName, Vector.c_str());
							FillColor = Extract(szName);
						}

						Vector = AttrFile.GetValue(KeyName, "playerambientcolor");
						if(Vector != "")
						{
							strcpy(szName, Vector.c_str());
							AmbientColor = Extract(szName);
						}

						Vector = AttrFile.GetValue(KeyName, "playerambientlightfromfloor");
						if(Vector == "false")
							AmbientLightFromFloor = GE_FALSE;

						geActor_SetStaticLightingOptions(WeaponD[weapptr].PActor, AmbientLightFromFloor, GE_TRUE, 3);

						geActor_SetLightingOptions(WeaponD[weapptr].PActor, GE_TRUE, &NewFillNormal, FillColor.X, FillColor.Y, FillColor.Z,
							AmbientColor.X, AmbientColor.Y, AmbientColor.Z, AmbientLightFromFloor, 6, NULL, GE_FALSE);

						Vector = AttrFile.GetValue(KeyName, "playerenvironmentmapping");
						if(Vector == "true")
						{
							bool all = false;
							Vector = AttrFile.GetValue(KeyName, "playerallmaterial");

							if(Vector == "true")
								all = true;

							float percent = (float)AttrFile.GetValueF(KeyName, "playerpercentmapping");
							float percentm = (float)AttrFile.GetValueF(KeyName, "playerpercentmaterial");
							SetEnvironmentMapping(WeaponD[weapptr].PActor, true, all, percent, percentm);
						}

						Vector = AttrFile.GetValue(KeyName, "playerrotation");
						if(Vector != "")
						{
							strcpy(szName, Vector.c_str());
							WeaponD[weapptr].PActorRotation = Extract(szName);
							geVec3d_Scale(&(WeaponD[weapptr].PActorRotation), GE_PIOVER180, &(WeaponD[weapptr].PActorRotation));
						}

						WeaponD[weapptr].PScale = CCD->Player()->GetScale() * (float)AttrFile.GetValueF(KeyName, "playerscale");
						WeaponD[weapptr].PMOffset = (float)AttrFile.GetValueF(KeyName, "playermuzzleoffset");
						WeaponD[weapptr].PBone[0] = '\0';

						Vector = AttrFile.GetValue(KeyName, "playerbone");
						if(Vector != "")
							strcpy(WeaponD[weapptr].PBone, Vector.c_str());

						Vector = AttrFile.GetValue(KeyName, "playerlaunchoffset");
						if(Vector != "")
						{
							strcpy(szName, Vector.c_str());
							WeaponD[weapptr].POffset = Extract(szName);
						}

						Type = AttrFile.GetValue(KeyName, "idle");
						strcpy(WeaponD[weapptr].Animations[IDLE], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "walk");
						strcpy(WeaponD[weapptr].Animations[WALK], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "run");
						strcpy(WeaponD[weapptr].Animations[RUN], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "jump");
						strcpy(WeaponD[weapptr].Animations[JUMP], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "starttojump");
						strcpy(WeaponD[weapptr].Animations[STARTJUMP], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "fall");
						strcpy(WeaponD[weapptr].Animations[FALL], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "land");
						strcpy(WeaponD[weapptr].Animations[LAND], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "slidetoleft");
						strcpy(WeaponD[weapptr].Animations[SLIDELEFT], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "slideruntoleft");
						strcpy(WeaponD[weapptr].Animations[RUNSLIDELEFT], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "slidetoright");
						strcpy(WeaponD[weapptr].Animations[SLIDERIGHT], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "slideruntoright");
						strcpy(WeaponD[weapptr].Animations[RUNSLIDERIGHT], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "crawl");
						strcpy(WeaponD[weapptr].Animations[CRAWL], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "crouchidle");
						strcpy(WeaponD[weapptr].Animations[CIDLE], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "crouchstarttojump");
						strcpy(WeaponD[weapptr].Animations[CSTARTJUMP], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "crouchland");
						strcpy(WeaponD[weapptr].Animations[CLAND], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "crawlslidetoleft");
						strcpy(WeaponD[weapptr].Animations[SLIDECLEFT], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "crawlslidetoright");
						strcpy(WeaponD[weapptr].Animations[SLIDECRIGHT], Type.c_str());

						Type = AttrFile.GetValue(KeyName, "shootup");
						strcpy(WeaponD[weapptr].Animations[SHOOT1], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "shootdwn");
						strcpy(WeaponD[weapptr].Animations[SHOOT], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "aimup");
						strcpy(WeaponD[weapptr].Animations[AIM1], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "aimdwn");
						strcpy(WeaponD[weapptr].Animations[AIM], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "walkshootup");
						strcpy(WeaponD[weapptr].Animations[WALKSHOOT1], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "walkshootdwn");
						strcpy(WeaponD[weapptr].Animations[WALKSHOOT], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "runshootup");
						strcpy(WeaponD[weapptr].Animations[RUNSHOOT1], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "runshootdwn");
						strcpy(WeaponD[weapptr].Animations[RUNSHOOT], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "slidetoleftshootup");
						strcpy(WeaponD[weapptr].Animations[SLIDELEFTSHOOT1], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "slidetoleftshootdwn");
						strcpy(WeaponD[weapptr].Animations[SLIDELEFTSHOOT], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "slideruntoleftshootup");
						strcpy(WeaponD[weapptr].Animations[RUNSLIDELEFTSHOOT1], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "slideruntoleftshootdwn");
						strcpy(WeaponD[weapptr].Animations[RUNSLIDELEFTSHOOT], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "slidetorightshootup");
						strcpy(WeaponD[weapptr].Animations[SLIDERIGHTSHOOT1], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "slidetorightshootdwn");
						strcpy(WeaponD[weapptr].Animations[SLIDERIGHTSHOOT], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "slideruntorightshootup");
						strcpy(WeaponD[weapptr].Animations[RUNSLIDERIGHTSHOOT1], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "slideruntorightshootdwn");
						strcpy(WeaponD[weapptr].Animations[RUNSLIDERIGHTSHOOT], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "jumpshootup");
						strcpy(WeaponD[weapptr].Animations[JUMPSHOOT1], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "jumpshootdwn");
						strcpy(WeaponD[weapptr].Animations[JUMPSHOOT], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "fallshootup");
						strcpy(WeaponD[weapptr].Animations[FALLSHOOT1], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "fallshootdwn");
						strcpy(WeaponD[weapptr].Animations[FALLSHOOT], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "crouchaimup");
						strcpy(WeaponD[weapptr].Animations[CAIM1], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "crouchaimdwn");
						strcpy(WeaponD[weapptr].Animations[CAIM], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "crouchshootup");
						strcpy(WeaponD[weapptr].Animations[CSHOOT1], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "crouchshootdwn");
						strcpy(WeaponD[weapptr].Animations[CSHOOT], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "crawlshootup");
						strcpy(WeaponD[weapptr].Animations[CRAWLSHOOT1], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "crawlshootdwn");
						strcpy(WeaponD[weapptr].Animations[CRAWLSHOOT], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "crawlslidetoleftshootup");
						strcpy(WeaponD[weapptr].Animations[SLIDECLEFTSHOOT1], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "crawlslidetoleftshootdwn");
						strcpy(WeaponD[weapptr].Animations[SLIDECLEFTSHOOT], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "crawlslidetorightshootup");
						strcpy(WeaponD[weapptr].Animations[SLIDECRIGHTSHOOT1], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "crawlslidetorightshootdwn");
						strcpy(WeaponD[weapptr].Animations[SLIDECRIGHTSHOOT], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "swim");
						strcpy(WeaponD[weapptr].Animations[SWIM], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "treadwater");
						strcpy(WeaponD[weapptr].Animations[TREADWATER], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "idletowalk");
						strcpy(WeaponD[weapptr].Animations[I2WALK], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "idletorun");
						strcpy(WeaponD[weapptr].Animations[I2RUN], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "walktoidle");
						strcpy(WeaponD[weapptr].Animations[W2IDLE], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "crawltoidle");
						strcpy(WeaponD[weapptr].Animations[C2IDLE], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "crouchtoidle");
						strcpy(WeaponD[weapptr].Animations[CROUCH2IDLE], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "idletocrouch");
						strcpy(WeaponD[weapptr].Animations[IDLE2CROUCH], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "swimtotread");
						strcpy(WeaponD[weapptr].Animations[SWIM2TREAD], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "treadtoswim");
						strcpy(WeaponD[weapptr].Animations[TREAD2SWIM], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "idletotread");
						strcpy(WeaponD[weapptr].Animations[IDLE2TREAD], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "swimtowalk");
						strcpy(WeaponD[weapptr].Animations[SWIM2WALK], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "walktoswim");
						strcpy(WeaponD[weapptr].Animations[WALK2SWIM], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "treadtoidle");
						strcpy(WeaponD[weapptr].Animations[TREAD2IDLE], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "jumptofall");
						strcpy(WeaponD[weapptr].Animations[JUMP2FALL], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "jumptotread");
						strcpy(WeaponD[weapptr].Animations[JUMP2TREAD], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "falltotread");
						strcpy(WeaponD[weapptr].Animations[FALL2TREAD], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "falltocrawl");
						strcpy(WeaponD[weapptr].Animations[FALL2CRAWL], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "falltowalk");
						strcpy(WeaponD[weapptr].Animations[FALL2WALK], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "falltojump");
						strcpy(WeaponD[weapptr].Animations[FALL2JUMP], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "walktojump");
						strcpy(WeaponD[weapptr].Animations[WALK2JUMP], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "walktocrawl");
						strcpy(WeaponD[weapptr].Animations[WALK2CRAWL], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "crawltowalk");
						strcpy(WeaponD[weapptr].Animations[CRAWL2WALK], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "idletocrawl");
						strcpy(WeaponD[weapptr].Animations[IDLE2CRAWL], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "aimtocrouch");
						strcpy(WeaponD[weapptr].Animations[AIM2CROUCH], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "crouchtoaim");
						strcpy(WeaponD[weapptr].Animations[CROUCH2AIM], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "walktotread");
						strcpy(WeaponD[weapptr].Animations[W2TREAD], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "falltorun");
						strcpy(WeaponD[weapptr].Animations[FALL2RUN], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "crawltorun");
						strcpy(WeaponD[weapptr].Animations[CRAWL2RUN], Type.c_str());

						Type = AttrFile.GetValue(KeyName, "walkback");
						strcpy(WeaponD[weapptr].Animations[WALKBACK], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "runback");
						strcpy(WeaponD[weapptr].Animations[RUNBACK], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "crawlback");
						strcpy(WeaponD[weapptr].Animations[CRAWLBACK], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "walkshootupback");
						strcpy(WeaponD[weapptr].Animations[WALKSHOOT1BACK], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "walkshootdwnback");
						strcpy(WeaponD[weapptr].Animations[WALKSHOOTBACK], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "runshootupback");
						strcpy(WeaponD[weapptr].Animations[RUNSHOOT1BACK], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "runshootdwnback");
						strcpy(WeaponD[weapptr].Animations[RUNSHOOTBACK], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "crawlshootupback");
						strcpy(WeaponD[weapptr].Animations[CRAWLSHOOT1BACK], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "crawlshootdwnback");
						strcpy(WeaponD[weapptr].Animations[CRAWLSHOOTBACK], Type.c_str());
						Type = AttrFile.GetValue(KeyName, "swimback");
						strcpy(WeaponD[weapptr].Animations[SWIMBACK], Type.c_str());
						// end change RF064

						// die animations
						Type = AttrFile.GetValue(KeyName, "die");
						char strip[256], *temp;
						int i = 0;
						WeaponD[weapptr].DieAnimAmt = 0;
						if(Type != "")
						{
							strcpy(strip, Type.c_str());
							temp = strtok(strip, " \n");

							while(temp)
							{
								strcpy(WeaponD[weapptr].DieAnim[i], temp);
								i += 1;
								WeaponD[weapptr].DieAnimAmt = i;
								if(i == 5)
									break;
								temp = strtok(NULL," \n");
							}
						}

						// injury animations
						Type = AttrFile.GetValue(KeyName, "injury");
						i = 0;
						WeaponD[weapptr].InjuryAnimAmt = 0;
						if(Type != "")
						{
							strcpy(strip, Type.c_str());
							temp = strtok(strip, " \n");
							while(temp)
							{
								strcpy(WeaponD[weapptr].InjuryAnim[i], temp);
								i += 1;
								WeaponD[weapptr].InjuryAnimAmt = i;
								if(i == 5)
									break;
								temp = strtok(NULL, " \n");
							}
						}

						activeflag = true;
					}

					geVFile_Close(ActorFile);
				}
			}

			if(activeflag)
			{
				WeaponD[weapptr].active = true;
				weapptr += 1;
			}
		}

		KeyName = AttrFile.FindNextKey();
	}
}

/* ------------------------------------------------------------------------------------ */
// GetFixedView
/* ------------------------------------------------------------------------------------ */
int CWeapon::GetFixedView()
{
	if(CurrentWeapon == -1 || CurrentWeapon == MAX_WEAPONS)
		return -1;

	return WeaponD[CurrentWeapon].FixedView;
}


/* ----------------------------------- END OF FILE ------------------------------------ */
