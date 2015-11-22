/*
	CViewSwitch.h:		ViewSwitch class handler

	(c) 2000 Ralph Deane
	All Rights Reserved

	This file contains the class declaration for ViewSwitch
handling.
*/

#ifndef __RGF_CVIEWSWITCH_H_
#define __RGF_CVIEWSWITCH_H_

class CViewSwitch : public CRGFComponent
{
public:
  CViewSwitch();
  ~CViewSwitch();
  void Tick();

private:
  int OldView;
  bool ViewActive;
  ViewSwitch *pViewSwitch;
};

#endif
