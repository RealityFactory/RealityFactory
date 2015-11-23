#include "ProcUtil.h"

////////////////////////////////////////////////////////////////////////////////////////
//	Procedural struct
////////////////////////////////////////////////////////////////////////////////////////
typedef struct Procedural
{
	geBitmap		*Bitmap;			// the output bitmap
	geBitmap		*Original;			// copy of the original bitmap
	geFloat			X, Y;				// current offsets
	geFloat			XOffset, YOffset;	// offset amounts
	geBoolean		MipsChecked;		// whether or not preprocessing of mips has occured
	gePixelFormat	PixelFormat;		// our prefered pixel format
	uint8			Table[256][256];	// color lookup table

} Procedural;


////////////////////////////////////////////////////////////////////////////////////////
//	Color struct
////////////////////////////////////////////////////////////////////////////////////////
typedef struct ColorRGBA
{
	int	R[256];
	int	G[256];
	int	B[256];
	int	A[256];

} ColorRGBA;

////////////////////////////////////////////////////////////////////////////////////////
//
//	Flow_CreateColorTable()
//
//	Creates a color lookup table.
//
////////////////////////////////////////////////////////////////////////////////////////
static uint8 Flow_FindBestColorMatch(
	ColorRGBA	*ColorTable,	// color table to use
	int			R,				// desired red
	int			G,				// desired green
	int			B,				// desired blue
	int			A )				// desired alpha
{

	// locals
	int		Color;
	int		RDiff, GDiff, BDiff, ADiff;
	int		BestAverageDiff = 0, AverageDiff;
	uint8	BestMatch = 0;

	// find best match
	for ( Color = 0; Color < 256; Color++ )
	{

		// get color deltas
		RDiff = abs( ColorTable->R[Color] - R );
		GDiff = abs( ColorTable->G[Color] - G );
		BDiff = abs( ColorTable->B[Color] - B );
		ADiff = abs( ColorTable->A[Color] - A );

		// mark new best match if required
		AverageDiff = ( ( RDiff + GDiff + BDiff + ADiff ) >> 2 );
		if ( ( Color == 0 ) || ( BestAverageDiff > AverageDiff ) )
		{
			BestAverageDiff = AverageDiff;
			BestMatch = (uint8)Color;
		}
	}

	// return best match
	return BestMatch;

} // Flow_CreateColorTable()



