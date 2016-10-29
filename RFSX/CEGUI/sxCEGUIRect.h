/************************************************************************************//**
 * @file sxCEGUIRect.h
 * @brief Class declaration for RFSX Rect object
 * @author Daniel Queteschiner
 * @date November 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_RECT_H_
#define _SX_RECT_H_

#include "RFSX.h"

// sxRect class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		sxVector2 getPosition();
		float getWidth();
		float getHeight();
		sxSize getSize();

		void setPosition(const Vector2& p);
		void setWidth(float w);
		void setHeight(float h);
		void setSize(const Size& sz);

		sxRect getIntersection(sxRect rect);
		bool isPointInRect(sxVector2 p);

		sxRect offset(sxVector2 p);
		sxRect constrainSizeMax(sxSize sz);
		sxRect constrainSizeMin(sxSize sz);
		sxRect constrainSize(sxSize min, sxSize max);

	Operators:
		equals

	Fields:
	// type_codes: RO=Read-only WO=Write-only RW=Read/Write
		float left;(RW)
		float top;(RW)
		float right;(RW)
		float bottom;(RW)
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxRect
#define SXRECT_METHODS 15

// Type-check and upcast macro
#define IS_RECT(o)	 CheckType<RFSX::sxRect>(o, RFSXU_RECT)

namespace RFSX
{

class SX_IMPL_TYPE: public skExecutable
{
public:
	SX_IMPL_TYPE();
	SX_IMPL_TYPE(const CEGUI::Rect& val);
	SX_IMPL_TYPE(float left, float top, float right, float bottom);
	SX_IMPL_TYPE(CEGUI::Point pos, CEGUI::Size sz);
	virtual ~SX_IMPL_TYPE();
	static void InitMHT();

	virtual bool setValue(const skString& name, const skString& att, const skRValue& val);
	virtual bool getValue(const skString& name, const skString& att, skRValue& val);
	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);
	virtual bool equals(const skiExecutable *other_object) const;
	virtual int executableType() const { return RFSXU_RECT; }

	CEGUI::Rect* Rect() { return &m_Rect; }
	const CEGUI::Rect& RectConstRef() const { return m_Rect; }

protected:
	SX_METHOD_DECL(getPosition);
	SX_METHOD_DECL(getWidth);
	SX_METHOD_DECL(getHeight);
	SX_METHOD_DECL(getSize);
	SX_METHOD_DECL(setPosition);
	SX_METHOD_DECL(setWidth);
	SX_METHOD_DECL(setHeight);
	SX_METHOD_DECL(setSize);
	SX_METHOD_DECL(getIntersection);
	SX_METHOD_DECL(offset);
	SX_METHOD_DECL(isPointInRect);
	SX_METHOD_DECL(constrainSizeMax);
	SX_METHOD_DECL(constrainSizeMin);
	SX_METHOD_DECL(constrainSize);
	SX_METHOD_DECL(scale);

private:
	CEGUI::Rect m_Rect;

	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_RECT_H_
