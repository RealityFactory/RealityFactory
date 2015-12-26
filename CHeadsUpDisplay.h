/************************************************************************************//**
 * @file CHeadsUpDisplay.h
 * @brief HUD display handler
 *
 * This file contains the class declaration for the CHeadsUpDisplay class that
 * encapsulates player status display functionality.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

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
	RADAR,
	PICTURE

} HUDTYPE;

typedef enum
{
	NONE = 0,
	CLIP,
	MAG
};

/**
 * @brief HUD element
 */
struct HUDEntry
{
	bool		active;
	char		szAttributeName[64];	///< Attribute name to display
	geBitmap	*Identifier;			///< Identifier bitmap
	geBitmap	*Indicator;				///< Indicator (level) bitmap
	geBitmap	*Indicator2;
	int			nTop, nLeft;			///< Top/left point of HUD display element
	int			iTopOffset, iLeftOffset;
	int			iHeight;
	geFloat		PixelsPerIncrement;		///< # of pixels per unit of measure
	HUDTYPE		Type;
	char		format[16];
	bool		display;
	bool		modify;
	int			direction;
	float		DisplayTime;
	float		CurrentTime;
	int			OldAmount;
	int			Style;
	bool		flipindicator;			///< flip the indicator scaling
	CAnimGif	*GifData;
};

/**
 * @brief CHeadsUpDisplay handles player status display
 */
class CHeadsUpDisplay : public CRGFComponent
{
public:
	CHeadsUpDisplay();							///< Default constructor
	~CHeadsUpDisplay();							///< Default destructor

	int LoadConfiguration();					///< Load configuration from PlayerSetup entity

	int Activate();								///< Turn HUD on

	int Deactivate();							///< Turn HUD off

	int RemoveElement(const char *szAttributeName);	///< Remove element from HUD
	int ActivateElement(const char *szAttributeName, bool activate);

	int Render();								///< Render the HUD out

	void Tick(geFloat dwTick);
	bool GetActive() { return m_bHUDActive; }
	int SetElementLeftTop(const char *szAttributeName, int nLeft, int nTop);
	int SetElementILeftTop(const char *szAttributeName, int iLeftOffset, int iTopOffset);
	int SetElementDisplayTime(const char *szAttributeName, float DisplayTime);

private:
	bool m_bHUDActive;							///< HUD displayed flag
	HUDEntry m_theHUD[MAXHUD];					///< HUD display array, up to 100 items
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
