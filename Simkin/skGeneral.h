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

  $Id: skGeneral.h,v 1.16 2001/11/22 11:13:21 sdw Exp $
*/
#ifndef skGENERAL_H
#define skGENERAL_H

// Look out for Windows CE - which doesn't support streams or exceptions or assert!

#ifndef _WIN32_WCE
 #define STREAMS_ENABLED
 #define EXCEPTIONS_DEFINED
 #include <assert.h>
#else
 #define assert(exp) ((void)0)
 #define UNICODE_STRINGS
#endif

#include <stdlib.h>

typedef unsigned int USize;

#ifndef max
#define max(a,b) (((a)>(b))?(a):(b))
#endif
#ifndef min
#define min(a,b) (((a)<(b))?(a):(b))
#endif

// Windows Specific Thread Control...
// We use this for skInterpreter::getInterpreter()
// What we should do is have a method whereby the curent interpreter is set into each executable
// object, so that it can be reused when that object spawns a new one.
// At present, this only occurs in skTreeNodeObject and skXMLElementObject
// TODO: Define thread control for other platforms!!!
#ifdef ENABLE_WINDOWS_THREAD_SUPPORT
#if defined(_MSC_VER) && (_MSC_VER >= 1200 )
#define THREAD __declspec(thread)
#else
#define THREAD
#endif
#else
#define THREAD
#endif

// define ENABLE_WINDOWS_DLL_SUPPORT when using a Windows DLL version of Simkin. 
// then define BUILDING_DLL when building the Simkin DLL

#ifdef ENABLE_WINDOWS_DLL_SUPPORT
 #if defined(_MSC_VER)
  #define LIBIMPORT       __declspec( dllimport )
  #define LIBEXPORT       __declspec( dllexport )
  #endif
 #if defined(BUILDING_DLL)
  #define CLASSEXPORT     LIBEXPORT
  #define EXTERN_TEMPLATE
 #else
  #define CLASSEXPORT     LIBIMPORT
  #define EXTERN_TEMPLATE extern
 #endif 
#else
// TODO: Dynamic linking declarations for other platforms?
 #define LIBIMPORT       
 #define LIBEXPORT       
 #define CLASSEXPORT
 #define EXTERN_TEMPLATE
#endif


#ifdef EXCEPTIONS_DEFINED
#  ifdef __AFX_H__  // this is intended to resolve clashes with MFC's THROW()
#  undef THROW
#  endif
#define THROW(x,c) throw x;
#else
#include <windows.h>
#define THROW(x,c) RaiseException(c,0,0,0)
#endif

#endif
