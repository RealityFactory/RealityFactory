/*
CAnimGif.cpp	:	Play AnimGif file into G3D engine

  (c) 2001 Ralph Deane
  
	This file contains the class implementation for the CAnimGif
	class.  This class will take an AnimGif file and play it back into
	the Genesis3D engine window at a specific position
*/

#include "RabidFramework.h"

//	CAnimGif
//
//	Constructor

CAnimGif::CAnimGif(char *szFile, int fileformat)
{
	long Size;
	
	GifSize=GlobalColorSize=0;
	Active = false;
	Texture = false;
	GlobalColorTable = NULL;

	if(!CCD->OpenRFFile(&MainFS, fileformat, szFile, GE_VFILE_OPEN_READONLY))
		return ;
	geVFile_Size(MainFS, &Size); 
	
	Palette = geBitmap_Palette_Create(GE_PIXELFORMAT_32BIT_XRGB, 256); 
	theBmp = NULL;
	pcGif = NULL;

	if(geVFile_Read(MainFS, buffer, 13)==GE_TRUE)
	{
		if((strncmp((char *)buffer, "GIF89a", 6)==0) || (strncmp((char *)buffer, "GIF87a", 6)==0))
		{
			nWidth = *(WORD*)(buffer+6);
			nWidth=((nWidth-1)|0x3)+1;
			nHeight = *(WORD*)(buffer+8);
			BackgroundColor = *(buffer+11);
			if(buffer[10]&0x80)
			{
				GlobalColorSize=0x01<<((buffer[10]&0x07)+1);
				GlobalColorTable=new BYTE[3*GlobalColorSize];
				if(geVFile_Read(MainFS, GlobalColorTable, 3*GlobalColorSize)!=GE_TRUE)
				{
					delete[] GlobalColorTable;
					geVFile_Close(MainFS);
					geBitmap_Palette_Destroy(&Palette);
					return;
				}
			}
			GifSize=Size-3*GlobalColorSize-12;
			pcGifTrack=pcGif=new BYTE[GifSize];
			if(geVFile_Read(MainFS, pcGif, GifSize)==GE_TRUE)
			{
				TotalReadByte = 0;
				FirstFrame = true;

				theBmp = geBitmap_Create(nWidth, nHeight, 1, GE_PIXELFORMAT_8BIT);
				geBitmap_SetPreferredFormat(theBmp, GE_PIXELFORMAT_8BIT);
				geEngine_AddBitmap(CCD->Engine()->Engine(), theBmp);
				geBitmap_GetInfo(theBmp,&Info,NULL);
				geBitmap_ClearMips(theBmp);
	
				if(GetImage(false))
				{
					Active = true;
				}
			}
		}
	}
	geVFile_Close(MainFS);
	return;
}

//	~CAnimGif
//
//	Destructor

CAnimGif::~CAnimGif()
{
	if(GlobalColorTable)
		delete[] GlobalColorTable;
	if(pcGif)
		delete[] pcGif;
	geBitmap_Palette_Destroy(&Palette); 
	if(theBmp && !Texture)
		geBitmap_Destroy(&theBmp);
	
	return;
}

//	Play
//

