/*
  Copyright 1996-2001
  Simon Whiteside

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  $Id: skStringTokenizer.h,v 1.5 2001/11/22 11:13:21 sdw Exp $
*/
#ifndef skSTRINGTOKENIZER_H
#define skSTRINGTOKENIZER_H

#include "skString.h"


/**
 * The string tokenizer class allows an application to break a 
 * string into tokens. 
 */
class CLASSEXPORT skStringTokenizer {


 public:
  /**
   * Constructs a string tokenizer for the specified string. All  
   * characters in the delim argument are the delimiters 
   * for separating tokens. 
   *
   * @param   str            a string to be parsed.
   * @param   delim          the delimiters.
   * @param   returnDelims   flag indicating whether to return the delimiters
   *                         as tokens.
   */
  skStringTokenizer(skString str, skString delim, bool returnDelims);
  
  /**
   * Constructs a string tokenizer for the specified string. The 
   * characters in the delim argument are the delimiters 
   * for separating tokens. Delimiter characters themselves will not 
   * be treated as tokens.
   *
   * @param   str     a string to be parsed.
   * @param   delim   the delimiters.
   */
  skStringTokenizer(skString str, skString delim);
  /**
   * Constructs a string tokenizer for the specified string. The 
   * tokenizer uses the default delimiter set, which is 
   * "&nbsp;&#92;t&#92;n&#92;r&#92;f": the space character, 
   * the tab character, the newline character, the carriage-return character,
   * and the form-feed character. Delimiter characters themselves will 
   * not be treated as tokens.
   *
   * @param   str   a string to be parsed.
   */
  skStringTokenizer(skString str);

  /**
   * Tests if there are more tokens available from this tokenizer's string. 
   * If this method returns true, then a subsequent call to 
   * nextToken with no argument will successfully return a token.
   *
   * @return  true if and only if there is at least one token 
   *          in the string after the current position; false 
   *          otherwise.
   */
  bool hasMoreTokens();

  /**
   * Returns the next token from this string tokenizer.
   *
   * @return     the next token from this string tokenizer.
   */
  skString nextToken();
  /**
   * Returns the next token in this string tokenizer's string. First, 
   * the set of characters considered to be delimiters by this 
   * skStringTokenizer object is changed to be the characters in 
   * the string delim. Then the next token in the string
   * after the current position is returned. The current position is 
   * advanced beyond the recognized token.  The new delimiter set 
   * remains the default after this call. 
   *
   * @param      delim   the new delimiters.
   * @return     the next token, after switching to the new delimiter set.
   */
  skString nextToken(skString delim);
  /**
   * Calculates the number of times that this tokenizer's 
   * nextToken method can be called. The current position is not advanced.
   *
   * @return  the number of tokens remaining in the string using the current
   *          delimiter set.
   */
  int countTokens();
 private:
  int currentPosition;
  int newPosition;
  int maxPosition;
  skString str;
  skString delimiters;
  bool retDelims;
  bool delimsChanged;

  /**
   * maxDelimChar stores the value of the delimiter character with the
   * highest value. It is used to optimize the detection of delimiter
   * characters.
   */
  char maxDelimChar;
  
  /**
   * Set maxDelimChar to the highest char in the delimiter set.
   */
  void setMaxDelimChar();
  /**
   * Initialize the tokenizer
   */
  void init(skString str, skString delim, bool returnDelims);
  /**
   * Skips delimiters starting from the specified position. If retDelims
   * is false, returns the index of the first non-delimiter character at or
   * after startPos. If retDelims is true, startPos is returned.
   */
  int skipDelimiters(int startPos);
  /**
   * Skips ahead from startPos and returns the index of the next delimiter
   * character encountered, or maxPosition if no such delimiter is found.
   */
  int scanToken(int startPos);
};
#endif
