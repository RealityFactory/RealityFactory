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

  * $Id: skValist.h,v 1.23 2003/04/14 15:24:57 simkin_cvs Exp $
  */

#ifndef skVALIST_H
#define skVALIST_H


#include "skGeneral.h"
#include "skBoundsException.h"
    

/**
 * This class gives a value and array-based list
 */
template <class T> class CLASSEXPORT skTVAList 
#ifdef __SYMBIAN32__
: public CBase
#endif
{         
 public:
  /**
   * Default Constructor - creates an empty list
   */
  skTVAList();
#ifndef __SYMBIAN32__
  /**
   * Copy Constructor - copies the contents of the other list into this one
   * \remarks not available in Symbian version 
   */
  skTVAList(const skTVAList<T>&);
#endif
  /**
   * Destructor
   */
  virtual ~skTVAList();                                 
  /**
   * Assignment operator - copies the contents of the other list
   * @exception a Symbian - a leaving function
   */
  skTVAList& operator=(const skTVAList<T>&);
  /**
   * Moves the contents of another list into this one. The other list will now be empty.
   */
  void moveFrom(skTVAList<T>&);
  /**
   * Clears this list
   */
  void  clear();                                        
  /**
   * Returns the number of entries in the list
   */
  USize entries() const;
  /**
   * deletes the nth item in the list
   * @exception skBoundsException is thrown if the index is invalid
   */
  void  deleteElt(USize  n);
  /**
   * Adds the given item to the start of the list
   * @exception a Symbian - a leaving function
   */
  void  prepend(const T &t);
  /**
   * Inserts the given item *before* the specified index
   * @exception skBoundsException is thrown if the index is invalid
   * @exception a Symbian - a leaving function
   */
  void  insert(const T &t,USize index);
  /**
   * adds the given item to the list
   * @exception a Symbian - a leaving function
   */
  void  append(const T &t);
  /**
   * removes the given item from the list
   */
  void  remove(const T &t);     
  /**
   * retrieves the nth item from the list
   * @exception skBoundsException is thrown if the index is invalid
   * @return a reference to the item
   */
  T& operator[](USize  n) const;
  /**
   * returns the index of the item within the list.
   * @return the index, or -1 if not found
   */
  int index(const T &t) const;
  /**
   * checks if the given item is in the list
   * @return true if the list contains the item, otherwise false
   */
  bool  contains(const T &t) const;
  /**
   * instructs the list to grow to the given size (which must be greater than the current size). The existing items are copied across
   * @exception a Symbian - a leaving function
   */
  void  growTo(USize new_size);
  /**
   * returns the capacity of the current list - i.e. the number of items that can be stored in the underlying array
   */
  USize getArraySize() const;
#ifdef __SYMBIAN32__
  /**
   * Conversion operator to Symbian TCleanupItem. This is provided to allow this object to be pushed by value onto the 
   * Symbian Cleanup Stack
   * \remarks only available in Symbian version
   */
  operator TCleanupItem();
  /**
   * Called via Symbian CleanupStack in the event of a leave. This calls the clear method in the object
   * \remarks only available in Symbian version
   */
  static void Cleanup(TAny * s);
#endif
 protected:
  /**
   * returns the index of the given item in the list, or -1 if not found
   */
  int   findElt(const T &t) const;
  /**
   * grows the array that represents the list by the growth increment
   * @exception a Symbian - a leaving function
   */
  void  grow();
  /**
   * Create the underlying array
   * @exception a Symbian - a leaving function
   */
  void createArray();
  /**
   * the array used to represent the list
   */
  T*  m_Array;  
  /**
   * the size of the array
   */
  USize m_ArraySize;
  /**
   * the number of items being used in the list
   */
  USize m_Entries;
};        

#ifdef __gnuc__
#define skTVALIST_PRE template <class T> 
#else
#define skTVALIST_PRE template <class T> 
#endif


