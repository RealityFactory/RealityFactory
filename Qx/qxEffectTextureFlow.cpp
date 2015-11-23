// qxEffectTextureFlow.cpp: implementation of the qxEffectTextureFlow class.
//
// Author: Jeff Thelen
//
// Copyright 1999 Quixotic, Inc.  All Rights Reserved.
//////////////////////////////////////////////////////////////////////

#include "..\\RabidFramework.h"
#include "qxEffectTextureFlow.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

qxEffectTextureFlow::qxEffectTextureFlow(char* strName, void*& qxFromEditor)
:
qxEffectBase(strName),
m_pOriginalBmpCopy(0),		
m_fCurX(0.0f),
m_fCurY(0.0f),			
m_PixelFormat(GE_PIXELFORMAT_8BIT),
m_bMipsChecked(false)
,OffsetX(10.0f)
,OffsetY(10.0f)
,AlphaName(0)
,BmpName(0)
{

}

qxEffectTextureFlow::~qxEffectTextureFlow()
{

	if ( m_pOriginalBmpCopy )
	{
		geBitmap_Destroy( &m_pOriginalBmpCopy );
	}
	
	if ( m_pBmp )
	{
		geBitmap_Destroy( &m_pBmp );
	}
	if(BmpName)
		delete BmpName;
	if(AlphaName)
		delete AlphaName;
}

bool qxEffectTextureFlow::Init()
{
	if(!qxEffectBase::Init() )
		return false;

	AttachBmp(BmpName, AlphaName);

	geBitmap_Info	DestInfo;

	// save bitmap info
	if ( !geBitmap_GetInfo( m_pBmp, &DestInfo, NULL ) )
	{
		return false;
	}
	if ( !gePixelFormat_HasPalette( DestInfo.Format ) )
	{
		m_bMipsChecked = true;
		m_PixelFormat = GE_PIXELFORMAT_16BIT_555_RGB;
	}

	geBitmap_SetColorKey( m_pBmp, GE_FALSE, 0, GE_FALSE );
	
	if(!geBitmap_SetFormatMin( m_pBmp, m_PixelFormat ))
		return false;

	geBitmap_ClearMips( m_pBmp );
	geBitmap_CreateRef( m_pBmp );
	
	// keep a copy of the original one
	m_pOriginalBmpCopy = geBitmap_Create( DestInfo.Width, DestInfo.Height, 1, m_PixelFormat );
	if ( !m_pOriginalBmpCopy )
	{
		return false;
	}
	geBitmap_ClearMips( m_pOriginalBmpCopy );
	geBitmap_BlitBitmap( m_pBmp, m_pOriginalBmpCopy );

	// build color lookup table
	if ( m_PixelFormat == GE_PIXELFORMAT_8BIT )
	{
		if(!CreateColorTable())
			return false;
	}

	return true;
}


