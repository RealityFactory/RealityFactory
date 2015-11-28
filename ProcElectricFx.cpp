/************************************************************************************//**
 * @file ProcElectricFx.cpp
 * @brief Procedural Electric Fx
 ****************************************************************************************/

// Disable unsafe warning
#pragma warning (disable : 4996)

#include "ProcUtil.h"

#define SMOOTH_WRAP GE_TRUE

// changed QD 12/15/05
//#define		PI	(3.14159f)
//#define		PI_2	(PI*2.0f)
// end change

#define		ELECTRICFX_MAX_FX_TYPES	128
#define		ELECTRICFX_MAX_VECS	128

#define		TOKEN_SEPERATOR		" ,"

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
typedef struct
{
	int32	x, y;

} ElectricFx_Vec2d;

typedef enum
{
	Fx_Line = 0,
	Fx_LineSpin,
	Fx_Sparkle,
	Fx_EnergySpin

} ElectricFx_Type;

typedef struct
{
	ElectricFx_Type	Type;			// Type of fx
	char			Str[256];		// String name
	int32			NumVecs;		// Number of vecs required to run fx

} ElectricFx_TypeData;

typedef struct
{
	ElectricFx_TypeData		*TypeData;
	ElectricFx_Vec2d		Vecs[2];
	geFloat					Rotation;

} ElectricFx_Fx;


ElectricFx_TypeData FxTypeDataTable[] =
{
	{Fx_Line,		"Fx_Line",			2},
	{Fx_LineSpin,	"Fx_LineSpin",		1},
	{Fx_Sparkle,	"Fx_Spark",			1},
	{Fx_EnergySpin, "Fx_EnergySpin",	1}
};

typedef struct Procedural
{
	geBitmap		*Bitmap;
	geBitmap_Info	BitmapInfo;

	int32			Width;
	int32			Height;
	int32			Size;
	int32			WMask;
	int32			HMask;

	uint8			*ZBuffer;

	uint16			CLut[256];

	int32			PalIndex;

	int32			NumFx;
	ElectricFx_Fx	Fx[ELECTRICFX_MAX_FX_TYPES];

} Procedural;

static int32 FxTypeDataTableSize = (sizeof(FxTypeDataTable)/sizeof(ElectricFx_TypeData));

void		ElectricFx_Destroy(Procedural *Proc);
geBoolean	ElectricFx_Animate(Procedural *Fx, geFloat ElapsedTime);

static geBoolean	ElectricFx_Shade(Procedural *Fx);
static void		ElectricFx_Update(Procedural *Fx, geFloat Time);
static ElectricFx_TypeData *ElectricFx_GetFxTypeDataFromString(const char *Str);
static int32 ElectricFx_GetPalIndexFromString(const char *Str);
static geBoolean	ElectricFx_FxParseData(ElectricFx_Fx *Fx);
static geBoolean	ElectricFx_InitPalette(Procedural *Proc);
static geBoolean	ElectricFx_InitBitmap(Procedural *Fx, geBitmap *ppBitmap);

/* ------------------------------------------------------------------------------------ */
//	ElectricFx_Create
/* ------------------------------------------------------------------------------------ */
Procedural *ElectricFx_Create(char *TextureName, geWorld  *World, const char *StrParms)
{
	Procedural	*Proc;
	geBitmap	*Bitmap;

	Proc = GE_RAM_ALLOCATE_STRUCT(Procedural);

	if(!Proc)
		goto ExitWithError;

	memset(Proc, 0, sizeof(*Proc));

	Bitmap = geWorld_GetBitmapByName(World, TextureName);

	if(!Bitmap)
	  goto ExitWithError;

	// Parse the params...
	{
		char		*Token;
		char		Parms[1024];

		strcpy(Parms, StrParms);

		Token = strtok(Parms, TOKEN_SEPERATOR);

		if (!Token)		// If nothing was passed in, then default it to something cool
		{
			strcpy(Parms, "Fx_PalSlime, Fx_EnergySpin, 63, 63");
			Token = strtok(Parms, TOKEN_SEPERATOR);
		}

		Proc->PalIndex = ElectricFx_GetPalIndexFromString(Token);

		Token = strtok((char *)NULL, TOKEN_SEPERATOR);

		while(Token)
		{
			ElectricFx_Fx	*Fx;

			if(Proc->NumFx >= ELECTRICFX_MAX_FX_TYPES)
				break;		// No more room for more fx...

			Fx = &Proc->Fx[Proc->NumFx++];

			Fx->TypeData = ElectricFx_GetFxTypeDataFromString(Token);

			if(!Fx->TypeData)
				goto ExitWithError;

			if(!ElectricFx_FxParseData(Fx))
				goto ExitWithError;

			Token = strtok((char*)NULL, TOKEN_SEPERATOR);
		}
	}

	if(!ElectricFx_InitBitmap(Proc, Bitmap))
		goto ExitWithError;

	if(!ElectricFx_Animate(Proc, 0.1f))
		goto ExitWithError;

	return Proc;


ExitWithError:

	if(Proc)
		ElectricFx_Destroy(Proc);

	return (Procedural*)NULL;
}

