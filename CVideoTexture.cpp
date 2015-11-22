/*
CVideoTexture.cpp:		

  (c) 2001 Ralph Deane
  
	This file contains the class implementation for the CVideoTexture
	class that encapsulates video texture replacement handling in the RGF.
*/

//	You only need the one, master include file.

#include "RabidFramework.h"

//	CVideoTexture
//
//	Default constructor.  Set up for texture replacement by scanning
//	..for texture replacement indicators.

CVideoTexture::CVideoTexture()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	m_TextureCount = 0;					// No video textures
	
	for(int nTemp = 0; nTemp < 40; nTemp++)
	{
		m_VidList[nTemp] = NULL;	// Nothing doing right now
		m_GifList[nTemp] = NULL;
	}
	
	//	Ok, check to see if there are video textures in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "VideoTextureReplacer");
	
	if(!pSet) 
		return;									// Not on this level.
	
	//	Ok, we have video textures somewhere.  Dig through 'em all.
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		VideoTextureReplacer *pTex = (VideoTextureReplacer*)geEntity_GetUserData(pEntity);
		m_TextureCount++;							// Kick door count
		pTex->Playing = GE_FALSE;
		if(pTex->Radius == 0.0)
			pTex->Playing = GE_TRUE;		// Video playing
		// Regardless of whether or not this is a ranged video texture,
		// ..we're going to initialize it and get the first frame
		// ..loaded.  This prevents a short (but noticable!) delay from
		// ..happening if we need to initialize the texture later.
		CString File = pTex->szVideoName;
		File.MakeLower();
		int i = File.Find(".gif");
		if(i>=0 && i<File.GetLength())
		{
			pTex->Gif = true;
			if((m_GifList[m_TextureCount-1] = new CAnimGif(pTex->szVideoName, kVideoFile)) == NULL)
			{
				CCD->ReportError("Can't create new CAnimGif", false);
				continue;
			}
			m_GifList[m_TextureCount-1]->DisplayNextFrameTexture(pTex->szTextureName, true);
		}
		else
		{
			pTex->Gif = false;
			if((m_VidList[m_TextureCount-1] = new CAVIPlayer()) == NULL)
			{
				CCD->ReportError("Can't create new CAVIPlayer", false);
				continue;
			}
			if(m_VidList[m_TextureCount-1]->Open(pTex->szVideoName) != RGF_SUCCESS)
			{
				char szBug[256];
				sprintf(szBug,"Can't open video texture '%s'", pTex->szVideoName);
				CCD->ReportError(szBug, false);
				delete m_VidList[m_TextureCount-1];
				m_VidList[m_TextureCount-1] = NULL;
				continue;
			}
			// Ok, the thing's open, initialize it
			m_VidList[m_TextureCount-1]->DisplayNextFrameTexture(pTex->szTextureName, true);
			// Initialize texture playback
		}
	}
	
	//	Ok, we've counted the texture replacers and reset 'em all to their 
	// ..default values.  Leave!
	
	return;
}

//	~CVideoTexture
//
//	Default destructor.  Shut down all playing videos.

CVideoTexture::~CVideoTexture()
{
	for(int nTemp = 0; nTemp < 40; nTemp++)
	{
		if(m_VidList[nTemp] != NULL)
		{
			m_VidList[nTemp]->Close();
			delete m_VidList[nTemp];
			m_VidList[nTemp] = NULL;
		}
		if(m_GifList[nTemp] != NULL)
		{
			delete m_GifList[nTemp];
			m_GifList[nTemp] = NULL;
		}
	}
	
	return;
}

//	Tick
//
//	Play the next frame of video for all playing videos, as well
//	..as check for the player coming into the trigger radius for
//	..a triggered video texture.  Note that, due to CPU performance
//	..constraints, this routine won't check for updates any more
//	..often than once every <n> milliseconds.

void CVideoTexture::Tick(geFloat dwTicks)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	int nSlot = 0;
	
	//	Ok, check to see if there are video textures in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "VideoTextureReplacer");
	
	if(!pSet) 
		return;									// Not on this level.
	
	//	Ok, we have video textures somewhere.  Dig through 'em all.
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		VideoTextureReplacer *pTex = (VideoTextureReplacer*)geEntity_GetUserData(pEntity);
		if(pTex->Radius == 0.0 || (pTex->OnlyPlayInRadius == GE_FALSE))
		{
			if(!pTex->Gif)
				// Play next frame of running video texture
				m_VidList[nSlot]->DisplayNextFrameTexture(pTex->szTextureName, false);
			else
			{
				m_GifList[nSlot]->DisplayNextFrameTexture(pTex->szTextureName, false);
			}
		}
		else
		{
			// This is a RANGE TRIGGERED video. Check to see if we're
			// ..in range before doing anything else
			if(geVec3d_DistanceBetween(&pTex->origin, &CCD->Player()->Position()) >
				pTex->Radius)
			{
				nSlot++;
				continue;
			}
			pTex->Playing = GE_TRUE;			// Playback triggered
			// Ok, we're in range!  If we don't have the video allocated,
			// ..do so now.
			if(!pTex->Gif)
			{
				if(m_VidList[nSlot] == NULL)
				{
					// Set up for texture playback
					if((m_VidList[nSlot] = new CAVIPlayer()) == NULL)
					{
						CCD->ReportError("vidtex start: can't make new CAVIPlayer", false);
						return;
					}
					if(m_VidList[nSlot]->Open(pTex->szVideoName) != RGF_SUCCESS)
					{
						char szBug[256];
						sprintf(szBug,"Can't open range texture '%s'", pTex->szVideoName);
						CCD->ReportError(szBug, false);
						return;
					}
					// Feh, after all that, let's do something REAL!
					m_VidList[nSlot]->DisplayNextFrameTexture(pTex->szTextureName, true);		// First frame out!
					nSlot++;
					continue;					// Next victim
				}
				// Ok, we're in range and this isn't the first frame,
				// ..play the next frame
				m_VidList[nSlot]->DisplayNextFrameTexture(pTex->szTextureName, false);		// Next frame out!
			}
			else
			{
				if(m_GifList[nSlot] == NULL)
				{
					if((m_GifList[nSlot] = new CAnimGif(pTex->szVideoName, kVideoFile)) == NULL)
					{
						CCD->ReportError("Can't create new CAnimGif", false);
						return;
					}
					m_GifList[nSlot]->DisplayNextFrameTexture(pTex->szTextureName, true);	
					nSlot++;
					continue;
				}
				m_GifList[nSlot]->DisplayNextFrameTexture(pTex->szTextureName, false);
			}
		}
		nSlot++;
	}
	
	//	Whew, all done!  Let's get outta here...
	
	return;
}

//	ReSynchronize
//
//	Correct internal timing to match current time, to make up for time lost
//	..when outside the game loop (typically in "menu mode").

int CVideoTexture::ReSynchronize()
{
	
	return RGF_SUCCESS;
}
