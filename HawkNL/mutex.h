/*
  HawkNL mutex module
  Copyright (C) 2000-2002 Phil Frisbie, Jr. (phil@hawksoft.com)

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public
  License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place - Suite 330,
  Boston, MA  02111-1307, USA.

  Or go to http://www.gnu.org/copyleft/lgpl.html
*/

/* 
  This basic mutex code originally contributed by Ryan Haksi:
*/


#ifndef _NL_MUTEX_H_
#define _NL_MUTEX_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Windows systems */
#if defined WIN32 || defined WIN64

#ifdef _MSC_VER
  #pragma warning (disable:4201)
  #pragma warning (disable:4214)
  #pragma warning (disable:4115)
  #pragma warning (disable:4514)
#endif
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
#ifdef _MSC_VER
  #pragma warning (default:4201)
  #pragma warning (default:4214)
  #pragma warning (default:4115)
#endif
#endif

#ifdef NL_WIN_THREADS
typedef CRITICAL_SECTION NLmutex;
#else
#include <pthread.h>
typedef pthread_mutex_t NLmutex;
#endif

void nlMutexInit(NLmutex *mutex);
void nlMutexLock(NLmutex *mutex);
void nlMutexUnlock(NLmutex *mutex);
void nlMutexDestroy(NLmutex *mutex);

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* _NL_MUTEX_H_ */

