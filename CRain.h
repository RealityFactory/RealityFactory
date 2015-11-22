/*
	CRain.h:		Rain handler

	(c) 1999 Ralph Deane

	This file contains the class declaration for the CRain
class that encapsulates rain based special effects for
RGF-based games.
*/

#ifndef __RGF_CRAIN_H_
#define __RGF_CRAIN_H_

#pragma warning( disable : 4068 )

#include "genesis.h"

// class declaration for CRain
class CRain : public CRGFComponent
{
public:
  CRain();
  ~CRain();	
  int Create(Rain *R);
  void Tick(float dwTicks);
  int LocateEntity(char *szName, void **pEntityData);
  int ReSynchronize();
};


#endif

