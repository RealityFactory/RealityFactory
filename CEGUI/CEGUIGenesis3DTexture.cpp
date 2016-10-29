/************************************************************************************//**
 * @file CEGUIGenesis3DTexture.cpp
 * @brief Implementation of Genesis3D Texture for CEGUI
 * @author Daniel Queteschiner
 * @date July 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include <Genesis.h>
#include <CEGUIExceptions.h>
#include <CEGUISystem.h>
#include "CEGUIGenesis3DTexture.h"
#include "CEGUIGenesis3DResourceProvider.h"
#include "Utilities.h"


namespace CEGUI
{

Genesis3DTexture::Genesis3DTexture(Renderer *owner) : Texture(owner)
{
	d_bitmap = NULL;
	d_isMemoryTexture = true;
	d_width = 0;
	d_height = 0;
}


Genesis3DTexture::~Genesis3DTexture(void)
{
	freeG3DTexture();
}


void Genesis3DTexture::freeG3DTexture()
{
	if(d_bitmap)
	{
		geWorld_RemoveBitmap(((Genesis3DRenderer*)getRenderer())->getWorld(), d_bitmap);
		geBitmap_Destroy(&d_bitmap);
		d_bitmap = NULL;
	}

	d_filename.clear();
	d_isMemoryTexture = true;
	d_width = 0;
	d_height = 0;
}


ushort Genesis3DTexture::getWidth(void) const
{
	return d_width;
}


ushort Genesis3DTexture::getHeight(void) const
{
	return d_height;
}


void Genesis3DTexture::loadFromFile(const String& filename, const String& resourceGroup)
{
	freeG3DTexture();

	String final_filename;

	final_filename = ((Genesis3DResourceProvider*)(CEGUI::System::getSingleton().getResourceProvider()))->getResourceGroupDirectory(resourceGroup);
	final_filename += filename;

	d_bitmap = CreateFromFileName(final_filename.c_str(), 0x0fff); //kRawFile

	if(d_bitmap)
	{
		if(geWorld_AddBitmap(((Genesis3DRenderer*)getRenderer())->getWorld(), d_bitmap) == GE_FALSE)
		{
			geBitmap_Destroy(&d_bitmap);
			d_bitmap = NULL;
		}
		else
		{
			d_filename = filename;
			d_resourceGroup = resourceGroup;
			d_isMemoryTexture = false;
			d_width = static_cast<ushort>(geBitmap_Width(d_bitmap));
			d_height = static_cast<ushort>(geBitmap_Height(d_bitmap));
		}
	}
	else
	{
		freeG3DTexture();
	}
}


void Genesis3DTexture::loadFromMemory(const void* buffPtr, uint buffWidth, uint buffHeight, PixelFormat pixelFormat)
{
	freeG3DTexture();

	geBitmap_Info info;
	geBitmap *alpha_bmp = NULL;
	geBitmap *lock		= NULL;
	geBitmap *alpha_lock= NULL;
	uint8 *bptr			= NULL;
	uint8 *abptr		= NULL;
	const uint8 *src	= static_cast<const uint8*>(buffPtr);
	int x, y;

	d_bitmap = geBitmap_Create(buffWidth, buffHeight, 1, GE_PIXELFORMAT_24BIT_BGR);

	geBitmap_ClearMips(d_bitmap);

	geBitmap_LockForWrite(d_bitmap, &lock, 0, 0);

	geBitmap_GetInfo(lock, &info, NULL);

	bptr = static_cast<uint8*>(geBitmap_GetBits(lock));

	if(pixelFormat == PF_RGBA)
	{
		alpha_bmp = geBitmap_Create(buffWidth, buffHeight, 1, GE_PIXELFORMAT_8BIT_GRAY);

		if(geBitmap_LockForWrite(alpha_bmp, &alpha_lock, 0, 0))
			abptr = static_cast<uint8*>(geBitmap_GetBits(alpha_lock));
	}

	for(y=0; y<info.Height; ++y)
	{
		for(x=0; x<info.Width; ++x)
		{
			*bptr++ = *src++;
			*bptr++ = *src++;
			*bptr++ = *src++;

			if(pixelFormat == PF_RGBA && abptr)
				*abptr++ = *src++;
		}

		// note that bptr is a char pointer, and Stride is in pixels :
		bptr += (info.Stride - info.Width) * 3;

		if(pixelFormat == PF_RGBA && abptr)
			abptr += info.Stride - info.Width;
	}

	geBitmap_UnLock(lock);

	if(alpha_bmp)
	{
		geBitmap_UnLock(alpha_lock);

		if(abptr)
		{
			geBitmap_SetAlpha(d_bitmap, alpha_bmp);
			geBitmap_Destroy(&alpha_bmp);
			geBitmap_SetPreferredFormat(d_bitmap, GE_PIXELFORMAT_32BIT_ARGB);
		}
	}

	geBitmap_RefreshMips(d_bitmap);

	geWorld_AddBitmap(((Genesis3DRenderer*)getRenderer())->getWorld(), d_bitmap);

	d_filename.clear();
	d_isMemoryTexture = true;
	d_width = buffWidth;
	d_height = buffHeight;
}


void Genesis3DTexture::setG3DTextureSize(uint size)
{
	freeG3DTexture();

	d_bitmap = geBitmap_Create(size, size, 1, GE_PIXELFORMAT_32BIT_RGBA);
	geWorld_AddBitmap(((Genesis3DRenderer*)getRenderer())->getWorld(), d_bitmap);

	d_filename.clear();
	d_isMemoryTexture = true;
	d_width = size;
	d_height = size;
}


geBitmap *Genesis3DTexture::getTexture()
{
	return d_bitmap;
}

}
