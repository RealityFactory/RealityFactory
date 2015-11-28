/*
CHeadsUpDisplay.h:		HUD display handler

  (c) 2001 Ralph Deane
  
	This file contains the class declaration for the CHeadsUpDisplay
	class that encapsulates player status display functionality.
*/

#ifndef __RGF_CHEADSUPDISPLAY_H_
#define __RGF_CHEADSUPDISPLAY_H_

#define MAXHUD   100

typedef enum 
{
	VERT = 0,
		HORIZ,
		VALUE,
		PPOS,
		COMPASS,
// changed RF063
		RADAR
} HUDTYPE;
// changed RF064
typedef enum 
{
	NONE = 0,
	CLIP,
	MAG
};
// end change RF064
struct HUDEntry
{
	bool active;
	char szAttributeName[64];		// Attribute name to display
	geBitmap *Identifier;				// Identifier bitmap
	geBitmap *Indicator;				// Indicator (level) bitmap
// changed RF063
	geBitmap *Indicator2;
	int nTop, nLeft;						// Top/left point of HUD display element
	int iTopOffset, iLeftOffset;
	int iHeight;
	geFloat PixelsPerIncrement;		// # of pixels per unit of measure
	HUDTYPE Type;
	char format[16];
	bool display;
	bool modify;
	int direction;
	float DisplayTime;
	float CurrentTime;
	int OldAmount;
// change RF064
	int Style;
// end change RF064
};

class CHeadsUpDisplay : public CRGFComponent
{
public:
	CHeadsUpDisplay();		// Default constructor
	~CHeadsUpDisplay();														// Default destructor
	int LoadConfiguration();		
	// Load configuration from PlayerSetup entity
	int Activate();							// Turn HUD on
	int Deactivate();						// Turn HUD off
	int RemoveElement(char *szAttributeName);		// Remove element from HUD
	int ActivateElement(char *szAttributeName, bool activate);
	int Render();	// Render the HUD out
	void Tick(geFloat dwTick);
	bool GetActive()
	{ return m_bHUDActive; }
private:
	bool m_bHUDActive;						// HUD displayed flag
	HUDEntry m_theHUD[MAXHUD];				// HUD display array, up to 12 items
};

#endif
