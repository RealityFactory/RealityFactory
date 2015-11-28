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

* $Id: skExecutableIterator.h,v 1.8 2003/04/10 16:20:15 simkin_cvs Exp $
*/

#ifndef skEXECUTABLEITERATOR_H
#define skEXECUTABLEITERATOR_H


#include "skGeneral.h"

class CLASSEXPORT skRValue;

/**
 * This class is used to iterate over a set of values whilst in the foreach statement
 */
class CLASSEXPORT skExecutableIterator 
{
 public:
  /** Destructor */
  virtual ~skExecutableIterator()=0;
  /** This method returns the next value in the enumeration. It is implemented in the actual base class
   * @param value - a value to receive the next item in the iteration (if present)
   * @return true if the iteration can continue, otherwise false to indicate it is finished and the value variable is underined.
   */
  virtual bool next(skRValue& value)=0;
};

#endif

