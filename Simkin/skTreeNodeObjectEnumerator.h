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

  $Id: skTreeNodeObjectEnumerator.h,v 1.18 2003/04/19 13:22:24 simkin_cvs Exp $
*/
#ifndef TREENODEOBJECTENUMERATOR_H
#define TREENODEOBJECTENUMERATOR_H

#include "skExecutable.h"
#include "skExecutableIterator.h"
#include "skTreeNode.h"

class CLASSEXPORT skTreeNodeObject;

/**
 * This class enumerates the elements in an TreeNodeObject 
 * 
*/
class CLASSEXPORT skTreeNodeObjectEnumerator : public skExecutable, public skExecutableIterator{
 public:
  /**
   * This constructor is for an enumerator that gives *all* the child elements of this element
   */
  IMPORT_C skTreeNodeObjectEnumerator(skTreeNodeObject * obj,const skString& location);
  /**
   * This constructor is for an enumerator that gives the child elements of this element whose tag name matches that given
   */
  IMPORT_C skTreeNodeObjectEnumerator(skTreeNodeObject * obj,const skString& location,const skString& tag);
  /**
   * Destructor
   */
  virtual ~skTreeNodeObjectEnumerator();
  /**
   * This method exposes the following methods to Simkin scripts:
   * "next" - returns the next object in the enumeration - or null if there are no more
   * "reset" - resets the enumeration to the start
   * @param s method name
   * @param args arguments to the function
   * @param r return value
   * @param ctxt context object to receive errors
   * @exception a Symbian - a leaving function
   * @exception skParseException - if a syntax error is encountered while the script is running
   * @exception skRuntimeException - if an error occurs while the script is running
   */
  virtual bool method(const skString& s,skRValueArray& args,skRValue& r,skExecutableContext& ctxt);
  /**
   * This method implements the method in skExecutableIterator
   * @exception a Symbian - a leaving function
   */
  virtual bool next(skRValue&);
 private:
  void findNextNode();

  skString m_Tag;
  skTreeNodeListIterator m_Iter;
  skTreeNode * m_CurrentNode;
  skString m_Location;
  skTreeNodeObject * m_Object;
};
#endif
