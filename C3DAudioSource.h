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

class C3DAudioSource : public CRGFComponent
{
public:
	C3DAudioSource();
	~C3DAudioSource();

	void Tick(float dwTicks);
// Start Aug2003DCS
	int SetProgrammedTrigger(char *szName, geBoolean Flag);
	geBoolean IsPlaying(char *szName);
// End Aug2003DCS
	int LocateEntity(char *szName, void **pEntityData);
	int ReSynchronize();

private:
// Start Aug2003DCS
	int Create(const geVec3d &Origin, char *SoundFile, float radius, geBoolean Looping);
// End Aug2003DCS
	int Count;		// Count of 3D audio sources
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