int qxEffectTextureFlow::Frame()
{
	// return -1 if we should continue in this function.
	int ret = qxEffectBase::Frame();
	if( ret != -1 )
	{
		return ret; // else, 0 == kill effect, 1 == continue effect
	}


	// locals
	geBitmap		*DestLocked = NULL;
	geBitmap		*SrcLocked = NULL;
	
	geBitmap_Info	DestInfo, SrcInfo;
	uint8			*DestBits_8, *SrcBits_8;
	uint8			*CurDestBits_8, *CurSrcBits1_8, *CurSrcBits2_8;
	uint16			*DestBits_16, *SrcBits_16;
	uint16			*CurDestBits_16, *CurSrcBits1_16, *CurSrcBits2_16;
	int				Row, Col;
	int				SrcRow, SrcCol;

	bool			bSuccess = true;

	// perform preprocessing on mips if required
	if ( ( !m_bMipsChecked ) && ( m_PixelFormat == GE_PIXELFORMAT_8BIT ) )
	{
		if ( !geBitmap_GetInfo( m_pBmp, &DestInfo, NULL ))
		{
			return false;
		}

		geBitmap_ClearMips( m_pBmp );
		m_bMipsChecked = true;
	}

	// lock the source bitmap for reading
	if ( !geBitmap_LockForReadNative( m_pOriginalBmpCopy, &SrcLocked, 0, 0 ) )
	{
		bSuccess = false;
		goto ALLDONE;
	}
	if ( !geBitmap_GetInfo( SrcLocked, &SrcInfo, NULL ) )
	{
		bSuccess = false;
		goto ALLDONE;
	}

	SrcBits_8 = (uint8*)geBitmap_GetBits( SrcLocked );
	if ( SrcBits_8 == NULL )
	{
		bSuccess = false;
		goto ALLDONE;
	}

	// lock the destination bitmap for writing
	if ( !geBitmap_LockForWriteFormat( m_pBmp, &DestLocked, 0, 0, m_PixelFormat )  )
	{
		bSuccess = false;
		goto ALLDONE;
	}

	if ( !geBitmap_GetInfo( DestLocked, &DestInfo, NULL ) )
	{
		bSuccess = false;
		goto ALLDONE;
	}

	DestBits_8 = (uint8*)geBitmap_GetBits( DestLocked );
	if ( DestBits_8 == NULL )
	{
		bSuccess = false;
		goto ALLDONE;
	}

	// process all pixels in 8bit pal format...
	if ( m_PixelFormat == GE_PIXELFORMAT_8BIT )
	{
		for ( Row = 0; Row < DestInfo.Height; Row++ )
		{

			// set destination pointer
			CurDestBits_8 = DestBits_8 + ( DestInfo.Stride * Row );

			// set source pointer 1
			SrcRow = (int32)m_fCurY + Row;
			if ( SrcRow >= SrcInfo.Height )
			{
				SrcRow -= SrcInfo.Height;
			}
			ASSERT( SrcRow < SrcInfo.Height );
			CurSrcBits1_8 = SrcBits_8 + ( SrcInfo.Stride * SrcRow );

			// set source pointer 2
			SrcCol = (int32)m_fCurX;
			CurSrcBits2_8 = SrcBits_8 + ( SrcInfo.Stride * Row );

			// copy data
			for ( Col = 0; Col < DestInfo.Width; Col++ )
			{

				// adjust current pixel
				*CurDestBits_8 = m_Table[*CurSrcBits1_8][*( CurSrcBits2_8 + SrcCol )];

				// adjust first src pointer
				CurSrcBits1_8++;

				// adjust second src pointer
				SrcCol++;
				if ( SrcCol >= SrcInfo.Width )
				{
					SrcCol -= SrcInfo.Width;
				}
				ASSERT( SrcCol >= 0 );
				ASSERT( SrcCol < SrcInfo.Width );
				
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
			SrcRow = (int32)m_fCurY + Row;
			if ( SrcRow >= SrcInfo.Height )
			{
				SrcRow -= SrcInfo.Height;
			}
			ASSERT( SrcRow < SrcInfo.Height );
			CurSrcBits1_16 = SrcBits_16 + ( SrcInfo.Stride * SrcRow );

			// set source pointer 2
			SrcCol = (int32)m_fCurX;
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
				ASSERT( SrcCol >= 0 );
				ASSERT( SrcCol < SrcInfo.Width );
				
				// adjust dest pointer
				CurDestBits_16++;
			}
		}
	}


	
	// adjust offsets
	m_fCurX +=  (CCD->LastElapsedTime_F()*0.001f) * OffsetX ;
	if ( m_fCurX >= SrcInfo.Width )
	{
		m_fCurX = 0.0f;
	}

	m_fCurY +=  (CCD->LastElapsedTime_F()*0.001f) * OffsetY ;
	
	if ( m_fCurY >= SrcInfo.Height )
	{
		m_fCurY = 0.0f;
	}

	// unlock all bitmaps
	ALLDONE:
	if ( SrcLocked )
	{
		VERIFY(geBitmap_UnLock( SrcLocked ));
	}
	if ( DestLocked )
	{
		VERIFY(geBitmap_UnLock( DestLocked ));
	}

	// all done
	if ( m_PixelFormat == GE_PIXELFORMAT_16BIT_555_RGB )
	{
		geBitmap_RefreshMips( m_pBmp );
	}
	
	return bSuccess;

	
}



////////////////////////////////////////////////////////////////////////////////////////
//
//
//	FindBestColorMatch()
//
//
////////////////////////////////////////////////////////////////////////////////////////
uint8 qxEffectTextureFlow::FindBestColorMatch(
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

	// ensure valid data
	ASSERT( ColorTable != NULL );
	ASSERT( R >= 0 );
	ASSERT( R <= 255 );
	ASSERT( G >= 0 );
	ASSERT( G <= 255 );
	ASSERT( B >= 0 );
	ASSERT( B <= 255 );
	ASSERT( A >= 0 );
	ASSERT( A <= 255 );

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
//	CreateColorTable()
//
//	Creates a color lookup table.
//
////////////////////////////////////////////////////////////////////////////////////////
bool qxEffectTextureFlow::CreateColorTable()
{
	// locals
	geBitmap_Palette	*Pal;
	int					Color1, Color2;
	ColorRGBA			ColorTable;
	int					NewR, NewG, NewB, NewA;

	// get the palette
	Pal = geBitmap_GetPalette( m_pOriginalBmpCopy );
	if ( !Pal )
		return false;

	// load up all the colors
	for ( Color1 = 0; Color1 < 256; Color1++ )
	{
		if(!geBitmap_Palette_GetEntryColor( Pal, Color1, 
											&( ColorTable.R[Color1] ), 
											&( ColorTable.G[Color1] ), 
											&( ColorTable.B[Color1] ), 
											&( ColorTable.A[Color1] ) ))
		return false;
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
			m_Table[Color1][Color2] = 
				FindBestColorMatch( &ColorTable, NewR, NewG, NewB, NewA );
		}
	}

	

	return true;
}
