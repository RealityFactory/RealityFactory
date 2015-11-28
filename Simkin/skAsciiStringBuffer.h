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

* $Id: skAsciiStringBuffer.h,v 1.3 2003/04/19 17:56:15 simkin_cvs Exp $
*/


#ifndef skASCIISTRINGBUFFER_H
#define skASCIISTRINGBUFFER_H

#include "skAsciiString.h"

/**
 * This class provides an expandable buffer for textual characters. 
 */
class skAsciiStringBuffer 
{
 public:
  /** Constructs a blank buffer of the given capacity. The buffer's length will be zero.
   * @param capacity - the capacity of the buffer to create. This can be zero
   * @param growth_increment - the minimum amount to grow the buffer by if it is enlarged
   */
  IMPORT_C skAsciiStringBuffer(USize capacity,USize growth_increment=16);
#ifndef __SYMBIAN32__
  /** Constructs a buffer from the given string. The string's data will be copied. The buffer's initial capacity and length will be the length of the string.
   * \remarks not available in Symbian version
   * @param s - the string to copy
   * @param growth_increment - the minimum amount to grow the buffer by if it is enlarged
   */
  IMPORT_C skAsciiStringBuffer(const skAsciiString& s,USize growth_increment=16);
  /** Copy constructor - it takes a copy of the underlying buffer
   * \remarks not available in Symbian version
   * @param s - the other string buffer. A copy is taken of its underlying buffer
   */
  IMPORT_C skAsciiStringBuffer(const skAsciiStringBuffer& s);
#endif
  /**
   * Destroys the underlying character buffer
   */
  IMPORT_C virtual ~skAsciiStringBuffer();
  /** Assignment operator - it takes a copy of the underlying buffer
   * @param s - the other string buffer. A copy is taken of its underlying buffer
   */
  IMPORT_C skAsciiStringBuffer& operator=(const skAsciiStringBuffer& s);
  /**
   * Adds a character to the buffer, growing it if necessary.
   * @param ch - the character to add
   * @exception Symbian - a leaving function
   */
  IMPORT_C void append(char ch);
  /**
   * Adds a string to the buffer, growing it if necessary
   * @param s - the string to append to the end of the current buffer
   * @exception Symbian - a leaving function
   */
  IMPORT_C void append(const skAsciiString& s);
  /**
   * Adds a string to the buffer, growing it if necessary
   * @param s - the string to append to the end of the current buffer
   * @exception Symbian - a leaving function
   */
  IMPORT_C void append(const char * s);
  /**
   * Creates a new string that owns the underlying buffer. The current buffer is set to null.
   * @return a string which owns the current buffer
   * @exception Symbian - a leaving function
   */
  IMPORT_C skAsciiString toString() ;
  /**
   * Creates a new string that copies the underlying buffer. The current buffer remains the same
   * @return a string which copies the current buffer
   * @exception Symbian - a leaving function
   */
  IMPORT_C skAsciiString toStringCopy() const;
  /**
   * Returns the underlying buffer
   * @return a pointer to the underlying buffer
   */
  IMPORT_C operator const char * () const;
  /**
   * Returns the current length of the text in the buffer
   * @return the length of the text in the buffer
   */
  IMPORT_C USize length() const;
  /**
   * Returns the current capacity the buffer
   * @return the capacity of the buffer
   */
  IMPORT_C USize capacity() const;
 private:
  /**
   * This function ensures that there is enough room in the buffer for a string of the given length. It will grow the buffer, if necessary.
   * @param capacity - the new capacity required. If the buffer is already big enough, it will not be altered
   * @exception Symbian - a leaving function
   */
  void ensureCapacity(USize capacity);
  /** the underlying buffer of characters */
  char * m_Buffer;
  /** the length of the string held in the buffer (note this is not necessarily the same as the capacity of the buffer) */
  USize m_Length;
  /** the size of the buffer (note this is not necessarily the same as the length of the string held in the buffer) */
  USize m_Capacity;
  /** the minimum amount to grow the buffer by each time the capacity is increased */
  USize m_GrowthIncrement;
};

#endif
