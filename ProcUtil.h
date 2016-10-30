/************************************************************************************//**
 * @file ProcUtil.h
 * @brief Utility functions for procedurals
 ****************************************************************************************/

#include <Genesis.h>
#include <ram.h>
#include <memory>
#include <string>
#include <malloc.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <ctype.h>

#define min(a, b)  (((a) < (b)) ? (a) : (b))
#define max(a, b)  (((a) > (b)) ? (a) : (b))


#ifndef __RGF_PROCUTIL_H_
#define __RGF_PROCUTIL_H_

geBoolean geBitmapUtil_SmoothBits(geBitmap_Info *pInfo, void *FmBits,
								  void *ToBits, int radius, geBoolean wrap);
geBoolean geBitmapUtil_SetColor(geBitmap *Bmp, int R, int G, int B, int A);
geBoolean ProcUtil_SetPaletteFromString(geBitmap *Bitmap,char **pParams);
void ProcUtil_Randomize(void);
uint32 ProcUtil_Rand(uint32 max);

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
