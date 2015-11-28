/****************************************************************************************/
/*																						*/
/*	CVideoTexture.h:		Video surface texture handler								*/
/*																						*/
/*	(c) 2001 Ralph Deane																*/
/*																						*/
/*	This file contains the class declaration for the CVideoTexture						*/
/*	class that encapsulates video surface texture handling in the RGF.					*/
/*																						*/
/****************************************************************************************/


#include "RabidFramework.h"

#ifndef __RGF_CVideoTexture_H__
#define __RGF_CVideoTexture_H__


class CVideoTexture : public CRGFComponent
{
public:
	CVideoTexture();				// Constructor
	~CVideoTexture();				// Destructor

	void Tick(geFloat dwTick);		// Update video textures
	int ReSynchronize();

private:
	int m_TextureCount;				// Count of doors in world
	CAVIPlayer *m_VidList[40];		// Up to 40 concurrent video textures
	CAnimGif *m_GifList[40];
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
