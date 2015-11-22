/*

    CShake.h               Screen Shake handler

	(c) 1999 Ralph Deane

	This file contains the class declarations for the 
	Screen Shake enitity  for RGF-based games.
*/

#ifndef __RGF_SHAKE_H_
#define __RGF_SHAKE_H_

class CShake : public CRGFComponent
{
public:
  CShake();
  ~CShake();	
  void Tick(geFloat dwTicks);
private:
};

#endif
