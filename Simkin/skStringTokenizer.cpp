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

  $Id: skStringTokenizer.cpp,v 1.3 2001/11/22 11:13:21 sdw Exp $
*/
#include "skStringTokenizer.h"

//------------------------------------------
void skStringTokenizer::setMaxDelimChar() 
//------------------------------------------
{
  if (delimiters.length() == 0) {
    maxDelimChar = 0;
    return;
  }
  
  char m = 0;
  for (unsigned int i = 0; i < delimiters.length(); i++) {
    char c = delimiters.at(i);
    if (m < c)
      m = c;
  }
  maxDelimChar = m;
}
//------------------------------------------
skStringTokenizer::skStringTokenizer(skString str, skString delim, bool returnDelims) 
//------------------------------------------
{
  init(str, delim, returnDelims);
}
//------------------------------------------
void skStringTokenizer::init(skString str, skString delim, bool returnDelims)
//------------------------------------------
{
  currentPosition = 0;
  newPosition = -1;
  delimsChanged = false;
  this->str = str;
  maxPosition = str.length();
  delimiters = delim;
  retDelims = returnDelims;
  setMaxDelimChar();
}
//------------------------------------------
skStringTokenizer::skStringTokenizer(skString str, skString delim) 
//------------------------------------------
{
  init(str, delim, false);
}
//------------------------------------------
skStringTokenizer::skStringTokenizer(skString str) 
//------------------------------------------
{
  init(str, " \t\n\r\f", false);
}
//------------------------------------------
int skStringTokenizer::skipDelimiters(int startPos) 
//------------------------------------------
{
  int position = startPos;
  while (!retDelims && position < maxPosition) {
    char c = str.at(position);
    if ((c > maxDelimChar) || (delimiters.indexOf(c) < 0))
      break;
    position++;
  }
  return position;
}
//------------------------------------------
int skStringTokenizer::scanToken(int startPos) 
//------------------------------------------
{
  int position = startPos;
  while (position < maxPosition) {
    char c = str.at(position);
    if ((c <= maxDelimChar) && (delimiters.indexOf(c) >= 0))
      break;
    position++;
  }
  if (retDelims && (startPos == position)) {
    char c = str.at(position);
    if ((c <= maxDelimChar) && (delimiters.indexOf(c) >= 0))
      position++;
  }
  return position;
}
//------------------------------------------
bool skStringTokenizer::hasMoreTokens() 
//------------------------------------------
{
  /*
   * Temporary store this position and use it in the following
   * nextToken() method only if the delimiters have'nt been changed in
   * that nextToken() invocation.
   */
  newPosition = skipDelimiters(currentPosition);
  return (newPosition < maxPosition);
}
//------------------------------------------
skString skStringTokenizer::nextToken() 
  //------------------------------------------
{
  /* 
   * If next position already computed in hasMoreElements() and
   * delimiters have changed between the computation and this invocation,
   * then use the computed value.
   */
  
  currentPosition = (newPosition >= 0 && !delimsChanged) ?  
    newPosition : skipDelimiters(currentPosition);
  
  /* Reset these anyway */
  delimsChanged = false;
  newPosition = -1;
  
  int start = currentPosition;
  currentPosition = scanToken(currentPosition);
  return str.substr(start, currentPosition-start);
}
//------------------------------------------
skString skStringTokenizer::nextToken(skString delim) 
  //------------------------------------------
{
  delimiters = delim;
  
  /* delimiter string specified, so set the appropriate flag. */
  delimsChanged = true;
  
  setMaxDelimChar();
  return nextToken();
}
//------------------------------------------
int skStringTokenizer::countTokens() 
  //------------------------------------------
{
  int count = 0;
  int currpos = currentPosition;
  while (currpos < maxPosition) {
    currpos = skipDelimiters(currpos);
    if (currpos >= maxPosition)
      break;
    currpos = scanToken(currpos);
    count++;
  }
  return count;
}
