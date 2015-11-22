/*

    CEffects.h               Predefined effects handler

	(c) 2001 Ralph Deane

	This file contains the class declarations for the predefined
	effects for RGF-based games.
*/

#ifndef __RGF_PREEFFMANAGER_H_
#define __RGF_PREEFFMANAGER_H_

#define MAXEXPITEM 200

typedef struct ExpItem
{
  geBoolean	Active;
  char Name[64];
  int Type;
  void *Data;
} ExpItem;

class CPreEffect : public CRGFComponent
{
public:
  CPreEffect();
  ~CPreEffect();	
  int AddEffect(int k, geVec3d Position, geVec3d Offset);
  bool EffectActive(int k)
  { return Effects[k].Active; }
  char *EffectName(int k)
  { return Effects[k].Name; }
  int EffectType(int k)
  { return Effects[k].Type; }
private:
  ExpItem Effects[MAXEXPITEM];
};

#endif
