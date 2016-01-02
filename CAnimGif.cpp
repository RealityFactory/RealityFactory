/************************************************************************************//**
 * @file CAnimGif.cpp
 * @brief Play AnimGif file into Genesis3D engine
 *
 * This file contains the class implementation for the CAnimGif class. This class
 * will take an AnimGif file and play it back into the Genesis3D engine window
 * at a specific position.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#include "RabidFramework.h"
#include "CFileManager.h"

typedef struct
{
	BYTE bit;
	WORD previouscode;
	WORD nextcode;

} GIFTABLE;

/* ------------------------------------------------------------------------------------ */
// Constructor
/* ------------------------------------------------------------------------------------ */
CAnimGif::CAnimGif(const char *szFile, int fileformat) :
	m_MainFS(NULL),
	m_Active(false),
	m_Texture(false),
	m_FirstFrame(true),
	m_nWidth(-1),
	m_nHeight(-1),
	m_BackgroundColor(-1),
	m_GlobalColorSize(0),
	m_GlobalColorTable(NULL),
	m_GifSize(0),
	m_pcGif(NULL),
	m_pcGifTrack(NULL),
	m_TotalReadByte(0),
	m_VAnimTime(0.0f)
{
	if(!CFileManager::GetSingletonPtr()->OpenRFFile(&m_MainFS, fileformat, szFile, GE_VFILE_OPEN_READONLY))
		return;

	long Size;
	geVFile_Size(m_MainFS, &Size);

	m_Palette = geBitmap_Palette_Create(GE_PIXELFORMAT_32BIT_XRGB, 256);
	m_Bmp = NULL;
	m_pcGif = NULL;

	if(geVFile_Read(m_MainFS, m_buffer, 13) == GE_TRUE)
	{
		if( !strncmp(reinterpret_cast<char*>(m_buffer), "GIF89a", 6) ||
			!strncmp(reinterpret_cast<char*>(m_buffer), "GIF87a", 6))
		{
			m_nWidth = *reinterpret_cast<WORD*>(m_buffer + 6);
			m_nWidth = ((m_nWidth - 1) | 0x3) + 1;
			m_nHeight = *reinterpret_cast<WORD*>(m_buffer + 8);
			m_BackgroundColor = *(m_buffer + 11);

			if(m_buffer[10] & 0x80)
			{
				m_GlobalColorSize = 0x01 << ((m_buffer[10] & 0x07) + 1);
				m_GlobalColorTable = new BYTE[3 * m_GlobalColorSize];
				if(geVFile_Read(m_MainFS, m_GlobalColorTable, 3 * m_GlobalColorSize) != GE_TRUE)
				{
					delete[] m_GlobalColorTable;
					geVFile_Close(m_MainFS);
					geBitmap_Palette_Destroy(&m_Palette);
					return;
				}
			}

			m_GifSize = Size - 3 * m_GlobalColorSize - 12;
			m_pcGifTrack = m_pcGif = new BYTE[m_GifSize];

			if(geVFile_Read(m_MainFS, m_pcGif, m_GifSize) == GE_TRUE)
			{
				m_Bmp = geBitmap_Create(m_nWidth, m_nHeight, 1, GE_PIXELFORMAT_8BIT);
				geBitmap_SetPreferredFormat(m_Bmp, GE_PIXELFORMAT_8BIT);
				geEngine_AddBitmap(CCD->Engine()->Engine(), m_Bmp);
				geBitmap_GetInfo(m_Bmp, &m_Info, NULL);
				geBitmap_ClearMips(m_Bmp);

				if(GetImage(false))
				{
					m_Active = true;
				}
			}
		}
	}

	geVFile_Close(m_MainFS);
}


/* ------------------------------------------------------------------------------------ */
// Destructor
/* ------------------------------------------------------------------------------------ */
CAnimGif::~CAnimGif()
{
	delete[] m_GlobalColorTable;
	delete[] m_pcGif;

	geBitmap_Palette_Destroy(&m_Palette);

	if(m_Bmp && !m_Texture)
	{
		geEngine_RemoveBitmap(CCD->Engine()->Engine(), m_Bmp);
		geBitmap_Destroy(&m_Bmp);
	}
}


