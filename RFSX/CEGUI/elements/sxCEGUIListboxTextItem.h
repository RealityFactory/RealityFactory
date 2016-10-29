/************************************************************************************//**
 * @file sxCEGUIListboxTextItem.h
 * @brief Class declaration for RFSX ListboxTextItem object
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_LISTBOXTEXTITEM_H_
#define _SX_LISTBOXTEXTITEM_H_

#include "RFSX.h"
#include "sxCEGUIListboxItem.h"

// sxListboxTextItem class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		Font* getFont() const;
		ColourRect getTextColours() const;

		void setFont(string font_name);
		void setFont(Font* font);

		void setTextColours(colour top_left_colour, colour top_right_colour, colour bottom_left_colour, colour bottom_right_colour);
		void setTextColours(const ColourRect& cols);
		void setTextColours(colour col);
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxListboxTextItem

#define SXLISTBOXTEXTITEM_METHODS 4

namespace RFSX
{

class SX_IMPL_TYPE: public sxListboxItem
{
public:
	SX_IMPL_TYPE(CEGUI::ListboxTextItem* item) : sxListboxItem(item) {}
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);

	CEGUI::ListboxTextItem* ListboxTextItem() { return static_cast<CEGUI::ListboxTextItem*>(ListboxItem()); }

protected:
	SX_METHOD_DECL(getFont);
	SX_METHOD_DECL(getTextColours);
	SX_METHOD_DECL(setFont);
	SX_METHOD_DECL(setTextColours);

private:
	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_LISTBOXTEXTITEM_H_
