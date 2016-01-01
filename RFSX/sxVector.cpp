/************************************************************************************//**
 * @file sxVector.cpp
 * @brief Implementation for RFSX Vector object.
 * @author D.R. Gunter
 *//*
 * Copyright (c) 2008 D.R. Gunter; All rights reserved.
 ****************************************************************************************/

#include "sxVector.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxVector

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxVector implementation
//////////////////////////////////////////////////////////////////////////////////////////

SX_IMPL_TYPE::SX_IMPL_TYPE(float x, float y, float z)
{
	m_Vector.X = x;
	m_Vector.Y = y;
	m_Vector.Z = z;
}


SX_IMPL_TYPE::SX_IMPL_TYPE(const geVec3d* pvec)
{
	m_Vector = *pvec;
}


SX_IMPL_TYPE::SX_IMPL_TYPE(const geVec3d& vec)
{
	m_Vector = vec;
}


SX_IMPL_TYPE::~SX_IMPL_TYPE()
{
}


bool SX_IMPL_TYPE::setValue(const skString& name ,const skString& att, const skRValue& val)
{
	if(name == "x")
	{
		Vector()->X = val.floatValue();
		return true;
	}
	if(name == "y")
	{
		Vector()->Y = val.floatValue();
		return true;
	}
	if(name == "z")
	{
		Vector()->Z = val.floatValue();
		return true;
	}

	return false;
}


bool SX_IMPL_TYPE::getValue(const skString& name ,const skString& att, skRValue& val)
{
	if(name == "x")
	{
		val = this->Vector()->X;
		return true;
	}
	if(name == "y")
	{
		val = this->Vector()->Y;
		return true;
	}
	if(name == "z")
	{
		val = this->Vector()->Z;
		return true;
	}
	if(name == "length")
	{
		val = geVec3d_Length(Vector());
		return true;
	}

	return skExecutable::getValue(name, att, val);
}


bool SX_IMPL_TYPE::method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
{
	if(h_method(this, m_name, args, r_val, ctxt))
		return true;
	else
		return skExecutable::method(m_name, args, r_val, ctxt);
}


//////////////////////////////////////////////////////////////////////////////////////////
// sxVector method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(Set)
{
	geVec3d_Set(caller->Vector(), args[0].floatValue(), args[1].floatValue(), args[3].floatValue());
	return true;
}


SX_METHOD_IMPL(Dot)
{
	sxVector *other = IS_VECTOR(args[0]);
	if(!other)
		return false;

	r_val = geVec3d_DotProduct(caller->Vector(), other->Vector());
	return true;
}


SX_METHOD_IMPL(Cross)
{
	sxVector *other = IS_VECTOR(args[0]);
	if(!other)
		return false;

	geVec3d rVec;
	geVec3d_CrossProduct(caller->Vector(), other->Vector(), &rVec);

	r_val.assignObject(new sxVector(&rVec), true);
	return true;
}


SX_METHOD_IMPL(Normalize)
{
	geVec3d_Normalize(caller->Vector());
	return true;
}


SX_METHOD_IMPL(IsNormalized)
{
	r_val = geVec3d_IsNormalized(caller->Vector()) ? true : false;
	return true;
}


SX_METHOD_IMPL(Scale)
{
	geVec3d_Scale(caller->Vector(), args[0].floatValue(), caller->Vector());
	return true;
}


SX_METHOD_IMPL(Subtract)
{
	sxVector *other = IS_VECTOR(args[0]);
	if(!other)
		return false;

	geVec3d_Subtract(caller->Vector(), other->Vector(), caller->Vector());
	return true;
}


SX_METHOD_IMPL(Add)
{
	sxVector *other = IS_VECTOR(args[0]);
	if(!other)
		return false;

	geVec3d_Add(caller->Vector(), other->Vector(), caller->Vector());
	return true;
}


SX_METHOD_IMPL(Inverse)
{
	geVec3d_Inverse(caller->Vector());
	return true;
}


SX_METHOD_IMPL(DistanceTo)
{
	sxVector *other = IS_VECTOR(args[0]);
	if(!other)
		return false;

	r_val = geVec3d_DistanceBetween(caller->Vector(), other->Vector());
	return true;
}


SX_METHOD_IMPL(Length)
{
	r_val = geVec3d_Length(caller->Vector());
	return true;
}


SX_METHOD_IMPL(LengthSquared)
{
	r_val = geVec3d_LengthSquared(caller->Vector());
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxVector initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXVECTOR_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(Set);
	SX_ADD_METHOD(Dot);
	SX_ADD_METHOD(Cross);
	SX_ADD_METHOD(Normalize);
	SX_ADD_METHOD(IsNormalized);
	SX_ADD_METHOD(Scale);
	SX_ADD_METHOD(Subtract);
	SX_ADD_METHOD(Add);
	SX_ADD_METHOD(Inverse);
	SX_ADD_METHOD(DistanceTo);
	SX_ADD_METHOD(Length);
	SX_ADD_METHOD(LengthSquared);
}

} // namespace RFSX

#undef SX_IMPL_TYPE
