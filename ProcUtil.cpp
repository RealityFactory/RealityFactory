/************************************************************************************//**
 * @file ProcUtil.cpp
 * @brief Utility functions for procedural textures
 ****************************************************************************************/

// Disable unsafe warning
#pragma warning (disable : 4996)

#include "ProcUtil.h"

#define TWO_PI (6.28318530718f)

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
geBoolean geBitmapUtil_SmoothBits(geBitmap_Info *pInfo, void *FmBits, void *ToBits,
								  int radius, geBoolean wrap)
{
	int bpp, x, y, w, h, s;

	if(radius > 3)
		radius = 3;

	bpp = gePixelFormat_BytesPerPel(pInfo->Format);

	w = pInfo->Width;
	h = pInfo->Height;
	s = pInfo->Stride;

	switch(bpp)
	{
	case 0:
		return GE_FALSE;
	case 1:
		{
			uint8 *pSrc, *pDst;

			pSrc = static_cast<uint8*>(FmBits);
			pDst = static_cast<uint8*>(ToBits);

			if(wrap)
			{
				uint8 *pSrcN, *pSrcP;
				switch(radius)
				{
				case 1:
					for(y=0; y<h; ++y)
					{
						pSrcN = pSrc+s;
						pSrcP = pSrc-s;

						if(y == 0)
						{
							pSrcP = pSrc + s*(h-1);
						}
						else if(y == (h-1))
						{
							pSrcN = static_cast<uint8*>(FmBits);
						}

						//first pel
						*pDst++ = (pSrc[w-1] + pSrc[1] + *pSrcN + *pSrcP)>>2;
						++pSrc;
						++pSrcN;
						++pSrcP;

						for(x=w-2; x--;)
						{
							*pDst++ = (pSrc[-1] + pSrc[1] + *pSrcN + *pSrcP)>>2;
							++pSrc;
							++pSrcN;
							++pSrcP;
						}

						// last pel
						*pDst++ = (pSrc[-1] + pSrc[1-w] + *pSrcN + *pSrcP)>>2;
						++pSrc;
						++pSrcN;
						++pSrcP;

						pDst += (s-w);
						pSrc += (s-w);
					}
					break;
				default:
				case 2:
					for(y=0; y<h; ++y)
					{
						pSrcN = pSrc+s;
						pSrcP = pSrc-s;

						if(y == 0)
						{
							pSrcP = pSrc + s*(h-1);
						}
						else if(y == (h-1))
						{
							pSrcN = static_cast<uint8*>(FmBits);
						}

						//first pel
						*pDst++ = (pSrcN[-1] + pSrc[1] + *pSrcN + *pSrcP)>>2;
						++pSrc;
						++pSrcN;
						++pSrcP;

						for(x=w-2; x--;)
						{
							*pDst++ = (pSrc[-1] + pSrc[1] + pSrcN[0] + pSrcP[0] +
										pSrcN[1] + pSrcN[-1] + pSrcP[1] + pSrcP[-1])>>3;
							++pSrc;
							++pSrcN;
							++pSrcP;
						}

						// last pel
						*pDst++ = (pSrc[-1] + pSrcP[1] + *pSrcN + *pSrcP)>>2;
						++pSrc;
						++pSrcN;
						++pSrcP;

						pDst += (s-w);
						pSrc += (s-w);
					}
					break;
				}
			}
			else
			{
				switch(radius)
				{
				case 1:
					// first line
					*pDst++ = (pSrc[1] + pSrc[s])>>1;
					++pSrc;
					for(x=w-1; x--;)
					{
						*pDst++ = (pSrc[-1] + pSrc[1] + pSrc[s]+ pSrc[s-1])>>2;
						++pSrc;
					}
					pDst += (s-w);
					pSrc += (s-w);

					for(y=h-2; y--;)
					{
						// middle lines
						x = w;

						__asm
						{
							mov esi, pSrc
							mov edi, pDst

							// something like 10 clocks per byte = molto bene

						mainloop:
							xor eax,eax						// load the sum into eax

							movzx edx,BYTE PTR [esi+1]		// port 2, latency 1
							add eax,edx						// port 1, latency 1

							movzx edx,BYTE PTR [esi-1]
							add eax,edx

							mov ecx,esi						// ecx = esi + s
							add ecx,s
							movzx edx,BYTE PTR [ecx]		// eax += (BYTE) [ecx]
							add eax,edx						// we have to use edx cuz of the byte

							mov ecx,esi
							sub ecx,s
							movzx edx,BYTE PTR [ecx]
							add eax,edx

							shr eax,2

							mov [edi],al

							inc edi
							inc esi
							dec x

							jnz mainloop
						}

						pDst += s;
						pSrc += s;

					}

					// last line
					for(x=w-1; x--;)
					{
						*pDst++ = (pSrc[-1] + pSrc[1] + pSrc[-s]+ pSrc[-s+1])>>2;
						++pSrc;
					}
					*pDst++ = (pSrc[-1] + pSrc[-s])>>1;
					++pSrc;
					pSrc += (s-w);
					pDst += (s-w);
					break;
				default:
				case 2:
					// first line
					*pDst++ = (pSrc[1] + pSrc[s])>>1;
					++pSrc;
					for(x=w-1; x--;)
					{
						*pDst++ = (pSrc[-1] + pSrc[1] + pSrc[s]+ pSrc[s-1])>>2;
						++pSrc;
					}
					pSrc += (s-w);
					pDst += (s-w);

					for(y=h-2; y--;)
					{
						*pDst++ = (pSrc[-1] + pSrc[1] + pSrc[s] + pSrc[-s])>>2;
						++pSrc;

						x = w-2;

						__asm
						{
							mov esi, pSrc
							//lea esi, [pSrc]
							mov edi, pDst
							//lea edi, [pDst]

						mainloop2:
							xor eax,eax						// load the sum into eax

							movzx edx,BYTE PTR [esi+1]		// eax += (BYTE) esi[1]
							add eax,edx						// we have to use edx cuz of the byte

							movzx edx,BYTE PTR [esi-1]
							add eax,edx

							mov ecx,esi						// ecx = esi + s
							add ecx,s
							movzx edx,BYTE PTR [ecx-1]
							add eax,edx
							movzx edx,BYTE PTR [ecx]
							add eax,edx
							movzx edx,BYTE PTR [ecx+1]
							add eax,edx

							mov ecx,esi
							sub ecx,s
							movzx edx,BYTE PTR [ecx-1]
							add eax,edx
							movzx edx,BYTE PTR [ecx]
							add eax,edx
							movzx edx,BYTE PTR [ecx+1]
							add eax,edx

							shr eax,3

							mov [edi],al

							inc edi
							inc esi
							dec x

							jnz mainloop2
						}

						pDst += w-2;
						pSrc += w-2;

						*pDst++ = (pSrc[-1] + pSrc[1] + pSrc[s] + pSrc[-s])>>2;
						++pSrc;
						pSrc += (s-w);
						pDst += (s-w);
					}
					// last line
					for(x=w-1;x--;)
					{
						*pDst++ = (pSrc[-1] + pSrc[1] + pSrc[-s]+ pSrc[-s+1])>>2;
						++pSrc;
					}
					*pDst++ = (pSrc[-1] + pSrc[-s])>>1;
					++pSrc;
					pSrc += (s-w);
					pDst += (s-w);
					break;
				}
			}
			break;
		}
	case 2:
	case 3:
	case 4:
		return GE_FALSE;
	}

	return GE_TRUE;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
geBoolean geBitmapUtil_SetColor(geBitmap *Bmp, int R, int G, int B, int A)
{
	geBitmap *Lock = NULL;
	geBitmap_Info Info;
	void *Bits;
	int w, h, s, bpp, x;

	if(!geBitmap_LockForWrite(Bmp, &Lock, 0, 0))
		goto fail;

	if(!geBitmap_GetInfo(Lock, &Info, NULL))
		goto fail;

	Bits = geBitmap_GetBits(Lock);

	if(!Bits)
		goto fail;

	bpp = gePixelFormat_BytesPerPel(Info.Format);

	w = Info.Width;
	h = Info.Height;
	s = Info.Stride;

	switch(bpp)
	{
	default:
		goto fail;
	case 1:
		{
			uint8 *pBits, Pixel;
			pBits = static_cast<uint8*>(Bits);
			Pixel = (R + G + B) / 3;

			while(h--)
			{
				for(x=w; x--;)
				{
					*pBits++ = Pixel;
				}

				pBits += (s-w);
			}

			break;
		}
	case 2:
		{
			uint16 *pBits, Pixel;
			pBits = static_cast<uint16*>(Bits);
			Pixel = static_cast<uint16>(gePixelFormat_ComposePixel(Info.Format, R, G, B, A));

			while(h--)
			{
				for(x=w; x--;)
				{
					*pBits++ = Pixel;
				}

				pBits += (s-w);
			}

			break;
		}
	case 4:
		{
			uint32 *pBits,Pixel;
			pBits = static_cast<uint32*>(Bits);
			Pixel = static_cast<uint32>(gePixelFormat_ComposePixel(Info.Format, R, G, B, A));

			while(h--)
			{
				for(x=w; x--;)
				{
					*pBits++ = Pixel;
				}

				pBits += (s-w);
			}

			break;
		}
	case 3:
		{
			uint8 * pBits,b1,b2,b3;
			uint32 Pixel;
			pBits = static_cast<uint8*>(Bits);
			Pixel = static_cast<uint32>(gePixelFormat_ComposePixel(Info.Format, R, G, B, A));
			b1 = static_cast<uint8>((Pixel >> 16) & 0xFF);
			b2 = static_cast<uint8>((Pixel >>  8) & 0xFF);
			b3 = static_cast<uint8>((Pixel >>  0) & 0xFF);

			while(h--)
			{
				for(x=w; x--;)
				{
					*pBits++ = b1;
					*pBits++ = b2;
					*pBits++ = b3;
				}

				pBits += 3*(s-w);
			}

			break;
		}
	}

	if(!geBitmap_UnLock(Lock))
		goto fail;

	geBitmap_RefreshMips(Bmp);

	return GE_TRUE;

fail:

	if(Lock)
		geBitmap_UnLock(Lock);

	return GE_FALSE;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
geBoolean ProcUtil_SetPaletteFromString(geBitmap *Bitmap, char **pParams)
{
	geBitmap_Info Info;
	geBitmap_Palette *Pal;
	void *PalData = NULL;
	gePixelFormat PalFormat;
	int PalSize;

	char ParamWork[1024];
	char *pstr;
	const char * strbreakers = " ,\t\n\r\34\39\09";

#define nextparam()	do{ if(!(pstr)) { goto fail; }; (pstr) = strtok(NULL, strbreakers); } while(0)
#define getint()	((pstr) == NULL) ? 0 : atol(pstr); nextparam();
#define getfloat()	((pstr) == NULL) ? 0.0f : static_cast<geFloat>(atof(pstr)); nextparam();

	if(pParams)
	{
		strcpy(ParamWork, *pParams);
	}
	else
	{
		strcpy(ParamWork, "list, 3, 0,0,0,0, 200,50,0,100, 255,100,50,255");
	}

	pstr = strtok(ParamWork, strbreakers);

	Pal = geBitmap_Palette_Create(GE_PIXELFORMAT_32BIT_ARGB, 256);

	if(!Pal)
		goto fail;

	if(!geBitmap_SetPalette(Bitmap, Pal))
		goto fail;

	geBitmap_Palette_Destroy(&Pal);

	geBitmap_GetInfo(Bitmap, &Info, NULL);

	if(!(Pal = geBitmap_GetPalette(Bitmap)))
		goto fail;

	if(!geBitmap_Palette_Lock(Pal, &PalData, &PalFormat, &PalSize))
		goto fail;

	if(PalSize < 256)
		goto fail;

	if(strnicmp(pstr, "list", 4) == 0)
	{
		int p, lp, r, g, b, a;
		uint8 *PalPtr;
		int NumColors;
		int pr, pg, pb, pa;
		int nr, ng, nb, na;
		double cstep, nextc, icstep;

		nextparam();
		NumColors = getint();

		if(NumColors == 0)
		{
			*pParams += (int)(pstr - ParamWork);
			pParams = NULL;
			strcpy(ParamWork, "list, 3, 0,0,0,0, 200,50,0,100, 255,100,50,255");
		}
		else if(NumColors < 2)
			goto fail;

		cstep = 256.0 / static_cast<double>(NumColors - 1);
		nextc = 0.0;
		icstep = 1.0 / cstep;

		--NumColors;
		nr = getint();
		ng = getint();
		nb = getint();
		na = getint();

		PalPtr = static_cast<uint8*>(PalData);

		for(p=0; p<256; ++p)
		{
			if(p >= static_cast<int>(nextc))
			{
				pr = nr;
				pg = ng;
				pb = nb;
				pa = na;
				nr = getint();
				ng = getint();
				nb = getint();
				na = getint();
				nextc += cstep;
				lp = p;
			}

			// <> use Spline or Bezier instead !

			r = (int)(pr + (nr - pr)*(p - lp)*icstep);
			g = (int)(pg + (ng - pg)*(p - lp)*icstep);
			b = (int)(pb + (nb - pb)*(p - lp)*icstep);
			a = (int)(pa + (na - pa)*(p - lp)*icstep);

			gePixelFormat_PutColor(PalFormat, &PalPtr, min(r,255), min(g,255), min(b,255), min(a,255));
		}
	}
	else if(strnicmp(pstr, "pow", 4) == 0)
	{
		int p, r, g, b, a;
		uint8 * PalPtr;
		double pr, pg, pb, pa, frac;
		int nr, ng, nb, na;

		nextparam();

		nr = getint();
		ng = getint();
		nb = getint();
		na = getint();

		if(pstr)
		{
			pr = getfloat();
			pg = getfloat();
			pb = getfloat();
			pa = getfloat();
		}
		else
		{
			pr = pg = pb = pa = 0.5;
		}

		PalPtr = static_cast<uint8*>(PalData);

		for(p=0; p<256; ++p)
		{
			frac = (double)p * (1.0/256.0);
			r = (int)( nr * pow( frac, pr) );
			g = (int)( ng * pow( frac, pg) );
			b = (int)( nb * pow( frac, pb) );
			a = (int)( na * pow( frac, pa) );
			gePixelFormat_PutColor(PalFormat,&PalPtr,min(r,255),min(g,255),min(b,255),min(a,255));
		}
	}
	else if(strnicmp(pstr, "trig", 4) == 0)
	{
		int p, r, g, b, a;
		uint8 * PalPtr;
		geFloat r_mult, g_mult, b_mult, a_mult;
		geFloat r_freq, g_freq, b_freq, a_freq;
		geFloat r_base, g_base, b_base, a_base;
		geFloat frac;

		nextparam();

		r_mult = getfloat();
		g_mult = getfloat();
		b_mult = getfloat();
		a_mult = getfloat();

		r_freq = getfloat();
		g_freq = getfloat();
		b_freq = getfloat();
		a_freq = getfloat();

		r_base = getfloat(); r_base *= TWO_PI;
		g_base = getfloat(); g_base *= TWO_PI;
		b_base = getfloat(); b_base *= TWO_PI;
		a_base = getfloat(); a_base *= TWO_PI;

		PalPtr = static_cast<uint8*>(PalData);

		for(p=0; p<256; ++p)
		{
			frac = static_cast<geFloat>(p) * (TWO_PI/256.0f);
			r = (int)( r_mult * (cos( r_freq * frac + r_base ) + 1.0) );
			g = (int)( g_mult * (cos( g_freq * frac + g_base ) + 1.0) );
			b = (int)( b_mult * (cos( b_freq * frac + b_base ) + 1.0) );
			a = (int)( a_mult * (cos( a_freq * frac + a_base ) + 1.0) );
			gePixelFormat_PutColor(PalFormat,&PalPtr,min(r,255),min(g,255),min(b,255),min(a,255));
		}
	}

	PalData = NULL;

	if(!geBitmap_Palette_UnLock(Pal))
		return GE_FALSE;

	if(pParams)
	{
		*pParams += (int)pstr - (int)ParamWork;
	}

	return GE_TRUE;

fail:

	if(PalData)
		geBitmap_Palette_UnLock(Pal);

	return GE_FALSE;
}

static uint32 lastRandomNumber = 0xA2A9; // that's a prime

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
void ProcUtil_Randomize(void)
{
	uint32 cnt;
	cnt = clock();
	cnt &= 0xFFFF;

	while(cnt--)
	{
		lastRandomNumber = lastRandomNumber*65539+3;
		lastRandomNumber = lastRandomNumber*1009 +7;
	}
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
uint32 ProcUtil_Rand(uint32 max)
{
	uint32 a;

	lastRandomNumber = lastRandomNumber*65539+3;

	a = lastRandomNumber>>16;

	return a%max;
}


/* ----------------------------------- END OF FILE ------------------------------------ */
