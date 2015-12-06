/************************************************************************************//**
 * @file CHeadsUpDisplay.cpp
 * @brief HUD display handler
 *
 * This file contains the class implementation for the CHeadsUpDisplay class that
 * encapsulates player status display functionality.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

// Include the One True Header
#include "RabidFramework.h"

// changed RF064
extern geBitmap *TPool_Bitmap(const char *DefaultBmp, const char *DefaultAlpha,
							  const char *BName, const char *AName);
// end change RF064

#define ZDEPTH11 1.1f
#define ZDEPTH1  1.0f

/* ------------------------------------------------------------------------------------ */
//	Constructor
//
//	Set up the HUD
/* ------------------------------------------------------------------------------------ */
CHeadsUpDisplay::CHeadsUpDisplay()
{
	m_bHUDActive = true;					// HUD not displayed

	// Clear the HUD array
	for(int nTemp=0; nTemp<MAXHUD; ++nTemp)
	{
		memset(&m_theHUD[nTemp], 0, sizeof(HUDEntry));
		m_theHUD[nTemp].active = false;
	}
}

/* ------------------------------------------------------------------------------------ */
//	Destructor
//
//	Release bitmaps, etc.
/* ------------------------------------------------------------------------------------ */
CHeadsUpDisplay::~CHeadsUpDisplay()
{
	// Clean up all the HUD bitmaps
	for(int nTemp=0; nTemp<MAXHUD; ++nTemp)
	{
// changed Nout 12/15/05
		if(m_theHUD[nTemp].GifData)
			delete m_theHUD[nTemp].GifData;
// end change

		memset(&m_theHUD[nTemp], 0, sizeof(HUDEntry));
	}
}

/* ------------------------------------------------------------------------------------ */
//	LoadConfiguration
//
//	Loads the HUD configuration from the HUD configuration file
//	..defined in the PlayerSetup entity.
/* ------------------------------------------------------------------------------------ */
int CHeadsUpDisplay::LoadConfiguration()
{
	char				*HudInfo;

	//	Ok, check to see if there's a PlayerSetup around...
	if(CCD->MenuManager()->GetUseSelect() && !EffectC_IsStringNull(CCD->MenuManager()->GetCurrentHud()))
	{
		HudInfo = CCD->MenuManager()->GetCurrentHud();
	}
	else
	{
		geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "PlayerSetup");

		if(!pSet)
			return RGF_FAILURE;									// No setup?

		// Ok, get the setup information.  There should only be one, so
		// ..we'll just take the first one we run into.
		geEntity *pEntity = geEntity_EntitySetGetNextEntity(pSet, NULL);

		PlayerSetup *pSetup = static_cast<PlayerSetup*>(geEntity_GetUserData(pEntity));

		if(EffectC_IsStringNull(pSetup->HUDInfoFile))
			return RGF_FAILURE;								// No HUD initialization?

		HudInfo = pSetup->HUDInfoFile;
	}

	CIniFile AttrFile(HudInfo);

	if(!AttrFile.ReadFile())
	{
		char szAttrError[256];
		sprintf(szAttrError, "[ERROR] File %s - Line %d: Failed to open HUD config file '%s'",
				__FILE__, __LINE__, HudInfo);
		CCD->ReportError(szAttrError, false);
		return RGF_FAILURE;
	}

	// Something is there, so let's clean up all the HUD bitmaps to prepare
	for(int nTemp=0; nTemp<MAXHUD; ++nTemp)
	{
// Note QD: TPool handles these bitmaps
/*
		if(m_theHUD[nTemp].Identifier != NULL)
			geBitmap_Destroy(&m_theHUD[nTemp].Identifier);

		if(m_theHUD[nTemp].Indicator != NULL)
			geBitmap_Destroy(&m_theHUD[nTemp].Indicator);

// changed RF063
		if(m_theHUD[nTemp].Indicator2 != NULL)
			geBitmap_Destroy(&m_theHUD[nTemp].Indicator2);
// end change RF063
*/
// changed Nout 12/15/05
		if(m_theHUD[nTemp].GifData)
			delete m_theHUD[nTemp].GifData;
// end change

		memset(&m_theHUD[nTemp], 0, sizeof(HUDEntry));
	}

	std::string KeyName = AttrFile.FindFirstKey();
	char szAttr[64], szType[64], szName[64], szAlpha[64];
	std::string Tname, Talpha;
// changed RF063
	geBitmap *TempBmp1, *TempBmp2, *TempBmp3;
	bool valid, active, modify, flipindicator; // changed QD 07/15/06
	int nTop, nLeft, iTopOffset, iLeftOffset, Height, direction;
// changed RF064
	int Style;
// end change RF064
	HUDTYPE Type;
	float Font, DisplayTime;
	char format[16];
// changed Nout 12/15/05
	CAnimGif *GifAnim;
