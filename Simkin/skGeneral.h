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

  $Id: skGeneral.h,v 1.36 2004/01/30 21:22:18 sdw Exp $
*/
#ifndef skGENERAL_H
#define skGENERAL_H

#if defined(_MSC_VER)
#if (_MSC_VER<=1300)
#ifndef STL_STREAMS
// STL streams introduce STL versions of new and delete
//#define USE_DEBUG_NEW
#endif
#endif
#endif

#ifndef __SYMBIAN32__
#if defined(USE_DEBUG_NEW)
   #ifdef _DEBUG
      #define MYDEBUG_NEW   new( _NORMAL_BLOCK, __FILE__, __LINE__)
       // Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
       //allocations to be of _CLIENT_BLOCK type
   #else
      #define MYDEBUG_NEW
   #endif // _DEBUG
#endif
#endif


#ifdef __SYMBIAN32__
#include <E32BASE.H>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "skSymbian.h"
#include <eikenv.h>
#else
#include <stdlib.h>
#endif

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
#ifdef __SYMBIAN32__
#define CLASSEXPORT     
#else
// TODO: Dynamic linking declarations for other platforms?
 #define LIBIMPORT       
 #define LIBEXPORT       
 #define CLASSEXPORT
 #define EXTERN_TEMPLATE 
#endif
#endif

#ifndef __SYMBIAN32__
// adds code to instantiate various templates- not necessary on MetroWerks CodeWarrior?
#define INSTANTIATE_TEMPLATES
#define IMPORT_C
#define EXPORT_C
#endif

// define this to enable floating point support
#define USE_FLOATING_POINT 1

// define this to enable execution scripts via the parse tree, rather than the new smaller representation
#define EXECUTE_PARSENODES 1


#if defined(USE_DEBUG_NEW)
#ifndef __SYMBIAN32__
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#define _INC_MALLOC
#include <crtdbg.h>
#endif
#endif
#endif


#ifndef __SYMBIAN32__
#if defined(USE_DEBUG_NEW)
   #ifdef _DEBUG
   #undef new
   #define new MYDEBUG_NEW
   #endif
#endif
#endif

#ifdef __SYMBIAN32__
// Symbian does not support exceptions or streams, but does use unicode
#include <assert.h>
#if defined(_UNICODE)
#define UNICODE_STRINGS
#endif
/** this method exits the system on a fatal error */
inline void ExitSystem()
{
  User::Leave(KErrGeneral);
}
/** a macro wrapping new so that code for Symbian is the same - here call the leaving operator new */
#define skNEW(a) new (ELeave) a
#define skARRAY_NEW(type,size) new (ELeave) type[size]
// puts a pointer into the cleanup stack
#define SAVE_POINTER(p) CleanupDeletePushL((p))
#define SAVE_VARIABLE(p) CleanupStack::PushL((p))
// removes the pointer, but doesn't delete it - it is assumed the method does this if it exits normally
#define RELEASE_POINTER(p) CleanupStack::Pop(p)
// removes the variable - don't call destroy as the destructor is called if the method exits normally
#define RELEASE_VARIABLE(p) CleanupStack::Pop(&p)
#else
/** a macro wrapping new so that code for Symbian is the same */
#define skNEW(a) new a
#define skARRAY_NEW(type,size) new type[size]
#define SAVE_POINTER(p) 
#define RELEASE_POINTER(p) 
#define SAVE_VARIABLE(p)
#define RELEASE_VARIABLE(p) 
// Look out for Windows CE - which doesn't support streams or exceptions or assert!
#ifdef _WIN32_WCE
#include <windows.h>
#include <dbgapi.h>
#define assert ASSERT
#define UNICODE_STRINGS
/** this method exits the system on a fatal error */
inline void ExitSystem()
{
  exit(EXIT_FAILURE);
}
#else
#define STREAMS_ENABLED
#if (_MSC_VER>1300)
#define STL_STREAMS 1
#endif
#define EXCEPTIONS_DEFINED
#include <assert.h>
#endif
#endif
#endif
