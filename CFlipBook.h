

#ifndef __RGF_CFLIP_H_
#define __RGF_CFLIP_H_

#pragma warning( disable : 4068 )

#include "genesis.h"

// class declaration for CFlipBook
class CFlipBook : public CRGFComponent
{
public:
  CFlipBook();
  ~CFlipBook();	
  void Tick(float dwTicks);
  int LocateEntity(char *szName, void **pEntityData);
  int ReSynchronize();
private:
  int CreateS(FlipBook *S);
};


#endif