// end change

	while(KeyName != "")
	{
// changed RF063
		if(KeyName[0] == '~')
			strcpy(szType, KeyName.substr(1).c_str());
		else
			strcpy(szType, KeyName.c_str());
// end change RF063

		valid = false;
		active = true;
		modify = false;
// changed QD 07/15/06
		flipindicator = false;
// end change
// changed RF064
		Style = NONE;
// end change RF064
		direction = 0;
		TempBmp1 = NULL;
		TempBmp2 = NULL;
// changed RF063
		TempBmp3 = NULL;
		Font = 0.0f;
		Height = 0;
		nTop = nLeft = iTopOffset = iLeftOffset = 0;
		format[0] = '\0';
// changed Nout 12/15/05
		GifAnim = NULL;

		if(!strnicmp(szType, "picture", 6))
		{
			Type = PICTURE;
			strcpy(szAttr, szType);
			valid = true;
			Tname = AttrFile.GetValue(KeyName, "bitmap");

			if(Tname != "")
			{
				Talpha = AttrFile.GetValue(KeyName, "bitmapalpha");

				if(Talpha == "")
					Talpha = Tname;

				strcpy(szName, Tname.c_str());
				strcpy(szAlpha, Talpha.c_str());
				TempBmp2 = TPool_Bitmap(szName, szAlpha, NULL, NULL);
			}
			else
			{
				Tname = AttrFile.GetValue(KeyName, "giffile");
				strcpy(szName, Tname.c_str());
				GifAnim = new CAnimGif(szName, kVideoFile);
			}
		}
// end changed
		else if(!stricmp(szType, "compass"))
		{
			Type = COMPASS;
			strcpy(szAttr, "compass");
			valid = true;
			Tname = AttrFile.GetValue(KeyName, "indicator");

			if(Tname != "")
			{
				Talpha = AttrFile.GetValue(KeyName, "indicatoralpha");

				if(Talpha == "")
					Talpha = Tname;

				strcpy(szName, Tname.c_str());
				strcpy(szAlpha, Talpha.c_str());
// changed RF064
				TempBmp2 = TPool_Bitmap(szName, szAlpha, NULL, NULL);
// end change RF064
			}
		}
// changed RF063
		else if(!stricmp(szType, "radar"))
		{
			Type = RADAR;
			strcpy(szAttr, "radar");
			valid = true;
			Tname = AttrFile.GetValue(KeyName, "indicator");

			if(Tname != "")
			{
				Talpha = AttrFile.GetValue(KeyName, "indicatoralpha");

				if(Talpha == "")
					Talpha = Tname;

				strcpy(szName, Tname.c_str());
				strcpy(szAlpha, Talpha.c_str());
// changed RF064
				TempBmp2 = TPool_Bitmap(szName, szAlpha, NULL, NULL);
// end change RF064
			}

			Tname = AttrFile.GetValue(KeyName, "npcindicator");

			if(Tname != "")
			{
				Talpha = AttrFile.GetValue(KeyName, "npcindicatoralpha");

				if(Talpha == "")
					Talpha = Tname;

				strcpy(szName, Tname.c_str());
				strcpy(szAlpha, Talpha.c_str());
// changed RF064
				TempBmp3 = TPool_Bitmap(szName, szAlpha, NULL, NULL);
// end change RF064

				if(!TempBmp3)
					TempBmp3 = TempBmp2;
			}
			else
				TempBmp3 = TempBmp2;

			Font = (float)AttrFile.GetValueI(KeyName, "range");
		}
// end change RF063
		else if(!stricmp(szType, "position"))
		{
			Type = PPOS;
			strcpy(szAttr, "position");
			valid = true;
			Font = (float)AttrFile.GetValueI(KeyName, "font");
			int width = AttrFile.GetValueI(KeyName, "width");

			if(width <= 0)
				width = 4;

			char buf[17];
			itoa(width, buf, 10);

			strcpy(format, "%");
			strcat(format, buf);
			strcat(format, ".0f %");
			strcat(format, buf);
			strcat(format, ".0f %");
			strcat(format, buf);
			strcat(format, ".0f");
		}
		else
		{
// changed RF063
			strcpy(szAttr, szType);
// end change RF063
			Tname = AttrFile.GetValue(KeyName, "type");

			if(Tname != "")
			{
				if(Tname == "verticle")
				{
					valid = true;
					Type = VERT;
					Height = AttrFile.GetValueI(KeyName, "indicatorheight");
				}
				if(Tname == "horizontal")
				{
					valid = true;
					Type = HORIZ;
					Height = AttrFile.GetValueI(KeyName, "indicatorwidth");
				}
				if(Tname == "numeric")
				{
					valid = true;
					Type = VALUE;
					Font = (float)AttrFile.GetValueI(KeyName, "font");
					int width = AttrFile.GetValueI(KeyName, "width");

					if(width <= 0)
						width = 3;

					char buf[17];
					itoa(width, buf, 10);

					strcpy(format, "%");
					strcat(format, buf);
					strcat(format, ".1d");
				}

				if(valid)
				{
// changed RF064
					Tname = AttrFile.GetValue(KeyName, "style");

					if(Tname == "clip")
						Style = CLIP;

					if(Tname == "magazine")
						Style = MAG;
// end change RF064

					if(Type != VALUE)
					{
						Tname = AttrFile.GetValue(KeyName, "indicator");

						if(Tname != "")
						{
							Talpha = AttrFile.GetValue(KeyName, "indicatoralpha");

							if(Talpha == "")
								Talpha = Tname;

							strcpy(szName, Tname.c_str());
							strcpy(szAlpha, Talpha.c_str());
// changed RF064
							TempBmp2 = TPool_Bitmap(szName, szAlpha, NULL, NULL);
// end change RF064
						}

// changed QD 07/15/06
						if(AttrFile.GetValue(KeyName, "flipindicator") == "true")
						{
							flipindicator = true;
						}
// end change QD 07/15/06
					}
				}
			}
		}

		if(valid)
		{
			int nItem;

			Tname = AttrFile.GetValue(KeyName, "frame");

			if(Tname != "")
			{
				Talpha = AttrFile.GetValue(KeyName, "framealpha");

				if(Talpha == "")
					Talpha = Tname;

				strcpy(szName, Tname.c_str());
				strcpy(szAlpha, Talpha.c_str());

// changed RF064
				TempBmp1 = TPool_Bitmap(szName, szAlpha, NULL, NULL);
// end change RF064
			}

			if(AttrFile.GetValue(KeyName, "framex") == "center")
			{
				if(TempBmp1)
					nLeft = geBitmap_Width(TempBmp1)/2;

				nLeft = (CCD->Engine()->Width()/2)-nLeft;
			}
			else
			{
				nLeft = AttrFile.GetValueI(KeyName, "framex");

				if(nLeft < 0)
					nLeft = CCD->Engine()->Width() + nLeft;
			}

			if(AttrFile.GetValue(KeyName, "framey") == "center")
			{
				if(TempBmp1)
					nTop = geBitmap_Height(TempBmp1)/2;

				nTop = (CCD->Engine()->Height()/2)-nTop;
			}
			else
			{
				nTop = AttrFile.GetValueI(KeyName, "framey");

				if(nTop < 0)
					nTop = CCD->Engine()->Height() + nTop;
			}

			iLeftOffset = AttrFile.GetValueI(KeyName, "indicatoroffsetx");
			iTopOffset = AttrFile.GetValueI(KeyName, "indicatoroffsety");


			if(AttrFile.GetValue(KeyName, "active") == "false")
				active = false;

			if(AttrFile.GetValue(KeyName, "active") == "modify")
			{
				active = false;
				modify = true;
			}

			DisplayTime = static_cast<float>(AttrFile.GetValueF(KeyName, "displaytime"));

			if(AttrFile.GetValue(KeyName, "modifydirection") == "down")
				direction = 1;

			if(AttrFile.GetValue(KeyName, "modifydirection") == "both")
				direction = 2;

			for(nItem=0; nItem<MAXHUD; ++nItem)
			{
				if(!m_theHUD[nItem].active)
					break;
			}

			if(nItem < MAXHUD)
			{
				m_theHUD[nItem].active				= true;
				m_theHUD[nItem].display				= active;
				m_theHUD[nItem].modify				= modify;
				m_theHUD[nItem].OldAmount			= -99999;
				m_theHUD[nItem].direction			= direction;
				m_theHUD[nItem].Type				= Type;
				m_theHUD[nItem].DisplayTime			= DisplayTime;
				m_theHUD[nItem].Identifier			= TempBmp1;
				m_theHUD[nItem].Indicator			= TempBmp2;
// changed RF063
				m_theHUD[nItem].Indicator2			= TempBmp3;
				strcpy(m_theHUD[nItem].szAttributeName, szAttr);
				strcpy(m_theHUD[nItem].format, format);
				m_theHUD[nItem].nTop				= nTop;
				m_theHUD[nItem].nLeft				= nLeft;
				m_theHUD[nItem].iTopOffset			= iTopOffset;
				m_theHUD[nItem].iLeftOffset			= iLeftOffset;
				m_theHUD[nItem].PixelsPerIncrement	= Font;
				m_theHUD[nItem].iHeight				= Height;
// changed RF064
				m_theHUD[nItem].Style				= Style;
// end change RF064
// changed QD 07/15/06
				m_theHUD[nItem].flipindicator		= flipindicator;
// end change QD 07/15/06
// changed Nout 12/15/05
				m_theHUD[nItem].GifData				= GifAnim;
// end change
			}
		}

		KeyName = AttrFile.FindNextKey();
	}

	//Activate();		// Turn the HUD ON!
	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	Activate
