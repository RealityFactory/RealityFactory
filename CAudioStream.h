/************************************************************************************//**
 * @file CAudioStream.h
 * @brief Streaming audio handler class
 *
 * This file contains the class declaration for the CAudioStream class that
 * handles streaming large WAVE files from mass storage into the Genesis3D
 * sound system.
 * @note This class depends on the 11/08/1999 Genesis3D engine mod to the file
 * Sound.c that exposes a function call to get the DirectSound object that the
 * Genesis3D engine is using. If you don't have this mod, this won't operate.
 * @author Ralph Deane
 *//*
 * Copyright(c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_CAUDIOSTREAM_H__
#define __RGF_CAUDIOSTREAM_H__

#include "StreamingAudio.h"

/**
 * @brief CAudioStream aggregates StreamingAudio objects.
 */
class CAudioStream : public CRGFComponent
{
public:
	CAudioStream();
	~CAudioStream();

	int Play(const char *szFilename, bool fLoopIt, bool fProxy);///< Stream a file
	int Pause(const char *szFilename);							///< Pause/unpause a stream
	int Stop(const char *szFilename);							///< Shut down & delete a stream
	void PauseAll();
	void StopAll();

	bool IsPlaying(const char *szFilename);						///< Is file streaming?
	int ReSynchronize();
	void SetVolume(LONG nVolume);
	void Tick(geFloat dwTicks);									///< Time-based action

private:

	int FindFreeSlot();						///< Find free entry in table
	int FindInList(const char *szFile);		///< Find streaming file in list
	void Sweep();							///< Clean up any stopped streams

private:

	int				m_nStreamerCount;		///< Count of streaming audio entities
	char			*m_FileList[32];		///< Up to 32 files can stream
	StreamingAudio	*m_Streams[32];			///< Up to 32 streaming audio objects
	LPDIRECTSOUND	m_dsPtr;				///< Pointer to Genesis3D DirectSound object
	int				m_LoopingProxy;			///< ONLY ONE ALLOWED!
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
