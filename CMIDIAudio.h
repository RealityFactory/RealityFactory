/************************************************************************************//**
 * @file CMIDIAudio.h
 * @brief MIDI Audio Playback Class
 *
 * This file contains the class declaration for the CMIDIAudio MIDI file
 * playback class.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_CMIDIAUDIO_H_
#define __RGF_CMIDIAUDIO_H_

#include "RabidFramework.h"

class CMIDIAudio : public CRGFComponent
{
public:
	CMIDIAudio();								// Sets up MCI for MIDI playback
	~CMIDIAudio();								// Clean up

	int Play(char *szFile, bool bLoop);			// Play a MIDI file with or w/o looping
	void Restore();
	int PlayList(char **szList, int nCount);	// Play a list of MIDI files
	int Stop();									// Stop MIDI playback
	bool IsPlaying();							// See if MIDI is playing
	void Check();								// Called to handle MIDI state processing

	int SaveTo(FILE *SaveFD);					// Save MIDI state to file
	int RestoreFrom(FILE *RestoreFD);			// Restore MIDI state from file

private:
	bool	m_bLooping;			// Current track looping?
	bool	m_bActive;			// MIDI device open and ready
	int		m_mciDeviceID;		// Device ID from open
	int		m_ListCount;		// MIDI file playlist size
	char	**m_szList;			// List of MIDI files to play
	int		m_Position;			// Current playlist position
	char	m_MIDIFile[256];	// If single file, current one playing
	char	m_savefile[256];
	bool	m_saveloop;
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
