/*
CHeadsUpDisplay.cpp:		HUD display handler

  (c) 1999 Edward A. Averill, III
  
	This file contains the class implementation for the CHeadsUpDisplay
	class that encapsulates player status display functionality.
*/

//	Include the One True Header

#include "RabidFramework.h"

//	Constructor
//
//	Set up the HUD

CHeadsUpDisplay::CHeadsUpDisplay()
{
	m_bHUDActive = false;					// HUD not displayed
	
	//	Clear the HUD array
	
	for(int nTemp = 0; nTemp < 12; nTemp++)
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
	
	for(int nTemp = 0; nTemp < 12; nTemp++)
	{
		if(m_theHUD[nTemp].Identifier != NULL)
			geBitmap_Destroy(&m_theHUD[nTemp].Identifier);
		if(m_theHUD[nTemp].Indicator != NULL)
			geBitmap_Destroy(&m_theHUD[nTemp].Indicator);
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
	
	for(int nTemp = 0; nTemp < 12; nTemp++)
	{
		if(m_theHUD[nTemp].Identifier != NULL)
			geBitmap_Destroy(&m_theHUD[nTemp].Identifier);
		if(m_theHUD[nTemp].Indicator != NULL)
			geBitmap_Destroy(&m_theHUD[nTemp].Indicator);
		memset(&m_theHUD[nTemp], 0, sizeof(HUDEntry));
	}
	
	CString KeyName = AttrFile.FindFirstKey();
	char szAttr[64], szType[64], szName[64], szAlpha[64];
	CString Tname, Talpha;
	geBitmap *TempBmp1, *TempBmp2;
	bool valid, active;
	int nTop, nLeft, iTopOffset, iLeftOffset, Height;
	HUDTYPE Type;
	float Font;
	char format[16];
	while(KeyName != "")
	{
		strcpy(szType,KeyName);
		valid = false;
		active = true;
		TempBmp1 = NULL;
		TempBmp2 = NULL;
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
				TempBmp2 = CreateFromFileAndAlphaNames(szName, szAlpha);
			}
		}
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
			strcpy(szAttr,KeyName);
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
							TempBmp2 = CreateFromFileAndAlphaNames(szName, szAlpha);
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
				TempBmp1 = CreateFromFileAndAlphaNames(szName, szAlpha);
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
			if(TempBmp1)
				geEngine_AddBitmap(CCD->Engine()->Engine(), TempBmp1);
			if(TempBmp2)
				geEngine_AddBitmap(CCD->Engine()->Engine(), TempBmp2);
			for(int nItem = 0; nItem < 12; nItem++)
				if(!m_theHUD[nItem].active)
					break;
				if(nItem < 12)
				{
					m_theHUD[nItem].active = true;
					m_theHUD[nItem].display = active;
					m_theHUD[nItem].Type = Type;
					m_theHUD[nItem].Identifier = TempBmp1;
					m_theHUD[nItem].Indicator = TempBmp2;
					strcpy(m_theHUD[nItem].szAttributeName, szAttr);
					strcpy(m_theHUD[nItem].format, format);
					m_theHUD[nItem].nTop = nTop;
					m_theHUD[nItem].nLeft = nLeft;
					m_theHUD[nItem].iTopOffset = iTopOffset;
					m_theHUD[nItem].iLeftOffset = iLeftOffset;
					m_theHUD[nItem].PixelsPerIncrement = Font;
					m_theHUD[nItem].iHeight = Height;
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
	for(int nItem = 0; nItem < 12; nItem++)
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
	for(int nItem = 0; nItem < 12; nItem++)
	{
		if(!strcmp(szAttributeName, m_theHUD[nItem].szAttributeName))
		{
			m_theHUD[nItem].display = activate;
			return RGF_SUCCESS;					// It's outta here
		}
	}
	
	//	Ooops, HUD attribute not found.
	
	return RGF_FAILURE;
}

//	Render
//
//	If the HUD is active, render it to the screen.

int CHeadsUpDisplay::Render()
{
	geRect theRect;
	int nValue;
	
	if(!m_bHUDActive)
		return RGF_SUCCESS;				// Ignore the call
	
	//	Iterate through the HUD element list, displaying each place there
	//	..is an entry in the slot.
	
	for(int nItem = 0; nItem < 12; nItem++)
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
				geEngine_DrawBitmap(CCD->Engine()->Engine(), m_theHUD[nItem].Identifier, 
					NULL, m_theHUD[nItem].nLeft, m_theHUD[nItem].nTop);
			CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
			nValue = theInv->Value(m_theHUD[nItem].szAttributeName);
			m_theHUD[nItem].PixelsPerIncrement = (float)m_theHUD[nItem].iHeight / ((float)theInv->High(m_theHUD[nItem].szAttributeName) - (float)theInv->Low(m_theHUD[nItem].szAttributeName));
			// Compute the rectangle to draw for the level indicator
			if(m_theHUD[nItem].Indicator)
			{
				theRect.Top = m_theHUD[nItem].iHeight-((int)((m_theHUD[nItem].PixelsPerIncrement * (geFloat)nValue))-1);
				theRect.Bottom = m_theHUD[nItem].iHeight;
				theRect.Left = 0;
				theRect.Right =  geBitmap_Width(m_theHUD[nItem].Indicator);
				geEngine_DrawBitmap(CCD->Engine()->Engine(), m_theHUD[nItem].Indicator, 
					&theRect, m_theHUD[nItem].nLeft+m_theHUD[nItem].iLeftOffset, m_theHUD[nItem].nTop+m_theHUD[nItem].iTopOffset+theRect.Top);
			}
		}
		// Horizontal Indicator
		if(m_theHUD[nItem].Type == HORIZ)
		{
			// Draw the identifier icon
			if(m_theHUD[nItem].Identifier)
				geEngine_DrawBitmap(CCD->Engine()->Engine(), m_theHUD[nItem].Identifier, 
				NULL, m_theHUD[nItem].nLeft, m_theHUD[nItem].nTop);
			CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
			nValue = theInv->Value(m_theHUD[nItem].szAttributeName);
			m_theHUD[nItem].PixelsPerIncrement = (float)m_theHUD[nItem].iHeight / ((float)theInv->High(m_theHUD[nItem].szAttributeName) - (float)theInv->Low(m_theHUD[nItem].szAttributeName));
			// Compute the rectangle to draw for the level indicator
			if(m_theHUD[nItem].Indicator)
			{
				theRect.Top = 0;
				theRect.Bottom = geBitmap_Height(m_theHUD[nItem].Indicator);
				theRect.Left = 0;
				theRect.Right = (int)((m_theHUD[nItem].PixelsPerIncrement * (geFloat)nValue))-1;
				geEngine_DrawBitmap(CCD->Engine()->Engine(), m_theHUD[nItem].Indicator, 
					&theRect, m_theHUD[nItem].nLeft+m_theHUD[nItem].iLeftOffset, m_theHUD[nItem].nTop+m_theHUD[nItem].iTopOffset+theRect.Top);
			}
			
		}
		// numeric indicator
		if(m_theHUD[nItem].Type == VALUE)
		{
			// Draw the identifier icon
			if(m_theHUD[nItem].Identifier)
				geEngine_DrawBitmap(CCD->Engine()->Engine(), m_theHUD[nItem].Identifier, 
				NULL, m_theHUD[nItem].nLeft, m_theHUD[nItem].nTop);
			CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
			nValue = theInv->Value(m_theHUD[nItem].szAttributeName);
			char szBug[256];
			sprintf(szBug, m_theHUD[nItem].format, nValue);
			CCD->MenuManager()->FontRect(szBug, (int)m_theHUD[nItem].PixelsPerIncrement, m_theHUD[nItem].nLeft+m_theHUD[nItem].iLeftOffset, m_theHUD[nItem].nTop+m_theHUD[nItem].iTopOffset);
		}
		// player position
		if(m_theHUD[nItem].Type == PPOS)
		{
			// Draw the identifier icon
			if(m_theHUD[nItem].Identifier)
				geEngine_DrawBitmap(CCD->Engine()->Engine(), m_theHUD[nItem].Identifier, 
					NULL, m_theHUD[nItem].nLeft, m_theHUD[nItem].nTop);
			char szBug[256];
			geVec3d Pos = CCD->Player()->Position();
			sprintf(szBug, m_theHUD[nItem].format, Pos.X, Pos.Y, Pos.Z);
			CCD->MenuManager()->FontRect(szBug, (int)m_theHUD[nItem].PixelsPerIncrement, m_theHUD[nItem].nLeft+m_theHUD[nItem].iLeftOffset, m_theHUD[nItem].nTop+m_theHUD[nItem].iTopOffset);
		}
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
				geEngine_DrawBitmap(CCD->Engine()->Engine(), m_theHUD[nItem].Indicator, 
					&theRect, m_theHUD[nItem].nLeft+m_theHUD[nItem].iLeftOffset, m_theHUD[nItem].nTop+m_theHUD[nItem].iTopOffset);
			}
			// Draw the identifier icon
			if(m_theHUD[nItem].Identifier)
				geEngine_DrawBitmap(CCD->Engine()->Engine(), m_theHUD[nItem].Identifier, 
				NULL, m_theHUD[nItem].nLeft, m_theHUD[nItem].nTop);
		}
	}
	
	return RGF_SUCCESS;
}

