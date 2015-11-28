/************************************************************************************//**
 * @file CViewSwitch.h
 * @brief ViewSwitch class handler
 *
 * This file contains the class declaration for ViewSwitch handling.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2000 Ralph Deane; All rights reserved.
 ****************************************************************************************/

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

/* ----------------------------------- END OF FILE ------------------------------------ */
