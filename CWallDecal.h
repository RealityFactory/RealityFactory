/*
	CWallDecal.h:		Decal class handler

	(c) 2000 Ralph Deane
	All Rights Reserved

	This file contains the class declaration for WallDecal
handling.
*/

#ifndef __RGF_CWALLDECAL_H_
#define __RGF_CWALLDECAL_H_

class CWallDecal : public CRGFComponent
{
public:
  CWallDecal();
  ~CWallDecal();
  void AddDecal(WallDecal *pSource);
// changed RF064
  void Tick(float dwTicks);
private:

};

#endif


