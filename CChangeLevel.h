/*
	CChangeLevel.h:		Changelevel class handler

	(c) 1999 Edward A. Averill, III
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
  bool CheckChangeLevel(geWorld_Model *theModel);
  int LocateEntity(char *szName, void **pEntityData);
};

#endif

// --------------------------------------------------------------------------
