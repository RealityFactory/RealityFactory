/************************************************************************************//**
 * @file ProcFire.cpp
 * @brief Procedural Fire
 ****************************************************************************************/

#include "ProcUtil.h"

#define BM_WIDTH	(128)
#define BM_HEIGHT	(128)

#define SMOOTH_WRAP (GE_FALSE)

#define SeedLines	(1)
#define myrand(max)	ProcUtil_Rand(max)

typedef struct Procedural
{
	geBitmap	*Bitmap;
	uint32		Frame;
	geBoolean	DoJets;

} Procedural;


static geBoolean FireAnimator_CreateFire(Procedural * Proc);

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
Procedural *Fire_Create(char *TextureName, geWorld  *World, const char *ParmStart)
{
	Procedural	*P;
	geBitmap	*ppBitmap;

	P = GE_RAM_ALLOCATE_STRUCT(Procedural);

	if(!P)
		return (Procedural*)NULL;

	memset(P, 0, sizeof(Procedural));
	ProcUtil_Randomize();

	ppBitmap = geWorld_GetBitmapByName(World, TextureName);

	if(!ppBitmap)
		return (Procedural*)NULL;

	P->Frame = 0;

	if(strlen(ParmStart) < 10)
	{
		ParmStart = strdup("F, pow, 400, 280, 200, 530, 0.3, 0.6, 1.0, 0.8");
	}

	if(!ppBitmap)
	{
		ppBitmap = geBitmap_Create(BM_WIDTH,BM_HEIGHT,1, GE_PIXELFORMAT_8BIT_PAL);

		if(!ppBitmap)
		{
			geRam_Free(P);
			return (Procedural*)NULL;
		}
	}
	else
	{
		geBitmap_CreateRef(ppBitmap);
	}

	if(!geBitmap_SetFormat(ppBitmap, GE_PIXELFORMAT_8BIT_PAL,GE_TRUE, 255, (geBitmap_Palette*)NULL))
	{
		geRam_Free(P);
		return (Procedural*)NULL;
	}

	geBitmap_ClearMips(ppBitmap);

	geBitmap_SetPreferredFormat(ppBitmap, GE_PIXELFORMAT_16BIT_4444_ARGB);

	P->Bitmap = ppBitmap;

	{
		char ParmWork[100];
		char *ptr;
		int len;

		strncpy(ParmWork, ParmStart, 100);
		ptr = strtok(ParmWork, " \t\n\r,");

		if(toupper(*ptr) == 'T')
			P->DoJets = 1;
		else
			P->DoJets = atol(ptr);

		ptr = strtok((char *)NULL," \t\n\r,");
		len = (int)(ptr - ParmWork);
		ParmStart += len;
	}

	if(!ProcUtil_SetPaletteFromString(P->Bitmap, (char**)&ParmStart))
	{
		geRam_Free(P);
		return (Procedural*)NULL;
	}

	return P;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
void Fire_Destroy(Procedural *P)
{
	if(!P)
	  return;

	if(P->Bitmap)
		geBitmap_Destroy(&(P->Bitmap));

	geRam_Free(P);
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
geBoolean Fire_Animate(Procedural *P, geFloat time)
{
	if(!FireAnimator_CreateFire(P))
		return GE_FALSE;

	(P->Frame)++;

	return GE_TRUE;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
static geBoolean FireAnimator_CreateFire(Procedural * Proc)
{
	geBitmap *FireLock = (geBitmap*)NULL;
	 geBitmap_Info FireInfo;
	uint8 *FireBits, *bPtr;
	int x, y, w, h, s;
	geBitmap *Bitmap;

	Bitmap = Proc->Bitmap;

	geBitmap_SetGammaCorrection(Bitmap, 1.0f, GE_FALSE);

	if(!geBitmap_LockForWriteFormat(Bitmap, &FireLock, 0, 0, GE_PIXELFORMAT_8BIT_PAL))
    {
		geBitmap_SetFormat(Bitmap, GE_PIXELFORMAT_8BIT_PAL, GE_TRUE, 0, (geBitmap_Palette*)NULL);
		geBitmap_LockForWriteFormat(Bitmap, &FireLock, 0, 0, GE_PIXELFORMAT_8BIT_PAL);

		if(!FireLock)
			return GE_FALSE;
    }

	if(!geBitmap_GetInfo(FireLock, &FireInfo, (geBitmap_Info*)NULL))
		goto fail;

	FireBits = (unsigned char*)geBitmap_GetBits(FireLock);

	if(!FireBits)
		goto fail;

	w = FireInfo.Width;
	h = FireInfo.Height;
	s = FireInfo.Stride;

	if(Proc->Frame == 0)
		memset(FireBits, 0, s*h);

	{
		int r;
		uint8 *bPtr;

		bPtr = FireBits + (h - SeedLines)*s;

		// seed hot points
		memset(bPtr, 0, s*SeedLines);

		for(r=20*SeedLines; r--;)
		{
			x = myrand(s*SeedLines);
			bPtr[x] = 254;
		}
	}

	// clear some points to avoid static junk
	{
		int r;
		uint8 * bPtr;

		for(y=0; y<h; y++)
		{
			bPtr = FireBits + y*s;

			for(r=10; r--;)
			{
				x = myrand(w);

				if(bPtr[x])
				{
					bPtr[x] -= ((bPtr[x])>>4) + 1;
				}
			}
		}
	}

	if(Proc->DoJets)
	{
		uint8 *pbPtr, *ppbPtr;
		int a;
		int randval, randbits;

		// now do a diffusion bleed
		randbits = 0;

		if(SeedLines < 2)
		{
			memcpy(FireBits + (h-2)*s, FireBits + (h-1)*s, s);
		}

		bPtr = FireBits + s;
		for(y=h-3; y--;)
		{
			pbPtr	=  bPtr + s;
			ppbPtr	= pbPtr + s;

			for(x=w; x--;)
			{
				a = (bPtr[-1] + pbPtr[0] + pbPtr[1] + pbPtr[-1] + pbPtr[-2] +
					ppbPtr[0] + ppbPtr[1] + ppbPtr[-1])>>3;

				if(a)
				{
					if(!randbits)
					{
						randval = myrand(1024);
						randbits = 10;
					}

					switch(randval & 1)
					{
					case 0:
						*bPtr = (a + pbPtr[1])>>1;
						break;
					case 1:
						*bPtr = (a + pbPtr[-1])>>1;
						break;
					}

					randval >>= 1;
					randbits --;
				}
				else
				{
					*bPtr = 0;
				}

				bPtr++;
				pbPtr++;
				ppbPtr++;
			}

			bPtr += s - w;
		}
	}
	else
	{
		FireInfo.Height --;

		if(!geBitmapUtil_SmoothBits(&FireInfo, FireBits + s, FireBits, 2, SMOOTH_WRAP))
			goto fail;
	}

	// fix up top lines
	{
		uint8 *sPtr;
		int passes,MaxY;

		passes = 2;

		if(Proc->DoJets)
			MaxY = 4;
		else
			MaxY = 2;

		while(passes--)
		{
			for(y=1; y<=MaxY; y++)
			{
				bPtr = FireBits + (h-y)*s;
				sPtr = bPtr - s;
				*bPtr++ = (bPtr[0] + bPtr[1] + sPtr[0] + sPtr[1])>>2;
				sPtr++;

				for(x=w-2; x--;)
				{
					*bPtr++ = (bPtr[0] + bPtr[1] + bPtr[-1] + sPtr[0] + sPtr[1] + sPtr[-1] + sPtr[-2] + sPtr[2])>>3;
					sPtr++;
				}

				*bPtr++ = (bPtr[0] + bPtr[-1] + sPtr[0] + sPtr[-1])>>2;
				sPtr++;
			}
		}
	}

	if(!geBitmap_UnLock(FireLock))
		goto fail;

	return GE_TRUE;

fail:

	if(FireLock)
		geBitmap_UnLock(FireLock);

	return GE_FALSE;
}


/* ----------------------------------- END OF FILE ------------------------------------ */