/* ------------------------------------------------------------------------------------ */
//	ElectricFx_Destroy
/* ------------------------------------------------------------------------------------ */
void ElectricFx_Destroy(Procedural *Proc)
{
	if(Proc->ZBuffer)
	{
		geRam_Free(Proc->ZBuffer);
		Proc->ZBuffer = (unsigned char*)NULL;
	}

	if(Proc->Bitmap)
	{
		geBitmap_Destroy(&Proc->Bitmap);
		Proc->Bitmap = (geBitmap*)NULL;
	}

	geRam_Free(Proc);
}

/* ------------------------------------------------------------------------------------ */
//	ElectricFx_Animate
/* ------------------------------------------------------------------------------------ */
geBoolean ElectricFx_Animate(Procedural *Fx, geFloat ElapsedTime)
{
	if(!Fx->Bitmap)
		return GE_TRUE;

	ElectricFx_Update(Fx, ElapsedTime);

	// Shade the Fx
	if(!ElectricFx_Shade(Fx))
		return GE_FALSE;

	return GE_TRUE;
}

/* ------------------------------------------------------------------------------------ */
// sgn() - This function is used by Line() to determine the sign of a long //
/* ------------------------------------------------------------------------------------ */
static int sgn (long a)
{
	if(a > 0)
		return +1;
	else if(a < 0)
		return -1;
	else
		return 0;
}

/* ------------------------------------------------------------------------------------ */
// round() - This function is used by Line() to round a long to the        //
/* ------------------------------------------------------------------------------------ */
static int round (geFloat a)
{
	if((a - (int)a) < 0.5f)
		return (int)floor(a);
	else
		return (int)ceil(a);
}


/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
static void ElectricFx_PutZ(Procedural *Fx, int x, int y, int ZVal, geFloat ZAge)
{
	Fx->ZBuffer[(y&Fx->HMask)*Fx->Width+(x&Fx->WMask)] = ZVal;
}

