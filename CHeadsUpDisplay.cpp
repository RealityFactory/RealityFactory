/*
CHeadsUpDisplay.cpp:		HUD display handler

  (c) 2001 Ralph Deane
  
	This file contains the class implementation for the CHeadsUpDisplay
	class that encapsulates player status display functionality.
*/

//	Include the One True Header

#include "RabidFramework.h"

// changed RF064
extern geBitmap *TPool_Bitmap(char *DefaultBmp, char *DefaultAlpha, char *BName, char *AName);
// end change RF064

//	Constructor
//
//	Set up the HUD

CHeadsUpDisplay::CHeadsUpDisplay()
{
	m_bHUDActive = true;					// HUD not displayed
	
	//	Clear the HUD array
	
	for(int nTemp = 0; nTemp < MAXHUD; nTemp++)
	{
		memset(&m_theHUD[nTemp], 0, sizeof(HUDEntry));
		m_theHUD[nTemp].active = false;
	}
}

//	Destructor
//
//	Release bitmaps, etc.

CHeadsUpDisplay::~CHeadsUpDisplay()
{
	//	Clean up all the HUD bitmaps
	
	for(int nTemp = 0; nTemp < MAXHUD; nTemp++)
	{
// changed RF064
/*
		if(m_theHUD[nTemp].Identifier != NULL)
		{
			geWorld_RemoveBitmap(CCD->World(), m_theHUD[nTemp].Identifier);
			geBitmap_Destroy(&m_theHUD[nTemp].Identifier);
		}
		if(m_theHUD[nTemp].Indicator != NULL)
		{
			geWorld_RemoveBitmap(CCD->World(), m_theHUD[nTemp].Indicator);
			geBitmap_Destroy(&m_theHUD[nTemp].Indicator);
		}
// changed RF063
		if(m_theHUD[nTemp].Indicator2 != NULL)
		{
			geWorld_RemoveBitmap(CCD->World(), m_theHUD[nTemp].Indicator2);
			geBitmap_Destroy(&m_theHUD[nTemp].Indicator2);
		}
// end change RF063
*/
// end change RF064
		memset(&m_theHUD[nTemp], 0, sizeof(HUDEntry));
	}
	
}

//	LoadConfiguration
//
//	Loads the HUD configuration from the HUD configuration file
//	..defined in the PlayerSetup entity.

int CHeadsUpDisplay::LoadConfiguration()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	//	Ok, check to see if there's a PlayerSetup around...
	
	pSet = geWorld_GetEntitySet(CCD->World(), "PlayerSetup");
	
	if(!pSet) 
		return RGF_FAILURE;									// No setup?
	
	//	Ok, get the setup information.  There should only be one, so
	//	..we'll just take the first one we run into.
	
	pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL);
	PlayerSetup *pSetup = (PlayerSetup*)geEntity_GetUserData(pEntity);
	
	if(EffectC_IsStringNull(pSetup->HUDInfoFile))
		return RGF_FAILURE;								// No HUD initialization?
	
	CIniFile AttrFile(pSetup->HUDInfoFile);
	if(!AttrFile.ReadFile())
	{
		char szAttrError[256];
		sprintf(szAttrError,"Can't open player config file '%s'", pSetup->HUDInfoFile);
		CCD->ReportError(szAttrError, false);
		return RGF_FAILURE;
	}
	
	//	Something is there, so let's clean up all the HUD bitmaps to prepare
	
	for(int nTemp = 0; nTemp < MAXHUD; nTemp++)
	{
		if(m_theHUD[nTemp].Identifier != NULL)
			geBitmap_Destroy(&m_theHUD[nTemp].Identifier);
		if(m_theHUD[nTemp].Indicator != NULL)
			geBitmap_Destroy(&m_theHUD[nTemp].Indicator);
// changed RF063
		if(m_theHUD[nTemp].Indicator2 != NULL)
			geBitmap_Destroy(&m_theHUD[nTemp].Indicator2);
// end change RF063
		memset(&m_theHUD[nTemp], 0, sizeof(HUDEntry));
	}
	
	CString KeyName = AttrFile.FindFirstKey();
	char szAttr[64], szType[64], szName[64], szAlpha[64];
	CString Tname, Talpha;
