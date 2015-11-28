#include "RabidFramework.h"

PWXImage Image[PWX_IMAGES];
int IMC;

PWXImageManager::PWXImageManager()
{
IMC = 0;

return;
}


PWXImageManager::~PWXImageManager()
{


return;
}

geBoolean PWXImageManager::AddImage(char* Txt, geBitmap* Bm, int Xp, int Yp, float Alph, int R, int G, int B,int tFont,float ZDepth)
{
	if(IMC > PWX_IMAGES)
		return GE_FALSE;

	Image[IMC].Xpos = Xp;
	Image[IMC].Ypos = Yp;
	Image[IMC].Alpha = Alph;
	Image[IMC].Fnt = tFont;
	Image[IMC].ZDpth = ZDepth;

	if(Txt != NULL)
	{

		strcpy(Image[IMC].PWX_Text,Txt);
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

void PWXImageManager::Display()
{
	int i;

	for(i=0;i<IMC;i++)
	{
		if(Image[i].IMactive)
		{
			CCD->Engine()->DrawBitmap(Image[i].PWX_Pic,NULL,Image[i].Xpos,Image[i].Ypos,Image[i].ZDpth);
			Image[i].IMactive = false;
		}
		else if(Image[i].TXTactive)
		{
			Image[i].TXTactive = false;
			CCD->MenuManager()->WorldFontRect(Image[i].PWX_Text,Image[i].Fnt,Image[i].Xpos,Image[i].Ypos,Image[i].Alpha);
		}
		else
			break;
		
	}
	IMC = 0;
}