/* ------------------------------------------------------------------------------------ */
// Line() - This draws a line from a,b to c,d of color col.                //
/* ------------------------------------------------------------------------------------ */
static void ElectricFx_ZLine(Procedural *Fx, int a, int b, int c, int d, int32 ZVal, geFloat ZAge)
{
	long u, s, v, d1x, d1y, d2x, d2y, m, n;
	int	i;

	u   = c-a;			// x2-x1
	v   = d-b;			// y2-y1
	d1x = sgn(u);		// d1x is the sign of u (x2-x1) (VALUE -1,0,1)
	d1y = sgn(v);		// d1y is the sign of v (y2-y1) (VALUE -1,0,1)
	d2x = sgn(u);		// d2x is the sign of u (x2-x1) (VALUE -1,0,1)
	d2y = 0;
	m   = abs(u);		// m is the distance between x1 and x2
	n   = abs(v);		// n is the distance between y1 and y2

	if(m <= n)			// if the x distance is greater than the y distance
	{
		d2x = 0;
		d2y = sgn(v);	// d2y is the sign of v (x2-x1) (VALUE -1,0,1)
		m   = abs(v);	// m is the distance between y1 and y2
		n   = abs(u);	// n is the distance between x1 and x2
	}

	s = (int)(m>>1);						// s is the m distance (either x or y) divided by 2

	// repeat this loop until it is = to m (y or x distance)
	for(i=m; i>0; i--)
	{
		ElectricFx_PutZ(Fx, a, b, ZVal, ZAge);// plot a pixel at the original x1, y1

		s += n;								// add n (dis of x or y) to s (dis of x of y)

		if(s >= m)							// if s is >= m (distance between y1 and y2)
		{
			s -= m;
			a += d1x;
			b += d1y;
		}
		else
		{
			a += d2x;
			b += d2y;
		}
	}
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
static void ElectricFx_ZLine2(Procedural *Fx, int x1, int y1, int x2, int y2, int32 ZVal, geFloat ZAge)
{
	int	x, y, xlength, ylength, dx, dy;
	geFloat	xslope, yslope;

	xlength = abs(x1-x2);

	if((x1-x2) < 0)
		dx = -1;
	else if((x1-x2) > 0)
		dx = +1;
	else
		dx =  0;

	ylength = abs(y1-y2);

	if((y1-y2) < 0)
		dy = -1;
	else if((y1-y2) > 0)
		dy = +1;
	else
		dy =  0;

	if(dy == 0)
	{
		if(dx < 0)
			for(x=x1; x<x2+1; x++)
				ElectricFx_PutZ(Fx, x, y1, ZVal, ZAge);

		if(dx > 0)
			for(x=x2; x<x1+1; x++)
				ElectricFx_PutZ(Fx, x, y1, ZVal, ZAge);
	}

	if(dx == 0)
	{
		if(dy < 0)
			for(y=y1; y<y2+1; y++)
				ElectricFx_PutZ(Fx, x1, y, ZVal, ZAge);

		if(dy > 0)
			for(y=y2; y<y1+1; y++)
				ElectricFx_PutZ(Fx, x1, y, ZVal, ZAge);
	}

	if((xlength != 0) && (ylength != 0))
	{
		xslope = (geFloat)xlength/(geFloat)ylength;
		yslope = (geFloat)ylength/(geFloat)xlength;
	}
	else
	{
		xslope = 0.0f;
		yslope = 0.0f;
	}

	if((xslope != 0) && (yslope != 0) && (yslope/xslope < 1) && (yslope/xslope > -1))
	{
		if(dx < 0)
		{
			for(x=x1; x<x2+1; x++)
			{
				y = round(yslope*x);
				ElectricFx_PutZ(Fx, x, y, ZVal, ZAge);
			}
		}
		else if(dx > 0)
		{
			for(x=x2; x<x1+1; x++)
			{
				y = round(yslope*x);
				ElectricFx_PutZ(Fx, x, y, ZVal, ZAge);
			}
		}
	}
	else
	{
		if(dy < 0)
		{
			for(y=x1; y<x2+1; y++)
			{
				x = round(xslope*y);
				ElectricFx_PutZ(Fx, x, y, ZVal, ZAge);
			}
		}
		else if(dy > 0)
		{
			for(y=x2; y<x1+1; y++)
			{
				x = round(xslope*y);
				ElectricFx_PutZ(Fx, x, y, ZVal, ZAge);
			}
		}
	}
}

/* ------------------------------------------------------------------------------------ */
//	ElectricFx_ElectricZLine_r
/* ------------------------------------------------------------------------------------ */
static void ElectricFx_ElectricZLine_r( Procedural *Fx,
										int32 x1, int32 y1,
										int32 x2, int32 y2,
										int32 ZVal, geFloat ZAge,
										int32 Recursion)
{
	if(Recursion > 0)
	{
		int32	MidX, MidY;

		MidX = (x1+x2)>>1;
		MidY = (y1+y2)>>1;

		MidX += 8 - (rand()&15);
		MidY += 8 - (rand()&15);
		/*
		if (MidX < 0)
			MidX = 0;
		else if (MidX >= Fx->Width)
			MidX = Fx->Width-1;

		if (MidY < 0)
			MidY = 0;
		else if (MidY >= Fx->Height)
			MidY = Fx->Height-1;
		*/

		ElectricFx_ElectricZLine_r(Fx, x1, y1, MidX, MidY, ZVal, ZAge, Recursion>>1);
		ElectricFx_ElectricZLine_r(Fx, MidX, MidY, x2, y2, ZVal, ZAge, Recursion>>1);
	}
	else
	{
		ElectricFx_ZLine(Fx, x1, y1, x2, y2, ZVal, ZAge);
	}
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
static geBoolean ElectricFx_Shade(Procedural *Fx)
{
	uint8		*ZBuffer;
	uint8		*Bits;
	int32		i;
	geBitmap	*Lock;

	if(!geBitmap_LockForWriteFormat(Fx->Bitmap, &Lock, 0, 0, GE_PIXELFORMAT_8BIT_PAL))
	{
		geBitmap_SetFormat(Fx->Bitmap, GE_PIXELFORMAT_8BIT_PAL, GE_TRUE, 0, (geBitmap_Palette*)NULL);
		geBitmap_LockForWriteFormat(Fx->Bitmap,&Lock,0,0, GE_PIXELFORMAT_8BIT_PAL);

		if(!Lock)
			return GE_FALSE;
	}

	if(!geBitmap_GetInfo(Lock, &(Fx->BitmapInfo),(geBitmap_Info*)NULL))
		goto Fail;

	if(Fx->BitmapInfo.Format != GE_PIXELFORMAT_8BIT_PAL)
		goto Fail;

	Bits = (unsigned char*)geBitmap_GetBits(Lock);
	ZBuffer = Fx->ZBuffer;

	// Shade the data for the Fx using the z buffer to provide
	// occlusion information.
	for(i=0; i<Fx->Size-5; i++, ZBuffer++)
	{
		int32	Result;
		int32	Val;

		if(*ZBuffer == 0)
			continue;

		Val = (int32)*ZBuffer;

		Result = Val;// - *(ZBuffer-1);

		Bits[i] = (uint8)min(Result + Bits[i], 255);

		if(Val > ZBuffer[1])
			Bits[i+1] = max(Bits[i+1]-3, 0);
		if(Val > ZBuffer[2])
			Bits[i+2] = max(Bits[i+2]-7, 0); // changed QD 12/15/05
		if(Val > ZBuffer[3])
			Bits[i+3] = max(Bits[i+3]-10, 0);
		if(Val > ZBuffer[4])
			Bits[i+4] = max(Bits[i+4]-15, 0);
		if(Val > ZBuffer[5])
			Bits[i+5] = max(Bits[i+5]-7, 0);
	}

	// Smooth the bitmap
	for(i=0; i<2; i++)
	{
		geBitmapUtil_SmoothBits(&Fx->BitmapInfo, Bits, Bits, 1, SMOOTH_WRAP);
	}

	if(!geBitmap_UnLock(Lock))
		goto Fail;

	return GE_TRUE;

Fail:

	if(Lock)
		geBitmap_UnLock(Lock);

	return GE_FALSE;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
static void ElectricFx_Update(Procedural *EFx, geFloat Time)
{
	uint8			*ZBuffer;
	ElectricFx_Fx	*Fx;
	int32			i;

	ZBuffer = EFx->ZBuffer;

	// Zero the destination buffer
	memset(ZBuffer,0, EFx->Size*sizeof(uint8));

	Fx = EFx->Fx;

	for(i=0; i<EFx->NumFx; i++, Fx++)
	{
		switch(Fx->TypeData->Type)
		{
		case Fx_Line:
			{
				ElectricFx_Vec2d	*pVec1, *pVec2;

				pVec1 = &Fx->Vecs[0];
				pVec2 = &Fx->Vecs[1];

				ElectricFx_ElectricZLine_r(EFx, pVec1->x, pVec1->y, pVec2->x, pVec2->y, 54, 1.0f, 16);
				break;
			}
		case Fx_Sparkle:
			{
				int32	i;

				for(i=0; i<3; i++)
				{
					int32		x, y;
					geFloat		r;

					// changed QD 12/15/05
					r = ((geFloat)i/6.0f)*GE_2PI; //PI_2;

					r += 2.0f - ((geFloat)rand()/RAND_MAX)*4.0f;

					x = (int32)(cos(r)*60.0f);
					y = (int32)(sin(r)*60.0f);

					x += Fx->Vecs[0].x;
					y += Fx->Vecs[0].y;

					ElectricFx_ElectricZLine_r(EFx, Fx->Vecs[0].x, Fx->Vecs[0].y, x, y, 64, 1.0f, 4);
				}

				break;
			}
		case Fx_EnergySpin:
			{
				int32				j;
				ElectricFx_Vec2d	*pVec;

				pVec = Fx->Vecs;

				for(j=0; j<Fx->TypeData->NumVecs; j++, pVec++)
				{
					int32	i, x, y;
					geFloat	r;

					for(i=0; i<3; i++)
					{
						geFloat	Val;

						r = Fx->Rotation;

						// changed QD 12/15/05
						Val = ((geFloat)i/3)*GE_2PI; //PI_2;

						r += Val;

						x = (int32)(cos(r)*63.0f);
						y = (int32)(sin(r)*63.0f);

						x += pVec->x;
						y += pVec->y;

						ElectricFx_ElectricZLine_r(EFx, pVec->x, pVec->y, x, y, 74, 1.0f, 8);
					}
				}

				Fx->Rotation += 2.0f*Time;		// 2 radians per sec

				// changed QD 12/15/05
				if(Fx->Rotation > GE_2PI) //PI_2)
					Fx->Rotation -= GE_2PI; //PI_2;

				break;
			}
		default:
			{
				break;
			}
		}
	}
}


typedef struct
{
	geFloat	f, a, r, g, b;

} CPoint;

static char PalStr[][256] =
{
	"Fx_PalSlime",
	"Fx_PalFire",
	"Fx_PalOrange",
	"Fx_PalBlue"
};


#define PalStrTableSize (sizeof(PalStr) / sizeof(char[256]))

static CPoint CPoints[PalStrTableSize][3] =
{
	0.0f,	0.0f,	0.0f,	0.0f,	0.0f,
	30.0f,	70.0f,	0.0f,	30.0f,	0.0f,
	180.0f, 255.0f, 255.0f, 255.0f, 25.0f,

	0.0f,	0.0f,	0.0f,	0.0f,	0.0f,
	30.0f,	70.0f,	40.0f,	0.0f,	0.0f,
	180.0f, 255.0f, 255.0f, 255.0f, 25.0f,

	0.0f,	0.0f,	0.0f,	0.0f,	0.0f,
	30.0f,	70.0f,	70.0f,	10.0f,	0.0f,
	180.0f, 255.0f, 255.0f, 255.0f, 25.0f,

	0.0f,	0.0f,	0.0f,	0.0f,	0.0f,
	30.0f,	60.0f,	5.0f,	5.0f,	30.0f,
	180.0f, 255.0f, 100.0f, 100.0f, 165.0f
};

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
static uint16 LerpColor(geFloat c1, geFloat c2, geFloat Ratio)
{
	geFloat	Val;

	Val = c1 + (c2 - c1)*Ratio;

	Val *= 8.0f;

	if(Val > 255.0f)
		Val = 255.0f;

	return (uint16)Val;
}

/* ------------------------------------------------------------------------------------ */
//	ElectricFx_GetPalIndexFromString
/* ------------------------------------------------------------------------------------ */
static int32 ElectricFx_GetPalIndexFromString(const char *Str)
{
	int32 i;

	for(i=0; i<PalStrTableSize; i++)
	{
		if(!stricmp(Str, PalStr[i]))
			return i;
	}

	return 0;
}

/* ------------------------------------------------------------------------------------ */
//	ElectricFx_GetFxTypeDataFromString
/* ------------------------------------------------------------------------------------ */
static ElectricFx_TypeData	*ElectricFx_GetFxTypeDataFromString(const char *Str)
{
	int32 i;

	for(i=0; i<FxTypeDataTableSize; i++)
	{
		if(!stricmp(Str, FxTypeDataTable[i].Str))
			return &FxTypeDataTable[i];
	}

	return (ElectricFx_TypeData*)NULL;
}

/* ------------------------------------------------------------------------------------ */
//	ElectricFx_FxParseData
/* ------------------------------------------------------------------------------------ */
static geBoolean ElectricFx_FxParseData(ElectricFx_Fx *Fx)
{
	int32	VecNum;
	char	*Token;

	VecNum = 0;

	while(1)
	{
		// Parse the first token
		Token = strtok((char*)NULL, TOKEN_SEPERATOR);

		if(!Token)
			return GE_FALSE;

		Fx->Vecs[VecNum].x = atoi(Token);

		// Parse the second token
		Token = strtok((char*)NULL, TOKEN_SEPERATOR);

		if(!Token)
			return GE_FALSE;

		Fx->Vecs[VecNum].y = atoi(Token);

		VecNum++;

		if(VecNum >= Fx->TypeData->NumVecs)
			break;
	}

	return GE_TRUE;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
static geBoolean ElectricFx_InitBitmap(Procedural *Fx, geBitmap *ppBitmap)
{
	if(!ppBitmap)
	{
		ppBitmap = geBitmap_Create(128, 128, 1, GE_PIXELFORMAT_8BIT_PAL);

		if(!ppBitmap)
			return GE_FALSE;
	}
	else
	{
		geBitmap_CreateRef(ppBitmap);
	}

	if(!geBitmap_SetFormat(ppBitmap, GE_PIXELFORMAT_8BIT_PAL,GE_TRUE, 255, (geBitmap_Palette*)NULL))
		return GE_FALSE;

	if(!geBitmap_ClearMips(ppBitmap))
		return GE_FALSE;

	if(!geBitmap_SetPreferredFormat(ppBitmap, GE_PIXELFORMAT_16BIT_4444_ARGB))
		return GE_FALSE;

	if(!geBitmapUtil_SetColor(ppBitmap, 0, 0, 0, 0))
		return GE_FALSE;

	if(!geBitmap_GetInfo(ppBitmap, &Fx->BitmapInfo, (geBitmap_Info*)NULL))
		return GE_FALSE;

	Fx->Bitmap = ppBitmap;

	if(!ElectricFx_InitPalette(Fx))
		return GE_FALSE;

	Fx->Width	= Fx->BitmapInfo.Width;
	Fx->Height	= Fx->BitmapInfo.Height;
	Fx->Size	= Fx->Width*Fx->Height;

	Fx->WMask	= Fx->Width-1;
	Fx->HMask	= Fx->Height-1;

	Fx->ZBuffer = GE_RAM_ALLOCATE_ARRAY(uint8, Fx->Size);

	if(!Fx->ZBuffer)
		return GE_FALSE;

	// Clear the buffers out
	memset(Fx->ZBuffer,0, Fx->Size*sizeof(uint8));

	return GE_TRUE;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
static geBoolean ElectricFx_InitPalette(Procedural *Proc)
{
	geBitmap_Info	Info;
	geBitmap_Palette *Pal;
	void *PalData = NULL;
	gePixelFormat	PalFormat;
	int				PalSize;

	Pal = geBitmap_Palette_Create(GE_PIXELFORMAT_32BIT_ARGB, 256);

	if(!Pal)
		goto fail;

	if(!geBitmap_SetPalette(Proc->Bitmap,Pal))
		goto fail;

	geBitmap_Palette_Destroy(&Pal);

	if(!geBitmap_GetInfo(Proc->Bitmap,&Info,(geBitmap_Info*)NULL))
		goto fail;

	if(Info.Format != GE_PIXELFORMAT_8BIT_PAL)
		goto fail;

	if(!(Pal = geBitmap_GetPalette(Proc->Bitmap)))
		goto fail;

	if(!geBitmap_Palette_Lock(Pal,&PalData, &PalFormat, &PalSize))
		goto fail;

	if(PalSize < 256)
		goto fail;

	{
		int32	i;
		int32	NumControlPoints, Current;
		CPoint	*pPoint;

		// Fill the palette
		NumControlPoints = 3;

		Current = 0;

		pPoint = &CPoints[Proc->PalIndex][0];

		for(i=0; i<256; i++)
		{
			geFloat	Ratio;
			int32	Next;
			uint32	R,G,B,A;
			uint8	*PalPtr;

			Next = Current+1;

			if(Next > NumControlPoints-1)
				Next = NumControlPoints-1;

			Ratio = (geFloat)(i-pPoint[Current].f)/pPoint[Next].f;

			if(Ratio > 1.0f)
				Ratio = 1.0f;
			else if(Ratio < 0.0f)
				Ratio = 0.0f;

			PalPtr = ((uint8 *)PalData) + i * gePixelFormat_BytesPerPel(PalFormat);

			A = (uint32)LerpColor(pPoint[Current].a, pPoint[Next].a, Ratio);
			R = (uint32)LerpColor(pPoint[Current].r, pPoint[Next].r, Ratio);
			G = (uint32)LerpColor(pPoint[Current].g, pPoint[Next].g, Ratio);
			B = (uint32)LerpColor(pPoint[Current].b, pPoint[Next].b, Ratio);

			gePixelFormat_PutColor(PalFormat, &PalPtr, R, G, B, A);

			if((geFloat)i >= pPoint[Next].f)
			{
				Current++;

				if(Current > NumControlPoints-1)
					Current = NumControlPoints-1;
			}
		}
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

/* ----------------------------------- END OF FILE ------------------------------------ */
