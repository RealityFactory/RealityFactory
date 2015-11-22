/*
		CCDAudio.h:		CD Audio Playback Class

		(c) 1999 Edward A. Averill, III

		This file contains the class declaration for the CCDAudio
CD Audio playback class.
*/

#ifndef __RGF_CCDAUDIO_H_
#define __RGF_CCDAUDIO_H_

#include "RabidFramework.h"

class CCDAudio : public CRGFComponent
{
public:
  CCDAudio();								// Sets up MCI for CD Audio work
  ~CCDAudio();											// Clean up
	int Play(int nTrack, bool bLoop);	// Play a single track with or w/o looping
	int Stop();												// Stop CD Audio playback
  bool IsPlaying();									// See if the CD is playing
	int PlaySequence(int nCount, int *nTrackList);	// Play sequence of tracks
	void Check();							// Called to handle CD state processing
	int SaveTo(FILE *SaveFD);	// Save state to a file
	int RestoreFrom(FILE *RestoreFD);		// Restore state from a file
	bool GetCdOn()
	{ return m_cdon; }
	void SetCdOn(bool state)
	{ m_cdon = state; }
	int Restore()
	{ return Play(m_savetrack, m_saveloop); }
private:
  int m_PlayCount;					// Count of tracks to play
  int m_PlayList[99];				// Up to 99 tracks sequenced for playback
	bool m_bLooping;					// Current track looping?
	bool m_bActive;						// CD Audio device open and ready
	int m_mciDeviceID;				// Device ID from open
	int m_nCurrentTrack;			// CD track we're currently playing
	int m_nPlayIndex;					// Current track in playlist, if list active
	bool m_saveloop;
	int m_savetrack;
	bool m_cdon;
};

#endif

// --------------------------------------------------------------------------
