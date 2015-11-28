/*
Copyright 1996-2002 Simon Whiteside

* $Id: skRValueArray.h,v 1.12 2003/04/11 18:05:39 simkin_cvs Exp $

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

*/
#ifndef skRVALUEARRAY_H
#define skRVALUEARRAY_H

#include "skRValue.h"
#include "skValist.h"

#ifdef INSTANTIATE_TEMPLATES
EXTERN_TEMPLATE template class CLASSEXPORT skTVAList<skRValue>;
#endif
/**
 * This class provides an array of RValues
 */
class CLASSEXPORT skRValueArray	: public skTVAList<skRValue>
{ 
 public:
  /**
   * Default Constructor: creates an array of zero size
   */
  inline skRValueArray() 
    {
    }
  virtual ~skRValueArray(){
  }
};
#endif



