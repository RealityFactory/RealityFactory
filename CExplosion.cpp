/************************************************************************************//**
 * @file CExplosion.cpp
 * @brief Explosion Manager handler
 *
 * This file contains the class implementation for the Explosion Manager that
 * handles Explosions for RGF-based games.
 * @author Ralph Deane
 *//*
 * Copyright(c) 1999 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#include "RabidFramework.h"
#include <Ram.h>
#include "IniFile.h"
#include "CLevel.h"
#include "CDamage.h"

/* ------------------------------------------------------------------------------------ */
// Constructor
/* ------------------------------------------------------------------------------------ */
CExplosionManager::CExplosionManager()
{
	CIniFile iniFile("explosion.ini");

	if(!iniFile.ReadFile())
	{
		CCD->Log()->Warning("Failed to open explosion.ini file");
		return;
	}

	std::string explosionName = iniFile.FindFirstKey();

	while(!explosionName.empty())
	{
		if(m_Explosions.find(explosionName) != m_Explosions.end())
		{
			CCD->Log()->Warning("Redefinition of explosion [" + explosionName + "] in explosion.ini file.");
			explosionName = iniFile.FindNextKey();
			continue;
		}

		std::string effectName, vector;
		std::string effect, delay, offset;

		m_Explosions[explosionName] = new ExplosionDefinition;

		for(char i=0; i<10; ++i)
		{
			char count = '0' + i;
			effect	= "effect";	effect	+= count;
			delay	= "delay";	delay	+= count;
			offset	= "offset";	offset	+= count;

			effectName = iniFile.GetValue(explosionName, effect);

			if(!effectName.empty())
			{
				m_Explosions[explosionName]->EffectName[i] = effectName;
				m_Explosions[explosionName]->Delay[i] = static_cast<float>(iniFile.GetValueF(explosionName, delay));

				vector = iniFile.GetValue(explosionName, offset);

				if(!vector.empty())
				{
					m_Explosions[explosionName]->Offset[i] = ToVec3d(vector);
				}
			}
		}

		explosionName = iniFile.FindNextKey();
	}
}


/* ------------------------------------------------------------------------------------ */
// Destructor
/* ------------------------------------------------------------------------------------ */
CExplosionManager::~CExplosionManager()
{
	m_DelayExplosions.clear();

	stdext::hash_map<std::string, ExplosionDefinition*>::iterator iter = m_Explosions.begin();

	for(; iter!=m_Explosions.end(); ++iter)
	{
		delete (iter->second);
	}

	m_Explosions.clear();
}


/* ------------------------------------------------------------------------------------ */
// AddExplosion
/* ------------------------------------------------------------------------------------ */
void CExplosionManager::AddExplosion(const std::string& name, const geVec3d& position,
									geActor* actor, const char* bone, bool tilt)
{
	if(m_Explosions.find(name) != m_Explosions.end())
	{
		for(int i=0; i<10; ++i)
		{
			if(!m_Explosions[name]->EffectName[i].empty())
			{
				if(CCD->Effect()->EffectExists(m_Explosions[name]->EffectName[i]))
				{
					DelayExp explosion;

					explosion.EffectName = m_Explosions[name]->EffectName[i];
					explosion.Offset = m_Explosions[name]->Offset[i];
					geVec3d_Add(&position, &explosion.Offset, &explosion.Offset);
					explosion.Delay = m_Explosions[name]->Delay[i];
					explosion.Attached = false;
					explosion.Actor = actor;
					explosion.Bone[0] = '\0';
					explosion.Tilt = tilt;

					if(actor != NULL)
					{
						if(bone != NULL)
							strcpy(explosion.Bone, bone);

						geXForm3d xForm;

						if(explosion.Bone[0] != '\0')
							geActor_GetBoneTransform(explosion.Actor, explosion.Bone, &xForm);
						else
							geActor_GetBoneTransform(explosion.Actor, RootBoneName(explosion.Actor), &xForm);

						explosion.Position = xForm.Translation;
					}

					m_DelayExplosions.push_back(explosion);
				}
			}
		}
	}
}


