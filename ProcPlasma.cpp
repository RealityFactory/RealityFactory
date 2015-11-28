/****************************************************************************************/
/*																						*/
/*	ProcPlasma.cpp																		*/
/*																						*/
/****************************************************************************************/

// Disable unsafe warning
#pragma warning (disable : 4996)

#include "ProcUtil.h"

#define	PI			(3.14159f)
#define SIZE_SCALE	(150.0)

#define R_PAL_STEP	(0.07)
#define G_PAL_STEP	(0.05)
#define B_PAL_STEP	(0.1)

#define BM_CREATE_WIDTH		(128)
#define BM_CREATE_HEIGHT	(128)
#define BM_MAX_WIDTH		(128)
#define BM_MAX_HEIGHT		(128)
#define BM_MIN_WIDTH		(128)
#define BM_MIN_HEIGHT		(128)

#define TAB_WIDTH	(BM_MAX_WIDTH<<1)
#define TAB_HEIGHT	(BM_MAX_HEIGHT<<1)

/**********************************************************/

typedef struct Procedural
{
	geBitmap	*Bitmap;
	geBoolean	PlasmaPalette,Displacer;
	geBitmap	*DisplaceOriginal;

	double		circle1,circle2,circle3,circle4,
				circle5,circle6,circle7,circle8;
	double		roll;
	double		R,G,B;
	double		CircleScale;
	double		RollStep;

} Procedural;

static geBoolean PlasmaAnimator_CreatePalette(Procedural * Proc, double time);
static geBoolean PlasmaAnimator_CreatePlasma(Procedural * Proc, double time);