//-----------------------------------------------------------------
skTVALIST_PRE inline skTVAList<T>::skTVAList()
//-----------------------------------------------------------------
  :m_Array(0),m_ArraySize(0),m_Entries(0)
{
}
#ifndef __SYMBIAN32__
//-----------------------------------------------------------------
skTVALIST_PRE inline skTVAList<T>::skTVAList(const skTVAList& l)
//-----------------------------------------------------------------
  : m_ArraySize(l.m_ArraySize),m_Entries(l.m_Entries)
{
  if(m_ArraySize){
    m_Array=skARRAY_NEW(T,m_ArraySize);
    for (USize x=0;x<m_Entries;x++)
      m_Array[x]=l.m_Array[x];
  }else
    m_Array = 0;
}
#endif
//-----------------------------------------------------------------
skTVALIST_PRE inline skTVAList<T>::~skTVAList()
//-----------------------------------------------------------------
{
  if(m_Array)
    delete [] m_Array;
}
//-----------------------------------------------------------------
skTVALIST_PRE inline void skTVAList<T>::createArray()
//-----------------------------------------------------------------
{
  if (m_ArraySize==0)
    m_ArraySize=1;
  if (m_ArraySize)
    m_Array=skARRAY_NEW(T,m_ArraySize);
}
//-----------------------------------------------------------------
skTVALIST_PRE inline void skTVAList<T>::clear()
//-----------------------------------------------------------------
{
  m_Entries=0;
  if(m_Array)
    delete [] m_Array;
  m_Array=0;
  m_ArraySize=0;
}
//-----------------------------------------------------------------
skTVALIST_PRE inline USize skTVAList<T>::entries() const
     //-----------------------------------------------------------------
{
  return m_Entries;
}
//-----------------------------------------------------------------
skTVALIST_PRE inline USize skTVAList<T>::getArraySize() const
//-----------------------------------------------------------------
{
  return m_ArraySize;
}
//-----------------------------------------------------------------
skTVALIST_PRE void skTVAList<T>::deleteElt(USize  n)
//-----------------------------------------------------------------
{
  assert(n<m_Entries);
  if (n>=m_Entries)
#ifdef EXCEPTIONS_DEFINED
    throw skBoundsException("Invalid index to DeleteElt",__FILE__,__LINE__);
#else
  ExitSystem();
#endif
  else{
    for (USize x=n;x<m_Entries-1;x++)
      m_Array[x]=m_Array[x+1];
    m_Entries--;
  }
}
//-----------------------------------------------------------------
skTVALIST_PRE void skTVAList<T>::insert(const T &t,USize index)
//-----------------------------------------------------------------
{
  assert(index<=m_Entries);
  if (index>m_Entries)
#ifdef EXCEPTIONS_DEFINED
    throw skBoundsException("Invalid index to Insert",__FILE__,__LINE__);
#else
  ExitSystem();
#endif
  if (m_Array==0)
    createArray();
  if (m_ArraySize==m_Entries)
    grow();
  if (index<m_Entries){
    for (USize x=m_Entries;x>index;x--)
      m_Array[x]=m_Array[x-1];
  }
  m_Array[index]=t;
  m_Entries++;
}
//-----------------------------------------------------------------
skTVALIST_PRE void skTVAList<T>::prepend(const T &t)
//-----------------------------------------------------------------
{
  if (m_Array==0)
    createArray();
  if (m_ArraySize==m_Entries)
    grow();
  m_Entries++;
  for (USize x=m_Entries-1;x>=1;x--)
    m_Array[x]=m_Array[x-1];
  m_Array[0]=t;
}
//-----------------------------------------------------------------
skTVALIST_PRE inline void skTVAList<T>::append(const T &t)
//-----------------------------------------------------------------
{
  if (m_Array==0)
    createArray();
  if (m_ArraySize==m_Entries)
    grow();
  m_Array[m_Entries]=t;
  m_Entries++;
}
//-----------------------------------------------------------------
skTVALIST_PRE void skTVAList<T>::remove(const T &t)
//-----------------------------------------------------------------
{
  int index = findElt(t);
  assert(index>=0 && "VAList does not contain item"!=0);
  if (index>=0){
    for (USize x=(USize)index;x<m_Entries-1;x++)
      m_Array[x]=m_Array[x+1];

    m_Entries--;
  }
}
//-----------------------------------------------------------------
skTVALIST_PRE inline T& skTVAList<T>::operator[](USize  n) const
//-----------------------------------------------------------------
{
  assert(n<m_Entries);
  if (n>=m_Entries)
#ifdef EXCEPTIONS_DEFINED
    throw skBoundsException(skSTR("Invalid index to []"),__FILE__,__LINE__);
#else
  ExitSystem();
#endif
  return m_Array[n];
}
//-----------------------------------------------------------------
skTVALIST_PRE inline int skTVAList<T>::index(const T &t) const
//-----------------------------------------------------------------
{
  return (USize)findElt(t);
}
//-----------------------------------------------------------------
skTVALIST_PRE inline bool skTVAList<T>::contains(const T &t) const
//-----------------------------------------------------------------
{
  return (bool)(findElt(t) >= 0);
}
//-----------------------------------------------------------------
skTVALIST_PRE void skTVAList<T>::moveFrom(skTVAList<T>& l)
//-----------------------------------------------------------------
{
  if (&l!=this){
    clear();
    // transfer the other list's contents across to us
    m_ArraySize=l.m_ArraySize;
    m_Entries=l.m_Entries;
    m_Array=l.m_Array;
    l.m_ArraySize=0;
    l.m_Entries=0;
    l.m_Array=0;
  }
}
//-----------------------------------------------------------------
skTVALIST_PRE skTVAList<T>& skTVAList<T>::operator=(const skTVAList<T>& l)
//-----------------------------------------------------------------
{
  if (&l!=this){
    clear();
    m_ArraySize=l.m_Entries;
    m_Entries=l.m_Entries;
    if(m_Entries){
      m_Array=skARRAY_NEW(T,m_Entries);
      for (USize x=0;x<m_Entries;x++)           
        m_Array[x]=l.m_Array[x];
    }else{
      m_Array=0;        // don't allocate until needed
      m_ArraySize=0;
    }
  }
  return *this;
}
// Returns -1 if not found.
//-----------------------------------------------------------------
skTVALIST_PRE inline int skTVAList<T>::findElt(const T &t) const
//-----------------------------------------------------------------
{
  for (USize index=0;index<m_Entries;index++){
    if (m_Array[index]==t)
      return (int)index;                // jump out
  }
  return -1;
}
//-----------------------------------------------------------------
skTVALIST_PRE void skTVAList<T>::grow()
//-----------------------------------------------------------------
{
  m_ArraySize *= 2;     // constant increase in size
  T *  new_array=skARRAY_NEW(T,m_ArraySize);
  if(m_Array){
    for (USize x=0;x<m_Entries;x++)             
      new_array[x]=m_Array[x];
    delete [] m_Array;
  }
  m_Array=new_array;
}
//-----------------------------------------------------------------
skTVALIST_PRE void skTVAList<T>::growTo(USize new_size)
//-----------------------------------------------------------------
{
  if (new_size>m_ArraySize){
    m_ArraySize=new_size;

    T *  new_array=skARRAY_NEW(T,m_ArraySize);
    if(m_Array){
      for (USize x=0;x<m_Entries;x++)           
        new_array[x]=m_Array[x];
      delete [] m_Array;
    }
    m_Array=new_array;
  }
}
#ifdef __SYMBIAN32__
//-------------------------------------------------------------------------
skTVALIST_PRE skTVAList<T>::operator TCleanupItem()
//-------------------------------------------------------------------------
// used for leave support of local variable lists
{
  return TCleanupItem(Cleanup,this);
}
//-------------------------------------------------------------------------
skTVALIST_PRE void skTVAList<T>::Cleanup(TAny * s)
//-------------------------------------------------------------------------
// called in case there is a leave
{
  ((skTVAList<T> *)s)->clear();
}
#endif

#endif
