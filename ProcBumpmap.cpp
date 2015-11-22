#include "ProcUtil.h"

/**********************************************************/

typedef struct Procedural
{
	uint32		Frame;
	geBitmap *	Bitmap;
} Procedural;

/**********************************************************/

static geBoolean BumpMap_CreateFromHeightMap(geBitmap * HeightMap,geBitmap * BumpMap);
static geBoolean BumpMap_ComputePalette(geBitmap *BumpMap, geVec3d *Horizontal,geVec3d * Vertical, geVec3d *Pos, geVec3d *LightPos);

/****/

Procedural *BumpMap_Create(char *TextureName, geWorld  *World, const char *ParmStart)
{
    Procedural * P;
    geBitmap * HeightMap;
    geBitmap	*ppBitmap;


	ppBitmap = geWorld_GetBitmapByName(World, TextureName);
	if(ppBitmap == NULL)
	  return (Procedural *)NULL;

	P = GE_RAM_ALLOCATE_STRUCT(Procedural);

	if ( ! P )
	  return (Procedural *)NULL;

	memset(P,0,sizeof(Procedural));

	P->Frame = 0;

	P->Bitmap = ppBitmap;

	if ( geBitmap_ClearMips(P->Bitmap) )
	{
		if ( geBitmap_SetColorKey(P->Bitmap,GE_FALSE,0,GE_FALSE) )
		{
			HeightMap = geBitmap_Create(geBitmap_Width(P->Bitmap),geBitmap_Height(P->Bitmap),1,GE_PIXELFORMAT_8BIT_GRAY);
			if ( HeightMap )
			{
				if ( geBitmap_BlitBitmap(P->Bitmap,HeightMap) )
				{
					if ( BumpMap_CreateFromHeightMap(HeightMap,P->Bitmap) )
					{
						geBitmap_Destroy(&HeightMap);
						return P;
					}
				}
				geBitmap_Destroy(&HeightMap);
			}
		}
	}

	geRam_Free(P);
	return (Procedural *)NULL;
}

void BumpMap_Destroy(Procedural * P)
{
	if ( ! P )
		return;
	geRam_Free(P);
}

geBoolean BumpMap_Animate(Procedural * P,geFloat time)
{
  geVec3d Pos,LightPos,Horizontal,Vertical;
  geBoolean Ret;

	// <> these should be the sides of the poly we're drawing on,
	//		but we can't get that info easily...
	Horizontal.X = 1.0f;
	Horizontal.Y = 0.0f;
	Horizontal.Z = 0.0f;
	Vertical.X = 0.0f;
	Vertical.Y = 1.0f;
	Vertical.Z = 0.0f;
	Pos.X = 0.0f;
	Pos.Y = 0.0f;
	Pos.Z = 0.0f;
	LightPos.X = (geFloat)(100.0f * cos(P->Frame * 0.05f));
	LightPos.Y = (geFloat)(100.0f * sin(P->Frame * 0.05f));
	LightPos.Z = 100.0f;

	Ret = BumpMap_ComputePalette(P->Bitmap, &Horizontal,&Vertical, &Pos, &LightPos );

	P->Frame ++;
  return Ret;
}

/*****************************************************************/

#define	PI		((double)3.14159265358979323846)
#define TWOPI	(2.0*PI)
#define PIOVER2	(PI*0.5)

static int BumpTableInited = 0;
static geFloat BumpTableX[256];
static geFloat BumpTableY[256];
static geFloat BumpTableZ[256];

static geBoolean BumpMap_InitTables(void)
{
int p;
double theta,phi;

	if ( BumpTableInited )
		return GE_TRUE;
	BumpTableInited = 1;

	for(p=0;p<256;p++)
	{
		theta = ((double)(p>>4) + 0.5)*PIOVER2/16.0;
		phi   = ((double)(p&0x0F) + 0.5)*TWOPI/16.0;

		BumpTableX[p] = (geFloat)( sin(theta) * cos(phi));
		BumpTableY[p] = (geFloat)( sin(theta) * sin(phi));
		BumpTableZ[p] = (geFloat) cos(theta);
	}
return GE_TRUE;
}

