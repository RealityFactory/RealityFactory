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
#include "CFileManager.h"
#include "IniFile.h"
#include "CGUIManager.h"
#include "CHeadsUpDisplay.h"
#include "CLevel.h"
#include "CAttribute.h"
#include "CCharacter.h"
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
CWeapon::CWeapon() :
	m_DropFlag(false),
	m_Holstered(false),
	m_AttackFlag(false),
	m_AltAttack(false),
	m_AttackTime(0),
	m_VSequence(VWEPIDLE),
	m_VAnimTime(-1.0f),
	m_VMCounter(0.0f),
	m_VBScale(0.0f),		// not used
	m_VBDiff(0.0f),			// not used
	m_VBOrigin(0.0f),		// not used
	m_VBactorMotion(NULL),	// not used
	m_VBlend(false),		// not used
	m_MFlash(false),
	m_Bottom(NULL)
{
	geVec3d_Clear(&m_CrossPoint); // not used
	geVec3d_Clear(&m_ProjectedPoint);

	// Ok, check to see if there's a PlayerSetup around...
	geEntity_EntitySet *entitySet = geWorld_GetEntitySet(CCD->World(), "PlayerSetup");

	if(!entitySet)
	{
		CCD->Log()->Error("No PlayerSetup found! Bad level");
		return;
	}

	geEntity *entity = geEntity_EntitySetGetNextEntity(entitySet, NULL);

	if(entity)
	{
		PlayerSetup *setup = static_cast<PlayerSetup*>(geEntity_GetUserData(entity));
		m_ViewPoint = setup->LevelViewPoint;
		m_OldViewPoint = m_ViewPoint;
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
	{
		Projectile *proj, *prevProj;
		proj = m_Bottom;

		while(proj != NULL)
		{
			prevProj = proj->prev;
			CCD->ActorManager()->RemoveActorCheck(proj->Actor);
			geActor_Destroy(&proj->Actor);
			delete proj;
			proj = prevProj;
		}

		m_Bottom = NULL;
	}

	{
		stdext::hash_map<std::string, ProjectileDefinition*>::iterator iter = m_Projectiles.begin();

		for(; iter!=m_Projectiles.end(); ++iter)
		{
			delete (iter->second);
		}

		m_Projectiles.clear();
	}

	{
		stdext::hash_map<std::string, WeaponDefinition*>::iterator iter = m_Weapons.begin();
		for(; iter!=m_Weapons.end(); ++iter)
		{
			if(iter->second->VActorDef)
			{
				geActor_DefDestroy(&iter->second->VActorDef);
				iter->second->VActorDef = NULL;
			}

			if(iter->second->VActor)
			{
				geWorld_RemoveActor(CCD->World(), iter->second->VActor);
				geActor_Destroy(&iter->second->VActor);
				iter->second->VActor = NULL;
			}

			if(iter->second->CrossHair)
			{
				if(iter->second->CrossHairFixed)
					geEngine_RemoveBitmap(CCD->Engine()->Engine(), iter->second->CrossHair);
				else
					geWorld_RemoveBitmap(CCD->World(), iter->second->CrossHair);

				geBitmap_Destroy(&iter->second->CrossHair);
				iter->second->CrossHair = NULL;
			}

			if(iter->second->ZoomOverlay)
			{
				geWorld_RemoveBitmap(CCD->World(), iter->second->ZoomOverlay);
				geBitmap_Destroy(&iter->second->ZoomOverlay);
				iter->second->ZoomOverlay = NULL;
			}

			if(iter->second->PActorDef)
			{
				geActor_DefDestroy(&iter->second->PActorDef);
				iter->second->PActorDef = NULL;
			}

			if(iter->second->PActor)
			{
				geWorld_RemoveActor(CCD->World(), iter->second->PActor);
				geActor_Destroy(&iter->second->PActor);
				iter->second->PActor = NULL;
			}

			delete (iter->second);
		}
		m_Weapons.clear();
	}
}

/* ------------------------------------------------------------------------------------ */
// Tick
/* ------------------------------------------------------------------------------------ */
void CWeapon::Tick(float timeElapsed)
{
	if(!m_Holstered)
	{
		stdext::hash_map<std::string, WeaponDefinition*>::iterator iter = m_Weapons.begin();
		for(; iter!=m_Weapons.end(); ++iter)
		{
			CPersistentAttributes *inventory = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
			if(inventory->Has(iter->second->Name))
			{
				if(inventory->Value(iter->second->Name) > 0)
				{
					m_Slot[iter->second->Slot] = iter->second->Name;

					// if no weapon is set and this is the default weapon
					if(m_CurrentWeapon.empty() && iter->second->Slot == MAX_WEAPONS-1)
					{
						SetWeapon(iter->second->Slot);			// then set this one as current weapon
					}
				}
			}
		}


		if(m_CurrentWeapon.empty()) // still no weapon?
		{

			for(iter=m_Weapons.begin(); iter!=m_Weapons.end(); ++iter)
			{
				{
					CPersistentAttributes *inventory = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());

					if(inventory->Has(iter->second->Name))	// does this weapon exist in the inventory?
					{
						if(inventory->Value(iter->second->Name) > 0) // do we have at least 1 of this type?
						{
							m_Slot[iter->second->Slot] = iter->second->Name;	// fill the slot with this weapon (-1 = empty)

							if(m_CurrentWeapon.empty())				// no current weapon? use this one
							{
								SetWeapon(iter->second->Slot);
							}
						}
					}
				}
			}
		}
	}

	// take care of any projectiles that are active
	Projectile *proj, *prevProj;

	proj = m_Bottom;

	timeElapsed *= 0.001f;

	while(proj != NULL)
	{
		geVec3d	oldPos,	newPos;
		geVec3d	distance;
		GE_Collision collision;
		geActor *actor = NULL;
		geWorld_Model *model = NULL;
		char boneHit[256] = {0};

		prevProj = proj->prev;
		CCD->ActorManager()->GetPosition(proj->Actor, &oldPos);

		// Movement
		geVec3d_Scale(&proj->Direction, timeElapsed, &distance);
		geVec3d_Add(&oldPos, &distance, &newPos);

		// LifeTime Checking
		bool alive = true;

		proj->LifeTime -= timeElapsed;

		if(proj->LifeTime <= 0.0f)
			alive = false;

		if(alive)
		{
			CCD->Collision()->IgnoreContents(false);
			CCD->Collision()->CheckLevel(RGF_COLLISIONLEVEL_1);

			// Bouncing
			if(proj->Bounce)
			{
				float totalTravelled = 1.0f;	// keeps track of fraction of path travelled (1.0=complete)
				float margin = 0.001f;			// margin to be satisfied with (1.0 - margin == 1.0)
				int loopCounter = 0;			// safety valve for endless collisions in tight corners
				const int maxLoops = 10;		// max. nr of collisions this frame
				bool playSound = true;

/* 07/15/2004 Wendell Buckner
	BUG FIX - Bone Collisions fail because we expect to hit the bone immediately after hitting the
	overall bounding box. So tag the actor as being hit at the bounding box level and after that check ONLY
	the bone bounding boxes until the whatever hit the overall bounding box no longer exists. */
				while(CCD->Collision()->CheckForBoneCollision(&proj->ExtBox.Min, &proj->ExtBox.Max, oldPos, newPos, &collision, proj->Actor, boneHit, proj->BoneLevel, proj))
				{
					// Process hit here
					int nHitType = CCD->Collision()->ProcessCollision(collision, proj->Actor, false);

					if(nHitType == kNoCollision)
						break;

					// impact damage
					if(collision.Actor)
					{
						CCD->Level()->Damage()->DamageActor(collision.Actor, proj->Damage, proj->Attribute, proj->AltDamage, proj->AltAttribute, proj->Name);

						if(proj->AttachActor && !EffectC_IsStringNull(boneHit) && proj->BoneLevel)
						{
							geVec3d zero = {0.0f, 0.0f, 0.0f};
							CCD->Level()->ExplosionManager()->AddExplosion(proj->ActorExplosion, zero, collision.Actor, boneHit);
						}
						else
						{
							CCD->Level()->ExplosionManager()->AddExplosion(proj->ActorExplosion, collision.Impact);
						}
					}

					if(collision.Model)
					{
						CCD->Level()->Damage()->DamageModel(collision.Model, proj->Damage, proj->Attribute, proj->AltDamage, proj->AltAttribute);
					}

					if(playSound)
					{
						PlaySound(proj->BounceSoundDef, collision.Impact, false);
						playSound = false;
					}

					float ratio;
					float elasticity = 1.3f;
					float friction = 0.9f; // loses (1 minus friction) of speed

					CollisionCalcRatio(collision, oldPos, newPos, &ratio);
					CollisionCalcImpactPoint(collision, oldPos, newPos, 1.0f, ratio, &newPos);
					CollisionCalcVelocityImpact(collision, proj->Direction, elasticity, friction, &proj->Direction);

					if(ratio >= 0)
						totalTravelled += (1.0f - totalTravelled) * ratio ;

					if(totalTravelled >= 1.0f - margin)
						break;

					if(++loopCounter >= maxLoops) // safety check
						break;
				}
			}
			else // No Bouncing
			{
/* 07/15/2004 Wendell Buckner
	BUG FIX - Bone Collisions fail because we expect to hit the bone immediately after hitting the
	overall bounding box. So tag the actor as being hit at the bounding box level and after that check ONLY
	the bone bounding boxes until the whatever hit the overall bounding box no longer exists. */
				if(CCD->Collision()->CheckForBoneCollision(&proj->ExtBox.Min, &proj->ExtBox.Max, oldPos, newPos, &collision, proj->Actor, boneHit, proj->BoneLevel, proj))
				{
					// Handle collision here
					int nHitType = CCD->Collision()->ProcessCollision(collision, proj->Actor, false);

					if(nHitType != kNoCollision)
					{
						alive = false;

						geVec3d_Add(&collision.Impact, &collision.Plane.Normal, &proj->Pos);
						actor = collision.Actor;
						model = collision.Model;

						if(nHitType == kCollideNoMove)
						{
							CCD->Level()->Decals()->AddDecal(proj->Decal, &collision.Impact, &collision.Plane.Normal, collision.Model);
						}
					}
				}
			}
		}

		// Move and Pose
		if(alive)
		{
			proj->Pos = newPos;
			CCD->ActorManager()->Position(proj->Actor, proj->Pos);
			CCD->ActorManager()->GetBoundingBox(proj->Actor, &proj->ExtBox);

			if(proj->MoveSoundEffect != -1)
			{
				Snd sound;
				geVec3d_Copy(&proj->Pos, &sound.Pos);
				CCD->EffectManager()->Item_Modify(EFF_SND, proj->MoveSoundEffect, static_cast<void*>(&sound), SND_POS);
			}

			for(int j=0; j<5; ++j)
			{
				if(proj->Effect[j] != -1)
				{
					geXForm3d xForm;

					if(proj->EffectBone[j].empty())
					{
						geActor_GetBoneTransform(proj->Actor, RootBoneName(proj->Actor), &xForm);
					}
					else
					{
						if(!geActor_GetBoneTransform(proj->Actor, proj->EffectBone[j].c_str(), &xForm))
							geActor_GetBoneTransform(proj->Actor, RootBoneName(proj->Actor), &xForm);
					}

					switch(proj->EffectType[j])
					{
					case EFF_SPRAY:
						Spray spray;
						geVec3d_Copy(&xForm.Translation, &spray.Source);
						geVec3d_AddScaled(&spray.Source, &proj->In, 50.0f, &spray.Dest);
						CCD->EffectManager()->Item_Modify(EFF_SPRAY, proj->Effect[j], static_cast<void*>(&spray), SPRAY_SOURCE | SPRAY_ACTUALDEST);
						break;
					case EFF_LIGHT:
						Glow glow;
						geVec3d_Copy(&xForm.Translation, &glow.Pos);
						CCD->EffectManager()->Item_Modify(EFF_LIGHT, proj->Effect[j], static_cast<void*>(&glow), GLOW_POS);
						break;
					case EFF_SND:
						Snd sound;
						geVec3d_Copy(&xForm.Translation, &sound.Pos);
						CCD->EffectManager()->Item_Modify(EFF_SND, proj->Effect[j], static_cast<void*>(&sound), SND_POS);
						break;
					case EFF_SPRITE:
						Sprite sprite;
						geVec3d_Copy(&xForm.Translation, &sprite.Pos);
						CCD->EffectManager()->Item_Modify(EFF_SPRITE, proj->Effect[j], static_cast<void*>(&sprite), SPRITE_POS);
						break;
					case EFF_CORONA:
						EffCorona corona;
						corona.Vertex.X = xForm.Translation.X;
						corona.Vertex.Y = xForm.Translation.Y;
						corona.Vertex.Z = xForm.Translation.Z;
						CCD->EffectManager()->Item_Modify(EFF_CORONA, proj->Effect[j], static_cast<void*>(&corona), CORONA_POS);
						break;
					case EFF_BOLT:
						EBolt bolt;
						geVec3d	distance;
						geVec3d_Subtract(&bolt.Start, &bolt.End, &distance);
						geVec3d_Copy(&xForm.Translation, &bolt.Start);
						geVec3d_Add(&bolt.Start, &distance, &bolt.End);
						CCD->EffectManager()->Item_Modify(EFF_BOLT, proj->Effect[j], static_cast<void*>(&bolt), BOLT_START | BOLT_END);
						break;
					}
				}
			}
		}
		else // Delete when done
		{
			if(proj->MoveSoundEffect != -1)
				CCD->EffectManager()->Item_Delete(EFF_SND, proj->MoveSoundEffect);

			for(int j=0; j<5; ++j)
			{
				if(proj->Effect[j] != -1)
				{
					CCD->EffectManager()->Item_Delete(proj->EffectType[j], proj->Effect[j]);
				}
			}

			PlaySound(proj->ImpactSoundDef, proj->Pos, false);

			// inflict damage
			if(actor)
			{
				CCD->Level()->Damage()->DamageActor(actor, proj->Damage, proj->Attribute, proj->AltDamage, proj->AltAttribute, proj->Name);

				if(proj->AttachActor && !EffectC_IsStringNull(boneHit) && proj->BoneLevel)
				{
					geVec3d zero = {0.0f, 0.0f, 0.0f};
					CCD->Level()->ExplosionManager()->AddExplosion(proj->ActorExplosion, zero, actor, boneHit);
				}
				else
				{
					CCD->Level()->ExplosionManager()->AddExplosion(proj->ActorExplosion, proj->Pos);
				}

				if(proj->ShowBoth)
				{
					CCD->Level()->ExplosionManager()->AddExplosion(proj->Explosion, proj->Pos);
				}
			}
			else if(model)
			{
				CCD->Level()->Damage()->DamageModel(model, proj->Damage, proj->Attribute, proj->AltDamage, proj->AltAttribute);
				CCD->Level()->ExplosionManager()->AddExplosion(proj->Explosion, proj->Pos);
			}
			else
			{
				CCD->Level()->ExplosionManager()->AddExplosion(proj->Explosion, proj->Pos);
			}

			// create explosion here
			if(proj->ShakeAmt>0.0f)
				CCD->CameraManager()->SetShake(proj->ShakeAmt, proj->ShakeDecay, proj->Pos);


			// Handle explosion damage here
			if(proj->RadiusDamage > 0.0f)
			{
				CCD->Level()->Damage()->DamageActorInRange(proj->Pos, proj->Radius, proj->RadiusDamage, proj->Attribute, proj->RadiusDamage, proj->Attribute, "Explosion");
				CCD->Level()->Damage()->DamageModelInRange(proj->Pos, proj->Radius, proj->RadiusDamage, proj->Attribute, proj->RadiusDamage, proj->Attribute);
			}

			if(m_Bottom == proj)
				m_Bottom = proj->prev;

			if(proj->prev != NULL)
				proj->prev->next = proj->next;

			if(proj->next != NULL)
				proj->next->prev = proj->prev;

/* 07/15/2004 Wendell Buckner
	BUG FIX - Bone Collisions fail because we expect to hit the bone immediately after hitting the
	overall bounding box. So tag the actor as being hit at the bounding box level and after that check ONLY
	the bone bounding boxes until the whatever hit the overall bounding box no longer exists. */
			CCD->ActorManager()->RemoveCollideObject(COLMaxBBox, NULL, static_cast<void*>(proj));

			CCD->ActorManager()->RemoveActorCheck(proj->Actor);
			geActor_Destroy(&proj->Actor);
			delete proj;
			proj = NULL;
		}

		proj = prevProj;
	}
}

/* ------------------------------------------------------------------------------------ */
// Display
/* ------------------------------------------------------------------------------------ */
void CWeapon::Display()
{
	if(CCD->Player()->GetMonitorMode())
		return;

	if(m_CurrentWeapon.empty())
		return;

	if(m_ViewPoint == FIRSTPERSON)
	{
		std::string weaponName = m_CurrentWeapon;
		if(m_Weapons[weaponName]->PActor)
			geWorld_SetActorFlags(CCD->World(), m_Weapons[weaponName]->PActor, CCD->Player()->GetMirror());

		geWorld_SetActorFlags(CCD->World(), m_Weapons[weaponName]->VActor, GE_ACTOR_RENDER_NORMAL);
		DisplayFirstPerson(weaponName); // m_CurrentWeapon may be altered here!

		if(CCD->Player()->GetMirror() != 0 && m_Weapons[weaponName]->PActor)
			DisplayThirdPerson(weaponName);
	}
	else
	{
		if(m_Weapons[m_CurrentWeapon]->VActor)
			geWorld_SetActorFlags(CCD->World(), m_Weapons[m_CurrentWeapon]->VActor, 0);

		if(m_Weapons[m_CurrentWeapon]->PActor)
		{
			geWorld_SetActorFlags(CCD->World(), m_Weapons[m_CurrentWeapon]->PActor, GE_ACTOR_RENDER_NORMAL | GE_ACTOR_RENDER_MIRRORS);
			DisplayThirdPerson(m_CurrentWeapon);
		}
	}
}

