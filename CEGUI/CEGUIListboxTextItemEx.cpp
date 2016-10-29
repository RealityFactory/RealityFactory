/************************************************************************************//**
 * @file CEGUIListboxTextItemEx.cpp
 * @brief Implementation of extended list box text items
 * @author Daniel Queteschiner
 * @date May 2011
 *//*
 * Copyright (c) 2011 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include <CEGUIFontManager.h>
#include <CEGUIFont.h>
#include <CEGUIWindow.h>
#include <CEGUIPropertyHelper.h>
#include <CEGUIImage.h>
#include "CEGUIListboxTextItemEx.h"

// Start of CEGUI namespace section
namespace CEGUI
{
/*************************************************************************
	Constants
*************************************************************************/
const colour	ListboxTextItemEx::DefaultUnselectedTextColour		= 0xFF000000;
const colour	ListboxTextItemEx::DefaultSelectedTextColour		= 0xFFFFFFFF;
const colour	ListboxTextItemEx::DefaultDisabledTextColour		= 0xFF808080;
const String	ListboxTextItemEx::UnselectedTextColourPropertyName( "NormalTextColour" );
const String	ListboxTextItemEx::SelectedTextColourPropertyName( "SelectedTextColour" );
const String	ListboxTextItemEx::DisabledTextColourPropertyName( "DisabledTextColour" );
const String	ListboxTextItemEx::SelectionBrushPropertyName( "SelectionBrush" );
const String	ListboxTextItemEx::SelectionColourPropertyName( "SelectionColour" );


/*************************************************************************
	Constructor
*************************************************************************/
ListboxTextItemEx::ListboxTextItemEx(const String& text, uint item_id, void* item_data, bool disabled, bool auto_delete) :
	ListboxItem(text, item_id, item_data, disabled, auto_delete),
	d_unselectedTextCols(DefaultUnselectedTextColour, DefaultUnselectedTextColour, DefaultUnselectedTextColour, DefaultUnselectedTextColour),
	d_selectedTextCols(DefaultSelectedTextColour, DefaultSelectedTextColour, DefaultSelectedTextColour, DefaultSelectedTextColour),
	d_disabledTextCols(DefaultDisabledTextColour, DefaultDisabledTextColour, DefaultDisabledTextColour, DefaultDisabledTextColour),
	d_font(0)
{
}


/*************************************************************************
	Return a pointer to the font being used by this ListboxTextItem
*************************************************************************/
Font* ListboxTextItemEx::getFont(void) const
{
	// prefer out own font
	if(d_font)
	{
		return d_font;
	}
	// try our owner window's font setting (may be null if owner uses no existant default font)
	else if(d_owner)
	{
		return d_owner->getFont();
	}
	// no owner, just use the default (which may be NULL anyway)
	else
	{
		return System::getSingleton().getDefaultFont();
	}
}


/*************************************************************************
	Set the font to be used by this ListboxTextItem
*************************************************************************/
void ListboxTextItemEx::setFont(const String& font_name)
{
	setFont(FontManager::getSingleton().getFont(font_name));
}


/*************************************************************************
	Return the rendered pixel size of this list box item.
*************************************************************************/
Size ListboxTextItemEx::getPixelSize(void) const
{
	Size tmp(0,0);

	Font* fnt = getFont();

	if(fnt)
	{
		tmp.d_height = PixelAligned(fnt->getLineSpacing());
		tmp.d_width = PixelAligned(fnt->getTextExtent(d_itemText));
	}

	return tmp;
}


/*************************************************************************
	Draw the list box item in its current state.
*************************************************************************/
void ListboxTextItemEx::draw(const Vector3& position, float alpha, const Rect& clipper) const
{

	ColourRect usTextCols(d_unselectedTextCols);
	ColourRect sTextCols(d_selectedTextCols);
	ColourRect dTextCols(d_disabledTextCols);
	const Image* sBrush = d_selectBrush;
	ColourRect sCols = d_selectCols;

	if(d_owner)
	{
		if(d_owner->isPropertyPresent(UnselectedTextColourPropertyName))
			usTextCols.setColours(PropertyHelper::stringToColour(d_owner->getProperty(UnselectedTextColourPropertyName)));

		if(d_owner->isPropertyPresent(SelectedTextColourPropertyName))
			sTextCols.setColours(PropertyHelper::stringToColour(d_owner->getProperty(SelectedTextColourPropertyName)));

		if(d_owner->isPropertyPresent(DisabledTextColourPropertyName))
			dTextCols.setColours(PropertyHelper::stringToColour(d_owner->getProperty(DisabledTextColourPropertyName)));

		if(d_owner->isPropertyPresent(SelectionBrushPropertyName))
			sBrush = PropertyHelper::stringToImage(d_owner->getProperty(SelectionBrushPropertyName));

		if(d_owner->isPropertyPresent(SelectionColourPropertyName))
			sCols = PropertyHelper::stringToColourRect(d_owner->getProperty(SelectionColourPropertyName));
	}


	if(d_selected && (sBrush != 0))
	{
		sBrush->draw(clipper, position.d_z, clipper, getModulateAlphaColourRect(sCols, alpha));
	}

	Font* fnt = getFont();

	if(fnt)
	{
		Vector3 finalPos(position);
		finalPos.d_y += PixelAligned((fnt->getLineSpacing() - fnt->getFontHeight()) * 0.5f);
		fnt->drawText(d_itemText, finalPos, clipper, getModulateAlphaColourRect(d_selected ? sTextCols : (d_disabled ? dTextCols : usTextCols), alpha));
	}
}