/* ------------------------------------------------------------------------------------ */
// Play
/* ------------------------------------------------------------------------------------ */
int CAnimGif::Play(int XPos, int YPos, bool Center)
{
	if(!m_Active)
		return RGF_FAILURE;

	if(Center)
	{
		XPos = (CCD->Engine()->Width() - m_nWidth) / 2;
		YPos = (CCD->Engine()->Height() - m_nHeight) / 2;
	}

	for(;;)
	{
		MSG msg;

		// If Winblows has something to say, take it in and pass it on in the
		// ..off-chance someone cares.
		while(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			GetMessage(&msg, NULL, 0, 0);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if(!CCD->GetHasFocus())
			continue;

		if(!NextFrame(false))
			break;

		geEngine_BeginFrame(CCD->Engine()->Engine(), CCD->CameraManager()->Camera(), GE_TRUE);
		geEngine_DrawBitmap(CCD->Engine()->Engine(), m_Bmp, NULL, XPos, YPos);
		geEngine_EndFrame(CCD->Engine()->Engine());

		if((GetAsyncKeyState(VK_SPACE) & 0x8000) != 0)
		{
			while((GetAsyncKeyState(VK_SPACE) & 0x8000) != 0)
			{
			}
			break;
		}
	}

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// NextFrame
/* ------------------------------------------------------------------------------------ */
geBitmap *CAnimGif::NextFrame(bool repeat)
{
	if(!m_Active)
		return NULL;

	if(m_FirstFrame)
	{
		m_TotalReadByte = 0;
		m_pcGifTrack = m_pcGif;
		m_VAnimTime = CCD->FreeRunningCounter_F();

		if(GetImage(repeat))
		{
			m_FirstFrame = false;
			return m_Bmp;
		}

		m_FirstFrame = false;
		return NULL;
	}

	int deltaTime = static_cast<int>(0.1f * (CCD->FreeRunningCounter_F() - m_VAnimTime));

	if(deltaTime < 0)
		deltaTime = m_DelayTime;

	if(deltaTime < m_DelayTime)
		return m_Bmp;

	m_VAnimTime = CCD->FreeRunningCounter_F();

	if(GetImage(repeat))
		return m_Bmp;

	if(repeat)
	{
		m_TotalReadByte = 0;
		m_pcGifTrack = m_pcGif;
		m_FirstFrame = false;
		m_VAnimTime = CCD->FreeRunningCounter_F();

		if(GetImage(repeat))
			return m_Bmp;
	}

	return NULL;
}


/* ------------------------------------------------------------------------------------ */
// DisplayNextFrameTexture
/* ------------------------------------------------------------------------------------ */
bool CAnimGif::DisplayNextFrameTexture(const char *szTextureName, bool FFrame)
{
	if(!m_Active)
		return false;

	if(FFrame)
	{
		geBitmap *theBitmap = geWorld_GetBitmapByName(CCD->World(), szTextureName);

		if(!theBitmap)
		{
			m_Active = false;
			return false;
		}

		geBitmap_Info Info;
		geBitmap_GetInfo(theBitmap, &Info, NULL);

		if(m_nWidth != Info.Width || m_nHeight != Info.Height)
		{
			m_Active = false;
			return false;
		}

		if(m_Bmp)
		{
			geEngine_RemoveBitmap(CCD->Engine()->Engine(), m_Bmp);
			geBitmap_Destroy(&m_Bmp);
		}

		m_Bmp = theBitmap;
		m_Texture = true;
		m_TotalReadByte = 0;
		m_pcGifTrack = m_pcGif;
		m_VAnimTime = CCD->FreeRunningCounter_F();

		if(GetImage(true))
		{
			m_FirstFrame = false;
			return true;
		}

		m_FirstFrame = false;
		return false;
	}
	else
	{
		NextFrame(true);
	}

	return true;
}


/* ------------------------------------------------------------------------------------ */
// GetImage
/* ------------------------------------------------------------------------------------ */
bool CAnimGif::GetImage(bool /*repeat*/)
{
	if(!m_pcGif)
		return false;

l1:
	if(m_TotalReadByte > m_GifSize)
	{
		m_pcGifTrack = m_pcGif;
		m_TotalReadByte = 0;
		return false;
	}

	++m_TotalReadByte;

	switch(*m_pcGifTrack++)
	{
	case 0x2C:
		return TakeIt();
		break;
	case 0x21:
		BYTE cSize;
		++m_TotalReadByte;
		switch(*m_pcGifTrack++)
		{
		case 0xF9:
			++m_pcGifTrack;	// block size
			m_DisposalMethod = (*m_pcGifTrack) & 28;
			m_bTransparentIndex = (*m_pcGifTrack++) & 1;
			m_DelayTime = *reinterpret_cast<WORD*>(m_pcGifTrack);
			m_pcGifTrack += 2;
			m_TransparentIndex = *m_pcGifTrack++;
			m_TotalReadByte += 5;
			break;
		case 0xFE:
			while((cSize = *m_pcGifTrack) != 0)
			{
				m_pcGifTrack += cSize + 1;
				m_TotalReadByte += cSize + 1;

				if(m_TotalReadByte > m_GifSize)
					return false;
			}
			break;
		case 0x01:
			m_pcGifTrack += 13;
			m_TotalReadByte += 13;
			while((cSize = *m_pcGifTrack) != 0)
			{
				m_pcGifTrack += cSize + 1;
				m_TotalReadByte += cSize + 1;

				if(m_TotalReadByte > m_GifSize)
					return false;
			}
			break;
		case 0xFF:
			m_pcGifTrack += 12;
			m_TotalReadByte += 12;
			while((cSize = *m_pcGifTrack) != 0)
			{
				m_pcGifTrack += cSize + 1;
				m_TotalReadByte += cSize + 1;
				if(m_TotalReadByte > m_GifSize)
					return false;
			}
			break;
		default:
			return false;
		}

		++m_pcGifTrack;
		++m_TotalReadByte;

		if(m_TotalReadByte > m_GifSize)
			return false;

		goto l1;
		break;
	case 0x3B:
		m_pcGifTrack = m_pcGif;
		m_TotalReadByte = 0;
		return false;
	case 0:
		m_pcGifTrack = m_pcGif;
		m_TotalReadByte = 0;
		return false;
	default:
		return false;
	}
}


/* ------------------------------------------------------------------------------------ */
// TakeIt
/* ------------------------------------------------------------------------------------ */
bool CAnimGif::TakeIt(void)
{
	UINT uLocalColorTableSize;
	WORD code, oldcode=0, code1;
	int iFinishCode, iResetCode;
	int iPrimaryTableSize, iTableSize;
	BYTE pcColorTable[4];
	GIFTABLE *pcGifTable;

	m_iLeft = *reinterpret_cast<WORD*>(m_pcGifTrack);
	m_pcGifTrack += 2;

	m_iTop = *reinterpret_cast<WORD*>(m_pcGifTrack);
	m_pcGifTrack += 2;

	m_iWidth = *reinterpret_cast<WORD*>(m_pcGifTrack);
	m_pcGifTrack += 2;

	m_iWidth1 = ((m_iWidth - 1) | 0x3) + 1;

	m_iHeight = *reinterpret_cast<WORD*>(m_pcGifTrack);
	m_pcGifTrack += 2;

	m_cPackedField = *m_pcGifTrack++;
	m_TotalReadByte += 9;

	m_iMaxByte = m_iWidth1 * m_iHeight;

	BYTE *pcbmp = m_pcBitmap = new BYTE[m_iMaxByte];
	memset(m_pcBitmap, m_TransparentIndex, m_iMaxByte);

	pcGifTable = new GIFTABLE[4096];

	for(int i=0; i<4096; ++i)
		pcGifTable[i].previouscode = pcGifTable[i].nextcode = 0;

	if(m_cPackedField & 0x80)
	{
		uLocalColorTableSize = 1;
		uLocalColorTableSize <<= (m_cPackedField & 7) + 1;
		m_TotalReadByte += uLocalColorTableSize * 3;

		for(UINT i=0; i<uLocalColorTableSize; ++i)
		{
			pcColorTable[0] = *m_pcGifTrack++;
			pcColorTable[1] = *m_pcGifTrack++;
			pcColorTable[2] = *m_pcGifTrack++;
			pcColorTable[3] = 255;

			geBitmap_Palette_SetEntryColor(m_Palette, i,
				pcColorTable[0], pcColorTable[1],
				pcColorTable[2], pcColorTable[3]);
		}

	}
	else
	{
		BYTE *pcGlobalColor = m_GlobalColorTable;

		for(int i=0; i<m_GlobalColorSize; ++i)
		{
			pcColorTable[0] = *pcGlobalColor++;
			pcColorTable[1] = *pcGlobalColor++;
			pcColorTable[2] = *pcGlobalColor++;
			pcColorTable[3] = 255;

			geBitmap_Palette_SetEntryColor(m_Palette, i,
				pcColorTable[0], pcColorTable[1],
				pcColorTable[2], pcColorTable[3]);
		}
	}

	m_uPrimaryBitSize = m_uBitSize = (*m_pcGifTrack++);
	++m_TotalReadByte;

	iPrimaryTableSize = iTableSize = (1 << m_uBitSize) + 2;
	iFinishCode = iTableSize - 1;
	iResetCode = iFinishCode - 1;

	++m_uPrimaryBitSize;
	++m_uBitSize;
	m_uRemain = 0;
	m_cCurentByte = 0;
	m_uBlockSize = 0;
	m_uReadByte = 1;
	m_x = m_y = 0;
	m_iPass = 1;
	m_iRow = 0;

	while((code = GetCode()) != iFinishCode)
	{
		if(code == iResetCode)
		{
			m_uBitSize = m_uPrimaryBitSize;
			iTableSize = iPrimaryTableSize;
			oldcode = GetCode();

			if(oldcode > iTableSize)
			{
				SAFE_DELETE_A(m_pcBitmap);
				SAFE_DELETE_A(pcGifTable);
				return false;
			}

			Output(static_cast<BYTE>(oldcode));
			continue;
		}

		if(code < iTableSize)
		{
			code1 = code;
			WORD code2 = 0;

			while(code1 >= iPrimaryTableSize)
			{
				pcGifTable[code1].nextcode = code2;
				code2 = code1;
				code1 = pcGifTable[code1].previouscode;

				if(code1 >= code2)
				{
					SAFE_DELETE_A(m_pcBitmap);
					SAFE_DELETE_A(pcGifTable);
					return false;
				}
			}

			Output(static_cast<BYTE>(code1));

			while(code2 != 0)
			{
				Output(pcGifTable[code2].bit);
				code2 = pcGifTable[code2].nextcode;
			}

			pcGifTable[iTableSize].bit = static_cast<BYTE>(code1);
			pcGifTable[iTableSize].previouscode = oldcode;
			++iTableSize;

			if(iTableSize == (0x0001 << m_uBitSize))
				++m_uBitSize;

			if(m_uBitSize > 12)
				m_uBitSize = 12;

			oldcode = code;
		}
		else
		{
			code1 = oldcode;
			WORD code2 = 0;

			while(code1 >= iPrimaryTableSize)
			{
				pcGifTable[code1].nextcode = code2;
				code2 = code1;
				code1 = pcGifTable[code1].previouscode;

				if(code1 >= code2)
				{
					SAFE_DELETE_A(m_pcBitmap);
					SAFE_DELETE_A(pcGifTable);
					return false;
				}
			}

			Output(static_cast<BYTE>(code1));

			while(code2 != 0)
			{
				Output(pcGifTable[code2].bit);
				code2 = pcGifTable[code2].nextcode;
			}

			Output(static_cast<BYTE>(code1));
			pcGifTable[iTableSize].bit = static_cast<BYTE>(code1);
			pcGifTable[iTableSize].previouscode = oldcode;
			++iTableSize;

			if(iTableSize == (0x0001 << m_uBitSize))
				++m_uBitSize;

			if(m_uBitSize > 12)
				m_uBitSize = 12;

			oldcode = code;
		}
	}

	geBitmap_SetFormat(m_Bmp, GE_PIXELFORMAT_8BIT_PAL, GE_TRUE, 0, m_Palette);

	geBitmap_SetPalette(m_Bmp, m_Palette);
	geBitmap_LockForWriteFormat(m_Bmp, &m_LockedBMP, 0, 0, GE_PIXELFORMAT_8BIT);

	if(m_LockedBMP == NULL)
	{
		geBitmap_SetFormat(m_Bmp, GE_PIXELFORMAT_8BIT, GE_TRUE, 0, m_Palette);
		geBitmap_LockForWriteFormat(m_Bmp, &m_LockedBMP, 0, 0, GE_PIXELFORMAT_8BIT);

		if(m_LockedBMP == NULL)
		{
			SAFE_DELETE_A(m_pcBitmap);
			SAFE_DELETE_A(pcGifTable);
			return false;
		}
	}

	unsigned char *wptr, *pptr;

	wptr = static_cast<unsigned char*>(geBitmap_GetBits(m_LockedBMP));
	pptr = pcbmp;

	memset(wptr, m_TransparentIndex, m_nWidth * m_nHeight);

	wptr += ((m_iTop * m_nWidth) + m_iLeft);
	int Temp1 = m_iWidth1;

	for(int yy=0; yy<m_iHeight; ++yy)
	{
		__asm
		{
			mov esi, pptr
			mov edi, wptr
			mov ecx, Temp1
			rep movs
		}

		wptr += m_nWidth;
		pptr += m_iWidth1;
	}

	geBitmap_UnLock(m_LockedBMP);

	if(m_bTransparentIndex)
		geBitmap_SetColorKey(m_Bmp, GE_TRUE, m_TransparentIndex, GE_FALSE);

	++m_pcGifTrack;
	++m_TotalReadByte;

	SAFE_DELETE_A(m_pcBitmap);
	SAFE_DELETE_A(pcGifTable);

	return true;
}


/* ------------------------------------------------------------------------------------ */
// Output
/* ------------------------------------------------------------------------------------ */
void CAnimGif::Output(BYTE bit)
{
	int tmp;

	if(m_cPackedField&0x40)
	{
		if(m_x == m_iWidth)
		{
			m_x = 0;
			switch(m_iPass)
			{
			case 1:
			case 2:
				m_iRow += 8;
				break;
			case 3:
				m_iRow += 4;
				break;
			case 4:
				m_iRow += 2;
				break;
			}

			if(m_iRow >= m_iHeight)
			{
				m_iPass += 1;
				m_iRow = 16 >> m_iPass;
			}
		}

		tmp = m_iRow * m_iWidth1 + m_x;
		m_pcBitmap[tmp] = bit;
		++m_x;
	}
	else
	{
		if(m_x == m_iWidth)
		{
			m_x = 0;
			++m_y;
		}

		tmp = m_y * m_iWidth1 + m_x;
		++m_x;
	}

	if(tmp > m_iMaxByte)
		return;

	m_pcBitmap[tmp] = bit;
}


/* ------------------------------------------------------------------------------------ */
// GetByte
/* ------------------------------------------------------------------------------------ */
BYTE CAnimGif::GetByte(void)
{
	if(m_uReadByte >= m_uBlockSize)
	{
		m_uBlockSize = *m_pcGifTrack++;
		m_uReadByte = 0;
		m_TotalReadByte += m_uBlockSize + 1;

		if(m_TotalReadByte > m_GifSize)
		{
			m_TotalReadByte -= m_uBlockSize + 1;
			return 0xFF;
		}

		if(m_uBlockSize == 0)
		{
			--m_pcGifTrack;
			--m_TotalReadByte;
			return 0xFF;
		}
	}

	++m_uReadByte;
	return *m_pcGifTrack++;
}


/* ------------------------------------------------------------------------------------ */
// GetCode
/* ------------------------------------------------------------------------------------ */
WORD CAnimGif::GetCode(void)
{
	UINT tmp1;
	BYTE tmp = 1;

	if(m_uRemain >= m_uBitSize)
	{
		tmp <<= m_uBitSize;
		--tmp;
		tmp1 = m_cCurentByte & tmp;
		m_cCurentByte >>= m_uBitSize;
		m_uRemain -= m_uBitSize;
	}
	else
	{
		tmp1 = m_cCurentByte;
		m_cCurentByte = GetByte();
		tmp = 1;

		if(8 >= (m_uBitSize - m_uRemain))
		{
			tmp <<= (m_uBitSize - m_uRemain);
			--tmp;
			tmp1 = ((static_cast<UINT>(m_cCurentByte & tmp)) << m_uRemain) + tmp1;
			m_cCurentByte >>= (m_uBitSize - m_uRemain);
			m_uRemain = 8 - (m_uBitSize - m_uRemain);
		}
		else
		{
			tmp1 = ((static_cast<UINT>(m_cCurentByte)) << m_uRemain) + tmp1;
			m_cCurentByte = GetByte();
			tmp <<= m_uBitSize - m_uRemain - 8;
			--tmp;
			tmp1 = ((static_cast<UINT>(m_cCurentByte & tmp)) << (m_uRemain + 8)) + tmp1;
			m_cCurentByte >>= m_uBitSize - m_uRemain - 8;
			m_uRemain = 8 - (m_uBitSize - m_uRemain - 8);
		}
	}

	return tmp1;
}

/* ----------------------------------- END OF FILE ------------------------------------ */