// changed RF063
	geBitmap *TempBmp1, *TempBmp2, *TempBmp3;
	bool valid, active, modify;
	int nTop, nLeft, iTopOffset, iLeftOffset, Height, direction;
// changed RF064
	int Style;
// end change RF064
	HUDTYPE Type;
	float Font, DisplayTime;
	char format[16];
	while(KeyName != "")
	{
// changed RF063
		if(KeyName[0]=='~')
			strcpy(szType,KeyName.Mid(1));
		else
			strcpy(szType,KeyName);
// end change RF063
		valid = false;
		active = true;
		modify = false;
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
		if(!stricmp(szType,"compass"))
		{
			Type = COMPASS;
			strcpy(szAttr,"compass");
			valid = true;
			Tname = AttrFile.GetValue(KeyName, "indicator");
			if(Tname!="")
			{
				Talpha = AttrFile.GetValue(KeyName, "indicatoralpha");
				if(Talpha=="")
					Talpha = Tname;
				strcpy(szName,Tname);
				strcpy(szAlpha,Talpha);
// changed RF064
				TempBmp2 = TPool_Bitmap(szName, szAlpha, NULL, NULL);
				//TempBmp2 = CreateFromFileAndAlphaNames(szName, szAlpha);
// end change RF064
			}
		}
// changed RF063
		else if(!stricmp(szType,"radar"))
		{
			Type = RADAR;
			strcpy(szAttr,"radar");
			valid = true;
			Tname = AttrFile.GetValue(KeyName, "indicator");
			if(Tname!="")
			{
				Talpha = AttrFile.GetValue(KeyName, "indicatoralpha");
				if(Talpha=="")
					Talpha = Tname;
				strcpy(szName,Tname);
				strcpy(szAlpha,Talpha);
// changed RF064
				TempBmp2 = TPool_Bitmap(szName, szAlpha, NULL, NULL);
				//TempBmp2 = CreateFromFileAndAlphaNames(szName, szAlpha);
// end change RF064
			}
			Tname = AttrFile.GetValue(KeyName, "npcindicator");
			if(Tname!="")
			{
				Talpha = AttrFile.GetValue(KeyName, "npcindicatoralpha");
				if(Talpha=="")
					Talpha = Tname;
				strcpy(szName,Tname);
				strcpy(szAlpha,Talpha);
// changed RF064
				TempBmp3 = TPool_Bitmap(szName, szAlpha, NULL, NULL);
				//TempBmp3 = CreateFromFileAndAlphaNames(szName, szAlpha);
// end change RF064
				if(!TempBmp3)
					TempBmp3 = TempBmp2;
			}
			else
				TempBmp3 = TempBmp2;
			Font = (float)AttrFile.GetValueI(KeyName, "range");
		}
// end change RF063
		else if(!stricmp(szType,"position"))
		{
			Type = PPOS;
			strcpy(szAttr,"position");
			valid = true;
			Font = (float)AttrFile.GetValueI(KeyName, "font");
			int width = AttrFile.GetValueI(KeyName, "width");
			if(width<=0)
				width = 4;
			char buf[17];
			itoa(width,buf,10);
			strcpy(format,"%");
			strcat(format,buf);
			strcat(format,".0f %");
			strcat(format,buf);
			strcat(format,".0f %");
			strcat(format,buf);
			strcat(format,".0f");
		}
		else
		{
// changed RF063
			strcpy(szAttr,szType);
// end change RF063
			Tname = AttrFile.GetValue(KeyName, "type");
			if(Tname!="")
			{
				if(Tname=="verticle")
				{
					valid = true;
					Type = VERT;
					Height = AttrFile.GetValueI(KeyName, "indicatorheight");
				}
				if(Tname=="horizontal")
				{
					valid = true;
					Type = HORIZ;
					Height = AttrFile.GetValueI(KeyName, "indicatorwidth");
				}
				if(Tname=="numeric")
				{
					valid = true;
					Type = VALUE;
					Font = (float)AttrFile.GetValueI(KeyName, "font");
					int width = AttrFile.GetValueI(KeyName, "width");
					if(width<=0)
						width = 3;
					char buf[17];
					itoa(width,buf,10);
					strcpy(format,"%");
					strcat(format,buf);
					strcat(format,".1d");	
				}
				if(valid)
				{
// changed RF064
					Tname = AttrFile.GetValue(KeyName, "style");
					if(Tname=="clip")
						Style = CLIP;
					if(Tname=="magazine")
						Style = MAG;
// end change RF064
					if(Type!=VALUE)
					{
						Tname = AttrFile.GetValue(KeyName, "indicator");
						if(Tname!="")
						{
							Talpha = AttrFile.GetValue(KeyName, "indicatoralpha");
							if(Talpha=="")
								Talpha = Tname;
							strcpy(szName,Tname);
							strcpy(szAlpha,Talpha);
// changed RF064
							TempBmp2 = TPool_Bitmap(szName, szAlpha, NULL, NULL);
							//TempBmp2 = CreateFromFileAndAlphaNames(szName, szAlpha);
// end change RF064
						}
					}	
				}
			}
		}
		if(valid)
		{
			Tname = AttrFile.GetValue(KeyName, "frame");
			if(Tname!="")
			{
				Talpha = AttrFile.GetValue(KeyName, "framealpha");
				if(Talpha=="")
					Talpha = Tname;
				strcpy(szName,Tname);
				strcpy(szAlpha,Talpha);
// changed RF064
				TempBmp1 = TPool_Bitmap(szName, szAlpha, NULL, NULL);
				//TempBmp1 = CreateFromFileAndAlphaNames(szName, szAlpha);
// end change RF064
			} 
			if(AttrFile.GetValue(KeyName, "framex")=="center")
			{
				if(TempBmp1)
					nLeft = geBitmap_Width(TempBmp1)/2;
				nLeft = (CCD->Engine()->Width()/2)-nLeft;
			}
			else
			{
				nLeft = AttrFile.GetValueI(KeyName, "framex");
				if(nLeft<0)
					nLeft = CCD->Engine()->Width() + nLeft;
			}
			if(AttrFile.GetValue(KeyName, "framey")=="center")
			{
				if(TempBmp1)
					nTop = geBitmap_Height(TempBmp1)/2;
				nTop = (CCD->Engine()->Height()/2)-nTop;
			}
			else
			{
				nTop = AttrFile.GetValueI(KeyName, "framey");
				if(nTop<0)
					nTop = CCD->Engine()->Height() + nTop;
			}
			iLeftOffset = AttrFile.GetValueI(KeyName, "indicatoroffsetx");
			iTopOffset = AttrFile.GetValueI(KeyName, "indicatoroffsety");
			if(AttrFile.GetValue(KeyName, "active")=="false")
				active = false;
			if(AttrFile.GetValue(KeyName, "active")=="modify")
			{
				active = false;
				modify = true;
			}
			DisplayTime = (float)AttrFile.GetValueF(KeyName, "displaytime");
			if(AttrFile.GetValue(KeyName, "modifydirection")=="down")
				direction = 1;
			if(AttrFile.GetValue(KeyName, "modifydirection")=="both")
				direction = 2;
// changed RF064
/*
			if(TempBmp1)
				geWorld_AddBitmap(CCD->World(), TempBmp1);
			if(TempBmp2)
				geWorld_AddBitmap(CCD->World(), TempBmp2);
// changed RF063
			if(TempBmp3)
				geWorld_AddBitmap(CCD->World(), TempBmp3);
// end change RF063
*/
// end change RF064
			for(int nItem = 0; nItem < MAXHUD; nItem++)
				if(!m_theHUD[nItem].active)
					break;
				if(nItem < MAXHUD)
				{
					m_theHUD[nItem].active = true;
					m_theHUD[nItem].display = active;
					m_theHUD[nItem].modify = modify;
					m_theHUD[nItem].OldAmount = -99999;
					m_theHUD[nItem].direction = direction;
					m_theHUD[nItem].Type = Type;
					m_theHUD[nItem].DisplayTime = DisplayTime;
					m_theHUD[nItem].Identifier = TempBmp1;
					m_theHUD[nItem].Indicator = TempBmp2;
// changed RF063
					m_theHUD[nItem].Indicator2 = TempBmp3;
					strcpy(m_theHUD[nItem].szAttributeName, szAttr);
					strcpy(m_theHUD[nItem].format, format);
					m_theHUD[nItem].nTop = nTop;
					m_theHUD[nItem].nLeft = nLeft;
					m_theHUD[nItem].iTopOffset = iTopOffset;
					m_theHUD[nItem].iLeftOffset = iLeftOffset;
					m_theHUD[nItem].PixelsPerIncrement = Font;
					m_theHUD[nItem].iHeight = Height;
// changed RF064
					m_theHUD[nItem].Style = Style;
// end change RF064
				}
		}
		KeyName = AttrFile.FindNextKey();
	}

	Activate();								// Turn the HUD ON!

	return RGF_SUCCESS;
}

