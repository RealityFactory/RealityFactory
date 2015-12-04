/************************************************************************************//**
 * @file ProcWater.cpp
 * @brief Procedural Water
 ****************************************************************************************/

#include "ProcUtil.h"

typedef struct Procedural
{
	geFloat		PosX;
	geFloat		PosY;
	geFloat		Angle;
	geFloat		TimeToSplashWater;

	int32		NPage;

	uint16		*OriginalBits;
	int16		*WaterData[2];

	geBitmap	*Bitmap;

	int32		Width;
	int32		Height;
	int32		Size;

	uint8		BlendLut[32][32];

} Procedural;

void Water_Destroy(Procedural *Water);

static void Water_BuildRGBLuts(Procedural *Water, geFloat RScale, geFloat GScale, geFloat BScale);
static geBoolean Water_ApplyToBitmap(Procedural *Water);
static void Water_Update(Procedural *Water, geFloat Time);

/* ------------------------------------------------------------------------------------ */
//	Water_Create
/* ------------------------------------------------------------------------------------ */
Procedural *Water_Create(char *TextureName, geWorld *World)
{
	Procedural *Water;

	Water = GE_RAM_ALLOCATE_STRUCT(Procedural);

	if(!Water)
	{
		return NULL;
	}

	memset(Water, 0, sizeof(*Water));

	Water->Bitmap = geWorld_GetBitmapByName(World, TextureName);

	if(Water->Bitmap == NULL)
	{
// changed QD 12/15/05
		if(Water)
		    Water_Destroy(Water);
// end change

		return NULL;
	}

	geBitmap_CreateRef(Water->Bitmap);

	// We need this to be only 1 miplevel!!!
	if(!geBitmap_ClearMips(Water->Bitmap))
	{
		if(Water)
			Water_Destroy(Water);

		return NULL;
	}
	else
	{
		// We need to change the format of this bitmap to a 565 in the world (hope he doesn't mind ;)
		gePixelFormat	Format;
		geBitmap_Info	Info;
		int32			i;

		Format = GE_PIXELFORMAT_16BIT_565_RGB;

		if(!geBitmap_SetFormat(Water->Bitmap, Format, GE_FALSE, 0, NULL))
		{
			if(Water)
				Water_Destroy(Water);

			return NULL;
		}

		if(!geBitmap_GetInfo(Water->Bitmap, &Info, NULL))
		{
			if(Water)
				Water_Destroy(Water);

			return NULL;
		}

		Water->Width = Info.Width;
		Water->Height = Info.Height;
		Water->Size = Water->Width*Water->Height;

		Water->OriginalBits = GE_RAM_ALLOCATE_ARRAY(uint16, Water->Size);

		if(!Water->OriginalBits)
		{
			if(Water)
				Water_Destroy(Water);

			return NULL;
		}

		for(i=0; i<2; i++)
		{
			Water->WaterData[i] = GE_RAM_ALLOCATE_ARRAY(int16, Water->Size);

			if(!Water->WaterData[i])
			{
				if (Water)
					Water_Destroy(Water);

				return NULL;
			}

			memset(Water->WaterData[i], 0, sizeof(int16)*Water->Size);
		}

	}

	// Get the original bits...
	{
		geBitmap		*Src;
		uint16			*pSrc16;
		int32			i;
		geBitmap_Info	Info;

		if(!geBitmap_GetInfo(Water->Bitmap, &Info, NULL))
		{
			if(Water)
				Water_Destroy(Water);

			return NULL;
		}

		if(!geBitmap_LockForRead(Water->Bitmap, &Src, 0, 0, Info.Format, GE_TRUE, 255))
		{
			if(Water)
				Water_Destroy(Water);

			return NULL;
		}

		pSrc16 = static_cast<uint16*>(geBitmap_GetBits(Src));

		for(i=0; i<Water->Size; i++)
			Water->OriginalBits[i] = pSrc16[i];

		if(!geBitmap_UnLock(Src))
		{
			if(Water)
				Water_Destroy(Water);

			return NULL;
		}
	}

	Water_BuildRGBLuts(Water, 1.0f, 1.0f, 1.0f);

	return Water;
}


/* ------------------------------------------------------------------------------------ */
//	Water_Destroy
/* ------------------------------------------------------------------------------------ */
void Water_Destroy(Procedural *Water)
{
	if(Water->Bitmap)
	{
		geBitmap_Destroy(&(Water->Bitmap));
		Water->Bitmap = NULL;
	}

	if(Water->OriginalBits)
		geRam_Free(Water->OriginalBits);
	Water->OriginalBits = NULL;

	if(Water->WaterData[0])
		geRam_Free(Water->WaterData[0]);
	Water->WaterData[0] = NULL;

	if(Water->WaterData[1])
		geRam_Free(Water->WaterData[1]);
	Water->WaterData[1] = NULL;

	geRam_Free(Water);
}

/* ------------------------------------------------------------------------------------ */
//	Water_BuildRGBLuts
/* ------------------------------------------------------------------------------------ */
static void Water_BuildRGBLuts(Procedural *Water, geFloat RScale, geFloat GScale, geFloat BScale)
{
	int32		i, j;

	for(i=0; i<32; i++)		// Shade
	{
		for(j=0; j<32; j++)	// Color
		{
			int32	Val;

			Val = (int32)(((geFloat)(31 - i)/14) * (geFloat)j);

			//Val <<= 2;

			if(Val > 31)
				Val = 31;

			Water->BlendLut[i][j] = static_cast<uint8>(Val);
		}
	}
}

