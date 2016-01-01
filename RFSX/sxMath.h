/************************************************************************************//**
 * @file sxMath.h
 * @brief Class declaration for RFSX Math library.
 * @author D.R. Gunter
 *//*
 * Copyright (c) 2008 D.R. Gunter; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_MATH_H_
#define _SX_MATH_H_

#include "RFSX.h"

// sxMath class definition
/*
	This class provides access to various standard math functions.
	Should be self-explanatory, as it's just an indirection to the
	standard C math functions.
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxMath

#define SXMATH_METHODS 20

namespace RFSX
{

class SX_IMPL_TYPE: public skExecutable
{
public:
	SX_IMPL_TYPE();
	virtual ~SX_IMPL_TYPE();
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);
	virtual int executableType() const { return RFSXG_MATH; }

protected:
	SX_METHOD_DECL(abs);
	SX_METHOD_DECL(acos);
	SX_METHOD_DECL(asin);
	SX_METHOD_DECL(atan);
	SX_METHOD_DECL(atan2);
	SX_METHOD_DECL(ceil);
	SX_METHOD_DECL(cos);
	SX_METHOD_DECL(cosh);
	SX_METHOD_DECL(exp);
	SX_METHOD_DECL(floor);
	SX_METHOD_DECL(fmod);
	SX_METHOD_DECL(log);
	SX_METHOD_DECL(rand);
	SX_METHOD_DECL(sin);
	SX_METHOD_DECL(sinh);
	SX_METHOD_DECL(sqrt);
	SX_METHOD_DECL(tan);
	SX_METHOD_DECL(tanh);
	SX_METHOD_DECL(deg2rad);
	SX_METHOD_DECL(rad2deg);

private:
	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _RFSXMATH_H_
