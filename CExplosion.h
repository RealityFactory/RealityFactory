/*

    ExplosionManager.h               Explosion Manager handler

	(c) 1999 Ralph Deane

	This file contains the class declarations for the Explosion
Manager that handles Explosions for RGF-based games.
*/

#ifndef __RGF_EXPMANAGER_H_
#define __RGF_EXPMANAGER_H_

#define MAXEXP 50

typedef struct preExplosion
{
  geBoolean	Active;
  char Name[64];
  char Effect[10][64];
  float Delay[10];
  geVec3d Offset[10];
} preExplosion;

typedef struct DelayExp
{
	DelayExp * prev;
	DelayExp * next;
	int Type;
	geVec3d Position;
	geVec3d Offset;
	float Delay;
} DelayExp;

class CExplosionInit : public CRGFComponent
{
public:
  CExplosionInit();
  ~CExplosionInit();	
  void AddExplosion(char *Name, geVec3d Position);
  void Tick(geFloat dwTicks);
private:
	preExplosion Explosions[MAXEXP];
	DelayExp * Bottom;
};


class CExplosion : public CRGFComponent
{
public:
  CExplosion();
  ~CExplosion();	
  void Tick(geFloat dwTicks);
private:
};

#endif