/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
struct isAttachedToActor
{
	isAttachedToActor(const geActor *actor): actor_( actor ) {}

	bool operator() (const DelayExp& explosion)
	{ return (explosion.Attached && explosion.Actor == actor_); }

	const geActor *actor_;
};


/* ------------------------------------------------------------------------------------ */
// UnAttach
/* ------------------------------------------------------------------------------------ */
void CExplosionManager::UnAttach(const geActor *actor)
{
	if(!actor) return;

	m_DelayExplosions.remove_if(isAttachedToActor(actor));
}


/* ------------------------------------------------------------------------------------ */
// Update
/* ------------------------------------------------------------------------------------ */
void CExplosionManager::Tick(float timeElapsed)
{
	geVec3d zero = {0.0f, 0.0f, 0.0f};

	std::list<DelayExp>::iterator explosion = m_DelayExplosions.begin();

	while(explosion!=m_DelayExplosions.end())
	{
		explosion->Delay -= timeElapsed * 0.001f;

		if(!explosion->Attached) // not (yet) attached to any actor
		{
			if(explosion->Delay <= 0.0f) // the waiting has come to an end, it's our turn
			{
				if(explosion->Actor) // there's an actor to attach to
				{
					geXForm3d xForm;

					if(explosion->Bone[0] != '\0')
						geActor_GetBoneTransform(explosion->Actor, explosion->Bone, &xForm);
					else
						geActor_GetBoneTransform(explosion->Actor, RootBoneName(explosion->Actor), &xForm);

					geVec3d position = xForm.Translation;

					geActor_GetBoneTransform(explosion->Actor, NULL, &xForm);

					if(explosion->Tilt)
						geXForm3d_RotateX(&xForm, CCD->CameraManager()->GetTilt());

					geVec3d direction;
					geXForm3d_GetIn(&xForm, &direction);

					if(!explosion->Tilt)
						geVec3d_Inverse(&direction);

					geVec3d_AddScaled(&position, &direction, explosion->Offset.Z, &position);

					geXForm3d_GetUp(&xForm, &direction);
					geVec3d_AddScaled(&position, &direction, explosion->Offset.Y, &position);

					geXForm3d_GetLeft(&xForm, &direction);
					geVec3d_AddScaled(&position, &direction, explosion->Offset.X, &position);

					explosion->index = CCD->Effect()->AddEffect(explosion->EffectName, position, zero);

					explosion->Attached = true;

					int type = CCD->Effect()->EffectType(explosion->EffectName);

					switch(type)
					{
					case EFF_SPRAY:
						Spray spray;
						geVec3d_Copy(&position, &spray.Source);
						geXForm3d_Copy(&xForm, &spray.Xform);
						CCD->EffectManager()->Item_Modify(EFF_SPRAY, explosion->index, &spray, SPRAY_SOURCE | SPRAY_DEST);
						break;
					case EFF_ACTORSPRAY:
						ActorSpray aSpray;
						geVec3d_Copy(&position, &aSpray.Source);
						geXForm3d_Copy(&xForm, &aSpray.Xform);
						CCD->EffectManager()->Item_Modify(EFF_ACTORSPRAY, explosion->index, &aSpray, SPRAY_SOURCE | SPRAY_DEST);
						break;
					case EFF_BOLT:
						EBolt bolt;
						geVec3d_Copy(&position, &(bolt.Start));
						geXForm3d_Copy(&xForm, &bolt.Xform);
						CCD->EffectManager()->Item_Modify(EFF_BOLT, explosion->index, &bolt, BOLT_START | BOLT_ENDOFFSET);
						break;
					}
				}
				else // explosion remains unattached
				{
					// go, spread your wings ...
					explosion->index = CCD->Effect()->AddEffect(explosion->EffectName, explosion->Position, explosion->Offset);

					m_DelayExplosions.erase(explosion++);
					continue;
				}
			}
		}
		else // currently attached to an actor
		{
			// update position if still alive
			if(CCD->EffectManager()->Item_Alive(explosion->index) && explosion->Actor)
			{
				geXForm3d xForm;

				if(explosion->Bone[0] != '\0')
					geActor_GetBoneTransform(explosion->Actor, explosion->Bone, &xForm);
				else
					geActor_GetBoneTransform(explosion->Actor, RootBoneName(explosion->Actor), &xForm);

				geVec3d position = xForm.Translation;

				geVec3d direction;

				geActor_GetBoneTransform(explosion->Actor, NULL, &xForm);

				if(explosion->Tilt)
					geXForm3d_RotateX(&xForm, CCD->CameraManager()->GetTilt());

				geXForm3d_GetIn(&xForm, &direction);

				if(!explosion->Tilt)
					geVec3d_Inverse(&direction);

				geVec3d_AddScaled(&position, &direction, explosion->Offset.Z, &position);

				geXForm3d_GetUp(&xForm, &direction);
				geVec3d_AddScaled(&position, &direction, explosion->Offset.Y, &position);

				geXForm3d_GetLeft(&xForm, &direction);
				geVec3d_AddScaled(&position, &direction, explosion->Offset.X, &position);

				int type = CCD->Effect()->EffectType(explosion->EffectName);

				switch(type)
				{
				case EFF_SPRAY:
					Spray spray;
					geVec3d_Copy(&position, &spray.Source);
					geXForm3d_Copy(&xForm, &spray.Xform);
					CCD->EffectManager()->Item_Modify(EFF_SPRAY, explosion->index, &spray, SPRAY_SOURCE | SPRAY_DEST);
					break;
				case EFF_LIGHT:
					Glow light;
					geVec3d_Copy(&position, &light.Pos);
					CCD->EffectManager()->Item_Modify(EFF_LIGHT, explosion->index, &light, GLOW_POS);
					break;
				case EFF_SND:
					Snd sound;
					geVec3d_Copy(&position, &sound.Pos);
					CCD->EffectManager()->Item_Modify(EFF_SND, explosion->index, &sound, SND_POS);
					break;
				case EFF_SPRITE:
					Sprite sprite;
					geVec3d_Copy(&position, &sprite.Pos);
					CCD->EffectManager()->Item_Modify(EFF_SPRITE, explosion->index, &sprite, SPRITE_POS);
					break;
				case EFF_CORONA:
					EffCorona corona;
					corona.Vertex.X = position.X;
					corona.Vertex.Y = position.Y;
					corona.Vertex.Z = position.Z;
					CCD->EffectManager()->Item_Modify(EFF_CORONA, explosion->index, &corona, CORONA_POS);
					break;
				case EFF_BOLT:
					EBolt bolt;
					geVec3d_Copy(&position, &bolt.Start);
					geXForm3d_Copy(&xForm, &bolt.Xform);
					CCD->EffectManager()->Item_Modify(EFF_BOLT, explosion->index, &bolt, BOLT_START | BOLT_ENDOFFSET);
					break;
				case EFF_ACTORSPRAY:
					ActorSpray aSpray;
					geVec3d_Copy(&position, &aSpray.Source);
					geXForm3d_Copy(&xForm, &aSpray.Xform);
					CCD->EffectManager()->Item_Modify(EFF_ACTORSPRAY, explosion->index, &aSpray, SPRAY_SOURCE | SPRAY_DEST);
					break;
				}
			}
			else // effect is dead, remove element from list
			{
				// alternatively, explosion = m_DelayExplosions.erase(explosion);
				m_DelayExplosions.erase(explosion++);
				continue;
			}
		}

		++explosion;
	}
}


