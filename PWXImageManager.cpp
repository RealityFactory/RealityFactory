/************************************************************************************//**
 * @file PWXImageManager.cpp
 * @brief PWXImageManager implementation
 ****************************************************************************************/

#include "RabidFramework.h"

/* ------------------------------------------------------------------------------------ */
//	Constructor
/* ------------------------------------------------------------------------------------ */
PWXImageManager::PWXImageManager()
{
	IMC = 0;
	return;
}

/* ------------------------------------------------------------------------------------ */
//	Destructor
/* ------------------------------------------------------------------------------------ */
PWXImageManager::~PWXImageManager()
{
	return;
}

/* ------------------------------------------------------------------------------------ */
//	AddImage
/* ------------------------------------------------------------------------------------ */
geBoolean PWXImageManager::AddImage(const char* Txt, geBitmap* Bm,
									int Xp, int Yp,
									float Alpha, int R, int G, int B,
									int tFont, float ZDepth)
{
// changed QD 12/15/05
	//if(IMC > PWX_IMAGES)
	if(IMC >= PWX_IMAGES)
		return GE_FALSE;

	Image[IMC].Xpos = Xp;
	Image[IMC].Ypos = Yp;
	Image[IMC].Alpha = Alpha;
	Image[IMC].Fnt = tFont;
	Image[IMC].ZDpth = ZDepth;

	if(Txt != NULL)
	{
		strncpy(Image[IMC].PWX_Text, Txt, 127);
		Image[IMC].PWX_Text[127] = '\0';
		Image[IMC].TXTactive = true;
	}
	else
	{
		Image[IMC].PWX_Pic = Bm;
		Image[IMC].IMactive = true;
	}

	IMC++;
	return GE_TRUE;
}

/* ------------------------------------------------------------------------------------ */
//	Display
/* ------------------------------------------------------------------------------------ */
void PWXImageManager::Display()
{
	int i;

	for(i=0; i<IMC; i++)
	{
		if(Image[i].IMactive)
		{
			CCD->Engine()->DrawBitmap(Image[i].PWX_Pic, NULL, Image[i].Xpos, Image[i].Ypos, Image[i].ZDpth);
			Image[i].IMactive = false;
		}
		else if(Image[i].TXTactive)
		{
			Image[i].TXTactive = false;
			CCD->MenuManager()->WorldFontRect(Image[i].PWX_Text, Image[i].Fnt, Image[i].Xpos, Image[i].Ypos, Image[i].Alpha);
		}
		else
			break;

	}

	IMC = 0;
}


/* ----------------------------------- END OF FILE ------------------------------------ */
