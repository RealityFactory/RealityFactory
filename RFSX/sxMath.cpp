/************************************************************************************//**
 * @file sxMath.cpp
 * @brief
 * @author D.R. Gunter
 *//*
 * Copyright (c) 2008 D.R. Gunter; All rights reserved.
 ****************************************************************************************/

#include "sxMath.h"
#include <math.h>

#define M_180_OVER_PI			(57.2957795130823208768f)
#define M_PI_OVER_180			(0.01745329251994329577f)

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxMath

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxMath class implementation
//////////////////////////////////////////////////////////////////////////////////////////

SX_IMPL_TYPE::SX_IMPL_TYPE()
{
}


SX_IMPL_TYPE::~SX_IMPL_TYPE()
{
}


bool SX_IMPL_TYPE::method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
{
	if(h_method(this, m_name, args, r_val, ctxt))
		return true;
	else
		return skExecutable::method(m_name, args, r_val, ctxt);
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxMath method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(abs)
{
	if(args.entries() != 1)
		return false;

	r_val = fabs(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(acos)
{
	if(args.entries() != 1)
		return false;

	r_val = acos(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(asin)
{
	if(args.entries() != 1)
		return false;

	r_val = asin(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(atan)
{
	if(args.entries() != 1)
		return false;

	r_val = atan(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(atan2)
{
	if(args.entries() != 2)
		return false;

	r_val = atan2(args[0].floatValue(), args[1].floatValue());
	return true;
}


SX_METHOD_IMPL(ceil)
{
	if(args.entries() != 1)
		return false;

	r_val = ceil(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(cos)
{
	if(args.entries() != 1)
		return false;

	r_val = cos(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(cosh)
{
	if(args.entries() != 1)
		return false;

	r_val = cosh(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(exp)
{
	if(args.entries() != 1)
		return false;

	r_val = exp(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(floor)
{
	if(args.entries() != 1)
		return false;

	r_val = floor(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(fmod)
{
	if(args.entries() != 2)
		return false;

	r_val = fmod(args[0].floatValue(), args[1].floatValue());
	return true;
}


SX_METHOD_IMPL(log)
{
	if((args.entries() != 1) || (args[0].floatValue() <= 0))
		return false;

	r_val = log(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(sin)
{
	if(args.entries() != 1)
		return false;

	r_val = sin(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(sinh)
{
	if(args.entries() != 1)
		return false;

	r_val = sinh(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(sqrt)
{
	if(args.entries() != 1)
		return false;

	r_val = sqrt(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(tan)
{
	if(args.entries() != 1)
		return false;

	r_val = tan(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(tanh)
{
	if(args.entries() != 1)
		return false;

	r_val = tanh(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(rand)
{
	if(args.entries() == 0)
	{
		r_val = rand();
		return true;
	}
	else if(args.entries() == 2)
	{
		if(	args[0].type() == skRValue::T_Float ||
			args[1].type() == skRValue::T_Float)
		{
			float lower = args[0].floatValue();
			float upper = args[1].floatValue();

			if(lower == upper)
			{
				r_val = lower;
			}
			else
			{
				if(upper < lower)
				{
					upper = lower;
					lower = args[1].floatValue();
				}

				// pick a random float from whithin the range
				float range = upper - lower;
				r_val = range * static_cast<float>(rand()) / static_cast<float>(RAND_MAX) + lower;
			}
		}
		else
		{
			int lower = args[0].intValue();
			int upper = args[1].intValue();

			if(lower == upper)
			{
				r_val = lower;
			}
			else
			{
				if(upper < lower)
				{
					upper = lower;
					lower = args[1].intValue();
				}

				// pick a random int from whithin the range

				// do not use (uses lower-order bits!):
				// rand() % (upper - lower + 1) + lower;
				// instead use:
				float range = static_cast<float>(upper - lower + 1);
				r_val = static_cast<int>(range * static_cast<float>(rand()) / (static_cast<float>(RAND_MAX) + 1.0f)) + lower;
			}
		}

		return true;
	}

	return false;
}


SX_METHOD_IMPL(deg2rad)
{
	if(args.entries() != 1)
		return false;

	r_val = args[0].floatValue() * M_PI_OVER_180;
	return true;
}


SX_METHOD_IMPL(rad2deg)
{
	if(args.entries() != 1)
		return false;

	r_val = args[0].floatValue() * M_180_OVER_PI;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxMath intialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXMATH_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	// hash the methods
	SX_ADD_METHOD(abs);
	SX_ADD_METHOD(acos);
	SX_ADD_METHOD(asin);
	SX_ADD_METHOD(atan);
	SX_ADD_METHOD(atan2);
	SX_ADD_METHOD(ceil);
	SX_ADD_METHOD(cos);
	SX_ADD_METHOD(cosh);
	SX_ADD_METHOD(exp);
	SX_ADD_METHOD(floor);
	SX_ADD_METHOD(fmod);
	SX_ADD_METHOD(log);
	SX_ADD_METHOD(sin);
	SX_ADD_METHOD(sinh);
	SX_ADD_METHOD(sqrt);
	SX_ADD_METHOD(tan);
	SX_ADD_METHOD(tanh);
	SX_ADD_METHOD(rand);
	SX_ADD_METHOD(deg2rad);
	SX_ADD_METHOD(rad2deg);
}

} // namespace RFSX
