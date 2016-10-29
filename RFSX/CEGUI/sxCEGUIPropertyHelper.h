/************************************************************************************//**
 * @file sxCEGUIPropertyHelper.h
 * @brief Class declaration for RFSX PropertyHelper object
 * @author Daniel Queteschiner
 * @date December 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_PROPERTYHELPER_H_
#define _SX_PROPERTYHELPER_H_

#include "RFSX.h"

// sxPropertyHelper class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		float			stringToFloat(skString str);
		bool			stringToBool(skString str);
		sxSize			stringToSize(skString str);
		sxVector2		stringToPoint(skString str);
		sxRect			stringToRect(skString str);
		sxImage			stringToImage(skString str);	-- may return null!
		sxColour		stringToColour(skString str);
		sxColourRect	stringToColourRect(skString str);
		sxUDim			stringToUDim(skString str);
		sxUVector2		stringToUVector2(skString str);
		sxURect			stringToURect(skString str);

		skString		floatToString(float val);
		skString		boolToString(bool val);
		skString		sizeToString(sxSize val);
		skString		pointToString(sxVector2 val);
		skString		rectToString(sxRect val);
		skString		imageToString(sxImage val);
		skString		colourToString(sxColour val);
		skString		colourRectToString(sxColourRect val);
		skString		udimToString(sxUDim val);
		skString		uvector2ToString(sxUVector2 val);
		skString		urectToString(sxURect val);

*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxPropertyHelper

#define SXPROPERTYHELPER_METHODS 22

namespace RFSX
{

class SX_IMPL_TYPE: public skExecutable
{
public:
	SX_IMPL_TYPE();
	virtual ~SX_IMPL_TYPE();
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);
	virtual int executableType() const { return RFSXU_PROPERTYHELPER; }

protected:
	SX_METHOD_DECL(stringToFloat);
	SX_METHOD_DECL(stringToBool);
	SX_METHOD_DECL(stringToSize);
	SX_METHOD_DECL(stringToPoint);
	SX_METHOD_DECL(stringToRect);
	SX_METHOD_DECL(stringToImage);
	SX_METHOD_DECL(stringToColour);
	SX_METHOD_DECL(stringToColourRect);
	SX_METHOD_DECL(stringToUDim);
	SX_METHOD_DECL(stringToUVector2);
	SX_METHOD_DECL(stringToURect);

	SX_METHOD_DECL(floatToString);
	SX_METHOD_DECL(boolToString);
	SX_METHOD_DECL(sizeToString);
	SX_METHOD_DECL(pointToString);
	SX_METHOD_DECL(rectToString);
	SX_METHOD_DECL(imageToString);
	SX_METHOD_DECL(colourToString);
	SX_METHOD_DECL(colourRectToString);
	SX_METHOD_DECL(udimToString);
	SX_METHOD_DECL(uvector2ToString);
	SX_METHOD_DECL(urectToString);

private:
	static sxMHT<SX_IMPL_TYPE> h_method;

};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_PROPERTYHELPER_H_
