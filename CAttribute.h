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
  bool HandleCollision(geActor *theTarget, geActor *pActor);
// changed RF063
  int SaveTo(FILE *SaveFD);
  int RestoreFrom(FILE *RestoreFD);
// end change RF063
  int ReSynchronize();
  int LocateEntity(char *szName, void **pEntityData);
};

#endif

// --------------------------------------------------------------------------
