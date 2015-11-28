#ifndef __RGF_CAREACHECK_H_
#define __RGF_CAREACHECK_H_

#pragma warning( disable : 4068 )

#include "genesis.h"

class CAreaChecker : public CRGFComponent
{
public:
  CAreaChecker();
  ~CAreaChecker();
  bool IsCloseEnough(geVec3d Pos1, geVec3d Pos2,float diameter);
  void Tick(float dwTicks);
};

#endif