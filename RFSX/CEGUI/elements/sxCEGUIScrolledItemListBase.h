/************************************************************************************//**
 * @file sxCEGUIScrolledItemListBase.h
 * @brief Class declaration for RFSX ScrolledItemListBase object
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_SCROLLEDITEMLISTBASE_H_
#define _SX_SCROLLEDITEMLISTBASE_H_

#include "RFSX.h"
#include "sxCEGUIItemListBase.h"

// sxScrolledItemListBase class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		bool isVertScrollbarAlwaysShown();
		bool isHorzScrollbarAlwaysShown();
		sxScrollbar getVertScrollbar();
		sxScrollbar getHorzScrollbar();
		void setShowVertScrollbar(bool mode);
		void setShowHorzScrollbar(bool mode);
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxScrolledItemListBase

#define SXSCROLLEDITEMLISTBASE_METHODS 6

namespace RFSX
{

class SX_IMPL_TYPE: public sxItemListBase
{
public:
	SX_IMPL_TYPE(CEGUI::ScrolledItemListBase* listBase) : sxItemListBase(listBase) {}
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);

	CEGUI::ScrolledItemListBase* ScrolledItemListBase()	{ return static_cast<CEGUI::ScrolledItemListBase*>(ItemListBase()); }

protected:
	SX_METHOD_DECL(isVertScrollbarAlwaysShown);
	SX_METHOD_DECL(isHorzScrollbarAlwaysShown);
	SX_METHOD_DECL(getVertScrollbar);
	SX_METHOD_DECL(getHorzScrollbar);
	SX_METHOD_DECL(setShowVertScrollbar);
	SX_METHOD_DECL(setShowHorzScrollbar);

private:
	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_SCROLLEDITEMLISTBASE_H_