int CAnimGif::Play(int XPos, int YPos, bool Center)
{
	if(!Active)
		return RGF_FAILURE;

	if(Center)
	{
		XPos = (CCD->Engine()->Width() - nWidth) / 2;
		YPos = (CCD->Engine()->Height() - nHeight) / 2;
	}
	
	
	for(;;)
	{
		MSG msg;

	// If Winblows has something to say, take it in and pass it on in the
	// ..off-chance someone cares.

		while (PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE))
		{
			GetMessage(&msg, NULL, 0, 0 );
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if(!CCD->GetHasFocus())
			continue;
	
		if(!NextFrame(false))
			break;
		geEngine_BeginFrame(CCD->Engine()->Engine(), CCD->CameraManager()->Camera(), GE_TRUE);
		geEngine_DrawBitmap(CCD->Engine()->Engine(), theBmp, NULL, XPos, YPos);
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

geBitmap *CAnimGif::NextFrame(bool repeat)
{
	if(!Active)
		return NULL;
	if(FirstFrame)
	{
		TotalReadByte = 0;
		pcGifTrack=pcGif;
		VAnimTime = (float)CCD->FreeRunningCounter();
		if(GetImage(repeat))
		{
			FirstFrame = false;
			return theBmp;
		}
		FirstFrame = false;
		return NULL;
	}

	int deltaTime = (int)(0.1f * (float)(CCD->FreeRunningCounter() - VAnimTime));
	if(deltaTime<0)
		deltaTime = DelayTime;
	if(deltaTime<DelayTime)
		return theBmp;
	VAnimTime = (float)CCD->FreeRunningCounter();
	if(GetImage(repeat))
		return theBmp;
	if(repeat)
	{
		TotalReadByte = 0;
		pcGifTrack=pcGif;
		FirstFrame = false;
		VAnimTime = (float)CCD->FreeRunningCounter();
		if(GetImage(repeat))
			return theBmp;
	}
	return NULL;
}

bool CAnimGif::DisplayNextFrameTexture(char *szTextureName, bool FFrame)
{
	if(!Active)
		return false;
	if(FFrame)
	{
		geBitmap_Info Info;
		geBitmap *theBitmap = geWorld_GetBitmapByName(CCD->World(), szTextureName);
		if(!theBitmap)
		{
			Active = false;
			return false;
		}
		geBitmap_GetInfo(theBitmap,&Info,NULL);
		if(nWidth!=Info.Width || nHeight!=Info.Height)
		{
			Active = false;
			return false;
		}
		if(theBmp)
			geBitmap_Destroy(&theBmp);
		theBmp = theBitmap;
		Texture = true;
		TotalReadByte = 0;
		pcGifTrack=pcGif;
		VAnimTime = (float)CCD->FreeRunningCounter();
		if(GetImage(true))
		{
			FirstFrame = false;
			return true;
		}
		FirstFrame = false;
		return false;
	}
	else
	{
		NextFrame(true);
	}
	return true;
}

bool CAnimGif::GetImage(bool repeat)
{
	if(pcGif==NULL)
		return false;
l1:	if(TotalReadByte>GifSize)
	{
		pcGifTrack=pcGif;
		TotalReadByte=0;
		return false;
	}
	TotalReadByte++;
	switch(*pcGifTrack++)
	{
	case 0x2C:
		return TakeIt();
		break;
	case 0x21:
			BYTE cSize;
			TotalReadByte++;
			switch(*pcGifTrack++)
			{ 
			case 0xF9:
				pcGifTrack++;//block size
				DisposalMethod=(*pcGifTrack)&28;
				bTransparentIndex=(*pcGifTrack++)&1;
				DelayTime=*(WORD*)pcGifTrack;
				pcGifTrack+=2;
				TransparentIndex=*pcGifTrack++;
				TotalReadByte+=5;
				break;
			case 0xFE:
				while((cSize=*pcGifTrack)!=0)
				{
					pcGifTrack+=cSize+1;
					TotalReadByte+=cSize+1;
					if(TotalReadByte>GifSize)
						return false;
				}
				break;
			case 0x01:
				pcGifTrack+=13;
				TotalReadByte+=13;
				while((cSize=*pcGifTrack)!=0)
				{
					pcGifTrack+=cSize+1;
					TotalReadByte+=cSize+1;
					if(TotalReadByte>GifSize)
						return false;
				}
				break;
			case 0xFF:
				pcGifTrack+=12;
				TotalReadByte+=12;
				while((cSize=*pcGifTrack)!=0)
				{
					pcGifTrack+=cSize+1;
					TotalReadByte+=cSize+1;
					if(TotalReadByte>GifSize)
						return false;
				}
				break;
			default:
				return false;
			}
			pcGifTrack++;
			TotalReadByte++;
			if(TotalReadByte>GifSize)
				return false;
			goto l1;
			break;
	case 0x3B:
		pcGifTrack=pcGif;
		TotalReadByte=0;
		return false;
	case 0:
		pcGifTrack=pcGif;
		TotalReadByte=0;
		return false;
	default: 
		return false;
	}
}


bool CAnimGif::TakeIt(void)
{
	UINT uLocalColorTableSize;
	WORD code,oldcode,code1;
	int iFinishCode,iResetCode;
	int iPrimaryTableSize,iTableSize;
	BYTE pcColorTable[4];
	GIFTABLE *pcGifTable;

	iLeft=*(WORD*)pcGifTrack;
	pcGifTrack+=2;
	iTop=*(WORD*)pcGifTrack;
	pcGifTrack+=2;
	iWidth=*(WORD*)pcGifTrack;
	pcGifTrack+=2;
	iWidth1=((iWidth-1)|0x3)+1;
	iHeight=*(WORD*)pcGifTrack;
	pcGifTrack+=2;
	cPackedField=*pcGifTrack++;
	TotalReadByte+=9;
	iMaxByte=iWidth1*iHeight;

	BYTE *pcbmp = pcBitmap=new BYTE[iMaxByte];
	memset(pcBitmap, TransparentIndex, iMaxByte);
	pcGifTable=(GIFTABLE*)new BYTE[sizeof(GIFTABLE)*4096];

	for(int i=0;i<4096;i++)
		pcGifTable[i].previouscode=pcGifTable[i].nextcode=0;

	if(cPackedField&0x80)
	{
		uLocalColorTableSize=1;
		uLocalColorTableSize<<=(cPackedField&7)+1;
		TotalReadByte+=uLocalColorTableSize*3;
		for(UINT i=0;i<uLocalColorTableSize;i++)
		{
			pcColorTable[0]=*pcGifTrack++;
			pcColorTable[1]=*pcGifTrack++;
			pcColorTable[2]=*pcGifTrack++;
			pcColorTable[3]=255;
			geBitmap_Palette_SetEntryColor(Palette, i, 
				pcColorTable[0], pcColorTable[1], 
				pcColorTable[2], pcColorTable[3]); 
		}

	}
	else 
	{
		BYTE *pcGlobalColor=GlobalColorTable;
		for(int i=0;i<GlobalColorSize;i++)
		{
			pcColorTable[0]=*pcGlobalColor++;
			pcColorTable[1]=*pcGlobalColor++;
			pcColorTable[2]=*pcGlobalColor++;
			pcColorTable[3]=255;
			geBitmap_Palette_SetEntryColor(Palette, i, 
				pcColorTable[0], pcColorTable[1], 
				pcColorTable[2], pcColorTable[3]); 
		}
	}
	uPrimaryBitSize=uBitSize=(*pcGifTrack++);
	TotalReadByte++;

	iPrimaryTableSize=iTableSize=(1<<uBitSize)+2;
	iFinishCode=iTableSize-1;
	iResetCode=iFinishCode-1;

	uPrimaryBitSize++;
	uBitSize++;
	uRemain=0;
	cCurentByte=0;
	uBlockSize=0;
	uReadByte=1;
	x=y=0;
	iPass=1;iRow=0;
	while((code=GetCode())!=iFinishCode)
	{
		if(code==iResetCode)
		{
			uBitSize=uPrimaryBitSize;
			iTableSize=iPrimaryTableSize;
			oldcode=GetCode();
			if(oldcode>iTableSize)
			{
				delete pcBitmap;
				delete pcGifTable;
				return false;
			}
			Output((BYTE)oldcode);
			continue;
		}
		if(code<iTableSize)
		{
			code1=code;
			WORD code2=0;
			while(code1>=iPrimaryTableSize)
			{
				pcGifTable[code1].nextcode=code2;
				code2=code1;
				code1=pcGifTable[code1].previouscode;
				if(code1>=code2)
				{
					delete pcBitmap;
					delete pcGifTable;
					return false;
				}
			}
			Output((BYTE)code1);
			while(code2!=0)
			{
				Output(pcGifTable[code2].bit);
				code2=pcGifTable[code2].nextcode;
			}
			pcGifTable[iTableSize].bit=(BYTE)code1;
			pcGifTable[iTableSize].previouscode=oldcode;
			iTableSize++;
			if(iTableSize==(0x0001<<uBitSize))
				uBitSize++;
			if(uBitSize>12)
				uBitSize=12;
			oldcode=code;
		} 
		else 
		{
			code1=oldcode;
			WORD code2=0;
			while(code1>=iPrimaryTableSize)
			{
				pcGifTable[code1].nextcode=code2;
				code2=code1;
				code1=pcGifTable[code1].previouscode;
				if(code1>=code2)
				{
					delete pcBitmap;
					delete pcGifTable;
					return false;
				}
			}
			Output((BYTE)code1);
			while(code2!=0)
			{
				Output(pcGifTable[code2].bit);
				code2=pcGifTable[code2].nextcode;
			}
			Output((BYTE)code1);
			pcGifTable[iTableSize].bit=(BYTE)code1;
			pcGifTable[iTableSize].previouscode=oldcode;
			iTableSize++;
			if(iTableSize==(0x0001<<uBitSize))
				uBitSize++;
			if(uBitSize>12)
				uBitSize=12;
			oldcode=code;
		}
	}
// changed RF064
	geBitmap_SetFormat(theBmp,GE_PIXELFORMAT_8BIT_PAL,GE_TRUE,0,Palette);
// end change RF064
	geBitmap_SetPalette(theBmp, Palette);
	geBitmap_LockForWriteFormat(theBmp,&LockedBMP,0,0, GE_PIXELFORMAT_8BIT);
	if(LockedBMP == NULL)
    {
		geBitmap_SetFormat(theBmp,GE_PIXELFORMAT_8BIT,GE_TRUE,0,Palette);
		geBitmap_LockForWriteFormat(theBmp,&LockedBMP,0,0, GE_PIXELFORMAT_8BIT);
		if(LockedBMP == NULL)
		{
			delete pcBitmap;
			delete pcGifTable;
			return false; 
		}
    }
	unsigned char *wptr, *pptr;
	wptr = (LPBYTE)geBitmap_GetBits(LockedBMP);
	pptr = (LPBYTE)pcbmp;

	memset(wptr, TransparentIndex, nWidth*nHeight);

	wptr += ((iTop*nWidth)+iLeft);
	int Temp1 = iWidth1;

	for(int yy=0; yy < iHeight; yy++)
	{
		__asm
		{
			mov esi, pptr
			mov edi, wptr
			mov ecx, Temp1
			rep movs
		}

		wptr += nWidth;
		pptr += iWidth1;
	}

	geBitmap_UnLock(LockedBMP);

	if(bTransparentIndex)
		geBitmap_SetColorKey(theBmp, GE_TRUE, TransparentIndex, GE_FALSE);

	pcGifTrack++;
	TotalReadByte++;
	delete pcBitmap;
	delete pcGifTable;
	return true;
}

void CAnimGif::Output(BYTE bit)
{
	int tmp;
	if(cPackedField&0x40)
	{
		if(x==iWidth)
		{
			x=0;
			if(iPass==1)iRow+=8;
			if(iPass==2)iRow+=8;
			if(iPass==3)iRow+=4;
			if(iPass==4)iRow+=2;
			if(iRow>=iHeight){iPass+=1;iRow=16>>iPass;}
		}
		tmp=iRow*iWidth1+x;
		pcBitmap[tmp]=bit;
		x++;
	}
	else
	{
		if(x==iWidth){x=0;y++;}
		tmp=y*iWidth1+x;
		x++;
	}
	if(tmp>iMaxByte)return;
	pcBitmap[tmp]=bit;
}

BYTE CAnimGif::GetByte(void)
{
	if(uReadByte>=uBlockSize)
	{
		uBlockSize=*pcGifTrack++;
		uReadByte=0;
		TotalReadByte+=uBlockSize+1;
		if(TotalReadByte>GifSize)
		{
			TotalReadByte-=uBlockSize+1;
			return 0xFF;
		}
		if(uBlockSize==0)
		{
			pcGifTrack--;
			TotalReadByte--;
			return 0xFF;
		}
	}
	uReadByte++;
	return *pcGifTrack++;
}

WORD CAnimGif::GetCode(void)
{
	UINT tmp1;
	BYTE tmp;
	tmp=1;
	if(uRemain>=uBitSize)
	{
		tmp<<=uBitSize;
		tmp--;
		tmp1=cCurentByte&tmp;
		cCurentByte>>=uBitSize;
		uRemain-=uBitSize;
	}
	else
	{
		tmp<<=uRemain;
		tmp--;
		tmp1=cCurentByte;
		cCurentByte=GetByte();
		tmp=1;
		if(8>=(uBitSize-uRemain))
		{
			tmp<<=(uBitSize-uRemain);
			tmp--;
			tmp1=(((UINT)(cCurentByte&tmp))<<uRemain)+tmp1;
			cCurentByte>>=(uBitSize-uRemain);
			uRemain=8-(uBitSize-uRemain);
		}
		else
		{
			tmp1=(((UINT)(cCurentByte))<<uRemain)+tmp1;
			cCurentByte=GetByte();
			tmp<<=uBitSize-uRemain-8;
			tmp--;
			tmp1=(((UINT)(cCurentByte&tmp))<<(uRemain+8))+tmp1;
			cCurentByte>>=uBitSize-uRemain-8;
			uRemain=8-(uBitSize-uRemain-8);
		}
	}
	return tmp1;
}
