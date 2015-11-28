/*
  Copyright 1996-2003
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

  $Id: skStringTokenizer.cpp,v 1.10 2003/04/27 16:32:02 simkin_cvs Exp $
*/
#include "skStringTokenizer.h"

#ifndef __SYMBIAN32__
//------------------------------------------
skStringTokenizer::skStringTokenizer(const skString& s, const skString& delim, bool returnDelims) 
//------------------------------------------
{
  init(s, delim, returnDelims);
}
//------------------------------------------
skStringTokenizer::skStringTokenizer(const skString& s, const skString& delim) 
//------------------------------------------
{
  init(s, delim, false);
}
//------------------------------------------
skStringTokenizer::skStringTokenizer(const skString& str) 
//------------------------------------------
{
  skString delim;
  delim=skSTR(" \t\n\r\f");
  init(str, delim, false);
}
#endif
//------------------------------------------
EXPORT_C void skStringTokenizer::init(const skString& s, const skString& delim, bool returnDelims)
//------------------------------------------
{
  m_CurrentPosition = 0;
  m_NewPosition = -1;
  m_DelimsChanged = false;
  m_Str = s;
  m_MaxPosition = m_Str.length();
  m_Delimiters = delim;
  m_RetDelims = returnDelims;
  setMaxDelimChar();
}
//------------------------------------------
int skStringTokenizer::skipDelimiters(int startPos) 
//------------------------------------------
{
  int position = startPos;
  while (!m_RetDelims && position < m_MaxPosition) {
    Char c = m_Str.at(position);
    if ((c > m_MaxDelimChar) || (m_Delimiters.indexOf(c) < 0))
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
  while (position < m_MaxPosition) {
    Char c = m_Str.at(position);
    if ((c <= m_MaxDelimChar) && (m_Delimiters.indexOf(c) >= 0))
      break;
    position++;
  }
  if (m_RetDelims && (startPos == position)) {
    Char c = m_Str.at(position);
    if ((c <= m_MaxDelimChar) && (m_Delimiters.indexOf(c) >= 0))
      position++;
  }
  return position;
}
//------------------------------------------
EXPORT_C bool skStringTokenizer::hasMoreTokens() 
//------------------------------------------
{
  /*
   * Temporary store this position and use it in the following
   * nextToken() method only if the m_Delimiters have'nt been changed in
   * that nextToken() invocation.
   */
  m_NewPosition = skipDelimiters(m_CurrentPosition);
  return (m_NewPosition < m_MaxPosition);
}
//------------------------------------------
EXPORT_C skString skStringTokenizer::nextToken() 
  //------------------------------------------
{
  /* 
   * If next position already computed in hasMoreElements() and
   * m_Delimiters have changed between the computation and this invocation,
   * then use the computed value.
   */
  
  m_CurrentPosition = (m_NewPosition >= 0 && !m_DelimsChanged) ?  
    m_NewPosition : skipDelimiters(m_CurrentPosition);
  
  /* Reset these anyway */
  m_DelimsChanged = false;
  m_NewPosition = -1;
  
  int start = m_CurrentPosition;
  m_CurrentPosition = scanToken(m_CurrentPosition);
  return m_Str.substr(start, m_CurrentPosition-start);
}
//------------------------------------------
EXPORT_C skString skStringTokenizer::nextToken(const skString& delim) 
  //------------------------------------------
{
  m_Delimiters = delim;
  
  /* delimiter string specified, so set the appropriate flag. */
  m_DelimsChanged = true;
  
  setMaxDelimChar();
  return nextToken();
}
//------------------------------------------
EXPORT_C int skStringTokenizer::countTokens() 
  //------------------------------------------
{
  int count = 0;
  int currpos = m_CurrentPosition;
  while (currpos < m_MaxPosition) {
    currpos = skipDelimiters(currpos);
    if (currpos >= m_MaxPosition)
      break;
    currpos = scanToken(currpos);
    count++;
  }
  return count;
}
//------------------------------------------
void skStringTokenizer::setMaxDelimChar() 
//------------------------------------------
{
  if (m_Delimiters.length() == 0) {
    m_MaxDelimChar = 0;
    return;
  }
  
  Char m = 0;
  for (unsigned int i = 0; i < m_Delimiters.length(); i++) {
    Char c = m_Delimiters.at(i);
    if (m < c)
      m = c;
  }
  m_MaxDelimChar = m;
}
