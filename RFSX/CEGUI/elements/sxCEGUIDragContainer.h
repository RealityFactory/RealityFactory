/************************************************************************************//**
 * @file sxCEGUIDragContainer.h
 * @brief Class declaration for RFSX DragContainer object
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_DRAGCONTAINER_H_
#define _SX_DRAGCONTAINER_H_

#include "RFSX.h"
#include "../sxCEGUIWindow.h"

// sxDragContainer class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		bool isDraggingEnabled();
		void setDraggingEnabled(bool setting);
		bool isBeingDragged();
		float getPixelDragThreshold();
		void setPixelDragThreshold(float pixels);
		float getDragAlpha();
		void setDragAlpha(float alpha);
		sxImage getDragCursorImage();
		void setDragCursorImage(skString imageset, skString image);
		void setDragCursorImage(sxImage image);
		sxWindow getCurrentDropTarget();	-- may return null!
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxDragContainer

#define SXDRAGCONTAINER_METHODS 10

namespace RFSX
{

class SX_IMPL_TYPE: public sxWindow
{
public:
	SX_IMPL_TYPE(CEGUI::DragContainer* dragContainer) : sxWindow(dragContainer) {}
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);

	CEGUI::DragContainer* DragContainer() { return static_cast<CEGUI::DragContainer*>(Window()); }

protected:
	SX_METHOD_DECL(isDraggingEnabled);
	SX_METHOD_DECL(setDraggingEnabled);
	SX_METHOD_DECL(isBeingDragged);
	SX_METHOD_DECL(getPixelDragThreshold);
	SX_METHOD_DECL(setPixelDragThreshold);
	SX_METHOD_DECL(getDragAlpha);
	SX_METHOD_DECL(setDragAlpha);
	SX_METHOD_DECL(getDragCursorImage);
	SX_METHOD_DECL(setDragCursorImage);
	SX_METHOD_DECL(getCurrentDropTarget);

private:
	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_DRAGCONTAINER_H_
