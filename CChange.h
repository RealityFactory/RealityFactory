/************************************************************************************//**
 * @file CChange.h
 * @brief ChangeAttribute class handler
 *
 * This file contains the class declaration for ChangeAttribute	handling.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2002 Ralph Deane; All rights reserved
 ****************************************************************************************/

#ifndef __RGF_CCHANGE_H_
#define __RGF_CCHANGE_H_

class CChangeAttribute : public CRGFComponent
{
public:
	CChangeAttribute();
	~CChangeAttribute();

	void Tick(float dwTicks);
	int SaveTo(FILE *SaveFD, bool type);
	int RestoreFrom(FILE *RestoreFD, bool type);
	bool GetActive()  { return Active; }

private:

	bool Active;
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
