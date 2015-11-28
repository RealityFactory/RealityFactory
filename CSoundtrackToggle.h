/************************************************************************************//**
 * @file CSoundtrackToggle.h
 * @brief Soundtrack Toggle Entity Handler
 *
 * This file contains the class declaration for the Soundtrack Toggle Entity
 * handling class.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_CSOUNDTRACKTOGGLE_H_
#define __RGF_CSOUNDTRACKTOGGLE_H_

class CAudioStream;

class CSoundtrackToggle : public CRGFComponent
{
public:
	CSoundtrackToggle();			// Default constructor
	~CSoundtrackToggle();			// Default destructor

	void Tick(geFloat dwTicks);		// Handle time-based action, if any
	int ReSynchronize();
	void SetVolume(LONG nVolume);

private:
	void StopStreaming();

private:
	int m_SoundtrackToggleCount;	// Count of toggle tntities
	CMIDIAudio *theMIDIPlayer;
	StreamingAudio *m_Streams;
	LPDIRECTSOUND m_dsPtr;
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
