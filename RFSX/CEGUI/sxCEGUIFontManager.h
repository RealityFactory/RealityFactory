/************************************************************************************//**
 * @file sxCEGUIFontManager.h
 * @brief Class declaration for RFSX FontManager object
 * @author Daniel Queteschiner
 * @date November 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_FONTMANAGER_H_
#define _SX_FONTMANAGER_H_

#include "RFSX.h"

// sxFontManager class definition
/*
	Description:
		Wrapper for the CEGUI::FontManager class
		The FontManager is used to create, access, and destroy Font objects.

	Methods:
		sxFont createFont(skString type, skString name, skString fontname, skString resourcegroup="");
		sxFont createFont(skString filename, skString resourcegroup="");
		void destroyFont(skString name);
		void destroyFont(sxFont font);
		void destroyAllFonts();
		bool isFontPresent(skString name);
		sxFont getFont(skString name);
		void notifyScreenResolution(sxSize sz);
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxFontManager

#define SXFONTMANAGER_METHODS 6

namespace RFSX
{

class SX_IMPL_TYPE: public skExecutable
{
public:
	SX_IMPL_TYPE();
	virtual ~SX_IMPL_TYPE();
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);
	virtual skExecutableIterator* createIterator(const skString& qualifier) { return createIterator(); }
	virtual skExecutableIterator* createIterator();
	virtual int executableType() const { return RFSXU_FONTMANAGER; }

protected:
	SX_METHOD_DECL(createFont);
	SX_METHOD_DECL(destroyFont);
	SX_METHOD_DECL(destroyAllFonts);
	SX_METHOD_DECL(isFontPresent);
	SX_METHOD_DECL(getFont);
	SX_METHOD_DECL(notifyScreenResolution);

private:
	static sxMHT<SX_IMPL_TYPE> h_method;

};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_FONTMANAGER_H_
