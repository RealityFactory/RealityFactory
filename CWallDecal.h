/****************************************************************************************/
/*																						*/
/*	CWallDecal.h:		Decal class handler												*/
/*																						*/
/*	(c) 2000 Ralph Deane																*/
/*	All Rights Reserved																	*/
/*																						*/
/*	This file contains the class declaration for WallDecal handling.					*/
/*																						*/
/****************************************************************************************/


#ifndef __RGF_CWALLDECAL_H_
#define __RGF_CWALLDECAL_H_

class CWallDecal : public CRGFComponent
{
public:
	CWallDecal();
	~CWallDecal();

	void AddDecal(WallDecal *pSource);
// changed RF064
	void Tick(float dwTicks);
// Start Aug2003DCS
	geFloat DotProductAngle(geVec3d *V1, geVec3d *V2);
	void AddDecal(WallDecal *pSource, geVec3d *InVec, geVec3d *RightVec);
	int  SetProgrammedTrigger(char *szName, geBoolean Flag);
	int  SetCurrentBitmap(char *szName, int CurrentBitmap);
// End Aug2003DCS
private:

};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */


