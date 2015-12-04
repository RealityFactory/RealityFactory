/************************************************************************************//**
 * @file CMorphingFields.h
 * @brief Morphing Fields class handler
 *
 * This file contains the class declaration for morphing fields handling.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_CMORPHINGFIELDS_H_
#define __RGF_CMORPHINGFIELDS_H_

/**
 * @brief CMorphingFields handles MorphingField entities
 */
class CMorphingFields : public CRGFComponent
{
public:
	CMorphingFields();
	~CMorphingFields();

	void Tick(geFloat dwTicks);				///< Do any animation

	int BindToPath(const char *szName);		///< Bind entity to motion path

	int SaveTo(FILE *SaveFD);				///< Save state to a file

	int RestoreFrom(FILE *RestoreFD);		///< Restore state from a file

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

private:
	int m_MorphingFieldCount;				///< Count of morphing fields
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
