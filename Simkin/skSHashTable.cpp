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

  $Id: skSHashTable.cpp,v 1.6 2003/07/01 12:06:05 sdw Exp $
*/
#include "skSHashTable.h"


//---------------------------------------------------
void skSHashTable::createSlots() 
//---------------------------------------------------
{
  m_Slots=skARRAY_NEW(skSHashEntryList,m_Size);
}
//---------------------------------------------------
EXPORT_C void skSHashTable::clear() 
//---------------------------------------------------
{
  if (m_Slots){
    for (USize x=0;x<m_Size;x++)
      m_Slots[x].clearAndDestroy();
  }
  m_NumEntries=0;
}
//---------------------------------------------------
EXPORT_C void skSHashTable::clearAndDestroyValues() 
//---------------------------------------------------
{
  if (m_Slots){
    for (USize x=0;x<m_Size;x++){
      skSHashEntryListIterator iter(m_Slots[x]);
      skSHashEntry * pentry=0;
      while ((pentry=iter())!=0){
        deleteValue(pentry->m_Value);
      }
      m_Slots[x].clearAndDestroy();
    }
  }
  m_NumEntries=0;
}
//---------------------------------------------------
EXPORT_C void skSHashTable::clearAndDestroy() 
//---------------------------------------------------
{
  if (m_Slots){
    for (USize x=0;x<m_Size;x++){
      skSHashEntryListIterator iter(m_Slots[x]);
      skSHashEntry * pentry=0;
      while ((pentry=iter())!=0){
        deleteValue(pentry->m_Value);
      }
      m_Slots[x].clearAndDestroy();
    }
  }
  m_NumEntries=0;
}
//---------------------------------------------------
EXPORT_C void skSHashTable::insertKeyAndValue(const skString& key, void * value) 
//---------------------------------------------------
{
  if (m_Slots==0)
    createSlots();
  assert(value);
  USize slot=0;
  skSHashEntry * pentry=findEntry(key,slot);
  if (pentry){
    // replace existing key and value
    deleteValue(pentry->m_Value);
    pentry->m_Key=key;
    pentry->m_Value=value;
  }else{
    // add key and value
    skSHashEntry * entry=skNEW(skSHashEntry(key,value));
    SAVE_POINTER(entry);
    m_Slots[slot].append(entry);
    RELEASE_POINTER(entry);
    m_NumEntries++;
  }
}
//---------------------------------------------------
EXPORT_C void skSHashTable::del(const skString& key) 
//---------------------------------------------------
{
  USize slot=0;
  skSHashEntry * pentry=findEntry(key,slot);
  if (pentry){
    deleteValue(pentry->m_Value);
    m_Slots[slot].removeAndDestroy(pentry);
    m_NumEntries--;
  }
}
//---------------------------------------------------
EXPORT_C void skSHashTable::remove(const skString& key) 
//---------------------------------------------------
{
  USize slot=0;
  skSHashEntry * pentry=findEntry(key,slot);
  if (pentry){
    m_Slots[slot].removeAndDestroy(pentry);
    m_NumEntries--;
  }
}
//---------------------------------------------------
EXPORT_C int skSHashTableIterator::operator()()
//---------------------------------------------------
{
  int ret=0;
  if (m_Table.m_NumEntries){
    while (m_Slot<m_Table.m_Size){
      if (m_ListIterator==0){
        m_ListIterator=skNEW(skSHashEntryListIterator(m_Table.m_Slots[m_Slot]));
      }
      skSHashEntry * pentry=m_ListIterator->operator()();
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
  }
  return ret;
}


