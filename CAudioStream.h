/*
	CAudioStream.h:		Steaming audio handler class

	(c) 2001 Ralph Deane

	This file contains the class declaration for the CAudioStream
class that handles streaming large WAVE files from mass storage
into the Genesis3D sound system.  Note that this class depends on
the 11/08/1999 Genesis3D engine mod to the file Sound.c that exposes
a function call to get the DirectSound object that the Genesis3D
engine is using.  If you don't have this mod, this won't operate.
*/

#ifndef __RGF_CAUDIOSTREAM_H__
#define __RGF_CAUDIOSTREAM_H__

//	CAudioStream aggregates StreamingAudio objects.

#include "StreamingAudio.h"

class CAudioStream : public CRGFComponent
{
public:
  CAudioStream();
	~CAudioStream();
	int Play(char *szFilename, bool fLoopIt, bool fProxy);
																			// Stream a file
	bool IsPlaying(char *szFilename);		// Is file streaming?
	int Pause(char *szFilename);				// Pause/unpause a stream
	int Stop(char *szFilename);					// Shut down & delete a stream
	void Tick(geFloat dwTicks);	// Time-based action
	int ReSynchronize();
	void SetVolume(LONG nVolume);
	void StopAll();
	void PauseAll();
private:
  int FindFreeSlot();								// Find free entry in table
	int FindInList(char *szFile);			// Find streaming file in list
	void Sweep();											// Clean up any stopped streams
  int m_nStreamerCount;							// Count of streaming audio entities
	char *m_FileList[32];							// Up to 32 files can stream
	StreamingAudio *m_Streams[32];		// Up to 32 streaming audio objects
	LPDIRECTSOUND m_dsPtr;						// Pointer to Genesis3D DirectSound object
	int m_LoopingProxy;								// ONLY ONE ALLOWED!
};

#endif