/* ------------------------------------------------------------------------------------ */
// Constructor
/* ------------------------------------------------------------------------------------ */
CExplosion::CExplosion()
{
	geEntity_EntitySet *pSet;

	// Ok, see if we have any Explosion entities at all
	pSet = geWorld_GetEntitySet(CCD->World(), "Explosion");

	if(!pSet)
		return;

	geEntity *entity;

	// Look through all of our Explosions
	for(entity=geEntity_EntitySetGetNextEntity(pSet, NULL); entity;
		entity=geEntity_EntitySetGetNextEntity(pSet, entity))
	{
		Explosion *explosion = static_cast<Explosion*>(geEntity_GetUserData(entity));

		explosion->OriginOffset = explosion->origin;

		if(explosion->Model)
		{
			geVec3d modelOrigin;
			geWorld_GetModelRotationalCenter(CCD->World(), explosion->Model, &modelOrigin);
			geVec3d_Subtract(&explosion->origin, &modelOrigin, &explosion->OriginOffset);
		}

		explosion->active = GE_FALSE;
	}
}


/* ------------------------------------------------------------------------------------ */
// Destructor
/* ------------------------------------------------------------------------------------ */
CExplosion::~CExplosion()
{
}


/* ------------------------------------------------------------------------------------ */
// Tick
/* ------------------------------------------------------------------------------------ */
void CExplosion::Tick(float /*timeElapsed*/)
{
	// Check to see if there are Explosions in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "Explosion");

	if(pSet)
	{
		geEntity *entity;

		// Ok, we have Explosions somewhere. Dig through 'em all.
		for(entity=geEntity_EntitySetGetNextEntity(pSet, NULL); entity;
			entity=geEntity_EntitySetGetNextEntity(pSet, entity))
		{
			Explosion *explosion = static_cast<Explosion*>(geEntity_GetUserData(entity));

			explosion->origin = explosion->OriginOffset;
			SetOriginOffset(explosion->EntityName, explosion->BoneName, explosion->Model, &explosion->origin);

			if(!EffectC_IsStringNull(explosion->TriggerName))
			{
				if(GetTriggerState(explosion->TriggerName))
				{
					if(explosion->active == GE_FALSE)
					{
						CCD->Level()->ExplosionManager()->AddExplosion(explosion->ExplosionName, explosion->origin);

						if(explosion->ShakeAmt > 0.0f)
							CCD->CameraManager()->SetShake(explosion->ShakeAmt, explosion->ShakeDecay, explosion->origin);

						explosion->active = GE_TRUE;

						if(!EffectC_IsStringNull(explosion->DamageAttribute))
						{
							if(explosion->Radius > 0.0f)
							{
								CCD->Level()->Damage()->DamageActorInRange(explosion->origin, explosion->Radius,
													explosion->DamageAmt, explosion->DamageAttribute,
													explosion->DamageAltAmt, explosion->DamageAltAttribute,
													"Explosion");
								CCD->Level()->Damage()->DamageModelInRange(explosion->origin, explosion->Radius,
													explosion->DamageAmt, explosion->DamageAttribute,
													explosion->DamageAltAmt, explosion->DamageAltAttribute);
							}
						}
					}
				}
				else
				{
					if(explosion->active == GE_TRUE)
					{
						explosion->active = GE_FALSE;
					}
				}
			}
			else
			{
				if(explosion->active == GE_FALSE)
				{
					CCD->Level()->ExplosionManager()->AddExplosion(explosion->ExplosionName, explosion->origin);

					if(explosion->ShakeAmt > 0.0f)
						CCD->CameraManager()->SetShake(explosion->ShakeAmt, explosion->ShakeDecay, explosion->origin);

					explosion->active = GE_TRUE;

					if(!EffectC_IsStringNull(explosion->DamageAttribute))
					{
						if(explosion->Radius > 0.0f)
						{
							CCD->Level()->Damage()->DamageActorInRange(explosion->origin, explosion->Radius,
													explosion->DamageAmt, explosion->DamageAttribute,
													explosion->DamageAltAmt, explosion->DamageAltAttribute,
													"Explosion");
							CCD->Level()->Damage()->DamageModelInRange(explosion->origin, explosion->Radius,
													explosion->DamageAmt, explosion->DamageAttribute,
													explosion->DamageAltAmt, explosion->DamageAltAttribute);
						}
					}
				}
			}
		}
	}
}

/* ----------------------------------- END OF FILE ------------------------------------ */
