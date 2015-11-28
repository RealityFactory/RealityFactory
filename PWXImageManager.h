/************************************************************************************//**
 * @file PWXImageManager.h
 * @brief PWXImageManager class
 ****************************************************************************************/

#ifndef __RGF_CRFPWXIM_H_
#define __RGF_CRFPWXIM_H_

#define PWX_IMAGES  200


typedef struct PWXImage
{
	geBitmap* PWX_Pic;
	char PWX_Text[128];
	geBoolean IMactive;
	geBoolean TXTactive;
	int Xpos;
	int Ypos;
	int R;
	int G;
	int B;
	int Fnt;
	float ZDpth;
	float Alpha;
}PWXImage;

/**
 * @brief Class declaration PWXImageManager
 */
class PWXImageManager : public CRGFComponent
{
public:
	PWXImageManager();
	~PWXImageManager();

	geBoolean AddImage(const char *Tx, geBitmap *Bm, int Xp, int Yp,
						float Alpha, int R, int G, int B,
						int tFont, float ZDepth);
	void Display();
private:
	PWXImage Image[PWX_IMAGES];
	int IMC;
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
