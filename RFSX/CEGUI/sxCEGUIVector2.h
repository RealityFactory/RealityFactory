/************************************************************************************//**
 * @file sxCEGUIVector2.h
 * @brief Class declaration for RFSX Vector2/Point object
 * @author: Daniel Queteschiner
 * @date November 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_VECTOR2_H_
#define _SX_VECTOR2_H_

#include "RFSX.h"

// sxVector2 class definition
/*
	Description:
		Wrapper for CEGUI::Vector2 / CEGUI::Point class

	Methods:
		sxSize asSize();
		void add(sxVector2 other);
		void subtract(sxVector2 other);

	Operators:
		equals

	Fields:
		float x;(RW)
		float y;(RW)
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxVector2
#define SXVECTOR2_METHODS 3

// Type-check and upcast macro
#define IS_VECTOR2(o)	CheckType<RFSX::sxVector2>(o, RFSXU_VECTOR2)
#define IS_POINT(o)		CheckType<RFSX::sxPoint>(o, RFSXU_POINT)

namespace RFSX
{

class SX_IMPL_TYPE: public skExecutable
{
public:
	SX_IMPL_TYPE();
	SX_IMPL_TYPE(const CEGUI::Vector2& val);
	SX_IMPL_TYPE(float x, float y);
	virtual ~SX_IMPL_TYPE();
	static void InitMHT();

	virtual bool setValue(const skString& name, const skString& att, const skRValue& val);
	virtual bool getValue(const skString& name, const skString& att, skRValue& val);
	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);
	virtual bool equals(const skiExecutable *other_object) const;
	virtual int executableType() const { return RFSXU_VECTOR2; }

	CEGUI::Point* Point() { return &m_Vector2; }
	const CEGUI::Point& PointConstRef() const { return m_Vector2; }

	CEGUI::Vector2* Vector2() { return &m_Vector2; }
	const CEGUI::Vector2& Vector2ConstRef() const { return m_Vector2; }

protected:
	SX_METHOD_DECL(add);
	SX_METHOD_DECL(subtract);
	SX_METHOD_DECL(asSize);

private:
	CEGUI::Vector2 m_Vector2;

	static sxMHT<SX_IMPL_TYPE> h_method;
};

typedef sxVector2	sxPoint;

} // namespace RFSX


#undef SX_IMPL_TYPE

#endif // _SX_VECTOR2_H_
