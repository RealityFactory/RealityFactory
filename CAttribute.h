/************************************************************************************//**
 * @file CAttribute.h
 * @brief Attribute class handler
 *
 * This file contains the class declaration for Attribute handling.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_CATTRIBUTE_H_
#define __RGF_CATTRIBUTE_H_

/**
 * @brief CAttribute handles all Attribute ModifyAttribute entities
 */
class CAttribute : public CRGFComponent
{
public:
	CAttribute();
	~CAttribute();

	void Tick(geFloat dwTicks);

	bool HandleCollision(geActor *theTarget, geActor *pActor, bool UseKey);

	int SaveTo(FILE *SaveFD, bool type);

	int RestoreFrom(FILE *RestoreFD, bool type);

	/**
	 * @brief Given a name, locate the desired entity in the currently loaded
	 * level and return its user data.
	 */
	int LocateEntity(const char *szName, void **pEntityData);

	/**
	 * @brief Correct internal timing to match current time, to make up for time
	 * lost when outside the game loop (typically in "menu mode").
	 */
	int ReSynchronize();

	void AddAttributeEntity(Attribute *pAttribute);

private:
	bool InitAttribute(Attribute *pAttribute);

private:
	int m_DynamicAttributes;
	geEntity *m_DynamicAttribute1;
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
