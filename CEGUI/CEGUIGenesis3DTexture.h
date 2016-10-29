/************************************************************************************//**
 * @file CEGUIGenesis3DTexture.h
 * @brief Class declaration for Genesis3D Texture for CEGUI
 * @author Daniel Queteschiner
 * @date July 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _CEGUI_Genesis3DTexture_h_
#define _CEGUI_Genesis3DTexture_h_

#include <list>
#include <CEGUIBase.h>
#include <CEGUIRenderer.h>
#include <CEGUITexture.h>
#include "CEGUIGenesis3DRenderer.h"

// Start of CEGUI namespace section
namespace CEGUI
{

class Genesis3DTexture : public Texture
{
private:
	/*************************************************************************
		Friends (to allow construction and destruction)
	*************************************************************************/
	friend	Texture* Genesis3DRenderer::createTexture(void);
	friend	Texture* Genesis3DRenderer::createTexture(const String& filename, const String& resourceGroup);
	friend	Texture* Genesis3DRenderer::createTexture(float size);
	friend	void	 Genesis3DRenderer::destroyTexture(Texture* texture);

	explicit Genesis3DTexture(Renderer *owner);
	virtual ~Genesis3DTexture(void);

public:

	/**
	 * @brief Returns the current pixel width of the texture
	 * @return ushort value that is the current width of the texture in pixels
	 */
	virtual ushort getWidth(void) const;

	/**
	 * @brief Returns the current pixel height of the texture
	 * @return ushort value that is the current height of the texture in pixels
	 */
	virtual ushort getHeight(void) const;

	/**
	 * @brief Loads the specified image file into the texture.
	 *        The texture is resized as required to hold the image.
	 * @param filename The filename of the image file that is to be loaded into the texture
	 * @param resourceGroup Resource group identifier to be passed to the resource provider
	 *        when loading the image file.
	 * @return Nothing.
	*/
	virtual void loadFromFile(const String& filename, const String& resourceGroup);

	/**
	 * @brief Loads (copies) an image in memory into the texture.
	 *        The texture is resized as required to hold the image.
	 * @param buffPtr Pointer to the buffer containing the image data
	 * @param buffWidth Width of the buffer (in pixels as specified by \a pixelFormat )
	 * @param buffHeight Height of the buffer (in pixels as specified by \a pixelFormat )
	 * @param pixelFormat PixelFormat value describing the format contained in \a buffPtr
	 * @return Nothing.
	 */
	virtual void loadFromMemory(const void* buffPtr, uint buffWidth, uint buffHeight, PixelFormat pixelFormat);

	/**
	 * @brief set the size of the internal D3D texture.  Previous D3D texture is lost.
	 * @param size pixel size of the new internal texture.  This will be rounded up to a power of 2.
	 * @return Nothing.
	 */
	void setG3DTextureSize(uint size);
	void freeG3DTexture();

	geBitmap* getTexture();

private:
	geBitmap*	d_bitmap;
	String		d_filename;			//!< name of file used to create the texture, if any.
	String		d_resourceGroup;    //!< resource provider group ID to use when loading file.
	bool		d_isMemoryTexture;	//!< true if the texture was created from memory (and not a file).
	ushort		d_width;			//!< cached width of the texture
	ushort		d_height;			//!< cached height of the texture
};

}

#endif
