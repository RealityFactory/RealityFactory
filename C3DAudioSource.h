/************************************************************************************//**
 * @file C3DAudioSource.h
 * @brief 3D Audio Source class handler
 *
 * This file contains the class declaration for 3D audio source handling.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_C3DAUDIOSOURCE_H_
#define __RGF_C3DAUDIOSOURCE_H_

/**
 * @brief 3D Audio Source class handler
 *
 * Manage all 3DAudioSource entities in a level
 */
class C3DAudioSource : public CRGFComponent
{
public:
	/**
	 * @brief Constructor
	 *
	 * Load up all 3D audio sources, load and initialize sounds and set the
	 * entities to default values.
	 */
	C3DAudioSource();

	/**
	 * @brief Destructor
	 *
	 * Clean up all audio we have loaded.
	 */
	~C3DAudioSource();

	/**
	 * @brief Update 3DAudioSource entities for current frame
	 */
	void Tick(geFloat dwTicks);

// Start Aug2003DCS
	/**
	 * @brief Given a name, locate the desired entity in the currently loaded
	 * level and set its ProgrammedTrigger boolean.
	 */
	int SetProgrammedTrigger(const char *szName, geBoolean Flag);

	/**
	 * @brief Given a name, locate the desired entity in the currently loaded
	 * level and return true if it is playing, false if not.
	 */
	geBoolean IsPlaying(const char *szName);
// End Aug2003DCS

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
	/**
	 * @brief Create 3D sound effect
	 */
	int Create(const geVec3d &Origin, const char *SoundFile, float radius, geBoolean Looping);

private:

	int m_EntityCount;		///< Count of 3D Audio Sources
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
