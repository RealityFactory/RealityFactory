/*
	C3DAudioSource.h:		3D Audio Source class handler

	(c) 2001 Ralph Deane
	All Rights Reserved

	This file contains the class declaration for 3D audio source
handling.
*/

#ifndef __RGF_C3DAUDIOSOURCE_H_
#define __RGF_C3DAUDIOSOURCE_H_

class C3DAudioSource : public CRGFComponent
{
public:
  C3DAudioSource();
  ~C3DAudioSource();
  void Tick(float dwTicks);
  int LocateEntity(char *szName, void **pEntityData);
  int ReSynchronize();
private:
  int Create(geVec3d Origin, char *SoundFile, float radius);
  int Count;		// Count of 3D audio sources
};

#endif

// --------------------------------------------------------------------------
