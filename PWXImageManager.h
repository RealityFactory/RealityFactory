#ifndef __RGF_CRFPWXIM_H_
#define __RGF_CRFPWXIM_H_

#define PWX_IMAGES  200

#include "genesis.h"

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

// class declaration for CFlipBook
class PWXImageManager : public CRGFComponent
{
public:
  PWXImageManager();
  ~PWXImageManager();
  geBoolean AddImage(char* Tx, geBitmap*, int Xp, int Yp, float Alph, int R, int G, int B,int tFont,float ZDepth);
  void Display();
};

#endif