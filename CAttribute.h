/*
	CAttribute.h:		Attribute class handler

	(c) 1999 Edward A. Averill, III
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
  int ReSynchronize();
  int LocateEntity(char *szName, void **pEntityData);
};

#endif

// --------------------------------------------------------------------------