/* ------------------------------------------------------------------------------------ */
// DisplayThirdPerson
/* ------------------------------------------------------------------------------------ */
void CWeapon::DisplayThirdPerson(const std::string& weaponName)
{
	geXForm3d xForm;
	geVec3d rotation;
	geVec3d position;
	geMotion *actorMotion;
	geMotion *actorBlendMotion;
	geActor *actor = CCD->Player()->GetActor();

	CCD->ActorManager()->GetRotate(actor, &rotation);
	CCD->ActorManager()->GetPosition(actor, &position);
	geActor_SetScale(m_Weapons[weaponName]->PActor,
		m_Weapons[weaponName]->PScale,
		m_Weapons[weaponName]->PScale,
		m_Weapons[weaponName]->PScale);

	// PActorRotation converted to radians at load time
	geXForm3d_SetZRotation(&xForm, m_Weapons[weaponName]->PActorRotation.Z + rotation.Z);
	geXForm3d_RotateX(&xForm, m_Weapons[weaponName]->PActorRotation.X + rotation.X);
	geXForm3d_RotateY(&xForm, m_Weapons[weaponName]->PActorRotation.Y + rotation.Y);
	geXForm3d_Translate(&xForm, position.X, position.Y, position.Z);

	actorMotion = CCD->ActorManager()->GetpMotion(actor);
	actorBlendMotion = CCD->ActorManager()->GetpBMotion(actor);

	if(actorMotion)
	{
		if(CCD->ActorManager()->GetTransitionFlag(actor))
		{
			geActor_SetPose(m_Weapons[weaponName]->PActor, actorMotion, 0.0f, &xForm);

			if(actorBlendMotion)
			{
				float blendAmount = (CCD->ActorManager()->GetBlendAmount(actor) -
					CCD->ActorManager()->GetAnimationTime(actor)) / CCD->ActorManager()->GetBlendAmount(actor);

				if(blendAmount < 0.0f)
					blendAmount = 0.0f;

				geActor_BlendPose(m_Weapons[weaponName]->PActor, actorBlendMotion,
					CCD->ActorManager()->GetStartTime(actor), &xForm, blendAmount);
			}
		}
		else
		{
			geActor_SetPose(m_Weapons[weaponName]->PActor, actorMotion, CCD->ActorManager()->GetAnimationTime(actor), &xForm);

			if(actorBlendMotion)
			{
				geActor_BlendPose(m_Weapons[weaponName]->PActor, actorBlendMotion,
					CCD->ActorManager()->GetAnimationTime(actor), &xForm,
					CCD->ActorManager()->GetBlendAmount(actor));
			}
		}
	}
	else
	{
		geActor_ClearPose(m_Weapons[weaponName]->PActor, &xForm);
	}

	float fAlpha;
	CCD->ActorManager()->GetAlpha(actor, &fAlpha);
	geActor_SetAlpha(m_Weapons[weaponName]->PActor, fAlpha);

	if(m_ViewPoint == FIRSTPERSON)
		return;

	geVec3d front, back;
	geVec3d direction;
	geExtBox extBox;
	GE_Collision collision;
	float currentDistance;

	extBox.Min.X = extBox.Min.Y = extBox.Min.Z = -1.0f;
	extBox.Max.X = extBox.Max.Y = extBox.Max.Z = 1.0f;
	currentDistance = 4000.0f;

	if(m_Weapons[m_CurrentWeapon]->PBone[0] != '\0')
		geActor_GetBoneTransform(m_Weapons[m_CurrentWeapon]->PActor, m_Weapons[m_CurrentWeapon]->PBone, &xForm);
	else
		geActor_GetBoneTransform(m_Weapons[m_CurrentWeapon]->PActor, RootBoneName(m_Weapons[m_CurrentWeapon]->PActor), &xForm);

	position = xForm.Translation;

	geXForm3d_SetZRotation(&xForm, rotation.Z);
	geXForm3d_RotateX(&xForm, rotation.X - CCD->CameraManager()->GetTilt());
	geXForm3d_RotateY(&xForm, rotation.Y);

	geXForm3d_GetIn(&xForm, &direction);
	geVec3d_AddScaled(&position, &direction, currentDistance, &back);

	geVec3d_Copy(&position, &front);

	if(geWorld_Collision(CCD->World(), &extBox.Min, &extBox.Max, &front,
		&back, GE_VISIBLE_CONTENTS, GE_COLLIDE_ALL, 0, NULL, NULL, &collision))
	{
		currentDistance = geVec3d_DistanceBetween(&collision.Impact, &front);

		if(currentDistance > 4000.0f)
			currentDistance = 4000.0f;

		geVec3d_AddScaled(&position, &direction, currentDistance, &back);
	}

	geCamera_TransformAndProject(CCD->CameraManager()->Camera(), &back, &m_ProjectedPoint);
}

/* ------------------------------------------------------------------------------------ */
// DisplayFirstPerson
/* ------------------------------------------------------------------------------------ */
void CWeapon::DisplayFirstPerson(const std::string& weaponName)
{
	if(CCD->PositionWeapon()) // used to move actor around in setup mode
	{
		if(!CCD->GetAlterKey())
		{
			if((GetAsyncKeyState(0x69) & 0x8000) != 0)
				m_Weapons[weaponName]->G += 0.01f;
			if((GetAsyncKeyState(0x66) & 0x8000) != 0)
				m_Weapons[weaponName]->G -= 0.01f;

			if((GetAsyncKeyState(0x67) & 0x8000) != 0 && (GetAsyncKeyState(0x60) & 0x8000) == 0)
				m_Weapons[weaponName]->F += 0.1f;
			if((GetAsyncKeyState(0x68) & 0x8000) != 0 && (GetAsyncKeyState(0x60) & 0x8000) == 0)
				m_Weapons[weaponName]->F -= 0.1f;

			if((GetAsyncKeyState(0x64) & 0x8000) != 0 && (GetAsyncKeyState(0x60) & 0x8000) == 0)
				m_Weapons[weaponName]->H += 0.1f;
			if((GetAsyncKeyState(0x65) & 0x8000) != 0 && (GetAsyncKeyState(0x60) & 0x8000) == 0)
				m_Weapons[weaponName]->H -= 0.1f;

			if((GetAsyncKeyState(0x61) & 0x8000) != 0 && (GetAsyncKeyState(0x60) & 0x8000) == 0)
				m_Weapons[weaponName]->J += 0.1f;
			if((GetAsyncKeyState(0x62) & 0x8000) != 0 && (GetAsyncKeyState(0x60) & 0x8000) == 0)
				m_Weapons[weaponName]->J -= 0.1f;

			if((GetAsyncKeyState(0x67) & 0x8000) != 0 && (GetAsyncKeyState(0x60) & 0x8000) != 0)
				m_Weapons[weaponName]->K += 0.1f;
			if((GetAsyncKeyState(0x68) & 0x8000) != 0 && (GetAsyncKeyState(0x60) & 0x8000) != 0)
				m_Weapons[weaponName]->K -= 0.1f;

			if((GetAsyncKeyState(0x64) & 0x8000) != 0 && (GetAsyncKeyState(0x60) & 0x8000) != 0)
				m_Weapons[weaponName]->L += 0.1f;
			if((GetAsyncKeyState(0x65) & 0x8000) != 0 && (GetAsyncKeyState(0x60) & 0x8000) != 0)
				m_Weapons[weaponName]->L -= 0.1f;

			if((GetAsyncKeyState(0x61) & 0x8000) != 0 && (GetAsyncKeyState(0x60) & 0x8000) != 0)
				m_Weapons[weaponName]->Z += 0.1f;
			if((GetAsyncKeyState(0x62) & 0x8000) != 0 && (GetAsyncKeyState(0x60) & 0x8000) != 0)
				m_Weapons[weaponName]->Z -= 0.1f;
		}
		else
		{
			if((GetAsyncKeyState(0x51) & 0x8000) != 0)
				m_Weapons[weaponName]->G += 0.01f;
			if((GetAsyncKeyState(0x57) & 0x8000) != 0)
				m_Weapons[weaponName]->G -= 0.01f;

			if((GetAsyncKeyState(0x4A) & 0x8000) != 0 && (GetAsyncKeyState(0x59) & 0x8000) == 0)
				m_Weapons[weaponName]->F += 0.1f;
			if((GetAsyncKeyState(0x4B) & 0x8000) != 0 && (GetAsyncKeyState(0x59) & 0x8000) == 0)
				m_Weapons[weaponName]->F -= 0.1f;

			if((GetAsyncKeyState(0x4D) & 0x8000) != 0 && (GetAsyncKeyState(0x59) & 0x8000) == 0)
				m_Weapons[weaponName]->H += 0.1f;
			if((GetAsyncKeyState(0x4E) & 0x8000) != 0 && (GetAsyncKeyState(0x59) & 0x8000) == 0)
				m_Weapons[weaponName]->H -= 0.1f;

			if((GetAsyncKeyState(0x4F) & 0x8000) != 0 && (GetAsyncKeyState(0x59) & 0x8000) == 0)
				m_Weapons[weaponName]->J += 0.1f;
			if((GetAsyncKeyState(0x50) & 0x8000) != 0 && (GetAsyncKeyState(0x59) & 0x8000) == 0)
				m_Weapons[weaponName]->J -= 0.1f;

			if((GetAsyncKeyState(0x4A) & 0x8000) != 0 && (GetAsyncKeyState(0x59) & 0x8000) != 0)
				m_Weapons[weaponName]->K += 0.1f;
			if((GetAsyncKeyState(0x4B) & 0x8000) != 0 && (GetAsyncKeyState(0x59) & 0x8000) != 0)
				m_Weapons[weaponName]->K -= 0.1f;

			if((GetAsyncKeyState(0x4D) & 0x8000) != 0 && (GetAsyncKeyState(0x59) & 0x8000) != 0)
				m_Weapons[weaponName]->L += 0.1f;
			if((GetAsyncKeyState(0x4E) & 0x8000) != 0 && (GetAsyncKeyState(0x59) & 0x8000) != 0)
				m_Weapons[weaponName]->L -= 0.1f;

			if((GetAsyncKeyState(0x4F) & 0x8000) != 0 && (GetAsyncKeyState(0x59) & 0x8000) != 0)
				m_Weapons[weaponName]->Z += 0.1f;
			if((GetAsyncKeyState(0x50) & 0x8000) != 0 && (GetAsyncKeyState(0x59) & 0x8000) != 0)
				m_Weapons[weaponName]->Z -= 0.1f;
		}
	}

	geXForm3d xForm;
	geXForm3d_SetIdentity(&xForm);

	if(CCD->PositionWeapon())
	{
		geActor_SetScale(m_Weapons[weaponName]->VActor,
			m_Weapons[weaponName]->VScale + m_Weapons[weaponName]->G,
			m_Weapons[weaponName]->VScale + m_Weapons[weaponName]->G,
			m_Weapons[weaponName]->VScale + m_Weapons[weaponName]->G);

		// VActorRotation converted to radians at load time
		geXForm3d_RotateX(&xForm, m_Weapons[weaponName]->VActorRotation.X + m_Weapons[weaponName]->K * GE_PIOVER180);
		geXForm3d_RotateZ(&xForm, m_Weapons[weaponName]->VActorRotation.Z + m_Weapons[weaponName]->L * GE_PIOVER180);
		geXForm3d_RotateY(&xForm, m_Weapons[weaponName]->VActorRotation.Y + m_Weapons[weaponName]->Z * GE_PIOVER180);
	}
	else
	{
		geActor_SetScale(m_Weapons[weaponName]->VActor,
			m_Weapons[weaponName]->VScale,
			m_Weapons[weaponName]->VScale,
			m_Weapons[weaponName]->VScale);

		// rotate actor into place
		// VActorRotation converted to radians at load time
		geXForm3d_RotateX(&xForm, m_Weapons[weaponName]->VActorRotation.X);
		geXForm3d_RotateZ(&xForm, m_Weapons[weaponName]->VActorRotation.Z);
		geXForm3d_RotateY(&xForm, m_Weapons[weaponName]->VActorRotation.Y);
	}

	geVec3d rotation;
	geVec3d position;
	float shx, shy, shz = 0.0f;

	CCD->CameraManager()->TrackMotion();
	CCD->CameraManager()->GetRotation(&rotation);
	CCD->CameraManager()->GetPosition(&position);
	CCD->CameraManager()->GetShake(&shx, &shy);

	if(CCD->Player()->GetHeadBobbing())
	{
		if(m_Weapons[weaponName]->BobAmt > 0.0f)
			shz = CCD->CameraManager()->GetHeadBobOffset() / m_Weapons[weaponName]->BobAmt;
	}

	geXForm3d_RotateX(&xForm, rotation.Z);						// rotate left/right
	geXForm3d_RotateZ(&xForm, rotation.X - 0.87266462599715f);	// rotate up/down (-5.f*GE_PIOVER180)
	geXForm3d_RotateY(&xForm, rotation.Y + GE_PIOVER2);			// swing side to side
	geXForm3d_Translate(&xForm, position.X + shx, position.Y + shy, position.Z);

	geVec3d forward;
	geVec3d right;
	geVec3d down;

	geXForm3d_GetLeft(&xForm, &forward);
	geXForm3d_GetUp(&xForm, &right);
	geXForm3d_GetIn(&xForm, &down);

	if(CCD->PositionWeapon())
	{
		geVec3d_Scale(&forward, m_Weapons[weaponName]->VActorOffset.X + m_Weapons[weaponName]->F, &forward);	// in
		geVec3d_Scale(&right,	m_Weapons[weaponName]->VActorOffset.Y + m_Weapons[weaponName]->H, &right);		// up
		geVec3d_Scale(&down,	m_Weapons[weaponName]->VActorOffset.Z + m_Weapons[weaponName]->J, &down);		// left
	}
	else
	{
		geVec3d_Scale(&forward, m_Weapons[weaponName]->VActorOffset.X, &forward);	// in
		geVec3d_Scale(&right,	m_Weapons[weaponName]->VActorOffset.Y, &right);		// up
		geVec3d_Scale(&down,	m_Weapons[weaponName]->VActorOffset.Z, &down);		// left
	}

	geXForm3d_Translate(&xForm, forward.X,	forward.Y,	forward.Z);
	geXForm3d_Translate(&xForm, right.X,	right.Y,	right.Z);
	geXForm3d_Translate(&xForm, down.X,		down.Y,		down.Z);

	geXForm3d_GetIn(&xForm, &forward);
	geVec3d_AddScaled(&xForm.Translation, &forward, shz, &xForm.Translation);

	float deltaTime, tStart, tEnd;
	geMotion *actorMotion = NULL;;

	if(m_VAnimTime < 0.0f)
		m_VAnimTime = CCD->FreeRunningCounter_F();

	deltaTime = 0.001f * (CCD->FreeRunningCounter_F() - m_VAnimTime);

	if(m_VSequence == VWEPHOLSTER)
		m_VMCounter -= (deltaTime * m_Weapons[weaponName]->VAnimSpeed);
	else
		m_VMCounter += (deltaTime * m_Weapons[weaponName]->VAnimSpeed);

	switch(m_VSequence)
	{
	case VWEPCHANGE:
		actorMotion = geActor_GetMotionByName(m_Weapons[weaponName]->VActorDef, m_Weapons[weaponName]->VArm.c_str());
		break;
	case VWEPHOLSTER:
		actorMotion = geActor_GetMotionByName(m_Weapons[weaponName]->VActorDef, m_Weapons[weaponName]->VArm.c_str());
		break;
	case VWEPRELOAD:
		actorMotion = geActor_GetMotionByName(m_Weapons[weaponName]->VActorDef, m_Weapons[weaponName]->VReload.c_str());
		break;
	case VWEPKEYRELOAD:
		actorMotion = geActor_GetMotionByName(m_Weapons[weaponName]->VActorDef, m_Weapons[weaponName]->VKeyReload.c_str());
		break;
	case VWEPATTACKEMPTY:
		actorMotion = geActor_GetMotionByName(m_Weapons[weaponName]->VActorDef, m_Weapons[weaponName]->VAttackEmpty.c_str());
		break;
	case VWEPALTATTACK:
		actorMotion = geActor_GetMotionByName(m_Weapons[weaponName]->VActorDef, m_Weapons[weaponName]->VAltAttack.c_str());
		if(!actorMotion)
		{
			actorMotion = geActor_GetMotionByName(m_Weapons[weaponName]->VActorDef, m_Weapons[weaponName]->VAttack.c_str());
			m_VSequence = VWEPATTACK;
		}
		break;
	case VWEPUSE:
		actorMotion = geActor_GetMotionByName(m_Weapons[weaponName]->VActorDef, m_Weapons[weaponName]->VUse.c_str());
		break;
	case VWEPHIT:
		actorMotion = geActor_GetMotionByName(m_Weapons[weaponName]->VActorDef, m_Weapons[weaponName]->VHit.c_str());
		break;
	case VWEPALTHIT:
		actorMotion = geActor_GetMotionByName(m_Weapons[weaponName]->VActorDef, m_Weapons[weaponName]->VAltHit.c_str());
		if(!actorMotion)
		{
			actorMotion = geActor_GetMotionByName(m_Weapons[weaponName]->VActorDef, m_Weapons[weaponName]->VHit.c_str());
			m_VSequence = VWEPHIT;
		}
		break;
	case VWEPWALK:
		actorMotion = geActor_GetMotionByName(m_Weapons[weaponName]->VActorDef, m_Weapons[weaponName]->VWalk.c_str());
		break;
	case VWEPIDLE:
		actorMotion = geActor_GetMotionByName(m_Weapons[weaponName]->VActorDef, m_Weapons[weaponName]->VIdle.c_str());
		break;
	case VWEPATTACK:
		actorMotion = geActor_GetMotionByName(m_Weapons[weaponName]->VActorDef, m_Weapons[weaponName]->VAttack.c_str());
		break;
	}

	if(!actorMotion) // no animation so use default
		actorMotion = geActor_GetMotionByName(m_Weapons[weaponName]->VActorDef, m_Weapons[weaponName]->VIdle.c_str());

	// test if at end of animation
	if(actorMotion)
	{
		geMotion_GetTimeExtents(actorMotion, &tStart, &tEnd);

		if(m_VMCounter <= tStart && m_VSequence == VWEPHOLSTER) // finish holster
		{
			m_CurrentWeapon.clear();
			m_Holstered = true;
			geWorld_SetActorFlags(CCD->World(), m_Weapons[weaponName]->VActor, 0);

			if(m_DropFlag)
			{
				CPersistentAttributes *inventory = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
				inventory->Modify(m_Weapons[weaponName]->Name, -1);
				m_Slot[m_Weapons[weaponName]->Slot].clear();
				m_DropFlag = false;
				SpawnWeaponAttribute(weaponName);
			}
		}

		if(m_VMCounter > tEnd && m_VSequence == VWEPCHANGE) // switch from change to idle
		{
			m_VSequence = VWEPIDLE;
			actorMotion = geActor_GetMotionByName(m_Weapons[weaponName]->VActorDef, m_Weapons[weaponName]->VIdle.c_str());
		}

		if(m_VMCounter > tEnd
			&& (m_VSequence == VWEPATTACK		|| m_VSequence == VWEPATTACKEMPTY ||
				m_VSequence == VWEPALTATTACK	|| m_VSequence == VWEPHIT ||
				m_VSequence == VWEPUSE			|| m_VSequence == VWEPALTHIT ||
				m_VSequence == VWEPRELOAD		|| m_VSequence == VWEPKEYRELOAD)) // switch from shoot to idle
		{
			geActor *theActor = CCD->Player()->GetActor();
			CPersistentAttributes *inventory = CCD->ActorManager()->Inventory(theActor);

			if(m_VSequence == VWEPKEYRELOAD || m_VSequence == VWEPRELOAD)
			{
				m_Weapons[weaponName]->ShotFired = 0;

				if(inventory->Value(m_Weapons[weaponName]->Ammunition) >= m_Weapons[weaponName]->ShotperMag)
					m_Weapons[weaponName]->MagAmt = m_Weapons[weaponName]->ShotperMag;
				else
					m_Weapons[weaponName]->MagAmt = inventory->Value(m_Weapons[weaponName]->Ammunition);
			}

			bool magflag = false;

			if(m_Weapons[weaponName]->Catagory == PROJECTILE && m_VSequence == VWEPATTACK)
			{
				if(inventory->Value(m_Weapons[weaponName]->Ammunition) >= m_Weapons[weaponName]->AmmoPerShot)
				{
					if(m_Weapons[weaponName]->ShotperMag > 0 && m_Weapons[weaponName]->ShotFired >= m_Weapons[weaponName]->ShotperMag)
					{
						magflag = true;
						m_VSequence = VWEPRELOAD;
						actorMotion = geActor_GetMotionByName(m_Weapons[weaponName]->VActorDef, m_Weapons[weaponName]->VReload.c_str());

						if(!m_Weapons[m_CurrentWeapon]->ReloadSound.empty())
						{
							Snd sound;
							memset(&sound, 0, sizeof(sound));
							CCD->CameraManager()->GetPosition(&sound.Pos);
							sound.Min = CCD->Level()->GetAudibleRadius();
							sound.Loop = GE_FALSE;
							sound.SoundDef = SPool_Sound(m_Weapons[m_CurrentWeapon]->ReloadSound.c_str());
							CCD->EffectManager()->Item_Add(EFF_SND, static_cast<void*>(&sound));
						}
					}
				}
			}

			if(!magflag)
			{
				if(CCD->Player()->GetMoving() == MOVEIDLE)
				{
					m_VSequence = VWEPIDLE;
					actorMotion = geActor_GetMotionByName(m_Weapons[weaponName]->VActorDef, m_Weapons[weaponName]->VIdle.c_str());
				}
				else
				{
					m_VSequence = VWEPWALK;
					actorMotion = geActor_GetMotionByName(m_Weapons[weaponName]->VActorDef, m_Weapons[weaponName]->VWalk.c_str());
				}
			}

			m_VBlend = false;
		}

		if(m_VMCounter > tEnd)
			m_VMCounter = 0.0f; // Wrap animation

		if(m_VSequence == VWEPWALK && m_VMCounter == 0.0f && (CCD->Player()->GetMoving() == MOVEIDLE))
		{
			m_VSequence = VWEPIDLE;
			m_VBlend = false; // all done blending
		}

		if(m_VSequence == VWEPIDLE && m_VMCounter == 0.0f && (CCD->Player()->GetMoving() != MOVEIDLE))
		{
			m_VSequence = VWEPWALK;
			m_VBlend = false; // all done blending
		}

		// set actor animation
		geActor_SetPose(m_Weapons[weaponName]->VActor, actorMotion, m_VMCounter, &xForm);
	}
	else
	{
		geActor_ClearPose(m_Weapons[weaponName]->VActor, &xForm);
	}

	m_VAnimTime = CCD->FreeRunningCounter_F();
}