//	Activate
//
//	Set up the HUD as renderable.

int CHeadsUpDisplay::Activate()
{
	m_bHUDActive = true;
	
	return RGF_SUCCESS;
}

//	Deactivate
//
//	Flag the HUD as non-renderable.

int CHeadsUpDisplay::Deactivate()
{
	m_bHUDActive = false;
	
	return RGF_SUCCESS;
}

//	RemoveElement
//
//	Remove the named element from the HUD display list.

int CHeadsUpDisplay::RemoveElement(char *szAttributeName)
{
	for(int nItem = 0; nItem < MAXHUD; nItem++)
	{
		if(!strcmp(szAttributeName, m_theHUD[nItem].szAttributeName))
		{
			// It's this attribute, wipe it out.
			if(m_theHUD[nItem].Identifier)
				geBitmap_Destroy(&m_theHUD[nItem].Identifier);
			if(m_theHUD[nItem].Indicator)
				geBitmap_Destroy(&m_theHUD[nItem].Indicator);
			memset(&m_theHUD[nItem], 0, sizeof(HUDEntry));
			return RGF_SUCCESS;					// It's outta here
		}
	}
	
	//	Ooops, HUD attribute not found.
	
	return RGF_FAILURE;
}

int CHeadsUpDisplay::ActivateElement(char *szAttributeName, bool activate)
{
// changed RF063
	bool flag = false;
	for(int nItem = 0; nItem < MAXHUD; nItem++)
	{
		if(!strcmp(szAttributeName, m_theHUD[nItem].szAttributeName))
		{
			m_theHUD[nItem].display = activate;
			flag = true;;					// It's outta here
		}
	}
	
	if(flag)
		return RGF_SUCCESS;
// end change RF063
	//	Ooops, HUD attribute not found.
	
	return RGF_FAILURE;
}

