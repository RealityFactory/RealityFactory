/*
	CChangeLevel.h:		Changelevel class handler

	(c) 2001 Ralph Deane
	All Rights Reserved

	This file contains the class declaration for ChangeLevel
handling.
*/

#ifndef __RGF_CCHANGELEVEL_H_
#define __RGF_CCHANGELEVEL_H_

class CChangeLevel : public CRGFComponent
{
public:
  CChangeLevel();
  ~CChangeLevel();
  void Tick(float dwTicks);
  bool CheckChangeLevel(geWorld_Model *theModel, bool UseKey);
  int LocateEntity(char *szName, void **pEntityData);
};

#endif

// --------------------------------------------------------------------------
