/*
	CSountrackToggle.h:		Soundtrack Toggle Entity Handler

	(c) 2001 Ralph Deane

	This file contains the class declaration for the Soundtrack Toggle
Entity handling class.  A soundtrack toggle switches between two
possible soundtracks (either MIDI or CD Audio) or can switch from
a MIDI soundtrack to a CD Audio soundtrack, based on how many times
the player has passed through it.  This is a cheap but effective way
to vary soundtrack playback during a game without having to resort
to special programming.
*/

#ifndef __RGF_CSOUNDTRACKTOGGLE_H_
#define __RGF_CSOUNDTRACKTOGGLE_H_

class CAudioStream;

class CSoundtrackToggle : public CRGFComponent
{
public:
  CSoundtrackToggle();		// Default constructor
	~CSoundtrackToggle();		// Default destructor
  void Tick(geFloat dwTicks);						// Handle time-based action, if any
	int ReSynchronize();
private:
	void CSoundtrackToggle::StopStreaming();
  int m_SoundtrackToggleCount;					// Count of toggle tntities
};

#endif

// --------------------------------------------------------------------------