static geBoolean BumpMap_CreateFromHeightMap(geBitmap * HeightMap,geBitmap * BumpMap)
{
geBitmap *HeightLock=(geBitmap *)NULL,*BumpLock=(geBitmap *)NULL;
uint8 *HeightBits,*BumpBits;
geBitmap_Info HeightInfo,BumpInfo;
int w,h;

	if ( ! geBitmap_SetFormat(BumpMap,GE_PIXELFORMAT_8BIT_PAL,GE_TRUE,255,(geBitmap_Palette *)NULL) )
		goto fail;

	if ( ! geBitmap_LockForRead(HeightMap,&HeightLock,0,0,GE_PIXELFORMAT_8BIT_GRAY,0,0) )
		goto fail;

	if ( ! geBitmap_GetInfo(HeightLock,&HeightInfo,(geBitmap_Info *)NULL) )
		goto fail;

	w = HeightInfo.Width;
	h = HeightInfo.Height;

	{
	geBitmap_Palette * Pal;
		Pal = geBitmap_Palette_Create(GE_PIXELFORMAT_32BIT_ARGB,256);
		if ( ! Pal )
			goto fail;
		if ( ! geBitmap_SetPalette(BumpMap,Pal) )
			goto fail;
		geBitmap_Palette_Destroy(&Pal);
	}

	if ( ! geBitmap_LockForWriteFormat(BumpMap,&BumpLock,0,0,GE_PIXELFORMAT_8BIT_PAL) )
    {
	  geBitmap_SetFormat(BumpMap,GE_PIXELFORMAT_8BIT_PAL,GE_TRUE,0,(geBitmap_Palette *)NULL);
	  geBitmap_LockForWriteFormat(BumpMap,&BumpLock,0,0, GE_PIXELFORMAT_8BIT_PAL);
	  if(BumpLock == NULL)
		goto fail;
    }


	if ( ! geBitmap_GetInfo(BumpLock,&BumpInfo,(geBitmap_Info *)NULL) )
		goto fail;

	HeightBits	= (unsigned char *)geBitmap_GetBits(HeightLock);
	BumpBits	= (unsigned char *)geBitmap_GetBits(BumpLock);

	if ( ! HeightBits || ! BumpBits )
		goto fail; 

{
int x,y,bs,hs;
uint8 *hp,*bp;

	hp = HeightBits;
	bp = BumpBits;
	hs = HeightInfo.Stride;
	bs = BumpInfo.Stride;

	for(y=0;y<(h-1);y++)
	{
		for(x=0;x<(w-1);x++)
		{
		int NW,SW,NE,SE,vx,vy;
		double scale,nX,nY,nZ;
		double theta,phi;
		int _theta,_phi;

			NW = hp[0];
			SW = hp[hs];
			hp++;
			NE = hp[0];
			SE = hp[hs];

			vx = ((NW - NE) + (SW - SE) + 1)>>1;
			vy = ((NW - SW) + (NE - SE) + 1)>>1;

			scale = 1.0 / ( 1 + vx*vx + vy*vy );
			scale = sqrt(scale);

			nX = scale*vx;
			nY = scale*vy;
			nZ = scale;

			theta = acos(nZ);
			if ( fabs(nX) > fabs(nY) )
			{
			double cosphi;
				cosphi = nX / sin(theta);
				if ( cosphi > 1.0 )
					phi = 0.0;
				else if ( cosphi < -1.0 )
					phi = PI;
				else
					phi = acos( cosphi );
				// (phi in 0 -> PI)
				if ( nY < 0.0 )
					phi = TWOPI - phi;
			}
			else
			{
			double sinphi;
				sinphi = nY / sin(theta);
				if ( sinphi > 1.0 )
					phi = PIOVER2;
				else if ( sinphi < -1.0 )
					phi = - PIOVER2;
				else
					phi = asin( sinphi );
				// (phi in -PI/2 -> PI/2 )
				if ( nX < 0.0 )
					phi = PI - phi;
				if ( phi < 0.0 )
					phi += TWOPI;
			}

			theta	= theta * 16.0 / (PIOVER2);
			phi		= phi	* 16.0 / (TWOPI);

			// we add 0.5 when we go backwards, so don't do it here
			_theta	= (int)theta;
			_phi	= (int)phi;

			*bp++ = (_theta << 4) | _phi;
		}
		// x == (w-1)
		*bp = bp[-1];
		bp++;
		hp++;

		hp += hs - w;
		bp += bs - w;
	}
	//y == (h-1)
	for(x=0;x<w;x++)
	{
		bp[x] = bp[x - bs];
	}
}

	if ( ! geBitmap_UnLock(BumpLock) )
		goto fail;
	if ( ! geBitmap_UnLock(HeightLock) )
		goto fail;
	
	if ( ! BumpMap_InitTables() )
		goto fail;

	geBitmap_SetPreferredFormat(BumpMap,GE_PIXELFORMAT_8BIT_PAL);

	return GE_TRUE;

fail:

	if ( BumpLock )		geBitmap_UnLock(BumpLock);
	if ( HeightLock )	geBitmap_UnLock(HeightLock);

	return GE_FALSE;
}