/* ------------------------------------------------------------------------------------ */
// Rendering
/* ------------------------------------------------------------------------------------ */
void CWeapon::Rendering(bool flag)
{
	if(m_CurrentWeapon.empty())
		return;

	if(m_ViewPoint == FIRSTPERSON)
	{
		if(m_Weapons[m_CurrentWeapon]->VActor)
		{
			geWorld_SetActorFlags(CCD->World(), m_Weapons[m_CurrentWeapon]->VActor, flag?GE_ACTOR_RENDER_NORMAL:0);
		}
	}
	else
	{
		if(m_Weapons[m_CurrentWeapon]->PActor)
		{
			geWorld_SetActorFlags(CCD->World(), m_Weapons[m_CurrentWeapon]->PActor, flag?(GE_ACTOR_RENDER_NORMAL|GE_ACTOR_RENDER_MIRRORS):0);
		}
	}
}

/* ------------------------------------------------------------------------------------ */
// Holster
/* ------------------------------------------------------------------------------------ */
void CWeapon::Holster()
{
	if(m_CurrentWeapon.empty())
		return;

	if(m_Weapons[m_CurrentWeapon]->Slot == MAX_WEAPONS-1) // default weapon that is always visible
		return;

	if(m_Weapons[m_CurrentWeapon]->Catagory == PROJECTILE
		|| (m_Weapons[m_CurrentWeapon]->Catagory == MELEE && m_Weapons[m_CurrentWeapon]->MeleeAmmo == true))
	{
		CCD->HUD()->ActivateElement(m_Weapons[m_CurrentWeapon]->Ammunition, false);
	}

	CCD->CameraManager()->CancelZoom();

	if(m_ViewPoint == FIRSTPERSON)
	{
		if(m_VSequence == VWEPHOLSTER)
			return;

		geMotion* actorMotion = geActor_GetMotionByName(m_Weapons[m_CurrentWeapon]->VActorDef, m_Weapons[m_CurrentWeapon]->VArm.c_str());

		if(actorMotion)
		{
			float tStart, tEnd;
			geMotion_GetTimeExtents(actorMotion, &tStart, &tEnd);
			m_VMCounter = tEnd;
			m_VSequence = VWEPHOLSTER;
			m_VAnimTime = -1.0f;
			m_AttackTime = 0;
			m_VBlend = false;
			m_AttackFlag = false;
		}
		else
		{
			m_CurrentWeapon.clear();// = MAX_WEAPONS;
			m_Holstered = true;
		}
	}
	else
	{
		m_AttackFlag = false;
		geWorld_SetActorFlags(CCD->World(), m_Weapons[m_CurrentWeapon]->PActor, 0);

		if(m_DropFlag)
		{
			CPersistentAttributes *inventory = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
			inventory->Modify(m_Weapons[m_CurrentWeapon]->Name, -1);
			m_Slot[m_Weapons[m_CurrentWeapon]->Slot].clear();
			m_DropFlag = false;
			SpawnWeaponAttribute(m_CurrentWeapon);
		}

		m_CurrentWeapon.clear();// = MAX_WEAPONS;
		m_Holstered = true;
	}
}

