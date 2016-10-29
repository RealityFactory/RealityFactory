/************************************************************************************//**
 * @file CEGUIGenesis3DRenderer.h
 * @brief Class declaration for Genesis3D Renderer for CEGUI
 * @author Daniel Queteschiner
 * @date July 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _CEGUI_Genesis3DRenderer_h_
#define _CEGUI_Genesis3DRenderer_h_

#include <set>
#include <list>
#include <Genesis.h>
#include <CEGUIRenderer.h>
#include <CEGUITexture.h>

namespace CEGUI
{

// Forward refs
class Genesis3DTexture;


class Genesis3DRenderer : public Renderer
{
public:
	Genesis3DRenderer(geEngine *engine, int width, int height, geWorld *world);
	virtual ~Genesis3DRenderer();

	/**
	 * @brief Add a quad to the rendering queue.  All clipping and other
	 *        adjustments should have been made prior to calling this.
	 * @param dest_rect	Rect object describing the destination area (values are in pixels)
	 * @param z float value specifying the z co-ordinate / z order of the quad
	 * @param tex pointer to the Texture object that holds the imagery to be rendered
	 * @param texture_rect Rect object holding the area of \a tex that is to be rendered
	 *        (values are in texture co-ordinates).
	 * @param colours ColourRect object describing the colour values that are to be applied when rendering.
	 * @param quad_split_mode One of the QuadSplitMode values specifying the way quads are split into triangles
	 * @return Nothing
	 */
	virtual	void addQuad(const Rect& dest_rect, float z, const Texture* tex,
		const Rect& texture_rect, const ColourRect& colours, QuadSplitMode quad_split_mode);

	/**
	 * @brief Perform final rendering for all quads that have been queued for rendering
	 *        The contents of the rendering queue is retained and can be rendered again as required.
	 *        If the contents is not required call clearRenderList().
	 * @return Nothing
	 */
	virtual	void doRender(void);

	/**
	 * @brief Clears all queued quads from the render queue.
	 * @return Nothing
	 */
	virtual	void clearRenderList(void);

	/**
	 * @brief Enable or disable the queueing of quads from this point on.
	 *        This only affects queueing. If queueing is turned off, any calls to addQuad
	 *        will cause the quad to be rendered directly.  Note that disabling queueing
	 *        will not cause currently queued quads to be rendered, nor is the queue cleared
	 *        - at any time the queue can still be drawn by calling doRender, and the list
	 *        can be cleared by calling clearRenderList.  Re-enabling the queue causes
	 *        subsequent quads to be added as if queueing had never been disabled.
	 * @param setting true to enable queueing, or false to disable queueing (see notes above).
	 * @return Nothing
	 */
	virtual void setQueueingEnabled(bool setting);

	/**
	 * @brief Creates a 'null' Texture object.
	 * @return a newly created Texture object. The returned Texture object has no size or
	 *         imagery associated with it, and is generally of little or no use.
	 */
	virtual	Texture* createTexture(void);

	/**
	 * @brief Create a Texture object using the given image file.
	 * @param filename String object that specifies the path and filename of the image
	 *        file to use when creating the texture.
	 * @param resourceGroup Resource group identifier to be passed to the resource provider
	 *        when loading the texture file.
	 * @return a newly created Texture object. The initial contents of the texture memory
	 *         is the requested image file.
	 * @note Textures are always created with a size that is a power of 2. If the file you specify is
	 *       of a size that is not a power of two, the final size will be rounded up. Additionally,
	 *		 textures are always square, so the ultimate size is governed by the larger of the width
	 *		 and height of the specified file.  You can check the ultimate sizes by querying the
	 *		 texture after creation.
	 */
	virtual	Texture* createTexture(const String& filename, const String& resourceGroup);

	/**
	 * @brief Create a Texture object with the given pixel dimensions as specified by \a size.
	 *        NB: Textures are always square.
	 * @param size float value that specifies the size to use for the width and height when creating the new texture.
	 * @return a newly created Texture object. The initial contents of the texture memory is undefined / random.
	 * @note Textures are always created with a size that is a power of 2. If you specify a size that is not a power
	 *       of two, the final size will be rounded up.  So if you specify a size of 1024, the texture will be
	 *       (1024 x 1024), however, if you specify a size of 1025, the texture will be (2048 x 2048).
	 *        You can check the ultimate size by querying the texture after creation.
	 */
	virtual	Texture* createTexture(float size);

	/**
	 * @brief Destroy the given Texture object.
	 * @param texture pointer to the Texture object to be destroyed
	 * @return Nothing
	 */
	virtual	void destroyTexture(Texture* texture);

	/**
	 * @brief Destroy all Texture objects.
	 * @return Nothing
	 */
	virtual void destroyAllTextures(void);

	/**
	 * @brief Return whether queueing is enabled.
	 * @return true if queueing is enabled, false if queueing is disabled.
	 */
	virtual bool isQueueingEnabled(void) const;

	/**
	 * @brief Return the current width of the display in pixels
	 * @return float value equal to the current width of the display in pixels.
	 */
	virtual float getWidth(void) const;

	/**
	 * @brief Return the current height of the display in pixels
	 * @return float value equal to the current height of the display in pixels.
	 */
	virtual float getHeight(void) const;

	/**
	 * @brief Return the size of the display in pixels
	 * @return Size object describing the dimensions of the current display.
	 */
	virtual Size getSize(void) const;

	/**
	 * @brief Return a Rect describing the screen
	 * @return A Rect object that describes the screen area. Typically, the top-left
	 *         values are always 0, and the size of the area described is equal to
	 *         the screen resolution.
	 */
	virtual Rect getRect(void) const;

	/**
	 * @brief Return the maximum texture size available
	 * @return Size of the maximum supported texture in pixels
	 *         (textures are always assumed to be square)
	 */
	virtual	uint getMaxTextureSize(void) const;

	/**
	 * @brief Return the horizontal display resolution dpi
	 * @return horizontal resolution of the display in dpi.
	 */
	virtual	uint getHorzScreenDPI(void) const;

	/**
	 * @brief Return the vertical display resolution dpi
	 * @return vertical resolution of the display in dpi.
	 */
	virtual	uint getVertScreenDPI(void) const;

	geEngine* getEngine() { return d_engine; }
	geWorld* getWorld() { return d_world; }

private:
	/**
	 * @brief structure holding details about a quad to be drawn
	 */
	struct QuadInfo
	{
		geBitmap		*texture;
		Rect			position;
		float			z;
		Rect			texPosition;
		GE_RGBA			topLeftCol;
		GE_RGBA			topRightCol;
		GE_RGBA			bottomLeftCol;
		GE_RGBA			bottomRightCol;

		QuadSplitMode	splitMode;

		bool operator<(const QuadInfo& other) const
		{
			// this is intentionally reversed.
			return z > other.z;
		}
	};

	/*************************************************************************
		Implementation Methods
	*************************************************************************/

	// render a quad directly to the display
	void renderQuadDirect(const QuadInfo &quad);

	/*************************************************************************
		Implementation Data
	*************************************************************************/
	typedef std::multiset<QuadInfo> QuadList;

	Rect			d_displayArea;
	QuadList		d_quadList;
	QuadList		d_directQuadList;
	bool			d_queueing;			//!< setting for queueing control.

	geBitmap		*d_currTexture;		//!< currently set texture;
	std::list<Genesis3DTexture*>	d_textureList;		//!< List used to track textures.

	uint			d_maxTextureSize;	//!< Holds maximum supported texture size (in pixels).
	bool			d_correctPixelMapping;

	geEngine		*d_engine;
	geWorld			*d_world;
};

}

#endif
