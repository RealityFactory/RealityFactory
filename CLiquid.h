/*
	CLiquid.h	:	Liquid

	(c) 2001 Ralph Deane

	This file contains the class declaration for the CLiquid
class.  This class implements Liquid
*/

#include "RabidFramework.h"

#ifndef __RGF_CLIQUID_H__
#define __RGF_CLIQUID_H__

class CLiquid : public CRGFComponent
{
public:
  CLiquid();
  ~CLiquid();
  Liquid *IsLiquid(geWorld_Model *theModel);

private:

};


class COverlay : public CRGFComponent
{
public:
  COverlay();
  ~COverlay();
  void Tick(float dwTicks);
  Overlay *IsOverlay(geWorld_Model *theModel);

private:

};

#endif