
#ifndef __RGF_CFOLIAGE_H_
#define __RGF_CFOLIAGE_H_

#pragma warning( disable : 4068 )

#include "genesis.h"

// class declaration for CFlipBook
class CFoliage : public CRGFComponent
{
public:
  CFoliage();
  ~CFoliage();	
  void Tick(float dwTicks);
};

#endif
