/************************************************************************************//**
 * @file sxCEGUIUVector2.h
 * @brief Class declaration for RFSX UVector2 object
 * @author Daniel Queteschiner
 * @date November 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_UVECTOR2_H_
#define _SX_UVECTOR2_H_

#include "RFSX.h"

// sxUVector2 class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		sxVector2 asAbsolute(sxSize base);
		sxVector2 asRelative(sxSize base);
		void add(sxUVector2 other);
		void subtract(sxUVector2 other);
		void multiply(sxUVector2 other);
		void divide(sxUVector2 other);

	Operators:
		equals

	Fields:
		sxUDim x;(RW)
		sxUDim y;(RW)
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxUVector2
#define SXUVECTOR2_METHODS 6

// Type-check and upcast macro
#define IS_UVECTOR2(o)	 CheckType<sxUVector2>(o, RFSXU_UVECTOR2)

namespace RFSX
{

class SX_IMPL_TYPE: public skExecutable
{
public:
	SX_IMPL_TYPE();
	SX_IMPL_TYPE(const CEGUI::UVector2& val);
	SX_IMPL_TYPE(const CEGUI::UDim& x, const CEGUI::UDim& y);
	virtual ~SX_IMPL_TYPE();
	static void InitMHT();

	virtual bool setValue(const skString& name, const skString& att, const skRValue& val);
	virtual bool getValue(const skString& name, const skString& att, skRValue& val);
	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);
	virtual bool equals(const skiExecutable *other_object) const;
	virtual int executableType() const { return RFSXU_UVECTOR2; }

	CEGUI::UVector2* UVector2() { return &m_UVector2; }
	const CEGUI::UVector2& UVector2ConstRef() const { return m_UVector2; }

protected:
	// Add methods here, by name.
	SX_METHOD_DECL(asAbsolute);
	SX_METHOD_DECL(asRelative);
	SX_METHOD_DECL(add);
	SX_METHOD_DECL(subtract);
	SX_METHOD_DECL(multiply);
	SX_METHOD_DECL(divide);

private:
	CEGUI::UVector2 m_UVector2;

	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_UVECTOR2_H_
