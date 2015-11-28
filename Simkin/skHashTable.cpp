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

  $Id: skHashTable.cpp,v 1.13 2003/04/24 10:19:43 simkin_cvs Exp $
*/
#include "skHashTable.h"


//---------------------------------------------------
void skHashTable::createSlots() 
//---------------------------------------------------
{
  m_Slots=skARRAY_NEW(skHashEntryList,m_Size);
}
//---------------------------------------------------
EXPORT_C void skHashTable::clear() 
//---------------------------------------------------
{
  if (m_Slots){
    for (USize x=0;x<m_Size;x++)
      m_Slots[x].clearAndDestroy();
  }
  m_NumEntries=0;
}
//---------------------------------------------------
EXPORT_C void skHashTable::clearAndDestroyKeys() 
//---------------------------------------------------
{
  if (m_Slots){
    for (USize x=0;x<m_Size;x++){
      skHashEntryListIterator iter(m_Slots[x]);
      skHashEntry * pentry=0;
      while ((pentry=iter())!=0){
        deleteKey(pentry->m_Key);
      }
      m_Slots[x].clearAndDestroy();
    }
  }
  m_NumEntries=0;
}
//---------------------------------------------------
EXPORT_C void skHashTable::clearAndDestroyValues() 
//---------------------------------------------------
{
  if (m_Slots){
    for (USize x=0;x<m_Size;x++){
      skHashEntryListIterator iter(m_Slots[x]);
      skHashEntry * pentry=0;
      while ((pentry=iter())!=0){
	deleteValue(pentry->m_Value);
      }
      m_Slots[x].clearAndDestroy();
    }
  }
  m_NumEntries=0;
}
//---------------------------------------------------
EXPORT_C void skHashTable::clearAndDestroy() 
//---------------------------------------------------
{
  if (m_Slots){
    for (USize x=0;x<m_Size;x++){
      skHashEntryListIterator iter(m_Slots[x]);
      skHashEntry * pentry=0;
      while ((pentry=iter())!=0){
	deleteKey(pentry->m_Key);
	deleteValue(pentry->m_Value);
      }
      m_Slots[x].clearAndDestroy();
    }
  }
  m_NumEntries=0;
}
//---------------------------------------------------
EXPORT_C void skHashTable::insertKeyAndValue(void * key, void * value) 
//---------------------------------------------------
{
  if (m_Slots==0)
    createSlots();
  assert(key);
  assert(value);
  USize slot=0;
  skHashEntry * pentry=findEntry(key,slot);
  if (pentry){
    // replace existing key and value
    deleteKey(pentry->m_Key);
    deleteValue(pentry->m_Value);
    pentry->m_Key=key;
    pentry->m_Value=value;
  }else{
    // add key and value
    skHashEntry * entry=skNEW(skHashEntry(key,value));
    m_Slots[slot].append(entry);
    m_NumEntries++;
  }
}
//---------------------------------------------------
void skHashTable::del(void * key) 
//---------------------------------------------------
{
  assert(key);
  USize slot=0;
  skHashEntry * pentry=findEntry(key,slot);
  if (pentry){
    deleteKey(pentry->m_Key);
    deleteValue(pentry->m_Value);
    m_Slots[slot].removeAndDestroy(pentry);
    m_NumEntries--;
  }
}
//---------------------------------------------------
void skHashTable::delKeyNotValue(const void * key) 
//---------------------------------------------------
{
  assert(key);
  USize slot=0;
  skHashEntry * pentry=findEntry(key,slot);
  if (pentry){
    deleteKey(pentry->m_Key);
    m_Slots[slot].removeAndDestroy(pentry);
    m_NumEntries--;
  }
}
//---------------------------------------------------
void skHashTable::remove(const void * key) 
//---------------------------------------------------
{
  assert(key);
  USize slot=0;
  skHashEntry * pentry=findEntry(key,slot);
  if (pentry){
    m_Slots[slot].removeAndDestroy(pentry);
    m_NumEntries--;
  }
}
//---------------------------------------------------
int skHashTableIterator::operator()()
//---------------------------------------------------
{
  int ret=0;
  while (m_Slot<m_Table.m_Size){
    if (m_ListIterator==0){
      m_ListIterator=skNEW(skHashEntryListIterator(m_Table.m_Slots[m_Slot]));
    }
    skHashEntry * pentry=m_ListIterator->operator()();
    if (pentry==0){
      // no more in this slot, go to the next one
      m_Slot++;
      delete m_ListIterator;
      m_ListIterator=0;
    }else{
      m_Value=pentry->m_Value;
      m_Key=pentry->m_Key;
      ret=1;
      break;
    }
  }
  return ret;
}
#ifdef __SYMBIAN32__
//-------------------------------------------------------------------------
skHashTable::operator TCleanupItem()
//-------------------------------------------------------------------------
// used for leave support of local variable lists
{
  return TCleanupItem(Cleanup,this);
}
//-------------------------------------------------------------------------
void skHashTable::Cleanup(TAny * s)
//-------------------------------------------------------------------------
// called in case there is a leave
{
  ((skHashTable *)s)->clearAndDestroy();
}
#endif


