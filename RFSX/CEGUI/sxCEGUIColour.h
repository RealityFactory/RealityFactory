/************************************************************************************//**
 * @file sxCEGUIColour.h
 * @brief Class declaration for RFSX colour object.
 * @author Daniel Queteschiner
 * @date November 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_COLOUR_H_
#define _SX_COLOUR_H_

#include "RFSX.h"

// sxColour class definition
/*
	Description:
		Wrapper for CEGUI::colour class

	Methods:
		float getAlpha();
		float getRed();
		float getGreen();
		float getBlue();
		float getHue();
		float getSaturation();
		float getLumination();
		void setAlpha(float a);
		void setRed(float r);
		void setGreen(float g);
		void setBlue(float b);
		void set(float r, float g, float b, float a);
		void setRGB(float r, float g, float b);
		void setHSL(float hue, float saturation, float luminance, float alpha = 1.0f);
		void invertColour();
		void invertColourWithAlpha();

	Fields:
	// type_codes: RO=Read-only WO=Write-only RW=Read/Write
		float a(RW);
		float r(RW);
		float g(RW);
		float b(RW);
		float h(RO);
		float s(RO);
		float l(RO);
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxColour
#define SXCOLOUR_METHODS 16

// Type-check and upcast macro
#define IS_COLOUR(o) CheckType<sxColour>(o, RFSXU_COLOUR);

namespace RFSX
{

class SX_IMPL_TYPE : public skExecutable
{
public:
	SX_IMPL_TYPE(void);
	SX_IMPL_TYPE(const CEGUI::colour& val);
	SX_IMPL_TYPE(float red, float green, float blue, float alpha = 1.0f);
	virtual ~SX_IMPL_TYPE();
	static void InitMHT();

	virtual bool setValue(const skString& name, const skString& att, const skRValue& val);
	virtual bool getValue(const skString& name, const skString& att, skRValue& val);
	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);
	virtual bool equals(const skiExecutable *other_object) const;
	virtual int executableType() const { return RFSXU_COLOUR; }

	CEGUI::colour* Colour() { return &m_Colour; }
	const CEGUI::colour& ColourConstRef() const { return m_Colour; }

protected:
	//SX_METHOD_DECL(getARGB);
	SX_METHOD_DECL(getAlpha);
	SX_METHOD_DECL(getRed);
	SX_METHOD_DECL(getGreen);
	SX_METHOD_DECL(getBlue);
	SX_METHOD_DECL(getHue);
	SX_METHOD_DECL(getSaturation);
	SX_METHOD_DECL(getLumination);
	//SX_METHOD_DECL(setARGB);
	SX_METHOD_DECL(setAlpha);
	SX_METHOD_DECL(setRed);
	SX_METHOD_DECL(setGreen);
	SX_METHOD_DECL(setBlue);
	SX_METHOD_DECL(set);
	SX_METHOD_DECL(setRGB);
	SX_METHOD_DECL(setHSL);
	SX_METHOD_DECL(invertColour);
	SX_METHOD_DECL(invertColourWithAlpha);
	//SX_METHOD_DECL(add);
	//SX_METHOD_DECL(substract);
	//SX_METHOD_DECL(multiply);

private:
	CEGUI::colour m_Colour;

	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _RFSX_COLOUR_H_
