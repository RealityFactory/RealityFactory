/************************************************************************************//**
 * @file sxCEGUIColour.cpp
 * @brief Implementation for RFSX color object.
 * @author Daniel Queteschiner
 * @date November 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUIColour.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxColour

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxColour implementation
//////////////////////////////////////////////////////////////////////////////////////////
SX_IMPL_TYPE::SX_IMPL_TYPE(void)
{
}

SX_IMPL_TYPE::SX_IMPL_TYPE(const CEGUI::colour& val)
	: m_Colour(val)
{
}

SX_IMPL_TYPE::SX_IMPL_TYPE(float red, float green, float blue, float alpha)
	: m_Colour(red, green, blue, alpha)
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


bool SX_IMPL_TYPE::equals(const skiExecutable *other_object) const
{
	if(other_object->executableType() != RFSXU_COLOUR)
		return false;

	const sxColour *other = static_cast<const sxColour*>(other_object);

	return (m_Colour == other->ColourConstRef());
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxColour fields
//////////////////////////////////////////////////////////////////////////////////////////

bool SX_IMPL_TYPE::setValue(const skString& name, const skString& att, const skRValue& val)
{
	if(name == "a" || name == "Alpha")
	{
		m_Colour.setAlpha(val.floatValue());
		return true;
	}
	if(name == "r" || name == "Red")
	{
		m_Colour.setRed(val.floatValue());
		return true;
	}
	if(name == "g" || name == "Green")
	{
		m_Colour.setGreen(val.floatValue());
		return true;
	}
	if(name == "b" || name == "Blue")
	{
		m_Colour.setBlue(val.floatValue());
		return true;
	}

	return false;
}


bool SX_IMPL_TYPE::getValue(const skString& name, const skString& att, skRValue& val)
{
	if(name == "a" || name == "Alpha")
	{
		val = m_Colour.getAlpha();
		return true;
	}
	if(name == "r" || name == "Red")
	{
		val = m_Colour.getRed();
		return true;
	}
	if(name == "g" || name == "Green")
	{
		val = m_Colour.getGreen();
		return true;
	}
	if(name == "b" || name == "Blue")
	{
		val = m_Colour.getBlue();
		return true;
	}
	if(name == "h" || name == "Hue")
	{
		val = m_Colour.getHue();
		return true;
	}
	if(name == "s" || name == "Saturation")
	{
		val = m_Colour.getSaturation();
		return true;
	}
	if(name == "l" || name == "Lumination")
	{
		val = m_Colour.getLumination();
		return true;
	}

	return skExecutable::getValue(name, att, val);
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxColour method functions
//////////////////////////////////////////////////////////////////////////////////////////

/*SX_METHOD_IMPL(getARGB)
{
	r_val = static_cast<int>(caller->Colour()->getARGB());
	return true;
}*/


SX_METHOD_IMPL(getAlpha)
{
	r_val = caller->Colour()->getAlpha();
	return true;
}


SX_METHOD_IMPL(getRed)
{
	r_val = caller->Colour()->getRed();
	return true;
}


SX_METHOD_IMPL(getGreen)
{
	r_val = caller->Colour()->getGreen();
	return true;
}


SX_METHOD_IMPL(getBlue)
{
	r_val = caller->Colour()->getBlue();
	return true;
}


SX_METHOD_IMPL(getHue)
{
	r_val = caller->Colour()->getHue();
	return true;
}


SX_METHOD_IMPL(getSaturation)
{
	r_val = caller->Colour()->getSaturation();
	return true;
}


SX_METHOD_IMPL(getLumination)
{
	r_val = caller->Colour()->getLumination();
	return true;
}


/*SX_METHOD_IMPL(setARGB)
{
	if(args.entries() != 1)
		return false;

	caller->Colour()->setARGB(args[0].intValue());
	return true;
}*/


SX_METHOD_IMPL(setAlpha)
{
	if(args.entries() != 1)
		return false;

	caller->Colour()->setAlpha(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(setRed)
{
	if(args.entries() != 1)
		return false;

	caller->Colour()->setRed(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(setGreen)
{
	if(args.entries() != 1)
		return false;

	caller->Colour()->setGreen(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(setBlue)
{
	if(args.entries() != 1)
		return false;

	caller->Colour()->setBlue(args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(set)
{
	skRValue::RType rtypes[] = {	skRValue::T_Float, skRValue::T_Float,
									skRValue::T_Float, skRValue::T_Float };

	if(!CheckParams(3, 4, args, rtypes))
		return false;

	if(args.entries() == 3)
		caller->Colour()->set(args[0].floatValue(), args[1].floatValue(), args[2].floatValue());
	else
		caller->Colour()->set(args[0].floatValue(), args[1].floatValue(), args[2].floatValue(), args[3].floatValue());

	return true;
}


SX_METHOD_IMPL(setRGB)
{
	if(args.entries() == 1)
	{
		sxColour *other = IS_COLOUR(args[0]);
		if(!other)
			return false;

		caller->Colour()->setRGB(*(other->Colour()));
	}
	else if(args.entries() == 3)
	{
		caller->Colour()->setRGB(args[0].floatValue(), args[1].floatValue(), args[2].floatValue());
	}
	else
		return false;

	return true;
}


SX_METHOD_IMPL(setHSL)
{
	skRValue::RType rtypes[] = {	skRValue::T_Float, skRValue::T_Float,
									skRValue::T_Float, skRValue::T_Float };

	if(!CheckParams(3, 4, args, rtypes))
		return false;

	if(args.entries() == 3)
		caller->Colour()->setHSL(args[0].floatValue(), args[1].floatValue(), args[2].floatValue());
	else
		caller->Colour()->setHSL(args[0].floatValue(), args[1].floatValue(), args[2].floatValue(), args[3].floatValue());

	return true;
}


SX_METHOD_IMPL(invertColour)
{
	caller->Colour()->invertColour();
	return true;
}

SX_METHOD_IMPL(invertColourWithAlpha)
{
	caller->Colour()->invertColourWithAlpha();
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxColour initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXCOLOUR_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	//SX_ADD_METHOD(getARGB);
	SX_ADD_METHOD(getAlpha);
	SX_ADD_METHOD(getRed);
	SX_ADD_METHOD(getGreen);
	SX_ADD_METHOD(getBlue);
	SX_ADD_METHOD(getHue);
	SX_ADD_METHOD(getSaturation);
	SX_ADD_METHOD(getLumination);
	//SX_ADD_METHOD(setARGB);
	SX_ADD_METHOD(setAlpha);
	SX_ADD_METHOD(setRed);
	SX_ADD_METHOD(setGreen);
	SX_ADD_METHOD(setBlue);
	SX_ADD_METHOD(set);
	SX_ADD_METHOD(setRGB);
	SX_ADD_METHOD(setHSL);
	SX_ADD_METHOD(invertColour);
	SX_ADD_METHOD(invertColourWithAlpha);
	//SX_ADD_METHOD(add);
	//SX_ADD_METHOD(substract);
	//SX_ADD_METHOD(multiply);
}

} // namespace RFSX

#undef SX_IMPL_TYPE