void ListboxTextItemEx::draw(RenderCache& cache,const Rect& targetRect, float zBase, float alpha, const Rect* clipper) const
{
	ColourRect usTextCols(d_unselectedTextCols);
	ColourRect sTextCols(d_selectedTextCols);
	ColourRect dTextCols(d_disabledTextCols);
	const Image* sBrush = d_selectBrush;
	ColourRect sCols = d_selectCols;

	if(d_owner)
	{
		if(d_owner->isPropertyPresent(UnselectedTextColourPropertyName))
			usTextCols.setColours(PropertyHelper::stringToColour(d_owner->getProperty(UnselectedTextColourPropertyName)));

		if(d_owner->isPropertyPresent(SelectedTextColourPropertyName))
			sTextCols.setColours(PropertyHelper::stringToColour(d_owner->getProperty(SelectedTextColourPropertyName)));

		if(d_owner->isPropertyPresent(DisabledTextColourPropertyName))
			dTextCols.setColours(PropertyHelper::stringToColour(d_owner->getProperty(DisabledTextColourPropertyName)));

		if(d_owner->isPropertyPresent(SelectionBrushPropertyName))
			sBrush = PropertyHelper::stringToImage(d_owner->getProperty(SelectionBrushPropertyName));

		if(d_owner->isPropertyPresent(SelectionColourPropertyName))
			sCols = PropertyHelper::stringToColourRect(d_owner->getProperty(SelectionColourPropertyName));
	}


	if(d_selected && sBrush != 0)
	{
		cache.cacheImage(*sBrush, targetRect, zBase, getModulateAlphaColourRect(sCols, alpha), clipper);
	}

	Font* font = getFont();

	if(font)
	{
		Rect finalPos(targetRect);
		finalPos.d_top += PixelAligned((font->getLineSpacing() - font->getFontHeight()) * 0.5f);
		cache.cacheText(d_itemText, font, LeftAligned, finalPos, zBase, getModulateAlphaColourRect(d_selected ? sTextCols : (d_disabled ? dTextCols : usTextCols), alpha), clipper);
	}
}


/*************************************************************************
	Set the colours used for text rendering.
*************************************************************************/
void ListboxTextItemEx::setUnselectedTextColours(colour top_left_colour, colour top_right_colour, colour bottom_left_colour, colour bottom_right_colour)
{
	d_unselectedTextCols.d_top_left		= top_left_colour;
	d_unselectedTextCols.d_top_right	= top_right_colour;
	d_unselectedTextCols.d_bottom_left	= bottom_left_colour;
	d_unselectedTextCols.d_bottom_right	= bottom_right_colour;
}


/*************************************************************************
	Set the colours used for text rendering.
*************************************************************************/
void ListboxTextItemEx::setSelectedTextColours(colour top_left_colour, colour top_right_colour, colour bottom_left_colour, colour bottom_right_colour)
{
	d_selectedTextCols.d_top_left		= top_left_colour;
	d_selectedTextCols.d_top_right		= top_right_colour;
	d_selectedTextCols.d_bottom_left	= bottom_left_colour;
	d_selectedTextCols.d_bottom_right	= bottom_right_colour;
}

/*************************************************************************
	Set the colours used for text rendering.
*************************************************************************/
void ListboxTextItemEx::setDisabledTextColours(colour top_left_colour, colour top_right_colour, colour bottom_left_colour, colour bottom_right_colour)
{
	d_disabledTextCols.d_top_left		= top_left_colour;
	d_disabledTextCols.d_top_right		= top_right_colour;
	d_disabledTextCols.d_bottom_left	= bottom_left_colour;
	d_disabledTextCols.d_bottom_right	= bottom_right_colour;
}

} // End of  CEGUI namespace section
