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

* $Id: skRValue.h,v 1.27 2003/04/16 21:48:06 simkin_cvs Exp $
*/
#ifndef skRVALUE_H
#define skRVALUE_H

#include "skString.h"

class CLASSEXPORT skiExecutable;
class CLASSEXPORT skObjectRef;

/**
 * This class is used to hold the value of a variable in Simkin. It can hold reference counts for objects and delete them when these reach zero.
 */
class CLASSEXPORT  skRValue	
#ifdef __SYMBIAN32__
: public CBase
#endif
{ 
 public:
  /**
   * An RValue has a value of one of these types: Executable object, String, integer, float or character
   */
  enum RType { T_Object,T_String,T_Int,
#ifdef USE_FLOATING_POINT
	       T_Float,
#endif
	       T_Char,T_Bool, NUM_RTYPES };
   
  /**
   * Default Constructor - creates a blank string object
   */ 
  inline skRValue();
  /**
   * Copy Constructor - copies type and value from the other object
   */ 
  IMPORT_C skRValue(const skRValue&);
#ifndef __SYMBIAN32__
  /**
   * Constructor with an executable object
   * \remarks only available in Symbian version
   * @param object - the object to be associated with the RValue
   * @param created - set this to true if you want the object to be deleted once all references have reached zero
   */
  skRValue(skiExecutable * object,bool created=false);
#endif
  /**
   * Assign an executable object
   * @param object - the object to be associated with the RValue
   * @param created - set this to true if you want the object to be deleted once all references have reached zero
   * @exception Symbian - a leaving function
   */
  inline void assignObject(skiExecutable * object,bool created=false);
  /**
   * Constructor with a string
   */
  inline skRValue(const skString& s);
  /**
   * Constructor with a character
   */
  inline skRValue(Char c);
  /**
   * Constructor with an integer
   */
  inline skRValue(int n);
  /**
   * Constructor with an unsigned integer
   */
  inline skRValue(unsigned int n);
#ifdef USE_FLOATING_POINT
  /**
   * Constructor with a float
   */
  inline skRValue(float f);
#endif
#ifndef __SYMBIAN32__
  /**
   * Constructor with a boolean value
   * \remarks not available in Symbian version
   */
  inline skRValue(bool b);
#endif
  /**
   * Destructor - will delete an associated object if the created flag is set in constructor
   */
  virtual inline ~skRValue();
  /**
   * Assigment operator - clears this value, and then assigns type and value
   */
  IMPORT_C skRValue& operator=(const skRValue& v);
  /**
   * Assigment operator - clears this value, and then assigns boolean value
   */
  IMPORT_C skRValue& operator=(bool value);
  /**
   * Assigment operator - clears this value, and then assigns string value
   */
  IMPORT_C skRValue& operator=(const skString& value);
  /**
   * Assigment operator - clears this value, and then assigns integer value
   */
  IMPORT_C skRValue& operator=(int value);
#ifdef USE_FLOATING_POINT
  /**
   * Assigment operator - clears this value, and then assigns float value
   */
  IMPORT_C skRValue& operator=(float value);
#endif
  /**
   * Assigment operator - clears this value, and then assigns character value
   */
  IMPORT_C skRValue& operator=(Char value);
  /**
   * returns true if the other value is equal to this one. Comparison depends on the types held in each value.
   */
  IMPORT_C bool operator==(const skRValue&);
  /**
   * returns a character representation of the value, converting if necessary
   */
  IMPORT_C Char	charValue()  const;		
  /**
   * returns a boolean representation of the value, converting if necessary
   */
  IMPORT_C bool	boolValue()  const;		
  /**
   * returns an integer representation of the value, converting if necessary
   */
  IMPORT_C int intValue()  const;		
#ifdef USE_FLOATING_POINT
  /**
   * returns a float representation of the value, converting if necessary
   */
  IMPORT_C float floatValue()  const;		
#endif
  /**
   * returns a string representation of the value, converting if necessary
   */
  IMPORT_C skString str() const;
  /**
   * returns the object associated with this RValue, or 0 if there is none
   */
  inline skiExecutable * obj()  const;
  /**
   * returns the type of the value held in this RValue - one of the enumerated RType
   */
  inline RType	type()  const;
#ifdef __SYMBIAN32__
  /**
   * Conversion operator to Symbian TCleanupItem. This is provided to allow this object to be pushed by value onto the 
   * Symbian Cleanup Stack
   * \remarks only available in Symbian version
   */
  inline operator TCleanupItem();
  /**
   * Called via Symbian CleanupStack in the event of a leave. This calls the deRef method in the object
   * \remarks only available in Symbian version
   */
  static inline void Cleanup(TAny * s);
#endif
 private:	
  /** delete underlying object, if necessary */                         
  void deRef();
  /**
   * The type of the value
   */
  unsigned char m_Type;
  /**
   * The string value
   */
  skString m_String;
  
  /**
   * A union of object reference or numeric type
   */
  union {
    skObjectRef * m_ObjectRef;	
    Char m_Char;
    int	m_Int;
#ifdef USE_FLOATING_POINT
    float m_Float;
#endif
    bool m_Bool;
  }m_Value;
};                              
#include "skRValue.inl"
#endif


