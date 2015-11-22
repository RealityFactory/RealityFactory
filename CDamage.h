/*
	CDamage.h:		Damage class handler

	(c) 1999 Edward A. Averill, III
	All Rights Reserved

	This file contains the class declaration for damage
handling.
*/

#ifndef __RGF_CDAMAGE_H_
#define __RGF_CDAMAGE_H_

class CDamage : public CRGFComponent
{
public:
  CDamage();
  ~CDamage();
  void Tick(float dwTicks);
  void DamageActor(geActor *Actor, float amount, char *Attr);
  void DamageActorInRange(geVec3d Point, geFloat Range, float amount, char *Attr);
  void DamageModel(geWorld_Model *Model, float amount, char *Attr);
  void DamageModelInRange(geVec3d Point, geFloat Range, float amount, char *Attr);
  bool IsDestroyable(geWorld_Model *Model, int *Percentage);
  int ReSynchronize();
  int LocateEntity(char *szName, void **pEntityData);
private:

};

#endif