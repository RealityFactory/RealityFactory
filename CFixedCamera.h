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

class CFixedCamera : public CRGFComponent
{
public:
	CFixedCamera();
	~CFixedCamera();

	void Tick();
	bool GetFirstCamera();
	int GetNumber()				{ return Number; }
	FixedCamera *GetCamera()	{ return Camera; }

	void SetPosition(const geVec3d &Position);
	void Move(const geVec3d &Move);
	void SetRotation(const geVec3d &Rotation);
	void Rotate(const geVec3d &Rotate);
	void SetFOV(float FOV);

private:
	bool CheckFieldofView(FixedCamera *pSource);
private:
	int Number;
	FixedCamera *Camera;
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
