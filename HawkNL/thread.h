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

#ifndef _NL_THREAD_H_
#define _NL_THREAD_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef void *(*NLThreadFunc)(void *data);

void nlThreadCreate(NLThreadFunc func, void *data);
void nlThreadYield(void);

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* _NL_THREAD_H_ */

