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

#define MAXEXP 50

/**
 * @brief Predefined explosion
 */
typedef struct preExplosion
{
	geBoolean	Active;
	char		Name[64];
	char		Effect[10][64];
	float		Delay[10];
	geVec3d		Offset[10];

} preExplosion;

typedef struct DelayExp
{
	DelayExp	*prev;
	DelayExp	*next;
	int			Type;
	geVec3d		Position;
	geVec3d		Offset;
	float		Delay;
	bool		Attached;
// changed RF063
	geActor		*Actor;
	char		Bone[64];
	int			index;
	bool		Tilt;
// end change RF063

} DelayExp;

/**
 * @brief CExplosionInit handles predefined explosion effects
 */
class CExplosionInit : public CRGFComponent
{
public:
	CExplosionInit();
	~CExplosionInit();

	void AddExplosion(const char *Name, const geVec3d &Position, geActor *theActor, const char *theBone);
	void AddExplosion(const char *Name, const geVec3d &Position, geActor *theActor, const char *theBone, bool Tilt);
	void AddExplosion(const char *Name, const geVec3d &Position);
	void Tick(geFloat dwTicks);
	void UnAttach(const geActor *Actor);

private:

	preExplosion Explosions[MAXEXP];
	DelayExp *Bottom;
};

/**
 * @brief CExplosion handles Explosion entities
 */
class CExplosion : public CRGFComponent
{
public:
	CExplosion();
	~CExplosion();

	void Tick(geFloat dwTicks);
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
