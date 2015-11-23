/*
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

* $Id: skAlist.h,v 1.12 2001/11/22 11:13:21 sdw Exp $
*/

#ifndef skALIST_H
#define skALIST_H


#include "skGeneral.h"
  
class  CLASSEXPORT skAList;  

/**
 * This class provides an interator for the list, it is subclassed to provide type-safety
 */
class  CLASSEXPORT skAListIterator 
{       
 public:
  /**
   * resets the iteration to the start of the list
   */
  void  reset();
  /**
   * destructor
   */
  virtual ~skAListIterator();
 protected:
  /**
   * the copy constructor is protected to prevent it being called
   */
  skAListIterator(const skAList&);
  /**
   * the assigment operator is protected to prevent it being called
   */
  skAListIterator& operator=(const skAListIterator&);
  /**
   * the iteration step - overrided in the sub-class for type-safety
   */
  void * operator()();
 private:
  /**
   * the index of the current item in the iteration
   */
  USize m_Current;
  /**
   * the list being iterated over
   */
  const skAList& m_AList;
};    
/**
 * Array-Based List of pointers with template sub-classes
*/

class  CLASSEXPORT skAList 
{   
 public:
  /**
   * default constructor - makes initial size zero, with growth increment of 4
   */
  skAList();
  /**
   * This constructor sets the initial size and growth increment for the list
   * @param initial_size - initial size to make the list
   * @param growth_increment - growth increment for list
   */
  skAList(USize initial_size,USize growth_increment);
  /**
   * does NOT destroy items, this is done in the derived class
   */
  virtual ~skAList();       
  /** 
   * deletes all items in the list, calling deleteItem on each one
   */
  void clearAndDestroy();
  /**
   * removes all items from the list, without deleting
   */
  void clear();
  /**
   * returns the number of items in the list
   */
  USize entries() const;
  /**
   * removes and deletes the nth item in the list, deleteItem is called to delete the item
   */
  void deleteElt(USize  n);
  /**
   * performs some checks on the list
   */
  void test() const;
  /**
   * instructs the list to grow to the given size (which must be greater than the current size), the current contents are copied across
   */
  void growTo(USize size);
 protected:
  /**
   * insert *before* the given position
   */
  void insert(void *,USize index);
  /**
   * put at the start of the list
   */
  void prepend(void *);
  /**
   * put at the end of the list
   */
  void append(void *);
  /**
   * remove an item, but *not* delete it
   */
  void remove(void *);
  /**
   * remove an item and delete it
   */
  void removeAndDestroy(void *); 
  /**
   * retrieves the nth item within the list, a BoundsException is thrown if the index is out of range
   */
  void * operator[](USize  n) const;
  /**
   * returns the index of the given object, or -1 if not found
   */
  int index(const void *) const;
  /**
   * returns true if the given object is in the list
   */
  bool contains(const void *) const;
  /**
   * this method is overriden in the derived classes so that the correct destructor is called
   */
  virtual void deleteItem(void *)=0;
  friend class skAListIterator;
  /**
   * searches for an element, returns -1 if not found
   */
  int findElt(const void * i) const;
  /**
   * grows the array to the next size
   */
  void grow();
  /**
   * An array of pointers to the objects. This can be null
   */
  void ** m_Array;
  /**
   * this gives the actual size of the array of pointers
   */
  USize m_ArraySize;
  /**
   * this gives the number of slots which are currently being used
   */
  USize m_Entries;
  /**
   * the amount to grow the list each time. 0 means "double in size"
   */
  USize m_GrowthIncrement;	

 private:
  /**
   * the default size for the list (0)
   */
  static const USize	DEFAULT_SIZE;
  /**
   * the default growth increment (4)
   */
  static const USize	DEFAULT_GROWTH_INCREMENT;
  /**
   * copy constructor is private to prevent copying at this level
   */
  skAList(const skAList&);
  /**
   * assignment operator is private to prevent copying at this level
   */
  skAList& operator=(const skAList&);
};    
/**
 * This template class is used to provide type-safety to the list
 */
template <class T> class CLASSEXPORT skTAList : public skAList
{
 public:
  /**
   * default constructor
   */
  skTAList();
  /**
   * Constructor specifying the size and growth increment for the list
   */
  skTAList(USize initial_size,USize growth_increment);
  /**
   * Destructor deletes all the contained objects by calling clearAndDestroy
   */
  virtual ~skTAList();
  /**
   * inserts the object *before* the given index
   */
  void insert(T *,USize index);
  /**
   * puts the object at the start of the list
   */
  void prepend(T *);
  /**
   * puts the object at the end of the list
   */
  void append(T *);
  /**
   * removes the given object from the list, but does not delete it
   */
  void remove(T *);
  /**
   * removes the given object from the list and deletes it
   */
  void removeAndDestroy(T *);
  /**
   * retrieves the object at the nth position in the list
   */
  T * operator[](USize  n) const;
  /**
   * returns the index of the item in the list, or -1 if not found
   */
  int index(const T *) const;
  /**
   * returns true if the list contains the given object
   */
  bool contains(const T *) const;
 protected:
  /**
   * this method calls delete on the object casted to the correct type, this ensures the destructor is called
   */
  void deleteItem(void *);
 private:
  /**
   * assignment operator - clears and destroys this list, and then does a deep copy
   */
  skTAList<T>& operator=(const skTAList<T>& l);
  /**
   * Copy constructor - does a deep copy
   */
  skTAList(const skTAList<T>&);
};
/**
 * this class provides a templated list iterator
 */
template <class T> class CLASSEXPORT skTAListIterator : public skAListIterator
{       
 public:
  /**
   * constructor - pass in a list
   */
  skTAListIterator(const skTAList<T>&);
  /**
   * this provides the next item in the iteration, it returns 0 if the iteration is finished
   */
  T * operator()();
};    

#include "skAlist.inl"

#endif




