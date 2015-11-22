/*
	CDamage.h:		Damage class handler

	(c) 2001 Ralph Deane
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
// changed RF063
  int SaveTo(FILE *SaveFD);
  int RestoreFrom(FILE *RestoreFD);
  void DamageActor(geActor *Actor, float amount, char *Attr, float Altamount, char *AltAttr);
  void DamageActorInRange(geVec3d Point, geFloat Range, float amount, char *Attr, float Altamount, char *AltAttr);
  void DamageModel(geWorld_Model *Model, float amount, char *Attr, float Altamount, char *AltAttr);
  void DamageModelInRange(geVec3d Point, geFloat Range, float amount, char *Attr, float Altamount, char *AltAttr);
// end change RF063
  bool IsDestroyable(geWorld_Model *Model, int *Percentage);
  int ReSynchronize();
  int LocateEntity(char *szName, void **pEntityData);
private:

};

#endif