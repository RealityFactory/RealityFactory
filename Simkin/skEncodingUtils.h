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

  $Id: skEncodingUtils.h,v 1.2 2003/01/20 18:48:18 simkin_cvs Exp $
*/

#ifndef skENCODINGUTILS_H
#define skENCODINGUTILS_H

#include "skString.h"
#ifdef UNICODE_STRINGS
#include "skAsciiString.h"
#endif

class CLASSEXPORT skEncodingUtils
{
public:
  enum Encoding { ASCII, LE_UNICODE,BE_UNICODE  };
  static int            toLocalEncoding(char c);
  static int            toLocalEncoding(wchar_t c,Encoding encoding);
  static char           toAscii(wchar_t c,Encoding encoding);
  static wchar_t        toUnicode(char c,Encoding encoding);
#ifdef UNICODE_STRINGS
  static skAsciiString  toAscii(const skString& s);
  static skString       fromAscii(const skAsciiString& s);
#endif
};



#endif 
