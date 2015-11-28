
#ifndef __RGF_CFLIPTREE_H_
#define __RGF_CFLIPTREE_H_

#pragma warning( disable : 4068 )

#include "genesis.h"

// class declaration for CFlipBook
class CFlipTree : public CRGFComponent
{
public:
  CFlipTree();
  ~CFlipTree();	
  void Tick(float dwTicks);
};

#endif