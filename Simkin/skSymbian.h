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

  $Id: skSymbian.h,v 1.3 2003/03/28 21:00:39 simkin_cvs Exp $
*/
#ifndef skSYMBIAN_H
#define skSYMBIAN_H
#include <string.h>

/**
 * These functions provide implementations not included in the Symbian stdlib
 */

int wcsncmp( const wchar_t *string1, const wchar_t *string2, size_t count );
wchar_t * wcschr(const wchar_t * src, wchar_t c);
wchar_t * wcsrchr(const wchar_t * src, wchar_t c);
wchar_t * wcsstr(const wchar_t * src, const wchar_t * s);
wchar_t * wcsncpy(wchar_t * a, const wchar_t * b, size_t size);
int iswspace( wchar_t c );
int iswalpha( wchar_t c );
int iswalnum( wchar_t c );
int iswdigit( wchar_t c );
#endif
