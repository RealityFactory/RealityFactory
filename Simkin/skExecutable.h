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

* $Id: skExecutable.h,v 1.22 2001/11/22 11:13:21 sdw Exp $
*/

#ifndef skEXECUTABLE_H
#define skEXECUTABLE_H

#include "skiExecutable.h"


/**
 * This class gives a default implementation for the skiExecutable interface.
 */
class CLASSEXPORT skExecutable : public skiExecutable
{ 
 public:
  /**
   * Default Constructor
   */
  skExecutable();
  /**
   * Destructor
   */
  virtual ~skExecutable();
  
  /**
   * this method returns the type of the object at this level this is UNDEFINED_TYPE.
   * Define your own value if you want to check the type of your object at run-time
   */
  virtual int executableType() const;			
  /**
   * returns an integer equivalent of this object
   */
  virtual int intValue() const;
  /**
   * returns a boolean equivalent of this object
   */
  virtual bool boolValue() const;
  /**
   * returns a character equivalent of this object
   */
  virtual Char charValue() const;
  /**
   * returns a String equivalent of this object
   */
  virtual skString strValue() const; 
  /**
   * returns a float equivalent of this object
   */
  virtual float floatValue() const;
  /**
   * requests the object to set a field to the given value
   * @param field_name - the name of the field name to set
   * @param attribute - the attribute name to set (may be blank)
   * @param value - the value to be set
   * @return true if the field was changed, false if the field could not be set or found
   */
  virtual bool setValue(const skString& field_name,const skString& attribute,const skRValue& value); 
  /**
   * requests the object to set an item in its collection to a certain value
   * @param array_index - the identifier of the item - this might be a string, integer or any other legal value
   * @param attribute - the attribute name to set (may be blank)
   * @param value - the value to be set
   * @return true if the field was changed, false if the field could not be set or found
   */
  virtual bool setValueAt(const skRValue& array_index,const skString& attribute,const skRValue& value); 
  /**
   * requests the object to return a field's  value
   * @param field_name - the name of the field name to get
   * @param attribute - the attribute name to get (may be blank)
   * @param value - the value to receive the value of the field
   * @return true if the field was changed, false if the field could not be get or found
   */
  virtual bool getValue(const skString& field_name,const skString& attribute,skRValue& value);
  /**
   * requests the object to return an object from its collection
   * @param array_index - the identifier of the item - this might be a string, integer or any other legal value
   * @param attribute - the attribute name to get (may be blank)
   * @param value - the value to receive the value of the field
   * @return true if the field was changed, false if the field could not be get or found
   */
  virtual bool getValueAt(const skRValue& array_index,const skString& attribute,skRValue& value);
  /**
   * Requests that the object execute the given method
   * @param method_name - the name of the method to execute
   * @param arguments - an array of RValue objects, which are the arguments to the method
   * @param return_value - an object to receive the return value of the method
   * @param return true if the method could be executed, or false if the method is not supported
   */
  virtual bool method(const skString& method_name,skRValueArray& arguments,skRValue& return_value);
  /**
   * This method compares this object with another object. This implementation checks object pointers.
   */
  virtual bool equals(const skiExecutable * other_object) const;
  /**
   * This method returns an executable iterator used in foreach statements
   * @param qualifier a value to qualify the iteration by
   * @return an skExecutableIterator object that can be used to iterate over the result of the qualifier
   */
  virtual skExecutableIterator * createIterator(const skString& qualifier);
  /**
   * This method returns an executable iterator used in foreach statements
   * @return an skExecutableIterator object that can be used to iterate over the components of this container
   */
  virtual skExecutableIterator * createIterator();
 private:
  /**
   * Executables can't be copied
   */
  skExecutable(const skExecutable& other);
  /**
   * Executables can't be copied
   */
  skExecutable& operator=(const skExecutable& other);
};



#endif
