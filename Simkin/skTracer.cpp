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

* $Id: skTracer.cpp,v 1.17 2003/11/20 16:24:22 sdw Exp $
*/
#include "skTracer.h"
#ifdef STL_STREAMS
#include <iostream>
using namespace std;
#endif

//------------------------------------------
void skTracer::trace(const skString& s)
//------------------------------------------
{
#ifdef STREAMS_ENABLED
  cout << s;
  cout.flush();
#else
#ifdef UNICODE_STRINGS
#ifdef _WIN32_WCE
  NKDbgPrintfW((Char *)(const Char *)s);
#else
#ifdef __SYMBIAN32__
  // TO BE IMPLEMENTED
#else
  wprintf((const Char *)s);
#endif
#endif
#else
  printf((const Char *)s);
#endif
#endif
}
//------------------------------------------
void skTracer::trace(const Char * s)
//------------------------------------------
{
#ifdef STREAMS_ENABLED
  cout << s;
  cout.flush();
#else
#ifdef UNICODE_STRINGS
#ifdef _WIN32_WCE
  NKDbgPrintfW((Char *)(const Char *)s);
#else
#ifdef __SYMBIAN32__
  // TO BE IMPLEMENTED
#else
  wprintf((const Char *)s);
#endif
#endif
#else
  printf((const Char *)s);
#endif
#endif
}
