/************************************************************************************//**
 * @file sxCEGUIDragContainer.cpp
 * @brief Implementation for RFSX DragContainer object.
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUIDragContainer.h"
#include "../sxCEGUIImage.h"


#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxDragContainer

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxDragContainer implementation
//////////////////////////////////////////////////////////////////////////////////////////

bool SX_IMPL_TYPE::method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
{
	if(h_method(this, m_name, args, r_val, ctxt))
		return true;
	else
		return sxWindow::method(m_name, args, r_val, ctxt);
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxDragContainer method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(isDraggingEnabled)
{
	//bool isDraggingEnabled() const;
	r_val = caller->DragContainer()->isDraggingEnabled();
	return true;
}


SX_METHOD_IMPL(setDraggingEnabled)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	//void setDraggingEnabled(bool setting);
	caller->DragContainer()->setDraggingEnabled(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(isBeingDragged)
{
	//bool isBeingDragged() const;
	r_val = caller->DragContainer()->isBeingDragged();
	return true;
}


SX_METHOD_IMPL(getPixelDragThreshold)
{
	//float getPixelDragThreshold() const;
	r_val = caller->DragContainer()->getPixelDragThreshold();
	return true;
}


SX_METHOD_IMPL(setPixelDragThreshold)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Float)
		return false;

	//void setPixelDragThreshold(float pixels);
	caller->DragContainer()->setPixelDragThreshold(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(getDragAlpha)
{
	//float getDragAlpha() const;
	r_val = caller->DragContainer()->getDragAlpha();
	return true;
}


SX_METHOD_IMPL(setDragAlpha)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Float)
		return false;

	//void setDragAlpha(float alpha);
	caller->DragContainer()->setDragAlpha(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(getDragCursorImage)
{
	//const Image* getDragCursorImage() const;
	r_val.assignObject(new sxImage(caller->DragContainer()->getDragCursorImage()), true);
	return true;
}


SX_METHOD_IMPL(setDragCursorImage)
{
	//void setDragCursorImage(const Image* image);
	if(args.entries() == 1)
	{
		sxImage *image = IS_IMAGE(args[0]);

		if(!image)
			return false;

		caller->DragContainer()->setDragCursorImage(image->Image());
		return true;
	}

	skRValue::RType rtypes[] = { skRValue::T_String, skRValue::T_String };

	if(!CheckParams(2, 2, args, rtypes))
		return false;

	//void setDragCursorImage(string imageset, string image);
	caller->DragContainer()->setDragCursorImage(
		reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()),
		reinterpret_cast<const CEGUI::utf8*>(args[1].str().c_str()));

	return true;
}


SX_METHOD_IMPL(getCurrentDropTarget)
{
	//Window* getCurrentDropTarget() const;
	CEGUI::Window *win = caller->DragContainer()->getCurrentDropTarget();

	if(win)
		r_val.assignObject(new sxWindow(win), true);
	else
		r_val.assignObject(&ctxt.getInterpreter()->getNull());

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxDragContainer initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXDRAGCONTAINER_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(isDraggingEnabled);
	SX_ADD_METHOD(setDraggingEnabled);
	SX_ADD_METHOD(isBeingDragged);
	SX_ADD_METHOD(getPixelDragThreshold);
	SX_ADD_METHOD(setPixelDragThreshold);
	SX_ADD_METHOD(getDragAlpha);
	SX_ADD_METHOD(setDragAlpha);
	SX_ADD_METHOD(getDragCursorImage);
	SX_ADD_METHOD(setDragCursorImage);
	SX_ADD_METHOD(getCurrentDropTarget);
}

} // namespace RFSX

#undef SX_IMPL_TYPE // And that's it in a nutshell
