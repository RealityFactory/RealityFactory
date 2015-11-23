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

* $Id: skiExecutable.h,v 1.3 2001/11/22 11:13:21 sdw Exp $
*/

#ifndef skiEXECUTABLE_H
#define skiEXECUTABLE_H

#include "skString.h"

class CLASSEXPORT skRValueArray;
class CLASSEXPORT skRValue;
class CLASSEXPORT skExecutableIterator;


/*
 * this constant is for undefined type primitive objects
 */
const int UNDEFINED_TYPE=0;
/*
 * this constant is should be used for the type of any user-defined primitive objects
 */
const int START_USER_TYPES=10;

/**
 * This class gives an interface for the Interpreter to call into.
 * All its methods are pure virtuals so that it can be used in as many different class hierarchies as possible.
 */
class CLASSEXPORT skiExecutable
{ 
 public:
  /**
   * Destructor
   */
   virtual ~skiExecutable() {}
  
  /**
   * this method returns the type of the object. 
   * Define your own value if you want to check the type of your object at run-time
   */
  virtual int executableType() const=0;			
  /**
   * returns an integer equivalent of this object
   */
  virtual int intValue() const=0;
  /**
   * returns a boolean equivalent of this object
   */
  virtual bool boolValue() const=0;
  /**
   * returns a character equivalent of this object
   */
  virtual Char charValue() const=0;
  /**
   * returns a String equivalent of this object
   */
  virtual skString strValue() const=0; 
  /**
   * returns a float equivalent of this object
   */
  virtual float floatValue() const=0;
  /**
   * requests the object to set a field to the given value
   * @param field_name - the name of the field name to set
   * @param attribute - the attribute name to set (may be blank)
   * @param value - the value to be set
   * @return true if the field was changed, false if the field could not be set or found
   */
  virtual bool setValue(const skString& field_name,const skString& attribute,const skRValue& value)=0; 
  /**
   * requests the object to set an item in its collection to a certain value
   * @param array_index - the identifier of the item - this might be a string, integer or any other legal value
   * @param attribute - the attribute name to set (may be blank)
   * @param value - the value to be set
   * @return true if the field was changed, false if the field could not be set or found
   */
  virtual bool setValueAt(const skRValue& array_index,const skString& attribute,const skRValue& value)=0; 
  /**
   * requests the object to return a field's  value
   * @param field_name - the name of the field name to get
   * @param attribute - the attribute name to get (may be blank)
   * @param value - the value to receive the value of the field
   * @return true if the field was changed, false if the field could not be get or found
   */
  virtual bool getValue(const skString& field_name,const skString& attribute,skRValue& value)=0;
  /**
   * requests the object to return an object from its collection
   * @param array_index - the identifier of the item - this might be a string, integer or any other legal value
   * @param attribute - the attribute name to get (may be blank)
   * @param value - the value to receive the value of the field
   * @return true if the field was changed, false if the field could not be get or found
   */
  virtual bool getValueAt(const skRValue& array_index,const skString& attribute,skRValue& value)=0;
  /**
   * Requests that the object execute the given method
   * @param method_name - the name of the method to execute
   * @param arguments - an array of RValue objects, which are the arguments to the method
   * @param return_value - an object to receive the return value of the method
   * @param return true if the method could be executed, or false if the method is not supported
   */
  virtual bool method(const skString& method_name,skRValueArray& arguments,skRValue& return_value)=0;
  /**
   * This method compares this object with another object. 
   */
  virtual bool equals(const skiExecutable * other_object) const=0;
  /**
   * This method returns an executable iterator used in foreach statements
   * @param qualifier a value to qualify the iteration by
   * @return an skExecutableIterator object that can be used to iterate over the result of the qualifier
   */
  virtual skExecutableIterator * createIterator(const skString& qualifier)=0;
  /**
   * This method returns an executable iterator used in foreach statements
   * @return an skExecutableIterator object that can be used to iterate over the components of this container
   */
  virtual skExecutableIterator * createIterator()=0;
};

// Some help-defines for method, getValue and setValue

#define IS_METHOD(s,m)		(s==m)
#define IS_GETVALUE(s,v)	(s==v)
#define IS_SETVALUE(s,v)	(s==v)


#endif
