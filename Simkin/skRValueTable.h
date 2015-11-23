/**
   $Id: skRValueTable.h,v 1.4 2001/11/22 11:13:21 sdw Exp $
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

*/
#ifndef skRVALUETABLE_H
#define skRVALUETABLE_H

#include "skHashTable.h"
#include "skRValue.h"

EXTERN_TEMPLATE template class CLASSEXPORT skTHashTable<skString,skRValue>;

/** This class is used to hold a hash table of skRValue's based on name. It is used to store local or global variables in the interpreter
 */
class CLASSEXPORT skRValueTable: public skTHashTable<skString,skRValue>
{
 public:
  /** This method constructs a blank RValueTable
   * @param size - the initial size of the table
   */
  skRValueTable(unsigned short  size)
    : skTHashTable<skString,skRValue>(size){
  }
  /** Default constructor */
  skRValueTable(){
  }
  /** Destructor */
  ~skRValueTable(){
  }

};

#endif
