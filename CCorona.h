/*
	CCorona.h:		

	(c) 2001 Ralph Deane
	All Rights Reserved

	This file contains the class declaration for Corona
handling.
*/

#ifndef __RGF_CCORONA_H_
#define __RGF_CCORONA_H_


class CCorona : public CRGFComponent
{
public:
  CCorona();
  ~CCorona();
  void Tick(float dwTicks);
  int LocateEntity(char *szName, void **pEntityData);
  int ReSynchronize();
private:
  int Create(geVec3d Origin, Corona *pCorona);
  int Count;
  geBitmap	*CoronaBitmap;
};

#endif

// --------------------------------------------------------------------------
