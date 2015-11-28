/************************************************************************************//**
 * @file CFixedCamera.h
 * @brief Fixed Camera class handler
 *
 * This file contains the class declaration for Fixed Camera handling.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2000 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_CFIXEDCAMERA_H_
#define __RGF_CFIXEDCAMERA_H_

/**
 * @brief CFixedCamera handles FixedCamera entities
 */
class CFixedCamera : public CRGFComponent
{
public:
	CFixedCamera();
	~CFixedCamera();

	void Tick();
	bool GetFirstCamera();
	int GetNumber()				{ return Number; }
	FixedCamera *GetCamera()	{ return Camera; }

	void SetPosition(const geVec3d &Position);	///< Set position of the currently active fixed camera
	void Move(const geVec3d &Move);				///< Move the currently active fixed camera
	void SetRotation(const geVec3d &Rotation);	///< Set rotation of the currently active fixed camera
	void Rotate(const geVec3d &Rotate);			///< Rotate the currently active fixed camera
	void SetFOV(float FOV);						///< Set field of view of the currently active fixed camera

private:
	bool CheckFieldofView(FixedCamera *pSource);
private:
	int Number;
	FixedCamera *Camera;	///< Currently active fixed camera, NULL if none
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
