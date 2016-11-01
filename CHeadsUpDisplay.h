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
	PICTURE,
	UNDEFINED
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
	bool		display;
	bool		modify;
	bool		flipindicator;			///< flip the indicator scaling
	CAnimGif*	GifData;
	geBitmap*	Identifier;				///< Identifier bitmap
	geBitmap*	Indicator;				///< Indicator (level) bitmap
	geBitmap*	Indicator2;
	int			nTop, nLeft;			///< Top/left point of HUD display element
	int			iTopOffset, iLeftOffset;
	int			iHeight;
	float		PixelsPerIncrement;		///< # of pixels per unit of measure
	HUDTYPE		Type;
	char		format[16];
	int			direction;
	float		DisplayTime;
	float		CurrentTime;
	int			OldAmount;
	int			Style;
	std::string	AttributeName;			///< Attribute name to display
	std::string	font;					///< font to use to display text

	HUDEntry() :
		active(false),
		display(false),
		modify(false),
		flipindicator(false),
		GifData(NULL),
		Identifier(NULL),
		Indicator(NULL),
		Indicator2(NULL),
		nTop(0),nLeft(0),
		iTopOffset(0),iLeftOffset(0),
		iHeight(0),
		PixelsPerIncrement(0.f),
		Type(UNDEFINED),
		direction(0),
		DisplayTime(0.f),
		CurrentTime(0.f),
		OldAmount(0),
		Style(0)
	{ format[0] = 0; }

	void clear()
	{
		active = false;
		display = false;
		modify = false;
		flipindicator = false;
		SAFE_DELETE(GifData);
		Identifier = NULL;
		Indicator = NULL;
		Indicator2 = NULL;
		nTop = 0; nLeft = 0;
		iTopOffset = 0; iLeftOffset = 0;
		iHeight = 0;
		PixelsPerIncrement = 0.f;
		Type = UNDEFINED;
		format[0] = 0;
		direction = 0;
		DisplayTime = 0.f;
		CurrentTime = 0.f;
		OldAmount = 0;
		Style = 0;
		AttributeName.clear();
		font.clear();
	}
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

	int RemoveElement(const std::string& attributeName);	///< Remove element from HUD

	int ActivateElement(const std::string& attributeName, bool activate);

	int Render();								///< Render the HUD out

	void Tick(float dwTick);

	bool GetActive() const { return m_bHUDActive; }

	int SetElementLeftTop(const std::string& attributeName, int nLeft, int nTop);

	int SetElementILeftTop(const std::string& attributeName, int iLeftOffset, int iTopOffset);

	int SetElementDisplayTime(const std::string& attributeName, float displayTime);

private:
	bool m_bHUDActive;							///< HUD displayed flag
	HUDEntry m_theHUD[MAXHUD];					///< HUD display array, up to 100 items
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
