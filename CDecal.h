/*
	CDecal.h:		Decal class handler

	(c) 2001 Ralph Deane
	All Rights Reserved

	This file contains the class declaration for Decal
handling.
*/

#ifndef __RGF_CDECAL_H_
#define __RGF_CDECAL_H_

typedef struct	Decal
{
  Decal    	*next;
  Decal    	*prev;
  float		TimeToLive;
  geBitmap	*Bitmap;
  GE_LVertex	vertex[4];
  long		Leaf;
} Decal;


class CDecal : public CRGFComponent
{
public:
  CDecal();
  ~CDecal();
  void Tick(float dwTicks);
  void AddDecal(int type, geVec3d *impact, geVec3d *normal);
private:

  Decal *Bottom;
};

#endif


