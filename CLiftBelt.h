/************************************************************************************//**
 * @file CLiftBelt.h
 * @author Ralph Deane
 * @brief CLifBelt class
 *//*
 * Copyright (c) Ralph Deane; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_CLIFTBELT_H_
#define __RGF_CLIFTBELT_H_

/**
 * CLifBelt handles LiftBelt entities
 */
class CLiftBelt : public CRGFComponent
{
public:
	CLiftBelt();
	~CLiftBelt();

	void Tick(geFloat dwTicks);
	void ChangeLift(bool increase);
	void DisableHud(const char *Attr);
	int	LocateEntity(const char *szName, void **pEntityData);

private:
	bool Change;
	bool Increase;
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
