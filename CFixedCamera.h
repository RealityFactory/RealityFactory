/*
	CFixedCamera.h:		Fixed Camera class handler

	(c) 2000 Ralph Deane
	All Rights Reserved

	This file contains the class declaration for Fixed Camera
handling.
*/

#ifndef __RGF_CFIXEDCAMERA_H_
#define __RGF_CFIXEDCAMERA_H_

class CFixedCamera : public CRGFComponent
{
public:
  CFixedCamera();
  ~CFixedCamera();
  void Tick();
  bool GetFirstCamera();
  int GetNumber()
  { return Number; }
  FixedCamera *GetCamera()
  { return Camera; }
private:
	bool CheckFieldofView(FixedCamera *pSource);
	int Number;
	FixedCamera *Camera;
};

#endif