/* ------------------------------------------------------------------------------------ */
// Use
/* ------------------------------------------------------------------------------------ */
void CWeapon::Use()
{
	if(m_CurrentWeapon.empty())
		return;

	if(m_ViewPoint == FIRSTPERSON)
	{
		if(m_Weapons[m_CurrentWeapon]->VUse.empty())
			return;

		if(m_VSequence == VWEPUSE)
			return;

		m_VSequence = VWEPUSE;
		m_VAnimTime = -1.0f;
		m_AttackTime = 0;
		m_VMCounter = 0.0f;
		m_VBlend = false;
		m_AttackFlag = false;

		if(!m_Weapons[m_CurrentWeapon]->UseSound.empty())
		{
			Snd sound;
			geVec3d origin = CCD->Player()->Position();
			memset(&sound, 0, sizeof(sound));
			geVec3d_Copy(&origin, &sound.Pos);
			sound.Min = CCD->Level()->GetAudibleRadius();
			sound.Loop = GE_FALSE;
			sound.SoundDef = SPool_Sound(m_Weapons[m_CurrentWeapon]->UseSound.c_str());

			if(sound.SoundDef)
				CCD->EffectManager()->Item_Add(EFF_SND, static_cast<void*>(&sound));
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
	if(m_AttackFlag)
	{
		m_AttackFlag = false;

		switch(m_Weapons[m_CurrentWeapon]->Catagory)
		{
		case PROJECTILE:
			{
				ProjectileAttack();
				break;
			}
		case MELEE:
			{
				MeleeAttack();
				break;
			}
		}
	}
}

/* ------------------------------------------------------------------------------------ */
// DisplayZoom
/* ------------------------------------------------------------------------------------ */
void CWeapon::DisplayZoom()
{
	if(m_CurrentWeapon.empty())
		return;

	if(CCD->CameraManager()->GetZooming())
	{
		if(m_Weapons[m_CurrentWeapon]->ZoomOverlay)
		{
			CCD->Engine()->DrawAlphaBitmap(m_Weapons[m_CurrentWeapon]->ZoomOverlay,
											NULL,
											NULL,	// if null, uses full screen
											NULL,	// pixels in the "camera" view
											NULL,	// percent of the "camera" view
											255.0f,
											NULL,
											1.0f);
		}
	}
}

/* ------------------------------------------------------------------------------------ */
// SaveTo
/* ------------------------------------------------------------------------------------ */
int CWeapon::SaveTo(FILE* saveFD)
{
	int length;

	fwrite(&m_ViewPoint,				sizeof(int),	1,		saveFD);
	for(int i=0; i<MAX_WEAPONS; ++i)
	{
		length = m_Slot[i].length() + 1;
		fwrite(&length,					sizeof(int),	1,		saveFD);
		fwrite(m_Slot[i].c_str(),		sizeof(char),	length,	saveFD);
	}
	length = m_CurrentWeapon.length() + 1;
	fwrite(&length,						sizeof(int),	1,		saveFD);
	fwrite(m_CurrentWeapon.c_str(),		sizeof(char),	length,	saveFD);

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// RestoreFrom
/* ------------------------------------------------------------------------------------ */
int CWeapon::RestoreFrom(FILE* restoreFD)
{
	int length;
	char *buffer;

	fread(&m_ViewPoint,		sizeof(int),	1,		restoreFD);
	for(int i=0; i<MAX_WEAPONS; ++i)
	{
		fread(&length,			sizeof(int),	1,		restoreFD);
		buffer = new char[length];
		fread(buffer,			sizeof(char),length,	restoreFD);
		m_Slot[i] = buffer;
		delete [] buffer;
	}
	fread(&length,			sizeof(int),	1,		restoreFD);
	buffer = new char[length];
	fread(buffer,			sizeof(char),length,	restoreFD);
	m_CurrentWeapon = buffer;
	delete [] buffer;

	ResetWeapon(m_CurrentWeapon);

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// SetWeapon
/* ------------------------------------------------------------------------------------ */
void CWeapon::SetWeapon(const std::string& weapon)
{
	if(m_Weapons.find(weapon) != m_Weapons.end())
	{
		SetWeapon(m_Weapons[weapon]->Slot);
	}
	else
	{
		Holster();
	}
}

/* ------------------------------------------------------------------------------------ */
// SetWeapon
//
// change to new weapon
/* ------------------------------------------------------------------------------------ */
void CWeapon::SetWeapon(int slot)
{
	if(slot < 0 || slot >= MAX_WEAPONS)
		return;

	if(m_Slot[slot].empty())
		return;

	if(!m_CurrentWeapon.empty())
	{
		if(m_Weapons[m_CurrentWeapon]->Slot == slot) // same weapon?
			return;

		// stop displaying old weapon
		if(m_Weapons[m_CurrentWeapon]->VActor)
			geWorld_SetActorFlags(CCD->World(), m_Weapons[m_CurrentWeapon]->VActor, 0);

		if(m_Weapons[m_CurrentWeapon]->PActor)
			geWorld_SetActorFlags(CCD->World(), m_Weapons[m_CurrentWeapon]->PActor, 0);

		if(m_Weapons[m_CurrentWeapon]->Catagory == PROJECTILE ||
			(m_Weapons[m_CurrentWeapon]->Catagory == MELEE && m_Weapons[m_CurrentWeapon]->MeleeAmmo == true))
			CCD->HUD()->ActivateElement(m_Weapons[m_CurrentWeapon]->Ammunition, false);
	}

	m_CurrentWeapon = m_Slot[slot]; // change to new weapon
	m_Holstered = false;
	std::string motion = CCD->ActorManager()->GetMotion(CCD->Player()->GetActor());
	int index = -1;

	for(int i=0; i<ANIMMAX; ++i)
	{
		if(motion == m_Weapons[m_CurrentWeapon]->Animations[i])
		{
			index = i;
			break;
		}
	}

	if(index != -1)
	{
		CCD->ActorManager()->SetMotion(CCD->Player()->GetActor(), PlayerAnim(index));
	}

	if(m_Weapons[m_CurrentWeapon]->Catagory == PROJECTILE ||
			(m_Weapons[m_CurrentWeapon]->Catagory == MELEE &&
			 m_Weapons[m_CurrentWeapon]->MeleeAmmo == true))
	{
		CCD->HUD()->ActivateElement(m_Weapons[m_CurrentWeapon]->Ammunition, true);

		if(m_Weapons[m_CurrentWeapon]->ShotperMag > 0)
		{
			if(m_Weapons[m_CurrentWeapon]->MagAmt == -1)
			{
				geActor *actor = CCD->Player()->GetActor();
				CPersistentAttributes *inventory = CCD->ActorManager()->Inventory(actor);

				if(inventory->Value(m_Weapons[m_CurrentWeapon]->Ammunition) >= m_Weapons[m_CurrentWeapon]->ShotperMag)
					m_Weapons[m_CurrentWeapon]->MagAmt = m_Weapons[m_CurrentWeapon]->ShotperMag;
				else
					m_Weapons[m_CurrentWeapon]->MagAmt = inventory->Value(m_Weapons[m_CurrentWeapon]->Ammunition);
			}
		}
	}

	DoChange();
	m_VSequence = VWEPCHANGE;
	m_VAnimTime = -1.0f;
	m_AttackTime = 0;
	m_VMCounter = 0.0f;
	m_VBlend = false;
	m_AttackFlag = false;
}

/* ------------------------------------------------------------------------------------ */
// DoChange
/* ------------------------------------------------------------------------------------ */
void CWeapon::DoChange()
{
	if(m_Weapons[m_CurrentWeapon]->FixedView != -1 && !CCD->Player()->GetMonitorMode())
	{
		if(m_ViewPoint != m_Weapons[m_CurrentWeapon]->FixedView)
		{
			int TempView = m_OldViewPoint;
			CCD->Player()->SwitchCamera(m_Weapons[m_CurrentWeapon]->FixedView);
			m_OldViewPoint = TempView;
		}
	}
	else
	{
		if(m_ViewPoint != m_OldViewPoint)
		{
			int TempView = m_OldViewPoint;
			CCD->Player()->SwitchCamera(m_OldViewPoint);
			m_OldViewPoint = TempView;
		}
	}
	if(m_ViewPoint == FIRSTPERSON)
	{
		if(m_Weapons[m_CurrentWeapon]->VActor)
			geWorld_SetActorFlags(CCD->World(), m_Weapons[m_CurrentWeapon]->VActor, GE_ACTOR_RENDER_NORMAL);
	}
	else
	{
		if(m_Weapons[m_CurrentWeapon]->PActor)
			geWorld_SetActorFlags(CCD->World(), m_Weapons[m_CurrentWeapon]->PActor, GE_ACTOR_RENDER_NORMAL | GE_ACTOR_RENDER_MIRRORS);
	}
}

/* ------------------------------------------------------------------------------------ */
// ClearWeapon
/* ------------------------------------------------------------------------------------ */
void CWeapon::ClearWeapon()
{
	if(!m_CurrentWeapon.empty())
	{
		if(m_Weapons[m_CurrentWeapon]->Catagory == PROJECTILE ||
			(m_Weapons[m_CurrentWeapon]->Catagory == MELEE && m_Weapons[m_CurrentWeapon]->MeleeAmmo == true))
			CCD->HUD()->ActivateElement(m_Weapons[m_CurrentWeapon]->Ammunition, false);
	}
}

/* ------------------------------------------------------------------------------------ */
// ResetWeapon
/* ------------------------------------------------------------------------------------ */
void CWeapon::ResetWeapon(const std::string& value)
{
	std::string motion = CCD->ActorManager()->GetMotion(CCD->Player()->GetActor());
	int index = -1;

	if(!value.empty())
	{
		for(int i=0; i<ANIMMAX; ++i)
		{
			if(motion == m_Weapons[m_CurrentWeapon]->Animations[i])
			{
				index = i;
				break;
			}
		}
	}

	m_CurrentWeapon = value; // change to new weapon

	if(index != -1)
	{
		CCD->ActorManager()->SetMotion(CCD->Player()->GetActor(), PlayerAnim(index));
	}

	if(m_CurrentWeapon.empty())
		return;

	m_Holstered = false;

	if(m_Weapons[m_CurrentWeapon]->Catagory == PROJECTILE ||
		(m_Weapons[m_CurrentWeapon]->Catagory == MELEE && m_Weapons[m_CurrentWeapon]->MeleeAmmo == true))
	{
		CCD->HUD()->ActivateElement(m_Weapons[m_CurrentWeapon]->Ammunition, true);

		if(m_Weapons[m_CurrentWeapon]->Catagory == PROJECTILE)
		{
			if(m_Weapons[m_CurrentWeapon]->MagAmt == -1)
			{
				geActor *actor = CCD->Player()->GetActor();
				CPersistentAttributes *inventory = CCD->ActorManager()->Inventory(actor);

				if(inventory->Value(m_Weapons[m_CurrentWeapon]->Ammunition) >= m_Weapons[m_CurrentWeapon]->ShotperMag)
					m_Weapons[m_CurrentWeapon]->MagAmt = m_Weapons[m_CurrentWeapon]->ShotperMag;
				else
					m_Weapons[m_CurrentWeapon]->MagAmt = inventory->Value(m_Weapons[m_CurrentWeapon]->Ammunition);
			}
		}
	}

	DoChange();
	m_VSequence = VWEPCHANGE;
	m_VAnimTime = -1.0f;
	m_AttackTime = 0;
	m_VMCounter = 0.0f;
	m_VBlend = false;
	m_AttackFlag = false;
}

/* ------------------------------------------------------------------------------------ */
// CrossHair
/* ------------------------------------------------------------------------------------ */
bool CWeapon::CrossHair()
{
	if(m_CurrentWeapon.empty())
		return false;

	if(m_Weapons[m_CurrentWeapon]->Catagory != PROJECTILE ||
		!(m_ViewPoint == FIRSTPERSON || m_ViewPoint == THIRDPERSON))
		return false;

	return true;
}

/* ------------------------------------------------------------------------------------ */
// CrossHairFixed
/* ------------------------------------------------------------------------------------ */
bool CWeapon::CrossHairFixed()
{
	return m_Weapons[m_CurrentWeapon]->CrossHairFixed;
}

/* ------------------------------------------------------------------------------------ */
// DisplayCrossHair
/* ------------------------------------------------------------------------------------ */
void CWeapon::DisplayCrossHair()
{
	geBitmap *bmp = m_Weapons[m_CurrentWeapon]->CrossHair;

	if(bmp)
	{
		geBitmap_Info bmpInfo;

		if(geBitmap_GetInfo(bmp, &bmpInfo, NULL) == GE_TRUE)
		{
			int x = (CCD->Engine()->Width() - bmpInfo.Width) / 2;
			int y = (CCD->Engine()->Height() - bmpInfo.Height) / 2;
			geEngine_DrawBitmap(CCD->Engine()->Engine(), bmp, NULL, x, y);
		}
	}
}

/* ------------------------------------------------------------------------------------ */
// GetCrossHair
/* ------------------------------------------------------------------------------------ */
geBitmap* CWeapon::GetCrossHair()
{
	return m_Weapons[m_CurrentWeapon]->CrossHair;
}

/* ------------------------------------------------------------------------------------ */
// PlayerAnim
/* ------------------------------------------------------------------------------------ */
const std::string& CWeapon::PlayerAnim(int index)
{
	if(m_Weapons.find(m_CurrentWeapon) != m_Weapons.end())
	{
		if(!m_Weapons[m_CurrentWeapon]->Animations[index].empty())
		{
			return m_Weapons[m_CurrentWeapon]->Animations[index];
		}
	}

	return CCD->Player()->GetAnimations(index);
}

/* ------------------------------------------------------------------------------------ */
// DieAnim
/* ------------------------------------------------------------------------------------ */
std::string CWeapon::DieAnim()
{
	if(m_Weapons.find(m_CurrentWeapon) != m_Weapons.end())
	{
		if(m_Weapons[m_CurrentWeapon]->DieAnimAmt > 0)
		{
			return m_Weapons[m_CurrentWeapon]->DieAnim[rand() % m_Weapons[m_CurrentWeapon]->DieAnimAmt];
		}
	}

	return "";
}

/* ------------------------------------------------------------------------------------ */
// InjuryAnim
/* ------------------------------------------------------------------------------------ */
std::string CWeapon::InjuryAnim()
{
	if(m_Weapons.find(m_CurrentWeapon) != m_Weapons.end())
	{
		if(m_Weapons[m_CurrentWeapon]->InjuryAnimAmt > 0)
		{
			return m_Weapons[m_CurrentWeapon]->InjuryAnim[rand() % m_Weapons[m_CurrentWeapon]->InjuryAnimAmt];
		}
	}

	return "";
}

/* ------------------------------------------------------------------------------------ */
// WeaponData
/* ------------------------------------------------------------------------------------ */
void CWeapon::WeaponData()
{
	if(m_ViewPoint == FIRSTPERSON && m_Weapons.find(m_CurrentWeapon) != m_Weapons.end())
	{
		if(CCD->PositionWeapon())
		{
			char szData[256];

			// VActorRotation converted to radians at load time
			// display rotation in degrees
			sprintf(szData, "Weapon Rotation:  X = %.2f  Y = %.2f  Z = %.2f",
				m_Weapons[m_CurrentWeapon]->VActorRotation.X * GE_180OVERPI + m_Weapons[m_CurrentWeapon]->K,
				m_Weapons[m_CurrentWeapon]->VActorRotation.Y * GE_180OVERPI + m_Weapons[m_CurrentWeapon]->Z,
				m_Weapons[m_CurrentWeapon]->VActorRotation.Z * GE_180OVERPI + m_Weapons[m_CurrentWeapon]->L);

			{
				CEGUI::Rect draw_area(4.f,
										27.f * CCD->Engine()->Height() / 30.f,
										CCD->Engine()->Width() - 4.f,
										28.f * CCD->Engine()->Height() / 30.f);
				CCD->GUIManager()->DrawText(szData, "", draw_area, 1.0f);
			}

			sprintf(szData, "Weapon Offset: X = %.2f  Y = %.2f  Z = %.2f, Scale: %.2f",
				m_Weapons[m_CurrentWeapon]->VActorOffset.X + m_Weapons[m_CurrentWeapon]->F,
				m_Weapons[m_CurrentWeapon]->VActorOffset.Y + m_Weapons[m_CurrentWeapon]->H,
				m_Weapons[m_CurrentWeapon]->VActorOffset.Z + m_Weapons[m_CurrentWeapon]->J,
				m_Weapons[m_CurrentWeapon]->VScale + m_Weapons[m_CurrentWeapon]->G);

			{
				CEGUI::Rect draw_area(	4.f,
										28.f * CCD->Engine()->Height() / 30.f,
										CCD->Engine()->Width() - 4.f,
										29.f * CCD->Engine()->Height() / 30.f);
				CCD->GUIManager()->DrawText(szData, "", draw_area, 1.0f);
			}
		}
	}
}

/* ------------------------------------------------------------------------------------ */
// KeyReload
/* ------------------------------------------------------------------------------------ */
void CWeapon::KeyReload()
{
	if(m_CurrentWeapon.empty())
		return;
	if(m_Weapons[m_CurrentWeapon]->Catagory != PROJECTILE)
		return;
	if(m_Weapons[m_CurrentWeapon]->ShotperMag == 0)
		return;
	if(m_Weapons[m_CurrentWeapon]->ShotFired == 0)
		return;

	if(m_ViewPoint == FIRSTPERSON)
	{
		if(	m_VSequence == VWEPCHANGE ||
			m_VSequence == VWEPHOLSTER ||
			m_VSequence == VWEPATTACKEMPTY ||
			m_VSequence == VWEPRELOAD ||
			m_VSequence == VWEPKEYRELOAD)
			return; // shooting or changing weapon

		geActor *actor = CCD->Player()->GetActor();
		CPersistentAttributes *inventory = CCD->ActorManager()->Inventory(actor);

		if(inventory->Value(m_Weapons[m_CurrentWeapon]->Ammunition) >= m_Weapons[m_CurrentWeapon]->AmmoPerShot)
		{
			m_VSequence = VWEPRELOAD;

			if(!m_Weapons[m_CurrentWeapon]->VKeyReload.empty())
				m_VSequence = VWEPKEYRELOAD;

			m_VMCounter = 0.0f;

			int leftInMag = m_Weapons[m_CurrentWeapon]->ShotperMag - m_Weapons[m_CurrentWeapon]->ShotFired;

			if(leftInMag > 0 && m_Weapons[m_CurrentWeapon]->LooseMag)
				inventory->Modify(m_Weapons[m_CurrentWeapon]->Ammunition, -leftInMag);

			if(!m_Weapons[m_CurrentWeapon]->ReloadSound.empty())
			{
				Snd sound;
				memset(&sound, 0, sizeof(sound));
				CCD->CameraManager()->GetPosition(&sound.Pos);
				sound.Min = CCD->Level()->GetAudibleRadius();
				sound.Loop = GE_FALSE;
				sound.SoundDef = SPool_Sound(m_Weapons[m_CurrentWeapon]->ReloadSound.c_str());
				CCD->EffectManager()->Item_Add(EFF_SND, static_cast<void*>(&sound));
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
	if(m_CurrentWeapon.empty())
		return;

	m_DropFlag = true;
	Holster();
}

/* ------------------------------------------------------------------------------------ */
// SpawnWeaponAttribute
/* ------------------------------------------------------------------------------------ */
void CWeapon::SpawnWeaponAttribute(const std::string& weaponName)
{
	if(m_Weapons[weaponName]->DropActor[0])
	{
		Attribute tmpAttr;
		memset(&tmpAttr, 0, sizeof(Attribute));

		geVec3d Dir, theRotation;
		geVec3d Pos = CCD->Player()->Position();
		geXForm3d Xf;

		if(m_ViewPoint == FIRSTPERSON)
		{
			CCD->CameraManager()->GetRotation(&theRotation);
		}
		else
		{
			CCD->ActorManager()->GetRotate(CCD->Player()->GetActor(), &theRotation);
		}

		geXForm3d_SetYRotation(&Xf, theRotation.Y);

		Pos.Y += m_Weapons[weaponName]->DropActorOffset.Y;

		geXForm3d_GetIn(&Xf, &Dir);
		geVec3d_AddScaled(&Pos, &Dir, m_Weapons[weaponName]->DropActorOffset.Z, &Pos);

		geXForm3d_GetLeft(&Xf, &Dir);
		geVec3d_AddScaled(&Pos, &Dir, m_Weapons[weaponName]->DropActorOffset.X, &Pos);

		theRotation.X = m_Weapons[weaponName]->DropActorRotation.X;
		theRotation.Y = theRotation.Y*GE_180OVERPI + m_Weapons[weaponName]->DropActorRotation.Y;
		theRotation.Z = m_Weapons[weaponName]->DropActorRotation.Z;

		tmpAttr.origin					= Pos;
		tmpAttr.szActorName				= m_Weapons[weaponName]->DropActor;
		tmpAttr.ActorRotation			= theRotation;
		tmpAttr.AmbientColor.r			= m_Weapons[weaponName]->DropAmbientColor.X;
		tmpAttr.AmbientColor.g			= m_Weapons[weaponName]->DropAmbientColor.Y;
		tmpAttr.AmbientColor.b			= m_Weapons[weaponName]->DropAmbientColor.Z;
		tmpAttr.FillColor.r				= m_Weapons[weaponName]->DropFillColor.X;
		tmpAttr.FillColor.g				= m_Weapons[weaponName]->DropFillColor.Y;
		tmpAttr.FillColor.b				= m_Weapons[weaponName]->DropFillColor.Z;
		tmpAttr.AmbientLightFromFloor	= m_Weapons[weaponName]->DropAmbientLightFromFloor;
		tmpAttr.EnvironmentMapping		= m_Weapons[weaponName]->DropEnvironmentMapping;
		tmpAttr.AllMaterial				= m_Weapons[weaponName]->DropAllMaterial;
		tmpAttr.PercentMapping			= m_Weapons[weaponName]->DropPercentMapping;
		tmpAttr.PercentMaterial			= m_Weapons[weaponName]->DropPercentMaterial;
		tmpAttr.Gravity					= m_Weapons[weaponName]->DropGravity;
		tmpAttr.Scale					= m_Weapons[weaponName]->DropScale;
		tmpAttr.AttributeName			= m_Weapons[weaponName]->Name;
		tmpAttr.AttributeAmount			= 1;
		tmpAttr.HideFromRadar			= m_Weapons[weaponName]->DropHideFromRadar;

		CCD->Level()->Attributes()->AddAttributeEntity(&tmpAttr);
	}
}

/* ------------------------------------------------------------------------------------ */
// Attack
/* ------------------------------------------------------------------------------------ */
void CWeapon::Attack(bool alternate)
{
	if(m_CurrentWeapon.empty())
		return;

	m_AltAttack = alternate;

	if(m_ViewPoint == FIRSTPERSON)
	{
		if(m_Weapons[m_CurrentWeapon]->VAttack.empty())
			return;

		// shooting or changing weapon
		if(m_VSequence == VWEPCHANGE ||
			m_VSequence == VWEPHOLSTER ||
			m_VSequence == VWEPATTACKEMPTY ||
			m_VSequence == VWEPRELOAD ||
			m_VSequence == VWEPKEYRELOAD)
		{
			return;
		}

		if(m_Weapons[m_CurrentWeapon]->Catagory == MELEE)
		{
			if(m_VSequence == VWEPATTACK || m_VSequence == VWEPALTATTACK ||
				m_VSequence == VWEPHIT || m_VSequence == VWEPALTHIT ||
				m_VSequence == VWEPUSE)
			{
				return;
			}
		}

		if(m_AttackTime != 0)
		{
			int dtime = CCD->FreeRunningCounter() - m_AttackTime;

			if(dtime < (m_Weapons[m_CurrentWeapon]->FireRate * 1000.0f))
				return; // too soon
		}

		geActor *actor = CCD->Player()->GetActor();
		CPersistentAttributes *inventory = CCD->ActorManager()->Inventory(actor);

		if(m_Weapons[m_CurrentWeapon]->ShotperMag > 0 &&
			m_Weapons[m_CurrentWeapon]->ShotFired >= m_Weapons[m_CurrentWeapon]->ShotperMag)
		{
			if(inventory->Value(m_Weapons[m_CurrentWeapon]->Ammunition) < m_Weapons[m_CurrentWeapon]->AmmoPerShot)
				return;

			m_VSequence = VWEPRELOAD;
			m_VMCounter = 0.0f;
			return;
		}

		m_AttackTime = CCD->FreeRunningCounter();
		m_VSequence = VWEPATTACK; // shooting animation

		if(m_Weapons[m_CurrentWeapon]->Catagory == MELEE)
		{
			if(rand() & 1)
				m_VSequence = VWEPALTATTACK;
		}
		else if(m_Weapons[m_CurrentWeapon]->Catagory == PROJECTILE)
		{
			if(inventory->Value(m_Weapons[m_CurrentWeapon]->Ammunition) < m_Weapons[m_CurrentWeapon]->AmmoPerShot)
			{
				if(!m_Weapons[m_CurrentWeapon]->VAttackEmpty.empty())
					m_VSequence = VWEPATTACKEMPTY;
			}
		}

		m_VMCounter = 0.0f;
		m_AttackFlag = true;
	}
	else // 3rd person
	{
		if(!m_Weapons[m_CurrentWeapon]->PActor)
			return;

		if(m_AttackTime != 0)
		{
			if(m_Weapons[m_CurrentWeapon]->ShotperMag > 0 &&
				m_Weapons[m_CurrentWeapon]->ShotFired >= m_Weapons[m_CurrentWeapon]->ShotperMag)
			{
				m_Weapons[m_CurrentWeapon]->ShotFired = 0;
				geActor *actor = CCD->Player()->GetActor();
				CPersistentAttributes *inventory = CCD->ActorManager()->Inventory(actor);

				if(inventory->Value(m_Weapons[m_CurrentWeapon]->Ammunition) >= m_Weapons[m_CurrentWeapon]->ShotperMag)
					m_Weapons[m_CurrentWeapon]->MagAmt = m_Weapons[m_CurrentWeapon]->ShotperMag;
				else
					m_Weapons[m_CurrentWeapon]->MagAmt = inventory->Value(m_Weapons[m_CurrentWeapon]->Ammunition);
			}

			int dtime = CCD->FreeRunningCounter() - m_AttackTime;

			if(dtime < (m_Weapons[m_CurrentWeapon]->FireRate*1000.0f))
				return; // too soon
		}

		m_AttackTime = CCD->FreeRunningCounter();
		m_AttackFlag = true;
	}
}

/* ------------------------------------------------------------------------------------ */
// Sound
/* ------------------------------------------------------------------------------------ */
void CWeapon::Sound(bool attack, const geVec3d &origin, bool empty)
{
	Snd sound;

	memset(&sound, 0, sizeof(sound));
	geVec3d_Copy(&origin, &sound.Pos);
	sound.Min = CCD->Level()->GetAudibleRadius();
	sound.Loop = GE_FALSE;

	if(attack)
	{
		if(!empty)
		{
			if(m_Weapons[m_CurrentWeapon]->AttackSound.empty())
				return;

			sound.SoundDef = SPool_Sound(m_Weapons[m_CurrentWeapon]->AttackSound.c_str());
		}
		else
		{
			if(m_Weapons[m_CurrentWeapon]->EmptySound.empty())
				return;

			sound.SoundDef = SPool_Sound(m_Weapons[m_CurrentWeapon]->EmptySound.c_str());
		}
	}
	else
	{
		if(m_Weapons[m_CurrentWeapon]->HitSound.empty())
			return;

		sound.SoundDef = SPool_Sound(m_Weapons[m_CurrentWeapon]->HitSound.c_str());
	}

	if(!sound.SoundDef)
	{
		CCD->Log()->Warning("File %s - Line %d: Failed to open audio file for Weapon %s",
							__FILE__, __LINE__, m_CurrentWeapon.c_str());
		return;
	}

	CCD->EffectManager()->Item_Add(EFF_SND, static_cast<void*>(&sound));
}


/* ------------------------------------------------------------------------------------ */
// MeleeAttack
/* ------------------------------------------------------------------------------------ */
void CWeapon::MeleeAttack()
{
	geExtBox extBox;
	geVec3d forward, pos;
	GE_Collision collision;
	geXForm3d xForm;
	geVec3d rotation, position;

	CCD->ActorManager()->GetPosition(CCD->Player()->GetActor(), &position);
	CCD->ActorManager()->GetBoundingBox(CCD->Player()->GetActor(), &extBox);
	position.Y += extBox.Max.Y * 0.5f;
	CCD->ActorManager()->GetRotate(CCD->Player()->GetActor(), &rotation);

	float temp = (extBox.Max.Y - extBox.Min.Y) * 0.25f;
	extBox.Min.Y = -temp;
	extBox.Max.Y = temp;
	float tiltp = (CCD->CameraManager()->GetTiltPercent() * 2.0f) - 1.0f;
	position.Y += (temp * tiltp);

	geXForm3d_SetZRotation(&xForm, rotation.Z);
	geXForm3d_RotateX(&xForm, rotation.X);
	geXForm3d_RotateY(&xForm, rotation.Y);
	geXForm3d_Translate(&xForm, position.X, position.Y, position.Z);

	geXForm3d_GetIn(&xForm, &forward);
	float distance = 1.25f * ((extBox.Max.X < extBox.Max.Z) ? extBox.Max.X : extBox.Max.Z);
	geVec3d_AddScaled(&xForm.Translation, &forward, distance, &pos);

	collision.Actor = NULL;
	collision.Model = NULL;

	bool result = false;
	geWorld_Model *pModel;
	geActor *pActor;
	extBox.Min.X += pos.X;
	extBox.Min.Y += pos.Y;
	extBox.Min.Z += pos.Z;
	extBox.Max.X += pos.X;
	extBox.Max.Y += pos.Y;
	extBox.Max.Z += pos.Z;

	if(CCD->ActorManager()->DoesBoxHitActor(pos, extBox, &pActor, CCD->Player()->GetActor()) == GE_TRUE)
	{
		collision.Model = NULL;
		collision.Actor = pActor;		// Actor we hit
		result = true;
	}
	else if(CCD->ModelManager()->DoesBoxHitModel(pos, extBox, &pModel) == GE_TRUE)
	{
		collision.Actor = NULL;
		collision.Model = pModel;		// Model we hit
		result = true;
	}

	if(result)
	{
		bool stamina = true;

		if(!m_Weapons[m_CurrentWeapon]->Ammunition.empty())
		{
			CPersistentAttributes *inventory = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());

			if(inventory->Value(m_Weapons[m_CurrentWeapon]->Ammunition) < m_Weapons[m_CurrentWeapon]->AmmoPerShot)
				stamina = false;
			else
				inventory->Modify(m_Weapons[m_CurrentWeapon]->Ammunition, -m_Weapons[m_CurrentWeapon]->AmmoPerShot);
		}

		if(stamina)
		{
			int nHitType = CCD->Collision()->ProcessCollision(collision, NULL, false);

			if(nHitType != kNoCollision)
			{
				if(m_VSequence == VWEPATTACK)
					m_VSequence = VWEPHIT;
				else
					m_VSequence = VWEPALTHIT;

				Sound(false, position, false);

				// Process damage here
				if(collision.Actor != NULL)
				{
					CCD->Level()->Damage()->DamageActor(collision.Actor, m_Weapons[m_CurrentWeapon]->MeleeDamage, m_Weapons[m_CurrentWeapon]->Attribute, m_Weapons[m_CurrentWeapon]->MeleeAltDamage, m_Weapons[m_CurrentWeapon]->AltAttribute, m_Weapons[m_CurrentWeapon]->Name);
					CCD->Level()->ExplosionManager()->AddExplosion(m_Weapons[m_CurrentWeapon]->MeleeExplosion, pos);
				}

				if(collision.Model)
					CCD->Level()->Damage()->DamageModel(collision.Model, m_Weapons[m_CurrentWeapon]->MeleeDamage, m_Weapons[m_CurrentWeapon]->Attribute, m_Weapons[m_CurrentWeapon]->MeleeAltDamage, m_Weapons[m_CurrentWeapon]->AltAttribute);
			}
			else
			{
				Sound(true, position, false);
			}
		}
		else
		{
			Sound(true, position, false);
		}
	}
	else
	{
		Sound(true, position, false);
	}
}

/* ------------------------------------------------------------------------------------ */
// ProjectileAttack
/* ------------------------------------------------------------------------------------ */
void CWeapon::ProjectileAttack()
{
	geVec3d rotation;
	geVec3d position;
	geXForm3d xForm;
	geVec3d front, back;
	geVec3d direction, pos, orient;
	geExtBox extBox;
	GE_Collision collision;
	float currentDistance;

	extBox.Min.X = extBox.Min.Y = extBox.Min.Z = -1.0f;
	extBox.Max.X = extBox.Max.Y = extBox.Max.Z = 1.0f;
	geActor *playerActor = CCD->Player()->GetActor();
	currentDistance = 4000.0f;

	if(m_ViewPoint == FIRSTPERSON)
	{
		CCD->CameraManager()->GetRotation(&rotation);
		CCD->CameraManager()->GetPosition(&position);

		geXForm3d_SetZRotation(&xForm, rotation.Z);
		geXForm3d_RotateX(&xForm, rotation.X);
		geXForm3d_RotateY(&xForm, rotation.Y);
	}
	else
	{
		if(m_Weapons[m_CurrentWeapon]->PBone[0] != '\0')
		{
			if(m_Weapons[m_CurrentWeapon]->PActor)
				geActor_GetBoneTransform(m_Weapons[m_CurrentWeapon]->PActor, m_Weapons[m_CurrentWeapon]->PBone, &xForm);
			else
				geActor_GetBoneTransform(playerActor, m_Weapons[m_CurrentWeapon]->PBone, &xForm);
		}
		else
		{
			geActor_GetBoneTransform(playerActor, RootBoneName(playerActor), &xForm);
		}

		position = xForm.Translation;
		CCD->ActorManager()->GetRotate(playerActor, &rotation);

		geXForm3d_SetZRotation(&xForm, rotation.Z);
		geXForm3d_RotateX(&xForm, rotation.X - CCD->CameraManager()->GetTilt());
		geXForm3d_RotateY(&xForm, rotation.Y);
	}

	pos = position;

	geXForm3d_GetIn(&xForm, &direction);
	geVec3d_AddScaled(&pos, &direction, currentDistance, &back);

	geVec3d_Copy(&pos, &front);

	if(geWorld_Collision(CCD->World(), &extBox.Min, &extBox.Max, &front, &back,
		GE_VISIBLE_CONTENTS, GE_COLLIDE_ALL, 0, NULL, NULL, &collision))
	{
		currentDistance = geVec3d_DistanceBetween(&collision.Impact, &front);

		if(currentDistance > 4000.0f)
			currentDistance = 4000.0f;

		geVec3d_AddScaled(&pos, &direction, currentDistance, &back);
	}

	// add launchoffsets to position
	if(m_ViewPoint == FIRSTPERSON)
	{
		if(m_Weapons[m_CurrentWeapon]->VBone[0] != '\0')
		{
			geXForm3d xForm;

			if(geActor_GetBoneTransform(m_Weapons[m_CurrentWeapon]->VActor, m_Weapons[m_CurrentWeapon]->VBone, &xForm) == GE_TRUE)
			{
				geVec3d_Copy(&xForm.Translation, &pos);
			}
			else
			{
				geXForm3d_GetUp(&xForm, &direction);
				geVec3d_AddScaled(&pos, &direction, m_Weapons[m_CurrentWeapon]->VOffset.Y, &pos);

				geXForm3d_GetLeft(&xForm, &direction);
				geVec3d_AddScaled(&pos, &direction, m_Weapons[m_CurrentWeapon]->VOffset.X, &pos);
			}
		}
		else
		{
			geXForm3d_GetUp(&xForm, &direction);
			geVec3d_AddScaled(&pos, &direction, m_Weapons[m_CurrentWeapon]->VOffset.Y, &pos);

			geXForm3d_GetLeft(&xForm, &direction);
			geVec3d_AddScaled(&pos, &direction, m_Weapons[m_CurrentWeapon]->VOffset.X, &pos);
		}
	}
	else
	{
		geXForm3d_GetUp(&xForm, &direction);
		geVec3d_AddScaled(&pos, &direction, m_Weapons[m_CurrentWeapon]->POffset.Y, &pos);

		geXForm3d_GetLeft(&xForm, &direction);
		geVec3d_AddScaled(&pos, &direction, m_Weapons[m_CurrentWeapon]->POffset.X, &pos);
	}

	// direction from actual launch point to target
	geVec3d_Subtract(&back, &pos, &orient);
	float l = geVec3d_Length(&orient);

	// protect from Div by Zero
	if(l > 0.0f)
	{
		float x = orient.X;
		orient.X = GE_PIOVER2 - acos(orient.Y / l);
		orient.Y = atan2(x, orient.Z) + GE_PI;
		orient.Z = 0.0f;	// roll is zero - always!!?

		geXForm3d_SetZRotation(&xForm, orient.Z);
		geXForm3d_RotateX(&xForm, orient.X);
		geXForm3d_RotateY(&xForm, orient.Y);

		// weapon precision - randomize orientation
		if(m_Weapons[m_CurrentWeapon]->MaxDeviationRadius > 0.0f)
		{
			//float maxr = m_Weapons[m_CurrentWeapon]->MaxDeviationRadius;
			//float u = EffectC_Frand(0.f, maxr) + EffectC_Frand(0.f, maxr);//random()+random()
			//float r = (u > maxr) ? 2.f*maxr-u : u; //if u>1 then 2-u else u
			// would also work with sqrt: r = sqrt(EffectC_Frand(0.f, maxr));
			// denser near the origin:    r = EffectC_Frand(0.f, maxr);
			float r = EffectC_Frand(0.f, m_Weapons[m_CurrentWeapon]->MaxDeviationRadius);
			float theta = EffectC_Frand(0.f, GE_2PI);//2*pi*random()
			float left = r*cos(theta);
			float up   = r*sin(theta);

			geVec3d v3up, v3left;
			geXForm3d_GetUp(&xForm, &v3up);
			geXForm3d_GetLeft(&xForm, &v3left);
			geXForm3d_GetIn(&xForm, &orient);

			geVec3d_Scale(&v3up,   up,   &v3up);
			geVec3d_Scale(&v3left, left, &v3left);
			geVec3d_Add(&orient, &v3up,   &orient);
			geVec3d_Add(&orient, &v3left, &orient);

			l = geVec3d_Length(&orient);
			x = orient.X;
			orient.X = GE_PIOVER2 - acos(orient.Y / l);
			orient.Y = atan2(x, orient.Z) + GE_PI;
			orient.Z = 0.0f;	// roll is zero - always!!?

			geXForm3d_SetZRotation(&xForm, orient.Z);
			geXForm3d_RotateX(&xForm, orient.X);
			geXForm3d_RotateY(&xForm, orient.Y);
		}

		geVec3d_Set(&(xForm.Translation), pos.X, pos.Y, pos.Z);

		geXForm3d_GetIn(&xForm, &direction);
		geVec3d_Add(&pos, &direction, &front);
		collision.Actor = NULL;
		CCD->Collision()->IgnoreContents(false);

		while(CCD->Collision()->CheckForCollision(&extBox.Min, &extBox.Max, front, front, &collision))
		{
			if(collision.Actor != playerActor)
				break;

			geVec3d_Add(&front, &direction, &front);
			collision.Actor = NULL;
		}

		if(m_ViewPoint == FIRSTPERSON)
		{
			geVec3d_AddScaled(&front, &direction, m_Weapons[m_CurrentWeapon]->VOffset.Z, &pos);
			geVec3d_AddScaled(&m_Weapons[m_CurrentWeapon]->VMOffset, &direction, m_Weapons[m_CurrentWeapon]->VOffset.Z, &m_Weapons[m_CurrentWeapon]->VMOffset);
		}
		else
		{
			geVec3d_AddScaled(&front, &direction, m_Weapons[m_CurrentWeapon]->POffset.Z, &pos);
		}

		geVec3d_Subtract(&pos, &position, &m_Weapons[m_CurrentWeapon]->VMOffset);

		GE_Contents ZoneContents;

		if(geWorld_GetContents(CCD->World(), &pos, &extBox.Min,	&extBox.Max,
			GE_COLLIDE_MODELS, 0, NULL, NULL, &ZoneContents) == GE_TRUE)
		{
			Liquid *LQ = CCD->Level()->Liquids()->IsLiquid(ZoneContents.Model);

			if(LQ)
			{
				if(!m_Weapons[m_CurrentWeapon]->WorksUnderwater)
				{
					Sound(true, position, true);
					return;
				}
			}
		}

		CPersistentAttributes *inventory = CCD->ActorManager()->Inventory(playerActor);

		if(inventory->Value(m_Weapons[m_CurrentWeapon]->Ammunition) >= m_Weapons[m_CurrentWeapon]->AmmoPerShot)
		{
			inventory->Modify(m_Weapons[m_CurrentWeapon]->Ammunition, -m_Weapons[m_CurrentWeapon]->AmmoPerShot);

			AddProjectile(pos, front, orient, m_Weapons[m_CurrentWeapon]->Projectile, m_Weapons[m_CurrentWeapon]->Attribute, m_Weapons[m_CurrentWeapon]->AltAttribute);

			m_Weapons[m_CurrentWeapon]->ShotFired += m_Weapons[m_CurrentWeapon]->AmmoPerShot;

			if(m_ViewPoint == FIRSTPERSON)
			{
				if(m_Weapons[m_CurrentWeapon]->JerkAmt > 0.0f)
				{
					CCD->CameraManager()->SetJerk(m_Weapons[m_CurrentWeapon]->JerkAmt, m_Weapons[m_CurrentWeapon]->JerkDecay);
				}

				if(m_Weapons[m_CurrentWeapon]->MuzzleFlash[0] != '\0')
				{
					if(m_Weapons[m_CurrentWeapon]->VBone[0] != '\0')
					{
						CCD->Level()->ExplosionManager()->AddExplosion(m_Weapons[m_CurrentWeapon]->MuzzleFlash, m_Weapons[m_CurrentWeapon]->VOffset, m_Weapons[m_CurrentWeapon]->VActor, m_Weapons[m_CurrentWeapon]->VBone);
						m_MFlash = false;
					}
					else
					{
						CCD->Level()->ExplosionManager()->AddExplosion(m_Weapons[m_CurrentWeapon]->MuzzleFlash, pos);
					}
				}
			}
			else
			{
				if(m_Weapons[m_CurrentWeapon]->PBone[0] != '\0')
				{
					geVec3d position, direction;
					geXForm3d_SetIdentity(&xForm);
					geXForm3d_RotateX(&xForm, CCD->CameraManager()->GetTilt());
					position = xForm.Translation;

					geXForm3d_GetIn(&xForm, &direction);
					geVec3d_AddScaled(&position, &direction, m_Weapons[m_CurrentWeapon]->POffset.Z+m_Weapons[m_CurrentWeapon]->PMOffset, &position);

					geXForm3d_GetUp(&xForm, &direction);
					geVec3d_AddScaled(&position, &direction, m_Weapons[m_CurrentWeapon]->POffset.Y, &position);

					geXForm3d_GetLeft(&xForm, &direction);
					geVec3d_AddScaled(&position, &direction, m_Weapons[m_CurrentWeapon]->POffset.X, &position);

					CCD->Level()->ExplosionManager()->AddExplosion(m_Weapons[m_CurrentWeapon]->MuzzleFlash3rd, position, m_Weapons[m_CurrentWeapon]->PActor, m_Weapons[m_CurrentWeapon]->PBone, true);
					m_MFlash = false;
				}
				else
				{
					geVec3d_AddScaled(&front, &direction, m_Weapons[m_CurrentWeapon]->PMOffset, &front);
					CCD->Level()->ExplosionManager()->AddExplosion(m_Weapons[m_CurrentWeapon]->MuzzleFlash3rd, front);
				}
			}

			Sound(true, position, false);
		}
		else
		{
			Sound(true, position, true);
		}
	}
}

/* ------------------------------------------------------------------------------------ */
// AddProjectile
/* ------------------------------------------------------------------------------------ */
void CWeapon::AddProjectile(const geVec3d& pos, const geVec3d& front, const geVec3d& orient,
							const std::string& projName, const std::string& PAttribute, const std::string& PAltAttribute)
{
	if(m_Projectiles.find(projName) == m_Projectiles.end())
		return;

	Projectile *proj;

	{
		geXForm3d xForm;
		proj = new Projectile;

		proj->Rotation	= m_Projectiles[projName]->Rotation;
		proj->Angles	= orient;
		proj->Pos		= pos;
		proj->Gravity	= m_Projectiles[projName]->Gravity;
		proj->LifeTime	= m_Projectiles[projName]->LifeTime;
		proj->Bounce	= m_Projectiles[projName]->Bounce;

		proj->BounceSoundDef = NULL;
		if(!m_Projectiles[projName]->BounceSound.empty())
			proj->BounceSoundDef = SPool_Sound(m_Projectiles[projName]->BounceSound.c_str());

		proj->MoveSoundDef = NULL;
		if(!m_Projectiles[projName]->MoveSound.empty())
			proj->MoveSoundDef = SPool_Sound(m_Projectiles[projName]->MoveSound.c_str());

		proj->ImpactSoundDef = NULL;
		if(!m_Projectiles[projName]->ImpactSound.empty())
			proj->ImpactSoundDef = SPool_Sound(m_Projectiles[projName]->ImpactSound.c_str());

		proj->MoveSoundEffect	= PlaySound(proj->MoveSoundDef, proj->Pos, true);
		proj->Decal				= m_Projectiles[projName]->Decal;
		proj->Explosion			= m_Projectiles[projName]->Explosion;
		proj->ActorExplosion	= m_Projectiles[projName]->ActorExplosion;
		proj->ShowBoth			= m_Projectiles[projName]->ShowBoth;
		proj->AttachActor		= m_Projectiles[projName]->AttachActor;
		proj->BoneLevel			= m_Projectiles[projName]->BoneLevel;
		proj->ShakeAmt			= m_Projectiles[projName]->ShakeAmt;
		proj->ShakeDecay		= m_Projectiles[projName]->ShakeDecay;
		proj->Damage			= m_Projectiles[projName]->Damage;
		proj->AltDamage			= m_Projectiles[projName]->AltDamage;
		proj->AltAttribute		= PAltAttribute;
		proj->RadiusDamage		= m_Projectiles[projName]->RadiusDamage;
		proj->Radius			= m_Projectiles[projName]->Radius;
		proj->Attribute			= PAttribute;
		proj->Name				= projName;

		geXForm3d_SetZRotation(&xForm, orient.Z);
		geXForm3d_RotateX(&xForm, orient.X);
		geXForm3d_RotateY(&xForm, orient.Y);

		geXForm3d_GetIn(&xForm, &proj->Direction);
		proj->In = proj->Direction;
		geVec3d_Scale(&proj->Direction, m_Projectiles[projName]->Speed, &proj->Direction);

		geVec3d_Inverse(&proj->In);

		proj->Actor = CCD->ActorManager()->SpawnActor(
			m_Projectiles[projName]->ActorFile.c_str(),
			proj->Pos, proj->Rotation,
			m_Projectiles[projName]->ActorAnimation.c_str(),
			m_Projectiles[projName]->ActorAnimation.c_str(),
			NULL);

		CCD->ActorManager()->Rotate(proj->Actor, proj->Angles);
		CCD->ActorManager()->SetType(proj->Actor, ENTITY_PROJECTILE);
		CCD->ActorManager()->SetScale(proj->Actor, m_Projectiles[projName]->Scale);
		CCD->ActorManager()->SetBoxChange(proj->Actor, false);
		CCD->ActorManager()->SetNoCollide(proj->Actor);
		CCD->ActorManager()->SetBBox(proj->Actor,
			m_Projectiles[projName]->BoxSize,
			m_Projectiles[projName]->BoxSize,
			m_Projectiles[projName]->BoxSize);
		CCD->ActorManager()->GetBoundingBox(proj->Actor, &proj->ExtBox);
		CCD->ActorManager()->SetStepHeight(proj->Actor, -1.0f);
		CCD->ActorManager()->SetHideRadar(proj->Actor, true);

		if(proj->Gravity)
			CCD->ActorManager()->SetGravity(proj->Actor, CCD->Level()->GetGravity());

		GE_Collision collision;
		CCD->Collision()->IgnoreContents(false);
		CCD->Collision()->CheckLevel(RGF_COLLISIONLEVEL_1);
		char BoneHit[256] = { 0 };

		if(CCD->Collision()->CheckForBoneCollision(&proj->ExtBox.Min, &proj->ExtBox.Max,
				front, proj->Pos, &collision, proj->Actor, BoneHit, proj->BoneLevel))
		{
			int nHitType = CCD->Collision()->ProcessCollision(collision, proj->Actor, false);

			if(nHitType != kNoCollision)
			{
				if(proj->MoveSoundEffect != -1)
					CCD->EffectManager()->Item_Delete(EFF_SND, proj->MoveSoundEffect);

				PlaySound(proj->ImpactSoundDef, proj->Pos, false);

				// inflict damage
				if(collision.Actor)
					CCD->Level()->Damage()->DamageActor(collision.Actor, proj->Damage, proj->Attribute.c_str(), proj->AltDamage, proj->AltAttribute.c_str(), proj->Name.c_str());

				if(collision.Model)
					CCD->Level()->Damage()->DamageModel(collision.Model, proj->Damage, proj->Attribute.c_str(), proj->AltDamage, proj->AltAttribute.c_str());

				// create explosion here
				CCD->Level()->ExplosionManager()->AddExplosion(proj->Explosion, proj->Pos);

				if(proj->ShakeAmt>0.0f)
					CCD->CameraManager()->SetShake(proj->ShakeAmt, proj->ShakeDecay, proj->Pos);

				if(collision.Actor)
					CCD->Level()->ExplosionManager()->AddExplosion(proj->ActorExplosion, proj->Pos);

				// Handle explosion damage here
				if(proj->RadiusDamage > 0.0f)
				{
					CCD->Level()->Damage()->DamageActorInRange(proj->Pos, proj->Radius, proj->RadiusDamage, proj->Attribute, proj->RadiusDamage, proj->AltAttribute, "Explosion");
					CCD->Level()->Damage()->DamageModelInRange(proj->Pos, proj->Radius, proj->RadiusDamage, proj->Attribute, proj->RadiusDamage, proj->AltAttribute);
				}

				CCD->ActorManager()->RemoveActorCheck(proj->Actor);
				geActor_Destroy(&proj->Actor);
				delete proj;
				return;
			}
		}

		for(int i=0; i<5; ++i)
		{
			proj->Effect[i] = -1;
			geXForm3d xForm;

			if(m_Projectiles[projName]->EffectBone[i].empty())
			{
				geActor_GetBoneTransform(proj->Actor, RootBoneName(proj->Actor), &xForm);
			}
			else
			{
				if(!geActor_GetBoneTransform(proj->Actor, m_Projectiles[projName]->EffectBone[i].c_str(), &xForm))
					geActor_GetBoneTransform(proj->Actor, RootBoneName(proj->Actor), &xForm);
			}

			if(!m_Projectiles[projName]->EffectName[i].empty())
			{
				if(CCD->Effect()->EffectExists(m_Projectiles[projName]->EffectName[i]))
				{
					geVec3d zero = {0.0f, 0.0f, 0.0f};
					proj->Effect[i] = CCD->Effect()->AddEffect(m_Projectiles[projName]->EffectName[i], xForm.Translation, zero);
					proj->EffectType[i] = CCD->Effect()->EffectType(m_Projectiles[projName]->EffectName[i]);
					proj->EffectBone[i] = m_Projectiles[projName]->EffectBone[i];
					break;
				}
			}
		}

		proj->next = NULL;
		proj->prev = m_Bottom;

		if(m_Bottom != NULL)
			m_Bottom->next = proj;

		m_Bottom = proj;
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
	Sound.Min = CCD->Level()->GetAudibleRadius();
	Sound.Loop = Loop;
	Sound.SoundDef = SoundDef;

	return CCD->EffectManager()->Item_Add(EFF_SND, static_cast<void*>(&Sound));
}

/* ------------------------------------------------------------------------------------ */
// ZoomAmount
/* ------------------------------------------------------------------------------------ */
int CWeapon::ZoomAmount()
{
	if(!m_CurrentWeapon.empty() && m_ViewPoint == FIRSTPERSON)
	{
		if(m_Weapons[m_CurrentWeapon]->ZoomAmt > 1)
		{
			if(m_Weapons[m_CurrentWeapon]->ZoomAmt > 20)
				return 20;

			return m_Weapons[m_CurrentWeapon]->ZoomAmt;
		}
	}

	return 0;
}


/* ------------------------------------------------------------------------------------ */
// GetVActor
/* ------------------------------------------------------------------------------------ */
geActor* CWeapon::GetVActor()
{
	if(m_CurrentWeapon.empty())
		return NULL;

	return m_Weapons[m_CurrentWeapon]->VActor;
}

/* ------------------------------------------------------------------------------------ */
// GetPActor
/* ------------------------------------------------------------------------------------ */
geActor* CWeapon::GetPActor()
{
	if(m_CurrentWeapon.empty())
		return NULL;

	return m_Weapons[m_CurrentWeapon]->PActor;
}

/* ------------------------------------------------------------------------------------ */
// LoadDefaults
/* ------------------------------------------------------------------------------------ */
void CWeapon::LoadDefaults()
{
	std::string weaponini("weapon.ini");

	if(CCD->MenuManager()->GetUseSelect())
		weaponini = CCD->MenuManager()->GetSelectedCharacter()->GetWeaponConfigFilename();

	CIniFile iniFile(weaponini);

	if(!iniFile.ReadFile())
	{
		CCD->Log()->Error("Failed to open " + weaponini + " file");
		return;
	}

	std::string KeyName = iniFile.FindFirstKey();
	std::string type, vector, value;
	geVFile *ActorFile;

	while(!KeyName.empty())
	{
		type = iniFile.GetValue(KeyName, "type");

		// -------------------------------------------------------------------------------
		//								PROJECTILES
		// -------------------------------------------------------------------------------
		if(type == "projectile")
		{
			if(m_Projectiles.find(KeyName) != m_Projectiles.end())
			{
				CCD->Log()->Warning("Redefinition of proj [" + KeyName + "] in " + weaponini + " file.");
				KeyName = iniFile.FindNextKey();
				continue;
			}

			m_Projectiles[KeyName] = new ProjectileDefinition;

			m_Projectiles[KeyName]->ActorFile = iniFile.GetValue(KeyName, "actor");
			vector = iniFile.GetValue(KeyName, "rotation");

			if(!vector.empty())
			{
				m_Projectiles[KeyName]->Rotation = ToVec3d(vector);
				geVec3d_Scale(&m_Projectiles[KeyName]->Rotation, GE_PIOVER180, &m_Projectiles[KeyName]->Rotation);
			}

			geActor* actor = CCD->ActorManager()->SpawnActor(m_Projectiles[KeyName]->ActorFile.c_str(),
							m_Projectiles[KeyName]->Rotation, m_Projectiles[KeyName]->Rotation, "", "", NULL);
			CCD->ActorManager()->RemoveActor(actor);
			geActor_Destroy(&actor);
			actor = NULL;

			m_Projectiles[KeyName]->Scale = static_cast<float>(iniFile.GetValueF(KeyName, "scale"));

			m_Projectiles[KeyName]->ActorAnimation = iniFile.GetValue(KeyName, "animation");

			m_Projectiles[KeyName]->Bounce = false;
			value = iniFile.GetValue(KeyName, "bounce");
			if(value == "true")
				m_Projectiles[KeyName]->Bounce = true;

			m_Projectiles[KeyName]->Gravity = false;
			value = iniFile.GetValue(KeyName, "gravity");
			if(value == "true")
				m_Projectiles[KeyName]->Gravity = true;

			m_Projectiles[KeyName]->Speed		= static_cast<float>(iniFile.GetValueF(KeyName, "speed"));
			m_Projectiles[KeyName]->LifeTime	= static_cast<float>(iniFile.GetValueF(KeyName, "lifetime"));

			m_Projectiles[KeyName]->Explosion		= iniFile.GetValue(KeyName, "explosion");
			m_Projectiles[KeyName]->ActorExplosion	= iniFile.GetValue(KeyName, "actorexplosion");

			m_Projectiles[KeyName]->ShowBoth = false;
			value = iniFile.GetValue(KeyName, "showboth");
			if(value == "true")
				m_Projectiles[KeyName]->ShowBoth = true;

			m_Projectiles[KeyName]->AttachActor = false;
			value = iniFile.GetValue(KeyName, "attachactor");
			if(value == "true")
				m_Projectiles[KeyName]->AttachActor = true;

			m_Projectiles[KeyName]->BoneLevel = true;
			value = iniFile.GetValue(KeyName, "bonelevel");
			if(value == "false")
				m_Projectiles[KeyName]->BoneLevel = false;

			m_Projectiles[KeyName]->ShakeAmt		= static_cast<float>(iniFile.GetValueF(KeyName, "shakeamount"));
			m_Projectiles[KeyName]->ShakeDecay		= static_cast<float>(iniFile.GetValueF(KeyName, "shakedecay"));
			m_Projectiles[KeyName]->BoxSize			= static_cast<float>(iniFile.GetValueF(KeyName, "boundingbox"));
			m_Projectiles[KeyName]->Damage			= static_cast<float>(iniFile.GetValueF(KeyName, "damage"));
			m_Projectiles[KeyName]->AltDamage		= static_cast<float>(iniFile.GetValueF(KeyName, "altdamage"));
			m_Projectiles[KeyName]->Radius			= static_cast<float>(iniFile.GetValueF(KeyName, "explosionradius"));
			m_Projectiles[KeyName]->RadiusDamage	= static_cast<float>(iniFile.GetValueF(KeyName, "explosiondamage"));
			m_Projectiles[KeyName]->Decal			= iniFile.GetValueI(KeyName, "decal");

			m_Projectiles[KeyName]->MoveSound = iniFile.GetValue(KeyName, "movesound");
			if(!m_Projectiles[KeyName]->MoveSound.empty())
				SPool_Sound(m_Projectiles[KeyName]->MoveSound.c_str());

			m_Projectiles[KeyName]->ImpactSound = iniFile.GetValue(KeyName, "impactsound");
			if(!m_Projectiles[KeyName]->ImpactSound.empty())
				SPool_Sound(m_Projectiles[KeyName]->ImpactSound.c_str());

			m_Projectiles[KeyName]->BounceSound = iniFile.GetValue(KeyName, "bouncesound");
			if(!m_Projectiles[KeyName]->BounceSound.empty())
				SPool_Sound(m_Projectiles[KeyName]->BounceSound.c_str());

			// effect 0
			m_Projectiles[KeyName]->EffectName[0] = iniFile.GetValue(KeyName, "effect0");
			m_Projectiles[KeyName]->EffectBone[0] = iniFile.GetValue(KeyName, "effectbone0");

			// effect 1
			m_Projectiles[KeyName]->EffectName[1] = iniFile.GetValue(KeyName, "effect1");
			m_Projectiles[KeyName]->EffectBone[1] = iniFile.GetValue(KeyName, "effectbone1");

			// effect 2
			m_Projectiles[KeyName]->EffectName[2] = iniFile.GetValue(KeyName, "effect2");
			m_Projectiles[KeyName]->EffectBone[2] = iniFile.GetValue(KeyName, "effectbone2");

			// effect 3
			m_Projectiles[KeyName]->EffectName[3] = iniFile.GetValue(KeyName, "effect3");
			m_Projectiles[KeyName]->EffectBone[3] = iniFile.GetValue(KeyName, "effectbone3");

			// effect 4
			m_Projectiles[KeyName]->EffectName[4] = iniFile.GetValue(KeyName, "effect4");
			m_Projectiles[KeyName]->EffectBone[4] = iniFile.GetValue(KeyName, "effectbone4");
		}
		// -------------------------------------------------------------------------------
		//								WEAPONS
		// -------------------------------------------------------------------------------
		else if(type == "weapon")
		{
			if(m_Weapons.find(KeyName) != m_Weapons.end())
			{
				CCD->Log()->Warning("Redefinition of weapon [" + KeyName + "] in " + weaponini + " file.");
				KeyName = iniFile.FindNextKey();
				continue;
			}

			m_Weapons[KeyName] = new WeaponDefinition;

			m_Weapons[KeyName]->VActor = NULL;
			m_Weapons[KeyName]->VActorDef = NULL;
			m_Weapons[KeyName]->PActor = NULL;
			m_Weapons[KeyName]->PActorDef = NULL;

			m_Weapons[KeyName]->F = m_Weapons[KeyName]->H = m_Weapons[KeyName]->J = 0.0f;
			m_Weapons[KeyName]->G = m_Weapons[KeyName]->K = m_Weapons[KeyName]->L = m_Weapons[KeyName]->Z = 0.0f;

			for(int k=0; k<ANIMMAX; ++k)
				m_Weapons[KeyName]->Animations[k].clear();

			strcpy(m_Weapons[KeyName]->Name, KeyName.c_str());

			value = iniFile.GetValue(KeyName, "slot");
			if(value == "default")
				m_Weapons[KeyName]->Slot = MAX_WEAPONS-1;
			else
				m_Weapons[KeyName]->Slot = iniFile.GetValueI(KeyName, "slot");

			m_Weapons[KeyName]->Catagory = MELEE;
			type = iniFile.GetValue(KeyName, "catagory");
			if(type == "projectile")
				m_Weapons[KeyName]->Catagory = PROJECTILE;
			else if(type == "beam")
				m_Weapons[KeyName]->Catagory = BEAM;

			m_Weapons[KeyName]->MaxDeviationRadius = 0.0f;
			{
				float maxDeviationAngle = static_cast<float>(iniFile.GetValueF(KeyName, "maxdeviationangle"));
				if(maxDeviationAngle > 0.0f)
				{
					if(maxDeviationAngle > 89.99f)
						maxDeviationAngle = 89.99f;

					m_Weapons[KeyName]->MaxDeviationRadius = tan(maxDeviationAngle * GE_PIOVER180);
				}
			}

			// ---------------------------------------------------------------------------
			//							Weapon Catagory
			// ---------------------------------------------------------------------------
			switch(m_Weapons[KeyName]->Catagory)
			{
			case MELEE:
				m_Weapons[KeyName]->MeleeDamage		= static_cast<float>(iniFile.GetValueF(KeyName, "damage"));
				m_Weapons[KeyName]->MeleeAltDamage	= static_cast<float>(iniFile.GetValueF(KeyName, "altdamage"));

				m_Weapons[KeyName]->HitSound = iniFile.GetValue(KeyName, "hitsound");
				if(!m_Weapons[KeyName]->HitSound.empty())
					SPool_Sound(m_Weapons[KeyName]->HitSound.c_str());

				m_Weapons[KeyName]->MeleeExplosion = iniFile.GetValue(KeyName, "meleexplosion");

				m_Weapons[KeyName]->MeleeAmmo = false;
				value = iniFile.GetValue(KeyName, "meleeammoactivate");
				if(value == "true")
					m_Weapons[KeyName]->MeleeAmmo = true;

				break;
			case PROJECTILE:
				m_Weapons[KeyName]->Projectile = iniFile.GetValue(KeyName, "projectile");

				m_Weapons[KeyName]->MuzzleFlash = iniFile.GetValue(KeyName, "muzzleflash");

				m_Weapons[KeyName]->MuzzleFlash3rd = iniFile.GetValue(KeyName, "muzzleflash3rd");
				if(m_Weapons[KeyName]->MuzzleFlash3rd.empty())
					m_Weapons[KeyName]->MuzzleFlash3rd = m_Weapons[KeyName]->MuzzleFlash;

				m_Weapons[KeyName]->WorksUnderwater = true;
				value = iniFile.GetValue(KeyName, "worksunderwater");
				if(value == "false")
					m_Weapons[KeyName]->WorksUnderwater = false;

				m_Weapons[KeyName]->MeleeAmmo = false;

				break;
			case BEAM:
				break;
			}

			// ---------------------------------------------------------------------------
			//							Weapon ViewPoint
			// ---------------------------------------------------------------------------
			m_Weapons[KeyName]->FixedView = -1;
			value = iniFile.GetValue(KeyName, "forceviewto");
			if(value == "firstperson")
				m_Weapons[KeyName]->FixedView = FIRSTPERSON;
			else if(value == "thirdperson")
				m_Weapons[KeyName]->FixedView = THIRDPERSON;
			else if(value == "isometric")
				m_Weapons[KeyName]->FixedView = ISOMETRIC;
			else if(value == "fixed")
				m_Weapons[KeyName]->FixedView = FIXEDCAMERA;

			// ---------------------------------------------------------------------------
			//							Weapon Sounds
			// ---------------------------------------------------------------------------

			m_Weapons[KeyName]->AttackSound = iniFile.GetValue(KeyName, "attacksound");
			if(!m_Weapons[KeyName]->AttackSound.empty())
				SPool_Sound(m_Weapons[KeyName]->AttackSound.c_str());

			m_Weapons[KeyName]->EmptySound = iniFile.GetValue(KeyName, "emptysound");
			if(!m_Weapons[KeyName]->EmptySound.empty())
				SPool_Sound(m_Weapons[KeyName]->EmptySound.c_str());

			m_Weapons[KeyName]->UseSound = iniFile.GetValue(KeyName, "usesound");
			if(!m_Weapons[KeyName]->UseSound.empty())
				SPool_Sound(m_Weapons[KeyName]->UseSound.c_str());

			m_Weapons[KeyName]->ReloadSound = iniFile.GetValue(KeyName, "reloadsound");
			if(!m_Weapons[KeyName]->ReloadSound.empty())
				SPool_Sound(m_Weapons[KeyName]->ReloadSound.c_str());

			m_Weapons[KeyName]->Attribute		= iniFile.GetValue(KeyName, "attribute");
			m_Weapons[KeyName]->AltAttribute	= iniFile.GetValue(KeyName, "altattribute");
			m_Weapons[KeyName]->Ammunition		= iniFile.GetValue(KeyName, "ammunition");
			m_Weapons[KeyName]->AmmoPerShot		= iniFile.GetValueI(KeyName, "ammopershot");
			m_Weapons[KeyName]->FireRate		= static_cast<float>(iniFile.GetValueF(KeyName, "firerate"));

			bool activeflag = false;

			// ---------------------------------------------------------------------------
			//							Drop Weapon Attribute
			// ---------------------------------------------------------------------------
			m_Weapons[KeyName]->DropActor[0] = 0;
			value = iniFile.GetValue(KeyName, "dropactor");
			if(!value.empty())
			{
				strcpy(m_Weapons[KeyName]->DropActor, value.c_str());
				geActor *actor = CCD->ActorManager()->SpawnActor(m_Weapons[KeyName]->DropActor,
					m_Weapons[KeyName]->DropActorOffset, m_Weapons[KeyName]->DropActorRotation, "", "", NULL);

				if(!actor)
				{
					CCD->Log()->Warning("File %s - Line %d: %s : Missing Actor '%s'",
										__FILE__, __LINE__, KeyName.c_str(), m_Weapons[KeyName]->DropActor);
				}
				else
				{
					CCD->ActorManager()->RemoveActor(actor);
					geActor_Destroy(&actor);
					actor = NULL;
				}

				vector = iniFile.GetValue(KeyName, "dropoffset");
				if(!vector.empty())
				{
					m_Weapons[KeyName]->DropActorOffset = ToVec3d(vector);
				}

				m_Weapons[KeyName]->DropScale = 1.0f;
				if(iniFile.GetValueF(KeyName, "dropscale"))
					m_Weapons[KeyName]->DropScale = static_cast<float>(iniFile.GetValueF(KeyName, "dropscale"));

				geVec3d_Set(&m_Weapons[KeyName]->DropFillColor, 255.0f, 255.0f, 255.0f);
				geVec3d_Set(&m_Weapons[KeyName]->DropAmbientColor, 255.0f, 255.0f, 255.0f);

				vector = iniFile.GetValue(KeyName, "dropfillcolor");
				if(!vector.empty())
				{
					m_Weapons[KeyName]->DropFillColor = ToVec3d(vector);
				}

				vector = iniFile.GetValue(KeyName, "dropambientcolor");
				if(!vector.empty())
				{
					m_Weapons[KeyName]->DropAmbientColor = ToVec3d(vector);
				}

				m_Weapons[KeyName]->DropAmbientLightFromFloor = GE_TRUE;
				value = iniFile.GetValue(KeyName, "dropambientlightfromfloor");
				if(value == "false")
					m_Weapons[KeyName]->DropAmbientLightFromFloor = GE_FALSE;

				value = iniFile.GetValue(KeyName, "dropenvironmentmapping");
				if(value == "true")
				{
					m_Weapons[KeyName]->DropEnvironmentMapping = GE_TRUE;

					value = iniFile.GetValue(KeyName, "dropallmaterial");
					if(value == "true")
						m_Weapons[KeyName]->DropAllMaterial = GE_TRUE;

					m_Weapons[KeyName]->DropPercentMapping = static_cast<float>(iniFile.GetValueF(KeyName, "droppercentmapping"));
					m_Weapons[KeyName]->DropPercentMaterial = static_cast<float>(iniFile.GetValueF(KeyName, "droppercentmaterial"));
				}

				vector = iniFile.GetValue(KeyName, "droprotation");
				if(!vector.empty())
				{
					m_Weapons[KeyName]->DropActorRotation = ToVec3d(vector);
				}

				value = iniFile.GetValue(KeyName, "dropgravity");
				if(value == "true")
					m_Weapons[KeyName]->DropGravity = GE_TRUE;

				value = iniFile.GetValue(KeyName, "drophidefromradar");
				if(value == "true")
					m_Weapons[KeyName]->DropHideFromRadar = GE_TRUE;
			}

			// ---------------------------------------------------------------------------
			//							1st Person Weapon
			// ---------------------------------------------------------------------------
			value = iniFile.GetValue(KeyName, "viewactor");

			if(!value.empty())
			{
				CFileManager::GetSingletonPtr()->OpenRFFile(&ActorFile, kActorFile, value.c_str(), GE_VFILE_OPEN_READONLY);

				if(ActorFile)
				{
					m_Weapons[KeyName]->VActorDef = geActor_DefCreateFromFile(ActorFile);

					if(m_Weapons[KeyName]->VActorDef)
					{
						m_Weapons[KeyName]->VActor = geActor_Create(m_Weapons[KeyName]->VActorDef);
						geWorld_AddActor(CCD->World(), m_Weapons[KeyName]->VActor, 0, 0xffffffff);

						geVec3d Fill = {0.0f, 1.0f, 0.0f};
						geXForm3d Xf;
						geXForm3d XfT;
						geVec3d FillColor = {255.0f, 255.0f, 255.0f};
						geVec3d AmbientColor = {255.0f, 255.0f, 255.0f};
						geBoolean AmbientLightFromFloor = GE_TRUE;
						geVec3d NewFillNormal;
						geActor_GetBoneTransform(m_Weapons[KeyName]->VActor, RootBoneName(m_Weapons[KeyName]->VActor), &Xf);
						geXForm3d_GetTranspose(&Xf, &XfT);
						geXForm3d_Rotate(&XfT, &Fill, &NewFillNormal);

						vector = iniFile.GetValue(KeyName, "viewfillcolor");
						if(!vector.empty())
						{
							FillColor = ToVec3d(vector);
						}

						vector = iniFile.GetValue(KeyName, "viewambientcolor");
						if(!vector.empty())
						{
							AmbientColor = ToVec3d(vector);
						}

						value = iniFile.GetValue(KeyName, "viewambientlightfromfloor");
						if(value == "false")
							AmbientLightFromFloor = GE_FALSE;

						geActor_SetStaticLightingOptions(m_Weapons[KeyName]->VActor, AmbientLightFromFloor, GE_TRUE, 3);

						geActor_SetLightingOptions(m_Weapons[KeyName]->VActor, GE_TRUE, &NewFillNormal, FillColor.X, FillColor.Y, FillColor.Z,
						AmbientColor.X, AmbientColor.Y, AmbientColor.Z, AmbientLightFromFloor, 6, NULL, GE_FALSE);

						value = iniFile.GetValue(KeyName, "environmentmapping");
						if(value == "true")
						{
							bool all = false;
							value = iniFile.GetValue(KeyName, "allmaterial");
							if(value == "true")
								all = true;

							float percent = static_cast<float>(iniFile.GetValueF(KeyName, "percentmapping"));
							float percentm = static_cast<float>(iniFile.GetValueF(KeyName, "percentmaterial"));
							SetEnvironmentMapping(m_Weapons[KeyName]->VActor, true, all, percent, percentm);
						}

						vector = iniFile.GetValue(KeyName, "viewrotation");
						if(!vector.empty())
						{
							m_Weapons[KeyName]->VActorRotation = ToVec3d(vector);
							// convert to radians
							geVec3d_Scale(&m_Weapons[KeyName]->VActorRotation, GE_PIOVER180, &m_Weapons[KeyName]->VActorRotation);
						}

						vector = iniFile.GetValue(KeyName, "viewoffset");
						if(!vector.empty())
						{
							m_Weapons[KeyName]->VActorOffset = ToVec3d(vector);
						}

						m_Weapons[KeyName]->VScale = static_cast<float>(iniFile.GetValueF(KeyName, "viewscale"));
						float tspeed = static_cast<float>(iniFile.GetValueF(KeyName, "viewanimspeed"));

						m_Weapons[KeyName]->VAnimSpeed = 1.0f;
						if(tspeed > 0.0f)
							m_Weapons[KeyName]->VAnimSpeed = tspeed;

						m_Weapons[KeyName]->VArm	= iniFile.GetValue(KeyName, "viewarmanim");
						m_Weapons[KeyName]->VIdle	= iniFile.GetValue(KeyName, "viewidleanim");
						m_Weapons[KeyName]->VAttack = iniFile.GetValue(KeyName, "viewattackanim");
						m_Weapons[KeyName]->VWalk	= iniFile.GetValue(KeyName, "viewwalkanim");

						geVec3d LitColor = {255.0f, 255.0f, 255.0f};
						m_Weapons[KeyName]->CrossHair = NULL;
						m_Weapons[KeyName]->ZoomOverlay = NULL;

						switch(m_Weapons[KeyName]->Catagory)
						{
						case MELEE:
							m_Weapons[KeyName]->VAltAttack	= iniFile.GetValue(KeyName, "viewaltattackanim");
							m_Weapons[KeyName]->VHit		= iniFile.GetValue(KeyName, "viewhitanim");
							m_Weapons[KeyName]->VAltHit		= iniFile.GetValue(KeyName, "viewalthitanim");
							m_Weapons[KeyName]->VUse		= iniFile.GetValue(KeyName, "viewuseanim");
							break;
						case PROJECTILE:
							vector = iniFile.GetValue(KeyName, "viewlaunchoffset");
							if(!vector.empty())
							{
								m_Weapons[KeyName]->VOffset = ToVec3d(vector);
							}

							m_Weapons[KeyName]->VReload = iniFile.GetValue(KeyName, "viewreloadanim");

							m_Weapons[KeyName]->ShotperMag = iniFile.GetValueI(KeyName, "shotpermagazine");

							m_Weapons[KeyName]->ShotFired = 0;

							m_Weapons[KeyName]->MagAmt = -1;

							m_Weapons[KeyName]->VKeyReload = iniFile.GetValue(KeyName, "viewkeyreloadanim");

							m_Weapons[KeyName]->VAttackEmpty = iniFile.GetValue(KeyName, "viewattackemptyanim");

							m_Weapons[KeyName]->LooseMag = false;
							value = iniFile.GetValue(KeyName, "loosemag");
							if(value == "true")
								m_Weapons[KeyName]->LooseMag = true;

							value = iniFile.GetValue(KeyName, "viewlaunchbone");
							if(!value.empty())
								strcpy(m_Weapons[KeyName]->VBone, value.c_str());

							m_Weapons[KeyName]->CrossHairFixed = false;
							value = iniFile.GetValue(KeyName, "crosshairfixed");
							if(value == "true")
								m_Weapons[KeyName]->CrossHairFixed = true;

							m_Weapons[KeyName]->CrossHair = NULL;
							value = iniFile.GetValue(KeyName, "crosshair");
							if(!value.empty())
							{
								std::string alpha = iniFile.GetValue(KeyName, "crosshairalpha");

								if(alpha.empty())
									alpha = value;

								m_Weapons[KeyName]->CrossHair = CreateFromFileAndAlphaNames(value.c_str(), alpha.c_str());

								if(m_Weapons[KeyName]->CrossHairFixed)
									geEngine_AddBitmap(CCD->Engine()->Engine(), m_Weapons[KeyName]->CrossHair);
								else
									geWorld_AddBitmap(CCD->World(), m_Weapons[KeyName]->CrossHair);
							}

							vector = iniFile.GetValue(KeyName, "crosshairlitcolor");
							if(!vector.empty())
							{
								LitColor = ToVec3d(vector);
							}

							m_Weapons[KeyName]->LitColor = LitColor;

							m_Weapons[KeyName]->AllowLit = false;
							value = iniFile.GetValue(KeyName, "allowlitcrosshair");
							if(value == "true")
								m_Weapons[KeyName]->AllowLit = true;

							m_Weapons[KeyName]->ZoomAmt = iniFile.GetValueI(KeyName, "zoomamount");

							m_Weapons[KeyName]->ZoomOverlay = NULL;
							value = iniFile.GetValue(KeyName, "zoomoverlay");
							if(!value.empty())
							{
								std::string alpha = iniFile.GetValue(KeyName, "zoomoverlayalpha");

								if(alpha.empty())
									alpha = value;

								m_Weapons[KeyName]->ZoomOverlay = CreateFromFileAndAlphaNames(value.c_str(), alpha.c_str());
								geWorld_AddBitmap(CCD->World(), m_Weapons[KeyName]->ZoomOverlay);
							}

							m_Weapons[KeyName]->MoveZoom = true;
							value = iniFile.GetValue(KeyName, "allowmovezoom");
							if(value == "false")
								m_Weapons[KeyName]->MoveZoom = false;

							m_Weapons[KeyName]->JerkAmt = static_cast<float>(iniFile.GetValueF(KeyName, "recoilamount"));
							m_Weapons[KeyName]->JerkDecay = static_cast<float>(iniFile.GetValueF(KeyName, "recoildecay"));
							m_Weapons[KeyName]->BobAmt = static_cast<float>(iniFile.GetValueF(KeyName, "bobamount"));
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
			value = iniFile.GetValue(KeyName, "playeractor");

			if(!value.empty())
			{
				CFileManager::GetSingletonPtr()->OpenRFFile(&ActorFile, kActorFile, value.c_str(), GE_VFILE_OPEN_READONLY);

				if(ActorFile)
				{
					m_Weapons[KeyName]->PActorDef = geActor_DefCreateFromFile(ActorFile);

					if(m_Weapons[KeyName]->PActorDef)
					{
						m_Weapons[KeyName]->PActor = geActor_Create(m_Weapons[KeyName]->PActorDef);
						if(m_Weapons[KeyName]->PActor)
						{
						if(geWorld_AddActor(CCD->World(), m_Weapons[KeyName]->PActor, 0, 0xffffffff))
						{

						geVec3d Fill = {0.0f, 1.0f, 0.0f};
						geXForm3d Xf;
						geXForm3d XfT;
						geVec3d FillColor = {255.0f, 255.0f, 255.0f};
						geVec3d AmbientColor = {255.0f, 255.0f, 255.0f};
						geBoolean AmbientLightFromFloor = GE_TRUE;
						geVec3d NewFillNormal;
						geActor_GetBoneTransform(m_Weapons[KeyName]->PActor, RootBoneName(m_Weapons[KeyName]->PActor), &Xf);
						geXForm3d_GetTranspose(&Xf, &XfT);
						geXForm3d_Rotate(&XfT, &Fill, &NewFillNormal);

						vector = iniFile.GetValue(KeyName, "playerfillcolor");
						if(!vector.empty())
						{
							FillColor = ToVec3d(vector);
						}

						vector = iniFile.GetValue(KeyName, "playerambientcolor");
						if(!vector.empty())
						{
							AmbientColor = ToVec3d(vector);
						}

						value = iniFile.GetValue(KeyName, "playerambientlightfromfloor");
						if(value == "false")
							AmbientLightFromFloor = GE_FALSE;

						geActor_SetStaticLightingOptions(m_Weapons[KeyName]->PActor, AmbientLightFromFloor, GE_TRUE, 3);

						geActor_SetLightingOptions(m_Weapons[KeyName]->PActor, GE_TRUE, &NewFillNormal, FillColor.X, FillColor.Y, FillColor.Z,
							AmbientColor.X, AmbientColor.Y, AmbientColor.Z, AmbientLightFromFloor, 6, NULL, GE_FALSE);

						value = iniFile.GetValue(KeyName, "playerenvironmentmapping");
						if(value == "true")
						{
							bool all = false;
							value = iniFile.GetValue(KeyName, "playerallmaterial");

							if(value == "true")
								all = true;

							float percent = static_cast<float>(iniFile.GetValueF(KeyName, "playerpercentmapping"));
							float percentm = static_cast<float>(iniFile.GetValueF(KeyName, "playerpercentmaterial"));
							SetEnvironmentMapping(m_Weapons[KeyName]->PActor, true, all, percent, percentm);
						}

						vector = iniFile.GetValue(KeyName, "playerrotation");
						if(!vector.empty())
						{
							m_Weapons[KeyName]->PActorRotation = ToVec3d(vector);
							// convert to radians
							geVec3d_Scale(&m_Weapons[KeyName]->PActorRotation, GE_PIOVER180, &m_Weapons[KeyName]->PActorRotation);
						}

						m_Weapons[KeyName]->PScale = CCD->Player()->GetScale() * static_cast<float>(iniFile.GetValueF(KeyName, "playerscale"));
						m_Weapons[KeyName]->PMOffset = static_cast<float>(iniFile.GetValueF(KeyName, "playermuzzleoffset"));

						m_Weapons[KeyName]->PBone[0] = '\0';
						value = iniFile.GetValue(KeyName, "playerbone");
						if(!value.empty())
							strcpy(m_Weapons[KeyName]->PBone, value.c_str());

						vector = iniFile.GetValue(KeyName, "playerlaunchoffset");
						if(!vector.empty())
						{
							m_Weapons[KeyName]->POffset = ToVec3d(vector);
						}

						m_Weapons[KeyName]->Animations[IDLE]			= iniFile.GetValue(KeyName, "idle");
						m_Weapons[KeyName]->Animations[WALK]			= iniFile.GetValue(KeyName, "walk");
						m_Weapons[KeyName]->Animations[RUN]				= iniFile.GetValue(KeyName, "run");
						m_Weapons[KeyName]->Animations[JUMP]			= iniFile.GetValue(KeyName, "jump");
						m_Weapons[KeyName]->Animations[STARTJUMP]		= iniFile.GetValue(KeyName, "starttojump");
						m_Weapons[KeyName]->Animations[FALL]			= iniFile.GetValue(KeyName, "fall");
						m_Weapons[KeyName]->Animations[LAND]			= iniFile.GetValue(KeyName, "land");
						m_Weapons[KeyName]->Animations[SLIDELEFT]		= iniFile.GetValue(KeyName, "slidetoleft");
						m_Weapons[KeyName]->Animations[RUNSLIDELEFT]	= iniFile.GetValue(KeyName, "slideruntoleft");
						m_Weapons[KeyName]->Animations[SLIDERIGHT]		= iniFile.GetValue(KeyName, "slidetoright");
						m_Weapons[KeyName]->Animations[RUNSLIDERIGHT]	= iniFile.GetValue(KeyName, "slideruntoright");
						m_Weapons[KeyName]->Animations[CRAWL]			= iniFile.GetValue(KeyName, "crawl");
						m_Weapons[KeyName]->Animations[CIDLE]			= iniFile.GetValue(KeyName, "crouchidle");
						m_Weapons[KeyName]->Animations[CSTARTJUMP]		= iniFile.GetValue(KeyName, "crouchstarttojump");
						m_Weapons[KeyName]->Animations[CLAND]			= iniFile.GetValue(KeyName, "crouchland");
						m_Weapons[KeyName]->Animations[SLIDECLEFT]		= iniFile.GetValue(KeyName, "crawlslidetoleft");
						m_Weapons[KeyName]->Animations[SLIDECRIGHT]		= iniFile.GetValue(KeyName, "crawlslidetoright");

						m_Weapons[KeyName]->Animations[SHOOT1]			= iniFile.GetValue(KeyName, "shootup");
						m_Weapons[KeyName]->Animations[SHOOT]			= iniFile.GetValue(KeyName, "shootdwn");
						m_Weapons[KeyName]->Animations[AIM1]			= iniFile.GetValue(KeyName, "aimup");
						m_Weapons[KeyName]->Animations[AIM]				= iniFile.GetValue(KeyName, "aimdwn");
						m_Weapons[KeyName]->Animations[WALKSHOOT1]		= iniFile.GetValue(KeyName, "walkshootup");
						m_Weapons[KeyName]->Animations[WALKSHOOT]		= iniFile.GetValue(KeyName, "walkshootdwn");
						m_Weapons[KeyName]->Animations[RUNSHOOT1]		= iniFile.GetValue(KeyName, "runshootup");
						m_Weapons[KeyName]->Animations[RUNSHOOT]		= iniFile.GetValue(KeyName, "runshootdwn");
						m_Weapons[KeyName]->Animations[SLIDELEFTSHOOT1] = iniFile.GetValue(KeyName, "slidetoleftshootup");
						m_Weapons[KeyName]->Animations[SLIDELEFTSHOOT]	= iniFile.GetValue(KeyName, "slidetoleftshootdwn");
						m_Weapons[KeyName]->Animations[RUNSLIDELEFTSHOOT1]	= iniFile.GetValue(KeyName, "slideruntoleftshootup");
						m_Weapons[KeyName]->Animations[RUNSLIDELEFTSHOOT]	= iniFile.GetValue(KeyName, "slideruntoleftshootdwn");
						m_Weapons[KeyName]->Animations[SLIDERIGHTSHOOT1]	= iniFile.GetValue(KeyName, "slidetorightshootup");
						m_Weapons[KeyName]->Animations[SLIDERIGHTSHOOT]		= iniFile.GetValue(KeyName, "slidetorightshootdwn");
						m_Weapons[KeyName]->Animations[RUNSLIDERIGHTSHOOT1] = iniFile.GetValue(KeyName, "slideruntorightshootup");
						m_Weapons[KeyName]->Animations[RUNSLIDERIGHTSHOOT]	= iniFile.GetValue(KeyName, "slideruntorightshootdwn");
						m_Weapons[KeyName]->Animations[JUMPSHOOT1]		= iniFile.GetValue(KeyName, "jumpshootup");
						m_Weapons[KeyName]->Animations[JUMPSHOOT]		= iniFile.GetValue(KeyName, "jumpshootdwn");
						m_Weapons[KeyName]->Animations[FALLSHOOT1]		= iniFile.GetValue(KeyName, "fallshootup");
						m_Weapons[KeyName]->Animations[FALLSHOOT]		= iniFile.GetValue(KeyName, "fallshootdwn");
						m_Weapons[KeyName]->Animations[CAIM1]			= iniFile.GetValue(KeyName, "crouchaimup");
						m_Weapons[KeyName]->Animations[CAIM]			= iniFile.GetValue(KeyName, "crouchaimdwn");
						m_Weapons[KeyName]->Animations[CSHOOT1]			= iniFile.GetValue(KeyName, "crouchshootup");
						m_Weapons[KeyName]->Animations[CSHOOT]			= iniFile.GetValue(KeyName, "crouchshootdwn");
						m_Weapons[KeyName]->Animations[CRAWLSHOOT1]		= iniFile.GetValue(KeyName, "crawlshootup");
						m_Weapons[KeyName]->Animations[CRAWLSHOOT]		= iniFile.GetValue(KeyName, "crawlshootdwn");
						m_Weapons[KeyName]->Animations[SLIDECLEFTSHOOT1]	= iniFile.GetValue(KeyName, "crawlslidetoleftshootup");
						m_Weapons[KeyName]->Animations[SLIDECLEFTSHOOT]		= iniFile.GetValue(KeyName, "crawlslidetoleftshootdwn");
						m_Weapons[KeyName]->Animations[SLIDECRIGHTSHOOT1]	= iniFile.GetValue(KeyName, "crawlslidetorightshootup");
						m_Weapons[KeyName]->Animations[SLIDECRIGHTSHOOT]	= iniFile.GetValue(KeyName, "crawlslidetorightshootdwn");

						m_Weapons[KeyName]->Animations[SWIM]			= iniFile.GetValue(KeyName, "swim");
						m_Weapons[KeyName]->Animations[TREADWATER]		= iniFile.GetValue(KeyName, "treadwater");

						m_Weapons[KeyName]->Animations[I2WALK]			= iniFile.GetValue(KeyName, "idletowalk");
						m_Weapons[KeyName]->Animations[I2RUN]			= iniFile.GetValue(KeyName, "idletorun");
						m_Weapons[KeyName]->Animations[W2IDLE]			= iniFile.GetValue(KeyName, "walktoidle");
						m_Weapons[KeyName]->Animations[C2IDLE]			= iniFile.GetValue(KeyName, "crawltoidle");
						m_Weapons[KeyName]->Animations[CROUCH2IDLE]		= iniFile.GetValue(KeyName, "crouchtoidle");
						m_Weapons[KeyName]->Animations[IDLE2CROUCH]		= iniFile.GetValue(KeyName, "idletocrouch");
						m_Weapons[KeyName]->Animations[SWIM2TREAD]		= iniFile.GetValue(KeyName, "swimtotread");
						m_Weapons[KeyName]->Animations[TREAD2SWIM]		= iniFile.GetValue(KeyName, "treadtoswim");
						m_Weapons[KeyName]->Animations[IDLE2TREAD]		= iniFile.GetValue(KeyName, "idletotread");
						m_Weapons[KeyName]->Animations[SWIM2WALK]		= iniFile.GetValue(KeyName, "swimtowalk");
						m_Weapons[KeyName]->Animations[WALK2SWIM]		= iniFile.GetValue(KeyName, "walktoswim");
						m_Weapons[KeyName]->Animations[TREAD2IDLE]		= iniFile.GetValue(KeyName, "treadtoidle");
						m_Weapons[KeyName]->Animations[JUMP2FALL]		= iniFile.GetValue(KeyName, "jumptofall");
						m_Weapons[KeyName]->Animations[JUMP2TREAD]		= iniFile.GetValue(KeyName, "jumptotread");
						m_Weapons[KeyName]->Animations[FALL2TREAD]		= iniFile.GetValue(KeyName, "falltotread");
						m_Weapons[KeyName]->Animations[FALL2CRAWL]		= iniFile.GetValue(KeyName, "falltocrawl");
						m_Weapons[KeyName]->Animations[FALL2WALK]		= iniFile.GetValue(KeyName, "falltowalk");
						m_Weapons[KeyName]->Animations[FALL2JUMP]		= iniFile.GetValue(KeyName, "falltojump");
						m_Weapons[KeyName]->Animations[WALK2JUMP]		= iniFile.GetValue(KeyName, "walktojump");
						m_Weapons[KeyName]->Animations[WALK2CRAWL]		= iniFile.GetValue(KeyName, "walktocrawl");
						m_Weapons[KeyName]->Animations[CRAWL2WALK]		= iniFile.GetValue(KeyName, "crawltowalk");
						m_Weapons[KeyName]->Animations[IDLE2CRAWL]		= iniFile.GetValue(KeyName, "idletocrawl");
						m_Weapons[KeyName]->Animations[AIM2CROUCH]		= iniFile.GetValue(KeyName, "aimtocrouch");
						m_Weapons[KeyName]->Animations[CROUCH2AIM]		= iniFile.GetValue(KeyName, "crouchtoaim");
						m_Weapons[KeyName]->Animations[W2TREAD]			= iniFile.GetValue(KeyName, "walktotread");
						m_Weapons[KeyName]->Animations[FALL2RUN]		= iniFile.GetValue(KeyName, "falltorun");
						m_Weapons[KeyName]->Animations[CRAWL2RUN]		= iniFile.GetValue(KeyName, "crawltorun");

						m_Weapons[KeyName]->Animations[WALKBACK]		= iniFile.GetValue(KeyName, "walkback");
						m_Weapons[KeyName]->Animations[RUNBACK]			= iniFile.GetValue(KeyName, "runback");
						m_Weapons[KeyName]->Animations[CRAWLBACK]		= iniFile.GetValue(KeyName, "crawlback");
						m_Weapons[KeyName]->Animations[WALKSHOOT1BACK]	= iniFile.GetValue(KeyName, "walkshootupback");
						m_Weapons[KeyName]->Animations[WALKSHOOTBACK]	= iniFile.GetValue(KeyName, "walkshootdwnback");
						m_Weapons[KeyName]->Animations[RUNSHOOT1BACK]	= iniFile.GetValue(KeyName, "runshootupback");
						m_Weapons[KeyName]->Animations[RUNSHOOTBACK]	= iniFile.GetValue(KeyName, "runshootdwnback");
						m_Weapons[KeyName]->Animations[CRAWLSHOOT1BACK] = iniFile.GetValue(KeyName, "crawlshootupback");
						m_Weapons[KeyName]->Animations[CRAWLSHOOTBACK]	= iniFile.GetValue(KeyName, "crawlshootdwnback");
						m_Weapons[KeyName]->Animations[SWIMBACK]		= iniFile.GetValue(KeyName, "swimback");

						// die animations
						value = iniFile.GetValue(KeyName, "die");
						m_Weapons[KeyName]->DieAnimAmt = 0;
						if(!value.empty())
						{
							char strip[256];
							char *subString;
							strcpy(strip, value.c_str());
							subString = strtok(strip, " \n");

							while(subString)
							{
								m_Weapons[KeyName]->DieAnim[m_Weapons[KeyName]->DieAnimAmt] = subString;
								m_Weapons[KeyName]->DieAnimAmt++;
								if(m_Weapons[KeyName]->DieAnimAmt == 5)
									break;
								subString = strtok(NULL," \n");
							}
						}

						// injury animations
						value = iniFile.GetValue(KeyName, "injury");
						m_Weapons[KeyName]->InjuryAnimAmt = 0;
						if(!value.empty())
						{
							char strip[256];
							char *subString;
							strcpy(strip, value.c_str());
							subString = strtok(strip, " \n");
							while(subString)
							{
								m_Weapons[KeyName]->InjuryAnim[m_Weapons[KeyName]->InjuryAnimAmt] = subString;
								m_Weapons[KeyName]->InjuryAnimAmt++;
								if(m_Weapons[KeyName]->InjuryAnimAmt == 5)
									break;
								subString = strtok(NULL, " \n");
							}
						}

						activeflag = true;
						}
						}
					}

					geVFile_Close(ActorFile);
				}
			}

			if(!activeflag)
			{
				delete m_Weapons[KeyName];
				m_Weapons.erase(KeyName);
			}
		}

		KeyName = iniFile.FindNextKey();
	}
}

/* ------------------------------------------------------------------------------------ */
// GetFixedView
/* ------------------------------------------------------------------------------------ */
int CWeapon::GetFixedView()
{
	if(m_CurrentWeapon.empty())
		return -1;

	return m_Weapons[m_CurrentWeapon]->FixedView;
}


/* ----------------------------------- END OF FILE ------------------------------------ */
