/************************************************************************************//**
 * @file sxCEGUIFont.h
 * @brief Class declaration for RFSX Font object
 * @author Daniel Queteschiner
 * @date November 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_FONT_H_
#define _SX_FONT_H_

#include "RFSX.h"

// sxFont class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		//////////////////////////////////////////////
		// PropertySet
		void setProperty(skString name, skString value);
		skString getProperty(skString name);
		skString getPropertyDefault(skString name);
		skString getPropertyHelp(skString name);

		bool isPropertyPresent(skString name);
		bool isPropertyDefault(skString name);

		void setDefaultResourceGroup(skString resourceGroup);
		skString getDefaultResourceGroup();

		void load();

		bool isCodepointAvailable(int cp);

		void setNativeResolution(sxSize sz);
		void notifyScreenResolution(sxSize sz);

		float getTextExtent(skString text, float xscale=1);
		float getLineSpacing(float yscale=1);
		float getFontHeight(float yscale=1);

		int getCharAtPixel(skString text, int start_char, float pixel, float xscale=1);
		int getCharAtPixel(skString text, float pixel, float xscale=1);

		int getFormattedLineCount(skString text, sxRect format_area, int fmt, float xscale=1);
		float getFormattedTextExtent(skString text, sxRect format_area, int fmt, float xscale=1);

	for each: PropertyIterator
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxFont
#define SXFONT_METHODS 19

// Type-check and upcast macro
#define IS_FONT(o) CheckType<sxFont>(o, RFSXU_FONT);

namespace RFSX
{

class SX_IMPL_TYPE: public skExecutable
{
public:
	SX_IMPL_TYPE(CEGUI::Font* font);
	virtual ~SX_IMPL_TYPE();
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);
	virtual skExecutableIterator* createIterator(const skString& qualifier) { return createIterator(); }
	virtual skExecutableIterator* createIterator();
	virtual int executableType() const { return RFSXU_FONT; }

	CEGUI::Font* Font() { return m_Font; }
	CEGUI::PropertySet* PropertySet()	{ return m_Font; }

protected:
	// Add methods here, by name.
	SX_METHOD_DECL(getProperty);
	SX_METHOD_DECL(setProperty);
	SX_METHOD_DECL(getPropertyDefault);
	SX_METHOD_DECL(getPropertyHelp);
	SX_METHOD_DECL(isPropertyPresent);
	SX_METHOD_DECL(isPropertyDefault);

	SX_METHOD_DECL(setDefaultResourceGroup);
	SX_METHOD_DECL(getDefaultResourceGroup);

	SX_METHOD_DECL(load);
	SX_METHOD_DECL(isCodepointAvailable);
	SX_METHOD_DECL(setNativeResolution);
	SX_METHOD_DECL(notifyScreenResolution);

	SX_METHOD_DECL(getTextExtent);
	SX_METHOD_DECL(getLineSpacing);
	SX_METHOD_DECL(getFontHeight);

	SX_METHOD_DECL(getCharAtPixel);
	SX_METHOD_DECL(getFormattedLineCount);
	SX_METHOD_DECL(getFormattedTextExtent);
	SX_METHOD_DECL(drawText);

private:
	SX_IMPL_TYPE() {}

	CEGUI::Font*	m_Font;

	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_FONT_H_
