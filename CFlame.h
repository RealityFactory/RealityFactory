

#ifndef __RGF_CFLAME_H_
#define __RGF_CFLAME_H_

#pragma warning( disable : 4068 )

#include "genesis.h"

// class declaration for CFlame
class CFlame : public CRGFComponent
{
public:
  CFlame();
  ~CFlame();	
  void Tick(float dwTicks);
  int LocateEntity(char *szName, void **pEntityData);
  int ReSynchronize();
private:
  int CreateS(Flame *S);
  int CreateG(Flame *S);
};


#endif

