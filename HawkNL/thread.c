/*
  HawkNL thread module
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

#include <stdlib.h>
#include "nl.h"
#include "thread.h"

/* Windows systems */
#if defined WIN32 || defined WIN64

#ifdef _MSC_VER
#pragma warning (disable:4201)
#pragma warning (disable:4214)
#pragma warning (disable:4115)
#pragma warning (disable:4514)
#pragma warning (disable:4127)
#endif /* _MSC_VER */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#ifdef _MSC_VER
#pragma warning (default:4201)
#pragma warning (default:4214)
#pragma warning (default:4115)
#endif /* _MSC_VER */
#endif /* WIN32 */

#ifdef NL_WIN_THREADS
#include <process.h>

typedef struct {
  void *(*func) (void *);
  void *arg;
} ThreadParms;

static unsigned  __stdcall threadfunc(void *arg)
{
    void *(*func) (void *);
    void *args;

    func = ((ThreadParms *)arg)->func;
    args = ((ThreadParms *)arg)->arg;
    free(arg);
    (void)(*func)(args);

    return 0;
}

#else
/* POSIX systems */
#include <pthread.h>
#include <sched.h>
#endif

void nlThreadCreate(NLThreadFunc func, void *data)
{
  /* Windows threads */
#ifdef NL_WIN_THREADS
      HANDLE        h;
      ThreadParms   *p;

      p = (ThreadParms *)malloc(sizeof(ThreadParms));
      if(p == NULL)
      {
          return;
      }
      p->func = func;
      p->arg = data;
      h = (HANDLE)_beginthreadex(NULL, 0, threadfunc, p, 0, NULL);
      CloseHandle(h);

  /* POSIX systems */
#else
    pthread_attr_t  attr;
    pthread_t       tid;

    (void)pthread_attr_init(&attr);
    (void)pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    (void)pthread_create(&tid, &attr, func, data);

#endif
}

void nlThreadYield(void)
{
    /* Windows threads */
#ifdef NL_WIN_THREADS
      Sleep(0);

  /* POSIX systems */
#else
    (void)sched_yield();

#endif
}

