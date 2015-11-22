/*
CHeadsUpDisplay.h:		HUD display handler

  (c) 1999 Edward A. Averill, III
  
	This file contains the class declaration for the CHeadsUpDisplay
	class that encapsulates player status display functionality.
*/

#ifndef __RGF_CHEADSUPDISPLAY_H_
#define __RGF_CHEADSUPDISPLAY_H_


typedef enum 
{
	VERT = 0,
		HORIZ,
		VALUE,
		PPOS,
		COMPASS
} HUDTYPE;

struct HUDEntry
{
	bool active;
	char szAttributeName[64];		// Attribute name to display
	geBitmap *Identifier;				// Identifier bitmap
	geBitmap *Indicator;				// Indicator (level) bitmap
	int nTop, nLeft;						// Top/left point of HUD display element
	int iTopOffset, iLeftOffset;
	int iHeight;
	geFloat PixelsPerIncrement;		// # of pixels per unit of measure
	HUDTYPE Type;
	char format[16];
	bool display;
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
	int Render();	
	// Render the HUD out
private:
	bool m_bHUDActive;						// HUD displayed flag
	HUDEntry m_theHUD[12];				// HUD display array, up to 12 items
};

#endif
