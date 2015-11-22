

#ifndef __RGF_CSPOUT_H_
#define __RGF_CSPOUT_H_

#pragma warning( disable : 4068 )

#include "genesis.h"

// class declaration for CSpout
class CSpout : public CRGFComponent
{
public:
  CSpout();
  ~CSpout();	
  int Create(Spout *S);
  void Tick(float dwTicks);
  int LocateEntity(char *szName, void **pEntityData);
  int ReSynchronize();
private:
};


#endif

