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

	/**
	 * @brief Given a name, locate the desired entity in the currently loaded
	 * level and return its user data.
	 */
	int	LocateEntity(const char *szName, void **pEntityData);

private:
	bool m_Change;
	bool m_Increase;
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