/**************************************************************/

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
Procedural *Plasma_Create(char *TextureName, geWorld *World, const char *ParmStart)
{
    Procedural *P;
    geBoolean DoAlpha = GE_FALSE;
    geBitmap *ppBitmap;

	P = GE_RAM_ALLOCATE_STRUCT(Procedural);

	if(!P)
		return (Procedural*)NULL;

	memset(P, 0, sizeof(Procedural));

	ppBitmap = geWorld_GetBitmapByName(World, TextureName);

	if(!ppBitmap)
	{
		ppBitmap = geBitmap_Create(BM_CREATE_WIDTH, BM_CREATE_HEIGHT, 1, GE_PIXELFORMAT_8BIT_PAL);

		if(!ppBitmap)
		{
			geRam_Free(P);
			return (Procedural*)NULL;
		}
	}
	else
	{
		if(!geBitmap_SetFormat(ppBitmap, GE_PIXELFORMAT_8BIT_PAL, GE_FALSE, 0, (geBitmap_Palette*)NULL))
		{
			geRam_Free(P);
			return (Procedural*)NULL;
		}

		geBitmap_ClearMips(ppBitmap);
		geBitmap_CreateRef(ppBitmap);
	}

	if(geBitmap_Width(ppBitmap) > BM_MAX_WIDTH  ||
		geBitmap_Height(ppBitmap) > BM_MAX_HEIGHT ||
		geBitmap_Width( ppBitmap) < BM_MIN_WIDTH  ||
		geBitmap_Height(ppBitmap) < BM_MIN_HEIGHT )
	{
		geRam_Free(P);
		return (Procedural*)NULL;
	}

	P->Bitmap = ppBitmap;

	if(!ParmStart || strlen(ParmStart) < 10)
	{
		P->PlasmaPalette = GE_TRUE;

		if(!PlasmaAnimator_CreatePalette(P, 0.0))
		{
			geRam_Free(P);
			return (Procedural *)NULL;
		}

		P->CircleScale = 1.0;
		P->RollStep = 1.0;
		P->R = (rand() % 6)/6.0*PI;
		P->G = (rand() % 6)/6.0*PI;
		P->B = (rand() % 6)/6.0*PI;
	}
	else
	{
		char * pstr;
		char ParmWork[1024];

		P->PlasmaPalette = GE_FALSE;

		strcpy(ParmWork,ParmStart);
		pstr = strtok(ParmWork," ,\n\r\r");

		P->CircleScale = atof(pstr);
		pstr = strtok((char*)NULL," ,\n\r\r");

		P->RollStep = atof(pstr);
		pstr = strtok((char*)NULL," ,\n\r\r");

		DoAlpha = ((toupper(*pstr) == 'T') ? GE_TRUE : atol(pstr));
		pstr = strtok((char*)NULL," ,\n\r\r");

		P->Displacer = ((toupper(*pstr) == 'T') ? GE_TRUE : atol(pstr));
		pstr = strtok((char*)NULL," ,\n\r\r");

		if(!(P->Displacer))
		{
			ParmStart += (int)(pstr - ParmWork);

			if(!ProcUtil_SetPaletteFromString(P->Bitmap,(char**)&ParmStart))
			{
				geBitmap_Destroy(&(P->Bitmap));
				geRam_Free(P);
				return (Procedural*)NULL;
			}
		}
	}

	if(P->Displacer)
	{
		P->DisplaceOriginal = geBitmap_Create(BM_CREATE_WIDTH, BM_CREATE_HEIGHT, 1, GE_PIXELFORMAT_8BIT_PAL);

		if(!P->DisplaceOriginal)
		{
			geBitmap_Destroy(&(P->Bitmap));
			geRam_Free(P);
			return (Procedural*)NULL;
		}

		geBitmap_BlitBitmap(P->Bitmap,P->DisplaceOriginal);
	}

	if(DoAlpha)
	{
		// this is the beauty of 'set preferred format'
		//	the only difference between Plasma with and without alpha
		//	is this one call !!!
		if(!geBitmap_SetPreferredFormat(P->Bitmap, GE_PIXELFORMAT_16BIT_4444_ARGB))
			return GE_FALSE;
	}

	P->circle1 = ( rand() % 10 ) * 0.1;
	P->circle2 = ( rand() % 10 ) * 0.1;
	P->circle3 = ( rand() % 10 ) * 0.1;
	P->circle4 = ( rand() % 10 ) * 0.1;
	P->circle5 = ( rand() % 10 ) * 0.1;
	P->circle6 = ( rand() % 10 ) * 0.1;
	P->circle7 = ( rand() % 10 ) * 0.1;
	P->circle8 = ( rand() % 10 ) * 0.1;

    return P;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
void Plasma_Destroy(Procedural *P)
{
	if(!P)
		return;

	if(P->Bitmap)
		geBitmap_Destroy(&(P->Bitmap));

	if(P->DisplaceOriginal)
		geBitmap_Destroy(&(P->DisplaceOriginal));

	geRam_Free(P);
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
geBoolean Plasma_Animate(Procedural *P, float time)
{
	if(P->PlasmaPalette)
	{
		if(!PlasmaAnimator_CreatePalette(P, time))
			return GE_FALSE;
	}

	if(!PlasmaAnimator_CreatePlasma(P, time))
		return GE_FALSE;

    return GE_TRUE;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
static geBoolean PlasmaAnimator_CreatePalette(Procedural *P, double time)
{
	geBitmap_Palette *Pal;
	void *PalData = NULL;
	gePixelFormat PalFormat;
	int PalSize;

	if(!(Pal = geBitmap_GetPalette(P->Bitmap)))
	{
		Pal = geBitmap_Palette_Create(GE_PIXELFORMAT_32BIT_ARGB, 256);

		if(!Pal)
			goto fail;

		if(!geBitmap_SetPalette(P->Bitmap,Pal))
			goto fail;

		geBitmap_Palette_Destroy(&Pal);

		if(!(Pal = geBitmap_GetPalette(P->Bitmap)))
			return GE_FALSE;
	}

	if(!geBitmap_Palette_Lock(Pal, &PalData, &PalFormat, &PalSize))
		goto fail;

	if(PalSize < 256)
		goto fail;

	{
		int p,r,g,b;
		uint8 * PalPtr;
		double R,G,B;
		geFloat u;

		R = P->R;
		G = P->G;
		B = P->B;

		PalPtr = (unsigned char*)PalData;

		for(p=0; p<256; p++)
		{
			u = (geFloat)((PI/128.0) * p);

			#define mycol(u,a) (int)((cos((u)+(a))+1)*127.0)

			r = mycol(u,R);
			g = mycol(u,G);
			b = mycol(u,B);

			gePixelFormat_PutColor(PalFormat,&PalPtr,r,g,b,0xFF);
		}

		P->R += R_PAL_STEP * time * 30.0;
		P->G -= G_PAL_STEP * time * 30.0;
		P->B += B_PAL_STEP * time * 30.0;
	}

	PalData = NULL;

	if(!geBitmap_Palette_UnLock(Pal))
		return GE_FALSE;

	return GE_TRUE;

fail:

	if(PalData)
		geBitmap_Palette_UnLock(Pal);

	return GE_FALSE;
}

static uint8 tab1[TAB_WIDTH * TAB_HEIGHT];
static uint8 tab2[TAB_WIDTH * TAB_HEIGHT];
static uint8 sintab1[256];
static signed char dtable[256];
static int calculated = 0;

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
static void CalculateTables(Procedural * Proc)
{
	int x,y;
	uint8 *ptab1,*ptab2;
	if(calculated)
		return;

	calculated = 1;
	ptab1 = tab1;
	ptab2 = tab2;

	for(y=0; y<TAB_HEIGHT; y++)
	{
		for(x=0; x<TAB_WIDTH; x++)
		{
			double d_to_center, r, dx, dy;

			dx = SIZE_SCALE * sin(x*PI/128.0);
			dy = SIZE_SCALE * cos(y*PI/128.0);

			r = 0.4;

			d_to_center = sqrt(16.0 + dx*dx + dy*dy ) - 4.0;
			*ptab1++ = (uint8)(d_to_center * 5.0 + r);
			*ptab2++= (uint8)((sin(d_to_center/9.5)+1.0)*90.0 + r);
		}
	}

	for(x=0; x<256; x++)
	{
		sintab1[x] = (uint8)((sin(x/(5.0*9.5))+1.0)*90.0 + 0.5);
		dtable[x] = (signed char)(sin(x*2.0*PI/256) * 16.0);
	}
}

static int DoCircle(double *pCircle, double step, int w, int doCos);

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
static geBoolean PlasmaAnimator_CreatePlasma(Procedural *Proc, double time)
{
	geBitmap *PlasmaLock = (geBitmap*)NULL;
	geBitmap_Info PlasmaInfo;
	uint8 *Bits, *bptr;
	int x, y, w, h, s;
	geBitmap *Bitmap;

	Bitmap = Proc->Bitmap;
	CalculateTables(Proc);

	geBitmap_SetGammaCorrection(Bitmap, 1.0f, GE_FALSE);

	if(!geBitmap_LockForWriteFormat(Bitmap,&PlasmaLock,0,0,GE_PIXELFORMAT_8BIT_PAL))
	{
		geBitmap_SetFormat(Bitmap, GE_PIXELFORMAT_8BIT_PAL, GE_TRUE, 0, (geBitmap_Palette*)NULL);
		geBitmap_LockForWriteFormat(Bitmap, &PlasmaLock, 0, 0, GE_PIXELFORMAT_8BIT_PAL);

		if(PlasmaLock == NULL)
		goto fail;
	}

	if(!geBitmap_GetInfo(PlasmaLock, &PlasmaInfo, (geBitmap_Info*)NULL))
		goto fail;

	Bits = (unsigned char*)geBitmap_GetBits(PlasmaLock);

	if(!Bits)
		goto fail;

	w = PlasmaInfo.Width;
	h = PlasmaInfo.Height;
	s = PlasmaInfo.Stride;

	// could do every other pixel & smooth
	// main advantage : smaller table

	{
		int	x1, y1, x2, y2, x3, y3, x4, y4;
		int o1, o2, o3, o4;
		uint8 *ptab1, *ptab2;
		uint8 roll;
		double CircleScale;

		CircleScale = Proc->CircleScale * time * 2.0;

		x1 = DoCircle(&(Proc->circle1),0.3  * CircleScale,w,1);
		y1 = DoCircle(&(Proc->circle2),0.2  * CircleScale,h,0);
		y2 = DoCircle(&(Proc->circle4),0.1  * CircleScale,h,1);
		x2 = DoCircle(&(Proc->circle3),0.85 * CircleScale,w,0);
		x3 = DoCircle(&(Proc->circle5),0.4  * CircleScale,w,1);
		y3 = DoCircle(&(Proc->circle6),0.15 * CircleScale,h,0);
		x4 = DoCircle(&(Proc->circle7),0.35 * CircleScale,w,1);
		y4 = DoCircle(&(Proc->circle8),0.05 * CircleScale,h,0);

		o1 = x1 + TAB_WIDTH*y1;
		o2 = x2 + TAB_WIDTH*y2;
		o3 = x3 + TAB_WIDTH*y3;
		o4 = x4 + TAB_WIDTH*y4;

		roll = (uint8)( Proc->roll + 0.5);
		Proc->roll += Proc->RollStep * time * 30.0;

		// roll uses the fact that we work modulo 255

		// this is the inner loop;
		// it's very lean, but also not blazing fast, cuz
		//  this is very bad on the cache
		// we have we have FIVE different active segments of memory,
		//	(bptr,ptab1+o1,ptab2+o2,ptab2+o3,ptab2+o4) all of which are
		// too large to fit in the L1 cache, so each mem-hit is 4 clocks

		if(Proc->Displacer)
		{
			geBitmap *Source, *SourceLock;
			geBitmap_Info SourceInfo;
			void *SourceBits;
			int ss;
			uint8 *sptr;

			// instensity is a displacement
			Source = Proc->DisplaceOriginal;

			if(!geBitmap_LockForReadNative(Source, &SourceLock, 0, 0))
				goto fail;

			if(!geBitmap_GetInfo(SourceLock, &SourceInfo, (geBitmap_Info*)NULL))
				goto fail;

			if(!(SourceBits = geBitmap_GetBits(SourceLock)))
				goto fail;

			ss = SourceInfo.Stride;

			bptr = (unsigned char*)Bits;
			sptr = (unsigned char*)SourceBits;

			for(y=0; y<h; y++)
			{
				ptab1 =	tab1 + TAB_WIDTH*y;
				ptab2 =	tab2 + TAB_WIDTH*y;

				for(x=0; x<w; x++)
				{
					int o, sx;

					o = ptab1[o1] + ptab2[o2] + ptab2[o3] + ptab2[o4];
					o = (o + roll)&0xFF;
					o = dtable[o];

					ptab1++;
					ptab2++;

					sx = x + o;

					if(sx < 0)
						sx += w;
					else if(sx >= w)
						sx -= w;

					*bptr++ = sptr[sx];
				}

				bptr += (s-w);
				sptr += ss;
			}

			geBitmap_UnLock(SourceLock);
		}
		else
		{
			// classic style
			bptr = Bits;

			if(PlasmaInfo.HasColorKey)
			{
				uint8 CK,pel;
				CK = (uint8)PlasmaInfo.ColorKey;

				if(CK > 200)
				{
					for(y=0; y<h; y++)
					{
						ptab1 =	tab1 + TAB_WIDTH*y;
						ptab2 =	tab2 + TAB_WIDTH*y;

						for(x=w; x--;)
						{
							pel = roll + ptab1[o1] + ptab2[o2] + ptab2[o3] + ptab2[o4];

							*bptr++ = pel%CK;
							ptab1++;
							ptab2++;
						}

						bptr += (s-w);
					}
				}
				else
				{
					for(y=0; y<h; y++)
					{
						ptab1 =	tab1 + TAB_WIDTH*y;
						ptab2 =	tab2 + TAB_WIDTH*y;

						for(x=w; x--;)
						{
							pel = roll + ptab1[o1] + ptab2[o2] + ptab2[o3] + ptab2[o4];

							if(pel == CK)
								pel ^= 1;

							*bptr++ = pel;
							ptab1++;
							ptab2++;
						}

						bptr += (s-w);
					}
				}
			}
			else
			{
				for(y=0; y<h; y++)
				{
					ptab1 =	tab1 + TAB_WIDTH*y;
					ptab2 =	tab2 + TAB_WIDTH*y;

					for(x=w; x--;)
					{
						*bptr++ = roll + ptab1[o1] + ptab2[o2] + ptab2[o3] + ptab2[o4];
						ptab1++;
						ptab2++;
					}

					bptr += (s-w);
				}
			}
		}
	}

	if(!geBitmap_UnLock(PlasmaLock))
		goto fail;

	return GE_TRUE;

fail:

	if(PlasmaLock)
		geBitmap_UnLock(PlasmaLock);

	return GE_FALSE;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
static int DoCircle(double *pCircle, double step, int w, int doCos)
{
	double cp, c, cn;
	double xp, x, xn;
	int ix, ixp, ixn;

	w--;

	// try to do temporal anti-aliasing
	// <> doesn't seem to help
	c = *pCircle;
	cp = c - step;
	cn = c + step;

	*pCircle += step;

	if(doCos)
	{
		x  = w*0.5*(1.0 + cos(c));
		xp = w*0.5*(1.0 + cos(cp));
		xn = w*0.5*(1.0 + cos(cn));
	}
	else
	{
		x  = w*0.5*(1.0 + sin(c));
		xp = w*0.5*(1.0 + sin(cp));
		xn = w*0.5*(1.0 + sin(cn));
	}

	ixn = (int)xn;
	ixp = (int)xp;

	if(ixp == ixn)
	{
		ix = ixp;
	}
	else
	{
		x += ( rand() % 50 ) * 0.01;
		ix = (int)((x + ixp + ixn)*(1.0/3.0));
	}

	return ix;
}

/* ----------------------------------- END OF FILE ------------------------------------ */
