/*
	CFloating.h:		Floating Particle handler

	(c) 1999 Ralph Deane

	This file contains the class declaration for the CFloat
class that encapsulates floating particle based special effects for
RGF-based games.
*/

#ifndef __RGF_CFLOAT_H_
#define __RGF_CFLOAT_H_

#pragma warning( disable : 4068 )

#include "genesis.h"

// class declaration for CFloat
class CFloat : public CRGFComponent
{
public:
  CFloat();
  ~CFloat();	
  void Tick(float dwTicks);
  int LocateEntity(char *szName, void **pEntityData);
  int ReSynchronize();
private:
};


#endif