// changed RF063
void CHeadsUpDisplay::Tick(geFloat dwTick)
{
	for(int nItem = 0; nItem < MAXHUD; nItem++)
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
					if(m_theHUD[nItem].direction==0 && (m_theHUD[nItem].OldAmount < theInv->Value(m_theHUD[nItem].szAttributeName)))
						flag = true;
					else if(m_theHUD[nItem].direction==1 && (m_theHUD[nItem].OldAmount > HudValue))
						flag = true;
					else if(m_theHUD[nItem].direction==2 && (m_theHUD[nItem].OldAmount != HudValue))
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
					if(m_theHUD[nItem].CurrentTime>=m_theHUD[nItem].DisplayTime)
					{
						m_theHUD[nItem].display = false;
					}
				}
			}
		}
	}
}
// end change RF063

//	Render
//
//	If the HUD is active, render it to the screen.

int CHeadsUpDisplay::Render()
{
	geRect theRect;
	int nValue, nLow, nHigh;
	
	if(!m_bHUDActive)
		return RGF_SUCCESS;				// Ignore the call
	
	//	Iterate through the HUD element list, displaying each place there
	//	..is an entry in the slot.
	
	for(int nItem = 0; nItem < MAXHUD; nItem++)
	{
		if(!m_theHUD[nItem].active)
			continue;								// No item in slot
		if(!m_theHUD[nItem].display)
			continue;
		// Verticle Indicator
		if(m_theHUD[nItem].Type == VERT)
		{
			// Draw the identifier icon
			if(m_theHUD[nItem].Identifier)
				// changed RF064
				CCD->Engine()->DrawBitmap(m_theHUD[nItem].Identifier, NULL,
					m_theHUD[nItem].nLeft,
					m_theHUD[nItem].nTop);
				// end change RF064

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
					if(magsize==0)
						continue;
					if(m_theHUD[nItem].Style==MAG)
					{
						if(nValue>magsize)
							nValue = magsize - CCD->Weapons()->GetShotFired();
						nHigh = magsize;
						nLow = 0;
					}
					else
					{
						nValue += (magsize-1);
						nHigh += (magsize-1);
						nHigh /= magsize;
						nLow /= magsize;
						nValue /= magsize;
					}
				}
				else
				{
					if(m_theHUD[nItem].Style==MAG)
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
			m_theHUD[nItem].PixelsPerIncrement = (float)m_theHUD[nItem].iHeight / ((float)nHigh - (float)nLow);
// end change RF063
			// Compute the rectangle to draw for the level indicator
			if(m_theHUD[nItem].Indicator)
			{
				theRect.Top = m_theHUD[nItem].iHeight-((int)((m_theHUD[nItem].PixelsPerIncrement * (geFloat)nValue))-1);
				theRect.Bottom = m_theHUD[nItem].iHeight;
				theRect.Left = 0;
				theRect.Right =  geBitmap_Width(m_theHUD[nItem].Indicator);
// changed RF064
				CCD->Engine()->DrawBitmap(m_theHUD[nItem].Indicator, &theRect,
					m_theHUD[nItem].nLeft+m_theHUD[nItem].iLeftOffset,
					m_theHUD[nItem].nTop+m_theHUD[nItem].iTopOffset+theRect.Top);
// end change RF064
			}
		}
		// Horizontal Indicator
		if(m_theHUD[nItem].Type == HORIZ)
		{
			// Draw the identifier icon
			if(m_theHUD[nItem].Identifier)
				// changed RF064
				CCD->Engine()->DrawBitmap(m_theHUD[nItem].Identifier, NULL,
					m_theHUD[nItem].nLeft,
					m_theHUD[nItem].nTop);
				// end change RF064

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
					if(magsize==0)
						continue;
					if(m_theHUD[nItem].Style==MAG)
					{
						if(nValue>magsize)
							nValue = magsize - CCD->Weapons()->GetShotFired();
						nHigh = magsize;
						nLow = 0;
					}
					else
					{
						nValue += (magsize-1);
						nHigh += (magsize-1);
						nHigh /= magsize;
						nLow /= magsize;
						nValue /= magsize;
					}
				}
				else
				{
					if(m_theHUD[nItem].Style==MAG)
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
			m_theHUD[nItem].PixelsPerIncrement = (float)m_theHUD[nItem].iHeight / ((float)nHigh - (float)nLow);
// end change RF063
			// Compute the rectangle to draw for the level indicator
			if(m_theHUD[nItem].Indicator)
			{
				theRect.Top = 0;
				theRect.Bottom = geBitmap_Height(m_theHUD[nItem].Indicator);
				theRect.Left = 0;
				theRect.Right = (int)((m_theHUD[nItem].PixelsPerIncrement * (geFloat)nValue))-1;
// changed RF064
				CCD->Engine()->DrawBitmap(m_theHUD[nItem].Indicator, &theRect,
					m_theHUD[nItem].nLeft+m_theHUD[nItem].iLeftOffset,
					m_theHUD[nItem].nTop+m_theHUD[nItem].iTopOffset+theRect.Top);
// end change RF064
			}
			
		}
		// numeric indicator
		if(m_theHUD[nItem].Type == VALUE)
		{
			// Draw the identifier icon
			if(m_theHUD[nItem].Identifier)
				// changed RF064
				CCD->Engine()->DrawBitmap(m_theHUD[nItem].Identifier, NULL,
					m_theHUD[nItem].nLeft,
					m_theHUD[nItem].nTop);
				// end change RF064

			CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
// changed RF064
			if(!theInv->Has(m_theHUD[nItem].szAttributeName))
				continue;
// end change RF064
			nValue = theInv->Value(m_theHUD[nItem].szAttributeName);
// changed RF063
// changed RF064
			if(m_theHUD[nItem].Style!=NONE)
			{
				if(!strcmp(m_theHUD[nItem].szAttributeName, CCD->Weapons()->GetWeaponAmmo()))
				{
					int magsize = CCD->Weapons()->GetMagSize();
					if(magsize==0)
						continue;
					if(m_theHUD[nItem].Style==MAG)
					{
						if(nValue>magsize)
							nValue = magsize - CCD->Weapons()->GetShotFired();
					}
					else
					{
						nValue += (magsize-1);
						nValue /= magsize;
					}
				}
				else
				{
					if(m_theHUD[nItem].Style==MAG)
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
			CCD->MenuManager()->FontRect(szBug, (int)m_theHUD[nItem].PixelsPerIncrement, m_theHUD[nItem].nLeft+m_theHUD[nItem].iLeftOffset, m_theHUD[nItem].nTop+m_theHUD[nItem].iTopOffset);
		}
		// player position
		if(m_theHUD[nItem].Type == PPOS)
		{
			// Draw the identifier icon
			if(m_theHUD[nItem].Identifier)
				// changed RF064
				CCD->Engine()->DrawBitmap(m_theHUD[nItem].Identifier, NULL,
					m_theHUD[nItem].nLeft,
					m_theHUD[nItem].nTop);
				// end change RF064

			char szBug[256];
			geVec3d Pos = CCD->Player()->Position();
			sprintf(szBug, m_theHUD[nItem].format, Pos.X, Pos.Y, Pos.Z);
			CCD->MenuManager()->FontRect(szBug, (int)m_theHUD[nItem].PixelsPerIncrement, m_theHUD[nItem].nLeft+m_theHUD[nItem].iLeftOffset, m_theHUD[nItem].nTop+m_theHUD[nItem].iTopOffset);
		} 
// changed RF063
		// Radar
		if(m_theHUD[nItem].Type == RADAR)
		{
			// Draw the identifier icon
			if(m_theHUD[nItem].Identifier)
				// changed RF064
				CCD->Engine()->DrawBitmap(m_theHUD[nItem].Identifier, NULL,
					m_theHUD[nItem].nLeft,
					m_theHUD[nItem].nTop);
				// end change RF064

			geActor *ActorsInRange[512];
			geVec3d Pos, RPos, Orient;
			Pos = CCD->Player()->Position();
			int nActorCount = CCD->ActorManager()->GetActorsInRange(Pos, 
								m_theHUD[nItem].PixelsPerIncrement, 512, &ActorsInRange[0]);
			if(nActorCount != 0 && m_theHUD[nItem].Indicator)
			{
				float scale = ((float)geBitmap_Height(m_theHUD[nItem].Identifier)*0.5f)/m_theHUD[nItem].PixelsPerIncrement;
				CCD->ActorManager()->GetRotate(CCD->Player()->GetActor(), &RPos);
				while(RPos.Y<0.0f)
				{
					RPos.Y+=(GE_PI*2);
				}
				while(RPos.Y>(GE_PI*2))
				{
					RPos.Y-=(GE_PI*2);
				}
				for(int nTemp = 0; nTemp < nActorCount; nTemp++)
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
								if(ActorType==ENTITY_NPC)
									flag = true;
								CCD->ActorManager()->GetPosition(ActorsInRange[nTemp], &APos);
								float dist = ((float)fabs(geVec3d_DistanceBetween(&Pos, &APos)))*scale;;
								geVec3d_Subtract(&APos,&Pos,&Orient);
								Orient.Y = (float)atan2(Orient.X , Orient.Z ) + GE_PI;
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
								if(angle<0.0f)
									mulx = 1.0f;
								if((float)fabs(angle)>(GE_PI/2.0f))
									muly = -1.0f;
								if((float)fabs(angle)==0.0f || (float)fabs(angle)==GE_PI)
								{
									offy += (int)(muly*dist);
								}
								else if((float)fabs(angle)==(GE_PI/2.0f))
								{
									offx += (int)(mulx*dist);
								}
								else
								{
									angle = (float)fabs(angle);
									if(muly==-1.0f)
									{
										angle = GE_PI - angle;
									}
									if(angle<=(GE_PI/4.0f))
									{
										if(!flag)
											// changed RF064
											CCD->Engine()->DrawBitmap(m_theHUD[nItem].Indicator, NULL,
											offx-(int)(sin(angle)*dist*mulx),
											offy-(int)(cos(angle)*dist*muly));
											// end change RF064
										else
											// changed RF064
											CCD->Engine()->DrawBitmap(m_theHUD[nItem].Indicator2, NULL,
											offx-(int)(sin(angle)*dist*mulx),
											offy-(int)(cos(angle)*dist*muly));
											// end change RF064
									}
									else
									{
										angle = (GE_PI/2.0f) - angle;
										if(!flag)
											// changed RF064
											CCD->Engine()->DrawBitmap(m_theHUD[nItem].Indicator, NULL,
											offx-(int)(cos(angle)*dist*mulx),
											offy-(int)(sin(angle)*dist*muly));
											// end change RF064
										else
											// changed RF064
											CCD->Engine()->DrawBitmap(m_theHUD[nItem].Indicator2, NULL,
											offx-(int)(cos(angle)*dist*mulx),
											offy-(int)(sin(angle)*dist*muly));
											// end change RF064
									}
								}
							}
						}
// end change RF064
					}
				}
			} 
		}
