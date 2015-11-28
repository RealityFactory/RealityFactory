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

* $Id: skAttribute.h,v 1.12 2003/05/19 17:58:33 simkin_cvs Exp $
*/
#ifndef skATTRIBUTE_H
#define skATTRIBUTE_H

#include "skAlist.h"
#include "skString.h"
class CLASSEXPORT skOutputDestination;
/**
 * This class represents an attribute of an element in an XML document. The class forms part of the Simkin DOM class library.
 */
class CLASSEXPORT skAttribute 
#ifdef __SYMBIAN32__
: public CBase
#endif
{
 public:
  /** Constructs a new attribute with the given name and value
   * @param name - the name of the attribute
   * @param value - the value of the attribute
   */
  inline skAttribute(const skString& name,const skString& value)
    : m_Name(name),m_Value(value){
  }
  /** this returns the name of the attribute
   * @return the name of the attribute
   */
  inline skString getName() const {
    return m_Name;
  }
  /** this returns the value of the attribute
   * @return the value of the attribute
   */
  inline skString getValue() const {
    return m_Value;
  }
  /** this method sets the value of the attribute */
  inline void setValue(const skString& name){
    m_Value=name;
  }
  /**
   * This method returns a string representation of the attribute, in the form <name>="<value>"
   * @exception Symbian - a leaving function
   */
  IMPORT_C skString toString() const;
  /**
   * This method writes the arribute out to the given destination, in the form <name>="<value>"
   * @exception Symbian - a leaving function
   */
  IMPORT_C void write(skOutputDestination& out);
 private:
  skString m_Name;
  skString m_Value;
};

#ifdef INSTANTIATE_TEMPLATES
EXTERN_TEMPLATE template class CLASSEXPORT skTAList<skAttribute>;
#endif

/**
 * This class holds a list of attributes - it is used by the Element class.
 */
class CLASSEXPORT skAttributeList : public skTAList<skAttribute>
{
 public:
  /**
   * This sets an attribute in this list. The attribute is added, if not already present.
   * @param name - the name of the attribute. If one already exists with this name, its value is overwritten.
   * @param value - the value for the attribute
   * @exception Symbian - a leaving function
   */
  IMPORT_C void setAttribute(const skString& name,const skString& value);
  /**
   * This returns the value of an attribute. If the attribute does not exist, a blank string is returned.
   * @param name - the name of the attribute being accessed
   * @return the value associated with the name, or a blank string.
   */
  IMPORT_C skString getAttribute(const skString& name) const;
  /**
   * This returns the value of an attribute. If the attribute does not exist, the default value is returned.
   * @param name - the name of the attribute being accessed
   * @param default_value - the default value for the attribute
   * @return the value associated with the name, or a blank string.
   */
  IMPORT_C skString getAttribute(const skString& name,const skString& default_value) const;
  /**
   * This removes an attribute from the list. 
   * @param name - the name of the attribute being accessed
   * @return true if the attribute was removed, false if it was not in the list
   */
  IMPORT_C bool removeAttribute(const skString& name);
  /** this method finds a named attribute 
   * @return the attribute, or 0 if there was no attribute with that name
   */
  IMPORT_C skAttribute * findAttribute(const skString& name) const;
};
#endif
