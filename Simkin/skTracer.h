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

* $Id: skTracer.h,v 1.11 2003/04/11 18:05:39 simkin_cvs Exp $
*/

#ifndef skTRACER_H
#define skTRACER_H

#include "skString.h"

/**
 * This class is used to provide output to a trace
 */
class CLASSEXPORT skTracer 
#ifdef __SYMBIAN32__
: public CBase
#endif
{
 public:
  /** 
   * this function outputs its arguments to stdout
   */
  IMPORT_C static void trace(const skString& s);
  /** 
   * this function outputs its arguments to stdout
   */
  IMPORT_C static void trace(const Char *  s);
};
#endif
