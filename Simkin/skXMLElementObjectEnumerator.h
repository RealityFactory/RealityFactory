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

  $Id: skXMLElementObjectEnumerator.h,v 1.17 2003/04/19 13:22:24 simkin_cvs Exp $
*/
#ifndef XMLELEMENTOBJECTENUMERATOR_H
#define XMLELEMENTOBJECTENUMERATOR_H

#include "skExecutable.h"
#include "skExecutableIterator.h"
#include <xercesc/dom/deprecated/DOM_Element.hpp>

class CLASSEXPORT skXMLElementObject;

/**
 * This class enumerates the element children of an skXMLElementObject
 */
class CLASSEXPORT skXMLElementObjectEnumerator : public skExecutable, public skExecutableIterator{
 public:
  /** Construct an enumerator for the given XML element
   * @param element - the element to enumerate the children of
   * @param add_if_not_present - value of the flag to be passed when skXMLElementObject's are created
   * @param location - the owning XML element name
   */
  skXMLElementObjectEnumerator(DOM_Element element,bool add_if_not_present,const skString& location);
  /** This contructs an skXMLElementObject enumerator over children with a particular tag name
   * @param element - the element to enumerate the children of
   * @param add_if_not_present - value of the flag to be passed when skXMLElementObject's are created
   * @param location - the owning XML element name
   * @param tag - the tag name to look for
   */
  skXMLElementObjectEnumerator(DOM_Element element,bool add_if_not_present,const skString& location,const skString& tag);
  /**
   * This method exposes the following methods to Simkin scripts:
   * "next" - returns the next element in the enumeration - or null if there are no more
   * "reset" - resets the enumeration to the start
   * @param s method name
   * @param args arguments to the function
   * @param r return value
   * @param ctxt context object to receive errors
   * @exception skParseException - if a syntax error is encountered while the script is running
   * @exception skRuntimeException - if an error occurs while the script is running
   */
  bool method(const skString& s,skRValueArray& args,skRValue& r,skExecutableContext& ctxt);
  /**
   * This method implements the method in skExecutableIterator
   */
  bool next(skRValue&);
 private:
  void findNextNode();

  DOM_Element m_Element;
  skString m_Location;
  bool m_AddIfNotPresent;
  int m_NodeNum;
  skString m_Tag;
};
#endif
