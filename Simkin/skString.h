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

  * $Id: skString.h,v 1.40 2003/11/20 16:24:22 sdw Exp $
  */


#ifndef skSTRING_H
#define skSTRING_H

#include "skGeneral.h"

#ifdef STREAMS_ENABLED

#ifdef STL_STREAMS
#include <ostream>
using namespace std;
#else
#include <iostream.h>
#endif

#endif
#include <string.h>

#ifdef UNICODE_STRINGS
typedef wchar_t Char;
#define skSTR(x) L ## x
#else
typedef char Char;
#define skSTR(x) x
#endif

class P_String;

/**
 * This class encapsulates a null-terminated 8-bit c-string
 * It uses a pointer to a string plus a reference count to save copying when passed by value
 */
class CLASSEXPORT skString 
#ifdef __SYMBIAN32__
: public CBase
#endif
{
 public:
  /**
   * Default Constructor - constructs a blank string
   */
  inline skString();
  /**
   * Copy Constructor
   */
  inline skString(const skString&);
#ifndef __SYMBIAN32__
  /**
   * Constructor - from a null-terminated c-string
   * \remarks not available in Symbian version
   */
  inline skString(const Char *);
  /**
   * Constructor - from a non null-terminated buffer
   * \remarks not available in Symbian version
   * @param buffer - the buffer to be copied from
   * @param len - the length of the data to be copied
   */
  inline skString(const Char * buffer, USize len);
  /**
   * Constructor - a repeated list of characters
   * \remarks not available in Symbian version
   * @param repeatChar - the character to be repeated
   * @param len - the number of times the character is repeated
   */
  skString(const Char repeatChar, USize len);
#endif
  /**
   * Destructor
   */
  virtual inline ~skString();
  /**
   * Assignment operator - increments reference count of underlying P_String object
   */
  inline skString& operator=(const skString&);
  /**
   * Assigns a character
   * @param c - the character to be assigned
   * @exception Symbian - a leaving function
   */
  IMPORT_C skString& operator=(Char c);
  /**
   * Assignment operator - dereferences the P_String object and makes a new one by copying the given buffer
   * @exception Symbian - a leaving function
   */
  IMPORT_C skString& operator=(const Char *);
  /**
   * Comparison operator
   * @return true if the current string is alphabetically before the other string
   */
  inline bool operator<(const skString&) const;
  /**
   * Equality operator
   * @return true if the other string is the same as the current one
   */
  inline bool operator==(const skString&) const;
  /**
   * Equality operator 
   * @return true if the other c-string is the same as the current one
   */
  inline bool operator==(const Char *) const;
#ifdef __SYMBIAN32__
  /**
   * Equality operator 
   * \remarks only available in Symbian version
   * @return true if the other descriptor is the same as the current one
   */
  inline bool operator==(const TDesC&) const;
#endif
  /**
   * Inequality operator
   * @return true if the other string is different to the current one
   */
  IMPORT_C bool operator!=(const skString&) const;
  /**
   * Inequality operator
   * @return true if the other c-string is different to the current one
   */
  IMPORT_C bool operator!=(const Char *) const;
#ifndef __SYMBIAN32__
  /**
   * Conversion operator
   * \remarks not available in Symbian version
   * @return a pointer to the buffer contained within the P_String object
   */
  inline operator const Char * () const;
#endif
#ifdef __SYMBIAN32__
  /**
   * Assignment operator from a Symbian descriptor
   * \remarks only available in Symbian version
   * @exception Symbian - a leaving function
   */
  inline skString& operator=(const TDesC&);
  /**
   * Accesses underlying buffer
   * \remarks only available in Symbian version
   * @return a pointer to the buffer contained within the P_String object
   */
  inline TPtrC ptr() const;
#else
  /**
   * Accesses underlying buffer
   * \remarks not available in Symbian version
   * @return a pointer to the buffer contained within the P_String object
   */
  inline const Char * ptr() const;
#endif
  /**
   * Accesses underlying buffer
   * @return a pointer to the buffer contained within the P_String object
   */
  inline const Char * c_str() const;
  /**
   * Case-insensitive equality
   * \remarks Symbian version performs a folding comparison
   * @return true if the other string is the same as the current one, ignoring case
   */
  IMPORT_C bool equalsIgnoreCase(const skString&) const;
  /**
   * Returns a hash value for this string
   */
  inline USize hash() const;
  /**
   * Returns a character within the string
   * @param index - the index of the character, starting at 0
   * @return the character, or 0 if the index does not lie within the length of the string
   */
  inline Char at(USize index) const;
  /**
   * Returns a substring of this string
   * @param start - the 0-based start of the substring
   * @param length - the length of the substring
   * @return a new String for the substring, or a blank string if the start and length do not lie within the current string
   * @exception Symbian - a leaving function
   */
  IMPORT_C skString substr(USize start,USize length) const;
  /**
   * Returns the substring from the start up to the end of the current string
   * @param start - the 0-based start of the substring
   * @return a new String for the substring, or a blank string if the start does not lie within the current string
   * @exception Symbian - a leaving function
   */
  IMPORT_C skString substr(USize start) const;
  /**
   * Addition operator
   * @return a String that contains this string followed by the other string
   * @exception Symbian - a leaving function
   */
  IMPORT_C skString operator+(const skString&) const ;
  /**
   * Increment operator - dereferences the P_String object, and replaces it with the concatenation of this string and the other one
   * @return this string
   * @exception Symbian - a leaving function
   */
  IMPORT_C skString& operator+=(const skString&);
  /**
   * Length of the string
   * @return the length of the buffer in the P_String object
   */
  inline USize length() const;
  /**
   * returns the index of the first occurrence of the given string within the string
   * @return an index, or -1 if not found
   */
  IMPORT_C int indexOf(const skString& s) const;
  /**
   * returns the index of the first occurrence of the given character within the string
   * @return an index, or -1 if not found
   */
  IMPORT_C int indexOf(Char c) const;
  /**
   * returns the index of the last occurrence of the given character within the string
   * @return an index, or -1 if not found
   */
  IMPORT_C int indexOfLast(Char c) const;
  /**
   * Converts the string to an integer
   */
  IMPORT_C int to() const;
#ifdef USE_FLOATING_POINT
  /**
   * Converts the string to a float
   */
  IMPORT_C float toFloat() const;
#endif
  /**
   * Constructs a string from static string - the static string is *not* copied
   * @exception Symbian - a leaving function
   */
  static IMPORT_C skString literal(const Char *);
  /**
   * Constructs a string from a signed integer
   * @exception Symbian - a leaving function
   */
  static IMPORT_C skString from(int);
  /**
   * Constructs a string from an unsigned integer
   * @exception Symbian - a leaving function
   */
  static IMPORT_C skString from(USize);
#ifdef USE_FLOATING_POINT
  /**
   * Constructs a string from a float
   * @exception Symbian - a leaving function
   */
  static IMPORT_C skString from(float);
#endif
  /**
   * Constructs a string from a character
   * @exception Symbian - a leaving function
   */
  static IMPORT_C skString from(Char ch);
  /**
   * Constructs a string from a buffer, which is *not* copied. The string will delete the buffer when the reference count reaches zero
   * @exception Symbian - a leaving function
   */
  static IMPORT_C skString fromBuffer(Char * buffer);
  /**
   * Constructs a string from the subset of a buffer, the contents are copied
   * @exception Symbian - a leaving function
   */
  static IMPORT_C skString copyFromBuffer(const Char * buffer,USize length);
  /**
   * @return returns a version of this string with leading blanks removed
   * @exception Symbian - a leaving function
   */
  skString IMPORT_C ltrim() const;
  /**
   * This method removes any initial blank lines from the given string. This is useful for trimming the beginning of methods in 
   * Simkin scripts.
   * @return returns the trimmed string
   * @exception Symbian - a leaving function
   */
  skString IMPORT_C removeInitialBlankLines() const;
#ifndef __SYMBIAN32__
  /**
   * Reads the contents of a file as a string
   * \remarks not available in Symbian version
   * @param filename the name of the file to read from
   */
  static skString IMPORT_C readFromFile(const skString& filename);
  /**
   * Writes the string to a file
   * \remarks not available in Symbian version
   * @param filename the name of the file to write to
   */
  void IMPORT_C writeToFile(const skString& filename);
#endif
  /**
   * Replaces a substring with another
   * @param old_substr the substring to be replaced
   * @param new_substr the new substring
   * @exception Symbian - a leaving function
   */
  IMPORT_C skString replace(const skString& old_substr,const skString& new_substr) const;
  /**
   * Replaces a substring with another
   * @param old_substr the substring to be replaced
   * @param new_substr the new substring
   * @exception Symbian - a leaving function
   */
  IMPORT_C skString replace(const Char * old_substr,const Char * new_substr) const;
  /**
   * Efficiently adds 2 strings
   * @exception Symbian - a leaving function
   */
  IMPORT_C static skString addStrings(const skString& s1,const skString & s2);
  /**
   * Efficiently adds 3 strings
   * @exception Symbian - a leaving function
   */
  IMPORT_C static skString addStrings(const skString & s1,const skString & s2,const skString & s3);
  /**
   * Efficiently adds 4 strings
   * @exception Symbian - a leaving function
   */
  IMPORT_C static skString addStrings(const skString & s1,const skString & s2,const skString & s3,const skString & s4);
  /**
   * Efficiently adds 5 strings
   * @exception Symbian - a leaving function
   */
  IMPORT_C static skString addStrings(const skString & s1,const skString & s2,const skString & s3,const skString & s4,const skString & s5);
  /**
   * Efficiently adds 6 strings
   * @exception Symbian - a leaving function
   */
  IMPORT_C static skString addStrings(const skString & s1,const skString & s2,const skString & s3,const skString & s4,const skString & s5,const skString & s6);
  /**
   * Efficiently adds 7 strings
   * @exception Symbian - a leaving function
   */
  IMPORT_C static skString addStrings(const skString & s1,const skString & s2,const skString & s3,const skString & s4,const skString & s5,const skString & s6,const skString & s7);
  /**
   * Efficiently adds 8 strings
   * @exception Symbian - a leaving function
   */
  IMPORT_C static skString addStrings(const skString & s1,const skString & s2,const skString & s3,const skString & s4,const skString & s5,const skString & s6,const skString & s7,const skString & s8);
#ifdef __SYMBIAN32__
  /**
   * Replaces a substring with another
   * \remarks only available in Symbian version
   * @param old_substr the substring to be replaced
   * @param new_substr the new substring
   * @exception Symbian - a leaving function
   */
  IMPORT_C skString replace(const TDesC& old_substr,const TDesC& new_substr) const;
  /**
   * Efficiently adds 2 strings
   * \remarks only available in Symbian version
   * @exception Symbian - a leaving function
   */
  IMPORT_C static skString addStrings(const TDesC& s1,const TDesC& s2);
  /**
   * Efficiently adds 3 strings
   * \remarks only available in Symbian version
   * @exception Symbian - a leaving function
   */
  IMPORT_C static skString addStrings(const TDesC& s1,const TDesC& s2,const TDesC& s3);
  /**
   * Efficiently adds 4 strings
   * @exception Symbian - a leaving function
   */
  IMPORT_C static skString addStrings(const TDesC& s1,const TDesC& s2,const TDesC& s3,const TDesC& s4);
  /**
   * Efficiently adds 5 strings
   * \remarks only available in Symbian version
   * @exception Symbian - a leaving function
   */
  IMPORT_C static skString addStrings(const TDesC& s1,const TDesC& s2,const TDesC& s3,const TDesC& s4,const TDesC& s5);
  /**
   * Efficiently adds 6 strings
   * \remarks only available in Symbian version
   * @exception Symbian - a leaving function
   */
  IMPORT_C static skString addStrings(const TDesC& s1,const TDesC& s2,const TDesC& s3,const TDesC& s4,const TDesC& s5,const TDesC& s6);
  /**
   * Efficiently adds 7 strings
   * \remarks only available in Symbian version
   * @exception Symbian - a leaving function
   */
  IMPORT_C static skString addStrings(const TDesC& s1,const TDesC& s2,const TDesC& s3,const TDesC& s4,const TDesC& s5,const TDesC& s6,const TDesC& s7);
  /**
   * Efficiently adds 8 strings
   * \remarks only available in Symbian version
   * @exception Symbian - a leaving function
   */
  IMPORT_C static skString addStrings(const TDesC& s1,const TDesC& s2,const TDesC& s3,const TDesC& s4,const TDesC& s5,const TDesC& s6,const TDesC& s7,const TDesC& s8);
#endif
  /**
   * This method replaces a character with a different one within this string. The underlying data *is* modified
   */
  IMPORT_C void replaceInPlace(Char old_char,Char new_char);
#ifdef __SYMBIAN32__
  /**
   * Conversion operator to Symbian TCleanupItem. This is provided to allow this object to be pushed by value onto the 
   * Symbian Cleanup Stack
   * \remarks only available in Symbian version
   */
  inline operator TCleanupItem();
  /**
   * Called via Symbian CleanupStack in the event of a leave. This calls the deRef method in the string.
   * \remarks only available in Symbian version
   */
  static inline void Cleanup(TAny * s);
#endif
 protected:
  /**
   * Constructor - internal taking a P_String and not copying it
   */
  skString(P_String *);
  /**
   * Assigns another buffer to this one, and copies it
   * @exception Symbian - a leaving function
   */
  IMPORT_C void assign(const Char *,int len=0);				
  /**
   * Dereferences the P_String object, deleting it if the reference count is zero
   */
  void deRef();
  /**
   * The underlying string object - this is null if the string is blank
   */
  P_String * pimp;
#ifdef __SYMBIAN32__
  friend IMPORT_C skString operator+(const TDesC& s1,const skString& s2);
#endif
  friend CLASSEXPORT IMPORT_C skString operator+(const Char * s1,const skString& s2);

};
/*
 * A hashKey function for HashTables
 */