// end change RF063
		// Compass
		if(m_theHUD[nItem].Type == COMPASS)
		{
			if(m_theHUD[nItem].Indicator)
			{
				geVec3d Pos;
				CCD->ActorManager()->GetRotate(CCD->Player()->GetActor(),&Pos);
				Pos.Y /= 0.0174532925199433f;
				Pos.Y += 30.0f;
				while(Pos.Y<0)
					Pos.Y = 360.0f+Pos.Y;
				while(Pos.Y>360.0f)
					Pos.Y = Pos.Y-360.0f;
				Pos.Y /=3.0f;
				theRect.Top = 0;
				theRect.Bottom = geBitmap_Height(m_theHUD[nItem].Indicator);
				theRect.Left = (int)Pos.Y;
				theRect.Right =  theRect.Left+40;
// changed RF064
				CCD->Engine()->DrawBitmap(m_theHUD[nItem].Indicator, &theRect,
					m_theHUD[nItem].nLeft+m_theHUD[nItem].iLeftOffset,
					m_theHUD[nItem].nTop+m_theHUD[nItem].iTopOffset);
// end change RF064
			}
			// Draw the identifier icon
			if(m_theHUD[nItem].Identifier)
				// changed RF064
				CCD->Engine()->DrawBitmap(m_theHUD[nItem].Identifier, NULL,
					m_theHUD[nItem].nLeft,
					m_theHUD[nItem].nTop);
				// end change RF064
		}
	}
	
	return RGF_SUCCESS;
}

