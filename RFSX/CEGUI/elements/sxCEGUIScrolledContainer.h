/************************************************************************************//**
 * @file sxCEGUIScrolledContainer.h
 * @brief Class declaration for RFSX ScrolledContainer object
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_SCROLLEDCONTAINER_H_
#define _SX_SCROLLEDCONTAINER_H_

#include "RFSX.h"
#include "../sxCEGUIWindow.h"

// sxScrolledContainer class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		bool isContentPaneAutoSized() const;
		void setContentPaneAutoSized(bool setting);

		const Rect& getContentArea() const;
		void setContentArea(const Rect& area);

		Rect getChildExtentsArea() const;
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxScrolledContainer

#define SXSCROLLEDCONTAINER_METHODS 5

namespace RFSX
{

class SX_IMPL_TYPE: public sxWindow
{
public:
	SX_IMPL_TYPE(CEGUI::ScrolledContainer* container) : sxWindow(container) {}
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);

	CEGUI::ScrolledContainer* ScrolledContainer() { return static_cast<CEGUI::ScrolledContainer*>(Window()); }

protected:
	SX_METHOD_DECL(isContentPaneAutoSized);
	SX_METHOD_DECL(setContentPaneAutoSized);
	SX_METHOD_DECL(getContentArea);
	SX_METHOD_DECL(setContentArea);
	SX_METHOD_DECL(getChildExtentsArea);

private:
	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_SCROLLEDCONTAINER_H_
