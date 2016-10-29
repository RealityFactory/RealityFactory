/************************************************************************************//**
 * @file CEGUIGenesis3DRenderer.cpp
 * @brief Implementation of Genesis3D Renderer for CEGUI
 * @author Daniel Queteschiner
 * @date July 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include <Genesis.h>
#include <CEGUIExceptions.h>
#include "CEGUIGenesis3DRenderer.h"
#include "CEGUIGenesis3DTexture.h"

#define DRV_RENDER_ALPHA		(1<<0)	///< Render function should expect alpha set in vertices
#define DRV_RENDER_FLUSH		(1<<1)	///< Render function should gaurentee that this render happens NOW
#define DRV_RENDER_NO_ZMASK		(1<<2)	///< No zbuffering should be performed
#define DRV_RENDER_NO_ZWRITE	(1<<3)	///< No z writing will be performed
#define DRV_RENDER_CLAMP_UV		(1<<4)	///< Clamp UV in both directions

namespace CEGUI
{

Genesis3DRenderer::Genesis3DRenderer(geEngine *engine, int width, int height, geWorld *world)
{
	d_engine = engine;
	d_world = world;

	/* TODO: get max texture size from driver; implement: int geEngine_MaxTextureSize(geEngine*) */
	d_maxTextureSize = 2048;
	/* TODO
	const char* driverName;
	geDriver_System* driverSystem;
	geDriver* driver;
	geDriver_System* geEngine_GetDriverSystem(geEngine* Engine);
	// geDriver* geEngine_GetCurrentDriver();
	geBoolean geDriver_GetName(geDriver *Driver, const char **Name);
	if(strstr(driverName, "D3D") != NULL)
		d_correctPixelMapping = true;*/
	d_correctPixelMapping = true;

	d_currTexture = NULL;

	d_displayArea.d_left	= 0;
	d_displayArea.d_top		= 0;
	d_displayArea.d_right	= static_cast<float>(width);
	d_displayArea.d_bottom	= static_cast<float>(height);

	d_queueing = true;

	d_identifierString = "CEGUI::Genesis3DRenderer - Genesis3D based renderer module for CEGUI";
}


Genesis3DRenderer::~Genesis3DRenderer()
{
	destroyAllTextures();
}


void Genesis3DRenderer::addQuad(const Rect& dest_rect, float z, const Texture* tex,
	const Rect& texture_rect, const ColourRect& colours, QuadSplitMode quad_split_mode)
{
	QuadInfo quad;

	quad.position			= dest_rect;
	quad.z					= z + 0.9f; // opengl driver needs this offset
	quad.texture			= ((Genesis3DTexture*)tex)->getTexture();
	quad.texPosition		= texture_rect;

	quad.topLeftCol.a		= colours.d_top_left.getAlpha();
	quad.topLeftCol.r		= colours.d_top_left.getRed();
	quad.topLeftCol.g		= colours.d_top_left.getGreen();
	quad.topLeftCol.b		= colours.d_top_left.getBlue();

	quad.topRightCol.a		= colours.d_top_right.getAlpha();
	quad.topRightCol.r		= colours.d_top_right.getRed();
	quad.topRightCol.g		= colours.d_top_right.getGreen();
	quad.topRightCol.b		= colours.d_top_right.getBlue();

	quad.bottomLeftCol.a	= colours.d_bottom_left.getAlpha();
	quad.bottomLeftCol.r	= colours.d_bottom_left.getRed();
	quad.bottomLeftCol.g	= colours.d_bottom_left.getGreen();
	quad.bottomLeftCol.b	= colours.d_bottom_left.getBlue();

	quad.bottomRightCol.a	= colours.d_bottom_right.getAlpha();
	quad.bottomRightCol.r	= colours.d_bottom_right.getRed();
	quad.bottomRightCol.g	= colours.d_bottom_right.getGreen();
	quad.bottomRightCol.b	= colours.d_bottom_right.getBlue();

	// set quad split mode
	quad.splitMode = quad_split_mode;

	// offset destination to get correct texel to pixel mapping from Direct3D
	if(d_correctPixelMapping)
		quad.position.offset(Point(-0.5f, -0.5f));

	if(!d_queueing)
	{
		renderQuadDirect(quad);
	}
	else
	{
		d_quadList.insert(quad);
	}
}


void Genesis3DRenderer::doRender(void)
{
	for(QuadList::iterator i = d_quadList.begin(); i != d_quadList.end(); ++i)
	{
		const QuadInfo& quad = (*i);
		renderQuadDirect(quad);
	}
}