////////////////////////////////////////////////////////////////////////////////////////
//
//	Flow_CreateColorTable()
//
//	Creates a color lookup table.
//
////////////////////////////////////////////////////////////////////////////////////////
static geBoolean Flow_CreateColorTable(
	Procedural	*P )	// procedural struct
{

	// locals
	geBitmap_Palette	*Pal;
	geBoolean			Result;
	int					Color1, Color2;
	ColorRGBA			ColorTable;
	int					NewR, NewG, NewB, NewA;

	// get the palette
	Pal = geBitmap_GetPalette( P->Original );
	if ( Pal == NULL )
	{
		return GE_FALSE;
	}

	// load up all the colors
	for ( Color1 = 0; Color1 < 256; Color1++ )
	{
		Result = geBitmap_Palette_GetEntryColor( Pal, Color1, &( ColorTable.R[Color1] ), &( ColorTable.G[Color1] ), &( ColorTable.B[Color1] ), &( ColorTable.A[Color1] ) );
		if ( Result == GE_FALSE )
		{
			return GE_FALSE;
		}
	}

	// build color table
	for ( Color1 = 0; Color1 < 256; Color1++ )
	{
		for ( Color2 = 0; Color2 < 256; Color2++ )
		{

			// pick new color value
			NewR = ( ( ColorTable.R[Color1] >> 1 ) + ( ColorTable.R[Color2] >> 1 ) );
			NewG = ( ( ColorTable.G[Color1] >> 1 ) + ( ColorTable.G[Color2] >> 1 ) );
			NewB = ( ( ColorTable.B[Color1] >> 1 ) + ( ColorTable.B[Color2] >> 1 ) );
			NewA = ( ( ColorTable.A[Color1] >> 1 ) + ( ColorTable.A[Color2] >> 1 ) );

			// save best color index in the table
			P->Table[Color1][Color2] = Flow_FindBestColorMatch( &ColorTable, NewR, NewG, NewB, NewA );
		}
	}

	// all done
	return GE_TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////
//
//	Flow_Create()
//
//	Create the procedural.
//
////////////////////////////////////////////////////////////////////////////////////////
Procedural * Flow_Create(char *TextureName, geWorld  *World, const char	*Parameters )
{
	Procedural		*P;
	geBoolean		Result;
	geBitmap_Info	DestInfo;
        geBitmap	*ppBitmap;


	ppBitmap = geWorld_GetBitmapByName(World, TextureName);
	if(ppBitmap == NULL)
	  return (Procedural *)NULL;

	// allocate memory for procedural struct
	P = GE_RAM_ALLOCATE_STRUCT(Procedural);

	if ( ! P )
	  return (Procedural *)NULL;
	memset(P,0,sizeof(Procedural));

	// set offsets
	if(strlen(Parameters) < 2)
	{
		P->XOffset = 18.0f;
		P->YOffset = 18.0f;
	}
	else
	{

		// locals
		char	*pstr;
		char	ParmWork[100];

		// make a work copy of parameters string
		strcpy( ParmWork, Parameters );

		// extract x offset rate
		pstr = strtok( ParmWork, " ,\n\r\r" );
		P->XOffset = (geFloat)atof(pstr);

		// extract y offset rate
		pstr = strtok( (char *)NULL, " ,\n\r\r" );
		P->YOffset = (geFloat)atof( pstr );
	}

	// save bitmap info
	P->Bitmap = ppBitmap;
	if ( geBitmap_GetInfo( P->Bitmap, &DestInfo, (geBitmap_Info *)NULL ) == GE_FALSE )
	{
	  geRam_Free( P );
	  return (Procedural *)NULL;
	}
	if ( gePixelFormat_HasPalette( DestInfo.Format ) )
	{
		P->PixelFormat = GE_PIXELFORMAT_8BIT;
	}
	else
	{
		P->MipsChecked = GE_TRUE;
		P->PixelFormat = GE_PIXELFORMAT_16BIT_555_RGB;
	}
	geBitmap_SetColorKey( P->Bitmap, GE_FALSE, 0, GE_FALSE );
	Result = geBitmap_SetFormatMin( P->Bitmap, P->PixelFormat );
	if ( Result != GE_TRUE )
	{
	  geRam_Free( P );
	  return (Procedural *)NULL;
	}
	geBitmap_ClearMips( P->Bitmap );
	geBitmap_CreateRef( P->Bitmap );
	
	// keep a copy of the original one
	P->Original = geBitmap_Create( DestInfo.Width, DestInfo.Height, 1, P->PixelFormat );
	if ( P->Original == NULL )
	{
	  geRam_Free( P );
	  return (Procedural *)NULL;
	}
	geBitmap_ClearMips( P->Original );
	geBitmap_BlitBitmap( P->Bitmap, P->Original );

	// build color lookup table
	if ( P->PixelFormat == GE_PIXELFORMAT_8BIT )
	{
		Result = Flow_CreateColorTable( P );
		if ( Result == GE_FALSE )
		{
		  geRam_Free( P );
		  return (Procedural *)NULL;
		}
	}

	// all done
	return P;

} // Flow_Create()



////////////////////////////////////////////////////////////////////////////////////////
//
//	Flow_Destroy()
//
//	Destroy the procedural.
//
////////////////////////////////////////////////////////////////////////////////////////
void Flow_Destroy(Procedural	*P )	// procedural to destroy
{

	// do nothing if its a null pointer
	if ( P == NULL )
	{
		return;
	}

	// zap the original copy
	if ( P->Original != NULL )
	{
		geBitmap_Destroy( &( P->Original ) );
	}

	// zap the output bitmap
	if ( P->Bitmap != NULL )
	{
		geBitmap_Destroy( &( P->Bitmap ) );
	}

	// free the procedural
	geRam_Free( P );

} // Flow_Destroy()



////////////////////////////////////////////////////////////////////////////////////////
//
//	Flow_Animate()
//
//	Animate the procedural.
//
////////////////////////////////////////////////////////////////////////////////////////
geBoolean Flow_Animate(Procedural *P, geFloat Time )
{

	// locals
	geBoolean		Result;
	geBoolean		Success = GE_TRUE;
	geBitmap		*DestLocked = (geBitmap *)NULL, *SrcLocked = (geBitmap *)NULL;
	geBitmap_Info	DestInfo, SrcInfo;
	uint8			*DestBits_8, *SrcBits_8;
	uint8			*CurDestBits_8, *CurSrcBits1_8, *CurSrcBits2_8;
	uint16			*DestBits_16, *SrcBits_16;
	uint16			*CurDestBits_16, *CurSrcBits1_16, *CurSrcBits2_16;
	int				Row, Col;
	int				SrcRow, SrcCol;

	// perform preprocessing on mips of required
	if ( ( P->MipsChecked == GE_FALSE ) && ( P->PixelFormat == GE_PIXELFORMAT_8BIT ) )
	{
		if ( geBitmap_GetInfo( P->Bitmap, &DestInfo, (geBitmap_Info *)NULL ) == GE_FALSE )
		{
			return GE_FALSE;
		}
		geBitmap_ClearMips( P->Bitmap );
		P->MipsChecked = GE_TRUE;
	}

	// lock the source bitmap for reading
	if ( geBitmap_LockForReadNative( P->Original, &SrcLocked, 0, 0 ) == GE_FALSE )
	{
		Success = GE_FALSE;
		goto ALLDONE;
	}
	if ( geBitmap_GetInfo( SrcLocked, &SrcInfo, (geBitmap_Info *)NULL ) == GE_FALSE )
	{
		Success = GE_FALSE;
		goto ALLDONE;
	}
	SrcBits_8 = (unsigned char *)geBitmap_GetBits( SrcLocked );
	if ( SrcBits_8 == NULL )
	{
		Success = GE_FALSE;
		goto ALLDONE;
	}

	// lock the destination bitmap for writing
	if ( geBitmap_LockForWriteFormat( P->Bitmap, &DestLocked, 0, 0, P->PixelFormat ) == GE_FALSE )
	{
		Success = GE_FALSE;
		goto ALLDONE;
	}
	if ( geBitmap_GetInfo( DestLocked, &DestInfo, (geBitmap_Info *)NULL ) == GE_FALSE )
	{
		Success = GE_FALSE;
		goto ALLDONE;
	}
	DestBits_8 = (unsigned char *)geBitmap_GetBits( DestLocked );
	if ( DestBits_8 == NULL )
	{
		Success = GE_FALSE;
		goto ALLDONE;
	}

	// process all pixels in 8bit pal format...
	if ( P->PixelFormat == GE_PIXELFORMAT_8BIT )
	{
		for ( Row = 0; Row < DestInfo.Height; Row++ )
		{

			// set destination pointer
			CurDestBits_8 = DestBits_8 + ( DestInfo.Stride * Row );

			// set source pointer 1
			SrcRow = (int32)P->Y + Row;
			if ( SrcRow >= SrcInfo.Height )
			{
				SrcRow -= SrcInfo.Height;
			}
			CurSrcBits1_8 = SrcBits_8 + ( SrcInfo.Stride * SrcRow );

			// set source pointer 2
			SrcCol = (int32)P->X;
			CurSrcBits2_8 = SrcBits_8 + ( SrcInfo.Stride * Row );

			// copy data
			for ( Col = 0; Col < DestInfo.Width; Col++ )
			{

				// adjust current pixel
				*CurDestBits_8 = P->Table[*CurSrcBits1_8][*( CurSrcBits2_8 + SrcCol )];

				// adjust first src pointer
				CurSrcBits1_8++;

				// adjust second src pointer
				SrcCol++;
				if ( SrcCol >= SrcInfo.Width )
				{
					SrcCol -= SrcInfo.Width;
				}

				// adjust dest pointer
				CurDestBits_8++;
			}
		}
	}
	// ...or in 555 format
	else
	{
		DestBits_16 = (uint16 *)DestBits_8;
		SrcBits_16 = (uint16 *)SrcBits_8;
		for ( Row = 0; Row < DestInfo.Height; Row++ )
		{

			// set destination pointer
			CurDestBits_16 = DestBits_16 + ( DestInfo.Stride * Row );

			// set source pointer 1
			SrcRow = (int32)P->Y + Row;
			if ( SrcRow >= SrcInfo.Height )
			{
				SrcRow -= SrcInfo.Height;
			}
			CurSrcBits1_16 = SrcBits_16 + ( SrcInfo.Stride * SrcRow );

			// set source pointer 2
			SrcCol = (int32)P->X;
			CurSrcBits2_16 = SrcBits_16 + ( SrcInfo.Stride * Row );

			// copy data
			for ( Col = 0; Col < DestInfo.Width; Col++ )
			{

				// adjust current pixel
				*CurDestBits_16 = (uint16)( ( ( ( *CurSrcBits1_16 & 0xF81F ) + ( *( CurSrcBits2_16 + SrcCol ) & 0xF81F ) ) >> 1 ) & 0xF81F );
				*CurDestBits_16 |= ( ( ( ( *CurSrcBits1_16 & 0x3E0 ) + ( *( CurSrcBits2_16 + SrcCol ) & 0x3E0 ) ) >> 1 ) & 0x3E0 );

				// adjust first src pointer
				CurSrcBits1_16++;

				// adjust second src pointer
				SrcCol++;
				if ( SrcCol >= SrcInfo.Width )
				{
					SrcCol -= SrcInfo.Width;
				}

				// adjust dest pointer
				CurDestBits_16++;
			}
		}
	}

	// adjust offsets
	P->X += ( Time * P->XOffset );
	if ( P->X >= SrcInfo.Width )
	{
		P->X = 0.0f;
	}
// changed RF064 by QuestOfDreams
	else if ( P->X < 0.0f )
	{
		P->X = (geFloat)SrcInfo.Width;
	}
	P->Y += ( Time * P->YOffset );
	if ( P->Y >= SrcInfo.Height )
	{
		P->Y = 0.0f;
	}
	else if ( P->Y < 0.0f )
	{
		P->Y = (geFloat)SrcInfo.Height;
	}
// end change RF064
	// unlock all bitmaps
	ALLDONE:
	if ( SrcLocked != NULL )
	{
		Result = geBitmap_UnLock( SrcLocked );
	}
	if ( DestLocked != NULL )
	{
		Result = geBitmap_UnLock( DestLocked );
	}

	// all done
	if ( P->PixelFormat == GE_PIXELFORMAT_16BIT_555_RGB )
	{
		geBitmap_RefreshMips( P->Bitmap );
	}
	return Success;

} // Flow_Animate()
