/************************************************************************************//**
 * @file sxNew.h
 * @brief Class declaration for RFSX object generator.
 * @author D.R. Gunter
 *//*
 * Copyright (c) 2008 D.R. Gunter; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_NEW_H_
#define _SX_NEW_H_

#include "RFSX.h"

// sxNew class definition
/*
	This class provides methods to create typed objects in a script at runtime.

	Think of it as a c++ "new" command for simkin scripts.
	The objects returned will be properly destroyed when the scripted executable is
	destroyed, or when they go out of scope in the script.

	Methods:
		sxVector		Vector(float x, float y, float z);

		// CEGUI
		sxVector2		Vector2(float x, float y);
		sxPoint			Point(float x, float y);
		sxSize			Size(float w, float h);
		sxRect			Rect(float l, float t, float r, float b);
		sxColour		Colour();
		sxColour		Colour(float r, float g, float b, float a = 1.0f);
		sxColourRect	ColourRect();
		sxColourRect	ColourRect(sxColour col);
		sxColourRect	ColourRect(sxColour top_left, sxColour top_right, sxColour bottom_left, sxColour bottom_right);
		sxUDim			UDim(float scale, float offset);
		sxUVector2		UVector2(sxUDim scale, sxUDim offset);
		sxURect			URect(sxUVector2 min, sxUVector2 max);
		sxURect			URect(sxUDim left, sxUDim top, sxUDim right, sxUDim bottom);

		sxListboxTextItem	ListboxTextItem(skString text, int item_id = 0, bool disabled = false);
		sxMCLGridRef		MCLGridRef(int row, int column);

	Example of possible application:
		{
			VEC = New.Vector(x, y, z);
			SetPositon(VEC);
		}
		or, simply:
		{
			SetPosition(New.Vector(x, y, z));
		}
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxNew

#define SXNEW_METHODS 13

namespace RFSX
{

class SX_IMPL_TYPE: public skExecutable
{
public:
	SX_IMPL_TYPE();
	virtual ~SX_IMPL_TYPE();
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);
	virtual int executableType() const { return RFSXG_NEW; }

protected:
	SX_METHOD_DECL(Camera);
	SX_METHOD_DECL(Vector);

	SX_METHOD_DECL(Vector2);
	SX_METHOD_DECL(Point);
	SX_METHOD_DECL(Size);
	SX_METHOD_DECL(Rect);
	SX_METHOD_DECL(Colour);
	SX_METHOD_DECL(ColourRect);
	SX_METHOD_DECL(UDim);
	SX_METHOD_DECL(UVector2);
	SX_METHOD_DECL(URect);

	SX_METHOD_DECL(ListboxTextItem);
	SX_METHOD_DECL(MCLGridRef);

private:
	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _RFSXNEW_H_
