/************************************************************************************//**
 * @file sxVector.h
 * @brief Class declaration for RFSX vector object.
 * @author D.R. Gunter
 *//*
 * Copyright (c) 2008 D.R. Gunter; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_VECTOR_H_
#define _SX_VECTOR_H_

#include "RFSX.h"
#include <vec3D.h>

// sxVector class definition
/*
	This class provides a wrapper for a geVec3D struct and provides
	an interface to Genesis3D vector manipulation functions.

	Fields(R/W):
		float x -- vector x-axis value
		float y -- vector y-axis value
		float z -- vector z-axis value

	Fields(RO):
		float mag -- current "calculated" vector magnitude
		float length -- alias for m

	Methods:
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxVector
#define SXVECTOR_METHODS 12

// Type-check and upcast macro
#define IS_VECTOR(o) CheckType<RFSX::sxVector>(o, RFSXU_VECTOR)

namespace RFSX
{

class SX_IMPL_TYPE : public skExecutable
{
public:
	SX_IMPL_TYPE(float x, float y, float z);
	SX_IMPL_TYPE(const geVec3d& vec);
	SX_IMPL_TYPE(const geVec3d* vec);
	virtual ~SX_IMPL_TYPE();
	static void InitMHT();

	virtual bool setValue(const skString& name ,const skString& att, const skRValue& val);
	virtual bool getValue(const skString& name ,const skString& att, skRValue& val);
	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);
	virtual int executableType() const { return RFSXU_VECTOR; }

	geVec3d* Vector()					{ return &m_Vector; }
	geVec3d* Vector3()					{ return &m_Vector; }
	const geVec3d& VectorConstRef()		{ return m_Vector; }
	const geVec3d& Vector3ConstRef()	{ return m_Vector; }

protected:
	SX_METHOD_DECL(Set);
	SX_METHOD_DECL(Dot);
	SX_METHOD_DECL(Cross);
	SX_METHOD_DECL(Normalize);
	SX_METHOD_DECL(IsNormalized);
	SX_METHOD_DECL(Scale);
	SX_METHOD_DECL(Add);
	SX_METHOD_DECL(Subtract);
	SX_METHOD_DECL(Inverse);
	SX_METHOD_DECL(DistanceTo);
	SX_METHOD_DECL(Length);
	SX_METHOD_DECL(LengthSquared);

private:
	geVec3d m_Vector;

	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _RFSX_VECTOR_H_
