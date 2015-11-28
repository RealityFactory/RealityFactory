/*
	CAttribute.h:		Attribute class handler

	(c) 2001 Ralph Deane
	All Rights Reserved

	This file contains the class declaration for Attribute
handling.
*/

#ifndef __RGF_CATTRIBUTE_H_
#define __RGF_CATTRIBUTE_H_

class CAttribute : public CRGFComponent
{
public:
  CAttribute();
  ~CAttribute();
  void Tick(float dwTicks);
// changed QuestOfDreams 08/13/03 added UseKey
  bool HandleCollision(geActor *theTarget, geActor *pActor, bool UseKey);
// end change 08/13/03
// changed RF063
  int SaveTo(FILE *SaveFD, bool type);
  int RestoreFrom(FILE *RestoreFD, bool type);
// end change RF063
  int ReSynchronize();
  int LocateEntity(char *szName, void **pEntityData);
};

#endif

// --------------------------------------------------------------------------
