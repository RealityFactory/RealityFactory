/************************************************************************************//**
 * @file sxCEGUIThumb.h
 * @brief Class declaration for RFSX Thumb object
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_THUMB_H_
#define _SX_THUMB_H_

#include "RFSX.h"
#include "sxCEGUIPushButton.h"

// sxThumb class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		bool isHotTracked();
		bool isVertFree();
		bool isHorzFree();
		void setHotTracked(bool setting);
		void setVertFree(bool setting);
		void setHorzFree(bool setting);
		sxVector2 getVertRange();
		sxVector2 getHorzRange();
		void setVertRange(float min, float max);
		void setHorzRange(float min, float max);
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxThumb

#define SXTHUMB_METHODS 10

namespace RFSX
{

class SX_IMPL_TYPE: public sxPushButton
{
public:
	SX_IMPL_TYPE(CEGUI::Thumb* thumb) : sxPushButton(thumb) {}
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);

	CEGUI::Thumb* Thumb() { return static_cast<CEGUI::Thumb*>(PushButton()); }

protected:
	SX_METHOD_DECL(isHotTracked);
	SX_METHOD_DECL(isVertFree);
	SX_METHOD_DECL(isHorzFree);
	SX_METHOD_DECL(setHotTracked);
	SX_METHOD_DECL(setVertFree);
	SX_METHOD_DECL(setHorzFree);
	SX_METHOD_DECL(getVertRange);
	SX_METHOD_DECL(getHorzRange);
	SX_METHOD_DECL(setVertRange);
	SX_METHOD_DECL(setHorzRange);

private:
	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_THUMB_H_
