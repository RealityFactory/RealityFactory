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

* $Id: skRValue.h,v 1.14 2001/11/22 11:13:21 sdw Exp $
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
{ 
 public:
  /**
   * An RValue has a value of one of these types: Executable object, String, integer, float or character
   */
  enum RType { T_Object,T_String,T_Int,T_Float,T_Char,T_Bool };
   
  /**
   * Default Constructor - creates a blank string object
   */ 
  skRValue();
  /**
   * Copy Constructor - copies type and value from the other object
   */ 
  skRValue(const skRValue&);
  /**
   * Constructor with an executable object
   * @param object - the object to be associated with the RValue
   * @param created - set this to true if you want the object to be deleted once all references have reached zero
   */
  skRValue(skiExecutable * obj,bool created=false);
  /**
   * Constructor with a string
   */
  skRValue(skString s);
  /**
   * Constructor with a character
   */
  skRValue(Char c);
  /**
   * Constructor with an integer
   */
  skRValue(int n);
  /**
   * Constructor with an unsigned integer
   */
  skRValue(unsigned int n);
  /**
   * Constructor with a float
   */
  skRValue(float f);
  /**
   * Constructor with a boolean value
   */
  skRValue(bool b);
  /**
   * Destructor - will delete an associated object if the created flag is set in constructor
   */
  ~skRValue();
  /**
   * Assigment operator - clears this value, and then assigns type and value
   */
  skRValue& operator=(const skRValue& v);
  /**
   * returns true if the other value is equal to this one. Comparison depends on the types held in each value.
   */
  bool operator==(const skRValue&);
  /**
   * returns a character representation of the value, converting if necessary
   */
  Char	charValue()  const;		
  /**
   * returns a boolean representation of the value, converting if necessary
   */
  bool	boolValue()  const;		
  /**
   * returns an integer representation of the value, converting if necessary
   */
  int intValue()  const;		
  /**
   * returns a float representation of the value, converting if necessary
   */
  float floatValue()  const;		
  /**
   * returns a string representation of the value, converting if necessary
   */
  skString str() const;
  /**
   * returns the object associated with this RValue, or 0 if there is none
   */
  skiExecutable * obj()  const;
  /**
   * returns the type of the value held in this RValue - one of the enumerated RType
   */
  RType	type()  const;
 private:	                         
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
    float m_Float;
    bool m_Bool;
  }m_Value;
};                              
#include "skRValue.inl"
#endif


