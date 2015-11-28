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

* $Id: skAlist.h,v 1.23 2003/04/24 10:19:43 simkin_cvs Exp $
*/

#ifndef skALIST_H
#define skALIST_H


#include "skGeneral.h"
  
class  CLASSEXPORT skAList;  

/**
 * This class provides an interator for the list, it is subclassed to provide type-safety
 */
class  CLASSEXPORT skAListIterator 
#ifdef __SYMBIAN32__
: public CBase
#endif
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
#ifdef __SYMBIAN32__
: public CBase
#endif
{   
 public:
  /**
   * does NOT destroy items, this is done in the derived class
   */
  inline virtual ~skAList();       
  /** 
   * deletes all items in the list, calling deleteItem on each one
   */
  IMPORT_C void clearAndDestroy();
  /**
   * removes all items from the list, without deleting
   */
  IMPORT_C void clear();
  /**
   * returns the number of items in the list
   */
  inline USize entries() const;
  /**
   * removes and deletes the nth item in the list, deleteItem is called to delete the item
   */
  IMPORT_C void deleteElt(USize  n);
  /**
   * performs some checks on the list
   */
  IMPORT_C void test() const;
  /**
   * instructs the list to grow to the given size (which must be greater than the current size), the current contents are copied across
   * @exception Symbian - a leaving function
   */
  IMPORT_C void growTo(USize size);
  /**
   * returns the capacity of the current list - i.e. the number of items that can be stored in the underlying array
   */
  inline USize getArraySize() const;
 protected:
  /**
   * default constructor - makes initial size zero
   */
  inline skAList();
  /**
   * insert *before* the given position
   * @exception Symbian - a leaving function
   */
  void insert(void *,USize index);
  /**
   * put at the start of the list
   * @exception Symbian - a leaving function
   */
  void prepend(void *);
  /**
   * put at the end of the list
   * @exception Symbian - a leaving function
   */
  inline void append(void *);
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
  inline void * operator[](USize  n) const;
  /**
   * returns the index of the given object, or -1 if not found
   */
  inline int index(const void *) const;
  /**
   * returns true if the given object is in the list
   */
  inline bool contains(const void *) const;
  /**
   * this method is overriden in the derived classes so that the correct destructor is called
   */
  virtual void deleteItem(void *)=0;
  friend class skAListIterator;
  /**
   * searches for an element, returns -1 if not found
   */
  inline int findElt(const void * i) const;
  /**
   * grows the array to the next size
   * @exception Symbian - a leaving function
   */
  IMPORT_C void grow();
  /**
   * Create the underlying array
   * @exception Symbian - a leaving function
   */
  IMPORT_C void createArray();
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
 private:
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
   * Destructor deletes all the contained objects by calling clearAndDestroy
   */
  virtual ~skTAList();
  /**
   * inserts the object *before* the given index
   * @exception Symbian - a leaving function
   */
  void insert(T *,USize index);
  /**
   * puts the object at the start of the list
   * @exception Symbian - a leaving function
   */
  void prepend(T *);
  /**
   * puts the object at the end of the list
   * @exception Symbian - a leaving function
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




