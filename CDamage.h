/************************************************************************************//**
 * @file CDamage.h
 * @brief Damage class handler
 *
 * This file contains the class declaration for damage handling.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_CDAMAGE_H_
#define __RGF_CDAMAGE_H_

/**
 * @brief CDamage class handles DestroyableModel entities and damage generation
 */
class CDamage : public CRGFComponent
{
public:
	CDamage();
	~CDamage();

	void Tick(geFloat dwTicks);
	int SaveTo(FILE *SaveFD, bool type);
	int RestoreFrom(FILE *RestoreFD, bool type);

	void DamageActor(const geActor *Actor,
							float amount, const char *Attr,
							float Altamount, const char *AltAttr, const char *name);
	void DamageActorInRange(geVec3d Point, geFloat Range,
							float amount, const char *Attr,
							float Altamount, const char *AltAttr, const char *name);

	void DamageModel(const geWorld_Model *Model,
							float amount, const char *Attr,
							float Altamount, const char *AltAttr);
	void DamageModelInRange(geVec3d Point, geFloat Range,
							float amount, const char *Attr,
							float Altamount, const char *AltAttr);

	bool IsDestroyable(geWorld_Model *Model, int *Percentage);


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
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