void Genesis3DRenderer::renderQuadDirect(const QuadInfo &quad)
{
	GE_TLVertex vertex[4];

	vertex[0].x = quad.position.d_left;
	vertex[0].y = quad.position.d_top;
	vertex[0].z = quad.z + 0.9f; // opengl driver needs this offset
	vertex[0].r = 255.f * quad.topLeftCol.r;
	vertex[0].g = 255.f * quad.topLeftCol.g;
	vertex[0].b = 255.f * quad.topLeftCol.b;
	vertex[0].a = 255.f * quad.topLeftCol.a;
	vertex[0].u = quad.texPosition.d_left;
	vertex[0].v = quad.texPosition.d_top;

	vertex[1].x = quad.position.d_right;
	vertex[1].y = quad.position.d_top;
	vertex[1].z = quad.z + 0.9f; // opengl driver needs this offset
	vertex[1].r = 255.f * quad.topRightCol.r;
	vertex[1].g = 255.f * quad.topRightCol.g;
	vertex[1].b = 255.f * quad.topRightCol.b;
	vertex[1].a = 255.f * quad.topRightCol.a;
	vertex[1].u = quad.texPosition.d_right;
	vertex[1].v = quad.texPosition.d_top;

	vertex[2].x = quad.position.d_right;
	vertex[2].y = quad.position.d_bottom;
	vertex[2].z = quad.z + 0.9f; // opengl driver needs this offset
	vertex[2].r = 255.f * quad.bottomRightCol.r;
	vertex[2].g = 255.f * quad.bottomRightCol.g;
	vertex[2].b = 255.f * quad.bottomRightCol.b;
	vertex[2].a = 255.f * quad.bottomRightCol.a;
	vertex[2].u = quad.texPosition.d_right;
	vertex[2].v = quad.texPosition.d_bottom;

	vertex[3].x = quad.position.d_left;
	vertex[3].y = quad.position.d_bottom;
	vertex[3].z = quad.z + 0.9f; // opengl driver needs this offset
	vertex[3].r = 255.f * quad.bottomLeftCol.r;
	vertex[3].g = 255.f * quad.bottomLeftCol.g;
	vertex[3].b = 255.f * quad.bottomLeftCol.b;
	vertex[3].a = 255.f * quad.bottomLeftCol.a;
	vertex[3].u = quad.texPosition.d_left;
	vertex[3].v = quad.texPosition.d_bottom;

	geBoolean alpha = GE_FALSE;

	if(vertex[0].a < 255.f || vertex[1].a < 255.f || vertex[2].a < 255.f || vertex[3].a < 255.f)
		alpha = GE_TRUE;

	geEngine_RenderPoly(d_engine, vertex, 4, quad.texture,
						(alpha ? DRV_RENDER_ALPHA : 0)
						| DRV_RENDER_CLAMP_UV
						| DRV_RENDER_NO_ZMASK
						| DRV_RENDER_NO_ZWRITE
						| DRV_RENDER_FLUSH);
}


void Genesis3DRenderer::clearRenderList(void)
{
	d_quadList.clear();
}


void Genesis3DRenderer::setQueueingEnabled(bool setting)
{
	d_queueing = setting;
}


Texture* Genesis3DRenderer::createTexture(void)
{
	Genesis3DTexture *tex = new Genesis3DTexture(this);
	d_textureList.push_back(tex);
	return tex;
}


Texture* Genesis3DRenderer::createTexture(const String& filename, const String& resourceGroup)
{
	Genesis3DTexture* tex = new Genesis3DTexture(this);

	try
	{
		tex->loadFromFile(filename, resourceGroup);
	}
	catch(RendererException&)
	{
		delete tex;
		throw;
	}

	d_textureList.push_back(tex);

	return tex;
}


Texture* Genesis3DRenderer::createTexture(float size)
{
	Genesis3DTexture *tex = new Genesis3DTexture(this);

	try
	{
		tex->setG3DTextureSize((uint)size);
	}
	catch(RendererException&)
	{
		delete tex;
		throw;
	}

	d_textureList.push_back(tex);

	return tex;
}


void Genesis3DRenderer::destroyTexture(Texture *texture)
{
	if(texture)
	{
		Genesis3DTexture *tex = static_cast<Genesis3DTexture*>(texture);
		d_textureList.remove(tex);
		delete tex;
	}
}


void Genesis3DRenderer::destroyAllTextures()
{
	while(!d_textureList.empty())
	{
		destroyTexture(*(d_textureList.begin()));
	}
}


bool Genesis3DRenderer::isQueueingEnabled(void) const
{
	return d_queueing;
}


float Genesis3DRenderer::getWidth(void) const
{
	return d_displayArea.getWidth();
}


float Genesis3DRenderer::getHeight(void) const
{
	return d_displayArea.getHeight();
}


Size Genesis3DRenderer::getSize(void) const
{
	return d_displayArea.getSize();
}


Rect Genesis3DRenderer::getRect(void) const
{
	return d_displayArea;
}


uint Genesis3DRenderer::getMaxTextureSize(void) const
{
	return d_maxTextureSize;
}


uint Genesis3DRenderer::getHorzScreenDPI(void) const
{
	return 96;
}


uint Genesis3DRenderer::getVertScreenDPI(void) const
{
	return 96;
}

} // end CEGUI namespace