//
//	Set up the HUD as renderable.
/* ------------------------------------------------------------------------------------ */
int CHeadsUpDisplay::Activate()
{
	m_bHUDActive = true;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	Deactivate
//
//	Flag the HUD as non-renderable.
/* ------------------------------------------------------------------------------------ */
int CHeadsUpDisplay::Deactivate()
{
	m_bHUDActive = false;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	RemoveElement
//
//	Remove the named element from the HUD display list.
/* ------------------------------------------------------------------------------------ */
int CHeadsUpDisplay::RemoveElement(const char *szAttributeName)
{
	for(int nItem=0; nItem<MAXHUD; ++nItem)
	{
		if(!strcmp(szAttributeName, m_theHUD[nItem].szAttributeName))
		{
// changed QD 12/15/05 TPool handles these bitmaps
/*			// It's this attribute, wipe it out.
			if(m_theHUD[nItem].Identifier)
				geBitmap_Destroy(&m_theHUD[nItem].Identifier);

			if(m_theHUD[nItem].Indicator)
				geBitmap_Destroy(&m_theHUD[nItem].Indicator);
*/
// end change
// changed Nout 12/15/05
			if(m_theHUD[nItem].GifData)
				delete m_theHUD[nItem].GifData;
// end change

			memset(&m_theHUD[nItem], 0, sizeof(HUDEntry));
			return RGF_SUCCESS;					// It's outta here
		}
	}

	//	Ooops, HUD attribute not found.
	return RGF_FAILURE;
}

/* ------------------------------------------------------------------------------------ */
//	ActivateElement
/* ------------------------------------------------------------------------------------ */
int CHeadsUpDisplay::ActivateElement(const char *szAttributeName, bool activate)
{
// changed RF063
	bool flag = false;

	for(int nItem=0; nItem<MAXHUD; ++nItem)
	{
		if(!strcmp(szAttributeName, m_theHUD[nItem].szAttributeName))
		{
			m_theHUD[nItem].display = activate;
			flag = true;					// It's outta here
		}
	}

	if(flag)
		return RGF_SUCCESS;
// end change RF063

	//	Ooops, HUD attribute not found.
	return RGF_FAILURE;
}

// changed Nout 12/15/05
/* ------------------------------------------------------------------------------------ */
//	SetElementLeftTop
/* ------------------------------------------------------------------------------------ */
int CHeadsUpDisplay::SetElementLeftTop(const char *szAttributeName, int nLeft, int nTop)
{
	for(int nItem = 0; nItem < MAXHUD; ++nItem)
	{
		if(!strcmp(szAttributeName, m_theHUD[nItem].szAttributeName))
		{
			m_theHUD[nItem].nLeft = nLeft;
			m_theHUD[nItem].nTop = nTop;
			return RGF_SUCCESS;
		}
	}

	return RGF_FAILURE;
}

/* ------------------------------------------------------------------------------------ */
//	SetElementILeftTop
/* ------------------------------------------------------------------------------------ */
int CHeadsUpDisplay::SetElementILeftTop(const char *szAttributeName, int iLeftOffset, int iTopOffset)
{
	for(int nItem = 0; nItem < MAXHUD; ++nItem)
	{
		if(!strcmp(szAttributeName, m_theHUD[nItem].szAttributeName))
		{
			m_theHUD[nItem].iLeftOffset = iLeftOffset;
			m_theHUD[nItem].iTopOffset = iTopOffset;
			return RGF_SUCCESS;
		}
	}

	return RGF_FAILURE;
}

/* ------------------------------------------------------------------------------------ */
//	SetElementDisplayTime
/* ------------------------------------------------------------------------------------ */
int CHeadsUpDisplay::SetElementDisplayTime(const char *szAttributeName, float DisplayTime)
{
	for(int nItem = 0; nItem < MAXHUD; ++nItem)
	{
		if(!strcmp(szAttributeName, m_theHUD[nItem].szAttributeName))
		{
			m_theHUD[nItem].DisplayTime = DisplayTime;

			if(DisplayTime == 0.0f)
				m_theHUD[nItem].modify = false;
			else
				m_theHUD[nItem].modify = true;

			return RGF_SUCCESS;
		}
	}

	return RGF_FAILURE;
}
// end change


// changed RF063
/* ------------------------------------------------------------------------------------ */
//	Tick
/* ------------------------------------------------------------------------------------ */
void CHeadsUpDisplay::Tick(geFloat dwTick)
{
	for(int nItem=0; nItem<MAXHUD; ++nItem)
	{
		if(!m_theHUD[nItem].modify)
			continue;

		if(m_theHUD[nItem].Type == VERT || m_theHUD[nItem].Type == HORIZ || m_theHUD[nItem].Type == VALUE)
		{
			CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
			int HudValue = theInv->Value(m_theHUD[nItem].szAttributeName);

			if(!strcmp(m_theHUD[nItem].szAttributeName, "LightValue"))
				HudValue = CCD->Player()->LightValue();

			if(m_theHUD[nItem].OldAmount == -99999)
			{
				m_theHUD[nItem].OldAmount = HudValue;
			}
			else
			{
				if(m_theHUD[nItem].OldAmount != HudValue)
				{
					bool flag = false;

					if(m_theHUD[nItem].direction == 0 && (m_theHUD[nItem].OldAmount < theInv->Value(m_theHUD[nItem].szAttributeName)))
						flag = true;
					else if(m_theHUD[nItem].direction == 1 && (m_theHUD[nItem].OldAmount > HudValue))
						flag = true;
					else if(m_theHUD[nItem].direction == 2 && (m_theHUD[nItem].OldAmount != HudValue))
						flag = true;

					if(flag)
					{
						m_theHUD[nItem].CurrentTime = 0.0f;
						m_theHUD[nItem].display = true;
					}

					m_theHUD[nItem].OldAmount = HudValue;
				}

				if(m_theHUD[nItem].display)
				{
					m_theHUD[nItem].CurrentTime += dwTick*0.001f;

					if(m_theHUD[nItem].CurrentTime >= m_theHUD[nItem].DisplayTime)
					{
						m_theHUD[nItem].display = false;
					}
				}
			}
		}
// changed Nout 12/15/05
		else if(m_theHUD[nItem].Type == PICTURE)
		{
			if(m_theHUD[nItem].display)
			{
				m_theHUD[nItem].CurrentTime += dwTick*0.001f;

				if(m_theHUD[nItem].CurrentTime >= m_theHUD[nItem].DisplayTime)
				{
					m_theHUD[nItem].display = false;
				}
			}
		}
// end change
	}
}
// end change RF063

/* ------------------------------------------------------------------------------------ */
//	Render
//
//	If the HUD is active, render it to the screen.
/* ------------------------------------------------------------------------------------ */
int CHeadsUpDisplay::Render()
{
	geRect theRect;
	int nValue, nLow, nHigh;
	int nItem;

// changed Nout 12/15/05
	// changed Nout 12/15/05
	for(nItem = 0; nItem < MAXHUD; ++nItem)
	{
		if(!m_theHUD[nItem].active)
			continue;								// No item in slot

		if(!m_theHUD[nItem].display)
			continue;

		if(m_theHUD[nItem].Type == PICTURE)
		{
			theRect.Top = 0;
			theRect.Bottom = m_theHUD[nItem].iTopOffset;
			theRect.Left = 0;
			theRect.Right =  m_theHUD[nItem].iLeftOffset;
			if(m_theHUD[nItem].Indicator) //a bitmap was defined
			{
				if((m_theHUD[nItem].iLeftOffset > 0) && (m_theHUD[nItem].iTopOffset))
					CCD->Engine()->DrawBitmap(m_theHUD[nItem].Indicator, &theRect,
						m_theHUD[nItem].nLeft,
						m_theHUD[nItem].nTop, ZDEPTH11);
				else
					CCD->Engine()->DrawBitmap(m_theHUD[nItem].Indicator, NULL,
						m_theHUD[nItem].nLeft,
						m_theHUD[nItem].nTop, ZDEPTH1);
			}
			else if(m_theHUD[nItem].GifData)
			{
				geBitmap *theBmp = m_theHUD[nItem].GifData->NextFrame(true);

				if((m_theHUD[nItem].iLeftOffset > 0) && (m_theHUD[nItem].iTopOffset))
					CCD->MenuManager()->DrawBitmap(theBmp, &theRect, m_theHUD[nItem].nLeft, m_theHUD[nItem].nTop);
				else
					CCD->MenuManager()->DrawBitmap(theBmp, NULL, m_theHUD[nItem].nLeft, m_theHUD[nItem].nTop);
			}
		}
	}

	if(!m_bHUDActive)
		return RGF_SUCCESS;				// Ignore the call
// end change

	// Iterate through the HUD element list, displaying each place there
	// ..is an entry in the slot.
	for(nItem=0; nItem<MAXHUD; ++nItem)
	{
		if(!m_theHUD[nItem].active)
			continue;								// No item in slot

		if(!m_theHUD[nItem].display)
			continue;

		switch(m_theHUD[nItem].Type)
		{
		case VERT:	// Verticle Indicator
			{
				// Draw the identifier icon
				if(m_theHUD[nItem].Identifier)
				{
// changed RF064
					CCD->Engine()->DrawBitmap(m_theHUD[nItem].Identifier,
												NULL,
												m_theHUD[nItem].nLeft,
												m_theHUD[nItem].nTop,
												ZDEPTH11);
// end change RF064
				}

				CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());

// changed RF064
				if(!theInv->Has(m_theHUD[nItem].szAttributeName))
					continue;
// end change RF064

				nValue = theInv->Value(m_theHUD[nItem].szAttributeName);

// changed RF063
				nHigh = theInv->High(m_theHUD[nItem].szAttributeName);
				nLow = theInv->Low(m_theHUD[nItem].szAttributeName);
// changed RF064

				if(m_theHUD[nItem].Style != NONE)
				{
					if(!strcmp(m_theHUD[nItem].szAttributeName, CCD->Weapons()->GetWeaponAmmo()))
					{
						int magsize = CCD->Weapons()->GetMagSize();

						if(magsize == 0)
							continue;

						if(m_theHUD[nItem].Style == MAG)
						{
							nValue = CCD->Weapons()->GetMagAmt() - CCD->Weapons()->GetShotFired();
							nHigh = magsize;
							nLow = 0;
						}
						else
						{
							nValue -= (CCD->Weapons()->GetMagAmt() - CCD->Weapons()->GetShotFired());
							nValue += (magsize-1);
							nValue /= magsize;

							nHigh += (magsize-1);
							nHigh /= magsize;
							nLow /= magsize;
						}
					}
					else
					{
						if(m_theHUD[nItem].Style == MAG)
							continue;
					}
				}
// end change RF064

				if(!strcmp(m_theHUD[nItem].szAttributeName, "LightValue"))
				{
					nValue = CCD->Player()->LightValue();
					nLow = 0;
					nHigh = CCD->Player()->LightLife();
				}

				m_theHUD[nItem].PixelsPerIncrement = static_cast<float>(m_theHUD[nItem].iHeight) / static_cast<float>(nHigh - nLow);

				// Compute the rectangle to draw for the level indicator
				if(m_theHUD[nItem].Indicator)
				{
					if(m_theHUD[nItem].flipindicator)
					{
						theRect.Top = 0;
						theRect.Bottom = static_cast<int>(m_theHUD[nItem].PixelsPerIncrement * nValue) - 1;
					}
					else
					{
						theRect.Top = m_theHUD[nItem].iHeight - (static_cast<int>(m_theHUD[nItem].PixelsPerIncrement * nValue) - 1);
						theRect.Bottom = m_theHUD[nItem].iHeight;
					}

					theRect.Left = 0;
					theRect.Right =  geBitmap_Width(m_theHUD[nItem].Indicator);

// changed RF064
					CCD->Engine()->DrawBitmap(m_theHUD[nItem].Indicator, &theRect,
						m_theHUD[nItem].nLeft+m_theHUD[nItem].iLeftOffset,
						m_theHUD[nItem].nTop+m_theHUD[nItem].iTopOffset+theRect.Top, ZDEPTH1);
// end change RF064
				}

				break;
			}
		case HORIZ:	// Horizontal Indicator
			{
				// Draw the identifier icon
				if(m_theHUD[nItem].Identifier)
				{
// changed RF064
					CCD->Engine()->DrawBitmap(m_theHUD[nItem].Identifier,
												NULL,
												m_theHUD[nItem].nLeft,
												m_theHUD[nItem].nTop,
												ZDEPTH11);
// end change RF064
				}

				CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());

// changed RF064
				if(!theInv->Has(m_theHUD[nItem].szAttributeName))
					continue;
// end change RF064

				nValue = theInv->Value(m_theHUD[nItem].szAttributeName);

// changed RF063
				nHigh = theInv->High(m_theHUD[nItem].szAttributeName);
				nLow = theInv->Low(m_theHUD[nItem].szAttributeName);

// changed RF064
				if(m_theHUD[nItem].Style!=NONE)
				{
					if(!strcmp(m_theHUD[nItem].szAttributeName, CCD->Weapons()->GetWeaponAmmo()))
					{
						int magsize = CCD->Weapons()->GetMagSize();

						if(magsize == 0)
							continue;

						if(m_theHUD[nItem].Style == MAG)
						{
							nValue = CCD->Weapons()->GetMagAmt() - CCD->Weapons()->GetShotFired();
							nHigh = magsize;
							nLow = 0;
						}
						else
						{
							nValue -= (CCD->Weapons()->GetMagAmt() - CCD->Weapons()->GetShotFired());
							nValue += (magsize-1);
							nValue /= magsize;

							nHigh += (magsize-1);
							nHigh /= magsize;
							nLow /= magsize;
						}
					}
					else
					{
						if(m_theHUD[nItem].Style == MAG)
							continue;
					}
				}
// end change RF064

				if(!strcmp(m_theHUD[nItem].szAttributeName, "LightValue"))
				{
					nValue = CCD->Player()->LightValue();
					nLow = 0;
					nHigh = CCD->Player()->LightLife();
				}

				m_theHUD[nItem].PixelsPerIncrement = static_cast<float>(m_theHUD[nItem].iHeight) / static_cast<float>(nHigh - nLow);

				// Compute the rectangle to draw for the level indicator
				if(m_theHUD[nItem].Indicator)
				{
					theRect.Top = 0;
					theRect.Bottom = geBitmap_Height(m_theHUD[nItem].Indicator);

					if(m_theHUD[nItem].flipindicator)
					{
						theRect.Left = m_theHUD[nItem].iHeight - (static_cast<int>(m_theHUD[nItem].PixelsPerIncrement * nValue) - 1);
						theRect.Right = m_theHUD[nItem].iHeight;
					}
					else
					{
						theRect.Left = 0;
						theRect.Right = static_cast<int>((m_theHUD[nItem].PixelsPerIncrement * nValue)) - 1;
					}

					CCD->Engine()->DrawBitmap(m_theHUD[nItem].Indicator, &theRect,
						m_theHUD[nItem].nLeft+m_theHUD[nItem].iLeftOffset+theRect.Left,
						m_theHUD[nItem].nTop+m_theHUD[nItem].iTopOffset, ZDEPTH1);
				}

				break;
			}
		case VALUE:	// numeric indicator
			{
				// Draw the identifier icon
				if(m_theHUD[nItem].Identifier)
				{
// changed RF064
					CCD->Engine()->DrawBitmap(m_theHUD[nItem].Identifier,
											NULL,
											m_theHUD[nItem].nLeft,
											m_theHUD[nItem].nTop,
											ZDEPTH11);
// end change RF064
				}

				CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());

// changed RF064
				if(!theInv->Has(m_theHUD[nItem].szAttributeName))
					continue;
// end change RF064

				nValue = theInv->Value(m_theHUD[nItem].szAttributeName);

// changed RF063
// changed RF064
				if(m_theHUD[nItem].Style != NONE)
				{
					if(!strcmp(m_theHUD[nItem].szAttributeName, CCD->Weapons()->GetWeaponAmmo()))
					{
						int magsize = CCD->Weapons()->GetMagSize();

						if(magsize == 0)
							continue;

						if(m_theHUD[nItem].Style==MAG)
						{
							nValue = CCD->Weapons()->GetMagAmt() - CCD->Weapons()->GetShotFired();
						}
						else
						{
							nValue -= (CCD->Weapons()->GetMagAmt() - CCD->Weapons()->GetShotFired());
							nValue += (magsize-1);
							nValue /= magsize;
						}
					}
					else
					{
						if(m_theHUD[nItem].Style == MAG)
							continue;
					}
				}
// end change RF064

				if(!strcmp(m_theHUD[nItem].szAttributeName, "LightValue"))
				{
					nValue = CCD->Player()->LightValue();
				}
// end change RF063

				char szBug[256];
				sprintf(szBug, m_theHUD[nItem].format, nValue);
				CCD->MenuManager()->WorldFontRect(szBug, (int)m_theHUD[nItem].PixelsPerIncrement, m_theHUD[nItem].nLeft+m_theHUD[nItem].iLeftOffset, m_theHUD[nItem].nTop+m_theHUD[nItem].iTopOffset, 255.0f);

				break;
			}
		case PPOS:	// player position
			{
				// Draw the identifier icon
				if(m_theHUD[nItem].Identifier)
				{
// changed RF064
					CCD->Engine()->DrawBitmap(m_theHUD[nItem].Identifier,
												NULL,
												m_theHUD[nItem].nLeft,
												m_theHUD[nItem].nTop,
												ZDEPTH11);
// end change RF064
				}

				char szBug[256];
				geVec3d Pos = CCD->Player()->Position();
				sprintf(szBug, m_theHUD[nItem].format, Pos.X, Pos.Y, Pos.Z);
				CCD->MenuManager()->WorldFontRect(szBug, (int)m_theHUD[nItem].PixelsPerIncrement, m_theHUD[nItem].nLeft+m_theHUD[nItem].iLeftOffset, m_theHUD[nItem].nTop+m_theHUD[nItem].iTopOffset, 255.0f);

				break;
			}
// changed RF063

		case RADAR:	// Radar
			{
				geActor *ActorsInRange[512];
				geVec3d Pos, RPos, Orient;
				Pos = CCD->Player()->Position();
				int nActorCount = CCD->ActorManager()->GetActorsInRange(Pos,
									m_theHUD[nItem].PixelsPerIncrement, 512, &ActorsInRange[0]);

				if(nActorCount != 0 && m_theHUD[nItem].Indicator)
				{
					float scale = (static_cast<float>(geBitmap_Height(m_theHUD[nItem].Identifier)) * 0.5f) / m_theHUD[nItem].PixelsPerIncrement;
					CCD->ActorManager()->GetRotate(CCD->Player()->GetActor(), &RPos);

					while(RPos.Y < 0.0f)
					{
						// changed QD 12/15/05
						RPos.Y += GE_2PI; //(GE_PI*2);
					}

					// changed QD 12/15/05
					while(RPos.Y > GE_2PI) //(GE_PI*2))
					{
						// changed QD 12/15/05
						RPos.Y -= GE_2PI; //(GE_PI*2);
					}

					for(int nTemp=0; nTemp<nActorCount; nTemp++)
					{
						if(ActorsInRange[nTemp]!=CCD->Player()->GetActor())
						{
							geVec3d APos;
							int ActorType;
							bool flag = false;
							int offx, offy;
// changed RF064
							if(CCD->ActorManager()->GetType(ActorsInRange[nTemp], &ActorType)!=RGF_NOT_FOUND)
							{
								bool hide;
								CCD->ActorManager()->GetHideRadar(ActorsInRange[nTemp], &hide);
								if(!hide)
								{
									if(ActorType==ENTITY_NPC || ActorType==ENTITY_VEHICLE)
										flag = true;

									CCD->ActorManager()->GetPosition(ActorsInRange[nTemp], &APos);
									// changed QD 12/15/05 - can't be negative (sqrt!)
									// float dist = ((float)fabs(geVec3d_DistanceBetween(&Pos, &APos)))*scale;
									float dist = geVec3d_DistanceBetween(&Pos, &APos)*scale;
									geVec3d_Subtract(&APos, &Pos, &Orient);
									Orient.Y = (float)atan2(Orient.X, Orient.Z) + GE_PI;

									float angle = RPos.Y - Orient.Y;
									float mulx = -1.0f;
									float muly = 1.0f;

									if(!flag)
									{
										offx = m_theHUD[nItem].nLeft+(geBitmap_Width(m_theHUD[nItem].Identifier)/2)-(geBitmap_Width(m_theHUD[nItem].Indicator)/2);
										offy = m_theHUD[nItem].nTop+(geBitmap_Height(m_theHUD[nItem].Identifier)/2)-(geBitmap_Height(m_theHUD[nItem].Indicator)/2);
									}
									else
									{
										offx = m_theHUD[nItem].nLeft+(geBitmap_Width(m_theHUD[nItem].Identifier)/2)-(geBitmap_Width(m_theHUD[nItem].Indicator2)/2);
										offy = m_theHUD[nItem].nTop+(geBitmap_Height(m_theHUD[nItem].Identifier)/2)-(geBitmap_Height(m_theHUD[nItem].Indicator2)/2);
									}

									if(angle < 0.0f)
										mulx = 1.0f;

									if(fabs(angle) > GE_PIOVER2)
										muly = -1.0f;

									if(fabs(angle) == 0.0f || fabs(angle) == GE_PI)
									{
										offy += static_cast<int>(muly * dist);
									}
									else if(fabs(angle) == GE_PIOVER2)
									{
										offx += static_cast<int>(mulx * dist);
									}
									else
									{
										angle = fabs(angle);

										if(muly == -1.0f)
										{
											angle = GE_PI - angle;
										}

										if(angle <= (GE_PI/4.0f))
										{
											if(!flag)
											{
												CCD->Engine()->DrawBitmap(m_theHUD[nItem].Indicator, NULL,
																			offx - static_cast<int>(sin(angle) * dist * mulx),
																			offy - static_cast<int>(cos(angle) * dist * muly),
																			ZDEPTH11);
											}
											else
											{
												CCD->Engine()->DrawBitmap(m_theHUD[nItem].Indicator2, NULL,
																			offx - static_cast<int>(sin(angle) * dist * mulx),
																			offy - static_cast<int>(cos(angle) * dist * muly),
																			ZDEPTH11);
											}
										}
										else
										{
											// changed QD 12/15/05
											angle = GE_PIOVER2 - angle;//(GE_PI/2.0f) - angle;

											if(!flag)
											{
												CCD->Engine()->DrawBitmap(m_theHUD[nItem].Indicator, NULL,
																			offx - static_cast<int>(cos(angle) * dist * mulx),
																			offy - static_cast<int>(sin(angle) * dist * muly),
																			ZDEPTH11);
											}
											else
											{
												CCD->Engine()->DrawBitmap(m_theHUD[nItem].Indicator2, NULL,
																			offx - static_cast<int>(cos(angle) *dist * mulx),
																			offy - static_cast<int>(sin(angle) *dist * muly),
																			ZDEPTH11);
											}
										}
									}
								}
							}
// end change RF064
						}
					}
				}

				// Draw the identifier icon
				if(m_theHUD[nItem].Identifier)
				{
					CCD->Engine()->DrawBitmap(m_theHUD[nItem].Identifier, NULL,
												m_theHUD[nItem].nLeft,
												m_theHUD[nItem].nTop, ZDEPTH1);
				}

				break;
			}
// end change RF063
		// Compass
		case COMPASS:
			{
				if(m_theHUD[nItem].Indicator)
				{
					geVec3d Pos;
					CCD->ActorManager()->GetRotate(CCD->Player()->GetActor(),&Pos);
					// changed QD 12/15/05
					Pos.Y *= GE_180OVERPI;// /= 0.0174532925199433f;
					Pos.Y += 30.0f;

					while(Pos.Y<0)
						Pos.Y = Pos.Y + 360.0f;

					while(Pos.Y > 360.0f)
						Pos.Y = Pos.Y - 360.0f;

					Pos.Y /=3.0f;

					theRect.Top = 0;
					theRect.Bottom = geBitmap_Height(m_theHUD[nItem].Indicator) - 1;
					theRect.Left = static_cast<int>(Pos.Y);
					theRect.Right =  theRect.Left + 40;

					CCD->Engine()->DrawBitmap(m_theHUD[nItem].Indicator, &theRect,
						m_theHUD[nItem].nLeft+m_theHUD[nItem].iLeftOffset,
						m_theHUD[nItem].nTop+m_theHUD[nItem].iTopOffset, ZDEPTH11);
// end change RF064
				}

				// Draw the identifier icon
				if(m_theHUD[nItem].Identifier)
				{
					// changed RF064
					CCD->Engine()->DrawBitmap(m_theHUD[nItem].Identifier,
												NULL,
												m_theHUD[nItem].nLeft,
												m_theHUD[nItem].nTop,
												ZDEPTH1);
					// end change RF064
				}

				break;
			}
		default:
			break;
		}
	}

	return RGF_SUCCESS;
}

/* ----------------------------------- END OF FILE ------------------------------------ */

