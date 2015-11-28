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

  $Id: skSHashTable.inl,v 1.5 2003/04/24 10:19:43 simkin_cvs Exp $
*/

//---------------------------------------------------
inline skSHashTable::skSHashTable(USize size) 
//---------------------------------------------------
  : m_Slots(0),m_Size(size),m_NumEntries(0)
{
}
//---------------------------------------------------
inline skSHashTable::~skSHashTable() 
//---------------------------------------------------
{
  clear();
  if (m_Slots)
    delete [] m_Slots;
}
//---------------------------------------------------
inline skSHashTable::skSHashTable(const skSHashTable&)
//---------------------------------------------------
{
}
//---------------------------------------------------
inline skSHashTable::skSHashTable()
//---------------------------------------------------
{
}
//---------------------------------------------------
inline skSHashTable& skSHashTable::operator=(const skSHashTable&)
//---------------------------------------------------
{
  return *this;
}
//---------------------------------------------------
inline skSHashTableIterator::skSHashTableIterator(const skSHashTable& t)
//---------------------------------------------------
  : m_Table(t), m_Slot(0),m_ListIterator(0)
{
}
//---------------------------------------------------
inline skSHashTableIterator::~skSHashTableIterator()
//---------------------------------------------------
{
  delete m_ListIterator;
}
//---------------------------------------------------
inline skSHashEntry * skSHashTable::findEntry(const skString& key, USize & slot) const
//---------------------------------------------------
{
  skSHashEntry * pentry=0;
  if (m_Slots){
    slot=key.hash()%m_Size;
    skSHashEntryListIterator iter(m_Slots[slot]);
    while ((pentry=iter())!=0)
      if (pentry->m_Key==key)
	break;
  }
  return pentry;
}
// Version which doesn't return slot.
//---------------------------------------------------
inline skSHashEntry * skSHashTable::findEntry(const skString& key) const
//---------------------------------------------------
{
  skSHashEntry * pentry=0;
  if (m_Slots){
    skSHashEntryListIterator iter(m_Slots[key.hash()%m_Size]);
    while ((pentry=iter())!=0)
      if (pentry->m_Key==key)
	break;
  }
  return pentry;
}
//---------------------------------------------------
inline void * skSHashTable::value(const skString& key)  const
//---------------------------------------------------
{
  skSHashEntry * pentry=findEntry(key);
  if (pentry)
    return pentry->m_Value;
  else
    return 0;
}
//---------------------------------------------------
inline skSHashTableIterator& skSHashTableIterator::operator=(const skSHashTableIterator&)
//---------------------------------------------------
{
  return *this;
}
//---------------------------------------------------
inline skString skSHashTableIterator::key() const
//---------------------------------------------------
{
  return m_Key;
}
//---------------------------------------------------
inline void * skSHashTableIterator::value() const
//---------------------------------------------------
{
  return m_Value;
}
//---------------------------------------------------
inline USize skSHashTable::entries() const
//---------------------------------------------------
{
  return m_NumEntries;
}
#ifdef __SYMBIAN32__
//-------------------------------------------------------------------------
inline skSHashTable::operator TCleanupItem()
//-------------------------------------------------------------------------
// used for leave support of local variable lists
{
  return TCleanupItem(Cleanup,this);
}
//-------------------------------------------------------------------------
inline void skSHashTable::Cleanup(TAny * s)
//-------------------------------------------------------------------------
// called in case there is a leave, make sure the contents of the hashtable are destroyed
{
  ((skSHashTable *)s)->clearAndDestroy();
}
#endif

#define TTHS_PREFIX template<class TValue> 

//---------------------------------------------------
TTHS_PREFIX inline skTSHashTable<TValue>::skTSHashTable(USize  size)
//---------------------------------------------------
  : skSHashTable(size)
{
}
//---------------------------------------------------
TTHS_PREFIX inline skTSHashTable<TValue>::skTSHashTable()
//---------------------------------------------------
  : skSHashTable(DEFAULT_skSHashTable_SIZE)
{
}
//---------------------------------------------------
TTHS_PREFIX inline skTSHashTable<TValue>::~skTSHashTable()
//---------------------------------------------------
{
  clearAndDestroy();
}

//---------------------------------------------------
TTHS_PREFIX inline void skTSHashTable<TValue>::insertKeyAndValue(const skString& key, TValue * value)
//---------------------------------------------------
{
  skSHashTable::insertKeyAndValue(key,value);
}
//---------------------------------------------------
TTHS_PREFIX inline TValue * skTSHashTable<TValue>::value(const skString& key)
//---------------------------------------------------
{
  return (TValue*)skSHashTable::value(key);
}
//---------------------------------------------------
TTHS_PREFIX inline void skTSHashTable<TValue>::deleteValue(void * value)
//---------------------------------------------------
{
  delete (TValue *)value;
}
//---------------------------------------------------
TTHS_PREFIX inline skTSHashTableIterator<TValue>::skTSHashTableIterator(const skTSHashTable<TValue>& l)
//---------------------------------------------------
  : skSHashTableIterator(l)
{
}
//---------------------------------------------------
TTHS_PREFIX inline TValue * skTSHashTableIterator<TValue>::value() const
//---------------------------------------------------
{
  return (TValue*)skSHashTableIterator::value();
}










