/************************************************************************************//**
 * @file CVideoTexture.h
 * @brief Video surface texture handler
 *
 * This file contains the class declaration for the CVideoTexture class that
 * encapsulates video surface texture handling in the RGF.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_CVideoTexture_H__
#define __RGF_CVideoTexture_H__

/**
 * @brief CVideoTexture handles VideoTextureReplacer entities
 */
class CVideoTexture : public CRGFComponent
{
public:
	CVideoTexture();				///< Constructor
	~CVideoTexture();				///< Destructor

	void Tick(geFloat dwTick);		///< Update video textures

	/**
	 * @brief Correct internal timing to match current time, to make up for time
	 * lost when outside the game loop (typically in "menu mode").
	 */
	int ReSynchronize();

private:
	int m_TextureCount;				///< Count of VideoTextureReplacers in world
	CAVIPlayer *m_VidList[40];		///< Up to 40 concurrent video textures
	CAnimGif *m_GifList[40];
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
