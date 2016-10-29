/************************************************************************************//**
 * @file CEGUIListboxTextItemEx.h
 * @brief Interface for extended list box text items
 * @author Daniel Queteschiner
 * @date May 2011
 *//*
 * Copyright (c) 2011 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _CEGUIListboxTextItemEx_H_
#define _CEGUIListboxTextItemEx_H_
#include "elements/CEGUIListboxItem.h"


// Start of CEGUI namespace section
namespace CEGUI
{
/**
 * @brief Class used for textual items in a list box.
 */
class ListboxTextItemEx : public ListboxItem
{
public:
	/*************************************************************************
		Constants
	*************************************************************************/
	static const colour	DefaultUnselectedTextColour;
	static const colour	DefaultSelectedTextColour;
	static const colour	DefaultDisabledTextColour;
	static const String	UnselectedTextColourPropertyName;
	static const String	SelectedTextColourPropertyName;
	static const String	DisabledTextColourPropertyName;
	static const String	SelectionBrushPropertyName;
	static const String	SelectionColourPropertyName;

	/*************************************************************************
		Construction and Destruction
	*************************************************************************/
	/**
	 * @brief base class constructor
	 */
	ListboxTextItemEx(const String& text, uint item_id = 0, void* item_data = 0, bool disabled = false, bool auto_delete = true);


	/**
	 * @brief base class destructor
	 */
	virtual ~ListboxTextItemEx(void) {}

	/*************************************************************************
		Accessor methods
	*************************************************************************/
	/**
	 * @brief Return a pointer to the font being used by this ListboxTextItem
	 *		This method will try a number of places to find a font to be used.
	 *		If no font can be found, NULL is returned.
	 * @return Font to be used for rendering this item
	 */
	Font*	getFont(void) const;

	/**
	 * @brief Return the current colours used to render normal unselected text.
	 * @return ColourRect object describing the currently set colours
	 */
	ColourRect getUnselectedTextColours() const		{return d_unselectedTextCols;}

	/**
	 * @brief Return the current colours used to render selected text.
	 * @return ColourRect object describing the currently set colours
	 */
	ColourRect getSelectedTextColours() const		{return d_selectedTextCols;}

	/**
	 * @brief Return the current colours used to render disabled text.
	 * @return ColourRect object describing the currently set colours
	 */
	ColourRect getDisabledTextColours() const		{return d_disabledTextCols;}

	/*************************************************************************
		Manipulator methods
	*************************************************************************/
	/**
	 * @brief Set the font to be used by this ListboxTextItem
	 * @param font Font to be used for rendering this item
	 * @return Nothing
	 */
	void	setFont(Font* font)		{d_font = font;}

	/**
	 * @brief Set the font to be used by this ListboxTextItem
	 * @param font_name String object containing the name of the Font to be used for rendering this item
	 * @return Nothing
	 */
	void	setFont(const String& font_name);

	/**
	 * @brief Set the colours used for text rendering.
	 * @param cols ColourRect object describing the colours to be used.
	 * @return Nothing.
	 */
	void	setUnselectedTextColours(const ColourRect& cols)			{d_unselectedTextCols = cols;}

	/**
	 * @brief Set the colours used for text rendering.
	 * @param top_left_colour Colour (as ARGB value) to be applied to the top-left corner of each text glyph rendered.
	 * @param top_right_colour Colour (as ARGB value) to be applied to the top-right corner of each text glyph rendered.
	 * @param bottom_left_colour Colour (as ARGB value) to be applied to the bottom-left corner of each text glyph rendered.
	 * @param bottom_right_colour Colour (as ARGB value) to be applied to the bottom-right corner of each text glyph rendered.
	 * @return Nothing.
	 */
	void	setUnselectedTextColours(colour top_left_colour, colour top_right_colour, colour bottom_left_colour, colour bottom_right_colour);

	/**
	 * @brief Set the colours used for text rendering.
	 * @param col colour value to be used when rendering.
	 * @return Nothing.
	 */
	void	setUnselectedTextColours(colour col)		{setUnselectedTextColours(col, col, col, col);}

	/**
	 * @brief Set the colours used for text rendering.
	 * @param cols ColourRect object describing the colours to be used.
	 * @return Nothing.
	 */
	void	setSelectedTextColours(const ColourRect& cols)			{d_selectedTextCols = cols;}

	/**
	 * @brief Set the colours used for text rendering.
	 * @param top_left_colour Colour (as ARGB value) to be applied to the top-left corner of each text glyph rendered.
	 * @param top_right_colour Colour (as ARGB value) to be applied to the top-right corner of each text glyph rendered.
	 * @param bottom_left_colour Colour (as ARGB value) to be applied to the bottom-left corner of each text glyph rendered.
	 * @param bottom_right_colour Colour (as ARGB value) to be applied to the bottom-right corner of each text glyph rendered.
	 * @return Nothing.
	 */
	void	setSelectedTextColours(colour top_left_colour, colour top_right_colour, colour bottom_left_colour, colour bottom_right_colour);

	/**
	 * @brief Set the colours used for text rendering.
	 * @param col colour value to be used when rendering.
	 * @return Nothing.
	 */
	void	setSelectedTextColours(colour col)		{setSelectedTextColours(col, col, col, col);}


	/**
	 * @brief Set the colours used for text rendering.
	 * @param cols ColourRect object describing the colours to be used.
	 * @return Nothing.
	 */
	void	setDisabledTextColours(const ColourRect& cols)			{d_disabledTextCols = cols;}

	/**
	 * @brief Set the colours used for text rendering.
	 * @param top_left_colour Colour (as ARGB value) to be applied to the top-left corner of each text glyph rendered.
	 * @param top_right_colour Colour (as ARGB value) to be applied to the top-right corner of each text glyph rendered.
	 * @param bottom_left_colour Colour (as ARGB value) to be applied to the bottom-left corner of each text glyph rendered.
	 * @param bottom_right_colour Colour (as ARGB value) to be applied to the bottom-right corner of each text glyph rendered.
	 * @return Nothing.
	 */
	void	setDisabledTextColours(colour top_left_colour, colour top_right_colour, colour bottom_left_colour, colour bottom_right_colour);

	/**
	 * @brief Set the colours used for text rendering.
	 * @param col colour value to be used when rendering.
	 * @return Nothing.
	 */
	void	setDisabledTextColours(colour col)		{setDisabledTextColours(col, col, col, col);}


	/*************************************************************************
		Required implementations of pure virtuals from the base class.
	*************************************************************************/
	Size getPixelSize(void) const;
	void draw(const Vector3& position, float alpha, const Rect& clipper) const;
	void draw(RenderCache& cache,const Rect& targetRect, float zBase,  float alpha, const Rect* clipper) const;

protected:
	/*************************************************************************
		Implementation Data
	*************************************************************************/
	ColourRect	d_unselectedTextCols;	//!< Colours used for rendering the text.
	ColourRect	d_selectedTextCols;		//!< Colours used for rendering the text.
	ColourRect	d_disabledTextCols;		//!< Colours used for rendering the text.
	Font*		d_font;					//!< Font used for rendering text.
};

} // End of  CEGUI namespace section


#endif	// end of guard _CEGUIListboxTextItemEX_h_