/* ------------------------------------------------------------------------------------ */
//	Water_Animate
/* ------------------------------------------------------------------------------------ */
geBoolean Water_Animate(Procedural *Water, geFloat ElapsedTime)
{
	if(!Water->Bitmap)
		return GE_TRUE;

	Water_Update(Water, ElapsedTime);

	if(!Water_ApplyToBitmap(Water))
		return GE_FALSE;

	return GE_TRUE;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
static geBoolean Water_ApplyToBitmap(Procedural *Water)
{
	geBitmap	*Dest;
	uint8		*pBlendLut;

	if(!geBitmap_LockForWriteFormat(Water->Bitmap, &Dest, 0, 0, GE_PIXELFORMAT_16BIT_565_RGB))
	{
		geBitmap_SetFormat(Water->Bitmap, GE_PIXELFORMAT_16BIT_565_RGB,GE_TRUE, 0, NULL);
		geBitmap_LockForWriteFormat(Water->Bitmap, &Dest, 0, 0, GE_PIXELFORMAT_16BIT_565_RGB);

		if(!Dest)
			return GE_FALSE;
	}

	pBlendLut = &Water->BlendLut[0][0];

	{
		uint16			*pSrc16, *pDest16;
		int16			*pWSrc16, *pOriginalWSrc16;
		int32			w, h, Extra, WMask, HMask;
		geBitmap_Info	Info;

		if(!geBitmap_GetInfo(Dest, &Info, NULL))
			return GE_FALSE;

		Extra = Info.Stride - Info.Width;

		WMask = Info.Width - 1;
		HMask = Info.Height - 1;

		pSrc16 = Water->OriginalBits;
		pOriginalWSrc16 = pWSrc16 = Water->WaterData[Water->NPage];
		pDest16 = static_cast<uint16*>(geBitmap_GetBits(Dest));

		// For the love of God, write this in assembly
		for(h=0; h<Info.Height; h++)
		{
			for(w=0; w<Info.Width; w++)
			{
				int32	x, y, Val;
				uint16	r, g, b;
				uint16	Color;

				Val = pWSrc16[w];

				if(h < Info.Height-1)
					y = Val - pWSrc16[w+Info.Stride];
				else
					y = Val - pOriginalWSrc16[w];

				x = Val - pWSrc16[(w+1)&WMask];
				Val = 127 - (y<<4);

				if(Val < 0)
					Val = 0;
				else if(Val > 255)
					Val = 255;

				Val >>= 3;
				Val <<= 5;

				x >>= 4;
				y >>= 4;

				Color = pSrc16[((h+y)&HMask)*Info.Stride + ((w+x)&WMask)];

				r = static_cast<uint16>(pBlendLut[Val + ((Color >> 11) & 31)]);
				g = static_cast<uint16>(pBlendLut[Val + ((Color >>  6) & 31)]);
				b = static_cast<uint16>(pBlendLut[Val + ( Color        & 31)]);

				*pDest16++ = (r<<11) | (g<<6) | b;
			}

			pDest16 += Extra;
			pWSrc16 += Info.Stride;//Extra;
		}

		if(!geBitmap_UnLock(Dest))
		{
			return GE_FALSE;
		}
	}

	return GE_TRUE;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
static void CalcRippleData(int16 *Src, int16 *Dest, int16 Density, int32 W, int32 H)
{
	int32	i, j;
	int16	Val;

	for(i=0; i<H; i++)
	{
		for(j=0; j<W; j++)
		{
			if(i > 0)							// Get top
				Val = *(Dest - W);
			else
				Val = *(Dest + W * (H-1));

			if(i < H-1)						// Get bottom
				Val += *(Dest + W);
			else
				Val += *(Dest - W * (H-1));

			if(j > 0)							// Get left
				Val += *(Dest - 1);
			else
				Val += *(Dest + (W-1));

			if(j < W-1)						// Get right
				Val += *(Dest + 1);
			else
				Val += *(Dest - (W-1));

			Val >>= 1;
			Val -= *Src;
			Val -= (Val >> Density);
			*Src = Val;

			Src++;
			Dest++;
		}
	}
}

/* ------------------------------------------------------------------------------------ */
//	FloatMod
/* ------------------------------------------------------------------------------------ */
static geFloat FloatMod(geFloat In, geFloat Wrap)
{
	Wrap = floor(fabs(In)/Wrap)*Wrap - Wrap;

	if(In > 0)
		In -= Wrap;
	else if(In < 0)
		In += Wrap;

	return In;
}

/* ------------------------------------------------------------------------------------ */
//	Water_Update
/* ------------------------------------------------------------------------------------ */
static void Water_Update(Procedural *Water, geFloat Time)
{
	int16		*Page1, *Page2, *Page3;

	Page1 = Water->WaterData[Water->NPage];
	Page2 = Water->WaterData[!Water->NPage];

	Page3 = Page2;

	Water->TimeToSplashWater += Time;

	if(Water->TimeToSplashWater > 0.8f)
	{
		int32 px, py, cx, cy, c;

		Water->TimeToSplashWater = 0.0f;

		for(c=0; c<2; c++)
		{
			px=(1+(rand()%(Water->Width-1-10)));
			py=(1+(rand()%(Water->Height-1-10)));

			for(cy=py; cy<(py+8); cy++)
				for(cx=px; cx<(px+8); cx++)
					Water->WaterData[!Water->NPage][cy * Water->Width + cx] = 255;
		}
	}

	CalcRippleData(Page1, Page2, 4, Water->Width, Water->Height);

	Water->NPage = !Water->NPage;
}


/* ----------------------------------- END OF FILE ------------------------------------ */
