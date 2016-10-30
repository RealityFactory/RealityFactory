/************************************************************************************//**
 * @file CExplosion.h
 * @brief Explosion Manager handler
 *
 * This file contains the class declarations for the Explosion Manager that
 * handles Explosions for RGF-based games.
 * @author Ralph Deane
 *//*
 * Copyright(c) 1999 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_EXPMANAGER_H_
#define __RGF_EXPMANAGER_H_

#include <hash_map>
#include <list>

/**
 * @brief Predefined explosion
 */
typedef struct ExplosionDefinition
{
	float		Delay[10];
	geVec3d		Offset[10];
	std::string	EffectName[10];

} ExplosionDefinition;


typedef struct DelayExp
{
	geVec3d		Position;
	geVec3d		Offset;
	float		Delay;
	bool		Attached;
	geActor*	Actor;
	char		Bone[64];
	int			index;
	bool		Tilt;
	std::string	EffectName;

	DelayExp() :
		Delay(0.f),
		Attached(false),
		Actor(NULL),
		index(0),
		Tilt(false)
	{
		geVec3d_Clear(&Position);
		geVec3d_Clear(&Offset);
	    Bone[0] = 0;
	}

	// copy constructor
	DelayExp(const DelayExp& other) :
		Position(other.Position),
		Offset(other.Offset),
		Delay(other.Delay),
		Attached(other.Attached),
		Actor(other.Actor),
		index(other.index),
		Tilt(other.Tilt),
		EffectName(other.EffectName)
	{
		strcpy(Bone, other.Bone);
	}
	//assignment operator
	DelayExp& operator=(const DelayExp& other)
	{
		Position = other.Position;
		Offset = other.Offset;
		Delay = other.Delay;
		Attached = other.Attached;
		Actor = other.Actor;
		strcpy(Bone, other.Bone);
		index = other.index;
		Tilt = other.Tilt;
		EffectName = other.EffectName;
		return *this;
	}


} DelayExp;


/**
 * @brief CExplosionManager handles predefined explosion effects
 */
class CExplosionManager : public CRGFComponent
{
public:
	CExplosionManager();
	~CExplosionManager();

	void AddExplosion(const std::string& name, const geVec3d& position,
		geActor* actor = NULL, const char* bone = NULL, bool tilt = false);

	void Tick(float timeElapsed);

	void UnAttach(const geActor* actor);

private:
	stdext::hash_map<std::string, ExplosionDefinition*> m_Explosions;
	std::list<DelayExp> m_DelayExplosions;
};


/**
 * @brief CExplosion handles Explosion entities
 */
class CExplosion : public CRGFComponent
{
public:
	CExplosion();
	~CExplosion();

	void Tick(float timeElapsed);
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
