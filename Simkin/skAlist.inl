/*
  Copyright 1996-2002
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

  $Id: skAlist.inl,v 1.18 2003/04/14 15:24:57 simkin_cvs Exp $
*/
#include "skBoundsException.h"

//-----------------------------------------------------------------
inline skAList::skAList()
//-----------------------------------------------------------------
  : m_Array(0),m_ArraySize(0),m_Entries(0)
{
}
//-----------------------------------------------------------------
inline skAList::~skAList()
//-----------------------------------------------------------------
{   
  if(m_Array)
    delete [] m_Array;
  //	can't delete items because virtual function DeleteItem is now lost
}
//-----------------------------------------------------------------
inline void * skAList::operator[](USize  n) const
//-----------------------------------------------------------------
{
  assert(n<m_Entries);
  if (n>=m_Entries){
#ifdef EXCEPTIONS_DEFINED
    throw skBoundsException(skSTR("Invalid index in []"),__FILE__,__LINE__);
#else
    ExitSystem();
#endif
  }
  return m_Array[n];
}
//-----------------------------------------------------------------
inline USize skAList::entries() const
//-----------------------------------------------------------------
{
  return m_Entries;
}	
//-----------------------------------------------------------------
inline USize skAList::getArraySize() const
//-----------------------------------------------------------------
{
  return m_ArraySize;
}
// Returns -1 if not found.
//-----------------------------------------------------------------
inline int skAList::findElt(const void * i) const
//-----------------------------------------------------------------
{
  assert(i);
  for (USize index=0;index<m_Entries;index++){
    if (m_Array[index]==i)
      return (int)index;
  }
  return -1;	
}
//-----------------------------------------------------------------
inline bool skAList::contains(const void * p) const
//-----------------------------------------------------------------
{
  assert(p);
  return (bool)(findElt(p) >= 0);
}
//-----------------------------------------------------------------
inline int skAList::index(const void * p) const
//-----------------------------------------------------------------
{
  assert(p);
  return (USize)findElt(p);
}
//-----------------------------------------------------------------
inline void skAList::append(void * i)
//-----------------------------------------------------------------
{
  assert(i);
  assert(contains(i)==false);
  if (m_Array==0)
    createArray();
  if (m_ArraySize==m_Entries)
    grow();
  m_Array[m_Entries]=i;
  m_Entries++;
}

//-----------------------------------------------------------------
inline void skAListIterator::reset()
//-----------------------------------------------------------------
{
  m_Current=0;
}
//-----------------------------------------------------------------
inline skAListIterator::~skAListIterator()
//-----------------------------------------------------------------
{
}
//-----------------------------------------------------------------
inline skAListIterator::skAListIterator(const skAList& l)
//-----------------------------------------------------------------
  : m_AList(l)
{
  m_Current=0;
}   
//-----------------------------------------------------------------
inline void *  skAListIterator::operator()()
//-----------------------------------------------------------------
{                                                    
  if (m_Current<m_AList.m_Entries){
    void* item=m_AList[m_Current];
    m_Current++;
    return item;
  }else
    return 0;
}
#define ALIST_PREFIX  template <class T> 

//-----------------------------------------------------------------
ALIST_PREFIX inline skTAList<T>::skTAList()
//-----------------------------------------------------------------
{
}
//-----------------------------------------------------------------
ALIST_PREFIX inline skTAList<T>::~skTAList()
//-----------------------------------------------------------------
{
  clearAndDestroy();
}
//-----------------------------------------------------------------
ALIST_PREFIX inline void skTAList<T>::deleteItem(void * p)
//-----------------------------------------------------------------
{
  delete (T*) p;
}
//-----------------------------------------------------------------
ALIST_PREFIX inline void skTAList<T>::insert(T * t,USize index)
//-----------------------------------------------------------------
{
  skAList::insert(t,index);
}
//-----------------------------------------------------------------
ALIST_PREFIX inline void skTAList<T>::prepend(T * t)
//-----------------------------------------------------------------
{
  skAList::prepend(t);
}
//-----------------------------------------------------------------
ALIST_PREFIX inline void skTAList<T>::append(T * t)
//-----------------------------------------------------------------
{
  skAList::append(t);
}
//-----------------------------------------------------------------
ALIST_PREFIX inline void skTAList<T>::remove(T * t)
//-----------------------------------------------------------------
{
  skAList::remove(t);
}
//-----------------------------------------------------------------
ALIST_PREFIX inline void skTAList<T>::removeAndDestroy(T * t)
//-----------------------------------------------------------------
{
  skAList::removeAndDestroy(t);
}
//-----------------------------------------------------------------
ALIST_PREFIX inline T * skTAList<T>::operator[](USize  n) const
//-----------------------------------------------------------------
{
  return (T *)skAList::operator [](n);
}
//-----------------------------------------------------------------
ALIST_PREFIX inline int skTAList<T>::index(const T * t) const
//-----------------------------------------------------------------
{
  return skAList::index(t);
}
//-----------------------------------------------------------------
ALIST_PREFIX inline bool skTAList<T>::contains(const T * t) const
//-----------------------------------------------------------------
{
  return skAList::contains(t);
}
//-----------------------------------------------------------------
ALIST_PREFIX inline skTAListIterator<T>::skTAListIterator(const skTAList<T>& l)
//-----------------------------------------------------------------
  : skAListIterator(l)
{
}
//-----------------------------------------------------------------
ALIST_PREFIX inline T * skTAListIterator<T>::operator()()
//-----------------------------------------------------------------
{
  return (T*)(skAListIterator::operator()());
}
//-----------------------------------------------------------------
ALIST_PREFIX inline skTAList<T>::skTAList(const skTAList<T>& l)
//-----------------------------------------------------------------
{
}
//-----------------------------------------------------------------
ALIST_PREFIX inline skTAList<T>& skTAList<T>::operator=(const skTAList<T>& l)
//-----------------------------------------------------------------
{
  return *this;
}
