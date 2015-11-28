/**
   $Id: skRValueTable.h,v 1.11 2003/04/11 18:05:39 simkin_cvs Exp $
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

*/
#ifndef skRVALUETABLE_H
#define skRVALUETABLE_H

#include "skSHashTable.h"
#include "skRValue.h"

#ifdef INSTANTIATE_TEMPLATES
EXTERN_TEMPLATE template class CLASSEXPORT skTSHashTable<skRValue>;
EXTERN_TEMPLATE template class CLASSEXPORT skTSHashTableIterator<skRValue>;
#endif

/** This class is used to hold a hash table of skRValue's based on name. It is used to store local or global variables in the interpreter
 */
class CLASSEXPORT skRValueTable: public skTSHashTable<skRValue>
{
 public:
  /** This method constructs a blank RValueTable
   * @param size - the initial size of the table
   */
  inline skRValueTable(unsigned short  size)
    : skTSHashTable<skRValue>(size){
  }
  /** Default constructor */
  inline skRValueTable(){
  }
  /** Destructor */
  inline virtual ~skRValueTable(){
  }

};
class CLASSEXPORT skRValueTableIterator: public skTSHashTableIterator<skRValue>
{
public:
  inline skRValueTableIterator(const skRValueTable& table)
    : skTSHashTableIterator<skRValue>(table){
  }
};

#endif
