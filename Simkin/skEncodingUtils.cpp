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

  $Id: skEncodingUtils.cpp,v 1.2 2003/01/20 18:48:18 simkin_cvs Exp $
*/

#include "skEncodingUtils.h"

#ifdef UNICODE_STRINGS
//-----------------------------------------------------------------
skAsciiString skEncodingUtils::toAscii(const skString& s)
//-----------------------------------------------------------------
{
  skAsciiString a;
  if (s.length()){
    char * buffer=new char[s.length()+1];
    memset(buffer,0,s.length()+1);
    for (unsigned int i=0;i<s.length();i++){
      wchar_t c=s.at(i);
      buffer[i]=toAscii(c,LE_UNICODE);
    }
    a=skAsciiString::fromBuffer(buffer);
  }
  return a;
}
//-----------------------------------------------------------------
skString skEncodingUtils::fromAscii(const skAsciiString& s)
//-----------------------------------------------------------------
{
  skString a;
  if (s.length()){
    wchar_t * buffer=new wchar_t[s.length()+1];
    memset(buffer,0,sizeof(wchar_t)*(s.length()+1));
    for (unsigned int i=0;i<s.length();i++){
      char c=s.at(i);
      buffer[i]=toUnicode(c,LE_UNICODE);
    }
    a=skString::fromBuffer(buffer);
  }
  return a;
}
#endif
//---------------------------------------------------------------
char skEncodingUtils::toAscii(wchar_t c,Encoding encoding)
//---------------------------------------------------------------
{
  // crude conversions
  return (char )c;
}
//---------------------------------------------------------------
wchar_t skEncodingUtils::toUnicode(char c,Encoding encoding)
//---------------------------------------------------------------
{
  // crude conversions
  return (wchar_t )c;
}
//---------------------------------------------------------------
int skEncodingUtils::toLocalEncoding(char c)
//-----------------------------------------------------------------
{
  // nb assuming Unicode platform is LE Unicode
  int r=c;
  // crude conversion - add zero hi-byte
  return r;
}
//-----------------------------------------------------------------
int skEncodingUtils::toLocalEncoding(wchar_t c,Encoding encoding)
//-----------------------------------------------------------------
{
  int r=c;
#ifndef UNICODE_STRINGS
  // crude conversion - drop hi-byte
  r=c & 0xFF;
#endif
  return r;
}
