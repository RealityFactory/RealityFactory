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

  $Id: skSHashTable.h,v 1.6 2003/04/24 10:19:43 simkin_cvs Exp $
*/
#ifndef skSHASHTBL_H
#define skSHASHTBL_H


#include "skGeneral.h"
#include "skString.h"
#include "skAlist.h"

/**
 * this class represents a single entry in the hashtable
 */
class  CLASSEXPORT skSHashEntry 
{    
 public:
  /**
   * Constructor
   */
  skSHashEntry(const skString& k,void * v) 
    : m_Key(k), m_Value(v) {
  }
  /**
   * Equality comparison - tests pointer equivalence
   */
  int	operator==(const skSHashEntry& h) { 
    return this==&h; 
  }
  skString m_Key;
  void * m_Value;
};

typedef skTAList<skSHashEntry> skSHashEntryList;
typedef skTAListIterator<skSHashEntry> skSHashEntryListIterator;

/**
 * This class maps string keys to object pointer values.
 * Concrete classes are created by instantiating the template sub-class skSTHashTable
*/

class CLASSEXPORT skSHashTable 
#ifdef __SYMBIAN32__
: public CBase
#endif
{
 public:
  /**
   * this clears the entries from the table, but does *not* delete them
   */
  IMPORT_C void clear();
  /**
   * this clears the list and deletes the values
   */
  IMPORT_C void clearAndDestroyValues();
  /**
   * this clears the list and deletes both the values and the keys
   */
  IMPORT_C void clearAndDestroy();
  /**
   * this returns the number of entries in the table
   */
  inline USize entries() const;
  /**
   * destructor
   */
  virtual ~skSHashTable();
  /**
   * deletes both the key and the value associated with the given key
   */
  IMPORT_C void del(const skString& key);
  /**
   * removes the entry associated with the given key
   */
  IMPORT_C void remove(const skString& key);
#ifdef __SYMBIAN32__
  /**
   * Conversion operator to Symbian TCleanupItem. This is provided to allow this object to be pushed by value onto the 
   * Symbian Cleanup Stack
   * \remarks only available in Symbian version
   */
  inline operator TCleanupItem();
  /**
   * Called via Symbian CleanupStack in the event of a leave. This calls the clearAndDestroy method in the table
   * \remarks only available in Symbian version
   */
  inline static void Cleanup(TAny * s);
#endif
 protected:
  /**
   * Constructor - makes the table an initial size
   */
  skSHashTable(USize  size);
  /**
   * puts a new key and value into the table. If the key already exists, it is first deleted
   * @exception Symbian - a leaving function
   */
  IMPORT_C void insertKeyAndValue(const skString& key, void * value);
  /**
   * returns the value associated with the given key
   */
  void *  value(const skString& key) const;
  /**
   * this method is overriden by derived classes and deletes the given value object
   */
  virtual void deleteValue(void * value)=0;
 private:
  /**
   * default constructor
   */
  skSHashTable();
  /**
   * copy constructor is private to prevent copying
   */
  skSHashTable(const skSHashTable&);
  /**
   * assignment operator is private to prevent copying
   */
  skSHashTable& operator=(const skSHashTable&);
  /**
   * finds the entry containing the given key, and also returns the slot number
   */
  skSHashEntry * findEntry(const skString& key,USize & slot) const;
  /**
   * finds the entry containing the given key
   */
  skSHashEntry * findEntry(const skString& key) const;
  /**
   * creates the underlying slots
   * @exception Symbian - a leaving function
   */
  void createSlots();
  /**
   * an array of HashEntry lists
   */
  skSHashEntryList * m_Slots;
  /**
   * the number of lists in the m_Slots array
   */
  USize m_Size;
  /**
   * the number of entries in the entire table
   */
  USize m_NumEntries;
  friend class skSHashTableIterator;
};
/**
 * this class provides an iterator for the hashtable
 */
class  CLASSEXPORT skSHashTableIterator
#ifdef __SYMBIAN32__
: public CBase
#endif
{
 public:
  /**
   * destructor
   */
  virtual ~skSHashTableIterator();
  /**
   * this function returns 1 if a value and key are available
   * @exception Symbian - a leaving function
   */
  IMPORT_C int operator()();
  /**
   * this returns the current key in the iteration - it is overriden in derived classes to provide type-safety
   */
  inline skString key() const; 
 protected:
  /**
   * this contructor is protected and is overriden in derived classes
   */
  skSHashTableIterator(const skSHashTable&);
  /**
   * this returns the current value in the iteration - it is overriden in derived classes to provide type-safety
   */
  void * value() const;
 private:
  /**
   * the assignment operator is private to prevent copying
   */
  skSHashTableIterator& operator=(const skSHashTableIterator&);
  /**
   * a reference to the table being iterated over
   */
  const skSHashTable& 	m_Table;
  /**
   * the current slot number in the iteration
   */
  USize  m_Slot;
  /**
   * the current value in the iteration
   */
  void * m_Value;
  /**
   * the current key
   */
  skString m_Key;
  /**
   * the current iterator over the hash entry list at the current slot
   */
  skSHashEntryListIterator * m_ListIterator;
};
#define TTS_PREFIX template <class TValue> 

const int DEFAULT_skSHashTable_SIZE=3;
/**
 * This class overrides the base class to provide type-safety
 */
template <class TValue>  class CLASSEXPORT skTSHashTable: public skSHashTable
{
 public:
  /**
   * constructs the table with the given size
   */
  skTSHashTable(USize  size);
  /**
   * default constructor which constructs a table with 3 entries
   */
  skTSHashTable();
  /**
   * destructor - destroys the keys and values in the table
   */
  ~skTSHashTable();
  /**
   * this method adds the given key and value to the table. If the key is already in the table, the existing version is first deleted
   * @exception Symbian - a leaving function
   */
  void insertKeyAndValue(const skString& key, TValue * value);
  /**
   * returns the value associated with the given key (or 0)
   */
  TValue * value(const skString& key);
    
 protected:
  /**
   * deletes the given value (casting it appropriately)
   */
  void deleteValue(void * value);
};
/**
 * This class provides a type-safe iterator for the templated hash table
 */
template <class TValue>  class CLASSEXPORT skTSHashTableIterator: public skSHashTableIterator
{
 public:
  /**
   * constructs the iterator referring to the given table
   */
  skTSHashTableIterator(const skTSHashTable<TValue>&);
  /**
   * returns the current value
   */
  TValue * value() const;
};

#include "skSHashTable.inl"

#endif
