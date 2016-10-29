/************************************************************************************//**
 * @file sxCEGUIColourRect.h
 * @brief Class declaration for RFSX ColourRect object
 * @author Daniel Queteschiner
 * @date November 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_COLOURRECT_H_
#define _SX_COLOURRECT_H_

#include "RFSX.h"

// sxColourRect class definition
/*
	Description:
		Wrapper for CEGUI::ColourRect class which represents colours for four
		corners of a rectangle

	Methods:
		void setAlpha(float alpha);
		void setTopAlpha(float alpha);
		void setBottomAlpha(float alpha);
		void setLeftAlpha(float alpha);
		void setRightAlpha(float alpha);
		bool isMonochromatic();
		sxColourRect getSubRectangle(float left, float right, float top, float bottom);
		sxColour getColourAtPoint(float x, float y);
		void setColours(sxColour col);
		void modulateAlpha(float alpha);
		void modulateColours(sxColourRect col);

	Fields:
	// type_codes: RO=Read-only WO=Write-only RW=Read/Write
		sxColour top_left(RW);
		sxColour top_right(RW);
		sxColour bottom_left(RW);
		sxColour bottom_right(RW);
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxColourRect
#define SXCOLOURRECT_METHODS 11

// Type-check and upcast macro
#define IS_COLOURRECT(o) CheckType<sxColourRect>(o, RFSXU_COLOURRECT);

namespace RFSX
{

class SX_IMPL_TYPE: public skExecutable
{
public:
	SX_IMPL_TYPE();
	SX_IMPL_TYPE(const CEGUI::ColourRect& val);
	SX_IMPL_TYPE(const CEGUI::colour& col);
	SX_IMPL_TYPE(const CEGUI::colour& top_left, const CEGUI::colour& top_right,
				 const CEGUI::colour& bottom_left, const CEGUI::colour& bottom_right);
	virtual ~SX_IMPL_TYPE();
	static void InitMHT();

	virtual bool setValue(const skString& name, const skString& att, const skRValue& val);
	virtual bool getValue(const skString& name, const skString& att, skRValue& val);
	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);
	virtual int executableType() const { return RFSXU_COLOURRECT; }

	CEGUI::ColourRect* ColourRect() { return &m_ColourRect; }
	const CEGUI::ColourRect& ColourRectConstRef() { return m_ColourRect; }

protected:
	// Add methods here, by name.
	SX_METHOD_DECL(setAlpha);
	SX_METHOD_DECL(setTopAlpha);
	SX_METHOD_DECL(setBottomAlpha);
	SX_METHOD_DECL(setLeftAlpha);
	SX_METHOD_DECL(setRightAlpha);
	SX_METHOD_DECL(isMonochromatic);
	SX_METHOD_DECL(getSubRectangle);
	SX_METHOD_DECL(getColourAtPoint);
	SX_METHOD_DECL(setColours);
	SX_METHOD_DECL(modulateAlpha);
	SX_METHOD_DECL(modulateColours);

private:
	CEGUI::ColourRect m_ColourRect;

	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_COLOURRECT_H_
