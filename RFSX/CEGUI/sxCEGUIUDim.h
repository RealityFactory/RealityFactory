/************************************************************************************//**
 * @file sxCEGUIUDim.h
 * @brief Class declaration for RFSX UDim object
 * @author Daniel Queteschiner
 * @date November 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_UDIM_H_
#define _SX_UDIM_H_

#include "RFSX.h"

// sxUDim class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		float asAbsolute(float base);
		float asRelative(float base);

		void add(sxUDim other);
		void subtract(sxUDim other);
		void multiply(sxUDim other);
		void divide(sxUDim other);

	Operators:
		equals

	Fields:
		float scale(RW);
		float offset(RW);
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxUDim
#define SXUDIM_METHODS 6

// Type-check and upcast macro
#define IS_UDIM(o)	CheckType<sxUDim>(o, RFSXU_UDIM)

namespace RFSX
{

class SX_IMPL_TYPE: public skExecutable
{
public:
	SX_IMPL_TYPE();
	SX_IMPL_TYPE(const CEGUI::UDim& val);
	SX_IMPL_TYPE(float scale, float offset);
	virtual ~SX_IMPL_TYPE();
	static void InitMHT();

	virtual bool setValue(const skString& name, const skString& att, const skRValue& val);
	virtual bool getValue(const skString& name, const skString& att, skRValue& val);
	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);
	virtual bool equals(const skiExecutable *other_object) const;
	virtual int executableType() const { return RFSXU_UDIM; }

	CEGUI::UDim* UDim() { return &m_UDim; }
	const CEGUI::UDim& UDimConstRef() const { return m_UDim; }

protected:
	SX_METHOD_DECL(asAbsolute);
	SX_METHOD_DECL(asRelative);
	SX_METHOD_DECL(add);
	SX_METHOD_DECL(subtract);
	SX_METHOD_DECL(multiply);
	SX_METHOD_DECL(divide);

private:
	CEGUI::UDim m_UDim;

	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_UDIM_H_