//---------------------------------------------------
inline USize hashKey(const skString * s)
//---------------------------------------------------
{
  return s->hash();
}
/*
 * An operator to add strings
 */
#ifdef __SYMBIAN32__
EXPORT_C skString operator+(const TDesC& s1,const skString& s2);
#endif
CLASSEXPORT IMPORT_C skString operator+(const Char *& s1,const skString& s2);

#ifdef STREAMS_ENABLED
/*
 * A streaming operator to write a string to an output stream
 */
CLASSEXPORT ostream& operator<<(ostream& out,const skString& s);
#endif
/*
 * Some helper macros for declaring literal strings, and references to literal strings
 */
#ifdef __SYMBIAN32__
//#define skLITERAL(name) _LIT(s_##name,#name)
#define skLITERAL(name) const TLitC16<sizeof(L## #name )/2> s_##name={sizeof(L## #name)/2-1,L## #name}
//#define xskLITERAL(name) _LIT(s_##name,#name)
#define xskLITERAL(name) extern const TLitC16<sizeof(L## #name )/2> s_##name
//#define skNAMED_LITERAL(name,value) _LIT(s_##name,#value)
#define skNAMED_LITERAL(name,value) const TLitC16<sizeof(value )/2> s_##name={sizeof(value)/2-1,value}
//#define xskNAMED_LITERAL(name,value) _LIT(s_##name,#value)
#define xskNAMED_LITERAL(name,value) extern const TLitC16<sizeof(value)/2> s_##name
//#define skLITERAL_STRING(name,value) _LIT(s_##name,value)
#define skLITERAL_STRING(name,value) const TLitC16<sizeof(L## #value )/2> s_##name={sizeof(L## #value)/2-1,L## #value}
//#define xskLITERAL_STRING(name,value) _LIT(s_##name,value)
#define xskLITERAL_STRING(name,value) extern const TLitC16<sizeof(L##value)/2> s_##name
#else
#define skLITERAL(name) const Char * s_##name=skSTR(#name)
#define xskLITERAL(name) extern const Char * s_##name
#define skNAMED_LITERAL(name,value) const Char * s_##name=value
#define xskNAMED_LITERAL(name,value) extern const Char * s_##name
#define skLITERAL_STRING(name,value) const Char * s_##name=skSTR(value)
#define xskLITERAL_STRING(name,value) extern const Char * s_##name;
#endif


#include "skString.inl"

#endif



