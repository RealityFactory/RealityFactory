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

#include "nl.h"
#include "mutex.h"

void nlMutexInit(NLmutex *mutex)
{
#ifdef NL_WIN_THREADS
    /* native Windows */
    InitializeCriticalSection(mutex);
#else
    /* POSIX */
    (void)pthread_mutex_init(mutex, NULL);
#endif
}

void nlMutexLock(NLmutex *mutex)
{
#ifdef NL_WIN_THREADS
    /* native Windows */
    EnterCriticalSection(mutex);
#else
    /* POSIX */
    (void)pthread_mutex_lock(mutex);
#endif
}

void nlMutexUnlock(NLmutex *mutex) {
#ifdef NL_WIN_THREADS
    /* native Windows */
    LeaveCriticalSection(mutex);
#else
    /* POSIX */
    (void)pthread_mutex_unlock(mutex);
#endif
}

void nlMutexDestroy(NLmutex *mutex) {
#ifdef NL_WIN_THREADS
    /* native Windows */
    DeleteCriticalSection(mutex);
#else
    /* POSIX */
    (void)pthread_mutex_destroy(mutex);
#endif
}
