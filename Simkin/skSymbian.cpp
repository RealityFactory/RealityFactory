/* 
  Copyright 1996-2003
  Simon Whiteside

  Portions of this file are from the GNU C Library
  Copyright (C) 1995, 1996 Free Software Foundation, Inc.
  Contributed by Ulrich Drepper <drepper@gnu.ai.mit.edu>, 1995.

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

  $Id: skSymbian.cpp,v 1.4 2003/04/11 18:05:39 simkin_cvs Exp $
*/
#include <e32def.h>
#include <e32std.h>

GLDEF_C TInt E32Dll(TDllReason aReason)
{
  return(KErrNone);
}

#include "skSymbian.h"

/** Find the first occurrence of WC in WCS.  */
wchar_t * wcschr(const wchar_t * wcs, wchar_t wc)
{
  do
    if (*wcs == wc)
      return (wchar_t *) wcs;
  while (*wcs++ != L'\0');

  return NULL;
}
/** Find the last occurrence of WC in WCS.  */
wchar_t * wcsrchr(const wchar_t * wcs, wchar_t wc)
{
  const wchar_t *retval = NULL;

  do
    if (*wcs == wc)
      retval = wcs;
  while (*wcs++ != L'\0');
  return (wchar_t *) retval;
}
wchar_t * wcsstr(const wchar_t * haystack, const wchar_t * needle)
{
  register wchar_t b, c;

  if ((b = *needle) != L'\0'){
    haystack--;				/* possible ANSI violation */
    do
      if ((c = *++haystack) == L'\0')
	goto ret0;
    while (c != b);

    if (!(c = *++needle))
      goto foundneedle;
    ++needle;
    goto jin;

    for (;;){
      register wchar_t a;
      register const wchar_t *rhaystack, *rneedle;

      do{
	if (!(a = *++haystack))
	  goto ret0;
	if (a == b)
	  break;
	if ((a = *++haystack) == L'\0')
	  goto ret0;
      shloop:	      ;
      }
      while (a != b);

    jin:	  if (!(a = *++haystack))
      goto ret0;

      if (a != c)
	goto shloop;

      if (*(rhaystack = haystack-- + 1) == (a = *(rneedle = needle)))
	do{
	  if (a == L'\0')
	    goto foundneedle;
	  if (*++rhaystack != (a = *++needle))
	    break;
	  if (a == L'\0')
	    goto foundneedle;
	}
	while (*++rhaystack == (a = *++needle));

      needle = rneedle;		  /* took the register-poor approach */

      if (a == L'\0')
	break;
    }
  }
 foundneedle:
  return (wchar_t*) haystack;
 ret0:
  return NULL;
}
wchar_t * wcsncpy(wchar_t * dest, const wchar_t * src, size_t n)
{
  wchar_t c;
  wchar_t *const s = dest;

  --dest;

  if (n >= 4){
    size_t n4 = n >> 2;

    for (;;){
      c = *src++;
      *++dest = c;
      if (c == L'\0')
	break;
      c = *src++;
      *++dest = c;
      if (c == L'\0')
	break;
      c = *src++;
      *++dest = c;
      if (c == L'\0')
	break;
      c = *src++;
      *++dest = c;
      if (c == L'\0')
	break;
      if (--n4 == 0)
	goto last_chars;
    }
    n = n - (dest - s) - 1;
    if (n == 0)
      return s;
    goto zero_fill;
  }

 last_chars:
  n &= 3;
  if (n == 0)
    return s;

  do{
    c = *src++;
    *++dest = c;
    if (--n == 0)
      return s;
  }
  while (c != L'\0');

 zero_fill:
  do
    *++dest = L'\0';
  while (--n > 0);

  return s;
}
#include <ctype.h>
int iswspace( wchar_t c ){
  return isspace(c);
}
int iswalpha( wchar_t c ){
  return isalpha(c);
}
int iswalnum( wchar_t c ){
  return isalnum(c);
}
int iswdigit( wchar_t c ){
  return isdigit(c);
}
/* Compare no more than N characters of S1 and S2,
   returning less than, equal to or greater than zero
   if S1 is lexicographically less than, equal to or
   greater than S2.  */
int wcsncmp (const wchar_t *s1, const wchar_t *s2, size_t n)
{
  wchar_t c1 = L'\0';
  wchar_t c2 = L'\0';

  if (n >= 4)
    {
      size_t n4 = n >> 2;
      do
	{
	  c1 = (wchar_t) *s1++;
	  c2 = (wchar_t) *s2++;
	  if (c1 == L'\0' || c1 != c2)
	    return c1 - c2;
	  c1 = (wchar_t) *s1++;
	  c2 = (wchar_t) *s2++;
	  if (c1 == L'\0' || c1 != c2)
	    return c1 - c2;
	  c1 = (wchar_t) *s1++;
	  c2 = (wchar_t) *s2++;
	  if (c1 == L'\0' || c1 != c2)
	    return c1 - c2;
	  c1 = (wchar_t) *s1++;
	  c2 = (wchar_t) *s2++;
	  if (c1 == L'\0' || c1 != c2)
	    return c1 - c2;
	} while (--n4 > 0);
      n &= 3;
    }

  while (n > 0)
    {
      c1 = (wchar_t) *s1++;
      c2 = (wchar_t) *s2++;
      if (c1 == L'\0' || c1 != c2)
	return c1 - c2;
      n--;
    }

  return c1 - c2;
}
