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
class CMIDIAudio;

/**
 * @brief CSoundtrackToggle handles SoundtrackToggle entities
 *
 * A soundtrack toggle switches between two possible soundtracks (either MIDI
 * or CD Audio) or can switch from a MIDI soundtrack to a CD Audio soundtrack,
 * based on how many times the player has passed through it. This is a cheap
 * but effective way to vary soundtrack playback during a game without having
 * to resort to special programming.
 */
class CSoundtrackToggle : public CRGFComponent
{
public:
	CSoundtrackToggle();			///< Default constructor
	~CSoundtrackToggle();			///< Default destructor

	void Tick(geFloat dwTicks);		///< Handle time-based action, if any

	/**
	 * @brief Correct internal timing to match current time, to make up for time
	 * lost when outside the game loop (typically in "menu mode").
	 */
	int ReSynchronize();

	void SetVolume(LONG nVolume);

private:
	void StopStreaming();

private:
	int m_EntityCount;	///< Count of toggle entities
	CMIDIAudio *m_MIDIPlayer;
	StreamingAudio *m_Streams;
	LPDIRECTSOUND m_dsPtr;
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