static geBoolean BumpMap_ComputePalette(geBitmap *BumpMap, geVec3d *pPlaneX,geVec3d *pPlaneY, geVec3d *pPos, geVec3d *pLightPos)
{
geBitmap_Palette *Pal;
geVec3d LightRelPos,PlaneZ;
geFloat LightDist;
void * PalData = NULL;
gePixelFormat PalFormat;
int PalSize;

	geBitmap_SetGammaCorrection(BumpMap, 1.0f, GE_FALSE);

	geVec3d_Subtract(pLightPos ,pPos,&LightRelPos );
	geVec3d_CrossProduct(pPlaneX,pPlaneY,&PlaneZ);
	geVec3d_Normalize(pPlaneX);
	geVec3d_Normalize(pPlaneY);
	geVec3d_Normalize(&PlaneZ);
	LightDist = geVec3d_Normalize(&LightRelPos);

	if ( ! (Pal = geBitmap_GetPalette(BumpMap)) )
		goto fail;

	// if BumpMap is attached to a driver, then this Pal is actually on hardware!

	if ( ! geBitmap_Palette_Lock(Pal,&PalData, &PalFormat, &PalSize) )
		goto fail;
	if ( PalSize < 256 )
		goto fail;

	{
	int p,c;
	uint8 * PalPtr;
	geFloat LightX,LightY,LightZ;
		
		LightX = geVec3d_DotProduct(&LightRelPos,pPlaneX) * 250.0f;
		LightY = geVec3d_DotProduct(&LightRelPos,pPlaneY) * 250.0f;
		LightZ = geVec3d_DotProduct(&LightRelPos,&PlaneZ) * 250.0f;

		PalPtr = (unsigned char *)PalData;
		for(p=0;p<256;p++)
		{
			
			c = (int)(LightX * BumpTableX[p] + LightY * BumpTableY[p] + LightZ * BumpTableZ[p]);
			if ( c < 0 )
				c = 0;

			gePixelFormat_PutColor(PalFormat,&PalPtr,c,c,c,0xFF);
		}		
	}

	PalData = NULL;

	if ( ! geBitmap_Palette_UnLock(Pal) )
		return GE_FALSE;

	Pal = geBitmap_GetPalette(BumpMap);
	if ( ! Pal )
		return GE_FALSE;
	if ( ! geBitmap_SetPalette(BumpMap,Pal) )
		return GE_FALSE;

	return GE_TRUE;

fail:

	if ( PalData )
		geBitmap_Palette_UnLock(Pal);

	return GE_FALSE;
}
