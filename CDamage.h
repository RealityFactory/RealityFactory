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

	void Tick(float timeElapsed);

	int SaveTo(FILE* saveFD, bool type);

	int RestoreFrom(FILE* restoreFD, bool type);

	void DamageActor(const geActor* actor,
							float amount, const std::string& attr,
							float altAmount, const std::string& altAttr, const std::string& name);

	void DamageActorInRange(geVec3d point, float range,
							float amount, const std::string& attr,
							float altAmount, const std::string& altAttr, const std::string& name);

	void DamageModel(const geWorld_Model* model,
							float amount, const std::string& attr,
							float altAmount, const std::string& altAttr);

	void DamageModelInRange(geVec3d point, float range,
							float amount, const std::string& attr,
							float altAmount, const std::string& altAttr);

	bool IsDestroyable(geWorld_Model* model, int* percentage);

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
