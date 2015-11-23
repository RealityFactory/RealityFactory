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

* $Id: skString.h,v 1.17 2002/01/08 23:04:40 sdw Exp $
*/


#ifndef skSTRING_H
#define skSTRING_H

#include "skGeneral.h"

#ifdef STREAMS_ENABLED
#include <iostream.h>
#endif


#ifdef UNICODE_STRINGS
typedef wchar_t Char;
#define skSTR(x)			L ## x
#else
typedef char Char;
#define skSTR(x)			x
#endif

class P_String;

/**
 * This class encapsulates a null-terminated 8-bit c-string
 * It uses a pointer to a string plus a reference count to save copying when passed by value
*/
class CLASSEXPORT skString 
{
 public:
  /**
   * Default Constructor - constructs a blank string
   */
  skString();
  /**
   * Constructor - from a null-terminated c-string
   */
  skString(const Char *);
  /**
   * Copy Constructor
   */
  skString(const skString&);
  /**
   * Constructor - from a non null-terminated buffer
   * @param buffer - the buffer to be copied from
   * @param len - the length of the data to be copied
   */
  skString(const Char * buffer, USize len);
  /**
   * Constructor - a repeated list of characters
   * @param repeatChar - the character to be repeated
   * @param len - the number of times the character is repeated
   */
  skString(const Char repeatChar, USize len);
  /**
   * Destructor
   */
  virtual ~skString();
  /**
   * Assignment operator - increments reference count of underlying P_String object
   */
  skString& operator=(const skString&);
  /**
   * Assignment operator - dereferences the P_String object and makes a new one by copying the given buffer
   */
  skString& operator=(const Char *);
  /**
   * Comparison operator
   * @return true if the current string is alphabetically before the other string
   */
  bool operator<(const skString&) const;
  /**
   * Equality operator
   * @return true if the other string is the same as the current one
   */
  bool operator==(const skString&) const;
  /**
   * Equality operator 
   * @return true if the other c-string is the same as the current one
   */
  bool operator==(const Char *) const;
  /**
   * Inequality operator
   * @return true if the other string is different to the current one
   */
  bool operator!=(const skString&) const;
  /**
   * Inequality operator
   * @return true if the other c-string is different to the current one
   */
  bool operator!=(const Char *) const;
  /**
   * Conversion operator
   * @return a pointer to the buffer contained within the P_String object
   */
  operator const Char * () const;
  /**
   * Returns a hash value for this string
   */
  USize hash() const;
  /**
   * Returns a character within the string
   * @param index - the index of the character, starting at 0
   * @return the character, or 0 if the index does not lie within the length of the string
   */
  Char at(USize index) const;
  /**
   * Returns a substring of this string
   * @param start - the 0-based start of the substring
   * @param length - the length of the substring
   * @return a new String for the substring, or a blank string if the start and length do not lie within the current string
   */
  skString substr(USize start,USize length) const;
  /**
   * Returns the substring from the start up to the end of the current string
   * @param start - the 0-based start of the substring
   * @return a new String for the substring, or a blank string if the start does not lie within the current string
   */
  skString substr(USize start) const;
  /**
   * Addition operator
   * @return a String that contains this string followed by the other string
   */
  skString operator+(const skString&) const ;
  /**
   * Increment operator - dereferences the P_String object, and replaces it with the concatenation of this string and the other one
   * @return this string
   */
  skString& operator+=(const skString&);
  /**
   * Length of the string
   * @return the length of the buffer in the P_String object
   */
  USize length() const;
  /**
   * returns the index of the first occurrence of the given character within the string
   * @return an index, or -1 if not found
   */
  int indexOf(Char c) const;
  /**
   * Converts the string to an integer
   */
  int to() const;
  /**
   * Converts the string to a float
   */
  float toFloat() const;
  /**
   * Constructs a string from static string - the static string is *not* copied
   */
  static skString literal(const Char *);
  /**
   * Constructs a string from a signed integer
   */
  static skString from(int);
  /**
   * Constructs a string from an unsigned integer
   */
  static skString from(USize);
  /**
   * Constructs a string from a float
   */
  static skString from(float);
  /**
   * Constructs a string from a character
   */
  static skString from(Char ch);
  /**
   * Constructs a string from a buffer, which is *not* copied. The string will delete the buffer when the reference count reaches zero
   */
  static skString fromBuffer(Char * buffer);
  /**
   * returns a version of this string with leading blanks removed
   */
  skString ltrim() const;
 protected:
  /**
   * Constructor - internal taking a P_String and not copying it
   */
  skString(P_String *);
  /**
   * Constructor - internal, taking a buffer and not copying it
   */
  skString(const Char * s,int);
  /**
   * Assigns another buffer to this one, and copies it
   */
  void assign(const Char *,int len=0);				
  /**
   * Dereferences the P_String object, deleting it if the reference count is zero
   */
  void deRef();
  /**
   * The underlying string object
   */
  P_String * pimp;
};
/*
 * A hashKey function for HashTables
 */
inline USize hashKey(const skString * s)
{
    return s->hash();
}
#ifdef STREAMS_ENABLED
/*
 * A streaming operator to write a string to an output stream
 */
CLASSEXPORT ostream& operator<<(ostream&,const skString&s);
#endif
/*
 * Some helper macros for declaring literal strings, and references to literal strings
 */
#define skLITERAL(s) skString s_##s=skString::literal(skSTR(#s))
#define xskLITERAL(s) extern skString s_##s
//#define skSTR(s) skString(s)

inline float ATOF(const Char * c){
#ifdef UNICODE_STRINGS
  return (float) wcstod(c,0);
#else
  return (float) atof(c);
#endif
}
inline int ATOI(const Char * c){
#ifdef UNICODE_STRINGS
  return _wtoi(c);
#else
  return atoi(c);
#endif
}

#include "skString.inl"

#endif



